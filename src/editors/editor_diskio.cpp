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
#include "editor_diskio.h"
#include "files.h"
#include "requesters.h"

#if defined(__AROS__)
#include <string.h>
#endif

// ------------------------------------------------------
// Variables
extern REQUESTER Overwrite_Requester;
extern REQUESTER Zzaapp_Requester;

int allow_save = TRUE;
extern int song_Seconds;
extern int song_Minutes;
extern int song_Hours;
int Allow_32bit = 0;
int Allow_Single_Render = 0;

int Tracks_To_Render[MAX_TRACKS];
char *Tracks_Labels[MAX_TRACKS] =
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
    "F"
};

typedef struct
{
    int x;
    int y;
} TRACK_POS, *LPTRACK_POS;

TRACK_POS Tracks_Position[MAX_TRACKS] =
{
    { 350 + (0 * 18), 0 },
    { 350 + (1 * 18), 0 },
    { 350 + (2 * 18), 0 },
    { 350 + (3 * 18), 0 },
    { 350 + (4 * 18), 0 },
    { 350 + (5 * 18), 0 },
    { 350 + (6 * 18), 0 },
    { 350 + (7 * 18), 0 },

    { 350 + (0 * 18), 18 },
    { 350 + (1 * 18), 18 },
    { 350 + (2 * 18), 18 },
    { 350 + (3 * 18), 18 },
    { 350 + (4 * 18), 18 },
    { 350 + (5 * 18), 18 },
    { 350 + (6 * 18), 18 },
    { 350 + (7 * 18), 18 },
};

// ------------------------------------------------------
// Functions
unsigned long Calc_Length(ptk_data *ptk);
void Reset_Song_Length(void);
void Display_Song_Length(ptk_data *ptk);
void Display_Tracks_To_Render(ptk_data *ptk);
void Check_Tracks_To_Render(ptk_data *ptk);
int Is_Track_To_Render_Solo(int nbr);
void Check_Tracks_To_Render_To_Solo(ptk_data *ptk);

void Draw_DiskIO_Ed(ptk_data *ptk)
{
    Draw_Editors_Bar(ptk, USER_SCREEN_DISKIO_EDIT);

    Gui_Draw_Button_Box(0, (Cur_Height - 153), ptk->fsize, 130, "", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Flat_Box("Disk Operations / Module Credits");

    Gui_Draw_Button_Box(254, (Cur_Height - 112), 80, 16, "Calc .ptp Size", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    outlong(254, (Cur_Height - 94), Final_Mod_Length, 7);

    Gui_Draw_Button_Box(254, (Cur_Height - 76), 80, 16, "Calc Length", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Display_Song_Length(ptk);

    Gui_Draw_Button_Box(8, (Cur_Height - 94), 80, 16, "Title", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(8, (Cur_Height - 76), 80, 16, "Produced By", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(8, (Cur_Height - 58), 80, 16, "Message", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(8, (Cur_Height - 130), 80, 16, "Zzaapp", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(90, (Cur_Height - 112), 80, 16, "WAV Render", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(90, (Cur_Height - 130), 80, 16, "Show Info", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

    Gui_Draw_Button_Box(342, (Cur_Height - 130), 404, 102, "", BUTTON_NORMAL | BUTTON_DISABLED);

    PrintString(350, (Cur_Height - 128), USE_FONT, "Tracks To Render :");
    Display_Tracks_To_Render(ptk);

    PrintString(654, (Cur_Height - 124), USE_FONT, "Render To :");

    Gui_Draw_Button_Box(350, (Cur_Height - 68), 106, 16, "Output Bits Quality", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(350, (Cur_Height - 49), 106, 16, "One file per track", BUTTON_NORMAL | BUTTON_DISABLED);

    Gui_Draw_Button_Box(532, (Cur_Height - 86), 60, 26, "From", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_NO_BORDER | BUTTON_TEXT_VTOP);
    Gui_Draw_Button_Box(532, (Cur_Height - 66), 60, 26, "To", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_NO_BORDER | BUTTON_TEXT_VTOP);
}

// ------------------------------------------------------
// Refresh function
void Actualize_DiskIO_Ed(ptk_data *ptk, int gode)
{
    if(ptk->userscreen == USER_SCREEN_DISKIO_EDIT)
    {
        char tname[32];

        if(gode == 0 || gode == 5)
        {
            switch(rawrender_target)
            {
                case RENDER_TO_FILE:
                    Gui_Draw_Button_Box(654, (Cur_Height - 106), 80, 16, "Wav File", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(654, (Cur_Height - 88), 80, 16, "Mono Sample", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(654, (Cur_Height - 70), 80, 16, "Stereo Sample", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    Allow_32bit = 0;
                    Allow_Single_Render = 0;
                    break;
                case RENDER_TO_MONO:
                    Gui_Draw_Button_Box(654, (Cur_Height - 106), 80, 16, "Wav File", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(654, (Cur_Height - 88), 80, 16, "Mono Sample", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(654, (Cur_Height - 70), 80, 16, "Stereo Sample", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    Allow_32bit = BUTTON_DISABLED;
                    Allow_Single_Render = BUTTON_DISABLED;
                    break;
                case RENDER_TO_STEREO:
                    Gui_Draw_Button_Box(654, (Cur_Height - 106), 80, 16, "Wav File", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(654, (Cur_Height - 88), 80, 16, "Mono Sample", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                    Gui_Draw_Button_Box(654, (Cur_Height - 70), 80, 16, "Stereo Sample", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
                    Allow_32bit = BUTTON_DISABLED;
                    Allow_Single_Render = BUTTON_DISABLED;
                    break;
            }
        }

        if(gode == 0 || gode == 1)
        {
            if(rawrender_32float)
            {
                Gui_Draw_Button_Box(458, (Cur_Height - 68), 29, 16, "32", Allow_32bit | BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(458 + 31, (Cur_Height - 68), 29, 16, "16", Allow_32bit | BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }
            else
            {
                Gui_Draw_Button_Box(458, (Cur_Height - 68), 29, 16, "32", Allow_32bit | BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(458 + 31, (Cur_Height - 68), 29, 16, "16", Allow_32bit | BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
            }
        }

        if(gode == 0 || gode == 2)
        {
            if(!rawrender_range)
            {
                Gui_Draw_Button_Box(534, (Cur_Height - 112), 40, 16, "Whole", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(534 + 42, (Cur_Height - 112), 40, 16, "Range", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }
            else
            {
                Gui_Draw_Button_Box(534, (Cur_Height - 112), 40, 16, "Whole", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(534 + 42, (Cur_Height - 112), 40, 16, "Range", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
            }
        }

        // From position
        if(gode == 0 || gode == 3)
        {
            if(rawrender_from < 0) rawrender_from = 0;
            if(rawrender_from > (Song_Length - 1)) rawrender_from = (Song_Length - 1);
            if(rawrender_from > rawrender_to)
            {
                rawrender_to = rawrender_from;
                Gui_Draw_Arrows_Number_Box(572, (Cur_Height - 66), rawrender_to, BUTTON_NORMAL | (rawrender_range ? 0 : BUTTON_DISABLED) | BUTTON_RIGHT_MOUSE | BUTTON_TEXT_CENTERED);
            }
            Gui_Draw_Arrows_Number_Box(572, (Cur_Height - 86), rawrender_from, BUTTON_NORMAL | (rawrender_range ? 0 : BUTTON_DISABLED) | BUTTON_RIGHT_MOUSE | BUTTON_TEXT_CENTERED);
        }

        // To position
        if(gode == 0 || gode == 4)
        {
            if(rawrender_to < 0) rawrender_to = 0;
            if(rawrender_to > (Song_Length - 1)) rawrender_to = (Song_Length - 1);
            if(rawrender_to < rawrender_from)
            {
                rawrender_from = rawrender_to;
                Gui_Draw_Arrows_Number_Box(572, (Cur_Height - 86), rawrender_from, BUTTON_NORMAL | (rawrender_range ? 0 : BUTTON_DISABLED) | BUTTON_RIGHT_MOUSE | BUTTON_TEXT_CENTERED);
            }
            Gui_Draw_Arrows_Number_Box(572, (Cur_Height - 66), rawrender_to, BUTTON_NORMAL | (rawrender_range ? 0 : BUTTON_DISABLED) | BUTTON_RIGHT_MOUSE | BUTTON_TEXT_CENTERED);
        }

        if(allow_save)
        {
            Gui_Draw_Button_Box(8, (Cur_Height - 112), 80, 16, "Save Module", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(254, (Cur_Height - 130), 80, 16, "Save .ptp", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
        }
        else
        {
            Gui_Draw_Button_Box(8, (Cur_Height - 112), 80, 16, "Save Module", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(254, (Cur_Height - 130), 80, 16, "Save .ptp", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
        }

        if(gode == 0 || gode == 5)
        {
            if(rawrender_multi)
            {
                Gui_Draw_Button_Box(458, (Cur_Height - 49), 29, 16, "On", Allow_Single_Render | BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(458 + 31, (Cur_Height - 49), 29, 16, "Off", Allow_Single_Render | BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }
            else
            {
                Gui_Draw_Button_Box(458, (Cur_Height - 49), 29, 16, "On", Allow_Single_Render | BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
                Gui_Draw_Button_Box(458 + 31, (Cur_Height - 49), 29, 16, "Off", Allow_Single_Render | BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
            }
        }


        if(ptk->snamesel == INPUT_MODULE_NAME)
        {
            sprintf(tname, "%s_", ptk->name);
            Gui_Draw_Button_Box(90, (Cur_Height - 94), 162, 16, tname, BUTTON_PUSHED | BUTTON_INPUT);
        }
        else
        {
            sprintf(tname, "%s", ptk->name);
            Gui_Draw_Button_Box(90, (Cur_Height - 94), 162, 16, tname, BUTTON_NORMAL | BUTTON_INPUT);
        }

        if(ptk->snamesel == INPUT_MODULE_ARTIST)
        {
            sprintf(tname, "%s_", ptk->artist);
            Gui_Draw_Button_Box(90, (Cur_Height - 76), 162, 16, tname, BUTTON_PUSHED | BUTTON_INPUT);
        }
        else
        {
            sprintf(tname, "%s", ptk->artist);
            Gui_Draw_Button_Box(90, (Cur_Height - 76), 162, 16, tname, BUTTON_NORMAL | BUTTON_INPUT);
        }

        if(ptk->snamesel == INPUT_MODULE_STYLE)
        {
            sprintf(tname, "%s_", ptk->style);
            Gui_Draw_Button_Box(90, (Cur_Height - 58), 162, 16, tname, BUTTON_PUSHED | BUTTON_INPUT);
        }
        else
        {
            sprintf(tname, "%s", ptk->style);
            Gui_Draw_Button_Box(90, (Cur_Height - 58), 162, 16, tname, BUTTON_NORMAL | BUTTON_INPUT);
        }
    }
}

// ------------------------------------------------------
// Handle right mouse button
void Mouse_Right_DiskIO_Ed(ptk_data *ptk)
{
    if(ptk->userscreen == USER_SCREEN_DISKIO_EDIT)
    {
        Check_Tracks_To_Render_To_Solo(ptk);

        if(rawrender_range)
        {
            // From position
            if(zcheckMouse(ptk, 572, (Cur_Height - 86), 16, 16) == 1)
            {
                rawrender_from -= 10;
                ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
                ptk->teac = 3;
            }

            // From position
            if(zcheckMouse(ptk, 572 + 44, (Cur_Height - 86), 16, 16) == 1)
            {
                rawrender_from += 10;
                ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
                ptk->teac = 3;
            }

            // To position
            if(zcheckMouse(ptk, 572, (Cur_Height - 66), 16, 16) == 1)
            {
                rawrender_to -= 10;
                ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
                ptk->teac = 4;
            }

            // To position
            if(zcheckMouse(ptk, 572 + 44, (Cur_Height - 66), 16, 16) == 1)
            {
                rawrender_to += 10;
                ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
                ptk->teac = 4;
            }
        }
    }
}

// ------------------------------------------------------
// Handle left mouse button
void Mouse_Left_DiskIO_Ed(ptk_data *ptk)
{
    int i;
    char WavFileName[MAX_PATH];

    if(ptk->userscreen == USER_SCREEN_DISKIO_EDIT)
    {
        // Save song
        if(zcheckMouse(ptk, 8, (Cur_Height - 112), 80, 16))
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
        // Save final
        if(zcheckMouse(ptk, 254, (Cur_Height - 130), 80, 16))
        {
            if(File_Exist_Req(ptk, "%s"SLASH"%s.ptp", Dir_Mods, ptk->name))
            {
                Display_Requester(ptk, &Overwrite_Requester, GUI_CMD_SAVE_FINAL);
            }
            else
            {
                ptk->gui_action = GUI_CMD_SAVE_FINAL;
            }
        }
        // Calc final
        if(zcheckMouse(ptk, 254, (Cur_Height - 112), 80, 16))
        {
            ptk->gui_action = GUI_CMD_CALC_FINAL;
        }
        // Calc length
        if(zcheckMouse(ptk, 254, (Cur_Height - 76), 80, 16))
        {
            Calc_Length(ptk);
        }

        if(zcheckMouse(ptk, 90, (Cur_Height - 130), 80, 16))
        {
            ptk->gui_action = GUI_CMD_MODULE_INFOS;
        }

        // Start module name input
        if(zcheckMouse(ptk, 90, (Cur_Height - 94), 162, 16) && ptk->snamesel == INPUT_NONE)
        {
            strcpy(cur_input_name, ptk->name);
            sprintf(ptk->name, "");
            ptk->namesize = 0;
            ptk->snamesel = INPUT_MODULE_NAME;
            ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
        }

        // Start ptk->artist name input
        if(zcheckMouse(ptk, 90, (Cur_Height - 76), 162, 16) && ptk->snamesel == INPUT_NONE)
        {
            strcpy(cur_input_name, ptk->artist);
            sprintf(ptk->artist, "");
            ptk->namesize = 0;
            ptk->snamesel = INPUT_MODULE_ARTIST;
            ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
        }

        // Start module ptk->style input
        if(zcheckMouse(ptk, 90, (Cur_Height - 58), 162, 16) && ptk->snamesel == INPUT_NONE)
        {
            strcpy(cur_input_name, ptk->style);
            sprintf(ptk->style, "");
            ptk->namesize = 0;
            ptk->snamesel = INPUT_MODULE_STYLE;
            ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
        }

        // Zzaapp
        if(zcheckMouse(ptk, 8, (Cur_Height - 130), 80, 16))
        {
            Display_Requester(ptk, &Zzaapp_Requester, GUI_CMD_NEW_MODULE);
        }

        if(zcheckMouse(ptk, 90, (Cur_Height - 112), 80, 16))
        {
            if(rawrender_target == RENDER_TO_FILE)
            {
                if(rawrender_multi &&
                   rawrender_target == RENDER_TO_FILE)
                {
                    int any_file = FALSE;
                    for(i = 0; i < Songtracks; i++)
                    {
                        sprintf(WavFileName, "%%s"SLASH"%%s_%x.wav", i);
                        if(File_Exist(ptk, WavFileName, Dir_Mods, ptk->name))
                        {
                            any_file = TRUE;
                            break;
                        }
                    }
                    if(any_file)
                    {
                        Overwrite_Requester.Text = "Some .wav files are about to be overwritten, is that ok ?";
                        Display_Requester(ptk, &Overwrite_Requester, GUI_CMD_RENDER_WAV);
                    }
                    else
                    {
                        ptk->gui_action = GUI_CMD_RENDER_WAV;
                    }
                }
                else
                {
                    if(File_Exist_Req(ptk, "%s"SLASH"%s.wav", Dir_Mods, ptk->name))
                    {
                        Display_Requester(ptk, &Overwrite_Requester, GUI_CMD_RENDER_WAV);
                    }
                    else
                    {
                        ptk->gui_action = GUI_CMD_RENDER_WAV;
                    }
                }
            }
            else
            {
                ptk->gui_action = GUI_CMD_RENDER_WAV;
            }
        }

        // Render as 32 bit on
        if(zcheckMouse(ptk, 458, (Cur_Height - 68), 29, 16) && !Allow_32bit)
        {
            rawrender_32float = TRUE;
            ptk->teac = 1;
            ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
        }

        // Render as 32 bit off
        if(zcheckMouse(ptk, 458 + 31, (Cur_Height - 68), 29, 16) && !Allow_32bit)
        {
            rawrender_32float = FALSE;
            ptk->teac = 1;
            ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
        }

        // Render entire song
        if(zcheckMouse(ptk, 534, (Cur_Height - 112), 40, 16))
        {
            rawrender_range = FALSE;
            ptk->teac = 0;
            ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
        }

        // Render a range
        if(zcheckMouse(ptk, 534 + 42, (Cur_Height - 112), 40, 16))
        {
            rawrender_range = TRUE;
            ptk->teac = 0;
            ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
        }

        if(rawrender_range)
        {
            // From position
            if(zcheckMouse(ptk, 572, (Cur_Height - 86), 16, 16) == 1)
            {
                rawrender_from--;
                ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
                ptk->teac = 3;
            }

            // From position
            if(zcheckMouse(ptk, 572 + 44, (Cur_Height - 86), 16, 16) == 1)
            {
                rawrender_from++;
                ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
                ptk->teac = 3;
            }

            // To position
            if(zcheckMouse(ptk, 572, (Cur_Height - 66), 16, 16) == 1)
            {
                rawrender_to--;
                ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
                ptk->teac = 4;
            }

            // To position
            if(zcheckMouse(ptk, 572 + 44, (Cur_Height - 66), 16, 16) == 1)
            {
                rawrender_to++;
                ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
                ptk->teac = 4;
            }
        }

        // Render to wav file
        if(zcheckMouse(ptk, 654, (Cur_Height - 106), 80, 16))
        {
            rawrender_target = RENDER_TO_FILE;
            ptk->teac = 0;
            ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
        }

        // Render to mono sample
        if(zcheckMouse(ptk, 654, (Cur_Height - 88), 80, 16))
        {
            rawrender_target = RENDER_TO_MONO;
            ptk->teac = 0;
            ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
        }

        // Render to stereo sample
        if(zcheckMouse(ptk, 654, (Cur_Height - 70), 80, 16))
        {
            rawrender_target = RENDER_TO_STEREO;
            ptk->teac = 0;
            ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
        }

        // Render as multiple file
        if(zcheckMouse(ptk, 458, (Cur_Height - 49), 29, 16) && !Allow_Single_Render)
        {
            rawrender_multi = TRUE;
            ptk->teac = 5;
            ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
        }

        // Render as single files
        if(zcheckMouse(ptk, 458 + 31, (Cur_Height - 49), 29, 16) && !Allow_Single_Render)
        {
            rawrender_multi = FALSE;
            ptk->teac = 5;
            ptk->gui_action = GUI_CMD_UPDATE_DISKIO_ED;
        }


        Check_Tracks_To_Render(ptk);
    }
}

void Display_Song_Length(ptk_data *ptk)
{
    char ms[64];

    if(ptk->userscreen == USER_SCREEN_DISKIO_EDIT)
    {
        sprintf(ms, "%.2d:%.2d:%.2d", song_Hours, song_Minutes, song_Seconds);
        Gui_Draw_Button_Box(254, (Cur_Height - 58), 80, 16, "", BUTTON_NORMAL | BUTTON_DISABLED);
        PrintString(274, (Cur_Height - 56), USE_FONT, ms);
    }
}

// ------------------------------------------------------
// Check if a track to render has been solo'ed already
int Is_Track_To_Render_Solo(int nbr)
{
    int i;
    int found_tracks = FALSE;

    for(i = 0; i < Songtracks; i++)
    {
        if(i != nbr)
        {
            if(!Tracks_To_Render[i]) found_tracks = TRUE;
        }
    }
    // Yes: unmute all
    if(found_tracks == FALSE) return(TRUE);
    // No: solo it
    return(FALSE);
}

// ------------------------------------------------------
// Display a track gadget with it's number
void Display_1_Track_To_Render(int nbr)
{
    if(Tracks_To_Render[nbr])
    {
        if(nbr < Songtracks)
        {
            Gui_Draw_Button_Box(Tracks_Position[nbr].x, (Cur_Height - 112) + Tracks_Position[nbr].y, 16, 16, Tracks_Labels[nbr], BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
        }
        else
        {
            Gui_Draw_Button_Box(Tracks_Position[nbr].x, (Cur_Height - 112) + Tracks_Position[nbr].y, 16, 16, Tracks_Labels[nbr], BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
        }
    }
    else
    {
        if(nbr < Songtracks)
        {
            Gui_Draw_Button_Box(Tracks_Position[nbr].x, (Cur_Height - 112) + Tracks_Position[nbr].y, 16, 16, Tracks_Labels[nbr], BUTTON_PUSHED | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
        }
        else
        {
            Gui_Draw_Button_Box(Tracks_Position[nbr].x, (Cur_Height - 112) + Tracks_Position[nbr].y, 16, 16, Tracks_Labels[nbr], BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
        }
    }
}

// ------------------------------------------------------
// Display the list of track to render
void Display_Tracks_To_Render(ptk_data *ptk)
{
    int i;

    if(ptk->userscreen == USER_SCREEN_DISKIO_EDIT)
    {
        for(i = 0; i < MAX_TRACKS; i++)
        {
            Display_1_Track_To_Render(i);
        }
    }
}

// ------------------------------------------------------
// Clear tracks to render status
void Reset_Tracks_To_Render(ptk_data *ptk)
{
    int i;
    for(i = 0; i < Songtracks; i++)
    {
        Tracks_To_Render[i] = FALSE;
    }
	if(ptk->start_gui == TRUE) Display_Tracks_To_Render(ptk);
}

// ------------------------------------------------------
// Check if a track can be solo'ed
void Check_Tracks_To_Render_To_Solo(ptk_data *ptk)
{
    int i;
    int j;

    for(i = 0; i < Songtracks; i++)
    {
        if(zcheckMouse(ptk, Tracks_Position[i].x, (Cur_Height - 112) + Tracks_Position[i].y, 17, 16))
        {
            if(Is_Track_To_Render_Solo(i))
            {
                for(j = 0; j < Songtracks; j++)
                {
                    Tracks_To_Render[j] = FALSE;
                }
            }
            else
            {
                for(j = 0; j < Songtracks; j++)
                {
                    Tracks_To_Render[j] = TRUE;
                }
                Tracks_To_Render[i] = FALSE;
            }
        }
    }
    Display_Tracks_To_Render(ptk);
}

// ------------------------------------------------------
// Handle clicks on the tracks gadgets
void Check_Tracks_To_Render(ptk_data *ptk)
{
    int i;

    for(i = 0; i < MAX_TRACKS; i++)
    {
        if(i < Songtracks)
        {
            if(zcheckMouse(ptk, Tracks_Position[i].x, (Cur_Height - 112) + Tracks_Position[i].y, 17, 16))
            {
                Tracks_To_Render[i] ^= TRUE;
                Display_1_Track_To_Render(i);
            }
        }
    }
}
