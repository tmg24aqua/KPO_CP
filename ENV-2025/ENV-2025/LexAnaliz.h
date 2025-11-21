// LexAnaliz.h - Ћексический анализатор
//
// Ћексический анализатор разбирает исходный код на токены (лексемы).
// »спользует конечные автоматы (FST) дл€ распознавани€ различных типов лексем.
//
// ќсновные функции:
// - analyze(): основной анализ исходного кода
// - getEntry(): создание записи в таблице идентификаторов
// - ѕроверка ключевых слов, стандартных функций, типов данных
// - –аспознавание литералов (числа, строки, символы, логические значени€)

#pragma once
#include "pch.h"

#define TYPE_INTEGER	"integer"	// “ип данных: целое число
#define TYPE_SYMBOL	    "symbol"	// “ип данных: символ
#define COMPARE_FUNC	"compare"
#define LENGTH_FUNC		"length"
#define MAIN_FUNC		"main"
#define WRITE_FUNC		"write"
#define WRITELINE_FUNC	"writeline"
#define REPEAT_KW		"repeat"
#define TIMES_KW		"times"
#define IS_KW		    "is"
#define TRUE_KW		    "true"
#define FALSE_KW		"false"
#define TYPE_KW		    "type"
#define RETURN_KW		"return"
#define END_KW		    "end"
#define FUNCTION_KW     "function"

#define ISTYPE(str) ( !strcmp(str, TYPE_INTEGER) || !strcmp(str, TYPE_SYMBOL) )

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

		Graph(char lex, FST::FST g) : lexema(lex), graph(g) {}
		Graph() = delete;
		Graph(const Graph&) = default;
		Graph& operator=(const Graph&) = default;
	};

	IT::Entry* getEntry(	// ШШШШШШШШШ Ш ШШШШШШШШШШ ШШШШШШ ШШ
		Lex::LEX& tables,	// ШШ + ШШ
		char lex,			// ШШШШШШШ
		char* id,			// ШШШШШШШШШШШШШ
		char* idtype,		// ШШШШШШШШШШ (ШШШ)
		bool isParam,		// ШШШШШШШ ШШШШШШШШШ ШШШШШШШ
		bool isFunc,		// ШШШШШШШ ШШШШШШШ
		Log::LOG log,		// ШШШШШШШШ
		int line,			// ШШШШШШ Ш ШШШШШШШШ ШШШШШШ
		bool& rc_err		// ШШШШ ШШШШШШ(ШШ ШШШШШШ)
	);

	struct ERROR_S			// ШШШ ШШШШШШШШШШ ШШШ throw ERROR_THROW | ERROR_THROW_IN
	{
		int id;
		char message[ERROR_MAXSIZE_MESSAGE];	// ШШШШШШШШШ			
		struct
		{
			short line = -1;	// ШШШШШ ШШШШШШ (0, 1, 2, ...)
			short col = -1;		// ШШШШШ ШШШШШШШ Ш ШШШШШШ(0, 1, 2, ...)
		} position;
	};

	bool analyze(LEX& tables, In::IN& in, Log::LOG& log, Parm::PARM& parm);
	int getIndexInLT(LT::LexTable& lextable, int itTableIndex);	// ШШШШШШ ШШШШШШ ШШШШШШШ Ш ШШШШШШШ ШШШШШШ

	// ШШШШШШШ ШШШ ENV-2025
	bool isStdFunction(const char* id);	// ШШШШШШШШ ШШ ШШШШШШШШШШШ ШШШШШШШ
	bool isKeyword(const char* id);		// ШШШШШШШШ ШШ ШШШШШШШШ ШШШШШ
	IT::IDDATATYPE getTypeFromString(const char* typeStr); // ШШШШШШШШШШШШШШ ШШШШШШ Ш ШШШ ШШШШШШ
	bool isLogicalLiteral(const char* id); // ШШШШШШШШ ШШ ШШШШШШШШШШ ШШШШШШШ
	bool isHexLiteral(const char* str);   // ШШШШШШШШ ШШ ШШШШШШШШШШШШШШШШШ ШШШШШШШ
	bool isBinaryLiteral(const char* str); // ШШШШШШШШ ШШ ШШШШШШШШ ШШШШШШШ
};