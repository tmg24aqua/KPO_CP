#include "pch.h"
namespace LT
{
	Entry::Entry()
	{
		lexema = NULL;
		sn = NULL;
		idxTI = NULLDX_TI;
	}

	Entry::Entry(unsigned char lexema, int snn, int idxti)
	{
		this->lexema = lexema;
		this->sn = snn;
		this->idxTI = idxti;
	}

	LexTable Create(int size)
	{
		if (size > LT_MAXSIZE)
			throw ERROR_THROW(202);
		LexTable lextable;
		lextable.table = new Entry[lextable.maxsize = size];
		lextable.size = NULL;
		return  lextable;
	}

	void Add(LexTable& lextable, Entry entry)
	{
		if (lextable.size >= lextable.maxsize)
			throw ERROR_THROW(202);
		lextable.table[lextable.size++] = entry;
	}

	void writeLexTable(std::ostream* stream, LT::LexTable& lextable)
	{
		*stream << "------------------------------ ТАБЛИЦА ЛЕКСЕМ ENV-2025 ------------------------\n" << std::endl;
		*stream << "|  N | ЛЕКСЕМА | СТРОКА | ИНДЕКС В ТИ | ТИП ЛЕКСЕМЫ |" << std::endl;
		for (int i = 0; i < lextable.size; i++)
		{
			if (lextable.table[i].lexema == '#')
				continue;

			*stream << "|" << std::setw(3) << i << " | " << std::setw(4) << lextable.table[i].lexema << "    |  " << std::setw(3)
				<< lextable.table[i].sn << "   |";

			if (lextable.table[i].idxTI == NULLDX_TI)
				*stream << "             | ";
			else
				*stream << std::setw(8) << lextable.table[i].idxTI << "     | ";

			// Добавляем описание типа лексемы
			switch (lextable.table[i].lexema)
			{
			case LEX_ID: *stream << "идентификатор   |"; break;
			case LEX_LITERAL: *stream << "литерал         |"; break;
			case LEX_FUNCTION: *stream << "function        |"; break;
			case LEX_MAIN: *stream << "main            |"; break;
			case LEX_RETURN: *stream << "return          |"; break;
			case LEX_TYPE: *stream << "type            |"; break;
			case LEX_WRITE: *stream << "write           |"; break;
			case LEX_REPEAT: *stream << "repeat          |"; break;
			case LEX_TIMES: *stream << "times           |"; break;
			case LEX_IS: *stream << "is              |"; break;
			case LEX_TRUE: *stream << "true            |"; break;
			case LEX_FALSE: *stream << "false           |"; break;
			case LEX_DO: *stream << "do              |"; break;
			case LEX_END: *stream << "end             |"; break;
			case LEX_ID_TYPE: *stream << "тип данных      |"; break;
			case LEX_STDFUNC: *stream << "стандартная ф-я |"; break;
			case LEX_SEPARATORS: *stream << "разделитель     |"; break;
			case '+': *stream << "сложение        |"; break;
			case '-': *stream << "вычитание       |"; break;
			case '*': *stream << "умножение       |"; break;
			case '/': *stream << "деление нацело  |"; break;
			case '%': *stream << "остаток от дел. |"; break;
			case '=': *stream << "присваивание    |"; break;
			case '&': *stream << "равенство (==)  |"; break;
			case '!': *stream << "неравенство (!=)|"; break;
			case '>': *stream << "больше          |"; break;
			case '<': *stream << "меньше          |"; break;
			default: *stream << "                |"; break;
			}
			*stream << std::endl;
		}
	}

	void writeLexemsOnLines(std::ostream* stream, LT::LexTable& lextable)
	{
		bool flagtemp = false;
		*stream << "\n-----------------  ЛЕКСЕМЫ СООТВЕТСТВУЮЩИЕ ИСХОДНОМУ КОДУ ENV-2025 ---------------------\n" << std::endl;
		for (int i = 0; i < lextable.size; )
		{
			if (lextable.table[i].lexema == '#')
			{
				flagtemp = true;
				i++;
				continue;
			}
			if (flagtemp)
			{
				flagtemp = false;
				*stream << lextable.table[i].lexema;
				*stream << std::endl;
				i++;
				continue;
			}
			int line = lextable.table[i].sn;
			*stream << std::setw(3) << line << " | ";
			while (i < lextable.size && lextable.table[i].sn == line)
			{
				if (lextable.table[i].lexema != '#')
				{
					*stream << lextable.table[i].lexema;
					if (lextable.table[i].idxTI != NULLDX_TI)
						*stream << "[" << lextable.table[i].idxTI << "]";
					*stream << " ";
				}
				i++;
			}
			if (i < lextable.size && lextable.table[i].lexema != '#')
				*stream << std::endl;
		}
		*stream << "-------------------------------------------------------------------------\n\n";
	}

	Entry GetEntry(LexTable& lextable, int n)
	{
		if (n < 0 || n >= lextable.size)
			throw ERROR_THROW(203);
		return lextable.table[n];
	}

	void Delete(LexTable& lextable)
	{
		if (lextable.table != nullptr)
		{
			delete[] lextable.table;
			lextable.table = nullptr;
		}
		lextable.size = 0;
		lextable.maxsize = 0;
	}

	Entry writeEntry(Entry& entry, unsigned char lexema, int indx, int line)
	{
		entry.lexema = lexema;
		entry.sn = line;
		entry.idxTI = indx;
		return entry;
	}
}