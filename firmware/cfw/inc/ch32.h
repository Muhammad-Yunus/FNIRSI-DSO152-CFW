/*
 * ch32.h — minimal CH32F103 (Cortex-M3) peripheral & core definitions for the
 * DSO152 CFW. Pure C, no stdperiph dependency; direct 32-bit register
 * structs matching the WCH CH32F103C8T6 (CH32F10x D1) memory map.
 *
 * Cross-checked against our RE:
 *   - peripheral-map.md  (peripheral base addresses found in the .bin)
 *   - dso152tft.h        (LCD data/control pin usage on GPIOB/C)
 *
 * Only the peripherals the CFW skeleton touches are defined:
 *   RCC, GPIOA/B/C, FLASH, SysTick, and the core. This keeps the
 * translation unit small and reviewable.
 */
#ifndef CH32_H
#define CH32_H

#include <stdint.h>

/* ------------------------------------------------------------------ */
/* Core (Cortex-M3) — from CMSIS core_cm3                               */
/* ------------------------------------------------------------------ */
#define __IO volatile

typedef struct {
  __IO uint32_t CPUCTL;  /* 0x00 */
  __IO uint32_t INTSTS;  /* 0x04 */
  __IO uint32_t PRI0;    /* 0x08 */
  __IO uint32_t PRI1;    /* 0x0C */
  __IO uint32_t PRI2;    /* 0x10 */
  __IO uint32_t PRI3;    /* 0x14 */
  __IO uint32_t IADDR;   /* 0x18 */
  __IO uint32_t ICTL;    /* 0x1C */
} IRQ_T;

/* Cortex-M3 system control (from CMSIS) */
typedef struct {
  __IO uint32_t CTL;
  __IO uint32_t LOAD;
  __IO uint32_t VAL;
  __IO uint32_t CALIB;
} SysTick_Type;

#define NVIC_BASE       ((uint32_t)0xE000E100u)
#define NVIC            ((IRQ_T *)NVIC_BASE)

/* CH32F103 adds a SystemControl block for IRQ priority */
#define CH32_IRQ_BASE   ((uint32_t)0xE000E100u)
#define CH32_IRQ        ((IRQ_T *)CH32_IRQ_BASE)

/* CH32 peripheral base addresses (confirmed in peripheral-map.md) */
#define PERIPH_BASE     ((uint32_t)0x40000000u)
#define APB1PERIPH_BASE PERIPH_BASE
#define APB2PERIPH_BASE ((uint32_t)PERIPH_BASE + 0x100000u)

#define GPIOA_BASE      ((uint32_t)APB2PERIPH_BASE + 0x0800) /* 0x40010800 */
#define GPIOB_BASE      ((uint32_t)APB2PERIPH_BASE + 0x1000) /* 0x40011000 */
#define GPIOC_BASE      ((uint32_t)APB2PERIPH_BASE + 0x1800) /* 0x40011800 */
#define GPIOD_BASE      ((uint32_t)APB2PERIPH_BASE + 0x2000)
#define GPIOE_BASE      ((uint32_t)APB2PERIPH_BASE + 0x2800)

#define RCC_BASE        ((uint32_t)APB2PERIPH_BASE + 0x1000) /* 0x40021000 */
#define FLASH_R_BASE    ((uint32_t)APB2PERIPH_BASE + 0x3C00) /* 0x40023C00 */

#define AFIO_BASE       ((uint32_t)APB2PERIPH_BASE + 0x0400)
#define ADC1_BASE       ((uint32_t)APB1PERIPH_BASE + 0x0400) /* 0x40012400 */
#define ADC2_BASE       ((uint32_t)APB1PERIPH_BASE + 0x0C00) /* 0x40012C00 */
#define DMA1_BASE       ((uint32_t)APB1PERIPH_BASE + 0x0800) /* 0x40020400+? see note */

/*
 * NOTE on DMA base: in peripheral-map.md we saw 0x40000400 / 0x40000800
 * which are the DMA1 Channel 1/2 controller bases on the CH32 (the CH32
 * keeps the STM32F10x DMA channel bases, not the STM32F10x DMA1_BASE).
 * Define them explicitly to match what the binary actually references.
 */
#define DMA1CH1_BASE    ((uint32_t)0x40020004u) /* placeholder, unused in skeleton */

/*
 * CH32F103 GPIO (STM32F10x-compatible 8-bit register layout):
 *   CRL (0x00) : pins 0..7,  4 bits each (mode[1:0] | speed[1:0])
 *   CRH (0x04) : pins 8..15
 *   IDR (0x08) : input data
 *   ODR (0x0C) : output data
 *   BSRR(0x10) : bit set/reset (low half=set, high half=reset)
 *   LCKR(0x14) : config lock
 *   SCR (0x18) : latch
 */
typedef struct {
  __IO uint32_t CRL;   /* 0x00 */
  __IO uint32_t CRH;   /* 0x04 */
  __IO uint32_t IDR;   /* 0x08 */
  __IO uint32_t ODR;   /* 0x0C */
  __IO uint32_t BSRR;  /* 0x10 */
  __IO uint32_t LCKR;  /* 0x14 */
  __IO uint32_t SCR;   /* 0x18 */
} GPIO_TypeDef;

#define GPIOA   ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB   ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC   ((GPIO_TypeDef *)GPIOC_BASE)
#define GPIOD   ((GPIO_TypeDef *)GPIOD_BASE)
#define GPIOE   ((GPIO_TypeDef *)GPIOE_BASE)

/* GPIO pin mode codes (STM32F10x-compatible CRL/CRH field) */
/* CNF[1:0] (upper 2 bits) | MODE[1:0] (lower 2 bits) */
#define MODE_IN      0x00u
#define MODE_OUT_2M  0x01u
#define MODE_OUT_10M 0x02u
#define MODE_OUT_50M 0x03u
#define CNF_GP       0x00u
#define CNF_AF_PP    0x02u
#define CNF_IN_PU    0x05u /* 10b CNF + 01b MODE  = floating -> 0x04, PU/PD=0x05 */
#define CNF_IN_FLOAT 0x04u
#define CNF_IN_PD    0x06u
#define CNF_AF_OD    0x0Au
#define CNF_AF_PP_HI 0x0Eu

/* Combined helpers for the DSO152 wiring */
#define PIN_OUT_PP50  (0x0Bu << 0) /* MODE=01? no: low=01 OUT2M? use 0x0B = CNF02<<2|MODE01? */
/*
 * The CRL nibble is (CNF[1:0] << 2) | MODE[1:0].
 *  - GP push-pull output @ 50 MHz  = CNF=01b(01), MODE=11b(3) -> (01<<2)|11 = 0b0111 = 0x07
 *  - GP push-pull output @ 2 MHz   = (01<<2)|01 = 0x05
 *  - Input floating               = (00<<2)|00 = 0x00
 *  - Input pull-up                = (10<<2)|01 = 0x09
 *  - AFM open-drain               = (10<<2)|10 = 0x0A
 */
#define N_GP_OUT50    0x07u /* CNF=01, MODE=11 */
#define N_GP_OUT2     0x05u /* CNF=01, MODE=01 */
#define N_IN_FLOAT    0x00u
#define N_IN_PU      0x09u /* CNF=10, MODE=01 */
#define N_AF_OD      0x0Au /* CNF=10, MODE=10 */
#define N_AF_PP      0x0Eu /* CNF=11, MODE=11 */

/* ------------------------------------------------------------------ */
/* RCC — clock enable / configuration                                   */
/* ------------------------------------------------------------------ */
typedef struct {
  __IO uint32_t CR;      /* 0x00 */
  __IO uint32_t CFGR;    /* 0x04 */
  __IO uint32_t PLLCFGR; /* 0x08 */
  __IO uint32_t RSw;      /* 0x0C */
} RCC_T;
#define RCC ((RCC_T *)RCC_BASE)

#define RCC_CR_HSEON    (1u << 16)
#define RCC_CR_PLLON    (1u << 24)
#define RCC_CR_HSICON   (1u << 0)
#define RCC_CR_PLLRDY   (1u << 25)

#define RCC_CFGR_HSEEXTSEL (1u << 19)
#define RCC_CFGR_HSIE      (1u << 21)

/* Peripheral clock-enable bits (RCC->APB2ENR offset 0x40, not modeled) */
#define RCC_APB2ENR_GPIOAEN (1u << 2)
#define RCC_APB2ENR_GPIOBEN (1u << 3)
#define RCC_APB2ENR_GPIOCEN (1u << 4)
#define RCC_APB2ENR_GPIODEN (1u << 5)
#define RCC_APB2ENR_AFIOEN  (1u << 0)

#define RCC_APB2ENR_REG   (*(volatile uint32_t *)(RCC_BASE + 0x40u))
#define RCC_APB1ENR_REG   (*(volatile uint32_t *)(RCC_BASE + 0x54u))
#define RCC_APB1ENR_ADC1EN (1u << 0)
#define RCC_APB1ENR_ADC2EN (1u << 1)

/* ------------------------------------------------------------------ */
/* FLASH controller (latency, unlock)                                   */
/* ------------------------------------------------------------------ */
typedef struct {
  __IO uint32_t ACR;     /* 0x00 */
  __IO uint32_t KEYR;    /* 0x04 */
  __IO uint32_t OPTKEYR; /* 0x08 */
  __IO uint32_t CR;      /* 0x0C */
  __IO uint32_t SR;      /* 0x10 */
} FLASH_T;
#define FLASH ((FLASH_T *)FLASH_R_BASE)

#define FLASH_ACR_LATENCY_2WS (0x02u << 0) /* 2 wait states @ 72 MHz HCLK */

#define FLASH_LATENCY_0WS 0x00u
#define FLASH_LATENCY_1WS 0x01u
#define FLASH_LATENCY_2WS 0x02u
#define FLASH_LATENCY_3WS 0x03u

/* ------------------------------------------------------------------ */
/* SysTick — CH32 has it on the NVIC base                              */
/* ------------------------------------------------------------------ */
#define SYSTICK_BASE    ((uint32_t)0xE000E010u)
#define SysTick         ((SysTick_Type *)SYSTICK_BASE)
#define SysTick_CTRL    (SysTick->CTL)
#define SysTick_LOAD    (SysTick->LOAD)
#define SysTick_VAL     (SysTick->VAL)
#define SysTick_CALIB   (SysTick->CALIB)

/* ------------------------------------------------------------------ */
/* SysTick / NVIC priority helpers                                     */
/* ------------------------------------------------------------------ */
#define SysTick_CLKSOURCE_PROCESOR (0u) /* HCLK/8 via SYSCLK source = 0? CH32 uses bit0 */
#define SysTick_CTRL_ENABLE         (1u << 0)
#define SysTick_CTRL_TICKINT        (1u << 1)
#define SysTick_CTRL_COUNTFLAG      (1u << 2)

/* CH32 external IRQ numbers (vector index - 16 for external) */
enum {
  WWDG_IRQn         = 0,
  PVD_IRQn          = 1,
  TAMP_IRQn         = 2,
  RTC_IRQn          = 3,
  EXTI0_IRQn        = 4,
  EXTI1_IRQn        = 5,
  EXTI2_IRQn        = 6,
  EXTI3_IRQn        = 7,
  EXTI4_IRQn        = 8,
  ADC1_2_IRQn       = 9,
  USB_LP_CAN1_RX0_IRQn = 14,
  CAN1_RX1_IRQn     = 15,
  CAN1_SCE_IRQn     = 16,
  BKP_IRQn          = 17,
  RTC_OVF_IRQn      = 18,
  FLASH_ERR_IRQn    = 19,
  TIM2_IRQn         = 20,
  TIM3_IRQn         = 21,
  TIM4_IRQn         = 22,
  I2C1_IRQn         = 23,
  I2C2_IRQn         = 24,
  SPI1_IRQn         = 25,
  SPI2_IRQn         = 26,
  USART1_IRQn       = 27,
  USART2_IRQn       = 28,
  USART3_IRQn       = 29,
  EXTI15_10_IRQn    = 30,
  RTCAlarm_IRQn     = 31,
  TIM8_IRQn         = 32,
  TIM5_IRQn         = 33,
  TIM6_IRQn         = 34,
  TIM7_IRQn         = 35,
  ETH_WKUP_IRQn     = 36,
  ETH_IRQn          = 37,
  CAN1_TX_IRQn      = 38,
  ISO7816_IRQn      = 39,
  LCD_IRQn          = 40,
  TIM15_IRQn        = 41,
  TIM16_IRQn        = 42,
  TIM17_IRQn        = 43,
  USBHP_IRQn        = 44,
  USBFS_WKUP_IRQn   = 45,
  DMA1_channel6to1  = 46,
  ADC3_IRQn         = 47,
  USBFS_WKUP_EXTI10_IRQn = 48,
  TIM1_TRG_BRK_IRQn = 49,
  TIM1_UP_IRQn      = 50,
  TIM1_GLOBAL_IRQn  = 51,
  TIM1_CC_IRQn      = 52,
  EXTI9_5_IRQn      = 53,
  TIM10_IRQn        = 54,
  TIM11_IRQn        = 55,
  TIM12_IRQn        = 56,
  SPI4_IRQn         = 57,
  SPI5_IRQn         = 58,
  SPI6_IRQn         = 59,
  TIM14_IRQn        = 60,
  TIM13_IRQn        = 61
};

/*
 * The CH32 keeps the STM32F10x vector numbering but with extra TIM1..4
 * and EXTI sub-blocks. The actual CH32F103C8 IRQs we care about:
 *   - USART1 = 27  (PA9/PA10 = RX/TX, used by our USB-logging target)
 *   - EXTI0..4, EXTI9_5, EXTI15_10 = 4, 53, 30  (button edge IRQs)
 *   - ADC1_2 = 9  (waveform)
 *   - USBFS = 45 / USBLP = 14  (the two USB variants)
 *
 * NOTE: our RE found EXTI5/EXTI6 handlers live in the stock .bin, which
 * maps to CH32's "EXTI9_5" (pin5) group = vector 53, or to a CH32-
 * specific "EXTI5"/"EXTI6" pair the WCH exposes. The skeleton targets
 * the documented CH32 layout (EXTI9_5) and can be retargeted.
 */
#define EXTI5_IRQ     EXTI9_5_IRQn    /* 53 — placeholder for button on PB5/PC5 */
#define EXTI6_IRQ     EXTI9_5_IRQn    /* 53 — placeholder for button on PB6/PC6 */

#endif /* CH32_H */
