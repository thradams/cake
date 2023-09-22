#pragma once

#ifdef _WIN64
    typedef unsigned long long size_t;
    typedef long long          ptrdiff_t;
    typedef long long          intptr_t;
#else
    typedef unsigned int     size_t;
    typedef int              ptrdiff_t;
    typedef int              intptr_t;
#endif


 void *memcpy(void * restrict s1, const void * restrict s2, size_t n);
 void *memccpy(void * restrict s1, const void * restrict s2, int c, size_t n);
 void *memmove(void *s1, const void *s2, size_t n);
 char *strcpy(char * restrict s1, const char * restrict s2);
 char *strncpy(char * restrict s1, const char * restrict s2, size_t n);
 char *strdup(const char *s);
 char *strndup(const char *s, size_t size);
 char *strcat(char * restrict s1, const char * restrict s2);
 char *strncat(char * restrict s1, const char * restrict s2, size_t n);
 int memcmp(const void *s1, const void *s2, size_t n);
 int strcmp(const char *s1, const char *s2);
 int strcoll(const char *s1, const char *s2);
 int strncmp(const char *s1, const char *s2, size_t n);
 size_t strxfrm(char * restrict s1, const char * restrict s2, size_t n);
 //QVoid *memchr(QVoid *s, int c, size_t n);
 //QChar *strchr(QChar *s, int c);
 size_t strcspn(const char *s1, const char *s2);
 //QChar *strpbrk(QChar *s1, const char *s2);
 //QChar *strrchr(QChar *s, int c);
 size_t strspn(const char *s1, const char *s2);
  //QChar *strstr(QChar *s1, const char *s2);
 char *strtok(char * restrict s1, const char * restrict s2);
 void *memset(void *s, int c, size_t n);
 void *memset_explicit(void *s, int c, size_t n);
 char *strerror(int errnum);
 size_t strlen(const char *s);

 //rrno_t                                        rsize_t

 typedef int errno_t;

 errno_t memcpy_s(void * restrict s1, rsize_t s1max, const void * restrict s2,  rsize_t n);
 errno_t memmove_s(void *s1, rsize_t s1max, const void *s2, rsize_t n);
 errno_t strcpy_s(char * restrict s1, rsize_t s1max, const char * restrict s2);
 errno_t strncpy_s(char * restrict s1, rsize_t s1max, const char * restrict s2,  rsize_t n);
 errno_t strcat_s(char * restrict s1, rsize_t s1max, const char * restrict s2);
 errno_t strncat_s(char * restrict s1, rsize_t s1max, const char * restrict s2, rsize_t n);
 char *strtok_s(char * restrict s1, rsize_t * restrict s1max, const char * restrict s2, char ** restrict ptr);
 errno_t memset_s(void *s, rsize_t smax, int c, rsize_t n)
 //errno_t strerror_s(char *s, rsize_t maxsize, errno_t errnum);
 //size_t strerrorlen_s(errno_t errnum);
 //size_t strnlen_s(const char *s, size_t maxsize);
