#!/bin/bash

if [[ -f /opt/qt54/bin/qt54-env.sh ]]; then
  source /opt/qt54/bin/qt54-env.sh
fi

MAKEFILE='Makefile.qt'

if [[ -f "${MAKEFILE}" ]]; then
  mv "${MAKEFILE}" "${MAKEFILE}.old"
fi

if qmake -makefile -o"${MAKEFILE}" wsserver.pro; then
  sed -ri \
      -e 's|(C(XX)?FLAGS.*)-O2|\1|' \
      -e 's|(C(XX)?FLAGS.*)-std=c\+\+0x|\1|' \
      "${MAKEFILE}"
  make -f "${MAKEFILE}"
fi
