# Instructions

* Download and extract the appropriate project package from the [Releases](https://github.com/avnet-iotconnect/iotc-azurertos-sdk/releases) page.
* Alternatively, clone this repository then execute the setup-project.sh. On Windows, you will need bash installed with MSYS2 or similar.
* Download and install STM32CubeIDE.
* File->Open Projects form File System, navigate to one of the samples, and select all projects in the list.
* This board contains a sensors-demo in addition to the basic-sample. 
  The basic-sample project contains a simple example how to send data to IoTConnect.
  The sensor-demo contains integration with sensors from a very limited port 
  of the STM32CubeExpansion_Cloud_AZURE_V1.2.1 BSP.
* When running/debugging, choose one of the demos form the project list first. Keep subdirectories selected, but unselect the actual stm32l4 sample directory.
