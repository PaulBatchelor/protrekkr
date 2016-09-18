#ifndef _PTK_DATA
#define _PTK_DATA

#ifdef __cplusplus
extern "C" {
#endif

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <soundpipe.h>
#include <sporth.h>

typedef struct {
    sp_data *sp;
    plumber_data pd;
    char use_sporth;
	sporth_listener sl;
    float tick; 
	float linepos;
    float play;
	sp_ftbl *notes;
	sp_ftbl *gates;
    sp_ftbl *tracks;
    sp_progress *prog;
} ptk_sporth;

typedef struct {
    int CONSOLE_WIDTH;
    int CONSOLE_HEIGHT;
    int CONSOLE_HEIGHT2;
    int CHANNELS_WIDTH;
    int TRACKS_WIDTH;

    /* do we need these? */
    int c_l_tvol;
    int c_r_tvol;
    int c_l_cvol;
    int c_r_cvol;

    int Display_Pointer;

    int Scopish;
    char Scopish_LeftRight;
    int fluzy;
    int rs_coef;
    int gui_lx;
    int gui_ly;
    int gui_lxs;
    int gui_lys;
    char trkchan;
    int pos_space;
    int multifactor;
    char seditor;
    char tipoftheday[256];
    int ctipoftheday;
    char sas;
    int ltretvalue;

    int retletter[256];
    int last_index;

    int gui_action_metronome;
    int gui_action;

    int gui_pushed;
    int Done_Tip;

    int player_pos;

    char teac;
    
    int Column_Under_Caret;

    char Current_Instrument_Split;
    int liveparam;
    int livevalue;

    int Track_Under_Caret;
    int gui_track;

    char userscreen;
    /* MAX_PATH is 260 */
    char name[260];
    char synthname[260];
    char instrname[260];
    char name303[260];
    char namerev[260];
    char namemidicfg[260];
    char artist[20];
    char style[20];
    char Prog_Path[260];

    int Current_Instrument;

    int restx;
    int resty;
    int fsize;

    char draw_sampled_wave;
    char draw_sampled_wave2;
    char draw_sampled_wave3;

    int redraw_everything;

    char boing;
    char po_ctrl2;
    char po_alt; 
    char po_alt2; 
    int sp_TickCounter;
    int LVColor;
    int RVColor;
    int key_record_first_time;
    int old_key_Pattern_Line;
    int Record_Keys[37];
    int Record_Keys_State[37];

    int snamesel;
    int player_line;

    char actuloop;

    int namesize;

    int MouseWheel_Multiplier;

    char Jazz_Edit;

    char Rows_Decimal;
    char See_Prev_Next_Pattern;
    int wait_AutoSave;
    int gui_thread_action;
    int gui_bpm_action;

    /* MAX_TRACKS is 16 */
    int CHAN_MIDI_PRG[16];

    float left_float;
    float right_float;
    float left_float_render;
    float right_float_render;
	char start_gui;
	char render_mode;

    int ntabs;
    char cur_input_name[1024];

    int Ticks_Synchro_Left;
    int Ticks_Synchro_Right;
    unsigned char sl3;

    int metronome_rows_counter;
    int metronome_magnify;

    int pos_scope;
    int pos_scope_latency;

    int lref;
    lua_State *L;
    ptk_sporth sporth;
} ptk_data;

#ifdef __cplusplus
}
#endif

#endif
