#!/bin/bash

blue='\033[0;34m'
NC='\033[0m' # No Color

prereq() {
  PREREQS="build-essential cmake $1"
  
  if dpkg -s $PREREQS > /dev/null 2>&1; then
    echo -e "Prerequisites already installed: $PREREQS"
  else
    echo -e "[${blue}Installing prerequisites${NC}]"
    sudo apt-get update
    sudo apt-get -y install $PREREQS
  fi
}

nxp() {
  if [[ -d "nxp" ]]; then
    echo -e "NXP Reader Library found, skipping install"
  else
    unzip -q -o -d nxp nxp.zip
    rm nxp.zip
  fi

  pushd nxp/build > /dev/null
  cmake ..
  make NxpRdLibLinuxPN512
  popd > /dev/null
}

cleanup() {
  rm -rf *.zip *.gz *.deb
}

all() {
  prereq $@
  nxp
  cleanup
}

if [[ $# -eq 0 ]]; then
  all
else
  $@
fi

