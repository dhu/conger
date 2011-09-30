################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../demo/ids/ids.cc 

OBJS += \
./demo/ids/ids.o 

CC_DEPS += \
./demo/ids/ids.d 


# Each subdirectory must supply rules for building sources it contributes
demo/ids/%.o: ../demo/ids/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


