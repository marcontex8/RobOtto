#!/bin/bash

# Start Mosquitto MQTT broker
cd ./mosquitto_mqtt/
mosquitto -c mosquitto.conf -v &
MOSQUITTO_PID=$!

# Start MQTT to ROS2 converter
cd ..
python ./mqtt_to_ros2.py &
MQTT_ROS2_PID=$!

# Start FoxGlove Bridge
ros2 launch foxglove_bridge foxglove_bridge_launch.xml &
FOXGLOVE_PID=$!

# Wait for all processes
wait $MOSQUITTO_PID $MQTT_ROS2_PID $FOXGLOVE_PID
