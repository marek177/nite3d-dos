# nite3d-dos

Reverse-engineering notes and reconstruction-oriented pseudocode for the DOS version of **Nitemare 3-D**.

This repository is clean-room documentation. It does **not** contain the original proprietary game source code or game assets.

## Current synchronization

The DOS work is now explicitly synchronized with the detailed Win16 reverse-engineering project in [`marek177/Nitemare3d-reversed`](https://github.com/marek177/Nitemare3d-reversed).

Start with:

- [`docs/DOS_WINDOWS_SYNC_2026-09-22.md`](docs/DOS_WINDOWS_SYNC_2026-09-22.md) — shared data facts, Win16 facts that must be tested against DOS, and the DOS audit checklist.

Important correction carried over from the newest executable audit: older approximate guesses such as OBJECT≈80 B, GUARD≈98 B and render-record≈52 B are **not** valid for the verified Win16 runtime. The current Windows values are OBJECT 28 B, GUARD 26 B and visible wall span 20 B. The DOS executable must now be checked independently for matching strides rather than inheriting the old guesses.

## Evidence labels

- **CONFIRMED / SHARED_DATA** — directly supported by original data and applicable to the documented shared format.
- **DOS_VERIFIED** — directly supported by a DOS executable/runtime trace.
- **WIN_VERIFIED** — directly supported by NITE3W.EXE and used only as a DOS comparison target.
- **PARTIAL** — structure/function is supported, but fields/constants remain unresolved.
- **INFERRED** — reconstruction consistent with evidence but not yet proven instruction-for-instruction.
- **UNKNOWN / TODO** — deliberately unresolved rather than guessed.

Do not silently convert a Win16 fact into a DOS fact.

## Shared format facts already established

- MAP world is 64x64 cells.
- Each map cell is `{wallByte, objectByte}`.
- Each level payload is 8192 bytes.
- Supplied MAP archive header is 514 bytes.
- MAP.1 / MAP.2 / MAP.3 contain 11 / 10 / 10 payloads.
- E1M11 is the internal/demo map.
- Player start IDs 1..4 encode the four cardinal orientations.
- DEMO.1-3 use a 6-byte header and an 8-byte event record once cross-bound to the Win16 recorder/player.
- ENDING.FLI analysis currently identifies 488 frames and COLOR_64/BRUN/LC/BLACK/COPY chunk families.

## Main DOS reconstruction targets

1. Identify and hash every DOS executable/version used for comparison.
2. Map the DOS MAP/IMG/SND/UIF/DEMO/BSF load paths.
3. Confirm or reject the Win16 OBJECT 28-byte / GUARD 26-byte record model in DOS.
4. Recover DOS renderer geometry separately from VGA output code.
5. Compare player movement, collision, USE and 64-world-unit tile scale.
6. Recover door / secret-panel / push / warp / trigger structures.
7. Recover the guard state machine, score switch, class transforms and attack timing.
8. Compare player/guard damage, weapon/ammo behavior and difficulty scaling.
9. Recover DOS save/config formats independently.
10. Validate DOS VOC/SND behavior against actual in-game playback.
11. Compare demo timing/input encoding and attract mode.
12. Reconstruct BSF integrity/version behavior and compare launchers.
13. Track intentional and accidental DOS-vs-Windows behavior differences.

## Cross-repository rule

Detailed instruction-level Win16 findings belong in `Nitemare3d-reversed`. When a DOS audit confirms, changes or disproves one of those findings, update both repositories with the executable version/hash and the exact evidence path.
