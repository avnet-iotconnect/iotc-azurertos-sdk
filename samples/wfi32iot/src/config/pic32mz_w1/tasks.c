/*******************************************************************************
 System Tasks File

  File Name:
    tasks.c

  Summary:
    This file contains source code necessary to maintain system's polled tasks.

  Description:
    This file contains source code necessary to maintain system's polled tasks.
    It implements the "SYS_Tasks" function that calls the individual "Tasks"
    functions for all polled MPLAB Harmony modules in the system.

  Remarks:
    This file requires access to the systemObjects global data structure that
    contains the object handles to all MPLAB Harmony module objects executing
    polled in the system.  These handles are passed into the individual module
    "Tasks" functions to identify the instance of the module to maintain.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "definitions.h"

/* ThreadX byte memory pool from which to allocate the thread stacks. */
#define TX_BYTE_POOL_SIZE   (32000 + 512)

#define PIC32MZW_TASK_STACK_SIZE    4*1024

unsigned char* get_byte_pool(void) {
    // this should make the byte pool not be initialized
    static unsigned char byte_pool_storage[TX_BYTE_POOL_SIZE];
    return byte_pool_storage;
}

TX_BYTE_POOL   byte_pool_0;

// *****************************************************************************
// *****************************************************************************
// Section: RTOS "Tasks" Routine
// *****************************************************************************
// *****************************************************************************



TX_THREAD      _DRV_MEMORY_0_Task_TCB;
uint8_t*       _DRV_MEMORY_0_Task_Stk_Ptr;

static void _DRV_MEMORY_0_Tasks( ULONG thread_input )
{
    while(1)
    {
        DRV_MEMORY_Tasks(sysObj.drvMemory0);
        tx_thread_sleep((ULONG)(DRV_MEMORY_RTOS_DELAY_IDX0 / (TX_TICK_PERIOD_MS)));
    }
}



TX_THREAD      _SYS_CMD_Task_TCB;
uint8_t*       _SYS_CMD_Task_Stk_Ptr;

static void _SYS_CMD_Tasks( ULONG thread_input )
{
    while(1)
    {
        SYS_CMD_Tasks();
        tx_thread_sleep((ULONG)(10 / (TX_TICK_PERIOD_MS)));
    }
}


TX_THREAD      _APP_IDLE_Task_TCB;
uint8_t*       _APP_IDLE_Task_Stk_Ptr;

static void _APP_IDLE_Tasks( ULONG thread_input )
{
    while(1)
    {
        /* IDLE Task will be executed if there are not tasks in ready state */

    }
}

TX_THREAD      _APP_Task_TCB;
uint8_t*       _APP_Task_Stk_Ptr;

TX_THREAD      _APP_PIC32MZ_W1_Task_TCB;
uint8_t*       _APP_PIC32MZ_W1_Task_Stk_Ptr;


static void _APP_Tasks( ULONG thread_input )
{
    while(1)
    {
        APP_Tasks();
        tx_thread_sleep((ULONG)(50 / (TX_TICK_PERIOD_MS)));
    }
}

static void _APP_PIC32MZ_W1_Tasks( ULONG thread_input )
{
    while(1)
    {
        APP_PIC32MZ_W1_Tasks();
        tx_thread_sleep((ULONG)(50 / (TX_TICK_PERIOD_MS)));
    }
}




TX_THREAD      _SYS_USB_DEVICE_Task_TCB;
uint8_t*       _SYS_USB_DEVICE_Task_Stk_Ptr;

static void _SYS_USB_DEVICE_Tasks( ULONG thread_input )
{
    while(1)
    {
        /* USB Device layer tasks routine */
        USB_DEVICE_Tasks(sysObj.usbDevObject0);
        tx_thread_sleep((ULONG)(10 / (TX_TICK_PERIOD_MS)));
    }
}




TX_THREAD      _SYS_FS_Task_TCB;
uint8_t*       _SYS_FS_Task_Stk_Ptr;

static void _SYS_FS_Tasks( ULONG thread_input )
{
    while(1)
    {
        SYS_FS_Tasks();
        tx_thread_sleep((ULONG)(10 / (TX_TICK_PERIOD_MS)));
    }
}


TX_THREAD   _WDRV_PIC32MZW_Task_TCB;
uint8_t*    _WDRV_PIC32MZW_Task_Stk_Ptr;

static void _WDRV_PIC32MZW_Tasks(ULONG thread_input)
{
    while(1)
    {
        SYS_STATUS status = SYS_STATUS_UNINITIALIZED;
        WDRV_PIC32MZW_Tasks(sysObj.drvWifiPIC32MZW1);
        status = WDRV_PIC32MZW_Status(sysObj.drvWifiPIC32MZW1);
        if ((SYS_STATUS_ERROR == status) || (SYS_STATUS_UNINITIALIZED == status))
        {
            tx_thread_sleep((ULONG)(50 / (TX_TICK_PERIOD_MS)));
        }
    }
}

void tx_application_define(void* first_unused_memory)
{
    (void)first_unused_memory;
    /* Create a byte memory pool from which to allocate the thread stacks. */
    tx_byte_pool_create(&byte_pool_0, "byte pool 0", get_byte_pool(), TX_BYTE_POOL_SIZE);

    /* Maintain system services */

#if 0
    tx_byte_allocate(&byte_pool_0,
        (VOID **) &_SYS_CMD_Task_Stk_Ptr,
        SYS_CMD_RTOS_STACK_SIZE,
        TX_NO_WAIT
    );

    tx_thread_create(&_SYS_CMD_Task_TCB,
        "SYS_CMD_TASKS",
        _SYS_CMD_Tasks,
        0,
        _SYS_CMD_Task_Stk_Ptr,
        SYS_CMD_RTOS_STACK_SIZE,
        SYS_CMD_RTOS_TASK_PRIORITY,
        SYS_CMD_RTOS_TASK_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START
    );

    if (status) { while (1); };
#endif
    
    tx_byte_allocate(&byte_pool_0,
        (VOID **) &_SYS_FS_Task_Stk_Ptr,
        SYS_FS_STACK_SIZE,
        TX_NO_WAIT
    );

    tx_thread_create(&_SYS_FS_Task_TCB,
        "SYS_FS_TASKS",
        _SYS_FS_Tasks,
        0,
        _SYS_FS_Task_Stk_Ptr,
        SYS_FS_STACK_SIZE,
        SYS_FS_PRIORITY,
        SYS_FS_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START
    );

    tx_byte_allocate(&byte_pool_0,
        (VOID **) &_WDRV_PIC32MZW_Task_Stk_Ptr,
        PIC32MZW_TASK_STACK_SIZE,
        TX_NO_WAIT
    );

    /* create the _WDRV_WINC_Tasks thread */
    tx_thread_create(&_WDRV_PIC32MZW_Task_TCB,
        (char*)"_WDRV_PIC32MZW_Tasks",
        _WDRV_PIC32MZW_Tasks,
        0,
        _WDRV_PIC32MZW_Task_Stk_Ptr,
        PIC32MZW_TASK_STACK_SIZE,
        6,
        6,
        TX_NO_TIME_SLICE,
        TX_AUTO_START
    );


    /* Maintain Device Drivers */
        tx_byte_allocate(&byte_pool_0,
       (VOID **) &_DRV_MEMORY_0_Task_Stk_Ptr,
        DRV_MEMORY_STACK_SIZE_IDX0,
        TX_NO_WAIT);

    tx_thread_create(&_DRV_MEMORY_0_Task_TCB,
        "DRV_MEM_0_TASKS",
        _DRV_MEMORY_0_Tasks,
        0,
        _DRV_MEMORY_0_Task_Stk_Ptr,
        DRV_MEMORY_STACK_SIZE_IDX0,
        DRV_MEMORY_PRIORITY_IDX0,
        DRV_MEMORY_PRIORITY_IDX0,
        TX_NO_TIME_SLICE,
        TX_AUTO_START
        );


    tx_byte_allocate(&byte_pool_0,
       (VOID **) &_SYS_USB_DEVICE_Task_Stk_Ptr,
        1024,
        TX_NO_WAIT);

    tx_thread_create(&_SYS_USB_DEVICE_Task_TCB,
        "USB_DEVICE_TASKS",
        _SYS_USB_DEVICE_Tasks,
        0,
        _SYS_USB_DEVICE_Task_Stk_Ptr,
        1024,
        1,
        1,
        TX_NO_TIME_SLICE,
        TX_AUTO_START
        );


    /* Maintain Middleware & Other Libraries */
        






    /* Maintain the application's state machine. */
    
        /* Allocate the stack for _APP_IDLE_Tasks threads */
    tx_byte_allocate(&byte_pool_0,
        (VOID **) &_APP_IDLE_Task_Stk_Ptr,
        TX_MINIMUM_STACK,
        TX_NO_WAIT
    );

    /* create the _APP_IDLE_Tasks thread */
    tx_thread_create(&_APP_IDLE_Task_TCB,
        "_APP_IDLE_Tasks",
        _APP_IDLE_Tasks,
        1,
        _APP_IDLE_Task_Stk_Ptr,
        TX_MINIMUM_STACK,
        (TX_MAX_PRIORITIES - 1),
        (TX_MAX_PRIORITIES - 1),
        TX_NO_TIME_SLICE,
        TX_AUTO_START
    );

    /* Allocate the stack for _APP threads */
    tx_byte_allocate(&byte_pool_0,
        (VOID **) &_APP_Task_Stk_Ptr,
        2048,
        TX_NO_WAIT
    );

    /* create the _APP thread */
    tx_thread_create(&_APP_Task_TCB,
        "_APP_Tasks",
        _APP_Tasks,
        0,
        _APP_Task_Stk_Ptr,
        2048,
        1,
        1,
        TX_NO_TIME_SLICE,
        TX_AUTO_START
    );
    /* Allocate the stack for _APP_PIC32MZ_W1 threads */
    tx_byte_allocate(&byte_pool_0,
        (VOID **) &_APP_PIC32MZ_W1_Task_Stk_Ptr,
        4096,
        TX_NO_WAIT
    );

    /* create the _APP_PIC32MZ_W1 thread */
    tx_thread_create(&_APP_PIC32MZ_W1_Task_TCB,
        "_APP_PIC32MZ_W1_Tasks",
        _APP_PIC32MZ_W1_Tasks,
        1,
        _APP_PIC32MZ_W1_Task_Stk_Ptr,
        4096,
        1,
        1,
        TX_NO_TIME_SLICE,
        TX_AUTO_START
    );
}

// *****************************************************************************
// *****************************************************************************
// Section: System "Tasks" Routine
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void SYS_Tasks ( void )

  Remarks:
    See prototype in system/common/sys_module.h.
*/
void SYS_Tasks ( void )
{
        /*Enter the ThreadX kernel.*/
    tx_kernel_enter();

}

/*******************************************************************************
 End of File
 */

