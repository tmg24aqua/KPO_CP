#pragma once
#define IN_MAX_LEN_TEXT 1024*1024
#define MAX_LEN_BUFFER  2048
#define IN_WORDS_MAXSIZE 4096
#define IN_CODE_ENDL    '\n'
#define IN_CODE_SPACE   ' '
#define IN_CODE_NULL    '\0'
#define IN_CODE_QUOTE   '\"'
#define IN_CODE_APOST   '\''
#define IN_CODE_TILDA   '~'

#define IN_CODE_TABLE {\
    IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,   IN::I, IN::W, IN::N, IN::I,  IN::I, IN::W, IN::I, IN::I,\
    IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,   IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,\
    IN::W, IN::S, IN::Q, IN::O,  IN::O, IN::O, IN::O, IN::A,   IN::S, IN::S, IN::O, IN::O,  IN::S, IN::O, IN::S, IN::O,\
    IN::D, IN::D, IN::D, IN::D,  IN::D, IN::D, IN::D, IN::D,   IN::D, IN::D, IN::S, IN::S,  IN::C, IN::S, IN::C, IN::S,\
    IN::S, IN::H, IN::H, IN::H,  IN::H, IN::H, IN::H, IN::L,   IN::L, IN::L, IN::L, IN::L,  IN::L, IN::L, IN::L, IN::L,\
    IN::L, IN::L, IN::L, IN::L,  IN::L, IN::L, IN::L, IN::L,   IN::L, IN::L, IN::L, IN::S,  IN::K, IN::S, IN::K, IN::L,\
    IN::L, IN::H, IN::H, IN::H,  IN::H, IN::H, IN::H, IN::L,   IN::L, IN::L, IN::L, IN::L,  IN::L, IN::L, IN::L, IN::L,\
    IN::L, IN::L, IN::L, IN::L,  IN::L, IN::L, IN::L, IN::L,   IN::L, IN::L, IN::L, IN::S,  IN::O, IN::S, IN::S, IN::S,\
	\
                IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,   IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,\
                IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,   IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,\
                IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,   IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,\
                IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,   IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,\
                IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,   IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,\
                IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,   IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,\
                IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,   IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,\
                IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I,   IN::I, IN::I, IN::I, IN::I,  IN::I, IN::I, IN::I, IN::I \
}

namespace In
{
	struct InWord
	{
        char word[MAX_LEN_BUFFER];
        int line;
        int col;
        int size;
	};

    struct IN
	{
        enum {
            L = 1024,
            D = 2048,
            H = 4096,
            B = 8192,
            O = 16384,
            C = 32768,
            S = 65536,
            Q = 131072,
            A = 262144,
            U = 524288,
            W = 1048576,
            N = 2097152,
            I = 4194304,
            K = 8388608
        };

        int size = 0;
        int lines = 1;
        int ignor = 0;
        unsigned char* text;
        int code[256] = IN_CODE_TABLE;
        InWord* words;
	};

    IN getin(wchar_t infile[], std::ostream* stream);
	void addWord(InWord* words, char* word, int line, int col = 0);
	InWord* getWordsTable(std::ostream* stream, unsigned char* text, int* code, int textSize);

	void printTable(InWord* table);
}
