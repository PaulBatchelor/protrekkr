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

#ifndef _MIDICFG_H_
#define _MIDICFG_H_

// ------------------------------------------------------
// Includes
#include "files.h"
#include "files_list.h"
#include "../../editors/include/editor_midi.h"
#include "../../ui/include/misc_draw.h"

// ------------------------------------------------------
// Functions
void Load_MidiCfg_Data(ptk_data *ptk, int (*Read_Function)(ptk_data *, void *, int ,int, FILE *),
                       int (*Read_Function_Swap)(ptk_data *, void *, int ,int, FILE *),
                       FILE *in);
void Save_MidiCfg_Data(ptk_data *ptk, int (*Write_Function)(ptk_data *, void *, int ,int, FILE *),
                       int (*Write_Function_Swap)(ptk_data *, void *, int ,int, FILE *),
                       FILE *in);
void LoadMidiCfg(ptk_data *ptk, char *FileName);
void SaveMidiCfg(ptk_data *ptk);

#endif
