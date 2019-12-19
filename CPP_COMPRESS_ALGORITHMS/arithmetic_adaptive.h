#pragma once
#ifndef ARITHMETIC_ADAPTIVE_H
    #define ARITHMETIC_ADAPTIVE_H

    #include "main.h"


    #define BITS_IN_REGISTER 16                                     // Количество битов в регистре
    #define TOP_VALUE (((long) 1 << BITS_IN_REGISTER) - 1)          // Максимально возможное значение в регистре
    #define FIRST_QTR (TOP_VALUE / 4 + 1)                           // Диапазоны
    #define HALF (2 * FIRST_QTR)
    #define THIRD_QTR (3 * FIRST_QTR)
    #define NO_OF_CHARS 256                                         // Количество символов алфавита
    #define EOF_SYMBOL (NO_OF_CHARS + 1)                            // Специальный символ КонецФайла
    #define NO_OF_SYMBOLS (NO_OF_CHARS + 1)                         // Всего символов в модели
    #define MAX_FREQUENCY 16383                                     // Порог частоты для масштабирования

    class ARITHMETIC_ADAPTIVE {
        public:
            ARITHMETIC_ADAPTIVE();
            ~ARITHMETIC_ADAPTIVE();
            FILE *in, *out;
            void encode(char *infile, char *outfile);
            void decode(char *infile, char *outfile);

        private:
            unsigned char index_to_char [NO_OF_SYMBOLS];            // Таблицы перекодировки
            int char_to_index [NO_OF_CHARS];
            int cum_freq [NO_OF_SYMBOLS + 1];                       // Таблицы частот
            int freq [NO_OF_SYMBOLS + 1];
            long low, high;                                         // Регистры границ и кода
            long value;
            long bits_to_follow;                                    // Поддержка побитлвых операций с файлами
            int buffer;
            int bits_to_go;
            int garbage_bits;
            void start_model(void);
            void update_model(int symbol);
            void start_inputing_bits(void);
            int input_bit(void);
            void start_outputing_bits(void);
            void output_bit(int bit);
            void done_outputing_bits(void);
            void output_bit_plus_follow(int bit);
            void start_encoding(void);
            void done_encoding(void);
            void start_decoding(void);
            void encode_symbol(int symbol);
            int decode_symbol(void);
    };

#endif // ARITHMETIC_ADAPTIVE_H
