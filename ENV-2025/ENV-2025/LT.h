#pragma once
#include "pch.h"
#define	LEXEMA_FIXSIZE   1
#define	LT_MAXSIZE		 4096
#define	NULLDX_TI	 0xffffffff
#define LEX_SEPARATORS	 'S'
#define	LEX_ID_TYPE    	 't'
#define	LEX_STDFUNC    	 'p'
#define	LEX_ID			 'i'
#define	LEX_LITERAL		 'l'
#define	LEX_FUNCTION     'f'
#define	LEX_MAIN		 'm'
#define	LEX_SEPARATOR	 ';'
#define	LEX_COMMA		 ','
#define	LEX_LEFTBRACE	 '{'
#define	LEX_BRACELET	 '}'
#define	LEX_SQ_LBRACELET '['
#define	LEX_SQ_RBRACELET ']'
#define	LEX_LEFTHESIS	 '('
#define	LEX_RIGHTTHESIS	 ')'
#define	LEX_PLUS		 '+'
#define	LEX_MINUS		 '-'
#define	LEX_STAR		 '*'
#define	LEX_INCR		 ':'
#define LEX_DIRSLASH	 '/'
#define LEX_PROCENT		 '%'
#define	LEX_EQUAL		 '='
#define LEX_IS			 '?'
#define LEX_TILDA		 '~'
#define LEX_WHILE		 'c'
#define LEX_REPEAT		 'R'
#define LEX_TIMES		 'T'
#define LEX_ISTRUE		 'r'
#define LEX_ISFALSE		 'w'
#define LEX_DO			 'd'
#define LEX_WRITE		 'o'
#define LEX_NEWLINE		 '^'
#define LEX_RETURN		 'e'
#define LEX_VOID		 'g'
#define LEX_TYPE		 'n'
#define LEX_MORE		 '>'
#define LEX_LESS		 '<'
#define LEX_EQUALS		 '&'
#define LEX_SUBST		 '@'
#define LEX_NOTEQUALS	 '!'

namespace LT
{
	struct Entry
	{
		unsigned char lexema;
		int sn;
		int col;
		int idxTI;

		Entry();
		Entry(unsigned char lexema, int snn, int coln = 0, int idxti = NULLDX_TI);

	};

	struct LexTable
	{
		int maxsize;
		int size;
		Entry* table;
	};

	LexTable Create(
		int size
	);

	void Add(
		LexTable& lextable,
		Entry entry
	);

	Entry GetEntry(
		LexTable& lextable,
		int n
	);

	void Delete(LexTable& lextable);

	Entry writeEntry(
		Entry &entry,
		unsigned char lexema,
		int indx,
		int line,
		int col = 0
	);
	void showTable(LexTable lextable, Log::LOG &log);
	void writeLexTable(std::ostream *stream, LT::LexTable &lextable);
	void writeLexemsOnLines(std::ostream *stream, LT::LexTable &lextable);
};