
# ./preprocessor.bash
emcc src/stb_image.cpp src/texture.cpp src/utilities.cpp src/pipeline.cpp src/serialise.cpp \
        src/scene_node.cpp src/scene.cpp src/fbo.cpp src/directional_light.cpp \
        src/parse_types.cpp src/application.cpp src/transform.cpp src/mesh.cpp src/camera.cpp src/main.cpp glad/src/glad.c \
        -o build/program.js \
        -I ./assimp/include/ -I ./glad/include -I ./glm -I ./include -I ./include/ \
        -L ./lib/wasm/ -lzlibstatic -lassimp \
        -s USE_SDL=2 -s FULL_ES3=1 -s WASM=1 -sINITIAL_MEMORY=106168320 \
        --preload-file assets --preload-file shaders --preload-file scenes \
        -Wno-pointer-arith -fpermissive
        # -D NDEBUG \
