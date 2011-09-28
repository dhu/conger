################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../test/composite/deltaProcessing/stars/motion_vector.cc \
../test/composite/deltaProcessing/stars/uncompressed.cc 

OBJS += \
./test/composite/deltaProcessing/stars/motion_vector.o \
./test/composite/deltaProcessing/stars/uncompressed.o 

CC_DEPS += \
./test/composite/deltaProcessing/stars/motion_vector.d \
./test/composite/deltaProcessing/stars/uncompressed.d 


# Each subdirectory must supply rules for building sources it contributes
test/composite/deltaProcessing/stars/%.o: ../test/composite/deltaProcessing/stars/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


