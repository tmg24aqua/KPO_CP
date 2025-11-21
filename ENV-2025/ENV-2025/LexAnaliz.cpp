// LexAnaliz.cpp - Реализация лексического анализатора
// 
// Лексический анализатор разбирает исходный код на токены (лексемы):
// - Идентификаторы (переменные, функции)
// - Ключевые слова (function, main, type, return, repeat, is, и т.д.)
// - Литералы (числа, строки, символы, логические значения)
// - Операторы (+, -, *, /, %, =, и т.д.)
// - Разделители (;, ,, (, ), {, }, [, ])
//
// Использует конечные автоматы (FST) для распознавания различных типов лексем

#include "pch.h"

namespace Lex
{
	// Массив графов переходов конечных автоматов для распознавания различных типов лексем
	// Каждый граф соответствует определённому типу токена языка ENV-2025
	Graph graphs[N_GRAPHS] =
	{
		Graph(LEX_SEPARATORS, FST::FST(GRAPH_SEPARATORS)),      // Разделители (;, ,, и т.д.)
		Graph(LEX_ID, FST::FST(GRAPH_ID)),                      // Идентификаторы (переменные, функции)
		Graph(LEX_ID_TYPE, FST::FST(GRAPH_INTEGER)),            // Тип данных integer
		Graph(LEX_ID_TYPE, FST::FST(GRAPH_SYMBOL)),             // Тип данных symbol
		Graph(LEX_STDFUNC, FST::FST(GRAPH_COMPARE)),            // Стандартная функция compare
		Graph(LEX_LITERAL, FST::FST(GRAPH_INTEGER_LITERAL)),    // Целочисленные литералы
		Graph(LEX_LITERAL, FST::FST(GRAPH_BINARY_LITERAL)),     // Двоичные литералы (0b...)
		Graph(LEX_LITERAL, FST::FST(GRAPH_HEX_LITERAL)),        // Шестнадцатеричные литералы (0x...)
		Graph(LEX_LITERAL, FST::FST(GRAPH_STRING_LITERAL)),      // Строковые литералы ("...")
		Graph(LEX_LITERAL, FST::FST(GRAPH_CHAR_LITERAL)),       // Символьные литералы ('...')
		Graph(LEX_FUNCTION, FST::FST(GRAPH_FUNCTION)),          // Ключевое слово function
		Graph(LEX_MAIN, FST::FST(GRAPH_MAIN)),                  // Ключевое слово main
		Graph(LEX_TYPE, FST::FST(GRAPH_TYPE)),                  // Ключевое слово type
		Graph(LEX_RETURN, FST::FST(GRAPH_RETURN)),              // Ключевое слово return
		Graph(LEX_WRITE, FST::FST(GRAPH_WRITE)),                // Ключевое слово write
		Graph(LEX_WRITELINE, FST::FST(GRAPH_WRITELINE)),        // Ключевое слово writeline
		Graph(LEX_REPEAT, FST::FST(GRAPH_REPEAT)),             // Ключевое слово repeat
		Graph(LEX_TIMES, FST::FST(GRAPH_TIMES)),                // Ключевое слово times
		Graph(LEX_IS, FST::FST(GRAPH_IS)),                     // Ключевое слово is
		Graph(LEX_TRUE, FST::FST(GRAPH_TRUE)),                  // Логический литерал true
		Graph(LEX_FALSE, FST::FST(GRAPH_FALSE)),                // Логический литерал false
		Graph(LEX_DO, FST::FST(GRAPH_DO)),                      // Ключевое слово do
		Graph(LEX_END, FST::FST(GRAPH_END))                     // Ключевое слово end
	};

	// Получить имя текущей области видимости (функции или main)
	// Используется для создания полных имён идентификаторов с учётом области видимости
	char* getScopeName(IT::IdTable idtable, char* prevword)
	{
		char* a = new char[6];
		strcpy_s(a, 6, "main");
		if (strcmp(prevword, MAIN_FUNC) == 0)
			return a;
		// Ищем последнюю объявленную функцию в таблице идентификаторов
		for (int i = idtable.size - 1; i >= 0; i--)
			if (idtable.table[i].idtype == IT::IDTYPE::F || idtable.table[i].idtype == IT::IDTYPE::S)
				return idtable.table[i].id;
		return nullptr;
	}

	// Проверка, является ли слово литералом (число, строка, символ, логическое значение)
	bool isLiteral(char* id)
	{
		if (isdigit(*id) || *id == IN_CODE_QUOTE || *id == '\'' || *id == 't' || *id == 'f' || *id == '\0')
			return true;
		return false;
	}

	// Определение стандартной функции по имени
	// Стандартные функции языка ENV-2025: compare, length и т.д.
	IT::STDFNC getStandFunction(char* id)
	{
		if (!strcmp(COMPARE_FUNC, id))
			return IT::STDFNC::F_COMPARE;
		return IT::STDFNC::F_NOT_STD;
	}

	// Поиск литерала в таблице идентификаторов
	// Если литерал с таким значением уже существует, возвращается его индекс
	// Это позволяет не дублировать одинаковые литералы в таблице
	int getLiteralIndex(IT::IdTable ittable, char* value, IT::IDDATATYPE type)
	{
		for (int i = 0; i < ittable.size; i++)
		{
			if (ittable.table[i].idtype == IT::IDTYPE::L && ittable.table[i].iddatatype == type)
			{
				switch (type)
				{
				case IT::IDDATATYPE::INT:
					// Сравнение целочисленных значений
					if (ittable.table[i].value.vint == atoi(value))
						return i;
					break;
				case IT::IDDATATYPE::SYM:
					// Сравнение символьных значений (value[1] - символ внутри кавычек)
					if (ittable.table[i].value.symbol == value[1])
						return i;
					break;
				case IT::IDDATATYPE::STR:
					// Сравнение строковых значений
					if (strcmp(ittable.table[i].value.vstr.str, value) == 0)
						return i;
					break;
				}
			}
		}
		return TI_NULLIDX;  // Литерал не найден
	}

	// Определение типа данных по слову и контексту
	// Типы: INT (integer), SYM (symbol), STR (string), UNDEF (неопределён)
	IT::IDDATATYPE getType(char* curword, char* idtype)
	{
		if (idtype == nullptr)
			return IT::IDDATATYPE::UNDEF;
		if (!strcmp(TYPE_SYMBOL, idtype))
			return IT::IDDATATYPE::SYM;
		if (!strcmp(TYPE_INTEGER, idtype))
			return IT::IDDATATYPE::INT;
		// Определение типа по первому символу слова
		if (isdigit(*curword) || (*curword == '0' && (curword[1] == 'x' || curword[1] == 'b')))
			return IT::IDDATATYPE::INT;  // Число или hex/bin литерал
		else if (*curword == IN_CODE_QUOTE)
			return IT::IDDATATYPE::STR;  // Строка в двойных кавычках
		else if (*curword == '\'')
			return IT::IDDATATYPE::SYM;  // Символ в одинарных кавычках
		else if (!strcmp(curword, TRUE_KW) || !strcmp(curword, FALSE_KW))
			return IT::IDDATATYPE::INT;  // Логические значения как целые числа

		return IT::IDDATATYPE::UNDEF;
	}

	// Генерация уникального имени для литерала
	// Литералы получают имена вида L1, L2, L3 и т.д.
	char* getNextLiteralName()
	{
		static int literalNumber = 1;
		char* buf = new char[SCOPED_ID_MAXSIZE];
		char num[10];
		strcpy_s(buf, SCOPED_ID_MAXSIZE, "L");
		_itoa_s(literalNumber++, num, 10, 10);
		strcat_s(buf, SCOPED_ID_MAXSIZE, num);
		return buf;
	}

	// Поиск индекса первой лексемы в таблице лексем для данного идентификатора
	// Используется для связи таблицы идентификаторов с таблицей лексем
	int getIndexInLT(LT::LexTable& lextable, int itTableIndex)
	{
		if (itTableIndex == TI_NULLIDX)
			return lextable.size;
		for (int i = 0; i < lextable.size; i++)
			if (itTableIndex == lextable.table[i].idxTI)
				return i;
		return TI_NULLIDX;
	}

	// Создание новой записи в таблице идентификаторов
	// Обрабатывает переменные, функции, параметры и литералы
	// Выполняет проверки на корректность объявления
	IT::Entry* getEntry(
		Lex::LEX& tables,
		char lex,           // Тип лексемы (LEX_ID, LEX_LITERAL и т.д.)
		char* id,          // Имя идентификатора
		char* idtype,      // Тип данных (если указан явно)
		bool isParam,      // Является ли параметром функции
		bool isFunc,       // Является ли функцией
		Log::LOG log,      // Лог для записи ошибок
		int line,          // Номер строки в исходном коде
		bool& lex_ok)      // Флаг успешности анализа
	{
		// Определение типа данных
		IT::IDDATATYPE type = getType(id, idtype);
		
		// Проверка, не объявлен ли уже этот идентификатор
		int index = IT::isId(tables.idtable, id);
		// Для литералов ищем существующий литерал с таким же значением
		if (lex == LEX_LITERAL)
			index = getLiteralIndex(tables.idtable, id, type);
		// Если идентификатор уже существует, возвращаем nullptr (будет использован существующий)
		if (index != TI_NULLIDX)
			return nullptr;

		// Создание новой записи в таблице идентификаторов
		IT::Entry* itentry = new IT::Entry;
		itentry->iddatatype = type;
		itentry->idxfirstLE = getIndexInLT(tables.lextable, index);

		// Обработка литералов
		if (lex == LEX_LITERAL)
		{
			// Установка значения литерала
			bool int_ok = IT::SetValue(itentry, id);
			// Проверка корректности целочисленного литерала
			if (!int_ok && itentry->iddatatype == IT::IDDATATYPE::INT)
			{
				Log::WriteError(log.stream, Error::geterrorin(313, line, 0));
				lex_ok = false;
			}
			// Проверка длины строкового литерала
			if (itentry->iddatatype == IT::IDDATATYPE::STR && strlen(id) > STR_MAXSIZE)
			{
				Log::WriteError(log.stream, Error::geterrorin(312, line, 0));
				lex_ok = false;
			}
			// Литералы получают автоматические имена L1, L2, L3...
			strcpy_s(itentry->id, SCOPED_ID_MAXSIZE, getNextLiteralName());
			itentry->idtype = IT::IDTYPE::L;  // L = Literal
		}
		// Обработка идентификаторов (не литералов)
		else
		{
			// Инициализация значений по умолчанию в зависимости от типа
			switch (type)
			{
			case IT::IDDATATYPE::INT:
				itentry->value.vint = TI_INT_DEFAULT;  // 0
				break;
			case IT::IDDATATYPE::SYM:
				itentry->value.symbol = TI_SYMBOL_DEFAULT;  // '\0'
				break;
			case IT::IDDATATYPE::STR:
				itentry->value.vstr.len = 0;
				memset(itentry->value.vstr.str, 0, STR_MAXSIZE);
				break;
			}

			// Определение типа идентификатора: функция, параметр или переменная
			if (isFunc)
			{
				// Обработка функций
				switch (getStandFunction(id))
				{
				case IT::STDFNC::F_COMPARE:
					// Стандартная функция compare
					itentry->idtype = IT::IDTYPE::S;  // S = Standard function
					itentry->iddatatype = IT::IDDATATYPE::INT;
					itentry->value.params.count = COMPARE_PARAMS_CNT;  // 2 параметра
					itentry->value.params.types = new IT::IDDATATYPE[COMPARE_PARAMS_CNT];
					for (int k = 0; k < COMPARE_PARAMS_CNT; k++)
						itentry->value.params.types[k] = IT::COMPARE_PARAMS[k];
					break;
				case IT::STDFNC::F_NOT_STD:
					// Пользовательская функция
					itentry->idtype = IT::IDTYPE::F;  // F = Function
					break;
				}
			}
			else if (isParam)
				itentry->idtype = IT::IDTYPE::P;  // P = Parameter
			else
				itentry->idtype = IT::IDTYPE::V;  // V = Variable

			// Копирование имени идентификатора
			memset(itentry->id, '\0', SCOPED_ID_MAXSIZE);
			strncat_s(itentry->id, SCOPED_ID_MAXSIZE, id, _TRUNCATE);
		}

		// Семантические проверки на этапе лексического анализа
		int i = tables.lextable.size;
		// Проверка: переменная должна быть объявлена после ключевого слова type
		if (i > 1 && itentry->idtype == IT::IDTYPE::V && tables.lextable.table[i - 2].lexema != LEX_TYPE)
		{
			Log::WriteError(log.stream, Error::geterrorin(304, line, 0));
			lex_ok = false;
		}
		// Проверка: функция должна быть объявлена после ключевого слова function
		if (i > 1 && itentry->idtype == IT::IDTYPE::F && tables.lextable.table[i - 1].lexema != LEX_FUNCTION)
		{
			Log::WriteError(log.stream, Error::geterrorin(303, line, 0));
			lex_ok = false;
		}
		// Проверка: тип данных должен быть определён
		if (itentry->iddatatype == IT::IDDATATYPE::UNDEF)
		{
			Log::WriteError(log.stream, Error::geterrorin(300, line, 0));
			lex_ok = false;
		}
		return itentry;
	}

	// Основная функция лексического анализа
	// Проходит по всем словам исходного кода и классифицирует их на лексемы
	// Заполняет таблицу лексем (LT) и таблицу идентификаторов (IT)
	bool analyze(LEX& tables, In::IN& in, Log::LOG& log, Parm::PARM& parm)
	{
		static bool lex_ok = true;  // Флаг успешности анализа
		
		// Инициализация таблиц
		tables.lextable = LT::Create(LT_MAXSIZE);      // Таблица лексем
		tables.idtable = IT::Create(MAXSIZE_TI);       // Таблица идентификаторов
		
		bool isParam = false, isFunc = false;  // Флаги для определения контекста
		int enterPoint = 0;  // Счётчик точек входа (должна быть одна функция main)
		char curword[STR_MAXSIZE], nextword[STR_MAXSIZE];  // Текущее и следующее слово
		int curline;  // Номер текущей строки
		std::stack <char*> scopes;  // Стек областей видимости

		// Проход по всем словам исходного кода
		for (int i = 0; i < in.words->size; i++)
		{
			// Получение текущего слова и следующего (для контекстного анализа)
			strcpy_s(curword, in.words[i].word);
			if (i < in.words->size - 1)
				strcpy_s(nextword, in.words[i + 1].word);
			curline = in.words[i].line;
			isFunc = false;
			int idxTI = TI_NULLIDX;  // Индекс в таблице идентификаторов

			// Попытка распознать слово с помощью конечных автоматов
			// Проверяем все возможные типы лексем
			for (int j = 0; j < N_GRAPHS; j++)
			{
				// Создание конечного автомата для текущего слова и графа распознавания
				FST::FST fst((unsigned char*)curword, graphs[j].graph);
				if (FST::execute(fst))  // Если автомат распознал слово
				{
					char lexema = graphs[j].lexema;  // Тип распознанной лексемы

					// Обработка различных типов лексем
					switch (lexema)
					{
					case LEX_MAIN:
						// Обнаружена функция main - точка входа программы
						enterPoint++;
						break;
					case LEX_SEPARATORS:
						// Разделители: используем сам символ как лексему (;, ,, и т.д.)
						lexema = *curword;
						break;
					case LEX_ID:        // Идентификатор (переменная, функция)
					case LEX_STDFUNC:    // Стандартная функция
					case LEX_LITERAL:    // Литерал (число, строка, символ)
					{
						char id[STR_MAXSIZE] = "";
						idxTI = TI_NULLIDX;

						// Определение, является ли идентификатор функцией
						// (если после него идёт '(' или он уже объявлен как функция)
						if (*nextword == '(' || IT::isId(tables.idtable, curword) != TI_NULLIDX)
						{
							isFunc = true;
						}

						// Получение типа данных из контекста (слово перед идентификатором)
						char* idtype = (isFunc && i > 1) ? in.words[i - 2].word : in.words[i - 1].word;
						if (i == 0)
							idtype = nullptr;

						// Формирование полного имени с учётом области видимости
						if (!isFunc && !scopes.empty())
							strncpy_s(id, STR_MAXSIZE, scopes.top(), ID_MAXSIZE);
						strncat_s(id, STR_MAXSIZE, curword, _TRUNCATE);

						// Для литералов используем их значение как имя
						if (isLiteral(curword))
							strcpy_s(id, STR_MAXSIZE, curword);

						// Создание или поиск записи в таблице идентификаторов
						IT::Entry* itentry = getEntry(tables, lexema, id, idtype, isParam, isFunc, log, curline, lex_ok);
						if (itentry != nullptr)
						{
							// Новая запись - добавляем в таблицу
							IT::Add(tables.idtable, *itentry);
							idxTI = tables.idtable.size - 1;
						}
						else
						{
							// Запись уже существует - получаем её индекс
							idxTI = IT::isId(tables.idtable, id);
						}
					}
					break;
					}

					// Добавление лексемы в таблицу лексем
					LT::Entry* ltentry = new LT::Entry(lexema, curline, idxTI);
					LT::Add(tables.lextable, *ltentry);
					break;
				}
				else if (j == N_GRAPHS - 1)
				{
					// Если ни один автомат не распознал слово - это ошибка
					Log::WriteError(log.stream, Error::geterrorin(201, curline, 0));
					lex_ok = false;
				}
			}
		}

		// Финальные проверки
		// Должна быть ровно одна функция main
		if (enterPoint == 0)
		{
			Log::WriteError(log.stream, Error::geterror(301));  // Нет функции main
			lex_ok = false;
		}
		if (enterPoint > 1)
		{
			Log::WriteError(log.stream, Error::geterror(302));  // Несколько функций main
			lex_ok = false;
		}
		return lex_ok;  // Возврат результата лексического анализа
	}
}