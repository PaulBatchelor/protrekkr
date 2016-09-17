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
#include "replay.h"
#include "ptk_data.h"
#include "editor_track_fx.h"

// ------------------------------------------------------
// Variables
char fld_chan = FALSE;
extern EQSTATE EqDat[MAX_TRACKS];

// ------------------------------------------------------
// Functions
void Display_Track_Compressor(ptk_data *ptk);
void Display_Track_Volume(ptk_data *ptk);

void Draw_Track_Fx_Ed(ptk_data *ptk)
{
    Draw_Editors_Bar(ptk, USER_SCREEN_TRACK_FX_EDIT);

    Gui_Draw_Button_Box(0, (Cur_Height - 153), ptk->fsize, 130, "", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Flat_Box("Track: Properties, Flanger & Compressor");

    Gui_Draw_Button_Box(4, (Cur_Height - 138), 228, 110, "", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_VTOP);

    Gui_Draw_Button_Box(8, (Cur_Height - 128), 64, 16, "Filter LFO", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(118, (Cur_Height - 128), 64, 16, "Flanger", BUTTON_NORMAL | BUTTON_DISABLED);

    Gui_Draw_Button_Box(8, (Cur_Height - 110), 64, 16, "Frequency", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(8, (Cur_Height - 92), 64, 16, "Amplitude", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(8, (Cur_Height - 74), 64, 16, "LFO Status", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(8, (Cur_Height - 49), 64, 16, "Flanger 3D", BUTTON_NORMAL | BUTTON_DISABLED);

    Gui_Draw_Button_Box(240, (Cur_Height - 138), 288, 110, "Flanger Settings", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_VTOP);
    Gui_Draw_Button_Box(248, (Cur_Height - 121), 56, 16, "Amount", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(248, (Cur_Height - 103), 56, 16, "Period", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(248, (Cur_Height - 85), 56, 16, "Amplitude", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(248, (Cur_Height - 67), 56, 16, "Feedback", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(248, (Cur_Height - 49), 56, 16, "Delay", BUTTON_NORMAL | BUTTON_DISABLED);

    Gui_Draw_Button_Box(536, (Cur_Height - 138), 144, 76, "Compressor", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_VTOP);
    Gui_Draw_Button_Box(544, (Cur_Height - 121), 56, 16, "Active", BUTTON_NORMAL | BUTTON_DISABLED);

    Gui_Draw_Button_Box(536, (Cur_Height - 58), 144, 30, "", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_VTOP);
    
    Gui_Draw_Button_Box(690, (Cur_Height - 138), 100, 110, "Equalizer", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_VTOP);

    Gui_Draw_Button_Box(710, (Cur_Height - 55), 16, 16, "C", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(710 + (22 * 1), (Cur_Height - 55), 16, 16, "C", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(710 + (22 * 2), (Cur_Height - 55), 16, 16, "C", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

    Gui_Draw_Button_Box(710, (Cur_Height - 40), 16, 16, L_ O_, BUTTON_NORMAL | BUTTON_NO_BORDER | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(710 + (22 * 1), (Cur_Height - 40), 16, 16, M_ E_ D_, BUTTON_NORMAL | BUTTON_NO_BORDER | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(710 + (22 * 2), (Cur_Height - 40), 16, 16, H_ I_, BUTTON_NORMAL | BUTTON_NO_BORDER | BUTTON_TEXT_CENTERED);
}

void Actualize_Track_Fx_Ed(ptk_data *ptk, char gode)
{
    if(ptk->userscreen == USER_SCREEN_TRACK_FX_EDIT)
    {
        if(gode == 0 || gode == 1 || gode == 11)
        {
            if(FLANGER_AMOUNT[ptk->Track_Under_Caret] > 1.0f) FLANGER_AMOUNT[ptk->Track_Under_Caret] = 1.0f;
            if(FLANGER_AMOUNT[ptk->Track_Under_Caret] < -1.0f) FLANGER_AMOUNT[ptk->Track_Under_Caret] = -1.0f;
            Realslider(308, (Cur_Height - 121), 64 + (int) (FLANGER_AMOUNT[ptk->Track_Under_Caret] * 64.0f), FLANGER_ON[ptk->Track_Under_Caret]);
            outlong(458, (Cur_Height - 121), long(FLANGER_AMOUNT[ptk->Track_Under_Caret] * 100.0f), 1);
        }

        if(gode == 0 || gode == 7 || gode == 11)
        {
            if(FLANGER_FEEDBACK[ptk->Track_Under_Caret] > 0.9f) FLANGER_FEEDBACK[ptk->Track_Under_Caret] = 0.9f;
            if(FLANGER_FEEDBACK[ptk->Track_Under_Caret] < -1.0f) FLANGER_FEEDBACK[ptk->Track_Under_Caret] = -1.0f;
            Realslider(308, (Cur_Height - 67), 64 + (int) (FLANGER_FEEDBACK[ptk->Track_Under_Caret] * 64.0f), FLANGER_ON[ptk->Track_Under_Caret]);
            outlong(458, (Cur_Height - 67), long(FLANGER_FEEDBACK[ptk->Track_Under_Caret] * 100.0f), 1);
        }

        if(gode == 0 || gode == 4 || gode == 11)
        {
            if(FLANGER_DEPHASE[ptk->Track_Under_Caret] > PIf) FLANGER_DEPHASE[ptk->Track_Under_Caret] = PIf;
            if(FLANGER_DEPHASE[ptk->Track_Under_Caret] < 0.0f) FLANGER_DEPHASE[ptk->Track_Under_Caret] = 0.0f;
            Realslider2(74, (Cur_Height - 49), (int) (FLANGER_DEPHASE[ptk->Track_Under_Caret] * 20.371833f), FLANGER_ON[ptk->Track_Under_Caret]);
            outlong(159, (Cur_Height - 49), (int) (FLANGER_DEPHASE[ptk->Track_Under_Caret] * 57.29578f), 6);
        }

        if(gode == 0 || gode == 5 || gode == 11)
        {
            if(FLANGER_RATE[ptk->Track_Under_Caret] < 0.000001f) FLANGER_RATE[ptk->Track_Under_Caret] = 0.000001f;
            if(FLANGER_RATE[ptk->Track_Under_Caret] > 0.0001363f) FLANGER_RATE[ptk->Track_Under_Caret] = 0.0001363f;
            Realslider(308, (Cur_Height - 103), (int) (FLANGER_RATE[ptk->Track_Under_Caret] * 939104.92f), FLANGER_ON[ptk->Track_Under_Caret]);
            outlong(458, (Cur_Height - 103), long(0.1424758f / FLANGER_RATE[ptk->Track_Under_Caret]), 2);
        }

        if(gode == 0 || gode == 6 || gode == 11)
        {
            if(FLANGER_AMPL[ptk->Track_Under_Caret] > 0.01f) FLANGER_AMPL[ptk->Track_Under_Caret] = 0.01f;
            if(FLANGER_AMPL[ptk->Track_Under_Caret] < 0.0f) FLANGER_AMPL[ptk->Track_Under_Caret] = 0.0f;
            Realslider(308, (Cur_Height - 85), (int) (FLANGER_AMPL[ptk->Track_Under_Caret] * 12800.0f), FLANGER_ON[ptk->Track_Under_Caret]);
            outlong(458, (Cur_Height - 85), (int) (FLANGER_AMPL[ptk->Track_Under_Caret] * 10000.0f), 1);
        }

        if(gode == 0 || gode == 2 || gode == 11)
        {
            if(LFO_RATE[ptk->Track_Under_Caret] < 0.0001f) LFO_RATE[ptk->Track_Under_Caret] = 0.0001f;
            if(LFO_RATE[ptk->Track_Under_Caret] > 0.0078125f) LFO_RATE[ptk->Track_Under_Caret] = 0.0078125f;
            Realslider(74, (Cur_Height - 110), (int) (LFO_RATE[ptk->Track_Under_Caret] * 16384.0f), LFO_ON[ptk->Track_Under_Caret]);
            float tmprate = (8.1632653f / LFO_RATE[ptk->Track_Under_Caret]);
            outlong(76, (Cur_Height - 74), (long) tmprate, 2);
            tmprate = 1000.0f / tmprate;
            outfloat(138, (Cur_Height - 74), tmprate, 3);
        }

        if(gode == 0 || gode == 3 || gode == 11)
        {
            if(LFO_AMPL[ptk->Track_Under_Caret] < 0) LFO_AMPL[ptk->Track_Under_Caret] = 0;
            if(LFO_AMPL[ptk->Track_Under_Caret] > 128) LFO_AMPL[ptk->Track_Under_Caret] = 128;
            Realslider(74, (Cur_Height - 92), (int) (LFO_AMPL[ptk->Track_Under_Caret]), LFO_ON[ptk->Track_Under_Caret]);
        }

        if(gode == 0 || gode == 9 || gode == 11)
        {
            if(LFO_ON[ptk->Track_Under_Caret] == 1)
            {
                Gui_Draw_Button_Box(74, (Cur_Height - 128), 20, 16, "On", BUTTON_PUSHED);
                Gui_Draw_Button_Box(96, (Cur_Height - 128), 20, 16, "Off", BUTTON_NORMAL);
            }
            else
            {
                Gui_Draw_Button_Box(74, (Cur_Height - 128), 20, 16, "On", BUTTON_NORMAL);
                Gui_Draw_Button_Box(96, (Cur_Height - 128), 20, 16, "Off", BUTTON_PUSHED);
            }
        }

        if(gode == 0 || gode == 10 || gode == 11)
        {
            if(FLANGER_ON[ptk->Track_Under_Caret])
            {
                Gui_Draw_Button_Box(184, (Cur_Height - 128), 20, 16, "On", BUTTON_PUSHED);
                Gui_Draw_Button_Box(206, (Cur_Height - 128), 20, 16, "Off", BUTTON_NORMAL);
            }
            else
            {
                Gui_Draw_Button_Box(184, (Cur_Height - 128), 20, 16, "On", BUTTON_NORMAL);
                Gui_Draw_Button_Box(206, (Cur_Height - 128), 20, 16, "Off", BUTTON_PUSHED);
            }
        }

        if(gode == 0 || gode == 8 || gode == 11)
        {
            if(FLANGER_DELAY[ptk->Track_Under_Caret] > 4096) FLANGER_DELAY[ptk->Track_Under_Caret] = 4096;
            if(FLANGER_DELAY[ptk->Track_Under_Caret] < 0) FLANGER_DELAY[ptk->Track_Under_Caret] = 0;
            if(fld_chan == TRUE)
            {
                FLANGER_OFFSET[ptk->Track_Under_Caret] = 8192;
                FLANGER_OFFSET2[ptk->Track_Under_Caret] = float(FLANGER_OFFSET[ptk->Track_Under_Caret] - FLANGER_DELAY[ptk->Track_Under_Caret]);
                FLANGER_OFFSET1[ptk->Track_Under_Caret] = float(FLANGER_OFFSET[ptk->Track_Under_Caret] - FLANGER_DELAY[ptk->Track_Under_Caret]);  
                fld_chan = FALSE;
            }
            Realslider(308, (Cur_Height - 49), FLANGER_DELAY[ptk->Track_Under_Caret] / 32, FLANGER_ON[ptk->Track_Under_Caret]);
            outlong(458, (Cur_Height - 49), long(FLANGER_DELAY[ptk->Track_Under_Caret] / 44.1f), 2);
        }

        if(gode == 0 || gode == 12)
        {
            if(Compress_Track[ptk->Track_Under_Caret])
            {
                Gui_Draw_Button_Box(602, (Cur_Height - 121), 20, 16, "On", BUTTON_PUSHED);
                Gui_Draw_Button_Box(624, (Cur_Height - 121), 20, 16, "Off", BUTTON_NORMAL);
            }
            else
            {
                Gui_Draw_Button_Box(602, (Cur_Height - 121), 20, 16, "On", BUTTON_NORMAL);
                Gui_Draw_Button_Box(624, (Cur_Height - 121), 20, 16, "Off", BUTTON_PUSHED);
            }
        }

        if(gode == 0 || gode == 12)
        {
            Display_Track_Compressor(ptk);
        }

        if(gode == 0 || gode == 13)
        {
            Display_Track_Volume(ptk);
        }

        if(gode == 0 || gode == 14)
        {
            Realslider_Vert(710, (Cur_Height - 120),
                            (int) ((2.0f - EqDat[ptk->Track_Under_Caret].lg) * 50.0f),
                            16,
                            100 + 16,
                            64,
                            TRUE);

            Realslider_Vert(710 + (22 * 1), (Cur_Height - 120),
                            (int) ((2.0f - EqDat[ptk->Track_Under_Caret].mg) * 50.0f),
                            16,
                            100 + 16,
                            64,
                            TRUE);

            Realslider_Vert(710 + (22 * 2), (Cur_Height - 120),
                            (int) ((2.0f - EqDat[ptk->Track_Under_Caret].hg) * 50.0f),
                            16,
                            100 + 16,
                            64,
                            TRUE);
        }
    }//User gui screen match
}

void Mouse_Sliders_Track_Fx_Ed(ptk_data *ptk)
{
    if(ptk->userscreen == USER_SCREEN_TRACK_FX_EDIT)
    {
        if(zcheckMouse(ptk, 74, (Cur_Height - 110), 148, 16) && LFO_ON[ptk->Track_Under_Caret])
        {
            LFO_RATE[ptk->Track_Under_Caret] = (Mouse.x - 84) / 16384.0f;
            ptk->teac = 2;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
        }
        if(zcheckMouse(ptk, 74, (Cur_Height - 92), 148, 16) && LFO_ON[ptk->Track_Under_Caret])
        {
            LFO_AMPL[ptk->Track_Under_Caret] = float(Mouse.x - 84);
            ptk->teac = 3;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
        }
        if(zcheckMouse(ptk, 74, (Cur_Height - 49), 148, 16) && FLANGER_ON[ptk->Track_Under_Caret])
        {
            FLANGER_DEPHASE[ptk->Track_Under_Caret] = (Mouse.x - 84) * 0.0490873f;
            ptk->teac = 4;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            ptk->teac = 4;
        }

        if(zcheckMouse(ptk, 308, (Cur_Height - 121), 148, 16) && FLANGER_ON[ptk->Track_Under_Caret])
        {
            ptk->teac = 1;
            FLANGER_AMOUNT[ptk->Track_Under_Caret] = ((Mouse.x - 318.0f) / 64.0f) - 1.0f;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
        }
        if(zcheckMouse(ptk, 308, (Cur_Height - 103), 148, 16) && FLANGER_ON[ptk->Track_Under_Caret])
        {
            FLANGER_RATE[ptk->Track_Under_Caret] = (Mouse.x - 318.0f) / 939104.92f;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            ptk->teac = 5;
        }
        if(zcheckMouse(ptk, 308, (Cur_Height - 85), 148, 16) && FLANGER_ON[ptk->Track_Under_Caret])
        {
            FLANGER_AMPL[ptk->Track_Under_Caret] = (Mouse.x - 318.0f) / 12800.0f;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            ptk->teac = 6;
        }
        if(zcheckMouse(ptk, 308, (Cur_Height - 67), 148, 16) && FLANGER_ON[ptk->Track_Under_Caret])
        {
            FLANGER_FEEDBACK[ptk->Track_Under_Caret] = ((Mouse.x - 318) / 64.0f) - 1.0f;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            ptk->teac = 7;
        }
        if(zcheckMouse(ptk, 308, (Cur_Height - 49), 148, 16) && FLANGER_ON[ptk->Track_Under_Caret])
        {
            FLANGER_DELAY[ptk->Track_Under_Caret] = (Mouse.x - 318) * 32;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            fld_chan = TRUE;
            ptk->teac = 8;
        }

        // Compressor threshold
        if(zcheckMouse(ptk, 602, (Cur_Height - 103), 67, 18) &&
           Compress_Track[ptk->Track_Under_Caret])
        {
            Mas_Compressor_Set_Variables_Track(ptk, ptk->Track_Under_Caret,
                                               (Mouse.x - 612.0f) * 2.0f,
                                                mas_comp_ratio_Track[ptk->Track_Under_Caret]);
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            ptk->teac = 12;
        }

        // Compressor ratio
        if(zcheckMouse(ptk, 602, (Cur_Height - 85), 67, 18) &&
           Compress_Track[ptk->Track_Under_Caret])
        {
            Mas_Compressor_Set_Variables_Track(ptk, ptk->Track_Under_Caret,
                                               mas_comp_threshold_Track[ptk->Track_Under_Caret],
                                               (Mouse.x - 612.0f) * 2.0f);
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            ptk->teac = 12;
        }

        // Volume
        if(zcheckMouse(ptk, 602, (Cur_Height - 53), 67, 18))
        {
            Track_Volume[ptk->Track_Under_Caret] = (Mouse.x - 612.0f) / 50.0f;
            if(Track_Volume[ptk->Track_Under_Caret] > 1.0f) Track_Volume[ptk->Track_Under_Caret] = 1.0f;
            if(Track_Volume[ptk->Track_Under_Caret] < 0.0f) Track_Volume[ptk->Track_Under_Caret] = 0.0f;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            ptk->teac = 13;
        }

        // Lo Eq
        if(zcheckMouse(ptk, 710, (Cur_Height - 120), 16, 64))
        {
            float Pos_Mouse;
            int Center = Slider_Get_Center(16, 100, 65);
            Pos_Mouse = ((float) ((Mouse.y - (Cur_Height - 120)) - (Center / 2)));
            Pos_Mouse /= 54.0f;
            if(Pos_Mouse > 1.0f) Pos_Mouse = 1.0f;
            Pos_Mouse *= 2.0f;
            EqDat[ptk->Track_Under_Caret].lg = 2.0f - Pos_Mouse;
            if(EqDat[ptk->Track_Under_Caret].lg < 0.0f) EqDat[ptk->Track_Under_Caret].lg = 0.0f;
            if(EqDat[ptk->Track_Under_Caret].lg > 2.0f) EqDat[ptk->Track_Under_Caret].lg = 2.0f;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            ptk->teac = 14;
        }

        // Med Eq
        if(zcheckMouse(ptk, 710 + (22 * 1), (Cur_Height - 120), 16, 64))
        {
            float Pos_Mouse;
            int Center = Slider_Get_Center(16, 100, 65);
            Pos_Mouse = ((float) ((Mouse.y - (Cur_Height - 120)) - (Center / 2)));
            Pos_Mouse /= 54.0f;
            if(Pos_Mouse > 1.0f) Pos_Mouse = 1.0f;
            Pos_Mouse *= 2.0f;
            EqDat[ptk->Track_Under_Caret].mg = 2.0f - Pos_Mouse;
            if(EqDat[ptk->Track_Under_Caret].mg < 0.0f) EqDat[ptk->Track_Under_Caret].mg = 0.0f;
            if(EqDat[ptk->Track_Under_Caret].mg > 2.0f) EqDat[ptk->Track_Under_Caret].mg = 2.0f;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            ptk->teac = 14;
        }

        // Hi Eq
        if(zcheckMouse(ptk, 710 + (22 * 2), (Cur_Height - 120), 16, 64))
        {
            float Pos_Mouse;
            int Center = Slider_Get_Center(16, 100, 65);
            Pos_Mouse = ((float) ((Mouse.y - (Cur_Height - 120)) - (Center / 2)));
            Pos_Mouse /= 54.0f;
            if(Pos_Mouse > 1.0f) Pos_Mouse = 1.0f;
            Pos_Mouse *= 2.0f;
            EqDat[ptk->Track_Under_Caret].hg = 2.0f - Pos_Mouse;
            if(EqDat[ptk->Track_Under_Caret].hg < 0.0f) EqDat[ptk->Track_Under_Caret].hg = 0.0f;
            if(EqDat[ptk->Track_Under_Caret].hg > 2.0f) EqDat[ptk->Track_Under_Caret].hg = 2.0f;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            ptk->teac = 14;
        }

        // Clear lo band
        if(zcheckMouse(ptk, 710, (Cur_Height - 55), 16, 16))
        {
            EqDat[ptk->Track_Under_Caret].lg = 1.0f;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            ptk->teac = 14;
        }

        // Clear med band
        if(zcheckMouse(ptk, 710 + (22 * 1), (Cur_Height - 55), 16, 16))
        {
            EqDat[ptk->Track_Under_Caret].mg = 1.0f;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            ptk->teac = 14;
        }

        // Clear hi band
        if(zcheckMouse(ptk, 710 + (22 * 2), (Cur_Height - 55), 16, 16))
        {
            EqDat[ptk->Track_Under_Caret].hg = 1.0f;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            ptk->teac = 14;
        }

    }
}

void Mouse_Left_Track_Fx_Ed(ptk_data *ptk)
{
    if(ptk->userscreen == USER_SCREEN_TRACK_FX_EDIT)
    {
        if(zcheckMouse(ptk, 74, (Cur_Height - 128), 20, 16) && LFO_ON[ptk->Track_Under_Caret] == FALSE)
        {
            LFO_ON[ptk->Track_Under_Caret] = TRUE;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            ptk->teac = 0;
        }
        if(zcheckMouse(ptk, 96, (Cur_Height - 128), 20, 16) && LFO_ON[ptk->Track_Under_Caret] == TRUE)
        {
            LFO_ON[ptk->Track_Under_Caret] = FALSE;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            ptk->teac = 0;
        }
        if(zcheckMouse(ptk, 184, (Cur_Height - 128), 20, 16) && FLANGER_ON[ptk->Track_Under_Caret] == FALSE)
        {
            FLANGER_ON[ptk->Track_Under_Caret] = TRUE;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            ptk->teac = 0;
        }
        if(zcheckMouse(ptk, 206, (Cur_Height - 128), 20, 16) && FLANGER_ON[ptk->Track_Under_Caret])
        {
            FLANGER_ON[ptk->Track_Under_Caret] = FALSE;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            ptk->teac = 0;
        }

        if(zcheckMouse(ptk, 602, (Cur_Height - 121), 20, 16) && Compress_Track[ptk->Track_Under_Caret] == FALSE)
        {
            Compress_Track[ptk->Track_Under_Caret] = TRUE;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            ptk->teac = 0;
        }
        if(zcheckMouse(ptk, 624, (Cur_Height - 121), 20, 16) && Compress_Track[ptk->Track_Under_Caret])
        {
            Compress_Track[ptk->Track_Under_Caret] = FALSE;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_FX_ED;
            ptk->teac = 0;
        }

    }
}

// ------------------------------------------------------
// Display compressor sliders
void Display_Track_Compressor(ptk_data *ptk)
{
    char string[64];

    Gui_Draw_Button_Box(544, (Cur_Height - 103), 56, 16, "Threshold", BUTTON_NORMAL | BUTTON_DISABLED);
    Realslider_Size(601, (Cur_Height - 103), 50, (int) (mas_comp_threshold_Track[ptk->Track_Under_Caret] * 0.5f), Compress_Track[ptk->Track_Under_Caret] ? TRUE : FALSE);
    sprintf(string, "%d%%", (int) (mas_comp_threshold_Track[ptk->Track_Under_Caret]));
    Print_String(string, 601, (Cur_Height - 101), 67, BUTTON_TEXT_CENTERED);

    Gui_Draw_Button_Box(544, (Cur_Height - 85), 56, 16, "Ratio", BUTTON_NORMAL | BUTTON_DISABLED);
    Realslider_Size(601, (Cur_Height - 85), 50, (int) (mas_comp_ratio_Track[ptk->Track_Under_Caret] * 0.5f), Compress_Track[ptk->Track_Under_Caret] ? TRUE : FALSE);
    sprintf(string, "%d%%", (int) (mas_comp_ratio_Track[ptk->Track_Under_Caret]));
    Print_String(string, 601, (Cur_Height - 83), 67, BUTTON_TEXT_CENTERED);
}

// ------------------------------------------------------
// Display volume slider
void Display_Track_Volume(ptk_data *ptk)
{
    char string[64];
    Gui_Draw_Button_Box(544, (Cur_Height - 51), 56, 16, "Volume", BUTTON_NORMAL | BUTTON_DISABLED);
    Realslider_Size(601, (Cur_Height - 51), 50, (int) (Track_Volume[ptk->Track_Under_Caret] * 50.0f), TRUE);
    sprintf(string, "%d%%", (int) (Track_Volume[ptk->Track_Under_Caret] * 100.0f));
    Print_String(string, 601, (Cur_Height - 49), 67, BUTTON_TEXT_CENTERED);
}
