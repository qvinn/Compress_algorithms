#pragma once
#ifndef HUFFMAN_H
    #define HUFFMAN_H

    #include "main.h"

    class HUFFMAN {
        public:
            HUFFMAN();
            ~HUFFMAN();
            FILE *ifile, *ofile; // входной и выходной файлы
            void Encode();
            void Decode();

        private:
            void reheap(unsigned short heap_entry);
            void compress_image();
            unsigned short generate_code_table();
            void build_code_tree();
            void build_initial_heap();
            void get_frequency_count();
            void build_decomp_tree();
            void decompress_image();
    };

#endif // HUFFMAN_H
