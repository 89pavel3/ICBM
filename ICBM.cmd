goto(){
    mkdir build
    cd build
    conan install ..
    cmake ..
    cmake --build .
    ./bin/ICBM
}

goto $@
exit
