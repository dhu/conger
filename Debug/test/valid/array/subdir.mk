################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../test/valid/array/validate_chunk.cc \
../test/valid/array/validate_dense.cc \
../test/valid/array/validate_sparse.cc 

OBJS += \
./test/valid/array/validate_chunk.o \
./test/valid/array/validate_dense.o \
./test/valid/array/validate_sparse.o 

CC_DEPS += \
./test/valid/array/validate_chunk.d \
./test/valid/array/validate_dense.d \
./test/valid/array/validate_sparse.d 


# Each subdirectory must supply rules for building sources it contributes
test/valid/array/%.o: ../test/valid/array/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


