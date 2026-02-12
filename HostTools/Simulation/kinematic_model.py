#!/usr/bin/env python3
"""Kinematic model for a differential-drive robot."""

from __future__ import annotations

import math
from simulation_common import Pose, KinematicStatus, WheelSpeedSetPoint, wrap_to_pi, WHEELS_DISTANCE, WHEELS_RADIUS

class DifferentialDriveKinematics:
    def __init__(self, pose: Pose) -> None:
        self._pose = Pose(pose.x, pose.y, pose.theta)
        self._x_dot = 0.0
        self._y_dot = 0.0
        self._theta_dot = 0.0

        self.base_slip = 0.01
        self.slip_gain = 0.25
        self.slip_accel_ref = 20.0
        self.slip_accel_threshold = 5.0
        self._prev_left = 0.0
        self._prev_right = 0.0

    def get_status(self) -> KinematicStatus:
        return KinematicStatus(
            x=self._pose.x,
            y=self._pose.y,
            theta=self._pose.theta,
            x_dot=self._x_dot,
            y_dot=self._y_dot,
            theta_dot=self._theta_dot
        )

    def step(self, wheel_speed_setpoint: WheelSpeedSetPoint, dt: float):
        """Advance the pose using wheel angular speeds (rad/s)."""
        if dt <= 0.0:
            return

        left = wheel_speed_setpoint.left
        right = wheel_speed_setpoint.right

        left_accel = (left - self._prev_left) / dt
        right_accel = (right - self._prev_right) / dt

        diff_accel = abs(right_accel - left_accel)
        diff_excess = max(0.0, diff_accel - self.slip_accel_threshold)
        slip_turn = self.base_slip + self.slip_gain * min(1.0, diff_excess / self.slip_accel_ref)

        v_left = left * WHEELS_RADIUS
        v_right = right * WHEELS_RADIUS

        v = 0.5 * (v_left + v_right)
        omega = (v_right - v_left) / WHEELS_DISTANCE
        omega *= (1.0 - slip_turn)
        
        self._x_dot = -v * math.sin(self._pose.theta)
        self._y_dot = v * math.cos(self._pose.theta)
        self._theta_dot = omega

        self._pose.x += self._x_dot * dt
        self._pose.y += self._y_dot * dt
        self._pose.theta = wrap_to_pi(self._pose.theta + omega * dt)

        self._prev_left = left
        self._prev_right = right
    
