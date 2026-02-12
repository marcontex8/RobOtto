#!/usr/bin/env python3
"""Pose estimation simulation using the kinematic model and sensors."""

from __future__ import annotations


import math
from typing import List, Tuple

import matplotlib.pyplot as plt

from fusion import HeadingKalmanFilter
from sensors import OdometryWithNoise, GyroscopeWithNoise
from kinematic_model import DifferentialDriveKinematics

from simulation_common import (
	KinematicStatus,
	Pose,
	START_POSE,
	TARGET_POINTS,
	DT,
	SIMULATION_DURATION,
	NOISE_STD,
)
from trajectory import TrajectoryPlanner


class Simulation:
	def __init__(self):
		self.kinematic = DifferentialDriveKinematics(START_POSE)
		self.odometry = OdometryWithNoise(START_POSE, noise_std=NOISE_STD)
		self.gyro = GyroscopeWithNoise(noise_std=NOISE_STD)
		self.planner = TrajectoryPlanner()
		self.filter = HeadingKalmanFilter(initial_theta=START_POSE.theta, dt=DT)

		self.kin_trace: List[KinematicStatus] = []
		self.odom_trace: List[Pose] = []
		self.gyro_trace: List[float] = []
		self.filter_trace: List[Tuple[float, float]] = []
		self.time: List[float] = []
		self.targets = TARGET_POINTS
		self.target_index = 0


	def simulate(self):
		steps = int(math.ceil(SIMULATION_DURATION / DT))

		self.planner.set_target_pose(self.targets[self.target_index])
		for i in range(steps + 1):
			t = i * DT

			current_status = self.kinematic.get_status()
			if self.planner.is_target_reached(self.odometry.get()):
				if self.target_index + 1 >= len(self.targets):
					self.target_index = 0
				else:
					self.target_index += 1
				self.planner.set_target_pose(self.targets[self.target_index])

			# simulate the trajectory controller
			setpoint = self.planner.compute_wheels_speed_setpoint(self.odometry.get(), DT)

			# simulate the robot motion
			self.kinematic.step(setpoint, DT)
			current_status = self.kinematic.get_status()

			# simulate the sensors and fusion
			self.odometry.stimulate(setpoint, DT)
			odom_pose = self.odometry.get()

			self.gyro.stimulate(current_status.theta_dot)
			gyro_omega = self.gyro.get()

			self.filter.step(odom_pose.theta, gyro_omega)
			filter_theta, filter_omega = self.filter.get()

			self.time.append(t)
			self.kin_trace.append(current_status)
			self.odom_trace.append(odom_pose)
			self.gyro_trace.append(gyro_omega)
			self.filter_trace.append((filter_theta, filter_omega))


	def plot_results(self) -> None:
		fig = plt.figure(figsize=(12, 5))

		ax1 = fig.add_subplot(1, 3, 1)
		ax1.plot(self.time, [pose.theta for pose in self.kin_trace], label="Kinematic")
		ax1.plot(self.time, [pose.theta for pose in self.odom_trace], label="Odometry")
		ax1.plot(self.time, [theta for (theta, _) in self.filter_trace], label="Kalman Filter")
		ax1.set_xlabel("time (s)")
		ax1.set_ylabel("theta (rad)")
		ax1.grid(True)
		ax1.legend()


		ax2 = fig.add_subplot(1, 3, 2)
		ax2.plot(self.time, [pose.theta_dot for pose in self.kin_trace], label="Kinematic")
		ax2.plot(self.time, self.gyro_trace, label="Gyro")
		ax2.plot(self.time, [omega for (_, omega) in self.filter_trace], label="Kalman Filter")
		ax2.set_xlabel("time (s)")
		ax2.set_ylabel("omega (rad)")
		ax2.grid(True)
		ax2.legend()


		ax3 = fig.add_subplot(1, 3, 3)
		ax3.plot([pose.x for pose in self.kin_trace], [pose.y for pose in self.kin_trace], label="Kinematic Model")
		ax3.plot([pose.x for pose in self.odom_trace], [pose.y for pose in self.odom_trace], label="Estimated pose")
		ax3.scatter([pose.x for pose in self.targets], [pose.y for pose in self.targets], c="green", label="Targets")
		ax3.set_xlabel("x (m)")
		ax3.set_ylabel("y (m)")
		ax3.set_aspect("equal", adjustable="box")
		ax3.grid(True)
		ax3.legend()

		fig.tight_layout()
		plt.show()


def main() -> None:
	simulation = Simulation()
	simulation.simulate()
	simulation.plot_results()

if __name__ == "__main__":
	main()
