################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
E:\Dokumente\Arduino\libraries\Metro\Metro.cpp 

LINK_OBJ += \
.\libraries\Metro\Metro.cpp.o 

CPP_DEPS += \
.\libraries\Metro\Metro.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
libraries\Metro\Metro.cpp.o: E:\Dokumente\Arduino\libraries\Metro\Metro.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"E:\Downloads\sloeber\/arduinoPlugin/packages/arduino/tools/avr-gcc/4.9.2-atmel3.5.4-arduino2/bin/avr-g++" -c -g -Os -Wall -Wextra -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -flto -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10802 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR   -I"E:\Downloads\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.20\cores\arduino" -I"E:\Downloads\sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.6.20\variants\standard" -I"E:\Dokumente\Arduino\libraries\Metro" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '


