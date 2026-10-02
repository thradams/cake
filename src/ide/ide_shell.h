/* ide_shell.h - what the IDE's own files (ide_shell.c, ide_themes.c,
 * ide_highlight.c, ide_services.c) share. The IDE is an app of the ide
 * GUI framework: it uses only ide_gui.h. See GUI_IDE_SPEC.md.
 */
#ifndef IDE_SHELL_H
#define IDE_SHELL_H

#include "ide_gui.h"
#include <stddef.h>

#ifdef _WIN32
#define IDE_PATH_SEP "\\"
#else
#define IDE_PATH_SEP "/"
#endif

/* --- Themes (ide_themes.c) --- */

extern const struct gui_theme ide_theme_ambar;
extern const struct gui_theme ide_theme_dark;
extern const struct gui_theme ide_theme_white;
extern const struct gui_theme ide_theme_nebula;
extern const struct gui_theme ide_theme_xcode_dark;

/* --- Highlighters (ide_highlight.c) --- */

/* C source; `ctx` is the struct gui_theme to color with. */
int ide_highlight_c(void* ctx, const char* line, int len, int* state,
                     struct gui_span* spans, int max);

/* Markdown source, its delimiters colored too; `ctx` the theme. Its row_bg
 * tints a ``` code block. */
int ide_highlight_md(void* ctx, const char* line, int len, int* state,
                      struct gui_span* spans, int max);
uint32_t ide_md_row_bg(void* ctx, const char* line, int len, int state);


/* --- Services (ide_services.c) - files and folders --- */

struct ide_dir_entry
{
    char name[260];
    int is_dir;
};

/* The entries of `dir`, folders first, each group sorted by name; "." and
 * ".." left out. Returns how many (at most `max`), -1 if it cannot be read. */
int ide_list_dir(const char* dir, struct ide_dir_entry* out, int max);

/* The whole file, malloc'ed, with "\r\n" turned into "\n"; *crlf says
 * whether it had any. NULL if it cannot be read. */
char* ide_read_file(const char* path, int* crlf);

/* Writes `text`, turning "\n" back into "\r\n" when `crlf`. 0 on success. */
int ide_write_file(const char* path, const char* text, int crlf);

/* The IDE's per-user folder - %APPDATA%\cake_ide on Windows - created if
 * missing. 0 if it cannot be found. */
int ide_config_dir(char* buf, int cap);

/* The folder of the running executable, into buf ("." if unknown). */
void ide_exe_dir(char* buf, int cap);

/* Opens `url` in the default browser. */
void ide_open_url(const char* url);

/* A terminal window of the system's, in `dir`. */
void ide_open_terminal(const char* dir);

/* `path` made absolute (against the current directory), into buf. */
void ide_full_path(const char* path, char* buf, int cap);

/* The current directory, into buf. */
void ide_current_dir(char* buf, int cap);

int ide_is_dir(const char* path);
int ide_file_exists(const char* path);

/* Creates the folder `path` (its parent must exist). 0 on success. */
int ide_make_dir(const char* path);

/* Deletes the file `path`, or the folder when `is_dir` (it must be empty).
 * 0 on success. */
int ide_delete_path(const char* path, int is_dir);

/* A child process whose stdout and stderr are read without waiting. */
struct ide_process;

/* Starts `command_line` (no shell) in `dir` (NULL: the current one). NULL
 * if it cannot start. */
struct ide_process* ide_process_start(const char* command_line, const char* dir);

/* What it printed since the last call, into buf: > 0 bytes, 0 nothing yet,
 * -1 it ended and everything was read. */
int ide_process_read(struct ide_process* p, char* buf, int cap);

/* Frees it (it has ended); returns its exit code. */
int ide_process_close(struct ide_process* p);

/* Typed into its stdin; -1 when it is closed or the process ended. */
int ide_process_write(struct ide_process* p, const char* data, int len);
void ide_process_close_stdin(struct ide_process* p);   /* end of input */
void ide_process_kill(struct ide_process* p);          /* then read to -1 and close */

/* Runs `command_line` (no shell) in `dir` (NULL: the current one) and waits
 * for it; its stdout and stderr, malloc'ed. NULL if it cannot start. */
char* ide_run_capture(const char* command_line, const char* dir);

/* This machine's system include directories - the ones the platform
 * compiler searches: on Windows MSVC's (found with vswhere.exe) and the
 * newest Windows SDK's (from the registry). `add` gets each one. What
 * could not be found goes into `problems`, one line each ("" if nothing). */
/* Where Visual Studio and the Windows SDK are - what the detection finds on
 * the way, for adding the compiler to Tools. Empty fields: not found. */
struct ide_msvc_toolchain
{
    char vs_dir[512];
    char version[64];
    char sdk_root[512];
    char sdk_version[64];
};

void ide_detect_include_dirs(void (*add)(void* ctx, const char* dir), void* ctx,
                              char* problems, int cap, struct ide_msvc_toolchain* toolchain);

/* The Tiny C Compiler: "tcc" when it runs from PATH, else tcc.exe in a usual
 * folder (C:\tcc, C:\tcc\win32, Program Files\tcc, a tcc folder next to the
 * IDE), into `command`. 0 if none. */
int ide_find_tcc(char* command, int cap);

/* TCC's include directories, from `tcc -print-search-dirs`; `problems` says
 * why when there are none. Returns how many. */
int ide_detect_tcc_include_dirs(void (*add)(void* ctx, const char* dir), void* ctx,
                                 char* problems, int cap);

/* Whether `command_line`'s output has `signature` - "gcc --version" has
 * "Free Software Foundation", say. */
int ide_output_has(const char* command_line, const char* signature);

/* --- Settings the compile uses --- */

/* Compiler Options: the global ones or a project's. */
struct compiler_settings
{
    int target, headers, style, diag;   /* each select's row */
    int flags[5];
    char output[256];
    char options[512];
};

#define MAX_INCLUDE_DIRS 64

struct include_dirs
{
    char dirs[MAX_INCLUDE_DIRS][512];
    int count;
};

/* --- Projects (ide_project.c) --- */

struct ide_strings
{
    char** items;
    int count, cap;
};

int ide_strings_add(struct ide_strings* l, const char* s);   /* 0 if out of memory */
void ide_strings_remove_at(struct ide_strings* l, int index);
void ide_strings_clear(struct ide_strings* l);
void ide_strings_destroy(struct ide_strings* l);

/* Case-insensitive, '/' and '\' alike. */
int ide_path_equal(const char* a, const char* b);

/* The last-modified time of `path`, 0 if it cannot be read. */
long long ide_file_time(const char* path);

/* A .c file's headers: indexes into its ide_build_state. */
struct ide_build_deps
{
    int* items;
    int count, cap;
};

/* A file a build read - a compiled .c or a header it included - with its
 * time as of that build. */
struct ide_build_entry
{
    char* path;
    long long time;
    int compiled;                  /* a .c file the build compiled */
    int ok;                        /* ...with no errors */
    struct ide_build_deps deps;   /* a compiled .c's headers */
    int changed;                   /* scratch for Build: the time differs */
};

/* What the successful Builds compiled, in memory only: Build compiles just
 * the .c files changed since, or including a header that changed. Entries
 * are never removed, so deps stay valid. */
struct ide_build_state
{
    struct ide_build_entry* items;
    int count, cap;
    char* settings;   /* the command line it was built with; a change rebuilds all */
};

void ide_build_state_clear(struct ide_build_state* s);
int ide_build_state_find(const struct ide_build_state* s, const char* path);
int ide_build_state_add(struct ide_build_state* s, const char* path, long long time);
void ide_build_state_add_dep(struct ide_build_state* s, int source, int header);
void ide_build_state_commit(struct ide_build_state* to, struct ide_build_state* from);

/* The open project: a .cakeproj whose files and include directories are
 * stored relative to its folder. */
struct ide_project
{
    char file_path[1024];   /* "" when no project is open */
    char dir[1024];
    char name[256];
    struct ide_strings files;
    struct include_dirs include_dirs;
    struct compiler_settings compile;
    char debug[3][512];             /* the old IDE's per-project debug command, kept as read */
    long long file_time;            /* the .cakeproj's time at our last load or save */
    struct ide_build_state built;
    struct ide_strings compiled;   /* the .c files the last Build compiled */
};

int ide_project_is_open(const struct ide_project* p);
void ide_project_relative(const struct ide_project* p, const char* abs_path, char* out, size_t cap);
void ide_project_absolute(const struct ide_project* p, const char* entry, char* out, size_t cap);
int ide_project_contains(const struct ide_project* p, const char* abs_path);
void ide_project_reset(struct ide_project* p);

/* --- Compile (ide_compile.c) - the Cake compiler, linked in, on a worker
 * thread; its stdout is captured --- */

/* A growing text buffer. */
struct ide_text
{
    char* data;
    size_t len, cap;
};

void ide_text_append(struct ide_text* t, const char* data, size_t n);
void ide_text_printf(struct ide_text* t, const char* fmt, ...);

/* --- Text search (ide_search.c) --- */

struct ide_search
{
    char pattern[256];
    int match_case;
    int whole_word;
};

/* Each match of `s` in `text` as "name:line:col: <line>", the match in
 * VT100 bold yellow. Returns how many. */
int ide_search_text(const char* name, const char* text, const struct ide_search* s, struct ide_text* out);

/* `text` with each match of `s` replaced by `with`, malloc'ed; NULL when
 * there is none. *count gets how many. */
char* ide_replace_text(const char* text, const struct ide_search* s, const char* with, int* count);

struct ide_compile_job;

struct ide_compile_job* ide_compile_create(void);
void ide_compile_destroy(struct ide_compile_job* job);

/* A new command line: reset, then the arguments one by one ("cake" first). */
void ide_compile_reset(struct ide_compile_job* job);
void ide_compile_arg(struct ide_compile_job* job, const char* arg);

/* Text put before the compiler's output (a Build's reasons). */
void ide_compile_note(struct ide_compile_job* job, const char* text);

/* Told, on the worker thread, of each file a source includes and of each
 * source done - for the incremental Build. NULL: nothing. */
struct ide_build_listener
{
    void (*included)(void* ctx, const char* source, const char* header);
    void (*done)(void* ctx, const char* source, int errors);
    void* ctx;
};
void ide_compile_set_listener(struct ide_compile_job* job, const struct ide_build_listener* l);

/* The arguments so far, one per line, into buf - what a Build compares to
 * know whether the settings changed. */
void ide_compile_command(const struct ide_compile_job* job, char* buf, size_t cap);

/* Starts the compile; 0 if it could not (or one is running). */
int ide_compile_start(struct ide_compile_job* job);
int ide_compile_running(const struct ide_compile_job* job);

/* Called from a timer: reads the output so far (*new_output says whether
 * any came) and returns 1 once, when the compile has ended. */
int ide_compile_poll(struct ide_compile_job* job, int* new_output);

const char* ide_compile_output(const struct ide_compile_job* job);
void ide_compile_counts(const struct ide_compile_job* job, int* errors, int* warnings, double* seconds);

#endif
