################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../utility/stale/tinydbwraper/test/Test.cc \
../utility/stale/tinydbwraper/test/TestReader.cc 

OBJS += \
./utility/stale/tinydbwraper/test/Test.o \
./utility/stale/tinydbwraper/test/TestReader.o 

CC_DEPS += \
./utility/stale/tinydbwraper/test/Test.d \
./utility/stale/tinydbwraper/test/TestReader.d 


# Each subdirectory must supply rules for building sources it contributes
utility/stale/tinydbwraper/test/%.o: ../utility/stale/tinydbwraper/test/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


