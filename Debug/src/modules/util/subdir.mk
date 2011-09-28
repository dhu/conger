################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/modules/util/BufferList.cc \
../src/modules/util/CpuStats.cc \
../src/modules/util/Exceptions.cc \
../src/modules/util/FieldExt.cc \
../src/modules/util/HashForBufferList.cc \
../src/modules/util/LockHolder.cc \
../src/modules/util/PtCondition.cc \
../src/modules/util/PtMutex.cc \
../src/modules/util/StackTrace.cc \
../src/modules/util/StorageMgr_Exceptions.cc \
../src/modules/util/XmlTempString.cc \
../src/modules/util/nmstl_util.cc \
../src/modules/util/parseutil.cc \
../src/modules/util/tupleGenerator.cc \
../src/modules/util/util.cc \
../src/modules/util/xercesDomUtil.cc 

OBJS += \
./src/modules/util/BufferList.o \
./src/modules/util/CpuStats.o \
./src/modules/util/Exceptions.o \
./src/modules/util/FieldExt.o \
./src/modules/util/HashForBufferList.o \
./src/modules/util/LockHolder.o \
./src/modules/util/PtCondition.o \
./src/modules/util/PtMutex.o \
./src/modules/util/StackTrace.o \
./src/modules/util/StorageMgr_Exceptions.o \
./src/modules/util/XmlTempString.o \
./src/modules/util/nmstl_util.o \
./src/modules/util/parseutil.o \
./src/modules/util/tupleGenerator.o \
./src/modules/util/util.o \
./src/modules/util/xercesDomUtil.o 

CC_DEPS += \
./src/modules/util/BufferList.d \
./src/modules/util/CpuStats.d \
./src/modules/util/Exceptions.d \
./src/modules/util/FieldExt.d \
./src/modules/util/HashForBufferList.d \
./src/modules/util/LockHolder.d \
./src/modules/util/PtCondition.d \
./src/modules/util/PtMutex.d \
./src/modules/util/StackTrace.d \
./src/modules/util/StorageMgr_Exceptions.d \
./src/modules/util/XmlTempString.d \
./src/modules/util/nmstl_util.d \
./src/modules/util/parseutil.d \
./src/modules/util/tupleGenerator.d \
./src/modules/util/util.d \
./src/modules/util/xercesDomUtil.d 


# Each subdirectory must supply rules for building sources it contributes
src/modules/util/%.o: ../src/modules/util/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


