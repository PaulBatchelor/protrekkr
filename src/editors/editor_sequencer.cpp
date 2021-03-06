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
#include "editor_sequencer.h"
#include "editor_pattern.h"
#include "patterns_blocks.h"

#include "files.h"
#include "requesters.h"

// ------------------------------------------------------
// Structures
typedef struct
{
    int pattern;
    char active_state[16];
} SEQ_POS, *LPSEQ_POS;

// ------------------------------------------------------
// Variables
extern REQUESTER Overwrite_Requester;

int transpose_semitones;
int Cur_Seq_Buffer = 0;

char Selection_Name[20];

int Seq_Buffers_Full[4];

SEQ_POS Seq_Buffers[4] =
{
    { 0, { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, } },
    { 0, { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, } },
    { 0, { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, } },
    { 0, { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, } },
};

int cur_seq_buffer[] =
{
    BUTTON_PUSHED,
    BUTTON_NORMAL,
    BUTTON_NORMAL,
    BUTTON_NORMAL
};

int Remap_From;
int Remap_To;

int Ext_Pos_Switch;
int Ext_Track_Switch;

// ------------------------------------------------------
// Functions
void SeqFill(int st, int en, char n);
void SeqDelete(ptk_data *ptk, int st);
void SeqInsert(ptk_data *ptk, int st);
void SeqCopy(ptk_data *ptk, int st);
void Display_Seq_Buffer(void);
void SeqPaste(ptk_data *ptk, int st);
void Bound_Patt_Pos(ptk_data *ptk);

void Draw_Sequencer_Ed(ptk_data *ptk)
{
    Draw_Editors_Bar(ptk, USER_SCREEN_SEQUENCER);

    Gui_Draw_Button_Box(0, (Cur_Height - 153), ptk->fsize, 130, "", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Flat_Box("Sequencer");

    Gui_Draw_Button_Box(4, (Cur_Height - 134), 80, 16, "Clear All", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(4, (Cur_Height - 116), 80, 16, "Clear Position", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(4, (Cur_Height - 78), 80, 16, "Reset All", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(4, (Cur_Height - 60), 80, 16, "Reset Position", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

    Gui_Draw_Button_Box(396, (Cur_Height - 134), 32, 16, "Cut", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(396 + 34, (Cur_Height - 134), 32, 16, "Copy", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(396, (Cur_Height - 116), 66, 16, "Paste", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(396, (Cur_Height - 98), 66, 16, "Insert/Paste", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

    Display_Seq_Buffer();

    Gui_Draw_Button_Box(308, (Cur_Height - 134), 80, 16, "Ptn->Pos [Cur]", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(308, (Cur_Height - 116), 80, 16, "Ptn->Pos [Sng]", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(308, (Cur_Height - 78), 80, 16, "Insert Position", BUTTON_NORMAL | BUTTON_RIGHT_MOUSE | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(308, (Cur_Height - 60), 80, 16, "Delete Position", BUTTON_NORMAL | BUTTON_RIGHT_MOUSE | BUTTON_TEXT_CENTERED);

    Gui_Draw_Button_Box(288, (Cur_Height - 134), 16, 16, "\07", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(288, (Cur_Height - 116), 16, 16, "\01", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(288, (Cur_Height - 78), 16, 16, "\02", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(288, (Cur_Height - 60), 16, 16, "\10", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

    Gui_Draw_Button_Box(89, (Cur_Height - 134), 24, 90, "", BUTTON_NORMAL);
    Gui_Draw_Button_Box(257, (Cur_Height - 134), 24, 90, "", BUTTON_NORMAL);
    Gui_Draw_Button_Box(120, (Cur_Height - 134), 132, 90, "", BUTTON_NORMAL);

    Gui_Draw_Button_Box(480, (Cur_Height - 134), 306, 28, "", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(480, (Cur_Height - 128), 190, 26, "Save selection :", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_NO_BORDER | BUTTON_TEXT_VTOP);
    Gui_Draw_Button_Box(745, (Cur_Height - 128), 34, 16, "Save", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);

    Gui_Draw_Button_Box(480, (Cur_Height - 99), 306, 64, "Remap Instrument", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_VTOP);
    Gui_Draw_Button_Box(720, (Cur_Height - 99), 60, 18, "Transpose", BUTTON_NO_BORDER | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(720, (Cur_Height - 56), 60, 18, "Semitones", BUTTON_NO_BORDER | BUTTON_DISABLED | BUTTON_TEXT_CENTERED);

    Gui_Draw_Button_Box(480, (Cur_Height - 77), 60, 26, "From", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_NO_BORDER | BUTTON_TEXT_VTOP);
    Gui_Draw_Button_Box(480, (Cur_Height - 56), 60, 26, "To", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_NO_BORDER | BUTTON_TEXT_VTOP);

    Gui_Draw_Button_Box(590, (Cur_Height - 76), 60, 16, "Selection", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(590, (Cur_Height - 56), 60, 16, "Track", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(652, (Cur_Height - 76), 60, 16, "Pattern", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
    Gui_Draw_Button_Box(652, (Cur_Height - 56), 60, 16, "Song", BUTTON_NORMAL | BUTTON_TEXT_CENTERED);
}

void Actualize_Seq_Ed(ptk_data *ptk, char gode)
{
    if(ptk->userscreen == USER_SCREEN_SEQUENCER)
    {
        SetColor(COL_BLACK);
        bjbox(91, (Cur_Height - 132), 21, 87);
        bjbox(122, (Cur_Height - 132), 129, 87);
        bjbox(259, (Cur_Height - 132), 21, 87);
        SetColor(COL_BACKGROUND);
        bjbox(91 + 1, (Cur_Height - 132) + 1, 21 - 2, 87 - 2);
        bjbox(122 + 1, (Cur_Height - 132) + 1, 129 - 2, 87 - 2);
        bjbox(259 + 1, (Cur_Height - 132) + 1, 21 - 2, 87 - 2);

        // Current pos bar
        SetColor(COL_PUSHED_MED);
        bjbox(123, (Cur_Height - 95), 127, 12);

        for(int lseq = -3; lseq < 4; lseq++)
        {
            int rel;
            int Cur_Position = Get_Song_Position(ptk);
            rel = lseq + Cur_Position;
            if(rel > -1 && rel < ptk->Song_Length)
            {
                out_decchar(93, (Cur_Height - 95) + lseq * 12, rel, 0);
                out_decchar(261, (Cur_Height - 95) + lseq * 12, ptk->pSequence[rel], 0);

                for(int rel2 = 0; rel2 < Songtracks; rel2++)
                {
                    if(CHAN_ACTIVE_STATE[rel][rel2]) out_nibble(124 + rel2 * 8, (Cur_Height - 95) + lseq * 12, USE_FONT, rel2);
                    else out_nibble(124 + rel2 * 8, (Cur_Height - 95) + lseq * 12, USE_FONT_LOW, rel2);
                } // sub for
            }
            else
            { // rel range OK
                PrintString(93, (Cur_Height - 95) + lseq * 12, USE_FONT, "000");
                PrintString(261, (Cur_Height - 95) + lseq * 12, USE_FONT, "000");
            }
        } // for end
        Actupated(ptk, 0);

        // From instrument
        if(gode == 0 || gode == 1)
        {
            if(Remap_From < 0) Remap_From = 0;
            if(Remap_From > 0x7f) Remap_From = 0x7f;
            value_box(520, (Cur_Height - 76), Remap_From, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
        }

        // To instrument
        if(gode == 0 || gode == 2)
        {
            if(Remap_To < 0) Remap_To = 0;
            if(Remap_To > 0x7f) Remap_To = 0x7f;
            value_box(520, (Cur_Height - 56), Remap_To, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
        }

        if(gode == 0 || gode == 3)
        {
            char tcp[30];
            sprintf(tcp, "%s_", Selection_Name);

            if(ptk->snamesel == INPUT_SELECTION_NAME)
            {
                Gui_Draw_Button_Box(579, (Cur_Height - 128), 164, 16, tcp, BUTTON_PUSHED | BUTTON_INPUT);
            }
            else
            {
                Gui_Draw_Button_Box(579, (Cur_Height - 128), 164, 16, Selection_Name, BUTTON_NORMAL | BUTTON_INPUT);
            }
        }

        // Transpose
        if(gode == 0 || gode == 4)
        {
            if(transpose_semitones < -120) transpose_semitones = -120;
            if(transpose_semitones > 120) transpose_semitones = 120;
            value_box_format(720, (Cur_Height - 76), transpose_semitones, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE, "%d");
        }
    }
}

void Mouse_Left_Sequencer_Ed(ptk_data *ptk)
{
    int i;
    int j;
    int k;
    int Cur_Position = Get_Song_Position(ptk);

    if(ptk->userscreen == USER_SCREEN_SEQUENCER)
    {
        // Remap Selection
        if(zcheckMouse(ptk, 590, (Cur_Height - 76), 60, 16))
        {
            if(transpose_semitones)
            {
                if(transpose_semitones > 0)
                {
                    for(k = 0; k < transpose_semitones; k++)
                    {
                        Instrument_Semitone_Up_Sel(ptk, Cur_Position, Get_Real_Selection(ptk, FALSE), 1, Remap_From);
                    }
                }
                else
                {
                    for(k = 0; k < -transpose_semitones; k++)
                    {
                        Instrument_Semitone_Down_Sel(ptk, Cur_Position, Get_Real_Selection(ptk, FALSE), 1, Remap_From);
                    }
                }
            }
            Instrument_Remap_Sel(ptk, Cur_Position, Get_Real_Selection(ptk, FALSE), Remap_From, Remap_To);
        }
        // Remap Track
        if(zcheckMouse(ptk, 590, (Cur_Height - 56), 60, 16))
        {
            if(transpose_semitones)
            {
                if(transpose_semitones > 0)
                {
                    for(k = 0; k < transpose_semitones; k++)
                    {
                        Instrument_Semitone_Up_Sel(ptk, Cur_Position, Select_Track(ptk, ptk->Track_Under_Caret), 1, Remap_From);
                    }
                }
                else
                {
                    for(k = 0; k < -transpose_semitones; k++)
                    {
                        Instrument_Semitone_Down_Sel(ptk, Cur_Position, Select_Track(ptk, ptk->Track_Under_Caret), 1, Remap_From);
                    }
                }
            }
            Instrument_Remap_Sel(ptk, Cur_Position, Select_Track(ptk, ptk->Track_Under_Caret), Remap_From, Remap_To);
        }
        // Remap Pattern
        if(zcheckMouse(ptk, 652, (Cur_Height - 76), 60, 16))
        {
            for(i = 0; i < Songtracks; i++)
            {
                if(transpose_semitones)
                {
                    if(transpose_semitones > 0)
                    {
                        for(k = 0; k < transpose_semitones; k++)
                        {
                            Instrument_Semitone_Up_Sel(ptk, Cur_Position, Select_Track(ptk, i), 1, Remap_From);
                        }
                    }
                    else
                    {
                        for(k = 0; k < -transpose_semitones; k++)
                        {
                            Instrument_Semitone_Down_Sel(ptk, Cur_Position, Select_Track(ptk, i), 1, Remap_From);
                        }
                    }
                }
                Instrument_Remap_Sel(ptk, Cur_Position, Select_Track(ptk, i), Remap_From, Remap_To);
            }
        }
        // Remap Song
        if(zcheckMouse(ptk, 652, (Cur_Height - 56), 60, 16))
        {
            char *Done_Pattern;
            int nbr_patterns;

            // We need an array to mark the patterns we already transposed
            // otherwise we could transpose them several times and that's not what we want.
            nbr_patterns = 0;
            for(i = 0; i < ptk->Song_Length; i++)
            {
                if(ptk->pSequence[i] > nbr_patterns)
                {
                    nbr_patterns = ptk->pSequence[i];
                }
            }
            Done_Pattern = (char *) malloc(nbr_patterns + 1);
            memset(Done_Pattern, 0, nbr_patterns + 1);
            if(Done_Pattern)
            {
                for(j = 0; j < ptk->Song_Length; j++)
                {
                    if(!Done_Pattern[ptk->pSequence[j]])
                    {
                        for(i = 0; i < Songtracks; i++)
                        {
                            if(transpose_semitones)
                            {
                                if(transpose_semitones > 0)
                                {
                                    for(k = 0; k < transpose_semitones; k++)
                                    {
                                        Instrument_Semitone_Up_Sel(ptk, j, Select_Track(ptk, i), 1, Remap_From);
                                    }
                                }
                                else
                                {
                                    for(k = 0; k < -transpose_semitones; k++)
                                    {
                                        Instrument_Semitone_Down_Sel(ptk, j, Select_Track(ptk, i), 1, Remap_From);
                                    }
                                }
                            }
                            Instrument_Remap_Sel(ptk, j, Select_Track(ptk, i), Remap_From, Remap_To);
                        }
                        Done_Pattern[ptk->pSequence[j]] = TRUE;
                    }
                }
                free(Done_Pattern);
            }
        }

        // From Instrument
        if(zcheckMouse(ptk, 520, (Cur_Height - 76), 16, 16) == 1)
        {
            Remap_From--;
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
            ptk->teac = 1;
        }

        // From Instrument
        if(zcheckMouse(ptk, 520 + 44, (Cur_Height - 76), 16, 16) == 1)
        {
            Remap_From++;
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
            ptk->teac = 1;
        }

        // To Instrument
        if(zcheckMouse(ptk, 520, (Cur_Height - 56), 16, 16) == 1)
        {
            Remap_To--;
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
            ptk->teac = 1;
        }

        // To Instrument
        if(zcheckMouse(ptk, 520 + 44, (Cur_Height - 56), 16, 16) == 1)
        {
            Remap_To++;
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
            ptk->teac = 1;
        }

        // Transpose
        if(zcheckMouse(ptk, 720, (Cur_Height - 76), 16, 16) == 1)
        {
            transpose_semitones--;
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
            ptk->teac = 4;
        }

        // Transpose
        if(zcheckMouse(ptk, 720 + 44, (Cur_Height - 76), 16, 16) == 1)
        {
            transpose_semitones++;
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
            ptk->teac = 4;
        }

        // Clear all
        if(zcheckMouse(ptk, 4, (Cur_Height - 134), 80, 16))
        {
            SeqFill(0, 256, FALSE);
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Clear position
        if(zcheckMouse(ptk, 4, (Cur_Height - 116), 80, 16))
        {
            SeqFill(Cur_Position, Cur_Position + 1, FALSE);
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Reset all
        if(zcheckMouse(ptk, 4, (Cur_Height - 78), 80, 16))
        {
            SeqFill(0, 256, TRUE);
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Reset position
        if(zcheckMouse(ptk, 4, (Cur_Height - 60), 80, 16))
        {
            SeqFill(Cur_Position, Cur_Position + 1, TRUE);
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Insert position
        if(zcheckMouse(ptk, 308, (Cur_Height - 78), 80, 16))
        {
            SeqInsert(ptk, Cur_Position);
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Delete position
        if(zcheckMouse(ptk, 308, (Cur_Height - 60), 80, 16))
        {
            SeqDelete(ptk, Cur_Position);
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Select the copy buffers
        if(zcheckMouse(ptk, 396, (Cur_Height - 78), 15, 16))
        {
            cur_seq_buffer[0] = BUTTON_PUSHED;
            cur_seq_buffer[1] = BUTTON_NORMAL;
            cur_seq_buffer[2] = BUTTON_NORMAL;
            cur_seq_buffer[3] = BUTTON_NORMAL;
            Display_Seq_Buffer();
            Cur_Seq_Buffer = 0;
        }
        if(zcheckMouse(ptk, 396 + 17, (Cur_Height - 78), 15, 16))
        {
            cur_seq_buffer[0] = BUTTON_NORMAL;
            cur_seq_buffer[1] = BUTTON_PUSHED;
            cur_seq_buffer[2] = BUTTON_NORMAL;
            cur_seq_buffer[3] = BUTTON_NORMAL;
            Display_Seq_Buffer();
            Cur_Seq_Buffer = 1;
        }
        if(zcheckMouse(ptk, 396 + (17 * 2), (Cur_Height - 78), 15, 16))
        {
            cur_seq_buffer[0] = BUTTON_NORMAL;
            cur_seq_buffer[1] = BUTTON_NORMAL;
            cur_seq_buffer[2] = BUTTON_PUSHED;
            cur_seq_buffer[3] = BUTTON_NORMAL;
            Display_Seq_Buffer();
            Cur_Seq_Buffer = 2;
        }
        if(zcheckMouse(ptk, 396 + (17 * 3), (Cur_Height - 78), 15, 16))
        {
            cur_seq_buffer[0] = BUTTON_NORMAL;
            cur_seq_buffer[1] = BUTTON_NORMAL;
            cur_seq_buffer[2] = BUTTON_NORMAL;
            cur_seq_buffer[3] = BUTTON_PUSHED;
            Display_Seq_Buffer();
            Cur_Seq_Buffer = 3;
        }

        // Cut
        if(zcheckMouse(ptk, 396, (Cur_Height - 134), 32, 16))
        {
            SeqCopy(ptk, Cur_Position);
            SeqDelete(ptk, Cur_Position);
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Copy
        if(zcheckMouse(ptk, 396 + 34, (Cur_Height - 134), 32, 16))
        {
            SeqCopy(ptk, Cur_Position);
        }

        // Paste
        if(zcheckMouse(ptk, 396, (Cur_Height - 116), 66, 16))
        {
            SeqPaste(ptk, Cur_Position);
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Insert/Paste
        if(zcheckMouse(ptk, 396, (Cur_Height - 98), 66, 16))
        {
            SeqInsert(ptk, Cur_Position);
            SeqPaste(ptk, Cur_Position);
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Ptn->Pos[Cur]
        if(zcheckMouse(ptk, 308, (Cur_Height - 134), 80, 16))
        {
            if(Cur_Position < 128)
            {
                ptk->pSequence[Cur_Position] = Cur_Position;
                Anat(ptk, Cur_Position);
                ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
            }
        }

        // Ptn->Pos[Sng]
        if(zcheckMouse(ptk, 308, (Cur_Height - 116), 80, 16))
        {
            for(int xpos = 0; xpos < 128; xpos++)
            {
                ptk->pSequence[xpos] = xpos;
            }
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Up 10 pos
        if(zcheckMouse(ptk, 288, (Cur_Height - 134), 16, 16))
        {
            ptk->gui_action = GUI_CMD_REDUCE_POSITIONS_10;
        }
        // Up 1 pos
        if(zcheckMouse(ptk, 288, (Cur_Height - 116), 16, 16))
        {
            ptk->gui_action = GUI_CMD_PREVIOUS_POSITION;
        }
        // Down 1 pos
        if(zcheckMouse(ptk, 288, (Cur_Height - 78), 16, 16)){
            ptk->gui_action = GUI_CMD_NEXT_POSITION;
        }
        // Down 10 pos
        if(zcheckMouse(ptk, 288, (Cur_Height - 60), 16, 16))
        {
            ptk->gui_action = GUI_CMD_INCREASE_POSITIONS_10;
        }

        // Add 100 to the selected pattern
        if(zcheckMouse(ptk, 260, (Cur_Height - 132), 7, 84))
        {
            int posindex = ((Mouse.y - ((Cur_Height - 132) + 1)) / 12) - 3;
            posindex += Cur_Position;
            if(posindex >= 0 && posindex < ptk->Song_Length)
            {
                if(ptk->pSequence[posindex] < 127)
                {
                    ptk->pSequence[posindex] += 100;   
                    if(ptk->pSequence[posindex] >= 128) ptk->pSequence[posindex] = 127;
                    Anat(ptk, posindex);

                    if(posindex != Cur_Position) ptk->gui_action = GUI_CMD_UPDATE_SEQ_ED;
                    else ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;

                }
                else
                {
                    ptk->gui_action = GUI_CMD_PATTERNS_POOL_EXHAUSTED;
                }
            }
        }

        // Add 10 to the selected pattern
        if(zcheckMouse(ptk, 266, (Cur_Height - 132), 7, 84))
        {
            int posindex = ((Mouse.y - ((Cur_Height - 132) + 1)) / 12) - 3;
            posindex += Cur_Position;
            if(posindex >= 0 && posindex < ptk->Song_Length)
            {
                if(ptk->pSequence[posindex] < 127)
                {
                    ptk->pSequence[posindex] += 10; 
                    if(ptk->pSequence[posindex] >= 128) ptk->pSequence[posindex] = 127;
                    Anat(ptk, posindex);

                    if(posindex != Cur_Position) ptk->gui_action = GUI_CMD_UPDATE_SEQ_ED;
                    else ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;

                }
                else
                {
                    ptk->gui_action = GUI_CMD_PATTERNS_POOL_EXHAUSTED;
                }
            }
        }

        // Add 1 to the selected pattern
        if(zcheckMouse(ptk, 272, (Cur_Height - 132), 7, 84))
        {
            int posindex = ((Mouse.y - ((Cur_Height - 132) + 1)) / 12) - 3;
            posindex += Cur_Position;
            if(posindex >= 0 && posindex < ptk->Song_Length)
            {
                if(ptk->pSequence[posindex] < 127)
                {
                    ptk->pSequence[posindex]++;
                    Anat(ptk, posindex);
                    if(posindex != Cur_Position) ptk->gui_action = GUI_CMD_UPDATE_SEQ_ED;
                    else ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
                }
                else
                {
                    ptk->gui_action = GUI_CMD_PATTERNS_POOL_EXHAUSTED;
                }
            }
        }

        // Enable/Disable channels
        if(zcheckMouse(ptk, 123, (Cur_Height - 131), 129, 84) == 1)
        {
            int posindex = ((Mouse.y - ((Cur_Height - 131) + 1)) / 12) - 3;
            posindex += Cur_Position;
            Toggle_Track_On_Off_Status(ptk, posindex, (Mouse.x - 123) / 8);
        }

        // Scroll the positions
        if(zcheckMouse(ptk, 89, (Cur_Height - 131), 24, 84))
        {
            int posindex = ((Mouse.y - (Cur_Height - 131)) / 12) - 3;
            posindex += Cur_Position;
            if(posindex >= 0 && posindex < ptk->Song_Length && posindex != Cur_Position)
            {
                Song_Position = posindex;
                ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
            }
        }

        // Start selection name input
        if(zcheckMouse(ptk, 579, (Cur_Height - 128), 164, 16) && ptk->snamesel == INPUT_NONE)
        {
            ptk->snamesel = INPUT_SELECTION_NAME;
            strcpy(ptk->cur_input_name, Selection_Name);
            ptk->namesize = 0;
            sprintf(Selection_Name, "");
            ptk->teac = 3;
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }
        
        // Save the data
        if(zcheckMouse(ptk, 745, (Cur_Height - 128), 34, 16))
        {
            if(File_Exist_Req(ptk, "%s"SLASH"%s.ppb", Dir_Patterns, Selection_Name))
            {
                Display_Requester(ptk, &Overwrite_Requester, GUI_CMD_SAVE_PATTERN);
            }
            else
            {
                ptk->gui_action = GUI_CMD_SAVE_PATTERN;
            }
        }
    }
}

void Mouse_Right_Sequencer_Ed(ptk_data *ptk)
{
    int Cur_Position = Get_Song_Position(ptk);
    int i;

    if(ptk->userscreen == USER_SCREEN_SEQUENCER)
    {
        // From Instrument
        if(zcheckMouse(ptk, 520, (Cur_Height - 76), 16, 16) == 1)
        {
            Remap_From -= 10;
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
            ptk->teac = 1;
        }

        // From Instrument
        if(zcheckMouse(ptk, 520 + 44, (Cur_Height - 76), 16, 16) == 1)
        {
            Remap_From += 10;
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
            ptk->teac = 1;
        }

        // To Instrument
        if(zcheckMouse(ptk, 520, (Cur_Height - 56), 16, 16) == 1)
        {
            Remap_To -= 10;
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
            ptk->teac = 1;
        }

        // To Instrument
        if(zcheckMouse(ptk, 520 + 44, (Cur_Height - 56), 16, 16) == 1)
        {
            Remap_To += 10;
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
            ptk->teac = 1;
        }

        // Sub 100 to the selected pattern
        if(zcheckMouse(ptk, 260, (Cur_Height - 132), 7, 84))
        {
            int posindex = ((Mouse.y - ((Cur_Height - 132) + 1)) / 12) - 3;
            posindex += Cur_Position;
            if(posindex >= 0 && posindex < ptk->Song_Length)
            {
                int reak = ptk->pSequence[posindex];
                reak -= 100;
                if(reak < 0) reak = 0;
                ptk->pSequence[posindex] = reak;
                Anat(ptk, posindex);
                if(posindex != Cur_Position) ptk->gui_action = GUI_CMD_UPDATE_SEQ_ED;
                else ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
            }
        }

        // Sub 10 to the selected pattern
        if(zcheckMouse(ptk, 266, (Cur_Height - 132), 7, 84))
        {
            int posindex = ((Mouse.y - ((Cur_Height - 132) + 1)) / 12) - 3;
            posindex += Cur_Position;
            if(posindex >= 0 && posindex < ptk->Song_Length)
            {
                int reak = ptk->pSequence[posindex];
                reak -= 10;
                if(reak < 0) reak = 0;
                ptk->pSequence[posindex] = reak;
                Anat(ptk, posindex);

                if(posindex != Cur_Position) ptk->gui_action = GUI_CMD_UPDATE_SEQ_ED;
                else ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
            }
        }

        // Sub 1 to the selected pattern
        if(zcheckMouse(ptk, 272, (Cur_Height - 132), 7, 84))
        {
            int posindex = ((Mouse.y - ((Cur_Height - 132) + 1)) / 12) - 3;
            posindex += Cur_Position;
            if(posindex >= 0 && posindex < ptk->Song_Length)
            {
                if(ptk->pSequence[posindex] > 0)
                {
                    ptk->pSequence[posindex]--;
                    Anat(ptk, posindex);

                    if(posindex != Cur_Position) ptk->gui_action = GUI_CMD_UPDATE_SEQ_ED;
                    else ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
                }
            }
        }

        // Solo a track
        if(zcheckMouse(ptk, 123, (Cur_Height - 131), 129, 84) == 1)
        {
            int posindex = ((Mouse.y - ((Cur_Height - 131) + 1)) / 12) - 3;
            posindex += Cur_Position;
            Solo_Track_On_Off(ptk, posindex, (Mouse.x - 123) / 8);
        }

        // Insert 10 positions
        if(zcheckMouse(ptk, 308, (Cur_Height - 78), 80, 16))
        {
            for(i = 0; i < 10; i++)
            {
                SeqInsert(ptk, Cur_Position);
            }
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Delete 10 positions
        if(zcheckMouse(ptk, 308, (Cur_Height - 60), 80, 16))
        {
            for(i = 0; i < 10; i++)
            {
                SeqDelete(ptk, Cur_Position);
            }
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
        }

        // Transpose
        if(zcheckMouse(ptk, 720, (Cur_Height - 76), 16, 16) == 1)
        {
            transpose_semitones -= 12;
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
            ptk->teac = 4;
        }

        // Transpose
        if(zcheckMouse(ptk, 720 + 44, (Cur_Height - 76), 16, 16) == 1)
        {
            transpose_semitones += 12;
            ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
            ptk->teac = 4;
        }
    }
}

void Actualize_Sequencer(ptk_data *ptk)
{
    int value;
    int Cur_Position;
    int i;

    if(Songplaying)
    {
        if(Song_Position < 0) Song_Position = 0;
        if(Song_Position > ptk->Song_Length - 1)
        {
            Song_Position = ptk->Song_Length - 1;
            Bound_Patt_Pos(ptk);
            Actupated(ptk, 0);
        }
        for(i = 0; i < MAX_TRACKS; i++)
        {
            CHAN_HISTORY_STATE[Song_Position][i] = FALSE;
        }
    }
    else
    {
        if(Song_Position < 0) Song_Position = 0;
        if(Song_Position > ptk->Song_Length - 1)
        {
            Song_Position = ptk->Song_Length - 1;
            Bound_Patt_Pos(ptk);
            Actupated(ptk, 0);
        }
        // Keep the coherency
        Song_Position_Visual = Song_Position;
        Pattern_Line_Visual = Pattern_Line;
    }
    Cur_Position = Get_Song_Position(ptk);

    value = ptk->pSequence[Cur_Position];
    if(value > 127) ptk->pSequence[Cur_Position] = 127;
    if(value < 0) ptk->pSequence[Cur_Position] = 0;

    Gui_Draw_Arrows_Number_Box(188, 28, Cur_Position, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
    Gui_Draw_Arrows_Number_Box(188, 46, ptk->pSequence[Cur_Position], BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
    Anat(ptk, Cur_Position);
    if(ptk->Rows_Decimal) Gui_Draw_Arrows_Number_Box(188, 82, ptk->patternLines[ptk->pSequence[Cur_Position]], BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
    else value_box(188, 82, ptk->patternLines[ptk->pSequence[Cur_Position]], BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
    Gui_Draw_Arrows_Number_Box(188, 64, ptk->Song_Length, BUTTON_NORMAL | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
    if(ptk->userscreen == USER_SCREEN_SEQUENCER) Actualize_Seq_Ed(ptk, 0);
}

void SeqFill(int st, int en, char n)
{
    for(int cl = st; cl < en; cl++)
    {
        for(char trk = 0; trk < Songtracks; trk++)
        {
            CHAN_ACTIVE_STATE[cl][trk] = n;
            CHAN_HISTORY_STATE[cl][trk] = FALSE;
        }
    }
}     

// ------------------------------------------------------
// Delete a position
void SeqDelete(ptk_data *ptk, int st)
{
    int cl;

    if(ptk->Song_Length > 1)
    {
        for(cl = st; cl < ptk->Song_Length - 1; cl++)
        {
            ptk->pSequence[cl] = ptk->pSequence[cl + 1];
            for(char trk = 0; trk < Songtracks; trk++)
            {
                CHAN_ACTIVE_STATE[cl][trk] = CHAN_ACTIVE_STATE[cl + 1][trk];
                CHAN_HISTORY_STATE[cl][trk] = CHAN_HISTORY_STATE[cl + 1][trk];
            }
        }
        ptk->pSequence[cl] = 0;
        for(char trk = 0; trk < Songtracks; trk++)
        {
            CHAN_ACTIVE_STATE[cl][trk] = TRUE;
            CHAN_HISTORY_STATE[cl][trk] = FALSE;
        }
        ptk->Song_Length--;
    }
}     

// ------------------------------------------------------
// Insert a position
void SeqInsert(ptk_data *ptk, int st)
{
    int cl;

    if(ptk->Song_Length < 255)
    {
        for(cl = ptk->Song_Length - 1; cl >= st; cl--)
        {
            ptk->pSequence[cl + 1] = ptk->pSequence[cl];
            for(char trk = 0; trk < Songtracks; trk++)
            {
                CHAN_ACTIVE_STATE[cl + 1][trk] = CHAN_ACTIVE_STATE[cl][trk];
                CHAN_HISTORY_STATE[cl + 1][trk] = CHAN_HISTORY_STATE[cl][trk];
            }
        }
        ptk->pSequence[st] = 0;
        for(char trk = 0; trk < Songtracks; trk++)
        {
            CHAN_ACTIVE_STATE[st][trk] = TRUE;
            CHAN_HISTORY_STATE[st][trk] = FALSE;
        }
    ptk->Song_Length++;
    }
}     

void Anat(ptk_data *ptk, int posil)
{
    if(ptk->pSequence[posil] >= ptk->nPatterns)
    {
        ptk->nPatterns = ptk->pSequence[posil] + 1;
    }
}

// ------------------------------------------------------
// Copy a position
void SeqCopy(ptk_data *ptk, int st)
{
    Seq_Buffers_Full[Cur_Seq_Buffer] = TRUE;
    Seq_Buffers[Cur_Seq_Buffer].pattern = ptk->pSequence[st];
    for(char trk = 0; trk < Songtracks; trk++)
    {
        Seq_Buffers[Cur_Seq_Buffer].active_state[trk] = CHAN_ACTIVE_STATE[st][trk];
    }
    Display_Seq_Buffer();
}     

// ------------------------------------------------------
// Paste a position
void SeqPaste(ptk_data *ptk, int st)
{
    ptk->pSequence[st] = Seq_Buffers[Cur_Seq_Buffer].pattern;
    for(char trk = 0; trk < Songtracks; trk++)
    {
        CHAN_ACTIVE_STATE[st][trk] = Seq_Buffers[Cur_Seq_Buffer].active_state[trk];
        CHAN_HISTORY_STATE[st][trk] = FALSE;
    }
}     

// ------------------------------------------------------
// Notify the user selected buffer visually
void Display_Seq_Buffer(void)
{
    Gui_Draw_Button_Box(396, (Cur_Height - 78), 15, 16, "1", cur_seq_buffer[0] | BUTTON_TEXT_CENTERED | (Seq_Buffers_Full[0] ? 0 : BUTTON_LOW_FONT));
    Gui_Draw_Button_Box(396 + 17, (Cur_Height - 78), 15, 16, "2", cur_seq_buffer[1] | BUTTON_TEXT_CENTERED | (Seq_Buffers_Full[1] ? 0 : BUTTON_LOW_FONT));
    Gui_Draw_Button_Box(396 + (17 * 2), (Cur_Height - 78), 15, 16, "3", cur_seq_buffer[2] | BUTTON_TEXT_CENTERED | (Seq_Buffers_Full[2] ? 0 : BUTTON_LOW_FONT));
    Gui_Draw_Button_Box(396 + (17 * 3), (Cur_Height - 78), 15, 16, "4", cur_seq_buffer[3] | BUTTON_TEXT_CENTERED | (Seq_Buffers_Full[3] ? 0 : BUTTON_LOW_FONT));
}

// ------------------------------------------------------
// Turn a channel active state on/off
void Toggle_Track_On_Off_Status(ptk_data *ptk, int posindex, int seqindex)
{
    if(posindex >= 0 && posindex < ptk->Song_Length)
    {
        if(seqindex < 0) seqindex = 0;
        if(seqindex > Songtracks - 1) seqindex = Songtracks - 1;
        if(!CHAN_ACTIVE_STATE[posindex][seqindex])
        {
            CHAN_ACTIVE_STATE[posindex][seqindex] = TRUE;
            CHAN_HISTORY_STATE[posindex][seqindex] = FALSE;
        }
        else
        {
            CHAN_ACTIVE_STATE[posindex][seqindex] = FALSE;
            CHAN_HISTORY_STATE[posindex][seqindex] = FALSE;
        }
        Actupated(ptk, 0);
        ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
    }
}

// ------------------------------------------------------
// Turn all channel active states on/off but one
void Solo_Track_On_Off(ptk_data *ptk, int posindex, int seqindex)
{
    int Already_Solo;

    if(posindex >= 0 && posindex < ptk->Song_Length)
    {
        if(seqindex < 0) seqindex = 0;
        if(seqindex > Songtracks - 1) seqindex = Songtracks - 1;

        if(CHAN_ACTIVE_STATE[posindex][seqindex])
        {
            // Check if it was the only track turned on
            Already_Solo = 0;
            for(int alphac = 0; alphac < Songtracks; alphac++)
            {
                if(CHAN_ACTIVE_STATE[posindex][alphac] == TRUE) Already_Solo++;
            }
            if(Already_Solo == 1)
            {
                // Was already soloed: turn'em all on
                for(int alphac = 0; alphac < Songtracks; alphac++)
                {
                    CHAN_ACTIVE_STATE[posindex][alphac] = TRUE;
                    CHAN_HISTORY_STATE[posindex][alphac] = FALSE;
                }
            }
            else
            {
                // Solo it
                for(int alphac = 0; alphac < Songtracks; alphac++)
                {
                    CHAN_ACTIVE_STATE[posindex][alphac] = FALSE;
                    CHAN_HISTORY_STATE[posindex][alphac] = FALSE;
                }
            }
        }
        else
        {
            // Solo it
            for(int alphac = 0; alphac < Songtracks; alphac++)
            {
                CHAN_ACTIVE_STATE[posindex][alphac] = FALSE;
                CHAN_HISTORY_STATE[posindex][alphac] = FALSE;
            }
        }
        // Active it
        CHAN_ACTIVE_STATE[posindex][seqindex] = TRUE;
        CHAN_HISTORY_STATE[posindex][seqindex] = FALSE;
        Actupated(ptk, 0);
        ptk->gui_action = GUI_CMD_UPDATE_SEQUENCER;
    }
}
