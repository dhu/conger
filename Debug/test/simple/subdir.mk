################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../test/simple/MytestdistMarshal.cc \
../test/simple/aggregate_test.cc \
../test/simple/aurorajoin_test.cc \
../test/simple/cp2test.cc \
../test/simple/cptest.cc \
../test/simple/join_test.cc \
../test/simple/joinstatic_test.cc \
../test/simple/movebox_test.cc \
../test/simple/mytest.cc \
../test/simple/mytestdist.cc \
../test/simple/table_test.cc \
../test/simple/update_queue.cc \
../test/simple/update_queue1.cc \
../test/simple/update_queue2.cc 

OBJS += \
./test/simple/MytestdistMarshal.o \
./test/simple/aggregate_test.o \
./test/simple/aurorajoin_test.o \
./test/simple/cp2test.o \
./test/simple/cptest.o \
./test/simple/join_test.o \
./test/simple/joinstatic_test.o \
./test/simple/movebox_test.o \
./test/simple/mytest.o \
./test/simple/mytestdist.o \
./test/simple/table_test.o \
./test/simple/update_queue.o \
./test/simple/update_queue1.o \
./test/simple/update_queue2.o 

CC_DEPS += \
./test/simple/MytestdistMarshal.d \
./test/simple/aggregate_test.d \
./test/simple/aurorajoin_test.d \
./test/simple/cp2test.d \
./test/simple/cptest.d \
./test/simple/join_test.d \
./test/simple/joinstatic_test.d \
./test/simple/movebox_test.d \
./test/simple/mytest.d \
./test/simple/mytestdist.d \
./test/simple/table_test.d \
./test/simple/update_queue.d \
./test/simple/update_queue1.d \
./test/simple/update_queue2.d 


# Each subdirectory must supply rules for building sources it contributes
test/simple/%.o: ../test/simple/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


