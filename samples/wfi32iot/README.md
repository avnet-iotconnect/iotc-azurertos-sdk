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
Even though each WFI32E01 Development Board has its own on-board light and temperature sensors, additional sensors can optionally be added relatively quickly using existing off-the-shelf hardware. 

The WFI32E01 Development Board, like many Microchip development boards, features a 16-pin (2 rows x 8 pins) expansion socket which conforms to the [mikroBUS™ specification](https://download.mikroe.com/documents/standards/mikrobus/mikroBUS-standard.pdf). The mikroBUS™ standard defines mainboard sockets used for interfacing microcontrollers or microprocessors (mainboards) with integrated circuits and peripheral modules (add-on boards).

The standard specifies the physical layout of the mikroBUS™ pinout, the communication and power supply pins used, the positioning of the mikroBUS™ socket on the mainboard, and finally, the silkscreen marking conventions for both the sockets. The purpose of mikroBUS™ is to enable easy hardware expandability with a large number of standardized compact add-on boards, each one carrying a single sensor, transceiver, display, encoder, motor driver, connection port, or any other electronic module or integrated circuit. Created by [MikroElektronika](https://www.mikroe.com), mikroBUS™ is an open standard — anyone can implement mikroBUS™ in their hardware design.

<img src=".//media/image8.png" style="width:4in;height:5in"/>

MikroElektronika manufactures hundreds of ["Click" boards](https://www.mikroe.com/click) which conform to the mikroBUS™ standard. This demonstration currently supports the optional addition of up to 9 MikroElektronika Click boards which feature various sensors.
<br>

### "Altitude 2" Click Board

[Altitude 2 Click](https://www.mikroe.com/altitude-2-click) is a high-resolution barometric pressure sensor add-on Click board™. It carries the MS5607, a barometric pressure sensor IC with the stainless steel cap, produced by TE Connectivity. This sensor provides very accurate measurements of temperature and atmospheric pressure, which can be used to calculate the altitude with a very high resolution of 20cm per step. Besides that, the Besides that, the device also includes features such as the ultra-low noise delta-sigma 24bit ADC, low power consumption, fast conversion times, pre-programmed unique compensation values, and more. Low count of external components requirement, along with the simple interface which requires no extensive configuration programming, makes this sensor very attractive for building altitude or air pressure measuring applications.

<img src=".//media/clicks/Altitude2.png" style="width:2in;height:2.8in"/>

<br>

### "PHT" Click Board

[PHT Click](https://www.mikroe.com/pht-click) is a compact add-on board that contains a PHT combo sensor. This board features the MS8607, a digital combination sensor providing 3 environmental measurements all-in-one: pressure, humidity, and temperature from TE Connectivity. This sensor is based on leading MEMS technologies, provides factory-calibrated PHT data available over an I2C serial interface. The standout feature of the MS8607, alongside its very respectable low power consumption at as low as 0.78 µA, is also ultra-low power consumption and high PHT accuracy. This Click board™ is appropriate for environmental monitoring, as well as PHT applications such as HVAC and weather stations.

<img src=".//media/clicks/PHT.png" style="width:2in;height:2.8in"/>

<br>

### "TEMPHUM14" Click Board

[TEMPHUM14 Click](https://www.mikroe.com/temphum-14-click) is a compact add-on board that contains one of the smallest and most accurate humidity and temperature sensors on the market. This board features the HTU31D, a highly accurate digital relative humidity sensor with temperature output from TE Connectivity. With power consumption down to 3.78μW and accuracy of ±2%RH and ±0.2°C, this Click board™ provides fast response time, precision measurement, low hysteresis, and sustained performance even when exposed to extreme temperature up to 125°C and humidity environments. This Click board™ is suitable for relative humidity and temperature measuring applications, including weather stations, reliable monitoring systems, and more.

<img src=".//media/clicks/TempHum14.png" style="width:2in;height:2.8in"/>

<br>

### "Ultra-Low Press" Click Board

[Ultra-Low Press Click](https://www.mikroe.com/ultra-low-press-click) is a compact add-on board that contains a mountable gage pressure sensor for pneumatic pressure measurements. This board features the SM8436, an I2C configurable ultra-low pressure sensor with high accuracy and long-term stability from Silicon Microstructure (part of TE Connectivity). A state-of-the-art MEMS pressure transducer technology and CMOS mixed-signal processing technology produces a digital, fully conditioned, multi-order pressure and temperature compensated sensor like this available in a gage pressure configuration. It also features superior sensitivity needed for ultra-low pressure measurements ranging from 0 to 250Pa Differential / 500 Pa Gauge. Therefore, this Click board™ is suitable for differential pressure measurements found in pressure monitoring applications like building fire safety systems, isolation rooms, and high purity work stations as well as positive pressure solutions found in hospital surgical environments.

<img src=".//media/clicks/ULP.png" style="width:2in;height:3.5in"/>

<br>

### "VAV Press" Click Board

[VAV Press Click](https://www.mikroe.com/vav-press-click) is a compact add-on board that contains a board-mount pressure sensor. This board features the LMIS025B, a low differential pressure sensor from First Sensor (part of TE Connectivity). It is based on thermal flow measurement of gas through a micro-flow channel integrated within the sensor chip. The innovative LMI technology features superior sensitivity, especially for ultra-low pressures ranging from 0 to 25 Pa. The extremely low gas flow through the sensor ensures high immunity to dust contamination, humidity, and long tubing compared to other flow-based pressure sensors. This Click board™ is suitable for pressure measurements in Variable Air Volume (VAV) building ventilation systems, industrial, and respiratory applications in medical.

<img src=".//media/clicks/VAV.png" style="width:2in;height:3.5in"/>

<br>

### Stacking Click Boards

Any combination of up to 4 of these Click boards can be connected to the WFI32E01 Development Board at the same time using the MikroElektronika [Shuttle Bundle](https://www.mikroe.com/mikrobus-shuttle-bundle) accessory kit. The bundle features the [Shuttle click](https://www.mikroe.com/shuttle-click) 4-socket expansion board, which provides an easy and elegant solution for stacking up to four Click boards™ onto a single mikroBUS™ socket. It is a perfect solution for expanding the capacity of the development system with additional mikroBUS™ sockets when there is a demand for using more Click boards™ than the used development system is able to support.

<img src=".//media/image10a.png">

<img src=".//media/image10b.png">

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

