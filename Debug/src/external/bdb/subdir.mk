################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/external/bdb/CatalogBdb.cc \
../src/external/bdb/DeleteQBox.cc \
../src/external/bdb/ExistsQBox.cc \
../src/external/bdb/InsertQBox.cc \
../src/external/bdb/InstantiateBdb.cc \
../src/external/bdb/NodeTable.cc \
../src/external/bdb/SelectQBox.cc \
../src/external/bdb/Table.cc \
../src/external/bdb/TableCursors.cc \
../src/external/bdb/UpdateQBox.cc 

OBJS += \
./src/external/bdb/CatalogBdb.o \
./src/external/bdb/DeleteQBox.o \
./src/external/bdb/ExistsQBox.o \
./src/external/bdb/InsertQBox.o \
./src/external/bdb/InstantiateBdb.o \
./src/external/bdb/NodeTable.o \
./src/external/bdb/SelectQBox.o \
./src/external/bdb/Table.o \
./src/external/bdb/TableCursors.o \
./src/external/bdb/UpdateQBox.o 

CC_DEPS += \
./src/external/bdb/CatalogBdb.d \
./src/external/bdb/DeleteQBox.d \
./src/external/bdb/ExistsQBox.d \
./src/external/bdb/InsertQBox.d \
./src/external/bdb/InstantiateBdb.d \
./src/external/bdb/NodeTable.d \
./src/external/bdb/SelectQBox.d \
./src/external/bdb/Table.d \
./src/external/bdb/TableCursors.d \
./src/external/bdb/UpdateQBox.d 


# Each subdirectory must supply rules for building sources it contributes
src/external/bdb/%.o: ../src/external/bdb/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


