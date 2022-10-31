#pragma once

#ifdef _WIN32 
#include <direct.h>
#include <sys/types.h>
#include <sys/stat.h>

//https://docs.microsoft.com/pt-br/cpp/c-runtime-library/reference/mkdir-wmkdir?view=msvc-160
#define mkdir(a, b) _mkdir(a)
#define rmdir _rmdir
#define chdir _chdir
/*
 opendir,  readdir closedir for windows.
 include dirent.h on linux
*/

#ifdef __POCC__
/*missing in pelles c*/
typedef unsigned short ino_t; // inode number (unused on Windows)
typedef long off_t; // file offset value
#endif

enum
{
    DT_UNKNOWN = 0,
    DT_FIFO = 1,
    DT_CHR = 2,
    DT_DIR = 4,
    DT_BLK = 6,
    DT_REG = 8,
    DT_LNK = 10,
    DT_SOCK = 12,
    DT_WHT = 14
};

struct dirent
{
    ino_t d_ino;             /* Inode number */
    off_t d_off;             /* Not an offset; see below */
    unsigned short d_reclen; /* Length of this record */
    unsigned char d_type;    /* Type of file; not supported
                                     by all filesystem types*/
    char d_name[256];        /* Null-terminated filename */
};


struct TAGDIR;
typedef struct TAGDIR DIR;

DIR* opendir(const char* name);
int closedir(DIR* dirp);
struct dirent* readdir(DIR* dirp);


#else

#define MAX_PATH 500

//https://man7.org/linux/man-pages/man2/mkdir.2.html
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#endif



int get_self_path(char* buffer, int maxsize);

char* readfile(const char* path);
char* dirname(char* path);

const char* get_posix_error_message(int error);


