################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../test/composite/fault/FaulttestMarshal.cc \
../test/composite/fault/SunionsMarshal.cc \
../test/composite/fault/faulttest.cc \
../test/composite/fault/sunions.cc 

OBJS += \
./test/composite/fault/FaulttestMarshal.o \
./test/composite/fault/SunionsMarshal.o \
./test/composite/fault/faulttest.o \
./test/composite/fault/sunions.o 

CC_DEPS += \
./test/composite/fault/FaulttestMarshal.d \
./test/composite/fault/SunionsMarshal.d \
./test/composite/fault/faulttest.d \
./test/composite/fault/sunions.d 


# Each subdirectory must supply rules for building sources it contributes
test/composite/fault/%.o: ../test/composite/fault/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


