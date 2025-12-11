################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Tasks/src/communication_manager.c \
../Tasks/src/motion_planning.c \
../Tasks/src/pose_estimation.c \
../Tasks/src/robotto_tasks.c \
../Tasks/src/wheels_control.c 

OBJS += \
./Tasks/src/communication_manager.o \
./Tasks/src/motion_planning.o \
./Tasks/src/pose_estimation.o \
./Tasks/src/robotto_tasks.o \
./Tasks/src/wheels_control.o 

C_DEPS += \
./Tasks/src/communication_manager.d \
./Tasks/src/motion_planning.d \
./Tasks/src/pose_estimation.d \
./Tasks/src/robotto_tasks.d \
./Tasks/src/wheels_control.d 


# Each subdirectory must supply rules for building sources it contributes
Tasks/src/%.o Tasks/src/%.su Tasks/src/%.cyclo: ../Tasks/src/%.c Tasks/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/FreeRTOS/include" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/FreeRTOS/portable" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/RTT/Config" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/RTT/RTT" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/Config" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/FreeRTOSV11" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/SEGGER" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/SYSVIEW" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/SensorsAndActuators/include" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/Common/include" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/Logic/include" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/Tasks/include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Tasks-2f-src

clean-Tasks-2f-src:
	-$(RM) ./Tasks/src/communication_manager.cyclo ./Tasks/src/communication_manager.d ./Tasks/src/communication_manager.o ./Tasks/src/communication_manager.su ./Tasks/src/motion_planning.cyclo ./Tasks/src/motion_planning.d ./Tasks/src/motion_planning.o ./Tasks/src/motion_planning.su ./Tasks/src/pose_estimation.cyclo ./Tasks/src/pose_estimation.d ./Tasks/src/pose_estimation.o ./Tasks/src/pose_estimation.su ./Tasks/src/robotto_tasks.cyclo ./Tasks/src/robotto_tasks.d ./Tasks/src/robotto_tasks.o ./Tasks/src/robotto_tasks.su ./Tasks/src/wheels_control.cyclo ./Tasks/src/wheels_control.d ./Tasks/src/wheels_control.o ./Tasks/src/wheels_control.su

.PHONY: clean-Tasks-2f-src

