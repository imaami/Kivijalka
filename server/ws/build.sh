#!/bin/bash

if [[ -f /opt/qt54/bin/qt54-env.sh ]]; then
  source /opt/qt54/bin/qt54-env.sh
fi

if [[ ! -f Makefile ]]; then
  if qmake; then
    sed -i 's|-pipe -O2|-march=native -pipe -O3|' Makefile
    make
  fi
fi
