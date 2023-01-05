#!/bin/bash

set -e

show_help() {
  echo "Usage: $0 <project_name>"
  echo "Available projects: stm32l4, mimxrt1060, same54xpro, " \
      "same54xprov2, maaxboardrt, rx65ncloudkit, wfi32iot"
}

# All the IDEs officially support Windows, so it is likely that a developer
# will be running it. This script can execute fine on WSL, however symlinks 
# are not working correctly there. Detect if we are running on Windows and invoke
# the appropriate program to create the symlink.
make_sym_link() {
  if [ $(uname -r | grep "Microsoft") ] || [ $(uname -o | grep "Msys") ]; then
    # Create a Windows directory junction, or Windows file hardlink
    link_type=$([[ -d $1 ]] && echo "/J" || echo "/h")
    cmd.exe /c "mklink $link_type "${2//\//\\}" "${1//\//\\}
  else
    relative_source_loc=$(realpath --relative-to="$(dirname ${2})" ${1})
    ln -sf ${relative_source_loc} ${2}
  fi
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

create_source_file_symlinks() {
  for f in $1/*.{c,h}; do
    target=$(echo "$2/"$(basename $f))
    make_sym_link $f $target
  done
}

# Symlink the contents of iotc-azrtos-sdk into the current working
# directory (e.g. the azure sdk eclipse project)
create_iotc_azrtos_symlinks() {
  source_dir="${1:-../../../iotc-azrtos-sdk/}"
  target_dir=${2:-iotc-azrtos-sdk/}

  # Make sure tha the script is idempotent
  rm -rf $target_dir
  mkdir -p $target_dir

  pushd $target_dir >/dev/null
  for f in $source_dir/*; do
    target=$(basename $f)
    # Don't link everything under libTO and cJSON, we can handle that later
    if [[ $target != "libTO" && $target != "cJSON" ]]; then
      make_sym_link $f $target
    fi
  done

  # Link LibTO aside from the examples and wrapper files
  mkdir -p libTO/Sources/src
  make_sym_link $source_dir/libTO/Sources/include libTO/Sources/include
  make_sym_link $source_dir/libTO/Sources/src/aes-gcm-sw libTO/Sources/src/aes-gcm-sw
  create_source_file_symlinks $source_dir/libTO/Sources/src/ libTO/Sources/src/

  # Only link the core source files for cJSON, skip the tests, cmake config etc.
  mkdir -p cJSON
  create_source_file_symlinks $source_dir/cJSON cJSON

  popd >/dev/null
}

create_threadx_project_maxxboard() {
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
}

create_threadx_projects() {
  name=$1
  case "$name" in
    stm32l4)
      echo Downloading Azure_RTOS_6...
      wget -q -O azrtos.zip https://saleshosted.z13.web.core.windows.net/sdk/AzureRTOS/Azure_RTOS_6.1_STM32L4+-DISCO_STM32CubeIDE_Samples_2021_11_03.zip
      project_platform_dir='b-l4s5i-iot01a/'
      project_ide_dir='stm32cubeide/'
      libs="stm32l4xx_lib common_hardware_code "
      ;;
    mimxrt1060)
      echo Downloading Azure_RTOS_6...
      wget -q -O azrtos.zip https://saleshosted.z13.web.core.windows.net/sdk/AzureRTOS/Azure_RTOS_6.1_MIMXRT1060_MCUXpresso_Samples_2021_11_03.zip
      project_platform_dir='mimxrt1060/'
      project_ide_dir='MCUXpresso/'
      libs="mimxrt1060_library filex common_hardware_code "
      ;;
    same54xpro)
      echo Downloading Azure_RTOS_6...
      #wget -q -O azrtos.zip https://github.com/azure-rtos/samples/releases/download/v6.1_rel/Azure_RTOS_6.1_ATSAME54-XPRO_MPLab_Samples_2020_10_10.zip
      #wget -q -O azrtos.zip https://saleshosted.z13.web.core.windows.net/sdk/AzureRTOS/Azure_RTOS_6.1_ADU_ATSAME54-XPRO_MPLab_Sample_2021_03_02.zip
      #wget -q -O azrtos.zip https://github.com/azure-rtos/samples/releases/download/v6.1_rel/Azure_RTOS_6.1_ATSAME54-XPRO_MPLab_Samples_2021_11_03.zip
      wget -q -O azrtos.zip https://saleshosted.z13.web.core.windows.net/sdk/AzureRTOS/Azure_RTOS_6.1_ATSAME54-XPRO_MPLab_Samples_2021_11_03.zip
      project_platform_dir='same54Xpro/'
      project_ide_dir='mplab/'
      libs="same54_lib filex common_hardware_code "
      ;;
    rx65ncloudkit)
      echo Downloading Azure_RTOS_6...
      wget -q -O azrtos.zip https://saleshosted.z13.web.core.windows.net/sdk/AzureRTOS/Azure_RTOS_6.1_RX65N_Cloud_Kit_E2Studio_GNURX_Samples_2022_05_25.zip
      project_platform_dir=''
      project_ide_dir='e2studio_gnurx/'
      libs="filex netxduo_addons "
      ;;
    *)
      popd >/dev/null
      echo Done
      exit 0
      ;;
  esac

  project_dir="${project_platform_dir}${project_ide_dir}"

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

  # Delete the extracted zip archive
  rm -rf ${project_ide_dir}
  rm -rf ${project_platform_dir}

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

name="${1}"
if [[ -z "$name" ]]; then
  show_help
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
  wfi32iot)
    pushd "$(dirname $0)"/../samples/"${name}"
	;;
  maaxboardrt)
    pushd "$(dirname $0)"/../samples/"${name}"
  ;;
  rx65ncloudkit)
    pushd "$(dirname $0)"/../samples/"${name}"
	;;
  *)
    show_help
    exit 1
  ;;
esac

sample_dir=$(pwd)

# initial cleanup
rm -rf iotc-c-lib cJSON libTO b-l4s5i-iot01a mimxrt1060 same54Xpro stm32l4 maaxboardrt rx65ncloudkit

# Pull Git submodules for iotc-azrtos-sdk
git submodule update --init --recursive

case "$name" in
  same54xprov2)
    create_iotc_azrtos_symlinks
    rm -rf AzureDemo_ATSAME54-XPRO
    git clone https://github.com/MicrochipTech/AzureDemo_ATSAME54-XPRO.git
    cd AzureDemo_ATSAME54-XPRO
    git reset --hard b1431c287f2d3eb60c4ef4463abcf367257d0b16
    cd ..
    cp -nr  AzureDemo_ATSAME54-XPRO/firmware/src .
    rm -rf AzureDemo_ATSAME54-XPRO/
  ;;
  wfi32iot)
    create_iotc_azrtos_symlinks
    rm -rf AzureDemo_WFI32E01
    git clone https://github.com/MicrochipTech/AzureDemo_WFI32E01.git
    cd AzureDemo_WFI32E01
    git reset --hard v0.9.1
    cd ..
    cp -nr  AzureDemo_WFI32E01/firmware/src .
    rm -rf AzureDemo_WFI32E01
    ;;
  maaxboardrt)
    create_iotc_azrtos_symlinks ../../../../iotc-azrtos-sdk/ basic-sample/iotc-azrtos-sdk/
    create_threadx_project_maxxboard
    git_hide_config_files
  ;;
  stm32l4 | mimxrt1060 | same54xpro | rx65ncloudkit)
    create_iotc_azrtos_symlinks
    create_threadx_projects $name
    git_hide_config_files
  ;;
  *)
    echo "Unknown project name $name"
    exit -1
esac



popd >/dev/null #samples/"${name}"
echo Done
