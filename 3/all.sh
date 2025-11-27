#!/bin/bash

rm ./app.exe
gcc -std=gnu99 -O0 *.c -o app.exe

./app.exe