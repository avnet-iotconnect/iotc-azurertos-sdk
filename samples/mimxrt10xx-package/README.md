## About
Implement NXP MIMXRT10x0 boards with the iotc-azrtos-sdk.

## Project Setup Instructions
* Open the MCUXpresso and on the *Welcome* screen, click "Download and install SDKs".
* Select your board, then click the *Install* button.
* On the *Welcome* screen, click ```Import SDK Examples``` and click *Go straight to the Wizard*.
* Select your board, click *Next* and select "azure_iot_embedded_sdk" under the azure_rtos_examples and click *Next*.
* Set the standard library to *NewlibNano(semihost)* at the top of the screen and enable "Use floating point version of printf" and "Use floating point version of scanf". Then click *Finish*.
* Exclude the ```source``` folder from build or delete it from the project.
* Download the project package from this repository and unzip it. 
* Select *File -> Import... -> General -> Existing Projects into Workspace* and click *Next*.
  
* Browse to where the package was unzipped and *Select* that folder and ensure that *iotc-azrtos-sdk* is checked.

* Copy the *src* folder from Github into the project. You can drag the folder and drop it onto the project's root directory in the Project Explorer.
	
* Add ```path``` into the azure_iot_embedded_sdk project.
	Property -> C/C++ Build -> Settings -> MCU C Compiler -> Includes.
	{workspace_loc:/iotc-azrtos-sdk/include}
	{workspace_loc:/iotc-azrtos-sdk/iotc-c-lib/include}
	{workspace_loc:/iotc-azrtos-sdk/azrtos-layer/include}
	{workspace_loc:/iotc-azrtos-sdk/azrtos-layer/azrtos-ota/include}
	{workspace_loc:/${ProjName}/include}

* Add ```Library``` and Source location.
	Property -> C/C++ General -> Paths and Symbols -> Libraries:					Add "iotc-azrtos-sdk"
	Property -> C/C++ General -> Paths and Symbols -> Library Paths:				Add "/iotc-azrtos-sdk/Debug"
	Property -> C/C++ General -> Paths and Symbols -> Includes -> Assembly: 		Add "/${ProjName}/src"
	Property -> C/C++ General -> Paths and Symbols -> Source Location: 				Add "azure_iot_embedded_sdk/src"
	
* Set up the linker for using the library
	Property -> C/C++ Build -> Settings -> MCU C Linker -> Miscellaneous -> Other projects -> Add "${workspace_loc:/iotc-azrtos-sdk/Debug/libiotc-azrtos-sdk.a}"
	

	
	
	
	

