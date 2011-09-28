################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/modules/queryProcessor/boxes/AggregateQBox.cc \
../src/modules/queryProcessor/boxes/AuroraJoinQBox.cc \
../src/modules/queryProcessor/boxes/BSortQBox.cc \
../src/modules/queryProcessor/boxes/Bucket.cc \
../src/modules/queryProcessor/boxes/FilterArrayQBox.cc \
../src/modules/queryProcessor/boxes/FilterQBox.cc \
../src/modules/queryProcessor/boxes/JoinQBox.cc \
../src/modules/queryProcessor/boxes/LockQBox.cc \
../src/modules/queryProcessor/boxes/MapQBox.cc \
../src/modules/queryProcessor/boxes/RandomDropQBox.cc \
../src/modules/queryProcessor/boxes/RevisionAggregateQBox.cc \
../src/modules/queryProcessor/boxes/RevisionFilterQBox.cc \
../src/modules/queryProcessor/boxes/RevisionMapQBox.cc \
../src/modules/queryProcessor/boxes/SControlQBox.cc \
../src/modules/queryProcessor/boxes/SJoinQBox.cc \
../src/modules/queryProcessor/boxes/SOutputQBox.cc \
../src/modules/queryProcessor/boxes/SUnionQBox.cc \
../src/modules/queryProcessor/boxes/UnionQBox.cc \
../src/modules/queryProcessor/boxes/UnlockQBox.cc \
../src/modules/queryProcessor/boxes/WaitForQBox.cc \
../src/modules/queryProcessor/boxes/WindowDropQBox.cc 

OBJS += \
./src/modules/queryProcessor/boxes/AggregateQBox.o \
./src/modules/queryProcessor/boxes/AuroraJoinQBox.o \
./src/modules/queryProcessor/boxes/BSortQBox.o \
./src/modules/queryProcessor/boxes/Bucket.o \
./src/modules/queryProcessor/boxes/FilterArrayQBox.o \
./src/modules/queryProcessor/boxes/FilterQBox.o \
./src/modules/queryProcessor/boxes/JoinQBox.o \
./src/modules/queryProcessor/boxes/LockQBox.o \
./src/modules/queryProcessor/boxes/MapQBox.o \
./src/modules/queryProcessor/boxes/RandomDropQBox.o \
./src/modules/queryProcessor/boxes/RevisionAggregateQBox.o \
./src/modules/queryProcessor/boxes/RevisionFilterQBox.o \
./src/modules/queryProcessor/boxes/RevisionMapQBox.o \
./src/modules/queryProcessor/boxes/SControlQBox.o \
./src/modules/queryProcessor/boxes/SJoinQBox.o \
./src/modules/queryProcessor/boxes/SOutputQBox.o \
./src/modules/queryProcessor/boxes/SUnionQBox.o \
./src/modules/queryProcessor/boxes/UnionQBox.o \
./src/modules/queryProcessor/boxes/UnlockQBox.o \
./src/modules/queryProcessor/boxes/WaitForQBox.o \
./src/modules/queryProcessor/boxes/WindowDropQBox.o 

CC_DEPS += \
./src/modules/queryProcessor/boxes/AggregateQBox.d \
./src/modules/queryProcessor/boxes/AuroraJoinQBox.d \
./src/modules/queryProcessor/boxes/BSortQBox.d \
./src/modules/queryProcessor/boxes/Bucket.d \
./src/modules/queryProcessor/boxes/FilterArrayQBox.d \
./src/modules/queryProcessor/boxes/FilterQBox.d \
./src/modules/queryProcessor/boxes/JoinQBox.d \
./src/modules/queryProcessor/boxes/LockQBox.d \
./src/modules/queryProcessor/boxes/MapQBox.d \
./src/modules/queryProcessor/boxes/RandomDropQBox.d \
./src/modules/queryProcessor/boxes/RevisionAggregateQBox.d \
./src/modules/queryProcessor/boxes/RevisionFilterQBox.d \
./src/modules/queryProcessor/boxes/RevisionMapQBox.d \
./src/modules/queryProcessor/boxes/SControlQBox.d \
./src/modules/queryProcessor/boxes/SJoinQBox.d \
./src/modules/queryProcessor/boxes/SOutputQBox.d \
./src/modules/queryProcessor/boxes/SUnionQBox.d \
./src/modules/queryProcessor/boxes/UnionQBox.d \
./src/modules/queryProcessor/boxes/UnlockQBox.d \
./src/modules/queryProcessor/boxes/WaitForQBox.d \
./src/modules/queryProcessor/boxes/WindowDropQBox.d 


# Each subdirectory must supply rules for building sources it contributes
src/modules/queryProcessor/boxes/%.o: ../src/modules/queryProcessor/boxes/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


