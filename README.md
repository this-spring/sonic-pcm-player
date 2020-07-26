<!--
 * @Author: xiuquanxu
 * @Company: kaochong
 * @Date: 2020-07-24 11:45:29
 * @LastEditors: xiuquanxu
 * @LastEditTime: 2020-07-26 13:37:02
--> 
## sonic-pcm-player  

PCM realizes double speed playback through sonic Library. play by AudioContext.  

## Use  

### step1  

download test-pcm-web  

### step2  

open url:  https://this-spring.github.io/sonic-pcm-player/index.html  

### step3  

click 选择文件->获取buffer->start  

### sonic build  

1. git clone https://github.com/waywardgeek/sonic.git  

2. make clean & make & make install  

3. sh webassembly.sh(make sure install emsdk)  

## source  

c:  sonic/wrapper_sonic.c  
wasm:  webassembly/sonic.wasm  
胶水js: webassembly/sonic.js  
player.js: ./player.js  
index.html: ./index.html


## api  

TODO

## vx  
<img src="./vx.jpg"/>

## github  

https://github.com/this-spring/sonic-pcm-player  
