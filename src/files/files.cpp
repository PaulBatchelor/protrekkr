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
#include "ptk_data.h"
#include "variables.h"
#include "replay.h"
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
#include "requesters.h"

#include "endianness.h"

// ------------------------------------------------------
// Variables
extern REQUESTER Overwrite_Requester;

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
    return(0);
}

// ------------------------------------------------------
// Write data into a module file (handling bytes swapping)
int Write_Mod_Data_Swap(ptk_data *ptk, void *Datas, int Unit, int Length, FILE *Handle)
{
    return(0);
}

// ------------------------------------------------------
// Read data from a module file
int Read_Mod_Data(ptk_data *ptk, void *Datas, int Unit, int Length, FILE *Handle)
{
    return(0);
}

// ------------------------------------------------------
// Read data from a module file
int Read_Mod_Data_Swap(ptk_data *ptk, void *Datas, int Unit, int Length, FILE *Handle)
{
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
        sprintf(ptk->OverWrite_Name, "File '%s' already exists, overwrite ?", Temph);
        Overwrite_Requester.Text = ptk->OverWrite_Name;
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
    return 0;
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

/*TODO: Calc_Length should eventually find its way into a C file */
extern "C" {
unsigned long Calc_Length(ptk_data *ptk)
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
    unsigned long samps = 0;
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
    while(i < ptk->Song_Length)
    {
        if(have_break < MAX_ROWS) pos_patt = have_break;
        else pos_patt = 0;
        have_break = 255;
        while(pos_patt < ptk->patternLines[ptk->pSequence[i]])
        {
            Cur_Patt = ptk->RawPatterns + (ptk->pSequence[i] * PATTERN_LEN) + (pos_patt * PATTERN_ROW_LEN);
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
                                if(i == (ptk->Song_Length - 1) || patt_datas[l] <= i)
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
                samps += PosTicks - 1;

                nbr_ticks++;
                PosTicks = 0;
                if(have_break > 127)
                {
                    pos_patt++;
                }
                else
                {
                    // End the pattern here
                    pos_patt = ptk->patternLines[ptk->pSequence[i]];
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

    if(ptk->start_gui == TRUE) Display_Song_Length(ptk);

    return samps;
}
} /* extern C */

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
        beatsync[n_index] = FALSE;

        // Internal is default compression
        SampleCompression[n_index] = SMP_PACK_INTERNAL;
        SamplesSwap[n_index] = FALSE;
        Mp3_BitRate[n_index] = 0;
        At3_BitRate[n_index] = 0;
    }
}

extern "C" {
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
