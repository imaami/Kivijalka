#!/bin/bash

if [[ -f /opt/qt54/bin/qt54-env.sh ]]; then
  source /opt/qt54/bin/qt54-env.sh
fi

if [[ -f Makefile ]]; then
  mv Makefile Makefile.old
fi

if qmake; then
  sed -ri 's|(CFLAGS.*)-pipe -O2|\1-march=native -pipe -O3 -std=gnu11|' Makefile
  sed -ri 's|(CXXFLAGS.*)-pipe -O2|\1-march=native -pipe -O3|' Makefile
  make
fi
