/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/


#ifdef CAKE_HEADERS
    
    typedef long long fpos_t;
    typedef unsigned long size_t;
    
    #define EXIT_SUCCESS 0
    #define EXIT_FAILURE 1
    #define NULL ((void*)0)
    
    typedef int wchar_t;
    [[nodiscard]] double atof(const char* nptr);
    [[nodiscard]] int atoi(const char* nptr);
    [[nodiscard]] long int atol(const char* nptr);
    [[nodiscard]] long long int atoll(const char* nptr);
    double strtod(const char* restrict nptr, char** restrict endptr);
    float strtof(const char* restrict nptr, char** restrict endptr);
    long double strtold(const char* restrict nptr, char** restrict endptr);
    long int strtol(const char* restrict nptr, char** restrict endptr, int base);
    long long int strtoll(const char* restrict nptr, char** restrict endptr, int base);
    unsigned long int strtoul(const char* restrict nptr, char** restrict endptr, int base);
    unsigned long long int strtoull(const char* restrict nptr, char** restrict endptr, int base);
    int rand(void);
    void srand(unsigned int seed);
    void* aligned_alloc(size_t alignment, size_t size);
    
    
    [[nodiscard]] void* _Owner _Opt _Clear calloc(size_t nmemb, size_t size);
    void free(void* _Owner _Opt ptr);
    [[nodiscard]] void* _Owner _Opt _Uninitialized malloc(size_t size);
    [[nodiscard]] void* _Owner _Opt realloc(void* _Opt ptr, size_t size);    
    
    [[noreturn]] void abort(void);
    int atexit(void (*func)(void));
    int at_quick_exit(void (*func)(void));
    [[noreturn]] void exit(int status);
    [[noreturn]] void _Exit(int status);
    char* getenv(const char* name);
    [[noreturn]] void quick_exit(int status);
    int system(const char* string);
    

#else


    #ifdef _WIN64
        typedef unsigned __int64 size_t;    
    #elif defined _WIN32
        typedef unsigned int     size_t;
    #endif
    
    #ifdef __linux__
        typedef __SIZE_TYPE__ size_t;
    #endif
    
    #ifdef __APPLE__
        typedef __SIZE_TYPE__ size_t;
    #endif
    

    void* _Owner _Opt _Clear calloc(size_t nmemb, size_t size);
    void free(void* _Owner _Opt ptr);
    void* _Owner _Opt _Uninitialized malloc(size_t size);
    void* _Owner _Opt realloc(void* _Opt ptr, size_t size);


    long strtol(char const* _String, char** _Opt _EndPtr, int _Radix);
    long long strtoll(char const* _String, char** _Opt _EndPtr, int _Radix);
    double strtod(char const* _String, char** _Opt _EndPtr);
    long double strtold(char const* _String, char** _Opt _EndPtr);
    unsigned long long strtoull(char const* _String, char** _Opt _EndPtr, int _Radix);
    float strtof(char const* _String, char** _Opt _EndPtr);

    char* _Opt _fullpath(char* _Opt absPath, const char* relPath, size_t maxLength);

    /* ..then lets include the system headers ... */
    #include_next <stdlib.h>

#endif
