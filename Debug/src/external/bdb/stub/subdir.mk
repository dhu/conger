################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/external/bdb/stub/CatalogBdb.cc \
../src/external/bdb/stub/InstantiateBdb.cc \
../src/external/bdb/stub/NodeTable.cc 

OBJS += \
./src/external/bdb/stub/CatalogBdb.o \
./src/external/bdb/stub/InstantiateBdb.o \
./src/external/bdb/stub/NodeTable.o 

CC_DEPS += \
./src/external/bdb/stub/CatalogBdb.d \
./src/external/bdb/stub/InstantiateBdb.d \
./src/external/bdb/stub/NodeTable.d 


# Each subdirectory must supply rules for building sources it contributes
src/external/bdb/stub/%.o: ../src/external/bdb/stub/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


