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

#ifndef _MAIN_H_
#define _MAIN_H_

#ifdef __cplusplus

#include <string>
#include <list>
#include <sstream>
using namespace std;

#endif

extern SDL_Surface *POINTER;

typedef struct
{
    int Channel;
    int Sub_Channel;
    int Note;
} JAZZ_KEY, *LPJAZZ_KEY;

#if defined(__WIN32__)
#define STDCALL __stdcall
#else
#define STDCALL
#endif

#if defined(__WIN32__)
#define GETCWD _getcwd
#define CHDIR _chdir
#define GETDRIVE _getdrive
#else
#define GETCWD getcwd
#define CHDIR chdir
#define FA_DIREC 0x10
#define A_SUBDIR FA_DIREC
#define _A_SUBDIR FA_DIREC
#endif
#define _A_FILE 0
#define _A_SEP -1

int Init_Context(ptk_data *ptk);
void Destroy_Context(ptk_data *ptk);
int Screen_Update(ptk_data *ptk);
void STDCALL Mixer(ptk_data *ptk, Uint8 *Buffer, Uint32 Len);
SDL_Surface *Load_Skin_Picture(char *name);
LPJAZZ_KEY Get_Jazz_Key_Off(JAZZ_KEY Pool[MAX_TRACKS][MAX_POLYPHONY], int Note);
int Discard_Key_Note_Off(JAZZ_KEY Pool[MAX_TRACKS][MAX_POLYPHONY], int Channel, int Sub_Channel);
void Set_Default_Channels_Polyphony(void);
void Clear_Midi_Channels_Pool(void);
void Send_Note(int Note, int Raw_Note, int One_Channel);
void Note_Jazz(ptk_data *ptk, int track, int note, float volume);
void Note_Jazz_Off(ptk_data *ptk, int note);
void Display_Beat_Time(void);
void ptk_init(ptk_data *ptk);
void ptk_close(ptk_data *ptk);

#endif
