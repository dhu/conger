################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../demo/contrast_composite/process_video.cc 

OBJS += \
./demo/contrast_composite/process_video.o 

CC_DEPS += \
./demo/contrast_composite/process_video.d 


# Each subdirectory must supply rules for building sources it contributes
demo/contrast_composite/%.o: ../demo/contrast_composite/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


