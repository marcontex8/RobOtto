
/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#include "serializer.h"
#include <string.h>

int write_u32_le(uint8_t *out, int out_size, uint32_t value)
{
	if (out_size < 4) {
		return -1;
	}
	out[0] = (uint8_t)(value & 0xFFu);
	out[1] = (uint8_t)((value >> 8) & 0xFFu);
	out[2] = (uint8_t)((value >> 16) & 0xFFu);
	out[3] = (uint8_t)((value >> 24) & 0xFFu);
	return 4;
}

int write_float_le(uint8_t *out, int out_size, float value)
{
	uint32_t bits = 0;
	memcpy(&bits, &value, sizeof(bits));
	return write_u32_le(out, out_size, bits);
}

int serializePoseBinary(const RobottoPose *pose, uint8_t *out, int out_size)
{
	int off = 0;
	int written = write_u32_le(out + off, out_size - off, (uint32_t)pose->timestamp);
	if (written < 0) return -1;
	off += written;
	written = write_float_le(out + off, out_size - off, pose->x);
	if (written < 0) return -1;
	off += written;
	written = write_float_le(out + off, out_size - off, pose->y);
	if (written < 0) return -1;
	off += written;
	written = write_float_le(out + off, out_size - off, pose->theta);
	if (written < 0) return -1;
	off += written;
	return off;
}

int serializeSpeedSetPointBinary(const WheelSpeedSetPoint *speed_set_point, uint8_t *out, int out_size)
{
	int off = 0;
	int written = write_float_le(out + off, out_size - off, speed_set_point->left);
	if (written < 0) return -1;
	off += written;
	written = write_float_le(out + off, out_size - off, speed_set_point->right);
	if (written < 0) return -1;
	off += written;
	if (out_size - off < 1) return -1;
	out[off++] = speed_set_point->active ? 1u : 0u;
	return off;
}

int serializeDetectionTelemetryBinary(const RobottoDetectionTelemetry *detection_telemetry, uint8_t *out, int out_size)
{
	int off = 0;
	int written = write_u32_le(out + off, out_size - off, (uint32_t)detection_telemetry->timestamp);
	if (written < 0) return -1;
	off += written;
	written = write_float_le(out + off, out_size - off, detection_telemetry->distance_m);
	if (written < 0) return -1;
	off += written;
	written = write_float_le(out + off, out_size - off, detection_telemetry->servo_angle);
	if (written < 0) return -1;
	off += written;
	return off;
}
