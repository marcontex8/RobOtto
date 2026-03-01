#!/usr/bin/env python3
"""Pose estimation simulation using the kinematic model and sensors."""

from __future__ import annotations


import math
from typing import List, Tuple

import matplotlib.pyplot as plt

from fusion import HeadingKalmanFilter, SlipDetector
from sensors import OdometryWithNoise, GyroscopeWithNoise
from kinematic_model import DifferentialDriveKinematics

from simulation_common import (
	wrap_to_pi,
	KinematicStatus,
	Pose,
	START_POSE,
	TARGET_POINTS,
	DT,
	SIMULATION_DURATION,
	GYRO_STD,
	ODOM_STD,
)
from trajectory import TrajectoryPlanner


class Simulation:
	def __init__(self):
		self.kinematic = DifferentialDriveKinematics(START_POSE)
		self.odometry = OdometryWithNoise(START_POSE, noise_std=ODOM_STD)
		self.gyro = GyroscopeWithNoise(noise_std=GYRO_STD)
		self.planner = TrajectoryPlanner()
		self.filter = HeadingKalmanFilter(initial_theta=START_POSE.theta, dt=DT)
		self.filter_odom_only = HeadingKalmanFilter(initial_theta=START_POSE.theta, dt=DT)
		self.filter_gyro_only = HeadingKalmanFilter(initial_theta=START_POSE.theta, dt=DT)

		for f in [self.filter, self.filter_odom_only, self.filter_gyro_only]:
			f.setMeasurementNoise(r_theta=2 * ODOM_STD, r_omega=GYRO_STD)
			f.setProcessNoise(q_theta=100, q_omega=0.005)
		self.slip_detector = SlipDetector()

		self.kin_trace: List[KinematicStatus] = []
		self.odom_trace: List[Pose] = []
		self.gyro_trace: List[float] = []
		self.filter_trace: List[Tuple[float, float]] = []
		self.filter_odom_only_trace: List[Tuple[float, float]] = []
		self.filter_gyro_only_trace: List[Tuple[float, float]] = []
		self.filter_pose_trace: List[Pose] = []
		self.filter_odom_only_pose_trace: List[Pose] = []
		self.filter_gyro_only_pose_trace: List[Pose] = []
		self.slip_detector_trace: List[bool] = []
		self.time: List[float] = []
		self.targets = TARGET_POINTS
		self.target_index = 0

		self.gyro_theta = 0.0
		self.gyro_theta_trace: List[float] = []

		self.odom_omega = 0.0
		self.odom_omega_trace: List[float] = []

		self.filter_pose = Pose(START_POSE.x, START_POSE.y, START_POSE.theta)
		self.filter_odom_only_pose = Pose(START_POSE.x, START_POSE.y, START_POSE.theta)
		self.filter_gyro_only_pose = Pose(START_POSE.x, START_POSE.y, START_POSE.theta)

	def simulate(self):
		steps = int(math.ceil(SIMULATION_DURATION / DT))

		self.planner.set_target_pose(self.targets[self.target_index])
		for i in range(steps + 1):
			t = i * DT

			current_status = self.kinematic.get_status()
			if self.planner.is_target_reached(current_status): #self.odometry.get()):
				if self.target_index + 1 >= len(self.targets):
					self.target_index = 0
				else:
					self.target_index += 1
				self.planner.set_target_pose(self.targets[self.target_index])

			# simulate the trajectory controller
			#curr_pose = self.odometry.get()
			#curr_pose.theta = self.gyro_theta
			curr_pose = current_status # use the true pose for control to isolate estimation errors
			setpoint = self.planner.compute_wheels_speed_setpoint(curr_pose, DT)

			# simulate the robot motion
			self.kinematic.step(setpoint, DT)
			current_status = self.kinematic.get_status()

			# simulate the sensors and fusion
			self.odometry.stimulate(setpoint, DT)
			odom_pose = self.odometry.get()

			self.gyro.stimulate(current_status.theta_dot)
			gyro_omega = self.gyro.get()

			# extract the angle from gyro and odometry for comparison and fusion
			self.gyro_theta = wrap_to_pi(self.gyro_theta + gyro_omega * DT)
			self.odom_omega = (odom_pose.theta - self.odom_trace[-1].theta) / DT if len(self.odom_trace) > 0 else 0.0
			odom_linear_speed = math.hypot(
				odom_pose.x - self.odom_trace[-1].x,
				odom_pose.y - self.odom_trace[-1].y,
			) / DT if len(self.odom_trace) > 0 else 0.0

			self.gyro_theta_trace.append(self.gyro_theta)

			slipping = self.slip_detector.detect_slip(self.odom_omega, gyro_omega)
			self.slip_detector_trace.append(slipping)

			self.filter.predict()
			self.filter_odom_only.predict()
			self.filter_gyro_only.predict()

			if not slipping:
				self.filter.update_odometry(odom_pose.theta)
			self.filter.update_gyro(gyro_omega)
			self.filter_odom_only.update_odometry(odom_pose.theta)
			self.filter_gyro_only.update_gyro(gyro_omega)
			
			filter_theta, filter_omega = self.filter.get()
			filter_odom_only_theta, filter_odom_only_omega = self.filter_odom_only.get()
			filter_gyro_only_theta, filter_gyro_only_omega = self.filter_gyro_only.get()

			self.filter_pose.x += -odom_linear_speed * math.sin(filter_theta) * DT
			self.filter_pose.y += odom_linear_speed * math.cos(filter_theta) * DT
			self.filter_pose.theta = filter_theta

			self.filter_odom_only_pose.x += -odom_linear_speed * math.sin(filter_odom_only_theta) * DT
			self.filter_odom_only_pose.y += odom_linear_speed * math.cos(filter_odom_only_theta) * DT
			self.filter_odom_only_pose.theta = filter_odom_only_theta

			self.filter_gyro_only_pose.x += -odom_linear_speed * math.sin(filter_gyro_only_theta) * DT
			self.filter_gyro_only_pose.y += odom_linear_speed * math.cos(filter_gyro_only_theta) * DT
			self.filter_gyro_only_pose.theta = filter_gyro_only_theta

			self.time.append(t)
			self.kin_trace.append(current_status)
			self.odom_trace.append(odom_pose)
			self.gyro_trace.append(gyro_omega)
			self.odom_omega_trace.append(self.odom_omega)
			self.filter_trace.append((filter_theta, filter_omega))
			self.filter_odom_only_trace.append((filter_odom_only_theta, filter_odom_only_omega))
			self.filter_gyro_only_trace.append((filter_gyro_only_theta, filter_gyro_only_omega))
			self.filter_pose_trace.append(Pose(self.filter_pose.x, self.filter_pose.y, self.filter_pose.theta))
			self.filter_odom_only_pose_trace.append(Pose(self.filter_odom_only_pose.x, self.filter_odom_only_pose.y, self.filter_odom_only_pose.theta))
			self.filter_gyro_only_pose_trace.append(Pose(self.filter_gyro_only_pose.x, self.filter_gyro_only_pose.y, self.filter_gyro_only_pose.theta))

	def plot_results(self) -> None:
		if not self.time:
			return

		fig = plt.figure(figsize=(14, 20))
		gs = fig.add_gridspec(5, 2)
		fig.suptitle("Kalman filters comparison", fontsize=14)

		# Row 1: Kalman using odometry + gyro
		ax1 = fig.add_subplot(gs[0, 0])
		ax1.plot(self.time, [pose.theta for pose in self.kin_trace], label="Kinematic")
		ax1.plot(self.time, [pose.theta for pose in self.odom_trace], label="Odometry input")
		ax1.plot(self.time, self.gyro_theta_trace, label="Gyro input (integrated)")
		ax1.plot(self.time, [theta for (theta, _) in self.filter_trace], label="Kalman output")
		ax1.set_title("Odom + gyro: theta")
		ax1.set_xlabel("time (s)")
		ax1.set_ylabel("theta (rad)")
		ax1.grid(True)
		ax1.legend()

		ax2 = fig.add_subplot(gs[0, 1], sharex=ax1)
		ax2.plot(self.time, [pose.theta_dot for pose in self.kin_trace], label="Kinematic")
		ax2.plot(self.time, self.odom_omega_trace, label="Odometry input (derived)")
		ax2.plot(self.time, self.gyro_trace, label="Gyro input")
		ax2.plot(self.time, [omega for (_, omega) in self.filter_trace], label="Kalman output")
		ax2.set_title("Odom + gyro: omega")
		ax2.set_xlabel("time (s)")
		ax2.set_ylabel("omega (rad/s)")
		ax2.grid(True)
		ax2.legend()

		# Row 2: Kalman using odometry only
		ax3 = fig.add_subplot(gs[1, 0], sharex=ax1)
		ax3.plot(self.time, [pose.theta for pose in self.kin_trace], label="Kinematic")
		ax3.plot(self.time, [pose.theta for pose in self.odom_trace], label="Odometry input")
		ax3.plot(self.time, [theta for (theta, _) in self.filter_odom_only_trace], label="Kalman output")
		ax3.set_title("Odom only: theta")
		ax3.set_xlabel("time (s)")
		ax3.set_ylabel("theta (rad)")
		ax3.grid(True)
		ax3.legend()

		ax4 = fig.add_subplot(gs[1, 1], sharex=ax1)
		ax4.plot(self.time, [pose.theta_dot for pose in self.kin_trace], label="Kinematic")
		ax4.plot(self.time, self.odom_omega_trace, label="Odometry input (derived)")
		ax4.plot(self.time, [omega for (_, omega) in self.filter_odom_only_trace], label="Kalman output")
		ax4.set_title("Odom only: omega")
		ax4.set_xlabel("time (s)")
		ax4.set_ylabel("omega (rad/s)")
		ax4.grid(True)
		ax4.legend()

		# Row 3: Kalman using gyro only
		ax5 = fig.add_subplot(gs[2, 0], sharex=ax1)
		ax5.plot(self.time, [pose.theta for pose in self.kin_trace], label="Kinematic")
		ax5.plot(self.time, self.gyro_theta_trace, label="Gyro input (integrated)")
		ax5.plot(self.time, [theta for (theta, _) in self.filter_gyro_only_trace], label="Kalman output")
		ax5.set_title("Gyro only: theta")
		ax5.set_xlabel("time (s)")
		ax5.set_ylabel("theta (rad)")
		ax5.grid(True)
		ax5.legend()

		ax6 = fig.add_subplot(gs[2, 1], sharex=ax1)
		ax6.plot(self.time, [pose.theta_dot for pose in self.kin_trace], label="Kinematic")
		ax6.plot(self.time, self.gyro_trace, label="Gyro input")
		ax6.plot(self.time, [omega for (_, omega) in self.filter_gyro_only_trace], label="Kalman output")
		ax6.set_title("Gyro only: omega")
		ax6.set_xlabel("time (s)")
		ax6.set_ylabel("omega (rad/s)")
		ax6.grid(True)
		ax6.legend()

		# Row 4: Slip detection over time
		ax7 = fig.add_subplot(gs[3, 0], sharex=ax1)
		slip_int = [1 if s else 0 for s in self.slip_detector_trace]
		ax7.step(self.time, slip_int, where="post", label="Slip detected")
		ax7.fill_between(self.time, slip_int, step="post", alpha=0.2)
		ax7.set_title("Slip detection")
		ax7.set_xlabel("time (s)")
		ax7.set_ylabel("slip")
		ax7.set_yticks([0, 1])
		ax7.set_yticklabels(["No", "Yes"])
		ax7.grid(True)
		ax7.legend()

		ax8 = fig.add_subplot(gs[3, 1], sharex=ax1)
		slip_int = [1 if s else 0 for s in self.slip_detector_trace]
		ax8.step(self.time, slip_int, where="post", label="Slip detected")
		ax8.fill_between(self.time, slip_int, step="post", alpha=0.2)
		ax8.set_title("Slip detection")
		ax8.set_xlabel("time (s)")
		ax8.set_ylabel("slip")
		ax8.set_yticks([0, 1])
		ax8.set_yticklabels(["No", "Yes"])
		ax8.grid(True)
		ax8.legend()

		# Row 5: Pose plot spanning both columns
		ax9 = fig.add_subplot(gs[4, :])
		kin_x = [pose.x for pose in self.kin_trace]
		kin_y = [pose.y for pose in self.kin_trace]
		odom_x = [pose.x for pose in self.odom_trace]
		odom_y = [pose.y for pose in self.odom_trace]
		filter_x = [pose.x for pose in self.filter_pose_trace]
		filter_y = [pose.y for pose in self.filter_pose_trace]
		filter_odom_only_x = [pose.x for pose in self.filter_odom_only_pose_trace]
		filter_odom_only_y = [pose.y for pose in self.filter_odom_only_pose_trace]
		filter_gyro_only_x = [pose.x for pose in self.filter_gyro_only_pose_trace]
		filter_gyro_only_y = [pose.y for pose in self.filter_gyro_only_pose_trace]

		kin_line_xy, = ax9.plot(kin_x, kin_y, label="Kinematic Model")
		odom_line_xy, = ax9.plot(odom_x, odom_y, label="Estimated pose")
		filter_line_xy, = ax9.plot(filter_x, filter_y, label="Kalman (odom + gyro)")
		filter_odom_only_line_xy, = ax9.plot(filter_odom_only_x, filter_odom_only_y, label="Kalman (odom only)")
		filter_gyro_only_line_xy, = ax9.plot(filter_gyro_only_x, filter_gyro_only_y, label="Kalman (gyro only)")
		ax9.scatter([pose.x for pose in self.targets], [pose.y for pose in self.targets], c="green", label="Targets")
		ax9.set_title("XY trajectory")
		ax9.set_xlabel("x (m)")
		ax9.set_ylabel("y (m)")
		ax9.set_aspect("equal", adjustable="box")
		ax9.grid(True)
		ax9.legend()

		def update_xy_for_time_window(_):
			x_min, x_max = ax1.get_xlim()
			indices = [i for i, t in enumerate(self.time) if x_min <= t <= x_max]

			if not indices:
				kin_line_xy.set_data([], [])
				odom_line_xy.set_data([], [])
				filter_line_xy.set_data([], [])
				filter_odom_only_line_xy.set_data([], [])
				filter_gyro_only_line_xy.set_data([], [])
				fig.canvas.draw_idle()
				return

			kin_x_view = [kin_x[i] for i in indices]
			kin_y_view = [kin_y[i] for i in indices]
			odom_x_view = [odom_x[i] for i in indices]
			odom_y_view = [odom_y[i] for i in indices]
			filter_x_view = [filter_x[i] for i in indices]
			filter_y_view = [filter_y[i] for i in indices]
			filter_odom_only_x_view = [filter_odom_only_x[i] for i in indices]
			filter_odom_only_y_view = [filter_odom_only_y[i] for i in indices]
			filter_gyro_only_x_view = [filter_gyro_only_x[i] for i in indices]
			filter_gyro_only_y_view = [filter_gyro_only_y[i] for i in indices]

			kin_line_xy.set_data(kin_x_view, kin_y_view)
			odom_line_xy.set_data(odom_x_view, odom_y_view)
			filter_line_xy.set_data(filter_x_view, filter_y_view)
			filter_odom_only_line_xy.set_data(filter_odom_only_x_view, filter_odom_only_y_view)
			filter_gyro_only_line_xy.set_data(filter_gyro_only_x_view, filter_gyro_only_y_view)

			x_all = kin_x_view + odom_x_view + filter_x_view + filter_odom_only_x_view + filter_gyro_only_x_view
			y_all = kin_y_view + odom_y_view + filter_y_view + filter_odom_only_y_view + filter_gyro_only_y_view
			if x_all and y_all:
				x_span = max(x_all) - min(x_all)
				y_span = max(y_all) - min(y_all)
				x_margin = 0.05 * x_span if x_span > 0 else 0.05
				y_margin = 0.05 * y_span if y_span > 0 else 0.05
				ax9.set_xlim(min(x_all) - x_margin, max(x_all) + x_margin)
				ax9.set_ylim(min(y_all) - y_margin, max(y_all) + y_margin)

			fig.canvas.draw_idle()

		for time_axis in [ax1, ax2, ax3, ax4, ax5, ax6, ax7, ax8]:
			time_axis.callbacks.connect("xlim_changed", update_xy_for_time_window)

		update_xy_for_time_window(None)

		fig.tight_layout(rect=[0, 0, 1, 0.98])
		plt.show()


def main() -> None:
	simulation = Simulation()
	simulation.simulate()
	simulation.plot_results()

if __name__ == "__main__":
	main()
