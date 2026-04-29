/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#include "unity.h"
#include "trajectory_planner.h"
#include <math.h>
#include "robotto_conf.h"
#include "logic_common.h"

/**
 * This file contains validation tests for the trajectory planner module.
 * 
 * The module provide the following functions:
 * WheelSpeedSetPoint computeWheelSpeedSetpoint(const RobottoPose* current);
 * void defineNewTargetPose(RobottoPose end);
 * bool endPoseReached(const RobottoPose* current);
 * 
 * The tests are designed to verify the correctness of the trajectory planner's behavior in various scenarios.
 * The control is assumed to be periodic, and the tests will simulate a sequence of pose updates to evaluate the planner's response.
 * A basic mathematical model of the robot's kinematics will be used to calculate the pose according to the wheel speed setpoints.
 * The tests will cover scenarios such as:
 * - Moving straight towards a target pose.
 * - Turning in place to align with the target pose.
 * - Moving in an arc towards the target pose.
 * Test will pass if the simulated model is able to reach the target pose within a specified tolerance.
 */

#define TOLERANCE 5e-2
#define SIMULATION_STEPS 1000
#define CONTROL_PERIOD 0.01f  // 10ms control period

void setUp(void) 
{
}

void tearDown(void) 
{
}

/**
 * Helper function to simulate robot movement based on wheel speed setpoints
 * Updates the pose according to basic kinematics model
 */
static void simulateRobotMovement(RobottoPose* pose, WheelSpeedSetPoint wheel_speeds)
{
    // Calculate instantaneous speeds from wheel speeds
    float v_left = wheel_speeds.left * WHEELS_RADIUS;
    float v_right = wheel_speeds.right * WHEELS_RADIUS;
    
    // Calculate linear and angular velocity
    float v_linear = (v_left + v_right) / 2.0f;
    float v_angular = (v_right - v_left) / WHEELS_DISTANCE;
    
    // Update pose using kinematic model
    pose->x += -1.0 * v_linear * CONTROL_PERIOD * sinf(pose->theta);
    pose->y += v_linear * CONTROL_PERIOD * cosf(pose->theta);
    pose->theta += v_angular * CONTROL_PERIOD;
    
    // Normalize theta to [-pi, pi]
    while (pose->theta > M_PI) pose->theta -= 2.0f * M_PI;
    while (pose->theta < -M_PI) pose->theta += 2.0f * M_PI;
}

// =====================================================================
// Test 1: Moving straight forward towards a target pose
// =====================================================================

void test_MoveStrightForwardToTarget()
{
    RobottoPose start = {.x = 0.0f, .y = 0.0f, .theta = 0.0f, .timestamp = 0};
    RobottoPose target = {.x = 0.0f, .y = 5.0f, .theta = 0.0f, .timestamp = 0};
    
    defineNewTargetPose(target);
    
    RobottoPose current = start;
    for (int step = 0; step < SIMULATION_STEPS; step++)
    {
        WheelSpeedSetPoint setpoint = computeWheelSpeedSetpoint(&current);
        simulateRobotMovement(&current, setpoint);
        
        if (endPoseReached(&current))
        {
            break;
        }
    }
    
    // Verify we reached the target within tolerance
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, target.x, current.x);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, target.y, current.y);
    TEST_ASSERT_TRUE(endPoseReached(&current));
}

// =====================================================================
// Test 2: Moving backward to a target pose
// =====================================================================

void test_MoveBackwardToTarget()
{
    RobottoPose start = {.x = 0.0f, .y = 0.0f, .theta = 0.0f, .timestamp = 0};
    RobottoPose target = {.x = 0.0f, .y = -3.0f, .theta = 0.0f, .timestamp = 0};
    
    defineNewTargetPose(target);
    
    RobottoPose current = start;
    for (int step = 0; step < SIMULATION_STEPS; step++)
    {
        WheelSpeedSetPoint setpoint = computeWheelSpeedSetpoint(&current);
        simulateRobotMovement(&current, setpoint);
        
        if (endPoseReached(&current))
        {
            break;
        }
    }
    
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, target.x, current.x);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, target.y, current.y);
    TEST_ASSERT_TRUE(endPoseReached(&current));
}

// =====================================================================
// Test 3: Turning in place to align with target orientation
// =====================================================================

void test_TurnInPlaceToAlignWithTarget()
{
    RobottoPose start = {.x = 0.0f, .y = 0.0f, .theta = 0.0f, .timestamp = 0};
    RobottoPose target = {.x = 0.0f, .y = 0.0f, .theta = M_PI / 4.0f, .timestamp = 0};
    
    defineNewTargetPose(target);
    
    RobottoPose current = start;
    for (int step = 0; step < SIMULATION_STEPS; step++)
    {
        WheelSpeedSetPoint setpoint = computeWheelSpeedSetpoint(&current);
        simulateRobotMovement(&current, setpoint);
        
        if (endPoseReached(&current))
        {
            break;
        }
    }
    
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, target.x, current.x);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, target.y, current.y);
    TEST_ASSERT_TRUE(endPoseReached(&current));
}

// =====================================================================
// Test 4: Turning in the opposite direction (negative angle)
// =====================================================================

void test_TurnInPlaceNegativeAngle()
{
    RobottoPose start = {.x = 0.0f, .y = 0.0f, .theta = 0.0f, .timestamp = 0};
    RobottoPose target = {.x = 0.0f, .y = 0.0f, .theta = -M_PI / 3.0f, .timestamp = 0};
    
    defineNewTargetPose(target);
    
    RobottoPose current = start;
    for (int step = 0; step < SIMULATION_STEPS; step++)
    {
        WheelSpeedSetPoint setpoint = computeWheelSpeedSetpoint(&current);
        simulateRobotMovement(&current, setpoint);
        
        if (endPoseReached(&current))
        {
            break;
        }
    }
    
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, target.x, current.x);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, target.y, current.y);
    TEST_ASSERT_TRUE(endPoseReached(&current));
}

// =====================================================================
// Test 5: Moving in an arc to reach target pose
// =====================================================================

void test_MoveInArcToTarget()
{
    RobottoPose start = {.x = 0.0f, .y = 0.0f, .theta = 0.0f, .timestamp = 0};
    RobottoPose target = {.x = 2.0f, .y = 2.0f, .theta = M_PI / 4.0f, .timestamp = 0};
    
    defineNewTargetPose(target);
    
    RobottoPose current = start;
    for (int step = 0; step < SIMULATION_STEPS; step++)
    {
        WheelSpeedSetPoint setpoint = computeWheelSpeedSetpoint(&current);
        simulateRobotMovement(&current, setpoint);
        
        if (endPoseReached(&current))
        {
            break;
        }
    }
    
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, target.x, current.x);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, target.y, current.y);
    TEST_ASSERT_TRUE(endPoseReached(&current));
}

// =====================================================================
// Test 6: Moving right (positive X direction)
// =====================================================================

void test_MoveRightToTarget()
{
    RobottoPose start = {.x = 0.0f, .y = 0.0f, .theta = 0.0f, .timestamp = 0};
    RobottoPose target = {.x = 3.0f, .y = 0.0f, .theta = 0.0f, .timestamp = 0};
    
    defineNewTargetPose(target);
    
    RobottoPose current = start;
    for (int step = 0; step < SIMULATION_STEPS; step++)
    {
        WheelSpeedSetPoint setpoint = computeWheelSpeedSetpoint(&current);
        simulateRobotMovement(&current, setpoint);
        
        if (endPoseReached(&current))
        {
            break;
        }
    }
    
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, target.x, current.x);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, target.y, current.y);
    TEST_ASSERT_TRUE(endPoseReached(&current));
}

// =====================================================================
// Test 7: Moving left (negative X direction)
// =====================================================================

void test_MoveLeftToTarget()
{
    RobottoPose start = {.x = 0.0f, .y = 0.0f, .theta = 0.0f, .timestamp = 0};
    RobottoPose target = {.x = -2.5f, .y = 0.0f, .theta = 0.0f, .timestamp = 0};
    
    defineNewTargetPose(target);
    
    RobottoPose current = start;
    for (int step = 0; step < SIMULATION_STEPS; step++)
    {
        WheelSpeedSetPoint setpoint = computeWheelSpeedSetpoint(&current);
        simulateRobotMovement(&current, setpoint);
        
        if (endPoseReached(&current))
        {
            break;
        }
    }
    
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, target.x, current.x);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, target.y, current.y);
    TEST_ASSERT_TRUE(endPoseReached(&current));
}

// =====================================================================
// Test 8: Complex movement with both translation and rotation
// =====================================================================

void test_ComplexArcMovement()
{
    RobottoPose start = {.x = 0.0f, .y = 0.0f, .theta = 0.0f, .timestamp = 0};
    RobottoPose target = {.x = 1.0f, .y = 3.0f, .theta = M_PI / 2.0f, .timestamp = 0};
    
    defineNewTargetPose(target);
    
    RobottoPose current = start;
    for (int step = 0; step < SIMULATION_STEPS; step++)
    {
        WheelSpeedSetPoint setpoint = computeWheelSpeedSetpoint(&current);
        simulateRobotMovement(&current, setpoint);
        
        if (endPoseReached(&current))
        {
            break;
        }
    }
    
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, target.x, current.x);
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE, target.y, current.y);
    TEST_ASSERT_TRUE(endPoseReached(&current));
}

// =====================================================================
// Test 9: Verify end pose reached detection at target
// =====================================================================

void test_EndPoseReachedDetection()
{
    RobottoPose target = {.x = 1.0f, .y = 2.0f, .theta = M_PI / 6.0f, .timestamp = 0};
    defineNewTargetPose(target);
    
    // Should be true when at target
    TEST_ASSERT_TRUE(endPoseReached(&target));
    
    // Should be true within tolerance
    RobottoPose near_target = {.x = 1.0f + TOLERANCE/2, .y = 2.0f + TOLERANCE/2, 
                                .theta = M_PI/6.0f + TOLERANCE/2, .timestamp = 0};
    TEST_ASSERT_TRUE(endPoseReached(&near_target));
}

// =====================================================================
// Test 10: Verify end pose not reached when far from target
// =====================================================================

void test_EndPoseNotReachedWhenFarFromTarget()
{
    RobottoPose target = {.x = 1.0f, .y = 2.0f, .theta = M_PI / 6.0f, .timestamp = 0};
    defineNewTargetPose(target);
    
    // Create pose that is clearly far from target
    RobottoPose far_pose = {.x = 0.0f, .y = 0.0f, .theta = 0.0f, .timestamp = 0};
    
    TEST_ASSERT_FALSE(endPoseReached(&far_pose));
}
