#pragma once
#include"pch.h"
#define ID_MAXSIZE	8					 // макс число символов идентификатора (8 по спецификации)
#define SCOPED_ID_MAXSIZE   (ID_MAXSIZE*2-1) // макс число символов идентификатор + область видимости
#define MAXSIZE_TI		4096			 // макс число количество строк в таблице идентификаторов
#define TI_INT_DEFAULT	0x00000000		 // значение по умолчанию для integer
#define TI_SYMBOL_DEFAULT	0x00		 // значение по умолчанию для symbol
#define TI_NULLIDX		0xffffffff		 // нет элемента таблицы идентификаторов
#define STR_MAXSIZE	255					 // максимальная длина строкового литерала
#define TI_INT_MAXSIZE   32767			 // максимальное значение для типа integer (2 байта)
#define TI_INT_MINSIZE  -32768			 // минимальное значение для типа integer (2 байта)
#define MAX_PARAMS_COUNT 5			   	 // максимальное количество параметров у функции
#define COMPARE_PARAMS_CNT 2			 // кол-во параметров у функции compare
#define LENGTH_PARAMS_CNT 1				 // кол-во параметров у функции length

namespace IT			// таблица идентификаторов
{
	enum IDDATATYPE { INT = 1, SYM = 2, STR = 3, PROC = 4, UNDEF = 5, HEX = 6, BIN = 7 };	// типы данных ENV-2025
	enum IDTYPE { V = 1, F = 2, P = 3, L = 4, S = 5 };			// типы идентификаторов: переменная, функция, параметр, литерал, стандартная функция
	enum STDFNC { F_COMPARE, F_LENGTH, F_NOT_STD };				// стандартные функции ENV-2025

	// Параметры стандартных функций
	static const IDDATATYPE COMPARE_PARAMS[] = { IT::SYM, IT::SYM };
	static const IDDATATYPE LENGTH_PARAMS[] = { IT::STR };

	struct Entry	// строка таблицы идентификаторов
	{
		int			idxfirstLE;			// индекс первой строки в таблице лексем
		char		id[SCOPED_ID_MAXSIZE];	// идентификатор (автоматически усекается до ID_MAXSIZE)
		IDDATATYPE	iddatatype;			// тип данных
		IDTYPE		idtype;				// тип идентификатора

		union
		{
			int vint;					// значение integer (2 байта)
			struct
			{
				int len;				// количество символов в строке
				char str[STR_MAXSIZE];	// символы строки
			} vstr;						// значение строкового литерала
			char symbol;				// значение символьного литерала
			struct
			{
				int count;				// количество параметров функции
				IDDATATYPE* types;		// типы параметров функции
			} params;
		} value;		// значение идентификатора

		Entry()							// конструктор без параметров
		{
			this->value.vint = TI_INT_DEFAULT;
			this->value.vstr.len = 0;
			this->value.params.count = 0;
			this->value.symbol = TI_SYMBOL_DEFAULT;
		};

		Entry(char* id, int idxLT, IDDATATYPE datatype, IDTYPE idtype) // конструктор с параметрами
		{
			strncpy_s(this->id, id, SCOPED_ID_MAXSIZE - 1);
			this->idxfirstLE = idxLT;
			this->iddatatype = datatype;
			this->idtype = idtype;
		};
	};

	struct IdTable				// экземпляр таблицы идентификаторов
	{
		int maxsize;			// емкость таблицы идентификаторов < TI_MAXSIZE
		int size;				// текущий размер таблицы идентификаторов < maxsize
		Entry* table;			// массив строк таблицы идентификаторов
	};

	IdTable Create(int size);	// создать таблицу идентификаторов

	void Add(					// добавить строку в таблицу идентификаторов
		IdTable& idtable,		// экземпляр таблицы идентификаторов
		Entry entry				// строка таблицы идентификаторов
	);

	int isId(					// возврат: номер строки (если есть), TI_NULLIDX (если нет)
		IdTable& idtable,		// экземпляр таблицы идентификаторов
		char id[SCOPED_ID_MAXSIZE]	// идентификатор
	);

	bool SetValue(IT::Entry* entry, char* value);	// задать значение идентификатора
	bool SetValue(IT::IdTable& idtable, int index, char* value);
	void writeIdTable(std::ostream* stream, IT::IdTable& idtable); // вывести таблицу идентификаторов
};