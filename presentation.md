# Embedded Project

1. Personal project, not company related
 + no nda, open source, single developer
 - limited resource:
 	HW cost		-> very sub-optimal choices
 	available time	-> not complete + technical debt


2. RobOtto - Hardware
 roomba-style indoor robot, cheap, goal being an occupancy grid of the area (or 2d slam but with coarse grain) 
	Control unit: STM32F446		ARM Cortex-M4, con FPU, 180 MHz, 512 Kbytes of Flash memory, 128 Kbytes of SRAM
	Motor driver: L298		Dual full-bridge driver, PWM controlled
	Wheels encoder (2x): AS5600	Magnetic rotary position sensor, with I2C
	IMU: MPU-6500			Accelerometer+Gyro, with I2C
	Ultrasonic  Ranging: HC-SR04	mounted on top of a servomotor, provide coarse range detections
	+
	Communication unit: ESP32	32-bit RISC-V, 2.4 GHz Wi-Fi (802.11b/g/n)		

 
3. Third Party firmware libraries
	STM32F446			STM32-HAL
					FreeRTOS (V11.1.0)
					Segger J-Link RTT + SystemView
	
	ESP32				prebuild ESP-AT by expressif


4. Tasks
    wheels control			closed loop PID control of wheels speed
    motion planning			give target pose and estimated positione calculates wheels speed setpoint
    pose estimation			calculate wheels odometry (+ WIP: fuse with gyro sensor using kalman, + magnetometer?)
    object detection		controls servo and ultrasonic sensor to provide object detections
  > telemetry <				manage communication stack: STM32 >uart< ESP32 >wifi< HOST SYSTEM

   
5. Telemetry
	objective: collect data at runtime for testing algorithms

	main issues:
		- avoid disruption of high priority tasks
		- manage asynchronous UART communication
		- keep track of the ESP32 communication status
	
	structure:
		set of event-based state machines, with shared events queue
	
	key-protocols:
		- UART + AT (expressif)
		- Wifi + MQTT

