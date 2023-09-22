#pragma once
typedef char* va_list;


/* Seek method constants */

#define SEEK_CUR    1
#define SEEK_END    2
#define SEEK_SET    0

#define EOF    (-1)

#define FILENAME_MAX    260
#define FOPEN_MAX       20
//#define _SYS_OPEN       20

#define stdin  ((0))
#define stdout ((1))
#define stderr ((2))
#define BUFSIZ  512
#define TMP_MAX         2147483647
#define L_tmpnam   260 // _MAX_PATH
typedef struct _iobuf FILE;

#define _IOFBF 0x0000
#define _IOLBF 0x0040
#define _IONBF 0x0004

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef unsigned long size_t;

/*

size_t                  _IONBF                 SEEK_CUR                stdout
FILE                    BUFSIZ                 SEEK_END                _PRINTF_NAN_LEN_MAX
fpos_t                  EOF                    SEEK_SET
NULL                    FOPEN_MAX              TMP_MAX                 __STDC_VERSION_STDIO_H__
_IOFBF                  FILENAME_MAX           stderr
_IOLBF                  L_tmpnam               stdin
*/
typedef unsigned long long fpos_t;

typedef struct _iobuf FILE;

 int remove(const char *filename);
 int rename(const char *old, const char *new);
 FILE *tmpfile(void);
 char *tmpnam(char *s);
 int fclose(FILE *stream);
 int fflush(FILE *stream);
 FILE *fopen(const char * restrict filename, const char * restrict mode);
 FILE *freopen(const char * restrict filename, const char * restrict mode, FILE * restrict stream);
 void setbuf(FILE * restrict stream, char * restrict buf);
 int setvbuf(FILE * restrict stream, char * restrict buf, int mode, size_t size);
 int printf(const char * restrict format, ...);
 int scanf(const char * restrict format, ...);
 int snprintf(char * restrict s, size_t n, const char * restrict format, ...);
 int sprintf(char * restrict s, const char * restrict format, ...);
 int sscanf(const char * restrict s, const char * restrict format, ...);
 int vfprintf(FILE * restrict stream, const char * restrict format, va_list arg);
 int vfscanf(FILE * restrict stream, const char * restrict format, va_list arg);
 int vprintf(const char * restrict format, va_list arg);
 int vscanf(const char * restrict format, va_list arg);
 int vsnprintf(char * restrict s, size_t n, const char * restrict format, va_list arg);
 int vsprintf(char * restrict s, const char * restrict format, va_list arg);
 int vsscanf(const char * restrict s, const char * restrict format, va_list arg);
 int fgetc(FILE *stream);

 char *fgets(char * restrict s, int n, FILE * restrict stream);
 int fputc(int c, FILE *stream);
 int fputs(const char * restrict s, FILE * restrict stream);
 int getc(FILE *stream);
 int getchar(void);
 int putc(int c, FILE *stream);
 int putchar(int c);
 int puts(const char *s);
 int ungetc(int c, FILE *stream);
 size_t fread(void * restrict ptr, size_t size, size_t nmemb, FILE * restrict stream);
 size_t fwrite(const void * restrict ptr, size_t size, size_t nmemb,  FILE * restrict stream);
 int fgetpos(FILE * restrict stream, fpos_t * restrict pos);
 int fseek(FILE *stream, long int offset, int whence);
 int fsetpos(FILE *stream, const fpos_t *pos);
 long int ftell(FILE *stream);
 void rewind(FILE *stream);
 void clearerr(FILE *stream);
 int feof(FILE *stream);
 int ferror(FILE *stream);
 void perror(const char *s);
 int fprintf(FILE * restrict stream, const char * restrict format, ...);
 int fscanf(FILE * restrict stream, const char * restrict format, ...);
