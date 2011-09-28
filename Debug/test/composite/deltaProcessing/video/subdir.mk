################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../test/composite/deltaProcessing/video/adjacent_heavy.cc \
../test/composite/deltaProcessing/video/adjacent_light.cc \
../test/composite/deltaProcessing/video/batch_heavy.cc \
../test/composite/deltaProcessing/video/batch_light.cc \
../test/composite/deltaProcessing/video/chunk_heavy.cc \
../test/composite/deltaProcessing/video/chunk_light.cc \
../test/composite/deltaProcessing/video/face_detect_heavy.cc \
../test/composite/deltaProcessing/video/face_detect_light.cc \
../test/composite/deltaProcessing/video/recurse_heavy.cc \
../test/composite/deltaProcessing/video/recurse_light.cc \
../test/composite/deltaProcessing/video/sample_heavy.cc \
../test/composite/deltaProcessing/video/sample_light.cc \
../test/composite/deltaProcessing/video/spatial_heavy.cc \
../test/composite/deltaProcessing/video/spatial_light.cc 

OBJS += \
./test/composite/deltaProcessing/video/adjacent_heavy.o \
./test/composite/deltaProcessing/video/adjacent_light.o \
./test/composite/deltaProcessing/video/batch_heavy.o \
./test/composite/deltaProcessing/video/batch_light.o \
./test/composite/deltaProcessing/video/chunk_heavy.o \
./test/composite/deltaProcessing/video/chunk_light.o \
./test/composite/deltaProcessing/video/face_detect_heavy.o \
./test/composite/deltaProcessing/video/face_detect_light.o \
./test/composite/deltaProcessing/video/recurse_heavy.o \
./test/composite/deltaProcessing/video/recurse_light.o \
./test/composite/deltaProcessing/video/sample_heavy.o \
./test/composite/deltaProcessing/video/sample_light.o \
./test/composite/deltaProcessing/video/spatial_heavy.o \
./test/composite/deltaProcessing/video/spatial_light.o 

CC_DEPS += \
./test/composite/deltaProcessing/video/adjacent_heavy.d \
./test/composite/deltaProcessing/video/adjacent_light.d \
./test/composite/deltaProcessing/video/batch_heavy.d \
./test/composite/deltaProcessing/video/batch_light.d \
./test/composite/deltaProcessing/video/chunk_heavy.d \
./test/composite/deltaProcessing/video/chunk_light.d \
./test/composite/deltaProcessing/video/face_detect_heavy.d \
./test/composite/deltaProcessing/video/face_detect_light.d \
./test/composite/deltaProcessing/video/recurse_heavy.d \
./test/composite/deltaProcessing/video/recurse_light.d \
./test/composite/deltaProcessing/video/sample_heavy.d \
./test/composite/deltaProcessing/video/sample_light.d \
./test/composite/deltaProcessing/video/spatial_heavy.d \
./test/composite/deltaProcessing/video/spatial_light.d 


# Each subdirectory must supply rules for building sources it contributes
test/composite/deltaProcessing/video/%.o: ../test/composite/deltaProcessing/video/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


