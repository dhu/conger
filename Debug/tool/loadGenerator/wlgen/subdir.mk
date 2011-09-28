################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../tool/loadGenerator/wlgen/AggSourceFileReader.cc \
../tool/loadGenerator/wlgen/AggStreamSource.cc \
../tool/loadGenerator/wlgen/AttributeSource.cc \
../tool/loadGenerator/wlgen/GenDataType.cc \
../tool/loadGenerator/wlgen/RandomNumGenerator.cc \
../tool/loadGenerator/wlgen/SingleSource.cc \
../tool/loadGenerator/wlgen/SourceGenerator.cc \
../tool/loadGenerator/wlgen/SourcePool.cc \
../tool/loadGenerator/wlgen/StreamBuffer.cc \
../tool/loadGenerator/wlgen/StreamSender.cc \
../tool/loadGenerator/wlgen/StreamTuple.cc \
../tool/loadGenerator/wlgen/TimeStampGenerator.cc \
../tool/loadGenerator/wlgen/TuplesSave.cc \
../tool/loadGenerator/wlgen/WGController.cc \
../tool/loadGenerator/wlgen/WGServer.cc \
../tool/loadGenerator/wlgen/WorkloadGenerator.cc \
../tool/loadGenerator/wlgen/XmlHandler.cc \
../tool/loadGenerator/wlgen/workloadGenerator.cc 

OBJS += \
./tool/loadGenerator/wlgen/AggSourceFileReader.o \
./tool/loadGenerator/wlgen/AggStreamSource.o \
./tool/loadGenerator/wlgen/AttributeSource.o \
./tool/loadGenerator/wlgen/GenDataType.o \
./tool/loadGenerator/wlgen/RandomNumGenerator.o \
./tool/loadGenerator/wlgen/SingleSource.o \
./tool/loadGenerator/wlgen/SourceGenerator.o \
./tool/loadGenerator/wlgen/SourcePool.o \
./tool/loadGenerator/wlgen/StreamBuffer.o \
./tool/loadGenerator/wlgen/StreamSender.o \
./tool/loadGenerator/wlgen/StreamTuple.o \
./tool/loadGenerator/wlgen/TimeStampGenerator.o \
./tool/loadGenerator/wlgen/TuplesSave.o \
./tool/loadGenerator/wlgen/WGController.o \
./tool/loadGenerator/wlgen/WGServer.o \
./tool/loadGenerator/wlgen/WorkloadGenerator.o \
./tool/loadGenerator/wlgen/XmlHandler.o \
./tool/loadGenerator/wlgen/workloadGenerator.o 

CC_DEPS += \
./tool/loadGenerator/wlgen/AggSourceFileReader.d \
./tool/loadGenerator/wlgen/AggStreamSource.d \
./tool/loadGenerator/wlgen/AttributeSource.d \
./tool/loadGenerator/wlgen/GenDataType.d \
./tool/loadGenerator/wlgen/RandomNumGenerator.d \
./tool/loadGenerator/wlgen/SingleSource.d \
./tool/loadGenerator/wlgen/SourceGenerator.d \
./tool/loadGenerator/wlgen/SourcePool.d \
./tool/loadGenerator/wlgen/StreamBuffer.d \
./tool/loadGenerator/wlgen/StreamSender.d \
./tool/loadGenerator/wlgen/StreamTuple.d \
./tool/loadGenerator/wlgen/TimeStampGenerator.d \
./tool/loadGenerator/wlgen/TuplesSave.d \
./tool/loadGenerator/wlgen/WGController.d \
./tool/loadGenerator/wlgen/WGServer.d \
./tool/loadGenerator/wlgen/WorkloadGenerator.d \
./tool/loadGenerator/wlgen/XmlHandler.d \
./tool/loadGenerator/wlgen/workloadGenerator.d 


# Each subdirectory must supply rules for building sources it contributes
tool/loadGenerator/wlgen/%.o: ../tool/loadGenerator/wlgen/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


