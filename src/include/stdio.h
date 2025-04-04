/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma once
#define _IOFBF 0x0000
#define _IOLBF 0x0040
#define _IONBF 0x0004

#define BUFSIZ  512

#define EOF    (-1)

#define FILENAME_MAX    260
#define FOPEN_MAX       20

#define L_tmpnam   260 // _MAX_PATH

/* Seek method constants */

#define SEEK_CUR    1
#define SEEK_END    2
#define SEEK_SET    0


#define TMP_MAX         2147483647



typedef long long fpos_t;
typedef int FILE;

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

typedef unsigned long size_t;
typedef void* va_list;
int remove(const char* filename);
int rename(const char* old, const char* news);
FILE* _Opt tmpfile(void);
char* tmpnam(char* s);
#if defined(__STDC_OWNERSHIP__) 
int fclose(FILE* _Owner stream);
#else
int fclose(FILE* stream);
#endif
int fflush(FILE* stream);
#if defined(__STDC_OWNERSHIP__) 
FILE* _Owner _Opt fopen(const char* restrict filename, const char* restrict mode);
FILE* _Owner _Opt freopen(const char* restrict filename, const char* restrict mode, FILE* restrict stream);
#else
FILE* fopen(const char* restrict filename, const char* restrict mode);
FILE* freopen(const char* restrict filename, const char* restrict mode, FILE* restrict stream);
#endif
void setbuf(FILE* restrict stream, char* restrict buf);
int setvbuf(FILE* restrict stream, char* restrict buf, int mode, size_t size);
int fprintf(FILE* restrict stream, const char* restrict format, ...);
int fscanf(FILE* restrict stream, const char* restrict format, ...);
int printf(const char* restrict format, ...);
int scanf(const char* restrict format, ...);
int snprintf(char* restrict s, size_t n, const char* restrict format, ...);
int sprintf(char* restrict s, const char* restrict format, ...);
int sscanf(const char* restrict s, const char* restrict format, ...);
int vfprintf(FILE* restrict stream, const char* restrict format, va_list arg);
int vfscanf(FILE* restrict stream, const char* restrict format, va_list arg);
int vprintf(const char* restrict format, va_list arg);
int vscanf(const char* restrict format, va_list arg);
int puts(const char* str);
int fputs(const char* restrict s, FILE* restrict stream);
int getc(FILE* stream);
int getchar(void);
int putc(int c, FILE* stream);
int putchar(int c);
int puts(const char* s);
int ungetc(int c, FILE* stream);
int fgetc(FILE* stream);
size_t fread(void* restrict ptr, size_t size, size_t nmemb, FILE* restrict stream);
size_t fwrite(const void* restrict ptr, size_t size, size_t nmemb, FILE* restrict stream);
int fgetpos(FILE* restrict stream, fpos_t* restrict pos);
int fseek(FILE* stream, long int offset, int whence);
int fsetpos(FILE* stream, const fpos_t* pos);
long int ftell(FILE* stream);
void rewind(FILE* stream);
void clearerr(FILE* stream);
int feof(FILE* stream);
int ferror(FILE* stream);
void perror(const char* s);



#ifndef NULL
#define NULL ((void*)0)
#endif
