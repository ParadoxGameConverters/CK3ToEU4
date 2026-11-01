# Ck3SaveInspector.ps1
#
# Standalone diagnostics for a CK3 save. It needs no compiler and no converter build.
#
#   1. Melts the save with the rakaly library the converter ships with, and writes the plain-text gamestate next to the report.
#   2. Lists the top-level blocks of the gamestate with the line where each starts.
#   3. Checks the religion layout (faiths nested in religion, or a top-level faiths block) and counts the faith entries.
#   4. Counts placeholder keys such as jens_179=, which are tokens rakaly could not name. Those fields are misread by the converter.
#
# Usage:
#   powershell -NoProfile -ExecutionPolicy Bypass -File Ck3SaveInspector.ps1 -SavePath "C:\...\autosave.ck3" -OutDir "C:\...\debug"
#
# The melted file can be ~300 MB. Use -KeepMelted to keep it for manual searching.

param(
	[Parameter(Mandatory = $true)][string]$SavePath,
	[string]$OutDir = (Join-Path (Get-Location) "debug"),
	[string]$RakalyDll,
	[switch]$KeepMelted
)

$ErrorActionPreference = "Stop"
if (-not $RakalyDll) { $RakalyDll = Join-Path $PSScriptRoot "..\..\CK3ToEU4\Resources\rakaly.dll" }
$RakalyDll = (Resolve-Path $RakalyDll).Path
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
$baseName = [System.IO.Path]::GetFileNameWithoutExtension($SavePath)
$meltedPath = Join-Path $OutDir "$baseName.melted.txt"
$reportPath = Join-Path $OutDir "$baseName.inspector.txt"

$dllEscaped = $RakalyDll.Replace('\', '\\')
$code = @"
using System;
using System.Runtime.InteropServices;
public static class Rk
{
	[DllImport("$dllEscaped", CallingConvention = CallingConvention.Cdecl)] public static extern IntPtr rakaly_ck3_file(IntPtr data, UIntPtr len);
	[DllImport("$dllEscaped", CallingConvention = CallingConvention.Cdecl)] public static extern IntPtr rakaly_file_error(IntPtr res);
	[DllImport("$dllEscaped", CallingConvention = CallingConvention.Cdecl)] public static extern IntPtr rakaly_file_value(IntPtr res);
	[DllImport("$dllEscaped", CallingConvention = CallingConvention.Cdecl)] [return: MarshalAs(UnmanagedType.I1)] public static extern bool rakaly_file_is_binary(IntPtr file);
	[DllImport("$dllEscaped", CallingConvention = CallingConvention.Cdecl)] public static extern IntPtr rakaly_file_melt(IntPtr file);
	[DllImport("$dllEscaped", CallingConvention = CallingConvention.Cdecl)] public static extern IntPtr rakaly_melt_error(IntPtr res);
	[DllImport("$dllEscaped", CallingConvention = CallingConvention.Cdecl)] public static extern IntPtr rakaly_melt_value(IntPtr res);
	[DllImport("$dllEscaped", CallingConvention = CallingConvention.Cdecl)] public static extern UIntPtr rakaly_melt_data_length(IntPtr melt);
	[DllImport("$dllEscaped", CallingConvention = CallingConvention.Cdecl)] [return: MarshalAs(UnmanagedType.I1)] public static extern bool rakaly_melt_is_verbatim(IntPtr melt);
	[DllImport("$dllEscaped", CallingConvention = CallingConvention.Cdecl)] [return: MarshalAs(UnmanagedType.I1)] public static extern bool rakaly_melt_binary_unknown_tokens(IntPtr melt);
	[DllImport("$dllEscaped", CallingConvention = CallingConvention.Cdecl)] public static extern UIntPtr rakaly_melt_write_data(IntPtr melt, IntPtr buffer, UIntPtr length);
	[DllImport("$dllEscaped", CallingConvention = CallingConvention.Cdecl)] public static extern int rakaly_error_length(IntPtr err);
	[DllImport("$dllEscaped", CallingConvention = CallingConvention.Cdecl)] public static extern int rakaly_error_write_data(IntPtr err, IntPtr buffer, int length);
	[DllImport("$dllEscaped", CallingConvention = CallingConvention.Cdecl)] public static extern void rakaly_free_error(IntPtr err);
	[DllImport("$dllEscaped", CallingConvention = CallingConvention.Cdecl)] public static extern void rakaly_free_melt(IntPtr melt);
	[DllImport("$dllEscaped", CallingConvention = CallingConvention.Cdecl)] public static extern void rakaly_free_file(IntPtr file);
}
"@
Add-Type -TypeDefinition $code

function Get-RakalyError([IntPtr]$err) {
	$len = [Rk]::rakaly_error_length($err)
	$buf = [System.Runtime.InteropServices.Marshal]::AllocHGlobal($len + 1)
	try {
		[void][Rk]::rakaly_error_write_data($err, $buf, $len)
		return [System.Runtime.InteropServices.Marshal]::PtrToStringAnsi($buf, $len)
	} finally {
		[System.Runtime.InteropServices.Marshal]::FreeHGlobal($buf)
		[Rk]::rakaly_free_error($err)
	}
}

# ---- 1. melt -------------------------------------------------------------
$report = New-Object System.Collections.Generic.List[string]
function Add-Line([string]$text) { $report.Add($text); Write-Output $text }

# The game rewrites autosave.ck3 while it runs, so analyse a copy. Otherwise two runs can read different saves.
$snapshot = Join-Path $OutDir "$baseName.snapshot.ck3"
Copy-Item -LiteralPath $SavePath -Destination $snapshot -Force
$saveBytes = [System.IO.File]::ReadAllBytes($snapshot)
Remove-Item $snapshot -Force
$pin = [System.Runtime.InteropServices.GCHandle]::Alloc($saveBytes, 'Pinned')
try {
	$fileResult = [Rk]::rakaly_ck3_file($pin.AddrOfPinnedObject(), [UIntPtr]::new([uint64]$saveBytes.Length))
	$err = [Rk]::rakaly_file_error($fileResult)
	if ($err -ne [IntPtr]::Zero) { throw "rakaly could not parse the save: " + (Get-RakalyError $err) }
	$file = [Rk]::rakaly_file_value($fileResult)
	Add-Line ("save: {0}" -f $SavePath)
	Add-Line ("binary: {0}" -f [Rk]::rakaly_file_is_binary($file))

	$meltResult = [Rk]::rakaly_file_melt($file)
	$err = [Rk]::rakaly_melt_error($meltResult)
	if ($err -ne [IntPtr]::Zero) { throw "rakaly could not melt the save: " + (Get-RakalyError $err) }
	$melt = [Rk]::rakaly_melt_value($meltResult)
	Add-Line ("rakaly unknown tokens flag: {0}" -f [Rk]::rakaly_melt_binary_unknown_tokens($melt))

	$len = [uint64][Rk]::rakaly_melt_data_length($melt)
	$out = New-Object byte[] $len
	$outPin = [System.Runtime.InteropServices.GCHandle]::Alloc($out, 'Pinned')
	try { [void][Rk]::rakaly_melt_write_data($melt, $outPin.AddrOfPinnedObject(), [UIntPtr]::new($len)) } finally { $outPin.Free() }
	[System.IO.File]::WriteAllBytes($meltedPath, $out)
	$out = $null
	[Rk]::rakaly_free_melt($melt)
	[Rk]::rakaly_free_file($file)
} finally {
	$pin.Free()
	$saveBytes = $null
}

# ---- 2-4. one streaming pass over the melted gamestate ---------------------
$topBlocks = [ordered]@{}
$placeholderTotal = 0
$placeholderNames = @{}
$religionLine = 0; $religionFaithsNested = $false; $topFaithsLine = 0; $faithsDatabaseLine = 0; $faithDoctrineLines = 0
$inReligion = $false
$placeholderRe = [regex]'^\s*([a-z]+_[0-9]+)='
$topRe = [regex]'^([a-z_]+)=\{'
$lineNo = 0
$sr = [System.IO.StreamReader]::new($meltedPath)
try {
	while (($line = $sr.ReadLine()) -ne $null) {
		$lineNo++
		if ($line.Length -gt 0 -and $line[0] -ne "`t" -and $line[0] -ne ' ') {
			$m = $topRe.Match($line)
			if ($m.Success) {
				$name = $m.Groups[1].Value
				if ($name -ne 'triggered_event' -and -not $topBlocks.Contains($name)) { $topBlocks[$name] = $lineNo }
				if ($name -eq 'religion') { $religionLine = $lineNo; $inReligion = $true }
				if ($name -eq 'faiths') { $topFaithsLine = $lineNo }
			} elseif ($line -eq '}') {
				$inReligion = $false
			}
		}
		# The faiths block sits directly under religion (one tab). Deeper faiths={ids} lists are per-religion ID lists.
		if ($inReligion -and $line -match '^\tfaiths=\{') { $religionFaithsNested = $true }
		if ($topFaithsLine -gt 0 -and $faithsDatabaseLine -eq 0 -and $line -match '^\tdatabase=\{') { $faithsDatabaseLine = $lineNo }
		if ($topFaithsLine -gt 0 -and $line -match '^\t\t\t(doctrine|jens_174)=') { $faithDoctrineLines++ }
		$pm = $placeholderRe.Match($line)
		if ($pm.Success) {
			$placeholderTotal++
			$key = $pm.Groups[1].Value
			if ($placeholderNames.ContainsKey($key)) { $placeholderNames[$key]++ } else { $placeholderNames[$key] = 1 }
		}
	}
} finally {
	$sr.Close()
}

Add-Line ""
Add-Line "== Top-level blocks (first line) =="
foreach ($k in $topBlocks.Keys) { Add-Line ("{0,-34} line {1}" -f $k, $topBlocks[$k]) }

Add-Line ""
Add-Line "== Religion layout =="
Add-Line ("religion block line: {0}" -f $religionLine)
Add-Line ("faiths nested inside religion: {0}" -f $religionFaithsNested)
Add-Line ("top-level faiths block line: {0}" -f $topFaithsLine)
Add-Line ("faiths block has 'database' wrapper at line: {0}" -f $faithsDatabaseLine)
Add-Line ("doctrine / tenet lines inside top-level faiths: {0}" -f $faithDoctrineLines)
if ($religionLine -gt 0 -and -not $religionFaithsNested -and $topFaithsLine -gt 0) {
	Add-Line "RESULT: faiths are in a top-level block. The converter's religion handler sees no faiths and stops with 'no faith definitions'."
} elseif ($religionFaithsNested) {
	Add-Line "RESULT: faiths are nested in the religion block (CK3 1.19-style layout)."
} else {
	Add-Line "RESULT: no faith definitions found. Check the melted file."
}

Add-Line ""
Add-Line "== Unresolved placeholder keys =="
Add-Line ("total placeholder lines: {0}" -f $placeholderTotal)
Add-Line ("distinct placeholder names: {0}" -f $placeholderNames.Count)
# Prefixes seen in the CK3 1.20.1 autosave in very large numbers and absent from the CK3 1.19.0.6 save. Adjust as new versions show new families.
$newFamilies = @('jens', 'daan', 'gelmer', 'jacopo')
$familyTotal = 0
foreach ($key in $placeholderNames.Keys) { if ($newFamilies -contains ($key -replace '_\d+$', '')) { $familyTotal += $placeholderNames[$key] } }
Add-Line ("placeholders in known-new families (jens, daan, gelmer, jacopo): {0}" -f $familyTotal)
if ($placeholderTotal -gt 0) {
	Add-Line "Placeholders (e.g. jens_179=) are tokens rakaly could not name. Some legitimate keys also look like this, so compare against a known-good save."
	$placeholderNames.GetEnumerator() | Sort-Object Value -Descending | Select-Object -First 30 | ForEach-Object { Add-Line ("  {0,-16} {1}" -f $_.Key, $_.Value) }
}

$report | Set-Content -Path $reportPath -Encoding UTF8
Add-Line ""
Add-Line "report: $reportPath"
if (-not $KeepMelted) { Remove-Item $meltedPath -Force; Add-Line "melted file removed (use -KeepMelted to keep it)" } else { Add-Line "melted file: $meltedPath" }
