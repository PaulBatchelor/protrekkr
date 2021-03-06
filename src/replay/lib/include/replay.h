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

#ifndef _REPLAY_H_
#define _REPLAY_H_

// ------------------------------------------------------
// Includes
#include "tb_303.h"
#include "cubic_spline.h"
#include "spline.h"
#include "../../../include/ptk_data.h"
#if defined(__WIN32__)
#include "../sounddriver/include/sounddriver_windows.h"
#elif defined(__LINUX__)
#include "../sounddriver/include/sounddriver_linux.h"
#elif defined(__MACOSX__)
#include "../sounddriver/include/sounddriver_macosx.h"
#elif defined(__AROS__)
#include "../sounddriver/include/sounddriver_aros.h"
#elif defined(__AMIGAOS4__)
#include "../sounddriver/include/sounddriver_aos4.h"
#elif defined(__PSP__)
#include "../sounddriver/include/sounddriver_psp.h"
#else
#error "Can't work without any sound driver !"
#endif
#include "samples_unpack.h"
#include "ptkreplay.h"

// Types
#if defined(__WIN32__) && !defined(__GCC__)
typedef __int64 int64;
typedef unsigned __int64 Uint64;
#else
typedef long long int64;
#if defined(__LINUX__) && !defined(__FREEBSD__)
typedef uint64_t Uint64;
#else
typedef unsigned long long Uint64;
#endif
#endif

// ------------------------------------------------------
// Constants
#define USE_FASTPOW

#define PI 3.1415926535897932384626433832795
#define PIf 3.1415927f

#define MIX_RATE 44100
#define fMIX_RATE 44100.0f

#define NONE_INT 0
#define CUBIC_INT 1
#define SPLINE_INT 2

#define DEFAULT_BASE_NOTE 48

#define SMP_PACK_GSM 0
#define SMP_PACK_MP3 1
#define SMP_PACK_TRUESPEECH 2
#define SMP_PACK_NONE 3
#define SMP_PACK_AT3 4
#define SMP_PACK_ADPCM 5
#define SMP_PACK_8BIT 6
#define SMP_PACK_INTERNAL 7

#define MAX_FX 4

#define FLANGER_LOPASS_CUTOFF 0.1f
#define FLANGER_LOPASS_RESONANCE 0.4f

#define SMP_LOOP_NONE 0
#define SMP_LOOP_FORWARD 1
#define SMP_LOOP_PINGPONG 2

#define SMP_LOOPING_FORWARD 0
#define SMP_LOOPING_BACKWARD 1

#define SYNTH_WAVE_OFF 0
#define SYNTH_WAVE_CURRENT 1

#define PLAYING_NOSAMPLE 0
#define PLAYING_SAMPLE 1
#define PLAYING_SAMPLE_NOTEOFF 2
#define PLAYING_STOCK 3

#define SYNTH_ATTACK 1
#define SYNTH_DECAY 2
#define SYNTH_SUSTAIN 3
#define SYNTH_RELEASE 4

#if defined(USE_FASTPOW)
float FastPow(float a, float b);
float FastPow2(float a);
float FastLog(float i);
#define POWF(x, y) FastPow(x, y)
#define POWF2(x) FastPow2(x)
#else
#define POWF(x, y) powf(x, y)
#define POWF2(x) powf(2.0f, x)
#endif

#if !defined(__STAND_ALONE__) || defined(__WINAMP__)
#define RENDER_TO_FILE 0
#define RENDER_TO_MONO 1
#define RENDER_TO_STEREO 2
extern char rawrender;
extern char rawrender_32float;
extern char rawrender_multi;
extern int rawrender_range;
extern char rawrender_target;
extern int rawrender_from;
extern int rawrender_to;
#endif

// ------------------------------------------------------
// Structures

// SAMPLE COUNTER
struct smpos
{
#if defined(__BIG_ENDIAN__)
    Uint32 first;
    Uint32 last;
#else
    Uint32 last;
    Uint32 first;
#endif
};

union s_access
{
    smpos half;
    int64 absolu;
};

typedef struct
{
    int inote;
    int sample;
    float vol;
    float vol_synth;
    unsigned int offset;
    int glide;
    int Play_Selection;
    int midi_sub_channel;
    int start_backward;
    int age;
} INSTR_SCHEDULE, *LPINSTR_SCHEDULE;

typedef struct
{
    int Line;
    int Pos;
    int SamplesPerTick;
    int shufflestep;
} VISUAL_DELAY_DAT, *LPVISUAL_DELAY_DAT;

typedef struct
{
    float lf;
    float f1p0[2];
    float f1p1[2];
    float f1p2[2];
    float f1p3[2];

    float hf;
    float f2p0[2];
    float f2p1[2];
    float f2p2[2];
    float f2p3[2];

    float sdm1[2];
    float sdm2[2];
    float sdm3[2];

    float lg;
    float mg;
    float hg;
} EQSTATE, *LPEQSTATE;

// ------------------------------------------------------
// Variables
extern float decays[MAX_COMB_FILTERS];
extern int delays[MAX_COMB_FILTERS];       // delays for the comb filters
extern int counters_L[MAX_COMB_FILTERS];
extern int counters_R[MAX_COMB_FILTERS];

#if defined(PTK_MP3)
extern char Mp3_BitRate[MAX_INSTRS];
extern int Type_Mp3_BitRate[];
#endif

#if defined(PTK_AT3)
extern char At3_BitRate[MAX_INSTRS];
extern int Type_At3_BitRate[];
#endif

#if defined(PTK_COMPRESSOR)
extern char num_echoes;
#endif

#if defined(PTK_303)
extern unsigned char track3031;
extern unsigned char track3032;
#endif

//extern unsigned char *RawPatterns;
extern int Song_Position;
extern unsigned int lchorus_counter;
extern unsigned int rchorus_counter;
extern unsigned int lchorus_counter2;
extern unsigned int rchorus_counter2;
extern int lchorus_delay;
extern int rchorus_delay;
extern float Track_Volume[MAX_TRACKS];
extern float mas_comp_threshold_Track[MAX_TRACKS];
extern float mas_comp_ratio_Track[MAX_TRACKS];
extern char Compress_Track[MAX_TRACKS];
extern float mas_comp_threshold_Master;
extern float mas_comp_ratio_Master;
extern char Compress_Master;
extern char Songtracks;
extern unsigned char Song_Length;
extern unsigned char pSequence[256];
extern short patternLines[MAX_ROWS];
extern char nameins[MAX_INSTRS][20];
extern char Midiprg[MAX_INSTRS];

#if !defined(__STAND_ALONE__)
extern char SamplesSwap[MAX_INSTRS];
extern short *RawSamples_Swap[MAX_INSTRS][2][MAX_INSTRS_SPLITS];
extern Uint32 SampleLength_Packed[MAX_INSTRS][MAX_INSTRS_SPLITS];
#endif

extern char SampleType[MAX_INSTRS][MAX_INSTRS_SPLITS];
extern char SampleCompression[MAX_INSTRS];
extern char SampleName[MAX_INSTRS][MAX_INSTRS_SPLITS][64];
extern char Basenote[MAX_INSTRS][MAX_INSTRS_SPLITS];
extern Uint32 LoopStart[MAX_INSTRS][MAX_INSTRS_SPLITS];
extern Uint32 LoopEnd[MAX_INSTRS][MAX_INSTRS_SPLITS];
extern char LoopType[MAX_INSTRS][MAX_INSTRS_SPLITS];
extern Uint32 SampleLength[MAX_INSTRS][MAX_INSTRS_SPLITS];
extern char Finetune[MAX_INSTRS][MAX_INSTRS_SPLITS];
extern float Sample_Amplify[MAX_INSTRS][MAX_INSTRS_SPLITS];
extern float FDecay[MAX_INSTRS][MAX_INSTRS_SPLITS];
extern short *RawSamples[MAX_INSTRS][2][MAX_INSTRS_SPLITS];
extern char SampleChannels[MAX_INSTRS][MAX_INSTRS_SPLITS];         // Mono / Stereo
extern float TCut[MAX_TRACKS];
extern float ICut[MAX_TRACKS];
extern float TPan[MAX_TRACKS];
extern int FType[MAX_TRACKS];
extern int FRez[MAX_TRACKS];
extern float DThreshold[MAX_TRACKS];
extern float DClamp[MAX_TRACKS];
extern float DSend[MAX_TRACKS]; 
extern int CSend[MAX_TRACKS];
extern char Channels_Polyphony[MAX_TRACKS];
extern char compressor; // 0-->Off 1-->On
extern int c_threshold;
extern int BeatsPerMin;
extern int TicksPerBeat;
extern float mas_vol;
extern float local_mas_vol;
extern volatile float local_ramp_vol;

extern int delay_time;
extern float Feedback;

extern float lchorus_feedback;
extern float rchorus_feedback;
extern int shuffle;

extern char CHAN_ACTIVE_STATE[256][16];
extern char CHAN_HISTORY_STATE[256][16];
extern float CCoef[MAX_TRACKS];
extern int CHAN_MIDI_PRG[MAX_TRACKS];

extern char LFO_ON[MAX_TRACKS];
extern float LFO_RATE[MAX_TRACKS];
extern float LFO_AMPL[MAX_TRACKS];

extern char FLANGER_ON[MAX_TRACKS];
extern float FLANGER_AMOUNT[MAX_TRACKS];
extern float FLANGER_DEPHASE[MAX_TRACKS];
extern float FLANGER_RATE[MAX_TRACKS];
extern float FLANGER_AMPL[MAX_TRACKS];
extern float FLANGER_GR[MAX_TRACKS];
extern float FLANGER_FEEDBACK[MAX_TRACKS];
extern int FLANGER_DELAY[MAX_TRACKS];
extern int FLANGER_OFFSET[MAX_TRACKS];

extern float FLANGER_OFFSET2[MAX_TRACKS];
extern float FLANGER_OFFSET1[MAX_TRACKS];

extern int CHAN_MUTE_STATE[MAX_TRACKS]; // 0->Normal 1->Muted
extern char Disclap[MAX_TRACKS];

extern char artist[20];
extern char style[20];

extern char beatsync[128];
extern short beatlines[128];

extern float Reverb_Filter_Cutoff;
extern float Reverb_Filter_Resonance;
extern unsigned char Reverb_Stereo_Amount;

extern float Sample_Vol[MAX_INSTRS];

#if !defined(__STAND_ALONE__)
extern unsigned int SubCounter;
extern int PosInTick;
extern int plx;
extern int Reserved_Sub_Channels[MAX_TRACKS][MAX_POLYPHONY];
extern int Locked_Sub_Channels[MAX_TRACKS][MAX_POLYPHONY];
extern int sp_Stage[MAX_TRACKS][MAX_POLYPHONY];
extern int Cut_Stage[MAX_TRACKS][MAX_POLYPHONY];
extern int Glide_Stage[MAX_TRACKS][MAX_POLYPHONY];

extern int L_MaxLevel;
extern int R_MaxLevel;

extern float oldspawn[MAX_TRACKS];
extern float roldspawn[MAX_TRACKS];

extern float Player_FD[MAX_TRACKS];
extern char sp_channelsample[MAX_TRACKS][MAX_POLYPHONY];
extern char sp_split[MAX_TRACKS][MAX_POLYPHONY];
#if defined(__PSP__)
extern volatile int Songplaying;
#else
extern int Songplaying;
#endif
extern int left_value;
extern int right_value;


extern float ramper[MAX_TRACKS];
extern unsigned char nPatterns;
extern int delay_time;
extern int DelayType;
#endif

#if defined(PTK_TRACK_EQ)
extern EQSTATE EqDat[MAX_TRACKS];
#endif

// ------------------------------------------------------
// Functions
void Pre_Song_Init(ptk_data *ptk);
void Sp_Player(ptk_data *ptk);
void Schedule_Instrument(ptk_data *ptk, int channel, int sub_channel,
                         int note, int sample,
                         unsigned int offset,
                         int glide,
                         int Play_Selection,
                         int midi_sub_channel,
                         float volume,
                         int Pos,
                         int Row);
void Play_Instrument(ptk_data *ptk, int channel, int sub_channel);
void ResetFilters(ptk_data *ptk, int tr);
void ComputeStereo(ptk_data *ptk, int channel);
void FixStereo(ptk_data *ptk, int channel);
void GetPlayerValues(ptk_data *ptk);
void noteoff303(ptk_data *ptk, char strack);
void init_sample_bank(ptk_data *ptk);
void KillInst(ptk_data *ptk, int inst_nbr, int all_splits);
void Post_Song_Init(ptk_data *ptk);

#if defined(PTK_LIMITER_MASTER)
void Mas_Compressor_Set_Variables_Master(ptk_data *ptk, float treshold, float ratio);
float Mas_Compressor_Master(ptk_data *ptk, float input, float *rms_sum, float *Buffer, float *Env);
#endif
#if defined(PTK_LIMITER_TRACKS)
void Mas_Compressor_Set_Variables_Track(ptk_data *ptk, int Track, float treshold, float ratio);
float Mas_Compressor_Track(ptk_data *ptk, int Track, float input, float *rms_sum, float *Buffer, float *Env);
#endif
#if defined(PTK_LIMITER_TRACKS) || defined(PTK_LIMITER_MASTER)
float Do_RMS(ptk_data *ptk, float input, float *rms_sum, float *buffer);
#endif
int Get_Free_Sub_Channel(ptk_data *ptk, int channel, int polyphony);
int Get_Pattern_Offset(ptk_data *ptk, int pattern, int track, int row);
void InitReverbFilter(ptk_data *ptk);
void Set_Spline_Boundaries(ptk_data *ptk, unsigned int Position,
                           unsigned int *Boundaries,
                           int LoopType,
                           unsigned int LoopWay,
                           unsigned int Length,
                           unsigned int LoopEnd,
                           unsigned int LoopStart);
float Process_Sample(ptk_data *ptk, short *Data, int c, int i, unsigned int res_dec);
void init_eq(ptk_data *pt, LPEQSTATE es);
float do_eq(ptk_data *ptk, LPEQSTATE es, float sample, int Left);
#if defined(PTK_SHUFFLE)
void Update_Shuffle(ptk_data *ptk);
#endif

#if defined(PTK_COMPRESSOR)
class rFilter
{
    private: 

        float buffy0;
        float buffy1;

    public:

        rFilter()
        {
            Reset();
        };

        void Reset()
        {
            buffy0 = 0.0f;
            buffy1 = 0.0f;
        };

        float fWork(float input, float f, float q)
        {
            float fa = 1.0f - f;
            float fb = float(q * (1.0f + (1.0f / fa)));

            buffy0 = fa * buffy0 + f * (input + fb * (buffy0 - buffy1));
            buffy1 = fa * buffy1 + f * buffy0;
            return buffy1;
        };
};
#endif

#endif
