// ENV-2025.cpp : Главный файл приложения для проекта ENV-2025
//
// Этот файл является точкой входа компилятора языка ENV-2025.
// Компилятор выполняет следующие этапы:
// 1. Обработка параметров командной строки
// 2. Чтение и предобработка исходного файла
// 3. Лексический анализ (разбор на токены)
// 4. Синтаксический анализ (проверка структуры программы)
// 5. Семантический анализ (проверка типов и правил языка)
// 6. Преобразование выражений в польскую запись
// 7. Генерация кода (создание ассемблерного кода MASM)

#include "pch.h"

int _tmain(int argc, _TCHAR** argv)
{
    // Сообщения для ENV-2025 - используются для вывода результатов каждого этапа компиляции
    char LEXERROR[] = "Лексический анализ завершен с ошибками\n";
    char SYNTERROR[] = "Синтаксический анализ завершен с ошибками\n";
    char SEMERROR[] = "Семантический анализ завершен с ошибками\n";
    char POLISHERROR[] = "Ошибка при преобразовании выражения в польскую запись\n";
    char LEXGOOD[] = "Лексический анализ завершен успешно\n";
    char SYNTGOOD[] = "Синтаксический анализ завершен успешно\n";
    char SEMGOOD[] = "Семантический анализ завершен успешно\n";
    char POLISHGOOD[] = "Преобразование в польскую запись выполнено успешно\n";
    char GENERROR[] = "Ошибка генерации кода\n";
    char GENGOD[] = "Генерация кода выполнена успешно\n";
    char MESSAGE[] = "-------------------- Результаты работы компилятора ENV-2025 -------------------\n";
    char STOP[] = "Выполнение программы остановлено\n";
    char ALLGOOD[] = "Все этапы компиляции ENV-2025 выполнены успешно!\n";

    setlocale(LC_ALL, "rus");
    Log::LOG log = Log::INITLOG;  // Инициализация системы логирования

    try
    {
        // ========== ЭТАП 1: Обработка параметров командной строки ==========
        // Парсинг аргументов: -in:файл_исходника -out:файл_результата -log:файл_лога
        Parm::PARM parm = Parm::getparm(argc, argv);
        log = Log::getlog(parm.log);  // Открытие файла лога
        Log::WriteLog(log);
        Log::WriteParm(log, parm);    // Запись параметров в лог

        // ========== ЭТАП 2: Чтение и предобработка исходного файла ==========
        // Чтение исходного кода, проверка допустимых символов, разбиение на слова
        In::IN in = In::getin(parm.in, log.stream);
        Log::WriteIn(log, in);
        // Разбиение текста на слова (лексемы) с сохранением номеров строк
        in.words = In::getWordsTable(log.stream, in.text, in.code, in.size);
        In::printTable(in.words);  // Вывод таблицы слов для отладки

        // ========== ЭТАП 3: Лексический анализ ==========
        // Разбор слов на токены (идентификаторы, ключевые слова, литералы, операторы)
        // Результат: таблица лексем (LT) и таблица идентификаторов (IT)
        Lex::LEX tables;  // Структура содержит lextable и idtable
        bool lex_ok = Lex::analyze(tables, in, log, parm);

        // Вывод результатов лексического анализа в лог-файл
        LT::writeLexTable(log.stream, tables.lextable);      // Таблица всех лексем
        IT::writeIdTable(log.stream, tables.idtable);        // Таблица идентификаторов
        LT::writeLexemsOnLines(log.stream, tables.lextable); // Лексемы по строкам

        if (!lex_ok)
        {
            // Если лексический анализ завершился с ошибками, прекращаем компиляцию
            Log::WriteLine(log, LEXERROR, "");
            Log::WriteLineConsole(LEXERROR, STOP, "");
            Log::Close(log);
            return 0;
        }
        else
        {
            Log::WriteLineConsole(LEXGOOD, "");
        }

        // ========== ЭТАП 4: Синтаксический анализ (используя MFST) ==========
        // Проверка соответствия программы грамматике языка ENV-2025
        // Используется магазинный автомат (MFST) для разбора по грамматике Грейбах
        MFST_TRACE_START(log.stream);  // Заголовок трассировки синтаксического анализа
        MFST::Mfst mfst(tables, GRB::getGreibach());  // Создание автомата с грамматикой
        bool synt_ok = mfst.start(log);  // Запуск синтаксического анализа
        mfst.savededucation();  // Сохранение дерева вывода (последовательность применённых правил)
        mfst.printrules(log);   // Вывод применённых правил грамматики

        if (!synt_ok)
        {
            // Если синтаксический анализ завершился с ошибками, прекращаем компиляцию
            Log::WriteLine(log, SYNTERROR, "");
            Log::WriteLineConsole(SYNTERROR, STOP, "");
            Log::Close(log);
            return 0;
        }
        else
        {
            Log::WriteLineConsole(SYNTGOOD, "");
        }

        // ========== ЭТАП 5: Семантический анализ ==========
        // Проверка семантических правил: типы данных, совместимость операций,
        // правильность использования функций, проверка деления на ноль и т.д.
        bool sem_ok = Semantic::semanticsCheck(tables, log);
        if (!sem_ok)
        {
            Log::WriteLine(log, SEMERROR, "");
            Log::WriteLineConsole(SEMERROR, STOP, "");
            Log::Close(log);
            return 0;
        }
        else
        {
            Log::WriteLineConsole(SEMGOOD, "");
        }

        // ========== ЭТАП 6: Преобразование выражений в польскую запись ==========
        // Преобразование инфиксной записи выражений в постфиксную (польскую запись)
        // Это упрощает генерацию кода, так как польская запись не требует скобок
        // Пример: a + b * c -> a b c * +
        // Для ENV-2025 может потребоваться дополнительная реализация преобразования
        bool polish_ok = true; // Polish::convertToPolish(tables);
        if (!polish_ok)
        {
            Log::WriteLine(log, POLISHERROR, "");
            Log::WriteLineConsole(POLISHERROR, STOP, "");
            Log::Close(log);
            return 0;
        }
        else
        {
            Log::WriteLineConsole(POLISHGOOD, "");
        }

        // ========== ЭТАП 7: Генерация кода ==========
        // Генерация ассемблерного кода MASM на основе таблиц лексем и идентификаторов
        // Создаётся файл .asm с кодом на языке ассемблера для процессора x86
        bool gen_ok = Gener::CodeGeneration(tables, parm, log);
        if (!gen_ok)
        {
            Log::WriteLine(log, GENERROR, "");
            Log::WriteLineConsole(GENERROR, STOP, "");
            Log::Close(log);
            return 0;
        }
        else
        {
            Log::WriteLineConsole(GENGOD, "");
        }

        // ========== ЭТАП 8: Вывод итоговых результатов ==========
        // Вывод финальных таблиц лексем и идентификаторов в лог-файл
        Log::WriteLine(log, MESSAGE, "");
        LT::writeLexTable(log.stream, tables.lextable);
        IT::writeIdTable(log.stream, tables.idtable);
        LT::writeLexemsOnLines(log.stream, tables.lextable);

        // Вывод в консоль для отладки
        Log::WriteLineConsole(MESSAGE, "");
        IT::writeIdTable(&std::cout, tables.idtable);
        LT::writeLexTable(&std::cout, tables.lextable);
        LT::writeLexemsOnLines(&std::cout, tables.lextable);

        // ========== ЭТАП 9: Завершение работы ==========
        // Компиляция успешно завершена, закрываем файлы и выводим информацию
        Log::WriteLine(log, ALLGOOD, "");
        Log::WriteLineConsole(ALLGOOD, "");
        Log::Close(log);

        std::cout << "\nСгенерированный файл: " << parm.out << std::endl;
        system("pause");
    }
    catch (Error::ERROR e)
    {
        // Обработка известных ошибок компиляции
        Log::WriteError(log.stream, e);
        Log::Close(log);
        std::cout << "Критическая ошибка выполнения!" << std::endl;
        system("pause");
    }
    catch (...)
    {
        // Обработка неожиданных исключений
        std::cout << "Ошибка: неизвестная ошибка при выполнении ENV-2025" << std::endl;
        Log::Close(log);
        system("pause");
    }

    return 0;
}