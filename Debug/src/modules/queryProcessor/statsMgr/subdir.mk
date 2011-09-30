################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/modules/queryProcessor/statsMgr/FixLenTimeSeries.cc \
../src/modules/queryProcessor/statsMgr/RWlock.cc \
../src/modules/queryProcessor/statsMgr/Statistics.cc \
../src/modules/queryProcessor/statsMgr/StatsMgr.cc \
../src/modules/queryProcessor/statsMgr/TSStats.cc \
../src/modules/queryProcessor/statsMgr/VersionRWLock.cc 

OBJS += \
./src/modules/queryProcessor/statsMgr/FixLenTimeSeries.o \
./src/modules/queryProcessor/statsMgr/RWlock.o \
./src/modules/queryProcessor/statsMgr/Statistics.o \
./src/modules/queryProcessor/statsMgr/StatsMgr.o \
./src/modules/queryProcessor/statsMgr/TSStats.o \
./src/modules/queryProcessor/statsMgr/VersionRWLock.o 

CC_DEPS += \
./src/modules/queryProcessor/statsMgr/FixLenTimeSeries.d \
./src/modules/queryProcessor/statsMgr/RWlock.d \
./src/modules/queryProcessor/statsMgr/Statistics.d \
./src/modules/queryProcessor/statsMgr/StatsMgr.d \
./src/modules/queryProcessor/statsMgr/TSStats.d \
./src/modules/queryProcessor/statsMgr/VersionRWLock.d 


# Each subdirectory must supply rules for building sources it contributes
src/modules/queryProcessor/statsMgr/%.o: ../src/modules/queryProcessor/statsMgr/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


