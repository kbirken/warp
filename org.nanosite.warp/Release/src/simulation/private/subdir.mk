################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/simulation/private/CAPSScheduler.cpp \
../src/simulation/private/CMessage.cpp \
../src/simulation/private/CMessageQueue.cpp \
../src/simulation/private/CResourceSlotVector.cpp \
../src/simulation/private/CSimCore.cpp \
../src/simulation/private/CTimeMonitor.cpp \
../src/simulation/private/CToken.cpp \
../src/simulation/private/CTokenFactory.cpp 

OBJS += \
./src/simulation/private/CAPSScheduler.o \
./src/simulation/private/CMessage.o \
./src/simulation/private/CMessageQueue.o \
./src/simulation/private/CResourceSlotVector.o \
./src/simulation/private/CSimCore.o \
./src/simulation/private/CTimeMonitor.o \
./src/simulation/private/CToken.o \
./src/simulation/private/CTokenFactory.o 

CPP_DEPS += \
./src/simulation/private/CAPSScheduler.d \
./src/simulation/private/CMessage.d \
./src/simulation/private/CMessageQueue.d \
./src/simulation/private/CResourceSlotVector.d \
./src/simulation/private/CSimCore.d \
./src/simulation/private/CTimeMonitor.d \
./src/simulation/private/CToken.d \
./src/simulation/private/CTokenFactory.d 


# Each subdirectory must supply rules for building sources it contributes
src/simulation/private/%.o: ../src/simulation/private/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ -I"D:\pgm\warp\warp\src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


