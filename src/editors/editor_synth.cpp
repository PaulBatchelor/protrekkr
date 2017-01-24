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
#include "editor_synth.h"
#include "editor_instrument.h"
#include "timer.h"
#include "files.h"
#include "requesters.h"

// ------------------------------------------------------
// Variables
extern REQUESTER Overwrite_Requester;

extern int plx;
int csynth_slv_OSC1;
int csynth_slv_OSC2;
int csynth_slv_VCF;
int csynth_slv_LFO1;
int csynth_slv_LFO2;
int csynth_slv_ENV1;
int csynth_slv_ENV2;
int csynth_slv_Misc;

int Allow_All;
int Allow_Button;
int Allow_Button_Pushed;
int Allow_Phase_Distortion_OSC1 = TRUE;
int Allow_Phase_Distortion_OSC2 = TRUE;

void Renew_Sample_Ed(ptk_data *ptk);
void Center_SynthParam_OSC1(ptk_data *ptk);
void Center_SynthParam_OSC2(ptk_data *ptk);
void Center_SynthParam_VCF(ptk_data *ptk);
void Center_SynthParam_LFO1(ptk_data *ptk);
void Center_SynthParam_LFO2(ptk_data *ptk);
void Center_SynthParam_ENV1(ptk_data *ptk);
void Center_SynthParam_ENV2(ptk_data *ptk);
void Center_SynthParam_Misc(ptk_data *ptk);
void Rand_OSC1(ptk_data *ptk);
void Rand_OSC2(ptk_data *ptk);
void Rand_VCF(ptk_data *ptk);
void Rand_LFO1(ptk_data *ptk);
void Rand_LFO2(ptk_data *ptk);
void Rand_ENV1(ptk_data *ptk);
void Rand_ENV2(ptk_data *ptk);
void Rand_Misc(ptk_data *ptk);
void CParcha_OSC1(ptk_data *ptk, int cpar);
void CParcha_OSC2(ptk_data *ptk, int cpar);
void CParcha_VCF(ptk_data *ptk, int cpar);
void CParcha_LFO1(ptk_data *ptk, int cpar);
void CParcha_LFO2(ptk_data *ptk, int cpar);
void CParcha_ENV1(ptk_data *ptk, int cpar);
void CParcha_ENV2(ptk_data *ptk, int cpar);
void CParcha_Misc(ptk_data *ptk, int cpar);

// 2 = no filter by legacy
int filter_vcf_type_plus[] =
{
    1, 3, 2, 4, 2
};

int filter_vcf_type_minus[] =
{
    0, 0, 4, 1, 3
};

int Tbl_Synth_OSC1[] =
{
    1, /* Osc1_PD */
    7, /* ENV1_Attack */
    8, /* ENV1_Decay */
    9, /* ENV1_Sustain */
    10, /* ENV1_Release */
    49, /* OSC3 volume */
};
int Pos_Tbl_Synth_OSC1;
int Size_Tbl_Synth_OSC1 = (sizeof(Tbl_Synth_OSC1) / sizeof(int)) - 1;
char Names_Tbl_Synth_OSC1[(sizeof(Tbl_Synth_OSC1) / sizeof(int))][24];

int Tbl_Synth_OSC2[] =
{
    2, /* Osc2_PD */
    3, /* Osc2_Detune */
    4, /* Osc2_Finetune */
    11, /* ENV2_Attack */
    12, /* ENV2_Decay */
    13, /* ENV2_Sustain */
    14, /* ENV2_Release */
};
int Pos_Tbl_Synth_OSC2;
int Size_Tbl_Synth_OSC2 = (sizeof(Tbl_Synth_OSC2) / sizeof(int)) - 1;
char Names_Tbl_Synth_OSC2[(sizeof(Tbl_Synth_OSC2) / sizeof(int))][24];

int Tbl_Synth_VCF[] =
{
    5, /* VCF Cutoff */
    6, /* VCF_Resonance */
};
int Pos_Tbl_Synth_VCF;
int Size_Tbl_Synth_VCF = (sizeof(Tbl_Synth_VCF) / sizeof(int)) - 1;
char Names_Tbl_Synth_VCF[(sizeof(Tbl_Synth_VCF) / sizeof(int))][24];

int Tbl_Synth_LFO1[] =
{
    15, /* LFO1_Period */
    17, /* Lfo1->osc1 pd */
    18, /* Lfo1->osc2 pd */
    19, /* Lfo1->osc1 pitch */
    20, /* Lfo1->osc1 pitch */
    21, /* Lfo1->osc1 volume */
    22, /* Lfo1->osc2 volume */
    23, /* Lfo1->vcf cutoff */
    24, /* Lfo1->vcf resonance */
    25, /* Lfo1_attack */
    26, /* Lfo1_decay */
    27, /* Lfo1_sustain */
    28, /* Lfo1_release */
};
int Pos_Tbl_Synth_LFO1;
int Size_Tbl_Synth_LFO1 = (sizeof(Tbl_Synth_LFO1) / sizeof(int)) - 1;
char Names_Tbl_Synth_LFO1[(sizeof(Tbl_Synth_LFO1) / sizeof(int))][24];

int Tbl_Synth_LFO2[] =
{
    16, /* LFO2_Period */
    25, /* Lfo2->osc1 pd */
    26, /* Lfo2->osc2 pd */
    27, /* Lfo2->osc1 pitch */
    28, /* Lfo2->osc2 pitch */
    29, /* Lfo2->osc1 volume */
    30, /* Lfo2->osc2 volume */
    31, /* Lfo2->vcf cutoff */
    32, /* Lfo2->vcf resonance */
    25, /* Lfo2_attack */
    26, /* Lfo2_decay */
    27, /* Lfo2_sustain */
    28, /* Lfo2_release */
};
int Pos_Tbl_Synth_LFO2;
int Size_Tbl_Synth_LFO2 = (sizeof(Tbl_Synth_LFO2) / sizeof(int)) - 1;
char Names_Tbl_Synth_LFO2[(sizeof(Tbl_Synth_LFO2) / sizeof(int))][24];

int Tbl_Synth_ENV1[] =
{
    33, /* Env1->osc1 pd */
    34, /* Env1->osc2 pd */
    35, /* Env1->osc1 pitch */
    36, /* Env1->osc2 pitch */
    37, /* Env1->osc1 volume */
    38, /* Env1->osc2 volume */
    39, /* Env1->vcf cutoff */
    40, /* Env1->vcf resonance */
};
int Pos_Tbl_Synth_ENV1;
int Size_Tbl_Synth_ENV1 = (sizeof(Tbl_Synth_ENV1) / sizeof(int)) - 1;
char Names_Tbl_Synth_ENV1[(sizeof(Tbl_Synth_ENV1) / sizeof(int))][24];

int Tbl_Synth_ENV2[] =
{
    41, /* Env2->osc1 pd */
    42, /* Env2->osc2 pd */
    43, /* Env2->osc1 pitch */
    44, /* Env2->osc2 pitch */
    45, /* Env2->osc1 volume */
    46, /* Env2->osc2 volume */
    47, /* Env2->vcf cutoff */
    48, /* Env2->vcf resonance */
};
int Pos_Tbl_Synth_ENV2;
int Size_Tbl_Synth_ENV2 = (sizeof(Tbl_Synth_ENV2) / sizeof(int)) - 1;
char Names_Tbl_Synth_ENV2[(sizeof(Tbl_Synth_ENV2) / sizeof(int))][24];

int Tbl_Synth_Misc[] =
{
    50, /* Misc. glide */
    51, /* Misc. Global volume */
    52, /* Misc. Distortion */
};
int Pos_Tbl_Synth_Misc;
int Size_Tbl_Synth_Misc = (sizeof(Tbl_Synth_Misc) / sizeof(int)) - 1;
char Names_Tbl_Synth_Misc[(sizeof(Tbl_Synth_Misc) / sizeof(int))][24];

void Draw_Synth_Ed(ptk_data *ptk)
{
}

void Actualize_Synth_Ed(ptk_data *ptk, char gode)
{
}

void Check_Slider(ptk_data *ptk, int *Value, int x, int y)
{
}

void Check_Sliders_Arrows(ptk_data *ptk, int value_step)
{
}

void Mouse_Right_Synth_Ed(ptk_data *ptk)
{
}

void Mouse_Left_Synth_Ed(ptk_data *ptk)
{
}

void Actualize_SynthParSlider(ptk_data *ptk)
{
}

void Center_SynthParam_OSC2(ptk_data *ptk)
{
}

void Center_SynthParam_VCF(ptk_data *ptk)
{
}

void Center_SynthParam_LFO1(ptk_data *ptk)
{
}

void Center_SynthParam_LFO2(ptk_data *ptk)
{
}

void Center_SynthParam_ENV1(ptk_data *ptk)
{
}

void Center_SynthParam_ENV2(ptk_data *ptk)
{
}

void Center_SynthParam_Misc(ptk_data *ptk)
{
}

void CParcha_OSC1(ptk_data *ptk, int cpar)
{
}

void CParcha_OSC2(ptk_data *ptk, int cpar)
{
}

void CParcha_VCF(ptk_data *ptk, int cpar)
{
}

void CParcha_LFO1(ptk_data *ptk, int cpar)
{
}

void CParcha_LFO2(ptk_data *ptk, int cpar)
{
}

void CParcha_ENV1(ptk_data *ptk, int cpar)
{
}

void CParcha_ENV2(ptk_data *ptk, int cpar)
{
}

void CParcha_Misc(ptk_data *ptk, int cpar)
{
}

void Rand_OSC1(ptk_data *ptk)
{
}

void Rand_OSC2(ptk_data *ptk)
{
}

void Rand_VCF(ptk_data *ptk)
{
}

void Rand_LFO1(ptk_data *ptk)
{
}

void Rand_LFO2(ptk_data *ptk)
{
}

void Rand_ENV1(ptk_data *ptk)
{
}

void Rand_ENV2(ptk_data *ptk)
{
}

void Rand_Misc(ptk_data *ptk)
{
}

void IniCsParNames(ptk_data *ptk)
{
}
