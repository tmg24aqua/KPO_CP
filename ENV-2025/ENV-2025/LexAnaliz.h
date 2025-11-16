#pragma once
#include "pch.h"

#define TYPE_INTEGER	"integer"
#define TYPE_CHAR	    "char"
#define COMPARE_FUNC	"compare"
#define RANDOM_FUNC		"random"
#define MAIN_FUNC		"main"
#define WRITE_FUNC		"write"
#define REPEAT_KW		"repeat"
#define TIMES_KW		"times"
#define IS_KW		    "is"
#define TRUE_KW		    "true"
#define FALSE_KW		"false"
#define TYPE_KW		    "type"
#define RETURN_KW		"return"
#define END_KW		    "end"
#define DO_KW		    "do"

#define ISTYPE(str) ( !strcmp(str, TYPE_INTEGER) || !strcmp(str, TYPE_CHAR) )

namespace Lex
{
	struct LEX
	{
		LT::LexTable lextable;
		IT::IdTable	idtable;
		LEX() {}
	};

	struct Graph
	{
		char lexema;
		FST::FST graph;
	};

	IT::Entry* getEntry(	// формирует и возвращает строку ТИ
		Lex::LEX& tables,	// ТЛ + ТИ
		char lex,			// лексема
		char* id,			// идентификатор
		char* idtype,		// предыдущая (тип)
		bool isParam,		// признак параметра функции
		bool isFunc,		// признак функции
		Log::LOG log,		// протокол
		int line,			// строка в исходном тексте
		bool& rc_err		// флаг ошибки(по ссылке)
	);

	struct ERROR_S			// тип исключения для throw ERROR_THROW | ERROR_THROW_IN
	{
		int id;
		char message[ERROR_MAXSIZE_MESSAGE];	// сообщение			
		struct
		{
			short line = -1;	// номер строки (0, 1, 2, ...)
			short col = -1;		// номер позиции в строке(0, 1, 2, ...)
		} position;
	};

	bool analyze(LEX& tables, In::IN& in, Log::LOG& log, Parm::PARM& parm);
	int getIndexInLT(LT::LexTable& lextable, int itTableIndex);	// индекс первой встречи в таблице лексем

	// Новые функции для ENV-2025
	bool isStdFunction(const char* id);	// проверка на стандартную функцию
	bool isKeyword(const char* id);		// проверка на ключевое слово
	IT::IDDATATYPE getTypeFromString(const char* typeStr); // преобразование строки в тип данных
};