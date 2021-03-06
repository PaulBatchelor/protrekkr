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
#include "ptk.h"

#include "include/midi_dispatch.h"

// ------------------------------------------------------
// Variables
/*TODO: remove this ugly global variable */
extern ptk_data *g_ptk;

// ------------------------------------------------------
// Master volume (LIVE)
void Midi_Song_Set_Volume(int Data)
{
    //local_mas_vol = ((float) Data / 127.0f);
    //if(local_mas_vol > 1.0f) local_mas_vol = 1.0f;
    //if(local_mas_vol < 0.0f) local_mas_vol = 0.0f;
    //if(sr_isrecording || is_editing)
    //{
    //    ptk->liveparam = LIVE_PARAM_SONG_VOLUME;
    //    ptk->livevalue = (int) (local_mas_vol * 255.0f);
    //}
}

// ------------------------------------------------------
// BPM (LIVE)
void Midi_Song_Set_BPM(int Data)
{
    //BeatsPerMin = (int) ((((float) Data / 127.0f) * 235.0f) + 20.0f);
    //SamplesPerTick = (int) ((60 * MIX_RATE) / (BeatsPerMin * TicksPerBeat));  
    //SamplesPerSub = SamplesPerTick / 6;
    //if(sr_isrecording || is_editing)
    //{
    //    ptk->liveparam = LIVE_PARAM_SONG_BPM;
    //    ptk->livevalue = BeatsPerMin;
    //}
    //ptk->gui_bpm_action = TRUE;
}

// ------------------------------------------------------
// 303: set tune (LIVE)
void Midi_303_Set_Tune(int Unit, int Data)
{
    //tb303[Unit].tune = Data;
    //if(sr_isrecording || is_editing)
    //{
    //    ptk->liveparam = LIVE_PARAM_303_1_TUNE + Unit;
    //    ptk->livevalue = Data << 1;
    //}
    //if(Unit == sl3)
    //{
    //    ptk->teac = 3;
    //    Refresh_Unit = Unit;
    //    ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS_EXTERNAL;
    //}
}

void Midi_303_Set_1_Tune(int Data) { Midi_303_Set_Tune(0, Data); }
void Midi_303_Set_2_Tune(int Data) { Midi_303_Set_Tune(1, Data); }
void Midi_303_Set_Cur_Tune(int Data) { /*Midi_303_Set_Tune(sl3, Data);*/ }

// ------------------------------------------------------
// 303: set cutoff (LIVE)
void Midi_303_Set_Cutoff(int Unit, int Data)
{
    //tb303[Unit].cutoff = Data;
    //if(sr_isrecording || is_editing)
    //{
    //    ptk->liveparam = LIVE_PARAM_303_1_CUTOFF + Unit;
    //    ptk->livevalue = Data << 1;
    //}
    //if(Unit == sl3)
    //{
    //    ptk->teac = 4;
    //    Refresh_Unit = Unit;
    //    ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS_EXTERNAL;
    //}
}

void Midi_303_Set_1_Cutoff(int Data) { Midi_303_Set_Cutoff(0, Data); }
void Midi_303_Set_2_Cutoff(int Data) { Midi_303_Set_Cutoff(1, Data); }
void Midi_303_Set_Cur_Cutoff(int Data) { /*Midi_303_Set_Cutoff(sl3, Data);*/ }

// ------------------------------------------------------
// 303: set resonance (LIVE)
void Midi_303_Set_Resonance(int Unit, int Data)
{
    //tb303[Unit].resonance = Data;
    //if(sr_isrecording || is_editing)
    //{
    //    ptk->liveparam = LIVE_PARAM_303_1_RESONANCE + Unit;
    //    ptk->livevalue = Data << 1;
    //}
    //if(Unit == sl3)
    //{
    //    ptk->teac = 5;
    //    Refresh_Unit = Unit;
    //    ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS_EXTERNAL;
    //}
}

void Midi_303_Set_1_Resonance(int Data) { Midi_303_Set_Resonance(0, Data); }
void Midi_303_Set_2_Resonance(int Data) { Midi_303_Set_Resonance(1, Data); }
void Midi_303_Set_Cur_Resonance(int Data) { /*Midi_303_Set_Resonance(sl3, Data);*/ }

// ------------------------------------------------------
// 303: set env mod (LIVE)
void Midi_303_Set_Envmod(int Unit, int Data)
{
    //tb303[Unit].envmod = Data;
    //if(sr_isrecording || is_editing)
    //{
    //    ptk->liveparam = LIVE_PARAM_303_1_ENVMOD + Unit;
    //    ptk->livevalue = Data << 1;
    //}
    //if(Unit == sl3)
    //{
    //    ptk->teac = 6;
    //    Refresh_Unit = Unit;
    //    ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS_EXTERNAL;
    //}
}

void Midi_303_Set_1_Envmod(int Data) { Midi_303_Set_Envmod(0, Data); }
void Midi_303_Set_2_Envmod(int Data) { Midi_303_Set_Envmod(1, Data); }
void Midi_303_Set_Cur_Envmod(int Data) { /*Midi_303_Set_Envmod(sl3, Data);*/ }

// ------------------------------------------------------
// 303: set decay (LIVE)
void Midi_303_Set_Decay(int Unit, int Data)
{
    //tb303[Unit].decay = Data;
    //if(sr_isrecording || is_editing)
    //{
    //    ptk->liveparam = LIVE_PARAM_303_1_DECAY + Unit;
    //    ptk->livevalue = Data << 1;
    //}
    //if(Unit == sl3)
    //{
    //    ptk->teac = 7;
    //    Refresh_Unit = Unit;
    //    ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS_EXTERNAL;
    //}
}

void Midi_303_Set_1_Decay(int Data) { Midi_303_Set_Decay(0, Data); }
void Midi_303_Set_2_Decay(int Data) { Midi_303_Set_Decay(1, Data); }
void Midi_303_Set_Cur_Decay(int Data) { /* Midi_303_Set_Decay(sl3, Data); */ }

// ------------------------------------------------------
// 303: set accent (LIVE)
void Midi_303_Set_Accent(int Unit, int Data)
{
    //tb303[Unit].accent = Data;
    //if(sr_isrecording || is_editing)
    //{
    //    ptk->liveparam = LIVE_PARAM_303_1_ACCENT + Unit;
    //    ptk->livevalue = Data << 1;
    //}
    //if(Unit == sl3)
    //{
    //    ptk->teac = 8;
    //    Refresh_Unit = Unit;
    //    ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS_EXTERNAL;
    //}
}

void Midi_303_Set_1_Accent(int Data) { Midi_303_Set_Accent(0, Data); }
void Midi_303_Set_2_Accent(int Data) { Midi_303_Set_Accent(1, Data); }
void Midi_303_Set_Cur_Accent(int Data) { /*Midi_303_Set_Accent(sl3, Data);*/ }

// ------------------------------------------------------
// 303: set volume (LIVE)
void Midi_303_Set_Volume(int Unit, int Data)
{
    //tb303engine[Unit].tbVolume = ((float) Data) / 127.0f;
    //if(sr_isrecording || is_editing)
    //{
    //    ptk->liveparam = LIVE_PARAM_303_1_VOLUME + Unit;
    //    ptk->livevalue = (int) tb303engine[Unit].tbVolume * 255.0f;
    //}
    //if(Unit == sl3)
    //{
    //    ptk->teac = 15;
    //    Refresh_Unit = Unit;
    //    ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS_EXTERNAL;
    //}
}

void Midi_303_Set_1_Volume(int Data) { Midi_303_Set_Volume(0, Data); }
void Midi_303_Set_2_Volume(int Data) { Midi_303_Set_Volume(1, Data); }
void Midi_303_Set_Cur_Volume(int Data) { /* Midi_303_Set_Volume(sl3, Data); */ }

// ------------------------------------------------------
// 303: Select unit
void Midi_303_Switch(int Data)
{
    //sl3 ^= 1;
    //ptk->teac = 0;
    //ptk->gui_action = GUI_CMD_REFRESH_TB303_PARAMS;
}

// ------------------------------------------------------
// Track panning (LIVE)
void Midi_Track_Set_Panning(int Data)
{
    //TPan[ptk->Track_Under_Caret] = ((float) Data) / 127.0f;
    //if(sr_isrecording || is_editing)
    //{
    //    ptk->liveparam = LIVE_PARAM_TRACK_PANNING;
    //    ptk->livevalue = Data;
    //}
    //Actualize_Track_Ed(g_ptk, 9);
}

// ------------------------------------------------------
// Track volume (LIVE)
void Midi_Track_Set_Volume(int Data)
{
    //if(sr_isrecording || is_editing)
    //{
    //    ptk->liveparam = LIVE_PARAM_TRACK_VOLUME;
    //    ptk->livevalue = ((float) Data / 127.0f) * 64.0f;
    //}
}

// ------------------------------------------------------
// Track volume (LIVE)
void Midi_Track_Set_LFO_Carrier(int Data)
{
    //if(sr_isrecording || is_editing)
    //{
    //    ptk->liveparam = LIVE_PARAM_TRACK_LFO_CARRIER;
    //    ptk->livevalue = Data << 1;
    //}
}

// ------------------------------------------------------
// Track cutoff (LIVE)
void Midi_Track_Set_Cutoff(int Data)
{
    //TCut[ptk->Track_Under_Caret] = ((float) Data);
    //if(sr_isrecording || is_editing)
    //{
    //    ptk->liveparam = LIVE_PARAM_TRACK_CUTOFF;
    //    ptk->livevalue = Data;
    //}
    //Actualize_Track_Ed(g_ptk, 1);
}

// ------------------------------------------------------
// Track resonance (LIVE)
void Midi_Track_Set_Resonance(int Data)
{
    //FRez[ptk->Track_Under_Caret] = ((float) Data);
    //if(sr_isrecording || is_editing)
    //{
    //    ptk->liveparam = LIVE_PARAM_TRACK_RESONANCE;
    //    ptk->livevalue = Data;
    //}
    //Actualize_Track_Ed(g_ptk, 2);
}

// ------------------------------------------------------
// Track reverb amount (LIVE)
void Midi_Track_Set_Reverb(int Data)
{
    //DSend[ptk->Track_Under_Caret] = ((float) Data) / 127.0f;
    //if(sr_isrecording || is_editing)
    //{
    //    ptk->liveparam = LIVE_PARAM_TRACK_REVERB_SEND;
    //    ptk->livevalue = Data;
    //}
    //Actualize_Track_Ed(g_ptk, 5);
}

// ------------------------------------------------------
// Track disto threshold (LIVE)
void Midi_Track_Set_Disto_Threshold(int Data)
{
    //DThreshold[ptk->Track_Under_Caret] = (((float) Data) / 127.0f) * 32767.0f;
    //if(sr_isrecording || is_editing)
    //{
    //    ptk->liveparam = LIVE_PARAM_TRACK_THRESHOLD;
    //    ptk->livevalue = Data;
    //}
    //Actualize_Track_Ed(g_ptk, 7);
}

// ------------------------------------------------------
// Track disto clamp (LIVE)
void Midi_Track_Set_Disto_Clamp(int Data)
{
    //DClamp[ptk->Track_Under_Caret] = (((float) Data) / 127.0f) * 32767.0f;
    //if(sr_isrecording || is_editing)
    //{
    //    ptk->liveparam = LIVE_PARAM_TRACK_CLAMP;
    //    ptk->livevalue = Data;
    //}
    //Actualize_Track_Ed(g_ptk, 8);
}

// ------------------------------------------------------
// Start playing (EDIT)
void Midi_Transport_Play(int Data)
{
    //plx = 0;
    //Pattern_Line = 0;
    //ptk->gui_action = GUI_CMD_PLAY_SONG;
}

// ------------------------------------------------------
// Stop playing (EDIT)
void Midi_Transport_Stop(int Data)
{
    //ptk->gui_action = GUI_CMD_STOP_SONG;
}

// ------------------------------------------------------
// Start/Stop recording (EDIT)
void Midi_Transport_Record(int Data)
{
    //ptk->gui_action = GUI_CMD_RECORD_MODE;
}

// ------------------------------------------------------
// Fast forward (EDIT)
void Midi_Transport_Forward(int Data)
{
    //ptk->gui_action = GUI_CMD_NEXT_POSITION;
}

// ------------------------------------------------------
// Rewind (EDIT)
void Midi_Transport_Rewind(int Data)
{
    //ptk->gui_action = GUI_CMD_PREVIOUS_POSITION;
}

// ------------------------------------------------------
// Start/Stop edit mode (EDIT)
void Midi_Transport_Edit(int Data)
{
    //ptk->gui_action = GUI_CMD_EDIT_MODE;
}

// ------------------------------------------------------
// Start/Stop live rec (EDIT)
void Midi_Transport_LiveRec(int Data)
{
    //sr_isrecording ^= 1;
    //ptk->gui_action = GUI_CMD_RECORD_303;
}

// ------------------------------------------------------
// Set pattern row (EDIT)
void Midi_Edit_Set_Row(int Data)
{
    //ptk_data *ptk = g_ptk;
    //if(!is_recording && !Songplaying)
    //{
    //    int Pos = Get_Song_Position(ptk);
    //    int Rows = patternLines[pSequence[Pos]];
    //    float factor = ((float) Rows / 127.0f);
    //    
    //    Pattern_Line = (int) (Data * factor);
    //    if(Pattern_Line < 0) Pattern_Line = 0;
    //    if(Pattern_Line >= Rows) Pattern_Line = Rows - 1;
    //    ptk->gui_action = GUI_CMD_UPDATE_PATTERN;
    //}
}

// ------------------------------------------------------
// Goto previous pattern row (EDIT)
void Midi_Edit_Previous_Row(int Data)
{
    //if(!is_recording && !Songplaying)
    //{
    //    ptk->gui_action = GUI_CMD_GOTO_PREVIOUS_ROW;
    //}
}

// ------------------------------------------------------
// Goto next pattern row (EDIT)
void Midi_Edit_Next_Row(int Data)
{
    //if(!is_recording && !Songplaying)
    //{
    //    ptk->gui_action = GUI_CMD_GOTO_NEXT_ROW;
    //}
}

// ------------------------------------------------------
// Set pattern track (EDIT)
void Midi_Edit_Set_Track(int Data)
{
    //float factor = ((float) Songtracks / 16.0f);
    //ptk->Column_Under_Caret = 0;
    //ptk->Track_Under_Caret = (int) (((float) Data * factor) / 127.0f * 15.0f);
    //ptk->gui_action = GUI_CMD_SET_FOCUS_TRACK_EXTERNAL;
}

// ------------------------------------------------------
// Set current instrument (EDIT)
void Midi_Edit_Set_Instrument(int Data)
{
    //Current_Instrument = Data;
    //ptk->gui_action = GUI_CMD_SELECT_INSTR_EXTERNAL;
}

// ------------------------------------------------------
// Switch track on/off (LIVE)
void Midi_Edit_Track_On_Off(int Data)
{
    ///*TODO: remove this ugly global variable */
    //ptk_data *ptk = g_ptk;
    //Ext_Pos_Switch = Get_Song_Position(ptk);
    //Ext_Track_Switch = ptk->Track_Under_Caret;
    //ptk->gui_action = GUI_CMD_SWITCH_TRACK_STATUS;
}

// ------------------------------------------------------
// Focus on previous track (EDIT/LIVE)
void Midi_Edit_Previous_Track(int Data)
{
    //ptk->Track_Under_Caret--;
    //ptk->gui_action = GUI_CMD_SET_FOCUS_TRACK_EXTERNAL;
}

// ------------------------------------------------------
// Focus on next track (EDIT/LIVE)
void Midi_Edit_Next_Track(int Data)
{
    //ptk->Track_Under_Caret++;
    //ptk->gui_action = GUI_CMD_SET_FOCUS_TRACK_EXTERNAL;
}

// ------------------------------------------------------
// Dispatch table
MIDI_DISPATCH Midi_Dispatch_Table[NBR_MIDI_DISPATCH_MSG] =
{
    //MIDI_AUTOMATION_NONE, 0,
    //MIDI_AUTOMATION_NONE, 0,
    //MIDI_AUTOMATION_NONE, 0,
    //MIDI_AUTOMATION_NONE, 0,
    //MIDI_AUTOMATION_NONE, 0,
    //MIDI_AUTOMATION_NONE, 0,
    //MIDI_AUTOMATION_NONE, 0,
    //MIDI_AUTOMATION_NONE, 0,
    //MIDI_AUTOMATION_NONE, 0,
    //MIDI_AUTOMATION_NONE, 0,
    //MIDI_AUTOMATION_NONE, 0,
    //MIDI_AUTOMATION_NONE, 0,
    //MIDI_AUTOMATION_NONE, 0,
    //MIDI_AUTOMATION_NONE, 0,
    //MIDI_AUTOMATION_NONE, 0
};

MIDI_PTK_CMD Str_Midi_Commands[NBR_MIDI_AUTOMATION] =
{
    //{ OPA_ N_ O_ N_ E_ CPA_, NULL },

    //{ E_ D_ I_ T_ DDT_ SPC_ P_ L_ A_ Y_, &Midi_Transport_Play },
    //{ E_ D_ I_ T_ DDT_ SPC_ S_ T_ O_ P_, &Midi_Transport_Stop },
    //{ E_ D_ I_ T_ DDT_ SPC_ E_ D_ I_ T_ SPC_ O_ N_ SLA_ O_ F_ F_, &Midi_Transport_Edit },
    //{ E_ D_ I_ T_ DDT_ SPC_ R_ E_ C_ O_ R_ D_ SPC_ O_ N_ SLA_ O_ F_ F_, &Midi_Transport_Record },
    //{ E_ D_ I_ T_ DDT_ SPC_ F_ O_ R_ W_ A_ R_ D_, &Midi_Transport_Forward },
    //{ E_ D_ I_ T_ DDT_ SPC_ R_ E_ W_ I_ N_ D_, &Midi_Transport_Rewind },
    //{ E_ D_ I_ T_ DDT_ SPC_ L_ I_ V_ E_ SPC_ R_ E_ C_ SPC_ O_ N_ SLA_ O_ F_ F_, &Midi_Transport_LiveRec },

    //{ E_ D_ I_ T_ DDT_ SPC_ S_ E_ T_ SPC_ R_ O_ W_, &Midi_Edit_Set_Row },
    //{ E_ D_ I_ T_ DDT_ SPC_ S_ E_ T_ SPC_ T_ R_ A_ C_ K_, &Midi_Edit_Set_Track },
    //{ E_ D_ I_ T_ DDT_ SPC_ S_ E_ T_ SPC_ I_ N_ S_ T_ R_ U_ M_ E_ N_ T_, &Midi_Edit_Set_Instrument },
    //{ E_ D_ I_ T_ DDT_ SPC_ P_ R_ E_ V_ I_ O_ U_ S_ SPC_ T_ R_ A_ C_ K_, &Midi_Edit_Previous_Track },
    //{ E_ D_ I_ T_ DDT_ SPC_ N_ E_ X_ T_ SPC_ T_ R_ A_ C_ K_, &Midi_Edit_Next_Track },
    //{ E_ D_ I_ T_ DDT_ SPC_ P_ R_ E_ V_ I_ O_ U_ S_ SPC_ R_ O_ W_, &Midi_Edit_Previous_Row },
    //{ E_ D_ I_ T_ DDT_ SPC_ N_ E_ X_ T_ SPC_ R_ O_ W_, &Midi_Edit_Next_Row },

    //{ S_ O_ N_ G_ DDT_ SPC_ S_ E_ T_ SPC_ V_ O_ L_ U_ M_ E_, &Midi_Song_Set_Volume },
    //{ S_ O_ N_ G_ DDT_ SPC_ S_ E_ T_ SPC_ B_ P_ M_, &Midi_Song_Set_BPM },

    //{ T_ R_ A_ C_ K_ DDT_ SPC_ O_ N_ SLA_ O_ F_ F_, &Midi_Edit_Track_On_Off },
    //{ T_ R_ A_ C_ K_ DDT_ SPC_ P_ A_ N_ N_ I_ N_ G_, &Midi_Track_Set_Panning },
    //{ T_ R_ A_ C_ K_ DDT_ SPC_ V_ O_ L_ U_ M_ E_, &Midi_Track_Set_Volume },
    //{ T_ R_ A_ C_ K_ DDT_ SPC_ L_ F_ O_ SPC_ C_ A_ R_ R_ I_ E_ R_, &Midi_Track_Set_LFO_Carrier },
    //{ T_ R_ A_ C_ K_ DDT_ SPC_ C_ U_ T_ O_ F_ F_, &Midi_Track_Set_Cutoff },
    //{ T_ R_ A_ C_ K_ DDT_ SPC_ R_ E_ S_ O_ N_ A_ N_ C_ E_, &Midi_Track_Set_Resonance },
    //{ T_ R_ A_ C_ K_ DDT_ SPC_ R_ E_ V_ E_ R_ B_, &Midi_Track_Set_Reverb },
    //{ T_ R_ A_ C_ K_ DDT_ SPC_ D_ I_ S_ T_ O_ SPC_ T_ H_ R_ E_ S_ H_ O_ L_ D_, &Midi_Track_Set_Disto_Threshold },
    //{ T_ R_ A_ C_ K_ DDT_ SPC_ D_ I_ S_ T_ O_ SPC_ C_ L_ A_ M_ P_, &Midi_Track_Set_Disto_Clamp },

    //{ _3 _0 _3 DOT_ _1 DDT_ SPC_ T_ U_ N_ E_, &Midi_303_Set_1_Tune },
    //{ _3 _0 _3 DOT_ _2 DDT_ SPC_ T_ U_ N_ E_, &Midi_303_Set_2_Tune },
    //{ _3 _0 _3 SPC_ OPA_ C_ U_ R_ R_ DOT_ CPA_ DDT_ SPC_ T_ U_ N_ E_, &Midi_303_Set_Cur_Tune },
    //{ _3 _0 _3 DOT_ _1 DDT_ SPC_ C_ U_ T_ O_ F_ F_, &Midi_303_Set_1_Cutoff },
    //{ _3 _0 _3 DOT_ _2 DDT_ SPC_ C_ U_ T_ O_ F_ F_, &Midi_303_Set_2_Cutoff },
    //{ _3 _0 _3 SPC_ OPA_ C_ U_ R_ R_ DOT_ CPA_ DDT_ SPC_ C_ U_ T_ O_ F_ F_, &Midi_303_Set_Cur_Cutoff },
    //{ _3 _0 _3 DOT_ _1 DDT_ SPC_ R_ E_ S_ O_ N_ A_ N_ C_ E_, &Midi_303_Set_1_Resonance },
    //{ _3 _0 _3 DOT_ _2 DDT_ SPC_ R_ E_ S_ O_ N_ A_ N_ C_ E_, &Midi_303_Set_2_Resonance },
    //{ _3 _0 _3 SPC_ OPA_ C_ U_ R_ R_ DOT_ CPA_ DDT_ SPC_ R_ E_ S_ O_ N_ A_ N_ C_ E_, &Midi_303_Set_Cur_Resonance },
    //{ _3 _0 _3 DOT_ _1 DDT_ SPC_ E_ N_ V_ M_ O_ D_, &Midi_303_Set_1_Envmod },
    //{ _3 _0 _3 DOT_ _2 DDT_ SPC_ E_ N_ V_ M_ O_ D_, &Midi_303_Set_2_Envmod },
    //{ _3 _0 _3 SPC_ OPA_ C_ U_ R_ R_ DOT_ CPA_ DDT_ SPC_ E_ N_ V_ M_ O_ D_, &Midi_303_Set_Cur_Envmod },
    //{ _3 _0 _3 DOT_ _1 DDT_ SPC_ D_ E_ C_ A_ Y_, &Midi_303_Set_1_Decay },
    //{ _3 _0 _3 DOT_ _2 DDT_ SPC_ D_ E_ C_ A_ Y_, &Midi_303_Set_2_Decay },
    //{ _3 _0 _3 SPC_ OPA_ C_ U_ R_ R_ DOT_ CPA_ DDT_ SPC_ D_ E_ C_ A_ Y_, &Midi_303_Set_Cur_Decay },
    //{ _3 _0 _3 DOT_ _1 DDT_ SPC_ A_ C_ C_ E_ N_ T_ , &Midi_303_Set_1_Accent },
    //{ _3 _0 _3 DOT_ _2 DDT_ SPC_ A_ C_ C_ E_ N_ T_, &Midi_303_Set_2_Accent },
    //{ _3 _0 _3 SPC_ OPA_ C_ U_ R_ R_ DOT_ CPA_ DDT_ SPC_ A_ C_ C_ E_ N_ T_, &Midi_303_Set_Cur_Accent },
    //{ _3 _0 _3 DOT_ _1 DDT_ SPC_ V_ O_ L_ U_ M_ E_, &Midi_303_Set_1_Volume },
    //{ _3 _0 _3 DOT_ _2 DDT_ SPC_ V_ O_ L_ U_ M_ E_, &Midi_303_Set_2_Volume },
    //{ _3 _0 _3 SPC_ OPA_ C_ U_ R_ R_ DOT_ CPA_ DDT_ SPC_ V_ O_ L_ U_ M_ E_, &Midi_303_Set_Cur_Volume },
    //{ _3 _0 _3 DDT_ SPC_ S_ W_ I_ T_ C_ H_, &Midi_303_Switch }
};

// ------------------------------------------------------
// Run various command associated to midi messages
void Dispatch_Midi_Msg(ptk_data *ptk, int CC, int Data)
{
    //int i;

    //// relations are 1n & n1
    //for(i = 0; i < sizeof(Midi_Dispatch_Table) / sizeof(MIDI_DISPATCH); i++)
    //{
    //    if(CC == Midi_Dispatch_Table[i].CC)
    //    {
    //        if(Str_Midi_Commands[Midi_Dispatch_Table[i].Automation].routine)
    //        {
    //            Str_Midi_Commands[Midi_Dispatch_Table[i].Automation].routine(Data);
    //        }
    //    }
    //}
}

