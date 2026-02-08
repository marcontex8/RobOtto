/*
 * json_writer.c
 *
 *  Created on: Jan 16, 2026
 *      Author: marco
 */

#include <stdio.h>
#include <string.h>

#include "at_command_writer.h"

#define TELEMETRY_BINARY_VERSION 2u
#define TELEMETRY_POSE_BINARY_SIZE 17
#define TELEMETRY_V2_BINARY_SIZE 42

static int write_u32_le(uint8_t *out, int out_size, uint32_t value)
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

static int write_float_le(uint8_t *out, int out_size, float value)
{
    uint32_t bits = 0;
    memcpy(&bits, &value, sizeof(bits));
    return write_u32_le(out, out_size, bits);
}

static int serializeTelemetryBinary(const RobottoPose *pose,
                                    const RobottoPose *target_pose,
                                    const WheelSpeedSetPoint *speed_set_point,
                                    uint8_t *out,
                                    int out_size)
{
    int off = 0;

    if (out_size < TELEMETRY_V2_BINARY_SIZE) {
        return -1;
    }

    out[off++] = (uint8_t)TELEMETRY_BINARY_VERSION;

    int written = write_u32_le(out + off, out_size - off, (uint32_t)pose->timestamp);
    if (written < 0) {
        return -1;
    }
    off += written;

    written = write_float_le(out + off, out_size - off, pose->x);
    if (written < 0) {
        return -1;
    }
    off += written;

    written = write_float_le(out + off, out_size - off, pose->y);
    if (written < 0) {
        return -1;
    }
    off += written;

    written = write_float_le(out + off, out_size - off, pose->theta);
    if (written < 0) {
        return -1;
    }
    off += written;

    written = write_u32_le(out + off, out_size - off, (uint32_t)target_pose->timestamp);
    if (written < 0) {
        return -1;
    }
    off += written;

    written = write_float_le(out + off, out_size - off, target_pose->x);
    if (written < 0) {
        return -1;
    }
    off += written;

    written = write_float_le(out + off, out_size - off, target_pose->y);
    if (written < 0) {
        return -1;
    }
    off += written;

    written = write_float_le(out + off, out_size - off, target_pose->theta);
    if (written < 0) {
        return -1;
    }
    off += written;

    written = write_float_le(out + off, out_size - off, speed_set_point->left);
    if (written < 0) {
        return -1;
    }
    off += written;

    written = write_float_le(out + off, out_size - off, speed_set_point->right);
    if (written < 0) {
        return -1;
    }
    off += written;

    out[off++] = speed_set_point->active ? 1u : 0u;

    return off;
}

static int base64Encode(const uint8_t *in, int in_len, char *out, int out_size)
{
    static const char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int out_len = 4 * ((in_len + 2) / 3);

    if (out_size < (out_len + 1)) {
        return -1;
    }

    int i = 0;
    int j = 0;
    while (i < in_len) {
        uint32_t octet_a = i < in_len ? in[i++] : 0u;
        uint32_t octet_b = i < in_len ? in[i++] : 0u;
        uint32_t octet_c = i < in_len ? in[i++] : 0u;

        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        out[j++] = table[(triple >> 18) & 0x3Fu];
        out[j++] = table[(triple >> 12) & 0x3Fu];
        out[j++] = table[(triple >> 6) & 0x3Fu];
        out[j++] = table[triple & 0x3Fu];
    }

    int mod = in_len % 3;
    if (mod > 0) {
        out[out_len - 1] = '=';
        if (mod == 1) {
            out[out_len - 2] = '=';
        }
    }

    out[out_len] = '\0';
    return out_len;
}

int atMqttPubFromTelemetry(const RobottoPose *pose,
                      const RobottoPose *target_pose,
                      const WheelSpeedSetPoint *speed_set_point,
                      const char *topic,
                      char *out,
                      int max_out_size)
{
    int off = 0;

    uint8_t payload[TELEMETRY_V2_BINARY_SIZE];
    int payload_len = serializeTelemetryBinary(pose, target_pose, speed_set_point, payload, sizeof(payload));
    if (payload_len < 0) {
        return -1;
    }

    char payload_b64[96];
    int payload_b64_len = base64Encode(payload, payload_len, payload_b64, sizeof(payload_b64));
    if (payload_b64_len < 0) {
        return -1;
    }

    off += snprintf(out, max_out_size,
                    "AT+MQTTPUB=0,\"%s\",\"",
                    topic);

    off += snprintf(out + off, max_out_size - off,
                    "%s",
                    payload_b64);

    off += snprintf(out + off, max_out_size - off,
                    "\",0,0");

    return off;
}
