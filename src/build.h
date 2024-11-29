#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#if defined(_M_IX86)  || defined(__i386__) /*gcc*/
    #define CPU_ARCHITECTURE_TEXT "X86"
#elif defined(_M_X64) || defined(__x86_64__)
    #define CPU_ARCHITECTURE_TEXT "X64"
#else
    #define CPU_ARCHITECTURE_TEXT "??"
#endif


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
#define CC_DESCRIPTION "CLANG WINDOWS "

#elif defined __GNUC__

#define BUILD_WINDOWS_GCC
#define CC "gcc "
#define OUT_OPT " -o "
#define CC_DESCRIPTION "MINGW " 

#elif defined __HLC__

#define BUILD_WINDOWS_HLC
#define CC "hlc "
#define OUT_OPT " -o "
#define CC_DESCRIPTION "HLC MSVC " 

#elif defined _MSC_VER

#define BUILD_WINDOWS_MSC
#define CC "cl "
#define OUT_OPT " -o "
#define CC_DESCRIPTION "MSVC " 

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
#elif __APPLE__

#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUILD_MACOS
#define RUN "./"

#if defined __clang__

#define BUILD_MACOS_CLANG
#define CC "clang "
#define OUT_OPT " -o "
#define CC_DESCRIPTION "clang macos" 

#elif defined __GNUC__

#define BUILD_MACOS_GCC

#ifndef GCC
#error "You must specify the GCC executable to use, as `gcc` redirects to apple clang!"
#endif
#define CC GCC" "
#define OUT_OPT " -o "
#define CC_DESCRIPTION GCC 

#else

#error unknown compiler for macos

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


static int echo_chdir(const char* path)
{
    printf("chdir: %s\n", path);
    fflush(stdout);
    return chdir(path);
}
