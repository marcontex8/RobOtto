#!/usr/bin/env python3

import cv2
import cv2.aruco as aruco
import numpy as np
import os
import sys
from pathlib import Path
import logging

# Setup logging
logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger('aruco_test_detector')

class ArucoTestDetector:
    def __init__(self, marker_size=0.05, aruco_dict_name='DICT_5X5_50', output_dir='./aruco_detections', display_scale=0.5):
        self.marker_size = marker_size
        self.output_dir = output_dir
        self.display_scale = display_scale
        
        # Create output directory
        os.makedirs(output_dir, exist_ok=True)
        
        # Initialize ArUco detector
        try:
            logger.info(f'Initializing ArUco detector with dictionary: {aruco_dict_name}')
            self.aruco_dict = aruco.getPredefinedDictionary(getattr(aruco, aruco_dict_name))
            self.aruco_params = aruco.DetectorParameters()
            self.detector = aruco.ArucoDetector(self.aruco_dict, self.aruco_params)
            logger.info('ArUco detector initialized successfully')
        except Exception as e:
            logger.error(f'Failed to initialize ArUco detector: {e}')
            raise
    
    def detect_markers_in_image(self, image_path):
        """Detect ArUco markers in a single image."""
        logger.info(f'Processing: {image_path}')
        
        try:
            # Read image
            img = cv2.imread(str(image_path))
            if img is None:
                logger.error(f'Failed to load image: {image_path}')
                return None
            #resize image to 800*600 for faster processing
            img = cv2.resize(img, (600, 800))
            logger.debug(f'Image shape: {img.shape}')
            
            # Convert to grayscale if needed
            if len(img.shape) == 3:
                gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
            else:
                gray = img
            
            # Detect markers
            corners, ids, rejected = self.detector.detectMarkers(gray)
            
            num_detected = len(ids) if ids is not None else 0
            num_rejected = len(rejected) if rejected is not None else 0
            
            logger.info(f'Detection results - Detected: {num_detected}, Rejected: {num_rejected}')
            
            # Prepare result image
            result_img = img.copy() if len(img.shape) == 3 else cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)
            
            if ids is not None and len(ids) > 0:
                # Draw detected markers
                result_img = cv2.aruco.drawDetectedMarkers(result_img, corners, ids)
                
                # Log marker details
                for i, marker_id in enumerate(ids.flatten()):
                    logger.debug(f'Marker {marker_id}: corners shape = {corners[i].shape}')
            else:
                logger.warning('No markers detected in this image')
            
            # Draw rejected markers in red (optional)
            if rejected is not None and len(rejected) > 0:
                for corner in rejected:
                    pts = corner[0].astype(int)
                    cv2.polylines(result_img, [pts], True, (0, 0, 255), 2)
                logger.debug(f'Drew {len(rejected)} rejected candidates')
            
            return {
                'image': img,
                'gray': gray,
                'result': result_img,
                'corners': corners,
                'ids': ids,
                'rejected': rejected,
                'num_detected': num_detected,
                'num_rejected': num_rejected
            }
            
        except Exception as e:
            logger.error(f'Error processing image {image_path}: {e}')
            return None
    
    def process_folder(self, folder_path, display=True, save_results=True):
        """Process all images in a folder."""
        folder = Path(folder_path)
        
        if not folder.exists():
            logger.error(f'Folder does not exist: {folder_path}')
            return
        
        # Supported image extensions
        image_extensions = {'.jpg', '.jpeg', '.png', '.bmp', '.tiff'}
        image_files = [f for f in folder.iterdir() 
                      if f.suffix.lower() in image_extensions]
        
        if not image_files:
            logger.warning(f'No image files found in {folder_path}')
            return
        
        logger.info(f'Found {len(image_files)} images to process')
        
        total_detected = 0
        detection_count = 0
        
        # Create resizable window if displaying
        if display:
            window_name = 'ArUco Detection Result'
            cv2.namedWindow(window_name, cv2.WINDOW_NORMAL)
            cv2.resizeWindow(window_name, 800, 600)
        
        for image_file in sorted(image_files):
            result = self.detect_markers_in_image(image_file)
            
            if result is None:
                continue
            
            if result['num_detected'] > 0:
                detection_count += 1
                total_detected += result['num_detected']
            
            # Display result
            if display and result['num_detected'] > 0:
                # Resize for display
                display_img = result['result']
                if self.display_scale != 1.0:
                    h, w = display_img.shape[:2]
                    new_w = int(w * self.display_scale)
                    new_h = int(h * self.display_scale)
                    display_img = cv2.resize(display_img, (new_w, new_h), interpolation=cv2.INTER_LINEAR)
                
                cv2.imshow(window_name, display_img)
                key = cv2.waitKey(0)  # Press any key to continue
                if key == 27:  # ESC key
                    logger.info('User cancelled display')
                    display = False
            
            # Save result
            if save_results:
                output_filename = f"detected_{image_file.stem}.png"
                output_path = os.path.join(self.output_dir, output_filename)
                cv2.imwrite(output_path, result['result'])
                logger.info(f'Saved result: {output_path}')
        
        cv2.destroyAllWindows()
        
        # Print summary statistics
        logger.info('\n' + '='*60)
        logger.info('DETECTION SUMMARY')
        logger.info('='*60)
        logger.info(f'Total images processed: {len(image_files)}')
        logger.info(f'Images with detections: {detection_count}')
        logger.info(f'Total markers detected: {total_detected}')
        if detection_count > 0:
            logger.info(f'Average markers per detection: {total_detected / detection_count:.2f}')
        logger.info('='*60 + '\n')

def main():
    import argparse
    
    parser = argparse.ArgumentParser(
        description='Test ArUco marker detection on a folder of images')
    parser.add_argument('folder', help='Path to folder containing images')
    parser.add_argument('--marker-size', type=float, default=0.05,
                       help='Marker size in meters (default: 0.05)')
    parser.add_argument('--dict', default='DICT_5X5_50',
                       help='ArUco dictionary to use (default: DICT_5X5_50)')
    parser.add_argument('--output', default='./aruco_detections',
                       help='Output directory for results (default: ./aruco_detections)')
    parser.add_argument('--scale', type=float, default=0.5,
                       help='Display scale factor (default: 0.5). Use 1.0 for full size')
    parser.add_argument('--no-display', action='store_true',
                       help='Do not display detection results')
    parser.add_argument('--no-save', action='store_true',
                       help='Do not save detection results')
    
    args = parser.parse_args()
    
    # Create detector
    detector = ArucoTestDetector(
        marker_size=args.marker_size,
        aruco_dict_name=args.dict,
        output_dir=args.output,
        display_scale=args.scale
    )
    
    # Process folder
    detector.process_folder(
        args.folder,
        display=not args.no_display,
        save_results=not args.no_save
    )

if __name__ == '__main__':
    main()
