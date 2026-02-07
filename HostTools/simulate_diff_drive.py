#!/usr/bin/env python3
"""Simulate differential-drive robot using the provided wheel speed setpoint algorithm."""

from __future__ import annotations

import argparse
import math
from dataclasses import dataclass
from typing import List, Tuple

import matplotlib.pyplot as plt


@dataclass
class Pose:
    x: float
    y: float
    theta: float


@dataclass
class WheelSpeedSetPoint:
    active: bool
    left: float
    right: float


def wrap_to_pi(angle: float) -> float:
    while angle > math.pi:
        angle -= 2.0 * math.pi
    while angle < -math.pi:
        angle += 2.0 * math.pi
    return angle


def compute_wheel_speed_setpoint(
    current: Pose,
    end_pose: Pose,
    k_p_linear: float,
    k_p_angular: float,
    lookahead_distance: float,
    cruise_speed: float,
    min_speed: float,
    slow_radius: float,
    wheels_distance: float,
    wheels_radius: float,
) -> WheelSpeedSetPoint:
    target_vector_x = end_pose.x - current.x
    target_vector_y = end_pose.y - current.y
    distance = math.sqrt(target_vector_x * target_vector_x + target_vector_y * target_vector_y)

    linear_speed = cruise_speed

    if distance < 0.01:
        return WheelSpeedSetPoint(active=False, left=0.0, right=0.0)
    else:
        scale = min(1.0, lookahead_distance / distance)
        lookahead_x = current.x + target_vector_x * scale
        lookahead_y = current.y + target_vector_y * scale
        alpha = wrap_to_pi(math.atan2(current.x - lookahead_x, lookahead_y - current.y) - current.theta)
        ld = max(lookahead_distance * scale, 1e-6)
        curvature = 2.0 * math.sin(alpha) / ld
        angular_speed = k_p_angular * linear_speed * curvature

        v_left = linear_speed - (angular_speed * wheels_distance / 2.0)
        v_right = linear_speed + (angular_speed * wheels_distance / 2.0)

        left_speed = v_left / wheels_radius
        right_speed = v_right / wheels_radius

    return WheelSpeedSetPoint(active=True, left=left_speed, right=right_speed)


def simulate(
    start_pose: Pose,
    end_pose: Pose,
    k_p_linear: float,
    k_p_angular: float,
    lookahead_distance: float,
    cruise_speed: float,
    min_speed: float,
    slow_radius: float,
    wheels_distance: float,
    wheels_radius: float,
    dt: float,
    max_time: float,
    stop_distance: float,
    stop_angle: float,
) -> Tuple[List[float], List[Pose], List[float], List[float], List[float], List[float]]:
    time: List[float] = []
    poses: List[Pose] = []
    distances: List[float] = []
    angle_errors: List[float] = []
    left_speeds: List[float] = []
    right_speeds: List[float] = []

    current = Pose(start_pose.x, start_pose.y, start_pose.theta)
    t = 0.0

    while t <= max_time:
        setpoint = compute_wheel_speed_setpoint(
            current,
            end_pose,
            k_p_linear,
            k_p_angular,
            lookahead_distance,
            cruise_speed,
            min_speed,
            slow_radius,
            wheels_distance,
            wheels_radius,
        )

        v_left = setpoint.left * wheels_radius
        v_right = setpoint.right * wheels_radius

        v = 0.5 * (v_left + v_right)
        omega = (v_right - v_left) / wheels_distance

        current.x += -v * math.sin(current.theta) * dt
        current.y += v * math.cos(current.theta) * dt
        current.theta = wrap_to_pi(current.theta + omega * dt)

        dx = end_pose.x - current.x
        dy = end_pose.y - current.y
        distance = math.hypot(dx, dy)
        target_angle = math.atan2(dy, dx)
        error_angle = wrap_to_pi(target_angle - (current.theta + math.pi / 2.0))

        time.append(t)
        poses.append(Pose(current.x, current.y, current.theta))
        distances.append(distance)
        angle_errors.append(error_angle)
        left_speeds.append(setpoint.left)
        right_speeds.append(setpoint.right)

        if distance <= stop_distance and abs(error_angle) <= stop_angle:
            break

        t += dt

    return time, poses, distances, angle_errors, left_speeds, right_speeds


def plot_results(
    time: List[float],
    poses: List[Pose],
    distances: List[float],
    angle_errors: List[float],
    left_speeds: List[float],
    right_speeds: List[float],
    start_pose: Pose,
    end_pose: Pose,
) -> None:
    xs = [p.x for p in poses]
    ys = [p.y for p in poses]
    rel_xs = [p.x - end_pose.x for p in poses]
    rel_ys = [p.y - end_pose.y for p in poses]

    fig = plt.figure(figsize=(12, 8))

    ax1 = fig.add_subplot(2, 2, 1)
    ax1.plot(xs, ys, label="Trajectory")
    ax1.scatter([start_pose.x], [start_pose.y], c="green", label="Start")
    ax1.scatter([end_pose.x], [end_pose.y], c="red", label="Target")
    ax1.set_aspect("equal", adjustable="box")
    ax1.set_xlabel("x (m)")
    ax1.set_ylabel("y (m)")
    ax1.set_title("Robot Trajectory")
    ax1.grid(True)
    ax1.legend()

    ax2 = fig.add_subplot(2, 2, 2)
    ax2.plot(time, distances, label="Distance")
    ax2.set_xlabel("time (s)")
    ax2.set_ylabel("distance (m)")
    ax2.set_title("Distance to Target")
    ax2.grid(True)

    ax3 = fig.add_subplot(2, 2, 3)
    ax3.plot(time, angle_errors, label="Angle Error")
    ax3.set_xlabel("time (s)")
    ax3.set_ylabel("angle error (rad)")
    ax3.set_title("Heading Error")
    ax3.grid(True)

    ax4 = fig.add_subplot(2, 2, 4)
    ax4.plot(time, left_speeds, label="Left wheel")
    ax4.plot(time, right_speeds, label="Right wheel")
    ax4.set_xlabel("time (s)")
    ax4.set_ylabel("wheel speed (rad/s)")
    ax4.set_title("Wheel Speeds")
    ax4.grid(True)
    ax4.legend()

    fig.tight_layout()

    plt.show()


def parse_pose(value: str) -> Pose:
    parts = value.split(",")
    if len(parts) != 3:
        raise argparse.ArgumentTypeError("Pose must be in the form x,y,theta")
    return Pose(float(parts[0]), float(parts[1]), float(parts[2]))


def main() -> None:
    parser = argparse.ArgumentParser(description="Simulate differential drive controller.")
    parser.add_argument("--start", type=parse_pose, default="0,0,0", help="Start pose x,y,theta")
    parser.add_argument("--target", type=parse_pose, default="2,2,0", help="Target pose x,y,theta")
    parser.add_argument("--kpl", type=float, default=0.1, help="Linear gain")
    parser.add_argument("--kpa", type=float, default=0.5, help="Angular gain")
    parser.add_argument("--lookahead", type=float, default=0.4, help="Pure pursuit lookahead distance (m)")
    parser.add_argument("--cruise", type=float, default=1.0, help="Cruise linear speed (m/s)")
    parser.add_argument("--min-speed", type=float, default=0.5, help="Minimum wheel linear speed (m/s)")
    parser.add_argument("--slow-radius", type=float, default=0.4, help="Distance where min speed ramps to zero (m)")
    parser.add_argument("--wheel-distance", type=float, default=0.20, help="Wheel distance (m)")
    parser.add_argument("--wheel-radius", type=float, default=0.05, help="Wheel radius (m)")
    parser.add_argument("--dt", type=float, default=0.02, help="Time step (s)")
    parser.add_argument("--tmax", type=float, default=30.0, help="Max simulation time (s)")
    parser.add_argument("--stop-distance", type=float, default=0.1, help="Stop distance (m)")
    parser.add_argument("--stop-angle", type=float, default=0.03, help="Stop angle error (rad)")

    args = parser.parse_args()

    time, poses, distances, angle_errors, left_speeds, right_speeds = simulate(
        start_pose=args.start,
        end_pose=args.target,
        k_p_linear=args.kpl,
        k_p_angular=args.kpa,
        lookahead_distance=args.lookahead,
        cruise_speed=args.cruise,
        min_speed=args.min_speed,
        slow_radius=args.slow_radius,
        wheels_distance=args.wheel_distance,
        wheels_radius=args.wheel_radius,
        dt=args.dt,
        max_time=args.tmax,
        stop_distance=args.stop_distance,
        stop_angle=args.stop_angle,
    )

    plot_results(
        time,
        poses,
        distances,
        angle_errors,
        left_speeds,
        right_speeds,
        start_pose=args.start,
        end_pose=args.target,
    )


if __name__ == "__main__":
    main()
