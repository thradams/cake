/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake 
*/

#pragma once
#include "ownership.h"
#include <stdbool.h>
 

#ifdef _WIN32 
#include <direct.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __CAKE__
#pragma cake diagnostic push
#pragma cake diagnostic ignored "-Wstyle"
#endif


//https://docs.microsoft.com/pt-br/cpp/c-runtime-library/reference/mkdir-wmkdir?_View=msvc-160
#define mkdir(a, b) _mkdir(a)
#define rmdir _rmdir
#define chdir _chdir

#ifdef __CAKE__
#pragma cake diagnostic pop
#endif

/*
 opendir,  readdir closedir for windows.
 include dirent.h on linux
*/



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

#ifdef __CAKE__
#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignored "-Wstyle"
#endif
struct TAGDIR;
typedef struct TAGDIR DIR;

#ifdef __CAKE__
#pragma CAKE diagnostic pop
#endif

DIR* _Owner _Opt opendir(const char* name);
int closedir(DIR* _Owner dirp);
struct dirent* _Opt readdir(DIR* dirp);


#else

//TODO fails on macos because it has a diferent declaration
//typedef struct __dirstream DIR;
//DIR * _Owner _Opt opendir (const char *__name);
//int closedir(DIR* _Owner dirp);


#define MAX_PATH 500

//https://man7.org/linux/man-pages/man2/mkdir.2.html
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#endif



char* _Opt realpath(const char* restrict path, char* restrict resolved_path);

int get_self_path(char* buffer, int maxsize);

char* _Owner _Opt read_file(const char* path, bool append_newline);
char* dirname(char* path);
char* basename(const char* filename);
void remove_file_extension(const char* filename, int n, char out[/*n*/]);

const char* get_posix_error_message(int error);


bool path_is_relative(const char* path);
bool path_is_absolute(const char* path);
void path_normalize(char* path);
bool path_is_normalized(const char* path);
