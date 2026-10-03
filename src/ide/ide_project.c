/* ide_project.c - the open project's data, as the old IDE keeps it
 * (g_project in ide.c): its .cakeproj path, name, files and include
 * directories (stored relative to the project folder), and what the last
 * successful Builds compiled, for the incremental Build. Reading and writing
 * the .cakeproj is in ide_shell.c, next to the other settings.
 */
#include "ide_shell.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#endif

/* --- String lists --- */

int ide_strings_add(struct ide_strings* l, const char* s)
{
    if (l->count == l->cap)
    {
        int cap = l->cap ? l->cap * 2 : 16;
        char** items = realloc(l->items, (size_t)cap * sizeof *items);
        if (!items)
            return 0;
        l->items = items;
        l->cap = cap;
    }
    size_t n = strlen(s);
    char* copy = malloc(n + 1);
    if (!copy)
        return 0;
    memcpy(copy, s, n + 1);
    l->items[l->count++] = copy;
    return 1;
}

void ide_strings_remove_at(struct ide_strings* l, int index)
{
    if (index < 0 || index >= l->count)
        return;
    free(l->items[index]);
    memmove(&l->items[index], &l->items[index + 1], sizeof l->items[0] * (size_t)(l->count - index - 1));
    l->count--;
}

void ide_strings_clear(struct ide_strings* l)
{
    for (int i = 0; i < l->count; i++)
        free(l->items[i]);
    l->count = 0;
}

void ide_strings_destroy(struct ide_strings* l)
{
    ide_strings_clear(l);
    free(l->items);
    l->items = NULL;
    l->cap = 0;
}

/* --- Paths --- */

/* Case-insensitive, '/' and '\' alike - Windows paths. */
int ide_path_equal(const char* a, const char* b)
{
    for (; *a && *b; a++, b++)
    {
        char ca = *a == '\\' ? '/' : (char)tolower((unsigned char)*a);
        char cb = *b == '\\' ? '/' : (char)tolower((unsigned char)*b);
        if (ca != cb)
            return 0;
    }
    return *a == *b;
}

static int is_absolute(const char* p)
{
    return p[0] == '/' || p[0] == '\\' || (isalpha((unsigned char)p[0]) && p[1] == ':');
}

long long ide_file_time(const char* path)
{
#ifdef _WIN32
    wchar_t w[1024];
    WIN32_FILE_ATTRIBUTE_DATA data;
    if (!MultiByteToWideChar(CP_UTF8, 0, path, -1, w, 1024) ||
        !GetFileAttributesExW(w, GetFileExInfoStandard, &data))
        return 0;
    return ((long long)data.ftLastWriteTime.dwHighDateTime << 32) | data.ftLastWriteTime.dwLowDateTime;
#else
    struct stat st;
    return stat(path, &st) == 0 ? (long long)st.st_mtime : 0;
#endif
}

/* --- The project --- */

int ide_project_is_open(const struct ide_project* p)
{
    return p->file_path[0] != '\0';
}

/* `abs_path` relative to the project folder when it is inside it, with '/'
 * separators; anything else stays absolute. */
void ide_project_relative(const struct ide_project* p, const char* abs_path, char* out, size_t cap)
{
    char norm[1024];
    snprintf(norm, sizeof norm, "%s", abs_path);
    for (char* c = norm; *c; c++)
    {
        if (*c == '\\')
            *c = '/';
    }
    size_t base = strlen(p->dir);
    if (base > 0 && strlen(norm) > base && norm[base] == '/')
    {
        char head[1024];
        snprintf(head, sizeof head, "%.*s", (int)base, norm);
        if (ide_path_equal(head, p->dir))
        {
            snprintf(out, cap, "%s", norm + base + 1);
            return;
        }
    }
    snprintf(out, cap, "%s", norm);
}

/* A files[]/include_dirs[] entry back to an absolute path. */
void ide_project_absolute(const struct ide_project* p, const char* entry, char* out, size_t cap)
{
    if (is_absolute(entry))
    {
        snprintf(out, cap, "%s", entry);
    }
    else
    {
        if (snprintf(out, cap, "%s/%s", p->dir, entry) >= (int)cap)
        {
            out[0] = '\0';   /* does not fit */
        }
    }
#ifdef _WIN32
    for (char* c = out; *c; c++)
    {
        if (*c == '/')
            *c = '\\';
    }
#endif
}

int ide_project_contains(const struct ide_project* p, const char* abs_path)
{
    if (!ide_project_is_open(p) || !abs_path)
        return 0;
    for (int i = 0; i < p->files.count; i++)
    {
        char entry[1024];
        ide_project_absolute(p, p->files.items[i], entry, sizeof entry);
        if (ide_path_equal(entry, abs_path))
            return 1;
    }
    return 0;
}

/* Back to "no project open"; the build state too. */
void ide_project_reset(struct ide_project* p)
{
    p->file_path[0] = p->dir[0] = p->name[0] = '\0';
    ide_strings_clear(&p->files);
    p->include_dirs.count = 0;
    memset(p->debug, 0, sizeof p->debug);
    ide_build_state_clear(&p->built);
    ide_strings_clear(&p->compiled);
}

/* --- What the Builds compiled --- */

static void deps_add(struct ide_build_deps* deps, int index)
{
    for (int i = 0; i < deps->count; i++)
    {
        if (deps->items[i] == index)
            return;
    }
    if (deps->count == deps->cap)
    {
        int cap = deps->cap ? deps->cap * 2 : 16;
        int* items = realloc(deps->items, (size_t)cap * sizeof *items);
        if (!items)
            return;
        deps->items = items;
        deps->cap = cap;
    }
    deps->items[deps->count++] = index;
}

void ide_build_state_clear(struct ide_build_state* s)
{
    for (int i = 0; i < s->count; i++)
    {
        free(s->items[i].path);
        free(s->items[i].deps.items);
    }
    s->count = 0;
    free(s->settings);
    s->settings = NULL;
}

int ide_build_state_find(const struct ide_build_state* s, const char* path)
{
    for (int i = 0; i < s->count; i++)
    {
        if (ide_path_equal(s->items[i].path, path))
            return i;
    }
    return -1;
}

int ide_build_state_add(struct ide_build_state* s, const char* path, long long time)
{
    int index = ide_build_state_find(s, path);
    if (index >= 0)
        return index;
    if (s->count == s->cap)
    {
        int cap = s->cap ? s->cap * 2 : 16;
        struct ide_build_entry* items = realloc(s->items, (size_t)cap * sizeof *items);
        if (!items)
            return -1;
        s->items = items;
        s->cap = cap;
    }
    size_t n = strlen(path);
    char* copy = malloc(n + 1);
    if (!copy)
        return -1;
    memcpy(copy, path, n + 1);
    struct ide_build_entry* e = &s->items[s->count];
    memset(e, 0, sizeof *e);
    e->path = copy;
    e->time = time;
    return s->count++;
}

void ide_build_state_add_dep(struct ide_build_state* s, int source, int header)
{
    deps_add(&s->items[source].deps, header);
}

/* Moves what the build `from` read into `to`: times, and each compiled .c's
 * headers. A file with errors is compiled again next time. Leaves `from`
 * empty. */
void ide_build_state_commit(struct ide_build_state* to, struct ide_build_state* from)
{
    int* map = malloc((size_t)(from->count > 0 ? from->count : 1) * sizeof *map);
    if (map)
    {
        for (int i = 0; i < from->count; i++)
        {
            map[i] = ide_build_state_add(to, from->items[i].path, from->items[i].time);
            if (map[i] >= 0)
                to->items[map[i]].time = from->items[i].time;
        }
        for (int i = 0; i < from->count; i++)
        {
            if (!from->items[i].compiled || map[i] < 0)
                continue;
            struct ide_build_entry* e = &to->items[map[i]];
            e->compiled = from->items[i].ok;
            if (!e->compiled)
                continue;
            e->deps.count = 0;
            for (int k = 0; k < from->items[i].deps.count; k++)
            {
                int dep = map[from->items[i].deps.items[k]];
                if (dep >= 0)
                    deps_add(&e->deps, dep);
            }
        }
        free(map);
        free(to->settings);
        to->settings = from->settings;
        from->settings = NULL;
    }
    ide_build_state_clear(from);
}
