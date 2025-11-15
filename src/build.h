#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//

#if defined(__clang__)
#  define COMPILER_CLANG 1
#define CC_OUTPUT(X) " -o " X " "
#define CC " clang "
#elif defined(__GNUC__) && !defined(__TINYC__) && !defined(__HLC__)
#  define COMPILER_GCC 1
#define CC_OUTPUT(X) " -o " X " "
#define CC " gcc "
#elif defined(_MSC_VER) && !defined(__TINYC__) && !defined(__HLC__)
#  define COMPILER_MSVC 1
#define CC_OUTPUT(FILENAME) " -o " FILENAME " "
#define CC " cl "
#elif defined(__INTEL_COMPILER)
#  define COMPILER_INTEL 1
# error not tested
#define CC " cl "
#elif defined(__TINYC__)
#  define COMPILER_TINYC 1
#define CC " tcc "
#define CC_OUTPUT(X) " -o " X
#else
#  define COMPILER_UNKNOWN 1
#endif


// OS detection

#if defined(_WIN32) || defined(_WIN64)
#  define PLATFORM_WINDOWS 1
#elif defined(__APPLE__) && defined(__MACH__)
#  define PLATFORM_MACOS 1
#elif defined(__linux__)
#  define PLATFORM_LINUX 1
#elif defined(__ANDROID__)
#  define PLATFORM_ANDROID 1
#elif defined(__unix__) || defined(__unix)
#  define PLATFORM_UNIX 1
#elif defined(__FreeBSD__)
#  define PLATFORM_FREEBSD 1
#elif defined(__NetBSD__)
#  define PLATFORM_NETBSD 1
#elif defined(__OpenBSD__)
#  define PLATFORM_OPENBSD 1
#elif defined(__DragonFly__)
#  define PLATFORM_DRAGONFLY 1
#elif defined(__CYGWIN__)
#  define PLATFORM_CYGWIN 1
#elif defined(__HAIKU__)
#  define PLATFORM_HAIKU 1
#elif defined(__sun) && defined(__SVR4)
#  define PLATFORM_SOLARIS 1
#else
#error PLATFORM_UNKNOWN
#endif

      
#if defined(__x86_64__) || defined(_M_X64)
#  define ARCH_X86_64 1
#elif defined(__i386__) || defined(_M_IX86)
#  define ARCH_X86 1
#elif defined(__aarch64__) || defined(_M_ARM64)
#  define ARCH_ARM64 1
#elif defined(__arm__) || defined(_M_ARM)
#  define ARCH_ARM 1
#elif defined(__powerpc64__) || defined(__ppc64__)
#  define ARCH_PPC64 1
#elif defined(__powerpc__) || defined(__ppc__) || defined(_M_PPC)
#  define ARCH_PPC 1
#elif defined(__riscv)
#  define ARCH_RISCV 1
#elif defined(__mips__)
#  define ARCH_MIPS 1
#elif defined(__sparc__)
#  define ARCH_SPARC 1
#elif defined(__s390__)
#  define ARCH_S390 1
#else
#error ARCH_UNKNOWN
#endif

         
#if defined(__GLIBC__)
#  define LIBC_GLIBC 1
#elif defined(__UCLIBC__)
#  define LIBC_UCLIBC 1
#elif defined(__NEWLIB__)
#  define LIBC_NEWLIB 1
#elif defined(__BIONIC__)
#  define LIBC_BIONIC 1
#elif defined(__MSVCRT__) || defined(_MSC_VER)
#  define LIBC_MSVCRT 1
#elif defined(__MINGW32__) || defined(__MINGW64__)
#  define LIBC_MINGW 1
#elif defined(__dietlibc__)
#  define LIBC_DIET 1
#elif defined(__APPLE__) && defined(__MACH__)
#  define LIBC_DARWIN 1
#else
#error LIBC_UNKNOWN
#endif


#if defined(PLATFORM_WINDOWS) || defined(LIBC_MSVCRT) || defined(LIBC_MINGW)
#  define API_WINDOWS 1
#endif

#if defined(PLATFORM_LINUX)     || defined(PLATFORM_MACOS)     || \
    defined(PLATFORM_UNIX)      || defined(PLATFORM_FREEBSD)   || \
    defined(PLATFORM_NETBSD)    || defined(PLATFORM_OPENBSD)   || \
    defined(PLATFORM_DRAGONFLY) || defined(PLATFORM_SOLARIS)   || \
    defined(PLATFORM_ANDROID)   || defined(PLATFORM_CYGWIN)    || \
    defined(PLATFORM_HAIKU)
#  define API_POSIX 1
#endif

#include <stdio.h>

int copy_file(const char *src, const char *dst) {
    FILE *fin = fopen(src, "rb");
    if (!fin) {
        perror("fopen(src)");
        exit(1);
    }

    FILE *fout = fopen(dst, "wb");
    if (!fout) {
        perror("fopen(dst)");
        fclose(fin);
        exit(1);
        return -1;
    }

    int c;
    while ((c = fgetc(fin)) != EOF) {
        if (fputc(c, fout) == EOF) {
            perror("fputc");
            fclose(fin);
            fclose(fout);
            return -1;
        }
    }

    if (ferror(fin)) {
        perror("fgetc");
        fclose(fin);
        fclose(fout);
        return -1;
    }

    fclose(fin);
    fclose(fout);
    return 0;
}


#ifdef PLATFORM_WINDOWS

#include <Windows.h>
#include <direct.h>
#include <sys/types.h>
#include <sys/stat.h>

#define rmdir _rmdir
#define mkdir(A, B)  _mkdir(A)
#define chdir  _chdir

#define BUILD_WINDOWS
#define RUN " "

int system_like(const char* command)
{
    return system(command);
}


#endif

#ifdef API_POSIX

#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>

#define RUN " ./"

int system_like(const char* command)
{
    int status;

    // Execute the command
    status = system(command);

    // Check if the command executed successfully
    if (status == -1)
    {
        perror("Error executing command");
        return -1;
    }
    else
    {
        // Wait for the command to finish
        while (wait(&status) > 0);

        // Check the exit status of the command
        if (WIFEXITED(status))
        {
            // Command exited normally
            return WEXITSTATUS(status);
        }
        else
        {
            // Command exited abnormally
            return -1;
        }
    }
}

#endif



static void execute_cmd(const char* cmd)
{
    printf("%s\n", cmd);
    fflush(stdout);
    if (system_like(cmd) != 0)
    {
        exit(1);
    }
}

static int echo_sytem(const char* cmd)
{
    printf("%s\n", cmd);
    fflush(stdout);
    return system_like(cmd);
}


static void echo_chdir(const char* path)
{
    printf("chdir: %s\n", path);
    fflush(stdout);
    int r = chdir(path);
    if (r != 0)
    {
        printf("chdir failed. Wrong dir?\n");
        exit(1);
    }
    
}










