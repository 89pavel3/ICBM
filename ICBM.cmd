goto(){
    mkdir build
    cd build
    cmake ..
    cmake --build .
    ./bin/ICBM
}

goto $@
exit
