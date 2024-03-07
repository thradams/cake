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

int system_like(const char* command)
{
    return system(command);
}

#if defined __clang__

#define BUILD_WINDOWS_CLANG
#define CC "clang "
#define OUT_OPT " -o "
#define CC_DESCRIPTION "clang windows"

#elif defined __GNUC__

#define BUILD_WINDOWS_GCC
#define CC "gcc "
#define OUT_OPT " -o "
#define CC_DESCRIPTION "mingw"

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

#define system system_like
#else

#error Unknown Platform

#endif


#ifdef WITH_ANSI_COLOR
#define COLOR_ESC(x) x
#else
#define COLOR_ESC(x) ""
#endif
#define ESC COLOR_ESC("\x1b")
#define CSI COLOR_ESC("\x1b[")
#define RESET ESC COLOR_ESC("[0m")
#define BLACK     COLOR_ESC("\x1b[30m")
#define BLUE     COLOR_ESC("\x1b[34m")
#define GREEN     COLOR_ESC("\x1b[32m")
#define CYAN     COLOR_ESC("\x1b[36m")
#define RED COLOR_ESC("\x1b[31;1m")
#define MAGENTA     COLOR_ESC("\x1b[35m")
#define BROWN     COLOR_ESC("\x1b[31m")
#define LIGHTGRAY COLOR_ESC("\x1b[30;1m")
#define DARKGRAY COLOR_ESC("\x1b[30m")
#define LIGHTBLUE    COLOR_ESC("\x1b[34;1m")
#define  LIGHTGREEN COLOR_ESC("\x1b[32,1m")
#define LIGHTCYAN COLOR_ESC("\x1b[36;1m")
#define LIGHTRED COLOR_ESC("\x1b[31;1m")
#define LIGHTMAGENTA COLOR_ESC("\x1b[35;1m")
#define YELLOW COLOR_ESC("\x1b[33;1m")
#define WHITE COLOR_ESC("\x1b[37;1m")

