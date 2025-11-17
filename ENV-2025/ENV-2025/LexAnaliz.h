#pragma once
#include "pch.h"

#define TYPE_INTEGER	"integer"
#define TYPE_SYMBOL	    "symbol"
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

	IT::Entry* getEntry(	// ˜˜˜˜˜˜˜˜˜ ˜ ˜˜˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜ ˜˜
		Lex::LEX& tables,	// ˜˜ + ˜˜
		char lex,			// ˜˜˜˜˜˜˜
		char* id,			// ˜˜˜˜˜˜˜˜˜˜˜˜˜
		char* idtype,		// ˜˜˜˜˜˜˜˜˜˜ (˜˜˜)
		bool isParam,		// ˜˜˜˜˜˜˜ ˜˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜˜
		bool isFunc,		// ˜˜˜˜˜˜˜ ˜˜˜˜˜˜˜
		Log::LOG log,		// ˜˜˜˜˜˜˜˜
		int line,			// ˜˜˜˜˜˜ ˜ ˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜
		bool& rc_err		// ˜˜˜˜ ˜˜˜˜˜˜(˜˜ ˜˜˜˜˜˜)
	);

	struct ERROR_S			// ˜˜˜ ˜˜˜˜˜˜˜˜˜˜ ˜˜˜ throw ERROR_THROW | ERROR_THROW_IN
	{
		int id;
		char message[ERROR_MAXSIZE_MESSAGE];	// ˜˜˜˜˜˜˜˜˜			
		struct
		{
			short line = -1;	// ˜˜˜˜˜ ˜˜˜˜˜˜ (0, 1, 2, ...)
			short col = -1;		// ˜˜˜˜˜ ˜˜˜˜˜˜˜ ˜ ˜˜˜˜˜˜(0, 1, 2, ...)
		} position;
	};

	bool analyze(LEX& tables, In::IN& in, Log::LOG& log, Parm::PARM& parm);
	int getIndexInLT(LT::LexTable& lextable, int itTableIndex);	// ˜˜˜˜˜˜ ˜˜˜˜˜˜ ˜˜˜˜˜˜˜ ˜ ˜˜˜˜˜˜˜ ˜˜˜˜˜˜

	// ˜˜˜˜˜˜˜ ˜˜˜ ENV-2025
	bool isStdFunction(const char* id);	// ˜˜˜˜˜˜˜˜ ˜˜ ˜˜˜˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜˜
	bool isKeyword(const char* id);		// ˜˜˜˜˜˜˜˜ ˜˜ ˜˜˜˜˜˜˜˜ ˜˜˜˜˜
	IT::IDDATATYPE getTypeFromString(const char* typeStr); // ˜˜˜˜˜˜˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜ ˜ ˜˜˜ ˜˜˜˜˜˜
	bool isLogicalLiteral(const char* id); // ˜˜˜˜˜˜˜˜ ˜˜ ˜˜˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜˜
	bool isHexLiteral(const char* str);   // ˜˜˜˜˜˜˜˜ ˜˜ ˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜˜
	bool isBinaryLiteral(const char* str); // ˜˜˜˜˜˜˜˜ ˜˜ ˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜˜
};