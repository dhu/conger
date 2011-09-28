################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/modules/catalog/BoxParameter.cc \
../src/modules/catalog/BoxPort.cc \
../src/modules/catalog/Catalog.cc \
../src/modules/catalog/CatalogBox.cc \
../src/modules/catalog/CatalogQuery.cc \
../src/modules/catalog/CatalogSchema.cc \
../src/modules/catalog/CatalogStream.cc \
../src/modules/catalog/CatalogTable.cc \
../src/modules/catalog/CatalogValidate.cc \
../src/modules/catalog/CatalogView.cc \
../src/modules/catalog/Diagram.cc \
../src/modules/catalog/SchemaArray.cc 

OBJS += \
./src/modules/catalog/BoxParameter.o \
./src/modules/catalog/BoxPort.o \
./src/modules/catalog/Catalog.o \
./src/modules/catalog/CatalogBox.o \
./src/modules/catalog/CatalogQuery.o \
./src/modules/catalog/CatalogSchema.o \
./src/modules/catalog/CatalogStream.o \
./src/modules/catalog/CatalogTable.o \
./src/modules/catalog/CatalogValidate.o \
./src/modules/catalog/CatalogView.o \
./src/modules/catalog/Diagram.o \
./src/modules/catalog/SchemaArray.o 

CC_DEPS += \
./src/modules/catalog/BoxParameter.d \
./src/modules/catalog/BoxPort.d \
./src/modules/catalog/Catalog.d \
./src/modules/catalog/CatalogBox.d \
./src/modules/catalog/CatalogQuery.d \
./src/modules/catalog/CatalogSchema.d \
./src/modules/catalog/CatalogStream.d \
./src/modules/catalog/CatalogTable.d \
./src/modules/catalog/CatalogValidate.d \
./src/modules/catalog/CatalogView.d \
./src/modules/catalog/Diagram.d \
./src/modules/catalog/SchemaArray.d 


# Each subdirectory must supply rules for building sources it contributes
src/modules/catalog/%.o: ../src/modules/catalog/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


