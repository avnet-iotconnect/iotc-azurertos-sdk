## Introduction

This document is aiming to provide an easy way to test out and evaluate the 
[WFI32-IoT](https://www.microchip.com/en-us/development-tool/ev36w50a) board 
along with IoTConnect, without the need to compile the source code.

The board features USB Mass Storage Device (MSD) support, which makes quick IoTConnect setup possible without the need
to change the source code.

The built-in ATECC608 secure element provides a secure and quick way 
to set up the device with IoTConnect.

The Click Board interface makes it possible to make use of hundreds of different Click Boards from MikroE.
The [VAV Press Click](https://www.mikroe.com/vav-press-click) and [Ultra-Low Press Click]() boards are
directly supported by the demo software and allow you to send the sensor readings from these boards to IoTConnect.

## Hardware Setup

* You can plug in either the VAV Press Click or Ultra-Low Press Click bard onto Click interface of the board.
* To have both Click boards connected at the same time to WFI32-IoT, you can connect them via the
[Shuttle Click](https://www.mikroe.com/shuttle-click) adapter.
* When connecting Click Boards, ensure that the pin markings on the Click boards are matching to the header markings on the WFI32-IoT.
* The WFI32-IoT board needs to be connected to a USB port on your PC via the Micro USB cable.
* Once the board boots up, new USB devices will be available. 
  * You can optionally connect a terminal program (like TeraTerm) to one of the two COMM ports
which is named "USB Serial". Use defaults for 115200 baud: 8 bits 1 stop bit. no flow control or parity. 
Firmware logs will be available on that COMM port. Use trial and error to determine which one of the two ports. 
  * The MSD should appear as a new removable drive some time after the board boots up.

## Propgramming the Firmware

* Download and install the [MPLAB X IDE package](https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide). 
You only need to select MPLAB IPE and 32-bit device support during the installation.
* Download the [binary package zip](https://saleshosted.z13.web.core.windows.net/sdk/AzureRTOS/iotconnect-demo-wfi32-011123.zip)
* Extract the iotconnect-demo.X.production.hex file from the zip.
* Open the Microchip IPE application in order to program the firmware: 
  * Enter WFI32E01 in the **Device** entry box
  * Select "Curiosity/Starter Kits (PKBO4)" for the **Tool**. Click the **Connect** button next to the entry.
  * Ignore any DFP related warnings in the logs.
  * Once the device connect, for the **Hex file** filed, click the **Browse** button on the right and select your iotconnect-demo.X.production.hex file that was extracted in the previous step.
  * Click the **Program** button.
  * Screenshot below shows an example of what the IPE would look like once the device has been programmed successfully:

![IPE Screenshot](media/IPE.png "IPE Screenshot")

### IoTConnect Template Setup

* At your IoTConnect account web page, navigate to Devices->Device->Templates(dock at the bottom) and click Create Template on that page.
  * Enter a desired name like "wfi32demo" for Template Code and Template Name. 
  * Select *Self Signed Certificate* in the Authentication Type pulldown.
  * Ensure that **Device Message Version** is **1.0**
  * Click "Save"
* At the same page, click the Attributes tab.
* Add a field to the list of attributes called "Version" of type ***STRING** 
* Add the following NUMERIC fields to the list of attributes. ULP or VAV fields are optional, if you have the Click Boards:
  * *WFI32IoT_temperature* - Note that this value seems rather high. It is likely measuring the chip internal temperature of the WFI32-IoT.
  * *WFI32IoT_light* - The amount light value reported by the light sensor on the WFI32-IoTboard.
  * *ULP_pressure* - Air pressure measured on the Ultra-Low Press Click Board.
  * *ULP_temperature* - Temperature measured on the Ultra-Low Press Click Board.
  * *VAV_pressure* - Air pressure measured on the VAV Press Click Board.
  * *VAV_temperature* - Temperature measured on the VAV Press Click Board.
* the screenshot below shows an example template:

![Template Screenshot](media/template.png "Template Screenshot")

## Obtaining the Device Certificate Fingerprint

We will be explaining how to set up your device for Self Signed Certificate IoTConnect authentication type.
Steps for other authentication types are of scope for this gide and are available in the main README.md.

* In order to complete the next steps, we need to obtain the fingerprint of our device certificate.
The device certificate is located on the MSD file named snXXXXXX_device.pem.
The fingerprint of the certificate can be either SHA256 or SHA1.
There are a couple of ways to go about that:
   * One can execute ``` openssl x509 -noout -fingerprint -inform pem -in snxXXXX_device.pem ``` if openssl is installed.
   * The contents of snxXXXX_device.pem can be pasted into a public
web site online like [this one](https://www.samltool.com/fingerprint.php). 
Note that publishing the device certificate or fingerprint online is not technically unsafe 
because the certificate must be paired by the private key derived data during authentication. 
The private key is securely stored on the device and cannot be accessed even programmatically.
The only information that may potentially "leak" by using a public web site in this fashion is the informational 
manufacturer data, including the device serial number. Below is a sample screenshot:

![Fingerprint Web Site Screenshot](media/fingerprint.png "Fingerprint Web Site Screenshot")

### Setting up The Device in IoTConnect
 
* Navigate to Device->Device and click the **Create Device** button.
* Your device unique ID is the "snXXXXXXXX" collection numbers and letters by which the files on the MSD are created. 
Enter that name (including "sn") as the device name and display name.
* Choose your entity where you will create the device.
* Select the Template that was created in the previous step.
* Enter the fingerprint obtained in the previous step (without colons) as the **Primary Thumbprint**. See screenshot below.
* Click the *Save*.

![Device Creation Screenshot](media/iotc-device.png "Device Creation Screenshot")

### Setting up the WIFI.CFG file.

* The WIFI.CFG file is located on the MSD. Open the file in a text editor and write your WiFi credentials into it like this:
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


### Setting up the CLOUD.CFG file

* Ensure that you eject the drive, and then restart the board after making any changes to the files and restarting the board.
The PC filesystem may be caching the file contents and it may not allow the device to complete writing to the device before a restart.
* Open the CLOUD.CFG file in a text editor. If the contents of CLOUD.CFG do not have text like CPID and ENV, 
delete the file, eject the drive, restart the board and re-open the file.
Restarting will populate the defaults.
* Set the CPID and Environment per your IoTConnect account settings, which can be found in Settings->Key Vault at your IoTConnect portal.
* Save the file, eject the drive and restart the board.
* Your device should connect to your IoTConnect account and publish sensor data periodically



