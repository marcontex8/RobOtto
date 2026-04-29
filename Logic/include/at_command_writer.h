/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#ifndef LOGIC_INCLUDE_AT_COMMAND_WRITER_H
#define LOGIC_INCLUDE_AT_COMMAND_WRITER_H

#include "serializer.h"
#include "robotto_common.h"

int getATMqttPubTelemetryMessage(const RobottoPose *pose,
                      const RobottoPose *target_pose,
                      const WheelSpeedSetPoint *speed_set_point,
                      const RobottoDetectionTelemetry *detection_telemetry,
                      const char *topic,
                      char *out,
                      int max_out_size);

#endif /* LOGIC_INCLUDE_AT_COMMAND_WRITER_H */
