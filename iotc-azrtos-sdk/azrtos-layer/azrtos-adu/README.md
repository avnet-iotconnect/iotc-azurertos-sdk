## ADU Requirements
In order to make use of ADU support, the baseline Azure RTOS project mush have ADU support built-in.

The code from this directory should be included in the project and compile-time define
IOTC_ENABLE_ADU_SUPPORT should be added to the project that is building the ADU files.
The supported projects already enable this define.

## Firmware Package Setup

In order to provide the project firmware, the .bin file must be used and a
Device Update manifest must be created.

The adu-generate-package.sh script in the scripts directory of this git repository.

First, the binary (.bin file) should be obtained. 
This is done automatically for other supported projects, but the steps in the 
section below should be followed for the SAM E54 Xplained Pro board.

Modify basic-sample/src/iotconnect_app.c per desired application package parameters:

```C
#define APP_VERSION "1.0.0"
// ... Once the device connects For SAM E54 Xplained Pro:
        iothub_start_device_agent(
            IOTC_ADU_MICROCHIP,
            IOTC_ADU_SAME54,
            "AVNET",
            "SAMPLEAPP",
            APP_VERSION
            );


```

Compile the binary and execute the adu-generate-package.sh with matching parameters, for example:
```shell
cp samples/same54xpro/basic-sample/dist/default/production/basic-sample.production.bin basic-sample-1.1.0.bin
scripts/adu-generate-package.sh same54xpro basic-sample-1.1.0.bin 1.1.0 AVNET SAMPLEAPP
```

A directory will be created called AVNET-SAMPLEAPP-1.1.0 that will contain
the binary and the manifest. Those should be supplied to the 

Note that IOTC_ADU_MICROCHIP and IOTC_ADU_SAME54 are defines already available in azrtos_adu_agent.c and these values are
automatically set in the package manifest when passing the *ame54xpro* value as the first argument 
to the adu-generate-package.sh script. You should use the values corresponding to your board and manifacturer
when using other boards.

# SAM E54 Xplained Pro Binary File Setup
Follow the steps below to obtain the samples/same54xpro/basic-sample/dist/default/production/basic-sample.production.bin
file using the MPLAB X IDE: 
* Right-Click the basic-sample project and select *Set as Main Project*
* Right-Click the basic-sample project and select *Properties*
* In the *Building* section ensure that the *Execute this line after the build* is checked
* Select *Build Main Project* from the *Production* main menu item.
* Note that only production build can generate the binary file and building 
the debug build with *Execute this line after the build* enabled will fail, so this 
checkbox needs to be disabled when building a debug build.

 




