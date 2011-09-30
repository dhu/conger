################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../test/developer/aggregate_test.cc \
../test/developer/aggregaterevtest.cc \
../test/developer/downstreamdemo1.cc \
../test/developer/downstreamdemo2.cc \
../test/developer/dynamic_mod_test.cc \
../test/developer/filter_test.cc \
../test/developer/lmtest.cc \
../test/developer/loop.cc \
../test/developer/lstest.cc \
../test/developer/lstest2.cc \
../test/developer/lstest_aggregate.cc \
../test/developer/lstest_simple.cc \
../test/developer/lstest_simple_aggregate.cc \
../test/developer/lstest_split.cc \
../test/developer/lstest_split2.cc \
../test/developer/lstest_two_server.cc \
../test/developer/maptest.cc \
../test/developer/movebox_union_test.cc \
../test/developer/myoldtest.cc \
../test/developer/revisiontest.cc \
../test/developer/rpctest.cc \
../test/developer/tuple_stime_test.cc \
../test/developer/two_server_test.cc \
../test/developer/upstreamdemo1.cc \
../test/developer/upstreamdemo2.cc 

OBJS += \
./test/developer/aggregate_test.o \
./test/developer/aggregaterevtest.o \
./test/developer/downstreamdemo1.o \
./test/developer/downstreamdemo2.o \
./test/developer/dynamic_mod_test.o \
./test/developer/filter_test.o \
./test/developer/lmtest.o \
./test/developer/loop.o \
./test/developer/lstest.o \
./test/developer/lstest2.o \
./test/developer/lstest_aggregate.o \
./test/developer/lstest_simple.o \
./test/developer/lstest_simple_aggregate.o \
./test/developer/lstest_split.o \
./test/developer/lstest_split2.o \
./test/developer/lstest_two_server.o \
./test/developer/maptest.o \
./test/developer/movebox_union_test.o \
./test/developer/myoldtest.o \
./test/developer/revisiontest.o \
./test/developer/rpctest.o \
./test/developer/tuple_stime_test.o \
./test/developer/two_server_test.o \
./test/developer/upstreamdemo1.o \
./test/developer/upstreamdemo2.o 

CC_DEPS += \
./test/developer/aggregate_test.d \
./test/developer/aggregaterevtest.d \
./test/developer/downstreamdemo1.d \
./test/developer/downstreamdemo2.d \
./test/developer/dynamic_mod_test.d \
./test/developer/filter_test.d \
./test/developer/lmtest.d \
./test/developer/loop.d \
./test/developer/lstest.d \
./test/developer/lstest2.d \
./test/developer/lstest_aggregate.d \
./test/developer/lstest_simple.d \
./test/developer/lstest_simple_aggregate.d \
./test/developer/lstest_split.d \
./test/developer/lstest_split2.d \
./test/developer/lstest_two_server.d \
./test/developer/maptest.d \
./test/developer/movebox_union_test.d \
./test/developer/myoldtest.d \
./test/developer/revisiontest.d \
./test/developer/rpctest.d \
./test/developer/tuple_stime_test.d \
./test/developer/two_server_test.d \
./test/developer/upstreamdemo1.d \
./test/developer/upstreamdemo2.d 


# Each subdirectory must supply rules for building sources it contributes
test/developer/%.o: ../test/developer/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


