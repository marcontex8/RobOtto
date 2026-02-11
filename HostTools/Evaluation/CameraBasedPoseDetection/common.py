import glob
import os

def find_camera_device(camera_id_name):
	by_id_dir = '/dev/v4l/by-id'
	target = os.path.join(by_id_dir, camera_id_name)
	if os.path.exists(target):
		return target
	matches = glob.glob(os.path.join(by_id_dir, f'*{camera_id_name}*'))
	if matches:
		return matches[0]
	raise RuntimeError(f'Camera not found in {by_id_dir}: {camera_id_name}')