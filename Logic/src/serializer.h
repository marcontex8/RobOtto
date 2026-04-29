/*
 * serializer.h
 *
 *  Created on: Apr 29, 2026
 *      Author: marco
 */

#ifndef LOGIC_SERIALIZER_H_
#define LOGIC_SERIALIZER_H_

#include <stdint.h>
#include <stddef.h>
#include "robotto_common.h"

int serializePoseBinary(const RobottoPose *pose, uint8_t *out, int out_size);
int serializeSpeedSetPointBinary(const WheelSpeedSetPoint *speed_set_point, uint8_t *out, int out_size);
int serializeDetectionTelemetryBinary(const RobottoDetectionTelemetry *detection_telemetry, uint8_t *out, int out_size);

#endif /* LOGIC_SERIALIZER_H_ */
