// Semantic.cpp - Реализация семантического анализатора
//
// Семантический анализатор проверяет семантические правила языка ENV-2025:
// - Соответствие типов данных в операциях
// - Правильность использования функций (количество и типы параметров)
// - Проверка деления на ноль
// - Корректность операций с типами данных
// - Проверка возвращаемых значений функций
//
// Семантический анализ выполняется после синтаксического анализа,
// так как требует полной информации о структуре программы

#include "pch.h"

namespace Semantic
{
    // Основная функция семантического анализа
    // Проходит по таблице лексем и проверяет семантические правила
    bool semanticsCheck(Lex::LEX& tables, Log::LOG& log)
    {
        bool sem_ok = true;

        // Проход по всем лексемам программы
        for (int i = 0; i < tables.lextable.size; i++)
        {
            switch (tables.lextable.table[i].lexema)
            {
            case LEX_TYPE:
            {
                // Проверка: после ключевого слова type должен следовать тип данных
                if (i + 1 >= tables.lextable.size || tables.lextable.table[i + 1].lexema != LEX_ID_TYPE)
                {
                    sem_ok = false;
                    Log::WriteError(log.stream, Error::geterrorin(303, tables.lextable.table[i].sn, 0));
                }
                break;
            }

            case LEX_EQUAL:  // Операция присваивания
            {
                if (i > 0 && tables.lextable.table[i - 1].idxTI != TI_NULLIDX)
                {
                    IT::IDDATATYPE leftType = tables.idtable.table[tables.lextable.table[i - 1].idxTI].iddatatype;

                    for (int k = i + 1; k < tables.lextable.size && tables.lextable.table[k].lexema != LEX_SEPARATOR; k++)
                    {
                        if (tables.lextable.table[k].idxTI != TI_NULLIDX)
                        {
                            IT::IDDATATYPE rightType = tables.idtable.table[tables.lextable.table[k].idxTI].iddatatype;

                            if (leftType != rightType && !(leftType == IT::INT && rightType == IT::UNDEF))
                            {
                                Log::WriteError(log.stream, Error::geterrorin(314, tables.lextable.table[k].sn, 0));
                                sem_ok = false;
                                break;
                            }
                        }

                        if ((leftType == IT::SYM) &&
                            (tables.lextable.table[k].lexema == LEX_PLUS ||
                                tables.lextable.table[k].lexema == LEX_MINUS ||
                                tables.lextable.table[k].lexema == LEX_STAR ||
                                tables.lextable.table[k].lexema == LEX_DIRSLASH ||
                                tables.lextable.table[k].lexema == LEX_PROCENT))
                        {
                            Log::WriteError(log.stream, Error::geterrorin(316, tables.lextable.table[k].sn, 0));
                            sem_ok = false;
                            break;
                        }
                    }
                }
                break;
            }

            case LEX_DIRSLASH:  // Деление
            case LEX_PROCENT:   // Остаток от деления
            {
                // Проверка деления на ноль
                if (i + 1 < tables.lextable.size && tables.lextable.table[i + 1].idxTI != TI_NULLIDX)
                {
                    IT::Entry& rightOperand = tables.idtable.table[tables.lextable.table[i + 1].idxTI];
                    // Если правый операнд - целое число, равное нулю
                    if (rightOperand.iddatatype == IT::INT && rightOperand.value.vint == 0)
                    {
                        Log::WriteError(log.stream, Error::geterrorin(318, tables.lextable.table[i].sn, 0));
                        sem_ok = false;
                    }
                }
                break;
            }

            case LEX_FUNCTION:
            {
                if (i + 2 < tables.lextable.size && tables.lextable.table[i + 2].idxTI != TI_NULLIDX)
                {
                    IT::Entry& function = tables.idtable.table[tables.lextable.table[i + 2].idxTI];
                    if (function.idtype != IT::F)
                    {
                        Log::WriteError(log.stream, Error::geterrorin(303, tables.lextable.table[i].sn, 0));
                        sem_ok = false;
                    }
                }
                break;
            }

            case LEX_RETURN:
            {
                int functionStart = -1;
                for (int j = i; j >= 0; j--)
                {
                    if (tables.lextable.table[j].lexema == LEX_FUNCTION)
                    {
                        functionStart = j;
                        break;
                    }
                }

                if (functionStart != -1 && functionStart + 2 < tables.lextable.size)
                {
                    IT::Entry& function = tables.idtable.table[tables.lextable.table[functionStart + 2].idxTI];

                    if (i + 1 < tables.lextable.size && tables.lextable.table[i + 1].idxTI != TI_NULLIDX)
                    {
                        IT::Entry& returnValue = tables.idtable.table[tables.lextable.table[i + 1].idxTI];
                        if (function.iddatatype != returnValue.iddatatype)
                        {
                            Log::WriteError(log.stream, Error::geterrorin(315, tables.lextable.table[i].sn, 0));
                            sem_ok = false;
                        }
                    }
                }
                break;
            }

            case LEX_REPEAT:
            {
                if (i + 2 >= tables.lextable.size || tables.lextable.table[i + 1].idxTI == TI_NULLIDX)
                {
                    Log::WriteError(log.stream, Error::geterrorin(618, tables.lextable.table[i].sn, 0));
                    sem_ok = false;
                }
                else
                {
                    IT::Entry& count = tables.idtable.table[tables.lextable.table[i + 1].idxTI];
                    if (count.iddatatype != IT::INT)
                    {
                        Log::WriteError(log.stream, Error::geterrorin(614, tables.lextable.table[i].sn, 0));
                        sem_ok = false;
                    }
                }
                break;
            }

            case LEX_IS:
            {
                if (i + 3 >= tables.lextable.size)
                {
                    Log::WriteError(log.stream, Error::geterrorin(606, tables.lextable.table[i].sn, 0));
                    sem_ok = false;
                }
                else
                {
                    IT::IDDATATYPE leftType = IT::UNDEF;
                    IT::IDDATATYPE rightType = IT::UNDEF;

                    if (tables.lextable.table[i + 1].idxTI != TI_NULLIDX)
                        leftType = tables.idtable.table[tables.lextable.table[i + 1].idxTI].iddatatype;
                    if (tables.lextable.table[i + 3].idxTI != TI_NULLIDX)
                        rightType = tables.idtable.table[tables.lextable.table[i + 3].idxTI].iddatatype;

                    if (leftType != rightType && leftType != IT::UNDEF && rightType != IT::UNDEF)
                    {
                        Log::WriteError(log.stream, Error::geterrorin(317, tables.lextable.table[i].sn, 0));
                        sem_ok = false;
                    }
                }
                break;
            }

            case LEX_ID:
            {
                if (i + 1 < tables.lextable.size && tables.lextable.table[i + 1].lexema == LEX_LEFTHESIS)
                {
                    IT::Entry& function = tables.idtable.table[tables.lextable.table[i].idxTI];

                    if (function.idtype == IT::F || function.idtype == IT::S)
                    {
                        int paramCount = 0;
                        int j = i + 2;

                        while (j < tables.lextable.size && tables.lextable.table[j].lexema != LEX_RIGHTTHESIS)
                        {
                            if (tables.lextable.table[j].lexema == LEX_ID || tables.lextable.table[j].lexema == LEX_LITERAL)
                            {
                                paramCount++;
                                if (paramCount > MAX_PARAMS_COUNT)
                                {
                                    Log::WriteError(log.stream, Error::geterrorin(307, tables.lextable.table[i].sn, 0));
                                    sem_ok = false;
                                    break;
                                }
                            }
                            j++;
                        }

                        if (paramCount != function.value.params.count)
                        {
                            Log::WriteError(log.stream, Error::geterrorin(308, tables.lextable.table[i].sn, 0));
                            sem_ok = false;
                        }
                    }
                }
                break;
            }
            }
        }

        return sem_ok;
    }
}

