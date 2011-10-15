#!/bin/sh

if [ "$1" ]; then
	while kill -0 $1 > /dev/null; do sleep 1; done
fi

./configure >/dev/null 2>./compile_errors.txt
make >/dev/null 2>>./compile_errors.txt

./LoQuality

