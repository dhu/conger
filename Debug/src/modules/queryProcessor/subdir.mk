################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/modules/queryProcessor/AdminCatalog.cc \
../src/modules/queryProcessor/ConsistencyMngr.cc \
../src/modules/queryProcessor/DataPath.cc \
../src/modules/queryProcessor/DynamicArray.cc \
../src/modules/queryProcessor/IOQueue.cc \
../src/modules/queryProcessor/QueryProcessor.cc \
../src/modules/queryProcessor/QueryProcessorHA.cc \
../src/modules/queryProcessor/QueryProcessorWithoutHA.cc \
../src/modules/queryProcessor/StreamBuffer.cc \
../src/modules/queryProcessor/UpdateServer.cc 

OBJS += \
./src/modules/queryProcessor/AdminCatalog.o \
./src/modules/queryProcessor/ConsistencyMngr.o \
./src/modules/queryProcessor/DataPath.o \
./src/modules/queryProcessor/DynamicArray.o \
./src/modules/queryProcessor/IOQueue.o \
./src/modules/queryProcessor/QueryProcessor.o \
./src/modules/queryProcessor/QueryProcessorHA.o \
./src/modules/queryProcessor/QueryProcessorWithoutHA.o \
./src/modules/queryProcessor/StreamBuffer.o \
./src/modules/queryProcessor/UpdateServer.o 

CC_DEPS += \
./src/modules/queryProcessor/AdminCatalog.d \
./src/modules/queryProcessor/ConsistencyMngr.d \
./src/modules/queryProcessor/DataPath.d \
./src/modules/queryProcessor/DynamicArray.d \
./src/modules/queryProcessor/IOQueue.d \
./src/modules/queryProcessor/QueryProcessor.d \
./src/modules/queryProcessor/QueryProcessorHA.d \
./src/modules/queryProcessor/QueryProcessorWithoutHA.d \
./src/modules/queryProcessor/StreamBuffer.d \
./src/modules/queryProcessor/UpdateServer.d 


# Each subdirectory must supply rules for building sources it contributes
src/modules/queryProcessor/%.o: ../src/modules/queryProcessor/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


