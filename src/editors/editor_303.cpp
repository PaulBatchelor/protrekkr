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
#include "include/editor_303.h"
#include "../files/include/files.h"
#include "../ui/include/requesters.h"

// ------------------------------------------------------
// Variables
SDL_Surface *SKIN303;

int Current_copy_buffer;
int Copied_Buffer[4];

int tb303_pattern_buffer_full[4];
flag303 tb303_pattern_buffer[4][16];
unsigned char tb303_buffer_tone[4][16];

unsigned char editsteps[2][32];

int Refresh_Unit;

extern REQUESTER Overwrite_Requester;

// ------------------------------------------------------
// Fucntions
void tb303_copy_pattern(void);
void tb303_paste_pattern(void);
void tb303_notes_up(void);
void tb303_notes_down(void);
void Display_Cur_copy_Buffer(void);

void Draw_303_Ed(ptk_data *ptk)
{
    Draw_Editors_Bar(ptk, USER_SCREEN_TB303_EDIT);

    Gui_Draw_Button_Box(0, (Cur_Height - 153), fsize, 130, "", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Flat_Box("303 Units");

    Gui_Draw_Button_Box(8, (Cur_Height - 134), 64, 16, "Reset Patt.", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(8, (Cur_Height - 116), 64, 16, "Clear Patt.", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(8, (Cur_Height - 98), 64, 16, "Rnd.NoteOn", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(8, (Cur_Height - 80), 64, 16, "Rand.Tones", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(8, (Cur_Height - 62), 64, 16, "Rand.Flags", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

    Gui_Draw_Button_Box(600, (Cur_Height - 98), 64, 16, "All Notes Up", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(668, (Cur_Height - 98), 64, 16, "All Notes Dn", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(600, (Cur_Height - 78), 64, 16, "Copy Patt.", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(600, (Cur_Height - 60), 64, 16, "Paste Patt.", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

    Gui_Draw_Button_Box(600, (Cur_Height - 138), 56, 16, "Patt. Name", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(658, (Cur_Height - 138), 34, 16, "Save", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

    Skincopy(ptk, 80, (Cur_Height - 140), 0, 0, 510, 114);

    Gui_Draw_Button_Box(668, (Cur_Height - 60), 64, 16, "Scale", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_NO_BORDER | BUTTON_TEXT_CENTERED);

    Actualize_303_Ed(ptk, 0);
}

void Actualize_303_Ed(ptk_data *ptk, char gode)
{
    if(ptk->userscreen == USER_SCREEN_TB303_EDIT)
    {
        Refresh_303_Unit(ptk, sl3, gode);

        if(gode == 0 || gode == 17)
        {
            Display_Cur_copy_Buffer();
        }
    }
}

void Refresh_303_Unit(ptk_data *ptk, int Unit, int gode)
{
    char tcp[40];

    if(ptk->userscreen == USER_SCREEN_TB303_EDIT)
    {
    
        if(gode == 0 ||
           gode == 1)
        {
            number303(ptk, tb303[Unit].patternlength[tb303[Unit].selectedpattern], 118, (Cur_Height - 44));
        }

        // Selected bassline
        if(gode == 0)
        {
            if(Unit)
            {
                Skincopy(ptk, 577, (Cur_Height - 58), 138, 119, 3, 3);
                Skincopy(ptk, 558, (Cur_Height - 58), 143, 119, 3, 3);
                Gui_Draw_Button_Box(668, (Cur_Height - 78), 64, 16, "Tune to 1", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }
            else
            {
                Skincopy(ptk, 558, (Cur_Height - 58), 138, 119, 3, 3);
                Skincopy(ptk, 577, (Cur_Height - 58), 143, 119, 3, 3);
                Gui_Draw_Button_Box(668, (Cur_Height - 78), 64, 16, "Tune to 2", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }

            // Make sure we display the right boundaries
            if(editsteps[Unit][tb303[Unit].selectedpattern] > tb303[Unit].patternlength[tb303[Unit].selectedpattern] - 1)
            {
                editsteps[Unit][tb303[Unit].selectedpattern] = tb303[Unit].patternlength[tb303[Unit].selectedpattern] - 1;
            }

            number303(ptk, editsteps[Unit][tb303[Unit].selectedpattern] + 1, 486, (Cur_Height - 114));
            // Displaying pattern selection leds
            // Bank [A-D]
            int tbank303 = tb303[Unit].selectedpattern / 8;
            tbank303 *= 15;
            // Restoring background
            Skincopy(ptk, 86, (Cur_Height - 74), 6, 66, 59, 14);
            // Light the bank
            Skincopy(ptk, 86 + tbank303, (Cur_Height - 74), 15 + tbank303, 117, 14, 14);
            // Displaying pattern selection leds
            // Pattern [1-8]
            int tpat303 = tb303[Unit].selectedpattern - (tb303[Unit].selectedpattern / 8) * 8;
            // Restoring background
            Skincopy(ptk, 86, (Cur_Height - 116), 6, 24, 59, 30);
            // Light the bank
            if(tpat303 < 4)
            {
                Skincopy(ptk, 86 + tpat303 * 15, (Cur_Height - 116), 75 + tpat303 * 15, 117, 14, 14);
            }
            else
            {
                tpat303 -= 4;
                Skincopy(ptk, 86 + tpat303 * 15, (Cur_Height - 101), 75 + tpat303 * 15, 132, 14, 14);
            }
        }

        // Displaying waveform switch
        if(gode == 0 || gode == 2)
        {
            if(tb303[Unit].waveform) Skincopy(ptk, 180, (Cur_Height - 128), 137, 135, 13, 8);
            else Skincopy(ptk, 180, (Cur_Height - 128), 137, 125, 13, 8);
        }

        // Draw 303 Knobs
        if(gode == 0 || gode == 3) knob(ptk, 229, (Cur_Height - 124), tb303[Unit].tune / 2);
        if(gode == 0 || gode == 4) knob(ptk, 262, (Cur_Height - 124), tb303[Unit].cutoff / 2);
        if(gode == 0 || gode == 5) knob(ptk, 295, (Cur_Height - 124), tb303[Unit].resonance / 2);
        if(gode == 0 || gode == 6) knob(ptk, 328, (Cur_Height - 124), tb303[Unit].envmod / 2);
        if(gode == 0 || gode == 7) knob(ptk, 361, (Cur_Height - 124), tb303[Unit].decay / 2);
        if(gode == 0 || gode == 8) knob(ptk, 394, (Cur_Height - 124), tb303[Unit].accent / 2);

        // Restoring notes background
        if(gode == 0 || gode == 9)
        {
            // Restore it
            Skincopy(ptk, 88 + 80, 66 + (Cur_Height - 140), 88, 66, 195, 40);

            // Light a note
            switch(tb303[Unit].tone[tb303[Unit].selectedpattern][editsteps[Unit][tb303[Unit].selectedpattern]])
            {
                case 0: Skincopy(ptk, 88 + 80, 91 + (Cur_Height - 140), 301, 119, 13, 13); break;
                case 1: Skincopy(ptk, 101 + 80, 66 + (Cur_Height - 140), 301, 119, 13, 13); break;
                case 2: Skincopy(ptk, 114 + 80, 91 + (Cur_Height - 140), 301, 119, 13, 13); break;
                case 3: Skincopy(ptk, 127 + 80, 66 + (Cur_Height - 140), 301, 119, 13, 13); break;
                case 4: Skincopy(ptk, 140 + 80, 91 + (Cur_Height - 140), 301, 119, 13, 13); break;
                case 5: Skincopy(ptk, 166 + 80, 91 + (Cur_Height - 140), 301, 119, 13, 13); break;
                case 6: Skincopy(ptk, 179 + 80, 66 + (Cur_Height - 140), 301, 119, 13, 13); break;
                case 7: Skincopy(ptk, 191 + 80, 91 + (Cur_Height - 140), 301, 119, 13, 13); break;
                case 8: Skincopy(ptk, 204 + 80, 66 + (Cur_Height - 140), 301, 119, 13, 13); break;
                case 9: Skincopy(ptk, 217 + 80, 91 + (Cur_Height - 140), 301, 119, 13, 13); break;
                case 10: Skincopy(ptk, 230 + 80, 66 + (Cur_Height - 140), 301, 119, 13, 13); break;
                case 11: Skincopy(ptk, 243 + 80, 91 + (Cur_Height - 140), 301, 119, 13, 13); break;
                case 12: Skincopy(ptk, 269 + 80, 91 + (Cur_Height - 140), 301, 119, 13, 13); break;
            }
        }

        // Light pause/note led
        if(gode == 0 || gode == 10)
        {
            if(tb303[Unit].flag[tb303[Unit].selectedpattern][editsteps[Unit][tb303[Unit].selectedpattern]].pause)
            {
                Skincopy(ptk, 402, (Cur_Height - 79), 138, 119, 3, 3);
                Skincopy(ptk, 439, (Cur_Height - 79), 143, 119, 3, 3);
            }
            else
            {
                Skincopy(ptk, 402, (Cur_Height - 79), 143, 119, 3, 3);
                Skincopy(ptk, 439, (Cur_Height - 79), 138, 119, 3, 3);
            }
        }

        // Light slide/off led
        if(gode == 0 || gode == 11)
        {
            if(tb303[Unit].flag[tb303[Unit].selectedpattern][editsteps[Unit][tb303[Unit].selectedpattern]].slide_flag)
            {
                Skincopy(ptk, 456, (Cur_Height - 57), 138, 119, 3, 3);
            }
            else
            {
                Skincopy(ptk, 456, (Cur_Height - 57), 143, 119, 3, 3);
            }
        }

        // Light accent/off led
        if(gode == 0 || gode == 12)
        {
            if(tb303[Unit].flag[tb303[Unit].selectedpattern][editsteps[Unit][tb303[Unit].selectedpattern]].accent_flag)
            {
                Skincopy(ptk, 431, (Cur_Height - 57), 138, 119, 3, 3);
            }
            else
            {
                Skincopy(ptk, 431, (Cur_Height - 57), 143, 119, 3, 3);
            }
        }

        // Transpose up flag
        if(gode == 0 || gode == 13)
        {
            if(tb303[Unit].flag[tb303[Unit].selectedpattern][editsteps[Unit][tb303[Unit].selectedpattern]].transposeup_flag)
            {
                Skincopy(ptk, 406, (Cur_Height - 57), 138, 119, 3, 3);
            }
            else
            {
                Skincopy(ptk, 406, (Cur_Height - 57), 143, 119, 3, 3);
            }
        }

        // Transpose down flag
        if(gode == 0 || gode == 14)
        {
            if(tb303[Unit].flag[tb303[Unit].selectedpattern][editsteps[Unit][tb303[Unit].selectedpattern]].transposedown_flag)
            {
                Skincopy(ptk, 381, (Cur_Height - 57), 138, 119, 3, 3);
            }
            else
            {
                Skincopy(ptk, 381, (Cur_Height - 57), 143, 119, 3, 3);
            }
        }

        // Volume
        if(gode == 0 || gode == 15)
        {
            // volume background
            Skincopy(ptk, 529, (Cur_Height - 115), 449, 25, 19, 88);
            int tb303v = (int) (tb303engine[Unit].tbVolume * 72.0f);
            // Volume slider
            Skincopy(ptk, 531, (Cur_Height - 42) - tb303v, 0, 116, 13, 11);
        }

        if(gode == 0 || gode == 18)
        {
            sprintf(tcp, "%s_", tb303[Unit].pattern_name[tb303[Unit].selectedpattern]);

            if(snamesel == INPUT_303_PATTERN)
            {
                Gui_Draw_Button_Box(600, (Cur_Height - 120), 164, 16, tcp, BUTTON_PUSHED | BUTTON_INPUT);
            }
            else
            {
                Gui_Draw_Button_Box(600, (Cur_Height - 120), 164, 16, tb303[Unit].pattern_name[tb303[Unit].selectedpattern], BUTTON_NORMAL | BUTTON_INPUT);
            }
        }

        if(gode == 0 || gode == 19)
        {
            Gui_Draw_Arrows_Number_Box2(670, (Cur_Height - 42), tb303[Unit].scale, BUTTON_NORMAL | BUTTON_RIGHT_MOUSE | BUTTON_TEXT_CENTERED);
        }

    }
}

void number303(ptk_data *ptk, unsigned char number, int x, int y)
{
    switch(number)
    {
        case 0: Skincopy(ptk, x, y, 156, 118, 7, 14); Skincopy(ptk, x + 8, y, 156, 118, 7, 14); break;
        case 1: Skincopy(ptk, x, y, 156, 118, 7, 14); Skincopy(ptk, x + 8, y, 164, 118, 7, 14); break;
        case 2: Skincopy(ptk, x, y, 156, 118, 7, 14); Skincopy(ptk, x + 8, y, 172, 118, 7, 14); break;
        case 3: Skincopy(ptk, x, y, 156, 118, 7, 14); Skincopy(ptk, x + 8, y, 180, 118, 7, 14); break;
        case 4: Skincopy(ptk, x, y, 156, 118, 7, 14); Skincopy(ptk, x + 8, y, 188, 118, 7, 14); break;
        case 5: Skincopy(ptk, x, y, 156, 118, 7, 14); Skincopy(ptk, x + 8, y, 196, 118, 7, 14); break;
        case 6: Skincopy(ptk, x, y, 156, 118, 7, 14); Skincopy(ptk, x + 8, y, 204, 118, 7, 14); break;
        case 7: Skincopy(ptk, x, y, 156, 118, 7, 14); Skincopy(ptk, x + 8, y, 212, 118, 7, 14); break;
        case 8: Skincopy(ptk, x, y, 156, 118, 7, 14); Skincopy(ptk, x + 8, y, 220, 118, 7, 14); break;
        case 9: Skincopy(ptk, x, y, 156, 118, 7, 14); Skincopy(ptk, x + 8, y, 228, 118, 7, 14); break;
        case 10: Skincopy(ptk, x, y, 164, 118, 7, 14); Skincopy(ptk, x + 8, y, 156, 118, 7, 14); break;
        case 11: Skincopy(ptk, x, y, 164, 118, 7, 14); Skincopy(ptk, x + 8, y, 164, 118, 7, 14); break;
        case 12: Skincopy(ptk, x, y, 164, 118, 7, 14); Skincopy(ptk, x + 8, y, 172, 118, 7, 14); break;
        case 13: Skincopy(ptk, x, y, 164, 118, 7, 14); Skincopy(ptk, x + 8, y, 180, 118, 7, 14); break;
        case 14: Skincopy(ptk, x, y, 164, 118, 7, 14); Skincopy(ptk, x + 8, y, 188, 118, 7, 14); break;
        case 15: Skincopy(ptk, x, y, 164, 118, 7, 14); Skincopy(ptk, x + 8, y, 196, 118, 7, 14); break;
        case 16: Skincopy(ptk, x, y, 164, 118, 7, 14); Skincopy(ptk, x + 8, y, 204, 118, 7, 14); break;
    }
}

void knob(ptk_data *ptk, int x, int y, unsigned char number)
{
    if(number > 62) number = 62;
    Skincopy(ptk, x, y, number * 25, 147, 24, 24);
}

void Mouse_Right_303_Ed(ptk_data *ptk)
{
    if(ptk->userscreen == USER_SCREEN_TB303_EDIT)
    {
        // Volume Slider
        if(zcheckMouse(ptk, 529, (Cur_Height - 115), 19, 88))
        {
            tb303engine[sl3].tbVolume = 0.5f;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 15;
            ptk->liveparam = LIVE_PARAM_303_1_VOLUME + sl3;
            ptk->livevalue = (int) (tb303engine[sl3].tbVolume * 255.0f);
        }

        // Tune Knob
        if(zcheckMouse(ptk, 229, (Cur_Height - 124), 24, 24))
        {
            int tempz = 64;
            tb303[sl3].tune = tempz;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 3;
            ptk->liveparam = LIVE_PARAM_303_1_TUNE + sl3;
            ptk->livevalue = tempz << 1;
        }

        // CutOff Knob
        if(zcheckMouse(ptk, 262, (Cur_Height - 124), 24, 24))
        {
            int tempz = 64;
            tb303[sl3].cutoff = tempz;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 4;
            ptk->liveparam = LIVE_PARAM_303_1_CUTOFF + sl3;
            ptk->livevalue = tempz << 1;
        }

        // Reso knob
        if(zcheckMouse(ptk, 295, (Cur_Height - 124), 24, 24))
        {
            int tempz = 64;
            tb303[sl3].resonance = tempz;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 5;
            ptk->liveparam = LIVE_PARAM_303_1_RESONANCE + sl3;
            ptk->livevalue = tempz << 1;
        }

        // Envmod knob
        if(zcheckMouse(ptk, 328, (Cur_Height - 124), 24, 24))
        {
            int tempz = 64;
            tb303[sl3].envmod = tempz;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 6;
            ptk->liveparam = LIVE_PARAM_303_1_ENVMOD + sl3;
            ptk->livevalue = tempz << 1;
        }

        // Decay knob
        if(zcheckMouse(ptk, 361, (Cur_Height - 124), 24, 24))
        {
            int tempz = 64;
            tb303[sl3].decay = tempz;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 7;
            ptk->liveparam = LIVE_PARAM_303_1_DECAY + sl3;
            ptk->livevalue = tempz << 1;
        }

        // Accent knob
        if(zcheckMouse(ptk, 394, (Cur_Height - 124), 24, 24))
        {
            int tempz = 64;
            tb303[sl3].accent = tempz;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 8;
            ptk->liveparam = LIVE_PARAM_303_1_ACCENT + sl3;
            ptk->livevalue = tempz << 1;
        }

        // Scale
        if(zcheckMouse(ptk, 670, (Cur_Height - 42), 16, 16))
        {
            int value = tb303[sl3].scale;
            value -= 4;
            if(value < 1) value = 1;
            tb303[sl3].scale = value;
            tb303engine[sl3].tbCurMultiple = tb303[sl3].scale;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 19;
        }

        // Scale
        if(zcheckMouse(ptk, 670 + 44, (Cur_Height - 42), 16, 16))
        {
            tb303[sl3].scale += 4;
            if(tb303[sl3].scale > 16) tb303[sl3].scale = 16;
            tb303engine[sl3].tbCurMultiple = tb303[sl3].scale;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 19;
        }
    }
}

void Mouse_Wheel_303_Ed(ptk_data *ptk, int roll_amount)
{
    if(ptk->userscreen == USER_SCREEN_TB303_EDIT)
    {
        // Volume Knob
        if(zcheckMouse(ptk, 529, (Cur_Height - 115), 19, 88))
        {
            float froll = roll_amount / 86.0f;
            float breakvol = tb303engine[sl3].tbVolume + froll;
            if(breakvol < 0.0f) breakvol = 0.0f;
            if(breakvol > 1.0f) breakvol = 1.0f;
            tb303engine[sl3].tbVolume = breakvol;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->liveparam = LIVE_PARAM_303_1_VOLUME + sl3;
            ptk->livevalue = (int) (breakvol * 255.0f);
            ptk->teac = 15;
        }

        // Tune Knob
        if(zcheckMouse(ptk, 229, (Cur_Height - 124), 24, 24))
        {
            int tempz = tb303[sl3].tune + roll_amount;
            if(tempz < 0) tempz = 0;
            if(tempz > 127) tempz = 127;
            tb303[sl3].tune = tempz;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 3;
            ptk->liveparam = LIVE_PARAM_303_1_TUNE + sl3;
            ptk->livevalue = tempz << 1;
        }

        // CutOff Knob
        if(zcheckMouse(ptk, 262, (Cur_Height - 124), 24, 24))
        {
            int tempz = tb303[sl3].cutoff + roll_amount;
            if(tempz < 0) tempz = 0;
            if(tempz > 127) tempz = 127;
            tb303[sl3].cutoff = tempz;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 4;
            ptk->liveparam = LIVE_PARAM_303_1_CUTOFF + sl3;
            ptk->livevalue = tempz << 1;
        }

        // Reso knob
        if(zcheckMouse(ptk, 295, (Cur_Height - 124), 24, 24))
        {
            int tempz = tb303[sl3].resonance + roll_amount;
            if(tempz < 0) tempz = 0;
            if(tempz > 127) tempz = 127;
            tb303[sl3].resonance = tempz;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 5;
            ptk->liveparam = LIVE_PARAM_303_1_RESONANCE + sl3;
            ptk->livevalue = tempz << 1;
        }

        // Envmod knob
        if(zcheckMouse(ptk, 328, (Cur_Height - 124), 24, 24))
        {
            int tempz = tb303[sl3].envmod + roll_amount;
            if(tempz < 0) tempz = 0;
            if(tempz > 127) tempz = 127;
            tb303[sl3].envmod = tempz;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 6;
            ptk->liveparam = LIVE_PARAM_303_1_ENVMOD + sl3;
            ptk->livevalue = tempz << 1;
        }

        // Decay knob
        if(zcheckMouse(ptk, 361, (Cur_Height - 124), 24, 24))
        {
            int tempz = tb303[sl3].decay + roll_amount;
            if(tempz < 0) tempz = 0;
            if(tempz > 127) tempz = 127;
            tb303[sl3].decay = tempz;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 7;
            ptk->liveparam = LIVE_PARAM_303_1_DECAY + sl3;
            ptk->livevalue = tempz << 1;
        }

        // Accent knob
        if(zcheckMouse(ptk, 394, (Cur_Height - 124), 24, 24))
        {
            int tempz = tb303[sl3].accent + roll_amount;
            if(tempz < 0) tempz = 0;
            if(tempz > 127) tempz = 127;
            tb303[sl3].accent = tempz;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 8;
            ptk->liveparam = LIVE_PARAM_303_1_ACCENT + sl3;
            ptk->livevalue = tempz << 1;
        }
    }
}

void Mouse_Left_303_Ed(ptk_data *ptk)
{
    if(ptk->userscreen == USER_SCREEN_TB303_EDIT)
    {
        // Step Forward
        if(zcheckMouse(ptk, 479, (Cur_Height - 50), 25, 16))
        {
            editsteps[sl3][tb303[sl3].selectedpattern]++;
            if(editsteps[sl3][tb303[sl3].selectedpattern] > tb303[sl3].patternlength[tb303[sl3].selectedpattern] - 1)
            {
                editsteps[sl3][tb303[sl3].selectedpattern] = 0;
            }
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 0;
        }

        // Step Back
        if(zcheckMouse(ptk, 484, (Cur_Height - 78), 15, 10))
        {
            if(editsteps[sl3][tb303[sl3].selectedpattern] == 0) editsteps[sl3][tb303[sl3].selectedpattern] = tb303[sl3].patternlength[tb303[sl3].selectedpattern] - 1;
            else editsteps[sl3][tb303[sl3].selectedpattern]--;
            ptk->teac = 0;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
        }

        // Pattern Steps +1
        if(zcheckMouse(ptk, 135, (Cur_Height - 45), 11, 9) &&
           tb303[sl3].patternlength[tb303[sl3].selectedpattern] < 16)
        {
            tb303[sl3].patternlength[tb303[sl3].selectedpattern]++;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 0;
        }

        // Pattern Steps -1
        if(zcheckMouse(ptk, 135, (Cur_Height - 36), 11, 9) &&
           tb303[sl3].patternlength[tb303[sl3].selectedpattern] > 1)
        {
            tb303[sl3].patternlength[tb303[sl3].selectedpattern]--;
            if(editsteps[sl3][tb303[sl3].selectedpattern] > tb303[sl3].patternlength[tb303[sl3].selectedpattern] - 1)
            {
                editsteps[sl3][tb303[sl3].selectedpattern] = tb303[sl3].patternlength[tb303[sl3].selectedpattern] - 1;
            }
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 0;
        }

        // Pause/Note button
        if(zcheckMouse(ptk, 450, (Cur_Height - 82), 15, 10))
        {
            if(tb303[sl3].flag[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]].pause)
            {
                tb303[sl3].flag[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]].pause = 0;
            }
            else
            {
                tb303[sl3].flag[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]].pause = 1;
            }
            ptk->teac = 10;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
        }

        // Glide button
        if(zcheckMouse(ptk, 454, (Cur_Height - 50), 10, 15))
        {
            if(tb303[sl3].flag[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]].slide_flag)
            {
                tb303[sl3].flag[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]].slide_flag = 0;
            }
            else
            {
                tb303[sl3].flag[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]].slide_flag = 1;
            }
            ptk->teac = 11;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
        }

        // Accent button
        if(zcheckMouse(ptk, 428, (Cur_Height - 50), 10, 15))
        {
            if(tb303[sl3].flag[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]].accent_flag)
            {
                tb303[sl3].flag[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]].accent_flag = 0;
            }
            else
            {
                tb303[sl3].flag[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]].accent_flag = 1;
            }
            ptk->teac = 12;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
        }

        // Transpose up button
        if(zcheckMouse(ptk, 403, (Cur_Height - 50), 10, 15))
        {
            if(tb303[sl3].flag[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]].transposeup_flag)
            {
                tb303[sl3].flag[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]].transposeup_flag = 0;
            }
            else
            {
                tb303[sl3].flag[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]].transposeup_flag = 1;
            }
            ptk->teac = 13;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
        }

        // Transpose down button
        if(zcheckMouse(ptk, 378, (Cur_Height - 50), 10, 15))
        {
            if(tb303[sl3].flag[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]].transposedown_flag)
            {
                tb303[sl3].flag[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]].transposedown_flag = 0;
            }
            else
            {
                tb303[sl3].flag[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]].transposedown_flag = 1;
            }
            ptk->teac = 14;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
        }

        // Sawtooth/Square switching
        if(zcheckMouse(ptk, 177, (Cur_Height - 129), 19, 10))
        {
            tb303[sl3].waveform ^= 1;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 2;
        }

        // NoteOn/Off buttons
        if(zcheckMouse(ptk, 87 + 80, (Cur_Height - 50), 17, 15))
        {
            tb303[sl3].tone[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]] = 0;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 9;
        }
        if(zcheckMouse(ptk, 100 + 80, (Cur_Height - 76), 17, 15))
        {
            tb303[sl3].tone[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]] = 1;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 9;
        }
        if(zcheckMouse(ptk, 113 + 80, (Cur_Height - 50), 17, 15))
        {
            tb303[sl3].tone[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]] = 2;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 9;
        }
        if(zcheckMouse(ptk, 126 + 80, (Cur_Height - 76), 17, 15))
        {
            tb303[sl3].tone[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]] = 3;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 9;
        }
        if(zcheckMouse(ptk, 139 + 80, (Cur_Height - 50), 17, 15))
        {
            tb303[sl3].tone[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]] = 4;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 9;
        }
        if(zcheckMouse(ptk, 165 + 80, (Cur_Height - 50), 17, 15))
        {
            tb303[sl3].tone[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]] = 5;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 9;
        }
        if(zcheckMouse(ptk, 178 + 80, (Cur_Height - 76), 17, 15))
        {
            tb303[sl3].tone[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]] = 6;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 9;
        }
        if(zcheckMouse(ptk, 190 + 80, (Cur_Height - 50), 17, 15))
        {
            tb303[sl3].tone[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]] = 7;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 9;
        }
        if(zcheckMouse(ptk, 203 + 80, (Cur_Height - 76), 17, 15))
        {
            tb303[sl3].tone[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]] = 8;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 9;
        }
        if(zcheckMouse(ptk, 216 + 80, (Cur_Height - 50), 17, 15))
        {
            tb303[sl3].tone[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]] = 9;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 9;
        }

        if(zcheckMouse(ptk, 229 + 80, (Cur_Height - 76), 87, 15))
        {
            tb303[sl3].tone[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]] = 10;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 9;
        }
        if(zcheckMouse(ptk, 242 + 80, (Cur_Height - 50), 17, 15))
        {
            tb303[sl3].tone[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]] = 11;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 9;
        }
        if(zcheckMouse(ptk, 268 + 80, (Cur_Height - 50), 17, 15))
        {
            tb303[sl3].tone[tb303[sl3].selectedpattern][editsteps[sl3][tb303[sl3].selectedpattern]] = 12;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 9;
        }

        // Pattern selection buttons
        if(zcheckMouse(ptk, 86, (Cur_Height - 117), 16, 16))
        {
            char gcp = (tb303[sl3].selectedpattern / 8) * 8;
            tb303[sl3].selectedpattern = gcp;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 0;
        }
        if(zcheckMouse(ptk, 100, (Cur_Height - 117), 16, 16))
        {
            char gcp = (tb303[sl3].selectedpattern / 8) * 8;
            tb303[sl3].selectedpattern = gcp + 1;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 0;
        }
        if(zcheckMouse(ptk, 114, (Cur_Height - 117), 16, 16))
        {
            char gcp = (tb303[sl3].selectedpattern / 8) * 8;
            tb303[sl3].selectedpattern = gcp + 2;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 0;
        }
        if(zcheckMouse(ptk, 130, (Cur_Height - 117), 16, 16))
        {
            char gcp = (tb303[sl3].selectedpattern / 8) * 8;
            tb303[sl3].selectedpattern = gcp + 3;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 0;
        }
        if(zcheckMouse(ptk, 86, (Cur_Height - 102), 16, 16))
        {
            char gcp = (tb303[sl3].selectedpattern / 8) * 8;
            tb303[sl3].selectedpattern = gcp + 4;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 0;
        }
        if(zcheckMouse(ptk, 100, (Cur_Height - 102), 16, 16))
        {
            char gcp = (tb303[sl3].selectedpattern / 8) * 8;
            tb303[sl3].selectedpattern = gcp + 5;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 0;
        }
        if(zcheckMouse(ptk, 114, (Cur_Height - 102), 16, 16))
        {
            char gcp = (tb303[sl3].selectedpattern / 8) * 8;
            tb303[sl3].selectedpattern = gcp + 6;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 0;
        }
        if(zcheckMouse(ptk, 130, (Cur_Height - 102), 16, 16))
        {
            char gcp = (tb303[sl3].selectedpattern / 8) * 8;
            tb303[sl3].selectedpattern = gcp + 7;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 0;
        }

        // Bank selection buttons
        if(zcheckMouse(ptk, 86, (Cur_Height - 75), 16, 16))
        {
            tb303[sl3].selectedpattern = (tb303[sl3].selectedpattern - (tb303[sl3].selectedpattern / 8) * 8);
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 0;
        }
        if(zcheckMouse(ptk, 100, (Cur_Height - 75), 16, 16))
        {
            tb303[sl3].selectedpattern = ((tb303[sl3].selectedpattern - (tb303[sl3].selectedpattern / 8) * 8)) + 8;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 0;
        }
        if(zcheckMouse(ptk, 114, (Cur_Height - 75), 16, 16))
        {
            tb303[sl3].selectedpattern = ((tb303[sl3].selectedpattern - (tb303[sl3].selectedpattern / 8) * 8)) + 16;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 0;
        }
        if(zcheckMouse(ptk, 130, (Cur_Height - 75), 16, 16))
        {
            tb303[sl3].selectedpattern = ((tb303[sl3].selectedpattern - (tb303[sl3].selectedpattern / 8) * 8)) + 24;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 0;
        }

        // 303 #1
        if(zcheckMouse(ptk, 553, (Cur_Height - 71), 15, 12))
        {
            sl3 = 0;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 0;
        }
        // 303 #2
        if(zcheckMouse(ptk, 573, (Cur_Height - 71), 15, 12))
        {
            sl3 = 1;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 0;
        }

        // Rand tones
        if(zcheckMouse(ptk, 8, (Cur_Height - 80), 64, 16))
        {
            for(char alter = 0; alter < 16; alter++)
            {
                tb303[sl3].tone[tb303[sl3].selectedpattern][alter] = (rand() % 13);
            }
            ptk->teac = 0;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
        }

        // rand flags
        if(zcheckMouse(ptk, 8, (Cur_Height - 62), 64, 16))
        {
            for(char alter = 0; alter < 16; alter++)
            {
                tb303[sl3].flag[tb303[sl3].selectedpattern][alter].slide_flag = rand() % 2;
                tb303[sl3].flag[tb303[sl3].selectedpattern][alter].transposeup_flag = rand() % 2;
                tb303[sl3].flag[tb303[sl3].selectedpattern][alter].transposedown_flag = rand() % 2;
                tb303[sl3].flag[tb303[sl3].selectedpattern][alter].accent_flag = rand() % 2;
            }
            ptk->teac = 0;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
        }

        // Rand noteon
        if(zcheckMouse(ptk, 8, (Cur_Height - 98), 64, 16))
        {
            for(char alter = 0; alter < 16; alter++)
            {
                tb303[sl3].flag[tb303[sl3].selectedpattern][alter].pause = rand() % 2;
            }
            ptk->teac = 0;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
        }

        // Clear pattern
        if(zcheckMouse(ptk, 8, (Cur_Height - 116), 64, 16))
        {
            for(char alter = 0; alter < 16; alter++)
            {
                tb303[sl3].tone[tb303[sl3].selectedpattern][alter] = 0;
                tb303[sl3].flag[tb303[sl3].selectedpattern][alter].pause = 0;
                tb303[sl3].flag[tb303[sl3].selectedpattern][alter].slide_flag = 0;
                tb303[sl3].flag[tb303[sl3].selectedpattern][alter].transposeup_flag = 0;
                tb303[sl3].flag[tb303[sl3].selectedpattern][alter].transposedown_flag = 0;
                tb303[sl3].flag[tb303[sl3].selectedpattern][alter].accent_flag = 0;
            }
            ptk->teac = 0;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
        }

        // Reset pattern
        if(zcheckMouse(ptk, 8, (Cur_Height - 134), 64, 16))
        {
            for(char alter = 0; alter < 16; alter++)
            {
                tb303[sl3].tone[tb303[sl3].selectedpattern][alter] = 0;
                tb303[sl3].flag[tb303[sl3].selectedpattern][alter].pause = 1;
                tb303[sl3].flag[tb303[sl3].selectedpattern][alter].slide_flag = 0;
                tb303[sl3].flag[tb303[sl3].selectedpattern][alter].transposeup_flag = 0;
                tb303[sl3].flag[tb303[sl3].selectedpattern][alter].transposedown_flag = 0;
                tb303[sl3].flag[tb303[sl3].selectedpattern][alter].accent_flag = 0;
            }
            ptk->teac = 0;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
        }

        // All notes up
        if(zcheckMouse(ptk, 600, (Cur_Height - 98), 64, 16))
        {
            tb303_notes_up();
            ptk->teac = 0;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
        }

        // All notes down
        if(zcheckMouse(ptk, 668, (Cur_Height - 98), 64, 16))
        {
            tb303_notes_down();
            ptk->teac = 0;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
        }

        // Copy pattern
        if(zcheckMouse(ptk, 600, (Cur_Height - 78), 64, 16))
        {
            tb303_copy_pattern();
            ptk->teac = 0;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
        }

        // Paste pattern
        if(zcheckMouse(ptk, 600, (Cur_Height - 60), 64, 16))
        {
            tb303_paste_pattern();
            ptk->teac = 0;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
        }

        // Select the copy buffers
        if(zcheckMouse(ptk, 600, (Cur_Height - 42), 15, 16))
        {
            Current_copy_buffer = 0;
            ptk->teac = 17;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
        }
        if(zcheckMouse(ptk, 616, (Cur_Height - 42), 15, 16))
        {
            Current_copy_buffer = 1;
            ptk->teac = 17;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
        }
        if(zcheckMouse(ptk, 633, (Cur_Height - 42), 15, 16))
        {
            Current_copy_buffer = 2;
            ptk->teac = 17;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
        }
        if(zcheckMouse(ptk, 649, (Cur_Height - 42), 15, 16))
        {
            Current_copy_buffer = 3;
            ptk->teac = 17;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
        }

        // Start name input
        if(zcheckMouse(ptk, 600, (Cur_Height - 120), 164, 16) && snamesel == INPUT_NONE)
        {
            snamesel = INPUT_303_PATTERN;
            strcpy(cur_input_name, tb303[sl3].pattern_name[tb303[sl3].selectedpattern]);
            namesize = 0;
            sprintf(tb303[sl3].pattern_name[tb303[sl3].selectedpattern], "");
            ptk->teac = 18;
            ptk->gui_action = GUI_CMD_UPDATE_MIDI_303_ED;
        }

        // Save the data
        if(zcheckMouse(ptk, 658, (Cur_Height - 138), 34, 16))
        {
            if(File_Exist_Req(ptk, "%s"SLASH"%s.303", Dir_Patterns, tb303[sl3].pattern_name[tb303[sl3].selectedpattern]))
            {
                Display_Requester(ptk, &Overwrite_Requester, GUI_CMD_SAVE_303_PATTERN);
            }
            else
            {
                ptk->gui_action = GUI_CMD_SAVE_303_PATTERN;
            }
        }

        // Tune to the other unit
        if(zcheckMouse(ptk, 668, (Cur_Height - 78), 64, 16))
        {
            if(sl3)
            {
                tb303[1].tune = tb303[0].tune;
            }
            else
            {
                tb303[0].tune = tb303[1].tune;
            }
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 3;
        }

        // Scale
        if(zcheckMouse(ptk, 670, (Cur_Height - 42), 16, 16))
        {
            tb303[sl3].scale--;
            if(tb303[sl3].scale < 1) tb303[sl3].scale = 1;
            tb303engine[sl3].tbCurMultiple = tb303[sl3].scale;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 19;
        }

        // Scale
        if(zcheckMouse(ptk, 670 + 44, (Cur_Height - 42), 16, 16))
        {
            tb303[sl3].scale++;
            if(tb303[sl3].scale > 16) tb303[sl3].scale = 16;
            tb303engine[sl3].tbCurMultiple = tb303[sl3].scale;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 19;
        }

    }
}

void Mouse_Sliders_303_Ed(ptk_data *ptk)
{
    if(ptk->userscreen == USER_SCREEN_TB303_EDIT)
    {
        // Volume Knob
        if(zcheckMouse(ptk, 529, (Cur_Height - 115), 19, 88))
        {
            float breakvol = (float) (80 - (Mouse.y - (Cur_Height - 115)));
            breakvol /= 72.0f;
            if(breakvol < 0.0f) breakvol = 0.0f;
            if(breakvol > 1.0f) breakvol = 1.0f;
            tb303engine[sl3].tbVolume = breakvol;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 15;
            ptk->liveparam = LIVE_PARAM_303_1_VOLUME + sl3;
            ptk->livevalue = (int) (breakvol * 255.0f);
        }

        // Tune Knob
        if(zcheckMouse(ptk, 229, (Cur_Height - 124), 24, 24))
        {
            if(ptk->fluzy == -1) ptk->fluzy = (Mouse.y * 3) + tb303[sl3].tune;

            int tempz = ptk->fluzy - (Mouse.y * 3);
            if(tempz < 0) tempz = 0;
            if(tempz > 127) tempz = 127;
            tb303[sl3].tune = tempz;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 3;
            ptk->liveparam = LIVE_PARAM_303_1_TUNE + sl3;
            ptk->livevalue = tempz << 1;
        }

        // CutOff Knob
        if(zcheckMouse(ptk, 262, (Cur_Height - 124), 24, 24))
        {
            if(ptk->fluzy == -1) ptk->fluzy = (Mouse.y * 3) + tb303[sl3].cutoff;

            int tempz = ptk->fluzy - (Mouse.y * 3);
            if(tempz < 0) tempz = 0;
            if(tempz > 127) tempz = 127;
            tb303[sl3].cutoff = tempz;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 4;
            ptk->liveparam = LIVE_PARAM_303_1_CUTOFF + sl3;
            ptk->livevalue = tempz << 1;
        }

        // Reso knob
        if(zcheckMouse(ptk, 295, (Cur_Height - 124), 24, 24))
        {
            if(ptk->fluzy == -1) ptk->fluzy = (Mouse.y * 3) + tb303[sl3].resonance;

            int tempz = ptk->fluzy - (Mouse.y * 3);
            if(tempz < 0) tempz = 0;
            if(tempz > 127) tempz = 127;
            tb303[sl3].resonance = tempz;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 5;
            ptk->liveparam = LIVE_PARAM_303_1_RESONANCE + sl3;
            ptk->livevalue = tempz << 1;
        }

        // Envmod knob
        if(zcheckMouse(ptk, 328, (Cur_Height - 124), 24, 24))
        {
            if(ptk->fluzy == -1) ptk->fluzy = (Mouse.y * 3) + tb303[sl3].envmod;

            int tempz = ptk->fluzy - (Mouse.y * 3);
            if(tempz < 0) tempz = 0;
            if(tempz > 127) tempz = 127;
            tb303[sl3].envmod = tempz;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 6;
            ptk->liveparam = LIVE_PARAM_303_1_ENVMOD + sl3;
            ptk->livevalue = tempz << 1;
        }

        // Decay knob
        if(zcheckMouse(ptk, 361, (Cur_Height - 124), 24, 24))
        {
            if(ptk->fluzy == -1) ptk->fluzy = (Mouse.y * 3) + tb303[sl3].decay;

            int tempz = ptk->fluzy - (Mouse.y * 3);
            if(tempz < 0) tempz = 0;
            if(tempz > 127) tempz = 127;
            tb303[sl3].decay = tempz;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 7;
            ptk->liveparam = LIVE_PARAM_303_1_DECAY + sl3;
            ptk->livevalue = tempz << 1;
        }

        // Accent knob
        if(zcheckMouse(ptk, 394, (Cur_Height - 124), 24, 24))
        {
            if(ptk->fluzy == -1) ptk->fluzy = (Mouse.y * 3) + tb303[sl3].accent;

            int tempz = ptk->fluzy - (Mouse.y * 3);
            if(tempz < 0) tempz = 0;
            if(tempz > 127) tempz = 127;
            tb303[sl3].accent = tempz;
            ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
            ptk->teac = 8;
            ptk->liveparam = LIVE_PARAM_303_1_ACCENT + sl3;
            ptk->livevalue = tempz << 1;
        }
    }
}

void Skincopy(ptk_data *ptk, int xd, int yd, int xs, int ys, int w, int h)
{
    Copy(SKIN303, xd, yd, xs, ys, xs + w, ys + h);
}

void tb303_copy_pattern(void)
{
    tb303_pattern_buffer_full[Current_copy_buffer] = TRUE;
    for(char alter = 0; alter < 16; alter++)
    {
        tb303_buffer_tone[Current_copy_buffer][alter] = tb303[sl3].tone[tb303[sl3].selectedpattern][alter];
        tb303_pattern_buffer[Current_copy_buffer][alter].pause = tb303[sl3].flag[tb303[sl3].selectedpattern][alter].pause;
        tb303_pattern_buffer[Current_copy_buffer][alter].slide_flag = tb303[sl3].flag[tb303[sl3].selectedpattern][alter].slide_flag;
        tb303_pattern_buffer[Current_copy_buffer][alter].transposeup_flag = tb303[sl3].flag[tb303[sl3].selectedpattern][alter].transposeup_flag;
        tb303_pattern_buffer[Current_copy_buffer][alter].transposedown_flag = tb303[sl3].flag[tb303[sl3].selectedpattern][alter].transposedown_flag;
        tb303_pattern_buffer[Current_copy_buffer][alter].accent_flag = tb303[sl3].flag[tb303[sl3].selectedpattern][alter].accent_flag;
    }
    Copied_Buffer[Current_copy_buffer] = TRUE;
    Display_Cur_copy_Buffer();
}

void tb303_paste_pattern(void)
{
    if(Copied_Buffer[Current_copy_buffer])
    {
        for(char alter = 0; alter < 16; alter++)
        {
            tb303[sl3].tone[tb303[sl3].selectedpattern][alter] = tb303_buffer_tone[Current_copy_buffer][alter];
            tb303[sl3].flag[tb303[sl3].selectedpattern][alter].pause = tb303_pattern_buffer[Current_copy_buffer][alter].pause;
            tb303[sl3].flag[tb303[sl3].selectedpattern][alter].slide_flag = tb303_pattern_buffer[Current_copy_buffer][alter].slide_flag;
            tb303[sl3].flag[tb303[sl3].selectedpattern][alter].transposeup_flag = tb303_pattern_buffer[Current_copy_buffer][alter].transposeup_flag;
            tb303[sl3].flag[tb303[sl3].selectedpattern][alter].transposedown_flag = tb303_pattern_buffer[Current_copy_buffer][alter].transposedown_flag;
            tb303[sl3].flag[tb303[sl3].selectedpattern][alter].accent_flag = tb303_pattern_buffer[Current_copy_buffer][alter].accent_flag;
        }
    }
}

void tb303_notes_up(void)
{
    for(char alter = 0; alter < 16; alter++)
    {
        if(tb303[sl3].tone[tb303[sl3].selectedpattern][alter] != 12)
        {
            tb303[sl3].tone[tb303[sl3].selectedpattern][alter]++;
        }
    }
}

void tb303_notes_down(void)
{
    for(char alter = 0; alter < 16; alter++)
    {
        if(tb303[sl3].tone[tb303[sl3].selectedpattern][alter])
        {
            tb303[sl3].tone[tb303[sl3].selectedpattern][alter]--;
        }
    }
}

void Display_Cur_copy_Buffer(void)
{
    int highlight[4] =
    {
        BUTTON_NORMAL,
        BUTTON_NORMAL,
        BUTTON_NORMAL,
        BUTTON_NORMAL
    };
    highlight[Current_copy_buffer] = BUTTON_PUSHED;
    Gui_Draw_Button_Box(600, (Cur_Height - 42), 15, 16, "1", highlight[0] | BUTTON_TEXT_CENTERED | (tb303_pattern_buffer_full[0] ? 0 : BUTTON_LOW_FONT));
    Gui_Draw_Button_Box(616, (Cur_Height - 42), 15, 16, "2", highlight[1] | BUTTON_TEXT_CENTERED | (tb303_pattern_buffer_full[1] ? 0 : BUTTON_LOW_FONT));
    Gui_Draw_Button_Box(633, (Cur_Height - 42), 15, 16, "3", highlight[2] | BUTTON_TEXT_CENTERED | (tb303_pattern_buffer_full[2] ? 0 : BUTTON_LOW_FONT));
    Gui_Draw_Button_Box(649, (Cur_Height - 42), 15, 16, "4", highlight[3] | BUTTON_TEXT_CENTERED | (tb303_pattern_buffer_full[3] ? 0 : BUTTON_LOW_FONT));
}
