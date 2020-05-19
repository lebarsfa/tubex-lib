#!/bin/bash

cd $HOME
echo 'Installing IBEX...';
if [ ! -d "ibex-lib" ]; then
  git clone -b develop https://github.com/ibex-team/ibex-lib.git ;
  cd ibex-lib ;
  git checkout 47fc75a5e14eb2f353ae7e0b37ac0f9dad7801c0 ;
  mkdir build && cd build ;
  cmake -DCMAKE_CXX_FLAGS="-fPIC -msse3" -DCMAKE_C_FLAGS="-fPIC -msse3" -DCMAKE_INSTALL_PREFIX=$HOME/ibex-lib/build_install -DCMAKE_BUILD_TYPE=Debug .. ;
  make ;
  make install ;
else
  echo 'Using cached directory.' ;
fi
cd $HOME