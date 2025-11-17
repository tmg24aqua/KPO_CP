#pragma once
#include "pch.h"

typedef std::vector <LT::Entry> ltvec;
typedef std::vector <int> intvec;

namespace Polish
{
    int PolishNotation(int lextable_pos, Lex::LEX& lex);
    int getPriority(unsigned char e);
    int searchExpression(Lex::LEX lex);
};