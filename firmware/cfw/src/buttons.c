/*
 * buttons.c — DSO152 button handling, pure C.
 *
 * The DSO152 has a 4x5 button matrix wired through the ADC:
 *   - COL[0:3] = 4 column driver lines
 *   - ROW[0:3] = 4 row sense lines
 *   - Each button connects a col to a row; the ADC reads the row's
 *     voltage divider to identify which col is driven.
 *
 * In dso152tft.h (David Given's breakout) the "buttons" were mapped to
 * the PA10/PC8 left/right pins for game control. But for a scope, the
 * real UI uses the matrix.
 *
 * This file implements both:
 *   - cfw_button_scan()        : poll the matrix via ADC (full CFW UI)
 *   - cfw_button_game()        : PA10/PC8 fast digital (game/legacy mode)
 *
 * The CFW main loop will call cfw_button_scan() each frame. The ADC
 * readout requires the ADC to be initialized and the correct row pin to
 * be driven high before sampling.
 */
#include "ch32.h"
#include "buttons.h"

/* Button matrix mapping (from RE of the stock .bin) */
enum {
  BTN_1 = 0, /* bottom-left of 4x4 */
  BTN_2,
  BTN_3,
  BTN_4,
  BTN_5,
  BTN_6,
  BTN_7,
  BTN_8,
  BTN_9,
  BTN_10,
  BTN_11,
  BTN_12,
  BTN_13,
  BTN_14,
  BTN_15,
  BTN_16,
  BTN_COUNT
};

/* Legacy game-control pins (dso152tft.h) */
#define GAME_LEFT  ((GPIO_TypeDef *)GPIOA_BASE)
#define GAME_LEFT_BIT  10
#define GAME_RIGHT ((GPIO_TypeDef *)GPIOC_BASE)
#define GAME_RIGHT_BIT 8

/* Poll the 4x4 button matrix via the ADC.
 *
 * For each row (0..3):
 *   - Drive the row output high
 *   - Read the ADC on the column sense pin
 *   - The voltage divider ratio identifies which col was driven
 *   - Map (row, col) -> button index 0..15
 *
 * Returns a 16-bit bitmask (bit i = button i pressed).
 * This is a stub that returns 0; a full build would wire up the ADC.
 */
uint16_t cfw_button_scan(void)
{
  /*
   * Full ADC matrix scan is not in the skeleton. For now, return the
   * legacy game pins (PA10/PC8) which the breakout driver also used,
   * so the CFW at minimum has a UI path that works with the breakout
   * wiring.
   */
  uint16_t state = 0;
  if (GAME_LEFT->IDR & (1u << GAME_LEFT_BIT))
    state |= (1u << BTN_1); /* PA10 mapped to BTN_1 placeholder */
  if (GAME_RIGHT->IDR & (1u << GAME_RIGHT_BIT))
    state |= (1u << BTN_2); /* PC8 mapped to BTN_2 placeholder */
  return state;
}

/* Fast digital read of just the two game-control pins. */
uint8_t cfw_button_game_left(void)  { return GAME_LEFT->IDR  & (1u << GAME_LEFT_BIT); }
uint8_t cfw_button_game_right(void) { return GAME_RIGHT->IDR & (1u << GAME_RIGHT_BIT); }
