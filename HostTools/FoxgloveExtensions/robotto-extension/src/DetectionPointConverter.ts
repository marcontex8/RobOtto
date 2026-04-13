// Converts /RobOtto/detection/point (PointStamped) to a foxglove.SceneUpdate
// Each detection gets a unique ID derived from its timestamp, so successive calls
// accumulate as separate entities in the 3D panel, building up the detection map.
// Register in the 3D panel via: Topics → /RobOtto/detection/point → Schema: foxglove.SceneUpdate

type Time = { sec: number; nsec: number };
type Vec3 = { x: number; y: number; z: number };
type Color = { r: number; g: number; b: number; a: number };
type Pose = { position: Vec3; orientation: { x: number; y: number; z: number; w: number } };

type PointStamped = {
  header: { stamp: Time; frame_id: string };
  point: Vec3;
};

type SpherePrimitive = { pose: Pose; size: Vec3; color: Color };

type SceneEntity = {
  timestamp: Time;
  frame_id: string;
  id: string;
  lifetime: Time;
  frame_locked: boolean;
  metadata: Record<string, string>[];
  arrows: Record<string, unknown>[];
  cubes: Record<string, unknown>[];
  spheres: SpherePrimitive[];
  cylinders: Record<string, unknown>[];
  lines: Record<string, unknown>[];
  triangles: Record<string, unknown>[];
  texts: Record<string, unknown>[];
  models: Record<string, unknown>[];
};

type SceneUpdate = {
  deletions: Record<string, unknown>[];
  entities: SceneEntity[];
};

export function detectionPointConverter(msg: PointStamped): SceneUpdate {
  // Unique id per sample: the 3D panel accumulates entities by id and keeps them
  // until they are explicitly deleted or the panel is reset.
  const id = `det_${msg.header.stamp.sec}_${msg.header.stamp.nsec}`;

  return {
    deletions: [],
    entities: [
      {
        timestamp: msg.header.stamp,
        frame_id: msg.header.frame_id,
        id,
        lifetime: { sec: 0, nsec: 0 },
        frame_locked: false,
        metadata: [],
        arrows: [],
        cubes: [],
        spheres: [
          {
            pose: {
              position: msg.point,
              orientation: { x: 0, y: 0, z: 0, w: 1 },
            },
            size: { x: 0.04, y: 0.04, z: 0.04 },
            color: { r: 0.9, g: 0.3, b: 0.2, a: 0.85 },
          },
        ],
        cylinders: [],
        lines: [],
        triangles: [],
        texts: [],
        models: [],
      },
    ],
  };
}
