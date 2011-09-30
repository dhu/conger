################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../test/composite/loadbox/DynamicFilterQBox.cc \
../test/composite/loadbox/loadbox_test.cc 

OBJS += \
./test/composite/loadbox/DynamicFilterQBox.o \
./test/composite/loadbox/loadbox_test.o 

CC_DEPS += \
./test/composite/loadbox/DynamicFilterQBox.d \
./test/composite/loadbox/loadbox_test.d 


# Each subdirectory must supply rules for building sources it contributes
test/composite/loadbox/%.o: ../test/composite/loadbox/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


