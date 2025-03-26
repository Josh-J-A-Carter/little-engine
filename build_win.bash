# FILES=$(find | grep ".cpp$")
# g++ ${FILES} -o program -I ./glad/include  -lmingw32 -lSDL2main -lSDL2
# ./preprocessor.bash
g++ src/stb_image.cpp src/texture.cpp src/utilities.cpp src/pipeline.cpp src/serialise.cpp \
        src/scene_node.cpp src/scene.cpp src/directional_shadow_map.cpp \
        src/parse_types.cpp src/application.cpp src/transform.cpp src/mesh.cpp src/camera.cpp src/main.cpp glad/src/glad.c \
        -o build/program \
        -I ./assimp/include/ -I ./glad/include -I ./glm -I ./include \
        -lmingw32 -lSDL2main -lSDL2 -lassimp \
        -Wno-pointer-arith -fpermissive
        # -D NDEBUG