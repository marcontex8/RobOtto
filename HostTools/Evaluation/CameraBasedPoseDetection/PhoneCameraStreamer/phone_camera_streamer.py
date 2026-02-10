#!/usr/bin/env python3
import asyncio
import json
import ssl
import subprocess
import threading
from pathlib import Path
from typing import Optional

import cv2
import rclpy
from aiohttp import web
from aiortc import RTCPeerConnection, RTCSessionDescription
from cv_bridge import CvBridge
from rclpy.executors import MultiThreadedExecutor
from rclpy.node import Node
from sensor_msgs.msg import CameraInfo, Image

BASE_DIR = Path(__file__).resolve().parent
INDEX_PATH = BASE_DIR / "index.html"
STYLES_PATH = BASE_DIR / "styles.css"


def _read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


class PhoneCameraStreamer(Node):
    def __init__(self) -> None:
        super().__init__("phone_camera_streamer")
        self.declare_parameter("topic", "/phone_camera/image_raw")
        self.declare_parameter("camera_info_topic", "/phone_camera/camera_info")
        self.declare_parameter("frame_id", "camera")
        self.declare_parameter("publish_camera_info", False)
        self.declare_parameter("http_port", 8080)

        self.bridge = CvBridge()
        self.image_pub = self.create_publisher(
            Image,
            self.get_parameter("topic").get_parameter_value().string_value,
            10,
        )
        self.camera_info_pub = self.create_publisher(
            CameraInfo,
            self.get_parameter("camera_info_topic").get_parameter_value().string_value,
            10,
        )

        self._pcs = set()
        self._last_info: Optional[CameraInfo] = None
        self._frame_id = self.get_parameter("frame_id").get_parameter_value().string_value
        self._publish_camera_info = (
            self.get_parameter("publish_camera_info").get_parameter_value().bool_value
        )

    async def handle_index(self, request: web.Request) -> web.Response:
        return web.Response(text=_read_text(INDEX_PATH), content_type="text/html")

    async def handle_styles(self, request: web.Request) -> web.Response:
        return web.Response(text=_read_text(STYLES_PATH), content_type="text/css")

    async def handle_offer(self, request: web.Request) -> web.Response:
        params = await request.json()
        offer = RTCSessionDescription(sdp=params["sdp"], type=params["type"])

        pc = RTCPeerConnection()
        self._pcs.add(pc)
        self.get_logger().info("Peer connected")

        @pc.on("connectionstatechange")
        async def on_connectionstatechange() -> None:
            if pc.connectionState in {"failed", "closed", "disconnected"}:
                await pc.close()
                self._pcs.discard(pc)
                self.get_logger().info("Peer disconnected")

        @pc.on("track")
        def on_track(track) -> None:
            if track.kind == "video":
                asyncio.create_task(self._consume_video(track))

        await pc.setRemoteDescription(offer)
        answer = await pc.createAnswer()
        await pc.setLocalDescription(answer)

        return web.Response(
            content_type="application/json",
            text=json.dumps(
                {"sdp": pc.localDescription.sdp, "type": pc.localDescription.type}
            ),
        )

    async def _consume_video(self, track) -> None:
        while True:
            frame = await track.recv()
            image = frame.to_ndarray(format="bgr24")

            image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
            encoding = "mono8"

            msg = self.bridge.cv2_to_imgmsg(image, encoding=encoding)
            msg.header.stamp = self.get_clock().now().to_msg()
            msg.header.frame_id = self._frame_id
            self.image_pub.publish(msg)

            if self._publish_camera_info:
                self._publish_info(image)

    def _publish_info(self, image) -> None:
        height, width = image.shape[:2]
        if (
            self._last_info is None
            or self._last_info.width != width
            or self._last_info.height != height
        ):
            info = CameraInfo()
            info.width = width
            info.height = height
            info.distortion_model = "plumb_bob"
            info.k = [1.0, 0.0, width / 2.0, 0.0, 1.0, height / 2.0, 0.0, 0.0, 1.0]
            info.d = [0.0, 0.0, 0.0, 0.0, 0.0]
            info.r = [1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0]
            info.p = [
                1.0,
                0.0,
                width / 2.0,
                0.0,
                0.0,
                1.0,
                height / 2.0,
                0.0,
                0.0,
                0.0,
                1.0,
                0.0,
            ]
            self._last_info = info
        else:
            info = self._last_info

        info.header.stamp = self.get_clock().now().to_msg()
        info.header.frame_id = self._frame_id
        self.camera_info_pub.publish(info)


async def run_server(node: PhoneCameraStreamer) -> None:
    app = web.Application()
    app.router.add_get("/", node.handle_index)
    app.router.add_get("/styles.css", node.handle_styles)
    app.router.add_post("/offer", node.handle_offer)

    runner = web.AppRunner(app)
    await runner.setup()
    port = node.get_parameter("http_port").get_parameter_value().integer_value
    cert_path = BASE_DIR / "cert.pem"
    key_path = BASE_DIR / "key.pem"

    if not cert_path.exists() or not key_path.exists():
        node.get_logger().info("Generating self-signed TLS certificate...")
        subprocess.run(
            [
                "openssl",
                "req",
                "-x509",
                "-newkey",
                "rsa:2048",
                "-keyout",
                str(key_path),
                "-out",
                str(cert_path),
                "-days",
                "365",
                "-nodes",
                "-subj",
                "/CN=phone-camera",
            ],
            check=True,
        )

    ssl_ctx = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
    ssl_ctx.load_cert_chain(certfile=str(cert_path), keyfile=str(key_path))

    site = web.TCPSite(runner, "0.0.0.0", port, ssl_context=ssl_ctx)
    await site.start()

    node.get_logger().info("Open the page on your phone: https://<laptop-ip>:" + str(port))

    while rclpy.ok():
        await asyncio.sleep(0.5)


def main() -> None:
    rclpy.init()
    node = PhoneCameraStreamer()

    executor = MultiThreadedExecutor()
    executor.add_node(node)
    spin_thread = threading.Thread(target=executor.spin, daemon=True)
    spin_thread.start()

    try:
        asyncio.run(run_server(node))
    except KeyboardInterrupt:
        pass
    finally:
        executor.shutdown()
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
