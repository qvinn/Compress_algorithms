#pragma once
#ifndef LZH_H
    #define LZH_H

    #include "main.h"

    /* Параметры кодирования Хаффмана */
    #define N_CHAR (256 - THRESHOLD + F) /* character code (= 0..N_CHAR-1) */
    #define T (N_CHAR * 2 - 1) /* размер таблицы */
    #define R (T - 1)   /* позиция корня */
    #define MAX_FREQ 0x8000
    #define N  4096 /* размер буфера строки */
    #define F  60  /* размер "скользящего окна" */
    #define THRESHOLD 2
    #define NIL  N  /* End of tree's node  */
    typedef unsigned char uchar;

    class LZH {
        public:
            LZH();
            ~LZH();
            FILE  *infile, *outfile;
            unsigned long int  textsize = 0, codesize = 0, printcount = 0;
            unsigned getbuf = 0;
            uchar getlen = 0;
            unsigned putbuf = 0;
            uchar putlen = 0;
            void Encode(void);
            void Decode(void);

        private:
            void Error(char *message);
            void InitTree(void);
            void InsertNode(int r);
            void DeleteNode(int p);
            int GetBit(void);
            int GetByte(void);
            void Putcode(int l, unsigned c);
            void StartHuff();
            void reconst();
            void update(int c);
            void EncodeChar(unsigned c);
            void EncodePosition(unsigned c);
            void EncodeEnd();
            int DecodeChar();
            int DecodePosition();
            unsigned code, len;
            unsigned char text_buf[N + F - 1];
            int  match_position, match_length, lson[N + 1], rson[N + 257], dad[N + 1];
    };

#endif // LZH_H
