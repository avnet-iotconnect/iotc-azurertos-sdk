/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name        : Config_SCI5.c
* Component Version: 1.12.0
* Device(s)        : R5F565NEDxFP
* Description      : This file implements device driver for Config_SCI5.
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "Config_SCI5.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
volatile uint8_t * gp_sci5_tx_address;                /* SCI5 transmit buffer address */
volatile uint16_t  g_sci5_tx_count;                   /* SCI5 transmit data number */
volatile uint8_t * gp_sci5_rx_address;                /* SCI5 receive buffer address */
volatile uint16_t  g_sci5_rx_count;                   /* SCI5 receive data number */
volatile uint16_t  g_sci5_rx_length;                  /* SCI5 receive data length */
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_SCI5_Create
* Description  : This function initializes the SCI5 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI5_Create(void)
{
    /* Cancel SCI stop state */
    MSTP(SCI5) = 0U;

    /* Set interrupt priority */
    IPR(SCI5, RXI5) = _0F_SCI_PRIORITY_LEVEL15;
    IPR(SCI5, TXI5) = _0F_SCI_PRIORITY_LEVEL15;

    /* Clear the control register */
    SCI5.SCR.BYTE = 0x00U;

    /* Set clock enable */
    SCI5.SCR.BYTE = _00_SCI_INTERNAL_SCK_UNUSED;

    /* Clear the SIMR1.IICM, SPMR.CKPH, and CKPOL bit, and set SPMR */
    SCI5.SIMR1.BIT.IICM = 0U;
    SCI5.SPMR.BYTE = _00_SCI_RTS | _00_SCI_CLOCK_NOT_INVERTED | _00_SCI_CLOCK_NOT_DELAYED;

    /* Set control registers */
    SCI5.SMR.BYTE = _00_SCI_CLOCK_PCLK | _00_SCI_MULTI_PROCESSOR_DISABLE | _00_SCI_STOP_1 | _00_SCI_PARITY_DISABLE | 
                    _00_SCI_DATA_LENGTH_8 | _00_SCI_ASYNCHRONOUS_OR_I2C_MODE;
    SCI5.SCMR.BYTE = _00_SCI_SERIAL_MODE | _00_SCI_DATA_INVERT_NONE | _00_SCI_DATA_LSB_FIRST | 
                     _10_SCI_DATA_LENGTH_8_OR_7 | _62_SCI_SCMR_DEFAULT;
    SCI5.SEMR.BYTE = _00_SCI_BIT_MODULATION_DISABLE | _10_SCI_8_BASE_CLOCK | _00_SCI_NOISE_FILTER_DISABLE | 
                     _40_SCI_BAUDRATE_DOUBLE | _00_SCI_LOW_LEVEL_START_BIT;

    /* Set bit rate */
    SCI5.BRR = 0x40U;

    /* Set RXD5 pin */
    MPC.PA3PFS.BYTE = 0x0AU;
    PORTA.PMR.BYTE |= 0x08U;

    /* Set TXD5 pin */
    MPC.PA4PFS.BYTE = 0x0AU;
    PORTA.PODR.BYTE |= 0x10U;
    PORTA.PDR.BYTE |= 0x10U;

    R_Config_SCI5_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_SCI5_Start
* Description  : This function starts the SCI5 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI5_Start(void)
{
    /* Clear interrupt flag */
    IR(SCI5, TXI5) = 0U;
    IR(SCI5, RXI5) = 0U;

    /* Enable SCI interrupt */
    IEN(SCI5, TXI5) = 1U;
    ICU.GENBL0.BIT.EN10 = 1U;
    IEN(SCI5, RXI5) = 1U;
    ICU.GENBL0.BIT.EN11 = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_SCI5_Stop
* Description  : This function stop the SCI5 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI5_Stop(void)
{
    /* Set TXD5 pin */
    PORTA.PMR.BYTE &= 0xEFU;

    /* Disable serial transmit */
    SCI5.SCR.BIT.TE = 0U;

    /* Disable serial receive */
    SCI5.SCR.BIT.RE = 0U;

    /* Disable SCI interrupt */
    SCI5.SCR.BIT.TIE = 0U;
    SCI5.SCR.BIT.RIE = 0U;
    IEN(SCI5, TXI5) = 0U;
    ICU.GENBL0.BIT.EN10 = 0U;
    IR(SCI5, TXI5) = 0U;
    IEN(SCI5, RXI5) = 0U;
    ICU.GENBL0.BIT.EN11 = 0U;
    IR(SCI5, RXI5) = 0U;
}

/***********************************************************************************************************************
* Function Name: R_Config_SCI5_Serial_Receive
* Description  : This function receive SCI5data
* Arguments    : rx_buf -
*                    receive buffer pointer (Not used when receive data handled by DMAC/DTC)
*                rx_num -
*                    buffer size (Not used when receive data handled by DMAC/DTC)
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/

MD_STATUS R_Config_SCI5_Serial_Receive(uint8_t * const rx_buf, uint16_t rx_num)
{
    MD_STATUS status = MD_OK;

    if (1U > rx_num)
    {
        status = MD_ARGERROR;
    }
    else
    {
        g_sci5_rx_count = 0U;
        g_sci5_rx_length = rx_num;
        gp_sci5_rx_address = rx_buf;
        SCI5.SCR.BIT.RIE = 1U;
        SCI5.SCR.BIT.RE = 1U;
    }

    return (status);
}

/***********************************************************************************************************************
* Function Name: R_Config_SCI5_Serial_Send
* Description  : This function transmits SCI5 data
* Arguments    : tx_buf -
*                    transfer buffer pointer (Not used when transmit data handled by DMAC/DTC)
*                tx_num -
*                    buffer size (Not used when transmit data handled by DMAC/DTC)
* Return Value : status -
*                    MD_OK or MD_ARGERROR or MD_ERROR
***********************************************************************************************************************/

MD_STATUS R_Config_SCI5_Serial_Send(uint8_t * const tx_buf, uint16_t tx_num)
{
    MD_STATUS status = MD_OK;

    if (1U > tx_num)
    {
        status = MD_ARGERROR;
    }
    else if (0U == IEN(SCI5, TXI5))
    {
        status = MD_ERROR;
    }
    else
    {
        gp_sci5_tx_address = tx_buf;
        g_sci5_tx_count = tx_num;
        IEN(SCI5, TXI5) = 0U;
        SCI5.SCR.BYTE |= 0xA0U;

        /* Set TXD5 pin */
        PORTA.PMR.BYTE |= 0x10U;
        IEN(SCI5, TXI5) = 1U;
    }

    return (status);
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
