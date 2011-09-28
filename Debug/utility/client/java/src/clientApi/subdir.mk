################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../utility/client/java/src/clientApi/subscribe.cc 

OBJS += \
./utility/client/java/src/clientApi/subscribe.o 

CC_DEPS += \
./utility/client/java/src/clientApi/subscribe.d 


# Each subdirectory must supply rules for building sources it contributes
utility/client/java/src/clientApi/%.o: ../utility/client/java/src/clientApi/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


