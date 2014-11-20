################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../IMURead/imuread.cpp 

OBJS += \
./IMURead/imuread.o 

CPP_DEPS += \
./IMURead/imuread.d 


# Each subdirectory must supply rules for building sources it contributes
IMURead/%.o: ../IMURead/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/localdisk/simulation/blauret/personnal/RTIMULib/RTIMULib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


