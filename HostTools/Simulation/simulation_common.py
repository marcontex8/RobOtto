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
POSITION_TOLERANCE = 0.1

# sensor parameters
GYRO_STD = 0.1
ODOM_STD = 0.01
SLIPPAGE_FACTOR = 0.1

# Simulation configuration (hardcoded defaults)
DT = 0.02
SIMULATION_DURATION = 10.0

START_POSE = Pose(0.0, 0.0, 0.0)

TARGET_POINTS = [
    Pose(0.0, 0.0, 0.0),
    Pose(0.0, 1.0, 0.0),
    Pose(1.0, 1.0, 0.0),
    Pose(1.0, 0.0, 0.0),
    Pose(0.0, 0.0, 0.0),
]
'''
TARGET_POINTS = [
    # Smooth closed trajectory (gentle S-curves)
    Pose(0.0, 0.0, 0.0),
    Pose(0.4, 0.8, 0.0),
    Pose(1.0, 1.6, 0.0),
    Pose(1.8, 2.2, 0.0),
    Pose(2.8, 2.5, 0.0),
    Pose(3.8, 2.2, 0.0),
    Pose(4.6, 1.4, 0.0),
    Pose(5.0, 0.4, 0.0),
    Pose(5.2, -0.6, 0.0),
    Pose(4.8, -1.5, 0.0),
    Pose(4.0, -2.2, 0.0),
    Pose(3.0, -2.5, 0.0),
    Pose(2.0, -2.2, 0.0),
    Pose(1.2, -1.5, 0.0),
    Pose(0.6, -0.7, 0.0),
    Pose(0.0, 0.0, 0.0),
]
'''

TARGET_POSE = TARGET_POINTS[-1]
