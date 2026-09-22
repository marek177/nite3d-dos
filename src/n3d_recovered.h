#pragma once
/* Clean-room runtime facts recovered from original Nitemare 3D executables/data.
   Target: 16-bit MS-DOS C. Keep this header free of Win32/.NET dependencies. */

#define N3D_MAP_W 64
#define N3D_MAP_H 64
#define N3D_TILE_UNITS 64
#define N3D_MAP_HEADER_BYTES 514
#define N3D_MAP_LEVEL_BYTES 8192

#define N3D_PLAYER_MAX_HP 100
#define N3D_PLAYER_HALF_EXTENT 27
#define N3D_INPUT_FIRE 0x0080u
#define N3D_INPUT_STRAFE 0x0100u
#define N3D_INPUT_USE 0x0200u

#define N3D_MAX_DOORS 64
#define N3D_MAX_PANELS 32
#define N3D_MAX_PUSHABLES 12
#define N3D_MAX_OBJECTS 350
#define N3D_OBJECT_STRIDE 28
#define N3D_MAX_GUARDS 100
#define N3D_GUARD_STRIDE 26
#define N3D_MAX_VECTORS 1000

#define N3D_WALL_BLOCK 0x04
#define N3D_WALL_DOOR 0x08
#define N3D_WALL_SCRIPT_TOUCH 0x40
#define N3D_OBJECT_PRESENT 0x01
#define N3D_OBJECT_BLOCKS 0x02
#define N3D_OBJECT_SPECIAL_TOUCH 0x04
#define N3D_OBJECT_CREATES_GUARD 0x08

#define N3D_FB_W 320
#define N3D_FB_H 200
#define N3D_VIEW_X 8
#define N3D_VIEW_Y 4
#define N3D_VIEW_W 304
#define N3D_VIEW_H 152
#define N3D_TRANSPARENT_INDEX 0x29

/* Original object record anchors */
#define N3D_OBJ_FLAGS_OFS 0x05
#define N3D_OBJ_CLASS_OFS 0x06
#define N3D_OBJ_GUARD_OFS 0x07
#define N3D_OBJ_MAP_OFS 0x0C
#define N3D_OBJ_X_OFS 0x10
#define N3D_OBJ_Y_OFS 0x12
#define N3D_OBJ_PROJECTED_DAMAGE_BASE_OFS 0x18

/* Original guard record anchors */
#define N3D_GUARD_TIMER_OFS 0x06
#define N3D_GUARD_OBJECT_SLOT_OFS 0x08
#define N3D_GUARD_STRATEGY_OFS 0x0A
#define N3D_GUARD_STATE_OFS 0x0B
#define N3D_GUARD_NEXT_STATE_OFS 0x0C
#define N3D_GUARD_STRENGTH_OFS 0x10
#define N3D_GUARD_PAIN_STATE 0x15

/* Archive payload counts; not claimed as universal format maxima. */
#define N3D_EP1_LEVELS 11
#define N3D_EP2_LEVELS 10
#define N3D_EP3_LEVELS 10
