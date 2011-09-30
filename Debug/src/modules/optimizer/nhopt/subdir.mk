################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/modules/optimizer/nhopt/ContractBasedOptimizer.cc \
../src/modules/optimizer/nhopt/NHLoadShedder.cc \
../src/modules/optimizer/nhopt/NHOptimizer.cc \
../src/modules/optimizer/nhopt/QOSOptimizer.cc 

OBJS += \
./src/modules/optimizer/nhopt/ContractBasedOptimizer.o \
./src/modules/optimizer/nhopt/NHLoadShedder.o \
./src/modules/optimizer/nhopt/NHOptimizer.o \
./src/modules/optimizer/nhopt/QOSOptimizer.o 

CC_DEPS += \
./src/modules/optimizer/nhopt/ContractBasedOptimizer.d \
./src/modules/optimizer/nhopt/NHLoadShedder.d \
./src/modules/optimizer/nhopt/NHOptimizer.d \
./src/modules/optimizer/nhopt/QOSOptimizer.d 


# Each subdirectory must supply rules for building sources it contributes
src/modules/optimizer/nhopt/%.o: ../src/modules/optimizer/nhopt/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


