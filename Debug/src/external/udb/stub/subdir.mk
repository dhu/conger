################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/external/udb/stub/CatalogUdb.cc \
../src/external/udb/stub/InstantiateUdb.cc 

OBJS += \
./src/external/udb/stub/CatalogUdb.o \
./src/external/udb/stub/InstantiateUdb.o 

CC_DEPS += \
./src/external/udb/stub/CatalogUdb.d \
./src/external/udb/stub/InstantiateUdb.d 


# Each subdirectory must supply rules for building sources it contributes
src/external/udb/stub/%.o: ../src/external/udb/stub/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


