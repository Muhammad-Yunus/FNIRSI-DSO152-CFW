# Parameterized vector-table + peripheria scanner for DSO152 .bin (little-endian CH32/STM32)
Param(
  [string]$BinPath,
  [string]$OutDir,
  [int]$Vectors = 64
)

$ErrorActionPreference = "Stop"
$bin = [System.IO.File]::ReadAllBytes($BinPath)
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

# ---- 1. Vector table dump ----
$lines = New-Object System.Collections.Generic.List[string]
$lines.Add("[idx]  VMA          raw            flag")
for ($i = 0; $i -lt $Vectors; $i++) {
  $val = [BitConverter]::ToUInt32($bin, $i*4)
  $hex = "0x{0:X8}" -f $val
  $vma = "0x{0:X8}" -f (0x08000000 + $i*4)
  $flag = ""
  if ($val -ge 0x08000000 -and $val -le 0x0800FFFF) {
    $thumb = $val -band 0xFFFFFFFE
    $flag = "FLASH handler -> 0x" + ("{0:X8}" -f ($thumb + 1))
  } elseif ($val -eq 0) {
    $flag = "(NULL / default handler)"
  } else {
    $flag = "(off-flash: " + $hex + ")"
  }
  $lines.Add(("[{0:D2}]  {1}  {2}  {3}" -f $i, $vma, $hex, $flag))
}
[System.IO.File]::WriteAllText((Join-Path $OutDir "vector-table.txt"), ($lines -join "`n"))

# ---- 2. Scan the whole flash image for 0x40xxxxxx literals (LDR immediate patterns) ----
# In Thumb, absolute data addresses are usually loaded via PC-relative LDR from a literal pool,
# so we instead scan raw 4-byte LE words in the file for values in the peripheral map.
$periph = New-Object System.Collections.Generic.List[string]
$periph.Add("offset(vma)     raw        -> interpretation")
$seen = New-Object System.Collections.Generic.HashSet[string]
$periphStart = 0x40000000
for ($i = 0; $i -le ($bin.Length - 4); $i += 4) {
  $val = [BitConverter]::ToUInt32($bin, $i)
  if (($val -ge 0x40000000 -and $val -le 0x401FFFFF) -or ($val -ge 0x50000000 -and $val -le 0x500FFFFF)) {
    $vma = "0x{0:X8}" -f (0x08000000 + $i)
    $key = $val.ToString("X8")
    if (-not $seen.Contains($key)) { $seen.Add($key) | Out-Null }
    $periph.Add(("0x{0:X8}(vma {1}) {2:X8} -> 0x{3:X8}" -f $i, $vma, $val, $val))
  }
}
[System.IO.File]::WriteAllText((Join-Path $OutDir "periph-scan.txt"), ($periph -join "`n"))

Write-Host "Wrote vector-table.txt and periph-scan.txt to $OutDir"
