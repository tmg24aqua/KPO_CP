#include "pch.h"

int _tmain(int argc, _TCHAR** argv)
{
	char MESSAGE[] = "--------------------КОНЕЧНЫЕ ТАБЛИЦЫ ЛЕКСЕМ И ИДЕНТИФИКАТОРОВ-------------------\n";
	setlocale(LC_ALL, "rus");
	Log::LOG log = Log::INITLOG;

	In::IN in;
	in.text = nullptr;
	in.words = nullptr;
	in.size = 0;
	in.lines = 0;
	in.ignor = 0;

	try
	{
		Parm::PARM parm = Parm::getparm(argc, argv);
		log = Log::getlog(parm.log);
		Log::WriteLog(log);
		Log::WriteParm(log, parm);

		in = In::getin(parm.in, log.stream);
		Log::WriteIn(log, in);

		in.words = In::getWordsTable(log.stream, in.text, in.code, in.size);
		Lex::LEX tables;

		bool lex_ok = Lex::analyze(tables, in, log, parm);

		LT::writeLexTable(log.stream, tables.lextable);
		IT::writeIdTable(log.stream, tables.idtable);
		LT::writeLexemsOnLines(log.stream, tables.lextable);

		std::cout << "---------------- ТАБЛИЦЫ ПОСЛЕ ЛЕКСИЧЕСКОГО АНАЛИЗА ----------------" << std::endl;
		LT::writeLexTable(&std::cout, tables.lextable);
		IT::writeIdTable(&std::cout, tables.idtable);
		LT::writeLexemsOnLines(&std::cout, tables.lextable);
		std::cout << "--------------------------------------------------------------------" << std::endl;

		if (!lex_ok)
		{
			Log::WriteLine(log, "Лексический анализ завершен с ошибками", "");
			std::cout << "Лексический анализ завершен с ошибками" << std::endl;

			if (in.text != nullptr) delete[] in.text;
			if (in.words != nullptr) delete[] in.words;
			Log::Close(log);
			system("pause");
			return 0;
		}
		Log::WriteLineConsole("Лексический анализ завершен без ошибок", "");


		bool sem_ok = Semantic::semanticsCheck(tables, log);
		if (!sem_ok)
		{
			Log::WriteLine(log, "Обнаружены семантические ошибки", "");
			std::cout << "Обнаружены семантические ошибки" << std::endl;

			if (in.text != nullptr) delete[] in.text;
			if (in.words != nullptr) delete[] in.words;
			Log::Close(log);
			system("pause");
			return 0;
		}
		Log::WriteLineConsole("Семантический анализ завершен без ошибок", "");


		MFST_TRACE_START(log.stream);
		MFST::Mfst mfst(tables, GRB::getGreibach());
		bool synt_ok = mfst.start(log);
		mfst.savededucation();
		mfst.printrules(log);

		if (!synt_ok)
		{
			Log::WriteLine(log, "Синтаксический анализ завершен с ошибками", "");
			std::cout << "Синтаксический анализ завершен с ошибками" << std::endl;

			if (in.text != nullptr) delete[] in.text;
			if (in.words != nullptr) delete[] in.words;
			Log::Close(log);
			system("pause");
			return 0;
		}
		Log::WriteLineConsole("Синтаксический анализ завершен без ошибок", "");


		tables.lextable.size = Polish::searchExpression(tables);
		if (tables.lextable.size == 0)
		{
			Log::WriteLine(log, "Ошибка при попытке преобразования выражения", "");
			std::cout << "Ошибка при попытке преобразования выражения" << std::endl;

			if (in.text != nullptr) delete[] in.text;
			if (in.words != nullptr) delete[] in.words;
			Log::Close(log);
			system("pause");
			return 0;
		}
		Log::WriteLineConsole("\nПреобразование выражений завершено без ошибок", "");


		Log::WriteLine(log, MESSAGE, "");
		LT::writeLexTable(log.stream, tables.lextable);
		IT::writeIdTable(log.stream, tables.idtable);
		LT::writeLexemsOnLines(log.stream, tables.lextable);

		Log::WriteLineConsole(MESSAGE, "");
		IT::writeIdTable(&std::cout, tables.idtable);
		LT::writeLexTable(&std::cout, tables.lextable);
		LT::writeLexemsOnLines(&std::cout, tables.lextable);


		bool gen_ok = Gener::CodeGeneration(tables, parm, log);
		if (!gen_ok)
		{
			Log::WriteLine(log, "Ошибка генерации кода", "");
			std::cout << "Ошибка генерации кода" << std::endl;

			if (in.text != nullptr) delete[] in.text;
			if (in.words != nullptr) delete[] in.words;
			Log::Close(log);
			system("pause");
			return 0;
		}

		Log::WriteLine(log, "Программа успешно завершена!", "");
		Log::WriteLineConsole("Программа успешно завершена!", "");

		if (in.text != nullptr) delete[] in.text;
		if (in.words != nullptr) delete[] in.words;
		Log::Close(log);
		system("pause");
	}
	catch (Error::ERROR e)
	{
		Log::WriteError(log.stream, e);
		if (e.id >= 200 && e.id < 300)
		{
			std::cout << "Лексический анализ завершен с ошибками" << std::endl;
		}
		else if (e.id >= 300 && e.id < 400)
		{
			std::cout << "Обнаружены семантические ошибки" << std::endl;
		}
		else if (e.id >= 600 && e.id < 700)
		{
			std::cout << "Синтаксический анализ завершен с ошибками" << std::endl;
		}
		else
		{
			std::cout << "Выполнение программы остановлено из-за ошибки (см. log)" << std::endl;
		}

		if (in.text != nullptr) delete[] in.text;
		if (in.words != nullptr) delete[] in.words;
		if (log.stream != nullptr) Log::Close(log);
		system("pause");
	}
	catch (...)
	{
		std::cout << "Ошибка: Системный сбой" << std::endl;
		if (in.text != nullptr) delete[] in.text;
		if (in.words != nullptr) delete[] in.words;
		if (log.stream != nullptr) Log::Close(log);
		system("pause");
	}
	return 0;
}