#pragma once
#ifndef LZ77_H
    #define LZ77_H

    #include "main.h"

    #define OFFS_LN 8
    #define LEN_LN 4
    #define BYTE_LN 8
    //#define BUF_LEN 15
    #define BUF_LEN ((1 << LEN_LN) - 1)
    //#define DICT_LEN 256
    #define DICT_LEN ((1 << OFFS_LN) - 1)

    class LZ77 {
        public:
            LZ77();
            ~LZ77();
            int in_file;  // входной файл
            int out_file; // выходной файл
            int dict_pos = 0;
            void assign_read(int h); // связывание для чтения
            void assign_write(int h); // связываение для записи
            void encode();
            void decode();

        private:
            int handle;
            unsigned char buffer1, mask;
            char buffer[BUF_LEN + 1], dict[DICT_LEN];
            int match_pos, match_len, unmatched;
            void flush(); // заполнение нулями
            void putbit(bool val); // запись бита
            bool getbit(); //чтенние бита
            void putbits(int val, int n); // запись n битов
            int getbits(int n); // чтение n битов
            int strpos(char *src, int len, char *sub, int sub_len);
            void add_dict(char c);
            void find_match();

    };

#endif // LZ77_H
