#include "lzss.h"

LZSS::LZSS(){}

LZSS::~LZSS(){}

void LZSS::send_reset() {
    o_bitbuffer = 0;
    o_bitcount = 0;
};

void LZSS::send_code(unsigned int what_code, unsigned int how_bits)
{
    o_bitbuffer |= (unsigned long int)what_code << o_bitcount;
    o_bitcount += how_bits;

    while (o_bitcount >= 8) {
        putc(o_bitbuffer & 0xff, outfile);
        o_bitbuffer >>= 8;
        o_bitcount -= 8;
    };
}

void LZSS::send_finish() { while (o_bitcount!=0) send_code(0,1); };
//---------------------------------------------------------
// функции битового ввода



void LZSS::read_reset(void) { i_bitbuffer = 0; i_bitcount = 0; };

unsigned int LZSS::read_code(unsigned int how_bits)
{
    unsigned int answer;
    unsigned long int temp;

    while(i_bitcount < how_bits) {
        temp = (getc(infile) & 0xff);
        temp <<= i_bitcount;
        i_bitbuffer |= temp;
        i_bitcount += 8;
    };

    answer = i_bitbuffer & ((1 << how_bits) - 1);
    i_bitbuffer >>= how_bits;
    i_bitcount -= how_bits;
    return answer;
};
//=========================================================
// функции реализации lzss

//---------------------------------------------------------
// инициализация lzss
void LZSS::init_lzss()
{
    unsigned int i;

    for (i = 0; i < N+F; i++)
        ring_buff[i] = '\0';

    for (i = 0; i < N + 1 + 4096; i++)
        next[i] = N;

    for (i = 0; i < N + 1; i++)
        prev[i] = N;
};
//---------------------------------------------------------
// удаление
void LZSS::del(unsigned int r)
{
    if ( prev[r] == N ) return;
    next[prev[r]] = next[r];
    prev[next[r]] = prev[r];
    prev[r] = next[r] = N;
};
//---------------------------------------------------------
// вставка
void LZSS::insert(unsigned int r)
{
    unsigned int next_r, c;
    c = ring_buff[r] + (ring_buff[r+1] << 8) & 0xfff;
    next_r = next[c + N + 1];
    next[c + N + 1] = r;
    prev[r] = c + N + 1;
    next[r] = next_r;
    if (next_r != N) prev[next_r] = r;
};
//---------------------------------------------------------
//
void LZSS::locate(unsigned int r)
{
    unsigned int p, c, i;

    match_len = match_pos = 0;
    c = ring_buff[r] + (ring_buff[r+1] << 8) & 0xfff;

    p = next[c + N + 1];
    i = 0;

    while (p != N) {
        for(i = 0; (i < F) && (ring_buff[p+i] ==
                               ring_buff[r+i]); i++);
        if (i>match_len) {
            match_len = i;
            match_pos = (r - p) & (N - 1);
        };
        if (i == F) break;
        p = next[p];
    };

    if (i == F)
        del(p);
};
//---------------------------------------------------------
// функция сжатия фходного файла в выходной
void LZSS::lzss_encode()
{
    unsigned int r, maxlen;
    int c;
    unsigned long int ps = 0, textsize;   /*printsize*/

    init_lzss();
    send_reset();
    r = 0;
    textsize = 0;

    maxlen = 0;
    while (maxlen<F) {
        if ((c = getc(infile)) != EOF) {
            ring_buff[maxlen + N] = ring_buff[maxlen] = c;
            maxlen++; textsize++;
        } else break;
    };

    while(maxlen) {
        locate(r);
        if (match_len > maxlen) match_len = maxlen;
        if (match_len<THRESHOLD) {
            send_code(0, match_len = 1);
            send_code(ring_buff[r], 8);
        } else {
            send_code(1,1);
            send_code(match_pos,     N_BITS);
            send_code(match_len - 1, F_BITS);
        };

        while(match_len--) {
            del( (r+F) & (N - 1) );
            maxlen--;
            if ((c = getc(infile)) != EOF) {
                ring_buff[(r+F) & (N - 1)] = c;
                if (r+F >= N) ring_buff[r+F] = c;
                textsize++; maxlen++;
            };
            insert(r);
            r = (r+1) & (N-1);
        };

        if (ps<textsize) {
            printf(" Done %10ld\r", textsize);
            ps+=4000;
        };
    };

    send_code(1,1);
    send_code(0,N_BITS);
    send_finish();
};
//---------------------------------------------------------
// Функция декомпрессии
void LZSS::lzss_decode()
{
    unsigned int r, c, d, l;

    init_lzss();
    read_reset();
    r = 0;

    while(1) {
        c = read_code(1);
        if (!c) {
            putc(ring_buff[r] = read_code(8), outfile);
            r = (r+1) & (N - 1);
        } else {
            d = read_code(N_BITS);
            if (!d) break;
            l = read_code(F_BITS) + 1;
            d = (r-d) & (N - 1);
            while(l--) {
                putc(ring_buff[r] = ring_buff[d], outfile);
                r = (r+1) & (N-1);
                d = (d+1) & (N-1);
            };
        };
    };
};
