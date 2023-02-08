#!/bin/bash

set -e

board=$1
fw_filename=$2
fw_version=$3
fw_provider=$4
fw_name=$5
dev_manufacturer=$6
dev_model=$7

this_dir=$(dirname "$0")

function usage {
  echo "Usage: $0 <same54xpro> <fw_filename_path> <fw_version> <fw_provider> <fw_name> [[device manufacturer] [device model]]" >&2
  echo "If either the device model and manufacturer are not provided, values will be set according to bard." >&2
  echo "If both the device model and manufacturer are provided the board parameter will be ignored." >&2
  echo "Example:" >&2
  echo "    "$(dirname "$0")"/adu-generate-package.sh same54xpro ./fw1.1.0.bin 1.3.0 AVNET SAMPLEAPP" >&2
}

if [[ -z "${board}" || -z "${fw_filename}" || -z "${fw_version}" || -z "${fw_provider}" ]]; then
  usage
  exit 1
fi

if [[ -z "${dev_manufacturer}" && -z "${dev_model}" ]]; then
  if [[ "${board}" == "same54xpro" ]]; then
    dev_manufacturer="MICROCHIP"
    dev_model="SAME54"
  else
    usage
    exit 2
  fi
fi

if [[ -z "${fw_filename}" ]]; then
  usage
  exit 3
fi

if [[ ! -f "${fw_filename}" ]]; then
  usage
  exit 4
fi

td="${fw_provider}-${fw_name}-${fw_version}"
tf="${td}/${fw_provider}-${fw_name}-${fw_version}.importmanifest.json"
sha256=$(sha256sum -b "${fw_filename}" | cut -f1 -d' ' | xxd -r -p - | base64)
file_size=$(du -b "${fw_filename}" | cut -f1)
created_date=$(date -u +"%Y-%m-%dT%H:%M:%SZ")
mkdir -p "${td}"
cp -f "$this_dir/adu-import-manifest-template.json" "${tf}"
cp -f "${fw_filename}" "${td}"
sed -i "s#@@@FW_PROVIDER@@@#${fw_provider}#g" "${tf}"
sed -i "s#@@@FW_NAME@@@#${fw_name}#g" "${tf}"
sed -i "s#@@@FW_VERSION@@@#${fw_version}#g" "${tf}"
sed -i "s#@@@DEVICE_MANUFACTURER@@@#${dev_manufacturer}#g" "${tf}"
sed -i "s#@@@DEVICE_MODEL@@@#${dev_model}#g" "${tf}"
sed -i "s#@@@FW_FILENAME@@@#$(basename ${fw_filename})#g" "${tf}"
sed -i "s#@@@FW_FILE_SIZE@@@#${file_size}#g" "${tf}"
sed -i "s#@@@FW_SHA256@@@#${sha256}#g" "${tf}"
sed -i "s#@@@CREATED_DATE@@@#${created_date}#g" "${tf}"

echo Done



