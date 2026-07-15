В данном шаблоне проекта настроена сборка программы и библиотеки. Библиотека может собираться статической или динамической (опция BUILD_SHARED_LIBS).
Для программы и библиотеки настроена сборка тестов (опция BUILD_TESTING).


Подготовка проекта для сборки в студии (выполняем команды в консоле в директории проекта):
> mkdir build
> cd build
Конфигурация со статической библиотекой
> cmake -S .. -G "Visual Studio 17 2022" -A Win32
Конфигурация с динамической библиотекой
> cmake -S .. -G "Visual Studio 17 2022" -A Win32 -DBUILD_SHARED_LIBS=ON

В каталоге build создаются нужные для сборки файлы. Можно открыть проект (.sln-файл) в Visual Studio и в ней собирать. Можно открыть в Visual Studio файл CMakeLists.txt. Можно собирать проект через cmake в консоле.

Сборка проекта в консоле:
> cmake --build . --config Release
> cmake --build . --config Debug


После сборки можно запустить тесты:
> cmake --build . --target test_lib
> cmake --build . --target test_app
> cmake --build . --target test_all
> ctest -C Release
> ctest -C Debug
