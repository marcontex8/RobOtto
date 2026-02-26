from __future__ import annotations
from dataclasses import dataclass

import numpy as np

from simulation_common import wrap_to_pi



class HeadingKalmanFilter:
    def __init__(self, initial_theta: float, dt: float):
        # Tuning parameters (these could be adjusted based on expected noise characteristics)
        self.Q = np.diag([1, 1])  # Process noise covariance
        self.R_odom = np.array([[1]])  # Measurement noise covariance (odometry)
        self.R_gyro = np.array([[1]])  # Measurement noise covariance (gyro)
        self.dt = dt

        # State-space model matrices
        self.F = np.array([[1.0, dt], [0.0, 1.0]])  # State transition matrix
        self.H_odom = np.array([[1.0, 0.0]])  # Measurement matrix for odometry (only measures theta)
        self.H_gyro = np.array([[0.0, 1.0]])  # Measurement matrix for gyro (only measures omega)

        # Initial state and covariance
        self.X = np.array([[initial_theta], [0.0]])  # [theta; omega]
        self.P = np.eye(2) * 0.1  # Initial covariance

    def setMeasurementNoise(self, r_theta: float, r_omega: float):
        self.R_odom = np.array([[r_theta]])
        self.R_gyro = np.array([[r_omega]])    

    def setProcessNoise(self, q_theta: float, q_omega: float):
        self.Q = np.diag([q_theta, q_omega])

    def predict(self):
        self.X = self.F @ self.X
        self.P = self.F @ self.P @ self.F.T + self.Q

    def update_odometry(self, odom_theta: float):
        z = np.array([[wrap_to_pi(odom_theta)]])
        y = z - self.H_odom @ self.X  # Measurement residual
        y[0, 0] = wrap_to_pi(y[0, 0])

        S = self.H_odom @ self.P @ self.H_odom.T + self.R_odom  # Residual covariance
        K = self.P @ self.H_odom.T @ np.linalg.inv(S)  # Kalman gain

        self.X = self.X + K @ y  # Updated state estimate
        I = np.eye(2)
        TMP = (I - K @ self.H_odom)
        self.P = TMP @ self.P @ TMP.T + K @ self.R_odom @ K.T  # Updated covariance
        self.X[0, 0] = wrap_to_pi(self.X[0, 0])


    def update_gyro(self, gyro_angular_velocity: float):
        z = np.array([[gyro_angular_velocity]])
        y = z - self.H_gyro @ self.X  # Measurement residual

        S = self.H_gyro @ self.P @ self.H_gyro.T + self.R_gyro  # Residual covariance
        K = self.P @ self.H_gyro.T @ np.linalg.inv(S)  # Kalman gain

        self.X = self.X + K @ y  # Updated state estimate
        I = np.eye(2)
        TMP = (I - K @ self.H_gyro)
        self.P = TMP @ self.P @ TMP.T + K @ self.R_gyro @ K.T  # Updated covariance
        self.X[0, 0] = wrap_to_pi(self.X[0, 0])

    def get(self):
        return self.X[0, 0], self.X[1, 0]


class SlipDetector:
    def __init__(self):
        self.slip_threshold = 0.3
        pass

    def detect_slip(self, angular_velocity_from_odometry: float, angular_velocity_from_gyro: float) -> bool:
        if abs(angular_velocity_from_odometry) < 0.01:
            return False
        slip_ratio = abs(angular_velocity_from_gyro - angular_velocity_from_odometry) / abs(angular_velocity_from_odometry)
        return slip_ratio > self.slip_threshold
        