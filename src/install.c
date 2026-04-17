/*
 * install.c  –  Cross-platform installer (Windows & Linux)
 *
 * Compile on Windows:
 *   MSVC  :  cl install.c
 *   MinGW :  gcc install.c -o install.exe          (x64)
 *            gcc -m32 install.c -o install32.exe   (x86)
 *
 * Compile on Linux:
 *   gcc install.c -o install
 *
 * Requires Administrator / root to write to the system install path
 * and to update the system PATH.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "version.h"
 /* ------------------------------------------------------------------ */
 /*  Platform detection & OS-specific includes                          */
 /* ------------------------------------------------------------------ */

#ifdef _WIN32
#include <windows.h>
#pragma comment(lib, "user32.lib")   /* SendMessageTimeoutA */
#pragma comment(lib, "advapi32.lib") /* Registry API        */
#define PATH_SEP         '\\'
#define PATH_SEP_STR     "\\"
#define PATH_MAX_LEN     MAX_PATH
#else
    /* Linux / macOS */
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <glob.h>
#include <limits.h>
#include <libgen.h>
#define PATH_SEP         '/'
#define PATH_SEP_STR     "/"
#define PATH_MAX_LEN     PATH_MAX
#endif

/* termios for raw single-keypress on Linux/macOS */
#ifndef _WIN32
#include <termios.h>
#endif

/* ------------------------------------------------------------------ */
/*  Install configuration                                               */
/* ------------------------------------------------------------------ */

#define APP_DIR_NAME  "cake"

/*
 * Install entries: { "source", "dest_subfolder", recursive, exec_bit }
 *
 *  - source       : file pattern (wildcards * ?) OR a directory name.
 *  - dest_subdir  : subfolder inside the install dir, or "" for root.
 *  - recursive    : 0 = wildcard file copy  |  1 = full recursive dir copy
 *  - exec_bit     : (Linux only) 1 = chmod +x every file copied from this
 *                   entry.  Use for binaries, .sh scripts, etc.
 *                   On Windows this field is ignored.
 *
 * When recursive=1 the entire source directory tree is mirrored under
 * dest_subdir, creating all sub-directories automatically.
 * When recursive=1 AND exec_bit=1, chmod +x is applied to every file
 * in the tree (useful for a bin/ directory full of executables).
 *
 * Windows examples:
 *   { "*.exe",       "",        0, 0 }   wildcard, no exec needed on Windows
 *   { "plugins",     "plugins", 1, 0 }   full tree
 *
 * Linux examples:
 *   { "myapp",       "",        0, 1 }   single binary -> chmod +x
 *   { "*.sh",        "",        0, 1 }   scripts       -> chmod +x
 *   { "*.so",        "",        0, 0 }   libraries, no exec bit needed
 *   { "plugins",     "plugins", 1, 0 }   full tree, no exec
 *   { "bin",         "bin",     1, 1 }   full bin/ tree -> chmod +x all
 */
typedef struct {
    const char* source;
    const char* dest_subdir;
    int         recursive;  /* 0 = wildcard pattern, 1 = recursive dir  */
    int         exec_bit;   /* Linux only: 1 = chmod +x after copy      */
} InstallEntry;


static const InstallEntry INSTALL_ENTRIES[] = {
#ifdef _WIN32
    { "cake.exe",          "",        0 , 0},

#else
    { "cake",           "",      0 , 1},
#endif
    { "cakeconf.h",        "",   0 , 0},
    { "web",        "web", 1 , 0},   /* recursive: copies entire plugins\ tree */
};
#define INSTALL_ENTRIES_COUNT  (sizeof(INSTALL_ENTRIES) / sizeof(INSTALL_ENTRIES[0]))

/* ------------------------------------------------------------------ */
/*  Common helpers                                                      */
/* ------------------------------------------------------------------ */

static int ask_yes_no(const char* question)
{
    char buf[8];
    for (;;)
    {
        printf("%s [y/n]: ", question);
        fflush(stdout);
        if (!fgets(buf, sizeof buf, stdin)) return 0;
        if (buf[0] == 'y' || buf[0] == 'Y') return 1;
        if (buf[0] == 'n' || buf[0] == 'N') return 0;
        printf("  Please enter y or n.\n");
    }
}

static void print_separator(void)
{
    printf("------------------------------------------------------------\n");
}

/* ------------------------------------------------------------------ */
/*  ensure_directory_exists()  –  mkdir -p for both platforms          */
/* ------------------------------------------------------------------ */

static int ensure_directory_exists(const char* path)
{
    char  tmp[PATH_MAX_LEN];
    char* p;

#ifdef _WIN32
    DWORD attrs = GetFileAttributesA(path);
    if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY))
        return 1;
#else
    struct stat st;
    if (stat(path, &st) == 0 && S_ISDIR(st.st_mode))
        return 1;
#endif

    strncpy(tmp, path, sizeof tmp - 1);
    tmp[sizeof tmp - 1] = '\0';

    for (p = tmp + 1; *p; p++)
    {
        if (*p == '\\' || *p == '/')
        {
            *p = '\0';
#ifdef _WIN32
            CreateDirectoryA(tmp, NULL);
#else
            mkdir(tmp, 0755);
#endif
            * p = PATH_SEP;
        }
    }

#ifdef _WIN32
    if (!CreateDirectoryA(tmp, NULL))
    {
        DWORD err = GetLastError();
        if (err != ERROR_ALREADY_EXISTS)
        {
            fprintf(stderr, "  Error: cannot create directory \"%s\" (code %lu).\n",
                    path, err);
            return 0;
        }
    }
#else
    if (mkdir(tmp, 0755) != 0 && errno != EEXIST)
    {
        fprintf(stderr, "  Error: cannot create directory \"%s\": %s\n",
                path, strerror(errno));
        return 0;
    }
#endif
    return 1;
}

/* ------------------------------------------------------------------ */
/*  copy_file()  –  copies a single file src -> dest                   */
/* ------------------------------------------------------------------ */

static int copy_file(const char* src, const char* dest, int exec_bit)
{
#ifdef _WIN32
    (void)exec_bit;   /* not used on Windows */
    if (!CopyFileA(src, dest, FALSE))
    {
        fprintf(stderr, "  [ERROR]   %s  (code %lu)\n", src, GetLastError());
        return 0;
    }
    return 1;
#else
    FILE* fsrc, * fdst;
    char   buf[65536];
    size_t n;
    int    ok = 1;
    struct stat src_st;

    fsrc = fopen(src, "rb");
    if (!fsrc)
    {
        fprintf(stderr, "  [ERROR]   %s  (%s)\n", src, strerror(errno));
        return 0;
    }
    fdst = fopen(dest, "wb");
    if (!fdst)
    {
        fprintf(stderr, "  [ERROR]   %s -> %s  (%s)\n", src, dest, strerror(errno));
        fclose(fsrc);
        return 0;
    }
    while ((n = fread(buf, 1, sizeof buf, fsrc)) > 0)
    {
        if (fwrite(buf, 1, n, fdst) != n) { ok = 0; break; }
    }
    fclose(fsrc);
    fclose(fdst);

    if (!ok)
    {
        fprintf(stderr, "  [ERROR]   write failed: %s\n", dest);
        unlink(dest);
        return 0;
    }

    /* Preserve source permissions, then apply exec_bit if requested */
    if (stat(src, &src_st) == 0)
    {
        mode_t mode = src_st.st_mode & 0777;
        if (exec_bit)
            mode |= S_IXUSR | S_IXGRP | S_IXOTH;  /* chmod +x */
        chmod(dest, mode);
    }
    else if (exec_bit)
    {
        chmod(dest, 0755);  /* fallback: rwxr-xr-x */
    }
    else
    {
        chmod(dest, 0644);  /* fallback: rw-r--r-- */
    }

    if (exec_bit)
        printf("  [CHMOD+X] %s\n", dest);

    return 1;
#endif
}

/* ------------------------------------------------------------------ */
/*  copy_dir_recursive()                                                 */
/*  Mirrors an entire directory tree src_dir -> dest_dir.               */
/*  dest_dir and all sub-directories are created as needed.             */
/* ------------------------------------------------------------------ */

static int copy_dir_recursive(const char* src_dir, const char* dest_dir, int exec_bit)
{
    int errors = 0;

    if (!ensure_directory_exists(dest_dir))
    {
        fprintf(stderr, "  [ERROR] Cannot create dir: %s\n", dest_dir);
        return 1;
    }

#ifdef _WIN32
    {
        char             pattern[MAX_PATH];
        WIN32_FIND_DATAA fd;
        HANDLE           hfind;

        snprintf(pattern, sizeof pattern, "%s\\*", src_dir);
        hfind = FindFirstFileA(pattern, &fd);
        if (hfind == INVALID_HANDLE_VALUE) return 0;

        do
        {
            char src_child[MAX_PATH];
            char dst_child[MAX_PATH];

            if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0)
                continue;

            snprintf(src_child, sizeof src_child, "%s\\%s", src_dir, fd.cFileName);
            snprintf(dst_child, sizeof dst_child, "%s\\%s", dest_dir, fd.cFileName);

            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                /* Recurse into sub-directory */
                printf("  [DIR]     %s\n", src_child);
                errors += copy_dir_recursive(src_child, dst_child, exec_bit);
            }
            else
            {
                if (!copy_file(src_child, dst_child, exec_bit))
                    errors++;
                else
                    printf("  [OK]      %s\n         -> %s\n", src_child, dst_child);
            }
        } while (FindNextFileA(hfind, &fd));
        FindClose(hfind);
    }
#else
    {
        DIR* d = opendir(src_dir);
        struct dirent* entry;

        if (!d)
        {
            fprintf(stderr, "  [ERROR] Cannot open dir: %s (%s)\n",
                    src_dir, strerror(errno));
            return 1;
        }

        while ((entry = readdir(d)) != NULL)
        {
            char src_child[PATH_MAX];
            char dst_child[PATH_MAX];
            struct stat st;

            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            snprintf(src_child, sizeof src_child, "%s/%s", src_dir, entry->d_name);
            snprintf(dst_child, sizeof dst_child, "%s/%s", dest_dir, entry->d_name);

            if (stat(src_child, &st) != 0) continue;

            if (S_ISDIR(st.st_mode))
            {
                printf("  [DIR]     %s\n", src_child);
                errors += copy_dir_recursive(src_child, dst_child, exec_bit);
            }
            else
            {
                if (!copy_file(src_child, dst_child, exec_bit))
                    errors++;
                else
                    printf("  [OK]      %s\n         -> %s\n", src_child, dst_child);
            }
        }
        closedir(d);
    }
#endif

    return errors;
}

/*  copy_with_wildcard()                                                */
/*  Mirrors an entire directory tree src_dir -> dest_dir.               */
/*  dest_dir and all sub-directories are created as needed.             */
/* ------------------------------------------------------------------ */

static int copy_with_wildcard(const char* src_pattern, const char* dest_dir, int exec_bit)
{
    char src_file[PATH_MAX_LEN];
    char dest_file[PATH_MAX_LEN];
    char src_dir[PATH_MAX_LEN];
    const char* last_sep;
    int  errors = 0;
    int  found_any = 0;

    /* Derive source directory from pattern */
    last_sep = strrchr(src_pattern, '\\');
    if (!last_sep) last_sep = strrchr(src_pattern, '/');
    if (last_sep)
    {
        size_t dlen = (size_t)(last_sep - src_pattern);
        strncpy(src_dir, src_pattern, dlen);
        src_dir[dlen] = '\0';
    }
    else
    {
        src_dir[0] = '.';
        src_dir[1] = '\0';
    }

    /* Ensure destination exists */
    if (!ensure_directory_exists(dest_dir))
    {
        fprintf(stderr, "  [ERROR] Could not create dest dir: %s\n", dest_dir);
        return 1;
    }

#ifdef _WIN32
    /* ---- Windows: FindFirstFile / FindNextFile ------------------- */
    {
        WIN32_FIND_DATAA fd;
        HANDLE hfind = FindFirstFileA(src_pattern, &fd);

        if (hfind == INVALID_HANDLE_VALUE)
        {
            printf("  [NOMATCH] %s\n", src_pattern);
            return 0;
        }
        do
        {
            if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0)
                continue;
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                continue;

            found_any = 1;
            snprintf(src_file, sizeof src_file, "%s\\%s", src_dir, fd.cFileName);
            snprintf(dest_file, sizeof dest_file, "%s\\%s", dest_dir, fd.cFileName);

            if (!copy_file(src_file, dest_file, 0))
                errors++;
            else
                printf("  [OK]      %s\n         -> %s\n", src_file, dest_file);

        } while (FindNextFileA(hfind, &fd));
        FindClose(hfind);
    }
#else
    /* ---- Linux: glob() ------------------------------------------ */
    {
        glob_t gl;
        int    rc = glob(src_pattern, GLOB_TILDE, NULL, &gl);

        if (rc == GLOB_NOMATCH)
        {
            printf("  [NOMATCH] %s\n", src_pattern);
            return 0;
        }
        if (rc != 0)
        {
            fprintf(stderr, "  [ERROR] glob failed for: %s\n", src_pattern);
            return 1;
        }

        for (size_t i = 0; i < gl.gl_pathc; i++)
        {
            const char* match = gl.gl_pathv[i];
            struct stat st;

            if (stat(match, &st) != 0 || S_ISDIR(st.st_mode))
                continue;   /* skip directories */

            found_any = 1;

            /* Filename only (basename) */
            char match_copy[PATH_MAX_LEN];
            strncpy(match_copy, match, sizeof match_copy - 1);
            const char* fname = basename(match_copy);

            snprintf(dest_file, sizeof dest_file, "%s/%s", dest_dir, fname);

            if (!copy_file(match, dest_file, 0))
                errors++;
            else
                printf("  [OK]      %s\n         -> %s\n", match, dest_file);
        }
        globfree(&gl);
    }
#endif

    if (!found_any)
        printf("  [EMPTY]   %s  (pattern matched no files)\n", src_pattern);

    return errors;
}

/* ------------------------------------------------------------------ */
/*  clean_directory()  –  rm -rf contents (keeps the dir itself)       */
/* ------------------------------------------------------------------ */

static int clean_directory(const char* dir)
{
    int errors = 0;

#ifdef _WIN32
    char             pattern[MAX_PATH];
    WIN32_FIND_DATAA fd;
    HANDLE           hfind;

    snprintf(pattern, sizeof pattern, "%s\\*", dir);
    hfind = FindFirstFileA(pattern, &fd);
    if (hfind == INVALID_HANDLE_VALUE) return 0;

    do
    {
        char child[MAX_PATH];
        if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0)
            continue;

        snprintf(child, sizeof child, "%s\\%s", dir, fd.cFileName);

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            errors += clean_directory(child);
            if (!RemoveDirectoryA(child))
            {
                fprintf(stderr, "  [ERROR] Cannot remove dir : %s (code %lu)\n",
                        child, GetLastError());
                errors++;
            }
            else
            {
                printf("  [DEL-DIR]  %s\n", child);
            }
        }
        else
        {
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
                SetFileAttributesA(child,
                    fd.dwFileAttributes & ~FILE_ATTRIBUTE_READONLY);
            if (!DeleteFileA(child))
            {
                fprintf(stderr, "  [ERROR] Cannot delete: %s (code %lu)\n",
                        child, GetLastError());
                errors++;
            }
            else
            {
                printf("  [DEL-FILE] %s\n", child);
            }
        }
    } while (FindNextFileA(hfind, &fd));
    FindClose(hfind);

#else
    /* Linux: opendir / readdir, recurse */
    DIR* d = opendir(dir);
    struct dirent* entry;

    if (!d) return 0;

    while ((entry = readdir(d)) != NULL)
    {
        char child[PATH_MAX];
        struct stat st;

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(child, sizeof child, "%s/%s", dir, entry->d_name);

        if (stat(child, &st) != 0) continue;

        if (S_ISDIR(st.st_mode))
        {
            errors += clean_directory(child);
            if (rmdir(child) != 0)
            {
                fprintf(stderr, "  [ERROR] Cannot remove dir : %s (%s)\n",
                        child, strerror(errno));
                errors++;
            }
            else
            {
                printf("  [DEL-DIR]  %s\n", child);
            }
        }
        else
        {
            if (unlink(child) != 0)
            {
                fprintf(stderr, "  [ERROR] Cannot delete: %s (%s)\n",
                        child, strerror(errno));
                errors++;
            }
            else
            {
                printf("  [DEL-FILE] %s\n", child);
            }
        }
    }
    closedir(d);
#endif

    return errors;
}

/* ------------------------------------------------------------------ */
/*  add_to_system_path()                                                */
/*  Windows: writes HKLM registry + broadcasts WM_SETTINGCHANGE        */
/*  Linux:   appends export PATH=... to /etc/profile.d/myapp.sh        */
/* ------------------------------------------------------------------ */

#ifdef _WIN32

/*
 * is_app_path_entry()
 *   Returns 1 if 'token' looks like a previous install of this app,
 *   i.e. it ends with \APP_DIR_NAME regardless of which Program Files
 *   variant (x64 or x86) it came from.
 */
static int is_app_path_entry(const char* token)
{
    /* Match both:
     *   C:\Program Files\cake
     *   C:\Program Files (x86)\cake
     * by checking that the last component equals APP_DIR_NAME.
     */
    size_t tlen = strlen(token);
    size_t alen = strlen(APP_DIR_NAME);

    if (tlen < alen) return 0;

    /* Check suffix is \APP_DIR_NAME */
    if (_strnicmp(token + tlen - alen, APP_DIR_NAME, alen) != 0)
        return 0;

    /* The character before it must be a path separator (or nothing) */
    if (tlen > alen && token[tlen - alen - 1] != '\\')
        return 0;

    return 1;
}

/*
 * rebuild_path_without_app()
 *   Iterates every semicolon-delimited token in 'src'.
 *   Tokens matching old app installs are skipped (removed).
 *   The current target_dir is also skipped here so we can
 *   re-append it cleanly at the end.
 *   Returns the number of stale entries removed.
 */
static int rebuild_path_without_app(const char* src,
                                    char* dst,
                                    size_t      dst_size,
                                    const char* target_dir)
{
    const char* p = src;
    int         removed = 0;
    dst[0] = '\0';

    while (*p)
    {
        char        token[MAX_PATH];
        const char* end = strchr(p, ';');
        size_t      seg_len = end ? (size_t)(end - p) : strlen(p);

        if (seg_len == 0) { p = end ? end + 1 : p + seg_len; continue; }

        strncpy(token, p, seg_len);
        token[seg_len] = '\0';

        /* Drop stale app entries AND the current target (re-added later).
         * The current target is silently stripped here and re-appended at
         * the end – no message, because it is not being removed. */
        if (is_app_path_entry(token))
        {
            if (_strnicmp(token, target_dir, MAX_PATH) != 0)
            {
                printf("  [REMOVED] %s\n", token);
                removed++;   /* only count genuinely stale entries */
            }
            /* current target: silently drop here, re-added below */
        }
        else
        {
            /* Keep this entry */
            if (dst[0] != '\0') strncat(dst, ";", dst_size - strlen(dst) - 1);
            strncat(dst, token, dst_size - strlen(dst) - 1);
        }

        p = end ? end + 1 : p + seg_len;
        if (!end) break;
    }
    return removed;
}

static void add_to_system_path(const char* target_dir)
{
    HKEY  hkey;
    DWORD type, data_size = 0;
    char* old_path = NULL;
    char* new_path = NULL;
    int   removed = 0;
    int   needs_add = 1;

    const char* REG_ENV =
        "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment";

    print_separator();
    printf("  Checking system PATH (registry)...\n");
    print_separator();

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, REG_ENV, 0, KEY_READ, &hkey) != ERROR_SUCCESS)
    {
        fprintf(stderr, "  Warning: could not open registry key. PATH not modified.\n\n");
        return;
    }
    RegQueryValueExA(hkey, "Path", NULL, &type, NULL, &data_size);

    /* Allocate generously: old + room for new entry */
    old_path = (char*)malloc(data_size + 2);
    new_path = (char*)malloc(data_size + MAX_PATH + 4);
    if (!old_path || !new_path)
    {
        free(old_path); free(new_path);
        RegCloseKey(hkey); return;
    }

    if (RegQueryValueExA(hkey, "Path", NULL, &type,
        (LPBYTE)old_path, &data_size) != ERROR_SUCCESS)
    {
        free(old_path); free(new_path);
        RegCloseKey(hkey); return;
    }
    RegCloseKey(hkey);

    /* ---- Scan: show what will be removed / kept ------------------- */
    printf("  Scanning for stale %s entries...\n\n", APP_DIR_NAME);

    /* Check if current target is already present BEFORE rebuilding */
    {
        const char* p = old_path;
        size_t tlen = strlen(target_dir);
        while (*p)
        {
            const char* end = strchr(p, ';');
            size_t seg_len = end ? (size_t)(end - p) : strlen(p);
            if (seg_len == tlen && _strnicmp(p, target_dir, tlen) == 0)
            {
                needs_add = 0; break;
            }
            if (!end) break;
            p = end + 1;
        }
    }

    removed = rebuild_path_without_app(old_path, new_path,
                                       data_size + MAX_PATH + 4,
                                       target_dir);

    if (removed > 0)
        printf("\n  %d stale PATH entry(s) will be removed.\n", removed);
    else
        printf("  No stale entries found.\n");

    if (!needs_add && removed == 0)
    {
        printf("  PATH already contains the correct entry:\n    %s\n"
               "  No changes needed.\n\n", target_dir);
        free(old_path); free(new_path); return;
    }

    /* ---- Confirm with user --------------------------------------- */
    printf("\n");
    if (removed > 0 && needs_add)
        printf("  Will remove stale entry(s) and add:\n    %s\n\n", target_dir);
    else if (removed > 0)
        printf("  Will remove stale entry(s) only.\n\n");
    else
        printf("  Will add:\n    %s\n\n", target_dir);

    if (!ask_yes_no("Apply PATH changes?"))
    {
        printf("  PATH not modified.\n\n");
        free(old_path); free(new_path); return;
    }

    /* ---- Append current target_dir ------------------------------ */
    if (needs_add)
    {
        size_t l = strlen(new_path);
        if (l > 0 && new_path[l - 1] != ';') { new_path[l] = ';'; new_path[l + 1] = '\0'; }
        strcat(new_path, target_dir);
    }

    /* ---- Write back to registry --------------------------------- */
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, REG_ENV, 0,
        KEY_SET_VALUE, &hkey) != ERROR_SUCCESS)
    {
        fprintf(stderr, "  Error: cannot write registry. Run as Administrator.\n\n");
        free(old_path); free(new_path); return;
    }
    if (RegSetValueExA(hkey, "Path", 0, type,
        (const BYTE*)new_path,
        (DWORD)(strlen(new_path) + 1)) != ERROR_SUCCESS)
        fprintf(stderr, "  Error: RegSetValueEx failed (code %lu).\n\n", GetLastError());
    else
    {
        printf("  PATH updated successfully.\n\n");
        SendMessageTimeoutA(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                            (LPARAM)"Environment", SMTO_ABORTIFHUNG, 1000, NULL);
    }
    RegCloseKey(hkey);
    free(old_path);
    free(new_path);
}

#else  /* Linux */

/*
 * is_app_path_entry_linux()
 *   Returns 1 if the token ends with /APP_DIR_NAME,
 *   covering any prefix (/usr/local, /opt, custom INSTALL_PREFIX).
 */
static int is_app_path_entry_linux(const char* token)
{
    size_t tlen = strlen(token);
    size_t alen = strlen(APP_DIR_NAME);
    if (tlen < alen) return 0;
    if (strcmp(token + tlen - alen, APP_DIR_NAME) != 0) return 0;
    if (tlen > alen && token[tlen - alen - 1] != '/') return 0;
    return 1;
}

static void add_to_system_path(const char* target_dir)
{
    const char* profile_file = "/etc/profile.d/cake.sh";
    FILE* fp;
    char        existing[4096] = { 0 };
    int         stale_found = 0;

    print_separator();
    printf("  Checking system PATH (/etc/profile.d)...\n");
    print_separator();

    /* ---- Read existing profile file if present ------------------- */
    fp = fopen(profile_file, "r");
    if (fp)
    {
        char line[512];
        printf("  Existing entries in %s:\n", profile_file);
        while (fgets(line, sizeof line, fp))
        {
            /* Look for lines like: export PATH="/old/path:$PATH" */
            char* start = strstr(line, "export PATH=\"");
            if (start)
            {
                start += 13; /* skip: export PATH=" */
                char* colon = strchr(start, ':');
                if (colon)
                {
                    char entry[PATH_MAX];
                    size_t elen = (size_t)(colon - start);
                    strncpy(entry, start, elen);
                    entry[elen] = '\0';
                    if (is_app_path_entry_linux(entry))
                    {
                        if (strcmp(entry, target_dir) == 0)
                            printf("  [CURRENT] %s\n", entry);
                        else
                        {
                            printf("  [STALE]   %s  (will be replaced)\n", entry);
                            stale_found = 1;
                        }
                    }
                }
            }
        }
        fclose(fp);
        printf("\n");
    }
    else
    {
        printf("  No existing profile file found.\n\n");
    }

    if (!stale_found)
    {
        /* Check if already correct */
        const char* sys_path = getenv("PATH");
        if (sys_path)
        {
            char buf[4096]; char* tok;
            strncpy(buf, sys_path, sizeof buf - 1);
            tok = strtok(buf, ":");
            while (tok)
            {
                if (strcmp(tok, target_dir) == 0)
                {
                    printf("  PATH already contains: %s\n"
                           "  No changes needed.\n\n", target_dir);
                    return;
                }
                tok = strtok(NULL, ":");
            }
        }
    }

    printf("  Target : %s\n\n", target_dir);
    if (!ask_yes_no(stale_found
        ? "Replace stale entry and update PATH?"
        : "Add install directory to system PATH?"))
    {
        printf("  PATH not modified.\n\n");
        return;
    }

    /* ---- (Re)write the profile file with only the current entry -- */
    fp = fopen(profile_file, "w");
    if (!fp)
    {
        fprintf(stderr, "  Error: cannot write %s: %s\n"
                        "  Try running as root (sudo).\n\n",
                profile_file, strerror(errno));
        return;
    }
    fprintf(fp, "# Added by Cake installer\n");
    fprintf(fp, "export PATH=\"%s:$PATH\"\n", target_dir);
    fclose(fp);
    chmod(profile_file, 0644);

    if (stale_found)
        printf("  Stale entry replaced.\n");
    printf("  Created: %s\n"
           "  PATH will be updated for new login shells.\n\n", profile_file);
}

#endif  /* _WIN32 */

/* ------------------------------------------------------------------ */
/*  main                                                                */
/* ------------------------------------------------------------------ */

int main(void)
{
    char  target_dir[PATH_MAX_LEN];
    int   dir_exists;

    printf("\n");
    print_separator();
    printf("  Cake " CAKE_VERSION " Installer\n");
    print_separator();
    printf("\n");

    /* ---- 1. Resolve install directory ----------------------------- */
#ifdef _WIN32
    {
        char prog_files[MAX_PATH];

#if defined(_WIN64)
#define PROGFILES_VAR  "ProgramFiles"
#define PROGFILES_FB   "C:\\Program Files"
#define ARCH_LABEL     "x64"
#else
#define PROGFILES_VAR  "ProgramFiles(x86)"
#define PROGFILES_FB   "C:\\Program Files (x86)"
#define ARCH_LABEL     "x86"
#endif

        if (!GetEnvironmentVariableA(PROGFILES_VAR, prog_files, sizeof prog_files))
            strncpy(prog_files, PROGFILES_FB, sizeof prog_files - 1);

        printf("  Build architecture : %s\n", ARCH_LABEL);
        printf("  Program Files path : %s\n\n", prog_files);
        snprintf(target_dir, sizeof target_dir, "%s\\%s", prog_files, APP_DIR_NAME);
    }
#else
    /*
     * Linux default: /usr/local/<AppName>
     * Override by setting INSTALL_PREFIX, e.g.:
     *   INSTALL_PREFIX=/opt ./install
     */
    {
        const char* prefix = getenv("INSTALL_PREFIX");
        if (!prefix) prefix = "/usr/local";
        snprintf(target_dir, sizeof target_dir, "%s/%s", prefix, APP_DIR_NAME);
    }
#endif

    printf("  Target directory   : %s\n\n", target_dir);

    /* ---- 2. Check / create root install directory ----------------- */
    {
#ifdef _WIN32
        DWORD attrs = GetFileAttributesA(target_dir);
        dir_exists = (attrs != INVALID_FILE_ATTRIBUTES) &&
            (attrs & FILE_ATTRIBUTE_DIRECTORY);
#else
        struct stat st;
        dir_exists = (stat(target_dir, &st) == 0 && S_ISDIR(st.st_mode));
#endif
    }

    if (!dir_exists)
    {
        printf("The directory does not exist.\n");
        if (!ask_yes_no("Create it now?"))
        {
            printf("\nInstallation cancelled.\n");
            return 0;
        }
        if (!ensure_directory_exists(target_dir))
        {
#ifdef _WIN32
            fprintf(stderr, "\nTry running as Administrator.\n");
#else
            fprintf(stderr, "\nTry running as root (sudo).\n");
#endif
            return 1;
        }
        printf("Directory created successfully.\n\n");
    }
    else
    {
        printf("The directory already exists.\n");
        if (!ask_yes_no("Override / reinstall?"))
        {
            printf("\nInstallation cancelled.\n");
            return 0;
        }

        /* Clean existing contents before copying fresh files */
        print_separator();
        printf("  Cleaning existing directory...\n");
        print_separator();
        {
            int clean_errors = clean_directory(target_dir);
            if (clean_errors > 0)
                fprintf(stderr, "  Warning: %d item(s) could not be removed.\n", clean_errors);
            else
                printf("  Directory cleaned successfully.\n");
        }
        printf("\n");
    }

    /* ---- 3. Copy entries (wildcard loop) -------------------------- */
    print_separator();
    printf("  Copying files (%zu pattern(s))...\n", INSTALL_ENTRIES_COUNT);
    print_separator();

    {
        size_t i;
        int    total_errors = 0;

        for (i = 0; i < INSTALL_ENTRIES_COUNT; i++)
        {
            const char* source = INSTALL_ENTRIES[i].source;
            const char* subdir = INSTALL_ENTRIES[i].dest_subdir;
            int         recursive = INSTALL_ENTRIES[i].recursive;
            int         exec_bit = INSTALL_ENTRIES[i].exec_bit;
            char        dest_dir[PATH_MAX_LEN];

            if (subdir[0] != '\0')
                snprintf(dest_dir, sizeof dest_dir,
                         "%s%c%s", target_dir, PATH_SEP, subdir);
            else
                strncpy(dest_dir, target_dir, sizeof dest_dir - 1);

            if (recursive)
            {
                printf("\n  [RECURSIVE] %s  ->  %s\n", source, dest_dir);
                total_errors += copy_dir_recursive(source, dest_dir, exec_bit);
            }
            else
            {
                printf("\n  Pattern : %s  ->  %s\n", source, dest_dir);
                total_errors += copy_with_wildcard(source, dest_dir, exec_bit);
            }
        }

        printf("\n");
        if (total_errors == 0)
            printf("  All files copied successfully.\n\n");
        else
            fprintf(stderr, "  Warning: %d file(s) could not be copied.\n\n",
                    total_errors);
    }

    /* ---- 4. Check / update system PATH ---------------------------- */
    add_to_system_path(target_dir);

    /* ---- 5. Done -------------------------------------------------- */
    print_separator();
    printf("  Installation complete!\n");
    print_separator();
    printf("\n");
    printf("  Press any key to exit...\n");
    fflush(stdout);
#ifdef _WIN32
    /* On Windows, read a raw keypress without needing Enter */
    {
        HANDLE hin = GetStdHandle(STD_INPUT_HANDLE);
        DWORD  mode;
        GetConsoleMode(hin, &mode);
        SetConsoleMode(hin, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
        {
            char  ch;
            DWORD read;
            ReadConsoleA(hin, &ch, 1, &read, NULL);
        }
        SetConsoleMode(hin, mode);   /* restore original mode */
    }
#else
    /* On Linux, switch terminal to raw mode for single-keypress */
    {
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  /* restore */
    }
#endif
    printf("\n");

    return 0;
}
