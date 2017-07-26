################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/warp.cpp 

OBJS += \
./src/warp.o 

CPP_DEPS += \
./src/warp.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I"../src" -I"../src-gen" -I"/Library/Java/JavaVirtualMachines/jdk1.7.0_09.jdk/Contents/Home/include" -I"/Library/Java/JavaVirtualMachines/jdk1.7.0_09.jdk/Contents/Home/include/darwin" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


