# Tools for Host System

## Run everything

Launch Mosquitto, MQTT->ROS2, and FoxGlove bridge (each in its own process):

python3 run_all.py


## Phone Camera To ROS2

### Camera Phone -> ROS2
python3 PoseDetection/PhoneCameraStreamer/phone_camera_streamer.py

Note: most mobile browsers require HTTPS for camera access (or localhost). This server now requires HTTPS.
On first run, a self-signed cert is generated in PoseDetection/PhoneCameraStreamer (cert.pem/key.pem).

Open the page:
https://<laptop-ip>:8080/?w=1280&h=720&fps=30

Python deps:
- aiohttp
- aiortc
- av
- opencv-python
- cv-bridge (from ROS2)

ROS2 params (examples):
- `http_port` (default 8080)
- `topic` (default /camera/image_raw)
- `camera_info_topic` (default /camera/camera_info)
- `grayscale` (default false)
- `publish_camera_info` (default false)
- `frame_id` (default camera)

### ARUCO pose detector
python3 aruco_pose_detector.py --ros-args -p debug_level:=DEBUG -p visualize:=true -p marker_size:=0.05 -p aruco_dict:=DICT_5X5_50

### Camera Calibration
python3  calibrate_camera.py --image-dir PoseDetection/PhoneCameraStreamer/calib_images


## RobOtto to ROS2

### Mosquitto MQTT broker
cd ./MosquittoMQTT/
mosquitto -c mosquitto.conf -v

### MQTT -> ROS2
This ROS2 node is used to convert MQTT data to ROS2 visualizable data
python ./mqtt_to_ros2.py





## Visualization

## FoxGlove Bridge
ros2 launch foxglove_bridge foxglove_bridge_launch.xml