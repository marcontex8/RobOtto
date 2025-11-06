################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ThirdParty/RTT/Syscalls/SEGGER_RTT_Syscalls_GCC.c 

OBJS += \
./ThirdParty/RTT/Syscalls/SEGGER_RTT_Syscalls_GCC.o 

C_DEPS += \
./ThirdParty/RTT/Syscalls/SEGGER_RTT_Syscalls_GCC.d 


# Each subdirectory must supply rules for building sources it contributes
ThirdParty/RTT/Syscalls/%.o ThirdParty/RTT/Syscalls/%.su ThirdParty/RTT/Syscalls/%.cyclo: ../ThirdParty/RTT/Syscalls/%.c ThirdParty/RTT/Syscalls/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/FreeRTOS/include" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/FreeRTOS/portable" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/RTT/Config" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/RTT/RTT" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/Config" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/FreeRTOSV11" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/SEGGER" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/SystemView/SYSVIEW" -I"/home/marco/STM32CubeIDE/workspace_1.19.0/Robotto/ThirdParty/FreeRTOS/portable/GCC/ARM_CM4F" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ThirdParty-2f-RTT-2f-Syscalls

clean-ThirdParty-2f-RTT-2f-Syscalls:
	-$(RM) ./ThirdParty/RTT/Syscalls/SEGGER_RTT_Syscalls_GCC.cyclo ./ThirdParty/RTT/Syscalls/SEGGER_RTT_Syscalls_GCC.d ./ThirdParty/RTT/Syscalls/SEGGER_RTT_Syscalls_GCC.o ./ThirdParty/RTT/Syscalls/SEGGER_RTT_Syscalls_GCC.su

.PHONY: clean-ThirdParty-2f-RTT-2f-Syscalls

