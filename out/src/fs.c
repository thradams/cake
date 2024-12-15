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

struct _FILETIME {
    unsigned long dwLowDateTime;
    unsigned long dwHighDateTime;
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

struct HINSTANCE__ {
    int unused;
};

struct dirent {
    unsigned short d_ino;
    long d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[256];
};

struct TAGDIR {
    void * handle;
    struct dirent  dirent;
};

struct _iobuf {
    void * _Placeholder;
};


int tolower(int _C);

unsigned char  path_is_normalized(char * path)
{
    {
        char * p = path;
        for (;  *p; p++)
        {
            int before =  *p;
            int after = tolower( *p);
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
        char * p = path;
        for (;  *p; p++)
        {
             *p = (char)tolower( *p);
            if ( *p == 92)
            {
                 *p = 47;
            }
        }
    }
}

unsigned char  path_is_absolute(char * path)
{
    char ch = (char)tolower(path[0]);
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
    return  !path_is_absolute(path);
}

char *strcat(char * _Destination, char * _Source);
void *FindFirstFileA(char * lpFileName, struct _WIN32_FIND_DATAA * lpFindFileData);
void *calloc(int nmemb, unsigned int size);
int FindClose(void * hFindFile);
int *_errno(void);
int windows_error_to_posix(int i);
unsigned long GetLastError(void);

struct TAGDIR *opendir(char * name)
{
    ;
    struct _WIN32_FIND_DATAA  fdfile = {0};
    char path[260] = {0};
    strcat(path, name);
    strcat(path, "\\*.*");
    void * handle = FindFirstFileA(path, &fdfile);
    if (handle != ((void *)(long) -1))
    {
        struct TAGDIR * p = calloc(1, sizeof  *p);
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
        ( *_errno()) = windows_error_to_posix(GetLastError());
    }
    return ((void *)0);
}

void free(void * ptr);

int closedir(struct TAGDIR * dirp)
{
    FindClose(dirp->handle);
    free(dirp);
    return 0;
}

int FindNextFileA(void * hFindFile, struct _WIN32_FIND_DATAA * lpFindFileData);
void *memset(void * _Dst, int _Val, unsigned int _Size);
char *strncpy(char * _Destination, char * _Source, unsigned int _Count);

struct dirent *readdir(struct TAGDIR * dirp)
{
    struct _WIN32_FIND_DATAA  fdfile = {0};
    int b = FindNextFileA(dirp->handle, &fdfile);
    if (b)
    {
        memset(&dirp->dirent, 0, sizeof (dirp->dirent));
        if (fdfile.dwFileAttributes & 16)
        {
            dirp->dirent.d_type = 4;
        }
        strncpy(dirp->dirent.d_name, fdfile.cFileName, sizeof (dirp->dirent.d_name) - 1);
        return &dirp->dirent;
    }
    else
    {
        ( *_errno()) = windows_error_to_posix(GetLastError());
    }
    return ((void *)0);
}

char *_fullpath(char * _Buffer, char * _Path, unsigned int _BufferCount);

char *realpath(char * path, char * resolved_path)
{
    char * p = _fullpath(resolved_path, path, 260);
    if (p)
    {
        char * p2 = resolved_path;
        while ( *p2)
        {
            if ( *p2 == 92)
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
unsigned int fwrite(void * _Buffer, unsigned int _ElementSize, unsigned int _ElementCount, struct _iobuf * _Stream);

int copy_file(char * pathfrom, char * pathto)
{
    struct _iobuf * fd_from = fopen(pathfrom, "rb");
    if (fd_from == ((void *)0))
    {
        return  -1;
    }
    struct _iobuf * fd_to = fopen(pathto, "wb");
    if (fd_to == ((void *)0))
    {
        fclose(fd_from);
        return  -1;
    }
    char buf[4096] = {0};
    unsigned int nread;
    while (nread = fread(buf, sizeof (char), sizeof buf, fd_from), nread > 0)
    {
        char * out_ptr = buf;
        unsigned int nwritten;
        do
        {
            nwritten = fwrite(out_ptr, sizeof (char), nread, fd_to);
            nread = nwritten;
            out_ptr = nwritten;
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
    return  -1;
}

int _mkdir(char * _Path);
int strcmp(char * _Str1, char * _Str2);
int snprintf(char * _Buffer, unsigned int _BufferCount, char * _Format, ...);

int copy_folder(char * from, char * to)
{
    int errcode = _mkdir(to);
    if (errcode != 0)
    {
        return errcode;
    }
    struct TAGDIR * dir = opendir(from);
    if (dir == ((void *)0))
    {
        return ( *_errno());
    }
    struct dirent * dp;
    while ((dp = readdir(dir)) != ((void *)0))
    {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
        {
            continue;
        }
        char fromlocal[260] = {0};
        snprintf(fromlocal, 260, "%s/%s", from, dp->d_name);
        char tolocal[260] = {0};
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

unsigned long GetModuleFileNameA(struct HINSTANCE__ * hModule, char * lpFilename, unsigned long nSize);

int get_self_path(char * buffer, int maxsize)
{
    unsigned long r = GetModuleFileNameA(((void *)0), buffer, maxsize);
    return r;
}

char *strrchr_ex(char * s, int c1, int c2)
{
    char * last = ((void *)0);
    char * p = s;
    while ( *p)
    {
        if ( *p == c1 ||  *p == c2)
        {
            last = p;
        }
        p++;
    }
    return (char *)last;
}

char *basename(char * filename)
{
    char * p = strrchr_ex(filename, 47, 92);
    return p ? p + 1 : (char *)filename;
}

char *dirname(char * path)
{
    int last =  -1;
    {
        int i = 0;
        for (; path[i]; i++)
        {
            if (path[i] == 92 || path[i] == 47)
            {
                last = i;
            }
        }
    }
    if (last !=  -1)
    {
        path[last] = 0;
    }
    else
    {
        path[0] = 0;
    }
    return path;
}


int _stat64i32(char * _FileName, struct _stat64i32 * _Stat);
inline static int stat(char * _FileName, struct stat * _Stat)
{
    return _stat64i32(_FileName, (struct _stat64i32 *)_Stat);
}
void *malloc(unsigned int size);
int feof(struct _iobuf * _Stream);

char *read_file(char * path, unsigned char   append_newline)
{
    char * data = ((void *)0);
    struct _iobuf * file = ((void *)0);
    struct stat  info = {0};
    if (stat(path, &info) != 0)
    {
        return ((void *)0);
    }
    int mem_size_bytes = sizeof (char) * info.st_size + 1 + 1;
    if (mem_size_bytes < 4)
    {
        mem_size_bytes = 4;
    }
    data = malloc(mem_size_bytes);
    if (data == ((void *)0))
    {
        return ((void *)0);
    }
    file = fopen(path, "r");
    if (file == ((void *)0))
    {
        free(data);
        return ((void *)0);
    }
    unsigned int bytes_read = fread(data, 1, 3, file);
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
        return ((void *)0);
    }
    unsigned int bytes_read_part2 = 0;
    if ((unsigned char)data[0] == (unsigned char)239 && (unsigned char)data[1] == (unsigned char)187 && (unsigned char)data[2] == (unsigned char)191)
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

