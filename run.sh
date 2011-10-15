#!/bin/sh
./configure >/dev/null 2>./compile_errors.txt
./make >/dev/null 2>>./compile_errors.txt
./LoQuality

