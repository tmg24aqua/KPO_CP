#pragma once
#include "pch.h"
#define SEPSTREMP  "\n;------------------------------\n"
#define SEPSTR(x)  "\n;----------- " + string(x) + " ------------\n"

#define BEGIN ".586\n"\
".model flat, stdcall\n"\
"includelib libucrt.lib\n"\
"includelib kernel32.lib\n"\
"includelib \"ENV2025lib.lib\"\n"\
"ExitProcess PROTO:DWORD \n"\
".stack 4096\n"

#define END "\ncall system_pause"\
			"\npush 0"\
			"\ncall ExitProcess"\
			"\nSOMETHINGWRONG:"\
			"\npush offset null_division"\
			"\ncall outstrline\n"\
			"\tcall system_pause"\
			"\npush -1"\
			"\ncall ExitProcess"\
			"\nEXIT_OVERFLOW:"\
			"\npush offset overflow"\
			"\ncall outstrline\n"\
			"\tcall system_pause"\
			"\npush -2"\
			"\ncall ExitProcess"\
			"\nmain ENDP\nend main"

#define EXTERN "\n; --- ˜˜˜˜˜˜˜˜˜˜˜ ˜˜˜˜˜˜˜ ENV-2025 ---\n"\
"outnum PROTO : DWORD\n"\
"outstr PROTO : DWORD\n"\
"outstrline PROTO : DWORD\n"\
"outnumline PROTO : DWORD\n"\
"system_pause PROTO \n"\
"compare PROTO : DWORD, : DWORD\n"\
"length PROTO : DWORD\n"

#define ITENTRY(x)  lex.idtable.table[lex.lextable.table[x].idxTI]
#define LEXEMA(x)   lex.lextable.table[x].lexema

namespace Gener
{
	bool CodeGeneration(Lex::LEX& lex, Parm::PARM& parm, Log::LOG& log);
};