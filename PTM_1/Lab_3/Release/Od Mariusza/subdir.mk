################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Od\ Mariusza/main.c 

C_DEPS += \
./Od\ Mariusza/main.d 

OBJS += \
./Od\ Mariusza/main.o 


# Each subdirectory must supply rules for building sources it contributes
Od\ Mariusza/main.o: ../Od\ Mariusza/main.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=1000000UL -MMD -MP -MF"Od Mariusza/main.d" -MT"Od\ Mariusza/main.d" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


