# Reconstruction status

This file tracks what is known versus what still needs binary-level proof. The pseudocode intentionally avoids inventing unresolved constants.

| Area | Status | Next proof target |
|---|---|---|
| MAXVEC + fixed arrays | PARTIAL | enumerate every fixed runtime vector, capacity, record size, allocation/free loop and overflow behavior |
| Renderer projection/clipping | PARTIAL | recover projection constants, near clip, column clipping and integer/fixed-point rounding |
| Door/panel/push collision | PARTIAL | recover exact open threshold, occupancy checks, movement increments and reversal rules |
| Guard state machine | PARTIAL | finish all 26 bytes of GUARD record; bind HP/direction/timer/speed/sequence/flags |
| Warp/trigger system | PARTIAL | enumerate trigger opcodes/conditions, two-destination warp state and cancel path |
| USER.SAV | PARTIAL | slot size is 0xD6E7 (55,015 bytes); map large runtime blocks to guards/objects/projectiles and globals |
| Demo timing | UNKNOWN/PARTIAL | identify recording stream layout and timer source; prove deterministic tick cadence |
| PCX decoder | PARTIAL | locate original decoder and compare accepted header modes, stride and palette behavior |
| FLI decoder | PARTIAL | locate original chunk dispatcher and exact frame timing/palette semantics |
| DOS vs Windows bugs | PARTIAL | construct one reproducible test per discovered issue and tie each to code path |

## Known compatibility evidence from TECHNOTE.TXT

The supplied technical notes say the game was not designed to run under Windows and may run slower/jerkily there, with possible sound-card problems and no PC-speaker sound. They also document startup switches useful for isolating compatibility paths:

- `-t`: disable timer interrupts and use DOS time functions; automatically enabled when Windows is detected.
- `-x`: disable extended memory usage.
- `-p`: force PC-speaker sound; the notes warn not to combine this with `-t` because the game can freeze.
- `-q port irq`: force Sound Blaster DSP detection.
- `-s`: disable sound-card detection; sound/music/joystick become unavailable.
- `-o`: enable `DEBUG.TXT` initialization diagnostics.

These switches are especially useful for the DOS/Windows bug-comparison harness because they expose alternate timing, memory and sound paths without patching the executable.

## Immediate RE order

1. Resolve MAXVEC and every fixed array first. Array capacities provide anchors for savegame blocks and runtime loops.
2. Resolve the 26-byte GUARD record and state transitions.
3. Map USER.SAV blocks against those confirmed runtime records.
4. Recover timer/tick handling and then decode demo timing.
5. Recover projection/clipping and moving-wall collision using fixed-point constants from the executable.
6. Recover PCX and FLI dispatchers independently because their signatures and chunk/RLE behavior are easier to validate with known files.
7. Build the DOS/Windows comparison table only from reproducible observations.
