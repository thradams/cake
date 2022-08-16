#include "fs.h"



#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include "error.h"
#include <stdlib.h>

#ifdef _WIN32
#include <crtdbg.h>
#include <Windows.h>

#pragma comment (lib, "Rpcrt4.lib")

#else
#include <stdlib.h>
#include <unistd.h>
#include <uuid/uuid.h>
/*
caso nao tenha este arquivos apt-get install uuid-dev
*/
#endif

#include <assert.h>

#include <string.h>
#include <stdbool.h>
#include <errno.h>

#ifdef _WIN32


struct TAGDIR
{
    void* handle;
    struct dirent dirent;
};

//typedef struct TAGDIR DIR;


DIR* opendir(const char* name)
{
    assert(name != 0);
    WIN32_FIND_DATAA fdFile;

    char sPath[MAX_PATH] = { 0 };
    strcat(sPath, name);
    strcat(sPath, "\\*.*");

    HANDLE handle = FindFirstFileA(sPath, &fdFile);

    if (handle != INVALID_HANDLE_VALUE)
    {
        DIR* p = calloc(1, sizeof * p);
        if (p)
        {
            p->handle = handle;
            return p;
        }
        else
        {
            /*calloc sets errno to ENOMEM if a memory allocation fails */
            FindClose(handle);
        }
    }
    else
    {
        errno = windows_error_to_posix(GetLastError());
    }

    return NULL;
}

int closedir(DIR* dirp)
{
    FindClose(dirp->handle);
    free(dirp);
    return 0;
}

struct dirent* readdir(DIR* dirp)
{
    WIN32_FIND_DATAA fdFile;
    BOOL b = FindNextFileA(dirp->handle, &fdFile);
    if (b)
    {
        /*clear*/
        memset(&dirp->dirent, 0, sizeof(dirp->dirent));

        if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            dirp->dirent.d_type |= DT_DIR;
        }

        /*worst case trunks the string*/
        strncpy(dirp->dirent.d_name,
                fdFile.cFileName,
                sizeof(dirp->dirent.d_name) - 1);

        return &dirp->dirent;
    }
    else
    {
        errno = windows_error_to_posix(GetLastError());
    }
    return NULL;
}

#endif //_WINDOWS_

int copy_file(const char* pathfrom,
              const char* pathto)
{
    char buf[4096];
    size_t nread;
    int saved_errno;

    FILE* fd_from = fopen(pathfrom, "rb");
    if (fd_from < 0)
        return -1;

    FILE* fd_to = fopen(pathto, "wb");
    if (fd_to < 0)
        goto out_error;

    while (nread = fread(buf, sizeof(char), sizeof buf, fd_from), nread > 0) //lint !e668  (warning -- possibly passing null pointer to function 'fread(void *, size_t, size_t, FILE *)', arg. no. 4)
    {
        char* out_ptr = buf;
        size_t nwritten;

        do
        {
            nwritten = fwrite(out_ptr, sizeof(char), nread, fd_to);//lint !e668

            nread -= nwritten;
            out_ptr += nwritten;
        }
        while (nread > 0);
    }

    if (nread == 0)
    {
        if (fclose(fd_to) < 0)//lint !e668
        {
            fd_to = NULL;
            goto out_error;
        }
        fclose(fd_from);//lint !e668

        /* Success! */
        return 0;
    }

out_error:
    saved_errno = errno;

    fclose(fd_from); //lint !e668

    if (fd_to != NULL)
        fclose(fd_to);

    return saved_errno;
}

int copy_folder(const char* from, const char* to)
{
    int errcode = mkdir(to, 0700);
    if (errcode != 0)
    {
        return errcode;
    }

    DIR* dir = opendir(from);

    if (dir == NULL)
    {
        return errno;
    }

    struct dirent* dp;
    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
        {
            /* skip self and parent */
            continue;
        }

        char fromlocal[MAX_PATH];
        snprintf(fromlocal, MAX_PATH, "%s/%s", from, dp->d_name);

        char tolocal[MAX_PATH];
        snprintf(tolocal, MAX_PATH, "%s/%s", to, dp->d_name);

        if (dp->d_type & DT_DIR)
        {
            errcode = copy_folder(fromlocal, tolocal);
        }
        else
        {
            errcode = copy_file(fromlocal, tolocal);
        }

        if (errcode != 0)
            break;
    }

    closedir(dir);
    return errcode;
}

#ifdef _WIN32
int get_self_path(char* buffer, int maxsize)
{
    DWORD r = GetModuleFileNameA(NULL, buffer, maxsize);
    return r;
}

#else

int get_self_path(char* buffer, int maxsize)
{

    memset(buffer, 0, maxsize); // readlink does not null terminate!
    if (readlink("/proc/self/exe", buffer, maxsize) == -1)
    {
        //perror("readlink");
        return 1;
    }
    else
    {
        //printf("%s\n", dest);
    }
    return 0;

}
/*
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

int main()
{
  char dest[PATH_MAX];
  memset(dest,0,sizeof(dest)); // readlink does not null terminate!
  if (readlink("/proc/self/exe", dest, PATH_MAX) == -1) {
    perror("readlink");
  } else {
    printf("%s\n", dest);
  }
  return 0;
}
*/
#endif

#if !defined __EMSCRIPTEN__
char* dirname(char* path)
{
    int last = -1;
    for (int i = 0; path[i]; i++)
    {
        if (path[i] == '\\' || path[i] == '/')
            last = i;
    }

    if (last != -1)
    {
        path[last] = 0;
    }
    return path;
}
#endif

static bool fs_fread2(void* buffer, size_t size, size_t count, FILE* stream, size_t* sz)
{
    *sz = 0;//out

    bool result = false;
    size_t n = fread(buffer, size, count, stream);
    if (n == count)
    {
        *sz = n;
        result = true;
    }
    else if (n < count)
    {
        if (feof(stream))
        {
            *sz = n;
            result = true;
        }
    }
    return result;
}

char* readfile_core(const char* path)
{
    char* result = NULL;

    struct stat info;
    if (stat(path, &info) == 0)
    {
        char* data = (char*)malloc(sizeof(char) * info.st_size + 1);
        if (data != NULL)
        {
            FILE* file = fopen(path, "r");
            if (file != NULL)
            {
                if (info.st_size >= 3)
                {
                    size_t n = 0;
                    if (fs_fread2(data, 1, 3, file, &n))
                    {
                        if (n == 3)
                        {
                            if ((unsigned char) data[0] == (unsigned char)0xEF &&
                                    (unsigned char) data[1] == (unsigned char)0xBB &&
                                    (unsigned char) data[2] == (unsigned char)0xBF)
                            {
                                if (fs_fread2(data, 1, info.st_size - 3, file, &n))
                                {
                                    //ok
                                    data[n] = 0;
                                    result = data;
                                    data = 0;
                                }
                            }
                            else if (fs_fread2(data + 3, 1, info.st_size - 3, file, &n))
                            {
                                data[3 + n] = 0;
                                result = data;
                                data = 0;
                            }
                        }
                        else
                        {
                            data[n] = 0;
                            result = data;
                            data = 0;
                        }
                    }
                }
                else
                {
                    size_t n = 0;
                    if (fs_fread2(data, 1, info.st_size, file, &n))
                    {
                        data[n] = 0;
                        result = data;
                        data = 0;
                    }
                }
                fclose(file);
            }
            free(data);
        }
    }
#ifdef  DEBUG
    const char* p = result;
    while (*p)
    {
        assert(*p != '\r');
        p++;
    }
#endif //  DEBUG

    
    return result;
}
//#define MOCKFILES

#ifdef MOCKFILES
    extern const char* file_stdio_h;
    extern const char* file_stdlib_h;
#endif

char* readfile(const char* path)
{
#ifdef MOCKFILES
    if (strcmp(path, "stdio.h") == 0)
        return strdup(file_stdio_h);
    else if (strcmp(path, "stdlib.h") == 0)
        return strdup(file_stdlib_h);
#endif

    return readfile_core(path);
}


void NewCode(char dest[], int destsize)
{
    assert(destsize > 4);
    int range_min = 1000;
    int range_max = 9999;
    int u = range_min + rand() % (range_max - range_min);
    snprintf(dest, destsize, "%d", u);
}


#ifdef MOCKFILES
const char* file_stdio_h =
"typedef int FILE;\n"
"typedef int size_t;\n"
"typedef void* va_list;\n"
"int remove(const char *filename);\n"
"int rename(const char *old, const char *news);\n"
"FILE *tmpfile(void);\n"
"char *tmpnam(char *s);\n"
"int fclose(FILE *stream);\n"
"int fflush(FILE *stream);\n"
"FILE *fopen(const char * restrict filename,const char * restrict mode);\n"
"FILE *freopen(const char * restrict filename,const char * restrict mode,FILE * restrict stream);\n"
"void setbuf(FILE * restrict stream,char * restrict buf);\n"
"int setvbuf(FILE * restrict stream,char * restrict buf, int mode, size_t size);\n"
"int fprintf(FILE * restrict stream,const char * restrict format, ...);\n"
"int fscanf(FILE * restrict stream, const char * restrict format, ...);\n"
"int printf(const char * restrict format, ...);\n"
"int scanf(const char * restrict format, ...);\n"
"int snprintf(char * restrict s, size_t n, const char * restrict format, ...);\n"
"int sprintf(char * restrict s, const char * restrict format, ...);\n"
"int sscanf(const char * restrict s, const char * restrict format, ...);\n"
"int vfprintf(FILE * restrict stream,const char * restrict format, va_list arg);\n"
"int vfscanf(FILE * restrict stream,const char * restrict format, va_list arg);\n"
"int vprintf(const char * restrict format, va_list arg);\n"
"int vscanf(const char * restrict format, va_list arg);\n"
"\n";

const char* file_stdlib_h =
"\n"
"typedef int size_t;\n"
"typedef int wchar_t;\n"
"\n"
"\n"
"\n"
"double atof(const char *nptr);\n"
"int atoi(const char *nptr);\n"
"long int atol(const char *nptr);\n"
"long long int atoll(const char *nptr);\n"
"double strtod(const char * restrict nptr,char ** restrict endptr);\n"
"float strtof(const char * restrict nptr,char ** restrict endptr);\n"
"long double strtold(const char * restrict nptr,char ** restrict endptr);\n"
"long int strtol(const char * restrict nptr,char ** restrict endptr, int base);\n"
"long long int strtoll(const char * restrict nptr,char ** restrict endptr, int base);\n"
"unsigned long int strtoul(const char * restrict nptr,char ** restrict endptr, int base);\n"
"unsigned long long int strtoull(const char * restrict nptr,char ** restrict endptr, int base);\n"
"int rand(void);\n"
"void srand(unsigned int seed);\n"
"void *aligned_alloc(size_t alignment, size_t size);\n"
"void *calloc(size_t nmemb, size_t size);\n"
"void free(void *ptr);\n"
"void *malloc(size_t size);\n"
"void *realloc(void *ptr, size_t size);\n"
"_Noreturn void abort(void);\n"
"int atexit(void (*func)(void));\n"
"int at_quick_exit(void (*func)(void));\n"
"_Noreturn void exit(int status);\n"
"_Noreturn void _Exit(int status);\n"
"char *getenv(const char *name);\n"
"_Noreturn void quick_exit(int status);\n"
"int system(const char *string);\n"
"\n"
;

#endif
