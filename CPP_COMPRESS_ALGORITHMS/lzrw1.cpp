#include "lzrw1.h"
#include "memory.h"

LZRW1::LZRW1(){}

LZRW1::~LZRW1(){}

/******************************************************************************/
void LZRW1::compress_decompress_file(int i) {
    static unsigned char inb[BUFSIZE+256];
    static unsigned char outb[BUFSIZE+256];
    unsigned int n_in = 0;
    unsigned int n_out = 0;
    if(i == 0) {
        while((n_in = fread(inb, 1, BUFSIZE, in_file)) > 0) {
            lzrw1_compress(inb, n_in, outb, &n_out);
            fprintf(out_file, "%c%c%c%c", n_out >> 24, n_out >> 16, n_out >> 8, n_out);
            fwrite(outb, 1, n_out, out_file);
        }
    } else if(i == 1) {
        int c;
        while((c = getc(in_file)) != EOF) {
            n_in = c << 24;
            n_in |= getc(in_file) << 16;
            n_in |= getc(in_file) << 8;
            n_in |= getc(in_file);
            fread(inb, 1, n_in, in_file);
            lzrw1_decompress(inb, n_in, outb, &n_out);
            fwrite(outb, 1, n_out, out_file);
        }
    }
}

/******************************************************************************/
void LZRW1::fast_copy(unsigned char *p_src, unsigned char *p_dst, unsigned long len) {
    while(len--) {
        *p_dst++ = *p_src++;
    }
}

/******************************************************************************/
void LZRW1::lzrw1_compress(unsigned char *p_src_first, unsigned int src_len, unsigned char *p_dst_first, unsigned int *p_dst_len)
#define PS *p++ != *s++     /* Body of inner unrolled matching loop.         */
#define ITEMMAX 16          /* Maximum number of bytes in an expanded item.  */
{
    unsigned char *p_src = p_src_first;
    unsigned char *p_dst = p_dst_first;
    unsigned char *p_src_post = p_src_first + src_len;
    unsigned char *p_dst_post = p_dst_first + src_len;
    unsigned char *p_src_max1 = p_src_post - ITEMMAX;
    unsigned char *p_src_max16 = p_src_post - 16 * ITEMMAX;
    unsigned char *hash[4096] = {0};
    unsigned char *p_control;
    unsigned short control = 0;
    unsigned short control_bits = 0;
    *p_dst = FLAG_COMPRESS;
    p_dst += FLAG_BYTES;
    p_control = p_dst;
    p_dst += 2;
    while(1) {
        unsigned char *p;
        unsigned char *s;
        unsigned short unroll = 16;
        unsigned short len;
        unsigned short index;
        unsigned int offset;
        if(p_dst > p_dst_post) {
            goto overrun;
        }
        if(p_src > p_src_max16) {
            unroll = 1;
            if(p_src > p_src_max1) {
                if(p_src == p_src_post) {
                    break;
                }
                goto literal;
            }
        }
        begin_unrolled_loop:
        index = ((40543 * ((((p_src[0] << 4) ^ p_src[1]) << 4) ^ p_src[2])) >> 4) & 0xFFF;
        p = hash[index];
        hash[index] = s = p_src;
        offset = s - p;
        if((offset > 4095) || (p < p_src_first) || (offset == 0) || PS || PS || PS) {
            literal:
            *p_dst++ = *p_src++;
            control >>= 1;
            control_bits++;
        } else {
            PS || PS || PS || PS || PS || PS || PS || PS || PS || PS || PS || PS || PS || s++;
            len = s - p_src - 1;
            *p_dst++ = ((offset & 0xF00) >> 4) + (len - 1);
            *p_dst++ = offset & 0xFF;
            p_src += len;
            control = (control >> 1) | 0x8000;
            control_bits++;
        }
        end_unrolled_loop:
        if(--unroll) {
            goto begin_unrolled_loop;
        }
        if(control_bits == 16) {
            *p_control = control & 0xFF;
            *(p_control + 1) = control >> 8;
            p_control = p_dst;
            p_dst += 2;
            control = control_bits = 0;
        }
    }
    control >>= 16 - control_bits;
    *p_control++ = control & 0xFF;
    *p_control++ = control >> 8;
    if(p_control == p_dst) {
        p_dst -= 2;
    }
    *p_dst_len = p_dst - p_dst_first;
    return;
    overrun:
    fast_copy(p_src_first, (p_dst_first + FLAG_BYTES), src_len);
    *p_dst_first = FLAG_COPY;
    *p_dst_len = src_len + FLAG_BYTES;
}

/******************************************************************************/
void LZRW1::lzrw1_decompress(unsigned char *p_src_first, unsigned int src_len, unsigned char *p_dst_first, unsigned int *p_dst_len) {
    unsigned short controlbits = 0;
    unsigned short control;
    unsigned char *p_src = p_src_first + FLAG_BYTES;
    unsigned char *p_dst = p_dst_first;
    unsigned char *p_src_post = p_src_first + src_len;
    if(*p_src_first == FLAG_COPY) {
        fast_copy((p_src_first + FLAG_BYTES), p_dst_first, (src_len - FLAG_BYTES));
        *p_dst_len = src_len - FLAG_BYTES;
        return;
    }
    while(p_src != p_src_post) {
        if(controlbits == 0) {
            control = *p_src++;
            control |= (*p_src++) << 8;
            controlbits = 16;
        }
        if(control & 1) {
            unsigned short offset;
            unsigned short len;
            unsigned char *p;
            offset = (*p_src & 0xF0) << 4;
            len = 1 + (*p_src++&0xF);
            offset += *p_src++ & 0xFF;
            p = p_dst - offset;
            while(len--) {
                *p_dst++ = *p++;
            }
        } else {
            *p_dst++ = *p_src++;
        }
        control >>= 1;
        controlbits--;
    }
    *p_dst_len = p_dst - p_dst_first;
}
