emcc stb_image.cpp texture.cpp utilities.cpp pipeline.cpp serialise.cpp \
        application.cpp transform.cpp mesh.cpp camera.cpp main.cpp glad/src/glad.c \
        -o build/program.js \
        -I ./assimp/include/ -I ./glad/include -I ./glm -I ./include -I ./include/ \
        -L ./lib/wasm/ -lzlibstatic -lassimp \
        -s USE_SDL=2 -s FULL_ES3=1 -s WASM=1 -sINITIAL_MEMORY=106168320 \
        --preload-file assets --preload-file shaders
        # -D NDEBUG \
