################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'C2000 Compiler - building file: "$<"'
	"C:/ti/ccs2050/ccs/tools/compiler/ti-cgt-c2000_25.11.0.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 -O2 --include_path="C:/Users/Danre/Documents/CCS/Maglev" --include_path="C:/ti/ccs2050/ccs/tools/compiler/ti-cgt-c2000_25.11.0.LTS/include" --include_path="C:/ti/c2000/C2000Ware_4_03_00_00/device_support/f2806x/common/include" --include_path="C:/ti/c2000/C2000Ware_4_03_00_00/device_support/f2806x/headers/include" --define=_INLINE --define=_RELEASE --undefine=_DEBUG --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --ramfunc=off --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'C2000 Compiler - building file: "$<"'
	"C:/ti/ccs2050/ccs/tools/compiler/ti-cgt-c2000_25.11.0.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 -O2 --include_path="C:/Users/Danre/Documents/CCS/Maglev" --include_path="C:/ti/ccs2050/ccs/tools/compiler/ti-cgt-c2000_25.11.0.LTS/include" --include_path="C:/ti/c2000/C2000Ware_4_03_00_00/device_support/f2806x/common/include" --include_path="C:/ti/c2000/C2000Ware_4_03_00_00/device_support/f2806x/headers/include" --define=_INLINE --define=_RELEASE --undefine=_DEBUG --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --ramfunc=off --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


