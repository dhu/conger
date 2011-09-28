################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../utility/stale/runtime/ConverterBox.cc \
../utility/stale/runtime/ConverterBoxRunner.cc \
../utility/stale/runtime/PriorityScheduler.cc \
../utility/stale/runtime/RegexConverterBox.cc \
../utility/stale/runtime/SimpleConverterBox.cc 

OBJS += \
./utility/stale/runtime/ConverterBox.o \
./utility/stale/runtime/ConverterBoxRunner.o \
./utility/stale/runtime/PriorityScheduler.o \
./utility/stale/runtime/RegexConverterBox.o \
./utility/stale/runtime/SimpleConverterBox.o 

CC_DEPS += \
./utility/stale/runtime/ConverterBox.d \
./utility/stale/runtime/ConverterBoxRunner.d \
./utility/stale/runtime/PriorityScheduler.d \
./utility/stale/runtime/RegexConverterBox.d \
./utility/stale/runtime/SimpleConverterBox.d 


# Each subdirectory must supply rules for building sources it contributes
utility/stale/runtime/%.o: ../utility/stale/runtime/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


