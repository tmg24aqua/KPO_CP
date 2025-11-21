// Error.h - Система обработки ошибок
//
// Модуль обеспечивает централизованную обработку ошибок компилятора.
// Каждая ошибка имеет:
// - Уникальный идентификатор (id)
// - Текстовое сообщение
// - Позицию в исходном коде (строка, столбец) - для ошибок в исходном коде
//
// Использование:
// - ERROR_THROW(id) - выбросить ошибку по идентификатору
// - ERROR_THROW_IN(id, line, col) - выбросить ошибку с указанием позиции в исходном коде

#pragma once
#define ERROR_THROW(id) Error::geterror(id);
#define ERROR_THROW_IN(id,l,c) Error::geterrorin(id,l,c);
#define ERROR_ENTRY(id, m) {id,m,{-1,-1}}        // Элемент таблицы ошибок
#define ERROR_MAXSIZE_MESSAGE 200                // Максимальная длина сообщения об ошибке
#define ERROR_ENTRY_NODEF(id) ERROR_ENTRY(-id,"Неопределенная ошибка") // 1 неопределенный элемент таблицы ошибок
// ERROR_ENTRY_NODEF10(id) - 10 неопределенных элементов таблицы ошибок
#define ERROR_ENTRY_NODEF10(id) ERROR_ENTRY_NODEF(id+0),ERROR_ENTRY_NODEF(id+1),ERROR_ENTRY_NODEF(id+2),ERROR_ENTRY_NODEF(id+3),\
								ERROR_ENTRY_NODEF(id+4),ERROR_ENTRY_NODEF(id+5),ERROR_ENTRY_NODEF(id+6),ERROR_ENTRY_NODEF(id+7),\
								ERROR_ENTRY_NODEF(id+8),ERROR_ENTRY_NODEF(id+9)
// ERROR_ENTRY_NODEF10(id) - 100 неопределенных элементов таблицы ошибок
#define ERROR_ENTRY_NODEF100(id) ERROR_ENTRY_NODEF10(id+0),ERROR_ENTRY_NODEF10(id+10),ERROR_ENTRY_NODEF10(id+20),ERROR_ENTRY_NODEF10(id+30),\
								ERROR_ENTRY_NODEF10(id+40),ERROR_ENTRY_NODEF10(id+50),ERROR_ENTRY_NODEF10(id+60),ERROR_ENTRY_NODEF10(id+70),\
								ERROR_ENTRY_NODEF10(id+80),ERROR_ENTRY_NODEF10(id+90)
#define ERROR_MAX_ENTRY 1000       //количество элементов в таблице ошибок

namespace Error
{
	// Структура ошибки компилятора
	struct ERROR
	{
		int id;         // Код ошибки (уникальный идентификатор)
		char message[ERROR_MAXSIZE_MESSAGE];    // Текстовое сообщение об ошибке
		struct IN           // Позиция ошибки в исходном коде
		{
			short line;      // Номер строки (начиная с 1)
			short col;		// Номер позиции в строке (начиная с 0)
		}position;
	};

	// Сформировать структуру ERROR для ERROR_THROW (без указания позиции)
	ERROR geterror(int id);
	
	// Сформировать структуру ERROR для ERROR_THROW_IN (с указанием позиции в исходном коде)
	ERROR geterrorin(int id, int line, int col);
};






