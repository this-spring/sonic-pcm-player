/*
 * @Author: xiuquanxu
 * @Company: kaochong
 * @Date: 2020-08-05 15:49:26
 * @LastEditors: xiuquanxu
 * @LastEditTime: 2020-08-05 15:57:47
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sonic/sonic.h"
#include "opus/opus.h"
#include "resample.h"


#define SUCCESS 0
#define ERROR -1
#define SAMPLE_RATE 48000
#define CHANNEL_NUM 1

const int MAX_FRAME_SIZE = 6 * 690;
const int CHANNELS = 1;
const int INPUT_MAX_LEN = 48000; // opus的长度为：一个字节

int encode_pcm_buffer_size = 480;
int encode_pcm_buffer_position = 0;
int encode_pcm_encode_count = 0;
int encode_output_len = 0;
uint8_t pcm[1024 * 10];

opus_int32 output_samples = 48000;

typedef struct Opus2Pcm {
  OpusDecoder *opus_decoder;
  rs_data *resample;
  sonicStream sonic_handle;
  uint8_t *in_data;
  uint8_t *out_data;
} as_opus2pcm_t;

typedef struct {
    /* data */
    uint32_t stream_id;
    opus_int16 data[];
} input_data_t;

typedef struct {
    /* data */
    uint32_t stream_id;
    uint8_t data[];
} output_data_t;

typedef struct {
    /* data */
    rs_data *resample;
    uint8_t in_data[48000];
    uint8_t out_data[48000];
} as_resample_t;

typedef struct {
    /* data */
    OpusEncoder *encoder;
    opus_int32 max_data_bytes;
    uint32_t frame_size;
    input_data_t *in_data;
    output_data_t *out_data;
} as_o2p_encoder_t;

typedef struct {
    /* data */
    as_o2p_encoder_t *encoder;
    as_resample_t *resample;
} as_o2p_resample_t;