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
#include "replay.h"
#include "ptk.h"
#include "files_list.h"

#if defined(__WIN32__)
#include <shlwapi.h>
#elif defined(__AMIGAOS4__) || defined(__AROS__)
#include <dos/dosextens.h>
#include <proto/dos.h>
#include <dirent.h>
#if defined(__AROS__)
#include <stdint.h>
#define int32 int32_t
#define uint32 uint32_t
#endif
#else
#include <ftw.h>
#endif

// ------------------------------------------------------
// Structures
typedef struct
{
    char Name[1024];
    int Type;
} FILEENTRY, *LPFILEENTRY;

// ------------------------------------------------------
// Variables
int lt_ykar[SCOPE_LAST_DIR];
int lt_items[SCOPE_LAST_DIR];
int lt_index[SCOPE_LAST_DIR];
int lt_curr[SCOPE_LAST_DIR];
int list_counter[SCOPE_LAST_DIR];
int sort_files = TRUE;              // Just in case i would need to make it optional someday
int nbr_dirs;

FILEENTRY SMPT_LIST[2048];
char UpName1[1024];
char UpName2[1024];

#if defined(__WIN32__)
char List_Drives[1024 + 1];
#endif

char Dir_Act[MAX_PATH];
char Dir_Mods[MAX_PATH];
char Dir_Instrs[MAX_PATH];
char Dir_Presets[MAX_PATH];
char Dir_Reverbs[MAX_PATH];
char Dir_MidiCfg[MAX_PATH];
char Dir_Patterns[MAX_PATH];
char Dir_Samples[MAX_PATH];
char *cur_dir;

/* TODO: remove global variable hack */
extern ptk_data *g_ptk;

// ------------------------------------------------------
// Functions
void Clear_Files_List(ptk_data *ptk)
{
    for(int listcleaner = 0; listcleaner < 2048; listcleaner++)
    {
        memset(SMPT_LIST[listcleaner].Name, 0, sizeof(SMPT_LIST[0]));
    }
}

void Add_Entry(ptk_data *ptk, const char *Name, int Type)
{
    sprintf(SMPT_LIST[list_counter[ptk->Scopish]].Name, Name);
    SMPT_LIST[list_counter[ptk->Scopish]].Type = Type;
    lt_items[ptk->Scopish]++;
    list_counter[ptk->Scopish]++;
}

void Insert_Entry(ptk_data *ptk, char *Name, int Type, int idx)
{
    int i;
    for(i = list_counter[ptk->Scopish] - 1; i >= idx; i--)
    {
        memcpy(&SMPT_LIST[i + 1], &SMPT_LIST[i], sizeof(FILEENTRY));
    }
    sprintf(SMPT_LIST[idx].Name, Name);
    SMPT_LIST[idx].Type = Type;
    lt_items[ptk->Scopish]++;
    list_counter[ptk->Scopish]++;
}

char *Get_FileName(ptk_data *ptk, int idx)
{
    return(SMPT_LIST[idx].Name);
}

int Get_FileType(ptk_data *ptk, int idx)
{
    return(SMPT_LIST[idx].Type);
}

int FileComp(const void *elem1, const void *elem2)
{
    int i;
    LPFILEENTRY Ent1 = (LPFILEENTRY) elem1;
    LPFILEENTRY Ent2 = (LPFILEENTRY) elem2;
    for(i = 0; Ent1->Name[i]; i++)
    {
        UpName1[i] = toupper(Ent1->Name[i]);
    }
    for(i = 0; Ent2->Name[i]; i++)
    {
        UpName2[i] = toupper(Ent2->Name[i]);
    }
    return strcmp(UpName1, UpName2);
}

int FileComp_Files(const void *elem1, const void *elem2)
{
    int i;
    LPFILEENTRY Ent1 = (LPFILEENTRY) elem1;
    LPFILEENTRY Ent2 = (LPFILEENTRY) elem2;
    
    // File before directories
    if(Ent1->Type == _A_SUBDIR && Ent2->Type == _A_FILE) return 1;
    if(Ent1->Type == _A_FILE && Ent2->Type == _A_SUBDIR) return -1;

    // Make sure both entries share the same type
    if(Ent1->Type == _A_FILE && Ent2->Type == _A_FILE ||
       Ent1->Type == _A_SUBDIR && Ent1->Type == _A_SUBDIR)
    {
        for(i = 0; Ent1->Name[i]; i++)
        {
            UpName1[i] = toupper(Ent1->Name[i]);
        }
        for(i = 0; Ent2->Name[i]; i++)
        {
            UpName2[i] = toupper(Ent2->Name[i]);
        }
        return strcmp(UpName1, UpName2);
    }
    return 0;
}

/*void Insert_List_Separators(void)
{
    int i;
    char Sort_Letter;
    char Sort_Type;
    char Cur_Letter;
    int Cur_Type;
    
    Sort_Letter = 0;
    Sort_Type = Get_FileType(ptk, 0);
    for(i = 0; i < list_counter[ptk->Scopish]; i++)
    {
        // Make sure we're in the same ensemble
        Cur_Type = Get_FileType(ptk, i);
        if(Sort_Type == Cur_Type && Cur_Type == _A_FILE)
        {
            Cur_Letter = toupper(Get_FileName(ptk, i)[0]);
            if(Sort_Letter != Cur_Letter)
            {
                Sort_Letter = Cur_Letter;
                Insert_Entry(ptk, "", _A_SEP, i);
            }
        }
        else
        {
            Sort_Type = Cur_Type;
        }
    }
}*/

void Set_Current_Dir(ptk_data *ptk)
{
    char filename[MAX_PATH];

#if defined(__AMIGAOS4__) || defined(__AROS__)
    char *tmp = Dir_Act[0] ? &Dir_Act[strlen(Dir_Act) - 1] : NULL;

    if (tmp && *tmp == '/') *tmp = 0;
    if (strrchr(Dir_Act, '/') == Dir_Act &&
        !strcmp(Get_FileName(ptk, lt_curr[ptk->Scopish]), ".."))
    {
        switch(ptk->Scopish)
        {
            case SCOPE_ZONE_MOD_DIR:
                strcpy(Dir_Mods, "/");
                break;
            case SCOPE_ZONE_INSTR_DIR:
                strcpy(Dir_Instrs, "/");
                break;
            case SCOPE_ZONE_PRESET_DIR:
                strcpy(Dir_Presets, "/");
                break;
            case SCOPE_ZONE_REVERB_DIR:
                strcpy(Dir_Reverbs, "/");
                break;
            case SCOPE_ZONE_MIDICFG_DIR:
                strcpy(Dir_MidiCfg, "/");
                break;
            case SCOPE_ZONE_PATTERN_DIR:
                strcpy(Dir_Patterns, "/");
                break;
            case SCOPE_ZONE_SAMPLE_DIR:
                strcpy(Dir_Samples, "/");
                break;
        }
        return;
    }
    if (tmp && *tmp == 0)
    {
        *tmp = '/';
    }    
    if (!strcmp(Dir_Act, "/"))
    {
        strcpy(filename, "/");
        strcat(filename, Get_FileName(ptk, lt_curr[ptk->Scopish]));
    }
    else
    {
        strcpy(filename, Get_FileName(ptk, lt_curr[ptk->Scopish]));
    }
#else
    strcpy(filename, Get_FileName(ptk, lt_curr[ptk->Scopish]));
#endif
 
    switch(ptk->Scopish)
    {
        case SCOPE_ZONE_MOD_DIR:
            CHDIR(filename);
            GETCWD(Dir_Mods, MAX_PATH);
            break;
        case SCOPE_ZONE_INSTR_DIR:
            CHDIR(filename);
            GETCWD(Dir_Instrs, MAX_PATH);
            break;
        case SCOPE_ZONE_PRESET_DIR:
            CHDIR(filename);
            GETCWD(Dir_Presets, MAX_PATH);
            break;
        case SCOPE_ZONE_REVERB_DIR:
            CHDIR(filename);
            GETCWD(Dir_Reverbs, MAX_PATH);
            break;
        case SCOPE_ZONE_MIDICFG_DIR:
            CHDIR(filename);
            GETCWD(Dir_MidiCfg, MAX_PATH);
            break;
        case SCOPE_ZONE_PATTERN_DIR:
            CHDIR(filename);
            GETCWD(Dir_Patterns, MAX_PATH);
            break;
        case SCOPE_ZONE_SAMPLE_DIR:
            CHDIR(filename);
            GETCWD(Dir_Samples, MAX_PATH);
            break;
    }
}

#if defined(__LINUX__) || defined(__MACOSX__)
/*TODO is there a way to remove global variables from this ?*/
int list_file(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    int i;
    int len_name;

    if(ftwbuf->level == 1)
    {
        if(strcmp(fpath, Dir_Act))
        {
            switch(typeflag)
            {
                case FTW_D:
                    len_name = strlen(fpath);
                    while(len_name--)
                    {
                        if(fpath[len_name] == '/')
                        {
                            len_name++;
                            break;
                        }
                    }
                    
                    nbr_dirs++;
                    Add_Entry(g_ptk, &fpath[len_name], _A_SUBDIR);
                    break;

                case FTW_F:
                    len_name = strlen(fpath);
                    while(len_name--)
                    {
                        if(fpath[len_name] == '/')
                        {
                            len_name++;
                            break;
                        }
                    }
                    
                    Add_Entry(g_ptk, &fpath[len_name], _A_FILE);
                    break;
            }
        }
    }
    return 0;
}
#endif

// AROS
#if defined(__AROS__)
void CopyStringBSTRToC(BSTR in,
                       STRPTR out,
                       uint32_t max)
{
    uint32_t i;
    
    max = AROS_BSTR_strlen(in);
    
    for(i = 0; i < max; i++)
    {
        out[i] = *(AROS_BSTR_ADDR(in) + i);
    }
    out[i] = 0;
}
#endif

// ------------------------------------------------------
// Fill the list with the content of the relevant directory
void Read_SMPT(ptk_data *ptk)
{
    int i;

    lt_items[ptk->Scopish] = 0;  
    list_counter[ptk->Scopish] = 0;

#if defined(__WIN32__)
    struct _finddata_t c_file;
#endif

    long hFile;

    nbr_dirs = 0;

    switch(ptk->Scopish)
    {
        case SCOPE_ZONE_MOD_DIR:
            cur_dir = Dir_Mods;
            break;
        case SCOPE_ZONE_INSTR_DIR:
            cur_dir = Dir_Instrs;
            break;
        case SCOPE_ZONE_PRESET_DIR:
            cur_dir = Dir_Presets;
            break;
        case SCOPE_ZONE_REVERB_DIR:
            cur_dir = Dir_Reverbs;
            break;
        case SCOPE_ZONE_PATTERN_DIR:
            cur_dir = Dir_Patterns;
            break;
        case SCOPE_ZONE_SAMPLE_DIR:
            cur_dir = Dir_Samples;
            break;
        case SCOPE_ZONE_MIDICFG_DIR:
            cur_dir = Dir_MidiCfg;
            break;
    }
    CHDIR(cur_dir);

    // Find first .c file in current directory
    strcpy(Dir_Act, cur_dir);

#if defined(__WIN32__)
    strcat(Dir_Act, "\\*.*");

    if((hFile = _findfirst(Dir_Act, &c_file)) == -1L)
    {
        Add_Entry(ptk, "No files in current directory.", 0);
    }
    else
    {
        // The first directory
        if(c_file.attrib & _A_SUBDIR)
        {
            if(strcmp(c_file.name, ".") &&
               strcmp(c_file.name, ".."))
            {
                nbr_dirs++;
                Add_Entry(ptk, c_file.name, _A_SUBDIR);
            }
        }

        // Find the rest of the directories 
        while(_findnext(hFile, &c_file) == 0)
        {
            if(c_file.attrib & _A_SUBDIR)
            {
                if(strcmp(c_file.name, ".") &&
                   strcmp(c_file.name, ".."))
                {
                    nbr_dirs++;
                    Add_Entry(ptk, c_file.name, _A_SUBDIR);
                }
            }
        }
        // End dir
        _findclose(hFile);

        if((hFile = _findfirst(Dir_Act, &c_file)) != -1L)
        {
            // The first file
            if(!(c_file.attrib & _A_SUBDIR))
            {
                Add_Entry(ptk, c_file.name, 0);
            }
            // Find the rest of the files
            while(_findnext(hFile, &c_file) == 0)
            {
                if(!(c_file.attrib & _A_SUBDIR))
                {
                    Add_Entry(ptk, c_file.name, 0);
                }
            } // while      
            _findclose(hFile);

            if(sort_files)
            {
                qsort(&SMPT_LIST[0], list_counter[ptk->Scopish], sizeof(FILEENTRY), &FileComp_Files);
                //Insert_List_Separators();
            }
            //else
            {
                Insert_Entry(ptk, "", _A_SEP, 0);
            }
            Insert_Entry(ptk, "..", _A_SUBDIR, 0);
            Insert_Entry(ptk, ".", _A_SUBDIR, 0);
        }
    }

    // Add the available drives to the bottom of the list
    char *Ptr_Drives;
    GetLogicalDriveStrings(1024, List_Drives);

    Ptr_Drives = List_Drives;
    i = 0;
    if(Ptr_Drives[0]) Add_Entry(ptk, "", _A_SEP);
    while(Ptr_Drives[0])
    {
        Add_Entry(ptk, Ptr_Drives, _A_SUBDIR);
        Ptr_Drives += strlen(Ptr_Drives) + 1;
    }

#elif defined(__AMIGAOS4__) || defined(__AROS__)

#if defined(__AMIGAOS4__)
#define LockDosList(f) IDOS->LockDosList(f)
#define NextDosEntry(d,f) IDOS->NextDosEntry(d, f)
#define CopyStringBSTRToC(bin,sout,len) IDOS->CopyStringBSTRToC(bin, sout, len)
#define	UnLockDosList(f) IDOS->UnLockDosList(f)
#endif

    if (!strcmp(Dir_Act, "/"))
    {
        // Only display volumes
        struct DosList *dl;
        const uint32 flags = LDF_VOLUMES | LDF_READ;
        char BString[1024];
        dl = LockDosList(flags);
        while ((dl = NextDosEntry(dl, flags)) != NULL)
        {
            // Convert it first
            CopyStringBSTRToC(dl->dol_Name, BString, sizeof(BString));
            Add_Entry(ptk, BString, _A_SUBDIR);
        }
        UnLockDosList(flags);
    }
    else
    {
        DIR *dirp;
        struct dirent *dp;

        dirp = opendir(Dir_Act);
        if (dirp)
        {
            // Add the directories first
            while ((dp = readdir(dirp)) != NULL)
            {
                if(dp->d_type == DT_DIR)
                {
                    nbr_dirs++;
                    Add_Entry(ptk, dp->d_name, _A_SUBDIR);
                }
            }
            closedir(dirp);
        }

        dirp = opendir(Dir_Act);
        if (dirp)
        {
            // Then add the files
            while ((dp = readdir(dirp)) != NULL)
            {
                if(dp->d_type != DT_DIR)
                {
                    Add_Entry(ptk, dp->d_name, 0);
                }
            }
            closedir(dirp);
        }

        if(sort_files)
        {
            qsort(&SMPT_LIST[0], list_counter[ptk->Scopish], sizeof(FILEENTRY), &FileComp_Files);
            //Insert_List_Separators();
        }
        //else
        {
            Insert_Entry(ptk, "", _A_SEP, 0);
        }
        // Insert parent directory at the top
        Insert_Entry(ptk, "/", _A_SUBDIR, 0);
    }

#else

    // Enum them
    nftw(Dir_Act, &list_file, FTW_PHYS, 0);

    if(sort_files)
    {
        qsort(&SMPT_LIST[0], list_counter[ptk->Scopish], sizeof(FILEENTRY), &FileComp_Files);
        //Insert_List_Separators();
    }
    //else
    {
        Insert_Entry(ptk, "", _A_SEP, 0);
    }
    // Always insert them at the top of the list
    Insert_Entry(ptk, "../", _A_SUBDIR, 0);
    Insert_Entry(ptk, "./", _A_SUBDIR, 0);

#endif

    // Insert a separator between files and directories
    if(nbr_dirs)
    {
        for(i = list_counter[ptk->Scopish] - 1; i >= 0; i--)
        {
            if(SMPT_LIST[i].Type == _A_FILE)
            {
                Insert_Entry(ptk, "", _A_SEP, i + 1);
                break;
            }
        }
    }
}

// ------------------------------------------------------
// Display the files list on screen
void Dump_Files_List(ptk_data *ptk, int xr, int yr)
{
    int y = lt_index[ptk->Scopish];
    FILE *File;
    char Size_String[64];
    int space = Font_Height + 1;

    switch(ptk->Scopish)
    {
        case SCOPE_ZONE_MOD_DIR:
        case SCOPE_ZONE_INSTR_DIR:
        case SCOPE_ZONE_PRESET_DIR:
        case SCOPE_ZONE_REVERB_DIR:
        case SCOPE_ZONE_PATTERN_DIR:
        case SCOPE_ZONE_MIDICFG_DIR:
        case SCOPE_ZONE_SAMPLE_DIR:
            SetColor(COL_BACKGROUND);
            bjbox(xr - 1, yr + 1, Cur_Width - 412, 137);

            // Current dir background
            Gui_Draw_Button_Box(394, 24, Cur_Width - 522, 16, "", BUTTON_NORMAL | BUTTON_DISABLED);

            switch(ptk->Scopish)
            {
                case SCOPE_ZONE_MOD_DIR:
                    PrintString(398, 26, USE_FONT, Dir_Mods, (Cur_Width - 522));
                    break;
                case SCOPE_ZONE_INSTR_DIR:
                    PrintString(398, 26, USE_FONT, Dir_Instrs, (Cur_Width - 522));
                    break;
                case SCOPE_ZONE_PRESET_DIR:
                    PrintString(398, 26, USE_FONT, Dir_Presets, (Cur_Width - 522));
                    break;
                case SCOPE_ZONE_REVERB_DIR:
                    PrintString(398, 26, USE_FONT, Dir_Reverbs, (Cur_Width - 522));
                    break;
                case SCOPE_ZONE_MIDICFG_DIR:
                    PrintString(398, 26, USE_FONT, Dir_MidiCfg, (Cur_Width - 522));
                    break;
                case SCOPE_ZONE_PATTERN_DIR:
                    PrintString(398, 26, USE_FONT, Dir_Patterns, (Cur_Width - 522));
                    break;
                case SCOPE_ZONE_SAMPLE_DIR:
                    PrintString(398, 26, USE_FONT, Dir_Samples, (Cur_Width - 522));
                    break;
            }

            if(lt_items[ptk->Scopish])
            {
                for(int counter = 0; counter < NBR_ITEMS; counter++)
                {
                    int rel_val = y + counter;

                    if(y + counter < lt_items[ptk->Scopish])
                    {
                        // Highlight bar in files requester.
                        if(y + counter == lt_curr[ptk->Scopish])
                        {
                            SetColor(COL_PUSHED_MED);
                            bjbox(xr - 1, yr + (counter * space) + 2, (Cur_Width - 413), space);
                        }

                        switch(Get_FileType(ptk, rel_val))
                        {
                            case _A_SUBDIR:
                                PrintString(xr, yr + (counter * space), USE_FONT_LOW, Get_FileName(ptk, rel_val), Cur_Width - 504);
                                PrintString(xr + (Cur_Width - 436), yr + (counter * space) + 1, USE_FONT_LOW, "<Dir>");
                                break;
                            case _A_FILE:
                                PrintString(xr, yr + (counter * space) + 1, USE_FONT, Get_FileName(ptk, rel_val), Cur_Width - 504);
                                File = fopen(Get_FileName(ptk, rel_val), "rb");
                                if(File)
                                {
                                    int Size = Get_File_Size(ptk, File);
                                    if(Size == 0)
                                    {
                                        sprintf(Size_String, "0");
                                    }
                                    else sprintf(Size_String, "%9.d", Size);
                                    int pos = (xr + (Cur_Width - 415)) - Get_Size_Text(Size_String);
                                    PrintString(pos, yr + (counter * space) + 1, USE_FONT, Size_String);
                                    fclose(File);
                                }
                                else
                                {
                                    PrintString(xr + (Cur_Width - 460), yr + (counter * space) + 1, USE_FONT_LOW, "<Locked>");
                                }
                                break;
                            case _A_SEP:
                                SetColor(COL_PUSHED_HI);
                                bjbox(xr - 1, yr + (counter * space) + (space / 2) + 1, Cur_Width - 413, 1);
                                break;
                        }
                    }
                }
            }
            else
            {
                PrintString(xr, yr, USE_FONT_LOW, Get_FileName(ptk, 0));
            }
            break;
    }
}

// ------------------------------------------------------
// Redraw the files list
void Actualize_Files_List(ptk_data *ptk, int modeac)
{
    int const brolim = lt_items[ptk->Scopish] - NBR_ITEMS;

    switch(ptk->Scopish)
    {
        case SCOPE_ZONE_MOD_DIR:
        case SCOPE_ZONE_INSTR_DIR:
        case SCOPE_ZONE_PRESET_DIR:
        case SCOPE_ZONE_REVERB_DIR:
        case SCOPE_ZONE_MIDICFG_DIR:
        case SCOPE_ZONE_PATTERN_DIR:
        case SCOPE_ZONE_SAMPLE_DIR:

            if(modeac == 0)
            {
                if(lt_ykar[ptk->Scopish] > 70) lt_ykar[ptk->Scopish] = 70;
                if(lt_ykar[ptk->Scopish] < 0) lt_ykar[ptk->Scopish] = 0;
                lt_index[ptk->Scopish] = (lt_ykar[ptk->Scopish] * brolim) / 70;
            }

            if(lt_index[ptk->Scopish] > brolim) lt_index[ptk->Scopish] = brolim;
            if(lt_index[ptk->Scopish] < 0) lt_index[ptk->Scopish] = 0;
            if(modeac != 0)
            {
                if(brolim)
                {
                    lt_ykar[ptk->Scopish] = (lt_index[ptk->Scopish] * 70) / brolim;
                }
                else
                {
                    lt_ykar[ptk->Scopish] = (lt_index[ptk->Scopish] * 70);
                }
            }

            // Draw the files slider
            Draw_Lists_Slider(ptk, lt_ykar[ptk->Scopish]);
            if(ptk->last_index != lt_index[ptk->Scopish])
            {
                Dump_Files_List(ptk, 395, 41);
                ptk->last_index = lt_index[ptk->Scopish];
            }
            break;
    }
}

// ------------------------------------------------------
// Draw the slider beside the list
void Draw_Lists_Slider(ptk_data *ptk, int idx)
{
    SetColor(COL_BLACK);
    bjbox(Cur_Width - 18, 42, 18, 136);
    SetColor(COL_SLIDER_LO);
    bjbox(Cur_Width - 18, 59 - 1, 15 + 2, 103 + 2);
    SetColor(COL_SLIDER_HI);
    bjbox(Cur_Width - 17, 59, 15 + 1, 103 + 1);
    SetColor(COL_SLIDER_MED);
    bjbox(Cur_Width - 17, 59, 15, 103);
    Gui_Draw_Button_Box(MAX_PATT_SCREEN_X + 2, 58 + idx + 1, 16 - 2, 32, "", BUTTON_NORMAL);
    Gui_Draw_Button_Box(MAX_PATT_SCREEN_X + 1, 42, 16, 14, "\01", BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
    Gui_Draw_Button_Box(MAX_PATT_SCREEN_X + 1, 164, 16, 14, "\02", BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
}

// ------------------------------------------------------
// Move the lists up & down
void Files_List_Move(ptk_data *ptk, int Amount)
{
    switch(ptk->Scopish)
    {
        case SCOPE_ZONE_INSTR_LIST:
        case SCOPE_ZONE_SYNTH_LIST:

            if(zcheckMouse(ptk, MAX_PATT_SCREEN_X + 1, 42, 16, 14))
            {
                Instrs_index -= Amount;
                ptk->gui_action = GUI_CMD_INSTR_SYNTH_SCROLL;
            }

            if(zcheckMouse(ptk, MAX_PATT_SCREEN_X + 1, 164, 16, 14))
            {
                Instrs_index += Amount;
                ptk->gui_action = GUI_CMD_INSTR_SYNTH_SCROLL;
            }
            break;

        default:

            // Files list up
            if(zcheckMouse(ptk, MAX_PATT_SCREEN_X + 1, 42, 16, 14))
            {
                if(abs(Amount) > 1)
                {
                    Prev_Prefix(ptk);
                }
                else
                {
                    lt_index[ptk->Scopish] -= Amount;
                }
                ptk->gui_action = GUI_CMD_FILELIST_SCROLL;
            }

            // Files list down
            if(zcheckMouse(ptk, MAX_PATT_SCREEN_X + 1, 164, 16, 14))
            {
                if(abs(Amount) > 1)
                {
                    Next_Prefix(ptk);
                }
                else
                {
                    lt_index[ptk->Scopish] += Amount;
                }
                ptk->gui_action = GUI_CMD_FILELIST_SCROLL;
            }
            break;
    }
}

// ------------------------------------------------------
// Move an index in the files list
// Return TRUE if any boundary has been reached.
int Move_Idx(ptk_data *ptk, int *Idx, int Amount)
{
    *Idx += Amount;
    if(*Idx < 0)
    {
        lt_index[ptk->Scopish] = 0;
        return TRUE;
    }
    if(*Idx > lt_items[ptk->Scopish] - NBR_ITEMS)
    {
        lt_index[ptk->Scopish] = lt_items[ptk->Scopish] - NBR_ITEMS;
        return TRUE;
    }
    return FALSE;
}

// ------------------------------------------------------
// Go to previous prefix in a file list
void Prev_Prefix(ptk_data *ptk)
{
    char Cur_Letter;
    char Start_Letter;
    int Done_Sep;
    int Idx = lt_index[ptk->Scopish];

    // Adjust to a real entry
    Done_Sep = FALSE;
    if(Get_FileType(ptk, Idx) == _A_SUBDIR)
    {
        Move_Idx(ptk, &Idx, -1);
    }
    while(Get_FileType(ptk, Idx) == _A_SEP)
    {
        Done_Sep = TRUE;
        if(Move_Idx(ptk, &Idx, -1)) return;
    }
    if(Get_FileType(ptk, Idx) == _A_SUBDIR)
    {
        lt_index[ptk->Scopish] = Idx;
        return;
    }

    Start_Letter = toupper(Get_FileName(ptk, Idx)[0]);
    if(Move_Idx(ptk, &Idx, -1)) return;
    if(!Done_Sep)
    {
        while(Get_FileType(ptk, Idx) == _A_SEP)
        {
            if(Move_Idx(ptk, &Idx, -1)) return;
            if(Get_FileType(ptk, Idx) != _A_SEP)
            {
                Start_Letter = toupper(Get_FileName(ptk, Idx)[0]);
            }
        }
    }
    Cur_Letter = toupper(Get_FileName(ptk, Idx)[0]);
    if(Cur_Letter != Start_Letter)
    {
        // Renew prefix
        Start_Letter = toupper(Get_FileName(ptk, Idx)[0]);
    }
    while(Start_Letter == Cur_Letter)
    {
        if(Move_Idx(ptk, &Idx, -1)) return;
        Cur_Letter = toupper(Get_FileName(ptk, Idx)[0]);
    }
    lt_index[ptk->Scopish] = Idx + 1;
}

// ------------------------------------------------------
// Go to next prefix in a file list
void Next_Prefix(ptk_data *ptk)
{
    char Cur_Letter;
    char Start_Letter;
    int Idx = lt_index[ptk->Scopish];
    int Was_Dir;

    Was_Dir = FALSE;

    // Adjust to a real entry
    if(Get_FileType(ptk, Idx) == _A_SUBDIR)
    {
        Was_Dir = TRUE;
        Move_Idx(ptk, &Idx, 1);
    }
    while(Get_FileType(ptk, Idx) == _A_SEP)
    {
        if(Move_Idx(ptk, &Idx, 1)) return;
    }
    if(Get_FileType(ptk, Idx) == _A_SUBDIR)
    {
        lt_index[ptk->Scopish] = Idx;
        return;
    }
    if(!Was_Dir)
    {
        Start_Letter = toupper(Get_FileName(ptk, Idx)[0]);
        if(Move_Idx(ptk, &Idx, 1)) return;
        Cur_Letter = toupper(Get_FileName(ptk, Idx)[0]);
        while(Start_Letter == Cur_Letter ||
              Get_FileType(ptk, Idx) == _A_SEP)
        {
            if(Move_Idx(ptk, &Idx, 1)) return;
            Cur_Letter = toupper(Get_FileName(ptk, Idx)[0]);
        }
    }
    lt_index[ptk->Scopish] = Idx;
}
