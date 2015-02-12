# build

Русский текст смотри ниже.

The tool for automatic compilation C++ source code. Presented in open soure, licence TDB.

To build the tool in linux you need to run compile.sh scrtipt.

To build it manualy:
g++ *.cpp -o build -g -Wall -std=c++1y

How you use it:
* Copy binary file "build" and config file "build.cfg" into the folder with your project
* run ./build
* As output you will get binary file (for instance "opengl")
* Run built "opengl". Enjoy!

Утилита для автоматической компиляции исходных кодов на с++, предоставляется в открытом виде.

Для сборки утилиты в операционной системе Linux достаточно запустить скрипт compile.sh

Команда для сборки вручную:
g++ *.cpp -o build -g -Wall -std=c++1y

Инструкция по использованию:
* Скорировать запускаемый файл "build" и кофигурационный файл "build.cfg" в папку с проектом.
* Запустить ./build
* На выходе получаем бинарный файл "opengl".
* Запускаем собранный "opengl". Радуемся жизни.
