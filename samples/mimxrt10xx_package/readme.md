## About
Implement NXP MIMXRT10x0 boards with the iotc-azrtos-sdk.

## Project Setup Instructions
* Open the MCUXpresso and "Download and install SDKs"
* Select your board on the left screen and Add "Azure RTOS" on the bottom right screen, then install.
* In bottom left window, Click import ```SDK example```, select the board, and choose "azure_iot_embedded_sdk" under the azure_rtos_examples.
* Set the standard library to NewlibNano(semihost) and enable "Use floating point version of printf" and "Use floating point version of scanf". Then click Finish.
* Exclude the ```source``` folder from build or delete it in the project.
* Import the iotc-azrtos-sdk project into the workspace as a library.
	Copy the iotc-azrtos-sdk into the workspace.
	File -> Import -> General -> Existing Project into Workspace.
	Select your workspace as the root directory and select the iotc-azrtos-sdk project.
* Copy the "src" folder from Github into the project. You can drag the folder and drop it onto the project's root directory in the Project Explorer.
	
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
	

	
	
	
	

