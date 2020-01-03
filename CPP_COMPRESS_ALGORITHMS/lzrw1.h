#pragma once
#ifndef LZRW1_H
    #define LZRW1_H

    #include "main.h"

    #define FLAG_BYTES 4            /* Number of bytes used by copy flag. */
    #define FLAG_COMPRESS 0         /* Signals that compression occurred. */
    #define FLAG_COPY 1             /* Signals that a copyover occurred.  */
    #define BUFSIZE 0x100000

    class LZRW1 {
        public:
            LZRW1();
            ~LZRW1();
            FILE *in_file;
            FILE *out_file; // входной и выходной файлы
            void compress_decompress_file(int i);

        private:
            void lzrw1_compress(unsigned char *p_src_first, unsigned int src_len, unsigned char *p_dst_first, unsigned int *p_dst_len);
            void lzrw1_decompress(unsigned char *p_src_first, unsigned int src_len, unsigned char *p_dst_first, unsigned int *p_dst_len);
            void fast_copy(unsigned char *p_src, unsigned char *p_dst, unsigned long len);
    };

#endif // LZRW1_H
