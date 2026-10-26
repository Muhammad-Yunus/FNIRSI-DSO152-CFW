/*
 * cfw_init.c — .data/.bss initialization helpers used by the reset path.
 *
 * The linker script (cfw.ld) emits _data_load_start/_data_start/_data_end
 * (the FLASH-backed copy of initialized globals and its SRAM destination)
 * and _bss_start/_bss_end (zeroed region). Copy + clear run before
 * main; they are weak so a build that doesn't need them still links.
 */
#include "ch32.h"

extern uint32_t _data_load_start;
extern uint32_t _data_start;
extern uint32_t _data_end;
extern uint32_t _bss_start;
extern uint32_t _bss_end;

void cfw_init_data(void)
{
  uint32_t *dst = (uint32_t *)&_data_start;
  const uint32_t *src = (const uint32_t *)&_data_load_start;
  const uint32_t *stop = (const uint32_t *)&_data_end;
  while (dst < stop)
    *dst++ = *src++;

  dst = (uint32_t *)&_bss_start;
  const uint32_t *bss_stop = (const uint32_t *)&_bss_end;
  while (dst < bss_stop)
    *dst++ = 0u;
}
