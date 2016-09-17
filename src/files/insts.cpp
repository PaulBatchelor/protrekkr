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
#include "insts.h"
#include "editor_synth.h"
#include "editor_instrument.h"

// ------------------------------------------------------
// Load an instrument
void LoadInst(ptk_data *ptk, char *FileName)
{
    int old_bug = FALSE;
    int Pack_Scheme = FALSE;
    int Mp3_Scheme = FALSE;
    int new_adsr = FALSE;
    int tight = FALSE;
    int Env_Modulation = FALSE;
    int New_Env = FALSE;
    int Glob_Vol = FALSE;
    int Combine = FALSE;

    Status_Box(ptk, "Attempting to load an instrument file...");
    FILE *in;
    in = fopen(FileName, "rb");

    if(in != NULL)
    {
        // Reading and checking extension...
        char extension[10];
        Read_Data(ptk, extension, sizeof(char), 9, in);

        switch(extension[7])
        {
            case '9':
                Combine = TRUE;
            case '8':
                Glob_Vol = TRUE;
            case '7':
                New_Env = TRUE;
            case '6':
                Env_Modulation = TRUE;
            case '5':
                Mp3_Scheme = TRUE;
            case '4':
                tight = TRUE;
            case '3':
                new_adsr = TRUE;
            case '2':
                Pack_Scheme = TRUE;
            case '1':
                break;
            case '0':
                old_bug = TRUE;
                break;
        }

        KillInst(ptk, ptk->Current_Instrument, TRUE);
        Status_Box(ptk, "Loading Instrument -> Header..."); 
        Read_Data(ptk, &nameins[ptk->Current_Instrument], sizeof(char), 20, in);

        // Reading sample data
        Status_Box(ptk, "Loading Instrument -> Sample data...");

        int swrite = ptk->Current_Instrument;

        Read_Data(ptk, &Midiprg[swrite], sizeof(char), 1, in);
        Read_Data(ptk, &Synthprg[swrite], sizeof(char), 1, in);

        PARASynth[swrite].disto = 0;

        PARASynth[swrite].lfo1_attack = 0;
        PARASynth[swrite].lfo1_decay = 0;
        PARASynth[swrite].lfo1_sustain = 128;
        PARASynth[swrite].lfo1_release = 0x10000;

        PARASynth[swrite].lfo2_attack = 0;
        PARASynth[swrite].lfo2_decay = 0;
        PARASynth[swrite].lfo2_sustain = 128;
        PARASynth[swrite].osc_combine = COMBINE_ADD;

        PARASynth[swrite].lfo2_release = 0x10000;

        Read_Synth_Params(ptk, Read_Data, Read_Data_Swap, in, swrite,
                          !old_bug, new_adsr, tight,
                          Env_Modulation, New_Env, FALSE, Combine);

        // Gsm by default
        if(Pack_Scheme)
        {
            Read_Data(ptk, &SampleCompression[swrite], sizeof(char), 1, in);
        }
        else
        {
            SampleCompression[swrite] = SMP_PACK_INTERNAL;
        }
        // Load the bitrate
        if(Mp3_Scheme)
        {
            switch(SampleCompression[swrite])
            {
                case SMP_PACK_MP3:
                    Read_Data(ptk, &Mp3_BitRate[swrite], sizeof(char), 1, in);
                    break;

                case SMP_PACK_AT3:
                    Read_Data(ptk, &At3_BitRate[swrite], sizeof(char), 1, in);
                    break;
            }
        }
        SampleCompression[swrite] = Fix_Codec(ptk, SampleCompression[swrite]);

        Sample_Vol[swrite] = 1.0f;
        if(Glob_Vol)
        {
            Read_Data_Swap(ptk, &Sample_Vol[swrite], sizeof(float), 1, in);
        }

        for(int slwrite = 0; slwrite < MAX_INSTRS_SPLITS; slwrite++)
        {
            Read_Data(ptk, &SampleType[swrite][slwrite], sizeof(char), 1, in);
            if(SampleType[swrite][slwrite])
            {
                if(old_bug) Read_Data(ptk, &SampleName[swrite][slwrite], sizeof(char), 256, in);
                else Read_Data(ptk, &SampleName[swrite][slwrite], sizeof(char), 64, in);
                Read_Data(ptk, &Basenote[swrite][slwrite], sizeof(char), 1, in);
                
                Read_Data_Swap(ptk, &LoopStart[swrite][slwrite], sizeof(int), 1, in);
                Read_Data_Swap(ptk, &LoopEnd[swrite][slwrite], sizeof(int), 1, in);
                Read_Data(ptk, &LoopType[swrite][slwrite], sizeof(char), 1, in);
                
                Read_Data_Swap(ptk, &SampleLength[swrite][slwrite], sizeof(int), 1, in);
                Read_Data(ptk, &Finetune[swrite][slwrite], sizeof(char), 1, in);
                Read_Data_Swap(ptk, &Sample_Amplify[swrite][slwrite], sizeof(float), 1, in);
                Read_Data_Swap(ptk, &FDecay[swrite][slwrite], sizeof(float), 1, in);

                AllocateWave(swrite, slwrite, SampleLength[swrite][slwrite], 1, FALSE, NULL, NULL);

                // Samples data
                Read_Data(ptk, RawSamples[swrite][0][slwrite], sizeof(short), SampleLength[swrite][slwrite], in);
                Swap_Sample(ptk, RawSamples[swrite][0][slwrite], swrite, slwrite);
                *RawSamples[swrite][0][slwrite] = 0;

                // Number of channel(s)
                Read_Data(ptk, &SampleChannels[swrite][slwrite], sizeof(char), 1, in);
                if(SampleChannels[swrite][slwrite] == 2)
                {
                    RawSamples[swrite][1][slwrite] = (short *) malloc(SampleLength[swrite][slwrite] * sizeof(short) + 8);
                    memset(RawSamples[swrite][1][slwrite], 0, SampleLength[swrite][slwrite] * sizeof(short) + 8);
                    Read_Data(ptk, RawSamples[swrite][1][slwrite], sizeof(short), SampleLength[swrite][slwrite], in);
                    Swap_Sample(ptk, RawSamples[swrite][1][slwrite], swrite, slwrite);
                    *RawSamples[swrite][1][slwrite] = 0;
                }
            } // Exist Sample
        }
        fclose(in);
        Actualize_Patterned(ptk);
        Actualize_Instrument_Ed(ptk, 2, 0);
        Actualize_Synth_Ed(ptk, UPDATE_SYNTH_ED_ALL);
        Status_Box(ptk, "Instrument loaded ok.");
    }
    else
    {
        Status_Box(ptk, "Instrument loading failed. (Possible cause: file not found)");
    }
    
    Clear_Input(ptk);
}

// ------------------------------------------------------
// Save the current instrument
void SaveInst(ptk_data *ptk)
{
    FILE *in;
    char Temph[MAX_PATH];
    char extension[10];
    char synth_prg;
    int synth_save;

    sprintf(extension, "TWNNINS9");
    if(!strlen(nameins[ptk->Current_Instrument])) sprintf(nameins[ptk->Current_Instrument], "Untitled");
    sprintf (Temph, "Saving '%s.pti' instrument in instruments directory...", nameins[ptk->Current_Instrument]);
    Status_Box(ptk, Temph);
    sprintf(Temph, "%s"SLASH"%s.pti", Dir_Instrs, nameins[ptk->Current_Instrument]);

    in = fopen(Temph, "wb");
    if(in != NULL)
    {
        // Writing header & name...
        Write_Data(ptk, extension, sizeof(char), 9, in);
        rtrim_string(ptk, nameins[ptk->Current_Instrument], 20);
        Write_Data(ptk, nameins[ptk->Current_Instrument], sizeof(char), 20, in);

        // Writing sample data
        int swrite = ptk->Current_Instrument;

        Write_Data(ptk, &Midiprg[swrite], sizeof(char), 1, in);
        switch(Synthprg[swrite])
        {
            case SYNTH_WAVE_OFF:
            case SYNTH_WAVE_CURRENT:
                synth_prg = Synthprg[swrite];
                synth_save = swrite;
                break;
            default:
                synth_prg = SYNTH_WAVE_CURRENT;
                synth_save = Synthprg[swrite] - 2;
                break;
        }

        Write_Data(ptk, &synth_prg, sizeof(char), 1, in);

        Write_Synth_Params(ptk, Write_Data, Write_Data_Swap, in, swrite);

        Write_Data(ptk, &SampleCompression[swrite], sizeof(char), 1, in);
        switch(SampleCompression[swrite])
        {
            case SMP_PACK_MP3:
                Write_Data(ptk, &Mp3_BitRate[swrite], sizeof(char), 1, in);
                break;

            case SMP_PACK_AT3:
                Write_Data(ptk, &At3_BitRate[swrite], sizeof(char), 1, in);
                break;
        }

        Write_Data_Swap(ptk, &Sample_Vol[swrite], sizeof(float), 1, in);

        swrite = synth_save;
        for(int slwrite = 0; slwrite < MAX_INSTRS_SPLITS; slwrite++)
        {
            Write_Data(ptk, &SampleType[swrite][slwrite], sizeof(char), 1, in);
            if(SampleType[swrite][slwrite])
            {
                rtrim_string(ptk, SampleName[swrite][slwrite], 64);
                Write_Data(ptk, SampleName[swrite][slwrite], sizeof(char), 64, in);
                Write_Data(ptk, &Basenote[swrite][slwrite], sizeof(char), 1, in);
                
                Write_Data_Swap(ptk, &LoopStart[swrite][slwrite], sizeof(int), 1, in);
                Write_Data_Swap(ptk, &LoopEnd[swrite][slwrite], sizeof(int), 1, in);
                Write_Data(ptk, &LoopType[swrite][slwrite], sizeof(char), 1, in);
                
                Write_Data_Swap(ptk, &SampleLength[swrite][slwrite], sizeof(int), 1, in);
                Write_Data(ptk, &Finetune[swrite][slwrite], sizeof(char), 1, in);
                Write_Data_Swap(ptk, &Sample_Amplify[swrite][slwrite], sizeof(float), 1, in);
                Write_Data_Swap(ptk, &FDecay[swrite][slwrite], sizeof(float), 1, in);

                Write_Unpacked_Sample(ptk, Write_Data, in, swrite, slwrite);

            } // Exist Sample
        }
        fclose(in);

        Read_SMPT(ptk);
        ptk->last_index = -1;
        Actualize_Files_List(ptk, 0);
        Actualize_Patterned(ptk);
        Status_Box(ptk, "Instrument saved succesfully."); 
    }
    else
    {
        Status_Box(ptk, "Instrument save failed.");
    }

    Clear_Input(ptk);
}
