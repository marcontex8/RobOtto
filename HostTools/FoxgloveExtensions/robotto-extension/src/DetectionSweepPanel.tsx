// Custom panel: top-down polar sweep view.
// Subscribes to /RobOtto/detection/distance_m and /RobOtto/detection/servo_angle,
// accumulates up to MAX_POINTS and draws them on a canvas as a sonar/lidar-style display.

import { PanelExtensionContext, RenderState, MessageEvent } from "@foxglove/extension";
import { useEffect, useLayoutEffect, useRef, useState } from "react";
import { createRoot } from "react-dom/client";

type Float32Msg = { data: number };
type ScanPoint = { angle_deg: number; distance_m: number };

const MAX_POINTS = 600;
const MAX_RANGE_M = 3.5;

// ---------------------------------------------------------------------------
// Pure drawing function – no React state, only reads from refs / arguments.
// ---------------------------------------------------------------------------
function drawScan(
  canvas: HTMLCanvasElement,
  points: ScanPoint[],
  sweepAngle: number,
  latestDist: number,
): void {
  const ctx = canvas.getContext("2d");
  if (!ctx) return;

  const w = canvas.width;
  const h = canvas.height;
  const cx = w / 2;
  const cy = h * 0.88;
  const scale = (Math.min(w, h) * 0.80) / MAX_RANGE_M;

  // Background
  ctx.fillStyle = "#0d1117";
  ctx.fillRect(0, 0, w, h);

  // Grid arcs
  ctx.font = `${Math.max(10, Math.round(w * 0.022))}px monospace`;
  for (let r = 0.5; r <= MAX_RANGE_M; r += 0.5) {
    ctx.strokeStyle = r % 1 === 0 ? "#2a3a4a" : "#171e26";
    ctx.lineWidth = r % 1 === 0 ? 1 : 0.5;
    ctx.beginPath();
    ctx.arc(cx, cy, r * scale, Math.PI, 2 * Math.PI);
    ctx.stroke();
    if (r % 1 === 0) {
      ctx.fillStyle = "#3a5570";
      ctx.fillText(`${r}m`, cx + r * scale + 3, cy - 3);
    }
  }

  // Radial guide lines every 30°
  for (let a = -90; a <= 90; a += 30) {
    const rad = (a * Math.PI) / 180;
    const ex = cx + Math.sin(rad) * MAX_RANGE_M * scale;
    const ey = cy - Math.cos(rad) * MAX_RANGE_M * scale;
    ctx.strokeStyle = "#1e2a38";
    ctx.lineWidth = 0.8;
    ctx.beginPath();
    ctx.moveTo(cx, cy);
    ctx.lineTo(ex, ey);
    ctx.stroke();
    ctx.fillStyle = "#3a5570";
    ctx.fillText(`${a}°`, ex + (a < 0 ? -24 : a === 0 ? -8 : 3), ey + 4);
  }

  // Baseline
  ctx.strokeStyle = "#2a3a4a";
  ctx.lineWidth = 1;
  ctx.beginPath();
  ctx.moveTo(cx - MAX_RANGE_M * scale, cy);
  ctx.lineTo(cx + MAX_RANGE_M * scale, cy);
  ctx.stroke();

  // Sweep line
  const sr = (sweepAngle * Math.PI) / 180;
  ctx.strokeStyle = "rgba(80, 200, 120, 0.28)";
  ctx.lineWidth = 2;
  ctx.beginPath();
  ctx.moveTo(cx, cy);
  ctx.lineTo(cx + Math.sin(sr) * MAX_RANGE_M * scale, cy - Math.cos(sr) * MAX_RANGE_M * scale);
  ctx.stroke();

  // Detection points – fade older ones using index
  const n = points.length;
  for (let i = 0; i < n; i++) {
    const pt = points[i]!;
    const rad = (pt.angle_deg * Math.PI) / 180;
    const px = cx + pt.distance_m * scale * Math.sin(rad);
    const py = cy - pt.distance_m * scale * Math.cos(rad);
    const alpha = 0.25 + 0.75 * (i / n);
    ctx.beginPath();
    ctx.arc(px, py, 3, 0, 2 * Math.PI);
    ctx.fillStyle = `rgba(226, 95, 78, ${alpha})`;
    ctx.fill();
  }

  // Robot origin (blue dot)
  ctx.beginPath();
  ctx.arc(cx, cy, 5, 0, 2 * Math.PI);
  ctx.fillStyle = "#4e98e2";
  ctx.fill();

  // HUD text
  const fs = Math.max(10, Math.round(w * 0.024));
  ctx.font = `${fs}px monospace`;
  ctx.fillStyle = "#4a6a8a";
  ctx.fillText(`pts: ${n}`, 8, fs + 4);
  ctx.fillText(`angle: ${sweepAngle.toFixed(1)}°`, 8, fs * 2 + 6);
  ctx.fillText(`dist: ${latestDist.toFixed(2)} m`, 8, fs * 3 + 8);
}

// ---------------------------------------------------------------------------
// React component
// ---------------------------------------------------------------------------
function DetectionSweepPanel({ context }: { context: PanelExtensionContext }) {
  const [renderDone, setRenderDone] = useState<(() => void) | undefined>();
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const wrapperRef = useRef<HTMLDivElement>(null);
  const scanPoints = useRef<ScanPoint[]>([]);
  const latestDist = useRef(0);
  const sweepAngle = useRef(0);
  const prevAngle = useRef<number | null>(null);
  const prevDelta = useRef(0);

  // Resize canvas pixel buffer when the panel container resizes.
  useEffect(() => {
    const wrapper = wrapperRef.current;
    const canvas = canvasRef.current;
    if (!wrapper || !canvas) return;
    const observer = new ResizeObserver(() => {
      canvas.width = wrapper.clientWidth;
      canvas.height = wrapper.clientHeight;
    });
    observer.observe(wrapper);
    return () => observer.disconnect();
  }, []);

  // Subscribe to topics and redraw on every render tick.
  useLayoutEffect(() => {
    context.onRender = (renderState, done) => {
      setRenderDone(() => done);

      for (const ev of (renderState.currentFrame ?? []) as readonly MessageEvent<Float32Msg>[]) {
        if (ev.topic === "/RobOtto/detection/distance_m") {
          latestDist.current = ev.message.data;
        } else if (ev.topic === "/RobOtto/detection/servo_angle") {
          const invertedAngle = -ev.message.data;
          sweepAngle.current = invertedAngle;

          // Detect sweep reversal and clear old points
          if (prevAngle.current !== null) {
            const delta = invertedAngle - prevAngle.current;
            if (delta !== 0) {
              if (prevDelta.current !== 0 && Math.sign(delta) !== Math.sign(prevDelta.current)) {
                scanPoints.current = [];
              }
              prevDelta.current = delta;
            }
          }
          prevAngle.current = invertedAngle;

          // Keep last MAX_POINTS samples
          if (scanPoints.current.length >= MAX_POINTS) {
            scanPoints.current.splice(0, scanPoints.current.length - MAX_POINTS + 1);
          }
          scanPoints.current.push({
            angle_deg: invertedAngle,
            distance_m: latestDist.current,
          });
        }
      }

      if (canvasRef.current) {
        drawScan(canvasRef.current, scanPoints.current, sweepAngle.current, latestDist.current);
      }
    };

    context.watch("currentFrame");
    context.subscribe([
      { topic: "/RobOtto/detection/distance_m" },
      { topic: "/RobOtto/detection/servo_angle" },
    ]);
  }, [context]);

  useEffect(() => {
    renderDone?.();
  }, [renderDone]);

  return (
    <div
      ref={wrapperRef}
      style={{ width: "100%", height: "100%", background: "#0d1117", overflow: "hidden" }}
    >
      <canvas ref={canvasRef} style={{ display: "block" }} />
    </div>
  );
}

// ---------------------------------------------------------------------------
// Panel entry point registered in index.ts
// ---------------------------------------------------------------------------
export function initDetectionSweepPanel(context: PanelExtensionContext): () => void {
  const root = createRoot(context.panelElement);
  root.render(<DetectionSweepPanel context={context} />);
  return () => root.unmount();
}
