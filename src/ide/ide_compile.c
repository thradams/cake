/* ide_compile.c - runs the Cake compiler, linked in, on a worker thread,
 * as the old IDE does (compile_stream_* in ide.c): stdout goes to a pipe
 * the UI thread drains with ide_compile_poll, from a gui timer.
 *
 * Two traps, both from the old IDE:
 *  1. A GUI program may start with no valid stdout, so it is reopened on
 *     NUL before being dup2'ed onto the pipe.
 *  2. stdout must be unbuffered or nothing streams until the end.
 */
#include "ide_shell.h"
#include "../compile.h"
#include "../parser.h"
#include "../tinycthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif

#define COMPILE_MAX_OUTPUT (8 * 1024 * 1024)

struct ide_compile_job
{
    int argc;
    const char* argv[64];
    char storage[64][1024];
    struct report report;
    thrd_t thread;
    int saved_stdout;
    volatile int finished;   /* set last by the worker */
    int running;
    struct ide_text text;   /* everything read so far */
    struct ide_build_listener listener;
    struct include_listener include_listener;
#ifdef _WIN32
    HANDLE hread;
#else
    int fdread;
#endif
};

struct ide_compile_job* ide_compile_create(void)
{
    return calloc(1, sizeof(struct ide_compile_job));
}

void ide_compile_destroy(struct ide_compile_job* job)
{
    if (job)
        free(job->text.data);
    free(job);
}

void ide_compile_arg(struct ide_compile_job* job, const char* arg)
{
    if (job->argc < (int)(sizeof job->argv / sizeof job->argv[0]))
    {
        snprintf(job->storage[job->argc], sizeof job->storage[0], "%s", arg);
        job->argv[job->argc] = job->storage[job->argc];
        job->argc++;
    }
}

static void append(struct ide_compile_job* job, const char* data, size_t n);

static void on_included(void* data, const char* source, const char* header)
{
    struct ide_compile_job* job = data;
    job->listener.included(job->listener.ctx, source, header);
}

static void on_done(void* data, const char* source, int errors)
{
    struct ide_compile_job* job = data;
    if (job->listener.done)
        job->listener.done(job->listener.ctx, source, errors);
}

void ide_compile_set_listener(struct ide_compile_job* job, const struct ide_build_listener* l)
{
    memset(&job->listener, 0, sizeof job->listener);
    if (l)
        job->listener = *l;
}

void ide_compile_command(const struct ide_compile_job* job, char* buf, size_t cap)
{
    size_t n = 0;
    buf[0] = '\0';
    for (int i = 0; i < job->argc && n < cap; i++)
        n += (size_t)snprintf(buf + n, cap - n, "%s\n", job->argv[i]);
}

void ide_compile_note(struct ide_compile_job* job, const char* text)
{
    append(job, text, strlen(text));
}

void ide_compile_reset(struct ide_compile_job* job)
{
    memset(&job->listener, 0, sizeof job->listener);
    job->argc = 0;
    job->text.len = 0;
    if (job->text.data)
        job->text.data[0] = '\0';
}

int ide_compile_running(const struct ide_compile_job* job)
{
    return job->running;
}

const char* ide_compile_output(const struct ide_compile_job* job)
{
    return job->text.data ? job->text.data : "";
}

void ide_compile_counts(const struct ide_compile_job* job, int* errors, int* warnings, double* seconds)
{
    *errors = job->report.error_count;
    *warnings = job->report.warnings_count;
    *seconds = job->report.cpu_time_used_sec;
}

static void append(struct ide_compile_job* job, const char* data, size_t n)
{
    struct ide_text* t = &job->text;
    if (t->len + n + 1 > COMPILE_MAX_OUTPUT)
        n = t->len + 1 < COMPILE_MAX_OUTPUT ? COMPILE_MAX_OUTPUT - t->len - 1 : 0;
    if (n == 0)
        return;
    if (t->len + n + 1 > t->cap)
    {
        size_t cap = t->cap ? t->cap * 2 : 65536;
        while (cap < t->len + n + 1)
            cap *= 2;
        char* p = realloc(t->data, cap);
        if (!p)
            return;
        t->data = p;
        t->cap = cap;
    }
    memcpy(t->data + t->len, data, n);
    t->len += n;
    t->data[t->len] = '\0';
}

/* The worker: only the compile. It touches no gui_* state. */
static int worker(void* param)
{
    struct ide_compile_job* job = param;
    compile(job->argc, job->argv, &job->report);
    fflush(stdout);
    job->finished = 1;
    return 0;
}

int ide_compile_start(struct ide_compile_job* job)
{
    if (job->running)
        return 0;
    job->finished = 0;
    memset(&job->report, 0, sizeof job->report);
    if (job->listener.included)
    {
        job->include_listener.callback = on_included;
        job->include_listener.file_done = on_done;
        job->include_listener.data = job;
        job->report.include_listener = &job->include_listener;
    }

    fflush(stdout);
#ifdef _WIN32
    if (!freopen("NUL", "w", stdout))
        return 0;
    job->saved_stdout = _dup(_fileno(stdout));
    SECURITY_ATTRIBUTES sa = { sizeof sa, NULL, TRUE };
    HANDLE hwrite = 0;
    if (!CreatePipe(&job->hread, &hwrite, &sa, 1 << 20))
        return 0;
    int wfd = _open_osfhandle((intptr_t)hwrite, _O_WRONLY | _O_TEXT);
    if (wfd == -1)
    {
        CloseHandle(job->hread);
        CloseHandle(hwrite);
        return 0;
    }
    _dup2(wfd, _fileno(stdout));
    _close(wfd);   /* closes hwrite too: stdout keeps its own duplicate */
#else
    if (!freopen("/dev/null", "w", stdout))
        return 0;
    job->saved_stdout = dup(fileno(stdout));
    int fds[2] = { 0 };
    if (pipe(fds) != 0)
        return 0;
    job->fdread = fds[0];
    fcntl(job->fdread, F_SETFL, O_NONBLOCK);
    dup2(fds[1], fileno(stdout));
    close(fds[1]);
#endif
    setvbuf(stdout, NULL, _IONBF, 0);

    job->running = 1;
    if (thrd_create(&job->thread, worker, job) != thrd_success)
    {
        job->running = 0;
        return 0;
    }
    return 1;
}

/* Whatever is in the pipe now, without blocking. */
static size_t drain(struct ide_compile_job* job)
{
    char buf[8192] = { 0 };
    size_t total = 0;
#ifdef _WIN32
    for (;;)
    {
        DWORD avail = 0;
        if (!job->hread || !PeekNamedPipe(job->hread, NULL, 0, NULL, &avail, NULL) || avail == 0)
            break;
        DWORD got = 0;
        if (!ReadFile(job->hread, buf, avail > sizeof buf ? (DWORD)sizeof buf : avail, &got, NULL) || got == 0)
            break;
        append(job, buf, got);
        total += got;
    }
#else
    for (;;)
    {
        ssize_t got = read(job->fdread, buf, sizeof buf);
        if (got <= 0)
            break;
        append(job, buf, (size_t)got);
        total += (size_t)got;
    }
#endif
    return total;
}

static void finish(struct ide_compile_job* job)
{
    thrd_join(job->thread, NULL);
#ifdef _WIN32
    if (job->hread)
    {
        CloseHandle(job->hread);
        job->hread = NULL;
    }
    fflush(stdout);
    if (job->saved_stdout >= 0)
    {
        _dup2(job->saved_stdout, _fileno(stdout));
        _close(job->saved_stdout);
    }
#else
    close(job->fdread);
    fflush(stdout);
    if (job->saved_stdout >= 0)
    {
        dup2(job->saved_stdout, fileno(stdout));
        close(job->saved_stdout);
    }
#endif
    job->saved_stdout = -1;
    job->running = 0;
}

int ide_compile_poll(struct ide_compile_job* job, int* new_output)
{
    *new_output = 0;
    if (!job->running)
        return 0;
    size_t got = drain(job);
    *new_output = got > 0;
    if (job->finished && got == 0)
    {
        *new_output |= drain(job) > 0;   /* the last bytes */
        finish(job);
        return 1;
    }
    return 0;
}
