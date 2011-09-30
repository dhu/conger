################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../test/composite/NHLoadShedder/nh_ls.cc \
../test/composite/NHLoadShedder/nh_ls_equal.cc \
../test/composite/NHLoadShedder/nh_ls_one.cc \
../test/composite/NHLoadShedder/nh_ls_two.cc 

OBJS += \
./test/composite/NHLoadShedder/nh_ls.o \
./test/composite/NHLoadShedder/nh_ls_equal.o \
./test/composite/NHLoadShedder/nh_ls_one.o \
./test/composite/NHLoadShedder/nh_ls_two.o 

CC_DEPS += \
./test/composite/NHLoadShedder/nh_ls.d \
./test/composite/NHLoadShedder/nh_ls_equal.d \
./test/composite/NHLoadShedder/nh_ls_one.d \
./test/composite/NHLoadShedder/nh_ls_two.d 


# Each subdirectory must supply rules for building sources it contributes
test/composite/NHLoadShedder/%.o: ../test/composite/NHLoadShedder/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


