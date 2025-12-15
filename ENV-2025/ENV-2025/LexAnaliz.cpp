#pragma once
#include "pch.h"
namespace Lex
{
	Graph graphs[N_GRAPHS] =
	{
		{ LEX_SEPARATORS, FST::FST(GRAPH_SEPARATORS) },
		{ LEX_SEPARATORS, FST::FST(GRAPH_INKR) },
		{ LEX_ID_TYPE, FST::FST(GRAPH_INTEGER) },
		{ LEX_VOID, FST::FST(GRAPH_VOID) },
		{ LEX_STDFUNC, FST::FST(GRAPH_POW) },
		{ LEX_STDFUNC, FST::FST(GRAPH_RANDOM) },
		{ LEX_STDFUNC, FST::FST(GRAPH_LENGTH) },
		{ LEX_STDFUNC, FST::FST(GRAPH_COMPARE) },
		{ LEX_LITERAL, FST::FST(GRAPH_HEX_BIN_LITERAL) },
		{ LEX_LITERAL, FST::FST(GRAPH_INT_LITERAL) },
		{ LEX_LITERAL, FST::FST(GRAPH_STRING_LITERAL) },
		{ LEX_LITERAL, FST::FST(GRAPH_SYMBOL_LITERAL) },
		{ LEX_LITERAL, FST::FST(GRAPH_INT_NEGATIVE) },
		{ LEX_ISTRUE, FST::FST(GRAPH_ISTRUE) },
		{ LEX_ISFALSE, FST::FST(GRAPH_ISFALSE) },
		{ LEX_TYPE, FST::FST(GRAPH_TYPE) },
		{ LEX_MAIN, FST::FST(GRAPH_MAIN) },
		{ LEX_ID_TYPE, FST::FST(GRAPH_STRING) },
		{ LEX_FUNCTION, FST::FST(GRAPH_FUNCTION) },
		{ LEX_ID_TYPE, FST::FST(GRAPH_SYMBOL) },
		{ LEX_RETURN, FST::FST(GRAPH_RETURN) },
		{ LEX_WRITE, FST::FST(GRAPH_WRITE) },
		{ LEX_NEWLINE, FST::FST(GRAPH_WRITELINE) },
		{ LEX_IS, FST::FST(GRAPH_IS) },
		{ LEX_WHILE, FST::FST(GRAPH_WHILE) },
		{ LEX_REPEAT, FST::FST(GRAPH_REPEAT) },
		{ LEX_TIMES, FST::FST(GRAPH_TIMES) },
		{ LEX_DO, FST::FST(GRAPH_DO) },
		{ LEX_LITERAL, FST::FST(GRAPH_BOOL_LITERAL) },
		{ LEX_ID, FST::FST(GRAPH_ID) }
	};

	const char* getScopeName(IT::IdTable idtable, const char* prevword)
	{
		if (prevword == nullptr || *prevword == '\0')
			return nullptr;

		if (strcmp(prevword, MAIN) == 0)
			return "main";
		for (int i = idtable.size - 1; i >= 0; i--)
			if (idtable.table[i].idtype == IT::IDTYPE::F || idtable.table[i].idtype == IT::IDTYPE::S)
				return idtable.table[i].id;
		return nullptr;
	}

	bool isLiteral(const char* id)
	{
		if (isdigit(static_cast<unsigned char>(*id)) || *id == IN_CODE_QUOTE || *id == LEX_MINUS || *id == IN_CODE_TILDA || *id == '\0' || *id == IN_CODE_APOST)
			return true;
		return false;
	}

	short parseBoolLiteral(const char* value)
	{
		char lower[256] = { 0 };
		for (int i = 0; value[i] != '\0' && i < 255; i++)
			lower[i] = tolower(static_cast<unsigned char>(value[i]));

		if (strstr(lower, "true") || strstr(lower, "yes") || strstr(lower, "1"))
			return 1;
		if (strstr(lower, "false") ||  strstr(lower, "no") || strstr(lower, "0"))
			return 0;
		return 0;
	}

	IT::STDFNC getStandFunction(const char* id)
	{
		if (!strcmp(RANDOM, id))
			return IT::STDFNC::F_RANDOM;
		if (!strcmp(POW, id))
			return IT::STDFNC::F_POW;
		if (!strcmp(LENGHT, id))
			return IT::STDFNC::F_LENGTH;
		if (!strcmp(COMPARE, id))
			return IT::STDFNC::F_COMPARE;
		return IT::STDFNC::F_NOT_STD;
	}

	int getLiteralIndex(IT::IdTable ittable, const char* value, IT::IDDATATYPE type)
	{
		for (int i = 0; i < ittable.size; i++)
		{
			if (ittable.table[i].idtype == IT::IDTYPE::L && ittable.table[i].iddatatype == type)
			{
				switch (type)
				{
				case IT::IDDATATYPE::INT:
				{
					short temp;
					const char* parseValue = value;
					char* tempBuf = nullptr;
					bool hasQuotes = (value[0] == IN_CODE_APOST && value[strlen(value) - 1] == IN_CODE_APOST);

					if (hasQuotes && strlen(value) > 2)
					{
						size_t bufSize = strlen(value) - 1;
						tempBuf = new char[bufSize];
						strncpy_s(tempBuf, bufSize, value + 1, strlen(value) - 2);
						tempBuf[strlen(value) - 2] = '\0';
						parseValue = tempBuf;
					}

					if ((parseValue[0] == '-' && (parseValue[1] == '0' && (parseValue[2] == 'x' || parseValue[2] == 'X'))) || (parseValue[0] == '0' && (parseValue[1] == 'x' || parseValue[1] == 'X')))
						temp = (short)strtol(parseValue, NULL, 16);
					else if ((parseValue[0] == '-' && parseValue[1] == '0' && (parseValue[2] == 'b' || parseValue[2] == 'B')) || (parseValue[0] == '0' && (parseValue[1] == 'b' || parseValue[1] == 'B')))
					{
						const char* binStr = parseValue;
						if (parseValue[0] == '-')
							binStr = parseValue + 3;
						else
							binStr = parseValue + 2;
						temp = 0;
						for (int j = 0; binStr[j] != '\0'; j++)
						{
							if (binStr[j] == '0' || binStr[j] == '1')
								temp = (temp << 1) | (binStr[j] - '0');
						}
						if (parseValue[0] == '-')
							temp = -temp;
					}
					else if ((parseValue[0] == '-' && parseValue[1] == '0') || parseValue[0] == '0')
						temp = (short)strtol(parseValue, NULL, 8);
					else if (isalpha(parseValue[0]))
					{
						char lower[256] = { 0 };
						for (int j = 0; parseValue[j] != '\0' && j < 255; j++)
							lower[j] = tolower(parseValue[j]);
						lower[255] = '\0';
						if (strstr(lower, "true") ||
							strstr(lower, "yes") || strstr(lower, "1"))
							temp = 1;
						else if (strstr(lower, "false") ||
							strstr(lower, "no") || strstr(lower, "0"))
							temp = 0;
						else
							temp = (short)atoi(parseValue);
					}
					else
						temp = (short)atoi(parseValue);

					if (tempBuf != nullptr)
					{
						delete[] tempBuf;
					}

					if (ittable.table[i].value.vint == temp)
						return i;
					break;
				}
				case IT::IDDATATYPE::STR:
				{
					if (strlen(value) < 2) break;
					char buf[STR_MAXSIZE] = { 0 };
					size_t copy_len = strlen(value) - 2;
					if (copy_len >= STR_MAXSIZE)
						copy_len = STR_MAXSIZE - 1;
					if (copy_len > 0)
					{
						memcpy(buf, value + 1, copy_len);
					}
					buf[copy_len] = '\0';
					if (strcmp((char*)ittable.table[i].value.vstr.str, buf) == 0)
						return i;
					break;
				}
				case IT::IDDATATYPE::SYM:
					if (ittable.table[i].value.symbol == value[1])
						return i;
					break;
				}
			}
		}
		return TI_NULLIDX;
	}

	IT::IDDATATYPE getType(const char* curword, const char* idtype)
	{
		if (idtype == nullptr)
			return IT::IDDATATYPE::UNDEF;
		if (!strcmp(TYPE_SYMBOL, idtype))
			return IT::IDDATATYPE::SYM;
		if (!strcmp(TYPE_VOID, idtype))
			return IT::IDDATATYPE::PROC;
		if (!strcmp(TYPE_STRING, idtype))
			return IT::IDDATATYPE::STR;
		if (!strcmp(TYPE_INTEGER, idtype))
			return IT::IDDATATYPE::INT;
		if (curword && (isdigit(static_cast<unsigned char>(*curword)) || *curword == LEX_MINUS || *curword == IN_CODE_APOST || (*curword == '0' && (curword[1] == 'x' || curword[1] == 'b')) ||
			(*curword == IN_CODE_APOST && curword[1] == '0' && (curword[2] == 'x' || curword[2] == 'X' || curword[2] == 'b' || curword[2] == 'B')) ||
			(*curword == IN_CODE_APOST && curword[1] == '-' && curword[2] == '0' && (curword[3] == 'x' || curword[3] == 'X' || curword[3] == 'b' || curword[3] == 'B'))))
			return IT::IDDATATYPE::INT;
		else if (curword && *curword == IN_CODE_QUOTE)
			return IT::IDDATATYPE::STR;
		else if (curword && *curword == IN_CODE_TILDA)
			return IT::IDDATATYPE::SYM;
		else if (curword && isalpha(static_cast<unsigned char>(*curword)))
		{
			char lower[256] = { 0 };
			for (int i = 0; curword[i] != '\0' && i < 255; i++)
				lower[i] = tolower(static_cast<unsigned char>(curword[i]));

			if (strstr(lower, "true") || strstr(lower, "false") ||
				strstr(lower, "yes") || strstr(lower, "no")
				)
				return IT::IDDATATYPE::INT;
		}

		return IT::IDDATATYPE::UNDEF;
	}

	char* getNextLiteralName()
	{
		static int literalNumber = 1;
		char* buf = new char[SCOPED_ID_MAXSIZE];
		snprintf(buf, SCOPED_ID_MAXSIZE, "L%d", literalNumber++);
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

	bool isKeyword(const char* word)
	{
		char lower[256] = { 0 };
		for (int i = 0; word[i] != '\0' && i < 255; i++)
			lower[i] = tolower(static_cast<unsigned char>(word[i]));

		const char* keywords[] = { "type", "integer", "string", "symbol", "function", "main", "return",
			"write", "writeline", "is", "while", "repeat", "times", "do", "pow", "random", "lenght", "compare", "void" };
		for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
		{
			if (strcmp(lower, keywords[i]) == 0)
				return true;
		}
		return false;
	}

	IT::Entry* getEntry(
		Lex::LEX& tables,
		char lex,
		const char* id,
		const char* idtype,
		bool isParam,
		bool isFunc,
		Log::LOG log,
		int line,
		int col,
		bool& lex_ok,
		const char* originalWord = nullptr)
	{
		IT::IDDATATYPE type = getType(id, idtype);
		int index = IT::isId(tables.idtable, id);
		if (lex == LEX_LITERAL)
			index = getLiteralIndex(tables.idtable, id, type);
		if (index != TI_NULLIDX)
			return nullptr;

		std::unique_ptr<IT::Entry> itentry = std::make_unique<IT::Entry>();
		itentry->iddatatype = type;
		itentry->idxfirstLE = getIndexInLT(tables.lextable, index);

		if (lex == LEX_LITERAL)
		{
			bool int_ok = IT::SetValue(itentry.get(), id);
			if (int_ok && itentry->iddatatype == IT::IDDATATYPE::INT)
			{
				char p[11];
				_itoa_s(itentry->value.vint, p, 10);
				index = getLiteralIndex(tables.idtable, p, type);
				if (index != TI_NULLIDX)
					return nullptr;
			}
			if (!int_ok)
			{
				Log::WriteError(log.stream, Error::geterrorin(208, line, col));
				lex_ok = false;
			}
			if (itentry->iddatatype == IT::IDDATATYPE::STR && itentry->value.vstr.len == 0)
			{
				Log::WriteError(log.stream, Error::geterrorin(310, line, col));
				lex_ok = false;
			}
			strcpy_s(itentry->id, SCOPED_ID_MAXSIZE, getNextLiteralName());
			itentry->idtype = IT::IDTYPE::L;
		}
		else
		{
			switch (type)
			{
			case IT::IDDATATYPE::STR:
				itentry->value.vstr.str[0] = '\0';
				itentry->value.vstr.len = TI_STR_DEFAULT;
				break;
			case IT::IDDATATYPE::INT:
				itentry->value.vint = TI_INT_DEFAULT;
				break;
			case IT::IDDATATYPE::SYM:
				itentry->value.symbol = TI_SYM_DEFAULT;
				break;
			}

			if (isFunc)
			{
				switch (getStandFunction(id))
				{
				case IT::STDFNC::F_RANDOM:
				{
					itentry->idtype = IT::IDTYPE::S;
					itentry->iddatatype = RANDOM_TYPE;
					itentry->value.params.count = RANDOM_PARAMS_CNT;
					itentry->value.params.types = new IT::IDDATATYPE[RANDOM_PARAMS_CNT];
					for (int k = 0; k < RANDOM_PARAMS_CNT; k++)
						itentry->value.params.types[k] = IT::RANDOM_PARAMS[k];
					break;
				}
				case IT::STDFNC::F_POW:
				{
					itentry->idtype = IT::IDTYPE::S;
					itentry->iddatatype = POW_TYPE;
					itentry->value.params.count = POW_PARAMS_CNT;
					itentry->value.params.types = new IT::IDDATATYPE[POW_PARAMS_CNT];
					for (int k = 0; k < POW_PARAMS_CNT; k++)
						itentry->value.params.types[k] = IT::POW_PARAMS[k];
					break;
				}
				case IT::STDFNC::F_LENGTH:
				{
					itentry->idtype = IT::IDTYPE::S;
					itentry->iddatatype = LENGHT_TYPE;
					itentry->value.params.count = LENGHT_PARAMS_CNT;
					itentry->value.params.types = new IT::IDDATATYPE[LENGHT_PARAMS_CNT];
					for (int k = 0; k < LENGHT_PARAMS_CNT; k++)
						itentry->value.params.types[k] = IT::LENGHT_PARAMS[k];
					break;
				}
				case IT::STDFNC::F_COMPARE:
				{
					itentry->idtype = IT::IDTYPE::S;
					itentry->iddatatype = COMPARE_TYPE;
					itentry->value.params.count = COMPARE_PARAMS_CNT;
					itentry->value.params.types = new IT::IDDATATYPE[COMPARE_PARAMS_CNT];
					for (int k = 0; k < COMPARE_PARAMS_CNT; k++)
						itentry->value.params.types[k] = IT::COMPARE_PARAMS[k];
					break;
				}
				case IT::STDFNC::F_NOT_STD:
					itentry->idtype = IT::IDTYPE::F;
					break;
				}
			}
			else if (isParam)
				itentry->idtype = IT::IDTYPE::P;
			else
				itentry->idtype = IT::IDTYPE::V;

			if (itentry->idtype == IT::IDTYPE::S && getStandFunction(id) == IT::STDFNC::F_POW)
			{
				char tempId[SCOPED_ID_MAXSIZE] = { 0 };
				strcpy_s(tempId, SCOPED_ID_MAXSIZE, id);
				strcat_s(tempId, SCOPED_ID_MAXSIZE, "er");
				strcpy_s(itentry->id, SCOPED_ID_MAXSIZE, tempId);
			}
			else
			{
				strcpy_s(itentry->id, SCOPED_ID_MAXSIZE, id);
			}
		}

		int i = tables.lextable.size;

		if (i > 1 && itentry->idtype == IT::IDTYPE::V && tables.lextable.table[i - 2].lexema != LEX_TYPE)
		{
			bool isUsage = false;
			if (i > 0)
			{
				char prevLex = tables.lextable.table[i - 1].lexema;
				if (prevLex == LEX_WRITE || prevLex == LEX_NEWLINE ||
					prevLex == LEX_PLUS || prevLex == LEX_MINUS || prevLex == LEX_STAR ||
					prevLex == LEX_DIRSLASH || prevLex == LEX_PROCENT || prevLex == LEX_EQUAL ||
					prevLex == LEX_LEFTHESIS || prevLex == LEX_COMMA)
				{
					isUsage = true;
				}
			}

			if (!isUsage)
			{
				Log::WriteError(log.stream, Error::geterrorin(209, line, col));
				lex_ok = false;
			}
		}
		if (i > 1 && itentry->idtype == IT::IDTYPE::F && tables.lextable.table[i - 1].lexema != LEX_FUNCTION)
		{
			Log::WriteError(log.stream, Error::geterrorin(303, line, col));
			lex_ok = false;
		}


		return itentry.release();
	}

	bool analyze(LEX& tables, In::IN& in, Log::LOG& log, Parm::PARM& parm)
	{
		bool lex_ok = true;
		try
		{
			tables.lextable = LT::Create(LT_MAXSIZE);
			tables.idtable = IT::Create(MAXSIZE_TI);
		}
		catch (Error::ERROR e)
		{
			Log::WriteError(log.stream, e);
			lex_ok = false;
			return lex_ok;
		}
		bool isParam = false, isFunc = false;
		int enterPoint = 0;
		char curword[STR_MAXSIZE] = { 0 }, nextword[STR_MAXSIZE] = { 0 };
		int curline;
		std::stack<std::string> scopes;

		for (int i = 0; i < in.words->size && lex_ok; i++)
		{
			if (in.words[i].word != nullptr)
			{
				size_t word_len = strlen(in.words[i].word);
				if (word_len >= STR_MAXSIZE)
					word_len = STR_MAXSIZE - 1;
				if (word_len > 0)
				{
					memcpy(curword, in.words[i].word, word_len);
				}
				curword[word_len] = '\0';
			}
			if (i < in.words->size - 1 && in.words[i + 1].word != nullptr)
			{
				size_t word_len = strlen(in.words[i + 1].word);
				if (word_len >= STR_MAXSIZE)
					word_len = STR_MAXSIZE - 1;
				if (word_len > 0)
				{
					memcpy(nextword, in.words[i + 1].word, word_len);
				}
				nextword[word_len] = '\0';
			}
			curline = in.words[i].line;
			int curcol = in.words[i].col;
			isFunc = false;
			int idxTI = TI_NULLIDX;
			bool wordRecognized = false;

			for (int j = 0; j < N_GRAPHS && lex_ok; j++)
			{
				FST::FST fst((unsigned char*)curword, graphs[j].graph);
				if (FST::execute(fst))
				{
					wordRecognized = true;
					char lexema = graphs[j].lexema;


					if ((lexema == LEX_ISTRUE || lexema == LEX_ISFALSE) && i > 0 && tables.lextable.size > 0)
					{
						int lastIdx = tables.lextable.size - 1;
						if (tables.lextable.table[lastIdx].lexema == LEX_EQUAL ||
							tables.lextable.table[lastIdx].lexema == LEX_SEPARATOR ||
							(lastIdx > 0 && tables.lextable.table[lastIdx - 1].lexema == LEX_EQUAL))
						{
							lexema = LEX_LITERAL;
						}
					}
					if (lexema == LEX_ID && i > 0 && tables.lextable.size > 0)
					{
						int lastIdx = tables.lextable.size - 1;
						if (tables.lextable.table[lastIdx].lexema == LEX_EQUAL)
						{
							char lower[256] = { 0 };
							for (int k = 0; curword[k] != '\0' && k < 255; k++)
								lower[k] = tolower(static_cast<unsigned char>(curword[k]));

							if (strcmp(lower, "yes") == 0 || strcmp(lower, "no") == 0 ||
								strcmp(lower, "true") == 0 || strcmp(lower, "false") == 0 ||
								strcmp(lower, "on") == 0 || strcmp(lower, "off") == 0)
							{
								lexema = LEX_LITERAL;
							}
						}
					}
					switch (lexema)
					{
					case LEX_MAIN:
						enterPoint++;
						if (i > 0 && tables.lextable.size > 0)
						{
							int lastIdx = tables.lextable.size - 1;
							if (lastIdx >= 0 && tables.lextable.table[lastIdx].lexema == LEX_MAIN)
							{
								Log::WriteError(log.stream, Error::geterrorin(600, curline, curcol));
								lex_ok = false;
							}
						}
						break;
					case LEX_SEPARATORS:
					{
						if (curword != nullptr && curword[0] == '@' && strlen(curword) == 1)
						{
							lexema = LEX_SUBST;
						}

						switch (*curword)
						{
						case LEX_LEFTHESIS:
						{
							isParam = true;

							if (*nextword == LEX_RIGHTTHESIS || ISTYPE(nextword))
							{
								const char* scopename = getScopeName(tables.idtable, in.words[i - 1].word);
								if (scopename == nullptr)
									break;
								scopes.push(std::string(scopename));
							}
							break;
						}
						case LEX_COMMA:
						{
							break;
						}
						case LEX_RIGHTTHESIS:
						{
							isParam = false;
							if (*in.words[i - 1].word == LEX_LEFTHESIS || (i > 2 && (tables.lextable.table[tables.lextable.size - 2].lexema == LEX_ID_TYPE)))
								if (!scopes.empty())
									scopes.pop();
							break;
						}
						case LEX_EQUAL:
						{
							break;
						}
						case LEX_SEPARATOR:
						{
							break;
						}
						case LEX_SUBST:
						{
							break;
						}
						case LEX_LEFTBRACE:
						{
							if (i == 0)
							{
								Log::WriteError(log.stream, Error::geterrorin(205, curline, curcol));
								lex_ok = false;
								break;
							}

							const char* scopename = getScopeName(tables.idtable, in.words[i - 1].word);
							if (scopename == nullptr)
							{
								Log::WriteError(log.stream, Error::geterrorin(205, curline, curcol));
								lex_ok = false;
								break;
							}
							scopes.push(std::string(scopename));
							break;
						}
						case LEX_BRACELET:
						{
							if (!scopes.empty())
								scopes.pop();
							break;
						}
						case LEX_INCR:
						{
							int index = IT::isId(tables.idtable, curword);
							std::unique_ptr<IT::Entry> itentry = std::make_unique<IT::Entry>();
							if (index != TI_NULLIDX)
							{
								idxTI = index;
							}
							else
							{
								memset(itentry->id, 0, SCOPED_ID_MAXSIZE);
								strcpy_s(itentry->id, SCOPED_ID_MAXSIZE, curword);
								itentry->idtype = IT::IDTYPE::Z;
								itentry->iddatatype = IT::IDDATATYPE::INT;
								itentry->idxfirstLE = getIndexInLT(tables.lextable, index);
								try
								{
									IT::Add(tables.idtable, *itentry);
								}
								catch (Error::ERROR e)
								{
									Log::WriteError(log.stream, e);
									lex_ok = false;
									return lex_ok;
								}
								idxTI = tables.idtable.size - 1;
							}
							lexema = *curword;
							break;
						}
						}
						lexema = *curword;
						break;
					}

					case LEX_ID_TYPE:
					{
						char id[STR_MAXSIZE] = { 0 };
						idxTI = TI_NULLIDX;

						bool isTypeData = false;

						if (tables.lextable.size > 0)
						{
							int lastIdx = tables.lextable.size - 1;
							if (lastIdx >= 0 && tables.lextable.table[lastIdx].lexema == LEX_TYPE)
							{
								isTypeData = true;
							}
						}

						if (!isTypeData && i + 1 < in.words->size && in.words[i + 1].word != nullptr)
						{
							if (strcmp(in.words[i + 1].word, "function") == 0)
							{
								isTypeData = true;
							}
						}

						if (!isTypeData && tables.lextable.size > 0)
						{
							int lastIdx = tables.lextable.size - 1;
							if (lastIdx >= 0)
							{
								char prevLex = tables.lextable.table[lastIdx].lexema;
								if (prevLex == LEX_LEFTHESIS || prevLex == LEX_COMMA)
								{
									isTypeData = true;
								}
							}
						}

						if (!isTypeData)
						{
							if (*nextword == LEX_LEFTHESIS || IT::isId(tables.idtable, curword) != TI_NULLIDX)
							{
								isFunc = true;
								if (getStandFunction(curword) == IT::STDFNC::F_NOT_STD)
									strcat_s(id, STR_MAXSIZE, "_");
							}

							const char* idtype = (isFunc && i > 1) ? in.words[i - 2].word : in.words[i - 1].word;
							if (i == 0)
								idtype = nullptr;

							if (!isFunc && !scopes.empty())
								strcpy_s(id, STR_MAXSIZE, scopes.top().c_str());

							strcat_s(id, STR_MAXSIZE, curword);

							if (isLiteral(curword))
								strcpy_s(id, STR_MAXSIZE, curword);

							IT::Entry* itentry = getEntry(tables, lexema, id, idtype, isParam, isFunc, log, curline, curcol, lex_ok, curword);
							if (itentry != nullptr)
							{
								if (isFunc)
								{
									itentry->value.params.count = 0;
									itentry->value.params.types = new IT::IDDATATYPE[MAX_PARAMS_COUNT];
									for (int k = i; k < in.words->size && in.words[k].word[0] != LEX_RIGHTTHESIS; k++)
									{
										if (in.words[k].word[0] == LEX_SEPARATOR)
											break;
										if (ISTYPE(in.words[k].word))
										{
											if (itentry->value.params.count < MAX_PARAMS_COUNT)
											{
												itentry->value.params.types[itentry->value.params.count++] = getType(nullptr, in.words[k].word);
											}
										}
									}
								}
								try
								{
									IT::Add(tables.idtable, *itentry);
								}
								catch (Error::ERROR e)
								{
									Log::WriteError(log.stream, e);
									lex_ok = false;
									delete itentry;
									return lex_ok;
								}
								idxTI = tables.idtable.size - 1;
								delete itentry;
							}
							else
							{
								int lt_index = tables.lextable.size - 1;
								if (lt_index > 0 && (tables.lextable.table[lt_index - 1].lexema == LEX_TYPE || tables.lextable.table[lt_index].lexema == LEX_TYPE
									|| tables.lextable.table[lt_index - 1].lexema == LEX_FUNCTION || tables.lextable.table[lt_index].lexema == LEX_FUNCTION
									|| tables.lextable.table[lt_index - 1].lexema == LEX_ID_TYPE || tables.lextable.table[lt_index].lexema == LEX_ID_TYPE
									|| tables.lextable.table[lt_index - 1].lexema == LEX_VOID || tables.lextable.table[lt_index].lexema == LEX_VOID))
								{
									Log::WriteError(log.stream, Error::geterrorin(305, curline, curcol));
									lex_ok = false;
								}
								if (lexema == LEX_LITERAL)
								{
									idxTI = getLiteralIndex(tables.idtable, curword, getType(id, idtype));
								}
								else
								{
									idxTI = IT::isId(tables.idtable, id);
								}
							}
						}
						break;
					}

					case LEX_ID:
					case LEX_STDFUNC:
					case LEX_LITERAL:
					{
						char id[STR_MAXSIZE] = { 0 };
						idxTI = TI_NULLIDX;

						if (*nextword == LEX_LEFTHESIS || IT::isId(tables.idtable, curword) != TI_NULLIDX)
						{
							isFunc = true;
							if (getStandFunction(curword) == IT::STDFNC::F_NOT_STD)
								strcat_s(id, STR_MAXSIZE, "_");
						}

						const char* idtype = (isFunc && i > 1) ? in.words[i - 2].word : in.words[i - 1].word;
						if (i == 0)
							idtype = nullptr;

						if (!isFunc && !scopes.empty())
							strcpy_s(id, STR_MAXSIZE, scopes.top().c_str());

						strcat_s(id, STR_MAXSIZE, curword);

						if (isLiteral(curword))
							strcpy_s(id, STR_MAXSIZE, curword);

						IT::Entry* itentry = getEntry(tables, lexema, id, idtype, isParam, isFunc, log, curline, curcol, lex_ok, curword);
						if (itentry != nullptr)
						{
							if (isFunc)
							{
								itentry->value.params.count = 0;
								itentry->value.params.types = new IT::IDDATATYPE[MAX_PARAMS_COUNT];
								for (int k = i; k < in.words->size && in.words[k].word[0] != LEX_RIGHTTHESIS; k++)
								{
									if (in.words[k].word[0] == LEX_SEPARATOR)
										break;
									if (ISTYPE(in.words[k].word))
									{
										if (itentry->value.params.count < MAX_PARAMS_COUNT)
										{
											itentry->value.params.types[itentry->value.params.count++] = getType(nullptr, in.words[k].word);
										}
									}
								}
							}
							try
							{
								IT::Add(tables.idtable, *itentry);
							}
							catch (Error::ERROR e)
							{
								Log::WriteError(log.stream, e);
								lex_ok = false;
								delete itentry;
								return lex_ok;
							}
							idxTI = tables.idtable.size - 1;
							delete itentry;
						}
						else
						{
							int lt_index = tables.lextable.size - 1;
							if (lt_index > 0 && (tables.lextable.table[lt_index - 1].lexema == LEX_TYPE || tables.lextable.table[lt_index].lexema == LEX_TYPE
								|| tables.lextable.table[lt_index - 1].lexema == LEX_FUNCTION || tables.lextable.table[lt_index].lexema == LEX_FUNCTION
								|| tables.lextable.table[lt_index - 1].lexema == LEX_ID_TYPE || tables.lextable.table[lt_index].lexema == LEX_ID_TYPE
								|| tables.lextable.table[lt_index - 1].lexema == LEX_VOID || tables.lextable.table[lt_index].lexema == LEX_VOID))
							{
								Log::WriteError(log.stream, Error::geterrorin(305, curline, curcol));
								lex_ok = false;
							}
							if (lexema == LEX_LITERAL)
							{
								idxTI = getLiteralIndex(tables.idtable, curword, getType(id, idtype));
							}
							else
							{
								idxTI = IT::isId(tables.idtable, id);
							}
						}
						break;
					}
					}

					LT::Entry ltentry(lexema, curline, curcol, idxTI);

					if (curword != nullptr && curword[0] == '@' && strlen(curword) == 1)
					{
						if (i > 0 && i + 1 < in.words->size && in.words[i + 1].word != nullptr && in.words[i - 1].word != nullptr)
						{
							char nextChar = in.words[i + 1].word[0];
							char prevChar = in.words[i - 1].word[0];

							bool hasValidPredecessor = (isalpha(static_cast<unsigned char>(prevChar)) ||
								isdigit(static_cast<unsigned char>(prevChar)) ||
								prevChar == IN_CODE_QUOTE || prevChar == IN_CODE_APOST || prevChar == LEX_MINUS);

							if (hasValidPredecessor)
							{
								if (isdigit(static_cast<unsigned char>(nextChar)) || isalpha(static_cast<unsigned char>(nextChar)) ||
									nextChar == IN_CODE_QUOTE || nextChar == IN_CODE_APOST || nextChar == LEX_MINUS)
								{
									throw Error::geterrorin(610, curline, curcol);
								}
							}
						}
						lexema = LEX_SUBST;
						ltentry.lexema = LEX_SUBST;
					}


					if (lexema == LEX_ID_TYPE && tables.lextable.size > 0)
					{
						if (tables.lextable.table[tables.lextable.size - 1].lexema == LEX_TYPE)
						{
							if (i + 1 < in.words->size)
							{
								char nextChar = in.words[i + 1].word[0];
								if (nextChar == LEX_SEPARATOR || nextChar == LEX_BRACELET)
								{
									Log::WriteError(log.stream, Error::geterrorin(303, curline, curcol));
									lex_ok = false;
								}
							}
							else
							{
								Log::WriteError(log.stream, Error::geterrorin(303, curline, curcol));
								lex_ok = false;
							}
						}
					}

					if (lexema == LEX_PLUS || lexema == LEX_MINUS || lexema == LEX_STAR ||
						lexema == LEX_DIRSLASH || lexema == LEX_PROCENT)
					{
						if (i + 1 >= in.words->size)
						{
							throw Error::geterrorin(611, curline, curcol);
						}
						else if (i + 1 < in.words->size && in.words[i + 1].word != nullptr)
						{
							char nextChar = in.words[i + 1].word[0];
							if (nextChar == LEX_SEPARATOR || nextChar == LEX_BRACELET ||
								nextChar == LEX_SQ_RBRACELET)
							{
								throw Error::geterrorin(611, curline, curcol);
							}
						}
					}

					try
					{
						LT::Add(tables.lextable, ltentry);
					}
					catch (Error::ERROR e)
					{
						Log::WriteError(log.stream, e);
						lex_ok = false;
						return lex_ok;
					}
					break;
				}
			}

			if (!wordRecognized)
			{
				if (curword != nullptr && strlen(curword) > 0 && curword[0] != ' ' && curword[0] != '\t' && curword[0] != '\0')
				{
					Log::WriteError(log.stream, Error::geterrorin(201, curline, curcol));
					lex_ok = false;
				}
			}
		}

		if (lex_ok && enterPoint == 0)
		{
			Log::WriteError(log.stream, Error::geterror(205));
			lex_ok = false;
		}
		if (enterPoint > 1)
		{
			Log::WriteError(log.stream, Error::geterror(302));
			lex_ok = false;
		}

		if (lex_ok && !scopes.empty())
		{
			throw Error::geterrorin(618, in.lines, 0);
		}

		return lex_ok;
	}
}