# project5loader – SIC / SICXE Relocating Loader

## Overview

`project5loader` is a relocating loader for the SIC architecture with optional SIC/XE support.

The program:

- Reads a SIC/SICXE object file in SCOFF format.
- Relocates the program to a new starting address.
- Outputs the relocated T (Text) and E (End) records to `stdout`.

This project is designed for a course in systems programming and is implemented in standard C with a portable build system (Makefile).

---

## Command-Line Interface

The loader accepts **three** command-line arguments:

- **objectFile** – path to a SIC/SICXE object file.
- **relocAddressHex** – relocation address in hexadecimal.
- **machineType** – target machine type: `SIC` or `SICXE`.

### Usage

```bash
project5loader <objectFile> <relocAddressHex> <machineType>
```

- `objectFile` is read from disk.
- `relocAddressHex` is parsed as a hex integer and used as the new starting address.
- `machineType` selects the relocation rules:
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

- **Linux:** gcc, clang, xlc, or similar.
- **Windows:** gcc (MinGW/MSYS), clang, or MSVC (with small Makefile tweaks if needed).
- **z/OS USS:** c89, xlc, or equivalent.

Also required: **make**.

---

## Building

From the project root directory:

```bash
make
```

This will produce an executable named: 

```bash
project5loader
```

Additional build options:

Clean:

```bash
make clean # removes object files and the executable
```

---

## Project Layout

Suggested directory structure:

```text
project5loader/
├── Makefile
├── README.md
├── docs/
│   └── slides/
│       └── project5loaderSlides.pptx
├── include/
│   ├── loader.h
│   ├── memory.h
│   ├── relocSic.h
│   ├── relocSicXE.h
│   ├── objFile.h
│   ├── sic.h
│   ├── sicxe.h
│   └── util.h
├── src/
│   ├── main.c
│   ├── loader.c
│   ├── objFileParser.c
│   ├── relocSic.c
│   ├── relocSicXE.c
│   ├── memory.c
│   └── util.c
└── tests/
```

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

### `src/objFileParser.c`

- Reads H/T/M/E records.
- Builds internal C structures (e.g., `headerRecord`, `textRecord`, `modRecord`, `endRecord`).
- Performs basic consistency checks (record sizes, addresses, etc.).

### `src/relocSic.c`

Implements relocation logic for **SIC**:

- Applies relocation factor to addresses referenced in modification records.
- Ensures addresses remain within SIC’s 24-bit limits as appropriate.

### `src/relocSicXES.c`

Implements relocation logic for **SIC/XE**:

- Handles different instruction formats and addressing modes.
- Applies relocation factor per modification records.

### `src/memory.c`

- Models a memory image if you choose to load the program into a byte array and then re-emit T records.
- Provides helper functions (write word, read word, etc.).

### `src/util.c` / `include/util.h`

- Hex string parsing.
- Error reporting.
- Common string manipulation utilities.

### `include/*.h`

Defines public interfaces and core structures:

- object file records.
- Loader configuration struct.
- Relocation interfaces.
- Utility function prototypes.

### `docs/slides/project5loaderSlides.pptx`

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

#### `struct objFile`

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
   - `R = newStart - start`

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

6. **Print relocated T and E records to `stdout`** in correct object file format.

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

```bash
make
./project5loader <objectFile> <relocAddressHex> <SIC|SICXE>
```


### Example

```bash
./project5loader examples/example.obj 2000 SIC
```