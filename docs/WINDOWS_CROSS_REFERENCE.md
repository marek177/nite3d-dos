# Windows -> DOS algorithm cross-reference

Date: 2026-09-17

This document uses `marek177/Nitemare3d-reversed` as a *cross-reference source* for reconstructing the DOS engine. The working hypothesis is that DOS and Windows editions share core gameplay/data algorithms while differing in platform, memory, rendering backend integration, timers, input and multimedia APIs.

Evidence labels used here:

- **VERIFIED_WIN_EXE** — directly recovered from NITE3W.EXE / Win16 RE.
- **VERIFIED_WIN_SAVE** — recovered from Windows USER.SAV physical layout.
- **CROSS_EDITION_CANDIDATE** — highly useful candidate for DOS, but must still be verified in DOS N3D.EXE before being promoted to DOS-confirmed.
- **DO_NOT_PORT_BLINDLY** — Windows clean-room implementation or platform-specific behavior, not evidence of the DOS algorithm.

## 1. Fixed runtime pools

### GUARD

Windows evidence gives:

- record stride: `0x1A` = 26 bytes
- capacity: `100`
- persisted block: `0x0A28` = 2600 bytes at USER.SAV `+0xB43B`
- GUARD `+0x08` is a runtime object slot; object stride is `0x1C` = 28 bytes

**DOS status:** CROSS_EDITION_CANDIDATE, very high priority to verify.

The first DOS search should therefore look for:

```text
imul/add patterns equivalent to index * 0x1A
MAXGUARD diagnostic or compare against 100
object linkage using guard+8 then object_index * 0x1C
```

### Push records

Windows executable evidence gives:

```text
MAXPUSHES = 12
push record size = 6 bytes
FixedUnitsPerTile = 64
PushStepUnits = 8
PushTicks = 8
```

The push update transfers the object-layer byte when the moving object changes tile.

**DOS status:** CROSS_EDITION_CANDIDATE. These constants are specific enough to search directly in DOS disassembly.

## 2. GUARD runtime record

Windows recovered layout:

```c
struct GuardRuntimeRecord /* 26 bytes */ {
    u8  unknown00_01[2];
    u32 timeStamp;          // +02
    u16 stateTimer;         // +06
    u16 objectSlot;         // +08
    u8  behaviorControl;    // +0A
    u8  state;              // +0B
    u8  nextState;          // +0C
    u8  objectId;           // +0D
    u8  definitionId;       // +0E
    u8  syncFlag;           // +0F
    u8  targetOrSequence;   // +10, initialized FF
    u8  unknown11_12[2];
    u8  transitionParam;    // +13
    u8  unknown14_15[2];
    u8  transitionFlag;     // +16
    u8  unknown17_19[3];
};
```

Important correction to the earlier DOS reconstruction model: **X/Y probably do not live in the 26-byte GUARD record.** Windows RE shows GUARD -> object slot -> 28-byte object record, and object coordinate fields are used for world/map position. DOS should be checked for the same indirection before assigning GUARD bytes to X/Y.

## 3. Save-state block map from Windows

Windows save slot size is `0xD6E7` (55,015 bytes), with:

```text
0000  4       record size guard
0004  41      save description
002D  2       episode
002F  2       zero-based level
0031  4       saved game tick/time base
0035  2000    mutable 64x64x2 map image
2035  005E    global/player gameplay block
2093  6D60    fixed runtime pool
8DF3  2648    fixed runtime pool
B43B  0A28    GUARD pool = 100 x 26
BE63  0580    64 x 22-byte runtime records
C3E3  0020    auxiliary state
C403  0150    runtime state
C553  0008    runtime state
C55B  0048    runtime state
C5A3  1000    64x64 byte map/state plane
D5A3  0040    runtime state
D5E3  0100    256-byte lookup/state block
D6E3  1       floor palette index
D6E4  1       ceiling palette index
D6E5  2       environment/render parameter
```

The Windows load routine restores raw blocks, rebuilds pointers, and rebases timers relative to current game time.

**DOS implication:** do not assume the DOS save format is identical, but use these pool sizes and record strides as direct search candidates in the DOS executable.

## 4. Demo format/timing

The Windows reconstructed reader recognizes:

```text
6-byte header = 3 x u16
then records of 8 bytes:
    u32 rawInputState
    u32 tick
```

Ticks are expected to be monotonic.

**DOS status:** CROSS_EDITION_CANDIDATE. The key DOS question is whether the exact same 8-byte record exists, or whether DOS stores equivalent input state against an IRQ/DOS-time tick source.

Search candidates in DOS:

- file-size check equivalent to `(size - 6) % 8 == 0`
- reads in 8-byte strides
- 32-bit input mask followed by 32-bit timestamp/tick
- playback loop comparing stored tick against global game tick

## 5. Renderer warning

Do **not** use the current Windows clean-room `Raycaster.cpp` as original-engine evidence. That file implements a textbook grid DDA renderer for the reconstruction project.

The Windows reverse-engineering roadmap instead states that the original renderer is **not** a Wolf3D-style textbook DDA and points to a custom vector/line-segment textured-column pipeline with:

- 320-entry column hit/far-pointer structure
- 320 x 16-bit depth buffer
- 320x200 8-bit framebuffer
- recovered default/configured 3-D viewport width of 304

Therefore DOS renderer reconstruction should search for the same structural signatures: 320-entry per-column tables, segment clipping/projection and textured-column sampling, rather than DDA map stepping.

## 6. Collision / push behavior

Windows executable-derived push logic is suitable as a DOS hypothesis:

```text
BuildPushTable:
    scan runtime object records (stride 28)
    identify PUSH class
    allocate one of 12 x 6-byte push records

StartPush:
    reject active push
    validate cardinal destination
    store signed movement increments
    set remaining_updates = 8

UpdatePushes:
    fixedX += stepX
    fixedY += stepY
    recompute tile from coord >> 6
    on tile change:
        move object-layer byte old_cell -> new_cell
    remaining_updates--
```

This is much stronger than the older generic `pushwall_has_vacated_cell()` placeholder and should replace it in DOS pseudocode with an evidence-tagged cross-edition implementation.

## 7. FLI and PCX

The Windows reconstruction currently only reads the standard 128-byte FLI/FLC header fields; it does **not** yet provide evidence for the game's original chunk decoder. Therefore existing generic FLI chunk pseudocode in the DOS repo remains an implementation hypothesis, not Windows-confirmed game code.

No PCX source was found in the Windows reversed repository during this pass. PCX remains DOS-binary/data RE work.

## 8. Cross-edition verification strategy

Promote a Windows-derived fact into DOS **CONFIRMED** only when one of these matches exists in DOS N3D.EXE:

1. identical constant + identical record stride + matching call role;
2. equivalent state transition or memory-layout access pattern;
3. same data-file behavior observed against the original DOS assets;
4. same save/demo structure verified byte-for-byte.

Until then mark it `WIN-CANDIDATE` rather than assuming identity.
