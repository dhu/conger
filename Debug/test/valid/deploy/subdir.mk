################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../test/valid/deploy/validate_add.cc \
../test/valid/deploy/validate_deploy.cc 

OBJS += \
./test/valid/deploy/validate_add.o \
./test/valid/deploy/validate_deploy.o 

CC_DEPS += \
./test/valid/deploy/validate_add.d \
./test/valid/deploy/validate_deploy.d 


# Each subdirectory must supply rules for building sources it contributes
test/valid/deploy/%.o: ../test/valid/deploy/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


