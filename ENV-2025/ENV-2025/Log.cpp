#include "pch.h"
namespace Log
{

	LOG getlog(wchar_t logfile[])
	{
		LOG temp;
		temp.stream = new std::ofstream(logfile);
		if (!temp.stream->is_open())
			throw ERROR_THROW(112);
		wcscpy_s(temp.logfile, logfile);
		return temp;
	}

	void WriteLine(LOG log, const char * c, ...)
	{
		const char **ptr = &c;
		while (strlen(*ptr) > 0)
		{
			*log.stream << *ptr;
			++ptr;
		}
		*log.stream << std::endl;
	}
	void WriteLineConsole(const char * c, ...)
	{
		const char** ptr = &c;
		char* result = nullptr;
		size_t size = 0;
		size_t capacity = 15;
		result = new char[capacity];
		result[0] = '\0';
		
		while (strcmp(*ptr, "") != 0)
		{
			size_t slen = strlen(*ptr);
			if (size + slen + 1 >= capacity)
			{
				capacity = (size + slen + 1) * 2;
				char* new_result = new char[capacity];
				size_t old_size = strlen(result);
				strcpy_s(new_result, capacity, result);
				delete[] result;
				result = new_result;
			}
			strcat_s(result, capacity, *ptr);
			size = strlen(result); 
			ptr++;
		}
		std::cout << result << std::endl;
		delete[] result; 
	}

	void WriteLine(LOG log, const wchar_t * c, ...)
	{
		const wchar_t **ptr = &c;
		while (wcslen(*ptr))
		{
			char out[PARM_MAX_SIZE];
			size_t charsConverted = 0;								
			wcstombs_s(&charsConverted, out, *ptr, PARM_MAX_SIZE);	
			*log.stream << out;											
			++ptr;
		}
		*log.stream << std::endl;
	}

	void WriteLog(LOG log)
	{
		time_t  t = time(nullptr);									
		tm  now ;													
		localtime_s(&now, &t);										
		char date[PARM_MAX_SIZE];									
		strftime(date, PARM_MAX_SIZE, "%d.%m.%Y %H:%M:%S", &now);	
		*log.stream << "---- Протокол ------- Дата: " << date << std::endl;
	}

	void WriteParm(LOG log, Parm::PARM parm)
	{
		*log.stream << "---- Параметры -------" << std::endl;
		char out[PARM_MAX_SIZE];
		size_t charsConverted(0);
		wcstombs_s(&charsConverted, out, parm.log, PARM_MAX_SIZE);
		*log.stream << "-log: " << out << std::endl;
		wcstombs_s(&charsConverted, out, parm.out, PARM_MAX_SIZE);
		*log.stream << "-out: " << out << std::endl;
		wcstombs_s(&charsConverted, out, parm.in, PARM_MAX_SIZE);
		*log.stream << "-in: " << out << std::endl;
	}

	void WriteIn(LOG log, In::IN in)
	{
		*log.stream << "---- Исходные данные -----" << std::endl;
		*log.stream << "Количество символов: " << in.size << std::endl;
		*log.stream << "Проигнорировано:     " << in.ignor << std::endl;
		*log.stream << "Количество строк:    " << in.lines << std::endl;
	}
	void WriteError(std::ostream *stream, Error::ERROR e)
	{
		if (stream == nullptr || stream == NULL)
		{
			if (e.position.col == -1 || e.position.line == -1)
				std::cout << std::endl << "Ошибка N" << e.id << ": " << e.message << std::endl;
			else
				std::cout << std::endl << "Ошибка N" << e.id << ": " << e.message
				<< " Строка: " << e.position.line
				<< " Позиция в строке: " << e.position.col << std::endl;
			system("pause");
		}
		else
		{
			try
			{
				if (e.position.col == -1 || e.position.line == -1)
					*stream << std::endl << "Ошибка N" << e.id << ": " << e.message;
				else
					*stream << std::endl << "Ошибка N" << e.id << ": " << e.message
					<< " Строка: " << e.position.line
					<< " Позиция в строке: " << e.position.col;
				*stream << std::endl;
			}
			catch (...)
			{
					if (e.position.col == -1 || e.position.line == -1)
					std::cout << std::endl << "Ошибка N" << e.id << ": " << e.message << std::endl;
				else
					std::cout << std::endl << "Ошибка N" << e.id << ": " << e.message
					<< " Строка: " << e.position.line
					<< " Позиция в строке: " << e.position.col << std::endl;
			}
		}
	}

	void Close(LOG log)
	{
		if (log.stream != nullptr)									
		{
			log.stream->close();
			delete log.stream;
		}
	}

}