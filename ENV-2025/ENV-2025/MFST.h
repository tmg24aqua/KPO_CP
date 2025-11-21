// MFST.h - Магазинный автомат (MFST) для синтаксического анализа
//
// Магазинный автомат используется для проверки соответствия программы
// грамматике языка ENV-2025 в нормальной форме Грейбах.
// Автомат работает по принципу: читает ленту (последовательность лексем),
// использует стек для хранения нетерминалов грамматики и применяет правила
// грамматики для разбора программы.
//
// Основные компоненты:
// - lenta: лента входных символов (лексемы из таблицы лексем)
// - st: стек автомата (хранит нетерминалы и терминалы)
// - storestate: стек для сохранения состояний (для возврата при ошибках)
// - grebach: грамматика в нормальной форме Грейбах

#pragma once
#pragma once
#include "pch.h"

#define MFST_DIAGN_MAXSIZE 2*ERROR_MAXSIZE_MESSAGE  // Максимальный размер диагностического сообщения
#define MFST_DIAGN_NUMBER 3                         // Количество сохраняемых диагностических сообщений

#define MFST_TRACE_START(LOG) *log.stream << std::setw( 4)<<std::left<<"Шаг"<<":" \
	<< std::setw(20) << std::left << "Правило"  \
	<< std::setw(30) << std::left << "Входная лента" \
	<< std::setw(20) << std::left << "Стек" \
	<< std::endl;

#define MFST_TRACE1(LOG) *log.stream <<std::setw( 4)<<std::left<<++FST_TRACE_n<<":" \
	<< std::setw(20) << std::left << rule.getCRule(rbuf, nrulechain)  \
	<< std::setw(30) << std::left << getCLenta(lbuf, lenta_position) \
	<< std::setw(20) << std::left << getCSt(sbuf) \
	<< std::endl;

#define MFST_TRACE2(LOG)    *log.stream <<std::setw( 4)<<std::left<<FST_TRACE_n<<":" \
	<< std::setw(20) << std::left << " "  \
	<< std::setw(30) << std::left << getCLenta(lbuf, lenta_position) \
	<< std::setw(20) << std::left << getCSt(sbuf) \
	<< std::endl;

#define MFST_TRACE3(LOG)     *log.stream<<std::setw( 4)<<std::left<<++FST_TRACE_n<<":" \
	<< std::setw(20) << std::left << " "  \
	<< std::setw(30) << std::left << getCLenta(lbuf, lenta_position) \
	<< std::setw(20) << std::left << getCSt(sbuf) \
	<< std::endl;

#define MFST_TRACE4(LOG, c) *log.stream<<std::setw(4)<<std::left<<++FST_TRACE_n<<": "<<std::setw(20)<<std::left<<c<<std::endl;
#define MFST_TRACE5(LOG, c) *log.stream<<std::setw(4)<<std::left<<  FST_TRACE_n<<": "<<std::setw(20)<<std::left<<c<<std::endl;

#define MFST_TRACE6(LOG,c,k) *log.stream<<std::setw(4)<<std::left<<++FST_TRACE_n<<": "<<std::setw(20)<<std::left<<c<<k<<std::endl;

#define MFST_TRACE7(LOG)  *log.stream<<std::setw(4)<<std::left<<state.lenta_position<<": "\
	<< std::setw(20) << std::left << rule.getCRule(rbuf, state.nrulechain) \
	<< std::endl;


typedef std::stack<short> MFSTSTACK;		// Стек автомата (хранит символы грамматики)

namespace MFST
{
	// Состояние автомата для сохранения и восстановления
	// Используется при возврате (backtracking) при неудачных попытках применить правило
	struct MfstState
	{
		short lenta_position;			// Позиция на входной ленте (какая лексема обрабатывается)
		short nrule;					// Номер текущего правила грамматики
		short nrulechain;				// Номер текущей цепочки правила (правая часть правила)
		MFSTSTACK st;					// Состояние стека автомата
		MfstState();
		MfstState(
			short pposition,			// позиция на ленте
			MFSTSTACK pst,				// стек автомата
			short pnrulechain			// номер текущей цепочки, текущего правила
		);
		MfstState(
			short pposition,			// позиция на ленте
			MFSTSTACK pst,				// стек автомата
			short pnrule,				// номер текущего правила
			short pnrulechain			// номер текущей цепочки, текущего правила
		);
	};

	// Магазинный автомат для синтаксического анализа
	// Реализует алгоритм разбора по грамматике в нормальной форме Грейбах
	struct Mfst
	{
		// Коды возврата функции step() - результат одного шага работы автомата
		enum RC_STEP {
			NS_OK,				// Найдено правило и цепочка, цепочка записана в стек (успешный шаг)
			NS_NORULE,			// Не найдено правило грамматики (ошибка в грамматике)
			NS_NORULECHAIN,		// Не найдена подходящая цепочка правила (ошибка в исходном коде)
			NS_ERROR,			// Неизвестный нетерминальный символ грамматики
			TS_OK,				// Текущий символ ленты == вершине стека, продвинулась лента, pop стека
			TS_NOK,				// Текущий символ ленты != вершине стека, восстановлено состояние
			LENTA_END,			// Текущая позиция ленты >= lenta_size (конец входной последовательности)
			SURPRISE			// Неожиданный код возврата (ошибка в реализации step)
		};

		// Диагностическая информация об ошибках синтаксического анализа
		// Сохраняет последние самые глубокие ошибки для вывода пользователю
		struct MfstDiagnosis
		{
			short lenta_position;		// Позиция на ленте, где произошла ошибка
			RC_STEP rc_step;			// Код завершения шага (тип ошибки)
			short nrule;				// Номер правила, которое пытались применить
			short nrule_chain;			// Номер цепочки правила
			MfstDiagnosis();
			MfstDiagnosis(
				short plenta_position,	// позиция на ленте
				RC_STEP prt_step,		// код завершения шага
				short pnrule,			// номер правила
				short pnrule_chain		// номер цепочки правила
			);
		} diagnosis[MFST_DIAGN_NUMBER];		// Массив последних самых глубоких диагностических сообщений

		GRBALPHABET* lenta;				// Перекодированная лента (лексемы преобразованы в символы грамматики)
		short lenta_position;			// Текущая позиция на ленте (какая лексема обрабатывается)
		short nrule;					// Номер текущего правила грамматики
		short nrulechain;				// Номер текущей цепочки правила
		short lenta_size;				// Размер ленты (количество лексем)
		GRB::Greibach grebach;			// Грамматика в нормальной форме Грейбах
		Lex::LEX lex;					// Результат работы лексического анализатора (таблицы лексем и идентификаторов)
		MFSTSTACK st;					// Стек автомата (хранит символы грамматики)
		std::stack<MfstState> storestate;	// Стек для сохранения состояний (для возврата при ошибках) 
		Mfst();
		Mfst(
			Lex::LEX plex,			//результат работы лексического анализатора
			GRB::Greibach pgrebach		//грамматика Грейбах
		);
		char* getCSt(char* buf);							// Получить содержимое стека в виде строки (для отладки)
		char* getCLenta(char* buf, short pos, short n = 25);	// Получить n символов ленты начиная с позиции pos
		const char* getDiagnosis(short n, char* buf);	// Получить n-ю строку диагностики или пустую строку
		bool savestate(const Log::LOG& log);				// Сохранить текущее состояние автомата в стек состояний
		bool reststate(const Log::LOG& log);			// Восстановить предыдущее состояние автомата из стека
		bool push_chain(								// Поместить цепочку правила в стек (обратном порядке)
			GRB::Rule::Chain chain						// Цепочка правила (правая часть правила грамматики)
		);
		RC_STEP step(const Log::LOG& log);				// Выполнить один шаг работы автомата
		bool start(const Log::LOG& log);					// Запустить автомат (выполнить полный синтаксический анализ)
		bool savediagnosis(								// Сохранить диагностическую информацию об ошибке
			RC_STEP pprc_step							// Код завершения шага (тип ошибки)
		);
		void printrules(const Log::LOG& log);			// Вывести последовательность применённых правил грамматики

		// Дерево вывода (последовательность применённых правил грамматики)
		// Используется для отслеживания того, какие правила были применены при разборе
		struct Deducation
		{
			short size;			// Количество шагов в выводе (количество применённых правил)
			short* nrules;		// Номера правил грамматики, которые были применены
			short* nrulechains;	// Номера цепочек правил грамматики (какая правая часть правила была выбрана)
			Deducation() { size = 0; nrules = 0; nrulechains = 0; };
		} deducation;

		bool savededucation();	// Сохранить дерево вывода (последовательность применённых правил)
	};
}