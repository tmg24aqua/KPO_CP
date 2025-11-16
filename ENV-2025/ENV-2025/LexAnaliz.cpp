#pragma once
#include "pch.h"
namespace Lex
{
	Graph graphs[N_GRAPHS] =
	{
		{ LEX_SEPARATORS, FST::FST(GRAPH_SEPARATORS) },
		{ LEX_ID, FST::FST(GRAPH_ID) },
		{ LEX_ID_TYPE, FST::FST(GRAPH_INTEGER) },
		{ LEX_ID_TYPE, FST::FST(GRAPH_CHAR) },
		{ LEX_STDFUNC, FST::FST(GRAPH_COMPARE) },
		{ LEX_STDFUNC, FST::FST(GRAPH_RANDOM) },
		{ LEX_LITERAL, FST::FST(GRAPH_INTEGER_LITERAL) },
		{ LEX_LITERAL, FST::FST(GRAPH_BINARY_LITERAL) },
		{ LEX_LITERAL, FST::FST(GRAPH_HEX_LITERAL) },
		{ LEX_LITERAL, FST::FST(GRAPH_STRING_LITERAL) },
		{ LEX_LITERAL, FST::FST(GRAPH_CHAR_LITERAL) },
		{ LEX_FUNCTION, FST::FST(GRAPH_FUNCTION) },
		{ LEX_MAIN, FST::FST(GRAPH_MAIN) },
		{ LEX_TYPE, FST::FST(GRAPH_TYPE) },
		{ LEX_RETURN, FST::FST(GRAPH_RETURN) },
		{ LEX_WRITE, FST::FST(GRAPH_WRITE) },
		{ LEX_REPEAT, FST::FST(GRAPH_REPEAT) },
		{ LEX_TIMES, FST::FST(GRAPH_TIMES) },
		{ LEX_IS, FST::FST(GRAPH_IS) },
		{ LEX_TRUE, FST::FST(GRAPH_TRUE) },
		{ LEX_FALSE, FST::FST(GRAPH_FALSE) },
		{ LEX_DO, FST::FST(GRAPH_DO) },
		{ LEX_END, FST::FST(GRAPH_END) }
	};

	char* getScopeName(IT::IdTable idtable, char* prevword)
	{
		char* a = new char[5];
		a = (char*)"main\0";
		if (strcmp(prevword, MAIN_FUNC) == 0)
			return a;
		for (int i = idtable.size - 1; i >= 0; i--)
			if (idtable.table[i].idtype == IT::IDTYPE::F || idtable.table[i].idtype == IT::IDTYPE::S)
				return idtable.table[i].id;
		return nullptr;
	}

	bool isLiteral(char* id)
	{
		if (isdigit(*id) || *id == IN_CODE_QUOTE || *id == '\'' || *id == 't' || *id == 'f' || *id == NULL)
			return true;
		return false;
	}

	IT::STDFNC getStandFunction(char* id)
	{
		if (!strcmp(COMPARE_FUNC, id))
			return IT::STDFNC::F_COMPARE;
		if (!strcmp(RANDOM_FUNC, id))
			return IT::STDFNC::F_RANDOM;
		return IT::STDFNC::F_NOT_STD;
	}

	int getLiteralIndex(IT::IdTable ittable, char* value, IT::IDDATATYPE type)
	{
		for (int i = 0; i < ittable.size; i++)
		{
			if (ittable.table[i].idtype == IT::IDTYPE::L && ittable.table[i].iddatatype == type)
			{
				switch (type)
				{
				case IT::IDDATATYPE::INT:
					if (ittable.table[i].value.vint == atoi(value))
						return i;
					break;
				case IT::IDDATATYPE::CHAR:
					if (ittable.table[i].value.vsymbol == value[1])
						return i;
					break;
					// Убрана обработка STR, так как его нет в IDDATATYPE
				}
			}
		}
		return TI_NULLIDX;
	}

	IT::IDDATATYPE getType(char* curword, char* idtype)
	{
		if (idtype == nullptr)
			return IT::IDDATATYPE::UNDEF;
		if (!strcmp(TYPE_CHAR, idtype))
			return IT::IDDATATYPE::CHAR;
		if (!strcmp(TYPE_INTEGER, idtype))
			return IT::IDDATATYPE::INT;
		if (isdigit(*curword) || (*curword == '0' && (curword[1] == 'x' || curword[1] == 'b')))
			return IT::IDDATATYPE::INT;
		else if (*curword == IN_CODE_QUOTE)
			return IT::IDDATATYPE::INT; // Строки как INT для упрощения
		else if (*curword == '\'')
			return IT::IDDATATYPE::CHAR;
		else if (!strcmp(curword, TRUE_KW) || !strcmp(curword, FALSE_KW))
			return IT::IDDATATYPE::INT;

		return IT::IDDATATYPE::UNDEF;
	}

	char* getNextLiteralName()
	{
		static int literalNumber = 1;
		char* buf = new char[SCOPED_ID_MAXSIZE];
		char num[3];
		strcpy_s(buf, ID_MAXSIZE, "L");
		_itoa_s(literalNumber++, num, 10);
		strcat(buf, num);
		return buf;
	}

	int getIndexInLT(LT::LexTable& lextable, int itTableIndex)
	{
		if (itTableIndex == TI_NULLIDX)
			return lextable.size;
		for (int i = 0; i < lextable.size; i++)
			if (itTableIndex == lextable.table[i].idxTI)
				return i;
		return TI_NULLIDX;
	}

	IT::Entry* getEntry(
		Lex::LEX& tables,
		char lex,
		char* id,
		char* idtype,
		bool isParam,
		bool isFunc,
		Log::LOG log,
		int line,
		bool& lex_ok)
	{
		IT::IDDATATYPE type = getType(id, idtype);
		int index = IT::isId(tables.idtable, id);
		if (lex == LEX_LITERAL)
			index = getLiteralIndex(tables.idtable, id, type);
		if (index != TI_NULLIDX)
			return nullptr;

		IT::Entry* itentry = new IT::Entry;
		itentry->iddatatype = type;
		itentry->idxfirstLE = getIndexInLT(tables.lextable, index);

		if (lex == LEX_LITERAL)
		{
			bool int_ok = IT::SetValue(itentry, id);
			if (!int_ok && itentry->iddatatype == IT::IDDATATYPE::INT)
			{
				Log::WriteError(log.stream, Error::geterrorin(313, line, 0));
				lex_ok = false;
			}
			// Убрана проверка для STR
			strcpy_s(itentry->id, getNextLiteralName());
			itentry->idtype = IT::IDTYPE::L;
		}
		else
		{
			switch (type)
			{
			case IT::IDDATATYPE::INT:
				itentry->value.vint = TI_INT_DEFAULT;
				break;
			case IT::IDDATATYPE::CHAR:
				itentry->value.vsymbol = TI_CHAR_DEFAULT;
				break;
			}

			if (isFunc)
			{
				switch (getStandFunction(id))
				{
				case IT::STDFNC::F_RANDOM:
					itentry->idtype = IT::IDTYPE::S;
					itentry->iddatatype = IT::IDDATATYPE::INT;
					itentry->value.params.count = RANDOM_PARAMS_CNT;
					itentry->value.params.types = new IT::IDDATATYPE[RANDOM_PARAMS_CNT];
					for (int k = 0; k < RANDOM_PARAMS_CNT; k++)
						itentry->value.params.types[k] = IT::RANDOM_PARAMS[k];
					break;
				case IT::STDFNC::F_COMPARE:
					itentry->idtype = IT::IDTYPE::S;
					itentry->iddatatype = IT::IDDATATYPE::INT;
					itentry->value.params.count = COMPARE_PARAMS_CNT;
					itentry->value.params.types = new IT::IDDATATYPE[COMPARE_PARAMS_CNT];
					for (int k = 0; k < COMPARE_PARAMS_CNT; k++)
						itentry->value.params.types[k] = IT::COMPARE_PARAMS[k];
					break;
				case IT::STDFNC::F_NOT_STD:
					itentry->idtype = IT::IDTYPE::F;
					break;
				}
			}
			else if (isParam)
				itentry->idtype = IT::IDTYPE::P;
			else
				itentry->idtype = IT::IDTYPE::V;

			memset(itentry->id, '\0', SCOPED_ID_MAXSIZE);
			strncat(itentry->id, id, SCOPED_ID_MAXSIZE);
		}

		// Проверки ошибок
		int i = tables.lextable.size;
		if (i > 1 && itentry->idtype == IT::IDTYPE::V && tables.lextable.table[i - 2].lexema != LEX_TYPE)
		{
			Log::WriteError(log.stream, Error::geterrorin(304, line, 0));
			lex_ok = false;
		}
		if (i > 1 && itentry->idtype == IT::IDTYPE::F && tables.lextable.table[i - 1].lexema != LEX_FUNCTION)
		{
			Log::WriteError(log.stream, Error::geterrorin(303, line, 0));
			lex_ok = false;
		}
		if (itentry->iddatatype == IT::IDDATATYPE::UNDEF)
		{
			Log::WriteError(log.stream, Error::geterrorin(300, line, 0));
			lex_ok = false;
		}
		return itentry;
	}

	bool analyze(LEX& tables, In::IN& in, Log::LOG& log, Parm::PARM& parm)
	{
		static bool lex_ok = true;
		tables.lextable = LT::Create(LT_MAXSIZE);
		tables.idtable = IT::Create(MAXSIZE_TI);
		bool isParam = false, isFunc = false;
		int enterPoint = 0;
		char curword[STR_MAXSIZE], nextword[STR_MAXSIZE];
		int curline;
		std::stack <char*> scopes;

		for (int i = 0; i < in.words->size; i++)
		{
			strcpy_s(curword, in.words[i].word);
			if (i < in.words->size - 1)
				strcpy_s(nextword, in.words[i + 1].word);
			curline = in.words[i].line;
			isFunc = false;
			int idxTI = TI_NULLIDX;

			for (int j = 0; j < N_GRAPHS; j++)
			{
				FST::FST fst((unsigned char*)curword, graphs[j].graph);
				if (FST::execute(fst))
				{
					char lexema = graphs[j].lexema;

					// Обработка основных лексем
					switch (lexema)
					{
					case LEX_MAIN:
						enterPoint++;
						break;
					case LEX_SEPARATORS:
						lexema = *curword; // Используем сам символ как лексему
						break;
					case LEX_ID:
					case LEX_STDFUNC:
					case LEX_LITERAL:
					{
						char id[STR_MAXSIZE] = "";
						idxTI = TI_NULLIDX;

						if (*nextword == '(' || IT::isId(tables.idtable, curword) != TI_NULLIDX)
						{
							isFunc = true;
						}

						char* idtype = (isFunc && i > 1) ? in.words[i - 2].word : in.words[i - 1].word;
						if (i == 0)
							idtype = nullptr;

						if (!isFunc && !scopes.empty())
							strncpy_s(id, scopes.top(), ID_MAXSIZE);
						strncat(id, curword, ID_MAXSIZE);

						if (isLiteral(curword))
							strcpy_s(id, curword);

						IT::Entry* itentry = getEntry(tables, lexema, id, idtype, isParam, isFunc, log, curline, lex_ok);
						if (itentry != nullptr)
						{
							IT::Add(tables.idtable, *itentry);
							idxTI = tables.idtable.size - 1;
						}
						else
						{
							idxTI = IT::isId(tables.idtable, id);
						}
					}
					break;
					}

					LT::Entry* ltentry = new LT::Entry(lexema, curline, idxTI);
					LT::Add(tables.lextable, *ltentry);
					break;
				}
				else if (j == N_GRAPHS - 1)
				{
					Log::WriteError(log.stream, Error::geterrorin(201, curline, 0));
					lex_ok = false;
				}
			}
		}

		if (enterPoint == 0)
		{
			Log::WriteError(log.stream, Error::geterror(301));
			lex_ok = false;
		}
		if (enterPoint > 1)
		{
			Log::WriteError(log.stream, Error::geterror(302));
			lex_ok = false;
		}
		return lex_ok;
	}
}