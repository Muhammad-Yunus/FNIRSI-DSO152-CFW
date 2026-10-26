/*
 * lcd_st7789.h / lcd_st7789.c — ST7789 16-bit parallel TFT driver for the
 * DSO152, pure C, register-level port of David Given's dso152tft.h
 * (https://github.com/davidgiven/dso152-breakout).
 *
 * The original driver is C++ (Adafruit_GFX). This port drops the Adafruit
 * C++ dependency and the framebuffer abstraction, and talks directly to
 * the ST7789 over the DSO152's 16-bit parallel bit-bang interface:
 *
 *   LCD_CS  = PA15   (active low)   GPIOA
 *   LCD_RST = PA8    (active low)   GPIOA
 *   LCD_RS  = PC13   (0=command)    GPIOC
 *   LCD_WR  = PC14   (write strobe) GPIOC
 *   LCD_RD  = PC15   (read, unused) GPIOC
 *   LCD_BL  = PC9    (backlight, PWM)
 *   LCD_D[0:15] = PB[0:15]          GPIOB (16-bit data bus)
 *
 * The 16-bit data bus is wired to GPIOB, which is why dso152tft.h writes
 * 0x33333333 to CRL (PB0..7 input-float config) and 0x88888888 to CRH
 * (PB8..15 alt-out, push-pull) before toggling ODR directly.
 *
 * ST7789 init sequence: standard 320x240 portrait.
 */
#ifndef LCD_ST7789_H
#define LCD_ST7789_H

#include <stdint.h>
#include <stdbool.h>

/* Display geometry */
#define LCD_WIDTH   240
#define LCD_HEIGHT  320

/* Pin helpers (16-bit data bus on GPIOB) */
#define LCD_PORT    ((volatile uint32_t *)0x40011000u) /* GPIOB */
#define LCD_CRL     (*(volatile uint32_t *)(0x40011000u))  /* CRL */
#define LCD_CRH     (*(volatile uint32_t *)(0x40011004u))  /* CRH */
#define LCD_ODR     (*(volatile uint32_t *)(0x4001100Cu))  /* ODR */
#define LCD_BSRR    (*(volatile uint32_t *)(0x40011010u))  /* BSRR */

/* Control pins on GPIOC */
#define LCD_C_CRL   (*(volatile uint32_t *)(0x40011800u))  /* GPIOC CRL (PC0-7) */
#define LCD_C_CRH   (*(volatile uint32_t *)(0x40011804u))  /* GPIOC CRH (PC8-15) */
#define LCD_C_ODR   (*(volatile uint32_t *)(0x4001180Cu))  /* GPIOC ODR */
#define LCD_C_BSRR  (*(volatile uint32_t *)(0x40011810u))  /* GPIOC BSRR */

/* Control on GPIOA (CS=PA15, RST=PA8) */
#define LCD_A_CRH   (*(volatile uint32_t *)(0x40010804u))  /* GPIOA CRH (PA8-15) */
#define LCD_A_ODR   (*(volatile uint32_t *)(0x4001080Cu))  /* GPIOA ODR */
#define LCD_A_BSRR  (*(volatile uint32_t *)(0x40010810u))  /* GPIOA BSRR */

/* Pin bit positions (port-absolute, matching dso152tft.h) */
#define LCD_CS_BIT  15   /* PA15 */
#define LCD_RST_BIT 8    /* PA8  */
#define LCD_BL_BIT  9    /* PC9  */
#define LCD_RS_BIT  13   /* PC13 */
#define LCD_WR_BIT  14   /* PC14 */

/* GPIO nibble values (from ch32.h) */
#define N_GPIO_OUT_PP_50M  0x0Eu   /* CNF=11b, MODE=11b = 0xE push-pull out */
#define N_GPIO_OUT_AF_OD   0x0Au   /* CNF=10b, MODE=10b = 0xA alt open-drain */
#define N_GPIO_IN_FLOAT    0x00u

void lcd_init(void);
void lcd_set_brightness(uint8_t pct);
void lcd_fill(uint16_t color);
void lcd_draw_char(int x, int y, uint8_t chr, uint16_t bg, uint16_t fg);
void lcd_draw_string(int x, int y, const char *s, uint16_t bg, uint16_t fg);
void lcd_draw_waveform(const int16_t *samples, uint16_t n, int x_off,
                       int y_off, int h, uint16_t color);
void lcd_write_cmd(uint8_t cmd);
void lcd_write_data16(uint16_t val);

/* 16-bit palette (5-6-5 RGB565), same as dso152tft.h constants */
#define LCD_C_BLACK   0x0000
#define LCD_C_WHITE   0xFFFF
#define LCD_C_RED     0xF800
#define LCD_C_GREEN   0x07E0
#define LCD_C_BLUE    0x001F
#define LCD_C_YELLOW  0xFFE0
#define LCD_C_CYAN    0x07FF
#define LCD_C_MAGENTA 0xF81F
#define LCD_C_ORANGE  0xFD20
#define LCD_C_GRAY    0x8412

#endif /* LCD_ST7789_H */
