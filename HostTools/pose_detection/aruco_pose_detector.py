#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image, CameraInfo
from geometry_msgs.msg import PoseStamped, PoseArray, Pose
from cv_bridge import CvBridge
import cv2
import cv2.aruco as aruco
import numpy as np
from builtin_interfaces.msg import Time

class ArucoPoseDetector(Node):
    def __init__(self):
        super().__init__('aruco_pose_detector')

        # Parameters
        self.declare_parameter('marker_size', 0.05)  # Marker size in meters
        self.declare_parameter('aruco_dict', 'DICT_4X4_50')  # ArUco dictionary
        self.marker_size = self.get_parameter('marker_size').value
        dict_name = self.get_parameter('aruco_dict').value

        # ArUco setup
        self.aruco_dict = aruco.getPredefinedDictionary(getattr(aruco, dict_name))
        self.aruco_params = aruco.DetectorParameters()
        self.detector = aruco.ArucoDetector(self.aruco_dict, self.aruco_params)

        # ROS setup
        self.bridge = CvBridge()
        self.camera_matrix = None
        self.dist_coeffs = None

        # Subscriptions
        self.image_sub = self.create_subscription(
            Image,
            '/camera/image_raw',
            self.image_callback,
            10)
        self.camera_info_sub = self.create_subscription(
            CameraInfo,
            '/camera/camera_info',
            self.camera_info_callback,
            10)

        # Publisher
        self.pose_pub = self.create_publisher(PoseArray, '/aruco_poses', 10)

    def camera_info_callback(self, msg):
        # Extract camera intrinsics
        self.camera_matrix = np.array(msg.k).reshape(3, 3)
        self.dist_coeffs = np.array(msg.d)

    def image_callback(self, msg):
        if self.camera_matrix is None or self.dist_coeffs is None:
            self.get_logger().warn('Camera intrinsics not received yet')
            return

        try:
            # Convert ROS image to OpenCV
            cv_image = self.bridge.imgmsg_to_cv2(msg, desired_encoding='mono8')

            # Detect markers
            corners, ids, rejected = self.detector.detectMarkers(cv_image)

            if ids is not None:
                # Estimate pose
                rvecs, tvecs, _ = aruco.estimatePoseSingleMarkers(
                    corners, self.marker_size, self.camera_matrix, self.dist_coeffs)

                # Create PoseArray message
                pose_array_msg = PoseArray()
                pose_array_msg.header.stamp = msg.header.stamp
                pose_array_msg.header.frame_id = msg.header.frame_id

                for i, marker_id in enumerate(ids.flatten()):
                    # Rotation vector to quaternion
                    rvec = rvecs[i].flatten()
                    tvec = tvecs[i].flatten()

                    # Convert rotation vector to quaternion
                    rotation_matrix, _ = cv2.Rodrigues(rvec)
                    quaternion = self.rotation_matrix_to_quaternion(rotation_matrix)

                    pose = Pose()
                    pose.position.x = tvec[0]
                    pose.position.y = tvec[1]
                    pose.position.z = tvec[2]
                    pose.orientation.x = quaternion[0]
                    pose.orientation.y = quaternion[1]
                    pose.orientation.z = quaternion[2]
                    pose.orientation.w = quaternion[3]

                    pose_array_msg.poses.append(pose)

                # Publish pose array
                self.pose_pub.publish(pose_array_msg)
                self.get_logger().info(f'Published poses for {len(ids)} markers')

        except Exception as e:
            self.get_logger().error(f'Error processing image: {e}')

    def rotation_matrix_to_quaternion(self, R):
        # Convert rotation matrix to quaternion
        q = np.zeros(4)
        trace = np.trace(R)
        if trace > 0:
            s = 0.5 / np.sqrt(trace + 1.0)
            q[3] = 0.25 / s
            q[0] = (R[2, 1] - R[1, 2]) * s
            q[1] = (R[0, 2] - R[2, 0]) * s
            q[2] = (R[1, 0] - R[0, 1]) * s
        else:
            if R[0, 0] > R[1, 1] and R[0, 0] > R[2, 2]:
                s = 2.0 * np.sqrt(1.0 + R[0, 0] - R[1, 1] - R[2, 2])
                q[3] = (R[2, 1] - R[1, 2]) / s
                q[0] = 0.25 * s
                q[1] = (R[0, 1] + R[1, 0]) / s
                q[2] = (R[0, 2] + R[2, 0]) / s
            elif R[1, 1] > R[2, 2]:
                s = 2.0 * np.sqrt(1.0 + R[1, 1] - R[0, 0] - R[2, 2])
                q[3] = (R[0, 2] - R[2, 0]) / s
                q[0] = (R[0, 1] + R[1, 0]) / s
                q[1] = 0.25 * s
                q[2] = (R[1, 2] + R[2, 1]) / s
            else:
                s = 2.0 * np.sqrt(1.0 + R[2, 2] - R[0, 0] - R[1, 1])
                q[3] = (R[1, 0] - R[0, 1]) / s
                q[0] = (R[0, 2] + R[2, 0]) / s
                q[1] = (R[1, 2] + R[2, 1]) / s
                q[2] = 0.25 * s
        return q

def main(args=None):
    rclpy.init(args=args)
    node = ArucoPoseDetector()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()</content>
<parameter name="filePath">/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/HostTools/pose_detection/aruco_pose_detector.py