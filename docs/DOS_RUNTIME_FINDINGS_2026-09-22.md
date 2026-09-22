# DOS runtime findings — 2026-09-22

Scope: DOS N3D v2.0 Ghidra export `N3D-DOS-UNFULL-v20.exe.c`. Addresses below refer to the DOS binary. Names are proposed, not recovered original symbols. Ghidra can misidentify 16-bit far calls and function boundaries.

| Address | Observed behavior | Confidence |
|---|---|---|
| `1000:0052` | Lookup in 14-byte special-wall records by X/Y. | High |
| `1000:00A2` | Map-object iterator with three caller-supplied arguments; the empty pseudo-C body is erroneous. | Purpose high; prototype unresolved |
| `1000:00A4` | Lookup in 6-byte object-reference records, resolving a 28-byte runtime object. | High |
| `1000:0212` | Build paired-wall runtime records from 64×64 map, maximum 64 × 18 bytes. | High |
| `1000:03FE` | Build class-3 special-wall table, maximum 32 × 14 bytes, with up to four directional components. | High |
| `1000:04BE` | Called by animation dispatch when class `0x2D` finishes; actual function boundary and map/collision writes require raw disassembly. | Trigger high, side effect open |
| `1000:0598` | Propagate paired-wall state to linked neighbor; changes blocking bit `0x01`, uses SFX `0x25/0x26`. | High |
| `1000:0704` | Central player USE dispatch. | High |
| `1000:0A40` | Countdown and automatic closing of paired walls/doors. | High |
| `1000:0C5F` | Per-tick paired/special wall movement in steps of 2 units. | High |
| `1000:13F4`, `1000:12C4` | Teleport destination and adjacent free tile/direction selection. | High |
| `1000:241E` | Actor animation update and class `0x2D` completion dispatch. | High |

DOS projectile wall-hit follows the observed chain `0x2E/0x2F → 0x2D`; explosion SFX `0x29` differs from door transition SFX `0x25/0x26`. Exact animation frame delay and final collision removal remain open.

The DOS evidence above independently establishes the 28-byte object indexing in this lookup and 64 paired-wall / 32 class-3 table capacities. Other constants in `src/n3d_recovered.h` marked WINREF still require DOS verification. Do not import Win16 door-record sizes or offsets blindly.

Next raw-disassembly checks: `1000:0090–00DF` for the iterator prototype; `1000:04A0–0510` for completion entry and writes; XREF of global `0x430E`; dynamic logging of wall record state `+8` (values 0–4).
