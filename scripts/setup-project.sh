#!/bin/bash

set -e

# All the IDEs offically support Windows, so it is likely that a developer
# will be running it. This script can execute fine on WSL, however symlinks 
# are not working correctly there. Detect if we are running on Windows and invoke
# the appropriate program to create the symlink.
make_sym_link() {
  if [ $(uname -r | grep "Microsoft") ];
  then
    if [ -d $1 ]; then
      # Create a Windows directory junction
      cmd.exe /c "mklink /J "${2//\//\\}" "${1//\//\\}
    else
      # Create a Windows file hardlink
      cmd.exe /c "mklink /h "${2//\//\\}" "${1//\//\\}
    fi
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

# Create symbolic links from the contents of the source directory ($1) to the current
# working directory.
link_directories() {
  for f in $1/*; do
    target=$(get_file_name $f)

    # If the file to a link is a directory and already exists within the destination,
    # enter the destination directory and symlink its children that aren't present. 
    # Else, just create the symlink at this directory.
    if [ -d $f ]; then
      if [ -d $target ]; then
        pushd $target >/dev/null
        link_directories ../$f
        popd >/dev/null
      else
        make_sym_link $f $target
      fi
    # If the file to link is a regular file and doesn't exist in the destination,
    # create the link. Avoiding linking pre-existing files allows for custom files 
    # to be added per-project.
    elif [ -f $f ]; then
      if [ ! -f $target ]; then
        make_sym_link $f $target
      fi
    fi
  done
}

# Symlink the contents of iotc-azrtos-sdk into the current working
# directory (e.g. the azure sdk eclipse project)
create_iotc_azure_sdk_sym_links() {
  pushd iotc-azrtos-sdk/ >/dev/null
  link_directories ../../../iotc-azrtos-sdk
  popd >/dev/null
}

# Symlink the contents of iotc-azrtos-sdk into the current working
# directory (e.g. the azure sdk eclipse project)
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

# Prevent accidental commit of private information by default
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

show_help() {
  echo "Usage: setup-project.sh <project_name>"
  echo "Available projects: stm32l4, mimxrt1060, same54xpro, " \
      "same54xprov2, maaxboardrt, rx65ncloudkit"
}

# If the project does not currently have a project file that supports Git submodules
# for the iotc-azure-sdk dependencies, it can use this function to pull the nessesary resources
# directly from the Github repos.
legacy_iotc_deps_setup() {
  # clone the dependency repos
  git clone --depth 1 --branch v2.0.4 https://github.com/avnet-iotconnect/iotc-c-lib.git
  git clone --depth 1 --branch v1.7.13 https://github.com/DaveGamble/cJSON.git
  git clone --depth 1 --branch main https://github.com/TrustedObjects/libTO.git
  pushd libTO
  git checkout 2217696249de310b15b17b1a262422de9b3a7d04 #as of April 2022
  popd

  # move only relevant files into corresponding locations
  rm -rf iotc-azrtos-sdk/iotc-c-lib
  rm -rf iotc-azrtos-sdk/cJSON
  rm -rf iotc-azrtos-sdk/libTO

  mkdir -p iotc-azrtos-sdk/iotc-c-lib
  mkdir -p iotc-azrtos-sdk/cJSON
  mkdir -p iotc-azrtos-sdk/libTO

  mv iotc-c-lib/include iotc-c-lib/src iotc-azrtos-sdk/iotc-c-lib/
  mv cJSON/cJSON.* iotc-azrtos-sdk/cJSON/
  mv libTO/Sources/*  iotc-azrtos-sdk/libTO
  # remove libTO examples:
  rm -rf iotc-azrtos-sdk/libTO/examples
  rm -rf iotc-azrtos-sdk/libTO/src/wrapper

  rm -rf iotc-c-lib cJSON libTO
}

# If the project does not currently have a project file that supports the Git submodules
# for threadx components, it can use this function to pull the nessesary resources 
# from the pre-packaged zip files.
legacy_threadx_setup() {
  name = $1
  case "$name" in
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
      wget -q -O azrtos.zip https://saleshosted.z13.web.core.windows.net/sdk/AzureRTOS/Azure_RTOS_6.1_RX65N_Cloud_Kit_E2Studio_GNURX_Samples_2022_05_25.zip
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

legacy_maaxboardrt_setup() {
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
}


#################################################################################################
# Script start
#################################################################################################
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

# Perform setup steps for the required project
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
    legacy_iotc_deps_setup
    legacy_maaxboardrt_setup
    git_hide_config_files
    ;;
  stm32l4 | mimxrt1060 | same54xpro)
    legacy_iotc_deps_setup
    create_iotc_azure_sdk_sym_links
    legacy_threadx_setup $name
    git_hide_config_files
    ;;
  rx65ncloudkit)
    git submodule update --init --recursive
    create_iotc_azure_sdk_sym_links
    create_threadx_sym_links
    git_hide_config_files
    ;;
  *)
    popd >/dev/null
    echo "No implemented build steps for: $name"
    exit 1
esac

popd >/dev/null #samples/"${name}"
echo Done
