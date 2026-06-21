#!/bin/bash

clang src/main.c \
      src/depends.c \
      src/bitmapfiles.c \
      src/filters.c \
      -I include \
      -o imagedtecv2 \
      -lm

echo "Done Compiling"