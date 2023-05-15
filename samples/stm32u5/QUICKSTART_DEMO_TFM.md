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

* Download and extract the latest [binary package](https://saleshosted.z13.web.core.windows.net/sdk/AzureRTOS/stm32u5-tfm-package-041823.zip) zip file.
* Connect a USB cable to the Micro USB slot of the B-U585I-IOT02A board.
* Execute the *trust-zone-enable.bat* batch script included in the package.<br>This will enable the TrustZone feature which is required to run the next step.
Running this script will make it so that only TrustZone enabled applications are authorized to run on the board. This only needs to be run once per board.
* Execute the *tfm-update.bat* batch script included in the package to setup TFM on the board.
* Note: After completion of this demo, the *trust-zone-disable.bat* batch script can be run to once again allow non-TrustZone applications to be executed. Do not run this now.

## Device Configuration
* Ensure your serial termainal application is running.
* In your terminal application enter "y" when prompted to set device configuration.
* Set values for you WiFi SSID (Network Name) and Password
* Set the values for your CPID and Environment. These values can be located 
in  the IoTConnect WebUI on the *Key Vault* page. Navigate there using the image below:<br>![Key Vault](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/assets/key_vault.png "Key Vault")
* The DUID value can be left unset. A DUID unique to your device 
will be generated and displayed on the console during startup.
* To utilize the secure element, leave the symetric key blank as this will allow for STSAFE with x509 authentication.
* After configuring your device's settings, reset the device and monitor the serial terminal for information similar to the screenshot below
  ![Certificate and DUID Screemshot](media/duid-and-certifciate-console.png "Certificate and DUID Screemshot") 
* A device fingerprint needs to be generated from the certificate.
  * Copy the Device Certificate from the console, including the BEGIN and END lines.
  * Paste the contents into the X509 Cert field on  [this web site](https://www.samltool.com/fingerprint.php). (Optionally you can use openssl to print the device fingerprint, but this is outside the scope of this guide.)
  * Leave the "Algorithum" selection at the default, press "Calculate Fingerprint" and copy/save the Fingerprint field for later use.

## IoTConnect Template Setup
A Device Template with Self Signed authentication type will need to be imported.
* Download the premade [Device Template with Self-Signed Auth](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/templates/devices/stm32u5/stm32u5self_signed_template.JSON).
* Import the template into your IoTConnect instance. (A guide on [Importing a Device Template](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/iotconnect/import_device_template.md) is available or for more information on [Template Management](https://docs.iotconnect.io/iotconnect/user-manuals/devices/template-management/), please see the [IoTConnect Documentation](https://iotconnect.io) website.)

## IoTConnect Device Setup
* Create a new device in the IoTConnect portal. (Follow the [Create a New Device](https://github.com/avnet-iotconnect/avnet-iotconnect.github.io/blob/main/documentation/iotconnect/create_new_device.md) guide for a detailed walkthrough.)
* Enter a *Unique ID* and descriptive *Display Name* of your choice.
* Select the template from the dropdwon box that was just imported.
* Enter the Fingerprint calculated in the "Device Configuration" step into the Thumbprint field.
* Click Save.

## IoTConnect Dashboard Setup
An easier way to view sensor data from the device is to visualize using a dashboard.
* Download the premade *STM32U5 TFM Dashboard Example* and follow the (TBD) *Import an Existing Dashboard* guide to create the dasbhaord.  Be sure to connect the dashboard elements to your specific device using the *Unique ID* created in the previous a step.
