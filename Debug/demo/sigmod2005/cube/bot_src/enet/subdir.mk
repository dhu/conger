################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../demo/sigmod2005/cube/bot_src/enet/host.c \
../demo/sigmod2005/cube/bot_src/enet/list.c \
../demo/sigmod2005/cube/bot_src/enet/memory.c \
../demo/sigmod2005/cube/bot_src/enet/packet.c \
../demo/sigmod2005/cube/bot_src/enet/peer.c \
../demo/sigmod2005/cube/bot_src/enet/protocol.c \
../demo/sigmod2005/cube/bot_src/enet/unix.c \
../demo/sigmod2005/cube/bot_src/enet/win32.c 

OBJS += \
./demo/sigmod2005/cube/bot_src/enet/host.o \
./demo/sigmod2005/cube/bot_src/enet/list.o \
./demo/sigmod2005/cube/bot_src/enet/memory.o \
./demo/sigmod2005/cube/bot_src/enet/packet.o \
./demo/sigmod2005/cube/bot_src/enet/peer.o \
./demo/sigmod2005/cube/bot_src/enet/protocol.o \
./demo/sigmod2005/cube/bot_src/enet/unix.o \
./demo/sigmod2005/cube/bot_src/enet/win32.o 

C_DEPS += \
./demo/sigmod2005/cube/bot_src/enet/host.d \
./demo/sigmod2005/cube/bot_src/enet/list.d \
./demo/sigmod2005/cube/bot_src/enet/memory.d \
./demo/sigmod2005/cube/bot_src/enet/packet.d \
./demo/sigmod2005/cube/bot_src/enet/peer.d \
./demo/sigmod2005/cube/bot_src/enet/protocol.d \
./demo/sigmod2005/cube/bot_src/enet/unix.d \
./demo/sigmod2005/cube/bot_src/enet/win32.d 


# Each subdirectory must supply rules for building sources it contributes
demo/sigmod2005/cube/bot_src/enet/%.o: ../demo/sigmod2005/cube/bot_src/enet/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


