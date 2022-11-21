################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

-include makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/smc_gen/r_wifi_sx_ulpgn/src/subdir.mk
-include src/smc_gen/r_sci_rx/src/targets/rx65n/subdir.mk
-include src/smc_gen/r_sci_rx/src/subdir.mk
-include src/smc_gen/r_pincfg/subdir.mk
-include src/smc_gen/r_cmt_rx/src/subdir.mk
-include src/smc_gen/r_byteq/src/subdir.mk
-include src/smc_gen/r_bsp/mcu/rx65n/subdir.mk
-include src/smc_gen/r_bsp/mcu/all/subdir.mk
-include src/smc_gen/r_bsp/board/generic_rx65n/subdir.mk
-include src/smc_gen/general/subdir.mk
-include src/smc_gen/Config_SCI5/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(JMP_DEPS)),)
-include $(JMP_DEPS)
endif
ifneq ($(strip $(FSY_DEPS)),)
-include $(FSY_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(SRC_UPPER_DEPS)),)
-include $(SRC_UPPER_DEPS)
endif
ifneq ($(strip $(P_UPPER_DEPS)),)
-include $(P_UPPER_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(PP_UPPER_DEPS)),)
-include $(PP_UPPER_DEPS)
endif
ifneq ($(strip $(P_DEPS)),)
-include $(P_DEPS)
endif
ifneq ($(strip $(FSY_UPPER_DEPS)),)
-include $(FSY_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(SRC_DEPS)),)
-include $(SRC_DEPS)
endif
ifneq ($(strip $(JMP_UPPER_DEPS)),)
-include $(JMP_UPPER_DEPS)
endif
ifneq ($(strip $(PP_DEPS)),)
-include $(PP_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(SX_DEPS)),)
-include $(SX_DEPS)
endif
ifneq ($(strip $(SX_UPPER_DEPS)),)
-include $(SX_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LIBRARY_GENERATOR_OUTPUTTYPE_OUTPUTS += \

OBJCOPY += \
basic_sample.mot \

PRINT_SIZE += \
basic_sample.siz \


# All Target
all: basic_sample.elf secondary-outputs

# Tool invocations
basic_sample.elf: $(OBJS) $(USER_OBJS) $(LIBRARY_GENERATOR_OUTPUTTYPE_OUTPUTS) $(LINKER_SCRIPT)
	$(file > basic_sample.elf.in,-Og -ffunction-sections -fdata-sections -fdiagnostics-parseable-fixits -Wstack-usage=100 -g2 -mcpu=rx64m -misa=v2 -mlittle-endian-data -o "basic_sample.elf" $(OBJS) $(USER_OBJS) $(LIBS) -T "C:/Users/rbateman/Documents/Projects/iotconnect/iotc-azurertos-sdk/samples/rx65ncloudkit/basic_sample/src/linker_script.ld" -L"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\filex\\Debug" -L"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\iotc-azrtos-sdk\\Debug" -L"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo\\Debug" -L"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\Debug" -L"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\threadx\\Debug" -Wl,--start-group -lm -lc -lgcc -lfilex -liotc-azrtos-sdk -lnetxduo -lnetxduo_addons -lthreadx -Wl,--end-group -nostartfiles -Wl,-e_PowerON_Reset -Wl,-M=basic_sample.map)
	@echo Building target: $@ && rx-elf-gcc @"basic_sample.elf.in"
: 
	rx-elf-libgen  --select-lib=newlib --compiler-options=-Og,-ffunction-sections,-fdata-sections,-mcpu=rx64m,-misa=v2,-mlittle-endian-data --assembler-options=-mlittle-endian-data -o
basic_sample.mot: basic_sample.elf
	rx-elf-objcopy "basic_sample.elf" -O srec -I elf32-rx-be-ns  "basic_sample.mot"
basic_sample.siz: basic_sample.elf
	rx-elf-size --format=berkeley "basic_sample.elf"
# Other Targets
clean:
	-$(file > clean_file.tmp,  $(PRINT_SIZE)  $(JMP_DEPS)  $(FSY_DEPS)  $(C_UPPER_DEPS)  $(SRC_UPPER_DEPS)  $(P_UPPER_DEPS)  $(LST)  $(S_DEPS)  $(PP_UPPER_DEPS)  $(P_DEPS)  $(FSY_UPPER_DEPS)  $(C_DEPS)  $(SRC_DEPS)  $(OBJCOPY)  $(JMP_UPPER_DEPS)  $(PP_DEPS)  $(ASM_UPPER_DEPS)  $(SX_DEPS)  $(OBJS)  $(SX_UPPER_DEPS)  $(ASM_DEPS)  $(LIBRARY_GENERATOR_OUTPUTTYPE_OUTPUTS)  $(MAP)  $(S_UPPER_DEPS) )
	-@ xargs -P 2 -s 32000 -t -a clean_file.tmp $(RM)
	-@$(RM) clean_file.tmp
	-$(RM)  basic_sample.elf
	-@echo ' '

secondary-outputs: $(OBJCOPY) $(PRINT_SIZE)

.PHONY: all clean dependents 

-include ../makefile.targets
