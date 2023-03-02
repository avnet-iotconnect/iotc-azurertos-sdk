
# Renesas CK-RX65N Quickstart Guide

This document is intended to introduce the use of the blue Renesas CK-RX65N
board, using the ck-rx65n sample project, with IoTConnect.

<img style="width:45%; height:auto" src="./assets/ck-rx65n/IMG_20230316_120246661-crop.jpg"/>

## Prerequisites

- Windows PC (this setup guide details installation, etc. on a Windows PC; a
  Linux setup might be setup in an analogous manner).
- Internet connection
- Internet with ethernet connection for the Renesas board’s ethernet.
- optionally: plastic tweezers.

## Download and install latest Renesas GCC

You will need to register to [download the latest GCC
release](https://llvm-gcc-renesas.com/rx-download-toolchains/).

Please consult Renesas documentation on installing the latest GCC release.

## Download and install latest e<sup>2</sup> studio

You will need to register to [download the latest e<sup>2</sup> studio
release](https://www.renesas.com/us/en/software-tool/e2studio-information-rx-family).

Please consult Renesas documentation on installing the latest e<sup>2</sup>
studio.

The following options are required for Cloudkit development: RX support, and
optionally Jlink debugging, terminal, and git support.

## Run/setup e<sup>2</sup> studio

You will need to use the your Renesas account information that was used to
download the latest e<sup>2</sup> studio release when setting up/registering
e<sup>2</sup> studio.

<details><summary>EXAMPLE STEPS TO RUN AND SETUP E2 STUDIO</summary>

<img style="width:23.59%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_08_02_2023_10_22_40.png"/>

<img style="width:24.1%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_08_02_2023_10_22_51.png"/>

<img style="width:20.51%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_08_02_2023_10_23_06.png"/>

<img style="width:23.52%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_08_02_2023_10_23_21.png"/>

<img style="width:39.33%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_08_02_2023_10_23_34.png"/>

<img style="width:39.33%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_08_02_2023_10_23_45.png"/>

<img style="width:39.41%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_08_02_2023_10_24_20.png"/>

<img style="width:75%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_08_02_2023_10_25_07.png"/>

<img style="width:75%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_08_02_2023_10_25_22.png"/>

</details>

## Optional: Update e<sup>2</sup> studio to latest versions

"Check for Updates" – can be a little slow.

<details><summary>EXAMPLE STEPS TO UPDATE E2 STUDIO</summary>

<img style="width:75%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_08_02_2023_10_37_17.png"/>

<img style="width:75%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_08_02_2023_10_40_40.png"/>

<img style="width:75%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_08_02_2023_10_40_55.png"/>

<img style="width:75%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_08_02_2023_10_41_03.png"/>

<img style="width:75%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_08_02_2023_10_44_00.png"/>

</details>

## Optional: setup egit in e<sup>2</sup> studio

If using egit support in e<sup>2</sup> studio then [follow Renesas
notes](https://www.renesas.com/us/en/document/apn/integrated-development-environment-e-studio-how-use-egit-e-studio).

<details><summary>EXAMPLE STEPS TO SETUP EGIT</summary>

<img style="width:75%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_08_02_2023_10_45_37.png"/>

<img style="width:75%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_08_02_2023_10_46_00.png"/>

<img style="width:75%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_08_02_2023_10_46_37.png"/>

</details>

## Using git clone to access iotc-azurertos-sdk

The `git clone` details can be found on the [IotConnect Azure RTOS SDK github
project](https://github.com/avnet-iotconnect/iotc-azurertos-sdk).

<img style="width:75%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_08_02_2023_10_50_22.png"/>

### Using e<sup>2</sup> studio to git clone iotc-azurertos-sdk

### Install "Git for Windows" and clone iotc-azurertos-sdk

Alternatively, install ["git for Windows"](https://gitforwindows.org/).

And use the bash shell to clone the repository – right-click on directory -
"Other options", then "Git Bash" to launch the shell.

<img style="width:39.98%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_03_03_2023_11_06_48.png"/>

### Windows Subsystem for Linux (WSL)

Alternatively, install Windows Subsystem for Linux (WSL) and clone the
repository

### Cygwin

Alternatively, install [Cygwin](https://www.cygwin.com) - and ensure that
`git`, `unzip` and at least one of `wget` or `curl` are installed - and clone
the repository.

## Setup the Renesas iotc-azurertos-sdk project fully

The setup process requires a bash-like shell (not cmd) – could use Windows
Subsystem for Linux (WSL), MinGW shell from "git for Windows", or Cygwin. The
shell needs access to:

- `git`
- `unzip`
- `wget`, or `curl`

To setup the `iotc-azurertos-sdk` ck-rx65n project fully:

- change into the `scripts` directory in the `iotc-azurertos-sdk` project: `cd
  iotc-azurertos-sdk/scripts`
- run the script: `setup-project.sh ck-rx65n`

<img style="width:39.98%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_03_03_2023_11_13_34.png"/>

## Import iotc-azurertos-sdk ck-rx65n project into e<sup>2</sup> studio

Need to import the `samples/ck-rx65n` project into e<sup>2</sup> studio to
build and debug - note deselect any "CMake Project" items, so that the
r65ncloudkit (and its subprojects) are imported as Eclipse projects.

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_09_46_14.png"/>

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_09_46_48.png"/>

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_09_47_29.png"/>

Once the project is imported into e<sup>2</sup> studio, then check that the
latest version of the compiler is used for ck-rx65n project and each of
the subprobjects.

Expand the ck-rx65n project.  Right click on "basic-sample" and bring up
the "C/C++" properties sheet.  Ensure that the toolchain being used is the
latest one – if not, then select the latest one, and "Apply and Close".

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_09_47_54.png"/>

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_09_48_07.png"/>

There may be other dialogs about smart-manual – accept/dismiss as required.

## Update the RX Driver Package (FIT modules)

Expand "basic-sample" and the "src" subdirectory, so that "basic-sample.scfg"
can be seen in the "Project Explorer" pane.

Double-click on "basic-sample.scfg" to bring up the "Overview Information".
Download any FIT modules.

Press the "Generate Code" button at the top-right corner of the "Project
Explorer" pane.

<details><summary>EXAMPLE STEPS TO UPDATE E2 STUDIO FIT MODULES</summary>

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_09_48_35.png"/>

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_09_48_46.png"/>

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_09_48_52.png"/>

Missing components (in the component tab) are shown as grey, clicking on them
will suggest a download.

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_10_40_39.png"/>

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_10_41_12.png"/>

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_10_41_24.png"/>

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_10_41_31.png"/>

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_10_41_38.png"/>

</details>

## Clean and build the project

Right-click on the basic-sample item under the ck-rx65n project in the
"Project Explorer" and select "Clean Project" in case any previous build
artefacts are present.

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_10_41_55.png"/>

Right-click on the basic-sample item in the "Project Explorer" and select
"Build Project".

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_10_42_32.png"/>

### Build products

Build product (`basic-sample.elf`, `basic-sample.mot`, etc.) is generated by
default under the `ck-rx65n/basic-sample` directory in the `HardwareDebug`
sub-directory.

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_10_47_50.png"/>

## Setup the CK-RX65N board

[Setup the CK-RX65N](https://www.renesas.com/us/en/products/microcontrollers-microprocessors/rx-32-bit-performance-efficiency-mcus/rx65n-cloud-kit-renesas-rx65n-cloud-kit).

Connect the two USB-micro leads and the ethernet cable:

<img style="width:45%; height:auto" src="./assets/ck-rx65n/IMG_20230303_093310710-crop.jpg"/>

The power LED should be lit. The power LED forms the "-" in the CK-RX65N on the silk-screen on the board, above the fingerprint reader.

<img style="width:45%; height:auto" src="./assets/ck-rx65n/IMG_20230316_120246661-crop-power.jpg"/>

### Setup a terminal connection

Install
[termite](https://www.virtual-serial-port.org/articles/alternative-to-termite-terminal/),
[Tera Term](https://ttssh2.osdn.jp/index.html.en), or a similar terminal
application.

May need to press the "reset" button to restart to see output.

The terminal settings to connect are:

- Chose appropriate COM port. Note: In Linux the terminal may enumerate as
  `/dev/ttyACM0`.
- 115200 baud rate
- 8-bit data
- No parity

<img style="width:30%; height:auto" src="./assets/quickstart/teraterm1.png"/>
<img style="width:30%; height:auto" src="./assets/quickstart/teraterm2.png"/>

The default installed application behaviour is shown below:

<img style="width:30%; height:auto" src="./assets/ck-rx65n/Installed_App1.png"/>
<img style="width:30%; height:auto" src="./assets/ck-rx65n/Installed_App2.png"/>
<img style="width:30%; height:auto" src="./assets/ck-rx65n/Installed_App3.png"/>

<img style="width:30%; height:auto" src="./assets/ck-rx65n/Installed_App4.png"/>
<img style="width:30%; height:auto" src="./assets/ck-rx65n/Installed_App5.png"/>
<img style="width:30%; height:auto" src="./assets/ck-rx65n/Installed_App6.png"/>

## Running/debugging a build generated by e<sup>2</sup> studio

_Warning this will overwrite any software flashed/supplied previously on the
CK-RX65N board._

Need to ensure the jumper on the right of the board is switched to debug before running/debugging a build -
obviously, remove any USB leads to power off the board before doing so.

<img style="width:45%; height:auto" src="./assets/ck-rx65n/IMG_20230316_120246661-crop-debug.jpg"/>

### Create a debug .launch file

If a .launch file is not present in the basic-sample directory, e.g the .launch
file may be called `basic-sample HardwareDebug.launch`, then you will need to
create a suitable .launch file.

<details><summary>EXAMPLE OF CREATING A .LAUNCH FILE</summary>

Ensure the ck-rx65n board is connected to the PC and is powered up, i.e.
the top-left green LED is lit.

Right click on the basic-sample and select "Debug as" then "Renesas GDB
Hardware Debugging".

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_10_49_06.png"/>

Select "E2 Lite (RX)" as the debug hardware

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_10_49_15.png"/>

Select "R5F565NE" as the "target device"

<img style="width:45%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_10_49_29.png"/>
<img style="width:45%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_10_49_42.png"/>

This should create a `basic-sample.elf.launch` file.

This file may not immediately work, so right-click again on basic-sample to
bring up the "Debug Configurations" panel

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_10_50_31.png"/>

Check that "E2 Lite (RX)" and "R5F565NE" are selected

<img style="width:45%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_10_50_38.png"/>
<img style="width:45%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_10_50_52.png"/>

Click on the "Connection Settings" tab, and set the "Connection Type" to be
"Fine"

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_10_51_12.png"/>

"Apply" and "Close", and then it should be possible to debug the rx65nxcloudkit
IoTConnect platform.

</details>

### Debug code

Press the "Launch in ‘Debug’ mode" button ("bug") on the left-side of
e<sup>2</sup> studio.

If there are errors:
- check that the board is connected via USB
- check that the jumper has been removed (once powered off)

<details><summary>EXAMPLE STEPS TO RUN/DEBUG</summary>

Respond appropriately to any pop-ups relating to Windows Defender Firewall prompts.

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_10_54_08.png"/>

Switch to the debug perspective when prompted.

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_10_55_00.png"/>

</details>

The build will have two default breakpoints already enabled, so presume the
Resume button (green "left-pointing triangle") in the middle of e<sup>2</sup>
studio twice to enable to code to execute past those two breakpoints.

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_10_55_12.png"/>

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_10_55_21.png"/>

Output should be visible in any attached terminal now.

When finished press the Terminate button (red "square") next to the Resume
button.

To debug again, press the debug button.

To return to C/C++ development, select the "Window", then "Perspective", then
"Open perspective", then "C/C++ project".

To return to debugging/running, select the "Window", then "Perspective", then
"Open perspective", then "Debug".

#### Developing with e<sup>2</sup> studio

May need to select the "Refresh" option by right-clicking on a project in
e<sup>2</sup> studio if/when the git branch has been changed, or other
"external" changes have been made.

Also if a git pull modifies any git submodules, then will likely need to rerun
setup-project.sh.

Without editting `app_config.h` the code will produce the following output:

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_11_00_50.png"/>

Note: If the board is powered-down and the jumper replaced, it seems that the
board will be flashed to run the newly "debugged" code, replacing previous
code.

## Create a sequence of 16-64 random bytes that are Base64 encoded - for use as a Symmetric Key

To create a random symmetric "key" we will generate a random 32 byte sequence
and then Base64 encode it – so that the binary values can be shared easily.

Run this command in a shell, e.g. in Git for Windows Bash, Cygwin, etc.:

    dd if=/dev/urandom bs=32 count=1 status=none | base64

A symmetric key is used to both encode and decode - so the same key is used on
the IoT device and on the IoTConnect server.

<img style="width:28.63%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_21_02_2023_10_04_32.png"/>

Make a note of this output, as it will be used to setup the IoTConnect template
for the device on the IoTConnect dashboard – and in the application
configuration file `app_config.h` that will be compiled/built and run on the
device.  This is important as isn’t possible to recover key information once
added to the IoTConnect dashboard – also consider if key information might get
removed from any local iotc-azurertos-sdk git repository (if it gets cleaned,
branches are deleted, or changes get reverted). Obviously, key information
shouldn’t be pushed to external public repos.

In this case the symmetric key is:

    YzlgdRbYcreYW1fhjwxO4b3X7hBlDY3OVuw6q9wDbAo=

## IoTConnect template and device

For further information, please consult the "Knowledge Base" which can be found
on the [left side of the IoTConnect dashboard](https://help.iotconnect.io).

<img style="width:75%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_17_02_2023_13_41_11.png"/>

### Create IoTConnect template

Log into your IoTConnect account and open the appropriate Device page which can
be found on the [left side of the IoTConnect
dashboard](https://avnet.iotconnect.io/device/1).

The template page can be selected by using the "Templates" tab at the bottom of
the page.

<img style="width:75%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_20_02_2023_14_15_26.png"/>

Press "Create Template" at the top right corner of the page to create a new
template.

When creating the template choose a unique name for the "Template Code" (which
is a maximum of 10 characters and must start with a letter) and a useful
description in the "Template Name".  Choose "Symmetric Key" for the
"Authentication Type" and choose "2.1" for the "Device Message Version".

<img style="width:75%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_20_02_2023_14_40_52.png"/>

When the template is saved, we can add attributes – "measurements" (and types)
– for values that will be sent by the ck-rx65n board.

<img style="width:75%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_20_02_2023_14_41_51.png"/>

Click "Attributes (0)" and add a "version" attribute with STRING type and "Save" it.

<img style="width:75%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_20_02_2023_14_49_29.png"/>

Add a second "button" attribute with BOOLEAN type, a third "temperature" attribute with
DECIMAL type, and a fourth "humidity" attribute with DECIMAL type.

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_13_53_25.png"/>

Note: in the ck-rx65n basic-sample the attribute values are sent using
`iotcl_telemetry_set_xxx()` style commands.

Make a note of the template name, as this will be used as the "Template" to
create the IoTConnect device in the next step – but it will be visible in the
drop-down list.

### Create IoTConnect device

Log into your IoTConnect account and open the appropriate Device page which can
be found on the [left side of the IoTConnect
dashboard](https://avnet.iotconnect.io/device/1).

The device page can be selected by using the "Devices" tab at the bottom of the
page.

<img style="width:75%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_21_02_2023_09_56_06.png"/>

- Chose a "Unique Id" for your device – this will be used in `app_config.h`
  built/compiled in the device later.
- Chose a "Display Name" for your device.
- Select an appropriate "Entity" – this is a pre-populated menu item.
- Select the "Template" display name that was created in the previous step –
  this will require filling in extra fields "Primary Key" and "Secondary Key".
- Use the symmetric key that was generated previously, including any trailing
  "=" signs, e.g. `YzlgdRbYcreYW1fhjwxO4b3X7hBlDY3OVuw6q9wDbAo=` as the value
  for the "Primary Key" and the "Secondary Key".

<img style="width:75%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_21_02_2023_09_57_43.png"/>

### _WARNING!!! this generated Symmetric Key didn’t work I’ve had to use the value_ cGFzc3dvcmRwYXNzd29yZAo=

Values I generated by `dd` and encoded weren't accepted as valid `base64`
encodings on the IoTConnect create a device page -- note the `dd` command is
borrowed from `stm32u5/QUICKSTART_DEMO.md`

When I created the template I pasted the same value in both the
"Primary Key" and the "Secondary Key" and included the trailing "=" equals
sign.

## Updating `app_config.h`

In `app_config.h`

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_10_39_00.png"/>

you will need to specify:

    //
    // Copyright: Avnet 2022
    //
    
    #ifndef APP_CONFIG_H
    #define APP_CONFIG_H
    
    #define IOTCONNECT_CPID ""
    #define IOTCONNECT_ENV  ""
    
    #define IOTCONNECT_DUID "" // you can supply a custom device UID, or...
    #define DUID_PREFIX "" // mac address will be appended in format 012345abcdef
    
    // if it is not defined, device certs will be used in iotconnect_app.c
    #define IOTCONNECT_SYMETRIC_KEY ""
    
    #define SAMPLE_SNTP_SERVER_NAME ""    /* SNTP Server.  */
    
    #endif // APP_CONFIG_H

After updating `app_config.h`, then in e<sup>2</sup> studio: refresh, clean,
and build the ck-rx65n basic-sample, so that any modifications are
incorporated into the binary -- before debugging/running, or flashing the
board.

### `SAMPLE_SNTP_SERVER_NAME`

Choose a suitable public NTP time server, e.g. "time.google.com".

### `IOTCONNECT_CPID` and `IOTCONNECT_ENV`

Log into your IoTConnect account and open the Key Vault which can be found on
the [left side of the IoTConnect
dashboard](https://avnet.iotconnect.io/key-vault).

<img style="width:75%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_17_02_2023_13_15_37.png"/>

#### `IOTCONNECT_CPID`

This is the value under "CPID:" at the top left of the "Home / Setting / Key
Vault" screen.

#### `IOTCONNECT_ENV`

This is the value under "Environment:" at the top middle of the "Home / Setting
/ Key Vault" screen.

### `IOTCONNECT_DUID` and `DUID_PREFIX`

Log into your IoTConnect account and open the appropriate page for the device
that you created previously.  The device should be found on the [left side of
the IoTConnect dashboard](https://avnet.iotconnect.io/device/1).

#### `IOTCONNECT_DUID`

This is the "Unique ID" for the IoTConnect device that you previously created.

<img style="width:75%; height:auto" src="./assets/quickstart/VirtualBox_WinDev2301Eval_17_02_2023_13_24_18.png"/>

#### `DUID_PREFIX`

This can be ignored provided that `IOTCONNECT_DUID` has some meaningful defined
value.

### `IOTCONNECT_SYMETRIC_KEY`

Paste your previously created Base64 encoded symmetric key.

## Running the ck-rx65n board with customised `app_config.h`

After saving/refreshing/rebuilding and initialising the debug session in
e<sup>2</sup> studio, example output is shown on the left in the terminal (Tera
Term VT):

<img style="width:75%; height:auto" src="./assets/ck-rx65n/connected2.png"/>

The reported measurement values have been highlighted – and changed values can
be seen in the terminal output.

## Viewing IoTConnect device measurements

### Measurements

#### temperature

The "temperature" measurement will be a temperature DECIMAL value in degrees Celcius.
This can be seen in the terminal output.

#### humidity

The "humidity" measurement will be a humidity DECIMAL value.
This can be seen in the terminal output.

#### version

The "version" measurement will be a constant "01.00.00" STRING. This can be
seen in the terminal output.

#### button

The "button" value will return false BOOLEAN value when the lower button is not
pressed. 

<img style="width:45%; height:auto" src="./assets/ck-rx65n/IMG_20230316_120246661-crop-user-button.jpg"/>

The "button" value will return true BOOLEAN value when the lower button is
pressed.

<img style="width:45%; height:auto" src="./assets/ck-rx65n/IMG_20230303_104024332-finger.jpg"/>

The "button" values can also be seen in the terminal output.

### Viewing IoTConnect device measurements on the IoTConnect Dashboard

Log into your IoTConnect account and open the appropriate Device page which can
be found on the [left side of the IoTConnect
dashboard](https://avnet.iotconnect.io/device/1).

The device page can be selected by using the "Devices" tab at the bottom of the
page.

The connected devices will be shown as a green "CONNECTED" status – may need to
refresh the page to get an up to date view.

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_13_52_56.png"/>

Click on the connected device name, to open its specific page

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_13_53_05.png"/>

### Live Data

Click on the "Live Data" item on the left of the device’s own page.

Note: obviously this requires the IoTConnect device to be connected and
actually sending data.  If the device has been previously connected, but is not
currently connected, then "Historical Data" should be used.

#### Telemetry

The "Telemetry" tab is a mirror of the data sent by the IoTConnect device – as
shown in the data element of the JSON shown in the terminal output from the
device.

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_17_20_28.png"/>

#### Graph

The "Graph" tab is a graph of numerical attribute values as sent by the
IoTConnect device over time.

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_13_55_48.png"/>

#### Tabular

The "Tabular" tab is a table of attributes values as sent by the IoTConnect
device.

<img style="width:75%; height:auto" src="./assets/ck-rx65n/VirtualBox_WinDev2301Eval_29_03_2023_17_20_39.png"/>

## Appendix: Direct flashing a build generated by e<sup>2</sup> studio

### Install Renesas Flash Programmer

Follow the [Renesas Flash Programmer (Programming GUI) installation
instructions](https://www.renesas.com/eu/en/software-tool/renesas-flash-programmer-programming-gui#overview).

### Use Renesas Flash Programmer

To flash the CK-RX65N board follow the [flashing
instructions](https://www.renesas.com/eu/en/document/mat/renesas-flash-programmer-v311-flash-memory-programming-software-users-manual?r=488871).
