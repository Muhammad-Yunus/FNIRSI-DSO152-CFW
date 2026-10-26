/*
 * cfw_main.c — DSO152 Custom Firmware (CFW) entry point.
 *
 * Boot flow:
 *   1. system_init()      : clocks + GPIOs + IRQ setup
 *   2. cfw_redirect_vectors() : load RAM vector table (disable stock IRQs)
 *   3. lcd_init()         : ST7789 hardware init
 *   4. Show a CFW banner  : prove the CFW is running, not stock firmware
 *   5. Main loop          : button scan + waveform hook (stubs)
 *
 * CFW feature stubs (to be filled in by the full build):
 *   - cfw_usb_stub        : replace stock USB enumeration with a custom
 *     vendor-class handler (USB ISR override in cfw_vectors.S)
 *   - cfw_hold_trigger    : detect the "HOLD" button long-press to
 *     freeze the trace (the stock firmware's HOLD feature)
 *   - cfw_calib_bypass    : disable the stock auto-calibration that
 *     zeroes the channel on boot (a known DSO152 quirk)
 */
#include "ch32.h"
#include "lcd_st7789.h"
#include "buttons.h"
#include "cfw_main.h"

extern void system_init(void);
extern void cfw_redirect_vectors(void);

/* Stub entry points (declared in cfw_main.h) */
void cfw_usb_stub(void);
void cfw_hold_trigger(void);
void cfw_calib_bypass(void);

void cfw_main(void)
{
  /* 1. Clocks + GPIO + IRQs */
  system_init();

  /* 2. Redirect the vector table to our RAM copy so stock USB/ADC
   *    IRQs are replaced with no-ops or our own handlers. */
  cfw_redirect_vectors();

  /* 3. Bring up the LCD */
  lcd_init();

  /* 4. CFW feature stubs */
  cfw_usb_stub();       /* replace stock USB enumeration */
  cfw_hold_trigger();   /* enable HOLD (freeze) button behavior */
  cfw_calib_bypass();   /* skip stock auto-calibration */

  /* 5. Show the CFW banner on the LCD */
  lcd_fill(LCD_C_BLACK);
  lcd_set_brightness(100);
  /*
   * The skeleton banner is a 5x8 font string drawn at (10,10). A full
   * build would render "CFW v0.1" in a larger font and show the chip
   * ID / build timestamp in the top row.
   */
  lcd_draw_string(10, 10, "CFW v0.1 - Sapiens", LCD_C_BLACK, LCD_C_GREEN);

  /* 6. Main loop */
  for (;;)
  {
    uint16_t btns = cfw_button_scan();
    if (btns) {
      /* Full build: map button -> scope action (HOLD, TRIGGER, etc.) */
    }
    /* Full build: trigger an ADC capture and lcd_draw_waveform() */
  }
}
