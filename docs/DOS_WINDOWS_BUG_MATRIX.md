# DOS / Windows compatibility bug matrix

Do not treat a row as an engine bug until it is reproduced and tied to a routine. `TECHNOTE.TXT` is contemporary documentation of expected compatibility problems, not by itself proof of the internal cause.

| ID | Area | DOS baseline | Windows observation/documentation | Isolation method | Internal cause | Status |
|---|---|---|---|---|---|---|
| W01 | Main timing | timer interrupt path | game may run slower/jerkily | compare default vs `-t` | locate IRQ timer vs DOS-time dispatcher | PARTIAL |
| W02 | Windows detection | normal DOS timing | `-t` reportedly enabled automatically under Windows | inspect environment/Windows detection branch | unknown | PARTIAL |
| W03 | PC speaker | available in DOS | documented unavailable under Windows | `-p`, native DOS vs Windows | timer/speaker hardware access path | PARTIAL |
| W04 | `-t` + `-p` | separate options usable individually | combination documented as capable of freezing | reproduce in DOS/VM and trace wait/tick loop | likely incompatible timing dependency; exact cause unknown | PARTIAL |
| W05 | Sound card | hardware detection path | possible Windows sound-card problems | default vs `-s`; `-q 220 7`; `-o` | third-party sound/hardware detection path | PARTIAL |
| W06 | Sound detection startup lock | hardware-dependent | auto-detection can lock during startup | compare default vs `-s` | locate detection polling/timeout | PARTIAL |
| W07 | Extended memory | up to 2 MiB used when available | environment-dependent | default vs `-x` | XMS allocation/cache path | PARTIAL |
| W08 | Renderer clipping | establish native DOS pixel/hash baseline | unknown | camera corpus at near plane and screen edges | projection/clipping arithmetic | UNKNOWN |
| W09 | Fixed arrays | establish capacity-edge baseline | unknown | fill guards/objects/projectiles to N-1/N/N+1 | bounds/overflow behavior | UNKNOWN |
| W10 | Door collision | establish native DOS movement trace | unknown | block door during close/open; compare traces | occupancy and reversal rules | UNKNOWN |
| W11 | Demo sync | native DOS playback hash/tick trace | possible timing-dependent drift to test | record tick/input/frame hashes | timer source and fixed-step semantics | UNKNOWN |
| W12 | FLI timing | native ending playback cadence | possible timing variance to test | timestamp frame presents | FLI speed conversion / global timer | UNKNOWN |

## Recommended capture per test

Record executable build/hash, environment, CPU/emulation settings, command-line switches, map/save/demo input, initial RNG state if found, tick counter, player position/angle, active entity count, frame hash and `DEBUG.TXT`. A useful comparison should separate a gameplay-state divergence from a display-only timing difference.
