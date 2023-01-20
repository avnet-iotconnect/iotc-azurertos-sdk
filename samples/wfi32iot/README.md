## Introduction
This document describes how to connect the WFI32-IoT Development Board (featuring the fully certified, highly integrated WFI32E01PC wireless module) to Avnet IoTConnect built on Microsoft Azure Cloud services, including Azure IoT Hub, and leverages Azure RTOS to enable better experiences of embedded firmware development for Cloud applications.

<img src=".//media/iotc-wfi32.png" />

## Table of Contents
- [Introduction](#introduction)
- [Adding Extra Sensors to the WFI32 IoT Board](#adding-extra-sensors-to-the-wfi32-iot-board)
- [Program the WFI32-IoT Development Board](#program-the-wfi32-iot-development-board)
- [References](#references)
- [Conclusion](#conclusion)

### WFI32-IoT Development Board Overview & Features (SMART \| CONNECTED \| SECURE)

<img src=".//media/wfi32-iot-board.png"/>
Download the [WFI32-IoT Development Board User Guide](https://www.microchip.com/content/dam/mchp/documents/WSG/ProductDocuments/UserGuides/EV36W50A-WFI32-IoT-Board-Users-Guide-DS50003262.pdf) for more details including the schematics for the board (but do not follow the setup procedure in the document)

## Adding Extra Sensors to the WFI32 IoT Board
Even though each WFI32E01 Development Board has its own on-board light and temperature sensors, additional sensors can optionally be added relatively quickly using existing off-the-shelf hardware.  [Click here](https://github.com/MicrochipTech/AzureDemo_WFI32E01/blob/main/Clicks.md) for more details on how to add these sensors!

## Program the WFI32-IoT Development Board
Please refer to instructions Microchip [here](https://github.com/MicrochipTech/AzureDemo_WFI32E01#program-the-wfi32e01-development-board)

## References

Refer to the following links for additional information for IoT Explorer, IoT Hub, DPS, Plug and Play model, and IoT Central

•	[IoTConnect overview](https://help.iotconnect.io/knowledgebase/iotconnect-overview/)

•	[IotConnect quick start guide](https://help.iotconnect.io/knowledgebase/quick-start/)

•	[Onboard a device with IoTConnect](https://help.iotconnect.io/knowledgebase/device-onboarding/)

•	[Configure to connect to IoT Hub](https://docs.microsoft.com/en-us/azure/iot-pnp/quickstart-connect-device-c)

•	[Avnet IoTConnect - All Documentation](https://help.iotconnect.io/)

•	[How to connect devices with X.509 certificates for IoTConnect](https://help.iotconnect.io/knowledgebase/x-509-self-singed-certificate/)

•	[Create a new dashboard using the dynamic dashboard feature](https://help.iotconnect.io/documentation/dashboard/create-a-new-dashboard/)

## Conclusion

You are now able to connect WFI32-IoT to Avnet IoTConnect and should have deeper knowledge of how all the pieces of the puzzle fit together between Microchip's hardware and Microsoft's Azure Cloud services. Let’s start thinking out of the box and see how you can apply this project to provision securely and quickly a massive number of Microchip devices to Azure and safely manage them through the entire product life cycle.

