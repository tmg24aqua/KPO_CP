// Log.h - Система логирования работы компилятора
//
// Модуль обеспечивает запись информации о работе компилятора в файл протокола.
// В протокол записывается:
// - Заголовок с информацией о компиляторе
// - Параметры командной строки
// - Информация о входном файле
// - Результаты каждого этапа компиляции
// - Ошибки компиляции с указанием позиции в исходном коде
// - Таблицы лексем и идентификаторов

#pragma once
#include <fstream>
#include "In.h"
#include "Parm.h"
#include "Error.h"
namespace Log // Работа с протоколом
{
	// Структура протокола работы компилятора
	struct LOG
	{
		wchar_t logfile[PARM_MAX_SIZE]; // Имя файла протокола
		std::ofstream* stream; // Выходной поток протокола
	};

	static const LOG INITLOG = { L"",NULL };			 // Структура для начальной инициализации LOG
	LOG getlog(wchar_t logfile[]);					 // Сформировать структуру LOG и открыть файл протокола
	void WriteLine(LOG log, const char* c, ...);	     // Вывести в протокол конкатенацию строк
	void WriteLine(LOG, const wchar_t* c, ...);      // Вывести в протокол конкатенацию строк (wide char)
	void WriteLineConsole(char* c, ...);				// Вывести в консоль конкатенацию строк
	void WriteLog(LOG log);							 // Вывести в протокол заголовок
	void WriteParm(LOG log, Parm::PARM parm);		 // Вывести в протокол информацию о входных параметрах
	void WriteIn(LOG log, In::IN in);				 // Вывести в протокол информацию о входном потоке
	void WriteError(std::ostream* stream, Error::ERROR e);// Вывести в протокол информацию об ошибке
	void Close(LOG log);							 // Закрыть протокол (закрыть файл)
}