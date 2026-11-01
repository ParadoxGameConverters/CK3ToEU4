# Conversion diagnostics

The converter writes a diagnostic report to `debug/conversion_report.txt` at the end of every run, including runs that fail.
`Tools/Ck3SaveInspector` analyses a save on its own, so you can check a save before running the converter.

## debug/conversion_report.txt

Written next to `log.txt` in the converter folder. Sections:

- **Timings (ms)**: `CK3 world load` and `EU4 world build`.
- **Counts**: religions and faiths loaded, top-level `faiths` blocks found, and faiths sent to the EU4 religion import.
- **Issues**: `ERROR`, `WARNING`, and `INFO` lines, each tagged with the stage that raised it. Fatal errors are tagged `fatal`.
- **Unresolved save tokens**: the most common placeholder-style keys (for example `jens_179`). Normal saves contain some of these too (`trinket_1`, `spouse_1`), so compare with a known-good save.
- **Notes**: lists such as faiths with no `religion_map.txt` entry (exported as `converted_*`).

## Ck3SaveInspector

Works without building the converter. It uses the `rakaly.dll` in `CK3ToEU4/Resources`.

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File Tools\Ck3SaveInspector\Ck3SaveInspector.ps1 `
  -SavePath "C:\Users\<you>\Documents\Paradox Interactive\Crusader Kings III\save games\autosave.ck3" `
  -OutDir ".\debug" -KeepMelted
```

It copies the save first, because the game rewrites `autosave.ck3` while it runs. It then writes:

- `<save>.inspector.txt`: top-level blocks with line numbers, the religion layout (`RESULT:` line), and the placeholder-key counts.
- `<save>.melted.txt` (with `-KeepMelted`): the plain-text gamestate, for searching by hand.

## Known limits

- The placeholder counts are a signal, not a verdict. A save from a new CK3 version may need new token names in the bundled rakaly before the counts drop.
- The inspector checks the religion layout, not the content of each faith.
