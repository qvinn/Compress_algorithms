#pragma once
#ifndef LZARI_H
    #define LZARI_H

    #include "main.h"

    #define M   15
    #define Q1  (1UL << M)
    #define Q2  (2 * Q1)
    #define Q3  (3 * Q1)
    #define Q4  (4 * Q1)
    #define MAX_CUM (Q1 - 1)
    #define N_CHAR  (256 - THRESHOLD + F)
    #define N 4096 /* размер кольцевого буфера */
    #define F 60   /* верхний предел для match_length */
    #define THRESHOLD 2 /* строка кодируется в позицию и длину если match_length > THRESHOLD */
    #define NIL N /* индекс для корня дерева двоичного поиска */

    class LZAri {
        public:
            LZAri();
            ~LZAri();
            void Encode(void);
            void Decode(void);
            FILE  *infile, *outfile;
            unsigned long int  textsize = 0, codesize = 0, printcount = 0;
            unsigned long int  low = 0, high = Q4, value = 0;
            int  shifts = 0;

        private:
            void Error(char *message);
            void PutBit(int bit);
            void FlushBitBuffer(void);
            int GetBit(void);
            void InitTree(void);
            void InsertNode(int r);
            void DeleteNode(int p);
            void StartModel(void);
            void UpdateModel(int sym);
            void Output(int bit);
            void EncodeChar(int ch);
            void EncodePosition(int position);
            void EncodeEnd(void);
            int BinarySearchSym(unsigned int x);
            int BinarySearchPos(unsigned int x);
            void StartDecode(void);
            int DecodeChar(void);
            int DecodePosition(void);
            unsigned char  text_buf[N + F - 1]; /* кольцевоё буфер размера N, с дополнительными (F-1) байтами для сравнения строк */
            int match_position, match_length,  /* of longest match. Эти значения устанавливаются функцией InsertNode()*/
            lson[N + 1], rson[N + 257], dad[N + 1];  /* левые и правые потомки  и родители - составляющие бинарного дерева. */
    };

#endif // LZH_H
