################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../test/valid/revisions/validate_downstream_revisionaggregate.cc \
../test/valid/revisions/validate_upstream_revisionaggregate.cc \
../test/valid/revisions/validate_upstream_revisionfilter.cc \
../test/valid/revisions/validate_upstream_revisionmap.cc 

OBJS += \
./test/valid/revisions/validate_downstream_revisionaggregate.o \
./test/valid/revisions/validate_upstream_revisionaggregate.o \
./test/valid/revisions/validate_upstream_revisionfilter.o \
./test/valid/revisions/validate_upstream_revisionmap.o 

CC_DEPS += \
./test/valid/revisions/validate_downstream_revisionaggregate.d \
./test/valid/revisions/validate_upstream_revisionaggregate.d \
./test/valid/revisions/validate_upstream_revisionfilter.d \
./test/valid/revisions/validate_upstream_revisionmap.d 


# Each subdirectory must supply rules for building sources it contributes
test/valid/revisions/%.o: ../test/valid/revisions/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


