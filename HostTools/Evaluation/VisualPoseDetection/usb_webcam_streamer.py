#!/usr/bin/env python3

import time
import os
import glob

import cv2
import rclpy
from cv_bridge import CvBridge
from rclpy.node import Node
from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy
from sensor_msgs.msg import Image


class UsbWebcamStreamer(Node):
	def __init__(self):
		super().__init__('usb_webcam_streamer')

		# Hardcoded options
		self.camera_id_name = 'usb-BC-250325---X_Trust_1080P_Webcam-video-index0'
		self.width = 1280
		self.height = 720
		self.fps = 10.0
		self.frame_id = 'usb_camera'
		self.topic = 'usb_camera/image_raw'
		self.use_v4l2 = True
		self.grayscale = True
		self.show_preview = False

		self.device = self._find_camera_device(self.camera_id_name)

		# Open camera
		backend = cv2.CAP_V4L2 if self.use_v4l2 else 0
		self.cap = cv2.VideoCapture(self.device, backend) if self.use_v4l2 else cv2.VideoCapture(self.device)
		if not self.cap.isOpened():
			self.get_logger().error(f'Failed to open camera device {self.device}')
			raise RuntimeError('Camera open failed')

		# Apply camera settings
		if self.width > 0:
			self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, self.width)
		if self.height > 0:
			self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, self.height)
		if self.fps > 0:
			self.cap.set(cv2.CAP_PROP_FPS, self.fps)

		actual_width = int(self.cap.get(cv2.CAP_PROP_FRAME_WIDTH))
		actual_height = int(self.cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
		actual_fps = float(self.cap.get(cv2.CAP_PROP_FPS))
		self.get_logger().info(
			f'Camera opened: device={self.device}, '
			f'resolution={actual_width}x{actual_height}, fps={actual_fps:.2f}')

		# ROS setup
		image_qos = QoSProfile(
			reliability=ReliabilityPolicy.BEST_EFFORT,
			history=HistoryPolicy.KEEP_LAST,
			depth=1
		)
		self.publisher = self.create_publisher(Image, self.topic, qos_profile=image_qos)
		self.bridge = CvBridge()

		if self.fps <= 0:
			self.fps = 30.0
		self.timer = self.create_timer(1.0 / self.fps, self._timer_callback)
		self._last_fail_log = 0.0

		encoding = 'mono8' if self.grayscale else 'bgr8'
		self.get_logger().info(
			f'Publishing to {self.topic} as {encoding}, frame_id={self.frame_id}, '
			f'preview={self.show_preview}')

	def _find_camera_device(self, camera_id_name):
		by_id_dir = '/dev/v4l/by-id'
		target = os.path.join(by_id_dir, camera_id_name)
		if os.path.exists(target):
			return target
		matches = glob.glob(os.path.join(by_id_dir, f'*{camera_id_name}*'))
		if matches:
			return matches[0]
		self.get_logger().error(f'Camera not found in {by_id_dir}: {camera_id_name}')
		raise RuntimeError('Target camera not found')

	def _timer_callback(self):
		ret, frame = self.cap.read()
		if not ret:
			now = time.time()
			if now - self._last_fail_log > 2.0:
				self.get_logger().warning('Failed to read frame from camera')
				self._last_fail_log = now
			return

		if self.grayscale:
			frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
			msg = self.bridge.cv2_to_imgmsg(frame, encoding='mono8')
		else:
			msg = self.bridge.cv2_to_imgmsg(frame, encoding='bgr8')

		msg.header.stamp = self.get_clock().now().to_msg()
		msg.header.frame_id = self.frame_id
		self.publisher.publish(msg)

		if self.show_preview:
			cv2.imshow('USB Webcam Stream', frame)
			cv2.waitKey(1)

	def destroy_node(self):
		try:
			if self.cap is not None:
				self.cap.release()
			if self.show_preview:
				cv2.destroyAllWindows()
		finally:
			super().destroy_node()


def main(args=None):
	rclpy.init(args=args)
	node = None
	try:
		node = UsbWebcamStreamer()
		rclpy.spin(node)
	except Exception as exc:
		if node is not None:
			node.get_logger().error(f'Fatal error: {exc}')
		raise
	finally:
		if node is not None:
			node.destroy_node()
		rclpy.shutdown()


if __name__ == '__main__':
	main()
