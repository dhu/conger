################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/modules/ha/AuroraNodeWithHA.cc \
../src/modules/ha/HA.cc \
../src/modules/ha/HASegment.cc \
../src/modules/ha/QueryProcessorWithHA.cc 

OBJS += \
./src/modules/ha/AuroraNodeWithHA.o \
./src/modules/ha/HA.o \
./src/modules/ha/HASegment.o \
./src/modules/ha/QueryProcessorWithHA.o 

CC_DEPS += \
./src/modules/ha/AuroraNodeWithHA.d \
./src/modules/ha/HA.d \
./src/modules/ha/HASegment.d \
./src/modules/ha/QueryProcessorWithHA.d 


# Each subdirectory must supply rules for building sources it contributes
src/modules/ha/%.o: ../src/modules/ha/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


