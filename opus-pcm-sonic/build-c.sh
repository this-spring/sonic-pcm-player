###
 # @Author: xiuquanxu
 # @Company: kaochong
 # @Date: 2020-07-23 16:48:45
 # @LastEditors: xiuquanxu
 # @LastEditTime: 2020-07-27 15:52:05
### 
 gcc opus2pcmsonic.c resample.c ./lib/sonic/libsonic.a  -I/Users/xuxiuquan/mygithub/sonic-pcm-player/opus-pcm-sonic/include/ -o test.a 