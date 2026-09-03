/* ide_debug.c - see ide_debug.h for the module's purpose and usage. */
#include "ide_ui.h"   /* ui_process_start_direct/_write/_read/_close - see
                       * ide_debug.h's comment on why it doesn't include
                       * this itself */
#include "ide_debug.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

#if !defined(_WIN32)
/* fork/execlp/unlink, waitpid and nanosleep - see unix_open_debuggee_tty(),
 * the only thing in this file needing them. */
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#endif

                       /* Trace log for diagnosing the cdb backend - every command sent and every
                        * state transition/parse result, appended to ./cake_ide_debug_trace.log
                        * (relative to wherever the process runs from). Off by default now that the
                        * cdb breakpoint-resolution/locals-parsing/exit-detection issues it was
                        * built to chase are fixed - flip CAKE_IDE_DEBUG_TRACE to 1 to re-enable if
                        * a similar cdb issue needs diagnosing again. Call sites are left in place
                        * either way (see dbg_log() calls throughout this file) since they compile
                        * to nothing when disabled. */
#define CAKE_IDE_DEBUG_TRACE 0

static void dbg_log(const char* fmt, ...)
{
#if CAKE_IDE_DEBUG_TRACE
    static FILE* f = NULL;
    static bool tried_open = false;
    if (!tried_open)
    {
        tried_open = true;
        f = fopen("cake_ide_debug_trace.log", "a");
    }
    if (!f)
        return;

    va_list ap;
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);
    fputc('\n', f);
    fflush(f);
#else
    (void)fmt;
#endif
}

#if defined(_WIN32)
/* Sentinel markers for cdb's `.echo` trick - see debug_refresh_info()'s own
 * comment. Arbitrary but distinctive enough that no real cdb output (a
 * variable name, a stack frame) could ever collide with one by accident. */
#define DEBUG_CDB_LOCALS_DONE "CAKE_IDE_LOCALS_DONE"
#define DEBUG_CDB_BT_DONE     "CAKE_IDE_BT_DONE"
#define DEBUG_CDB_LOCATE_DONE "CAKE_IDE_LOCATE_DONE"

 /* How many times debug_handle_line() will auto-step past a no-line-info
  * stop (see debug_cdb_queue_locate()'s own comment) before giving up.
  * Generous enough to clear a handful of chained thunks (ILT -> import
  * thunk -> ...), but bounded so a genuine pathological case (stepping
  * through library code with no symbols/line info at all, e.g. into a
  * system DLL) can't spin forever. */
#define DEBUG_CDB_LOCATE_MAX_RETRIES 50
  /* Reads just enough of a PE image's headers to say whether it's a 32-bit
   * (I386) executable - the DOS stub's e_lfanew (offset 0x3C, a 4-byte LE
   * offset to the "PE\0\0" header) then that header's IMAGE_FILE_HEADER.
   * Machine field (2 bytes right after the 4-byte signature). Used only to
   * pick a bitness-matched cdb.exe (see debug_start()'s own comment on why -
   * WOW64's extra loader breakpoint otherwise stalls every session before it
   * reaches user code). Any read failure (short file, bad offsets, ...)
   * conservatively returns false (64-bit), which just means falling back to
   * the PATH-resolved "cdb" - the same behavior this had before bitness
   * detection existed. */
static bool win_exe_is_32bit(const char* exe_path)
{
    FILE* f = fopen(exe_path, "rb");
    if (!f)
        return false;

    unsigned char hdr[64];
    bool is32 = false;
    if (fread(hdr, 1, sizeof hdr, f) == sizeof hdr && hdr[0] == 'M' && hdr[1] == 'Z')
    {
        long e_lfanew = hdr[0x3C] | (hdr[0x3D] << 8) | (hdr[0x3E] << 16) | ((long)hdr[0x3F] << 24);
        if (e_lfanew > 0 && fseek(f, e_lfanew, SEEK_SET) == 0)
        {
            unsigned char pe[6];
            if (fread(pe, 1, sizeof pe, f) == sizeof pe &&
                pe[0] == 'P' && pe[1] == 'E' && pe[2] == 0 && pe[3] == 0)
            {
                unsigned short machine = (unsigned short)(pe[4] | (pe[5] << 8));
                is32 = (machine == 0x14c);   /* IMAGE_FILE_MACHINE_I386 */
            }
        }
    }
    fclose(f);
    return is32;
}

/* Picks the bitness-matched cdb.exe for `exe_path` under the standard
 * "Debugging Tools for Windows" install layout (this is where the Windows
 * SDK's own installer, and winget's Microsoft.WindowsSDK.* packages, put
 * it), so a 32-bit target is debugged by a 32-bit cdb rather than falling
 * through WOW64. `out`/`out_cap` receive the full path; returns false (and
 * leaves `out` untouched) if no such file exists there, in which case the
 * caller should fall back to a bare "cdb" resolved off PATH instead. */
static bool win_pick_cdb(const char* exe_path, char* out, size_t out_cap)
{
    const char* pf86 = getenv("ProgramFiles(x86)");
    if (!pf86 || !pf86[0])
        pf86 = "C:\\Program Files (x86)";

    const char* arch = win_exe_is_32bit(exe_path) ? "x86" : "x64";
    snprintf(out, out_cap, "%s\\Windows Kits\\10\\Debuggers\\%s\\cdb.exe", pf86, arch);

    FILE* f = fopen(out, "rb");
    if (!f)
        return false;
    fclose(f);
    return true;
}
#endif

void debug_init(struct debug_session* s)
{
    memset(s, 0, sizeof * s);
    s->state = DBG_IDLE;
    s->cur_line = -1;
}

bool debug_start(struct debug_session* s, const char* exe_path,
                  const char* const exe_args[], const char* dir,
                  char* err, int errcap)
{
    if (s->proc != NULL)
    {
        if (err && errcap > 0)
            snprintf(err, (size_t)errcap, "a debug session is already running");
        return false;
    }

    const char* argv[64];
    int argc = 0;

#if defined(_WIN32)
    s->backend = DEBUG_BACKEND_CDB;
    /* -lines: resolve source-line info, so debug_handle_line()'s
     * cdb_parse_stop_location() has the "[FILE @ LINE]" annotation to
     * scrape (see debug_start()'s own doc comment in ide_debug.h for why
     * cdb rather than lldb on Windows at all). cdb has no "--" option
     * terminator the way lldb does - it simply treats everything after its
     * own recognized flags as the target program and its arguments.
     *
     * A bitness-matched cdb.exe (see win_pick_cdb()) avoids a real trap: a
     * 32-bit target under a 64-bit cdb runs through WOW64, which inserts a
     * SECOND automatic loader breakpoint (ntdll32!LdrpDoDebuggerBreak,
     * after the normal ntdll!LdrpDoDebuggerBreak every launch stops at)
     * before the process reaches user code - one "g" (debug_run's own
     * command) only clears the first, leaving the session stuck there
     * forever with every breakpoint silently unreached. Falls back to a
     * bare "cdb" off PATH if the standard install layout isn't found. */
    char cdb_path[DEBUG_MAX_PATH];
    if (win_pick_cdb(exe_path, cdb_path, sizeof cdb_path))
        argv[argc++] = cdb_path;
    else
        argv[argc++] = "cdb";
    argv[argc++] = "-lines";
    /* -2 gives the debuggee its own console window, the way Visual Studio
     * does: without it the target inherits cdb's stdout - which is this
     * module's scrape pipe - so the program's own printf output lands in
     * the Output window interleaved with cdb's, and anything reading stdin
     * would consume debug_send()'s commands. */
    argv[argc++] = "-2";
    argv[argc++] = exe_path;
    if (exe_args)
    {
        for (int i = 0; exe_args[i] != NULL && argc < 62; i++)
            argv[argc++] = exe_args[i];
    }
    argv[argc] = NULL;
#else
    s->backend = DEBUG_BACKEND_LLDB;
    /* --no-use-colors keeps ANSI escapes out of the Output window's log;
     * -x skips ~/.lldbinit (a human's personal aliases/settings there
     * would otherwise change this module's output in unpredictable ways);
     * "--" ends lldb's own option parsing so exe_path/exe_args are never
     * mistaken for lldb flags, and doubles as the target's own launch
     * arguments once "run" is sent later (same role gdb's --args played). */
    argv[argc++] = "lldb";
    argv[argc++] = "--no-use-colors";
    argv[argc++] = "-x";
    argv[argc++] = "--";
    argv[argc++] = exe_path;
    if (exe_args)
    {
        for (int i = 0; exe_args[i] != NULL && argc < 62; i++)
            argv[argc++] = exe_args[i];
    }
    argv[argc] = NULL;
#endif

    s->proc = ui_process_start_direct(argv, dir, err, errcap);
    if (!s->proc)
        return false;

#if defined(_WIN32)
    /* cdb's "p"/"t" (debug_step_over/debug_step_into) default to stepping
     * ONE MACHINE INSTRUCTION, not one source line - "-lines" only makes
     * line info available for lookup, it doesn't change p/t's own
     * granularity. "l+t" turns on line-oriented stepping (the cdb/WinDbg
     * name for it is literally "line trace"), so p/t behave the way the
     * IDE's Step Over/Into buttons - and debug_step_over/into's own queued
     * "u . L1" follow-up - assume: one source line per step, matching
     * lldb's "next"/"step" on the other platform branch. Without this, a
     * step usually lands on the same line it started on (several
     * instructions per line is typical), which looks from the IDE side
     * exactly like stepping doing nothing. */
    debug_send(s, "l+t");
#endif

    s->state = DBG_STARTING;
    s->cur_file[0] = 0;
    s->cur_line = -1;
    s->last_exit_code = -1;
    s->inbuf_len = 0;
    return true;
}

void debug_send(struct debug_session* s, const char* command)
{
    dbg_log("SEND: %s", command);
    if (!s->proc)
        return;

    char line[1024];
    int n = snprintf(line, sizeof line, "%s\n", command);
    if (n <= 0)
        return;
    if ((size_t)n >= sizeof line)
        n = (int)sizeof line - 1;   /* truncated - still send what fits */

    /* Commands here are short (well under the pipe's own buffer), so a
     * single ui_process_write is expected to accept all of it in one go.
     * If the pipe were ever momentarily full, retrying belongs on a later
     * debug_poll() tick rather than blocking here - not needed in
     * practice for command lines this size, so not built out yet. */
    ui_process_write(s->proc, line, n);
}

void debug_break_insert(struct debug_session* s, const char* file, int line)
{
    char cmd[DEBUG_MAX_PATH + 64];
#if defined(_WIN32)
    /* cdb's own source-breakpoint syntax: backtick-delimited "file:line",
     * matched against loaded modules' line-number tables (see -lines in
     * debug_start()) rather than needing an explicit file path. No
     * quoting needed inside the backticks - cdb's own tokenizer treats
     * everything up to the closing backtick as one argument, spaces
     * included. */
    snprintf(cmd, sizeof cmd, "bp `%s:%d`", file, line);
    dbg_log("BREAK INSERT: file=%s line=%d cmd=%s", file, line, cmd);
#else
    /* Quoted, since `file` can contain spaces (e.g. under "Application
     * Support" - see do_debug_start's own history with exactly that). */
    snprintf(cmd, sizeof cmd, "breakpoint set --file \"%s\" --line %d", file, line);
#endif
    debug_send(s, cmd);
}

/* debug_run/continue/step_over/step_into all set DBG_RUNNING themselves,
 * right after sending the command, rather than waiting for something in
 * lldb's output to say so - unlike GDB/MI's *running record, lldb's plain
 * console output has no single reliable "now running" line to scan for
 * (a step that lands back in the same stopped state almost immediately is
 * common enough that scraping for a transient marker isn't worth it). The
 * optimistic state still self-corrects the moment debug_poll() sees the
 * next real stop or exit report. */
#if defined(_WIN32)
 /* cdb only annotates a location with "[FILE @ LINE]" (what
  * cdb_parse_stop_location() scrapes) on demand - commands like `u`
  * (unassemble), not on the plain stop banner a breakpoint hit or a step
  * landing prints by itself (confirmed empirically: "playground!main+0x3:"
  * with no bracket). "u . L1" (unassemble exactly one instruction at the
  * current instruction pointer, "." meaning "here") is the cheapest command
  * that reliably includes it.
  *
  * Queuing it immediately after g/p/t - rather than waiting to see the stop
  * happen first - relies on the same ordering guarantee debug_refresh_info()
  * already does: cdb (like lldb) processes queued stdin strictly in order,
  * so this command simply sits unexecuted until g/p/t's own blocking wait
  * for the next stop resolves, then runs right after - no separate "has it
  * stopped yet" state needed on this module's side at all.
  *
  * A ".echo" sentinel right after "u . L1" (same trick as
  * debug_refresh_info()'s locals/backtrace pair) closes the probe even when
  * "u . L1" comes back with NO bracket - a real case, not hypothetical: a
  * step can land in a linker-generated ILT thunk or an import thunk, bare
  * glue code with no line info of its own (confirmed empirically - "t" out
  * of a source line whose next statement is a call landed in
  * "playground!ILT+12460(_f):", no bracket). Without this sentinel,
  * debug_handle_line() would have no way to tell "no bracket yet, more
  * output still coming" apart from "no bracket, and that's all there ever
  * will be for this probe" - and s->state would simply stay DBG_RUNNING
  * forever, which looks from the IDE side exactly like Step Into silently
  * doing nothing (confirmed - that's the actual bug report this fixed). */
static void debug_cdb_queue_locate(struct debug_session* s)
{
    s->cdb_locating = true;
    debug_send(s, "u . L1");
    debug_send(s, ".echo " DEBUG_CDB_LOCATE_DONE);
}
#endif

#if !defined(_WIN32)
/* Opens a terminal window for the debuggee to use as its own stdin/stdout,
 * so the program under debug reads and writes a real tty the user can type
 * into instead of sharing lldb's scrape pipe - which would put the
 * program's own printf output in the Output window interleaved with lldb's.
 * This is the equivalent of the "-2" that debug_start() passes cdb on
 * Windows.
 *
 * lldb has its own "process launch -t" for this, but it is not used: on
 * Linux it fails outright rather than falling back, and on macOS it hands
 * the debuggee to a Terminal window through darwin-debug in a way that
 * broke this module's session (see the revert of the first attempt). What
 * is done instead works the same on both: open the window here, and point
 * an ordinary launch at its device with "process launch --stdin/--stdout/
 * --stderr", leaving lldb in full control of the process the way plain
 * "run" does.
 *
 * There is no API for "give me a terminal": the way the window is opened is
 * to run a throwaway shell inside a terminal emulator, have it report which
 * tty it landed on, and then point lldb at that device. The shell then just
 * sleeps, because the window closes the moment its child exits - it is what
 * keeps the window alive for the debuggee's whole session, and killing it
 * (see debug_shutdown()) is what closes the window afterwards. It reports
 * its own pid on the second line for exactly that, and traps SIGTERM into
 * an *exit 0* so the emulator sees a clean exit - Terminal.app only closes
 * a window by itself when the shell exited cleanly.
 *
 * Best effort by design: if no emulator is installed, or the window never
 * reports a tty, the caller falls back to plain "run" and the debuggee's
 * output goes to the Output window as before. Returns true only if
 * s->tty_path is now usable.
 *
 * The wait for that report is a bounded busy-wait on the UI thread - up to
 * TTY_WAIT_MS, only on the first Run of a session. A terminal window takes
 * long enough to appear that polling this across frames instead would mean
 * threading a "still starting" state through debug_poll() and every command
 * that can be issued before the program is running; that complexity is not
 * worth it for a one-off wait this short. */
static bool unix_open_debuggee_tty(struct debug_session* s)
{
    char report[128];
    snprintf(report, sizeof report, "/tmp/cake_ide_tty.%ld", (long)getpid());
    unlink(report);

    char script[512];
    snprintf(script, sizeof script,
             "tty > %s; echo $$ >> %s; "
             "trap 'exit 0' TERM; "
             "while :; do sleep 3600 & wait $!; done",
             report, report);

#if defined(__APPLE__)
    /* Terminal.app is driven through AppleScript, and its "do script" takes
     * the command as an AppleScript string literal - so the shell script
     * goes to a file rather than being quoted into one. "exec /bin/sh" then
     * replaces the interactive login shell the window starts with, which
     * both keeps the reported pid meaningful and keeps zsh's job-control
     * chatter about the sleep out of the debuggee's window. */
    char script_path[128];
    snprintf(script_path, sizeof script_path,
             "/tmp/cake_ide_tty_script.%ld", (long)getpid());
    FILE* sf = fopen(script_path, "w");
    if (!sf)
        return false;
    fprintf(sf, "%s\n", script);
    fclose(sf);
#endif

    pid_t pid = fork();
    if (pid < 0)
    {
#if defined(__APPLE__)
        unlink(script_path);
#endif
        return false;
    }
    if (pid == 0)
    {
#if defined(__APPLE__)
        char osa[256];
        snprintf(osa, sizeof osa,
                 "tell application \"Terminal\" to do script \"exec /bin/sh %s\"",
                 script_path);
        execlp("osascript", "osascript", "-e", osa, (char*)NULL);
#else
        /* Each execlp only returns if that emulator is not installed, so
         * this walks the list in one child process. gnome-terminal is the
         * odd one out: its "-e" is deprecated and mangles a quoted command,
         * while "--" passes the rest through untouched. */
        execlp("gnome-terminal", "gnome-terminal", "--", "sh", "-c", script, (char*)NULL);
        execlp("x-terminal-emulator", "x-terminal-emulator", "-e", "sh", "-c", script, (char*)NULL);
        execlp("konsole", "konsole", "-e", "sh", "-c", script, (char*)NULL);
        execlp("xfce4-terminal", "xfce4-terminal", "-e", "sh", "-c", script, (char*)NULL);
        execlp("mate-terminal", "mate-terminal", "-e", "sh", "-c", script, (char*)NULL);
        execlp("alacritty", "alacritty", "-e", "sh", "-c", script, (char*)NULL);
        execlp("kitty", "kitty", "sh", "-c", script, (char*)NULL);
        execlp("xterm", "xterm", "-e", "sh", "-c", script, (char*)NULL);
#endif
        _exit(127);   /* nothing to run - the parent sees no report and gives up */
    }

    /* osascript, gnome-terminal and konsole all hand the request to an
     * already-running server and exit immediately, so this pid is NOT the
     * window - reaping it just avoids leaving a zombie. The pid that
     * matters is the shell's own, read from the report below. */
    const int TTY_WAIT_MS = 5000;
    bool opened = false;
    for (int waited = 0; waited < TTY_WAIT_MS && !opened; waited += 20)
    {
        FILE* f = fopen(report, "r");
        if (f)
        {
            char dev[64] = { 0 };
            long holder = 0;
            /* Both lines are written before the shell starts sleeping, but
             * this may catch the file with only the first one flushed -
             * treat that as "not ready yet" and come back. */
            if (fgets(dev, sizeof dev, f) && fscanf(f, "%ld", &holder) == 1)
            {
                dev[strcspn(dev, "\r\n")] = 0;
                if (dev[0] == '/')
                {
                    snprintf(s->tty_path, sizeof s->tty_path, "%s", dev);
                    s->tty_pid = holder;
                    opened = true;
                }
            }
            fclose(f);
            if (opened)
            {
                waitpid(pid, NULL, WNOHANG);
                break;
            }
        }

        int status;
        if (pid > 0 && waitpid(pid, &status, WNOHANG) == pid)
        {
            /* The launcher exited. For the server-backed emulators that is
             * normal and the window may still be coming up, so only give up
             * if it failed outright (nothing was installed). */
            if (WIFEXITED(status) && WEXITSTATUS(status) == 127)
                break;
            pid = -1;   /* already reaped - keep waiting for the report */
        }

        struct timespec ts = { 0, 20 * 1000 * 1000 };
        nanosleep(&ts, NULL);
    }

    unlink(report);
#if defined(__APPLE__)
    /* The window's shell has already exec'd it by the time it reports, and
     * nothing re-reads it afterwards. */
    unlink(script_path);
#endif
    return opened;
}
#endif

void debug_run(struct debug_session* s)
{
#if defined(_WIN32)
    /* cdb launches the target itself already suspended at the loader's own
     * initial breakpoint (before any user code runs) - there is no
     * separate "start running" command the way lldb's "run" is; "g" (go)
     * both leaves that initial stop and is the same command debug_continue
     * uses for every stop after. */
    debug_send(s, "g");
    s->cdb_locate_retries = 0;
    debug_cdb_queue_locate(s);
#else
    /* One window per session: the first Run opens it and every later Run
     * relaunches into the same one. When it cannot be opened at all this
     * falls back to plain "run", and the debuggee's output lands in the
     * Output window the way it always did. */
    if (s->tty_path[0] == 0)
        unix_open_debuggee_tty(s);

    if (s->tty_path[0])
    {
        char cmd[256];
        snprintf(cmd, sizeof cmd,
                 "process launch --stdin %s --stdout %s --stderr %s",
                 s->tty_path, s->tty_path, s->tty_path);
        debug_send(s, cmd);
    }
    else
        debug_send(s, "run");
#endif
    s->state = DBG_RUNNING;
    s->cur_line = -1;
}

void debug_continue(struct debug_session* s)
{
#if defined(_WIN32)
    debug_send(s, "g");
    s->cdb_locate_retries = 0;
    debug_cdb_queue_locate(s);
#else
    debug_send(s, "continue");
#endif
    s->state = DBG_RUNNING;
    s->cur_line = -1;
}

void debug_step_over(struct debug_session* s)
{
#if defined(_WIN32)
    debug_send(s, "p");
    s->cdb_locate_retries = 0;
    debug_cdb_queue_locate(s);
#else
    debug_send(s, "next");
#endif
    s->state = DBG_RUNNING;
    s->cur_line = -1;
}

void debug_step_into(struct debug_session* s)
{
#if defined(_WIN32)
    debug_send(s, "t");
    s->cdb_locate_retries = 0;
    debug_cdb_queue_locate(s);
#else
    debug_send(s, "step");
#endif
    s->state = DBG_RUNNING;
    s->cur_line = -1;
}

void debug_interrupt(struct debug_session* s)
{
#if defined(_WIN32)
    /* cdb has no textual "break the running inferior" command reachable
     * over a plain stdin pipe the way lldb's "process interrupt" is - it
     * needs an actual Ctrl+Break delivered to the child's console process
     * group (GenerateConsoleCtrlEvent), which this module's pipe-based
     * ui_process_start_direct doesn't set up. Sending anything over stdin
     * here would be actively harmful, too: cdb re-runs the last g/p/t
     * command on a bare blank line, so even a no-op-looking write could
     * resume/step the target unexpectedly. Left unimplemented rather than
     * doing either of those - Debug menu's own interrupt entry simply has
     * no effect yet on Windows. */
    (void)s;
#else
    debug_send(s, "process interrupt");
#endif
}

void debug_quit(struct debug_session* s)
{
#if defined(_WIN32)
    /* cdb owns the child it launched (unlike lldb's separate "kill the
     * inferior, then quit" split) - "q" alone terminates both the target
     * process and cdb itself, no confirmation prompt. */
    debug_send(s, "q");
#else
    /* "quit" alone prompts "Quit anyway? [Y/n]" (unanswerable over this
     * pipe) whenever the inferior is still running - killing it first
     * avoids that prompt so the session actually ends. */
    debug_send(s, "process kill");
    debug_send(s, "quit");
#endif
}

void debug_refresh_info(struct debug_session* s)
{
    if (!s->proc || s->query_state != DQS_IDLE)
        return;

    s->locals_count = 0;
    s->frames_count = 0;

#if defined(_WIN32)
    /* cdb, driven over a non-tty pipe, does not echo back the command it
     * just read the way lldb's "(lldb) <cmd>" prompt does - so there is no
     * echo line here to wait for before real output starts (unlike lldb's
     * DQS_SENT below), and no echo line to use as a completion marker
     * either. `.echo` (a cdb meta-command that prints its argument back
     * verbatim, unconditionally) stands in for that instead: one sentinel
     * after each real command gives debug_handle_line() an unambiguous,
     * un-misparseable line to watch for. Going straight to DQS_LOCALS
     * (skipping DQS_SENT) reflects that collection starts immediately,
     * with the first sentinel closing it rather than opening it. */
    s->query_state = DQS_LOCALS;
    debug_send(s, "dv");
    debug_send(s, ".echo " DEBUG_CDB_LOCALS_DONE);
    debug_send(s, "k");
    debug_send(s, ".echo " DEBUG_CDB_BT_DONE);
#else
    /* All three sent up front - lldb processes stdin strictly in the order
     * written, so there is no need to wait for one's output before sending
     * the next (see debug_query_state's own comment in ide_debug.h for why
     * three, and why up front rather than reactively). "target list" is
     * the throwaway third command - its own real output is discarded (see
     * debug_handle_line()'s DQS_BACKTRACE case), only its echo line matters. */
    s->query_state = DQS_SENT;
    debug_send(s, "frame variable");
    debug_send(s, "bt");
    debug_send(s, "target list");
#endif
}

void debug_clear_info(struct debug_session* s)
{
    s->locals_count = 0;
    s->frames_count = 0;
    s->info_dirty = true;
}

/* Scans `line` for a stop location - the "frame #0: 0x... `func at
 * FILE:LINE:COL" line lldb prints as part of every stop report (breakpoint
 * hit, step landing, signal), immediately after "Process N stopped" and
 * the "stop reason = ..." line. Only frame #0 (the innermost, currently-
 * executing frame) is ever looked at - a `bt`/backtrace this module never
 * issues would print further "frame #N:" lines this deliberately ignores.
 * Returns true and fills `file_out`/`*line_out` if found. */
static bool lldb_parse_frame0_location(const char* line, char* file_out,
                                        size_t file_cap, int* line_out)
{
    const char* frame = strstr(line, "frame #0:");
    if (!frame)
        return false;

    /* The frame's own "at FILE:LINE:COL" always comes last on the line,
     * after the full "func(args)" description - but when one of those
     * args is itself a function pointer with debug info, lldb annotates
     * IT with its own "at FILE:LINE" too (e.g. "callback=(module`fn at
     * foo.c:14)"), an EARLIER " at " that is not this frame's location at
     * all. Taking the first match (the original bug here) parsed that
     * inner annotation's file instead, corrupting it into garbage
     * spanning both occurrences - silently breaking the exec-line
     * highlight for exactly the frames whose arguments include a function
     * pointer (real-world example: stepping into a function taking a
     * callback parameter). Take the LAST " at " instead. */
    const char* at = NULL;
    for (const char* p = frame; (p = strstr(p, " at ")) != NULL; p++)
        at = p;
    if (!at)
        return false;   /* no debug info for this frame - nothing to parse */
    at += 4;

    /* `at` is "FILE:LINE:COL" through to the end of the line (lldb's own
     * format, not user-controllable) - split from the right twice rather
     * than searching for the first ':', so a path that itself contains a
     * colon (unusual, but not impossible) still parses correctly. */
    const char* last_colon = strrchr(at, ':');
    if (!last_colon || last_colon == at)
        return false;

    const char* p = last_colon - 1;
    while (p > at && *p != ':')
        p--;
    if (*p != ':')
        return false;
    const char* line_colon = p;

    size_t flen = (size_t)(line_colon - at);
    if (flen >= file_cap)
        flen = file_cap - 1;
    memcpy(file_out, at, flen);
    file_out[flen] = 0;

    *line_out = atoi(line_colon + 1);
    return *line_out > 0;
}

/* Parses one line of `bt` output into a debug_frame row: matches "frame
 * #N:" anywhere in the line (the innermost/selected frame is prefixed
 * "* frame #N:" rather than "  frame #N:" - that leading marker plays no
 * part in the match) and keeps everything after "frame #N: " as this
 * frame's display text verbatim - see struct debug_frame's own comment in
 * ide_debug.h on why this isn't reformatted further. Returns false for a
 * non-frame line (`bt` also prints a "* thread #1, ..." header line,
 * which this correctly rejects). */
static bool lldb_parse_backtrace_line(const char* line, int* index_out,
                                       char* text_out, size_t text_cap)
{
    const char* frame = strstr(line, "frame #");
    if (!frame)
        return false;

    const char* num = frame + 7;
    char* end = NULL;
    long idx = strtol(num, &end, 10);
    if (end == num || *end != ':')
        return false;

    const char* text = end + 1;
    while (*text == ' ')
        text++;

    *index_out = (int)idx;
    snprintf(text_out, text_cap, "%s", text);
    return true;
}

/* Parses one line of `frame variable` output into a debug_local row:
 * lldb's format is "(type) name = value" (or just "name = value",
 * depending on settings) - only the name/value split is needed, not the
 * type, so this walks back from the first " = " to the previous space for
 * `name` (works whether or not a "(type) " prefix precedes it) and takes
 * the rest of the line as `value`. Returns false for a line with no
 * " = " at all (an empty scope, or a continuation line of a multi-line
 * aggregate value, is simply skipped rather than misparsed). */
static bool lldb_parse_locals_line(const char* line, char* name_out,
                                    size_t name_cap, char* value_out,
                                    size_t value_cap)
{
    const char* eq = strstr(line, " = ");
    if (!eq)
        return false;

    const char* name_end = eq;
    const char* name_start = name_end;
    while (name_start > line && name_start[-1] != ' ')
        name_start--;
    if (name_start == name_end)
        return false;

    size_t nlen = (size_t)(name_end - name_start);
    if (nlen >= name_cap)
        nlen = name_cap - 1;
    memcpy(name_out, name_start, nlen);
    name_out[nlen] = 0;

    snprintf(value_out, value_cap, "%s", eq + 3);
    return true;
}

#if defined(_WIN32)
/* Scans `line` for cdb's "[FILE @ LINE]" annotation - appended to a
 * module+offset symbol lookup (breakpoint-hit banner, step landing, "u"/"k"
 * output, ...) once -lines has resolved line-number info for that address
 * (see debug_start()'s own comment on why -lines is always passed). Takes
 * the LAST "[...]" containing " @ " on the line, the same reasoning as
 * lldb_parse_frame0_location's own "last ` at `" rule - a frame whose
 * disassembly or arguments embed another bracketed annotation earlier on
 * the line should not shadow this frame's own location. */
static bool cdb_parse_stop_location(const char* line, char* file_out,
                                     size_t file_cap, int* line_out)
{
    const char* best_open = NULL;
    const char* best_at = NULL;
    for (const char* p = line; (p = strchr(p, '[')) != NULL; p++)
    {
        const char* close = strchr(p, ']');
        const char* at = strstr(p, " @ ");
        if (close&& at&& at < close)
        {
            best_open = p;
            best_at = at;
        }
    }
    if (!best_open || !best_at)
        return false;

    size_t flen = (size_t)(best_at - (best_open + 1));
    if (flen >= file_cap)
        flen = file_cap - 1;
    memcpy(file_out, best_open + 1, flen);
    file_out[flen] = 0;

    *line_out = atoi(best_at + 3);
    return *line_out > 0;
}

/* Parses one line of `k` (call stack) output into a debug_frame row's text.
 * Plain `k` (unlike `kn`) prints NO frame-number column at all - each line
 * starts directly with the Child-SP address ("000000ae`bd4ffee0 ..."), not
 * a "00 " index - so this only extracts the "module!function+0xNN
 * [FILE @ LINE]" tail (found textually via '!', the same "don't fully
 * parse, just extract what's needed" spirit as lldb_parse_backtrace_line),
 * never a frame number from the line itself. The caller assigns the frame's
 * index sequentially instead (see its own call site) - an earlier version
 * of this function tried strtol-ing the Child-SP address as if it were a
 * frame number, which every frame on the same thread's stack (nearby
 * addresses share the same upper bits) then reported as the SAME wrong
 * number - confirmed empirically, every row in the Call Stack panel showing
 * identical "#147". Returns false for the header line (" # ...") or any
 * frame with no resolved symbol (no '!' - e.g. deep in ntdll with no public
 * symbols loaded, not useful to show anyway). */
static bool cdb_parse_backtrace_line(const char* line, char* text_out, size_t text_cap)
{
    const char* bang = strchr(line, '!');
    if (!bang)
        return false;

    const char* sym = bang;
    while (sym > line && !isspace((unsigned char)sym[-1]))
        sym--;

    snprintf(text_out, text_cap, "%s", sym);
    return true;
}
#endif

/* Parses one complete console line (no trailing newline) from lldb.
 *
 * While a Locals/Call Stack refresh is in flight (query_state != DQS_IDLE
 * - see debug_refresh_info() and debug_query_state's own comment in
 * ide_debug.h) every line belongs to that state machine instead: an "echo"
 * line (lldb's own "(lldb) ..." prompt+echo of whatever command was just
 * read) closes the current collection phase and advances to the next one;
 * any other line is a real output row, parsed by whichever of the two
 * helpers above matches the current phase. Ordinary stop/exit parsing
 * below is deliberately skipped for the whole window - `bt`'s own output
 * contains a "frame #0:" line that would otherwise be misread as a fresh
 * stop event.
 *
 * Outside that window, the two facts this module otherwise cares about are
 * handled - a stop location (see lldb_parse_frame0_location above) and the
 * inferior's exit report ("Process N exited with status = CODE") -
 * everything else (launch banners, breakpoint-set confirmations, an idle
 * "(lldb) " prompt with nothing queued, ...) is left to the caller's
 * on_output for display and otherwise ignored. */
static void debug_handle_line(struct debug_session* s, const char* line)
{
    dbg_log("RECV: [qstate=%d locating=%d state=%d] %s",
             (int)s->query_state, (int)s->cdb_locating, (int)s->state, line);
#if defined(_WIN32)
    /* cdb's response to ANY command (g, t, p, ...) once the debuggee has
     * actually terminated - unlike a normal exit, which prints "... exit
     * code: N" (see the exit-report check below), this path never does: the
     * process is simply gone and every further command just echoes this
     * same error back, forever. Checked first, unconditionally (even while
     * a Locals/Backtrace query is in flight, since a stale refresh can land
     * here too - see the "Unable to enumerate locals" case observed
     * alongside this), so a dead debuggee is recognized on the very FIRST
     * occurrence instead of burning through DEBUG_CDB_LOCATE_MAX_RETRIES
     * blind "t" retries and then falling back to a fake DBG_STOPPED that
     * just repeats the same retry storm on the next Continue/Step - which
     * is exactly why a session could previously run to completion and then
     * simply never end on its own (Debug > Stop Debugging was the only way
     * out). No exit code is available on this path (cdb never reported
     * one), so last_exit_code is left at whatever it already was. */
    if (strstr(line, "No runnable debuggees") != NULL)
    {
        s->state = DBG_EXITED;
        s->cur_line = -1;
        s->cdb_locating = false;
        s->query_state = DQS_IDLE;
        dbg_log("  NO RUNNABLE DEBUGGEES - forcing DBG_EXITED");
        return;
    }

    if (s->query_state != DQS_IDLE)
    {
        switch (s->query_state)
        {
        case DQS_LOCALS:
            /* debug_refresh_info() starts straight in DQS_LOCALS (cdb has
             * no leading echo of "dv" to wait for, unlike lldb below) - so
             * the FIRST thing seen here is either a real "dv" output row or
             * (an empty locals scope) straight away the LOCALS_DONE
             * sentinel. */
             /* strstr, not strcmp: cdb glues its own "0:000> " prompt onto
              * the front of this echoed sentinel whenever the preceding
              * command (dv, here) produced no output of its own first (an
              * empty locals scope) - an exact-match strcmp then never fires,
              * wedging query_state in DQS_LOCALS forever and silently
              * swallowing every subsequent line (including every future
              * step's own stop location) into this dead branch. The sentinel
              * name is deliberately unique (see its own #define comment), so
              * a substring match is safe. */
            if (strstr(line, DEBUG_CDB_LOCALS_DONE) != NULL)
            {
                s->query_state = DQS_BACKTRACE;
                return;
            }
            /* cdb's own one-time notices (e.g. "*** The C++ standard
             * library and CRT step filter can be enabled to skip this
             * function. Run .settings set Sources.SkipCrtCode = true to
             * enable it. ***", printed the first time a step lands in CRT
             * code with no source) start with "***" and happen to contain a
             * literal " = " of their own ("SkipCrtCode = true") - fed
             * straight into lldb_parse_locals_line() below with no other
             * check, this false-matched as a real locals row (name
             * "Sources.SkipCrtCode"), corrupting the Locals panel with a
             * fake entry. Skip cdb's own notice lines here instead - strstr,
             * not a leading-"***" check, since cdb can glue its own
             * "0:000> " prompt onto the front of this line too (same
             * reasoning as the sentinel checks above). */
            if (strstr(line, "***") != NULL)
                return;
            if (s->locals_count < DEBUG_MAX_LOCALS)
            {
                struct debug_local* loc = &s->locals[s->locals_count];
                if (lldb_parse_locals_line(line, loc->name, sizeof loc->name,
                    loc->value, sizeof loc->value))
                {
                    dbg_log("  LOCAL PARSED: name=%s value=%s", loc->name, loc->value);
                    s->locals_count++;
                }
            }
            return;

        case DQS_BACKTRACE:
            /* strstr, not strcmp - same merged-prompt reasoning as the
             * DQS_LOCALS sentinel check above. */
            if (strstr(line, DEBUG_CDB_BT_DONE) != NULL)
            {
                s->query_state = DQS_IDLE;
                s->info_dirty = true;
                return;
            }
            if (s->frames_count < DEBUG_MAX_FRAMES)
            {
                struct debug_frame* fr = &s->frames[s->frames_count];
                if (cdb_parse_backtrace_line(line, fr->text, sizeof fr->text))
                {
                    fr->index = s->frames_count;   /* sequential - see cdb_parse_backtrace_line's own comment */
                    s->frames_count++;
                }
            }
            return;

        case DQS_SENT:
        case DQS_IDLE:
            break;   /* DQS_SENT unused on this backend; DQS_IDLE unreachable */
        }
    }

    /* cdb's own exit-report wording ("... exit code: N") - scraped the same
     * "find the label, parse what follows" way as lldb's own case below. */
    const char* exited = strstr(line, "exit code:");
    if (exited)
    {
        s->last_exit_code = (int)strtol(exited + strlen("exit code:"), NULL, 0);
        s->state = DBG_EXITED;
        s->cur_line = -1;
        s->cdb_locating = false;   /* nothing left to probe for - the target is gone */
        dbg_log("  EXITED: code=%d", s->last_exit_code);
        return;
    }

    char file[DEBUG_MAX_PATH];
    int found_line = 0;
    if (cdb_parse_stop_location(line, file, sizeof file, &found_line))
    {
        snprintf(s->cur_file, sizeof s->cur_file, "%s", file);
        s->cur_line = found_line;
        s->state = DBG_STOPPED;
        s->cdb_locating = false;
        dbg_log("  STOP LOCATION: file=%s line=%d", s->cur_file, s->cur_line);
        return;
    }

    /* debug_cdb_queue_locate()'s own LOCATE_DONE sentinel, arriving while
     * still cdb_locating, means "u . L1" came back with NO bracket - a real
     * stop with no line info of its own (an ILT/import thunk - see that
     * function's own comment on how this was actually observed). Rather
     * than leaving s->state stuck at DBG_RUNNING forever (which is
     * indistinguishable, from the IDE side, from Step Into silently doing
     * nothing - the actual bug report this fixed), step once more and
     * probe again, up to DEBUG_CDB_LOCATE_MAX_RETRIES times; if even that
     * gives up, still surface DBG_STOPPED (with no cur_file/cur_line - the
     * editor simply won't have anything to highlight) so the Debug menu's
     * Continue/Step items re-enable instead of the session staying wedged. */
     /* strstr, not strcmp - same merged-prompt reasoning as the DQS_LOCALS
      * sentinel check above. */
    if (s->cdb_locating && strstr(line, DEBUG_CDB_LOCATE_DONE) != NULL)
    {
        if (s->cdb_locate_retries < DEBUG_CDB_LOCATE_MAX_RETRIES)
        {
            s->cdb_locate_retries++;
            dbg_log("  LOCATE RETRY #%d (no bracket found)", s->cdb_locate_retries);
            debug_send(s, "t");
            debug_cdb_queue_locate(s);
        }
        else
        {
            dbg_log("  LOCATE GIVE UP after %d retries - forcing DBG_STOPPED with no location", s->cdb_locate_retries);
            s->cdb_locating = false;
            s->state = DBG_STOPPED;
        }
    }
    return;
#else
    if (s->query_state != DQS_IDLE)
    {
        /* Matches both "(lldb) <echoed command>" and a bare "(lldb)" -
         * only the prefix, not the trailing space, is required. */
        bool is_echo = strncmp(line, "(lldb)", 6) == 0;

        switch (s->query_state)
        {
        case DQS_SENT:
            if (is_echo)
                s->query_state = DQS_LOCALS;   /* "frame variable"'s own echo - real output starts next */
            return;

        case DQS_LOCALS:
            if (is_echo)
            {
                s->query_state = DQS_BACKTRACE;   /* "bt"'s echo - locals block is done */
                return;
            }
            if (s->locals_count < DEBUG_MAX_LOCALS)
            {
                struct debug_local* loc = &s->locals[s->locals_count];
                if (lldb_parse_locals_line(line, loc->name, sizeof loc->name,
                    loc->value, sizeof loc->value))
                    s->locals_count++;
            }
            return;

        case DQS_BACKTRACE:
            if (is_echo)
            {
                /* "target list"'s echo - backtrace block is done, and so
                 * is the whole refresh; its own real output (the next
                 * lines) is of no interest and just falls through to
                 * ordinary handling below from here on, once query_state
                 * is back to DQS_IDLE. */
                s->query_state = DQS_IDLE;
                s->info_dirty = true;
                return;
            }
            if (s->frames_count < DEBUG_MAX_FRAMES)
            {
                struct debug_frame* fr = &s->frames[s->frames_count];
                if (lldb_parse_backtrace_line(line, &fr->index, fr->text, sizeof fr->text))
                    s->frames_count++;
            }
            return;

        case DQS_IDLE:
            break;   /* unreachable - the outer `if` already excludes this */
        }
    }

    const char* exited = strstr(line, "exited with status = ");
    if (exited)
    {
        s->last_exit_code = atoi(exited + strlen("exited with status = "));
        s->state = DBG_EXITED;
        s->cur_line = -1;
        return;
    }

    char file[DEBUG_MAX_PATH];
    int found_line = 0;
    if (lldb_parse_frame0_location(line, file, sizeof file, &found_line))
    {
        snprintf(s->cur_file, sizeof s->cur_file, "%s", file);
        s->cur_line = found_line;
        s->state = DBG_STOPPED;
    }
#endif
}

void debug_poll(struct debug_session* s)
{
    if (!s->proc)
        return;

    for (;;)
    {
        if (s->inbuf_len >= sizeof s->inbuf - 1)
        {
            /* A single "line" somehow exceeded the buffer (should not
             * happen for lldb's own output in practice) - drop it rather
             * than wedge forever, and keep going from whatever comes next. */
            s->inbuf_len = 0;
        }

        char chunk[4096];
        int n = ui_process_read(s->proc, chunk, (int)sizeof chunk);
        if (n < 0)
        {
            s->proc_ended = true;
            break;
        }
        if (n == 0)
            break;

        size_t room = sizeof s->inbuf - 1 - s->inbuf_len;
        size_t take = (size_t)n < room ? (size_t)n : room;
        memcpy(s->inbuf + s->inbuf_len, chunk, take);
        s->inbuf_len += take;
        s->inbuf[s->inbuf_len] = 0;

        /* Consume every complete line currently in the buffer, then loop
         * back to read more - ui_process_read may have more waiting even
         * though this particular chunk read fewer than `cap` bytes. */
        for (;;)
        {
            char* nl = memchr(s->inbuf, '\n', s->inbuf_len);
            if (!nl)
                break;

            size_t linelen = (size_t)(nl - s->inbuf);
            char* line = s->inbuf;
            if (linelen > 0 && line[linelen - 1] == '\r')
                linelen--;
            line[linelen] = 0;

            debug_handle_line(s, line);
            if (s->on_output)
                s->on_output(s->on_output_ctx, line, linelen);

            size_t consumed = (size_t)(nl - s->inbuf) + 1;
            memmove(s->inbuf, s->inbuf + consumed, s->inbuf_len - consumed);
            s->inbuf_len -= consumed;
        }
    }
}

bool debug_backend_exited(struct debug_session* s)
{
    if (!s->proc || !s->proc_ended)
        return false;

    ui_process_close(s->proc);
    s->proc = NULL;
    s->proc_ended = false;
    s->state = DBG_IDLE;
    return true;
}

void debug_shutdown(struct debug_session* s)
{
    if (!s->proc)
        return;

    debug_quit(s);

#if !defined(_WIN32)
    /* Close the debuggee's terminal window by killing the shell holding it
     * open (see unix_open_debuggee_tty()) - without this it would sit there
     * sleeping until the user closed it by hand. */
    if (s->tty_pid > 0)
        kill((pid_t)s->tty_pid, SIGTERM);
    s->tty_pid = 0;
    s->tty_path[0] = 0;
#endif

    ui_process_close(s->proc);
    s->proc = NULL;
    s->state = DBG_IDLE;
}
