################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/model/private/CBehaviour.cpp \
../src/model/private/CFunctionBlock.cpp \
../src/model/private/CPool.cpp \
../src/model/private/CPoolVector.cpp \
../src/model/private/CResource.cpp \
../src/model/private/CSimModel.cpp \
../src/model/private/CStep.cpp 

OBJS += \
./src/model/private/CBehaviour.o \
./src/model/private/CFunctionBlock.o \
./src/model/private/CPool.o \
./src/model/private/CPoolVector.o \
./src/model/private/CResource.o \
./src/model/private/CSimModel.o \
./src/model/private/CStep.o 

CPP_DEPS += \
./src/model/private/CBehaviour.d \
./src/model/private/CFunctionBlock.d \
./src/model/private/CPool.d \
./src/model/private/CPoolVector.d \
./src/model/private/CResource.d \
./src/model/private/CSimModel.d \
./src/model/private/CStep.d 


# Each subdirectory must supply rules for building sources it contributes
src/model/private/%.o: ../src/model/private/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


