################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../demo/array/webcam_demo.cc 

OBJS += \
./demo/array/webcam_demo.o 

CC_DEPS += \
./demo/array/webcam_demo.d 


# Each subdirectory must supply rules for building sources it contributes
demo/array/%.o: ../demo/array/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


