#!/bin/bash
zip='/c/Program Files/7-Zip/7z.exe'

if [[ -n "$1" ]]; then
  zip=$1
fi

rm -f stm32u5_tfm_package.zip

"${zip}" a -tzip stm32u5_tfm_package.zip  -- \
./Projects/B-U585I-IOT02A/Applications/TFM_Azure_IoT/TFM_Appli/Binary/tfm_ns_app_init.bin \
./Projects/B-U585I-IOT02A/Applications/TFM_Azure_IoT/TFM_Appli/Binary/tfm_ns_data_init.bin \
./Projects/B-U585I-IOT02A/Applications/TFM_Azure_IoT/TFM_Appli/Binary/tfm_s_app_init.bin \
./Projects/B-U585I-IOT02A/Applications/TFM_Azure_IoT/TFM_Appli/Binary/tfm_s_data_init.bin \
./Projects/B-U585I-IOT02A/Applications/TFM_Azure_IoT/TFM_Loader/Binary/loader.bin \
./Projects/B-U585I-IOT02A/Applications/TFM_Azure_IoT/TFM_SBSFU_Boot/Binary/bootloader.bin \
./Projects/B-U585I-IOT02A/Applications/TFM_Azure_IoT/TFM_SBSFU_Boot/STM32CubeIDE/TFM_UPDATE.sh \
./Projects/B-U585I-IOT02A/Applications/TFM_Azure_IoT/TFM_SBSFU_Boot/STM32CubeIDE/regression.sh \

