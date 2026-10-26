/*
 * usb_stub.c — custom USB device handler that replaces the stock one.
 *
 * The DSO152's stock firmware enumerates as a CDC (serial) device so
 * Windows/macOS drivers load. The CFW replaces this with a minimal
 * vendor-class device: no class code, 1 endpoint, and a custom bulk
 * channel we use to receive trace data at a higher rate than the CDC
 * bulk path allows.
 *
 * This is a stub: it sets up the USB interrupt handler to ack the
 * EP0 SETUP and send a fake descriptor, so the host sees a valid but
 * empty device. A full build would implement the trace-exfil protocol.
 */
#include "ch32.h"
#include "cfw_main.h"

/* CH32 USBFS base (confirmed in peripheral-map.md: 0x50000000). The
 * stock .bin references it; we override its IRQs (USBFS_WKUP=45,
 * USBFS=46) with no-ops via cfw_vectors.S.
 */
#define USBFS_BASE  ((uint32_t)0x50000000u)

/* Minimal SETUP response: a valid vendor device descriptor. */
static const uint8_t cfw_device_desc[18] = {
  0x12, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,  /* length, type, ver, class, sub, proto, maxpkg */
  0x44, 0x00,                                       /* VID = 0x0044 (placeholder) */
  0x33, 0x00,                                       /* PID = 0x0033 (placeholder) */
  0x00, 0x01,                                       /* device release */
  0x01, 0x02, 0x03,                                 /* iManufacturer, iProduct, iSerial */
  0x01                                              /* num configs */
};

/* Stub: set the EP0 buffer pointer and enable the USB IRQ. A full
 * build would replace this with a real vendor protocol. */
void cfw_usb_stub(void)
{
  volatile uint32_t *p = (volatile uint32_t *)USBFS_BASE;
  /*
   * CH32 USBFS controller register layout (from our RE):
   *   0x00 D0  : EP0 endpoint buffer address
   *   0x44 DEVCTL : device control (USB_EN bit)
   *   0x48 INTEN  : interrupt enable
   *
   * For now, point EP0 at our descriptor and enable the USB clock.
   * The IRQs are no-ops (cfw_vectors.S), so the host will time out
   * on the SETUP but at least see a valid device on the bus.
   */
  p[0] = (uint32_t)cfw_device_desc;
  /* p[0x44/4] |= USB_EN;  -- left as a TODO */
}

/* HOLD trigger: detect the HOLD button and freeze the trace.
 * Stub — a full build wires cfw_button_scan() -> hold_state. */
void cfw_hold_trigger(void) { /* TODO */ }

/* Calibration bypass: the stock firmware does an auto-cal that zeroes
 * the channels on boot. The CFW skips it so the user sees the real
 * trace immediately. Stub. */
void cfw_calib_bypass(void) { /* TODO */ }
