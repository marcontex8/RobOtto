from dataclasses import dataclass
import math


@dataclass
class Pose:
    x: float
    y: float
    theta: float

@dataclass
class KinematicStatus:
    x: float
    y: float
    theta: float
    x_dot: float
    y_dot: float
    theta_dot: float

@dataclass
class WheelSpeedSetPoint:
    left: float
    right: float

def wrap_to_pi(angle: float) -> float:
    while angle > math.pi:
        angle -= 2.0 * math.pi
    while angle < -math.pi:
        angle += 2.0 * math.pi
    return angle


# hardware parameters
WHEELS_DISTANCE = 0.186
WHEELS_RADIUS = 0.0334

# behavior parameters
CRUISE_SPEED = 6.28 * WHEELS_RADIUS
LOOKAHEAD_DISTANCE = 0.4
POSITION_TOLERANCE = 0.05

# sensor parameters
GYRO_STD = 0.1
ODOM_STD = 0.01
SLIPPAGE_FACTOR = 0.1

# Simulation configuration (hardcoded defaults)
DT = 0.02
SIMULATION_DURATION = 60.0

START_POSE = Pose(0.0, 0.0, 0.0)

SQUARE_SIZE = 2.0
TARGET_POINTS = [
    Pose(0.0, 0.0, 0.0),
    Pose(0.0, 10.0, 0.0),
]

TARGET_POSE = TARGET_POINTS[-1]
