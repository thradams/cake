/* ide_services.c - the IDE's access to files and folders. Windows only
 * for now (the IDE is Windows-first until it is stable); the other
 * platforms come with their backends.
 */
#include "ide_shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <strings.h>
#define _stricmp strcasecmp
#endif

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#ifdef _MSC_VER
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#endif
#else
#include <dirent.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>   /* _NSGetExecutablePath */
#endif
#endif

static int compare_entries(const void* a, const void* b)
{
    const struct ide_dir_entry* x = a;
    const struct ide_dir_entry* y = b;
    if (x->is_dir != y->is_dir)
        return y->is_dir - x->is_dir;   /* folders first */
    return _stricmp(x->name, y->name);
}

int ide_list_dir(const char* dir, struct ide_dir_entry* out, int max)
{
#ifdef _WIN32
    wchar_t pattern[1024] = { 0 };
    char utf8[1024] = { 0 };
    snprintf(utf8, sizeof utf8, "%s\\*", dir);
    if (!MultiByteToWideChar(CP_UTF8, 0, utf8, -1, pattern, 1024))
        return -1;
    WIN32_FIND_DATAW fd = { 0 };
    HANDLE h = FindFirstFileW(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE)
        return -1;
    int count = 0;
    do
    {
        if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0)
            continue;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
            continue;
        struct ide_dir_entry* e = &out[count];
        WideCharToMultiByte(CP_UTF8, 0, fd.cFileName, -1, e->name, (int)sizeof e->name, NULL, NULL);
        e->is_dir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        count++;
    } while (count < max && FindNextFileW(h, &fd));
    FindClose(h);
    qsort(out, (size_t)count, sizeof *out, compare_entries);
    return count;
#else
    DIR* d = opendir(dir);
    if (!d)
        return -1;
    int count = 0;
    struct dirent* de = 0;
    while (count < max && (de = readdir(d)) != NULL)
    {
        if (de->d_name[0] == '.')
            continue;   /* ".", ".." and hidden files */
        struct ide_dir_entry* e = &out[count];
        snprintf(e->name, sizeof e->name, "%s", de->d_name);
        char full[2048] = { 0 };
        snprintf(full, sizeof full, "%s/%s", dir, de->d_name);
        struct stat st = { 0 };
        e->is_dir = stat(full, &st) == 0 && S_ISDIR(st.st_mode);
        count++;
    }
    closedir(d);
    qsort(out, (size_t)count, sizeof *out, compare_entries);
    return count;
#endif
}

char* ide_read_file(const char* path, int* crlf)
{
    FILE* f = fopen(path, "rb");
    if (!f)
        return NULL;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* text = malloc((size_t)size + 1);
    if (!text)
    {
        fclose(f);
        return NULL;
    }
    size_t got = fread(text, 1, (size_t)size, f);
    fclose(f);
    text[got] = '\0';
    *crlf = 0;
    char* o = text;
    for (const char* p = text; *p; p++)
    {
        if (*p == '\r' && p[1] == '\n')
        {
            *crlf = 1;
            continue;
        }
        *o++ = *p;
    }
    *o = '\0';
    return text;
}

int ide_write_file(const char* path, const char* text, int crlf)
{
    FILE* f = fopen(path, "wb");
    if (!f)
        return -1;
    int ok = 1;
    for (const char* p = text; *p && ok; p++)
    {
        if (*p == '\n' && crlf)
            ok = fputc('\r', f) != EOF;
        if (ok)
            ok = fputc(*p, f) != EOF;
    }
    if (fclose(f) != 0)
        ok = 0;
    return ok ? 0 : -1;
}

int ide_is_dir(const char* path)
{
#ifdef _WIN32
    wchar_t w[1024] = { 0 };
    if (!MultiByteToWideChar(CP_UTF8, 0, path, -1, w, 1024))
        return 0;
    DWORD attributes = GetFileAttributesW(w);
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY);
#else
    struct stat st = { 0 };
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
#endif
}

int ide_file_exists(const char* path)
{
    FILE* f = fopen(path, "rb");
    if (f)
        fclose(f);
    return f != NULL;
}

int ide_make_dir(const char* path)
{
#ifdef _WIN32
    wchar_t w[1024] = { 0 };
    if (!MultiByteToWideChar(CP_UTF8, 0, path, -1, w, 1024))
        return -1;
    return CreateDirectoryW(w, NULL) ? 0 : -1;
#else
    return mkdir(path, 0777);
#endif
}

int ide_delete_path(const char* path, int is_dir)
{
#ifdef _WIN32
    wchar_t w[1024] = { 0 };
    if (!MultiByteToWideChar(CP_UTF8, 0, path, -1, w, 1024))
        return -1;
    return (is_dir ? RemoveDirectoryW(w) : DeleteFileW(w)) ? 0 : -1;
#else
    return is_dir ? rmdir(path) : unlink(path);
#endif
}

struct ide_process
{
#ifdef _WIN32
    HANDLE process;
    HANDLE read_end;
    HANDLE write_in;   /* its stdin; NULL once closed */
#else
    pid_t pid;
    int fd;     /* stdout and stderr */
    int in_fd;  /* its stdin; -1 once closed */
#endif
    int unused;
};

struct ide_process* ide_process_start(const char* command_line, const char* dir)
{
#ifdef _WIN32
    size_t n = strlen(command_line) + 1;
    wchar_t* wcmd = malloc(n * sizeof(wchar_t));
    wchar_t wdir[1024] = { 0 };
    if (!wcmd || !MultiByteToWideChar(CP_UTF8, 0, command_line, -1, wcmd, (int)n) ||
        (dir && !MultiByteToWideChar(CP_UTF8, 0, dir, -1, wdir, 1024)))
    {
        free(wcmd);
        return NULL;
    }
    SECURITY_ATTRIBUTES sa = { sizeof sa, NULL, TRUE };
    HANDLE read_end = 0, write_end = 0;
    if (!CreatePipe(&read_end, &write_end, &sa, 1 << 16))
    {
        free(wcmd);
        return NULL;
    }
    SetHandleInformation(read_end, HANDLE_FLAG_INHERIT, 0);
    HANDLE read_in = NULL, write_in = NULL;
    if (!CreatePipe(&read_in, &write_in, &sa, 1 << 16))
    {
        CloseHandle(read_end);
        CloseHandle(write_end);
        free(wcmd);
        return NULL;
    }
    SetHandleInformation(write_in, HANDLE_FLAG_INHERIT, 0);
    STARTUPINFOW si = { sizeof si };
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = write_end;
    si.hStdError = write_end;
    si.hStdInput = read_in;
    PROCESS_INFORMATION pi = { 0 };
    BOOL started = CreateProcessW(NULL, wcmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL,
                                  dir ? wdir : NULL, &si, &pi);
    CloseHandle(write_end);
    CloseHandle(read_in);
    free(wcmd);
    if (!started)
    {
        CloseHandle(read_end);
        CloseHandle(write_in);
        return NULL;
    }
    CloseHandle(pi.hThread);
    struct ide_process* p = calloc(1, sizeof *p);
    if (!p)
    {
        TerminateProcess(pi.hProcess, 1);
        CloseHandle(pi.hProcess);
        CloseHandle(read_end);
        CloseHandle(write_in);
        return NULL;
    }
    p->process = pi.hProcess;
    p->read_end = read_end;
    p->write_in = write_in;
    return p;
#else
    int fds[2] = { 0 }, in[2] = { 0 };
    if (pipe(fds) != 0)
        return NULL;
    if (pipe(in) != 0)
    {
        close(fds[0]);
        close(fds[1]);
        return NULL;
    }
    pid_t pid = fork();
    if (pid < 0)
    {
        close(fds[0]);
        close(fds[1]);
        close(in[0]);
        close(in[1]);
        return NULL;
    }
    if (pid == 0)
    {
        /* the command line is quoted like a Windows one: sh reads it the same way */
        dup2(fds[1], 1);
        dup2(fds[1], 2);
        dup2(in[0], 0);
        setpgid(0, 0);   /* its own group: Stop kills what sh started too */
        close(fds[0]);
        close(fds[1]);
        close(in[0]);
        close(in[1]);
        if (dir && dir[0] && chdir(dir) != 0)
            _exit(127);
        execl("/bin/sh", "sh", "-c", command_line, (char*)NULL);
        _exit(127);
    }
    close(fds[1]);
    close(in[0]);
    fcntl(fds[0], F_SETFD, FD_CLOEXEC);
    fcntl(in[1], F_SETFD, FD_CLOEXEC);
    signal(SIGPIPE, SIG_IGN);   /* a write after it ended fails, not kills the IDE */
    struct ide_process* p = calloc(1, sizeof *p);
    if (!p)
    {
        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0);
        close(fds[0]);
        close(in[1]);
        return NULL;
    }
    p->pid = pid;
    p->fd = fds[0];
    p->in_fd = in[1];
    return p;
#endif
}

int ide_process_read(struct ide_process* p, char* buf, int cap)
{
#ifdef _WIN32
    DWORD avail = 0;
    if (!PeekNamedPipe(p->read_end, NULL, 0, NULL, &avail, NULL))
        return -1;   /* broken: every writer is gone */
    if (avail == 0)
    {
        if (WaitForSingleObject(p->process, 0) != WAIT_OBJECT_0)
            return 0;
        /* ended: one last look, its last bytes may have come just now */
        if (!PeekNamedPipe(p->read_end, NULL, 0, NULL, &avail, NULL) || avail == 0)
            return -1;
    }
    DWORD got = 0;
    if (!ReadFile(p->read_end, buf, avail < (DWORD)cap ? avail : (DWORD)cap, &got, NULL))
        return -1;
    return (int)got;
#else
    struct pollfd pfd = { p->fd, POLLIN, 0 };
    if (poll(&pfd, 1, 0) <= 0)
        return 0;
    ssize_t got = read(p->fd, buf, (size_t)cap);
    return got > 0 ? (int)got : -1;   /* 0: every writer is gone */
#endif
}

int ide_process_close(struct ide_process* p)
{
    int code = 0;
#ifdef _WIN32
    DWORD exit_code = 0;
    WaitForSingleObject(p->process, INFINITE);
    if (GetExitCodeProcess(p->process, &exit_code))
        code = (int)exit_code;
    CloseHandle(p->process);
    CloseHandle(p->read_end);
    if (p->write_in)
        CloseHandle(p->write_in);
#else
    int status = 0;
    close(p->fd);
    if (p->in_fd >= 0)
        close(p->in_fd);
    if (waitpid(p->pid, &status, 0) == p->pid)
        code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
#endif
    free(p);
    return code;
}

int ide_process_write(struct ide_process* p, const char* data, int len)
{
#ifdef _WIN32
    DWORD written = 0;
    if (!p->write_in || !WriteFile(p->write_in, data, (DWORD)len, &written, NULL))
        return -1;
    return (int)written;
#else
    if (p->in_fd < 0)
        return -1;
    ssize_t n = write(p->in_fd, data, (size_t)len);
    return n >= 0 ? (int)n : -1;
#endif
}

void ide_process_close_stdin(struct ide_process* p)
{
#ifdef _WIN32
    if (p->write_in)
        CloseHandle(p->write_in);
    p->write_in = NULL;
#else
    if (p->in_fd >= 0)
        close(p->in_fd);
    p->in_fd = -1;
#endif
}

void ide_process_kill(struct ide_process* p)
{
#ifdef _WIN32
    TerminateProcess(p->process, 1);
#else
    kill(-p->pid, SIGKILL);   /* sh and what it started */
    kill(p->pid, SIGKILL);
#endif
}

char* ide_run_capture(const char* command_line, const char* dir)
{
#ifdef _WIN32
    wchar_t wcmd[2048] = { 0 }, wdir[1024] = { 0 };
    if (!MultiByteToWideChar(CP_UTF8, 0, command_line, -1, wcmd, 2048))
        return NULL;
    if (dir && !MultiByteToWideChar(CP_UTF8, 0, dir, -1, wdir, 1024))
        return NULL;
    SECURITY_ATTRIBUTES sa = { sizeof sa, NULL, TRUE };
    HANDLE read_end = 0, write_end = 0;
    if (!CreatePipe(&read_end, &write_end, &sa, 0))
        return NULL;
    SetHandleInformation(read_end, HANDLE_FLAG_INHERIT, 0);
    STARTUPINFOW si = { sizeof si };
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = write_end;
    si.hStdError = write_end;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    PROCESS_INFORMATION pi = { 0 };
    BOOL started = CreateProcessW(NULL, wcmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL,
                                  dir ? wdir : NULL, &si, &pi);
    CloseHandle(write_end);
    if (!started)
    {
        CloseHandle(read_end);
        return NULL;
    }
    size_t len = 0, cap = 4096;
    char* out = malloc(cap);
    for (;;)
    {
        char chunk[2048] = { 0 };
        DWORD got = 0;
        if (!ReadFile(read_end, chunk, sizeof chunk, &got, NULL) || got == 0)
            break;
        if (out && cap - len <= got)
        {
            char* bigger = realloc(out, cap * 2 + got);
            if (!bigger)
                free(out);
            out = bigger;
            cap = cap * 2 + got;
        }
        if (out)
        {
            memcpy(out + len, chunk, got);
            len += got;
        }
    }
    CloseHandle(read_end);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    if (out)
        out[len] = '\0';
    return out;
#else
    struct ide_process* p = ide_process_start(command_line, dir);
    if (!p)
        return NULL;
    ide_process_close_stdin(p);   /* nothing to type into it */
    size_t len = 0, cap = 4096;
    char* out = malloc(cap + 1);
    for (;;)
    {
        struct pollfd pfd = { p->fd, POLLIN, 0 };
        poll(&pfd, 1, -1);
        char chunk[4096] = { 0 };
        int got = ide_process_read(p, chunk, sizeof chunk);
        if (got < 0)
            break;
        if (out && len + (size_t)got > cap)
        {
            char* bigger = realloc(out, cap * 2 + (size_t)got + 1);
            if (!bigger)
                free(out);
            out = bigger;
            cap = cap * 2 + (size_t)got;
        }
        if (out)
        {
            memcpy(out + len, chunk, (size_t)got);
            len += (size_t)got;
        }
    }
    ide_process_close(p);
    if (out)
        out[len] = '\0';
    return out;
#endif
}

static void add_problem(char* problems, int cap, const char* line)
{
    size_t n = strlen(problems);
    snprintf(problems + n, (size_t)cap - n, "%s\n", line);
}

int ide_output_has(const char* command_line, const char* signature)
{
    char* out = ide_run_capture(command_line, NULL);
    int found = out && strstr(out, signature) != NULL;
    free(out);
    return found;
}

int ide_find_tcc(char* command, int cap)
{
    if (ide_output_has("tcc -v", "tcc version"))
    {
        snprintf(command, (size_t)cap, "tcc");
        return 1;
    }
#ifdef _WIN32
    char dirs[4][MAX_PATH + 16] = { "C:\\tcc", "C:\\tcc\\win32", { 0 }, { 0 } };
    char pf[MAX_PATH] = { 0 };
    DWORD n = GetEnvironmentVariableA("ProgramFiles", pf, sizeof pf);
    if (n > 0 && n < sizeof pf)
        snprintf(dirs[2], sizeof dirs[2], "%s\\tcc", pf);
    char self[1024] = { 0 };
    ide_exe_dir(self, sizeof self);
    snprintf(dirs[3], sizeof dirs[3], "%s\\tcc", self);
    for (int i = 0; i < 4; i++)
    {
        char exe[MAX_PATH + 32] = { 0 };
        snprintf(exe, sizeof exe, "%s\\tcc.exe", dirs[i]);
        if (GetFileAttributesA(exe) != INVALID_FILE_ATTRIBUTES)
        {
            snprintf(command, (size_t)cap, "%s", exe);
            return 1;
        }
    }
#endif
    return 0;
}

int ide_detect_tcc_include_dirs(void (*add)(void* ctx, const char* dir), void* ctx,
                                 char* problems, int cap)
{
    char tcc[1024] = { 0 };
    if (!ide_find_tcc(tcc, sizeof tcc))
    {
        add_problem(problems, cap, "tcc was not found (PATH, C:\\tcc, Program Files\\tcc).");
        return 0;
    }
    char cmd[1200] = { 0 };
    snprintf(cmd, sizeof cmd, "\"%s\" -print-search-dirs", tcc);
    char* out = ide_run_capture(cmd, NULL);
    int count = 0, in_include = 0;
    for (const char* p = out ? out : ""; *p;)
    {
        const char* eol = strchr(p, '\n');
        size_t len = eol ? (size_t)(eol - p) : strlen(p);
        char line[512] = { 0 };
        snprintf(line, sizeof line, "%.*s", (int)len, p);
        p += len + (eol ? 1 : 0);
        line[strcspn(line, "\r")] = '\0';
        if (line[0] != ' ')
        {
            in_include = strcmp(line, "include:") == 0;
            continue;
        }
        char* dir = line;
        while (*dir == ' ')
            dir++;
        if (in_include && dir[0])
        {
            add(ctx, dir);
            count++;
        }
    }
    if (count == 0)
    {
        size_t used = strlen(problems);
        snprintf(problems + used, (size_t)cap - used, "No include section in the output of\n  %s\n\n%s", cmd, out ? out : "");
    }
    free(out);
    return count;
}

int ide_detect_cc_include_dirs(const char* compiler, void (*add)(void* ctx, const char* dir), void* ctx,
                                char* problems, int cap)
{
    char cmd[256] = { 0 };
    /* LC_ALL=C: gcc translates the "search starts here" lines */
    snprintf(cmd, sizeof cmd, "LC_ALL=C %s -v -E -x c /dev/null", compiler);
    char* out = ide_run_capture(cmd, NULL);
    int count = 0, in_include = 0;
    for (const char* p = out ? out : ""; *p;)
    {
        const char* eol = strchr(p, '\n');
        size_t len = eol ? (size_t)(eol - p) : strlen(p);
        char line[512] = { 0 };
        snprintf(line, sizeof line, "%.*s", (int)len, p);
        p += len + (eol ? 1 : 0);
        line[strcspn(line, "\r")] = '\0';
        if (strstr(line, "#include <...> search starts here:"))
        {
            in_include = 1;
            continue;
        }
        if (!in_include)
            continue;
        if (strstr(line, "End of search list."))
            break;
        char* dir = line;
        while (*dir == ' ')
            dir++;
        /* clang labels macOS framework paths; the label is not part of the path */
        char* tag = strstr(dir, " (framework directory)");
        if (tag)
            *tag = '\0';
        if (dir[0])
        {
            add(ctx, dir);
            count++;
        }
    }
    if (count == 0)
    {
        size_t n = strlen(problems);
        snprintf(problems + n, (size_t)cap - n, "No include search list in the output of\n  %s\n\n%s", cmd, out ? out : "");
    }
    free(out);
    return count;
}

void ide_detect_include_dirs(void (*add)(void* ctx, const char* dir), void* ctx,
                              char* problems, int cap, struct ide_msvc_toolchain* toolchain)
{
    problems[0] = '\0';
    if (toolchain)
        memset(toolchain, 0, sizeof *toolchain);
#ifdef _WIN32
    /* MSVC: vswhere.exe sits at a fixed place (Visual Studio 2017 and later). */
    char pf86[MAX_PATH] = { 0 };
    DWORD n = GetEnvironmentVariableA("ProgramFiles(x86)", pf86, sizeof pf86);
    if (n > 0 && n < sizeof pf86)
    {
        char cmd[1024] = { 0 };
        snprintf(cmd, sizeof cmd,
                 "\"%s\\Microsoft Visual Studio\\Installer\\vswhere.exe\" -latest -products * "
                 "-requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath",
                 pf86);
        char* out = ide_run_capture(cmd, NULL);
        char vs_dir[512] = { 0 };
        snprintf(vs_dir, sizeof vs_dir, "%s", out ? out : "");
        free(out);
        vs_dir[strcspn(vs_dir, "\r\n")] = '\0';
        char version_file[700] = { 0 };
        snprintf(version_file, sizeof version_file,
                 "%s\\VC\\Auxiliary\\Build\\Microsoft.VCToolsVersion.default.txt", vs_dir);
        int crlf = 0;
        char* version = vs_dir[0] ? ide_read_file(version_file, &crlf) : NULL;
        if (version)
        {
            version[strcspn(version, " \t\r\n")] = '\0';
            if (toolchain)
            {
                snprintf(toolchain->vs_dir, sizeof toolchain->vs_dir, "%s", vs_dir);
                snprintf(toolchain->version, sizeof toolchain->version, "%s", version);
            }
            char dir[1024] = { 0 };
            snprintf(dir, sizeof dir, "%s\\VC\\Tools\\MSVC\\%s\\include", vs_dir, version);
            add(ctx, dir);
            free(version);
        }
        else
        {
            add_problem(problems, cap, "Visual Studio with the C/C++ toolset was not found (vswhere.exe).");
        }
    }
    else
    {
        add_problem(problems, cap, "ProgramFiles(x86) is not set - cannot locate vswhere.exe.");
    }

    /* Windows SDK: KitsRoot10, and the newest version whose headers exist. */
    HKEY key = 0;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots",
                      0, KEY_READ | KEY_WOW64_32KEY, &key) != ERROR_SUCCESS)
    {
        add_problem(problems, cap, "The Windows SDK is not in the registry.");
        return;
    }
    char root[MAX_PATH] = { 0 };
    DWORD size = sizeof root - 1;
    DWORD type = 0;
    char best[64] = { 0 };
    unsigned best_v[4] = { 0 };
    if (RegQueryValueExA(key, "KitsRoot10", NULL, &type, (BYTE*)root, &size) == ERROR_SUCCESS &&
        type == REG_SZ && root[0])
    {
        size_t len = strlen(root);
        if (root[len - 1] != '\\' && len + 1 < sizeof root)
            strcat(root, "\\");
        for (DWORD i = 0;; i++)
        {
            char name[64] = { 0 };
            DWORD name_len = sizeof name;
            if (RegEnumKeyExA(key, i, name, &name_len, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)
                break;
            unsigned v[4] = { 0 };
            if (sscanf(name, "%u.%u.%u.%u", &v[0], &v[1], &v[2], &v[3]) != 4)
                continue;
            char probe[MAX_PATH + 100] = { 0 };
            snprintf(probe, sizeof probe, "%sInclude\\%s\\ucrt", root, name);
            if (GetFileAttributesA(probe) == INVALID_FILE_ATTRIBUTES)
                continue;   /* the key can outlive an uninstalled SDK */
            int newer = 0;
            for (int k = 0; k < 4; k++)
            {
                if (v[k] != best_v[k])
                {
                    newer = v[k] > best_v[k];
                    break;
                }
            }
            if (newer)
            {
                snprintf(best, sizeof best, "%s", name);
                memcpy(best_v, v, sizeof v);
            }
        }
    }
    RegCloseKey(key);
    if (best[0] && toolchain)
    {
        snprintf(toolchain->sdk_root, sizeof toolchain->sdk_root, "%s", root);
        snprintf(toolchain->sdk_version, sizeof toolchain->sdk_version, "%s", best);
    }
    if (!best[0])
    {
        add_problem(problems, cap, "No Windows 10/11 SDK headers were found (KitsRoot10 in the registry).");
        return;
    }
    /* Same subfolders, in the same order, as a Visual Studio prompt's INCLUDE. */
    static const char* const subdirs[] = { "ucrt", "um", "shared", "winrt", "cppwinrt" };
    for (int i = 0; i < (int)(sizeof subdirs / sizeof subdirs[0]); i++)
    {
        char dir[512] = { 0 };
        snprintf(dir, sizeof dir, "%sInclude\\%s\\%s", root, best, subdirs[i]);
        if (GetFileAttributesA(dir) != INVALID_FILE_ATTRIBUTES)
            add(ctx, dir);
    }
#else
    /* The platform compiler's own "-v -E" output, between
     * "#include <...> search starts here:" and "End of search list." (ide.c's). */
#ifdef __APPLE__
    const char* cmd = "echo | clang -v -E - 2>&1";
#else
    const char* cmd = "echo | gcc -v -E - 2>&1";
#endif
    char* out = ide_run_capture(cmd, NULL);
    int count = 0, in_include = 0;
    for (const char* p = out ? out : ""; *p;)
    {
        const char* eol = strchr(p, '\n');
        size_t len = eol ? (size_t)(eol - p) : strlen(p);
        char line[512] = { 0 };
        snprintf(line, sizeof line, "%.*s", (int)len, p);
        p += len + (eol ? 1 : 0);
        line[strcspn(line, "\r")] = '\0';
        if (strstr(line, "#include <...> search starts here:"))
        {
            in_include = 1;
            continue;
        }
        if (!in_include)
            continue;
        if (strstr(line, "End of search list."))
            break;
        char* dir = line;
        while (*dir == ' ')
            dir++;
        /* clang marks macOS framework paths - not part of the path */
        char* tag = strstr(dir, " (framework directory)");
        if (tag)
            *tag = '\0';
        if (dir[0])
        {
            add(ctx, dir);
            count++;
        }
    }
    if (count == 0)
    {
        char msg[1500] = { 0 };
        snprintf(msg, sizeof msg, "No include search list in the output of\n  %s\n\n%s", cmd, out ? out : "");
        add_problem(problems, cap, msg);
    }
    free(out);
#endif
}

int ide_config_dir(char* buf, int cap)
{
#ifdef _WIN32
    const char* appdata = getenv("APPDATA");
    if (!appdata || !appdata[0])
        return 0;
    snprintf(buf, (size_t)cap, "%s\\cake_ide", appdata);
    ide_make_dir(buf);   /* fails when it exists - fine */
    return 1;
#else
    const char* home = getenv("HOME");
    if (!home || !home[0])
        return 0;
    snprintf(buf, (size_t)cap, "%s/.cake_ide", home);
    ide_make_dir(buf);   /* fails when it exists - fine */
    return 1;
#endif
}

void ide_exe_dir(char* buf, int cap)
{
#ifdef _WIN32
    wchar_t w[1024] = { 0 };
    DWORD n = GetModuleFileNameW(NULL, w, 1024);
    if (n > 0 && n < 1024)
    {
        wchar_t* slash = wcsrchr(w, L'\\');
        if (slash)
            *slash = L'\0';
        if (WideCharToMultiByte(CP_UTF8, 0, w, -1, buf, cap, NULL, NULL))
            return;
    }
#elif defined(__APPLE__)
    char path[PATH_MAX] = { 0 }, real[PATH_MAX] = { 0 };
    uint32_t size = sizeof path;
    if (_NSGetExecutablePath(path, &size) == 0 && realpath(path, real))
    {
        char* slash = strrchr(real, '/');
        if (slash)
            *slash = '\0';
        snprintf(buf, (size_t)cap, "%s", real);
        return;
    }
#else
    char path[PATH_MAX] = { 0 };
    ssize_t n = readlink("/proc/self/exe", path, sizeof path - 1);
    if (n > 0)
    {
        path[n] = '\0';
        char* slash = strrchr(path, '/');
        if (slash)
            *slash = '\0';
        snprintf(buf, (size_t)cap, "%s", path);
        return;
    }
#endif
    snprintf(buf, (size_t)cap, ".");
}

void ide_open_url(const char* url)
{
#ifdef _WIN32
    wchar_t w[1024] = { 0 };
    if (MultiByteToWideChar(CP_UTF8, 0, url, -1, w, 1024))
        ShellExecuteW(NULL, L"open", w, NULL, NULL, SW_SHOWNORMAL);
#else
    /* detached: a grandchild of init, never waited for */
    pid_t pid = fork();
    if (pid == 0)
    {
        if (fork() == 0)
        {
#ifdef __APPLE__
            execlp("open", "open", url, (char*)NULL);
#else
            execlp("xdg-open", "xdg-open", url, (char*)NULL);
#endif
            _exit(127);
        }
        _exit(0);
    }
    if (pid > 0)
        waitpid(pid, NULL, 0);
#endif
}

/* A terminal window of the system's in `dir` - the old IDE's ui_open_terminal. */
void ide_open_terminal(const char* dir)
{
#ifdef _WIN32
    wchar_t wdir[MAX_PATH] = { 0 };
    if (dir && dir[0])
        MultiByteToWideChar(CP_UTF8, 0, dir, -1, wdir, MAX_PATH);
    wchar_t shell[MAX_PATH] = { 0 };
    DWORD n = GetEnvironmentVariableW(L"COMSPEC", shell, MAX_PATH);
    if (n == 0 || n >= MAX_PATH)
        wcscpy(shell, L"cmd.exe");
    STARTUPINFOW si = { sizeof si };
    PROCESS_INFORMATION pi = { 0 };
    if (CreateProcessW(NULL, shell, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, wdir[0] ? wdir : NULL, &si, &pi))
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
#else
    pid_t pid = fork();
    if (pid == 0)
    {
        if (fork() == 0)
        {
#ifdef __APPLE__
            if (dir && dir[0])
                execlp("open", "open", "-a", "Terminal", dir, (char*)NULL);
            else
                execlp("open", "open", "-a", "Terminal", (char*)NULL);
#else
            if (dir && dir[0] && chdir(dir) != 0)
                _exit(127);
            execlp("x-terminal-emulator", "x-terminal-emulator", (char*)NULL);
            execlp("gnome-terminal", "gnome-terminal", (char*)NULL);
            execlp("konsole", "konsole", (char*)NULL);
            execlp("xfce4-terminal", "xfce4-terminal", (char*)NULL);
            execlp("xterm", "xterm", (char*)NULL);
#endif
            _exit(127);
        }
        _exit(0);
    }
    if (pid > 0)
        waitpid(pid, NULL, 0);
#endif
}

void ide_full_path(const char* path, char* buf, int cap)
{
#ifdef _WIN32
    wchar_t w[1024] = { 0 }, full[1024] = { 0 };
    if (MultiByteToWideChar(CP_UTF8, 0, path, -1, w, 1024) &&
        GetFullPathNameW(w, 1024, full, NULL) > 0 &&
        WideCharToMultiByte(CP_UTF8, 0, full, -1, buf, cap, NULL, NULL))
        return;
#else
    char full[PATH_MAX] = { 0 };
    if (realpath(path, full))
    {
        snprintf(buf, (size_t)cap, "%s", full);
        return;
    }
#endif
    snprintf(buf, (size_t)cap, "%s", path);
}

void ide_current_dir(char* buf, int cap)
{
#ifdef _WIN32
    wchar_t wdir[1024] = { 0 };
    if (GetCurrentDirectoryW(1024, wdir) &&
        WideCharToMultiByte(CP_UTF8, 0, wdir, -1, buf, cap, NULL, NULL))
        return;
#else
    if (getcwd(buf, (size_t)cap))
        return;
#endif
    snprintf(buf, (size_t)cap, ".");
}
