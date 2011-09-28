################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../utility/stale/boxes/CatalogArray.cc \
../utility/stale/boxes/ChunkAssemblerQBox.cc \
../utility/stale/boxes/ConcatenateQBox.cc \
../utility/stale/boxes/ContrastCompositeQBox.cc \
../utility/stale/boxes/ContrastQBox.cc \
../utility/stale/boxes/DeltaAisQBox.cc \
../utility/stale/boxes/DeltaDetectAdjQBox.cc \
../utility/stale/boxes/DeltaDetectBatchQBox.cc \
../utility/stale/boxes/DeltaDetectQBox.cc \
../utility/stale/boxes/DeltaDetectRecurseQBox.cc \
../utility/stale/boxes/DeltaDetectSampleQBox.cc \
../utility/stale/boxes/EdgeDetectQBox.cc \
../utility/stale/boxes/ExperimentalJoinQBox.cc \
../utility/stale/boxes/FaceDetectAdjQBox.cc \
../utility/stale/boxes/FaceDetectBatchQBox.cc \
../utility/stale/boxes/FaceDetectChunkQBox.cc \
../utility/stale/boxes/FaceDetectQBox.cc \
../utility/stale/boxes/FaceDetectRecurseQBox.cc \
../utility/stale/boxes/FaceDetectSpatialQBox.cc \
../utility/stale/boxes/InstantiateArray.cc \
../utility/stale/boxes/JoinFitsQBox.cc \
../utility/stale/boxes/MapFrameCompositeQBox.cc \
../utility/stale/boxes/MapFrameQBox.cc \
../utility/stale/boxes/MergeSketchQBox.cc \
../utility/stale/boxes/MySQLQBox.cc \
../utility/stale/boxes/RegridQBox.cc \
../utility/stale/boxes/RenderDeltasQBox.cc \
../utility/stale/boxes/RenderQBox.cc \
../utility/stale/boxes/ResampleQBox.cc \
../utility/stale/boxes/StockQuoteConverterBox.cc 

OBJS += \
./utility/stale/boxes/CatalogArray.o \
./utility/stale/boxes/ChunkAssemblerQBox.o \
./utility/stale/boxes/ConcatenateQBox.o \
./utility/stale/boxes/ContrastCompositeQBox.o \
./utility/stale/boxes/ContrastQBox.o \
./utility/stale/boxes/DeltaAisQBox.o \
./utility/stale/boxes/DeltaDetectAdjQBox.o \
./utility/stale/boxes/DeltaDetectBatchQBox.o \
./utility/stale/boxes/DeltaDetectQBox.o \
./utility/stale/boxes/DeltaDetectRecurseQBox.o \
./utility/stale/boxes/DeltaDetectSampleQBox.o \
./utility/stale/boxes/EdgeDetectQBox.o \
./utility/stale/boxes/ExperimentalJoinQBox.o \
./utility/stale/boxes/FaceDetectAdjQBox.o \
./utility/stale/boxes/FaceDetectBatchQBox.o \
./utility/stale/boxes/FaceDetectChunkQBox.o \
./utility/stale/boxes/FaceDetectQBox.o \
./utility/stale/boxes/FaceDetectRecurseQBox.o \
./utility/stale/boxes/FaceDetectSpatialQBox.o \
./utility/stale/boxes/InstantiateArray.o \
./utility/stale/boxes/JoinFitsQBox.o \
./utility/stale/boxes/MapFrameCompositeQBox.o \
./utility/stale/boxes/MapFrameQBox.o \
./utility/stale/boxes/MergeSketchQBox.o \
./utility/stale/boxes/MySQLQBox.o \
./utility/stale/boxes/RegridQBox.o \
./utility/stale/boxes/RenderDeltasQBox.o \
./utility/stale/boxes/RenderQBox.o \
./utility/stale/boxes/ResampleQBox.o \
./utility/stale/boxes/StockQuoteConverterBox.o 

CC_DEPS += \
./utility/stale/boxes/CatalogArray.d \
./utility/stale/boxes/ChunkAssemblerQBox.d \
./utility/stale/boxes/ConcatenateQBox.d \
./utility/stale/boxes/ContrastCompositeQBox.d \
./utility/stale/boxes/ContrastQBox.d \
./utility/stale/boxes/DeltaAisQBox.d \
./utility/stale/boxes/DeltaDetectAdjQBox.d \
./utility/stale/boxes/DeltaDetectBatchQBox.d \
./utility/stale/boxes/DeltaDetectQBox.d \
./utility/stale/boxes/DeltaDetectRecurseQBox.d \
./utility/stale/boxes/DeltaDetectSampleQBox.d \
./utility/stale/boxes/EdgeDetectQBox.d \
./utility/stale/boxes/ExperimentalJoinQBox.d \
./utility/stale/boxes/FaceDetectAdjQBox.d \
./utility/stale/boxes/FaceDetectBatchQBox.d \
./utility/stale/boxes/FaceDetectChunkQBox.d \
./utility/stale/boxes/FaceDetectQBox.d \
./utility/stale/boxes/FaceDetectRecurseQBox.d \
./utility/stale/boxes/FaceDetectSpatialQBox.d \
./utility/stale/boxes/InstantiateArray.d \
./utility/stale/boxes/JoinFitsQBox.d \
./utility/stale/boxes/MapFrameCompositeQBox.d \
./utility/stale/boxes/MapFrameQBox.d \
./utility/stale/boxes/MergeSketchQBox.d \
./utility/stale/boxes/MySQLQBox.d \
./utility/stale/boxes/RegridQBox.d \
./utility/stale/boxes/RenderDeltasQBox.d \
./utility/stale/boxes/RenderQBox.d \
./utility/stale/boxes/ResampleQBox.d \
./utility/stale/boxes/StockQuoteConverterBox.d 


# Each subdirectory must supply rules for building sources it contributes
utility/stale/boxes/%.o: ../utility/stale/boxes/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


