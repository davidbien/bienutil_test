# bienutil_test
Unit tests for various objects in the bienutil submodule include directory

synopsis:  
git clone --recursive git@github.com:davidbien/bienutil_test.git

Requires c++20 and \<compare\> header existence.

Linux:  
sudo apt install uuid-dev  
sudo apt install libicu-dev  
Requires clang11 and libc++11  

Windows:  
Compiles under Visual Studio 2019 - but there is a bug with cmake projects in certain circumstances (unclear when and how) and on one of my machines it requires Visual Studio 2022 Preview to compile at all.


