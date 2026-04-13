import base64
import json
import struct
from typing import Dict, Union


def decode_telemetry_payload(payload: Union[bytes, str]) -> Dict[str, float]:
    """
    Binary payload format (base64 encoded):
    """
    raw = base64.b64decode(payload, validate=True)

    if len(raw) < 1:
        raise ValueError('Payload too short')

    version = raw[0]
    if version == 1:
        unpacked = struct.unpack('<B I f f f I f f f f f B I f f', raw)
        (_, timestamp, x, y, theta,
         target_timestamp, target_x, target_y, target_theta,
         left, right, active,
         detection_timestamp, detection_distance_m, detection_servo_angle) = unpacked
    else:
        raise ValueError(f"Unsupported payload version: {version}")

    telemetry = {
        'robotto_timestamp': int(timestamp),
        'robotto_x': float(x),
        'robotto_y': float(y),
        'robotto_theta': float(theta),
        'target_timestamp': int(target_timestamp),
        'target_x': float(target_x),
        'target_y': float(target_y),
        'target_theta': float(target_theta),
        'wheel_speed_setpoint_left': float(left),
        'wheel_speed_setpoint_right': float(right),
        'wheel_speed_setpoint_active': bool(active),
        'object_detection_timestamp': int(detection_timestamp),
        'object_detection_distance_m': float(detection_distance_m),
        'object_detection_servo_angle': float(detection_servo_angle),
    }
    return telemetry
