#include "pch.h"
namespace LT
{
	Entry::Entry()
	{
		lexema = NULL;
		sn = NULL;
		col = 0;
		idxTI = NULLDX_TI;
	}

	Entry::Entry(unsigned char lexema, int snn, int coln, int idxti)
	{
		this->lexema = lexema;
		this->sn = snn;
		this->col = coln;
		this->idxTI = idxti;
	}

	LexTable Create(int size)
	{
		if (size > LT_MAXSIZE)
		{
			Error::ERROR err = Error::geterror(202);
			throw err;
		}
		LexTable lextable;
		lextable.table = new Entry[lextable.maxsize = size];
		lextable.size = 0;
		return  lextable;
	}

	void Add(LexTable &lextable, Entry entry)
	{
		if (lextable.size >= lextable.maxsize)
		{
			Error::ERROR err = Error::geterrorin(202, entry.sn, entry.col);
			throw err;
		}
		lextable.table[lextable.size++] = entry;
	}
	void writeLexTable(std::ostream* stream, LT::LexTable& lextable)
	{
		*stream << "------------------------------ ÒÀÁËÈÖÀ ËÅÊÑÅÌ  ------------------------\n" << std::endl;
		*stream << "|  N | ËÅÊÑÅÌÀ | ÑÒÐÎÊÀ | ÈÍÄÅÊÑ Â ÒÈ |" << std::endl;
		for (int i = 0; i < lextable.size; i++)
		{
			if (lextable.table[i].lexema == '#')
				continue;
			*stream << "|" << std::setw(3) << i << " | " << std::setw(4) << lextable.table[i].lexema << "    |  " << std::setw(3)
				<< lextable.table[i].sn << "   |";
			if (lextable.table[i].idxTI == -1)
				*stream << "             |" << std::endl;
			else
				*stream << std::setw(8) << lextable.table[i].idxTI << "     |" << std::endl;
		}
	}

	void writeLexemsOnLines(std::ostream* stream, LT::LexTable& lextable)
	{
		bool flagtemp = false;
		*stream << "\n-----------------  ËÅÊÑÅÌÛ ÑÎÎÒÂÅÒÑÒÂÓÞÙÈÅ ÈÑÕÎÄÍÎÌÓ ÊÎÄÓ ---------------------\n" << std::endl;
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
			while (lextable.table[i].sn == line)
			{
				*stream << lextable.table[i].lexema;
				if (lextable.table[i].idxTI != NULLDX_TI)
					*stream << "[" << lextable.table[i].idxTI << "]";
				i++;
			}
			if(lextable.table[i].lexema != '#')
			*stream << std::endl;
		}
		*stream << "-------------------------------------------------------------------------\n\n";
	}
}