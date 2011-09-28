################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../test/composite/loadnetwork/loadnetwork_test.cc 

OBJS += \
./test/composite/loadnetwork/loadnetwork_test.o 

CC_DEPS += \
./test/composite/loadnetwork/loadnetwork_test.d 


# Each subdirectory must supply rules for building sources it contributes
test/composite/loadnetwork/%.o: ../test/composite/loadnetwork/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


