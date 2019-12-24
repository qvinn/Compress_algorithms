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
    char *algo_name;
    long start_size;
    long final_size;
    float percent_compression;
    unsigned int time_encode;
    unsigned int time_decode;
    int packet_loss;
    float percent_packet_loss;
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
            out << "algo_name|start_size|final_size|percent_compression|time_encode|time_decode|bytes_loss|percent_bytes_loss" << std::endl;
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
            out <<info.algo_name<<"|"<< info.start_size<<"|"<<info.final_size<<"|"<<info.percent_compression<<"|"<<info.time_encode<<"|"<<info.time_decode<<"|"<<info.packet_loss<<"|"<<info.percent_packet_loss<<"|"<<std::endl;
        }
    out.close();
}

// Посчитаем который процент от изначально размера остался
float calculate_percent_compression(long start_size, long final_size)
{
    float result = (100 * final_size)/start_size;
    return result;
}

// Посчитаем количество процентов потерянных пакетов от изначально размера
float calculate_percent_loss_packet(long start_size, int loss_packet)
{
    float result = (100 * loss_packet)/(float)start_size;
    return result;
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

bool clear(char *file_encode, char *file_decode)
{
    if(( remove(file_encode) != 0))    // удаление файлов
    {
        std::cout << "Error was occurred: " << file_encode << endl;
        return false;
    } else
    {
        std::cout << "File was deleted successfully:" << file_encode << endl;
    }

    if(( remove(file_decode) != 0))    // удаление файлов
    {
        std::cout << "Error was occurred: " << file_decode << endl;
        return false;
    } else
    {
        std::cout << "File was deleted successfully: " << file_decode << endl;
    }
    return true;
}

int compare_files(char *file,char *after_decode_file)
{
    int counter  = 0;
    FILE  *file_1, *file_2;
    if ((file_1 = fopen(file, "rb")) != nullptr)
    {
        if ((file_2 = fopen(after_decode_file, "rb")) != nullptr)
        {
            int ch1 = 0, ch2 = 0;
            while(ch1 != EOF)
            {
                ch1 = getc(file_1);
                ch2 = getc(file_2);
                if (ch1 != ch2)
                {
                    counter++;
                    //printf("Files no idents!\nNumber of different byte: %i\n", counter);
                }
            }
            counter == 0 ? printf("Two files are ident\n") : printf("Two files__________________________________________ARE NOT IDENT!\n");
            fclose(file_1);
            fclose(file_2);
        } else {
            printf("Cant open after_decode_file\n");
        }
    } else {
        printf("Cant open start_file\n");
    }
    return counter;
}

void lzari_test(char *file, char *encode_file, char *decode_file, char *logfile) {
    operation_info info;                                // Создадим структуру
    info.algo_name = "LZARI";                           // Добавим название алгоритма
    info.start_size = GetFileSize(file);                // Запишем в структуру начальный размер файла
    // РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ LZAri
    printf("START LZARI \n");
    LZAri lzari;

    // Обновим переменные, необходимые для вычеслений
    lzari.reset_vars();

    lzari.infile = fopen(file, "rb");
    if(lzari.infile == nullptr){printf("File does not exist!");}
    lzari.outfile =  fopen(encode_file, "wb");

    printf("Encode: \n");
    start_clock();
    lzari.Encode();
    info.time_encode = stop_clock();                    // Запишем в структуру время архивации файла

    fclose(lzari.infile);
    fclose(lzari.outfile);
    info.final_size = GetFileSize(encode_file);        // Запишем в структуру начальный размер файла
    // Обновим переменные, необходимые для вычеслений
    lzari.reset_vars();
    //------------------------------------------------------------
    lzari.infile = fopen(encode_file, "rb");
    if(lzari.infile == nullptr){printf("File does not exist!");}
    lzari.outfile =  fopen(decode_file, "wb");

    printf("Decode: \n");
    start_clock();
    lzari.Decode();
    info.time_decode = stop_clock();                    // Запишем в структуру время деархивации

    fclose(lzari.infile);
    fclose(lzari.outfile);

    info.percent_compression = calculate_percent_compression(info.start_size, info.final_size);   // Запишем в структуру процент "полезности"

    int packets_loss = compare_files(file,decode_file);
    float percent_packets_loss = calculate_percent_loss_packet(info.start_size,packets_loss);
    info.packet_loss = packets_loss;
    info.percent_packet_loss = percent_packets_loss;

    add_info(logfile, info);                            // Сохранияем результаты опреций в лог файл

    lzari.~LZAri();
    printf("FINISH LZARI \n");
}

void lzh_test(char *file, char *encode_file, char *decode_file, char *logfile) {
    operation_info info;                                // Создадим структуру
    info.algo_name = "LZH";                           // Добавим название алгоритма
    info.start_size = GetFileSize(file);                // Запишем в структуру начальный размер файла
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ LZH
    printf("START LZH \n");
    LZH lzh;

    // Обновим переменные, необходимые для вычеслений
    lzh.reset_vars();

    lzh.infile = fopen(file, "rb");
    if(lzh.infile == nullptr){printf("File does not exist!");}
    lzh.outfile =  fopen(encode_file, "wb");

    printf("Encode: \n");
    start_clock();
    lzh.Encode();
    info.time_encode = stop_clock();                    // Запишем в структуру время архивации

    fclose(lzh.infile);
    fclose(lzh.outfile);
    info.final_size = GetFileSize(encode_file);        // Запишем в структуру финальный размер файла

    // Обновим переменные, необходимые для вычеслений
    lzh.reset_vars();

    lzh.infile = fopen(encode_file, "rb");
    if(lzh.infile == nullptr){printf("File does not exist!");}
    lzh.outfile =  fopen(decode_file, "wb");

    printf("Decode: \n");
    start_clock();
    lzh.Decode();
    info.time_decode = stop_clock();                    // Запишем в структуру время деархивации

    fclose(lzh.infile);
    fclose(lzh.outfile);

    info.percent_compression = calculate_percent_compression(info.start_size, info.final_size);   // Запишем в структуру процент "полезности"

    int packets_loss = compare_files(file,decode_file);
    float percent_packets_loss = calculate_percent_loss_packet(info.start_size,packets_loss);
    info.packet_loss = packets_loss;
    info.percent_packet_loss = percent_packets_loss;

    add_info(logfile, info);                            // Сохранияем результаты опреций в лог файл

    lzh.~LZH();
    printf("FINISH LZH \n");
}

void lzw_test(char *file, char *encode_file, char *decode_file, char *logfile) {
    operation_info info;                                // Создадим структуру
    info.algo_name = "LZW";                             // Добавим название алгоритма
    info.start_size = GetFileSize(file);                // Запишем в структуру начальный размер файла
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ LZW
    printf("START LZW \n");
    LZW lzw;

    FILE *infile;
    BFILE *outfile;

    infile = fopen(file, "rb");
    if(infile == nullptr){printf("File does not exist!");}
    outfile = lzw.OpenOutputBFile(encode_file);

    printf("Encode: \n");
    start_clock();                 // начальное время
    lzw.CompressFile(infile,outfile);
    info.time_encode = stop_clock();                    // Запишем в структуру время архивации

    fclose(infile);
    lzw.CloseOutputBFile(outfile);
    info.final_size = GetFileSize(encode_file);        // Запишем в структуру финальный размер файла

    BFILE *infile_1;
    FILE *outfile_1;

    infile_1 = lzw.OpenInputBFile(encode_file);
    if(infile_1 == nullptr){printf("File does not exist!");}
    outfile_1 =  fopen(decode_file, "wb");

    printf("Decode: \n");
    start_clock();                 // начальное время
    lzw.ExpandFile(infile_1,outfile_1);
    info.time_decode = stop_clock();                    // Запишем в структуру время деархивации

    lzw.CloseOutputBFile(infile_1);
    fclose(outfile_1);

    info.percent_compression = calculate_percent_compression(info.start_size, info.final_size);   // Запишем в структуру процент "полезности"

    int packets_loss = compare_files(file,decode_file);
    float percent_packets_loss = calculate_percent_loss_packet(info.start_size,packets_loss);
    info.packet_loss = packets_loss;
    info.percent_packet_loss = percent_packets_loss;

    add_info(logfile, info);                            // Сохранияем результаты опреций в лог файл

    lzw.~LZW();
    printf("FINISH LZW \n");
}

void lzss_test(char *file, char *encode_file, char *decode_file, char *logfile) {
    operation_info info;                                // Создадим структуру
    info.algo_name = "LZSS";                            // Добавим название алгоритма
    info.start_size = GetFileSize(file);                // Запишем в структуру начальный размер файла
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ LZSS
    printf("START LZSS \n");

    LZSS lzss;
    lzss.infile = fopen(file, "rb");
    if(lzss.infile == nullptr){printf("File does not exist!");}
    lzss.outfile =  fopen(encode_file, "wb");

    printf("Encode: \n");
    start_clock();                 // начальное время
    lzss.lzss_encode();
    info.time_encode = stop_clock();                    // Запишем в структуру время архивации

    fclose(lzss.infile);
    fclose(lzss.outfile);
    info.final_size = GetFileSize(encode_file);        // Запишем в структуру финальный размер файла

    lzss.infile = fopen(encode_file, "rb");
    if(lzss.infile == nullptr){printf("File does not exist!");}
    lzss.outfile =  fopen(decode_file, "wb");

    printf("Decode: \n");
    start_clock();                // начальное время
    lzss.lzss_decode();
    info.time_decode = stop_clock();                    // Запишем в структуру время деархивации

    fclose(lzss.infile);
    fclose(lzss.outfile);

    info.percent_compression = calculate_percent_compression(info.start_size, info.final_size);   // Запишем в структуру процент "полезности"

    int packets_loss = compare_files(file,decode_file);
    float percent_packets_loss = calculate_percent_loss_packet(info.start_size,packets_loss);
    info.packet_loss = packets_loss;
    info.percent_packet_loss = percent_packets_loss;

    add_info(logfile, info);                            // Сохранияем результаты опреций в лог файл

    lzss.~LZSS();
    printf("FINISH LZSS \n");
}

void lz77_test(char *file, char *encode_file, char *decode_file, char *logfile) {
    operation_info info;                                // Создадим структуру
    info.algo_name = "LZ77";                            // Добавим название алгоритма
    info.start_size = GetFileSize(file);                // Запишем в структуру начальный размер файла
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ LZ77
    printf("START LZ77 \n");

    LZ77 lz77;
    lz77.in_file = open(file, _O_BINARY | _O_RDWR, _S_IREAD | _S_IWRITE);
    lz77.out_file = open(encode_file, _O_BINARY | _O_WRONLY | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE);

    printf("Encode: \n");
    start_clock();                 // начальное время
    lz77.assign_write(lz77.out_file);
    lz77.encode();
    info.time_encode = stop_clock();                    // Запишем в структуру время архивации

    close(lz77.in_file);
    close(lz77.out_file);
    info.final_size = GetFileSize(encode_file);        // Запишем в структуру финальный размер файла

    lz77.dict_pos = 0;

    lz77.in_file = open(encode_file, _O_BINARY | _O_RDWR, _S_IREAD | _S_IWRITE);
    lz77.out_file = open(decode_file, _O_BINARY | _O_WRONLY | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE);

    printf("Decode: \n");
    start_clock();                // начальное время
    lz77.assign_read(lz77.in_file);
    lz77.decode();
    info.time_decode = stop_clock();                    // Запишем в структуру время деархивации

    close(lz77.in_file);
    close(lz77.out_file);

    info.percent_compression = calculate_percent_compression(info.start_size, info.final_size);   // Запишем в структуру процент "полезности"

    int packets_loss = compare_files(file,decode_file);
    float percent_packets_loss = calculate_percent_loss_packet(info.start_size,packets_loss);
    info.packet_loss = packets_loss;
    info.percent_packet_loss = percent_packets_loss;

    add_info(logfile, info);                            // Сохранияем результаты опреций в лог файл

    lz77.~LZ77();
    printf("FINISH LZ77 \n");
}

void rle_test(char *file, char *encode_file, char *decode_file, char *logfile) {
    operation_info info;                                // Создадим структуру
    info.algo_name = "RLE";                             // Добавим название алгоритма
    info.start_size = GetFileSize(file);                // Запишем в структуру начальный размер файла
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ RLE
    printf("START RLE \n");

    RLE rle;
    rle.source_file = fopen(file, "rb");
    if(rle.source_file == nullptr) {
        printf("File does not exist!");
    }
    rle.dest_file =  fopen(encode_file, "wb");

    printf("Encode: \n");
    start_clock();              // начальное время
    rle.rle1encoding();
    info.time_encode = stop_clock();                    // Запишем в структуру время архивации

    fclose(rle.source_file);
    fclose(rle.dest_file);
    info.final_size = GetFileSize(encode_file);        // Запишем в структуру финальный размер файла

    rle.byte_stored_status = FALSE;

    rle.source_file = fopen(encode_file, "rb");
    if(rle.source_file == nullptr) {
        printf("File does not exist!");
    }
    rle.dest_file =  fopen(decode_file, "wb");

    printf("Decode: \n");
    start_clock();                // начальное время
    rle.rle1decoding();
    info.time_decode = stop_clock();                    // Запишем в структуру время деархивации

    fclose(rle.source_file);
    fclose(rle.dest_file);

    info.percent_compression = calculate_percent_compression(info.start_size, info.final_size);   // Запишем в структуру процент "полезности"

    int packets_loss = compare_files(file,decode_file);
    float percent_packets_loss = calculate_percent_loss_packet(info.start_size,packets_loss);
    info.packet_loss = packets_loss;
    info.percent_packet_loss = percent_packets_loss;

    add_info(logfile, info);                            // Сохранияем результаты опреций в лог файл

    rle.~RLE();
    printf("FINISH RLE \n");
}

void huffman_test(char *file, char *encode_file, char *decode_file, char *logfile) {
    operation_info info;                                // Создадим структуру
    info.algo_name = "HUFFMAN";                         // Добавим название алгоритма
    info.start_size = GetFileSize(file);                // Запишем в структуру начальный размер файла
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ HUFFMAN
    printf("START HUFFMAN \n");

    HUFFMAN huffman;
    huffman.ifile = fopen(file, "rb");
    if(huffman.ifile == nullptr) {
        printf("File does not exist!");
    }
    huffman.ofile =  fopen(encode_file, "wb");

    printf("Encode: \n");
    start_clock();                // начальное время
    huffman.Encode();
    info.time_encode = stop_clock();                    // Запишем в структуру время архивации

    fclose(huffman.ifile);
    fclose(huffman.ofile);
    info.final_size = GetFileSize(encode_file);        // Запишем в структуру финальный размер файла

    huffman.ifile = fopen(encode_file, "rb");
    if(huffman.ifile == nullptr) {
        printf("File does not exist!");
    }
    huffman.ofile =  fopen(decode_file, "wb");

    printf("Decode: \n");
    start_clock();                // начальное время
    huffman.Decode();
    info.time_decode = stop_clock();                    // Запишем в структуру время деархивации

    fclose(huffman.ifile);
    fclose(huffman.ofile);

    info.percent_compression = calculate_percent_compression(info.start_size, info.final_size);   // Запишем в структуру процент "полезности"

    int packets_loss = compare_files(file,decode_file);
    float percent_packets_loss = calculate_percent_loss_packet(info.start_size,packets_loss);
    info.packet_loss = packets_loss;
    info.percent_packet_loss = percent_packets_loss;

    add_info(logfile, info);                            // Сохранияем результаты опреций в лог файл

    huffman.~HUFFMAN();
    printf("FINISH HUFFMAN \n");
}

void huffman_addaptive_test(char *file, char *encode_file, char *decode_file, char *logfile) {
    operation_info info;                                // Создадим структуру
    info.algo_name = "ADAPTIVE_HUFFMAN";                // Добавим название алгоритма
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
    outfile = huffman_adaptive.OpenOutputCompressedFile(encode_file);

    printf("Encode: \n");
    start_clock();                // начальное время
    huffman_adaptive.CompressFile(infile, outfile);
    info.time_encode = stop_clock();                    // Запишем в структуру время архивации

    fclose(infile);
    huffman_adaptive.CloseOutputCompressedFile(outfile);
    info.final_size = GetFileSize(encode_file);        // Запишем в структуру финальный размер файла

    COMPRESSED_FILE *infile_1;
    FILE *outfile_1;
    infile_1 = huffman_adaptive.OpenInputCompressedFile(encode_file);
    outfile_1 = fopen(decode_file, "wb");

    printf("Decode: \n");
    start_clock();                // начальное время
    huffman_adaptive.ExpandFile(infile_1, outfile_1);
    info.time_decode = stop_clock();                    // Запишем в структуру время деархивации

    huffman_adaptive.CloseInputCompressedFile(infile_1);
    fclose(outfile_1);

    info.percent_compression = calculate_percent_compression(info.start_size, info.final_size);   // Запишем в структуру процент "полезности"

    int packets_loss = compare_files(file,decode_file);
    float percent_packets_loss = calculate_percent_loss_packet(info.start_size,packets_loss);
    info.packet_loss = packets_loss;
    info.percent_packet_loss = percent_packets_loss;

    add_info(logfile, info);                            // Сохранияем результаты опреций в лог файл

    huffman_adaptive.~HUFFMAN_ADAPTIVE();
    printf("FINISH HUFFMAN_ADAPTIVE \n");
}

void arithmetic_addaptive_test(char *file, char *encode_file, char *decode_file, char *logfile) {
    operation_info info;                                // Создадим структуру
    info.algo_name = "ARITHMETIC_ADAPTIVE";             // Добавим название алгоритма
    info.start_size = GetFileSize(file);                // Запишем в структуру начальный размер файла
    //РЕАЛИЗУЕМ ГРУППУ МЕТОДОВ С АЛГОРИТМОМ ARITHMETIC_ADAPTIVE
    printf("START ARITHMETIC_ADAPTIVE \n");

    ARITHMETIC_ADAPTIVE arithmetic_adaptive;

    printf("Encode: \n");
    start_clock();                // начальное время
    arithmetic_adaptive.encode(file, encode_file);
    info.time_encode = stop_clock();                    // Запишем в структуру время архивации
    info.final_size = GetFileSize(encode_file);        // Запишем в структуру финальный размер файла

    printf("Decode: \n");
    start_clock();                // начальное время
    arithmetic_adaptive.decode(encode_file, decode_file);
    info.time_decode = stop_clock();                    // Запишем в структуру время деархивации

    info.percent_compression = calculate_percent_compression(info.start_size, info.final_size);   // Запишем в структуру процент "полезности"

    int packets_loss = compare_files(file,decode_file);
    float percent_packets_loss = calculate_percent_loss_packet(info.start_size,packets_loss);
    info.packet_loss = packets_loss;
    info.percent_packet_loss = percent_packets_loss;

    add_info(logfile, info);                            // Сохранияем результаты опреций в лог файл

    arithmetic_adaptive.~ARITHMETIC_ADAPTIVE();
    printf("FINISH ARITHMETIC_ADAPTIVE \n");
}

void auto_indexing_file(std::string *file,std::string type, int i)
{
    file->append(std::to_string(i));
    file->append(type);
}

void automating_tests(int number)
{
    std::string log_pattern = "Logs/log";
    std::string main_file = "Files/file";
    std::string type_csv = ".csv";
    std::string type_txt = ".adc"; //txt

    std::string lzari_test_file_encode = "Data/lzari_test_file_encode";
    std::string lzari_test_file_decode = "Data/lzari_test_file_decode";

    std::string lzw_test_file_encode = "Data/lzw_test_file_encode";
    std::string lzw_test_file_decode = "Data/lzw_test_file_decode";

    std::string lzss_test_file_encode = "Data/lzss_test_file_encode";
    std::string lzss_test_file_decode = "Data/lzss_test_file_decode";

    std::string lz77_test_file_encode = "Data/lz77_test_file_encode";
    std::string lz77_test_file_decode = "Data/lz77_test_file_decode";

    std::string rle_test_file_encode = "Data/rle_test_file_encode";
    std::string rle_test_file_decode = "Data/rle_test_file_decode";

    std::string arithmetic_addaptive_test_file_encode = "Data/arithmetic_addaptive_test_file_encode";
    std::string arithmetic_addaptive_test_file_decode = "Data/arithmetic_addaptive_test_file_decode";

    std::string lzh_test_file_encode = "Data/lzh_test_file_encode";
    std::string lzh_test_file_decode = "Data/lzh_test_file_decode";

    std::string huffman_test_file_encode = "Data/huffman_test_file_encode";
    std::string huffman_test_file_decode = "Data/huffman_test_file_decode";

    std::string huffman_addaptive_test_file_encode = "Data/huffman_addaptive_test_file_encode";
    std::string huffman_addaptive_test_file_decode = "Data/huffman_addaptive_test_file_decode";

    for(int i = 1; i <= number; i++)
    {
        // Создадим индексированное название для файла-лога: log<index>.csv
        log_pattern.append(std::to_string(i));
        log_pattern.append(type_csv);
        char log_path[log_pattern.size() + 1];
        log_pattern.copy(log_path, log_pattern.size() + 1);
        log_path[log_pattern.size()] = '\0';

        // Создадим индексированное название для исходного файла: file<index>.txt
        main_file.append(std::to_string(i));
        main_file.append(type_txt);
        char main_file_path[main_file.size() + 1];
        main_file.copy(main_file_path, main_file.size() + 1);
        main_file_path[main_file.size()] = '\0';

        // Создадим ЛОГ-файл для прохождения алгоритмов
        create_log(log_path);

        // Пройдемся всеми алгоритмами по указанному файлу и соберем сатистику

        // БЛОК_ОПЕРАЦИЙ_ДЛЯ--------------------------------------//LZARI (сейчас не может работать в режиме конвеера)
        auto_indexing_file(&lzari_test_file_encode, type_txt, i);
        auto_indexing_file(&lzari_test_file_decode, type_txt, i);
        char lzari_file_path_encode[lzari_test_file_encode.size() + 1];lzari_test_file_encode.copy(lzari_file_path_encode, lzari_test_file_encode.size() + 1);lzari_file_path_encode[lzari_test_file_encode.size()] = '\0';
        char lzari_file_path_decode[lzari_test_file_decode.size() + 1];lzari_test_file_decode.copy(lzari_file_path_decode, lzari_test_file_decode.size() + 1);lzari_file_path_decode[lzari_test_file_decode.size()] = '\0';

        lzari_test(main_file_path,lzari_file_path_encode,lzari_file_path_decode,log_path);
//        clear(lzari_file_path_encode, lzari_file_path_decode);
        //=============================================================

        // БЛОК_ОПЕРАЦИЙ_ДЛЯ--------------------------------------//LZW
        auto_indexing_file(&lzw_test_file_encode, type_txt, i);
        auto_indexing_file(&lzw_test_file_decode, type_txt, i);
        char lzw_file_path_encode[lzw_test_file_encode.size() + 1];lzw_test_file_encode.copy(lzw_file_path_encode, lzw_test_file_encode.size() + 1);lzw_file_path_encode[lzw_test_file_encode.size()] = '\0';
        char lzw_file_path_decode[lzw_test_file_decode.size() + 1];lzw_test_file_decode.copy(lzw_file_path_decode, lzw_test_file_decode.size() + 1);lzw_file_path_decode[lzw_test_file_decode.size()] = '\0';

        lzw_test(main_file_path,lzw_file_path_encode,lzw_file_path_decode, log_path);
//        clear(lzw_file_path_encode,lzw_file_path_decode);
        //=============================================================

        // БЛОК_ОПЕРАЦИЙ_ДЛЯ--------------------------------------//LZSS
        auto_indexing_file(&lzss_test_file_encode, type_txt, i);
        auto_indexing_file(&lzss_test_file_decode, type_txt, i);
        char lzss_file_path_encode[lzss_test_file_encode.size() + 1];lzss_test_file_encode.copy(lzss_file_path_encode, lzss_test_file_encode.size() + 1);lzss_file_path_encode[lzss_test_file_encode.size()] = '\0';
        char lzss_file_path_decode[lzss_test_file_decode.size() + 1];lzss_test_file_decode.copy(lzss_file_path_decode, lzss_test_file_decode.size() + 1);lzss_file_path_decode[lzss_test_file_decode.size()] = '\0';

        lzss_test(main_file_path,lzss_file_path_encode,lzss_file_path_decode,log_path);
//        clear(lzss_file_path_encode, lzss_file_path_decode);
        //=============================================================

        // БЛОК_ОПЕРАЦИЙ_ДЛЯ--------------------------------------//LZ77
        auto_indexing_file(&lz77_test_file_encode, type_txt, i);
        auto_indexing_file(&lz77_test_file_decode, type_txt, i);
        char lz77_file_path_encode[lz77_test_file_encode.size() + 1];lz77_test_file_encode.copy(lz77_file_path_encode, lz77_test_file_encode.size() + 1);lz77_file_path_encode[lz77_test_file_encode.size()] = '\0';
        char lz77_file_path_decode[lz77_test_file_decode.size() + 1];lz77_test_file_decode.copy(lz77_file_path_decode, lz77_test_file_decode.size() + 1);lz77_file_path_decode[lz77_test_file_decode.size()] = '\0';

        lz77_test(main_file_path,lz77_file_path_encode,lz77_file_path_decode,log_path);
//        clear(lz77_file_path_encode, lz77_file_path_decode);
        //=============================================================

        // БЛОК_ОПЕРАЦИЙ_ДЛЯ--------------------------------------//RLE
        auto_indexing_file(&rle_test_file_encode, type_txt, i);
        auto_indexing_file(&rle_test_file_decode, type_txt, i);
        char rle_file_path_encode[rle_test_file_encode.size() + 1];rle_test_file_encode.copy(rle_file_path_encode, rle_test_file_encode.size() + 1);rle_file_path_encode[rle_test_file_encode.size()] = '\0';
        char rle_file_path_decode[rle_test_file_decode.size() + 1];rle_test_file_decode.copy(rle_file_path_decode, rle_test_file_decode.size() + 1);rle_file_path_decode[rle_test_file_decode.size()] = '\0';

        rle_test(main_file_path,rle_file_path_encode,rle_file_path_decode,log_path);
//        clear(rle_file_path_encode, rle_file_path_decode);
        //=============================================================

        // БЛОК_ОПЕРАЦИЙ_ДЛЯ--------------------------------------//ARITHMETIC_ADDAPTIVE
        auto_indexing_file(&arithmetic_addaptive_test_file_encode, type_txt, i);
        auto_indexing_file(&arithmetic_addaptive_test_file_decode, type_txt, i);
        char arith_addap_file_path_encode[arithmetic_addaptive_test_file_encode.size() + 1];arithmetic_addaptive_test_file_encode.copy(arith_addap_file_path_encode, arithmetic_addaptive_test_file_encode.size() + 1);arith_addap_file_path_encode[arithmetic_addaptive_test_file_encode.size()] = '\0';
        char arith_addap_file_path_decode[arithmetic_addaptive_test_file_decode.size() + 1];arithmetic_addaptive_test_file_decode.copy(arith_addap_file_path_decode, arithmetic_addaptive_test_file_decode.size() + 1);arith_addap_file_path_decode[arithmetic_addaptive_test_file_decode.size()] = '\0';

        arithmetic_addaptive_test(main_file_path,arith_addap_file_path_encode,arith_addap_file_path_decode,log_path);
//        clear(arith_addap_file_path_encode, arith_addap_file_path_decode);
        //=============================================================

        // БЛОК_ОПЕРАЦИЙ_ДЛЯ--------------------------------------//HUFFMAN_ADDAPTIVE
        auto_indexing_file(&huffman_addaptive_test_file_encode, type_txt, i);
        auto_indexing_file(&huffman_addaptive_test_file_decode, type_txt, i);
        char huff_addap_file_path_encode[huffman_addaptive_test_file_encode.size() + 1];huffman_addaptive_test_file_encode.copy(huff_addap_file_path_encode, huffman_addaptive_test_file_encode.size() + 1);huff_addap_file_path_encode[huffman_addaptive_test_file_encode.size()] = '\0';
        char huff_addap_file_path_decode[huffman_addaptive_test_file_decode.size() + 1];huffman_addaptive_test_file_decode.copy(huff_addap_file_path_decode, huffman_addaptive_test_file_decode.size() + 1);huff_addap_file_path_decode[huffman_addaptive_test_file_decode.size()] = '\0';

        huffman_addaptive_test(main_file_path,huff_addap_file_path_encode,huff_addap_file_path_decode,log_path);
//        clear(huff_addap_file_path_encode, huff_addap_file_path_decode);
        //=============================================================

        // БЛОК_ОПЕРАЦИЙ_ДЛЯ--------------------------------------//LZH (worked(with bugs))
        auto_indexing_file(&lzh_test_file_encode, type_txt, i);
        auto_indexing_file(&lzh_test_file_decode, type_txt, i);
        char lzh_file_path_encode[lzh_test_file_encode.size() + 1];lzh_test_file_encode.copy(lzh_file_path_encode, lzh_test_file_encode.size() + 1);lzh_file_path_encode[lzh_test_file_encode.size()] = '\0';
        char lzh_file_path_decode[lzh_test_file_decode.size() + 1];lzh_test_file_decode.copy(lzh_file_path_decode, lzh_test_file_decode.size() + 1);lzh_file_path_decode[lzh_test_file_decode.size()] = '\0';

        lzh_test(main_file_path,lzh_file_path_encode,lzh_file_path_decode,log_path);
//        clear(lzh_file_path_encode, lzh_file_path_decode);
        //=============================================================

        // БЛОК_ОПЕРАЦИЙ_ДЛЯ--------------------------------------//HUFFMAN (не справляется с большими файлами)
//        auto_indexing_file(&huffman_test_file_encode, type_txt, i);
//        auto_indexing_file(&huffman_test_file_decode, type_txt, i);
//        char huff_file_path_encode[huffman_test_file_encode.size() + 1];huffman_test_file_encode.copy(huff_file_path_encode, huffman_test_file_encode.size() + 1);huff_file_path_encode[huffman_test_file_encode.size()] = '\0';
//        char huff_file_path_decode[huffman_test_file_decode.size() + 1];huffman_test_file_decode.copy(huff_file_path_decode, huffman_test_file_decode.size() + 1);huff_file_path_decode[huffman_test_file_decode.size()] = '\0';

//        huffman_test(main_file_path,huff_file_path_encode,huff_file_path_decode,log_path);
        //clear(huff_file_path_encode, huff_file_path_decode);
        //=============================================================

//        {
//            clear(lzari_file_path_encode, lzari_file_path_decode);
//            clear(lzw_file_path_encode,lzw_file_path_decode);
//            clear(lzss_file_path_encode, lzss_file_path_decode);
//            clear(lz77_file_path_encode, lz77_file_path_decode);
//            clear(rle_file_path_encode, rle_file_path_decode);
//            clear(arith_addap_file_path_encode, arith_addap_file_path_decode);
//            clear(huff_addap_file_path_encode, huff_addap_file_path_decode);
//            clear(lzh_file_path_encode, lzh_file_path_decode);
//            //clear(huff_file_path_encode, huff_file_path_decode);
//        }

        log_pattern = "Logs/log";
        main_file = "Files/file";

        lzari_test_file_encode = "Data/lzari_test_file_encode";
        lzari_test_file_decode = "Data/lzari_test_file_decode";

        lzw_test_file_encode = "Data/lzw_test_file_encode";
        lzw_test_file_decode = "Data/lzw_test_file_decode";

        lzss_test_file_encode = "Data/lzss_test_file_encode";
        lzss_test_file_decode = "Data/lzss_test_file_decode";

        lz77_test_file_encode = "Data/lz77_test_file_encode";
        lz77_test_file_decode = "Data/lz77_test_file_decode";

        rle_test_file_encode = "Data/rle_test_file_encode";
        rle_test_file_decode = "Data/rle_test_file_decode";

        arithmetic_addaptive_test_file_encode = "Data/arithmetic_addaptive_test_file_encode";
        arithmetic_addaptive_test_file_decode = "Data/arithmetic_addaptive_test_file_decode";

        lzh_test_file_encode = "Data/lzh_test_file_encode";
        lzh_test_file_decode = "Data/lzh_test_file_decode";

        huffman_test_file_encode = "Data/huffman_test_file_encode";
        huffman_test_file_decode = "Data/huffman_test_file_decode";

        huffman_addaptive_test_file_encode = "Data/huffman_addaptive_test_file_encode";
        huffman_addaptive_test_file_decode = "Data/huffman_addaptive_test_file_decode";

    }
}

int main(int argc, char *argv[]) {
    printf("|---------------------ALL_TESTS_STARTED-------------------|");
    automating_tests(4); // ВНИМАНИЕ - ставить число тестов на 1 больше, чем файлов(мин. 2)
    printf("|---------------------ALL_TESTS_ENDED---------------------|");
    return EXIT_SUCCESS;
}
