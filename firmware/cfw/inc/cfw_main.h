/*
 * cfw_main.h
 */
#ifndef CFW_MAIN_H
#define CFW_MAIN_H

#include <stdint.h>

void cfw_main(void);

/* CFW feature stubs */
void cfw_usb_stub(void);
void cfw_hold_trigger(void);
void cfw_calib_bypass(void);

/*
 * SysTick tick timer (1 ms).
 *   cfw_tick_init()  : configure + start the SysTick, zero the counter.
 *   cfw_tick_ms      : monotonically-increasing 1 ms counter, bumped in IRQ.
 *   cfw_delay_ms(n)  : block until n ms elapse (busy-wait on the counter).
 * Used by the main loop for time-based work; the SysTick_Handler that
 * services cfw_tick_ms lives in cfw_main.c and overrides the weak alias
 * defined in cfw_vectors.S.
 */
void  cfw_tick_init(void);
uint32_t cfw_tick_ms(void);
void  cfw_delay_ms(uint32_t nms);

#endif /* CFW_MAIN_H */
