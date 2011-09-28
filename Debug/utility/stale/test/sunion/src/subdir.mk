################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../utility/stale/test/sunion/src/suniontest.cc \
../utility/stale/test/sunion/src/suniontestin.cc \
../utility/stale/test/sunion/src/suniontestmiddle.cc \
../utility/stale/test/sunion/src/suniontestout.cc 

OBJS += \
./utility/stale/test/sunion/src/suniontest.o \
./utility/stale/test/sunion/src/suniontestin.o \
./utility/stale/test/sunion/src/suniontestmiddle.o \
./utility/stale/test/sunion/src/suniontestout.o 

CC_DEPS += \
./utility/stale/test/sunion/src/suniontest.d \
./utility/stale/test/sunion/src/suniontestin.d \
./utility/stale/test/sunion/src/suniontestmiddle.d \
./utility/stale/test/sunion/src/suniontestout.d 


# Each subdirectory must supply rules for building sources it contributes
utility/stale/test/sunion/src/%.o: ../utility/stale/test/sunion/src/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


