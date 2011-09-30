################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/modules/queryProcessor/runtime/AuroraNode.cc \
../src/modules/queryProcessor/runtime/AuroraNodeMonitor.cc \
../src/modules/queryProcessor/runtime/AuroraNodeWithoutHA.cc \
../src/modules/queryProcessor/runtime/ConverterBox.cc \
../src/modules/queryProcessor/runtime/ConverterBoxRunner.cc \
../src/modules/queryProcessor/runtime/PseudoScheduler.cc \
../src/modules/queryProcessor/runtime/PseudoSchedulerMonitor.cc \
../src/modules/queryProcessor/runtime/QBox.cc \
../src/modules/queryProcessor/runtime/QBoxState.cc \
../src/modules/queryProcessor/runtime/RegexConverterBox.cc \
../src/modules/queryProcessor/runtime/Scheduler.cc \
../src/modules/queryProcessor/runtime/SimpleConverterBox.cc \
../src/modules/queryProcessor/runtime/Stream.cc \
../src/modules/queryProcessor/runtime/TuplesAvailableMailbox.cc 

OBJS += \
./src/modules/queryProcessor/runtime/AuroraNode.o \
./src/modules/queryProcessor/runtime/AuroraNodeMonitor.o \
./src/modules/queryProcessor/runtime/AuroraNodeWithoutHA.o \
./src/modules/queryProcessor/runtime/ConverterBox.o \
./src/modules/queryProcessor/runtime/ConverterBoxRunner.o \
./src/modules/queryProcessor/runtime/PseudoScheduler.o \
./src/modules/queryProcessor/runtime/PseudoSchedulerMonitor.o \
./src/modules/queryProcessor/runtime/QBox.o \
./src/modules/queryProcessor/runtime/QBoxState.o \
./src/modules/queryProcessor/runtime/RegexConverterBox.o \
./src/modules/queryProcessor/runtime/Scheduler.o \
./src/modules/queryProcessor/runtime/SimpleConverterBox.o \
./src/modules/queryProcessor/runtime/Stream.o \
./src/modules/queryProcessor/runtime/TuplesAvailableMailbox.o 

CC_DEPS += \
./src/modules/queryProcessor/runtime/AuroraNode.d \
./src/modules/queryProcessor/runtime/AuroraNodeMonitor.d \
./src/modules/queryProcessor/runtime/AuroraNodeWithoutHA.d \
./src/modules/queryProcessor/runtime/ConverterBox.d \
./src/modules/queryProcessor/runtime/ConverterBoxRunner.d \
./src/modules/queryProcessor/runtime/PseudoScheduler.d \
./src/modules/queryProcessor/runtime/PseudoSchedulerMonitor.d \
./src/modules/queryProcessor/runtime/QBox.d \
./src/modules/queryProcessor/runtime/QBoxState.d \
./src/modules/queryProcessor/runtime/RegexConverterBox.d \
./src/modules/queryProcessor/runtime/Scheduler.d \
./src/modules/queryProcessor/runtime/SimpleConverterBox.d \
./src/modules/queryProcessor/runtime/Stream.d \
./src/modules/queryProcessor/runtime/TuplesAvailableMailbox.d 


# Each subdirectory must supply rules for building sources it contributes
src/modules/queryProcessor/runtime/%.o: ../src/modules/queryProcessor/runtime/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


