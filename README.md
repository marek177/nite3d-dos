# nite3d-dos

Reverse-engineering notes and reconstruction-oriented pseudocode for the DOS version of **Nitemare 3D**.

This repository is intended as clean-room documentation. It does **not** contain the original proprietary game source code or game assets.

## Evidence labels

- **CONFIRMED** — directly supported by binary/data analysis.
- **PARTIAL** — structure/function is supported, but one or more fields/constants remain unresolved.
- **INFERRED** — reconstruction consistent with observed behavior, not yet proven instruction-for-instruction.
- **UNKNOWN** — deliberately left unresolved rather than guessed.

## Reconstruction targets

1. MAXVEC and fixed-size runtime arrays
2. Renderer projection and clipping
3. Door / secret-panel / push collision
4. Guard state machine
5. Warp and trigger system
6. Savegame format
7. Demo timing
8. PCX decoder
9. FLI decoder
10. DOS/Windows bug comparison

See `pseudocode/` and `docs/` for the reconstruction work.
