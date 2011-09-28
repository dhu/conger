################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/external/array/stub/CatalogArray.cc \
../src/external/array/stub/InstantiateArray.cc 

OBJS += \
./src/external/array/stub/CatalogArray.o \
./src/external/array/stub/InstantiateArray.o 

CC_DEPS += \
./src/external/array/stub/CatalogArray.d \
./src/external/array/stub/InstantiateArray.d 


# Each subdirectory must supply rules for building sources it contributes
src/external/array/stub/%.o: ../src/external/array/stub/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


