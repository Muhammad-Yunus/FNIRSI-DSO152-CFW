/*
 * buttons.h
 */
#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>

/* 4x4 button matrix indices (row-major, 0..15) */


/* Returns 16-bit press bitmask; bit i = button i. */
uint16_t cfw_button_scan(void);

/* Fast digital game-control pins (PA10 left, PC8 right) */
uint8_t cfw_button_game_left(void);
uint8_t cfw_button_game_right(void);

#endif /* BUTTONS_H */
