//#include "main.h"
#include "lzw.h"

LZW::LZW(){}
LZW::~LZW(){}

void LZW::fatal_error( char *str )
{
   printf( "Fatal error: %s\n", str );
   exit(1);
}

/*-----------------------------------------------------------
   Открытие файла для побитовой записи
*/
BFILE *LZW::OpenOutputBFile ( char *name )
{
   BFILE *bfile;

   bfile = (BFILE *) calloc( 1, sizeof( BFILE ) );
   bfile->file = fopen( name, "wb" );
   bfile->rack = 0;
   bfile->mask = 0x80;
   bfile->pacifier_counter = 0;
   return bfile;
}

/*-----------------------------------------------------------
   Открытие файла для побитового чтения
*/

BFILE *LZW::OpenInputBFile( char *name )
{
   BFILE *bfile;

   bfile = (BFILE *) calloc( 1, sizeof( BFILE ) );
   bfile->file = fopen( name, "rb" );
   bfile->rack = 0;
   bfile->mask = 0x80;
   bfile->pacifier_counter = 0;
   return bfile;
}

/*-----------------------------------------------------------
   Закрытие файла для побитовой записи
*/

void LZW::CloseOutputBFile ( BFILE *bfile )
{
   if ( bfile->mask != 0x80 )
      putc( bfile->rack, bfile->file );
   fclose ( bfile->file );
   free ( (char *) bfile );
}

/*-----------------------------------------------------------
   Закрытие файла для побитового чтения
*/

void LZW::CloseInputBFile ( BFILE *bfile )
{
    fclose ( bfile->file );
    free ( (char *) bfile );
}

/*-----------------------------------------------------------
   Вывод одного бита
*/

void LZW::WriteBit ( BFILE *bfile, int bit )
{
   if ( bit )
      bfile->rack |= bfile->mask;
   bfile->mask >>= 1;
   if ( bfile->mask == 0 )
   {
      putc( bfile->rack, bfile->file );
      if ( ( bfile->pacifier_counter++ & PACIFIER_COUNT ) == 0 )
         putc( '.', stdout );
      bfile->rack = 0;
      bfile->mask = 0x80;
   }
}

/*-----------------------------------------------------------
   Вывод нескольких битов
*/

void LZW::WriteBits( BFILE *bfile, unsigned long code, int count )
{
   unsigned long mask;

   mask = 1L << ( count - 1 );
   while ( mask != 0)
   {
      if ( mask & code )
  bfile->rack |= bfile->mask;
      bfile->mask >>= 1;
      if ( bfile->mask == 0 )
      {
  putc( bfile->rack, bfile->file );
  if ( ( bfile->pacifier_counter++ & PACIFIER_COUNT ) == 0 )
            putc( '.', stdout );
  bfile->rack = 0;
  bfile->mask = 0x80;
      }
      mask >>= 1;
   }
}

/*-----------------------------------------------------------
   Ввод одного бита
*/

int LZW::ReadBit( BFILE *bfile )
{
   int value;

   if ( bfile->mask == 0x80 )
   {
      bfile->rack = getc( bfile->file );
      if ( bfile->rack == EOF )
         fatal_error( "Error in function ReadBit!\n" );
      if ( ( bfile->pacifier_counter++ & PACIFIER_COUNT ) == 0 )
         putc( '.', stdout );
   }

   value = bfile->rack & bfile->mask;
   bfile->mask >>= 1;
   if ( bfile->mask == 0 )
      bfile->mask = 0x80;
   return ( value ? 1 : 0 );
}

/*-----------------------------------------------------------
   Ввод нескольких битов
*/

unsigned long LZW::ReadBits ( BFILE *bfile, int bit_count )
{
   unsigned long mask;
   unsigned long return_value;

   mask = 1L << ( bit_count - 1 );
   return_value = 0;
   while ( mask != 0 )
   {
      if ( bfile->mask == 0x80 )
      {
  bfile->rack = getc( bfile->file );
  if ( bfile->rack == EOF )
            fatal_error( "Error in function ReadBits!\n" );
  if ( ( bfile->pacifier_counter++ & PACIFIER_COUNT ) == 0 )
            putc( '.', stdout );
      }
      if ( bfile->rack & bfile->mask )
         return_value |= mask;
      mask >>= 1;
      bfile->mask >>= 1;
      if ( bfile->mask == 0 )
  bfile->mask = 0x80;
   }

   return return_value;
}

/*-----------------------------------------------------------
   Вывод сообщения об использовании
*/

void LZW::usage_exit ()
{
 printf ("e - for encoding (compression)\n");
 printf ("d - for decoding (decompression)\n");
 exit( 0 );
}

/*-----------------------------------------------------------
   Измерение размера файла
*/

long LZW::file_size ( char *name )
{
   long eof_ftell;
   FILE *file;

   file = fopen( name, "r" );
   if ( file == NULL )
      return( 0L );
   fseek( file, 0L, SEEK_END );
   eof_ftell = ftell( file );
   fclose( file );
   return eof_ftell;
}

/*-----------------------------------------------------------
   Вывод сообщения о соотношении размеров файлов
*/
void LZW::print_ratios( char *input, char *output )
{
   long input_size;
   long output_size;
   int ratio;

   input_size = file_size( input );
   if ( input_size == 0 )
      input_size = 1;
   output_size = file_size( output );
   ratio = 100 - (int) ( output_size * 100L / input_size );
   printf( "\nInput size:        %ld bytes\n", input_size );
   printf( "Output size:       %ld bytes\n", output_size );
   if ( output_size == 0 )
      output_size = 1;
   printf( "Ratio: %d%%\n", ratio );
}

/*-----------------------------------------------------------
   Далее начинается исходный текст собственно алгоритма LZW
*/

/* Структура словаря для алгоритма LZW */

struct dictionary
{
   int code_value;
   int prefix_code;
   char character;
}
dict[TABLE_SIZE];

/* Стек для декодирования */

char decode_stack[TABLE_SIZE];

/*-----------------------------------------------------------
   Процедура сжатия файла
*/

void LZW::CompressFile ( FILE *input, BFILE *output )
{
   int next_code, character, string_code;
   unsigned int index, i;

   /* Инициализация */
   next_code = FIRST_CODE;
   for ( i = 0 ; i < TABLE_SIZE ; i++ )
       dict[i].code_value = UNUSED;
   /* Считать первый символ */
   if ( ( string_code = getc( input ) ) == EOF )
       string_code = END_OF_STREAM;
   /* Пока не конец сообщения */
   while ( ( character = getc( input ) ) != EOF )
   {
      /* Попытка найти в словаре пару <фраза, символ> */
      index = find_dictionary_match ( string_code, character );
      /* Соответствие найдено */
      if ( dict[index].code_value != -1 )
         string_code = dict[index].code_value;
      /* Такой пары в словаре нет */
      else
      {
         /* Добавление в словарь */
  if ( next_code <= MAX_CODE )
         {
            dict[index].code_value = next_code++;
            dict[index].prefix_code = string_code;
            dict[index].character = (char) character;
         }
         /* Выдача кода */
         WriteBits( output, (unsigned long) string_code, BITS );
         string_code = character;
      }
   }
   /* Завершение кодирования */
   WriteBits( output, (unsigned long) string_code, BITS );
   WriteBits( output, (unsigned long) END_OF_STREAM, BITS );
}

/*-----------------------------------------------------------
   Процедура декодирования сжатого файла
*/

void LZW::ExpandFile ( BFILE *input, FILE *output )
{
   unsigned int next_code, new_code, old_code;
   int character;
   unsigned int count;

   next_code = FIRST_CODE;
   old_code = (unsigned int) ReadBits( input, BITS );
   if ( old_code == END_OF_STREAM )
      return;
   character = old_code;

   putc ( old_code, output );

   while ( ( new_code = (unsigned int) ReadBits( input, BITS ) )
             != END_OF_STREAM )
   {
      /* Обработка возможной исключительной ситуации */
      if ( new_code >= next_code )
      {
         decode_stack[ 0 ] = (char) character;
         count = decode_string( 1, old_code );
      }
      else
         count = decode_string( 0, new_code );

      character = decode_stack[ count - 1 ];
      /* Выдача раскодированной строки */
      while ( count > 0 )
         putc( decode_stack[--count], output );
      /* Обновление словаря */
      if ( next_code <= MAX_CODE )
      {
         dict[next_code].prefix_code = old_code;
         dict[next_code].character = (char) character;
         next_code++;
      }
      old_code = new_code;
   }
}

/*-----------------------------------------------------------
   Процедура поиска в словаре указанной пары <код фразы,
   символ>. Для ускорения поиска используется хеш, получаемый
   из параметров.
*/

unsigned int LZW::find_dictionary_match ( int prefix_code, int character )
{
   int index;
   int offset;

   /* Собственно получение значения хеш-функции */
   index = ( character << ( BITS - 8 ) ) ^ prefix_code;
   /* Разрешение возможных коллизий */
   if ( index == 0 )
      offset = 1;
   else
      offset = TABLE_SIZE - index;
   for ( ; ; )
   {
      /* Эта ячейка словаря не использована */
      if ( dict[index].code_value == UNUSED )
         return index;
      /* Найдено соответствие */
      if ( dict[index].prefix_code == prefix_code &&
           dict[index].character == (char) character )
         return index;
      /* Коллизия. Подготовка к следующей попытке ее
         разрешения */
      index -= offset;
      if ( index < 0 )
         index += TABLE_SIZE;
   }
}

/*-----------------------------------------------------------
   Процедура декодирования строки. Размещает символы в стеке,
   возвращает их количество.
*/

unsigned int LZW::decode_string ( unsigned int count, unsigned int code )
{
   while ( code > 255 ) /* Пока не встретится код символа */
   {
      decode_stack[count++] = dict[code].character;
      code = dict[code].prefix_code;
   }
   decode_stack[count++] = (char) code;
   return count;
}
