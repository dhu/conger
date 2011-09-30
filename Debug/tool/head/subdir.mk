################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../tool/head/BigGiantHead.o \
../tool/head/HeadServer.o 

CC_SRCS += \
../tool/head/BigGiantHead.cc \
../tool/head/DeployDiagram.cc \
../tool/head/HeadServer.cc 

OBJS += \
./tool/head/BigGiantHead.o \
./tool/head/DeployDiagram.o \
./tool/head/HeadServer.o 

CC_DEPS += \
./tool/head/BigGiantHead.d \
./tool/head/DeployDiagram.d \
./tool/head/HeadServer.d 


# Each subdirectory must supply rules for building sources it contributes
tool/head/%.o: ../tool/head/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


