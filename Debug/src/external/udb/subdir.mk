################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/external/udb/CatalogUdb.cc \
../src/external/udb/CombineQBox.cc \
../src/external/udb/CountDistinct.cc \
../src/external/udb/DelayQBox.cc \
../src/external/udb/DelaySelQBox.cc \
../src/external/udb/FidAlarmCounterFunc.cc \
../src/external/udb/FidComplianceLookthroughAndAgg.cc \
../src/external/udb/FidLateTrackerFunc.cc \
../src/external/udb/InstantiateUdb.cc \
../src/external/udb/JoinStatic.cc \
../src/external/udb/LRAccidentAggFunc1.cc \
../src/external/udb/LRAccidentAggFunc2.cc \
../src/external/udb/LRAvg.cc \
../src/external/udb/LRCarCount.cc \
../src/external/udb/LRSecToMin.cc \
../src/external/udb/LRSegmentStatisticsFunc2.cc \
../src/external/udb/LRWaitForQBox.cc \
../src/external/udb/RandFunction.cc 

OBJS += \
./src/external/udb/CatalogUdb.o \
./src/external/udb/CombineQBox.o \
./src/external/udb/CountDistinct.o \
./src/external/udb/DelayQBox.o \
./src/external/udb/DelaySelQBox.o \
./src/external/udb/FidAlarmCounterFunc.o \
./src/external/udb/FidComplianceLookthroughAndAgg.o \
./src/external/udb/FidLateTrackerFunc.o \
./src/external/udb/InstantiateUdb.o \
./src/external/udb/JoinStatic.o \
./src/external/udb/LRAccidentAggFunc1.o \
./src/external/udb/LRAccidentAggFunc2.o \
./src/external/udb/LRAvg.o \
./src/external/udb/LRCarCount.o \
./src/external/udb/LRSecToMin.o \
./src/external/udb/LRSegmentStatisticsFunc2.o \
./src/external/udb/LRWaitForQBox.o \
./src/external/udb/RandFunction.o 

CC_DEPS += \
./src/external/udb/CatalogUdb.d \
./src/external/udb/CombineQBox.d \
./src/external/udb/CountDistinct.d \
./src/external/udb/DelayQBox.d \
./src/external/udb/DelaySelQBox.d \
./src/external/udb/FidAlarmCounterFunc.d \
./src/external/udb/FidComplianceLookthroughAndAgg.d \
./src/external/udb/FidLateTrackerFunc.d \
./src/external/udb/InstantiateUdb.d \
./src/external/udb/JoinStatic.d \
./src/external/udb/LRAccidentAggFunc1.d \
./src/external/udb/LRAccidentAggFunc2.d \
./src/external/udb/LRAvg.d \
./src/external/udb/LRCarCount.d \
./src/external/udb/LRSecToMin.d \
./src/external/udb/LRSegmentStatisticsFunc2.d \
./src/external/udb/LRWaitForQBox.d \
./src/external/udb/RandFunction.d 


# Each subdirectory must supply rules for building sources it contributes
src/external/udb/%.o: ../src/external/udb/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


