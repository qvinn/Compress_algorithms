#include "main.h"
#include "lzari.h"
#include "lzh.h"
#include "lzw.h"
#include "lzss.h"
#include "lz77.h"
#include "rle.h"
#include "huffman.h"
#include "huffman_adaptive.h"
#include "arithmetic_adaptive.h"

unsigned int start_time;

void start_clock() {
    start_time =  clock();          // начальное время
}
void stop_clock() {
    unsigned int end_time = clock();                    // конечное время
    unsigned int search_time = end_time - start_time;   // искомое время
    printf("time_operation in ticks: %d ticks\n", search_time);
    printf("time_operation in seconds: %f seconds\n", (float)search_time / CLOCKS_PER_SEC);
}

void lzari_test() {
    // РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ LZAri
    printf("START LZARI \n");
    LZAri lzari;

    lzari.infile = fopen("file.txt", "rb");
    if(lzari.infile == nullptr){printf("File does not exist!");}
    lzari.outfile =  fopen("file_R.txt", "wb");

    printf("Encode: \n");
    start_clock();
    lzari.Encode();
    stop_clock();

    fclose(lzari.infile);
    fclose(lzari.outfile);
    // Обновим переменные, необходимые для вычеслений
    lzari.textsize = 0; lzari.codesize = 0; lzari.printcount = 0;
    lzari.low = 0; lzari.high = Q4; lzari.value = 0;
    lzari.shifts = 0;
    //------------------------------------------------------------
    lzari.infile = fopen("file_R.txt", "rb");
    if(lzari.infile == nullptr){printf("File does not exist!");}
    lzari.outfile =  fopen("file_N.txt", "wb");

    printf("Decode: \n");
    start_clock();
    lzari.Decode();
    stop_clock();

    fclose(lzari.infile);
    fclose(lzari.outfile);

    lzari.~LZAri();
    printf("FINISH LZARI \n");
}

void lzh_test() {
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ LZH
    printf("START LZH \n");
    LZH lzh;

    lzh.infile = fopen("file.txt", "rb");
    if(lzh.infile == nullptr){printf("File does not exist!");}
    lzh.outfile =  fopen("file_R.txt", "wb");

    printf("Encode: \n");
    start_clock();
    lzh.Encode();
    stop_clock();

    fclose(lzh.infile);
    fclose(lzh.outfile);
    // Обновим переменные, необходимые для вычеслений
    lzh.textsize = 0; lzh.codesize = 0; lzh.printcount = 0;
    lzh.getbuf = 0;
    lzh.getlen = 0;
    lzh.putbuf = 0;
    lzh.putlen = 0;
    ------------------------------------------------------------
    lzh.infile = fopen("file_R.txt", "rb");
    if(lzh.infile == nullptr){printf("File does not exist!");}
    lzh.outfile =  fopen("file_N.txt", "wb");

    printf("Decode: \n");
    start_clock();
    lzh.Decode();
    stop_clock();

    fclose(lzh.infile);
    fclose(lzh.outfile);

    lzh.~LZH();
    printf("FINISH LZH \n");
}

void lzw_test() {
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ LZW
    printf("START LZW \n");
    LZW lzw;

    FILE *infile;
    BFILE *outfile;

    infile = fopen("file.txt", "rb");
    if(infile == nullptr){printf("File does not exist!");}
    outfile = lzw.OpenOutputBFile("file_R.txt");

    printf("Encode: \n");
    start_clock();                 // начальное время
    lzw.CompressFile(infile,outfile);
    stop_clock();

    fclose(infile);
    lzw.CloseOutputBFile(outfile);

    BFILE *infile_1;
    FILE *outfile_1;

    infile_1 = lzw.OpenInputBFile("file_R.txt");
    if(infile_1 == nullptr){printf("File does not exist!");}
    outfile_1 =  fopen("file_N.txt", "wb");

    printf("Decode: \n");
    start_clock();                 // начальное время
    lzw.ExpandFile(infile_1,outfile_1);
    stop_clock();

    lzw.CloseOutputBFile(infile_1);
    fclose(outfile_1);

    lzw.~LZW();
    printf("FINISH LZW \n");
}

void lzss_test() {
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ LZSS
    printf("START LZSS \n");

    LZSS lzss;
    lzss.infile = fopen("file.txt", "rb");
    if(lzss.infile == nullptr){printf("File does not exist!");}
    lzss.outfile =  fopen("file_R.txt", "wb");

    printf("Encode: \n");
    start_clock();                 // начальное время
    lzss.lzss_encode();
    stop_clock();

    fclose(lzss.infile);
    fclose(lzss.outfile);

    lzss.infile = fopen("file_R.txt", "rb");
    if(lzss.infile == nullptr){printf("File does not exist!");}
    lzss.outfile =  fopen("file_N.txt", "wb");

    printf("Decode: \n");
    start_clock();                // начальное время
    lzss.lzss_decode();
    stop_clock();

    fclose(lzss.infile);
    fclose(lzss.outfile);

    lzss.~LZSS();
    printf("FINISH LZSS \n");
}

void lz77_test() {
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ LZ77
    printf("START LZ77 \n");

    LZ77 lz77;
    lz77.in_file = open("file.txt", _O_BINARY | _O_RDWR, _S_IREAD | _S_IWRITE);
    lz77.out_file = open("file_R.txt", _O_BINARY | _O_WRONLY | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE);

    printf("Encode: \n");
    start_clock();                 // начальное время
    lz77.assign_write(lz77.out_file);
    lz77.encode();
    stop_clock();

    close(lz77.in_file);
    close(lz77.out_file);

    lz77.dict_pos = 0;

    lz77.in_file = open("file_R.txt", _O_BINARY | _O_RDWR, _S_IREAD | _S_IWRITE);
    lz77.out_file = open("file_N.txt", _O_BINARY | _O_WRONLY | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE);

    printf("Decode: \n");
    start_clock();                // начальное время
    lz77.assign_read(lz77.in_file);
    lz77.decode();
    stop_clock();

    close(lz77.in_file);
    close(lz77.out_file);

    lz77.~LZ77();
    printf("FINISH LZ77 \n");
}

void rle_test() {
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ RLE
    printf("START RLE \n");

    RLE rle;
    rle.source_file = fopen("file.txt", "rb");
    if(rle.source_file == nullptr) {
        printf("File does not exist!");
    }
    rle.dest_file =  fopen("file_R.txt", "wb");

    printf("Encode: \n");
    start_clock();                              // начальное время
    rle.rle1encoding();
    stop_clock();

    fclose(rle.source_file);
    fclose(rle.dest_file);

    rle.byte_stored_status = FALSE;

    rle.source_file = fopen("file_R.txt", "rb");
    if(rle.source_file == nullptr) {
        printf("File does not exist!");
    }
    rle.dest_file =  fopen("file_N.txt", "wb");

    printf("Decode: \n");
    start_clock();                // начальное время
    rle.rle1decoding();
    stop_clock();

    fclose(rle.source_file);
    fclose(rle.dest_file);

    rle.~RLE();
    printf("FINISH RLE \n");
}

void huffman_test() {
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ HUFFMAN
    printf("START HUFFMAN \n");

    HUFFMAN huffman;
    huffman.ifile = fopen("file.txt", "rb");
    if(huffman.ifile == nullptr) {
        printf("File does not exist!");
    }
    huffman.ofile =  fopen("file_R.txt", "wb");

    printf("Encode: \n");
    start_clock();                // начальное время
    huffman.Encode();
    stop_clock();

    fclose(huffman.ifile);
    fclose(huffman.ofile);

    huffman.ifile = fopen("file_R.txt", "rb");
    if(huffman.ifile == nullptr) {
        printf("File does not exist!");
    }
    huffman.ofile =  fopen("file_N.txt", "wb");

    printf("Decode: \n");
    start_clock();                // начальное время
    huffman.Decode();
    stop_clock();

    fclose(huffman.ifile);
    fclose(huffman.ofile);

    huffman.~HUFFMAN();
    printf("FINISH HUFFMAN \n");
}

void huffman_addaptive_test() {
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ HUFFMAN_ADAPTIVE
    printf("START HUFFMAN_ADAPTIVE \n");

    HUFFMAN_ADAPTIVE huffman_adaptive;
    FILE *infile;
    COMPRESSED_FILE *outfile;

    infile = fopen("file.txt", "rb");
    if(infile == nullptr) {
        printf("File does not exist!");
    }
    outfile = huffman_adaptive.OpenOutputCompressedFile("file_R.txt");

    printf("Encode: \n");
    start_clock();                // начальное время
    huffman_adaptive.CompressFile(infile, outfile);
    stop_clock();

    fclose(infile);
    huffman_adaptive.CloseOutputCompressedFile(outfile);

    COMPRESSED_FILE *infile_1;
    FILE *outfile_1;
    infile_1 = huffman_adaptive.OpenInputCompressedFile("file_R.txt");
    outfile_1 =  fopen("file_N.txt", "wb");

    printf("Decode: \n");
    start_clock();                // начальное время
    huffman_adaptive.ExpandFile(infile_1, outfile_1);
    stop_clock();

    huffman_adaptive.CloseOutputCompressedFile(infile_1);
    fclose(outfile_1);

    huffman_adaptive.~HUFFMAN_ADAPTIVE();
    printf("FINISH HUFFMAN_ADAPTIVE \n");
}

void arithmetic_addaptive_test() {
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ ARITHMETIC_ADAPTIVE
    printf("START ARITHMETIC_ADAPTIVE \n");

    ARITHMETIC_ADAPTIVE arithmetic_adaptive;

    printf("Encode: \n");
    start_clock();                // начальное время
    arithmetic_adaptive.encode("file.txt", "file_R.txt");
    stop_clock();

    printf("Decode: \n");
    start_clock();                // начальное время
    arithmetic_adaptive.decode("file_R.txt", "file_N.txt");
    stop_clock();

    arithmetic_adaptive.~ARITHMETIC_ADAPTIVE();
    printf("FINISH ARITHMETIC_ADAPTIVE \n");
}

int main(int argc, char *argv[]) {
    //lzari_test();
    //lzh_test();
    //lzw_test();
    //lzss_test();
    //lz77_test();
    //rle_test();
    //huffman_test();
    //huffman_addaptive_test();
    arithmetic_addaptive_test();
    return EXIT_SUCCESS;
}
