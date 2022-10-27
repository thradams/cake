#pragma once


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#include <Windows.h>
#include <direct.h>
#include <sys/types.h>
#include <sys/stat.h>
#define mkdir(a, b) _mkdir(a)
#define rmdir _rmdir
#define mkdir(A, B)  _mkdir(A)
#define chdir  _chdir
#else
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#endif


#if defined(_WIN32) && defined(_MSC_VER) && !defined(__clang__)
#define BUILD_WINDOWS
#define BUILD_WINDOWS_MSC
#define COMPILER_NAME "cl"
#define RUN ""
#elif defined(_WIN32) && defined(__clang__)
#define BUILD_WINDOWS
#define BUILD_WINDOWS_CLANG
#define RUN ""
#define COMPILER_NAME "clang"
#elif defined(__linux__) && defined(__clang__)
#define BUILD_LINUX
#define BUILD_LINUX_CLANG
#define COMPILER_NAME "clang"
#define RUN "./"
#elif defined(__linux__) && defined(__GNUC__)
#define BUILD_LINUX
#define BUILD_LINUX_GCC
#define COMPILER_NAME "gcc"
#define RUN "./"
#elif defined(_WIN32) && defined(__GNUC__)
#define BUILD_WINDOWS
#define BUILD_WINDOWS_GCC
#define COMPILER_NAME "gcc"
#define RUN ""
#else
#error Unknown Platform/Compiler
#endif


#ifndef _WIN32
int system_like(const char* command)
{
    int test_result = system(command);
    int stat = 0;
    wait(&stat);
    if (WIFEXITED(stat))
    {
        test_result = WEXITSTATUS(stat);
    }
    else if (WIFSIGNALED(stat))
    {
        test_result = WTERMSIG(stat);
    }
    return test_result;
}

#define system system_like
#endif



    
#define ESC "\x1b"
#define CSI "\x1b["
#define RESET ESC "[0m"
#define BLACK     "\x1b[30m"
#define BLUE     "\x1b[34m"
#define GREEN     "\x1b[32m"
#define CYAN     "\x1b[36m"
#define RED "\x1b[31;1m"
#define MAGENTA     "\x1b[35m"
#define BROWN     "\x1b[31m"
#define LIGHTGRAY "\x1b[30;1m"
#define DARKGRAY "\x1b[30m"
#define LIGHTBLUE    "\x1b[34;1m"
#define  LIGHTGREEN "\x1b[32,1m"
#define LIGHTCYAN "\x1b[36;1m"
#define LIGHTRED "\x1b[31;1m"
#define LIGHTMAGENTA "\x1b[35;1m"
#define YELLOW "\x1b[33;1m"
#define WHITE "\x1b[37;1m"

extern int build_main();

int main()
{

#ifdef BUILD_LINUX_GCC
#define STR(a) #a
#define F(a)  STR(a)
#define GCC  F(gcc-__GNUC__)
    printf("Gcc version %s %s \n", GCC, __VERSION__);
#endif

    int r = build_main();



    
    return r;
}