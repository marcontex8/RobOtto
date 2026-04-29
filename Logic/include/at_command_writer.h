/*
 * at_command_writer.h
 *
 *  Created on: Jan 16, 2026
 *      Author: marco
 */

#ifndef INCLUDE_AT_COMMAND_WRITER_H_
#define INCLUDE_AT_COMMAND_WRITER_H_

#include "serializer.h"
#include "robotto_common.h"

int getATMqttPubTelemetryMessage(const RobottoPose *pose,
                      const RobottoPose *target_pose,
                      const WheelSpeedSetPoint *speed_set_point,
                      const RobottoDetectionTelemetry *detection_telemetry,
                      const char *topic,
                      char *out,
                      int max_out_size);

#endif /* AT_COMMAND */
