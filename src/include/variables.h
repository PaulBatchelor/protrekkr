// ------------------------------------------------------
// Protrekkr
// Based on Juan Antonio Arguelles Rius's NoiseTrekker.
//
// Copyright (C) 2008-2011 Franck Charlet.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//  1. Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL FRANCK CHARLET OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.
// ------------------------------------------------------

#ifndef _VARIABLES_H_
#define _VARIABLES_H_

#undef __STAND_ALONE__

#include <SDL/SDL.h>

#define MAXLOOPTYPE 1
#define FULLYESNO 0

#if !defined(__WIN32__) && !defined(__AROS__)
#define FALSE 0
#define TRUE 1
#endif

#define GUI_CMD_NONE 0

#define GUI_CMD_SET_FILES_LIST_SLIDER 1
#define GUI_CMD_SET_FILES_LIST_SELECT_FILE 2

#define GUI_CMD_PREVIOUS_POSITION 3
#define GUI_CMD_NEXT_POSITION 4
#define GUI_CMD_SET_PATTERN_LENGTH 5
#define GUI_CMD_PREV_PATT 7
#define GUI_CMD_NEXT_PATT 8
#define GUI_CMD_REDUCE_SONG_LENGTH 9
#define GUI_CMD_INCREASE_SONG_LENGTH 10
#define GUI_CMD_UPDATE_SEQUENCER 11
#define GUI_CMD_SET_FOCUS_TRACK 15
#define GUI_CMD_PREV_INSTR 25
#define GUI_CMD_NEXT_INSTR 26
#define GUI_CMD_SWITCH_TRACK_MUTE_STATE 27
#define GUI_CMD_PLAY_SONG 28
#define GUI_CMD_STOP_SONG 29
#define GUI_CMD_RECORD_303 30
#define GUI_CMD_CHANGE_BPM_TICKS_NBR 40
#define GUI_CMD_CHANGE_TRACKS_NBR 41
#define GUI_CMD_SET_TRACK_CUTOFF_FREQ 50
#define GUI_CMD_SET_TRACK_RESONANCE 51
#define GUI_CMD_SET_TRACK_INERTIA 52
#define GUI_CMD_UPDATE_TRACK_ED 53
#define GUI_CMD_SET_TRACK_THRESHOLD 54
#define GUI_CMD_SET_TRACK_CLAMP 55
#define GUI_CMD_SET_TRACK_REVERB_SEND 56
#define GUI_CMD_SET_TRACK_PANNING 57
#define GUI_CMD_CALC_LENGTH 58

#define GUI_CMD_SELECT_REVERB_EDIT 59
#define GUI_CMD_SELECT_DISKIO_EDIT 60
#define GUI_CMD_SELECT_TRACK_EDIT 61
#define GUI_CMD_SELECT_INSTRUMENT_EDIT 62
#define GUI_CMD_SELECT_FX_EDIT 63
#define GUI_CMD_SELECT_SEQUENCER 64
#define GUI_CMD_SELECT_SCREEN_SETUP_EDIT 65
#define GUI_CMD_SELECT_SYNTH_EDIT 66
#define GUI_CMD_SELECT_TRACK_FX_EDIT 67
#define GUI_CMD_SELECT_SAMPLE_EDIT 68
#define GUI_CMD_SMALL_EDIT 69
#define GUI_CMD_SAVE_INSTRUMENT 70
#define GUI_CMD_MODULE_INFOS 71
#define GUI_CMD_SELECT_TB303_EDIT 72
#define GUI_CMD_REFRESH_TB303_PARAMS 73

#define GUI_CMD_SELECT_LARGE_PATTERNS 74

#define GUI_CMD_SWITCH_TRACK_LARGE_STATE 75
#define GUI_CMD_SWITCH_TRACK_SMALL_STATE 76

#define GUI_CMD_EXPORT_WAV 77

#define GUI_CMD_SET_FILES_LIST_PLAY_WAV 78

#define GUI_CMD_REDUCE_TRACK_NOTES 79
#define GUI_CMD_EXPAND_TRACK_NOTES 80

#define GUI_CMD_SELECT_MIDI_SETUP 81

#define GUI_CMD_REDUCE_TRACK_EFFECTS 82
#define GUI_CMD_EXPAND_TRACK_EFFECTS 83

#define GUI_CMD_FILELIST_SCROLL 100
#define GUI_CMD_UPDATE_LOOP_EDITOR_ED 101
#define GUI_CMD_SAVE_MODULE 102

#define GUI_CMD_INSTR_SYNTH_SCROLL 103

#define GUI_CMD_SET_INSTRUMENT_AMPLI 104
#define GUI_CMD_SET_INSTRUMENT_FINETUNE 105
#define GUI_CMD_SET_INSTRUMENT_DECAY 106

#define GUI_CMD_UPDATE_INSTRUMENT_ED 107
#define GUI_CMD_UPDATE_DISKIO_ED 108

#define GUI_CMD_NEW_MODULE 109
#define GUI_CMD_UPDATE_SEQ_ED 110
#define GUI_CMD_UPDATE_FX_ED 112
#define GUI_CMD_CHANGE_SHUFFLE 113
#define GUI_CMD_UPDATE_SETUP_ED 114

#define GUI_CMD_REDUCE_PATTERNS_10 115
#define GUI_CMD_INCREASE_PATTERNS_10 116

#define GUI_CMD_SAVE_FINAL 117
#define GUI_CMD_CALC_FINAL 118
#define GUI_CMD_SAVE_303_PATTERN 119

#define GUI_CMD_SET_INSTR_SYNTH_LIST_SLIDER 120
#define GUI_CMD_SET_INSTR_SYNTH_LIST_SELECT 121

#define GUI_CMD_UPDATE_REVERB_ED 122

#define GUI_CMD_REFRESH_TB303_PARAMS_EXTERNAL 123

#define GUI_CMD_UPDATE_PATTERN 124

#define GUI_CMD_PREVIOUS_16_INSTR 125
#define GUI_CMD_NEXT_16_INSTR 126

#define GUI_CMD_SAVE_REVERB 127
#define GUI_CMD_SAVE_PATTERN 128

#define GUI_CMD_REFRESH_PALETTE 129

#define GUI_CMD_SET_FOCUS_TRACK_EXTERNAL 130

#define GUI_CMD_SELECT_INSTR_EXTERNAL 131

#define GUI_CMD_SWITCH_TRACK_STATUS 132

#define GUI_CMD_GOTO_PREVIOUS_ROW 133
#define GUI_CMD_GOTO_NEXT_ROW 134

#define GUI_CMD_UPDATE_MIDI_ED 135

#define GUI_CMD_SAVE_MIDICFG 136

#define GUI_CMD_FLASH_METRONOME_ON 137
#define GUI_CMD_FLASH_METRONOME_OFF 138

#define GUI_CMD_UPDATE_SYNTH_ED 150

#define GUI_CMD_MIDI_NOTE_OFF_1_TRACK 151
#define GUI_CMD_MIDI_NOTE_OFF_ALL_TRACKS 152

#define GUI_CMD_UPDATE_TRACK_FX_ED 154
#define GUI_CMD_UPDATE_MIDI_303_ED 155

#define GUI_CMD_INSERT_TRACK 156
#define GUI_CMD_DELETE_TRACK 157

#define GUI_CMD_RENDER_WAV 169

#define GUI_CMD_REDUCE_POSITIONS_10 170
#define GUI_CMD_INCREASE_POSITIONS_10 171

#define GUI_CMD_INCREASE_STEP_ADD 172
#define GUI_CMD_DECREASE_STEP_ADD 173

#define GUI_CMD_UPDATE_PATTERN_ED 174

#define GUI_CMD_HIGHER_OCTAVE 175
#define GUI_CMD_LOWER_OCTAVE 176

#define GUI_CMD_TIMED_REFRESH_SEQUENCER 177

#define GUI_CMD_DELETE_INSTRUMENT 201
#define GUI_CMD_SAVE_SYNTH 202
#define GUI_CMD_PATTERNS_POOL_EXHAUSTED 204
#define GUI_CMD_TURN_SCOPES_ON 205
#define GUI_CMD_RAND_SYNTH 206
#define GUI_CMD_REFRESH_SAMPLE_ED 210
#define GUI_CMD_RECORD_MODE 251
#define GUI_CMD_EDIT_MODE 252
#define GUI_CMD_ASK_EXIT 253
#define GUI_CMD_EXIT 254

#define GUI_CMD_NOP 255

#define USER_SCREEN_DISKIO_EDIT 0
#define USER_SCREEN_TRACK_EDIT 1
#define USER_SCREEN_INSTRUMENT_EDIT 2
#define USER_SCREEN_FX_SETUP_EDIT 3
#define USER_SCREEN_SEQUENCER 4
#define USER_SCREEN_SETUP_EDIT 5
#define USER_SCREEN_SYNTH_EDIT 6
#define USER_SCREEN_TRACK_FX_EDIT 7
#define USER_SCREEN_SAMPLE_EDIT 8
#define USER_SCREEN_TB303_EDIT 9
#define USER_SCREEN_REVERB_EDIT 10
#define USER_SCREEN_LARGE_PATTERN 11
#define USER_SCREEN_SETUP_MIDI 12

#define INPUT_NONE 0
#define INPUT_MODULE_NAME 1
#define INPUT_INSTRUMENT_NAME 2
#define INPUT_SYNTH_NAME 3
#define INPUT_MODULE_ARTIST 4
#define INPUT_MODULE_STYLE 5
#define INPUT_303_PATTERN 6
#define INPUT_REVERB_NAME 7
#define INPUT_SELECTION_NAME 8
#define INPUT_MIDI_NAME 9

#define MARKER_START_SET 1
#define MARKER_END_SET 2

#define LIVE_PARAM_SONG_VOLUME 1
#define LIVE_PARAM_SONG_BPM 2
#define LIVE_PARAM_TRACK_CUTOFF 3
#define LIVE_PARAM_TRACK_RESONANCE 4
#define LIVE_PARAM_TRACK_REVERB_SEND 5
#define LIVE_PARAM_TRACK_THRESHOLD 6
#define LIVE_PARAM_TRACK_CLAMP 7
#define LIVE_PARAM_TRACK_PANNING 8
#define LIVE_PARAM_TRACK_VOLUME 9
#define LIVE_PARAM_TRACK_LFO_CARRIER 10
#define LIVE_PARAM_303_1_CUTOFF 11
#define LIVE_PARAM_303_2_CUTOFF 12
#define LIVE_PARAM_303_1_RESONANCE 13
#define LIVE_PARAM_303_2_RESONANCE 14
#define LIVE_PARAM_303_1_ENVMOD 15
#define LIVE_PARAM_303_2_ENVMOD 16
#define LIVE_PARAM_303_1_DECAY 17
#define LIVE_PARAM_303_2_DECAY 18
#define LIVE_PARAM_303_1_TUNE 19
#define LIVE_PARAM_303_2_TUNE 20
#define LIVE_PARAM_303_1_ACCENT 21
#define LIVE_PARAM_303_2_ACCENT 22
#define LIVE_PARAM_303_1_VOLUME 23
#define LIVE_PARAM_303_2_VOLUME 24

#define SCOPE_ZONE_SCOPE 0
#define SCOPE_ZONE_MOD_DIR 1
#define SCOPE_ZONE_INSTR_DIR 2
#define SCOPE_ZONE_PRESET_DIR 3
#define SCOPE_ZONE_INSTR_LIST 4
#define SCOPE_ZONE_SYNTH_LIST 5
#define SCOPE_ZONE_REVERB_DIR 6
#define SCOPE_ZONE_PATTERN_DIR 7
#define SCOPE_ZONE_SAMPLE_DIR 8
#define SCOPE_ZONE_MIDICFG_DIR 9
#define SCOPE_LAST_DIR 10

#define MIN_VUMETER 22
#define MAX_VUMETER 155
#define LARG_VUMETER 134
#define MIN_PEAK MAX_VUMETER - 18

#define MAX_PATH 260
#define MAX_TRACKS 16
#define MAX_POLYPHONY 16
#define DEFAULT_POLYPHONY 1
#define MAX_FILTER 23
#define MAX_COMB_FILTERS 10
#define MAX_EVENTS 16

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_BPP 8

#define MOUSE_LEFT_BUTTON 1
#define MOUSE_MIDDLE_BUTTON 2
#define MOUSE_RIGHT_BUTTON 4

#define SDL_MOUSE_LEFT_BUTTON 1
#define SDL_MOUSE_MIDDLE_BUTTON 2
#define SDL_MOUSE_RIGHT_BUTTON 3


#define ZZAAPP_ALL 1
#define ZZAAPP_PATTERNS 2
#define ZZAAPP_INSTRUMENTS 3
#define ZZAAPP_SPLIT 4
#define ZZAAPP_SYNTHS 5
#define ZZAAPP_303 6

/* replay.h defines */

#define NOTE_MAX 119
#define NOTE_OFF 120
#define NO_NOTE 121
#define NO_INSTR 255

#define MAX_ROWS 128
#define MAX_SEQUENCES 256
#define MAX_INSTRS 128
#define MAX_PATTERNS 128
#define MAX_INSTRS_SPLITS 16
#define DEFAULT_PATTERN_LEN 64

#define PATTERN_NOTE1 0
#define PATTERN_INSTR1 1
#define PATTERN_NOTE2 2
#define PATTERN_INSTR2 3
#define PATTERN_NOTE3 4
#define PATTERN_INSTR3 5
#define PATTERN_NOTE4 6
#define PATTERN_INSTR4 7
#define PATTERN_NOTE5 8
#define PATTERN_INSTR5 9
#define PATTERN_NOTE6 10
#define PATTERN_INSTR6 11
#define PATTERN_NOTE7 12
#define PATTERN_INSTR7 13
#define PATTERN_NOTE8 14
#define PATTERN_INSTR8 15
#define PATTERN_NOTE9 16
#define PATTERN_INSTR9 17
#define PATTERN_NOTE10 18
#define PATTERN_INSTR10 19
#define PATTERN_NOTE11 20
#define PATTERN_INSTR11 21
#define PATTERN_NOTE12 22
#define PATTERN_INSTR12 23
#define PATTERN_NOTE13 24
#define PATTERN_INSTR13 25
#define PATTERN_NOTE14 26
#define PATTERN_INSTR14 27
#define PATTERN_NOTE15 28
#define PATTERN_INSTR15 29
#define PATTERN_NOTE16 30
#define PATTERN_INSTR16 31

#define PATTERN_VOLUME 32
#define PATTERN_PANNING 33
#define PATTERN_FX 34
#define PATTERN_FXDATA 35

#define PATTERN_FX2 36
#define PATTERN_FXDATA2 37

#define PATTERN_FX3 38
#define PATTERN_FXDATA3 39

#define PATTERN_FX4 40
#define PATTERN_FXDATA4 41

#define PATTERN_BYTES (PATTERN_FXDATA4 + 1)
#define PATTERN_ROW_LEN MAX_TRACKS * PATTERN_BYTES
#define PATTERN_TRACK_LEN MAX_ROWS * PATTERN_BYTES
#define PATTERN_LEN PATTERN_ROW_LEN * MAX_ROWS
#define PATTERN_POOL_SIZE PATTERN_LEN * MAX_PATTERNS



// ------------------------------------------------------
// Structures
typedef struct
{
    int Start_Row;
    int Start_Col;
    int End_Row;
    int End_Col;
    int cStatus;
} MARKER, *LPMARKER;

typedef struct
{
    int x;
    int y;
    int old_x;
    int old_y;
    int button;
    int button_oneshot;
    int wheel;
} MOUSE, *LPMOUSE;


extern int Pattern_Line;
extern int Pattern_Line_Visual;
extern int Song_Position;
extern int Song_Position_Visual;

extern char userscreen;

extern int last_index;

extern int Pos_Tbl_Synth_OSC1;
extern int Pos_Tbl_Synth_OSC2;
extern int Pos_Tbl_Synth_VCF;
extern int Pos_Tbl_Synth_LFO1;
extern int Pos_Tbl_Synth_LFO2;
extern int Pos_Tbl_Synth_ENV1;
extern int Pos_Tbl_Synth_ENV2;

extern char draw_sampled_wave;
extern char draw_sampled_wave2;
extern char draw_sampled_wave3;
extern char Current_Instrument_Split;
extern int resty;
extern int rs_coef;

extern SDL_Surface *Main_Screen;

extern float left_float_render;
extern float right_float_render;

extern SDL_Surface *PFONT;
extern SDL_Surface *FONT;
extern SDL_Surface *FONT_LOW;

extern int gui_action;
extern int gui_action_metronome;
extern char teac;

extern MOUSE Mouse;

extern int fluzy;

extern signed char c_midiin;
extern signed char c_midiout;

extern int namesize;

extern int liveparam;
extern int livevalue;

extern int Final_Mod_Length;

extern char name[MAX_PATH];
extern char artist[20];

extern char style[20];

extern char trkchan;

extern char seditor;
extern char actuloop;

extern char Visible_Columns;
extern int VIEWLINE;
extern char is_editing;
extern char is_recording;
extern char is_recording_2;
extern char is_record_key;

extern char sr_isrecording;

extern int Songplaying;

extern int Current_Edit_Steps;
extern int Current_Octave;

extern int restx;
extern int resty;
extern int fsize;

extern unsigned int SamplesPerSub;

extern char sas;

extern float *Scope_Dats[MAX_TRACKS];
extern float *Scope_Dats_LeftRight[2];

extern SDL_Surface *SKIN303;
extern SDL_Surface *LOGOPIC;

extern int MouseWheel_Multiplier;
extern char Rows_Decimal;
extern char See_Prev_Next_Pattern;

extern MARKER Patterns_Marker;

extern int LastProgram[MAX_TRACKS];

extern Uint32 Alloc_midi_Channels[MAX_TRACKS][MAX_POLYPHONY];

extern char *ExePath;

extern int allow_save;

extern char Channels_Polyphony[MAX_TRACKS];
extern char Channels_MultiNotes[MAX_TRACKS];
extern char Channels_Effects[MAX_TRACKS];

void WavRenderizer(ptk_data *ptk);
void Newmod(ptk_data *ptk);
void Initreverb(ptk_data *ptk);
int Get_Number_Of_Splits(int n_index);
void Clear_Instrument_Dat(int n_index, int split, int lenfir);

#ifdef __cplusplus
extern "C" {
#endif
void AllocateWave(int n_index, int split, long lenfir,
                  int samplechans, int clear,
                  short *Waveform1, short *Waveform2);
#ifdef __cplusplus
}
#endif

void LoadFile(ptk_data *ptk, int Freeindex, const char *str);
void RefreshSample(ptk_data *ptk);
void ShowInfo(ptk_data *ptk);
void out_decchar(int x, int y, int number, char smith);

void out_nibble(int x, int y, int color, int number);
void SongPlay(ptk_data *ptk);
void SongStop(ptk_data *ptk);
void Stop_Current_Instrument(ptk_data *ptk);
void DeleteInstrument(ptk_data *ptk);
void StartRec(ptk_data *ptk);
void Notify_Edit(ptk_data *ptk);
void Notify_Play(ptk_data *ptk);
char zcheckMouse_nobutton(int x, int y, int xs, int ys);
char zcheckMouse(ptk_data *ptk, int x, int y, int xs, int ys);
void Keyboard_Handler(ptk_data *ptk);
void Mouse_Handler(ptk_data *ptk);
int Get_Track_Over_Mouse(ptk_data *ptk, int Mouse, int *Was_Scrolling, int Left);
void Set_Track_Slider(ptk_data *ptk, int pos);
#define BLOCK_MARK_TRACKS 1
#define BLOCK_MARK_ROWS 2
void Insert_Selection(ptk_data *ptk, int Cur_Track, int Position);
void Remove_Selection(ptk_data *ptk, int Cur_Track, int Position);
int Next_Line_Pattern_Auto(ptk_data *ptk, int *position, int lines, int *line);
void Draw_Scope_Files_Button(ptk_data *ptk);

void Actualize_Master(ptk_data *ptk, char gode);
void Display_Master_Comp(void);
void Display_Master_Volume(void);
void Display_Shuffle(void);
void Actualize_Input(ptk_data *ptk);
void Draw_Scope(ptk_data *ptk);
int Init_Scopes_Buffers(ptk_data *ptk);

void Switch_Cmd_Playing(int Enable);

#endif
