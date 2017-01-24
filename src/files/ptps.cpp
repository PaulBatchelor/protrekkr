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
#include "version.h"
#include "ptps.h"

// ------------------------------------------------------
// Variables
FILE *Out_constants;
FILE *Out_FX;
int Rec_Fx = 0;
LPSYNC_FX Sync_Fx;
unsigned char Old_pSequence[MAX_SEQUENCES];
int Nbr_Muted_Tracks;
int Muted_Tracks[MAX_TRACKS];
int Nbr_Used_Instr;
extern EQSTATE EqDat[MAX_TRACKS];
INSTR_ORDER Used_Instr[MAX_INSTRS];
INSTR_ORDER Used_Instr2[MAX_INSTRS];
INSTR_ORDER Used_Instr3[MAX_INSTRS];
extern gear303 tb303engine[2];
extern para303 tb303[2];

// ------------------------------------------------------
// Save a constant into a properties include file
void Save_Constant(char *Name, int value)
{
    if(Out_constants)
    {
        if(value) fprintf(Out_constants, "#define %s\n", Name);
        else fprintf(Out_constants, "// #define %s\n", Name);
    }
}

// ------------------------------------------------------
// Look for a fx already recorded for that pos/row
int Check_FX(int pos, int row)
{
    int i;

    for(i = 0; i < Rec_Fx; i++)
    {
        if(pos == Sync_Fx[i].Pos &&
           row == Sync_Fx[i].Row)
        {
            return(i);
        }
    }
    return(-1);
}

// ------------------------------------------------------
// Save a synchro marker
void Save_FX(int pos, int row, int data)
{
    if(Out_FX)
    {
        fwrite(&pos, 1, 1, Out_FX);
        fwrite(&row, 1, 1, Out_FX);
        fwrite(&data, 1, 1, Out_FX);
    }
}

// ------------------------------------------------------
// Look for a sequence index
int Search_Sequence(int sequence_idx)
{
    int i;

    for(i = 0; i < MAX_SEQUENCES; i++)
    {
        if(Old_pSequence[i] == sequence_idx)
        {
            return(i);
        }
    }
    return(-1);
}

// ------------------------------------------------------
// Return TRUE if a given track is muted
int Track_Is_Muted(int track)
{
    int i;

    for(i = 0; i < Nbr_Muted_Tracks; i++)
    {
        if(Muted_Tracks[i] == track) return(TRUE);
    }
    return(FALSE);
}

// ------------------------------------------------------
// Return the new index of an instrument for an old one
int Get_Instr_New_Order(int instr)
{
    int i;

    for(i = 0; i < MAX_INSTRS; i++)
    {
        if(Used_Instr[i].old_order == instr) return(Used_Instr[i].new_order);
    }
    return(-1);
}

// ------------------------------------------------------
// Check if an index is within range of channel's variable data
int Check_Range(int Idx, int Bound, int Start)
{
    int i;
    int Val = Start;

    for(i = 0; i < Bound; i++)
    {
        if(Idx == Val)
        {
            return TRUE;
        }
        Val += 2;
    }
    return FALSE;
}

// ------------------------------------------------------
// Save a packed (.ptp) module
// (Only the samples are actually (if requested) packed,
//  the rest is just "demangled" to ease packers compression ratio).
int SavePtp(ptk_data *ptk, FILE *in, int Simulate, char *FileName)
{
}
