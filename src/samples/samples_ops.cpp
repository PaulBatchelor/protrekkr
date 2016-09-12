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
#include "include/samples_ops.h"

#if defined(__AROS__)
#include <stdint.h>
#define int32 int32_t
#endif

// ------------------------------------------------------
// Variables
int cur_sample_buffer;
short *Sample_Back[4][2];
int Sample_Back_Channels[4];
int Sample_Back_Size[4];
extern short *Player_WL[MAX_TRACKS][MAX_POLYPHONY];
extern short *Player_WR[MAX_TRACKS][MAX_POLYPHONY];

// ------------------------------------------------------
// Rotate a selection to the left by a given amount
int Sample_Rotate_Left(ptk_data *ptk, int32 range_start, int32 range_end, int amount)
{
    int32 i;
    int j;
    short sample1;
    short sample2;
    char nc;
    long shiftsize = (range_end - range_start);
 
    if(shiftsize)
    {
        nc = SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split];

        for(j = 0; j < amount; j++)
        {
            sample1 = RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split][range_start];
            if(nc == 2) sample2 = RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split][range_start];
 
            // Shift it
            for(i = range_start; i < range_end - 1; i++)
            {
                RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split][i] = RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split][i + 1];
                if(nc == 2)
                {
                    RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split][i] = RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split][i + 1];
                }
            }
            RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split][i] = sample1;
            if(nc == 2) RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split][i] = sample2;
        }

        Status_Box(ptk, "Shift left done.");
        return 1;
    }
    return 0;
}

// ------------------------------------------------------
// Rotate a selection to the right by a given amount
int Sample_Rotate_Right(ptk_data *ptk, int32 range_start, int32 range_end, int amount)
{
    int32 i;
    int j;
    short sample1;
    short sample2;
    char nc;
    long shiftsize = (range_end - range_start);
 
    if(shiftsize)
    {
        nc = SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split];

        for(j = 0; j < amount; j++)
        {
            sample1 = RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split][range_end - 1];
            if(nc == 2) sample2 = RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split][range_end - 1];
 
            // Shift it
            for(i = range_end - 2; i >= range_start; i--)
            {
                RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split][i + 1] = RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split][i];
                if(nc == 2)
                {
                    RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split][i + 1] = RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split][i];
                }
            }
            RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split][range_start] = sample1;
            if(nc == 2) RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split][range_start] = sample2;
        }

        Status_Box(ptk, "Shift right done.");
        return 1;
    }
    return 0;
}

// ------------------------------------------------------
// Swap the data of a selection
int Sample_Reverse(ptk_data *ptk, int32 range_start, int32 range_end)
{
    int32 i;
    short sample;
    char nc;
    long p_s;
    long reversesize = (range_end - range_start) / 2;
 
    if(reversesize)
    {
        nc = SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split];

        p_s = range_end - 1;

        // Reverse it
        for(i = range_start; i < (range_start + reversesize); i++)
        {
            sample = RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split][p_s];
            RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split][p_s] = RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split][i];
            RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split][i] = sample;
            if(nc == 2)
            {
                sample = RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split][p_s];
                RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split][p_s] = RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split][i];
                RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split][i] = sample;
            }
            p_s--;
        }
        Status_Box(ptk, "Reverse done.");
        return 1;
    }
    return 0;
}

// ------------------------------------------------------
// Convert a selection into a whole sample
int Sample_Crop(ptk_data *ptk, int32 range_start, int32 range_end)
{
    int32 i;
    short *NewBuffer[2];
    char nc;
    long p_s;
    long cropsize = (range_end - range_start);

    if(cropsize)
    {
        Stop_Current_Instrument(ptk);
        AUDIO_Stop();

        nc = SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split];

        NewBuffer[0] = (short *) malloc(cropsize * 2 + 8);
        if(!NewBuffer[0]) return 0;
        memset(NewBuffer[0], 0, cropsize * 2 + 8);
        if(nc == 2)
        {
            NewBuffer[1] = (short *) malloc(cropsize * 2 + 8);
            if(!NewBuffer[1])
            {
                free(NewBuffer[0]);
                return 0;
            }
            memset(NewBuffer[1], 0, cropsize * 2 + 8);
        }

        p_s = 0;

        // Copy the selection
        for(i = range_start; i < range_end; i++)
        {
            *(NewBuffer[0] + p_s) = *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i);
            if(nc == 2) *(NewBuffer[1] + p_s) = *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i);
            p_s++;
        }

        // Set the new buffer as current sample
        if(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split]) free(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split]);
        RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] = NewBuffer[0];
        Player_WL[ptk->Current_Instrument][ptk->Current_Instrument_Split] = NewBuffer[0];
        if(nc == 2)
        {
            if(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split]) free(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split]);
            RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] = NewBuffer[1];
            Player_WR[ptk->Current_Instrument][ptk->Current_Instrument_Split] = NewBuffer[1];
        }
        SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split] = cropsize;

        Status_Box(ptk, "Crop done.");
        AUDIO_Play();
        return 1;
    }
    return 0;
}

// ------------------------------------------------------
// Copy part of a sample
int Sample_Copy(ptk_data *ptk, int32 range_start, int32 range_end)
{
    int i;
    short *dest_mono;
    short *dest_stereo;
    long copysize = (range_end - range_start);
    char nc = SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split];

    if(copysize)
    {
        // Free both back buffers
        if(Sample_Back[cur_sample_buffer][0]) free(Sample_Back[cur_sample_buffer][0]);
        if(Sample_Back[cur_sample_buffer][1]) free(Sample_Back[cur_sample_buffer][1]);
        Sample_Back[cur_sample_buffer][0] = NULL;
        Sample_Back[cur_sample_buffer][1] = NULL;

        // Copy the data into the back buffer
        Sample_Back[cur_sample_buffer][0] = (short *) malloc(copysize * 2 + 8);
        if(!Sample_Back[cur_sample_buffer][0]) return 0;
        memset(Sample_Back[cur_sample_buffer][0], 0, copysize * 2 + 8);
        if(nc == 2)
        {
            Sample_Back[cur_sample_buffer][1] = (short *) malloc(copysize * 2 + 8);
            if(!Sample_Back[cur_sample_buffer][1])
            {
                free(Sample_Back[cur_sample_buffer][0]);
                return 0;
            }
            memset(Sample_Back[cur_sample_buffer][1], 0, copysize * 2 + 8);
        }
        Sample_Back_Channels[cur_sample_buffer] = nc;
        Sample_Back_Size[cur_sample_buffer] = copysize;

        // Copy the selection into the back buffer
        dest_mono = Sample_Back[cur_sample_buffer][0];
        dest_stereo = Sample_Back[cur_sample_buffer][1];
        for(i = range_start; i < range_end; i++)
        {
            *dest_mono++ = *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i);
            if(nc == 2) *dest_stereo++ = *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i);
        }
        Status_Box(ptk, "Copy done.");
        return 1;
    }
    return 0;
}

// ------------------------------------------------------
// Paste a back buffer into a sample
int Sample_Paste(ptk_data *ptk, int32 range_start)
{
    int32 i;
    short *NewBuffer[2];
    char nc;
    long p_s;
    long cutsize = Sample_Back_Size[cur_sample_buffer];
    long newsize = SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split] + cutsize;

    if(cutsize)
    {
        Stop_Current_Instrument(ptk);
        AUDIO_Stop();

        nc = SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split];

        // Allocate the destination buffer(s)
        // (We need to clear the second one as the back buffer may not be stereo).
        NewBuffer[0] = (short *) malloc(newsize * 2 + 8);
        if(!NewBuffer[0]) return 0;
        memset(NewBuffer[0], 0, newsize * 2 + 8);

        if(nc == 2)
        {
            NewBuffer[1] = (short *) malloc(newsize * 2 + 8);
            if(!NewBuffer[1])
            {
                free(NewBuffer[0]);
                return 0;
            }
            memset(NewBuffer[1], 0, newsize * 2 + 8);
        }

        p_s = 0;
        if(range_start > 0)
        {
            // Copy the original data into the new buffer
            for(i = 0; i < range_start; i++)
            {
                *(NewBuffer[0] + p_s) = *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i);
                if(nc == 2) *(NewBuffer[1] + p_s) = *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i);
                p_s++;
            }
        }

        // Copy the back buffer(s) now
        for(i = 0; i < cutsize; i++)
        {
            *(NewBuffer[0] + p_s) = *(Sample_Back[cur_sample_buffer][0] + i);
            if(Sample_Back_Size[cur_sample_buffer] == 2 && nc == 2) *(NewBuffer[1] + p_s) = *(Sample_Back[cur_sample_buffer][1] + i);
            p_s++;
        }

        if((SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split] - range_start) > 0)
        {
            // Add the rest of the original data
            for(i = range_start; i < (int32) SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split]; i++)
            {
                *(NewBuffer[0] + p_s) = *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i);
                if(nc == 2) *(NewBuffer[1] + p_s) = *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i);
                p_s++;
            }
        }

        // Set the new buffer as current sample
        if(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split]) free(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split]);
        RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] = NewBuffer[0];
        Player_WL[ptk->Current_Instrument][ptk->Current_Instrument_Split] = NewBuffer[0];
        if(nc == 2)
        {
            if(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split]) free(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split]);
            RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] = NewBuffer[1];
            Player_WR[ptk->Current_Instrument][ptk->Current_Instrument_Split] = NewBuffer[1];
        }
        SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split] = newsize;

        Status_Box(ptk, "Paste done.");
        AUDIO_Play();
        return 1;
    }
    return 0;
}

// ------------------------------------------------------
// Cut part of a sample
int Sample_Cut(ptk_data *ptk, int32 range_start, int32 range_end, int do_copy)
{
    int32 i;
    short *NewBuffer[2];
    char nc;
    long p_s;
    long cutsize = (range_end - range_start);
    long newsize = SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split] - cutsize;

    if(newsize)
    {
        Stop_Current_Instrument(ptk);
        AUDIO_Stop();

        nc = SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split];

        // Allocate the wav with the minus the block to cut
        NewBuffer[0] = (short *) malloc(newsize * 2 + 8);
        if(!NewBuffer[0]) return 0;
        memset(NewBuffer[0], 0, newsize * 2 + 8);
        if(nc == 2)
        {
            NewBuffer[1] = (short *) malloc(newsize * 2 + 8);
            if(!NewBuffer[1])
            {
                free(NewBuffer[0]);
                return 0;
            }
            memset(NewBuffer[1], 0, newsize * 2 + 8);
        }

        if(do_copy)
        {
            if(!Sample_Copy(ptk, range_start, range_end))
            {
                free(NewBuffer[1]);
                free(NewBuffer[0]);
                return 0;
            }
        }

        p_s = 0;
        if(range_start > 0)
        {
            // Copy the data located before the range start
            for(i = 0; i < range_start; i++)
            {
                *(NewBuffer[0] + p_s) = *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i);
                if(nc == 2) *(NewBuffer[1] + p_s) = *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i);
                p_s++;
            }
        }

        if((SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split] - range_end) > 0)
        {
            // Add the data located after the range end
            for(i = range_end; i < (int32) SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split]; i++)
            {
                *(NewBuffer[0] + p_s) = *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i);
                if(nc == 2) *(NewBuffer[1] + p_s) = *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i);
                p_s++;
            }
        }

        // Set the new buffer as current sample
        if(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split]) free(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split]);
        RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] = NewBuffer[0];
        Player_WL[ptk->Current_Instrument][ptk->Current_Instrument_Split] = NewBuffer[0];
        if(nc == 2)
        {
            if(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split]) free(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split]);
            RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] = NewBuffer[1];
            Player_WR[ptk->Current_Instrument][ptk->Current_Instrument_Split] = NewBuffer[1];
        }
        SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split] = newsize;

        Status_Box(ptk, "Cut done.");
        AUDIO_Play();
        return 1;
    }
    else
    {
        if(do_copy)
        {
            Status_Box(ptk, "You cannot cut entire sample, use 'delete' on instrument instead.");
        }
        else
        {
            Status_Box(ptk, "You cannot zap entire sample, use 'delete' on instrument instead.");
        }
        return 0;
    }
}

// ------------------------------------------------------
// Adjust DC of a sample
void Sample_DC_Adjust(ptk_data *ptk, int32 range_start, int32 range_end)
{
    int32 i;
    char nc = SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split];
    float l_shift = 0;
    float r_shift = 0;

    for(i = range_start; i < range_end + 1; i++)
    {
        l_shift += *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i);
        if(nc == 2) r_shift += *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i);
    }

    l_shift /= (range_end + 1) - range_start;
    r_shift /= (range_end + 1) - range_start;

    for(i = range_start; i < range_end + 1; i++)
    {
        float bleak = *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i);
        bleak -= l_shift;

        if(bleak > 32767) bleak = 32767;
        if(bleak < -32767) bleak = -32767;
        *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i) = (short) bleak;

        if(nc == 2)
        {
            bleak = *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i);
            bleak -= r_shift;

            if(bleak > 32767) bleak = 32767;
            if(bleak < -32767) bleak = -32767;
            *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i) = (short) bleak;
        }
    }

    draw_sampled_wave = TRUE;
    Status_Box(ptk, "DC adjust done.");
}

// ------------------------------------------------------
// Maximize a sample
void Sample_Maximize(ptk_data *ptk, int32 range_start, int32 range_end)
{
    int32 i;
    char nc = SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split];
    float l_shift = 0;

    for(i = range_start; i < range_end + 1; i++)
    {
        if(abs(*(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i)) > l_shift)
        {
            l_shift = *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i);
        }
        if(nc == 2)
        {
            if(abs(*(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i)) > l_shift)
            {
                l_shift = *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i);
            }
        }
    }

    l_shift = 32768.0f / l_shift;

    for(i = range_start; i < range_end + 1; i++)
    {
        float bleak = *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i);
        bleak *= l_shift;

        if(bleak > 32767) bleak = 32767;
        if(bleak < -32767) bleak = -32767;
        *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i) = (short) bleak;

        if(nc == 2)
        {
            bleak = *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i);
            bleak *= l_shift;

            if(bleak > 32767) bleak = 32767;
            if(bleak < -32767) bleak = -32767;
            *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i) = (short) bleak;
        }
    }

    draw_sampled_wave = TRUE;
    Status_Box(ptk, "Maximize done.");
}

// ------------------------------------------------------
// Zeroize a sample
void Sample_Zeroize(ptk_data *ptk, int32 range_start, int32 range_end)
{
    int32 i;
    char nc = SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split];

    for(i = range_start; i < range_end + 1; i++)
    {
        *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i) = 0;
        if(nc == 2)
        {
            *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i) = 0;
        }
    }

    draw_sampled_wave = TRUE;
    Status_Box(ptk, "Zero done.");
}

// ------------------------------------------------------
// Fade a sample in
void Sample_FadeIn(ptk_data *ptk, int32 range_start, int32 range_end)
{
    int i;
    char nc = SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split];
    float c_vol = 0.0f;
    float const coef_vol = 1.0f / ((range_end + 1) - range_start);

    for(i = range_start; i < range_end + 1; i++)
    {
        float bleak = *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i);
        bleak *= c_vol;
        if(bleak > 32767) bleak = 32767;
        if(bleak < -32767) bleak = -32767;

        *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i) = (short) bleak;

        if(nc == 2)
        {
            bleak = *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i);
            bleak *= c_vol;

            if(bleak > 32767) bleak = 32767;
            if(bleak < -32767) bleak = -32767;
            *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i) = (short) bleak;
        }
        c_vol += coef_vol;
    }

    draw_sampled_wave = TRUE;
    Status_Box(ptk, "Fade in done.");
}

// ------------------------------------------------------
// Fade a sample out
void Sample_FadeOut(ptk_data *ptk, int32 range_start, int32 range_end)
{
    int32 i;
    Status_Box(ptk, "Fade Out Selection...");
    SDL_Delay(100);

    char nc = SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split];

    float c_vol = 1.0f;
    float const coef_vol = 1.0f / ((range_end + 1) - range_start);

    for(i = range_start; i < range_end + 1; i++)
    {
        float bleak = *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i);
        bleak *= c_vol;
        if(bleak > 32767) bleak = 32767;
        if(bleak < -32767) bleak = -32767;

        *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i) = (short) bleak;

        if(nc == 2)
        {
            bleak = *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i);
            bleak *= c_vol;

            if(bleak > 32767) bleak = 32767;
            if(bleak < -32767) bleak = -32767;
            *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i) = (short) bleak;
        }
        c_vol -= coef_vol;
    }

    draw_sampled_wave = TRUE;
    Status_Box(ptk, "Fade out done.");
}

// ------------------------------------------------------
// Half a sample
void Sample_Half(ptk_data *ptk, int32 range_start, int32 range_end)
{
    int32 i;

    char nc = SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split];

    float c_vol = 0.5f;

    for(i = range_start; i < range_end + 1; i++)
    {
        float bleak = *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i);
        bleak *= c_vol;

        *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i) = (short) bleak;
        if(nc == 2)
        {
            bleak = *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i);
            bleak *= c_vol;
            *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i) = (short) bleak;
        }
    }

    draw_sampled_wave = TRUE;
    Status_Box(ptk, "Half done.");
}

// ------------------------------------------------------
// Duplicate the current selection
int Sample_Duplicate(ptk_data *ptk, int32 range_start, int32 range_end)
{
    int32 i;
    short *NewBuffer[2];
    char nc;
    long p_s;
    long cutsize = range_end - range_start;
    long newsize = SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split] + cutsize;

    if(cutsize)
    {
        Stop_Current_Instrument(ptk);
        AUDIO_Stop();

        nc = SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split];

        // Allocate the destination buffer(s)
        // (We need to clear the second one as the back buffer may not be stereo).
        NewBuffer[0] = (short *) malloc(newsize * 2 + 8);
        if(!NewBuffer[0]) return 0;
        memset(NewBuffer[0], 0, newsize * 2 + 8);

        if(nc == 2)
        {
            NewBuffer[1] = (short *) malloc(newsize * 2 + 8);
            if(!NewBuffer[1])
            {
                free(NewBuffer[0]);
                return 0;
            }
            memset(NewBuffer[1], 0, newsize * 2 + 8);
        }

        p_s = 0;
        if(range_start > 0)
        {
            // Copy the original data into the new buffer
            for(i = 0; i < range_start; i++)
            {
                *(NewBuffer[0] + p_s) = *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i);
                if(nc == 2) *(NewBuffer[1] + p_s) = *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i);
                p_s++;
            }
        }

        // Copy the selection
        for(i = range_start; i < range_end; i++)
        {
            *(NewBuffer[0] + p_s) = *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i);
            if(nc == 2) *(NewBuffer[1] + p_s) = *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i);
            p_s++;
        }

        if((SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split] - range_start) > 0)
        {
            // Add the rest of the original data
            for(i = range_start; i < (int32) SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split]; i++)
            {
                *(NewBuffer[0] + p_s) = *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i);
                if(nc == 2) *(NewBuffer[1] + p_s) = *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i);
                p_s++;
            }
        }

        // Set the new buffer as current sample
        if(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split]) free(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split]);
        RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] = NewBuffer[0];
        Player_WL[ptk->Current_Instrument][ptk->Current_Instrument_Split] = NewBuffer[0];
        if(nc == 2)
        {
            if(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split]) free(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split]);
            RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] = NewBuffer[1];
            Player_WR[ptk->Current_Instrument][ptk->Current_Instrument_Split] = NewBuffer[1];
        }
        SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split] = newsize;

        Status_Box(ptk, "Insert zeroes done.");
        AUDIO_Play();
        return 1;
    }
    return 0;
}

// ------------------------------------------------------
// Insert zeroes into a sample the length of the selection buffer
int Sample_InsertZero(ptk_data *ptk, int32 range_start, int32 range_end)
{
    int32 i;
    short *NewBuffer[2];
    char nc;
    long p_s;
    long cutsize = range_end - range_start;
    long newsize = SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split] + cutsize;

    if(cutsize)
    {
        Stop_Current_Instrument(ptk);
        AUDIO_Stop();

        nc = SampleChannels[ptk->Current_Instrument][ptk->Current_Instrument_Split];

        // Allocate the destination buffer(s)
        // (We need to clear the second one as the back buffer may not be stereo).
        NewBuffer[0] = (short *) malloc(newsize * 2 + 8);
        if(!NewBuffer[0]) return 0;
        memset(NewBuffer[0], 0, newsize * 2 + 8);

        if(nc == 2)
        {
            NewBuffer[1] = (short *) malloc(newsize * 2 + 8);
            if(!NewBuffer[1])
            {
                free(NewBuffer[0]);
                return 0;
            }
            memset(NewBuffer[1], 0, newsize * 2 + 8);
        }

        p_s = 0;
        if(range_start > 0)
        {
            // Copy the original data into the new buffer
            for(i = 0; i < range_start; i++)
            {
                *(NewBuffer[0] + p_s) = *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i);
                if(nc == 2) *(NewBuffer[1] + p_s) = *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i);
                p_s++;
            }
        }

        // Skip a buffer of the selection's size
        p_s += cutsize;

        if((SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split] - range_start) > 0)
        {
            // Add the rest of the original data
            for(i = range_start; i < (int32) SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split]; i++)
            {
                *(NewBuffer[0] + p_s) = *(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] + i);
                if(nc == 2) *(NewBuffer[1] + p_s) = *(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] + i);
                p_s++;
            }
        }

        // Set the new buffer as current sample
        if(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split]) free(RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split]);
        RawSamples[ptk->Current_Instrument][0][ptk->Current_Instrument_Split] = NewBuffer[0];
        Player_WL[ptk->Current_Instrument][ptk->Current_Instrument_Split] = NewBuffer[0];
        if(nc == 2)
        {
            if(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split]) free(RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split]);
            RawSamples[ptk->Current_Instrument][1][ptk->Current_Instrument_Split] = NewBuffer[1];
            Player_WR[ptk->Current_Instrument][ptk->Current_Instrument_Split] = NewBuffer[1];
        }
        SampleLength[ptk->Current_Instrument][ptk->Current_Instrument_Split] = newsize;

        Status_Box(ptk, "Duplicate done.");
        AUDIO_Play();
        return 1;
    }
    return 0;
}
