################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ThirdParty/SystemView/Config/SEGGER_SYSVIEW_Config_FreeRTOS.c 

OBJS += \
./ThirdParty/SystemView/Config/SEGGER_SYSVIEW_Config_FreeRTOS.o 

C_DEPS += \
./ThirdParty/SystemView/Config/SEGGER_SYSVIEW_Config_FreeRTOS.d 


# Each subdirectory must supply rules for building sources it contributes
ThirdParty/SystemView/Config/%.o ThirdParty/SystemView/Config/%.su ThirdParty/SystemView/Config/%.cyclo: ../ThirdParty/SystemView/Config/%.c ThirdParty/SystemView/Config/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/FreeRTOS/include" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/FreeRTOS/portable" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/RTT/Config" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/RTT/RTT" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/Config" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/FreeRTOSV11" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/SEGGER" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/SYSVIEW" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/SensorsAndActuators/include" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/Common/include" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/Logic/include" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/Tasks/include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ThirdParty-2f-SystemView-2f-Config

clean-ThirdParty-2f-SystemView-2f-Config:
	-$(RM) ./ThirdParty/SystemView/Config/SEGGER_SYSVIEW_Config_FreeRTOS.cyclo ./ThirdParty/SystemView/Config/SEGGER_SYSVIEW_Config_FreeRTOS.d ./ThirdParty/SystemView/Config/SEGGER_SYSVIEW_Config_FreeRTOS.o ./ThirdParty/SystemView/Config/SEGGER_SYSVIEW_Config_FreeRTOS.su

.PHONY: clean-ThirdParty-2f-SystemView-2f-Config

