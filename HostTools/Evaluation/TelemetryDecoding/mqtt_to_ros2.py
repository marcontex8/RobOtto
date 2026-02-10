import rclpy
from rclpy.node import Node
from rclpy.time import Time

from geometry_msgs.msg import PoseStamped
from std_msgs.msg import Float32MultiArray

import paho.mqtt.client as mqtt
import math
import time

from telemetry_decode import decode_telemetry_payload

class MqttToRos2Node(Node):
    def __init__(self):
        super().__init__('mqtt_pose_publisher')
        self.robotto_pose_publisher_ = self.create_publisher(PoseStamped, '/RobOtto/pose', 10)
        self.target_pose_publisher_ = self.create_publisher(PoseStamped, '/RobOtto/target_pose', 10)
        self.wheels_speed_setpoint_publisher_ = self.create_publisher(Float32MultiArray, '/RobOtto/wheels_speed_setpoint', 10)

        self.declare_parameter('frame_id', 'map')
        self._frame_id = self.get_parameter('frame_id').get_parameter_value().string_value

        self.mqtt_client = mqtt.Client(callback_api_version=mqtt.CallbackAPIVersion.VERSION2)
        self.mqtt_client.on_message = self.on_mqtt_telemetry_message
        try:
            self.mqtt_client.connect('127.0.0.1', 1884, 60)
        except Exception as e:
            self.get_logger().error(
                f"Failed to connect to MQTT broker: {e}. Ensure the broker is running and reachable."
            )
            raise RuntimeError(f"MQTT broker connection failed: {e}")
        self.get_logger().info(
                f"Successfully connected to MQTT broker."
            )
        self.mqtt_client.subscribe('RobOtto/telemetry')
        self.mqtt_client.loop_start()

    def on_mqtt_telemetry_message(self, client, userdata, msg):
        try:
            telemetry = decode_telemetry_payload(msg.payload)
            pose_msg = self._pose_from_xytheta(
                telemetry['robotto_timestamp'],
                telemetry['robotto_x'],
                telemetry['robotto_y'],
                telemetry['robotto_theta'],
            )
            self.robotto_pose_publisher_.publish(pose_msg)

            target_pose_msg = self._pose_from_xytheta(
                telemetry['target_timestamp'],
                telemetry['target_x'],
                telemetry['target_y'],
                telemetry['target_theta'],
            )
            self.target_pose_publisher_.publish(target_pose_msg)

            speed_setpoint_msg = Float32MultiArray()
            speed_setpoint_msg.data = [
                telemetry['wheel_speed_setpoint_left'],
                telemetry['wheel_speed_setpoint_right'],
                1.0 if telemetry['wheel_speed_setpoint_active'] else 0.0,
            ]
            self.wheels_speed_setpoint_publisher_.publish(speed_setpoint_msg)   

            self.get_logger().info('Published telemetry')
        except Exception as e:
            self.get_logger().error(f'Error processing MQTT message: {e}')

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
