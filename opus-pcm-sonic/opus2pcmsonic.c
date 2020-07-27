/*
 * @Author: xiuquanxu
 * @Company: kaochong
 * @Date: 2020-07-24 18:29:09
 * @LastEditors: xiuquanxu
 * @LastEditTime: 2020-07-27 18:54:08
 */ 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sonic/sonic.h"
#include "opus/opus.h"
#include "resample.h"

#define SUCCESS 0;
#define ERROR -1;

const int MAX_FRAME_SIZE = 6 * 690;
const int CHANNELS = 1;
const int INPUT_MAX_LEN = 256 * 2; // opus的长度为：一个字节

opus_int32 output_samples = 48000;

typedef struct Opus2Pcm {
  OpusDecoder *opus_decoder;
  rs_data *resample;
  sonicStream sonic_handle;
  uint8_t *in_data;
  uint8_t *out_data;
} as_opus2pcm_t;

uint8_t* init(int inputSampleRate, int outSampleRate, int numChannel) {
  int err_code = 0;
  as_opus2pcm_t *opus2pcm = (as_opus2pcm_t *)malloc(sizeof(as_opus2pcm_t));
  opus2pcm->opus_decoder = opus_decoder_create(inputSampleRate, numChannel, &err_code);
  if (err_code != OPUS_OK) {
    printf(" init err_code != OPUS_OK, err_code:%d\n", err_code);
    return (uint8_t *)ERROR;
  }
  opus2pcm->resample = InitRsData(inputSampleRate, outSampleRate);
  opus2pcm->sonic_handle = sonicCreateStream(outSampleRate, numChannel);
  sonicSetSpeed(opus2pcm->sonic_handle, 2);
  // 为in_data和out_data申请空间
  opus2pcm->in_data = (uint8_t *)malloc(sizeof(char) * INPUT_MAX_LEN);
  return (uint8_t *)opus2pcm;
}

int MakePcmStream(uint8_t* dec, uint8_t* input, int input_len, uint8_t* output) {
  as_opus2pcm_t *o2p_p = (as_opus2pcm_t *)dec;
  memcpy(o2p_p->in_data, input, input_len);
  int position =  0;
  int data_size = 0;
  int frame_size = 0;
  uint8_t payload_content[INPUT_MAX_LEN];
  opus_int16 pcm_content[MAX_FRAME_SIZE * CHANNELS];
  // copy sonic后结果
  int after_sonic_pos = 0;
  uint8_t after_sonic[4410 * 5 * 100];
  printf(" input_len:%d\n", input_len);
  int cout = 0;
  while (position < input_len && cout <= 500) {
    cout += 1;
    data_size = o2p_p->in_data[position];
    printf(" data_size:%d cout:%d\n", data_size, cout);
    memcpy(payload_content, o2p_p->in_data + position + 1, data_size);
    // 解码opus
    frame_size = opus_decode(o2p_p->opus_decoder, payload_content, data_size, pcm_content, output_samples, 0);
    printf(" frame_size:%d\n", frame_size);
    unsigned char pcm_bytes[frame_size * CHANNELS * 2];
    if (frame_size < 0) {
      return ERROR;
    }
    int i = 0;
    for (;i < CHANNELS * frame_size; i += 1) {
      pcm_bytes[2 * i] = pcm_content[i] & 0xFF;
      pcm_bytes[2 * i + 1] = (pcm_content[i] >> 8) & 0xFF;
    }
    // 重采样
    uint8_t after_reasmple[48000];
    int outputSampleLength = SrcLinear3((short *)pcm_bytes, (short *)after_reasmple, i, 2, o2p_p->resample);

    printf(" outputSampleLength:%d\n", outputSampleLength);
    // sonic倍速
    int write_res = sonicWriteShortToStream(o2p_p->sonic_handle, (short *)after_reasmple, outputSampleLength);
    printf(" write_res:%d\n", write_res);
    if (write_res != 1) {
      return ERROR;
    }
    unsigned char sonic_res[44100 * 2];
    int read_res = sonicReadShortFromStream(o2p_p->sonic_handle, (short *)sonic_res, outputSampleLength);
    printf(" read_res:%d\n", read_res);
    if (read_res != 0) {
      printf(" copy to after_sonic");
      memcpy(after_sonic + after_sonic_pos, sonic_res, read_res * 2);
      after_sonic_pos += read_res * 2;
    }
    position += 1 + data_size;
    printf(" position: %d, after_sonic_pos:%d\n", position, after_sonic_pos);
  }
  memcpy(output, after_sonic, after_sonic_pos);
  return after_sonic_pos;
}

// int main() {
//   FILE *fin = NULL;
//   FILE *fout = NULL;
//   fin = fopen("/Users/xuxiuquan/mygithub/sonic-pcm-player/opus-pcm-sonic/test.opus", "rb");
//   fout = fopen("/Users/xuxiuquan/mygithub/sonic-pcm-player/opus-pcm-sonic/output.pcm", "wb");
//   int read_len = 35000 * 2;
//   uint8_t read_content[read_len];
//   fread(read_content, read_len, 1, fin);
//   // 
//   uint8_t *o2p_p = init(48000, 44100, 1);
//   // 返回容器
//   uint8_t pcm_res[1024 * 1024];
//   int res_len = MakePcmStream(o2p_p, read_content, read_len, pcm_res);
//   printf(" res_len:%d\n", res_len);
//   fwrite(pcm_res, res_len, 1, fout);
//   return 1;
// }