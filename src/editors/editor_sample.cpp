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
#include "editor_sample.h"
#include "editor_instrument.h"
#include "samples_ops.h"

#if defined(__AROS__)
#include <stdint.h>
#define int32 int32_t
#endif

// ------------------------------------------------------
// Constants
#define WAVE_LEFT 2
#define SAMPLE_LINES_HEIGHT 109
#define SAMPLE_HEIGHT SAMPLE_LINES_HEIGHT

#define LARGE_SMP_VIEW 512

#define SMPED_CUT 20
#define SMPED_COPY (SMPED_CUT + 1)
#define SMPED_PASTE (SMPED_COPY + 1)
#define SMPED_DC_ADJUST (SMPED_PASTE + 1)
#define SMPED_MAXIMIZE (SMPED_DC_ADJUST + 1)
#define SMPED_FADEIN (SMPED_MAXIMIZE + 1)
#define SMPED_FADEOUT (SMPED_FADEIN + 1)
#define SMPED_HALF (SMPED_FADEOUT + 1)
#define SMPED_BUF1 (SMPED_HALF + 1)
#define SMPED_BUF2 (SMPED_BUF1 + 1)
#define SMPED_BUF3 (SMPED_BUF2 + 1)
#define SMPED_BUF4 (SMPED_BUF3 + 1)
#define SMPED_ZAP (SMPED_BUF4 + 1)
#define SMPED_CROP (SMPED_ZAP + 1)
#define SMPED_REVERSE (SMPED_CROP + 1)
#define SMPED_ROTATE_LEFT_1 (SMPED_REVERSE + 1)
#define SMPED_ROTATE_LEFT_X (SMPED_ROTATE_LEFT_1 + 1)
#define SMPED_ROTATE_RIGHT_1 (SMPED_ROTATE_LEFT_X + 1)
#define SMPED_ROTATE_RIGHT_X (SMPED_ROTATE_RIGHT_1 + 1)
#define SMPED_ZEROIZE (SMPED_ROTATE_RIGHT_X + 1)
#define SMPED_DUPLICATE (SMPED_ZEROIZE + 1)
#define SMPED_INSERTZERO (SMPED_DUPLICATE + 1)

// ------------------------------------------------------
// Variables
extern s_access sp_Position[MAX_TRACKS][MAX_POLYPHONY];

int32 axswave = 0;

int32 sed_display_start = 0;
int32 sed_display_length = 0;
int32 sed_range_start = 0;
int32 sed_range_end = 0;
char sed_range_mode = FALSE;

int cur_smp_buffer[] =
{
    BUTTON_PUSHED,
    BUTTON_NORMAL,
    BUTTON_NORMAL,
    BUTTON_NORMAL
};

// ------------------------------------------------------
// Functions
void Draw_Wave_PlayBack_Pos(ptk_data *ptk);
void Draw_Wave_Data3(void);
void Refresh_Sample(ptk_data *ptk, int clear_sel);
void Display_Sample_Buffers(ptk_data *ptk, int Allow);
void Zoom_In_Sel(ptk_data *ptk);
void Zoom_Out_Sel(ptk_data *ptk);

void Draw_Sample_Ed(ptk_data *ptk)
{
    Draw_Editors_Bar(ptk, USER_SCREEN_SAMPLE_EDIT);

    Gui_Draw_Button_Box(0, (Cur_Height - 153), ptk->fsize, 130, "", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Flat_Box("");

    Gui_Draw_Button_Box(650, (Cur_Height - 78), 60, 16, "Range", BUTTON_NORMAL | BUTTON_DISABLED);
    Gui_Draw_Button_Box(712, (Cur_Height - 78), 60, 16, "View", BUTTON_NORMAL | BUTTON_DISABLED);
        
    ptk->draw_sampled_wave = TRUE;
    Actualize_Sample_Ed(ptk, 0);
}

void Draw_Wave_Data(ptk_data *ptk)
{
    int32 sed_real_range_start;
    int32 sed_real_range_end;
    int32 s_offset;
    int32 s_ex;
    int start_rect;
    int set_start_rect;
    int end_rect;
    int64 pos_in_wav;

    if(ptk->userscreen == USER_SCREEN_SAMPLE_EDIT)
    {
        sed_real_range_start = sed_range_start;
        sed_real_range_end = sed_range_end;

        if(sed_real_range_start > sed_real_range_end)
        {
            int Swap_Range = sed_real_range_start;
            sed_real_range_start = sed_real_range_end;
            sed_real_range_end = Swap_Range;
        }

        if(ptk->draw_sampled_wave)
        {
            // Redraw the bottom slider
            Realslider_Horiz(WAVE_LEFT + 18, (Cur_Height - 41), sed_display_start, sed_display_length,
                             SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split],
                             LARGE_SMP_VIEW + 2 - (18 * 2), TRUE);

            // Layout around the waveform
            SetColor(COL_BACKGROUND);
            Fillrect(WAVE_LEFT, (Cur_Height - 150), LARGE_SMP_VIEW + WAVE_LEFT + 1, (Cur_Height - 150) + SAMPLE_HEIGHT);
            DrawHLine((Cur_Height - 150), WAVE_LEFT, LARGE_SMP_VIEW + WAVE_LEFT + 1, COL_BLACK);
            DrawHLine((Cur_Height - 150) + SAMPLE_HEIGHT - 1, WAVE_LEFT, LARGE_SMP_VIEW + WAVE_LEFT + 1, COL_BLACK);
            DrawVLine(LARGE_SMP_VIEW + WAVE_LEFT + 1, (Cur_Height - 150), (Cur_Height - 150) + SAMPLE_HEIGHT - 1, COL_BLACK);
            DrawVLine(WAVE_LEFT, (Cur_Height - 150), (Cur_Height - 150) + SAMPLE_HEIGHT - 1, COL_BLACK);

            if(SampleType[ptk->Current_Instrument][ptk->Current_Instrument_Split] > 0)
            {   
                // Any Sample Out There?
                int strober = SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split] * 2;
                int s_ey;
                int s_ey2;
                int s_size = 0;
                int s_coef;

                if(strober)
                {
                    s_ey = (Cur_Height - 150) + 1 + (SAMPLE_LINES_HEIGHT / strober);
                    s_ey2 = s_ey + ((SAMPLE_LINES_HEIGHT / strober) * 2);
                    if(s_ey2 > ((Cur_Height - 150) + SAMPLE_HEIGHT - 2)) s_ey2 = (Cur_Height - 150) + SAMPLE_HEIGHT - 2;
                    if(s_ey > ((Cur_Height - 150) + SAMPLE_HEIGHT - 2)) s_ey = (Cur_Height - 150) + SAMPLE_HEIGHT - 2;
                    s_size = SAMPLE_LINES_HEIGHT / strober;
                    s_coef = 32768 / s_size;

                    if(ptk->rs_coef > s_coef) ptk->rs_coef = s_coef;
                    if(ptk->rs_coef < 1) ptk->rs_coef = 1;
                }

                // Draw a selection rect if there's any
                start_rect = 0;
                end_rect = LARGE_SMP_VIEW;
                set_start_rect = 0;
                for(s_ex = 0; s_ex < LARGE_SMP_VIEW; s_ex++)
                {
                    pos_in_wav = ((int64) s_ex * (int64) sed_display_length);
                    s_offset = (int) (pos_in_wav / LARGE_SMP_VIEW) + sed_display_start;

                    if(sed_range_mode &&
                       s_offset >= sed_real_range_start)
                    {
                        if(!set_start_rect)
                        {
                            // Record the start position
                            start_rect = s_ex;
                            set_start_rect = 1;
                        }
                    }
                    if(sed_range_mode &&
                       s_offset >= sed_real_range_end)
                    {
                        // Record the end position and exit
                        end_rect = s_ex;
                        break;
                    }
                }
                if(set_start_rect)
                {
                    SetColor(COL_SCOPESSAMPLES);
                    if(end_rect == start_rect) end_rect++;
                    Fillrect(start_rect + WAVE_LEFT + 1, (Cur_Height - 150) + 1, end_rect + WAVE_LEFT + 1, (Cur_Height - 150) + SAMPLE_HEIGHT - 1);
                }

                // Now draw the sample data
                switch(SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split])
                {
                    case 1:
                        // Mono sample
                        for(s_ex = 0; s_ex < LARGE_SMP_VIEW; s_ex++)
                        {
                            pos_in_wav = ((int64) s_ex * (int64) sed_display_length);
                            s_offset = (int) (pos_in_wav / LARGE_SMP_VIEW) + sed_display_start;
                            int h = *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + s_offset) / ptk->rs_coef;
                            if(h > s_size) h = s_size;
                            if(h < -s_size) h = -s_size;

                            int s_y = s_ey - h;
                            if(s_y > ((Cur_Height - 150) + SAMPLE_HEIGHT - 2)) s_y = (Cur_Height - 150) + SAMPLE_HEIGHT - 2;

                            // Look for selection
                            if(sed_range_mode &&
                               s_offset >= sed_real_range_start &&
                               s_offset < sed_real_range_end)
                            {

                                // Sample datas
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey, s_y, COL_BACKGROUND);
                                DrawPixel(s_ex + WAVE_LEFT + 1, s_ey, COL_BACKGROUND);
                            }
                            else
                            {
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey, s_y, COL_SCOPESSAMPLES);
                                DrawPixel(s_ex + WAVE_LEFT + 1, s_ey, COL_SCOPESSAMPLES);
                            }
                        }
                        break;

                    case 2:
                        // Stereo sample
                        for(s_ex = 0; s_ex < LARGE_SMP_VIEW; s_ex++)
                        {
                            pos_in_wav = ((int64) s_ex * (int64) sed_display_length);
                            s_offset = (int) (pos_in_wav / LARGE_SMP_VIEW) + sed_display_start;
                            int h = *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + s_offset) / ptk->rs_coef;
                            int h2 = *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + s_offset) / ptk->rs_coef;
                            if(h > s_size) h = s_size;
                            if(h < -s_size) h = -s_size;
                            if(h2 > s_size) h2 = s_size;
                            if(h2 < -s_size) h2 = -s_size;
                            int s_y = s_ey - h;
                            int s_y2 = s_ey2 - h2;
                            if(s_y2 > ((Cur_Height - 150) + SAMPLE_HEIGHT - 2)) s_y2 = (Cur_Height - 150) + SAMPLE_HEIGHT - 2;
                            if(s_y > ((Cur_Height - 150) + SAMPLE_HEIGHT - 2)) s_y = (Cur_Height - 150) + SAMPLE_HEIGHT - 2;

                            if(sed_range_mode &&
                               s_offset >= sed_real_range_start &&
                               s_offset < sed_real_range_end)
                            {
                                // Sample datas
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey, s_y, COL_BACKGROUND);
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey2, s_y2, COL_BACKGROUND);
                                // Straight line
                                DrawPixel(s_ex + WAVE_LEFT + 1, s_ey, COL_BACKGROUND);
                                DrawPixel(s_ex + WAVE_LEFT + 1, s_ey2, COL_BACKGROUND);
                            }
                            else
                            {
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey, s_y, COL_SCOPESSAMPLES);
                                DrawVLine(s_ex + WAVE_LEFT + 1, s_ey2, s_y2, COL_SCOPESSAMPLES);
                                DrawPixel(s_ex + WAVE_LEFT + 1, s_ey, COL_SCOPESSAMPLES);
                                DrawPixel(s_ex + WAVE_LEFT + 1, s_ey2, COL_SCOPESSAMPLES);
                            }
                        }
                        break;
                }

                // Display the loop infos bars
                if(LoopType[ptk->Current_Instrument][ptk->Current_Instrument_Split])
                {
                    int64 LSX64 = ((int64) (LoopStart[ptk->Current_Instrument][ptk->Current_Instrument_Split] - sed_display_start)) * LARGE_SMP_VIEW;
                    int32 LSX = (int32) ((int64) LSX64 / (int64) sed_display_length);
                    int64 LEX64 = ((int64) (LoopEnd[ptk->Current_Instrument][ptk->Current_Instrument_Split] - sed_display_start)) * LARGE_SMP_VIEW;
                    int32 LEX = (int32) ((int64) LEX64 / (int64) sed_display_length);

                    if(LSX >= 0 && LSX <= LARGE_SMP_VIEW)
                    {
                        DrawVLine(LSX + WAVE_LEFT + 1, (Cur_Height - 150) + 1, (Cur_Height - 150) + SAMPLE_HEIGHT - 2, COL_VUMETERPEAK);
                        DrawHLine((Cur_Height - 150) + 5, LSX + WAVE_LEFT + 1 + 2, LSX + WAVE_LEFT + 1 + 5, COL_VUMETERPEAK);
                        DrawHLine((Cur_Height - 150) + 6, LSX + WAVE_LEFT + 1 + 2, LSX + WAVE_LEFT + 1 + 2, COL_VUMETERPEAK);
                        DrawHLine((Cur_Height - 150) + 7, LSX + WAVE_LEFT + 1 + 2, LSX + WAVE_LEFT + 1 + 5, COL_VUMETERPEAK);
                        DrawHLine((Cur_Height - 150) + 8, LSX + WAVE_LEFT + 1 + 5, LSX + WAVE_LEFT + 1 + 5, COL_VUMETERPEAK);
                        DrawHLine((Cur_Height - 150) + 9, LSX + WAVE_LEFT + 1 + 2, LSX + WAVE_LEFT + 1 + 5, COL_VUMETERPEAK);
                    }

                    if(LEX >= 0 && LEX <= LARGE_SMP_VIEW)
                    {
                        if(LEX == LARGE_SMP_VIEW) LEX--;
                        DrawVLine(LEX + WAVE_LEFT + 1 + 1, (Cur_Height - 150) + 1, (Cur_Height - 150) + SAMPLE_HEIGHT - 2, COL_VUMETERPEAK);
                        DrawHLine((Cur_Height - 150) + SAMPLE_HEIGHT - 9, LEX + WAVE_LEFT + 1 - 5, LEX + WAVE_LEFT + 1 - 2, COL_VUMETERPEAK);
                        DrawHLine((Cur_Height - 150) + SAMPLE_HEIGHT - 8, LEX + WAVE_LEFT + 1 - 5, LEX + WAVE_LEFT + 1 - 5, COL_VUMETERPEAK);
                        DrawHLine((Cur_Height - 150) + SAMPLE_HEIGHT - 7, LEX + WAVE_LEFT + 1 - 5, LEX + WAVE_LEFT + 1 - 2, COL_VUMETERPEAK);
                        DrawHLine((Cur_Height - 150) + SAMPLE_HEIGHT - 6, LEX + WAVE_LEFT + 1 - 5, LEX + WAVE_LEFT + 1 - 5, COL_VUMETERPEAK);
                        DrawHLine((Cur_Height - 150) + SAMPLE_HEIGHT - 5, LEX + WAVE_LEFT + 1 - 5, LEX + WAVE_LEFT + 1 - 2, COL_VUMETERPEAK);
                    }
                }
            }
            ptk->draw_sampled_wave = FALSE;
        }
        Draw_Wave_PlayBack_Pos(ptk);
        if(ptk->draw_sampled_wave3)
        {
            ptk->draw_sampled_wave = TRUE;
            ptk->draw_sampled_wave3 = FALSE;
            Draw_Wave_Data(ptk);
        }
    }
}

// ------------------------------------------------------
void Renew_Sample_Ed(ptk_data *ptk)
{
    ptk->draw_sampled_wave = TRUE;
    sed_display_start = 0;
    sed_display_length = SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split];
    sed_range_start = 0;
    sed_range_end = 0;
    sed_range_mode = FALSE;
    Actualize_Sample_Ed(ptk, 0);
}

// ------------------------------------------------------
// Draw the current playback position
void Draw_Wave_PlayBack_Pos(ptk_data *ptk)
{
    int32 sed_real_range_start;
    int32 sed_real_range_end;
    int32 pos_in_sample;
    int i;
    int64 pos_in_wav;

    if(ptk->draw_sampled_wave2)
    {
        if(SampleType[ptk->Current_Instrument][ptk->Current_Instrument_Split] > 0)
        { 
            // Any Sample Out There ?
            int strober = SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split] * 2;
            int s_ey = (Cur_Height - 150) + 1 + (SAMPLE_LINES_HEIGHT / strober);
            int s_ey2 = s_ey + ((SAMPLE_LINES_HEIGHT / strober) * 2);
            int rcolor3;
            int rcolor4;

            pos_in_sample = 0;
            for(i = 0; i < MAX_POLYPHONY; i++)
            {
                if((int32) sp_Position[ptk->Track_Under_Caret][i].half.first > pos_in_sample)
                {
                    if(sp_Stage[ptk->Track_Under_Caret][i] == PLAYING_SAMPLE)
                    {
                        pos_in_sample = sp_Position[ptk->Track_Under_Caret][i].half.first;
                    }
                }
            }

            sed_real_range_start = sed_range_start;
            sed_real_range_end = sed_range_end;

            if(sed_real_range_start > sed_real_range_end)
            {
                int Swap_Range = sed_real_range_start;
                sed_real_range_start = sed_real_range_end;
                sed_real_range_end = Swap_Range;
            }

            if(SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split] == 1)
            {
                for(int32 s_ex = 0; s_ex < LARGE_SMP_VIEW; s_ex++)
                {
                    pos_in_wav = ((int64) s_ex * (int64) sed_display_length);
                    int32 s_offset = (int) (pos_in_wav / LARGE_SMP_VIEW) + sed_display_start;

                    if(pos_in_sample > s_offset)
                    {
                        rcolor3 = COL_PUSHED_MED;
                    }
                    else
                    {
                        if(sed_range_mode &&
                           s_offset >= sed_real_range_start &&
                           s_offset <= sed_real_range_end)
                        {
                            rcolor3 = COL_BACKGROUND;
                        }
                        else
                        {
                            rcolor3 = COL_SCOPESSAMPLES;
                        }
                    }
                    DrawVLine(s_ex + WAVE_LEFT + 1, s_ey, s_ey, rcolor3);
                }
            } // If

            // STEREO DISPLAY
            if(SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split] == 2)
            {
                for(int32 s_ex = 0; s_ex < LARGE_SMP_VIEW; s_ex++)
                {
                    pos_in_wav = ((int64) s_ex * (int64) sed_display_length);
                    int32 s_offset = (int) (pos_in_wav / LARGE_SMP_VIEW) + sed_display_start;
 
                    if(pos_in_sample > s_offset)
                    {
                        rcolor3 = COL_PUSHED_MED;
                        rcolor4 = COL_PUSHED_MED;
                    }
                    else
                    {
                        if(sed_range_mode &&
                           s_offset >= sed_real_range_start &&
                           s_offset <= sed_real_range_end)
                        {
                            rcolor3 = COL_BACKGROUND;
                            rcolor4 = COL_BACKGROUND;
                        }
                        else
                        {
                            rcolor3 = COL_SCOPESSAMPLES;
                            rcolor4 = COL_SCOPESSAMPLES;
                        }
                    }

                    DrawVLine(s_ex + WAVE_LEFT + 1, s_ey, s_ey, rcolor3);
                    DrawVLine(s_ex + WAVE_LEFT + 1, s_ey2, s_ey2, rcolor4);
                }

            } // If Stereo

        } // IF CHECK SAMPLE

        ptk->draw_sampled_wave2 = FALSE;
    }
}

// ------------------------------------------------------
// Display the various element of the editor
// and responds to the various commands
void Actualize_Sample_Ed(ptk_data *ptk, char gode)
{
    int32 sed_real_range_start;
    int32 sed_real_range_end;
    int ReadOnly;
    int Allow;

    if(ptk->userscreen == USER_SCREEN_SAMPLE_EDIT)
    {
        Allow = 0;
        if(!SampleType[ptk->Current_Instrument][ptk->Current_Instrument_Split]) Allow = BUTTON_DISABLED;

        if(SamplesSwap[ptk->Current_Instrument]) ReadOnly = BUTTON_DISABLED;
        else
        {
            ReadOnly = 0;
        }

        if(gode == 0)
        {

            // Those commands aren't available when viewing packed samples (which are read only)
            Gui_Draw_Button_Box(520, (Cur_Height - 150), 29, 16, "Cut", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(551, (Cur_Height - 150), 29, 16, "Copy", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(582, (Cur_Height - 150), 29, 16, "Paste", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(613, (Cur_Height - 150), 29, 16, "Crop", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(551, (Cur_Height - 132), 29, 16, "Half", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);

            Gui_Draw_Button_Box(650, (Cur_Height - 132), 16, 16, "\05", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
            Gui_Draw_Button_Box(668, (Cur_Height - 132), 16, 16, "\03", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(686, (Cur_Height - 132), 16, 16, "\04", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(704, (Cur_Height - 132), 16, 16, "\06", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
            Gui_Draw_Button_Box(722, (Cur_Height - 132), 50, 16, "Reverse", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            
            Gui_Draw_Button_Box(520, (Cur_Height - 132), 29, 16, "Zap", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);

            Gui_Draw_Button_Box(520, (Cur_Height - 114), 29, 16, "Max.", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(551, (Cur_Height - 114), 29, 16, "Zero", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(520, (Cur_Height - 96), 29, 16, "Dup.", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(551, (Cur_Height - 96), 29, 16, "Ins.", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(520, (Cur_Height - 78), 60, 16, "DC Adjust", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(520, (Cur_Height - 60), 29, 16, "F. In", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(551, (Cur_Height - 60), 29, 16, "F. Out", BUTTON_NORMAL | Allow | ReadOnly | BUTTON_TEXT_CENTERED);

            // Non-modifying operations are allowed
            Gui_Draw_Button_Box(582, (Cur_Height - 132), 60, 16, "Sel. View", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(582, (Cur_Height - 114), 60, 16, "Unselect", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED);

            Gui_Draw_Button_Box(582, (Cur_Height - 96), 60, 16, "View All", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(582, (Cur_Height - 78), 60, 16, "VZoom In", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(582, (Cur_Height - 60), 60, 16, "VZoom Out", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED);

            Gui_Draw_Button_Box(650, (Cur_Height - 114), 60, 16, "Zoom In", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(650, (Cur_Height - 96), 60, 16, "Zoom Out", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED);

            Gui_Draw_Button_Box(712, (Cur_Height - 114), 60, 16, "Loop Start", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED);
            Gui_Draw_Button_Box(712, (Cur_Height - 96), 60, 16, "Loop End", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED);

            Gui_Draw_Button_Box(WAVE_LEFT, (Cur_Height - 41), 16, 16, "\03", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);
            Gui_Draw_Button_Box(WAVE_LEFT + LARGE_SMP_VIEW - (18 * 1) + 3, (Cur_Height - 41), 16, 16, "\04", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED | BUTTON_RIGHT_MOUSE);

            Gui_Draw_Button_Box(722, (Cur_Height - 150), 50, 16, "Sel. All", BUTTON_NORMAL | Allow | BUTTON_TEXT_CENTERED);

            Display_Sample_Buffers(ptk, Allow | ReadOnly);
        }

        sed_real_range_start = sed_range_start;
        sed_real_range_end = sed_range_end;

        if(sed_real_range_start > sed_real_range_end)
        {
            int Swap_Range = sed_real_range_start;
            sed_real_range_start = sed_real_range_end;
            sed_real_range_end = Swap_Range;
        }

        if(gode == 5 || gode == 0)
        {
            if(LoopType[ptk->Current_Instrument][ptk->Current_Instrument_Split] == SMP_LOOP_NONE)
            {
                Gui_Draw_Button_Box(520, (Cur_Height - 42), 60, 16, "S: -", BUTTON_NORMAL | BUTTON_DISABLED);
                Gui_Draw_Button_Box(582, (Cur_Height - 42), 60, 16, "E: -", BUTTON_NORMAL | BUTTON_DISABLED);
            }
            else
            {
                outlong(520, (Cur_Height - 42), LoopStart[ptk->Current_Instrument][ptk->Current_Instrument_Split], 11);
                outlong(582, (Cur_Height - 42), LoopEnd[ptk->Current_Instrument][ptk->Current_Instrument_Split], 12);
            }
        }

        if(SampleType[ptk->Current_Instrument][ptk->Current_Instrument_Split])
        {
            if(gode == 3 || gode == 0)
            {
                outlong(712, (Cur_Height - 60), sed_display_start, 10);
                outlong(712, (Cur_Height - 42), sed_display_length, 12);
            }

            if(gode == 4 || gode == 5 || gode == 0)
            {
                outlong(650, (Cur_Height - 60), sed_real_range_start, 10);
            }

            if(gode == 4 || gode == 5 || gode == 0)
            {
                outlong(650, (Cur_Height - 42), sed_real_range_end, 11);
            }

            // Rotate 1 sample left
            if(gode == SMPED_ROTATE_LEFT_1)
            {
                if(Sample_Rotate_Left(ptk, sed_real_range_start, sed_real_range_end, 1))
                {
                    Refresh_Sample(ptk, FALSE);
                }
            }

            // Rotate 1 sample left
            if(gode == SMPED_ROTATE_RIGHT_1)
            {
                if(Sample_Rotate_Right(ptk, sed_real_range_start, sed_real_range_end, 1))
                {
                    Refresh_Sample(ptk, FALSE);
                }
            }

            // Rotate x samples left
            if(gode == SMPED_ROTATE_LEFT_X)
            {
                if(Sample_Rotate_Left(ptk, sed_real_range_start, sed_real_range_end, (sed_real_range_end - sed_real_range_start) / 2))
                {
                    Refresh_Sample(ptk, FALSE);
                }
            }

            // Rotate x samples right
            if(gode == SMPED_ROTATE_RIGHT_X)
            {
                if(Sample_Rotate_Right(ptk, sed_real_range_start, sed_real_range_end, (sed_real_range_end - sed_real_range_start) / 2))
                {
                    Refresh_Sample(ptk, FALSE);
                }
            }

            // Cut Sample
            if(gode == SMPED_CUT)
            {
                if(Sample_Cut(ptk, sed_real_range_start, sed_real_range_end, TRUE))
                {
                    Refresh_Sample(ptk, TRUE);
                }
            }

            // Copy Sample
            if(gode == SMPED_COPY)
            {
                if(Sample_Copy(ptk, sed_real_range_start, sed_real_range_end))
                {
                    Display_Sample_Buffers(ptk, Allow | ReadOnly);
                }
            }

            // Paste Sample
            if(gode == SMPED_PASTE)
            {
                if(Sample_Paste(ptk, sed_real_range_start))
                {
                    Refresh_Sample(ptk, TRUE);
                }
            }

            // Crop Sample
            if(gode == SMPED_CROP)
            {
                if(Sample_Crop(ptk, sed_real_range_start, sed_real_range_end))
                {
                    Refresh_Sample(ptk, TRUE);
                }
            }

            // Reverse Sample
            if(gode == SMPED_REVERSE)
            {
                if(Sample_Reverse(ptk, sed_real_range_start, sed_real_range_end))
                {
                    Refresh_Sample(ptk, FALSE);
                }
            }

            // Select buffer 1
            if(gode == SMPED_BUF1)
            {
                cur_sample_buffer = 0;
                Display_Sample_Buffers(ptk, Allow | ReadOnly);
            }

            // Select buffer 2
            if(gode == SMPED_BUF2)
            {
                cur_sample_buffer = 1;
                Display_Sample_Buffers(ptk, Allow | ReadOnly);
            }

            // Select buffer 3
            if(gode == SMPED_BUF3)
            {
                cur_sample_buffer = 2;
                Display_Sample_Buffers(ptk, Allow | ReadOnly);
            }

            // Select buffer 4
            if(gode == SMPED_BUF4)
            {
                cur_sample_buffer = 3;
                Display_Sample_Buffers(ptk, Allow | ReadOnly);
            }

            // Zap
            if(gode == SMPED_ZAP)
            {
                if(Sample_Cut(ptk, sed_real_range_start, sed_real_range_end, FALSE))
                {
                    Refresh_Sample(ptk, TRUE);
                }
            }

            // DC adjust
            if(gode == SMPED_DC_ADJUST)
            {
                Sample_DC_Adjust(ptk, sed_real_range_start, sed_real_range_end);
            }

            // Maximize
            if(gode == SMPED_MAXIMIZE)
            {
                Sample_Maximize(ptk, sed_real_range_start, sed_real_range_end);
            }

            // Zeroize
            if(gode == SMPED_ZEROIZE)
            {
                Sample_Zeroize(ptk, sed_real_range_start, sed_real_range_end);
            }

            // Fade In
            if(gode == SMPED_FADEIN)
            {
                Sample_FadeIn(ptk, sed_real_range_start, sed_real_range_end);
            }

            // Fade Out
            if(gode == SMPED_FADEOUT)
            {
                Sample_FadeOut(ptk, sed_real_range_start, sed_real_range_end);
            }

            // Half
            if(gode == SMPED_HALF)
            {
                Sample_Half(ptk, sed_real_range_start, sed_real_range_end);
            }

            // Duplicate
            if(gode == SMPED_DUPLICATE)
            {
                if(Sample_Duplicate(ptk, sed_real_range_start, sed_real_range_end))
                {
                    Refresh_Sample(ptk, TRUE);
                }
            }

            // Insert zero
            if(gode == SMPED_INSERTZERO)
            {
                if(Sample_InsertZero(ptk, sed_real_range_start, sed_real_range_end))
                {
                    Refresh_Sample(ptk, TRUE);
                }
            }

        }
        else
        {
            Gui_Draw_Button_Box(650, (Cur_Height - 60), 122, 34, "No Sample Loaded", BUTTON_NORMAL | BUTTON_DISABLED | BUTTON_TEXT_VTOP);
        }
    }
}

// ------------------------------------------------------
// Right mouse buttons events
void Mouse_Right_Sample_Ed(ptk_data *ptk)
{
    int Allow = TRUE;

    if(SamplesSwap[ptk->Current_Instrument]) Allow = FALSE;

    if(ptk->userscreen == USER_SCREEN_SAMPLE_EDIT)
    {
        if(zcheckMouse(ptk, WAVE_LEFT + 1, (Cur_Height - 150), LARGE_SMP_VIEW, SAMPLE_LINES_HEIGHT))
        {
            if(sed_range_mode)
            {
                // Unselect
                sed_range_start = 0;
                sed_range_end = 0;
                sed_range_mode = FALSE;
                ptk->draw_sampled_wave = TRUE;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
                ptk->teac = 4;
            }
            else
            {
                // Select view
                sed_range_mode = TRUE;
                sed_range_start = sed_display_start;
                sed_range_end = sed_display_start + sed_display_length;
                ptk->draw_sampled_wave = TRUE;
                ptk->teac = 0;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }
        }

        // Rotate left x
        if(zcheckMouse(ptk, 650, (Cur_Height - 132), 16, 16) && sed_range_mode && Allow)
        {
            ptk->teac = SMPED_ROTATE_LEFT_X;
            ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
        }

        // Rotate right x
        if(zcheckMouse(ptk, 704, (Cur_Height - 132), 16, 16) && sed_range_mode && Allow)
        {
            ptk->teac = SMPED_ROTATE_RIGHT_X;
            ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
        }

        // Bottom arrow left
        if(zcheckMouse(ptk, WAVE_LEFT, (Cur_Height - 41), 16, 16))
        {
            if(SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split])
            {
                sed_display_start -= sed_display_length;
                if((int) sed_display_start < 0) sed_display_start = 0;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
                ptk->draw_sampled_wave = TRUE;
            }
        }

        // Bottom arrow right
        if(zcheckMouse(ptk, WAVE_LEFT + LARGE_SMP_VIEW - (18 * 1) + 3, (Cur_Height - 41), 16, 16))
        {
            if(SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split])
            {
                int max_length = SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split] - sed_display_length;
                sed_display_start += sed_display_length;
                if((int) sed_display_start > max_length) sed_display_start = max_length;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
                ptk->draw_sampled_wave = TRUE;
            }
        }
    }
}

// ------------------------------------------------------
// Middle mouse buttons events
void Mouse_Middle_Sample_Ed(ptk_data *ptk)
{
    if(ptk->userscreen == USER_SCREEN_SAMPLE_EDIT)
    {
        if(zcheckMouse(ptk, WAVE_LEFT + 1, (Cur_Height - 150), LARGE_SMP_VIEW, SAMPLE_LINES_HEIGHT))
        {
            if(sed_range_mode && sed_range_start != sed_range_end) Zoom_In_Sel(ptk);
            else Zoom_Out_Sel(ptk);
        }
    }
}

// ------------------------------------------------------
// Left mouse buttons events
void Mouse_Left_Sample_Ed(ptk_data *ptk)
{
    int Allow = TRUE;

    if(SamplesSwap[ptk->Current_Instrument]) Allow = FALSE;

    if(ptk->userscreen == USER_SCREEN_SAMPLE_EDIT)
    {
        if(SampleType[ptk->Current_Instrument][ptk->Current_Instrument_Split])
        {
            // Set Loop Start
            if(zcheckMouse(ptk, 712, (Cur_Height - 114), 60, 16) && sed_range_mode)
            {
                if(sed_range_start > sed_range_end)
                {
                    LoopStart[ptk->Current_Instrument][ptk->Current_Instrument_Split] = sed_range_end;
                }
                else
                {
                    LoopStart[ptk->Current_Instrument][ptk->Current_Instrument_Split] = sed_range_start;
                }

                if(LoopStart[ptk->Current_Instrument][ptk->Current_Instrument_Split] > LoopEnd[ptk->Current_Instrument][ptk->Current_Instrument_Split])
                {
                    LoopEnd[ptk->Current_Instrument][ptk->Current_Instrument_Split] = LoopStart[ptk->Current_Instrument][ptk->Current_Instrument_Split];
                }

                // Initialize a new loop
                if(LoopType[ptk->Current_Instrument][ptk->Current_Instrument_Split] == SMP_LOOP_NONE)
                {
                    LoopType[ptk->Current_Instrument][ptk->Current_Instrument_Split] = SMP_LOOP_FORWARD;
                    LoopEnd[ptk->Current_Instrument][ptk->Current_Instrument_Split] = SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split];
                }

                ptk->draw_sampled_wave = TRUE;
                switch(ptk->userscreen)
                {
                    case USER_SCREEN_INSTRUMENT_EDIT:
                        ptk->gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                        ptk->teac = 5;
                        break;

                    case USER_SCREEN_SAMPLE_EDIT:
                        ptk->teac = 5;
                        ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
                        break;
                }
            }

            // Set Loop End
            if(zcheckMouse(ptk, 712, (Cur_Height - 96), 60, 16) && sed_range_mode)
            {
                if(sed_range_start > sed_range_end)
                {
                    LoopEnd[ptk->Current_Instrument][ptk->Current_Instrument_Split] = sed_range_start;
                }
                else
                {
                    LoopEnd[ptk->Current_Instrument][ptk->Current_Instrument_Split] = sed_range_end;
                }

                if(LoopEnd[ptk->Current_Instrument][ptk->Current_Instrument_Split] < LoopStart[ptk->Current_Instrument][ptk->Current_Instrument_Split])
                {
                    LoopStart[ptk->Current_Instrument][ptk->Current_Instrument_Split] = LoopEnd[ptk->Current_Instrument][ptk->Current_Instrument_Split];
                }

                if(LoopType[ptk->Current_Instrument][ptk->Current_Instrument_Split] == SMP_LOOP_NONE)
                {
                    LoopType[ptk->Current_Instrument][ptk->Current_Instrument_Split] = SMP_LOOP_FORWARD;
                    LoopStart[ptk->Current_Instrument][ptk->Current_Instrument_Split] = 0;
                }

                if(LoopEnd[ptk->Current_Instrument][ptk->Current_Instrument_Split] == LoopStart[ptk->Current_Instrument][ptk->Current_Instrument_Split])
                {
                    LoopType[ptk->Current_Instrument][ptk->Current_Instrument_Split] = SMP_LOOP_NONE;
                    LoopStart[ptk->Current_Instrument][ptk->Current_Instrument_Split] = 0;
                    LoopEnd[ptk->Current_Instrument][ptk->Current_Instrument_Split] = 0;
                }

                ptk->draw_sampled_wave = TRUE;

                switch(ptk->userscreen)
                {
                    case USER_SCREEN_INSTRUMENT_EDIT:
                        ptk->gui_action = GUI_CMD_UPDATE_INSTRUMENT_ED;
                        ptk->teac = 5;
                        break;

                    case USER_SCREEN_SAMPLE_EDIT:
                        ptk->teac = 5;
                        ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
                        break;
                }
            }

            // Rotate left 1
            if(zcheckMouse(ptk, 668, (Cur_Height - 132), 16, 16) && sed_range_mode && Allow)
            {
                ptk->teac = SMPED_ROTATE_LEFT_1;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Rotate right 1
            if(zcheckMouse(ptk, 686, (Cur_Height - 132), 16, 16) && sed_range_mode && Allow)
            {
                ptk->teac = SMPED_ROTATE_RIGHT_1;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Cut
            if(zcheckMouse(ptk, 520, (Cur_Height - 150), 29, 16) && sed_range_mode && Allow)
            {
                ptk->teac = SMPED_CUT;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Copy
            if(zcheckMouse(ptk, 551, (Cur_Height - 150), 29, 16) && sed_range_mode && Allow)
            {
                ptk->teac = SMPED_COPY;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Paste
            if(zcheckMouse(ptk, 582, (Cur_Height - 150), 29, 16) && sed_range_mode && Allow)
            {
                ptk->teac = SMPED_PASTE;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Crop
            if(zcheckMouse(ptk, 613, (Cur_Height - 150), 29, 16) && sed_range_mode && Allow)
            {
                ptk->teac = SMPED_CROP;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Reverse
            if(zcheckMouse(ptk, 722, (Cur_Height - 132), 50, 16) && sed_range_mode && Allow)
            {
                ptk->teac = SMPED_REVERSE;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Select buffer 1
            if(zcheckMouse(ptk, 650, (Cur_Height - 150), 16, 16) && Allow)
            {
                ptk->teac = SMPED_BUF1;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Select buffer 2
            if(zcheckMouse(ptk, 668, (Cur_Height - 150), 16, 16) && Allow)
            {
                ptk->teac = SMPED_BUF2;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Select buffer 3
            if(zcheckMouse(ptk, 686, (Cur_Height - 150), 16, 16) && Allow)
            {
                ptk->teac = SMPED_BUF3;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Select buffer 4
            if(zcheckMouse(ptk, 704, (Cur_Height - 150), 16, 16) && Allow)
            {
                ptk->teac = SMPED_BUF4;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Select buffer 4
            if(zcheckMouse(ptk, 520, (Cur_Height - 132), 16, 16) && Allow)
            {
                ptk->teac = SMPED_ZAP;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Half
            if(zcheckMouse(ptk, 551, (Cur_Height - 132), 29, 16) && sed_range_mode && Allow)
            {
                ptk->teac = SMPED_HALF;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Maximize
            if(zcheckMouse(ptk, 520, (Cur_Height - 114), 29, 16) && sed_range_mode && Allow)
            {
                ptk->teac = SMPED_MAXIMIZE;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Zeroize
            if(zcheckMouse(ptk, 551, (Cur_Height - 114), 29, 16) && sed_range_mode && Allow)
            {
                ptk->teac = SMPED_ZEROIZE;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Duplicate
            if(zcheckMouse(ptk, 520, (Cur_Height - 96), 29, 16) && sed_range_mode && Allow)
            {
                ptk->teac = SMPED_DUPLICATE;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Insert zeroes
            if(zcheckMouse(ptk, 551, (Cur_Height - 96), 29, 16) && sed_range_mode && Allow)
            {
                ptk->teac = SMPED_INSERTZERO;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // DC Adjust
            if(zcheckMouse(ptk, 520, (Cur_Height - 78), 60, 16) && sed_range_mode && Allow)
            {
                ptk->teac = SMPED_DC_ADJUST;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Fade in
            if(zcheckMouse(ptk, 520, (Cur_Height - 60), 29, 16) && sed_range_mode && Allow)
            {
                ptk->teac = SMPED_FADEIN;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Fade out
            if(zcheckMouse(ptk, 551, (Cur_Height - 60), 29, 16) && sed_range_mode && Allow)
            {
                ptk->teac = SMPED_FADEOUT;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // View All
            if(zcheckMouse(ptk, 582, (Cur_Height - 96), 60, 16))
            {
                ptk->rs_coef = 32768;
                sed_display_start = 0;
                sed_display_length = SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split];
                ptk->draw_sampled_wave = TRUE;
                ptk->teac = 3;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // VZoom In
            if(zcheckMouse(ptk, 582, (Cur_Height - 78), 60, 16))
            {
                ptk->rs_coef /= 2;
                ptk->draw_sampled_wave = TRUE;
            }

            // VZoom Out
            if(zcheckMouse(ptk, 582, (Cur_Height - 60), 60, 16))
            {
                ptk->rs_coef *= 2;
                ptk->draw_sampled_wave = TRUE;
            }

            // Unselect
            if(zcheckMouse(ptk, 582, (Cur_Height - 114), 60, 16))
            {
                sed_range_mode = FALSE;
                sed_range_start = 0;
                sed_range_end = 0;
                ptk->draw_sampled_wave = TRUE;
                ptk->teac = 0;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Select all
            if(zcheckMouse(ptk, 722, (Cur_Height - 150), 50, 16))
            {
                sed_range_mode = TRUE;
                sed_range_start = 0;
                sed_range_end = SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split];
                ptk->draw_sampled_wave = TRUE;
                ptk->teac = 0;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Select view
            if(zcheckMouse(ptk, 582, (Cur_Height - 132), 60, 16))
            {
                sed_range_mode = TRUE;
                sed_range_start = sed_display_start;
                sed_range_end = sed_display_start + sed_display_length;
                ptk->draw_sampled_wave = TRUE;
                ptk->teac = 0;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            }

            // Zoom in
            if(zcheckMouse(ptk, 650, (Cur_Height - 114), 60, 16) && sed_range_mode)
            {
                Zoom_In_Sel(ptk);
            }

            // Zoom out
            if(zcheckMouse(ptk, 650, (Cur_Height - 96), 60, 16))
            {
                Zoom_Out_Sel(ptk);
            }

            // Bottom arrow left
            if(zcheckMouse(ptk, WAVE_LEFT, (Cur_Height - 41), 16, 16))
            {
                if(SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split])
                {
                    sed_display_start--;
                    if((int) sed_display_start < 0) sed_display_start = 0;
                    ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
                    ptk->draw_sampled_wave = TRUE;
                }
            }

            // Bottom arrow right
            if(zcheckMouse(ptk, WAVE_LEFT + LARGE_SMP_VIEW - (18 * 1) + 3, (Cur_Height - 41), 16, 16))
            {
                if(SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split])
                {
                    int max_length = SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split] - sed_display_length;
                    sed_display_start++;
                    if((int) sed_display_start > max_length) sed_display_start = max_length;
                    ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
                    ptk->draw_sampled_wave = TRUE;
                }
            }

        }
    }
}

// ------------------------------------------------------
// Mouse wheel events
void Mouse_Wheel_Sample_Ed(ptk_data *ptk, int roll_amount)
{
    if(ptk->userscreen == USER_SCREEN_SAMPLE_EDIT)
    {
        if(zcheckMouse(ptk, WAVE_LEFT + 1, (Cur_Height - 150), LARGE_SMP_VIEW, 109 + 16))
        {
            int max_length = SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split];
            sed_display_start += roll_amount * (sed_display_length / 16);

            if(sed_display_start < 0) sed_display_start = 0;
            if(sed_display_start > (max_length - sed_display_length))
            {
                sed_display_start = (max_length - sed_display_length);
            }
            ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
            ptk->draw_sampled_wave = TRUE;
        }
    }
}

// ------------------------------------------------------
// Sliders events
void Mouse_Sliders_Sample_Ed(ptk_data *ptk)
{
    int Mouse_Pos;
    double test;
    int Allow = TRUE;

    if(SamplesSwap[ptk->Current_Instrument]) Allow = FALSE;

    if(ptk->userscreen == USER_SCREEN_SAMPLE_EDIT)
    {
        if(zcheckMouse(ptk, WAVE_LEFT, (Cur_Height - 150), LARGE_SMP_VIEW + 1, 109))
        {
            // Move within the sample
            if(SampleType[ptk->Current_Instrument][ptk->Current_Instrument_Split])
            {
                Mouse_Pos = (Mouse.x - WAVE_LEFT) - 1;
                if(Mouse_Pos < 0) Mouse_Pos = 0;
                if(Mouse_Pos > LARGE_SMP_VIEW) Mouse_Pos = LARGE_SMP_VIEW;
                axswave = SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split];
                sed_range_mode = TRUE;
                test = (double) (((int64) Mouse_Pos * (int64) sed_display_length)) / LARGE_SMP_VIEW;
                sed_range_end = sed_display_start + (int32) test;
                ptk->teac = 4;

                if(!ptk->sas)
                {
                    // Selection start
                    sed_range_start = sed_range_end;
                    ptk->teac = 5;
                }

                if(sed_range_start < sed_display_start)
                {
                    sed_range_start = sed_display_start;
                    ptk->teac = 5;
                }
                if(sed_range_end < sed_display_start)
                {
                    sed_range_end = sed_display_start;
                    ptk->teac = 5;
                }

                if(sed_range_end > axswave)
                {
                    sed_range_end = axswave;
                }

                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
                ptk->draw_sampled_wave = TRUE;
                ptk->sas = TRUE;
            } // SAMPLETYPE
        } // MOUSEBOX

        // Bottom slider
        if(zcheckMouse(ptk, WAVE_LEFT + 18, (Cur_Height - 41), LARGE_SMP_VIEW + 2 - (18 * 2), 16))
        {
            if(SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split])
            {
                int max_length = SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split];
                int Center = Slider_Get_Center(sed_display_length, max_length, LARGE_SMP_VIEW + 2 - (18 * 2));
                float Pos_Mouse = ((float) ((Mouse.x - (WAVE_LEFT + 18)) - (Center / 2))) / (float) (LARGE_SMP_VIEW + 2 - (18 * 2));
                if(Pos_Mouse > 1.0f) Pos_Mouse = 1.0f;
                float s_offset = (Pos_Mouse * max_length);

                if(s_offset > (float) (max_length - sed_display_length))
                {
                    s_offset = (float) (max_length - sed_display_length);
                }
                if(s_offset < 0) s_offset = 0;
                sed_display_start = (int32) s_offset;
                ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
                ptk->draw_sampled_wave = TRUE;
            }
        }

        // Rotate left 1
        if(zcheckMouse(ptk, 650, (Cur_Height - 132), 16, 16) && sed_range_mode && Allow)
        {
            ptk->teac = SMPED_ROTATE_LEFT_1;
            ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
        }

        // Rotate right 1
        if(zcheckMouse(ptk, 704, (Cur_Height - 132), 16, 16) && sed_range_mode && Allow)
        {
            ptk->teac = SMPED_ROTATE_RIGHT_1;
            ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
        }

    }
}

// ------------------------------------------------------
// Make sure the loop infos are sane
void Check_Loops(ptk_data *ptk)
{
    if(LoopStart[ptk->Current_Instrument][ptk->Current_Instrument_Split] < 0) LoopStart[ptk->Current_Instrument][ptk->Current_Instrument_Split] = 0;
    if(LoopStart[ptk->Current_Instrument][ptk->Current_Instrument_Split] > SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split])
    {
        LoopStart[ptk->Current_Instrument][ptk->Current_Instrument_Split] = SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split];
    }
    if(LoopEnd[ptk->Current_Instrument][ptk->Current_Instrument_Split] < 0) LoopEnd[ptk->Current_Instrument][ptk->Current_Instrument_Split] = 0;
    if(LoopEnd[ptk->Current_Instrument][ptk->Current_Instrument_Split] > SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split])
    {
        LoopEnd[ptk->Current_Instrument][ptk->Current_Instrument_Split] = SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split];
    }
    if(LoopStart[ptk->Current_Instrument][ptk->Current_Instrument_Split] == LoopEnd[ptk->Current_Instrument][ptk->Current_Instrument_Split])
    {
        LoopStart[ptk->Current_Instrument][ptk->Current_Instrument_Split] = 0;
        LoopEnd[ptk->Current_Instrument][ptk->Current_Instrument_Split] = SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split];
    }
}

// ------------------------------------------------------
// Bring the sample editor up to date
void Refresh_Sample(ptk_data *ptk, int clear_sel)
{
    int ReadOnly;
    int Allow = 0;
    if(!SampleType[ptk->Current_Instrument][ptk->Current_Instrument_Split]) Allow = BUTTON_DISABLED;

    if(SamplesSwap[ptk->Current_Instrument]) ReadOnly = BUTTON_DISABLED;
    else
    {
        ReadOnly = 0;
    }

    // Adjust after the cut
    if(sed_display_length > (int32) SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split])
    {
        sed_display_length = SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split];
    }
    if(sed_display_length + sed_display_start > (int32) SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split])
    {
        sed_display_start = SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split] - sed_display_length;
    }
    if(clear_sel)
    {
        sed_range_mode = FALSE;
        sed_range_start = 0;
        sed_range_end = 0;
    }
    ptk->draw_sampled_wave = TRUE;
    outlong(712, (Cur_Height - 60), sed_display_start, 10);
    outlong(712, (Cur_Height - 42), sed_display_length, 12);
    outlong(650, (Cur_Height - 60), sed_range_start, 10);
    outlong(650, (Cur_Height - 42), sed_range_end, 11);
    Check_Loops(ptk);
    if(ptk->userscreen == USER_SCREEN_INSTRUMENT_EDIT) Actualize_Instrument_Ed(ptk, 0, 4);
    Display_Sample_Buffers(ptk, Allow | ReadOnly);
}

// ------------------------------------------------------
// Notify the user selected buffer visually
void Display_Sample_Buffers(ptk_data *ptk, int Allow)
{
    switch(cur_sample_buffer)
    {
        case 0:
            cur_smp_buffer[0] = BUTTON_PUSHED;
            cur_smp_buffer[1] = BUTTON_NORMAL;
            cur_smp_buffer[2] = BUTTON_NORMAL;
            cur_smp_buffer[3] = BUTTON_NORMAL;
            break;
        case 1:
            cur_smp_buffer[0] = BUTTON_NORMAL;
            cur_smp_buffer[1] = BUTTON_PUSHED;
            cur_smp_buffer[2] = BUTTON_NORMAL;
            cur_smp_buffer[3] = BUTTON_NORMAL;
            break;
        case 2:
            cur_smp_buffer[0] = BUTTON_NORMAL;
            cur_smp_buffer[1] = BUTTON_NORMAL;
            cur_smp_buffer[2] = BUTTON_PUSHED;
            cur_smp_buffer[3] = BUTTON_NORMAL;
            break;
        case 3:
            cur_smp_buffer[0] = BUTTON_NORMAL;
            cur_smp_buffer[1] = BUTTON_NORMAL;
            cur_smp_buffer[2] = BUTTON_NORMAL;
            cur_smp_buffer[3] = BUTTON_PUSHED;
            break;
    }

    Gui_Draw_Button_Box(650, (Cur_Height - 150), 16, 16, "1", Allow | cur_smp_buffer[0] | BUTTON_TEXT_CENTERED | (Sample_Back_Size[0] ? 0 : BUTTON_LOW_FONT));
    Gui_Draw_Button_Box(668, (Cur_Height - 150), 16, 16, "2", Allow | cur_smp_buffer[1] | BUTTON_TEXT_CENTERED | (Sample_Back_Size[1] ? 0 : BUTTON_LOW_FONT));
    Gui_Draw_Button_Box(686, (Cur_Height - 150), 16, 16, "3", Allow | cur_smp_buffer[2] | BUTTON_TEXT_CENTERED | (Sample_Back_Size[2] ? 0 : BUTTON_LOW_FONT));
    Gui_Draw_Button_Box(704, (Cur_Height - 150), 16, 16, "4", Allow | cur_smp_buffer[3] | BUTTON_TEXT_CENTERED | (Sample_Back_Size[3] ? 0 : BUTTON_LOW_FONT));
}

// ------------------------------------------------------
// Display a selection
void Zoom_In_Sel(ptk_data *ptk)
{
    int max_length;
    sed_range_mode = FALSE;
    if(sed_range_end != sed_range_start)
    {
        if((int) sed_range_end < (int) sed_range_start)
        {
            int swap_range = sed_range_start;
            sed_range_start = sed_range_end;
            sed_range_end = swap_range;
        }
        sed_display_length = (sed_range_end - sed_range_start);
        sed_display_start = sed_range_start;
        max_length = SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split] - sed_display_length;
        if((int) sed_display_start > max_length) sed_display_start = max_length;

        ptk->draw_sampled_wave = TRUE;
        ptk->teac = 3;
        ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
    }
}

// ------------------------------------------------------
// Move from a selection
void Zoom_Out_Sel(ptk_data *ptk)
{
    int start_test;
    sed_display_start -= sed_display_length;
    start_test = sed_display_start;
    if(start_test < 0) sed_display_start = 0;
    sed_display_length *= 3;

    if(sed_display_length > (int32) SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split])
    {
        sed_display_length = SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split];
    }
    if(sed_display_length + sed_display_start > (int32) SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split])
    {
        sed_display_start = SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split] - sed_display_length;
    }
    sed_range_start = sed_display_start;
    sed_range_end = sed_display_start + sed_display_length;
    ptk->draw_sampled_wave = TRUE;
    ptk->teac = 3;
    ptk->gui_action = GUI_CMD_REFRESH_SAMPLE_ED;
}
