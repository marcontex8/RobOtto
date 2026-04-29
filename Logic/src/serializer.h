/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#ifndef LOGIC_SRC_SERIALIZER_H
#define LOGIC_SRC_SERIALIZER_H

#include <stdint.h>
#include <stddef.h>
#include "robotto_common.h"

int serializePoseBinary(const RobottoPose *pose, uint8_t *out, int out_size);
int serializeSpeedSetPointBinary(const WheelSpeedSetPoint *speed_set_point, uint8_t *out, int out_size);
int serializeDetectionTelemetryBinary(const RobottoDetectionTelemetry *detection_telemetry, uint8_t *out, int out_size);

#endif /* LOGIC_SRC_SERIALIZER_H */
