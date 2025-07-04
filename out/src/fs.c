struct _stat64i32 {
    unsigned int st_dev;
    unsigned short st_ino;
    unsigned short st_mode;
    short st_nlink;
    short st_uid;
    short st_gid;
    unsigned int st_rdev;
    long st_size;
    __int64 st_atime;
    __int64 st_mtime;
    __int64 st_ctime;
};

struct _iobuf {
    void * _Placeholder;
};

struct dirent {
    unsigned short d_ino;
    long d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[256];
};

struct _FILETIME {
    unsigned long dwLowDateTime;
    unsigned long dwHighDateTime;
};

struct stat {
    unsigned int st_dev;
    unsigned short st_ino;
    unsigned short st_mode;
    short st_nlink;
    short st_uid;
    short st_gid;
    unsigned int st_rdev;
    long st_size;
    __int64 st_atime;
    __int64 st_mtime;
    __int64 st_ctime;
};

struct _WIN32_FIND_DATAA {
    unsigned long dwFileAttributes;
    struct _FILETIME  ftCreationTime;
    struct _FILETIME  ftLastAccessTime;
    struct _FILETIME  ftLastWriteTime;
    unsigned long nFileSizeHigh;
    unsigned long nFileSizeLow;
    unsigned long dwReserved0;
    unsigned long dwReserved1;
    char cFileName[260];
    char cAlternateFileName[14];
};

struct TAGDIR {
    void * handle;
    struct dirent  dirent;
};

struct HINSTANCE__ {
    int unused;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

int _Avx2WmemEnabledWeakValue = 0;
int __cdecl tolower(int _C);

unsigned char  path_is_normalized(char * path)
{
    {
        char * p;
        p = path;
        for (; *p; p++)
        {
            int before;
            int after;

            before = *p;
            after = tolower(*p);
            if (before != after)
            {
                return 0;
            }
            if (after == 92)
            {
                return 0;
            }
        }
    }
    return 1;
}

void path_normalize(char * path)
{
    {
        char * p;
        p = path;
        for (; *p; p++)
        {
            *p = (char)tolower(*p);
            if (*p == 92)
            {
                *p = 47;
            }
        }
    }
}

unsigned char  path_is_absolute(char * path)
{
    char ch;

    ch = (char)tolower(path[0]);
    if (ch >= 97 && ch <= 122)
    {
        if (path[1] == 58 && (path[2] == 92 || path[2] == 47))
        {
            return 1;
        }
    }
    if (path[0] == 92 && path[1] == 92)
    {
        return 1;
    }
    return 0;
}

unsigned char  path_is_relative(char * path)
{
    return !!(!path_is_absolute(path));
}

char *__cdecl strcat(char * _Destination, char * _Source);
void *__stdcall FindFirstFileA(char * lpFileName, struct _WIN32_FIND_DATAA * lpFindFileData);
void *calloc(unsigned int nmemb, unsigned int size);
int __stdcall FindClose(void * hFindFile);
int *__cdecl _errno(void);
int windows_error_to_posix(int i);
unsigned long __stdcall GetLastError(void);

struct TAGDIR *opendir(char * name)
{
    struct _WIN32_FIND_DATAA  fdfile;
    char path[260];
    void * handle;

    ;
    _cake_zmem(&fdfile, 320);
    _cake_zmem(&path, 260);
    strcat(path, name);
    strcat(path, "\\*.*");
    handle = FindFirstFileA(path, &fdfile);
    if (handle != 4294967295U)
    {
        struct TAGDIR * p;

        p = calloc(1, 272U);
        if (p)
        {
            p->handle = handle;
            return p;
        }
        else
        {
            FindClose(handle);
        }
    }
    else
    {
        (*_errno()) = windows_error_to_posix(GetLastError());
    }
    return 0U;
}

void free(void * ptr);

int closedir(struct TAGDIR * dirp)
{
    FindClose(dirp->handle);
    free(dirp);
    return 0;
}

int __stdcall FindNextFileA(void * hFindFile, struct _WIN32_FIND_DATAA * lpFindFileData);
void *__cdecl memset(void * _Dst, int _Val, unsigned int _Size);
char *__cdecl strncpy(char * _Destination, char * _Source, unsigned int _Count);

struct dirent *readdir(struct TAGDIR * dirp)
{
    struct _WIN32_FIND_DATAA  fdfile;
    int b;

    _cake_zmem(&fdfile, 320);
    b = FindNextFileA(dirp->handle, &fdfile);
    if (b)
    {
        memset(&dirp->dirent, 0, 268U);
        if (fdfile.dwFileAttributes & 16)
        {
            dirp->dirent.d_type |= 4;
        }
        strncpy(dirp->dirent.d_name, fdfile.cFileName, 255);
        return &dirp->dirent;
    }
    else
    {
        (*_errno()) = windows_error_to_posix(GetLastError());
    }
    return 0U;
}

char *__cdecl _fullpath(char * _Buffer, char * _Path, unsigned int _BufferCount);

char *realpath(char * path, char * resolved_path)
{
    char * p;

    p = _fullpath(resolved_path, path, 260);
    if (p)
    {
        char * p2;

        p2 = resolved_path;
        while (*p2)
        {
            if (*p2 == 92)
            {
                *p2 = 47;
            }
            p2++;
        }
    }
    return p;
}

struct _iobuf *fopen(char * _FileName, char * _Mode);
int fclose(struct _iobuf * _Stream);
unsigned int fread(void * _Buffer, unsigned int _ElementSize, unsigned int _ElementCount, struct _iobuf * _Stream);
unsigned int __cdecl fwrite(void * _Buffer, unsigned int _ElementSize, unsigned int _ElementCount, struct _iobuf * _Stream);

int copy_file(char * pathfrom, char * pathto)
{
    struct _iobuf * fd_from;
    struct _iobuf * fd_to;
    char buf[4096];
    unsigned int nread;

    fd_from = fopen(pathfrom, "rb");
    if (fd_from == 0U)
    {
        return -1;
    }
    fd_to = fopen(pathto, "wb");
    if (fd_to == 0U)
    {
        fclose(fd_from);
        return -1;
    }
    _cake_zmem(&buf, 4096);
    while (nread = fread(buf, 1U, 4096U, fd_from), nread > 0)
    {
        char * out_ptr;
        unsigned int nwritten;

        out_ptr = buf;
        do
        {
            nwritten = fwrite(out_ptr, 1U, nread, fd_to);
            nread -= nwritten;
            out_ptr += nwritten;
        }
        while (nread > 0);
    }
    if (nread == 0)
    {
        fclose(fd_to);
        fclose(fd_from);
        return 0;
    }
    fclose(fd_to);
    fclose(fd_from);
    return -1;
}

int __cdecl _mkdir(char * _Path);
int __cdecl strcmp(char * _Str1, char * _Str2);
int snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);

int copy_folder(char * from, char * to)
{
    int errcode;
    struct TAGDIR * dir;
    struct dirent * dp;

    errcode = _mkdir(to);
    if (errcode != 0)
    {
        return errcode;
    }
    dir = opendir(from);
    if (dir == 0U)
    {
        return (*_errno());
    }
    while ((dp = readdir(dir)) != 0U)
    {
        char fromlocal[260];
        char tolocal[260];

        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
        {
            continue;
        }
        _cake_zmem(&fromlocal, 260);
        snprintf(fromlocal, 260, "%s/%s", from, dp->d_name);
        _cake_zmem(&tolocal, 260);
        snprintf(tolocal, 260, "%s/%s", to, dp->d_name);
        if (dp->d_type & 4)
        {
            errcode = copy_folder(fromlocal, tolocal);
        }
        else
        {
            errcode = copy_file(fromlocal, tolocal);
        }
        if (errcode != 0)
        {
            break;
        }
    }
    closedir(dir);
    return errcode;
}

unsigned long __stdcall GetModuleFileNameA(struct HINSTANCE__ * hModule, char * lpFilename, unsigned long nSize);

int get_self_path(char * buffer, int maxsize)
{
    unsigned long r;

    r = GetModuleFileNameA(0U, buffer, maxsize);
    return r;
}

char *strrchr_ex(char * s, int c1, int c2)
{
    char * last;
    char * p;

    last = 0U;
    p = s;
    while (*p)
    {
        if (*p == c1 || *p == c2)
        {
            last = p;
        }
        p++;
    }
    return (char *)last;
}

char *basename(char * filename)
{
    char * p;

    p = strrchr_ex(filename, 47, 92);
    return p ? p + 1 : (char *)filename;
}

char *dirname(char * path)
{
    int last;

    last = -1;
    {
        int i;
        i = 0;
        for (; path[i]; i++)
        {
            if (path[i] == 92 || path[i] == 47)
            {
                last = i;
            }
        }
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


int __cdecl _stat64i32(char * _FileName, struct _stat64i32 * _Stat);
inline static int __cdecl stat(char * _FileName, struct stat * _Stat)
{
    return _stat64i32(_FileName, (struct _stat64i32 *)_Stat);
}
void *malloc(unsigned int size);
int __cdecl feof(struct _iobuf * _Stream);

char *read_file(char * path, unsigned char   append_newline)
{
    char * data;
    struct _iobuf * file;
    struct stat  info;
    int mem_size_bytes;
    unsigned int bytes_read;
    unsigned int bytes_read_part2;

    data = 0U;
    file = 0U;
    _cake_zmem(&info, 48);
    if (stat(path, &info) != 0)
    {
        return 0U;
    }
    mem_size_bytes = 1U * info.st_size + 1 + 1;
    if (mem_size_bytes < 4)
    {
        mem_size_bytes = 4;
    }
    data = malloc(mem_size_bytes);
    if (data == 0U)
    {
        return 0U;
    }
    file = fopen(path, "r");
    if (file == 0U)
    {
        free(data);
        return 0U;
    }
    bytes_read = fread(data, 1, 3, file);
    if (bytes_read < 3)
    {
        data[bytes_read] = 0;
        if (feof(file))
        {
            fclose(file);
            return data;
        }
        free(data);
        fclose(file);
        return 0U;
    }
    bytes_read_part2 = 0;
    if ((unsigned char)data[0] == 239 && (unsigned char)data[1] == 187 && (unsigned char)data[2] == 191)
    {
        bytes_read_part2 = fread(&data[0], 1, info.st_size - 3, file);
    }
    else
    {
        bytes_read_part2 = fread(&data[3], 1, info.st_size - 3, file);
        bytes_read_part2 = bytes_read_part2 + 3;
    }
    data[bytes_read_part2] = 0;
    if (append_newline && data[bytes_read_part2 - 1] != 10)
    {
        data[bytes_read_part2] = 10;
        ;
        data[bytes_read_part2 + 1] = 0;
    }
    fclose(file);
    return data;
}

