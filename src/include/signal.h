/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

    #pragma once

    typedef int sig_atomic_t;
    typedef void (*__cake_sighandler_t)(int);

    #define SIG_DFL ((__cake_sighandler_t)0)
    #define SIG_IGN ((__cake_sighandler_t)1)
    #define SIG_ERR ((__cake_sighandler_t)-1)

    #if defined(_WIN32)
        #define SIGINT  2
        #define SIGILL  4
        #define SIGABRT 22
        #define SIGFPE  8
        #define SIGSEGV 11
        #define SIGTERM 15
    #else
        #define SIGINT  2
        #define SIGILL  4
        #define SIGABRT 6
        #define SIGFPE  8
        #define SIGSEGV 11
        #define SIGTERM 15
    #endif

    __cake_sighandler_t signal(int sig, __cake_sighandler_t func);
    int raise(int sig);

    #if !defined(_WIN32)
        /* POSIX: the same numbers on Linux and macOS */
        #define SIGHUP  1
        #define SIGQUIT 3
        #define SIGKILL 9
        #define SIGPIPE 13
        #define SIGALRM 14
        int kill(int pid, int sig);   /* pid_t is int on Linux and macOS */
    #endif

#else
    #include_next <signal.h>
#endif
