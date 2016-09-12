#ifndef _PTK_DATA
#define _PTK_DATA

#ifdef __cplusplus
extern "C" {
#endif

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#ifdef __cplusplus
}
#endif

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

    lua_State *L;
} ptk_data;
#endif
