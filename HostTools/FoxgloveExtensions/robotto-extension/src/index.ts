import { ExtensionContext } from "@foxglove/extension";
import { targetPoseConverter } from "./TargetPoseConverter";
import { detectionPointConverter } from "./DetectionPointConverter";
import { initDetectionSweepPanel } from "./DetectionSweepPanel";

export function activate(extensionContext: ExtensionContext): void {
  // Converts /RobOtto/target_pose → foxglove.SceneUpdate (blue flat square + label).
  // Enable it in the 3D panel: topic settings → /RobOtto/target_pose → Schema: foxglove.SceneUpdate
  extensionContext.registerMessageConverter({
    fromSchemaName: "geometry_msgs/msg/PoseStamped",
    toSchemaName: "foxglove.SceneUpdate",
    converter: targetPoseConverter,
  });

  // Converts /RobOtto/detection/point → foxglove.SceneUpdate (red sphere, unique id per sample).
  // Each sample gets a unique entity id so detections accumulate in the 3D panel.
  // Enable it in the 3D panel: topic settings → /RobOtto/detection/point → Schema: foxglove.SceneUpdate
  extensionContext.registerMessageConverter({
    fromSchemaName: "geometry_msgs/msg/PointStamped",
    toSchemaName: "foxglove.SceneUpdate",
    converter: detectionPointConverter,
  });

  // Top-down polar sweep panel: add it from the Foxglove panel menu → "RobOtto Detection Sweep"
  extensionContext.registerPanel({
    name: "RobOtto Detection Sweep",
    initPanel: initDetectionSweepPanel,
  });
}
