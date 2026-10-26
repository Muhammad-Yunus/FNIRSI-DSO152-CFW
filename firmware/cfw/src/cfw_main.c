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
 * CFW features:
 *   - cfw_usb_stub        : replace stock USB enumeration with a custom
 *     vendor-class handler (USB ISR override in cfw_vectors.S). Stub.
 *   - cfw_hold_trigger    : detect the "HOLD" button press and freeze the
 *     trace (implemented, debounced in hold_trigger.c against SysTick).
 *   - cfw_calib_bypass    : disable the stock auto-calibration that
 *     zeroes the channel on boot (a known DSO152 quirk). Stub.
 */
#include "ch32.h"
#include "lcd_st7789.h"
#include "buttons.h"
#include "cfw_main.h"
#include "hold_trigger.h"

extern void system_init(void);
extern void cfw_redirect_vectors(void);

/* Stub entry points (declared in cfw_main.h) */
void cfw_usb_stub(void);
void cfw_calib_bypass(void);

/* ------------------------------------------------------------------ */
/* SysTick tick timer (1 ms).                                          */
/*                                                                      */
/* cfw_vectors.S defines every external IRQ handler as a weak alias to  */
/* Default_Handler. Defining SysTick_Handler here (with normal linkage)*/
/* overrides that weak alias for this target, so the 1 ms IRQ lands    */
/* here instead of the infinite loop.                                   */
/* ------------------------------------------------------------------ */
static volatile uint32_t cfw_tick_count; /* bumped once per 1 ms tick  */

void cfw_tick_init(void)
{
  /* Reload value for a 1 ms tick at 72 MHz HCLK (see CFW_SYSTICK_1MS). */
  SysTick->LOAD = CFW_SYSTICK_1MS;
  /* Clear COUNTFLAG before starting so the first IRQ is exactly 1 ms. */
  SysTick->VAL = 0u;
  /*
   * CTL bits: TICKINT (IRQ on each underflow) | ENABLE. On Cortex-M3
   * the SysTick runs off the CPU clock (HCLK) when CLKSOURCE is 0, which
   * is its reset value, so we do not need to touch bit 2.
   */
  SysTick->CTL = SysTick_CTRL_ENABLE | SysTick_CTRL_TICKINT;
}

uint32_t cfw_tick_ms(void)
{
  return cfw_tick_count;
}

/* Blocking wait: safe from the main loop (not from an ISR). We capture
 * the start counter and spin until n ticks have elapsed. */
void cfw_delay_ms(uint32_t nms)
{
  uint32_t start = cfw_tick_count;
  while ((cfw_tick_count - start) < nms) {
    /* busy-wait; the 1 ms IRQ advances cfw_tick_count */
  }
}

/* Service the 1 ms tick. Runs in IRQ context; keep it minimal. */
void SysTick_Handler(void)
{
  cfw_tick_count++;
}

void cfw_main(void)
{
  /* 1. Clocks + GPIO + IRQs */
  system_init();

  /* 2. Redirect the vector table to our RAM copy so stock USB/ADC
   *    IRQs are replaced with no-ops or our own handlers. */
  cfw_redirect_vectors();

  /* 3. Start the 1 ms SysTick so the main loop has a time base. */
  cfw_tick_init();
  cfw_delay_ms(10); /* let PLL + clocks settle before touching the LCD */

  /* 4. Bring up the LCD */
  lcd_init();

  /* 5. CFW feature stubs */
  cfw_usb_stub();       /* replace stock USB enumeration */
  cfw_hold_trigger();   /* enable HOLD (freeze) button behavior */
  cfw_calib_bypass();   /* skip stock auto-calibration */

  /* 6. Show the CFW banner on the LCD */
  lcd_fill(LCD_C_BLACK);
  lcd_set_brightness(100);
  /*
   * The skeleton banner is a 5x8 font string drawn at (10,10). A full
   * build would render "CFW v0.1" in a larger font and show the chip
   * ID / build timestamp in the top row.
   */
  lcd_draw_string(10, 10, "CFW v0.1 - Sapiens", LCD_C_BLACK, LCD_C_GREEN);

  /* 7. Main loop — time-based cadence via the SysTick. The HOLD button
   *    is polled each iteration through cfw_hold_trigger_poll(); while
   *    the trace is frozen we skip waveform refresh so the display holds
   *    the last captured trace. A full build gates the real ADC capture
   *    + lcd_draw_waveform() on cfw_hold_active(). */
  static uint8_t s_hold_indicator_drawn;
  for (;;)
  {
    uint16_t btns = cfw_button_scan();
    cfw_hold_trigger_poll(btns);

    if (cfw_hold_active()) {
      /* Trace frozen: show the HOLD indicator once, then hold the screen. */
      if (!s_hold_indicator_drawn) {
        lcd_draw_string(240 - 72, 10, "HOLD", LCD_C_BLACK, LCD_C_RED);
        s_hold_indicator_drawn = 1;
      }
      continue;
    }

    if (s_hold_indicator_drawn) {
      /* HOLD released: clear the indicator, resume live refresh. */
      lcd_draw_string(240 - 72, 10, "    ", LCD_C_BLACK, LCD_C_BLACK);
      s_hold_indicator_drawn = 0;
    }

    /* Live mode: full build triggers an ADC capture here and calls
     * lcd_draw_waveform(). Skipped while HOLD is active above. */
  }
}
