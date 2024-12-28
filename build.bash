FILES=$(find | grep ".cpp$")
g++ ${FILES} -o program -lmingw32 -lSDL2main -lSDL2