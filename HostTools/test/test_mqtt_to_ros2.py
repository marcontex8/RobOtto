import rclpy
from rclpy.node import Node
from geometry_msgs.msg import PoseStamped
import paho.mqtt.client as mqtt
import json
import time
import subprocess
import threading

class PoseSubscriber(Node):
    def __init__(self):
        super().__init__('pose_subscriber')
        self.subscription = self.create_subscription(
            PoseStamped,
            '/pose',
            self.listener_callback,
            10)

    def listener_callback(self, msg):
        self.get_logger().info(f'Received pose: position=({msg.pose.position.x}, {msg.pose.position.y}, {msg.pose.position.z}), '
                               f'orientation=({msg.pose.orientation.x}, {msg.pose.orientation.y}, {msg.pose.orientation.z}, {msg.pose.orientation.w})')

def generate_fake_pose(i):
    return {
        'timestamp': time.time(),
        'position': {'x': i * 0.1, 'y': i * 0.2, 'z': i * 0.3},
        'orientation': {'x': 0.0, 'y': 0.0, 'z': 0.0, 'w': 1.0}  # Identity quaternion
    }

def mqtt_publisher():
    client = mqtt.Client(callback_api_version=mqtt.CallbackAPIVersion.VERSION2)
    try:
        client.connect('127.0.0.1', 1884, 60)
    except Exception as e:
        print(f"Failed to connect to MQTT broker: {e}. Ensure the broker is running and reachable.")
        return
    while True:
        pose = generate_fake_pose(int(time.time()) % 100)
        client.publish('mqtt_pose_topic', json.dumps(pose))
        time.sleep(1)
    client.disconnect()

def main():
    # Start the ROS2 node in a subprocess
    node_process = subprocess.Popen(['python3', '/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/HostTools/mqtt_to_ros2.py'])
    
    # Initialize ROS2 for the subscriber
    rclpy.init()
    subscriber_node = PoseSubscriber()
    
    # Start MQTT publisher in a thread
    publisher_thread = threading.Thread(target=mqtt_publisher)
    publisher_thread.start()
    
    # Spin the subscriber
    try:
        rclpy.spin(subscriber_node)
    except KeyboardInterrupt:
        pass
    finally:
        subscriber_node.destroy_node()
        rclpy.shutdown()
        node_process.terminate()
        publisher_thread.join()

if __name__ == '__main__':
    main()
