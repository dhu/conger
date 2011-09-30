################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../tool/optimizer/loadShedder/LoadShedder.cc \
../tool/optimizer/loadShedder/LoadSheddingControl.cc \
../tool/optimizer/loadShedder/LoadSheddingTool.cc 

OBJS += \
./tool/optimizer/loadShedder/LoadShedder.o \
./tool/optimizer/loadShedder/LoadSheddingControl.o \
./tool/optimizer/loadShedder/LoadSheddingTool.o 

CC_DEPS += \
./tool/optimizer/loadShedder/LoadShedder.d \
./tool/optimizer/loadShedder/LoadSheddingControl.d \
./tool/optimizer/loadShedder/LoadSheddingTool.d 


# Each subdirectory must supply rules for building sources it contributes
tool/optimizer/loadShedder/%.o: ../tool/optimizer/loadShedder/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


