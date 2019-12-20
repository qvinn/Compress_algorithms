#include "lzh.h"

LZH::LZH() {}
LZH::~LZH() {}

void LZH::reset_vars() {
    textsize = 0;
    codesize = 0;
    printcount = 0;
    getbuf = 0;
    getlen = 0;
    putbuf = 0;
    putlen = 0;
}

void LZH::Error(char *message) {
    printf("\n%s\n", message);
    exit(EXIT_FAILED);
}

void LZH::InitTree(void) {  /* initialize trees */
    int i;
    for(i = N + 1; i <= N + 256; i++) {
        rson[i] = NIL;          /* root */
    }
    for(i = 0; i < N; i++) {
        dad[i] = NIL;           /* node */
    }
}

void LZH::InsertNode(int r) {  /* insert to tree */
    int i, p, cmp;
    unsigned char  *key;
    unsigned int c;
    cmp = 1;
    key = &text_buf[r];
    p = N + 1 + key[0];
    rson[r] = lson[r] = NIL;
    match_length = 0;
    for( ; ; ) {
        if(cmp >= 0) {
            if(rson[p] != NIL) {
                p = rson[p];
            } else {
                rson[p] = r;
                dad[r] = p;
                return;
            }
        } else {
            if(lson[p] != NIL) {
                p = lson[p];
            } else {
                lson[p] = r;
                dad[r] = p;
                return;
            }
        }
        for(i = 1; i < F; i++) {
            if ((cmp = key[i] - text_buf[p + i]) != 0) {
                break;
            }
        }
        if(i > THRESHOLD) {
            if(i > match_length) {
                match_position = ((r - p) & (N - 1)) - 1;
                if((match_length = i) >= F) {
                    break;
                }
            }
            if(i == match_length) {
                if((c = ((r - p) & (N - 1)) - 1) < match_position) {
                    match_position = c;
                }
            }
        }
    }
    dad[r] = dad[p];
    lson[r] = lson[p];
    rson[r] = rson[p];
    dad[lson[p]] = r;
    dad[rson[p]] = r;
    if(rson[dad[p]] == p) {
        rson[dad[p]] = r;
    } else {
        lson[dad[p]] = r;
    }
    dad[p] = NIL;  /* remove p */
}

void LZH::DeleteNode(int p) {            /* remove from tree */
    int q;
    if(dad[p] == NIL) {
        return;         /* not registered */
    }
    if(rson[p] == NIL) {
        q = lson[p];
    } else {
        if(lson[p] == NIL) {
            q = rson[p];
        } else {
            q = lson[p];
            if(rson[q] != NIL) {
                do {
                    q = rson[q];
                } while(rson[q] != NIL);
                rson[dad[q]] = lson[q];
                dad[lson[q]] = dad[q];
                lson[q] = lson[p];
                dad[lson[p]] = q;
            }
            rson[q] = rson[p];
            dad[rson[p]] = q;
        }
    }
    dad[q] = dad[p];
    if(rson[dad[p]] == p) {
        rson[dad[p]] = q;
    } else {
        lson[dad[p]] = q;
    }
    dad[p] = NIL;
}

unsigned short LZH::GetBit() {   /* get one bit */
    unsigned short i;
    while(getlen <= 8) {
        /////////////////////////////////////////////////////////////////////i = *input++;
        if((i = getc(infile)) < 0) {
            i = 0;
        }
        getbuf |= i << (8 - getlen);
        getlen += 8;
    }
    i = getbuf;
    getbuf <<= 1;
    getlen--;
    return (i >> 15);
}

unsigned short LZH::GetByte() {  /* get one byte */
    unsigned short i;
    while(getlen <= 8) {
        /////////////////////////////////////////////////////////////////////i = *input++;
        if((i = getc(infile)) < 0) {
            i = 0;
        }
        getbuf |= i << (8 - getlen);
        getlen += 8;
    }
    i = getbuf;
    getbuf <<= 8;
    getlen -= 8;
    return (i >> 8);
}

void LZH::Putcode(int l, unsigned int c) {    /* output c bits of code */
    putbuf |= c >> putlen;
    if((putlen += l) >= 8) {
        //////////////////////////////output[codesize++] = (putbuf >> 8);
        putc(putbuf >> 8, outfile);
        if((putlen -= 8) >= 8) {
            /////////////////////////////////////////output[codesize++] = putbuf;

            putc(putbuf, outfile);
            codesize += 2;

            putlen -= 8;
            putbuf = c << (l - putlen);
        } else {
            putbuf <<= 8;
        }
    }
}

/* initialization of tree */
void LZH::StartHuff(void) {
    unsigned short i, j;
    for(i = 0; i < N_CHAR; i++) {
        freq[i] = 1;
        son[i] = i + T;
        prnt[i + T] = i;
    }
    i = 0; j = N_CHAR;
    while(j <= R) {
        freq[j] = freq[i] + freq[i + 1];
        son[j] = i;
        prnt[i] = prnt[i + 1] = j;
        i += 2;
        j++;
    }
    freq[T] = 0xffff;
    prnt[R] = 0;
}

/* reconstruction of tree */
void LZH::reconst(void) {
    int i, j, k;
    unsigned f, l;
    j = 0;                      /* collect leaf nodes in the first half of the table and replace the freq by (freq + 1) / 2. */
    for(i = 0; i < T; i++) {
        if(son[i] >= T) {
            freq[j] = (freq[i] + 1) / 2;
            son[j] = son[i];
            j++;
        }
    }
    for(i = 0, j = N_CHAR; j < T; i += 2, j++) {           /* begin constructing tree by connecting sons */
        k = i + 1;
        f = freq[j] = freq[i] + freq[k];
        for(k = j - 1; f < freq[k]; k--);
        k++;
        l = (j - k) * 2;
        memmove(&freq[k + 1], &freq[k], l);
        freq[k] = f;
        memmove(&son[k + 1], &son[k], l);
        son[k] = i;
    }
    for(i = 0; i < T; i++) {               /* connect prnt */
        if((k = son[i]) >= T) {
            prnt[k] = i;
        } else {
            prnt[k] = prnt[k + 1] = i;
        }
    }
}


/* increment frequency of given code by one, and update tree */
void LZH::update(unsigned int c) {
    unsigned int i, j, k, l;
    if(freq[R] == MAX_FREQ) {
        reconst();
    }
    c = prnt[c + T];
    do {
        k = ++freq[c];
        if(k > freq[l = c + 1]) {          /* if the order is disturbed, exchange nodes */
            while(k > freq[++l]);
            l--;
            freq[c] = freq[l];
            freq[l] = k;
            i = son[c];
            prnt[i] = l;
            if(i < T) {
                prnt[i + 1] = l;
            }
            j = son[l];
            son[l] = i;
            prnt[j] = c;
            if(j < T) {
                prnt[j + 1] = c;
            }
            son[c] = j;
            c = l;
        }
    } while((c = prnt[c]) != 0);   /* repeat up to root */
}

void LZH::EncodeChar(unsigned c) {
    unsigned i;
    int j, k;
    i = 0;
    j = 0;
    k = prnt[c + T];
    do {            /* travel from leaf to root */
        i >>= 1;
        if(k & 1) {             /* if node's address is odd-numbered, choose bigger brother node */
            i += 0x8000;
        }
        j++;
    } while((k = prnt[k]) != R);
    Putcode(j, i);
    code = i;
    len = j;
    update(c);
}

void LZH::EncodePosition(unsigned c) {
    unsigned i;
    i = c >> 6;                                     /* output upper 6 bits by table lookup */
    Putcode(p_len[i], (unsigned)p_code[i] << 8);
    Putcode(6, (c & 0x3f) << 10);                   /* output lower 6 bits verbatim */
}

void LZH::EncodeEnd(void) {
    if(putlen) {
        //output[codesize++] = (putbuf >> 8);
        putc(putbuf >> 8, outfile);
        codesize++;
    }
}

unsigned int LZH::DecodeChar() {
    unsigned int c;
    c = son[R];
    while(c < T) {              /* travel from root to leaf, choosing the smaller child node (son[]) if the read bit is 0, the bigger (son[]+1} if 1 */
        c += GetBit();
        c = son[c];
    }
    c -= T;
    update(c);
    return c;
}

unsigned int LZH::DecodePosition() {
    unsigned int i, j, c;
    i = GetByte();                      /* recover upper 6 bits from table */
    c = (unsigned int)d_code[i] << 6;
    j = d_len[i];
    j -= 2;                             /* read lower 6 bits verbatim */
    while(j--) {
        i = (i << 1) + GetBit();
    }
    return c | (i & 0x3f);
}

///* compression */
//unsigned int LZH::Encode(unsigned char *in, unsigned int inlen, unsigned char *out) {
//    int i, c, len, r, s, last_match_length;
//    unsigned int offset = 0;
//    if(inlen == 0) {
//        return 0;
//    }
//    input = in;
//    output = out;
//    *(unsigned int*)output = inlen;
//    codesize += 4;
//    textsize = 0;
//    StartHuff();
//    InitTree();
//    s = 0;
//    r = N - F;
//    for(i = s; i < r; i++) {
//        text_buf[i] = ' ';
//    }
//    for(len = 0; len < F && len < inlen; len++) {
//        text_buf[r + len] = input[offset++];
//    }
//    textsize = len;
//    for(i = 1; i <= F; i++) {
//        InsertNode(r - i);
//    }
//    InsertNode(r);
//    do {
//        if(match_length > len) {
//            match_length = len;
//        }
//        if(match_length <= THRESHOLD) {
//            match_length = 1;
//            EncodeChar(text_buf[r]);
//        } else {
//            EncodeChar(255 - THRESHOLD + match_length);
//            EncodePosition(match_position);
//        }
//        last_match_length = match_length;
//        for(i = 0; i < last_match_length && offset < inlen; i++) {
//            c = input[offset++];
//            DeleteNode(s);
//            text_buf[s] = c;
//            if(s < F - 1) {
//                text_buf[s + N] = c;
//            }
//            s = (s + 1) & (N - 1);
//            r = (r + 1) & (N - 1);
//            InsertNode(r);
//        }
//        while(i++ < last_match_length) {
//            DeleteNode(s);
//            s = (s + 1) & (N - 1);
//            r = (r + 1) & (N - 1);
//            if (--len) InsertNode(r);
//        }
//    } while(len > 0);
//    EncodeEnd();
//    return codesize;
//}

/*const*/ unsigned int LZH::DecodedLength(unsigned char *in) {
    return *(unsigned int*)in;
}

///* decompression */
//unsigned int LZH::Decode(unsigned char *in, unsigned char *out) {
//    unsigned int c, count;
//    unsigned int i, j, k, r;
//    input = in;
//    textsize = *(unsigned int*)input;
//    input += 4;
//    if(textsize == 0) {
//        return 0;
//    }
//    StartHuff();
//    for(i = 0; i < N - F; i++) {
//        text_buf[i] = ' ';
//    }
//    r = N-F;
//    for(count = 0; count < textsize; ) {
//        c = DecodeChar();
//        if(c < 256) {
//            out[count] = c;
//            text_buf[r++] = c;
//            r &= (N - 1);
//            count++;
//        } else {
//            i = (r - DecodePosition() - 1) & (N-1);
//            j = c - 255 + THRESHOLD;
//            for(k = 0; k < j; k++) {
//                c = text_buf[(i + k) & (N -1)];
//                out[count] = c;
//                text_buf[r++] = c;
//                r &= (N - 1);
//                count++;
//            }
//        }
//    }
//    return count;
//}

//---------------------------------------------------------
/* Компрессия */
void LZH::Encode(void) {
    int i, c, len, r, s, last_match_length;
    fseek(infile, 0L, 2);
    textsize = ftell(infile);
    if(fwrite(&textsize, sizeof textsize, 1, outfile) < 1) {
        Error("Unable to write");
    }
    if(textsize == 0) {
        return;
    }
    rewind(infile);
    textsize = 0;
    StartHuff();
    InitTree();
    s = 0;
    r = N - F;
    for(i = s; i < r; i++) {
        text_buf[i] = ' ';
    }
    for(len = 0; len < F && (c = getc(infile)) != EOF; len++) {
        text_buf[r + len] = c; textsize = len;
    }
    for(i = 1; i <= F; i++) {
        InsertNode(r - i);
    }
    InsertNode(r);
    do {
        if(match_length > len) {
            match_length = len;
        }
        if(match_length <= THRESHOLD) {
            match_length = 1; EncodeChar(text_buf[r]);
        } else {
            EncodeChar(255 - THRESHOLD + match_length);
            EncodePosition(match_position);
        }
        last_match_length = match_length;
        for(i = 0; i < last_match_length && (c = getc(infile)) != EOF; i++) {
            DeleteNode(s);
            text_buf[s] = c;
            if(s < F - 1) {
                text_buf[s + N] = c;
            }
            s = (s + 1) & (N - 1);
            r = (r + 1) & (N - 1);
            InsertNode(r);
        }
        if((textsize += i) > printcount) {
            printf("%12ld\r", textsize);
            printcount += 1024;
        }
        while(i++ < last_match_length) {
            DeleteNode(s);
            s = (s + 1) & (N - 1);
            r = (r + 1) & (N - 1);
            if(--len) {
                InsertNode(r);
            }
        }
    } while(len > 0);
    EncodeEnd();
    printf("input: %ld bytes\n", textsize);
    printf("output: %ld bytes\n", codesize);
    printf("output/input: %.3f\n", (double)codesize / textsize);
}

//---------------------------------------------------------
/* Декомпрессия */
void LZH::Decode(void) {
    int i, j, k, r, c;
    unsigned long int count;
    if(fread(&textsize, sizeof textsize, 1, infile) < 1) {
        Error("Unable to read");
    }
    if(textsize == 0) {  /* read size of original text */
        return;
    }
    StartHuff();
    for(i = 0; i < N - F; i++) {
        text_buf[i] = ' ';
    }
    r = N - F;
    for(count = 0; count < textsize; ) {
        c = DecodeChar();
        if(c < 256) {
            putc(c, outfile);
            text_buf[r++] = c;
            r &= (N - 1); count++;
        } else {
            i = (r - DecodePosition() - 1) & (N - 1);
            j = c - 255 + THRESHOLD;
            for(k = 0; k < j; k++) {
                c = text_buf[(i + k) & (N - 1)];
                putc(c, outfile);
                text_buf[r++] = c;
                r &= (N - 1); count++;
            }
        }
        if(count > printcount) {
            printf("%12ld\r", count);
            printcount += 1024;
        }
    }
    printf("%12ld\n", count);
}
