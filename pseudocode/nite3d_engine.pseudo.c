/*
 * Nitemare 3D DOS — reconstruction pseudocode
 * Clean-room documentation, NOT original source.
 *
 * Evidence tags:
 *   [C] CONFIRMED  [P] PARTIAL  [I] INFERRED  [U] UNKNOWN
 */

/* ================================================================
 * 1. MAXVEC + FIXED ARRAYS
 * ================================================================ */

/* [P] DOS engine uses bounded/fixed runtime storage. Exact capacities
 * must be filled only after confirming every allocation/reference. */
#define MAP_W 64                  /* [C] map format/reconstruction evidence */
#define MAP_H 64                  /* [C] */
#define MAP_CELLS (MAP_W*MAP_H)
#define MAXVEC_UNKNOWN (-1)       /* [U] do not guess */

struct RuntimeArrays {
    Guard guards[MAXVEC_UNKNOWN];
    Object objects[MAXVEC_UNKNOWN];
    Projectile projectiles[MAXVEC_UNKNOWN];
    Door doors[MAXVEC_UNKNOWN];
    Trigger triggers[MAXVEC_UNKNOWN];
    /* [U] additional renderer/audio/animation vectors */
};

bool bounded_alloc_slot(Array *a, int capacity, int *out_index) {
    for (int i=0; i<capacity; ++i) {
        if (!a[i].active) { *out_index=i; return true; }
    }
    return false; /* important for overflow/Windows comparison */
}

/* ================================================================
 * 2. RENDERER — PROJECTION / CLIPPING
 * ================================================================ */

struct Camera {
    fixed x, y;
    angle_t angle;
    fixed sin_a, cos_a;
};

struct ProjectedPoint {
    fixed view_x;
    fixed depth;
    int screen_x;
};

ProjectedPoint world_to_view(Camera c, fixed wx, fixed wy) {
    fixed dx = wx-c.x, dy = wy-c.y;
    ProjectedPoint p;
    p.view_x = dx*c.sin_a - dy*c.cos_a;   /* [I] sign/order to verify */
    p.depth  = dx*c.cos_a + dy*c.sin_a;   /* [I] */
    if (p.depth <= NEAR_CLIP) p.screen_x = CLIPPED;
    else p.screen_x = SCREEN_CENTER_X + fixed_div(p.view_x*PROJ_SCALE,p.depth);
    return p;
}

void render_wall_segment(WallSeg s) {
    ProjectedPoint a=world_to_view(camera,s.x1,s.y1);
    ProjectedPoint b=world_to_view(camera,s.x2,s.y2);
    if (a.depth<=NEAR_CLIP && b.depth<=NEAR_CLIP) return;
    clip_segment_to_near_plane(&a,&b);            /* [P] */
    int x1=clamp(a.screen_x,0,SCREEN_W-1);
    int x2=clamp(b.screen_x,0,SCREEN_W-1);
    if (x2<x1) swap(&x1,&x2);
    for (int x=x1; x<=x2; ++x) {
        fixed depth=interpolate_depth(a,b,x);
        if (depth < z_column[x]) {
            draw_textured_wall_column(x,s,depth);
            z_column[x]=depth;
        }
    }
}

/* ================================================================
 * 3. DOOR / PANEL / PUSH COLLISION
 * ================================================================ */

enum CellKind { EMPTY, SOLID_WALL, DOOR_CELL, SECRET_PANEL, PUSH_CELL };

bool can_cross_cell(Entity e, int tx, int ty) {
    Cell *c=&map[ty][tx];
    switch(c->kind) {
        case EMPTY: return !occupied_by_solid_entity(tx,ty,e);
        case SOLID_WALL: return false;
        case DOOR_CELL:
            return door_passage_open_enough(c->door_id,e.radius); /* [P] threshold unknown */
        case SECRET_PANEL:
            return panel_is_fully_displaced(c->panel_id);         /* [I] */
        case PUSH_CELL:
            return pushwall_has_vacated_cell(c->push_id);         /* [I] */
    }
    return false;
}

void update_door(Door *d, int ticks) {
    switch(d->state) {
        case CLOSED: break;
        case OPENING:
            d->position += d->speed*ticks;
            if (d->position>=DOOR_OPEN_MAX) { d->position=DOOR_OPEN_MAX; d->state=OPEN; d->timer=0; }
            break;
        case OPEN:
            d->timer += ticks;
            if (d->timer>=d->hold_ticks && doorway_clear(d)) d->state=CLOSING;
            break;
        case CLOSING:
            if (!doorway_clear(d)) d->state=OPENING;
            else {
                d->position -= d->speed*ticks;
                if (d->position<=0) { d->position=0; d->state=CLOSED; }
            }
            break;
    }
}

/* ================================================================
 * 4. GUARD STATE MACHINE
 * ================================================================ */

/* [P] GUARD runtime record known to be a compact fixed record in the
 * reverse-engineering work. Exact 26-byte field mapping stays unresolved
 * here until every byte is proven. */
enum GuardState { G_IDLE, G_ALERT, G_CHASE, G_ATTACK, G_PAIN, G_DYING, G_DEAD };

void guard_tick(Guard *g, int ticks) {
    if (!g->active) return;
    g->timer -= ticks;
    switch(g->state) {
        case G_IDLE:
            if (guard_detects_player(g)) guard_enter(g,G_ALERT);
            break;
        case G_ALERT:
            if (g->timer<=0) guard_enter(g,G_CHASE);
            break;
        case G_CHASE:
            guard_path_or_steer(g,ticks);
            if (guard_can_attack(g)) guard_enter(g,G_ATTACK);
            break;
        case G_ATTACK:
            if (g->timer<=0) {
                guard_execute_attack(g); /* damage/range/projectile values [U/P] */
                guard_enter(g,G_CHASE);
            }
            break;
        case G_PAIN:
            if (g->timer<=0) guard_enter(g,G_CHASE);
            break;
        case G_DYING:
            advance_death_sequence(g,ticks);
            if (death_sequence_finished(g)) guard_enter(g,G_DEAD);
            break;
        case G_DEAD: break;
    }
}

void guard_take_damage(Guard *g, int damage) {
    if (g->state==G_DEAD || g->state==G_DYING) return;
    g->hp -= damage;
    if (g->hp<=0) guard_enter(g,G_DYING);
    else guard_enter(g,G_PAIN);
}

/* ================================================================
 * 5. WARP / TRIGGER SYSTEM
 * ================================================================ */

enum TriggerType { TR_USE, TR_CROSS, TR_SHOOT, TR_ITEM, TR_TIMER };
enum ActionType { ACT_DOOR, ACT_PANEL, ACT_WARP_MENU, ACT_WARP_DIRECT,
                  ACT_REVEAL_ITEM, ACT_MESSAGE, ACT_LEVEL_EVENT };

void trigger_fire(Trigger *t, Entity *activator) {
    if (!t->enabled) return;
    if (!trigger_conditions_met(t,activator)) return;
    for (int i=0;i<t->action_count;i++) execute_action(t->actions[i],activator);
    if (t->one_shot) t->enabled=false;
}

void execute_warp_menu(WarpDef *w, Player *p) {
    int choice = show_warp_choices(w->destination_a,w->destination_b,true);
    if (choice==CANCEL) return;
    Destination d=(choice==0)?w->destination_a:w->destination_b;
    p->x=d.x; p->y=d.y;
    p->angle=d.exit_angle; /* preserve/rotate behavior [P] */
}

/* ================================================================
 * 6. SAVEGAME FORMAT
 * ================================================================ */

/* [C] previous RE established USER.SAV slot size 0xD6E7 = 55015 bytes.
 * [P] internal runtime blocks still require field-level mapping. */
#define SAVE_SLOT_BYTES 0xD6E7

struct SaveSlotLogical {
    PlayerState player;
    MapRuntime map_state;
    RuntimeArrays arrays;
    Inventory inventory;
    GameClock clock;
    /* [U] byte-exact ordering/padding/check fields */
};

bool load_save_slot(byte raw[SAVE_SLOT_BYTES], SaveSlotLogical *s) {
    Cursor c={raw,0};
    /* Every read offset must be replaced with verified offsets. */
    parse_player_block(&c,&s->player);       /* [P] */
    parse_map_runtime_block(&c,&s->map_state);/* [P] */
    parse_entity_blocks(&c,&s->arrays);      /* [P] */
    parse_inventory_block(&c,&s->inventory); /* [P] */
    return validate_save_runtime(s);
}

/* ================================================================
 * 7. DEMO TIMING
 * ================================================================ */

struct DemoEvent { uint32 tick; byte command; byte arg0; byte arg1; };

void demo_play(Demo *d) {
    uint32 sim_tick=0;
    while (!d->finished) {
        while (d->next.tick<=sim_tick) {
            apply_demo_input(d->next);
            demo_read_next(d);
        }
        game_simulate_one_tick();
        sim_tick++;
        render_if_due();
    }
}

/* [P] Critical RE question: whether DOS recording is input-per-tick,
 * delta-time encoded, or tied to timer interrupt cadence. */

/* ================================================================
 * 8. PCX DECODER
 * ================================================================ */

bool pcx_decode_scanline(Stream *f, byte *dst, int expected) {
    int out=0;
    while(out<expected) {
        byte b=read8(f);
        if ((b & 0xC0)==0xC0) {
            int run=b & 0x3F;
            byte value=read8(f);
            while(run-- && out<expected) dst[out++]=value;
        } else dst[out++]=b;
    }
    return out==expected;
}

bool pcx_decode_8bit(Stream *f, Image8 *img) {
    PCXHeader h=read_pcx_header(f);
    if (!pcx_header_supported(h)) return false;
    for(int y=0;y<h.height;y++)
        if(!pcx_decode_scanline(f,img->pixels+y*img->pitch,h.bytes_per_line)) return false;
    /* Palette handling/location must follow the exact PCX variant used. */
    return true;
}

/* ================================================================
 * 9. FLI DECODER
 * ================================================================ */

void fli_play(Stream *f, Surface8 *screen) {
    FliHeader h=read_fli_header(f);
    for(int frame=0; frame<h.frames; ++frame) {
        FliFrameHeader fh=read_fli_frame_header(f);
        for(int c=0;c<fh.chunks;c++) {
            FliChunk ch=read_fli_chunk_header(f);
            switch(ch.type) {
                case FLI_COLOR: decode_fli_palette(f,ch,screen); break;
                case FLI_LC:    decode_fli_line_compressed(f,ch,screen); break;
                case FLI_BRUN:  decode_fli_byte_run(f,ch,screen); break;
                case FLI_COPY:  decode_fli_copy(f,ch,screen); break;
                case FLI_BLACK: clear_surface(screen); break;
                default: skip_bytes(f,ch.size-CHUNK_HEADER_SIZE); break;
            }
        }
        present(screen);
        wait_until_next_fli_frame(h.speed);
    }
}

/* ================================================================
 * 10. DOS / WINDOWS BUG COMPARISON HARNESS
 * ================================================================ */

enum BugArea { BUG_TIMER, BUG_SOUND, BUG_MEMORY, BUG_COLLISION,
               BUG_RENDER_CLIP, BUG_ARRAY_BOUND, BUG_DEMO_SYNC,
               BUG_FILE_IO, BUG_INPUT };

struct BugObservation {
    BugArea area;
    char *test;
    char *dos_result;
    char *windows_result;
    char *cause;
    Evidence evidence;
};

void compatibility_test_matrix(void) {
    test_timer_interrupt_vs_dos_time();
    test_extended_memory_enabled_disabled();
    test_sound_detection_enabled_disabled();
    test_pc_speaker_timer_interaction();
    test_fixed_array_capacity_edges();
    test_renderer_near_plane_edges();
    test_door_entity_overlap();
    test_demo_determinism();
}
