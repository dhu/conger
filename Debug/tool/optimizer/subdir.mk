################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../tool/optimizer/MetaOptimizer.cc 

OBJS += \
./tool/optimizer/MetaOptimizer.o 

CC_DEPS += \
./tool/optimizer/MetaOptimizer.d 


# Each subdirectory must supply rules for building sources it contributes
tool/optimizer/%.o: ../tool/optimizer/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


