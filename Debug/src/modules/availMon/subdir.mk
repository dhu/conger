################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/modules/availMon/AvailabilityMonitor.cc 

OBJS += \
./src/modules/availMon/AvailabilityMonitor.o 

CC_DEPS += \
./src/modules/availMon/AvailabilityMonitor.d 


# Each subdirectory must supply rules for building sources it contributes
src/modules/availMon/%.o: ../src/modules/availMon/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


