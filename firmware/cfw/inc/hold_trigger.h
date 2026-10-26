/*
 * hold_trigger.h — HOLD (trace-freeze) button state machine.
 *
 * The stock DSO152 has a momentary HOLD button that freezes the trace
 * while pressed. This CFW reimplementation works off the polled button
 * bitmask from cfw_button_scan(), debounced against the 1 ms SysTick
 * time base, and exposes cfw_hold_active() so the main loop can gate
 * waveform redraw while the user is holding the trace.
 *
 * Button mapping is a placeholder: the full 4x4 ADC matrix scan is not
 * yet implemented (see buttons.c), so HOLD is wired to BTN_2, the
 * legacy "right" game pin (PC8) already surfaced by cfw_button_scan().
 */
#ifndef HOLD_TRIGGER_H
#define HOLD_TRIGGER_H

#include <stdint.h>
#include <stdbool.h>

/* Called once at boot before the main loop starts. Zeroes internal
 * state so the first poll is well-defined. */
void cfw_hold_trigger(void);

/* Called every main-loop iteration with the raw cfw_button_scan()
 * result. Internally debounces the HOLD button against cfw_tick_ms(). */
void cfw_hold_trigger_poll(uint16_t btns);

/* True while the (debounced) HOLD button is pressed — trace should be
 * frozen; main loop should skip waveform refresh. */
bool cfw_hold_active(void);

/* Milliseconds the HOLD button has been held (0 when released). */
uint32_t cfw_hold_pressed_for_ms(void);

#endif /* HOLD_TRIGGER_H */
