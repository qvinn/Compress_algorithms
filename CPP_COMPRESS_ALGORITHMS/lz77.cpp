#include "lz77.h"

LZ77::LZ77(){}

LZ77::~LZ77(){}

//---------------------------------------------------------
// заполнение нулями
void LZ77::flush()
{
    for (int i = 0; i < 7; i++) putbit(0);
}

//---------------------------------------------------------
// связываение для записи
void LZ77::assign_write(int h)
{
    handle = h;
    buffer1 = 0;
    mask = 128;
}

//---------------------------------------------------------
// связывание для чтения
void LZ77::assign_read(int h)
{
    handle = h;
    buffer1 = 0;
    mask = 0;
}

//---------------------------------------------------------
//чтенние бита
bool LZ77::getbit()
{
    if ((mask >>= 1) == 0)
    {
        read(handle, &buffer1, 1);
        mask = 128;//двоичное 10000000
    }
    return (buffer1 & mask) != 0;
}

//---------------------------------------------------------
// запись бита
void LZ77::putbit(bool val)
{
    if (val) buffer1 |= mask;
    if ((mask >>= 1) == 0)
    {
        write(handle, &buffer1, 1);
        buffer1 = 0;
        mask = 128;//двоичное 10000000
    }
}

//---------------------------------------------------------
// запись n битов
void LZ77::putbits(int val, int n)
{
    int m = 1;
    for (int i = 0; i < n; i++)
    {
        putbit((val & m) != 0);
        m <<= 1;
    }
}

//---------------------------------------------------------
// чтение n битов
int LZ77::getbits(int n)
{
    int result = 0;
    for (int i = 0; i < n; i++)
        result |= getbit() << i;
    return result;
}

int LZ77::strpos(char *src, int len, char *sub, int sub_len)
{
    for (int i = 0; i <= len - sub_len; i++)
        if (memcmp(src + i, sub, sub_len) == 0) return len - i;
    return 0;
}

//---------------------------------------------------------
// добавление в словарь символа
void LZ77::add_dict(char c)
{
    if (dict_pos == (DICT_LEN - 1))
    {
        memcpy(dict, dict + 1, DICT_LEN - 1);
        dict[dict_pos - 1] = c;
    }
    else
    {
        dict[dict_pos] = c;
        dict_pos++;
    }
}

//---------------------------------------------------------
// поиск совпадения в словаре
void LZ77::find_match()
{
    match_len = 0;
    match_pos = 1;
    while(match_len < BUF_LEN)
    {
        read(in_file, &buffer[match_len], 1);
        if (eof(in_file)) break;
        int pos1 = strpos(dict, dict_pos, buffer, match_len + 1);
        if (pos1 == 0) break;
        match_pos = pos1;
        match_len++;
    }
    unmatched = buffer[match_len];
}

//---------------------------------------------------------
// компрессия
void LZ77::encode()
{
    while(!eof(in_file))
    {
        find_match();
        putbits(match_pos, OFFS_LN);
        putbits(match_len, LEN_LN);
        if (match_len < BUF_LEN)
            putbits(unmatched, BYTE_LN);

        for (int i = 0; i < match_len; i++)
            add_dict(buffer[i]);
        if (match_len < BUF_LEN)
            add_dict(unmatched);
    }
    putbits(0, BYTE_LN);
    flush();
}

//---------------------------------------------------------
// декомпрессия
void LZ77::decode()
{
    char c;
    int i;
    for(;;)
    {
        match_pos = getbits(OFFS_LN);
        if (match_pos == 0) break;
        match_len = getbits(LEN_LN);
        memcpy(buffer, dict + dict_pos - match_pos, match_len);
        write(out_file, buffer, match_len);
        for (i = 0; i < match_len; i++)
            add_dict(buffer[i]);
        if (match_len < BUF_LEN)
        {
            c = getbits(BYTE_LN);
            add_dict(c);
            write(out_file, &c, 1);
        }
    }
}
