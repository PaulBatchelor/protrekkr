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
#include "../extralibs/zlib-1.2.3/zlib.h"

#include "ptk.h"
#include "files.h"
#include "mods.h"
#include "patterns.h"
#include "reverbs.h"
#include "303s.h"
#include "synths.h"
#include "ptps.h"
#include "misc_draw.h"
#include "samples_pack.h"
#include "editor_synth.h"
#include "editor_diskio.h"
#include "editor_reverb.h"
#include "editor_instrument.h"
#include "editor_sequencer.h"
#include "editor_setup.h"
#include "editor_midi.h"
#include "editor_303.h"
#include "editor_sample.h"
#include "editor_fx_setup.h"
#include "editor_track_fx.h"
#include "editor_track.h"
#include "editor_pattern.h"

#include "endianness.h"

// ------------------------------------------------------
// Variables
extern REQUESTER Overwrite_Requester;
extern char OverWrite_Name[1024];

extern SynthParameters PARASynth[128];

extern int Beveled;
char AutoBackup;

int Mod_Length;
int Mod_Simulate;
Uint8 *Mod_Memory;
int Mod_Mem_Pos;
int Final_Mod_Length;

// ------------------------------------------------------
// Functions
int Read_Mod_Data(ptk_data *ptk, void *Datas, int Unit, int Length, FILE *Handle);
int Read_Mod_Data_Swap(ptk_data *ptk, void *Datas, int Unit, int Length, FILE *Handle);
short *Unpack_Sample(ptk_data *ptk, FILE *FileHandle, int Dest_Length, char Pack_Type, int BitRate);
void Swap_Short_Buffer(ptk_data *ptk, short *buffer, int size);
short *Swap_New_Sample(ptk_data *ptk, short *buffer, int sample, int bank);

// ------------------------------------------------------
// Prepare the tracker interface once a module has been loaded
void Init_Tracker_Context_After_ModLoad(ptk_data *ptk)
{
    ptk->Track_Under_Caret = 0;
    ptk->Current_Instrument = 0;
    ptk->Column_Under_Caret = 0;

    Pattern_Line = 0;
    Pattern_Line_Visual = 0;
    Song_Position = 0;
    Song_Position_Visual = 0;

    Final_Mod_Length = 0;

    Post_Song_Init(ptk);

    Draw_Scope(ptk);
    ptk->gui_track = 0;

    lchorus_counter = MIX_RATE;
    rchorus_counter = MIX_RATE;
    lchorus_counter2 = MIX_RATE - lchorus_delay;
    rchorus_counter2 = MIX_RATE - rchorus_delay;
    Initreverb(ptk);

#if defined(PTK_LIMITER_MASTER)
    Mas_Compressor_Set_Variables_Master(ptk, mas_comp_threshold_Master,
                                        mas_comp_ratio_Master);
#endif

#if defined(PTK_LIMITER_TRACKS)
    int i;
    for(i = 0; i < MAX_TRACKS; i++)
    {
        Mas_Compressor_Set_Variables_Track(ptk, i,
                                           mas_comp_threshold_Track[i],
                                           mas_comp_ratio_Track[i]);
    }
#endif

    Reset_Song_Length(ptk);

	if(ptk->start_gui == TRUE) Display_Song_Length(ptk);

    ptk->Scopish = SCOPE_ZONE_MOD_DIR;
    //Draw_Scope_Files_Button();

    Reset_Tracks_To_Render(ptk);

	if(ptk->start_gui == TRUE) Refresh_UI_Context(ptk);
    Unselect_Selection(ptk);

}

// ------------------------------------------------------
// Load a module file
int LoadPtk(ptk_data *ptk, char *FileName)
{
    int Ye_Old_Phony_Value;
    int New_adsr = FALSE;
    int New_Comp = FALSE;
    int Portable = FALSE;
    int Poly = FALSE;
    int Multi = FALSE;
    int Sel_Interpolation = FALSE;
    int New_Reverb = FALSE;
    int Env_Modulation = FALSE;
    int New_Env = FALSE;
    int Fx2 = FALSE;
    int XtraFx = FALSE;
    int Combine = FALSE;
    int Stereo_Reverb = FALSE;
    int Reverb_Resonance = FALSE;
    int Tb303_Scaling = FALSE;
    char Comp_Flag;
    int i;
    int j;
    int k;
    int Old_Ntk;
    int Ntk_Beta;
    unsigned char *TmpPatterns;
    unsigned char *TmpPatterns_Tracks;
    unsigned char *TmpPatterns_Notes;
    unsigned char *TmpPatterns_Rows;
    int Old_Bug = TRUE;
    int new_disto = FALSE;
    int Pack_Scheme = FALSE;
    int Mp3_Scheme = FALSE;
    int Compress_Tracks = FALSE;
    int tps_pos;
    int tps_trk;
    int twrite;
    int fake_value;
    int Packed_Size;
    int UnPacked_Size;
    int Flanger_Bug = FALSE;
    unsigned char *Packed_Module = NULL;

    Mod_Simulate = LOAD_READ;
    Mod_Mem_Pos = 0;
    Mod_Memory = NULL;

    FILE *in;
    in = fopen(FileName, "rb");
    Old_Ntk = FALSE;
    Ntk_Beta = FALSE;

    if(in != NULL)
    {

        Status_Box(ptk, "Attempting to load the song file...");

        Songplaying = FALSE;

        // Reading and checking extension...
        char extension[10];
        Read_Data(ptk, extension, sizeof(char), 9, in);

        switch(extension[7])
        {
            case 'O':
                Tb303_Scaling = TRUE;
            case 'N':
                Reverb_Resonance = TRUE;
            case 'M':
                Stereo_Reverb = TRUE;
            case 'L':
                XtraFx = TRUE;
                Combine = TRUE;
            case 'K':
                Compress_Tracks = TRUE;
            case 'J':
                Flanger_Bug = TRUE;
            case 'I':
                Fx2 = TRUE;
            case 'H':
                New_Env = TRUE;
            case 'G':
                Env_Modulation = TRUE;
            case 'F':
                New_Reverb = TRUE;
            case 'E':
                Sel_Interpolation = TRUE;
            case 'D':
                Mp3_Scheme = TRUE;
            case 'C':
                Multi = TRUE;
            case 'B':
                Poly = TRUE;
            case 'A':
                Portable = TRUE;
            case '9':
                Mod_Simulate = LOAD_READMEM;
            case '8':
                New_Comp = TRUE;
            case '7':
                New_adsr = TRUE;
            case '6':
                Pack_Scheme = TRUE;
            case '5':
                new_disto = TRUE;
            case '4':
                Old_Bug = FALSE;
            case '3':
                goto Read_Mod_File;

            // Old noisetrekker
            case '2':
                Old_Ntk = TRUE;

            // Noisetrekker Beta (1.6)
            case '1':
                Ntk_Beta = TRUE;
        }

Read_Mod_File:

        Status_Box(ptk, "Loading song -> Header...");
        Free_Samples(ptk);

        mas_comp_threshold_Master = 100.0f;
        mas_comp_ratio_Master = 0.0f;

        for(i = 0; i < MAX_TRACKS; i++)
        {
            mas_comp_threshold_Track[i] = 100.0f;
            mas_comp_ratio_Track[i] = 0.0f;
        }

        allow_save = TRUE;

        Clear_Patterns_Pool(ptk);

#if !defined(__NO_MIDI__)
        Midi_Reset(ptk);
#endif

        init_sample_bank(ptk);
        Pre_Song_Init(ptk);

        // Load the module into memory and depack it
        if(Mod_Simulate == LOAD_READMEM)
        {
            Packed_Size = Get_File_Size(ptk, in) - 9;
            Packed_Module = (unsigned char *) malloc(Packed_Size);
            if(Packed_Module)
            {
                if(Poly)
                {
                    Read_Data_Swap(ptk, &UnPacked_Size, sizeof(int), 1, in);
                }
                else
                {
                    UnPacked_Size = Packed_Size * 10;
                }
                Read_Data(ptk, Packed_Module, sizeof(char), Packed_Size, in);
                Mod_Memory = Depack_Data(ptk, Packed_Module, Packed_Size, UnPacked_Size);
                Mod_Mem_Pos = 0;
                free(Packed_Module);
            }
        }

        Read_Mod_Data(ptk, FileName, sizeof(char), 20, in);
        Read_Mod_Data(ptk, &nPatterns, sizeof(char), 1, in);

        Songtracks = MAX_TRACKS;
        Read_Mod_Data(ptk, &Song_Length, sizeof(char), 1, in);

        Use_Cubic = CUBIC_INT;

        if(Sel_Interpolation)
        {
            Read_Mod_Data(ptk, &Use_Cubic, sizeof(char), 1, in);
        }

        Read_Mod_Data(ptk, pSequence, sizeof(char), 256, in);

        Clear_Patterns_Pool(ptk);

        // Load the patterns rows infos
        if(!Ntk_Beta)
        {
            for(i = 0; i < MAX_ROWS; i++)
            {
                Read_Mod_Data_Swap(ptk, &patternLines[i], sizeof(short), 1, in);
            }
        }

        // Multi notes
        if(Multi)
        {
            Read_Mod_Data(ptk, Channels_MultiNotes, sizeof(char), MAX_TRACKS, in);
        }

        // Up to 4 fx
        if(XtraFx)
        {
            Read_Mod_Data(ptk, Channels_Effects, sizeof(char), MAX_TRACKS, in);
            for(i = 0; i < MAX_TRACKS; i++)
            {
                Read_Mod_Data_Swap(ptk, &Track_Volume[i], sizeof(float), 1, in);
            }
            for(i = 0; i < MAX_TRACKS; i++)
            {
                init_eq(ptk, &EqDat[i]);
                Read_Mod_Data_Swap(ptk, &EqDat[i].lg, sizeof(float), 1, in);
                Read_Mod_Data_Swap(ptk, &EqDat[i].mg, sizeof(float), 1, in);
                Read_Mod_Data_Swap(ptk, &EqDat[i].hg, sizeof(float), 1, in);
            }
        }

        // Load the patterns data
        int Bytes_Per_Track = PATTERN_BYTES;
        if(!Multi)
        {
            Bytes_Per_Track = 6;
        }

        TmpPatterns = RawPatterns;
        for(int pwrite = 0; pwrite < nPatterns; pwrite++)
        {
            TmpPatterns_Rows = TmpPatterns + (pwrite * PATTERN_LEN);
            int Rows_To_Read = MAX_ROWS;
            if(Ntk_Beta) Rows_To_Read = 64;

            for(j = 0; j < Rows_To_Read; j++)
            {
                // Bytes / track
                for(k = 0; k < Songtracks; k++)
                {
                    // Tracks
                    TmpPatterns_Tracks = TmpPatterns_Rows + (k * PATTERN_BYTES);
                    // Rows
                    TmpPatterns_Notes = TmpPatterns_Tracks + (j * PATTERN_ROW_LEN);
                    if(Multi)
                    {
                        for(i = 0; i < MAX_POLYPHONY; i++)
                        {
                            Read_Mod_Data(ptk, TmpPatterns_Notes + PATTERN_NOTE1 + (i * 2), sizeof(char), 1, in);
                            Read_Mod_Data(ptk, TmpPatterns_Notes + PATTERN_INSTR1 + (i * 2), sizeof(char), 1, in);
                        }
                    }
                    else
                    {
                        Read_Mod_Data(ptk, TmpPatterns_Notes + PATTERN_NOTE1, sizeof(char), 1, in);
                        Read_Mod_Data(ptk, TmpPatterns_Notes + PATTERN_INSTR1, sizeof(char), 1, in);
                    }
                    Read_Mod_Data(ptk, TmpPatterns_Notes + PATTERN_VOLUME, sizeof(char), 1, in);
                    Read_Mod_Data(ptk, TmpPatterns_Notes + PATTERN_PANNING, sizeof(char), 1, in);
                    Read_Mod_Data(ptk, TmpPatterns_Notes + PATTERN_FX, sizeof(char), 1, in);
                    Read_Mod_Data(ptk, TmpPatterns_Notes + PATTERN_FXDATA, sizeof(char), 1, in);
                    if(Fx2)
                    {
                        Read_Mod_Data(ptk, TmpPatterns_Notes + PATTERN_FX2, sizeof(char), 1, in);
                        Read_Mod_Data(ptk, TmpPatterns_Notes + PATTERN_FXDATA2, sizeof(char), 1, in);
                        if(!XtraFx)
                        {
                            // Check if there's data on older 2nd effects columns
                            int d = *(TmpPatterns_Notes + PATTERN_FX2);
                            int d2 = *(TmpPatterns_Notes + PATTERN_FXDATA2);
                            if(*(TmpPatterns_Notes + PATTERN_FX2) ||
                               *(TmpPatterns_Notes + PATTERN_FXDATA2)
                              )
                            {
                                Channels_Effects[k] = 2;
                            }
                        }
                    }
                    if(XtraFx)
                    {
                        Read_Mod_Data(ptk, TmpPatterns_Notes + PATTERN_FX3, sizeof(char), 1, in);
                        Read_Mod_Data(ptk, TmpPatterns_Notes + PATTERN_FXDATA3, sizeof(char), 1, in);
                        Read_Mod_Data(ptk, TmpPatterns_Notes + PATTERN_FX4, sizeof(char), 1, in);
                        Read_Mod_Data(ptk, TmpPatterns_Notes + PATTERN_FXDATA4, sizeof(char), 1, in);
                    }
                }
            }
        }

        Status_Box(ptk, "Loading song -> Sample data...");
        for(int swrite = 0; swrite < MAX_INSTRS; swrite++)
        {
            Read_Mod_Data(ptk, &nameins[swrite], sizeof(char), 20, in);
            Read_Mod_Data(ptk, &Midiprg[swrite], sizeof(char), 1, in);
            Read_Mod_Data(ptk, &Synthprg[swrite], sizeof(char), 1, in);

            PARASynth[swrite].disto = 0;

            PARASynth[swrite].lfo1_attack = 0;
            PARASynth[swrite].lfo1_decay = 0;
            PARASynth[swrite].lfo1_sustain = 128;
            PARASynth[swrite].lfo1_release = 0x10000;

            PARASynth[swrite].lfo2_attack = 0;
            PARASynth[swrite].lfo2_decay = 0;
            PARASynth[swrite].lfo2_sustain = 128;
            PARASynth[swrite].lfo2_release = 0x10000;

            Read_Synth_Params(ptk, Read_Mod_Data, Read_Mod_Data_Swap, in, swrite,
                              new_disto, New_adsr, Portable, Env_Modulation,
                              New_Env, Ntk_Beta, Combine);

            // Fix some very old Ntk bugs
            if(PARASynth[swrite].lfo1_period > 128) PARASynth[swrite].lfo1_period = 128;
            if(PARASynth[swrite].lfo2_period > 128) PARASynth[swrite].lfo2_period = 128;
            if(Old_Ntk)
            {
                if(PARASynth[swrite].ptc_glide < 1) PARASynth[swrite].ptc_glide = 64;
                if(PARASynth[swrite].glb_volume < 1) PARASynth[swrite].glb_volume = 64;
            }

            // Compression type
            SampleCompression[swrite] = SMP_PACK_INTERNAL;
            if(Pack_Scheme)
            {
                Read_Mod_Data(ptk, &SampleCompression[swrite], sizeof(char), 1, in);
                if(Mp3_Scheme)
                {
                    switch(SampleCompression[swrite])
                    {
                        case SMP_PACK_MP3:
                            Read_Mod_Data(ptk, &Mp3_BitRate[swrite], sizeof(char), 1, in);
                            break;

                        case SMP_PACK_AT3:
                            Read_Mod_Data(ptk, &At3_BitRate[swrite], sizeof(char), 1, in);
                            break;
                    }
                }
                SampleCompression[swrite] = Fix_Codec(ptk, SampleCompression[swrite]);
            }
            for(int slwrite = 0; slwrite < MAX_INSTRS_SPLITS; slwrite++)
            {
                Read_Mod_Data(ptk, &SampleType[swrite][slwrite], sizeof(char), 1, in);
                if(SampleType[swrite][slwrite])
                {
                    if(Old_Bug) Read_Mod_Data(ptk, &SampleName[swrite][slwrite], sizeof(char), 256, in);
                    else Read_Mod_Data(ptk, &SampleName[swrite][slwrite], sizeof(char), 64, in);
                    Read_Mod_Data(ptk, &Basenote[swrite][slwrite], sizeof(char), 1, in);

                    Read_Mod_Data_Swap(ptk, &LoopStart[swrite][slwrite], sizeof(int), 1, in);
                    Read_Mod_Data_Swap(ptk, &LoopEnd[swrite][slwrite], sizeof(int), 1, in);
                    Read_Mod_Data(ptk, &LoopType[swrite][slwrite], sizeof(char), 1, in);

                    Read_Mod_Data_Swap(ptk, &SampleLength[swrite][slwrite], sizeof(int), 1, in);
                    Read_Mod_Data(ptk, &Finetune[swrite][slwrite], sizeof(char), 1, in);
                    Read_Mod_Data_Swap(ptk, &Sample_Amplify[swrite][slwrite], sizeof(float), 1, in);
                    Read_Mod_Data_Swap(ptk, &FDecay[swrite][slwrite], sizeof(float), 1, in);

                    AllocateWave(swrite, slwrite, SampleLength[swrite][slwrite], 1, FALSE, NULL, NULL);
                    Read_Mod_Data(ptk, RawSamples[swrite][0][slwrite], sizeof(short), SampleLength[swrite][slwrite], in);
                    Swap_Sample(ptk, RawSamples[swrite][0][slwrite], swrite, slwrite);
                    *RawSamples[swrite][0][slwrite] = 0;

                    // Stereo flag
                    Read_Mod_Data(ptk, &SampleChannels[swrite][slwrite], sizeof(char), 1, in);
                    if(SampleChannels[swrite][slwrite] == 2)
                    {
                        RawSamples[swrite][1][slwrite] = (short *) malloc(SampleLength[swrite][slwrite] * sizeof(short) + 8);
                        memset(RawSamples[swrite][1][slwrite], 0, SampleLength[swrite][slwrite] * sizeof(short) + 8);
                        Read_Mod_Data(ptk, RawSamples[swrite][1][slwrite], sizeof(short), SampleLength[swrite][slwrite], in);
                        Swap_Sample(ptk, RawSamples[swrite][1][slwrite], swrite, slwrite);
                        *RawSamples[swrite][1][slwrite] = 0;
                    }
                } // Exist Sample
            }
        }

        Status_Box(ptk, "Loading song -> Track info, patterns and sequence...");   

        Set_Default_Channels_Polyphony();

        // Reading Track Properties
        for(twrite = 0; twrite < Songtracks; twrite++)
        {
            Read_Mod_Data_Swap(ptk, &TCut[twrite], sizeof(float), 1, in);
            Read_Mod_Data_Swap(ptk, &ICut[twrite], sizeof(float), 1, in);
            if(ICut[twrite] > 0.0078125f) ICut[twrite] = 0.0078125f;
            if(ICut[twrite] < 0.00006103515625f) ICut[twrite] = 0.00006103515625f;
            Read_Mod_Data_Swap(ptk, &TPan[twrite], sizeof(float), 1, in);
            ComputeStereo(ptk, twrite);
            FixStereo(ptk, twrite);
            Read_Mod_Data_Swap(ptk, &FType[twrite], sizeof(int), 1, in);
            Read_Mod_Data_Swap(ptk, &FRez[twrite], sizeof(int), 1, in);
            Read_Mod_Data_Swap(ptk, &DThreshold[twrite], sizeof(float), 1, in);
            Read_Mod_Data_Swap(ptk, &DClamp[twrite], sizeof(float), 1, in);
            Read_Mod_Data_Swap(ptk, &DSend[twrite], sizeof(float), 1, in);
            Read_Mod_Data_Swap(ptk, &CSend[twrite], sizeof(int), 1, in);
            if(Poly) Read_Mod_Data(ptk, &Channels_Polyphony[twrite], sizeof(char), 1, in);
        }

        // Reading mod properties
        int cvalue;
        Read_Mod_Data_Swap(ptk, &cvalue, sizeof(int), 1, in);
        compressor = cvalue;
        Read_Mod_Data_Swap(ptk, &c_threshold, sizeof(int), 1, in);
        Read_Mod_Data_Swap(ptk, &BeatsPerMin, sizeof(int), 1, in);
        Read_Mod_Data_Swap(ptk, &TicksPerBeat, sizeof(int), 1, in);
        Read_Mod_Data_Swap(ptk, &mas_vol, sizeof(float), 1, in);
        if(mas_vol < 0.01f) mas_vol = 0.01f;
        if(mas_vol > 1.0f) mas_vol = 1.0f;

        if(New_Comp)
        {
            Comp_Flag = 0;
            Read_Mod_Data(ptk, &Comp_Flag, sizeof(char), 1, in);
            if(Comp_Flag)
            {
                Read_Mod_Data_Swap(ptk, &mas_comp_threshold_Master, sizeof(float), 1, in);
                if(mas_comp_threshold_Master < 0.01f) mas_comp_threshold_Master = 0.01f;
                if(mas_comp_threshold_Master > 100.0f) mas_comp_threshold_Master = 100.0f;

                Read_Mod_Data_Swap(ptk, &mas_comp_ratio_Master, sizeof(float), 1, in);
                if(mas_comp_ratio_Master < 0.01f) mas_comp_ratio_Master = 0.01f;
                if(mas_comp_ratio_Master > 100.0f) mas_comp_ratio_Master = 100.0f;
            }
            if(Compress_Tracks)
            {
                for(i = 0; i < MAX_TRACKS; i++)
                {
                    Read_Mod_Data_Swap(ptk, &mas_comp_threshold_Track[i], sizeof(float), 1, in);
                }

                for(i = 0; i < MAX_TRACKS; i++)
                {
                    Read_Mod_Data_Swap(ptk, &mas_comp_ratio_Track[i], sizeof(float), 1, in);
                }
                Read_Mod_Data(ptk, &Compress_Track, sizeof(char), MAX_TRACKS, in);
            }
        }

        if(!New_Reverb) Read_Mod_Data_Swap(ptk, &delay_time, sizeof(int), 1, in);
        Read_Mod_Data_Swap(ptk, &Feedback, sizeof(float), 1, in);
        if(!New_Reverb) Read_Mod_Data_Swap(ptk, &DelayType, sizeof(int), 1, in);
        Read_Mod_Data_Swap(ptk, &lchorus_delay, sizeof(int), 1, in);
        Read_Mod_Data_Swap(ptk, &rchorus_delay, sizeof(int), 1, in);
        Read_Mod_Data_Swap(ptk, &lchorus_feedback, sizeof(float), 1, in);
        Read_Mod_Data_Swap(ptk, &rchorus_feedback, sizeof(float), 1, in);
        Read_Mod_Data_Swap(ptk, &shuffle, sizeof(int), 1, in);

        // Load the new reverb data
        if(New_Reverb)
        {
            Load_Reverb_Data(ptk, Read_Mod_Data, Read_Mod_Data_Swap, in, !Reverb_Resonance);
        }

        // Reading track part sequence
        for(tps_pos = 0; tps_pos < 256; tps_pos++)
        {
            for(tps_trk = 0; tps_trk < MAX_TRACKS; tps_trk++)
            {
                Read_Mod_Data(ptk, &CHAN_ACTIVE_STATE[tps_pos][tps_trk], sizeof(char), 1, in);
                CHAN_HISTORY_STATE[tps_pos][tps_trk] = FALSE;
            }
        }

        for(int spl = 0; spl < Songtracks; spl++)
        {
            CCoef[spl] = float((float) CSend[spl] / 127.0f);
        }

        for(twrite = 0; twrite < Songtracks; twrite++)
        {
            Read_Mod_Data_Swap(ptk, &ptk->CHAN_MIDI_PRG[twrite], sizeof(int), 1, in);
        }

        for(twrite = 0; twrite < Songtracks; twrite++)
        {
            Read_Mod_Data(ptk, &LFO_ON[twrite], sizeof(char), 1, in);
            Read_Mod_Data_Swap(ptk, &LFO_RATE[twrite], sizeof(float), 1, in);
            Read_Mod_Data_Swap(ptk, &LFO_AMPL[twrite], sizeof(float), 1, in);
        }

        for(twrite = 0; twrite < Songtracks; twrite++)
        {
            Read_Mod_Data(ptk, &FLANGER_ON[twrite], sizeof(char), 1, in);
            Read_Mod_Data_Swap(ptk, &FLANGER_AMOUNT[twrite], sizeof(float), 1, in);
            Read_Mod_Data_Swap(ptk, &FLANGER_DEPHASE[twrite], sizeof(float), 1, in);
            Read_Mod_Data_Swap(ptk, &FLANGER_RATE[twrite], sizeof(float), 1, in);
            Read_Mod_Data_Swap(ptk, &FLANGER_AMPL[twrite], sizeof(float), 1, in);
            Read_Mod_Data_Swap(ptk, &FLANGER_FEEDBACK[twrite], sizeof(float), 1, in);
            Read_Mod_Data_Swap(ptk, &FLANGER_DELAY[twrite], sizeof(int), 1, in);
            FLANGER_OFFSET[twrite] = 8192;
            FLANGER_OFFSET2[twrite] = float(FLANGER_OFFSET[twrite] - FLANGER_DELAY[twrite]);
            FLANGER_OFFSET1[twrite] = float(FLANGER_OFFSET[twrite] - FLANGER_DELAY[twrite]);
        }

        if(!Flanger_Bug)
        {
            Read_Mod_Data_Swap(ptk, &FLANGER_DEPHASE, sizeof(float), 1, in);
        }

        for(tps_trk = 0; tps_trk < Songtracks; tps_trk++)
        {
            Read_Mod_Data_Swap(ptk, &CHAN_MUTE_STATE[tps_trk], sizeof(int), 1, in);
        }

        Read_Mod_Data(ptk, &Songtracks, sizeof(char), 1, in);

        for(tps_trk = 0; tps_trk < MAX_TRACKS; tps_trk++)
        {
            Read_Mod_Data(ptk, &Disclap[tps_trk], sizeof(char), 1, in);
            if(!Portable) Read_Mod_Data(ptk, &fake_value, sizeof(char), 1, in);
        }

        if(!Ntk_Beta)       // Nothing like that in ntk beta
        {
            Read_Mod_Data(ptk, ptk->artist, sizeof(char), 20, in);
            Read_Mod_Data(ptk, ptk->style, sizeof(char), 20, in);

            if(!Portable) Read_Mod_Data(ptk, &Ye_Old_Phony_Value, sizeof(char), 1, in);

            Read_Mod_Data(ptk, beatsync, sizeof(char), MAX_INSTRS, in);

            for(i = 0; i < MAX_INSTRS; i++)
            {
                Read_Mod_Data_Swap(ptk, &beatlines[i], sizeof(short), 1, in);
            }

            Read_Mod_Data_Swap(ptk, &Reverb_Filter_Cutoff, sizeof(float), 1, in);

            if(Reverb_Resonance)
            {
                Read_Mod_Data_Swap(ptk, &Reverb_Filter_Resonance, sizeof(float), 1, in);
            }
            if(Stereo_Reverb)
            {
                Read_Mod_Data(ptk, &Reverb_Stereo_Amount, sizeof(char), 1, in);
            }

            for(i = 0; i < MAX_INSTRS; i++)
            {
                Read_Mod_Data_Swap(ptk, &Sample_Vol[i], sizeof(float), 1, in);
            }

            if(!Portable) Read_Mod_Data(ptk, &Ye_Old_Phony_Value, sizeof(char), 1, in);

            // Read the 303 datas
            for(j = 0; j < 2; j++)
            {
                Read_Mod_Data(ptk, &tb303[j].enabled, sizeof(char), 1, in);
                Read_Mod_Data(ptk, &tb303[j].selectedpattern, sizeof(char), 1, in);
                Read_Mod_Data(ptk, &tb303[j].tune, sizeof(char), 1, in);
                Read_Mod_Data(ptk, &tb303[j].cutoff, sizeof(char), 1, in);
                Read_Mod_Data(ptk, &tb303[j].resonance, sizeof(char), 1, in);
                Read_Mod_Data(ptk, &tb303[j].envmod, sizeof(char), 1, in);
                Read_Mod_Data(ptk, &tb303[j].decay, sizeof(char), 1, in);
                Read_Mod_Data(ptk, &tb303[j].accent, sizeof(char), 1, in);
                Read_Mod_Data(ptk, &tb303[j].waveform, sizeof(char), 1, in);
                // Default value
                tb303[j].scale = 1;
                if(Tb303_Scaling)
                {
                    Read_Mod_Data(ptk, &tb303[j].scale, sizeof(char), 1, in);
                }
                tb303engine[j].tbCurMultiple = tb303[j].scale;
                if(Portable)
                {
                    for(i = 0; i < 32; i++)
                    {
                        Load_303_Data(ptk, Read_Mod_Data, Read_Mod_Data_Swap, in, j, i);
                    }
                }
                else
                {
                    Read_Mod_Data(ptk, &tb303[j].patternlength, sizeof(char), 32, in);
                    Read_Mod_Data(ptk, &tb303[j].tone, sizeof(char), 32 * 16, in);
                    Read_Mod_Data(ptk, &Ye_Old_Phony_Value, sizeof(char), 1, in);
                    Read_Mod_Data(ptk, &Ye_Old_Phony_Value, sizeof(char), 1, in);
                    Read_Mod_Data(ptk, &Ye_Old_Phony_Value, sizeof(char), 1, in);
                    for(k = 0; k < 32; k++)
                    {
                        for(i = 0; i < 16; i++)
                        {
                            Read_Mod_Data_Swap(ptk, &tb303[j].flag[k][i], sizeof(struct flag303), 1, in);
                        }
                    }
                    Read_Mod_Data(ptk, &tb303[j].pattern_name, sizeof(char), 32 * 20, in);
                }
            }
            Read_Mod_Data_Swap(ptk, &tb303engine[0].tbVolume, sizeof(float), 1, in);
            Read_Mod_Data_Swap(ptk, &tb303engine[1].tbVolume, sizeof(float), 1, in);
        }

        fclose(in);

        if(!New_Reverb)
        {
            // Set the reverb to one of the old presets
            Load_Old_Reverb_Presets(ptk, DelayType);
        }

        // Init the tracker context
        Init_Tracker_Context_After_ModLoad(ptk);

        Status_Box(ptk, "Module loaded sucessfully...");

    }
    else
    {

        Status_Box(ptk, "Module loading failed. (Possible cause: file not found)");
        return(FALSE);
    }

    Clear_Input(ptk);
    if(Mod_Memory) free(Mod_Memory);

    return(TRUE);
}

// ------------------------------------------------------
// Load and decode a packed sample
short *Unpack_Sample(ptk_data *ptk, FILE *FileHandle, int Dest_Length, char Pack_Type, int BitRate)
{
    int Packed_Length;

    short *Dest_Buffer;

    Uint8 *Packed_Read_Buffer;

    Read_Mod_Data(ptk, &Packed_Length, sizeof(int), 1, FileHandle);
    if(Packed_Length == -1)
    {
        // Sample wasn't packed
        Packed_Read_Buffer = (Uint8 *) malloc(Dest_Length * 2 + 8);
        memset(Packed_Read_Buffer, 0, Dest_Length * 2 + 8);
        Read_Mod_Data(ptk, Packed_Read_Buffer, sizeof(char), Dest_Length * 2, FileHandle);
        return((short *) Packed_Read_Buffer);
    }
    else
    {

        Packed_Read_Buffer = (Uint8 *) malloc(Packed_Length);
        // Read the packer buffer
        Read_Mod_Data(ptk, Packed_Read_Buffer, sizeof(char), Packed_Length, FileHandle);
        Dest_Buffer = (short *) malloc(Dest_Length * 2 + 8);
        memset(Dest_Buffer, 0, Dest_Length * 2 + 8);

        switch(Pack_Type)
        {
#if defined(__AT3_CODEC__)
            case SMP_PACK_AT3:
                UnpackAT3(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length, BitRate);
                break;
#endif
#if defined(__GSM_CODEC__)
            case SMP_PACK_GSM:
                UnpackGSM(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length);
                break;
#endif
#if defined(__MP3_CODEC__)
            case SMP_PACK_MP3:
                UnpackMP3(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length, BitRate);
                break;
#endif
#if defined(__TRUESPEECH_CODEC__)
            case SMP_PACK_TRUESPEECH:
                UnpackTrueSpeech(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length);
                break;
#endif
#if defined(__ADPCM_CODEC__)
            case SMP_PACK_ADPCM:
                UnpackADPCM(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length);
                break;
#endif
            case SMP_PACK_8BIT:
                Unpack8Bit(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length);
                break;

            case SMP_PACK_INTERNAL:
                UnpackInternal(Packed_Read_Buffer, Dest_Buffer, Packed_Length, Dest_Length);
                break;
        }
        free(Packed_Read_Buffer);
        return(Dest_Buffer);

    }
}

// ------------------------------------------------------
// Save a packed sample
void Pack_Sample(ptk_data *ptk, FILE *FileHandle, short *Sample, int Size, char Pack_Type, int BitRate)
{
    int PackedLen = 0;
    short *PackedSample = NULL;

#if defined(__ADPCM_CODEC__) || defined(__TRUESPEECH_CODEC__)
    short *AlignedSample;
    int Aligned_Size;
#endif

    switch(Pack_Type)
    {
#if defined(__AT3_CODEC__)
        case SMP_PACK_AT3:
            PackedSample = (short *) malloc(Size * 2 + 8);
            memset(PackedSample, 0, Size * 2 + 8);
            PackedLen = ToAT3(Sample, PackedSample, Size * 2, BitRate);
            break;
#endif
#if defined(__GSM_CODEC__)
        case SMP_PACK_GSM:
            PackedSample = (short *) malloc(Size * 2 + 8);
            memset(PackedSample, 0, Size * 2 + 8);
            PackedLen = ToGSM(Sample, PackedSample, Size * 2);
            break;
#endif
#if defined(__MP3_CODEC__)
        case SMP_PACK_MP3:
            PackedSample = (short *) malloc(Size * 2 + 8);
            memset(PackedSample, 0, Size * 2 + 8);
            PackedLen = ToMP3(Sample, PackedSample, Size * 2, BitRate);
            break;
#endif
#if defined(__TRUESPEECH_CODEC__)
        case SMP_PACK_TRUESPEECH:
            Aligned_Size = (Size * 2) + 0x400;
            AlignedSample = (short *) malloc(Aligned_Size + 8);
            if(AlignedSample)
            {
                memset(AlignedSample, 0, Aligned_Size + 8);
                memcpy(AlignedSample, Sample, Size * 2);
                // Size must be aligned
                PackedSample = (short *) malloc(Aligned_Size + 8);
                if(PackedSample)
                {
                    memset(PackedSample, 0, Aligned_Size + 8);
                    PackedLen = ToTrueSpeech(AlignedSample, PackedSample, Aligned_Size);
                }
                free(AlignedSample);
            }
            break;
#endif
#if defined(__ADPCM_CODEC__)
        case SMP_PACK_ADPCM:
            Aligned_Size = (Size * 2) + 0x1000;
            AlignedSample = (short *) malloc(Aligned_Size + 8);
            if(AlignedSample)
            {
                memset(AlignedSample, 0, Aligned_Size + 8);
                memcpy(AlignedSample, Sample, Size * 2);
                // Size must be aligned
                PackedSample = (short *) malloc(Aligned_Size + 8);
                if(PackedSample)
                {
                    memset(PackedSample, 0, Aligned_Size + 8);
                    PackedLen = ToADPCM(AlignedSample, PackedSample, Aligned_Size);
                }
                free(AlignedSample);
            }
            break;
#endif
        case SMP_PACK_8BIT:
            PackedSample = (short *) malloc(Size * 2 + 8);
            memset(PackedSample, 0, Size * 2 + 8);
            PackedLen = To8Bit(Sample, PackedSample, Size);
            break;

        case SMP_PACK_INTERNAL:
            PackedSample = (short *) malloc(Size * 2 + 8);
            memset(PackedSample, 0, Size * 2 + 8);
            PackedLen = ToInternal(Sample, PackedSample, Size);
            break;

        case SMP_PACK_NONE:
            PackedLen = 0;
            break;
    }
    if(PackedLen)
    {
        // Write the encoded length
        Write_Mod_Data(ptk, &PackedLen, sizeof(char), 4, FileHandle);
        // Write the encoded datas
        Write_Mod_Data(ptk, PackedSample, sizeof(char), PackedLen, FileHandle);
    }
    else
    {
        // Couldn't pack (too small or user do not want that to happen)
        PackedLen = -1;
        Write_Mod_Data(ptk, &PackedLen, sizeof(char), 4, FileHandle);
        Write_Mod_Data(ptk, Sample, sizeof(char), Size * 2, FileHandle);
    }
    if(PackedSample) free(PackedSample);
}

// ------------------------------------------------------
// Write data into a module file
int Write_Mod_Data(ptk_data *ptk, void *Datas, int Unit, int Length, FILE *Handle)
{
    switch(Mod_Simulate)
    {
        case SAVE_WRITE:
            Write_Data(ptk, Datas, Unit, Length, Handle);
            break;

        case SAVE_CALCLEN:
            Mod_Length += Unit * Length;
            break;

        case SAVE_WRITEMEM:
            memcpy(Mod_Memory + Mod_Mem_Pos, Datas, Unit * Length);
            Mod_Mem_Pos += Unit * Length;
            break;
    }
    return(0);
}

// ------------------------------------------------------
// Write data into a module file (handling bytes swapping)
int Write_Mod_Data_Swap(ptk_data *ptk, void *Datas, int Unit, int Length, FILE *Handle)
{
    short sswap_value;
    int iswap_value;
    short *svalue;
    int *ivalue;
    
    switch(Mod_Simulate)
    {
        case SAVE_WRITE:
            Write_Data_Swap(ptk, Datas, Unit, Length, Handle);
            break;

        case SAVE_CALCLEN:
            Mod_Length += Unit * Length;
            break;

        case SAVE_WRITEMEM:
            switch(Unit)
            {
                case 2:
                    svalue = (short *) Datas;
                    sswap_value = Swap_16(*svalue);
                    memcpy(Mod_Memory + Mod_Mem_Pos, &sswap_value, Unit * Length);
                    Mod_Mem_Pos += Unit * Length;
                    break;

                case 4:
                    ivalue = (int *) Datas;
                    iswap_value = Swap_32(*ivalue);
                    memcpy(Mod_Memory + Mod_Mem_Pos, &iswap_value, Unit * Length);
                    Mod_Mem_Pos += Unit * Length;
                    break;

                default:
                    printf("Invalid writing size.\n");
                    break;
            }
            break;
    }
    return(0);
}

// ------------------------------------------------------
// Read data from a module file
int Read_Mod_Data(ptk_data *ptk, void *Datas, int Unit, int Length, FILE *Handle)
{
    switch(Mod_Simulate)
    {
        case LOAD_READ:
            Read_Data(ptk, Datas, Unit, Length, Handle);
            break;

        case LOAD_READMEM:
            memcpy(Datas, Mod_Memory + Mod_Mem_Pos, Unit * Length);
            Mod_Mem_Pos += Unit * Length;
            break;
    }
    return(0);
}

// ------------------------------------------------------
// Read data from a module file
int Read_Mod_Data_Swap(ptk_data *ptk, void *Datas, int Unit, int Length, FILE *Handle)
{
    short svalue;
    int ivalue;

    switch(Mod_Simulate)
    {
        case LOAD_READ:
            Read_Data_Swap(ptk, Datas, Unit, Length, Handle);
            break;

        case LOAD_READMEM:
            switch(Unit)
            {
                case 2:
                    memcpy(&svalue, Mod_Memory + Mod_Mem_Pos, Unit * Length);
                    svalue = Swap_16(svalue);
                    *((short *) Datas) = (int) svalue;
                    Mod_Mem_Pos += Unit * Length;
                    break;

                case 4:
                    memcpy(&ivalue, Mod_Memory + Mod_Mem_Pos, Unit * Length);
                    ivalue = Swap_32(ivalue);
                    *((int *) Datas) = (int) ivalue;
                    Mod_Mem_Pos += Unit * Length;
                    break;

                default:
                    printf("Invalid reading size.\n");
                    break;
            }
            break;
    }
    return(0);
}

// ------------------------------------------------------
// Check if a file exists
int File_Exist(ptk_data *ptk, char *Format, char *Directory, char *FileName)
{
    FILE *in;
    char Temph[MAX_PATH];

    sprintf(Temph, Format, Directory, FileName);
    in = fopen(Temph, "rb");
    if(in)
    {
        fclose(in);
        return(TRUE);
    }
    return(FALSE);
}

// ------------------------------------------------------
// Check if a file exists and prepare a requester if it does
int File_Exist_Req(ptk_data *ptk, char *Format, char *Directory, char *FileName)
{
    char Temph[MAX_PATH];

    sprintf(Temph, Format, Directory, FileName);

    if(File_Exist(ptk, Format, Directory, FileName))
    {
        sprintf(OverWrite_Name, "File '%s' already exists, overwrite ?", Temph);
        Overwrite_Requester.Text = OverWrite_Name;
        return(TRUE);
    }
    return(FALSE);
}

// ------------------------------------------------------
// Return the size of an opened file
int Get_File_Size(ptk_data *ptk, FILE *Handle)
{
    int File_Size;
    int Current_Pos;

    Current_Pos = ftell(Handle);
    fseek(Handle, 0, SEEK_END);
    File_Size = ftell(Handle);
    fseek(Handle, Current_Pos, SEEK_SET);
    return(File_Size);
}

// ------------------------------------------------------
// Remvove the spaces chars located at the end of a string
void rtrim_string(ptk_data *ptk, char *string, int maxlen)
{
    int len = strlen(string);

    while(len < maxlen)
    {
        string[len] = 0;
        len++;
    }
}

// ------------------------------------------------------
// Entry point function for modules saving
int SavePtk(ptk_data *ptk, char *FileName, int NewFormat, int Simulate, Uint8 *Memory)
{
    FILE *in;
    int i;
    int j;
    int k;
    int l;
    char Temph[MAX_PATH];
    int Ok_Memory = TRUE;
    char Comp_Flag = TRUE;
    unsigned char *cur_pattern_col;
    unsigned char *cur_pattern;

    int twrite;
    int tps_trk;
    
    Mod_Length = 0;
    Mod_Mem_Pos = 0;
    Mod_Simulate = SAVE_WRITE;
    Mod_Memory = Memory;

    // Store the different possible modes
    if(Simulate)
    {
        Mod_Simulate = Simulate;
    }

    if(!Simulate)
    {
        if(NewFormat)
        {
            sprintf(Temph, "Saving '%s.ptp' song in modules directory...", FileName);
            Status_Box(ptk, Temph);
            sprintf(Temph, "%s"SLASH"%s.ptp", Dir_Mods, FileName);
        }
        else
        {
            sprintf(Temph, "Saving '%s.ptk' song in modules directory...", FileName);
            Status_Box(ptk, Temph);
            sprintf(Temph, "%s"SLASH"%s.ptk", Dir_Mods, FileName);
        }
        in = fopen(Temph, "wb");
    }
    else
    {
        in = (FILE *) -1;
    }

    if(in != NULL)
    {
        if(NewFormat)
        {
            // .ptp
            Ok_Memory = SavePtp(ptk, in, Simulate, FileName);
        }
        else
        {
            // .ptk
            if(strlen(FileName)) rtrim_string(ptk, FileName, 20);
            Write_Mod_Data(ptk, FileName, sizeof(char), 20, in);

            nPatterns = 0;

            int found_dat_in_patt;

            // Calc the real number of patterns
            for(j = 0; j < MAX_PATTERNS; j++)
            {
                cur_pattern_col = RawPatterns + (j * PATTERN_LEN);
                found_dat_in_patt = FALSE;
                // Next column
                for(i = 0; i < Songtracks; i++)
                {
                    cur_pattern = cur_pattern_col + (i * PATTERN_BYTES);
                    // Next pattern
                    for(k = 0; k < patternLines[j]; k++)
                    {
                        for(l = 0; l < MAX_POLYPHONY; l += 2)
                        {
                            if(cur_pattern[PATTERN_NOTE1 + l] != 121 ||
                               cur_pattern[PATTERN_INSTR1 + l] != 255)
                            {
                                found_dat_in_patt = TRUE;
                                break;
                            }
                        }
                        if(cur_pattern[PATTERN_VOLUME] != 255 ||
                           cur_pattern[PATTERN_PANNING] != 255 ||
                           cur_pattern[PATTERN_FX] != 0 ||
                           cur_pattern[PATTERN_FXDATA] != 0 ||
                           cur_pattern[PATTERN_FX2] != 0 ||
                           cur_pattern[PATTERN_FXDATA2] != 0 ||
                           cur_pattern[PATTERN_FX3] != 0 ||
                           cur_pattern[PATTERN_FXDATA3] != 0 ||
                           cur_pattern[PATTERN_FX4] != 0 ||
                           cur_pattern[PATTERN_FXDATA4] != 0)
                        {
                            // Was used
                            found_dat_in_patt = TRUE;
                            break;
                        }
                        // Next line
                        cur_pattern += PATTERN_ROW_LEN;
                        if(found_dat_in_patt) break;
                    }
                    if(found_dat_in_patt) break;
                }
                if(found_dat_in_patt)
                {
                    nPatterns = j + 1;
                }
            }

/*            for(i = 0 ; i < Song_Length; i++)
            {
                if((pSequence[i] + 1) > nPatterns)
                {
                    nPatterns = pSequence[i] + 1;
                }
            }
*/
            Write_Mod_Data(ptk, &nPatterns, sizeof(char), 1, in);
            Write_Mod_Data(ptk, &Song_Length, sizeof(char), 1, in);
            Write_Mod_Data(ptk, &Use_Cubic, sizeof(char), 1, in);

            Write_Mod_Data(ptk, pSequence, sizeof(char), MAX_SEQUENCES, in);

            Swap_Short_Buffer(ptk, patternLines, MAX_ROWS);
            Write_Mod_Data(ptk, patternLines, sizeof(short), MAX_ROWS, in);
            Swap_Short_Buffer(ptk, patternLines, MAX_ROWS);

            Write_Mod_Data(ptk, Channels_MultiNotes, sizeof(char), MAX_TRACKS, in);
            Write_Mod_Data(ptk, Channels_Effects, sizeof(char), MAX_TRACKS, in);
            for(i = 0; i < MAX_TRACKS; i++)
            {
                Write_Mod_Data_Swap(ptk, &Track_Volume[i], sizeof(float), 1, in);
            }
            for(i = 0; i < MAX_TRACKS; i++)
            {
                Write_Mod_Data_Swap(ptk, &EqDat[i].lg, sizeof(float), 1, in);
                Write_Mod_Data_Swap(ptk, &EqDat[i].mg, sizeof(float), 1, in);
                Write_Mod_Data_Swap(ptk, &EqDat[i].hg, sizeof(float), 1, in);
            }

            // Clean the unused patterns garbage (doesn't seem to do much)
            for(i = Songtracks; i < MAX_TRACKS; i++)
            {
                // Next column
                cur_pattern_col = RawPatterns + (i * PATTERN_BYTES);
                for(j = 0; j < nPatterns; j++)
                {
                    // Next pattern
                    cur_pattern = cur_pattern_col + (j * PATTERN_LEN);
                    for(k = 0; k < patternLines[j]; k++)
                    {
                        for(l = 0; l < MAX_POLYPHONY; l += 2)
                        {
                            cur_pattern[PATTERN_NOTE1 + l] = 121;
                            cur_pattern[PATTERN_INSTR1 + l] = 255;
                        }

                        cur_pattern[PATTERN_VOLUME] = 255;
                        cur_pattern[PATTERN_PANNING] = 255;
                        cur_pattern[PATTERN_FX] = 0;
                        cur_pattern[PATTERN_FXDATA] = 0;
                        cur_pattern[PATTERN_FX2] = 0;
                        cur_pattern[PATTERN_FXDATA2] = 0;
                        cur_pattern[PATTERN_FX3] = 0;
                        cur_pattern[PATTERN_FXDATA3] = 0;
                        cur_pattern[PATTERN_FX4] = 0;
                        cur_pattern[PATTERN_FXDATA4] = 0;
                        // Next line
                        cur_pattern += PATTERN_ROW_LEN;
                    }
                }
            }

            for(int pwrite = 0; pwrite < nPatterns; pwrite++)
            {
                Write_Mod_Data(ptk, RawPatterns + (pwrite * PATTERN_LEN),
                               sizeof(char), PATTERN_LEN, in);
            }

            // Writing sample data
            for(int swrite = 0; swrite < MAX_INSTRS; swrite++)
            {
                rtrim_string(ptk, nameins[swrite], 20);
                Write_Mod_Data(ptk, nameins[swrite], sizeof(char), 20, in);
                Write_Mod_Data(ptk, &Midiprg[swrite], sizeof(char), 1, in);
                Write_Mod_Data(ptk, &Synthprg[swrite], sizeof(char), 1, in);

                Write_Synth_Params(ptk, Write_Mod_Data, Write_Mod_Data_Swap, in, swrite);

                // Compression type
                Write_Mod_Data(ptk, &SampleCompression[swrite], sizeof(char), 1, in);
                switch(SampleCompression[swrite])
                {
                    case SMP_PACK_MP3:
                        Write_Mod_Data(ptk, &Mp3_BitRate[swrite], sizeof(char), 1, in);
                        break;

                    case SMP_PACK_AT3:
                        Write_Mod_Data(ptk, &At3_BitRate[swrite], sizeof(char), 1, in);
                        break;
                }

                // 16 splits / instrument
                for(int slwrite = 0; slwrite < MAX_INSTRS_SPLITS; slwrite++)
                {
                    Write_Mod_Data(ptk, &SampleType[swrite][slwrite], sizeof(char), 1, in);
                    if(SampleType[swrite][slwrite])
                    {
                        rtrim_string(ptk, SampleName[swrite][slwrite], 64);
                        Write_Mod_Data(ptk, SampleName[swrite][slwrite], sizeof(char), 64, in);
                        Write_Mod_Data(ptk, &Basenote[swrite][slwrite], sizeof(char), 1, in);
                        
                        Write_Mod_Data_Swap(ptk, &LoopStart[swrite][slwrite], sizeof(int), 1, in);
                        Write_Mod_Data_Swap(ptk, &LoopEnd[swrite][slwrite], sizeof(int), 1, in);
                        Write_Mod_Data(ptk, &LoopType[swrite][slwrite], sizeof(char), 1, in);
                        
                        Write_Mod_Data_Swap(ptk, &SampleLength[swrite][slwrite], sizeof(int), 1, in);
                        Write_Mod_Data(ptk, &Finetune[swrite][slwrite], sizeof(char), 1, in);
                        Write_Mod_Data_Swap(ptk, &Sample_Amplify[swrite][slwrite], sizeof(float), 1, in);
                        Write_Mod_Data_Swap(ptk, &FDecay[swrite][slwrite], sizeof(float), 1, in);

                        // All samples are converted into 16 bits
                        Write_Unpacked_Sample(ptk, Write_Mod_Data, in, swrite, slwrite);
                    }// Exist Sample
                }
            }

            // Writing Track Properties
            for(twrite = 0; twrite < MAX_TRACKS; twrite++)
            {
                Write_Mod_Data_Swap(ptk, &TCut[twrite], sizeof(float), 1, in);
                Write_Mod_Data_Swap(ptk, &ICut[twrite], sizeof(float), 1, in);

                Write_Mod_Data_Swap(ptk, &TPan[twrite], sizeof(float), 1, in);

                Write_Mod_Data_Swap(ptk, &FType[twrite], sizeof(int), 1, in);
                Write_Mod_Data_Swap(ptk, &FRez[twrite], sizeof(int), 1, in);
                Write_Mod_Data_Swap(ptk, &DThreshold[twrite], sizeof(float), 1, in);
                Write_Mod_Data_Swap(ptk, &DClamp[twrite], sizeof(float), 1, in);
                Write_Mod_Data_Swap(ptk, &DSend[twrite], sizeof(float), 1, in);
                Write_Mod_Data_Swap(ptk, &CSend[twrite], sizeof(int), 1, in);
                Write_Mod_Data(ptk, &Channels_Polyphony[twrite], sizeof(char), 1, in);
            }

            // Writing mod properties
            int cvalue;   
            cvalue = compressor;
            Write_Mod_Data_Swap(ptk, &cvalue, sizeof(int), 1, in);
            Write_Mod_Data_Swap(ptk, &c_threshold, sizeof(int), 1, in);
            Write_Mod_Data_Swap(ptk, &BeatsPerMin, sizeof(int), 1, in);
            Write_Mod_Data_Swap(ptk, &TicksPerBeat, sizeof(int), 1, in);
            Write_Mod_Data_Swap(ptk, &mas_vol, sizeof(float), 1, in);
            
            Write_Mod_Data(ptk, &Comp_Flag, sizeof(char), 1, in);
            Write_Mod_Data_Swap(ptk, &mas_comp_threshold_Master, sizeof(float), 1, in);
            Write_Mod_Data_Swap(ptk, &mas_comp_ratio_Master, sizeof(float), 1, in);

            for(i = 0; i < MAX_TRACKS; i++)
            {
                if(mas_comp_threshold_Track[i] < 0.01f) mas_comp_threshold_Track[i] = 0.01f;
                if(mas_comp_threshold_Track[i] > 100.0f) mas_comp_threshold_Track[i] = 100.0f;
                Write_Mod_Data_Swap(ptk, &mas_comp_threshold_Track[i], sizeof(float), 1, in);
            }
            for(i = 0; i < MAX_TRACKS; i++)
            {
                if(mas_comp_ratio_Track[i] < 0.01f) mas_comp_ratio_Track[i] = 0.01f;
                if(mas_comp_ratio_Track[i] > 100.0f) mas_comp_ratio_Track[i] = 100.0f;
                Write_Mod_Data_Swap(ptk, &mas_comp_ratio_Track[i], sizeof(float), 1, in);
            }
            Write_Mod_Data(ptk, &Compress_Track, sizeof(char), MAX_TRACKS, in);

            Write_Mod_Data_Swap(ptk, &Feedback, sizeof(float), 1, in);
            Write_Mod_Data_Swap(ptk, &lchorus_delay, sizeof(int), 1, in);
            Write_Mod_Data_Swap(ptk, &rchorus_delay, sizeof(int), 1, in);
            Write_Mod_Data_Swap(ptk, &lchorus_feedback, sizeof(float), 1, in);
            Write_Mod_Data_Swap(ptk, &rchorus_feedback, sizeof(float), 1, in);
            Write_Mod_Data_Swap(ptk, &shuffle, sizeof(int), 1, in);

            // Save the reverb data
            Save_Reverb_Data(ptk, Write_Mod_Data, Write_Mod_Data_Swap, in);

            // Writing part sequence data
            for(int tps_pos = 0; tps_pos < MAX_SEQUENCES; tps_pos++)
            {
                for(tps_trk = 0; tps_trk < MAX_TRACKS; tps_trk++)
                {
                    Write_Mod_Data(ptk, &CHAN_ACTIVE_STATE[tps_pos][tps_trk], sizeof(char), 1, in);
                }
            }

            for(twrite = 0; twrite < MAX_TRACKS; twrite++)
            {
                Write_Mod_Data_Swap(ptk, &ptk->CHAN_MIDI_PRG[twrite], sizeof(int), 1, in);
            }

            for(twrite = 0; twrite < MAX_TRACKS; twrite++)
            {
                Write_Mod_Data(ptk, &LFO_ON[twrite], sizeof(char), 1, in);
                Write_Mod_Data_Swap(ptk, &LFO_RATE[twrite], sizeof(float), 1, in);
                Write_Mod_Data_Swap(ptk, &LFO_AMPL[twrite], sizeof(float), 1, in);
            }

            for(twrite = 0; twrite < MAX_TRACKS; twrite++)
            {
                Write_Mod_Data(ptk, &FLANGER_ON[twrite], sizeof(char), 1, in);
                Write_Mod_Data_Swap(ptk, &FLANGER_AMOUNT[twrite], sizeof(float), 1, in);
                Write_Mod_Data_Swap(ptk, &FLANGER_DEPHASE[twrite], sizeof(float), 1, in);
                Write_Mod_Data_Swap(ptk, &FLANGER_RATE[twrite], sizeof(float), 1, in);
                Write_Mod_Data_Swap(ptk, &FLANGER_AMPL[twrite], sizeof(float), 1, in);
                Write_Mod_Data_Swap(ptk, &FLANGER_FEEDBACK[twrite], sizeof(float), 1, in);
                Write_Mod_Data_Swap(ptk, &FLANGER_DELAY[twrite], sizeof(int), 1, in);
            }

            // Was a bug
            //Write_Mod_Data_Swap(ptk, &FLANGER_DEPHASE, sizeof(float), 1, in);

            for(tps_trk = 0; tps_trk < MAX_TRACKS; tps_trk++)
            {
                Write_Mod_Data_Swap(ptk, &CHAN_MUTE_STATE[tps_trk], sizeof(int), 1, in);
            }
            Write_Mod_Data(ptk, &Songtracks, sizeof(char), 1, in);

            for(tps_trk = 0; tps_trk < MAX_TRACKS; tps_trk++)
            {
                Write_Mod_Data(ptk, &Disclap[tps_trk], sizeof(char), 1, in);
            }

            rtrim_string(ptk, ptk->artist, 20);
            Write_Mod_Data(ptk, ptk->artist, sizeof(char), 20, in);
            rtrim_string(ptk, ptk->style, 20);
            Write_Mod_Data(ptk, ptk->style, sizeof(char), 20, in);

            Write_Mod_Data(ptk, beatsync, sizeof(char), 128, in);

            for(i = 0; i < 128; i++)
            {
                Write_Mod_Data_Swap(ptk, &beatlines[i], sizeof(short), 1, in);
            }
            Write_Mod_Data_Swap(ptk, &Reverb_Filter_Cutoff, sizeof(float), 1, in);
            Write_Mod_Data_Swap(ptk, &Reverb_Filter_Resonance, sizeof(float), 1, in);
            Write_Mod_Data(ptk, &Reverb_Stereo_Amount, sizeof(char), 1, in);

            for(i = 0; i < 128; i++)
            {
                Write_Mod_Data_Swap(ptk, &Sample_Vol[i], sizeof(float), 1, in);
            }

            // Include the patterns names
            for(i = 0; i < 32; i++)
            {
                rtrim_string(ptk, tb303[0].pattern_name[i], 20);
                rtrim_string(ptk, tb303[1].pattern_name[i], 20);
            }
            
            for(j = 0; j < 2; j++)
            {
                Write_Mod_Data(ptk, &tb303[j].enabled, sizeof(char), 1, in);
                Write_Mod_Data(ptk, &tb303[j].selectedpattern, sizeof(char), 1, in);
                Write_Mod_Data(ptk, &tb303[j].tune, sizeof(char), 1, in);
                Write_Mod_Data(ptk, &tb303[j].cutoff, sizeof(char), 1, in);
                Write_Mod_Data(ptk, &tb303[j].resonance, sizeof(char), 1, in);
                Write_Mod_Data(ptk, &tb303[j].envmod, sizeof(char), 1, in);
                Write_Mod_Data(ptk, &tb303[j].decay, sizeof(char), 1, in);
                Write_Mod_Data(ptk, &tb303[j].accent, sizeof(char), 1, in);
                Write_Mod_Data(ptk, &tb303[j].waveform, sizeof(char), 1, in);
                Write_Mod_Data(ptk, &tb303[j].scale, sizeof(char), 1, in);

                for(i = 0; i < 32; i++)
                {
                    Save_303_Data(ptk, Write_Mod_Data, Write_Mod_Data_Swap, in, j, i); 
                }
            }

            Write_Mod_Data_Swap(ptk, &tb303engine[0].tbVolume, sizeof(float), 1, in);
            Write_Mod_Data_Swap(ptk, &tb303engine[1].tbVolume, sizeof(float), 1, in);
        }

        if(!Simulate)
        {
            fclose(in);
            ptk->last_index = -1;
            Read_SMPT(ptk);
            Actualize_Files_List(ptk, 0);

            if(Ok_Memory)
            {
                char name[128];
                if(NewFormat)
                {
                    sprintf(name, "Module '%s.ptp' saved succesfully.", FileName);
                }
                else
                {
                    sprintf(name, "Module '%s.ptk' saved succesfully.", FileName);
                }
                Status_Box(ptk, name);
            }
        }
        if(!Ok_Memory) Status_Box(ptk, "Not enough memory.");
    }
    else
    {
        if(!Simulate) Status_Box(ptk, "Module save failed.");   
    }

    return(Mod_Length);
}

// ------------------------------------------------------
// Pack a module
Uint8 *Pack_Data(ptk_data *ptk, Uint8 *Memory, int *Size)
{
    z_stream c_stream; // compression stream
    int Comp_Len = -1;
    Uint8 *Final_Mem_Out = (Uint8 *) malloc(*Size);

    memset(&c_stream, 0, sizeof(c_stream));
    deflateInit(&c_stream, Z_BEST_COMPRESSION);
    c_stream.next_in = (Bytef *) Memory;
    c_stream.next_out = Final_Mem_Out;
    while (c_stream.total_in != *Size)
    {
        c_stream.avail_in = 1;
        c_stream.avail_out = 1;
        deflate(&c_stream, Z_NO_FLUSH);
    }
    for (;;)
    {
        c_stream.avail_out = 1;
        if (deflate(&c_stream, Z_FINISH) == Z_STREAM_END) break;
    }
    deflateEnd(&c_stream);
    *Size = c_stream.total_out;
    return(Final_Mem_Out);
}

// ------------------------------------------------------
// Depack a compressed module
Uint8 *Depack_Data(ptk_data *ptk, Uint8 *Memory, int Sizen, int Size_Out)
{
    z_stream d_stream;

    Uint8 *Test_Mem = (Uint8 *) malloc(Size_Out);
    if(Test_Mem)
    {
        memset(&d_stream, 0, sizeof(d_stream));
        d_stream.next_in = (Uint8 *) Memory;
        d_stream.next_out = Test_Mem;
        inflateInit(&d_stream);
        while (d_stream.total_out < Size_Out &&
               d_stream.total_in < Sizen)
        {
            d_stream.avail_in = 1;
            d_stream.avail_out = 1;
            if (inflate(&d_stream, Z_NO_FLUSH) == Z_STREAM_END) break;
        }
        inflateEnd(&d_stream);
    }
    return(Test_Mem);
}

// ------------------------------------------------------
// Backup a module
void Backup_Module(ptk_data *ptk, char *FileName)
{
    FILE *In;
    FILE *Out;
    int backup_size;
    unsigned char *backup_mem;
    time_t rawtime;
    struct tm *timeinfo;
    char backup_savename[MAX_PATH];

    sprintf(backup_savename, "%s"SLASH"%s.ptk", Dir_Mods, FileName);

    In = fopen(backup_savename, "rb");
    if(In)
    {
        // If the module already exists then load it
        // and make a backup before saving the new version
        backup_size = Get_File_Size(ptk, In);
        backup_mem = (unsigned char *) malloc(backup_size);
        if(backup_mem)
        {
           time(&rawtime);
           timeinfo = localtime(&rawtime);
           
            fread(backup_mem, 1, backup_size, In);
            sprintf(backup_savename,
                    "%s"SLASH"%s_%.2d%.2d%.2d.ptk",
                    Dir_Mods,
                    ptk->name,
                    timeinfo->tm_hour,
                    timeinfo->tm_min,
                    timeinfo->tm_sec);
            Out = fopen(backup_savename, "wb");
            if(Out)
            {
                fwrite(backup_mem, 1, backup_size, Out);
                fclose(Out);
            }
            free(backup_mem);
        }
        fclose(In);
    }
}

// ------------------------------------------------------
// Pack & save .ptk module
int Pack_Module(ptk_data *ptk, char *FileName)
{
    FILE *output;
    char name[128];
    char extension[10];
    char Temph[MAX_PATH];
    Uint8 *Final_Mem_Out;
    int Depack_Size;

    // Reset autosave counter
    ptk->wait_AutoSave = 0;
    
    if(!strlen(FileName))
    {
        sprintf(name, "Can't save module without a name...");
        Status_Box(ptk, name);
        return(FALSE);
    }

    // Backup the old version of the module
    if(AutoBackup) Backup_Module(ptk, FileName);

    // Save the new one
    sprintf(Temph, "%s"SLASH"%s.ptk", Dir_Mods, FileName);

    int Len = SavePtk(ptk, "", FALSE, SAVE_CALCLEN, NULL);

    Depack_Size = Len;

    Uint8 *Final_Mem = (Uint8 *) malloc(Len);
    SavePtk(ptk, FileName, FALSE, SAVE_WRITEMEM, Final_Mem);

    Final_Mem_Out = Pack_Data(ptk, Final_Mem, &Len);

    output = fopen(Temph, "wb");
    if(output)
    {
        sprintf(extension, "PROTREKO");
        Write_Data(ptk, extension, sizeof(char), 9, output);
        Write_Data_Swap(ptk, &Depack_Size, sizeof(int), 1, output);
        Write_Data(ptk, Final_Mem_Out, sizeof(char), Len, output);
        fclose(output);
        sprintf(name, "Module '%s.ptk' saved succesfully.", FileName);
    }
    else
    {
        sprintf(name, "Module save failed.");
    }
    if(Final_Mem_Out) free(Final_Mem_Out);
    if(Final_Mem) free(Final_Mem);

    Clear_Input(ptk);
    Status_Box(ptk, name);
    Read_SMPT(ptk);
    ptk->last_index = -1;
    Actualize_Files_List(ptk, 0);
    return(FALSE);
}

// ------------------------------------------------------
// Return the length of a compressed module
int TestMod(ptk_data *ptk)
{
    Uint8 *Final_Mem_Out;
    int Len = SavePtk(ptk, "", TRUE, SAVE_CALCLEN, NULL);

    Uint8 *Final_Mem = (Uint8 *) malloc(Len);
    SavePtk(ptk, "", TRUE, SAVE_WRITEMEM, Final_Mem);

    Final_Mem_Out = Pack_Data(ptk, Final_Mem, &Len);

    if(Final_Mem_Out) free(Final_Mem_Out);
    if(Final_Mem) free(Final_Mem);
    return(Len);
}

// ------------------------------------------------------
// Switch the endianness of a 16 bit buffer
// (size is the number of short elements, not bytes)
void Swap_Short_Buffer(ptk_data *ptk, short *buffer, int size)
{
#if defined(__BIG_ENDIAN__)
    int i;

    for(i = 0; i < size; i++)
    {
        buffer[i] = Swap_16(buffer[i]);
    }
#endif
}

// ------------------------------------------------------
// Switch the endianness of a sample
void Swap_Sample(ptk_data *ptk, short *buffer, int sample, int bank)
{
#if defined(__BIG_ENDIAN__)
    Swap_Short_Buffer(ptk, buffer, SampleLength[sample][bank]);
#endif
}

// ------------------------------------------------------
// Create a new buffer and switch the endianness of a sample
short *Swap_New_Sample(ptk_data *ptk, short *buffer, int sample, int bank)
{
#if defined(__BIG_ENDIAN__)
    short *new_buffer = (short *) malloc(SampleLength[sample][bank] * sizeof(short) + 8);
    memset(new_buffer, 0, SampleLength[sample][bank] * sizeof(short) + 8);
    memcpy(new_buffer, buffer, SampleLength[sample][bank] * sizeof(short));
    Swap_Sample(ptk, new_buffer, sample, bank);
    return(new_buffer);
#else
    return(NULL);
#endif
}

// ------------------------------------------------------
// Save a given unpacked sample
void Write_Unpacked_Sample(ptk_data *ptk, int (*Write_Function)(ptk_data *, void *, int ,int, FILE *),
                           FILE *in, int sample, int bank)
{
    short *swap_buffer;

    swap_buffer = Swap_New_Sample(ptk, Get_WaveForm(ptk, sample, 0, bank), sample, bank);
    if(swap_buffer)
    {
        Write_Function(ptk, swap_buffer, sizeof(short), SampleLength[sample][bank], in);
        free(swap_buffer);
    }
    else
    {
        Write_Function(ptk, Get_WaveForm(ptk, sample, 0, bank), sizeof(short), SampleLength[sample][bank], in);
    }

    Write_Function(ptk, &SampleChannels[sample][bank], sizeof(char), 1, in);
    if(SampleChannels[sample][bank] == 2)
    {
        swap_buffer = Swap_New_Sample(ptk, Get_WaveForm(ptk, sample, 1, bank), sample, bank);
        if(swap_buffer)
        {
            Write_Function(ptk, swap_buffer, sizeof(short), SampleLength[sample][bank], in);
            free(swap_buffer);
        }
        else
        {
            Write_Function(ptk, Get_WaveForm(ptk, sample, 1, bank), sizeof(short), SampleLength[sample][bank], in);
        }
    }
}

// ------------------------------------------------------
// Write data into a file
int Write_Data(ptk_data *ptk, void *value, int size, int amount, FILE *handle)
{
    return(fwrite(value, size, amount, handle));
}

// ------------------------------------------------------
// Write data into a file taking care of the endianness
int Write_Data_Swap(ptk_data *ptk, void *value, int size, int amount, FILE *handle)
{
    short sswap_value;
    int iswap_value;
    short *svalue;
    int *ivalue;

    switch(size)
    {
        case 2:
            svalue = (short *) value;
            sswap_value = Swap_16(*svalue);
            return(Write_Data(ptk, &sswap_value, size, amount, handle));

        case 4:
            ivalue = (int *) value;
            iswap_value = Swap_32(*ivalue);
            return(Write_Data(ptk, &iswap_value, size, amount, handle));

        default:
            printf("Invalid writing size.\n");
            break;
    }
    return(TRUE);
}

// ------------------------------------------------------
// Read data from a file
int Read_Data(ptk_data *ptk, void *value, int size, int amount, FILE *handle)
{
    return(fread(value, size, amount, handle));
}

// ------------------------------------------------------
// Read data from a file taking care of the endianness
int Read_Data_Swap(ptk_data *ptk, void *value, int size, int amount, FILE *handle)
{
    short svalue;
    int ivalue;
    int ret_value;

    switch(size)
    {
        case 2:
            ret_value = Read_Data(ptk, &svalue, size, amount, handle);
            svalue = Swap_16(svalue);
            *((short *) value) = (int) svalue;
            return(ret_value);

        case 4:
            ret_value = Read_Data(ptk, &ivalue, size, amount, handle);
            ivalue = Swap_32(ivalue);
            *((int *) value) = (int) ivalue;
            return(ret_value);

        default:
            printf("Invalid reading size.\n");
            break;
    }
    return(0);
}

// -------------------------------------
// Calculate the length of the song in hours:minutes:seconds
int song_Seconds;
int song_Minutes;
int song_Hours;

int Calc_Length(ptk_data *ptk)
{
    int i;
    int k;
    int l;
    int pos_patt;
    int patt_cmd[MAX_FX];
    int patt_datas[MAX_FX];
    Uint8 *Cur_Patt;
    float Ticks = (float) TicksPerBeat;
    float BPM = (float) BeatsPerMin;
    int rep_pos = 0;
    int rep_counter = -1;
    int have_break = 255;
    int PosTicks;
    int shuffle_switch;
    int shuffle_stp = shuffle;
    double len;
    int nbr_ticks;
    int Samples;
    int ilen;
    int early_exit = FALSE;
    int already_in_loop = FALSE;

    shuffle_switch = -1;
    Samples = (int) ((60 * MIX_RATE) / (BeatsPerMin * TicksPerBeat));
    if(shuffle_switch == 1) shuffle_stp = -((Samples * shuffle) / 200);
    else shuffle_stp = (Samples * shuffle) / 200;

    PosTicks = 0;
    nbr_ticks = 0;
    len = 0;
    i = 0;
    while(i < Song_Length)
    {
        if(have_break < MAX_ROWS) pos_patt = have_break;
        else pos_patt = 0;
        have_break = 255;
        while(pos_patt < patternLines[pSequence[i]])
        {
            Cur_Patt = RawPatterns + (pSequence[i] * PATTERN_LEN) + (pos_patt * PATTERN_ROW_LEN);
            if(!PosTicks)
            {
                for(k = 0; k < Songtracks; k++)
                {
                    // Check if there's a pattern loop command
                    // or a change in the tempo/ticks
                    for(l = 0; l < Channels_Effects[k]; l++)
                    {
                        patt_cmd[l] = Cur_Patt[PATTERN_FX + (l * 2)];
                        patt_datas[l] = Cur_Patt[PATTERN_FXDATA + (l * 2)];
                    }                    

                    for(l = 0; l < Channels_Effects[k]; l++)
                    {
                        switch(patt_cmd[l])
                        {
                            case 0x6:
                                if(!already_in_loop)
                                {
                                    if(!patt_datas[l])
                                    {
                                        rep_counter = -1;
                                        rep_pos = pos_patt;
                                        already_in_loop = TRUE;
                                    }
                                    else
                                    {
                                        if(rep_counter == -1)
                                        {
                                            rep_counter = (int) patt_datas[l];
                                            pos_patt = rep_pos;
                                        }
                                        else
                                        {
                                            // count
                                            rep_counter--;
                                            if(rep_counter)
                                            {
                                                pos_patt = rep_pos;
                                            }
                                            else
                                            {
                                                rep_counter = -1;
                                                rep_pos = 0;
                                                already_in_loop = FALSE;
                                            }
                                        }
                                    }
                                }
                                break;

                            case 0xd:
                                if(patt_datas[l] < MAX_ROWS) have_break = patt_datas[l];
                                break;
                        
                            case 0x1f:
                                // Avoid looping the song when jumping
                                if(i == (Song_Length - 1) || patt_datas[l] <= i)
                                {
                                    early_exit = TRUE;
                                }
                                i = patt_datas[l];
                                // Was there a break already ?
                                if(have_break >= MAX_ROWS) have_break = 0;
                                break;
                        
                            case 0xf:
                                Ticks = (float) patt_datas[l];
                                break;
    
                            case 0xf0:
                                BPM = (float) patt_datas[l];
                                break;
                        }
                    }
                    Cur_Patt += PATTERN_BYTES;
                }
            }
            Samples = (int) ((60 * MIX_RATE) / (BPM * Ticks));

            PosTicks++;
            if(PosTicks > Samples + shuffle_stp)
            {
                shuffle_switch = -shuffle_switch;

                if(shuffle_switch == 1)
                {
                    shuffle_stp = -((Samples * shuffle) / 200);
                }
                else
                {
                    shuffle_stp = (Samples * shuffle) / 200;
                }
                len += PosTicks - 1;

                nbr_ticks++;
                PosTicks = 0;
                if(have_break > 127)
                {
                    pos_patt++;
                }
                else
                {
                    // End the pattern here
                    pos_patt = patternLines[pSequence[i]];
                    rep_counter = -1;
                    already_in_loop = FALSE;
                    rep_pos = 0;
                }
            }
        }
        if(early_exit) break;
        i++;
    }
    len /= MIX_RATE;

    ilen = (int) len;

    song_Seconds = (int) ilen;
    song_Seconds %= 60;
    song_Minutes = (ilen / 60);
    song_Hours = ilen / 60 / 24;

    Display_Song_Length(ptk);

    return((int) (len * 1000));
}

void Reset_Song_Length(ptk_data *ptk)
{
    song_Hours = 0;
    song_Minutes = 0;
    song_Seconds = 0;
}

// ------------------------------------------------------
// Return the data of an unpacked sample
short *Get_WaveForm(ptk_data *ptk, int Instr_Nbr, int Channel, int Split)
{
    if(SamplesSwap[Instr_Nbr])
    {
        return(RawSamples_Swap[Instr_Nbr][Channel][Split]); 
    }
    else
    {
        return(RawSamples[Instr_Nbr][Channel][Split]); 
    }
}

// ------------------------------------------------------
// Return the number of splits used in an instrument
int Get_Number_Of_Splits(int n_index)
{
    int i;

    // If this is the first waveform of the instrument
    // we set the global volume to max
    int nbr_splits = 0;
    for(i = 0; i < MAX_INSTRS_SPLITS; i++)
    {
        if(RawSamples[n_index][0][i])
        {
            nbr_splits++;
        }
    }
    return nbr_splits;
}

// ------------------------------------------------------
// Clear the data of a given instrument
void Clear_Instrument_Dat(int n_index, int split, int lenfir)
{
    SampleType[n_index][split] = 0;
    LoopStart[n_index][split] = 0;
    LoopEnd[n_index][split] = lenfir;
    LoopType[n_index][split] = SMP_LOOP_NONE;
    SampleLength[n_index][split] = lenfir;
    Finetune[n_index][split] = 0;
    Sample_Amplify[n_index][split] = 1.0f;
    FDecay[n_index][split] = 0.0f;
    Basenote[n_index][split] = DEFAULT_BASE_NOTE;

    if(Get_Number_Of_Splits(n_index) == 0)
    {
        Sample_Vol[n_index] = 0.0f;
        Midiprg[n_index] = -1;
        Synthprg[n_index] = SYNTH_WAVE_OFF;
        beatsync[n_index] = FALSE;

        // Internal is default compression
        SampleCompression[n_index] = SMP_PACK_INTERNAL;
        SamplesSwap[n_index] = FALSE;
        Mp3_BitRate[n_index] = 0;
        At3_BitRate[n_index] = 0;
    }
}

// ------------------------------------------------------
// Allocate space for a waveform
void AllocateWave(int n_index, int split, long lenfir,
                  int samplechans, int clear,
                  short *Waveform1, short *Waveform2)
{
    // Freeing if memory was allocated before -----------------------
    if(SampleType[n_index][split] != 0)
    {
        if(RawSamples[n_index][0][split]) free(RawSamples[n_index][0][split]);
        RawSamples[n_index][0][split] = NULL;
        if(SampleChannels[n_index][split] == 2)
        {
            if(RawSamples[n_index][1][split]) free(RawSamples[n_index][1][split]);
            RawSamples[n_index][1][split] = NULL;
        }

        if(RawSamples_Swap[n_index][0][split]) free(RawSamples_Swap[n_index][0][split]);
        RawSamples_Swap[n_index][0][split] = NULL;
        if(SampleChannels[n_index][split] == 2)
        {
            if(RawSamples_Swap[n_index][1][split]) free(RawSamples_Swap[n_index][1][split]);
            RawSamples_Swap[n_index][1][split] = NULL;
        }

    }

    if(clear) Clear_Instrument_Dat(n_index, split, lenfir);

    SampleType[n_index][split] = 1;

    SampleChannels[n_index][split] = samplechans;
    // Was it already supplied ?
    if(Waveform1)
    {
        RawSamples[n_index][0][split] = Waveform1;
    }
    else
    {
        RawSamples[n_index][0][split] = (short *) malloc((lenfir * 2) + 8);
        memset(RawSamples[n_index][0][split], 0, (lenfir * 2) + 8);
    }
    if(samplechans == 2)
    {
        if(Waveform2)
        {
            RawSamples[n_index][1][split] = Waveform2;
        }
        else
        {
            RawSamples[n_index][1][split] = (short *) malloc((lenfir * 2) + 8);
            memset(RawSamples[n_index][1][split], 0, (lenfir * 2) + 8);
        }
    }
}

// ------------------------------------------------------
// Clear any pending input
void Clear_Input(ptk_data *ptk)
{
    if(ptk->snamesel == INPUT_303_PATTERN)
    {
        ptk->snamesel = INPUT_NONE;
        Actualize_303_Ed(ptk, 0);
    }

    if(ptk->snamesel == INPUT_SYNTH_NAME)
    {
        ptk->snamesel = INPUT_NONE;
        Actualize_Synth_Ed(ptk, 0);
    }

    if(ptk->snamesel == INPUT_MODULE_NAME ||
       ptk->snamesel == INPUT_MODULE_ARTIST ||
       ptk->snamesel == INPUT_MODULE_STYLE)
    {
        ptk->snamesel = INPUT_NONE;
        Actualize_DiskIO_Ed(ptk, 0);
    }

    if(ptk->snamesel == INPUT_INSTRUMENT_NAME)
    {
        ptk->snamesel = INPUT_NONE;
        Actualize_Patterned(ptk);
    }

    if(ptk->snamesel == INPUT_REVERB_NAME)
    {
        ptk->snamesel = INPUT_NONE;
        Actualize_Reverb_Ed(ptk, 0);
    }

    if(ptk->snamesel == INPUT_MIDI_NAME)
    {
        ptk->snamesel = INPUT_NONE;
        Actualize_Midi_Ed(ptk, 0);
    }

    if(ptk->snamesel == INPUT_SELECTION_NAME)
    {
        ptk->snamesel = INPUT_NONE;
        Actualize_Seq_Ed(ptk, 0);
    }
}

// ------------------------------------------------------
// Make sure an instrument isn't using a non existant codec
int Fix_Codec(ptk_data *, int Scheme)
{
#if !defined(__AT3_CODEC__)
    if(Scheme == SMP_PACK_AT3) return SMP_PACK_INTERNAL;
#endif

#if !defined(__MP3_CODEC__)
    if(Scheme == SMP_PACK_MP3) return SMP_PACK_INTERNAL;
#endif

#if !defined(__ADPCM_CODEC__)
    if(Scheme == SMP_PACK_ADPCM) return SMP_PACK_INTERNAL;
#endif

#if !defined(__TRUESPEECH_CODEC__)
    if(Scheme == SMP_PACK_TRUESPEECH) return SMP_PACK_INTERNAL;
#endif

#if !defined(__GSM_CODEC__)
    if(Scheme == SMP_PACK_GSM) return SMP_PACK_INTERNAL;
#endif

    return(Scheme);
}
