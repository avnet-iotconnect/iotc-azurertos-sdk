## Introduction

This document aims to provide a step-by-step-guide way to test and evaluate the 
[B-U585I-IOT02A STM32U4 Discovery kit for IoT](https://www.st.com/en/evaluation-tools/b-u585i-iot02a.html) board 
with IoTConnect.

### Project Build Setup

The software must be built and programmed onto the board by using the following steps:

* If on Windows, download and install [MSYS2](https://www.msys2.org/). [WSL](https://learn.microsoft.com/en-us/windows/wsl/about) or GIT Bash may also work.
* Ensure that you have git command line installed. If on Windows, you can use [this link](https://gitforwindows.org/).   
* Clone this repo
* Download the [X-CUBE-AZURE Expansion Package](https://www.st.com/en/embedded-software/x-cube-azure.html) version 2.1.0, 
and place the zip into the [samples/stm32u5](samples/stm32u5) directory.
* In a bash shell, execute:

```shell
cd <root of this repo>
scripts/setup-project.sh stm32u5 
```

* Download, install and open the [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html).
* In STM32CubeIDE, select *File -> Open Projects from File System*.
* Select the [samples/stm32u5](samples/stm32u5) directory of this repo in the dialog box and click *Finish*.
* Uncheck the stm32u5 root project and leave the other project checked like this:

![Import Project Screenshot](media/import-project.png "Import Project Screenshot]")

### IoTConnect Template Setup

* Login to the IoTConnect platform, navigate to *Devices -> Device -> Templates* and click **Create Template**.
* Enter a name like "stm32u5" for **Template Code** and **Template Name**. 
* Select *Symmetric Key* in the **Authentication Type** pulldown.
* Ensure that **Device Message Version** is **1.0**
* Click **Save**
* On the same page, click the Attributes tab.
* Add a field to the list of attributes called "version" of type **STRING** 
* Add the following **NUMERIC** fields to the list of attributes. ULP or VAV fields are optional, if you have the Click Boards:
  * *temperature* - Note that this value seems rather high. It is likely measuring the chip internal temperature of the chip or module.
  * *humidity* - Relaitve air humidity.
  * *pressure* - Air pressure.
  * *button_counter* - Recorded number of button presses since board start or "reset_counters" command was issued (see below).
* Add the following **OBJECT** fields to the list of attributes, *adding x, y, and z NUMERIC values to each object*
  * *accelerometer* - Accelerometer force measured in one-thousandth of Earth gravity relative to the board.
  * *magnetometer* - Magnetic vector direction pointing north, relative to the board.
  * *gyroscope* - World orientation vector of the board.

![Template Screenshot](media/template-screenshot.png "Template Screenshot")

* Add the following commands in the *Command* tab:
  * *led-red*   - Toggle the *Parameter Required* toggle button to *enabled*. If parameter is set to "on", the red LED on the board next to the blue user button will be turned on. 
  * *led-green* - Toggle the *Parameter Required* toggle button to *enabled*. If parameter is set to "on", the green LED on the board next to the blue user button will be turned on. 
  * *reset-counters* - When issued, this command will reset the button press counter to zero.

### IoTConnect Device Setup

* Run this command, or obtain a randomly generated Base64 encoded 16 to 64 bytes:

```shell
dd if=/dev/urandom bs=32 count=1 status=none | base64
```

* Navigate to Device -> Device and click the **Create Device** button.
* Enter a name for the device like "stm32u5"
* Choose your entity where you will create the device.
* Enter the Base64 encoded value in the previous step as Primary and Secondary Key. 
We will be using the same random value the section below, so do not close the command output window just yet.
* Click **Save**.

![Device Creation Screenshot](media/create-device.png "Device Creation Screenshot")


### Project Build and Device Setup

* In STM32CubeIDE, edit the app_config.h file located under Nx_Azure_Iot/basic-sample/include in the *Project Explorer* tab.
* Set WIFI_SSID and WIFI_PASSWORD per your WiFi credentials.
* Set the IOTCONNECT_CPID and IOTCONNECT_ENV  per your IoTConnect account settings, which can be found in Settings -> Key Vault in your IoTConnect portal.
* Set the IOTCONNECT_SYMMETRIC_KEY value to the same Base64 encoded random key in the steps above.
* Set the IOTCONNECT_DUID to the device ID that was selected in the Deivce Creation step above.
* Connect your STM32U5 board to a USB port on your PC using a Micro USB cable.
* Optionally, connect a terminal program (like TeraTerm) to newly available COMMport. Use defaults for 115200 baud: 8 bits, 1 stop bit, no flow control or parity.
* Right-Click on the Nx_Azure_Iot project in the *Project Explorer* tab and select *Debug As -> STM32 Cortex-M C/C++ Application.
* The above step should program your board,the device should connect and send periodic sensor readings in IoTConnect.  


