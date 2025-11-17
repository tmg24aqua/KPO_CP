#include "pch.h"
using namespace std;

namespace Polish
{
	int getPriority(unsigned char e)
	{
		switch (e)
		{
		case LEX_LEFTHESIS: case LEX_RIGHTTHESIS: return 0;
		case LEX_COMMA: return 1;
		case LEX_PLUS: case LEX_MINUS: return 2;
		case LEX_STAR: case LEX_DIRSLASH: case LEX_PROCENT: return 3;
		case '@': return 4; // вызов функции - высший приоритет
		default: return -1;
		}
	}

	int searchExpression(Lex::LEX lex) {
		bool fl = false;
		Lex::LEX result;
		result.lextable = LT::Create(lex.lextable.size);
		result.idtable = IT::Create(lex.idtable.size);

		for (int i = 0, k = 0; i < lex.lextable.size; i++)
		{
			if (lex.lextable.table[i].lexema == LEX_EQUAL) {
				lex.lextable.size = PolishNotation(++i, lex);
			}
		}
		return lex.lextable.size;
	}

	int PolishNotation(int lextable_pos, Lex::LEX& lex)
	{
		stack<LT::Entry> stack;														// стек для операций
		queue<LT::Entry> queue;														// очередь для операндов
		LT::Entry temp;		temp.idxTI = -1;	temp.lexema = '#';	temp.sn = -1;	// запрещенная лексема
		LT::Entry func;		func.idxTI = -1;	func.lexema = '@';	func.sn = -1;	// лексема для вызова функции
		LT::Entry tilda;	tilda.idxTI = -1;	tilda.lexema = '~';	tilda.sn = -1;	// лексема для отрицательных

		int countLex = 0;															// количество преобразованных лексем
		int countParm = 0;															// количество параметров функции
		int posLex = lextable_pos;													// позиция в таблице лексем
		bool findFunc = false;														// флаг для функции
		bool flagthethis = false;													// флаг для унарного минуса
		LT::Entry funcBuffer;														// буфер для хранения идентификатора функции

		// Обработка выражения до разделителя или конца блока
		for (int i = lextable_pos;
			lex.lextable.table[i].lexema != LEX_SEPARATOR &&
			lex.lextable.table[i].lexema != LEX_BRACELET &&
			lex.lextable.table[i].lexema != LEX_SQ_RBRACELET;
			i++, countLex++)
		{
			switch (lex.lextable.table[i].lexema)
			{
			case LEX_ID:															// если идентификатор
			{
				// Проверяем, является ли идентификатор функцией
				if (lex.idtable.table[lex.lextable.table[i].idxTI].idtype == IT::F)
				{
					findFunc = true;
					funcBuffer = lex.lextable.table[i]; // сохраняем идентификатор функции
				}
				else
				{
					queue.push(lex.lextable.table[i]);
					if (findFunc) countParm++;
				}
				continue;
			}
			case LEX_LITERAL:														// если литерал
			{
				queue.push(lex.lextable.table[i]);
				if (findFunc) countParm++;
				continue;
			}
			case LEX_LEFTHESIS:														// если (
			{
				// Обработка унарного минуса: (-identifier)
				if (lex.lextable.table[i + 1].lexema == LEX_MINUS &&
					lex.lextable.table[i + 2].lexema == LEX_ID)
				{
					flagthethis = true;
					stack.push(lex.lextable.table[i]); // помещаем ( в стек
					continue;
				}

				stack.push(lex.lextable.table[i]);									// помещаем ( в стек
				continue;
			}
			case LEX_RIGHTTHESIS:													// если )
			{
				if (findFunc)														// если это вызов функции
				{
					// Добавляем идентификатор функции в очередь
					queue.push(funcBuffer);

					// Добавляем лексему вызова функции
					func.sn = lex.lextable.table[i].sn;
					queue.push(func);

					// Добавляем количество параметров
					char* buf = new char[10];
					_itoa_s(countParm, buf, 10, 10);
					temp.lexema = buf[0];
					temp.idxTI = -1;
					temp.sn = lex.lextable.table[i].sn;
					queue.push(temp);

					findFunc = false;
					countParm = 0;
				}
				else
				{
					// Обычная обработка скобок
					while (!stack.empty() && stack.top().lexema != LEX_LEFTHESIS)
					{
						queue.push(stack.top());
						stack.pop();
					}
					if (!stack.empty()) stack.pop(); // удаляем (
				}
				continue;
			}
			case LEX_PLUS:															// сложение
			case LEX_MINUS:															// вычитание
			case LEX_STAR:															// умножение
			case LEX_DIRSLASH:														// деление
			case LEX_PROCENT:														// остаток от деления
			{
				if (flagthethis) // обработка унарного минуса
				{
					tilda.sn = lex.lextable.table[i].sn;
					queue.push(lex.lextable.table[i + 1]); // идентификатор
					queue.push(tilda); // унарный минус
					flagthethis = false;
					i += 2;
					countLex += 2;
					continue;
				}

				// Обработка бинарных операторов
				while (!stack.empty() &&
					getPriority(lex.lextable.table[i].lexema) <= getPriority(stack.top().lexema))
				{
					queue.push(stack.top());
					stack.pop();
				}
				stack.push(lex.lextable.table[i]);
				continue;
			}
			case LEX_EQUALS:    // равно &
			case LEX_NOTEQUALS: // не равно !
			case LEX_LESS:      // меньше <
			case LEX_MORE:      // больше >
			{
				// Операторы сравнения имеют низкий приоритет
				while (!stack.empty() &&
					getPriority(lex.lextable.table[i].lexema) <= getPriority(stack.top().lexema))
				{
					queue.push(stack.top());
					stack.pop();
				}
				stack.push(lex.lextable.table[i]);
				continue;
			}
			case LEX_COMMA:															// запятая
			{
				if (findFunc) countParm++;
				continue;
			}
			default:
				// Прочие лексемы (write, writeline, и т.д.) не обрабатываются в выражениях
				continue;
			}
		}

		// Выталкиваем оставшиеся операторы из стека
		while (!stack.empty())
		{
			if (stack.top().lexema != LEX_LEFTHESIS && stack.top().lexema != LEX_RIGHTTHESIS)
			{
				queue.push(stack.top());
			}
			stack.pop();
		}

		// Заменяем исходное выражение польской записью в таблице лексем
		while (countLex > 0)
		{
			if (!queue.empty())
			{
				lex.lextable.table[posLex++] = queue.front();
				cout << lex.lextable.table[posLex - 1].lexema << " "; // отладочный вывод
				queue.pop();
			}
			else
			{
				lex.lextable.table[posLex++] = temp; // заполняем пустые места
			}
			countLex--;
		}

		// Восстановление индексов первого вхождения для литералов
		for (int i = lextable_pos; i < posLex; i++)
		{
			if (lex.lextable.table[i].lexema == LEX_LITERAL && lex.lextable.table[i].idxTI != -1)
			{
				lex.idtable.table[lex.lextable.table[i].idxTI].idxfirstLE = i;
			}
		}

		return posLex; // возвращаем новую позицию в таблице лексем
	}
}