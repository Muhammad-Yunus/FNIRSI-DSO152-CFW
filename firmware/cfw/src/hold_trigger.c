/*
 * hold_trigger.c — debounced HOLD (trace-freeze) button state machine.
 *
 * Polls cfw_button_scan() against the 1 ms SysTick time base every
 * HOLD_POLL_INTERVAL_MS, requires HOLD_DEBOUNCE_SAMPLES consecutive
 * agreeing samples to flip the state, and exposes cfw_hold_active() so
 * the main loop can skip waveform redraws while the trace is frozen.
 */
#include "ch32.h"
#include "hold_trigger.h"
#include "buttons.h"
#include "cfw_main.h"

/* Placeholder mapping: cfw_button_scan() currently only surfaces the
 * two legacy game pins (PA10=BTN_1, PC8=BTN_2). PC8 is the natural
 * "right-hand control" and is used as the HOLD button until the real
 * 4x4 ADC matrix scan lands. See buttons.c. */
#define CFW_HOLD_BTN_BIT  (1u << 2)   /* BTN_2 = PC8 */

/* Poll cadence + debounce window, in ms (SysTick is 1 ms resolution). */
#define HOLD_POLL_INTERVAL_MS   20u
#define HOLD_DEBOUNCE_SAMPLES   3u    /* 3 x 20 ms = 60 ms */

static uint32_t s_last_poll_ms;
static bool     s_hold_pressed;       /* debounced output */
static uint32_t s_press_start_ms;     /* ms tick the state went HIGH */
static uint8_t  s_run_samples;        /* consecutive same-state samples */

void cfw_hold_trigger(void)
{
  s_last_poll_ms   = 0;
  s_hold_pressed   = false;
  s_press_start_ms = 0;
  s_run_samples    = 0;
}

void cfw_hold_trigger_poll(uint16_t btns)
{
  uint32_t now = cfw_tick_ms();
  if ((now - s_last_poll_ms) < HOLD_POLL_INTERVAL_MS)
    return;
  s_last_poll_ms = now;

  bool raw = (btns & CFW_HOLD_BTN_BIT) != 0;

  if (raw == s_hold_pressed) {
    s_run_samples = 0;               /* no edge; reset the run counter */
    return;
  }

  /* Edge: require N consecutive polls to agree before flipping. */
  if (++s_run_samples < HOLD_DEBOUNCE_SAMPLES)
    return;
  s_run_samples = 0;

  s_hold_pressed   = raw;
  s_press_start_ms = now;
}

bool cfw_hold_active(void)
{
  return s_hold_pressed;
}

/* Milliseconds the HOLD button has been held (0 when released).
 * Useful for a future long-press action (e.g. auto-cal bypass). */
uint32_t cfw_hold_pressed_for_ms(void)
{
  if (!s_hold_pressed)
    return 0;
  return cfw_tick_ms() - s_press_start_ms;
}
