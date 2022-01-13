## About
Implement NXP MCUXpresso SDK projects with the iotc-azrtos-sdk library to connect to Avnet IoTConnect Platform. The supported boards are listed below:
* **MaaxBoard RT**
* **MIMXRT1060 EVK**
* **LPC55S69**
	
## Project Setup Instructions
* Open the MCUXpresso IDE and on the **Welcome** screen, click **"Download and install SDKs"**.
* Select your board, then click the **Install** button.
* On the **Welcome** screen, click **Import SDK Examples** and click **Go straight to the Wizard**.
* Select your board, click **Next** and select ```azure_iot_embedded_sdk``` under the azure_rtos_examples and click **Next**.
* Set the standard library to **NewlibNano(semihost)** on MIMXRT1060 and LPC55S69, **NewlibNano(nohost)** on Maaxboard RT 
  and at the top of the screen and enable **"Use floating point 
  version of printf"** and **"Use floating point version of scanf"**. Then click **Finish**.
* Download the project package for your board from this repository's [Releases](https://github.com/avnet-iotconnect/iotc-azurertos-sdk/releases) 
  page and unzip it.
* Select the project in project explorer and right click *Import... -> General -> File System* and click **Next**.
* Browse to the last level of the unzipped package folder and choose it. **Check** the folders below, then click **Finish**. 
  Click **Yes to all** when prompted to overwrite files.
	* ```iotc-azrtos-sdk```
	* ```iotconnectdemo```
	* ```include```
	* ```board``` if exists
	* ```phy``` if exists
	* ```xip``` if exists
* Add **include path**:
  *Property -> C/C++ Build -> Settings -> MCU C Compiler -> Includes:*
	* *"${workspace_loc:/${ProjName}/iotc-azrtos-sdk/azrtos-layer/include}"*
	* *"${workspace_loc:/${ProjName}/iotc-azrtos-sdk/azrtos-layer/azrtos-ota/include}"*
	* *"${workspace_loc:/${ProjName}/iotc-azrtos-sdk/include}"*
	* *"${workspace_loc:/${ProjName}/iotc-azrtos-sdk/iotc-c-lib/include}"*
	* *"${workspace_loc:/${ProjName}/iotc-azrtos-sdk/cJSON}"*
	* *"${workspace_loc:/${ProjName}/include}"*
* Add **preprocessors**:
  *Property -> C/C++ Build -> Settings -> MCU C Compiler -> Preprocessor:*
	* *NX_WEB_HTTPS_ENABLE*
	* *IOTC_NEEDS_GETTIMEOFDAY*
* Add **Source Location**:
	*Property -> C/C++ General -> Paths and Symbols -> Source Location tab -> Add the folders below*
	* *iotc-azrtos-sdk*
	* *iotconnectdemo*
* Exclude the **source** folder from build:
	* Right click on the *source* folder -> *Resource Configurations* -> *Exclude from build* -> check both *"Debug"* and *"Release"* -> click **OK**
* Adjust heap size
	* *Right click on the project -> C/C++ Build -> Settings -> MCU C++ Linker -> Manager Linker Script -> Change Heap size from Default to 0x2000*
* If using Maaxboard RT, change flash configuration:
	* *Right click on the project -> C/C++ Build -> MCU settings -> Change Flash Driver from MIMXRT1170_SFDP_QSPI.cfx to ${workspace_loc:}/${ProjName}/xip/MaaXBoard_S26KS256.cfx*

	
	
	
	

