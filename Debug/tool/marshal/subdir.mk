################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../tool/marshal/GenerateBody.o \
../tool/marshal/GenerateHeader.o \
../tool/marshal/GenerateMarshal.o \
../tool/marshal/MarshalDiagram.o \
../tool/marshal/marshal.o 

CC_SRCS += \
../tool/marshal/GenerateBody.cc \
../tool/marshal/GenerateHeader.cc \
../tool/marshal/GenerateMarshal.cc \
../tool/marshal/MarshalDiagram.cc \
../tool/marshal/marshal.cc 

OBJS += \
./tool/marshal/GenerateBody.o \
./tool/marshal/GenerateHeader.o \
./tool/marshal/GenerateMarshal.o \
./tool/marshal/MarshalDiagram.o \
./tool/marshal/marshal.o 

CC_DEPS += \
./tool/marshal/GenerateBody.d \
./tool/marshal/GenerateHeader.d \
./tool/marshal/GenerateMarshal.d \
./tool/marshal/MarshalDiagram.d \
./tool/marshal/marshal.d 


# Each subdirectory must supply rules for building sources it contributes
tool/marshal/%.o: ../tool/marshal/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


