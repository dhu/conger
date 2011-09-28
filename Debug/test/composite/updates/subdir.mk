################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../test/composite/updates/append_queue.cc \
../test/composite/updates/append_queue_slide.cc \
../test/composite/updates/append_queue_tumble.cc \
../test/composite/updates/ls.cc \
../test/composite/updates/ls_slide.cc \
../test/composite/updates/ls_tumble.cc \
../test/composite/updates/update_queue0.cc \
../test/composite/updates/update_queue1.cc \
../test/composite/updates/update_queue2.cc \
../test/composite/updates/update_queue3.cc \
../test/composite/updates/update_queue4.cc \
../test/composite/updates/update_queue5.cc \
../test/composite/updates/update_queue6.cc \
../test/composite/updates/update_queue7.cc \
../test/composite/updates/update_queue_slide0.cc \
../test/composite/updates/update_queue_slide1.cc \
../test/composite/updates/update_queue_slide2.cc \
../test/composite/updates/update_queue_slide3.cc \
../test/composite/updates/update_queue_slide4.cc \
../test/composite/updates/update_queue_slide5.cc \
../test/composite/updates/update_queue_tumble0.cc \
../test/composite/updates/update_queue_tumble1.cc \
../test/composite/updates/update_queue_tumble2.cc \
../test/composite/updates/update_queue_tumble3.cc \
../test/composite/updates/update_queue_tumble4.cc \
../test/composite/updates/update_queue_tumble5.cc 

OBJS += \
./test/composite/updates/append_queue.o \
./test/composite/updates/append_queue_slide.o \
./test/composite/updates/append_queue_tumble.o \
./test/composite/updates/ls.o \
./test/composite/updates/ls_slide.o \
./test/composite/updates/ls_tumble.o \
./test/composite/updates/update_queue0.o \
./test/composite/updates/update_queue1.o \
./test/composite/updates/update_queue2.o \
./test/composite/updates/update_queue3.o \
./test/composite/updates/update_queue4.o \
./test/composite/updates/update_queue5.o \
./test/composite/updates/update_queue6.o \
./test/composite/updates/update_queue7.o \
./test/composite/updates/update_queue_slide0.o \
./test/composite/updates/update_queue_slide1.o \
./test/composite/updates/update_queue_slide2.o \
./test/composite/updates/update_queue_slide3.o \
./test/composite/updates/update_queue_slide4.o \
./test/composite/updates/update_queue_slide5.o \
./test/composite/updates/update_queue_tumble0.o \
./test/composite/updates/update_queue_tumble1.o \
./test/composite/updates/update_queue_tumble2.o \
./test/composite/updates/update_queue_tumble3.o \
./test/composite/updates/update_queue_tumble4.o \
./test/composite/updates/update_queue_tumble5.o 

CC_DEPS += \
./test/composite/updates/append_queue.d \
./test/composite/updates/append_queue_slide.d \
./test/composite/updates/append_queue_tumble.d \
./test/composite/updates/ls.d \
./test/composite/updates/ls_slide.d \
./test/composite/updates/ls_tumble.d \
./test/composite/updates/update_queue0.d \
./test/composite/updates/update_queue1.d \
./test/composite/updates/update_queue2.d \
./test/composite/updates/update_queue3.d \
./test/composite/updates/update_queue4.d \
./test/composite/updates/update_queue5.d \
./test/composite/updates/update_queue6.d \
./test/composite/updates/update_queue7.d \
./test/composite/updates/update_queue_slide0.d \
./test/composite/updates/update_queue_slide1.d \
./test/composite/updates/update_queue_slide2.d \
./test/composite/updates/update_queue_slide3.d \
./test/composite/updates/update_queue_slide4.d \
./test/composite/updates/update_queue_slide5.d \
./test/composite/updates/update_queue_tumble0.d \
./test/composite/updates/update_queue_tumble1.d \
./test/composite/updates/update_queue_tumble2.d \
./test/composite/updates/update_queue_tumble3.d \
./test/composite/updates/update_queue_tumble4.d \
./test/composite/updates/update_queue_tumble5.d 


# Each subdirectory must supply rules for building sources it contributes
test/composite/updates/%.o: ../test/composite/updates/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


