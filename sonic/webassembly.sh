###
 # @Author: xiuquanxu
 # @Company: kaochong
 # @Date: 2020-04-07 12:10:05
 # @LastEditors: xiuquanxu
 # @LastEditTime: 2020-07-24 14:29:01
 ###
echo "----------------------------"
echo "start building"
# source /Users/xuxiuquan/github/webassembly/emsdk/emsdk_env.sh
rm -rf ../webassembly
mkdir ../webassembly
EMCC_DEBUG=1 \
EMMAKEN_CFLAGS="-I/usr/local/include -DNOPUS_HAVE_RTCD" \
emcc    ./sonic.c ./wrapper_sonic.c\
        -g2 \
        -s EMULATE_FUNCTION_POINTER_CASTS=1 \
        -s ASSERTIONS=2 \
        -s WASM=1 \
        -s ALLOW_MEMORY_GROWTH=1 \
        -s "MODULARIZE=1" \
        -s "EXPORT_NAME='WebSonic'" \
        -s "BINARYEN_METHOD='native-wasm'" \
        -s "EXPORTED_FUNCTIONS=['_sonicInit', '_pcmHandleRateBySonic', '_setSpeed', '_destory']" \
        -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
        -o ../webassembly/sonic.js \
        -Wall \

# Module['onRuntimeInitialized'] = onRuntimeInitialized;
