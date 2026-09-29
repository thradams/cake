/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake 
*/

#pragma once
#include "cake_compat.h"
#include <stdbool.h>
#include <limits.h>


#if defined(PATH_MAX)
#define FS_MAX_PATH PATH_MAX // Linux uses this in realpath
#elif defined(MAX_PATH)
#define FS_MAX_PATH MAX_PATH // Some systems define this
#else
#define FS_MAX_PATH 500 

#endif
/* outside the #ifdef: the amalgamator (lib.c) keeps only the first include of a header */
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32 
#include <direct.h>

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

//https://man7.org/linux/man-pages/man2/mkdir.2.html
#include <unistd.h>

#ifdef __CAKE__
/*
  The system <dirent.h> carries no ownership information: opendir's result
  reads as non-owning and closedir as not releasing it. It cannot simply be
  re-declared either -- on macos DIR is an anonymous struct typedef, so any
  pre-declaration conflicts with the SDK one. So for the analyzer only,
  declare the three entry points we use (with ownership) and skip the SDK
  header, exactly as the windows branch above already does. The real
  declarations come from <dirent.h> in every non-cake build.
*/
struct _cake_DIR;
typedef struct _cake_DIR DIR;

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
    unsigned char d_type;
    char d_name[256];
};

DIR* _Owner _Opt opendir(const char* name);
int closedir(DIR* _Owner dirp);
struct dirent* _Opt readdir(DIR* dirp);
#else
#include <dirent.h>
#endif

#endif



char* _Opt realpath(const char* restrict path, char* restrict resolved_path);

int get_self_path(char* buffer, int maxsize);

char* _Owner _Opt read_file(const char* path, bool append_newline);

/* the file as it is on disk: no BOM skipping, \r\n kept */
char* _Owner _Opt read_file_binary(const char* path);
bool file_exists(const char* path);
/* an existing regular file (not a directory) */
bool path_is_regular_file(const char* path);
/* last-modified time, or 0 if it can't be stat'ed */
long long file_mtime(const char* path);

/* creates every folder of outdir after root (root itself must exist); 0 or errno */
int create_multiple_paths(const char* root, const char* outdir);
char* dirname(char* path);
char* basename(const char* filename);
void remove_file_extension(const char* filename, int n, char out[/*n*/]);

const char* get_posix_error_message(int error);


bool path_is_relative(const char* path);
bool path_is_absolute(const char* path);
void path_normalize(char* path);
bool path_is_normalized(const char* path);

/* '/' and '\' compare equal; case insensitive on Windows */
bool path_equal(const char* a, const char* b);
/* file is inside dir (dir without a trailing separator) */
bool path_is_under(const char* file, const char* dir);
