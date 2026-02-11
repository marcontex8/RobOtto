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
import sys
import time
import logging
import traceback

class ArucoPoseDetector(Node):
    def __init__(self):
        super().__init__('aruco_pose_detector')
        self.counter = 0
        
        # --- Ground marker world positions (meters) ---
        # Example: marker ID 1 at (0,0), ID 2 at (1,0), ID 3 at (0,1)
        self.ground_markers_world = {
            1: np.array([0.0, 0.0]),
            2: np.array([1.0, 0.0]),
            3: np.array([0.0, 1.0]),
        }
        self.robot_marker_id = 10  # Set this to your robot's marker ID
        self.ground_marker_ids = set(self.ground_markers_world.keys())
        self.camera_to_ground_R = None
        self.camera_to_ground_t = None

        # Parameters
        self.declare_parameter('marker_size', 0.08)  # Marker size in meters
        self.declare_parameter('aruco_dict', 'DICT_5X5_1000')  # ArUco dictionary
        self.declare_parameter('debug_level', 'DEBUG')  # Debug level
        self.declare_parameter('visualize', True)  # Enable visualization
        
        self.marker_size = self.get_parameter('marker_size').value
        dict_name = self.get_parameter('aruco_dict').value
        debug_level = self.get_parameter('debug_level').value
        self.visualize = self.get_parameter('visualize').value

        # Setup logging
        self.logger = logging.getLogger('aruco_pose_detector')
        level = getattr(logging, debug_level, logging.INFO)
        self.logger.setLevel(level)
        
        # ArUco setup
        try:
            self.get_logger().info(f'Initializing ArUco detector with dictionary: {dict_name}')
            self.aruco_dict = aruco.getPredefinedDictionary(getattr(aruco, dict_name))
            self.aruco_params = aruco.DetectorParameters()
            self.detector = aruco.ArucoDetector(self.aruco_dict, self.aruco_params)
            self.get_logger().info('ArUco detector initialized successfully')
        except Exception as e:
            self.get_logger().error(f'Failed to initialize ArUco detector: {e}')
            raise

        # ROS setup
        self.bridge = CvBridge()
        self.camera_matrix = None
        self.dist_coeffs = None
        self.camera_info_received = False

        # Load camera calibration from JSON
        import json, os
        calib_path = '/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/HostTools/Evaluation/CameraBasedPoseDetection/calibration.json'
        try:
            with open(calib_path, 'r') as f:
                calib = json.load(f)
            self.camera_matrix = np.array(calib['camera_matrix'])
            self.dist_coeffs = np.array(calib['distortion_coefficients'])
            self.camera_info_received = True
            self.get_logger().info(f"Loaded camera calibration from {calib_path}")
        except Exception as e:
            self.get_logger().error(f"Failed to load camera calibration: {e}")
            raise

        # Statistics
        self.frame_count = 0
        self.detection_count = 0
        self.total_markers_detected = 0
        self.last_stats_time = time.time()

        # Subscriptions
        from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy
        image_qos = QoSProfile(
            reliability=ReliabilityPolicy.BEST_EFFORT,
            history=HistoryPolicy.KEEP_LAST,
            depth=1
        )
        self.image_sub = self.create_subscription(
            Image,
            'usb_camera/image_raw',
            self.image_callback,
            qos_profile=image_qos)
        self.get_logger().info('Subscribed to usb_camera/image_raw (Image) with queue size 1 and BEST_EFFORT QoS')

        # Publisher: only robot pose
        self.robot_pose_pub = self.create_publisher(PoseStamped, '/robot_pose', 10)
        self.get_logger().info('Created publisher for /robot_pose')

        self.get_logger().info(f'ArucoPoseDetector initialized. Debug level: {debug_level}, '
                              f'Marker size: {self.marker_size}m, '
                              f'Visualization: {self.visualize}')

    def image_callback(self, msg):
        """Process only the most recent image, drop older ones if still processing."""
        self.counter += 1
        print(f'Received image #{self.counter}')
        self.frame_count += 1
        if self.frame_count % 100 == 0:
            self.get_logger().info(f'Frame {self.frame_count} received')
        try:
            cv_image = self.bridge.imgmsg_to_cv2(msg)
            self._process_frame(cv_image)
        except Exception as e:
            self.get_logger().error(
                f'Error processing image (frame {self.frame_count}): {e}\n'
                f'{traceback.format_exc()}')


    def _detect_markers(self, cv_image):
        detect_start = time.time()
        corners, ids, rejected = self.detector.detectMarkers(cv_image)
        detect_time = time.time() - detect_start
        num_detected = len(ids) if ids is not None else 0
        num_rejected = len(rejected) if rejected is not None else 0
        if self.frame_count % 100 == 0:
            self.get_logger().info(
                f'Detection time: {detect_time*1000:.2f}ms, '
                f'Detected: {num_detected}, Rejected: {num_rejected}')
        return corners, ids, rejected

    def _estimate_marker_poses(self, corners, ids):
        marker_poses = {}
        if ids is not None:
            for i, corners_set in enumerate(corners):
                marker_id = int(ids[i][0])
                image_points = corners_set[0].astype(np.float32)
                object_points = np.array([
                    [-self.marker_size/2, -self.marker_size/2, 0],
                    [self.marker_size/2, -self.marker_size/2, 0],
                    [self.marker_size/2, self.marker_size/2, 0],
                    [-self.marker_size/2, self.marker_size/2, 0]
                ], dtype=np.float32)
                success, rvec, tvec = cv2.solvePnP(
                    object_points, image_points,
                    self.camera_matrix, self.dist_coeffs,
                    useExtrinsicGuess=False, flags=cv2.SOLVEPNP_ITERATIVE)
                if success:
                    marker_poses[marker_id] = (rvec, tvec)
        return marker_poses


    def _process_frame(self, cv_image):
        frame_start_time = time.time()
        corners, ids, rejected = self._detect_markers(cv_image)
        marker_poses = self._estimate_marker_poses(corners, ids)
        if self.frame_count % 100 == 0:
            self.get_logger().info(f'Frame {self.frame_count}: Found {len(marker_poses)} markers')
        if self.visualize:
            self._visualize_detections(cv_image, corners, ids)
        frame_total_time = time.time() - frame_start_time
        if self.frame_count % 100 == 0:
            self.get_logger().info(f'Total frame processing: {frame_total_time*1000:.2f}ms')


    def _visualize_detections(self, cv_image, corners, ids):
        """Draw detected markers on image and display."""
        try:
            # Convert to color image for display
            cv_image_color = cv2.cvtColor(cv_image, cv2.COLOR_GRAY2BGR)
            
            if ids is not None and len(ids) > 0:
                cv_image_color = cv2.aruco.drawDetectedMarkers(cv_image_color, corners, ids)
            else:
                cv_image_color = cv_image_color
            
            cv2.imshow('ArUco Detections', cv_image_color)
            cv2.waitKey(1)
            self.get_logger().debug('Visualization image displayed')
        except Exception as e:
            self.get_logger().warn(f'Visualization failed: {e}')


def main(args=None):
    # Setup console logging
    logging.basicConfig(
        level=logging.DEBUG,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    
    try:
        rclpy.init(args=args)
        logging.info('=' * 60)
        logging.info('ArUco Pose Detector Node Started')
        logging.info('=' * 60)
        node = ArucoPoseDetector()
        rclpy.spin(node)
        node.destroy_node()
    except Exception as e:
        logging.error(f'Fatal error: {e}\n{traceback.format_exc()}')
        sys.exit(1)
    finally:
        rclpy.shutdown()
        logging.info('=' * 60)
        logging.info('ArUco Pose Detector Node Stopped')
        logging.info('=' * 60 + '\n')

if __name__ == '__main__':
    main()
