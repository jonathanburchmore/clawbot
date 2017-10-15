################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
INO_SRCS += \
../SoftwareSerial/examples/TwoPortReceive/TwoPortReceive.ino 

OBJS += \
./SoftwareSerial/examples/TwoPortReceive/TwoPortReceive.o 

INO_DEPS += \
./SoftwareSerial/examples/TwoPortReceive/TwoPortReceive.d 


# Each subdirectory must supply rules for building sources it contributes
SoftwareSerial/examples/TwoPortReceive/%.o: ../SoftwareSerial/examples/TwoPortReceive/%.ino
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"C:\Users\burch\arduino-1.0\hardware\arduino\cores\arduino" -I"C:\Users\burch\arduino-1.0\hardware\arduino\variants\standard" -I"C:\Users\burch\Arduino\clawbot" -D__IN_ECLIPSE__=1 -DARDUINO=100 -Wall -Os -ffunction-sections -fdata-sections -fno-exceptions -g -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" -x c++ "$<"
	@echo 'Finished building: $<'
	@echo ' '


