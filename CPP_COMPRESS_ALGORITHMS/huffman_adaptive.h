#pragma once
#ifndef HUFFMAN_ADAPTIVE_H
    #define HUFFMAN_ADAPTIVE_H

    #include "main.h"

    //Константы, используемые в алгоритме кодирования
    #define END_OF_STREAM 256 /* Маркер конца потока */
    #define ESCAPE 257 /* Маркер начала ESCAPE последовательности */
    #define SYMBOL_COUNT 258 /* Максимально возможное количество листьев дерева (256+2 маркера)*/
    #define NODE_TABLE_COUNT ((SYMBOL_COUNT * 2) - 1)
    #define ROOT_NODE 0
    #define MAX_WEIGHT 0x8000 /* Вес корня, при котором начинается масштабирование веса */
    #define TRUE 1
    #define FALSE 0
    #define PACIFIER_COUNT 2047 // шаг индикатора выполнения
    #define NO_ERROR 0      /* Коды ошибок */
    #define BAD_FILE_NAME 1
    #define BAD_ARGUMENT 2

    #ifndef SEEK_END
        #define SEEK_END 2          // Эта функция возвращает размер указанного ей файла
    #endif

    struct COMPRESSED_FILE {
        FILE *file;
        unsigned char mask;
        int rack;
        int pacifier_counter;
    };

    // структура, описывающая узел дерева
    struct node {
        unsigned int weight; /* Вес узла */
        int parent; /* Номер родителя в массиве узлов */
        int child_is_leaf; /* Флаг листа (TRUE, если лист) */
        int child;
    };

    struct TREE {
        int leaf[ SYMBOL_COUNT ]; /* Массив листьев дерева */
        int next_free_node; /* Номер следующего свободного элемента массива листьев */
        node nodes[ NODE_TABLE_COUNT ]; /* Массив узлов */
    };

    class HUFFMAN_ADAPTIVE {
        public:
            HUFFMAN_ADAPTIVE();
            ~HUFFMAN_ADAPTIVE();
            void ExpandFile(COMPRESSED_FILE *input, FILE *output);
            void CompressFile(FILE *input, COMPRESSED_FILE *output);
            COMPRESSED_FILE *OpenOutputCompressedFile(char *name);
            COMPRESSED_FILE *OpenInputCompressedFile(char *name);
            void CloseOutputCompressedFile(COMPRESSED_FILE *compressed_file);
            void CloseInputCompressedFile(COMPRESSED_FILE *compressed_file);

        private:
            TREE Tree; //Дерево адаптивного кодирования Хаффмена
            void help();
            long file_size(char *name);
            void print_ratios(char *input, char *output);
            void fatal_error(char *fmt);
            void OutputBit(COMPRESSED_FILE *compressed_file, int bit);
            void OutputBits(COMPRESSED_FILE *compressed_file, unsigned long code, int count);
            int InputBit(COMPRESSED_FILE *compressed_file);
            unsigned long InputBits(COMPRESSED_FILE *compressed_file, int bit_count);
            void InitializeTree(TREE *tree);
            void EncodeSymbol(TREE *tree, unsigned int c, COMPRESSED_FILE *output);
            int DecodeSymbol(TREE *tree, COMPRESSED_FILE *input);
            void UpdateModel(TREE *tree, int c);
            void RebuildTree(TREE *tree);
            void swap_nodes(TREE *tree, int i, int j);
            void add_new_node(TREE *tree, int c);
    };

#endif // HUFFMAN_ADAPTIVE_H
