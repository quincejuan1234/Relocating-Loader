# SIC / SICXE Relocating Loader – project4loader

## 1. Project Overview

This project implements a **relocating loader** for the SIC architecture, with optional SICXE support, in ISO C17.

The loader:

1. Reads a SIC object file in SCOFF format (`H`, `T`, `M`, `E` records).
2. Accepts a relocation base address (hex) and a target machine type (`SIC` or `SICXE`) from the command line.
3. Computes a relocation constant and applies all necessary relocations.
4. Prints the **relocated `T` and `E` records** to `stdout`.

Command-line interface:

```bash
./project5loader <object_file> <reloc_addr_hex> <machine_type>
# machine_type ∈ { SIC, SICXE }