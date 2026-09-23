/*
 * Nitemare 3D DOS reconstruction
 * Windows-derived cross-edition candidates.
 *
 * These routines are NOT promoted to DOS-confirmed until matched against
 * DOS N3D.EXE. They encode executable-derived Windows behavior that is
 * likely shared by the two editions.
 */

#define WIN_CANDIDATE_GUARD_CAPACITY 100
#define WIN_CANDIDATE_GUARD_STRIDE   0x1A
#define WIN_CANDIDATE_OBJECT_STRIDE  0x1C

#define WIN_CANDIDATE_MAX_PUSHES       12
#define WIN_CANDIDATE_PUSH_REC_BYTES    6
#define WIN_CANDIDATE_FIXED_TILE       64
#define WIN_CANDIDATE_PUSH_STEP         8
#define WIN_CANDIDATE_PUSH_TICKS        8

#pragma pack(push,1)
typedef struct GuardCandidate {
    uint8_t  unk00_01[2];
    uint32_t timestamp;          /* +02 */
    uint16_t state_timer;        /* +06 */
    uint16_t object_slot;        /* +08 */
    uint8_t  behavior_control;   /* +0A */
    uint8_t  state;              /* +0B */
    uint8_t  next_state;         /* +0C */
    uint8_t  object_id;          /* +0D */
    uint8_t  definition_id;      /* +0E */
    uint8_t  sync_flag;          /* +0F */
    uint8_t  target_or_sequence; /* +10, spawn sentinel FF */
    uint8_t  unk11_12[2];
    uint8_t  transition_param;   /* +13 */
    uint8_t  unk14_15[2];
    uint8_t  transition_flag;    /* +16 */
    uint8_t  unk17_19[3];
} GuardCandidate;
#pragma pack(pop)

/* Critical architectural clue:
 * GUARD does not need to own X/Y. Position can be indirect:
 * guard.object_slot -> OBJECT[slot] -> fixed X/Y.
 */
ObjectRecord *guard_object(GuardCandidate *g) {
    return object_pool_base + g->object_slot * WIN_CANDIDATE_OBJECT_STRIDE;
}

int object_tile_x(ObjectRecord *o) {
    return o->fixed_x >> 6;
}

int object_tile_y(ObjectRecord *o) {
    return o->fixed_y >> 6;
}

/* Push subsystem candidate recovered from Windows executable behavior. */
typedef struct PushCandidate {
    /* exact six-byte physical layout still needs DOS confirmation */
    int8_t step_x;
    int8_t step_y;
    uint8_t updates_left;
    uint8_t remaining[3];
} PushCandidate;

bool start_push_candidate(RuntimeObject *obj, int dir_x, int dir_y) {
    PushCandidate *p = lookup_push_record(obj);
    if (!p) return false;
    if (p->updates_left != 0) return false;
    if (abs(dir_x) + abs(dir_y) != 1) return false;

    int old_tx = obj->fixed_x >> 6;
    int old_ty = obj->fixed_y >> 6;
    int new_tx = old_tx + dir_x;
    int new_ty = old_ty + dir_y;

    if (!push_destination_allowed(new_tx, new_ty)) return false;

    p->step_x = (int8_t)(dir_x * WIN_CANDIDATE_PUSH_STEP);
    p->step_y = (int8_t)(dir_y * WIN_CANDIDATE_PUSH_STEP);
    p->updates_left = WIN_CANDIDATE_PUSH_TICKS;
    return true;
}

void update_push_candidate(RuntimeObject *obj, PushCandidate *p) {
    if (p->updates_left == 0) return;

    int old_tx = obj->fixed_x >> 6;
    int old_ty = obj->fixed_y >> 6;

    obj->fixed_x += p->step_x;
    obj->fixed_y += p->step_y;

    int new_tx = obj->fixed_x >> 6;
    int new_ty = obj->fixed_y >> 6;

    if (new_tx != old_tx || new_ty != old_ty) {
        /* Original mutable map stores wall+object per cell. */
        map[new_ty][new_tx].object = map[old_ty][old_tx].object;
        map[old_ty][old_tx].object = 0;
    }

    --p->updates_left;
    if (p->updates_left == 0) {
        p->step_x = 0;
        p->step_y = 0;
    }
}

/* Windows DEMO reader candidate:
 * header: 3 x u16 (6 bytes)
 * records: u32 raw_input_state + u32 tick
 */
typedef struct DemoRecordCandidate {
    uint32_t raw_input_state;
    uint32_t tick;
} DemoRecordCandidate;

bool demo_layout_candidate(size_t file_size) {
    if (file_size < 6) return false;
    return ((file_size - 6) % 8) == 0;
}

void demo_play_candidate(DemoRecordCandidate *records, size_t count) {
    size_t i = 0;
    while (i < count) {
        uint32_t now = game_tick();
        while (i < count && records[i].tick <= now) {
            set_raw_input_state(records[i].raw_input_state);
            ++i;
        }
        simulate_game_tick();
    }
}

/* USER.SAV Windows physical layout is useful primarily for pattern matching
 * against DOS save writes/reads. Do not assume byte-identical DOS format.
 */
typedef struct WinSaveBlockCandidate {
    uint32_t record_size;           /* +0000 */
    uint8_t  description[41];       /* +0004 */
    uint16_t episode;               /* +002D */
    uint16_t level_zero_based;      /* +002F */
    uint32_t tick_base;             /* +0031 */
    uint8_t  mutable_map[0x2000];   /* +0035 */
    uint8_t  global_state[0x5E];    /* +2035 */
    uint8_t  pool_2093[0x6D60];
    uint8_t  pool_8DF3[0x2648];
    GuardCandidate guards[100];     /* +B43B = 0A28 */
    uint8_t  pool_BE63[0x580];      /* 64 x 22 */
    uint8_t  aux_C3E3[0x20];
    uint8_t  state_C403[0x150];
    uint8_t  state_C553[0x08];
    uint8_t  state_C55B[0x48];
    uint8_t  byte_map_C5A3[0x1000]; /* 64x64 */
    uint8_t  state_D5A3[0x40];
    uint8_t  lookup_D5E3[0x100];
    uint8_t  floor_palette;
    uint8_t  ceiling_palette;
    uint16_t env_render_param;
} WinSaveBlockCandidate;

/* Renderer note:
 * Do NOT replace DOS reconstruction by textbook DDA solely because the
 * clean-room Windows project currently has a DDA Raycaster.cpp.
 * Original Windows RE points to a custom vector/line-segment column renderer.
 * DOS should be searched for matching signatures:
 *   - 320 column records / far pointers
 *   - 320 x 16-bit depth table
 *   - segment projection + near clipping
 *   - textured column interpolation/sampling
 */
