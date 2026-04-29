/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#ifndef PLATFORMINDEPENDENT_SRC_SERIALIZER_H
#define PLATFORMINDEPENDENT_SRC_SERIALIZER_H

#include "robotto_common.h"
#include <stddef.h>
#include <stdint.h>

int serializePoseBinary(const RobottoPose *pose, uint8_t *out, int out_size);
int serializeSpeedSetPointBinary(const WheelSpeedSetPoint *speed_set_point,
                                 uint8_t *out,
                                 int out_size);
int serializeDetectionTelemetryBinary(const RobottoDetectionTelemetry *detection_telemetry,
                                      uint8_t *out,
                                      int out_size);

#endif /* PLATFORMINDEPENDENT_SRC_SERIALIZER_H */
