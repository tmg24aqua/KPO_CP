#pragma once
#include "pch.h"
#define GRB_ERROR_SERIES 600
#define NS(n)	GRB::Rule::Chain::N(n)
#define TS(n)	GRB::Rule::Chain::T(n)
#define ISNS(n)	GRB::Rule::Chain::isN(n)

namespace GRB
{
	Greibach greibach(NS('S'), TS('$'), 11,

		// Правила для стартового символа S (программа)
		// S ? function F E S | main { K } | ?
		Rule(NS('S'), GRB_ERROR_SERIES, 3,
			Rule::Chain(3, TS('f'), NS('F'), NS('E')),      // объявление функции
			Rule::Chain(4, TS('m'), TS('{'), NS('K'), TS('}')), // главная функция
			Rule::Chain(1, TS('$'))  // пустое правило
		),

		// Правила для объявления функции F
		// F ? type identifier F | type identifier
		Rule(NS('F'), GRB_ERROR_SERIES + 1, 2,
			Rule::Chain(3, TS('t'), TS('i'), NS('F')),  // тип и идентификатор с продолжением
			Rule::Chain(2, TS('t'), TS('i'))           // тип и идентификатор
		),

		// Правила для параметров функции E
		// E ? ( P ) | ( )
		Rule(NS('E'), GRB_ERROR_SERIES + 2, 2,
			Rule::Chain(3, TS('('), NS('P'), TS(')')),  // параметры в скобках
			Rule::Chain(2, TS('('), TS(')'))           // пустые параметры
		),

		// Правила для списка параметров P
		// P ? type identifier , P | type identifier
		Rule(NS('P'), GRB_ERROR_SERIES + 3, 2,
			Rule::Chain(4, TS('t'), TS('i'), TS(','), NS('P')), // параметр с разделителем
			Rule::Chain(2, TS('t'), TS('i'))                   // последний параметр
		),

		// Правила для тела функции K
		// K ? type type identifier = W ; K | type type identifier ; K | identifier = W ; K | write V ; K | 
		//      repeat identifier times [ X ] K | is Z Y K | return V ; | ?
		Rule(NS('K'), GRB_ERROR_SERIES + 4, 8,
			Rule::Chain(7, TS('n'), TS('t'), TS('i'), TS('='), NS('W'), TS(';'), NS('K')), // объявление с инициализацией
			Rule::Chain(5, TS('n'), TS('t'), TS('i'), TS(';'), NS('K')),                   // объявление без инициализации
			Rule::Chain(5, TS('i'), TS('='), NS('W'), TS(';'), NS('K')),                   // присваивание
			Rule::Chain(4, TS('o'), NS('V'), TS(';'), NS('K')),                            // вывод
			Rule::Chain(6, TS('c'), TS('i'), TS('x'), TS('['), NS('X'), TS(']'), NS('K')), // цикл repeat
			Rule::Chain(4, TS('?'), NS('Z'), NS('Y'), NS('K')),                            // условный оператор is
			Rule::Chain(3, TS('e'), NS('V'), TS(';')),                                     // возврат из функции
			Rule::Chain(1, TS('$'))  // конец тела функции
		),

		// Правила для выражений W
		// W ? identifier | literal | ( W ) | W A W | U F
		Rule(NS('W'), GRB_ERROR_SERIES + 5, 5,
			Rule::Chain(1, TS('i')),                              // идентификатор
			Rule::Chain(1, TS('l')),                              // литерал
			Rule::Chain(3, TS('('), NS('W'), TS(')')),            // выражение в скобках
			Rule::Chain(3, NS('W'), NS('A'), NS('W')),            // бинарная операция
			Rule::Chain(2, NS('U'), NS('F'))                      // вызов функции
		),

		// Правила для арифметических операторов A
		// A ? + | - | * | / | %
		Rule(NS('A'), GRB_ERROR_SERIES + 6, 5,
			Rule::Chain(1, TS('+')),  // сложение
			Rule::Chain(1, TS('-')),  // вычитание
			Rule::Chain(1, TS('*')),  // умножение
			Rule::Chain(1, TS('/')),  // деление
			Rule::Chain(1, TS('%'))   // остаток от деления
		),

		// Правила для простых выражений V
		// V ? identifier | literal | true | false
		Rule(NS('V'), GRB_ERROR_SERIES + 7, 4,
			Rule::Chain(1, TS('i')),  // идентификатор
			Rule::Chain(1, TS('l')),  // литерал
			Rule::Chain(1, TS('r')),  // логическое true
			Rule::Chain(1, TS('w'))   // логическое false
		),

		// Правила для условий Z
		// Z ? identifier L identifier | identifier L literal | literal L identifier | literal L literal
		Rule(NS('Z'), GRB_ERROR_SERIES + 8, 4,
			Rule::Chain(3, TS('i'), NS('L'), TS('i')),  // идентификатор оператор идентификатор
			Rule::Chain(3, TS('i'), NS('L'), TS('l')),  // идентификатор оператор литерал
			Rule::Chain(3, TS('l'), NS('L'), TS('i')),  // литерал оператор идентификатор
			Rule::Chain(3, TS('l'), NS('L'), TS('l'))   // литерал оператор литерал
		),

		// Правила для операторов сравнения L
		// L ? & | ! | < | >
		Rule(NS('L'), GRB_ERROR_SERIES + 9, 4,
			Rule::Chain(1, TS('&')),  // равно
			Rule::Chain(1, TS('!')),  // не равно
			Rule::Chain(1, TS('<')),  // меньше
			Rule::Chain(1, TS('>'))   // больше
		),

		// Правила для тела условного оператора Y
		// Y ? [ X ] | [ X ] Y
		Rule(NS('Y'), GRB_ERROR_SERIES + 10, 2,
			Rule::Chain(3, TS('['), NS('X'), TS(']')),        // один блок
			Rule::Chain(4, TS('['), NS('X'), TS(']'), NS('Y')) // несколько блоков
		),

		// Правила для блоков кода X
		// X ? identifier = W ; X | write V ; X | repeat identifier times [ X ] X | ?
		Rule(NS('X'), GRB_ERROR_SERIES + 11, 4,
			Rule::Chain(5, TS('i'), TS('='), NS('W'), TS(';'), NS('X')),                   // присваивание в блоке
			Rule::Chain(4, TS('o'), NS('V'), TS(';'), NS('X')),                            // вывод в блоке
			Rule::Chain(6, TS('c'), TS('i'), TS('x'), TS('['), NS('X'), TS(']'), NS('X')), // цикл в блоке
			Rule::Chain(1, TS('$'))  // конец блока
		),

		// Правила для идентификаторов функций U
		// U ? identifier | p (стандартная функция)
		Rule(NS('U'), GRB_ERROR_SERIES + 12, 2,
			Rule::Chain(1, TS('i')),  // пользовательская функция
			Rule::Chain(1, TS('p'))   // стандартная функция
		)

	);
}