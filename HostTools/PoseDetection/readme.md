This folder contains a ROS2 node that detects ArUco markers in camera images and estimates their 3D pose.

## Files
- `aruco_pose_detector.py`: ROS2 node for ArUco marker detection and pose estimation

## Usage
1. Ensure ROS2 is installed and sourced
2. Install dependencies: `pip install opencv-python cv-bridge`
3. Run the node: `python3 aruco_pose_detector.py`

## Topics
- Subscribes to:
  - `/camera/image_raw` (sensor_msgs/Image)
  - `/camera/camera_info` (sensor_msgs/CameraInfo)
- Publishes to:
  - `/aruco_poses` (geometry_msgs/PoseArray)

## Parameters
- `marker_size`: Size of the ArUco marker in meters (default: 0.05)
- `aruco_dict`: ArUco dictionary to use (default: DICT_4X4_50)
- `debug_level`: Logging level (DEBUG, INFO, WARNING, ERROR) (default: INFO)
- `visualize`: Enable visualization of detected markers in a window (default: False)
- `save_debug_images`: Save debug images with detections to files (default: False)

## Debugging

The node includes comprehensive debugging capabilities:

### Logging Levels
Run with different debug levels to control verbosity:
```bash
# Maximum verbosity
ros2 run my_package aruco_pose_detector --ros-args -p debug_level:=DEBUG

# Minimum verbosity
ros2 run my_package aruco_pose_detector --ros-args -p debug_level:=WARNING
```

### Debug Output
The node logs:
- **Initialization**: ArUco detector setup and parameter values
- **Camera info**: Calibration matrix and distortion coefficients
- **Frame processing**: Image conversion, detection, and pose estimation times
- **Detections**: Marker IDs and 3D positions/rotations (when DEBUG level)
- **Statistics**: FPS, frame count, detection rate (every 5 seconds)
- **Errors**: Full stack traces for troubleshooting

### Enable Visualization
Display detected markers in real-time:
```bash
ros2 run my_package aruco_pose_detector --ros-args -p visualize:=true
```

### Save Debug Images
Save frames with detected markers:
```bash
ros2 run my_package aruco_pose_detector --ros-args -p save_debug_images:=true
```
Debug images are saved as `aruco_debug_XXXXXX.png`

### Example Full Debug Run
```bash
ros2 run my_package aruco_pose_detector --ros-args \
  -p debug_level:=DEBUG \
  -p visualize:=true \
  -p marker_size:=0.05 \
  -p aruco_dict:=DICT_5X5_50
```