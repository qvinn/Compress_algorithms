#pragma once
#ifndef LZARI_H
    #define LZARI_H

    #include "main.h"

    #define EXIT_SUCCESS 0
    #define EXIT_FAILURE 1
    #define N 4096                                  /* размер кольцевого буфера */
    #define F 60                                    /* верхний предел для match_length */
    #define THRESHOLD 2                             /* строка кодируется в позицию и длину если match_length > THRESHOLD */
    #define NIL N                                   /* индекс для корня дерева двоичного поиска */
    #define M 15
    #define Q1 (1UL << M)                           /* Q1 (= 2^M) должно быть достаточно большим, но не больше чем unsigned long 4 * Q1 * (Q1 - 1)  */
    #define Q2 (2 * Q1)
    #define Q3 (3 * Q1)
    #define Q4 (4 * Q1)
    #define MAX_CUM (Q1 - 1)
    #define N_CHAR (256 - THRESHOLD + F)            /* код символа = 0, 1, ..., N_CHAR - 1 */

    class LZAri {
        public:
            LZAri();
            ~LZAri();
            FILE *infile;//*infile;
            FILE *outfile;//*outfile;
            void Encode(void);
            void Decode(void);
            void reset_vars();

        private:
            unsigned long int textsize;// = 0;
            volatile unsigned long int codesize;// = 0;
            volatile unsigned long int printcount;// = 0;
            unsigned char text_buf[N + F - 1];      /* кольцевоё буфер размера N, с дополнительными (F-1) байтами для сравнения строк of longest match. Эти значения устанавливаются функцией InsertNode() левые и правые потомки и родители - составляющие бинарного дерева.*/
            volatile int match_position;
            volatile int match_length;
            volatile int lson[N + 1];
            volatile int rson[N + 257];
            volatile int dad[N + 1];
            volatile unsigned long int low;// = 0;
            volatile unsigned long int high;// = Q4;
            volatile unsigned long int value;// = 0;
            volatile int shifts;// = 0;
            volatile int char_to_sym[N_CHAR];
            volatile int sym_to_char[N_CHAR + 1];
            volatile unsigned int sym_freq[N_CHAR + 1];      /* частота символов */
            volatile unsigned int sym_cum[N_CHAR + 1];       /* кумулятивная частота символов */
            volatile unsigned int position_cum[N + 1];       /* кумулятивная частота для позиций */
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
    };

#endif // LZARI_H
