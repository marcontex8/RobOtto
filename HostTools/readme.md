# Tools for Host System

## Mosquitto MQTT broker
cd ./mosquitto_mqtt/
mosquitto -c mosquitto.conf -v

## MQTT -> ROS2
This ROS2 node is used to convert MQTT data to ROS2 visualizable data
python ./mqtt_to_ros2.py

## FoxGlove Bridge
ros2 launch foxglove_bridge foxglove_bridge_launch.xml

