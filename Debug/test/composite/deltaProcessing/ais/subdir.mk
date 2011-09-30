################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../test/composite/deltaProcessing/ais/csvparser.cc \
../test/composite/deltaProcessing/ais/magick_stub.cc \
../test/composite/deltaProcessing/ais/validate_ais.cc \
../test/composite/deltaProcessing/ais/validate_ais_delta.cc 

OBJS += \
./test/composite/deltaProcessing/ais/csvparser.o \
./test/composite/deltaProcessing/ais/magick_stub.o \
./test/composite/deltaProcessing/ais/validate_ais.o \
./test/composite/deltaProcessing/ais/validate_ais_delta.o 

CC_DEPS += \
./test/composite/deltaProcessing/ais/csvparser.d \
./test/composite/deltaProcessing/ais/magick_stub.d \
./test/composite/deltaProcessing/ais/validate_ais.d \
./test/composite/deltaProcessing/ais/validate_ais_delta.d 


# Each subdirectory must supply rules for building sources it contributes
test/composite/deltaProcessing/ais/%.o: ../test/composite/deltaProcessing/ais/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


