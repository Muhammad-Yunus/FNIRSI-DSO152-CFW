# Param[0]: BinPath, [1]: OutDir
$bin = [System.IO.File]::ReadAllBytes($args[0])
$sz = $bin.Length
$out = [System.IO.DirectoryInfo]::new($args[1])
New-Item -ItemType Directory -Force -Path $out.FullName | Out-Null

Write-Host "File size: $sz = 0x$($sz.ToString('X'))"

# Check all 64 vectors
$lines = [System.Collections.Generic.List[string]]::new()
for ($i = 0; $i -lt 64; $i++) {
  $val = [BitConverter]::ToUInt32($bin, $i * 4)
  if ($val -ge 0x08000000 -and $val -le 0x0800FFFF) {
    $off = $val - 0x08000000
    $inRange = $off -lt $sz
    $thumb = ($val -band 0xFFFFFFFE) -bor 1
    $lines.Add(("[{0:D2}] 0x{1:X8} -> file off 0x{2:X:} in-range={3}  (Thumb entry: 0x{4:X8})" -f $i, $val, $off, $inRange, $thumb))
  } elseif ($val -eq 0) {
    $lines.Add(("[{0:D2}] 0x00000000 (NULL/default)" -f $i))
  } else {
    $lines.Add(("[{0:D2}] 0x{1:X8} (off-flash value)" -f $i, $val))
  }
}
[System.IO.File]::WriteAllText((Join-Path $out.FullName "vector-bounds.txt"), ($lines -join "`n"))
Write-Host "Wrote vector-bounds.txt"
