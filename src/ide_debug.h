/* ide_debug.h - minimal scripted lldb/cdb client for the Cake IDE.
 *
 * Drives a child console debugger (via ui_process_start_direct/
 * ui_process_write, see ide_ui.h), feeding it plain console commands over
 * its stdin and scanning its plain-text stdout, to implement "break at
 * line" style debugging: insert breakpoints by file:line, run/continue/
 * step, and learn where execution stopped. Two backends are supported -
 * lldb (macOS/Linux) and cdb, the console front-end to Microsoft's own
 * Windows debugger engine, used on Windows instead since it needs no
 * Python runtime dependency the way lldb's Windows build does (see
 * debug_start()'s own comment). Neither has a GDB/MI-equivalent structured
 * machine interface in its command-line form, so this module scrapes the
 * small, stable subset of each one's human-readable output needed for that
 * - it is not a general lldb/cdb output parser (see debug_handle_line()'s
 * own comment for exactly what it looks for).
 *
 * Usage, from ide.c: one struct debug_session, zero-initialized (or through
 * debug_init()); debug_start() launches lldb for a given executable; the
 * various debug_* commands drive it; debug_poll() must be called once per
 * frame (the same polling discipline as compile_stream_poll() for external
 * tools) to drain lldb's output and update session state; debug_shutdown()
 * tears it down.
 */
#ifndef IDE_DEBUG_H
#define IDE_DEBUG_H

/* Deliberately NOT "#include ide_ui.h" here: its own header guard only
 * covers most of the file (see the lone "#endif" closing UI_H partway
 * through it) - a second inclusion from here, in a translation unit that
 * already included it directly (every real caller), redefines the
 * unguarded tail (ui_diag_type and friends). All this module actually
 * needs from there is the opaque ui_process pointer type - forward declare
 * it instead, the same way ide_ui.h itself declares it, which C tolerates
 * as an identical redeclaration. Callers (ide.c, ide_debug.c) still need
 * their own "#include ide_ui.h" for the ui_process_start_direct/_write/
 * _read/_close declarations this module's .c file actually calls. */
typedef struct ui_process ui_process;

#include <stddef.h>
#include <stdbool.h>

enum debug_state
{
    DBG_IDLE,      /* no session */
    DBG_STARTING,  /* lldb launched, breakpoints/run not sent yet */
    DBG_RUNNING,   /* debuggee running (or about to be - see debug_run/
                    * debug_continue/debug_step_*, which set this
                    * optimistically the moment the command is sent, since
                    * lldb's plain-text output has no single reliable
                    * "now running" marker the way GDB/MI's *running did) */
    DBG_STOPPED,   /* debuggee stopped (breakpoint hit or step finished) */
    DBG_EXITED     /* debuggee has exited; lldb itself may still be up */
};

#define DEBUG_MAX_PATH 1024
#define DEBUG_INBUF_CAP 16384

/* Which console debugger debug_start() launched - chosen automatically
 * per-platform (see its own comment), never user-selectable, but every
 * command-building/output-parsing path in ide_debug.c that differs between
 * the two branches on this. */
enum debug_backend
{
    DEBUG_BACKEND_LLDB,
    DEBUG_BACKEND_CDB
};

/* One row for the Locals panel - see debug_refresh_info()'s own comment. */
struct debug_local
{
    char name[128];
    char value[256];
};

/* One row for the Call Stack panel. `text` is lldb's own line for this
 * frame verbatim (everything after "frame #N: ") - e.g.
 * "0x0000000100003f78 playground`main at playground.c:7:5" - not
 * reformatted, see the module doc comment on why this stays a scraper
 * rather than a real parser. */
struct debug_frame
{
    int index;
    char text[256];
};

#define DEBUG_MAX_LOCALS 128
#define DEBUG_MAX_FRAMES 64

/* State machine for a Locals/Call Stack refresh (see debug_refresh_info()
 * in ide_debug.c). lldb has no structured "here is where command X's
 * output ends" marker, but when driven over a non-tty pipe it does echo
 * each command it reads back, combined with its own "(lldb) " prompt, as
 * one line BEFORE that command's real output follows - e.g. sending
 * "frame variable" produces a line "(lldb) frame variable", then the
 * actual variable dump, then the next command's own "(lldb) <next>" line.
 * debug_refresh_info() exploits this by sending three commands up front -
 * "frame variable", "bt", and a throwaway third command whose only purpose
 * is to produce one more echo line - so every real command's output block
 * is reliably closed by the NEXT command's echo line, including the last
 * one (a bare idle "(lldb) " prompt with nothing queued after it is not
 * guaranteed to ever be newline-terminated the way an echoed line is, so
 * this sidesteps relying on that). */
enum debug_query_state
{
    DQS_IDLE,       /* no locals/backtrace query in flight */
    DQS_SENT,       /* all three commands sent; waiting for "frame
                     * variable"'s own echo line, which starts real
                     * collection (see debug_handle_line()'s own comment) */
    DQS_LOCALS,     /* collecting "frame variable"'s output, one row per
                     * line, until "bt"'s echo line arrives */
    DQS_BACKTRACE   /* collecting "bt"'s output the same way, until the
                     * closing throwaway command's echo line arrives */
};

struct debug_session
{
    ui_process* proc;
    enum debug_state state;
    enum debug_backend backend;

    /* Where the debuggee is currently stopped, valid when state == DBG_STOPPED. */
    char cur_file[DEBUG_MAX_PATH];
    int cur_line;

    int last_exit_code;   /* valid when state == DBG_EXITED */

    /* cdb backend only - see debug_cdb_queue_locate()'s own comment in
     * ide_debug.c. A step can land in code with no line info of its own
     * (a linker-generated ILT thunk, an import thunk, ...) - cdb_locating
     * tracks a "find where we actually stopped" probe in flight so
     * debug_handle_line() can auto-retry (one more step, then probe again)
     * instead of leaving the session stuck in DBG_RUNNING forever once a
     * probe comes back with no resolvable location; cdb_locate_retries
     * bounds how many times it will do that before giving up and just
     * reporting DBG_STOPPED with no cur_file/cur_line, rather than looping
     * indefinitely (see debug_handle_line()'s own comment). Unused on the
     * lldb backend. */
    bool cdb_locating;
    int cdb_locate_retries;

    /* Raw bytes from lldb not yet resolved into a complete '\n'-terminated
     * line - carried across debug_poll() calls, since a single read can
     * land mid-line (see ui_process_read's non-blocking, partial-read
     * contract in ide_ui.h). */
    char inbuf[DEBUG_INBUF_CAP];
    size_t inbuf_len;

    /* Called with every raw line lldb prints (its own "(lldb) " prompt
     * included), newline included - for mirroring into the IDE's Output
     * window the same way External Tools output is shown. May be NULL. */
    void (*on_output)(void* ctx, const char* line, size_t len);
    void* on_output_ctx;

    bool proc_ended;   /* set by debug_poll() once lldb's own pipe hits EOF -
                         * see debug_backend_exited() */

    /* Locals/Call Stack query state - see debug_refresh_info() and
     * debug_handle_line()'s own comments in ide_debug.c. */
    enum debug_query_state query_state;

    struct debug_local locals[DEBUG_MAX_LOCALS];
    int locals_count;
    struct debug_frame frames[DEBUG_MAX_FRAMES];
    int frames_count;
    bool info_dirty;   /* set once a query completes (or state leaves
                        * DBG_STOPPED and the arrays are cleared) - ide.c
                        * clears it after repainting the panel */

#if !defined(_WIN32)
    /* The terminal window the debuggee gets for its own stdin/stdout, on
     * the platforms where lldb is the backend - see
     * unix_open_debuggee_tty() in ide_debug.c. tty_path is the window's
     * device ("/dev/ttys002", ...) and tty_pid the shell holding it open,
     * which debug_shutdown() kills to close the window again. Both stay
     * zeroed when no window could be opened, which is the signal to fall
     * back to a plain "run". */
    char tty_path[64];
    long tty_pid;
#endif
};

void debug_init(struct debug_session* s);

/* Launches the platform's console debugger against `exe_path`/`exe_args`
 * (exe_args may be NULL for none) in `dir`, and records which one in
 * `s->backend`:
 *
 *   non-Windows: `lldb --no-use-colors -x -- <exe_path> <exe_args...>`.
 *                -x skips the user's ~/.lldbinit, so output stays
 *                predictable regardless of what a human's own lldb setup
 *                does.
 *
 *   Windows:     `cdb -lines <exe_path> <exe_args...>`. cdb, not lldb -
 *                LLVM's Windows lldb build links against a specific
 *                python3XX.dll a machine may not have (the whole reason
 *                this backend exists), whereas cdb ships as part of the
 *                OS-vendor's own "Debugging Tools for Windows" with no such
 *                dependency. -lines turns on line-number resolution, the
 *                same thing debug_handle_line()'s cdb-side stop parser
 *                depends on (a "[FILE @ LINE]" annotation cdb appends to
 *                symbol lookups once source-line info is loaded - see its
 *                own comment).
 *
 * Returns true on success; on failure `err`/`errcap` receive a message
 * (same convention as ui_process_start). Session state becomes
 * DBG_STARTING either way. */
bool debug_start(struct debug_session* s, const char* exe_path,
                  const char* const exe_args[], const char* dir,
                  char* err, int errcap);

/* Sends one raw lldb console command line (a trailing '\n' is added
 * automatically - do not include one). No-op if there is no live session. */
void debug_send(struct debug_session* s, const char* command);

/* Convenience wrappers around debug_send() for the operations the IDE's
 * Debug menu needs. `file` for debug_break_insert should be a path lldb
 * can resolve - typically the same path the editor has open, since
 * codegen's #line directives (see codegen.c) make lldb report/stop at the
 * original source file even though it is really debugging generated C. */
void debug_break_insert(struct debug_session* s, const char* file, int line);
void debug_run(struct debug_session* s);
void debug_continue(struct debug_session* s);
void debug_step_over(struct debug_session* s);
void debug_step_into(struct debug_session* s);
void debug_interrupt(struct debug_session* s);   /* pause a running debuggee */
void debug_quit(struct debug_session* s);         /* kill the inferior, then quit lldb */

/* Kicks off a refresh of `s->locals`/`s->frames`: sends "frame variable",
 * "bt", and a throwaway closing command up front (see debug_query_state's
 * own comment for why), then lets debug_poll()/debug_handle_line() collect
 * each command's output as it streams in, finishing with `s->info_dirty =
 * true`. Only meaningful (and only ever called by ide.c) right after a
 * fresh stop - see the module doc comment. A no-op if a query is already
 * in flight (`s->query_state != DQS_IDLE`), so a caller doesn't need to
 * track that itself. */
void debug_refresh_info(struct debug_session* s);

/* Clears `s->locals`/`s->frames` and sets `s->info_dirty` - call whenever
 * the debuggee leaves DBG_STOPPED (running again, exited, or the session
 * ending), so the panel doesn't keep showing a stale stop's data. */
void debug_clear_info(struct debug_session* s);

/* Drains whatever lldb has printed since the last call, updates `s`'s
 * state/cur_file/cur_line, and invokes on_output for each line seen. Cheap
 * no-op when idle (mirrors compile_stream_poll's contract) - call once per
 * frame. */
void debug_poll(struct debug_session* s);

/* True once lldb's own process has exited and been reaped (distinct from
 * the debuggee exiting - DBG_EXITED - which leaves lldb itself still
 * running). Reaps the process and resets `s` to DBG_IDLE when it becomes
 * true. */
bool debug_backend_exited(struct debug_session* s);

void debug_shutdown(struct debug_session* s);

#endif
