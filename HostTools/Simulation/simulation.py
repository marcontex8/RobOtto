#!/usr/bin/env python3
"""Pose estimation simulation using the kinematic model and sensors."""

from __future__ import annotations

from dataclasses import dataclass
import math
from typing import List, Tuple

import matplotlib.pyplot as plt

from kinematic_model import (
	DifferentialDriveKinematics,
	OdometryWithNoise,
	GyroscopeWithNoise,
)
from simulation_common import (
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
		self.gyro = GyroscopeWithNoise(START_POSE, noise_std=NOISE_STD)
		self.planner = TrajectoryPlanner()

		self.kin_trace: List[Pose] = []
		self.odom_trace: List[Pose] = []
		self.gyro_trace: List[float] = []
		self.time: List[float] = []
		self.targets = TARGET_POINTS
		self.target_index = 0


	def simulate(self):
		steps = int(math.ceil(SIMULATION_DURATION / DT))

		self.planner.set_target_pose(self.targets[self.target_index])
		for i in range(steps + 1):
			t = i * DT

			current_status = self.kinematic.get_status()
			current_pose = Pose(current_status.x, current_status.y, current_status.theta)
			if self.planner.is_target_reached(current_pose):
				if self.target_index + 1 >= len(self.targets):
					self.target_index = 0
				else:
					self.target_index += 1
				self.planner.set_target_pose(self.targets[self.target_index])

			# simulate the trajectory controller
			setpoint = self.planner.compute_wheels_speed_setpoint(current_pose, DT)

			# simulate the robot motion
			self.kinematic.step(setpoint, DT)
			current_status = self.kinematic.get_status()

			# simulate the sensors
			self.odometry.step(setpoint, DT)
			self.gyro.step(current_status.theta_dot, DT)

			odom_pose = self.odometry.get()
			gyro_theta = self.gyro.get()

			self.time.append(t)

			self.kin_trace.append(Pose(current_status.x, current_status.y, current_status.theta))
			self.odom_trace.append(odom_pose)
			self.gyro_trace.append(gyro_theta)


	def plot_results(self) -> None:
		fig = plt.figure(figsize=(12, 10))

		ax1 = fig.add_subplot(2, 2, 1)
		ax1.plot(self.time, [pose.x for pose in self.kin_trace], label="Kinematic")
		ax1.plot(self.time, [pose.x for pose in self.odom_trace], label="Odometry")
		ax1.set_ylabel("x (m)")
		ax1.grid(True)
		ax1.legend()

		ax2 = fig.add_subplot(2, 2, 2)
		ax2.plot(self.time, [pose.y for pose in self.kin_trace], label="Kinematic")
		ax2.plot(self.time, [pose.y for pose in self.odom_trace], label="Odometry")
		ax2.set_ylabel("y (m)")
		ax2.grid(True)
		ax2.legend()

		ax3 = fig.add_subplot(2, 2, 3)
		ax3.plot(self.time, [pose.theta for pose in self.kin_trace], label="Kinematic")
		ax3.plot(self.time, [pose.theta for pose in self.odom_trace], label="Odometry")
		ax3.plot(self.time, self.gyro_trace, label="Gyro")
		ax3.set_xlabel("time (s)")
		ax3.set_ylabel("theta (rad)")
		ax3.grid(True)
		ax3.legend()

		ax4 = fig.add_subplot(2, 2, 4)
		ax4.plot([pose.x for pose in self.kin_trace], [pose.y for pose in self.kin_trace], label="Kinematic Model")
		ax4.plot([pose.x for pose in self.odom_trace], [pose.y for pose in self.odom_trace], label="Estimated pose")
		ax4.scatter([pose.x for pose in self.targets], [pose.y for pose in self.targets], c="green", label="Targets")
		ax4.set_xlabel("x (m)")
		ax4.set_ylabel("y (m)")
		ax4.set_aspect("equal", adjustable="box")
		ax4.grid(True)
		ax4.legend()

		fig.tight_layout()
		plt.show()


def main() -> None:
	simulation = Simulation()
	simulation.simulate()
	simulation.plot_results()

if __name__ == "__main__":
	main()
