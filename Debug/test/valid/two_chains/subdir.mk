################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../test/valid/two_chains/validate_two_chains.cc 

OBJS += \
./test/valid/two_chains/validate_two_chains.o 

CC_DEPS += \
./test/valid/two_chains/validate_two_chains.d 


# Each subdirectory must supply rules for building sources it contributes
test/valid/two_chains/%.o: ../test/valid/two_chains/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


