/*
 * lcd_st7789.c — register-level ST7789 driver, pure C.
 *
 * Ported from David Given's dso152tft.h. The bit-bang timing constants
 * are copied verbatim from that driver. The 16-bit data bus is driven by
 * toggling GPIOB->ODR directly (fast), while the control signals use the
 * slower CS/RS/WR strobes.
 *
 * DSO152 wiring (confirmed by peripheral-map.md):
 *   Data  D[0:15]  -> GPIOB PB0..PB15
 *   CS    PA15, RST PA8 (GPIOA)
 *   RS/WR/RD/BL -> PC13/PC14/PC15/PC9 (GPIOC)
 *
 * The driver assumes clocks + GPIOB/C/A + AFIO are enabled by the
 * startup code (see cfw_main.c / system_ch32.c).
 */
#include "ch32.h"
#include "lcd_st7789.h"
#include "font5x8.h"

/* Bit-bang strobe delays (from dso152tft.h) */
#define LCD_CS_HIGH  (GPIOA->BSRR = (1u << (LCD_CS_BIT + 16)))
#define LCD_CS_LOW   (GPIOA->BSRR = (1u << LCD_CS_BIT))
#define LCD_WR_HIGH  (GPIOC->BSRR = (1u << (LCD_WR_BIT + 16)))
#define LCD_WR_LOW   (GPIOC->BSRR = (1u << LCD_WR_BIT))
#define LCD_RS_HIGH  (GPIOC->BSRR = (1u << (LCD_RS_BIT + 16)))
#define LCD_RS_LOW   (GPIOC->BSRR = (1u << LCD_RS_BIT))
#define LCD_RST_HIGH (GPIOA->BSRR = (1u << (LCD_RST_BIT + 16)))
#define LCD_RST_LOW  (GPIOA->BSRR = (1u << LCD_RST_BIT))

static inline void lcd_wait(void) { /* ST7789 needs ~1-2 us between strobes */
  __asm__ volatile ("nop\n\t nop\n\t nop\n\t nop\n\t nop\n\t nop" :::);
}

/* Write a 16-bit word on the data bus */
static void lcd_write16(uint16_t w)
{
  LCD_ODR = w;
  lcd_wait();
  LCD_WR_LOW;
  lcd_wait();
  LCD_WR_HIGH;
}

/* ---- internal command/data helpers ---------------------------------- */
void lcd_write_cmd(uint8_t cmd)
{
  LCD_RS_LOW;
  lcd_wait();
  LCD_CS_LOW;
  lcd_write16((uint16_t)cmd);
  LCD_CS_HIGH;
}

void lcd_write_data16(uint16_t val)
{
  LCD_RS_HIGH;
  lcd_wait();
  LCD_CS_LOW;
  lcd_write16(val);
  LCD_CS_HIGH;
}

/* ---- hardware init --------------------------------------------------- */
void lcd_init(void)
{
  /* GPIOB: all 16 data pins as 50 MHz push-pull outputs.
   * CRL = 0x33333333 (input-float config for low half, per dso152tft.h
   * quirk) then CRH = 0x88888888 to set PB8-15 as 50M push-pull. The
   * final ODR writes drive the actual levels.
   *
   * We keep the same values as dso152tft.h for bit-exact behavior.
   */
  LCD_CRL = 0x33333333u;
  LCD_CRH = 0x88888888u;
  LCD_ODR = 0x0000;

  /* GPIOC: PC13/PC14/PC15 (RS/WR/RD) as 50M push-pull; PC9 (BL) as AF
   * open-drain for PWM. CRL/CRH nibbles from dso152tft.h.
   */
  LCD_C_CRL = 0x33333333u;
  LCD_C_CRH = 0x33333333u;

  /* GPIOA: PA15 (CS) + PA8 (RST) as 50M push-pull. CRH covers PA8-PA15.
   */
  LCD_A_CRH = 0x33333333u;

  /* Pull CS/RST high (inactive). */
  LCD_CS_HIGH;
  LCD_RST_HIGH;

  /* Hardware reset: ST7789 wants RST low for >= 10 us, then high.
   * The 1 ms delay is a no-op here (we don't have ms delay yet in the
   * skeleton); just do a few iterations.
   */
  for (volatile int i = 0; i < 2000; i++) {}
  LCD_RST_LOW;
  for (volatile int i = 0; i < 2000; i++) {}
  LCD_RST_HIGH;

  /* ST7789 init sequence (from dso152tft.h / common ST7789 init) */
  lcd_write_cmd(0x01); lcd_wait(); lcd_write_cmd(0x11);
  lcd_write_cmd(0x36); lcd_write_data16(0x0000);
  lcd_write_cmd(0x3A); lcd_write_data16(0x0505);
  lcd_write_cmd(0xB2);
  lcd_write_data16(0x0000); lcd_write_data16(0x0000);
  lcd_write_data16(0x0018);
  lcd_write_cmd(0xB7); lcd_write_data16(0x0505);
  lcd_write_cmd(0xBB); lcd_write_data16(0x2020);
  lcd_write_cmd(0xC0); lcd_write_data16(0x0000);
  lcd_write_cmd(0xC2); lcd_write_data16(0x0F03);
  lcd_write_cmd(0xC3); lcd_write_data16(0x1414);
  lcd_write_cmd(0xC4); lcd_write_data16(0x2020);
  lcd_write_cmd(0xC6); lcd_write_data16(0x1313);
  lcd_write_cmd(0xD0); lcd_write_data16(0xA4A4);
  lcd_write_cmd(0xE0);
  lcd_write_data16(0x1000); lcd_write_data16(0x0A0A); lcd_write_data16(0x0F0F);
  lcd_write_data16(0x0F0F); lcd_write_data16(0x1717); lcd_write_data16(0x0B0B);
  lcd_write_cmd(0xE1);
  lcd_write_data16(0x1000); lcd_write_data16(0x0909); lcd_write_data16(0x1111);
  lcd_write_data16(0x1111); lcd_write_data16(0x1414); lcd_write_data16(0x0B0B);
  lcd_write_cmd(0x21);
  lcd_write_cmd(0x13);
  lcd_write_cmd(0x29);
}

void lcd_set_brightness(uint8_t pct)
{
  /* Simple: PC9 toggled at a duty derived from pct. In the skeleton we
   * just drive it high for pct>=50 else low; a real PWM would use a TIM.
   */
  if (pct >= 50) LCD_C_BSRR = (1u << (LCD_BL_BIT + 16));
  else            LCD_C_BSRR = (1u << LCD_BL_BIT);
}

/* ---- frame-buffer-optional fill (writes raw pixels) ----------------- */
void lcd_fill(uint16_t color)
{
  /* Set window to full 320x240, column 0..239, row 0..319. */
  lcd_write_cmd(0x2A);
  lcd_write_data16(0x0000); lcd_write_data16(0x00F0); /* col 0..239 */
  lcd_write_cmd(0x2B);
  lcd_write_data16(0x0000); lcd_write_data16(0x01FF); /* row 0..319 */
  lcd_write_cmd(0x2C);
  for (int i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++)
    lcd_write_data16(color);
}

/* ---- 5x8 font rendering -------------------------------------------- */
void lcd_draw_char(int x, int y, uint8_t chr, uint16_t bg, uint16_t fg)
{
  (void)x; (void)y; (void)chr; (void)bg; (void)fg;
  /*
   * Real 5x8 font rendering needs a frame-buffer + windowed pixel write.
   * The skeleton stubs it so the build is complete; a full port would:
   *   const uint8_t *g = font5x8[chr & 0x7F];
   *   for (byte row=0..7) for (col=0..4) set pixel
   * via setAddressWindow + writeColor, in BG->FG color.
   */
}

void lcd_draw_string(int x, int y, const char *s, uint16_t bg, uint16_t fg)
{
  while (*s) lcd_draw_char(x, y, (uint8_t)(*s++), bg, fg);
}

/* ---- waveform trace (the actual oscilloscope hook) ------------------ */
void lcd_draw_waveform(const int16_t *samples, uint16_t n, int x_off,
                       int y_off, int h, uint16_t color)
{
  /*
   * Draw a raw 12-bit-ish waveform across the screen width.
   *   x_off/y_off = top-left of the trace region
   *   h           = trace height in pixels
   *   n           = number of samples (mapped linearly across width)
   *
   * Skeleton: for each sample compute a vertical pixel offset and write
   * a 1px column via windowed writeColor. The full implementation would
   * batch into setAddressWindow(x_off, y_off, x_off+width, y_off+h).
   */
  (void)samples; (void)n; (void)x_off; (void)y_off; (void)h; (void)color;
}
