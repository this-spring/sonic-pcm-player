/*
 * @Author: xiuquanxu
 * @Company: kaochong
 * @Date: 2020-04-12 16:49:47
 * @LastEditors: xiuquanxu
 * @LastEditTime: 2020-07-24 11:35:18
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "sonic.h"

#define TAG "wrapper_sonic"

typedef struct {
    sonicStream sonic_handle;
    uint8_t *in_data;
    uint8_t *out_data;
} as_sonic_t;

uint8_t* sonicInit(int sampleRate, int numChannels, int maxRate) {
    as_sonic_t *sonicer = (as_sonic_t *)malloc(sizeof(as_sonic_t));
    sonicer->sonic_handle = sonicCreateStream(sampleRate, numChannels);
    sonicer->in_data = (uint8_t *)malloc(sampleRate * numChannels * maxRate);
    sonicer->out_data = (uint8_t *)malloc(sampleRate * numChannels * maxRate);
    sonicSetSpeed(sonicer->sonic_handle, 0.5);
    return (uint8_t *)sonicer;
}

void setSpeed(uint8_t* dec, int speed) {
    as_sonic_t *sonicer = (as_sonic_t *)dec;
    sonicSetSpeed(sonicer->sonic_handle, speed);
}

int pcmHandleRateBySonic(uint8_t* dec, uint8_t* input, int input_len, uint8_t* output, int samples) {
    printf(" pcmHandleRateBySonic input_len\n");
    as_sonic_t *sonicer = (as_sonic_t *)dec;
    printf(" before memcpy\n");
    memcpy(sonicer->in_data, input, input_len);
    int write_res = sonicWriteShortToStream(sonicer->sonic_handle, (short *)sonicer->in_data, samples / 2);
    printf(" after memcpy write_res:%d\n", write_res);
    if (write_res != 1) {
        printf("%s pcmHandleRateBySonic write_res:%d\n", TAG, write_res);
        return write_res;
    }
    int read_res = sonicReadShortFromStream(sonicer->sonic_handle, (short *)sonicer->out_data, samples / 2);
    printf(" read res:%d\n", read_res);
    memcpy(output, sonicer->out_data, read_res * 2);
    return read_res * 2;
}

int destory(uint8_t *dec) {
    as_sonic_t *sonicer = (as_sonic_t *)dec;
    if (sonicer) {
        if (sonicer->sonic_handle) {
            sonicDestroyStream(sonicer->sonic_handle);
        }
        if (sonicer->in_data) {
            free(sonicer->in_data);
        }
        if (sonicer->out_data) {
            free(sonicer->out_data);
        }
        free(sonicer);
    }
    return 0;
}

int test() {
    int sampleRate = 48000;
    int numChannels = 1;
    int maxRate = 10;
    FILE *fin = NULL;
    FILE *fout = NULL;
    fin = fopen("/Users/xuxiuquan/mygithub/like-player-lib/lib-sonic/sonic/webassembly-test/test-pcm-web.pcm", "rb");
    fout = fopen("/Users/xuxiuquan/mygithub/like-player-lib/lib-sonic/sonic/webassembly-test/after-sonic-pcm.pcm", "wb");
    // 初始化sonic
    uint8_t* content = (uint8_t*)malloc(1024 * 1024);
    uint8_t* res = (uint8_t *)malloc(1024 * 1024);
    fread(content, 48000, 10, fin);
    uint8_t* son = sonicInit(sampleRate, numChannels, maxRate);
    int read_res = pcmHandleRateBySonic(son, content, 48000 * 10, res, 48000 * 10);
    printf("test res:%d\n", read_res);
    fwrite(res, read_res, 1, fout);
    return 0;
}

// int main() {
//     printf("start test\n");
//     test();
//     printf("after test\n");
//     return 0;
//     FILE *fin = NULL;
//     FILE *fout = NULL;
//     fin = fopen("/Users/xuxiuquan/mygithub/like-player-lib/lib-sonic/sonic/webassembly-test/test-pcm-web.pcm", "rb");
//     fout = fopen("/Users/xuxiuquan/mygithub/like-player-lib/lib-sonic/sonic/webassembly-test/after-sonic-pcm.pcm", "wb");
//     // 初始化sonic
//     unsigned char *content = (unsigned char *)malloc(1024 * 1024);
//     unsigned char *res = (unsigned char *)malloc(1024 * 1024);
//     int sampleRate = 48000;
//     int numChannels = 1;
//     sonicStream handle = sonicCreateStream(sampleRate, numChannels);
//     sonicSetSpeed(handle, 0.5);
//     // sonicSetPitch(handle, 1.0);
//     // sonicSetRate(handle, 0.5);
//     // sonicSetVolume(handle, 1.0);
//     // sonicSetChordPitch(handle, 0);
//     // sonicSetQuality(handle, 0);
//     // 使用sonic
//     int count = 0;
//     while (count == 0)
//     {
//         printf("while start\n");
//         fread(content, 48000, 10, fin);
//         // fseek(fin, 48000, 1);
//         printf("fread res 0:%d, 1:%d, 2:%d\n", content[0], content[1], content[2]);
//         unsigned short *a = (unsigned short *)content;
//         int write_res = sonicWriteShortToStream(handle, (short *)content, 48000 * 10 / 2);
//         printf("write_res:%d\n", write_res);
//         int read_res = sonicReadShortFromStream(handle, (short *)res, 48000 * 10 / 2);
//         printf("read_res:%d\n", read_res);
//         fwrite(res, read_res * 2, 1, fout);
//         count += 1;
//     }
    
//     return 0;
// }
