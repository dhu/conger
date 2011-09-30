################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../demo/parallel/PassMarshal.cc \
../demo/parallel/WorkerMarshal.cc \
../demo/parallel/WorkerMarshalCommon.cc \
../demo/parallel/validate_pass.cc \
../demo/parallel/validate_worker.cc 

OBJS += \
./demo/parallel/PassMarshal.o \
./demo/parallel/WorkerMarshal.o \
./demo/parallel/WorkerMarshalCommon.o \
./demo/parallel/validate_pass.o \
./demo/parallel/validate_worker.o 

CC_DEPS += \
./demo/parallel/PassMarshal.d \
./demo/parallel/WorkerMarshal.d \
./demo/parallel/WorkerMarshalCommon.d \
./demo/parallel/validate_pass.d \
./demo/parallel/validate_worker.d 


# Each subdirectory must supply rules for building sources it contributes
demo/parallel/%.o: ../demo/parallel/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


