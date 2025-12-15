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
		in.size = 0;
		in.lines = 1;
		in.ignor = 0;
		in.text = nullptr;
		in.words = nullptr;
		int code_table[256] = IN_CODE_TABLE;
		memcpy(in.code, code_table, sizeof(in.code));
		
		int pos = 0;
		bool isLiteral = false;
		while (true)
		{
			unsigned char uch = instream.get();
			if (instream.eof())
				break;
			
			if (in.size >= IN_MAX_LEN_TEXT - 1)
			{
				throw Error::geterrorin(204, in.lines, pos);
			}
			
			if (in.code[uch] == IN::Q)
				isLiteral = !isLiteral;
			switch (in.code[uch])
			{
			case IN::N:
				text[in.size++] = uch;
				in.lines++;
				pos = -1;
				break;
			case IN::L:
			case IN::D:
			case IN::H:
			case IN::B:
			case IN::O:
			case IN::C:
			case IN::S:
			case IN::Q:
			case IN::A:
			case IN::U:
			case IN::W:
			case IN::K:
				text[in.size++] = uch;
				break;
			case IN::I:
				throw Error::geterrorin(200, in.lines, pos);
			default:
				text[in.size++] = uch;
			}
			pos++;
		}
		if (in.size < IN_MAX_LEN_TEXT)
		text[in.size] = IN_CODE_NULL;
		else
			text[IN_MAX_LEN_TEXT - 1] = IN_CODE_NULL;
		in.text = text;
		instream.close();
		return in;
	}
	void addWord(InWord* words, char* word, int line, int col)
	{
		if (word == nullptr || *word == IN_CODE_NULL || words->size >= IN_WORDS_MAXSIZE - 1)
			return;
		words[words->size].line = line;
		words[words->size].col = col;
		size_t word_len = 0;
		for (size_t i = 0; i < MAX_LEN_BUFFER && word[i] != '\0'; i++)
		{
			word_len++;
		}
		if (word_len >= MAX_LEN_BUFFER)
		{
			word_len = MAX_LEN_BUFFER - 1;
		}
		if (word_len > 0)
		{
			memcpy(words[words->size].word, word, word_len);
		}
		words[words->size].word[word_len] = '\0';
		words->size++;
	}

	InWord* getWordsTable(std::ostream* stream, unsigned char* text, int* code, int textSize)
	{
		InWord* words = new InWord[IN_WORDS_MAXSIZE];
		words->size = 0;
		int bufpos = 0;
		int line = 1;
		int col = 0;
		int wordStartCol = 0;
		char buffer[MAX_LEN_BUFFER];
		memset(buffer, 0, MAX_LEN_BUFFER);
		for (int i = 0; i < textSize; i++)
		{
			if (code[text[i]] != IN::W && code[text[i]] != IN::N)
			{
				if (bufpos == 0)
				{
					wordStartCol = col;
				}
				col++;
			}
			else if (code[text[i]] == IN::W)
			{
				col++;
			}
			
			if (bufpos >= MAX_LEN_BUFFER - 1)
			{
				buffer[MAX_LEN_BUFFER - 1] = IN_CODE_NULL;
				addWord(words, buffer, line, wordStartCol);
				*buffer = IN_CODE_NULL;
				bufpos = 0;
			}
			
			switch (code[text[i]])
			{
			case IN::S:
			{
				if (code[text[i]] == IN::O)
				{
				}
				
				if (text[i] == IN_CODE_TILDA && i + 2 < textSize)
				{
					if (bufpos > 0 && buffer[0] != IN_CODE_NULL)
					{
						addWord(words, buffer, line, wordStartCol);
						memset(buffer, 0, MAX_LEN_BUFFER);
						bufpos = 0;
					}
					if (i + 2 < textSize && text[i + 2] == IN_CODE_TILDA)
					{
						if (bufpos < MAX_LEN_BUFFER - 3)
						{
							wordStartCol = col;
							buffer[bufpos++] = text[i];
							buffer[bufpos++] = text[i + 1];
							buffer[bufpos++] = text[i + 2];
							buffer[bufpos] = IN_CODE_NULL;
							addWord(words, buffer, line, wordStartCol);
							memset(buffer, 0, MAX_LEN_BUFFER);
							bufpos = 0;
							i += 2;
							break;
						}
					}
				}
				
				if ((i + 1 < textSize && text[i] == LEX_MINUS && isdigit(text[i + 1]))
					|| (words->size > 0 && text[i] == LEX_MINUS && words[words->size - 1].word[0] == '=')
					|| (i + 1 < textSize && text[i] == LEX_MINUS && text[i + 1] == LEX_MINUS && words->size > 0 && line > words[words->size - 1].line))
				{
					if (bufpos > 0 && buffer[0] != IN_CODE_NULL)
					{
						addWord(words, buffer, line, wordStartCol);
						memset(buffer, 0, MAX_LEN_BUFFER);
						bufpos = 0;
					}
					if (i + 1 < textSize && text[i + 1] == LEX_MINUS && words->size >= 2 && words[words->size - 1].line > words[words->size - 2].line)
					{
						wordStartCol = col;
						if (bufpos < MAX_LEN_BUFFER - 2)
					{
						buffer[bufpos++] = ':';
						buffer[bufpos++] = text[i];
						buffer[bufpos] = IN_CODE_NULL;
						}
						addWord(words, buffer, line, wordStartCol);
						memset(buffer, 0, MAX_LEN_BUFFER);
						bufpos = 0;
						i++;
						col++;
						break;
					}
					if ((words->size > 0 && isdigit(words[words->size - 1].word[strlen(words[words->size - 1].word) - 1]))
						|| (words->size > 0 && words[words->size - 1].word[0] == ')')
						|| (words->size > 0 && isalpha(words[words->size - 1].word[strlen(words[words->size - 1].word) - 1]))
						|| (i + 1 < textSize && isalpha(text[i + 1]))
						)
					{
						wordStartCol = col;
						if (bufpos < MAX_LEN_BUFFER - 1)
					{
						buffer[bufpos++] = text[i];
						buffer[bufpos] = IN_CODE_NULL;
						}
						addWord(words, buffer, line, wordStartCol);
						memset(buffer, 0, MAX_LEN_BUFFER);
						bufpos = 0;
						break;
					}
					wordStartCol = col;
					if (bufpos < MAX_LEN_BUFFER - 1)
					{
					buffer[bufpos++] = text[i];
					buffer[bufpos] = IN_CODE_NULL;
					}
					break;
				}
				if ((i + 1 < textSize && text[i] == LEX_PLUS && text[i + 1] == LEX_PLUS)
					|| (i + 1 < textSize && text[i] == LEX_STAR && text[i + 1] == LEX_STAR)
					|| (i + 1 < textSize && text[i] == LEX_DIRSLASH && text[i + 1] == LEX_DIRSLASH))
				{
					if (bufpos > 0 && buffer[0] != IN_CODE_NULL)
					{
						addWord(words, buffer, line, wordStartCol);
						memset(buffer, 0, MAX_LEN_BUFFER);
						bufpos = 0;
					}
					wordStartCol = col;
					if (bufpos < MAX_LEN_BUFFER - 2)
					{
					buffer[bufpos++] = ':';
					buffer[bufpos++] = text[i];
					buffer[bufpos] = IN_CODE_NULL;
					}
					addWord(words, buffer, line, wordStartCol);
					memset(buffer, 0, MAX_LEN_BUFFER);
					bufpos = 0;
					i++;
					col++;
					break;
				}
				if (bufpos > 0 && buffer[0] != IN_CODE_NULL)
				{
					addWord(words, buffer, line, wordStartCol);
					memset(buffer, 0, MAX_LEN_BUFFER);
				bufpos = 0;
				}
				char letter[2] = { (char)text[i], IN_CODE_NULL };
				addWord(words, letter, line, col);
				break;
			}
			case IN::K:
			{
				while (i < textSize && text[i] != '\n')
				{
					i++;
				}
				if (bufpos > 0 && buffer[0] != IN_CODE_NULL)
				{
					addWord(words, buffer, line, wordStartCol);
					memset(buffer, 0, MAX_LEN_BUFFER);
					bufpos = 0;
				}
				line++;
				col = 0;
				break;
			}
			case IN::N:
			case IN::W:
				if (bufpos > 0 && buffer[0] != IN_CODE_NULL)
				{
					addWord(words, buffer, line, wordStartCol);
					memset(buffer, 0, MAX_LEN_BUFFER);
					bufpos = 0;
				}
				if (code[text[i]] == IN::N)
				{
					line++;
					col = 0;
				}
				break;
			case IN::A:
			{
				if (bufpos > 0 && buffer[0] != IN_CODE_NULL)
			{
				addWord(words, buffer, line, wordStartCol);
					memset(buffer, 0, MAX_LEN_BUFFER);
				bufpos = 0;
				}
				if (bufpos < MAX_LEN_BUFFER - 1)
				{
					wordStartCol = col;
					buffer[bufpos++] = IN_CODE_APOST;
					for (int j = 1; ; j++)
				{
						if (j == 256 || i + j >= textSize || bufpos >= MAX_LEN_BUFFER - 1)
				{
					delete[] words;
					throw Error::geterrorin(207, line, wordStartCol);
				}
						buffer[bufpos++] = text[i + j];
						col++;
						if (text[i + j] == IN_CODE_APOST)
						{
				buffer[bufpos] = IN_CODE_NULL;
				addWord(words, buffer, line, wordStartCol);
							i = i + j;
							memset(buffer, 0, MAX_LEN_BUFFER);
							bufpos = 0;
							break;
						}
						if (code[text[i + j]] == IN::N)
						{
							delete[] words;
							throw Error::geterrorin(206, line, wordStartCol);
						}
					}
				}
				memset(buffer, 0, MAX_LEN_BUFFER);
				bufpos = 0;
				break;
			}
			case IN::Q:
			{
				if (bufpos > 0 && buffer[0] != IN_CODE_NULL)
			{
				addWord(words, buffer, line, wordStartCol);
					memset(buffer, 0, MAX_LEN_BUFFER);
				bufpos = 0;
				}
				int stringStartCol = col;
				for (int j = i + 1; j < textSize && text[j] != IN_CODE_QUOTE; j++)
				{
					if (code[text[j]] == IN::N)
					{
						delete[] words;
						throw Error::geterrorin(206, line, stringStartCol);
					}
				}
				if (bufpos < MAX_LEN_BUFFER - 1)
				{
					wordStartCol = col;
					buffer[bufpos++] = IN_CODE_QUOTE;
					int string_length = 0;
					for (int j = 1; ; j++)
					{
						if (i + j >= textSize)
						{
							if (string_length >= STR_MAXSIZE)
							{
								delete[] words;
								throw Error::geterrorin(207, line, wordStartCol);
							}
							break;
						}
						
						if (text[i + j] == IN_CODE_QUOTE)
						{
							if (string_length >= STR_MAXSIZE)
							{
								delete[] words;
								throw Error::geterrorin(207, line, wordStartCol);
							}
							size_t max_buf_pos = (STR_MAXSIZE + 1 < MAX_LEN_BUFFER - 1) ? (STR_MAXSIZE + 1) : (MAX_LEN_BUFFER - 1);
							if ((size_t)bufpos >= max_buf_pos)
							{
								bufpos = max_buf_pos - 1;
							}
							if (bufpos < MAX_LEN_BUFFER - 1)
							{
								buffer[bufpos++] = IN_CODE_QUOTE;
							}
							buffer[bufpos] = IN_CODE_NULL;
							addWord(words, buffer, line, wordStartCol);
							i = i + j;
							col += j + 1;
							break;
						}
						
						string_length++;
						
						if (string_length > STR_MAXSIZE)
						{
							delete[] words;
							throw Error::geterrorin(207, line, wordStartCol);
						}
						
						if (bufpos < MAX_LEN_BUFFER - 1 && string_length <= STR_MAXSIZE)
						{
							buffer[bufpos++] = text[i + j];
						}
					}
				}
				memset(buffer, 0, MAX_LEN_BUFFER);
				bufpos = 0;
				break;
			}
			case IN::O:
			{
				if (bufpos > 0 && buffer[0] != IN_CODE_NULL)
				{
					addWord(words, buffer, line, wordStartCol);
					memset(buffer, 0, MAX_LEN_BUFFER);
					bufpos = 0;
				}
				char op[2] = { (char)text[i], IN_CODE_NULL };
				addWord(words, op, line, col);
				break;
			}
			case IN::L:
			{
				if (bufpos < MAX_LEN_BUFFER - 1)
				{
					buffer[bufpos++] = text[i];
					buffer[bufpos] = IN_CODE_NULL;
				}
				break;
			}
			case IN::D:
			case IN::H:
			case IN::B:
			{
				if (bufpos < MAX_LEN_BUFFER - 1)
				{
					buffer[bufpos++] = text[i];
					buffer[bufpos] = IN_CODE_NULL;
				}
				break;
			}
			default:
				if (code[text[i]] == IN::L && text[i] == '`')
				{
					if (bufpos < MAX_LEN_BUFFER - 1)
					{
						buffer[bufpos++] = text[i];
						buffer[bufpos] = IN_CODE_NULL;
					}
				}
				else if (bufpos < MAX_LEN_BUFFER - 1)
				{
				buffer[bufpos++] = text[i];
				buffer[bufpos] = IN_CODE_NULL;
			}
		}
		}
		
		if (bufpos > 0 && buffer[0] != IN_CODE_NULL)
		{
			addWord(words, buffer, line, wordStartCol);
		}
		
		return words;
	}
	void printTable(InWord* table)
	{
		std::cout << " ------------------ ÒÀÁËÈÖÀ ÑËÎÂ: ------------------" << std::endl;
		for (int i = 0; i < table->size; i++)
			std::cout << std::setw(5) << i << std::setw(5) << table[i].line << " |  " << table[i].word << std::endl;
	}

}