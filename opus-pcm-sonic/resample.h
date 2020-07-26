/* resample.h - sampling rate conversion subroutines
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

#define log(format, args...)                                                                                                                                                                           \
    do {                                                                                                                                                                                               \
        char buf[1024] = { '\0' };                                                                                                                                                                     \
        int len = snprintf(buf, 1024, format, args);                                                                                                                                                   \
        AS3_DeclareVar(tmp, String);                                                                                                                                                                   \
        AS3_CopyCStringToVar(tmp, buf, len);                                                                                                                                                           \
        AS3_Trace(tmp);                                                                                                                                                                                \
    } while (0)

#define FP_DIGITS 15
#define FP_FACTOR (1 << FP_DIGITS) // 32768
#define FP_MASK (FP_FACTOR - 1)    // 32767

#define MAX_HWORD (32767)
#define MIN_HWORD (-32768)

#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif
#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif

typedef struct _rs_data {
    float factor;
    float time;

    short *in_buf;
    int in_buf_size;
    int in_total_index;

    short *out_buf;
    int out_buf_size;
    int out_total_index;
} rs_data;

// int resample_simple(double factor, short *in_buf, short *out_buf, int buffer_size);
//
// struct rs_data *resample_init(int in_rate, int out_rate, int buffer_size);
//
// int resample(struct rs_data *rs, short *in_buf, int in_buf_size, short *out_buf,
//         int out_buf_size, int last);
//
// void resample_close(struct rs_data *rs);

// int SrcLinear(short X[], short Y[], double factor, unsigned int *Time,
//             unsigned short Nx);

rs_data *InitRsData(int inSampleRate, int outSampleRate);

int SrcLinear2(short X[], short Y[], double factor, double *TimeFloat, unsigned int *xIndex, unsigned int x_size);

int SrcLinear3(short X[], short Y[], int samples, int frameBytes, rs_data *rd);

void DestroyRsData(rs_data *rd);
