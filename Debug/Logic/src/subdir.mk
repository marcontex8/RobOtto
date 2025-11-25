################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Logic/src/odometry.c \
../Logic/src/pid_motor_controller.c \
../Logic/src/wheel_status_estimator.c 

OBJS += \
./Logic/src/odometry.o \
./Logic/src/pid_motor_controller.o \
./Logic/src/wheel_status_estimator.o 

C_DEPS += \
./Logic/src/odometry.d \
./Logic/src/pid_motor_controller.d \
./Logic/src/wheel_status_estimator.d 


# Each subdirectory must supply rules for building sources it contributes
Logic/src/%.o Logic/src/%.su Logic/src/%.cyclo: ../Logic/src/%.c Logic/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/FreeRTOS/include" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/FreeRTOS/portable" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/RTT/Config" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/RTT/RTT" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/Config" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/FreeRTOSV11" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/SEGGER" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/SYSVIEW" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/SensorsAndActuators/include" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/Common/include" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/Logic/include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Logic-2f-src

clean-Logic-2f-src:
	-$(RM) ./Logic/src/odometry.cyclo ./Logic/src/odometry.d ./Logic/src/odometry.o ./Logic/src/odometry.su ./Logic/src/pid_motor_controller.cyclo ./Logic/src/pid_motor_controller.d ./Logic/src/pid_motor_controller.o ./Logic/src/pid_motor_controller.su ./Logic/src/wheel_status_estimator.cyclo ./Logic/src/wheel_status_estimator.d ./Logic/src/wheel_status_estimator.o ./Logic/src/wheel_status_estimator.su

.PHONY: clean-Logic-2f-src

