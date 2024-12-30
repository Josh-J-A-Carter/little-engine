# FILES=$(find | grep ".cpp$")
# g++ ${FILES} -o program -I ./glad/include  -lmingw32 -lSDL2main -lSDL2
g++ main.cpp glad/src/glad.c -o program -I ./glad/include -I ./glm -lmingw32 -lSDL2main -lSDL2