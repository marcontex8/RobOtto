#!/usr/bin/env python3
"""Launch Mosquitto, MQTT->ROS2 bridge, and FoxGlove bridge as separate processes."""

from __future__ import annotations

import os
import signal
import subprocess
import sys
from pathlib import Path
from typing import List


ROOT_DIR = Path(__file__).resolve().parent
MOSQUITTO_DIR = ROOT_DIR / "MosquittoMQTT"
MOSQUITTO_CONF = MOSQUITTO_DIR / "mosquitto.conf"


def _start_process(argv: List[str], cwd: Path) -> subprocess.Popen:
    return subprocess.Popen(
        argv,
        cwd=str(cwd),
        start_new_session=True,
    )


def main() -> int:
    processes: List[subprocess.Popen] = []

    def _shutdown(signum: int, _frame) -> None:
        for proc in processes:
            if proc.poll() is None:
                try:
                    os.killpg(proc.pid, signal.SIGTERM)
                except ProcessLookupError:
                    pass
        for proc in processes:
            try:
                proc.wait(timeout=10)
            except subprocess.TimeoutExpired:
                try:
                    os.killpg(proc.pid, signal.SIGKILL)
                except ProcessLookupError:
                    pass
        sys.exit(128 + signum)

    signal.signal(signal.SIGINT, _shutdown)
    signal.signal(signal.SIGTERM, _shutdown)

    if not MOSQUITTO_CONF.exists():
        print(f"Mosquitto config not found: {MOSQUITTO_CONF}", file=sys.stderr)
        return 1

    try:
        processes.append(
            _start_process([
                "mosquitto",
                "-c",
                str(MOSQUITTO_CONF),
                "-v",
            ], MOSQUITTO_DIR)
        )

        processes.append(
            _start_process([
                sys.executable,
                str(ROOT_DIR / "mqtt_to_ros2.py"),
            ], ROOT_DIR)
        )

        processes.append(
            _start_process([
                "ros2",
                "launch",
                "foxglove_bridge",
                "foxglove_bridge_launch.xml",
            ], ROOT_DIR)
        )

        for proc in processes:
            proc.wait()

    except FileNotFoundError as exc:
        print(f"Command not found: {exc}", file=sys.stderr)
        return 1
    finally:
        for proc in processes:
            if proc.poll() is None:
                try:
                    os.killpg(proc.pid, signal.SIGTERM)
                except ProcessLookupError:
                    pass

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
