###
 # @Author: xiuquanxu
 # @Company: kaochong
 # @Date: 2020-04-07 12:10:05
 # @LastEditors: xiuquanxu
 # @LastEditTime: 2020-12-17 16:12:49
 ###
echo "----------------------------"
echo "start building"
source /Users/xuxiuquan/github/webassembly/emsdk/emsdk_env.sh
rm -rf ./webassembly
mkdir ./webassembly
EMCC_DEBUG=1 \
EMMAKEN_CFLAGS="-I/Users/xuxiuquan/mygithub/sonic-pcm-player/opus-pcm-sonic/wasm/include/ -L/Users/xuxiuquan/mygithub/sonic-pcm-player/opus-pcm-sonic/wasm/lib/ -lopus -lsonic -DNOPUS_HAVE_RTCD" \
emcc    ./opus2pcmsonic.c  \
        ./resample.c \
        -g2 \
        -s EMULATE_FUNCTION_POINTER_CASTS=1 \
        -s ASSERTIONS=2 \
        -s WASM=1 \
        -s ALLOW_MEMORY_GROWTH=1 \
        -s "MODULARIZE=1" \
        -s "EXPORT_NAME='WebOPSnoic'" \
        -s "BINARYEN_METHOD='native-wasm'" \
        -s "EXPORTED_FUNCTIONS=['_init', '_MakePcmStream', '_setSpeed', '_PCMEncoderData', '_OpusInitEncoder']" \
        -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
        -o ./webassembly/opus2pcmsonic.js \
        -Wall \
# Module['onRuntimeInitialized'] = onRuntimeInitialized;
