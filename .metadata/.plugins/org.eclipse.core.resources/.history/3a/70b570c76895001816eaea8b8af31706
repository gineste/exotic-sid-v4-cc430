################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv6/tools/compiler/msp430_15.12.3.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./Libraries/CC430/dma.obj" \
"./Libraries/CC430/hal_pmm.obj" \
"./Libraries/ES_Protocol/ES_Interfaces.obj" \
"./Libraries/ES_Protocol/ES_Protocol.obj" \
"./Libraries/RF/RF1A.obj" \
"./Libraries/RF/RfRegSettings.obj" \
"./Libraries/TPMS/tpms.obj" \
"./Libraries/TPMS/tpms_ak.obj" \
"./Libraries/TPMS/tpms_ask.obj" \
"./Libraries/TPMS/tpms_ldl.obj" \
"./Libraries/TPMS/tpms_mems.obj" \
"./Libraries/TPMS/tpms_sid.obj" \
"./Libraries/Tools/lib_ExoFifo.obj" \
"./Libraries/Tools/time_functions.obj" \
"./Projects/main.obj" \
"../Projects/lnk_cc430f5137.cmd" \
$(GEN_CMDS__FLAG) \
-llibmath.a \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include Libraries/CC430/subdir_vars.mk
-include Libraries/ES_Protocol/subdir_vars.mk
-include Libraries/RF/subdir_vars.mk
-include Libraries/TPMS/subdir_vars.mk
-include Libraries/Tools/subdir_vars.mk
-include Projects/subdir_vars.mk
-include Libraries/CC430/subdir_rules.mk
-include Libraries/ES_Protocol/subdir_rules.mk
-include Libraries/RF/subdir_rules.mk
-include Libraries/TPMS/subdir_rules.mk
-include Libraries/Tools/subdir_rules.mk
-include Projects/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
SIDV3_CC430.out \

EXE_OUTPUTS__QUOTED += \
"SIDV3_CC430.out" \

BIN_OUTPUTS += \
SIDV3_CC430.hex \

BIN_OUTPUTS__QUOTED += \
"SIDV3_CC430.hex" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "SIDV3_CC430.out" secondary-outputs

# Tool invocations
SIDV3_CC430.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: MSP430 Linker'
	"C:/ti/ccsv6/tools/compiler/msp430_15.12.3.LTS/bin/cl430" -vmspx --abi=eabi --data_model=large --code_model=large -Ooff --opt_for_speed=2 --use_hw_mpy=F5 --advice:power="all" -g --gcc --define=__CC430F5137__ --define=MHZ_433 --define=SPI_USE_IO_CS --define=DMA_USE_EXTERNAL_ISR --define=MSP430_USE_SMALL_MEMORY_MODEL --define=TPMS_USE_ENCODER --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU18 --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal --misra_required=error --misra_advisory=warning -z -m"SIDV3_CC430.map" --heap_size=40 --stack_size=150 --cinit_hold_wdt=on -i"C:/ti/ccsv6/ccs_base/msp430/include" -i"C:/ti/ccsv6/tools/compiler/msp430_15.12.3.LTS/lib" -i"C:/ti/ccsv6/tools/compiler/msp430_15.12.3.LTS/include" -i"C:/ti/ccsv6/ccs_base/msp430/lib/5xx_6xx_FRxx" --reread_libs --warn_sections --diag_wrap=off --display_error_number --xml_link_info="SIDV3_CC430_linkInfo.xml" --use_hw_mpy=F5 --rom_model -o "SIDV3_CC430.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

SIDV3_CC430.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: MSP430 Hex Utility'
	"C:/ti/ccsv6/tools/compiler/msp430_15.12.3.LTS/bin/hex430" --memwidth=8 --romwidth=8 --intel -o "SIDV3_CC430.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "Libraries\CC430\dma.d" "Libraries\CC430\hal_pmm.d" "Libraries\ES_Protocol\ES_Interfaces.d" "Libraries\ES_Protocol\ES_Protocol.d" "Libraries\RF\RF1A.d" "Libraries\RF\RfRegSettings.d" "Libraries\TPMS\tpms.d" "Libraries\TPMS\tpms_ak.d" "Libraries\TPMS\tpms_ask.d" "Libraries\TPMS\tpms_ldl.d" "Libraries\TPMS\tpms_mems.d" "Libraries\TPMS\tpms_sid.d" "Libraries\Tools\lib_ExoFifo.d" "Libraries\Tools\time_functions.d" "Projects\main.d" 
	-$(RM) "Libraries\CC430\dma.obj" "Libraries\CC430\hal_pmm.obj" "Libraries\ES_Protocol\ES_Interfaces.obj" "Libraries\ES_Protocol\ES_Protocol.obj" "Libraries\RF\RF1A.obj" "Libraries\RF\RfRegSettings.obj" "Libraries\TPMS\tpms.obj" "Libraries\TPMS\tpms_ak.obj" "Libraries\TPMS\tpms_ask.obj" "Libraries\TPMS\tpms_ldl.obj" "Libraries\TPMS\tpms_mems.obj" "Libraries\TPMS\tpms_sid.obj" "Libraries\Tools\lib_ExoFifo.obj" "Libraries\Tools\time_functions.obj" "Projects\main.obj" 
	-@echo 'Finished clean'
	-@echo ' '

secondary-outputs: $(BIN_OUTPUTS)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

