#include "pch.h"
#include <string>

using namespace std;

namespace Gener
{

	bool CodeGeneration(Lex::LEX& lex, Parm::PARM& parm, Log::LOG& log)
	{
		bool gen_ok;
		ofstream ofile(parm.out);
		ofile << BEGIN;
		ofile << EXTERN;
		ofile << ".const\n null_division BYTE 'ERROR: DIVISION BY ZERO', 0\n overflow BYTE 'ERROR: VARIABLE OVERFLOW', 0 \n";
		int conditionnum = 0, cyclenum = 0;

		// Константы и литералы
		for (int i = 0; i < lex.idtable.size; i++)
		{
			if (lex.idtable.table[i].idtype == IT::L)
			{
				ofile << "\t" << lex.idtable.table[i].id;
				if (lex.idtable.table[i].iddatatype == IT::SYM)
				{
					ofile << " BYTE \'" << lex.idtable.table[i].value.symbol << "\', 0\n";
				}
				if (lex.idtable.table[i].iddatatype == IT::STR)
				{
					ofile << " BYTE \'" << lex.idtable.table[i].value.vstr.str << "\', 0\n";
				}
				if (lex.idtable.table[i].iddatatype == IT::INT)
				{
					ofile << " SDWORD " << lex.idtable.table[i].value.vint << endl;
				}
			}
		}

		ofile << ".data\n";
		// Переменные
		for (int i = 0; i < lex.idtable.size; i++)
		{
			if (lex.idtable.table[i].idtype == IT::IDTYPE::V)
			{
				ofile << "\t" << lex.idtable.table[i].id;
				if (lex.idtable.table[i].iddatatype == IT::SYM)
				{
					ofile << " BYTE ?\n";
				}
				if (lex.idtable.table[i].iddatatype == IT::STR)
				{
					ofile << " BYTE 256 DUP(?)\n";
				}
				if (lex.idtable.table[i].iddatatype == IT::INT)
				{
					ofile << " SDWORD 0\n";
				}
			}
		}

		stack<string> stk;
		stack<IT::Entry> temp;
		string func_name = "";
		bool flag_main = false,
			flag_is = false,
			flag_true = false,
			flag_false = false,
			flag_return = false,
			in_cycle = false;

		ofile << "\n.code\n\n";

		for (int i = 0; i < lex.lextable.size; i++)
		{
			switch (lex.lextable.table[i].lexema)
			{
			case LEX_FUNCTION:
			{
				i++; // переходим к типу функции
				func_name = ITENTRY(i + 1).id; // имя функции
				flag_return = false;

				ofile << func_name << " PROC ";

				// Параметры функции
				int j = i + 3; // после function type id (
				while (LEXEMA(j) != LEX_RIGHTTHESIS)
				{
					if (LEXEMA(j) == LEX_ID)
					{
						ofile << ITENTRY(j).id << " : ";
						if (ITENTRY(j).iddatatype == IT::INT)
							ofile << "SDWORD";
						else if (ITENTRY(j).iddatatype == IT::SYM)
							ofile << "BYTE";
						else
							ofile << "DWORD";
					}
					if (LEXEMA(j) == LEX_COMMA)
						ofile << ", ";
					j++;
				}
				ofile << endl;
				break;
			}
			case LEX_MAIN:
			{
				flag_main = true;
				ofile << "main PROC\n";
				break;
			}
			case LEX_RETURN:
			{
				if (flag_main)
				{
					Log::WriteError(log.stream, Error::geterrorin(320, lex.lextable.table[i].sn, 0));
					ofile.close();
					return false;
				}

				if (LEXEMA(i + 1) != LEX_SEPARATOR)
				{
					if (ITENTRY(i + 1).iddatatype == IT::INT)
						ofile << "\tmov eax, " << ITENTRY(i + 1).id << "\n";
					else
						ofile << "\tmov eax, offset " << ITENTRY(i + 1).id << "\n";
				}
				ofile << "\tret\n";

				if (!flag_return)
				{
					ofile << "\nSOMETHINGWRONG:"\
						"\n\tpush offset null_division"\
						"\n\tcall outstrline\n"\
						"\tcall system_pause"\
						"\n\tpush -1"\
						"\n\tcall ExitProcess\n"\
						"\nEXIT_OVERFLOW:"\
						"\n\tpush offset overflow"\
						"\n\tcall outstrline\n"\
						"\tcall system_pause"\
						"\n\tpush -2"\
						"\n\tcall ExitProcess\n";
					flag_return = true;
				}
				break;
			}
			case LEX_END:
			{
				if (!flag_main)
					ofile << func_name << " ENDP\n\n";
				break;
			}
			case LEX_BRACELET:
			{
				if (flag_main)
					ofile << "main ENDP\n";
				break;
			}
			case LEX_EQUAL:
			{
				int result_position = i - 1;
				while (lex.lextable.table[i].lexema != LEX_SEPARATOR)
				{
					switch (LEXEMA(i))
					{
					case LEX_STDFUNC:
					case LEX_LITERAL:
					{
						if (ITENTRY(i).iddatatype == IT::INT)
						{
							ofile << "\tpush " << ITENTRY(i).id << endl;
							stk.push(ITENTRY(i).id);
						}
						else
						{
							ofile << "\tpush offset " << ITENTRY(i).id << endl;
							stk.push("offset " + string(ITENTRY(i).id));
						}
						break;
					}
					case LEX_ID:
					{
						ofile << "\tpush " << ITENTRY(i).id << endl;
						stk.push(ITENTRY(i).id);
						break;
					}
					case '@': // вызов функции в ПОЛИЗ
					{
						string func_name = stk.top();
						stk.pop();
						int param_count = LEXEMA(i + 1) - '0';

						ofile << "\tcall " << func_name << "\n";
						ofile << "\tadd esp, " << param_count * 4 << "\n";
						ofile << "\tpush eax\n";
						break;
					}
					case '~': // унарный минус
					{
						ofile << "\tpop eax\n";
						ofile << "\tneg eax\n";
						ofile << "\tpush eax\n";
						break;
					}
					case LEX_STAR:
					{
						ofile << "\tpop ebx\n\tpop eax\n";
						ofile << "\timul ebx\n\tjo EXIT_OVERFLOW\n\tpush eax\n";
						break;
					}
					case LEX_PLUS:
					{
						ofile << "\tpop ebx\n\tpop eax\n";
						ofile << "\tadd eax, ebx\n\tjo EXIT_OVERFLOW\n\tpush eax\n";
						break;
					}
					case LEX_MINUS:
					{
						ofile << "\tpop ebx\n\tpop eax\n";
						ofile << "\tsub eax, ebx\n\tpush eax\n";
						break;
					}
					case LEX_DIRSLASH:
					{
						ofile << "\tpop ebx\n\tpop eax\n";
						ofile << "\tcmp ebx, 0\n\tje SOMETHINGWRONG\n";
						ofile << "\tcdq\n\tidiv ebx\n\tpush eax\n";
						break;
					}
					case LEX_PROCENT:
					{
						ofile << "\tpop ebx\n\tpop eax\n";
						ofile << "\tcmp ebx, 0\n\tje SOMETHINGWRONG\n";
						ofile << "\tcdq\n\tidiv ebx\n\tpush edx\n";
						break;
					}
					}
					i++;
				}
				ofile << "\tpop " << ITENTRY(result_position).id << "\n\n";
				break;
			}
			case LEX_WRITELINE:
			{
				if (LEXEMA(i + 1) == LEX_LITERAL || LEXEMA(i + 1) == LEX_ID)
				{
					if (ITENTRY(i + 1).iddatatype == IT::INT)
					{
						ofile << "\tpush " << ITENTRY(i + 1).id << "\n";
						ofile << "\tcall outnumline\n";
					}
					else if (ITENTRY(i + 1).iddatatype == IT::STR || ITENTRY(i + 1).iddatatype == IT::SYM)
					{
						if (ITENTRY(i + 1).idtype == IT::L)
							ofile << "\tpush offset " << ITENTRY(i + 1).id << "\n";
						else
							ofile << "\tpush " << ITENTRY(i + 1).id << "\n";
						ofile << "\tcall outstrline\n";
					}
				}
				break;
			}
			case LEX_WRITE:
			{
				if (LEXEMA(i + 1) == LEX_LITERAL || LEXEMA(i + 1) == LEX_ID)
				{
					if (ITENTRY(i + 1).iddatatype == IT::INT)
					{
						ofile << "\tpush " << ITENTRY(i + 1).id << "\n";
						ofile << "\tcall outnum\n";
					}
					else if (ITENTRY(i + 1).iddatatype == IT::STR || ITENTRY(i + 1).iddatatype == IT::SYM)
					{
						if (ITENTRY(i + 1).idtype == IT::L)
							ofile << "\tpush offset " << ITENTRY(i + 1).id << "\n";
						else
							ofile << "\tpush " << ITENTRY(i + 1).id << "\n";
						ofile << "\tcall outstr\n";
					}
				}
				break;
			}
			case LEX_REPEAT:
			{
				cyclenum++;
				in_cycle = true;
				ofile << "; --- Цикл repeat " << cyclenum << " ---\n";
				ofile << "\tmov ecx, " << ITENTRY(i + 1).id << "\n";
				ofile << "cycle" << cyclenum << ":\n";
				ofile << "\tpush ecx\n"; // сохраняем счетчик
				break;
			}
			case LEX_SQ_RBRACELET:
			{
				if (in_cycle)
				{
					ofile << "\tpop ecx\n";
					ofile << "\tloop cycle" << cyclenum << "\n";
					ofile << "end_cycle" << cyclenum << ":\n";
					in_cycle = false;
				}
				if (flag_is)
				{
					ofile << "end_condition" << conditionnum << ":\n";
					flag_is = flag_true = flag_false = false;
				}
				break;
			}
			case LEX_IS:
			{
				conditionnum++;
				flag_is = true;
				string op;

				switch (LEXEMA(i + 2))
				{
				case LEX_MORE: op = "jg"; break;
				case LEX_LESS: op = "jl"; break;
				case LEX_EQUALS: op = "je"; break;
				case LEX_NOTEQUALS: op = "jne"; break;
				}

				// Сравнение целых чисел
				if (ITENTRY(i + 1).iddatatype == IT::INT && ITENTRY(i + 3).iddatatype == IT::INT)
				{
					ofile << "\tmov eax, " << ITENTRY(i + 1).id << "\n";
					ofile << "\tcmp eax, " << ITENTRY(i + 3).id << "\n";
				}
				// Сравнение строк/символов
				else
				{
					ofile << "\tpush offset " << ITENTRY(i + 1).id << "\n";
					ofile << "\tpush offset " << ITENTRY(i + 3).id << "\n";
					ofile << "\tcall compare\n";
					ofile << "\tcmp eax, 1\n";
					op = (LEXEMA(i + 2) == LEX_EQUALS) ? "je" : "jne";
				}

				ofile << "\t" << op << " true_block" << conditionnum << "\n";
				ofile << "\tjmp false_block" << conditionnum << "\n";
				break;
			}
			case LEX_TRUE:
			{
				flag_true = true;
				ofile << "true_block" << conditionnum << ":\n";
				break;
			}
			case LEX_FALSE:
			{
				flag_false = true;
				ofile << "false_block" << conditionnum << ":\n";
				break;
			}
			case LEX_STDFUNC:
			{
				if (strcmp(ITENTRY(i).id, "compare") == 0)
				{
					// compare уже обрабатывается в LEX_IS
				}
				else if (strcmp(ITENTRY(i).id, "length") == 0)
				{
					// Обработка вызова length(str)
					if (LEXEMA(i + 2) == LEX_ID || LEXEMA(i + 2) == LEX_LITERAL)
					{
						if (ITENTRY(i + 2).idtype == IT::L)
							ofile << "\tpush offset " << ITENTRY(i + 2).id << "\n";
						else
							ofile << "\tpush " << ITENTRY(i + 2).id << "\n";
						ofile << "\tcall length\n";
						ofile << "\tpush eax\n"; // результат длины
					}
				}
				break;
			}
			}
		}

		ofile << END;
		ofile.close();
		return true;
	};
}