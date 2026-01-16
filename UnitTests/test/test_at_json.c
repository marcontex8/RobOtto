#include "unity.h"
#include "at_command_writer.h"


void setUp(void) {}
void tearDown(void) {}


void test_PoseStringCreation()
{
    RobottoPose pose = {0};
    char buffer[265];

    pose.x = 1.23f;
    pose.y = 4.56f;
    pose.theta = 7.89f;
    pose.timestamp = 123456789;

    jsonFromPose(&pose, buffer, sizeof(buffer));
    TEST_ASSERT_EQUAL_STRING("{\\\"timestamp\\\":123456789\\,\\\"x\\\":1.2300\\,\\\"y\\\":4.5600\\,\\\"theta\\\":7.8900}", buffer);
}

void test_PoseMqttPublicationStringCreation()
{
    RobottoPose pose = {0};
    char buffer[256];
    const char *topic = "RobOtto/pose";
    pose.x = 1.23f;
    pose.y = 4.56f;
    pose.theta = 7.89f;
    pose.timestamp = 123456789;
    int length = atMqttPubFromPose(&pose, topic, buffer, sizeof(buffer));
    const char* expected_command = "AT+MQTTPUB=0,\"RobOtto/pose\",\"{\\\"timestamp\\\":123456789\\,\\\"x\\\":1.2300\\,\\\"y\\\":4.5600\\,\\\"theta\\\":7.8900}\",0,0";
    TEST_ASSERT_EQUAL_INT((int)strlen(expected_command), length);
    TEST_ASSERT_EQUAL_STRING(expected_command, buffer);
    
}