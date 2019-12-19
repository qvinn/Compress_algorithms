#include "arithmetic_adaptive.h"

ARITHMETIC_ADAPTIVE::ARITHMETIC_ADAPTIVE(){}

ARITHMETIC_ADAPTIVE::~ARITHMETIC_ADAPTIVE(){}

//------------------------------------------------------------
// Инициализация адаптивной модели
void ARITHMETIC_ADAPTIVE::start_model(void) {
    int i;
    for(i = 0; i < NO_OF_CHARS; i++) {
        char_to_index[i] = i + 1;
        index_to_char[i + 1] = i;
    }
    for(i = 0; i <= NO_OF_SYMBOLS; i++) {
        freq [i] = 1;
        cum_freq[i] = NO_OF_SYMBOLS - i;
    }
    freq[0] = 0;
}

//------------------------------------------------------------
// Обновление модели очередным символом
void ARITHMETIC_ADAPTIVE::update_model(int symbol) {
    int i;
    int ch_i, ch_symbol;
    int cum;
    if(cum_freq[0] == MAX_FREQUENCY) {            // проверка на переполнение счетчика частоты
        cum = 0; // масштабирование частот при переполнении
        for (i = NO_OF_SYMBOLS; i >= 0; i--) {
            freq[i] = (freq[i] + 1) / 2;
            cum_freq[i] = cum;
            cum += freq[i];
        }
    }
    for(i = symbol; freq[i] == freq[i - 1]; i--);
    if(i < symbol) {
        ch_i = index_to_char[i];
        ch_symbol = index_to_char [symbol];
        index_to_char[i] = ch_symbol;
        index_to_char[symbol] = ch_i;
        char_to_index[ch_i] = symbol;
        char_to_index[ch_symbol] = i;
    }
    freq[i] += 1;      // обновление значений в таблицах частот
    while(i > 0) {
        i -= 1;
        cum_freq[i] += 1;
    }
}

//------------------------------------------------------------
// Инициализация побитового ввода
void ARITHMETIC_ADAPTIVE::start_inputing_bits(void) {
    bits_to_go = 0;
    garbage_bits = 0;
}

//------------------------------------------------------------
// Ввод очередного бита сжатой информации
int ARITHMETIC_ADAPTIVE::input_bit(void) {
    int t;
    if(bits_to_go == 0) {
        buffer = getc (in);
        if(buffer == EOF) {
            garbage_bits += 1;
            if(garbage_bits > BITS_IN_REGISTER - 2) {
                printf("Ошибка в сжатом файле\n");
                exit(-1);
            }
        }
        bits_to_go = 8;
    }
    t = buffer & 1;
    buffer >>= 1;
    bits_to_go -= 1;
    return t;
}

//------------------------------------------------------------
// Инициализация побитового вывода
void ARITHMETIC_ADAPTIVE::start_outputing_bits(void) {
    buffer = 0;
    bits_to_go = 8;
}

//------------------------------------------------------------
// Вывод очередного бита сжатой информации
void ARITHMETIC_ADAPTIVE::output_bit(int bit) {
    buffer >>= 1;
    if(bit) {
        buffer |= 0x80;
    }
    bits_to_go -= 1;
    if(bits_to_go == 0) {
        putc(buffer, out);
        bits_to_go = 8;
    }
}

//------------------------------------------------------------
// Очистка буфера побитового вывода
void ARITHMETIC_ADAPTIVE::done_outputing_bits(void) {
    putc(buffer >> bits_to_go, out);
}

//------------------------------------------------------------
// Вывод указанного бита и отложенных ранее
void ARITHMETIC_ADAPTIVE::output_bit_plus_follow(int bit) {
    output_bit (bit);
    while(bits_to_follow > 0) {
        output_bit(!bit);
        bits_to_follow--;
    }
}

//------------------------------------------------------------
// Инициализация регистров границ и кода перед началом сжатия
void ARITHMETIC_ADAPTIVE::start_encoding(void) {
    low = 0l;
    high = TOP_VALUE;
    bits_to_follow = 0l;
}

//------------------------------------------------------------
// Очистка побитового вывода
void ARITHMETIC_ADAPTIVE::done_encoding(void) {
    bits_to_follow++;
    if(low < FIRST_QTR) {
        output_bit_plus_follow(0);
    } else {
        output_bit_plus_follow(1);
    }
}

//------------------------------------------------------------
/* Инициализация регистров перед декодированием. Загрузка начала сжатого сообщения */
void ARITHMETIC_ADAPTIVE::start_decoding(void) {
    int i;
    value = 0l;
    for(i = 1; i <= BITS_IN_REGISTER; i++) {
        value = 2 * value + input_bit();
    }
    low = 0l;
    high = TOP_VALUE;
}

//------------------------------------------------------------
// Кодирование очередного символа
void ARITHMETIC_ADAPTIVE::encode_symbol(int symbol) {
    long range;
    // пересчет значений границ
    range = (long) (high - low) + 1;
    high = low + (range * cum_freq[symbol - 1]) / cum_freq[0] - 1;
    low = low + (range * cum_freq[symbol]) / cum_freq[0];
    // выдвигание очередных битов
    for(;;) {
        if(high < HALF) {
            output_bit_plus_follow(0);
        } else if(low >= HALF) {
            output_bit_plus_follow(1);
            low -= HALF;
            high -= HALF;
        } else if(low >= FIRST_QTR && high < THIRD_QTR) {
            bits_to_follow += 1;
            low -= FIRST_QTR;
            high -= FIRST_QTR;
        } else {
            break;
        }
        // сдвиг влево с "втягиванием" очередного бита
        low = 2 * low;
        high = 2 * high + 1;
    }
}

//------------------------------------------------------------
// Декодирование очередного символа
int ARITHMETIC_ADAPTIVE::decode_symbol(void) {
    long range;
    int cum, symbol;
    // определение текущего масштаба частот
    range = (long) (high - low) + 1;
    // масштабирование значения в регистре кода
    cum = (int) ((((long) (value - low) + 1) * cum_freq[0] - 1) / range);
    // поиск соответствующего символа в таблице частот
    for(symbol = 1; cum_freq[symbol] > cum; symbol++);
    // пересчет границ
    high = low + (range * cum_freq[symbol - 1]) / cum_freq[0] - 1;
    low = low + (range * cum_freq[symbol]) / cum_freq[0];
    // удаление очередного символа из входного потока
    for(;;) {
        if(high < HALF) {
        } else if(low >= HALF) {
            value -= HALF;
            low -= HALF;
            high -= HALF;
        } else if(low >= FIRST_QTR && high < THIRD_QTR) {
            value -= FIRST_QTR;
            low -= FIRST_QTR;
            high -= FIRST_QTR;
        } else {
            break;
        }
        // сдвиг влево с "втягиванием очередного бита
        low = 2 * low;
        high = 2 * high + 1;
        value = 2 * value + input_bit();
    }
    return symbol;
}

//------------------------------------------------------------
// Собственно адаптивное арифметическое кодирование
void ARITHMETIC_ADAPTIVE::encode(char *infile, char *outfile) {
    int ch, symbol;
    in = fopen(infile, "r+b");
    out = fopen(outfile, "w+b");
    if (in == NULL || out == NULL) {
        return;
    }
    start_model();
    start_outputing_bits();
    start_encoding();
    for(;;) {
        ch = getc(in);
        if(ch == EOF) {
            break;
        }
        symbol = char_to_index[ch];
        encode_symbol(symbol);
        update_model(symbol);
    }
    encode_symbol(EOF_SYMBOL);
    done_encoding();
    done_outputing_bits();
    fclose(in);
    fclose(out);
}

//------------------------------------------------------------
// Собственно адаптивное арифметическое декодирование
void ARITHMETIC_ADAPTIVE::decode(char *infile, char *outfile) {
    int ch, symbol;
    in = fopen(infile, "r+b");
    out = fopen(outfile, "w+b");
    if(in == NULL || out == NULL) {
        return;
    }
    start_model();
    start_inputing_bits();
    start_decoding();
    for(;;) {
        symbol = decode_symbol();
        if(symbol == EOF_SYMBOL) {
            break;
        }
        ch = index_to_char[symbol];
        putc(ch, out);
        update_model(symbol);
    }
    fclose(in);
    fclose(out);
}
