/*
Разработка генератора случайных паролей
Описание файла: Реализация парсера аргументов и алгоритма генерации паролей.

Бабурин Дмитрий Сергеевич
МК-101
*/

#include "lib_main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Вспомогательная функция: проверяем является ли символ разделителем
static int is_delimiter(char c, const char *delims) {
    return strchr(delims, c) != NULL;
}

// Вспомогательная функция: проверяем является ли строка числом
static int is_number(const char *str) {
    if (str == NULL || *str == '\0') return 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') return 0;
    }
    return 1;
}

// Вспомогательная функция: валидация строки набора символов для -C (только a, A, D, S без повторов)
static int validate_classes(const char *val) {
    if (val == NULL || *val == '\0') return 0;
    int counts[256] = {0};
    for (int i = 0; val[i] != '\0'; i++) {
        char c = val[i];
        if (c != 'a' && c != 'A' && c != 'D' && c != 'S') {
            return 0; // Недопустимый символ
        }
        counts[(unsigned char)c]++;
        if (counts[(unsigned char)c] > 1) {
            return 0; // Обнаружен дубликат
        }
    }
    return 1;
}

// Вспомогательная функция для добавления разделителя (опция -d)
static void add_delimiter(char *delims, char sym) {
    if (strchr(delims, sym) == NULL) {
        size_t len = strlen(delims);
        if (len < MAX_DELIMITERS - 1) {
            delims[len] = sym;
            delims[len + 1] = '\0';
        }
    }
}

// Вспомогательная функция для замены разделителей (опция -D)
static void replace_delimiters(char *delims, char sym) {
    delims[0] = sym;
    delims[1] = '\0';
}

// Вспомогательная функция извлечения значения для опции.
// Обрабатывает три стиля: слитно (-minl10), через разделитель (-minl:10), через пробел (-minl 10).
static const char* extract_value(const char *arg, const char *opt, const char *delims, int *used_next, const char *next_arg) {
    size_t opt_len = strlen(opt);
    if (strncmp(arg, opt, opt_len) != 0) {
        return NULL; // Аргумент не начинается с этой опции
    }
    
    const char *rest = arg + opt_len;
    *used_next = 0;
    
    if (strlen(rest) == 0) {
        if (next_arg == NULL) {
            return ""; // Значение отсутствует
        }
        *used_next = 1;
        return next_arg;
    }
    
    if (is_delimiter(rest[0], delims)) {
        return rest + 1;
    }
    
    return rest;
}

// Вспомогательная функция для сборки пула доступных символов (Рефакторинг)
static void build_char_pool(const PasswordConfig *config, char *pool) {
    if (config->has_alphabet) {
        // Если задан конкретный пользовательский алфавит
        strcpy(pool, config->alphabet);
    } else if (config->has_classes) {
        // Если заданы классы символов a, A, D, S
        for (int i = 0; config->alphabet[i] != '\0'; i++) {
            char c = config->alphabet[i];
            if (c == 'a') {
                strcat(pool, "abcdefghijklmnopqrstuvwxyz");
            } else if (c == 'A') {
                strcat(pool, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
            } else if (c == 'D') {
                strcat(pool, "0123456789");
            } else if (c == 'S') {
                strcat(pool, "!@#$%^&*()-_=+[]{}|;:,.<>/?");
            }
        }
    } else {
        // Дефолтный алфавит символов (строчные буквы + цифры), если ничего не передали
        strcpy(pool, "abcdefghijklmnopqrstuvwxyz0123456789");
    }
}

// Инициализация структуры дефолтными значениями
void init_config(PasswordConfig *config) {
    config->min_len = -1;
    config->max_len = -1;
    config->fixed_len = -1;
    config->count = 1; // По умолчанию генерируем 1 пароль
    config->alphabet[0] = '\0';
    
    // Начальные разделители: '=' и ':'
    strcpy(config->delimiters, "=:");
    
    config->has_min_len = 0;
    config->has_max_len = 0;
    config->has_fixed_len = 0;
    config->has_count = 0;
    config->has_alphabet = 0;
    config->has_classes = 0;
    config->has_m1 = 0;
    config->has_m2 = 0;
}

// Парсинг аргументов командной строки
int parse_arguments(int argc, char **argv, PasswordConfig *config) {
    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];
        const char *next_arg = (i + 1 < argc) ? argv[i + 1] : NULL;
        int used_next = 0;
        
        // 1. Обработка -minl
        const char *val = extract_value(arg, "-minl", config->delimiters, &used_next, next_arg);
        if (val != NULL) {
            if (config->has_min_len) {
                fprintf(stderr, "Error: Duplicate option -minl\n");
                return 1;
            }
            if (strlen(val) == 0) {
                fprintf(stderr, "Error: Missing value for -minl\n");
                return 1;
            }
            if (!is_number(val)) {
                fprintf(stderr, "Error: Option -minl expects a numeric value\n");
                return 1;
            }
            config->min_len = atoi(val);
            config->has_min_len = 1;
            if (used_next) i++;
            continue;
        }
        
        // 2. Обработка -maxl
        val = extract_value(arg, "-maxl", config->delimiters, &used_next, next_arg);
        if (val != NULL) {
            if (config->has_max_len) {
                fprintf(stderr, "Error: Duplicate option -maxl\n");
                return 1;
            }
            if (strlen(val) == 0) {
                fprintf(stderr, "Error: Missing value for -maxl\n");
                return 1;
            }
            if (!is_number(val)) {
                fprintf(stderr, "Error: Option -maxl expects a numeric value\n");
                return 1;
            }
            config->max_len = atoi(val);
            config->has_max_len = 1;
            if (used_next) i++;
            continue;
        }
        
        // 3. Обработка -n
        val = extract_value(arg, "-n", config->delimiters, &used_next, next_arg);
        if (val != NULL) {
            if (config->has_fixed_len) {
                fprintf(stderr, "Error: Duplicate option -n\n");
                return 1;
            }
            if (strlen(val) == 0) {
                fprintf(stderr, "Error: Missing value for -n\n");
                return 1;
            }
            if (!is_number(val)) {
                fprintf(stderr, "Error: Option -n expects a numeric value\n");
                return 1;
            }
            config->fixed_len = atoi(val);
            config->has_fixed_len = 1;
            if (used_next) i++;
            continue;
        }
        
        // 4. Обработка -c
        val = extract_value(arg, "-c", config->delimiters, &used_next, next_arg);
        if (val != NULL) {
            if (config->has_count) {
                fprintf(stderr, "Error: Duplicate option -c\n");
                return 1;
            }
            if (strlen(val) == 0) {
                fprintf(stderr, "Error: Missing value for -c\n");
                return 1;
            }
            if (!is_number(val)) {
                fprintf(stderr, "Error: Option -c expects a numeric value\n");
                return 1;
            }
            config->count = atoi(val);
            config->has_count = 1;
            if (used_next) i++;
            continue;
        }
        
        // 5. Обработка -a
        val = extract_value(arg, "-a", config->delimiters, &used_next, next_arg);
        if (val != NULL) {
            if (config->has_alphabet) {
                fprintf(stderr, "Error: Duplicate option -a\n");
                return 1;
            }
            if (strlen(val) == 0) {
                fprintf(stderr, "Error: Missing value for -a\n");
                return 1;
            }
            strncpy(config->alphabet, val, MAX_ALPHABET_SIZE - 1);
            config->alphabet[MAX_ALPHABET_SIZE - 1] = '\0';
            config->has_alphabet = 1;
            if (used_next) i++;
            continue;
        }
        
        // 6. Обработка -C
        val = extract_value(arg, "-C", config->delimiters, &used_next, next_arg);
        if (val != NULL) {
            if (config->has_classes) {
                fprintf(stderr, "Error: Duplicate option -C\n");
                return 1;
            }
            if (strlen(val) == 0) {
                fprintf(stderr, "Error: Missing value for -C\n");
                return 1;
            }
            if (!validate_classes(val)) {
                fprintf(stderr, "Error: Option -C expects unique characters from {a, A, D, S}\n");
                return 1;
            }
            strncpy(config->alphabet, val, MAX_ALPHABET_SIZE - 1);
            config->alphabet[MAX_ALPHABET_SIZE - 1] = '\0';
            config->has_classes = 1;
            if (used_next) i++;
            continue;
        }
        
        // 7. Обработка -d
        val = extract_value(arg, "-d", config->delimiters, &used_next, next_arg);
        if (val != NULL) {
            if (strlen(val) == 0) {
                fprintf(stderr, "Error: Missing value for -d\n");
                return 1;
            }
            add_delimiter(config->delimiters, val[0]);
            if (used_next) i++;
            continue;
        }
        
        // 8. Обработка -D
        val = extract_value(arg, "-D", config->delimiters, &used_next, next_arg);
        if (val != NULL) {
            if (strlen(val) == 0) {
                fprintf(stderr, "Error: Missing value for -D\n");
                return 1;
            }
            replace_delimiters(config->delimiters, val[0]);
            if (used_next) i++;
            continue;
        }
        
        // 9. Обработка -m1
        val = extract_value(arg, "-m1", config->delimiters, &used_next, next_arg);
        if (val != NULL) {
            config->has_m1 = 1;
            if (used_next) {
                fprintf(stderr, "Error: Option -m1 does not take a separate value\n");
                return 1;
            }
            continue;
        }
        
        // 10. Обработка -m2
        val = extract_value(arg, "-m2", config->delimiters, &used_next, next_arg);
        if (val != NULL) {
            config->has_m2 = 1;
            if (used_next) {
                fprintf(stderr, "Error: Option -m2 does not take a separate value\n");
                return 1;
            }
            continue;
        }
    }
    
    // Проверки совместимости
    if (config->has_alphabet && config->has_classes) {
        fprintf(stderr, "Error: Options -a and -C are mutually exclusive\n");
        return 1;
    }
    
    if (config->has_m1 && !config->has_m2) {
        fprintf(stderr, "Error: Option -m1 requires option -m2\n");
        return 1;
    }
    
    if ((config->has_m1 || config->has_m2) && config->has_fixed_len) {
        fprintf(stderr, "Error: Options -m1/-m2 and -n are mutually exclusive\n");
        return 1;
    }
    
    if ((config->has_min_len || config->has_max_len) && config->has_fixed_len) {
        fprintf(stderr, "Error: Options -minl/-maxl and -n are mutually exclusive\n");
        return 1;
    }
    
    if (config->has_min_len != config->has_max_len) {
        fprintf(stderr, "Error: Both -minl and -maxl must be specified together\n");
        return 1;
    }
    
    if (config->has_min_len && config->has_max_len) {
        if (config->min_len <= 0 || config->max_len <= 0) {
            fprintf(stderr, "Error: Password lengths must be positive numbers\n");
            return 1;
        }
        if (config->min_len > config->max_len) {
            fprintf(stderr, "Error: -minl cannot be greater than -maxl\n");
            return 1;
        }
    }
    
    if (config->has_fixed_len && config->fixed_len <= 0) {
        fprintf(stderr, "Error: Fixed password length (-n) must be a positive number\n");
        return 1;
    }
    
    if (config->count <= 0) {
        fprintf(stderr, "Error: Number of passwords (-c) must be positive\n");
        return 1;
    }
    
    return 0;
}

// Генерация случайных паролей на основе собранной конфигурации
void generate_passwords(const PasswordConfig *config) {
    char pool[1024] = {0};
    
    // Вызываем вспомогательную функцию для сборки пула (Рефакторинг)
    build_char_pool(config, pool);
    
    int pool_size = (int)strlen(pool);
    if (pool_size == 0) {
        return;
    }
    
    // Инициализация генератора случайных чисел один раз за запуск программы
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = 1;
    }
    
    // Задаем базовую длину, если параметры длины не передавались вовсе
    int base_len = 10;
    if (config->has_fixed_len) {
        base_len = config->fixed_len;
    }
    
    // Генерируем указанное количество паролей
    for (int c_idx = 0; c_idx < config->count; c_idx++) {
        int current_len = base_len;
        
        // Если задан интервал minl - maxl, выбираем случайную длину в этом диапазоне
        if (config->has_min_len && config->has_max_len) {
            current_len = config->min_len + rand() % (config->max_len - config->min_len + 1);
        }
        
        for (int char_idx = 0; char_idx < current_len; char_idx++) {
            putchar(pool[rand() % pool_size]);
        }
        putchar('\n');
    }
}