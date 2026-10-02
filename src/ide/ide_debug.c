/*
 * The child-process plumbing ide_debugger.c (lldb / cdb) talks through -
 * ui_process_start_direct/_read/_write/_close, as the old IDE's backends
 * (the old IDE's backends) had them, so the IDE drives the
 * same debugger session code.
 */
#include "ide_debugger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>

struct ui_process
{
    HANDLE hread;
    HANDLE hwrite_in;
    HANDLE hproc;
    int ended;
};

static void append_quoted_arg(WCHAR* wcmd, size_t cap, const char* arg)
{
    WCHAR warg[2048] = { 0 };
    MultiByteToWideChar(CP_UTF8, 0, arg, -1, warg, 2048);
    int quotes = warg[0] == 0;
    for (WCHAR* q = warg; *q && !quotes; q++)
        quotes = *q == L' ' || *q == L'\t' || *q == L'"';
    size_t len = wcslen(wcmd);
    if (len > 0 && len < cap - 1)
    {
        wcmd[len++] = L' ';
        wcmd[len] = 0;
    }
    if (!quotes)
    {
        wcsncat(wcmd, warg, cap - wcslen(wcmd) - 1);
        return;
    }
    wcsncat(wcmd, L"\"", cap - wcslen(wcmd) - 1);
    for (WCHAR* q = warg; *q; q++)
    {
        WCHAR one[2] = { *q, 0 };
        wcsncat(wcmd, *q == L'"' ? L"\\\"" : one, cap - wcslen(wcmd) - 1);
    }
    wcsncat(wcmd, L"\"", cap - wcslen(wcmd) - 1);
}

ui_process* ui_process_start_direct(const char* const argv[], const char* dir, char* err, int errcap)
{
    if (err && errcap > 0)
        err[0] = 0;
    WCHAR wcmd[4096] = { 0 };
    for (int i = 0; argv[i]; i++)
        append_quoted_arg(wcmd, 4096, argv[i]);
    WCHAR wdir[1024];
    if (dir && dir[0] && !MultiByteToWideChar(CP_UTF8, 0, dir, -1, wdir, 1024))
        dir = NULL;

    SECURITY_ATTRIBUTES sa = { sizeof sa, NULL, TRUE };
    HANDLE hread, hwrite, hread_in, hwrite_in;
    if (!CreatePipe(&hread, &hwrite, &sa, 1 << 20))
    {
        snprintf(err, (size_t)errcap, "CreatePipe failed");
        return NULL;
    }
    SetHandleInformation(hread, HANDLE_FLAG_INHERIT, 0);
    if (!CreatePipe(&hread_in, &hwrite_in, &sa, 1 << 16))
    {
        snprintf(err, (size_t)errcap, "CreatePipe failed");
        CloseHandle(hread);
        CloseHandle(hwrite);
        return NULL;
    }
    SetHandleInformation(hwrite_in, HANDLE_FLAG_INHERIT, 0);
    STARTUPINFOW si = { sizeof si };
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = hwrite;
    si.hStdError = hwrite;
    si.hStdInput = hread_in;
    PROCESS_INFORMATION pi = { 0 };
    BOOL ok = CreateProcessW(NULL, wcmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL,
                             dir && dir[0] ? wdir : NULL, &si, &pi);
    CloseHandle(hwrite);
    CloseHandle(hread_in);
    if (!ok)
    {
        snprintf(err, (size_t)errcap, "CreateProcess failed (error %lu)", (unsigned long)GetLastError());
        CloseHandle(hread);
        CloseHandle(hwrite_in);
        return NULL;
    }
    CloseHandle(pi.hThread);
    ui_process* p = calloc(1, sizeof *p);
    if (!p)
    {
        TerminateProcess(pi.hProcess, 1);
        CloseHandle(pi.hProcess);
        CloseHandle(hread);
        CloseHandle(hwrite_in);
        return NULL;
    }
    p->hread = hread;
    p->hwrite_in = hwrite_in;
    p->hproc = pi.hProcess;
    return p;
}

int ui_process_read(ui_process* p, char* buf, int cap)
{
    if (!p || p->ended)
        return -1;
    DWORD avail = 0;
    if (PeekNamedPipe(p->hread, NULL, 0, NULL, &avail, NULL) && avail > 0)
    {
        DWORD got = 0;
        if (ReadFile(p->hread, buf, avail > (DWORD)cap ? (DWORD)cap : avail, &got, NULL) && got > 0)
            return (int)got;
    }
    if (WaitForSingleObject(p->hproc, 0) == WAIT_OBJECT_0)
    {
        avail = 0;
        if (!PeekNamedPipe(p->hread, NULL, 0, NULL, &avail, NULL) || avail == 0)
        {
            p->ended = 1;
            return -1;
        }
    }
    return 0;
}

int ui_process_write(ui_process* p, const char* data, int len)
{
    DWORD written = 0;
    if (!p || !p->hwrite_in || !WriteFile(p->hwrite_in, data, (DWORD)len, &written, NULL))
        return -1;
    return (int)written;
}

int ui_process_close(ui_process* p)
{
    if (!p)
        return -1;
    DWORD code = 0;
    /* a busy cdb never reads its "q": killing it also ends the program it debugs */
    if (WaitForSingleObject(p->hproc, 2000) == WAIT_TIMEOUT)
        TerminateProcess(p->hproc, 1);
    WaitForSingleObject(p->hproc, INFINITE);
    if (!GetExitCodeProcess(p->hproc, &code))
        code = (DWORD)-1;
    CloseHandle(p->hproc);
    CloseHandle(p->hread);
    if (p->hwrite_in)
        CloseHandle(p->hwrite_in);
    free(p);
    return (int)code;
}

#else
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

struct ui_process
{
    int fd;      /* stdout and stderr */
    int in_fd;   /* its stdin */
    pid_t pid;
    int ended;
};

ui_process* ui_process_start_direct(const char* const argv[], const char* dir, char* err, int errcap)
{
    if (err && errcap > 0)
        err[0] = 0;
    int fds[2], infds[2];
    if (pipe(fds) != 0)
    {
        snprintf(err, (size_t)errcap, "pipe failed: %s", strerror(errno));
        return NULL;
    }
    if (pipe(infds) != 0)
    {
        snprintf(err, (size_t)errcap, "pipe failed: %s", strerror(errno));
        close(fds[0]);
        close(fds[1]);
        return NULL;
    }
    pid_t pid = fork();
    if (pid < 0)
    {
        snprintf(err, (size_t)errcap, "fork failed: %s", strerror(errno));
        close(fds[0]);
        close(fds[1]);
        close(infds[0]);
        close(infds[1]);
        return NULL;
    }
    if (pid == 0)
    {
        close(fds[0]);
        dup2(fds[1], STDOUT_FILENO);
        dup2(fds[1], STDERR_FILENO);
        close(fds[1]);
        dup2(infds[0], STDIN_FILENO);
        close(infds[0]);
        close(infds[1]);
        if (dir && dir[0] && chdir(dir) != 0)
        {
            fprintf(stderr, "cannot change directory to '%s': %s\n", dir, strerror(errno));
            _exit(127);
        }
        execvp(argv[0], (char* const*)argv);
        fprintf(stderr, "cannot run '%s': %s\n", argv[0], strerror(errno));
        _exit(127);
    }
    close(fds[1]);
    close(infds[0]);
    fcntl(fds[0], F_SETFL, O_NONBLOCK);
    fcntl(infds[1], F_SETFL, O_NONBLOCK);
    fcntl(fds[0], F_SETFD, FD_CLOEXEC);
    fcntl(infds[1], F_SETFD, FD_CLOEXEC);
    ui_process* p = calloc(1, sizeof *p);
    if (!p)
    {
        close(fds[0]);
        close(infds[1]);
        return NULL;
    }
    p->fd = fds[0];
    p->in_fd = infds[1];
    p->pid = pid;
    return p;
}

int ui_process_read(ui_process* p, char* buf, int cap)
{
    if (!p || p->ended)
        return -1;
    ssize_t n = read(p->fd, buf, (size_t)cap);
    if (n > 0)
        return (int)n;
    if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return 0;
    p->ended = 1;
    return -1;
}

int ui_process_write(ui_process* p, const char* data, int len)
{
    if (!p || p->in_fd < 0)
        return -1;
    ssize_t n = write(p->in_fd, data, (size_t)len);
    if (n >= 0)
        return (int)n;
    return errno == EAGAIN || errno == EWOULDBLOCK ? 0 : -1;
}

int ui_process_close(ui_process* p)
{
    if (!p)
        return -1;
    int status = 0;
    close(p->fd);
    if (p->in_fd >= 0)
        close(p->in_fd);
    if (waitpid(p->pid, &status, 0) < 0)
        status = -1;
    free(p);
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}
#endif
