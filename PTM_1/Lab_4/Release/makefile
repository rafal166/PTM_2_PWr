################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
lab1.4.lss \

FLASH_IMAGE += \
lab1.4.hex \

EEPROM_IMAGE += \
lab1.4.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: lab1.4.elf secondary-outputs

# Tool invocations
lab1.4.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,lab1.4.map -mmcu=atmega32 -o "lab1.4.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

lab1.4.lss: lab1.4.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S lab1.4.elf  >"lab1.4.lss"
	@echo 'Finished building: $@'
	@echo ' '

lab1.4.hex: lab1.4.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex lab1.4.elf  "lab1.4.hex"
	@echo 'Finished building: $@'
	@echo ' '

lab1.4.eep: lab1.4.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex lab1.4.elf  "lab1.4.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: lab1.4.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega32 lab1.4.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(ASM_DEPS)$(EEPROM_IMAGE)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) lab1.4.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
