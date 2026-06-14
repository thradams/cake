#pragma once

/*
 * build.h - Build system for Cake
 *
 * Detects compiler, platform, architecture, and libc at compile time,
 * and provides cross-platform helpers for executing build commands.
 *
 * Usage:
 *   Windows:    cl build.c && build
 *   Linux/macOS: gcc build.c -o build && ./build
 */

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ============================================================
 * COMPILER DETECTION
 * ============================================================ */

#if defined(__clang__)
#  define COMPILER_CLANG 1
#  define CC        " clang "
#  define CC_OUTPUT(X) " -o " X " "

#elif defined(__GNUC__) && !defined(__TINYC__) && !defined(__HLC__)
#  define COMPILER_GCC 1
#  define CC        " gcc "
#  define CC_OUTPUT(X) " -o " X " "

#elif defined(_MSC_VER) && !defined(__TINYC__) && !defined(__HLC__)
#  define COMPILER_MSVC 1
#  define CC        " cl "
#  define CC_OUTPUT(X) " -o " X " "

#elif defined(__INTEL_COMPILER)
#  define COMPILER_INTEL 1
#  define CC        " cl "
#  error "Intel compiler not tested"

#elif defined(__TINYC__)
#  define COMPILER_TINYC 1
#  define CC        " tcc "
#  define CC_OUTPUT(X) " -o " X

#else
#  define COMPILER_UNKNOWN 1
#endif


/* ============================================================
 * PLATFORM DETECTION
 * ============================================================ */

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
#  error "PLATFORM_UNKNOWN"
#endif


/* ============================================================
 * ARCHITECTURE DETECTION
 * ============================================================ */

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
#  error "ARCH_UNKNOWN"
#endif


/* ============================================================
 * LIBC DETECTION
 * ============================================================ */

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
#  error "LIBC_UNKNOWN"
#endif


/* ============================================================
 * DERIVED API GROUPS
 *
 * API_WINDOWS - Windows API available (Win32/MSVCRT/MinGW)
 * API_POSIX   - POSIX API available (Linux, macOS, BSDs, etc.)
 * ============================================================ */

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


/* ============================================================
 * PLATFORM-SPECIFIC SETUP
 * ============================================================ */

#ifdef PLATFORM_WINDOWS

#include <Windows.h>
#include <direct.h>
#include <sys/types.h>
#include <sys/stat.h>

/* POSIX compat shims for Windows */
#define rmdir      _rmdir
#define mkdir(A,B) _mkdir(A)
#define chdir      _chdir

/* On Windows, built executables run directly (no ./ prefix needed) */
#define RUN " "

static int system_like(const char* command)
{
    return system(command);
}

#endif /* PLATFORM_WINDOWS */


#ifdef API_POSIX

#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>

/* On POSIX, local executables require ./ prefix */
#define RUN " ./"

/*
 * system_like() - execute a shell command and return its exit code.
 *
 * Wraps system() with proper POSIX wait semantics so the exit code
 * of the child process is reliably returned rather than the raw
 * waitpid status word.
 *
 * Returns: exit code of the command, or -1 on error.
 */
static int system_like(const char* command)
{
    int status = system(command);

    if (status == -1)
    {
        perror("system");
        return -1;
    }

    /* Drain any remaining children */
    while (wait(&status) > 0)
        ;

    if (WIFEXITED(status))
        return WEXITSTATUS(status);

    return -1; /* abnormal exit (signal, etc.) */
}

#endif /* API_POSIX */


/* ============================================================
 * FILE UTILITIES
 * ============================================================ */

/*
 * copy_file() - copy a file byte-by-byte from src to dst.
 *
 * Exits on failure to open src. Returns -1 on write/read errors,
 * 0 on success.
 */
static int copy_file(const char* src, const char* dst)
{
    FILE* fin = fopen(src, "rb");
    if (!fin) { perror("fopen(src)"); exit(1); }

    FILE* fout = fopen(dst, "wb");
    if (!fout) { perror("fopen(dst)"); fclose(fin); exit(1); }

    int c;
    while ((c = fgetc(fin)) != EOF)
    {
        if (fputc(c, fout) == EOF)
        {
            perror("fputc");
            fclose(fin);
            fclose(fout);
            return -1;
        }
    }

    if (ferror(fin))
    {
        perror("fgetc");
        fclose(fin);
        fclose(fout);
        return -1;
    }

    fclose(fin);
    fclose(fout);
    return 0;
}


/* ============================================================
 * BUILD COMMAND HELPERS
 * ============================================================ */

/*
 * execute_cmd() - run a shell command, print it first, exit on failure.
 *
 * This is the primary build action primitive. Any non-zero exit
 * from the command aborts the build immediately.
 */
static void execute_cmd(const char* cmd)
{
    printf("%s\n", cmd);
    fflush(stdout);
    if (system_like(cmd) != 0)
        exit(1);
}

/*
 * echo_sytem() - run a shell command, print it, return exit code.
 *
 * Like execute_cmd() but does NOT abort on failure — use when the
 * caller needs to handle the result itself.
 */
static int echo_sytem(const char* cmd)
{
    printf("%s\n", cmd);
    fflush(stdout);
    return system_like(cmd);
}

/*
 * echo_chdir() - change working directory, print the path, exit on failure.
 *
 * Build scripts rely on relative paths, so a failed chdir would
 * silently corrupt all subsequent paths — aborting is correct here.
 */
static void echo_chdir(const char* path)
{
    printf("chdir: %s\n", path);
    fflush(stdout);
    if (chdir(path) != 0)
    {
        printf("chdir failed: %s\n", path);
        exit(1);
    }
}