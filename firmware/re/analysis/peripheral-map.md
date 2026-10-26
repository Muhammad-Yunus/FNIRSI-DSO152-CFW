# FNIRSI DSO152 — Peripheral & Memory Map (CH32F103C8T6)

## Architecture Summary
| Field | Value |
|-------|-------|
| MCU | CH32F103C8T6 (ARM Cortex-M3, 72 MHz, 64 KB flash, 20 KB RAM) |
| Endianness | Little |
| CPU mode | Thumb (LSB = 1 on all handlers) |
| Flash base | `0x08000000` |
| RAM base | `0x20000000` |
| Initial SP | `0x20004F68` (top of 20 KB RAM, 256 B below = 0x20004F68) |
| Reset handler | `0x08003DD4` |
| Binary size | 39,532 bytes (< 64 KB flash ✓) |
| SHA-256 | `4b79cb402d3e453f01f1b2a72a08349c770d2bf777c0899bc09b0fea26b2631e` |

## Vector Table (First 60 Entries)

| idx | VMA | Raw | IRQ (CH32F103 names) |
|-----|-----|-----|----------------------|
| 0 | 0x08000000 | 0x20004F68 | **Initial SP** |
| 1 | 0x08000004 | 0x08003DD5 | **Reset_Handler** |
| 2 | 0x08000008 | 0x080085C1 | NMI_Handler |
| 3 | 0x0800000C | 0x08007FDB | HardFault_Handler |
| 4 | 0x08000010 | 0x080085BD | MemManage_Handler |
| 5 | 0x08000014 | 0x080046C5 | BusFault_Handler |
| 6 | 0x08000018 | 0x08008BB3 | UsageFault_Handler |
| 7 | 0x0800001C | 0x00000000 | (reserved) |
| 8 | 0x08000020 | 0x00000000 | (reserved) |
| 9 | 0x08000024 | 0x00000000 | (reserved) |
| 10 | 0x08000028 | 0x00000000 | (reserved) |
| 11 | 0x0800002C | 0x080085C5 | **SysTick_Handler** |
| 12 | 0x08000030 | 0x08007741 | **ExtInt4_IRQHandler (RTC/Tamper)** |
| 13 | 0x08000034 | 0x00000000 | |
| 14 | 0x08000038 | 0x080085C3 | **ExtInt5_IRQHandler** |
| 15 | 0x0800003C | 0x08008BB1 | **ExtInt6_IRQHandler** |
| 16–44 | 0x08000040–B4 | 0x08003DEF | DefaultHandler (shared weak alias) |
| 45 | 0x080000B4 | 0x0800B60D | **USB_LP_CAN1_RX0_IRQHandler** ← USB! |
| 46–59 | 0x080000B8–EC | 0x08003DEF | DefaultHandler |

> **Key**: idx 45 = `USB_LP_CAN1_RX0` is the only non-default external IRQ with a real
> handler (`0x0800B60D`), confirming the firmware does use the USB peripheral.

## Peripheral Base Addresses Found in Binary (from periph-scan)

| Address | Peripheral | Occurrences (VMA offsets) |
|---------|------------|---------------------------|
| `0x40010800` | **GPIOB** | 0x0800093C, 0x08001230, 0x08001688, 0x08001730, 0x080029B0, 0x08002DF0, 0x08007128, 0x08007614, 0x08007EA8 |
| `0x40011000` | **GPIOC** | 0x08002DBC, 0x080043F8, 0x080049B4, 0x08004FA0, 0x08006500, 0x080066A8, 0x08007124, 0x0800717C, 0x08007214, 0x08007218, 0x08007E58, 0x08007EAC, 0x08007F2C |
| `0x40012400` | **ADC1** | 0x08000888, 0x08000940, 0x080029A8 |
| `0x4001244C` | **ADC1->JDR1** (offset 0x4C) | 0x08000ABC, 0x08000DEC |
| `0x40012C00` | **ADC2** | 0x080078C0, 0x08007B18, 0x08007BC0, 0x08007C24 |
| `0x40010C0C` | **GPIOE->CRL** (offset 0xC) | 0x080043FC, 0x080049B8, 0x08004FA4, 0x0800650C, 0x080066AC, 0x08007180, 0x0800721C |
| `0x40010C00` | **GPIOE** base | 0x0800721C |
| `0x40010810` | **GPIOB->CRH** (offset 0x10) | 0x080074D4 |
| `0x40021000` | **RCC->CR** | 0x0800762C–0x08007830 (6×), 0x080077B0 |
| `0x40022000` | **FLASH->ACR** | 0x08003C74–0x08003D1C (5×), 0x080077B4 |
| `0x40020008` | **RCC->CFGR** (offset 0x8) | 0x08000AC0, 0x08000C24, 0x08000DF0, 0x08001B40, 0x080028D4 |
| `0x40020408` | **RCC->APB2ENR** (offset 0x40+8) | 0x08000C28 |
| `0x40010800` | **GPIOB** base (again) | 0x0800093C, 0x08001230, … |
| `0x40000400` | **DMA1 Channel base** | 0x08007A08, 0x08007B1C, 0x08007C28 |
| `0x40000800` | **DMA1 Channel 2?** | 0x08007B20, 0x08007C2C |
| `0x4000F04F` | **USB? (PCC?)** | 0x080095FC |
| `0x4000C07F` | **AFIO? / PWD?** | 0x08004534, 0x0800453C |
| `0x400040FF` | **GPIO? (PCLK?)** | 0x08004540 |
| `0x40000068` | **FLASH? (offset 0x68)** | 0x080045A4 |
| `0x40000810` | **FLASH? (offset 0x10)** | 0x080045BC |
| `0x40000000` | **FLASH base / PRC?** | 0x080045E8, 0x08009808 |
| `0x401CE8BD` | off-range literal (code) | 0x080002FC |
| `0x4010E8BD` | off-range literal (code) | 0x080005E8–0x080095FC |

### Inferred Peripherals in Use
| Peripheral | Evidence |
|------------|----------|
| **RCC (clock tree)** | `RCC->CR` written 6× + `RCC->CFGR` written 5× — HSI→PLL init at startup |
| **FLASH latency** | `FLASH->ACR` written 5× — flash latency set for 72 MHz (LATENCY_2WS or 3WS) |
| **GPIOB** (LCD data bus 16-bit) | 9 occurrences — pin config + BSRR writes |
| **GPIOC** (LCD control: PC13/PC14/PC15) | 13 occurrences — densest peripheral, confirms LCD control pins on C |
| **GPIOE->CRL** (PB/PE output?) | 7 occurrences — possibly LCD back or extra data |
| **ADC1** (main analog input) | 3 occurrences + 2 reads of `JDR1` — ADC1 in injected mode |
| **ADC2** (second channel) | 4 occurrences — dual-channel scope (CH1+CH2) |
| **DMA1** (transfer) | 2 occurrences — ADC→RAM bulk transfer |
| **USB (idx 45 IRQ)** | `USB_LP_IRQHandler` at `0x0800B60D` — USB firmware present |

### LCD Parallel Bus — Pin Map (from `dso152tft.h` reference + peripheral scan)
| LCD Signal | Port.Pin | Perif Addr | Notes |
|------------|----------|------------|-------|
| LCD_CS   | PA15     | GPIOA      | Chip select |
| LCD_RST  | PA8      | GPIOA      | Reset |
| LCD_RS   | PC13     | GPIOC      | Data/Command select |
| LCD_WR   | PC14     | GPIOC      | Write strobe |
| LCD_RD   | PC15     | GPIOC      | Read |
| LCD_BL   | PC9      | GPIOC      | Backlight |
| LCD_D[0:15] | PB[0:15] | GPIOB | 16-bit parallel data |
| BTN_LEFT | PA10     | GPIOA      | Also TX log (9600 bps) |
| BTN_RIGHT| PC8      | GPIOC      | |
