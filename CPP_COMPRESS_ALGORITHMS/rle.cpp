#include "rle.h"

RLE::RLE(){}
RLE::~RLE(){}

void RLE::rle1encoding() {
    register unsigned char byte1, byte2, frame_size, array[129];
    if (!end_of_data()) {
        byte1=read_byte();
        frame_size=1;
        if (!end_of_data()) {
            byte2=read_byte();
            frame_size=2;
            do {
                if (byte1==byte2) {
                    while ((!end_of_data())&&(byte1==byte2) &&(frame_size<129)) {
                        byte2=read_byte();
                        frame_size++;
                    }
                    if (byte1==byte2) { /* встретили последовательность одинаковых байт */
                        write_byte(126+frame_size);
                        write_byte(byte1);
                        if (!end_of_data()) {
                            byte1=read_byte();
                            frame_size=1;
                        } else {
                            frame_size=0;
                        }
                    } else {
                        write_byte(125+frame_size);
                        write_byte(byte1);
                        byte1=byte2;
                        frame_size=1;
                    }
                    if (!end_of_data()) {
                        byte2=read_byte();
                        frame_size=2;
                    }
                } else { /* подготовка массива для сравнений, в нем будут храниться все идентичные байты */
                    *array = byte1;
                    array[1]=byte2;
                    while ((!end_of_data())&&(array[frame_size-2]!= array[frame_size-1])&&(frame_size<128)) {
                        array[frame_size]=read_byte();
                        frame_size++;
                    }
                    if (array[frame_size-2]==array[frame_size-1]) { /* встретилась ли последовательность из разных байт, следующих за одинаковыми? */ /* Да, тогда не считаем 2 последних байта */
                        write_byte(frame_size-3);
                        write_array(array,frame_size-2);
                        byte1=array[frame_size-2];
                        byte2=byte1;
                        frame_size=2;
                    } else {
                        write_byte(frame_size-1);
                        write_array(array,frame_size);
                        if (end_of_data()) {
                            frame_size=0;
                        } else {
                            byte1=read_byte();
                            if (end_of_data()) {
                                frame_size=1;
                            } else {
                                byte2=read_byte();
                                frame_size=2;
                            }
                        }
                    }
                }
            } while ((!end_of_data())||(frame_size>=2));
        }
        if (frame_size==1) {
            write_byte(0);
            write_byte(byte1);
        }
    }
}

//---------------------------------------------------------
// Декомпрессия методом RLE
void RLE::rle1decoding() {
    unsigned char header;
    register unsigned char i;
    while (!end_of_data()) {
        header=read_byte();
        switch (header & 128) {
            case 0:
                if (!end_of_data()) {
                    for (i=0;i<=header;i++) {
                        write_byte(read_byte());
                    }
                }
                /* else INVALID FILE */
                break;
            case 128:
                if (!end_of_data()) {
                    write_block(read_byte(),(header & 127)+2);
                }
                /* else INVALID FILE */
        }
    }
}

//---------------------------------------------------------
// Вывод помощи о пользовании программой
void RLE::help() {
    printf("RLE_01 e(encoding)|d(decoding) source target\n");
}
