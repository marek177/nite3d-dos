# Nitemare 3D DOS reverse-engineering sync — 2026-09-22

This document synchronizes the DOS-side reconstruction targets with the current findings tracked in `marek177/Nitemare3d-reversed`.

## Evidence policy

- **CONFIRMED** — directly supported by binary/data analysis or reproducible runtime observation.
- **PARTIAL** — function/structure is supported, but exact fields/constants remain unresolved.
- **INFERRED** — consistent with observed behaviour, not instruction-for-instruction proven.
- **UNKNOWN** — intentionally unresolved.

## Player

- Health is treated as a bounded gameplay value in the range 0..100. **CONFIRMED/PARTIAL**
- Player X/Y commit locations observed in the current Windows reverse-engineering work: around `0x4BF6` / `0x4BF8`. These addresses are not DOS offsets, but the behaviour they expose is relevant for DOS-side reconstruction. **PARTIAL**
- Ammo-related locations under active study: `0x4C1F`, `0x4C20`, `0x4C44`. **PARTIAL**
- Collision work must account for player radius/flags, walls, OBJECT/GUARD collision and sliding. **PARTIAL**

## OBJECT runtime

- Do not use the old 28-byte assumption as a runtime object layout. **CONFIRMED**
- Current runtime iteration evidence indicates an approximately `0x50` / 80-byte stride. **PARTIAL**
- Movement-related fields are observed around `+0x10/+0x12`. **PARTIAL**
- Fields around `+0x14/+0x18` remain unresolved and are high-value tracing targets. **UNKNOWN/PARTIAL**

## GUARD runtime

- Runtime iteration indicates an approximately 98-byte guard structure in the studied executable. **PARTIAL**
- Required reconstruction areas: type, state, HP, movement, attack, pain/death, timers and sound links. **PARTIAL**
- Guard runtime evidence should be separated from map/static object records; do not collapse them into one structure merely because fields appear related. **CONFIRMED design rule**

## Dracula transformation

- A distinct Dracula transformation path changes Dracula into an internal Bat form identified in the audit as `GUARD13 / type 0x14`. **STRONG PARTIAL / code-level evidence**
- This differs from normal Bat type `0x08`. **PARTIAL**
- Transformation code sets/restores 255 HP for the transformed form. **PARTIAL**
- The transformed form should remain distinct from a normal map-spawned Bat in a reconstruction until disproved. **RECONSTRUCTION REQUIREMENT**

## Combat / HP / difficulty

- Enemy HP is processed per guard. **CONFIRMED/PARTIAL**
- Exact per-enemy HP and shots-to-kill tables are still incomplete. **UNKNOWN/PARTIAL**
- Damage analysis must correlate weapon, range, difficulty, guard type and possible special/immunity behaviour. **PARTIAL**
- Dr. Hammerstein should be treated as a boss-specific audit target rather than inferred from ordinary guard values. **PARTIAL**

## USE / interaction

- `0x0200` is an important USE/interaction-related value in the current reconstruction. **PARTIAL**
- The USE chain must cover doors, switches, warp/teleport walls, push/special interactions, keyed interactions and special walls. **PARTIAL**
- Do not implement USE as a simple "open door" action. **RECONSTRUCTION REQUIREMENT**

## Warp and keyed walls

Known wall-function/name families encountered during analysis include:

- `WARP`
- `WARP_L1`
- `WARP_L2`
- episode-specific `WARP_1..n`
- elevator/level warp variants
- `JAMB`
- vertical/horizontal door variants
- `SPECIAL1`
- `ONE_SHOOT`
- `REVWALL`
- `CONTROL`
- `LEVEL_UP`

`WARP_L1/WARP_L2` have evidence of key-dependent behaviour, but exact state/script semantics remain to be confirmed by XREF/runtime tracing. **PARTIAL**

## Renderer

- The Windows-side renderer analysis shows a 320-pixel-wide framebuffer-oriented path. **CONFIRMED for studied build**
- Relevant comparison targets remain Wolf3D/Catacomb/Hovertank-era techniques, but Nitemare 3D behaviour must be independently verified. **METHOD NOTE**
- Current reverse-engineering evidence also points to a renderer-related runtime record stride of about 52 bytes. **PARTIAL**

## Data/resource formats

The three episodes use:

- `MAP.1..3`
- `IMG.1..3`
- `WALLS.1..3`
- `OBJECTS.1..3`

Round-trip editing/generation must preserve IDs, object/guard placement, doors, warps, episode indexing, spawn/orientation, special walls and original executable limits. **RECONSTRUCTION REQUIREMENT**

`E1M11` is used by `DEMO.1`; current comparison material indicates that `E1M11` and `E1M3` are effectively identical. **PARTIAL/strong behavioural evidence**

## DEMO

- `DEMO.1` is associated with `E1M11`. **PARTIAL/strong evidence**
- Reconstruction target: compact input stream for forward/back, turn, fire and use. **PARTIAL**
- Playback terminates on user key input and interacts with music/MIDI state. **PARTIAL**
- `DEMO.2` and `DEMO.3` map identification remains unresolved. **UNKNOWN**

## SND.DAT

- Audio extraction must be validated against actual in-game playback, not only whether the result is technically playable. **CONFIRMED workflow requirement**
- Current targets: MIDI/music mapping, SFX extraction, failed/incorrect VOC entries, and `guard type -> attack/pain/death sound IDs`. **PARTIAL**

## ENDING.FLI

- Current analysis counts 488 frames. **CONFIRMED for analysed file**
- Relevant chunk types: `COLOR_64`, `BRUN`, `LC`, `BLACK`, `COPY`. **CONFIRMED/PARTIAL**
- Correct editing must preserve palette state and delta-frame dependencies. **RECONSTRUCTION REQUIREMENT**

## USER.SAV

Still partial. Required fields include player state, episode/map state, inventory/ammo, serialized OBJECT/GUARD state, timers/flags, exact offsets and sizes.

## BSF / executable versions

Versions under comparison: `1.0`, `1.7`, `1.8`, `1.9`, `2.0`.

Priority work:

1. enumerate every XREF to `nite3d.bsf`,
2. identify parent functions,
3. follow open/read/seek/close chains,
4. reconstruct the byte comparison/check algorithm,
5. compare algorithm changes across versions.

## Highest-priority DOS reconstruction tasks

1. Player collision and sliding.
2. Runtime OBJECT layout.
3. GUARD movement/state machine.
4. Player damage/death.
5. Weapon cadence/damage.
6. Enemy HP and pain/death.
7. Enemy SFX mapping.
8. USE/door/switch/teleport/push interactions.
9. Savegame serialization.
10. DEMO command/value mapping.
11. Renderer write/projection path.
12. DOS-vs-Windows behavioural differences.

## Important separation rule

Addresses quoted from the current Windows executable reverse-engineering work are evidence about behaviour and structure, not drop-in DOS offsets. The DOS implementation should use them to guide equivalence testing, not to hard-code Windows addresses.
