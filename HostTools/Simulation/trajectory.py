import math
from simulation_common import (
    LOOKAHEAD_DISTANCE,
    POSITION_TOLERANCE,
    Pose,
    WHEELS_DISTANCE,
    WHEELS_RADIUS,
    wrap_to_pi,
    WheelSpeedSetPoint,
    CRUISE_SPEED,
)

class TrajectoryPlanner:
    """Pure pursuit trajectory planner for differential drive robots."""
    def __init__(self):
        self.target_pose = Pose(0.0, 0.0, 0.0)

    def set_target_pose(self, target_pose: Pose) -> None:
        self.target_pose = target_pose

    def is_target_reached(self, current_pose: Pose) -> bool:
        target_vector_x = self.target_pose.x - current_pose.x
        target_vector_y = self.target_pose.y - current_pose.y
        distance = math.sqrt(target_vector_x * target_vector_x + target_vector_y * target_vector_y)
        return distance <= POSITION_TOLERANCE

    def compute_wheels_speed_setpoint(self, current_pose: Pose, dt: float) -> WheelSpeedSetPoint:
        target_vector_x = self.target_pose.x - current_pose.x
        target_vector_y = self.target_pose.y - current_pose.y
        distance = math.sqrt(target_vector_x * target_vector_x + target_vector_y * target_vector_y)

        linear_speed = CRUISE_SPEED
        if distance < POSITION_TOLERANCE:
            return WheelSpeedSetPoint(left=0.0, right=0.0)
        else:
            scale = min(1.0, LOOKAHEAD_DISTANCE / distance)
            lookahead_x = current_pose.x + target_vector_x * scale
            lookahead_y = current_pose.y + target_vector_y * scale
            alpha = wrap_to_pi(math.atan2(current_pose.x - lookahead_x, lookahead_y - current_pose.y) - current_pose.theta)
            ld = max(LOOKAHEAD_DISTANCE * scale, 1e-6)
            curvature = 2.0 * math.sin(alpha) / ld
            angular_speed = linear_speed * curvature

            v_left = linear_speed - (angular_speed * WHEELS_DISTANCE / 2.0)
            v_right = linear_speed + (angular_speed * WHEELS_DISTANCE / 2.0)

            left_speed = v_left / WHEELS_RADIUS
            right_speed = v_right / WHEELS_RADIUS
        return WheelSpeedSetPoint(left=left_speed, right=right_speed)

