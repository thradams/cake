#include "fs.h"



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

#ifdef _WIN32


struct TAGDIR
{
    void* handle;
    struct dirent dirent;
};


DIR* opendir(const char* name)
{
    assert(name != 0);
    WIN32_FIND_DATAA fdfile;

    char path[MAX_PATH] = { 0 };
    strcat(path, name);
    strcat(path, "\\*.*");

    HANDLE handle = FindFirstFileA(path, &fdfile);

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
    WIN32_FIND_DATAA fdfile;
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

#endif //_WINDOWS_

int copy_file(const char* pathfrom,
              const char* pathto)
{
    char buf[4096];
    size_t nread;
    int saved_errno;

    FILE* fd_from = fopen(pathfrom, "rb");
    if (fd_from == NULL)
        return -1;

    FILE* fd_to = fopen(pathto, "wb");
    if (fd_to == NULL)
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


char* readfile(const char* path)
{
#ifdef MOCKFILES
    
    /*
    * .include files are generated by the build. (tool/embed)
    */

    static const unsigned char file_stdio_h[] = {
#include ".\web_include\stdio.h.include"
    ,0 };

    static const unsigned char file_stdlib_h[] = {
#include ".\web_include\stdlib.h.include"
    ,0 };

    static const unsigned char file_math_h[] = {
#include ".\web_include\math.h.include"
    ,0 };

    static const unsigned char file_errno_h[] = {
#include ".\web_include\errno.h.include"
    ,0 };

    static const unsigned char file_string_h[] = {
#include ".\web_include\string.h.include"
    ,0 };

    static const unsigned char file_assert_h[] = {
#include ".\web_include\assert.h.include"
    ,0 };

    if (strcmp(path, "stdio.h") == 0)
        return strdup(file_stdio_h);
    else if (strcmp(path, "stdlib.h") == 0)
        return strdup(file_stdlib_h);
    else if (strcmp(path, "math.h") == 0)
        return strdup(file_math_h);
    else if (strcmp(path, "errno.h") == 0)
        return strdup(file_errno_h);
    else if (strcmp(path, "string.h") == 0)
        return strdup(file_string_h);
    else if (strcmp(path, "assert.h") == 0)
        return strdup(file_assert_h);
  
#endif

    return readfile_core(path);
}


