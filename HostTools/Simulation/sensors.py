import random
from simulation_common import Pose, WheelSpeedSetPoint, wrap_to_pi, WHEELS_DISTANCE, WHEELS_RADIUS
import math

class OdometryWithNoise:
    """Simulated odometry with noise."""
    def __init__(self, pose: Pose, noise_std: float = 0.01) -> None:
        self._pose = Pose(pose.x, pose.y, pose.theta)
        self._noise_std = noise_std

    def get(self) -> Pose:
        return Pose(self._pose.x, self._pose.y, self._pose.theta)
    
    def stimulate(self, wheel_speed_setpoint: WheelSpeedSetPoint, dt: float):
        """Advance the pose using wheel angular speeds (rad/s) with noise."""
        if dt <= 0.0:
            return

        left = wheel_speed_setpoint.left
        right = wheel_speed_setpoint.right

        v_left = left * WHEELS_RADIUS + random.gauss(0.0, self._noise_std)
        v_right = right * WHEELS_RADIUS + random.gauss(0.0, self._noise_std)

        v = 0.5 * (v_left + v_right)
        omega = (v_right - v_left) / WHEELS_DISTANCE

        self._pose.x += -v * math.sin(self._pose.theta) * dt
        self._pose.y += v * math.cos(self._pose.theta) * dt
        self._pose.theta = wrap_to_pi(self._pose.theta + omega * dt)



class GyroscopeWithNoise:
    """Simulated gyroscope with noise."""
    def __init__(self, noise_std: float) -> None:
        self._noise_std = noise_std
        self._omega = 0.0

    def get(self) -> float:
        return self._omega
    
    def stimulate(self, omega: float):
        self._omega = omega + random.gauss(0.0, self._noise_std)