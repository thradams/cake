/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma safety enable

#include "ownership.h"
#include "fs.h"
#include <wchar.h>

#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include "error.h"
#include <stdlib.h>

#ifdef _WIN32
#include <Windows.h>
#endif


#if defined _MSC_VER && !defined __POCC__
#include <crtdbg.h>
#include <debugapi.h>
#endif


#ifdef __POCC__
#define stat _stat
#endif

#ifdef _WIN32
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

bool path_is_normalized(const char* path)
{
#ifdef _WINDOWS_
    for (const char* p = path; *p; p++)
    {
        int before = *p;
        int after = tolower(*p);

        if (before != after)
            return false;

        if (after == '\\')
        {
            return false;
        }
    }
    return true;
#else
    return true;
#endif
}

void path_normalize(char* path)
{
#ifdef _WINDOWS_
    for (char* p = path; *p; p++)
    {
        *p = (char)tolower(*p);
        if (*p == '\\')
        {
            *p = '/';
        }
    }
#else

#endif
}

bool path_is_absolute(const char* path)
{
#ifdef _WINDOWS_
    const char ch = (char)tolower(path[0]);
    if (ch >= 'a' && ch <= 'z')
    {
        /*  c:/ or c:\ */
        if (path[1] == ':' && (path[2] == '\\' || path[2] == '/'))
            return true;
    }

    if (path[0] == '\\' && path[1] == '\\')
    {
        // //server
        return true;
    }
#else
    return path[0] == '/';
#endif

    return false;
}

bool path_is_relative(const char* path)
{
    return !path_is_absolute(path);
}


#ifdef _WIN32

#ifdef __CAKE__
#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignored "-Wstyle"
#endif

struct TAGDIR
{
    void* handle;
    struct dirent dirent;
};

#ifdef __CAKE__
#pragma CAKE diagnostic pop
#endif

DIR* _Owner _Opt opendir(const char* name)
{
    assert(name != 0);
    WIN32_FIND_DATAA fdfile = { 0 };

    char path[FS_MAX_PATH] = { 0 };
    strcat(path, name);
    strcat(path, "\\*.*");

    HANDLE handle = FindFirstFileA(path, &fdfile);

    if (handle != INVALID_HANDLE_VALUE)
    {
        _Opt DIR* _Owner _Opt p = calloc(1, sizeof * p);
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

int closedir(DIR* _Owner dirp)
{
    FindClose(dirp->handle);
    free(dirp);
    return 0;
}

struct dirent* _Opt readdir(DIR* dirp)
{
    WIN32_FIND_DATAA fdfile = { 0 };
    BOOL b = FindNextFileA(dirp->handle, &fdfile);
    if (b)
    {
        /*clear*/
        memset(&dirp->dirent, 0, sizeof(dirp->dirent));

        if (fdfile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            dirp->dirent.d_type |= DT_DIR;
        }

        /*worst case trunks the string*/
        strncpy(dirp->dirent.d_name,
            fdfile.cFileName,
            sizeof(dirp->dirent.d_name) - 1);

        return &dirp->dirent;
    }
    else
    {
        errno = windows_error_to_posix(GetLastError());
    }
    return NULL;
}

/*
* realpath
   If there is no error, realpath() returns a pointer to the
       resolved_path.

       Otherwise, it returns NULL, the contents of the array
       resolved_path are undefined, and errno is set to indicate the
       error.
*/
char* _Opt realpath(const char* restrict path, char* restrict resolved_path)
{
    /*
    * _fullpath
    * Each of these functions returns a pointer to a buffer
      containing the absolute path name (absPath). If there's an
      error (for example, if the value passed in relPath includes a drive
      letter that isn't valid or can't be found, or if the length of the
      created absolute path name (absPath) is greater than maxLength), the function returns NULL.
    */
#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignored "-Wflow-not-null"
    char* _Opt p = _fullpath(resolved_path, path, FS_MAX_PATH);
    if (p)
    {
        char* p2 = resolved_path;
        while (*p2)
        {
            if (*p2 == '\\')
                *p2 = '/';
            p2++;
        }
    }
#pragma CAKE diagnostic pop

    return p;
}

#endif //_WINDOWS_

int copy_file(const char* pathfrom, const char* pathto)
{

    FILE* _Owner _Opt fd_from = fopen(pathfrom, "rb");
    if (fd_from == NULL)
        return -1;

    FILE* _Owner _Opt fd_to = fopen(pathto, "wb");
    if (fd_to == NULL)
    {
        fclose(fd_from);
        return -1;
    }

    char buf[4096] = { 0 };
    size_t nread;
    while (nread = fread(buf, sizeof(char), sizeof buf, fd_from), nread > 0) //lint !e668  (warning -- possibly passing null pointer to function 'fread(void *, size_t, size_t, FILE *)', arg. no. 4)
    {
        char* out_ptr = buf;
        size_t nwritten;

        do
        {
            nwritten = fwrite(out_ptr, sizeof(char), nread, fd_to);//lint !e668

            nread -= nwritten;
            out_ptr += nwritten;
        } while (nread > 0);
    }

    if (nread == 0)
    {
        fclose(fd_to);
        fclose(fd_from);

        /* Success! */
        return 0;
    }

    fclose(fd_to);
    fclose(fd_from);

    return -1;
}

int copy_folder(const char* from, const char* to)
{
#if !defined __EMSCRIPTEN__
    int errcode = mkdir(to, 0700);
    if (errcode != 0)
    {
        return errcode;
    }

    DIR* _Owner _Opt dir = opendir(from);

    if (dir == NULL)
    {
        return errno;
    }

    struct dirent* _Opt dp;
    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
        {
            /* skip self and parent */
            continue;
        }

        char fromlocal[FS_MAX_PATH] = { 0 };
        snprintf(fromlocal, FS_MAX_PATH, "%s/%s", from, dp->d_name);

        char tolocal[FS_MAX_PATH] = { 0 };
        snprintf(tolocal, FS_MAX_PATH, "%s/%s", to, dp->d_name);

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
#else
    return -1;
#endif
}

#ifdef _WIN32
int get_self_path(char* buffer, int maxsize)
{

#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignored "-Wnullable-to-non-nullable"
#pragma CAKE diagnostic ignored "-Wanalyzer-null-dereference"

    DWORD r = GetModuleFileNameA(NULL, buffer, maxsize);

#pragma CAKE diagnostic pop

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

#endif




#if !defined __EMSCRIPTEN__

/* Find the last occurrence of c1 or c2 in s. */
char* _Opt strrchr_ex(const char* s, int c1, int c2)
{
    const char* _Opt last = NULL;
    const char* p = s;
    while (*p)
    {
        if (*p == c1 || *p == c2)
        {
            last = p;
        }
        p++;
    }
    return (char*)last;
}

void remove_file_extension(const char* filename, int n, char out[/*n*/])
{
    int last_dot_index = -1;
    const char* p = filename;
    int count = 0;
    while (*p)
    {
        if (n == count)
            break;

        if (*p == '.')
            last_dot_index = count;
        out[count] = *p;
        count++;
        p++;
    }

    out[count] = 0;
    if (last_dot_index > 0)
        out[last_dot_index] = 0;
}

char* basename(const char* filename)
{
    char* _Opt p = strrchr_ex(filename, '/', '\\'); //added \ to windows path
    return p ? p + 1 : (char*)filename;
}

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
    else
    {
        path[0] = 0;
    }
    return path;
}
#endif

#ifndef MOCKFILES

char* _Owner _Opt read_file(const char* const path, bool append_newline)
{
    char* _Owner _Opt data = NULL;
    FILE* _Owner _Opt file = NULL;
    struct stat info = { 0 };

    if (stat(path, &info) != 0)
        return NULL;

    size_t mem_size_bytes = sizeof(char) * info.st_size + 1 /* \0 */ + 1 /*newline*/;

    if (mem_size_bytes < 4)
    {
        //we always read 3 chars even if file is small
        mem_size_bytes = 4; //BOM + /0
    }

    data = malloc(mem_size_bytes);
    if (data == NULL)
        return NULL;

    file = fopen(path, "r");
    if (file == NULL)
    {
        free(data);
        return NULL;
    }

    /* first we read 3 bytes */
    size_t bytes_read = fread(data, 1, 3, file);

    if (bytes_read < 3)
    {
        /* we have less than 3 bytes - no BOM */

        data[bytes_read] = '\0';
        if (feof(file))
        {
            fclose(file);
            return data;
        }

        free(data);
        fclose(file);

        return NULL;
    }

    size_t bytes_read_part2 = 0;

    /* check byte order mark (BOM) */
    if ((unsigned char)data[0] == (unsigned char)0xEF &&
        (unsigned char)data[1] == (unsigned char)0xBB &&
        (unsigned char)data[2] == (unsigned char)0xBF)
    {
        /* in this case we skip this BOM, reading again*/
        bytes_read_part2 = fread(&data[0], 1, info.st_size - 3, file);
    }
    else
    {
        bytes_read_part2 = fread(&data[3], 1, info.st_size - 3, file);
        bytes_read_part2 = bytes_read_part2 + 3;
    }

    data[bytes_read_part2] = 0;
    if (append_newline && data[bytes_read_part2 - 1] != '\n')
    {
        /*
        A source file that is not empty shall end in a new-line character, which shall not
        be immediately preceded by a backslash character before any such splicing takes place.
        */
        data[bytes_read_part2] = '\n';

        //we already allocated an extra char for this
        assert(bytes_read_part2 + 1 < mem_size_bytes);
        data[bytes_read_part2 + 1] = '\0';
    }

    fclose(file);
    return data;
}



#else

/*
   used in web build
   embeded standard headers from .\include\
   the tool embed creates the .include version of each file
   in .\include\
*/static const char file_assert_h[] = {
#include "include/assert.h.include"
, 0 };
static const char file_complex_h[] = {
#include "include/complex.h.include"
, 0 };
static const char file_ctype_h[] = {
#include "include/ctype.h.include"
, 0 };
static const char file_errno_h[] = {
#include "include/errno.h.include"
, 0 };
static const char file_fenv_h[] = {
#include "include/fenv.h.include"
, 0 };
static const char file_float_h[] = {
#include "include/float.h.include"
, 0 };
static const char file_inttypes_h[] = {
#include "include/inttypes.h.include"
, 0 };
static const char file_iso646_h[] = {
#include "include/iso646.h.include"
, 0 };
static const char file_limits_h[] = {
#include "include/limits.h.include"
, 0 };
static const char file_locale_h[] = {
#include "include/locale.h.include"
, 0 };
static const char file_math_h[] = {
#include "include/math.h.include"
, 0 };
static const char file_setjmp_h[] = {
#include "include/setjmp.h.include"
, 0 };
static const char file_signal_h[] = {
#include "include/signal.h.include"
, 0 };
static const char file_stdalign_h[] = {
#include "include/stdalign.h.include"
, 0 };
static const char file_stdarg_h[] = {
#include "include/stdarg.h.include"
, 0 };
static const char file_stdatomic_h[] = {
#include "include/stdatomic.h.include"
, 0 };
static const char file_stdbit_h[] = {
#include "include/stdbit.h.include"
, 0 };
static const char file_stdbool_h[] = {
#include "include/stdbool.h.include"
, 0 };
static const char file_stdckdint_h[] = {
#include "include/stdckdint.h.include"
, 0 };
static const char file_stddef_h[] = {
#include "include/stddef.h.include"
, 0 };
static const char file_stdint_h[] = {
#include "include/stdint.h.include"
, 0 };
static const char file_stdio_h[] = {
#include "include/stdio.h.include"
, 0 };
static const char file_stdlib_h[] = {
#include "include/stdlib.h.include"
, 0 };
static const char file_stdnoreturn_h[] = {
#include "include/stdnoreturn.h.include"
, 0 };
static const char file_string_h[] = {
#include "include/string.h.include"
, 0 };
static const char file_tgmath_h[] = {
#include "include/tgmath.h.include"
, 0 };
static const char file_threads_h[] = {
#include "include/threads.h.include"
, 0 };
static const char file_time_h[] = {
#include "include/time.h.include"
, 0 };
static const char file_uchar_h[] = {
#include "include/uchar.h.include"
, 0 };
static const char file_wchar_h[] = {
#include "include/wchar.h.include"
, 0 };
static const char file_wctype_h[] = {
#include "include/wctype.h.include"
, 0 };





char* _Owner read_file(const char* path, bool append_newline)
{
    if (strcmp(path, "c:/assert.h") == 0)
    {
        return strdup(file_assert_h);
    }
    else if (strcmp(path, "c:/complex.h") == 0)
    {
        return strdup(file_complex_h);
    }
    else if (strcmp(path, "c:/ctype.h") == 0)
    {
        return strdup(file_ctype_h);
    }
    else if (strcmp(path, "c:/errno.h") == 0)
    {
        return strdup(file_errno_h);
    }
    else if (strcmp(path, "c:/fenv.h") == 0)
    {
        return strdup(file_fenv_h);
    }
    else if (strcmp(path, "c:/float.h") == 0)
    {
        return strdup(file_float_h);
    }
    else if (strcmp(path, "c:/inttypes.h") == 0)
    {
        return strdup(file_inttypes_h);
    }
    else if (strcmp(path, "c:/iso646.h") == 0)
    {
        return strdup(file_iso646_h);
    }
    else if (strcmp(path, "c:/limits.h") == 0)
    {
        return strdup(file_limits_h);
    }
    else if (strcmp(path, "c:/locale.h") == 0)
    {
        return strdup(file_locale_h);
    }
    else if (strcmp(path, "c:/math.h") == 0)
    {
        return strdup(file_math_h);
    }
    else if (strcmp(path, "c:/setjmp.h") == 0)
    {
        return strdup(file_setjmp_h);
    }
    else if (strcmp(path, "c:/signal.h") == 0)
    {
        return strdup(file_signal_h);
    }
    else if (strcmp(path, "c:/stdalign.h") == 0)
    {
        return strdup(file_stdalign_h);
    }
    else if (strcmp(path, "c:/stdarg.h") == 0)
    {
        return strdup(file_stdarg_h);
    }
    else if (strcmp(path, "c:/stdatomic.h") == 0)
    {
        return strdup(file_stdatomic_h);
    }
    else if (strcmp(path, "c:/stdbit.h") == 0)
    {
        return strdup(file_stdbit_h);
    }
    else if (strcmp(path, "c:/stdbool.h") == 0)
    {
        return strdup(file_stdbool_h);
    }
    else if (strcmp(path, "c:/stdckdint.h") == 0)
    {
        return strdup(file_stdckdint_h);
    }
    else if (strcmp(path, "c:/stddef.h") == 0)
    {
        return strdup(file_stddef_h);
    }
    else if (strcmp(path, "c:/stdint.h") == 0)
    {
        return strdup(file_stdint_h);
    }
    else if (strcmp(path, "c:/stdio.h") == 0)
    {
        return strdup(file_stdio_h);
    }
    else if (strcmp(path, "c:/stdlib.h") == 0)
    {
        return strdup(file_stdlib_h);
    }
    else if (strcmp(path, "c:/stdnoreturn.h") == 0)
    {
        return strdup(file_stdnoreturn_h);
    }
    else if (strcmp(path, "c:/string.h") == 0)
    {
        return strdup(file_string_h);
    }
    else if (strcmp(path, "c:/tgmath.h") == 0)
    {
        return strdup(file_tgmath_h);
    }
    else if (strcmp(path, "c:/threads.h") == 0)
    {
        return strdup(file_threads_h);
    }
    else if (strcmp(path, "c:/time.h") == 0)
    {
        return strdup(file_time_h);
    }
    else if (strcmp(path, "c:/uchar.h") == 0)
    {
        return strdup(file_uchar_h);
    }
    else if (strcmp(path, "c:/wchar.h") == 0)
    {
        return strdup(file_wchar_h);
    }
    else if (strcmp(path, "c:/wctype.h") == 0)
    {
        return strdup(file_wctype_h);
    }
    return NULL;
}
#endif


