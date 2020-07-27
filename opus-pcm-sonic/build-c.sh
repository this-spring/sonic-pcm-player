###
 # @Author: xiuquanxu
 # @Company: kaochong
 # @Date: 2020-07-23 16:48:45
 # @LastEditors: xiuquanxu
 # @LastEditTime: 2020-07-27 16:17:33
### 
 gcc opus2pcmsonic.c resample.c ./lib/opus/libopus.a ./lib/sonic/libsonic.a -I/Users/xuxiuquan/mygithub/sonic-pcm-player/opus-pcm-sonic/include/ -o test.a 