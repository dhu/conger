################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../test/valid/global/validate_global.cc 

OBJS += \
./test/valid/global/validate_global.o 

CC_DEPS += \
./test/valid/global/validate_global.d 


# Each subdirectory must supply rules for building sources it contributes
test/valid/global/%.o: ../test/valid/global/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


