goto(){
    mkdir build
    cd build
    conan install ..
    cmake ..
    make
    ./bin/ICBM
}

goto $@
exit

:(){

rem Windows script here
echo %OS%
exit
