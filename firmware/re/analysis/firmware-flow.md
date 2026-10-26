# DSO152 Firmware — Flow Analysis (CH32F103C8T6)

## Binary Layout
```
0x08000000  Vector Table       0x100 bytes (64 vectors × 4)
0x08000100  .text (code)      starts here
...
~0x08003CC0 .data copy + reset handler
~0x08003DD4 Reset_Handler (Thumb entry from vector[1])
...
~0x08009A6C end of .bin       39,532 bytes total (0x9A6C)
```

## Vector Table Summary (CH32F103 IRQ names)
| idx | VMA | Handler (Thumb) | IRQ Name | In .bin? |
|-----|------|-------------------|----------|----------|
| 0 | 0x08000000 | 0x20004F68 | Initial SP | — |
| 1 | 0x08000004 | 0x08003DD4 | **Reset_Handler** | ✓ |
| 2 | 0x08000008 | 0x080085C0 | NMI_Handler | ✓ |
| 3 | 0x0800000C | 0x08007FDA | HardFault_Handler | ✓ |
| 4 | 0x08000010 | 0x080085BC | MemManage_Handler | ✓ |
| 5 | 0x08000014 | 0x080046C4 | BusFault_Handler | ✓ |
| 6 | 0x08000018 | 0x08008BB2 | UsageFault_Handler | ✓ |
| 11 | 0x0800002C | 0x080085C4 | **SysTick_Handler** | ✓ |
| 12 | 0x08000030 | 0x08007740 | **RTC/Tamper IRQ** | ✓ |
| 14 | 0x08000038 | 0x080085C2 | **EXTI5_IRQHandler** | ✓ |
| 15 | 0x0800003C | 0x08008BB0 | **EXTI6_IRQHandler** | ✓ |
| 16–44 | 0x08000040–B4 | 0x08003DEF | DefaultHandler (weak) | ✓ |
| **45** | 0x080000B4 | **0x0800B60C** | **USB_LP_IRQHandler** | ✗ (>39.5KB) |

> **Note:** Vector 45 (USB ISR) points to `0x0800B60C` which is at file offset 0xB60C = 46,604 bytes,
> but the .bin is only 39,532 bytes. The USB ISR exists in the full 64 KB flash but not in this
> partial .bin. **The firmware does use USB (USB IRQ registered)** even though the USB-C port
> on the DSO152 is not visible on the PCB — it is wired internally on the CH32.

## Active IRQ Analysis

### SysTick_Handler (0x080085C4)
```asm
80085c4:  ldrd  r2, r3, [r3]        ; load SysTick->VAL
80085c8:  bl    0x8008954           ; call tick_update()
80085cc:  str   r0, [sp, #56]       ; store result
80085ce:  str   r1, [sp, #28]
80085d0:  ldr   r0, [sp, #4]
80085d2:  bl    0x8008dda           ; call tick_callback()
...
```
Periodic timer — likely drives the main loop's 10 ms tick for UI/menu updates.

### RTC/Tamper IRQ (0x08007740)
```asm
8007740:  (not found in disasm — likely falls into default handler region)
```

### EXTI5/EXTI6 (0x080085C2 / 0x08008BB0)
These are the **button press IRQs**:
- `EXTI5` = likely GPIOB pin 5 or similar → **BTN_LEFT** (PA10 via EXTI? Or PB5?)
- `EXTI6` = likely GPIOC pin 6 → **BTN_RIGHT** (PC8 via EXTI? Or PC6?)

The EXTI mechanism means buttons are interrupt-driven, not polled.

## Firmware String Inventory (from `strings-filtered.txt`)

### Key Identifiable Strings
| String | Meaning |
|--------|---------|
| `DSO152` | Model identifier |
| `Cn:http://www.fnirsi.cn` | Manufacturer URL |
| `Calib` + "Please press 'RUN'" | Calibration mode |
| `Vmax/Vmin/Vave/Vrms/Vpp` | Measurement readout format |
| `Fre:%d.%03dkHz` | Frequency display |
| `Cyc:%d.%03dms` | Period display |
| `AUTO` | Auto-scale mode |
| `STOP` | Trigger stop |
| `100mS / 200mS / 500mS / 10mS...` | Time/division options |
| `100V / 200V / 500V / 100mV...` | Voltage/division options |
| `v%d.%d` | Version string format |

### Inferred Features from Strings
- **Calibration mode** (RUN/OK buttons)
- **Auto/Stop trigger** modes
- **5 measurement values**: Vmax, Vmin, Vave, Vrms, Vpp
- **Frequency + period** measurements
- **Time base**: 10ms – 500ms (5 settings)
- **Voltage range**: 10mV – 500V (6 settings)

## Data Pool (0x08003E2C) — RAM Pointers Found
| Pool Offset | Value | Meaning |
|-------------|-------|---------|
| +00 | `0x0800FC00` | (out of .bin range — could be full-flash addr) |
| +04 | `0x20003C28` | RAM: large buffer (waveform storage?) |
| +08 | `0x20000120` | RAM: .data initialized section start |
| +12 | `0x20000121` | |
| +16 | `0x20000124` | |
| +20 | `0x20000111` | |
| +24 | `0x20000174` | |
| +28 | `0x20000178` | |
| +32 | `0x20000114` | |
| +40 | `0x2001FF5B` | (high RAM — near top of 20KB?) |

> `0x20003C28` (15,304 bytes into RAM) is likely the **waveform sample buffer**.

## CFW Patch Targets

| Target | Address | Description |
|--------|---------|-------------|
| **USB enable** | `0x0800B60C` | USB ISR exists in full flash but not in this .bin; CFW can add USB data logging |
| **Calibration bypass** | `0x08003DD4`+ | Reset handler → main init; can skip calibration check |
| **Voltage range cap** | string `100V`/`500V` | Rewrite menu to add 1000V/2000V or mV ranges |
| **Sample rate cap** | time base strings | Add 1ms or 5ms for higher sample rates |
| **Trigger hold** | `STOP` string | Modify to add HOLD trigger mode |
| **Calib data** | `0x20000120`+ | .data section in RAM; calibration constants live here |

## Tooling Status & Gaps

| Tool | Status | Note |
|------|--------|------|
| `arm-none-eabi-objdump` | ✓ Working | Full disasm at `disasm/full-thumb-objdump.asm` |
| REA `open_binary` | ⚠ | Ghidra 12.2 installed but REA requires 12.1.x; Java path bug (`bin\bin`) |
| REA native analysis | ✗ | All 4 providers (Ghidra/Hopper/IDA/Kuna) reject raw ARM on this host |
| Radare2 / Rizin | ✗ | Not installed |
| Ghidra decompiler | ⚠ | Available at `C:\D\App\ghidra_12.2` but REA-locked to 12.1.x |

### Next Steps for Deeper RE
1. **Fix Ghidra for REA**: install 12.1.4 OR fix `JAVA_HOME` path bug (remove one `bin\`)
2. **Disassemble IRQ 45**: need full 64KB flash dump (SWD read) to get USB ISR
3. **Identify `0x8008954`**: called from SysTick — likely the main tick/disp driver
4. **Trace `.data` copy**: reset handler copies `0x20000120` → `.bss` zeroed → `main()`
5. **ADC scan**: `0x40012400` (ADC1) and `0x40012C00` (ADC2) — confirm dual-channel or single
