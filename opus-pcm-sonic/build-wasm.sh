###
 # @Author: xiuquanxu
 # @Company: kaochong
 # @Date: 2020-04-07 12:10:05
 # @LastEditors: xiuquanxu
 # @LastEditTime: 2020-07-27 19:34:36
 ###
echo "----------------------------"
echo "start building"
source /Users/xuxiuquan/github/webassembly/emsdk/emsdk_env.sh
rm -rf ./webassembly
mkdir ./webassembly
EMCC_DEBUG=1 \
EMMAKEN_CFLAGS="-I/Users/xuxiuquan/mygithub/sonic-pcm-player/opus-pcm-sonic/wasm/include/ -DNOPUS_HAVE_RTCD" \
emcc    ./opus2pcmsonic.c  \
        ./resample.c \
        ./wasm/lib/opus/libopus.a \
        ./wasm/lib/sonic/libsonic.a \
        -g2 \
        -s EMULATE_FUNCTION_POINTER_CASTS=1 \
        -s ASSERTIONS=2 \
        -s WASM=1 \
        -s ALLOW_MEMORY_GROWTH=1 \
        -s "MODULARIZE=1" \
        -s "EXPORT_NAME='WebOPSnoic'" \
        -s "BINARYEN_METHOD='native-wasm'" \
        -s "EXPORTED_FUNCTIONS=['_init', '_MakePcmStream']" \
        -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
        -o ./webassembly/opus2pcmsonic.js \
        -Wall \
# Module['onRuntimeInitialized'] = onRuntimeInitialized;
