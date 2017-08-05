#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "core.h"
#include "random.h"
#include "game_qs.h"
#include "gfx_qs.h"
#include "game_menu.h"
#include "gfx.h"
#include "qrs.h"
#include "timer.h"
#include "audio.h"

const char *grade_names[37] =
{
    "  9", "  8", "  7", "  6", "  5", "  4", "  3", "  2", "  1",
    " S1", " S2", " S3", " S4", " S5", " S6", " S7", " S8", " S9", "S10", "S11", "S12", "S13",
    " m1", " m2", " m3", " m4", " m5", " m6", " m7", " m8", " m9",
    "  M", " MK", " MV", " MO", " MM", " GM"
};

static qrs_timings qs_curve[QS_CURVE_MAX] =
{
    {0, 4, 60, 12, 25, 25, 12},
    {100, 16, 60, 12, 25, 25, 12},
    {120, 32, 60, 12, 25, 25, 12},
    {140, 48, 60, 12, 25, 25, 12},
    {160, 64, 60, 12, 25, 25, 12},
    {180, 96, 60, 12, 25, 25, 12},
    {200, 128, 60, 12, 25, 25, 12},
    {300, 192, 60, 12, 25, 25, 12},
    {400, 256, 60, 12, 25, 25, 12},
    {600, 512, 60, 12, 25, 25, 8},
    {640, 768, 60, 12, 25, 25, 8},
    {660, 1024, 60, 12, 25, 25, 8},
    {700, 20*256, 40, 8, 20, 18, 6},
    {800, 20*256, 30, 8, 20, 18, 6},
    {900, 20*256, 26, 8, 16, 16, 6}
};

static qrs_timings g1_master_curve[G1_MASTER_CURVE_MAX] =
{
    {0, 4, 30, 14, 30, 30, 41},
	{30, 6, 30, 14, 30, 30, 41},
	{35, 8, 30, 14, 30, 30, 41},
	{40, 10, 30, 14, 30, 30, 41},
	{50, 12, 30, 14, 30, 30, 41},
	{60, 16, 30, 14, 30, 30, 41},
	{70, 32, 30, 14, 30, 30, 41},
	{80, 48, 30, 14, 30, 30, 41},
	{90, 64, 30, 14, 30, 30, 41},
	{100, 80, 30, 14, 30, 30, 41},
	{120, 96, 30, 14, 30, 30, 41},
	{140, 112, 30, 14, 30, 30, 41},
	{160, 128, 30, 14, 30, 30, 41},
	{170, 144, 30, 14, 30, 30, 41},
	{200, 4, 30, 14, 30, 30, 41},
	{220, 32, 30, 14, 30, 30, 41},
	{230, 64, 30, 14, 30, 30, 41},
	{233, 96, 30, 14, 30, 30, 41},
	{236, 128, 30, 14, 30, 30, 41},
	{239, 160, 30, 14, 30, 30, 41},
	{243, 192, 30, 14, 30, 30, 41},
	{247, 224, 30, 14, 30, 30, 41},
	{251, 256, 30, 14, 30, 30, 41},
	{300, 512, 30, 14, 30, 30, 41},
	{330, 768, 30, 14, 30, 30, 41},
	{360, 1024, 30, 14, 30, 30, 41},
	{400, 1280, 30, 14, 30, 30, 41},
	{420, 1024, 30, 14, 30, 30, 41},
	{450, 768, 30, 14, 30, 30, 41},
	{500, 5120, 30, 14, 30, 30, 41}
};

static qrs_timings g2_death_curve[G2_DEATH_CURVE_MAX] =
{
    {0, 5120, 30, 10, 16, 12, 12},
    {101, 5120, 26, 10, 12, 6, 6},
    {200, 5120, 26, 9, 12, 6, 6},
    {201, 5120, 22, 9, 12, 6, 6},
    {300, 5120, 22, 8, 12, 6, 6},
    {301, 5120, 18, 8, 6, 6, 6},
    {400, 5120, 18, 6, 6, 6, 6},
    {401, 5120, 15, 6, 5, 5, 5},
    {500, 5120, 15, 6, 4, 4, 4}
};

static qrs_timings g3_terror_curve[G3_TERROR_CURVE_MAX] =
{
    {0, 5120, 18, 8, 10, 6, 6},
    {100, 5120, 18, 6, 10, 5, 5},
    {200, 5120, 17, 6, 10, 4, 4},
    {300, 5120, 15, 6, 4, 4, 4},
    {500, 5120, 13, 4, 4, 3, 3},
    {600, 5120, 12, 4, 4, 3, 3},
    {1100, 5120, 10, 4, 4, 3, 3},
    {1200, 5120, 8, 4, 4, 3, 3},
    {1300, 5120, 15, 4, 4, 4, 6}
};

int g2_advance_garbage[12][24] =
{
    {QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL,
     QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL,
     QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL,
     QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL},

     {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1,   // column 1 y
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},

     {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1,   // column 2 y
      1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},

     {0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1,   // column 3
      1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},

     {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1,   // column 4
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},

     {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1,   // column 5
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},

     {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1,   // column 6
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},

     {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1,   // column 7
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},

     {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1,   // column 8
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},

     {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1,   // column 9
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},

     {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1,   // column 10
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},

     {QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL,
      QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL,
      QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL,
      QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL, QRS_WALL},
};


const char *get_grade_name(int index)
{
    if(index < 0 || index > 36)
        return " --";

    else return grade_names[index];
}

game_t *qs_game_create(coreState *cs, int level, unsigned int flags, char *replay_fname)
{
    game_t *g = malloc(sizeof(game_t));
	qrsdata *q = NULL;
	qrs_player *p = NULL;

	g->origin = cs;
	g->field = qrsfield_create();

	g->init = qs_game_init;
	g->quit = qs_game_quit;
    g->preframe = qs_game_preframe;
	g->input = &qrs_input;
	g->frame = qs_game_frame;
	g->draw = gfx_drawqs;

    g->frame_counter = 0;

	g->data = malloc(sizeof(qrsdata));
	q = (qrsdata *)(g->data);

    q->mode_flags = flags;

	q->piecepool = qrspool_create();
	q->timer = nz_timer_create(60);
	q->p1 = malloc(sizeof(qrs_player));
		p = q->p1;
		p->def = NULL;
		p->speeds = NULL;
		p->state = PSINACTIVE;
		p->x = 0;
		p->y = 0;
		p->orient = FLAT;

	q->p1counters = malloc(sizeof(qrs_counters));
		q->p1counters->init = 0;
		q->p1counters->lock = 0;
		q->p1counters->are = 0;
		q->p1counters->lineare = 0;
		q->p1counters->lineclear = 0;
        q->p1counters->floorkicks = 0;
        q->p1counters->hold_flash = 0;

    q->randomizer = NULL;

    if(replay_fname) {
        qrs_load_replay(g, replay_fname);
        if(!q->replay) {
            free(q->p1);
            free(q->p1counters);
            nz_timer_destroy(q->timer);
            free(q->piecepool);
            free(q);
            grid_destroy(g->field);
            free(g);

            return NULL;
        }

        flags = q->replay->mode_flags;
        level = q->replay->starting_level;
    } else {
        q->replay = NULL;
    }

    q->recording = 0;
    q->playback = 0;
    q->playback_index = 0;

    q->is_practice = 0;

    //q->field_tex = SDL_CreateTexture(cs->screen.renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 12*16, 20*16);
    //q->previous_field = NULL;
    //q->field_deltas = qrsfield_create();

    q->garbage = NULL;
    q->garbage_row_index = 0;
    q->garbage_counter = 0;
    q->garbage_delay = 0;

    q->previews[0] = NULL;
    q->previews[1] = NULL;
    q->previews[2] = NULL;
    q->hold = NULL;

    q->field_x = QRS_FIELD_X;
    q->field_y = QRS_FIELD_Y;

    q->field_w = 12;
    q->randomizer_type = RANDOMIZER_NORMAL;
    q->tetromino_only = 0;
    q->pentomino_only = 0;
    q->lock_delay_enabled = 1;
    q->lock_protect = 1;
    q->num_previews = 3;
    q->hold_enabled = 0;
    q->special_irs = 1;

    q->state_flags = 0;

    q->max_floorkicks = 3;
    q->lock_on_rotate = 0;

    request_fps(cs, 60);
    q->game_type = 0;
    q->mode_type = MODE_UNSPECIFIED;

    q->grade = -1;

    if(flags & MODE_G2_DEATH)
    {
        q->mode_type = MODE_G2_DEATH;
        //q->field_x = QRS_FIELD_X - 28;
        //q->field_y = QRS_FIELD_Y - 16 + 2;
        q->lock_protect = 1;
        flags |= SIMULATE_G2;
        flags |= TETROMINO_ONLY;
        flags &= ~SIMULATE_G1;
        flags &= ~SIMULATE_G3;
    }
    else if(flags & MODE_G3_TERROR)
    {
        q->mode_type = MODE_G3_TERROR;
        q->lock_protect = 1;
        flags |= SIMULATE_G3;
        flags |= TETROMINO_ONLY;
        flags &= ~SIMULATE_G1;
        flags &= ~SIMULATE_G2;
        if(level < 1000 && level >= 500) {
            q->state_flags |= GAMESTATE_RISING_GARBAGE;
        } else if(level >= 1000) {
            q->state_flags |= GAMESTATE_BRACKETS;
        }
    }
    else if(flags & MODE_G1_MASTER)
    {
        q->mode_type = MODE_G1_MASTER;
        q->grade = GRADE_9;
        flags |= SIMULATE_G1;
        flags |= TETROMINO_ONLY;
        flags &= ~SIMULATE_G2;
        flags &= ~SIMULATE_G3;
    }
    else if(flags & MODE_G1_20G)
    {
        q->mode_type = MODE_G1_20G;
        q->grade = GRADE_9;
        flags |= SIMULATE_G1;
        flags |= TETROMINO_ONLY;
        flags &= ~SIMULATE_G2;
        flags &= ~SIMULATE_G3;
    }

    if(flags & NIGHTMARE_MODE)
        q->randomizer_type = RANDOMIZER_NIGHTMARE;
    if(flags & NO_LOCK_DELAY)
        q->lock_delay_enabled = 0;
    if(flags & PENTOMINO_ONLY)
        q->pentomino_only = 1;

    if(flags & TETROMINO_ONLY) {
        q->field_w = 10;
        qrsfield_set_w(g->field, 10);
        q->tetromino_only = 1;
    }

    if(flags & SIMULATE_G1) {
        q->tetromino_only = 1;
        q->randomizer_type = RANDOMIZER_G1;
        q->game_type = SIMULATE_G1;
        q->num_previews = 1;
        q->max_floorkicks = 0;
        q->special_irs = 0;
        q->lock_protect = 0;
        q->piecepool[QRS_I4]->flags &= ~PDNOWKICK;
    }

    if(flags & SIMULATE_G2) {
        q->tetromino_only = 1;
        q->randomizer_type = RANDOMIZER_G2;
        q->game_type = SIMULATE_G2;
        q->num_previews = 1;
        q->max_floorkicks = 0;
        q->special_irs = 0;
        q->piecepool[QRS_I4]->flags &= ~PDNOWKICK;
        request_fps(cs, G2_FPS);
    }

    if(flags & SIMULATE_G3) {
        q->tetromino_only = 1;
        q->randomizer_type = RANDOMIZER_G3;
        q->game_type = SIMULATE_G3;
        q->num_previews = 3;
        q->max_floorkicks = 1;
        q->special_irs = 0;
        q->lock_protect = 1;
        q->hold_enabled = 1;
    }

    uint32_t randomizer_flags = 0;

    switch(q->randomizer_type) {
        case RANDOMIZER_NORMAL:
            if(q->pentomino_only)
                randomizer_flags |= PENTO_RAND_NOTETS;

            q->randomizer = pento_randomizer_create(randomizer_flags);
            break;

        case RANDOMIZER_NIGHTMARE:
            randomizer_flags |= PENTO_RAND_NIGHTMARE;
            q->randomizer = pento_randomizer_create(randomizer_flags);
            break;

        case RANDOMIZER_G1:
            q->randomizer = g1_randomizer_create(randomizer_flags);
            break;

        case RANDOMIZER_G2:
            q->randomizer = g2_randomizer_create(randomizer_flags);
            break;

        case RANDOMIZER_G3:
            q->randomizer = g2_randomizer_create(randomizer_flags);
            break;

        default:
            q->randomizer = pento_randomizer_create(randomizer_flags);
            break;
    }

    q->randomizer_seed = 0;

    q->level = 0;
    q->section = 0;
    q->lastclear = 0;
    q->locking_row = -1;
    q->lock_held = 0;
    q->lvlinc = 0;
    q->combo = 0;
    q->combo_simple = 0;
    q->singles = 0;
    q->doubles = 0;
    q->triples = 0;
    q->tetrises = 0;
    q->pentrises = 0;
    q->recoveries = 0;
    q->is_recovering = 0;
    q->medal_re = 0;
    q->medal_sk = 0;
    q->medal_st = 0;
    q->medal_co = 0;
    q->speed_curve_index = 0;
    q->music = 0;
    q->mute = 1;

    if(flags & QRS_PRACTICE) {
        q->is_practice = 1;

        if(!cs->pracdata_mirror) {
            q->pracdata = malloc(sizeof(struct pracdata));

            q->pracdata->field_w = 10;
            q->pracdata->game_type = SIMULATE_G2;
            //q->pracdata->long_history = NULL;   // unused at the moment
            q->pracdata->usr_seq_expand_len = 0;
            q->pracdata->usr_seq_len = 0;
            q->pracdata->usr_field_undo = NULL;
            q->pracdata->usr_field_redo = NULL;
            q->pracdata->usr_field_undo_len = 0;
            q->pracdata->usr_field_redo_len = 0;
            q->pracdata->field_edit_in_progress = 0;
        	q->pracdata->usr_field = qrsfield_create();
            q->pracdata->palette_selection = -5;
            q->pracdata->field_selection = 0;
            q->pracdata->field_selection_vertex1_x = 0;
            q->pracdata->field_selection_vertex1_y = 0;
            q->pracdata->field_selection_vertex2_x = 0;
            q->pracdata->field_selection_vertex2_y = 0;
            if(flags & TETROMINO_ONLY) {
                qrsfield_set_w(q->pracdata->usr_field, 10);
                q->field_w = 10;
            }

            q->pracdata->usr_timings = malloc(sizeof(qrs_timings));
            q->pracdata->usr_timings->level = 0;
            q->pracdata->usr_timings->are = 30;
            q->pracdata->usr_timings->grav = 4;
            q->pracdata->usr_timings->lock = 30;
            q->pracdata->usr_timings->das = 14;
            q->pracdata->usr_timings->lineare = 30;
            q->pracdata->usr_timings->lineclear = 40;
        	q->pracdata->hist_index = 0;
        	q->pracdata->paused = QRS_FIELD_EDIT;
        	q->pracdata->grid_lines_shown = 0;
            q->pracdata->brackets = 0;
            q->pracdata->invisible = 0;
            q->pracdata->infinite_floorkicks = 0;
            q->pracdata->lock_protect = -1;
            if(flags & TETROMINO_ONLY)
                q->pracdata->piece_subset = SUBSET_TETS;
            else if(flags & PENTOMINO_ONLY)
                q->pracdata->piece_subset = SUBSET_PENTS;
            else
                q->pracdata->piece_subset = SUBSET_ALL;
        	q->pracdata->randomizer_seed = 0;

            cs->pracdata_mirror = q->pracdata;
        } else {
            q->pracdata = cs->pracdata_mirror;
            q->field_w = q->pracdata->field_w;
            q->game_type = q->pracdata->game_type;
        }
    } else
        q->pracdata = NULL;

    if(!level)
        q->p1->speeds = &qs_curve[0];
    else {
        q->level = level;
        q->section = level / 100;
        q->p1->speeds = &qs_curve[8];
    }

	return g;
}

int qs_game_init(game_t *g)
{
    if(!g)
		return -1;

	qrsdata *q = (qrsdata *)(g->data);
    qrs_player *p = q->p1;

    piece_id next1_id, next2_id, next3_id;

    /*SDL_SetRenderTarget(g->origin->screen.renderer, q->field_tex);
    SDL_SetRenderDrawColor(g->origin->screen.renderer, 0, 0, 0, 0);
    SDL_SetRenderDrawBlendMode(g->origin->screen.renderer, SDL_BLENDMODE_NONE);
    SDL_RenderClear(g->origin->screen.renderer);
    SDL_SetRenderDrawBlendMode(g->origin->screen.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(g->origin->screen.renderer, 0, 0, 0, 255);
    SDL_SetRenderTarget(g->origin->screen.renderer, NULL);*/

    //gfx_createbutton(g->origin, "TEST", 31*16 - 6, 16 - 6, 0, toggle_obnoxious_text, NULL, NULL, RGBA_DEFAULT);

	/*int i = 0;
	int j = 0;
	int t = 0;*/

    if(q->replay) {
        *(q->randomizer->seedp) = q->replay->seed;
        q->randomizer_seed = q->replay->seed;
    } else {
        q->randomizer_seed = *(q->randomizer->seedp);
    }

    printf("Random seed: %ld\n", q->randomizer_seed);

	if(q->randomizer)
        q->randomizer->init(q->randomizer);

    next1_id = q->randomizer->lookahead(q->randomizer, 1);
    next2_id = q->randomizer->lookahead(q->randomizer, 2);
    next3_id = q->randomizer->lookahead(q->randomizer, 3);

    if(q->randomizer->num_pieces == 7) {
        next1_id = ars_to_qrs_id(next1_id);
        next2_id = ars_to_qrs_id(next2_id);
        next3_id = ars_to_qrs_id(next3_id);
    }

    if(q->pracdata) {
        if(q->pracdata->usr_seq_len) {
            q->previews[0] = qrspiece_cpy(q->piecepool, qs_get_usrseq_elem(q->pracdata, 0));
            q->previews[1] = qrspiece_cpy(q->piecepool, qs_get_usrseq_elem(q->pracdata, 1));
            q->previews[2] = qrspiece_cpy(q->piecepool, qs_get_usrseq_elem(q->pracdata, 2));
        } else {
            q->previews[0] = qrspiece_cpy(q->piecepool, next1_id);
            q->previews[1] = qrspiece_cpy(q->piecepool, next2_id);
            q->previews[2] = qrspiece_cpy(q->piecepool, next3_id);
        }
    } else {
        q->previews[0] = qrspiece_cpy(q->piecepool, next1_id);
        q->previews[1] = qrspiece_cpy(q->piecepool, next2_id);
        q->previews[2] = qrspiece_cpy(q->piecepool, next3_id);
    }

    if(q->state_flags & GAMESTATE_BRACKETS) {
        if(q->previews[0])
            q->previews[0]->flags |= PDBRACKETS;

        if(q->previews[1])
            q->previews[1]->flags |= PDBRACKETS;

        if(q->previews[2])
            q->previews[2]->flags |= PDBRACKETS;
    }

    if(q->cur_piece_qrs_id >= 18)
    	p->y = ROWTOY(SPAWNY_QRS + 2);
    else
   	    p->y = ROWTOY(SPAWNY_QRS);

    p->def = NULL;
    p->x = SPAWNX_QRS;

	q->p1->state = PSFALL;

    if(!g->origin->assets->entry)   // used to check if we are in a testing environment
        return 0;

    bstring bgname = NULL;
    if(q->section < 10)
        bgname = bformat("bg%d", q->section);
    else
        bgname = bfromcstr("bg9");

    if(!q->pracdata) {
        g->origin->bg = (asset_by_name(g->origin, (char *)(bgname->data)))->data;
        gfx_start_bg_fade_in(g->origin);

        if(q->mode_type == MODE_G2_DEATH) {
            //g->origin->anim_bg = g->origin->g2_bgs[q->section > 9 ? 9 : q->section];
            //g->origin->anim_bg_old = g->origin->anim_bg;
        }
    } else {
        q->p1->state = PSINACTIVE;
    }

    if(bgname) {
        bdestroy(bgname);
    }

	return 0;
}

int qs_game_pracinit(game_t *g, int val)
{
    coreState *cs = g->origin;
    g = cs->p1game;
    qrsdata *q = g->data;
    qrs_player *p = q->p1;
    qrs_counters *c = q->p1counters;
    piece_id next1_id, next2_id, next3_id;

    cs->menu_input_override = 0;

    gridcpy(q->pracdata->usr_field, g->field);

    c->init = 0;
    c->lock = 0;
    c->are = 0;
    c->lineare = 0;
    c->lineclear = 0;
    c->floorkicks = 0;
    q->timer->time = 0;

    q->p1->speeds = q->pracdata->usr_timings;
    if(q->pracdata->usr_timings->lock < 0)
        q->lock_delay_enabled = 0;
    else
        q->lock_delay_enabled = 1;

    q->state_flags = 0;

    if(q->pracdata->invisible)
        q->state_flags |= GAMESTATE_INVISIBLE;
    if(q->pracdata->brackets)
        q->state_flags |= GAMESTATE_BRACKETS;

    if(q->previews[0])
        piecedef_destroy(q->previews[0]);
    if(q->previews[1])
        piecedef_destroy(q->previews[1]);
    if(q->previews[2])
        piecedef_destroy(q->previews[2]);

    if(q->pracdata->usr_seq_len) {
        q->pracdata->hist_index = -1;

        q->previews[0] = qrspiece_cpy(q->piecepool, qs_get_usrseq_elem(q->pracdata, 0));
        q->previews[1] = qrspiece_cpy(q->piecepool, qs_get_usrseq_elem(q->pracdata, 1));
        q->previews[2] = qrspiece_cpy(q->piecepool, qs_get_usrseq_elem(q->pracdata, 2));
    } else {
        q->randomizer->init(q->randomizer);

        next1_id = q->randomizer->lookahead(q->randomizer, 1);
        next2_id = q->randomizer->lookahead(q->randomizer, 2);
        next3_id = q->randomizer->lookahead(q->randomizer, 3);

        if(q->randomizer->num_pieces == 7) {
            next1_id = ars_to_qrs_id(next1_id);
            next2_id = ars_to_qrs_id(next2_id);
            next3_id = ars_to_qrs_id(next3_id);
        }

        q->previews[0] = qrspiece_cpy(q->piecepool, next1_id);
        q->previews[1] = qrspiece_cpy(q->piecepool, next2_id);
        q->previews[2] = qrspiece_cpy(q->piecepool, next3_id);
    }

    if(q->state_flags & GAMESTATE_BRACKETS) {
        if(q->previews[0])
            q->previews[0]->flags |= PDBRACKETS;

        if(q->previews[1])
            q->previews[1]->flags |= PDBRACKETS;

        if(q->previews[2])
            q->previews[2]->flags |= PDBRACKETS;
    }

    if(q->cur_piece_qrs_id >= 18)
    	p->y = ROWTOY(SPAWNY_QRS + 2);
    else
   	    p->y = ROWTOY(SPAWNY_QRS);

    p->def = NULL;
    p->x = SPAWNX_QRS;

	q->p1->state = PSFALL;
    q->level = 0;
    q->pracdata->paused = 0;

    return 0;
}

int qs_game_quit(game_t *g)
{
    if(!g)
		return -1;

    //qrsdata *q = g->data;

    keyflags_init(g->origin->keys[0]);

	if(g->field)
		grid_destroy(g->field);

	if(g->data)
		qrsdata_destroy(g->data);

	Mix_HaltMusic();
    gfx_quit(g->origin);

    // mostly a band-aid for quitting practice tool properly, so menu input does not take priority for regular modes
    g->origin->menu_input_override = 0;

	return 0;
}

int qs_game_preframe(game_t *g)
{
    return 0;
}

int qs_game_frame(game_t *g)
{
    if(!g)
		return -1;

	coreState *cs = g->origin;
	qrsdata *q = (qrsdata *)(g->data);

	unsigned int *s = &q->p1->state;
	qrs_counters *c = q->p1counters;

    struct asset *ready = asset_by_name(cs, "ready");
    struct asset *go = asset_by_name(cs, "go");
    struct asset *track0 = asset_by_name(cs, "track0");
    struct asset *track1 = asset_by_name(cs, "track1");
    struct asset *track2 = asset_by_name(cs, "track2");
    struct asset *track3 = asset_by_name(cs, "track3");
    struct asset *medal = asset_by_name(cs, "medal");

    bstring bgname = NULL;

	/*int n = 0;
    int row = 0;
    int i = 0;
    int j = 0;*/

    if(q->pracdata) {
        if(q->pracdata->paused)
            return 0;
    }

	if(c->init < 120) {
		if(c->init == 0) {
			gfx_pushmessage(cs, "READY", (6*16 + 8 - QRS_FIELD_X + q->field_x), (15*16 + 8 - QRS_FIELD_Y + q->field_y),
                            0, 60, qrs_game_is_inactive, RGBA_DEFAULT);

            play_sfx(ready->data, ready->volume);
		}

		if(c->init == 60) {
			gfx_pushmessage(cs, "GO", (8*16 - QRS_FIELD_X + q->field_x), (15*16 + 8 - QRS_FIELD_Y + q->field_y),
                            0, 60, qrs_game_is_inactive, RGBA_DEFAULT);

            play_sfx(go->data, go->volume);
		}

        if(c->init == 119 && !q->pracdata) {
            if(q->replay) {
                qrs_start_playback(g);
            } else {
                qrs_start_record(g);
            }
        }

		(*s) = PSINACTIVE;
		c->init++;
		return 0;
	} else if(c->init == 120) {
		c->init++;
		qs_initnext(g, q->p1, 0);
		qrs_proc_initials(g);

        if(qrs_chkcollision(g, q->p1)) {
            if(q->p1->def->flags & PDBRACKETS)
                qrs_lock(g, q->p1, LOCKPIECE_BRACKETS);
            else
                qrs_lock(g, q->p1, 0);
            (*s) = PSINACTIVE;
            Mix_HaltMusic();
            if(q->playback)
                qrs_end_playback(g);
            else if(q->recording)
                qrs_end_record(g);
            return 0;
        }

        if(!q->pracdata) {
            switch(q->mode_type) {
                case MODE_UNSPECIFIED:
                    if(q->level < 300) {
                        play_track(cs, track0->data, track0->volume);
                        q->music = 0;
                    } else if(q->level < 500) {
                        play_track(cs, track1->data, track1->volume);
                        q->music = 1;
                    } else if(q->level < 700) {
                        play_track(cs, track2->data, track2->volume);
                        q->music = 2;
                    } else {
                        play_track(cs, track3->data, track3->volume);
                        q->music = 3;
                    }

                    break;

                case MODE_G2_DEATH:
                    if(q->level < 300) {
                        play_track(cs, (asset_by_name(cs, "g2/track1"))->data, (asset_by_name(cs, "g2/track1"))->volume);
                        q->music = 1;
                    } else if(q->level < 500) {
                        play_track(cs, (asset_by_name(cs, "g2/track2"))->data, (asset_by_name(cs, "g2/track2"))->volume);
                        q->music = 2;
                    } else {
                        play_track(cs, (asset_by_name(cs, "g2/track3"))->data, (asset_by_name(cs, "g2/track3"))->volume);
                        q->music = 3;
                    }

                    break;

                case MODE_G3_TERROR:
                    if(q->level < 500) {
                        play_track(cs, (asset_by_name(cs, "g3/track2"))->data, (asset_by_name(cs, "g3/track2"))->volume);
                        q->music = 1;
                    } else if(q->level < 700) {
                        play_track(cs, (asset_by_name(cs, "g3/track3"))->data, (asset_by_name(cs, "g3/track3"))->volume);
                        q->music = 2;
                    } else if(q->level < 1000) {
                        play_track(cs, (asset_by_name(cs, "g3/track4"))->data, (asset_by_name(cs, "g3/track4"))->volume);
                        q->music = 3;
                    } else {
                        play_track(cs, (asset_by_name(cs, "g3/track5"))->data, (asset_by_name(cs, "g3/track5"))->volume);
                        q->music = 3;
                    }

                    break;

                case MODE_G1_MASTER:
                case MODE_G1_20G:
                    if(q->level < 500) {
                        play_track(cs, (asset_by_name(cs, "g1/track0"))->data, (asset_by_name(cs, "g1/track0"))->volume);
                        q->music = 0;
                    } else {
                        play_track(cs, (asset_by_name(cs, "g1/track1"))->data, (asset_by_name(cs, "g2/track1"))->volume);
                        q->music = 1;
                    }

                    break;

                default:
                    break;
            }

            q->mute = 0;
        }
	}

	if((*s) == PSINACTIVE) {
        //printf("Currently inactive\n");
		return 0;
    }

    if(!q->pracdata) {
        if(q->mode_type == MODE_G2_DEATH) {
            for(; q->speed_curve_index < G2_DEATH_CURVE_MAX && g2_death_curve[q->speed_curve_index].level <= q->level;) {
                q->p1->speeds = &g2_death_curve[q->speed_curve_index];
                q->speed_curve_index++;
            }
        } else if(q->mode_type == MODE_G3_TERROR) {
            for(; q->speed_curve_index < G3_TERROR_CURVE_MAX && g3_terror_curve[q->speed_curve_index].level <= q->level;) {
                q->p1->speeds = &g3_terror_curve[q->speed_curve_index];
                q->speed_curve_index++;
            }
        } else if(q->mode_type == MODE_G1_MASTER) {
            for(; q->speed_curve_index < G1_MASTER_CURVE_MAX && g1_master_curve[q->speed_curve_index].level <= q->level;) {
                q->p1->speeds = &g1_master_curve[q->speed_curve_index];
                q->speed_curve_index++;
            }
        } else if(q->mode_type == MODE_G1_20G) {
            q->p1->speeds = &g1_master_curve[G1_MASTER_CURVE_MAX - 1];
        } else {
            for(; q->speed_curve_index < QS_CURVE_MAX && qs_curve[q->speed_curve_index].level <= q->level;) {
                q->p1->speeds = &qs_curve[q->speed_curve_index];
                q->speed_curve_index++;
            }
        }

        switch(q->mode_type) {
            case MODE_UNSPECIFIED:
                if(q->level > 290 && q->level < 300 && !q->mute) {
                    Mix_HaltMusic();
                    q->mute = 1;
                }
                if(q->level >= 300 && q->level < 315 && q->mute) {
                    play_track(cs, track1->data, track1->volume);
                    q->music = 1;
                    q->mute = 0;
                }

                if(q->level > 490 && q->level < 500 && !q->mute) {
                    Mix_HaltMusic();
                    q->mute = 1;
                }
                if(q->level >= 500 && q->level < 515 && q->mute) {
                    play_track(cs, track2->data, track2->volume);
                    q->music = 2;
                    q->mute = 0;
                }

                if(q->level > 690 && q->level < 700 && !q->mute) {
                    Mix_HaltMusic();
                    q->mute = 1;
                }
                if(q->level >= 700 && q->level < 715 && q->mute) {
                    play_track(cs, track3->data, track3->volume);
                    q->music = 3;
                    q->mute = 0;
                }

                break;

            case MODE_G2_DEATH:
                if(q->level >= 280 && q->level < 300 && !q->mute) {
                    Mix_HaltMusic();
                    q->mute = 1;
                }
                if(q->level >= 300 && q->level < 315 && q->mute) {
                    play_track(cs, (asset_by_name(cs, "g2/track2"))->data, (asset_by_name(cs, "g2/track2"))->volume);
                    q->music = 2;
                    q->mute = 0;
                }

                if(q->level >= 480 && q->level < 500 && !q->mute) {
                    Mix_HaltMusic();
                    q->mute = 1;
                }
                if(q->level >= 500 && q->level < 515 && q->mute) {
                    play_track(cs, (asset_by_name(cs, "g2/track3"))->data, (asset_by_name(cs, "g2/track3"))->volume);
                    q->music = 3;
                    q->mute = 0;
                }

                break;

            case MODE_G3_TERROR:
                if(q->level >= 480 && q->level < 500 && !q->mute) {
                    Mix_HaltMusic();
                    q->mute = 1;
                }
                if(q->level >= 500 && q->level < 515 && q->mute) {
                    play_track(cs, (asset_by_name(cs, "g3/track3"))->data, (asset_by_name(cs, "g3/track3"))->volume);
                    q->music = 3;
                    q->mute = 0;
                }

                if(q->level >= 680 && q->level < 700 && !q->mute) {
                    Mix_HaltMusic();
                    q->mute = 1;
                }
                if(q->level >= 700 && q->level < 715 && q->mute) {
                    play_track(cs, (asset_by_name(cs, "g3/track4"))->data, (asset_by_name(cs, "g3/track4"))->volume);
                    q->music = 4;
                    q->mute = 0;
                }

                if(q->level >= 980 && q->level < 1000 && !q->mute) {
                    Mix_HaltMusic();
                    q->mute = 1;
                }
                if(q->level >= 1000 && q->level < 1015 && q->mute) {
                    play_track(cs, (asset_by_name(cs, "g3/track5"))->data, (asset_by_name(cs, "g3/track5"))->volume);
                    q->music = 5;
                    q->mute = 0;
                }

                break;

            case MODE_G1_MASTER:
            case MODE_G1_20G:
                if(q->level >= 485 && q->level < 500 && !q->mute) {
                    Mix_HaltMusic();
                    q->mute = 1;
                }
                if(q->level >= 500 && q->level < 515 && q->mute) {
                    play_track(cs, (asset_by_name(cs, "g1/track1"))->data, (asset_by_name(cs, "g1/track1"))->volume);
                    q->music = 1;
                    q->mute = 0;
                }

                break;

            default:
                break;
        }

    }

    if((*s) & PSSPAWN) {
        qs_process_fall(g);
        (*s) &= ~PSSPAWN;
        //printf("First piece\n");
    } else {
        if(qs_process_are(g) == QSGAME_SHOULD_TERMINATE) {
            if(q->pracdata) {
                q->pracdata->paused = 1;
            }

            q->p1->state = PSINACTIVE;
        }

        if((*s) & PSSPAWN) {
            qs_process_fall(g);
            (*s) &= ~PSSPAWN;
        } else {
            qs_process_prelockflash(g);
            qs_process_fall(g);
            qs_process_lock(g);
            qs_process_lockflash(g);
            qs_process_lineclear(g);
            qs_process_lineare(g);
            if((*s) & PSSPAWN) {
                qs_process_fall(g);
                (*s) &= ~PSSPAWN;
            }
        }
    }

/*
    qs_process_are(g);
    qs_process_prelockflash(g);
    qs_process_lock(g);
    qs_process_fall(g);
    qs_process_lockflash(g);
    qs_process_lineclear(g);
    qs_process_lineare(g);
    if((*s) & PSSPAWN) {
        qs_process_fall(g);
        (*s) &= ~PSSPAWN;
    }
*/
    if((*s) == PSINACTIVE)
        return 0;

    if(!q->pracdata && q->is_recovering && q->game_type == 0) {
        if(grid_cells_filled(g->field) <= 70) {
            q->recoveries++;
            q->is_recovering = 0;
            switch(q->recoveries) {
                case 1:
                    q->medal_re = BRONZE;
                    play_sfx(medal->data, medal->volume);
                    break;
                case 2:
                    q->medal_re = SILVER;
                    play_sfx(medal->data, medal->volume);
                    break;
                case 3:
                    q->medal_re = GOLD;
                    play_sfx(medal->data, medal->volume);
                    break;
                case 5:
                    q->medal_re = PLATINUM;
                    play_sfx(medal->data, medal->volume);
                    break;
                default:
                    break;
            }
        }
    } else if(grid_cells_filled(g->field) >= 150)
        q->is_recovering = 1;

    /*for(i = 0; i < QRS_FIELD_W; i++) {
        for(j = 0; j < QRS_FIELD_H; j++) {
            if(gridgetcell(g->field, i, j) != gridgetcell(q->previous_field, i, j)) {
                gridsetcell(q->field_deltas, i, j, 1);
            } else
                gridsetcell(q->field_deltas, i, j, 0);
        }
    }*/

    if(!q->pracdata && q->mode_type == MODE_G3_TERROR) {
        if(q->level >= 1000) {
            q->state_flags |= GAMESTATE_BRACKETS;
            q->state_flags &= ~GAMESTATE_RISING_GARBAGE;
        } else if(q->level >= 500) {
            q->state_flags |= GAMESTATE_RISING_GARBAGE;
            switch(q->section) {
                case 5:
                    q->garbage_delay = 20;
                    break;
                case 6:
                    q->garbage_delay = 18;
                    break;
                case 7:
                    q->garbage_delay = 10;
                    break;
                case 8:
                    q->garbage_delay = 9;
                    break;
                case 9:
                    q->garbage_delay = 8;
                    break;
                default:
                    break;
            }
        }

        if(q->section)
            q->grade = GRADE_S1 + q->section - 1;
    }

    if(q->pracdata) {
        //if(!q->pracdata->paused && (*s) & PSARE) printf("ARE = %d\n", c->are);
    }

    if(!(q->state_flags & GAMESTATE_CREDITS))
	   timeinc(q->timer);

    if(bgname)
    {
        bdestroy(bgname);
    }

	return 0;
}

int qs_process_are(game_t *g)
{
    //coreState *cs = g->origin;
    qrsdata *q = g->data;
    unsigned int *s = &q->p1->state;
    qrs_counters *c = q->p1counters;

    if((*s) & PSLOCKFLASH4)
        (*s) &= ~PSLOCKFLASH4;

    if((*s) & PSLOCKFLASH3) {
        (*s) &= ~PSLOCKFLASH3;
        (*s) |= PSLOCKFLASH4;
    }

    if((*s) & PSLOCKFLASH2) {
        (*s) &= ~PSLOCKFLASH2;
        (*s) |= PSLOCKFLASH3;

        if(!q->lastclear)
            (*s) |= PSARE;
    }

    if((*s) & PSARE) {
		if(c->are == q->p1->speeds->are) {
			q->lastclear = 0;
            if(q->level % 100 != 99) {
                switch(q->mode_type) {
                    case MODE_G2_DEATH:
                    case MODE_G1_20G:
                    case MODE_G1_MASTER:
                        if(q->level != 998) {
                            q->level++;
                            q->lvlinc = 1;
                        } else
                            q->lvlinc = 0;

                        break;

                    default:
                        q->level++;
                        q->lvlinc = 1;
                        break;
                }
            } else
                q->lvlinc = 0;

			c->are = 0;
			c->lock = 0;
			if(qs_initnext(g, q->p1, 0) == QSGAME_SHOULD_TERMINATE) {
                return QSGAME_SHOULD_TERMINATE;
            }

			qrs_proc_initials(g);

			if(qrs_chkcollision(g, q->p1)) {
                if(q->p1->def->flags & PDBRACKETS)
                    qrs_lock(g, q->p1, LOCKPIECE_BRACKETS);
                else
                    qrs_lock(g, q->p1, 0);
				(*s) = PSINACTIVE;
				Mix_HaltMusic();
                if(q->playback)
                    qrs_end_playback(g);
                else if(q->recording)
                    qrs_end_record(g);
				return 0;
			}

            if(q->state_flags & GAMESTATE_RISING_GARBAGE) {
                switch(q->mode_type) {
                    case MODE_G3_TERROR:
                        q->garbage_counter++;
                        break;

                    default:
                        break;
                }
            }
		} else {
			c->are++;
            if(c->are == 1) {
                if(q->state_flags & GAMESTATE_RISING_GARBAGE) {
                    switch(q->mode_type) {
                        case MODE_G3_TERROR:
                            if(q->garbage_counter >= q->garbage_delay) {
                                qrs_spawn_garbage(g, GARBAGE_COPY_BOTTOM_ROW);
                                q->garbage_counter = 0;
                            }
                            break;

                        default:
                            break;
                    }
                }
            }
        }
	}

    return 0;
}

int qs_process_lineare(game_t *g)
{
    //coreState *cs = g->origin;
    qrsdata *q = g->data;
    unsigned int *s = &q->p1->state;
    qrs_counters *c = q->p1counters;

    if((*s) & PSLINEARE) {
		if(c->lineare == q->p1->speeds->lineare) {
			q->lastclear = 0;
            if(q->level % 100 != 99) {
                switch(q->mode_type) {
                    case MODE_G2_DEATH:
                    case MODE_G1_20G:
                    case MODE_G1_MASTER:
                        if(q->level != 998) {
                            q->level++;
                            q->lvlinc = 1;
                        } else
                            q->lvlinc = 0;

                        break;

                    default:
                        q->level++;
                        q->lvlinc = 1;
                        break;
                }
            } else
                q->lvlinc = 0;

			c->lineare = 0;
			c->lock = 0;
			if(qs_initnext(g, q->p1, 0) == QSGAME_SHOULD_TERMINATE)
                return QSGAME_SHOULD_TERMINATE;

			qrs_proc_initials(g);

			if(qrs_chkcollision(g, q->p1)) {
                if(q->p1->def->flags & PDBRACKETS)
                    qrs_lock(g, q->p1, LOCKPIECE_BRACKETS);
                else
                    qrs_lock(g, q->p1, 0);
				(*s) = PSINACTIVE;
				Mix_HaltMusic();
                if(q->playback)
                    qrs_end_playback(g);
                else if(q->recording)
                    qrs_end_record(g);
				return 0;
			}

            if(q->state_flags & GAMESTATE_RISING_GARBAGE) {
                switch(q->mode_type) {
                    case MODE_G3_TERROR:
                        q->garbage_counter++;
                        break;

                    default:
                        break;
                }
            }
		} else
			c->lineare++;
	}

    return 0;
}

int qs_process_lineclear(game_t *g)
{
    coreState *cs = g->origin;
    qrsdata *q = g->data;
    unsigned int *s = &q->p1->state;
    qrs_counters *c = q->p1counters;

    struct asset *dropfield = asset_by_name(cs, "dropfield");

    if((*s) & PSLINECLEAR) {
		if(c->lineclear == q->p1->speeds->lineclear) {
			(*s) &= ~PSLINECLEAR;
			(*s) |= PSLINEARE;

			c->lineclear = 0;
			qrs_dropfield(g);
            play_sfx(dropfield->data, dropfield->volume);
		} else
			c->lineclear++;
	}

    return 0;
}

int qs_process_lock(game_t *g)
{
    //coreState *cs = g->origin;
    qrsdata *q = g->data;
    unsigned int *s = &q->p1->state;
    qrs_counters *c = q->p1counters;

    if((*s) & PSLOCK && q->lock_delay_enabled) {
		if(qrs_isonground(g, q->p1)) {
            if(YTOROW(q->p1->y) != q->locking_row) {
                q->locking_row = YTOROW(q->p1->y);
                c->lock = 0;
            }

            if(!q->p1->speeds->lock) {
                (*s) &= ~PSLOCK;
                (*s) |= PSPRELOCKED;
            } else {
                c->lock++;
                if(c->lock == q->p1->speeds->lock) {
                    c->lock = 0;
                    /*if(q->state_flags & GAMESTATE_BRACKETS)
                        qrs_lock(g, q->p1, LOCKPIECE_BRACKETS);
                    else
                        qrs_lock(g, q->p1, 0);*/
                    (*s) &= ~PSLOCK;
                    (*s) |= PSPRELOCKED;
    			}
            }
		} else {
			(*s) &= ~PSLOCK;
			(*s) |= PSFALL;
		}
	} else if((*s) & PSLOCK && q->lock_on_rotate == 2) {
        if(q->pracdata) {
            if(!q->pracdata->infinite_floorkicks) {
                c->lock = 0;
                (*s) &= ~PSLOCK;
                (*s) |= PSPRELOCKED;
            }
        } else {
            c->lock = 0;
            (*s) &= ~PSLOCK;
            (*s) |= PSPRELOCKED;
        }

        q->lock_on_rotate = 0;
    }

    return 0;
}

int qs_process_fall(game_t *g)
{
    //coreState *cs = g->origin;
    qrsdata *q = g->data;
    unsigned int *s = &q->p1->state;
    qrs_counters *c = q->p1counters;

    int row = 0;

    if((*s) & PSFALL) {
        row = YTOROW(q->p1->y);
		qrs_fall(g, q->p1, 0);
        if(YTOROW(q->p1->y) < row) {
            c->lock = 0;
        }
	}

    return 0;
}

int qs_process_prelockflash(game_t *g)
{
    //coreState *cs = g->origin;
    qrsdata *q = g->data;
    unsigned int *s = &q->p1->state;
    //qrs_counters *c = q->p1counters;

    if((*s) & PSLOCKFLASH1) {
		(*s) &= ~PSLOCKFLASH1;
		(*s) |= PSLOCKFLASH2;
	}

    if((*s) & PSPRELOCKED) {
        (*s) &= ~PSPRELOCKED;
        if(q->p1->def->flags & PDBRACKETS)
            qrs_lock(g, q->p1, LOCKPIECE_BRACKETS);
        else
            qrs_lock(g, q->p1, 0);

        (*s) &= ~PSPRELOCKFLASH1;
        (*s) |= PSLOCKFLASH1;
    }

    if((*s) & PSLOCKPRESSED) {
        (*s) &= ~PSLOCKPRESSED;
        (*s) |= PSPRELOCKED;
    }

    if((*s) & PSPRELOCKFLASH1) {
        (*s) &= ~PSPRELOCKFLASH1;
        (*s) |= PSLOCKFLASH1;
    }

    return 0;
}

int qs_process_lockflash(game_t *g)
{
    coreState *cs = g->origin;
    qrsdata *q = g->data;
    unsigned int *s = &q->p1->state;
    //qrs_counters *c = q->p1counters;

    struct asset *lineclear = asset_by_name(cs, "lineclear");
    struct asset *newsection = asset_by_name(cs, "newsection");
    struct asset *medal = asset_by_name(cs, "medal");

    bstring bgname = NULL;
    int n = 0;

    if((*s) & PSLOCKFLASH1) {
		n = qrs_lineclear(g, q->p1);
		q->lastclear = n;

		if(n) {
			(*s) |= PSLINECLEAR;

            if(q->game_type == SIMULATE_G3 && n > 2)
                q->lvlinc = 2*n - 2;
            else
                q->lvlinc = n;

			q->level += q->lvlinc;
			q->combo += 2*n - 2;
            q->combo_simple += (n > 1);

            if(q->state_flags & GAMESTATE_RISING_GARBAGE) {
                switch(q->mode_type) {
                    case MODE_G3_TERROR:
                        q->garbage_counter -= n;
                        break;

                    default:
                        break;
                }
            }

            if(!q->pracdata && q->game_type == 0) {
                switch(q->combo_simple) {
                    case 3:
                        if(q->medal_co <= BRONZE)
                            q->medal_co = BRONZE;
                        play_sfx(medal->data, medal->volume);
                        break;
                    case 4:
                        if(q->medal_co <= SILVER)
                            q->medal_co = SILVER;
                        play_sfx(medal->data, medal->volume);
                        break;
                    case 5:
                        if(q->medal_co <= GOLD)
                            q->medal_co = GOLD;
                        play_sfx(medal->data, medal->volume);
                        break;
                    case 6:
                        if(q->medal_co <= PLATINUM)
                            q->medal_co = PLATINUM;
                        play_sfx(medal->data, medal->volume);
                        break;
                    default:
                        break;
                }

                if(n == 5 && q->mode_type == MODE_UNSPECIFIED) {
                    q->pentrises++;
                    switch(q->pentrises) {
                        case 2:
                            if(q->medal_sk <= BRONZE)
                                q->medal_sk = BRONZE;
                            play_sfx(medal->data, medal->volume);
                            break;
                        case 4:
                            if(q->medal_sk <= SILVER)
                                q->medal_sk = SILVER;
                            play_sfx(medal->data, medal->volume);
                            break;
                        case 7:
                            if(q->medal_sk <= GOLD)
                                q->medal_sk = GOLD;
                            play_sfx(medal->data, medal->volume);
                            break;
                        case 9:
                            if(q->medal_sk <= PLATINUM)
                                q->medal_sk = PLATINUM;
                            play_sfx(medal->data, medal->volume);
                            break;
                        default:
                            break;
                    }
                } else switch(n) {
                    case 4:
                        q->tetrises++;
                        break;
                    case 3:
                        q->triples++;
                        break;
                    case 2:
                        q->doubles++;
                        break;
                    case 1:
                        q->singles++;
                        break;
                    default:
                        break;
                }
            }

			play_sfx(lineclear->data, lineclear->volume);
            if(((q->level - q->lvlinc) % 100) > 90 && (q->level % 100) < 10) {
                q->section++;

                if(!q->pracdata) {
                    switch(q->mode_type) {
                        case MODE_G2_DEATH:
                            if(q->section == 5) {
                                if(q->timer->time > G2_DEATH_TORIKAN) {
                                    q->section--;
                                    q->p1->state = PSINACTIVE;
                                } else {
                                    q->grade = GRADE_M;
                                }
                            } else if(q->level > 999) {
                                q->level = 999;
                                q->grade = GRADE_GM;
                                if(q->playback)
                                    qrs_end_playback(g);
                                else if(q->recording)
                                    qrs_end_record(g);
                                q->p1->state = PSINACTIVE;
                            }

                            break;

                        case MODE_G3_TERROR:
                            if(q->section == 5) {
                                if(q->timer->time > G3_TERROR_TORIKAN) {
                                    q->section--;
                                    q->p1->state = PSINACTIVE;
                                } else {
                                    q->grade = GRADE_S5;
                                }
                            } else if(q->section == 10) {
                                if(q->timer->time > 2*G3_TERROR_TORIKAN) {
                                    q->section--;
                                    q->p1->state = PSINACTIVE;
                                } else {
                                    q->grade = GRADE_S10;
                                }
                            } else if(q->section == 13) {
                                q->level = 1300;
                                q->grade = GRADE_S13;
                                if(q->playback)
                                    qrs_end_playback(g);
                                else if(q->recording)
                                    qrs_end_record(g);
                                q->p1->state = PSINACTIVE;
                            }

                            break;

                        case MODE_G1_20G:
                        case MODE_G1_MASTER:
                            if(q->level > 999) {
                                q->level = 999;
                                q->grade = GRADE_GM;
                                if(q->playback)
                                    qrs_end_playback(g);
                                else if(q->recording)
                                    qrs_end_record(g);
                                q->p1->state = PSINACTIVE;
                            }

                            break;

                        default:
                            break;
                    }

                    play_sfx(newsection->data, newsection->volume);
                    if(q->section < 10) {
                        bgname = bformat("bg%d", q->section);
                        cs->bg = (asset_by_name(cs, (char *)(bgname->data)))->data;
                    }
                    /*if(q->mode_type == MODE_G2_DEATH && q->section < 10)
                        cs->anim_bg = cs->g2_bgs[q->section];*/
                }
            } else if(q->level == 999) {
                switch(q->mode_type) {
                    case MODE_G2_DEATH:
                    case MODE_G1_20G:
                    case MODE_G1_MASTER:
                        q->grade = GRADE_GM;
                        if(q->playback)
                            qrs_end_playback(g);
                        else if(q->recording)
                            qrs_end_record(g);
                        q->p1->state = PSINACTIVE;
                        break;

                    default:
                        break;
                }
            }
    	} else {
            q->combo = 1;
            q->combo_simple = 0;
        }
	}

    return 0;
}

int qrs_game_is_inactive(coreState *cs)
{
    qrsdata *q = cs->p1game->data;

    if(q->pracdata) {
        if(q->pracdata->paused == QRS_FIELD_EDIT)
            return 1;
    }

    return 0;
}

// TODO please fix this mess...
/*
need to move practool-related stuff to a separate game_t than the QRS game_t
so there aren't so many awkward overlapping functions

need to break this up into multiple functions which each update exactly one
thing

split up pieceseq parser into: parsing function, expansion function, and
get_usrseq_elem

get_elem should return failure if q->pracdata->usr_seq_expand is NULL and it
should not modify state
*/

int qs_update_pracdata(coreState *cs)
{
    if(!cs->p1game || !cs->menu)
        return 1;

    qrsdata *q = cs->p1game->data;
    struct pracdata *d = q->pracdata;
    menudata *md = cs->menu->data;
    bstring seq_bstr = NULL;
    char name_str[3] = {0, 0, 0};

    int piece_seq[3000];
    int num = 0;

    int i = 0;
    int j = 0;
    int k = 0;
    int t = 0;
    unsigned char c;

    int rpt_start = 0;
    int rpt_end = 0;
    int rpt = 0;
    int rpt_count = 0;
    int pre_rpt_count = 0;

    char rpt_count_strbuf[5];

    q->game_type = d->game_type;
    q->field_w = d->field_w;

    switch(q->game_type) {
        case 0:
            q->num_previews = 3;
            q->randomizer_type = RANDOMIZER_NORMAL;
            if(q->randomizer) randomizer_destroy(q->randomizer);
            q->randomizer = pento_randomizer_create(0);

            q->hold_enabled = 0;
            q->max_floorkicks = 3;
            q->lock_protect = 1;
            q->piecepool[QRS_I4]->flags &= ~PDNOWKICK;
            q->tetromino_only = 0;
            q->pentomino_only = 0;
            request_fps(cs, 60);
            break;
        case SIMULATE_G1:
            q->num_previews = 1;
            q->randomizer_type = RANDOMIZER_G1;
            if(q->randomizer) randomizer_destroy(q->randomizer);
            q->randomizer = g1_randomizer_create(0);

            q->hold_enabled = 0;
            q->max_floorkicks = 0;
            q->lock_protect = 0;
            q->piecepool[QRS_I4]->flags |= PDNOWKICK;
            q->tetromino_only = 1;
            q->pentomino_only = 0;
            request_fps(cs, 60);
            break;
        case SIMULATE_G2:
            q->num_previews = 1;
            q->randomizer_type = RANDOMIZER_G2;
            if(q->randomizer) randomizer_destroy(q->randomizer);
            q->randomizer = g2_randomizer_create(0);

            q->hold_enabled = 0;
            q->max_floorkicks = 0;
            q->lock_protect = 1;
            q->piecepool[QRS_I4]->flags |= PDNOWKICK;
            q->tetromino_only = 1;
            q->pentomino_only = 0;
            request_fps(cs, G2_FPS);
            break;
        case SIMULATE_G3:
            q->num_previews = 3;
            q->randomizer_type = RANDOMIZER_G3;
            if(q->randomizer) randomizer_destroy(q->randomizer);
            q->randomizer = g3_randomizer_create(0);

            q->hold_enabled = 1;
            q->max_floorkicks = 1;
            q->lock_protect = 1;
            q->piecepool[QRS_I4]->flags &= ~PDNOWKICK;
            q->tetromino_only = 1;
            q->pentomino_only = 0;
            request_fps(cs, 60);
            break;
        default:
            break;
    }

    q->hold = NULL;

    // and now for the hackiest check ever to see if we need to update the usr_seq

    if(md->numopts == MENU_PRACTICE_NUMOPTS && md->menu[md->numopts - 1]->type == MENU_TEXTINPUT) {
        seq_bstr = ((struct text_opt_data *)(md->menu[md->numopts - 1]->data))->text;
        for(i = 0; i < seq_bstr->slen; i++) {
            c = seq_bstr->data[i];
            if( (c < 'A' || c > 'Z') && !(c == '*' || c == '(' || c == ')') ) {
                if(rpt_count) {
                    k = 0;
                    while(k < 4 && i < seq_bstr->slen && seq_bstr->data[i] >= '0' && seq_bstr->data[i] <= '9') {
                        rpt_count_strbuf[k] = seq_bstr->data[i];
                        rpt_count_strbuf[k+1] = '\0';
                        i++;
                        k++;
                    }

                    i--;

                    num++;

                    if(k) {
                        piece_seq[num - 1] = (strtol(rpt_count_strbuf, NULL, 10) & 1023);
                    } else {
                        piece_seq[num - 1] = 1;
                    }

                    rpt_count = 0;
                    continue;
                } else
                    continue;
            }

            if(rpt_count) {
                num++;

                if(i < seq_bstr->slen - 1) {
                    if(c == 'I' && seq_bstr->data[i+1] == 'N' && seq_bstr->data[i+2] == 'F') {
                        piece_seq[num - 1] = SEQUENCE_REPEAT_INF;
                        goto end_sequence_proc;
                    } else {
                        piece_seq[num - 1] = 1;
                    }
                } else {
                    piece_seq[num - 1] = 1;
                }

                rpt_count = 0;
                continue;
            }

            if(c == '*') {
                if(rpt) {
                    if(!rpt_start) {
                        rpt_count = 1;
                        pre_rpt_count = 0;
                        rpt = 0;
                        if(!(piece_seq[num - 1] & SEQUENCE_REPEAT_END))
                            piece_seq[num - 1] |= SEQUENCE_REPEAT_END;
                    }

                    continue;
                } else {
                    if(num > 1) {
                        if(!(piece_seq[num - 2] & SEQUENCE_REPEAT_END)) {
                            rpt_count = 1;
                            pre_rpt_count = 0;
                            if(!(piece_seq[num - 1] & SEQUENCE_REPEAT_END)) {
                                piece_seq[num - 1] |= SEQUENCE_REPEAT_END;
                                piece_seq[num - 1] |= SEQUENCE_REPEAT_START;
                            }
                            continue;
                        } else
                            continue;
                    } else if(num) {
                        piece_seq[0] |= (SEQUENCE_REPEAT_START | SEQUENCE_REPEAT_END);
                        rpt_count = 1;
                        pre_rpt_count = 0;
                        continue;
                    } else
                        continue;
                }
            }

            if(c == '(') {
                if(rpt)
                    continue;

                rpt_start = 1;
                rpt = 1;
                continue;
            }

            if(c == ')') {
                if(!rpt)
                    continue;

                if(num > 0) {
                    piece_seq[num - 1] |= SEQUENCE_REPEAT_END;
                    pre_rpt_count = 1;
                }
                continue;
            }

            if(pre_rpt_count) {
                num++;
                piece_seq[num - 1] = 1;
                pre_rpt_count = 0;
                i--;
                continue;
            }

            name_str[0] = seq_bstr->data[i];

            if(seq_bstr->data[i + 1] == '4') {
                name_str[1] = '4';
                name_str[2] = '\0';

                for(j = 0; j < 25; j++) {
                    if(strcmp(name_str, get_qrspiece_name(j)) == 0) {
                        t = j;
                        if(!q->pentomino_only) {
                            goto found;
                        }
                    }
                }
            }

            name_str[1] = '\0';

            for(j = 0; j < 25; j++) {
                if(strcmp(name_str, get_qrspiece_name(j)) == 0) {
                    t = j;
                    if(q->tetromino_only) {
                        switch(t) {
                            case QRS_I:
                                t += 18;
                                break;
                            case QRS_T:
                                t += 10;
                                break;
                            case QRS_J:
                            case QRS_L:
                            case QRS_S:
                            case QRS_Z:
                                t += 19;
                                break;
                            default:
                                break;
                        }

                        if(t >= 18)
                            goto found;
                    } else if(q->pentomino_only) {
                        switch(t) {
                            case QRS_I4:
                                t -= 18;
                                break;
                            case QRS_T4:
                                t -= 10;
                                break;
                            case QRS_J4:
                            case QRS_L4:
                            case QRS_S4:
                            case QRS_Z4:
                                t -= 19;
                                break;
                            default:
                                break;
                        }

                        if(t < 18)
                            goto found;
                    } else {
                        goto found;
                    }
                }
            }

            if(seq_bstr->data[i + 1] == 'a') {
                name_str[1] = 'a';
                name_str[2] = '\0';

                for(j = 0; j < 25; j++) {
                    if(strcmp(name_str, get_qrspiece_name(j)) == 0) {
                        t = j;
                        if(q->tetromino_only) {
                            switch(t) {
                                case QRS_I:
                                    t += 18;
                                    break;
                                case QRS_T:
                                    t += 10;
                                    break;
                                case QRS_J:
                                case QRS_L:
                                case QRS_S:
                                case QRS_Z:
                                    t += 19;
                                    break;
                                default:
                                    break;
                            }

                            if(t >= 18)
                                goto found;
                        } else if(q->pentomino_only) {
                            switch(t) {
                                case QRS_I4:
                                    t -= 18;
                                    break;
                                case QRS_T4:
                                    t -= 10;
                                    break;
                                case QRS_J4:
                                case QRS_L4:
                                case QRS_S4:
                                case QRS_Z4:
                                    t -= 19;
                                    break;
                                default:
                                    break;
                            }

                            if(t < 18)
                                goto found;
                        } else {
                            goto found;
                        }
                    }
                }
            } else if(seq_bstr->data[i + 1] == 'b') {
                name_str[1] = 'b';
                name_str[2] = '\0';

                for(j = 0; j < 25; j++) {
                    if(strcmp(name_str, get_qrspiece_name(j)) == 0) {
                        t = j;
                        if(q->tetromino_only) {
                            switch(t) {
                                case QRS_I:
                                    t += 18;
                                    break;
                                case QRS_T:
                                    t += 10;
                                    break;
                                case QRS_J:
                                case QRS_L:
                                case QRS_S:
                                case QRS_Z:
                                    t += 19;
                                    break;
                                default:
                                    break;
                            }

                            if(t >= 18)
                                goto found;
                        } else if(q->pentomino_only) {
                            switch(t) {
                                case QRS_I4:
                                    t -= 18;
                                    break;
                                case QRS_T4:
                                    t -= 10;
                                    break;
                                case QRS_J4:
                                case QRS_L4:
                                case QRS_S4:
                                case QRS_Z4:
                                    t -= 19;
                                    break;
                                default:
                                    break;
                            }

                            if(t < 18)
                                goto found;
                        } else {
                            goto found;
                        }
                    }
                }
            }

            continue;
found:
            num++;
            piece_seq[num - 1] = t;

            if(rpt_start) {
                piece_seq[num - 1] |= SEQUENCE_REPEAT_START;
                rpt_start = 0;
            } else if(rpt_end) {
                piece_seq[num - 1] |= SEQUENCE_REPEAT_END;
                rpt_end = 0;
            }
        }
    }

    if(rpt_count) {
        num++;
        piece_seq[num - 1] = 1;
    }

end_sequence_proc:
    for(i = 0; i < num; i++)
        d->usr_sequence[i] = piece_seq[i];

    d->usr_seq_len = num;

    d->usr_seq_expand_len = 0;
/*
    switch(d->piece_subset) {
        case SUBSET_ALL:
            q->tetromino_only = 0;
            q->pentomino_only = 0;
            break;
        case SUBSET_TETS:
            q->tetromino_only = 1;
            q->pentomino_only = 0;
            break;
        case SUBSET_PENTS:
            q->tetromino_only = 0;
            q->pentomino_only = 1;
            break;
        default:
            break;
    }
*/
    qrsfield_set_w(cs->p1game->field, q->field_w);
    qrsfield_set_w(q->pracdata->usr_field, q->field_w);

    for(i = 0; i < d->usr_field_undo_len; i++)
        qrsfield_set_w(q->pracdata->usr_field_undo[i], q->field_w);

    for(i = 0; i < d->usr_field_redo_len; i++)
        qrsfield_set_w(q->pracdata->usr_field_redo[i], q->field_w);

    d->field_selection = 0;

    // process randomizer seed entry...

    // q->previews are expected to be destroyed and re-allocated as needed

    if(q->previews[0])
        piecedef_destroy(q->previews[0]);
    if(q->previews[1])
        piecedef_destroy(q->previews[1]);
    if(q->previews[2])
        piecedef_destroy(q->previews[2]);

    q->previews[0] = NULL;
    q->previews[1] = NULL;
    q->previews[2] = NULL;

    if(q->pracdata->usr_seq_len) {
        q->previews[0] = qrspiece_cpy(q->piecepool, qs_get_usrseq_elem(d, 0));
        q->previews[1] = qrspiece_cpy(q->piecepool, qs_get_usrseq_elem(d, 1));
        q->previews[2] = qrspiece_cpy(q->piecepool, qs_get_usrseq_elem(d, 2));
    } else {
        q->previews[0] = qrspiece_cpy(q->piecepool, q->randomizer->lookahead(q->randomizer, 1));
        q->previews[1] = qrspiece_cpy(q->piecepool, q->randomizer->lookahead(q->randomizer, 2));
        q->previews[2] = qrspiece_cpy(q->piecepool, q->randomizer->lookahead(q->randomizer, 3));
    }

    if(d->brackets)
        q->state_flags |= GAMESTATE_BRACKETS;
    else
        q->state_flags &= ~GAMESTATE_BRACKETS;

    if(d->invisible)
        q->state_flags |= GAMESTATE_INVISIBLE;
    else
        q->state_flags &= ~GAMESTATE_INVISIBLE;

    if(q->state_flags & GAMESTATE_BRACKETS) {
        if(q->previews[0])
            q->previews[0]->flags |= PDBRACKETS;

        if(q->previews[1])
            q->previews[1]->flags |= PDBRACKETS;

        if(q->previews[2])
            q->previews[2]->flags |= PDBRACKETS;
    } else {
        if(q->previews[0])
            q->previews[0]->flags &= ~PDBRACKETS;

        if(q->previews[1])
            q->previews[1]->flags &= ~PDBRACKETS;

        if(q->previews[2])
            q->previews[2]->flags &= ~PDBRACKETS;
    }

    return 0;
}

// TODO: clean this function up, especially the parser + expander, and use more established terminology
int qs_get_usrseq_elem(struct pracdata *d, int index)
{
    int *seq = d->usr_sequence;
    int expand[4000];
    int expand_count = 0;

    int rpt_start = 0;
    int rpt_end = 0;
    int rpt_len = 0;
    int rpt_count = 0;
    int rpt = 0;
    int inf_rpt_len = 0;
    int inf_start = 0;

    int inf = 0;

    int i = 0;
    int j = 0;
    int k = 0;
    int val = 0;
    int complex = 0;

    if(d->usr_seq_expand_len) {
        for(i = 0; i < d->usr_seq_expand_len; i++) {
            if(d->usr_seq_expand[i] & SEQUENCE_REPEAT_INF) {
                inf = 1;
                rpt_start = i;
                inf_rpt_len = d->usr_seq_expand_len - rpt_start;
            }
        }

        if(inf) {
            if(index < rpt_start) {
                val = d->usr_seq_expand[index] & 0b11111;
            } else {
                if(inf_rpt_len > 1)
                    index = (index - rpt_start) % inf_rpt_len;
                else
                    index = 0;

                val = d->usr_seq_expand[rpt_start + index] & 0b11111;
            }
        } else {
            if(index > d->usr_seq_expand_len - 1)
                val = -1;
            else
                val = d->usr_seq_expand[index] & 0b11111;
        }

        return val;
    }

    if(!d->usr_seq_len) {
        d->usr_seq_expand_len = 0;
        return -1;
    }

    for(i = 0; i < index && i < d->usr_seq_len; i++) {
        if(!IS_QRS_PIECE(seq[i]))
            complex = 1;
    }

    if(!complex && index < d->usr_seq_len) {
        if(IS_QRS_PIECE(seq[index]))
            return seq[index];
    }

    for(i = 0;; i++) {
        if(i >= d->usr_seq_len || expand_count > 3500) {
            // 3500 is an arbitrary limit to prevent taking up too much memory for little benefit
            break;
        }

        val = seq[i];
        if(rpt) {
            /* rpt = 1 implies we encountered the beginning of a grouped subsequence, where groups are assumed
            to have a rep count specified at the end */
            if(val & SEQUENCE_REPEAT_END || i == d->usr_seq_len - 1) {
                rpt = 0;
                rpt_end = i;
                rpt_len = rpt_end - rpt_start + 1; // length of grouped subsequence
                i++; // next element of the sequence is either a repetition count or beyond the end of the sequence

                if(i != d->usr_seq_len && seq[i] == SEQUENCE_REPEAT_INF) {
                    // bound check must come first to prevent seq[i] from creating UB
                    inf = 1;
                    inf_rpt_len = rpt_len;
                    inf_start = rpt_start;
                    expand_count += rpt_len;
                    for(k = 0; k < rpt_len; k++)
                    {
                        /* seq[blah] & 0b11111 zeroes other flags on the piece;
                        first repeated element gets repeat_inf flag;
                        expanded sequence is cut off after the infinitely repeating group and this expansion loop ends */

                        expand[expand_count - rpt_len + k] = (seq[rpt_start + k] & 0b11111) | (!k ? SEQUENCE_REPEAT_INF : 0);
                    }

                    break;
                } else {
                    // dealing with finite repetition or open-ended grouped subsequence

                    if(i == d->usr_seq_len) rpt_count = 1;
                    // ^^ force open-ended group to 1 repetition (same as if there was no grouping)

                    else {
                        if(seq[i] > USRSEQ_RPTCOUNT_MAX)   // last-minute bounding of repetition count to something reasonable
                            rpt_count = USRSEQ_RPTCOUNT_MAX;
                        else
                            rpt_count = seq[i];
                    }

                    for(k = 0; k < rpt_count; k++) {    // expand entire repetition to sequence of pieces with no special flags
                        for(j = 0; j < rpt_len; j++) {
                            expand[expand_count + k*rpt_len + j] = seq[rpt_start + j] & 0b11111;
                        }
                    }

                    expand_count += rpt_count*rpt_len;
                    rpt = 0;    // we are done with this grouped subsequence
                }
            }
        } else {
            if(val & SEQUENCE_REPEAT_START) {
                rpt_start = i;
                rpt = 1;
                if(val & SEQUENCE_REPEAT_END || i == d->usr_seq_len - 1) {
                    i--;        // hacky way for the loop to go into the if(rpt) branch and handle these edge cases
                    continue;   // the i-- ensures the loop reads the additional flags on this element/doesn't go OOB
                }
            } else {    // no expansion to be done
                expand_count++;
                expand[expand_count - 1] = seq[i] & 0b11111;
            }
        }
    }

    // end of expansion loop

    if(inf) {
        if(index < inf_start) { // index is of the piece we want to return to the function caller
            val = expand[index] & 0b11111;
        } else {
            if(inf_rpt_len > 1) {
                // collapse an arbitrarily large index value to a relative one that fits in the array bounds
                index = (index - inf_start) % inf_rpt_len;
            } else
                index = 0;

            val = expand[inf_start + index] & 0b11111;
        }
    } else {
        if(index < expand_count)
            val = expand[index] & 0b11111;
        else
            val = USRSEQ_ELEM_OOB;
    }

    // printf("Expanded sequence:");
    // for(i = 0; i < expand_count; i++) { printf(" %d", expand[i]); }
    // printf("\n");



    // updating the pracdata's expanded sequence field so we don't have to waste time expanding it later unless it changes
    for(i = 0; i < expand_count; i++)
        d->usr_seq_expand[i] = expand[i];

    d->usr_seq_expand_len = expand_count;


    return val;
}

// return value: 0 success, -1 invalid argument(s), 1 no next piece to deal to the player
int qs_initnext(game_t *g, qrs_player *p, unsigned int flags)
{
    if(!g || !p)
		return -1;

	coreState *cs = g->origin;
	qrsdata *q = (qrsdata *)(g->data);
    struct randomizer *qrand = q->randomizer;

	int i = 0;
    piece_id t = 0;

	struct asset *a = NULL;

    if(flags & INITNEXT_DURING_ACTIVE_PLAY)
    {
        // special check so we don't tell the game to terminate, even if there is no new piece to deal out
        // we don't want the game to terminate while the player is controlling a piece
        // edge case where this is relevant: player uses hold with no held piece, at last piece in user seq

        if(q->pracdata && q->pracdata->usr_seq_len)
        {
            if(qs_get_usrseq_elem(q->pracdata, q->pracdata->hist_index + 1) == USRSEQ_ELEM_OOB)
            {
                return 1;
            }
        }
    }

    q->p1counters->floorkicks = 0;

    if(q->pracdata && q->pracdata->usr_seq_len)
        q->pracdata->hist_index++;

    else {
        t = qrand->pull(qrand);
        if(q->randomizer->num_pieces == 7)
            t = ars_to_qrs_id(t);
    }

    if(p->def) piecedef_destroy(p->def);
    p->def = q->previews[0];

    if(p->def)
        q->cur_piece_qrs_id = p->def->qrs_id;
    else
        q->cur_piece_qrs_id = PIECE_ID_INVALID;

    q->previews[0] = q->previews[1];
    q->previews[1] = q->previews[2];
    q->previews[2] = qrspiece_cpy(q->piecepool, t);
    //printf("New piecedef to deal out: %lx with ID %d\n", q->previews[2], t);

    if(q->state_flags & GAMESTATE_BRACKETS) {
        if(q->previews[2]) q->previews[2]->flags |= PDBRACKETS;
    }

    t = qrand->lookahead(qrand, 1);
    if(qrand->num_pieces == 7)
        t = ars_to_qrs_id(t);

    if(t != PIECE_ID_INVALID) {
        switch( t >= 18 ? (t - 18) : (t % 7) ) {
    		case 0:
    			a = asset_by_name(cs, "piece0");
    			break;

    		case 1:
    			a = asset_by_name(cs, "piece1");
    			break;

    		case 2:
    			a = asset_by_name(cs, "piece2");
    			break;

    		case 3:
    			a = asset_by_name(cs, "piece3");
    			break;

    		case 4:
    			a = asset_by_name(cs, "piece4");
    			break;

    		case 5:
    			a = asset_by_name(cs, "piece5");
    			break;

    		case 6:
    			a = asset_by_name(cs, "piece6");
    			break;

    		default:
    			break;
    	}

        play_sfx(a->data, a->volume);
    }

    if(q->cur_piece_qrs_id == USRSEQ_ELEM_OOB || !p->def) {
        // pause the game if we can't deal a new piece
        return QSGAME_SHOULD_TERMINATE;
    }

    if(q->cur_piece_qrs_id >= 18)
    	p->y = ROWTOY(SPAWNY_QRS + 2);
    else
        p->y = ROWTOY(SPAWNY_QRS);

    p->x = SPAWNX_QRS;
	p->orient = FLAT;
	p->state = PSFALL|PSSPAWN;

	q->p1counters->lock = 0;

    return 0;
}

/*
int qs_init_randomize(game_t *g)
{
    qrsdata *q = g->data;
    int *history = q->history;
    long double modified_weights[25];
    int t = 0;
    int i = 0;

    if(q->randomizer_type == RANDOMIZER_NORMAL && !q->pentomino_only) {
        t = rand() % 8;

        switch(t) {
            case 0:
                history[7] = QRS_I;
                break;
            case 1:
                history[7] = QRS_J;
                break;
            case 2:
                history[7] = QRS_L;
                break;
            case 3:
                history[7] = QRS_Ya;
                break;
            case 4:
                history[7] = QRS_Yb;
                break;
            case 5:
                history[7] = QRS_I4;
                break;
            case 6:
                history[7] = QRS_J4;
                break;
            case 7:
                history[7] = QRS_L4;
                break;
            default:
                history[7] = QRS_I;
                break;
        }

        history[8] = QRS_S4;
        history[9] = QRS_Z4;

        history[8] = qs_normal_randomize(history, piece_weights);
        history[9] = qs_normal_randomize(history, piece_weights);
    } else if(q->randomizer_type == RANDOMIZER_NIGHTMARE) {
        t = rand() % 9;
        switch(t) {
            case 0:
            case 1:
            case 2:
                t = QRS_X;
                break;
            case 3:
            case 4:
                t = QRS_Fa;
                break;
            case 5:
            case 6:
                t = QRS_Fb;
                break;
            case 7:
                t = QRS_S;
                break;
            case 8:
                t = QRS_Z;
                break;
            default:
                t = QRS_V;
                break;
        }

        for(i = 0; i < 10; i++) {
            history[i] = t;
        }
    } else if(q->pentomino_only) {
        q->history[0] = QRS_Fb;
    	q->history[1] = QRS_Fa;
    	q->history[2] = QRS_X;
    	q->history[3] = QRS_W;
        q->history[4] = QRS_S;
        q->history[5] = QRS_Z;
        q->cur_piece_qrs_id = QRS_X;

        t = rand() % 5;

        switch(t) {
            case 0:
                history[7] = QRS_I;
                break;
            case 1:
                history[7] = QRS_J;
                break;
            case 2:
                history[7] = QRS_L;
                break;
            case 3:
                history[7] = QRS_Ya;
                break;
            case 4:
                history[7] = QRS_Yb;
                break;
            default:
                history[7] = QRS_I;
                break;
        }

        for(i = 0; i < 18; i++)
            modified_weights[i] = piece_weights[i];

        for(i = 18; i < 25; i++)
            modified_weights[i] = 0;

        history[8] = QRS_Fa;
        history[9] = QRS_Fb;

        history[8] = qs_normal_randomize(history, modified_weights);
        history[9] = qs_normal_randomize(history, modified_weights);
    } else if(q->randomizer_type == RANDOMIZER_G2) {
        t = g2_get_init_piece();

        q->cur_piece_qrs_id = QRS_S4;
        q->history[7] = QRS_S4;
        q->history[8] = QRS_Z4;
        q->history[9] = t;  // first piece

        t = qs_g2_randomize(history);

        q->cur_piece_qrs_id = q->history[7];
        q->history[7] = q->history[8];
        q->history[8] = q->history[9];  // first
        q->history[9] = t;  // second

        t = qs_g2_randomize(history);

        q->cur_piece_qrs_id = q->history[7];  // QRS_Z4 - "dummy" piece, game immediately switches to next when game starts
        q->history[7] = q->history[8];  // first
        q->history[8] = q->history[9];  // second
        q->history[9] = t;  // third
    } else if(q->randomizer_type == RANDOMIZER_G1) {
        t = g2_get_init_piece();

        q->cur_piece_qrs_id = QRS_Z4;
        q->history[7] = QRS_Z4;
        q->history[8] = QRS_Z4;
        q->history[9] = t;  // first piece

        t = qs_g1_randomize(history);

        q->cur_piece_qrs_id = q->history[7];
        q->history[7] = q->history[8];
        q->history[8] = q->history[9];  // first
        q->history[9] = t;  // second

        t = qs_g1_randomize(history);

        q->cur_piece_qrs_id = q->history[7];  // QRS_Z4 - "dummy" piece, game immediately switches to next when game starts
        q->history[7] = q->history[8];  // first
        q->history[8] = q->history[9];  // second
        q->history[9] = t;  // third
    }

    return 0;
}

int qs_randomize(game_t *g)
{
    qrsdata *q = g->data;
    long double modified_weights[25];
    int i = 0;

    if(q->randomizer_type == RANDOMIZER_NORMAL && !q->pentomino_only) {
        return qs_normal_randomize(q->history, piece_weights);
    } else if(q->randomizer_type == RANDOMIZER_NIGHTMARE) {
        for(i = 0; i < 25; i++) {
            modified_weights[i] = 0.01L;
        }

        modified_weights[QRS_X] = 6.0L;
        modified_weights[QRS_S] = 1.3L;
        modified_weights[QRS_Z] = 1.3L;
        modified_weights[QRS_U] = 1.0L;
        modified_weights[QRS_Fa] = 1.5L;
        modified_weights[QRS_Fb] = 1.5L;
        modified_weights[QRS_W] = 1.0L;
        modified_weights[QRS_V] = 0.7L;

        return (qs_normal_randomize(q->history, modified_weights));
    } else if(q->pentomino_only) {
        for(i = 0; i < 18; i++)
            modified_weights[i] = piece_weights[i];
        for(i = 18; i < 25; i++)
            modified_weights[i] = 0;

        return (qs_normal_randomize(q->history, modified_weights));
    } else if(q->randomizer_type == RANDOMIZER_G2) {
        return qs_g2_randomize(q->history);
    } else if(q->randomizer_type == RANDOMIZER_G1) {
        return qs_g1_randomize(q->history);
    }

    return QRS_I;
}

int qs_normal_randomize(int *history, long double *weights)  // RAND_MAX == 2147483647
{
    if(!history)
		return -1;
    if(!weights)
        weights = piece_weights;

	int i = 0;
	int j = 0;
	//int k = 0;
	long double p = (long double)(rand()) / (long double)(RAND_MAX);
    long double sum = 0;
    int pieces[25];
    long double weights_[25];

    for(i = 0; i < 25; i++) {
        weights_[i] = weights[i];
        pieces[i] = 1;
        for(j = 0; j < 10; j++) {
            if(history[j] == i)
                pieces[i]++;
        }

        weights_[i] = weights_[i] / (long double)(pieces[i] * pieces[i] * (i < 18 ? pieces[i] : 1));
        sum += weights_[i];
    }

    p = p * sum;
    sum = 0;

    for(i = 0; i < 25; i++) {
        if(p >= sum && p < (sum + weights_[i]))
            return i;
        else
            sum += weights_[i];
    }

    return QRS_T4;

	for(i = 0; i < 8; i++) {
		t = rand() % 18;

		for(j = 0; j < 4; j++) {
			if(t != history[j])
					k++;
		}

		if(k == 4)
			return t;

		k = 0;
	}

	t = rand() % 18;
	return t;
}

int qs_g2_randomize(int *history)
{
    int t = 0;
    int i = 0;
    int j = 0;

    int is_in_history = 0;

    for(i = 0; i < 5; i++) {
        t = g2_read_rand() % 7;
        switch(t) {
            case 1:     // Z
                t = 6;
                break;
            case 2:     // S
                t = 5;
                break;
            case 3:     // J
                t = 2;
                break;
            case 4:     // L
                t = 3;
                break;
            case 5:     // O
                t = 4;
                break;
            case 6:     // T
                t = 1;
                break;
            default:
                break;
        }
        for(j = 6; j < 10; j++) {
            if(history[j] == t + 18)
                is_in_history = 1;
        }

        if(!is_in_history)
            return t + 18;

        is_in_history = 0;
        t = g2_read_rand() % 7;
    }

    switch(t) {
        case 1:
            t = 6;
            break;
        case 2:
            t = 5;
            break;
        case 3:
            t = 2;
            break;
        case 4:
            t = 3;
            break;
        case 5:
            t = 4;
            break;
        case 6:
            t = 1;
            break;
        default:
            break;
    }

    return t + 18;
}

int qs_g1_randomize(int *history)
{
    int t = 0;
    int i = 0;
    int j = 0;

    int is_in_history = 0;

    for(i = 0; i < 3; i++) {
        t = g2_read_rand() % 7;
        switch(t) {
            case 1:     // Z
                t = 6;
                break;
            case 2:     // S
                t = 5;
                break;
            case 3:     // J
                t = 2;
                break;
            case 4:     // L
                t = 3;
                break;
            case 5:     // O
                t = 4;
                break;
            case 6:     // T
                t = 1;
                break;
            default:
                break;
        }
        for(j = 6; j < 10; j++) {
            if(history[j] == t + 18)
                is_in_history = 1;
        }

        if(!is_in_history)
            return t + 18;

        is_in_history = 0;
        t = g2_read_rand() % 7;
    }

    switch(t) {
        case 1:
            t = 6;
            break;
        case 2:
            t = 5;
            break;
        case 3:
            t = 2;
            break;
        case 4:
            t = 3;
            break;
        case 5:
            t = 4;
            break;
        case 6:
            t = 1;
            break;
        default:
            break;
    }

    return t + 18;
}
*/