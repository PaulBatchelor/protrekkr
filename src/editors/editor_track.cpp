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
#include "editor_track.h"
#include "editor_pattern.h"

// ------------------------------------------------------
// Functions
void Draw_Track_Ed(ptk_data *ptk)
{
    Draw_Editors_Bar(ptk, USER_SCREEN_TRACK_EDIT);

    Gui_Draw_Button_Box(0, (Cur_Height - 153), ptk->fsize, 130, "", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Flat_Box("Track: Properties & Fx Send");

    Gui_Draw_Button_Box(508, (Cur_Height - 132), 60, 16, "Track", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(570, (Cur_Height - 132), 60, 16, "Delay Send", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(508, (Cur_Height - 70), 60, 16, "Solo Track", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(508, (Cur_Height - 52), 60, 16, "Un-mute All", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

    Gui_Draw_Button_Box(8, (Cur_Height - 132), 224, 96, "Analog Filter Emulation", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_VTOP);
    Gui_Draw_Button_Box(18, (Cur_Height - 116), 56, 16, "CutOff Frq.", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(18, (Cur_Height - 98), 56, 16, "Reso.", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(18, (Cur_Height - 80), 56, 16, "Type", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(18, (Cur_Height - 62), 56, 16, "Inertia", BUTTON_NORMAL | BUTTON_DISABLED);

    Gui_Draw_Button_Box(240, (Cur_Height - 132), 260, 96, "Distorsion / Reverb / Pan", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_VTOP);
    Gui_Draw_Button_Box(248, (Cur_Height - 116), 56, 16, "Threshold", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(248, (Cur_Height - 98), 56, 16, "Clamp", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(248, (Cur_Height - 80), 56, 16, "Reverb", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(248, (Cur_Height - 62), 56, 16, "Pan", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(456, (Cur_Height - 62), 40, 16, "Center", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(570, (Cur_Height - 88), 60, 16, "Midi Chnl.", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);

    Gui_Draw_Button_Box(640, (Cur_Height - 132), 130, 96, "Config.", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_VTOP);
    Gui_Draw_Button_Box(710, (Cur_Height - 110), 60, 16, "Polyphony", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_NO_BORDER);
    Gui_Draw_Button_Box(710, (Cur_Height - 85), 60, 16, "Notes", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_NO_BORDER);
    Gui_Draw_Button_Box(710, (Cur_Height - 60), 60, 16, "Effects", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_NO_BORDER);
}
   
void Actualize_Track_Ed(ptk_data *ptk, char gode)
{
    if(ptk->userscreen == USER_SCREEN_TRACK_EDIT)
    {
        if(gode == 0 || gode == 15)
        {
            if(ptk->Track_Under_Caret > MAX_TRACKS - 1)
            {
                ptk->Track_Under_Caret = 0;
            }
            if(ptk->Track_Under_Caret < 0)
            {
                ptk->Track_Under_Caret = MAX_TRACKS - 1;
            }
            value_box(508, (Cur_Height - 114), ptk->Track_Under_Caret, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

            Gui_Draw_Button_Box(456, (Cur_Height - 116), 40, 16, "Flat2C", BUTTON_TEXT_CENTERED | BUTTON_NORMAL | (Disclap[ptk->Track_Under_Caret] ? 0 : BUTTON_DISABLED));
            Gui_Draw_Button_Box(456, (Cur_Height - 98), 40, 16, "Flat2T", BUTTON_TEXT_CENTERED | BUTTON_NORMAL | (Disclap[ptk->Track_Under_Caret] ? 0 : BUTTON_DISABLED));
        }

        if(gode == 0 || gode == 1 || gode == 15)
        {
            if(TCut[ptk->Track_Under_Caret] < 0)
            {
                TCut[ptk->Track_Under_Caret] = 0;
            }
            if(TCut[ptk->Track_Under_Caret] > 127)
            {
                TCut[ptk->Track_Under_Caret] = 127;
            }
            Realslider(77, (Cur_Height - 116), (int) TCut[ptk->Track_Under_Caret], FType[ptk->Track_Under_Caret] != 4);
        }

        if(gode == 0 || gode == 2 || gode == 15)
        {
            if(FRez[ptk->Track_Under_Caret] < 0)
            {
                FRez[ptk->Track_Under_Caret] = 0;
            }
            if(FRez[ptk->Track_Under_Caret] > 127)
            {
                FRez[ptk->Track_Under_Caret] = 127;
            }
            Realslider(77, (Cur_Height - 98), FRez[ptk->Track_Under_Caret], FType[ptk->Track_Under_Caret] != 4);
        }

        if(gode == 0 || gode == 3 || gode == 15)
        {
            if(ICut[ptk->Track_Under_Caret] > 0.0078125f)
            {
                ICut[ptk->Track_Under_Caret] = 0.0078125f;
            }
            if(ICut[ptk->Track_Under_Caret] < 0.00006103515625f)
            {
                ICut[ptk->Track_Under_Caret] = 0.00006103515625f;
            }
            Realslider(77, (Cur_Height - 62), (int) (ICut[ptk->Track_Under_Caret] * 16384.0f), FType[ptk->Track_Under_Caret] != 4);
        }

        if(gode == 0 || gode == 4 || gode == 15)
        {
            if(FType[ptk->Track_Under_Caret] < 0)
            {
                FType[ptk->Track_Under_Caret] = 0;
            }
            if(FType[ptk->Track_Under_Caret] >= MAX_FILTER)
            {
                FType[ptk->Track_Under_Caret] = MAX_FILTER;
            }
            switch(FType[ptk->Track_Under_Caret])
            {
                case 0: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "LoPass -12db", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 1: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "HiPass", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 2: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "BandPass", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 3: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "BandReject", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 4: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "Filter Off", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 5: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "LoPass -24db", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 6: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "LoPass -48db", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 7: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "LP-24 [Stereo]", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 8: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "A. Mod. [Mono]", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 9: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "A. Mod. [Stereo]", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 10: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "Single [Mono]", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 11: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "Single [Stereo]", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 12: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "ParaEq -15db", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 13: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "ParaEq -6db", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 14: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "ParaEq +6db", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 15: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "ParaEq +15db", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 16: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "Custom Delta", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 17: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "Comp. Distort L", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 18: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "Comp. Distort M", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 19: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "Comp. Distort H", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 20: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "Comp. Distort", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 21: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "W-HP12 [Mono]", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 22: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "W-HP12 [Stereo]", BUTTON_NORMAL | BUTTON_DISABLED); break;
                case 23: Gui_Draw_Button_Box(141, (Cur_Height - 80), 82, 16, "W-HP24 [Mono]", BUTTON_NORMAL | BUTTON_DISABLED); break;
            }
            value_box(79, (Cur_Height - 80), FType[ptk->Track_Under_Caret], BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
        }

        if(gode == 0 || gode == 5 || gode == 15)
        {
            if(DSend[ptk->Track_Under_Caret] < 0)
            {
                DSend[ptk->Track_Under_Caret] = 0;
            }
            if(DSend[ptk->Track_Under_Caret] > 1.0f)
            {
                DSend[ptk->Track_Under_Caret] = 1.0f;
            }
            Realslider(308, (Cur_Height - 80), int(DSend[ptk->Track_Under_Caret] * 128.0f), compressor != 0);
        }

        if(gode == 0 || gode == 6)
        {
            if(CSend[ptk->Track_Under_Caret] < 0)
            {
                CSend[ptk->Track_Under_Caret] = 0;
            }
            if(CSend[ptk->Track_Under_Caret] > 127)
            {
                CSend[ptk->Track_Under_Caret] = 127;
            }
            value_box(570, (Cur_Height - 114), CSend[ptk->Track_Under_Caret], BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
            if(gode == 6)
            {
                CCoef[ptk->Track_Under_Caret] = float((float) CSend[ptk->Track_Under_Caret] / 127.0f);
            }
        }

        if(gode == 0 || gode == 7 || gode == 15)
        {
            if(DThreshold[ptk->Track_Under_Caret] < 0)
            {
                DThreshold[ptk->Track_Under_Caret] = 0;
            }
            if(DThreshold[ptk->Track_Under_Caret] > 32767)
            {
                DThreshold[ptk->Track_Under_Caret] = 32767;
            }
            Realslider(308, (Cur_Height - 116), (int) DThreshold[ptk->Track_Under_Caret] / 256, Disclap[ptk->Track_Under_Caret]);
        }

        if(gode == 0 || gode == 8 || gode == 15)
        {
            if(DClamp[ptk->Track_Under_Caret] < 0)
            {
                DClamp[ptk->Track_Under_Caret] = 0;
            }
            if(DClamp[ptk->Track_Under_Caret] > 32767)
            {
                DClamp[ptk->Track_Under_Caret] = 32767;
            }
            Realslider(308, (Cur_Height - 98), (int) DClamp[ptk->Track_Under_Caret] / 256, Disclap[ptk->Track_Under_Caret]);
        }

        if(gode == 0 || gode == 9 || gode == 15)
        {
            if(TPan[ptk->Track_Under_Caret] < 0.0f)
            {
                TPan[ptk->Track_Under_Caret] = 0.0f;
            }
            if(TPan[ptk->Track_Under_Caret] > 1.0f)
            {
                TPan[ptk->Track_Under_Caret] = 1.0f;
            }
            ComputeStereo(ptk, ptk->Track_Under_Caret);
            Realslider(308, (Cur_Height - 62), (int) (TPan[ptk->Track_Under_Caret] * 127.0f), TRUE);
        }

        if(gode == 0 || gode == 10 || gode == 15)
        {
            if(CHAN_MUTE_STATE[ptk->Track_Under_Caret])
            {
                Gui_Draw_Button_Box(508, (Cur_Height - 88), 60, 16, "Un-Mute", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
            }
            else
            {
                Gui_Draw_Button_Box(508, (Cur_Height - 88), 60, 16, "Mute Track", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }
        }

        if(gode == 0 || gode == 11 || gode == 15)
        {
            if(ptk->CHAN_MIDI_PRG[ptk->Track_Under_Caret] > 15)
            {
                ptk->CHAN_MIDI_PRG[ptk->Track_Under_Caret] = 0;
            }
            if(ptk->CHAN_MIDI_PRG[ptk->Track_Under_Caret] < 0)
            {
                ptk->CHAN_MIDI_PRG[ptk->Track_Under_Caret] = 15;
            }
            Gui_Draw_Arrows_Number_Box2(570, (Cur_Height - 70), ptk->CHAN_MIDI_PRG[ptk->Track_Under_Caret] + 1, BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
        }

        if(gode == 0 || gode == 12 || gode == 15)
        {
            if(Disclap[ptk->Track_Under_Caret])
            {
                Gui_Draw_Button_Box(570, (Cur_Height - 52), 60, 16, "Distort On", BUTTON_PUSHED | BUTTON_TEXT_CENTERED);
            }
            else
            {
                Gui_Draw_Button_Box(570, (Cur_Height - 52), 60, 16, "Distort Off", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            }
        }

        if(gode == 0 || gode == 13 || gode == 14 || gode == 15)
        {
            if(Channels_Polyphony[ptk->Track_Under_Caret] < 1)
            {
                Channels_Polyphony[ptk->Track_Under_Caret] = 1;
            }
            if(Channels_Polyphony[ptk->Track_Under_Caret] > MAX_POLYPHONY)
            {
                Channels_Polyphony[ptk->Track_Under_Caret] = MAX_POLYPHONY;
            }
            Gui_Draw_Arrows_Number_Box2(647, (Cur_Height - 110), Channels_Polyphony[ptk->Track_Under_Caret], BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
        }
        if(gode == 0 || gode == 14 || gode == 13 || gode == 15)
        {
            if(Channels_MultiNotes[ptk->Track_Under_Caret] < 1)
            {
                Channels_MultiNotes[ptk->Track_Under_Caret] = 1;
            }
            if(Channels_MultiNotes[ptk->Track_Under_Caret] > Channels_Polyphony[ptk->Track_Under_Caret])
            {
                Channels_MultiNotes[ptk->Track_Under_Caret] = Channels_Polyphony[ptk->Track_Under_Caret];
            }
            if(Channels_MultiNotes[ptk->Track_Under_Caret] > MAX_POLYPHONY)
            {
                Channels_MultiNotes[ptk->Track_Under_Caret] = MAX_POLYPHONY;
            }
            Gui_Draw_Arrows_Number_Box2(647, (Cur_Height - 85), Channels_MultiNotes[ptk->Track_Under_Caret], BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
            if(gode)
            {
                Actupated(ptk, 0);
            }
        }

        if(gode == 0 || gode == 16)
        {
            if(Channels_Effects[ptk->Track_Under_Caret] < 1)
            {
                Channels_Effects[ptk->Track_Under_Caret] = 1;
            }
            if(Channels_Effects[ptk->Track_Under_Caret] > MAX_FX)
            {
                Channels_Effects[ptk->Track_Under_Caret] = MAX_FX;
            }
            Gui_Draw_Arrows_Number_Box2(647, (Cur_Height - 60), Channels_Effects[ptk->Track_Under_Caret], BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
            if(gode)
            {
                Actupated(ptk, 0);
            }
        }
        
        if(ptk->trkchan == TRUE)
        {
            Actupated(ptk, 0);
            ptk->trkchan = FALSE;
        }
    } // Userscreen match found
}

void Mouse_Left_Track_Ed(ptk_data *ptk)
{
    if(ptk->userscreen == USER_SCREEN_TRACK_EDIT)
    {
        // Filter type
        if(zcheckMouse(ptk, 79, (Cur_Height - 80), 16, 16) && FType[ptk->Track_Under_Caret] > 0)
        {
            ResetFilters(ptk, ptk->Track_Under_Caret);
            FType[ptk->Track_Under_Caret]--;
            ptk->teac = 0;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
        }
        if(zcheckMouse(ptk, 123, (Cur_Height - 80), 16, 16) && FType[ptk->Track_Under_Caret] < MAX_FILTER)
        {
            ResetFilters(ptk, ptk->Track_Under_Caret);
            FType[ptk->Track_Under_Caret]++;
            ptk->teac = 0;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
        }

        // Previous track
        if(zcheckMouse(ptk, 508, (Cur_Height - 114), 16, 16))
        {
            ptk->Track_Under_Caret--;
            ptk->teac = 0;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
            ptk->trkchan = TRUE;
        }

        // Next track
        if(zcheckMouse(ptk, 552, (Cur_Height - 114), 16, 16))
        {
            ptk->Track_Under_Caret++;
            ptk->teac = 0;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
            ptk->trkchan = TRUE;
        }

        // Delay send
        if(zcheckMouse(ptk, 570, (Cur_Height - 114), 16, 16))
        {
            CSend[ptk->Track_Under_Caret]--;
            ptk->teac = 6;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
        }
        if(zcheckMouse(ptk, 614, (Cur_Height - 114), 16, 16))
        {
            CSend[ptk->Track_Under_Caret]++;
            ptk->teac = 6;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
        }

        if(zcheckMouse(ptk, 456, (Cur_Height - 116), 40, 16) &&
           Disclap[ptk->Track_Under_Caret])
        {
            DThreshold[ptk->Track_Under_Caret] = DClamp[ptk->Track_Under_Caret];
            ptk->teac = 7;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
        }
        if(zcheckMouse(ptk, 456, (Cur_Height - 98), 40, 16) &&
           Disclap[ptk->Track_Under_Caret])
        {
            DClamp[ptk->Track_Under_Caret] = DThreshold[ptk->Track_Under_Caret];
            ptk->teac = 8;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
        }

        // Channel panning
        if(zcheckMouse(ptk, 456, (Cur_Height - 62), 40, 16))
        {
            TPan[ptk->Track_Under_Caret] = 0.5f;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
            ptk->teac = 9;
        }

        // Midi channel
        if(zcheckMouse(ptk, 570, (Cur_Height - 70), 16, 16))
        {
            ptk->CHAN_MIDI_PRG[ptk->Track_Under_Caret]--;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
            ptk->teac = 11;
        }
        if(zcheckMouse(ptk, 614, (Cur_Height - 70), 16, 16))
        {
            ptk->CHAN_MIDI_PRG[ptk->Track_Under_Caret]++;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
            ptk->teac = 11;
        }

        // Distortion
        if(zcheckMouse(ptk, 570, (Cur_Height - 52), 60, 16))
        {
            Disclap[ptk->Track_Under_Caret] = !Disclap[ptk->Track_Under_Caret];
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
            ptk->teac = 0;
        }

        // Mute track
        if(zcheckMouse(ptk, 508, (Cur_Height - 88), 64, 16))
        {
            if(CHAN_MUTE_STATE[ptk->Track_Under_Caret] == 0)
            {
                CHAN_MUTE_STATE[ptk->Track_Under_Caret] = 1;
            }
            else
            {
                CHAN_MUTE_STATE[ptk->Track_Under_Caret] = 0;
            }
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
            ptk->teac = 10;
            ptk->trkchan = TRUE;
        }

        // Solo track
        if(zcheckMouse(ptk, 508, (Cur_Height - 70), 64, 16))
        {
            for(int solify = 0; solify < MAX_TRACKS; solify++)
            {
                CHAN_MUTE_STATE[solify] = 1;
            }
            CHAN_MUTE_STATE[ptk->Track_Under_Caret] = 0;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
            ptk->trkchan = TRUE;
            ptk->teac = 10;
        }

        // Unmute all tracks
        if(zcheckMouse(ptk, 508, (Cur_Height - 52), 88, 16))
        {
            for(int solify = 0; solify < MAX_TRACKS; solify++)
            {
                CHAN_MUTE_STATE[solify] = 0;
            }
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
            ptk->trkchan = TRUE;
            ptk->teac = 10;
        }

        // Channels polyphony
        if(zcheckMouse(ptk, 647, (Cur_Height - 110), 16, 16) == 1)
        {
            Channels_Polyphony[ptk->Track_Under_Caret]--;
            if(Channels_Polyphony[ptk->Track_Under_Caret] < 1)
            {
                Channels_Polyphony[ptk->Track_Under_Caret] = 1;
            }
            if(Channels_MultiNotes[ptk->Track_Under_Caret] > Channels_Polyphony[ptk->Track_Under_Caret])
            {
                Channels_MultiNotes[ptk->Track_Under_Caret] = Channels_Polyphony[ptk->Track_Under_Caret];
            }
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
            ptk->teac = 13;
        }
        if(zcheckMouse(ptk, 647 + 44, (Cur_Height - 110), 16, 16) == 1)
        {
            Channels_Polyphony[ptk->Track_Under_Caret]++;
            if(Channels_Polyphony[ptk->Track_Under_Caret] > MAX_POLYPHONY)
            {
                Channels_Polyphony[ptk->Track_Under_Caret] = MAX_POLYPHONY;
            }
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
            ptk->teac = 13;
        }

        // Multi notes
        if(zcheckMouse(ptk, 647, (Cur_Height - 85), 16, 16) == 1)
        {
            Track_Sub_Notes(ptk, ptk->Track_Under_Caret, 1);
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
            ptk->teac = 14;
        }
        if(zcheckMouse(ptk, 647 + 44, (Cur_Height - 85), 16, 16) == 1)
        {
            Track_Add_Notes(ptk, ptk->Track_Under_Caret, 1);
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
            ptk->teac = 14;
        }

        // Number of effects
        if(zcheckMouse(ptk, 647, (Cur_Height - 60), 16, 16) == 1)
        {
            Track_Sub_Effects(ptk, ptk->Track_Under_Caret, 1);
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
            ptk->teac = 16;
        }
        if(zcheckMouse(ptk, 647 + 44, (Cur_Height - 60), 16, 16) == 1)
        {
            Track_Add_Effects(ptk, ptk->Track_Under_Caret, 1);
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
            ptk->teac = 16;
        }

    } // Userscreen 1
}

void Mouse_Right_Track_Ed(ptk_data *ptk)
{
    if(ptk->userscreen == USER_SCREEN_TRACK_EDIT)
    {
        if(zcheckMouse(ptk, 570, (Cur_Height - 114), 16, 16) == 1)
        {
            CSend[ptk->Track_Under_Caret] -= 16;
            ptk->teac = 6;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
        }
        if(zcheckMouse(ptk, 614, (Cur_Height - 114), 16, 16) == 1)
        {
            CSend[ptk->Track_Under_Caret] += 16;
            ptk->teac = 6;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
        }

        // Filter type
        if(zcheckMouse(ptk, 79, (Cur_Height - 80), 16, 16) && FType[ptk->Track_Under_Caret] > 0)
        {
            ResetFilters(ptk, ptk->Track_Under_Caret);
            FType[ptk->Track_Under_Caret] -= 16;
            ptk->teac = 0;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
        }
        if(zcheckMouse(ptk, 123, (Cur_Height - 80), 16, 16) && FType[ptk->Track_Under_Caret] < MAX_FILTER)
        {
            ResetFilters(ptk, ptk->Track_Under_Caret);
            FType[ptk->Track_Under_Caret] += 16;
            ptk->teac = 0;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
        }

        // Channels polyphony
        if(zcheckMouse(ptk, 647, (Cur_Height - 110), 16, 16) == 1)
        {
            Channels_Polyphony[ptk->Track_Under_Caret] -= 10;
            if(Channels_Polyphony[ptk->Track_Under_Caret] < 1) Channels_Polyphony[ptk->Track_Under_Caret] = 1;
            if(Channels_MultiNotes[ptk->Track_Under_Caret] > Channels_Polyphony[ptk->Track_Under_Caret]) Channels_MultiNotes[ptk->Track_Under_Caret] = Channels_Polyphony[ptk->Track_Under_Caret];
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
            ptk->teac = 13;
        }
        if(zcheckMouse(ptk, 647 + 44, (Cur_Height - 110), 16, 16) == 1)
        {
            Channels_Polyphony[ptk->Track_Under_Caret] += 10;
            if(Channels_Polyphony[ptk->Track_Under_Caret] > MAX_POLYPHONY) Channels_Polyphony[ptk->Track_Under_Caret] = MAX_POLYPHONY;
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
            ptk->teac = 13;
        }

        // Multi notes
        if(zcheckMouse(ptk, 647, (Cur_Height - 85), 16, 16) == 1)
        {
            Track_Sub_Notes(ptk, ptk->Track_Under_Caret, 10);
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
            ptk->teac = 14;
        }
        if(zcheckMouse(ptk, 647 + 44, (Cur_Height - 85), 16, 16) == 1)
        {
            Track_Add_Notes(ptk, ptk->Track_Under_Caret, 10);
            ptk->gui_action = GUI_CMD_UPDATE_TRACK_ED;
            ptk->teac = 14;
        }
    }
}

void Mouse_Sliders_Track_Ed(ptk_data *ptk)
{
    if(ptk->userscreen == USER_SCREEN_TRACK_EDIT)
    {
        if(zcheckMouse(ptk, 77, (Cur_Height - 116), 148, 16) && FType[ptk->Track_Under_Caret] != 4) ptk->gui_action = GUI_CMD_SET_TRACK_CUTOFF_FREQ;
        if(zcheckMouse(ptk, 77, (Cur_Height - 98), 148, 16) && FType[ptk->Track_Under_Caret] != 4) ptk->gui_action = GUI_CMD_SET_TRACK_RESONANCE;
        if(zcheckMouse(ptk, 77, (Cur_Height - 62), 148, 16) && FType[ptk->Track_Under_Caret] != 4) ptk->gui_action = GUI_CMD_SET_TRACK_INERTIA;
        if(zcheckMouse(ptk, 308, (Cur_Height - 116), 148, 16) && Disclap[ptk->Track_Under_Caret]) ptk->gui_action = GUI_CMD_SET_TRACK_THRESHOLD;
        if(zcheckMouse(ptk, 308, (Cur_Height - 98), 148, 16) && Disclap[ptk->Track_Under_Caret]) ptk->gui_action = GUI_CMD_SET_TRACK_CLAMP;
        if(compressor != 0) if(zcheckMouse(ptk, 308, (Cur_Height - 80), 148, 16)) ptk->gui_action = GUI_CMD_SET_TRACK_REVERB_SEND;
        if(zcheckMouse(ptk, 308, (Cur_Height - 62), 148, 16)) ptk->gui_action = GUI_CMD_SET_TRACK_PANNING;
    }
}

// ------------------------------------------------------
// Add a given amount of notes to a track
void Track_Add_Notes(ptk_data *ptk, int Track_Nbr, int Amount)
{
    Channels_MultiNotes[Track_Nbr] += Amount;
    if(Channels_MultiNotes[Track_Nbr] > MAX_POLYPHONY) Channels_MultiNotes[Track_Nbr] = MAX_POLYPHONY;
    if(Channels_MultiNotes[Track_Nbr] > Channels_Polyphony[Track_Nbr])
    {
        Channels_Polyphony[Track_Nbr] = Channels_MultiNotes[Track_Nbr];
    }
    if(Get_Track_Zoom(ptk, Track_Nbr) == TRACK_LARGE && Get_Track_Real_Size(ptk, Track_Nbr) >= ptk->TRACKS_WIDTH)
    {
        Set_Track_Zoom(ptk, Track_Nbr, TRACK_MEDIUM);
    }
    if(Get_Track_Zoom(ptk, Track_Nbr) == TRACK_MEDIUM && Get_Track_Real_Size(ptk, Track_Nbr) >= ptk->TRACKS_WIDTH)
    {
        Set_Track_Zoom(ptk, Track_Nbr, TRACK_SMALL);
    }
}

// ------------------------------------------------------
// Remove a given amount of notes from a track
void Track_Sub_Notes(ptk_data *ptk, int Track_Nbr, int Amount)
{
    Channels_MultiNotes[Track_Nbr] -= Amount;
    if(Channels_MultiNotes[Track_Nbr] < 1)
    {
        Channels_MultiNotes[Track_Nbr] = 1;
    }
    if(Get_Track_Zoom(ptk, Track_Nbr) == TRACK_LARGE && Get_Track_Real_Size(ptk, Track_Nbr) >= ptk->TRACKS_WIDTH)
    {
        Set_Track_Zoom(ptk, Track_Nbr, TRACK_MEDIUM);
    }
    if(Get_Track_Zoom(ptk, Track_Nbr) == TRACK_MEDIUM && Get_Track_Real_Size(ptk, Track_Nbr) >= ptk->TRACKS_WIDTH)
    {
        Set_Track_Zoom(ptk, Track_Nbr, TRACK_SMALL);
    }
}

// ------------------------------------------------------
// Add a given amount of effects to a track
void Track_Add_Effects(ptk_data *ptk, int Track_Nbr, int Amount)
{
    Channels_Effects[Track_Nbr] += Amount;
    if(Channels_Effects[Track_Nbr] > MAX_FX)
    {
        Channels_Effects[Track_Nbr] = MAX_FX;
    }
    if(Get_Track_Zoom(ptk, Track_Nbr) == TRACK_LARGE && Get_Track_Real_Size(ptk, Track_Nbr) >= ptk->TRACKS_WIDTH)
    {
        Set_Track_Zoom(ptk, Track_Nbr, TRACK_MEDIUM);
    }
    if(Get_Track_Zoom(ptk, Track_Nbr) == TRACK_MEDIUM && Get_Track_Real_Size(ptk, Track_Nbr) >= ptk->TRACKS_WIDTH)
    {
        Set_Track_Zoom(ptk, Track_Nbr, TRACK_SMALL);
    }
}

// ------------------------------------------------------
// Remove a given amount of effects from a track
void Track_Sub_Effects(ptk_data *ptk, int Track_Nbr, int Amount)
{
    Channels_Effects[Track_Nbr] -= Amount;
    if(Channels_Effects[Track_Nbr] < 1) Channels_Effects[Track_Nbr] = 1;
    if(Get_Track_Zoom(ptk, Track_Nbr) == TRACK_LARGE && Get_Track_Real_Size(ptk, Track_Nbr) >= ptk->TRACKS_WIDTH)
    {
        Set_Track_Zoom(ptk, Track_Nbr, TRACK_MEDIUM);
    }
    if(Get_Track_Zoom(ptk, Track_Nbr) == TRACK_MEDIUM && Get_Track_Real_Size(ptk, Track_Nbr) >= ptk->TRACKS_WIDTH)
    {
        Set_Track_Zoom(ptk, Track_Nbr, TRACK_SMALL);
    }
}
