#include "usb_thread_entry.h"

#include "r_usb_basic_config.h"
#include "r_usb_basic_if.h"

#include "r_usb_basic_pinset.h"

#include "fx_api.h"
#include "ux_api.h"
#include "ux_host_class_storage.h"
#include "ux_system.h"

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