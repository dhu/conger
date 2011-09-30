################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/modules/common/BasicComponent.cc \
../src/modules/common/Box.cc \
../src/modules/common/BoxPackage.cc \
../src/modules/common/CPViewDescription.cc \
../src/modules/common/CheckpointMessage.cc \
../src/modules/common/DataHandler.cc \
../src/modules/common/DataType.cc \
../src/modules/common/FeasibleInputTable.cc \
../src/modules/common/HAMethod.cc \
../src/modules/common/HAType.cc \
../src/modules/common/HAUnit.cc \
../src/modules/common/LSPlan.cc \
../src/modules/common/LoadOffer.cc \
../src/modules/common/MedusaID.cc \
../src/modules/common/NodeType.cc \
../src/modules/common/Object.cc \
../src/modules/common/Query.cc \
../src/modules/common/QueueTrimmingMessage.cc \
../src/modules/common/RecoveryTime.cc \
../src/modules/common/Schema.cc \
../src/modules/common/SlidingWindowStatistics.cc \
../src/modules/common/StreamDef.cc \
../src/modules/common/StreamProperties.cc \
../src/modules/common/TableDescription.cc \
../src/modules/common/Tuple.cc \
../src/modules/common/TupleDescription.cc \
../src/modules/common/TupleHeader.cc \
../src/modules/common/common.cc 

OBJS += \
./src/modules/common/BasicComponent.o \
./src/modules/common/Box.o \
./src/modules/common/BoxPackage.o \
./src/modules/common/CPViewDescription.o \
./src/modules/common/CheckpointMessage.o \
./src/modules/common/DataHandler.o \
./src/modules/common/DataType.o \
./src/modules/common/FeasibleInputTable.o \
./src/modules/common/HAMethod.o \
./src/modules/common/HAType.o \
./src/modules/common/HAUnit.o \
./src/modules/common/LSPlan.o \
./src/modules/common/LoadOffer.o \
./src/modules/common/MedusaID.o \
./src/modules/common/NodeType.o \
./src/modules/common/Object.o \
./src/modules/common/Query.o \
./src/modules/common/QueueTrimmingMessage.o \
./src/modules/common/RecoveryTime.o \
./src/modules/common/Schema.o \
./src/modules/common/SlidingWindowStatistics.o \
./src/modules/common/StreamDef.o \
./src/modules/common/StreamProperties.o \
./src/modules/common/TableDescription.o \
./src/modules/common/Tuple.o \
./src/modules/common/TupleDescription.o \
./src/modules/common/TupleHeader.o \
./src/modules/common/common.o 

CC_DEPS += \
./src/modules/common/BasicComponent.d \
./src/modules/common/Box.d \
./src/modules/common/BoxPackage.d \
./src/modules/common/CPViewDescription.d \
./src/modules/common/CheckpointMessage.d \
./src/modules/common/DataHandler.d \
./src/modules/common/DataType.d \
./src/modules/common/FeasibleInputTable.d \
./src/modules/common/HAMethod.d \
./src/modules/common/HAType.d \
./src/modules/common/HAUnit.d \
./src/modules/common/LSPlan.d \
./src/modules/common/LoadOffer.d \
./src/modules/common/MedusaID.d \
./src/modules/common/NodeType.d \
./src/modules/common/Object.d \
./src/modules/common/Query.d \
./src/modules/common/QueueTrimmingMessage.d \
./src/modules/common/RecoveryTime.d \
./src/modules/common/Schema.d \
./src/modules/common/SlidingWindowStatistics.d \
./src/modules/common/StreamDef.d \
./src/modules/common/StreamProperties.d \
./src/modules/common/TableDescription.d \
./src/modules/common/Tuple.d \
./src/modules/common/TupleDescription.d \
./src/modules/common/TupleHeader.d \
./src/modules/common/common.d 


# Each subdirectory must supply rules for building sources it contributes
src/modules/common/%.o: ../src/modules/common/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


