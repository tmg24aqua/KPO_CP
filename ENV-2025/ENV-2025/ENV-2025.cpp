// ENV-2025.cpp : Главный файл приложения для проекта ENV-2025
//

#include "pch.h"

int _tmain(int argc, _TCHAR** argv)
{
    // Сообщения для ENV-2025
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
    Log::LOG log = Log::INITLOG;

    try
    {
        // 1. Обработка параметров командной строки
        Parm::PARM parm = Parm::getparm(argc, argv);
        log = Log::getlog(parm.log);
        Log::WriteLog(log);
        Log::WriteParm(log, parm);

        // 2. Чтение и обработка исходного файла
        In::IN in = In::getin(parm.in, log.stream);
        Log::WriteIn(log, in);
        in.words = In::getWordsTable(log.stream, in.text, in.code, in.size);
        In::printTable(in.words);

        // 3. Лексический анализ
        Lex::LEX tables;
        bool lex_ok = Lex::analyze(tables, in, log, parm);

        // Вывод результатов лексического анализа
        LT::writeLexTable(log.stream, tables.lextable);
        IT::writeIdTable(log.stream, tables.idtable);
        LT::writeLexemsOnLines(log.stream, tables.lextable);

        if (!lex_ok)
        {
            Log::WriteLine(log, LEXERROR, "");
            Log::WriteLineConsole(LEXERROR, STOP, "");
            Log::Close(log);
            return 0;
        }
        else
        {
            Log::WriteLineConsole(LEXGOOD, "");
        }

        // 4. Синтаксический анализ (используя MFST)
        MFST_TRACE_START(log.stream);
        MFST::Mfst mfst(tables, GRB::getGreibach());
        bool synt_ok = mfst.start(log);
        mfst.savededucation();
        mfst.printrules(log);

        if (!synt_ok)
        {
            Log::WriteLine(log, SYNTERROR, "");
            Log::WriteLineConsole(SYNTERROR, STOP, "");
            Log::Close(log);
            return 0;
        }
        else
        {
            Log::WriteLineConsole(SYNTGOOD, "");
        }

        // 5. Семантический анализ
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

        // 6. Преобразование выражений в польскую запись (генерация промежуточного кода)
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

        // 7. Генерация кода
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

        // 8. Вывод итоговых результатов
        Log::WriteLine(log, MESSAGE, "");
        LT::writeLexTable(log.stream, tables.lextable);
        IT::writeIdTable(log.stream, tables.idtable);
        LT::writeLexemsOnLines(log.stream, tables.lextable);

        // Вывод в консоль для отладки
        Log::WriteLineConsole(MESSAGE, "");
        IT::writeIdTable(&std::cout, tables.idtable);
        LT::writeLexTable(&std::cout, tables.lextable);
        LT::writeLexemsOnLines(&std::cout, tables.lextable);

        // 9. Завершение работы
        Log::WriteLine(log, ALLGOOD, "");
        Log::WriteLineConsole(ALLGOOD, "");
        Log::Close(log);

        std::cout << "\nСгенерированный файл: " << parm.out << std::endl;
        system("pause");
    }
    catch (Error::ERROR e)
    {
        Log::WriteError(log.stream, e);
        Log::Close(log);
        std::cout << "Критическая ошибка выполнения!" << std::endl;
        system("pause");
    }
    catch (...)
    {
        std::cout << "Ошибка: неизвестная ошибка при выполнении ENV-2025" << std::endl;
        Log::Close(log);
        system("pause");
    }

    return 0;
}