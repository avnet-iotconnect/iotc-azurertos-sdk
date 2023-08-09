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
        PS > CreateSTM32L4S5HostLeafUpdate-3.0.0.ps1 -UpdateProvider "STMicroelectronics" `
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

##############################################################
# Update 
#
# Create a parent update that updates 'leaf1', 'leaf2'
#     - Each child update contains 1 inline step.
##############################################################

Import-Module ..\AduUpdate.psm1

$UpdateVersion = '4.0.0'

$parentCompat = New-AduUpdateCompatibility -DeviceManufacturer $DeviceManufacturer -DeviceModel $DeviceModel
$parentUpdateId = New-AduUpdateId -Provider $UpdateProvider -Name $UpdateName -Version $UpdateVersion
$parentUpdateIdStr = "$($parentUpdateId.Provider).$($parentUpdateId.Name).$($parentUpdateId.Version).Leaf"

$RefUpdateNamePrefix = $UpdateName

Write-Host "Preparing update $parentUpdateIdStr ..."

    # -------------------------------------------------
    # Create a child update for all 'leaf' components
    # -------------------------------------------------

    $RefUpdateManufacturer = "RENESAS"
    $RefUpdateName = "$RefUpdateNamePrefix-Leaf"
    $RefUpdateVersion = "3.0.0"

    Write-Host "Preparing child update ($RefUpdateManufacturer/$RefUpdateName/$RefUpdateVersion)..."

    $leafUpdateId = New-AduUpdateId -Provider $RefUpdateManufacturer -Name $RefUpdateName -Version $RefUpdateVersion

    # This components update only apply to 'leaf' group.
    $leafCompat = New-AduUpdateCompatibility -DeviceManufacturer $RefUpdateManufacturer -DeviceModel $RefUpdateName

    $leafFirmwareFile = "$PSScriptRoot\firmware_3.0.0.bin"  

    #------------
    # ADD STEP(S)
    #  
    
    # This update contains 1 steps.
    $leafInstallSteps = @()
    
    # Step 1 - Leaf1 update that installs leaf firmware package.
    $leafInstallSteps += New-AduInstallationStep -Handler 'microsoft/swupdate:1' `
                                                 -Files $leafFirmwareFile `
                                                 -HandlerProperties @{  `
                                                     'installedCriteria'="$RefUpdateManufacturer-$RefUpdateName-$RefUpdateVersion";   `
                                                 }   `
                                                 `
                                                 -Description 'Leaf Update'

    # ------------------------------
    # Create child update manifest
    # ------------------------------

    $childUpdateId = $leafUpdateId
    $childUpdateIdStr = "$($childUpdateId.Provider).$($childUpdateId.Name).$($childUpdateId.Version)"
    $childPayloadFiles = $leafFirmwareFile
    $childCompat = $leafCompat
    $childSteps = $leafInstallSteps

    Write-Host "    Preparing child update manifest $childUpdateIdStr ..."

    $childManifest = New-AduImportManifest -UpdateId $childUpdateId -IsDeployable $false `
                                           -Compatibility $childCompat `
                                           -InstallationSteps $childSteps `
                                           -ErrorAction Stop

    # Create folder for manifest files and payload.
    $outputPath = "$OutputManifestPath\$parentUpdateIdStr"
    Write-Host "    Saving child manifest files and payload to $outputPath..."
    New-Item $outputPath -ItemType Directory -ErrorAction SilentlyContinue | Out-Null

    # Generate manifest files.
    $childManifest | Out-File "$outputPath\$childUpdateIdStr.importmanifest.json" -Encoding utf8

    # Copy all payloads (if used)
    Copy-Item -Path $childPayloadFiles -Destination $outputPath -Force

    Write-Host " "

# ----------------------------
# Create the parent update 
# ----------------------------
Write-Host "    Preparing parent update $parentUpdateIdStr ..."

$payloadFiles = 
$parentSteps = @()

    #------------
    # ADD STEP(s)

    $parentSteps += New-AduInstallationStep -UpdateId $leafUpdateId -Description "Leaf Update"

# ------------------------------
# Create parent update manifest
# ------------------------------

Write-Host "    Generating an import manifest $parentUpdateIdStr..."

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
if ($payloadFiles) { Copy-Item -Path $payloadFiles -Destination $outputPath -Force }

Write-Host " "
