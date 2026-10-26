# DSO152 CFW — Patch Target Register

## Goals
| Goal | Status | Notes |
|------|--------|-------|
| Enable USB data logging (VGA serial out) | 🔲 | USB ISR at 0x0800B60C not in this .bin — need full flash |
| Add HOLD trigger mode | 🔲 | Modify trigger state machine |
| Extend voltage range (add 1000V) | 🔲 | Patch menu string + ADC gain table |
| Add 1ms timebase | 🔲 | Patch time base table + sample rate |
| Remove RDP protection for reflash | 🔲 | SWD: disable `0x1FFFF1E0` RDP byte |
| Bypass calibration on startup | 🔲 | Patch reset handler jump |

## Memory Addresses (CH32F103C8T6)
```
Flash:  0x08000000 - 0x0800FFFF  (64 KB)
RAM:    0x20000000 - 0x20004FFF  (20 KB)
Option Bytes: 0x1FFFF800
  RDP:   0x1FFFF801  (0xA5 = locked, 0xFF = unlocked)
  BOR:   0x1FFFF803
  BOOT1: 0x1FFFF805  (bit 7)
```

## Peripherals to Configure in CFW
```c
// Enable USB peripheral
RCC_APB2ENR |= (1 << 14);   // USB clock enable
// USB base: 0x50000000  (WCH CH32 USBFS — confirmed: original .bin data pool
// references 0x5000F44F x3 in periph-scan.txt; NOT 0x40007800, that was a
// STM32-style guess. The WCH CH32 maps USBFS at 0x50000000.)

// Enable UART1 for VGA debug log (if using USART)
RCC_APB2ENR |= (1 << 2);    // USART1
USART1_BASE  = 0x40013800;

// GPIO config for USB (PA11/PA12)
GPIOA_CRL |= ...;           // PA11=USB_DM, PA12=USB_DP
```

## Known Constraints
- 64 KB flash total — full CFW must fit within 64 KB including bootloader
- 20 KB RAM — waveform buffer at 0x20003C28 uses ~16 KB; leave ≥ 4 KB for stack+heap
- CH32 ≠ STM32: use CH32F10x stdperiph headers, not STM32 stdperiph
