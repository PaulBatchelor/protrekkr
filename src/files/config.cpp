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
#include "ptk_data.h"
#include "variables.h"
#include "replay.h"
#include "config.h"
#include "midi_cfg.h"

// ------------------------------------------------------
// Variables
extern int patt_highlight;
extern char FullScreen;
extern int Cur_Left;
extern int Cur_Top;
extern int Cur_Width;
extern int Cur_Height;
extern int Continuous_Scroll;
extern char AutoSave;
extern char AutoBackup;
extern char Accidental;
extern char Use_Shadows;
extern char Global_Patterns_Font;
extern char *cur_dir;

// ------------------------------------------------------
// Save the configuration file
void SaveConfig(ptk_data *ptk)
{
    FILE *out;
    char extension[10];
    char FileName[MAX_PATH];
    int i;
    int Real_Palette_Idx;
    char KeyboardName[MAX_PATH];
    signed char phony = -1;

    sprintf(extension, "PROTCFGF");
    Status_Box(ptk, "Saving 'ptk.cfg'...");

#ifdef __linux__
    sprintf(FileName, "%s/.ptk.cfg", getenv("HOME"));
#else
    sprintf(FileName, "%s"SLASH"ptk.cfg", ExePath);
#endif

    memset(KeyboardName, 0, sizeof(KeyboardName));
    sprintf(KeyboardName, "%s", Keyboard_Name);

    out = fopen(FileName, "wb");
    if(out != NULL)
    {
        Write_Data(ptk, extension, sizeof(char), 9, out);
        Write_Data_Swap(ptk, &Current_Edit_Steps, sizeof(Current_Edit_Steps), 1, out);
        Write_Data_Swap(ptk, &patt_highlight, sizeof(patt_highlight), 1, out);
        Write_Data_Swap(ptk, &AUDIO_Milliseconds, sizeof(AUDIO_Milliseconds), 1, out);

#if defined(__NO_MIDI__)
        Write_Data(ptk, &phony, sizeof(phony), 1, out);
#else
        Write_Data(ptk, &c_midiin, sizeof(c_midiin), 1, out);
#endif

#if defined(__NO_MIDI__)
        Write_Data(ptk, &phony, sizeof(phony), 1, out);
#else
        Write_Data(ptk, &c_midiout, sizeof(c_midiout), 1, out);
#endif

        Write_Data_Swap(ptk, &ptk->MouseWheel_Multiplier, sizeof(ptk->MouseWheel_Multiplier), 1, out);
        Write_Data(ptk, &ptk->Rows_Decimal, sizeof(ptk->Rows_Decimal), 1, out);
        Write_Data(ptk, &FullScreen, sizeof(FullScreen), 1, out);

        for(i = 0; i < NUMBER_COLORS; i++)
        {
            Real_Palette_Idx = Idx_Palette[i];
            Write_Data(ptk, &Ptk_Palette[Real_Palette_Idx].r, sizeof(char), 1, out);
            Write_Data(ptk, &Ptk_Palette[Real_Palette_Idx].g, sizeof(char), 1, out);
            Write_Data(ptk, &Ptk_Palette[Real_Palette_Idx].b, sizeof(char), 1, out);
        }
        Write_Data(ptk, &ptk->See_Prev_Next_Pattern, sizeof(ptk->See_Prev_Next_Pattern), 1, out);
        Write_Data_Swap(ptk, &Beveled, sizeof(Beveled), 1, out);
        Write_Data_Swap(ptk, &Continuous_Scroll, sizeof(Continuous_Scroll), 1, out);
        Write_Data(ptk, &AutoSave, sizeof(AutoSave), 1, out);
        Write_Data(ptk, &AutoBackup, sizeof(AutoBackup), 1, out);
        
        Write_Data(ptk, &Dir_Mods, sizeof(Dir_Mods), 1, out);
        Write_Data(ptk, &Dir_Instrs, sizeof(Dir_Instrs), 1, out);
        Write_Data(ptk, &Dir_Presets, sizeof(Dir_Presets), 1, out);
        Write_Data(ptk, &Dir_Reverbs, sizeof(Dir_Reverbs), 1, out);
        Write_Data(ptk, &Dir_MidiCfg, sizeof(Dir_MidiCfg), 1, out);
        Write_Data(ptk, &Dir_Patterns, sizeof(Dir_Patterns), 1, out);
        Write_Data(ptk, &Dir_Samples, sizeof(Dir_Samples), 1, out);
        Write_Data(ptk, KeyboardName, MAX_PATH, 1, out);

        Write_Data(ptk, &rawrender_32float, sizeof(char), 1, out);
        Write_Data(ptk, &rawrender_multi, sizeof(char), 1, out);
        Write_Data(ptk, &rawrender_target, sizeof(char), 1, out);
        Write_Data(ptk, &Large_Patterns, sizeof(char), 1, out);
        Write_Data(ptk, &ptk->Scopish_LeftRight, sizeof(char), 1, out);
 
        Write_Data(ptk, &Paste_Across, sizeof(char), 1, out);
        Write_Data(ptk, &ptk->Jazz_Edit, sizeof(char), 1, out);
        Write_Data(ptk, &Accidental, sizeof(char), 1, out);

        Write_Data(ptk, &Use_Shadows, sizeof(char), 1, out);
        Write_Data(ptk, &Global_Patterns_Font, sizeof(char), 1, out);

        Write_Data(ptk, &ptk->metronome_magnify, sizeof(int), 1, out);

        // Save the compelte midi automation config
        Save_MidiCfg_Data(ptk, Write_Data, Write_Data_Swap, out);

        Write_Data_Swap(ptk, &Cur_Width, sizeof(int), 1, out);
        Write_Data_Swap(ptk, &Cur_Height, sizeof(int), 1, out);

        Cur_Left = -1;
        Cur_Top = -1;
        Write_Data_Swap(ptk, &Cur_Left, sizeof(int), 1, out);
        Write_Data_Swap(ptk, &Cur_Top, sizeof(int), 1, out);

        fclose(out);

        Read_SMPT(ptk);
        ptk->last_index = -1;
        Actualize_Files_List(ptk, 0);
        Status_Box(ptk, "Configuration file saved succesfully.");  
    }
    else
    {
        Status_Box(ptk, "Configuration file save failed.");
    }
}

// ------------------------------------------------------
// Load the configuration file
void LoadConfig(ptk_data *ptk)
{
    FILE *in;
    int i;
    int Real_Palette_Idx;
    char FileName[MAX_PATH];
    char KeyboardName[MAX_PATH];
    signed char phony = -1;
    char Win_Coords[64];
    SDL_Surface *Desktop = NULL;

#ifdef __linux__
    sprintf(FileName, "%s/.ptk.cfg", getenv("HOME"));
#else
    sprintf(FileName, "%s"SLASH"ptk.cfg", ExePath);
#endif

    memset(KeyboardName, 0, sizeof(KeyboardName));

    in = fopen(FileName, "rb");
    if(in != NULL)
    {
        // Reading and checking extension...
        char extension[10];

        Read_Data(ptk, extension, sizeof(char), 9, in);
        if(strcmp(extension, "PROTCFGF") == 0)
        {
            Read_Data_Swap(ptk, &Current_Edit_Steps, sizeof(Current_Edit_Steps), 1, in);
            Read_Data_Swap(ptk, &patt_highlight, sizeof(patt_highlight), 1, in);
            Read_Data_Swap(ptk, &AUDIO_Milliseconds, sizeof(AUDIO_Milliseconds), 1, in);

#if defined(__NO_MIDI__)
            Read_Data(ptk, &phony, sizeof(phony), 1, in);
#else
            Read_Data(ptk, &c_midiin, sizeof(c_midiin), 1, in);
#endif

#if defined(__NO_MIDI__)
            Read_Data(ptk, &phony, sizeof(phony), 1, in);
#else
            Read_Data(ptk, &c_midiout, sizeof(c_midiout), 1, in);
#endif

            Read_Data_Swap(ptk, &ptk->MouseWheel_Multiplier, sizeof(ptk->MouseWheel_Multiplier), 1, in);
            Read_Data(ptk, &ptk->Rows_Decimal, sizeof(ptk->Rows_Decimal), 1, in);
            Read_Data(ptk, &FullScreen, sizeof(FullScreen), 1, in);

            for(i = 0; i < NUMBER_COLORS; i++)
            {
                Real_Palette_Idx = Idx_Palette[i];
                Read_Data(ptk, &Ptk_Palette[Real_Palette_Idx].r, sizeof(char), 1, in);
                Read_Data(ptk, &Ptk_Palette[Real_Palette_Idx].g, sizeof(char), 1, in);
                Read_Data(ptk, &Ptk_Palette[Real_Palette_Idx].b, sizeof(char), 1, in);
                Ptk_Palette[Real_Palette_Idx].unused = 0;
            }
            Read_Data(ptk, &ptk->See_Prev_Next_Pattern, sizeof(ptk->See_Prev_Next_Pattern), 1, in);
            Read_Data_Swap(ptk, &Beveled, sizeof(Beveled), 1, in);
            Read_Data_Swap(ptk, &Continuous_Scroll, sizeof(Continuous_Scroll), 1, in);
            Read_Data(ptk, &AutoSave, sizeof(AutoSave), 1, in);
            Read_Data(ptk, &AutoBackup, sizeof(AutoBackup), 1, in);
            Read_Data(ptk, &Dir_Mods, sizeof(Dir_Mods), 1, in);
            Read_Data(ptk, &Dir_Instrs, sizeof(Dir_Instrs), 1, in);
            Read_Data(ptk, &Dir_Presets, sizeof(Dir_Presets), 1, in);
            Read_Data(ptk, &Dir_Reverbs, sizeof(Dir_Reverbs), 1, in);
            Read_Data(ptk, &Dir_MidiCfg, sizeof(Dir_MidiCfg), 1, in);
            Read_Data(ptk, &Dir_Patterns, sizeof(Dir_Patterns), 1, in);
            Read_Data(ptk, &Dir_Samples, sizeof(Dir_Samples), 1, in);
            Read_Data(ptk, KeyboardName, MAX_PATH, 1, in);

            Read_Data(ptk, &rawrender_32float, sizeof(char), 1, in);
            Read_Data(ptk, &rawrender_multi, sizeof(char), 1, in);
            Read_Data(ptk, &rawrender_target, sizeof(char), 1, in);
            Read_Data(ptk, &Large_Patterns, sizeof(char), 1, in);
            Read_Data(ptk, &ptk->Scopish_LeftRight, sizeof(char), 1, in);

            Read_Data(ptk, &Paste_Across, sizeof(char), 1, in);
            Read_Data(ptk, &ptk->Jazz_Edit, sizeof(char), 1, in);
            Read_Data(ptk, &Accidental, sizeof(char), 1, in);

            Read_Data(ptk, &Use_Shadows, sizeof(char), 1, in);
            Read_Data(ptk, &Global_Patterns_Font, sizeof(char), 1, in);

            Read_Data(ptk, &ptk->metronome_magnify, sizeof(int), 1, in);

            if(Large_Patterns)
            {
                Set_Pattern_Size(ptk);
                ptk->userscreen = USER_SCREEN_LARGE_PATTERN;
                curr_tab_highlight = USER_SCREEN_DISKIO_EDIT;
            }
            else
            {
                Set_Pattern_Size(ptk);
                ptk->userscreen = USER_SCREEN_DISKIO_EDIT;
                curr_tab_highlight = USER_SCREEN_DISKIO_EDIT;
            }

            // Reload the compelte midi automation config
            Load_MidiCfg_Data(ptk, Read_Data, Read_Data_Swap, in);

            Read_Data_Swap(ptk, &Cur_Width, sizeof(int), 1, in);
            Read_Data_Swap(ptk, &Cur_Height, sizeof(int), 1, in);

            Read_Data_Swap(ptk, &Cur_Left, sizeof(int), 1, in);
            Read_Data_Swap(ptk, &Cur_Top, sizeof(int), 1, in);
            Desktop = SDL_SetVideoMode(0, 0, 0, 0);
            // Check if the coords are too big
            if(Cur_Width > SDL_GetVideoSurface()->w)
            {
                Cur_Left = 0;
                Cur_Width = SDL_GetVideoSurface()->w;
            }
            if(Cur_Height > SDL_GetVideoSurface()->h)
            {
                Cur_Top = 0;
                Cur_Height = SDL_GetVideoSurface()->h;
            }
            if(Cur_Left == -1 ||
               Cur_Top == -1)
            {
                Cur_Left = SDL_GetVideoSurface()->w;
                Cur_Top = SDL_GetVideoSurface()->h;
                Cur_Left = (Cur_Left - Cur_Width) / 2;
                Cur_Top = (Cur_Top - Cur_Height) / 2;
            }
            SDL_FreeSurface(Desktop);

            sprintf(Win_Coords,
                    "SDL_VIDEO_WINDOW_POS=%d,%d",
                    Cur_Left,
                    Cur_Top);
            SDL_putenv(Win_Coords);
        }
        fclose(in);
    }
    sprintf(Keyboard_Name, "%s", KeyboardName);

    // Set default dirs if nothing
    if(!strlen(Dir_Mods))
    {
        GETCWD(Dir_Mods, MAX_PATH);

#if defined(__WIN32__)
        strcat(Dir_Mods, "\\modules");
#else
        strcat(Dir_Mods, "/modules");
#endif

    }
    if(!strlen(Dir_Instrs))
    {
        GETCWD(Dir_Instrs, MAX_PATH);

#if defined(__WIN32__)
        strcat(Dir_Instrs, "\\instruments");
#else
        strcat(Dir_Instrs, "/instruments");
#endif

    }
    if(!strlen(Dir_Presets))
    {
        GETCWD(Dir_Presets, MAX_PATH);

#if defined(__WIN32__)
        strcat(Dir_Presets, "\\presets");
#else
        strcat(Dir_Presets, "/presets");
#endif

    }

    if(!strlen(Dir_Reverbs))
    {
        GETCWD(Dir_Reverbs, MAX_PATH);

#if defined(__WIN32__)
        strcat(Dir_Reverbs, "\\reverbs");
#else
        strcat(Dir_Reverbs, "/reverbs");
#endif

    }

    if(!strlen(Dir_MidiCfg))
    {
        GETCWD(Dir_MidiCfg, MAX_PATH);

#if defined(__WIN32__)
        strcat(Dir_MidiCfg, "\\midicfgs");
#else
        strcat(Dir_MidiCfg, "/midicfgs");
#endif

    }

    if(!strlen(Dir_Patterns))
    {
        GETCWD(Dir_Patterns, MAX_PATH);

#if defined(__WIN32__)
        strcat(Dir_Patterns, "\\patterns");
#else
        strcat(Dir_Patterns, "/patterns");
#endif

    }

    if(!strlen(Dir_Samples))
    {
        GETCWD(Dir_Samples, MAX_PATH);

#if defined(__WIN32__)
        strcat(Dir_Samples, "\\samples");
#else
        strcat(Dir_Samples, "/samples");
#endif

    }

    cur_dir = Dir_Mods;
}
