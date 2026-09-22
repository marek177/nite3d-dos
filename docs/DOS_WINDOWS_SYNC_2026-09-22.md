# Nitemare 3-D DOS / Windows reverse-engineering synchronization

Date: 2026-09-22

This repository focuses on the DOS branch of the Nitemare 3-D investigation. The most detailed executable-level reconstruction currently comes from the Win16 `NITE3W.EXE` V1.10 audit in `marek177/Nitemare3d-reversed`.

The purpose of this file is to separate three categories cleanly:

- `SHARED_DATA` — facts proven from common game data formats and therefore directly useful to DOS tooling;
- `WIN_VERIFIED` — facts proven in NITE3W.EXE that are strong DOS comparison targets but must not be silently called DOS facts;
- `DOS_TODO` — items that need confirmation in the DOS executable/runtime.

## Shared data facts

These are directly useful to DOS parsers/editors:

- MAP dimensions are 64x64 cells.
- Each MAP cell is two bytes: wall byte + object byte.
- Each level payload is 8192 bytes.
- Archive header is 514 bytes in the supplied final MAP sets.
- MAP.1 contains 11 payloads, MAP.2 10, MAP.3 10.
- E1M11 is the internal/demo map; E1M3/E1M11 are the important positive-control pair for demo/map matching.
- Player-start IDs 1..4 encode four cardinal orientations.
- Supplied DEMO.1-3 use a 6-byte header representing WORDs `(10,5,20)` followed by 8-byte records.
- DEMO record layout is `eventByte, inputMask, pad, timestamp` once cross-checked against the Windows recorder/player.
- ENDING.FLI audit currently identifies 488 frames and the chunk families `COLOR_64`, `BRUN`, `LC`, `BLACK`, `COPY`.
- Known special WALLS/OBJECTS definition families include keyed walls, door orientations, elevators, stair/dumbwaiter warps, level gateways, triggers, AI navigation markers and safe-combination markers.

## Win16 facts to test against DOS

The following are directly verified in NITE3W.EXE V1.10 and should be treated as DOS comparison hypotheses until the DOS executable confirms them.

### Runtime capacities / record sizes

| Family | Win16 capacity | Win16 stride |
|---|---:|---:|
| doors | 64 | 22 B |
| panels | 32 | 22 B |
| pushes | 12 | 6 B |
| objects | 350 | 28 B |
| guards | 100 | 26 B |
| vectors | 1000 | 28 B |
| VECLIST/orientation | 333 | 4-byte far ptr |
| visible wall spans | 50 | 20 B |
| projected sprites | 100 | 18 B |

Older notes that described OBJECT≈80 B, GUARD≈98 B or render records≈52 B are superseded for the Windows build.

### World scale / player

- 64 world units per map tile.
- Spawn is tile center (`x*64+32`, `y*64+32`).
- Player collision half extent in the Win16 build is 27 world units.
- X and Y collision are handled separately, yielding wall sliding.
- Normal player health is 0..100 and lethal damage clamps to 0.

### Input mask

Win16 demo/runtime input bits:

- `0x0002` forward;
- `0x0004` backward;
- `0x0008`, `0x0010` turn directions;
- `0x0020` double movement/turn increment;
- `0x0040` force increment 1;
- `0x0080` fire;
- `0x0100` strafe modifier;
- `0x0200` edge-triggered use/action.

DOS should be audited for the same command encoding before sharing a demo/input implementation.

### Renderer comparison target

The recovered Windows renderer is not Wolfenstein 3-D one-grid-ray-per-column DDA. It is a projected wall-vector/span pipeline:

```text
MAP 64x64
 -> exposed boundary extraction/merge
 -> VEC[1000]
 -> four orientation pointer lists
 -> camera transform + clipping + projection
 -> 320-column owner table
 -> <=50 visible wall spans
 -> wall rasterization / column occlusion
 -> <=100 projected sprite commands
 -> 320x200 indexed framebuffer
```

For DOS the central question is which parts of this geometry/visibility pipeline are shared and which parts are only the WinG/DisplayDib presentation backend.

### OBJECT / GUARD comparison target

Win16 OBJECT is 28 bytes and GUARD is 26 bytes. Important Win16 semantics include:

- OBJECT `+06` class;
- OBJECT `+10/+12` world X/Y;
- GUARD `+08` linked OBJECT slot;
- GUARD `+0A` strategy;
- GUARD `+0B/+0C` state/nextstate;
- GUARD `+06` timer;
- GUARD `+10` strength;
- GUARD `+11/+12` octant/result octant.

The Win16 state dispatcher accepts 22 states (`0x00..0x15`), with `0x15` identified as a pain/hit-reaction state.

### Combat comparison target

The Win16 build initializes normal GUARD strength to 255. No class-specific post-spawn HP table has been found there; effective toughness is mostly produced by weapon/class damage transforms and special cases.

Important class behavior in the Windows audit:

- Dracula class `0x11` transforms in place to class `0x14` and restores strength to 255 for phase 2.
- `0x14` is the internal Dracula-Bat phase, not normal Bat class `0x08`.
- Ghost is wand-sensitive in the audited player->GUARD producer while the other weapons return zero there.
- Penelope and Cannon return zero normal weapon damage in that producer.
- Hamerstein has a special gated damage path.
- difficulty affects both damage directions and guard timing.

These are high-priority DOS cross-checks because matching them would demonstrate substantial shared game-logic code.

### USE / special walls

Win16 USE is edge-triggered and resolves an adjacent cardinal cell. It is the entry point for multiple systems, not just doors.

Key wall families to compare in DOS:

- `WARP_L1..L4` colored-key locked walls/passages;
- `WARP_1..8` paired vertical connections;
- `WARP_E1/E2` elevator families;
- mirror special warps;
- normal, locked, remote and curtain door orientations;
- `CONTROL`, `LEVEL_UP`, `LEVEL_UP2`;
- `WALL_EX1/2`, `ONE_SHOT`, `SPECIAL1`;
- `ACTIONSPOT`, `TRIGGER1/2`, `RETREAT`, `TURN`, `FLEE`, `SAFESPOT`.

## DOS-specific audit plan

1. Identify exact DOS executable/version(s), hashes and packer state.
2. Build DOS function map for map load, input, movement, collision, USE and render loop.
3. Search for constants/strides corresponding to 28-byte OBJECT and 26-byte GUARD layouts.
4. Search for capacities 350 objects, 100 guards, 1000 vectors, 333 orientation-list entries, 50 wall spans and 100 projected sprites.
5. Compare DOS demo recorder/player with the 8-byte Win16 record interpretation.
6. Compare DOS player start/object scan and 64-world-unit coordinate model.
7. Recover DOS renderer geometry separately from VGA presentation code.
8. Audit DOS sound/VOC paths independently from the Windows PCM/SND.DAT behavior.
9. Compare guard state count, class switch, score values and Dracula transformation.
10. Compare weapon selector/ammo behavior, difficulty transforms and player-health saturation.
11. Audit door/panel/push structures and special-wall dispatcher.
12. Compare save/config formats; do not assume USER.SAV/CONFIG.SAV are shared unchanged.
13. Audit BSF integrity/version behavior in DOS launchers and compare to Win16 versions.

## Cross-repository source of truth

Use `marek177/Nitemare3d-reversed` for the current evidence-backed Windows reconstruction, especially:

- `DISCOVERIES.md`
- `docs/ALL_THREADS_CONSOLIDATION_2026-09-22.md`
- `analysis/nite3w_renderer.md`
- `docs/GUARD_AI_RE.md`
- `docs/COMBAT_DAMAGE_RE.md`
- `docs/PLAYER_HEALTH_RE.md`
- `docs/PLAYER_COLLISION_RE.md`
- `docs/USE_INTERACTION_RE.md`
- `docs/DEMO_FORMAT_RE.md`
- `docs/SAVE_LIBRARIES_IDA_REPORT.md`

When a DOS audit confirms or contradicts one of these Win16 facts, record the result here and in the canonical reversed repository with explicit version/hash evidence.