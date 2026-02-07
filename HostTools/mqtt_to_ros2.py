import rclpy
from rclpy.node import Node
from rclpy.time import Time

from geometry_msgs.msg import PoseStamped
from std_msgs.msg import String

import paho.mqtt.client as mqtt
import json
import math
import time

class MqttToRos2Node(Node):
    def __init__(self):
        super().__init__('mqtt_pose_publisher')
        self.publisher_ = self.create_publisher(PoseStamped, '/RobOtto/pose', 10)
        self.status_publisher_ = self.create_publisher(String, '/RobOtto/pose_info', 10)

        self.declare_parameter('use_fake_mqtt', False)
        self.declare_parameter('frame_id', 'map')

        self._use_fake_mqtt = self.get_parameter('use_fake_mqtt').get_parameter_value().bool_value
        self._frame_id = self.get_parameter('frame_id').get_parameter_value().string_value

        if self._use_fake_mqtt:
            self._start_time = time.time()
            self._timer = self.create_timer(0.1, self._publish_fake_pose)
        else:
            self.mqtt_client = mqtt.Client(callback_api_version=mqtt.CallbackAPIVersion.VERSION2)
            self.mqtt_client.on_message = self.on_mqtt_pose_message
            try:
                self.mqtt_client.connect('127.0.0.1', 1884, 60)
            except Exception as e:
                self.get_logger().error(
                    f"Failed to connect to MQTT broker: {e}. Ensure the broker is running and reachable."
                )
                raise RuntimeError(f"MQTT broker connection failed: {e}")
            self.mqtt_client.subscribe('RobOtto/pose')
            self.mqtt_client.loop_start()

    def on_mqtt_pose_message(self, client, userdata, msg):
        try:
            data = json.loads(msg.payload.decode())
            pose_msg = self._pose_from_xytheta(
                int(data['timestamp']),
                float(data['x']),
                float(data['y']),
                float(data['theta']),
            )
            self.publisher_.publish(pose_msg)
            status_msg = String()
            status_msg.data = (
                f"mqtt pose: t={data['timestamp']} x={data['x']} y={data['y']} theta={data['theta']}"
            )
            self.status_publisher_.publish(status_msg)
            self.get_logger().info('Published pose')
        except Exception as e:
            self.get_logger().error(f'Error processing MQTT message: {e}')

    def _publish_fake_pose(self):
        t = time.time() - self._start_time
        x = math.cos(t)
        y = math.sin(t)
        theta = t % (2.0 * math.pi)
        stamp_ns = time.time_ns()
        pose_msg = self._pose_from_xytheta(stamp_ns, x, y, theta)
        self.publisher_.publish(pose_msg)
        status_msg = String()
        status_msg.data = (
            f"fake pose: t={t:.2f} x={x:.3f} y={y:.3f} theta={theta:.3f} stamp_ns={stamp_ns}"
        )
        self.status_publisher_.publish(status_msg)
        self.get_logger().info(
            f"Fake publish /RobOtto/pose: t={t:.2f} x={x:.3f} y={y:.3f} theta={theta:.3f} stamp_ns={stamp_ns}"
        )

    def _pose_from_xytheta(self, timestamp_ns: int, x: float, y: float, theta: float) -> PoseStamped:
        pose_msg = PoseStamped()
        pose_msg.header.stamp = Time(nanoseconds=timestamp_ns).to_msg()
        pose_msg.header.frame_id = self._frame_id
        pose_msg.pose.position.x = x
        pose_msg.pose.position.y = y
        pose_msg.pose.position.z = 0.0

        half = theta * 0.5
        pose_msg.pose.orientation.x = 0.0
        pose_msg.pose.orientation.y = 0.0
        pose_msg.pose.orientation.z = math.sin(half)
        pose_msg.pose.orientation.w = math.cos(half)
        return pose_msg

def main(args=None):
    rclpy.init(args=args)
    try:
        node = MqttToRos2Node()
        rclpy.spin(node)
        node.destroy_node()
    except RuntimeError as e:
        print("Something went wrong:", e)
    rclpy.shutdown()

if __name__ == '__main__':
    main()
