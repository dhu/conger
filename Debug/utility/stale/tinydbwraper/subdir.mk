################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../utility/stale/tinydbwraper/Box.cc \
../utility/stale/tinydbwraper/Catalog.cc \
../utility/stale/tinydbwraper/Constraint.cc \
../utility/stale/tinydbwraper/DataThread.cc \
../utility/stale/tinydbwraper/Field.cc \
../utility/stale/tinydbwraper/JSocket.cc \
../utility/stale/tinydbwraper/LatencyFunction.cc \
../utility/stale/tinydbwraper/LifetimeFunction.cc \
../utility/stale/tinydbwraper/Metric.cc \
../utility/stale/tinydbwraper/Parameter.cc \
../utility/stale/tinydbwraper/Property.cc \
../utility/stale/tinydbwraper/QualityFunction.cc \
../utility/stale/tinydbwraper/Schema.cc \
../utility/stale/tinydbwraper/Score.cc \
../utility/stale/tinydbwraper/Scores.cc \
../utility/stale/tinydbwraper/Stream.cc \
../utility/stale/tinydbwraper/TQueries.cc \
../utility/stale/tinydbwraper/TQuery.cc \
../utility/stale/tinydbwraper/ThroughputFunction.cc \
../utility/stale/tinydbwraper/Wrapper.cc 

OBJS += \
./utility/stale/tinydbwraper/Box.o \
./utility/stale/tinydbwraper/Catalog.o \
./utility/stale/tinydbwraper/Constraint.o \
./utility/stale/tinydbwraper/DataThread.o \
./utility/stale/tinydbwraper/Field.o \
./utility/stale/tinydbwraper/JSocket.o \
./utility/stale/tinydbwraper/LatencyFunction.o \
./utility/stale/tinydbwraper/LifetimeFunction.o \
./utility/stale/tinydbwraper/Metric.o \
./utility/stale/tinydbwraper/Parameter.o \
./utility/stale/tinydbwraper/Property.o \
./utility/stale/tinydbwraper/QualityFunction.o \
./utility/stale/tinydbwraper/Schema.o \
./utility/stale/tinydbwraper/Score.o \
./utility/stale/tinydbwraper/Scores.o \
./utility/stale/tinydbwraper/Stream.o \
./utility/stale/tinydbwraper/TQueries.o \
./utility/stale/tinydbwraper/TQuery.o \
./utility/stale/tinydbwraper/ThroughputFunction.o \
./utility/stale/tinydbwraper/Wrapper.o 

CC_DEPS += \
./utility/stale/tinydbwraper/Box.d \
./utility/stale/tinydbwraper/Catalog.d \
./utility/stale/tinydbwraper/Constraint.d \
./utility/stale/tinydbwraper/DataThread.d \
./utility/stale/tinydbwraper/Field.d \
./utility/stale/tinydbwraper/JSocket.d \
./utility/stale/tinydbwraper/LatencyFunction.d \
./utility/stale/tinydbwraper/LifetimeFunction.d \
./utility/stale/tinydbwraper/Metric.d \
./utility/stale/tinydbwraper/Parameter.d \
./utility/stale/tinydbwraper/Property.d \
./utility/stale/tinydbwraper/QualityFunction.d \
./utility/stale/tinydbwraper/Schema.d \
./utility/stale/tinydbwraper/Score.d \
./utility/stale/tinydbwraper/Scores.d \
./utility/stale/tinydbwraper/Stream.d \
./utility/stale/tinydbwraper/TQueries.d \
./utility/stale/tinydbwraper/TQuery.d \
./utility/stale/tinydbwraper/ThroughputFunction.d \
./utility/stale/tinydbwraper/Wrapper.d 


# Each subdirectory must supply rules for building sources it contributes
utility/stale/tinydbwraper/%.o: ../utility/stale/tinydbwraper/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


