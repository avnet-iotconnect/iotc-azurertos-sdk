## About
Implement NXP boards with the iotc-azrtos-sdk.

## Project Setup Instructions
* Open the MCUXpresso IDE and on the *Welcome* screen, click "Download and install SDKs".
* Select your board, then click the *Install* button.
* On the *Welcome* screen, click ```Import SDK Examples``` and click *Go straight to the Wizard*.
* Select your board, click *Next* and select "azure_iot_embedded_sdk" under the azure_rtos_examples and click *Next*.
* Set the standard library to *NewlibNano(semihost)* at the top of the screen and enable "Use floating point version of printf" and "Use floating point version of scanf". Then click *Finish*.
* Download the project [Releases](https://github.com/avnet-iotconnect/iotc-azurertos-sdk/releases) from this repository's Releases page and unzip it.
* Select *File -> Import... -> General -> File System* and click *Next*.
* Browse to where the package was unzipped and *Select* the folders below, then click *Finish*
	* ```iotc-azrtos-sdk```
	* ```iotconnectdemo```
	* ```include```
	* ```board``` if exists
* Add ```include path``` :
	* *Property -> C/C++ Build -> Settings -> MCU C Compiler -> Includes:*
	* *"${workspace_loc:/${ProjName}/iotc-azrtos-sdk/azrtos-layer/include}"*
	* *"${workspace_loc:/${ProjName}/iotc-azrtos-sdk/azrtos-layer/azrtos-ota/include}"*
	* *"${workspace_loc:/${ProjName}/iotc-azrtos-sdk/include}"*
	* *"${workspace_loc:/${ProjName}/iotc-azrtos-sdk/iotc-c-lib/include}"*
	* *"${workspace_loc:/${ProjName}/iotc-azrtos-sdk/cJSON}"*
	* *"${workspace_loc:/${ProjName}/iotconnectdemo/include}"*
* Add ```preprocessors``` :
	* *Property -> C/C++ Build -> Settings -> MCU C Compiler -> Preprocessor:*
	* *NX_WEB_HTTPS_ENABLE*
	* *IOTC_NEEDS_GETTIMEOFDAY*
* Exclude the ```source``` folder from build :
	* Right click on the *source* folder -> *Resource Configurations* -> *Exclude from build* -> check both *Debug* and *Release* -> click *OK*
* Adjust heap size if needed:
	* *Right click on the project -> C/C++ Build -> Settings -> MCU C++ Linker -> Manager Linker Script -> Change Heap size from Default to 0x2000*

	
	
	
	

