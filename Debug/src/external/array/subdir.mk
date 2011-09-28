################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/external/array/CatalogArray.cc \
../src/external/array/GatherQBox.cc \
../src/external/array/InstantiateArray.cc \
../src/external/array/ScatterQBox.cc 

OBJS += \
./src/external/array/CatalogArray.o \
./src/external/array/GatherQBox.o \
./src/external/array/InstantiateArray.o \
./src/external/array/ScatterQBox.o 

CC_DEPS += \
./src/external/array/CatalogArray.d \
./src/external/array/GatherQBox.d \
./src/external/array/InstantiateArray.d \
./src/external/array/ScatterQBox.d 


# Each subdirectory must supply rules for building sources it contributes
src/external/array/%.o: ../src/external/array/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


