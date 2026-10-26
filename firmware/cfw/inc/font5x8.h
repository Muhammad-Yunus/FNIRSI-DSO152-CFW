/*
 * font5x8.h — minimal 5x8 font table declaration.
 *
 * Provides the classic 5x8 ASCII font used by ST7789 TFT code. The
 * CFW skeleton only *declares* it (so lcd_draw_char can reference it
 * later); a full build would ship the table. Kept external so the
 * translation unit is small.
 */
#ifndef FONT5X8_H
#define FONT5X8_H

#include <stdint.h>

/* 5x8 font: 5 bytes/char x 128 ASCII chars = 640 bytes */
extern const uint8_t font5x8[128][5];

#endif /* FONT5X8_H */
