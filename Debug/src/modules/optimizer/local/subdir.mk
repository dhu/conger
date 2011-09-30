################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/modules/optimizer/local/LocalLoadShedder.cc \
../src/modules/optimizer/local/LocalOptimizer.cc 

OBJS += \
./src/modules/optimizer/local/LocalLoadShedder.o \
./src/modules/optimizer/local/LocalOptimizer.o 

CC_DEPS += \
./src/modules/optimizer/local/LocalLoadShedder.d \
./src/modules/optimizer/local/LocalOptimizer.d 


# Each subdirectory must supply rules for building sources it contributes
src/modules/optimizer/local/%.o: ../src/modules/optimizer/local/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


