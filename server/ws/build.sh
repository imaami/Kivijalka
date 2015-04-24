#!/bin/bash

if [[ -f /opt/qt54/bin/qt54-env.sh ]]; then
  source /opt/qt54/bin/qt54-env.sh
fi

if [[ -f Makefile ]]; then
  mv Makefile Makefile.old
fi

if qmake; then
  sed -ri \
      -e 's|(C(XX)?FLAGS.*)-O2|\1|' \
      -e 's|(C(XX)?FLAGS.*)-std=c\+\+0x|\1|' \
      Makefile
  make
fi
