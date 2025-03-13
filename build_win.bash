# FILES=$(find | grep ".cpp$")
# g++ ${FILES} -o program -I ./glad/include  -lmingw32 -lSDL2main -lSDL2
g++ stb_image.cpp texture.cpp utilities.cpp pipeline.cpp serialise.cpp \
        application.cpp transform.cpp mesh.cpp camera.cpp main.cpp glad/src/glad.c \
        -o build/program \
        -I ./assimp/include/ -I ./glad/include -I ./glm -I ./include \
        -lmingw32 -lSDL2main -lSDL2 -lassimp \
        -Wno-pointer-arith
        # -D NDEBUG