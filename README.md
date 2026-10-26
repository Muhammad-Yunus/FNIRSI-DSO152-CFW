# <img src="" alt="" width="0"> FNIRSI-DSO152-CFW

> **Custom firmware + reverse-engineering toolkit** for the FNIRSI DSO152 70 MHz / 10 MSa/s pocket oscilloscope — CH32F103C8T6 (48 MHz Cortex-M3, 64 KB flash, 20 KB RAM)

**Status:** RE in progress · Firmware: `V0.1` (39.5 KB partial .bin) · Toolchain: GNU ARM 13.3

---

## ✨ Why This Repo

The stock DSO152 firmware is a 39.5 KB partial image of the full 64 KB flash. This project reverse-engineers that image (vector table, IRQs, peripheral map, string inventory, disassembly) to identify concrete patch points for a community CFW:

| CFW Goal | Where |
|---|---|
| USB data logging | `0x0800B60C` (outside this .bin — needs full flash) |
| HOLD trigger mode | Trigger state machine |
| 1000 V range + 1 ms timebase | Menu string / time-base tables |
| RDP unlock + reflash | Option bytes `0x1FFFF800` |
| Calibration bypass | Reset handler @ `0x08003DD4` |

---

## 📦 What's Inside

```
firmware/
├── original_firmware/
│   └── DSO152_APP_V0.1.bin        # 39,532-byte partial stock image
│
���── analysis/                      # All RE findings
│   ├── vector-table.txt            # 64-entry decoded table + Thumb-flag decode
│   ├── vector-bounds.txt           # In-range / out-of-range handler check
│   ├── periph-scan.txt             # 105 peripheral-base references
│   ├── strings-filtered.txt        # 118 meaningful strings
│   ├── peripheral-map.md           # GPIO / ADC / DMA / RCC / USB / LCD pin map
│   ├── firmware-flow.md            # Binary layout + IRQ analysis + CFW targets
│   ├── cfw-targets.md              # Concrete patch address register
│   └── irq-handlers.asm            # Active IRQ disassembly (SysTick, EXTI5/6…)
│
├── disasm/
│   └── full-thumb-objdump.asm      # 1.26 MB full Thumb disassembly
│
└── scripts/                       # PowerShell + objdump tooling
    ├── vt.ps1                     # Vector table decoder
    └── vecbounds.ps1              # Handler range validator
```

---

## 🧠 Firmware Architecture

### CH32F103C8T6 Memory Map
```
Flash:  0x08000000–0x0800FFFF  64 KB   (only 39.5 KB present in this .bin)
RAM:    0x20000000–0x20004FFF  20 KB
OptB:   0x1FFFF800             (RDP=0xA5 locked)
```

### Peripheral Usage (from `periph-scan.txt`)
| Base | Name | Notes |
|------|------|-------|
| `0x40010800` | RCC | Clock tree init (6 write sites) |
| `0x40010804` | RCC->APB2ENR | USART1 + USB clock enables |
| `0x40012400` | ADC1 | Primary waveform capture |
| `0x40012C00` | ADC2 | Second channel (dual-channel confirmed) |
| `0x40010410` | GPIOB_CRL | LCD data bus lower nibble |
| `0x40010414` | GPIOB_CRH | LCD data bus upper nibble |
| `0x40010420` | GPIOC_CRL | LCD control (RS/EN/…) |
| `0x40010430` | GPIOE_CRL | Status LED / extra pins |
| `0x40007800` | USB | **USB low-speed registered** (ISR @ `0x0800B60C`) |
| `0x40013800` | USART1 | UART debug interface |

### IRQ Table (Active Handlers)
| Vector | Handler | Function |
|--------|---------|----------|
| 11 | `0x080085C4` | **SysTick** — 10 ms UI tick |
| 12 | `0x08007740` | RTC/Tamper |
| 14 | `0x080085C2` | **EXTI5** — Left button |
| 15 | `0x08008BB0` | **EXTI6** — Right button |
| 45 | `0x0800B60C` | **USB LP** — outside this .bin |

### Firmware Features (from strings)
- 5 measurements: `Vmax / Vmin / Vave / Vrms / Vpp`
- Time base: `10 ms – 500 ms` (5 settings)
- Voltage range: `10 mV – 500 V` (6 settings)
- Trigger modes: `AUTO / STOP` (HOLD absent — CFW target)
- Calibration mode: "Please press RUN…"
- Dual ADC confirmed → dual-channel scope

---

## 🔌 LCD Pin Mapping (inferred)
```
GPIOB_CRL  → lower nibble of 8-bit LCD data bus
GPIOB_CRH  → upper nibble of LCD data bus
GPIOC_CRL  → RS / EN / RW / CS control lines
GPIOE_CRL  → status LED / additional signals
```

---

## 🛠 Tooling

| Tool | Path / Version | Status |
|------|---------------|--------|
| `arm-none-eabi-objdump` | STM32CubeIDE plugin 13.3 | ✓ Working |
| GNU GCC 13.3 rel1 | STM32CubeIDE bundle | ✓ Working |
| Ghidra 12.2 | `C:\D\App\ghidra_12.2` | ⚠ REA locked to 12.1.x |
| REA MCP Server | Sapiens AI REA | ✗ native provider unavailable |
| Docker | — | ✗ Not available |
| Rizin / Radare2 | — | ✗ Not installed |

---

## ⚠️ Limitations

- The `.bin` is a **39.5 KB partial image** of 64 KB flash — vectors pointing above `0x08009A6C` are out of range
- `arm-none-eabi-objdump` was run in **binary mode** (no symbol table), so disassembly labels are meaningless; only instruction-level patterns are valid
- Ghidra 12.2 cannot be used by REA (version mismatch); manual Ghidra headless not yet run
- Full USB ISR analysis requires a **64 KB SWD flash dump**

---

## 📄 License

Research / educational use. Stock firmware belongs to FNIRSI.
