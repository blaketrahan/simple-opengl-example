### simple-opengl-example
Simple SDL2 + OpenGL example that compiles under Linux.

required: glew glm SDL2 b_libs stb

command line: g++ main.cpp -g -std=c++11 -lSDL2 -lGL -lGLU -lGLEW -I../b_libs -I../stb -o sample_program && ./sample_program