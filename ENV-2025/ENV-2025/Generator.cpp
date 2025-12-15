#include "pch.h"
#include <string>
#include <vector>

using namespace std;

namespace Gener
{
	bool CodeGeneration(Lex::LEX& lex, Parm::PARM& parm, Log::LOG& log)
	{
		bool gen_ok = true;
		ofstream ofile(parm.out);
		ofile << BEGIN;
		ofile << EXTERN;

		ofile << ".const\n null_division BYTE 'ERROR: DIVISION BY ZERO', 0\n overflow BYTE 'ERROR: VARIABLE OVERFLOW', 0 \n";
		int conditionnum = 0, cyclenum = 0;
		
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
					ofile << " SWORD " << lex.idtable.table[i].value.vint << endl;
				}
			}
		}
		
		ofile << ".data\n";
		for (int i = 0; i < lex.idtable.size; i++)
		{
			if (lex.idtable.table[i].idtype == IT::IDTYPE::V)
			{
				ofile << "\t" << lex.idtable.table[i].id;
				if (lex.idtable.table[i].iddatatype == IT::SYM)
				{
					ofile << " DWORD ?\n";
				}
				if (lex.idtable.table[i].iddatatype == IT::STR)
				{
					ofile << " DWORD ?\n";
				}
				if (lex.idtable.table[i].iddatatype == IT::INT)
				{
					ofile << " SWORD 0\n";
				}
			}
		}

		stack<string> stk;
		stack<IT::Entry> temp;
		string cyclecode = "", func_name = "";					
		bool flag_cycle = false,				
			flag_main = false,
			flag_is = false,					
			flag_true = false,
			flag_false = false,
			flag_return = false;

		ofile << "\n.code\n\n";
		for (int i = 0; i < lex.lextable.size; i++)
		{
			switch (lex.lextable.table[i].lexema)
			{
			case LEX_FUNCTION:
			{
				i++;
				flag_return = false;
				ofile << (func_name = ITENTRY(i).id) << " PROC ";
				int j = i + 2;
				while (LEXEMA(j) != LEX_RIGHTTHESIS) 
				{
					if (lex.lextable.table[j].lexema == LEX_ID)
					{
						ofile << lex.idtable.table[lex.lextable.table[j].idxTI].id << " : ";
						if (ITENTRY(j).iddatatype == IT::IDDATATYPE::INT)
						{
							ofile << " SWORD ";
						}
						else if (ITENTRY(j).iddatatype == IT::IDDATATYPE::SYM)
						{
							ofile << " DWORD ";
						}
						else
						{
							ofile << " DWORD ";
						}
					}
					if (LEXEMA(j) == LEX_COMMA)
					{
						ofile << ", ";
					}
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
				if (LEXEMA(i + 1) != LEX_SEPARATOR)
				{
					if (LEXEMA(i + 1) != LEX_LEFTHESIS && 
					   (ITENTRY(i + 1).idtype == IT::IDTYPE::V || ITENTRY(i + 1).idtype == IT::IDTYPE::P))
					{
						if (ITENTRY(i + 1).iddatatype == IT::IDDATATYPE::INT)
						{
							ofile << "\tmovsx eax, word ptr " << ITENTRY(i + 1).id << "\n";
						}
						else
						{
							ofile << "\tmov eax, offset " << ITENTRY(i + 1).id << "\n";
						}
					}
					else if (ITENTRY(i + 1).idtype == IT::IDTYPE::L)
					{
						if (ITENTRY(i + 1).iddatatype == IT::IDDATATYPE::INT)
							ofile << "\tmov eax, " << ITENTRY(i + 1).value.vint << "\n";
						else
							ofile << "\tmov eax, offset " << ITENTRY(i + 1).id << "\n";
					}
					else
					{
						if (LEXEMA(i + 1) != LEX_LEFTHESIS)
							ofile << "\tmov eax, offset " << ITENTRY(i + 1).id << "\n";
						else if (LEXEMA(i + 2) != LEX_MINUS)
							ofile << "\tmov eax, " << ITENTRY(i + 2).id << "\n";
						else
						{
							ofile << "\tmov eax, 0\n";
							ofile << "\tmov ebx," << ITENTRY(i + 3).id;
							ofile << "\n\tsub eax, ebx\n";
						}
					}
				}
				
				ofile << "\tret\n";
				
				if (!flag_return)
				{
					ofile << "\nSOMETHINGWRONG:"\
						"\npush offset null_division"\
						"\ncall outstrline\n"\
						"call system_pause"\
						"\npush -1"\
						"\ncall ExitProcess\n"\
						"\nEXIT_OVERFLOW:"\
						"\npush offset overflow"\
						"\ncall outstrline\n"\
						"call system_pause"\
						"\npush -2"\
						"\ncall ExitProcess\n";
					flag_return = true;
				}
				break;
			}
			case LEX_BRACELET:
			{
				if (flag_main)
					break;
				ofile << func_name + " ENDP\n";
				break;
			}
			case LEX_EQUAL:
			{
				int result_position = i - 1;
				bool isSimpleLiteralAssignment = false;
				int expr_start = i;
				int next_separator = i;
				while (lex.lextable.table[next_separator].lexema != LEX_SEPARATOR)
					next_separator++;
				if (next_separator - expr_start == 2 && LEXEMA(i + 1) == LEX_LITERAL && ITENTRY(i + 1).iddatatype == IT::IDDATATYPE::INT)
				{
					isSimpleLiteralAssignment = true;
					ofile << "\tmov word ptr " << ITENTRY(result_position).id << ", " << ITENTRY(i + 1).value.vint << endl;
					i = next_separator;
					break;
				}

				while (lex.lextable.table[i].lexema != LEX_SEPARATOR)
				{

					switch (LEXEMA(i))
					{
					case LEX_STDFUNC:
					case LEX_LITERAL:
					{
						bool isFunctionName = (ITENTRY(i).idtype == IT::IDTYPE::F || ITENTRY(i).idtype == IT::IDTYPE::S);
						if (isFunctionName)
						{
							break;
						}
						if (ITENTRY(i).iddatatype == IT::IDDATATYPE::INT)
						{
							ofile << "\tmovsx eax, word ptr " << ITENTRY(i).id << endl;
							ofile << "\tpush eax" << endl;
							stk.push(lex.idtable.table[lex.lextable.table[i].idxTI].id);
							break;
						}
						else
						{
							ofile << "\tpush offset " << ITENTRY(i).id << endl;
							stk.push("offset" + (string)lex.idtable.table[lex.lextable.table[i].idxTI].id);
							break;
						}
					}
					case LEX_ID:
					{
						bool isFunctionName = (ITENTRY(i).idtype == IT::IDTYPE::F || ITENTRY(i).idtype == IT::IDTYPE::S);
						if (isFunctionName)
						{
							break;
						}
						if (ITENTRY(i).idtype == IT::IDTYPE::V && ITENTRY(i).iddatatype == IT::IDDATATYPE::INT)
						{
							ofile << "\tmovsx eax, word ptr " << ITENTRY(i).id << endl;
							ofile << "\tpush eax" << endl;
						}
						else
							ofile << "\tpush " << ITENTRY(i).id << endl;
						stk.push(lex.idtable.table[lex.lextable.table[i].idxTI].id);
						break;
					}
					case LEX_SUBST:
					{
							int param_count = 0;
						
						if (i + 1 < lex.lextable.size)
						{
							char count_char = lex.lextable.table[i + 1].lexema;
							if (count_char >= '0' && count_char <= '9')
							{
								param_count = count_char - '0';
							}
						}
						
						if (param_count <= 0 || param_count > 5)
						{
							for (int k = i + 1; k < lex.lextable.size && k <= i + 5; k++)
							{
								if (lex.lextable.table[k].lexema == LEX_SEPARATOR)
									break;
								char count_char = lex.lextable.table[k].lexema;
								if (count_char >= '0' && count_char <= '9')
								{
									int candidate = count_char - '0';
									if (candidate > 0 && candidate <= 5)
									{
										param_count = candidate;
										break;
									}
								}
							}
						}
						
						if (param_count <= 0 || param_count > 5)
						{
							int stack_size = (int)stk.size();
							if (stack_size > 0 && stack_size <= 5)
							{
								param_count = stack_size;
							}
							else
							{
								param_count = 2; 
							}
						}
						
						
						if (param_count > 5)
							param_count = 5;
						if (param_count <= 0)
							param_count = 1;
						string func_name = "";
						for (int k = i + 2; k < lex.lextable.size && k <= i + 20; k++) 
						{
							if (lex.lextable.table[k].lexema == LEX_SEPARATOR)
								break;
							if (lex.lextable.table[k].idxTI != TI_NULLIDX &&
								(ITENTRY(k).idtype == IT::IDTYPE::F || ITENTRY(k).idtype == IT::IDTYPE::S))
							{
								func_name = ITENTRY(k).id;
								break;
							}
						}
						if (func_name.empty())
						{
							for (int k = i - 1; k >= 0 && k >= i - 20; k--)
							{
								if (k < 0 || lex.lextable.table[k].lexema == LEX_SEPARATOR)
									break;
								if (lex.lextable.table[k].idxTI != TI_NULLIDX &&
									(ITENTRY(k).idtype == IT::IDTYPE::F || ITENTRY(k).idtype == IT::IDTYPE::S))
								{
									bool isAlreadyProcessed = false;
									for (int m = k + 1; m < i; m++)
						{
										if (lex.lextable.table[m].lexema == LEX_SUBST)
										{
											isAlreadyProcessed = true;
											break;
										}
									}
									if (!isAlreadyProcessed)
									{
										func_name = ITENTRY(k).id;
										break;
									}
								}
							}
						}
						if (func_name.empty() && i - param_count - 1 >= 0 && lex.lextable.table[i - param_count - 1].idxTI != TI_NULLIDX)
						{
							if (ITENTRY(i - param_count - 1).idtype == IT::IDTYPE::F || ITENTRY(i - param_count - 1).idtype == IT::IDTYPE::S)
							{
								func_name = ITENTRY(i - param_count - 1).id;
							}
						}
					if (func_name.empty())
					{
						ofile.close();
						return false;
					}
						bool is_string_func = (func_name == "compare" || func_name == "lenght");
						
						int actual_param_count = (int)stk.size();
						
						int expected_param_count = 0;
						if (func_name == "power" || func_name == "pow")
							expected_param_count = 2;
						else if (func_name == "random")
							expected_param_count = 2;
						else if (func_name == "compare")
							expected_param_count = 2;
						else if (func_name == "lenght")
							expected_param_count = 1;
						
						if (expected_param_count > 0)
						{
							param_count = expected_param_count;
						}
						else if (actual_param_count > 0 && actual_param_count <= 5 && 
							(param_count <= 0 || param_count > 5 || (param_count != actual_param_count && actual_param_count < param_count)))
						{
							param_count = actual_param_count;
						}
						
						vector<string> params;
						for (int j = 0; j < param_count; j++)
						{
							if (!stk.empty())
							{
								params.push_back(stk.top());
							stk.pop();
						}
						}

						if (param_count == 1)
						{

						}
						else if (param_count == 2)
						{
							ofile << "\tpop edx\n"; 
							ofile << "\tpop ecx\n"; 
							ofile << "\tpush edx\n"; 
							ofile << "\tpush ecx\n"; 
						}
						else
						{
							for (int j = param_count - 1; j >= 0; j--)
							{
								if (j == param_count - 1)
									ofile << "\tpop ecx\n";
								else if (j == param_count - 2)
									ofile << "\tpop edx\n";
								else if (j == param_count - 3)
									ofile << "\tpop esi\n";
								else
									ofile << "\tpop edi\n";
							}
							for (int j = param_count - 1; j >= 0; j--)
							{
								if (j == param_count - 1)
									ofile << "\tpush ecx\n";
								else if (j == param_count - 2)
									ofile << "\tpush edx\n";
								else if (j == param_count - 3)
									ofile << "\tpush esi\n";
								else
									ofile << "\tpush edi\n";
							}
						}
						ofile << "\t\tcall " << func_name << "\n\tpush eax\n";
						break;
					}
					case LEX_TILDA:
					{
						ofile << "\tmov eax, 0\n\tpop ebx\n";
						ofile << "\tsub eax, ebx\n\tpush eax\n";
						break;
					}
					case LEX_STAR:
					{
						ofile << "\tpop ebx\n\tpop eax\n";
						ofile << "\timul ebx\n\tpush eax\n";

						break;
					}

					case LEX_PLUS:
					{
						ofile << "\tpop ebx\n\tpop eax\n";
						ofile << "\tadd eax, ebx\n\tpush eax\n";
						break;
					}
					case LEX_MINUS:
					{
						if (LEXEMA(i - 2) == LEX_EQUAL)
						{
							ofile << "\tmov eax, 0\n\tpop ebx\n";
							ofile << "\tsub eax, ebx\n\tpush eax\n";
							break;
						}
						ofile << "\tpop ebx\n\tpop eax\n";
						ofile << "\tsub eax, ebx\n\tpush eax\n";
						break;
					}
					case LEX_DIRSLASH:
					{
						ofile << "\tpop ebx\n\tpop eax\n";
						ofile << "\tcmp ebx,0\n"\
							"\tje SOMETHINGWRONG\n";
						ofile << "\tcdq\n";
						ofile << "\tidiv ebx\n\tpush eax\n";
						break;
					}
					case LEX_PROCENT:
					{

						ofile << "\tpop ebx\n\tpop eax\n";
						ofile << "\tcmp ebx,0\n"\
							"\tje SOMETHINGWRONG\n";
						ofile << "\tcdq\n";
						ofile << "\tidiv ebx\n\tpush edx\n";
						break;
					}
					}
					i++;
				}
				if ((ITENTRY(result_position).idtype == IT::IDTYPE::V || ITENTRY(result_position).idtype == IT::IDTYPE::P) 
					&& ITENTRY(result_position).iddatatype == IT::IDDATATYPE::INT)
				{
					ofile << "\tpop eax\n";
					ofile << "\tmov word ptr " << ITENTRY(result_position).id << ", ax\n";
				}
				else
				{
					ofile << "\tpop " << ITENTRY(result_position).id << "\n";
				}
				ofile << endl;
				break;
			}
			case LEX_ID:
			{
				if (LEXEMA(i + 1) == LEX_LEFTHESIS && LEXEMA(i - 1) != LEX_FUNCTION
					&& lex.lextable.table[i].sn > lex.lextable.table[i - 1].sn)
				{

				}
				break;
			}
				case LEX_INCR:
				{
					ofile << "\tmov eax," << ITENTRY(i - 1).id << "\n";
					ofile << "\tmov ebx," << ITENTRY(i + 1).id << "\n";
					switch (ITENTRY(i).id[1])
					{
					case LEX_PLUS:
					{
						ofile << "\tadd eax, ebx\n";
						break;
					}
					case LEX_MINUS:
					{
						ofile << "\tsub eax, ebx\n";
						break;
					}
					case LEX_STAR:
					{
						ofile << "\timul ebx\n";
						break;
					}
					case LEX_DIRSLASH:
					{
						ofile << "\tcmp ebx,0\n"\
							"\tje SOMETHINGWRONG\n";
						ofile << "\tcdq\n";
						ofile << "\tidiv ebx\n";
						break;
					}
					}
					ofile << "\tmov " << ITENTRY(i - 1).id << " , eax\n";
					break;
				}
			case LEX_NEWLINE:
			{
				switch (ITENTRY(i + 1).iddatatype)
				{
				case IT::IDDATATYPE::INT:
					if (ITENTRY(i + 1).idtype == IT::IDTYPE::V || ITENTRY(i + 1).idtype == IT::IDTYPE::P)
						ofile << "\n\tmovsx eax, word ptr " << ITENTRY(i + 1).id << "\n\tpush eax\ncall outnumline\n";
					else
						ofile << "\npush " << ITENTRY(i + 1).id << "\ncall outnumline\n";
					break;
				case IT::IDDATATYPE::SYM:
				case IT::IDDATATYPE::STR:
					if (ITENTRY(i + 1).idtype == IT::IDTYPE::L)
						ofile << "\npush offset " << ITENTRY(i + 1).id << "\ncall outstrline\n";
					else ofile << "\npush " << ITENTRY(i + 1).id << "\ncall outstrline\n";
					break;
				}
				break;
			}
			case LEX_WRITE:
			{
				switch (ITENTRY(i + 1).iddatatype)
				{
				case IT::IDDATATYPE::INT:
					if (ITENTRY(i + 1).idtype == IT::IDTYPE::V || ITENTRY(i + 1).idtype == IT::IDTYPE::P)
						ofile << "\n\tmovsx eax, word ptr " << ITENTRY(i + 1).id << "\n\tpush eax\ncall outnum\n";
					else
						ofile << "\npush " << ITENTRY(i + 1).id << "\ncall outnum\n";
					break;
				case IT::IDDATATYPE::SYM:
				case IT::IDDATATYPE::STR:
					if (ITENTRY(i + 1).idtype == IT::IDTYPE::L)
						ofile << "\npush offset " << ITENTRY(i + 1).id << "\ncall outstr\n";
					else ofile << "\npush " << ITENTRY(i + 1).id << "\ncall outstr\n";
					break;
				}
				break;
			}
			case LEX_IS:
			{
				conditionnum++;
				flag_is = true;
				const char* right = "jz", * wrong = "jnz"; 
				int j = i;
				while (LEXEMA(j) != LEX_SQ_RBRACELET)
				{
					if (LEXEMA(j) == LEX_ISTRUE)
					{
						flag_true = true;
					}
					else if (LEXEMA(j) == LEX_ISFALSE)
					{
						flag_false = true;
					}
					j++;
				}
				if (LEXEMA(j + 1) == LEX_ISTRUE || LEXEMA(j + 1) == LEX_ISFALSE)
				{
					flag_true = true;
					flag_false = true;
				}
				switch (LEXEMA(i + 2))
				{
				case LEX_MORE:
					right = "jg";  wrong = "jle";
					break;
				case LEX_LESS:
					right = "jl";  wrong = "jge";
					break;
				case LEX_EQUALS:
					right = "jz";  wrong = "jnz";
					break;
				case LEX_NOTEQUALS:
					right = "jnz";  wrong = "jz";
					break;
				}
				if (ITENTRY(i + 1).iddatatype == IT::IDDATATYPE::INT)
				{
					ofile << "\tmov edx, " << ITENTRY(i + 1).id << "\n\tcmp edx, " << ITENTRY(i + 3).id << "\n";


				}
				if (ITENTRY(i + 1).iddatatype == IT::IDDATATYPE::SYM || ITENTRY(i + 1).iddatatype == IT::IDDATATYPE::STR)
				{
					if (ITENTRY(i + 1).idtype == IT::IDTYPE::V || ITENTRY(i + 1).idtype == IT::IDTYPE::P)
						ofile << "\n\tmov esi, " << ITENTRY(i + 1).id;
					else
						ofile << "\n\tmov esi, offset " << ITENTRY(i + 1).id;

					if (ITENTRY(i + 3).idtype == IT::IDTYPE::V || ITENTRY(i + 3).idtype == IT::IDTYPE::P)
						ofile << "\n\tmov edi, " << ITENTRY(i + 3).id;
					else
						ofile << "\n\tmov edi, offset " << ITENTRY(i + 3).id;

					if (ITENTRY(i + 1).idtype == IT::IDTYPE::V || ITENTRY(i + 1).idtype == IT::IDTYPE::P)
						ofile << "\n\t push " << ITENTRY(i + 1).id;
					else
						ofile << "\n\t push offset " << ITENTRY(i + 1).id;

					ofile << "\n\t call lenght";
					ofile << "\n\t mov ebx,eax";

					if (ITENTRY(i + 1).idtype == IT::IDTYPE::V || ITENTRY(i + 3).idtype == IT::IDTYPE::P)
						ofile << "\n\t push " << ITENTRY(i + 3).id;
					else
						ofile << "\n\t push offset " << ITENTRY(i + 3).id;

					ofile << "\n\t call lenght";
					ofile << "\n\t cmp ebx,eax";
					if (flag_false)
						ofile << "\n\t" << wrong << " wrong" << conditionnum;
					else
						ofile << "\n\t jne next" << conditionnum;
					ofile << "\n\t mov ecx,eax";
					ofile << "\n\t repe cmpsb\n";
				}
				if (flag_true)
					ofile << "\t" << right << " right" << conditionnum << "\n";
				if (flag_false)
					ofile << "\t" << wrong << " wrong" << conditionnum << "\n";
				ofile << "\t" << "jmp next" << conditionnum << "\n";
				i += 2;
				break;
			}
			case LEX_ISTRUE:
			{
				ofile << "right" << conditionnum << ":";
				break;
			}
			case LEX_ISFALSE:
			{
				ofile << "wrong" << conditionnum << ":";
				break;
			}
			case LEX_SQ_RBRACELET:
			{
				if (flag_cycle)
				{
					bool isRepeat = false;
					for (int k = i; k >= 0 && k > i - 50; k--)
					{
						if (LEXEMA(k) == LEX_REPEAT)
						{
							isRepeat = true;
							break;
						}
					}
					if (isRepeat)
					{
						ofile << "\tdec esi\n"; 
						ofile << "\tcmp esi, 0\n"; 
						ofile << "\tjne cycle" << cyclenum << "\n";
						ofile << "continue" << cyclenum << ":\n";
					}
					else
					{
						ofile << cyclecode;
						ofile << "continue" << cyclenum << ":";
					}
					flag_cycle = false;
				}
				if (flag_is)
				{
					if (LEXEMA(i + 1) != LEX_ISTRUE && LEXEMA(i + 1) != LEX_ISFALSE)
					{
						ofile << "\nnext" << conditionnum << ":";
						flag_is = false;
						flag_true = false;
						flag_false = false;
					}
					else
					{
						ofile << "\tjmp next" << conditionnum << "\n\n";
					}
				}
				break;
			}
			case LEX_WHILE:
			{
				flag_cycle = true;
				cyclecode.clear();
				cyclenum++;
				const char* right = "jz", * wrong = "jnz"; 
				switch (LEXEMA(i + 2))
				{
				case LEX_MORE:
					right = "jg";  wrong = "jle";
					break;
				case LEX_LESS:
					right = "jl";  wrong = "jge";
					break;
				case LEX_EQUALS:
					right = "jz";  wrong = "jnz";
					break;
				case LEX_NOTEQUALS:
					right = "jnz";  wrong = "jz";
					break;
				}
				if (ITENTRY(i + 1).iddatatype == IT::IDDATATYPE::INT)
				{
					cyclecode = "\tmov edx, " + (string)ITENTRY(i + 1).id + "\n\tcmp edx, " + (string)ITENTRY(i + 3).id + "\n";
				}
				if (ITENTRY(i + 1).iddatatype == IT::IDDATATYPE::SYM || ITENTRY(i + 1).iddatatype == IT::IDDATATYPE::STR)
				{
					if (ITENTRY(i + 1).idtype == IT::IDTYPE::V || ITENTRY(i + 1).idtype == IT::IDTYPE::P)
						cyclecode += "\tmov esi, " + (string)ITENTRY(i + 1).id;
					else
						cyclecode += "\tmov esi, offset" + (string)ITENTRY(i + 1).id;
					if (ITENTRY(i + 3).idtype == IT::IDTYPE::V || ITENTRY(i + 3).idtype == IT::IDTYPE::P)
						cyclecode += "\n\tmov edi, " + (string)ITENTRY(i + 3).id + "\n";
					else
						cyclecode += "\n\tmov edi, offset" + (string)ITENTRY(i + 3).id + "\n";

					if (ITENTRY(i + 1).idtype == IT::IDTYPE::V || ITENTRY(i + 1).idtype == IT::IDTYPE::P)
						cyclecode += "\n\t push " + (string)ITENTRY(i + 1).id;
					else
						cyclecode += "\n\t push offset" + (string)ITENTRY(i + 1).id;

					cyclecode += "\n\t call lenght";
					cyclecode += "\n\t mov ebx,eax";

					if (ITENTRY(i + 3).idtype == IT::IDTYPE::V || ITENTRY(i + 3).idtype == IT::IDTYPE::P)
						cyclecode += "\n\t push " + (string)ITENTRY(i + 3).id;
					else
						cyclecode += "\n\t push offset" + (string)ITENTRY(i + 3).id;
					cyclecode += "\n\t call lenght";
					cyclecode += "\n\t cmp ebx,eax";
					cyclecode += "\n\t" + (string)wrong + " continue" + std::to_string(cyclenum);
					cyclecode += "\n\t mov ecx,eax";
					cyclecode += "\n\t repe cmpsb\n";
				}
				cyclecode += "\t" + (string)right + " cycle" + std::to_string(cyclenum) + "\n";
				ofile << cyclecode;
				ofile << "\t" << "jmp continue" << cyclenum << "\n";
				i += 2;
				break;
			}
			case LEX_DO:
			{
				break;
			}
			case LEX_REPEAT:
			{
				flag_cycle = true;
				cyclecode.clear();
				cyclenum++;

				if (ITENTRY(i + 1).idtype == IT::IDTYPE::L)
				{
					short repeatCount = ITENTRY(i + 1).value.vint;
					ofile << "\tmov esi, " << repeatCount << "\n";
				}
				else
				{
					ofile << "\tmovsx esi, word ptr " << ITENTRY(i + 1).id << "\n";
				}

				ofile << "cycle" << cyclenum << ":\n";
				break;
			}
			case LEX_TIMES:
			{
				break;
			}

			}
		}
		ofile << END;
		ofile.flush();
		ofile.close();
		return true;
	};
}