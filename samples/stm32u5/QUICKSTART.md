## Introduction

This document provides a step-by-step-guide to program and evaluate the 
[B-U585I-IOT02A STM32U4 Discovery kit for IoT](https://www.st.com/en/evaluation-tools/b-u585i-iot02a.html) board 
with Trusted Firmware-M support on IoTConnect.

This guide currently only supports Windows 64-bit.

## Required Software

* Download and install the [STM32CubeProgrammer ](https://www.st.com/en/development-tools/stm32cubeprog.html) for STM32.<br>(Ensure the 64-bit version is installed to the default  directory ("C:\Program Files\")
* A serial console application, such as [Tera Term](https://ttssh2.osdn.jp/index.html.en), is required for the next steps. 
Configure settings per the screenshot below:

![Tera Term Serial Settings](media/teraterm-settings.png "Tera Term Serial Settings")

## Cloud Account Setup
An IoTConnect account is required to continue this guide. If you need to create an account, a free 2-month subscription is available.  Please follow the [Creating a New IoTConnect Account](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/iotconnect/subscription/subscription.md) guide and return to this guide once complete.

## Firmware Setup

* Download and extract the latest [binary package](https://saleshosted.z13.web.core.windows.net/sdk/AzureRTOS/stm32u5-tfm-package-050223.zip) zip file.
* Connect a USB cable to the Micro USB slot of the B-U585I-IOT02A board.
* Execute the *trust-zone-enable.bat* batch script included in the package.<br>This will enable the TrustZone feature which is required to run the next step.
Running this script will make it so that only TrustZone enabled applications are authorized to run on the board. This only needs to be run once per board.
* Execute the *tfm-update.bat* batch script included in the package to setup TFM on the board.
* Note: After completion of this demo, the *trust-zone-disable.bat* batch script can be run to once again allow non-TrustZone applications to be executed. Do not run this now.

## Device Configuration
* Ensure your serial termainal application is running.
* Press the RST button on the board and look for the "X-CUBE-AZURE" title block. This may take 1 or two presses of the button. (The reset button is the black button labled "RST" next to the only blue button on the top of the board.)
* In your terminal application enter "y" when prompted to set device configuration. You will only need to set values 1-4.
  * Configure your WiFi SSID (Network Name) by pressing 1
  * Configure your WiFi Password by pressing 2
  * Set the values for your CPID and Environment by pressing 3 and 4 respectively. These values can be located 
in  the IoTConnect WebUI on the *Key Vault* page. Navigate there using the image below:<br>![Key Vault](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/assets/key_vault.png "Key Vault")
  * The DUID value should be left unset. A DUID unique to your device will be generated and displayed on the console during startup.
  * The Symetric key should be left unset. This will allow for authentication using STSAFE with x509 authentication.
  * Press '0' to write the configuration/
  * Press the Reset button
* After configuring your device's settings, reset the device and monitor the serial terminal for information similar to the screenshot below
  ![Certificate and DUID Screemshot](media/duid-and-certifciate-console.png "Certificate and DUID Screemshot") 
* A device fingerprint needs to be generated from the certificate.
  * Copy the Device Certificate from the console, including the BEGIN and END lines.
  * Paste the contents into the X509 Cert field on  [this web site](https://www.samltool.com/fingerprint.php). (Optionally you can use openssl to print the device fingerprint, but this is outside the scope of this guide.)
  * Leave the "Algorithum" selection at the default, press "Calculate Fingerprint" and copy/save the Fingerprint field for later use.
* Copy the generated DUID displayed and save for later use.

## IoTConnect Device Template Setup
A Device Template with Self Signed authentication type will need to be imported.
* Download the premade [Device Template with Self-Signed Auth](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/templates/devices/stm32u5/stm32u5self_signed_template.JSON).
* Import the template into your IoTConnect instance. (A guide on [Importing a Device Template](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/iotconnect/import_device_template.md) is available or for more information on [Template Management](https://docs.iotconnect.io/iotconnect/user-manuals/devices/template-management/), please see the [IoTConnect Documentation](https://iotconnect.io) website.)

## IoTConnect Device Setup
* Create a new device in the IoTConnect portal. (Follow the [Create a New Device](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/iotconnect/create_new_device.md) guide for a detailed walkthrough.)
* Enter a the DUID saved from earlier in the *Unique ID* field and enter a descriptive *Display Name* of your choice.
* Select the template from the dropdwon box that was just imported.
* Enter the Fingerprint calculated in the "Device Configuration" step into the Thumbprint field.
* Click Save and press the Reset button.

## Verification
At this point the board should be sending telemetry to the IoTConnect portal. We can verify by checking the "Live Data" feed.
* Return to the *Devices* page and click on the newly created Device ID.
* On the left sub-menu, click "Live Data" and after a few seconds, MQTT data should be shown. See below:<br>![image](https://github.com/avnet-iotconnect/iotc-azurertos-sdk/assets/40640041/21d25bbb-71d0-4a9d-9e74-e2acf0983183)

## Troubleshooting
Using the serial terminal is the best way to identify issues. Common issues can be resolved by verifying the following items:
* Output stopping with a message about "IP Address":  Ensure valid WiFI credentials are used and that the network has an operational DHCP server.
* Output stopping before data is sent:  Verify CPID and Environment names
* Output stopping with "No Device Found":  Ensure a new device was created in the portal and that the DUID matches the Device ID

## Further Reading
* Visualize the data using [Dynamic Dashboards](https://help.iotconnect.io/documentation/dashboard/).
