from __future__ import annotations
from dataclasses import dataclass

import numpy as np

from simulation_common import wrap_to_pi

class StateSpaceModel:
    def __init__(self, dt: float):
        self.q_theta = 0.05
        self.q_omega = 0.01
        self.r_theta = 0.5
        self.r_omega = 0.1
        self.dt = dt
        self.F = np.array([[1.0, dt], [0.0, 1.0]])  # State transition matrix
        self.H = np.eye(2)                           # Measurement matrix

        self.Q = np.diag([self.q_theta, self.q_omega])  # Process noise covariance
        self.R = np.diag([self.r_theta, self.r_omega])  # Measurement noise covariance


class HeadingKalmanFilter:
    def __init__(self, initial_theta: float, dt: float):
        self.X = np.array([[initial_theta], [0.0]])  # [theta; omega]
        self.P = np.eye(2) * 0.1  # Initial covariance
        self.model = StateSpaceModel(dt)

    def step(self, odom_theta: float, gyro_angular_velocity: float):
        # Prediction step
        self.X = self.model.F @ self.X
        self.P = self.model.F @ self.P @ self.model.F.T + self.model.Q

        # Measurement update (theta from odometry, omega from gyro)
        z = np.array([[wrap_to_pi(odom_theta)], [gyro_angular_velocity]])
        y = z - self.model.H @ self.X
        y[0, 0] = wrap_to_pi(y[0, 0])

        S = self.model.H @ self.P @ self.model.H.T + self.model.R  # Residual covariance
        K = self.P @ self.model.H.T @ np.linalg.inv(S)  # Kalman gain

        self.X = self.X + K @ y  # Updated state estimate
        I = np.eye(2)
        TMP = (I - K @ self.model.H)
        self.P = TMP @ self.P @ TMP.T + K @ self.model.R @ K.T  # Updated covariance
        self.X[0, 0] = wrap_to_pi(self.X[0, 0])

    def get(self):
        return wrap_to_pi(self.X[0, 0]), self.X[1, 0]

