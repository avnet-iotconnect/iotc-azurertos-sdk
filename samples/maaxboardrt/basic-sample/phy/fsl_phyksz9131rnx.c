/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_phyksz9131rnx.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Defines the PHY KSZ9131RNX vendor defined registers. */
#define PHY_SPECIFIC_STATUS_REG 0x1U  /*!< The PHY specific status register. */
#define PHY_CONTROL_REG         0x1FU /*!< The PHY specific status register. */

/*! @brief Defines the PHY KSZ9131RNX ID number. */
#define PHY_CONTROL_ID1 0x0022U /*!< The PHY ID1 . */

/*! @brief Defines the mask flag in specific status register. */
#define PHY_SSTATUS_LINKSTATUS_MASK 0x04U /*!< The PHY link status mask. */
#define PHY_SSTATUS_LINKSPEED_MASK  0x70U /*!< The PHY link speed mask. */
#define PHY_SSTATUS_LINKDUPLEX_MASK 0x08U /*!< The PHY link duplex mask. */
#define PHY_SSTATUS_LINKSPEED_SHIFT 4U    /*!< The link speed shift */
#define PHY_SSTATUS_LINKSPEED_10M   1U
#define PHY_SSTATUS_LINKSPEED_100M  2U
#define PHY_SSTATUS_LINKSPEED_1000M 4U

/*! @brief Defines the PHY KSZ9131RNX MMD registers in specified device address. */
#define PHY_MDIO_MMD_DEVICE            0x2U  /*!< The MMD device address area including RGMII TX/RX delay setting. */
#define PHY_RGMII_RX_DELAY_REG         0x5U  /*!< The RGMII RX Data Pad Skew Register. */
#define PHY_RGMII_TX_DELAY_REG         0x6U  /*!< The RGMII TX Data Pad Skew Register. */
#define PHY_RGMII_CLK_DELAY_REG        0x8U  /*!< The Clock Pad Skew Register. */

/*! @brief Defines the timeout macro. */
#define PHY_READID_TIMEOUT_COUNT 1000U

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static status_t PHY_KSZ9131RNX_MMD_SetDevice(phy_handle_t *handle,
                                           uint8_t device,
                                           uint16_t addr,
                                           phy_mmd_access_mode_t mode);
static inline status_t PHY_KSZ9131RNX_MMD_ReadData(phy_handle_t *handle, uint32_t *data);
static inline status_t PHY_KSZ9131RNX_MMD_WriteData(phy_handle_t *handle, uint32_t data);
static status_t PHY_KSZ9131RNX_MMD_Read(phy_handle_t *handle, uint8_t device, uint16_t addr, uint32_t *data);
static status_t PHY_KSZ9131RNX_MMD_Write(phy_handle_t *handle, uint8_t device, uint16_t addr, uint32_t data);

/*******************************************************************************
 * Variables
 ******************************************************************************/

const phy_operations_t phyksz9131rnx_ops = {.phyInit            = PHY_KSZ9131RNX_Init,
                                          .phyWrite           = PHY_KSZ9131RNX_Write,
                                          .phyRead            = PHY_KSZ9131RNX_Read,
                                          .getAutoNegoStatus  = PHY_KSZ9131RNX_GetAutoNegotiationStatus,
                                          .getLinkStatus      = PHY_KSZ9131RNX_GetLinkStatus,
                                          .getLinkSpeedDuplex = PHY_KSZ9131RNX_GetLinkSpeedDuplex,
                                          .setLinkSpeedDuplex = PHY_KSZ9131RNX_SetLinkSpeedDuplex,
                                          .enableLoopback     = PHY_KSZ9131RNX_EnableLoopback};

/*******************************************************************************
 * Code
 ******************************************************************************/

status_t PHY_KSZ9131RNX_Init(phy_handle_t *handle, const phy_config_t *config)
{
    uint32_t counter = PHY_READID_TIMEOUT_COUNT;
    status_t result;
    uint32_t regValue = 0U;

    /* Init MDIO interface. */
    MDIO_Init(handle->mdioHandle);

    /* Assign phy address. */
    handle->phyAddr = config->phyAddr;

    /* Check PHY ID. */
    do
    {
        result = MDIO_Read(handle->mdioHandle, handle->phyAddr, PHY_ID1_REG, &regValue);
        if (result != kStatus_Success)
        {
            return result;
        }
        counter--;
    } while ((regValue != PHY_CONTROL_ID1) && (counter != 0U));

    if (counter == 0U)
    {
        return kStatus_Fail;
    }

    /* Reset PHY. */
    result = MDIO_Write(handle->mdioHandle, handle->phyAddr, PHY_BASICCONTROL_REG, PHY_BCTL_RESET_MASK);
    if (result != kStatus_Success)
    {
        return result;
    }

    if (config->autoNeg)
    {
        /* Set the auto-negotiation. */
        result =
            MDIO_Write(handle->mdioHandle, handle->phyAddr, PHY_AUTONEG_ADVERTISE_REG,
                       PHY_100BASETX_FULLDUPLEX_MASK | PHY_100BASETX_HALFDUPLEX_MASK | PHY_10BASETX_FULLDUPLEX_MASK |
                           PHY_10BASETX_HALFDUPLEX_MASK | PHY_IEEE802_3_SELECTOR_MASK);
        if (result == kStatus_Success)
        {
            result = MDIO_Write(handle->mdioHandle, handle->phyAddr, PHY_1000BASET_CONTROL_REG,
                                PHY_1000BASET_FULLDUPLEX_MASK);
            if (result == kStatus_Success)
            {
                result = MDIO_Read(handle->mdioHandle, handle->phyAddr, PHY_BASICCONTROL_REG, &regValue);
                if (result == kStatus_Success)
                {
                    result = MDIO_Write(handle->mdioHandle, handle->phyAddr, PHY_BASICCONTROL_REG,
                                        (regValue | PHY_BCTL_AUTONEG_MASK | PHY_BCTL_RESTART_AUTONEG_MASK));
                }
            }
        }
    }
    else
    {
        /* Disable isolate mode */
        result = MDIO_Read(handle->mdioHandle, handle->phyAddr, PHY_BASICCONTROL_REG, &regValue);
        if (result != kStatus_Success)
        {
            return result;
        }
        regValue &= ~PHY_BCTL_ISOLATE_MASK;
        result = MDIO_Write(handle->mdioHandle, handle->phyAddr, PHY_BASICCONTROL_REG, regValue);
        if (result != kStatus_Success)
        {
            return result;
        }

        /* Disable the auto-negotiation and set user-defined speed/duplex configuration. */
        result = PHY_KSZ9131RNX_SetLinkSpeedDuplex(handle, config->speed, config->duplex);
    }

    return result;
}

status_t PHY_KSZ9131RNX_Write(phy_handle_t *handle, uint32_t phyReg, uint32_t data)
{
    return MDIO_Write(handle->mdioHandle, handle->phyAddr, phyReg, data);
}

status_t PHY_KSZ9131RNX_Read(phy_handle_t *handle, uint32_t phyReg, uint32_t *dataPtr)
{
    return MDIO_Read(handle->mdioHandle, handle->phyAddr, phyReg, dataPtr);
}

status_t PHY_KSZ9131RNX_GetAutoNegotiationStatus(phy_handle_t *handle, bool *status)
{
    assert(status);

    status_t result;
    uint32_t regValue;

    *status = false;

    /* Check auto negotiation complete. */
    result = MDIO_Read(handle->mdioHandle, handle->phyAddr, PHY_BASICSTATUS_REG, &regValue);
    if (result == kStatus_Success)
    {
        if ((regValue & PHY_BSTATUS_AUTONEGCOMP_MASK) != 0U)
        {
            *status = true;
        }
    }
    return result;
}

status_t PHY_KSZ9131RNX_GetLinkStatus(phy_handle_t *handle, bool *status)
{
    assert(status);

    status_t result;
    uint32_t regValue;

    /* Read the basic status register. */
    result = MDIO_Read(handle->mdioHandle, handle->phyAddr, PHY_SPECIFIC_STATUS_REG, &regValue);
    if (result == kStatus_Success)
    {
        if ((PHY_SSTATUS_LINKSTATUS_MASK & regValue) != 0U)
        {
            /* Link up. */
            *status = true;
        }
        else
        {
            /* Link down. */
            *status = false;
        }
    }
    return result;
}

status_t PHY_KSZ9131RNX_GetLinkSpeedDuplex(phy_handle_t *handle, phy_speed_t *speed, phy_duplex_t *duplex)
{
    assert(!((speed == NULL) && (duplex == NULL)));

    status_t result;
    uint32_t regValue;

    /* Read the status register. */
    result = MDIO_Read(handle->mdioHandle, handle->phyAddr, PHY_CONTROL_REG, &regValue);
    if (result == kStatus_Success)
    {
        if (speed != NULL)
        {
            switch ((regValue & PHY_SSTATUS_LINKSPEED_MASK) >> PHY_SSTATUS_LINKSPEED_SHIFT)
            {
                case (uint32_t)PHY_SSTATUS_LINKSPEED_10M:
                    *speed = kPHY_Speed10M;
                    break;
                case (uint32_t)PHY_SSTATUS_LINKSPEED_100M:
                    *speed = kPHY_Speed100M;
                    break;
                case (uint32_t)PHY_SSTATUS_LINKSPEED_1000M:
                    *speed = kPHY_Speed1000M;
                    break;
                default:
                    *speed = kPHY_Speed10M;
                    break;
            }
        }

        if (duplex != NULL)
        {
            if ((regValue & PHY_SSTATUS_LINKDUPLEX_MASK) != 0U)
            {
                *duplex = kPHY_FullDuplex;
            }
            else
            {
                *duplex = kPHY_HalfDuplex;
            }
        }
    }
    return result;
}

status_t PHY_KSZ9131RNX_SetLinkSpeedDuplex(phy_handle_t *handle, phy_speed_t speed, phy_duplex_t duplex)
{
    status_t result;
    uint32_t regValue;

    result = MDIO_Read(handle->mdioHandle, handle->phyAddr, PHY_BASICCONTROL_REG, &regValue);
    if (result == kStatus_Success)
    {
        /* Disable the auto-negotiation and set according to user-defined configuration. */
        regValue &= ~PHY_BCTL_AUTONEG_MASK;
        if (speed == kPHY_Speed1000M)
        {
            regValue &= ~PHY_BCTL_SPEED0_MASK;
            regValue |= PHY_BCTL_SPEED1_MASK;
        }
        else if (speed == kPHY_Speed100M)
        {
            regValue |= PHY_BCTL_SPEED0_MASK;
            regValue &= ~PHY_BCTL_SPEED1_MASK;
        }
        else
        {
            regValue &= ~PHY_BCTL_SPEED0_MASK;
            regValue &= ~PHY_BCTL_SPEED1_MASK;
        }
        if (duplex == kPHY_FullDuplex)
        {
            regValue |= PHY_BCTL_DUPLEX_MASK;
        }
        else
        {
            regValue &= ~PHY_BCTL_DUPLEX_MASK;
        }
        result = MDIO_Write(handle->mdioHandle, handle->phyAddr, PHY_BASICCONTROL_REG, regValue);
    }
    return result;
}

status_t PHY_KSZ9131RNX_EnableLoopback(phy_handle_t *handle, phy_loop_t mode, phy_speed_t speed, bool enable)
{
    /* This PHY only supports local loopback. */
    assert(mode == kPHY_LocalLoop);

    status_t result;
    uint32_t regValue;

    /* Set the loop mode. */
    if (enable)
    {
        if (speed == kPHY_Speed1000M)
        {
            regValue = PHY_BCTL_SPEED1_MASK | PHY_BCTL_DUPLEX_MASK | PHY_BCTL_LOOP_MASK;
        }
        else if (speed == kPHY_Speed100M)
        {
            regValue = PHY_BCTL_SPEED0_MASK | PHY_BCTL_DUPLEX_MASK | PHY_BCTL_LOOP_MASK;
        }
        else
        {
            regValue = PHY_BCTL_DUPLEX_MASK | PHY_BCTL_LOOP_MASK;
        }
        result = MDIO_Write(handle->mdioHandle, handle->phyAddr, PHY_BASICCONTROL_REG, regValue);
    }
    else
    {
        /* First read the current status in control register. */
        result = MDIO_Read(handle->mdioHandle, handle->phyAddr, PHY_BASICCONTROL_REG, &regValue);
        if (result == kStatus_Success)
        {
            regValue &= ~PHY_BCTL_LOOP_MASK;
            result = MDIO_Write(handle->mdioHandle, handle->phyAddr, PHY_BASICCONTROL_REG,
                                (regValue | PHY_BCTL_RESTART_AUTONEG_MASK));
        }
    }
    return result;
}

static status_t PHY_KSZ9131RNX_MMD_SetDevice(phy_handle_t *handle,
                                           uint8_t device,
                                           uint16_t addr,
                                           phy_mmd_access_mode_t mode)
{
    status_t result = kStatus_Success;

    /* Set Function mode of address access(b00) and device address. */
    result = MDIO_Write(handle->mdioHandle, handle->phyAddr, PHY_MMD_ACCESS_CONTROL_REG, device);
    if (result != kStatus_Success)
    {
        return result;
    }

    /* Set register address. */
    result = MDIO_Write(handle->mdioHandle, handle->phyAddr, PHY_MMD_ACCESS_DATA_REG, addr);
    if (result != kStatus_Success)
    {
        return result;
    }

    /* Set Function mode of data access(b01~11) and device address. */
    result =
        MDIO_Write(handle->mdioHandle, handle->phyAddr, PHY_MMD_ACCESS_CONTROL_REG, (uint32_t)mode | (uint32_t)device);
    return result;
}

static inline status_t PHY_KSZ9131RNX_MMD_ReadData(phy_handle_t *handle, uint32_t *data)
{
    return MDIO_Read(handle->mdioHandle, handle->phyAddr, PHY_MMD_ACCESS_DATA_REG, data);
}

static inline status_t PHY_KSZ9131RNX_MMD_WriteData(phy_handle_t *handle, uint32_t data)
{
    return MDIO_Write(handle->mdioHandle, handle->phyAddr, PHY_MMD_ACCESS_DATA_REG, data);
}

static status_t PHY_KSZ9131RNX_MMD_Read(phy_handle_t *handle, uint8_t device, uint16_t addr, uint32_t *data)
{
    status_t result = kStatus_Success;
    result = PHY_KSZ9131RNX_MMD_SetDevice(handle, device, addr, kPHY_MMDAccessNoPostIncrement);
    if (result == kStatus_Success)
    {
        result = PHY_KSZ9131RNX_MMD_ReadData(handle, data);
    }
    return result;
}

static status_t PHY_KSZ9131RNX_MMD_Write(phy_handle_t *handle, uint8_t device, uint16_t addr, uint32_t data)
{
    status_t result = kStatus_Success;

    result = PHY_KSZ9131RNX_MMD_SetDevice(handle, device, addr, kPHY_MMDAccessNoPostIncrement);
    if (result == kStatus_Success)
    {
        result = PHY_KSZ9131RNX_MMD_WriteData(handle, data);
    }
    return result;
}
