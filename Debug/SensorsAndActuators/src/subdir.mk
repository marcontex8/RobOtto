################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SensorsAndActuators/src/encoder_reader.c \
../SensorsAndActuators/src/imu_reader.c \
../SensorsAndActuators/src/motor_driver.c \
../SensorsAndActuators/src/wheel_status_estimator.c 

OBJS += \
./SensorsAndActuators/src/encoder_reader.o \
./SensorsAndActuators/src/imu_reader.o \
./SensorsAndActuators/src/motor_driver.o \
./SensorsAndActuators/src/wheel_status_estimator.o 

C_DEPS += \
./SensorsAndActuators/src/encoder_reader.d \
./SensorsAndActuators/src/imu_reader.d \
./SensorsAndActuators/src/motor_driver.d \
./SensorsAndActuators/src/wheel_status_estimator.d 


# Each subdirectory must supply rules for building sources it contributes
SensorsAndActuators/src/%.o SensorsAndActuators/src/%.su SensorsAndActuators/src/%.cyclo: ../SensorsAndActuators/src/%.c SensorsAndActuators/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/FreeRTOS/include" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/FreeRTOS/portable" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/RTT/Config" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/RTT/RTT" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/Config" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/FreeRTOSV11" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/SEGGER" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/SYSVIEW" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/SensorsAndActuators/include" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/Common/include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-SensorsAndActuators-2f-src

clean-SensorsAndActuators-2f-src:
	-$(RM) ./SensorsAndActuators/src/encoder_reader.cyclo ./SensorsAndActuators/src/encoder_reader.d ./SensorsAndActuators/src/encoder_reader.o ./SensorsAndActuators/src/encoder_reader.su ./SensorsAndActuators/src/imu_reader.cyclo ./SensorsAndActuators/src/imu_reader.d ./SensorsAndActuators/src/imu_reader.o ./SensorsAndActuators/src/imu_reader.su ./SensorsAndActuators/src/motor_driver.cyclo ./SensorsAndActuators/src/motor_driver.d ./SensorsAndActuators/src/motor_driver.o ./SensorsAndActuators/src/motor_driver.su ./SensorsAndActuators/src/wheel_status_estimator.cyclo ./SensorsAndActuators/src/wheel_status_estimator.d ./SensorsAndActuators/src/wheel_status_estimator.o ./SensorsAndActuators/src/wheel_status_estimator.su

.PHONY: clean-SensorsAndActuators-2f-src

