/*
 * @Author: xiuquanxu
 * @Company: kaochong
 * @Date: 2020-07-24 18:29:09
 * @LastEditors: xiuquanxu
 * @LastEditTime: 2020-07-24 19:06:55
 */ 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sonic.h"
#include "opus/opus.h"
#include "resample.h"

typedef struct Opus2Pcm {
  OpusDecoder *opus_decoder;
  rs_data *resample;
  sonicStream sonic_handle;
} as_opus2pcm_t;

int init(int inputSampleRate, int outSampleRate, int numChannel) {
  int err_code = 0;
  as_opus2pcm_t *opus2pcm = (as_opus2pcm_t *)malloc(sizeof(as_opus2pcm_t));
  opus2pcm->opus_decoder = opus_decoder_create(inputSampleRate, numChannel, &err_code);
  if (err_code != OPUS_OK) {
    printf(" init err_code != OPUS_OK, err_code:%d\n", err_code);
    return err_code;
  }
  opus2pcm->resample = InitRsData(inputSampleRate, outSampleRate);
  opus2pcm->sonic_handle = sonicCreateStream(outSampleRate, numChannel);
  return 0;
}

