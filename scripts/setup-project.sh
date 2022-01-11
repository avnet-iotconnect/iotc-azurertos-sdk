#!/bin/bash

set -e

name="${1}"
if [[ -z "$name" ]]; then
  echo "Usge: $0 <mimxrt1060|stm32l4|same54xpro|mimxrt10xx-package|nxpdemos>"
  exit 1
fi

case "$name" in
  stm32l4)
	pushd "$(dirname $0)"/../samples/"${name}"
	;;
  mimxrt1060)
    pushd "$(dirname $0)"/../samples/"${name}"
	;;
  same54xpro)
    pushd "$(dirname $0)"/../samples/"${name}"
	;;
  mimxrt10xx-package)
    pushd "$(dirname $0)"/../samples/"${name}"
	;;
  rt1060)
    pushd "$(dirname $0)"/../samples/"${name}"
	;;
  maaxboardrt)
    pushd "$(dirname $0)"/../samples/"${name}"
	;;
  lpc55s69)
    pushd "$(dirname $0)"/../samples/"${name}"
	;;
esac

# initial cleanup

rm -rf iotc-c-lib cJSON b-l4s5i-iot01a mimxrt1060 same54Xpro mimxrt10xx-package nxpdemos

# clone the dependency repos
git clone --depth 1 --branch v2.0.0 https://github.com/avnet-iotconnect/iotc-c-lib.git
git clone --depth 1 --branch v1.7.13 https://github.com/DaveGamble/cJSON.git

case "$name" in
  rt1060)
	mkdir -p iotc-azrtos-sdk
    mv ../nxpdemos/overlay/rt1060/iotconnectdemo .
	mv ../nxpdemos/overlay/rt1060/include .
    ;;
  maaxboardrt)
	mkdir -p iotc-azrtos-sdk
    mv ../nxpdemos/overlay/maaxboardrt/iotconnectdemo .
	mv ../nxpdemos/overlay/maaxboardrt/board .
	mv ../nxpdemos/overlay/maaxboardrt/include .
    ;;
  lpc55s69)
	mkdir -p iotc-azrtos-sdk
    mv ../nxpdemos/overlay/lpc55s69/iotconnectdemo .
	mv ../nxpdemos/overlay/lpc55s69/include .
    ;;
esac

# move only relevant files into corresponding locations
rm -rf iotc-azrtos-sdk/iotc-c-lib
rm -rf iotc-azrtos-sdk/cJSON
mkdir -p iotc-azrtos-sdk/iotc-c-lib
mkdir -p iotc-azrtos-sdk/cJSON
pushd iotc-azrtos-sdk/ >/dev/null
  for f in ../../../iotc-azrtos-sdk/*; do
    ln -sf $f .
  done
popd >/dev/null
mv iotc-c-lib/include iotc-c-lib/src iotc-azrtos-sdk/iotc-c-lib/
mv cJSON/cJSON.* iotc-azrtos-sdk/cJSON/
rm -rf iotc-c-lib cJSON

exit 0

echo Downloading Azure_RTOS_6...
case "$name" in
  stm32l4)
    wget -q -O azrtos.zip https://saleshosted.z13.web.core.windows.net/sdk/AzureRTOS/Azure_RTOS_6.1_STM32L4+-DISCO_STM32CubeIDE_Samples_2021_11_03.zip
    project_dir='b-l4s5i-iot01a/stm32cubeide'
    libs="stm32l4xx_lib "
    ;;
  mimxrt1060)
    wget -q -O azrtos.zip https://saleshosted.z13.web.core.windows.net/sdk/AzureRTOS/Azure_RTOS_6.1_MIMXRT1060_MCUXpresso_Samples_2020_10_10.zip
    project_dir='mimxrt1060/MCUXpresso/'
    libs="mimxrt1060_library filex "
    ;;
  same54xpro)
    #wget -q -O azrtos.zip https://github.com/azure-rtos/samples/releases/download/v6.1_rel/Azure_RTOS_6.1_ATSAME54-XPRO_MPLab_Samples_2020_10_10.zip
    wget -q -O azrtos.zip https://saleshosted.z13.web.core.windows.net/sdk/AzureRTOS/Azure_RTOS_6.1_ADU_ATSAME54-XPRO_MPLab_Sample_2021_03_02.zip
    project_dir='same54Xpro/mplab/'
    libs="same54_lib filex "
    ;;
  mimxrt10xx-package)
    popd >/dev/null #samples/"${name}"
    echo Done
    exit 0
    ;;
  nxpdemos)
	popd >/dev/null #samples/"${name}"
	echo Done
	exit 0
	;;
  *)
    echo Invalid platform $name.
    exit 3
    ;;
esac

case "$name" in
  mimxrt1060)
    echo Extracting...
    unzip -q azrtos.zip
    rm -f azrtos.zip
    ;;
  stm32l4)
    echo Extracting...
    unzip -q azrtos.zip
    rm -f azrtos.zip
    ;;
  same54xpro)
    echo Extracting...
    unzip -q azrtos.zip
    rm -f azrtos.zip
    ;;
esac

case "$name" in
  same54xpro)
    # only 1 level of inderection in the zip (unlike others). Shim here:
    mkdir same54Xpro
    mv mplab/ same54Xpro/.
    ;;
esac

# copy  only relevant directories into corresponding locations without overwriting
pushd "${project_dir}" >/dev/null
  for dir in threadx netxduo common_hardware_code $libs; do
    echo cp -nr $dir ../../
    cp -nr $dir ../../
  done
popd >/dev/null

rm -rf $(dirname "${project_dir}")

  echo 'Applying patches for AzureRTOS component directory name references...'
case "$name" in
  stm32l4)
echo 'Applying patches for AzureRTOS component directory name references...'
    sed -i '/stm32l475_lib/d' ./stm32l4xx_lib/.cproject
    sed -i 's#nxd#netxduo#g' ./netxduo/.cproject
    sed -i 's#tx#threadx#g' ./threadx/.cproject
    ;;
  mimxrt1060)
    echo 'Applying patches for AzureRTOS component directory name references...'
    sed -i 's#nxd#netxduo#g' ./netxduo/.cproject
    sed -i 's#tx#threadx#g' ./threadx/.cproject
    ;;
  same54xpro)
    # Microsoft's samples point the lib to the common_hardware_code from the sample, so we point it to ours
    sed -i 's#sample_azure_iot_embedded_sdk#basic-sample#g' ./same54_lib/nbproject/configurations.xml
    ;;
esac

popd >/dev/null #samples/"${name}"
echo Done
