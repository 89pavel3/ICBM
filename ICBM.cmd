goto(){
conan install conanfile.txt
cmake .
make
./bin/ICBM
uname -o
}

goto $@
exit

:(){
conan install conanfile.txt

rem Windows script here
echo %OS%
exit
