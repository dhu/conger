################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../demo/ha/fail_demo.cc \
../demo/ha/ha_demo.cc 

OBJS += \
./demo/ha/fail_demo.o \
./demo/ha/ha_demo.o 

CC_DEPS += \
./demo/ha/fail_demo.d \
./demo/ha/ha_demo.d 


# Each subdirectory must supply rules for building sources it contributes
demo/ha/%.o: ../demo/ha/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


