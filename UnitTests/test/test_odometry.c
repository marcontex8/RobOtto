#include "unity.h"
#include "odometry.h"
#include <math.h>

void setUp(void) {}
void tearDown(void) {}

// testing with big angles implies we have a big approximation.
#define TOLERANCE 1e-2

void test_NoMovement()
{
    RobottoPose pose = {0};
    WheelsMovementUpdate w = { .timestamp = 0, .delta_angle_left = 0, .delta_angle_right = 0 };

    updateOdometry(&w, NULL, &pose);

    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 0.0f, pose.x);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 0.0f, pose.y);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 0.0f, pose.theta);
}

// -------------------------------------------------------------

void test_BothForward()
{
    RobottoPose pose = {0};
    WheelsMovementUpdate w = { .timestamp = 0, .delta_angle_left = 1.0f, .delta_angle_right = 1.0f};

    updateOdometry(&w, NULL, &pose);

    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 0.0f, pose.x);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 1.0f, pose.y);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 0.0f, pose.theta);
}


void test_BothBackard()
{
    RobottoPose pose = {0};
    WheelsMovementUpdate w = { .timestamp = 0, .delta_angle_left = -1.0f, .delta_angle_right = -1.0f};

    updateOdometry(&w, NULL, &pose);

    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 0.0f, pose.x);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, -1.0f, pose.y);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 0.0f, pose.theta);
}
// -------------------------------------------------------------

void test_LeftWheelForwardMovement()
{
    RobottoPose pose = {0};
    WheelsMovementUpdate w = { .timestamp = 0, .delta_angle_left = M_PI/6.0f, .delta_angle_right = 0.0f };

    updateOdometry(&w, NULL, &pose);

	TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 0.07f, pose.x);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 0.25f, pose.y);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, -M_PI/6.0f, pose.theta);
}

// -------------------------------------------------------------

void test_RightWheelForwardMovement()
{
    RobottoPose pose = {0};
    WheelsMovementUpdate w = { .timestamp = 0, .delta_angle_left = 0.0f, .delta_angle_right = M_PI/6.0f };

    updateOdometry(&w, NULL, &pose);

	TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, -0.07f, pose.x);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 0.25f, pose.y);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, M_PI/6.0f, pose.theta);
}

// -------------------------------------------------------------

void test_LeftWheelBackwardMovement()
{
    RobottoPose pose = {0};
    WheelsMovementUpdate w = { .timestamp = 0, .delta_angle_left = -M_PI/6.0f, .delta_angle_right = 0.0f };

    updateOdometry(&w, NULL, &pose);

	TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 0.07f, pose.x);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, -0.25f, pose.y);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, M_PI/6.0f, pose.theta);
}

// -------------------------------------------------------------

void test_RightWheelBackwardMovement()
{
    RobottoPose pose = {0};
    WheelsMovementUpdate w = { .timestamp = 0, .delta_angle_left = 0.0f, .delta_angle_right = -M_PI/6.0f };

    updateOdometry(&w, NULL, &pose);

	TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, -0.07f, pose.x);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, -0.25f, pose.y);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, -M_PI/6.0f, pose.theta);
}

// -------------------------------------------------------------

void test_PureLeftTurn()
{
    RobottoPose pose = {0};
    WheelsMovementUpdate w = { .timestamp = 0, .delta_angle_left = -M_PI/6.0f, .delta_angle_right = M_PI/6.0f };

    updateOdometry(&w, NULL, &pose);

    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 0.0f, pose.x);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 0.0f, pose.y);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, M_PI/3.0f, pose.theta);
}

// -------------------------------------------------------------

void test_PureRightTurn()
{
    RobottoPose pose = {0};
    WheelsMovementUpdate w = { .timestamp = 0, .delta_angle_left = M_PI/6.0f, .delta_angle_right = -M_PI/6.0f };

    updateOdometry(&w, NULL, &pose);

    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 0.0f, pose.x);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 0.0f, pose.y);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, -M_PI/3.0f, pose.theta);
}

// -------------------------------------------------------------


void test_CumulativeUpdates()
{
    RobottoPose pose = {0};
    WheelsMovementUpdate w1 = { .timestamp = 0, .delta_angle_left = M_PI/10.0f, .delta_angle_right = 0.0f };

    for(int i = 0; i < 10; ++i)
    {
    	updateOdometry(&w1, NULL, &pose);
    }

    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 1.0f, pose.x);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 0.0f, pose.y);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, -M_PI, pose.theta);


    WheelsMovementUpdate w2 = { .timestamp = 0, .delta_angle_left = 0.0f, .delta_angle_right = M_PI/10.0f };

    for(int i = 0; i < 10; ++i)
    {
    	updateOdometry(&w2, NULL, &pose);
    }

    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 2.0f, pose.x);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 0.0f, pose.y);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, 0, pose.theta);
}
