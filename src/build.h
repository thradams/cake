#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#ifdef _WIN32

    #include <Windows.h>
    #include <direct.h>
    #include <sys/types.h>
    #include <sys/stat.h>

    #define rmdir _rmdir
    #define mkdir(A, B)  _mkdir(A)
    #define chdir  _chdir

    #define BUILD_WINDOWS
    #define RUN ""

    #if defined __clang__

        #define BUILD_WINDOWS_CLANG
        #define CC "clang "
        #define OUT_OPT " -o "
        #define CC_DESCRIPTION "clang windows"

    #elif defined __POCC__ /*pelles C*/

        /*use cc -Ze build.c */
        #define BUILD_WINDOWS_POCC
        #define CC "cc "
        #define OUT_OPT " /OUT:"
        #define CC_DESCRIPTION "pelles c"

    #elif defined __GNUC__

        #define BUILD_WINDOWS_GCC
        #define CC "gcc "
        #define OUT_OPT " -o "
        #define CC_DESCRIPTION "mingw"

    #elif defined __ORANGEC__

        /*https://github.com/LADSoft/OrangeC*/

        #define BUILD_WINDOWS_OCC
        #define CC "occ "
        #define OUT_OPT " /o"
        #define CC_DESCRIPTION "orange c"

    #elif defined __TINYC__

        #define BUILD_WINDOWS_TCC
        #define CC "tcc "
        #define OUT_OPT " -o "
        #define CC_DESCRIPTION "tiny c compiler"

    #elif defined _MSC_VER

        #define BUILD_WINDOWS_MSC
        #define CC "cl "
        #define OUT_OPT " -o "
        #define CC_DESCRIPTION "msvc"

    #else

        #error unknown compiler for windows

    #endif


#elif __linux__

    #include <sys/wait.h>
    #include <sys/stat.h>
    #include <unistd.h>

    #define BUILD_LINUX
    #define RUN "./"

    #if defined __clang__

        #define BUILD_LINUX_CLANG
        #define CC "clang "
        #define OUT_OPT " -o "
        #define CC_DESCRIPTION "clang linux"

    #elif defined __GNUC__

        #define BUILD_LINUX_GCC
        #define CC "gcc "
        #define OUT_OPT " -o "
        #define CC_DESCRIPTION "gcc"

    #else

        #error unknown compiler for linux

    #endif

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
#else

#error Unknown Platform

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

