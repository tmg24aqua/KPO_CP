// In.cpp - Реализация модуля ввода и предобработки
//
// Функции модуля:
// - getin(): чтение исходного файла и классификация символов
// - getWordsTable(): разбиение текста на слова с сохранением номеров строк
// - addWord(): добавление слова в таблицу слов
// - printTable(): вывод таблицы слов для отладки

#include "pch.h"
namespace In
{
    // Чтение исходного файла и классификация символов
    // Проверяет допустимость символов для языка ENV-2025
    IN getin(wchar_t infile[], std::ostream* stream)
    {
        unsigned char* text = new unsigned char[IN_MAX_LEN_TEXT];
        std::ifstream instream(infile);
        if (!instream.is_open())
            throw ERROR_THROW(102);
        IN in;
        int pos = 0;
        bool isStringLiteral = false;
        bool isCharLiteral = false;
        while (true)
        {
            unsigned char uch = instream.get();
            if (instream.eof())
                break;

            // Отслеживание строковых и символьных литералов
            if (in.code[uch] == IN::Q) {              // Кавычка для строк
                isStringLiteral = !isStringLiteral;
            }
            else if (in.code[uch] == IN::A) {         // Апостроф для символов
                isCharLiteral = !isCharLiteral;
            }

            switch (in.code[uch])
            {
            case IN::N:                               // Новая строка
                text[in.size++] = uch;
                in.lines++;
                pos = -1;
                break;
            case IN::L: // Буква
            case IN::D: // Цифра
            case IN::H: // Шестнадцатеричная цифра
            case IN::B: // Двоичная цифра
            case IN::W: // Пробел или табуляция
            case IN::S: // Спецсимвол
            case IN::O: // Оператор
            case IN::C: // Комментарий
            case IN::U: // Неизвестный символ
            case IN::K: // Ключевое слово
            case IN::Q: // Кавычка для строкового литерала
            case IN::A: // Апостроф для символьного литерала
                text[in.size++] = uch;
                break;
            case IN::I: // Недопустимый символ
                if (isStringLiteral || isCharLiteral) {
                    // В строковом или символьном литерале - разрешен
                    text[in.size++] = uch;
                }
                else {
                    Log::WriteError(stream, Error::geterrorin(200, in.lines, pos));
                    in.ignor++;
                }
                break;
            default:
                text[in.size++] = uch;
            }
            pos++;
        }
        text[in.size] = IN_CODE_NULL;
        in.text = text;
        instream.close(); // Закрыть файл
        return in;
    }

    void addWord(InWord* words, char* word, int line) // Добавление слова в таблицу
    {
        if (*word == IN_CODE_NULL)
            return;

        // Проверка длины идентификатора (ошибка 324)
        if (strlen(word) > ID_MAXSIZE &&
            !strchr(word, '\"') && !strchr(word, '\'') && // Не строковый/символьный литерал
            !isdigit(word[0])) { // Не числовой литерал
            // Ошибка обрабатывается в getWordsTable, здесь просто добавляем слово
        }

        words[words->size].line = line;
        strcpy_s(words[words->size].word, strlen(word) + 1, word);
        words->size++;
    }

    InWord* getWordsTable(std::ostream* stream, unsigned char* text, int* code, int textSize)
    {
        InWord* words = new InWord[LT_MAXSIZE];       // Таблица слов
        words->size = 0;
        int bufpos = 0;                               // Позиция в буфере
        int line = 1;                                 // Текущая строка для добавления слова
        bool isStringLiteral = false;
        bool isCharLiteral = false;
        char buffer[MAX_LEN_BUFFER] = "";             // Буфер

        for (int i = 0; i < textSize; i++)            // Обработка текста
        {
            // Отслеживание строковых и символьных литералов
            if (code[text[i]] == IN::Q) {
                isStringLiteral = !isStringLiteral;
            }
            else if (code[text[i]] == IN::A) {
                isCharLiteral = !isCharLiteral;
            }

            // Если внутри литерала - добавляем символ в буфер
            if (isStringLiteral || isCharLiteral) {
                buffer[bufpos++] = text[i];
                buffer[bufpos] = IN_CODE_NULL;

                // Проверка длины символьного литерала (ошибка 321)
                if (isCharLiteral && text[i] == IN_CODE_APOST && bufpos > 3) {
                    // Символьный литерал не может быть длиннее 3 символов
                    Log::WriteError(stream, Error::geterrorin(321, line, i));
                }

                // Если закрывающая кавычка/апостроф - добавляем литерал в таблицу
                if ((isStringLiteral && text[i] == IN_CODE_QUOTE) ||
                    (isCharLiteral && text[i] == IN_CODE_APOST)) {
                    addWord(words, buffer, line);
                    *buffer = IN_CODE_NULL;
                    bufpos = 0;
                }
                continue;
            }

            switch (code[text[i]])
            {
            case IN::S: // Спецсимвол
            {
                // Обработка составных операторов (==, !=, <=, >=)
                if ((text[i] == '<' || text[i] == '>' || text[i] == '!') && text[i + 1] == '=') {
                    if (*buffer != IN_CODE_NULL) {
                        // Проверка длины идентификатора (ошибка 324)
                        // Для не строковых/символьных литералов и не числовых литералов
                        if (strlen(buffer) > ID_MAXSIZE &&
                            !strchr(buffer, '\"') && !strchr(buffer, '\'') &&
                            !isdigit(buffer[0])) {
                            Log::WriteError(stream, Error::geterrorin(324, line, i - strlen(buffer)));
                        }
                        addWord(words, buffer, line);
                        *buffer = IN_CODE_NULL; bufpos = 0;
                    }
                    buffer[bufpos++] = text[i];
                    buffer[bufpos++] = text[i + 1];
                    buffer[bufpos] = IN_CODE_NULL;
                    addWord(words, buffer, line);
                    *buffer = IN_CODE_NULL;
                    bufpos = 0;
                    i++; // Пропустить следующий символ
                    break;
                }

                // Одиночный спецсимвол
                char letter[] = { (char)text[i], IN_CODE_NULL };
                if (*buffer != IN_CODE_NULL) {
                    // Проверка длины идентификатора (ошибка 324)
                    // Для не строковых/символьных литералов и не числовых литералов
                    if (strlen(buffer) > ID_MAXSIZE &&
                        !strchr(buffer, '\"') && !strchr(buffer, '\'') &&
                        !isdigit(buffer[0])) {
                        Log::WriteError(stream, Error::geterrorin(324, line, i - strlen(buffer)));
                    }
                    addWord(words, buffer, line); // Добавить слово из буфера
                }
                addWord(words, letter, line);     // Добавить спецсимвол
                *buffer = IN_CODE_NULL;
                bufpos = 0;
                break;
            }
            case IN::O: // Оператор
            {
                // Одиночный оператор (например +)
                char letter[] = { (char)text[i], IN_CODE_NULL };
                if (*buffer != IN_CODE_NULL) {
                    // Проверка длины идентификатора (ошибка 324)
                    // Для не строковых/символьных литералов и не числовых литералов
                    if (strlen(buffer) > ID_MAXSIZE &&
                        !strchr(buffer, '\"') && !strchr(buffer, '\'') &&
                        !isdigit(buffer[0])) {
                        Log::WriteError(stream, Error::geterrorin(324, line, i - strlen(buffer)));
                    }
                    addWord(words, buffer, line); // Добавить слово из буфера
                }
                addWord(words, letter, line);     // Добавить оператор
                *buffer = IN_CODE_NULL;
                bufpos = 0;
                break;
            }
            case IN::W: // Пробел или разделитель
            case IN::N: // Новая строка
                if (*buffer != IN_CODE_NULL) {
                    // Проверка длины идентификатора (ошибка 324)
                    // Для не строковых/символьных литералов и не числовых литералов
                    if (strlen(buffer) > ID_MAXSIZE &&
                        !strchr(buffer, '\"') && !strchr(buffer, '\'') &&
                        !isdigit(buffer[0])) {
                        Log::WriteError(stream, Error::geterrorin(324, line, i - strlen(buffer)));
                    }
                    addWord(words, buffer, line);
                    *buffer = IN_CODE_NULL;
                    bufpos = 0;
                }
                if (code[text[i]] == IN::N) {
                    line++;
                }
                break;
            case IN::Q: // Начало/конец строкового литерала
            case IN::A: // Начало/конец символьного литерала
            {
                if (*buffer != IN_CODE_NULL) {
                    // Проверка длины идентификатора (ошибка 324)
                    // Для не строковых/символьных литералов и не числовых литералов
                    if (strlen(buffer) > ID_MAXSIZE &&
                        !strchr(buffer, '\"') && !strchr(buffer, '\'') &&
                        !isdigit(buffer[0])) {
                        Log::WriteError(stream, Error::geterrorin(324, line, i - strlen(buffer)));
                    }
                    addWord(words, buffer, line);
                    *buffer = IN_CODE_NULL;
                    bufpos = 0;
                }
                buffer[bufpos++] = text[i]; // Начать собирать литерал
                break;
            }
            default: // Буквы, цифры, другие допустимые символы
                buffer[bufpos++] = text[i];
                buffer[bufpos] = IN_CODE_NULL;

                // Проверка длины идентификатора во время сборки (ошибка 324)
                // Для не строковых/символьных литералов и не числовых литералов
                // В реальности проверка должна быть после завершения слова (writeline и т.д.)
                // Здесь проверяем только если буфер уже превысил максимальный размер
                if (bufpos > ID_MAXSIZE &&
                    !strchr(buffer, '\"') && !strchr(buffer, '\'') &&
                    !isdigit(buffer[0])) {
                    // Выводим ошибку, но продолжаем собирать слово, т.к. это может быть литерал
                    Log::WriteError(stream, Error::geterrorin(324, line, i));
                }
            }
        }

        // Добавить последнее слово, если оно есть
        if (*buffer != IN_CODE_NULL) {
            // Проверка длины идентификатора (ошибка 324)
            if (strlen(buffer) > ID_MAXSIZE &&
                !strchr(buffer, '\"') && !strchr(buffer, '\'') &&
                !isdigit(buffer[0])) {
                Log::WriteError(stream, Error::geterrorin(324, line, textSize - strlen(buffer)));
            }
            addWord(words, buffer, line);
        }

        return words;
    }

    void printTable(InWord* table) // Вывод таблицы слов
    {
        std::cout << " ------------------ Таблица слов: ------------------" << std::endl;
        for (int i = 0; i < table->size; i++) {
            std::cout << std::setw(2) << i << std::setw(3) << table[i].line << " |  " << table[i].word << std::endl;
        }
        std::cout << "Всего слов: " << table->size << std::endl;
    }
}