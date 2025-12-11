################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Common/src/at_state_machine.c \
../Common/src/i2c_busses.c \
../Common/src/network_communication.c \
../Common/src/uart_reader.c \
../Common/src/uart_writer.c 

OBJS += \
./Common/src/at_state_machine.o \
./Common/src/i2c_busses.o \
./Common/src/network_communication.o \
./Common/src/uart_reader.o \
./Common/src/uart_writer.o 

C_DEPS += \
./Common/src/at_state_machine.d \
./Common/src/i2c_busses.d \
./Common/src/network_communication.d \
./Common/src/uart_reader.d \
./Common/src/uart_writer.d 


# Each subdirectory must supply rules for building sources it contributes
Common/src/%.o Common/src/%.su Common/src/%.cyclo: ../Common/src/%.c Common/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/FreeRTOS/include" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/FreeRTOS/portable" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/RTT/Config" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/RTT/RTT" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/Config" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/FreeRTOSV11" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/SEGGER" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/SYSVIEW" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/SensorsAndActuators/include" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/Common/include" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/Logic/include" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/Tasks/include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Common-2f-src

clean-Common-2f-src:
	-$(RM) ./Common/src/at_state_machine.cyclo ./Common/src/at_state_machine.d ./Common/src/at_state_machine.o ./Common/src/at_state_machine.su ./Common/src/i2c_busses.cyclo ./Common/src/i2c_busses.d ./Common/src/i2c_busses.o ./Common/src/i2c_busses.su ./Common/src/network_communication.cyclo ./Common/src/network_communication.d ./Common/src/network_communication.o ./Common/src/network_communication.su ./Common/src/uart_reader.cyclo ./Common/src/uart_reader.d ./Common/src/uart_reader.o ./Common/src/uart_reader.su ./Common/src/uart_writer.cyclo ./Common/src/uart_writer.d ./Common/src/uart_writer.o ./Common/src/uart_writer.su

.PHONY: clean-Common-2f-src

