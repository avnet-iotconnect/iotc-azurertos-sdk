# Renesas Build Setup (DRAFT DO NOT RELEASE)

Created by Neil Matthews
Last updated: Feb 21, 2023
21 min read

Current document describes setting up `#00ff00` green rx65ncloudkit board.
![green rx65ncloudkit board](/assets/IMG_20230213_113723358-small-crop.jpg)

# KNOWN FIXES REQUIRED

If need a version for the `#0000ff` blue ck-rx65n board then need to change references in the text to rx65ncloudkit AND also change screenshots showing the rx65ncloudkit project in e<sup>2</sup> studio.

Need to redact any IoTConnect screens showing Avnet or Witekio parameters – ideally using screenshots from a fresh account.

Codebase in `iotc-azurertos-sdk` misspells "symmetric" and "asymmetric" which makes this guide look odd.

`DUID_PREFIX` isn’t used in `iotconnect_app.c` (provided `IOTCONNECT_DUID` is set) – seems odd to expose this value to a user in `app_config.h` – should hide this somehow? Note: had to read the code to work out what is does!

Do we need to expose the version of IoTConnect protocol? Is this meaningful when setting up a template, or a device? Can website "just" have some preselected "default" / "latest" version checked. Just trying to hide another option from a user.

Using symmetric key in this startup guide for simplicity.

Need to hide IoTHub documentation within the `iotc-azurertos-sdk` git submodules it downloads – otherwise it gets confusing, when searching for information.

Tooltip for a device using symmetric keys has same text for Primary Key and Secondary Key.

Help pages on [help.iotconnect.io](https://help.iotconnect.io) give more complicated results first – if want to create a device, probably don’t want to know about gateway devices (first).
No help / (useful) results for "symmetric key".

The version information is really, really unclear – the `iotc-c-lib` seems to be implementing version 2.0 (in discovery) – but the IoTConnect dashboard only lists 2.1

The attributes being sent by the rx65ncloudkit board are unclear without searching through the source code.

Can’t get the base64 encoding for the Symmetric Key to be accepted when creating a device – tried both under Windows and Ubuntu – in case of line-ending issues.

This document is intended to introduce the use of the Renesas RX65N Cloud Kit with IoTConnect.

- Please find information on [setup and direct integration with Azure IoT Hub here](). 
- Please find information on [setup and direct integration with AWS IoT here](https://www.renesas.com/us/en/document/mat/getting-started-edit-and-write-programs-rx65n-cloud-kit).

## Prerequisites:

- Windows PC (this setup guide details installation, etc. on a Windows PC; a Linux setup might be setup in an analogous manner).
- Internet connection via a 2.4GHz WiFi access point with known SSID (for the Renesas board’s internet connection)
- Plastic tweezers (optional)

# Download and install latest Renesas GCC

You will need to register to download the latest GCC release.
At time of writing the URL is: [Download Latest Toolchains | Open Source Tools for Renesas]()

Please consult Renesas company documentation on installing the latest GCC release.

# Download and install latest e<sup>2</sup> studio

You will need to register to download the latest e<sup>2</sup> studio release.
At time of writing the URL is: [e<sup>2</sup> studio -information for RX Family | Renesas]()

Please consult Renesas company documentation on installing the latest e<sup>2</sup> studio.

The following options are required for Cloudkit development: RX support, and optionally Jlink debugging, terminal, and git support.

# Run/setup e<sup>2</sup> studio

You will need to use the your Renesas account information that was used to download the latest e<sup>2</sup> studio release when setting up/registering e<sup>2</sup> studio.

# Optional: Update e<sup>2</sup> studio to latest versions

"Check for Updates" – can be a little slow.

# Optional: setup egit in e<sup>2</sup> studio

If using egit support in e<sup>2</sup> studio then follow Renesas notes, e.g. Integrated Development Environment e<sup>2</sup> studio `r20an0523ee0101-e2studio-egit.pdf`.

# Using git clone to access `iotc-azurertos-sdk`

The IotConnect Azure RTOS SDK URL is: 

## Using e<sup>2</sup> studio to git clone `iotc-azurertos-sdk`

## Install "Git for Windows" and clone `iotc-azurertos-sdk`

Alternatively, install "git for Windows", currently URL is: Git for Windows

And use the bash shell to clone the repository – right-click on directory - "Other options", then "Git Bash" to launch the shell.

## Windows Subsystem for Linux (WSL)

Alternatively, install Windows Subsystem for Linux (WSL) and clone the repository

## Cygwin

Alternatively, install [Cygwin](https://www.cygwin.com) - ensure that git, unzip and wget or curl are installed - and clone the repository.

# Setup the Renesas `iotc-azurertos-sdk` project fully

The setup process requires a `bash`-like shell (not `cmd`) – could use Windows Subsystem for Linux (WSL), MinGW shell from "git for Windows", or Cygwin. The shell needs access to:

- `git`
- `wget`, or `curl`
- `unzip`

To setup the `iotc-azurertos-sdk` project fully for Renesas

- cd `iotc-azurertos-sdk/scripts`
- run the setup-project.sh script

# Import `iotc-azurertos-sdk` rx65ncloudkit project into e<sup>2</sup> studio

Need to import the samples/rx65ncloudkit project into e<sup>2</sup> studio to build and debug - note deselect any "CMake Project" items, so that the r65ncloudkit (and its subprojects) are imported as Eclipse projects.

Once the project is imported into e<sup>2</sup> studio, then check that the latest version of the compiler is used for rx65ncloudkit project and each of the subprobjects.

Expand the rx65ncloudkit project.

Right click on "basic-sample" and bring up the C/C++ properties sheet. Ensure that the toolchain being used is the latest one – if not, then select the latest one, and "Apply and Close".
 
# Update the RX Driver Package (FIT modules)

Expand `basic-sample` and the `src` subdirectory, so that `basic-sample.scfg` can be seen in the "Project Explorer" pane.

Double-click on `basic-sample.scfg` to bring up the "Overview Information".

Download any FIT modules.

Press the "Generate Code" button at the top-right corner of the "Project Explorer" pane.

Missing components (in the component tab) are shown as grey, clicking on them will suggest a download.

# Clean and build the project

Right-click on the basic-sample item under the rx65ncloudkit project in the "Project Explorer" and select "Clean Project" in case any previous build artefacts are present. 

Right-click on the basic-sample item in the "Project Explorer" and select "Build Project". 

# Build products

Build product (basic-sample.elf, basic-sample.mot, etc.) is generated by default under the rx65ncloudkit/basic-sample directory in the HardwareDebug sub-directory.

# Rebuild with specific values

Examine the file `basic-sample/include/app_config.h`  and fill in the empty strings "" to match your IotConnect setup. And rebuild.

	//
	// Copyright: Avnet 2022
	// Created by Russell Bateman <rbateman@witekio.com> on 11/18/22.
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
	
	#define WIFI_SSID ""
	#define WIFI_PASSWORD ""
	
	#endif // APP_CONFIG_H
	# Setup the RX65N Cloud Kit

Follow the instructions in 

Connect the two USB-micro leads:

The power LED should be lit. The power LED is at the top left hand corner (in the diagram above) of the upper (cloud option) board – above the mini-USB port on the lower (target) board.
 
## Setup a terminal connection

Install termite (Termite Terminal. Alternative Serial Terminal utility for Windows), Tera Term (), or similar.

The terminal settings to connect are - chose appropriate COM port: 

- 115200 baud rate
- 8-bit data
- No parity

Note: In Linux the terminal may enumerate as /dev/ttyACM0.

# Running/debugging a build generated by e<sup>2</sup> studio

Warning this will overwrite any software flashed/supplied previously on the RX65N Cloud Kit board.

Need to remove the large jumper on the lower (target) board before running/debugging a build; obviously, remove any USB leads to power off the board – and a set of plastic tweezers may be useful.

The jumper is set on the left-hand picture, and "removed" (only using one pin – just to avoid losing it).

## Create a debug .launch file

If a .launch file is not present in the basic-sample directory, e.g launch file may be called basic-sample HardwareDebug.launch , then you will need to create a suitable .launch file.

## Debug code

Press the "Launch in ‘Debug’ mode" button ("bug") on the left-side of e<sup>2</sup> studio. If there are errors, check that the board is connected via USB, and that the jumper has been removed (once USB is disconnected).

Respond appropriately to any pop-ups relating to Windows Defender Firewall prompts.

Switch to the debug perspective when prompted.

The build will have two default breakpoints already enabled, so presume the Resume button (green "left-pointing triangle") in the middle of e<sup>2</sup> studio twice to enable to code to execute past those two breakpoints. 

Note: once the first break-point is passed then a red LED will light at the bottom left of the rx65ncloudkit board.

Output should be visible in any attached terminal now.

When finished press the Terminate button (red "square") next to the Resume button.

To debug again, press the debug button.

To return to C/C++ development, select the "Window / Perspective / Open perspective / C/C++ project".

To return to debugging/running, select the "Window / Perspective / Open perspective / Debug".

### Developing with e<sup>2</sup> studio

May need to select the "Refresh" option by right-clicking on a project in e<sup>2</sup> studio if/when the git branch has been changed, or other "external" changes have been made.

Also if a git pull modifies any git submodules, then will likely need to rerun setup-project.sh.

### Setting SSID and Password in `app_config.h`

Without editting `app_config.h` the code will produce the following output:

In e<sup>2</sup> studio edit the `app_config.h` to specify the SSID of a 2.4 GHz WiFi network and its password, then the code will produce output similar to that below (the "Wi-Fi Network" will display the SSID specified in `app_config.h`):

### Aside:

If the board is powered-down and the jumper replaced, it seems that the board will be flashed to run the newly "debugged" code, replacing previous code.

# Create a sequence of 16-64 random bytes that are Base64 encoded - for use as a Symmetric Key

To create a random symmetric "key" we will generate a random 32 byte sequence and then Base64 encode it – so that the binary values can be shared easily. 

Run this command in a shell, e.g. in Git for Windows Bash, Cygwin, etc.:
	dd if=/dev/urandom bs=32 count=1 status=none | base64

A symmetric key is used to both encode and decode - so the same key is used on the IoT device and on the IoTConnect server.

Make a note of this output, as it will be used to setup the IoTConnect template for the device on the IoTConnect dashboard – and in the application configuration file `app_config.h` that will be compiled/built and run on the device. This is important as isn’t possible to recover key information once added to the IoTConnect dashboard – also consider if key information might get removed from any local `iotc-azurertos-sdk` git repository (if it gets cleaned, branches are deleted, or changes get reverted). Obviously, key information shouldn’t be pushed to external public repos.

In this case the symmetric key is:
	YzlgdRbYcreYW1fhjwxO4b3X7hBlDY3OVuw6q9wDbAo=

# IoTConnect template and device

For further information, please consult the "Knowledge Base" which can be found on the left side of the IoTConnect dashboard, or via the following URL: 

## Create IoTConnect template

Log into your IoTConnect account and open the appropriate Device page which can be found on the [left side of the IoTConnect dashboard](https://avnet.iotconnect.io/device/1).

The template page can be selected by using the  "Templates" tab at the bottom of the page.

Press "Create Template" at the top right corner of the page to create a new template.

When creating the template choose a unique name for the "Template Code" (which is a maximum of 10 characters and must start with a letter) and a useful description in the "Template Name".
Choose "Symmetric Key" for the "Authentication Type" and choose "2.1" for the "Device Message Version" (in fact, the rx65ncloudkit uses `iotc-c-lib` which implements version 2.0).

When the template is saved, we can add attributes – "measurements" (and types) – for values that will be sent by the rx65ncloudkit board.

Click "Attributes (0)" and add a version "STRING" value and "Save" it.

Add a "button" attribute with BOOLEAN type, and a "random" attribute with DECIMAL type.

Note: in the rx65ncloudkit basic-sample the attribute values are sent using `iotcl_telemetry_set_xxx()` style commands.

Make a note of the template name, as this will be used as the "Template" to create the IoTConnect device in the next step – but it will be visible in the drop-down list.

## Create IoTConnect device

Log into your IoTConnect account and open the appropriate Device page which can be found on the [left side of the IoTConnect dashboard](https://avnet.iotconnect.io/device/1).

The device page can be selected by using the  "Devices" tab at the bottom of the page.

- Chose a "Unique Id" for your device – this will be use in `app_config.h` built/compiled in the device later.
- Chose a "Display Name" for your device.
- Select an appropriate "Entity" – this is a pre-populated menu item.
- Select the "Template" display name that was created in the previous step – this will require filling in extra fields "Primary Key" and "Secondary Key".
- Use the symmetric key that was generated previously, including any trailing "=" signs, e.g. `YzlgdRbYcreYW1fhjwxO4b3X7hBlDY3OVuw6q9wDbAo=` as the value for the "Primary Key" and the "Secondary Key".

## WARNING!!! this generated Symmetric Key didn’t work I’ve had to use Russell’s value cGFzc3dvcmRwYXNzd29yZAo=

Values I generated by dd and encoded weren't accepted as valid base64 encodings on the IoTConnect create a device page -- note the dd command is borrowed from `stm32u5/QUICKSTART_DEMO.md`

When I created the template I pasted the Russell’s same value in both the "Primary Key" and the "Secondary Key" and included the trailing "=" equals sign.

# Updating `app_config.h`

In `app_config.h` you will need to specify:

	//
	// Copyright: Avnet 2022
	// Created by Russell Bateman <rbateman@witekio.com> on 11/18/22.
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
	
	#define WIFI_SSID ""
	#define WIFI_PASSWORD ""
	
	#endif // APP_CONFIG_H

After updating `app_config.h`, then in e<sup>2</sup> studio: refresh, clean and build the basic-sample in the rx65ncloudkit, so that any modifications are incorporated into the binary -- before debugging/running, or flashing the board.

## `WIFI_SSID` and `WIFI_PASSWORD`

See previous section and fill in your 2.4 GHz WiFi access point SSID and password.

## `SAMPLE_SNTP_SERVER_NAME`

Choose a suitable public NTP time server, e.g. "time.google.com".

## `IOTCONNECT_CPID` and `IOTCONNECT_ENV`

Log into your IoTConnect account and open the Key Vault which can be found on the [left side of the IoTConnect dashboard](https://avnet.iotconnect.io/key-vault).

### `IOTCONNECT_CPID`

This is the value under "CPID:" at the top left of the "Home / Setting / Key Vault" screen.

### `IOTCONNECT_ENV`

This is the value under "Environment:" at the top middle of the "Home / Setting / Key Vault" screen.

## `IOTCONNECT_DUID` and `DUID_PREFIX`

Log into your IoTConnect account and open the appropriate page for the device that you created previously which can be found on the [left side of the IoTConnect dashboard](https://avnet.iotconnect.io/device/1).

### `IOTCONNECT_DUID`

This is the "Unique ID" for the IoTConnect device that you previously created.

### `DUID_PREFIX`

This can be ignored provided that IOTCONNECT_DUID has some meaningful defined value.

## `IOTCONNECT_SYMETRIC_KEY`

Paste your previously created Base64 encoded symmetric key.

# Running the rx65ncloudkit board with customised `app_config.h`

After saving/refreshing/rebuilding and initialising the debug session in e<sup>2</sup> studio, example output is shown on the left in the terminal (Tera Term VT):

The reported measurement values have been highlighted – and changed values can be seen in the terminal output.

# Viewing IoTConnect device measurements

## Measurements

### random

The "random" measurement will be a random DECIMAL value in the range 0-100. This can be seen in the terminal output.

### version

The "version" measurement will be a constant "01.00.00" STRING. This can be seen in the terminal output.

### button

The "button" value will return false BOOLEAN value when the lower button is not pressed. This can be seen in the terminal output.

The "button" value will return true BOOLEAN value when the lower button is pressed.

## Viewing IoTConnect device measurements on the IoTConnect Dashboard

Log into your IoTConnect account and open the appropriate Device page which can be found on the [left side of the IoTConnect dashboard](https://avnet.iotconnect.io/device/1).

The device page can be selected by using the  "Devices" tab at the bottom of the page.

The connected devices will be shown as a green "CONNECTED" status – may need to refresh the page to get an up to date view.

Click on the connected device name, to open its specific page

## Live Data

Click on the "Live Data" item on the left of the device’s own page. 

Note: obviously this requires the IoTConnect device to be connected and actually sending data. If the device has been previously connected, but is not currently connected, then "Historical Data" should be used.

### Telemetry

The "Telemetry" tab is a mirror of the data sent by the IoTConnect device – as shown in the data element of the JSON shown in the terminal output from the device.

### Graph

The "Graph" tab is a graph of numerical attribute values as sent by the IoTConnect device over time.

### Tabular

The "Tabular" tab is a table of attributes values as sent by the IoTConnect device.

# Appendix: Direct flashing a build generated by e<sup>2</sup> studio

Follow the instructions in: https://www.renesas.com/eu/en/document/mat/renesas-flash-programmer-v311-flash-memory-programming-software-users-manual?r=488871.

## Install Renesas Flash Programmer

Follow the instructions in Renesas Flash Programmer (Programming GUI) \| Renesas.

# Renesas example sensor code

There is example Renesas sensor code for RX65N at the following URL:  under "Buy/Sample".

There is also a Renesas github page: .

# Appendix: "Blue" RX65N board:

"Blue" RX65N board has a users-manual at the following URL: https://www.renesas.com/tw/en/document/mat/ck-rx65n-v1-users-manual.
