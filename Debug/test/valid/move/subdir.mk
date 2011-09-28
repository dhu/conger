################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../test/valid/move/region_move.cc \
../test/valid/move/validate_move.cc 

OBJS += \
./test/valid/move/region_move.o \
./test/valid/move/validate_move.o 

CC_DEPS += \
./test/valid/move/region_move.d \
./test/valid/move/validate_move.d 


# Each subdirectory must supply rules for building sources it contributes
test/valid/move/%.o: ../test/valid/move/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


