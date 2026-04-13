// Converts /RobOtto/target_pose (PoseStamped) to a foxglove.SceneUpdate
// containing a flat blue square + "TARGET" text label.
// Register in the 3D panel via: Topics → /RobOtto/target_pose → Schema: foxglove.SceneUpdate

type Time = { sec: number; nsec: number };
type Vec3 = { x: number; y: number; z: number };
type Quaternion = { x: number; y: number; z: number; w: number };
type Color = { r: number; g: number; b: number; a: number };
type Pose = { position: Vec3; orientation: Quaternion };

type PoseStamped = {
  header: { stamp: Time; frame_id: string };
  pose: Pose;
};

type CubePrimitive = { pose: Pose; size: Vec3; color: Color };
type TextPrimitive = {
  pose: Pose;
  billboard: boolean;
  font_size: number;
  scale_invariant: boolean;
  color: Color;
  text: string;
};

type SceneEntity = {
  timestamp: Time;
  frame_id: string;
  id: string;
  lifetime: Time;
  frame_locked: boolean;
  metadata: Record<string, string>[];
  arrows: Record<string, unknown>[];
  cubes: CubePrimitive[];
  spheres: Record<string, unknown>[];
  cylinders: Record<string, unknown>[];
  lines: Record<string, unknown>[];
  triangles: Record<string, unknown>[];
  texts: TextPrimitive[];
  models: Record<string, unknown>[];
};

type SceneUpdate = {
  deletions: Record<string, unknown>[];
  entities: SceneEntity[];
};

export function targetPoseConverter(msg: PoseStamped): SceneUpdate {
  const { pose, header } = msg;

  return {
    deletions: [],
    entities: [
      {
        timestamp: header.stamp,
        frame_id: header.frame_id,
        // Constant id: each new message replaces the previous target marker.
        id: "robotto_target_pose",
        lifetime: { sec: 0, nsec: 0 },
        frame_locked: false,
        metadata: [],
        arrows: [],
        cubes: [
          {
            pose,
            // Flat square lying on the floor (z is very thin)
            size: { x: 0.15, y: 0.15, z: 0.01 },
            color: { r: 0.1, g: 0.45, b: 1.0, a: 0.85 },
          },
        ],
        spheres: [],
        cylinders: [],
        lines: [],
        triangles: [],
        texts: [
          {
            pose: {
              position: { x: pose.position.x, y: pose.position.y, z: 0.06 },
              orientation: { x: 0, y: 0, z: 0, w: 1 },
            },
            billboard: true,
            font_size: 0.07,
            scale_invariant: false,
            color: { r: 0.6, g: 0.82, b: 1.0, a: 1.0 },
            text: "TARGET",
          },
        ],
        models: [],
      },
    ],
  };
}
