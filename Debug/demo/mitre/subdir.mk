################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../demo/mitre/Mitre_DemoMarshal.cc \
../demo/mitre/mitre_demo.cc 

OBJS += \
./demo/mitre/Mitre_DemoMarshal.o \
./demo/mitre/mitre_demo.o 

CC_DEPS += \
./demo/mitre/Mitre_DemoMarshal.d \
./demo/mitre/mitre_demo.d 


# Each subdirectory must supply rules for building sources it contributes
demo/mitre/%.o: ../demo/mitre/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


