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

using namespace std;

unsigned int start_time;

struct operation_info {
    long start_size;
    long final_size;
    float percent_compression;
    unsigned int time_encode;
    unsigned int time_decode;
    };

unsigned int start_clock() {
    start_time =  clock();                              // начальное время
    return start_time;
}
unsigned int stop_clock() {
    unsigned int end_time = clock();                    // конечное время
    unsigned int search_time = end_time - start_time;   // искомое время
    printf("time_operation in ticks: %d ticks\n", search_time);
    printf("time_operation in seconds: %f seconds\n", (float)search_time / CLOCKS_PER_SEC);
    return search_time;
}

// Создать файл для логгирования результатов + подготовить поля
void create_log(char *name_logfile)
{
    std::ofstream out;      // поток для записи
    out.open(name_logfile);          // окрываем файл для записи
    if (out.is_open())
        {
            out << "start_size,final_size,percent_compression,time_encode,time_decode" << std::endl;
        }
    out.close();            // закрываем файл после записи
}

// Добавить в лог-файл строку с информацией
void add_info(char *name_logfile, operation_info info)
{
    //char *name_logfile, long start_size, long final_size, float percent_compression, unsigned int time_encode, unsigned int time_decode
    std::ofstream out;      // поток для записи
    out.open(name_logfile, ios_base::app);          // окрываем файл для записи
    if (out.is_open())
        {
            out << info.start_size<<"|"<<info.final_size<<"|"<<info.percent_compression<<"|"<<info.time_encode<<"|"<<info.time_decode << std::endl;
        }
    out.close();
}

// Посчитаем который процент от изначально размера остался
float calculate_percent_compression(long start_size, long final_size)
{
    float result = (100 * final_size)/start_size;
    return 1;
}

// Узнаем размер файла
std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

// Узнаем размер файла - версия_2 (как по мне, более нативная, ибо возвращаяет "long")
long GetFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

void lzari_test(char *file, char *logfile) {
    operation_info info;                                // Создадим структуру
    info.start_size = GetFileSize(file);                // Запишем в структуру начальный размер файла
    // РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ LZAri
    printf("START LZARI \n");
    LZAri lzari;

    lzari.infile = fopen(file, "rb");
    if(lzari.infile == nullptr){printf("File does not exist!");}
    lzari.outfile =  fopen("file_R.txt", "wb");

    printf("Encode: \n");
    start_clock();
    lzari.Encode();
    info.time_encode = stop_clock();                    // Запишем в структуру время архивации

    fclose(lzari.infile);
    fclose(lzari.outfile);
    info.final_size = GetFileSize("file_R.txt");        // Запишем в структуру начальный размер файла
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
    info.time_decode = stop_clock();                    // Запишем в структуру время деархивации

    fclose(lzari.infile);
    fclose(lzari.outfile);

    info.percent_compression = calculate_percent_compression(info.start_size, info.final_size);   // Запишем в структуру процент "полезности"
    add_info(logfile, info);                            // Сохранияем результаты опреций в лог файл

    lzari.~LZAri();
    printf("FINISH LZARI \n");
}

void lzh_test(char *file, char *logfile) {
    operation_info info;                                // Создадим структуру
    info.start_size = GetFileSize(file);                // Запишем в структуру начальный размер файла
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ LZH
    printf("START LZH \n");
    LZH lzh;

    lzh.infile = fopen(file, "rb");
    if(lzh.infile == nullptr){printf("File does not exist!");}
    lzh.outfile =  fopen("file_R.txt", "wb");

    printf("Encode: \n");
    start_clock();
    lzh.Encode();
    info.time_encode = stop_clock();                    // Запишем в структуру время архивации

    fclose(lzh.infile);
    fclose(lzh.outfile);
    info.final_size = GetFileSize("file_R.txt");        // Запишем в структуру начальный размер файла
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
    info.time_decode = stop_clock();                    // Запишем в структуру время деархивации

    fclose(lzh.infile);
    fclose(lzh.outfile);

    info.percent_compression = calculate_percent_compression(info.start_size, info.final_size);   // Запишем в структуру процент "полезности"
    add_info(logfile, info);                            // Сохранияем результаты опреций в лог файл

    lzh.~LZH();
    printf("FINISH LZH \n");
}

void lzw_test(char *file, char *logfile) {
    operation_info info;                                // Создадим структуру
    info.start_size = GetFileSize(file);                // Запишем в структуру начальный размер файла
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ LZW
    printf("START LZW \n");
    LZW lzw;

    FILE *infile;
    BFILE *outfile;

    infile = fopen(file, "rb");
    if(infile == nullptr){printf("File does not exist!");}
    outfile = lzw.OpenOutputBFile("file_R.txt");

    printf("Encode: \n");
    start_clock();                 // начальное время
    lzw.CompressFile(infile,outfile);
    info.time_encode = stop_clock();                    // Запишем в структуру время архивации

    fclose(infile);
    lzw.CloseOutputBFile(outfile);
    info.final_size = GetFileSize("file_R.txt");        // Запишем в структуру начальный размер файла

    BFILE *infile_1;
    FILE *outfile_1;

    infile_1 = lzw.OpenInputBFile("file_R.txt");
    if(infile_1 == nullptr){printf("File does not exist!");}
    outfile_1 =  fopen("file_N.txt", "wb");

    printf("Decode: \n");
    start_clock();                 // начальное время
    lzw.ExpandFile(infile_1,outfile_1);
    info.time_decode = stop_clock();                    // Запишем в структуру время деархивации

    lzw.CloseOutputBFile(infile_1);
    fclose(outfile_1);

    info.percent_compression = calculate_percent_compression(info.start_size, info.final_size);   // Запишем в структуру процент "полезности"
    add_info(logfile, info);                            // Сохранияем результаты опреций в лог файл

    lzw.~LZW();
    printf("FINISH LZW \n");
}

void lzss_test(char *file, char *logfile) {
    operation_info info;                                // Создадим структуру
    info.start_size = GetFileSize(file);                // Запишем в структуру начальный размер файла
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ LZSS
    printf("START LZSS \n");

    LZSS lzss;
    lzss.infile = fopen(file, "rb");
    if(lzss.infile == nullptr){printf("File does not exist!");}
    lzss.outfile =  fopen("file_R.txt", "wb");

    printf("Encode: \n");
    start_clock();                 // начальное время
    lzss.lzss_encode();
    info.time_encode = stop_clock();                    // Запишем в структуру время архивации

    fclose(lzss.infile);
    fclose(lzss.outfile);
    info.final_size = GetFileSize("file_R.txt");        // Запишем в структуру начальный размер файла

    lzss.infile = fopen("file_R.txt", "rb");
    if(lzss.infile == nullptr){printf("File does not exist!");}
    lzss.outfile =  fopen("file_N.txt", "wb");

    printf("Decode: \n");
    start_clock();                // начальное время
    lzss.lzss_decode();
    info.time_decode = stop_clock();                    // Запишем в структуру время деархивации

    fclose(lzss.infile);
    fclose(lzss.outfile);

    info.percent_compression = calculate_percent_compression(info.start_size, info.final_size);   // Запишем в структуру процент "полезности"
    add_info(logfile, info);                            // Сохранияем результаты опреций в лог файл

    lzss.~LZSS();
    printf("FINISH LZSS \n");
}

void lz77_test(char *file, char *logfile) {
    operation_info info;                                // Создадим структуру
    info.start_size = GetFileSize(file);                // Запишем в структуру начальный размер файла
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ LZ77
    printf("START LZ77 \n");

    LZ77 lz77;
    lz77.in_file = open(file, _O_BINARY | _O_RDWR, _S_IREAD | _S_IWRITE);
    lz77.out_file = open("file_R.txt", _O_BINARY | _O_WRONLY | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE);

    printf("Encode: \n");
    start_clock();                 // начальное время
    lz77.assign_write(lz77.out_file);
    lz77.encode();
    info.time_encode = stop_clock();                    // Запишем в структуру время архивации

    close(lz77.in_file);
    close(lz77.out_file);
    info.final_size = GetFileSize("file_R.txt");        // Запишем в структуру начальный размер файла

    lz77.dict_pos = 0;

    lz77.in_file = open("file_R.txt", _O_BINARY | _O_RDWR, _S_IREAD | _S_IWRITE);
    lz77.out_file = open("file_N.txt", _O_BINARY | _O_WRONLY | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE);

    printf("Decode: \n");
    start_clock();                // начальное время
    lz77.assign_read(lz77.in_file);
    lz77.decode();
    info.time_decode = stop_clock();                    // Запишем в структуру время деархивации

    close(lz77.in_file);
    close(lz77.out_file);

    info.percent_compression = calculate_percent_compression(info.start_size, info.final_size);   // Запишем в структуру процент "полезности"
    add_info(logfile, info);                            // Сохранияем результаты опреций в лог файл

    lz77.~LZ77();
    printf("FINISH LZ77 \n");
}

void rle_test(char *file, char *logfile) {
    operation_info info;                                // Создадим структуру
    info.start_size = GetFileSize(file);                // Запишем в структуру начальный размер файла
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ RLE
    printf("START RLE \n");

    RLE rle;
    rle.source_file = fopen(file, "rb");
    if(rle.source_file == nullptr) {
        printf("File does not exist!");
    }
    rle.dest_file =  fopen("file_R.txt", "wb");

    printf("Encode: \n");
    start_clock();                              // начальное время
    rle.rle1encoding();
    info.time_encode = stop_clock();                    // Запишем в структуру время архивации

    fclose(rle.source_file);
    fclose(rle.dest_file);
    info.final_size = GetFileSize("file_R.txt");        // Запишем в структуру начальный размер файла

    rle.byte_stored_status = FALSE;

    rle.source_file = fopen("file_R.txt", "rb");
    if(rle.source_file == nullptr) {
        printf("File does not exist!");
    }
    rle.dest_file =  fopen("file_N.txt", "wb");

    printf("Decode: \n");
    start_clock();                // начальное время
    rle.rle1decoding();
    info.time_decode = stop_clock();                    // Запишем в структуру время деархивации

    fclose(rle.source_file);
    fclose(rle.dest_file);

    info.percent_compression = calculate_percent_compression(info.start_size, info.final_size);   // Запишем в структуру процент "полезности"
    add_info(logfile, info);                            // Сохранияем результаты опреций в лог файл

    rle.~RLE();
    printf("FINISH RLE \n");
}

void huffman_test(char *file, char *logfile) {
    operation_info info;                                // Создадим структуру
    info.start_size = GetFileSize(file);                // Запишем в структуру начальный размер файла
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ HUFFMAN
    printf("START HUFFMAN \n");

    HUFFMAN huffman;
    huffman.ifile = fopen(file, "rb");
    if(huffman.ifile == nullptr) {
        printf("File does not exist!");
    }
    huffman.ofile =  fopen("file_R.txt", "wb");

    printf("Encode: \n");
    start_clock();                // начальное время
    huffman.Encode();
    info.time_encode = stop_clock();                    // Запишем в структуру время архивации

    fclose(huffman.ifile);
    fclose(huffman.ofile);
    info.final_size = GetFileSize("file_R.txt");        // Запишем в структуру начальный размер файла

    huffman.ifile = fopen("file_R.txt", "rb");
    if(huffman.ifile == nullptr) {
        printf("File does not exist!");
    }
    huffman.ofile =  fopen("file_N.txt", "wb");

    printf("Decode: \n");
    start_clock();                // начальное время
    huffman.Decode();
    info.time_decode = stop_clock();                    // Запишем в структуру время деархивации

    fclose(huffman.ifile);
    fclose(huffman.ofile);

    info.percent_compression = calculate_percent_compression(info.start_size, info.final_size);   // Запишем в структуру процент "полезности"
    add_info(logfile, info);                            // Сохранияем результаты опреций в лог файл

    huffman.~HUFFMAN();
    printf("FINISH HUFFMAN \n");
}

void huffman_addaptive_test(char *file, char *logfile) {
    operation_info info;                                // Создадим структуру
    info.start_size = GetFileSize(file);                // Запишем в структуру начальный размер файла
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ HUFFMAN_ADAPTIVE
    printf("START HUFFMAN_ADAPTIVE \n");

    HUFFMAN_ADAPTIVE huffman_adaptive;
    FILE *infile;
    COMPRESSED_FILE *outfile;

    infile = fopen(file, "rb");
    if(infile == nullptr) {
        printf("File does not exist!");
    }
    outfile = huffman_adaptive.OpenOutputCompressedFile("file_R.txt");

    printf("Encode: \n");
    start_clock();                // начальное время
    huffman_adaptive.CompressFile(infile, outfile);
    info.time_encode = stop_clock();                    // Запишем в структуру время архивации

    fclose(infile);
    huffman_adaptive.CloseOutputCompressedFile(outfile);
    info.final_size = GetFileSize("file_R.txt");        // Запишем в структуру начальный размер файла

    COMPRESSED_FILE *infile_1;
    FILE *outfile_1;
    infile_1 = huffman_adaptive.OpenInputCompressedFile("file_R.txt");
    outfile_1 =  fopen("file_N.txt", "wb");

    printf("Decode: \n");
    start_clock();                // начальное время
    huffman_adaptive.ExpandFile(infile_1, outfile_1);
    info.time_decode = stop_clock();                    // Запишем в структуру время деархивации

    huffman_adaptive.CloseOutputCompressedFile(infile_1);
    fclose(outfile_1);

    info.percent_compression = calculate_percent_compression(info.start_size, info.final_size);   // Запишем в структуру процент "полезности"
    add_info(logfile, info);                            // Сохранияем результаты опреций в лог файл

    huffman_adaptive.~HUFFMAN_ADAPTIVE();
    printf("FINISH HUFFMAN_ADAPTIVE \n");
}

void arithmetic_addaptive_test(char *file, char *logfile) {
    operation_info info;                                // Создадим структуру
    info.start_size = GetFileSize(file);                // Запишем в структуру начальный размер файла
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ ARITHMETIC_ADAPTIVE
    printf("START ARITHMETIC_ADAPTIVE \n");

    ARITHMETIC_ADAPTIVE arithmetic_adaptive;

    printf("Encode: \n");
    start_clock();                // начальное время
    arithmetic_adaptive.encode(file, "file_R.txt");
    info.time_encode = stop_clock();                    // Запишем в структуру время архивации
    info.final_size = GetFileSize("file_R.txt");        // Запишем в структуру начальный размер файла

    printf("Decode: \n");
    start_clock();                // начальное время
    arithmetic_adaptive.decode("file_R.txt", "file_N.txt");
    info.time_decode = stop_clock();                    // Запишем в структуру время деархивации

    info.percent_compression = calculate_percent_compression(info.start_size, info.final_size);   // Запишем в структуру процент "полезности"
    add_info(logfile, info);                            // Сохранияем результаты опреций в лог файл

    arithmetic_adaptive.~ARITHMETIC_ADAPTIVE();
    printf("FINISH ARITHMETIC_ADAPTIVE \n");
}

void automating_tests(int number)
{
    std::string log_pattern = "log";
    std::string log_csv = ".csv";
    std::string log_file = "file";
    std::string log_txt = ".txt";
    for(int i = 1; i < number; i++)
    {
        log_pattern.append(std::to_string(i));
        log_pattern.append(log_csv);
        //cout << log_pattern << endl;
        char clogstr[log_pattern.size() + 1];
        log_pattern.copy(clogstr, log_pattern.size() + 1);
        clogstr[log_pattern.size()] = '\0';
        //cout << clogstr << endl;

        log_file.append(std::to_string(i));
        log_file.append(log_txt);
        //cout << log_file << endl;
        char ctxtstr[log_file.size() + 1];
        log_file.copy(ctxtstr, log_file.size() + 1);
        ctxtstr[log_file.size()] = '\0';
        //cout << ctxtstr << endl << endl;


        create_log(clogstr);
        lzari_test(ctxtstr, clogstr);
        //lzh_test();   // Временно не работает(вообще)
        lzw_test(ctxtstr, clogstr);
        lzss_test(ctxtstr, clogstr);
        lz77_test(ctxtstr, clogstr);
        rle_test(ctxtstr, clogstr);
        huffman_test(ctxtstr, clogstr);
        arithmetic_addaptive_test(ctxtstr, clogstr);
        //huffman_addaptive_test(ctxtstr, clogstr);(в цикле - является причиной прерывания цикла)

        log_pattern = "log";
        log_file = "file";
    }
}

int main(int argc, char *argv[]) {
     automating_tests(2); // ВНИМАНИЕ - ставить число тестов на 1 больше, чем файлов(мин. 2)
//    // Создадим ЛОГ-файл для прохождения алгоритмов
//    std::string s = "log.csv";
//    char cstr[s.size() + 1];
//    strcpy(cstr, s.c_str());	// or pass &s[0]
//    create_log(cstr);
//    // Пройдемся всеми алгоритмами по указанному файлу и соберем сатистику
//    lzari_test("file.txt", "log.csv");
//    //lzh_test();   // Временно не работает
//    lzw_test("file.txt", "log.csv");
//    lzss_test("file.txt", "log.csv");
//    lz77_test("file.txt", "log.csv");
//    rle_test("file.txt", "log.csv");
//    huffman_test("file.txt", "log.csv");
//    arithmetic_addaptive_test("file.txt", "log.csv");
//    huffman_addaptive_test("file.txt", "log.csv");
//    // Конец прохождения
//    return EXIT_SUCCESS;
}
