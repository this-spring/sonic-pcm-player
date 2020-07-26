/*
 **************************************************************************************
 *                      CopyRight (C) KaoChong Corp, Ltd.
 *
 *       Filename:  resample.c
 *    Description:   source file
 *
 *        Version:  1.0
 *        Created:  2019/06/04 16时22分30秒
 *         Author:  starhan     [hanxing@kaochong.com]
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */




/* resample.c - sampling rate conversion subroutines
 *
 * Original version available at the
 * Digital Audio Resampling Home Page located at
 * http://ccrma.stanford.edu/~jos/resample/.
 *
 * Modified for use on Android by Ethan Chen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "resample.h"

#define IBUFFSIZE 4096 /* default input buffer size */

static inline short WordToHword(int v, int scl) {
    short out;
    int llsb = (1 << (scl - 1));
    v += llsb; /* round */
    v >>= scl;
    if (v > MAX_HWORD) {
        v = MIN_HWORD;
    }
    out = (short)v;
    return out;
}

int SrcLinear3(short X[], short Y[], int samples, int frameSize, rs_data *rd) {
    short iconst;
    short *Xp;
    int v, x1, x2;
    double dt, factor; /* Step through input signal */

    dt = 1.0 / rd->factor;
    unsigned int preXIndex = rd->in_total_index;

    memcpy(rd->in_buf + rd->in_buf_size, X, samples << 1);
    rd->in_buf_size += samples;

    while (1) {
        int tmpIndex = (int)rd->time;
        //到最后一个采样就停止
        if (tmpIndex - preXIndex >= rd->in_buf_size - 1) {
            int used_index = rd->in_total_index - preXIndex;
            memcpy(rd->in_buf, rd->in_buf + used_index, (rd->in_buf_size - used_index) << 1);
            rd->in_buf_size -= used_index;

            rd->time = (rd->in_buf_size) * dt;
            rd->in_total_index = (int)rd->time - 1;
            rd->out_total_index = 0;

            break;
        }

        rd->in_total_index = tmpIndex;

        Xp = &rd->in_buf[rd->in_total_index - preXIndex]; /* Ptr to current input sample is top 16 bits， 换算出当前input的index */

        iconst = (int)(FP_FACTOR * dt * rd->out_total_index) & FP_MASK;

        x1 = *Xp++;
        x2 = *Xp;

        x1 *= FP_FACTOR - iconst;
        x2 *= iconst;

        v = x1 + x2;
        rd->out_buf[rd->out_buf_size++] = WordToHword(v, FP_DIGITS); /* Deposit output */
        rd->time += dt;                                              /* Move to next sample by time increment */

        rd->out_total_index++;
    }

    int pieces = rd->out_buf_size / frameSize;
    int sampleLen = pieces * frameSize;
    memcpy(Y, rd->out_buf, sampleLen << 1);
    memcpy(rd->out_buf, rd->out_buf + sampleLen, (rd->out_buf_size - sampleLen) << 1);
    rd->out_buf_size -= sampleLen;

    return sampleLen;
}

rs_data *InitRsData(int inSampleRate, int outSampleRate) {
    rs_data *rd = (rs_data *)malloc(sizeof(rs_data));

    rd->factor = (double)outSampleRate / (double)inSampleRate;

    rd->in_buf = (short *)malloc(sizeof(short) * 1024 * 1024);
    rd->in_buf_size = 0;
    rd->in_total_index = 0;

    rd->out_buf = (short *)malloc(sizeof(short) * 1024 * 1024);
    rd->out_buf_size = 0;
    rd->out_total_index = 0;

    rd->time = 0;

    return rd;
}

void DestroyRsData(rs_data *rd) {
    free(rd->in_buf);
    free(rd->out_buf);
    free(rd);
}

/********************************** END **********************************************/