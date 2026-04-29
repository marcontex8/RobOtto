/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#ifndef COMMON_INCLUDE_ROBOTTO_COMMON_H
#define COMMON_INCLUDE_ROBOTTO_COMMON_H

#define TickType_t uint32_t

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum
{
    ROBOTTO_OK = 0,
    ROBOTTO_ERROR = 1,
} RobottoErrorCode;

typedef enum
{
    ACTIVITY_STATUS_INIT = 0,
    ACTIVITY_STATUS_RUNNING,
    ACTIVITY_STATUS_ERROR
} ActivityStatus;

typedef struct
{
    float left;
    float right;
    bool active;
} WheelSpeedSetPoint;

typedef struct
{
    TickType_t timestamp;
    float delta_angle_left;
    float delta_angle_right;
} WheelsMovementUpdate;

typedef struct
{
    TickType_t timestamp;
    float x;
    float y;
    float theta;
} RobottoPose;

typedef struct
{
    TickType_t timestamp;
    float distance_m;
    float servo_angle;
} RobottoDetectionTelemetry;

typedef struct
{
    TickType_t timestamp;
    float acc_x;
    float acc_y;
    float acc_z;

    float gyro_x;
    float gyro_y;
    float gyro_z;
} ImuData;

typedef enum
{
    ROBOTTO_BEHAVIOR_IDLE,
    ROBOTTO_BEHAVIOR_RUNNING,
} RobottoBehavior;

typedef struct
{
    RobottoPose pose;
    RobottoPose target_pose;
    WheelSpeedSetPoint speed_set_point;
} RobottoMotionTelemetry;

typedef struct
{
    RobottoMotionTelemetry motion_telemetry;
    RobottoDetectionTelemetry detection_telemetry;
} RobottoAggregatedTelemetry;

#ifdef DEBUG
void SYSVIEW_PrintLines(const char *buf);
#define PRINT_LINES_ON_SYSTEMVIEW(buffer) SYSVIEW_PrintLines(buffer)
#else
#define PRINT_LINES_ON_SYSTEMVIEW(buffer) ((void)0)
#endif

#ifdef DEBUG
#define ROBOTTO_ASSERT_DEBUG(expr) \
    do                             \
    {                              \
        configASSERT(expr);        \
    } while (0)
#else
#define ROBOTTO_ASSERT_DEBUG(expr) ((void)0)
#endif

#endif /* COMMON_INCLUDE_ROBOTTO_COMMON_H */
