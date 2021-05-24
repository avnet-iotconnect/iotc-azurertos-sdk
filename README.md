## About
This repo contains the IoTConnect C SDK and samples for AzureRTOS, 
for STM32L4 IoT Discovery Kit and NXP MTIMXRT1060 EVK.

## Build Instructions

* Download the project files from Github Actions or Releases pages in this repo.
* Download STM32CubeIDE or MCUXpresso IDE
* File->Open Projects form File System, navigate to one of the samples, and select all projects in the list. 
Keep subdirectories selected, but unselect the actual directory (stm32l4/mimxrt1060)
* For MCUXpresso IDE:
  * Make sure to switch to IDE view (top right IDE icon).
  * Select the bottom-middle dock "Installed SDKs" tab and install the MCUExpresso SDK for your board. 
* Modify samples/<your_borard>/basic-sample/include/app_config.h per your IoTConnect deivce and account info.
* Build and run the project on your board.

## Creating Self-Signed x509 Device Certificates
As the AzureRTOS does not yet support ECC certificates yet, only RSA device certificates are supported. 

* Download and follow instructions at 
iotc-c-lib's [ecc-certs section](https://github.com/avnet-iotconnect/iotc-c-lib/tree/master/tools/ecc-certs)
to upload and validate your CA certificate in IoTConnect. Set up your device and template in IoTConnect.
* Replace this line in `function device`:

```shell script
openssl ecparam -name secp256k1 -genkey -noout -out "${CPID}/${name}-key.pem"
```

whith this:

```shell script
openssl genrsa -out "${CPID}/${name}-key.pem" 2048
```
* Generate the device certificates per iotc-c-lib's [ecc-certs section](https://github.com/avnet-iotconnect/iotc-c-lib/tree/master/tools/ecc-certs) instructions
* Generate .der format certificate and C snippets using the following example using your CPID and device ID in place: 

```shell script
cd <your_cpid>
$device=<duid> # IoTConnect device unique id
openssl rsa -inform PEM -outform DER -in $device-key.pem -out $device-key.pem.der
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

* Ofast Optimization should not be enabled in the main project because of the
AzureRTOS issue [#14](https://github.com/azure-rtos/samples/issues/14). Os (size) optimization has been tested in the the project.
