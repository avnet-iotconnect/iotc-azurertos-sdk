#
# Device Update for IoT Hub
# Sample PowerShell script for creating import manifests and payloads files for testing purposes.
# Copyright (c) Microsoft Corporation.
#

#Requires -Version 5.0

<#
    .SYNOPSIS
        Create a sample updates for testing purposes.

    .EXAMPLE
        PS > CreateSTM32L4S5HostUpdate-2.0.0.ps1 -UpdateProvider "STMicroelectronics" `
                                                 -UpdateName "STM32L4S5" `
                                                 -DeviceManufacturer "STMicroelectronics" `
                                                 -DeviceModel "STM32L4S5"`
                                                 -OutputManifestPath .
#>
[CmdletBinding()]
Param(

    # Update Provider
    [ValidateNotNullOrEmpty()]
    [string] $UpdateProvider = "RENESAS",

    # Update name
    [ValidateNotNullOrEmpty()]
    [string] $UpdateName = "RX65N",
    
    # Device Manufacturer
    [ValidateNotNullOrEmpty()]
    [string] $DeviceManufacturer = "RENESAS",

    # Device Model
    [ValidateNotNullOrEmpty()]
    [string] $DeviceModel = "RX65N",
    
    # Output Manifest Path 
    [ValidateNotNullOrEmpty()]
    [string] $OutputManifestPath = '.'
)


# ---------------------------------------------------------------
# Update
# Create the parent update with good Manifest file.
# ---------------------------------------------------------------
Import-Module ..\AduUpdate.psm1

$UpdateVersion = "1.1.0"
    
$parentCompat = New-AduUpdateCompatibility -DeviceManufacturer $DeviceManufacturer -DeviceModel $DeviceModel
$parentUpdateId = New-AduUpdateId -Provider $UpdateProvider -Name $UpdateName -Version $UpdateVersion
$parentUpdateIdStr = "$($parentUpdateId.Provider).$($parentUpdateId.Name).$($parentUpdateId.Version)"

$parentFirmwareFile = "$PSScriptRoot\firmware_$UpdateVersion.bin"  

$parentSteps = @()

Write-Host "Preparing parent update $UpdateVersion ..."
   
    # -----------
    # ADD STEP(S)

    # Step 1 - Host update that installs firmware package.
    $parentSteps += New-AduInstallationStep `
                            -Handler 'microsoft/swupdate:1' `
                            -Files $parentFirmwareFile `
                            -HandlerProperties @{ 'installedCriteria'="$DeviceManufacturer-$DeviceModel-$UpdateVersion"}

# ------------------------------
# Create parent update manifest
# ------------------------------

Write-Host "    Generating an import manifest $parentUpdateIdStr..."
$payloadFiles = $parentFirmwareFile

$parentManifest = New-AduImportManifest -UpdateId $parentUpdateId `
                                    -IsDeployable $true `
                                    -Compatibility $parentCompat `
                                    -InstallationSteps $parentSteps `
                                    -ErrorAction Stop

# Create folder for manifest files and payload.
$outputPath = "$OutputManifestPath\$parentUpdateIdStr"
Write-Host "    Saving parent manifest file and payload(s) to $outputPath..."
New-Item $outputPath -ItemType Directory -ErrorAction SilentlyContinue | Out-Null

# Generate manifest file.
$parentManifest | Out-File "$outputPath\$parentUpdateIdStr.importmanifest.json" -Encoding utf8

# Copy all payloads (if used)
Copy-Item -Path $payloadFiles -Destination $outputPath -Force

Write-Host " "