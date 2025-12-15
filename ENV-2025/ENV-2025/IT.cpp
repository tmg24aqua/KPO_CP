#include "pch.h"
#define W(x, y)\
		<< std::setw(x) << (y) <<
#define STR(n, line, type, id)\
		"|" W(4,n) " |  " W(6,line) "    |" W(21,type) " |  " W(SCOPED_ID_MAXSIZE, id) " |"
namespace IT
{
	IdTable Create(int size)
	{
		if (size > MAXSIZE_TI)
		{
			Error::ERROR err = Error::geterror(203);
			throw err;
		}
		IdTable idtable;
		idtable.table = new Entry[idtable.maxsize = size];
		idtable.size = 0;
		return idtable;
	}
	void Add(IdTable& idtable, Entry entry)
	{
		if (static_cast<unsigned int>(idtable.size) >= static_cast<unsigned int>(idtable.maxsize))
		{
			Error::ERROR err = Error::geterror(203);
			throw err;
		}
		idtable.table[idtable.size++] = entry;
	}

	int isId(IdTable& idtable, const char id[SCOPED_ID_MAXSIZE])
	{
		for (int i = 0; i < idtable.size; i++)
		{
			if (strcmp(idtable.table[i].id, id) == 0)
				return i;
		}
		return TI_NULLIDX;
	}

	bool SetValue(IT::IdTable& idtable, int index, const char* value)
	{
		return SetValue(&(idtable.table[index]), value);
	}
	bool SetValue(IT::Entry* entry, const char* value)
	{
		bool rc = true;
		if (entry->iddatatype == INT)
		{
			short temp = 0;
			const char* parseValue = value;
			bool hasQuotes = (value[0] == IN_CODE_APOST && value[strlen(value) - 1] == IN_CODE_APOST);
			
			if (hasQuotes && strlen(value) > 2)
			{
				size_t bufSize = strlen(value) - 1;
				char* tempBuf = new char[bufSize];
				strncpy_s(tempBuf, bufSize, value + 1, strlen(value) - 2);
				tempBuf[strlen(value) - 2] = '\0';
				parseValue = tempBuf;
			}
			
			bool valueValid = true;
			long long temp_long = 0;
			
			if ((parseValue[0] == '-' && (parseValue[1] == '0' && (parseValue[2] == 'x' || parseValue[2] == 'X'))) || (parseValue[0] == '0' && (parseValue[1] == 'x' || parseValue[1] == 'X')))
			{
				char* endptr;
				temp_long = strtoll(parseValue, &endptr, 16);
				if (temp_long > TI_INT_MAXSIZE || temp_long < TI_INT_MINSIZE)
				{
					valueValid = false;
				}
				temp = (short)temp_long;
			}
			else if ((parseValue[0] == '-' && parseValue[1] == '0' && (parseValue[2] == 'b' || parseValue[2] == 'B')) || (parseValue[0] == '0' && (parseValue[1] == 'b' || parseValue[1] == 'B')))
			{
				const char* binStr = parseValue;
				if (parseValue[0] == '-')
					binStr = parseValue + 3;
				else
					binStr = parseValue + 2;
				temp_long = 0;
				for (int i = 0; binStr[i] != '\0'; i++)
				{
					if (binStr[i] == '0' || binStr[i] == '1')
						temp_long = (temp_long << 1) | (binStr[i] - '0');
				}
				if (parseValue[0] == '-')
					temp_long = -temp_long;
				if (temp_long > TI_INT_MAXSIZE || temp_long < TI_INT_MINSIZE)
				{
					valueValid = false;
				}
				temp = (short)temp_long;
			}
			else if ((parseValue[0] == '-' && parseValue[1] == '0') || parseValue[0] == '0')
			{
				char* endptr;
				temp_long = strtoll(parseValue, &endptr, 8);
				if (temp_long > TI_INT_MAXSIZE || temp_long < TI_INT_MINSIZE)
				{
					valueValid = false;
				}
				temp = (short)temp_long;
			}
			else if (isalpha(parseValue[0]))
			{
				char lower[256];
				for (int i = 0; parseValue[i] != '\0' && i < 255; i++)
					lower[i] = tolower(parseValue[i]);
				lower[255] = '\0';
				if (strstr(lower, "true") || strstr(lower, "èñòèíà") || strstr(lower, "äà") ||
					strstr(lower, "yes") || strstr(lower, "1") || strstr(lower, "+") ||
					strstr(lower, "on") || strstr(lower, "âêë"))
					temp = 1;
				else if (strstr(lower, "false") || strstr(lower, "ëîæü") || strstr(lower, "íåò") ||
					strstr(lower, "no") || strstr(lower, "0") || strstr(lower, "-") ||
					strstr(lower, "off") || strstr(lower, "âûêë"))
					temp = 0;
				else
				{
					char* endptr;
					temp_long = strtoll(parseValue, &endptr, 10);
					if (temp_long > TI_INT_MAXSIZE || temp_long < TI_INT_MINSIZE)
					{
						valueValid = false;
					}
					temp = (short)temp_long;
				}
			}
			else
			{
				char* endptr;
				temp_long = strtoll(parseValue, &endptr, 10);
				if (temp_long > TI_INT_MAXSIZE || temp_long < TI_INT_MINSIZE)
				{
					valueValid = false;
				}
				temp = (short)temp_long;
			}
			
			if (hasQuotes && strlen(value) > 2)
			{
				delete[] const_cast<char*>(parseValue);
			}
			
			if (!valueValid)
			{
				rc = false;
				if (temp_long > TI_INT_MAXSIZE)
					temp = TI_INT_MAXSIZE;
				else if (temp_long < TI_INT_MINSIZE)
					temp = TI_INT_MINSIZE;
			}
			
			entry->value.vint = temp;
		}
		else if (entry->iddatatype == STR)
		{
			size_t value_len = strlen(value);
			size_t str_len = (value_len >= 2) ? (value_len - 2) : 0;
			
			size_t max_str_size = STR_MAXSIZE - 1;
			size_t max_copy_len = max_str_size - 1;
			
			if (str_len > max_copy_len)
			{
				str_len = max_copy_len;
			}
			
			if (value_len >= 2)
			{
				if (str_len > 0)
				{
					memcpy(entry->value.vstr.str, value + 1, str_len);
				}
				entry->value.vstr.str[str_len] = '\0';
				entry->value.vstr.len = static_cast<int>(str_len);
			}
			else
			{
				entry->value.vstr.str[0] = '\0';
				entry->value.vstr.len = 0;
			}
		}
		else
		{
			entry->value.symbol = value[1];
		}
		return rc;
	}
	void writeIdTable(std::ostream* stream, IT::IdTable& idtable)
	{
		*stream << "---------------------------- ÒÀÁËÈÖÀ ÈÄÅÍÒÈÔÈÊÀÒÎÐÎÂ ------------------------\n" << std::endl;
		*stream << "|  N  |ÑÒÐÎÊÀ Â ÒË |  ÒÈÏ ÈÄÅÍÒÈÔÈÊÀÒÎÐÀ  |        ÈÌß         | ÇÍÀ×ÅÍÈÅ (ÏÀÐÀÌÅÒÐÛ)" << std::endl;
		for (int i = 0; i < idtable.size; i++)
		{
			IT::Entry* e = &idtable.table[i];
			char type[50] = "";

			switch (e->iddatatype)
			{
			case IT::IDDATATYPE::INT:
				strcat_s(type, 50, "  integer ");
				break;
			case IT::IDDATATYPE::STR:
				strcat_s(type, 50, " string  ");
				break;
			case IT::IDDATATYPE::SYM:
				strcat_s(type, 50, "   sumbol  ");
				break;
			case IT::IDDATATYPE::UNDEF:
				strcat_s(type, 50, "UNDEFINED");
				break;
			}
			switch (e->idtype)
			{
			case IT::IDTYPE::V:
				strcat_s(type, 50, "  variable");
				break;
			case IT::IDTYPE::F:
				strcat_s(type, 50, "  function");
				break;
			case IT::IDTYPE::P:
				strcat_s(type, 50, " parameter");
				break;
			case IT::IDTYPE::L:
				strcat_s(type, 50, "   literal");
				break;
			case IT::IDTYPE::S:
				strcat_s(type, 50, "  LIB FUNC");
				break;
			default:
				strcat_s(type, 50, "UNDEFINED ");
				break;
			}

			*stream << STR(i, e->idxfirstLE, type, e->id);
			if ((e->idtype == IT::IDTYPE::L || e->idtype == IT::IDTYPE::V) && e->iddatatype != IT::IDDATATYPE::UNDEF)
			{
				if (e->iddatatype == IT::IDDATATYPE::INT)
					*stream << e->value.vint;
				else if (e->iddatatype == IT::IDDATATYPE::STR)
					*stream << "[" << (int)e->value.vstr.len << "]" << e->value.vstr.str;
				else
					*stream << e->value.symbol;
			}
			if (e->idtype == IT::IDTYPE::F || e->idtype == IT::IDTYPE::S)
			{
				for (int i = 0; i < e->value.params.count; i++)
				{
					*stream << " P" << i << ":";
					switch (e->value.params.types[i])
					{
					case IT::IDDATATYPE::INT:
						*stream << "INTEGER |";
						break;
					case IT::IDDATATYPE::STR:
						*stream << "STRING |";
						break;
					case IT::IDDATATYPE::SYM:
						*stream << "SYMBOL |";
						break;
					case IT::IDDATATYPE::UNDEF:
						*stream << "UNDEFINED";
						break;
					}
				}
			}
			*stream << std::endl;
		}
		*stream << "\n-------------------------------------------------------------------------\n\n";
	}


};