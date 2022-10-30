#!/bin/bash
cd build/ || exit
cmake ../CMakeLists.txt -DCMAKE_EXPORT_COMPILE_COMMANDS=1 && make
