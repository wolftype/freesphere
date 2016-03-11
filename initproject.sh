#!/bin/sh

wget -N https://raw.githubusercontent.com/AlloSphere-Research-Group/AlloProject/master/CMakeLists.txt
wget -N https://raw.githubusercontent.com/AlloSphere-Research-Group/AlloProject/master/run.sh
wget -N https://raw.githubusercontent.com/AlloSphere-Research-Group/AlloProject/master/distclean
wget -N https://raw.githubusercontent.com/AlloSphere-Research-Group/AlloProject/master/makexcode.sh

chmod 750 run.sh
chmod 750 distclean

mkdir -p cmake_modules
cd cmake_modules
wget -N https://raw.githubusercontent.com/AlloSphere-Research-Group/AlloProject/master/cmake_modules/make_dep.cmake
wget -N https://raw.githubusercontent.com/AlloSphere-Research-Group/AlloProject/master/cmake_modules/CMakeRunTargets.cmake

cd ..
git submodule add --depth 50 https://github.com/AlloSphere-Research-Group/AlloSystem.git AlloSystem
git submodule add --depth 50 https://github.com/rbtsx/cuttlebone.git cuttlebone
git submodule add --depth 50 https://github.com/AlloSphere-Research-Group/Gamma.git Gamma
git submodule add --depth 50 https://github.com/AlloSphere-Research-Group/GLV.git GLV
git init
git submodule init
git submodule update
git fat init
git fat pull
