## Introduction

This document aims to provide an easy way to test and evaluate the 
[WFI32-IoT](https://www.microchip.com/en-us/development-tool/ev36w50a) board 
with IoTConnect, without the need to compile the source code.

The board features USB Mass Storage Device (MSD) support, which makes the IoTConnect setup simple and without the need
to change the source code.

The built-in ATECC608 secure element provides a quick and secure way 
to set up the device with IoTConnect.

The Click board interface makes it possible to make use of hundreds of different Click boards from MikroE.
The [VAV Press Click](https://www.mikroe.com/vav-press-click) 
, [Ultra-Low Press Click](https://www.mikroe.com/ultra-low-press-click)
, [PHT Click](https://www.mikroe.com/pht-click)
, [TEMP&HUM 14 click](https://www.mikroe.com/temphum-14-click)
, [Telaire T6713 CO2 Sensor Module](https://www.amphenol-sensors.com/en/telaire/co2/525-co2-sensor-modules/3399-t6713) on the [PROTO Click](https://www.mikroe.com/proto-click)
, [Telaire T9602 IP67 Harsh Environment Humidity & Temperature Sensor](https://www.amphenol-sensors.com/en/telaire/humidity/527-humidity-sensors/3224-t9602) on the [Terminal 2 Click](https://www.mikroe.com/terminal-2-click)
, [Air Quality 7 click](https://www.mikroe.com/air-quality-7-click)
, [Altitude 4 Click](https://www.mikroe.com/altitude-4-click)
, and [Altitude 2 Click](https://www.mikroe.com/altitude-2-click)
boards are directly supported by the demo software and provide 
the ability to send sensor readings from these boards directly to IoTConnect.

## Hardware Setup

* Plug the VAV Press Click or Ultra-Low Press Click board onto Click interface of the board.
* To have both Click boards connected at the same time to WFI32-IoT, you can connect them via the
[Shuttle Click](https://www.mikroe.com/shuttle-click) adapter.
* When connecting Click boards, ensure that the pin markings on the Click boards are matching to the header markings on the WFI32-IoT.
* The WFI32-IoT board needs to be connected to a USB port on your PC via the Micro USB cable.
* Once the board boots up, a new USB devices will be available. 
  * Optionally, connect a terminal program (like TeraTerm) to one of the two COM ports
which is named "USB Serial". Use defaults for 115200 baud: 8 bits, 1 stop bit, no flow control or parity. 
Firmware logs will be available on that COM port. 
  * The MSD should appear as a new removable drive some time after the board boots up.

## Programming the Firmware

* Download and install the [MPLAB X IDE package](https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide). 
MPLAB IPE and 32-bit device support are the only required options during the installation.
* Download the [binary package zip](https://saleshosted.z13.web.core.windows.net/sdk/AzureRTOS/iotconnect-demo-wfi32-040523.zip)
* Extract the iotconnect-demo.X.production.hex file from the zip.
* Open the Microchip IPE application in order to program the firmware: 
  * In the **Device** entry box, select "WFI32E01"
  * In the **Tool** entry box, select "Curiosity/Starter Kits (PKBO4)"
  * Click the **Connect** button
  * Wait for any updates to complete and ignore any DFP related warnings in the output.
  * After the device is connected, updated and verified (as reported in the output), click the **Browse** button next to the **Hex file** field and select your iotconnect-demo.X.production.hex file that was extracted in a previous step.
  * Click the **Program** button.
  * Screenshot below shows an example of what the IPE would look like once the device has been programmed successfully:

![IPE Screenshot](media/IPE.png "IPE Screenshot")

## Cloud Account Setup
An IoTConnect account is required to continue this guide. If you need to create an account, a free 2-month subscription is available.  Please follow the [Creating a New IoTConnect Account](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/iotconnect/subscription/subscription.md) guide and return to this guide once complete.

## IoTConnect Device Template Setup
A Device Template with Self Signed authentication type will need to be imported.
* Download the premade [Device Template with Self-Signed Auth](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/templates/devices/wfi32iot/wfi32ss_template.JSON).
* Import the template into your IoTConnect instance. (A guide on [Importing a Device Template](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/iotconnect/import_device_template.md) is available or for more information on [Template Management](https://docs.iotconnect.io/iotconnect/user-manuals/devices/template-management/), please see the [IoTConnect Documentation](https://iotconnect.io) website.)

## Obtaining the Device Certificate Fingerprint
This section outlines how to set up the device for IoTConnect Self Signed Certificate authentication type.
Steps for other authentication types are out of scope for this guide and are available in the main README.md.

* In order to complete the next steps, obtain the fingerprint of device certificate.
The device certificate is located in the file named snXXXXXX_device.pem on the Mass Storage Device.
The fingerprint of the certificate can be either SHA256 or SHA1.
There are a couple of ways to go about that:
   * One can execute ``` openssl x509 -noout -fingerprint -inform pem -in snxXXXX_device.pem ``` if openssl is installed.
   * The contents of snxXXXX_device.pem can be pasted into an online
fingerprint calculator such as [this one](https://www.samltool.com/fingerprint.php). 
Note that publishing the device certificate or fingerprint online is not technically unsafe 
because the certificate must be paired by the private key derived data during authentication. 
The private key is securely stored on the device and cannot be accessed even programmatically.
The only information that may potentially "leak" by using a public web site in this fashion is the informational 
manufacturer data, including the device serial number. Below is a sample screenshot:

![Fingerprint Web Site Screenshot](media/fingerprint.png "Fingerprint Web Site Screenshot")

## IoTConnect Device Setup
* Create a new device in the IoTConnect portal. (Follow the [Create a New Device](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/iotconnect/create_new_device.md) guide for a detailed walkthrough.)
* Enter a the "snXXXXXXXX" saved from earlier in the *Unique ID* field and enter a descriptive *Display Name* of your choice.
* Select the template from the dropdwon box that was just imported.
* Enter the Fingerprint calculated in the "Device Configuration" step into the Thumbprint field.
* Click Save and press the Reset button.

## Configuration
To configure the WiFi Credentials and IoTConnect Account environment information, you will need to edit two file located on the USB Mass Storage Device

### Configure the WiFi Credentials
* The WiFi Credentials are configred in the WIFI.CFG file located on the MSD. Open the file in a text editor and input the WiFi credentials using one of the
following templates per the network configuration:
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
* Save the file when done.

### Configure the IoTConnect Account
* Open the CLOUD.CFG file in a text editor. If the contents of CLOUD.CFG do not have text like CPID and ENV, 
delete the file, eject the drive, reset the board and re-open the file as resetting will populate the defaults.
* Set the CPID and Environment per your IoTConnect account settings, which can be found in Settings -> Key Vault in your IoTConnect portal.
* Save the file, eject the USB drive then reset the board.
* Your device should connect to your IoTConnect account and publish sensor data periodically.
