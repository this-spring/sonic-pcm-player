/*
 * @Author: xiuquanxu
 * @Company: kaochong
 * @Date: 2020-07-24 18:29:09
 * @LastEditors: xiuquanxu
 * @LastEditTime: 2020-12-18 12:28:47
*/ 
#include "opus2pcmsonic.h"

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
  sonicSetSpeed(opus2pcm->sonic_handle, 1.0);
  // 为in_data和out_data申请空间
  opus2pcm->in_data = (uint8_t *)malloc(sizeof(char) * INPUT_MAX_LEN);
  opus2pcm->out_data = (uint8_t *)malloc(sizeof(char) * INPUT_MAX_LEN);
  return (uint8_t *)opus2pcm;
}

void setSpeed(uint8_t* dec, int speed) {
  as_opus2pcm_t *o2p_p = (as_opus2pcm_t *)dec;
  printf(" setSpeed speed:%d\n", speed);
  // 由于wasm和js之间只能传递int类型，所以这里做一个*10操作，在c层在做一个/10操作
  float rspeed = (float)speed / 100;
  printf(" setSpeed rspeed:%f\n", rspeed);
  sonicSetSpeed(o2p_p->sonic_handle, rspeed);
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
  // printf(" input_len:%d\n", input_len);
  int cout = 0;
  while (position < input_len && cout <= 500) {
    cout += 1;
    data_size = o2p_p->in_data[position];
    // printf(" data_size:%d cout:%d position:%d\n", data_size, cout, position);
    memcpy(payload_content, o2p_p->in_data + position + 1, data_size);
    // 解码opus
    frame_size = opus_decode(o2p_p->opus_decoder, payload_content, data_size, pcm_content, output_samples, 0);
    // printf(" frame_size:%d\n", frame_size);
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

    // printf(" outputSampleLength:%d\n", outputSampleLength);
    // sonic倍速
    int write_res = sonicWriteShortToStream(o2p_p->sonic_handle, (short *)after_reasmple, outputSampleLength);
    // printf(" write_res:%d\n", write_res);
    if (write_res != 1) {
      return ERROR;
    }
    unsigned char sonic_res[44100 * 2];
    int read_res = sonicReadShortFromStream(o2p_p->sonic_handle, (short *)sonic_res, outputSampleLength);
    // printf(" read_res:%d\n", read_res);
    if (read_res != 0) {
      // printf(" copy to after_sonic");
      memcpy(after_sonic + after_sonic_pos, sonic_res, read_res * 2);
      after_sonic_pos += read_res * 2;
    }
    position += 1 + data_size;
    // printf(" position: %d, after_sonic_pos:%d\n", position, after_sonic_pos);
  }
  memcpy(output, after_sonic, after_sonic_pos);
  return after_sonic_pos;
}

// **************************Pcm2Opus部分**************************

uint8_t* OpusInitEncoder(uint32_t frame_size, int inRate, int outRate) {
        
    int inSampleRate = inRate;
    int outSampleRate = outRate;
    as_resample_t *resample_point = (as_resample_t *)malloc(sizeof(as_resample_t));
    resample_point->resample = InitRsData(inSampleRate, outSampleRate);

    int err = 0;
    int complexity = 1;
    opus_int32 bitrate_bps = 64000;
    as_o2p_encoder_t *o2p_encoder = (as_o2p_encoder_t *)malloc(sizeof(as_o2p_encoder_t));
    // printf("OpusInitEncoder SAMPLE_RATE:%d, CHANNLE_NUM:%d\n", SAMPLE_RATE, CHANNEL_NUM);
    o2p_encoder->encoder = opus_encoder_create(SAMPLE_RATE, CHANNEL_NUM, OPUS_APPLICATION_VOIP, &err);
    o2p_encoder->max_data_bytes = (frame_size << 1);
    o2p_encoder->frame_size = frame_size;
    o2p_encoder->in_data = (input_data_t *)malloc(sizeof(input_data_t) + frame_size * 10);
    o2p_encoder->out_data = (output_data_t *)malloc(sizeof(output_data_t) + 1024 * 1024 * 2);

    if (err != OPUS_OK) {
        printf("cannot create opus encoder, code:%d, error_message:%s\n", err, opus_strerror(err));
        return NULL;
    }
    opus_encoder_ctl(o2p_encoder->encoder, OPUS_SET_BANDWIDTH(OPUS_BANDWIDTH_WIDEBAND));
    opus_encoder_ctl(o2p_encoder->encoder, OPUS_SET_BITRATE(bitrate_bps));
    opus_encoder_ctl(o2p_encoder->encoder, OPUS_SET_VBR(1));
    opus_encoder_ctl(o2p_encoder->encoder, OPUS_SET_COMPLEXITY(complexity));
    opus_encoder_ctl(o2p_encoder->encoder, OPUS_SET_INBAND_FEC(0));
    opus_encoder_ctl(o2p_encoder->encoder, OPUS_SET_FORCE_CHANNELS(OPUS_AUTO));
    opus_encoder_ctl(o2p_encoder->encoder, OPUS_SET_DTX(0));
    opus_encoder_ctl(o2p_encoder->encoder, OPUS_SET_LSB_DEPTH(16));

    as_o2p_resample_t *o2p_resample = (as_o2p_resample_t *)malloc(sizeof(as_o2p_resample_t));
    o2p_resample->encoder = o2p_encoder;
    o2p_resample->resample = resample_point;
    return (uint8_t *)o2p_resample;
}

int PCMEncoderData(uint8_t* dec, uint8_t* input, int input_len, uint8_t* output) {

    as_o2p_resample_t *o2p_resample = (as_o2p_resample_t *)dec;
    as_o2p_encoder_t *p2o_encoder = o2p_resample->encoder;
    as_resample_t *resample_point = o2p_resample->resample;

    memcpy((uint8_t *)resample_point->in_data, input, input_len * 2);
    
    int outputSampleLength = SrcLinear3((short *)resample_point->in_data, (short *)resample_point->out_data, input_len, 2, resample_point->resample);
    // printf("PCMEncoderData before SrcLinear3 input_len:%d outputlen %d\n", input_len, outputSampleLength);

    // 实现resample返给js 方便调试resample后的数据
    // memcpy(output, (uint8_t *) resample_point->out_data, outputSampleLength * 2);
    // return 2 * outputSampleLength;

    memcpy((pcm + encode_pcm_buffer_position), (uint8_t *)resample_point->out_data, outputSampleLength * 2);
    encode_pcm_buffer_position += 2 * outputSampleLength;

    // printf("before encode pcm data len %d encode pcm samples %d\n", encode_pcm_buffer_position, encode_pcm_buffer_size);
    int flag = -1;
    int return_opus_len = 0;
    while(1) {
        // printf("encode_pcm_encode_count:%d, return_opus_len:%d \n", encode_pcm_encode_count, return_opus_len);

        if (encode_pcm_buffer_position < (encode_pcm_buffer_size * 2)) {
            break;
        } else {
            int ret = opus_encode(p2o_encoder->encoder, (opus_int16 *)pcm, encode_pcm_buffer_size,
                p2o_encoder->out_data->data, p2o_encoder->max_data_bytes);

            memcpy(pcm, (pcm + (encode_pcm_buffer_size * 2)), encode_pcm_buffer_position - (encode_pcm_buffer_size * 2));
            encode_pcm_buffer_position -= (encode_pcm_buffer_size * 2);
            if (ret < 0) {
                printf("error, PCMEncoderData ret < 0, error message:%s \n", opus_strerror(ret));
            } else {
                encode_pcm_encode_count ++;
                // printf("PCMEncoderData ret:%d\n", ret);
                *(output + encode_output_len) = ret;
                memcpy((output + encode_output_len + 1), p2o_encoder->out_data->data, ret);
                encode_output_len += ret + 1;
            }
        }
        if ((encode_pcm_encode_count) % 10 == 0) {
            flag = 1;
            return_opus_len = encode_output_len;
            encode_output_len = 0;
            // printf("encode_pcm_encode_count:%d, return_opus_len:%d \n", encode_pcm_encode_count, return_opus_len);
            break;
        }
    }
    // printf("after encode pcm data left len %d output len %d\n", encode_pcm_buffer_position, output_len);
    // return output_len; 
    if (flag == 1) {
        flag = -1;
        return return_opus_len;
    } else {
        return -1;
    }
}

void resetPcmEncode() {
    encode_output_len = 0;
    encode_pcm_buffer_position = 0;
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