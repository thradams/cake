/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

    #pragma once

    #include <__cake_types.h>
    #include <time.h>

    #if defined(_WIN32)

        /* the msvc CRT exports only _stat64i32/_fstat64i32, stat and fstat are
           inline wrappers in its headers; the struct has the same layout */
        struct _stat64i32
        {
            unsigned int st_dev;
            unsigned short st_ino;
            unsigned short st_mode;
            short st_nlink;
            short st_uid;
            short st_gid;
            unsigned int st_rdev;
            long st_size;
            long long st_atime;
            long long st_mtime;
            long long st_ctime;
        };

        int _stat64i32(const char* path, _Out struct _stat64i32* buf);
        int _fstat64i32(int fd, _Out struct _stat64i32* buf);

        #define stat  _stat64i32
        #define fstat _fstat64i32

        #define S_IFMT   0xF000
        #define S_IFDIR  0x4000
        #define S_IFCHR  0x2000
        #define S_IFIFO  0x1000
        #define S_IFREG  0x8000
        #define S_IREAD  0x0100
        #define S_IWRITE 0x0080
        #define S_IEXEC  0x0040

        #define _S_IFMT   S_IFMT
        #define _S_IFDIR  S_IFDIR
        #define _S_IFCHR  S_IFCHR
        #define _S_IFIFO  S_IFIFO
        #define _S_IFREG  S_IFREG
        #define _S_IREAD  S_IREAD
        #define _S_IWRITE S_IWRITE
        #define _S_IEXEC  S_IEXEC

    #else

        #if defined(__APPLE__)

            typedef __cake_int32_t dev_t;
            typedef __cake_uint64_t ino_t;
            typedef __cake_uint16_t mode_t;
            typedef __cake_uint16_t nlink_t;
            typedef __cake_uint32_t uid_t;
            typedef __cake_uint32_t gid_t;
            typedef __cake_int64_t off_t;
            typedef __cake_int64_t blkcnt_t;
            typedef __cake_int32_t blksize_t;

            struct stat
            {
                dev_t st_dev;
                mode_t st_mode;
                nlink_t st_nlink;
                ino_t st_ino;
                uid_t st_uid;
                gid_t st_gid;
                dev_t st_rdev;
                struct timespec st_atimespec;
                struct timespec st_mtimespec;
                struct timespec st_ctimespec;
                struct timespec st_birthtimespec;
                off_t st_size;
                blkcnt_t st_blocks;
                blksize_t st_blksize;
                __cake_uint32_t st_flags;
                __cake_uint32_t st_gen;
                __cake_int32_t st_lspare;
                __cake_int64_t st_qspare[2];
            };

            #define st_atime st_atimespec.tv_sec
            #define st_mtime st_mtimespec.tv_sec
            #define st_ctime st_ctimespec.tv_sec

        #elif defined(__linux__)

            typedef unsigned long dev_t;
            typedef unsigned long ino_t;
            typedef unsigned int mode_t;
            typedef unsigned int uid_t;
            typedef unsigned int gid_t;
            typedef long off_t;
            typedef long blkcnt_t;

            #if defined(__x86_64__)

                typedef unsigned long nlink_t;
                typedef long blksize_t;

                struct stat
                {
                    dev_t st_dev;
                    ino_t st_ino;
                    nlink_t st_nlink;
                    mode_t st_mode;
                    uid_t st_uid;
                    gid_t st_gid;
                    int __pad0;
                    dev_t st_rdev;
                    off_t st_size;
                    blksize_t st_blksize;
                    blkcnt_t st_blocks;
                    struct timespec st_atim;
                    struct timespec st_mtim;
                    struct timespec st_ctim;
                    long __glibc_reserved[3];
                };

            #else

                /* the generic layout (aarch64, riscv64) */
                typedef unsigned int nlink_t;
                typedef int blksize_t;

                struct stat
                {
                    dev_t st_dev;
                    ino_t st_ino;
                    mode_t st_mode;
                    nlink_t st_nlink;
                    uid_t st_uid;
                    gid_t st_gid;
                    dev_t st_rdev;
                    unsigned long __pad1;
                    off_t st_size;
                    blksize_t st_blksize;
                    int __pad2;
                    blkcnt_t st_blocks;
                    struct timespec st_atim;
                    struct timespec st_mtim;
                    struct timespec st_ctim;
                    int __glibc_reserved[2];
                };

            #endif

            #define st_atime st_atim.tv_sec
            #define st_mtime st_mtim.tv_sec
            #define st_ctime st_ctim.tv_sec

        #endif

        #define S_IFMT   0170000
        #define S_IFSOCK 0140000
        #define S_IFLNK  0120000
        #define S_IFREG  0100000
        #define S_IFBLK  0060000
        #define S_IFDIR  0040000
        #define S_IFCHR  0020000
        #define S_IFIFO  0010000

        #define S_ISUID 04000
        #define S_ISGID 02000
        #define S_ISVTX 01000

        #define S_IRWXU 0700
        #define S_IRUSR 0400
        #define S_IWUSR 0200
        #define S_IXUSR 0100
        #define S_IRWXG 070
        #define S_IRGRP 040
        #define S_IWGRP 020
        #define S_IXGRP 010
        #define S_IRWXO 07
        #define S_IROTH 04
        #define S_IWOTH 02
        #define S_IXOTH 01

        #define S_ISREG(m)  (((m) & S_IFMT) == S_IFREG)
        #define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR)
        #define S_ISCHR(m)  (((m) & S_IFMT) == S_IFCHR)
        #define S_ISBLK(m)  (((m) & S_IFMT) == S_IFBLK)
        #define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
        #define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK)
        #define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)

        int stat(const char* restrict path, _Out struct stat* restrict buf);
        int fstat(int fd, _Out struct stat* buf);
        int lstat(const char* restrict path, _Out struct stat* restrict buf);
        int fstatat(int fd, const char* restrict path, _Out struct stat* restrict buf, int flag);
        int chmod(const char* path, mode_t mode);
        int fchmod(int fd, mode_t mode);
        int fchmodat(int fd, const char* path, mode_t mode, int flag);
        int mkdir(const char* path, mode_t mode);
        int mkdirat(int fd, const char* path, mode_t mode);
        int mkfifo(const char* path, mode_t mode);
        int mknod(const char* path, mode_t mode, dev_t dev);
        mode_t umask(mode_t mask);
        int futimens(int fd, const struct timespec times[2]);
        int utimensat(int fd, const char* path, const struct timespec times[2], int flag);

    #endif

#else

    struct stat;

    #ifdef _WIN32
        /* msvc defines stat as a static inline */
        static __inline int __cdecl stat(char const* const _FileName, _Out struct stat* const _Stat);
    #else
        int stat(const char* restrict _FileName, _Out struct stat* restrict _Stat);
    #endif

    #include_next <sys/stat.h>
#endif
