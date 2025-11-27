#!/bin/bash

gcc -std=c99 -Werror -Wpedantic -Wfloat-equal -g3 -finput-charset=UTF-8 -Wvla *.c -o app.exe

./app.exe
