/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#ifndef PLATFORMINDEPENDENT_INCLUDE_AT_COMMAND_WRITER_H
#define PLATFORMINDEPENDENT_INCLUDE_AT_COMMAND_WRITER_H

#include "robotto_common.h"
#include "serializer.h"

int makeATMQTTPubTelemetryMessage(const RobottoPose *pose,
                                  const RobottoPose *target_pose,
                                  const WheelSpeedSetPoint *speed_set_point,
                                  const RobottoDetectionTelemetry *detection_telemetry,
                                  const char *topic,
                                  char *out,
                                  int max_out_size);

#endif /* PLATFORMINDEPENDENT_INCLUDE_AT_COMMAND_WRITER_H */
