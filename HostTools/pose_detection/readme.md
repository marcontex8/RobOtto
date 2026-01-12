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