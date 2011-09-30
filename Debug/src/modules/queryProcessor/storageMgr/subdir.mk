################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/modules/queryProcessor/storageMgr/CP.cc \
../src/modules/queryProcessor/storageMgr/CPQBox.cc \
../src/modules/queryProcessor/storageMgr/CPStorage.cc \
../src/modules/queryProcessor/storageMgr/CPView.cc \
../src/modules/queryProcessor/storageMgr/PagePool.cc \
../src/modules/queryProcessor/storageMgr/SweeperQBox.cc \
../src/modules/queryProcessor/storageMgr/TupleQueue.cc \
../src/modules/queryProcessor/storageMgr/UpdateQueue.cc 

OBJS += \
./src/modules/queryProcessor/storageMgr/CP.o \
./src/modules/queryProcessor/storageMgr/CPQBox.o \
./src/modules/queryProcessor/storageMgr/CPStorage.o \
./src/modules/queryProcessor/storageMgr/CPView.o \
./src/modules/queryProcessor/storageMgr/PagePool.o \
./src/modules/queryProcessor/storageMgr/SweeperQBox.o \
./src/modules/queryProcessor/storageMgr/TupleQueue.o \
./src/modules/queryProcessor/storageMgr/UpdateQueue.o 

CC_DEPS += \
./src/modules/queryProcessor/storageMgr/CP.d \
./src/modules/queryProcessor/storageMgr/CPQBox.d \
./src/modules/queryProcessor/storageMgr/CPStorage.d \
./src/modules/queryProcessor/storageMgr/CPView.d \
./src/modules/queryProcessor/storageMgr/PagePool.d \
./src/modules/queryProcessor/storageMgr/SweeperQBox.d \
./src/modules/queryProcessor/storageMgr/TupleQueue.d \
./src/modules/queryProcessor/storageMgr/UpdateQueue.d 


# Each subdirectory must supply rules for building sources it contributes
src/modules/queryProcessor/storageMgr/%.o: ../src/modules/queryProcessor/storageMgr/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


