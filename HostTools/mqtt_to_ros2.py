import rclpy
from rclpy.node import Node

from geometry_msgs.msg import PoseStamped

import paho.mqtt.client as mqtt
import json
from builtin_interfaces.msg import Time

class MqttToRos2Node(Node):
    def __init__(self):
        super().__init__('mqtt_pose_publisher')
        self.publisher_ = self.create_publisher(PoseStamped, '/pose', 10)
        self.mqtt_client = mqtt.Client(callback_api_version=mqtt.CallbackAPIVersion.VERSION2)
        self.mqtt_client.on_message = self.on_mqtt_message
        try:
            self.mqtt_client.connect('127.0.0.1', 1884, 60)
        except Exception as e:
            self.get_logger().error(f"Failed to connect to MQTT broker: {e}. Ensure the broker is running and reachable.")
            raise RuntimeError(f"MQTT broker connection failed: {e}")
        self.mqtt_client.subscribe('mqtt_pose_topic')  # Replace with your MQTT topic
        self.mqtt_client.loop_start()

    def on_mqtt_message(self, client, userdata, msg):
        try:
            data = json.loads(msg.payload.decode())
            pose_msg = PoseStamped()
            pose_msg.header.stamp = Time(sec=int(data['timestamp']), nanosec=int((data['timestamp'] % 1) * 1e9))
            pose_msg.header.frame_id = 'map'  # Adjust frame as needed
            pose_msg.pose.position.x = data['position']['x']
            pose_msg.pose.position.y = data['position']['y']
            pose_msg.pose.position.z = data['position']['z']
            pose_msg.pose.orientation.x = data['orientation']['x']
            pose_msg.pose.orientation.y = data['orientation']['y']
            pose_msg.pose.orientation.z = data['orientation']['z']
            pose_msg.pose.orientation.w = data['orientation']['w']
            self.publisher_.publish(pose_msg)
            self.get_logger().info('Published pose')
        except Exception as e:
            self.get_logger().error(f'Error processing MQTT message: {e}')

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
