//#undef UX_SOURCE_CODE

#include "usb_thread_entry.h"

#include "r_usb_basic_config.h"
#include "r_usb_basic_if.h"

#include "r_usb_basic_pinset.h"

#include "fx_api.h"
#include "ux_api.h"
#include "ux_host_class_storage.h"
#include "ux_system.h"

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
  int8_t volume[32];
  FX_MEDIA *p_media;
  uint32_t actual_length = 0;
  uint32_t actual_flags;
  uint16_t tx_return;
  uint16_t fx_return;
  uint16_t data_count = 0;
  usb_ctrl_t ctrl;
  usb_cfg_t cfg;

  fx_system_initialize();
  ux_system_initialize((CHAR *)g_ux_pool_memory, MEMPOOL_SIZE, UX_NULL, 0);
  ux_host_stack_initialize(apl_change_function);
  tx_event_flags_create(&g_usb_plug_events, (CHAR *)"USB Plug Event Flags");

  /* Register the USB device controllers available in this system */
  R_USB_PinSet_USB0_HOST();

  ctrl.module = USB_IP0;
  ctrl.type = USB_HMSC;
  cfg.usb_speed = USB_FS; /* USB_HS/USB_FS */
  cfg.usb_mode = USB_HOST;
  R_USB_Open(&ctrl, &cfg);
}