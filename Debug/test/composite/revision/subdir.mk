################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../test/composite/revision/downstreamtest1.cc \
../test/composite/revision/downstreamtest2.cc \
../test/composite/revision/downstreamtest3.cc \
../test/composite/revision/downstreamtest4.cc \
../test/composite/revision/downstreamtest5.cc \
../test/composite/revision/downstreamtest6.cc \
../test/composite/revision/downstreamtest7.cc \
../test/composite/revision/upstreamdemo1.cc \
../test/composite/revision/upstreamdemo2.cc \
../test/composite/revision/upstreamtest1.cc \
../test/composite/revision/upstreamtest2.cc \
../test/composite/revision/upstreamtest3.cc \
../test/composite/revision/upstreamtest4.cc \
../test/composite/revision/upstreamtest5.cc \
../test/composite/revision/upstreamtest6.cc \
../test/composite/revision/upstreamtest7.cc 

OBJS += \
./test/composite/revision/downstreamtest1.o \
./test/composite/revision/downstreamtest2.o \
./test/composite/revision/downstreamtest3.o \
./test/composite/revision/downstreamtest4.o \
./test/composite/revision/downstreamtest5.o \
./test/composite/revision/downstreamtest6.o \
./test/composite/revision/downstreamtest7.o \
./test/composite/revision/upstreamdemo1.o \
./test/composite/revision/upstreamdemo2.o \
./test/composite/revision/upstreamtest1.o \
./test/composite/revision/upstreamtest2.o \
./test/composite/revision/upstreamtest3.o \
./test/composite/revision/upstreamtest4.o \
./test/composite/revision/upstreamtest5.o \
./test/composite/revision/upstreamtest6.o \
./test/composite/revision/upstreamtest7.o 

CC_DEPS += \
./test/composite/revision/downstreamtest1.d \
./test/composite/revision/downstreamtest2.d \
./test/composite/revision/downstreamtest3.d \
./test/composite/revision/downstreamtest4.d \
./test/composite/revision/downstreamtest5.d \
./test/composite/revision/downstreamtest6.d \
./test/composite/revision/downstreamtest7.d \
./test/composite/revision/upstreamdemo1.d \
./test/composite/revision/upstreamdemo2.d \
./test/composite/revision/upstreamtest1.d \
./test/composite/revision/upstreamtest2.d \
./test/composite/revision/upstreamtest3.d \
./test/composite/revision/upstreamtest4.d \
./test/composite/revision/upstreamtest5.d \
./test/composite/revision/upstreamtest6.d \
./test/composite/revision/upstreamtest7.d 


# Each subdirectory must supply rules for building sources it contributes
test/composite/revision/%.o: ../test/composite/revision/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


