################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../src/src/borealis.o \
../src/src/monitor.o 

CC_SRCS += \
../src/src/borealis.cc \
../src/src/monitor.cc 

OBJS += \
./src/src/borealis.o \
./src/src/monitor.o 

CC_DEPS += \
./src/src/borealis.d \
./src/src/monitor.d 


# Each subdirectory must supply rules for building sources it contributes
src/src/%.o: ../src/src/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


