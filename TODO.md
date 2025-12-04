# Relocating loader fixes

- Update relocation routines to load and emit relocated bytes rather than echoing the original T records. Specifically, apply the relocation factor to text load addresses and modification targets before writing to memory, then regenerate output bytes/addresses from relocated memory so the T records printed reflect relocation.
- Adjust modification handling to use relocated addresses/fields for both SIC and SIC/XE paths, and ensure relocated values remain masked to the appropriate field width before emitting.
- Extend the output path to include the relocated end (E) record, using the entry point shifted by the relocation factor, after printing all relocated T records.
- Ensure the print logic emits relocated addresses and bytes rather than the parsed originals, so the program output matches the expected relocated object file format.
