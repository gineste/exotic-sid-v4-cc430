################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
Libraries/CC430/dma.obj: ../Libraries/CC430/dma.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv6/tools/compiler/msp430_15.12.3.LTS/bin/cl430" -vmspx --abi=eabi --data_model=large --code_model=large -Ooff --opt_for_speed=2 --use_hw_mpy=F5 --include_path="C:/ti/ccsv6/ccs_base/msp430/include" --include_path="C:/_Projets_/SIDV4/SIDV3_CC430/Projects" --include_path="C:/_Projets_/SIDV4/SIDV3_CC430/Libraries" --include_path="C:/ti/ccsv6/tools/compiler/msp430_15.12.3.LTS/include" --advice:power="all" -g --gcc --define=__CC430F5137__ --define=MHZ_433 --define=SPI_USE_IO_CS --define=DMA_USE_EXTERNAL_ISR --define=MSP430_USE_SMALL_MEMORY_MODEL --define=TPMS_USE_ENCODER --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU18 --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal --misra_required=error --misra_advisory=warning --preproc_with_compile --preproc_dependency="Libraries/CC430/dma.d" --obj_directory="Libraries/CC430" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

Libraries/CC430/hal_pmm.obj: ../Libraries/CC430/hal_pmm.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv6/tools/compiler/msp430_15.12.3.LTS/bin/cl430" -vmspx --abi=eabi --data_model=large --code_model=large -Ooff --opt_for_speed=2 --use_hw_mpy=F5 --include_path="C:/ti/ccsv6/ccs_base/msp430/include" --include_path="C:/_Projets_/SIDV4/SIDV3_CC430/Projects" --include_path="C:/_Projets_/SIDV4/SIDV3_CC430/Libraries" --include_path="C:/ti/ccsv6/tools/compiler/msp430_15.12.3.LTS/include" --advice:power="all" -g --gcc --define=__CC430F5137__ --define=MHZ_433 --define=SPI_USE_IO_CS --define=DMA_USE_EXTERNAL_ISR --define=MSP430_USE_SMALL_MEMORY_MODEL --define=TPMS_USE_ENCODER --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU18 --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal --misra_required=error --misra_advisory=warning --preproc_with_compile --preproc_dependency="Libraries/CC430/hal_pmm.d" --obj_directory="Libraries/CC430" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


