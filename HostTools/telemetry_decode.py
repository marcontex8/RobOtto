import base64
import json
import struct
from typing import Dict, Union


def decode_telemetry_payload(payload: Union[bytes, str]) -> Dict[str, float]:
    """
    Binary payload format (base64 encoded):
    """
    raw = base64.b64decode(payload, validate=True)
    unpacked = struct.unpack('<B I f f f I f f f f f B', raw)
    version = unpacked[0]
    if version != 2:
        raise ValueError(f"Unsupported payload version: {version}")
    (_, timestamp, x, y, theta,
     target_timestamp, target_x, target_y, target_theta,
     left, right, active) = unpacked
    return {
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
    }
