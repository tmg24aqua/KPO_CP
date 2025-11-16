#include "pch.h"
#define W(x, y)\
		<< std::setw(x) << (y) <<
#define STR(n, line, type, id)\
		"|" W(4,n) " |  " W(6,line) "    |" W(21,type) " |  " W(SCOPED_ID_MAXSIZE, id) " |"
namespace IT
{
	IdTable Create(int size)
	{
		if (size > MAXSIZE_TI)
			throw ERROR_THROW(203);
		IdTable idtable;
		idtable.table = new Entry[idtable.maxsize = size];
		idtable.size = NULL;
		return idtable;
	}
	void Add(IdTable& idtable, Entry entry)
	{
		if (idtable.size >= idtable.maxsize)
			throw ERROR_THROW(203);
		idtable.table[idtable.size++] = entry;
	}

	// возврат: номер строки(если есть), TI_NULLIDX(если нет)
	int isId(IdTable& idtable, char id[SCOPED_ID_MAXSIZE])
	{
		for (int i = 0; i < idtable.size; i++)
		{
			if (strcmp(idtable.table[i].id, id) == 0)
				return i;
		}
		return TI_NULLIDX;
	}

	bool SetValue(IT::IdTable& idtable, int index, char* value)
	{
		return SetValue(&(idtable.table[index]), value);
	}

	bool SetValue(IT::Entry* entry, char* value) // установка значения переменной
	{
		bool rc = true;
		std::ostream* stream = nullptr; // Нужно передавать stream для вывода ошибок

		if (entry->iddatatype == INT)
		{
			int temp;
			// Обработка шестнадцатеричных литералов (0x5)
			if ((value[0] == '-' && value[1] == '0' && value[2] == 'x') ||
				(value[0] == '0' && value[1] == 'x'))
			{
				char* endptr;
				temp = strtol(value, &endptr, 16);
				// Проверка валидности шестнадцатеричного литерала (ошибка 323)
				if (*endptr != '\0' && *endptr != ' ' && *endptr != ';' && *endptr != '\n') {
					Log::WriteError(stream, Error::geterrorin(323, entry->idxfirstLE, 0));
					rc = false;
				}
			}
			// Обработка двоичных литералов (0b1101)
			else if ((value[0] == '-' && value[1] == '0' && value[2] == 'b') ||
				(value[0] == '0' && value[1] == 'b'))
			{
				// Убираем префикс "0b" или "-0b"
				char* binStart = value;
				if (value[0] == '-')
					binStart = value + 3; // "-0b..."
				else
					binStart = value + 2; // "0b..."

				temp = 0;
				bool validBinary = true;
				for (int i = 0; binStart[i] != '\0'; i++)
				{
					if (binStart[i] == '0' || binStart[i] == '1') {
						temp = temp * 2 + (binStart[i] - '0');
					}
					else if (binStart[i] != ' ' && binStart[i] != ';' && binStart[i] != '\n') {
						// Недопустимый символ в двоичном литерале (ошибка 322)
						validBinary = false;
						break;
					}
				}

				if (!validBinary) {
					Log::WriteError(stream, Error::geterrorin(322, entry->idxfirstLE, 0));
					rc = false;
				}

				if (value[0] == '-') temp = -temp;
			}
			else
			{
				char* endptr;
				temp = strtol(value, &endptr, 10);
				// Проверка валидности десятичного литерала
				if (*endptr != '\0' && *endptr != ' ' && *endptr != ';' && *endptr != '\n') {
					// Недопустимый целочисленный литерал (ошибка 313)
					rc = false;
				}
			}

			// Проверка диапазона для 2-байтового integer (ошибка 325)
			if (temp > TI_INT_MAXSIZE || temp < TI_INT_MINSIZE)
			{
				if (temp > TI_INT_MAXSIZE)
					temp = TI_INT_MAXSIZE;
				if (temp < TI_INT_MINSIZE)
					temp = TI_INT_MINSIZE;
				Log::WriteError(stream, Error::geterrorin(325, entry->idxfirstLE, 0));
				rc = false;
			}
			entry->value.vint = temp;
		}
		else if (entry->iddatatype == CHAR)
		{
			// Обработка символьных литералов в одинарных кавычках
			if (value[0] == '\'' && strlen(value) >= 3)
			{
				// Проверка корректности символьного литерала (ошибка 321)
				if (value[2] != '\'' || strlen(value) > 3) {
					// Символьный литерал должен быть формата 'X'
					Log::WriteError(stream, Error::geterrorin(321, entry->idxfirstLE, 0));
					rc = false;
				}
				entry->value.vsymbol = value[1]; // Берем символ между кавычками
			}
			else
			{
				entry->value.vsymbol = value[0]; // Одиночный символ
			}
		}
		else if (entry->iddatatype == UNDEF && entry->idtype == L)
		{
			// Обработка логических литералов
			if (strcmp(value, "true") == 0)
				entry->value.vint = 1;
			else if (strcmp(value, "false") == 0)
				entry->value.vint = 0;
			else {
				// Недопустимое значение для логического литерала
				rc = false;
			}
		}
		return rc;
	}

	// Новая перегруженная версия SetValue с stream для вывода ошибок
	bool SetValue(IT::Entry* entry, char* value, std::ostream* stream, int line, int col)
	{
		bool rc = true;

		if (entry->iddatatype == INT)
		{
			int temp;
			// Обработка шестнадцатеричных литералов (0x5)
			if ((value[0] == '-' && value[1] == '0' && value[2] == 'x') ||
				(value[0] == '0' && value[1] == 'x'))
			{
				char* endptr;
				temp = strtol(value, &endptr, 16);
				// Проверка валидности шестнадцатеричного литерала (ошибка 323)
				if (*endptr != '\0' && *endptr != ' ' && *endptr != ';' && *endptr != '\n') {
					if (stream) Log::WriteError(stream, Error::geterrorin(323, line, col));
					rc = false;
				}
			}
			// Обработка двоичных литералов (0b1101)
			else if ((value[0] == '-' && value[1] == '0' && value[2] == 'b') ||
				(value[0] == '0' && value[1] == 'b'))
			{
				// Убираем префикс "0b" или "-0b"
				char* binStart = value;
				if (value[0] == '-')
					binStart = value + 3; // "-0b..."
				else
					binStart = value + 2; // "0b..."

				temp = 0;
				bool validBinary = true;
				for (int i = 0; binStart[i] != '\0'; i++)
				{
					if (binStart[i] == '0' || binStart[i] == '1') {
						temp = temp * 2 + (binStart[i] - '0');
					}
					else if (binStart[i] != ' ' && binStart[i] != ';' && binStart[i] != '\n') {
						// Недопустимый символ в двоичном литерале (ошибка 322)
						validBinary = false;
						break;
					}
				}

				if (!validBinary) {
					if (stream) Log::WriteError(stream, Error::geterrorin(322, line, col));
					rc = false;
				}

				if (value[0] == '-') temp = -temp;
			}
			else
			{
				char* endptr;
				temp = strtol(value, &endptr, 10);
				// Проверка валидности десятичного литерала
				if (*endptr != '\0' && *endptr != ' ' && *endptr != ';' && *endptr != '\n') {
					if (stream) Log::WriteError(stream, Error::geterrorin(313, line, col));
					rc = false;
				}
			}

			// Проверка диапазона для 2-байтового integer (ошибка 325)
			if (temp > TI_INT_MAXSIZE || temp < TI_INT_MINSIZE)
			{
				if (temp > TI_INT_MAXSIZE)
					temp = TI_INT_MAXSIZE;
				if (temp < TI_INT_MINSIZE)
					temp = TI_INT_MINSIZE;
				if (stream) Log::WriteError(stream, Error::geterrorin(325, line, col));
				rc = false;
			}
			entry->value.vint = temp;
		}
		else if (entry->iddatatype == CHAR)
		{
			// Обработка символьных литералов в одинарных кавычках
			if (value[0] == '\'' && strlen(value) >= 3)
			{
				// Проверка корректности символьного литерала (ошибка 321)
				if (value[2] != '\'' || strlen(value) > 3) {
					// Символьный литерал должен быть формата 'X'
					if (stream) Log::WriteError(stream, Error::geterrorin(321, line, col));
					rc = false;
				}
				entry->value.vsymbol = value[1]; // Берем символ между кавычками
			}
			else if (strlen(value) == 1)
			{
				entry->value.vsymbol = value[0]; // Одиночный символ
			}
			else
			{
				if (stream) Log::WriteError(stream, Error::geterrorin(321, line, col));
				rc = false;
			}
		}
		else if (entry->iddatatype == UNDEF && entry->idtype == L)
		{
			// Обработка логических литералов
			if (strcmp(value, "true") == 0)
				entry->value.vint = 1;
			else if (strcmp(value, "false") == 0)
				entry->value.vint = 0;
			else {
				// Недопустимое значение для логического литерала
				rc = false;
			}
		}
		return rc;
	}

	void writeIdTable(std::ostream* stream, IT::IdTable& idtable)
	{
		*stream << "---------------------------- ТАБЛИЦА ИДЕНТИФИКАТОРОВ ENV-2025 ------------------------\n" << std::endl;
		*stream << "|  N  |СТРОКА В ТЛ |  ТИП ИДЕНТИФИКАТОРА  |        ИМЯ        | ЗНАЧЕНИЕ (ПАРАМЕТРЫ)" << std::endl;
		for (int i = 0; i < idtable.size; i++)
		{
			IT::Entry* e = &idtable.table[i];
			char type[50] = "";

			switch (e->iddatatype)
			{
			case IT::IDDATATYPE::INT:
				strcat(type, "  integer ");
				break;
			case IT::IDDATATYPE::CHAR:
				strcat(type, "   char   ");
				break;
			case IT::IDDATATYPE::PROC:
				strcat(type, " procedure");
				break;
			case IT::IDDATATYPE::UNDEF:
				strcat(type, " UNDEFINED");
				break;
			}
			switch (e->idtype)
			{
			case IT::IDTYPE::V:
				strcat(type, " variable ");
				break;
			case IT::IDTYPE::F:
				strcat(type, " function ");
				break;
			case IT::IDTYPE::P:
				strcat(type, " parameter");
				break;
			case IT::IDTYPE::L:
				strcat(type, " literal  ");
				break;
			case IT::IDTYPE::S:
				strcat(type, " std func ");
				break;
			default:
				strcat(type, " UNDEFINED");
				break;
			}

			*stream << STR(i, e->idxfirstLE, type, e->id);

			// Вывод значений
			if (e->idtype == IT::IDTYPE::L || e->idtype == IT::IDTYPE::V)
			{
				if (e->iddatatype == IT::IDDATATYPE::INT)
				{
					// Для логических литералов выводим текстовое представление
					if (e->idtype == L && (e->value.vint == 0 || e->value.vint == 1))
						*stream << (e->value.vint ? "true" : "false");
					else
						*stream << e->value.vint;
				}
				else if (e->iddatatype == IT::IDDATATYPE::CHAR)
					*stream << "'" << e->value.vsymbol << "'";
				else if (e->iddatatype == IT::IDDATATYPE::UNDEF && e->idtype == L)
				{
					if (e->value.vint == 1)
						*stream << "true";
					else if (e->value.vint == 0)
						*stream << "false";
				}
			}

			// Вывод параметров функций
			if (e->idtype == IT::IDTYPE::F || e->idtype == IT::IDTYPE::S)
			{
				*stream << " Параметры: ";
				for (int j = 0; j < e->value.params.count; j++)
				{
					if (j > 0) *stream << ", ";
					switch (e->value.params.types[j])
					{
					case IT::IDDATATYPE::INT:
						*stream << "integer";
						break;
					case IT::IDDATATYPE::CHAR:
						*stream << "char";
						break;
					case IT::IDDATATYPE::UNDEF:
						*stream << "undefined";
						break;
					}
				}
			}
			*stream << std::endl;
		}
		*stream << "\n-------------------------------------------------------------------------\n\n";
	}
};