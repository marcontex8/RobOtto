#include <stdio.h>
#include <string.h>

#include "unity.h"
#include "at_command_writer.h"


void setUp(void) {}
void tearDown(void) {}


static void assertPoseMqttPublication(const RobottoPose *pose,
                                      const RobottoPose *target_pose,
                                      const WheelSpeedSetPoint *speed_set_point,
                                      const char *expected_payload)
{
    char buffer[256];
    const char *topic = "RobOtto/pose";
    int length = atMqttPubFromPose(pose, target_pose, speed_set_point, topic, buffer, sizeof(buffer));

    char expected_command[256];
    snprintf(expected_command, sizeof(expected_command),
             "AT+MQTTPUB=0,\"%s\",\"%s\",0,0",
             topic, expected_payload);

    TEST_ASSERT_EQUAL_INT((int)strlen(expected_command), length);
    TEST_ASSERT_EQUAL_STRING(expected_command, buffer);
}


void test_PoseMqttPublication_Zero()
{
    RobottoPose pose = {0};
    RobottoPose target_pose = {0};
    WheelSpeedSetPoint speed_set_point = {0};
    pose.timestamp = 0;
    pose.x = 0.0f;
    pose.y = 0.0f;
    pose.theta = 0.0f;
    assertPoseMqttPublication(&pose, &target_pose, &speed_set_point, "AgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
}

void test_PoseMqttPublication_NegativeValues()
{
    RobottoPose pose = {0};
    RobottoPose target_pose = {0};
    WheelSpeedSetPoint speed_set_point = {0};
    pose.timestamp = 42;
    pose.x = -1.5f;
    pose.y = 2.25f;
    pose.theta = -3.5f;
    target_pose.timestamp = 100;
    target_pose.x = -4.0f;
    target_pose.y = 5.5f;
    target_pose.theta = -6.5f;
    speed_set_point.left = -2.0f;
    speed_set_point.right = 3.0f;
    speed_set_point.active = true;
    assertPoseMqttPublication(&pose, &target_pose, &speed_set_point, "AioAAAAAAMC/AAAQQAAAYMBkAAAAAACAwAAAsEAAANDAAAAAwAAAQEAB");
}

void test_PoseMqttPublication_MaxTimestamp()
{
    RobottoPose pose = {0};
    RobottoPose target_pose = {0};
    WheelSpeedSetPoint speed_set_point = {0};
    pose.timestamp = 4294967295u;
    pose.x = 123.0f;
    pose.y = -45.5f;
    pose.theta = 0.125f;
    target_pose.timestamp = 7;
    target_pose.x = 1.0f;
    target_pose.y = 2.0f;
    target_pose.theta = 3.0f;
    speed_set_point.left = 10.0f;
    speed_set_point.right = -11.0f;
    speed_set_point.active = false;
    assertPoseMqttPublication(&pose, &target_pose, &speed_set_point, "Av////8AAPZCAAA2wgAAAD4HAAAAAACAPwAAAEAAAEBAAAAgQQAAMMEA");
}