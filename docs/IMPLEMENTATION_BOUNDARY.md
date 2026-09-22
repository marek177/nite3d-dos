# DOS implementation boundary

This repository targets the 16-bit MS-DOS reconstruction. Shared MAP/data facts may be used directly. Constants recovered only from NITE3W.EXE are marked WINREF in `src/n3d_recovered.h` and must be independently confirmed against the DOS executable before being promoted to DOS_VERIFIED.

Current synchronized targets include 64x64 MAP geometry, 2-byte cells, 64 world units per tile, input masks, 28-byte OBJECT and 26-byte GUARD reference layouts, 27-unit player collision half-extent, USE=0x0200, original renderer dimensions, runtime capacities and DEMO 6+8-byte structure.

Do not restore the older provisional OBJECT=80 / GUARD=98 byte estimates; direct Win16 indexing and USER.SAV evidence superseded them.
