# project5loader – SIC / SICXE Relocating Loader

## Overview

`project5loader` is a relocating loader for the SIC architecture with optional SIC/XE support.

The program:

- Reads a SIC/SICXE object file (in SCOFF format as used in this course).
- Relocates the program to a new starting address.
- Outputs the relocated T (Text) and E (End) records to `stdout`.

This project is designed for a course in systems and assembly programming and is implemented in standard C with a portable build system (Makefile).

---

## Command-Line Interface

The loader accepts **three** command-line arguments:

- **object_file** – path to a SIC/SICXE object file.
- **reloc_address_hex** – relocation address in hexadecimal (no `0x` required).
- **machine_type** – target machine type: `SIC` or `SICXE`.

### Usage

project5loader <object_file> <reloc_address_hex> <machine_type>

- `object_file` is read from disk.
- `reloc_address_hex` is parsed as a hex integer and used as the new starting address.
- `machine_type` selects the relocation rules:
  - `SIC` → classic SIC (24-bit, format 3 only, absolute addressing + X bit).
  - `SICXE` → SIC/XE (formats 1/2/3/4, extended addressing, etc., depending on project scope).

The program writes **only** the relocated T and E records to standard output, as required.

---

## Features

- Parses SCOFF-style object files:
  - H (Header), T (Text), M (Modification), and E (End) records.
- Supports relocation of:
  - SIC programs (`SIC` mode).
  - SIC/XE programs (`SICXE` mode, if requested).
- Applies modification records relative to a user-specified relocation address.
- Validates:
  - Input arguments.
  - Machine type (`SIC` / `SICXE`).
  - Basic object file structure (H/T/E order, record lengths).
- Produces relocated Text and End records matching the project specification.

---

## Requirements

A C compiler is required:

- **Linux:** gcc, clang, or similar.
- **Windows:** gcc (MinGW/MSYS), clang, or MSVC (with small Makefile tweaks if needed).
- **z/OS USS:** c89, xlc, or equivalent.

Also required: **make**.

---

## Building

From the project root directory:
make

This will produce an executable named: 
project5loader

Additional build options: 
make clean # removes object files and the executable

If using a platform where the compiler name differs:
make CC=clang
make CC=c89


---

## Project Layout

Suggested directory structure:

project5loader/
├── Makefile
├── README.md
├── docs/
│   └── slides/
│       └── project5loader_slides.pptx
├── include/
│   ├── loader.h
│   ├── scoff.h
│   ├── sic.h
│   ├── sicxe.h
│   └── util.h
├── src/
│   ├── main.c
│   ├── loader.c
│   ├── scoff_parser.c
│   ├── reloc_sic.c
│   ├── reloc_sicxe.c
│   ├── memory.c
│   └── util.c
└── tests/

---

## Module Responsibilities (High-Level)

### `src/main.c`

- Parses command-line arguments.
- Validates `machine_type` (SIC or SICXE).
- Converts relocation address from hex string to an integer.
- Invokes the core loader with a `LoaderConfig`.

### `src/loader.c`

High-level pipeline:

1. Parse the object file into in-memory structures.
2. Apply relocation using the appropriate backend (SIC or SICXE).
3. Emit relocated T and E records to `stdout`.

### `src/scoff_parser.c`

- Reads H/T/M/E records.
- Builds internal C structures (e.g., `ScoffHeader`, `ScoffTextRecord`, `ScoffModRecord`, `ScoffEnd`).
- Performs basic consistency checks (record sizes, addresses, etc.).

### `src/reloc_sic.c`

Implements relocation logic for **SIC**:

- Applies relocation factor to addresses referenced in modification records.
- Ensures addresses remain within SIC’s 24-bit limits as appropriate.

### `src/reloc_sicxe.c`

Implements relocation logic for **SIC/XE**:

- Handles different instruction formats and addressing modes.
- Applies relocation factor per modification records.

### `src/memory.c`

(Optional)

- Models a memory image if you choose to load the program into a byte array and then re-emit T records.
- Provides helper functions (write word, read word, etc.).

### `src/util.c` / `include/util.h`

- Hex string parsing.
- Error reporting.
- Common string manipulation utilities.

### `include/*.h`

Defines public interfaces and core structures:

- SCOFF records.
- Loader configuration struct.
- Relocation interfaces.
- Utility function prototypes.

### `docs/design.md`

- Describes SCOFF parsing.
- Relocation algorithm details.
- Design choices and trade-offs.
- Project challenges and solutions.

### `docs/slides/project5loader_slides.pptx`

- Presentation summarizing:
  - Loader architecture.
  - Algorithms.
  - Example runs.
  - Challenges & solutions.

---

## Design Overview

### Input

A SCOFF-style object file containing:

- One **Header (H)** record.
- Zero or more **Text (T)** records.
- Zero or more **Modification (M)** records.
- One **End (E)** record.

### Core Data Structures (Conceptual)

#### `struct LoaderConfig`

Contains:

- Path to the object file.
- Relocation address (integer).
- Machine type (`SIC` or `SICXE` enum).

#### `struct ScoffFile`

Aggregates:

- Header record.
- Dynamic arrays/lists of T and M records.
- End record.

---

## Relocation Algorithm (High-Level)

1. **Parse H record:**
   - Extract original start address `start`.
   - Extract program length `len`.

2. **Compute relocation factor:**
   - `R = new_start - start`

3. **Parse T records:**
   - Load addresses and byte contents into memory or internal structures.

4. **Parse M records:**
   - For each modification:
     - Identify the field to be relocated (address + length).
     - Read value.
     - Add relocation factor `R`.
     - Write back modified value.

5. **Generate relocated T and E records:**
   - T records reflect new addresses and updated contents.
   - E record’s starting address is set to the relocated start.

6. **Print relocated T and E records to `stdout`** in correct SCOFF format.

---

## Testing

Example simple test approach:

1. Create a small SIC or SICXE program.
2. Assemble it using the course assembler.
3. Run: project5loader tests/sample_objs/simple.obj 1000 SIC

4. Verify:
   - T record addresses shift correctly.
   - M-record fields are updated.
   - E record reflects relocated starting address.

---

## How to Run

make
./project5loader <object_file> <reloc_address_hex> <SIC|SICXE>


### Example

./project5loader examples/example.obj 2000 SIC
The relocated T and E records will be printed to standard output.