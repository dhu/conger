################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../test/valid/catalog/validate_catalog.cc 

OBJS += \
./test/valid/catalog/validate_catalog.o 

CC_DEPS += \
./test/valid/catalog/validate_catalog.d 


# Each subdirectory must supply rules for building sources it contributes
test/valid/catalog/%.o: ../test/valid/catalog/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


