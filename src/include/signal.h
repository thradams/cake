#ifndef SIGNAL_H
#define SIGNAL_H

typedef void (*sighandler_t)(int);

/* Standard signals */
#define SIGABRT 6
#define SIGFPE 8
#define SIGILL 4
#define SIGINT 2
#define SIGSEGV 11
#define SIGTERM 15
#define SIG_DFL ((sighandler_t)0)
#define SIG_IGN ((sighandler_t)1)

/* Function declarations */
sighandler_t signal(int sig, sighandler_t handler);
int raise(int sig);

#endif /* SIGNAL_H */
