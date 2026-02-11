#!/usr/bin/env python3
"""Kinematic model for a differential-drive robot."""

from __future__ import annotations

from dataclasses import dataclass
import math
import random
from simulation_common import Pose, Status, WheelSpeedSetPoint, wrap_to_pi, WHEELS_DISTANCE, WHEELS_RADIUS

class DifferentialDriveKinematics:
    def __init__(self, pose: Pose) -> None:
        self._pose = Pose(pose.x, pose.y, pose.theta)
        self._x_dot = 0.0
        self._y_dot = 0.0
        self._theta_dot = 0.0

    def get_status(self) -> Status:
        return Status(
            x=self._pose.x,
            y=self._pose.y,
            theta=self._pose.theta,
            x_dot=self._x_dot,
            y_dot=self._y_dot,
            theta_dot=self._theta_dot
        )

    def step(self, wheel_speed_setpoint: WheelSpeedSetPoint, dt: float):
        """Advance the pose using wheel angular speeds (rad/s)."""
        v_left = wheel_speed_setpoint.left * WHEELS_RADIUS
        v_right = wheel_speed_setpoint.right * WHEELS_RADIUS

        v = 0.5 * (v_left + v_right)
        omega = (v_right - v_left) / WHEELS_DISTANCE

        self._x_dot = -v * math.sin(self._pose.theta)
        self._y_dot = v * math.cos(self._pose.theta)
        self._theta_dot = omega

        self._pose.x += self._x_dot * dt
        self._pose.y += self._y_dot * dt
        self._pose.theta = wrap_to_pi(self._pose.theta + omega * dt)
    

class OdometryWithNoise:
    """Simulated odometry with noise."""
    def __init__(self, pose: Pose, noise_std: float = 0.01) -> None:
        self._pose = Pose(pose.x, pose.y, pose.theta)
        self._noise_std = noise_std
        self.slippage_factor = 0.1

    def get(self) -> Pose:
        return Pose(self._pose.x, self._pose.y, self._pose.theta)
    
    def step(self, wheel_speed_setpoint: WheelSpeedSetPoint, dt: float):
        """Advance the pose using wheel angular speeds (rad/s) with noise."""
        v_left = wheel_speed_setpoint.left * WHEELS_RADIUS * (1.0 - self.slippage_factor) + random.gauss(0.0, self._noise_std)
        v_right = wheel_speed_setpoint.right * WHEELS_RADIUS * (1.0 - self.slippage_factor) + random.gauss(0.0, self._noise_std)

        v = 0.5 * (v_left + v_right)
        omega = (v_right - v_left) / WHEELS_DISTANCE

        self._pose.x += -v * math.sin(self._pose.theta) * dt
        self._pose.y += v * math.cos(self._pose.theta) * dt
        self._pose.theta = wrap_to_pi(self._pose.theta + omega * dt)



class GyroscopeWithNoise:
    """Simulated gyroscope with noise."""
    def __init__(self, pose: Pose, noise_std: float = 0.01) -> None:
        self._noise_std = noise_std
        self._theta = pose.theta

    def get(self) -> float:
        return self._theta
    
    def step(self, angular_velocity: float, dt: float):
        """Advance the angle using angular velocity (rad/s) with noise."""
        self._theta += angular_velocity * dt + random.gauss(0.0, self._noise_std)
        self._theta = wrap_to_pi(self._theta)