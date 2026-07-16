/*
lib_main.c - реализация функций библиотеки. Вызов функций инициализации, парсинга и генерации

Бабурин Дмитрий Сергеевич
МК-101
*/

#include "lib_main.h"
#include <stdlib.h>

int main(int argc, char **argv) {
    PasswordConfig config;
    
    // Инициализируем структуру конфигурации дефолтными значениями
    init_config(&config);
    
    // Парсим аргументы командной строки
    if (parse_arguments(argc, argv, &config) != 0) {
        // Если парсер вернул ошибку, завершаем программу с кодом неуспеха
        return EXIT_FAILURE;
    }
    
    // Запускаем генерацию паролей
    generate_passwords(&config);
    
    return EXIT_SUCCESS;
}
