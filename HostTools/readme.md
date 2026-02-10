# Tools for Host System

## Prerequisites
- ROS2 installed and sourced in your shell (provides rclpy, geometry_msgs, std_msgs, sensor_msgs, cv_bridge)
- Python 3.10+ recommended

## Install dependencies
Create a virtual environment and install Python dependencies:
```bash
python -m venv .venv
./.venv/bin/python -m pip install -r HostTools/requirements.txt
```

## Evaluation tools
The Evaluation folder contains tools to validate RobOtto behavior, including:
- MQTT telemetry bridge to ROS2
- Camera-based pose estimation using ArUco markers

### Telemetry decoding (MQTT → ROS2)
Start the MQTT broker:
```bash
cd HostTools/Evaluation/TelemetryDecoding
mosquitto -c mosquitto.conf -v
```

Run the bridge node:
```bash
python HostTools/Evaluation/TelemetryDecoding/mqtt_to_ros2.py
```

Launch Foxglove bridge (optional):
```bash
ros2 launch foxglove_bridge foxglove_bridge_launch.xml
```

### Camera-based pose estimation
You can use a web-based camera stream if a reliable Wi-Fi connection is available.
Otherwise, a standard USB webcam works better.

The pose-estimation tools are ROS2 nodes and can be visualized with
[Foxglove](https://foxglove.dev/product/visualization).

#### ArUco pose detector
```bash
python3 aruco_pose_detector.py --ros-args \
	-p debug_level:=DEBUG \
	-p visualize:=true \
	-p marker_size:=0.05 \
	-p aruco_dict:=DICT_5X5_50
```

#### Camera calibration
```bash
python3 calibrate_camera.py --image-dir PoseDetection/PhoneCameraStreamer/calib_images
```


