/*
 * system_ch32.c — clock + GPIO + IRQ setup for the DSO152 CFW skeleton.
 *
 * Brings up the minimum the LCD driver and button ISR need:
 *   - HSE -> PLL -> 72 MHz SYSCLK (CH32F103, confirmed in our RE)
 *   - Flash latency 2WS
 *   - Enable GPIOA/B/C + AFIO + ADC clocks
 *   - Set VTOR to the flash vector table (our .cfv_vector_table @ 0x08000000)
 */
#include "ch32.h"
#include <stdint.h>

/* Raw peripheral base offsets used by the skeleton. */
#define RCC_CR_REG      (*(volatile uint32_t *)(RCC_BASE + 0x00u))
#define RCC_PLLCFGR     (*(volatile uint32_t *)(RCC_BASE + 0x08u))
#define RCC_CFGR_REG    (*(volatile uint32_t *)(RCC_BASE + 0x04u))
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x40u))
#define RCC_APB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x54u))
#define RCC_CR_HSERDY   (1u << 17)

/* SCB->VTOR lives at fixed Cortex-M3 address 0xE000ED08. */
#define SCB_VTOR        (*(volatile uint32_t *)0xE000ED08u)
#define FLASH_VECTORS   ((uint32_t)0x08000000u)

void system_init(void)
{
  /* 1. Enable HSE (external 8 MHz crystal). */
  RCC_CR_REG |= RCC_CR_HSEON;
  while (!(RCC_CR_REG & RCC_CR_HSERDY)) {}

  /* 2. Configure PLL: HSE x9 = 72 MHz. The value 0x08D8A016 matches the
   *    PLLCFGR we recovered from the stock .bin, so it is known-good. */
  RCC_PLLCFGR = 0x08D8A016u;
  RCC_CR_REG |= RCC_CR_PLLON;
  while (!(RCC_CR_REG & RCC_CR_PLLRDY)) {}

  /* 3. Select HSE/PLL as SYSCLK. */
  RCC_CFGR_REG |= RCC_CFGR_HSEEXTSEL;

  /* 4. Flash latency: 2 wait states for 72 MHz HCLK. */
  FLASH->ACR = FLASH_ACR_LATENCY_2WS;

  /* 5. Enable GPIOA/B/C + AFIO clocks. */
  RCC_APB2ENR |= RCC_APB2ENR_GPIOAEN
               | RCC_APB2ENR_GPIOBEN
               | RCC_APB2ENR_GPIOCEN
               | RCC_APB2ENR_AFIOEN;

  /* 6. Enable ADC clocks (waveform input). */
  RCC_APB1ENR |= RCC_APB1ENR_ADC1EN | RCC_APB1ENR_ADC2EN;
}

/* Point the CPU at our flash vector table. The CPU's default VTOR at
 * reset already targets the start of code, but we write it explicitly so
 * the override of stock IRQ handlers (USB/ADC) is unambiguous. */
void cfw_redirect_vectors(void)
{
  SCB_VTOR = FLASH_VECTORS;
}
