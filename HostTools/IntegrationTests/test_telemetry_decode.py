import sys
import os
import unittest


CURRENT_DIR = os.path.dirname(__file__)
HOSTTOOLS_DIR = os.path.abspath(os.path.join(CURRENT_DIR, '..'))
if HOSTTOOLS_DIR not in sys.path:
    sys.path.insert(0, HOSTTOOLS_DIR)

from telemetry_decode import decode_telemetry_payload


class TestTelemetryDecode(unittest.TestCase):
    def assertTelemetry(
        self,
        payload,
        timestamp,
        x,
        y,
        theta,
        target_timestamp,
        target_x,
        target_y,
        target_theta,
        left,
        right,
        active,
        detection_timestamp,
        detection_distance_m,
        detection_servo_angle,
        places=5,
    ):
        telemetry = decode_telemetry_payload(payload)
        self.assertEqual(timestamp, telemetry['robotto_timestamp'])
        self.assertAlmostEqual(x, telemetry['robotto_x'], places=places)
        self.assertAlmostEqual(y, telemetry['robotto_y'], places=places)
        self.assertAlmostEqual(theta, telemetry['robotto_theta'], places=places)
        self.assertEqual(target_timestamp, telemetry['target_timestamp'])
        self.assertAlmostEqual(target_x, telemetry['target_x'], places=places)
        self.assertAlmostEqual(target_y, telemetry['target_y'], places=places)
        self.assertAlmostEqual(target_theta, telemetry['target_theta'], places=places)
        self.assertAlmostEqual(left, telemetry['wheel_speed_setpoint_left'], places=places)
        self.assertAlmostEqual(right, telemetry['wheel_speed_setpoint_right'], places=places)
        self.assertEqual(active, telemetry['wheel_speed_setpoint_active'])
        self.assertEqual(detection_timestamp, telemetry['object_detection_timestamp'])
        self.assertAlmostEqual(detection_distance_m, telemetry['object_detection_distance_m'], places=places)
        self.assertAlmostEqual(detection_servo_angle, telemetry['object_detection_servo_angle'], places=places)

    def test_decode_telemetry_zero(self):
        self.assertTelemetry(
            b"AwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
            0,
            0.0,
            0.0,
            0.0,
            0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            False,
            0,
            0.0,
            0.0,
        )

    def test_decode_telemetry_negative_values(self):
        self.assertTelemetry(
            b"AyoAAAAAAMC/AAAQQAAAYMBkAAAAAACAwAAAsEAAANDAAAAAwAAAQEABTQAAAAAAoD8AADTC",
            42,
            -1.5,
            2.25,
            -3.5,
            100,
            -4.0,
            5.5,
            -6.5,
            -2.0,
            3.0,
            True,
            77,
            1.25,
            -45.0,
        )

    def test_decode_telemetry_max_timestamp(self):
        self.assertTelemetry(
            b"A/////8AAPZCAAA2wgAAAD4HAAAAAACAPwAAAEAAAEBAAAAgQQAAMMEAoA8AAAAAIEAAALJC",
            4294967295,
            123.0,
            -45.5,
            0.125,
            7,
            1.0,
            2.0,
            3.0,
            10.0,
            -11.0,
            False,
            4000,
            2.5,
            89.0,
        )


if __name__ == '__main__':
    unittest.main()
