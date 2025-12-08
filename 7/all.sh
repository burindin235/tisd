#!/bin/bash

rm ./app.exe
gcc -std=gnu99 -O0 -g -w *.c -o app.exe

./app.exe