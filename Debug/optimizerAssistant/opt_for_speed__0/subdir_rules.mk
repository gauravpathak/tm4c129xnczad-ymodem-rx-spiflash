################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
main.obj: ../main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --opt_for_speed=0 --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="D:/Tiva/TivaWare(129X)" --include_path="D:/Tiva/TivaWare(129X)/examples/boards/dk-tm4c129x" -g --define=ccs="ccs" --define=TARGET_IS_SNOWFLAKE_RA0 --define=PART_TM4C129XNCZAD --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

spi_flash.obj: ../spi_flash.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --opt_for_speed=0 --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="D:/Tiva/TivaWare(129X)" --include_path="D:/Tiva/TivaWare(129X)/examples/boards/dk-tm4c129x" -g --define=ccs="ccs" --define=TARGET_IS_SNOWFLAKE_RA0 --define=PART_TM4C129XNCZAD --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="spi_flash.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

tm4c129xnczad_startup_ccs.obj: ../tm4c129xnczad_startup_ccs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --opt_for_speed=0 --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="D:/Tiva/TivaWare(129X)" --include_path="D:/Tiva/TivaWare(129X)/examples/boards/dk-tm4c129x" -g --define=ccs="ccs" --define=TARGET_IS_SNOWFLAKE_RA0 --define=PART_TM4C129XNCZAD --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="tm4c129xnczad_startup_ccs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


