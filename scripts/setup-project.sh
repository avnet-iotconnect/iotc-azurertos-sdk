#!/bin/bash

set -e

# All the IDEs offically support Windows, so it is likely that a developer
# will be running it. This script can execute fine on WSL, however symlinks 
# are not working correctly there. Detect if we are running on Windows and invoke
# the appropriate program to create the symlink.
make_sym_link() {
  if [ $(uname -r | grep "Microsoft") ];
  then
        cmd.exe /c "mklink /J "${2//\//\\}" "${1//\//\\}
  else
        ln -sf $1 $2
  fi
}

# The "." operator does not work with mklink, instead
# get the filename from the directory path e.g. 
# ../../../iotc-azrtos-sdk\authentication -> authentication
get_file_name() {
  echo $1 | rev | cut -d '/' -f1 | rev
}

legacy_threadx_setup() {
  name = $1
  case "$name" in
  maaxboardrt)
  #symlink iotc-azrtos-sdk into project
      echo Downloading MaaxXBoardRT baseline project packages...
      wget -q -O azrtos.zip https://saleshosted.z13.web.core.windows.net/sdk/AzureRTOS/Azure_RTOS_6.1_MIMXRT1060_MCUXpresso_Samples_2021_11_03.zip
      azrtos_dir='mimxrt1060/MCUXpresso/'
      wget -q -O project.zip https://saleshosted.z13.web.core.windows.net/sdk/AzureRTOS/evkmimxrt1170_azure_iot_embedded_sdk.zip
      project_dir=evkmimxrt1170_azure_iot_embedded_sdk/

      echo Extracting...
      unzip -q azrtos.zip
      unzip -q project.zip

      rm -rf ${project_dir}/azure-rtos/binary/netxduo
      rm -rf ${project_dir}/azure-rtos/netxduo
      rm -rf ${project_dir}/azure_iot
      rm -rf ${project_dir}/drivers

      rm -f azrtos.zip
      rm -f project.zip

      #copy original NXP project and netxduo lib into project
      cp -nr ${project_dir}/* basic-sample
      cp -nr ${azrtos_dir}/netxduo/* netxduo

      rm -rf $(dirname "${azrtos_dir}")
      rm -rf ${project_dir}

      ;;
    stm32l4)
      echo Downloading Azure_RTOS_6...
      wget -q -O azrtos.zip https://saleshosted.z13.web.core.windows.net/sdk/AzureRTOS/Azure_RTOS_6.1_STM32L4+-DISCO_STM32CubeIDE_Samples_2021_11_03.zip
      project_target_dir='b-l4s5i-iot01a/'
      project_ide_dir='stm32cubeide/'
      libs="stm32l4xx_lib common_hardware_code "
      ;;
    mimxrt1060)
      echo Downloading Azure_RTOS_6...
      wget -q -O azrtos.zip https://saleshosted.z13.web.core.windows.net/sdk/AzureRTOS/Azure_RTOS_6.1_MIMXRT1060_MCUXpresso_Samples_2021_11_03.zip
      project_target_dir='mimxrt1060/'
      project_ide_dir='MCUXpresso/'
      libs="mimxrt1060_library filex common_hardware_code "
      ;;
    same54xpro)
      echo Downloading Azure_RTOS_6...
      #wget -q -O azrtos.zip https://github.com/azure-rtos/samples/releases/download/v6.1_rel/Azure_RTOS_6.1_ATSAME54-XPRO_MPLab_Samples_2020_10_10.zip
      #wget -q -O azrtos.zip https://saleshosted.z13.web.core.windows.net/sdk/AzureRTOS/Azure_RTOS_6.1_ADU_ATSAME54-XPRO_MPLab_Sample_2021_03_02.zip
      #wget -q -O azrtos.zip https://github.com/azure-rtos/samples/releases/download/v6.1_rel/Azure_RTOS_6.1_ATSAME54-XPRO_MPLab_Samples_2021_11_03.zip
      wget -q -O azrtos.zip https://saleshosted.z13.web.core.windows.net/sdk/AzureRTOS/Azure_RTOS_6.1_ATSAME54-XPRO_MPLab_Samples_2021_11_03.zip
      project_target_dir='same54Xpro/'
      project_ide_dir='mplab/'
      libs="same54_lib filex common_hardware_code "
      ;;
    rx65ncloudkit)
      echo Downloading Azure_RTOS_6...
      wget -q -O azrtos.zip https://github.com/azure-rtos/samples/releases/download/v6.1_rel/Azure_RTOS_6.1_RX65N_Cloud_Kit_E2Studio_GNURX_Samples_2022_05_25.zip
      project_target_dir=''
      project_ide_dir='e2studio_gnurx/'
      libs="filex netxduo_addons "
      ;;
    *)
      popd >/dev/null
      echo Done
      exit 0
      ;;
  esac

  project_dir="${project_target_dir}${project_ide_dir}"

  case "$name" in
    mimxrt1060 | stm32l4 | same54xpro | rx65ncloudkit)
      echo Extracting...
      unzip -q azrtos.zip
      rm -f azrtos.zip
      ;;
  esac


  # copy  only relevant directories into corresponding locations without overwriting
  pushd "${project_dir}" >/dev/null
    for dir in threadx netxduo $libs; do
      echo cp -nr $dir $sample_dir
      cp -nr $dir $sample_dir
  #	rsync -av --exclude 'nbproject' $dir ../../
    done
  popd >/dev/null

  rm -rf ${project_dir}

    echo 'Applying patches for AzureRTOS component directory name references...'
  case "$name" in
    stm32l4)
  echo 'Applying patches for AzureRTOS component directory name references...'
      sed -i '/stm32l475_lib/d' ./stm32l4xx_lib/.cproject
      sed -i 's#nxd#netxduo#g' ./netxduo/.cproject
      sed -i 's#tx#threadx#g' ./threadx/.cproject
      ;;
    mimxrt1060 | rx65ncloudkit)
      echo 'Applying patches for AzureRTOS component directory name references...'
      sed -i 's#nxd#netxduo#g' ./netxduo/.cproject
      sed -i 's#tx#threadx#g' ./threadx/.cproject
      ;;
  esac
}

link_directories() {
  for f in $1/*; do
    if [ -d $f ]; then
      make_sym_link $f $(get_file_name $f)
    fi
  done
}

create_threadx_sym_links() {
  pushd threadx/ >/dev/null
  link_directories ../../../os/threadx
  popd >/dev/null

  pushd netxduo/ >/dev/null
  link_directories ../../../os/netxduo
  popd >/dev/null

  pushd filex/ >/dev/null
  link_directories ../../../os/filex
  popd >/dev/null
}

# prevent accidental commit of private information by default
# export NO_ASSUME_UNCHANGED=yes to allow commits to these files
git_hide_config_files() {
  if [[ -n "$NO_ASSUME_UNCHANGED" ]]; then
      git update-index --no-assume-unchanged basic-sample/src/sample_device_identity.c
      git update-index --no-assume-unchanged basic-sample/include/app_config.h
  else
    git update-index --assume-unchanged basic-sample/src/sample_device_identity.c
    git update-index --assume-unchanged basic-sample/include/app_config.h
  fi
}

name="${1}"
if [[ -z "$name" ]]; then
  echo "Usage: $0 <project_name>"
  echo "Available projects: stm32l4, mimxrt1060, same54xpro, " \
      "maaxboardrt, rx65ncloudkit"
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
  same54xprov2)
    pushd "$(dirname $0)"/../samples/"${name}"
	;;
  maaxboardrt)
    pushd "$(dirname $0)"/../samples/"${name}"
    ;;
  rx65ncloudkit)
    pushd "$(dirname $0)"/../samples/"${name}"
	;;
esac

sample_dir=$(pwd)

# initial cleanup

rm -rf iotc-c-lib cJSON libTO b-l4s5i-iot01a mimxrt1060 same54Xpro stm32l4 maaxboardrt rx65ncloudkit

case "$name" in
  same54xprov2)
    pushd iotc-azrtos-sdk/ >/dev/null
      for f in ../../../iotc-azrtos-sdk/*; do
        ln -sf $f .
      done
    popd >/dev/null

    rm -rf ATSAME54-XPRO
    git clone https://github.com/Microchip-Azure-Demos/ATSAME54-XPRO.git
    cd ATSAME54-XPRO
    git reset --hard b1431c287f2d3eb60c4ef4463abcf367257d0b16
    cd ..
    cp -nr  ATSAME54-XPRO/firmware/src .
    rm -rf ATSAME54-XPRO

     ;;

  maaxboardrt)
#symlink iotc-azrtos-sdk into project
    rm -rf basic-sample/iotc-azrtos-sdk
    mv iotc-azrtos-sdk basic-sample/
    pushd basic-sample/iotc-azrtos-sdk/ >/dev/null
      for f in ../../../../iotc-azrtos-sdk/*; do
        ln -sf $f .
      done
    popd >/dev/null

    echo Downloading MaaxXBoardRT baseline project packages...
    wget -q -O azrtos.zip https://saleshosted.z13.web.core.windows.net/sdk/AzureRTOS/Azure_RTOS_6.1_MIMXRT1060_MCUXpresso_Samples_2021_11_03.zip
    azrtos_dir='mimxrt1060/MCUXpresso/'
    wget -q -O project.zip https://saleshosted.z13.web.core.windows.net/sdk/AzureRTOS/evkmimxrt1170_azure_iot_embedded_sdk.zip
    project_dir=evkmimxrt1170_azure_iot_embedded_sdk/

    echo Extracting...
    unzip -q azrtos.zip
    unzip -q project.zip

    rm -rf ${project_dir}/azure-rtos/binary/netxduo
    rm -rf ${project_dir}/azure-rtos/netxduo
    rm -rf ${project_dir}/azure_iot
    rm -rf ${project_dir}/drivers

    rm -f azrtos.zip
    rm -f project.zip

    #copy original NXP project and netxduo lib into project
    cp -nr ${project_dir}/* basic-sample
    cp -nr ${azrtos_dir}/netxduo/* netxduo

    rm -rf $(dirname "${azrtos_dir}")
    rm -rf ${project_dir}

    git_hide_config_files
    ;;
  stm32l4 | mimxrt1060 | same54xpro | rx65ncloudkit)
    pushd iotc-azrtos-sdk/ >/dev/null
      for f in ../../../iotc-azrtos-sdk/*; do
        make_sym_link $f $(get_file_name $f)
      done
    popd >/dev/null
    git_hide_config_files
    ;;
  *)
    echo "Unknown project name $name"
    exit -1
esac

case "$name" in
  stm32l4 | mimxrt1060 | same54xpro)
    legacy_threadx_setup $name
    ;;
  rx65ncloudkit)
    create_threadx_sym_links
    ;;
  *)
    popd >/dev/null
  	echo Done
    exit 0
    ;;
esac

popd >/dev/null #samples/"${name}"
echo Done
