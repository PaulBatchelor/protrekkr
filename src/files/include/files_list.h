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

#ifndef _FILES_LIST_H_
#define _FILES_LIST_H_

// ------------------------------------------------------
// Includes
#if defined(__GCC__)
#include "../../include/variables.h"
#include "../../ui/include/misc_draw.h"
#else
#include "../include/variables.h"
#include "../ui/include/misc_draw.h"
#endif

#if defined(__WIN32__)
#include <io.h>
#include <direct.h>
#endif

// ------------------------------------------------------
// Constants
#define NBR_ITEMS 11

// ------------------------------------------------------
// Variables
extern int lt_ykar[SCOPE_LAST_DIR];
extern int lt_items[SCOPE_LAST_DIR];
extern int lt_index[SCOPE_LAST_DIR];
extern int lt_curr[SCOPE_LAST_DIR];

// ------------------------------------------------------
// Functions
void Set_Current_Dir(ptk_data *ptk);
void Read_SMPT(ptk_data *ptk);
void Dump_Files_List(ptk_data *ptk, int xr, int yr);
void Actualize_Files_List(ptk_data *ptk, int modeac);
void Clear_Files_List(ptk_data *ptk);
char *Get_FileName(ptk_data *ptk, int idx);
int Get_FileType(ptk_data *ptk, int idx);
void Draw_Lists_Slider(ptk_data *ptk, int idx);
void Files_List_Move(ptk_data *ptk, int Amount);
void Prev_Prefix(ptk_data *ptk);
void Next_Prefix(ptk_data *ptk);

#endif
