#pragma once
#ifndef RLE_H
    #define RLE_H

    #include "main.h"

    /* Коды ошибок */
    #define NO_ERROR      0
    #define BAD_FILE_NAME 1
    #define BAD_ARGUMENT  2
    #define FALSE 0
    #define TRUE  1
    #define end_of_data() (byte_stored_status?FALSE:\
        !(byte_stored_status=\
        ((val_byte_stored=fgetc(source_file))!=EOF)))

    #define read_byte()  (byte_stored_status?\
        byte_stored_status=FALSE,(unsigned char)val_byte_stored:\
        (unsigned char)fgetc(source_file))

    #define write_byte(byte)  ((void)fputc((byte),dest_file))

    #define write_array(array,byte_nb_to_write)\
          ((void)fwrite((array),1,(byte_nb_to_write),dest_file))

    #define write_block(byte,time_nb)\
    {\
        unsigned char array_to_write[129];\
        (void)memset(array_to_write,(byte),(time_nb));\
        write_array(array_to_write,(time_nb));\
    }

    class RLE {
        public:
            RLE();
            ~RLE();
            FILE *source_file, *dest_file;
            int byte_stored_status=FALSE;
            void rle1decoding();
            void rle1encoding();
            void help();

        private:
            int val_byte_stored;
    };

#endif // RLE_H
