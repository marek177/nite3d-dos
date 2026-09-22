#pragma once
/*
   Clean-room Nitemare 3-D constants for DOS reconstruction.

   IMPORTANT EVIDENCE BOUNDARY:
   - MAP/archive constants below are shared-data facts from original game data.
   - Runtime strides/capacities/offsets and viewport values are verified against
     NITE3W.EXE V1.10 (Win16) and are DOS comparison targets until the DOS EXE
     independently confirms them.

   Keep this header free of Win32/.NET dependencies and do not silently label a
   WINREF value as DOS_VERIFIED.
*/

/* SHARED_DATA: MAP/archive geometry. */
#define N3D_MAP_W 64
#define N3D_MAP_H 64
#define N3D_MAP_CELL_BYTES 2
#define N3D_TILE_UNITS 64
#define N3D_TILE_CENTER 32
#define N3D_MAP_HEADER_BYTES 514
#define N3D_MAP_LEVEL_BYTES 8192

/* WINREF: player/input behavior recovered from NITE3W.EXE. */
#define N3D_PLAYER_MAX_HP 100
#define N3D_PLAYER_HALF_EXTENT 27
#define N3D_INPUT_FORWARD 0x0002u
#define N3D_INPUT_BACKWARD 0x0004u
#define N3D_INPUT_TURN_A 0x0008u
#define N3D_INPUT_TURN_B 0x0010u
#define N3D_INPUT_FAST 0x0020u
#define N3D_INPUT_FINE_STEP 0x0040u
#define N3D_INPUT_FIRE 0x0080u
#define N3D_INPUT_STRAFE 0x0100u
#define N3D_INPUT_USE 0x0200u

/* WINREF: fixed runtime capacities/strides. */
#define N3D_MAX_DOORS 64
#define N3D_DOOR_STRIDE 22
#define N3D_MAX_PANELS 32
#define N3D_PANEL_STRIDE 22
#define N3D_MAX_PUSHABLES 12
#define N3D_PUSH_STRIDE 6
#define N3D_MAX_OBJECTS 350
#define N3D_OBJECT_STRIDE 28
#define N3D_MAX_GUARDS 100
#define N3D_GUARD_STRIDE 26
#define N3D_MAX_VECTORS 1000
#define N3D_VECTOR_STRIDE 28
#define N3D_VECLIST_ORIENTATIONS 4
#define N3D_VECLIST_ENTRIES 333
#define N3D_MAX_WALL_SPANS 50
#define N3D_WALL_SPAN_STRIDE 20
#define N3D_MAX_PROJECTED_SPRITES 100
#define N3D_PROJECTED_SPRITE_STRIDE 18

/* WINREF: flags/classes from the recovered original runtime tables. */
#define N3D_WALL_BLOCK 0x04
#define N3D_WALL_DOOR 0x08
#define N3D_WALL_SCRIPT_TOUCH 0x40
#define N3D_OBJECT_PRESENT 0x01
#define N3D_OBJECT_BLOCKS 0x02
#define N3D_OBJECT_SPECIAL_TOUCH 0x04
#define N3D_OBJECT_CREATES_GUARD 0x08

/* WINREF: framebuffer / normal 3-D viewport. */
#define N3D_FB_W 320
#define N3D_FB_H 200
#define N3D_VIEW_X 8
#define N3D_VIEW_Y 4
#define N3D_VIEW_W 304
#define N3D_VIEW_H 152
#define N3D_VIEW_CENTER_X 160
#define N3D_VIEW_CENTER_Y 80
#define N3D_TRANSPARENT_INDEX 0x29

/* WINREF: original 28-byte OBJECT record anchors. */
#define N3D_OBJ_FLAGS_OFS 0x05
#define N3D_OBJ_CLASS_OFS 0x06
#define N3D_OBJ_GUARD_OFS 0x07
#define N3D_OBJ_MAP_OFS 0x0C
#define N3D_OBJ_X_OFS 0x10
#define N3D_OBJ_Y_OFS 0x12
#define N3D_OBJ_PROJECTED_DAMAGE_BASE_OFS 0x18

/* WINREF: original 26-byte GUARD record anchors. */
#define N3D_GUARD_TIMER_OFS 0x06
#define N3D_GUARD_OBJECT_SLOT_OFS 0x08
#define N3D_GUARD_STRATEGY_OFS 0x0A
#define N3D_GUARD_STATE_OFS 0x0B
#define N3D_GUARD_NEXT_STATE_OFS 0x0C
#define N3D_GUARD_STRENGTH_OFS 0x10
#define N3D_GUARD_INITIAL_STRENGTH 255
#define N3D_GUARD_STATE_COUNT 22
#define N3D_GUARD_PAIN_STATE 0x15
#define N3D_DRACULA_PHASE1_CLASS 0x11
#define N3D_DRACULA_BAT_CLASS 0x14

/* SHARED_DATA + Win16 recorder/player binding: DEMO stream. */
#define N3D_DEMO_HEADER_BYTES 6
#define N3D_DEMO_RECORD_BYTES 8

/* Win16 save/config facts: comparison only until DOS formats are recovered. */
#define N3D_WIN_CONFIG_SAVE_BYTES 20
#define N3D_WIN_USER_SAVE_RECORD_BYTES 55015L

/* Archive payload counts in the supplied final files; not universal maxima. */
#define N3D_EP1_LEVELS 11
#define N3D_EP2_LEVELS 10
#define N3D_EP3_LEVELS 10
