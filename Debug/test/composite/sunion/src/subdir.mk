################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../test/composite/sunion/src/QueryMarshal.cc \
../test/composite/sunion/src/SUnionTestParams.cc \
../test/composite/sunion/src/suniondeploy.cc \
../test/composite/sunion/src/sunionin.cc 

OBJS += \
./test/composite/sunion/src/QueryMarshal.o \
./test/composite/sunion/src/SUnionTestParams.o \
./test/composite/sunion/src/suniondeploy.o \
./test/composite/sunion/src/sunionin.o 

CC_DEPS += \
./test/composite/sunion/src/QueryMarshal.d \
./test/composite/sunion/src/SUnionTestParams.d \
./test/composite/sunion/src/suniondeploy.d \
./test/composite/sunion/src/sunionin.d 


# Each subdirectory must supply rules for building sources it contributes
test/composite/sunion/src/%.o: ../test/composite/sunion/src/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


