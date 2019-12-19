#include "lzari.h"

LZAri::LZAri() {}
LZAri:: ~LZAri() {}

//---------------------------------------------------------
// Вывод соощения об ошибке с выходом из программы
void LZAri :: Error(char *message)
{
    printf("\n%s\n", message);
    exit(EXIT_FAILURE);
}
//---------------------------------------------------------
// Вывод одного бита (bit = 0,1)
void LZAri :: PutBit(int bit)
{
    static unsigned int  buffer = 0, mask = 128;
    if (bit) buffer |= mask;
    if ((mask >>= 1) == 0) {
        if (putc(buffer, outfile) == EOF) Error("Write Error");
        buffer = 0;  mask = 128;  codesize++;
    }
}
//---------------------------------------------------------
// Заполнение оставшихся битов
void LZAri :: FlushBitBuffer(void)
{
    int  i;
    for (i = 0; i < 7; i++) PutBit(0);
}
//---------------------------------------------------------
// Чтение одного бита
int LZAri :: GetBit(void)
{
    static unsigned int  buffer, mask = 0;

    if ((mask >>= 1) == 0) {
        buffer = getc(infile);  mask = 128;
    }
    return ((buffer & mask) != 0);
}
//---------------------------------------------------------
/********** LZSS с двоичными деревьями **********/

//---------------------------------------------------------
// Инициализация дерева
void LZAri :: InitTree(void)
{
    int  i;

    /* For i = 0 to N - 1, rson[i] и lson[i] будут правым
  и левым потомками узла i. Эти узлы не нуждаются
  в инициализации. dad[i] является родителем узла i.
  Они инициализируются как NIL (= N), означающее 'не использовано'
  For i = 0 to 255, rson[N + i + 1] - корени деревьев для строк,
  которые начнаются с символа i. Они инициализируются как NIL.
  Всего 256 деревьев */

    for (i = N + 1; i <= N + 256; i++) rson[i] = NIL; /* корень */
    for (i = 0; i < N; i++) dad[i] = NIL; /* узел */
}
//---------------------------------------------------------
// Вставка узла
void LZAri :: InsertNode(int r)
/* Вставляет строку text_buf[r..r+F-1] длины F в одно из
 деревьев (text_buf[r]'th tree) и возвращает позицию и длину
 через глобальные переменные match_position и match_length.
    Если match_length = F, то  старый узел замещается новым.
 r играет двойную роль - как узел дерева и позиция в буфере */
{
    int  i, p, cmp, temp;
    unsigned char  *key;

    cmp = 1;  key = &text_buf[r];  p = N + 1 + key[0];
    rson[r] = lson[r] = NIL;  match_length = 0;
    for ( ; ; ) {
        if (cmp >= 0) {
            if (rson[p] != NIL) p = rson[p];
            else { rson[p] = r;  dad[r] = p;  return; }
        } else {
            if (lson[p] != NIL) p = lson[p];
            else { lson[p] = r;  dad[r] = p;  return; }
        }

        for (i = 1; i < F; i++)
            if ((cmp = key[i] - text_buf[p + i]) != 0)  break;

        if (i > THRESHOLD) {
            if (i > match_length) {
                match_position = (r - p) & (N - 1);
                if ((match_length = i) >= F) break;
            } else if (i == match_length) {
                if ((temp = (r - p) & (N - 1)) < match_position)
                    match_position = temp;
            }
        }
    }
    dad[r] = dad[p];  lson[r] = lson[p];  rson[r] = rson[p];
    dad[lson[p]] = r;  dad[rson[p]] = r;
    if (rson[dad[p]] == p) rson[dad[p]] = r;
    else lson[dad[p]] = r;
    dad[p] = NIL;  /* удаление p */
}
//---------------------------------------------------------
// Удаление узла p из дерева
void LZAri :: DeleteNode(int p)
{
    int  q;

    if (dad[p] == NIL) return;  /* нет в дереве */
    if (rson[p] == NIL) q = lson[p];
    else if (lson[p] == NIL) q = rson[p];
    else {
        q = lson[p];
        if (rson[q] != NIL) {
            do { q = rson[q]; } while (rson[q] != NIL);
            rson[dad[q]] = lson[q];  dad[lson[q]] = dad[q];
            lson[q] = lson[p];  dad[lson[p]] = q;
        }
        rson[q] = rson[p];  dad[rson[p]] = q;
    }
    dad[q] = dad[p];
    if (rson[dad[p]] == p) rson[dad[p]] = q;
    else lson[dad[p]] = q;
    dad[p] = NIL;
}
//---------------------------------------------------------

/********** Арифметическое сжатие **********/

//#define M   15

/* Q1 (= 2^M) должно быть достаточно большим,
но не больше чем unsigned long 4 * Q1 * (Q1 - 1)  */
//#define Q1  (1UL << M)
//#define Q2  (2 * Q1)
//#define Q3  (3 * Q1)
//#define Q4  (4 * Q1)
//#define MAX_CUM (Q1 - 1)

//#define N_CHAR  (256 - THRESHOLD + F)
/* код символа = 0, 1, ..., N_CHAR - 1 */

int  char_to_sym[N_CHAR], sym_to_char[N_CHAR + 1];
unsigned int
sym_freq[N_CHAR + 1],  /* частота символов */
sym_cum[N_CHAR + 1],   /* кумулятивная частота символов */
position_cum[N + 1];   /* кумулятивная частота для позиций */
//---------------------------------------------------------
// Инициализация модели
void LZAri :: StartModel(void)
{
    int ch, sym, i;

    sym_cum[N_CHAR] = 0;
    for (sym = N_CHAR; sym >= 1; sym--) {
        ch = sym - 1;
        char_to_sym[ch] = sym;  sym_to_char[sym] = ch;
        sym_freq[sym] = 1;
        sym_cum[sym - 1] = sym_cum[sym] + sym_freq[sym];
    }
    sym_freq[0] = 0;
    position_cum[N] = 0;
    for (i = N; i >= 1; i--)
        position_cum[i - 1] = position_cum[i] + 10000 / (i + 200);
    /* функция распределения подобрана эмпирически */
}
//---------------------------------------------------------
// Обновление модели
void LZAri :: UpdateModel(int sym)
{
    int i, c, ch_i, ch_sym;

    if (sym_cum[0] >= MAX_CUM) {
        c = 0;
        for (i = N_CHAR; i > 0; i--) {
            sym_cum[i] = c;
            c += (sym_freq[i] = (sym_freq[i] + 1) >> 1);
        }
        sym_cum[0] = c;
    }
    for (i = sym; sym_freq[i] == sym_freq[i - 1]; i--) ;
    if (i < sym) {
        ch_i = sym_to_char[i];    ch_sym = sym_to_char[sym];
        sym_to_char[i] = ch_sym;  sym_to_char[sym] = ch_i;
        char_to_sym[ch_i] = sym;  char_to_sym[ch_sym] = i;
    }
    sym_freq[i]++;
    while (--i >= 0) sym_cum[i]++;
}
//---------------------------------------------------------
// Вывод одного бита с его дополнениями
void LZAri :: Output(int bit)
{
    PutBit(bit);
    for ( ; shifts > 0; shifts--) PutBit(! bit);
}
//---------------------------------------------------------
// Кодирование символа
void LZAri :: EncodeChar(int ch)
{
    int  sym;
    unsigned long int  range;

    sym = char_to_sym[ch];
    range = high - low;
    high = low + (range * sym_cum[sym - 1]) / sym_cum[0];
    low +=       (range * sym_cum[sym    ]) / sym_cum[0];
    for ( ; ; ) {
        if (high <= Q2) Output(0);
        else if (low >= Q2) {
            Output(1);  low -= Q2;  high -= Q2;
        } else if (low >= Q1 && high <= Q3) {
            shifts++;  low -= Q1;  high -= Q1;
        } else break;
        low += low;  high += high;
    }
    UpdateModel(sym);
}
//---------------------------------------------------------
// Кодирование позиции
void LZAri :: EncodePosition(int position)
{
    unsigned long int  range;

    range = high - low;
    high = low + (range * position_cum[position    ])
            / position_cum[0];
    low +=       (range * position_cum[position + 1])
            / position_cum[0];
    for ( ; ; ) {
        if (high <= Q2) Output(0);
        else if (low >= Q2) {
            Output(1);  low -= Q2;  high -= Q2;
        } else if (low >= Q1 && high <= Q3) {
            shifts++;  low -= Q1;  high -= Q1;
        } else break;
        low += low;  high += high;
    }
}
//---------------------------------------------------------
// Кодирование конца
void LZAri :: EncodeEnd(void)
{
    shifts++;
    if (low < Q1) Output(0);  else Output(1);
    FlushBitBuffer();  /* вывод оставшихся в буфере бит */
}
//---------------------------------------------------------
// Двоичный поиск символа
int LZAri :: BinarySearchSym(unsigned int x)
/* 1      if x >= sym_cum[1],
   N_CHAR if sym_cum[N_CHAR] > x,
   i such that sym_cum[i - 1] > x >= sym_cum[i] otherwise */
{
    int i, j, k;

    i = 1;  j = N_CHAR;
    while (i < j) {
        k = (i + j) / 2;
        if (sym_cum[k] > x) i = k + 1;  else j = k;
    }
    return i;
}
//---------------------------------------------------------
// Двоичный поиск позиции
int LZAri :: BinarySearchPos(unsigned int x)
/* 0 if x >= position_cum[1],
   N - 1 if position_cum[N] > x,
   i such that position_cum[i] > x >= position_cum[i + 1]
   otherwise */
{
    int i, j, k;

    i = 1;  j = N;
    while (i < j) {
        k = (i + j) / 2;
        if (position_cum[k] > x) i = k + 1;  else j = k;
    }
    return i - 1;
}
//---------------------------------------------------------
// Старт декодирования
void LZAri :: StartDecode(void)
{
    int i;
    for (i = 0; i < M + 2; i++)
        value = 2 * value + GetBit();
}
//---------------------------------------------------------
// Декодирование символа
int LZAri :: DecodeChar(void)
{
    int sym, ch;
    unsigned long int  range;

    range = high - low;
    sym = BinarySearchSym((unsigned int)
                          (((value - low + 1) * sym_cum[0] - 1) / range));
    high = low + (range * sym_cum[sym - 1]) / sym_cum[0];
    low += (range * sym_cum[sym]) / sym_cum[0];
    for ( ; ; ) {
        if (low >= Q2) {
            value -= Q2;  low -= Q2;  high -= Q2;
        } else if (low >= Q1 && high <= Q3) {
            value -= Q1;  low -= Q1;  high -= Q1;
        } else if (high > Q2) break;
        low += low;  high += high;
        value = 2 * value + GetBit();
    }
    ch = sym_to_char[sym];
    UpdateModel(sym);
    return ch;
}
//---------------------------------------------------------
// Декодирование позиции
int LZAri :: DecodePosition(void)
{
    int position;
    unsigned long int  range;

    range = high - low;
    position = BinarySearchPos((unsigned int)
                               (((value - low + 1) * position_cum[0] - 1) / range));
    high = low+(range*position_cum[position])/position_cum[0];
    low += (range*position_cum[position + 1])/position_cum[0];
    for ( ; ; ) {
        if (low >= Q2) {
            value -= Q2;  low -= Q2;  high -= Q2;
        } else if (low >= Q1 && high <= Q3) {
            value -= Q1;  low -= Q1;  high -= Q1;
        } else if (high > Q2) break;
        low += low;  high += high;
        value = 2 * value + GetBit();
    }
    return position;
}
//---------------------------------------------------------
/********** Компрессия и Декомпрессия **********/

//Компрессия
void LZAri :: Encode(void)
{
    int  i, c, len, r, s, last_match_length;

    fseek(infile, 0L, SEEK_END);
    textsize = ftell(infile);
    if (fwrite(&textsize, sizeof textsize, 1, outfile) < 1)
        Error("Write Error");  /* output size of text */
    codesize += sizeof textsize;
    if (textsize == 0) return;
    rewind(infile);  textsize = 0;
    StartModel();  InitTree();
    s = 0;  r = N - F;
    for (i = s; i < r; i++) text_buf[i] = ' ';
    for (len = 0; len < F && (c = getc(infile)) != EOF; len++)
        text_buf[r + len] = c;
    textsize = len;
    for (i = 1; i <= F; i++) InsertNode(r - i);
    InsertNode(r);
    do {
        if (match_length > len) match_length = len;
        if (match_length <= THRESHOLD) {
            match_length = 1;  EncodeChar(text_buf[r]);
        } else {
            EncodeChar(255 - THRESHOLD + match_length);
            EncodePosition(match_position - 1);
        }
        last_match_length = match_length;
        for (i = 0; i < last_match_length &&
             (c = getc(infile)) != EOF; i++) {
            DeleteNode(s);  text_buf[s] = c;
            if (s < F - 1) text_buf[s + N] = c;
            s = (s + 1) & (N - 1);
            r = (r + 1) & (N - 1);
            InsertNode(r);
        }
        if ((textsize += i) > printcount) {
            printf("%12ld\r", textsize);  printcount += 1024;
        }
        while (i++ < last_match_length) {
            DeleteNode(s);
            s = (s + 1) & (N - 1);
            r = (r + 1) & (N - 1);
            if (--len) InsertNode(r);
        }
    } while (len > 0);
    EncodeEnd();
    printf("In : %lu bytes\n", textsize);
    printf("Out: %lu bytes\n", codesize);
    printf("Out/In: %.3f\n", (double)codesize / textsize);
}
//---------------------------------------------------------
//Декомпрессия
void LZAri :: Decode(void)
{
    int  i, j, k, r, c;
    unsigned long int  count;

    if (fread(&textsize, sizeof textsize, 1, infile) < 1)
        Error("Read Error");  /* read size of text */
    if (textsize == 0) return;
    StartDecode();
    StartModel();
    for (i = 0; i < N - F; i++) text_buf[i] = ' ';
    r = N - F;
    for (count = 0; count < textsize; ) {
        c = DecodeChar();
        if (c < 256) {
            putc(c, outfile);  text_buf[r++] = c;
            r &= (N - 1);  count++;
        } else {
            i = (r - DecodePosition() - 1) & (N - 1);
            j = c - 255 + THRESHOLD;
            for (k = 0; k < j; k++) {
                c = text_buf[(i + k) & (N - 1)];
                putc(c, outfile);
                text_buf[r++] = c;
                r &= (N - 1);  count++;
            }
        }
        if (count > printcount) {
            printf("%12lu\r", count);  printcount += 1024;
        }
    }
    printf("%12lu\n", count);
}
