
# ./preprocessor.bash
emcc src/stb_image.cpp src/texture.cpp src/utilities.cpp src/pipeline.cpp src/serialise.cpp \
        src/scene_node.cpp src/scene.cpp src/fbo.cpp src/directional_light.cpp \
        src/parse_types.cpp src/application.cpp src/transform.cpp src/mesh.cpp src/camera.cpp src/main.cpp glad/src/glad.c \
        -o build/little-engine.js \
        -I ./assimp/include/ -I ./glad/include -I ./glm -I ./include -I ./include/ \
        -L ./lib/wasm/ -lzlibstatic -lassimp \
        --preload-file assets --preload-file shaders --preload-file scenes \
        -DNDEBUG \
        -Wno-pointer-arith -fpermissive \
        -sUSE_SDL=2 -sFULL_ES3=1 -sWASM=1 -sINITIAL_MEMORY=106168320 \
        -sEXPORTED_FUNCTIONS=_main,_remove_focus \
        -sEXPORT_ES6 \
        -sEXPORTED_RUNTIME_METHODS=ccall,cwrap \
        -sMODULARIZE=1 -sEXPORT_NAME=createLittleEngine
        # -D NDEBUG \
