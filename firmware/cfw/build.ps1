# build.ps1 — build the DSO152 CFW skeleton on Windows without make.
# Compiles each src/*.c with arm-none-eabi-gcc, assembles src/*.S, then
# links with cfw.ld and produces out/cfw.bin. Verifies the 48KB budget.

$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot   # make any relative -L/-o resolve under cfw/
$toolRoot = "C:\ST\STM32CubeIDE_2.0.0\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.win32_1.0.100.202509120712\tools\bin"
$gcc     = Join-Path $toolRoot "arm-none-eabi-gcc.exe"
$as      = Join-Path $toolRoot "arm-none-eabi-gcc.exe"   # -x assembler-with-cpp
$objcopy = Join-Path $toolRoot "arm-none-eabi-objcopy.exe"
$size    = Join-Path $toolRoot "arm-none-eabi-size.exe"

$srcDir = Join-Path $PSScriptRoot "src"
$incDir = Join-Path $PSScriptRoot "inc"
$outDir = Join-Path $PSScriptRoot "out"
$ld     = Join-Path $PSScriptRoot "link\cfw.ld"
if (-not (Test-Path $outDir)) { New-Item -ItemType Directory $outDir | Out-Null }

$arch = @("-mcpu=cortex-m3", "-mthumb")
$cflags = @("-Os", "-Wall", "-std=c11", "-g",
            "-ffunction-sections", "-fdata-sections", "-fno-common",
            "-I$incDir", "-DTARGET_CFV") + $arch
$ldflags = @("-T$ld", "-Wl,--gc-sections", "-Wl,-Map=out\cfw.map",
             "-Lout") + $arch

Write-Host "== Compiling C ==" -ForegroundColor Cyan
$objs = @()
foreach ($f in (Get-ChildItem $srcDir -Filter *.c)) {
    $o = Join-Path $outDir ($f.BaseName + ".o")
    & $gcc @cflags -c $f.FullName -o $o
    if ($LASTEXITCODE) { throw "compile failed: $($f.Name)" }
    $objs += $o
}

Write-Host "== Assembling .S ==" -ForegroundColor Cyan
foreach ($f in (Get-ChildItem $srcDir -Filter *.S)) {
    $o = Join-Path $outDir ($f.BaseName + ".o")
    & $gcc $arch -c $f.FullName -o $o
    if ($LASTEXITCODE) { throw "assemble failed: $($f.Name)" }
    $objs += $o
}

Write-Host "== Linking ==" -ForegroundColor Cyan
& $gcc $ldflags -o $outDir\cfw.elf $objs
if ($LASTEXITCODE) { throw "link failed" }

Write-Host "== Binary ==" -ForegroundColor Cyan
& $objcopy -O binary $outDir\cfw.elf $outDir\cfw.bin
& $size $outDir\cfw.elf

$sz = (Get-Item $outDir\cfw.bin).Length
if ($sz -gt 49152) {
    Write-Host "ERROR: cfw.bin is $sz bytes (> 48KB CFW budget)" -ForegroundColor Red
    exit 1
}
Write-Host "OK: cfw.bin = $sz bytes (< 48KB CFW budget)" -ForegroundColor Green
