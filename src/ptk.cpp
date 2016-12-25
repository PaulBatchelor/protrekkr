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

// ------------------------------------------------------
// Includes
#include <stdlib.h>
#include "ptk_data.h"
#include "variables.h"
#include "replay.h"
#include "ptk.h"
#include "ptk_lua.h"
#include "ptk_sporth.h"
#include "tab.h"

#include "reverbs.h"
#include "editor_303.h"
#include "303s.h"
#include "midi_cfg.h"
#include "mods.h"
#include "patterns.h"
#include "files.h"
#include "xml.h"
#include "main.h"
#include "aiff.h"
#include "requesters.h"
#include "editor_setup.h"
#include "editor_fx_setup.h"
#include "editor_synth.h"
#include "editor_track_fx.h"
#include "editor_diskio.h"
#include "editor_track.h"
#include "editor_instrument.h"
#include "editor_sample.h"

// ------------------------------------------------------
// Constants
// This is a nasty hack: we should have a specific ASCII buffer
// instead of using the unicode one.
#if !defined(__MACOSX__)
#define UNICODE_OFFSET1 0x20
#define UNICODE_OFFSET2 0x60
#else
#define UNICODE_OFFSET1 0
#define UNICODE_OFFSET2 0
#endif

// ------------------------------------------------------
// Variables
#if defined(__MACOSX__)
SystemSoundActionID WavActionID;
#endif

//extern int ptk->metronome_rows_counter;
//extern int ptk->metronome_magnify;

//extern int Songplaying_Pattern;

extern short *Player_WL[MAX_TRACKS][MAX_POLYPHONY];
extern short *Player_WR[MAX_TRACKS][MAX_POLYPHONY];

//extern VISUAL_DELAY_DAT Delays_Pos_Sound_Buffer[256];
//extern int Delay_Sound_Buffer;
//extern int Cur_Delay_Sound_Buffer;
//extern int Subicounter;

#if defined(PTK_SHUFFLE)
extern int shufflestep;
extern int shuffleswitch;
#endif

//extern char Use_Cubic;
//extern char Paste_Across;

//unsigned char sl3 = 0;

//extern int ptk->pos_scope;
//extern int ptk->pos_scope_latency;

extern float sp_Tvol_Mod[MAX_TRACKS];

ptk_data ptk;

/*TODO: this variable conflicts with tVisible_Columns somewhere */
char Visible_Columns = 0;

/*TODO: this variable xoffseted is not unique. */
int xoffseted;
//float gr = 0;
//float synthsignal = 0;

//int c_r_release = 0;
//int c_l_release = 0;

//int ptk->restx = 0;
//int ptk->resty = 0;

//int LastPedRow = -1;
//char ptk->po_alt = TRUE;




SDL_Surface *PFONT;
SDL_Surface *FONT;
SDL_Surface *FONT_LOW;

SDL_TimerID Timer;
Uint32 Timer_CallBack(Uint32 interval, void *param);
Uint32 (*Timer_Ptr)(Uint32 interval, void *param) = &Timer_CallBack;

extern s_access sp_Position[MAX_TRACKS][MAX_POLYPHONY];

extern int done;
//extern float local_curr_mas_vol;

SDL_Surface *LOGOPIC;
Uint32 Alloc_midi_Channels[MAX_TRACKS][MAX_POLYPHONY];

void Mouse_Sliders_Master_Shuffle(ptk_data *ptk);
unsigned long Calc_Length(ptk_data *ptk);

//extern int Continuous_Scroll;

void Draw_Scope(ptk_data *ptk);
void Draw_Scope_Files_Button(ptk_data *ptk);
void Display_Tracks_To_Render(ptk_data *ptk);
void Solo_Track(int track_to_solo);

static int vi_normal_mode(ptk_data *ptk, 
        unsigned short l_c, 
        unsigned short u_c, 
        bool is_editing);

JAZZ_KEY Sub_Channels_Jazz[MAX_TRACKS][MAX_POLYPHONY];

static int Values_AutoSave[] =
{
    0,
    1 * 60,
    2 * 60,
    4 * 60,
    8 * 60,
    10 * 60,
    15 * 60,
    30 * 60
};

static int Table_Right_Tab_Notes[] =
{
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
    3, 2, 1,
};

static int Table_Left_Tab_Notes[] =
{
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
    3, 1, 2,
};

//int Keyboard_Events_Channels[256];
JAZZ_KEY Sub_Channels_NoteOff[MAX_TRACKS][MAX_POLYPHONY];

REQUESTER_BUTTON Requester_Btn_Cancel =
{
    NULL,
    "Cancel",
    BUTTON_CANCEL,
    SDLK_c
};

REQUESTER_BUTTON Requester_Btn_303 =
{
    &Requester_Btn_Cancel,
    "303 Units",
    0,
    SDLK_u
};

REQUESTER_BUTTON Requester_Btn_Synths =
{
    &Requester_Btn_303,
    "Synths",
    0,
    SDLK_s
};

REQUESTER_BUTTON Requester_Btn_Instruments =
{
    &Requester_Btn_Synths,
    "Instruments",
    0,
    SDLK_i
};

REQUESTER_BUTTON Requester_Btn_Patterns =
{
    &Requester_Btn_Instruments,
    "Patterns",
    0,
    SDLK_p
};

REQUESTER_BUTTON Requester_Btn_All =
{
    &Requester_Btn_Patterns,
    "All",
    BUTTON_DEFAULT,
    SDLK_a
};

REQUESTER_BUTTON Requester_Btn_No =
{
    NULL,
    "No",
    BUTTON_CANCEL,
    SDLK_n
};

REQUESTER_BUTTON Requester_Btn_Yes =
{
    &Requester_Btn_No,
    "Yes",
    BUTTON_DEFAULT,
    SDLK_y
};

REQUESTER_BUTTON Requester_Btn_Synth =
{
    &Requester_Btn_Cancel,
    "Synth",
    0,
    SDLK_s
};

REQUESTER_BUTTON Requester_Btn_Split =
{
    &Requester_Btn_Synth,
    "Split",
    0,
    SDLK_p
};

REQUESTER_BUTTON Requester_Btn_Instrument =
{
    &Requester_Btn_Split,
    "Instrument",
    0,
    SDLK_i
};

REQUESTER_BUTTON Requester_Btn_All_Delete =
{
    &Requester_Btn_Instrument,
    "All",
    BUTTON_DEFAULT,
    SDLK_a
};

extern REQUESTER Title_Requester;

REQUESTER Exit_Requester =
{
    "Do you really want to quit ?",
    &Requester_Btn_Yes,
    NULL, 0
};

REQUESTER Overwrite_Requester =
{
    "",
    &Requester_Btn_Yes,
    NULL, 0
};

REQUESTER Zzaapp_Requester =
{
    "What do you want to Zzaapp ?",
    &Requester_Btn_All,
    NULL, 0
};

REQUESTER Delete_Requester =
{
    "What do you want to delete ?",
    &Requester_Btn_All_Delete,
    NULL, 0
};

static int mt_tags[16] =
{
    'NHC1', 'NHC2', 'NHC3', '.K.M',
    'NHC5', 'NHC6', 'NHC7', 'NHC8',
    'NHC9', 'HC01', 'HC11', 'HC21',
    'HC31', 'HC41', 'HC51', 'HC61'
};

static char mt_channels[16] =
{
     1,  2,  3,  4,
     5,  6,  7,  8,
     9, 10, 11, 12,
    13, 14, 15, 16
};

// ------------------------------------------------------
// Load a skin picture according to the xml script
SDL_Surface *Load_Skin_Picture(char *name)
{
    char filepath[MAX_PATH];
    const char *pic_filename;
    char error[256];
    SDL_Surface *surface;

    pic_filename = XML_get_string("files", "file", name, "value");
    if(!pic_filename)
    {
        Message_Error("XML error in files tag.");
        return(NULL);
    }

#if defined(__WIN32__)
    sprintf(filepath, "skins\\%s", pic_filename);
#else
/*TODO: handle file loading better*/
    sprintf(filepath, "/usr/local/share/protrekkr/skins/%s", pic_filename);
#endif

    surface = Load_Picture(filepath);
    if(surface == NULL)
    {
        sprintf(error, "Can't load %s file.", pic_filename);
        Message_Error(error);
        return(NULL);
    }
    return(surface);
}

// ------------------------------------------------------
// Load the font letters sizes according to the xml script
int Load_Font_Datas(char *name)
{
    char filepath[MAX_PATH];
    const char *pic_filename;
    char error[256];

    pic_filename = XML_get_string("files", "file", name, "value");

    if(!pic_filename)
    {
        Message_Error("XML error in files tag.");
        return(FALSE);
    }

#if defined(__WIN32__)
    sprintf(filepath, "skins\\%s", pic_filename);
#else
    //sprintf(filepath, "skins/%s", pic_filename);
    /* TODO: handle filepaths better*/
    sprintf(filepath, "/usr/local/share/protrekkr/skins/%s", pic_filename);
#endif
    if(!Create_Font_Datas(filepath))
    {
        sprintf(error, "Can't load %s file.", pic_filename);
        Message_Error(error);
        return(FALSE);
    }
    return(TRUE);
}

// ------------------------------------------------------
// Load the necessary datas and initialize the interface
int Init_Context(ptk_data *ptk)
{
#if defined(__WIN32__)
    srand(GetTickCount());
#else
    srand(rand());
#endif

    ptk->ctipoftheday = rand() % 12;

    switch(ptk->ctipoftheday)
    {
        case 0: sprintf(ptk->tipoftheday, "Tip Of The Hour: Pressing CTRL+I will interpolate effect value on a marked block."); break;
        case 1: sprintf(ptk->tipoftheday, "Tip Of The Hour: The right mouse button will have a secondary action on most buttons."); break;
        case 2: sprintf(ptk->tipoftheday, "Tip Of The Hour: Don't set excessive track reverb send values, to get better quality."); break;
        case 3: sprintf(ptk->tipoftheday, "Tip Of The Hour: When recording, number of positions will grow automatically when necessary."); break;
        case 4: sprintf(ptk->tipoftheday, "Remember: MIDI is not audio, realtime fx will not affect midi sound."); break;
        case 5: sprintf(ptk->tipoftheday, "Tip Of The Hour: On lower CPUs, you can renderize patterns to wav, and use them as samples without any loss of quality."); break;
        case 6: sprintf(ptk->tipoftheday, "Tip Of The Hour: Volume note-cut command 'Fx' is very useful to avoid sample-clicking."); break;
        case 7: sprintf(ptk->tipoftheday, "Tip Of The Hour: Left-Clicking on pattern editor channels numbers will mute/unmute any track (right clicking will solo it)."); break;
        case 8: sprintf(ptk->tipoftheday, "Tip Of The Hour: Pattern command '16xx' will reset the Filter LFO of the track. No parameter required."); break;
        case 9: sprintf(ptk->tipoftheday, "Tip Of The Hour: Use '90' command in the panning column to change midi controllers values."); break;
        case 10: sprintf(ptk->tipoftheday, "Tip Of The Hour: Increase latency time if sound is distorted."); break;
        case 11: sprintf(ptk->tipoftheday, "Tip Of The Hour: Pressing right mouse button on most arrows buttons (\03\04) will speed operation up.");

        default: sprintf(ptk->tipoftheday, "Tip Of The Hour: See readme.txt for more infos about help and pattern commands."); break;
    }

    L_MaxLevel = 0;
    R_MaxLevel = 0;

    sprintf(Selection_Name, "Untitled");
    sprintf(Reverb_Name, "Untitled");
    sprintf(Midi_Name, "Untitled");
    sprintf(ptk->name, "Untitled");
    sprintf(ptk->artist, "Somebody");
    sprintf(ptk->style, "Anything goes");

    ptk->namesize = 8;
    IniCsParNames(ptk);

    Clear_Files_List(ptk);

#if !defined(__NO_MIDI__)
    midiin_changed = 2;
    midiout_changed = 2;

    Midi_InitIn(ptk);
    Midi_InitOut(ptk);
#endif

    GETCWD(ptk->Prog_Path, MAX_PATH);

    Read_SMPT(ptk);

    //CHDIR(ptk->Prog_Path);

    if(ptk->start_gui == TRUE) if(!Init_Block_Work(ptk)) return(FALSE);

#if !defined(__NO_MIDI__)
    Midi_Reset(ptk);
#endif

    if(!Alloc_Patterns_Pool(ptk))
    {
        Message_Error("Can't allocate room for patterns.");
        return(FALSE);
    }

    // Player initialization
	if(ptk->start_gui == TRUE) {
#if defined(__WIN32__)
		if(!Ptk_InitDriver(ptk, Main_Window, AUDIO_Milliseconds))
#else
		if(!Ptk_InitDriver(ptk, AUDIO_Milliseconds))
#endif
		{
			Ptk_ReleaseDriver(ptk);
			return(FALSE);
		}
	}

    Set_Default_Channels_Polyphony();
    init_sample_bank(ptk);
    Pre_Song_Init(ptk);
    Post_Song_Init(ptk);

    // Old preset by default
    Load_Old_Reverb_Presets(ptk, 0);

    Initreverb(ptk);

	if(ptk->start_gui == TRUE) {
		LOGOPIC = Load_Skin_Picture("logo");
		if(!LOGOPIC) return(FALSE);

		POINTER = Load_Skin_Picture("pointer");
		if(!POINTER) return(FALSE);
		SKIN303 = Load_Skin_Picture("303");
		if(!SKIN303) return(FALSE);
		PFONT = Load_Skin_Picture("pattern_font");
		if(!PFONT) return(FALSE);
		FONT = Load_Skin_Picture("font");
		if(!FONT) return(FALSE);
		FONT_LOW = Load_Skin_Picture("font");
		if(!FONT_LOW) return(FALSE);

		if(!Set_Pictures_Colors()) return(FALSE);

		if(!Load_Font_Datas("font_datas")) return(FALSE);

		SDL_SetColorKey(FONT, SDL_SRCCOLORKEY, 0);
		SDL_SetColorKey(FONT_LOW, SDL_SRCCOLORKEY, 0);
		SDL_SetColorKey(POINTER, SDL_SRCCOLORKEY, 0);

		Timer = SDL_AddTimer(1000, Timer_Ptr, &ptk);
	}
    return(TRUE);
}

// ------------------------------------------------------
// Release the allocated resources
void Destroy_Context(ptk_data *ptk)
{
    if(Timer) SDL_RemoveTimer(Timer);
    ptk->AUDIO_Acknowledge = TRUE;

    Ptk_ReleaseDriver(ptk);

#if !defined(__NO_MIDI__)
    // Close any opened midi devices on any exit
    Midi_CloseIn(ptk);
    Midi_CloseOut(ptk);
#endif

#if !defined(__NO_MIDI__)
    // Free the devices enumeration
    Midi_FreeAll(ptk);
#endif

    Free_Samples(ptk);

    // Freeing Allocated Patterns
    if(ptk->RawPatterns) free(ptk->RawPatterns);

    Destroy_UI();
    SDL_Quit();
}

// ------------------------------------------------------
// Strictly for Mac OS X
#if defined(__MACOSX__)
OSStatus CompletionRoutine(SystemSoundActionID inAction, void *UserDat)
{
    SystemSoundRemoveActionID(inAction);
    return(noErr);
}
#endif

// ------------------------------------------------------
// Update the world
int Screen_Update(ptk_data *ptk)
{
    int FineTune_Value;
    int i;

#if defined(__MACOSX__)
    FSRef soundFileRef;
#endif

    ptk->redraw_everything = FALSE;
    if(Env_Change)
    {
        ptk->redraw_everything = TRUE;
        Env_Change = FALSE;
        
    }

    for(i = 0; i < Channels_Polyphony[ptk->Track_Under_Caret]; i++)
    {
        if(sp_Stage[ptk->Track_Under_Caret][i] == PLAYING_SAMPLE && ptk->Current_Instrument == sp_channelsample[ptk->Track_Under_Caret][i] && ptk->Current_Instrument_Split == sp_split[ptk->Track_Under_Caret][i])
        {
            ptk->draw_sampled_wave2 = TRUE;
            ptk->boing = TRUE;
            break;
        }
        else if(ptk->boing)
        {
            ptk->boing = FALSE;
            ptk->draw_sampled_wave3 = TRUE;
            break;
        }
    }

    if(ptk->Scopish == SCOPE_ZONE_SCOPE) Draw_Scope(ptk);

    // Sample ed.
    Draw_Wave_Data(ptk);

    int Lt_vu = (int) (MIN_VUMETER + (((float) L_MaxLevel / 32767.0f) * LARG_VUMETER));
    int Rt_vu = (int) (MIN_VUMETER + (((float) R_MaxLevel / 32767.0f) * LARG_VUMETER));
    int Lt_vu_Peak = Lt_vu;
    int Rt_vu_Peak = Rt_vu;
    if(Lt_vu_Peak > MAX_VUMETER - 2) Lt_vu_Peak = MAX_VUMETER - 2;
    if(Rt_vu_Peak > MAX_VUMETER - 2) Rt_vu_Peak = MAX_VUMETER - 2;
    if(Lt_vu > MIN_PEAK) Lt_vu = MIN_PEAK;
    if(Rt_vu > MIN_PEAK) Rt_vu = MIN_PEAK;

    // Draw the vu meters
    for(i = MIN_VUMETER ; i < Lt_vu; i += 2)
    {
        DrawVLine(i, 10, 13, COL_VUMETER);
    }
    for(i = MIN_VUMETER ; i < Rt_vu; i += 2)
    {
        DrawVLine(i, 15, 18, COL_VUMETER);
    }
    for(i = Lt_vu + 1; i < Lt_vu_Peak; i += 2)
    {
        DrawVLine(i, 10, 13, COL_VUMETERPEAK);
    }
    for(i = Rt_vu + 1; i < Rt_vu_Peak; i += 2)
    {
        DrawVLine(i, 15, 18, COL_VUMETERPEAK);
    }

    // Clear
    DrawHLine(10, Lt_vu_Peak, MAX_VUMETER - 2, COL_BACKGROUND);
    DrawHLine(11, Lt_vu_Peak, MAX_VUMETER - 2, COL_BACKGROUND);
    DrawHLine(12, Lt_vu_Peak, MAX_VUMETER - 2, COL_BACKGROUND);
    DrawHLine(13, Lt_vu_Peak, MAX_VUMETER - 2, COL_BACKGROUND);

    DrawHLine(15, Rt_vu_Peak, MAX_VUMETER - 2, COL_BACKGROUND);
    DrawHLine(16, Rt_vu_Peak, MAX_VUMETER - 2, COL_BACKGROUND);
    DrawHLine(17, Rt_vu_Peak, MAX_VUMETER - 2, COL_BACKGROUND);
    DrawHLine(18, Rt_vu_Peak, MAX_VUMETER - 2, COL_BACKGROUND);

    if(ptk->actuloop) Afloop(ptk);

    if(ptk->gui_action_metronome == GUI_CMD_FLASH_METRONOME_ON)
    {
        SetColor(COL_VUMETERPEAK);
        Fillrect(73, 83, 73 + 15, 83 + 15);
    }
    if(ptk->gui_action_metronome == GUI_CMD_FLASH_METRONOME_OFF)
    {
        Gui_Draw_Button_Box(72, 82, 16, 16, "", BUTTON_DISABLED);
    }
    ptk->gui_action_metronome = GUI_CMD_NOP;

    if(ptk->gui_action != GUI_CMD_NONE)
    { // There are some for me today.....:)

        // Files list slider
        if(ptk->gui_action == GUI_CMD_SET_FILES_LIST_SLIDER)
        {
            lt_ykar[ptk->Scopish] = Mouse.y - 72;
            Actualize_Files_List(ptk, 0);
        }

        // File selection
        if(ptk->gui_action == GUI_CMD_SET_FILES_LIST_SELECT_FILE)
        {
            if(lt_items[ptk->Scopish])
            {
                int broadcast = lt_index[ptk->Scopish] + (Mouse.y - 43) / (Font_Height + 1);
                if(Get_FileType(ptk, broadcast) != _A_SEP)
                {
                    ptk->last_index = -1;
                    if(broadcast != lt_curr[ptk->Scopish])
                    {
                        lt_curr[ptk->Scopish] = broadcast;
                        Actualize_Files_List(ptk, 1);
                    }
                    else
                    {
                        switch(Get_FileType(ptk, lt_curr[ptk->Scopish]))
                        {
                            case _A_FILE:
                                Stop_Current_Instrument(ptk);
                                LoadFile(ptk, ptk->Current_Instrument, Get_FileName(ptk, lt_curr[ptk->Scopish]));
                                break;
                            case _A_SUBDIR:
                                Set_Current_Dir(ptk);
                                Read_SMPT(ptk);
                                Actualize_Files_List(ptk, 0);
                                break;
                        }
                    }
                }
            }
        }

        // Play a .wav
        if(ptk->gui_action == GUI_CMD_SET_FILES_LIST_PLAY_WAV)
        {
            if(lt_items[ptk->Scopish])
            {
                int broadcast = lt_index[ptk->Scopish] + (Mouse.y - 43) / 12;
                ptk->last_index = -1;
                lt_curr[ptk->Scopish] = broadcast;
                switch(Get_FileType(ptk, lt_curr[ptk->Scopish]))
                {
                    case _A_FILE:
                        Actualize_Files_List(ptk, 1);
#if defined(__WIN32__)
                        PlaySound(Get_FileName(ptk, lt_curr[ptk->Scopish]), NULL, SND_FILENAME | SND_ASYNC);
#endif
#if defined(__MACOSX__)
                        if(FSPathMakeRef((Uint8 *) Get_FileName(ptk, lt_curr[ptk->Scopish]), &soundFileRef, NULL) == noErr)
                        {
                            SystemSoundGetActionID(&soundFileRef, &WavActionID);
                            SystemSoundSetCompletionRoutine(WavActionID,
                                                            NULL,
                                                            NULL,
                                                            &CompletionRoutine,
                                                            NULL);
                            AlertSoundPlayCustomSound(WavActionID);
                        }
#endif
#if defined(__LINUX__)
                        FILE *fptr;
                        char temp[1024];

                        sprintf(temp, "aplay %s & > /dev/null", Get_FileName(ptk, lt_curr[ptk->Scopish]));
                        fptr = popen(temp, "r");
                        pclose(fptr);
#endif
#if defined(__AROS__) || defined(__AMIGAOS4__)
                        // TODO
#endif

                        break;
                }
            }
        }

        // Instruments/synths list slider
        if(ptk->gui_action == GUI_CMD_SET_INSTR_SYNTH_LIST_SLIDER)
        {
            Instrs_ykar = Mouse.y - 72;
            Actualize_Instruments_Synths_List(ptk, 0);
        }

        // Select an instrument/synth
        if(ptk->gui_action == GUI_CMD_SET_INSTR_SYNTH_LIST_SELECT)
        {
            ptk->Current_Instrument = Instrs_index + (Mouse.y - 43) / 12;
            Actualize_Instruments_Synths_List(ptk, 1);
            Actualize_Patterned(ptk);
            RefreshSample(ptk);
            Renew_Sample_Ed(ptk);
            Actualize_Synth_Ed(ptk, UPDATE_SYNTH_ED_ALL);
        }

        // --- Sequence ---------------------------------------

        if(ptk->gui_action == GUI_CMD_PREVIOUS_POSITION)
        {
            Goto_Song_Position(ptk, Song_Position - 1);
        }

        if(ptk->gui_action == GUI_CMD_NEXT_POSITION)
        {
            Goto_Song_Position(ptk, Song_Position + 1);
        }

        if(ptk->gui_action == GUI_CMD_SET_PATTERN_LENGTH)
        {
            Actualize_Sequencer(ptk);
            Actupated(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_PREV_PATT)
        {
            ptk->pSequence[Song_Position]--;
            Bound_Patt_Pos(ptk);
            Actualize_Sequencer(ptk);
            Actupated(ptk, 0);
            Anat(ptk, Song_Position);
        }

        if(ptk->gui_action == GUI_CMD_NEXT_PATT)
        {
            ptk->pSequence[Song_Position]++;
            Bound_Patt_Pos(ptk);
            Actualize_Sequencer(ptk);
            Actupated(ptk, 0);
            Anat(ptk, Song_Position);
        }

        if(ptk->gui_action == GUI_CMD_GOTO_PREVIOUS_ROW)
        {
            Goto_Previous_Row(ptk);
        }

        if(ptk->gui_action == GUI_CMD_GOTO_NEXT_ROW)
        {
            Goto_Next_Row(ptk);
        }

        if(ptk->gui_action == GUI_CMD_REDUCE_SONG_LENGTH)
        {
            ptk->Song_Length--;
            Actualize_Sequencer(ptk);
            Actupated(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_INCREASE_SONG_LENGTH)
        {
            ptk->Song_Length++;
            Actualize_Sequencer(ptk);
            Actupated(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_UPDATE_SEQUENCER)
        {
            Actualize_Sequencer(ptk);
        }

        if(ptk->gui_action == GUI_CMD_SWITCH_TRACK_STATUS)
        {
            Toggle_Track_On_Off_Status(ptk, Ext_Pos_Switch, Ext_Track_Switch);
            Actualize_Sequencer(ptk);
        }

        if(ptk->gui_action == GUI_CMD_UPDATE_PATTERN)
        {
            Actupated(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_SET_FOCUS_TRACK_EXTERNAL)
        {
            Actupated(ptk, 0);
            Actualize_Track_Ed(ptk, 0);
            Actualize_Track_Fx_Ed(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_SET_FOCUS_TRACK)
        {
            Actualize_Track_Ed(ptk, 0);
            Actualize_Track_Fx_Ed(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_LOWER_OCTAVE)
        {
            Current_Octave--;
            Actualize_Patterned(ptk);
        }

        if(ptk->gui_action == GUI_CMD_HIGHER_OCTAVE)
        {
            Current_Octave++;
            Actualize_Patterned(ptk);
        }

        if(ptk->gui_action == GUI_CMD_DECREASE_STEP_ADD)
        {
            Current_Edit_Steps--;
            if(Current_Edit_Steps < 0) Current_Edit_Steps = 0;
            Actualize_Patterned(ptk);
        }

        if(ptk->gui_action == GUI_CMD_INCREASE_STEP_ADD)
        {
            Current_Edit_Steps++;
            if(Current_Edit_Steps > 16) Current_Edit_Steps = 16;
            Actualize_Patterned(ptk);
        }

        if(ptk->gui_action == GUI_CMD_SELECT_INSTR_EXTERNAL)
        {
            Clear_Input(ptk);
            Actualize_Patterned(ptk);
            RefreshSample(ptk);
            Renew_Sample_Ed(ptk);
            Actualize_Synth_Ed(ptk, UPDATE_SYNTH_ED_ALL);
        }

        if(ptk->gui_action == GUI_CMD_PREV_INSTR)
        {
            ptk->Current_Instrument--;
            Clear_Input(ptk);
            Actualize_Patterned(ptk);
            RefreshSample(ptk);
            Renew_Sample_Ed(ptk);
            Actualize_Synth_Ed(ptk, UPDATE_SYNTH_ED_ALL);
        }

        if(ptk->gui_action == GUI_CMD_NEXT_INSTR)
        {
            ptk->Current_Instrument++;
            Clear_Input(ptk);
            Actualize_Patterned(ptk);
            RefreshSample(ptk);
            Renew_Sample_Ed(ptk);
            Actualize_Synth_Ed(ptk, UPDATE_SYNTH_ED_ALL);
        }

        if(ptk->gui_action == GUI_CMD_SWITCH_TRACK_MUTE_STATE)
        {
            int tmp_track = Get_Track_Over_Mouse(ptk, Mouse.x, NULL, FALSE);

            if(CHAN_MUTE_STATE[tmp_track] == 0) CHAN_MUTE_STATE[tmp_track] = 1;
            else CHAN_MUTE_STATE[tmp_track] = 0;

            if(ptk->userscreen == USER_SCREEN_TRACK_EDIT) Actualize_Track_Ed(ptk, 10);
            Actupated(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_SWITCH_TRACK_LARGE_STATE)
        {
            Toggle_Track_Zoom(ptk, Get_Track_Over_Mouse(ptk, Mouse.x, NULL, FALSE), TRUE);
            Actupated(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_SWITCH_TRACK_SMALL_STATE)
        {
            Toggle_Track_Zoom(ptk, Get_Track_Over_Mouse(ptk, Mouse.x, NULL, FALSE), FALSE);
            Actupated(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_REDUCE_TRACK_NOTES)
        {
            Track_Sub_Notes(ptk, Get_Track_Over_Mouse(ptk, Mouse.x, NULL, FALSE), 1);
            Actupated(ptk, 0);
            Actualize_Track_Ed(ptk, 14);
        }

        if(ptk->gui_action == GUI_CMD_EXPAND_TRACK_NOTES)
        {
            Track_Add_Notes(ptk, Get_Track_Over_Mouse(ptk, Mouse.x, NULL, FALSE), 1);
            Actupated(ptk, 0);
            Actualize_Track_Ed(ptk, 14);
        }

        if(ptk->gui_action == GUI_CMD_REDUCE_TRACK_EFFECTS)
        {
            Track_Sub_Effects(ptk, Get_Track_Over_Mouse(ptk, Mouse.x, NULL, FALSE), 1);
            Actupated(ptk, 0);
            Actualize_Track_Ed(ptk, 14);
        }

        if(ptk->gui_action == GUI_CMD_EXPAND_TRACK_EFFECTS)
        {
            Track_Add_Effects(ptk, Get_Track_Over_Mouse(ptk, Mouse.x, NULL, FALSE), 1);
            Actupated(ptk, 0);
            Actualize_Track_Ed(ptk, 14);
        }

        if(ptk->gui_action == GUI_CMD_PLAY_SONG)
        {
            SongPlay(ptk);
        }

        if(ptk->gui_action == GUI_CMD_STOP_SONG)
        {
            is_recording = 0;
            is_recording_2 = 0;
            ptk->Nbr_Sub_NoteOff = 0;
            //is_editing = 0;
            Notify_Edit(ptk);
            SongStop(ptk);
        }

        if(ptk->gui_action == GUI_CMD_RECORD_303)
        {
            StartRec(ptk);
        }

        if(ptk->gui_action == GUI_CMD_EDIT_MODE)
        {

            is_recording = 0;
            is_editing ^= 1;

            if(is_editing) {
                /* make record bar red */
                ptk->edit_old_r = Ptk_Palette[COL_PATTERN_SEL_BACK].r;
                ptk->edit_old_g = Ptk_Palette[COL_PATTERN_SEL_BACK].g;
                ptk->edit_old_b = Ptk_Palette[COL_PATTERN_SEL_BACK].b;
                Get_Phony_Palette();
                /* turkey red! */
                Phony_Palette[COL_PATTERN_SEL_BACK].r = 169;
                Phony_Palette[COL_PATTERN_SEL_BACK].g = 17;
                Phony_Palette[COL_PATTERN_SEL_BACK].b = 1;
                Set_Phony_Palette();
            } else {
                Get_Phony_Palette();
                Phony_Palette[COL_PATTERN_SEL_BACK].r = ptk->edit_old_r;
                Phony_Palette[COL_PATTERN_SEL_BACK].g = ptk->edit_old_g;
                Phony_Palette[COL_PATTERN_SEL_BACK].b = ptk->edit_old_b;
                Set_Phony_Palette();
            }
            Refresh_Palette();

            is_recording_2 = 0;
            ptk->Nbr_Sub_NoteOff = 0;
            SongStop(ptk);
            Actupated(ptk, 0);
            Notify_Edit(ptk);
        }

        if(ptk->gui_action == GUI_CMD_RECORD_MODE)
        {
            SongStop(ptk);
            is_recording ^= 1;
            is_recording_2 = 0;
            ptk->Nbr_Sub_NoteOff = 0;
            is_editing = 0;
            ptk->key_record_first_time = TRUE;
            Clear_Midi_Channels_Pool();
            Actupated(ptk, 0);
            Notify_Edit(ptk);
        }

        if(ptk->gui_action == GUI_CMD_CHANGE_BPM_TICKS_NBR)
        {
            Actualize_Master(ptk, ptk->teac);
            Display_Beat_Time();
        }

        if(ptk->gui_action == GUI_CMD_CHANGE_TRACKS_NBR ||
           ptk->gui_action == GUI_CMD_INSERT_TRACK ||
           ptk->gui_action == GUI_CMD_DELETE_TRACK)
        {
            Actualize_Master(ptk, ptk->teac);
            Actupated(ptk, 0);
            Draw_Scope(ptk);
            Display_Tracks_To_Render(ptk);
            Actualize_Track_Ed(ptk, 0);
            Actualize_Track_Fx_Ed(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_SET_TRACK_CUTOFF_FREQ)
        {
            TCut[ptk->Track_Under_Caret] = float(Mouse.x - 88);
            Actualize_Track_Ed(ptk, 1);
            ptk->liveparam = LIVE_PARAM_TRACK_CUTOFF;
            ptk->livevalue = (Mouse.x - 88) * 2;
        }

        if(ptk->gui_action == GUI_CMD_SET_TRACK_RESONANCE)
        {
            FRez[ptk->Track_Under_Caret] = Mouse.x - 88;
            Actualize_Track_Ed(ptk, 2);
            ptk->liveparam = LIVE_PARAM_TRACK_RESONANCE;
            ptk->livevalue = (Mouse.x - 88) * 2;
        }

        if(ptk->gui_action == GUI_CMD_SET_TRACK_INERTIA)
        {
            ICut[ptk->Track_Under_Caret] = (Mouse.x - 88.0f) * 0.00006103515625f;
            Actualize_Track_Ed(ptk, 3);
        }

        if(ptk->gui_action == GUI_CMD_UPDATE_TRACK_ED)
        {
            Actualize_Track_Ed(ptk, ptk->teac);
        }

        if(ptk->gui_action == GUI_CMD_SET_TRACK_THRESHOLD)
        {
            DThreshold[ptk->Track_Under_Caret] = float((Mouse.x - 318) * 256);
            Actualize_Track_Ed(ptk, 7);
            ptk->liveparam = LIVE_PARAM_TRACK_THRESHOLD;
            ptk->livevalue = (Mouse.x - 318) * 2;
        }

        if(ptk->gui_action == GUI_CMD_SET_TRACK_CLAMP)
        {
            DClamp[ptk->Track_Under_Caret] = float((Mouse.x - 318) * 256);
            Actualize_Track_Ed(ptk, 8);
            ptk->liveparam = LIVE_PARAM_TRACK_CLAMP;
            ptk->livevalue = (Mouse.x - 318) * 2;
        }

        if(ptk->gui_action == GUI_CMD_SET_TRACK_REVERB_SEND)
        {
            DSend[ptk->Track_Under_Caret] = float(((float) Mouse.x - 318) / 128.0f);
            Actualize_Track_Ed(ptk, 5);
            ptk->liveparam = LIVE_PARAM_TRACK_REVERB_SEND;
            ptk->livevalue = (Mouse.x - 318) * 2;
        }

        if(ptk->gui_action == GUI_CMD_SET_TRACK_PANNING)
        {
            TPan[ptk->Track_Under_Caret] = ((float) Mouse.x - 318) / 128.0f;
            Actualize_Track_Ed(ptk, 9);
            ptk->liveparam = LIVE_PARAM_TRACK_PANNING;
            ptk->livevalue = Mouse.x - 318;
        }

        // Tabs select
        if(ptk->gui_action == GUI_CMD_SELECT_LARGE_PATTERNS)
        {
            if(!Large_Patterns)
            {
                Large_Patterns = TRUE;
                Set_Pattern_Size(ptk);
                Draw_Pattern_Right_Stuff(ptk);
                Actupated(ptk, 0);
                Draw_Editors_Bar(ptk, USER_SCREEN_LARGE_PATTERN);
            }
            else
            {
                ptk->userscreen = curr_tab_highlight;
                Large_Patterns = FALSE;
                Set_Pattern_Size(ptk);
                Draw_Pattern_Right_Stuff(ptk);
                Actupated(ptk, 0);
                Draw_Editors_Bar(ptk, -1);
                Refresh_UI_Context(ptk);
            }
        }

        if(ptk->gui_action == GUI_CMD_SELECT_DISKIO_EDIT)
        {
            ptk->retletter[71] = TRUE;
            Actualize_Input(ptk);
            ptk->retletter[71] = FALSE;
            ptk->userscreen = USER_SCREEN_DISKIO_EDIT;
            Draw_DiskIO_Ed(ptk);
            Actualize_DiskIO_Ed(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_SELECT_REVERB_EDIT)
        {
            ptk->retletter[71] = TRUE;
            Actualize_Input(ptk);
            ptk->retletter[71] = FALSE;
            ptk->userscreen = USER_SCREEN_REVERB_EDIT;
            Draw_Reverb_Ed(ptk);
            Actualize_Reverb_Ed(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_SELECT_TRACK_EDIT)
        {
            ptk->retletter[71] = TRUE;
            Actualize_Input(ptk);
            ptk->retletter[71] = FALSE;
            ptk->userscreen = USER_SCREEN_TRACK_EDIT;
            Draw_Track_Ed(ptk);
            Actualize_Track_Ed(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_SELECT_INSTRUMENT_EDIT)
        {
            ptk->retletter[71] = TRUE;
            Actualize_Input(ptk);
            ptk->retletter[71] = FALSE;
            ptk->userscreen = USER_SCREEN_INSTRUMENT_EDIT;
            Draw_Instrument_Ed(ptk);
            Actualize_Instrument_Ed(ptk, 2, 0);
        }

        if(ptk->gui_action == GUI_CMD_SELECT_FX_EDIT)
        {
            ptk->retletter[71] = TRUE;
            Actualize_Input(ptk);
            ptk->retletter[71] = FALSE;
            ptk->userscreen = USER_SCREEN_FX_SETUP_EDIT;
            Draw_Fx_Ed(ptk);
            Actualize_Fx_Ed(ptk, ptk->teac);
        }

        if(ptk->gui_action == GUI_CMD_SELECT_SEQUENCER)
        {
            ptk->retletter[71] = TRUE;
            Actualize_Input(ptk);
            ptk->retletter[71] = FALSE;
            ptk->userscreen = USER_SCREEN_SEQUENCER;
            Draw_Sequencer_Ed(ptk);
            Actualize_Seq_Ed(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_SELECT_SCREEN_SETUP_EDIT)
        {
            ptk->retletter[71] = TRUE;
            Actualize_Input(ptk);
            ptk->retletter[71] = FALSE;
            ptk->userscreen = USER_SCREEN_SETUP_EDIT;
            Draw_Master_Ed(ptk);
            Actualize_Master_Ed(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_SELECT_MIDI_SETUP)
        {
            //ptk->retletter[71] = TRUE;
            //Actualize_Input(ptk);
            //ptk->retletter[71] = FALSE;
            //ptk->userscreen = USER_SCREEN_SETUP_MIDI;
            //Draw_Midi_Ed(ptk);
            //Actualize_Midi_Ed(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_SELECT_SYNTH_EDIT)
        {
            ptk->retletter[71] = TRUE;
            Actualize_Input(ptk);
            ptk->retletter[71] = FALSE;
            ptk->userscreen = USER_SCREEN_SYNTH_EDIT;
            Draw_Synth_Ed(ptk);
            Actualize_Synth_Ed(ptk, UPDATE_SYNTH_ED_ALL);
        }

        if(ptk->gui_action == GUI_CMD_SELECT_TRACK_FX_EDIT)
        {
            ptk->retletter[71] = TRUE;
            Actualize_Input(ptk);
            ptk->retletter[71] = FALSE;
            ptk->userscreen = USER_SCREEN_TRACK_FX_EDIT;
            Draw_Track_Fx_Ed(ptk);
            Actualize_Track_Fx_Ed(ptk, ptk->teac);
        }

        if(ptk->gui_action == GUI_CMD_SELECT_SAMPLE_EDIT)
        {
            ptk->retletter[71] = TRUE;
            Actualize_Input(ptk);
            ptk->retletter[71] = FALSE;
            ptk->userscreen = USER_SCREEN_SAMPLE_EDIT;
            Draw_Sample_Ed(ptk);
            Actualize_Sample_Ed(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_SELECT_TB303_EDIT)
        {
            ptk->retletter[71] = TRUE;
            Actualize_Input(ptk);
            ptk->retletter[71] = FALSE;
            ptk->userscreen = USER_SCREEN_TB303_EDIT;
            Draw_303_Ed(ptk);
        }

        if(ptk->gui_action == GUI_CMD_EXPORT_WAV)
        {
            char buffer[64];

            Status_Box(ptk, "Writing Wav Header And Sample Data...");

            WaveFile RF;

            if(strlen(SampleName[ptk->Current_Instrument][ptk->Current_Instrument_Split]))
            {
                RF.OpenForWrite(SampleName[ptk->Current_Instrument][ptk->Current_Instrument_Split],
                                44100,
                                16,
                                SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split]);
            }
            else
            {
                RF.OpenForWrite("Untitled.wav",
                                44100,
                                16,
                                SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split]);
            }

            char t_stereo;

            if(SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split] == 1) t_stereo = FALSE;
            else t_stereo = TRUE;

            Uint32 woff = 0;

            short *eSamples = RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split];
            short *erSamples = RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split];

            while(woff < SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split])
            {
                if(t_stereo) RF.WriteStereoSample(*eSamples++, *erSamples++);
                else RF.WriteMonoSample(*eSamples++);
                woff++;
            }

            // Write the looping info
            if(LoopType[ptk->Current_Instrument][ptk->Current_Instrument_Split])
            {
                RiffChunkHeader header;
                WaveSmpl_ChunkData datas;

                header.ckID = FourCC("smpl");
                header.ckSize = 0x3c;

                memset(&datas, 0, sizeof(datas));
                datas.Num_Sample_Loops = 1;
                datas.Start = LoopStart[ptk->Current_Instrument][ptk->Current_Instrument_Split];
                datas.End = LoopEnd[ptk->Current_Instrument][ptk->Current_Instrument_Split];

                header.ckSize = Swap_32(header.ckSize);

                datas.Num_Sample_Loops = Swap_32(datas.Num_Sample_Loops);
                datas.Start = Swap_32(datas.Start);
                datas.End = Swap_32(datas.End);

                RF.WriteData((void *) &header, sizeof(header));
                RF.WriteData((void *) &datas, sizeof(datas));
            }

            RF.Close();
            if(strlen(SampleName[ptk->Current_Instrument][ptk->Current_Instrument_Split])) sprintf(buffer, "File '%s' saved.", SampleName[ptk->Current_Instrument][ptk->Current_Instrument_Split]);
            else sprintf(buffer, "File 'Untitled.wav' saved.");
            Status_Box(ptk, buffer);

            Read_SMPT(ptk);
            ptk->last_index = -1;
            Actualize_Files_List(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_SAVE_INSTRUMENT)
        {
            if(ptk->snamesel == INPUT_NONE) SaveInst(ptk);
        }

        if(ptk->gui_action == GUI_CMD_MODULE_INFOS)
        {
            ShowInfo(ptk);
        }

        if(ptk->gui_action == GUI_CMD_REFRESH_TB303_PARAMS)
        {
            Actualize_303_Ed(ptk, ptk->teac);
        }

        if(ptk->gui_action == GUI_CMD_REFRESH_TB303_PARAMS_EXTERNAL)
        {
            Refresh_303_Unit(ptk, Refresh_Unit, ptk->teac);
        }

        if(ptk->gui_action == GUI_CMD_SAVE_303_PATTERN)
        {
            if(ptk->snamesel == INPUT_NONE) Save303(ptk);
        }

        if(ptk->gui_action == GUI_CMD_SAVE_REVERB)
        {
            if(ptk->snamesel == INPUT_NONE) SaveReverb(ptk);
        }

        if(ptk->gui_action == GUI_CMD_SAVE_MIDICFG)
        {
            if(ptk->snamesel == INPUT_NONE) SaveMidiCfg(ptk);
        }

        if(ptk->gui_action == GUI_CMD_SAVE_PATTERN)
        {
            if(ptk->snamesel == INPUT_NONE) SavePattern(ptk);
        }

        if(ptk->gui_action == GUI_CMD_FILELIST_SCROLL)
        {
            Actualize_Files_List(ptk, 1);
        }

        if(ptk->gui_action == GUI_CMD_INSTR_SYNTH_SCROLL)
        {
            Actualize_Instruments_Synths_List(ptk, 1);
        }

        if(ptk->gui_action == GUI_CMD_UPDATE_LOOP_EDITOR_ED)
        {
            // 1 = loop end
            Actualize_Instrument_Ed(ptk, 1, ptk->teac);
        }

        if(ptk->gui_action == GUI_CMD_SAVE_MODULE)
        {
            if(ptk->snamesel == INPUT_NONE) Pack_Module(ptk, ptk->name);
        }

        // Save a .ptp module
        if(ptk->gui_action == GUI_CMD_SAVE_FINAL)
        {
            if(ptk->snamesel == INPUT_NONE) SavePtk(ptk, ptk->name, TRUE, SAVE_WRITE, NULL);
        }

        // Calculate the approximate size of a .ptp module
        if(ptk->gui_action == GUI_CMD_CALC_FINAL)
        {
            Final_Mod_Length = TestMod(ptk);
            outlong(254, (Cur_Height - 94), Final_Mod_Length, 7);
        }

        if(ptk->gui_action == GUI_CMD_SET_INSTRUMENT_AMPLI)
        {
            Sample_Amplify[ptk->Current_Instrument][ptk->Current_Instrument_Split] = float((Mouse.x - 436) / 32.0f);
            Actualize_Instrument_Ed(ptk, 0, 1);
        }

        if(ptk->gui_action == GUI_CMD_SET_INSTRUMENT_FINETUNE)
        {
            FineTune_Value = ((Mouse.x - 436) - 64) * 2;
            if(FineTune_Value > 127) FineTune_Value = 127;
            if(FineTune_Value < -127) FineTune_Value = -127;
            Finetune[ptk->Current_Instrument][ptk->Current_Instrument_Split] = FineTune_Value;
            Actualize_Instrument_Ed(ptk, 0, 2);
        }

        if(ptk->gui_action == GUI_CMD_SET_INSTRUMENT_DECAY)
        {
            FDecay[ptk->Current_Instrument][ptk->Current_Instrument_Split] = float(Mouse.x - 62) / 8192.0f;
            Actualize_Instrument_Ed(ptk, 0, 3);
        }

        if(ptk->gui_action == GUI_CMD_UPDATE_INSTRUMENT_ED)
        {
            // 0 loop start
            Actualize_Instrument_Ed(ptk, 0, ptk->teac);
        }

        if(ptk->gui_action == GUI_CMD_UPDATE_DISKIO_ED)
        {
            Actualize_DiskIO_Ed(ptk, ptk->teac);
        }

        if(ptk->gui_action == GUI_CMD_UPDATE_REVERB_ED)
        {
            Actualize_Reverb_Ed(ptk, ptk->teac);
        }

        if(ptk->gui_action == GUI_CMD_UPDATE_MIDI_ED)
        {
            //Actualize_Midi_Ed(ptk, ptk->teac);
        }

        if(ptk->gui_action == GUI_CMD_NEW_MODULE)
        {
            Newmod(ptk);
        }

        if(ptk->gui_action == GUI_CMD_UPDATE_SEQ_ED)
        {
            Actualize_Seq_Ed(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_UPDATE_FX_ED)
        {
            Actualize_Fx_Ed(ptk, ptk->teac);
        }

        if(ptk->gui_action == GUI_CMD_UPDATE_SETUP_ED)
        {
            Actualize_Master_Ed(ptk, ptk->teac);
        }

        if(ptk->gui_action == GUI_CMD_UPDATE_SYNTH_ED)
        {
            Actualize_Synth_Ed(ptk, ptk->teac);
        }

        if(ptk->gui_action == GUI_CMD_MIDI_NOTE_OFF_1_TRACK)
        {
            Status_Box(ptk, "Notes Off command sent to this track...");
        }

        if(ptk->gui_action == GUI_CMD_MIDI_NOTE_OFF_ALL_TRACKS)
        {
            Status_Box(ptk, "Notes Off command sent to all tracks...");
        }

        if(ptk->gui_action == GUI_CMD_UPDATE_TRACK_FX_ED)
        {
            Actualize_Track_Fx_Ed(ptk, ptk->teac);
        }

        if(ptk->gui_action == GUI_CMD_UPDATE_MIDI_303_ED)
        {
            Actualize_303_Ed(ptk, ptk->teac);
        }

        if(ptk->gui_action == GUI_CMD_PREVIOUS_16_INSTR)
        {
            ptk->Current_Instrument -= 16;
            Actualize_Patterned(ptk);
            RefreshSample(ptk);
            Renew_Sample_Ed(ptk);
            Actualize_Synth_Ed(ptk, UPDATE_SYNTH_ED_ALL);
        }

        if(ptk->gui_action == GUI_CMD_NEXT_16_INSTR)
        {
            ptk->Current_Instrument += 16;
            Actualize_Patterned(ptk);
            RefreshSample(ptk);
            Renew_Sample_Ed(ptk);
            Actualize_Synth_Ed(ptk, UPDATE_SYNTH_ED_ALL);
        }

        if(ptk->gui_action == GUI_CMD_UPDATE_PATTERN_ED)
        {
            Actualize_Patterned(ptk);
        }

        if(ptk->gui_action == GUI_CMD_REDUCE_POSITIONS_10)
        {
            Song_Position -= 10;
            Bound_Patt_Pos(ptk);
            Actualize_Sequencer(ptk);
            Actupated(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_INCREASE_POSITIONS_10)
        {
            Song_Position += 10;
            Bound_Patt_Pos(ptk);
            Actualize_Sequencer(ptk);
            Actupated(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_REDUCE_PATTERNS_10)
        {
            if(ptk->pSequence[Song_Position] > 9) ptk->pSequence[Song_Position] -= 10;
            else ptk->pSequence[Song_Position] = 0;
            Bound_Patt_Pos(ptk);
            Anat(ptk, Song_Position);
            Actualize_Sequencer(ptk);
            Actupated(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_INCREASE_PATTERNS_10)
        {
            if(ptk->pSequence[Song_Position] < 118) ptk->pSequence[Song_Position] += 10;
            else ptk->pSequence[Song_Position] = 127;
            Bound_Patt_Pos(ptk);
            Anat(ptk, Song_Position);
            Actualize_Sequencer(ptk);
            Actupated(ptk, 0);
        }

        if(ptk->gui_action == GUI_CMD_RENDER_WAV)
        {
            if(ptk->snamesel == INPUT_NONE) WavRenderizer(ptk);
        }

        if(ptk->gui_action == GUI_CMD_TIMED_REFRESH_SEQUENCER)
        {
            Actualize_Sequencer(ptk);
            ptk->player_pos = Get_Song_Position(ptk);
        }

        if(ptk->gui_action == GUI_CMD_DELETE_INSTRUMENT)
        {
            Clear_Input(ptk);
            DeleteInstrument(ptk);
        }

        if(ptk->gui_action == GUI_CMD_SAVE_SYNTH)
        {
            if(ptk->snamesel == INPUT_NONE) SaveSynth(ptk);
        }

        if(ptk->gui_action == GUI_CMD_PATTERNS_POOL_EXHAUSTED)
        {
            Status_Box(ptk, "Maximum number of patterns reached.");
        }

        if(ptk->gui_action == GUI_CMD_REFRESH_SAMPLE_ED)
        {
            Actualize_Sample_Ed(ptk, ptk->teac);
        }

        if(ptk->gui_action == GUI_CMD_REFRESH_PALETTE)
        {
            ptk->Display_Pointer = TRUE;
        }

        if(ptk->gui_action == GUI_CMD_EXIT)
        {
            Display_Requester(ptk, &Exit_Requester, GUI_CMD_NOP);
        }

        ptk->gui_action = GUI_CMD_NOP;
    }

    // Draw the main windows layout
    if(ptk->redraw_everything)
    {
        SetColor(COL_BLACK);
        Fillrect(0, 0, ptk->CONSOLE_WIDTH, ptk->CONSOLE_HEIGHT);

        ptk->last_index = -1;
        Gui_Draw_Button_Box(MIN_VUMETER - 4, 6, (MAX_VUMETER - MIN_VUMETER) + 6, 16, "blah", BUTTON_NORMAL | BUTTON_DISABLED);

        Display_Master_Comp();
        Display_Master_Volume();
        Display_Shuffle();

        Draw_Scope_Files_Button(ptk);

        if(!ptk->Done_Tip)
        {
            Status_Box(ptk, ptk->tipoftheday);
            ptk->Done_Tip = TRUE;
        }
        else
        {
            Status_Box(ptk, "Feeling groovy.");
        }

        Gui_Draw_Button_Box(0, 6, 16, 16, "\011", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

        Gui_Draw_Button_Box(0, 180, ptk->fsize, 2, "", BUTTON_NORMAL | BUTTON_DISABLED);

        Gui_Draw_Button_Box(0, 24, 96, 78, "", BUTTON_NORMAL | BUTTON_DISABLED);
        
        Gui_Draw_Button_Box(8, 46, 80, 16, "\254", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
        Notify_Play(ptk);
        StartRec(ptk);
        Notify_Edit(ptk);

        Gui_Draw_Button_Box(98, 24, 156, 78, "", BUTTON_NORMAL | BUTTON_DISABLED);
        Gui_Draw_Button_Box(106, 28, 80, 16, "Position", BUTTON_NORMAL | BUTTON_DISABLED);
        Gui_Draw_Button_Box(106, 46, 80, 16, "Pattern", BUTTON_NORMAL | BUTTON_DISABLED);
        Gui_Draw_Button_Box(106, 64, 80, 16, "Song Length", BUTTON_NORMAL | BUTTON_DISABLED);
        Gui_Draw_Button_Box(106, 82, 80, 16, "Pattern Lines", BUTTON_NORMAL | BUTTON_DISABLED);

        Gui_Draw_Button_Box(256, 24, 136, 78, "", BUTTON_NORMAL | BUTTON_DISABLED);

        Gui_Draw_Button_Box(262, 28, 38, 16, "Tracks", BUTTON_NORMAL | BUTTON_DISABLED);
        Gui_Draw_Button_Box(302, 28, 9, 16, I_, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_SMALL_FONT);
        Gui_Draw_Button_Box(313, 28, 9, 16, D_, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_SMALL_FONT);
        Gui_Draw_Button_Box(262, 46, 60, 16, "Beats/Min.", BUTTON_NORMAL | BUTTON_DISABLED);
        Gui_Draw_Button_Box(262, 64, 60, 16, "Ticks/Beat", BUTTON_NORMAL | BUTTON_DISABLED);
        Display_Beat_Time();

        Gui_Draw_Button_Box(0, 104, 392, 42, "", BUTTON_NORMAL | BUTTON_DISABLED);
        Gui_Draw_Button_Box(8, 108, 80, 16, "Instrument", BUTTON_NORMAL | BUTTON_DISABLED);
        Gui_Draw_Button_Box(320, 108, 64, 16, "Delete", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
        Gui_Draw_Button_Box(8, 126, 80, 16, "Step Add", BUTTON_NORMAL | BUTTON_DISABLED);
        Gui_Draw_Button_Box(166, 126, 90, 16, "Keyboard Octave", BUTTON_NORMAL | BUTTON_DISABLED);

        // Tracks sizes
        Gui_Draw_Button_Box(332, 126, 16, 16, "\012", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
        Gui_Draw_Button_Box(332 + 18, 126, 16, 16, "\013", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
        Gui_Draw_Button_Box(332 + (18 * 2), 126, 16, 16, "\014", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

        Gui_Draw_Button_Box(0, 148, 392, 30, "", BUTTON_NORMAL | BUTTON_DISABLED);
        Gui_Draw_Button_Box(8, 152, 61, 10, S_ E_ L_ DOT_ SPC_ T_ R_ A_ C_ K_, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE | BUTTON_SMALL_FONT);
        Gui_Draw_Button_Box(8, 164, 61, 10, S_ E_ L_ DOT_ SPC_ N_ O_ T_ E_, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE | BUTTON_SMALL_FONT);

        Gui_Draw_Button_Box(8 + 63, 152, 61, 10, C_ U_ T_, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_SMALL_FONT);
        Gui_Draw_Button_Box(8 + 63, 164, 61, 10, C_ O_ P_ Y_, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_SMALL_FONT);

        Gui_Draw_Button_Box(8 + (63 * 2), 152, 61, 10, P_ A_ S_ T_ E_, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_SMALL_FONT);
        Gui_Draw_Button_Box(8 + (63 * 2), 164, 61, 10, D_ E_ L_ E_ T_ E_, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_SMALL_FONT);

        Gui_Draw_Button_Box(8 + (63 * 3), 152, 61, 10, S_ P_ R_ E_ A_ D_, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_SMALL_FONT);
        Gui_Draw_Button_Box(8 + (63 * 3), 164, 61, 10, R_ A_ N_ D_ O_ M_ I_ Z_ E_, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_SMALL_FONT);

        Gui_Draw_Button_Box(8 + (63 * 4), 152, 61, 10, S_ E_ M_ I_ TIR_ T_ O_ N_ E_ SPC_ U_ P_, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE | BUTTON_SMALL_FONT);
        Gui_Draw_Button_Box(8 + (63 * 4), 164, 61, 10, S_ E_ M_ I_ TIR_ T_ O_ N_ E_ SPC_ D_ N_, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE | BUTTON_SMALL_FONT);

        Gui_Draw_Button_Box(8 + (63 * 5), 152, 61, 10, O_ C_ T_ A_ V_ E_ SPC_ U_ P_, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE | BUTTON_SMALL_FONT);
        Gui_Draw_Button_Box(8 + (63 * 5), 164, 61, 10, O_ C_ T_ A_ V_ E_ SPC_ D_ N_, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE | BUTTON_SMALL_FONT);

        Refresh_UI_Context(ptk);

        Actualize_Files_List(ptk, 0);

        // Vu meters background
        SetColor(COL_BLACK);
        Fillrect(MIN_VUMETER - 1, 9, MAX_VUMETER, 20);
        SetColor(COL_BACKGROUND);
        Fillrect(MIN_VUMETER, 10, MAX_VUMETER - 1, 19);

        Draw_Pattern_Right_Stuff(ptk);
        Actupated(ptk, 0);
    }

    if(ptk->gui_thread_action)
    {
        ptk->gui_thread_action = FALSE;
        Actupated(ptk, 0);
    }
    if(ptk->gui_bpm_action)
    {
        ptk->gui_bpm_action = FALSE;
        Display_Beat_Time();
        Actualize_Master(ptk, 2);
        Display_Shuffle();
    }

    if(Songplaying && Pattern_Line_Visual != ptk->player_line)
    {
        if(!sr_isrecording)
        {
            Actualize_Track_Ed(ptk, 15);
            Actualize_Track_Fx_Ed(ptk, 11);
        }
        Actupated(ptk, 0);
        ptk->player_line = Pattern_Line_Visual;
    }

    // Checking for mouse and keyboard events ------------------------------------
    if(!In_Requester)
    {
        Mouse_Handler(ptk);
        Keyboard_Handler(ptk);
    }
    else
    {
        Mouse_Handler_Requester(ptk);
        Keyboard_Handler_Requester(ptk);
    }

    // Refresh the sequencer each time the song position is different
    if(Songplaying)
    {
        if(ptk->player_pos != Song_Position_Visual)
        {
            ptk->gui_action = GUI_CMD_TIMED_REFRESH_SEQUENCER;
        }
    }

    Check_Requester(ptk, &Title_Requester);

    if(Check_Requester(ptk, &Overwrite_Requester) == 2)
    {
        ptk->gui_action = GUI_CMD_NOP;
    }

    switch(Check_Requester(ptk, &Zzaapp_Requester))
    {
        case 1:
            // All
            ptk->ZzaappOMatic = ZZAAPP_ALL;
            break;

        case 2:
            // Patterns
            ptk->ZzaappOMatic = ZZAAPP_PATTERNS;
            break;

        case 3:
            // Instruments
            ptk->ZzaappOMatic = ZZAAPP_INSTRUMENTS;
            break;

        case 4:
            // Synths
            ptk->ZzaappOMatic = ZZAAPP_SYNTHS;
            break;

        case 5:
            // 303
            ptk->ZzaappOMatic = ZZAAPP_303;
            break;

        case 6:
            // Cancel
            ptk->gui_action = GUI_CMD_NOP;
            break;

    }

    switch(Check_Requester(ptk, &Delete_Requester))
    {
        case 1:
            // All
            ptk->ZzaappOMatic = ZZAAPP_ALL;
            break;

        case 2:
            // Instrument
            ptk->ZzaappOMatic = ZZAAPP_INSTRUMENTS;
            break;

        case 3:
            // Split
            ptk->ZzaappOMatic = ZZAAPP_SPLIT;
            break;

        case 4:
            // Synth
            ptk->ZzaappOMatic = ZZAAPP_SYNTHS;
            break;

        case 5:
            ptk->gui_action = GUI_CMD_NOP;
            break;

    }

    if(Check_Requester(ptk, &Exit_Requester) == 1)
    {
        SongStop(ptk);
        Status_Box(ptk, "Seppuku...");
        return(FALSE);
    }

    return(TRUE);
}

// ------------------------------------------------------
// Attempt to load any supported file format
void LoadFile(ptk_data *ptk, int Freeindex, const char *str)
{
    long Datalen = 0;
    int ld0 = 0;
    int ld1 = 0;
    int ld2 = 0;
    int ld3 = 0;
    int Freeindex2 = 0;
    FILE *in;
    WaveFile Wav_File;
    AIFFFile AIFF_File;
    float save_sample_vol;

    int rate = 0;
    int bits = 0;
    short *csamples, *csamples2;
    char st_type = 0;
    short inx = 0;
    int fmtchklen = 0;
    int digibooster = FALSE;

    const char *FileName = str;

    if(FileName != NULL && (in = fopen(FileName, "rb")) != NULL) {
        char extension[10];
        int extension_AIFF[3];
        char extension_digi[21];
        unsigned extension_New;
        int modext = 0;
        char found_mod;
        int i;
        //char rebext[5];

        fseek(in, 1080, SEEK_SET);
        fread(&modext, sizeof(char), 4, in);

        fseek(in, 0, SEEK_SET);
        fread(extension, sizeof(char), 9, in);

        fseek(in, 0, SEEK_SET);
        fread(&extension_New, sizeof(char), 4, in);

        fseek(in, 0, SEEK_SET);
        fread(&extension_AIFF, sizeof(char), 12, in);

        memset(extension_digi, 0, sizeof(extension_digi));
        fseek(in, 0, SEEK_SET);
        fread(&extension_digi, sizeof(char), 20, in);

        // Look for sound/noise/star/protracker module
        found_mod = 0;
        for(i = 0; i < sizeof(mt_tags) / sizeof(int); i++) {
            if(Swap_32(mt_tags[i]) == modext) {
                found_mod = mt_channels[i];
                break;
            }
        }
        
        // Look for a digibooster v1.x module
        if(strcmp(extension_digi, "DIGI Booster module") == 0) {
            digibooster = TRUE;
            // Retrieve the number of channels
            fseek(in, 25, SEEK_SET);
            fread(&found_mod, sizeof(char), 1, in);
        }

        if(found_mod) {
            sprintf(ptk->name, "%s", FileName);
            // name / number of channels
            SongStop(ptk);
            AUDIO_Stop();
            LoadAmigaMod(ptk, ptk->name, FileName, found_mod, digibooster);
            Renew_Sample_Ed(ptk);
            fclose(in);
            ptk->gui_action = GUI_CMD_NONE;
            Actualize_DiskIO_Ed(ptk, 0);
            AUDIO_Play();
            return;
        }

        //fseek(in, 8, SEEK_SET);
        //fread(rebext, sizeof(char), 4, in);

        if(strcmp(extension, "TWNNINS0") == 0 ||
           strcmp(extension, "TWNNINS1") == 0 ||
           strcmp(extension, "TWNNINS2") == 0 ||
           strcmp(extension, "TWNNINS3") == 0 ||
           strcmp(extension, "TWNNINS4") == 0 ||
           strcmp(extension, "TWNNINS5") == 0 ||
           strcmp(extension, "TWNNINS6") == 0 ||
           strcmp(extension, "TWNNINS7") == 0 ||
           strcmp(extension, "TWNNINS8") == 0 ||
           strcmp(extension, "TWNNINS9") == 0)
        {
            sprintf(ptk->instrname, "%s", FileName);
            Stop_Current_Instrument(ptk);
            LoadInst(ptk, ptk->instrname);
            Renew_Sample_Ed(ptk);
        }
        else if(strcmp(extension, "TWNNSNG1") == 0 ||   // For salvage purpose
                strcmp(extension, "TWNNSNG2") == 0 ||
                strcmp(extension, "TWNNSNG3") == 0 ||
                strcmp(extension, "TWNNSNG4") == 0 ||
                strcmp(extension, "TWNNSNG5") == 0 ||
                strcmp(extension, "TWNNSNG6") == 0 ||
                strcmp(extension, "TWNNSNG7") == 0 ||
                strcmp(extension, "TWNNSNG8") == 0 ||
                strcmp(extension, "TWNNSNG9") == 0 ||
                strcmp(extension, "TWNNSNGA") == 0 ||
                strcmp(extension, "TWNNSNGB") == 0 ||
                strcmp(extension, "TWNNSNGC") == 0 ||
                strcmp(extension, "TWNNSNGD") == 0 ||
                strcmp(extension, "TWNNSNGE") == 0 ||
                strcmp(extension, "TWNNSNGF") == 0 ||
                strcmp(extension, "TWNNSNGG") == 0 ||
                strcmp(extension, "TWNNSNGH") == 0 ||
                strcmp(extension, "TWNNSNGI") == 0 ||
                strcmp(extension, "TWNNSNGJ") == 0 ||
                strcmp(extension, "PROTREKK") == 0 ||
                strcmp(extension, "PROTREKL") == 0 ||
                strcmp(extension, "PROTREKM") == 0 ||
                strcmp(extension, "PROTREKN") == 0 ||
                strcmp(extension, "PROTREKO") == 0)
        {
            sprintf(ptk->name, "%s", FileName);
            SongStop(ptk);
            AUDIO_Stop();
            LoadPtk(ptk, ptk->name);
            Renew_Sample_Ed(ptk);
            AUDIO_Play();
        }
        else if(strcmp(extension, "TWNNSYN0") == 0 ||
                strcmp(extension, "TWNNSYN1") == 0 ||
                strcmp(extension, "TWNNSYN2") == 0 ||
                strcmp(extension, "TWNNSYN3") == 0 ||
                strcmp(extension, "TWNNSYN4") == 0)
        {
            sprintf(ptk->synthname, "%s", FileName);
            Stop_Current_Instrument(ptk);
            LoadSynth(ptk, ptk->synthname);
        }
        else if(strcmp(extension, "TWNN3030") == 0 ||
                strcmp(extension, "TWNN3031") == 0)
        {
            sprintf(ptk->name303, "%s", FileName);
            Load303(ptk, ptk->name303);
        }
        else if(strcmp(extension, "TWNNREV1") == 0)
        {
            sprintf(ptk->namerev, "%s", FileName);
            LoadReverb(ptk, ptk->namerev);
        }
        else if(strcmp(extension, "PROTMID1") == 0)
        {
            sprintf(ptk->namemidicfg, "%s", FileName);
            LoadMidiCfg(ptk, ptk->namemidicfg);
        }
        else if((strcmp(extension, "TWNNBLK1") == 0) ||
                (strcmp(extension, "PROTBLK2") == 0)
               )
        {
            sprintf(ptk->namerev, "%s", FileName);
            LoadPattern(ptk, ptk->namerev);
        }
        else
        {
            if((extension_AIFF[0] == FormID &&
               extension_AIFF[2] == AiffID) ||
               (extension_AIFF[0] == FormID &&
               extension_AIFF[2] == AifcID))
            {
                Status_Box(ptk, "Attempting of loading an Audio IFF file...");
                if(AIFF_File.Open(FileName))
                {
                    int bits = AIFF_File.BitsPerSample();
                    int channels = AIFF_File.NumChannels();

                    if(channels != 1 && channels != 2)
                    {
                        Status_Box(ptk, "Protrekkr can only load mono or stereo samples.");
                    }
                    else
                    {
                        if(bits != 8 &&
                           bits != 12 &&
                           bits != 16 &&
                           bits != 24 &&
                           bits != 32 &&
                           bits != 64)
                        {
                            Status_Box(ptk, "Protrekkr can only load 8, 12, 16, 24, 32 or 64 bits samples.");
                        }
                        else
                        {
                            save_sample_vol = 1.0f;
                            if(Get_Number_Of_Splits(Freeindex)) save_sample_vol = Sample_Vol[Freeindex];
                            sp_Position[Freeindex][ptk->Current_Instrument_Split].absolu = 0;
                            Stop_Current_Instrument(ptk);
                            switch(channels)
                            {
                                case 1:
                                    AllocateWave(Freeindex, ptk->Current_Instrument_Split, AIFF_File.NumSamples(), 1, TRUE, NULL, NULL);
                                    csamples = RawSamples[Freeindex][0][ptk->Current_Instrument_Split];
                                    for(i = 0; i < AIFF_File.NumSamples(); i++)
                                    {
                                        AIFF_File.ReadMonoSample(&csamples[i]);
                                    }
                                    break;

                                case 2:
                                    AllocateWave(Freeindex, ptk->Current_Instrument_Split, AIFF_File.NumSamples(), 2, TRUE, NULL, NULL);
                                    csamples = RawSamples[Freeindex][0][ptk->Current_Instrument_Split];
                                    csamples2 = RawSamples[Freeindex][1][ptk->Current_Instrument_Split];
                                    for(i = 0; i < AIFF_File.NumSamples(); i++)
                                    {
                                        AIFF_File.ReadStereoSample(&csamples[i], &csamples2[i]);
                                    }
                                    break;
                            }
                            LoopStart[Freeindex][ptk->Current_Instrument_Split] = 0;
                            LoopEnd[Freeindex][ptk->Current_Instrument_Split] = 0;
                            LoopType[Freeindex][ptk->Current_Instrument_Split] = SMP_LOOP_NONE;
                            Basenote[Freeindex][ptk->Current_Instrument_Split] = DEFAULT_BASE_NOTE;
                            if(Get_Number_Of_Splits(Freeindex) == 1)
                            {
                                Sample_Vol[Freeindex] = save_sample_vol;
                            }

                            if(AIFF_File.BaseNote())
                            {
                                Basenote[Freeindex][ptk->Current_Instrument_Split] = AIFF_File.BaseNote();
                            }

                            if(AIFF_File.LoopType())
                            {
                                LoopType[Freeindex][ptk->Current_Instrument_Split] = AIFF_File.LoopType();
                                LoopStart[Freeindex][ptk->Current_Instrument_Split] = AIFF_File.LoopStart();
                                LoopEnd[Freeindex][ptk->Current_Instrument_Split] = AIFF_File.LoopEnd();
                            }

                            sprintf(SampleName[Freeindex][ptk->Current_Instrument_Split], "%s", FileName);
                            Actualize_Patterned(ptk);
                            Actualize_Instrument_Ed(ptk, 2, 0);
                            Renew_Sample_Ed(ptk);
                            switch(bits)
                            {
                                case 64:
                                    Status_Box(ptk, "64 bit Audio IFF PCM converted into 16 bit format.");
                                    break;
                                case 32:
                                    Status_Box(ptk, "32 bit Audio IFF PCM converted into 16 bit format.");
                                    break;
                                case 24:
                                    Status_Box(ptk, "24 bit Audio IFF PCM converted into 16 bit format.");
                                    break;
                                case 12:
                                    Status_Box(ptk, "12 bit Audio IFF PCM converted into 16 bit format.");
                                    break;
                                case 8:
                                    Status_Box(ptk, "8 bit Audio IFF PCM converted into 16 bit format.");
                                    break;
                                default:
                                    Status_Box(ptk, "16 bit Audio IFF PCM loaded.");
                                    break;
                            }
                        }
                    }

                    AIFF_File.Close();
                }
                else
                {
                    Status_Box(ptk, "Corrupted or unsupported Audio IFF file.");
                }
            }
            else
            {

                Status_Box(ptk, "Attempting of loading a RIFF file...");

                // We need the length
                if(Wav_File.OpenForRead(FileName) == DDC_SUCCESS)
                {
                    int bits = Wav_File.BitsPerSample();
                    int channels = Wav_File.NumChannels();
                    if(channels != 1 && channels != 2)
                    {
                        Status_Box(ptk, "Protrekkr can only load mono or stereo samples.");
                    }
                    else
                    {
                        if(bits != 8 &&
                           bits != 12 &&
                           bits != 16 &&
                           bits != 24 &&
                           bits != 32 &&
                           bits != 64)
                        {
                            Status_Box(ptk, "Protrekkr can only load 8, 12, 16, 24, 32 or 64 bits samples.");
                        }
                        else
                        {
                            save_sample_vol = 1.0f;
                            if(Get_Number_Of_Splits(Freeindex)) save_sample_vol = Sample_Vol[Freeindex];
                            sp_Position[Freeindex][ptk->Current_Instrument_Split].absolu = 0;
                            Stop_Current_Instrument(ptk);
                            switch(channels)
                            {
                                case 1:
                                    AllocateWave(Freeindex, ptk->Current_Instrument_Split, Wav_File.NumSamples(), 1, TRUE, NULL, NULL);
                                    csamples = RawSamples[Freeindex][0][ptk->Current_Instrument_Split];
                                    for(i = 0; i < Wav_File.NumSamples(); i++)
                                    {
                                        Wav_File.ReadMonoSample(&csamples[i]);
                                    }
                                    break;

                                case 2:
                                    AllocateWave(Freeindex, ptk->Current_Instrument_Split, Wav_File.NumSamples(), 2, TRUE, NULL, NULL);
                                    csamples = RawSamples[Freeindex][0][ptk->Current_Instrument_Split];
                                    csamples2 = RawSamples[Freeindex][1][ptk->Current_Instrument_Split];
                                    for(i = 0; i < Wav_File.NumSamples(); i++)
                                    {
                                        Wav_File.ReadStereoSample(&csamples[i], &csamples2[i]);
                                    }
                                    break;
                            }
                            LoopStart[Freeindex][ptk->Current_Instrument_Split] = 0;
                            LoopEnd[Freeindex][ptk->Current_Instrument_Split] = 0;
                            LoopType[Freeindex][ptk->Current_Instrument_Split] = SMP_LOOP_NONE;
                            Basenote[Freeindex][ptk->Current_Instrument_Split] = DEFAULT_BASE_NOTE;
                            if(Get_Number_Of_Splits(Freeindex) == 1)
                            {
                                Sample_Vol[Freeindex] = save_sample_vol;
                            }

                            if(Wav_File.LoopType())
                            {
                                LoopType[Freeindex][ptk->Current_Instrument_Split] = Wav_File.LoopType();
                                LoopStart[Freeindex][ptk->Current_Instrument_Split] = Wav_File.LoopStart();
                                LoopEnd[Freeindex][ptk->Current_Instrument_Split] = Wav_File.LoopEnd();
                            }

                            sprintf(SampleName[Freeindex][ptk->Current_Instrument_Split], "%s", FileName);
                            Actualize_Patterned(ptk);
                            Actualize_Instrument_Ed(ptk, 2, 0);
                            Renew_Sample_Ed(ptk);
                            switch(bits)
                            {
                                case 64:
                                    Status_Box(ptk, "64 bit WAV PCM converted into 16 bit format.");
                                    break;
                                case 32:
                                    Status_Box(ptk, "32 bit WAV PCM converted into 16 bit format.");
                                    break;
                                case 24:
                                    Status_Box(ptk, "24 bit WAV PCM converted into 16 bit format.");
                                    break;
                                case 12:
                                    Status_Box(ptk, "12 bit WAV PCM converted into 16 bit format.");
                                    break;
                                case 8:
                                    Status_Box(ptk, "8 bit WAV PCM converted into 16 bit format.");
                                    break;
                                default:
                                    Status_Box(ptk, "16 bit WAV PCM loaded.");
                                    break;
                            }
                        }
                    }

                    Wav_File.Close();
                }
                else
                {
                    Status_Box(ptk, "Invalid file format. I only accept '.wav' '.aiff' '.aifc' '.ptk' '.pti' '.303' '.pts' '.ppb' '.prv' '.mod' or '.ft' files.");
                }
            }
        }
        fclose(in);
    }
    else
    {
        Status_Box(ptk, "File loading error. (Possible cause: file not found)");
    }
    ptk->gui_action = GUI_CMD_NONE;
    if(ptk->start_gui == TRUE) Actualize_DiskIO_Ed(ptk, 0);
}

// ------------------------------------------------------
// Function to search for a free wave on board
int GetFreeWave(void)
{
    int inico = 0;
    int exiter = 0;
    int valuer = -1;

    do
    {
        if(SampleType[inico] == 0)
        {
            exiter = 1;
            valuer = inico;
        }
        inico++;
    }
    while(exiter == 0);

    return valuer;
}

// ------------------------------------------------------
// Start replaying
void SongPlay(ptk_data *ptk)
{
    Ptk_Stop(ptk);
    ptk->liveparam = 0;
    ptk->livevalue = 0;
    ptk->player_pos = -1;

    ptk_lua_call_noargs(ptk, ptk->play_cb);
    Post_Song_Init(ptk);
    Ptk_Play(ptk);
	if(ptk->start_gui) Notify_Play(ptk);
}

// ------------------------------------------------------
// Display live recording status
// "startrec" got it ? haha :(
void StartRec(ptk_data *ptk)
{
    ptk->liveparam = 0;
    ptk->livevalue = 0;
    if(sr_isrecording) Gui_Draw_Button_Box(8, 64, 80, 16, "Live Rec: On", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
    else Gui_Draw_Button_Box(8, 64, 80, 16, "Live Rec: Off", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
}

// ------------------------------------------------------
// Display editing status
void Notify_Edit(ptk_data *ptk)
{
    if(is_editing && !is_recording)
    {
        Gui_Draw_Button_Box(8, 82, 62, 16, "Editing...", BUTTON_PUSHED | BUTTON_RIGHT_MOUSE | BUTTON_TEXT_CENTERED);
    }
    if(is_recording)
    {
        Gui_Draw_Button_Box(8, 82, 62, 16, "Recording...", BUTTON_PUSHED | BUTTON_RIGHT_MOUSE | BUTTON_TEXT_CENTERED);
    }
    if(!is_editing && !is_recording)
    {
        Gui_Draw_Button_Box(8, 82, 62, 16, "Edit/Rec.", BUTTON_NORMAL | BUTTON_RIGHT_MOUSE | BUTTON_TEXT_CENTERED);
    }
    Gui_Draw_Button_Box(72, 82, 16, 16, "", BUTTON_DISABLED);
    ptk->gui_action_metronome = GUI_CMD_FLASH_METRONOME_OFF;
}

// ------------------------------------------------------
// Switch the command that can't be modified during playing
void Switch_Cmd_Playing(ptk_data *ptk, int Enable)
{
	if(ptk->start_gui == TRUE) {
    Gui_Draw_Button_Box(302, 28, 9, 16, I_, BUTTON_NORMAL | (Enable ? 0 : BUTTON_DISABLED) | BUTTON_TEXT_CENTERED | BUTTON_SMALL_FONT);
    Gui_Draw_Button_Box(313, 28, 9, 16, D_, BUTTON_NORMAL | (Enable ? 0 : BUTTON_DISABLED) | BUTTON_TEXT_CENTERED | BUTTON_SMALL_FONT);
    Gui_Draw_Arrows_Number_Box(324, 46, BeatsPerMin, BUTTON_NORMAL | (Enable ? 0 : BUTTON_DISABLED) | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
    Gui_Draw_Arrows_Number_Box2(324, 64, TicksPerBeat, BUTTON_NORMAL | (Enable ? 0 : BUTTON_DISABLED) | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
	}
}
            
// ------------------------------------------------------
// Display playing status
void Notify_Play(ptk_data *ptk)
{
    if(Songplaying)
    {
        if(plx == 0)
        {
            Gui_Draw_Button_Box(49, 28, 39, 16, "\253", BUTTON_NORMAL | BUTTON_RIGHT_MOUSE | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(8, 28, 39, 16, "\04", BUTTON_PUSHED | BUTTON_RIGHT_MOUSE | BUTTON_TEXT_CENTERED);
            Switch_Cmd_Playing(ptk, FALSE);
            Status_Box(ptk, "Playing song...");
        }
        else
        {
            Gui_Draw_Button_Box(8, 28, 39, 16, "\04", BUTTON_NORMAL | BUTTON_RIGHT_MOUSE | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(49, 28, 39, 16, "\253", BUTTON_PUSHED | BUTTON_RIGHT_MOUSE | BUTTON_TEXT_CENTERED);
            Switch_Cmd_Playing(ptk, FALSE);
            Status_Box(ptk, "Playing pattern...");
        }
    }
    else
    {
        Gui_Draw_Button_Box(8, 28, 39, 16, "\04", BUTTON_NORMAL | BUTTON_RIGHT_MOUSE | BUTTON_TEXT_CENTERED);
        Gui_Draw_Button_Box(49, 28, 39, 16, "\253", BUTTON_NORMAL | BUTTON_RIGHT_MOUSE | BUTTON_TEXT_CENTERED);
    }
}

// ------------------------------------------------------
// Stop replaying
void SongStop(ptk_data *ptk)
{
    Ptk_Stop(ptk);
	if(ptk->start_gui == TRUE) {
		Gui_Draw_Button_Box(8, 28, 39, 16, "\04", BUTTON_NORMAL | BUTTON_RIGHT_MOUSE | BUTTON_TEXT_CENTERED);
		Gui_Draw_Button_Box(49, 28, 39, 16, "\253", BUTTON_NORMAL | BUTTON_RIGHT_MOUSE | BUTTON_TEXT_CENTERED);
	}
    Status_Box(ptk, "Feeling groovy.");
    // Make sure the visuals stay
    Song_Position = Song_Position_Visual;
    Pattern_Line = Pattern_Line_Visual;
    Actualize_Master(ptk, 5);
}

// ------------------------------------------------------
// Create a new song
void Newmod(ptk_data *ptk)
{
    int i;
    int Old_Prg;

    Clear_Input(ptk);
    SongStop(ptk);
    Stop_Current_Instrument(ptk);

#if !defined(__NO_MIDI__)
    Midi_Reset(ptk);
#endif

    if(ptk->ZzaappOMatic == ZZAAPP_ALL || ptk->ZzaappOMatic == ZZAAPP_INSTRUMENTS)
    {
        Free_Samples(ptk);
        ptk->Current_Instrument = 0;
        ptk->seditor = 0;
        Final_Mod_Length = 0;
        Actualize_Master(ptk, 0);
        for(i = 0; i < MAX_INSTRS; i++)
        {
            Old_Prg = Synthprg[i];
            KillInst(ptk, i, TRUE);
            Synthprg[i] = Old_Prg;
            sprintf(nameins[i], "Untitled");
            if((Synthprg[i] - 2) == i)
            {
                Synthprg[i] = 1;
            }
        }
        Renew_Sample_Ed(ptk);
    }

    if(ptk->ZzaappOMatic == ZZAAPP_ALL || ptk->ZzaappOMatic == ZZAAPP_SYNTHS)
    {
        for(int ini = 0; ini < MAX_TRACKS; ini++)
        {
            for(i = 0; i < MAX_POLYPHONY; i++)
            {
                //Synthesizer[ini][i].Reset();
                ptk_synth_reset(&Synthesizer[ini][i]);
            }
        }

        for(i = 0; i < MAX_INSTRS; i++)
        {
            ResetSynthParameters(ptk, &PARASynth[i]);
            Synthprg[i] = 0;
        }
        Final_Mod_Length = 0;
        Actualize_Master(ptk, 0);
        ptk->Current_Instrument = 0;
    }

    if(ptk->ZzaappOMatic == ZZAAPP_ALL || ptk->ZzaappOMatic == ZZAAPP_PATTERNS)
    {
        Songtracks = 6;
        for(int api = 0; api < MAX_ROWS; api++)
        {
            ptk->patternLines[api] = 64;
        }
        Clear_Patterns_Pool(ptk);
        ptk->nPatterns = 1;
        Set_Default_Channels_Polyphony();
        for(int inico = 0; inico < 256; inico++)
        {
            ptk->pSequence[inico] = 0;
            for(int inico2 = 0; inico2 < MAX_TRACKS; inico2++)
            {
                CHAN_ACTIVE_STATE[inico][inico2] = TRUE;
                CHAN_HISTORY_STATE[inico][inico2] = FALSE;
            }
        }

        ptk->Song_Length = 1;
        ptk->Track_Under_Caret = 0;
        ptk->Column_Under_Caret = 0;
        Pattern_Line = 0;
        Pattern_Line_Visual = 0;
        ptk->player_line = 0;
        Song_Position = 0;
        Song_Position_Visual = 0;
        rawrender_range = FALSE;
        rawrender_target = RENDER_TO_FILE;
        rawrender_from = 0;
        rawrender_to = 0;
        Final_Mod_Length = 0;
        Actualize_Sequencer(ptk);
        Actualize_Patterned(ptk);
        Actualize_Master(ptk, 0);
        Actupated(ptk, 0);
        Reset_Tracks_To_Render(ptk);
    }

    if(ptk->ZzaappOMatic == ZZAAPP_ALL || ptk->ZzaappOMatic == ZZAAPP_303)
    {
        Reset_303_Parameters(&tb303[0]);
        Reset_303_Parameters(&tb303[1]);
        tb303engine[0].reset();
        tb303engine[1].reset();
        track3031 = 255;
        track3032 = 255;
    }

    if(ptk->ZzaappOMatic == ZZAAPP_ALL)
    {
        Use_Cubic = CUBIC_INT;
        sprintf(Selection_Name, "Untitled");
        sprintf(Reverb_Name, "Untitled");
        sprintf(Midi_Name, "Untitled");
        sprintf(ptk->name, "Untitled");
        sprintf(ptk->artist, "Somebody");
        sprintf(ptk->style, "Anything Goes");
        ptk->namesize = 8;
        BeatsPerMin = 125;
        TicksPerBeat = 4;
        DelayType = 1;
        mas_vol = 1.0f;
        Reverb_Filter_Cutoff = 0.08f;
        Reverb_Filter_Resonance = 0.5f;
        Reverb_Stereo_Amount = 50;

        for(i = 0; i < MAX_TRACKS; i++)
        {
            mas_comp_threshold_Track[i] = 100.0f;
            mas_comp_ratio_Track[i] = 0.0f;
        }
        lchorus_feedback = 0.6f;
        rchorus_feedback = 0.5f;
        lchorus_delay = 10584;
        rchorus_delay = 15876;
        lchorus_counter = MIX_RATE;
        rchorus_counter = MIX_RATE;
        lchorus_counter2 = MIX_RATE - lchorus_delay;
        rchorus_counter2 = MIX_RATE - rchorus_delay;
        compressor = FALSE;
        c_threshold = 32;
        delay_time = 0;

        // Old preset by default
        Load_Old_Reverb_Presets(ptk, DelayType);
        Initreverb(ptk);
        for(int spl = 0; spl < MAX_TRACKS; spl++)
        {
            CCoef[spl] = float((float) CSend[spl] / 127.0f);
        }
        Pre_Song_Init(ptk);
    }

    Post_Song_Init(ptk);

    SubCounter = 0;
    PosInTick = 0;
    PosInTick_Delay = 0;

    allow_save = TRUE;

    Actualize_DiskIO_Ed(ptk, 0);

    Reset_Song_Length(ptk);
    Display_Song_Length(ptk);

    Draw_Scope(ptk);

    Status_Box(ptk, "Zzaapp done.");

    Refresh_UI_Context(ptk);
}

// ------------------------------------------------------
// ------------------------------------------------------
// ------------------------------------------------------

// ------------------------------------------------------
// Change a name
char *table_newletter[71] =
{
    NULL, // 0
    "%sa",
    "%sb",
    "%sc",
    "%sd",
    "%se",
    "%sf",
    "%sg",
    "%sh",
    "%si",
    "%sj", // 10
    "%sk",
    "%sl",
    "%sm",
    "%sn",
    "%so",
    "%sp",
    "%sq",
    "%sr",
    "%ss",
    "%st", // 20
    "%su",
    "%sv",
    "%sw",
    "%sx",
    "%sy",
    "%sz",
    "%s0",
    "%s1",
    "%s2",
    "%s3",
    "%s4",
    "%s5",
    "%s6",
    "%s7",
    "%s8",
    "%s9",
    NULL, // 37
    "%s ", // 38
    NULL, // 39
    "%s.", // 40
    "%sA",
    "%sB",
    "%sC",
    "%sD",
    "%sE",
    "%sF",
    "%sG",
    "%sH",
    "%sI",
    "%sJ", // 50
    "%sK",
    "%sL",
    "%sM",
    "%sN",
    "%sO",
    "%sP",
    "%sQ",
    "%sR",
    "%sS",
    "%sT", // 60
    "%sU",
    "%sV",
    "%sW",
    "%sX",
    "%sY",
    "%sZ",
    "%s,", // 67
    NULL,  // 68
    "%s-", // 69
    NULL   // 70
};

// ------------------------------------------------------
// Change a letter in a string
void Actualize_Name(ptk_data *ptk, int *newletter, char *nam)
{
    int i;

    // Cancel input
    if(newletter[71])
    {
        newletter[71] = FALSE;
        strcpy(nam, ptk->cur_input_name);
        ptk->snamesel = INPUT_NONE;
        Keyboard_Nbr_Events = 0;
        return;
    }
    // Valid input
    if(newletter[39] && ptk->namesize > 0)
    {
        newletter[39] = FALSE;
        ptk->snamesel = INPUT_NONE;
        Keyboard_Nbr_Events = 0;
        return;
    }

    for(i = 0; i < 71; i++)
    {
        if(i == 37)
        {
            // Backspace
            if(newletter[i])
            {
                if(ptk->namesize > 0)
                {
                    nam[strlen(nam) - 1] = '\0';
                    if(ptk->namesize > 0) ptk->namesize--;
                }
                newletter[i] = FALSE;
            }
        }
        else
        {
            if(newletter[i])
            {
                if(ptk->namesize < 19)
                {
                    if(table_newletter[i])
                    {
                        sprintf(nam, table_newletter[i], nam);
                        ptk->namesize++;
                    }
                }
                newletter[i] = FALSE;
            }
        }
    }
}

// ------------------------------------------------------
// Render the song as a .wav file (or collection of files).
int Must_Render_Track(int Idx)
{
    if(rawrender_multi &&
       rawrender_target == RENDER_TO_FILE)
    {
        if(Tracks_To_Render[Idx])
        {
            return FALSE;
        }
    }
    return TRUE;
}

void WavRenderizer(ptk_data *ptk)
{
    plx = 0;
    char buffer[MAX_PATH];
    char buffer_name[MAX_PATH];
    int Save_CHAN_MUTE_STATE[MAX_TRACKS];
    int i;
    int j;
    int max_tracks_to_render;
    int Max_Position;
    WaveFile RF[MAX_TRACKS];
    short *Sample_Buffer[2];
    int Pos_In_Memory;
    int Mem_Buffer_Size;
    int Stereo = 0;
    int do_multi;
    float save_sample_vol;
    long filesize = 0;
    int rendered_track = 0;

    if(rawrender_multi &&
       rawrender_target == RENDER_TO_FILE)
    {
        max_tracks_to_render = Songtracks;
        do_multi = TRUE;
        for(i = 0; i < MAX_TRACKS; i++)
        {
            Save_CHAN_MUTE_STATE[i] = CHAN_MUTE_STATE[i];
            CHAN_MUTE_STATE[i] = TRUE;
        }
    }
    else
    {
        max_tracks_to_render = 1;
        do_multi = FALSE;
        SongStop(ptk);
    }

    for(j = 0; j < max_tracks_to_render; j++)
    {
        if(Must_Render_Track(j))
        {
            rendered_track++;
            if(do_multi)
            {
                SongStop(ptk);
                sprintf(buffer_name, "%s_%x.wav", ptk->name, j);
            }
            else
            {
                sprintf(buffer_name, "%s.wav", ptk->name);
            }

            switch(rawrender_target)
            {
                case RENDER_TO_FILE:
                    if(RF[j].OpenForWrite(buffer_name, 44100, rawrender_32float ? 32 : 16, 2) != DDC_SUCCESS)
                    {
                        sprintf(buffer, "Can't open '%s' file.", buffer_name);
                        Status_Box(ptk, buffer);
                        return;
                    }
                    if(rawrender_range)
                    {
                        sprintf(buffer, "Rendering selection to '%s' file. Please wait...", buffer_name);
                    }
                    else
                    {
                        sprintf(buffer, "Rendering module to '%s' file. Please wait...", buffer_name);
                    }
                    break;

                case RENDER_TO_MONO:
                case RENDER_TO_STEREO:
                    if(rawrender_range)
                    {
                        sprintf(buffer, "Rendering selection to instrument %d (split %d). Please wait...",
                                ptk->Current_Instrument,
                                ptk->Current_Instrument_Split);
                    }
                    else
                    {
                        sprintf(buffer, "Rendering module to instrument %d (split %d). Please wait...",
                                ptk->Current_Instrument,
                                ptk->Current_Instrument_Split);
                    }
                    break;
            }
            Status_Box(ptk, buffer);
            SDL_UpdateRect(Main_Screen, 0, 0, 0, 0);
            SDL_Delay(10);

            rawrender = TRUE;

            Pattern_Line = 0;
            if(rawrender_range)
            {
                Song_Position = rawrender_from;
                Max_Position = rawrender_to + 1;
            }
            else
            {
                Song_Position = 0;
                Max_Position = ptk->Song_Length;
            }

            filesize = 0;
            //ptk->done = FALSE;
            done = FALSE;

            if(!do_multi)
            {
                for(i = 0; i < MAX_TRACKS; i++)
                {
                    Save_CHAN_MUTE_STATE[i] = CHAN_MUTE_STATE[i];
                    CHAN_MUTE_STATE[i] = Tracks_To_Render[i];
                }
            }
            else
            {
                CHAN_MUTE_STATE[j] = FALSE;
            }

            switch(rawrender_target)
            {
                case RENDER_TO_FILE:
                    SongPlay(ptk);
                    //while(Song_Position < Max_Position && ptk->done == FALSE)
                    while(Song_Position < Max_Position && done == FALSE)
                    {
                        GetPlayerValues(ptk);
                        if(rawrender_32float)
                        {
                            // [-1.0..1.0]
                            RF[j].WriteStereoFloatSample(ptk->left_float_render, ptk->right_float_render);
                            filesize += 8;
                        }
                        else
                        {
                            RF[j].WriteStereoSample(left_value, right_value);
                            filesize += 4;
                        }
                    }
                    RF[j].Close();
                    break;

                case RENDER_TO_STEREO:
                    Stereo = 1;
                case RENDER_TO_MONO:
                    Mem_Buffer_Size = AUDIO_Latency * 64;
                    Sample_Buffer[0] = (short *) malloc(Mem_Buffer_Size * 2);
                    memset(Sample_Buffer[0], 0, Mem_Buffer_Size * 2);
                    Sample_Buffer[1] = NULL;
                    if(Sample_Buffer[0])
                    {
                        if(Stereo)
                        {
                            Sample_Buffer[1] = (short *) malloc(Mem_Buffer_Size * 2);
                            if(!Sample_Buffer[1])
                            {
                                free(Sample_Buffer[0]);
                                goto Stop_WavRender;
                            }
                            memset(Sample_Buffer[1], 0, Mem_Buffer_Size * 2);
                        }
                        Pos_In_Memory = 0;
                        SongPlay(ptk);
                        //while(Song_Position < Max_Position && ptk->done == FALSE)
                        while(Song_Position < Max_Position && done == FALSE)
                        {
                            if(!Mem_Buffer_Size)
                            {
                                // Increase the size of our buffer
                                Mem_Buffer_Size = (AUDIO_Latency * 64);
                                Sample_Buffer[0] = (short *) realloc(Sample_Buffer[0], (Pos_In_Memory + Mem_Buffer_Size) * 2);
                                if(Stereo)
                                {
                                    Sample_Buffer[1] = (short *) realloc(Sample_Buffer[1], (Pos_In_Memory + Mem_Buffer_Size) * 2);
                                }
                            }
                            GetPlayerValues(ptk);
                            if(Stereo)
                            {
                                Sample_Buffer[0][Pos_In_Memory] = ptk->left_float_render * 32767.0f;
                                Sample_Buffer[1][Pos_In_Memory] = ptk->right_float_render * 32767.0f;
                                filesize += 4;
                            }
                            else
                            {
                                // Mix both channels
                                Sample_Buffer[0][Pos_In_Memory] = ((ptk->left_float_render * 0.5f) +
                                                                   (ptk->right_float_render * 0.5f)) * 32767.0f;
                                filesize += 2;
                            }
                            Pos_In_Memory++;
                            Mem_Buffer_Size--;
                        }
                        save_sample_vol = 1.0f;
                        if(Get_Number_Of_Splits(ptk->Current_Instrument)) save_sample_vol = Sample_Vol[ptk->Current_Instrument];

                        AllocateWave(ptk->Current_Instrument, ptk->Current_Instrument_Split,
                                     Pos_In_Memory, (Stereo + 1), TRUE,
                                     NULL, NULL);

                        // Fixup the buffer with it's real size
                        memcpy(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split], 
                               Sample_Buffer[0], (Pos_In_Memory * 2));
                        if(Stereo)
                        {
                            memcpy(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split], 
                                   Sample_Buffer[1], (Pos_In_Memory * 2));
                            free(Sample_Buffer[1]);
                        }
                        free(Sample_Buffer[0]);
                        LoopStart[ptk->Current_Instrument][ptk->Current_Instrument_Split] = 0;
                        LoopEnd[ptk->Current_Instrument][ptk->Current_Instrument_Split] = 0;
                        LoopType[ptk->Current_Instrument][ptk->Current_Instrument_Split] = SMP_LOOP_NONE;
                        Basenote[ptk->Current_Instrument][ptk->Current_Instrument_Split] = DEFAULT_BASE_NOTE;
                        Renew_Sample_Ed(ptk);
                        if(Get_Number_Of_Splits(ptk->Current_Instrument) == 1)
                        {
                            Sample_Vol[ptk->Current_Instrument] = save_sample_vol;
                        }
                    }
                    break;
            }
            if(do_multi)
            {
                CHAN_MUTE_STATE[j] = TRUE;
            }
        }
    }

Stop_WavRender:
    SongStop(ptk);
    Post_Song_Init(ptk);
    Songplaying = FALSE;

    if(!do_multi)
    {
        for(i = 0; i < MAX_TRACKS; i++)
        {
            CHAN_MUTE_STATE[i] = Save_CHAN_MUTE_STATE[i];
        }
    }
    else
    {
        for(i = 0; i < MAX_TRACKS; i++)
        {
            CHAN_MUTE_STATE[i] = Save_CHAN_MUTE_STATE[i];
        }
    }

    int minutes = filesize / 10584000;
    int seconds = (filesize - minutes * 10584000) / 176400;

    switch(rawrender_target)
    {
        case RENDER_TO_FILE:
            if(do_multi)
            {
                sprintf(buffer, "Wav rendering finished. File size: %.2f Megabytes per file (Total: %.2f Megabytes). Playback time: %d'%d''.",
                                float(filesize / 1048576.0f), float(filesize / 1048576.0f) * rendered_track, minutes, seconds);
            }
            else
            {
                sprintf(buffer, "Wav rendering finished. File size: %.2f Megabytes. Playback time: %d'%d''.",
                                float(filesize / 1048576.0f), minutes, seconds);
            }
            break;
        default:
            sprintf(buffer, "Wav rendering finished. Waveform size: %.2f Megabytes. Playback time: %d'%d''.",
                            float(filesize / 1048576.0f), minutes, seconds);
    }
    //Status_Box(ptk, buffer);

    // Return to the start as all the values will be trashed anyway.
    Pattern_Line = 0;
    Song_Position = 0;
    Song_Position_Visual = 0;
    Pattern_Line_Visual = 0;
	if(ptk->start_gui == TRUE) Actualize_DiskIO_Ed(ptk, 0);
    rawrender = FALSE;

    ptk->last_index = -1;
    Read_SMPT(ptk);
    if(ptk->start_gui == TRUE) Actualize_Files_List(ptk, 0);

    Status_Box(ptk, buffer);
    if(ptk->start_gui == TRUE) Actupated(ptk, 0);
}

// ------------------------------------------------------
// Delete an instrument
void DeleteInstrument(ptk_data *ptk)
{
    int i;
    int Old_Prg;

    Stop_Current_Instrument(ptk);

    if(ptk->ZzaappOMatic == ZZAAPP_ALL || ptk->ZzaappOMatic == ZZAAPP_SYNTHS)
    {
        for(int ini = 0; ini < MAX_TRACKS; ini++)
        {
            for(i = 0; i < MAX_POLYPHONY; i++)
            {
                //Synthesizer[ini][i].Reset();
                ptk_synth_reset(&Synthesizer[ini][i]);
            }
        }

        ResetSynthParameters(ptk, &PARASynth[ptk->Current_Instrument]);
        Synthprg[ptk->Current_Instrument] = 0;
        Actualize_Master(ptk, 0);
        Final_Mod_Length = 0;
        Actualize_Synth_Ed(ptk, UPDATE_SYNTH_ED_ALL);
        Status_Box(ptk, "Synth deleted.");
    }

    if(ptk->ZzaappOMatic == ZZAAPP_SPLIT)
    {
        ptk->seditor = 0;
        Final_Mod_Length = 0;
        Actualize_Master(ptk, 0);
        Old_Prg = Synthprg[ptk->Current_Instrument];
        KillInst(ptk, ptk->Current_Instrument, FALSE);
        Synthprg[ptk->Current_Instrument] = Old_Prg;
        sprintf(nameins[ptk->Current_Instrument], "Untitled");
        if((Synthprg[ptk->Current_Instrument] - 2) == ptk->Current_Instrument)
        {
            Synthprg[ptk->Current_Instrument] = 1;
        }
        Renew_Sample_Ed(ptk);
        Status_Box(ptk, "Instrument deleted.");
        RefreshSample(ptk);
        Actualize_Master(ptk, 0);
    }

    if(ptk->ZzaappOMatic == ZZAAPP_ALL || ptk->ZzaappOMatic == ZZAAPP_INSTRUMENTS)
    {
        ptk->seditor = 0;
        Final_Mod_Length = 0;
        Actualize_Master(ptk, 0);
        Old_Prg = Synthprg[ptk->Current_Instrument];
        KillInst(ptk, ptk->Current_Instrument, TRUE);
        Synthprg[ptk->Current_Instrument] = Old_Prg;
        sprintf(nameins[ptk->Current_Instrument], "Untitled");
        if((Synthprg[ptk->Current_Instrument] - 2) == ptk->Current_Instrument)
        {
            Synthprg[ptk->Current_Instrument] = 1;
        }
        Renew_Sample_Ed(ptk);
        Status_Box(ptk, "Instrument deleted.");
        RefreshSample(ptk);
        Actualize_Master(ptk, 0);
    }
    Actualize_Instruments_Synths_List(ptk, 0);
}

void Stop_Current_Instrument(ptk_data *ptk)
{
    int i;

    for(char u = 0; u < MAX_TRACKS; u++)
    {
        for(i = 0; i < MAX_POLYPHONY; i++)
        {
            if(sp_channelsample[u][i] == ptk->Current_Instrument)
            {
                if(sp_Stage[u][i] == PLAYING_SAMPLE)
                {
                    sp_Stage[u][i] = PLAYING_NOSAMPLE;
                    Player_WL[u][i] = NULL;
                    Player_WR[u][i] = NULL;
                }
            }
        }
        Player_FD[u] = 0;
        ramper[u] = 0;
    }
}

// ------------------------------------------------------
// Refresh the view of a sample
void RefreshSample(ptk_data *ptk)
{
    ptk->seditor = 0;
    ptk->Current_Instrument_Split = 0;
    if(ptk->userscreen == USER_SCREEN_INSTRUMENT_EDIT)
    {
        Draw_Instrument_Ed(ptk);
        Actualize_Instrument_Ed(ptk, 2, 0);
    }
}

// ------------------------------------------------------
// Reset the channels polyphonies  to their default state
void Set_Default_Channels_Polyphony(void)
{
    int i;

    for(i = 0; i < MAX_TRACKS; i++)
    {
        Channels_Polyphony[i] = DEFAULT_POLYPHONY;
    }
}

// ------------------------------------------------------
// Show song informations (summing instruments and patterns)
void ShowInfo(ptk_data *ptk)
{
    char tmp[256];
    int pattsize = ptk->nPatterns * PATTERN_LEN;
    int sampsize = 0;
    int nbr_samp = 0;
    int nbr_synth = 0;
    int i;

    for(i = 0; i < MAX_INSTRS; i++)
    {
        if(Synthprg[i])
        {
            nbr_synth++;
        }
    }

    for(int pp = 0; pp < MAX_INSTRS; pp++)
    {
        for(int z = 0; z < MAX_INSTRS_SPLITS; z++)
        {
            if(SampleType[pp][z] != 0)
            {
                sampsize += SampleChannels[pp][z] * SampleLength[pp][z];
                nbr_samp++;
            }
        }
    }
    sprintf(tmp, "%d instruments (%d bytes), "
                 "%d synths, "
                 "%d patterns (%d bytes).",
                 nbr_samp, sampsize,
                 nbr_synth, 
                 ptk->nPatterns, pattsize);
    Status_Box(ptk, tmp);
}

// ------------------------------------------------------
// Function called each second mainly for autosaving purposes
Uint32 Timer_CallBack(Uint32 interval, void *param)
{

    ptk_data *ptk = (ptk_data *)param;
    if(Req_TimeOut)
    {
        Req_Timer++;
        if(Req_Timer > Req_TimeOut) Req_Pressed_Button = 1;
    }

    // Don't save during a requester
    if(ptk->AutoSave && Current_Requester == NULL)
    {
        ptk->wait_AutoSave++;
        // Autosave module
        if(ptk->wait_AutoSave > Values_AutoSave[ptk->AutoSave])
        {
            Pack_Module(ptk, ptk->name);
        }
    }

    return(interval);
}

// ------------------------------------------------------
// Validate or cancel an user input
void Actualize_Input(ptk_data *ptk)
{
    switch(ptk->snamesel)
    {
        // Module name
        case INPUT_MODULE_NAME:
            Actualize_Name(ptk, ptk->retletter, ptk->name);
            ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
            break;

        // Instrument name
        case INPUT_INSTRUMENT_NAME:
            Actualize_Name(ptk, ptk->retletter, nameins[ptk->Current_Instrument]);
            ptk->gui_action = GUI_CMD_UPDATE_PATTERN_ED;
            break;

        // Synth name
        case INPUT_SYNTH_NAME:
            Actualize_Name(ptk, ptk->retletter, PARASynth[ptk->Current_Instrument].presetname);
            ptk->teac = UPDATE_SYNTH_CHANGE_NAME;
            ptk->gui_action = GUI_CMD_UPDATE_SYNTH_ED;
            break;

        // Module ptk->artist
        case INPUT_MODULE_ARTIST:
            Actualize_Name(ptk, ptk->retletter, ptk->artist);
            ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
            break;

        // Module ptk->style
        case INPUT_MODULE_STYLE:
            Actualize_Name(ptk, ptk->retletter, ptk->style);
            ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
            break;

        // 303 pattern
        case INPUT_303_PATTERN:
            Actualize_Name(ptk, ptk->retletter, tb303[ptk->sl3].pattern_name[tb303[ptk->sl3].selectedpattern]);
            ptk->teac = 18;
            ptk->gui_action = GUI_CMD_UPDATE_MIDI_303_ED;
            break;

        // Reverb
        case INPUT_REVERB_NAME:
            Actualize_Name(ptk, ptk->retletter, Reverb_Name);
            ptk->teac = UPDATE_REVERB_ED_CHANGE_NAME;
            ptk->gui_action = GUI_CMD_UPDATE_REVERB_ED;
            break;

        // Selection
        case INPUT_SELECTION_NAME:
            Actualize_Name(ptk, ptk->retletter, Selection_Name);
            ptk->teac = 3;
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
            break;

        // Reverb
        case INPUT_MIDI_NAME:
            Actualize_Name(ptk, ptk->retletter, Midi_Name);
            ptk->teac = UPDATE_MIDI_ED_CHANGE_NAME;
            ptk->gui_action = GUI_CMD_UPDATE_MIDI_ED;
            break;

    }
}

// ------------------------------------------------------
// Return the channel and sub channel of a played note
LPJAZZ_KEY Get_Jazz_Key_Off(JAZZ_KEY Pool[MAX_TRACKS][MAX_POLYPHONY], int Note)
{
    int i;
    int j;

    for(i = 0; i < MAX_TRACKS; i++)
    {
        for(j = 0; j < MAX_POLYPHONY; j++)
        {
            if(Pool[i][j].Note == Note)
            {
                return(&Pool[i][j]);
            }
        }
    }
    return(NULL);
}

// ------------------------------------------------------
// Return the channel and sub channel of a played note
int Discard_Key_Note_Off(JAZZ_KEY Pool[MAX_TRACKS][MAX_POLYPHONY], int Channel, int Sub_Channel)
{
    int i;
    int j;
    int discarded = 0;

    for(i = 0; i < MAX_TRACKS; i++)
    {
        for(j = 0; j < MAX_POLYPHONY; j++)
        {
            if(Pool[i][j].Channel == Channel && Pool[i][j].Sub_Channel == Sub_Channel)
            {
                Pool[i][j].Sub_Channel = 0;
                Pool[i][j].Channel = 0;
                Pool[i][j].Note = 0;
                discarded++;
            }
        }
    }
    return(discarded);
}

int Raw_Keys_to_Send[] =
{
    0x10,
    3,
    0x11,
    4,
    0x12,
    0x13,
    6,
    0x14,
    7,
    0x15,
    8,
    0x16,
    0x17,
    0xa,
    0x18,
    0xb,
    0x19,
    0x1a,
    0xd,
    0x1b,
    0x2c,
    0x1f,
    0x2d,
    0x20,
    0x2e,
    0x2f,
    0x22,
    0x30,
    0x23,
    0x31,
    0x24,
    0x32,
    0x33,
    0x26,
    0x34,
    0x27,
    0x35
};

// ------------------------------------------------------
// KeyBoard ShortCut Handler
void Keyboard_Handler(ptk_data *ptk)
{
    int Cur_Position = Get_Song_Position(ptk);
    int Done_Value;

    // Exit tracker
    if(Get_LAlt() && Keys[SDLK_F4]) ptk->gui_action = GUI_CMD_EXIT;
    // Lower octave
    if(!Get_LShift() && Keys[SDLK_F1]) ptk->gui_action = GUI_CMD_LOWER_OCTAVE;
    // Higher octave
    if(!Get_LShift() && Keys[SDLK_F2]) ptk->gui_action = GUI_CMD_HIGHER_OCTAVE;

    if(Get_LShift())
    {
        if(Keys[SDLK_ESCAPE])
        {
            ptk->gui_action = GUI_CMD_SELECT_LARGE_PATTERNS;
        }
        if(Keys[SDLK_F1])
        {
            ptk->gui_action = GUI_CMD_SELECT_SEQUENCER;
        }
        if(Keys[SDLK_F2])
        {
            ptk->gui_action = GUI_CMD_SELECT_INSTRUMENT_EDIT;
        }
        if(Keys[SDLK_F3])
        {
            ptk->gui_action = GUI_CMD_SELECT_SYNTH_EDIT;
        }
        if(Keys[SDLK_F4])
        {
            ptk->gui_action = GUI_CMD_SELECT_SAMPLE_EDIT;
        }
        if(Keys[SDLK_F5])
        {
            ptk->gui_action = GUI_CMD_SELECT_TB303_EDIT;
        }
        if(Keys[SDLK_F6])
        {
            ptk->gui_action = GUI_CMD_SELECT_TRACK_EDIT;
        }
        if(Keys[SDLK_F7])
        {
            ptk->gui_action = GUI_CMD_SELECT_TRACK_FX_EDIT;
        }
        if(Keys[SDLK_F8])
        {
            ptk->gui_action = GUI_CMD_SELECT_FX_EDIT;
        }
        if(Keys[SDLK_F9])
        {
            ptk->gui_action = GUI_CMD_SELECT_REVERB_EDIT;
        }
        if(Keys[SDLK_F10])
        {
            ptk->gui_action = GUI_CMD_SELECT_DISKIO_EDIT;
        }
        if(Keys[SDLK_F11])
        {
            ptk->gui_action = GUI_CMD_SELECT_SCREEN_SETUP_EDIT;
        }
        if(Keys[SDLK_F12])
        {
            //ptk->gui_action = GUI_CMD_SELECT_MIDI_SETUP;
        }
    }

    // Previous track
    if(Get_LShift() && !Get_LCtrl() && Keys[SDLK_TAB])
    {
        Unselect_Selection(ptk);
        ptk->Track_Under_Caret--;
        ptk->Column_Under_Caret = 0;
        Actupated(ptk, 0);
        ptk->gui_action = GUI_CMD_SET_FOCUS_TRACK;
    }

    // Next track
    if(!Get_LShift() && !Get_LCtrl() && Keys[SDLK_TAB])
    {
        Unselect_Selection(ptk);
        ptk->Track_Under_Caret++;
        if(ptk->Track_Under_Caret >= Songtracks)
        {
            ptk->Track_Under_Caret = 0;
        }
        ptk->Column_Under_Caret = 0;
        Actupated(ptk, 0);
        ptk->gui_action = GUI_CMD_SET_FOCUS_TRACK;
    }

    // Next note
#if defined(__WIN32__)
    if(!Get_LShift() && Get_LCtrl() && Keys[SDLK_TAB] && !Key_Unicode)
#endif
#if defined(__LINUX__)
    if(!Get_LShift() && Get_LCtrl() && Keys[SDLK_TAB] && Keys_Sym[SDLK_TAB])
#else
    if(!Get_LShift() && Get_LCtrl() && Keys[SDLK_TAB])
#endif
    {
        Unselect_Selection(ptk);
        ptk->Column_Under_Caret += Table_Right_Tab_Notes[ptk->Column_Under_Caret];
        if(ptk->Column_Under_Caret >= (Channels_MultiNotes[ptk->Track_Under_Caret] * 3)) ptk->Column_Under_Caret = 0;
        Actupated(ptk, 0);
    }

    // Previous note
    if(Get_LShift() && Get_LCtrl() && Keys[SDLK_TAB])
    {
        Unselect_Selection(ptk);
        ptk->Column_Under_Caret -= Table_Left_Tab_Notes[ptk->Column_Under_Caret];
        if(ptk->Column_Under_Caret < 0) ptk->Column_Under_Caret = (Channels_MultiNotes[ptk->Track_Under_Caret] * 3) - 3;
        Actupated(ptk, 0);
    }

    if(!is_recording)
    {
        if(!Get_LShift())
        {
            // Jump to row 0
            if(Keys[SDLK_F5])
            {
                if(Pattern_Line != 0) Goto_Row(ptk, 0);
            }

            // Jump to row 16
            if(Keys[SDLK_F6])
            {
                int line = ptk->patternLines[ptk->pSequence[Cur_Position]] / 4;
                if(line > ptk->patternLines[ptk->pSequence[Cur_Position]] - 1) line = ptk->patternLines[ptk->pSequence[Cur_Position]] - 1;
                if(Pattern_Line != line) Goto_Row(ptk, line);
            }

            // Jump to row 32
            if(Keys[SDLK_F7])
            {
                int line = ptk->patternLines[ptk->pSequence[Cur_Position]] / 4;
                line *= 2;
                if(line > ptk->patternLines[ptk->pSequence[Cur_Position]] - 1) line = ptk->patternLines[ptk->pSequence[Cur_Position]] - 1;
                if(Pattern_Line != line) Goto_Row(ptk, line);
            }

            // Jump to row 48
            if(Keys[SDLK_F8])
            {
                int line = ptk->patternLines[ptk->pSequence[Cur_Position]] / 4;
                line *= 3;
                if(line > ptk->patternLines[ptk->pSequence[Cur_Position]] - 1) line = ptk->patternLines[ptk->pSequence[Cur_Position]] - 1;
                if(Pattern_Line != line) Goto_Row(ptk, line);
            }

            // Jump to row 63
            if(Keys[SDLK_F9])
            {
                int line = ptk->patternLines[ptk->pSequence[Cur_Position]] / 4;
                line *= 4;
                if(line > ptk->patternLines[ptk->pSequence[Cur_Position]] - 1) line = ptk->patternLines[ptk->pSequence[Cur_Position]] - 1;
                if(Pattern_Line != line) Goto_Row(ptk, line);
            }
        }

        // Top left of pattern
        if(Keys[SDLK_HOME])
        {
            Goto_Top_Left(ptk);
        }

        // Bottom right of pattern
        if(Keys[SDLK_END])
        {
            Goto_Bottom_Right(ptk);
        }

        // Previous column or previous track
        if(Keys[SDLK_LEFT] && !Get_LCtrl() && !Get_LAlt() ||
                vi_normal_mode(ptk, Keys[SDLK_h], Keys['H'], is_editing))
        {
            Goto_Previous_Column(ptk);
        }

        // Next column or next track
        if(Keys[SDLK_RIGHT] && !Get_LCtrl() && !Get_LAlt() ||
                vi_normal_mode(ptk, Keys[SDLK_l], Keys['L'], is_editing))
        {
            Goto_Next_Column(ptk);
        }

        // Previous row
        if((Keys[SDLK_UP] || vi_normal_mode(ptk, Keys[SDLK_k], Keys['K'], is_editing)) 
                    && !Songplaying)
        {
            Goto_Previous_Row(ptk);
        }

        // Next row
        if((Keys[SDLK_DOWN] || vi_normal_mode(ptk, Keys[SDLK_j], Keys['J'], is_editing)) 
            && !Songplaying)
        {
            Goto_Next_Row(ptk);
        }

        // Previous page (16 rows)
        if(Keys[SDLK_PAGEUP] && !Songplaying)
        {
            Goto_Previous_Page(ptk);
        }

        // Next page (16 rows)
        if(Keys[SDLK_PAGEDOWN] && !Songplaying)
        {
            Goto_Next_Page(ptk);
        }

        // Previous pattern
        if((Keys_Sym[SDLK_KP_MINUS]) && ptk->pSequence[Cur_Position] > 0)
        {
            ptk->gui_action = GUI_CMD_PREV_PATT;
        }

        // Next pattern
        if((Keys_Sym[SDLK_KP_PLUS]) && ptk->pSequence[Cur_Position] < 254)
        {
            ptk->gui_action = GUI_CMD_NEXT_PATT;
        }

        if(Keys[SDLK_INSERT] && is_editing)
        {
            // INSERT
            if(Get_LShift()) Insert_Pattern_Line(ptk, Cur_Position);
            else
            {
                Insert_Selection(ptk, ptk->Track_Under_Caret, Cur_Position);
            }
        }

        if(ptk->snamesel == INPUT_NONE)
        {
            if(Keys[SDLK_BACKSPACE] && is_editing)
            {
                // BACKSPACE
                if(Get_LShift())
                {
                    if(Pattern_Line)
                    {
                        Pattern_Line--;
                        Remove_Pattern_Line(ptk, Cur_Position);
                    }
                }
                else
                {
                    Remove_Selection(ptk, ptk->Track_Under_Caret, Cur_Position);
                }
            }
        }
    }

    // Lower octave
    if((Keys_Sym[SDLK_KP_DIVIDE]))
    {
        ptk->gui_action = GUI_CMD_LOWER_OCTAVE;
        Keys_Sym[SDLK_KP_DIVIDE] = FALSE;
    }
    // Upper octave
    if((Keys_Sym[SDLK_KP_MULTIPLY]))
    {
        ptk->gui_action = GUI_CMD_HIGHER_OCTAVE;
        Keys_Sym[SDLK_KP_MULTIPLY] = FALSE;
    }

    int retnote = NOTE_OFF;
    int retnote_raw = NOTE_OFF;
    int retvalue = NOTE_OFF;
    int reelletter = FALSE;

    // -------------------------------------------
    // Editing a name
    if(ptk->snamesel > INPUT_NONE)
    {
        if(Keys[SDLK_a])
        {
            ptk->retletter[1] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_b])
        {
            ptk->retletter[2] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_c])
        {
            ptk->retletter[3] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_d])
        {
            ptk->retletter[4] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_e])
        {
            ptk->retletter[5] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_f])
        {
            ptk->retletter[6] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_g])
        {
            ptk->retletter[7] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_h])
        {
            ptk->retletter[8] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_i])
        {
            ptk->retletter[9] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_j])
        {
            ptk->retletter[10] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_k])
        {
            ptk->retletter[11] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_l])
        {
            ptk->retletter[12] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_m])
        {
            ptk->retletter[13] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_n])
        {
            ptk->retletter[14] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_o])
        {
            ptk->retletter[15] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_p])
        {
            ptk->retletter[16] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_q])
        {
            ptk->retletter[17] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_r])
        {
            ptk->retletter[18] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_s])
        {
            ptk->retletter[19] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_t])
        {
            ptk->retletter[20] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_u])
        {
            ptk->retletter[21] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_v])
        {
            ptk->retletter[22] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_w])
        {
            ptk->retletter[23] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_x])
        {
            ptk->retletter[24] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_y])
        {
            ptk->retletter[25] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_z])
        {
            ptk->retletter[26] = TRUE;
            reelletter = TRUE;
        }

        if(Keys['A'])
        {
            ptk->retletter[41] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['B'])
        {
            ptk->retletter[42] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['C'])
        {
            ptk->retletter[43] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['D'])
        {
            ptk->retletter[44] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['E'])
        {
            ptk->retletter[45] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['F'])
        {
            ptk->retletter[46] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['G'])
        {
            ptk->retletter[47] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['H'])
        {
            ptk->retletter[48] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['I'])
        {
            ptk->retletter[49] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['J'])
        {
            ptk->retletter[50] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['K'])
        {
            ptk->retletter[51] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['L'])
        {
            ptk->retletter[52] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['M'])
        {
            ptk->retletter[53] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['N'])
        {
            ptk->retletter[54] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['O'])
        {
            ptk->retletter[55] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['P'])
        {
            ptk->retletter[56] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['Q'])
        {
            ptk->retletter[57] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['R'])
        {
            ptk->retletter[58] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['S'])
        {
            ptk->retletter[59] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['T'])
        {
            ptk->retletter[60] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['U'])
        {
            ptk->retletter[61] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['V'])
        {
            ptk->retletter[62] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['W'])
        {
            ptk->retletter[63] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['X'])
        {
            ptk->retletter[64] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['Y'])
        {
            ptk->retletter[65] = TRUE;
            reelletter = TRUE;
        }
        if(Keys['Z'])
        {
            ptk->retletter[66] = TRUE;
            reelletter = TRUE;
        }

        if(Keys[SDLK_KP0])
        {
            ptk->retletter[27] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['0'])
        {
            ptk->retletter[27] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP1])
        {
            ptk->retletter[28] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['1'])
        {
            ptk->retletter[28] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP2])
        {
            ptk->retletter[29] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['2'])
        {
            ptk->retletter[29] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP3])
        {
            ptk->retletter[30] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['3'])
        {
            ptk->retletter[30] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP4])
        {
            ptk->retletter[31] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['4'])
        {
            ptk->retletter[31] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP5])
        {
            ptk->retletter[32] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['5'])
        {
            ptk->retletter[32] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP6])
        {
            ptk->retletter[33] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['6'])
        {
            ptk->retletter[33] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP7])
        {
            ptk->retletter[34] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['7'])
        {
            ptk->retletter[34] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP8])
        {
            ptk->retletter[35] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['8'])
        {
            ptk->retletter[35] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP9])
        {
            ptk->retletter[36] = TRUE;
            reelletter = TRUE;
        }
        if(Keys_Unicode['9'])
        {
            ptk->retletter[36] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_BACKSPACE])
        {
            ptk->retletter[37] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_SPACE])
        {
            ptk->retletter[38] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_RETURN])
        {
            ptk->retletter[39] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_KP_ENTER])
        {
            ptk->retletter[39] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_PERIOD])
        {
            ptk->retletter[40] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_COMMA])
        {
            ptk->retletter[67] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_SLASH])
        {
            ptk->retletter[68] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_MINUS])
        {
            ptk->retletter[69] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_EQUALS])
        {
            ptk->retletter[70] = TRUE;
            reelletter = TRUE;
        }
        if(Keys[SDLK_ESCAPE])
        {
            ptk->retletter[71] = TRUE;
            reelletter = TRUE;
        }
    }

    if(reelletter)
    {
        Actualize_Input(ptk);
    }
    // -------------------------------------------

    int Shift = 0;
    if(Get_Caps())
    {
        if(!Get_LShift() && !Get_RShift())
        {
            Shift = UNICODE_OFFSET1;
        }
    }
    else
    {
        if(Get_LShift() || Get_RShift())
        {
            Shift = UNICODE_OFFSET1;
        }
    }

    if(ptk->snamesel == INPUT_NONE && !reelletter)
    {
        // Data columns
        if(Keys_Unicode[SDLK_0]) retvalue = 0;
        if(Keys[SDLK_KP0]) retvalue = 0;
        if(Keys_Unicode[SDLK_1]) retvalue = 1;
        if(Keys[SDLK_KP1]) retvalue = 1;
        if(Keys_Unicode[SDLK_2]) retvalue = 2;
        if(Keys[SDLK_KP2]) retvalue = 2;
        if(Keys_Unicode[SDLK_3]) retvalue = 3;
        if(Keys[SDLK_KP3]) retvalue = 3;
        if(Keys_Unicode[SDLK_4]) retvalue = 4;
        if(Keys[SDLK_KP4]) retvalue = 4;
        if(Keys_Unicode[SDLK_5]) retvalue = 5;
        if(Keys[SDLK_KP5]) retvalue = 5;
        if(Keys_Unicode[SDLK_6]) retvalue = 6;
        if(Keys[SDLK_KP6]) retvalue = 6;
        if(Keys_Unicode[SDLK_7]) retvalue = 7;
        if(Keys[SDLK_KP7]) retvalue = 7;
        if(Keys_Unicode[SDLK_8]) retvalue = 8;
        if(Keys[SDLK_KP8]) retvalue = 8;
        if(Keys_Unicode[SDLK_9]) retvalue = 9;
        if(Keys[SDLK_KP9]) retvalue = 9;
        if(Keys[SDLK_a - Shift]) {
            retvalue = 10;
        }
        if(Keys[SDLK_b - Shift]) retvalue = 11;
        if(Keys[SDLK_c - Shift]) retvalue = 12;
        if(Keys[SDLK_d - Shift]) retvalue = 13;
        if(Keys[SDLK_e - Shift]) retvalue = 14;
        if(Keys[SDLK_f - Shift]) retvalue = 15;
        if(Keys[SDLK_DELETE]) retvalue = 16;
    }

    // ------------------------------------------
    if(!Keys[SDLK_MENU] && !Get_LAlt() && !Get_LCtrl() && !Get_LShift() && ptk->snamesel == INPUT_NONE && !reelletter)
    {
        // Key jazz release
        if(Keys_Raw_Off[0x10]) { ptk->Record_Keys[0] = (12 + 1) | 0x80;  Keys_Raw_Off[0x10] = FALSE; Keys_Raw[0x10] = FALSE; }
        if(Keys_Raw_Off[3])    { ptk->Record_Keys[1] = (13 + 1) | 0x80;  Keys_Raw_Off[3] = FALSE; Keys_Raw[3] = FALSE; }
        if(Keys_Raw_Off[0x11]) { ptk->Record_Keys[2] = (14 + 1) | 0x80;  Keys_Raw_Off[0x11] = FALSE; Keys_Raw[0x11] = FALSE; }
        if(Keys_Raw_Off[4])    { ptk->Record_Keys[3] = (15 + 1) | 0x80;  Keys_Raw_Off[4] = FALSE; Keys_Raw[4] = FALSE; }
        if(Keys_Raw_Off[0x12]) { ptk->Record_Keys[4] = (16 + 1) | 0x80;  Keys_Raw_Off[0x12] = FALSE; Keys_Raw[0x12] = FALSE; }
        if(Keys_Raw_Off[0x13]) { ptk->Record_Keys[5] = (17 + 1) | 0x80;  Keys_Raw_Off[0x13] = FALSE; Keys_Raw[0x13] = FALSE; }
        if(Keys_Raw_Off[6])    { ptk->Record_Keys[6] = (18 + 1) | 0x80;  Keys_Raw_Off[6] = FALSE; Keys_Raw[6] = FALSE; }
        if(Keys_Raw_Off[0x14]) { ptk->Record_Keys[7] = (19 + 1) | 0x80;  Keys_Raw_Off[0x14] = FALSE; Keys_Raw[0x14] = FALSE; }
        if(Keys_Raw_Off[7])    { ptk->Record_Keys[8] = (20 + 1) | 0x80;  Keys_Raw_Off[7] = FALSE; Keys_Raw[7] = FALSE; }
        if(Keys_Raw_Off[0x15]) { ptk->Record_Keys[9] = (21 + 1) | 0x80;  Keys_Raw_Off[0x15] = FALSE; Keys_Raw[0x15] = FALSE; }
        if(Keys_Raw_Off[8])    { ptk->Record_Keys[10] = (22 + 1) | 0x80; Keys_Raw_Off[8] = FALSE; Keys_Raw[8] = FALSE; }
        if(Keys_Raw_Off[0x16]) { ptk->Record_Keys[11] = (23 + 1) | 0x80; Keys_Raw_Off[0x16] = FALSE; Keys_Raw[0x16] = FALSE; }
        if(Keys_Raw_Off[0x17]) { ptk->Record_Keys[12] = (24 + 1) | 0x80; Keys_Raw_Off[0x17] = FALSE; Keys_Raw[0x17] = FALSE; }
        if(Keys_Raw_Off[0xa])  { ptk->Record_Keys[13] = (25 + 1) | 0x80; Keys_Raw_Off[0xa] = FALSE; Keys_Raw[0xa] = FALSE; }
        if(Keys_Raw_Off[0x18]) { ptk->Record_Keys[14] = (26 + 1) | 0x80; Keys_Raw_Off[0x18] = FALSE; Keys_Raw[0x18] = FALSE; }
        if(Keys_Raw_Off[0xb])  { ptk->Record_Keys[15] = (27 + 1) | 0x80; Keys_Raw_Off[0xb] = FALSE; Keys_Raw[0xb] = FALSE; }
        if(Keys_Raw_Off[0x19]) { ptk->Record_Keys[16] = (28 + 1) | 0x80; Keys_Raw_Off[0x19] = FALSE; Keys_Raw[0x19] = FALSE; }
        if(Keys_Raw_Off[0x1a]) { ptk->Record_Keys[17] = (29 + 1) | 0x80; Keys_Raw_Off[0x1a] = FALSE; Keys_Raw[0x1a] = FALSE; }
        if(Keys_Raw_Off[0xd])  { ptk->Record_Keys[18] = (30 + 1) | 0x80; Keys_Raw_Off[0xd] = FALSE; Keys_Raw[0xd] = FALSE; }
        if(Keys_Raw_Off[0x1b]) { ptk->Record_Keys[19] = (31 + 1) | 0x80; Keys_Raw_Off[0x1b] = FALSE; Keys_Raw[0x1b] = FALSE; }
        if(Keys_Raw_Off[0x2c]) { ptk->Record_Keys[20] = (0 + 1) | 0x80;  Keys_Raw_Off[0x2c] = FALSE; Keys_Raw[0x2c] = FALSE; }
        if(Keys_Raw_Off[0x1f]) { ptk->Record_Keys[21] = (1 + 1) | 0x80;  Keys_Raw_Off[0x1f] = FALSE; Keys_Raw[0x1f] = FALSE; }
        if(Keys_Raw_Off[0x2d]) { ptk->Record_Keys[22] = (2 + 1) | 0x80;  Keys_Raw_Off[0x2d] = FALSE; Keys_Raw[0x2d] = FALSE; }
        if(Keys_Raw_Off[0x20]) { ptk->Record_Keys[23] = (3 + 1) | 0x80;  Keys_Raw_Off[0x20] = FALSE; Keys_Raw[0x20] = FALSE; }
        if(Keys_Raw_Off[0x2e]) { ptk->Record_Keys[24] = (4 + 1) | 0x80;  Keys_Raw_Off[0x2e] = FALSE; Keys_Raw[0x2e] = FALSE; }
        if(Keys_Raw_Off[0x2f]) { ptk->Record_Keys[25] = (5 + 1) | 0x80;  Keys_Raw_Off[0x2f] = FALSE; Keys_Raw[0x2f] = FALSE; }
        if(Keys_Raw_Off[0x22]) { ptk->Record_Keys[26] = (6 + 1) | 0x80;  Keys_Raw_Off[0x22] = FALSE; Keys_Raw[0x22] = FALSE; }
        if(Keys_Raw_Off[0x30]) { ptk->Record_Keys[27] = (7 + 1) | 0x80;  Keys_Raw_Off[0x30] = FALSE; Keys_Raw[0x30] = FALSE; }
        if(Keys_Raw_Off[0x23]) { ptk->Record_Keys[28] = (8 + 1) | 0x80;  Keys_Raw_Off[0x23] = FALSE; Keys_Raw[0x23] = FALSE; }
        if(Keys_Raw_Off[0x31]) { ptk->Record_Keys[29] = (9 + 1) | 0x80;  Keys_Raw_Off[0x31] = FALSE; Keys_Raw[0x31] = FALSE; }
        if(Keys_Raw_Off[0x24]) { ptk->Record_Keys[30] = (10 + 1) | 0x80; Keys_Raw_Off[0x24] = FALSE; Keys_Raw[0x24] = FALSE; }
        if(Keys_Raw_Off[0x32]) { ptk->Record_Keys[31] = (11 + 1) | 0x80; Keys_Raw_Off[0x32] = FALSE; Keys_Raw[0x32] = FALSE; }
        if(Keys_Raw_Off[0x33]) { ptk->Record_Keys[32] = (12 + 1) | 0x80; Keys_Raw_Off[0x33] = FALSE; Keys_Raw[0x33] = FALSE; }
        if(Keys_Raw_Off[0x26]) { ptk->Record_Keys[33] = (13 + 1) | 0x80; Keys_Raw_Off[0x26] = FALSE; Keys_Raw[0x26] = FALSE; }
        if(Keys_Raw_Off[0x34]) { ptk->Record_Keys[34] = (14 + 1) | 0x80; Keys_Raw_Off[0x34] = FALSE; Keys_Raw[0x34] = FALSE; }
        if(Keys_Raw_Off[0x27]) { ptk->Record_Keys[35] = (15 + 1) | 0x80; Keys_Raw_Off[0x27] = FALSE; Keys_Raw[0x27] = FALSE; }
        if(Keys_Raw_Off[0x35]) { ptk->Record_Keys[36] = (16 + 1) | 0x80; Keys_Raw_Off[0x35] = FALSE; Keys_Raw[0x35] = FALSE; }

        if(Keys[SDLK_DELETE])
        {
            printf("delete!\n");
            retnote = -1;
            retnote_raw = -1;
        }
        if(Keys[SDLK_INSERT])
        {
            retnote = -2;
            retnote_raw = -2;
        }
        if(Keys[SDLK_RSHIFT])
        {
            retnote = -3;
            retnote_raw = -3;
        }


        // Key jazz
        if(Keys_Raw[0x10]) { retnote_raw = 12; ptk->Record_Keys[0] = 12 + 1; }
        if(Keys_Raw[3])    { retnote_raw = 13; ptk->Record_Keys[1] = 13 + 1; }
        if(Keys_Raw[0x11]) { retnote_raw = 14; ptk->Record_Keys[2] = 14 + 1; }
        if(Keys_Raw[4])    { retnote_raw = 15; ptk->Record_Keys[3] = 15 + 1; }
        if(Keys_Raw[0x12]) { retnote_raw = 16; ptk->Record_Keys[4] = 16 + 1; }
        if(Keys_Raw[0x13]) { retnote_raw = 17; ptk->Record_Keys[5] = 17 + 1; }
        if(Keys_Raw[6])    { retnote_raw = 18; ptk->Record_Keys[6] = 18 + 1; }
        if(Keys_Raw[0x14]) { retnote_raw = 19; ptk->Record_Keys[7] = 19 + 1; }
        if(Keys_Raw[7])    { retnote_raw = 20; ptk->Record_Keys[8] = 20 + 1; }
        if(Keys_Raw[0x15]) { retnote_raw = 21; ptk->Record_Keys[9] = 21 + 1; }
        if(Keys_Raw[8])    { retnote_raw = 22; ptk->Record_Keys[10] = 22 + 1; }
        if(Keys_Raw[0x16]) { retnote_raw = 23; ptk->Record_Keys[11] = 23 + 1; }
        if(Keys_Raw[0x17]) { retnote_raw = 24; ptk->Record_Keys[12] = 24 + 1; }
        if(Keys_Raw[0xa])  { retnote_raw = 25; ptk->Record_Keys[13] = 25 + 1; }
        if(Keys_Raw[0x18]) { retnote_raw = 26; ptk->Record_Keys[14] = 26 + 1; }
        if(Keys_Raw[0xb])  { retnote_raw = 27; ptk->Record_Keys[15] = 27 + 1; }
        if(Keys_Raw[0x19]) { retnote_raw = 28; ptk->Record_Keys[16] = 28 + 1; }
        if(Keys_Raw[0x1a]) { retnote_raw = 29; ptk->Record_Keys[17] = 29 + 1; }
        if(Keys_Raw[0xd])  { retnote_raw = 30; ptk->Record_Keys[18] = 30 + 1; }
        if(Keys_Raw[0x1b]) { retnote_raw = 31; ptk->Record_Keys[19] = 31 + 1; }
        if(Keys_Raw[0x2c]) { retnote_raw = 0; ptk->Record_Keys[20] = 0 + 1; }
        if(Keys_Raw[0x1f]) { retnote_raw = 1; ptk->Record_Keys[21] = 1 + 1; }
        //if(Keys_Raw[0x2d]) { retnote_raw = 2; ptk->Record_Keys[22] = 2 + 1; }
        if(Keys_Raw[0x20]) { retnote_raw = 3; ptk->Record_Keys[23] = 3 + 1; }
        if(Keys_Raw[0x2e]) { retnote_raw = 4; ptk->Record_Keys[24] = 4 + 1; }
        if(Keys_Raw[0x2f]) { retnote_raw = 5; ptk->Record_Keys[25] = 5 + 1; }
        if(Keys_Raw[0x22]) { retnote_raw = 6; ptk->Record_Keys[26] = 6 + 1; }
        if(Keys_Raw[0x30]) { retnote_raw = 7; ptk->Record_Keys[27] = 7 + 1; }
        //if(Keys_Raw[0x23]) { retnote_raw = 8; ptk->Record_Keys[28] = 8 + 1; }
        if(Keys_Raw[0x31]) { retnote_raw = 9; ptk->Record_Keys[29] = 9 + 1; }
        //if(Keys_Raw[0x24]) { retnote_raw = 10; ptk->Record_Keys[30] = 10 + 1; }
        if(Keys_Raw[0x32]) { retnote_raw = 11; ptk->Record_Keys[31] = 11 + 1; }
        if(Keys_Raw[0x33]) { retnote_raw = 12; ptk->Record_Keys[32] = 12 + 1; }
        //if(Keys_Raw[0x26]) { retnote_raw = 13; ptk->Record_Keys[33] = 13 + 1; }
        if(Keys_Raw[0x34]) { retnote_raw = 14; ptk->Record_Keys[34] = 14 + 1; }
        //if(Keys_Raw[0x27]) { retnote_raw = 15; ptk->Record_Keys[35] = 15 + 1; }
        if(Keys_Raw[0x35]) { retnote_raw = 16; ptk->Record_Keys[36] = 16 + 1; }

        int i;
        if(is_recording) { // Start the real recording
            if(!is_recording_2 && (retnote_raw < NOTE_OFF && retnote_raw > 0) && ptk->key_record_first_time) {
                // Start recording
                is_recording_2 = 1;
                ptk->Nbr_Sub_NoteOff = 0;
                is_record_key = FALSE;
                is_editing = TRUE;
                L_MaxLevel = 0;
                R_MaxLevel = 0;
                Switch_Cmd_Playing(ptk, FALSE);
                Pattern_Line_Visual = Pattern_Line;
                ptk->key_record_first_time = FALSE;
                ptk->old_key_Pattern_Line = Pattern_Line;
                Clear_Midi_Channels_Pool();
                ptk->player_pos = -1;
                ptk->metronome_rows_counter = 0;
                Post_Song_Init(ptk);
                Ptk_Play(ptk);
            }
        }

        int Record_Key_Pressed = FALSE;
        for(i = 0; i < 37; i++) {
            if(ptk->Record_Keys[i])
            {
                Record_Key_Pressed = TRUE;
            }
        }

        if(Record_Key_Pressed)
        {
            for(i = 0; i < 37; i++)
            {
                if(ptk->Record_Keys[i])
                {
                    int tmp_note = ((ptk->Record_Keys[i] & 0x7f) - 1) + Current_Octave * 12;
                    if(tmp_note > NOTE_MAX) tmp_note = NOTE_MAX;

                    if(!(ptk->Record_Keys[i] & 0x80))
                    {
                        // Note on
                        if(ptk->Record_Keys_State[i] == FALSE)
                        {
                            ptk->Record_Keys_State[i] = TRUE;

                            // Write it
                            if(is_recording_2)
                            {
                                Send_Note(Raw_Keys_to_Send[i], FALSE, FALSE);
                            }

                            // Play it
                            Note_Jazz(ptk, ptk->Track_Under_Caret, tmp_note, 1.0f);
                        }
                    }
                    else
                    {
                        // Note off
                        if(ptk->Record_Keys_State[i] == TRUE)
                        {
                            ptk->Record_Keys_State[i] = FALSE;
                            ptk->Record_Keys[i] = 0;

                            // Do not reach that point
                            if(is_recording_2)
                            {
                                Send_Note(Raw_Keys_to_Send[i] | 0x80, FALSE, FALSE);
                            }

                            Note_Jazz_Off(ptk, tmp_note);
                        }
                    }
                }
            }
        }
    }

    // Turn edit mode on/off
    if(Keys[SDLK_ESCAPE] && ptk->snamesel == INPUT_NONE && ptk->pos_space == 1)
    {
        if(Get_LShift())
        {
            ptk->gui_action = GUI_CMD_RECORD_MODE;
        }
        else
        {
            ptk->gui_action = GUI_CMD_EDIT_MODE;
        }
    }

    /* vi mode: 'i' to "insert" */
    if(vi_normal_mode(ptk, Keys[SDLK_i], Keys[SDLK_i], is_editing)) 
    {
        ptk->gui_action = GUI_CMD_EDIT_MODE;
    }


    if(ptk->pos_space == 0 && !Keys[SDLK_SPACE]) ptk->pos_space = 1;

    if(!is_recording)
    {
        // Play song
        //if(Keys[SDLK_RCTRL] && ptk->snamesel == INPUT_NONE && ptk->po_ctrl2)
        if(Keys[SDLK_SPACE] && ptk->snamesel == INPUT_NONE && ptk->po_ctrl2)
        {
            plx = 0;
            ptk->po_ctrl2 = FALSE;
            if(!Get_LShift()) Pattern_Line = 0;
            if(Songplaying == TRUE) {
                ptk->gui_action = GUI_CMD_STOP_SONG;
            } else {
                ptk->gui_action = GUI_CMD_PLAY_SONG;
            }
        }

        if(!Keys[SDLK_RCTRL] && !ptk->po_ctrl2) ptk->po_ctrl2 = TRUE;

        if(Keys[SDLK_RALT] && ptk->snamesel == INPUT_NONE && ptk->po_alt2)
        {
            plx = 1;
            ptk->po_alt2 = FALSE;
            if(!Get_LShift()) Pattern_Line = 0;
            ptk->gui_action = GUI_CMD_PLAY_SONG;
        }
        if(!Keys[SDLK_RALT] && !ptk->po_alt2) ptk->po_alt2 = TRUE;

        // Play song
        if(Get_LAlt() && ptk->snamesel == INPUT_NONE && !ptk->po_alt)
        {
            ptk->po_alt = TRUE;
        }
        if(!Get_LAlt() && ptk->po_alt) ptk->po_alt = FALSE;

        if(Key_Unicode)
        {
            if(Get_LShift())
            {
                if(Keys[SDLK_m - UNICODE_OFFSET1])
                {
                    if(CHAN_MUTE_STATE[ptk->Track_Under_Caret] == 0) CHAN_MUTE_STATE[ptk->Track_Under_Caret] = 1;
                    else CHAN_MUTE_STATE[ptk->Track_Under_Caret] = 0;
                    if(ptk->userscreen == USER_SCREEN_TRACK_EDIT) Actualize_Track_Ed(ptk, 10);
                    Actupated(ptk, 0);
                }

                // Display instruments list
                if(Keys[SDLK_i - UNICODE_OFFSET1])
                {
                    ptk->Scopish = SCOPE_ZONE_INSTR_LIST;
                    Draw_Scope_Files_Button(ptk);
                }

                // Display synths list
                if(Keys[SDLK_s - UNICODE_OFFSET1])
                {
                    ptk->Scopish = SCOPE_ZONE_SYNTH_LIST;
                    Draw_Scope_Files_Button(ptk);
                }

            }
        }

        if(Get_LCtrl())
        {

            if(Keys[SDLK_PERIOD])
            {
                ptk->gui_action = GUI_CMD_INCREASE_STEP_ADD;
            }

            if(Keys[SDLK_COMMA])
            {
                ptk->gui_action = GUI_CMD_DECREASE_STEP_ADD;
            }

            if(!Get_LAlt() && !Get_LShift())
            {
                if(Keys[SDLK_1] || Keys[SDLK_KP1])
                {
                    Curr_Buff_Block = 0;
                    Actupated(ptk, 0);
                }
                if(Keys[SDLK_2] || Keys[SDLK_KP2])
                {
                    Curr_Buff_Block = 1;
                    Actupated(ptk, 0);
                }
                if(Keys[SDLK_3] || Keys[SDLK_KP3])
                {
                    Curr_Buff_Block = 2;
                    Actupated(ptk, 0);
                }
                if(Keys[SDLK_4] || Keys[SDLK_KP4])
                {
                    Curr_Buff_Block = 3;
                    Actupated(ptk, 0);
                }

                if(Keys[SDLK_LEFT])
                {
                    if(ptk->pSequence[Cur_Position] > 0)
                    {
                        ptk->gui_action = GUI_CMD_PREV_PATT;
                    }
                    Keys[SDLK_LEFT] = FALSE;
                }
                if(Keys[SDLK_RIGHT])
                {
                    if(ptk->pSequence[Cur_Position] < 254)
                    {
                        ptk->gui_action = GUI_CMD_NEXT_PATT;
                    }
                    Keys[SDLK_RIGHT] = FALSE;
                }

            }

            if(Key_Unicode)
            {
                if(Get_LShift())
                {
                    // Solo track
                    if(Keys[SDLK_m - UNICODE_OFFSET2])
                    {
                        Solo_Track(ptk->Track_Under_Caret);
                        // Will unmute the correct track
                        if(CHAN_MUTE_STATE[ptk->Track_Under_Caret] == 0) CHAN_MUTE_STATE[ptk->Track_Under_Caret] = 1;
                        else CHAN_MUTE_STATE[ptk->Track_Under_Caret] = 0;
                        if(ptk->userscreen == USER_SCREEN_TRACK_EDIT) Actualize_Track_Ed(ptk, 10);
                        Actupated(ptk, 0);
                    }
                }

                // Save
                if(Keys[SDLK_s - UNICODE_OFFSET2])
                {
                    if(File_Exist_Req(ptk, "%s"SLASH"%s.ptk", Dir_Mods, ptk->name))
                    {
                        Display_Requester(ptk, &Overwrite_Requester, GUI_CMD_SAVE_MODULE);
                    }
                    else
                    {
                        ptk->gui_action = GUI_CMD_SAVE_MODULE;
                    }
                }

                // Cut selected block
                if(Keys[SDLK_x - UNICODE_OFFSET2] &&
                   block_start_track[Curr_Buff_Block] != -1 &&
                   block_end_track[Curr_Buff_Block] != -1)
                {
                    Cut_Selection(ptk, Cur_Position);
                }

                // Copy selected block
                if(Keys[SDLK_c - UNICODE_OFFSET2] &&
                   block_start_track[Curr_Buff_Block] != -1 &&
                   block_end_track[Curr_Buff_Block] != -1)
                {
                    Copy_Selection(ptk, Cur_Position);
                }

                // Interpolate the values of a selected block
                if(Keys[SDLK_i - UNICODE_OFFSET2] && is_editing)
                {
                    Interpolate_Block(ptk, Cur_Position);
                }

                // Randomize the values of a selected block
                if(Keys[SDLK_r - UNICODE_OFFSET2] && is_editing)
                {
                    Randomize_Block(ptk, Cur_Position);
                }

                // Randomize the values of a selected block
                if(Keys[SDLK_w - UNICODE_OFFSET2])
                {
                    if(File_Exist_Req(ptk, "%s"SLASH"%s.ppb", Dir_Patterns, Selection_Name))
                    {
                        Display_Requester(ptk, &Overwrite_Requester, GUI_CMD_SAVE_PATTERN);
                    }
                    else
                    {
                        ptk->gui_action = GUI_CMD_SAVE_PATTERN;
                    }
                }

                // Paste the block buffer into a pattern
                if(Keys[SDLK_v - UNICODE_OFFSET2] && block_start_track_nibble[Curr_Buff_Block] != -1 && block_end_track_nibble[Curr_Buff_Block] != -1 && is_editing)
                {
                    Paste_Block(ptk, Cur_Position, Paste_Across, TRUE);
                }

                // Transpose the selection 1 semitone higher
                if(Keys[SDLK_u - UNICODE_OFFSET2] && is_editing)
                {
                    if(Get_LShift()) Instrument_Semitone_Up_Block(ptk, Cur_Position);
                    else Semitone_Up_Block(ptk, Cur_Position);
                }

                // Transpose the selection 1 semitone lower
                if(Keys[SDLK_d - UNICODE_OFFSET2] && is_editing)
                {
                    if(Get_LShift()) Instrument_Semitone_Down_Block(ptk, Cur_Position);
                    else Semitone_Down_Block(ptk, Cur_Position);
                }

                // Transpose the selection 1 octave higher
                if(Keys[SDLK_k - UNICODE_OFFSET2] && is_editing)
                {
                    if(Get_LShift()) Instrument_Octave_Up_Block(ptk, Cur_Position);
                    else Octave_Up_Block(ptk, Cur_Position);
                }

                // Transpose the selection 1 octave lower
                if(Keys[SDLK_l - UNICODE_OFFSET2] && is_editing)
                {
                    if(Get_LShift()) Instrument_Octave_Down_Block(ptk, Cur_Position);
                    else Octave_Down_Block(ptk, Cur_Position);
                }

                // Select the current track or the current pattern
                if(Keys[SDLK_a - UNICODE_OFFSET2])
                {
                    if(Get_LShift()) Select_Pattern_Block(ptk);
                    else Select_Track_Block(ptk);
                }
            }
        }

        if(Get_LAlt())
        {
            if(Get_LCtrl())
            {
                // Previous position in sequence
                if(Keys[SDLK_LEFT])
                {
                    ptk->gui_action = GUI_CMD_PREVIOUS_POSITION;
                    Keys[SDLK_LEFT] = FALSE;
                }

                // Next position in sequence
                if(Keys[SDLK_RIGHT])
                {
                    ptk->gui_action = GUI_CMD_NEXT_POSITION;
                    Keys[SDLK_RIGHT] = FALSE;
                }
            }
            else
            {
                // Previous instrument
                if(Keys[SDLK_LEFT])
                {
                    ptk->gui_action = GUI_CMD_PREV_INSTR;
                    Keys[SDLK_LEFT] = FALSE;
                }

                // Next instrument
                if(Keys[SDLK_RIGHT])
                {
                    ptk->gui_action = GUI_CMD_NEXT_INSTR;
                    Keys[SDLK_RIGHT] = FALSE;
                }

                // Select the current note/instrument
                if(Keys[SDLK_a])
                {
                    Select_Note_Block(ptk);
                }

                // Select the current track or the current pattern
                if(Keys[SDLK_a - UNICODE_OFFSET1])
                {
                    if(Get_LShift()) Select_All_Notes_Block(ptk);
                }

            }
        }

        Done_Value = FALSE;
        if(!Get_LAlt())
        {
            if(retvalue != NOTE_OFF && is_editing == 1)
            {
                int ped_cell;
                int i;
                int j = (Channels_MultiNotes[ptk->Track_Under_Caret] - 1) * 3;

                // Instrument
                for(i = 0; i < Channels_MultiNotes[ptk->Track_Under_Caret]; i++)
                {
                    if(ptk->Column_Under_Caret == 1 + (i * 3))
                    {
                        i++;
                        break;
                    }
                }
                i--;

                // Odd
                if(ptk->Column_Under_Caret == (1 + (i * 3)) ||
                   ptk->Column_Under_Caret == (3 + j) ||
                   ptk->Column_Under_Caret == (5 + j) ||
                   ptk->Column_Under_Caret == (7 + j) ||
                   ptk->Column_Under_Caret == (9 + j) ||
                   ptk->Column_Under_Caret == (11 + j) ||
                   ptk->Column_Under_Caret == (13 + j) ||
                   ptk->Column_Under_Caret == (15 + j) ||
                   ptk->Column_Under_Caret == (17 + j) ||
                   ptk->Column_Under_Caret == (19 + j) ||
                   ptk->Column_Under_Caret == (21 + j))
                {
                    ped_cell = PATTERN_INSTR1 + (i * 2);                    // instrument
                    if(ptk->Column_Under_Caret == (3 + j)) ped_cell = PATTERN_VOLUME;       // volume
                    if(ptk->Column_Under_Caret == (5 + j)) ped_cell = PATTERN_PANNING;      // panning
                    if(ptk->Column_Under_Caret == (7 + j)) ped_cell = PATTERN_FX;           // fx
                    if(ptk->Column_Under_Caret == (9 + j)) ped_cell = PATTERN_FXDATA;       // fx data
                    if(ptk->Column_Under_Caret == (11 + j)) ped_cell = PATTERN_FX2;         // fx 2
                    if(ptk->Column_Under_Caret == (13 + j)) ped_cell = PATTERN_FXDATA2;     // fx 2 data
                    if(ptk->Column_Under_Caret == (15 + j)) ped_cell = PATTERN_FX3;          // fx 3
                    if(ptk->Column_Under_Caret == (17 + j)) ped_cell = PATTERN_FXDATA3;     // fx 3 data
                    if(ptk->Column_Under_Caret == (19 + j)) ped_cell = PATTERN_FX4;         // fx 4
                    if(ptk->Column_Under_Caret == (21 + j)) ped_cell = PATTERN_FXDATA4;     // fx 4 data
                    
                    ptk->ltretvalue = retvalue;
                    xoffseted = (ptk->Track_Under_Caret * PATTERN_BYTES) + (Pattern_Line * PATTERN_ROW_LEN) + ped_cell;

                    int oldval = *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted);

                    if(retvalue < 16)
                    {
                        if(oldval == 255 && ptk->Column_Under_Caret == (1 + (i * 3))) oldval = 0;
                        if(oldval == 255 && ptk->Column_Under_Caret == (3 + j)) oldval = 0;
                        if(oldval == 255 && ptk->Column_Under_Caret == (5 + j)) oldval = 0;
                        oldval = (oldval & 0xf) + (retvalue << 4);
                        *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = oldval;

                        // Max panning
                        if(oldval != 255 && ptk->Column_Under_Caret == (5 + j) &&
                           *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) != 0x90)
                        {
                            if(oldval != 255 && ptk->Column_Under_Caret == (5 + j) &&
                            *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) > 0x80)
                            {
                                *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = 0x80;
                            }
                        }

                        // Max instrument
                        if(oldval != 255 && ptk->Column_Under_Caret == (1 + (i * 3)) &&
                           *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) > 0x7f)
                        {
                            *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = 0x7f;
                        }

                        if(!is_recording) if(!Songplaying)
                        {
                            Pattern_Line += Current_Edit_Steps;
                            if(!Current_Edit_Steps)
                            {
                                Goto_Next_Column(ptk);
                                Done_Value = TRUE;
                            }
                        }
                    }
                    else
                    {
                        if(!Delete_Selection(ptk, Cur_Position))
                        {
                            oldval = 0;
                            if(ptk->Column_Under_Caret == (1 + (i * 3))) oldval = 255;
                            if(ptk->Column_Under_Caret == (3 + j)) oldval = 255;
                            if(ptk->Column_Under_Caret == (5 + j)) oldval = 255;
                            *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = oldval;

                            // Max panning
                            if(oldval != 255 && ptk->Column_Under_Caret == (5 + j) &&
                               *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) != 0x90)
                            {
                                if(oldval != 255 && ptk->Column_Under_Caret == (5 + j) &&
                                   *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) > 0x80)
                                {
                                    *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = 0x80;
                                }
                            }

                            // Max instrument
                            if(oldval != 255 && ptk->Column_Under_Caret == (1 + (i * 3)) &&
                               *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) > 0x7f)
                            {
                                *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = 0x7f;
                            }

                            if(!is_recording) if(!Songplaying)
                            {
                                Pattern_Line += Current_Edit_Steps;
                                if(!Current_Edit_Steps)
                                {
                                    Goto_Next_Column(ptk);
                                    Done_Value = TRUE;
                                }
                            }
                        }
                    }
                    Actupated(ptk, 0);
                }

                if(!Done_Value)
                {
                    for(i = 0; i < Channels_MultiNotes[ptk->Track_Under_Caret]; i++)
                    {
                        if(ptk->Column_Under_Caret == 2 +(i * 3))
                        {
                            i++;
                            break;
                        }
                    }
                    i--;
                    // Even
                    if(ptk->Column_Under_Caret == (2 + (i * 3)) ||
                       ptk->Column_Under_Caret == (4 + j) ||
                       ptk->Column_Under_Caret == (6 + j) ||
                       ptk->Column_Under_Caret == (8 + j) ||
                       ptk->Column_Under_Caret == (10 + j) ||
                       ptk->Column_Under_Caret == (12 + j) ||
                       ptk->Column_Under_Caret == (14 + j) ||
                       ptk->Column_Under_Caret == (16 + j) ||
                       ptk->Column_Under_Caret == (18 + j) ||
                       ptk->Column_Under_Caret == (20 + j) ||
                       ptk->Column_Under_Caret == (22 + j))
                    {
                        ped_cell = PATTERN_INSTR1 + (i * 2);
                        if(ptk->Column_Under_Caret == (4 + j)) ped_cell = PATTERN_VOLUME;
                        if(ptk->Column_Under_Caret == (6 + j)) ped_cell = PATTERN_PANNING;
                        if(ptk->Column_Under_Caret == (8 + j)) ped_cell = PATTERN_FX;
                        if(ptk->Column_Under_Caret == (10 + j)) ped_cell = PATTERN_FXDATA;
                        if(ptk->Column_Under_Caret == (12 + j)) ped_cell = PATTERN_FX2;
                        if(ptk->Column_Under_Caret == (14 + j)) ped_cell = PATTERN_FXDATA2;
                        if(ptk->Column_Under_Caret == (16 + j)) ped_cell = PATTERN_FX3;
                        if(ptk->Column_Under_Caret == (18 + j)) ped_cell = PATTERN_FXDATA3;
                        if(ptk->Column_Under_Caret == (20 + j)) ped_cell = PATTERN_FX4;
                        if(ptk->Column_Under_Caret == (22 + j)) ped_cell = PATTERN_FXDATA4;

                        ptk->ltretvalue = retvalue;
                        xoffseted = (ptk->Track_Under_Caret * PATTERN_BYTES) + (Pattern_Line * PATTERN_ROW_LEN) + ped_cell;
                        int oldval = *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted);

                        if(retvalue < 16)
                        {
                            if(oldval == 255 && ptk->Column_Under_Caret == (2 + (i * 3))) oldval = 0;
                            if(oldval == 255 && ptk->Column_Under_Caret == (4 + j)) oldval = 0;
                            if(oldval == 255 && ptk->Column_Under_Caret == (6 + j)) oldval = 0;
                            oldval = (oldval & 0xf0) + retvalue;
                            *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = oldval;

                            // Max panning
                            if(oldval != 255 && ptk->Column_Under_Caret == (6 + j) &&
                               *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) != 0x90)
                            {
                                if(oldval != 255 && ptk->Column_Under_Caret == (6 + j) &&
                                   *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) > 0x80)
                                {
                                    *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = 0x80;
                                }
                            }

                            // Max instrument
                            if(oldval != 255 && ptk->Column_Under_Caret == (2 + (i * 3)) &&
                               *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) > 127)
                            {
                                *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = 127;
                            }

                            if(!is_recording) if(!Songplaying)
                            {
                                Pattern_Line += Current_Edit_Steps;
                                if(!Current_Edit_Steps)
                                {
                                    Goto_Next_Column(ptk);
                                }
                            }
                        }
                        else
                        {
                            if(!Delete_Selection(ptk, Cur_Position))
                            {
                                oldval = 0;
                                if(ptk->Column_Under_Caret == (2 + (i * 3))) oldval = 255;
                                if(ptk->Column_Under_Caret == (4 + j)) oldval = 255;
                                if(ptk->Column_Under_Caret == (6 + j)) oldval = 255;
                                *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = oldval;

                                // Max panning
                                if(oldval != 255 && ptk->Column_Under_Caret == (6 + j) &&
                                   *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) != 0x90)
                                {
                                    if(oldval != 255 && ptk->Column_Under_Caret == (6 + j) &&
                                       *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) > 0x80)
                                    {
                                        *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = 0x80;
                                    }
                                }

                                // Max instrument
                                if(oldval != 255 && ptk->Column_Under_Caret == (2 + (i * 3)) &&
                                   *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) > 0x7f)
                                {
                                    *(ptk->RawPatterns + ptk->pSequence[Cur_Position] * PATTERN_LEN + xoffseted) = 0x7f;
                                }

                                if(!is_recording) if(!Songplaying)
                                {
                                    Pattern_Line += Current_Edit_Steps;
                                    if(!Current_Edit_Steps)
                                    {
                                        Goto_Next_Column(ptk);
                                    }
                                }
                            }
                        }
                        Actupated(ptk, 0);
                    }
                }
            }
        }
    }

    // --------------------------------------------
    // Enter one or several notes (used for midi and record as well as simple editing)
    if(!Get_LAlt() && !Get_LCtrl() && !Get_LShift() && ptk->snamesel == INPUT_NONE && !reelletter)
    {
        int go_note = FALSE;
        int i;
        for(i = 0; i < Keyboard_Nbr_Events; i++)
        {
            int Idx = Keyboard_Events[i];

            if(!Keyboard_Notes_Type[i])
            {
                // Key editing
                switch(Idx & 0x7f)
                {
                    case 0x10:
                    case 3:
                    case 0x11:
                    case 4:
                    case 0x12:
                    case 0x13:
                    case 6:
                    case 0x14:
                    case 7:
                    case 0x15:
                    case 8:
                    case 0x16:
                    case 0x17:
                    case 0xa:
                    case 0x18:
                    case 0xb:
                    case 0x19:
                    case 0x1a:
                    case 0xd:
                    case 0x1b:
                    case 0x2c:
                    case 0x1f:
                    case 0x2d:
                    case 0x20:
                    case 0x2e:
                    case 0x2f:
                    case 0x22:
                    case 0x30:
                    case 0x23:
                    case 0x31:
                    case 0x24:
                    case 0x32:
                    case 0x33:
                    case 0x26:
                    case 0x34:
                    case 0x27:
                    case 0x35:
                        go_note = TRUE;
                        break;
                }
            }
            else
            {
                go_note = TRUE;
            }
        }
        // Write a note
        if(go_note)
        {
            int column = 0;
            int in_note = FALSE;

            // Check if this is a legal note column
            for(i = 0; i < Channels_MultiNotes[ptk->Track_Under_Caret]; i++)
            {
                if(ptk->Column_Under_Caret == (i * 3))
                {
                    in_note = TRUE;
                    break;
                }
                column += 2;
            }

            if(in_note)
            {
                int i;
                int move_down = FALSE;
                for(i = 0; i < Keyboard_Nbr_Events; i++)
                {
                    int Idx = Keyboard_Events[i];
                    int pos;
                    int tmp_note;
                    int line;

                    if(!Keyboard_Notes_Type[i])
                    {
                        // Key editing
                        switch(Idx & 0x7f)
                        {
                            case 0x10: { retnote_raw = 12; break; }
                            case 3:    { retnote_raw = 13; break; }
                            case 0x11: { retnote_raw = 14; break; }
                            case 4:    { retnote_raw = 15; break; }
                            case 0x12: { retnote_raw = 16; break; }
                            case 0x13: { retnote_raw = 17; break; }
                            case 6:    { retnote_raw = 18; break; }
                            case 0x14: { retnote_raw = 19; break; }
                            case 7:    { retnote_raw = 20; break; }
                            case 0x15: { retnote_raw = 21; break; }
                            case 8:    { retnote_raw = 22; break; }
                            case 0x16: { retnote_raw = 23; break; }
                            case 0x17: { retnote_raw = 24; break; }
                            case 0xa:  { retnote_raw = 25; break; }
                            case 0x18: { retnote_raw = 26; break; }
                            case 0xb:  { retnote_raw = 27; break; }
                            case 0x19: { retnote_raw = 28; break; }
                            case 0x1a: { retnote_raw = 29; break; }
                            case 0xd:  { retnote_raw = 30; break; }
                            case 0x1b: { retnote_raw = 31; break; }
                            case 0x2c: { retnote_raw = 0; break; }
                            case 0x1f: { retnote_raw = 1; break; }
                            case 0x2d: { retnote_raw = 2; break; }
                            case 0x20: { retnote_raw = 3; break; }
                            case 0x2e: { retnote_raw = 4; break; }
                            case 0x2f: { retnote_raw = 5; break; }
                            case 0x22: { retnote_raw = 6; break; }
                            case 0x30: { retnote_raw = 7; break; }
                            case 0x23: { retnote_raw = 8; break; }
                            case 0x31: { retnote_raw = 9; break; }
                            case 0x24: { retnote_raw = 10; break; }
                            case 0x32: { retnote_raw = 11; break; }
                            case 0x33: { retnote_raw = 12; break; }
                            case 0x26: { retnote_raw = 13; break; }
                            case 0x34: { retnote_raw = 14; break; }
                            case 0x27: { retnote_raw = 15; break; }
                            case 0x35: { retnote_raw = 16; break; }
                            default:
                                goto No_Key;
                        }
                        tmp_note = retnote_raw + (Current_Octave * 12);
                        if(tmp_note > NOTE_MAX) tmp_note = NOTE_MAX;
                    }
                    else
                    {
                        tmp_note = Idx & 0x7f;
                    }

                    line = Pattern_Line;
                    if(is_recording_2 || Songplaying) line = Pattern_Line_Visual;

                    if(!(Idx & 0x80))
                    {
                        // Note
                        if(retnote > -1)
                        {
                            xoffseted = Get_Pattern_Offset(ptk, ptk->pSequence[Cur_Position], ptk->Track_Under_Caret, line);

                            // Select the sub channel
                            pos = i;

                            if(is_recording_2 || is_editing)
                            {
                                // Store the note column where it was entered
                                Sub_Channels_NoteOff[ptk->Track_Under_Caret][ptk->Nbr_Sub_NoteOff].Note = ((tmp_note + 1) << 8);
                                Sub_Channels_NoteOff[ptk->Track_Under_Caret][ptk->Nbr_Sub_NoteOff].Channel = ptk->Track_Under_Caret;
                                if(Keyboard_Notes_Bound[i])
                                {
                                    pos = (ptk->Column_Under_Caret / 3);
                                }
                                else
                                {
                                    pos = (ptk->Column_Under_Caret / 3) + ptk->Nbr_Sub_NoteOff;
                                }
                                Sub_Channels_NoteOff[ptk->Track_Under_Caret][ptk->Nbr_Sub_NoteOff].Sub_Channel = pos;

                                if(ptk->Nbr_Sub_NoteOff < (Channels_MultiNotes[ptk->Track_Under_Caret] - 1)) ptk->Nbr_Sub_NoteOff++;
                                else ptk->Nbr_Sub_NoteOff = 0;
                            }

                            if(pos > (Channels_MultiNotes[ptk->Track_Under_Caret] - 1))
                            {
                                pos -= Channels_MultiNotes[ptk->Track_Under_Caret];
                            }
                            xoffseted += (pos * 2);

                            if(is_editing)
                            {
                                *(ptk->RawPatterns + xoffseted + PATTERN_NOTE1) = tmp_note;
                                *(ptk->RawPatterns + xoffseted + PATTERN_INSTR1) = ptk->Current_Instrument;
                            }
                        }
                        move_down = TRUE;
                    }
                    else
                    {
                        // Note off
                        if(is_recording_2 || is_editing)
                        {
                            LPJAZZ_KEY Channel = Get_Jazz_Key_Off(Sub_Channels_NoteOff, (tmp_note + 1) << 8);
                            if(Channel)
                            {
                                if(is_recording_2)
                                {
                                    xoffseted = Get_Pattern_Offset(ptk, ptk->pSequence[Cur_Position], Channel->Channel, line);
                                    pos = Channel->Sub_Channel;
                                    if(pos > (Channels_MultiNotes[ptk->Track_Under_Caret] - 1))
                                    {
                                        pos -= Channels_MultiNotes[ptk->Track_Under_Caret];
                                    }
                                    xoffseted += (pos * 2);
                                    *(ptk->RawPatterns + xoffseted + PATTERN_NOTE1) = NOTE_OFF;
                                    *(ptk->RawPatterns + xoffseted + PATTERN_INSTR1) = NO_INSTR;
                                }
                                // Now discard any remaining note off for that channel/sub channel
                                ptk->Nbr_Sub_NoteOff -= Discard_Key_Note_Off(Sub_Channels_NoteOff, Channel->Channel, Channel->Sub_Channel);
                                if(ptk->Nbr_Sub_NoteOff < 0) ptk->Nbr_Sub_NoteOff = 0;
                            }
                        }
                        if(is_recording_2) move_down = TRUE;
                    }
No_Key:;
                }
                Keyboard_Nbr_Events = 0;
                if(is_editing)
                {
                    if(!Songplaying)
                    {
                        if(move_down)
                        {
                            Pattern_Line += Current_Edit_Steps;
                            if(!Current_Edit_Steps)
                            {
                                Goto_Next_Column(ptk);
                            }
                        }
                    }
                }
                if(is_recording_2)
                {
                    if(ptk->old_key_Pattern_Line != Pattern_Line)
                    {
                        Actualize_Sequencer(ptk);
                    }
                    ptk->old_key_Pattern_Line = Pattern_Line;
                }
                Actupated(ptk, 0);
            }
        }

        /* vi key: 'x' deletes a note */
        if(vi_normal_mode(ptk, Keys[SDLK_x], Keys[SDLK_x], is_editing)) {
            retnote = -4;
            retnote_raw = -4;
        }


        // Note off (RSHIFT)
        if(retnote == -3 && is_editing && !is_recording_2)
        {
            int column = 0;
            int in_note = FALSE;
            int i;

            // Check if this is a legal note column
            for(i = 0; i < Channels_MultiNotes[ptk->Track_Under_Caret]; i++)
            {
                if(ptk->Column_Under_Caret == (i * 3))
                {
                    in_note = TRUE;
                    break;
                }
                column += 2;
            }

            if(in_note)
            {
                xoffseted = Get_Pattern_Offset(ptk, ptk->pSequence[Cur_Position], ptk->Track_Under_Caret, Pattern_Line);

                *(ptk->RawPatterns + xoffseted + PATTERN_NOTE1 + column) = NOTE_OFF;
                *(ptk->RawPatterns + xoffseted + PATTERN_INSTR1 + column) = NO_INSTR;

                if(!is_recording) if(!Songplaying)
                {
                    Pattern_Line += Current_Edit_Steps;
                    if(!Current_Edit_Steps)
                    {
                        Goto_Next_Column(ptk);
                    }
                }
                Actupated(ptk, 0);
            }
        }

        /* Delete a note
         * this conditional has been modified so that 'x' can also delete 
         * outside of edit mode.
         * When the 'x' key is pressed in normal mode, it returns the error
         * code -4.
         * 
         */
        if(((retnote == -1 && is_editing) || retnote == -4) && !is_recording_2)
        {
            int column = 0;
            int in_note = FALSE;
            int i;

            // Check if this is a legal note column
            for(i = 0; i < Channels_MultiNotes[ptk->Track_Under_Caret]; i++)
            {
                if(ptk->Column_Under_Caret == (i * 3))
                {
                    in_note = TRUE;
                    break;
                }
                column += 2;
            }

            if(in_note)
            {
                xoffseted = Get_Pattern_Offset(ptk, ptk->pSequence[Cur_Position], ptk->Track_Under_Caret, Pattern_Line);

                if(!Delete_Selection(ptk, Cur_Position))
                {
                    // Delete the note under the caret
                    *(ptk->RawPatterns + xoffseted + PATTERN_NOTE1 + column) = NO_NOTE;
                    *(ptk->RawPatterns + xoffseted + PATTERN_INSTR1 + column) = NO_INSTR;

                    if(!is_recording) if(!Songplaying)
                    {
                        Pattern_Line += Current_Edit_Steps;
                        if(!Current_Edit_Steps)
                        {
                            Goto_Next_Column(ptk);
                        }
                    }
                }
                Actupated(ptk, 0);
            }
        }
    }
}

// ------------------------------------------------------
// Retrieve mouse datas
char zcheckMouse_nobutton(int x, int y, int xs, int ys)
{
    if(Mouse.x > x && Mouse.x < x + xs && Mouse.y > y && Mouse.y < y + ys + 1)
    {
        return(1);
    }
    return(0);
}

char zcheckMouse(ptk_data *ptk, int x, int y, int xs, int ys)
{
    if(!ptk->gui_pushed && Mouse.x > x && Mouse.x < x + xs && Mouse.y > y && Mouse.y < y + ys + 1)
    {
        ptk->gui_lx = x;
        ptk->gui_ly = y;
        ptk->gui_lxs = xs;
        ptk->gui_lys = ys;
        ptk->gui_pushed = Mouse.button_oneshot;
        ptk->fluzy = -1;
        return(1);
    }
    if(ptk->gui_pushed && x == ptk->gui_lx && y == ptk->gui_ly && xs == ptk->gui_lxs && ys == ptk->gui_lys)
    {
        return(1);
    }
    return(0);
}

// ------------------------------------------------------
// Mouse Handler
void Mouse_Handler(ptk_data *ptk)
{
    int Cur_Position = Get_Song_Position(ptk);
    int i;

    if(ptk->gui_action == GUI_CMD_NOP) ptk->gui_action = GUI_CMD_NONE;

    ptk->teac = 0;

    // mouse wheel up
    if(Mouse.wheel == 1)
    {
        // Scroll the pattern
        Mouse_Wheel_Pattern_Ed(ptk, -ptk->MouseWheel_Multiplier, !Songplaying && !is_recording);

        // Scroll the files list
        switch(ptk->Scopish)
        {
            case SCOPE_ZONE_INSTR_LIST:
            case SCOPE_ZONE_SYNTH_LIST:

                // Scroll the instruments/synths lists
                if(zcheckMouse(ptk, 393, 41, (Cur_Width - 394), 136) == 1)
                {
                    Instrs_index--;
                    ptk->gui_action = GUI_CMD_INSTR_SYNTH_SCROLL;
                }
                break;

            case SCOPE_ZONE_MOD_DIR:
            case SCOPE_ZONE_INSTR_DIR:
            case SCOPE_ZONE_PRESET_DIR:
            case SCOPE_ZONE_REVERB_DIR:
            case SCOPE_ZONE_MIDICFG_DIR:
            case SCOPE_ZONE_PATTERN_DIR:
            case SCOPE_ZONE_SAMPLE_DIR:

                // Scroll the files lists
                if(zcheckMouse(ptk, 393, 41, (Cur_Width - 394), 136) == 1)
                {
                    lt_index[ptk->Scopish]--;
                    ptk->gui_action = GUI_CMD_FILELIST_SCROLL;
                }
                break;
        }

        // Scroll the sequences
        if(ptk->userscreen == USER_SCREEN_SEQUENCER)
        {
            if(zcheckMouse(ptk, 257, (Cur_Height - 134), 26, 90) ||
               zcheckMouse(ptk, 89, (Cur_Height - 134), 26, 90) ||
               zcheckMouse(ptk, 120, (Cur_Height - 134), 132, 90))
            {
                ptk->gui_action = GUI_CMD_PREVIOUS_POSITION;
            }
        }

        // Scroll the knobs
        Mouse_Wheel_303_Ed(ptk, ptk->MouseWheel_Multiplier);

        Mouse_Wheel_Sample_Ed(ptk, ptk->MouseWheel_Multiplier);
    }

    // mouse wheel down
    if(Mouse.wheel == -1)
    {
        // Scroll the pattern
        Mouse_Wheel_Pattern_Ed(ptk, ptk->MouseWheel_Multiplier, !Songplaying && !is_recording);

        switch(ptk->Scopish)
        {
            case SCOPE_ZONE_INSTR_LIST:
            case SCOPE_ZONE_SYNTH_LIST:

                // Scroll the instruments/synths lists
                if(zcheckMouse(ptk, 393, 41, (Cur_Width - 394), 136) == 1)
                {
                    Instrs_index++;
                    ptk->gui_action = GUI_CMD_INSTR_SYNTH_SCROLL;
                }
                break;

            case SCOPE_ZONE_MOD_DIR:
            case SCOPE_ZONE_INSTR_DIR:
            case SCOPE_ZONE_PRESET_DIR:
            case SCOPE_ZONE_REVERB_DIR:
            case SCOPE_ZONE_MIDICFG_DIR:
            case SCOPE_ZONE_PATTERN_DIR:
            case SCOPE_ZONE_SAMPLE_DIR:

                // Scroll the files lists
                if(zcheckMouse(ptk, 393, 41, (Cur_Width - 394), 136) == 1)
                {
                    lt_index[ptk->Scopish]++;
                    ptk->gui_action = GUI_CMD_FILELIST_SCROLL;
                }
                break;
        }

        // Scroll the sequences
        if(ptk->userscreen == USER_SCREEN_SEQUENCER)
        {
            if(zcheckMouse(ptk, 257, (Cur_Height - 134), 26, 90) ||
               zcheckMouse(ptk, 89, (Cur_Height - 134), 26, 90) ||
               zcheckMouse(ptk, 120, (Cur_Height - 134), 132, 90))
            {
                ptk->gui_action = GUI_CMD_NEXT_POSITION;
            }
        }

        // Scroll the knobs
        Mouse_Wheel_303_Ed(ptk, -ptk->MouseWheel_Multiplier);
        Mouse_Wheel_Sample_Ed(ptk, -ptk->MouseWheel_Multiplier);
    }

    if(Mouse.button & MOUSE_LEFT_BUTTON)
    {
        switch(ptk->Scopish)
        {
            case SCOPE_ZONE_INSTR_LIST:
            case SCOPE_ZONE_SYNTH_LIST:
                if(zcheckMouse(ptk, Cur_Width - 17, 59, 16, 103 + 1)) ptk->gui_action = GUI_CMD_SET_INSTR_SYNTH_LIST_SLIDER;
                break;

            case SCOPE_ZONE_MOD_DIR:
            case SCOPE_ZONE_INSTR_DIR:
            case SCOPE_ZONE_PRESET_DIR:
            case SCOPE_ZONE_REVERB_DIR:
            case SCOPE_ZONE_MIDICFG_DIR:
            case SCOPE_ZONE_PATTERN_DIR:
            case SCOPE_ZONE_SAMPLE_DIR:
                if(zcheckMouse(ptk, Cur_Width - 17, 59, 16, 103 + 1)) ptk->gui_action = GUI_CMD_SET_FILES_LIST_SLIDER;
                break;
        }

        Mouse_Left_Repeat_Instrument_Ed(ptk);
        Mouse_Sliders_303_Ed(ptk);
        Mouse_Sliders_Sample_Ed(ptk);
        Mouse_Sliders_Instrument_Ed(ptk);
        Mouse_Sliders_Master_Ed(ptk);
        Mouse_Sliders_Synth_Ed(ptk);
        Mouse_Sliders_Track_Fx_Ed(ptk);
        Mouse_Sliders_Fx_Ed(ptk);
        Mouse_Sliders_Track_Ed(ptk);
        Mouse_Sliders_Master_Shuffle(ptk);
        Mouse_Sliders_Pattern_Ed(ptk);
        Mouse_Sliders_Reverb_Ed(ptk);
    }
    else
    {
        // leftbutton
        ptk->sas = FALSE;
    }

    // Check Zones for GUI clicks -----------------------------------
    
    Mouse_Reverb_Ed(ptk);

    if(Mouse.button_oneshot & MOUSE_LEFT_BUTTON)
    {
        // Modules dir.
        if(zcheckMouse(ptk, Cur_Width - 126, 24, 18, 16))
        {
            ptk->Scopish = SCOPE_ZONE_MOD_DIR;
            Draw_Scope_Files_Button(ptk);
        }

        // Instruments dir.
        if(zcheckMouse(ptk, Cur_Width - 108, 24, 18, 16))
        {
            ptk->Scopish = SCOPE_ZONE_INSTR_DIR;
            Draw_Scope_Files_Button(ptk);
        }

        // Samples dir.
        if(zcheckMouse(ptk, Cur_Width - 90, 24, 18, 16))
        {
            ptk->Scopish = SCOPE_ZONE_SAMPLE_DIR;
            Draw_Scope_Files_Button(ptk);
        }

        // Presets dir.
        if(zcheckMouse(ptk, Cur_Width - 72, 24, 18, 16))
        {
            ptk->Scopish = SCOPE_ZONE_PRESET_DIR;
            Draw_Scope_Files_Button(ptk);
        }

        // Reverbs dir.
        if(zcheckMouse(ptk, Cur_Width - 54, 24, 18, 16))
        {
            ptk->Scopish = SCOPE_ZONE_REVERB_DIR;
            Draw_Scope_Files_Button(ptk);
        }

        // Patterns dir.
        if(zcheckMouse(ptk, Cur_Width - 36, 24, 18, 16))
        {
            ptk->Scopish = SCOPE_ZONE_PATTERN_DIR;
            Draw_Scope_Files_Button(ptk);
        }

        // Patterns dir.
        if(zcheckMouse(ptk, Cur_Width - 18, 24, 18, 16))
        {
            ptk->Scopish = SCOPE_ZONE_MIDICFG_DIR;
            Draw_Scope_Files_Button(ptk);
        }

        // Tracks scopes.
        if(zcheckMouse(ptk, Cur_Width - 54, 6, 18, 16))
        {
            if(ptk->Scopish != SCOPE_ZONE_SCOPE)
            {
                ptk->Scopish_LeftRight = FALSE;
                ptk->Scopish = SCOPE_ZONE_SCOPE;
                Draw_Scope_Files_Button(ptk);
            }
            if(ptk->Scopish == SCOPE_ZONE_SCOPE && ptk->Scopish_LeftRight == TRUE)
            {
                ptk->Scopish_LeftRight = FALSE;
                ptk->Scopish = SCOPE_ZONE_SCOPE;
                Draw_Scope_Files_Button(ptk);
            }
        }

        // Instruments list
        if(zcheckMouse(ptk, Cur_Width - 36, 6, 18, 16))
        {
            ptk->Scopish = SCOPE_ZONE_INSTR_LIST;
            Draw_Scope_Files_Button(ptk);
        }

        // Synths list
        if(zcheckMouse(ptk, MAX_PATT_SCREEN_X + 1, 6, 18, 16))
        {
            ptk->Scopish = SCOPE_ZONE_SYNTH_LIST;
            Draw_Scope_Files_Button(ptk);
        }

        Mouse_Left_303_Ed(ptk);
        Mouse_Left_Sample_Ed(ptk);

        // Change current instrument name
        if(zcheckMouse(ptk, 90, 108, 166, 16) && ptk->snamesel == INPUT_NONE)
        {
            ptk->snamesel = INPUT_INSTRUMENT_NAME;
            strcpy(ptk->cur_input_name, nameins[ptk->Current_Instrument]);
            sprintf(nameins[ptk->Current_Instrument], "");
            ptk->namesize = 0;
            ptk->gui_action = GUI_CMD_UPDATE_PATTERN_ED;
        }

        switch(ptk->Scopish)
        {
            case SCOPE_ZONE_INSTR_LIST:
            case SCOPE_ZONE_SYNTH_LIST:
                Files_List_Move(ptk, 1);

                // Select
                if(zcheckMouse(ptk, 393, 43, Cur_Width - 412, 133))
                {
                    ptk->gui_action = GUI_CMD_SET_INSTR_SYNTH_LIST_SELECT;
                }
                break;

            case SCOPE_ZONE_MOD_DIR:
            case SCOPE_ZONE_INSTR_DIR:
            case SCOPE_ZONE_PRESET_DIR:
            case SCOPE_ZONE_REVERB_DIR:
            case SCOPE_ZONE_PATTERN_DIR:
            case SCOPE_ZONE_MIDICFG_DIR:
            case SCOPE_ZONE_SAMPLE_DIR:
                Files_List_Move(ptk, 1);

                // Select a file
                if(zcheckMouse(ptk, 393, 43, Cur_Width - 412, 133))
                {
                    ptk->gui_action = GUI_CMD_SET_FILES_LIST_SELECT_FILE;
                }
                break;
        }

        if(zcheckMouse(ptk, 258, 126, 16, 16)) ptk->gui_action = GUI_CMD_LOWER_OCTAVE;
        if(zcheckMouse(ptk, 302, 126, 16, 16)) ptk->gui_action = GUI_CMD_HIGHER_OCTAVE;
        if(zcheckMouse(ptk, 90, 126, 16, 16)) ptk->gui_action = GUI_CMD_DECREASE_STEP_ADD;
        if(zcheckMouse(ptk, 134, 126, 16, 16)) ptk->gui_action = GUI_CMD_INCREASE_STEP_ADD;
        if(zcheckMouse(ptk, 258, 108, 16, 16)) ptk->gui_action = GUI_CMD_PREV_INSTR;
        if(zcheckMouse(ptk, 302, 108, 16, 16)) ptk->gui_action = GUI_CMD_NEXT_INSTR;

        // --- Player --------------------------------------------

        // Play song from top
        if(zcheckMouse(ptk, 8, 28, 39, 16))
        {
            plx = 0;
            Pattern_Line = 0;
            ptk->gui_action = GUI_CMD_PLAY_SONG;
        }
        // Play pattern from top
        if(zcheckMouse(ptk, 49, 28, 39, 16))
        {
            plx = 1;
            Pattern_Line = 0;
            ptk->gui_action = GUI_CMD_PLAY_SONG;
        }
        if(zcheckMouse(ptk, 8, 46, 80, 16))
        {
            ptk->gui_action = GUI_CMD_STOP_SONG;
        }

        if(zcheckMouse(ptk, 8, 64, 80, 16))
        {
            sr_isrecording ^= 1;
            ptk->gui_action = GUI_CMD_RECORD_303;
        }

        if(zcheckMouse(ptk, 8, 82, 80, 16))
        {
            ptk->gui_action = GUI_CMD_EDIT_MODE;
        }

        // --- Sequence -----------------------------------------

        if(zcheckMouse(ptk, 188, 28, 16, 16))
        {
            ptk->gui_action = GUI_CMD_PREVIOUS_POSITION;
        }
        if(zcheckMouse(ptk, 232, 28, 16, 16))
        {
            ptk->gui_action = GUI_CMD_NEXT_POSITION;
        }
        if(zcheckMouse(ptk, 188, 46, 16, 16) && ptk->pSequence[Cur_Position] > 0)
        {
            ptk->gui_action = GUI_CMD_PREV_PATT;
        }
        if(zcheckMouse(ptk, 232, 46, 16, 16) && ptk->pSequence[Cur_Position] < 254)
        {
            ptk->gui_action = GUI_CMD_NEXT_PATT;
        }
        if(zcheckMouse(ptk, 188, 64, 16, 16) && ptk->Song_Length > 1)
        {
            ptk->gui_action = GUI_CMD_REDUCE_SONG_LENGTH;
        }
        if(zcheckMouse(ptk, 232, 64, 16, 16) && ptk->Song_Length < 255)
        {
            ptk->gui_action = GUI_CMD_INCREASE_SONG_LENGTH;
        }

        // Decrease the number of lines for this pattern
        if(zcheckMouse(ptk, 188, 82, 16, 16) && ptk->patternLines[ptk->pSequence[Cur_Position]] > 1)
        {
            ptk->patternLines[ptk->pSequence[Cur_Position]]--;
            if(Pattern_Line >= ptk->patternLines[ptk->pSequence[Cur_Position]]) Pattern_Line = ptk->patternLines[ptk->pSequence[Cur_Position]] - 1;
            ptk->gui_action = GUI_CMD_SET_PATTERN_LENGTH;
        }
        // Increase the number of lines for this pattern
        if(zcheckMouse(ptk, 232, 82, 16, 16) && ptk->patternLines[ptk->pSequence[Cur_Position]] < 128)
        {
            ptk->patternLines[ptk->pSequence[Cur_Position]]++;
            ptk->gui_action = GUI_CMD_SET_PATTERN_LENGTH;
        }

        // --- Song Settings --------------------------------------

        // Reduce the number of tracks
        if(zcheckMouse(ptk, 324, 28, 16, 16) && Songtracks > 1)
        {
            Songtracks--;
            if(Songtracks < 1) Songtracks = 1;
            ptk->gui_action = GUI_CMD_CHANGE_TRACKS_NBR;
            ptk->teac = 4;
        }
        // Increase the number of tracks
        if(zcheckMouse(ptk, 368, 28, 16, 16) && Songtracks < 16)
        {
            Songtracks++;
            if(Songtracks > 16) Songtracks = 16;
            ptk->gui_action = GUI_CMD_CHANGE_TRACKS_NBR;
            ptk->teac = 4;
        }

        // Delete the current track
        if(zcheckMouse(ptk, 313, 28, 9, 16) && Songtracks >= 1 && !Songplaying)
        {
            Songtracks--;
            if(Songtracks < 1)
            {
                Songtracks = 1;
                // Just clear it
                Reset_Track(ptk, ptk->pSequence[Song_Position], ptk->Track_Under_Caret);
            }
            else
            {
                Delete_Track(ptk);
            }
            ptk->gui_action = GUI_CMD_DELETE_TRACK;
            ptk->teac = 4;
        }
        // Insert a track at current position
        if(zcheckMouse(ptk, 302, 28, 9, 16) && Songtracks < 16 && !Songplaying)
        {
            Songtracks++;
            if(Songtracks > 16)
            {
                Songtracks = 16;
            }
            else
            {
                Insert_Track(ptk);
                ptk->gui_action = GUI_CMD_INSERT_TRACK;
                ptk->teac = 4;
            }
        }
        
        // Reduce the number of BPM
        if(zcheckMouse(ptk, 324, 46, 16, 16) && !Songplaying)
        {
            BeatsPerMin--;
            ptk->gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            ptk->teac = 1;
        }

        // Increase the number of BPM
        if(zcheckMouse(ptk, 368, 46, 16, 16) && !Songplaying)
        {
            BeatsPerMin++;
            ptk->gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            ptk->teac = 1;
        }

        // Decrease the number of TPB
        if(zcheckMouse(ptk, 324, 64, 16, 16) && !Songplaying)
        {
            TicksPerBeat--;
            ptk->gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            ptk->teac = 2;
        }

        // Increase the number of TPB
        if(zcheckMouse(ptk, 368, 64, 16, 16) && !Songplaying)
        {
            TicksPerBeat++;
            ptk->gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            ptk->teac = 2;
        }

        // Delete instrument
        if(zcheckMouse(ptk, 320, 108, 64, 16))
        {
            Display_Requester(ptk, &Delete_Requester, GUI_CMD_DELETE_INSTRUMENT);
        }

        // Zoom'em small
        if(zcheckMouse(ptk, 332, 126, 16, 16))
        {
            for(i = 0; i < Songtracks; i++)
            {
                Set_Track_Zoom(ptk, i, TRACK_SMALL);
            }
            Actupated(ptk, 0);
            Set_Track_Slider(ptk, ptk->gui_track);
        }
        // Zoom'em normal
        if(zcheckMouse(ptk, 332 + (18 * 1), 126, 16, 16))
        {
            for(i = 0; i < Songtracks; i++)
            {
                Set_Track_Zoom(ptk, i, TRACK_MEDIUM);
            }
            Actupated(ptk, 0);
            Set_Track_Slider(ptk, ptk->gui_track);
        }
        // Zoom'em large
        if(zcheckMouse(ptk, 332 + (18 * 2), 126, 16, 16))
        {
            for(i = 0; i < Songtracks; i++)
            {
                Set_Track_Zoom(ptk, i, TRACK_LARGE);
            }
            Actupated(ptk, 0);
            Set_Track_Slider(ptk, ptk->gui_track);
        }

        // Select track
        if(zcheckMouse(ptk, 8, 152, 61, 10))
        {
            Select_Track_Block(ptk);
        }
        // Select note
        if(zcheckMouse(ptk, 8, 164, 61, 10))
        {
            Select_Note_Block(ptk);
        }
        // Cut
        if(zcheckMouse(ptk, 8 + 63, 152, 61, 10))
        {
            if(block_start_track[Curr_Buff_Block] != -1 &&
               block_end_track[Curr_Buff_Block] != -1)
            {
                Cut_Selection(ptk, Cur_Position);
            }
        }
        // Copy
        if(zcheckMouse(ptk, 8 + 63, 164, 61, 10))
        {
            if(block_start_track[Curr_Buff_Block] != -1 &&
               block_end_track[Curr_Buff_Block] != -1)
            {
                Copy_Selection(ptk, Cur_Position);
            }
        }
        // Paste
        if(zcheckMouse(ptk, 8 + (63 * 2), 152, 61, 10))
        {
            if(block_start_track_nibble[Curr_Buff_Block] != -1 &&
               block_end_track_nibble[Curr_Buff_Block] != -1 &&
               is_editing)
            {
                Paste_Block(ptk, Cur_Position, Paste_Across, TRUE);
            }
        }
        // Delete
        if(zcheckMouse(ptk, 8 + (63 * 2), 164, 61, 10))
        {
            if(is_editing)
            {
                Delete_Selection(ptk, Cur_Position);
                Actupated(ptk, 0);
            }
        }
        // Interpolate
        if(zcheckMouse(ptk, 8 + (63 * 3), 152, 61, 10))
        {
            if(is_editing) Interpolate_Block(ptk, Cur_Position);
        }
        // Randomize
        if(zcheckMouse(ptk, 8 + (63 * 3), 164, 61, 10))
        {
            if(is_editing) Randomize_Block(ptk, Cur_Position);
        }
        // Semitone up
        if(zcheckMouse(ptk, 8 + (63 * 4), 152, 61, 10))
        {
            if(is_editing) Semitone_Up_Block(ptk, Cur_Position);
        }
        // Semitone down
        if(zcheckMouse(ptk, 8 + (63 * 4), 164, 61, 10))
        {
            if(is_editing) Semitone_Down_Block(ptk, Cur_Position);
        }
        // Octave up
        if(zcheckMouse(ptk, 8 + (63 * 5), 152, 61, 10))
        {
            if(is_editing) Octave_Up_Block(ptk, Cur_Position);
        }
        // Octave down
        if(zcheckMouse(ptk, 8 + (63 * 5), 164, 61, 10))
        {
            if(is_editing) Octave_Down_Block(ptk, Cur_Position);
        }

        // Switch small / large patterns
        int Add_Offset = Patterns_Lines_Offset;
        if(zcheckMouse(ptk, 0, (Cur_Height - 171) + Add_Offset, 18, 16))
        {
            ptk->gui_action = GUI_CMD_SELECT_LARGE_PATTERNS;
        }

        // Exit
        if(zcheckMouse(ptk, 0, 6, 16, 16))
        {
            ptk->gui_action = GUI_CMD_EXIT;
        }

        if(zcheckMouse(ptk, 20, (Cur_Height - 171) + Add_Offset, 62, 16) && (ptk->userscreen != USER_SCREEN_SEQUENCER || Patterns_Lines_Offset)) ptk->gui_action = GUI_CMD_SELECT_SEQUENCER;
        if(zcheckMouse(ptk, 20 + (TAB_LARG * 1), (Cur_Height - 171) + Add_Offset, TAB_LARG - 2, 16) && (ptk->userscreen != USER_SCREEN_INSTRUMENT_EDIT || Patterns_Lines_Offset))
        {
            ptk->gui_action = GUI_CMD_SELECT_INSTRUMENT_EDIT;
            ptk->seditor = 0;
        }
        if(zcheckMouse(ptk, 20 + (TAB_LARG * 2), (Cur_Height - 171) + Add_Offset, TAB_LARG - 2, 16) && (ptk->userscreen != USER_SCREEN_SYNTH_EDIT || Patterns_Lines_Offset)) ptk->gui_action = GUI_CMD_SELECT_SYNTH_EDIT;
        if(zcheckMouse(ptk, 20 + (TAB_LARG * 3), (Cur_Height - 171) + Add_Offset, TAB_LARG - 2, 16) && (ptk->userscreen != USER_SCREEN_SAMPLE_EDIT || Patterns_Lines_Offset)) ptk->gui_action = GUI_CMD_SELECT_SAMPLE_EDIT;
        if(zcheckMouse(ptk, 20 + (TAB_LARG * 4), (Cur_Height - 171) + Add_Offset, TAB_LARG - 2, 16) && (ptk->userscreen != USER_SCREEN_TB303_EDIT || Patterns_Lines_Offset)) ptk->gui_action = GUI_CMD_SELECT_TB303_EDIT;
        if(zcheckMouse(ptk, 20 + (TAB_LARG * 5), (Cur_Height - 171) + Add_Offset, TAB_LARG - 2, 16) && (ptk->userscreen != USER_SCREEN_TRACK_EDIT || Patterns_Lines_Offset)) ptk->gui_action = GUI_CMD_SELECT_TRACK_EDIT;
        if(zcheckMouse(ptk, 20 + (TAB_LARG * 6), (Cur_Height - 171) + Add_Offset, TAB_LARG - 2, 16) && (ptk->userscreen != USER_SCREEN_TRACK_FX_EDIT || Patterns_Lines_Offset)) ptk->gui_action = GUI_CMD_SELECT_TRACK_FX_EDIT;
        if(zcheckMouse(ptk, 20 + (TAB_LARG * 7), (Cur_Height - 171) + Add_Offset, TAB_LARG - 2, 16) && (ptk->userscreen != USER_SCREEN_FX_SETUP_EDIT || Patterns_Lines_Offset))
        {
            ptk->gui_action = GUI_CMD_SELECT_FX_EDIT;
            ptk->teac = 0;
        }
        if(zcheckMouse(ptk, 20 + (TAB_LARG * 8), (Cur_Height - 171) + Add_Offset, TAB_LARG - 2, 16) && (ptk->userscreen != USER_SCREEN_REVERB_EDIT || Patterns_Lines_Offset)) ptk->gui_action = GUI_CMD_SELECT_REVERB_EDIT;
        if(zcheckMouse(ptk, 20 + (TAB_LARG * 9), (Cur_Height - 171) + Add_Offset, TAB_LARG - 2, 16) && (ptk->userscreen != USER_SCREEN_DISKIO_EDIT || Patterns_Lines_Offset)) ptk->gui_action = GUI_CMD_SELECT_DISKIO_EDIT;
        if(zcheckMouse(ptk, 20 + (TAB_LARG * 10), (Cur_Height - 171) + Add_Offset, TAB_LARG - 2, 16) && (ptk->userscreen != USER_SCREEN_SETUP_EDIT || Patterns_Lines_Offset)) ptk->gui_action = GUI_CMD_SELECT_SCREEN_SETUP_EDIT;
        if(zcheckMouse(ptk, 
					20 + (TAB_LARG * 11), (Cur_Height - 171) + Add_Offset, 
					TAB_LARG - 2, 16) && 
				(ptk->userscreen != USER_SCREEN_SETUP_MIDI 
				 || Patterns_Lines_Offset)) {
			printf("hey you guys!\n");
			//ptk->gui_action = GUI_CMD_SELECT_MIDI_SETUP;
		}

        Mouse_Left_Track_Fx_Ed(ptk);
        Mouse_Left_Sequencer_Ed(ptk);
        Mouse_Left_Reverb_Ed(ptk);
        //Mouse_Left_Midi_Ed(ptk);
        Mouse_Left_DiskIO_Ed(ptk);
        Mouse_Left_Track_Ed(ptk);
        Mouse_Left_Instrument_Ed(ptk);
        Mouse_Left_Fx_Ed(ptk);
        Mouse_Left_Synth_Ed(ptk);
        Mouse_Left_Master_Ed(ptk);
        Mouse_Left_Pattern_Ed(ptk);
    }

    // Middle mouse button
    if(Mouse.button_oneshot & MOUSE_MIDDLE_BUTTON)
    {
        Mouse_Middle_Sample_Ed(ptk);
    }

    // Right mouse button
    if(Mouse.button_oneshot & MOUSE_RIGHT_BUTTON)
    {
        Reset_Pattern_Scrolling_Horiz(ptk);
        Mouse_Right_Pattern_Ed(ptk);

        if(zcheckMouse(ptk, 188, 82, 16, 16))
        {
            int ltp = ptk->patternLines[ptk->pSequence[Cur_Position]];
            ltp -= 8;
            if(ltp < 1) ltp = 1;
            ptk->patternLines[ptk->pSequence[Cur_Position]] = ltp;
            if(Pattern_Line >= ptk->patternLines[ptk->pSequence[Cur_Position]]) Pattern_Line = ptk->patternLines[ptk->pSequence[Cur_Position]] - 1;
            ptk->gui_action = GUI_CMD_SET_PATTERN_LENGTH;
        }

        if(zcheckMouse(ptk, 232, 82, 16, 16))
        {
            int ltp = ptk->patternLines[ptk->pSequence[Cur_Position]];
            ltp += 8;
            if(ltp > 128) ltp = 128;
            ptk->patternLines[ptk->pSequence[Cur_Position]] = ltp;
            ptk->gui_action = GUI_CMD_SET_PATTERN_LENGTH;
        }

        if(zcheckMouse(ptk, 258, 108, 16, 16) == 1)
        {
            ptk->gui_action = GUI_CMD_PREVIOUS_16_INSTR;
        }
        if(zcheckMouse(ptk, 302, 108, 16, 16) == 1)
        {
            ptk->gui_action = GUI_CMD_NEXT_16_INSTR;
        }

        // Play song from current position
        if(zcheckMouse(ptk, 8, 28, 39, 16))
        {
            plx = 0;
            ptk->gui_action = GUI_CMD_PLAY_SONG;
        }
        // Play pattern from current position
        if(zcheckMouse(ptk, 49, 28, 39, 16))
        {
            plx = 1;
            ptk->gui_action = GUI_CMD_PLAY_SONG;
        }

        switch(ptk->Scopish)
        {
            case SCOPE_ZONE_INSTR_LIST:
            case SCOPE_ZONE_SYNTH_LIST:
                Files_List_Move(ptk, 10);
                break;


            case SCOPE_ZONE_MOD_DIR:
            case SCOPE_ZONE_INSTR_DIR:
            case SCOPE_ZONE_PRESET_DIR:
            case SCOPE_ZONE_REVERB_DIR:
            case SCOPE_ZONE_PATTERN_DIR:
            case SCOPE_ZONE_MIDICFG_DIR:
            case SCOPE_ZONE_SAMPLE_DIR:
                Files_List_Move(ptk, 10);
                break;
        }

        // Reduce the number of tracks
        if(zcheckMouse(ptk, 324, 28, 16, 16) && Songtracks > 1)
        {
            Songtracks -= 5;
            if(Songtracks < 1) Songtracks = 1;
            ptk->gui_action = GUI_CMD_CHANGE_TRACKS_NBR;
            ptk->teac = 4;
        }
        // Increase the number of tracks
        if(zcheckMouse(ptk, 368, 28, 16, 16) && Songtracks < 16)
        {
            Songtracks += 5;
            if(Songtracks > 16) Songtracks = 16;
            ptk->gui_action = GUI_CMD_CHANGE_TRACKS_NBR;
            ptk->teac = 4;
        }

        // Reduce the number of BPM by 16
        if(zcheckMouse(ptk, 324, 46, 16, 16) && !Songplaying)
        {
            BeatsPerMin -= 16;
            ptk->gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            ptk->teac = 1;
        }
        // Increase the number of BPM by 16
        if(zcheckMouse(ptk, 368, 46, 16, 16) && !Songplaying)
        {
            BeatsPerMin += 16;
            ptk->gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            ptk->teac = 1;
        }

        // Decrease the number of TPB
        if(zcheckMouse(ptk, 324, 64, 16, 16) && !Songplaying)
        {
            TicksPerBeat -= 10;
            ptk->gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            ptk->teac = 2;
        }

        // Increase the number of TPB
        if(zcheckMouse(ptk, 368, 64, 16, 16) && !Songplaying)
        {
            TicksPerBeat += 10;
            ptk->gui_action = GUI_CMD_CHANGE_BPM_TICKS_NBR;
            ptk->teac = 2;
        }

        // ptk->Song_Length - 10
        if(zcheckMouse(ptk, 188, 64, 16, 16) == 1 && ptk->Song_Length != 1)
        {
            int tLength = ptk->Song_Length;
            tLength -= 10;
            if(tLength < 1) tLength = 1;
            ptk->Song_Length = tLength;
            Actupated(ptk, 0);
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }
        // ptk->Song_Length + 10
        if(zcheckMouse(ptk, 232, 64, 16, 16) == 1 && ptk->Song_Length != 255)
        {
            int tLength = ptk->Song_Length;
            tLength += 10;
            if(tLength > 255) tLength = 255;
            ptk->Song_Length = tLength;
            Actupated(ptk, 0);
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // -10 positions
        if(zcheckMouse(ptk, 188, 28, 16, 16) == 1)
        {
            ptk->gui_action = GUI_CMD_REDUCE_POSITIONS_10;
        }
        // +10 positions
        if(zcheckMouse(ptk, 232, 28, 16, 16) == 1)
        {
            ptk->gui_action = GUI_CMD_INCREASE_POSITIONS_10;
        }

        // -10 patterns
        if(zcheckMouse(ptk, 188, 46, 16, 16) == 1)
        {
            ptk->gui_action = GUI_CMD_REDUCE_PATTERNS_10;
        }
        // +10 patterns
        if(zcheckMouse(ptk, 232, 46, 16, 16) == 1)
        {
            ptk->gui_action = GUI_CMD_INCREASE_PATTERNS_10;
        }

        // Record mode
        if(zcheckMouse(ptk, 8, 82, 80, 16))
        {
            ptk->gui_action = GUI_CMD_RECORD_MODE;
        }

        // Select pattern
        if(zcheckMouse(ptk, 8, 152, 61, 10))
        {
            Select_Pattern_Block(ptk);
        }
        // Select notes
        if(zcheckMouse(ptk, 8, 164, 61, 10))
        {
            Select_All_Notes_Block(ptk);
        }
        // Semitone up
        if(zcheckMouse(ptk, 8 + (63 * 4), 152, 61, 10))
        {
            if(is_editing) Instrument_Semitone_Up_Block(ptk, Cur_Position);
        }
        // Semitone down
        if(zcheckMouse(ptk, 8 + (63 * 4), 164, 61, 10))
        {
            if(is_editing) Instrument_Semitone_Down_Block(ptk, Cur_Position);
        }
        // Instrument octave up
        if(zcheckMouse(ptk, 8 + (63 * 5), 152, 61, 10))
        {
            if(is_editing) Instrument_Octave_Up_Block(ptk, Cur_Position);
        }
        // Instrument octave down
        if(zcheckMouse(ptk, 8 + (63 * 5), 164, 61, 10))
        {
            if(is_editing) Instrument_Octave_Down_Block(ptk, Cur_Position);
        }

        // Stereo scopes.
        if(zcheckMouse(ptk, Cur_Width - 54, 6, 18, 16))
        {
            if(ptk->Scopish != SCOPE_ZONE_SCOPE)
            {
                ptk->Scopish_LeftRight = TRUE;
                ptk->Scopish = SCOPE_ZONE_SCOPE;
                Draw_Scope_Files_Button(ptk);
            }
            if(ptk->Scopish == SCOPE_ZONE_SCOPE && ptk->Scopish_LeftRight == FALSE)
            {
                ptk->Scopish_LeftRight = TRUE;
                ptk->Scopish = SCOPE_ZONE_SCOPE;
                Draw_Scope_Files_Button(ptk);
            }
        }

        Mouse_Right_Fx_Ed(ptk);
        Mouse_Right_303_Ed(ptk);
        Mouse_Right_Reverb_Ed(ptk);
        Mouse_Right_Midi_Ed(ptk);
        Mouse_Right_DiskIO_Ed(ptk);
        Mouse_Right_Sample_Ed(ptk);
        Mouse_Right_Track_Ed(ptk);
        Mouse_Right_Instrument_Ed(ptk);
        Mouse_Right_Sequencer_Ed(ptk);
        Mouse_Right_Synth_Ed(ptk);
        Mouse_Right_Master_Ed(ptk);

        // Play a .wav
        switch(ptk->Scopish)
        {
            case SCOPE_ZONE_INSTR_LIST:
            case SCOPE_ZONE_SYNTH_LIST:
                break;

            case SCOPE_ZONE_MOD_DIR:
            case SCOPE_ZONE_INSTR_DIR:
            case SCOPE_ZONE_PRESET_DIR:
            case SCOPE_ZONE_REVERB_DIR:
            case SCOPE_ZONE_PATTERN_DIR:
            case SCOPE_ZONE_MIDICFG_DIR:
            case SCOPE_ZONE_SAMPLE_DIR:

                // Play the file
                if(zcheckMouse(ptk, 393, 43, Cur_Width - 412, 133))
                {
                    ptk->gui_action = GUI_CMD_SET_FILES_LIST_PLAY_WAV;
                }
                break;
        }


    } // RIGHT MOUSE

    if(Mouse.button & MOUSE_RIGHT_BUTTON)
    {
        Mouse_Sliders_Right_Pattern_Ed(ptk);
        Mouse_Sliders_Right_Instrument_Ed(ptk);
        Mouse_Sliders_Right_Reverb_Ed(ptk);
    }

}

// ------------------------------------------------------
// Search the first free slot in the sequences pool
int Search_Free_Pattern(ptk_data *ptk)
{
    int i;
    int j;
    int found;

    for(i = 0; i < ptk->Song_Length; i++)
    {
        found = FALSE;
        for(j = 0; j < 128; j++)
        {
            if(ptk->pSequence[j] == i)
            {
                found = TRUE;
                break;
            }
        }
        if(!found) return(i);
    }
    return(-1);
}

// ------------------------------------------------------
// Next pattern/position
int Next_Line_Pattern_Auto(ptk_data *ptk, int *position, int lines, int *line)
{
    int new_pattern;
    int max_value;

    if(*line >= lines)
    {
        // Normal end of pattern
        *line = *line - lines;
        *position += 1;
        if(*position >= ptk->Song_Length - 1)
        {
            ptk->Song_Length++;
            max_value = ptk->Song_Length;
            if(max_value > 255)
            {
                ptk->Song_Length = 255;
                Song_Position = 0;
            }
            // Alloc a new pattern position
            new_pattern = Search_Free_Pattern(ptk);
            if(new_pattern > -1) ptk->pSequence[*position] = new_pattern;
        }
    }
    return(*position);
}

// ------------------------------------------------------
// Return the track associated with the midi channel
int Get_Midi_Channel(int midi_channel)
{
    //int i;

    //for(i = 0; i < MAX_TRACKS; i++)
    //{
    //    if(midi_channel == ptk->CHAN_MIDI_PRG[i])
    //    {
    //        return(i);
    //    }
    //}
    return(-1);
}

// ------------------------------------------------------
// Return the first track which isn't allocated in the midi channels pool
int Get_Free_Midi_Sub_Channel(int track)
{
    int i;
    for(i = 0; i < MAX_POLYPHONY; i++)
    {
        if(Alloc_midi_Channels[track][i] == 0) return(i);
    }
    return(-1);
}

// ------------------------------------------------------
// Search & return the midi channel corresponding to the given data
int Search_Corresponding_Midi_Sub_Channel(int track, Uint32 Datas)
{
    int i;

    for(i = 0; i < MAX_TRACKS; i++)
    {
        if(Alloc_midi_Channels[track][i] == (Datas & 0x00ff00))
        {
            return(i);
        }
    }
    return(-1);
}

// ------------------------------------------------------
// Display the current beat time in milliseconds
void Display_Beat_Time(void)
{
    char String[64];
    sprintf(String, "1/4 Beat Time: %d ms.", (int) (15000.0f / (float) BeatsPerMin));
    Gui_Clear_Array(268, 86, 120, 16);
    PrintString(268, 86, USE_FONT, String);
}

// ------------------------------------------------------
// Display master compressor
void Display_Master_Comp(void)
{
    char string[64];

    Gui_Draw_Button_Box(159, 6, 54, 16, "Threshold", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
    Realslider_Size(159 + 54, 6, 50, (int) (mas_comp_threshold_Master * 0.5f), TRUE);
    if(mas_comp_ratio_Master <= 0.01f)
    {
        sprintf(string, "Off");
    }
    else
    {
        sprintf(string, "%d%%", (int) mas_comp_threshold_Master);
    }
    Print_String(string, 159 + 54, 8, 67, BUTTON_TEXT_CENTERED);

    Gui_Draw_Button_Box(283, 6, 41, 16, "Ratio", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
    Realslider_Size(283 + 41, 6, 50, (int) (mas_comp_ratio_Master * 0.5f), TRUE);
    if(mas_comp_ratio_Master <= 0.01f)
    {
        sprintf(string, "Off");
    }
    else
    {
        sprintf(string, "%d%%", (int) mas_comp_ratio_Master);
    }
    Print_String(string, 283 + 41, 8, 67, BUTTON_TEXT_CENTERED);
}

// ------------------------------------------------------
// Display master volume
void Display_Master_Volume(void)
{
    char String[64];

    if(mas_vol < 0.01f) mas_vol = 0.01f;
    if(mas_vol > 1.0f) mas_vol = 1.0f;
    Gui_Draw_Button_Box(394, 6, 44, 16, "Mst Vol.", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
    Realslider(394 + 44, 6, (int) (mas_vol * 128.0f), TRUE);
    sprintf(String, "%d%%", (int) (mas_vol * 100.0f));
    Print_String(String, 394 + 44, 8, 145, BUTTON_TEXT_CENTERED);
}

// ------------------------------------------------------
// Display the shuffle amount
void Display_Shuffle(void)
{
    char string[64];

    if(shuffle > 100) shuffle = 100;
    if(shuffle < 0) shuffle = 0;
    Gui_Draw_Button_Box(586, 6, 40, 16, "Shuffle", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);

    Realslider_Size(586 + 40, 6, 100, shuffle, TRUE);
    sprintf(string, "%d%%", shuffle);
    Print_String(string, 586 + 40, 8, 116, BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(746, 6, Cur_Width - 802, 16, "", BUTTON_NORMAL | BUTTON_DISABLED);
}

// ------------------------------------------------------
// Handle the mouse event of the top bar
void Mouse_Sliders_Master_Shuffle(ptk_data *ptk)
{
    // Compressor threshold
    if(zcheckMouse(ptk, 213, 6, 67, 18))
    {
        Mas_Compressor_Set_Variables_Master(ptk, (Mouse.x - 223.0f) * 2.0f, mas_comp_ratio_Master);
        Display_Master_Comp();
    }

    // Compressor ratio
    if(zcheckMouse(ptk, 324, 6, 67, 18))
    {
        Mas_Compressor_Set_Variables_Master(ptk, mas_comp_threshold_Master, (Mouse.x - 334.0f) * 2.0f);
        Display_Master_Comp();
    }
    
    // Master volume
    if(zcheckMouse(ptk, 438, 6, 148, 18))
    {
        // Normalized
        mas_vol = (Mouse.x - 448.0f) / 128.0f;
        Display_Master_Volume();
    }

    // Shuffle
    if(zcheckMouse(ptk, 586 + 40, 6, 120, 18))
    {
        shuffle = (int) ((Mouse.x - (586 + 40 + 10)));
        Display_Shuffle();
    }
}

void Actualize_Master(ptk_data *ptk, char gode)
{
    if(gode == 0 || gode == 1 || gode == 2)
    {
        if(BeatsPerMin < 20) BeatsPerMin = 20;
        if(BeatsPerMin > 255) BeatsPerMin = 255;
        Switch_Cmd_Playing(ptk, !Songplaying);
        //Gui_Draw_Arrows_Number_Box(324, 46, BeatsPerMin, (Songplaying ? BUTTON_NORMAL | BUTTON_DISABLED : BUTTON_NORMAL) |
          //                                               BUTTON_TEXT_CENTERED | (Songplaying ? 0 : BUTTON_RIGHT_MOUSE));
    }

    if(gode == 0 || gode == 2)
    {
        if(TicksPerBeat < 1) TicksPerBeat = 1;
        if(TicksPerBeat > 32) TicksPerBeat = 32;
        if((TicksPerBeat + 1) < ptk->Ticks_Synchro_Left)
        {
            ptk->Ticks_Synchro_Left = TicksPerBeat + 1;
            Actualize_Fx_Ed(ptk, 10);
            Actualize_Fx_Ed(ptk, 11);
        }
        if((TicksPerBeat + 1) < ptk->Ticks_Synchro_Right)
        {
            ptk->Ticks_Synchro_Right = TicksPerBeat + 1;
            Actualize_Fx_Ed(ptk, 10);
            Actualize_Fx_Ed(ptk, 11);
        }
        Switch_Cmd_Playing(ptk, !Songplaying);
//        Gui_Draw_Arrows_Number_Box2(324, 64, TicksPerBeat, (Songplaying ? BUTTON_NORMAL | BUTTON_DISABLED : BUTTON_NORMAL) |
  //                                                         BUTTON_TEXT_CENTERED | (Songplaying ? 0 : BUTTON_RIGHT_MOUSE));
    }

    if(gode == 0)
    {
        Gui_Draw_Arrows_Number_Box2(324, 28, Songtracks, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
    }

    if(gode == 4)
    {
        Gui_Draw_Arrows_Number_Box2(324, 28, Songtracks, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);

        if(ptk->userscreen == USER_SCREEN_SEQUENCER) Actualize_Seq_Ed(ptk, 0);
        Actupated(ptk, 0);
    }

    SamplesPerTick = (int) ((60 * MIX_RATE) / (BeatsPerMin * TicksPerBeat));
    float SamplesPerBeat = (float) MIX_RATE / (((float) BeatsPerMin * 4) / 60);
    SamplesPerSub = SamplesPerTick / 6;

    if(gode == 5)
    {
        Switch_Cmd_Playing(ptk, TRUE);
    }

    if(ptk->userscreen == USER_SCREEN_SETUP_EDIT) Actualize_Master_Ed(ptk, 3);
}

// ------------------------------------------------------
// Draw the stereo scope
typedef struct
{
    int nbr;
    int offset;
} POS_SCOPE, *LPPOS_SCOPE;

typedef struct
{
    int x_pos;
    int x_div;
    int x_mul;
    int y_pos;
    int y_large;
    int x_max;
} DAT_POS_SCOPE, *LPDAT_POS_SCOPE;

POS_SCOPE Scope_Table[] =
{
    { 0, 0 },
    { 1, 0 },
    { 2, 1 },
    { 3, 3 },
    { 4, 6 },
    { 5, 10 },
    { 6, 15 },
    { 7, 21 },
    { 8, 28 },
    { 9, 36 },
    { 10, 45 },
    { 11, 55 },
    { 12, 66 },
    { 13, 78 },
    { 14, 91 },
    { 15, 105 },
    { 16, 120 },
};

DAT_POS_SCOPE Scope_Table_Dats[] =
{
    // ---
    { 394, 0, 0, 135 / 2, 135 / 2, 1 },                     // 0

    // ---
    { 394, 0, 0, 135 / 2, 135 / 2, 2 },                     // 1
    { 394, 2, 1, 135 / 2, 135 / 2, 2 },                     // 2

    // ---
    { 394, 0, 0, 135 / 2, 135 / 2, 3 },                     // 3
    { 394, 3, 1, 135 / 2, 135 / 2, 3 },                     // 4
    { 394, 3, 2, 135 / 2, 135 / 2, 3 },                     // 5

    // ---
    { 394, 0, 0, 135 / 2, 135 / 2, 4 },                     // 6
    { 394, 4, 1, 135 / 2, 135 / 2, 4 },                     // 7
    { 394, 4, 2, 135 / 2, 135 / 2, 4 },                     // 8
    { 394, 4, 3, 135 / 2, 135 / 2, 4 },                     // 9

    // ---
    { 394, 0, 0, 135 / 2, 135 / 2, 5 },                     // 10
    { 394, 5, 1, 135 / 2, 135 / 2, 5 },                     // 11
    { 394, 5, 2, 135 / 2, 135 / 2, 5 },                     // 12
    { 394, 5, 3, 135 / 2, 135 / 2, 5 },                     // 13
    { 394, 5, 4, 135 / 2, 135 / 2, 5 },                     // 14

    // ---
    { 394, 0, 0, 135 / 4, 135 / 4, 3 },                     // 15
    { 394, 3, 1, 135 / 4, 135 / 4, 3 },                     // 16
    { 394, 3, 2, 135 / 4, 135 / 4, 3 },                     // 17
    { 394, 0, 0, (135 / 2) + (135 / 4) + 1, 135 / 4, 3 },   // 18
    { 394, 3, 1, (135 / 2) + (135 / 4) + 1, 135 / 4, 3 },   // 19
    { 394, 3, 2, (135 / 2) + (135 / 4) + 1, 135 / 4, 3 },   // 20

    // ---
    { 394, 0, 0, 135 / 4, 135 / 4, 4 },                     // 21
    { 394, 4, 1, 135 / 4, 135 / 4, 4 },                     // 22
    { 394, 4, 2, 135 / 4, 135 / 4, 4 },                     // 23
    { 394, 4, 3, 135 / 4, 135 / 4, 4 },                     // 24
    { 394, 0, 0, (135 / 2) + (135 / 4) + 1, 135 / 4, 3 },   // 25
    { 394, 3, 1, (135 / 2) + (135 / 4) + 1, 135 / 4, 3 },   // 26
    { 394, 3, 2, (135 / 2) + (135 / 4) + 1, 135 / 4, 3 },   // 27

    // ---
    { 394, 0, 0, 135 / 4, 135 / 4, 4 },                     // 28
    { 394, 4, 1, 135 / 4, 135 / 4, 4 },                     // 29
    { 394, 4, 2, 135 / 4, 135 / 4, 4 },                     // 30
    { 394, 4, 3, 135 / 4, 135 / 4, 4 },                     // 31
    { 394, 0, 0, (135 / 2) + (135 / 4) + 1, 135 / 4, 4 },   // 32
    { 394, 4, 1, (135 / 2) + (135 / 4) + 1, 135 / 4, 4 },   // 33
    { 394, 4, 2, (135 / 2) + (135 / 4) + 1, 135 / 4, 4 },   // 34
    { 394, 4, 3, (135 / 2) + (135 / 4) + 1, 135 / 4, 4 },   // 35

    // ---
    { 394, 0, 0, 135 / 4, 135 / 4, 5 },                     // 36
    { 394, 5, 1, 135 / 4, 135 / 4, 5 },                     // 37
    { 394, 5, 2, 135 / 4, 135 / 4, 5 },                     // 38
    { 394, 5, 3, 135 / 4, 135 / 4, 5 },                     // 39
    { 394, 5, 4, 135 / 4, 135 / 4, 5 },                     // 40
    { 394, 0, 0, (135 / 2) + (135 / 4) + 1, 135 / 4, 4 },   // 41
    { 394, 4, 1, (135 / 2) + (135 / 4) + 1, 135 / 4, 4 },   // 42
    { 394, 4, 2, (135 / 2) + (135 / 4) + 1, 135 / 4, 4 },   // 43
    { 394, 4, 3, (135 / 2) + (135 / 4) + 1, 135 / 4, 4 },   // 44

    // ---
    { 394, 0, 0, 135 / 4, 135 / 4, 5 },                     // 45
    { 394, 5, 1, 135 / 4, 135 / 4, 5 },                     // 46
    { 394, 5, 2, 135 / 4, 135 / 4, 5 },                     // 47
    { 394, 5, 3, 135 / 4, 135 / 4, 5 },                     // 48
    { 394, 5, 4, 135 / 4, 135 / 4, 5 },                     // 49
    { 394, 0, 0, (135 / 2) + (135 / 4) + 1, 135 / 4, 5 },   // 50
    { 394, 5, 1, (135 / 2) + (135 / 4) + 1, 135 / 4, 5 },   // 51
    { 394, 5, 2, (135 / 2) + (135 / 4) + 1, 135 / 4, 5 },   // 52
    { 394, 5, 3, (135 / 2) + (135 / 4) + 1, 135 / 4, 5 },   // 53
    { 394, 5, 4, (135 / 2) + (135 / 4) + 1, 135 / 4, 5 },   // 54

    // ---
    { 394, 0, 0, 135 / 6, 135 / 6, 4 },                     // 55
    { 394, 4, 1, 135 / 6, 135 / 6, 4 },                     // 56
    { 394, 4, 2, 135 / 6, 135 / 6, 4 },                     // 57
    { 394, 4, 3, 135 / 6, 135 / 6, 4 },                     // 58

    { 394, 0, 0, (135 / 2) + 1, 135 / 6, 4 },               // 59
    { 394, 4, 1, (135 / 2) + 1, 135 / 6, 4 },               // 60
    { 394, 4, 2, (135 / 2) + 1, 135 / 6, 4 },               // 61
    { 394, 4, 3, (135 / 2) + 1, 135 / 6, 4 },               // 62

    { 394, 0, 0, (135 / 2) + (135 / 3), 135 / 6, 3 },       // 63
    { 394, 3, 1, (135 / 2) + (135 / 3), 135 / 6, 3 },       // 64
    { 394, 3, 2, (135 / 2) + (135 / 3), 135 / 6, 3 },       // 65

    // ---
    { 394, 0, 0, 135 / 6, 135 / 6, 4 },                     // 66
    { 394, 4, 1, 135 / 6, 135 / 6, 4 },                     // 67
    { 394, 4, 2, 135 / 6, 135 / 6, 4 },                     // 68
    { 394, 4, 3, 135 / 6, 135 / 6, 4 },                     // 69

    { 394, 0, 0, (135 / 2) + 1, 135 / 6, 4 },               // 70
    { 394, 4, 1, (135 / 2) + 1, 135 / 6, 4 },               // 71
    { 394, 4, 2, (135 / 2) + 1, 135 / 6, 4 },               // 72
    { 394, 4, 3, (135 / 2) + 1, 135 / 6, 4 },               // 73

    { 394, 0, 0, (135 / 2) + (135 / 3), 135 / 6, 4 },       // 74
    { 394, 4, 1, (135 / 2) + (135 / 3), 135 / 6, 4 },       // 75
    { 394, 4, 2, (135 / 2) + (135 / 3), 135 / 6, 4 },       // 76
    { 394, 4, 3, (135 / 2) + (135 / 3), 135 / 6, 4 },       // 77

    // ---
    { 394, 0, 0, 135 / 6, 135 / 6, 5 },                     // 78
    { 394, 5, 1, 135 / 6, 135 / 6, 5 },                     // 79
    { 394, 5, 2, 135 / 6, 135 / 6, 5 },                     // 80
    { 394, 5, 3, 135 / 6, 135 / 6, 5 },                     // 81
    { 394, 5, 4, 135 / 6, 135 / 6, 5 },                     // 82

    { 394, 0, 0, (135 / 2) + 1, 135 / 6, 4 },               // 83
    { 394, 4, 1, (135 / 2) + 1, 135 / 6, 4 },               // 84
    { 394, 4, 2, (135 / 2) + 1, 135 / 6, 4 },               // 85
    { 394, 4, 3, (135 / 2) + 1, 135 / 6, 4 },               // 86

    { 394, 0, 0, (135 / 2) + (135 / 3), 135 / 6, 4 },       // 87
    { 394, 4, 1, (135 / 2) + (135 / 3), 135 / 6, 4 },       // 88
    { 394, 4, 2, (135 / 2) + (135 / 3), 135 / 6, 4 },       // 89
    { 394, 4, 3, (135 / 2) + (135 / 3), 135 / 6, 4 },       // 90

    // ---
    { 394, 0, 0, 135 / 6, 135 / 6, 5 },                     // 91
    { 394, 5, 1, 135 / 6, 135 / 6, 5 },                     // 92
    { 394, 5, 2, 135 / 6, 135 / 6, 5 },                     // 93
    { 394, 5, 3, 135 / 6, 135 / 6, 5 },                     // 94
    { 394, 5, 4, 135 / 6, 135 / 6, 5 },                     // 95

    { 394, 0, 0, (135 / 2) + 1, 135 / 6, 5 },               // 96
    { 394, 5, 1, (135 / 2) + 1, 135 / 6, 5 },               // 97
    { 394, 5, 2, (135 / 2) + 1, 135 / 6, 5 },               // 98
    { 394, 5, 3, (135 / 2) + 1, 135 / 6, 5 },               // 99
    { 394, 5, 4, (135 / 2) + 1, 135 / 6, 5 },               // 100

    { 394, 0, 0, (135 / 2) + (135 / 3), 135 / 6, 4 },       // 101
    { 394, 4, 1, (135 / 2) + (135 / 3), 135 / 6, 4 },       // 102
    { 394, 4, 2, (135 / 2) + (135 / 3), 135 / 6, 4 },       // 103
    { 394, 4, 3, (135 / 2) + (135 / 3), 135 / 6, 4 },       // 104

    // ---
    { 394, 0, 0, 135 / 6, 135 / 6, 5 },                     // 105
    { 394, 5, 1, 135 / 6, 135 / 6, 5 },                     // 106
    { 394, 5, 2, 135 / 6, 135 / 6, 5 },                     // 107
    { 394, 5, 3, 135 / 6, 135 / 6, 5 },                     // 108
    { 394, 5, 4, 135 / 6, 135 / 6, 5 },                     // 109

    { 394, 0, 0, (135 / 2) + 1, 135 / 6, 5 },               // 110
    { 394, 5, 1, (135 / 2) + 1, 135 / 6, 5 },               // 111
    { 394, 5, 2, (135 / 2) + 1, 135 / 6, 5 },               // 112
    { 394, 5, 3, (135 / 2) + 1, 135 / 6, 5 },               // 113
    { 394, 5, 4, (135 / 2) + 1, 135 / 6, 5 },               // 114

    { 394, 0, 0, (135 / 2) + (135 / 3), 135 / 6, 5 },       // 115
    { 394, 5, 1, (135 / 2) + (135 / 3), 135 / 6, 5 },       // 116
    { 394, 5, 2, (135 / 2) + (135 / 3), 135 / 6, 5 },       // 117
    { 394, 5, 3, (135 / 2) + (135 / 3), 135 / 6, 5 },       // 118
    { 394, 5, 4, (135 / 2) + (135 / 3), 135 / 6, 5 },       // 119

    // ---
    { 394, 0, 0, 135 / 8, 135 / 8, 4 },                     // 120
    { 394, 4, 1, 135 / 8, 135 / 8, 4 },                     // 121
    { 394, 4, 2, 135 / 8, 135 / 8, 4 },                     // 122
    { 394, 4, 3, 135 / 8, 135 / 8, 4 },                     // 123

    { 394, 0, 0, (135 / 3) + 4, 135 / 8, 4 },               // 125
    { 394, 4, 1, (135 / 3) + 4, 135 / 8, 4 },               // 126
    { 394, 4, 2, (135 / 3) + 4, 135 / 8, 4 },               // 127
    { 394, 4, 3, (135 / 3) + 4, 135 / 8, 4 },               // 128

    { 394, 0, 0, (135 / 3) + (135 / 4) + 3, 135 / 8, 4 },   // 130
    { 394, 4, 1, (135 / 3) + (135 / 4) + 3, 135 / 8, 4 },   // 131
    { 394, 4, 2, (135 / 3) + (135 / 4) + 3, 135 / 8, 4 },   // 132
    { 394, 4, 3, (135 / 3) + (135 / 4) + 3, 135 / 8, 4 },   // 133

    { 394, 0, 0, (135 / 3) + (135 / 2) + 2, 135 / 8, 4 },   // 134
    { 394, 4, 1, (135 / 3) + (135 / 2) + 2, 135 / 8, 4 },   // 135
    { 394, 4, 2, (135 / 3) + (135 / 2) + 2, 135 / 8, 4 },   // 136
    { 394, 4, 3, (135 / 3) + (135 / 2) + 2, 135 / 8, 4 },   // 137
};

char *table_channels_scopes[] =
{
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "L",
    "R",
};

void Draw_Scope(ptk_data *ptk)
{
    int i;
    int pixel_color = COL_SCOPESSAMPLES;
    float pos_in_line;
    float datas;
    int x_max;
    int nibble_pos;
    int offset_scope = ptk->pos_scope_latency;
    LPDAT_POS_SCOPE ptrTbl_Dat;
    int scope_pos = Get_Song_Position(ptk);
    int active_channel;
    int cur_pos_x = 394;
    int max_right = Cur_Width - 395;

    if(offset_scope < 0) offset_scope = 0;
    if(offset_scope > (AUDIO_Latency / 2) - 1) offset_scope = (AUDIO_Latency / 2) - 1;

    if(ptk->Scopish == SCOPE_ZONE_SCOPE)
    {
        SetColor(COL_BACKGROUND);
        Fillrect(394, 42, Cur_Width, 178);

        cur_pos_x = 0;
        if(ptk->Scopish_LeftRight)
        {
            // Left / Right
            ptrTbl_Dat = &Scope_Table_Dats[Scope_Table[2].offset];
            for(i = 0; i < Scope_Table[2].nbr; i++)
            {
                if(ptrTbl_Dat->x_div) nibble_pos = ((max_right / ptrTbl_Dat->x_div) * ptrTbl_Dat->x_mul);
                else
                {
                    nibble_pos = 0;
                    cur_pos_x = 394;
                }
                PrintString(cur_pos_x + 4,
                        44 + (ptrTbl_Dat->y_pos - ptrTbl_Dat->y_large),
                        USE_FONT,
                        table_channels_scopes[i + MAX_TRACKS]);
                x_max = max_right / ptrTbl_Dat->x_max;
                if(nibble_pos)
                {
                    x_max += max_right % ptrTbl_Dat->x_max;
                }
                for(int s = 0; s < x_max; s++)
                {
                    pos_in_line = ((float) s) / (float) x_max;
                    datas = (Scope_Dats_LeftRight[i][offset_scope + (int) (pos_in_line * 128)] * 0.25f) / 8192;
                    if(datas < -1.0f) datas = -1.0f;
                    if(datas > 1.0f) datas = 1.0f;
                    int y = 42 + ptrTbl_Dat->y_pos + (int) (datas * ptrTbl_Dat->y_large);
                    DrawPixel(cur_pos_x, y, pixel_color);
                    cur_pos_x++;
                }
                ptrTbl_Dat++;
                pixel_color = COL_SCOPESSAMPLES;
            }
        }
        else
        {
            // Tracks
            ptrTbl_Dat = &Scope_Table_Dats[Scope_Table[Songtracks].offset];
            for(i = 0; i < Scope_Table[Songtracks].nbr; i++)
            {
                if(ptrTbl_Dat->x_div) nibble_pos = ((max_right / ptrTbl_Dat->x_div) * ptrTbl_Dat->x_mul);
                else
                {
                    nibble_pos = 0;
                    cur_pos_x = 394;
                }
                // Print the number of the track
                if(CHAN_ACTIVE_STATE[scope_pos][i])
                {
                    PrintString(cur_pos_x + 4, 44 + (ptrTbl_Dat->y_pos - ptrTbl_Dat->y_large),
                                USE_FONT, table_channels_scopes[i]);
                    active_channel = TRUE;
                }
                else
                {
                    PrintString(cur_pos_x + 4,
                                44 + (ptrTbl_Dat->y_pos - ptrTbl_Dat->y_large),
                                USE_FONT_LOW,
                                table_channels_scopes[i]);
                    active_channel = FALSE;
                }
                x_max = max_right / ptrTbl_Dat->x_max;
                if(!nibble_pos)
                {
                    x_max += max_right % ptrTbl_Dat->x_max;
                }
                for(int s = 0; s < x_max; s++)
                {
                    // [0..1]
                    pos_in_line = ((float) s) / (float) x_max;
                    datas = Scope_Dats[i][offset_scope + (int) (pos_in_line * 128)] / 8192;
                    if(datas < -1.0f) datas = -1.0f;
                    if(datas > 1.0f) datas = 1.0f;
                    int y = 42 + ptrTbl_Dat->y_pos + (int) (datas * ptrTbl_Dat->y_large);
                    DrawPixel(cur_pos_x, y, pixel_color);
                    cur_pos_x++;
                }
                ptrTbl_Dat++;
                pixel_color = COL_SCOPESSAMPLES;
            }
        }
    }
}

int Init_Scopes_Buffers(ptk_data *ptk)
{
    int i;

    ptk->pos_scope = 0;
    ptk->pos_scope_latency = 0;
    for(i = 0; i < MAX_TRACKS; i++)
    {  
        if(Scope_Dats[i]) free(Scope_Dats[i]);
        Scope_Dats[i] = (float *) malloc(((AUDIO_Latency + 1) + (512 * 4)) * 2);
        if(!Scope_Dats[i]) return(FALSE);
        memset(Scope_Dats[i], 0, ((AUDIO_Latency + 1) + (512 * 4)) * 2);
    }

    if(Scope_Dats_LeftRight[0]) free(Scope_Dats_LeftRight[0]);
    if(Scope_Dats_LeftRight[1]) free(Scope_Dats_LeftRight[1]);
    Scope_Dats_LeftRight[0] = (float *) malloc(((AUDIO_Latency + 1) + (512 * 4)) * 2);
    if(!Scope_Dats_LeftRight[0]) return(FALSE);
    Scope_Dats_LeftRight[1] = (float *) malloc(((AUDIO_Latency + 1) + (512 * 4)) * 2);
    if(!Scope_Dats_LeftRight[1]) return(FALSE);
    memset(Scope_Dats_LeftRight[0], 0, ((AUDIO_Latency + 1) + (512 * 4)) * 2);
    memset(Scope_Dats_LeftRight[1], 0, ((AUDIO_Latency + 1) + (512 * 4)) * 2);
    return(TRUE);
}

void Display_Dirs_Icons(int Idx)
{
    int Tab_Highlight[] =
    {
        BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL,
        BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_PUSHED, BUTTON_NORMAL,
        BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_PUSHED,
        BUTTON_PUSHED, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL,
        BUTTON_NORMAL, BUTTON_PUSHED, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL,
        BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_PUSHED, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL,
        BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_PUSHED, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL,
        BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_PUSHED, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL,
        BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_PUSHED, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL,
        BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_NORMAL, BUTTON_PUSHED, BUTTON_NORMAL, BUTTON_NORMAL
    };

    Idx *= 9;

    Gui_Draw_Button_Box(Cur_Width - 126, 24, 16, 16, "M", Tab_Highlight[0 + Idx] | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(Cur_Width - 108, 24, 16, 16, "I", Tab_Highlight[1 + Idx] | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(Cur_Width - 90, 24, 16, 16, "S", Tab_Highlight[2 + Idx] | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(Cur_Width - 72, 24, 16, 16, "P", Tab_Highlight[3 + Idx] | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(Cur_Width - 54, 24, 16, 16, "R", Tab_Highlight[4 + Idx] | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(Cur_Width - 36, 24, 16, 16, "B", Tab_Highlight[5 + Idx] | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(Cur_Width - 18, 24, 16, 16, "A", Tab_Highlight[6 + Idx] | BUTTON_TEXT_CENTERED);
    
    Gui_Draw_Button_Box(Cur_Width - 36, 6, 16, 16, "In", Tab_Highlight[7 + Idx] | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(Cur_Width - 18, 6, 16, 16, "Sy", Tab_Highlight[8 + Idx] | BUTTON_TEXT_CENTERED);
}

void Draw_Scope_Files_Button(ptk_data *ptk)
{
    switch(ptk->Scopish)
    {
        case SCOPE_ZONE_SCOPE:
            SetColor(COL_BACKGROUND);
            bjbox(394, 42, 405, 137);
            Gui_Draw_Button_Box(394, 24, Cur_Width - 522, 16, "", BUTTON_NORMAL | BUTTON_DISABLED);
            Gui_Draw_Button_Box(Cur_Width - 54, 6, 16, 16, "\255", BUTTON_PUSHED | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);

            Display_Dirs_Icons(0);
            break;

        case SCOPE_ZONE_INSTR_LIST:
            Actualize_Instruments_Synths_List(ptk, 0);
            Gui_Draw_Button_Box(Cur_Width - 54, 6, 16, 16, "\255", BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);

            Display_Dirs_Icons(1);
            break;

        case SCOPE_ZONE_SYNTH_LIST:
            Actualize_Instruments_Synths_List(ptk, 0);
            Gui_Draw_Button_Box(Cur_Width - 54, 6, 16, 16, "\255", BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);

            Display_Dirs_Icons(2);
            break;

        case SCOPE_ZONE_MOD_DIR:
        case SCOPE_ZONE_INSTR_DIR:
        case SCOPE_ZONE_PRESET_DIR:
        case SCOPE_ZONE_REVERB_DIR:
        case SCOPE_ZONE_PATTERN_DIR:
        case SCOPE_ZONE_MIDICFG_DIR:
        case SCOPE_ZONE_SAMPLE_DIR:

            Read_SMPT(ptk);
            Dump_Files_List(ptk, 395, 41);
            Actualize_Files_List(ptk, 0);
            Gui_Draw_Button_Box(Cur_Width - 54, 6, 16, 16, "\255", BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);

            switch(ptk->Scopish)
            {
                case SCOPE_ZONE_MOD_DIR:
                    Display_Dirs_Icons(3);
                    break;

                case SCOPE_ZONE_INSTR_DIR:
                    Display_Dirs_Icons(4);
                    break;

                case SCOPE_ZONE_SAMPLE_DIR:
                    Display_Dirs_Icons(5);
                    break;

                case SCOPE_ZONE_PRESET_DIR:
                    Display_Dirs_Icons(6);
                    break;

                case SCOPE_ZONE_REVERB_DIR:
                    Display_Dirs_Icons(7);
                    break;

                case SCOPE_ZONE_PATTERN_DIR:
                    Display_Dirs_Icons(8);
                    break;

                case SCOPE_ZONE_MIDICFG_DIR:
                    Display_Dirs_Icons(9);
                    break;
            }
            break;
    }
}

// ------------------------------------------------------
// Free all allocated midi channels and sub channels
void Clear_Midi_Channels_Pool(void)
{
    int i;
    int j;

    for(j = 0; j < MAX_POLYPHONY; j++)
    {
        for(i = 0; i < MAX_TRACKS; i++)
        {
            Alloc_midi_Channels[i][j] = 0;
        }
    }
}

// ------------------------------------------------------
// Schedule a note
void Send_Note(int Note, int Raw_Note, int One_Channel)
{
    // Provision to send
    Keyboard_Events[Keyboard_Nbr_Events] = Note;
    Keyboard_Notes_Type[Keyboard_Nbr_Events] = Raw_Note;
    Keyboard_Notes_Bound[Keyboard_Nbr_Events] = One_Channel;
    Keyboard_Nbr_Events++;
    if(Keyboard_Nbr_Events >= 256) Keyboard_Nbr_Events = 0;
}

// ------------------------------------------------------
// Jazz a note man
void Note_Jazz(ptk_data *ptk, int track, int note, float volume)
{
    // Play the note
    int Sub_Channel = Get_Free_Sub_Channel(ptk, track, Channels_Polyphony[track]);

    if(Sub_Channel == -1)
    {
        // We didn't have enough channel
        Sub_Channel = 0;
    }
    Sub_Channels_Jazz[track][Sub_Channel].Note = ((note + 1) << 8);
    Sub_Channels_Jazz[track][Sub_Channel].Channel = track;
    Sub_Channels_Jazz[track][Sub_Channel].Sub_Channel = Sub_Channel;
    local_mas_vol = 1.0f;
    local_ramp_vol = 1.0f;

    if(ptk->Jazz_Edit || is_recording_2 || !is_editing)
    {
        sp_Tvol_Mod[track] = 1.0f;
        Schedule_Instrument(ptk, track,
                            Sub_Channel,
                            note,
                            ptk->Current_Instrument,
                            0,
                            0,
                            !is_recording,
                            -(Sub_Channel + 1),
                            volume,
                            0,
                            0);
    }
}

// ------------------------------------------------------
// Jazz a note off man
void Note_Jazz_Off(ptk_data *ptk, int note)
{
    // Play a note off
    LPJAZZ_KEY Channel = Get_Jazz_Key_Off(Sub_Channels_Jazz, (note + 1) << 8);
    if(Channel)
    {
        if(ptk->Jazz_Edit || is_recording_2 || !is_editing)
        {
            //Synthesizer[Channel->Channel][Channel->Sub_Channel].NoteOff();
            ptk_synth_note_off(&Synthesizer[Channel->Channel][Channel->Sub_Channel]);
            /*printf("REALTIME NOTEOFF! %d %d\n", Channel->Channel, Channel->Sub_Channel);*/
            if(sp_Stage[Channel->Channel][Channel->Sub_Channel] == PLAYING_SAMPLE)
            {
                sp_Stage[Channel->Channel][Channel->Sub_Channel] = PLAYING_SAMPLE_NOTEOFF;
            }
            // That's a possibility
            noteoff303(ptk, Channel->Channel);
        }
        Channel->Note = 0;
        Channel->Channel = 0;
        Channel->Sub_Channel = 0;
    }

#if !defined(__STAND_ALONE__)
#if !defined(__NO_MIDI__)
    if(ptk->Jazz_Edit || is_recording_2 || !is_editing)
    {
        Midi_NoteOff(ptk, ptk->Track_Under_Caret, note);
    }
#endif
#endif

}

void ptk_init(ptk_data *ptk)
{
    ptk->c_l_tvol = 32768;
    ptk->c_r_tvol = 32768;
    ptk->c_l_cvol = 32768;
    ptk->c_r_cvol = 32768;

    ptk->Display_Pointer = FALSE;
    
    ptk->Scopish = SCOPE_ZONE_MOD_DIR;
    ptk->Scopish_LeftRight = FALSE;

    ptk->fluzy = -1;
    ptk->rs_coef = 32768;
    ptk->gui_lx = -1;
    ptk->gui_ly = -1;
    ptk->gui_lxs = -1;
    ptk->gui_lys = -1;
    ptk->trkchan = TRUE;
    ptk->pos_space = 0;
    ptk->multifactor = 4;
    ptk->seditor = 0;
    ptk->ctipoftheday = 0;
    ptk->sas = FALSE;
    ptk->ltretvalue = 0;
    ptk->last_index = -1;

    ptk->gui_action_metronome = GUI_CMD_NOP;
    ptk->gui_action = GUI_CMD_NOP;
    ptk->gui_pushed = 0;
    ptk->Done_Tip = FALSE;
    ptk->player_pos = -1;
    ptk->teac = 0;
    ptk->Column_Under_Caret = 0;

    ptk->Current_Instrument_Split = 0;
    ptk->liveparam = 0;
    ptk->livevalue = 0;
    ptk->Track_Under_Caret = 0;
    ptk->gui_track = 0;

    ptk->Current_Instrument = 0;
    ptk->userscreen = USER_SCREEN_DISKIO_EDIT;
    ptk->restx = 0;
    ptk->resty = 0;
    ptk->fsize = 0;

    ptk->draw_sampled_wave = FALSE;
    ptk->draw_sampled_wave2 = FALSE;
    ptk->draw_sampled_wave3 = FALSE;

    ptk->redraw_everything = FALSE;

    ptk->boing = FALSE;
    
    ptk->po_ctrl2 = TRUE;
    ptk->po_alt = TRUE;
    ptk->po_alt2 = TRUE;

    ptk->sp_TickCounter = 0;
    ptk->LVColor = 0;
    ptk->RVColor = 0;

    ptk->L = luaL_newstate();

    ptk->snamesel = INPUT_NONE;

    ptk->player_line = 0;

    ptk->actuloop = 0;

    ptk->namesize = 8;
    
    ptk->MouseWheel_Multiplier = 1;

    ptk->Jazz_Edit = FALSE;
    ptk->Rows_Decimal = FALSE;
    ptk->See_Prev_Next_Pattern = FALSE;

    ptk->gui_thread_action = FALSE;
    ptk->gui_bpm_action = FALSE;
    ptk->sporth.use_sporth = FALSE;

	ptk->start_gui = TRUE;
	ptk->render_mode = FALSE;

	/* number of tabs in toolbar */
	ptk->ntabs = 11;

    /* lua reference value, storing callback function */
    ptk->lref = -1;

    ptk->Ticks_Synchro_Left = 1;
    ptk->Ticks_Synchro_Right = 1;
    ptk->sl3 = 0;

    /* note callback - bits set to zero */
    ptk->note_cb = 0;
    /* lua note callback reference */
    ptk->note_cb_ref = -1;
    
    /* lua sporth recompile callback */
    ptk->recompile_cb = -1;

    ptk_tab_dont_dump(&ptk->tab);
}

void ptk_close(ptk_data *ptk) 
{
    lua_close(ptk->L);
    if(ptk->sporth.use_sporth == TRUE) ptk_sporth_destroy(ptk);
}

static int vi_normal_mode(ptk_data *ptk, 
        unsigned short l_c, 
        unsigned short u_c, 
        bool is_editing)
{
    return (is_editing == FALSE) && 
        (l_c || u_c) && ptk->snamesel == INPUT_NONE;
}
