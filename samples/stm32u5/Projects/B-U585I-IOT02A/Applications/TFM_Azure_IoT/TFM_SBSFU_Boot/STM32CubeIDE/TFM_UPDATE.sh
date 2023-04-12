#!/bin/bash -
echo "TFM_UPDATE started"

SPEED_OPTS="${SPEED_OPTS:-speed=fast}"

_flash_all=1

while :
do
  case "$1" in
    --app_s )
      _app_s=1
      _flash_all=0
      shift 
      ;;
    --app_ns )
      _app_ns=1
      _flash_all=0
      shift 
      ;;
    --loader )
      _loader=1
      _flash_all=0
      shift 
      ;;
    --sbsfu )
      _sbsfu=1
      _flash_all=0
      shift 
      ;;
    *)
      shift;
      break
      ;;
  esac
done

# Absolute path to this script
SCRIPT=$(readlink -f $0)
# Absolute path this script
SCRIPTPATH=`dirname $SCRIPT`
PATH="/C/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/":$PATH
stm32programmercli="STM32_Programmer_CLI"
connect_no_reset="-c port=SWD mode=UR"
connect="-c port=SWD mode=UR $SPEED_OPTS --hardRst"
slot0=0xc028000
slot1=0xc056000
slot2=0xc0f8000
slot3=0xc126000
slot4=0xc026000
slot5=0xc0f6000
slot6=0xc1c6000
slot7=0xc1c8000
boot=0xc004000
loader=0xc1fa000

cfg_loader=1
app_image_number=2
s_data_image_number=1
ns_data_image_number=1

if [ $app_image_number == 2 ]; then
if [ $_flash_all -eq 1 ] || [ -n "$_app_s" ]; then
echo "Write TFM_Appli Secure"
$stm32programmercli $connect -d $SCRIPTPATH/../../TFM_Appli/Binary/tfm_s_app_init.bin $slot0 -v
ret=$?
if [ $ret != 0 ]; then
  if [ "$1" != "AUTO" ]; then read -p "TFM_UPDATE script failed, press a key" -n1 -s; fi
  exit 1
fi
echo "TFM_Appli Secure Written"
fi

if [ $_flash_all -eq 1 ] || [ -n ${_app_ns} ]; then
echo "Write TFM_Appli NonSecure"
$stm32programmercli $connect -d $SCRIPTPATH/../../TFM_Appli/Binary/tfm_ns_app_init.bin $slot1 -v
ret=$?
if [ $ret != 0 ]; then
  if [ "$1" != "AUTO" ]; then read -p "TFM_UPDATE script failed, press a key" -n1 -s; fi
  exit 1
fi
echo "TFM_Appli NonSecure Written"
fi
fi

if [ $_flash_all -eq 1 ] && [ $app_image_number == 1 ]; then
echo "Write TFM_Appli"
$stm32programmercli $connect -d $SCRIPTPATH/../../TFM_Appli/Binary/tfm_app_init.bin $slot0 -v
ret=$?
if [ $ret != 0 ]; then
  if [ "$1" != "AUTO" ]; then read -p "TFM_UPDATE script failed, press a key" -n1 -s; fi
  exit 1
fi
echo "TFM_Appli Written"
fi

if [ $_flash_all -eq 1 ] && [ $s_data_image_number == 1 ]; then
echo "Write TFM_Appli Secure Data"
if [ ! -e $SCRIPTPATH/../../TFM_Appli/Binary/tfm_s_data_init.bin ]; then
    if [ "$1" != "AUTO" ]; then read -p "Error : sbsfu_s_data_init.bin does not exist! Run dataimg.bat script to generate it" -n1 -s; fi
    exit 1
fi
$stm32programmercli $connect -d $SCRIPTPATH/../../TFM_Appli/Binary/tfm_s_data_init.bin $slot4 -v
ret=$?
if [ $ret != 0 ]; then
  if [ "$1" != "AUTO" ]; then read -p "TFM_UPDATE script failed, press a key" -n1 -s; fi
  exit 1
fi
echo "TFM_Appli Secure Data Written"
fi

if [ $_flash_all -eq 1 ] && [ $ns_data_image_number == 1 ]; then
echo "Write TFM_Appli NonSecure Data"
if [ ! -e $SCRIPTPATH/../../TFM_Appli/Binary/tfm_ns_data_init.bin ]; then
    if [ "$1" != "AUTO" ]; then read -p "Error : sbsfu_ns_data_init.bin does not exist! Run dataimg.bat script to generate it" -n1 -s; fi
    exit 1
fi
$stm32programmercli $connect -d $SCRIPTPATH/../../TFM_Appli/Binary/tfm_ns_data_init.bin $slot5 -v
ret=$?
if [ $ret != 0 ]; then
  if [ "$1" != "AUTO" ]; then read -p "TFM_UPDATE script failed, press a key" -n1 -s; fi
  exit 1
fi
echo "TFM NonSecure Data Written"
fi

# write loader if config loader is active
if [ $_flash_all -eq 1 ] && [ $cfg_loader == 1 ]; then
echo "Write TFM_Loader"
$stm32programmercli $connect -d $SCRIPTPATH/../../TFM_Loader/Binary/loader.bin $loader -v
ret=$?
if [ $ret != 0 ]; then
  if [ "$1" != "AUTO" ]; then read -p "TFM_UPDATE script failed, press a key" -n1 -s; fi
  exit 1
fi
echo "TFM_Loader Written"
fi


if [ $_flash_all -eq 1 ]; then
echo "Write TFM_SBSFU_Boot"
$stm32programmercli $connect -d $SCRIPTPATH/../Binary/bootloader.bin $boot -v
ret=$?
if [ $ret != 0 ]; then
  if [ "$1" != "AUTO" ]; then read -p "TFM_UPDATE script failed, press a key" -n1 -s; fi
  exit 1
fi
echo "TFM_SBSFU_Boot Written"
fi

if [ "$1" != "AUTO" ]; then 
   read -p "TFM_UPDATE script Done, press a key" -n1 -s; fi
exit 0
