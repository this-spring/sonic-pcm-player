<!--
 * @Author: xiuquanxu
 * @Company: kaochong
 * @Date: 2020-04-06 19:28:02
 * @LastEditors: xiuquanxu
 * @LastEditTime: 2020-04-06 22:30:28
 -->
# complied sonic  

## shell  

```
sh build.sh
```  

## test  

```
./sonic -c -n -q  -s 2.0  ./samples/talking.wav ./samples/sonic.wav
```  

transfer talking.wav(rate 2.0) in samples to output sonic.wav in samples  

sonic.wav play by rate 2.0  

## dist  

lib can use in dist