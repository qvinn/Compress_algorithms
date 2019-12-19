#pragma once
#ifndef LZSS_H
    #define LZSS_H

    #include "main.h"

    #define NO_ERROR 0
    #define BAD_FILE 1
    #define BAD_ARGUMENT 2
    #define N 4096 /* размер кольцевого буфера */
    #define F 128 /* размер просматриваемого буфера */
    #define THRESHOLD 3 /* minimum match length */
    #define N_BITS 12 /* log<2>N */
    #define F_BITS 7 /* log<2>F */

    class LZSS {
        public:
            LZSS();
            ~LZSS();
            FILE *infile, *outfile; // входной и выходной файлы
            void lzss_encode();
            void lzss_decode();

        private:
            unsigned char ring_buff[N+F]; /* кольцевой буфер */
            unsigned int next[N+1+4096], prev[N+1]; /* reserve space for hash as sons */
            unsigned int match_pos, match_len; /* match length & position */
            unsigned long int o_bitbuffer;
            unsigned int o_bitcount;
            unsigned long int i_bitbuffer;
            unsigned int i_bitcount;
            void read_reset(void);
            void send_code(unsigned int what_code, unsigned int how_bits);
            unsigned int read_code(unsigned int how_bits);
            void send_reset();
            void send_code();
            void send_finish();
            void init_lzss();
            void del(unsigned int r);
            void insert(unsigned int r);
            void locate(unsigned int r);
    };

#endif // LZSS_H
