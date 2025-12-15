#include "pch.h"
#include "IT.h"
#include <cctype>
#include <cstring>

namespace Semantic
{
	int getExpectedParamsCount(const IT::Entry& e)
	{
		if (e.idtype == IT::IDTYPE::S)
		{
			if (e.value.params.count > 0) return e.value.params.count;
			if (strstr(e.id, "random") != nullptr) return RANDOM_PARAMS_CNT;
			if (strstr(e.id, "lenght") != nullptr) return LENGHT_PARAMS_CNT;
			if (strstr(e.id, "power") != nullptr || strstr(e.id, "pow") != nullptr) return POW_PARAMS_CNT;
			if (strstr(e.id, "compare") != nullptr) return COMPARE_PARAMS_CNT;
			return 0;
		}
		return e.value.params.count;
	}

	bool isKeywordSem(const char* word)
	{
		char lower[256] = { 0 };
		for (int i = 0; word[i] != '\0' && i < 255; i++)
			lower[i] = tolower(static_cast<unsigned char>(word[i]));

		const char* keywords[] = {
			"type", "integer", "string", "symbol", "function", "main", "return",
			"write", "writeline", "is", "while", "repeat", "times", "do",
			"pow", "random", "lenght", "compare", "void"
		};

		for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
		{
			if (strcmp(lower, keywords[i]) == 0)
				return true;
		}
		return false;
	}

	bool semanticsCheck(Lex::LEX& tables, Log::LOG& log)
	{
		bool sem_ok = true;

		for (int i = 0; i < tables.idtable.size; i++)
		{
			if (tables.idtable.table[i].idtype == IT::IDTYPE::V || tables.idtable.table[i].idtype == IT::IDTYPE::P)
			{
				char cleanName[SCOPED_ID_MAXSIZE];
				strcpy_s(cleanName, tables.idtable.table[i].id);

				const char* nameToCheck = cleanName;

				if (strncmp(cleanName, "main", 4) == 0 && strlen(cleanName) > 4)
				{
					nameToCheck = cleanName + 4;
				}

				const char* underscore = strrchr(cleanName, '_');
				if (underscore != nullptr && *(underscore + 1) != '\0')
				{
					nameToCheck = underscore + 1;
				}

				if (isKeywordSem(nameToCheck))
				{
					int line = -1, col = -1;
					if (tables.idtable.table[i].idxfirstLE != TI_NULLIDX) {
						line = tables.lextable.table[tables.idtable.table[i].idxfirstLE].sn;
						col = tables.lextable.table[tables.idtable.table[i].idxfirstLE].col;
					}
					throw Error::geterrorin(318, line, col);
				}
			}
		}

		for (int i = 0; i < tables.idtable.size; i++)
		{
			if (tables.idtable.table[i].iddatatype == IT::IDDATATYPE::UNDEF)
			{
				int line = -1, col = -1;
				for (int j = 0; j < tables.lextable.size; j++) {
					if (tables.lextable.table[j].idxTI == i) {
						line = tables.lextable.table[j].sn;
						col = tables.lextable.table[j].col;
						break;
					}
				}
				throw Error::geterrorin(300, line, col);
			}
		}

		for (int i = 0; i < tables.lextable.size; i++) {
			if (tables.lextable.table[i].lexema == LEX_TYPE) {
				if (i + 2 < tables.lextable.size) {
					int idIdx = tables.lextable.table[i + 2].idxTI;
					if (idIdx != TI_NULLIDX && tables.idtable.table[idIdx].idxfirstLE < (i + 2)) {
						throw Error::geterrorin(305, tables.lextable.table[i + 2].sn, tables.lextable.table[i + 2].col);
					}
				}
			}
		}

		for (int i = 0; i < tables.lextable.size; i++)
		{
			if (tables.lextable.table[i].lexema == LEX_EQUAL)
			{
				if (i + 1 < tables.lextable.size)
				{
					char nextLex = tables.lextable.table[i + 1].lexema;
					if (nextLex == LEX_SEPARATOR || nextLex == LEX_BRACELET || nextLex == LEX_SQ_RBRACELET)
					{
						bool inLoop = false;
						for (int k = i; k >= 0 && k > i - 10; k--) {
							if (tables.lextable.table[k].lexema == LEX_REPEAT) { inLoop = true; break; }
						}
						if (inLoop)
							throw Error::geterrorin(617, tables.lextable.table[i].sn, tables.lextable.table[i].col);
						else
							throw Error::geterrorin(615, tables.lextable.table[i].sn, tables.lextable.table[i].col);
					}
					if (nextLex == LEX_EQUAL)
					{
						throw Error::geterrorin(612, tables.lextable.table[i].sn, tables.lextable.table[i].col);
					}
				}
			}
		}

		for (int i = 0; i < tables.lextable.size; i++)
		{
			if (tables.lextable.table[i].lexema == LEX_FUNCTION)
			{
				int j = i + 2;
				if (j < tables.lextable.size && tables.lextable.table[j].lexema == LEX_LEFTHESIS)
				{
					int declaredParams = 0;
					j++;
					while (j < tables.lextable.size && tables.lextable.table[j].lexema != LEX_RIGHTTHESIS)
					{
						if (tables.lextable.table[j].lexema == LEX_ID_TYPE) declaredParams++;
						j++;
					}
					if (declaredParams > MAX_PARAMS_COUNT)
					{
						throw Error::geterrorin(306, tables.lextable.table[i].sn, tables.lextable.table[i].col);
					}
				}
			}
		}

		for (int i = 0; i < tables.lextable.size; i++)
		{
			if (tables.lextable.table[i].lexema == LEX_LEFTHESIS)
			{
				if (i + 1 < tables.lextable.size && tables.lextable.table[i + 1].lexema == LEX_RIGHTTHESIS)
				{
					if (i > 0)
					{
						char prevLex = tables.lextable.table[i - 1].lexema;
						if (prevLex == LEX_ID || prevLex == LEX_STDFUNC)
						{
							throw Error::geterrorin(601, tables.lextable.table[i].sn, tables.lextable.table[i].col);
						}
					}
				}
			}
		}

		for (int i = 0; i < tables.lextable.size; i++)
		{
			switch (tables.lextable.table[i].lexema)
			{
			case LEX_INCR:
			{
				IT::Entry e = tables.idtable.table[tables.lextable.table[i - 1].idxTI];
				if (e.iddatatype != IT::IDDATATYPE::INT || tables.idtable.table[tables.lextable.table[i + 1].idxTI].iddatatype != IT::IDDATATYPE::INT)
				{
					throw Error::geterrorin(314, tables.lextable.table[i].sn, tables.lextable.table[i].col);
				}
			}
			break;

			case LEX_EQUAL:
				if (i > 0 && tables.lextable.table[i - 1].idxTI != TI_NULLIDX)
				{
					IT::IDDATATYPE lefttype = tables.idtable.table[tables.lextable.table[i - 1].idxTI].iddatatype;
					bool ignore = false;

					for (int k = i + 1; tables.lextable.table[k].lexema != LEX_SEPARATOR; k++)
					{
						if (k == tables.lextable.size) break;
						if (tables.lextable.table[k].idxTI != TI_NULLIDX)
						{
							if (!ignore)
							{
								IT::IDDATATYPE righttype = tables.idtable.table[tables.lextable.table[k].idxTI].iddatatype;
								if (lefttype != righttype)
								{
									throw Error::geterrorin(314, tables.lextable.table[k].sn, tables.lextable.table[k].col);
								}
							}
							if (tables.lextable.table[k + 1].lexema == LEX_LEFTHESIS) { ignore = true; continue; }
							if (ignore && tables.lextable.table[k + 1].lexema == LEX_RIGHTTHESIS) { ignore = false; continue; }
						}

						if (lefttype == IT::IDDATATYPE::STR || lefttype == IT::IDDATATYPE::SYM)
						{
							char l = tables.lextable.table[k].lexema;
							if (l == LEX_PLUS || l == LEX_MINUS || l == LEX_STAR || l == LEX_DIRSLASH || l == LEX_PROCENT)
							{
								throw Error::geterrorin(315, tables.lextable.table[k].sn, tables.lextable.table[k].col);
							}
						}
					}
				}
				break;

			case LEX_ID:
			case LEX_STDFUNC:
			{
				IT::Entry e = tables.idtable.table[tables.lextable.table[i].idxTI];

				if (i + 1 < tables.lextable.size && tables.lextable.table[i + 1].lexema == LEX_LEFTHESIS && (i == 0 || tables.lextable.table[i - 1].lexema != LEX_FUNCTION))
				{
					if (e.idtype == IT::IDTYPE::F || e.idtype == IT::IDTYPE::S)
					{
						int paramscount = 0;
						for (int j = i + 1; tables.lextable.table[j].lexema != LEX_RIGHTTHESIS; j++)
						{
							if (tables.lextable.table[j].lexema == LEX_ID || tables.lextable.table[j].lexema == LEX_LITERAL)
								paramscount++;
							if (j == tables.lextable.size) break;
						}

						int expectedParams = getExpectedParamsCount(e);

						if (expectedParams > 0 && paramscount > expectedParams)
						{
							throw Error::geterrorin(307, tables.lextable.table[i].sn, tables.lextable.table[i].col);
						}

						if (paramscount > MAX_PARAMS_COUNT) {
							throw Error::geterrorin(307, tables.lextable.table[i].sn, tables.lextable.table[i].col);
						}
					}
				}
				break;
			}

			case LEX_DIRSLASH:
			case LEX_PROCENT:
			{
				if (tables.lextable.table[i + 1].lexema == LEX_LITERAL)
				{
					if (tables.idtable.table[tables.lextable.table[i + 1].idxTI].value.vint == 0)
					{
						throw Error::geterrorin(317, tables.lextable.table[i].sn, tables.lextable.table[i].col);
					}
				}
			}
			break;
			}
		}

		return sem_ok;
	}
};