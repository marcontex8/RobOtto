#!/usr/bin/env python3
"""Camera calibration using a 9x6 chessboard pattern.

Supports two modes:
- Image mode: supply --images (directory or glob pattern)
- Live mode: omit --images to capture frames from a webcam
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import List, Tuple

from common import find_camera_device
import cv2
import numpy as np

DEFAULT_PATTERN = (9, 6)  # columns, rows (inner corners)
CAMERA_ID_NAME = 'usb-BC-250325---X_Trust_1080P_Webcam-video-index0'


def parse_pattern(pattern_str: str) -> Tuple[int, int]:
    if "x" not in pattern_str:
        raise ValueError("Pattern must be in COLSxROWS format, e.g. 9x6")
    cols_str, rows_str = pattern_str.lower().split("x", 1)
    return int(cols_str), int(rows_str)


def iter_image_paths(images_arg: str) -> List[Path]:
    path = Path(images_arg)
    if path.is_dir():
        exts = ("*.png", "*.jpg", "*.jpeg", "*.bmp", "*.tiff")
        files = []
        for ext in exts:
            files.extend(path.glob(ext))
        return sorted(files)
    return sorted(Path().glob(images_arg))


def find_corners(
    image: np.ndarray, pattern_size: Tuple[int, int]
) -> Tuple[bool, np.ndarray | None]:
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    flags = cv2.CALIB_CB_ADAPTIVE_THRESH | cv2.CALIB_CB_NORMALIZE_IMAGE
    found, corners = cv2.findChessboardCorners(gray, pattern_size, flags)
    if not found:
        return False, None
    criteria = (
        cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER,
        30,
        0.001,
    )
    refined = cv2.cornerSubPix(gray, corners, (11, 11), (-1, -1), criteria)
    return True, refined


def calibrate_from_points(
    objpoints: List[np.ndarray],
    imgpoints: List[np.ndarray],
    image_size: Tuple[int, int],
) -> Tuple[float, np.ndarray, np.ndarray, List[np.ndarray], List[np.ndarray]]:
    return cv2.calibrateCamera(objpoints, imgpoints, image_size, None, None)


def save_calibration(
    output_path: Path,
    pattern_size: Tuple[int, int],
    square_size: float,
    image_size: Tuple[int, int],
    rms: float,
    camera_matrix: np.ndarray,
    dist_coeffs: np.ndarray,
    num_images: int,
) -> None:
    data = {
        "pattern_size": [pattern_size[0], pattern_size[1]],
        "square_size": square_size,
        "image_size": [image_size[0], image_size[1]],
        "rms": rms,
        "camera_matrix": camera_matrix.tolist(),
        "distortion_coefficients": dist_coeffs.flatten().tolist(),
        "num_images": num_images,
    }
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(json.dumps(data, indent=2))


def build_object_points(pattern_size: Tuple[int, int], square_size: float) -> np.ndarray:
    cols, rows = pattern_size
    objp = np.zeros((rows * cols, 3), np.float32)
    objp[:, :2] = np.mgrid[0:cols, 0:rows].T.reshape(-1, 2)
    objp *= square_size
    return objp


def run_image_mode(
    image_paths: List[Path],
    pattern_size: Tuple[int, int],
    square_size: float,
    visualize: bool,
) -> Tuple[List[np.ndarray], List[np.ndarray], Tuple[int, int]]:
    objpoints: List[np.ndarray] = []
    imgpoints: List[np.ndarray] = []
    image_size: Tuple[int, int] | None = None
    objp = build_object_points(pattern_size, square_size)

    for path in image_paths:
        image = cv2.imread(str(path))
        if image is None:
            continue
        if image_size is None:
            image_size = (image.shape[1], image.shape[0])
        found, corners = find_corners(image, pattern_size)
        if found and corners is not None:
            objpoints.append(objp)
            imgpoints.append(corners)
            if visualize:
                cv2.drawChessboardCorners(image, pattern_size, corners, found)
                cv2.imshow("Corners", image)
                cv2.waitKey(200)
    if visualize:
        cv2.destroyAllWindows()

    if image_size is None:
        raise RuntimeError("No readable images were found.")

    return objpoints, imgpoints, image_size


def run_live_mode(
    pattern_size: Tuple[int, int],
    square_size: float,
) -> Tuple[List[np.ndarray], List[np.ndarray], Tuple[int, int]]:
    camera_id = find_camera_device(CAMERA_ID_NAME)
    cap = cv2.VideoCapture(camera_id, cv2.CAP_V4L2)
    if not cap.isOpened():
        raise RuntimeError(f"Unable to open camera {camera_id}.")

    objpoints: List[np.ndarray] = []
    imgpoints: List[np.ndarray] = []
    objp = build_object_points(pattern_size, square_size)
    image_size: Tuple[int, int] | None = None

    print("Press SPACE to capture a frame with visible corners. Press Q to finish.")

    while True:
        ok, frame = cap.read()
        if not ok:
            break
        if image_size is None:
            image_size = (frame.shape[1], frame.shape[0])

        found, corners = find_corners(frame, pattern_size)
        display = frame.copy()
        if found and corners is not None:
            cv2.drawChessboardCorners(display, pattern_size, corners, found)
        cv2.putText(
            display,
            f"Samples: {len(objpoints)}",
            (10, 30),
            cv2.FONT_HERSHEY_SIMPLEX,
            1.0,
            (0, 255, 0),
            2,
        )
        cv2.imshow("Calibration", display)
        key = cv2.waitKey(1) & 0xFF
        if key == ord("q"):
            break
        if key == ord(" ") and found and corners is not None:
            objpoints.append(objp)
            imgpoints.append(corners)
            print(f"Captured sample {len(objpoints)}")

    cap.release()
    cv2.destroyAllWindows()

    if image_size is None:
        raise RuntimeError("No frames captured from camera.")

    return objpoints, imgpoints, image_size


def main() -> int:
    parser = argparse.ArgumentParser(description="Camera calibration using OpenCV")
    parser.add_argument(
        "--images",
        help="Directory or glob pattern of calibration images. If omitted, uses live camera.",
    )
    parser.add_argument(
        "--square-size",
        type=float,
        default=0.025,
        help="Chessboard square size in chosen units (default: 0.025 meters)",
    )
    parser.add_argument(
        "--output",
        default="calibration.json",
        help="Output JSON file path",
    )
    parser.add_argument(
        "--visualize",
        action="store_true",
        help="Show detected corners while processing images",
    )

    args = parser.parse_args()

    if args.images:
        image_paths = iter_image_paths(args.images)
        if not image_paths:
            print("No images matched the provided path/pattern.", file=sys.stderr)
            return 2
        objpoints, imgpoints, image_size = run_image_mode(
            image_paths, DEFAULT_PATTERN, args.square_size, args.visualize
        )
    else:
        objpoints, imgpoints, image_size = run_live_mode(DEFAULT_PATTERN, args.square_size)

    if len(objpoints) < 5:
        print(
            "Not enough valid samples for calibration. Capture at least 5 images.",
            file=sys.stderr,
        )
        return 2

    rms, camera_matrix, dist_coeffs, rvecs, tvecs = calibrate_from_points(
        objpoints, imgpoints, image_size
    )

    output_path = Path(args.output)
    save_calibration(
        output_path,
        DEFAULT_PATTERN,
        args.square_size,
        image_size,
        rms,
        camera_matrix,
        dist_coeffs,
        len(objpoints),
    )

    print(f"Calibration saved to: {output_path}")
    print(f"RMS reprojection error: {rms:.4f}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
