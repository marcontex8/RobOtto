################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/SensorsAndActuators/src/encoder_reader.c \
../Drivers/SensorsAndActuators/src/motor_driver.c 

OBJS += \
./Drivers/SensorsAndActuators/src/encoder_reader.o \
./Drivers/SensorsAndActuators/src/motor_driver.o 

C_DEPS += \
./Drivers/SensorsAndActuators/src/encoder_reader.d \
./Drivers/SensorsAndActuators/src/motor_driver.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/SensorsAndActuators/src/%.o Drivers/SensorsAndActuators/src/%.su Drivers/SensorsAndActuators/src/%.cyclo: ../Drivers/SensorsAndActuators/src/%.c Drivers/SensorsAndActuators/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/FreeRTOS/include" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/FreeRTOS/portable" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/RTT/Config" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/RTT/RTT" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/Config" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/FreeRTOSV11" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/SEGGER" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/SYSVIEW" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/Drivers/SensorsAndActuators/include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-SensorsAndActuators-2f-src

clean-Drivers-2f-SensorsAndActuators-2f-src:
	-$(RM) ./Drivers/SensorsAndActuators/src/encoder_reader.cyclo ./Drivers/SensorsAndActuators/src/encoder_reader.d ./Drivers/SensorsAndActuators/src/encoder_reader.o ./Drivers/SensorsAndActuators/src/encoder_reader.su ./Drivers/SensorsAndActuators/src/motor_driver.cyclo ./Drivers/SensorsAndActuators/src/motor_driver.d ./Drivers/SensorsAndActuators/src/motor_driver.o ./Drivers/SensorsAndActuators/src/motor_driver.su

.PHONY: clean-Drivers-2f-SensorsAndActuators-2f-src

