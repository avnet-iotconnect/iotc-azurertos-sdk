################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../src/smc_gen/r_bsp/mcu/all/reset_program.S 

C_SRCS += \
../src/smc_gen/r_bsp/mcu/all/dbsct.c \
../src/smc_gen/r_bsp/mcu/all/lowlvl.c \
../src/smc_gen/r_bsp/mcu/all/lowsrc.c \
../src/smc_gen/r_bsp/mcu/all/mcu_locks.c \
../src/smc_gen/r_bsp/mcu/all/r_bsp_common.c \
../src/smc_gen/r_bsp/mcu/all/r_bsp_cpu.c \
../src/smc_gen/r_bsp/mcu/all/r_bsp_interrupts.c \
../src/smc_gen/r_bsp/mcu/all/r_bsp_locking.c \
../src/smc_gen/r_bsp/mcu/all/r_bsp_mcu_startup.c \
../src/smc_gen/r_bsp/mcu/all/r_bsp_software_interrupt.c \
../src/smc_gen/r_bsp/mcu/all/r_rx_intrinsic_functions.c \
../src/smc_gen/r_bsp/mcu/all/resetprg.c \
../src/smc_gen/r_bsp/mcu/all/sbrk.c 

LST += \
dbsct.lst \
lowlvl.lst \
lowsrc.lst \
mcu_locks.lst \
r_bsp_common.lst \
r_bsp_cpu.lst \
r_bsp_interrupts.lst \
r_bsp_locking.lst \
r_bsp_mcu_startup.lst \
r_bsp_software_interrupt.lst \
r_rx_intrinsic_functions.lst \
reset_program.lst \
resetprg.lst \
sbrk.lst 

C_DEPS += \
./src/smc_gen/r_bsp/mcu/all/dbsct.d \
./src/smc_gen/r_bsp/mcu/all/lowlvl.d \
./src/smc_gen/r_bsp/mcu/all/lowsrc.d \
./src/smc_gen/r_bsp/mcu/all/mcu_locks.d \
./src/smc_gen/r_bsp/mcu/all/r_bsp_common.d \
./src/smc_gen/r_bsp/mcu/all/r_bsp_cpu.d \
./src/smc_gen/r_bsp/mcu/all/r_bsp_interrupts.d \
./src/smc_gen/r_bsp/mcu/all/r_bsp_locking.d \
./src/smc_gen/r_bsp/mcu/all/r_bsp_mcu_startup.d \
./src/smc_gen/r_bsp/mcu/all/r_bsp_software_interrupt.d \
./src/smc_gen/r_bsp/mcu/all/r_rx_intrinsic_functions.d \
./src/smc_gen/r_bsp/mcu/all/resetprg.d \
./src/smc_gen/r_bsp/mcu/all/sbrk.d 

OBJS += \
./src/smc_gen/r_bsp/mcu/all/dbsct.o \
./src/smc_gen/r_bsp/mcu/all/lowlvl.o \
./src/smc_gen/r_bsp/mcu/all/lowsrc.o \
./src/smc_gen/r_bsp/mcu/all/mcu_locks.o \
./src/smc_gen/r_bsp/mcu/all/r_bsp_common.o \
./src/smc_gen/r_bsp/mcu/all/r_bsp_cpu.o \
./src/smc_gen/r_bsp/mcu/all/r_bsp_interrupts.o \
./src/smc_gen/r_bsp/mcu/all/r_bsp_locking.o \
./src/smc_gen/r_bsp/mcu/all/r_bsp_mcu_startup.o \
./src/smc_gen/r_bsp/mcu/all/r_bsp_software_interrupt.o \
./src/smc_gen/r_bsp/mcu/all/r_rx_intrinsic_functions.o \
./src/smc_gen/r_bsp/mcu/all/reset_program.o \
./src/smc_gen/r_bsp/mcu/all/resetprg.o \
./src/smc_gen/r_bsp/mcu/all/sbrk.o 

MAP += \
basic_sample.map 

S_UPPER_DEPS += \
./src/smc_gen/r_bsp/mcu/all/reset_program.d 


# Each subdirectory must supply rules for building sources it contributes
src/smc_gen/r_bsp/mcu/all/%.o: ../src/smc_gen/r_bsp/mcu/all/%.c
	@echo 'Building file: $<'
	$(file > $@.in,-Og -ffunction-sections -fdata-sections -fdiagnostics-parseable-fixits -Wstack-usage=100 -g2 -mcpu=rx64m -misa=v2 -mlittle-endian-data -std=gnu99 -DNX_INCLUDE_USER_DEFINE_FILE -DNX_ENABLE_DHCP -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\general" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\iotc-azrtos-sdk\\azrtos-layer\\nx-http-client" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\iotc-azrtos-sdk\\azrtos-layer\\azrtos-ota\\include" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\iotc-azrtos-sdk\\azrtos-layer\\include" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\iotc-azrtos-sdk\\authentication\\include" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\iotc-azrtos-sdk\\authentication\\driver" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\iotc-azrtos-sdk\\include" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\iotc-azrtos-sdk\\iotc-c-lib\\include" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\addons\\mqtt" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\addons\\cloud" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\addons\\azure_iot\\azure-sdk-for-c\\sdk\\inc" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\addons\\azure_iot\\azure_iot_security_module\\inc" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\addons\\azure_iot\\azure_iot_security_module\\iot-security-module-core\\inc" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\addons\\azure_iot" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\nx_secure\\ports" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\nx_secure\\inc" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\addons\\sntp" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\crypto_libraries\\inc" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\addons\\dns" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\filex\\common\\inc" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\filex\\ports\\generic\\inc" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo\\common\\inc" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo\\ports\\rxv2\\gnu\\inc" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\threadx\\common\\inc" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\threadx\\ports\\rxv2\\gnu\\inc" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_pincfg" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_bsp" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_config" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_byteq" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\Config_SCI5" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_wifi_sx_ulpgn\\src" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_wifi_sx_ulpgn" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_byteq\\src" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_sci_rx\\src" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_sci_rx" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_cmt_rx\\src" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_cmt_rx" -DNX_INCLUDE_USER_DEFINE_FILE -DNX_ENABLE_DHCP -Wa,-adlnh="$(basename $(notdir $<)).lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" "$<" -c -o "$@")
	@echo $< && rx-elf-gcc @"$@.in"
src/smc_gen/r_bsp/mcu/all/%.o: ../src/smc_gen/r_bsp/mcu/all/%.S
	@echo 'Building file: $<'
	$(file > $@.in,-Og -ffunction-sections -fdata-sections -fdiagnostics-parseable-fixits -Wstack-usage=100 -g2 -mcpu=rx64m -misa=v2 -mlittle-endian-data -x assembler-with-cpp -Wa,--gdwarf2 -I"C:/Users/rbateman/Documents/Projects/iotconnect/iotc-azurertos-sdk/samples/rx65ncloudkit/basic_sample/src" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\general" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_pincfg" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_bsp" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_config" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_byteq" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\Config_SCI5" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_wifi_sx_ulpgn\\src" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_wifi_sx_ulpgn" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_byteq\\src" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_sci_rx\\src" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_sci_rx" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_cmt_rx\\src" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_cmt_rx" -Wa,-adlhn="$(basename $(notdir $<)).lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c "$<" -o "$@")
	@echo $< && rx-elf-gcc @"$@.in"

