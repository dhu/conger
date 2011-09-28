################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../test/valid/connect/region_connect.cc \
../test/valid/connect/region_connect1.cc \
../test/valid/connect/region_connect2.cc \
../test/valid/connect/region_connect3.cc \
../test/valid/connect/region_connect4.cc \
../test/valid/connect/region_connect_head.cc \
../test/valid/connect/region_connect_main.cc \
../test/valid/connect/region_connect_wrap.cc \
../test/valid/connect/validate_connect.cc \
../test/valid/connect/validate_connect1.cc \
../test/valid/connect/validate_connect2.cc \
../test/valid/connect/validate_connect3.cc \
../test/valid/connect/validate_connect4.cc 

OBJS += \
./test/valid/connect/region_connect.o \
./test/valid/connect/region_connect1.o \
./test/valid/connect/region_connect2.o \
./test/valid/connect/region_connect3.o \
./test/valid/connect/region_connect4.o \
./test/valid/connect/region_connect_head.o \
./test/valid/connect/region_connect_main.o \
./test/valid/connect/region_connect_wrap.o \
./test/valid/connect/validate_connect.o \
./test/valid/connect/validate_connect1.o \
./test/valid/connect/validate_connect2.o \
./test/valid/connect/validate_connect3.o \
./test/valid/connect/validate_connect4.o 

CC_DEPS += \
./test/valid/connect/region_connect.d \
./test/valid/connect/region_connect1.d \
./test/valid/connect/region_connect2.d \
./test/valid/connect/region_connect3.d \
./test/valid/connect/region_connect4.d \
./test/valid/connect/region_connect_head.d \
./test/valid/connect/region_connect_main.d \
./test/valid/connect/region_connect_wrap.d \
./test/valid/connect/validate_connect.d \
./test/valid/connect/validate_connect1.d \
./test/valid/connect/validate_connect2.d \
./test/valid/connect/validate_connect3.d \
./test/valid/connect/validate_connect4.d 


# Each subdirectory must supply rules for building sources it contributes
test/valid/connect/%.o: ../test/valid/connect/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


