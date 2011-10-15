#!/bin/sh

if [ "$1" ]; then
	CORES=$1
fi

if [ "$2" ]; then
	echo "This is the updater. Please do not close this window."
	echo "Waiting for LQ to close..."
	while kill -0 $1 > /dev/null 2>/dev/null; do sleep 1; done
fi

echo "Configuring installation..."
./configure >/dev/null 2>./compile_errors.txt

echo "Compiling LoQuality. This may take up to a minute."
echo "Using $CORES cores."
echo "Please do not abort."
make -j $CORES >/dev/null 2>>./compile_errors.txt

./LoQuality

