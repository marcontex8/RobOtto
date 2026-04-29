/*
 * at_command_writer.c
 *
 *  Created on: Jan 16, 2026
 *      Author: marco
 */

#include <stdio.h>
#include <string.h>

#include "at_command_writer.h"
#include "serializer.h"

#define TELEMETRY_BINARY_VERSION 1u
#define TELEMETRY_BINARY_BUFFER_SIZE 128
#define TELEMETRY_B64_BUFFER_SIZE 128


static int serializeTelemetryBinary(const RobottoPose *pose,
                                    const RobottoPose *target_pose,
                                    const WheelSpeedSetPoint *speed_set_point,
                                    const RobottoDetectionTelemetry *detection_telemetry,
                                    uint8_t *out,
                                    int out_buffer_size)
{
    int off = 0;
    out[off++] = (uint8_t)TELEMETRY_BINARY_VERSION;

    int written = serializePoseBinary(pose, out + off, out_buffer_size - off);
    if (written < 0) return -1;
    off += written;

    written = serializePoseBinary(target_pose, out + off, out_buffer_size - off);
    if (written < 0) return -1;
    off += written;

    written = serializeSpeedSetPointBinary(speed_set_point, out + off, out_buffer_size - off);
    if (written < 0) return -1;
    off += written;

    written = serializeDetectionTelemetryBinary(detection_telemetry, out + off, out_buffer_size - off);
    if (written < 0) return -1;
    off += written;

    return off;
}

static int base64Encode(const uint8_t *in, int in_len, char *out, int out_buffer_size)
{
    static const char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int out_len = 4 * ((in_len + 2) / 3); // "+2" to round up to next multiple of 3

    if (out_buffer_size < (out_len + 1)) {
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

int getATMqttPubTelemetryMessage(const RobottoPose *pose,
                      const RobottoPose *target_pose,
                      const WheelSpeedSetPoint *speed_set_point,
                      const RobottoDetectionTelemetry *detection_telemetry,
                      const char *topic,
                      char *out,
                      int max_out_size)
{
    int off = 0;

    uint8_t payload[TELEMETRY_BINARY_BUFFER_SIZE];
    int payload_len = serializeTelemetryBinary(pose, target_pose, speed_set_point, detection_telemetry, payload, sizeof(payload));
    if (payload_len < 0) {
        return -1;
    }

    char payload_b64[TELEMETRY_B64_BUFFER_SIZE];
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
