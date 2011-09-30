################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../test/composite/GlobalLoadShedder/chain/chain_2_2.cc 

OBJS += \
./test/composite/GlobalLoadShedder/chain/chain_2_2.o 

CC_DEPS += \
./test/composite/GlobalLoadShedder/chain/chain_2_2.d 


# Each subdirectory must supply rules for building sources it contributes
test/composite/GlobalLoadShedder/chain/%.o: ../test/composite/GlobalLoadShedder/chain/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


