//#undef UX_SOURCE_CODE

#include "usb_thread_entry.h"

#include "r_usb_basic_config.h"
#include "r_usb_basic_if.h"

#include "r_usb_basic_pinset.h"

#include "fx_api.h"
#include "ux_api.h"
#include "ux_host_class_storage.h"
#include "ux_system.h"

#include "demo_printf.h"

#define TX_USER_ENABLE_TRACE

/******************************************************************************
 Macro definitions
 *****************************************************************************/
#define UX_STORAGE_BUFFER_SIZE (8 * 1024)

#define EVENT_USB_PLUG_IN (1UL << 0)
#define EVENT_USB_PLUG_OUT (1UL << 1)
#define MEMPOOL_SIZE (36864)
#define DATA_LEN (2048)

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
static TX_EVENT_FLAGS_GROUP g_usb_plug_events;
static FX_FILE g_file;
static FX_MEDIA *g_p_media = UX_NULL;
static uint16_t g_read_buf[UX_STORAGE_BUFFER_SIZE];
static uint16_t g_write_buf[UX_STORAGE_BUFFER_SIZE];
static uint8_t g_ux_pool_memory[MEMPOOL_SIZE];

void usb_thread_main();

static UINT apl_change_function(ULONG event, UX_HOST_CLASS *host_class,
                                VOID *instance) {
  UINT status = UX_SUCCESS;
  UX_HOST_CLASS *class;
  UX_HOST_CLASS_STORAGE *storage;
  UX_HOST_CLASS_STORAGE_MEDIA *storage_media;
  (void)instance;

  /* Check the class container if it is for a USBX Host Mass Storage class. */
  if (UX_SUCCESS ==
      _ux_utility_memory_compare(
          _ux_system_host_class_storage_name, host_class,
          _ux_utility_string_length_get(_ux_system_host_class_storage_name))) {
    if (UX_DEVICE_INSERTION ==
        event) /* Check if there is a device insertion. */
    {
      status =
          ux_host_stack_class_get(_ux_system_host_class_storage_name, &class);
      if (UX_SUCCESS != status) {
        return (status);
      }
      status = ux_host_stack_class_instance_get(class, 0, (void **)&storage);
      if (UX_SUCCESS != status) {
        return (status);
      }
      if (UX_HOST_CLASS_INSTANCE_LIVE != storage->ux_host_class_storage_state) {
        return (UX_ERROR);
      }

      storage_media = class->ux_host_class_media;
      g_p_media = &storage_media->ux_host_class_storage_media;

      tx_event_flags_set(&g_usb_plug_events, EVENT_USB_PLUG_IN, TX_OR);
    } else if (UX_DEVICE_REMOVAL ==
               event) /* Check if there is a device removal. */
    {
      g_p_media = UX_NULL;
      tx_event_flags_set(&g_usb_plug_events, EVENT_USB_PLUG_OUT, TX_OR);
    }
  }
  return status;
} /* End of function () */

void usb_thread_entry_func() {

  usb_ctrl_t ctrl;
  usb_cfg_t cfg;

  UINT ux_ret = 0;
  UINT fx_ret = 0;
  UINT tx_ret = 0;

  fx_system_initialize();
  ux_ret =
      ux_system_initialize((CHAR *)g_ux_pool_memory, MEMPOOL_SIZE, UX_NULL, 0);
  printf("ux ret \r\n", ux_ret);
  ux_ret = ux_host_stack_initialize(apl_change_function);
  printf("ux ret \r\n", ux_ret);
  tx_ret =
      tx_event_flags_create(&g_usb_plug_events, (CHAR *)"USB Plug Event Flags");
  printf("tx ret \r\n", tx_ret);
  /* Register the USB device controllers available in this system */
  R_USB_PinSet_USB0_HOST();

  ctrl.module = USB_IP0;
  ctrl.type = USB_HMSC;
  cfg.usb_speed = USB_FS; /* USB_HS/USB_FS */
  cfg.usb_mode = USB_HOST;
  usb_err_t usb_ret = 0;
  usb_ret = R_USB_Open(&ctrl, &cfg);
  printf("usb ret: %d\r\n", usb_ret);
  usb_thread_main();
}

void usb_thread_main() {

  int8_t volume[32];
  FX_MEDIA *p_media;
  uint32_t actual_length = 0;
  uint32_t actual_flags;
  uint16_t tx_return;
  uint16_t fx_return;
  uint16_t data_count = 0;

  while (1) {
    //
    // tx_thread_sleep(10 * TX_TIMER_TICKS_PER_SECOND);
    printf("USB THREAD RUNNING\r\n");
    /* Wait until device inserted. */
    tx_return = tx_event_flags_get(&g_usb_plug_events, EVENT_USB_PLUG_IN,
                                   TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);
    if (TX_SUCCESS != tx_return) {
      printf("tx return: %d 0x%x\r\n", tx_return, tx_return);
      tx_thread_sleep(TX_WAIT_FOREVER);
    } else {
      printf("USB THREAD RUNNING got connect\r\n");
      /* Get the pointer to FileX Media Control Block for a USB flash device */
      p_media = g_p_media;

      /* Retrieve the volume name of the opened media from the Data sector */
      fx_return =
          fx_media_volume_get(p_media, (CHAR *)volume, FX_DIRECTORY_SECTOR);
      if (FX_SUCCESS == fx_return) {
        /* Set the default directory in the opened media, arbitrary name called
         * "firstdir" */
        fx_directory_default_set(p_media, "firstdir");

        /* Suspend this thread for 200 time-ticks */
        tx_thread_sleep(100);

        /* Try to open the file, 'counter.txt'. */
        fx_return = fx_file_open(p_media, &g_file, "counter.txt",
                                 (FX_OPEN_FOR_READ | FX_OPEN_FOR_WRITE));
        if (FX_SUCCESS != fx_return) {
          /* The 'counter.txt' file is not found, so create a new file */
          fx_return = fx_file_create(p_media, "counter.txt");
          if (FX_SUCCESS != fx_return) {
            break;
          }
          /* Open that file */
          fx_return = fx_file_open(p_media, &g_file, "counter.txt",
                                   (FX_OPEN_FOR_READ | FX_OPEN_FOR_WRITE));
          if (FX_SUCCESS != fx_return) {
            break;
          }
        }

        /* Already open a file, then read the file in blocks */
        /* Set a specified byte offset for reading */
        fx_return = fx_file_seek(&g_file, 0);
        if (FX_SUCCESS == fx_return) {
          fx_return =
              fx_file_read(&g_file, g_read_buf, DATA_LEN, &actual_length);
          if ((FX_SUCCESS == fx_return) || (FX_END_OF_FILE == fx_return)) {
            if (data_count == 1024) {
              /* empty file */
              data_count = 0;
            }

            for (uint16_t data_max_count = data_count;
                 data_count < (data_max_count + 256); data_count++) {
              g_write_buf[data_count] = data_count;
            }

            /*  Set the specified byte offset for writing */
            fx_return = fx_file_seek(&g_file, 0);
            if (FX_SUCCESS == fx_return) {
              /* Write the file in blocks */
              fx_return = fx_file_write(&g_file, g_write_buf, DATA_LEN);
              if (FX_SUCCESS == fx_return) {
                /* None */
              } else {
                tx_thread_sleep(TX_WAIT_FOREVER);
              }
            }
          }
        } else {
          USB_DEBUG_HOOK(USB_DEBUG_HOOK_APL | USB_DEBUG_HOOK_CODE1);
        }

        /* Close already opened file */
        fx_return = fx_file_close(&g_file);
        if (FX_SUCCESS != fx_return) {
          tx_thread_sleep(TX_WAIT_FOREVER);
        }

        tx_thread_sleep(200);
      } else {
        tx_thread_sleep(TX_WAIT_FOREVER);
      }
      /* flush the media */
      fx_return = fx_media_flush(p_media);
      if (FX_SUCCESS != fx_return) {
        tx_thread_sleep(TX_WAIT_FOREVER);
      }

      /* close the media */
      fx_return = fx_media_close(p_media);
      if (FX_SUCCESS != fx_return) {
        tx_thread_sleep(TX_WAIT_FOREVER);
      }

      /*  Wait for unplugging the USB */
      tx_event_flags_get(&g_usb_plug_events, EVENT_USB_PLUG_OUT, TX_OR_CLEAR,
                         &actual_flags, TX_WAIT_FOREVER);
    }
  }
}
