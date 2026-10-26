# Makefile for the DSO152 CFW skeleton
#
# Build (from firmware/cfw):
#   make            # -> out/cfw.elf + out/cfw.bin
#   make check      # -> verify cfw.bin size is under 48KB
#
# Toolchain: GNU ARM Embedded 13.3 Rel1 (arm-none-eabi-*). Override
# CROSS to point at your own prefix.

CROSS   ?= arm-none-eabi-
CC      := $(CROSS)gcc
AS      := $(CROSS)as
LD      := $(CROSS)ld
OBJCOPY := $(CROSS)objcopy
SIZE    := $(CROSS)size

# CFW source layout
SRC   := $(wildcard src/*.c src/*.S)
OBJ   := $(SRC:src/%.c=out/%.o)
OBJ  += $(SRC:src/%.S=out/%.o)

# Flags: Cortex-M3 (no FPU on CH32F103), 72 MHz
ARCH    := -mcpu=cortex-m3 -mthumb
CFLAGS  := -Os -Wall -std=c11 $(ARCH) -ffunction-sections -fdata-sections \
          -fno-common -Iinc -DTARGET_CFV -g
ASFLAGS := $(ARCH)
LDFLAGS := $(ARCH) -Tlink/cfw.ld -Wl,--gc-sections -Wl,-Map=out/cfw.map \
           --specs=nano.specs -Lout -lnano

.PHONY: all check clean dirs

all: dirs out/cfw.elf out/cfw.bin
	$(SIZE) out/cfw.elf
	@echo "CFW image: out/cfw.bin  (size below)"

out/cfw.elf: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ)

out/cfw.bin: out/cfw.elf
	$(OBJCOPY) -O binary $< $@

out/%.o: src/%.c inc/*.h | dirs
	$(CC) $(CFLAGS) -c -o $@ $<

out/%.o: src/%.S | dirs
	$(AS) $(ASFLAGS) -o $@ $<

dirs:
	@mkdir -p out

check: out/cfw.bin
	@sz=$$(wc -c < out/cfw.bin); \
	if [ "$$sz" -gt 49152 ]; then \
	  echo "ERROR: cfw.bin is $$sz bytes (> 48KB) — exceeds CFW budget"; \
	  exit 1; \
	else \
	  echo "OK: cfw.bin is $$sz bytes (< 48KB CFW budget)"; \
	fi

clean:
	rm -rf out
