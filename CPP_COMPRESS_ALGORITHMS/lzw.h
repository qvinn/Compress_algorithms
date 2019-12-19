#pragma once
#ifndef LZW_H
    #define LZW_H

    #include "main.h"

    #define PACIFIER_COUNT 2047
    #define BITS 12                             /* Количество битов в коде */
    #define MAX_CODE (( 1 << BITS ) - 1)        /* Максимальное значение кода */
    #define TABLE_SIZE 5021                     /* Размер словаря в элементах */
    #define END_OF_STREAM 256                   /* Специальный код конца потока */
    #define FIRST_CODE 257                      /* Значение кода, которое получает первая добавленна в словарь фраза */
    #define UNUSED -1                           /* Признак свободной ячейки в словаре */

    struct BFILE
    {
        FILE *file;
        unsigned char mask;
        int rack;
        int pacifier_counter;
    };

    class LZW {
        public:
            LZW();
            ~LZW();
            BFILE *OpenInputBFile ( char *name );
            BFILE *OpenOutputBFile ( char *name );
            void CloseInputBFile ( BFILE *bfile );
            void CloseOutputBFile ( BFILE *bfile );
            void CompressFile ( FILE *input, BFILE *output );
            void ExpandFile   ( BFILE *input, FILE *output );

        private:
            void fatal_error( char *str );
            void WriteBit ( BFILE *bfile, int bit );
            void WriteBits ( BFILE *bfile, unsigned long code, int count );
            int ReadBit ( BFILE *bfile );
            unsigned long ReadBits ( BFILE *bfile, int bit_count );
            void usage_exit ( );
            void usage_exit ( char *prog_name );
            void print_ratios ( char *input, char *output );
            long file_size ( char *name );
            unsigned int find_dictionary_match ( int prefix_code, int character );
            unsigned int decode_string ( unsigned int offset, unsigned int code );
    };

#endif // LZW_H
