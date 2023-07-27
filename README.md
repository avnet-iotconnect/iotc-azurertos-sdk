## About

This repository contains the IoTConnect C SDK and samples for AzureRTOS running on the following devices:
* Avnet MaaxBoard RT
* NXP MTIMXRT1060 EVK
* STM32L4+ IoT Discovery Kit (B-L4S5I-IOT01A)
* Microchip SAME54 Xplained Pro
* Microchip WFI32-IoT (EV36W50A)
* Renesas CK-RX65N board
* Renesas RX65N Cloud Kit

For STM32U5 IoT Discovery Kit (B-U585I-IOT02A) support refer to the dedicated repository at [iotc-azurertos-stm32-u5](https://github.com/avnet-iotconnect/iotc-azurertos-stm32-u5)

To get started quickly, see the [IoTConnect AzureRTOS SDK STM32L4](https://www.youtube.com/watch?v=kkR9r2D4zBQ) demo video on YouTube for a walkthrough.

If contributing to this project, please follow the [contributing guidelines](CONTRIBUTING.md)

## Build Instructions

* If using the STM32U5, you should follow the initial [QUICKSTART.md](samples/stm32u5/QUICKSTART.md) 
located in the sample directory. Note that software X509 mutual authentication is also supported with the board but the 
steps are described in this document.
* Obtain the software corresponding to your board by using one of the following methods:
  * Option 1) Download and extract the project package for the target board from the [Releases](https://github.com/avnet-iotconnect/iotc-azurertos-sdk/releases) page.
  * Option 2) Clone this repository. After cloning, execute [setup-project.sh](scripts/setup-project.sh]) to setup the environment.
* On Windows, a bash interpreter is required (such as MSYS2). Alternatively, a Linux distribution can be installed through the Windows Store and can be run via Windows Subsystem for Linux.
* For STM32L4:
  * See the documentation in the [stm32l4](https://github.com/avnet-iotconnect/iotc-azurertos-sdk/tree/main/samples/stm32l4) directory.
* For Avnet MaaxBoardRT and NXP MIMXRT1060 EVK: 
  * See the documentation in the [maaxboardrt](https://github.com/avnet-iotconnect/iotc-azurertos-sdk/tree/main/samples/maaxboardrt) directory.
* For SAM E54 Xplained Pro:
    * See the documentation in the [same54xpro](https://github.com/avnet-iotconnect/iotc-azurertos-sdk/tree/main/samples/same54xpro) directory.
* For WFI32-IoT board:
  * See the documentation in the [wfi32iot](https://github.com/avnet-iotconnect/iotc-azurertos-sdk/tree/main/samples/wfi32iot) directory.
* For Renesas CK-RX65N board (The new blue PCB board cloudkit with Ethernet):
  * See the documentation in the [ck-rx65n](https://github.com/avnet-iotconnect/iotc-azurertos-sdk/tree/main/samples/ck-rx65n) directory.
* For Renesas RX65N Cloud Kit (the old green PCB Cloud Kit with the Wi-Fi module):
  * See the documentation in the [rx65ncloudkit](https://github.com/avnet-iotconnect/iotc-azurertos-sdk/tree/main/samples/rx65ncloudkit) directory.

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
