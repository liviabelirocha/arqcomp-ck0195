#!/bin/bash

g++ -Wall -Wextra -pedantic -std=c++17 emulador.cpp -o main.out && ./main.out

RET=$?

echo Retorno do g++: $RET

exit $RET