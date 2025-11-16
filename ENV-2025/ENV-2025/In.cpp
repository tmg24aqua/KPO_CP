#include "pch.h"
namespace In
{

	IN getin(wchar_t infile[], std::ostream* stream)
	{

		unsigned char* text = new unsigned char[IN_MAX_LEN_TEXT];
		std::ifstream instream(infile);
		if (!instream.is_open())
			throw ERROR_THROW(102);
		IN in;
		int pos = 0;
		bool isStringLiteral = false;
		bool isCharLiteral = false;
		while (true)
		{
			unsigned char uch = instream.get();
			if (instream.eof())
				break;

			// Обработка строковых и символьных литералов
			if (in.code[uch] == IN::Q) {              // строковый литерал
				isStringLiteral = !isStringLiteral;
			}
			else if (in.code[uch] == IN::A) {         // символьный литерал  
				isCharLiteral = !isCharLiteral;
			}

			switch (in.code[uch])
			{
			case IN::N:                               // новая строка 
				text[in.size++] = uch;
				in.lines++;
				pos = -1;
				break;
			case IN::L: // буквы
			case IN::D: // цифры
			case IN::H: // шестнадцатеричные цифры
			case IN::B: // двоичные цифры
			case IN::W: // пробелы и табуляции
			case IN::S: // разделители
			case IN::O: // операторы
			case IN::C: // символы сравнения
			case IN::U: // подчеркивание
			case IN::K: // символы для логических литералов
			case IN::Q: // кавычки для строк
			case IN::A: // апострофы для символов
				text[in.size++] = uch;
				break;
			case IN::I: // запрещенные символы
				if (isStringLiteral || isCharLiteral) {
					// В литералах разрешаем все символы
					text[in.size++] = uch;
				}
				else {
					Log::WriteError(stream, Error::geterrorin(200, in.lines, pos));
					in.ignor++;
				}
				break;
			default:
				text[in.size++] = uch;
			}
			pos++;
		}
		text[in.size] = IN_CODE_NULL;
		in.text = text;
		instream.close(); // закрыть поток 
		return in;
	}

	void addWord(InWord* words, char* word, int line) // добавить слово в таблицу
	{
		if (*word == IN_CODE_NULL)
			return;

		// Проверка длины идентификатора (ошибка 324)
		if (strlen(word) > ID_MAXSIZE &&
			!strchr(word, '\"') && !strchr(word, '\'') && // не строковый/символьный литерал
			!isdigit(word[0])) { // не числовой литерал
			// Ошибка будет обработана в основном потоке, здесь просто логируем
		}

		words[words->size].line = line;
		strcpy_s(words[words->size].word, strlen(word) + 1, word);
		words->size++;
	}

	InWord* getWordsTable(std::ostream* stream, unsigned char* text, int* code, int textSize)
	{
		InWord* words = new InWord[LT_MAXSIZE];       // слова
		words->size = 0;
		int bufpos = 0;                               // позиция в буфере
		int line = 1;                                 // номер строки исходного кода
		bool isStringLiteral = false;
		bool isCharLiteral = false;
		char buffer[MAX_LEN_BUFFER] = "";             // буфер

		for (int i = 0; i < textSize; i++)            // заполнение
		{
			// Обработка строковых и символьных литералов
			if (code[text[i]] == IN::Q) {
				isStringLiteral = !isStringLiteral;
			}
			else if (code[text[i]] == IN::A) {
				isCharLiteral = !isCharLiteral;
			}

			// Если внутри литерала - просто добавляем символы в буфер
			if (isStringLiteral || isCharLiteral) {
				buffer[bufpos++] = text[i];
				buffer[bufpos] = IN_CODE_NULL;

				// Проверка символьных литералов (ошибка 321)
				if (isCharLiteral && text[i] == IN_CODE_APOST && bufpos > 3) {
					// Символьный литерал должен содержать ровно один символ
					Log::WriteError(stream, Error::geterrorin(321, line, i));
				}

				// Если это закрывающая кавычка/апостроф - добавляем литерал в таблицу
				if ((isStringLiteral && text[i] == IN_CODE_QUOTE) ||
					(isCharLiteral && text[i] == IN_CODE_APOST)) {
					addWord(words, buffer, line);
					*buffer = IN_CODE_NULL;
					bufpos = 0;
				}
				continue;
			}

			switch (code[text[i]])
			{
			case IN::S: // разделители
			{
				// Обработка операторов сравнения (==, !=, <=, >=)
				if ((text[i] == '<' || text[i] == '>' || text[i] == '!') && text[i + 1] == '=') {
					if (*buffer != IN_CODE_NULL) {
						// Проверка длины идентификатора перед добавлением (ошибка 324)
						if (strlen(buffer) > ID_MAXSIZE) {
							Log::WriteError(stream, Error::geterrorin(324, line, i - strlen(buffer)));
						}
						addWord(words, buffer, line);
						*buffer = IN_CODE_NULL; bufpos = 0;
					}
					buffer[bufpos++] = text[i];
					buffer[bufpos++] = text[i + 1];
					buffer[bufpos] = IN_CODE_NULL;
					addWord(words, buffer, line);
					*buffer = IN_CODE_NULL;
					bufpos = 0;
					i++; // пропускаем следующий символ
					break;
				}

				// Обработка одиночных разделителей
				char letter[] = { (char)text[i], IN_CODE_NULL };
				if (*buffer != IN_CODE_NULL) {
					// Проверка длины идентификатора перед добавлением (ошибка 324)
					if (strlen(buffer) > ID_MAXSIZE) {
						Log::WriteError(stream, Error::geterrorin(324, line, i - strlen(buffer)));
					}
					addWord(words, buffer, line); // буфер перед разделителем
				}
				addWord(words, letter, line);     // сам разделитель
				*buffer = IN_CODE_NULL;
				bufpos = 0;
				break;
			}
			case IN::O: // операторы
			{
				// Обработка составных операторов (если нужны)
				char letter[] = { (char)text[i], IN_CODE_NULL };
				if (*buffer != IN_CODE_NULL) {
					// Проверка длины идентификатора перед добавлением (ошибка 324)
					if (strlen(buffer) > ID_MAXSIZE) {
						Log::WriteError(stream, Error::geterrorin(324, line, i - strlen(buffer)));
					}
					addWord(words, buffer, line); // буфер перед оператором
				}
				addWord(words, letter, line);     // сам оператор
				*buffer = IN_CODE_NULL;
				bufpos = 0;
				break;
			}
			case IN::W: // пробелы и табуляции
			case IN::N: // новая строка
				if (*buffer != IN_CODE_NULL) {
					// Проверка длины идентификатора перед добавлением (ошибка 324)
					if (strlen(buffer) > ID_MAXSIZE) {
						Log::WriteError(stream, Error::geterrorin(324, line, i - strlen(buffer)));
					}
					addWord(words, buffer, line);
					*buffer = IN_CODE_NULL;
					bufpos = 0;
				}
				if (code[text[i]] == IN::N) {
					line++;
				}
				break;
			case IN::Q: // начало строкового литерала
			case IN::A: // начало символьного литерала
			{
				if (*buffer != IN_CODE_NULL) {
					// Проверка длины идентификатора перед добавлением (ошибка 324)
					if (strlen(buffer) > ID_MAXSIZE) {
						Log::WriteError(stream, Error::geterrorin(324, line, i - strlen(buffer)));
					}
					addWord(words, buffer, line);
					*buffer = IN_CODE_NULL;
					bufpos = 0;
				}
				buffer[bufpos++] = text[i]; // добавляем открывающую кавычку
				break;
			}
			default: // буквы, цифры, идентификаторы
				buffer[bufpos++] = text[i];
				buffer[bufpos] = IN_CODE_NULL;

				// Проверка длины идентификатора в процессе набора (ошибка 324)
				if (bufpos > ID_MAXSIZE) {
					Log::WriteError(stream, Error::geterrorin(324, line, i));
					// Обрезаем идентификатор до максимальной длины
					buffer[ID_MAXSIZE] = IN_CODE_NULL;
					bufpos = ID_MAXSIZE;
				}
			}
		}

		// Добавляем последнее слово, если буфер не пуст
		if (*buffer != IN_CODE_NULL) {
			// Финальная проверка длины идентификатора (ошибка 324)
			if (strlen(buffer) > ID_MAXSIZE &&
				!strchr(buffer, '\"') && !strchr(buffer, '\'') &&
				!isdigit(buffer[0])) {
				Log::WriteError(stream, Error::geterrorin(324, line, textSize - strlen(buffer)));
			}
			addWord(words, buffer, line);
		}

		return words;
	}

	void printTable(InWord* table) // вывод таблицы на экран
	{
		std::cout << " ------------------ ТАБЛИЦА СЛОВ: ------------------" << std::endl;
		for (int i = 0; i < table->size; i++) {
			std::cout << std::setw(2) << i << std::setw(3) << table[i].line << " |  " << table[i].word << std::endl;
		}
		std::cout << "Всего слов: " << table->size << std::endl;
	}

}