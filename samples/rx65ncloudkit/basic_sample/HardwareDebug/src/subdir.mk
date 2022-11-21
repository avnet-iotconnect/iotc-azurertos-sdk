################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LINKER_SCRIPT += \
../src/linker_script.ld 

C_SRCS += \
../src/compat.c \
../src/demo_printf.c \
../src/hardware_setup.c \
../src/iotconnect_app.c \
../src/main.c \
../src/nx_driver_rx65n_cloud_kit.c 

LST += \
compat.lst \
demo_printf.lst \
hardware_setup.lst \
iotconnect_app.lst \
main.lst \
nx_driver_rx65n_cloud_kit.lst 

C_DEPS += \
./src/compat.d \
./src/demo_printf.d \
./src/hardware_setup.d \
./src/iotconnect_app.d \
./src/main.d \
./src/nx_driver_rx65n_cloud_kit.d 

OBJS += \
./src/compat.o \
./src/demo_printf.o \
./src/hardware_setup.o \
./src/iotconnect_app.o \
./src/main.o \
./src/nx_driver_rx65n_cloud_kit.o 

MAP += \
basic_sample.map 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	$(file > $@.in,-Og -ffunction-sections -fdata-sections -fdiagnostics-parseable-fixits -Wstack-usage=100 -g2 -mcpu=rx64m -misa=v2 -mlittle-endian-data -std=gnu99 -DNX_INCLUDE_USER_DEFINE_FILE -DNX_ENABLE_DHCP -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\general" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\iotc-azrtos-sdk\\azrtos-layer\\nx-http-client" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\iotc-azrtos-sdk\\azrtos-layer\\azrtos-ota\\include" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\iotc-azrtos-sdk\\azrtos-layer\\include" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\iotc-azrtos-sdk\\authentication\\include" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\iotc-azrtos-sdk\\authentication\\driver" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\iotc-azrtos-sdk\\include" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\iotc-azrtos-sdk\\iotc-c-lib\\include" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\addons\\mqtt" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\addons\\cloud" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\addons\\azure_iot\\azure-sdk-for-c\\sdk\\inc" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\addons\\azure_iot\\azure_iot_security_module\\inc" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\addons\\azure_iot\\azure_iot_security_module\\iot-security-module-core\\inc" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\addons\\azure_iot" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\nx_secure\\ports" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\nx_secure\\inc" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\addons\\sntp" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\crypto_libraries\\inc" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo_addons\\addons\\dns" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\filex\\common\\inc" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\filex\\ports\\generic\\inc" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo\\common\\inc" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\netxduo\\ports\\rxv2\\gnu\\inc" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\threadx\\common\\inc" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\threadx\\ports\\rxv2\\gnu\\inc" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_pincfg" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_bsp" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_config" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_byteq" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\Config_SCI5" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_wifi_sx_ulpgn\\src" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_wifi_sx_ulpgn" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_byteq\\src" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_sci_rx\\src" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_sci_rx" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_cmt_rx\\src" -I"C:\\Users\\rbateman\\Documents\\Projects\\iotconnect\\iotc-azurertos-sdk\\samples\\rx65ncloudkit\\basic_sample\\src\\smc_gen\\r_cmt_rx" -DNX_INCLUDE_USER_DEFINE_FILE -DNX_ENABLE_DHCP -Wa,-adlnh="$(basename $(notdir $<)).lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" "$<" -c -o "$@")
	@echo $< && rx-elf-gcc @"$@.in"

