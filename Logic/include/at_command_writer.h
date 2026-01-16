/*
 * at_command_writer.h
 *
 *  Created on: Jan 16, 2026
 *      Author: marco
 */

#ifndef INCLUDE_AT_COMMAND_WRITER_H_
#define INCLUDE_AT_COMMAND_WRITER_H_

#include "robotto_common.h"

int jsonFromPose(const RobottoPose *pose,
                            char *out,
                            int out_size);

int atMqttPubFromPose(const RobottoPose *pose,
                      const char *topic,
                      char *out,
                      int out_size);

#endif /* AT_COMMAND */
