#!/usr/bin/env bash

pyinstaller \
  -w \
  -n dsys \
  --version-file version.txt \
  -i dsys.ico \
  main.py \
  --add-data "resc;resc" \
  -y \
  --clean
