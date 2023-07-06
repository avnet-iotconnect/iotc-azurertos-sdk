## Avnet MaaXBoard RT (and NXP i.MX RT1060 EVK)
Below are the setup instructions for the Avnet MaaXBoard RT and NXP i.MX RT1060 Evaluation Kit.

# Procedure
* Download and install [MCUXpresso IDE](https://www.nxp.com/design/software/development-software/mcuxpresso-software-and-tools-/mcuxpresso-integrated-development-environment-ide:MCUXpresso-IDE).
* File -> Open Projects from File System and navigate to one of the samples.
* Select all projects in the list.
* Click the top right IDE icon to switch to IDE view.
* Close the "Globals" window, next to the "Outline" in the top right dock as it can sometimes cause the IDE to hang during debugging and perspective switching.
* Select the bottom-middle dock "Installed SDKs" tab and install the MCUExpresso SDK for the board. Keep subdirectories selected, but unselect the actual mimxrt1060 sample directory.
*Note:  WiFi and Ethernet are not supported
