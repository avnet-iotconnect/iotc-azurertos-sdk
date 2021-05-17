#!/bin/bash

set -e

name="${1}"
if [[ -z "$name" ]]; then
  echo "Usge: $0 <mimxrt1060|stm32l4>"
  exit 1
fi
pushd "$(dirname $0)"/../samples/"${name}"
# initial cleanup

rm -rf iotc-c-lib cJSON b-l4s5i-iot01a mimxrt1060

# prevent accidental commits on these files
# need to undo if, changes to these files are actually needed
git update-index --assume-unchanged basic-sample/include/app_config.h
git update-index --assume-unchanged basic-sample/src/sample_device_identity.c

# clone the dependency repos
git clone --depth 1 --branch v2.0.0 git://github.com/Avnet/iotc-c-lib.git
git clone --depth 1 --branch v1.7.13 git://github.com/DaveGamble/cJSON.git

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

echo Downloading Azure_RTOS_6...
case "$name" in
  stm32l4)
    wget -q -O azrtos.zip https://github.com/azure-rtos/samples/releases/download/v6.1_rel/Azure_RTOS_6.1_STM32L4+-DISCO_STM32CubeIDE_Samples_2020_10_10.zip
    project_dir='b-l4s5i-iot01a/stm32cubeide'
    libs="stm32l4xx_lib "
    ;;
  mimxrt1060)
    wget -q -O azrtos.zip https://github.com/azure-rtos/samples/releases/download/v6.1_rel/Azure_RTOS_6.1_MIMXRT1060_MCUXpresso_Samples_2020_10_10.zip
    project_dir='mimxrt1060/MCUXpresso/'
    libs="mimxrt1060_library filex "
    ;;
  *)
    echo Invalid platform $name.
    exit 3
    ;;
esac
echo Extracting...
unzip -q azrtos.zip
rm -f azrtos.zip

# copy  only relevant directories into corresponding locations without overwriting
pushd "${project_dir}" >/dev/null
  for dir in threadx netxduo common_hardware_code $libs; do
    echo cp -nr $dir ../../
    cp -nr $dir ../../
  done
  # prevent accidental commit of private information
  # export NO_ASSUME_UNCHANGED=yes to allow commits to these files
  if [[ -n "$NO_ASSUME_UNCHANGED" ]]; then
    git update-index --no-assume-unchanged basic-sample/src/sample_device_identity.c
    git update-index --no-assume-unchanged basic-sample/include/app_config.h
  else
    git update-index --assume-unchanged basic-sample/src/sample_device_identity.c
    git update-index --assume-unchanged basic-sample/include/app_config.h
  fi
popd >/dev/null

rm -rf $(dirname "${project_dir}")

echo 'Applying patches for AzureRTOS component directory name references...'

if [[ -f ./stm32l4xx_lib/.cproject ]]; then
  sed -i '/stm32l475_lib/d' ./stm32l4xx_lib/.cproject
fi
sed -i 's#nxd#netxduo#g' ./netxduo/.cproject
sed -i 's#tx#threadx#g' ./threadx/.cproject

popd >/dev/null #samples/"${name}"
echo Done
