zip='\c\Program Files\7-Zip\7z.exe'

del stm32u5-tfm-package.zip

md scripts
copy /y Projects\B-U585I-IOT02A\Applications\TFM_Azure_IoT\AzureScripts\STM32_Python_Utility\CubeProgrammer\Bat\STM32U5_TrustZone_Disable.bat .\trust-zone-disable.bat
copy /y Projects\B-U585I-IOT02A\Applications\TFM_Azure_IoT\AzureScripts\STM32_Python_Utility\CubeProgrammer\Bat\regression.bat .\trust-zone-enable.bat

"C:\Program Files\7-Zip\7z.exe" a -tzip stm32u5-tfm-package.zip -spf2 ^
Projects\B-U585I-IOT02A\Applications\TFM_Azure_IoT\AzureScripts\STM32_Python_Utility\CubeProgrammer\Bat\TFM_UPDATE.bat ^
Projects\B-U585I-IOT02A\Applications\TFM_Azure_IoT\TFM_Appli\Binary\tfm_ns_app_init.bin ^
Projects\B-U585I-IOT02A\Applications\TFM_Azure_IoT\TFM_Appli\Binary\tfm_ns_data_init.bin ^
Projects\B-U585I-IOT02A\Applications\TFM_Azure_IoT\TFM_Appli\Binary\tfm_s_app_init.bin ^
Projects\B-U585I-IOT02A\Applications\TFM_Azure_IoT\TFM_Appli\Binary\tfm_s_data_init.bin ^
Projects\B-U585I-IOT02A\Applications\TFM_Azure_IoT\TFM_Loader\Binary\loader.bin ^
Projects\B-U585I-IOT02A\Applications\TFM_Azure_IoT\TFM_SBSFU_Boot\Binary\bootloader.bin ^
Projects\B-U585I-IOT02A\Applications\TFM_Azure_IoT\TFM_SBSFU_Boot\STM32CubeIDE\TFM_UPDATE.sh ^
trust-zone-enable.bat ^
trust-zone-disable.bat ^
tfm-update.bat

del /q trust-zone-enable.bat trust-zone-disable.bat
