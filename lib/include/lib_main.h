/*
lib_main.h - Заголовочный файл библиотеки. Определение структуры конфигурации и прототипов функций.

Бабурин Дмитрий Сергеевич
МК-101
*/

#ifndef LIB_MAIN_H
#define LIB_MAIN_H

#define MAX_DELIMITERS 64
#define MAX_ALPHABET_SIZE 512


// Структура для хранения всех настроек генератора паролей
typedef struct {
    int min_len;        // Минимальная длина пароля (опция -minl)
    int max_len;        // Максимальная длина пароля (опция -maxl)
    int fixed_len;      // Фиксированная длина пароля (опция -n)
    int count;          // Количество паролей (опция -c)

    char alphabet[MAX_ALPHABET_SIZE];   // Итоговый алфавит для генерации
    char delimiters[MAX_DELIMITERS];    // Строка с текущими активными разделителями

    // Флаги для проверки дублирования опций (чтобы выдать ошибку, если опция передана дважды)
    int has_min_len;
    int has_max_len;
    int has_fixed_len;
    int has_count;
    int has_alphabet;  // Флаг для опции -a
    int has_classes;  // Флаг для опции -C
    int has_m1;       // Флаг для опции -m1
    int has_m2;      // Флаг для опции -m2
} PasswordConfig;

// Инициализация конфигурации дефолтными значениям
void init_arguments(PasswordConfig *config);

// Парсинг аргументов командной строки
// Возвращает 0 в случае успеха, и ненулевое значение в случае ошибки
int parse_arguments(int argc, char **argv, PasswordConfig *config);

// Генерация паролей на основе заполненной конфигурации
void generate_passwords(const PasswordConfig *config);

#endif // LIB_MAIN_H


