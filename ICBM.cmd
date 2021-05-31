goto(){
conan install conanfile.txt
cmake ..
make
./bin/ICBM
uname -o
}

goto $@
exit

:(){
rem Windows script here
echo %OS%
exit
