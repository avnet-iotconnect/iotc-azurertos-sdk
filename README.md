## About
This repo contains the IoTConnect C SDK and samples for AzureRTOS, 
for Avnet MaaxBoard RT, STM32L4 IoT Discovery Kit, NXP MTIMXRT1060 EVK, Microchip SAME54 Xplained Pro,
Microchip WFI32-IoT (EV36W50A), and Renesas RX65N Cloud Kit.

Various Click Board sensors are also supported by the WFI32-IoT project. See the [WFI32-IoT Quickstart Demo Guide](samples/wfi32iot/QUICKSTART_DEMO.md).

To get started quickly, see the [IoTConnect AzureRTOS SDK STM32L4](https://www.youtube.com/watch?v=kkR9r2D4zBQ) demo video on YouTube.

If contributing to this project, follow the [contributing guidelines](CONTRIBUTING.md)

## Build Instructions

* Download the project files from Releases pages in this repo.
* Alternatively, you can directly clone this repo. After cloning, execute [setup-project.sh](scripts/setup-project.sh]). 
On Windows, you will need bash installed with MSYS2 or similar. Alternatively, install a Linux distribution through the Windows Store 
and run it via. Windows Subsystem for Linux.   
* For STM32L4:
  * Download and install STM32CubeIDE 
  * File->Open Projects form File System, navigate to one of the samples, and select all projects in the list.
  * This board contains a sensors-demo in addition to the basic-sample. 
    The basic-sample project contains a simple example how to send data to IoTConnect.
    The sensor-demo contains integration with sensors from a very limited port 
    of the STM32CubeExpansion_Cloud_AZURE_V1.2.1 BSP.
  * When running/debugging choose one of the demos form the project list first.
Keep subdirectories selected, but unselect the actual stm32l4 sample directory
* For Avnet MaaxBoardRT and NXP MIMXRT1060 EVK: 
  * Note that for the MaaxBoardRT, the WiFi and gigabit Ethernet are not supported.
  * Download and install MCUXpresso IDE
  * File->Open Projects form File System, navigate to one of the samples, and select all projects in the list. 
  * Make sure to switch to IDE view (top right IDE icon).
  * It is also recommended to close the "Globals" window, next to the "Outline" in the top right dock.
It may be causing hangs during debugging and perspective switching.
  * Select the bottom-middle dock "Installed SDKs" tab and install the MCUExpresso SDK for your board. 
Keep subdirectories selected, but unselect the actual mimxrt1060 sample directory
* For SAM E54 Xplained Pro:
  * Download and install MPLAB X IDE 5.45. IDE 5.40 has a known issue right now, so please use 5.35, or 5.45 and newer.
  * Download and install the MPLAB XC32/32++ Compiler 2.4.0 or later.
  (Buy the subscription for XC32 pro compiler if needs size optimization. OTA is dual bank updates so make sure that the program size is below half of the flash size.)
  * In MPLab, File > Open Project, navigate to the samples/same54xpro directory, select basic-sample, 
  check the "Open Required Projects" checkbox, and click the "Open Project" button.   
  * Plug in your board AFTER opening the project, so that MPLAB detects it
from the extracted zip file
* For WFI32-IoT board:
  * Download and install MPLAB X IDE 6.0 or newer.
  * Download and install the MPLAB XC32/32++ Compiler 4.1.0 or later.
  (Buy the subscription for XC32 pro compiler if needs OTA feature because of the size optimization for the dual bank OTA)
  * The project also supports the sensors integration with VAV Press Click Board and Ultra-Low-Press Click Board. 
  You can get  more hardware related setup info [here](https://github.com/MicrochipTech/AzureDemo_WFI32E01/blob/v0.9.1/Clicks.md).
  * With MPLAX X IDE, Open the iotconnect-demo.X project from the [samples/wfi32iot] directory.
  * Run or Debug the project.
  * Once the board comes up, a new Mass Storage Device (MSD) will be registered via the USB interface and the 
  PC should detect a new disk drive.
  * If the contents of CLOUD.CFG do not have values like CPID and ENV, delete the file and restart the board. 
  Restarting will populate the defaults.
  * In CLOUD.CFG JSON document, fill in your CPID and ENV values.
  You should generally omit specifying Device ID (DUID) in the config file as the default name will be based on the serial of the 
  ATECC608 chip that's built into the module. The default name will be in format snSERIAL, where serial is a 16-character hex string like sn12ABCD...
  The PEM files on the MSD will be named by the device's serial.
  If using SymmetricKey auth type, you can enter your Symmetric key in the file, otherwise you should leave it blank.
  * Edit WIFI.CFG per Wi-Fi network settings. The file should look like one of the following examples:
    - Open Unsecured Network (no password protection)
        ```bash
        CMD:SEND_UART=wifi MY_SSID,,1
        ```
    - Wi-Fi Protected Access 2 (WPA2)
        ```bash
        CMD:SEND_UART=wifi MY_SSID,MY_PSWD,2
        ```
    - Wired Equivalent Privacy (WEP)
        ```bash
        CMD:SEND_UART=wifi MY_SSID,MY_PSWD,3
        ```
    - Wi-Fi Protected Access 3 (WPA3)
        ```bash
        CMD:SEND_UART=wifi MY_SSID,MY_PSWD,4
        ```
  * New values will take effect once the board is restarted. 
  * **IMPORTANT**: Ensure that you eject the drive first and then restart the board after making any changes to the files. 
  This is so that the PC's OS can flush any cached file changes.
  * If using SelfSigned authentication with the built-in AETCC608 secure element (this authentication type is the most secure and recommended),
  you should ensure that SYMMETRIC_KEY in CLOUD.CFG is left blank and use the device certificate's fingerprint in the snSERIAL_device.pem file on the MSD: 
* For Renesas RX65N Cloud Kit:
  * Download and install Renesas e2 Studio 2022-10 or later
  * Download and install GCC for Renesas RX v8.3 or later, this can be done as part of the e2 studio install or after
  * Note the sample project is currently only supports the old (green) Cloud Kit with the Wi-Fi module. The "Blue" board
    with Ethernet and Cellular is not supported yet.
  * In e2 Studio, open a workspace and click "Import projects into workspace". Then select the samples/rx65ncloudkit directory,
  and import the projects.
  * To run/debug the application you will need to generate your own debug .launch file, or copy one from an existing application.

* Modify samples/your_board/basic-sample/include/app_config.h (not needed for WFI32-IoT) per your IoTConnect device and account info.
* Build and run or debug the project on your board.

## Creating Self-Signed x509 Device Certificates
* Download and follow instructions at 
iotc-c-lib's [ecc-certs section](https://github.com/avnet-iotconnect/iotc-c-lib/tree/master/tools/ecc-certs)
to upload and validate your CA certificate in IoTConnect. Set up your device and template in IoTConnect.
* If your project Azure RTOS baseline does not support ECC certificates, replace this line in `function device`:

```shell script
openssl ecparam -name secp256k1 -genkey -noout -out "${CPID}/${name}-key.pem"
```

whith this:

```shell script
# for mimxrt1060, the 2048 key size makes processing TLS handshake too slow to complete within 60 seconds
# and that will cause the server to disconnect us
openssl genrsa -out "${CPID}/${name}-key.pem" 1024
# for all others, 2048 can be used to increase security, but 1024 will make the connection process faster:
openssl genrsa -out "${CPID}/${name}-key.pem" 2048
```
* Generate the device certificates per iotc-c-lib's [ecc-certs section](https://github.com/avnet-iotconnect/iotc-c-lib/tree/master/tools/ecc-certs) instructions
* Generate .der format certificate and C snippets using the following example using your CPID and device ID in place: 

```shell script
cd <your_cpid>
$device=<duid> # IoTConnect device unique id
openssl rsa -inform PEM -outform DER -in $device-key.pem -out $device-key.der
openssl x509 -outform DER -inform PEM -in $device-crt.pem -out $device-crt.der
xxd -i $device-crt.der > $device-crt.c
xxd -i $device-key.der > $device-key.c
```
* The generated *.c files will have code snippets that you can use to assign to IOTCONNECT_CONFIG
following the example in iotconnect_app.c

## License
Use of MICROSOFT AZURE RTOS software is restricted under the corresponding license for that software at the [azrtos-licenses](azrtos-licenses/) directory.

This repo's derivative work from Azure RTOS distribution also falls under the same MICROSOFT AZURE RTOS license.

# Notes and Known issues
* The WFI32-IoT project will cause USB devices to change serial numbers randomly in between device restarts. 
This can affect VirtualBox (and perhaps other virtualization platforms) filters and potentially COMM port ordering. 
To work around this issue in VirtualBox you can remove the serial to a USB device filter.
* The WFI32-IoT project cannot print long lines in the console past a certain length. This can cause data to be lost and missing newlines to "jumble" the text.
* The WFI32-IoT project does not have a lot of free RAM. You may need to hand-pick components when integrating your own project.
* When using  2048 key size mimxrt1060 is too slow to complete processing the TLS handshake within 60 seconds. 
That will cause the server to disconnect.
* Ofast Optimization should not be enabled in the main project because of the
AzureRTOS issue [#14](https://github.com/azure-rtos/samples/issues/14). Os (size) optimization has been tested in the the project.
* It appears that ES-WIFI firmware is unable to connect to IP addresses ending with 0. 
The reported AT command error is "Invalid Address". Needs further investigation...
