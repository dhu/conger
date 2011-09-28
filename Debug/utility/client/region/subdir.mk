################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../utility/client/region/RegionServer.cc \
../utility/client/region/RegionalCatalog.cc \
../utility/client/region/StatisticsServer.cc \
../utility/client/region/WriteRegion.cc 

OBJS += \
./utility/client/region/RegionServer.o \
./utility/client/region/RegionalCatalog.o \
./utility/client/region/StatisticsServer.o \
./utility/client/region/WriteRegion.o 

CC_DEPS += \
./utility/client/region/RegionServer.d \
./utility/client/region/RegionalCatalog.d \
./utility/client/region/StatisticsServer.d \
./utility/client/region/WriteRegion.d 


# Each subdirectory must supply rules for building sources it contributes
utility/client/region/%.o: ../utility/client/region/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


