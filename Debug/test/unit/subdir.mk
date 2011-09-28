################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../test/unit/Diagramtest.cc \
../test/unit/Headtest.cc \
../test/unit/Regiontest.cc \
../test/unit/statsMgrtest.cc 

OBJS += \
./test/unit/Diagramtest.o \
./test/unit/Headtest.o \
./test/unit/Regiontest.o \
./test/unit/statsMgrtest.o 

CC_DEPS += \
./test/unit/Diagramtest.d \
./test/unit/Headtest.d \
./test/unit/Regiontest.d \
./test/unit/statsMgrtest.d 


# Each subdirectory must supply rules for building sources it contributes
test/unit/%.o: ../test/unit/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


