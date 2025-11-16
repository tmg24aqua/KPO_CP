#pragma once

#define IN_MAX_LEN_TEXT 1024*1024    // макс размер исходного кода 1MB
#define MAX_LEN_BUFFER  2048
#define IN_CODE_ENDL    '\n'
#define IN_CODE_SPACE   ' '
#define IN_CODE_NULL    '\0'
#define IN_CODE_QUOTE   '\"'
#define IN_CODE_APOST   '\''

// Таблица классификации символов для ENV-2025
#define IN_CODE_TABLE {\
    /* 0-15 */   IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,   IN::I, IN::W, IN::N, IN::I,  IN::I, IN::W, IN::I, IN::I,\
    /* 16-31 */  IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,   IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,\
    /* 32-47 */  IN::W, IN::S, IN::Q, IN::O,  IN::O, IN::O, IN::O, IN::A,   IN::S, IN::S, IN::O, IN::O,  IN::S, IN::O, IN::S, IN::O,\
    /* 48-63 */  IN::D, IN::D, IN::D, IN::D,  IN::D, IN::D, IN::D, IN::D,   IN::D, IN::D, IN::S, IN::S,  IN::C, IN::S, IN::C, IN::S,\
    /* 64-79 */  IN::S, IN::H, IN::H, IN::H,  IN::H, IN::H, IN::H, IN::L,   IN::L, IN::L, IN::L, IN::L,  IN::L, IN::L, IN::L, IN::L,\
    /* 80-95 */  IN::L, IN::L, IN::L, IN::L,  IN::L, IN::L, IN::L, IN::L,   IN::L, IN::L, IN::L, IN::S,  IN::K, IN::S, IN::K, IN::L,\
    /* 96-111 */ IN::I, IN::H, IN::H, IN::H,  IN::H, IN::H, IN::H, IN::L,   IN::L, IN::L, IN::L, IN::L,  IN::L, IN::L, IN::L, IN::L,\
    /* 112-127*/ IN::L, IN::L, IN::L, IN::L,  IN::L, IN::L, IN::L, IN::L,   IN::L, IN::L, IN::L, IN::S,  IN::O, IN::S, IN::O, IN::I,\
    \
    /* 128-255 - все запрещенные для ENV-2025 */ \
                IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,   IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,\
                IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,   IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,\
                IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,   IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,\
                IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,   IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,\
                IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,   IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,\
                IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,   IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,\
                IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,   IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,\
                IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,   IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I \
            }

namespace In
{
    struct InWord
    {
        char word[MAX_LEN_BUFFER];    // слово
        int line;                     // строка в исходном тексте
        int size;                     // количество слов
    };

    struct IN // исходные данные
    {
        /*
        Коды категорий символов для ENV-2025:
        L - Буквы (A-Z, a-z) - для идентификаторов и ключевых слов
        D - Цифры (0-9) - для целочисленных литералов
        H - Шестнадцатеричные цифры (A-F, a-f)
        B - Двоичные цифры (0, 1)
        O - Операции (+ - * / % =)
        C - Сравнения (< > !) - для ==, !=, <=, >=
        S - Разделители (; , : ( ) { } [ ])
        Q - Строковый литерал (")
        A - Символьный литерал (')
        U - Подчеркивание (_) - для идентификаторов
        W - Пробельные символы (пробел, табуляция)
        N - Перевод строки
        I - Запрещенные символы
        K - Символы для логических литералов (T, F, t, f)
        */
        enum {
            L = 1024,  // Letter
            D = 2048,  // Digit  
            H = 4096,  // Hex digit
            B = 8192,  // Binary digit
            O = 16384, // Operator
            C = 32768, // Comparison
            S = 65536, // Separator
            Q = 131072, // Quote (string)
            A = 262144, // Apostrophe (char)
            U = 524288, // Underscore
            W = 1048576, // Whitespace
            N = 2097152, // Newline
            I = 4194304, // Invalid
            K = 8388608  // Boolean literal
        };

        int size = 0;           // размер исходного кода 
        int lines = 1;          // количество строк
        int ignor = 0;          // количество проигнорированных символов
        unsigned char* text;    // исходный текст
        int code[256] = IN_CODE_TABLE; // таблица классификации символов
        InWord* words;          // таблица слов
    };

    IN getin(wchar_t infile[], std::ostream* stream); // Ввести и проверить входной поток 
    void addWord(InWord* words, char* word, int line);
    InWord* getWordsTable(std::ostream* stream, unsigned char* text, int* code, int textSize);
    void printTable(InWord* table);
}