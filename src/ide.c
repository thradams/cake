/* ide.c - Cake IDE: a DOS-styled UI demo.
 *
 * Everything visible is a DOM node, all built programmatically via
 * ui_create_element/ui_append_child/ui_set_* - the menu bar/dropdowns/status
 * bar (build_screen(), called once from app_init()) and the "Welcome"
 * panel/text/cursor (also in app_init()) alike. Numeric event codes (not
 * function-call return values) report clicks and menu selections.
 * Platform-agnostic: uses only terminal.h/ui.h.
 */
#include "ide_ui.h"
#include "fs.h"
#include "version.h"
#include "ide_lsp.h"
#include "ide_debug.h"
#include "error.h"   /* _Countof */
#include "target.h"  /* parse_target/get_platform - see the $(CakeOutput)
                      * External Tools macro (exttool_expand()), which has to
                      * predict cake's own "<root>/<platform name>/..."
                      * output layout to hand a real compiler its files */
#include "options.h"  /* fill_options - validates the Compiler Options field, see copts_find_invalid() */
#include "json.h"     /* the ".cakeproj" reader/writer - see project_save() */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

 /* The real compiler, linked in separately for this integration test - not
  * implemented here. Prints its diagnostics straight to stdout; do_compile()
  * below redirects that into the Output window. */
struct report {
    int no_files;
    double cpu_time_used_sec;

    /* has_errors is not reseted when we remove diagnostics */
    bool has_errors;

    int error_count;
    int warnings_count;
    int info_count;

    bool test_mode;
    int test_failed;
    int test_succeeded;

    /*
      direct commands like -autoconfig doesnt use report
    */
    bool ignore_this_report;
};
extern int compile(int argc, const char** argv, struct report* report);

/* Authentic Turbo Vision DOS color palette, matched against a real Turbo
 * Vision screenshot (tvision/tvedit): blue window body, gray frames/menus/
 * status bar with black text, bright cyan selection highlight. */
#define COLOR_CYAN          TB_RGB(0x55, 0xFF, 0xFF) /* Bright cyan */
#define COLOR_YELLOW        TB_RGB(0xFF, 0xFF, 0x55) /* Bright yellow */
#define COLOR_WHITE         TB_RGB(0xFF, 0xFF, 0xFF) /* Bright white */
#define COLOR_GREEN         TB_RGB(0x55, 0xFF, 0x55) /* Bright green */

 /* Numeric event codes - assigned directly where each widget is built below,
  * Win32 WM_COMMAND-style. No collision-avoidance scheme needed: each
  * widget's id is just written by hand where it's declared. */
enum {
    EVT_FILE_NEW = 1,
    EVT_FILE_OPEN = 2,
    EVT_FILE_SAVE = 3,
    EVT_FILE_SAVEAS = 4,
    EVT_FILE_EXIT = 5,
    EVT_FILE_SAVEALL = 6,
    EVT_FILE_OPENFOLDER = 7,  /* the File > "Open Folder..." menu item's id */
    EVT_WINDOW_TILE = 80,
    EVT_WINDOW_CASCADE = 81,
    EVT_WINDOW_CLOSEALL = 82,
    EVT_WINDOW_FOLDER = 87,  /* the View > "Show Folder" menu item's id -
                              * re-raises the persistent folder browser
                              * window (moved here from Window, its ids kept
                              * as-is - see build_screen()) */
    EVT_WINDOW_PLAYGROUND = 88,  /* View > "Show Playground" - opens (or
                                  * re-raises, if already open) the scratch
                                  * editor window pinned to playground.c;
                                  * see open_playground() */
    EVT_VIEW_LINENUMBERS = 89,  /* View > "Line Numbers" - toggles the
                                 * gutter drawn in every source <editor> (see
                                 * ui_set_show_line_numbers/render_editor in
                                 * ide_ui.c). Same "[x]"/"[ ]" convention and
                                 * per-frame refresh as the other View items
                                 * just above, but a real boolean setting
                                 * rather than "is this window open" - see
                                 * g_view_linenumbers_item's own doc
                                 * comment. Defaults ON, not persisted
                                 * across sessions (same as the Environment
                                 * dialog's theme choice). */
    EVT_EDIT_UNDO = 10,
    EVT_EDIT_CUT = 11,
    EVT_EDIT_COPY = 12,
    EVT_EDIT_PASTE = 13,
    EVT_EDIT_REDO = 14,
    EVT_EDIT_STRINGIFY = 15,  /* Edit > "Stringify" - see do_edit_stringify() */
    EVT_EDIT_TOUPPER = 16,    /* Edit > "To Upper" */
    EVT_EDIT_TOLOWER = 17,    /* Edit > "To Lower" */
    EVT_EDIT_WORDWRAP = 18,   /* Edit > "Word Wrap..." - opens the Columns
                               * dialog, see do_edit_wordwrap() */
    EVT_EDIT_FORMAT = 19,     /* Edit > "Format C Source" - see do_edit_format() */
    /* Debug menu (scripted lldb integration - see ide_debug.h). Start launches lldb
     * on the active document's already-built executable (same directory,
     * same base name, no extension - e.g. foo.c -> foo) and inserts a
     * breakpoint for every line toggled in that document's gutter (see
     * ui_editor_toggle_breakpoint in ide_ui.h); building that executable
     * (with debug info) is on the user for now, same as any external build
     * step Compile itself doesn't do either - see do_debug_start()'s own
     * doc comment. */
    EVT_DEBUG_START = 30,
    EVT_DEBUG_STOP = 31,
    EVT_DEBUG_STEP_INTO = 34,
    EVT_DEBUG_STEP_OVER = 35,
    EVT_DEBUG_CONTINUE = 36,
    EVT_DEBUG_TOGGLE_BREAKPOINT = 37,  /* Debug > "Toggle Breakpoint" (F9) -
                                        * toggles a breakpoint on the active
                                        * editor's cursor line, the keyboard
                                        * equivalent of a gutter click (see
                                        * editor_click_set_cursor in
                                        * ide_ui.c) */
    EVT_WINDOW_DEBUGINFO = 38,  /* the Debug > "Debug Info" menu item's id -
                                 * re-raises the docked Locals/Call Stack
                                 * panel (see debug_info_panel_refresh()),
                                 * same singleton-window convention as
                                 * EVT_WINDOW_OUTPUT/EVT_WINDOW_FOLDER */
    EVT_COMPILE = 40,  /* Build - the whole project whenever one is open,
                        * otherwise just the active file (see do_build()) */
    EVT_COMPILE_FILE = 41,  /* Compile - always just the active file, never
                             * the project (see do_compile()) */
    EVT_COMPILE_OPTIONS = 45,  /* File > Options... - the global compiler
                                 * settings (cake.json); opens the dialog below */
    EVT_GLOBAL_INCLUDES = 47,      /* File > "Directories..." - the same
                                    * Include Directories dialog as
                                    * EVT_PROJECT_INCLUDES, but bound to the
                                    * global list in cake.json */
    EVT_WINDOW_OUTPUT = 83,  /* the View > "Show Output" menu item's id -
                              * re-raises the diagnostics Output window
                              * (moved here from Window - see build_screen()) */
    /* Window > Font Size +/- drive the same backend action as Ctrl+/Ctrl-,
     * via ui_env_adjust_font_size() (see ui.h). */
    EVT_WINDOW_FONT_INC = 85,
    EVT_WINDOW_FONT_DEC = 86,
    EVT_HELP_CONTEXTUAL = 200,  /* the status bar's F1 hotkey - see
                                 * show_hint_window() */
    EVT_HELP_ABOUT = 92,
    EVT_HELP_MANUAL = 94,  /* Help > "Manual" - web/manual.html in the browser,
                            * see do_help_manual() */
    EVT_HELP_WEBSITE = 95,  /* Help > "Cake Website" - https://cakecc.org/ */
    EVT_OPTIONS_ENV = 72,
    EVT_OPTIONS_DIRS = 73,
    EVT_ABOUT_MODAL = 400,
    EVT_ABOUT_CLOSE = 401,
    EVT_DIRS_OK = 501,
    EVT_DIRS_CANCEL = 502,
    EVT_DIRS_HELP = 503,
    EVT_ENV_OK = 601,
    EVT_ENV_THEME_AMBAR = 610,
    EVT_ENV_THEME_DARK = 611,
    EVT_ENV_THEME_WHITE = 612,
    EVT_ENV_THEME_NEBULA = 613,
    EVT_ENV_THEME_XCODE_DARK = 614,
    EVT_ENV_FONT_BASE = 620,  /* base id for the "Font" <select>'s options -
                               * EVT_ENV_FONT_BASE + index, reserving 620..63x
                               * (the backend's shortlist is a handful of
                               * entries; see ui_env_font_family_count) */
    EVT_OPEN_NAME = 700,
    EVT_OPEN_LISTBOX = 701,
    EVT_OPEN_OK = 702,
    EVT_OPEN_CANCEL = 704,
    EVT_OPEN_FILTER = 705,  /* base id for the "Files of type" <select>'s
                             * options - EVT_OPEN_FILTER + index into
                             * g_open_filters[]; reserves 705..70x, next free
                             * id after this block is 705 + OPEN_FILTER_COUNT */
    EVT_SEARCH_GOTO = 22,   /* the Search > "Go to line..." menu item's id */
    EVT_GOTO_INPUT = 801,
    EVT_GOTO_OK = 802,
    EVT_GOTO_CANCEL = 803,
    EVT_OUTPUT_DBLCLICK = 810,
    EVT_EDITOR_CTRLCLICK = 811,  /* every document <editor>'s id - Ctrl+click,
                                  * for Markdown "open link" (see ui.c's EDITOR
                                  * mouse handling and do_editor_ctrlclick) */
    EVT_EDITOR_TOGGLE_READONLY = 812,  /* the editor popup's "Read-only" item */
    EVT_FOLDER_LISTBOX = 813,  /* the persistent folder browser window's own
                                * <listbox> - see folder_window_activate */
    EVT_EDITOR_COPY_PATH = 814,  /* the editor popup's "Copy Full Path" item */
    EVT_EDITOR_TOGGLE_HDRSRC = 815,  /* the editor popup's "Toggle Header/
                                      * Source" item - .c/.h files only */
    EVT_EDITOR_SHOW_OUTPUT = 816,  /* the editor popup's/Compile menu's "Show
                                    * Generated Code" item - opens the
                                    * compiled/generated code, distinct from
                                    * View > "Show Output" (EVT_WINDOW_OUTPUT),
                                    * which raises the errors/diagnostics
                                    * window - kept "OUTPUT" in the id name
                                    * only for history's sake */
    EVT_EDITOR_SHOW_FOLDER = 817,  /* the editor popup's "Show My Folder" item -
                                    * reveals the document's containing folder
                                    * in the Folder panel, see
                                    * folder_reveal_directory() */
    EVT_EDITOR_CODEBLOCK_COPY = 818,  /* the editor popup's "Copy Code Block"
                                       * item - only enabled when the right-
                                       * click that opened the popup landed on/
                                       * in a Markdown fenced code block, see
                                       * refresh_codeblock_items() */
    EVT_EDITOR_CODEBLOCK_PLAYGROUND = 819,  /* the editor popup's "Copy to
                                             * Playground" item - writes that
                                             * same fenced code block into
                                             * playground.c and opens it, see
                                             * refresh_codeblock_items() */
    EVT_SEARCH_FIND = 20,      /* the Search > "Find..." menu item's id */
    EVT_SEARCH_REPLACE = 21,   /* the Search > "Replace..." menu item's id */
    EVT_SEARCH_NEXT = 23,      /* Search > "Search Next" (F3) */
    EVT_SEARCH_GOTO_DEFINITION = 24,  /* Search > "Go to Definition" (F12) -
                                       * see do_goto_definition() */
    EVT_REPLACE_OK = 820,
    EVT_REPLACE_CHANGEALL = 821,
    EVT_REPLACE_CANCEL = 822,
    EVT_FIND_OK = 830,
    EVT_FIND_CANCEL = 831,
    EVT_SAVEAS_OVERWRITE = 840,  /* "Yes" in the overwrite-confirm message box */
    EVT_PROJECT_NEW_OVERWRITE = 842,  /* same, for Project > "New Project..."
                                       * (see project_new_save_activate()) */
    EVT_FILE_RELOAD = 843,     /* "Yes" in the "changed outside the IDE"
                                * prompt for the active file - see
                                * file_watch_check() */
    EVT_PROJECT_RELOAD = 844,  /* same, for the open project's .cakeproj */
    EVT_CLOSE_DISCARD = 841, /* "Discard" in the unsaved-changes-on-close
                               * confirm message box - see UI_CLOSE_REQUEST_ID
                               * and g_pending_close_window */
    EVT_WORDWRAP_INPUT = 850,   /* Edit > Word Wrap...'s "Columns" <input> */
    EVT_WORDWRAP_OK = 851,
    EVT_WORDWRAP_CANCEL = 852,
    EVT_COPTS_OK = 900,
    EVT_COPTS_CANCEL = 901,
    EVT_COPTS_KEEP_INVALID = 904,  /* "Keep" in the invalid-options warning - saves them anyway, see copts_find_invalid() */
    EVT_COPTS_HELP = 902,  /* Compiler Options' Help button - the dialog's
                             * own overview in the help window, see
                             * show_help_text() */
    EVT_HINTWIN_CLOSE = 903,  /* the full-help window's Close button - see
                               * show_hint_window() */
    EVT_COPTS_TARGET = 910,  /* base id for the Target <select>'s options */
    EVT_COPTS_STYLE = 920,   /* base id for the Style <select>'s options */
    EVT_COPTS_DIAGFORMAT = 930, /* base id for the Output Format <select>'s options */
    EVT_TOOLS_FINDREPLACE = 63,  /* Tools > "Find and Replace..." - opens/
                                  * raises the docked panel (see g_fr.window) */
    EVT_TOOLS_TERMINAL = 64,    /* Tools > "Terminal" - see do_open_terminal() */
    EVT_TOOLS_EXTERNAL = 65,    /* Tools > "External Tools..." - the config dialog */
    EVT_TOOL_RUN_BASE = 1200,   /* base id for the configured tools' own Tools-menu
                                 * items - EVT_TOOL_RUN_BASE + index, reserving
                                 * 1200..12xx (EXT_TOOL_MAX of them) */
    EVT_EXTTOOL_LIST = 1230,    /* the dialog's tool <listbox> */
    EVT_EXTTOOL_OK = 1231,
    EVT_EXTTOOL_CANCEL = 1232,
    EVT_EXTTOOL_ADD = 1233,
    EVT_EXTTOOL_DELETE = 1234,
    EVT_EXTTOOL_TITLE = 1235,   /* the four field <input>s */
    EVT_EXTTOOL_CMD = 1236,
    EVT_EXTTOOL_ARGS = 1237,
    EVT_EXTTOOL_DIR = 1238,
    EVT_EXTTOOL_MOVEUP = 1239,    /* reorder - the list order IS the menu order */
    EVT_EXTTOOL_MOVEDOWN = 1240,
    EVT_EXTTOOL_MACRO_BTN = 1241,  /* base id for the ">" button on each of the
                                    * Arguments/Directory rows, which opens the
                                    * macro popup for that field -
                                    * EVT_EXTTOOL_MACRO_BTN + index into
                                    * g_exttool.macro_fields, reserving 1241..1242 */
    EVT_EXTTOOL_BROWSE = 1244,     /* the Command row's "..." button - the Open
                                    * dialog in OPEN_DLG_EXTTOOL_CMD mode */
    EVT_EXTTOOL_MACRO_BASE = 1250,  /* base id for the macro popup's own items -
                                     * EVT_EXTTOOL_MACRO_BASE + index into
                                     * ext_macros[], reserving 1250..126x */
    EVT_FR_MODE = 950,        /* the panel's own Find/Replace mode-toggle button */
    EVT_FR_FIND_BTN = 951,
    EVT_FR_REPLACE_BTN = 952,
    EVT_FR_FILETYPE_BASE = 960,  /* base id for the File Types <select>'s
                                  * options - EVT_FR_FILETYPE_BASE + index;
                                  * reserves 960..96x */
    EVT_NAV_BACK = 970,     /* status bar's "Back" hotkey - see nav_back() */
    EVT_NAV_FORWARD = 971,  /* status bar's "Forward" hotkey - see nav_forward() */
    EVT_FOLDER_COPY_PATH = 974,  /* same popup's "Copy Full Path" item -
                                  * copies g_folder.dir itself (the directory
                                  * the panel is currently browsing), not a
                                  * particular row - the popup isn't opened
                                  * per-row, unlike the editor
                                  * popup's identically-labeled EVT_EDITOR_
                                  * COPY_PATH, which copies a document
                                  * window's own path */
    /* The dock popup's three items - one per side a panel can be moved to,
     * see g_dockmenu / EVT_DOCK_* handling. Kept contiguous and in
     * ui_dock_side order (LEFT/RIGHT/BOTTOM) so the handler can subtract the
     * base id instead of switching on each one. */
    EVT_DOCK_LEFT = 1000,
    EVT_DOCK_RIGHT = 1001,
    EVT_DOCK_BOTTOM = 1002,

    EVT_FOLDER_NEWFILE = 975,  /* same popup's "New File..." item - opens the
                                * dialog below (g_foldernew.modal), same one
                                * EVT_FOLDER_NEWFOLDER opens - see
                                * g_foldernew.is_folder's own doc comment for
                                * how the two share it */
    EVT_FOLDERNEW_OK = 976,   /* also the file/folder name <input>'s own id
                               * (same "Enter commits like clicking OK"
                               * convention as g_copts.input/EVT_COPTS_OK) */
    EVT_FOLDERNEW_CANCEL = 977,
    EVT_FOLDER_NEWFOLDER = 980,  /* same popup's "New Folder..." item - opens
                                  * the same dialog as EVT_FOLDER_NEWFILE */
    EVT_FOLDER_DELETE = 978,  /* same popup's "Delete" item - deletes a file
                               * or an empty subdirectory, whichever the
                               * listbox's currently *selected* row is
                               * (ui_select_get_selected(g_folder.listbox)) -
                               * same as EVT_FOLDER_COPY_PATH's own doc
                               * comment notes, the popup itself isn't opened
                               * per-row; a right-click doesn't reposition
                               * the listbox's selection the way a left-click
                               * does (see process_window's UI_TAG_LISTBOX
                               * branch in ide_ui.c, gated on
                               * s->mouse_pressed, which is left-button-
                               * only), so this deletes whatever row was last
                               * actually selected, not necessarily the one
                               * under the cursor - the confirm prompt names
                               * it so a mismatch is caught before it's too
                               * late. */
    EVT_FOLDER_DELETE_CONFIRM = 979,  /* the confirm message box's "OK" -
                                       * Cancel just closes it, id 0, same
                                       * convention as EVT_CLOSE_DISCARD's own
                                       * confirm box */
    EVT_FOLDER_ADD_TO_PROJECT = 981,  /* same popup's "Add to Project" item -
                                       * adds the listbox's currently selected
                                       * row to the open project (see
                                       * project_add_file()), same "acts on
                                       * the selection, not the row the popup
                                       * happened to open over" caveat as
                                       * EVT_FOLDER_DELETE. Only meaningful
                                       * with a project open - disabled
                                       * otherwise (see the popup's own
                                       * open-time refresh). Distinct from the
                                       * Project panel's own popup (EVT_
                                       * PROJECT_POPUP_OPEN/REMOVE) - that one
                                       * removes a file already IN the
                                       * project; this one adds one from the
                                       * Folder panel's browse view. */

    /* Project > ... - see g_project's own doc comment. New/Open reuse the
     * Open dialog (g_open) in two new dialog_modes rather than each getting
     * a bespoke picker of their own - see OPEN_DLG_PROJECT_NEW/OPEN_DLG_
     * PROJECT_OPEN below. */
    EVT_PROJECT_NEW = 1300,
    EVT_PROJECT_NEW_BROWSE = 1300 + 50,  /* Browse button in New Project dialog */
    EVT_PROJECT_NEW_OK = 1300 + 51,      /* OK button */
    EVT_PROJECT_NEW_CANCEL = 1300 + 52,  /* Cancel button */
    EVT_PROJECT_NEW_FOLDER = 1300 + 53,  /* Folder path input */
    EVT_PROJECT_NEW_NAME = 1300 + 54,    /* Project name input */
    EVT_PROJECT_NEW_HELLOWORLD = 1300 + 55, /* Hello World checkbox */
    EVT_PROJECT_OPEN = 1301,
    EVT_PROJECT_ADD_FILE = 1302,
    EVT_PROJECT_SAVE = 1304,
    EVT_PROJECT_CLOSE = 1305,
    EVT_WINDOW_PROJECT = 1306,  /* the View > "Show Project" menu item's id -
                                 * same toggle convention as EVT_WINDOW_FOLDER */
    EVT_PROJECT_LISTBOX = 1307,  /* the Project panel's own <listbox> */
    EVT_PROJECT_POPUP_OPEN = 1308,  /* the panel's right-click popup's "Open" */
    EVT_PROJECT_POPUP_REMOVE = 1309,  /* same popup's "Remove from Project" -
                                       * only removes the entry, never touches
                                       * the file on disk */
    EVT_PROJECT_POPUP_NEWFILE = 1319,  /* same popup's "New File..." - opens
                                        * the Folder panel's name dialog
                                        * (g_foldernew) in project mode: the
                                        * file is created in g_project.dir
                                        * and added to the project - see
                                        * g_foldernew.in_project */
    EVT_PROJECT_BUILD = 1310,  /* Project > "Build" - see do_project_build() */
    EVT_PROJECT_INCLUDES = 1311,  /* Project > "Include Directories..." - opens
                                   * the list dialog below (replaces the old
                                   * one-shot "Add Include Directory..." item) */
    EVT_PROJECT_INCLUDES_LISTBOX = 1312,
    EVT_PROJECT_INCLUDES_ADD = 1313,     /* opens the folder-picker (reuses
                                          * OPEN_DLG_PROJECT_ADDINCLUDE) on top
                                          * of this dialog */
    EVT_PROJECT_INCLUDES_REMOVE = 1314,  /* drops the selected row */
    EVT_PROJECT_INCLUDES_CLOSE = 1315,
    EVT_PROJECT_INCLUDES_UP = 1316,      /* moves the selected row earlier -
                                          * include directories are searched in
                                          * order, so the order is meaningful */
    EVT_PROJECT_INCLUDES_DOWN = 1317,    /* ... and later */
    EVT_PROJECT_INCLUDES_DETECT = 1338,  /* replaces the list with
                                          * detect_system_include_dirs() -
                                          * only shown for the global list */
    EVT_PROJECT_OPTIONS = 1318,  /* Project > "Options..." - same dialog as
                                  * Compile > "Options..." (EVT_COMPILE_OPTIONS)
                                  * but always against g_project.compile,
                                  * never whichever file happens to be active -
                                  * see open_compiler_options_dialog() */
    EVT_WINDOW_GIT = 1320,  /* the View > "Git Changes" menu item's id -
                             * re-raises the docked git status panel, same
                             * singleton-window convention as EVT_WINDOW_
                             * FOLDER - see git_show_panel() */
    EVT_GIT_LISTBOX = 1321,  /* the Git Changes panel's own <listbox> - see
                              * git_window_activate() */
    EVT_GIT_COMMIT_BTN = 1323,   /* the popup's "Commit" item - opens the
                                  * commit message dialog below (g_gitcommit),
                                  * see git_do_commit() */
    EVT_GIT_DISCARD_BTN = 1324,  /* "Discard" - discards the selected row's
                                  * changes, after confirming - see
                                  * git_do_discard()/EVT_GIT_DISCARD_CONFIRM */
    EVT_GIT_DISCARD_CONFIRM = 1325,  /* the confirm message box's "OK" -
                                      * same "set right before opening it,
                                      * consume+clear it here" shape as
                                      * EVT_FOLDER_DELETE_CONFIRM */
    EVT_GIT_PULL_BTN = 1326,     /* "Pull" - git_do_pull() */
    EVT_GIT_PUSH_BTN = 1327,     /* "Push" - git_do_push() */
    EVT_GITCOMMIT_OK = 1328,  /* also the commit message dialog's own <input>
                               * id - "Enter commits like clicking OK", same
                               * convention as EVT_FOLDERNEW_OK */
    EVT_GITCOMMIT_CANCEL = 1329,
    EVT_GITDIFF_PREV = 1330,  /* the diff viewer's own "< Prev Change" button -
                               * see git_diff_goto_change() */
    EVT_GITDIFF_NEXT = 1331,  /* "Next Change >" */
    EVT_GIT_CLONE_BTN = 1332,  /* popup's "Clone..." item - opens the clone
                                * URL dialog below (g_gitclone), see
                                * git_clone_start() */
    EVT_GITCLONE_OK = 1333,  /* also the clone dialog's own <input> id -
                              * "Enter clones like clicking OK", same
                              * convention as EVT_GITCOMMIT_OK */
    EVT_GITCLONE_CANCEL = 1334,
    EVT_GITCLONE_BROWSE = 1337,  /* Browse button in the Clone dialog - same
                                  * folder-picker retarget as
                                  * EVT_PROJECT_NEW_BROWSE, see
                                  * OPEN_DLG_GITCLONE_FOLDER */
    EVT_GIT_COMMITPUSH_BTN = 1335,  /* popup's "Commit All && Push" item -
                                     * same g_gitcommit dialog as
                                     * EVT_GIT_COMMIT_BTN, just with
                                     * g_pending_commit_push set first so
                                     * git_commit_confirm() pushes after a
                                     * successful commit */
    EVT_GIT_COMMITFILE_BTN = 1339,  /* popup's "Commit File" - see git_commitfile_start() */
    EVT_GIT_COMMITSTAGED_BTN = 1340,  /* popup's "Commit Staged" - see git_commitstaged_start() */
    EVT_GIT_STAGE_BTN = 1341,    /* popup's "Stage" - git_do_stage() */
    EVT_GIT_UNSTAGE_BTN = 1342,  /* popup's "Unstage" - git_do_unstage() */
    EVT_GIT_BRANCH_BTN = 1343,   /* popup's "Branch..." - opens g_gitbranch, see git_branch_start() */
    EVT_GITBRANCH_CHECKOUT = 1344,  /* also the branch listbox's own id - Enter/double-click checks out */
    EVT_GITBRANCH_NEW = 1345,    /* also the new-branch <input>'s id - Enter creates */
    EVT_GITBRANCH_CANCEL = 1346,
    EVT_GITDIFF_COPY_PATH = 1347,    /* diff viewer popup's "Copy Full Path" - g_gitdiff_path */
    EVT_GITDIFF_SHOW_FOLDER = 1348,  /* diff viewer popup's "Show My Folder" */
    EVT_GITDIFF_EDIT = 1350,  /* diff viewer popup's "Edit" - git_diff_edit() */
    EVT_GIT_COMMITSTAGEDPUSH_BTN = 1349,  /* popup's "Commit Staged && Push" - only shown while something is staged */
    EVT_GIT_SYNC_BTN = 1336,  /* popup's "Sync" item - git_do_sync() (pull
                               * then push) */
    EVT_OUTPUT_CMDLINE = 1400,  /* the Output window's command <input> - Enter runs it, see cmdline_execute() */
};

/* One row of a <menu>'s dropdown: an id/label/shortcut triple, "---" for a
 * separator (id/shortcut ignored), and an enabled flag - see add_menu()
 * below, which turns an array of these into the real ui_node tree. */
typedef struct {
    int id;
    const char* label;
    const char* shortcut;  /* NULL if none */
    int enabled;
    const char* short_help; /* status bar hint (ui_set_help), NULL if none */
    const char* help;       /* its full text for F1 */
} menu_item_spec;
#define SEP { 0, "---", NULL, 1 }

/* Build one <menu> dropdown (its items given as a menu_item_spec array) and
 * append it to `menubar`. Returns the <menu> node itself so a caller can
 * append further items by hand afterward (see the Edit menu's "Read-only"
 * item in build_screen()). */
static ui_node* add_menu(ui_node* menubar, const char* label,
                          const menu_item_spec* items, int count)
{
    ui_node* menu = ui_create_element(UI_TAG_MENU);
    ui_set_label(menu, label);
    ui_append_child(menubar, menu);

    for (int i = 0; i < count; i++)
    {
        ui_node* item = ui_create_element(UI_TAG_ITEM);
        if (strcmp(items[i].label, "---") == 0)
        {
            ui_set_separator(item, 1);
        }
        else
        {
            ui_set_id(item, items[i].id);
            ui_set_label(item, items[i].label);
            if (items[i].shortcut)
                ui_set_shortcut(item, items[i].shortcut);
            if (!items[i].enabled)
                ui_set_enabled(item, 0);
            if (items[i].short_help)
                ui_set_help(item, items[i].short_help, items[i].help);
        }
        ui_append_child(menu, item);
    }
    return menu;
}

/* Build the menu bar/dropdowns/status bar, appended once to `root` at
 * startup. The "Welcome" panel is appended to the same tree programmatically
 * in app_init() below, via the same ui_create_element/ui_append_child/
 * ui_set_* calls - both routes build the identical ui_node tree. */
/* The View menu's "Show Output"/"Show Folder"/"Show Playground" items (see
 * build_screen() below) - same forward-declared-for-build_screen()/kept-
 * current-every-frame pattern, showing
 * "[x]"/"[ ]" for whether each one's window is currently open - see refresh_view_item()/
 * window_is_shown() and their app_frame() call sites. */
/* The Tools <menu> itself - the configured External Tools are appended to
 * it as their own items, and re-appended whenever the list changes (see
 * rebuild_tools_menu()). Same forward-declared-for-build_screen() pattern
 * as the item globals around it. */
static ui_node* g_tools_menu;

/* Defined much further down, next to the External Tools list it renders -
 * forward declared so build_screen() (well above that point) can do the
 * initial fill. */
static void rebuild_tools_menu(void);

static ui_node* g_view_output_item;
static ui_node* g_view_folder_item;
static ui_node* g_view_git_item;
static ui_node* g_view_playground_item;

/* Compile > Options...' settings, gathered into one struct - see g_compile's
 * own doc comment further below for the full rationale. Declared up here
 * (ahead of its only other use) purely so g_project below can embed one of
 * its own (per-project compiler options - see g_project.compile's own doc
 * comment). */
typedef struct
{
    char options[512];    /* free-text tokens, split on whitespace at compile time */
    char output[256];     /* Compiler Options' "Output" field: the built
                           * executable's file name, "" meaning "derive it"
                           * - see out_exe_name(), which resolves that
                           * default and is the single place $(OutExe) and
                           * the debugger's own launch path both read. */
    const char* target;   /* slug for the chosen Target, or "" for none */
    const char* style;    /* slug for -style=<name>, or "" for "disabled" */
    const char* diagnostic_format; /* slug for -fdiagnostics-format=<name> */
    int no_output;         /* -no-output */
    int line_directives;   /* -line-directives */
    int fanalyzer;          /* -fanalyzer */
    int const_literal;      /* -const-literal */
    int wall;                /* -Wall */
    int unused_extern_report; /* -unused-extern-report */
    int use_cake_headers;     /* -cake-headers */
} compile_settings;

#define CAKE_PROJECT_EXT ".cakeproj"
#define CAKE_PROJECT_MAX_FILES 512
#define CAKE_PROJECT_MAX_INCLUDES 64

/* The open Project, if any - Project > New/Open Project (project_new_create/
 * project_open_file) load one; Project > Close Project (EVT_PROJECT_CLOSE)
 * clears it back to empty. Persisted as a small JSON-like ".cakeproj" file
 * (see project_save/project_load_from_file) sitting in g_project.dir -
 * files[]/include_dirs[] are always stored relative to that directory (not
 * absolute), so a project still resolves correctly after the whole tree is
 * moved or checked out somewhere else.
 *
 * Deliberately narrow: the IDE only tracks which files belong to the project
 * and where its include directories are - it doesn't know how to build one.
 * That's the compiler's job, not the IDE's, so there's no per-project build
 * step here (see do_compile, which still just compiles whichever document is
 * active, project or not). */
static struct
{
    char file_path[1024];  /* absolute path to the .cakeproj file, "" if none open */
    char dir[1024];        /* file_path's own directory - every files[]/
                             * include_dirs[] entry is relative to this */
    char name[256];

    char files[CAKE_PROJECT_MAX_FILES][512];
    int file_count;

    char include_dirs[CAKE_PROJECT_MAX_INCLUDES][512];
    int include_count;

    /* Compile > Options...' settings, but scoped to this project instead of
     * the IDE-wide default (g_compile) - persisted in the ".cakeproj" file
     * (see project_save/project_load_from_file) so a project remembers its
     * own target/style/flags/options independent of whatever else the IDE
     * is set to. Initialized from g_compile's current values when a project
     * is first created (project_new_create) - "start from what's already
     * set" rather than some separate hardcoded default. See
     * active_compile_settings(), which is what actually picks between this
     * and g_compile at compile/build time. */
    compile_settings compile;

    ui_node* window;
    ui_node* listbox;

    /* Its right-click popup - "Open" and "Remove from Project", opened over
     * `listbox` the same way g_folder.popup opens over g_folder.listbox. */
    ui_node* popup;

    /* The View menu's "Show Project" item - relabeled "[x]"/"[ ]" each frame,
     * same convention as g_view_output_item/g_view_folder_item. */
    ui_node* view_item;

    /* Project > "Include Directories..." - a small list dialog over
     * include_dirs[] (listbox + Add.../Remove/Close), same shape as the
     * External Tools dialog's own list editor. "Add..." reopens the Open
     * dialog in OPEN_DLG_PROJECT_ADDINCLUDE mode on top of this one - see
     * EVT_PROJECT_INCLUDES_ADD/project_add_include(). */
    ui_node* includes_modal;
    ui_node* includes_window;   /* title says which list is being edited */
    ui_node* includes_listbox;
    ui_node* includes_detect;   /* attached only while editing the global
                                 * list - see includes_set_detect_visible() */

    /* Project menu items that need an open project to do anything (Add
     * Existing File.../Include Directories.../Build/Save Project/Close
     * Project - everything except New/Open Project) - disabled each frame
     * when none is open, same "[x] Label" idiom's sibling for enabled state
     * as refresh_view_item elsewhere. Filled in build_screen(), read in
     * app_frame(). */
    ui_node* menu_items_requiring_project[4];

    /* file_path's last-modified time as of our own last load/save - see
     * file_watch_check(). */
    long long file_time;
} g_project;

/* The editor popup's "[x] Line Numbers" item (EVT_VIEW_LINENUMBERS) - its
 * label/enabled state is refreshed each time the popup opens (see
 * app_frame()): enabled only over a C source editor, the only kind that
 * draws a line-number gutter (see editor_gutter_width() in ide_ui.c). */
static ui_node* g_view_linenumbers_item;

/* The Compile menu's own "Build" item (id 40 / EVT_COMPILE, "Ctrl+F7") - see
 * build_screen() below) - same forward-declared-for-build_screen()/kept-
 * current-every-frame pattern as the View menu items above. Disabling the
 * top-level <menu> container itself (as opposed to this leaf item) turns out
 * not to do anything in this framework - clicking a menubar header opens its
 * dropdown unconditionally (see the menubar loop in ide_ui.c's update()),
 * and F7 is dispatched by activate_menu_shortcut() scanning each *item*'s
 * own enabled flag, never the parent menu's - so this has to be the actual
 * item, not the menu, for either route (click or F7) to actually be blocked.
 * See path_is_c_source() and this pointer's app_frame() call site. */
static ui_node* g_compile_item;

/* The Build menu's "Compile" item (EVT_COMPILE_FILE) - compiles just the
 * active file, never the project. Same forward-declared/kept-current-every-
 * frame pattern and the same path_is_c_source() condition as g_compile_item
 * above. */
static ui_node* g_compile_file_item;

/* The Edit menu's "Format" item (EVT_EDIT_FORMAT, "Ctrl+Shift+F") - same
 * forward-declared/kept-current-every-frame pattern as g_compile_item above,
 * and the same path_is_c_source() condition, since Format only makes sense
 * for a real .c file. The editor popup's own copy of this item
 * (g_editor_popup_format) is a separate node and refreshes alongside it. */
static ui_node* g_edit_format_item;
static ui_node* g_editor_popup_format;

/* The Compile menu's own "Show Generated Code" item - a separate node from
 * the editor popup's identically-labeled/identically-idd (EVT_EDITOR_SHOW_
 * OUTPUT) copy (see g_editor_popup_show_output), which already disables
 * itself for a .md file on its own popup-open refresh. This one has no such
 * open-time hook (it's a plain menubar dropdown - same reasoning as
 * g_compile_item above), so it's kept current every frame instead,
 * alongside g_compile_item and with the same path_is_c_source() condition -
 * there's nothing to show without a compiled .c to show it for. */
static ui_node* g_compile_show_output_item;

/* Debug menu items (see build_screen()'s debug_items[]) and the live
 * session driving them - see ide_debug.h. Continue/Step/Stop start
 * disabled and are kept current every frame by debug_menu_refresh(), same
 * pattern as g_compile_item above, gated on g_dbg.state instead of
 * path_is_c_source(). */
static ui_node* g_debug_start_item;
static ui_node* g_debug_stop_item;
static ui_node* g_debug_continue_item;
static ui_node* g_debug_step_over_item;
static ui_node* g_debug_step_into_item;
static struct debug_session g_dbg;

/* Docked Locals/Call Stack panel (see build_screen()'s "--- Debug Info
 * window ---" block, next to Output/Folder) - one combined listbox, not
 * two separate docked panels, since dock_layout() (ide_ui.c) only tracks
 * one window per side. Repainted by debug_info_panel_refresh() whenever
 * g_dbg.info_dirty is set (see debug_stream_poll()). */
static ui_node* g_debuginfo_window;
static ui_node* g_debuginfo_listbox;
static ui_node* g_view_debuginfo_item;

static ui_node* g_statusbar_compile_item;  /* "Compiling..." while a build
                                            * runs - see compile_status_set */

static ui_node* g_menubar;  /* the menu bar - the command line looks menu items up in it, see cmdline_menu_find */

static void build_screen(ui_node* root)
{
    ui_node* menubar = ui_create_element(UI_TAG_MENUBAR);
    g_menubar = menubar;
    ui_append_child(root, menubar);

    static const menu_item_spec file_items[] = {
        { 1, "New", NULL, 1 },
        { 2, "Open...", "Ctrl+O", 1 },
        { EVT_FILE_OPENFOLDER, "Open Folder...", NULL, 1 },
        { EVT_GIT_CLONE_BTN, "Git Clone...", NULL, 1 },
        { 3, "Save", "Ctrl+S", 1 },
        { 4, "Save As...", NULL, 1 },
        { 6, "Save all", "Ctrl+Shift+S", 1 },
        SEP,
        { EVT_GLOBAL_INCLUDES, "System Directories...", NULL, 1,
          "System Directories: the global `#include` search path, saved in `cake.json`", "# System Directories\n\nthe global `#include` search path, saved in `cake.json`\n"
          "\n"
          "Used for every file that is not part of the open project. Open it for the full explanation (F1 inside the dialog)." },
        { EVT_COMPILE_OPTIONS, "Options...", NULL, 1 },
        SEP,
        { 5, "Exit", NULL, 1 },
    };
    add_menu(menubar, "File", file_items, sizeof file_items / sizeof file_items[0]);

    static const menu_item_spec edit_items[] = {
        { EVT_EDIT_UNDO, "Undo", "Ctrl+Z", 1 },
        { EVT_EDIT_REDO, "Redo", "Ctrl+Y", 1 },
        SEP,
        { EVT_EDIT_CUT, "Cut", "Ctrl+X", 1 },
        { EVT_EDIT_COPY, "Copy", "Ctrl+C", 1 },
        { EVT_EDIT_PASTE, "Paste", "Ctrl+V", 1 },
        SEP,
        { EVT_EDIT_STRINGIFY, "Stringify", NULL, 1 },
        { EVT_EDIT_TOUPPER, "To Upper", "Ctrl+U", 1 },
        { EVT_EDIT_TOLOWER, "To Lower", "Ctrl+L", 1 },
        SEP,
        { EVT_EDIT_WORDWRAP, "Word Wrap...", "Ctrl+W", 1 },
        SEP,
        { EVT_EDIT_FORMAT, "Format", "Ctrl+Shift+F", 1 },
    };
    ui_node* edit_menu = add_menu(menubar, "Edit", edit_items, sizeof edit_items / sizeof edit_items[0]);
    g_edit_format_item = ui_find_by_id(edit_menu, EVT_EDIT_FORMAT);

    /* View > "Show Output"/"Show Folder"/"Show Playground" - previously on
     * the Window menu (see EVT_WINDOW_OUTPUT/EVT_WINDOW_FOLDER/
     * EVT_WINDOW_PLAYGROUND's own doc comments) alongside window-management
     * commands (Tile/Cascade/Close all/Refresh/Font size) that don't really
     * belong with "raise this panel" commands - moved next to Edit into
     * their own menu instead. ids are unchanged, only where they're shown. */
    static const menu_item_spec view_items[] = {
        { EVT_WINDOW_OUTPUT, "Output", NULL, 1 },
        { EVT_WINDOW_FOLDER, "Folder", NULL, 1 },
        { EVT_WINDOW_PROJECT, "Project", NULL, 1 },
        { EVT_WINDOW_GIT, "Git Changes", NULL, 1 },
        { EVT_WINDOW_PLAYGROUND, "Playground", NULL, 1,
          "Playground: a scratch C file to try something small - no project needed", "# Playground\n\na scratch C file to try something small - no project needed\n"
          "\n"
          "Always the same file, `playground.c`, kept in the IDE's own config "
          "directory, independent of any open project. It starts as a small Hello "
          "World the first time the IDE runs.\n"
          "\n"
          "Open it, edit it, **Build** (F7), **Debug** (F5). It uses the global "
          "settings (**File > Directories...** / **File > Options...**), never the "
          "open project's.\n"
          "\n"
          "Closing the window doesn't delete the file; reopening it reads the "
          "latest saved contents from disk." },
    };
    ui_node* view_menu = add_menu(menubar, "View", view_items, sizeof view_items / sizeof view_items[0]);
    /* Grabbed back out by id (add_menu builds items from the plain spec
     * array above, so it never hands back per-item pointers) so app_frame()
     * can relabel them "[x]"/"[ ] Show ..." each frame - see
     * refresh_view_item()/window_is_shown() and g_view_output_item's own
     * doc comment. */
    g_view_output_item = ui_find_by_id(view_menu, EVT_WINDOW_OUTPUT);
    g_view_folder_item = ui_find_by_id(view_menu, EVT_WINDOW_FOLDER);
    g_project.view_item = ui_find_by_id(view_menu, EVT_WINDOW_PROJECT);
    g_view_git_item = ui_find_by_id(view_menu, EVT_WINDOW_GIT);
    g_view_playground_item = ui_find_by_id(view_menu, EVT_WINDOW_PLAYGROUND);

    static const menu_item_spec search_items[] = {
        { 20, "Find...", NULL, 1 },
        { 21, "Replace...", "Ctrl+R", 1 },
        { 23, "Search Next", "F3", 1 },
        { 22, "Go to line...", "Ctrl+G", 1 },
        { EVT_SEARCH_GOTO_DEFINITION, "Go to Definition", "F12", 1 },
        SEP,
        { EVT_TOOLS_FINDREPLACE, "Find in Files...", "Ctrl+F", 1 }
    };
    add_menu(menubar, "Search", search_items, sizeof search_items / sizeof search_items[0]);

    /* Project > ... - see g_project's own doc comment for what a project is
     * (a file list + include dirs, persisted as a ".cakeproj" file). "Build"
     * only gathers the project's own .c files and hands them to the compiler
     * in one invocation (see do_project_build()) - multi-file building itself
     * (linking, etc.) is entirely the compiler's own job, not reimplemented
     * here. */
    static const menu_item_spec project_items[] = {
        { EVT_PROJECT_NEW, "New Project...", NULL, 1 },
        { EVT_PROJECT_OPEN, "Open Project...", NULL, 1 },
        SEP,
        { EVT_PROJECT_ADD_FILE, "Add Existing File...", NULL, 1 },
        { EVT_PROJECT_INCLUDES, "Include Directories...", NULL, 1 },
        { EVT_PROJECT_OPTIONS, "Options...", NULL, 1 },
        SEP,
        { EVT_PROJECT_CLOSE, "Close Project", NULL, 1 },
    };
    ui_node* project_menu = add_menu(menubar, "Project", project_items, sizeof project_items / sizeof project_items[0]);
    /* Every Project item except New/Open Project needs an open project to do
     * anything - grabbed back out by id (same reason/pattern as the View
     * menu's items in build_screen() - add_menu() never hands back per-item
     * pointers) so app_frame() can enable/disable them each frame off of
     * project_is_open() (see g_project.menu_items_requiring_project's own
     * doc comment). */
    static const int project_menu_ids_requiring_project[] = {
        EVT_PROJECT_ADD_FILE, EVT_PROJECT_INCLUDES, EVT_PROJECT_OPTIONS,
        EVT_PROJECT_CLOSE,
    };
    for (int i = 0; i < (int)(sizeof project_menu_ids_requiring_project /
                              sizeof project_menu_ids_requiring_project[0]); i++)
        g_project.menu_items_requiring_project[i] =
            ui_find_by_id(project_menu, project_menu_ids_requiring_project[i]);

    static const menu_item_spec compile_items[] = {
        { EVT_COMPILE, "Build", "F7", 1, "Compile every file of the project (or the current file) and link the executable" },
        { EVT_COMPILE_FILE, "Compile", "Ctrl+F7", 1, "Compile only the current file" },
        //{ 41, "Make", NULL, 1 },
       // { 42, "Link", NULL, 1 },
       // { 43, "Build all", NULL, 1 },
        SEP,
        { EVT_EDITOR_SHOW_OUTPUT, "Show Generated Code", NULL, 1, "Open the C89 code Cake generated for the current file" },
    };
    ui_node* compile_menu = add_menu(menubar, "Build", compile_items, sizeof compile_items / sizeof compile_items[0]);
    /* Grabbed back out by id, same reason/pattern as the View menu's items
     * just above - app_frame() needs the actual item node to disable (see
     * g_compile_item's own doc comment for why the menu container itself
     * wouldn't work). */
    g_compile_item = ui_find_by_id(compile_menu, EVT_COMPILE);
    g_compile_file_item = ui_find_by_id(compile_menu, EVT_COMPILE_FILE);
    g_compile_show_output_item = ui_find_by_id(compile_menu, EVT_EDITOR_SHOW_OUTPUT);

    /* Shortcuts match Visual Studio's own debugger keys exactly - F5 does
     * double duty as Start Debugging and Continue, the same key VS uses for
     * both, which works here because the two items are never enabled at
     * the same time (see debug_menu_refresh() below): activate_menu_
     * shortcut() only ever fires an enabled item, so F5 starts a session
     * when idle and continues one that's already stopped, with no separate
     * handling needed for "which one did the user mean". F5 previously
     * belonged to Window > Refresh (removed) to make room for this. */
    static const menu_item_spec debug_items[] = {
        { EVT_DEBUG_START, "Start Debugging", "F5", 1 },
        { EVT_DEBUG_STOP, "Stop Debugging", "Shift+F5", 0 },
        SEP,
        { EVT_DEBUG_CONTINUE, "Continue", "F5", 0 },
        { EVT_DEBUG_STEP_OVER, "Step Over", "F10", 0 },
        { EVT_DEBUG_STEP_INTO, "Step Into", "F11", 0 },
        SEP,
        { EVT_DEBUG_TOGGLE_BREAKPOINT, "Toggle Breakpoint", "F9", 1 },
        SEP,
        { EVT_WINDOW_DEBUGINFO, "Debug Info", NULL, 1 },
    };
    ui_node* debug_menu = add_menu(menubar, "Debug", debug_items, sizeof debug_items / sizeof debug_items[0]);
    /* Continue/Step/Stop start disabled (see the `0` enabled flags above) -
     * only meaningful once a debug session is actually stopped/running; see
     * debug_menu_refresh(), called every frame the same way g_compile_item
     * is kept in sync. */
    g_debug_start_item = ui_find_by_id(debug_menu, EVT_DEBUG_START);
    g_debug_stop_item = ui_find_by_id(debug_menu, EVT_DEBUG_STOP);
    g_debug_continue_item = ui_find_by_id(debug_menu, EVT_DEBUG_CONTINUE);
    g_debug_step_over_item = ui_find_by_id(debug_menu, EVT_DEBUG_STEP_OVER);
    g_debug_step_into_item = ui_find_by_id(debug_menu, EVT_DEBUG_STEP_INTO);
    /* "Debug Info" lives here rather than on View with the other panels
     * (it's only useful mid-session) but is relabeled "[x]"/"[ ]" each
     * frame exactly like them - see refresh_view_item(). */
    g_view_debuginfo_item = ui_find_by_id(debug_menu, EVT_WINDOW_DEBUGINFO);

    /* Debug > Watches is a submenu (an <item> with its own <item> children),
     * not a plain leaf - built by hand rather than through add_menu(). */
     /*
      static const menu_item_spec debug_items[] = {
         { 50, "Step over", NULL, 1 },
         { 51, "Trace into", NULL, 1 },
     };
     add_menu(menubar, "Debug", debug_items, sizeof debug_items / sizeof debug_items[0]);

     ui_node* debug_menu = ui_child_at(menubar, ui_child_count(menubar) - 1);
     ui_node* watches = ui_create_element(UI_TAG_ITEM);
     ui_set_id(watches, 52);
     ui_set_label(watches, "Watches");
     ui_append_child(debug_menu, watches);
     static const menu_item_spec watch_items[] = {
         { 53, "Add watch...", NULL, 1 },
         { 54, "Delete watch", NULL, 1 },
         { 55, "Remove all watches", NULL, 1 },
     };
     for (int i = 0; i < (int) (sizeof watch_items / sizeof watch_items[0]); i++)
     {
         ui_node* item = ui_create_element(UI_TAG_ITEM);
         ui_set_id(item, watch_items[i].id);
         ui_set_label(item, watch_items[i].label);
         ui_append_child(watches, item);
     }
  */
    static const menu_item_spec tools_items[] = {
        { EVT_TOOLS_TERMINAL, "Terminal", NULL, 1 },
    };
    g_tools_menu = add_menu(menubar, "Tools", tools_items, sizeof tools_items / sizeof tools_items[0]);
    /* Everything below "Terminal" - the configured tools, then "External
     * Tools..." last - is appended by rebuild_tools_menu(), which re-runs
     * whenever the list changes. */
    rebuild_tools_menu();

    static const menu_item_spec window_items[] = {
        { 80, "Tile", NULL, 1 },
        { 81, "Cascade", NULL, 1 },
        { 82, "Close all", NULL, 1 },
        SEP,
        { 72, "Environment...", NULL, 1 },
        SEP,
        { 85, "Font", "Ctrl++", 1 },
        { 86, "Font", "Ctrl+-", 1 },
    };
    add_menu(menubar, "Window", window_items, sizeof window_items / sizeof window_items[0]);

    static const menu_item_spec help_items[] = {
        { EVT_HELP_MANUAL, "Manual", NULL, 1 },
        //{ 91, "Topic search", NULL, 1 },
        SEP,
        { EVT_HELP_WEBSITE, "Cake Website", NULL, 1 },
        SEP,
        { 92, "About...", NULL, 1 },
    };
    add_menu(menubar, "Help", help_items, sizeof help_items / sizeof help_items[0]);

    ui_node* statusbar = ui_create_element(UI_TAG_STATUSBAR);
    ui_append_child(root, statusbar);

    /* F2/F3 were the only two of these never wired to a real action (see
     * on_ui_event - ids 201/202 have no case), so Navigate Back/Forward
     * reuses those two slots/keys instead of growing the bar. Both are
     * Shift+ variants, not plain F2/F3 - Search > Search Next already owns
     * plain F3 (see EVT_SEARCH_NEXT), and Shift+F2 just keeps the pair
     * consistent - so the labels spell out the modifier. F1 doubles as a
     * real global shortcut too (see show_hint_window()), unlike the
     * decorative F4/F5/F6/F10 hints this replaces - removed since none of
     * them were ever wired to anything real either (F5 Refresh's actual
     * shortcut lives on the Window menu, untouched by removing this hint). */
    static const struct { int id; const char* label; const char* shortcut; } hotkeys[] = {
        { EVT_HELP_CONTEXTUAL, "F1:Help", "F1" },
        { EVT_NAV_BACK, "S+F2:Back", "Shift+F2" },
        { EVT_NAV_FORWARD, "S+F3:Fwd", "Shift+F3" },
    };
    for (int i = 0; i < (int)(sizeof hotkeys / sizeof hotkeys[0]); i++)
    {
        ui_node* hk = ui_create_element(UI_TAG_HOTKEY);
        ui_set_id(hk, hotkeys[i].id);
        ui_set_label(hk, hotkeys[i].label);
        if (hotkeys[i].shortcut)
            ui_set_shortcut(hk, hotkeys[i].shortcut);
        ui_append_child(statusbar, hk);
    }

    /* Compile state - blank when idle, "Compiling..." while a build is in
     * flight (see compile_status_set). Now that the compile streams on a
     * worker thread instead of freezing the whole window, there is no other
     * cue that one is running, and Compile/F7 silently does nothing while
     * one is (see do_compile's re-entry guard) - which needs explaining
     * rather than looking broken. No id: not clickable, display only. */
    ui_node* compile_item = ui_create_element(UI_TAG_HOTKEY);
    ui_set_label(compile_item, "");
    ui_append_child(statusbar, compile_item);
    g_statusbar_compile_item = compile_item;

    /* Purely decorative - see render_statusbar() in ide_ui.c: drawn
     * right-aligned in whatever room is left past the hotkeys above, and
     * just as silently skipped if the window's too narrow to fit it. */
    ui_set_label(statusbar, "Cake " CAKE_VERSION);
}

static ui_screen* g_screen;
static ui_env* g_env;  /* stashed in app_init() so menu handlers (e.g. Window
                        * > Font Size +/-) can drive ui_env_adjust_font_size() */
static ui_node* g_about_modal;
static ui_node* g_dirs_modal;
/* Options > Environment... dialog: the modal, its two <select>s, and the
 * settings they edit. Both indices are persisted with the session, and both
 * <select>s are re-synced from them each time the modal opens (see
 * EVT_OPTIONS_ENV). */
static struct
{
    ui_node* modal;
    ui_node* theme_select;
    ui_node* font_select;   /* populated from the backend's shortlist (see
                             * ui_env_font_family_count); NULL when the
                             * backend offers none, and the row is omitted */
    int theme_index;
    int font_index;
} g_envdlg;

                                      /* Compile > Options... dialog and the settings it edits - all of it
                                       * gathered into one compile_settings struct (g_compile) rather than a
                                       * pile of separate globals, so do_compile()/save_session()/load_session()
                                       * each just touch g_compile.* instead of five individually-named
                                       * variables. options is the raw text from the "Options" field (split on
                                       * whitespace into argv tokens at compile time); target is the slug for
                                       * the chosen Target, or "" for none - both fed into do_compile()'s argv,
                                       * same as no_output/line_directives/fanalyzer below. */
/* Compile > Options... dialog - every widget in it, grouped rather than
 * spread over five separate globals (they are only ever touched together:
 * opened as a unit, read back as a unit on OK - see EVT_COMPILE_OPTIONS /
 * EVT_COPTS_OK). */
static struct
{
    ui_node* modal;
    ui_node* window;  /* retitled " Compiler Options "/" Compiler Options (Project) "
                       * each time it opens (EVT_COMPILE_OPTIONS) so it's
                       * obvious which settings - g_project.compile or
                       * g_compile - are actually being edited, see
                       * active_compile_settings() */
    ui_node* input;
    ui_node* output;  /* "Output" - the executable name, see
                       * compile_settings.output */
    ui_node* target;
    ui_node* style;   /* -style=<name> <select> */
    ui_node* diagformat; /* -fdiagnostics-format=<name> <select> */
    ui_node* flags;   /* "-no-output"/"-line-directives"/"-fanalyzer"/"-const-literal"/"-Wall"
                       * check-box GROUP - same control as Find's "Options" */
    compile_settings* editing;  /* which settings struct this open dialog is
                                 * against - g_project.compile or g_compile -
                                 * set by open_compiler_options_dialog(), read
                                 * back by EVT_COPTS_OK so it writes to the
                                 * same one it opened, no matter what happens
                                 * to project_is_open()/the active document
                                 * while the modal is up. */
    char bad_option[256];       /* the option copts_find_invalid() rejected, for the warning */
} g_copts;  /* "-no-output"/"-line-directives"/"-fanalyzer"/"-const-literal"/"-Wall" -
                                 * a check-box GROUP (multi=1),
                                 * same control as Find's "Options"
                                 * (g_find.opts). Read/written via
                                 * ui_group_get_checked/ui_group_set_checked,
                                 * only at dialog-open/OK time - see
                                 * EVT_COMPILE_OPTIONS/EVT_COPTS_OK - same as the
                                 * Target <select> above, so Cancel discards
                                 * whatever got clicked. */

/* The full-help window: F1 (or a click on the status bar) on something
 * with a hint shows that hint's whole Markdown text (ui_screen_get_hint_text)
 * in a read-only Markdown <editor> - see show_hint_window(). */
static struct
{
    ui_node* modal;
    ui_node* editor;
} g_hintwin;

static const char* g_target_slugs[] = {
    "default",
    "x86_msvc",
    "x64_msvc",
    "x86_x64_gcc",
    "macos_arm64",
};

static int target_slug_to_index(const char* slug)
{
    if (!slug || !*slug)
        return 0;

    int count = (int)(sizeof g_target_slugs / sizeof g_target_slugs[0]);
    for (int i = 0; i < count; i++)
    {
        if (strcmp(slug, g_target_slugs[i]) == 0)
            return i;
    }

    return 0;
}

/* -style=<name>'s accepted values, per options.c/lib.c's own argument
 * parser (has_prefix(argv[i], "-style") block) - "none"/"cake"/"gnu"/
 * "microsoft" are all it currently recognizes, so that's what's offered
 * here; the manual's llvm/google/chromium/mozilla/webkit entries aren't
 * wired into the compiler yet (no style_options_llvm() etc.), and passing
 * -style=llvm today just gets rejected with "Invalid style" at compile
 * time - left out until the compiler itself grows them. Index 0 is the
 * empty slug, i.e. no -style flag at all ("disabled" in the dialog). */
static const char* g_style_slugs[] = {
    "",
    "cake",
    "gnu",
    "microsoft",
};

static int style_slug_to_index(const char* slug)
{
    if (!slug)
        slug = "";

    int count = (int)(sizeof g_style_slugs / sizeof g_style_slugs[0]);
    for (int i = 0; i < count; i++)
    {
        if (strcmp(slug, g_style_slugs[i]) == 0)
            return i;
    }

    return 0;
}

/* -fdiagnostics-format=<name>'s accepted values (see options.c). They differ
 * in how each diagnostic's position is printed - "ide" prints it the same way
 * "gcc" does, and is the default here so the format the IDE reads can evolve
 * on its own:
 *
 *   ide   file.c:1:2:
 *   gcc   file.c:1:2:
 *   msvc  file.c(1,2):
 *
 * output_goto_source() parses all of them, so this only changes how the
 * Output window reads. */
static const char* g_diagformat_slugs[] = {
    "ide",
    "gcc",
    "msvc",
};

static int diagformat_slug_to_index(const char* slug)
{
    if (!slug)
        slug = "";

    int count = (int)(sizeof g_diagformat_slugs / sizeof g_diagformat_slugs[0]);
    for (int i = 0; i < count; i++)
    {
        if (strcmp(slug, g_diagformat_slugs[i]) == 0)
            return i;
    }

    return 0;
}

/* The global include directories - the "include_dirs" half of cake.json,
 * used for any file that isn't part of the open project (see
 * active_compile_settings()). Stored as absolute paths: unlike a project's
 * own list, which is relative to the project directory, this one has no
 * directory to be relative to. */
static char g_include_dirs[CAKE_PROJECT_MAX_INCLUDES][512];
static int g_include_count;

/* Which list the one Include Directories dialog is editing right now - a
 * project's own, or the global one above. Bound when the dialog is opened
 * (EVT_PROJECT_INCLUDES / EVT_GLOBAL_INCLUDES), same idea as g_copts.editing
 * for the Compiler Options dialog, so one dialog serves both and a change
 * always lands in the file that owns the list. */
static struct
{
    char (*dirs)[512];   /* the array being edited */
    int* count;
    int is_project;      /* 1 -> project_save(), 0 -> global_settings_save() */
} g_includes_editing = { NULL, NULL, 0 };

static compile_settings g_compile =
{
    .options = "",
    .target = "default",
    .style = "",
    .diagnostic_format = "ide",
    .no_output = 0,
    .line_directives = 0,
    .fanalyzer = 0,
    .const_literal = 0,
    .wall = 0,
    .unused_extern_report = 0,
    .use_cake_headers = 0,
    .output = "",
};

static ui_node* g_output_window;
static ui_node* g_output_editor;

/* The Git Changes docked panel - lists `git status --porcelain` for
 * g_folder.dir (the same directory the Folder panel browses, so both stay in
 * sync with no directory field of their own) and, on activating a row, opens
 * that file's diff in g_gitdiff_window below. Same singleton-window pattern
 * as g_output_window/g_folder above - see git_show_panel(). `root` is the
 * repo's top-level directory (from `git rev-parse --show-toplevel`, cached by
 * git_panel_refresh()) - `git status --porcelain`'s own paths are always
 * root-relative regardless of the cwd it was run from, so every later `git
 * diff -- <path>` (and the plain-file-read fallback) has to run against
 * `root`, not g_folder.dir, or those paths won't resolve when the Folder
 * panel is browsing a subdirectory of the repo. */
static struct
{
    ui_node* window;
    ui_node* listbox;
    ui_node* popup;      /* right-click popup - Commit/Discard/Pull/Push, same
                          * shape as g_folder.popup */
    /* Commit Staged, Commit Staged && Push, Unstage - in the popup only while
     * something is staged, each right after its anchor; see git_popup_refresh() */
    ui_node* staged_items[3];
    ui_node* staged_anchors[3];
    char root[1024];
} g_git;

/* The Git Changes popup's "Commit" item's own message dialog - same shared
 * name-then-OK/Cancel shape as g_foldernew above, just for a commit message
 * instead of a filename. Kept off the docked panel itself (no permanently
 * empty <input> row sitting above the listbox) - see git_commit_start()/
 * EVT_GITCOMMIT_OK. */
static struct
{
    ui_node* modal;
    ui_node* window;
    ui_node* input;
} g_gitcommit;

/* Set by EVT_GIT_COMMITPUSH_BTN right before it opens g_gitcommit's dialog
 * (same one EVT_GIT_COMMIT_BTN opens) - consumed by git_commit_confirm(),
 * which pushes after a successful commit when this is set, same "stash a
 * flag before the shared dialog, consume it in the shared confirm" shape as
 * g_pending_git_discard_path/g_pending_git_untracked. */
static int g_pending_commit_push;

/* Set by EVT_GIT_COMMITFILE_BTN: the one path git_commit_confirm() commits; empty means commit all. */
static char g_pending_commit_file[1024];

/* Set by EVT_GIT_COMMITSTAGED_BTN: git_commit_confirm() commits only the index, skipping `git add -A`. */
static int g_pending_commit_staged;

/* The Git Changes popup's "Branch..." dialog - local branches list plus a new-branch name field. */
static struct
{
    ui_node* modal;
    ui_node* window;
    ui_node* listbox;
    ui_node* input;
} g_gitbranch;

/* The Git Changes popup's "Clone..." item's own URL dialog - same shared
 * name-then-OK/Cancel shape as g_gitcommit above, just for a repository URL
 * instead of a commit message - see git_clone_start()/EVT_GITCLONE_OK. */
static struct
{
    ui_node* modal;
    ui_node* window;
    ui_node* input;
    ui_node* folder_input;
    ui_node* open_folder_check;
    char last_url[1024];       /* URL the Folder field was last suggested
                                * from - see git_clone_update_suggestion() */
    char suggested_name[256];  /* repository name last appended to the
                                * Folder field, swapped on the next change */
} g_gitclone;

/* The floating diff viewer - reused for every diff the Git Changes panel
 * opens (its content/title are replaced each time, see
 * git_window_activate()), rather than a new window per file. */
static ui_node* g_gitdiff_window;
static ui_node* g_gitdiff_editor;
static ui_node* g_gitdiff_popup;  /* right-click popup - Copy Full Path/Show My Folder */

/* Full path of the file the diff viewer shows. Kept here, not in ui_set_path() on the window, since a path there marks an editor window (is_editor_window()). */
static char g_gitdiff_path[1400];

/* 1 when the viewer holds a real diff (every row starts with ' ', '+' or '-'); 0 for an untracked file shown as-is - see git_diff_edit(). */
static int g_gitdiff_prefixed;

/* First line (1-based) of each run of changed rows in the diff viewer, found once per diff by git_diff_index_changes(). */
static int* g_gitdiff_runs;
static int g_gitdiff_run_count;
static ui_node* g_gitdiff_counter;  /* "2/23" between the Previous/Next buttons - see git_diff_counter_refresh() */
static int g_gitdiff_counter_line;  /* caret line the counter was last computed for; -1 forces a refresh */
static void git_diff_index_changes(void);

/* The persistent folder browser window - File > Open Folder... (via the
 * picker dialog, folder_select_confirm) or Window > Folder both just
 * re-raise this one singleton, same pattern as g_output_window above.
 * g_folder.dir is the directory it's currently showing (independent of the
 * Open dialog's own g_open.dir, which is only live while that dialog is
 * open). */
static struct
{
    ui_node* window;
    ui_node* listbox;
    char dir[1024];       /* directory currently shown */

    /* Its right-click popup. */
    ui_node* popup;
    ui_node* popup_add_to_project;  /* "Add to Project" - enabled/disabled
                                     * each time the popup opens, off with no
                                     * project open (see EVT_FOLDER_ADD_TO_
                                     * PROJECT's own doc comment) */
} g_folder;

/* Same popup's "New File..."/"New Folder..." items (EVT_FOLDER_NEWFILE/
 * EVT_FOLDER_NEWFOLDER) - one shared name-then-OK/Cancel dialog (same shape
 * as g_goto_modal/g_goto_input) that creates either an empty file or an
 * empty directory inside g_folder.dir (the directory the popup was opened
 * over), depending on which item opened it - see EVT_FOLDERNEW_OK. */
/* Folder panel's "New File..."/"New Folder..." dialog - one dialog serving
 * both, retitled per use. */
static struct
{
    ui_node* modal;
    ui_node* window;    /* retitled " New File "/" New Folder " */
    ui_node* input;
    int is_folder;      /* which of the two items opened it */
    int in_project;     /* opened by the Project panel's "New File..."
                         * (EVT_PROJECT_POPUP_NEWFILE) instead of the Folder
                         * panel's: the file goes into g_project.dir rather
                         * than g_folder.dir, and is added to the project
                         * (project_add_file) once created */
} g_foldernew;

/* The "Dock Left/Right/Bottom" popup, shared by every dockable panel
 * (Output, Folder, Find and Replace) rather than built once per panel - the
 * three items always mean the same thing, so only which panel they act on
 * changes. It opens on a right-click that lands on a docked panel's frame
 * (its border/title row, i.e. inside the <window> but outside every control
 * in it) - the panel's *contents* keep their own popups, which is why the
 * Output editor and the Folder listbox still show theirs.
 *
 * target is the <window> node the popup was last opened over - the same node
 * ui_set_dock() was called on at build time, i.e. child 0 of the panel's
 * wrapper, not the wrapper itself. It is only read while the popup is up.
 * items[] is indexed by ui_dock_side - 1 so the open code can check the side
 * the panel is already on. */
static struct
{
    ui_node* popup;
    ui_node* target;
    ui_node* items[3];  /* [UI_DOCK_LEFT-1], [UI_DOCK_RIGHT-1], [UI_DOCK_BOTTOM-1] */
} g_dockmenu;

/* Tools > Find and Replace - a persistent docked panel (UI_DOCK_RIGHT), same
 * singleton pattern as g_folder.window/g_output_window above: raised again
 * instead of rebuilt every time the Tools menu item fires. Its content is
 * rebuilt from scratch (fr_rebuild_content, defined near do_find_replace
 * below) each time the mode toggles between Find and Replace, since the two
 * modes show a different set of fields - the g_fr.find_text..g_fr.file_type
 * fields below are what survives that rebuild (the widgets themselves don't).
 * fr_rebuild_content() reads g_fr.panel's CURRENT rect (ui_get_rect), not
 * whatever it was created with - it's a docked window, so ui_set_dock's first
 * layout pass (dock_layout, in ide_ui.c) relocates it once shown, and a node
 * built fresh after that has no shift history of its own to inherit; only
 * nodes that already existed at the time of that move get carried along with
 * it (see set_window_rect in ide_ui.c). */
/* Tools > Find and Replace - the docked panel, its controls, and the
 * search settings they edit. One struct because the whole thing is torn
 * down and rebuilt as a unit whenever the mode flips or the panel is
 * resized (see fr_rebuild_content). */
static struct
{
    ui_node* window;        /* wrapper <modal> passed to ui_screen_show_window */
    ui_node* panel;         /* the <window> node itself */

    int last_w;             /* width as of the last fr_rebuild_content() -
                             * app_frame() compares against this each frame
                             * to notice a dock-border drag */
    int mode;               /* 0 = Find, 1 = Replace */

    ui_node* mode_btn;
    ui_node* find_input;
    ui_node* replace_input; /* NULL while in Find mode (not built) */
    ui_node* opts;          /* "Match case"/"Match whole word" - GROUP multi=1 */
    ui_node* lookin;        /* "Current File"/"Current Dir"/"Include Dir" */
    ui_node* filetypes;     /* "*.c" / "*.c;*.h" / "*.md" / "*.*" - SELECT */
    ui_node* find_btn;
    ui_node* replace_btn;   /* NULL while in Find mode (not built) */

    char find_text[256];
    char replace_text[256];
    int match_case;
    int match_word;
    int look_in;            /* index into "Look in" - see fr_look_in */
    int file_type;          /* index into "File Types" - see fr_file_type */
} g_fr = { .match_case = 1, .file_type = 2 /* FR_FILETYPE_C_H */ };
#define FR_PANEL_MIN_W 26  /* dragging the dock border narrower than this
                            * would crush the Find/Replace buttons and
                            * checkboxes past usability - enforced each frame
                            * in app_frame() rather than left to the generic
                            * dock-resize clamp (which only floors at 1) */

static ui_node* g_root;
static int g_new_count = 0;

/* Options > File > Open...'s dialog state - the directory currently being
 * browsed and the wildcard mask filtering its file rows (directories always
 * show regardless of the mask, same as a classic DOS-era file dialog). */
typedef enum { OPEN_DLG_FILE, OPEN_DLG_SAVE, OPEN_DLG_FOLDER,
               OPEN_DLG_PROJECT_NEW,     /* Save-As-style: the Name field's
                                          * dir+filename together become the
                                          * new project - see
                                          * project_new_save_activate() */
               OPEN_DLG_PROJECT_OPEN,    /* file-picker, mask "*.cakeproj" -
                                          * OK loads that project instead of
                                          * opening it as a text document */
               OPEN_DLG_PROJECT_ADDFILE, /* file-picker, mask "*.c;*.h" - OK
                                          * adds the chosen file to the open
                                          * project instead of opening it */
               OPEN_DLG_PROJECT_ADDINCLUDE, /* folder-picker, like OPEN_DLG_
                                             * FOLDER - OK adds the chosen
                                             * directory to the open project's
                                             * include_dirs instead */
               OPEN_DLG_NEWPROJECT_FOLDER, /* folder-picker, like OPEN_DLG_
                                            * FOLDER - OK drops the chosen
                                            * directory into the New Project
                                            * dialog's Folder field and
                                            * reopens that dialog */
               OPEN_DLG_EXTTOOL_CMD,       /* file-picker, defaulted to the
                                            * "Programs" filter - OK drops the
                                            * chosen path into the External
                                            * Tools dialog's Command field
                                            * (which stays open underneath)
                                            * instead of opening the file */
               OPEN_DLG_GITCLONE_FOLDER    /* folder-picker, like OPEN_DLG_
                                            * NEWPROJECT_FOLDER - OK drops the
                                            * chosen directory into the Clone
                                            * dialog's Folder field and
                                            * reopens that dialog - see
                                            * EVT_GITCLONE_BROWSE */
} open_dialog_mode;

/* The Open/Save As dialog - one dialog serving several modes (see
 * open_dialog_mode / g_open.dialog_mode), so its title, OK label and
 * filter visibility are all retargeted per use rather than duplicated. */
static struct
{
    ui_node* modal;
    ui_node* window;        /* title changes per mode */
    ui_node* name_input;
    ui_node* listbox;
    ui_node* list_label;    /* "Files" or "Folders" above the list, per mode */
    ui_node* ok;            /* label changes per mode */
    ui_node* filter;        /* "Files of type" <select> - see g_open_filters */
    ui_node* filter_label;  /* its "Type" <text> - shown/hidden together,
                             * see open_dialog_set_filter_visible */

    open_dialog_mode dialog_mode;
    char dir[1024];         /* directory being browsed */
    char mask[64];          /* active filename mask, e.g. "*.c" */
    int allow_multi;        /* 1 = the listbox picks several files at once
                             * (Ctrl/Shift click, see ui_set_multi) instead
                             * of one. Only Project > "Add Existing File..."
                             * wants that - every other mode opens/saves
                             * exactly one path - so it's derived from
                             * dialog_mode in open_dialog_refresh() and read
                             * back by the EVT_OPEN_OK handler. */
} g_open = { .dir = ".", .mask = "*.c" };

/* The Open/Save dialog's "Files of type" options, Windows-Explorer style -
 * label shown in the dropdown paired with the mask it applies to
 * g_open.mask (see mask_matches). Picking one re-filters g_open.listbox and,
 * in File-Open mode, replaces the mask portion of the Name field too (see
 * open_dialog_refresh and the EVT_OPEN_FILTER handler). Index 0 is the
 * default selected when the dialog opens (see EVT_FILE_OPEN/EVT_FILE_SAVEAS). */
typedef struct { const char* label; const char* mask; } open_filter_entry;
static const open_filter_entry g_open_filters[] = {
    { "C Source Files (*.c)",    "*.c" },
    { "Header Files (*.h)",      "*.h" },
    { "C/C++ Sources (*.c;*.h)", "*.c;*.h" },
    { "Markdown Files (*.md)",   "*.md" },
    { "Cake Project Files (*.cakeproj)", "*.cakeproj" },
    { "Programs (*.exe;*.bat;*.cmd)", "*.exe;*.bat;*.cmd" },
    { "All Files (*.*)",         "*" },
};
#define OPEN_FILTER_COUNT ((int)(sizeof g_open_filters / sizeof g_open_filters[0]))
#define CAKE_PROJECT_FILTER_INDEX 4  /* g_open_filters' own "*.cakeproj" row -
                                      * see the EVT_PROJECT_OPEN handler */
#define PROGRAM_FILTER_INDEX 5  /* its "*.exe;*.bat;*.cmd" row - see the
                                 * EVT_EXTTOOL_BROWSE handler */

/* File > Save As... and File > Open Folder... both reuse the Open dialog
 * (see save_as_activate/folder_select_confirm) - g_open.dialog_mode picks
 * which behavior its Name field/listbox/OK button follow. In save mode the
 * OK button writes the active editor to the chosen path instead of opening a
 * file; g_save_window is the editor window being saved, g_save_name the bare
 * filename shown/edited in the Name field. In folder mode the listbox shows
 * only directories and OK confirms the current one (see open_dialog_refresh/
 * activate). */
static ui_node* g_save_window;
static char g_save_name[300];
static char g_saveas_path[1024];  /* target path, pending the overwrite prompt */
static char g_project_new_path[1024];  /* Project > "New Project..."'s own
                                        * target path, pending the overwrite
                                        * prompt - see project_new_save_
                                        * activate()/project_new_create() */

/* Set right before opening the Save As dialog on behalf of a Compile (see
 * do_compile()'s untitled-file check): resumes the compile from
 * save_as_commit() once the file has actually been saved for real, since an
 * untitled window has no real path to hand the compiler yet. Cleared (without
 * compiling) if the dialog is canceled instead - see EVT_OPEN_CANCEL. Left at
 * 0 for an ordinary Save/Save As not triggered by a pending compile. */
static int g_pending_compile_after_saveas;

/* Search > Go to line...'s dialog. g_goto_pending_focus is the editor to
 * focus once the current ui_screen_update() finishes - focusing from inside
 * the event handler doesn't stick, since firing the input's Enter event then
 * blurs the focus right after we set it (see ui.c). */
static ui_node* g_goto_modal;
static ui_node* g_goto_input;
static ui_node* g_goto_pending_focus;

/* Edit > Word Wrap...'s dialog: a "Columns" <input> and a "Justify"
 * checkbox, both prefilled with the last values used (COLUMNS_DEFAULT/off
 * the first time). OK reflows the active editor's selection to that width -
 * see do_edit_wordwrap(). */
#define WORDWRAP_COLUMNS_DEFAULT 80
static ui_node* g_wordwrap_modal;
static ui_node* g_wordwrap_input;
static ui_node* g_wordwrap_justify;
static int g_wordwrap_columns = WORDWRAP_COLUMNS_DEFAULT;
static int g_wordwrap_justify_on = 0;

/* Search > Replace...'s dialog. The widget handles are kept so the OK/Change
 * All handlers can read what the user typed/checked; g_replace.target is the
 * editor window's editor captured when the dialog opened. */
/* Replace Text dialog - the modal and every control in it. */
static struct
{
    ui_node* modal;
    ui_node* find;    /* "Text to Find" <input> */
    ui_node* new_;    /* "New Text" <input> */
    ui_node* opts;    /* Options check-box group */
    ui_node* dir;     /* Direction radio group */
    ui_node* scope;   /* Scope radio group */
    ui_node* origin;  /* Origin radio group */
    ui_node* target;  /* the <editor> being searched */
} g_replace;

/* Search > Find...'s dialog - same shape as Replace minus the New Text field
 * (OK just locates and selects the next match, no replacement). */
/* Find Text dialog - same shape as g_replace above, minus the "New Text"
 * field. */
static struct
{
    ui_node* modal;
    ui_node* input;
    ui_node* opts;
    ui_node* dir;
    ui_node* scope;
    ui_node* origin;
    ui_node* target;
} g_find;

/* Right-click context menu for the editor (see ui_screen_open_popup).
 * g_editor_popup_readonly is its "Read-only" item - its label/enabled state
 * is refreshed each time the popup opens (see app_frame()), since it reflects
 * whichever editor is under the popup, not a single fixed state. */
static ui_node* g_editor_popup;
static ui_node* g_editor_popup_readonly;
static ui_node* g_editor_popup_hdrsrc;  /* "Toggle Header/Source" item */
static ui_node* g_editor_popup_compile;  /* "Compile" item - .c files only,
                                          * EVT_COMPILE_FILE: this one always
                                          * compiles just the active file */
static ui_node* g_editor_popup_show_output;  /* "Show Generated Code" item -
                                              * disabled for a .md file, see
                                              * app_frame() */
static ui_node* g_editor_popup_codeblock_copy;        /* "Copy Code Block" item */
static ui_node* g_editor_popup_codeblock_playground;  /* "Copy to Playground" item */

/* The fenced code block (see markdown_codeblock_at_line()) the right-click
 * that's currently open landed on/in, if any - a malloc'd copy captured by
 * refresh_codeblock_items() when the popup opens (the same point-in-time
 * "refresh state for whichever window/click this popup is about" the other
 * g_editor_popup_* items above use), read back by EVT_EDITOR_CODEBLOCK_COPY/
 * _PLAYGROUND's handlers when an item actually fires. NULL whenever the
 * click wasn't on a Markdown fenced code block - both items are disabled in
 * that case, so their handlers should never see it, but they still guard
 * against it (a stale click position, or the document changing between
 * the popup opening and an item firing, are all this is really protecting
 * against - see refresh_codeblock_items() itself for why this can't just be
 * recomputed from mx/my as it is there). */
static char* g_md_codeblock_text;

/* Forward declaration: the <editor> inside a document window's wrapper. */
static ui_node* editor_in_window(const ui_node* wrapper);

/* The frontmost *document* window - i.e. the last one of our real editor
 * windows (see make_editor_window) that was on top, as opposed to whatever
 * ui_screen_top_window() currently reports. That distinction matters because
 * top_window() also returns the docked Folder/Output panels whenever one of
 * them is frontmost, and editor_in_window() doesn't type-check its result -
 * it just indexes the wrapper's first child, so it happily "succeeds" on a
 * Folder window's listbox too. Save/Save As used to act on whatever
 * top_window() returned; when the Folder panel was frontmost that meant
 * retitling and overwriting *it* instead of the actual document. Refreshed
 * once per frame in app_frame() (the right-click menu instead gets its
 * target window passed straight through as the event's `param` - see
 * ui_screen_open_popup) - only editor windows carry a non-empty path (see
 * find_open_window), so that's what distinguishes a real one here. */
static ui_node* g_active_editor_window;

/* The document window a UI_CLOSE_REQUEST_ID event (see ide_ui.h) is currently
 * asking to discard-or-cancel-close for - set when that prompt is shown,
 * consumed (and cleared) by EVT_CLOSE_DISCARD if the user confirms. Left
 * alone (and simply overwritten) if a second close request fires before the
 * first is answered - message boxes are themselves modal, so at most one can
 * ever be showing at a time. */
static ui_node* g_pending_close_window;

/* The Folder panel popup's "Delete File" - the full path awaiting the
 * confirm message box's "OK" (EVT_FOLDER_DELETE_CONFIRM), same "set right
 * before showing the prompt, consumed and cleared once confirmed" shape as
 * g_pending_close_window just above, just a path instead of a window (a
 * listbox row has no ui_node of its own worth keeping around). Empty when
 * no delete is pending. */
static char g_pending_delete_path[1024];
static int g_pending_delete_is_dir;  /* set alongside g_pending_delete_path -
                                      * rmdir() vs remove() at confirm time,
                                      * see EVT_FOLDER_DELETE_CONFIRM */

/* Same "stash it before the confirm prompt, consume+clear it once confirmed"
 * shape as g_pending_delete_path above, for the Git Changes panel's
 * "Discard" button - see EVT_GIT_DISCARD_BTN/EVT_GIT_DISCARD_CONFIRM.
 * g_pending_git_untracked is set alongside it: an untracked ("??") row has
 * nothing in git to restore, so discarding it deletes the file outright
 * instead of running `git checkout --`. */
static char g_pending_git_discard_path[1024];
static int g_pending_git_untracked;

static void do_compile(void);  /* defined below */
static void do_project_build(void);  /* defined below */
static void file_watch_reload_file(void);     /* defined below */
static void file_watch_reload_project(void);  /* defined below */
static void do_goto_definition(void);  /* defined below */
static void open_playground(void);  /* defined below; called on View > "Show Playground" */
static int get_playground_file_path(char* buf, size_t cap);  /* defined below;
                                                               * used by
                                                               * EVT_EDITOR_
                                                               * CODEBLOCK_
                                                               * PLAYGROUND */
static const char* label_for_path(const char* path);  /* defined below; used wherever a
                                                        * path is reopened without going
                                                        * through open_playground() itself */

#define OPEN_MAX_ENTRIES 512

/* Options > Environment...'s Theme select offers "Ambar" (see
 * g_theme_ambar below), "Dark" and "White", which mirror Visual
 * Studio's own Dark and Light (Blue-accented) palettes, and "Nebula"
 * (see g_theme_nebula below) and "Xcode Dark" (see g_theme_xcode_dark
 * below) - together they
 * prove ui_set_theme() really does re-theme the whole running app, not
 * just newly-created widgets. */

/* Visual Studio Dark theme palette (editor #1E1E1E/#D4D4D4, chrome #2D2D30,
 * accent #007ACC). */
/* "Ambar" - dark chrome with an amber accent (#F5C242): the accent
 * carries buttons, the active border, selection and keywords, with a
 * warm grey text ramp and a gruvbox-leaning syntax palette. Replaces
 * the old "Classic" entry, which was only ever a snapshot of the
 * framework default. */
static const ui_theme g_theme_ambar = {
    /* NOTE: anything drawn ON the #F5C242 accent uses dark ink, not white -
     * white scores 1.66:1 against this amber (unreadable), dark ink 10:1.
     * Buttons are the exception: their text is light, so their hover state
     * stays grey instead of turning amber. */
    .desktop_bg = TB_RGB(0x1E, 0x1E, 0x20),

    .btn_bg = TB_RGB(0x3A, 0x3A, 0x3F),
    .btn_bg_hot = TB_RGB(0x4A, 0x4A, 0x50),
    .btn_bg_active = TB_RGB(0x7A, 0x5A, 0x12),
    .btn_fg = TB_RGB(0xE8, 0xE4, 0xDA),

    .hotkey_fg = TB_RGB(0x1E, 0x1E, 0x20),
    .hotkey_key_fg = TB_RGB(0x8A, 0x3B, 0x0A),
    .hotkey_bg = TB_RGB(0xF5, 0xC2, 0x42),
    .hotkey_fg_hot = TB_RGB(0x1E, 0x1E, 0x20),
    .hotkey_bg_hot = TB_RGB(0xFF, 0xD1, 0x66),

    .menu_fg = TB_RGB(0xE8, 0xE4, 0xDA),
    .menu_bg = TB_RGB(0x2F, 0x2F, 0x35),
    .menu_fg_sel = TB_RGB(0x1E, 0x1E, 0x20),
    .menu_bg_sel = TB_RGB(0xF5, 0xC2, 0x42),
    .menu_item_fg = TB_RGB(0xE8, 0xE4, 0xDA),
    .menu_item_bg = TB_RGB(0x26, 0x26, 0x2B),
    .menu_item_fg_hot = TB_RGB(0x1E, 0x1E, 0x20),
    .menu_item_bg_hot = TB_RGB(0xF5, 0xC2, 0x42),
    .menu_item_shortcut_fg = TB_RGB(0xA8, 0xA2, 0x94),
    .menu_item_fg_disabled = TB_RGB(0x65, 0x65, 0x65),
    .menu_border_fg = TB_RGB(0x3A, 0x3A, 0x3F),  /* dark gray, not bright white */
    .menu_border_bg = TB_RGB(0x26, 0x26, 0x2B),
    .menu_border_style = UI_BORDER_SINGLE,

    .box_fg = TB_RGB(0xE8, 0xE4, 0xDA),
    .box_bg = TB_RGB(0x26, 0x26, 0x2B),
    .box_border_style = UI_BORDER_DOUBLE,

    .window_border_fg = TB_RGB(0xE8, 0xE4, 0xDA),
    .window_border_bg = TB_RGB(0x2F, 0x2F, 0x35),
    .window_border_fg_dragging = TB_RGB(0xF5, 0xC2, 0x42),
    .window_border_fg_unfocused = TB_RGB(0x65, 0x65, 0x65),
    .window_border_style = UI_BORDER_DOUBLE,
    .window_border_style_unfocused = UI_BORDER_SINGLE,
    .window_border_style_docked = UI_BORDER_SINGLE,  /* Output/Folder panels -
                                                       * a lighter frame than
                                                       * a floating document
                                                       * window's */
    .window_border_style_docked_unfocused = UI_BORDER_SINGLE,
    .window_close_bg = TB_RGB(0xE0, 0x3E, 0x36),
    .window_fg = TB_RGB(0xE8, 0xE4, 0xDA),
    .window_bg = TB_RGB(0x26, 0x26, 0x2B),  /* VS's actual docked-panel
                                             * (Git Changes/Folder/Solution
                                             * Explorer) body color */
    .modal_border_fg = TB_RGB(0xE8, 0xE4, 0xDA),
    .modal_border_bg = TB_RGB(0x26, 0x26, 0x2B),
    .modal_border_style = UI_BORDER_DOUBLE,
    .modal_fg = TB_RGB(0xE8, 0xE4, 0xDA),
    .modal_bg = TB_RGB(0x26, 0x26, 0x2B),  /* VS's actual Find/Replace and
                                            * other dialog body color */
    .label_fg = TB_RGB(0xF5, 0xC2, 0x42),  /* the theme's amber accent */
    .scrollbar_bg = TB_RGB(0x2F, 0x2F, 0x35),  /* matches the window border */
    .scrollbar_thumb_bg = TB_RGB(0x42, 0x42, 0x42),

    .input_bg = TB_RGB(0x3C, 0x3C, 0x3C),  /* VS's actual text box/dropdown
                                            * fill - lighter than modal_bg
                                            * so fields stand out in dialogs */
    .input_bg_focus = TB_RGB(0x1E, 0x1E, 0x20),  /* darker than the neutral
                                                  * gray input_bg, so focus is
                                                  * still visible - deliberately
                                                  * NOT a blue tint, since
                                                  * input_sel_bg below (the
                                                  * caret/selection block,
                                                  * #264F78) is a similar blue
                                                  * and would nearly vanish
                                                  * against a same-hued focus
                                                  * fill */
    .input_fg = TB_RGB(0xD8, 0xD6, 0xD0),
    .input_fg_focus = TB_RGB(0xFF, 0xFF, 0xFF),
    .input_sel_bg = TB_RGB(0x5A, 0x48, 0x1C),
    .input_sel_fg = TB_RGB(0xFF, 0xFF, 0xFF),

    .editor_bg = TB_RGB(0x1E, 0x1E, 0x20),
    .editor_fg = TB_RGB(0xD8, 0xD6, 0xD0),
    .editor_keyword_fg = TB_RGB(0xF5, 0xC2, 0x42),   /* types/storage: VS blue */
    .editor_keyword2_fg = TB_RGB(0xD3, 0x86, 0x9B),  /* control flow: VS purple */
    .editor_string_fg = TB_RGB(0xB8, 0xBB, 0x6B),
    .editor_comment_fg = TB_RGB(0x7C, 0x7C, 0x74),
    .editor_lint_fg = TB_RGB(0xF5, 0xC2, 0x42),  /* same gold as
                                                  * editor_keyword_fg - stands
                                                  * out from the dim gray
                                                  * editor_comment_fg */
    .editor_linenum_fg = TB_RGB(0x85, 0x85, 0x85),  /* VS Code Dark's actual
                                                     * gutter gray */
    .editor_preproc_fg = TB_RGB(0xD3, 0x86, 0x9B),
    .editor_sel_bg = TB_RGB(0x5A, 0x48, 0x1C),  /* #264F78 - VS Dark's actual selection color */
    .editor_sel_fg = TB_RGB(0xFF, 0xFF, 0xFF),
    .editor_word_match_bg = TB_RGB(0x3A, 0x33, 0x22),  /* a warm step up from
                                                        * editor_bg, matching
                                                        * the amber accent */
    .editor_current_line_bg = TB_RGB(0x2A, 0x2A, 0x2A),  /* subtle - close to
                                                          * VS Code Dark's own
                                                          * current-line tint */
    .editor_breakpoint_fg = TB_RGB(0xF4, 0x47, 0x47),  /* VS Code's own
                                                        * breakpoint red */
    .editor_exec_line_bg = TB_RGB(0x1F, 0x3A, 0x1F),   /* subtle green step up
                                                        * from editor_bg */
    .editor_bracket_fg = {
        TB_RGB(0xFF, 0xD1, 0x66),  /* gold */
        TB_RGB(0xFF, 0xB4, 0x5A),  /* pink */
        TB_RGB(0x40, 0xE0, 0xD0),  /* turquoise */
        TB_RGB(0x8A, 0xB4, 0xF8),  /* steel blue */
    },
    .editor_tag_fg = TB_RGB(0xE7, 0x8A, 0x4E),  /* teal - same accent as this
                                                 * theme's own hotkey_key_fg,
                                                 * VS Code Dark's actual type-
                                                 * name color */
    .editor_number_fg = TB_RGB(0xB5, 0xCE, 0xA8),  /* pale green - VS Code
                                                     * Dark's actual numeric
                                                     * literal color */
    .editor_char_fg = TB_RGB(0xF0, 0x55, 0x4E),  /* rose - Atom One Dark's
                                                   * character/constant color,
                                                   * distinct from string_fg's
                                                   * tan */
    .editor_function_fg = TB_RGB(0xDC, 0xDC, 0xAA),  /* soft yellow - VS Code
                                                       * Dark's actual function-
                                                       * name color */
    .editor_output_bg = TB_RGB(0x18, 0x18, 0x18),    /* one step darker than
                                                       * editor_bg (#1E1E1E) -
                                                       * VS Code Dark's recessed
                                                       * panel look for Output */
    .editor_output_fg = TB_RGB(0xD8, 0xD6, 0xD0),    /* same as editor_fg - the
                                                       * dark panel keeps the
                                                       * plain light default */

    /* UI_SYNTAX_MARKDOWN - mirrors this theme's own C-highlighting accents
     * rather than reusing them directly, so Markdown reads as part of the
     * same VS Code Dark palette. */
    .md_heading_fg = TB_RGB(0xF5, 0xC2, 0x42),     /* same VS blue as
                                                     * editor_keyword_fg */
    .md_blockquote_fg = TB_RGB(0x7C, 0x7C, 0x74),  /* same green as
                                                     * editor_comment_fg */
    .md_code_fg = TB_RGB(0xB8, 0xBB, 0x6B),        /* same tan as
                                                     * editor_string_fg */
    .md_bold_fg = TB_RGB(0xD8, 0xD6, 0xD0),        /* same as editor_fg */
    .md_link_fg = TB_RGB(0xE9, 0xB4, 0x6A),        /* VS Code Dark's actual
                                                     * hyperlink blue */
    .editor_diff_add_bg = TB_RGB(0x16, 0x3A, 0x2E),
    .editor_diff_remove_bg = TB_RGB(0x3A, 0x1D, 0x1D),
    .md_code_bg = TB_RGB(0x28, 0x2C, 0x34),        /* slate - subtly lighter/
                                                     * cooler than editor_bg
                                                     * and distinct from
                                                     * editor_current_line_bg,
                                                     * GitHub dark's own code-
                                                     * block tint */

    /* <listbox> - same body colors as <editor>/<input> rather than the
     * classic theme's cyan, selection reuses the same #007ACC accent as
     * every other "selected" state in this theme. */
    .listbox_fg = TB_RGB(0x96, 0x94, 0x8E),  /* deliberately dimmer than
                                              * editor_fg: the Folder panel
                                              * is the biggest list on
                                              * screen and shouldn't pull
                                              * attention off the code */
    .listbox_bg = TB_RGB(0x1E, 0x1E, 0x20),
    .listbox_sel_fg = TB_RGB(0x1E, 0x1E, 0x20),
    .listbox_sel_bg = TB_RGB(0xF5, 0xC2, 0x42),
    .listbox_sel_inactive_fg = TB_RGB(0xD8, 0xD6, 0xD0),
    .listbox_sel_inactive_bg = TB_RGB(0x3A, 0x3A, 0x3F),  /* VS's own muted gray */

    /* Project panel file-type markers - see ui_theme's own doc comment. */
    .project_icon_c_fg = TB_RGB(0x56, 0x9C, 0xD6),   /* VS blue */
    .project_icon_h_fg = TB_RGB(0xB5, 0xCE, 0xA8),   /* muted green */
    .project_icon_md_fg = TB_RGB(0xE9, 0xB4, 0x6A),  /* matches this theme's
                                                       * own md_link_fg */

    /* <editor> inline diagnostics - VS Code Dark's actual error/warning/info
     * squiggle colors, so they read as authentically part of this theme. */
    .diag_error_fg = TB_RGB(0xF4, 0x47, 0x47),
    .diag_warning_fg = TB_RGB(0xCC, 0xA7, 0x00),
    .diag_info_fg = TB_RGB(0x37, 0x94, 0xFF),
    /* Each fg at 18% over editor_bg - left unset these are 0, a black bar. */
    .diag_error_bg = TB_RGB(0x45, 0x25, 0x27),
    .diag_warning_bg = TB_RGB(0x3D, 0x37, 0x1A),
    .diag_info_bg = TB_RGB(0x22, 0x33, 0x48),
};

static const ui_theme g_theme_dark = {
    .desktop_bg = TB_RGB(0x1E, 0x1E, 0x1E),

    .btn_bg = TB_RGB(0x3F, 0x3F, 0x46),
    .btn_bg_hot = TB_RGB(0x00, 0x7A, 0xCC),
    .btn_bg_active = TB_RGB(0x00, 0x5A, 0x9E),
    .btn_fg = TB_RGB(0xF1, 0xF1, 0xF1),

    .hotkey_fg = TB_RGB(0xF1, 0xF1, 0xF1),
    .hotkey_key_fg = TB_RGB(0x4E, 0xC9, 0xB0),
    .hotkey_bg = TB_RGB(0x00, 0x7A, 0xCC),
    .hotkey_fg_hot = TB_RGB(0xFF, 0xFF, 0xFF),
    .hotkey_bg_hot = TB_RGB(0x1C, 0x97, 0xEA),

    .menu_fg = TB_RGB(0xF1, 0xF1, 0xF1),
    .menu_bg = TB_RGB(0x2D, 0x2D, 0x30),
    .menu_fg_sel = TB_RGB(0xFF, 0xFF, 0xFF),
    .menu_bg_sel = TB_RGB(0x00, 0x7A, 0xCC),
    .menu_item_fg = TB_RGB(0xF1, 0xF1, 0xF1),
    .menu_item_bg = TB_RGB(0x25, 0x25, 0x26),
    .menu_item_fg_hot = TB_RGB(0xFF, 0xFF, 0xFF),
    .menu_item_bg_hot = TB_RGB(0x00, 0x7A, 0xCC),
    .menu_item_shortcut_fg = TB_RGB(0x9C, 0xDC, 0xFE),
    .menu_item_fg_disabled = TB_RGB(0x65, 0x65, 0x65),
    .menu_border_fg = TB_RGB(0x3F, 0x3F, 0x46),  /* dark gray, not bright white */
    .menu_border_bg = TB_RGB(0x25, 0x25, 0x26),
    .menu_border_style = UI_BORDER_SINGLE,

    .box_fg = TB_RGB(0xF1, 0xF1, 0xF1),
    .box_bg = TB_RGB(0x25, 0x25, 0x26),
    .box_border_style = UI_BORDER_DOUBLE,

    .window_border_fg = TB_RGB(0xF1, 0xF1, 0xF1),
    .window_border_bg = TB_RGB(0x2D, 0x2D, 0x30),
    .window_border_fg_dragging = TB_RGB(0x00, 0x7A, 0xCC),
    .window_border_fg_unfocused = TB_RGB(0x65, 0x65, 0x65),
    .window_border_style = UI_BORDER_DOUBLE,
    .window_border_style_unfocused = UI_BORDER_SINGLE,
    .window_border_style_docked = UI_BORDER_SINGLE,  /* Output/Folder panels -
                                                       * a lighter frame than
                                                       * a floating document
                                                       * window's */
    .window_border_style_docked_unfocused = UI_BORDER_SINGLE,
    .window_close_bg = TB_RGB(0xE8, 0x11, 0x23),
    .window_fg = TB_RGB(0xF1, 0xF1, 0xF1),
    .window_bg = TB_RGB(0x25, 0x25, 0x26),  /* VS's actual docked-panel
                                             * (Git Changes/Folder/Solution
                                             * Explorer) body color */
    .modal_border_fg = TB_RGB(0xF1, 0xF1, 0xF1),
    .modal_border_bg = TB_RGB(0x25, 0x25, 0x26),
    .modal_border_style = UI_BORDER_DOUBLE,
    .modal_fg = TB_RGB(0xF1, 0xF1, 0xF1),
    .modal_bg = TB_RGB(0x25, 0x25, 0x26),  /* VS's actual Find/Replace and
                                            * other dialog body color */
    .label_fg = COLOR_YELLOW,
    .scrollbar_bg = TB_RGB(0x2D, 0x2D, 0x30),  /* matches the window border */
    .scrollbar_thumb_bg = TB_RGB(0x42, 0x42, 0x42),

    .input_bg = TB_RGB(0x3C, 0x3C, 0x3C),  /* VS's actual text box/dropdown
                                            * fill - lighter than modal_bg
                                            * so fields stand out in dialogs */
    .input_bg_focus = TB_RGB(0x1E, 0x1E, 0x1E),  /* darker than the neutral
                                                  * gray input_bg, so focus is
                                                  * still visible - deliberately
                                                  * NOT a blue tint, since
                                                  * input_sel_bg below (the
                                                  * caret/selection block,
                                                  * #264F78) is a similar blue
                                                  * and would nearly vanish
                                                  * against a same-hued focus
                                                  * fill */
    .input_fg = TB_RGB(0xD4, 0xD4, 0xD4),
    .input_fg_focus = TB_RGB(0xFF, 0xFF, 0xFF),
    .input_sel_bg = TB_RGB(0x26, 0x4F, 0x78),
    .input_sel_fg = TB_RGB(0xFF, 0xFF, 0xFF),

    .editor_bg = TB_RGB(0x1E, 0x1E, 0x1E),
    .editor_fg = TB_RGB(0xD4, 0xD4, 0xD4),
    .editor_keyword_fg = TB_RGB(0x56, 0x9C, 0xD6),   /* types/storage: VS blue */
    .editor_keyword2_fg = TB_RGB(0xC5, 0x86, 0xC0),  /* control flow: VS purple */
    .editor_string_fg = TB_RGB(0xCE, 0x91, 0x78),
    .editor_comment_fg = TB_RGB(0x6A, 0x99, 0x55),
    .editor_lint_fg = TB_RGB(0x56, 0x9C, 0xD6),  /* same blue as
                                                  * editor_keyword_fg - stands
                                                  * out from the green
                                                  * editor_comment_fg */
    .editor_linenum_fg = TB_RGB(0x85, 0x85, 0x85),  /* VS Code Dark's actual
                                                     * gutter gray */
    .editor_preproc_fg = TB_RGB(0xC5, 0x86, 0xC0),
    .editor_sel_bg = TB_RGB(0x26, 0x4F, 0x78),  /* #264F78 - VS Dark's actual selection color */
    .editor_sel_fg = TB_RGB(0xFF, 0xFF, 0xFF),
    .editor_word_match_bg = TB_RGB(0x33, 0x3A, 0x40),
    .editor_current_line_bg = TB_RGB(0x2A, 0x2A, 0x2A),  /* subtle - close to
                                                          * VS Code Dark's own
                                                          * current-line tint */
    .editor_breakpoint_fg = TB_RGB(0xF4, 0x47, 0x47),  /* VS Code's own
                                                        * breakpoint red */
    .editor_exec_line_bg = TB_RGB(0x1F, 0x3A, 0x1F),   /* subtle green step up
                                                        * from editor_bg */
    .editor_bracket_fg = {
        TB_RGB(0xFF, 0xD7, 0x00),  /* gold */
        TB_RGB(0xFF, 0x6A, 0xC1),  /* pink */
        TB_RGB(0x40, 0xE0, 0xD0),  /* turquoise */
        TB_RGB(0x8A, 0xB4, 0xF8),  /* steel blue */
    },
    .editor_tag_fg = TB_RGB(0x4E, 0xC9, 0xB0),  /* teal - same accent as this
                                                 * theme's own hotkey_key_fg,
                                                 * VS Code Dark's actual type-
                                                 * name color */
    .editor_number_fg = TB_RGB(0xB5, 0xCE, 0xA8),  /* pale green - VS Code
                                                     * Dark's actual numeric
                                                     * literal color */
    .editor_char_fg = TB_RGB(0xE0, 0x6C, 0x75),  /* rose - Atom One Dark's
                                                   * character/constant color,
                                                   * distinct from string_fg's
                                                   * tan */
    .editor_function_fg = TB_RGB(0xDC, 0xDC, 0xAA),  /* soft yellow - VS Code
                                                       * Dark's actual function-
                                                       * name color */
    .editor_output_bg = TB_RGB(0x18, 0x18, 0x18),    /* one step darker than
                                                       * editor_bg (#1E1E1E) -
                                                       * VS Code Dark's recessed
                                                       * panel look for Output */
    .editor_output_fg = TB_RGB(0xD4, 0xD4, 0xD4),    /* same as editor_fg - the
                                                       * dark panel keeps the
                                                       * plain light default */

    /* UI_SYNTAX_MARKDOWN - mirrors this theme's own C-highlighting accents
     * rather than reusing them directly, so Markdown reads as part of the
     * same VS Code Dark palette. */
    .md_heading_fg = TB_RGB(0x56, 0x9C, 0xD6),     /* same VS blue as
                                                     * editor_keyword_fg */
    .md_blockquote_fg = TB_RGB(0x6A, 0x99, 0x55),  /* same green as
                                                     * editor_comment_fg */
    .md_code_fg = TB_RGB(0xCE, 0x91, 0x78),        /* same tan as
                                                     * editor_string_fg */
    .md_bold_fg = TB_RGB(0xD4, 0xD4, 0xD4),        /* same as editor_fg */
    .md_link_fg = TB_RGB(0x3D, 0xA8, 0xF5),        /* VS Code Dark's actual
                                                     * hyperlink blue */
    .editor_diff_add_bg = TB_RGB(0x16, 0x3A, 0x2E),
    .editor_diff_remove_bg = TB_RGB(0x3A, 0x1D, 0x1D),
    .md_code_bg = TB_RGB(0x28, 0x2C, 0x34),        /* slate - subtly lighter/
                                                     * cooler than editor_bg
                                                     * and distinct from
                                                     * editor_current_line_bg,
                                                     * GitHub dark's own code-
                                                     * block tint */

    /* <listbox> - same body colors as <editor>/<input> rather than the
     * classic theme's cyan, selection reuses the same #007ACC accent as
     * every other "selected" state in this theme. */
    .listbox_fg = TB_RGB(0x93, 0x93, 0x93),  /* dimmer than editor_fg - see
                                              * the Ambar theme's own note */
    .listbox_bg = TB_RGB(0x1E, 0x1E, 0x1E),
    .listbox_sel_fg = TB_RGB(0xFF, 0xFF, 0xFF),
    .listbox_sel_bg = TB_RGB(0x00, 0x7A, 0xCC),
    .listbox_sel_inactive_fg = TB_RGB(0xD4, 0xD4, 0xD4),
    .listbox_sel_inactive_bg = TB_RGB(0x3F, 0x3F, 0x46),  /* VS's own muted gray */

    /* Project panel file-type markers - see ui_theme's own doc comment. */
    .project_icon_c_fg = TB_RGB(0x56, 0x9C, 0xD6),   /* VS blue, matches this
                                                       * theme's own
                                                       * editor_keyword_fg */
    .project_icon_h_fg = TB_RGB(0xB5, 0xCE, 0xA8),   /* muted green */
    .project_icon_md_fg = TB_RGB(0x3D, 0xA8, 0xF5),  /* matches this theme's
                                                       * own md_link_fg */

    /* <editor> inline diagnostics - VS Code Dark's actual error/warning/info
     * squiggle colors, so they read as authentically part of this theme. */
    .diag_error_fg = TB_RGB(0xF4, 0x47, 0x47),
    .diag_warning_fg = TB_RGB(0xCC, 0xA7, 0x00),
    .diag_info_fg = TB_RGB(0x37, 0x94, 0xFF),
    /* Each fg at 18% over editor_bg - left unset these are 0, a black bar. */
    .diag_error_bg = TB_RGB(0x45, 0x25, 0x25),
    .diag_warning_bg = TB_RGB(0x3D, 0x37, 0x19),
    .diag_info_bg = TB_RGB(0x22, 0x33, 0x46),
};

/* Visual Studio Light theme palette (editor #FFFFFF/#1E1E1E, chrome #F3F3F3,
 * accent #007ACC/#CCE8FF selection). */
static const ui_theme g_theme_white = {
    .desktop_bg = TB_RGB(0xEA, 0xEA, 0xEA),

    .btn_bg = TB_RGB(0xE1, 0xE1, 0xE1),
    .btn_bg_hot = TB_RGB(0xCC, 0xE4, 0xF7),
    .btn_bg_active = TB_RGB(0x99, 0xD1, 0xFF),
    .btn_fg = TB_RGB(0x00, 0x00, 0x00),

    .hotkey_fg = TB_RGB(0x1E, 0x1E, 0x1E),
    .hotkey_key_fg = TB_RGB(0x00, 0x7A, 0xCC),
    .hotkey_bg = TB_RGB(0xCC, 0xE8, 0xFF),
    .hotkey_fg_hot = TB_RGB(0x00, 0x00, 0x00),
    .hotkey_bg_hot = TB_RGB(0x99, 0xD1, 0xFF),

    .menu_fg = TB_RGB(0x1E, 0x1E, 0x1E),
    .menu_bg = TB_RGB(0xF3, 0xF3, 0xF3),
    /* #0078D7 - the Windows accent blue, with white ink on it (black would
     * score 4.1:1 against it, white 4.7:1, and white is what Windows itself
     * uses for a selected menu row). */
    .menu_fg_sel = TB_RGB(0xFF, 0xFF, 0xFF),
    .menu_bg_sel = TB_RGB(0x00, 0x78, 0xD7),
    .menu_item_fg = TB_RGB(0x00, 0x00, 0x00),
    .menu_item_bg = TB_RGB(0xFF, 0xFF, 0xFF),
    .menu_item_fg_hot = TB_RGB(0xFF, 0xFF, 0xFF),
    .menu_item_bg_hot = TB_RGB(0x00, 0x78, 0xD7),
    .menu_item_shortcut_fg = TB_RGB(0x00, 0x66, 0x99),
    .menu_item_fg_disabled = TB_RGB(0xA0, 0xA0, 0xA0),
    .menu_border_fg = TB_RGB(0xCC, 0xCC, 0xCC),  /* light gray, not near-black */
    .menu_border_bg = TB_RGB(0xFF, 0xFF, 0xFF),
    .menu_border_style = UI_BORDER_DOUBLE,

    .box_fg = TB_RGB(0x1E, 0x1E, 0x1E),
    .box_bg = TB_RGB(0xFF, 0xFF, 0xFF),
    .box_border_style = UI_BORDER_DOUBLE,

    /* #99B4D1 - the classic Windows window-frame blue-gray. Near-black
     * (#1E1E1E) frames around every window and dialog were the loudest thing
     * on screen in a light theme; this recedes into the chrome the way a real
     * light UI's frames do, while still separating a window from the desktop
     * behind it. */
    .window_border_fg = TB_RGB(0x99, 0xB4, 0xD1),
    .window_border_bg = TB_RGB(0xF0, 0xF0, 0xF0),
    .window_border_fg_dragging = TB_RGB(0x00, 0x78, 0xD7),
    .window_border_fg_unfocused = TB_RGB(0xC8, 0xC8, 0xC8),
    .window_border_style = UI_BORDER_DOUBLE,
    .window_border_style_unfocused = UI_BORDER_SINGLE,
    .window_border_style_docked = UI_BORDER_DOUBLE,
    .window_border_style_docked_unfocused = UI_BORDER_SINGLE,
    .window_close_bg = TB_RGB(0xE8, 0x11, 0x23),
    .window_fg = TB_RGB(0x00, 0x00, 0x00),
    .window_bg = TB_RGB(0xF0, 0xF0, 0xF0),
    .modal_border_fg = TB_RGB(0x99, 0xB4, 0xD1),
    .modal_border_bg = TB_RGB(0xF0, 0xF0, 0xF0),
    .modal_border_style = UI_BORDER_DOUBLE,
    .modal_fg = TB_RGB(0x1E, 0x1E, 0x1E),
    .modal_bg = TB_RGB(0xF0, 0xF0, 0xF0),
    .label_fg = TB_RGB(0x00, 0x00, 0x00),  /* plain black, not an accent - the
                                            * yellow the other two themes use
                                            * scores ~1.3:1 on this light
                                            * chrome, i.e. invisible. Kept a
                                            * shade off modal_fg (#1E1E1E)
                                            * deliberately: retheme_color()
                                            * tells a baked label apart from
                                            * baked body text by its value
                                            * alone, so the two slots must
                                            * never hold the same color. */
    .scrollbar_bg = TB_RGB(0xF0, 0xF0, 0xF0),  /* matches the window border */
    .scrollbar_thumb_bg = TB_RGB(0xC2, 0xC2, 0xC2),

    .input_bg = TB_RGB(0xFF, 0xFF, 0xFF),
    .input_bg_focus = TB_RGB(0x00, 0x33, 0x66),  /* dark navy - unlike the
                                                  * other themes, White's own
                                                  * unfocused fill is already
                                                  * plain white, so a light
                                                  * focus tint wouldn't read
                                                  * as a change (and hides
                                                  * the caret); going dark
                                                  * instead keeps the caret/
                                                  * selection block (light
                                                  * blue) visible against it */
    .input_fg = TB_RGB(0x1E, 0x1E, 0x1E),
    .input_fg_focus = TB_RGB(0xFF, 0xFF, 0xFF),  /* white - must stay
                                                  * readable against the dark
                                                  * input_bg_focus above,
                                                  * unlike the black used
                                                  * against the plain white
                                                  * unfocused fill */
    /* #ADD6FF - Visual Studio Light's actual text-selection color (also used
     * by <editor>, which reuses input_sel_*). Pale, so the selected text's
     * own color stays dark instead of the white used against the old darker
     * blue - otherwise it'd be unreadable against this light a background. */
    .input_sel_bg = TB_RGB(0xAD, 0xD6, 0xFF),
    .input_sel_fg = TB_RGB(0x00, 0x00, 0x00),

    .editor_bg = TB_RGB(0xFF, 0xFF, 0xFF),
    .editor_fg = TB_RGB(0x1E, 0x1E, 0x1E),
    .editor_keyword_fg = TB_RGB(0x00, 0x00, 0xFF),   /* types/storage: blue */
    .editor_keyword2_fg = TB_RGB(0xAF, 0x00, 0xDB),  /* control flow: purple */
    .editor_string_fg = TB_RGB(0xA3, 0x15, 0x15),
    .editor_comment_fg = TB_RGB(0x00, 0x80, 0x00),
    .editor_lint_fg = TB_RGB(0x00, 0x00, 0xFF),  /* same blue as
                                                  * editor_keyword_fg - stands
                                                  * out from the green
                                                  * editor_comment_fg */
    .editor_linenum_fg = TB_RGB(0x23, 0x78, 0x93),  /* teal-blue - VS Code
                                                     * Light+'s actual line-
                                                     * number color (matches the
                                                     * playground's gutter) */
    .editor_preproc_fg = TB_RGB(0x80, 0x00, 0x80),
    .editor_sel_bg = TB_RGB(0xAD, 0xD6, 0xFF),  /* #ADD6FF - VS Light's actual selection color */
    .editor_sel_fg = TB_RGB(0x00, 0x00, 0x00),
    .editor_word_match_bg = TB_RGB(0xE0, 0xE8, 0xF0),  /* pale blue-gray -
                                                        * reads against the
                                                        * white page without
                                                        * competing with the
                                                        * #ADD6FF selection */
    .editor_current_line_bg = TB_RGB(0xF0, 0xF0, 0xF0),  /* subtle - close to
                                                          * VS Code Light's own
                                                          * current-line tint */
    .editor_breakpoint_fg = TB_RGB(0xE5, 0x14, 0x00),  /* VS Code's own
                                                        * breakpoint red */
    .editor_exec_line_bg = TB_RGB(0xDD, 0xF4, 0xDD),   /* pale green - reads
                                                        * against the white
                                                        * page like word_match_bg
                                                        * reads against blue */
    .editor_bracket_fg = {
        TB_RGB(0x79, 0x5E, 0x26),  /* gold/brown */
        TB_RGB(0x26, 0x7F, 0x99),  /* teal */
        TB_RGB(0xB5, 0x60, 0x2B),  /* rust */
        TB_RGB(0x5C, 0x6B, 0xC0),  /* indigo */
    },
    .editor_tag_fg = TB_RGB(0x8B, 0x45, 0x13),  /* saddle brown - struct/
                                                 * union/enum tag names */
    .editor_number_fg = TB_RGB(0x09, 0x86, 0x58),  /* teal-green - VS Code
                                                     * Light's actual numeric
                                                     * literal color */
    .editor_char_fg = TB_RGB(0xAF, 0x00, 0x75),  /* deep pink - distinct from
                                                   * string_fg's dark red and
                                                   * keyword2_fg's purple */
    .editor_function_fg = TB_RGB(0x79, 0x5E, 0x26),  /* dark gold - VS Code
                                                       * Light's actual function-
                                                       * name color */
    .editor_output_bg = TB_RGB(0xF3, 0xF3, 0xF3),    /* light gray - VS Code
                                                       * Light's panel/chrome
                                                       * color, so the Output
                                                       * panel reads as recessed
                                                       * against the white
                                                       * document (editor_bg) */
    .editor_output_fg = TB_RGB(0x1A, 0x73, 0xE8),    /* bright azure blue - the
                                                       * playground's Output text
                                                       * color; also the readable
                                                       * fallback for bright ANSI
                                                       * colors that would vanish
                                                       * on the gray
                                                       * editor_output_bg */

    /* UI_SYNTAX_MARKDOWN - mirrors this theme's own C-highlighting accents
     * rather than reusing them directly, so Markdown reads as part of the
     * same VS Code Light palette. */
    .md_heading_fg = TB_RGB(0x00, 0x00, 0xFF),     /* same blue as
                                                     * editor_keyword_fg */
    .md_blockquote_fg = TB_RGB(0x00, 0x80, 0x00),  /* same green as
                                                     * editor_comment_fg */
    .md_code_fg = TB_RGB(0xA3, 0x15, 0x15),        /* same dark red as
                                                     * editor_string_fg */
    .md_bold_fg = TB_RGB(0x1E, 0x1E, 0x1E),        /* same as editor_fg */
    .md_link_fg = TB_RGB(0x00, 0x66, 0xCC),        /* VS Code Light's actual
                                                     * hyperlink blue */
    .editor_diff_add_bg = TB_RGB(0xE6, 0xFF, 0xEC),
    .editor_diff_remove_bg = TB_RGB(0xFF, 0xEB, 0xE9),
    .md_code_bg = TB_RGB(0xF6, 0xF8, 0xFA),        /* GitHub Light's actual
                                                     * code-block gray -
                                                     * distinct from both
                                                     * editor_bg and editor_
                                                     * current_line_bg */

    /* <listbox> - same body colors as <editor>/<input> rather than the
     * classic theme's cyan, selection reuses the same #CCE8FF accent as
     * every other "selected" state in this theme. */
    .listbox_fg = TB_RGB(0x60, 0x60, 0x60),  /* dimmer than the editor's own
                                              * #1E1E1E body text - the
                                              * Folder panel is the biggest
                                              * listbox on screen and
                                              * shouldn't pull attention off
                                              * the code */
    .listbox_bg = TB_RGB(0xF3, 0xF3, 0xF3),  /* VS Code Light's actual side
                                              * bar gray rather than the
                                              * editor's plain white, so the
                                              * panel recedes next to the
                                              * <editor> it sits beside */
    .listbox_sel_fg = TB_RGB(0x00, 0x00, 0x00),
    .listbox_sel_bg = TB_RGB(0xCC, 0xE8, 0xFF),
    .listbox_sel_inactive_fg = TB_RGB(0x1E, 0x1E, 0x1E),
    .listbox_sel_inactive_bg = TB_RGB(0xE0, 0xE0, 0xE0),

    /* Project panel file-type markers - see ui_theme's own doc comment. */
    .project_icon_c_fg = TB_RGB(0x00, 0x00, 0xFF),   /* matches this theme's
                                                       * own editor_keyword_fg */
    .project_icon_h_fg = TB_RGB(0x00, 0x80, 0x00),   /* dark green, legible
                                                       * against a light bg */
    .project_icon_md_fg = TB_RGB(0x00, 0x66, 0xCC),  /* matches this theme's
                                                       * own md_link_fg */

    /* <editor> inline diagnostics - VS Code Light's actual error/warning/
     * info squiggle colors, so they read as authentically part of this
     * theme. */
    .diag_error_fg = TB_RGB(0xE5, 0x14, 0x00),
    .diag_warning_fg = TB_RGB(0xBF, 0x88, 0x03),
    .diag_info_fg = TB_RGB(0x1A, 0x85, 0xFF),
    /* Pale tints of each fg - left unset these are 0, a black bar on a white editor. */
    .diag_error_bg = TB_RGB(0xFD, 0xE7, 0xE9),
    .diag_warning_bg = TB_RGB(0xFF, 0xF4, 0xCE),
    .diag_info_bg = TB_RGB(0xE5, 0xF1, 0xFB),
};

/* "Nebula" - a deep indigo night palette (editor #1A1B26, text #C0CAF5,
 * accent #7AA2F7): violet keywords, blue function names, green strings and
 * orange literals, with the whole chrome tinted the same indigo instead of
 * a neutral gray. */
static const ui_theme g_theme_nebula = {
    /* NOTE: anything drawn ON the #7AA2F7 accent uses the dark #1A1B26 ink,
     * not white - white scores 2.4:1 against this blue, the dark ink 8.7:1. */
    .desktop_bg = TB_RGB(0x16, 0x16, 0x1E),

    .btn_bg = TB_RGB(0x2A, 0x2E, 0x40),
    .btn_bg_hot = TB_RGB(0x3B, 0x42, 0x61),
    .btn_bg_active = TB_RGB(0x7A, 0xA2, 0xF7),
    .btn_fg = TB_RGB(0xC0, 0xCA, 0xF5),

    .hotkey_fg = TB_RGB(0x1A, 0x1B, 0x26),
    .hotkey_key_fg = TB_RGB(0x8C, 0x2E, 0x2E),
    .hotkey_bg = TB_RGB(0x7A, 0xA2, 0xF7),
    .hotkey_fg_hot = TB_RGB(0x1A, 0x1B, 0x26),
    .hotkey_bg_hot = TB_RGB(0x9E, 0xBC, 0xFF),

    .menu_fg = TB_RGB(0xC0, 0xCA, 0xF5),
    .menu_bg = TB_RGB(0x1F, 0x22, 0x33),
    .menu_fg_sel = TB_RGB(0x1A, 0x1B, 0x26),
    .menu_bg_sel = TB_RGB(0x7A, 0xA2, 0xF7),
    .menu_item_fg = TB_RGB(0xC0, 0xCA, 0xF5),
    .menu_item_bg = TB_RGB(0x1F, 0x22, 0x33),
    .menu_item_fg_hot = TB_RGB(0x1A, 0x1B, 0x26),
    .menu_item_bg_hot = TB_RGB(0x7A, 0xA2, 0xF7),
    .menu_item_shortcut_fg = TB_RGB(0x7D, 0x86, 0xA8),
    .menu_item_fg_disabled = TB_RGB(0x56, 0x5F, 0x89),
    .menu_border_fg = TB_RGB(0x3B, 0x42, 0x61),
    .menu_border_bg = TB_RGB(0x1F, 0x22, 0x33),
    .menu_border_style = UI_BORDER_SINGLE,

    .box_fg = TB_RGB(0xC0, 0xCA, 0xF5),
    .box_bg = TB_RGB(0x1F, 0x22, 0x33),
    .box_border_style = UI_BORDER_DOUBLE,

    .window_border_fg = TB_RGB(0xC0, 0xCA, 0xF5),
    .window_border_bg = TB_RGB(0x1F, 0x22, 0x33),
    .window_border_fg_dragging = TB_RGB(0x7A, 0xA2, 0xF7),
    .window_border_fg_unfocused = TB_RGB(0x56, 0x5F, 0x89),
    .window_border_style = UI_BORDER_DOUBLE,
    .window_border_style_unfocused = UI_BORDER_SINGLE,
    .window_border_style_docked = UI_BORDER_SINGLE,
    .window_border_style_docked_unfocused = UI_BORDER_SINGLE,
    .window_close_bg = TB_RGB(0xF7, 0x76, 0x8E),
    .window_fg = TB_RGB(0xC0, 0xCA, 0xF5),
    .window_bg = TB_RGB(0x1F, 0x22, 0x33),
    .modal_border_fg = TB_RGB(0xC0, 0xCA, 0xF5),
    .modal_border_bg = TB_RGB(0x1F, 0x22, 0x33),
    .modal_border_style = UI_BORDER_DOUBLE,
    .modal_fg = TB_RGB(0xC0, 0xCA, 0xF5),
    .modal_bg = TB_RGB(0x1F, 0x22, 0x33),
    .label_fg = TB_RGB(0x7A, 0xA2, 0xF7),  /* the theme's blue accent */
    .scrollbar_bg = TB_RGB(0x1F, 0x22, 0x33),
    .scrollbar_thumb_bg = TB_RGB(0x3B, 0x42, 0x61),

    .input_bg = TB_RGB(0x2A, 0x2E, 0x40),
    .input_bg_focus = TB_RGB(0x16, 0x16, 0x1E),  /* darker than input_bg so
                                                  * focus reads without a blue
                                                  * tint, which would collide
                                                  * with input_sel_bg below */
    .input_fg = TB_RGB(0xC0, 0xCA, 0xF5),
    .input_fg_focus = TB_RGB(0xFF, 0xFF, 0xFF),
    .input_sel_bg = TB_RGB(0x2E, 0x3C, 0x64),
    .input_sel_fg = TB_RGB(0xFF, 0xFF, 0xFF),

    .editor_bg = TB_RGB(0x1A, 0x1B, 0x26),
    .editor_fg = TB_RGB(0xC0, 0xCA, 0xF5),
    .editor_keyword_fg = TB_RGB(0xF7, 0x76, 0x8E),   /* types/storage: rose -
                                                      * the same color the
                                                      * screenshot gives
                                                      * `return`/`const` */
    .editor_keyword2_fg = TB_RGB(0xBB, 0x9A, 0xF7),  /* control flow: violet */
    .editor_string_fg = TB_RGB(0x9E, 0xCE, 0x6A),
    .editor_comment_fg = TB_RGB(0x56, 0x5F, 0x89),
    .editor_lint_fg = TB_RGB(0xE0, 0xAF, 0x68),  /* amber - stands out from the
                                                  * dim indigo comment gray */
    .editor_linenum_fg = TB_RGB(0x3B, 0x42, 0x61),
    .editor_preproc_fg = TB_RGB(0xBB, 0x9A, 0xF7),
    .editor_sel_bg = TB_RGB(0x2E, 0x3C, 0x64),
    .editor_sel_fg = TB_RGB(0xFF, 0xFF, 0xFF),
    .editor_word_match_bg = TB_RGB(0x28, 0x2D, 0x43),
    .editor_current_line_bg = TB_RGB(0x21, 0x23, 0x33),  /* one subtle step up
                                                          * from editor_bg */
    .editor_breakpoint_fg = TB_RGB(0xE0, 0x5A, 0x5A),  /* red, distinct from
                                                        * the rose editor_keyword_fg */
    .editor_exec_line_bg = TB_RGB(0x1F, 0x33, 0x2A),   /* green step up from
                                                        * editor_bg, same idea
                                                        * as editor_current_line_bg */
    .editor_bracket_fg = {
        TB_RGB(0xE0, 0xAF, 0x68),  /* amber */
        TB_RGB(0xF7, 0x76, 0x8E),  /* rose */
        TB_RGB(0x73, 0xDA, 0xCA),  /* teal */
        TB_RGB(0x7A, 0xA2, 0xF7),  /* blue */
    },
    .editor_tag_fg = TB_RGB(0x73, 0xDA, 0xCA),  /* teal - deliberately NOT the
                                                 * cyan editor_keyword_fg, so a
                                                 * tag name reads apart from
                                                 * the `struct` before it */
    .editor_number_fg = TB_RGB(0xFF, 0x9E, 0x64),  /* orange - numeric and
                                                     * NULL-style literals */
    .editor_char_fg = TB_RGB(0xFF, 0xC7, 0x77),    /* warm gold - distinct from
                                                     * the green string_fg and
                                                     * from the rose now used
                                                     * by editor_keyword_fg */
    .editor_function_fg = TB_RGB(0x7A, 0xA2, 0xF7),  /* blue - the same accent
                                                       * the chrome uses */
    .editor_output_bg = TB_RGB(0x16, 0x16, 0x1E),  /* one step darker than
                                                    * editor_bg, so Output
                                                    * reads as recessed */
    .editor_output_fg = TB_RGB(0xC0, 0xCA, 0xF5),

    /* UI_SYNTAX_MARKDOWN - mirrors this theme's own C-highlighting accents
     * rather than reusing them directly, so Markdown reads as part of the
     * same indigo palette. */
    .md_heading_fg = TB_RGB(0x7A, 0xA2, 0xF7),
    .md_blockquote_fg = TB_RGB(0x56, 0x5F, 0x89),
    .md_code_fg = TB_RGB(0x9E, 0xCE, 0x6A),
    .md_bold_fg = TB_RGB(0xC0, 0xCA, 0xF5),
    .md_link_fg = TB_RGB(0x73, 0xDA, 0xCA),
    .editor_diff_add_bg = TB_RGB(0x21, 0x3B, 0x2E),
    .editor_diff_remove_bg = TB_RGB(0x3B, 0x21, 0x30),
    .md_code_bg = TB_RGB(0x1F, 0x22, 0x33),

    /* <listbox> - same body colors as <editor>/<input>, selection reuses the
     * same #7AA2F7 accent as every other "selected" state in this theme. */
    .listbox_fg = TB_RGB(0x9A, 0xA5, 0xCE),  /* dimmer than editor_fg: the
                                              * Folder panel is the biggest
                                              * list on screen and shouldn't
                                              * pull attention off the code */
    .listbox_bg = TB_RGB(0x1A, 0x1B, 0x26),
    .listbox_sel_fg = TB_RGB(0x1A, 0x1B, 0x26),
    .listbox_sel_bg = TB_RGB(0x7A, 0xA2, 0xF7),
    .listbox_sel_inactive_fg = TB_RGB(0xC0, 0xCA, 0xF5),
    .listbox_sel_inactive_bg = TB_RGB(0x2A, 0x2E, 0x40),

    /* Project panel file-type markers - see ui_theme's own doc comment. */
    .project_icon_c_fg = TB_RGB(0x7A, 0xA2, 0xF7),   /* this theme's own accent */
    .project_icon_h_fg = TB_RGB(0x9E, 0xCE, 0x6A),   /* matches md_code_fg's green */
    .project_icon_md_fg = TB_RGB(0x73, 0xDA, 0xCA),  /* matches this theme's
                                                       * own md_link_fg */

    /* <editor> inline diagnostics - the palette's own red/amber/blue, so the
     * squiggles read as part of this theme. */
    .diag_error_fg = TB_RGB(0xDB, 0x4B, 0x4B),
    .diag_warning_fg = TB_RGB(0xE0, 0xAF, 0x68),
    .diag_info_fg = TB_RGB(0x0D, 0xB9, 0xD7),
    /* Each fg at 18% over editor_bg - left unset these are 0, a black bar. */
    .diag_error_bg = TB_RGB(0x3D, 0x24, 0x2D),
    .diag_warning_bg = TB_RGB(0x3E, 0x36, 0x32),
    .diag_info_bg = TB_RGB(0x18, 0x37, 0x46),
};

/* "Xcode Dark" - Xcode's own Default (Dark) editor palette (editor #292A30,
 * text #FFFFFF, current line #2F3239, selection #646F83): pink keywords,
 * salmon strings, khaki numbers/chars, orange preprocessor, gray-blue
 * comments, mint type names and teal function names. Chrome is Xcode's
 * dark navigator/inspector graphite with macOS's system blue as accent. */
static const ui_theme g_theme_xcode_dark = {
    .desktop_bg = TB_RGB(0x1F, 0x1F, 0x24),

    .btn_bg = TB_RGB(0x3D, 0x3E, 0x45),
    .btn_bg_hot = TB_RGB(0x0A, 0x84, 0xFF),
    .btn_bg_active = TB_RGB(0x00, 0x58, 0xD0),
    .btn_fg = TB_RGB(0xE5, 0xE5, 0xEA),

    .hotkey_fg = TB_RGB(0xFF, 0xFF, 0xFF),
    .hotkey_key_fg = TB_RGB(0xD0, 0xBF, 0x69),  /* the editor's khaki literal color */
    .hotkey_bg = TB_RGB(0x00, 0x58, 0xD0),
    .hotkey_fg_hot = TB_RGB(0xFF, 0xFF, 0xFF),
    .hotkey_bg_hot = TB_RGB(0x0A, 0x84, 0xFF),

    .menu_fg = TB_RGB(0xE5, 0xE5, 0xEA),
    .menu_bg = TB_RGB(0x2D, 0x2E, 0x33),
    .menu_fg_sel = TB_RGB(0xFF, 0xFF, 0xFF),
    .menu_bg_sel = TB_RGB(0x00, 0x58, 0xD0),
    .menu_item_fg = TB_RGB(0xE5, 0xE5, 0xEA),
    .menu_item_bg = TB_RGB(0x2D, 0x2E, 0x33),
    .menu_item_fg_hot = TB_RGB(0xFF, 0xFF, 0xFF),
    .menu_item_bg_hot = TB_RGB(0x00, 0x58, 0xD0),
    .menu_item_shortcut_fg = TB_RGB(0x8E, 0x8E, 0x93),
    .menu_item_fg_disabled = TB_RGB(0x63, 0x63, 0x66),
    .menu_border_fg = TB_RGB(0x4A, 0x4A, 0x4F),
    .menu_border_bg = TB_RGB(0x2D, 0x2E, 0x33),
    .menu_border_style = UI_BORDER_SINGLE,

    .box_fg = TB_RGB(0xE5, 0xE5, 0xEA),
    .box_bg = TB_RGB(0x2D, 0x2E, 0x33),
    .box_border_style = UI_BORDER_DOUBLE,

    .window_border_fg = TB_RGB(0xC7, 0xC7, 0xCC),
    .window_border_bg = TB_RGB(0x2D, 0x2E, 0x33),
    .window_border_fg_dragging = TB_RGB(0x0A, 0x84, 0xFF),
    .window_border_fg_unfocused = TB_RGB(0x63, 0x63, 0x66),
    .window_border_style = UI_BORDER_DOUBLE,
    .window_border_style_unfocused = UI_BORDER_SINGLE,
    .window_border_style_docked = UI_BORDER_SINGLE,
    .window_border_style_docked_unfocused = UI_BORDER_SINGLE,
    .window_close_bg = TB_RGB(0xFF, 0x5F, 0x57),  /* macOS's own red traffic light */
    .window_fg = TB_RGB(0xE5, 0xE5, 0xEA),
    .window_bg = TB_RGB(0x2D, 0x2E, 0x33),
    .modal_border_fg = TB_RGB(0xC7, 0xC7, 0xCC),
    .modal_border_bg = TB_RGB(0x2D, 0x2E, 0x33),
    .modal_border_style = UI_BORDER_DOUBLE,
    .modal_fg = TB_RGB(0xE5, 0xE5, 0xEA),
    .modal_bg = TB_RGB(0x2D, 0x2E, 0x33),
    .label_fg = TB_RGB(0x5A, 0xC8, 0xFA),  /* macOS system teal-blue */
    .scrollbar_bg = TB_RGB(0x2D, 0x2E, 0x33),
    .scrollbar_thumb_bg = TB_RGB(0x5A, 0x5A, 0x5F),

    .input_bg = TB_RGB(0x3D, 0x3E, 0x45),
    .input_bg_focus = TB_RGB(0x1F, 0x1F, 0x24),  /* darker than input_bg, not
                                                  * a blue tint, so the blue
                                                  * input_sel_bg stays visible */
    .input_fg = TB_RGB(0xE5, 0xE5, 0xEA),
    .input_fg_focus = TB_RGB(0xFF, 0xFF, 0xFF),
    .input_sel_bg = TB_RGB(0x64, 0x6F, 0x83),
    .input_sel_fg = TB_RGB(0xFF, 0xFF, 0xFF),

    .editor_bg = TB_RGB(0x29, 0x2A, 0x30),
    .editor_fg = TB_RGB(0xFF, 0xFF, 0xFF),
    .editor_keyword_fg = TB_RGB(0xFC, 0x5F, 0xA3),   /* Xcode uses one pink for
                                                      * every keyword... */
    .editor_keyword2_fg = TB_RGB(0xFC, 0x5F, 0xA3),  /* ...control flow too */
    .editor_string_fg = TB_RGB(0xFC, 0x6A, 0x5D),
    .editor_comment_fg = TB_RGB(0x6C, 0x79, 0x86),
    .editor_lint_fg = TB_RGB(0x5A, 0xC8, 0xFA),  /* stands out from the
                                                  * gray-blue comment color */
    .editor_linenum_fg = TB_RGB(0x5C, 0x5F, 0x66),
    .editor_preproc_fg = TB_RGB(0xFD, 0x8F, 0x3F),
    .editor_sel_bg = TB_RGB(0x64, 0x6F, 0x83),  /* Xcode Dark's actual selection */
    .editor_sel_fg = TB_RGB(0xFF, 0xFF, 0xFF),
    .editor_word_match_bg = TB_RGB(0x3C, 0x40, 0x48),
    .editor_current_line_bg = TB_RGB(0x2F, 0x32, 0x39),  /* Xcode Dark's actual
                                                          * current-line tint */
    .editor_breakpoint_fg = TB_RGB(0x3F, 0x80, 0xF5),  /* Xcode's breakpoints
                                                        * are blue chips */
    .editor_exec_line_bg = TB_RGB(0x2E, 0x4A, 0x3C),   /* green step up from
                                                        * editor_bg */
    .editor_bracket_fg = {
        TB_RGB(0xFD, 0x8F, 0x3F),  /* orange */
        TB_RGB(0xD0, 0xBF, 0x69),  /* khaki */
        TB_RGB(0x9E, 0xF1, 0xDD),  /* mint */
        TB_RGB(0xDA, 0xBA, 0xFF),  /* lavender */
    },
    .editor_tag_fg = TB_RGB(0x9E, 0xF1, 0xDD),  /* mint - Xcode's project
                                                 * type-name color */
    .editor_number_fg = TB_RGB(0xD0, 0xBF, 0x69),
    .editor_char_fg = TB_RGB(0xD0, 0xBF, 0x69),  /* Xcode colors chars like
                                                  * numbers, not like strings */
    .editor_function_fg = TB_RGB(0x67, 0xB7, 0xA4),  /* teal - Xcode's project
                                                       * function-name color */
    .editor_output_bg = TB_RGB(0x1F, 0x1F, 0x24),  /* one step darker than
                                                    * editor_bg, like Xcode's
                                                    * console */
    .editor_output_fg = TB_RGB(0xE5, 0xE5, 0xEA),

    /* UI_SYNTAX_MARKDOWN - mirrors this theme's own C-highlighting accents
     * so Markdown reads as part of the same palette. */
    .md_heading_fg = TB_RGB(0xFC, 0x5F, 0xA3),
    .md_blockquote_fg = TB_RGB(0x6C, 0x79, 0x86),
    .md_code_fg = TB_RGB(0xD0, 0xBF, 0x69),
    .md_bold_fg = TB_RGB(0xFF, 0xFF, 0xFF),
    .md_link_fg = TB_RGB(0x5A, 0xC8, 0xFA),
    .editor_diff_add_bg = TB_RGB(0x1C, 0x33, 0x20),
    .editor_diff_remove_bg = TB_RGB(0x3A, 0x1E, 0x1E),
    .md_code_bg = TB_RGB(0x2D, 0x2E, 0x33),

    /* <listbox> - Xcode's navigator: a shade darker than the editor, with
     * the same system blue as every other "selected" state in this theme. */
    .listbox_fg = TB_RGB(0xD1, 0xD1, 0xD6),  /* dimmer than editor_fg so the
                                              * Folder panel doesn't pull
                                              * attention off the code */
    .listbox_bg = TB_RGB(0x26, 0x26, 0x2B),
    .listbox_sel_fg = TB_RGB(0xFF, 0xFF, 0xFF),
    .listbox_sel_bg = TB_RGB(0x00, 0x58, 0xD0),
    .listbox_sel_inactive_fg = TB_RGB(0xE5, 0xE5, 0xEA),
    .listbox_sel_inactive_bg = TB_RGB(0x3D, 0x3E, 0x45),

    /* Project panel file-type markers - see ui_theme's own doc comment. */
    .project_icon_c_fg = TB_RGB(0x9E, 0xF1, 0xDD),   /* matches editor_tag_fg */
    .project_icon_h_fg = TB_RGB(0xDA, 0xBA, 0xFF),   /* lavender bracket color */
    .project_icon_md_fg = TB_RGB(0xFD, 0x8F, 0x3F),  /* matches editor_preproc_fg */

    /* <editor> inline diagnostics - Xcode's own issue-navigator red/yellow
     * and the system teal-blue for notes. */
    .diag_error_fg = TB_RGB(0xFF, 0x4B, 0x4B),
    .diag_warning_fg = TB_RGB(0xFF, 0xC6, 0x27),
    .diag_info_fg = TB_RGB(0x5A, 0xC8, 0xFA),
    /* Each fg at 18% over editor_bg - left unset these are 0, a black bar. */
    .diag_error_bg = TB_RGB(0x50, 0x30, 0x35),
    .diag_warning_bg = TB_RGB(0x50, 0x46, 0x2E),
    .diag_info_bg = TB_RGB(0x32, 0x46, 0x54),
};

/* Index into the Theme <select> (Ambar=0/Dark=1/White=2/Nebula=3/Xcode Dark=4 - matches the
 * add_select_item() order in app_init()) of whichever theme is currently
 * applied. Kept in sync by apply_theme()'s callers below and read
 * back by EVT_OPTIONS_ENV to select the right row each time the dialog
 * opens, instead of always defaulting to the first. */

static int g_quit = 0;

/* Create a <text> node, attach it to `parent`, and return it. */
static ui_node* add_text(ui_node* parent, int x, int y, const char* label,
                          uint32_t fg, uint32_t bg)
{
    ui_node* n = ui_create_element(UI_TAG_TEXT);
    ui_set_rect(n, x, y, 0, 0);
    ui_set_label(n, label);
    ui_set_color(n, fg, bg);
    ui_append_child(parent, n);
    return n;
}

/* Create an <input> node, attach it to `parent`, and return it. */
static ui_node* add_input(ui_node* parent, int x, int y, int w, const char* value)
{
    ui_node* n = ui_create_element(UI_TAG_INPUT);
    ui_set_rect(n, x, y, w, 1);
    ui_set_value(n, value);
    ui_append_child(parent, n);
    return n;
}

/* Create a <select> node, attach it to `parent`, and return it - its
 * options are added separately as plain UI_TAG_ITEM children (ui_create_
 * element + ui_set_label + ui_set_id + ui_append_child), same as a <menu>'s. */
static ui_node* add_select(ui_node* parent, int x, int y, int w)
{
    ui_node* n = ui_create_element(UI_TAG_SELECT);
    ui_set_rect(n, x, y, w, 1);
    ui_append_child(parent, n);
    return n;
}

/* Create an <item> option for a <select>, attach it, and return it. */
static ui_node* add_select_item(ui_node* select, int id, const char* label)
{
    ui_node* n = ui_create_element(UI_TAG_ITEM);
    ui_set_id(n, id);
    ui_set_label(n, label);
    ui_append_child(select, n);
    return n;
}

/* Create a GROUP (radio cluster when multi=0, check-box cluster when multi=1),
 * attach it, and return it - its choices are added as UI_TAG_ITEM children,
 * one per row, via add_group_item(). */
static ui_node* add_group(ui_node* parent, int x, int y, int w, int h, int multi)
{
    ui_node* g = ui_create_element(UI_TAG_GROUP);
    ui_set_rect(g, x, y, w, h);
    ui_set_multi(g, multi);
    ui_append_child(parent, g);
    return g;
}

static ui_node* add_group_item(ui_node* group, const char* label)
{
    ui_node* n = ui_create_element(UI_TAG_ITEM);
    ui_set_label(n, label);
    ui_append_child(group, n);
    return n;
}

/* Picks a document <editor>'s syntax color mode from its file's extension -
 * defined below, after ci_strcmp() which it uses. */
static ui_syntax syntax_for_path(const char* path);

/* File > New (EVT_FILE_NEW): unlike About/Directories/Output, which are each
 * a single node shown/reused via ui_screen_show_window(), "New" makes a
 * brand new <modal>+<window>+<editor> triple every time it fires - the same
 * call ends up opening a whole separate window instead of just re-raising
 * one, simply because the node it's given is a fresh one, never previously
 * shown. Cascades each new window's position so they don't land exactly on
 * top of one another, and numbers the title NONAME00.C, NONAME01.C, ... */
/* `path`'s last-modified time, or 0 if it can't be stat'ed (e.g. a
 * NONAMEnn.C placeholder not saved yet) - see file_watch_check(). */
static long long file_mtime(const char* path)
{
    struct stat st;
    if (!path || !path[0] || stat(path, &st) != 0)
        return 0;
    return (long long)st.st_mtime;
}

static ui_node* make_editor_window(ui_node* root, int seq, const char* title,
                                    const char* content, const char* path)
{
    ui_node* wrapper = ui_create_element(UI_TAG_MODAL);
    ui_set_path(wrapper, path);
    ui_set_file_time(wrapper, file_mtime(path));  /* see file_watch_check() */
    ui_set_transient(wrapper, 1);  /* a one-off document window - torn down
                                    * for good when closed, not kept around
                                    * as a reusable singleton like a docked
                                    * panel/dialog - see ui_set_transient and
                                    * app_frame()'s ui_screen_take_closed_
                                    * window() call. */
    ui_append_child(root, wrapper);

    int ew_x = 5 + (seq % 6) * 2, ew_y = 2 + (seq % 6), ew_w = 70, ew_h = 20;
    ui_node* window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(window, ew_x, ew_y, ew_w, ew_h);
    ui_set_label(window, title);
    ui_set_color(window, ui_get_theme()->window_fg, ui_get_theme()->window_bg);
    ui_set_resizable(window, 1);
    ui_set_shadow(window, 0);  /* floating document window - no shadow */
    ui_append_child(wrapper, window);

    /* Fill the window's interior (just inside its border), not some smaller
     * fixed rect - matches its own w/h so a resize (which stretches an
     * <editor> child by the same delta the window grew by) keeps it snug. */
    ui_node* editor = ui_create_element(UI_TAG_EDITOR);
    ui_set_id(editor, EVT_EDITOR_CTRLCLICK);
    ui_set_rect(editor, ew_x + 1, ew_y + 1, ew_w - 2, ew_h - 2);
    ui_syntax syntax = syntax_for_path(path);
    ui_set_syntax(editor, syntax);
    ui_set_value(editor, content);
    if (syntax == UI_SYNTAX_MARKDOWN)
        ui_set_read_only(editor, 1);  /* .md always opens read-only - see
                                       * syntax_for_path's own doc comment;
                                       * still toggleable by hand afterward
                                       * via the editor popup/Edit menu's
                                       * "Read-only" item like any other file,
                                       * this is just the opening default. */
    ui_append_child(window, editor);

    /* Every new document window opens filling whatever desktop space is
     * left once the docked Folder/Output panels (see ui_set_dock) are
     * subtracted, rather than at the small cascaded rect above - that rect
     * is kept anyway as the un-maximize/"restore" geometry. */
    ui_window_maximize(g_screen, window);

    return wrapper;
}

/* File > New's own default starting content/title, unlike a File > Open'd
 * document which brings its own (see open_dialog_activate). */
static ui_node* make_new_editor_window(ui_node* root, int seq)
{
    char title[16], path[16];
    snprintf(title, sizeof title, " NONAME%02d.C ", seq);
    snprintf(path, sizeof path, "NONAME%02d.C", seq);
    ui_node* wrapper = make_editor_window(root, seq, title,
        "#include <stdio.h>\n"
        "\n"
        "int main(void)\n"
        "{\n"
        "    printf(\"Hello, world!\\n\");\n"
        "    return 0;\n"
        "}\n", path);

    /* Still at this placeholder path, not a real file the user chose - see
     * ui_set_untitled. File > Save (and Compile) both check this to route to
     * Save As instead of silently writing/compiling "NONAME00.C" into
     * whatever the current directory happens to be. */
    ui_set_untitled(wrapper, 1);
    return wrapper;
}

/* Switches the whole running app to `theme`: the framework-drawn chrome
 * (menus, buttons, editor, window borders, ...) re-styles on its own the
 * next frame just from ui_set_theme() - the desktop backdrop doesn't,
 * since it's a per-screen value the app set explicitly (ui_screen_set_
 * desktop()), so it needs this one extra call to track the theme too. */
/* Theme <select> row order - Ambar=0/Dark=1/White=2/Nebula=3/Xcode Dark=4, matching the
 * add_select_item() calls in app_init(). Out-of-range (a hand-edited or
 * future session file) falls back to the startup default rather than
 * indexing off the end. */
static const ui_theme* theme_by_index(int index)
{
    switch (index)
    {
    case 0:  return &g_theme_ambar;
    case 2:  return &g_theme_white;
    case 3:  return &g_theme_nebula;
    case 4:  return &g_theme_xcode_dark;
    default: return &g_theme_dark;
    }
}

/* All defined further down (with the rest of the session plumbing, and with
 * the ".cakeproj" JSON helpers) - forward declared so the peek below, which
 * app_init needs early, can use them. */
static int get_session_file_path(char* buf, size_t cap);
static char* read_file_to_string(const char* path);
static int project_json_get_int(const struct json_value* object, const char* key, int fallback);


/* The parsed session.json, read exactly once per run by session_json() and
 * released by session_json_close() at the end of load_session(). NULL both
 * before the first read and whenever there is no usable session file. */
static struct json_value* g_session_json;
static int g_session_json_read;    /* the parse has been attempted */
static int g_session_file_found;   /* a session file existed and was read */

/* The session document, parsed on first use and shared from then on: both
 * the early theme peek below and load_session() at the end of app_init read
 * this same tree, so session.json is read and parsed once, not once per
 * caller. Returns NULL on a first run (no file yet) or an unparseable file -
 * every caller then just keeps its own default. */
static struct json_value* session_json(void)
{
    if (g_session_json_read)
        return g_session_json;
    g_session_json_read = 1;

    char path[FS_MAX_PATH];
    if (!get_session_file_path(path, sizeof path))
        return NULL;

    char* text = read_file_to_string(path);
    if (!text)
        return NULL;

    g_session_file_found = 1;

    struct json_value* root = json_parse(text, NULL);
    free(text);

    if (root && root->type != JSON_OBJECT)
    {
        json_delete(root);
        root = NULL;
    }

    g_session_json = root;
    return g_session_json;
}

/* Releases the parsed session - called once load_session() is done with it,
 * since nothing reads the session document after startup. */
static void session_json_close(void)
{
    json_delete(g_session_json);
    g_session_json = NULL;
}

/* Reads a single integer key out of the session document, before the real
 * load_session() runs.
 *
 * Needed because the theme has to be known BEFORE app_init() builds its
 * windows: 14 of them bake theme colors into ui_set_color() at construction
 * time, and apply_theme() only swaps the live palette - it cannot go back
 * and re-color what was already built. Restoring the theme at the end of
 * app_init (where the font is restored) would leave every one of those
 * windows wearing the startup theme's colors. */
static int session_peek_int(const char* key, int fallback)
{
    return project_json_get_int(session_json(), key, fallback);
}

static void apply_theme(const ui_theme* theme)
{
    /* ui_screen_set_theme(), not ui_set_theme(): the dialogs and document
     * windows built in app_init() baked this app's theme colors into their
     * nodes, and only the screen-aware variant goes back and re-colors them
     * (see its comment in ui.h). Without it a runtime theme switch left,
     * say, every dialog label sitting on the old theme's background. */
    ui_screen_set_theme(g_screen, theme);
    ui_screen_set_desktop(g_screen, theme->desktop_bg);
}

/* Case-insensitive strcmp - DOS-style filename matching ("*.c" matches
 * "FOO.C") without relying on POSIX-only strcasecmp/Win32-only _stricmp. */
static int ci_strcmp(const char* a, const char* b)
{
    while (*a && *b)
    {
        int ca = tolower((unsigned char)*a);
        int cb = tolower((unsigned char)*b);
        if (ca != cb)
            return ca - cb;
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

/* A document <editor>'s syntax color mode is derived from its file's
 * extension, not chosen explicitly - C source for .c/.h, Markdown for .md,
 * and the fallback (no coloring at all, like a real .txt file) for anything
 * else, including a path with no extension at all. The Output window's
 * editor is the one exception (see its own ui_set_syntax call) - its content
 * is always set programmatically, never opened from a file.
 *
 * make_editor_window() also uses the UI_SYNTAX_MARKDOWN result to default
 * every .md file to read-only when it's opened - Markdown in this project is
 * always either shipped reference text (the .md files under help/) or generated/prose meant
 * to be read, not hand-edited in place; still just a default, toggleable
 * afterward like any other file via the editor popup/Edit menu's "Read-only"
 * item. */
static ui_syntax syntax_for_path(const char* path)
{
    const char* dot = strrchr(path, '.');
    if (!dot)
        return UI_SYNTAX_NONE;
    if (ci_strcmp(dot, ".c") == 0 || ci_strcmp(dot, ".h") == 0)
        return UI_SYNTAX_C;
    if (ci_strcmp(dot, ".md") == 0)
        return UI_SYNTAX_MARKDOWN;
    return UI_SYNTAX_NONE;
}

/* Whether `path` is a compilable translation unit - deliberately stricter
 * than "syntax_for_path(path) == UI_SYNTAX_C", which also covers .h (a
 * header alone isn't something do_compile() can hand the compiler as its
 * own file argument). Gates "Compile"/F7 - see g_compile_item's own doc
 * comment and its app_frame() call site. */
static int path_is_c_source(const char* path)
{
    const char* dot = strrchr(path, '.');
    return dot && ci_strcmp(dot, ".c") == 0;
}

/* Writes `path` with its extension swapped .c<->.h (case preserved: .c/.h
 * stay lowercase, .C/.H stay uppercase) into `out`, and returns 1 - the
 * editor popup's "Toggle Header/Source" item. Returns 0 (leaving `out`
 * untouched) if `path` isn't a .c/.h file, or doesn't fit in `out`. */
static int header_source_counterpart(const char* path, char* out, size_t out_size)
{
    const char* dot = strrchr(path, '.');
    if (!dot || strlen(dot) != 2)
        return 0;
    char other;
    if (dot[1] == 'c') other = 'h';
    else if (dot[1] == 'C') other = 'H';
    else if (dot[1] == 'h') other = 'c';
    else if (dot[1] == 'H') other = 'C';
    else return 0;

    size_t len = strlen(path);
    if (len >= out_size)
        return 0;
    strcpy(out, path);
    out[len - 1] = other;
    return 1;
}

/* Update a "Read-only" item's (`item`) label and enabled state to reflect
 * the editor in `win`. Centralising this avoids duplicated logic across the
 * editor popup and the Edit menu's own copy of the same toggle. */
static void refresh_readonly_item(ui_node* item, ui_node* win)
{
    if (!item)
        return;
    ui_node* ed = editor_in_window(win);
    if (!ed)
    {
        ui_set_label(item, "[ ] Read-only");
        ui_set_enabled(item, 0);
        return;
    }
    int vt100 = ui_get_syntax(ed) == UI_SYNTAX_VT100;
    ui_set_label(item, ui_get_read_only(ed) ? "[x] Read-only" : "[ ] Read-only");
    ui_set_enabled(item, !vt100);
}

/* Whether `text`'s line at byte range [ls, ls+line_len) opens/closes a
 * fenced code block - same "```" leading-whitespace-then-3-backticks test
 * ide_ui.c's own scan_markdown_fence_state()/render_editor_line_markdown()
 * use, duplicated here (not exposed from ide_ui.c) since this is the only
 * place ide.c itself needs to walk a Markdown document's fence structure. */
static int md_line_is_fence(const char* text, int ls, int line_len)
{
    int j = 0;
    while (j < line_len && (text[ls + j] == ' ' || text[ls + j] == '\t'))
        j++;
    return line_len - j >= 3 && text[ls + j] == '`' && text[ls + j + 1] == '`' && text[ls + j + 2] == '`';
}

/* The fenced code block ("```" ... "```") containing 0-based document line
 * `click_line` in Markdown source `text`, if any - either a line strictly
 * between the fences, or one of the fence lines themselves (so right-
 * clicking the opening/closing ``` still counts as "on" the block, not just
 * its content). Returns a malloc'd copy of the block's *content* (the lines
 * between the fences, excluding both fence lines and their language tag)
 * the caller must free, or NULL if click_line isn't part of a fenced block
 * - including an unterminated one that never closes before EOF, since
 * there's then no real block to have clicked on. */
static char* markdown_codeblock_at_line(const char* text, int click_line)
{
    int text_len = (int)strlen(text);
    int line_idx = 0, off = 0;
    int in_block = 0, block_open_line = -1, content_start = -1;

    while (off <= text_len)
    {
        int ls = off;
        while (off < text_len && text[off] != '\n')
            off++;

        if (!in_block && md_line_is_fence(text, ls, off - ls))
        {
            in_block = 1;
            block_open_line = line_idx;
            content_start = (off < text_len) ? off + 1 : off;
        }
        else if (in_block && md_line_is_fence(text, ls, off - ls))
        {
            if (click_line >= block_open_line && click_line <= line_idx)
            {
                int len = ls - content_start;
                if (len < 0)
                    len = 0;
                char* out = malloc(len + 1);
                memcpy(out, text + content_start, len);
                out[len] = 0;
                return out;
            }
            in_block = 0;
            block_open_line = -1;
            content_start = -1;
        }

        if (off >= text_len)
            break;
        off++;  /* skip the '\n' onto the next line */
        line_idx++;
    }

    return NULL;  /* click_line never fell inside/on a *closed* fenced block */
}

/* Refreshes the editor popup's "Copy Code Block"/"Copy to Playground" items
 * (see g_md_codeblock_text) for the right-click at screen point (mx, my) -
 * enabled only when `win` is a Markdown document and that point lands on/in
 * one of its fenced code blocks, same "point-in-time snapshot" pattern as
 * refresh_readonly_item() above. Captured now (rather than re-deriving the
 * block from mx/my again when an item actually fires) because by then the
 * popup may have scrolled the editor via its own scrollbar, or the document
 * may simply have changed - the block under the original click is whatever
 * this function decides right now, once, not whatever happens to be under
 * that same screen point later. */
static void refresh_codeblock_items(ui_node* copy_item, ui_node* playground_item,
                                     ui_node* win, int mx, int my)
{
    free(g_md_codeblock_text);
    g_md_codeblock_text = NULL;

    ui_node* ed = editor_in_window(win);
    if (ed && ui_get_syntax(ed) == UI_SYNTAX_MARKDOWN)
    {
        int line = ui_editor_line_at_point(ed, mx, my);
        if (line >= 0)
            g_md_codeblock_text = markdown_codeblock_at_line(ui_get_label(ed), line);
    }

    ui_set_enabled(copy_item, g_md_codeblock_text != NULL);
    ui_set_enabled(playground_item, g_md_codeblock_text != NULL);
}

/* Whether `wrapper` is one of the currently open floating windows - i.e. it
 * would actually show up on screen right now, not just that the node
 * exists somewhere in the tree (a closed-but-not-torn-down persistent
 * singleton like g_output_window/g_folder.window stays a child of root -
 * see ui_set_transient's own doc comment - so checking the tree wouldn't
 * tell "open" from "closed"; the shown-window list does). Used by the View
 * menu's "[x] Show ..." items below. */
static int window_is_shown(ui_node* wrapper)
{
    if (!wrapper)
        return 0;
    int count = ui_screen_window_count(g_screen);
    for (int i = 0; i < count; i++)
    {
        if (ui_screen_window_at(g_screen, i) == wrapper)
            return 1;
    }
    return 0;
}

/* View > "Show Output"/"Show Folder"/"Show Playground" - same "[x] Label"/
 * "[ ] Label" convention as refresh_readonly_item/refresh_folder_filter_item
 * above, just reporting whether the window is currently open instead of a
 * boolean setting. Refreshed every frame from app_frame() (see
 * refresh_readonly_item's own doc comment for why - the menubar's dropdowns
 * have no app-level open-time hook to piggyback on instead). */
static void refresh_view_item(ui_node* item, const char* label, int visible)
{
    if (!item)
        return;
    char buf[40];
    snprintf(buf, sizeof buf, "%s %s", visible ? "[x]" : "[ ]", label);
    ui_set_label(item, buf);
}

/* The docked panel whose frame is under (x, y), or NULL. "Frame" means the
 * <window>'s own border/title row: inside the window but outside every
 * control in it, so a right-click over the Output editor or the Folder
 * listbox still gets that control's own popup and only the surrounding
 * border opens the dock menu. Returns the <window> node (child 0 of the
 * panel's wrapper), which is what ui_set_dock/ui_get_dock take. */
static ui_node* _Opt docked_panel_frame_at(int x, int y)
{
    for (int i = 0; i < ui_child_count(g_root); i++)
    {
        ui_node* wrapper = ui_child_at(g_root, i);
        if (!window_is_shown(wrapper))
            continue;

        ui_node* win = ui_child_at(wrapper, 0);
        if (!win || ui_get_dock(win) == UI_DOCK_NONE)
            continue;

        if (!ui_node_contains(win, x, y))
            continue;

        int on_content = 0;
        for (int k = 0; k < ui_child_count(win); k++)
        {
            if (ui_node_contains(ui_child_at(win, k), x, y))
            {
                on_content = 1;
                break;
            }
        }

        if (!on_content)
            return win;
    }

    return NULL;
}

/* The shown docked panel occupying `side`, other than `except`, or NULL. A
 * side holds one panel: ui_set_dock gives each docked window the full extent
 * of its edge, so two on the same side would sit on top of each other. Until
 * the panels could be moved at runtime (see g_dockmenu) that couldn't happen
 * - Folder was built LEFT, Find and Replace RIGHT, Output BOTTOM - so the
 * check lives here, at the one place that changes a side. */
static ui_node* _Opt docked_panel_on_side(ui_dock_side side, const ui_node* _Opt except)
{
    for (int i = 0; i < ui_child_count(g_root); i++)
    {
        ui_node* wrapper = ui_child_at(g_root, i);
        if (!window_is_shown(wrapper))
            continue;

        ui_node* win = ui_child_at(wrapper, 0);
        if (win && win != except && ui_get_dock(win) == side)
            return win;
    }

    return NULL;
}

/* Move `win` to `side`, sizing it to a quarter of the screen along whichever
 * axis that side owns - a panel keeps only the extent it can control (a
 * left/right dock's width, a bottom dock's height; the other axis always
 * spans the full edge - see ui_set_dock), and the size it had on its old
 * axis means nothing on the new one. A quarter is what the Folder/Output
 * panels are built with anyway, and the dock border stays draggable from
 * there.
 *
 * Refuses, with a message box, when another panel already holds that side -
 * see docked_panel_on_side. */
static void dock_panel_to(ui_node* _Opt win, ui_dock_side side)
{
    if (!win)
        return;

    ui_node* _Opt occupied_by = docked_panel_on_side(side, win);
    if (occupied_by)
    {
        const char* label = ui_get_label(occupied_by);
        char msg[160];
        snprintf(msg, sizeof msg,
                 "There is already a panel there (%s).\n"
                 "Move or close it first.",
                 (label && label[0]) ? label : "another panel");
        ui_msgbox_button ok = { "   OK   ", 0 };
        ui_message_box(g_screen, "Dock", msg, &ok, 1);
        return;
    }

    int size = (side == UI_DOCK_BOTTOM) ? ui_screen_height(g_screen) / 4
                                        : ui_screen_width(g_screen) / 4;
    if (size < 1)
        size = 1;

    ui_set_dock(win, side, size);
}

/* Classic '*'/'?' glob, case-insensitive - just enough to match a DOS-style
 * mask like "*.C" against a filename. */
static int wildcard_match(const char* pattern, const char* name)
{
    if (*pattern == '*')
    {
        pattern++;
        if (!*pattern)
            return 1;  /* trailing '*' matches whatever's left of the name */
        for (; *name; name++)
            if (wildcard_match(pattern, name))
                return 1;
        return wildcard_match(pattern, name);  /* '*' can also match nothing */
    }
    if (*pattern == 0)
        return *name == 0;
    if (*name == 0)
        return 0;
    if (*pattern != '?' && tolower((unsigned char)*pattern) != tolower((unsigned char)*name))
        return 0;
    return wildcard_match(pattern + 1, name + 1);
}

/* Matches `name` against a ';'-separated list of globs, e.g. "*.h;*.c;*.md" -
 * true if any one matches. A single pattern (no ';') behaves exactly like
 * wildcard_match. */
static int mask_matches(const char* masks, const char* name)
{
    const char* p = masks;
    while (*p)
    {
        const char* sep = strchr(p, ';');
        int len = sep ? (int)(sep - p) : (int)strlen(p);
        char one[64];
        if (len > (int)sizeof one - 1)
            len = (int)sizeof one - 1;
        memcpy(one, p, (size_t)len);
        one[len] = 0;
        if (one[0] && wildcard_match(one, name))
            return 1;
        if (!sep)
            break;
        p = sep + 1;
    }
    return 0;
}

/* Sorts the Open dialog's rows the way a classic DOS-era file dialog does:
 * directories (marked with our own trailing "\", see open_dialog_refresh)
 * before files, alphabetically (case-insensitive) within each group. */
static int open_entry_cmp(const void* pa, const void* pb)
{
    const char* a = (const char*)pa;
    const char* b = (const char*)pb;
    int a_dir = a[0] && a[strlen(a) - 1] == '\\';
    int b_dir = b[0] && b[strlen(b) - 1] == '\\';
    if (a_dir != b_dir)
        return b_dir - a_dir;
    return ci_strcmp(a, b);
}

/* Normalizes `path` in place: '\' becomes '/', repeated separators collapse
 * to one (a leading "//" UNC prefix is kept), "." segments are dropped, ".."
 * removes the segment before it (never climbing above the root), and a
 * trailing '/' is removed unless it is the root itself ("/" or "C:/").
 * E.g. "C:\Users\thiag\source\repos\/installer" -> "C:/Users/thiag/source/repos/installer". */
static void ide_path_normalize(char* path)
{
    char* p;
    for (p = path; *p; p++)
        if (*p == '\\')
            *p = '/';

    /* Root prefix that ".." must never remove: "C:/", "//" (UNC) or "/". */
    size_t root = 0;
    if (((path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z')) &&
        path[1] == ':')
        root = path[2] == '/' ? 3 : 2;
    else if (path[0] == '/' && path[1] == '/')
        root = 2;
    else if (path[0] == '/')
        root = 1;

    char* r = path + root;
    char* w = path + root;
    while (*r)
    {
        while (*r == '/')
            r++;
        if (!*r)
            break;
        const char* seg = r;
        while (*r && *r != '/')
            r++;
        size_t seg_len = (size_t)(r - seg);

        if (seg_len == 1 && seg[0] == '.')
            continue;
        if (seg_len == 2 && seg[0] == '.' && seg[1] == '.')
        {
            char* start = path + root;
            /* Drop the previous segment, unless there is none (or it is
             * itself an unresolved ".." of a relative path). */
            if (w > start && !(w - start >= 2 && w[-1] == '.' && w[-2] == '.' &&
                               (w - start == 2 || w[-3] == '/')))
            {
                while (w > start && w[-1] != '/')
                    w--;
                if (w > start)
                    w--;  /* the '/' before it */
                continue;
            }
            if (root > 0)
                continue;  /* ".." at the root stays at the root */
        }

        if (w > path + root)
            *w++ = '/';
        memmove(w, seg, seg_len);
        w += seg_len;
    }
    *w = 0;
}

/* `dir` + "/" + `name` into `out`, normalized (see ide_path_normalize) - the one
 * way the Open dialog/Folder panel build a full path from a listing row. */
static void path_join(char* out, size_t out_size, const char* dir, const char* name)
{
    snprintf(out, out_size, "%s/%s", dir, name);
    ide_path_normalize(out);
}

/* Collapse every CRLF in `s` to a bare LF, in place.
 *
 * The editor's buffer is LF-only by construction - Enter inserts '\n', and
 * every line-splitting scan in ide_ui.c looks for '\n' alone - and saving
 * writes the buffer out verbatim through a "wb" stream (see
 * save_active_file), so LF is what lands on disk. Without this, a file
 * authored on Windows arrives with its CRs still in the text, where they
 * are ordinary content bytes: they sit at the end of every line, get
 * trimmed inconsistently by the renderer, and travel into anything copied
 * out of the buffer. Converting on the way in makes the whole round trip
 * LF - deliberately, not as a configurable choice. Lone CRs (classic
 * Mac line endings) are left alone; they're not a line ending this editor
 * has ever produced or split on. */
static void normalize_newlines(char* s)
{
    if (!s)
        return;
    char* w = s;
    char* r = s;
    while (*r)
    {
        if (*r == '\r')
        {
            char* p = r;
            while (*p == '\r')
                p++;
            if (*p == '\n')
            {
                r = p;  /* drop the whole run of CRs; the LF is copied below */
                continue;
            }
        }
        *w++ = *r++;
    }
    *w = '\0';
}

/* Rebuilds `listbox`'s rows from `dir`'s contents: ".." to go up, then every
 * subdirectory (marked with a trailing "\" - see dir_row_navigate, which
 * keys off that marker) and, unless `dirs_only`, every file whose name
 * matches `mask` (NULL/empty = no filtering, show them all) - dirs-first,
 * alphabetical (case-insensitive) within each group.
 *
 * Every row's real disk name (with the trailing "\" marker) is kept in
 * ui_set_path - that's what dir_row_navigate/folder_window_activate/
 * open_dialog_activate key off of. Shared by both callers. */
static void populate_listbox_from_dir(ui_node* listbox, const char* dir,
                                       const char* mask, int dirs_only)
{
    static ui_dir_entry raw[OPEN_MAX_ENTRIES];
    int n = ui_list_dir(dir, raw, OPEN_MAX_ENTRIES);

    while (ui_child_count(listbox) > 0)
    {
        ui_node* c = ui_child_at(listbox, 0);
        ui_remove_child(listbox, c);
        ui_node_free(c);
    }

    static char names[OPEN_MAX_ENTRIES][300];
    int count = 0;
    for (int i = 0; i < n; i++)
    {
        if (!raw[i].is_dir)
        {
            if (dirs_only)
                continue;
            if (mask && mask[0] && !mask_matches(mask, raw[i].name))
                continue;
        }
        snprintf(names[count], sizeof names[count], "%s%s", raw[i].name,
                 raw[i].is_dir ? "\\" : "");
        count++;
    }
    if (count > 1)
        qsort(names, (size_t)count, sizeof names[0], open_entry_cmp);

    ui_node* up_item = ui_create_element(UI_TAG_ITEM);
    ui_set_label(up_item, "..\\");
    ui_set_path(up_item, "..\\");
    ui_append_child(listbox, up_item);

    for (int i = 0; i < count; i++)
    {
        ui_node* item = ui_create_element(UI_TAG_ITEM);
        ui_set_label(item, names[i]);
        ui_set_path(item, names[i]);
        ui_append_child(listbox, item);
    }
    ui_select_set_selected(listbox, 0);
}

/* Rebuilds g_open.listbox's rows from g_open.dir (see
 * populate_listbox_from_dir), and refreshes the Name field to match -
 * called on first opening the dialog and after every navigation (up, into a
 * subdirectory, or a typed Name field). */
/* The Open dialog's overview (see ui_set_help on a <window>) for its current
 * mode - it is shared by several commands, so the overview follows the mode.
 * NULL where the dialog explains itself (plain Open/Save/folder pickers).
 * *short_help gets its status bar line. */
static const char* open_dialog_overview(const char** short_help)
{
    *short_help = NULL;
    switch (g_open.dialog_mode)
    {
    case OPEN_DLG_PROJECT_OPEN:
        *short_help = "Open a Cake project (`.cakeproj`)";
        return
            "# Open a Cake project (`.cakeproj`)\n"
            "\n"
            "Pick the project's `.cakeproj` file.\n"
            "\n"
            "A project is a `.cakeproj` file: a list of source files, plus the include directories and compiler options used to build them. File paths inside the project folder are stored relative to it, so the project can be moved or shared.\n"
            "\n"
            "## Build and Compile\n"
            "\n"
            "- **Build** (F7) compiles every `.c` file of the project in one Cake invocation - linking them is the output compiler's job. When the active file is not part of the open project (or no project is open), Build compiles just that file.\n"
            "- **Compile** (Ctrl+F7) always compiles only the active file.\n"
            "\n"
            "## Project settings vs. global settings\n"
            "\n"
            "- **Project > Include Directories...** and **Project > Options...** edit the project's own settings, saved in its `.cakeproj`. Include directories are stored relative to the project folder.\n"
            "- **File > Directories...** and **File > Options...** edit the global settings in `cake.json`, next to the IDE executable. They are used for every file that is not part of the open project - the Playground, a file opened on its own.\n"
            "\n"
            "The two are never merged: a file gets either the project's settings or the global ones.\n"
            "\n"
            "With the `default` target, the same `.cakeproj` works unchanged on Windows, Linux and macOS.";
    case OPEN_DLG_PROJECT_ADDFILE:
        *short_help = "Add existing source files to the open project";
        return
            "# Add existing source files to the open project\n"
            "\n"
            "Check several files to add them all at once. Files inside the project folder are stored relative to it; files elsewhere keep their full path. A file already in the project is not added twice.";
    case OPEN_DLG_PROJECT_ADDINCLUDE:
        *short_help = "Pick a directory to add to the include directory list";
        return
            "# Pick a directory to add to the include directory list\n"
            "\n"
            "The directory is searched for `#include` files, in list order - the **Up** / **Down** buttons of the Include Directories dialog change that order. For a project the path is stored relative to the project folder; for the global list (`cake.json`) it is stored as a full path.";
    default:
        return NULL;
    }
}

static void open_dialog_refresh(void)
{
    ide_path_normalize(g_open.dir);
    const char* overview_short;
    const char* overview = open_dialog_overview(&overview_short);
    ui_set_help(g_open.window, overview_short, overview);
    int folder_mode = g_open.dialog_mode == OPEN_DLG_FOLDER ||
        g_open.dialog_mode == OPEN_DLG_PROJECT_ADDINCLUDE ||
        g_open.dialog_mode == OPEN_DLG_NEWPROJECT_FOLDER ||
        g_open.dialog_mode == OPEN_DLG_GITCLONE_FOLDER;
    ui_set_label(g_open.list_label, folder_mode ? "Folders" : "Files");
    populate_listbox_from_dir(g_open.listbox, g_open.dir,
                               folder_mode ? NULL : g_open.mask, folder_mode);

    /* Multi-select is a per-mode property of the dialog (see
     * g_open.allow_multi) - set here, right after the rows were rebuilt, so
     * every fresh listing starts with nothing checked whichever mode is
     * active. */
    g_open.allow_multi = (g_open.dialog_mode == OPEN_DLG_PROJECT_ADDFILE);
    ui_set_multi(g_open.listbox, g_open.allow_multi);

    /* Cosmetic display only (backslashes regardless of platform, matching
     * this whole app's DOS/Windows IDE look) - never parsed back except
     * through EVT_OPEN_NAME, which accepts either separator style. Folder
     * mode shows just the directory - there's no mask/filename to append.
     * File-Open mode leaves the name portion empty: the mask that filters
     * the listing belongs to the "Type" combo alone (see g_open_filters /
     * EVT_OPEN_FILTER), same as the real Windows dialog, so it is never
     * echoed back into the Name field. Typing a wildcard there by hand
     * still works (EVT_OPEN_NAME), it just isn't the default content. */
    char display[1024];
    if (folder_mode)
        snprintf(display, sizeof display, "%s", g_open.dir);
    else
    {
        const char* name = (g_open.dialog_mode == OPEN_DLG_SAVE ||
                            g_open.dialog_mode == OPEN_DLG_PROJECT_NEW) ? g_save_name : "";
        path_join(display, sizeof display, g_open.dir, name);
        /* No name yet: keep a trailing separator so typing starts a new
         * name inside the directory ("C:/" already ends in one). */
        size_t n = strlen(display);
        if (!name[0] && n > 0 && display[n - 1] != '/' && n + 1 < sizeof display)
        {
            display[n] = '/';
            display[n + 1] = 0;
        }
    }
    for (char* p = display; *p; p++)
        if (*p == '/')
            *p = '\\';
    ui_set_value(g_open.name_input, display);

    /* Select just the mask/filename portion (after the last backslash), not
     * the directory - so the user can start typing a new mask like *.md
     * straight away and have it replace rather than get appended to
     * whatever mask/filename was showing before (see ui_set_value, which
     * always leaves the cursor at the end with nothing selected). */
    const char* last_bslash = strrchr(display, '\\');
    int sel_start = last_bslash ? (int)(last_bslash - display + 1) : 0;
    ui_editor_set_selection(g_open.name_input, sel_start, (int)strlen(display));
}

/* Shows or hides the "Type" label + files-of-type <select> as a pair.
 * Folder mode has no mask concept at all (populate_listbox_from_dir gets a
 * NULL mask there - see open_dialog_refresh), so rather than just disabling
 * the control - which would still sit there looking live - it's detached
 * from the dialog entirely. Always detaches first, which is a no-op if it
 * isn't currently attached (see ui_remove_child), so this is safe to call
 * regardless of the dialog's previous mode.
 *
 * Every node stores absolute screen coordinates, not parent-relative ones
 * (see shift_subtree's own doc comment in ide_ui.c) - dragging g_open.window
 * shifts every node currently attached to it, but a detached filter_label/
 * filter sits outside that subtree and is left behind wherever it last was.
 * Re-deriving their rect from g_open.window's *current* position here, every
 * time they're reattached, is what keeps them lined up with the dialog even
 * after it's been dragged while they were detached - relying on whatever
 * rect they were originally created with would otherwise leave them stranded
 * at their old position, disconnected from a since-moved dialog. */
static void open_dialog_set_filter_visible(int visible)
{
    ui_remove_child(g_open.window, g_open.filter_label);
    ui_remove_child(g_open.window, g_open.filter);

    /* The window itself grows/shrinks to match - without the Type row there's
     * nothing below the file list, and leaving the window at its full height
     * anyway just wastes the bottom few rows as dead space (see OPEN_DLG_
     * PROJECT_ADDFILE/OPEN_DLG_PROJECT_ADDINCLUDE, neither of which ever show
     * this row). Width and position are left untouched - only the height
     * changes, and only when it's actually different, so this is a no-op on
     * repeated calls with the same `visible` (no accumulating drift). */
    int wx, wy, ww, wh;
    ui_get_rect(g_open.window, &wx, &wy, &ww, &wh);
    int full_h = 21, short_h = 18;  /* full_h matches this window's own
                                     * original build_screen() height;
                                     * short_h ends one row past the file
                                     * list (which runs oy+6..oy+15), same
                                     * one-row-margin-then-border convention
                                     * the Type row's own oy+18/oy+20 gap
                                     * follows in the full-height case. */
    int want_h = visible ? full_h : short_h;
    if (wh != want_h)
        ui_set_rect(g_open.window, wx, wy, ww, want_h);

    if (visible)
    {
        ui_set_rect(g_open.filter_label, wx + 3, wy + 17, 0, 0);  /* <text> is
                                                                   * always
                                                                   * auto-sized
                                                                   * to its
                                                                   * label, same
                                                                   * as add_text() */
        ui_set_rect(g_open.filter, wx + 3, wy + 18, 41, 1);

        ui_append_child(g_open.window, g_open.filter_label);
        ui_append_child(g_open.window, g_open.filter);
        ui_set_enabled(g_open.filter, 1);
    }
}

/* An already-open editor window backed by `path`, or NULL - see
 * find_open_window() (defined after editor_in_window, which it needs). */
static ui_node* find_open_window(const char* path);

/* The <editor> inside a document window's <modal> wrapper - see
 * editor_in_window() (defined further below; forward-declared so
 * open_link_in_window()/do_editor_ctrlclick() above it can call it too). */
static ui_node* editor_in_window(const ui_node* wrapper);

/* File > Open Folder...'s OK/"Select" confirmation - defined with the rest
 * of the persistent folder browser window, forward-declared so the main
 * event dispatcher (which comes first) can call it from EVT_OPEN_OK. */
static void folder_select_confirm(void);

/* Project > New/Open Project/Add Existing File/Add Include Directory's own
 * confirmations - defined with the rest of the Project panel, forward-
 * declared for the same reason as folder_select_confirm() just above:
 * open_dialog_activate()/the main event dispatcher come first and need to
 * call them from EVT_OPEN_LISTBOX/EVT_OPEN_OK. */
static void project_open_file(const char* path);
static void project_add_file(const char* path);
static void project_add_include(const char* path);

/* The External Tools dialog's "..." Command browse (OPEN_DLG_EXTTOOL_CMD) -
 * defined with the rest of that dialog, forward-declared for the same
 * reason. */
static void exttool_browse_pick(const char* path);

/* The filename part of a path (after the last '/' or '\') - defined further
 * below, forward-declared so folder_window_refresh() above it can title the
 * folder window with just the folder's own name, not its full path. */
static const char* basename_of(const char* p);

/* Activates row `index`: a directory navigates into it (or up, for "..") and
 * refreshes the listing; a file is read from disk into a new editor window
 * (real content/title, unlike File > New's blank template) and the dialog
 * closes. Silently gives up if the file can't be read - nothing else
 * sensible to do without a status-bar error message this demo doesn't have. */
 /* Handles a directory row - one ending in "\" (the marker
  * populate_listbox_from_dir() gives every subdirectory and ".."): ".." goes
  * up via open_path_up, anything else appends into `dir_buf`. Returns 1 if
  * `label` was a directory row (and `dir_buf` was navigated - the caller
  * still has to refresh its own listbox), or 0 if it's a plain file row the
  * caller must handle itself. Shared by open_dialog_activate and
  * folder_window_activate, which each navigate a different directory
  * (g_open.dir vs g_folder.dir). */
static int dir_row_navigate(char* dir_buf, size_t dir_buf_size, const char* label)
{
    size_t len = strlen(label);
    if (len == 0 || label[len - 1] != '\\')
        return 0;

    char name[300];
    strncpy(name, label, sizeof name - 1);
    name[sizeof name - 1] = 0;
    name[strlen(name) - 1] = 0;  /* drop the trailing "\" marker */

    /* ".." included - ide_path_normalize resolves it (and stops at the root). */
    char new_dir[1024];
    path_join(new_dir, sizeof new_dir, dir_buf, name);
    strncpy(dir_buf, new_dir, dir_buf_size - 1);
    dir_buf[dir_buf_size - 1] = 0;
    return 1;
}

/* Opens `path` (labeled `label` for the new window's title) into a document
 * editor window - already open? just re-raises it instead of duplicating.
 * Shared by open_dialog_activate (which also closes the Open dialog
 * afterward - not this function's concern) and folder_window_activate
 * (which has no modal to close, the folder window isn't one). */
static void open_file_path_into_editor(const char* path, const char* label)
{
    ui_node* existing = find_open_window(path);
    if (existing)
    {
        ui_screen_show_window(g_screen, existing);
        return;
    }

    FILE* f = fopen(path, "rb");
    if (!f)
    {
        ui_msgbox_button ok = { "   OK   ", 0 };
        char message[1536];
        snprintf(message, sizeof message, "File not found:\n%s", path);
        ui_message_box(g_screen, "Error", message, &ok, 1);
        return;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size < 0)
        size = 0;

    char* content = malloc((size_t)size + 1);
    if (content)
    {
        size_t got = fread(content, 1, (size_t)size, f);
        content[got] = 0;
        normalize_newlines(content);
    }
    fclose(f);

    char title[300];
    snprintf(title, sizeof title, " %s ", label);
    ui_screen_show_window(g_screen,
        make_editor_window(g_root, g_new_count++, title, content ? content : "", path));
    free(content);
}

/* Navigate Back/Forward (status bar): a browser-style history of the
 * document/caret/scroll positions "jumped from" by an explicit navigation -
 * Go to line, an Output double-click, a Markdown link, Toggle Header/Source,
 * and opening a file (File > Open / the Folder panel). Ordinary typing and
 * caret movement never push an entry, so Back always lands somewhere
 * meaningfully different, same as Visual Studio's Navigate Backward. A
 * position is stored by path (not a raw ui_node* - open_link_in_window
 * mutates a window's content/path in place rather than opening a new one,
 * so a window pointer alone wouldn't reliably identify "the file that was
 * open there") and re-located the same way output_goto_source() does: reuse
 * the window if it's still open, else reopen it from disk. */
typedef struct
{
    char path[1024];
    int cursor;
    int scroll;
} nav_pos;

#define NAV_STACK_MAX 64
/* Navigate Back/Forward history - the two stacks and the re-entry guard
 * that keeps a restore from recording itself as a new jump. */
static struct
{
    nav_pos back[NAV_STACK_MAX];
    int back_count;
    nav_pos fwd[NAV_STACK_MAX];
    int fwd_count;
    int restoring;
} g_nav;

/* Fills `out` from g_active_editor_window's current path/caret/scroll, or
 * returns 0 if there's no active document window to capture (every window
 * closed, or a Folder/Output panel is frontmost with no document behind
 * it). */
static int nav_capture(nav_pos* out)
{
    ui_node* win = g_active_editor_window;
    ui_node* ed = win ? editor_in_window(win) : NULL;
    if (!ed)
        return 0;
    const char* path = ui_get_path(win);
    if (!path[0])
        return 0;
    strncpy(out->path, path, sizeof out->path - 1);
    out->path[sizeof out->path - 1] = 0;
    out->cursor = ui_editor_get_cursor(ed);
    out->scroll = ui_editor_get_scroll(ed);
    return 1;
}

/* Pushes `pos` onto `stack`/`*count` (capped at NAV_STACK_MAX), dropping the
 * oldest entry to make room - a session rarely needs more than a handful of
 * hops back, so this is generous rather than exact. */
static void nav_push(nav_pos* stack, int* count, const nav_pos* pos)
{
    if (*count == NAV_STACK_MAX)
    {
        memmove(&stack[0], &stack[1], (NAV_STACK_MAX - 1) * sizeof(nav_pos));
        (*count)--;
    }
    stack[(*count)++] = *pos;
}

/* Records the current position as a Back stop and clears the Forward stack -
 * call this right before an explicit jump actually moves the caret/window,
 * so Back always returns to where the jump was made FROM. A no-op while
 * g_nav.restoring (see above) or if there's nothing to capture. */
static void nav_record_jump(void)
{
    if (g_nav.restoring)
        return;
    nav_pos here;
    if (!nav_capture(&here))
        return;
    nav_push(g_nav.back, &g_nav.back_count, &here);
    g_nav.fwd_count = 0;
}

/* Opens/raises `pos`'s window - reusing find_open_window/
 * open_file_path_into_editor the same way output_goto_source() does for a
 * result that isn't currently open - and restores its caret + scroll.
 * Silently gives up if the file isn't open and can't be reopened (e.g. an
 * untitled NONAMEnn.C buffer that was since closed), same "already reported,
 * nothing more to do" reasoning as output_goto_source(). */
static void nav_restore(const nav_pos* pos)
{
    ui_node* win = find_open_window(pos->path);
    if (!win)
    {
        open_file_path_into_editor(pos->path, label_for_path(pos->path));
        win = find_open_window(pos->path);
        if (!win)
            return;
    }
    ui_node* ed = editor_in_window(win);
    if (!ed)
        return;

    ui_screen_show_window(g_screen, win);
    ui_editor_set_selection(ed, pos->cursor, pos->cursor);
    ui_editor_set_scroll(ed, pos->scroll);
    g_goto_pending_focus = ed;  /* focus after this update finishes - see app_frame */
}

static void nav_back(void)
{
    if (g_nav.back_count == 0)
        return;
    nav_pos here;
    int have_here = nav_capture(&here);

    nav_pos target = g_nav.back[--g_nav.back_count];
    if (have_here)
        nav_push(g_nav.fwd, &g_nav.fwd_count, &here);

    g_nav.restoring = 1;
    nav_restore(&target);
    g_nav.restoring = 0;
}

static void nav_forward(void)
{
    if (g_nav.fwd_count == 0)
        return;
    nav_pos here;
    int have_here = nav_capture(&here);

    nav_pos target = g_nav.fwd[--g_nav.fwd_count];
    if (have_here)
        nav_push(g_nav.back, &g_nav.back_count, &here);

    g_nav.restoring = 1;
    nav_restore(&target);
    g_nav.restoring = 0;
}

/* True if `path` names an existing regular file - used so pasting/typing a
 * full path into the Open dialog's Name field and pressing Enter/OK can open
 * it immediately, rather than the field's normal dir+mask split (see
 * EVT_OPEN_NAME/EVT_OPEN_OK) navigating into it as if it were just a new
 * mask. Deliberately narrow: directories and nonexistent paths fall through
 * to the existing split/navigate behavior unchanged. */
static int path_is_regular_file(const char* path)
{
    if (!path || !path[0])
        return 0;
    struct stat st;
    if (stat(path, &st) != 0)
        return 0;
    /* MSVC's <sys/stat.h> doesn't define the POSIX S_ISREG macro (only
     * S_IFREG/S_IFMT), so test the mode bits directly - works the same on
     * both. */
    return (st.st_mode & S_IFMT) == S_IFREG ? 1 : 0;
}

/* Adds every checked row of the Open dialog's listbox to the open project -
 * the multi-select half of Project > "Add Existing File..." (see
 * g_open.allow_multi). Directory rows (marked with a trailing "\", see
 * populate_listbox_from_dir) are skipped: a checked directory has no
 * meaning here, and the user navigates with a double-click anyway. Returns
 * how many files were actually added, so a listing with nothing checked
 * reads as 0 and the caller can fall back to the single-row behavior. */
static int open_dialog_add_checked_files(void)
{
    int added = 0;
    int count = ui_child_count(g_open.listbox);
    for (int i = 0; i < count; i++)
    {
        if (!ui_group_get_checked(g_open.listbox, i))
            continue;
        const char* label = ui_get_label(ui_child_at(g_open.listbox, i));
        size_t len = label ? strlen(label) : 0;
        if (len == 0 || label[len - 1] == '\\')
            continue;
        char path[1024];
        path_join(path, sizeof path, g_open.dir, label);
        project_add_file(path);
        added++;
    }
    return added;
}

static void open_dialog_activate(int index)
{
    if (index < 0 || index >= ui_child_count(g_open.listbox))
        return;
    const char* label = ui_get_label(ui_child_at(g_open.listbox, index));
    if (!label[0])
        return;

    if (dir_row_navigate(g_open.dir, sizeof g_open.dir, label))
    {
        open_dialog_refresh();
        return;
    }

    /* Save mode (and New Project, the same Save-As shape): picking a file row
     * doesn't open it - it drops that name into the Name field to overwrite
     * (the user still confirms via Save). */
    if (g_open.dialog_mode == OPEN_DLG_SAVE || g_open.dialog_mode == OPEN_DLG_PROJECT_NEW)
    {
        strncpy(g_save_name, label, sizeof g_save_name - 1);
        g_save_name[sizeof g_save_name - 1] = 0;
        open_dialog_refresh();
        return;
    }

    /* External Tools' Command browse: picking a file row just fills that
     * field in - see exttool_browse_pick(). */
    if (g_open.dialog_mode == OPEN_DLG_EXTTOOL_CMD)
    {
        char path[1024];
        path_join(path, sizeof path, g_open.dir, label);
        exttool_browse_pick(path);
        ui_screen_close_modal(g_screen, g_open.modal);
        g_open.dialog_mode = OPEN_DLG_FILE;
        return;
    }

    /* Project Open/Add File modes: picking a file row acts on the project
     * instead of opening the file as a text document - see
     * project_open_file()/project_add_file(). */
    if (g_open.dialog_mode == OPEN_DLG_PROJECT_OPEN || g_open.dialog_mode == OPEN_DLG_PROJECT_ADDFILE)
    {
        char path[1024];
        path_join(path, sizeof path, g_open.dir, label);
        if (g_open.dialog_mode == OPEN_DLG_PROJECT_OPEN)
            project_open_file(path);
        else
            project_add_file(path);
        ui_screen_close_modal(g_screen, g_open.modal);
        g_open.dialog_mode = OPEN_DLG_FILE;
        return;
    }

    char path[1024];
    path_join(path, sizeof path, g_open.dir, label);
    nav_record_jump();
    open_file_path_into_editor(path, label);
    ui_screen_close_modal(g_screen, g_open.modal);
}

/* Rebuilds the persistent folder browser window's listbox from g_folder.dir
 * (only source files - see the mask - plus all subdirectories for
 * navigation, see populate_listbox_from_dir), and retitles the window to
 * show the directory it's now showing. */
static void folder_window_refresh(void)
{
    ide_path_normalize(g_folder.dir);
    populate_listbox_from_dir(g_folder.listbox, g_folder.dir, "*.h;*.c;*.md;*.txt", 0);

    /* Just the folder's own name, not the full path - there's no room for
     * that in the title bar. */
    ui_node* window = ui_child_at(g_folder.window, 0);
    if (window)
    {
        char title[320];
        snprintf(title, sizeof title, " %s ", basename_of(g_folder.dir));
        ui_set_label(window, title);
    }
}

/* A row in the folder browser window was activated (double-click/Enter) -
 * a directory navigates the listing in place; a file opens it, same as
 * picking one in the Open dialog (open_dialog_activate) does, just with no
 * modal to close afterward since this window isn't one. Reads the row's
 * ui_get_path, not its ui_get_label - the Folder panel may be showing an
 * index.txt title in the label (see populate_listbox_from_dir), but the
 * path always still carries the real disk name underneath, which is what
 * navigation/opening (and the opened editor window's own title) need. */
static void folder_window_activate(int index)
{
    if (index < 0 || index >= ui_child_count(g_folder.listbox))
        return;
    const char* entry = ui_get_path(ui_child_at(g_folder.listbox, index));
    if (!entry[0])
        return;

    if (dir_row_navigate(g_folder.dir, sizeof g_folder.dir, entry))
    {
        folder_window_refresh();
        return;
    }

    char path[1024];
    path_join(path, sizeof path, g_folder.dir, entry);
    nav_record_jump();
    open_file_path_into_editor(path, entry);
}

/* Closes `other` (a docked-panel wrapper, e.g. g_folder.window/g_project.
 * window/g_git.window) if it's currently shown and still sitting on the same
 * LEFT dock as `keep` - shared by folder_show_panel()/project_show_panel()/
 * git_show_panel() so Folder, Project and Git Changes stay mutually
 * exclusive on LEFT (dock_layout() only lays out one window per side, and
 * showing more than one of these file-browsing panels at once would just be
 * visual clutter over the same job). A no-op once either side has been
 * redocked elsewhere via g_dockmenu's "Dock Left/Right/Bottom" popup.
 *
 * Returns `other`'s width right before closing it (0 if nothing was closed) -
 * each panel keeps its own dock width (set at ui_set_dock() time in
 * app_init, and updated independently by dragging its own border), so
 * switching from one to another without this would silently snap back to
 * whichever default width the incoming panel happened to be built with,
 * undoing a resize the user made on the outgoing one. Callers pass this
 * width into their own ui_set_dock() before showing `keep`, so the LEFT
 * dock's width carries over across the switch - see folder_show_panel()/
 * project_show_panel()/git_show_panel(). */
static int close_other_left_panel(ui_node* other, ui_node* keep)
{
    if (window_is_shown(other) &&
        ui_get_dock(ui_child_at(other, 0)) == UI_DOCK_LEFT &&
        ui_get_dock(ui_child_at(keep, 0)) == UI_DOCK_LEFT)
    {
        int w = 0;
        ui_get_rect(ui_child_at(other, 0), NULL, NULL, &w, NULL);
        ui_screen_close_modal(g_screen, other);
        return w;
    }
    return 0;
}

/* Raises the Folder panel - the exact mirror of project_show_panel()/
 * git_show_panel() (defined further below, since they need g_project's/
 * g_git's helpers) - see close_other_left_panel()'s own doc comment for why
 * Project and Git Changes are closed first, and why their width carries
 * over. */
static void folder_show_panel(void)
{
    int w = close_other_left_panel(g_project.window, g_folder.window);
    if (!w)
        w = close_other_left_panel(g_git.window, g_folder.window);
    else
        close_other_left_panel(g_git.window, g_folder.window);
    if (w)
        ui_set_dock(ui_child_at(g_folder.window, 0), UI_DOCK_LEFT, w);

    ui_screen_show_window(g_screen, g_folder.window);
}

/* File > Open Folder...'s OK/"Select" confirmation (EVT_OPEN_OK while
 * g_open.dialog_mode == OPEN_DLG_FOLDER): point the singleton folder browser
 * window at whichever directory the picker had navigated to, close the
 * picker, and raise it. */
static void folder_select_confirm(void)
{
    strncpy(g_folder.dir, g_open.dir, sizeof g_folder.dir - 1);
    g_folder.dir[sizeof g_folder.dir - 1] = 0;
    folder_window_refresh();

    ui_screen_close_modal(g_screen, g_open.modal);
    g_open.dialog_mode = OPEN_DLG_FILE;
    folder_show_panel();
}

/* Points the persistent Folder panel at `dir` and raises it - the editor
 * popup's "Show My Folder" (EVT_EDITOR_SHOW_FOLDER), so it lands the user on
 * the relevant folder without a separate File > Open Folder... trip. Same "write g_folder.dir
 * directly, then refresh+show" idiom folder_select_confirm uses above, just
 * without an Open dialog to close first - neither caller has one open. A
 * no-op for a NULL/empty dir (e.g. a document with no path yet). */
static void folder_reveal_directory(const char* dir)
{
    if (!dir || !dir[0])
        return;
    strncpy(g_folder.dir, dir, sizeof g_folder.dir - 1);
    g_folder.dir[sizeof g_folder.dir - 1] = 0;
    folder_window_refresh();
    folder_show_panel();
}

/* True if a project is currently open - g_project.file_path is only ever
 * non-empty between a successful project_new_create()/project_open_file()
 * and the matching EVT_PROJECT_CLOSE (project_close()). */
static int project_is_open(void)
{
    return g_project.file_path[0] != 0;
}

/* Rewrites `abs_path` relative to `base_dir` when it actually sits inside
 * base_dir (the common case - a project's files normally live under its own
 * directory tree); anything else is kept as an absolute path unchanged rather
 * than forced into a nonsensical "../../.." chain. */
static void project_make_relative(const char* base_dir, const char* abs_path,
                                   char* out, size_t out_size)
{
    /* Compare on normalized slashes: base_dir is always stored '/'-only
     * (see project_load_from_file), while abs_path arrives straight from
     * the platform Open dialog, which on Windows hands back backslashes -
     * a raw strncmp between the two never matches, so every added file
     * fell through to the absolute-path branch below even when it sat
     * right inside the project directory. */
    char norm[1024];
    snprintf(norm, sizeof norm, "%s", abs_path);
    ide_path_normalize(norm);

    size_t base_len = strlen(base_dir);
    if (base_len > 0 && strncmp(norm, base_dir, base_len) == 0 &&
        norm[base_len] == '/')
        snprintf(out, out_size, "%s", norm + base_len + 1);
    else
        snprintf(out, out_size, "%s", norm);
    ide_path_normalize(out);
}

/* Resolves a files[]/include_dirs[] entry back to an absolute path - the
 * inverse of project_make_relative(). An entry that was stored absolute
 * (project_make_relative's own fallback) is returned as-is. */
static void project_abs_path(const char* relative_path, char* out, size_t out_size)
{
    int is_absolute = relative_path[0] == '/' || relative_path[0] == '\\' ||
        (isalpha((unsigned char)relative_path[0]) && relative_path[1] == ':');
    if (is_absolute)
        snprintf(out, out_size, "%s", relative_path);
    else
        path_join(out, out_size, g_project.dir, relative_path);
}

/* Clears every data field of g_project back to "no project open" - never
 * touches window/listbox/popup/view_item, which are built once at startup
 * and outlive any particular project being open or closed. Shared by
 * project_close() and project_load_from_file() (which resets before loading
 * the file it was just handed, so a failed/partial load can't leave the
 * previous project's entries mixed in with the new one's). */
static void project_reset_data(void)
{
    g_project.file_path[0] = 0;
    g_project.dir[0] = 0;
    g_project.name[0] = 0;
    g_project.file_count = 0;
    g_project.include_count = 0;
    g_project.compile = g_compile;  /* start from whatever the IDE is
                                     * currently set to - see g_project.
                                     * compile's own doc comment */
}

/* Whether `abs_path` is one of the open project's own g_project.files[] -
 * same case-insensitive, '/'-or-'\\'-alike comparison find_open_window()
 * uses to match an editor window to a path, since files[] entries and an
 * open document's own path can each spell separators either way. No project
 * open is trivially "no". */
static int project_contains_file(const char* abs_path)
{
    if (!project_is_open() || !abs_path)
        return 0;
    for (int i = 0; i < g_project.file_count; i++)
    {
        char entry_abs[1024];
        project_abs_path(g_project.files[i], entry_abs, sizeof entry_abs);
        const char* a = entry_abs;
        const char* b = abs_path;
        while (*a && *b)
        {
            char ca = *a == '\\' ? '/' : (char)tolower((unsigned char)*a);
            char cb = *b == '\\' ? '/' : (char)tolower((unsigned char)*b);
            if (ca != cb)
                break;
            a++;
            b++;
        }
        if (!*a && !*b)
            return 1;
    }
    return 0;
}

/* Which compile_settings a compile/build should actually use right now -
 * the open project's own (g_project.compile) when `file` is actually one of
 * that project's files, else the IDE-wide default (g_compile). Being part of
 * an open project isn't enough on its own (see project_contains_file()) - a
 * Playground/scratch file compiled while some unrelated project happens to
 * be open still gets the IDE-wide settings, not that project's. `file` may
 * be NULL/"" (no active document, or a caller with nothing file-specific to
 * check, e.g. Project > "Options..." on the panel itself) - treated as "not
 * in the project", same as any other non-member path. do_project_build()
 * doesn't go through this at all: it always means the project's own
 * settings, unconditionally. */
static compile_settings* active_compile_settings(const char* file)
{
    return project_contains_file(file) ? &g_project.compile : &g_compile;
}

/* Rebuilds the Project panel's listbox from g_project.files[] (empty when no
 * project is open) and retitles the window - same shape as
 * folder_window_refresh(). */
/* The Project panel's own per-row file-type marker: one letter plus a
 * trailing space, colored from the current theme (see ui_theme's
 * project_icon_*_fg fields) - "C" for .c, "H" for .h, "M" for .md. Any other
 * extension still gets the same two reserved columns (so names stay
 * column-aligned regardless of type), just as a blank, uncolored marker -
 * see render_listbox's own "fg 0 means no tint" rule. */
static void project_file_marker(const char* filename, char* out_marker, uint32_t* out_fg)
{
    const ui_theme* theme = ui_get_theme();
    const char* dot = strrchr(filename, '.');
    if (dot && strcmp(dot, ".c") == 0)
    {
        strcpy(out_marker, "C ");
        *out_fg = theme->project_icon_c_fg;
    }
    else if (dot && strcmp(dot, ".h") == 0)
    {
        strcpy(out_marker, "H ");
        *out_fg = theme->project_icon_h_fg;
    }
    else if (dot && strcmp(dot, ".md") == 0)
    {
        strcpy(out_marker, "M ");
        *out_fg = theme->project_icon_md_fg;
    }
    else
    {
        strcpy(out_marker, "  ");
        *out_fg = 0;
    }
}

static void project_window_refresh(int selected_index)
{
    if (!g_project.listbox)
        return;

    while (ui_child_count(g_project.listbox) > 0)
    {
        ui_node* child = ui_child_at(g_project.listbox, 0);
        ui_remove_child(g_project.listbox, child);
        ui_node_free(child);
    }

    for (int i = 0; i < g_project.file_count; i++)
    {
        ui_node* item = ui_create_element(UI_TAG_ITEM);
        char marker[8], label[520];
        uint32_t marker_fg;
        project_file_marker(g_project.files[i], marker, &marker_fg);
        snprintf(label, sizeof label, "%s%s", marker, g_project.files[i]);
        ui_set_label(item, label);
        ui_set_color(item, marker_fg, 0);
        ui_set_path(item, g_project.files[i]);
        ui_append_child(g_project.listbox, item);
    }
    ui_select_set_selected(g_project.listbox, selected_index);

    ui_node* window = ui_child_at(g_project.window, 0);
    if (window)
    {
        char title[320];
        snprintf(title, sizeof title, " %s ", g_project.name[0] ? g_project.name : "Project");
        ui_set_label(window, title);
    }
}

/* Rebuilds the Include Directories dialog's own listbox from
 * g_project.include_dirs[] - same clear-then-repopulate shape as
 * project_window_refresh(), just for the dialog opened separately via
 * Project > "Include Directories..." (EVT_PROJECT_INCLUDES) rather than the
 * always-visible panel. A no-op before app_init builds the dialog. */
static void project_includes_dialog_refresh(int selected_index)
{
    if (!g_project.includes_listbox || !g_includes_editing.dirs)
        return;

    while (ui_child_count(g_project.includes_listbox) > 0)
    {
        ui_node* child = ui_child_at(g_project.includes_listbox, 0);
        ui_remove_child(g_project.includes_listbox, child);
        ui_node_free(child);
    }

    for (int i = 0; i < *g_includes_editing.count; i++)
    {
        ui_node* item = ui_create_element(UI_TAG_ITEM);
        ui_set_label(item, g_includes_editing.dirs[i]);
        ui_append_child(g_project.includes_listbox, item);
    }
    ui_select_set_selected(g_project.includes_listbox, selected_index);
}

/* Points the Include Directories dialog at one of the two lists and saves
 * whichever file owns it - see g_includes_editing. */
static void includes_edit_project(void)
{
    g_includes_editing.dirs = g_project.include_dirs;
    g_includes_editing.count = &g_project.include_count;
    g_includes_editing.is_project = 1;
}

static void includes_edit_global(void)
{
    g_includes_editing.dirs = g_include_dirs;
    g_includes_editing.count = &g_include_count;
    g_includes_editing.is_project = 0;
}

/* Attaches the dialog's "Detect" button only for the global (system) list -
 * a project's own include directories are the user's, nothing to detect.
 * Same detach/reattach idiom as open_dialog_set_filter_visible(), and for the
 * same reason the rect is re-derived from the window's current position. */
static void includes_set_detect_visible(int visible)
{
    ui_remove_child(g_project.includes_window, g_project.includes_detect);
    if (!visible)
        return;

    int wx, wy, ww, wh;
    ui_get_rect(g_project.includes_window, &wx, &wy, &ww, &wh);
    const int bw = 13;  /* same as the other buttons, see build_screen() */
    ui_set_rect(g_project.includes_detect, wx + ww - bw - 3, wy + 11, bw, 1);
    ui_append_child(g_project.includes_window, g_project.includes_detect);

    /* Tab follows child order: move Close back behind Detect, which sits
     * above it on screen. */
    ui_node* close = ui_find_by_id(g_project.includes_window, EVT_PROJECT_INCLUDES_CLOSE);
    if (close)
    {
        ui_remove_child(g_project.includes_window, close);
        ui_append_child(g_project.includes_window, close);
    }
}

/* Defined below with the rest of the ".cakeproj" JSON helpers - the shared
 * shape for compiler options, used by both a project and the session file. */
static void compile_settings_to_json(struct json_value* object, const compile_settings* c);
static void compile_settings_from_json(const struct json_value* object, compile_settings* c);

/* Writes the open project back out to g_project.file_path as a ".cakeproj"
 * file - real JSON, built as a json_value tree and serialized by json.c, so
 * a name or path containing a quote or backslash comes back out intact (see
 * project_load_from_file for the matching reader). No-op if no project is
 * open, and silent if the write fails, same as the fopen() it replaced. */
static void project_save(void)
{
    if (!project_is_open())
        return;

    struct json_value* _Opt _Owner root = calloc(1, sizeof *root);
    if (!root)
        return;
    root->type = JSON_OBJECT;

    json_set_string(root, "name", g_project.name);

    /* Compiler options, scoped to this project - see g_project.compile's own
     * doc comment. Nested under "compile" in the same shape the session file
     * uses (compile_settings_to_json), so both are read back by the same
     * code. */
    compile_settings_to_json(json_set_object(root, "compile"), &g_project.compile);

    struct json_value* includes = json_set_array(root, "include_dirs");
    for (int i = 0; i < g_project.include_count; i++)
        json_add_string(includes, g_project.include_dirs[i]);

    struct json_value* files = json_set_array(root, "files");
    for (int i = 0; i < g_project.file_count; i++)
        json_add_string(files, g_project.files[i]);

    json_write_file(g_project.file_path, root);
    json_delete(root);
    g_project.file_time = file_mtime(g_project.file_path);
}

/* Copies the string member `key` into `out`, leaving `out` untouched if the
 * member is missing or isn't a string - so a ".cakeproj" written before a
 * field existed keeps whatever default the caller preloaded. */
static void project_json_get_string(const struct json_value* object, const char* key,
                                    char* out, size_t out_size)
{
    const struct json_value* member = json_find_member(object, key);
    if (member && member->type == JSON_STRING)
        snprintf(out, out_size, "%s", member->string);
}

/* Reads a compile-settings flag, written by project_save() as true/false.
 * Returns `fallback` when the member is missing or isn't a boolean - so a
 * ".cakeproj" saved before a flag existed keeps the caller's default. */
static int project_json_get_bool(const struct json_value* object, const char* key, int fallback)
{
    const struct json_value* member = json_find_member(object, key);
    if (!member)
        return fallback;

    if (member->type == JSON_TRUE)
        return 1;
    if (member->type == JSON_FALSE)
        return 0;

    return fallback;
}

/* Reads a numeric member. Returns `fallback` when it is missing or isn't a
 * number - so a file written before a field existed keeps the default. */
static int project_json_get_int(const struct json_value* object, const char* key, int fallback)
{
    const struct json_value* member = object ? json_find_member(object, key) : NULL;
    if (member && member->type == JSON_NUMBER)
        return (int)member->number;

    return fallback;
}

/* Copies the string elements of the array member `key` into `out`, stopping
 * at `max` entries. Non-string elements are skipped. Returns how many were
 * copied. */
static int project_json_get_string_array(const struct json_value* object, const char* key,
                                         char out[][512], int max, size_t entry_size)
{
    const struct json_value* array = json_find_member(object, key);
    if (!array || array->type != JSON_ARRAY)
        return 0;

    int count = 0;
    for (const struct json_value* item = array->first_child;
         item != NULL && count < max;
         item = item->next)
    {
        if (item->type != JSON_STRING)
            continue;
        snprintf(out[count], entry_size, "%s", item->string);
        count++;
    }

    return count;
}

/* Writes `c` as the members of `object` - the shared shape for compiler
 * options, used both by a project's own "compile" object (project_save) and
 * by the IDE's session file (save_session), so the two never drift apart. */
static void compile_settings_to_json(struct json_value* object, const compile_settings* c)
{
    json_set_string(object, "options", c->options);
    json_set_string(object, "output", c->output);
    json_set_string(object, "target", c->target ? c->target : "");
    json_set_string(object, "style", c->style ? c->style : "");
    json_set_string(object, "diagnostic_format", c->diagnostic_format ? c->diagnostic_format : "");
    json_set_bool(object, "no_output", c->no_output);
    json_set_bool(object, "line_directives", c->line_directives);
    json_set_bool(object, "fanalyzer", c->fanalyzer);
    json_set_bool(object, "const_literal", c->const_literal);
    json_set_bool(object, "wall", c->wall);
    json_set_bool(object, "unused_extern_report", c->unused_extern_report);
    json_set_bool(object, "use_cake_headers", c->use_cake_headers);
}

/* Reads a "compile" object back into `c`, leaving any field the object
 * doesn't carry at whatever the caller preloaded - so a file written before
 * a setting existed just keeps the caller's default.
 *
 * target/style/diagnostic_format are slugs, always one of a fixed set (see
 * g_target_slugs/g_style_slugs/g_diagformat_slugs): each resolves back to
 * the matching interned slug pointer (index 0 for anything unrecognized,
 * same as the Compiler Options dialog itself does), so `c` never ends up
 * pointing into the parsed tree, which is freed by the caller. */
static void compile_settings_from_json(const struct json_value* object, compile_settings* c)
{
    if (!object || object->type != JSON_OBJECT)
        return;

    project_json_get_string(object, "options", c->options, sizeof c->options);
    project_json_get_string(object, "output", c->output, sizeof c->output);

    const struct json_value* member = json_find_member(object, "target");
    if (member && member->type == JSON_STRING)
        c->target = g_target_slugs[target_slug_to_index(member->string)];

    member = json_find_member(object, "style");
    if (member && member->type == JSON_STRING)
        c->style = g_style_slugs[style_slug_to_index(member->string)];

    member = json_find_member(object, "diagnostic_format");
    if (member && member->type == JSON_STRING)
        c->diagnostic_format = g_diagformat_slugs[diagformat_slug_to_index(member->string)];

    c->no_output = project_json_get_bool(object, "no_output", c->no_output);
    c->line_directives = project_json_get_bool(object, "line_directives", c->line_directives);
    c->fanalyzer = project_json_get_bool(object, "fanalyzer", c->fanalyzer);
    c->const_literal = project_json_get_bool(object, "const_literal", c->const_literal);
    c->wall = project_json_get_bool(object, "wall", c->wall);
    c->unused_extern_report = project_json_get_bool(object, "unused_extern_report", c->unused_extern_report);
    c->use_cake_headers = project_json_get_bool(object, "use_cake_headers", c->use_cake_headers);
}

/* The IDE executable's own directory in `out`, or - if that can't be
 * determined - the current directory ("" if neither is known).
 * get_self_path()'s return value means different things per platform
 * (fs.c: the path length on Windows, 0 elsewhere), so success is judged by
 * the path it fills in, as tokenizer.c does. */
static void exe_dir(char* out, size_t cap)
{
    char path[FS_MAX_PATH] = { 0 };
    get_self_path(path, sizeof path);
    if (path[0])
    {
        snprintf(out, cap, "%s", path);
        dirname(out);
        return;
    }
    if (!ui_get_cwd(out, (int)cap))
        out[0] = 0;
}

/* "cake.json" - the global compiler settings, kept beside the executable
 * itself (same place as cake.json), not in
 * the per-user config directory session.json lives in: these belong to the
 * install, not to one window layout.
 *
 * Falls back to a bare "cake.json" in the current directory when the
 * executable's own path can't be determined, same as cake.json does. */
static int get_global_settings_path(char* buf, size_t cap)
{
    char exe_path[FS_MAX_PATH] = { 0 };
    get_self_path(exe_path, sizeof exe_path);
    if (!exe_path[0])
    {
        snprintf(buf, cap, "%s", "cake.json");
        return 1;
    }

    char exe_dir[FS_MAX_PATH];
    snprintf(exe_dir, sizeof exe_dir, "%s", exe_path);
    dirname(exe_dir);
    snprintf(buf, cap, "%s/cake.json", exe_dir);
    return 1;
}

/* Writes g_compile out to cake.json. These are the settings used whenever
 * the file being compiled isn't part of the open project - Playground, a
 * scratch file, anything opened on its own (see active_compile_settings()) -
 * so a project's own ".cakeproj" always overrides them.
 *
 * Same "compile" node a ".cakeproj" carries, written by the same helper: a
 * project file is this plus a name and its own file list. Returns whether it
 * was written. */
static bool global_settings_save(void)
{
    char path[FS_MAX_PATH];
    if (!get_global_settings_path(path, sizeof path))
        return false;

    struct json_value* _Opt _Owner root = calloc(1, sizeof *root);
    if (!root)
        return false;
    root->type = JSON_OBJECT;

    compile_settings_to_json(json_set_object(root, "compile"), &g_compile);

    struct json_value* includes = json_set_array(root, "include_dirs");
    for (int i = 0; i < g_include_count; i++)
        json_add_string(includes, g_include_dirs[i]);

    bool ok = json_write_file(path, root);
    json_delete(root);
    return ok;
}

/* Reads cake.json back into g_compile, called once at startup. Anything
 * missing - no file yet, an unparseable one, or a setting added since it was
 * written - just leaves that field at its built-in default. */
static void global_settings_load(void)
{
    char path[FS_MAX_PATH];
    if (!get_global_settings_path(path, sizeof path))
        return;

    char* text = read_file_to_string(path);
    if (!text)
        return;

    struct json_value* root = json_parse(text, NULL);
    free(text);

    compile_settings_from_json(json_find_member(root, "compile"), &g_compile);
    g_include_count = project_json_get_string_array(root, "include_dirs", g_include_dirs,
                                                    CAKE_PROJECT_MAX_INCLUDES,
                                                    sizeof g_include_dirs[0]);
    json_delete(root);
}

/* Loads a ".cakeproj" file written by project_save() into g_project, fully
 * replacing whatever project was open before (see project_reset_data()).
 * Returns 0 (leaving g_project untouched) if `path` can't be opened. */
static int project_load_from_file(const char* path)
{
    char* text = read_file_to_string(path);
    if (!text)
        return 0;

    struct json_error json_error;
    struct json_value* root = json_parse(text, &json_error);
    free(text);

    /* Malformed JSON now fails the load outright, where the old line-scanner
     * would silently keep whatever fields it happened to recognize. */
    if (!root)
        return 0;

    if (root->type != JSON_OBJECT)
    {
        json_delete(root);
        return 0;
    }

    char loaded_name[256] = "";
    char loaded_files[CAKE_PROJECT_MAX_FILES][512];
    int loaded_file_count = 0;
    char loaded_includes[CAKE_PROJECT_MAX_INCLUDES][512];
    int loaded_include_count = 0;

    /* Compile settings default to whatever the IDE is currently set to
     * (same as project_reset_data()) - a ".cakeproj" written before these
     * fields existed, or one missing a particular key, just falls back to
     * that instead of some separate hardcoded default. */
    compile_settings loaded_compile = g_compile;
    compile_settings_from_json(json_find_member(root, "compile"), &loaded_compile);

    project_json_get_string(root, "name", loaded_name, sizeof loaded_name);

    loaded_file_count = project_json_get_string_array(root, "files", loaded_files,
                                                      CAKE_PROJECT_MAX_FILES, sizeof loaded_files[0]);
    loaded_include_count = project_json_get_string_array(root, "include_dirs", loaded_includes,
                                                         CAKE_PROJECT_MAX_INCLUDES, sizeof loaded_includes[0]);

    json_delete(root);

    project_reset_data();

    snprintf(g_project.file_path, sizeof g_project.file_path, "%s", path);
    ide_path_normalize(g_project.file_path);
    g_project.file_time = file_mtime(g_project.file_path);

    snprintf(g_project.dir,sizeof g_project.dir, "%s", g_project.file_path);
    char* last_slash = strrchr(g_project.dir, '/');
    if (last_slash)
        *last_slash = 0;
    else
        strcpy(g_project.dir, ".");

    snprintf(g_project.name, sizeof g_project.name,
             "%s", loaded_name[0] ? loaded_name : basename_of(path));

    g_project.compile = loaded_compile;

    g_project.file_count = loaded_file_count;
    for (int i = 0; i < loaded_file_count; i++)
        snprintf(g_project.files[i], sizeof g_project.files[0], "%s", loaded_files[i]);

    g_project.include_count = loaded_include_count;
    for (int i = 0; i < loaded_include_count; i++)
        snprintf(g_project.include_dirs[i], sizeof g_project.include_dirs[0], "%s", loaded_includes[i]);

    return 1;
}

/* Raises the Project panel - shared by every path that opens/creates a
 * project (project_new_create/project_open_file) and by View > "Show
 * Project" (EVT_WINDOW_PROJECT). dock_layout() (ide_ui.c) only lays out one
 * window per side, so showing this while the Folder panel is also sitting on
 * LEFT would just overlap it rather than actually dock - close Folder first
 * in that specific case (only ever fires when both still sit on their
 * original default side; either one redocked elsewhere via g_dockmenu's
 * "Dock Left/Right/Bottom" popup coexists fine, so this is a no-op then). */
static void project_show_panel(void)
{
    int w = close_other_left_panel(g_folder.window, g_project.window);
    if (!w)
        w = close_other_left_panel(g_git.window, g_project.window);
    else
        close_other_left_panel(g_git.window, g_project.window);
    if (w)
        ui_set_dock(ui_child_at(g_project.window, 0), UI_DOCK_LEFT, w);

    ui_screen_show_window(g_screen, g_project.window);
}

/* Defined below (with Save As) - project_new_save_activate() needs it to
 * default the ".cakeproj" extension the same way save_as_activate() does. */
static const char* current_filter_extension(void);

/* Project > "New Project..." - creates <path> (a ".cakeproj" file, empty)
 * and opens it as the current project. `path`'s directory and filename
 * together are the project: its own name (independent of the folder's,
 * so two projects can share one folder) comes straight from what was typed
 * into the Save-As-style dialog (see project_new_save_activate()), same as
 * any other Save As - there's no separate name prompt. */
static void project_new_create(const char* path)
{
    char dir[1024];
    snprintf(dir, sizeof dir, "%s", path);
    ide_path_normalize(dir);
    char* last_slash = strrchr(dir, '/');
    if (last_slash)
        *last_slash = 0;
    else
        strcpy(dir, ".");
    mkdir(dir, 0755);  /* fine if it already exists - return value ignored */

    char name[256];
    snprintf(name, sizeof name, "%s", basename_of(path));
    char* dot = strrchr(name, '.');
    if (dot && strcmp(dot, CAKE_PROJECT_EXT) == 0)
        *dot = 0;

    project_reset_data();
    snprintf(g_project.dir, sizeof g_project.dir, "%s", dir);
    snprintf(g_project.name, sizeof g_project.name, "%s", name);
    snprintf(g_project.file_path, sizeof g_project.file_path, "%s", path);
    ide_path_normalize(g_project.file_path);

    project_save();
    project_window_refresh(0);

    ui_screen_close_modal(g_screen, g_open.modal);
    g_open.dialog_mode = OPEN_DLG_FILE;
    project_show_panel();
}

/* Project > "New Project..."'s own Save-As-style confirmation (EVT_OPEN_NAME/
 * EVT_OPEN_OK while g_open.dialog_mode == OPEN_DLG_PROJECT_NEW) - same
 * "split Name field into dir+filename, default the extension from the
 * current filter, confirm before overwriting" shape as save_as_activate(),
 * just creating a project instead of writing a document's content. */
static void project_new_save_activate(void)
{
    char buf[1024];
    strncpy(buf, ui_get_value(g_open.name_input), sizeof buf - 1);
    buf[sizeof buf - 1] = 0;
    for (char* p = buf; *p; p++)
        if (*p == '\\')
            *p = '/';

    char* sep = NULL;
    for (char* p = buf; *p; p++)
        if (*p == '/')
            sep = p;

    const char* name = buf;
    if (sep)
    {
        *sep = 0;
        name = sep + 1;
        if (buf[0])
        {
            strncpy(g_open.dir, buf, sizeof g_open.dir - 1);
            g_open.dir[sizeof g_open.dir - 1] = 0;
        }
    }
    if (!name[0])
        return;  /* no name typed - leave the dialog open */

    char name_buf[300];
    strncpy(name_buf, name, sizeof name_buf - 1);
    name_buf[sizeof name_buf - 1] = 0;
    if (!strchr(name_buf, '.'))
    {
        const char* ext = current_filter_extension();
        if (ext && strlen(name_buf) + strlen(ext) < sizeof name_buf)
            strcat(name_buf, ext);
    }

    path_join(g_project_new_path, sizeof g_project_new_path, g_open.dir, name_buf);

    FILE* exists = fopen(g_project_new_path, "rb");
    if (exists)
    {
        fclose(exists);
        char msg[400];
        snprintf(msg, sizeof msg, "%s already exists.\nOverwrite?", name_buf);
        ui_msgbox_button btns[] = {
            { "  Yes  ", EVT_PROJECT_NEW_OVERWRITE },
            { "  No  ", 0 },
        };
        ui_message_box(g_screen, "New Project", msg, btns, 2);
        return;
    }

    project_new_create(g_project_new_path);
}

/* Project > "Open Project..." - loads `path` (a ".cakeproj" file picked from
 * the Open dialog in OPEN_DLG_PROJECT_OPEN mode) and raises the panel. Does
 * nothing if the file can't be read (e.g. picked, then deleted out from under
 * the dialog). */
static void project_open_file(const char* path)
{
    if (!project_load_from_file(path))
        return;
    project_window_refresh(0);
    project_show_panel();
}

/* Project > "Add Existing File..." - adds `path` (absolute, from the Open
 * dialog in OPEN_DLG_PROJECT_ADDFILE mode) to the open project, stored
 * relative to g_project.dir. Silently ignored if no project is open, the
 * project is already full, or the file is already listed. */
static void project_add_file(const char* path)
{
    if (!project_is_open() || g_project.file_count >= CAKE_PROJECT_MAX_FILES)
        return;

    char relative_path[512];
    project_make_relative(g_project.dir, path, relative_path, sizeof relative_path);

    for (int i = 0; i < g_project.file_count; i++)
        if (strcmp(g_project.files[i], relative_path) == 0)
            return;

    snprintf(g_project.files[g_project.file_count++], sizeof g_project.files[0], "%s", relative_path);
    project_save();
    project_window_refresh(0);
}

/* Project > "Add Include Directory..." - same shape as project_add_file(),
 * for a directory (from OPEN_DLG_PROJECT_ADDINCLUDE) instead of a file. */
static void project_add_include(const char* path)
{
    if (!g_includes_editing.dirs || *g_includes_editing.count >= CAKE_PROJECT_MAX_INCLUDES)
        return;

    /* A project's entries are stored relative to the project directory (see
     * project_abs_path); the global list has no such directory, so it keeps
     * the absolute path exactly as picked. */
    char entry[512];
    if (g_includes_editing.is_project)
    {
        if (!project_is_open())
            return;
        project_make_relative(g_project.dir, path, entry, sizeof entry);
    }
    else
    {
        snprintf(entry, sizeof entry, "%s", path);
    }

    for (int i = 0; i < *g_includes_editing.count; i++)
        if (strcmp(g_includes_editing.dirs[i], entry) == 0)
            return;

    snprintf(g_includes_editing.dirs[(*g_includes_editing.count)++], 512, "%s", entry);

    if (g_includes_editing.is_project)
        project_save();
    else
        global_settings_save();

    project_includes_dialog_refresh(0);  /* no-op if the dialog isn't built/open */
}

/* Opens the project's file at `index` (a row in g_project.listbox) into an
 * editor - shared by the panel's double-click/Enter (project_window_activate)
 * and its right-click popup's "Open" (EVT_PROJECT_POPUP_OPEN). */
static void project_open_at(int index)
{
    if (index < 0 || index >= g_project.file_count)
        return;
    char abs_path[1024];
    project_abs_path(g_project.files[index], abs_path, sizeof abs_path);
    nav_record_jump();
    open_file_path_into_editor(abs_path, basename_of(abs_path));
}

/* A row in the Project panel was activated (double-click/Enter). Unlike the
 * Folder panel's equivalent (folder_window_activate), every row here is a
 * file - a project's own file list has no directories to navigate into. */
static void project_window_activate(int index)
{
    project_open_at(index);
}

/* The panel's right-click popup's "Remove from Project" - drops the entry at
 * `index` from files[] and re-saves. Never touches the file on disk, only
 * the project's own list of it. */
static void project_remove_at(int index)
{
    if (index < 0 || index >= g_project.file_count)
        return;
    for (int i = index; i + 1 < g_project.file_count; i++)
        snprintf(g_project.files[i], sizeof g_project.files[0], "%s", g_project.files[i + 1]);
    g_project.file_count--;
    project_save();
    int selected_index = index < g_project.file_count ? index : g_project.file_count - 1;
    project_window_refresh(selected_index);
}

/* Project > "Close Project" - clears g_project back to empty and hides the
 * panel. Files stay on disk untouched; only the ".cakeproj" bookkeeping is
 * forgotten (already saved, so nothing here is lost). */
static void project_close(void)
{
    if (!project_is_open())
        return;
    project_reset_data();
    project_window_refresh(0);
    if (window_is_shown(g_project.window))
        ui_screen_close_modal(g_screen, g_project.window);
}

static int open_local_file_in_browser(const char* path, const char* fragment);  /* defined further
                                                            * down with the
                                                            * other process
                                                            * helpers */

/* Opens web/manual.html - the generated HTML manual the installer ships
 * next to the executable (see the "web" entry of the installers in tools/ and build.c's
 * generate_doc) - in the default web browser, rather than into an editor
 * window like the .md help topics. `fragment` is an optional "#anchor"
 * (without the '#') to land on a section - a stable <a id="..."> written
 * by hand in manual.md, not hoedown's positional toc_N ids, which renumber
 * whenever a heading is added. NULL opens the top. Used by Help > Manual
 * (do_help_manual). */
static void open_manual(const char* fragment)
{
    char dir[1024];
    exe_dir(dir, sizeof dir);

    char path[1024];
    snprintf(path, sizeof path, "%s/web/manual.html", dir);

    ui_msgbox_button ok = { "   OK   ", 0 };
    FILE* f = fopen(path, "rb");
    if (!f)
    {
        char msg[1200];
        snprintf(msg, sizeof msg, "Manual not found:\n%s", path);
        ui_message_box(g_screen, "Help", msg, &ok, 1);
        return;
    }
    fclose(f);

    if (!open_local_file_in_browser(path, fragment))
        ui_message_box(g_screen, "Help", "Could not open the web browser.", &ok, 1);
}

/* Help > Manual (EVT_HELP_MANUAL): the manual from the top. */
static void do_help_manual(void)
{
    open_manual(NULL);
}

/* Help > "Cake Website": ui_open_url() with a fixed URL; the message box
 * covers the one thing that can go wrong on this side (no browser handler
 * registered / xdg-open missing - see ui_open_url in ide_ui.h). */
static void do_help_open_link(const char* url)
{
    if (!ui_open_url(url))
    {
        ui_msgbox_button ok = { "   OK   ", 0 };
        char msg[512];
        snprintf(msg, sizeof msg, "Could not open the web browser for:\n%s", url);
        ui_message_box(g_screen, "Help", msg, &ok, 1);
    }
}

/* Display width of a Markdown table cell as the read-only Markdown editor
 * draws it: inline `code` backticks and **bold** markers are hidden there
 * (see render_editor_line_markdown in ide_ui.c), and a UTF-8 sequence is
 * one column. */
static int md_cell_width(const char* s, size_t len)
{
    int w = 0;
    for (size_t i = 0; i < len; i++)
    {
        unsigned char c = (unsigned char)s[i];
        if (c == '`')
            continue;
        if (c == '*' && i + 1 < len && s[i + 1] == '*')
        {
            i++;
            continue;
        }
        if ((c & 0xC0) == 0x80)
            continue;
        w++;
    }
    return w;
}

#define MD_TABLE_MAX_ROWS 64
#define MD_TABLE_MAX_COLS 8

/* Splits one "| a | b |" row into trimmed cells (pointers into `line`).
 * Returns the cell count. */
static int md_table_cells(const char* line, size_t len, const char** cell, size_t* cell_len)
{
    size_t i = 0;
    if (i < len && line[i] == '|')
        i++;
    int n = 0;
    while (i < len && n < MD_TABLE_MAX_COLS)
    {
        size_t start = i;
        while (i < len && line[i] != '|')
            i++;
        size_t a = start, b = i;
        while (a < b && line[a] == ' ')
            a++;
        while (b > a && line[b - 1] == ' ')
            b--;
        if (i >= len && a == b)
            break;
        cell[n] = line + a;
        cell_len[n] = b - a;
        n++;
        if (i < len)
            i++;
    }
    return n;
}

static int md_is_table_rule(const char* line, size_t len)
{
    for (size_t i = 0; i < len; i++)
        if (line[i] != '|' && line[i] != '-' && line[i] != ':' && line[i] != ' ')
            return 0;
    return len > 0;
}

/* Writes the table whose rows are `lines[0..count)` into out[*o..] with
 * its "|" columns aligned: every cell padded to its column's widest display
 * width (md_cell_width - the hidden backticks/bold marks don't count), and
 * the "|---|" rule stretched to match. Still plain Markdown table syntax -
 * the editor just shows it as text, now lined up. */
static void md_emit_table(const char** lines, const size_t* lens, int count,
                          char* out, size_t* o, size_t cap)
{
    const char* cell[MD_TABLE_MAX_ROWS][MD_TABLE_MAX_COLS];
    size_t cell_len[MD_TABLE_MAX_ROWS][MD_TABLE_MAX_COLS];
    int ncell[MD_TABLE_MAX_ROWS];
    int is_rule[MD_TABLE_MAX_ROWS];
    int width[MD_TABLE_MAX_COLS] = { 0 };
    int cols = 0;

    for (int r = 0; r < count; r++)
    {
        is_rule[r] = md_is_table_rule(lines[r], lens[r]);
        ncell[r] = is_rule[r] ? 0 : md_table_cells(lines[r], lens[r], cell[r], cell_len[r]);
        if (ncell[r] > cols)
            cols = ncell[r];
        for (int c = 0; c < ncell[r]; c++)
        {
            int w = md_cell_width(cell[r][c], cell_len[r][c]);
            if (w > width[c])
                width[c] = w;
        }
    }

    for (int r = 0; r < count; r++)
    {
        if (*o + 1 < cap)
            out[(*o)++] = '|';
        for (int c = 0; c < cols; c++)
        {
            if (is_rule[r])
            {
                for (int k = 0; k < width[c] + 2 && *o + 1 < cap; k++)
                    out[(*o)++] = '-';
            }
            else
            {
                const char* t = c < ncell[r] ? cell[r][c] : "";
                size_t tl = c < ncell[r] ? cell_len[r][c] : 0;
                if (*o + 1 < cap)
                    out[(*o)++] = ' ';
                for (size_t k = 0; k < tl && *o + 1 < cap; k++)
                    out[(*o)++] = t[k];
                for (int k = md_cell_width(t, tl); k < width[c] + 1 && *o + 1 < cap; k++)
                    out[(*o)++] = ' ';
            }
            if (*o + 1 < cap)
                out[(*o)++] = '|';
        }
        if (r < count - 1 && *o + 1 < cap)
            out[(*o)++] = '\n';
    }
}

/* Breaks each Markdown paragraph or list-item line of `src` longer than
 * `width` at spaces into `out`. A list item's ("- ", "* ", "1. ")
 * continuation lines are indented under its text, which Markdown reads as
 * the same item. Tables ("|" rows) get their columns aligned instead
 * (md_emit_table) - the Markdown editor draws them as plain text. Headings ("#") and
 * fenced ``` code blocks are copied unchanged - wrapping those would break
 * their syntax. A single word longer than `width` stays whole. */
static void md_wrap_paragraphs(const char* src, int width, char* out, size_t cap)
{
    size_t o = 0;
    int in_fence = 0;
    const char* line = src;
    while (*line && o + 1 < cap)
    {
        const char* end = strchr(line, '\n');
        size_t len = end ? (size_t)(end - line) : strlen(line);

        if (!in_fence && line[0] == '|')
        {
            const char* rows[MD_TABLE_MAX_ROWS];
            size_t row_len[MD_TABLE_MAX_ROWS];
            int count = 0;
            const char* r = line;
            const char* r_end = end;
            size_t r_len = len;
            for (;;)
            {
                if (count < MD_TABLE_MAX_ROWS)
                {
                    rows[count] = r;
                    row_len[count] = r_len;
                    count++;
                }
                if (!r_end || r_end[1] != '|')
                    break;
                r = r_end + 1;
                r_end = strchr(r, '\n');
                r_len = r_end ? (size_t)(r_end - r) : strlen(r);
            }
            md_emit_table(rows, row_len, count, out, &o, cap);
            end = r_end;
            if (!end)
                break;
            if (o + 1 < cap)
                out[o++] = '\n';
            line = end + 1;
            continue;
        }

        int is_fence = len >= 3 && strncmp(line, "```", 3) == 0;
        int keep = in_fence || is_fence || len <= (size_t)width || line[0] == '#';

        int indent = 0;
        if ((line[0] == '-' || line[0] == '*') && len > 1 && line[1] == ' ')
            indent = 2;
        else
        {
            size_t d = 0;
            while (d < len && line[d] >= '0' && line[d] <= '9')
                d++;
            if (d > 0 && d + 1 < len && line[d] == '.' && line[d + 1] == ' ')
                indent = (int)d + 2;
        }
        if (is_fence)
            in_fence = !in_fence;

        if (keep)
        {
            for (size_t i = 0; i < len && o + 1 < cap; i++)
                out[o++] = line[i];
        }
        else
        {
            int col = 0;
            size_t i = 0;
            while (i < len && o + 1 < cap)
            {
                while (i < len && line[i] == ' ')
                    i++;
                size_t w = i;
                while (i < len && line[i] != ' ')
                    i++;
                int wlen = (int)(i - w);
                if (wlen == 0)
                    break;
                if (col > indent && col + 1 + wlen > width)
                {
                    out[o++] = '\n';
                    for (int k = 0; k < indent && o + 1 < cap; k++)
                        out[o++] = ' ';
                    col = indent;
                }
                else if (col > 0)
                {
                    out[o++] = ' ';
                    col++;
                }
                for (size_t k = w; k < i && o + 1 < cap; k++)
                    out[o++] = line[k];
                col += wlen;
            }
        }
        if (!end)
            break;
        if (o + 1 < cap)
            out[o++] = '\n';
        line = end + 1;
    }
    out[o] = 0;
}

/* The warning/error number of a compiler diagnostic line - "warning 10:",
 * "error 1234:" or "warning: 10", in any of the -fdiagnostics-format
 * shapes - or 0 if the line has none (notes carry no number). ANSI color
 * escapes, which the Output window keeps in its text, are skipped. */
static int diagnostic_number_in_line(const char* s, int len)
{
    char clean[512];
    int n = 0;
    for (int i = 0; i < len && n + 1 < (int)sizeof clean; i++)
    {
        if (s[i] == '\x1b' && i + 1 < len && s[i + 1] == '[')
        {
            i += 2;
            while (i < len && !isalpha((unsigned char)s[i]))
                i++;
            continue;
        }
        clean[n++] = s[i];
    }
    clean[n] = 0;

    static const char* const words[] = { "warning", "error" };
    for (int w = 0; w < 2; w++)
    {
        size_t wl = strlen(words[w]);
        for (const char* p = strstr(clean, words[w]); p; p = strstr(p + 1, words[w]))
        {
            if (p > clean && isalnum((unsigned char)p[-1]))
                continue;
            const char* q = p + wl;
            if (*q != ':' && *q != ' ')
                continue;
            if (*q == ':')
                q++;
            while (*q == ' ')
                q++;
            if (!isdigit((unsigned char)*q))
                continue;
            int number = atoi(q);
            while (isdigit((unsigned char)*q))
                q++;
            if (*q == ':' || *q == ' ')
                return number;
        }
    }
    return 0;
}

/* diagnostics.md's "### <number> <title>" section for `number` as a
 * malloc'd Markdown help text whose first line is "# <number> <title>" (see
 * output_diagnostic_help_refresh, which reads the number back from it), or
 * NULL if diagnostics.md can't be read. Looked up in the IDE's web folder,
 * then one level up - the repository root, for an IDE run from src/. */
static char* diagnostic_help_text(int number)
{
    char dir[1024];
    exe_dir(dir, sizeof dir);
    char path[1100];
    snprintf(path, sizeof path, "%s/web/diagnostics.md", dir);
    char* md = read_file_to_string(path);
    if (!md)
    {
        snprintf(path, sizeof path, "%s/../diagnostics.md", dir);
        md = read_file_to_string(path);
    }
    if (!md)
        return NULL;

    const char* start = NULL;
    for (const char* p = md; p; p = strchr(p, '\n'))
    {
        if (*p == '\n')
            p++;
        if (strncmp(p, "### ", 4) == 0 && atoi(p + 4) == number && isdigit((unsigned char)p[4]))
        {
            start = p + 4;
            break;
        }
    }

    size_t cap = 256;
    const char* end = NULL;
    if (start)
    {
        for (end = strchr(start, '\n'); end; end = strchr(end + 1, '\n'))
            if (strncmp(end + 1, "## ", 3) == 0 || strncmp(end + 1, "### ", 4) == 0)
                break;
        if (!end)
            end = start + strlen(start);
        cap += (size_t)(end - start);
    }
    char* out = malloc(cap);
    if (!out)
    {
        free(md);
        return NULL;
    }
    if (!start)
    {
        snprintf(out, cap, "# %d\n\nThis diagnostic has no description in `diagnostics.md`.", number);
        free(md);
        return out;
    }

    /* "\#" is only escaped for the Markdown-to-HTML step; "<!-- runnable -->"
     * marks samples for the web page and means nothing here. */
    size_t o = 0;
    o += (size_t)snprintf(out, cap, "# ");
    for (const char* p = start; p < end && o + 1 < cap;)
    {
        const char* nl = memchr(p, '\n', (size_t)(end - p));
        const char* line_end = nl ? nl : end;
        if (strncmp(p, "<!-- runnable -->", 17) != 0)
        {
            for (const char* c = p; c < line_end && o + 1 < cap; c++)
            {
                if (c[0] == '\\' && c + 1 < line_end && c[1] == '#')
                    continue;
                out[o++] = *c;
            }
            if (nl && o + 1 < cap)
                out[o++] = '\n';
        }
        p = nl ? nl + 1 : end;
    }
    while (o > 0 && (out[o - 1] == '\n' || out[o - 1] == ' '))
        o--;
    out[o] = 0;
    free(md);
    return out;
}

/* Keeps the Output window's help (ui_set_help - the status bar hint, F1
 * for the whole text) on the diagnostic under its caret: the warning/error
 * number of that line, explained from diagnostics.md. The number currently
 * shown is read back from the short help ("<number> <title>"), so the
 * file is only read again when the caret moves to a different diagnostic.
 * Called every frame. */
static void output_diagnostic_help_refresh(void)
{
    ui_node* ed = g_output_editor;
    if (!ed || ui_screen_focused(g_screen) != ed)
        return;

    const char* text = ui_get_value(ed);
    int len = (int)strlen(text);
    int caret = ui_editor_get_cursor(ed);
    if (caret > len)
        caret = len;
    int lo = caret, hi = caret;
    while (lo > 0 && text[lo - 1] != '\n')
        lo--;
    while (hi < len && text[hi] != '\n')
        hi++;
    int number = diagnostic_number_in_line(text + lo, hi - lo);

    const char* current = ui_get_short_help(ed);
    int shown = current ? atoi(current) : 0;
    if (number == shown)
        return;
    if (!number)
    {
        ui_set_help(ed, NULL, NULL);
        return;
    }
    char* help = diagnostic_help_text(number);
    if (help)
    {
        /* The short help is the "# <number> <title>" heading, without "# ". */
        char short_help[200];
        snprintf(short_help, sizeof short_help, "%.*s", (int)strcspn(help + 2, "\n"), help + 2);
        ui_set_help(ed, short_help, help);
        free(help);
        return;
    }
    char short_help[16];
    snprintf(short_help, sizeof short_help, "%d", number);
    char missing[160];
    snprintf(missing, sizeof missing,
             "# %d\n\n`diagnostics.md` was not found next to the IDE.", number);
    ui_set_help(ed, short_help, missing);
}

static void show_help_text(const char* md);

/* F1/a click on the status bar (UI_HINT_DETAILS_ID): the whole Markdown
 * text of what the bar shows (ui_screen_get_hint_text) in g_hintwin.
 * Returns 0, showing nothing, when the bar has no hint. */
static int show_hint_window(void)
{
    char text[8192];
    if (!ui_screen_get_hint_text(g_screen, text, sizeof text))
        return 0;
    show_help_text(text);
    return 1;
}

/* `md` (Markdown) in g_hintwin - the help window F1 and the dialogs' Help
 * buttons share. */
static void show_help_text(const char* md)
{
    /* The editor has no soft wrap - break long paragraph lines to its text
     * width: minus the Markdown editor's 1-column left margin, a matching
     * 1-column right margin, and the scrollbar column. */
    int ex, ey, ew, eh;
    ui_get_rect(g_hintwin.editor, &ex, &ey, &ew, &eh);
    char wrapped[10000];
    md_wrap_paragraphs(md, ew - 3, wrapped, sizeof wrapped);
    ui_set_value(g_hintwin.editor, wrapped);
    ui_editor_set_selection(g_hintwin.editor, 0, 0);
    ui_editor_set_scroll(g_hintwin.editor, 0);
    ui_screen_show_modal(g_screen, g_hintwin.modal);
    ui_screen_focus(g_screen, g_hintwin.editor);
}

static int is_word_char(int c) { return isalnum((unsigned char)c) || c == '_'; }

/* Extracts the C-identifier-ish word (letters/digits/'_') touching `cursor`
 * in `text` into `out` (up to out_cap-1 bytes, always 0-terminated) - the
 * same notion of "word" a double-click would select (see editor_select_word
 * in ui.c), just read-only and exposed to app code, which has no other way
 * to ask "what's under the caret". Returns 1 if the caret was on or right
 * after a word, 0 (leaving `out` empty) if it landed on whitespace/
 * punctuation with no word on either side. */
static int word_at_cursor(const char* text, int len, int cursor, char* out, int out_cap)
{
    if (cursor < 0) cursor = 0;
    if (cursor > len) cursor = len;

    int lo = cursor, hi = cursor;
    /* The caret usually sits right AFTER the word it's "on" (e.g. having
     * just clicked/typed at the end of an identifier) - if so, look one byte
     * back so "printf|" still finds "printf" instead of nothing. */
    if (lo > 0 && !(lo < len && is_word_char((unsigned char)text[lo])) &&
        is_word_char((unsigned char)text[lo - 1]))
    {
        lo--;
        hi--;
    }

    while (lo > 0 && is_word_char((unsigned char)text[lo - 1]))
        lo--;
    while (hi < len && is_word_char((unsigned char)text[hi]))
        hi++;

    if (out_cap > 0)
        out[0] = 0;
    if (lo == hi)
        return 0;

    int wlen = hi - lo;
    if (wlen >= out_cap)
        wlen = out_cap - 1;
    if (wlen > 0)
    {
        memcpy(out, text + lo, wlen);
        out[wlen] = 0;
    }
    return 1;
}

/* Tools > Terminal: opens a native terminal (see ui_open_terminal(), one
 * implementation per backend) in whatever directory is most likely what the
 * user means right now - the active document's own folder if one's open, or
 * else the persistent Folder panel's browsed directory, or else just the
 * app's own working directory. Fire-and-forget, like every other Tools
 * action here; nothing about the new terminal is tracked afterward. */
static void do_open_terminal(void)
{
    char dir[1024] = { 0 };

    if (g_active_editor_window)
    {
        const char* path = ui_get_path(g_active_editor_window);
        if (path[0])
        {
            strncpy(dir, path, sizeof dir - 1);
            dir[sizeof dir - 1] = 0;
            dirname(dir);
        }
    }

    if (!dir[0] && g_folder.dir[0])
    {
        strncpy(dir, g_folder.dir, sizeof dir - 1);
        dir[sizeof dir - 1] = 0;
    }

    if (!dir[0])
        ui_get_cwd(dir, sizeof dir);

    ui_open_terminal(dir);
}

/* Reads `link` (resolved relative to `win`'s own directory, or the app's
 * working directory if `win` has none) and replaces `win`'s <editor> content
 * with it in place - unlike File > Open, this never opens a new
 * window. Retitles the window, re-derives the syntax mode from the new
 * extension, and shows a message box instead of doing nothing if the file
 * doesn't exist. Used by do_editor_ctrlclick() below for Markdown links. */
static void open_link_in_window(ui_node* win, const char* link)
{
    ui_node* editor = editor_in_window(win);
    if (!editor)
        return;

    /* Directory of win's current path, excluding the trailing separator -
     * empty if the path has no directory component (e.g. an unsaved File >
     * New buffer), in which case fall back to the app's own directory. */
    const char* winpath = ui_get_path(win);
    const char* last_sep = NULL;
    for (const char* p = winpath; *p; p++)
        if (*p == '/' || *p == '\\')
            last_sep = p;

    char dir[1024];
    if (last_sep)
    {
        size_t len = (size_t)(last_sep - winpath);
        if (len >= sizeof dir)
            len = sizeof dir - 1;
        memcpy(dir, winpath, len);
        dir[len] = 0;
    }
    else if (!ui_get_cwd(dir, sizeof dir))
    {
        dir[0] = 0;
    }

    char path[1024];
    path_join(path, sizeof path, dir, link);

    FILE* f = fopen(path, "rb");
    if (!f)
    {
        char msg[300];
        snprintf(msg, sizeof msg, "%s not found.", link);
        ui_msgbox_button ok = { "   OK   ", 0 };
        ui_message_box(g_screen, "Open Link", msg, &ok, 1);
        return;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size < 0)
        size = 0;

    char* content = malloc((size_t)size + 1);
    if (content)
    {
        size_t got = fread(content, 1, (size_t)size, f);
        content[got] = 0;
    }
    fclose(f);

    ui_set_value(editor, content ? content : "");
    free(content);
    ui_set_path(win, path);
    ui_set_syntax(editor, syntax_for_path(path));

    ui_node* window_child = ui_child_at(win, 0);
    if (window_child)
    {
        char title[320];
        snprintf(title, sizeof title, " %s ", link);
        ui_set_label(window_child, title);
    }
}

/* Returns 1 if the current caret position on `text` is inside a markdown
 * "[text](path)" link on that line. If so, writes the link target into
 * `link` (up to `link_cap` bytes). */
static int markdown_link_at_cursor(const char* text, int len, int cursor,
                                   char* link, int link_cap)
{
    int ls = cursor;
    while (ls > 0 && text[ls - 1] != '\n')
        ls--;
    int le = cursor;
    while (le < len && text[le] != '\n')
        le++;

    for (const char* p = text + ls; p < text + le; )
    {
        if (*p != '[')
        {
            p++;
            continue;
        }

        const char* rb = memchr(p + 1, ']', (text + le) - (p + 1));
        if (!rb || rb + 1 >= text + le || rb[1] != '(')
        {
            p++;
            continue;
        }

        const char* lp = rb + 2;
        const char* rp = memchr(lp, ')', (text + le) - lp);
        if (!rp)
            break;

        if (cursor >= p - text && cursor <= rp - text)
        {
            int plen = (int)(rp - lp);
            if (plen >= link_cap)
                plen = link_cap - 1;
            memcpy(link, lp, plen);
            link[plen] = 0;
            return 1;
        }

        p = rp + 1;  /* keep scanning past this link */
    }

    return 0;
}

static int is_path_char(int c)
{
    return isalnum((unsigned char)c) || c == '_' || c == '-' || c == '.' ||
        c == '/' || c == '\\';
}

/* Extracts a bare filename/relative-path token (letters/digits/'_'/'-'/'.'/
 * '/'/'\\') touching `cursor` in `text` - same idea as word_at_cursor()
 * above, just with a wider character set so "src/foo.h" or "../bar.c" comes
 * out whole rather than just "h" or "bar" (a wrapping '"'/'<'/'>', e.g. from
 * a #include line, isn't in that set either, so it naturally bounds the
 * token without special-casing). Requires the result end in ".c", ".h", or
 * ".md" (case-insensitive) so an ordinary identifier or a "1.5" number
 * doesn't get treated as a file reference. Returns 1 and fills `out` if so,
 * 0 (leaving `out` empty) otherwise - mirrors word_at_cursor()'s contract. */
static int filename_at_cursor(const char* text, int len, int cursor, char* out, int out_cap)
{
    if (cursor < 0) cursor = 0;
    if (cursor > len) cursor = len;

    int lo = cursor, hi = cursor;
    if (lo > 0 && !(lo < len && is_path_char((unsigned char)text[lo])) &&
        is_path_char((unsigned char)text[lo - 1]))
    {
        lo--;
        hi--;
    }
    while (lo > 0 && is_path_char((unsigned char)text[lo - 1]))
        lo--;
    while (hi < len && is_path_char((unsigned char)text[hi]))
        hi++;

    if (out_cap > 0)
        out[0] = 0;
    if (lo == hi)
        return 0;

    int wlen = hi - lo;
    if (wlen >= out_cap)
        wlen = out_cap - 1;
    if (wlen <= 0)
        return 0;
    memcpy(out, text + lo, wlen);
    out[wlen] = 0;

    const char* dot = strrchr(out, '.');
    if (!dot || dot == out)
        return 0;
    if (ci_strcmp(dot, ".c") != 0 && ci_strcmp(dot, ".h") != 0 && ci_strcmp(dot, ".md") != 0)
        return 0;

    return 1;
}

/* Defined later, next to output_goto_source() (which shares the same
 * "resolve a bare filename against the directories it plausibly came from"
 * need) - forward declared so do_editor_ctrlclick() below can open a
 * ".c"/".h"/".md" reference under the caret the same way. */
static void open_referenced_file(const char* filename);

/* Ctrl+click inside a document <editor> - or a plain click if it's read-only
 * (EVT_EDITOR_CTRLCLICK; the click itself already positioned the caret, see
 * ui.c's EDITOR mouse handling). Two independent things it can land on, tried
 * in this order:
 *   1. Markdown mode only: a "[text](path)" link on the current line - opens
 *      `path` into this same window (see open_link_in_window() above), like
 *      an IDE's "go to file", not a new tab.
 *   2. Any syntax: a bare ".c"/".h"/".md" filename/path under the caret (a
 *      #include target, a comment mentioning another file, a Markdown doc
 *      naming one outside link syntax, ...) - opens it into its own window
 *      via open_referenced_file() below, same as File > Open would.
 * A no-op if neither matched. */
static void do_editor_ctrlclick(void)
{
    ui_node* win = ui_screen_top_window(g_screen);
    ui_node* ed = editor_in_window(win);
    if (!ed)
        return;

    const char* text = ui_get_value(ed);
    int cursor = ui_editor_get_cursor(ed);
    int len = (int)strlen(text);

    if (ui_get_syntax(ed) == UI_SYNTAX_MARKDOWN)
    {
        char link[512];
        if (markdown_link_at_cursor(text, len, cursor, link, (int)sizeof link))
        {
            nav_record_jump();
            open_link_in_window(win, link);
            return;
        }
    }

    char filename[512];
    if (filename_at_cursor(text, len, cursor, filename, (int)sizeof filename))
        open_referenced_file(filename);
}

#define CAPTURE_BUF_SIZE 16384
#define CAPTURE_TMP_FILE ".compile_output.tmp"

/* --- External Tools (Visual Studio-style) --------------------------------
 *
 * A short list of user-configured programs, each appearing as its own item
 * in the Tools menu. Running one spawns it as a real child process (see
 * ui_process_start) and streams its stdout+stderr into the Output window
 * through exactly the same per-frame drain the internal compile uses - the
 * only difference is where the bytes come from, a pipe-from-a-child rather
 * than a pipe-from-a-worker-thread.
 *
 * Arguments support the usual Visual Studio macros, expanded against the
 * active document (see exttool_expand):
 *     $(FilePath)  full path            $(FileDir)   containing directory
 *     $(FileName)  name without ext     $(FileExt)   extension, with the dot
 * Plus three cake-specific macros:
 *     $(CakeOutput) - see exttool_append_cake_output()'s own doc comment:
 *                      cake's own predicted generated-output file(s), quoted
 *                      and space-separated - e.g. an External Tool "gcc"
 *                      with arguments "$(CakeOutput) -o myapp" builds
 *                      whatever cake's own Build just produced. With a
 *                      project open, one entry per .c file in it; without
 *                      one, just the single active document (empty if
 *                      there's no active .c document).
 *     $(Target)      - that target platform's name on its own, e.g. for a
 *                      cross-compiler flag - the open project's own
 *                      (project_target_platform_name()) if there is one,
 *                      else this build's own compile-time default, same
 *                      fallback $(CakeOutput) uses without a project.
 *     $(ProjectDir) - the open project's own directory, for a tool that
 *                      should follow the project rather than whichever
 *                      document has focus; the active document's own
 *                      directory when no project is open.
 * which Visual Studio itself spells $(ItemPath)/$(ItemDir)/$(ItemFilename)/
 * $(ItemExt) - both spellings work. The binary being built has its own
 * Visual Studio family too, all rooted at the open project or, with none,
 * the active document's own directory:
 *     $(TargetDir)      the folder the compiler writes it to, i.e.
 *                        "<root>/<platform>" - see target_dir_path()
 *     $(TargetFileName) its file name, e.g. main.exe - Compiler Options'
 *                        own "Output" field when set, else the project's
 *                        name (the document's own with no project open),
 *                        plus ".exe" on the msvc targets - see
 *                        target_file_name()
 *     $(TargetName)     that with the extension stripped; $(TargetExt)
 *                        just the extension itself
 *     $(TargetPath)     $(TargetDir)/$(TargetFileName) - exactly the path
 *                        Debug (F5) launches (do_debug_start() goes through
 *                        the same two helpers), so a tool that builds to it
 *                        and the debugger cannot point at different files
 *     $(ProjectName)    the open project's name; $(Platform) the target
 *                        platform slug, e.g. x64_msvc (the older $(Target)
 *                        spelling of it still works)
 * A literal "$$" produces a single "$".
 */
#define EXT_TOOL_MAX 12

typedef struct
{
    char title[64];    /* shown in the Tools menu */
    char command[512]; /* program to run */
    char args[512];    /* argument string, macros expanded before use */
    char dir[512];     /* working directory, macros expanded too ("" = inherit) */
} ext_tool;

static struct
{
    ext_tool items[EXT_TOOL_MAX];
    int count;
} g_tools;

/* Tools > External Tools... - the config dialog. `edit` is a working copy
 * the dialog mutates; it is only committed over g_tools on OK, so Cancel
 * discards everything (same pattern as the Compiler Options dialog
 * re-syncing its controls on open). `sel` is the row being edited. */
static struct
{
    ui_node* modal;
    ui_node* listbox;
    ui_node* title_input;
    ui_node* cmd_input;
    ui_node* args_input;
    ui_node* dir_input;

    /* The macro popup (see ext_macros) and the two fields its ">" buttons
     * open it for, indexed the same way EVT_EXTTOOL_MACRO_BTN is. `macro_target`
     * is whichever of them the open popup belongs to - only read while it is
     * up, so a picked item knows where to insert. */
    ui_node* macro_popup;
    ui_node* macro_fields[2];
    ui_node* macro_target;

    ext_tool edit[EXT_TOOL_MAX];
    int count;
    int sel;
} g_exttool;

static struct
{
    ui_node* modal;
    ui_node* folder_input;
    ui_node* name_input;
    ui_node* helloworld_check;
} g_newproject;

/* --- External Tools dialog helpers ---
 * The dialog edits g_exttool.edit (a working copy) and only writes it back
 * over g_tools on OK, so Cancel is a true discard. */

static void exttool_refresh_list(void)
{
    ui_node* lb = g_exttool.listbox;
    if (!lb)
        return;
    while (ui_child_count(lb) > 0)
    {
        ui_node* c = ui_child_at(lb, 0);
        ui_remove_child(lb, c);
        ui_node_free(c);
    }
    for (int i = 0; i < g_exttool.count; i++)
    {
        ui_node* it = ui_create_element(UI_TAG_ITEM);
        ui_set_label(it, g_exttool.edit[i].title[0] ? g_exttool.edit[i].title
                                                     : "(untitled)");
        ui_append_child(lb, it);
    }
    if (g_exttool.sel >= g_exttool.count)
        g_exttool.sel = g_exttool.count - 1;
    if (g_exttool.sel < 0 && g_exttool.count > 0)
        g_exttool.sel = 0;
    ui_select_set_selected(lb, g_exttool.sel);
}

/* Copies the selected row into the four field <input>s (all blank when
 * there is no selection). */
static void exttool_load_fields(void)
{
    const ext_tool* t = (g_exttool.sel >= 0 && g_exttool.sel < g_exttool.count)
                        ? &g_exttool.edit[g_exttool.sel] : NULL;
    ui_set_value(g_exttool.title_input, t ? t->title : "");
    ui_set_value(g_exttool.cmd_input, t ? t->command : "");
    ui_set_value(g_exttool.args_input, t ? t->args : "");
    ui_set_value(g_exttool.dir_input, t ? t->dir : "");
}

/* The reverse: fields back into the selected row. Called on every change
 * that could move the selection away, so edits aren't silently lost. */
static void exttool_store_fields(void)
{
    if (g_exttool.sel < 0 || g_exttool.sel >= g_exttool.count)
        return;
    ext_tool* t = &g_exttool.edit[g_exttool.sel];
    snprintf(t->title, sizeof t->title, "%s", ui_get_value(g_exttool.title_input));
    snprintf(t->command, sizeof t->command, "%s", ui_get_value(g_exttool.cmd_input));
    snprintf(t->args, sizeof t->args, "%s", ui_get_value(g_exttool.args_input));
    snprintf(t->dir, sizeof t->dir, "%s", ui_get_value(g_exttool.dir_input));
}

/* The macros offered by the dialog's ">" popup, in menu order - the same
 * names exttool_expand() understands (its Visual Studio $(Item*) aliases are
 * deliberately left out: they expand identically to the $(File*) ones above
 * them, so listing both would just be two ways to pick the same thing). The
 * label is what the menu shows AND what gets inserted, which is why it is
 * spelled out in full rather than assembled from the bare name. */
static const struct
{
    const char* name;
    const char* short_help;  /* status bar hint of its popup item */
    const char* help;        /* F1 text of its popup item */
} ext_macros[] = {
    { "$(FilePath)",
      "`$(FilePath)`: The active document's full path", "# `$(FilePath)`\n\nThe active document's full path\n"
      "\n"
      "Also spelled `$(ItemPath)`.\n"
      "\n"
      "Example: active document `C:/work/hello/src/main.c`, project `hello` in `C:/work/hello`, target `x64_msvc`. Expands to `C:/work/hello/src/main.c`." },
    { "$(FileDir)",
      "`$(FileDir)`: The active document's folder, without a trailing slash", "# `$(FileDir)`\n\nThe active document's folder, without a trailing slash\n"
      "\n"
      "Also spelled `$(ItemDir)`.\n"
      "\n"
      "Example: active document `C:/work/hello/src/main.c`, project `hello` in `C:/work/hello`, target `x64_msvc`. Expands to `C:/work/hello/src`." },
    { "$(FileName)",
      "`$(FileName)`: The active document's file name, without its extension", "# `$(FileName)`\n\nThe active document's file name, without its extension\n"
      "\n"
      "Also spelled `$(ItemFilename)`.\n"
      "\n"
      "Example: active document `C:/work/hello/src/main.c`, project `hello` in `C:/work/hello`, target `x64_msvc`. Expands to `main`." },
    { "$(FileExt)",
      "`$(FileExt)`: The active document's extension, including the dot", "# `$(FileExt)`\n\nThe active document's extension, including the dot\n"
      "\n"
      "Also spelled `$(ItemExt)`.\n"
      "\n"
      "Example: active document `C:/work/hello/src/main.c`, project `hello` in `C:/work/hello`, target `x64_msvc`. Expands to `.c`." },
    { "$(CakeOutput)",
      "`$(CakeOutput)`: Cake's output file(s) - the C89 code Build generates", "# `$(CakeOutput)`\n\nCake's output file(s) - the C89 code Build generates\n"
      "\n"
      "With a project open: one path per `.c` file of the project, separated by spaces. Without one: just the active file's output. Each lands in a folder named after the target (see Build > Show Generated Code). Pass it to the compiler that links them." },
    { "$(TargetPath)",
      "`$(TargetPath)`: The full path of the binary - exactly what Debug (F5) launches", "# `$(TargetPath)`\n\nThe full path of the binary - exactly what Debug (F5) launches\n"
      "\n"
      "`$(TargetDir)/$(TargetFileName)`. Use it as the linker's output, so the external compile and Debug agree on one file.\n"
      "\n"
      "Example: active document `C:/work/hello/src/main.c`, project `hello` in `C:/work/hello`, target `x64_msvc`. Expands to `C:/work/hello/x64_msvc/hello.exe`." },
    { "$(TargetDir)",
      "`$(TargetDir)`: The folder the binary goes to: `<project dir>/<platform>`", "# `$(TargetDir)`\n\nThe folder the binary goes to: `<project dir>/<platform>`\n"
      "\n"
      "Without a project open, the active document's folder is used instead.\n"
      "\n"
      "Example: active document `C:/work/hello/src/main.c`, project `hello` in `C:/work/hello`, target `x64_msvc`. Expands to `C:/work/hello/x64_msvc`." },
    { "$(TargetFileName)",
      "`$(TargetFileName)`: The binary's file name, with its extension", "# `$(TargetFileName)`\n\nThe binary's file name, with its extension\n"
      "\n"
      "Compiler Options' **Output** field when set; otherwise the project's name (the document's name without a project), plus `.exe` on the MSVC targets.\n"
      "\n"
      "Example: active document `C:/work/hello/src/main.c`, project `hello` in `C:/work/hello`, target `x64_msvc`. Expands to `hello.exe`." },
    { "$(TargetName)",
      "`$(TargetName)`: The binary's file name without its extension", "# `$(TargetName)`\n\nThe binary's file name without its extension\n"
      "\n"
      "`$(TargetFileName)` up to its last dot.\n"
      "\n"
      "Example: active document `C:/work/hello/src/main.c`, project `hello` in `C:/work/hello`, target `x64_msvc`. Expands to `hello`." },
    { "$(TargetExt)",
      "`$(TargetExt)`: The binary's extension, including the dot", "# `$(TargetExt)`\n\nThe binary's extension, including the dot\n"
      "\n"
      "`.exe` on the MSVC targets; empty on the GCC/Clang ones, which have none.\n"
      "\n"
      "Example: active document `C:/work/hello/src/main.c`, project `hello` in `C:/work/hello`, target `x64_msvc`. Expands to `.exe`." },
    { "$(ProjectDir)",
      "`$(ProjectDir)`: The open project's folder", "# `$(ProjectDir)`\n\nThe open project's folder\n"
      "\n"
      "Without a project open, the active document's folder - so the same tool also works on a single file.\n"
      "\n"
      "Example: active document `C:/work/hello/src/main.c`, project `hello` in `C:/work/hello`, target `x64_msvc`. Expands to `C:/work/hello`." },
    { "$(ProjectName)",
      "`$(ProjectName)`: The open project's name", "# `$(ProjectName)`\n\nThe open project's name\n"
      "\n"
      "Without a project open, the active document's name without its extension.\n"
      "\n"
      "Example: active document `C:/work/hello/src/main.c`, project `hello` in `C:/work/hello`, target `x64_msvc`. Expands to `hello`." },
    { "$(Platform)",
      "`$(Platform)`: The compilation target's name, e.g. `x64_msvc`", "# `$(Platform)`\n\nThe compilation target's name, e.g. `x64_msvc`\n"
      "\n"
      "The target selected in Compiler Options, with `default` resolved to the real platform. Also spelled `$(Target)`." },
};
#define EXT_MACRO_COUNT ((int)(sizeof ext_macros / sizeof ext_macros[0]))

/* Insert `text` into `field` at its caret, replacing whatever is selected,
 * and leave the caret just past what was inserted - what picking a macro
 * from the ">" popup does. The caret/selection an <input> holds survives it
 * losing focus to the button, so this lands where the user last was in the
 * field rather than always at the end (ui_set_value() alone would do the
 * latter, hence the set_selection afterwards). */
static void exttool_insert_macro(ui_node* field, const char* text)
{
    if (!field || !text)
        return;
    const char* cur = ui_get_value(field);
    int len = (int)strlen(cur);
    int lo = ui_editor_get_cursor(field), hi = lo;
    ui_editor_get_selection(field, &lo, &hi);   /* leaves lo/hi alone if none */
    if (lo < 0 || lo > len) lo = len;
    if (hi < lo || hi > len) hi = lo;

    char buf[1024];
    int n = snprintf(buf, sizeof buf, "%.*s%s%s", lo, cur, text, cur + hi);
    if (n < 0 || n >= (int)sizeof buf)
        return;                                  /* would truncate - leave as-is */
    ui_set_value(field, buf);
    int caret = lo + (int)strlen(text);
    ui_editor_set_selection(field, caret, caret);
}

/* The Command "..." browse landing back here with a picked program: drop it
 * into the Command field and commit the row, so the change survives the very
 * next selection change like any typed edit would. Backslashes, since this
 * is a command line the user will read and (on Windows) a path the shell
 * takes either way. */
static void exttool_browse_pick(const char* path)
{
    char buf[1024];
    snprintf(buf, sizeof buf, "%s", path);
    for (char* p = buf; *p; p++)
        if (*p == '/')
            *p = '\\';
    ui_set_value(g_exttool.cmd_input, buf);
    exttool_store_fields();
}

/* Rebuilds everything in the Tools menu below its one fixed item
 * ("Terminal", built in build_screen()): the configured tools, then
 * "External Tools..." pinned LAST so the configuration entry stays at the
 * bottom no matter how many tools are defined. No separator before it -
 * it reads as part of the same list. Dropped and refilled each
 * time the list changes - same remove-then-refill idiom the Open dialog's
 * listbox uses (see open_dialog_refresh). */
#define TOOLS_MENU_FIXED_ITEMS 1

static void rebuild_tools_menu(void)
{
    if (!g_tools_menu)
        return;

    while (ui_child_count(g_tools_menu) > TOOLS_MENU_FIXED_ITEMS)
    {
        ui_node* c = ui_child_at(g_tools_menu, ui_child_count(g_tools_menu) - 1);
        ui_remove_child(g_tools_menu, c);
        ui_node_free(c);
    }

    if (g_tools.count > 0)
    {
        ui_node* sep = ui_create_element(UI_TAG_ITEM);
        ui_set_separator(sep, 1);
        ui_append_child(g_tools_menu, sep);

        for (int i = 0; i < g_tools.count; i++)
        {
            ui_node* it = ui_create_element(UI_TAG_ITEM);
            ui_set_id(it, EVT_TOOL_RUN_BASE + i);
            ui_set_label(it, g_tools.items[i].title[0] ? g_tools.items[i].title
                                                        : g_tools.items[i].command);
            ui_append_child(g_tools_menu, it);
        }
    }

    ui_node* cfg = ui_create_element(UI_TAG_ITEM);
    ui_set_id(cfg, EVT_TOOLS_EXTERNAL);
    ui_set_label(cfg, "External Tools...");
    ui_append_child(g_tools_menu, cfg);
}

/* A growable text buffer for building an External Tool's command line.
 *
 * $(CakeOutput) expands to one quoted path PER .c FILE in the project, so
 * the result has no useful upper bound - a fixed buffer here silently cut
 * the command in half (mid-path, so the compiler saw a garbage file name)
 * on any project past a couple of dozen files. `data` is NULL until the
 * first append; `oom` latches on allocation failure, after which every
 * append is a no-op and callers report the failure once, rather than each
 * append having to be checked. */
struct exttool_buf
{
    char* data;
    size_t len;
    size_t cap;
    bool oom;
};

static void exttool_buf_free(struct exttool_buf* b)
{
    free(b->data);
    b->data = NULL;
    b->len = b->cap = 0;
    b->oom = false;
}

/* The buffer's text, never NULL - an untouched (or failed) buffer reads as
 * the empty string, so callers can use it without a null check. */
static const char* exttool_buf_text(const struct exttool_buf* b)
{
    return b->data ? b->data : "";
}

/* Appends `n` bytes of `text` to `b`, growing it as needed. */
static void exttool_append_n(struct exttool_buf* b, const char* text, size_t n)
{
    if (b->oom || n == 0)
        return;

    if (b->len + n + 1 > b->cap)
    {
        size_t newcap = b->cap ? b->cap * 2 : 256;
        while (newcap < b->len + n + 1)
            newcap *= 2;
        char* p = realloc(b->data, newcap);
        if (!p)
        {
            b->oom = true;
            return;
        }
        b->data = p;
        b->cap = newcap;
    }

    memcpy(b->data + b->len, text, n);
    b->len += n;
    b->data[b->len] = 0;
}

/* Appends `text` to `b`, growing it as needed. */
static void exttool_append(struct exttool_buf* b, const char* text)
{
    exttool_append_n(b, text, strlen(text));
}

/* --- Git integration: "Git Changes" docked panel + diff viewer window ---
 * Runs `cmd` (dir defaults to the IDE's own cwd when NULL/empty, same as
 * ui_process_start's own dir param) through to completion and appends its
 * combined stdout+stderr into `out`, replacing whatever `out` held before.
 * Same "run a process, capture its output" building block do_build() uses
 * (ui_process_start), just driven to completion synchronously here instead
 * of polled from app_frame() - git status/diff on one repo/file return in
 * well under a frame, so there is nothing to stream. Returns the exit code,
 * or -1 if the process could not start. */
static int run_process_capture(const char* cmd, const char* dir, struct exttool_buf* out)
{
    exttool_buf_free(out);
    ui_process* proc = ui_process_start(cmd, dir && dir[0] ? dir : NULL, NULL, 0);
    if (!proc)
        return -1;
    char buf[4096];
    for (;;)
    {
        int n = ui_process_read(proc, buf, sizeof buf);
        if (n > 0)
        {
            exttool_append_n(out, buf, (size_t)n);
            continue;
        }
        if (n == 0)
            continue;  /* still running, nothing to read yet */
        break;  /* -1: EOF, the child is done */
    }
    return ui_process_close(proc);
}

/* The Git Changes panel's own per-row status marker - two columns (letter +
 * trailing space) colored from the current theme's diagnostic colors,
 * reusing them rather than adding new theme fields just for this panel (see
 * ui_theme's diag_error_fg/diag_warning_fg/diag_info_fg). `xy` is the two
 * status characters `git status --porcelain` prints before each path. */
static void git_status_marker(char x, char y, char* out_marker, uint32_t* out_fg)
{
    const ui_theme* theme = ui_get_theme();
    if (x == '?' && y == '?')
    {
        strcpy(out_marker, "? ");
        *out_fg = theme->diag_info_fg;
    }
    else if (x == 'A' || y == 'A')
    {
        strcpy(out_marker, "A ");
        *out_fg = theme->editor_number_fg;
    }
    else if (x == 'D' || y == 'D')
    {
        strcpy(out_marker, "D ");
        *out_fg = theme->diag_error_fg;
    }
    else if (x == 'M' || y == 'M')
    {
        strcpy(out_marker, "M ");
        *out_fg = theme->diag_warning_fg;
    }
    else
    {
        strcpy(out_marker, "  ");
        *out_fg = 0;
    }
}

/* Rebuilds the Git Changes panel's listbox from `git status --porcelain`,
 * run against g_folder.dir (the same directory the Folder panel browses -
 * this panel has no directory field of its own, see g_git's own doc
 * comment). Same clear-then-refill shape as project_window_refresh(). */
static void git_panel_refresh(void)
{
    if (!g_git.listbox)
        return;

    struct exttool_buf root_out = { 0 };
    run_process_capture("git rev-parse --show-toplevel", g_folder.dir, &root_out);
    size_t rootlen = root_out.len;
    while (rootlen > 0 && (root_out.data[rootlen - 1] == '\n' || root_out.data[rootlen - 1] == '\r'))
        rootlen--;
    if (rootlen >= sizeof g_git.root)
        rootlen = sizeof g_git.root - 1;
    memcpy(g_git.root, exttool_buf_text(&root_out), rootlen);
    g_git.root[rootlen] = 0;
    exttool_buf_free(&root_out);

    struct exttool_buf out = { 0 };
    run_process_capture("git status --porcelain", g_folder.dir, &out);

    while (ui_child_count(g_git.listbox) > 0)
    {
        ui_node* c = ui_child_at(g_git.listbox, 0);
        ui_remove_child(g_git.listbox, c);
        ui_node_free(c);
    }

    const char* p = exttool_buf_text(&out);
    while (*p)
    {
        const char* eol = strchr(p, '\n');
        size_t linelen = eol ? (size_t)(eol - p) : strlen(p);
        if (linelen >= 4)
        {
            char marker[8];
            uint32_t marker_fg;
            git_status_marker(p[0], p[1], marker, &marker_fg);

            char filename[1024];
            size_t namelen = linelen - 3;
            if (namelen >= sizeof filename)
                namelen = sizeof filename - 1;
            memcpy(filename, p + 3, namelen);
            filename[namelen] = 0;

            /* Renames show as "old -> new" - only the new path is a real,
             * openable/diffable file. */
            char* arrow = strstr(filename, " -> ");
            const char* real_path = arrow ? arrow + 4 : filename;

            ui_node* item = ui_create_element(UI_TAG_ITEM);
            char label[1200];
            /* index column set = staged; both columns set = only part of the changes is staged */
            const char* staged = "";
            if (p[0] != ' ' && p[0] != '?')
                staged = p[1] != ' ' ? "  [partly staged]" : "  [staged]";
            snprintf(label, sizeof label, "%s%s%s", marker, filename, staged);
            ui_set_label(item, label);
            ui_set_color(item, marker_fg, 0);
            ui_set_path(item, real_path);
            ui_append_child(g_git.listbox, item);
        }
        p = eol ? eol + 1 : p + linelen;
    }
    ui_select_set_selected(g_git.listbox, 0);
    exttool_buf_free(&out);

    ui_node* window = ui_child_at(g_git.window, 0);
    if (window)
    {
        char title[64];
        snprintf(title, sizeof title, " Git Changes (%d) ", ui_child_count(g_git.listbox));
        ui_set_label(window, title);
    }
}

/* Raises the Git Changes panel - shared by View > "Git Changes"
 * (EVT_WINDOW_GIT). Same LEFT-dock mutual exclusion as folder_show_panel()/
 * project_show_panel(): dock_layout() only lays out one window per side, so
 * this closes Folder/Project first if either is still sitting on LEFT (a
 * no-op once any of the three has been redocked elsewhere). */
static void git_show_panel(void)
{
    int w = close_other_left_panel(g_folder.window, g_git.window);
    if (!w)
        w = close_other_left_panel(g_project.window, g_git.window);
    else
        close_other_left_panel(g_project.window, g_git.window);
    if (w)
        ui_set_dock(ui_child_at(g_git.window, 0), UI_DOCK_LEFT, w);

    git_panel_refresh();
    ui_screen_show_window(g_screen, g_git.window);
}

/* Activates row `index` of the Git Changes panel's listbox: shows that
 * file's diff in the singleton g_gitdiff_window (see its own doc comment).
 * Tries the working-tree diff first, then the staged diff, then - for an
 * untracked file, where git has nothing to diff against - just the file's
 * own content, so a click always shows something. */
static void git_window_activate(int index)
{
    if (index < 0 || index >= ui_child_count(g_git.listbox))
        return;
    const char* path = ui_get_path(ui_child_at(g_git.listbox, index));
    if (!path || !path[0])
        return;

    const char* root = g_git.root[0] ? g_git.root : g_folder.dir;

    /* No --color=always here - g_gitdiff_editor is UI_SYNTAX_DIFF, which
     * colors this plain, uncolored diff text itself (real C token colors
     * plus a per-row +/- background wash, see ui_syntax's own doc comment),
     * the same way UI_SYNTAX_C colors a plain .c file's own plain text.
     *
     * -U100000 - an unrealistically large context count so every hunk merges
     * into one covering the whole file: this diff view is meant to be read
     * like the file itself (see git_window_activate's own maximize-on-open),
     * not squinted at through git's usual handful of context lines around
     * each change. */
    char cmd[1200];
    struct exttool_buf out = { 0 };
    snprintf(cmd, sizeof cmd, "git diff -U100000 -- \"%s\"", path);
    run_process_capture(cmd, root, &out);

    if (out.len == 0)
    {
        snprintf(cmd, sizeof cmd, "git diff -U100000 --cached -- \"%s\"", path);
        run_process_capture(cmd, root, &out);
    }
    if (out.len == 0)
    {
        char full[1400];
        path_join(full, sizeof full, root, path);
        char* content = read_file_to_string(full);
        if (content)
        {
            exttool_append(&out, content);
            free(content);
        }
        else
        {
            exttool_append(&out, "(no changes to display)\n");
        }
    }

    /* git diff/read_file_to_string both end their output in a trailing '\n'
     * - the editor treats whatever comes after the last '\n' as one more
     * (empty) line, same as any text file with a final newline, so left as
     * is that's a blank row at the bottom of every diff. Trimmed here rather
     * than changed editor-wide since a real source file's own trailing
     * newline is normal and expected while editing it. */
    while (out.len > 0 && out.data[out.len - 1] == '\n')
        out.data[--out.len] = 0;

    /* Drop `git diff`'s own leading "diff --git .../index .../--- a/...
     * /+++ b/..." header lines, and the "@@ -a,b +c,d @@" hunk header after
     * them - the path is already in this window's own title, and with
     * -U100000 there's only ever one hunk covering the whole file, so its
     * own line-number range is meaningless noise too. Only for a real diff
     * (the file's plain content/"(no changes to display)" fallbacks above
     * never start with "diff --git", so this is a no-op for those). */
    {
        /* stderr is captured too, so git warnings (e.g. "LF will be replaced
         * by CRLF") can precede the header - they are dropped with it. */
        const char* text = exttool_buf_text(&out);
        g_gitdiff_prefixed = 0;
        if (strncmp(text, "diff --git ", 11) == 0 || strstr(text, "\ndiff --git "))
        {
            g_gitdiff_prefixed = 1;
            const char* at = strstr(text, "\n@@");
            const char* eol = at ? strchr(at + 1, '\n') : NULL;
            if (eol)
            {
                size_t skip = (size_t)(eol + 1 - text);
                memmove(out.data, out.data + skip, out.len - skip + 1);
                out.len -= skip;
            }
        }
    }

    path_join(g_gitdiff_path, sizeof g_gitdiff_path, root, path);

    char title[300];
    snprintf(title, sizeof title, " Diff: %s ", basename_of(path));
    ui_node* window = ui_child_at(g_gitdiff_window, 0);
    ui_set_label(window, title);
    ui_set_value(g_gitdiff_editor, exttool_buf_text(&out));
    exttool_buf_free(&out);
    git_diff_index_changes();

    ui_screen_show_window(g_screen, g_gitdiff_window);
    ui_window_maximize(g_screen, window);  /* always opens filling the desktop
                                            * space left by the docked panels,
                                            * same as any new document window
                                            * (make_editor_window) - a diff is
                                            * opened to be read, not squeezed
                                            * into whatever small rect it was
                                            * last left at. */
}

/* The diff viewer's "Edit": opens the file with the caret on the file line and column the diff's caret is on.
 * The diff is one -U100000 hunk covering the whole file, so the file line is just the count of ' ' and '+'
 * rows up to the caret; a '-' row (gone from the file) lands where it was removed. */
static void git_diff_edit(void)
{
    if (!g_gitdiff_path[0])
        return;

    const char* text = ui_get_value(g_gitdiff_editor);
    int caret_line = ui_editor_caret_line(g_gitdiff_editor);
    int cursor = ui_editor_get_cursor(g_gitdiff_editor);

    int file_line = caret_line, col = 1;
    const char* row = text;
    for (int line = 1; line < caret_line && *row; row++)
        if (*row == '\n')
            line++;
    col = (int)(text + cursor - row) + 1;

    if (g_gitdiff_prefixed)
    {
        file_line = 1;
        const char* p = text;
        for (int line = 1; line < caret_line && *p; line++)
        {
            if (*p != '-')
                file_line++;
            while (*p && *p != '\n')
                p++;
            if (*p)
                p++;
        }
        col = *row == '-' ? 1 : (col > 1 ? col - 1 : 1);
    }

    /* The caret's row on screen, so the file opens scrolled with that line at the same height. */
    int screen_row = (caret_line - 1) - ui_editor_get_scroll(g_gitdiff_editor);

    nav_record_jump();
    open_file_path_into_editor(g_gitdiff_path, basename_of(g_gitdiff_path));
    ui_node* window = find_open_window(g_gitdiff_path);
    ui_node* editor = editor_in_window(window);
    if (!editor)
        return;
    ui_screen_show_window(g_screen, window);
    ui_editor_goto_line_col(editor, file_line, col);
    int scroll = (file_line - 1) - screen_row;
    ui_editor_set_scroll(editor, scroll > 0 ? scroll : 0);
    ui_screen_focus(g_screen, editor);
}

/* A diff row that's part of a change - the same leading '+'/'-' rule
 * render_editor() uses for the added/removed background wash (see its own
 * line_bg computation), duplicated here rather than shared since one works
 * on a byte range mid-buffer and the other on `n->label` directly. */
static int git_diff_line_is_changed(const char* s, int len)
{
    if (len <= 0)
        return 0;
    if (s[0] == '+' && !(len >= 3 && s[1] == '+' && s[2] == '+'))
        return 1;
    if (s[0] == '-' && !(len >= 3 && s[1] == '-' && s[2] == '-'))
        return 1;
    return 0;
}

/* "< Prev Change"/"Next Change >": jumps the diff viewer's caret to the
 * start of the next (dir > 0) or previous (dir < 0) contiguous run of
 * changed lines, wrapping around at either end - see the buttons' own doc
 * comment above for why this exists (a whole-file diff has nothing else to
 * scroll by). A "run" is however many consecutive +/- lines a change spans,
 * not a single line, so this doesn't stop partway through one hunk. */
/* Records where each run of changed rows starts (g_gitdiff_runs), once per loaded diff - both the
 * Previous/Next buttons and the "2/23" counter read it instead of rescanning the text. */
static void git_diff_index_changes(void)
{
    free(g_gitdiff_runs);
    g_gitdiff_runs = NULL;
    g_gitdiff_run_count = 0;
    g_gitdiff_counter_line = -1;

    const char* text = ui_get_value(g_gitdiff_editor);
    if (!text || !text[0] || !g_gitdiff_prefixed)
        return;

    int total_lines = 1;
    for (const char* p = text; *p; p++)
        if (*p == '\n')
            total_lines++;
    g_gitdiff_runs = malloc(sizeof(int) * (size_t)total_lines);
    if (!g_gitdiff_runs)
        return;

    int prev_changed = 0, line_no = 1;
    const char* p = text;
    for (;;)
    {
        const char* nl = strchr(p, '\n');
        int len = nl ? (int)(nl - p) : (int)strlen(p);
        int changed = git_diff_line_is_changed(p, len);
        if (changed && !prev_changed)
            g_gitdiff_runs[g_gitdiff_run_count++] = line_no;
        prev_changed = changed;
        if (!nl)
            break;
        p = nl + 1;
        line_no++;
    }
}

/* Per frame: "n/total" for the change the caret is in (or last passed), "-/total" above the first one. Recomputed only when the caret line moves. */
static void git_diff_counter_refresh(void)
{
    if (!g_gitdiff_counter)
        return;
    int cur = ui_editor_caret_line(g_gitdiff_editor);
    if (cur == g_gitdiff_counter_line)
        return;
    g_gitdiff_counter_line = cur;

    int index = 0;
    while (index < g_gitdiff_run_count && g_gitdiff_runs[index] <= cur)
        index++;

    char label[32];
    if (g_gitdiff_run_count == 0)
        snprintf(label, sizeof label, " no changes");
    else if (index == 0)
        snprintf(label, sizeof label, "  -/%d", g_gitdiff_run_count);
    else
        snprintf(label, sizeof label, "%3d/%d", index, g_gitdiff_run_count);
    ui_set_label(g_gitdiff_counter, label);
}

static void git_diff_goto_change(int dir)
{
    int cur = ui_editor_caret_line(g_gitdiff_editor);
    const int* run_starts = g_gitdiff_runs;
    int run_count = g_gitdiff_run_count;

    int target = -1;
    if (dir > 0)
    {
        for (int i = 0; i < run_count; i++)
            if (run_starts[i] > cur) { target = run_starts[i]; break; }
        if (target < 0 && run_count > 0)
            target = run_starts[0];  /* wrap to the first change */
    }
    else
    {
        for (int i = run_count - 1; i >= 0; i--)
            if (run_starts[i] < cur) { target = run_starts[i]; break; }
        if (target < 0 && run_count > 0)
            target = run_starts[run_count - 1];  /* wrap to the last change */
    }

    if (target > 0)
        ui_editor_goto_line(g_gitdiff_editor, target);
}

/* Escapes `in` for safe use inside a double-quoted shell argument (only "
 * and \ need escaping there) into `out` (truncated to fit outcap) - the
 * commit message is the one piece of free-form text a git command here
 * embeds. */
static void shell_escape_dq(const char* in, char* out, size_t outcap)
{
    size_t o = 0;
    for (const char* p = in; *p && o + 2 < outcap; p++)
    {
        if (*p == '"' || *p == '\\')
            out[o++] = '\\';
        out[o++] = *p;
    }
    out[o] = 0;
}

/* --- Async git job ---
 * git commands that touch the network or take a while (commit, pull, push,
 * clone, branch, stage) run here instead of through run_process_capture(),
 * so the UI keeps drawing. Same "start the process, drain it once per frame
 * from app_frame()" shape as the compile/External Tool job (g_job), but kept
 * separate so a git command never fights a build over the Output window.
 * A job is a short list of commands run in order; the first failing one
 * stops the rest (e.g. no push after a failed commit). */
enum git_job_after
{
    GIT_AFTER_SHOW,         /* always show the output in a message box */
    GIT_AFTER_SHOW_ERROR,   /* show it only when a step failed (Stage/Unstage) */
    GIT_AFTER_CLONE,        /* show it, then open the cloned folder on success */
};

#define GIT_JOB_MAX_STEPS 4

static void compile_status_set(const char* text);  /* defined below */

static struct
{
    ui_process* proc;
    struct exttool_buf out;
    char steps[GIT_JOB_MAX_STEPS][2400];
    int nsteps;
    int step;
    int failed;
    char dir[1024];
    char title[32];
    enum git_job_after after;
    char clone_dest[1024];  /* GIT_AFTER_CLONE: folder to open */
    int clone_open;         /* GIT_AFTER_CLONE: the "Open folder" box was checked */
    int reselect;           /* listbox row to reselect after the refresh, -1 for none */
} g_gitjob;

/* Refuses (with a message) while a git job is still running - only one at a time. */
static int git_job_busy(void)
{
    if (g_gitjob.step >= g_gitjob.nsteps)
        return 0;
    ui_msgbox_button ok = { "   OK   ", 0 };
    ui_message_box(g_screen, "Git", "A git command is still running.", &ok, 1);
    return 1;
}

static void git_job_begin(const char* title, const char* dir, enum git_job_after after)
{
    exttool_buf_free(&g_gitjob.out);
    g_gitjob.nsteps = 0;
    g_gitjob.step = 0;
    g_gitjob.failed = 0;
    g_gitjob.reselect = -1;
    g_gitjob.clone_open = 0;
    g_gitjob.clone_dest[0] = 0;
    g_gitjob.after = after;
    snprintf(g_gitjob.title, sizeof g_gitjob.title, "%s", title);
    snprintf(g_gitjob.dir, sizeof g_gitjob.dir, "%s", dir ? dir : "");
}

static void git_job_add(const char* cmd)
{
    if (g_gitjob.nsteps < GIT_JOB_MAX_STEPS)
        snprintf(g_gitjob.steps[g_gitjob.nsteps++], sizeof g_gitjob.steps[0], "%s", cmd);
}

/* Starts step g_gitjob.step; a start failure marks the job failed. */
static void git_job_start_step(void)
{
    const char* cmd = g_gitjob.steps[g_gitjob.step];
    if (g_gitjob.nsteps > 1)
    {
        exttool_append(&g_gitjob.out, "$ ");
        exttool_append(&g_gitjob.out, cmd);
        exttool_append(&g_gitjob.out, "\n");
    }
    g_gitjob.proc = ui_process_start(cmd, g_gitjob.dir[0] ? g_gitjob.dir : NULL, NULL, 0);
    if (!g_gitjob.proc)
    {
        exttool_append(&g_gitjob.out, "Failed to start git - check that it is installed and on the PATH.\n");
        g_gitjob.failed = 1;
    }
}

static void git_job_finish(void)
{
    g_gitjob.step = g_gitjob.nsteps;
    compile_status_set("");

    git_panel_refresh();
    if (g_gitjob.reselect >= 0 && g_gitjob.reselect < ui_child_count(g_git.listbox))
        ui_select_set_selected(g_git.listbox, g_gitjob.reselect);

    if (g_gitjob.after != GIT_AFTER_SHOW_ERROR || g_gitjob.failed)
    {
        ui_msgbox_button ok = { "   OK   ", 0 };
        ui_message_box(g_screen, g_gitjob.title, g_gitjob.out.len ? exttool_buf_text(&g_gitjob.out) : "(no output)", &ok, 1);
    }

    if (g_gitjob.after == GIT_AFTER_CLONE && !g_gitjob.failed && g_gitjob.clone_open)
    {
        folder_reveal_directory(g_gitjob.clone_dest);
        git_panel_refresh();
    }
    exttool_buf_free(&g_gitjob.out);
}

static void git_job_run(void)
{
    if (g_gitjob.nsteps == 0)
        return;
    char status[64];
    snprintf(status, sizeof status, "git %s...", g_gitjob.title);
    compile_status_set(status);
    git_job_start_step();
    if (g_gitjob.failed)
        git_job_finish();
}

/* Called once per frame from app_frame(). Cheap no-op when idle. */
static void git_job_poll(void)
{
    if (!g_gitjob.proc)
        return;
    char buf[4096];
    for (;;)
    {
        int n = ui_process_read(g_gitjob.proc, buf, sizeof buf);
        if (n > 0)
        {
            exttool_append_n(&g_gitjob.out, buf, (size_t)n);
            continue;
        }
        if (n == 0)
            return;  /* still running, nothing more this frame */
        break;
    }

    int rc = ui_process_close(g_gitjob.proc);
    g_gitjob.proc = NULL;
    if (rc != 0)
        g_gitjob.failed = 1;
    g_gitjob.step++;
    if (!g_gitjob.failed && g_gitjob.step < g_gitjob.nsteps)
    {
        exttool_append(&g_gitjob.out, "\n");
        git_job_start_step();
        if (!g_gitjob.failed)
            return;
    }
    git_job_finish();
}

/* Git Changes popup's "Commit" item: opens g_gitcommit's message dialog
 * (EVT_GITCOMMIT_OK does the actual work once confirmed) rather than
 * committing straight from a field that would otherwise have to sit
 * permanently visible - empty - above the panel's own listbox. */
static void git_commit_start(void)
{
    g_pending_commit_push = 0;
    g_pending_commit_staged = 0;
    g_pending_commit_file[0] = 0;
    ui_set_label(g_gitcommit.window, " Commit All ");
    ui_set_value(g_gitcommit.input, "");
    ui_screen_show_modal(g_screen, g_gitcommit.modal);
    ui_screen_focus(g_screen, g_gitcommit.input);
}

/* "Commit All && Push" - same dialog as git_commit_start(), just marks
 * g_pending_commit_push so git_commit_confirm() pushes once the commit
 * succeeds. */
static void git_commitpush_start(void)
{
    g_pending_commit_push = 1;
    g_pending_commit_staged = 0;
    g_pending_commit_file[0] = 0;
    ui_set_label(g_gitcommit.window, " Commit All ");
    ui_set_value(g_gitcommit.input, "");
    ui_screen_show_modal(g_screen, g_gitcommit.modal);
    ui_screen_focus(g_screen, g_gitcommit.input);
}

/* "Commit File" - same dialog, but commits only the listbox's selected row (stashed in g_pending_commit_file). */
static void git_commitfile_start(void)
{
    int index = ui_select_get_selected(g_git.listbox);
    if (index < 0 || index >= ui_child_count(g_git.listbox))
        return;
    const char* path = ui_get_path(ui_child_at(g_git.listbox, index));
    if (!path || !path[0])
        return;
    snprintf(g_pending_commit_file, sizeof g_pending_commit_file, "%s", path);
    g_pending_commit_push = 0;
    g_pending_commit_staged = 0;

    char title[128];
    snprintf(title, sizeof title, " Commit File: %.100s ", path);
    ui_set_label(g_gitcommit.window, title);
    ui_set_value(g_gitcommit.input, "");
    ui_screen_show_modal(g_screen, g_gitcommit.modal);
    ui_screen_focus(g_screen, g_gitcommit.input);
}

/* "Commit Staged" - same dialog, commits only what Stage already put in the index. */
static void git_commitstaged_start(void)
{
    g_pending_commit_push = 0;
    g_pending_commit_staged = 1;
    g_pending_commit_file[0] = 0;
    ui_set_label(g_gitcommit.window, " Commit Staged ");
    ui_set_value(g_gitcommit.input, "");
    ui_screen_show_modal(g_screen, g_gitcommit.modal);
    ui_screen_focus(g_screen, g_gitcommit.input);
}

/* "Commit Staged && Push" - git_commitstaged_start() plus a push after the commit. */
static void git_commitstagedpush_start(void)
{
    git_commitstaged_start();
    g_pending_commit_push = 1;
    ui_set_label(g_gitcommit.window, " Commit Staged && Push ");
}

/* Shows the staged-only items (g_git.staged_items) only when some listbox row
 * is staged - the rows' "[staged]"/"[partly staged]" suffix is what
 * git_panel_refresh() derived from `git status --porcelain`. */
static void git_popup_refresh(void)
{
    int has_staged = 0;
    for (int i = 0; i < ui_child_count(g_git.listbox) && !has_staged; i++)
    {
        const char* label = ui_get_label(ui_child_at(g_git.listbox, i));
        has_staged = label && strstr(label, "staged]") != NULL;
    }

    int count = (int)(sizeof g_git.staged_items / sizeof g_git.staged_items[0]);
    for (int k = 0; k < count; k++)
        ui_remove_child(g_git.popup, g_git.staged_items[k]);
    if (!has_staged)
        return;

    /* In order, so an item anchored on an earlier staged item finds it inserted */
    for (int k = 0; k < count; k++)
    {
        for (int i = 0; i < ui_child_count(g_git.popup); i++)
        {
            if (ui_child_at(g_git.popup, i) == g_git.staged_anchors[k])
            {
                ui_insert_child(g_git.popup, g_git.staged_items[k], i + 1);
                break;
            }
        }
    }
}

/* Runs `git <verb> -- "<selected path>"` for Stage/Unstage, showing git's output only on failure. */
static void git_run_on_selected(const char* verb, const char* title)
{
    int index = ui_select_get_selected(g_git.listbox);
    if (index < 0 || index >= ui_child_count(g_git.listbox))
        return;
    const char* path = ui_get_path(ui_child_at(g_git.listbox, index));
    if (!path || !path[0])
        return;

    if (git_job_busy())
        return;
    char cmd[1200];
    snprintf(cmd, sizeof cmd, "git %s -- \"%s\"", verb, path);
    git_job_begin(title, g_git.root[0] ? g_git.root : g_folder.dir, GIT_AFTER_SHOW_ERROR);
    git_job_add(cmd);
    g_gitjob.reselect = index;
    git_job_run();
}

static void git_do_stage(void)
{
    git_run_on_selected("add -A", "Stage");
}

static void git_do_unstage(void)
{
    /* `reset` rather than `restore --staged`: it also works before the first commit */
    git_run_on_selected("reset -q", "Unstage");
}

/* Refills g_gitbranch.listbox from `git branch`, selecting the current branch (the "* " row). */
static void git_branch_refresh(void)
{
    while (ui_child_count(g_gitbranch.listbox) > 0)
    {
        ui_node* c = ui_child_at(g_gitbranch.listbox, 0);
        ui_remove_child(g_gitbranch.listbox, c);
        ui_node_free(c);
    }

    const char* root = g_git.root[0] ? g_git.root : g_folder.dir;
    struct exttool_buf out = { 0 };
    run_process_capture("git branch --no-color", root, &out);

    int current = 0;
    const char* p = exttool_buf_text(&out);
    while (*p)
    {
        const char* eol = strchr(p, '\n');
        size_t linelen = eol ? (size_t)(eol - p) : strlen(p);
        while (linelen > 0 && p[linelen - 1] == '\r')
            linelen--;
        if (linelen > 2)
        {
            char label[512];
            char name[512];
            size_t n = linelen < sizeof label ? linelen : sizeof label - 1;
            memcpy(label, p, n);
            label[n] = 0;
            snprintf(name, sizeof name, "%s", label + 2);

            ui_node* item = ui_create_element(UI_TAG_ITEM);
            ui_set_label(item, label);
            ui_set_path(item, name);
            if (p[0] == '*')
                current = ui_child_count(g_gitbranch.listbox);
            ui_append_child(g_gitbranch.listbox, item);
        }
        p = eol ? eol + 1 : p + linelen;
    }
    exttool_buf_free(&out);
    ui_select_set_selected(g_gitbranch.listbox, current);
}

static void git_branch_start(void)
{
    git_branch_refresh();
    ui_set_value(g_gitbranch.input, "");
    ui_screen_show_modal(g_screen, g_gitbranch.modal);
    ui_screen_focus(g_screen, g_gitbranch.listbox);
}

/* Shared tail of Checkout/New: closes the dialog and runs `cmd` as a git job, which shows git's output. */
static void git_branch_run(const char* cmd)
{
    if (git_job_busy())
        return;
    ui_screen_close_modal(g_screen, g_gitbranch.modal);
    git_job_begin("Branch", g_git.root[0] ? g_git.root : g_folder.dir, GIT_AFTER_SHOW);
    git_job_add(cmd);
    git_job_run();
}

static void git_branch_checkout(void)
{
    int index = ui_select_get_selected(g_gitbranch.listbox);
    if (index < 0 || index >= ui_child_count(g_gitbranch.listbox))
        return;
    const char* name = ui_get_path(ui_child_at(g_gitbranch.listbox, index));
    if (!name || !name[0])
        return;
    char cmd[700];
    snprintf(cmd, sizeof cmd, "git checkout \"%s\"", name);
    git_branch_run(cmd);
}

/* Creates the branch named in the input from HEAD and switches to it (`git checkout -b`). */
static void git_branch_new(void)
{
    const char* raw = ui_get_value(g_gitbranch.input);
    char name[512];
    snprintf(name, sizeof name, "%s", raw ? raw : "");
    if (!name[0] || strpbrk(name, " \t\"\\"))
    {
        ui_msgbox_button ok = { "   OK   ", 0 };
        ui_message_box(g_screen, "Branch", "Enter a branch name without spaces or quotes.", &ok, 1);
        return;
    }
    char cmd[700];
    snprintf(cmd, sizeof cmd, "git checkout -b \"%s\"", name);
    git_branch_run(cmd);
}

/* EVT_GITCOMMIT_OK: `git add -A` (stages everything - there's no separate
 * staging step in this UI, it always shows/commits the whole working tree)
 * then `git commit -m <message>`. Shows git's own output in a message box
 * either way, so a mistake (nothing to commit, a hook rejection, ...) is
 * visible, and always refreshes the panel after. If g_pending_commit_push
 * was set (git_commitpush_start()), also runs git_do_push() after a
 * successful commit. */
static void git_commit_confirm(void)
{
    const char* msg = ui_get_value(g_gitcommit.input);
    if (!msg || !msg[0])
    {
        ui_msgbox_button ok = { "   OK   ", 0 };
        ui_message_box(g_screen, "Commit", "Enter a commit message first.", &ok, 1);
        return;
    }

    if (git_job_busy())
        return;

    const char* root = g_git.root[0] ? g_git.root : g_folder.dir;
    char escaped[1024];
    shell_escape_dq(msg, escaped, sizeof escaped);

    /* `git commit` exits non-zero on "nothing to commit", which also skips the push */
    git_job_begin("Commit", root, GIT_AFTER_SHOW);
    char cmd[2400];
    if (g_pending_commit_file[0])
    {
        /* `git add` first so an untracked or deleted file can be committed too */
        snprintf(cmd, sizeof cmd, "git add -A -- \"%s\"", g_pending_commit_file);
        git_job_add(cmd);
        snprintf(cmd, sizeof cmd, "git commit -m \"%s\" -- \"%s\"", escaped, g_pending_commit_file);
    }
    else
    {
        if (!g_pending_commit_staged)
            git_job_add("git add -A");
        snprintf(cmd, sizeof cmd, "git commit -m \"%s\"", escaped);
    }
    git_job_add(cmd);
    if (g_pending_commit_push)
        git_job_add("git push");
    g_pending_commit_file[0] = 0;
    g_pending_commit_staged = 0;
    g_pending_commit_push = 0;

    ui_screen_close_modal(g_screen, g_gitcommit.modal);
    git_job_run();
}

/* "Pull"/"Push" - plain `git pull`/`git push` against whatever remote/branch
 * is already configured (same as typing them by hand in this repo - no
 * remote/branch picker here), showing git's own output either way so a
 * failure (no upstream, conflicts, auth) is visible instead of silent. */
static void git_do_pull(void)
{
    if (git_job_busy())
        return;
    git_job_begin("Pull", g_git.root[0] ? g_git.root : g_folder.dir, GIT_AFTER_SHOW);
    git_job_add("git pull");
    git_job_run();
}

static void git_do_push(void)
{
    if (git_job_busy())
        return;
    git_job_begin("Push", g_git.root[0] ? g_git.root : g_folder.dir, GIT_AFTER_SHOW);
    git_job_add("git push");
    git_job_run();
}

/* "Sync" - `git pull` then `git push` as one job, so both outputs read as one
 * result and a failed pull skips the push. */
static void git_do_sync(void)
{
    if (git_job_busy())
        return;
    git_job_begin("Sync", g_git.root[0] ? g_git.root : g_folder.dir, GIT_AFTER_SHOW);
    git_job_add("git pull");
    git_job_add("git push");
    git_job_run();
}

/* Copies `raw` into `out` without surrounding spaces/tabs/line breaks - a
 * pasted URL or path often carries them, and they would end up inside the
 * quoted git arguments and the folder name. */
static void git_clone_trim(const char* raw, char* out, size_t out_size)
{
    snprintf(out, out_size, "%s", raw ? raw : "");
    size_t lead = strspn(out, " \t\r\n");
    memmove(out, out + lead, strlen(out + lead) + 1);
    size_t n = strlen(out);
    while (n > 0 && strchr(" \t\r\n", out[n - 1]))
        out[--n] = 0;
}

/* The repository's name from its URL: the last path segment, trailing "/"
 * and ".git" stripped - the same name plain `git clone <url>` picks for its
 * directory. Empty if the URL has none. */
static void git_url_repo_name(const char* url, char* name, size_t name_size)
{
    size_t ulen = strlen(url);
    while (ulen > 0 && (url[ulen - 1] == '/' || url[ulen - 1] == '\\'))
        ulen--;
    size_t start = ulen;
    while (start > 0 && url[start - 1] != '/' && url[start - 1] != '\\' && url[start - 1] != ':')
        start--;
    size_t nlen = ulen - start;
    if (nlen >= name_size)
        nlen = name_size - 1;
    memcpy(name, url + start, nlen);
    name[nlen] = 0;
    if (nlen > 4 && strcmp(name + nlen - 4, ".git") == 0)
        name[nlen - 4] = 0;
}

/* Sets the Folder field to `parent`/`name` and remembers `name` as the
 * suggested part, so the next URL change can swap just that part. */
static void git_clone_set_folder(const char* parent, const char* name)
{
    char path[1024];
    size_t plen = strlen(parent);
    int has_sep = plen > 0 && (parent[plen - 1] == '/' || parent[plen - 1] == '\\');
    if (!name[0])
        snprintf(path, sizeof path, "%s", parent);
    else
        snprintf(path, sizeof path, "%s%s%s", parent, has_sep || plen == 0 ? "" : "/", name);
    ui_set_value(g_gitclone.folder_input, path);
    snprintf(g_gitclone.suggested_name, sizeof g_gitclone.suggested_name, "%s", name);
}

/* The Folder field without the name git_clone_set_folder() last suggested
 * - i.e. the parent the user chose. The whole field if it no longer ends
 * with that name (edited by hand). */
static void git_clone_folder_parent(char* out, size_t out_size)
{
    git_clone_trim(ui_get_value(g_gitclone.folder_input), out, out_size);
    size_t len = strlen(out);
    size_t nlen = strlen(g_gitclone.suggested_name);
    if (nlen > 0 && len > nlen &&
        strcmp(out + len - nlen, g_gitclone.suggested_name) == 0 &&
        (out[len - nlen - 1] == '/' || out[len - nlen - 1] == '\\'))
    {
        out[len - nlen - 1] = 0;
    }
}

/* Called every frame while the Clone dialog is open: when the URL changes,
 * the Folder field follows it - parent folder + repository name - like
 * Visual Studio's Clone dialog. A Folder field edited by hand is kept as
 * the parent, with the new name appended. */
static void git_clone_update_suggestion(void)
{
    if (ui_screen_active_modal(g_screen) != g_gitclone.modal)
        return;
    char url[1024];
    git_clone_trim(ui_get_value(g_gitclone.input), url, sizeof url);
    if (strcmp(url, g_gitclone.last_url) == 0)
        return;
    snprintf(g_gitclone.last_url, sizeof g_gitclone.last_url, "%s", url);

    char parent[1024];
    git_clone_folder_parent(parent, sizeof parent);
    char name[256];
    git_url_repo_name(url, name, sizeof name);
    git_clone_set_folder(parent, name);
}

/* Git Changes popup's "Clone..." item: opens g_gitclone's URL+Folder dialog
 * (EVT_GITCLONE_OK does the actual work once confirmed) - same shape as
 * git_commit_start()/g_gitcommit. The Folder field starts at the currently
 * open folder (or cwd) and gets the repository name appended as the URL is
 * typed (git_clone_update_suggestion). */
static void git_clone_start(void)
{
    ui_set_value(g_gitclone.input, "");
    g_gitclone.last_url[0] = 0;
    char parent[1024];
    if (g_folder.dir[0])
        snprintf(parent, sizeof parent, "%s", g_folder.dir);
    else if (!ui_get_cwd(parent, sizeof parent))
        strcpy(parent, ".");
    git_clone_set_folder(parent, "");
    ui_group_set_checked(g_gitclone.open_folder_check, 0, 1);
    ui_screen_show_modal(g_screen, g_gitclone.modal);
    ui_screen_focus(g_screen, g_gitclone.input);
}

/* EVT_GITCLONE_OK: `git clone <url> <folder>` - the Folder field is the
 * final destination, like Visual Studio's Clone dialog. git creates it,
 * along with any missing parent folders. An existing folder is refused up
 * front (the dialog stays open to change it) rather than cloning on top of
 * it. On success, opens the result via folder_reveal_directory() (same
 * "point the persistent Folder panel at a directory and raise it" idiom
 * used elsewhere), so cloning behaves like an Open Folder trip to it. */
static void git_clone_confirm(void)
{
    char url[1024];
    git_clone_trim(ui_get_value(g_gitclone.input), url, sizeof url);
    if (!url[0])
    {
        ui_msgbox_button ok = { "   OK   ", 0 };
        ui_message_box(g_screen, "Clone", "Enter a repository URL first.", &ok, 1);
        return;
    }

    /* A trailing separator is dropped (except on a bare root) - Windows'
     * stat() rejects "C:\dir\" even when the directory exists. */
    char dest[1024];
    git_clone_trim(ui_get_value(g_gitclone.folder_input), dest, sizeof dest);
    size_t dlen = strlen(dest);
    while (dlen > 1 && (dest[dlen - 1] == '/' || dest[dlen - 1] == '\\') && dest[dlen - 2] != ':')
        dest[--dlen] = 0;
    if (!dest[0])
    {
        ui_msgbox_button ok = { "   OK   ", 0 };
        ui_message_box(g_screen, "Clone", "Enter the destination folder first.", &ok, 1);
        return;
    }

    struct stat dest_st;
    if (stat(dest, &dest_st) == 0)
    {
        char msg[1200];
        snprintf(msg, sizeof msg, "The folder already exists:\n%s\n\nChoose another folder.", dest);
        ui_msgbox_button ok = { "   OK   ", 0 };
        ui_message_box(g_screen, "Clone", msg, &ok, 1);
        return;
    }

    char escaped_url[1024];
    shell_escape_dq(url, escaped_url, sizeof escaped_url);
    char escaped_dest[1024];
    shell_escape_dq(dest, escaped_dest, sizeof escaped_dest);
    char cmd[2200];
    snprintf(cmd, sizeof cmd, "git clone \"%s\" \"%s\"", escaped_url, escaped_dest);

    if (git_job_busy())
        return;
    ui_screen_close_modal(g_screen, g_gitclone.modal);
    git_job_begin("Clone", NULL, GIT_AFTER_CLONE);
    git_job_add(cmd);
    snprintf(g_gitjob.clone_dest, sizeof g_gitjob.clone_dest, "%s", dest);
    g_gitjob.clone_open = ui_group_get_checked(g_gitclone.open_folder_check, 0);
    git_job_run();
}

/* "Discard" - asks for confirmation (stashing the target in g_pending_git_
 * discard_path/g_pending_git_untracked, consumed by EVT_GIT_DISCARD_CONFIRM
 * below - same "stash it before the prompt" shape as g_pending_delete_path),
 * then either deletes an untracked file outright (git has nothing to restore
 * it FROM) or runs `git checkout -- <path>` to discard a tracked file's
 * changes back to HEAD. Acts on the listbox's current selection, same
 * "selection, not necessarily whatever row was last clicked" caveat as
 * EVT_FOLDER_DELETE. */
static void git_do_discard(void)
{
    int index = ui_select_get_selected(g_git.listbox);
    if (index < 0 || index >= ui_child_count(g_git.listbox))
        return;
    ui_node* item = ui_child_at(g_git.listbox, index);
    const char* path = ui_get_path(item);
    const char* label = ui_get_label(item);
    if (!path || !path[0])
        return;

    g_pending_git_untracked = label && (label[0] == '?' || label[0] == 'A');
    snprintf(g_pending_git_discard_path, sizeof g_pending_git_discard_path, "%s", path);

    char message[1200];
    snprintf(message, sizeof message,
             g_pending_git_untracked
                 ? "Delete this new file?\n%s"
                 : "Discard changes to this file?\n%s\n\nThis restores it to the last commit - not undoable.",
             path);
    ui_msgbox_button btns[] = {
        { "   OK   ", EVT_GIT_DISCARD_CONFIRM },
        { " Cancel ", 0 },
    };
    ui_message_box(g_screen, "Discard", message, btns, 2);
}

/* The open project's own target platform name (e.g. "x64", "x86") - the same
 * subdirectory name component cake's own output uses (compile.c's
 * longest_common_path/get_platform use in compile()), and what $(Target)
 * expands to on its own. Falls back to this build's own compile-time target,
 * same default compile() itself uses, when the project has none set. Only
 * meaningful with a project open - callers check project_is_open() first. */
static const char* project_target_platform_name(void)
{
    enum target target_enum = TARGET_DEFAULT;
    if (g_project.compile.target[0])
        parse_target(g_project.compile.target, &target_enum);
    return get_platform(target_enum)->name;
}

/* The platform slug every $(Target...) macro and the debugger's own launch
 * path are built from: the open project's, else whatever the user picked
 * in Compiler Options, else this build's compile-time default. The same
 * three-way fallback exttool_append_cake_output() and do_debug_start()
 * each spell out inline - new code shares this one instead. */
static const char* active_platform_name(void)
{
    if (project_is_open())
        return project_target_platform_name();
    enum target target_enum = TARGET_DEFAULT;
    if (g_compile.target[0])
        parse_target(g_compile.target, &target_enum);
    return get_platform(target_enum)->name;
}

/* $(TargetDir) - the directory cake writes its generated output into, and
 * where an external compiler is expected to leave the executable:
 * "<root>/<platform>", root being the open project's directory or, with no
 * project, the active document's own (`doc_dir`). No trailing separator, so
 * it composes as "$(TargetDir)/$(TargetFileName)". */
static void target_dir_path(const char* doc_dir, char* out, size_t cap)
{
    const char* root = project_is_open() ? g_project.dir : doc_dir;
    path_join(out, cap, root, active_platform_name());
}

/* $(TargetFileName) - the executable's file NAME (with extension), not a
 * path. Compiler Options' own "Output" field when the user set one, taken
 * verbatim; otherwise the project's name, or the active document's base
 * name (`doc_base`) with no project open. MSVC targets link to "<name>.exe"
 * (cl.exe's own default output name), the gcc/clang ones to a bare name -
 * same split do_debug_start() has always made for its own launch path,
 * which now goes through here so the two can't disagree. */
static void target_file_name(const char* doc_base, char* out, size_t cap)
{
    const compile_settings* cs = project_is_open() ? &g_project.compile : &g_compile;
    if (cs->output[0])
    {
        snprintf(out, cap, "%s", cs->output);
        return;
    }
    const char* platform_name = active_platform_name();
    snprintf(out, cap, "%s%s", project_is_open() ? g_project.name : doc_base,
             strstr(platform_name, "msvc") ? ".exe" : "");
}

/* $(CakeOutput) - see exttool_expand()'s own doc comment: cake's own
 * predicted OUTPUT paths (not the sources), quoted and space-separated,
 * ready to hand straight to a real compiler as its input file list. Cake
 * writes generated output to "<root>/<platform name>/<relative path>".
 *
 * With a project open, root is g_project.dir and every .c file in the
 * project contributes one entry (true whenever the project's files don't
 * all live deeper in some shared subdirectory of it).
 *
 * Without one, there is no file list to walk - root becomes the active
 * document's own directory instead, and the macro expands to just that
 * one file's predicted output path (empty if there's no active .c
 * document, same as a file macro with no active document elsewhere). This
 * is the same "<its own dir>/<platform>/<name><ext>" prediction
 * do_debug_start() computes by hand for exactly this no-project case (see
 * its own doc comment) - kept in sync with it rather than duplicating a
 * third slightly different guess at cake's output layout. */
static void exttool_append_cake_output(struct exttool_buf* out,
                                        const char* path, const char* dir,
                                        const char* name, const char* ext)
{
    /* No project open: use whatever target the user picked in Compiler
     * Options (g_compile.target - see do_debug_start()'s own matching fix),
     * not this build's own compile-time default. */
    const char* platform_name = active_platform_name();

    if (project_is_open())
    {
        int first = 1;
        for (int i = 0; i < g_project.file_count; i++)
        {
            const char* entry = g_project.files[i];
            size_t elen = strlen(entry);
            if (elen < 2 || entry[elen - 2] != '.' || entry[elen - 1] != 'c')
                continue;

            /* Entries are normally stored relative to g_project.dir, but
             * an absolute one is legal (project_make_relative's own
             * fallback, and project files written before it normalized
             * slashes hold them): gluing that onto "<dir>/<platform>/"
             * builds a doubled path the compiler cannot open. Relativize
             * first, and for a file genuinely outside the project predict
             * against its own directory, the way the no-project branch
             * below does. */
            char rel[512];
            project_make_relative(g_project.dir, entry, rel, sizeof rel);

            /* Appended a component at a time rather than through one
             * snprintf'd scratch buffer: an absolute g_project.dir plus a
             * long entry already overflows any fixed size worth writing
             * down here, and a path cut short is worse than a long one. */
            if (!first)
                exttool_append(out, " ");
            exttool_append(out, "\"");
            if (rel[0] == '/' ||
                (isalpha((unsigned char)rel[0]) && rel[1] == ':'))
            {
                char edir[512];
                snprintf(edir, sizeof edir, "%s", rel);
                char* slash = strrchr(edir, '/');
                const char* base = slash ? slash + 1 : edir;
                if (slash)
                    *slash = 0;
                else
                    edir[0] = 0;
                exttool_append(out, edir);
                if (edir[0])
                    exttool_append(out, "/");
                exttool_append(out, platform_name);
                exttool_append(out, "/");
                exttool_append(out, base);
            }
            else
            {
                exttool_append(out, g_project.dir);
                exttool_append(out, "/");
                exttool_append(out, platform_name);
                exttool_append(out, "/");
                exttool_append(out, rel);
            }
            exttool_append(out, "\"");
            first = 0;
        }
        return;
    }

    if (!path || !path[0] || strcmp(ext, ".c") != 0)
        return;   /* no active document, or it isn't a .c file */

    exttool_append(out, "\"");
    exttool_append(out, dir);
    if (dir[0])
        exttool_append(out, "/");
    exttool_append(out, platform_name);
    exttool_append(out, "/");
    exttool_append(out, name);
    exttool_append(out, ext);
    exttool_append(out, "\"");
}

/* Expands the $(...) macros above in `in`, writing to `out`. `path` is the
 * active document's full path ("" when there is none, which simply makes
 * every file macro expand to nothing rather than failing). */
static void exttool_expand(const char* in, const char* path, struct exttool_buf* out)
{
    char dir[1024] = { 0 }, name[512] = { 0 }, ext[64] = { 0 };
    if (path && path[0])
    {
        snprintf(dir, sizeof dir, "%s", path);
        char* slash = strrchr(dir, '/');
        char* back = strrchr(dir, '\\');
        if (back && (!slash || back > slash))
            slash = back;
        const char* base = slash ? slash + 1 : dir;
        snprintf(name, sizeof name, "%s", base);
        if (slash)
            *slash = 0;          /* dir now holds just the directory */
        else
            dir[0] = 0;          /* bare filename - no directory part */

        char* dot = strrchr(name, '.');
        if (dot)
        {
            snprintf(ext, sizeof ext, "%s", dot);
            *dot = 0;            /* name now has no extension */
        }
    }

    for (const char* p = in; *p; )
    {
        if (p[0] == '$' && p[1] == '$')          /* "$$" -> literal '$' */
        {
            exttool_append(out, "$");
            p += 2;
        }
        else if (p[0] == '$' && p[1] == '(')
        {
            const char* close = strchr(p + 2, ')');
            if (!close)                          /* unterminated - copy as-is */
            {
                exttool_append_n(out, p, 1);
                p++;
                continue;
            }
            size_t n = (size_t)(close - (p + 2));
            char macro[32];
            if (n >= sizeof macro)
                n = sizeof macro - 1;
            memcpy(macro, p + 2, n);
            macro[n] = 0;

            /* Visual Studio names these Item*; both spellings work, the
             * File* ones being what this IDE shipped with. */
            if (strcmp(macro, "FilePath") == 0 ||
                strcmp(macro, "ItemPath") == 0)      exttool_append(out, path ? path : "");
            else if (strcmp(macro, "FileDir") == 0 ||
                     strcmp(macro, "ItemDir") == 0)  exttool_append(out, dir);
            else if (strcmp(macro, "FileName") == 0 ||
                     strcmp(macro, "ItemFilename") == 0) exttool_append(out, name);
            else if (strcmp(macro, "FileExt") == 0 ||
                     strcmp(macro, "ItemExt") == 0)  exttool_append(out, ext);
            else if (strcmp(macro, "CakeOutput") == 0)
                exttool_append_cake_output(out, path, dir, name, ext);
            /* Visual Studio's own External Tools vocabulary for the built
             * binary (see its Macros menu): $(TargetDir) the folder it
             * lands in, $(TargetName)/$(TargetExt)/$(TargetFileName) its
             * name split three ways, $(TargetPath) the whole thing - which
             * is exactly what Debug launches, so a tool that writes to
             * $(TargetPath) and F5 can never disagree. */
            else if (strcmp(macro, "TargetDir") == 0)
            {
                char buf[1024];
                target_dir_path(dir, buf, sizeof buf);
                exttool_append(out, buf);
            }
            else if (strcmp(macro, "TargetFileName") == 0)
            {
                char buf[512];
                target_file_name(name, buf, sizeof buf);
                exttool_append(out, buf);
            }
            else if (strcmp(macro, "TargetName") == 0 ||
                     strcmp(macro, "TargetExt") == 0)
            {
                /* The same resolved file name, split at its last dot:
                 * "main.exe" -> "main" + ".exe". A gcc/clang target has
                 * no extension at all, so $(TargetExt) is empty there. */
                char buf[512];
                target_file_name(name, buf, sizeof buf);
                char* dot = strrchr(buf, '.');
                if (macro[6] == 'N')          /* TargetName */
                {
                    if (dot)
                        *dot = 0;
                    exttool_append(out, buf);
                }
                else                          /* TargetExt */
                    exttool_append(out, dot ? dot : "");
            }
            else if (strcmp(macro, "TargetPath") == 0)
            {
                char d[1024], f[512], buf[1600];
                target_dir_path(dir, d, sizeof d);
                target_file_name(name, f, sizeof f);
                path_join(buf, sizeof buf, d, f);
                exttool_append(out, buf);
            }
            else if (strcmp(macro, "Platform") == 0)
                /* Visual Studio's own name for the architecture (its
                 * $(Platform) is "x64"); $(Target) below predates this
                 * and means the same thing here, kept so tools already
                 * configured with it keep working. */
                exttool_append(out, active_platform_name());
            else if (strcmp(macro, "ProjectName") == 0)
                exttool_append(out,
                    project_is_open() ? g_project.name : name);
            else if (strcmp(macro, "ProjectDir") == 0)
                /* The open project's own directory - what a tool that
                 * builds or outputs "for this project" wants, rather than
                 * the active document's $(FileDir), which points at
                 * whatever happens to be focused (the playground is a
                 * scratch file, never part of a project, so a tool string
                 * using $(FileDir) silently retargets to %APPDATA% the
                 * moment it has focus). Falls back to the active
                 * document's directory with no project open, so the same
                 * tool string still works on a standalone file - the same
                 * no-project fallback $(CakeOutput) and $(Target) make. */
                exttool_append(out,
                    project_is_open() ? g_project.dir : dir);
            else if (strcmp(macro, "Target") == 0)
                /* Same no-project fallback as exttool_append_cake_output's
                 * own - kept consistent since "$(CakeOutput) $(Target)" is
                 * the actual default args string (see copts_window's
                 * build_screen() block), and leaving just this one still
                 * gated on project_is_open() would silently drop half of
                 * that pair for a standalone file. */
                exttool_append(out, active_platform_name());
            /* An unknown macro expands to nothing, rather than being left in
             * the command line where it would confuse the program. */
            p = close + 1;
        }
        else
        {
            /* Copy the whole run of ordinary text up to the next '$' in
             * one go rather than a byte at a time. */
            const char* start = p;
            while (*p && *p != '$')
                p++;
            exttool_append_n(out, start, (size_t)(p - start));
        }
    }
}

/* --- Streaming compile ---------------------------------------------------
 *
 * compile() is linked in and called IN-PROCESS (deliberately: it keeps the
 * compiler debuggable from this same debugger session - breakpoints, call
 * stack, stepping all work). The cost is that it printf()s its diagnostics
 * and has no other channel back to us, and it runs for many seconds on a
 * large file.
 *
 * The old shape redirected stdout to a temp file, called compile(), and
 * only read the file back once it returned - so nothing appeared until the
 * end, and because do_compile() is invoked from inside app_frame() (the
 * WM_TIMER tick), the whole message loop was blocked meanwhile: no repaint,
 * no scrolling, frozen window for the duration.
 *
 * Now: stdout is redirected onto an anonymous PIPE, compile() runs on a
 * worker thread, and the main thread drains the pipe's read end without
 * blocking once per frame (see compile_stream_poll), appending whatever has
 * arrived to the Output window. Same process, same debuggability - only the
 * frame loop stops being held hostage.
 *
 * Two traps worth naming, both already implied by the note this replaces:
 *
 * 1. A GUI-subsystem build has no console, so stdout's underlying fd is
 *    invalid from process start and _dup2 onto it fails. freopen() to the
 *    null device FIRST gives the stream a real fd, which _dup2 can then
 *    legally replace with the pipe.
 * 2. stdout to a pipe is fully buffered by default (~4 KB), which would
 *    defeat the entire point - output would arrive in lumps, not as
 *    produced. setvbuf(_IONBF) makes each printf reach the pipe
 *    immediately.
 */

#include "tinycthread.h"  /* C11-style threads - one API on every backend */

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#pragma comment(lib, "advapi32.lib")  /* Reg* - detect_system_include_dirs() */
/* MSVC provides these under their POSIX names too, but deprecated (C4996).
 * Alias to the underscored spellings so the shared code below reads the
 * same on every platform without warnings. */
#define ide_dup    _dup
#define ide_dup2   _dup2
#define ide_close  _close
#define ide_fileno _fileno
#else
#include <unistd.h>
#include <fcntl.h>
#define ide_dup    dup
#define ide_dup2   dup2
#define ide_close  close
#define ide_fileno fileno
#endif

/* Fills `dirs` with this machine's system include directories - the ones the
 * platform compiler itself searches - for the System Directories dialog's
 * "Detect" button (EVT_PROJECT_INCLUDES_DETECT). Returns how many were found.
 *
 * Same idea as compile.c's collect_system_include_dirs (cake -autoconfig),
 * except on Windows: that one reads INCLUDE, which only exists inside a
 * Visual Studio command prompt, and the IDE is rarely launched from one - so
 * here MSVC's headers are located with vswhere.exe and the Windows SDK's
 * from the registry.
 *
 * Whatever could not be found is described in `problems` (one line each,
 * "" when everything was), for the message box the caller shows - a
 * partial result, e.g. an SDK but no MSVC, still returns what it found. */
static int detect_system_include_dirs(char (*dirs)[512], int max,
                                      char* problems, int problems_size)
{
    int count = 0;
    problems[0] = 0;

#ifdef _WIN32

    /* MSVC: vswhere.exe is installed at this fixed place by the Visual
     * Studio installer (2017 and later); asks for the newest install that
     * has the C/C++ toolset. */
    char pf86[MAX_PATH] = { 0 };
    DWORD n = GetEnvironmentVariableA("ProgramFiles(x86)", pf86, sizeof pf86);
    if (n > 0 && n < sizeof pf86)
    {
        /* The outermost quotes are for cmd.exe (ui_process_start runs
         * "cmd /c <command>") - without them it strips the inner pair
         * around a path containing "(x86)". */
        char cmd[1024];
        snprintf(cmd, sizeof cmd,
                 "\"\"%s\\Microsoft Visual Studio\\Installer\\vswhere.exe\" -latest -products * "
                 "-requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath\"",
                 pf86);
        struct exttool_buf out = { 0 };
        run_process_capture(cmd, NULL, &out);
        char vs_dir[512];
        snprintf(vs_dir, sizeof vs_dir, "%s", exttool_buf_text(&out));
        vs_dir[strcspn(vs_dir, "\r\n")] = 0;
        exttool_buf_free(&out);

        /* The toolset version the install builds with by default - the
         * same file vcvarsall.bat reads to pick VC\Tools\MSVC\<version>.
         * Missing (no VS, or vswhere printed an error instead of a path)
         * just means no MSVC entry. */
        char version_file[700];
        snprintf(version_file, sizeof version_file,
                 "%s\\VC\\Auxiliary\\Build\\Microsoft.VCToolsVersion.default.txt", vs_dir);
        char* version = vs_dir[0] ? read_file_to_string(version_file) : NULL;
        if (version)
        {
            version[strcspn(version, " \t\r\n")] = 0;
            if (count < max)
                snprintf(dirs[count++], 512, "%s\\VC\\Tools\\MSVC\\%s\\include", vs_dir, version);
            free(version);
        }
        else
        {
            snprintf(problems + strlen(problems), problems_size - strlen(problems),
                     "Visual Studio with the C/C++ toolset was not found (vswhere.exe).\n");
        }
    }
    else
    {
        snprintf(problems + strlen(problems), problems_size - strlen(problems),
                 "ProgramFiles(x86) is not set - cannot locate vswhere.exe.\n");
    }

    /* Windows SDK: the root is the KitsRoot10 value, and every installed
     * version is a subkey of the same key (named like "10.0.22621.0") -
     * the newest one whose headers are really on disk wins, same default
     * as vcvarsall.bat. */
    HKEY key;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots",
                      0, KEY_READ | KEY_WOW64_32KEY, &key) == ERROR_SUCCESS)
    {
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
                char name[64];
                DWORD name_len = sizeof name;
                if (RegEnumKeyExA(key, i, name, &name_len, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)
                    break;

                unsigned v[4] = { 0 };
                if (sscanf(name, "%u.%u.%u.%u", &v[0], &v[1], &v[2], &v[3]) != 4)
                    continue;

                /* The key can outlive an uninstalled SDK. */
                char probe[MAX_PATH + 100];
                snprintf(probe, sizeof probe, "%sInclude\\%s\\ucrt", root, name);
                if (GetFileAttributesA(probe) == INVALID_FILE_ATTRIBUTES)
                    continue;

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

        if (best[0])
        {
            /* Same subdirectories, in the same order, as the INCLUDE a
             * Visual Studio command prompt sets. */
            static const char* const subdirs[] = { "ucrt", "um", "shared", "winrt", "cppwinrt" };
            for (int i = 0; i < (int)_Countof(subdirs) && count < max; i++)
            {
                char dir[512];
                snprintf(dir, sizeof dir, "%sInclude\\%s\\%s", root, best, subdirs[i]);
                if (GetFileAttributesA(dir) != INVALID_FILE_ATTRIBUTES)
                    snprintf(dirs[count++], 512, "%s", dir);
            }
        }
        else
        {
            snprintf(problems + strlen(problems), problems_size - strlen(problems),
                     "No Windows 10/11 SDK headers were found (KitsRoot10 in the registry).\n");
        }
    }
    else
    {
        snprintf(problems + strlen(problems), problems_size - strlen(problems),
                 "The Windows SDK is not in the registry "
                 "(HKLM\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots).\n");
    }

#else

    /* Parsed out of the platform compiler's own "-v -E" output, between
     * "#include <...> search starts here:" and "End of search list." -
     * same as collect_system_include_dirs. */
#ifdef __APPLE__
    const char* cmd = "echo | clang -v -E - 2>&1";
#else
    const char* cmd = "echo | gcc -v -E - 2>&1";
#endif
    struct exttool_buf out = { 0 };
    run_process_capture(cmd, NULL, &out);

    const char* p = exttool_buf_text(&out);
    int in_include_section = 0;
    while (*p && count < max)
    {
        const char* eol = strchr(p, '\n');
        size_t len = eol ? (size_t)(eol - p) : strlen(p);
        char line[512];
        snprintf(line, sizeof line, "%.*s", (int)len, p);
        p += len + (eol ? 1 : 0);
        line[strcspn(line, "\r")] = 0;

        if (strstr(line, "#include <...> search starts here:") != NULL)
        {
            in_include_section = 1;
            continue;
        }
        if (!in_include_section)
            continue;
        if (strstr(line, "End of search list.") != NULL)
            break;

        char* dir = line;
        while (*dir == ' ')
            dir++;

        /* clang labels macOS framework search paths with a trailing
         * " (framework directory)" - it is not part of the path. */
        char* framework_tag = strstr(dir, " (framework directory)");
        if (framework_tag != NULL)
            *framework_tag = 0;

        if (dir[0])
            snprintf(dirs[count++], 512, "%s", dir);
    }

    /* The compiler's own output says why - typically "gcc: not found". */
    if (count == 0)
        snprintf(problems, problems_size, "No include search list in the output of\n  %s\n\n%s",
                 cmd, exttool_buf_text(&out));
    exttool_buf_free(&out);

#endif

    return count;
}

/* ui_open_url() for a file on disk: turns an absolute local path
 * into a file:// URL (backslashes to slashes, a leading "/" before a drive
 * letter, and everything outside the unreserved/path-safe set %-escaped so a
 * space or a '#' in the path doesn't get read as URL syntax) and hands it
 * off. `fragment`, if non-NULL, is appended as "#fragment" (already URL-safe
 * by contract - an id written in the .md source). Same return contract as
 * ui_open_url(). */
static int open_local_file_in_browser(const char* path, const char* fragment)
{
    if (!path || !path[0])
        return 0;

    char url[3 * FS_MAX_PATH + 16];
    size_t n = 0;
    memcpy(url, "file://", 7);
    n = 7;
    if (path[0] != '/' && path[0] != '\\')
        url[n++] = '/';  /* "C:/..." needs the third slash: file:///C:/... */

    for (const unsigned char* p = (const unsigned char*)path; *p && n + 4 < sizeof url; p++)
    {
        unsigned char c = *p;
        if (c == '\\')
            c = '/';
        if (isalnum(c) || c == '/' || c == ':' || c == '.' || c == '-' ||
            c == '_' || c == '~')
        {
            url[n++] = (char)c;
        }
        else
        {
            static const char hex[] = "0123456789ABCDEF";
            url[n++] = '%';
            url[n++] = hex[c >> 4];
            url[n++] = hex[c & 15];
        }
    }
    url[n] = 0;
    if (fragment && fragment[0])
        snprintf(url + n, sizeof url - n, "#%s", fragment);
    return ui_open_url(url);
}

#define COMPILE_STREAM_MAX_BYTES (8 * 1024 * 1024)

static struct compile_job
{
    int running;          /* a compile is in flight - guards re-entry */
    int finished;         /* worker returned; drain the pipe, then finalize */

    /* argv must outlive do_compile()'s stack frame now that the compile
     * runs on another thread, so the strings live here instead. */
    char storage[64][512];
    const char* argv[64];
    int argc;

    struct report report;

    char* text;           /* everything read from the pipe so far */
    size_t len, cap;
    int lines;            /* newlines seen - keeps the Output view pinned to
                           * the bottom without rescanning the buffer every
                           * frame (there is no ui_editor_line_count) */

    ui_node* active;      /* window being compiled - captured at start */

    int saved_stdout;     /* dup of the original stdout fd, restored at end */

    thrd_t thread;

    /* Non-NULL when this job is an External Tool rather than the in-process
     * compile: output then comes from a child process's pipe instead of the
     * worker thread's, and there is no report/diagnostic ownership to hand
     * back (see compile_stream_poll / exttool_finish). */
    ui_process* proc;
    char proc_title[64];   /* tool name, for the status bar and Output header */

    /* The response file this tool's arguments were moved into, "" when the
     * command line was short enough to pass directly (see
     * exttool_write_response_file). Deleted by exttool_finish once the
     * child that reads it has exited. */
    char rsp_path[FS_MAX_PATH];

    /* The pipe itself still has to be platform-specific: Win32 needs the
     * HANDLE for PeekNamedPipe (the only way to check "is there anything to
     * read" without blocking), while POSIX gets the same effect from a
     * plain O_NONBLOCK fd. */
#ifdef _WIN32
    HANDLE hread, hwrite;
#else
    int fdread, fdwrite;
#endif
} g_job;

/* The status bar's compile slot - "" when idle. Kept as one helper so the
 * start/finish/rejected paths can't drift out of sync. */
static void compile_status_set(const char* text)
{
    if (g_statusbar_compile_item)
        ui_set_label(g_statusbar_compile_item, text);
}

static void compile_text_append(const char* data, size_t n)
{
    if (g_job.len + n + 1 > COMPILE_STREAM_MAX_BYTES)
        n = g_job.len < COMPILE_STREAM_MAX_BYTES ? COMPILE_STREAM_MAX_BYTES - g_job.len - 1 : 0;
    if (n == 0)
        return;

    if (g_job.len + n + 1 > g_job.cap)
    {
        size_t newcap = g_job.cap ? g_job.cap * 2 : 65536;
        while (newcap < g_job.len + n + 1)
            newcap *= 2;
        char* p = realloc(g_job.text, newcap);
        if (!p)
            return;  /* keep what we have rather than losing the run */
        g_job.text = p;
        g_job.cap = newcap;
    }
    memcpy(g_job.text + g_job.len, data, n);
    g_job.len += n;
    g_job.text[g_job.len] = 0;

    for (size_t i = 0; i < n; i++)
        if (data[i] == '\n')
            g_job.lines++;
}

/* Worker thread body: nothing but the compile itself. It must touch NO ui_*
 * state - every UI update happens on the main thread in
 * compile_stream_poll() below, which is what keeps this safe without any
 * locking beyond the `finished` flag. */
static int compile_thread_main(void* param)
{
    (void)param;
    compile(g_job.argc, g_job.argv, &g_job.report);

    /* Flush only - deliberately no closing of the pipe's write end here.
     *
     * The reader does NOT depend on EOF: compile_stream_poll() finishes on
     * `finished` plus a drained pipe, precisely because the write end can't
     * be closed from here. stdout holds its own duplicate of it either way
     * (dup2'd in compile_stream_start), so closing our copy would not
     * produce EOF anyway - and on Windows there is no copy left to close:
     * _open_osfhandle() hands the HANDLE's ownership to the CRT descriptor,
     * so _close() there has already closed it. Doing it again threw
     * 0xC0000008 "invalid handle".
     *
     * The write end is released for real by compile_stream_end(), when it
     * restores the original stdout descriptor. */
    fflush(stdout);

    /* Written last: compile_stream_poll() treats this as "no more output is
     * coming", so everything above must already have happened. */
    g_job.finished = 1;
    return 0;
}

/* Redirects stdout onto a fresh pipe and starts the worker. Returns 0 if
 * anything failed, in which case nothing was redirected and the caller
 * should just not start a compile. */
static int compile_stream_start(void)
{
    g_job.len = 0;
    g_job.lines = 0;
    if (g_job.text)
        g_job.text[0] = 0;
    g_job.finished = 0;
    memset(&g_job.report, 0, sizeof g_job.report);

    /* See trap 1 above: give stdout a valid fd before dup2'ing onto it. */
    fflush(stdout);
#ifdef _WIN32
    freopen("NUL", "w", stdout);
#else
    freopen("/dev/null", "w", stdout);
#endif

    g_job.saved_stdout = ide_dup(ide_fileno(stdout));

#ifdef _WIN32
    SECURITY_ATTRIBUTES sa = { sizeof sa, NULL, TRUE };
    if (!CreatePipe(&g_job.hread, &g_job.hwrite, &sa, 1 << 20))
        return 0;

    int wfd = _open_osfhandle((intptr_t)g_job.hwrite, _O_WRONLY | _O_TEXT);
    if (wfd == -1)
    {
        CloseHandle(g_job.hread);
        CloseHandle(g_job.hwrite);
        return 0;
    }
    _dup2(wfd, _fileno(stdout));
    _close(wfd);
    /* _open_osfhandle() transferred the HANDLE to wfd, so the _close()
     * above closed hwrite along with it. stdout keeps its own duplicate
     * (made by _dup2), which is the one that matters; forget ours so
     * nothing tries to close it twice. */
    g_job.hwrite = NULL;
#else
    int fds[2];
    if (pipe(fds) != 0)
        return 0;
    g_job.fdread = fds[0];
    g_job.fdwrite = fds[1];
    fcntl(g_job.fdread, F_SETFL, O_NONBLOCK);
    dup2(g_job.fdwrite, fileno(stdout));
    /* stdout now holds its own duplicate of the write end, so drop ours -
     * otherwise every compile leaks a descriptor. Mirrors the Windows side,
     * where the CRT took ownership of the handle instead. */
    close(g_job.fdwrite);
    g_job.fdwrite = -1;
#endif

    /* See trap 2: unbuffered, or nothing streams. */
    setvbuf(stdout, NULL, _IONBF, 0);

    g_job.running = 1;

    if (thrd_create(&g_job.thread, compile_thread_main, NULL) != thrd_success)
    {
        g_job.running = 0;
        return 0;
    }
    return 1;
}

/* Puts stdout back the way it was and releases the pipe/thread handles. */
static void compile_stream_end(void)
{
    /* The worker has already set `finished`, so this never actually waits -
     * it just reaps the thread. */
    thrd_join(g_job.thread, NULL);

#ifdef _WIN32
    if (g_job.hread)
    {
        CloseHandle(g_job.hread);
        g_job.hread = NULL;
    }
#else
    if (g_job.fdread >= 0)
    {
        close(g_job.fdread);
        g_job.fdread = -1;
    }
#endif

    fflush(stdout);
    if (g_job.saved_stdout >= 0)
    {
        ide_dup2(g_job.saved_stdout, ide_fileno(stdout));
        ide_close(g_job.saved_stdout);
        g_job.saved_stdout = -1;
    }
    g_job.running = 0;
}

/* Reads whatever is sitting in the pipe right now, without ever blocking.
 * Returns the number of bytes appended. */
static size_t compile_stream_drain(void)
{
    char buf[8192];
    size_t total = 0;

#ifdef _WIN32
    for (;;)
    {
        DWORD avail = 0;
        if (!g_job.hread || !PeekNamedPipe(g_job.hread, NULL, 0, NULL, &avail, NULL))
            break;
        if (avail == 0)
            break;
        DWORD want = avail > sizeof buf ? (DWORD)sizeof buf : avail;
        DWORD got = 0;
        if (!ReadFile(g_job.hread, buf, want, &got, NULL) || got == 0)
            break;
        compile_text_append(buf, got);
        total += got;
    }
#else
    for (;;)
    {
        ssize_t got = read(g_job.fdread, buf, sizeof buf);
        if (got <= 0)
            break;
        compile_text_append(buf, (size_t)got);
        total += (size_t)got;
    }
#endif
    return total;
}

static void compile_finish(void);
static void exttool_finish(void);

/* Called once per frame from app_frame(). Cheap no-op when idle. */
static void compile_stream_poll(void)
{
    if (!g_job.running)
        return;

    /* External Tool: drain the child's pipe instead of the worker's. Same
     * shape otherwise - append, show, and finalize when the source ends. */
    if (g_job.proc)
    {
        char buf[8192];
        int total = 0;
        for (;;)
        {
            int n = ui_process_read(g_job.proc, buf, (int)sizeof buf);
            if (n > 0)
            {
                compile_text_append(buf, (size_t)n);
                total += n;
                continue;
            }
            if (n < 0)
            {
                if (total > 0)
                {
                    ui_set_value(g_output_editor, g_job.text ? g_job.text : "");
                    ui_editor_goto_line(g_output_editor, g_job.lines + 1);
                }
                exttool_finish();
                return;
            }
            break;  /* nothing available this frame */
        }
        if (total > 0)
        {
            ui_set_value(g_output_editor, g_job.text ? g_job.text : "");
            ui_editor_goto_line(g_output_editor, g_job.lines + 1);
        }
        return;
    }

    size_t got = compile_stream_drain();
    if (got > 0)
    {
        /* Show it as it arrives - this is the whole point. */
        ui_set_value(g_output_editor, g_job.text ? g_job.text : "");
        ui_editor_goto_line(g_output_editor, g_job.lines + 1);  /* follow the tail */
    }

    /* Only finalize once the worker is done AND the pipe has run dry, so no
     * trailing output is dropped. */
    if (g_job.finished && got == 0)
    {
        compile_stream_drain();  /* last sip, post-EOF */
        compile_finish();
    }
}

/* An editor window's only child is its <window>, whose only child is its
 * <editor> - see make_editor_window(). Direct indexing rather than a
 * type-searching walk since we control that exact shape. */
static ui_node* editor_in_window(const ui_node* wrapper)
{
    if (!wrapper || ui_child_count(wrapper) == 0)
        return NULL;
    ui_node* window = ui_child_at(wrapper, 0);
    if (!window || ui_child_count(window) == 0)
        return NULL;
    return ui_child_at(window, 0);
}

/* Whether `wrapper` is one of our real document windows (make_editor_window)
 * as opposed to a docked Folder/Output panel or another modal - those also
 * have a <window> as their first child, so editor_in_window() alone can't
 * tell them apart (see g_active_editor_window above). Only document windows
 * carry a non-empty path (find_open_window relies on the same fact), so
 * that's the check. */
static int is_editor_window(const ui_node* wrapper)
{
    return wrapper && ui_get_path(wrapper)[0] != '\0';
}

/* The open editor window backed by `path`, or NULL. Compared case-
 * insensitively with '/' and '\' treated alike, so the same file reached by
 * either separator style matches. Only editor windows carry a real path (the
 * About/Directories/Output wrappers leave it ""), so a non-empty path can
 * only match one of them. */
static ui_node* find_open_window(const char* path)
{
    for (int i = 0; i < ui_child_count(g_root); i++)
    {
        ui_node* w = ui_child_at(g_root, i);
        if (!editor_in_window(w))
            continue;
        const char* a = ui_get_path(w);
        const char* b = path;
        while (*a && *b)
        {
            char ca = *a == '\\' ? '/' : (char)tolower((unsigned char)*a);
            char cb = *b == '\\' ? '/' : (char)tolower((unsigned char)*b);
            if (ca != cb)
                break;
            a++;
            b++;
        }
        if (!*a && !*b)
            return w;
    }
    return NULL;
}

/* Writes the active window's current editor content to the file it was
 * opened from/named after (ui_set_path, set in make_editor_window) - F7 must
 * compile what's on screen, not stale content already on disk. No-op if
 * there's no active window or it has no path (shouldn't happen - every
 * editor window gets one, real or a NONAMEnn.C placeholder). */
/* Writes `content` to `f`, optionally expanding '\n' to "\r\n" so saved
 * files keep whichever line ending is asked for; written verbatim otherwise. */
static void fwrite_text(const char* content, size_t len, FILE* f, int crlf)
{
    if (crlf)
    {
        /* content is expected to be LF-only already, but a stray leftover
         * '\r' (e.g. from a normalization gap upstream) must never survive
         * here - writing it verbatim next to the '\r' this loop adds for
         * '\n' is exactly how a single CRLF turns into "\r\r\n" on disk. */
        for (size_t i = 0; i < len; i++)
        {
            if (content[i] == '\r')
                continue;
            if (content[i] == '\n')
                fputc('\r', f);
            fputc(content[i], f);
        }
    }
    else
    {
        for (size_t i = 0; i < len; i++)
        {
            if (content[i] != '\r')
                fputc(content[i], f);
        }
    }
}

/* Whether `path`'s existing content (if any) uses CRLF line endings, checked
 * by peeking at the bytes before the first '\n'. Used so Save keeps a file's
 * original line-ending style instead of silently converting it - important
 * when the same file is edited from both Windows and Linux (see
 * normalize_newlines): always forcing the platform's native ending here would
 * flip every line of a Linux-authored file to CRLF the first time it's saved
 * from Windows, and vice versa. No existing file (a brand-new save) falls
 * back to the platform's native ending. */
static int file_uses_crlf(const char* path)
{
    FILE* f = fopen(path, "rb");
    if (!f)
#ifdef _WIN32
        return 1;
#else
        return 0;
#endif
    int prev = 0, c, saw_nl = 0, crlf = 0;
    while ((c = fgetc(f)) != EOF)
    {
        if (c == '\n')
        {
            saw_nl = 1;
            crlf = (prev == '\r');
            break;
        }
        prev = c;
    }
    fclose(f);
    if (!saw_nl)
#ifdef _WIN32
        return 1;
#else
        return 0;
#endif
    return crlf;
}

static void save_active_file(ui_node* active)
{
    ui_node* editor = editor_in_window(active);
    const char* path = active ? ui_get_path(active) : "";
    if (!editor || !path[0])
        return;

    int crlf = file_uses_crlf(path);

    FILE* f = fopen(path, "wb");
    if (!f)
    {
        /* e.g. a read-only file or a folder the user can't write to - say
         * so instead of silently leaving the window dirty and the file
         * untouched. */
        ui_msgbox_button ok = { "   OK   ", 0 };
        char message[200];
        snprintf(message, sizeof message, "Cannot save file:\n%s\n\n%s",
                 path, strerror(errno));
        ui_message_box(g_screen, " Error ", message, &ok, 1);
        return;
    }
    const char* content = ui_get_value(editor);
    fwrite_text(content, strlen(content), f, crlf);
    int write_failed = ferror(f);
    if (fclose(f) != 0)
        write_failed = 1;
    if (write_failed)
    {
        /* Opened fine but the write didn't land (disk full, device gone) -
         * keep the window dirty so the content isn't mistaken for saved. */
        ui_msgbox_button ok = { "   OK   ", 0 };
        char message[200];
        snprintf(message, sizeof message, "Error writing file:\n%s\n\n%s",
                 path, strerror(errno));
        ui_message_box(g_screen, " Error ", message, &ok, 1);
        return;
    }
    ui_set_dirty(editor, 0);
    ui_set_file_time(active, file_mtime(path));  /* our own write isn't an
                                                  * outside change */
}

/* Point the active editor window at g_saveas_path, retitle it, write it out,
 * and close the Save As dialog. Called once the target is settled - either
 * straight away (new file) or after the overwrite prompt is confirmed. */
static void save_as_commit(void)
{
    ui_set_path(g_save_window, g_saveas_path);

    /* Retitle the window (the title lives on the <window> child; the wrapper
     * only carries the path). */
    ui_node* win = ui_child_at(g_save_window, 0);
    if (win)
    {
        char title[320];
        snprintf(title, sizeof title, " %s ", g_save_name);
        ui_set_label(win, title);
    }

    /* Re-derive the syntax mode from the new name - e.g. saving NONAME00.C
     * as notes.md should switch it from C to Markdown highlighting. */
    ui_node* editor = editor_in_window(g_save_window);
    if (editor)
        ui_set_syntax(editor, syntax_for_path(g_saveas_path));

    save_active_file(g_save_window);  /* writes content, clears dirty */
    ui_set_untitled(g_save_window, 0);  /* it's a real, named file now */

    ui_screen_close_modal(g_screen, g_open.modal);
    g_open.dialog_mode = OPEN_DLG_FILE;

    /* Resume a Compile that was waiting on this exact Save As (see
     * do_compile's untitled-file check) - the file now has the real path a
     * compile needs, so pick up right where that request left off. */
    if (g_pending_compile_after_saveas)
    {
        g_pending_compile_after_saveas = 0;
        do_compile();
    }
}

/* The current "Files of type" filter's extension, including the leading
 * dot (".md"), if it's a single plain one - i.e. not a multi-pattern mask
 * like "*.c;*.h" and not the "*" catch-all. NULL otherwise. Used by
 * save_as_activate to auto-append an extension when the typed name has
 * none, the way native Windows Save dialogs do. */
static const char* current_filter_extension(void)
{
    int idx = ui_select_get_selected(g_open.filter);
    if (idx < 0 || idx >= OPEN_FILTER_COUNT)
        return NULL;
    const char* mask = g_open_filters[idx].mask;
    if (mask[0] != '*' || mask[1] != '.' || strchr(mask, ';') || strchr(mask + 1, '*'))
        return NULL;
    return mask + 1;  /* skip the leading '*', keep the '.' */
}

/* File > Save As... confirmation (Save button, Enter in the Name field, or a
 * file row picked into it). Parses the Name field into directory + filename,
 * then either writes straight away or - if the file exists - asks first. */
static void save_as_activate(void)
{
    if (!g_save_window || !editor_in_window(g_save_window))
    {
        ui_screen_close_modal(g_screen, g_open.modal);
        g_open.dialog_mode = OPEN_DLG_FILE;
        return;
    }

    /* Split at the last separator (either style) into directory + filename,
     * same as the Open dialog's Name field. The field itself always shows
     * backslashes (see open_dialog_refresh's "cosmetic display only"), so
     * normalize back to '/' first - '/' is a valid separator on every
     * platform this app runs on (Windows included), while a literal '\\'
     * is not on Linux/macOS: left as typed, an unedited default directory
     * like "\home\proton\project" doesn't exist as a real path there, so
     * the fopen() in save_active_file silently fails and nothing gets
     * written, even though the window's title/path already updated. */
    char buf[1024];
    strncpy(buf, ui_get_value(g_open.name_input), sizeof buf - 1);
    buf[sizeof buf - 1] = 0;
    for (char* p = buf; *p; p++)
        if (*p == '\\')
            *p = '/';

    char* sep = NULL;
    for (char* p = buf; *p; p++)
        if (*p == '/')
            sep = p;

    const char* name = buf;
    if (sep)
    {
        *sep = 0;
        name = sep + 1;
        if (buf[0])
        {
            strncpy(g_open.dir, buf, sizeof g_open.dir - 1);
            g_open.dir[sizeof g_open.dir - 1] = 0;
        }
    }
    if (!name[0])
        return;  /* no filename typed - leave the dialog open */

    /* Windows Save dialogs append the selected filter's extension when you
     * type a bare name with none - e.g. typing "notes" with "Markdown Files
     * (*.md)" selected saves "notes.md". Only kicks in for a single plain
     * extension (see current_filter_extension) and only when the typed name
     * has no '.' of its own - an explicit extension always wins. */
    char name_buf[300];
    strncpy(name_buf, name, sizeof name_buf - 1);
    name_buf[sizeof name_buf - 1] = 0;
    if (!strchr(name_buf, '.'))
    {
        const char* ext = current_filter_extension();
        if (ext && strlen(name_buf) + strlen(ext) < sizeof name_buf)
            strcat(name_buf, ext);
    }
    name = name_buf;

    strncpy(g_save_name, name, sizeof g_save_name - 1);
    g_save_name[sizeof g_save_name - 1] = 0;

    path_join(g_saveas_path, sizeof g_saveas_path, g_open.dir, g_save_name);

    /* If the target already exists, confirm the overwrite first (the Save As
     * dialog stays open behind the prompt so "No" returns to it). */
    FILE* exists = fopen(g_saveas_path, "rb");
    if (exists)
    {
        fclose(exists);
        char msg[400];
        snprintf(msg, sizeof msg, "%s already exists.\nOverwrite?", g_save_name);
        ui_msgbox_button btns[] = {
            { "  Yes  ", EVT_SAVEAS_OVERWRITE },
            { "  No  ", 0 },
        };
        ui_message_box(g_screen, "Save As", msg, btns, 2);
        return;
    }

    save_as_commit();
}

/* Opens the Save As dialog for `win` - the guts of EVT_FILE_SAVEAS, pulled
 * out so File > Save (see EVT_FILE_SAVE) and Compile (see do_compile) can
 * both reuse it for a still-untitled window (see ui_get_untitled): a normal
 * numbered-placeholder path like "NONAME00.C" is never something to silently
 * write to or hand the compiler - it has to become a real file first, same
 * as a "normal text editor". No-op if `win` isn't a real document window. */
static void open_saveas_dialog_for(ui_node* win)
{
    if (!win || !editor_in_window(win))
        return;

    g_save_window = win;
    g_open.dialog_mode = OPEN_DLG_SAVE;

    /* Default the Name to the window's current file, and start browsing in
     * that file's own directory when it has one. */
    const char* cur = ui_get_path(win);
    const char* base = basename_of(cur);
    strncpy(g_save_name, base, sizeof g_save_name - 1);
    g_save_name[sizeof g_save_name - 1] = 0;

    if (base > cur)
    {
        size_t dlen = (size_t)(base - cur - 1);
        if (dlen > 0 && dlen < sizeof g_open.dir)
        {
            memcpy(g_open.dir, cur, dlen);
            g_open.dir[dlen] = 0;
        }
        else if (!ui_get_cwd(g_open.dir, sizeof g_open.dir))
            strcpy(g_open.dir, ".");
    }
    else if (!ui_get_cwd(g_open.dir, sizeof g_open.dir))
    {
        strcpy(g_open.dir, ".");
    }
    strcpy(g_open.mask, g_open_filters[0].mask);
    ui_select_set_selected(g_open.filter, 0);
    open_dialog_set_filter_visible(1);

    ui_set_label(g_open.window, " Save File As ");
    ui_set_label(g_open.ok, "  Save  ");
    open_dialog_refresh();
    ui_screen_show_modal(g_screen, g_open.modal);
}

/* Strips embedded VT100 SGR escape sequences ("\x1b[...m") from `s`, in
 * place. The Output window renders those directly (see UI_SYNTAX_VT100), but
 * a diagnostic's inline overlay in the source editor (see ui.c's
 * render_diagnostic) doesn't interpret them - without this, raw escape
 * bytes from a colorized compiler message would show up as visible junk
 * there. */
static void strip_ansi_sgr(char* s)
{
    char* w = s;
    for (char* r = s; *r; )
    {
        if (r[0] == '\x1b' && r[1] == '[')
        {
            char* p = r + 2;
            while (*p && *p != 'm' && (*p < 0x40 || *p > 0x7E))
                p++;
            if (*p)
            {
                r = p + 1;  /* skip the whole "\x1b[...m" (or other CSI) */
                continue;
            }
        }
        *w++ = *r++;
    }
    *w = 0;
}
/* Helper: case‑insensitive strstr */
static const char* ci_strstr(const char* haystack, const char* needle)
{
    if (!*needle) return haystack;
    for (; *haystack; haystack++)
    {
        const char* h = haystack, * n = needle;
        while (*h && *n && tolower((unsigned char)*h) == tolower((unsigned char)*n))
        {
            h++; n++;
        }
        if (!*n) return haystack;
    }
    return NULL;
}

/* The filename part of a path (after the last '/' or '\'). */
static const char* basename_of(const char* p)
{
    const char* b = p;
    for (const char* q = p; *q; q++)
        if (*q == '/' || *q == '\\')
            b = q + 1;
    return b;
}

/* Case-insensitive comparison of two paths by basename (Windows filenames
 * aren't case-sensitive, and the output line may name a file with no path
 * while the window stores one, or vice versa). */
static int paths_match(const char* a, const char* b)
{
    a = basename_of(a);
    b = basename_of(b);
    while (*a && *b && tolower((unsigned char)*a) == tolower((unsigned char)*b))
    {
        a++; b++;
    }
    return *a == *b;
}

/* Whether `path` names a file that can currently be opened for reading - a
 * quick existence probe (fopen+fclose, no content read) used by
 * resolve_referenced_path() below to try several candidate paths without
 * flashing a "File not found" message box for each wrong guess - only the
 * one it finally commits to (or the plain fallback) goes through
 * open_file_path_into_editor()'s own not-found handling. */
static int file_readable(const char* path)
{
    FILE* f = fopen(path, "rb");
    if (!f)
        return 0;
    fclose(f);
    return 1;
}

/* Resolves a bare `filename` (or relative path) with no reliable directory
 * of its own - e.g. a #include target or a filename typed in a comment/doc
 * (do_editor_ctrlclick), a Tools > Find and Replace "Current Dir" result,
 * which only ever prints bare names (see fr_search_text), or a "Project"/
 * F12 result (fr_search_project/do_goto_definition), which prints paths
 * relative to the open project's own directory (g_project.files[] - see
 * g_project's own doc comment) - against the directories it most plausibly
 * came from: the open project's directory, the active document's own
 * folder, then the persistent Folder panel's browsed directory, before
 * falling back to `filename` as-is (already a real openable path for e.g. a
 * compiler diagnostic, which carries a real path). Writes the resolved path
 * into `out` (up to out_cap bytes, always 0-terminated). */
static void resolve_referenced_path(const char* filename, char* out, size_t out_cap)
{
    char candidate[1024];
    const char* open_path = NULL;

    if (project_is_open())
    {
        path_join(candidate, sizeof candidate, g_project.dir, filename);
        if (file_readable(candidate))
            open_path = candidate;
    }
    if (!open_path && g_active_editor_window)
    {
        char dir[1024];
        strncpy(dir, ui_get_path(g_active_editor_window), sizeof dir - 1);
        dir[sizeof dir - 1] = 0;
        dirname(dir);
        path_join(candidate, sizeof candidate, dir, filename);
        if (file_readable(candidate))
            open_path = candidate;
    }
    if (!open_path && g_folder.dir[0])
    {
        path_join(candidate, sizeof candidate, g_folder.dir, filename);
        if (file_readable(candidate))
            open_path = candidate;
    }
    if (!open_path)
        open_path = filename;

    strncpy(out, open_path, out_cap - 1);
    out[out_cap - 1] = 0;
}

/* Ctrl+click on a bare ".c"/".h"/".md" reference (do_editor_ctrlclick, via
 * filename_at_cursor): resolves it with resolve_referenced_path() and opens
 * it into its own window - unlike a Markdown link (open_link_in_window),
 * this never touches the window that was clicked in, same as File > Open. */
static void open_referenced_file(const char* filename)
{
    char open_path[1024];
    resolve_referenced_path(filename, open_path, sizeof open_path);
    nav_record_jump();
    open_file_path_into_editor(open_path, basename_of(open_path));
}

/* Double-click on an Output line that is not a diagnostic, such as a "dir",
 * "ls" or "git status" row: the file or directory name is at the end of the
 * line, after columns of other text, and may contain spaces - so try each
 * suffix that starts after a blank, longest first, against the command
 * line's directory. A file opens in an editor; a directory becomes the
 * Folder panel's (and the command line's) directory, like "cd". */
static void output_open_listed_path(const char* line)
{
    char dir[FS_MAX_PATH];
    if (g_folder.dir[0])
        snprintf(dir, sizeof dir, "%s", g_folder.dir);
    else if (!ui_get_cwd(dir, sizeof dir))
        return;

    for (const char* p = line; *p; p++)
    {
        if (p != line && !(p[-1] == ' ' || p[-1] == '\t'))
            continue;
        if (*p == ' ' || *p == '\t')
            continue;

        char name[FS_MAX_PATH];
        snprintf(name, sizeof name, "%s", p);
        size_t n = strlen(name);
        while (n > 0 && (name[n - 1] == ' ' || name[n - 1] == '\t' || name[n - 1] == '\r'))
            name[--n] = 0;
        if (strcmp(name, ".") == 0)
            continue;

        char path[FS_MAX_PATH];
        if (name[0] == '/' || name[0] == '\\' || (isalpha((unsigned char)name[0]) && name[1] == ':'))
        {
            snprintf(path, sizeof path, "%s", name);
            ide_path_normalize(path);
        }
        else
            path_join(path, sizeof path, dir, name);

        struct stat st;
        if (stat(path, &st) != 0)
            continue;
        if (st.st_mode & S_IFDIR)
            folder_reveal_directory(path);
        else
        {
            nav_record_jump();
            open_file_path_into_editor(path, basename_of(path));
        }
        return;
    }
}

/* Double-click in the Output window: parse the clicked line's
 * "filename:line:col ..." compiler-diagnostic reference and jump the matching
 * editor window to that source line. The Output editor is VT100-mode, so
 * the line carries VT100 SGR escapes that must be stripped before parsing; the
 * filename is matched against each editor window's path (see ui_set_path). */
static void output_goto_source(void)
{
    int row = ui_editor_caret_line(g_output_editor);  /* 1-based clicked line */
    if (row < 1)
        return;

    /* Copy just the clicked line out of the full output text. */
    const char* text = ui_get_value(g_output_editor);
    const char* ls = text;
    for (int i = 1; i < row && *ls; ls++)
        if (*ls == '\n')
            i++;
    const char* le = ls;
    while (*le && *le != '\n')
        le++;

    char line[1024];
    int len = (int)(le - ls);
    if (len >= (int)sizeof line)
        len = (int)sizeof line - 1;
    memcpy(line, ls, (size_t)len);
    line[len] = '\0';

    strip_ansi_sgr(line);

    /* Both diagnostic formats are accepted here (Compile > Options... picks
     * which one the compiler prints - see g_diagformat_slugs), so a line
     * pasted from either still jumps:
     *
     *   file.c:1:2: ...   gcc/ide  - the filename ends at the first ':'
     *                     followed by a digit, so a Windows drive letter
     *                     ("C:") isn't mistaken for the separator
     *   file.c(1,2): ...  msvc     - the filename ends at the '(' of a
     *                     "(<digits>,<digits>)" group
     */
    char* sep = NULL;
    int src_line = 0;
    for (char* p = line; *p; p++)
    {
        if (*p == ':' && isdigit((unsigned char)p[1]))
        {
            sep = p;
            src_line = atoi(p + 1);
            break;
        }

        if (*p == '(' && isdigit((unsigned char)p[1]))
        {
            char* q = p + 1;
            while (isdigit((unsigned char)*q))
                q++;
            if (*q != ',')
                continue;  /* not a position group - keep looking */
            sep = p;
            src_line = atoi(p + 1);
            break;
        }
    }
    if (!sep)
    {
        output_open_listed_path(line);
        return;
    }
    *sep = '\0';
    if (src_line < 1)
        return;

    const char* filename = line;
    while (*filename == ' ' || *filename == '\t')
        filename++;

    /* Find the editor window whose path matches that filename. */
    ui_node* target = NULL;
    for (int i = 0; i < ui_child_count(g_root); i++)
    {
        ui_node* w = ui_child_at(g_root, i);
        const char* path = ui_get_path(w);
        if (editor_in_window(w) && path[0] && paths_match(path, filename))
        {
            target = w;
            break;
        }
    }

    if (!target)
    {
        /* Not already open - e.g. a Tools > Find and Replace "Current Dir"
         * result (see fr_search_dir), which searches files the user may
         * never have opened and only ever prints a bare name for (see
         * fr_search_text) - resolve_referenced_path() tries the directories
         * such a result would plausibly have come from before falling back
         * to `filename` as-is, which is already a real openable path for a
         * compiler diagnostic (do_compile gives the compiler the active
         * file's full path, not just its name). */
        char open_path[1024];
        resolve_referenced_path(filename, open_path, sizeof open_path);

        nav_record_jump();
        open_file_path_into_editor(open_path, basename_of(open_path));
        target = find_open_window(open_path);
        if (!target)
            return;  /* open_file_path_into_editor() already reported "not found" */
    }
    else
    {
        nav_record_jump();
    }

    ui_screen_show_window(g_screen, target);  /* bring the source to the front */
    ui_node* editor = editor_in_window(target);
    ui_editor_goto_line(editor, src_line);
    g_goto_pending_focus = editor;  /* focus after this update finishes - see app_frame */
}

static int parse_diagnostic_line(char* line, char** out_file, ui_diag_type* type, int* out_line,
                                 int* out_code, char** message)
{
    strip_ansi_sgr(line);

    // Recognized severity keywords: "error", "warning", "info", and "note" (mapped to INFO)
    const char* keywords[] = { "error", "warning", "info", "note" };
    ui_diag_type types[] = { UI_DIAG_ERROR, UI_DIAG_WARNING, UI_DIAG_INFO, UI_DIAG_INFO };
    int found_type = -1;
    const char* kw_pos = NULL;

    // 1. Find the first occurrence of any keyword as a whole word
    for (int i = 0; i < 4; i++)
    {
        const char* p = line;
        while (1)
        {
            const char* pos = ci_strstr(p, keywords[i]);
            if (!pos) break;
            size_t kwlen = strlen(keywords[i]);
            // Check word boundaries: preceding and following chars not alnum or '_'
            int prev_ok = (pos == line || (!isalnum((unsigned char)pos[-1]) && pos[-1] != '_'));
            int next_ok = (!isalnum((unsigned char)pos[kwlen]) && pos[kwlen] != '_');
            if (prev_ok && next_ok)
            {
                found_type = i;
                kw_pos = pos;
                break;
            }
            p = pos + 1; // continue after this false match
        }
        if (found_type != -1) break;
    }

    if (found_type == -1 || !kw_pos)
        return 0;

    *type = types[found_type];

    // 2. Extract source line number from the "file:line:col" (gcc/ide) or
    // "file(line,col)" (msvc) prefix - both formats are accepted, same as
    // output_goto_source(). For the first, find the first colon followed by a
    // digit (skips drive letters); for the second, the '(' of the
    // "(<digits>,<digits>)" group.
    char* p = line;
    const char* line_start = NULL;
    while (*p)
    {
        if (*p == ':' && isdigit((unsigned char)*(p + 1)))
        {
            line_start = p + 1; // point to the first digit of the line number
            break;
        }

        if (*p == '(' && isdigit((unsigned char)*(p + 1)))
        {
            const char* q = p + 1;
            while (isdigit((unsigned char)*q))
                q++;
            if (*q == ',')
            {
                line_start = p + 1;
                break;
            }
        }
        p++;
    }
    if (!line_start)
        return 0;

    int line_num = 0;
    while (isdigit((unsigned char)*line_start))
    {
        line_num = line_num * 10 + (*line_start - '0');
        line_start++;
    }
    *out_line = line_num;

    // 3. Extract the diagnostic message after the severity keyword
    // Skip keyword itself, then any whitespace, then the optional diagnostic
    // number (captured, not discarded - it's shown inline next to the severity
    // tag), then whitespace and a colon, then the actual message.
    const char* msg_start = kw_pos + strlen(keywords[found_type]);
    while (*msg_start == ' ' || *msg_start == '\t') msg_start++;
    int code = 0;
    while (isdigit((unsigned char)*msg_start))  // nonexistent for "note"
    {
        code = code * 10 + (*msg_start - '0');
        msg_start++;
    }
    *out_code = code;
    while (*msg_start == ' ' || *msg_start == '\t') msg_start++;
    if (*msg_start == ':') msg_start++;  // skip colon
    while (*msg_start == ' ' || *msg_start == '\t') msg_start++;

    // Trim trailing newline/spaces
    char* msg = (char*)msg_start;
    char* end = msg + strlen(msg);
    while (end > msg && (end[-1] == '\n' || end[-1] == '\r' || end[-1] == ' ' || end[-1] == '\t'))
        end--;
    *end = '\0';

    *message = msg;

    /* p is the separator before the line number; cut the filename there */
    *p = '\0';
    char* file = line;
    while (*file == ' ' || *file == '\t')
        file++;
    *out_file = file;
    return 1;
}

/* Clears every editor window's diagnostics, then adds each diagnostic line of
 * `text` to the window of the file it names, so a project build marks every
 * open file and not only the active one. Mutates `text` (strtok). */
static void apply_diagnostics(char* text)
{
    for (int i = 0; i < ui_child_count(g_root); i++)
    {
        ui_node* editor = editor_in_window(ui_child_at(g_root, i));
        if (editor)
            ui_editor_clear_diagnostics(editor);
    }

    for (char* line = strtok(text, "\n"); line; line = strtok(NULL, "\n"))
    {
        char* file;
        ui_diag_type type;
        int diag_line;
        int diag_code;
        char* message;
        if (!parse_diagnostic_line(line, &file, &type, &diag_line, &diag_code, &message))
            continue;

        for (int i = 0; i < ui_child_count(g_root); i++)
        {
            ui_node* w = ui_child_at(g_root, i);
            ui_node* editor = editor_in_window(w);
            const char* path = ui_get_path(w);
            if (editor && path[0] && paths_match(path, file))
            {
                ui_editor_add_diagnostic(editor, type, diag_line, diag_code, message);
                break;
            }
        }
    }
}

/* Reload every open window's content from its file on disk, skipping any with
 * unsaved changes (dirty) or no backing file (e.g. the Output window). Used by
 * the Refresh command and run automatically after a compile, so windows pick
 * up any on-disk changes a build produced. */
static void refresh_open_windows(void)
{
    for (int i = 0; i < ui_screen_window_count(g_screen); i++)
    {
        ui_node* wrapper = ui_screen_window_at(g_screen, i);
        ui_node* ed = editor_in_window(wrapper);
        if (!ed || ui_get_dirty(ed))
            continue;  /* leave windows with unsaved edits alone */
        const char* path = ui_get_path(wrapper);
        if (!path[0])
            continue;

        FILE* f = fopen(path, "rb");
        if (!f)
            continue;
        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        fseek(f, 0, SEEK_SET);
        if (size < 0)
            size = 0;
        char* content = malloc((size_t)size + 1);
        if (content)
        {
            size_t got = fread(content, 1, (size_t)size, f);
            content[got] = 0;
            /* Nothing actually changed on disk - most often this *is* the
             * file we just wrote out ourselves (save_active_file, called by
             * do_compile right before this). Reloading it would be a no-op
             * visually but ui_set_value() throws the editor's undo/redo
             * history away, so the user would silently lose Ctrl+Z after
             * every save/compile. Only reload when the bytes really differ. */
            if (strcmp(content, ui_get_value(ed)) == 0)
            {
                free(content);
                fclose(f);
                continue;
            }
            int cur = ui_editor_get_cursor(ed);      /* keep the caret put */
            int scroll = ui_editor_get_scroll(ed);   /* and the scroll position */
            ui_set_value(ed, content);  /* (ui_set_value moves the caret to the
                                          * end and resets scroll to the top) */
            ui_editor_set_selection(ed, cur, cur);
            ui_editor_set_scroll(ed, scroll);
            ui_set_dirty(ed, 0);  /* freshly loaded from disk = clean */
            free(content);
        }
        fclose(f);
    }
}

/* Run > Compile (EVT_COMPILE) / F7 - the test integration point for the
 * real compiler, linked in separately. Compiles whichever document window
 * is currently active - g_active_editor_window, not ui_screen_top_window()
 * (see g_active_editor_window's own doc comment): top_window() also returns
 * the docked Folder/Output panel whenever one of those is frontmost instead,
 * and neither carries a real path, so compiling "whatever's on top" without
 * this distinction would silently hand the compiler an empty path the moment
 * Output (e.g. from a previous compile) or Folder happened to be the last
 * window clicked - reachable from the menu (a mouse trip up to Compile
 * doesn't require clicking back into the document first) even though F7
 * usually doesn't hit it, since typing right before pressing F7 naturally
 * keeps the real document on top. Save/Save As already got this same fix -
 * Compile just never had it applied. */

/* Unified Build action: with a project open, build the entire project
 * (whatever file happens to be frontmost - or none at all) unless the
 * Playground is the active window; without one,
 * Build and Compile mean the same thing and just compile the active file
 * using the IDE-wide settings. External tools' $(CakeOutput) and $(Target)
 * macros expand correctly in both cases: project context uses all files and
 * project target, standalone context uses just the active file and IDE-wide
 * target settings. The per-frame enable in app_frame() mirrors this split. */
static int active_is_playground(void)
{
    char playground_path[FS_MAX_PATH];
    return g_active_editor_window &&
           get_playground_file_path(playground_path, sizeof playground_path) &&
           strcmp(ui_get_path(g_active_editor_window), playground_path) == 0;
}

static void do_build(void)
{
    /* The Playground is independent of any project: with it active, Build
     * compiles just playground.c with the IDE-wide settings. */
    if (project_is_open() && !active_is_playground())
    {
        do_project_build();
    }
    else
    {
        do_compile();
    }
}

/* Fills g_job.argv/storage with "tcc" plus everything Compile > Options...
 * says for `cs` - the Output Format/Target/Style rows, each "[x] -flag"
 * toggle, the free-text tokens (split on whitespace) and, last, the include
 * directories as -I flags - and returns how many entries that is, so the
 * caller appends the file(s) from there. The one place a checkbox is turned
 * into its flag, shared by do_compile() and do_project_build().
 *
 * The directories come from whichever list owns `cs`: a project's own,
 * resolved to absolute paths (its entries are relative to the project
 * directory), or the global list, already absolute. Written straight into
 * g_job.storage, which outlives the calling frame - the compile runs on a
 * worker thread after the caller returns, through job_push(), which drops
 * excess tokens and always leaves one slot free for the caller's file. */
static void job_push(int* argc, const char* token)
{
    /* one slot is always left free for the caller's file */
    if (*argc + 1 < (int)_Countof(g_job.storage))
    {
        snprintf(g_job.storage[*argc], sizeof g_job.storage[0], "%s", token);
        g_job.argv[*argc] = g_job.storage[*argc];
        (*argc)++;
    }
}

static int job_argv_from_settings(const compile_settings* cs)
{
    int argc = 0;
    char flag[1024];

    job_push(&argc, "tcc");
    if (cs->diagnostic_format && cs->diagnostic_format[0])
    {
        snprintf(flag, sizeof flag, "-fdiagnostics-format=%s", cs->diagnostic_format);
        job_push(&argc, flag);
    }
    if (cs->target[0])
    {
        snprintf(flag, sizeof flag, "-target=%s", cs->target);
        job_push(&argc, flag);
    }
    if (cs->style[0])
    {
        snprintf(flag, sizeof flag, "-style=%s", cs->style);
        job_push(&argc, flag);
    }
    if (cs->no_output)
    {
        job_push(&argc, "-no-output");
    }
    if (cs->line_directives)
    {
        job_push(&argc, "-line-directives");
    }
    if (cs->fanalyzer)
    {
        job_push(&argc, "-fanalyzer");
    }
    if (cs->const_literal)
    {
        job_push(&argc, "-const-literal");
    }
    if (cs->wall)
    {
        job_push(&argc, "-Wall");
    }
    if (cs->unused_extern_report)
    {
        job_push(&argc, "-unused-extern-report");
    }
    if (cs->use_cake_headers)
    {
        job_push(&argc, "-cake-headers");
    }

    char optbuf[sizeof cs->options];
    snprintf(optbuf, sizeof optbuf, "%s", cs->options);
    for (char* tok = strtok(optbuf, " \t"); tok; tok = strtok(NULL, " \t"))
    {
        job_push(&argc, tok);
    }

    /* The global list (g_include_dirs) is not passed as -I: the compiler
     * already reads it from cake.json (preprocessor_load_config), and
     * passing it here too would add every directory twice. */
    if (cs == &g_project.compile)
    {
        for (int i = 0; i < g_project.include_count; i++)
        {
            char abs_dir[1024 - 2];
            project_abs_path(g_project.include_dirs[i], abs_dir, sizeof abs_dir);
            snprintf(flag, sizeof flag, "-I%s", abs_dir);
            job_push(&argc, flag);
        }
    }
    return argc;
}

static void do_compile(void)
{
    /* One compile at a time. Without this, a second F7 landing mid-build
     * would redirect stdout again underneath the running worker and race it
     * for g_job. Silently ignored: the status bar already reads
     * "Compiling...", which is answer enough. */
    if (g_job.running)
        return;

    ui_node* active = g_active_editor_window;

    /* Still untitled (see ui_get_untitled) - e.g. Run > Compile straight from
     * File > New, never saved - there's no real path yet to hand the
     * compiler, so ask for one first via the same Save As dialog Ctrl+Shift+S
     * opens, then resume this exact compile once it's actually saved (see
     * save_as_commit). Canceling that dialog aborts the compile instead of
     * going ahead against a placeholder path (see EVT_OPEN_CANCEL). */
    if (active && ui_get_untitled(active))
    {
        g_pending_compile_after_saveas = 1;
        open_saveas_dialog_for(active);
        return;
    }

    save_active_file(active);
    const char* file = active ? ui_get_path(active) : "";

    struct report report;
    memset(&report, 0, sizeof report);

    /* The open project's own settings if `file` is one of its files, else
     * the IDE-wide default (see active_compile_settings()), then the file. */
    int argc = job_argv_from_settings(active_compile_settings(file));
    snprintf(g_job.storage[argc], sizeof g_job.storage[0], "%s", file);  /* job_push() always leaves this slot */
    g_job.argv[argc] = g_job.storage[argc];
    g_job.argc = argc + 1;
    g_job.active = active;

    ui_set_value(g_output_editor, "");
    ui_screen_show_window(g_screen, g_output_window);
    if (active)
    {
        ui_screen_show_window(g_screen, active);
        ui_node* editor = editor_in_window(active);
        if (editor)
            ui_screen_focus(g_screen, editor);
    }

    if (!compile_stream_start())
    {
        compile_status_set("");
        ui_set_value(g_output_editor, "Could not start the compile (pipe/thread creation failed).\n");
        return;
    }
    compile_status_set("Compiling...");
    /* Returns immediately now - compile_stream_poll(), called once per frame
     * from app_frame(), streams the output and calls compile_finish() when
     * the worker is done. */
}

/* Project > "Build" - same argv shape do_compile() builds from Compile >
 * Options... (g_compile: target/style/diagnostic format/flags/free-text
 * options), just with every ".c" file in the open project appended instead
 * of only the active document's own path. Handing the compiler more than one
 * translation unit at once - and whatever that takes from there (linking,
 * etc.) - is entirely its own job; the IDE's part ends at gathering the file
 * list and invoking it, same division of labor g_project's own doc comment
 * describes. No-op without an open project, or while a compile/build/
 * external tool is already running (shares g_job, same one-at-a-time rule as
 * do_compile()). */
static void do_project_build(void)
{
    if (g_job.running || !project_is_open())
        return;

    /* Save every project file that's currently open in an editor first - the
     * compiler reads from disk, so a stale copy would silently build the
     * last-saved version instead of what's on screen (do_compile()'s own
     * save_active_file() call, just for every project file instead of only
     * the active one). */
    for (int i = 0; i < g_project.file_count; i++)
    {
        char abs_path[1024];
        project_abs_path(g_project.files[i], abs_path, sizeof abs_path);
        ui_node* open_win = find_open_window(abs_path);
        if (open_win)
            save_active_file(open_win);
    }

    /* Always this project's own settings, never the IDE-wide g_compile -
     * do_project_build() only ever runs while a project is open (checked
     * above). Then every ".c" file in the project, resolved to an absolute
     * path - headers are never handed to the compiler directly, same as a
     * normal single-file Compile never would be pointed at a .h. */
    int argc = job_argv_from_settings(&g_project.compile);
    int file_argc = 0;
    for (int i = 0; i < g_project.file_count && argc < (int)_Countof(g_job.storage); i++)
    {
        const char* entry = g_project.files[i];
        size_t len = strlen(entry);
        if (len < 2 || entry[len - 2] != '.' || entry[len - 1] != 'c')
        {
            continue;
        }
        project_abs_path(entry, g_job.storage[argc], sizeof g_job.storage[0]);
        g_job.argv[argc] = g_job.storage[argc];
        argc++;
        file_argc++;
    }

    if (file_argc == 0)
    {
        ui_set_value(g_output_editor, "The open project has no .c files to build.\n");
        ui_screen_show_window(g_screen, g_output_window);
        return;
    }
    g_job.argc = argc;
    g_job.active = g_active_editor_window;

    ui_set_value(g_output_editor, "");
    ui_screen_show_window(g_screen, g_output_window);

    if (!compile_stream_start())
    {
        compile_status_set("");
        ui_set_value(g_output_editor, "Could not start the build (pipe/thread creation failed).\n");
        return;
    }
    compile_status_set("Building...");
}

static int get_config_dir(char* buf, size_t cap);   /* defined further down */

/* Longest command line handed to the shell before the arguments are moved
 * into a response file instead (see exttool_write_response_file).
 *
 * cmd.exe refuses anything past 8191 characters outright, so the Windows
 * figure leaves room for the "cmd.exe /c " prefix and the tool's own name.
 * POSIX shells have no such limit - execve's ARG_MAX is megabytes - so the
 * threshold there is high enough that ordinary command lines never take
 * the response-file path at all, and it exists only as a backstop. */
#ifdef _WIN32
#define EXTTOOL_CMDLINE_MAX 7000
#else
#define EXTTOOL_CMDLINE_MAX 100000
#endif

/* Writes `args` to a response file and stores its path in `path`, so the
 * tool can be invoked as `tool @file` instead of with the arguments spelled
 * out on a command line the shell would reject.
 *
 * "@file" is a compiler convention, not a shell one - cl, link, gcc and
 * clang all understand it, an arbitrary program may not - so this is only
 * ever reached once the command line is genuinely too long to run as-is:
 * a tool that doesn't support it then fails with its own message about the
 * "@..." argument, which beats the shell's flat refusal to run anything.
 *
 * Returns 1 on success. On failure `path` is set to "" and the caller runs
 * the long command line anyway, letting the shell report the problem. */
static int exttool_write_response_file(const char* args, char* path, size_t cap)
{
    char dir[FS_MAX_PATH];
    if (!get_config_dir(dir, sizeof dir))
    {
        path[0] = 0;
        return 0;
    }

#ifdef _WIN32
    snprintf(path, cap, "%s\\exttool.rsp", dir);
#else
    snprintf(path, cap, "%s/exttool.rsp", dir);
#endif

    FILE* f = fopen(path, "wb");
    if (!f)
    {
        path[0] = 0;
        return 0;
    }

    size_t n = strlen(args);
    int ok = fwrite(args, 1, n, f) == n;
    if (fclose(f) != 0)
        ok = 0;
    if (!ok)
    {
        remove(path);
        path[0] = 0;
        return 0;
    }
    return 1;
}

/* Launches External Tool `index`. Returns quietly if one is already
 * running (same one-at-a-time rule as Compile - they share g_job) or the
 * tool has no command configured. */
static void do_run_external_tool(int index)
{
    if (g_job.running || index < 0 || index >= g_tools.count)
        return;

    const ext_tool* t = &g_tools.items[index];
    if (!t->command[0])
        return;

    /* Save first, like Compile does - a tool almost always reads the file
     * from disk, so running it against a stale copy would be surprising. */
    ui_node* active = g_active_editor_window;
    save_active_file(active);
    const char* path = active ? ui_get_path(active) : "";

    /* Growable, not fixed: $(CakeOutput) contributes one path per project
     * file, so a project of any size runs past whatever limit we'd pick -
     * and a command line cut short doesn't fail cleanly, it hands the
     * compiler a half-written path as its last file name. */
    struct exttool_buf argsb = { 0 }, dirb = { 0 }, cmdb = { 0 };
    exttool_expand(t->args, path, &argsb);
    exttool_expand(t->dir, path, &dirb);

    const char* args = exttool_buf_text(&argsb);
    const char* dir = exttool_buf_text(&dirb);

    exttool_append(&cmdb, t->command);
    if (args[0])
    {
        exttool_append(&cmdb, " ");
        exttool_append(&cmdb, args);
    }
    const char* cmd = exttool_buf_text(&cmdb);

    /* A leftover from a previous run only survives if that run couldn't
     * clean up; drop it before claiming the name again. */
    if (g_job.rsp_path[0])
    {
        remove(g_job.rsp_path);
        g_job.rsp_path[0] = 0;
    }

    if (cmdb.len > EXTTOOL_CMDLINE_MAX && args[0] &&
        exttool_write_response_file(args, g_job.rsp_path, sizeof g_job.rsp_path))
    {
        exttool_buf_free(&cmdb);
        exttool_append(&cmdb, t->command);
        exttool_append(&cmdb, " @\"");
        exttool_append(&cmdb, g_job.rsp_path);
        exttool_append(&cmdb, "\"");
        cmd = exttool_buf_text(&cmdb);
    }

    g_job.len = 0;
    g_job.lines = 0;
    if (g_job.text)
        g_job.text[0] = 0;
    g_job.active = active;
    snprintf(g_job.proc_title, sizeof g_job.proc_title, "%s", t->title);

    if (argsb.oom || dirb.oom || cmdb.oom)
    {
        const char* oom = "Out of memory building the command line.\n";
        compile_text_append(oom, strlen(oom));
        ui_set_value(g_output_editor, g_job.text ? g_job.text : "");
        ui_screen_show_window(g_screen, g_output_window);
        exttool_buf_free(&argsb);
        exttool_buf_free(&dirb);
        exttool_buf_free(&cmdb);
        return;
    }

    /* Echo exactly what is about to run - the fully expanded command and
     * the directory it runs in - the way Visual Studio's Output window
     * does. Without it a tool that prints nothing looks like nothing
     * happened, and a macro that expanded to something unexpected is
     * invisible. */
    compile_text_append("> ", 2);
    compile_text_append(cmd, strlen(cmd));
    compile_text_append("\n  (in ", 7);
    {
        const char* where = dir[0] ? dir : "the IDE's own directory";
        compile_text_append(where, strlen(where));
    }
    compile_text_append(")\n", 2);

    /* The echoed command above now says only "@<file>", so spell out what
     * went into it - otherwise the one case where the arguments matter
     * most is the one case they're invisible. */
    if (g_job.rsp_path[0])
    {
        static const char pre[] = "  (arguments passed in ";
        static const char post[] = ", too long for one command line)\n    ";
        compile_text_append(pre, sizeof pre - 1);
        compile_text_append(g_job.rsp_path, strlen(g_job.rsp_path));
        compile_text_append(post, sizeof post - 1);
        compile_text_append(args, strlen(args));
        compile_text_append("\n", 1);
    }
    ui_set_value(g_output_editor, g_job.text ? g_job.text : "");

    ui_screen_show_window(g_screen, g_output_window);
    if (active)
    {
        ui_screen_show_window(g_screen, active);
        ui_node* editor = editor_in_window(active);
        if (editor)
            ui_screen_focus(g_screen, editor);
    }

    /* The err out-param is not shown - see the failure block below. */
    g_job.proc = ui_process_start(cmd, dir[0] ? dir : NULL, NULL, 0);
    if (!g_job.proc)
    {
        /* Report the PATHS actually attempted, post-macro-expansion -
         * that is what is worth checking when a tool doesn't run. The OS
         * error code itself is deliberately not shown: it says nothing
         * useful here, and the common "program not found" case never
         * reaches this path anyway - the command runs through a shell, so
         * that arrives as ordinary captured output instead. */
        struct exttool_buf msg = { 0 };
        exttool_append(&msg, "Failed to start.\n  Command  : ");
        exttool_append(&msg, t->command);
        exttool_append(&msg, "\n  Arguments: ");
        exttool_append(&msg, args[0] ? args : "(none)");
        exttool_append(&msg, "\n  Directory: ");
        exttool_append(&msg, dir[0] ? dir : "(inherited from the IDE)");
        exttool_append(&msg, "\n");
        const char* text = exttool_buf_text(&msg);
        compile_text_append(text, strlen(text));
        exttool_buf_free(&msg);
        ui_set_value(g_output_editor, g_job.text ? g_job.text : "");
        compile_status_set("");
        if (g_job.rsp_path[0])   /* nothing ever read it - the child never ran */
        {
            remove(g_job.rsp_path);
            g_job.rsp_path[0] = 0;
        }
        exttool_buf_free(&argsb);
        exttool_buf_free(&dirb);
        exttool_buf_free(&cmdb);
        return;
    }
    g_job.running = 1;
    compile_status_set(g_job.proc_title[0] ? g_job.proc_title : "Running...");

    exttool_buf_free(&argsb);
    exttool_buf_free(&dirb);
    exttool_buf_free(&cmdb);
}

/* --- Output command line --------------------------------------------------- */

/* The <input> on the Output window's bottom row. A line typed there is, in order:
 * input for the running child (its stdin), one of g_cmdline_commands (IDE
 * actions such as "clone"), or else a shell command run like an External Tool. */
static struct
{
    ui_node* prompt;
    ui_node* input;
    int last_x, last_y, last_w;
    int last_stdin;
} g_cmdline;

/* The directory shell commands run in (see cmdline_cd). */
static void cmdline_dir(char* out, size_t cap)
{
    if (g_folder.dir[0])
        snprintf(out, cap, "%s", g_folder.dir);
    else if (!ui_get_cwd(out, (int)cap))
        snprintf(out, cap, ".");
}

static void cmdline_print(const char* text)
{
    compile_text_append(text, strlen(text));
    ui_set_value(g_output_editor, g_job.text ? g_job.text : "");
    ui_editor_goto_line(g_output_editor, g_job.lines + 1);
}

#define CMDLINE_MAX_ARGS 16

/* A command line split into words. argv[0] is the command name; a "quoted word" may hold spaces. */
struct cmdline_args
{
    int argc;
    char* argv[CMDLINE_MAX_ARGS];
    const char* rest; /* the text after the command name, unsplit - for commands that take free text */
};

/* Splits `line` in place into `args`. Returns 0 when there are more than CMDLINE_MAX_ARGS words. */
static int cmdline_split(char* line, struct cmdline_args* args)
{
    args->argc = 0;
    args->rest = "";
    char* p = line;
    for (;;)
    {
        while (*p == ' ' || *p == '\t')
            p++;
        if (!*p)
            return 1;
        if (args->argc == 1)
            args->rest = p;
        if (args->argc == CMDLINE_MAX_ARGS)
            return 0;

        char* out = p;
        args->argv[args->argc++] = out;
        char quote = 0;
        while (*p && (quote || (*p != ' ' && *p != '\t')))
        {
            if (*p == '"')
                quote = !quote;
            else
                *out++ = *p;
            p++;
        }
        if (*p)
            p++;
        *out = 0;
    }
}

enum cmdline_flags
{
    CMDLINE_IDLE_ONLY = 1 /* refused while a build or tool is running */
};

/* An IDE command. To add one: write a cmdline_xxx function and add a row to g_cmdline_commands. */
struct cmdline_command
{
    const char* name;
    const char* usage;    /* the arguments, as shown by "help" - "" when none */
    const char* help;
    int min_args, max_args; /* not counting the name; checked before run is called */
    int flags;
    void (*run)(const struct cmdline_args* args);
};

static void cmdline_help(const struct cmdline_args* args);

static void cmdline_clone(const struct cmdline_args* args)
{
    git_clone_start();
    if (args->argc > 1)
    {
        ui_set_value(g_gitclone.input, args->argv[1]);
        git_clone_update_suggestion();
    }
}

static void cmdline_build(const struct cmdline_args* args)
{
    (void)args;
    do_build();
}

static void cmdline_clear(const struct cmdline_args* args)
{
    (void)args;
    g_job.len = 0;
    g_job.lines = 0;
    if (g_job.text)
        g_job.text[0] = 0;
    ui_set_value(g_output_editor, "");
}

/* Shell commands each run in a fresh process, so a shell "cd" would not last; this one changes the Folder directory they all start in. */
static void cmdline_cd(const struct cmdline_args* args)
{
    char cwd[FS_MAX_PATH];
    cmdline_dir(cwd, sizeof cwd);

    if (args->argc > 1)
    {
        const char* arg = args->argv[1];
        int absolute = arg[0] == '/' || arg[0] == '\\' || (isalpha((unsigned char)arg[0]) && arg[1] == ':');
        char dir[FS_MAX_PATH];
        if (absolute)
        {
            snprintf(dir, sizeof dir, "%s", arg);
            ide_path_normalize(dir);
        }
        else
            path_join(dir, sizeof dir, cwd, arg);

        struct stat st;
        if (stat(dir, &st) != 0 || !(st.st_mode & S_IFDIR))
        {
            cmdline_print("No such directory: ");
            cmdline_print(dir);
            cmdline_print("\n");
            return;
        }
        folder_reveal_directory(dir);
        snprintf(cwd, sizeof cwd, "%s", dir);
    }
    cmdline_print(cwd);
    cmdline_print("\n");
}

static void cmdline_line(const struct cmdline_args* args)
{
    int line = atoi(args->argv[1]);
    ui_node* editor = editor_in_window(g_active_editor_window);
    if (line <= 0 || !editor)
    {
        cmdline_print(line <= 0 ? "Not a line number.\n" : "No file is open.\n");
        return;
    }
    nav_record_jump();
    ui_screen_show_window(g_screen, g_active_editor_window);
    ui_editor_goto_line(editor, line);
    ui_screen_focus(g_screen, editor);
}

static void on_ui_event(void* ctx, int id, void* param);

static void cmdline_tools(const struct cmdline_args* args)
{
    (void)args;
    on_ui_event(NULL, EVT_TOOLS_EXTERNAL, NULL);
}

/* A menu label as typed on the command line: lowercase letters and digits only, so "Open Folder..." is "openfolder". */
static void cmdline_menu_key(const char* label, char* out, size_t cap)
{
    size_t n = 0;
    for (const char* p = label; *p && n + 1 < cap; p++)
    {
        if (isalnum((unsigned char)*p))
            out[n++] = (char)tolower((unsigned char)*p);
    }
    out[n] = 0;
}

/* Finds the menu item whose label, or menu + label ("fileclose"), is `key`. Sets *ambiguous when several items match the label alone. */
static ui_node* cmdline_menu_find(const char* key, int* ambiguous)
{
    ui_node* found = NULL;
    *ambiguous = 0;
    for (int m = 0; m < ui_child_count(g_menubar); m++)
    {
        ui_node* menu = ui_child_at(g_menubar, m);
        char menu_key[64];
        cmdline_menu_key(ui_get_label(menu), menu_key, sizeof menu_key);
        for (int i = 0; i < ui_child_count(menu); i++)
        {
            ui_node* item = ui_child_at(menu, i);
            if (ui_get_separator(item) || ui_get_id(item) == 0)
                continue;
            char item_key[128], full_key[192];
            cmdline_menu_key(ui_get_label(item), item_key, sizeof item_key);
            snprintf(full_key, sizeof full_key, "%s%s", menu_key, item_key);
            if (strcmp(full_key, key) == 0)
            {
                *ambiguous = 0;
                return item;
            }
            if (strcmp(item_key, key) == 0)
            {
                if (found)
                    *ambiguous = 1;
                found = item;
            }
        }
    }
    return *ambiguous ? NULL : found;
}

/* Runs the External Tool whose title is the whole line (compared like menu labels). Returns 0 when no tool matches. */
static int cmdline_run_tool(const char* line)
{
    char key[192], title_key[192];
    cmdline_menu_key(line, key, sizeof key);
    if (!key[0])
        return 0;
    for (int i = 0; i < g_tools.count; i++)
    {
        cmdline_menu_key(g_tools.items[i].title, title_key, sizeof title_key);
        if (strcmp(title_key, key) == 0)
        {
            do_run_external_tool(i);
            return 1;
        }
    }
    return 0;
}

/* Runs the menu item named by the whole line, as if clicked. Returns 0 when no item matches. */
static int cmdline_run_menu(const char* line)
{
    char key[192];
    cmdline_menu_key(line, key, sizeof key);
    if (!key[0])
        return 0;

    int ambiguous;
    ui_node* item = cmdline_menu_find(key, &ambiguous);
    if (ambiguous)
    {
        cmdline_print("Several menus have that item - prefix the menu name, e.g. \"file close\".\n");
        return 1;
    }
    if (!item)
        return 0;
    if (!ui_get_enabled(item))
        cmdline_print("That menu item is disabled right now.\n");
    else
        on_ui_event(NULL, ui_get_id(item), NULL);
    return 1;
}

static void cmdline_menu(const struct cmdline_args* args)
{
    (void)args;
    for (int m = 0; m < ui_child_count(g_menubar); m++)
    {
        ui_node* menu = ui_child_at(g_menubar, m);
        char key[128];
        cmdline_menu_key(ui_get_label(menu), key, sizeof key);
        cmdline_print(key);
        cmdline_print(":");
        for (int i = 0; i < ui_child_count(menu); i++)
        {
            ui_node* item = ui_child_at(menu, i);
            if (ui_get_separator(item) || ui_get_id(item) == 0)
                continue;
            cmdline_menu_key(ui_get_label(item), key, sizeof key);
            cmdline_print(" ");
            cmdline_print(key);
        }
        cmdline_print("\n");
    }
}

static const struct cmdline_command g_cmdline_commands[] = {
    { "help",  "[command]", "list the IDE commands, or describe one", 0, 1, 0, cmdline_help },
    { "clone", "[url]", "open the Git Clone dialog", 0, 1, CMDLINE_IDLE_ONLY, cmdline_clone },
    { "build", "", "build the project or the active file", 0, 0, CMDLINE_IDLE_ONLY, cmdline_build },
    { "clear", "", "clear the Output window", 0, 0, 0, cmdline_clear },
    { "tools", "", "open the External Tools dialog", 0, 0, 0, cmdline_tools },
    { "line",  "<n>", "go to line n of the active file", 1, 1, 0, cmdline_line },
    { "menu",  "", "list the menu items - type one (\"format\", \"file close\") to run it", 0, 0, 0, cmdline_menu },
    { "cd",    "[dir]", "show or change the Folder directory, where shell commands run", 0, 1, CMDLINE_IDLE_ONLY, cmdline_cd },
};

static const struct cmdline_command* cmdline_find(const char* name)
{
    for (size_t i = 0; i < sizeof g_cmdline_commands / sizeof g_cmdline_commands[0]; i++)
    {
        if (strcmp(g_cmdline_commands[i].name, name) == 0)
            return &g_cmdline_commands[i];
    }
    return NULL;
}

static void cmdline_print_usage(const struct cmdline_command* c)
{
    char line[256];
    snprintf(line, sizeof line, "  %s %-10s %s\n", c->name, c->usage, c->help);
    cmdline_print(line);
}

static void cmdline_help(const struct cmdline_args* args)
{
    if (args->argc > 1)
    {
        const struct cmdline_command* c = cmdline_find(args->argv[1]);
        if (c)
            cmdline_print_usage(c);
        else
            cmdline_print("Not an IDE command - it would run in the shell.\n");
        return;
    }
    for (size_t i = 0; i < sizeof g_cmdline_commands / sizeof g_cmdline_commands[0]; i++)
        cmdline_print_usage(&g_cmdline_commands[i]);
    cmdline_print("An External Tool's title or a menu item's name runs it (see \"menu\"). Anything else, or a line starting with !, runs in the shell.\n"
                  "While a program runs, a line is sent to its input.\n");
}

#ifdef _WIN32
/* POSIX command names cmd.exe lacks, typed out of habit. Add a row to add one. */
static const struct { const char* posix; const char* win; } g_cmdline_shell_aliases[] = {
    { "ls", "dir" },
};

/* Rewrites `line` into `out` with a POSIX command name swapped for its cmd.exe one ("ls src" -> "dir src").
 * Options ("-la") are dropped: cmd.exe would read them as file names. */
static const char* cmdline_shell_alias(const char* line, char* out, size_t cap)
{
    size_t name_len = strcspn(line, " \t");
    for (size_t i = 0; i < sizeof g_cmdline_shell_aliases / sizeof g_cmdline_shell_aliases[0]; i++)
    {
        const char* posix = g_cmdline_shell_aliases[i].posix;
        if (strlen(posix) != name_len || strncmp(posix, line, name_len) != 0)
            continue;

        snprintf(out, cap, "%s", g_cmdline_shell_aliases[i].win);
        const char* p = line + name_len;
        while (*p)
        {
            while (*p == ' ' || *p == '\t')
                p++;
            size_t w = strcspn(p, " \t");
            if (w > 0 && p[0] != '-')
            {
                size_t n = strlen(out);
                snprintf(out + n, cap - n, " %.*s", (int)w, p);
            }
            p += w;
        }
        return out;
    }
    return line;
}
#endif

static void cmdline_run_shell(const char* line)
{
#ifdef _WIN32
    char aliased[1024];
    line = cmdline_shell_alias(line, aliased, sizeof aliased);
#endif
    g_job.len = 0;
    g_job.lines = 0;
    if (g_job.text)
        g_job.text[0] = 0;
    g_job.active = NULL;
    snprintf(g_job.proc_title, sizeof g_job.proc_title, "%s", line);

    const char* dir = g_folder.dir[0] ? g_folder.dir : NULL;
    cmdline_print("> ");
    cmdline_print(line);
    cmdline_print("\n");

    char err[256];
#ifdef _WIN32
    g_job.proc = ui_process_start(line, dir, err, sizeof err);
#else
    const char* argv[] = { "/bin/sh", "-c", line, NULL };
    g_job.proc = ui_process_start_direct(argv, dir, err, sizeof err);
#endif
    if (!g_job.proc)
    {
        cmdline_print("Failed to start: ");
        cmdline_print(err);
        cmdline_print("\n");
        return;
    }
    g_job.running = 1;
    compile_status_set(g_job.proc_title);
}

/* Sends one typed line to the running child's stdin. */
static void cmdline_send_stdin(const char* text)
{
    char line[1024 + 2];
    size_t n = strlen(text);
    if (n > sizeof line - 2)
        n = sizeof line - 2;
    memcpy(line, text, n);
    line[n] = '\n';
    line[n + 1] = 0;
    if (ui_process_write(g_job.proc, line, (int)n + 1) < 0)
        cmdline_print("(the running program does not accept input)\n");
    else
        cmdline_print(line);
}

/* EVT_OUTPUT_CMDLINE: Enter in the command line. */
static void cmdline_execute(void)
{
    char line[1024];
    snprintf(line, sizeof line, "%s", ui_get_value(g_cmdline.input));
    ui_set_value(g_cmdline.input, "");

    if (g_job.running && g_job.proc)
    {
        cmdline_send_stdin(line);
        return;
    }

    /* "!cmd" always goes to the shell, even when cmd is also an IDE command or menu item. */
    const char* start = line;
    while (*start == ' ' || *start == '\t')
        start++;
    if (*start == '!')
    {
        if (g_job.running)
            cmdline_print("A build is still running.\n");
        else if (start[1])
            cmdline_run_shell(start + 1);
        return;
    }

    /* The shell gets the line as typed; splitting would drop its quotes. */
    char shell_line[1024];
    snprintf(shell_line, sizeof shell_line, "%s", line);

    struct cmdline_args args;
    if (!cmdline_split(line, &args))
    {
        cmdline_print("Too many arguments.\n");
        return;
    }
    if (args.argc == 0)
        return;

    const struct cmdline_command* c = cmdline_find(args.argv[0]);
    if (c)
    {
        int n = args.argc - 1;
        if (n < c->min_args || n > c->max_args)
        {
            cmdline_print("Usage:\n");
            cmdline_print_usage(c);
        }
        else if ((c->flags & CMDLINE_IDLE_ONLY) && g_job.running)
            cmdline_print("A build is still running.\n");
        else
            c->run(&args);
        return;
    }

    if (g_job.running)
    {
        cmdline_print("A build is still running.\n");
        return;
    }

    if (cmdline_run_tool(shell_line) || cmdline_run_menu(shell_line))
        return;
    cmdline_run_shell(shell_line);
}

/* Per frame: a click in the Output text moves the focus to the command line once the button is released, unless it selected text (kept so Ctrl+C still copies it). */
static void cmdline_take_focus(void)
{
    if (ui_screen_focused(g_screen) != g_output_editor || ui_screen_mouse_down(g_screen))
        return;
    int lo, hi;
    if (ui_editor_get_selection(g_output_editor, &lo, &hi) && lo != hi)
        return;
    ui_screen_focus(g_screen, g_cmdline.input);
}

/* Per frame: only the output <editor> is stretched reliably with the window (a docked window's own h is its remembered thickness, and the <input>/<text> are not kept on the bottom row), so put the prompt and input on the row just below the editor. */
static void cmdline_layout(void)
{
    int ex, ey, ew, eh;
    ui_get_rect(g_output_editor, &ex, &ey, &ew, &eh);
    int row = ey + eh;

    /* Output's own colors, re-read every frame: a theme switch does not re-theme editor_output_* colors baked into nodes. */
    const ui_theme* theme = ui_get_theme();
    ui_set_color(g_cmdline.prompt, theme->window_fg, theme->editor_output_bg);
    ui_set_input_colors(g_cmdline.input, theme->editor_output_fg, theme->editor_output_bg);

    int to_stdin = g_job.running && g_job.proc;
    if (ex == g_cmdline.last_x && row == g_cmdline.last_y && ew == g_cmdline.last_w &&
        to_stdin == g_cmdline.last_stdin)
        return;
    g_cmdline.last_x = ex;
    g_cmdline.last_y = row;
    g_cmdline.last_w = ew;
    g_cmdline.last_stdin = to_stdin;

    /* "stdin>" while a program runs: a line typed then is its input, not a command (see cmdline_execute). */
    const char* label = to_stdin ? "stdin>" : ">";
    int gap = (int)strlen(label) + 1;
    ui_set_label(g_cmdline.prompt, label);
    ui_set_rect(g_cmdline.prompt, ex, row, 0, 0);
    ui_set_rect(g_cmdline.input, ex + gap, row, ew - gap, 1);
}

/* --- Debug menu (scripted lldb - see ide_debug.h) --------------------------- */

/* Routes lldb's raw session log (every line debug_poll() sees, prompts
 * and all) into the same Output window Compile/External Tools already
 * share (compile_text_append/g_job.text) - one place to look regardless of
 * what produced the text, same reasoning as do_run_external_tool above. */
static void debug_append_output(void* ctx, const char* line, size_t len)
{
    (void)ctx;
    compile_text_append(line, len);
    compile_text_append("\n", 1);
    ui_set_value(g_output_editor, g_job.text ? g_job.text : "");
    ui_editor_goto_line(g_output_editor, g_job.lines + 1);
}

/* Finds the open document window whose path matches `file` - lldb's
 * reported source file from its "frame #0: ... at FILE:LINE:COL" stop
 * report (see lldb_parse_frame0_location in ide_debug.c) - so the exec-
 * line highlight/auto-scroll (see debug_sync_exec_line below) lands on the
 * right editor. Exact match first, then a basename fallback, since lldb
 * and the IDE do not always agree on absolute vs relative form for the
 * same file. */
/* Returns the WRAPPER (not the editor child) so the caller can raise the
 * window (ui_screen_show_window) as well as edit its content - a stop that
 * only updates exec_line/scroll without also bringing the window to front
 * is invisible whenever something else (e.g. the Output window, likely
 * still frontmost from being read right after the previous stop) is on
 * top of it. */
static ui_node* find_editor_window_for_path(const char* file)
{
    if (!file || !file[0])
        return NULL;

    /* `file` comes straight from the backend's own stop-location report -
     * lldb always uses '/', but cdb (see cdb_parse_stop_location() in
     * ide_debug.c) reports whatever separator the PDB's source path used,
     * which for a Windows build is '\' (e.g. "c:\...\playground.c") - so
     * the basename split has to recognize either, the same fix already
     * applied to do_debug_start()'s own path splitting. Without it, the
     * whole backslash path is compared as a "basename" and never matches
     * the editor's forward-slash path, silently leaving the exec-line
     * highlight unapplied even though the debugger really did stop there
     * (confirmed - the Output window showed the right file@line, only the
     * editor highlight was missing). */
    const char* file_base = strrchr(file, '/');
    const char* file_base_bs = strrchr(file, '\\');
    if (file_base_bs && (!file_base || file_base_bs > file_base))
        file_base = file_base_bs;
    file_base = file_base ? file_base + 1 : file;

    for (int i = 0; i < ui_screen_window_count(g_screen); i++)
    {
        ui_node* wrapper = ui_screen_window_at(g_screen, i);
        ui_node* ed = editor_in_window(wrapper);
        if (!ed)
            continue;
        const char* path = ui_get_path(wrapper);
        if (!path[0])
            continue;
        if (strcmp(path, file) == 0)
            return wrapper;

        const char* path_base = strrchr(path, '/');
        const char* path_base_bs = strrchr(path, '\\');
        if (path_base_bs && (!path_base || path_base_bs > path_base))
            path_base = path_base_bs;
        path_base = path_base ? path_base + 1 : path;
        /* Case-insensitive: cdb lower-cases the drive letter and whole
         * path in its own reports ("c:\users\..."), which will never
         * byte-for-byte match the editor's path as opened (whatever case
         * the user's filesystem/dialog produced) even after the separator
         * fix above - Windows paths are case-insensitive anyway, so this
         * is the correct comparison there, not just a workaround. */
        if (ci_strcmp(path_base, file_base) == 0)
            return wrapper;
    }
    return NULL;
}

/* Clears the exec-line highlight from every open editor - called whenever
 * the debuggee stops being "stopped at a specific line" (running again,
 * exited, or the session ending), so a stale highlight never lingers on a
 * window the debugger has moved on from. */
static void debug_clear_exec_line_everywhere(void)
{
    for (int i = 0; i < ui_screen_window_count(g_screen); i++)
    {
        ui_node* wrapper = ui_screen_window_at(g_screen, i);
        ui_node* ed = editor_in_window(wrapper);
        if (ed)
            ui_set_exec_line(ed, 0);
    }
}

/* Which stop debug_sync_exec_line() last actually synced onto an editor -
 * so a stop already reflected isn't re-applied on every single poll tick
 * while the debuggee just sits there stopped. Cleared (line back to -1)
 * whenever state leaves DBG_STOPPED, so the NEXT stop - even one landing on
 * this exact same file/line again (a breakpoint inside a loop) - is always
 * treated as new. */
static char g_debug_synced_file[DEBUG_MAX_PATH] = { 0 };
static int g_debug_synced_line = -1;

/* Reflects g_dbg's current stop location onto the editor it belongs to -
 * called once per tick from app_frame(), right after draining whatever the
 * backend debugger printed (see debug_stream_poll below). */
static void debug_sync_exec_line(void)
{
    if (g_dbg.state == DBG_STOPPED && g_dbg.cur_line > 0)
    {
        /* Only do the actual sync - including ui_screen_show_window()
         * below - on a NEW stop, not every poll tick spent sitting at an
         * already-synced one. ui_screen_show_window() unconditionally
         * clears s->open_menu (see its own doc comment - reusing a window
         * slot resets menu/select/focus state right along with it), so
         * calling it every frame while stopped was force-closing any menu
         * the user tried to open the instant they clicked it - Step
         * Into/Over included, and indistinguishable from the whole menu
         * bar being frozen. Confirmed: reported as "can't press menu Step
         * Into... but only when stopped at the breakpoint line." */
        if (g_debug_synced_line == g_dbg.cur_line &&
            strcmp(g_debug_synced_file, g_dbg.cur_file) == 0)
            return;

        debug_clear_exec_line_everywhere();
        ui_node* wrapper = find_editor_window_for_path(g_dbg.cur_file);
        if (wrapper)
        {
            ui_node* ed = editor_in_window(wrapper);
            ui_set_exec_line(ed, g_dbg.cur_line);
            ui_editor_goto_line(ed, g_dbg.cur_line);
            /* Raise the window on every NEW stop, not just the session's
             * first (see find_editor_window_for_path's own comment) -
             * without this, a stop reached after the user has clicked into
             * the Output window (to read the very log proving the
             * debugger IS stopped in the right place) applies the
             * highlight to a window that's silently behind another one,
             * and looks from the outside exactly like "it didn't stop". */
            ui_screen_show_window(g_screen, wrapper);
        }

        snprintf(g_debug_synced_file, sizeof g_debug_synced_file, "%s", g_dbg.cur_file);
        g_debug_synced_line = g_dbg.cur_line;
    }
    else if (g_dbg.state != DBG_STOPPED)
    {
        debug_clear_exec_line_everywhere();
        g_debug_synced_file[0] = 0;
        g_debug_synced_line = -1;
    }
}

/* Keeps the Debug menu's items enabled/disabled for whatever g_dbg.state
 * currently is - same forward-declared/kept-current-every-frame pattern as
 * g_compile_item (see its own doc comment). */
static void debug_menu_refresh(void)
{
    if (g_debug_start_item)
        ui_set_enabled(g_debug_start_item, g_dbg.state == DBG_IDLE);
    if (g_debug_stop_item)
        ui_set_enabled(g_debug_stop_item, g_dbg.state != DBG_IDLE);
    int stopped = g_dbg.state == DBG_STOPPED;
    if (g_debug_continue_item)
        ui_set_enabled(g_debug_continue_item, stopped);
    if (g_debug_step_over_item)
        ui_set_enabled(g_debug_step_over_item, stopped);
    if (g_debug_step_into_item)
        ui_set_enabled(g_debug_step_into_item, stopped);
}

/* Debug > "Toggle Breakpoint" (EVT_DEBUG_TOGGLE_BREAKPOINT) / F9 - the
 * keyboard equivalent of clicking the active editor's gutter (see
 * editor_click_set_cursor in ide_ui.c), toggling a breakpoint on whichever
 * line the caret is currently on. */
static void do_debug_toggle_breakpoint(void)
{
    ui_node* active = g_active_editor_window;
    ui_node* ed = editor_in_window(active);
    if (!ed || ui_get_syntax(ed) != UI_SYNTAX_C)
        return;   /* same restriction as the gutter click - see
                   * editor_click_set_cursor's own comment */

    const char* text = ui_get_value(ed);
    int cursor = ui_editor_get_cursor(ed);
    int line = 1;
    for (int i = 0; i < cursor && text[i]; i++)
    {
        if (text[i] == '\n')
            line++;
    }
    ui_editor_toggle_breakpoint(ed, line);
}

/* Debug > "Start Debugging" (EVT_DEBUG_START) / F5.
 *
 * Launches lldb on the active document's executable and inserts a
 * breakpoint for every line toggled in that document's gutter (see
 * ui_editor_toggle_breakpoint in ide_ui.h). The executable itself is NOT
 * built here - this assumes one already exists at cake's own predicted
 * OUTPUT location, same "<root>/<platform name>/<relative path>" layout
 * $(CakeOutput) itself predicts (see exttool_append_cake_output's own doc
 * comment) - e.g. "foo.c" next to no project gets root = its own
 * directory, so the expected binary is "<that dir>/<platform>/foo" (no
 * extension), the way `cc -g "<that dir>/<platform>/foo.c" -o
 * "<that dir>/<platform>/foo"` run on cake's own generated C would leave
 * it. Automatically running cake + that compile step first (threading a -g
 * flag through the External Tools' $(CakeOutput)-style expansion - see
 * exttool_expand) is a natural follow-up, deliberately left out of this
 * first pass rather than guessing at a toolchain command that fits every
 * user's setup. */
static void do_debug_start(void)
{
    if (g_dbg.state != DBG_IDLE)
        return;   /* one session at a time */

    ui_node* active = g_active_editor_window;
    ui_node* ed = editor_in_window(active);
    const char* path = active ? ui_get_path(active) : "";
    if (!ed || !path[0])
        return;

    save_active_file(active);

    /* Split `path` into its directory, bare file name (with extension -
     * src_base, "playground.c"), and base name (no extension - base,
     * "playground") - same split exttool_expand() does for its own
     * $(Dir)/$(Name) macros, just inlined here since this needs only the
     * pieces, not the full macro table. src_base (not the full `path`) is
     * what breakpoint file arguments below are matched against: lldb
     * resolves "breakpoint set --file" against whatever filename form is
     * actually embedded in the debug info, which for cake's own generated
     * C is the #line directive's own filename (codegen.c) - a bare
     * relative name, same convention cake itself was invoked with, not an
     * IDE-side absolute path. Passing the full absolute `path` there
     * instead left every breakpoint "pending" (never resolved, so the
     * program just ran straight through) - a real symptom seen while
     * testing this. */
    char src_dir[DEBUG_MAX_PATH] = { 0 };
    char src_base[512];
    char base[512];
    {
        char tmp[DEBUG_MAX_PATH];
        snprintf(tmp, sizeof tmp, "%s", path);
        char* slash = strrchr(tmp, '/');
        char* backslash = strrchr(tmp, '\\');
        if (backslash && (!slash || backslash > slash))
            slash = backslash;
        snprintf(src_base, sizeof src_base, "%s", slash ? slash + 1 : tmp);
        if (slash)
        {
            *slash = 0;
            snprintf(src_dir, sizeof src_dir, "%s", tmp);
        }
        snprintf(base, sizeof base, "%s", src_base);
        char* dot = strrchr(base, '.');
        if (dot)
            *dot = 0;   /* playground.c -> playground */
    }

    /* The same two helpers $(TargetDir)/$(TargetFileName) expand through,
     * so an External Tool that writes its binary to $(TargetPath) and this
     * launch cannot drift apart - with a project open both root at the
     * PROJECT's directory, not the active document's (the playground is a
     * scratch file that is never part of a project, so deriving the path
     * from whatever has focus pointed Debug at %APPDATA% instead of the
     * project the user was actually building). They also carry the
     * platform/extension fallbacks this function used to spell out inline
     * (msvc links "<name>.exe", gcc/clang a bare name). */
    char dir[DEBUG_MAX_PATH];
    target_dir_path(src_dir, dir, sizeof dir);

    char exe_name[512];
    target_file_name(base, exe_name, sizeof exe_name);
    char exe_path[DEBUG_MAX_PATH];
    path_join(exe_path, sizeof exe_path, dir, exe_name);

    g_job.len = 0;
    g_job.lines = 0;
    if (g_job.text)
        g_job.text[0] = 0;
    ui_set_value(g_output_editor, "");
    ui_screen_show_window(g_screen, g_output_window);
    ui_screen_show_window(g_screen, active);

    char header[DEBUG_MAX_PATH + 64];
    /* Cosmetic only - quoted so a path containing spaces (e.g. under
     * "Application Support") reads unambiguously as one argument. The
     * actual launch never goes through a shell (see debug_start() in
     * ide_debug.c - argv is exec'd directly), so this quoting has no
     * bearing on whether the real command works, only on how it echoes.
     * Must match debug_start()'s own choice of backend per platform (cdb
     * on Windows, lldb elsewhere - see its doc comment in ide_debug.h) or
     * this just prints a lie. */
#if defined(_WIN32)
    snprintf(header, sizeof header, "> cdb -lines \"%s\"\n", exe_path);
#else
    snprintf(header, sizeof header, "> lldb --no-use-colors -x -- \"%s\"\n", exe_path);
#endif
    compile_text_append(header, strlen(header));
    ui_set_value(g_output_editor, g_job.text ? g_job.text : "");

    debug_init(&g_dbg);
    g_dbg.on_output = debug_append_output;

    /* debug_start()'s failure could mean either of two very different
     * things - the target wasn't built yet, or the backend debugger itself
     * (cdb on Windows, lldb elsewhere - see debug_start()'s own doc
     * comment) isn't installed/on PATH - and CreateProcess's own error text
     * ("The system cannot find the file specified") doesn't say which file
     * it means, so a single generic message blaming exe_path (the old
     * wording here) was actively misleading whenever the real problem was
     * a missing backend. Telling these apart doesn't need CreateProcess at
     * all - exe_path is just a plain argument to the backend, never looked
     * up by CreateProcess itself, so whether it exists on disk is
     * orthogonal to whether the backend launched; checking it directly
     * (fopen, same pattern this file already uses elsewhere) picks the
     * right explanation instead of guessing. */
    bool exe_exists = false;
    {
        FILE* f = fopen(exe_path, "rb");
        if (f)
        {
            exe_exists = true;
            fclose(f);
        }
    }

    char err[256] = { 0 };
    if (!debug_start(&g_dbg, exe_path, NULL, dir[0] ? dir : NULL, err, sizeof err))
    {
        char msg[DEBUG_MAX_PATH + 384];
        if (!exe_exists)
        {
            snprintf(msg, sizeof msg,
                      "Could not start debugging: no built executable at '%s'.\n"
                      "Build the project for this target first, then try Debug again.\n",
                      exe_path);
        }
        else
        {
#if defined(_WIN32)
            snprintf(msg, sizeof msg,
                      "Could not start debugging: %s\n"
                      "'%s' exists, so this is cdb itself failing to launch - "
                      "make sure cdb (part of the Windows SDK's \"Debugging Tools "
                      "for Windows\", or WinDbg) is installed and its directory is "
                      "on your PATH.\n"
                      "winget install Microsoft.WinDbg\n",
                      err, exe_path);
#else
            snprintf(msg, sizeof msg,
                      "Could not start debugging: %s\n"
                      "'%s' exists, so this is lldb itself failing to launch - "
                      "make sure lldb (LLVM) is installed and its directory is on your PATH.\n",
                      err, exe_path);
#endif
        }
        compile_text_append(msg, strlen(msg));
        ui_set_value(g_output_editor, g_job.text ? g_job.text : "");
        debug_menu_refresh();
        return;
    }

    int bp_lines[256];
    int bp_count = ui_editor_get_breakpoints(ed, bp_lines, 256);
    for (int i = 0; i < bp_count; i++)
        debug_break_insert(&g_dbg, src_base, bp_lines[i]);

    debug_run(&g_dbg);
    debug_menu_refresh();
}

static void do_debug_stop(void)
{
    if (g_dbg.state == DBG_IDLE)
        return;
    debug_shutdown(&g_dbg);
    debug_clear_exec_line_everywhere();
    debug_menu_refresh();
}

static void do_debug_continue(void)
{
    if (g_dbg.state != DBG_STOPPED)
        return;
    debug_continue(&g_dbg);
}

static void do_debug_step_over(void)
{
    if (g_dbg.state != DBG_STOPPED)
        return;
    debug_step_over(&g_dbg);
}

static void do_debug_step_into(void)
{
    if (g_dbg.state != DBG_STOPPED)
        return;
    debug_step_into(&g_dbg);
}

/* Rebuilds the Debug Info panel's listbox from g_dbg.locals/g_dbg.frames -
 * a "Locals" header row, one row per local (name = value), a "Call Stack"
 * header row, one row per frame - same clear-then-rebuild pattern as
 * populate_listbox_from_dir() (ide.c:2987 area): drop every existing
 * child, then append fresh UI_TAG_ITEM rows. Header rows carry no id/path
 * (nothing to select them into, they're just section labels); the listbox
 * itself is read-only for this pass - see Stage 4's own non-goals (no
 * click-to-select-frame yet). Called only when g_dbg.info_dirty is set
 * (see debug_stream_poll below), not every frame. */
static void debug_info_panel_refresh(void)
{
    if (!g_debuginfo_listbox)
        return;

    while (ui_child_count(g_debuginfo_listbox) > 0)
    {
        ui_node* c = ui_child_at(g_debuginfo_listbox, 0);
        ui_remove_child(g_debuginfo_listbox, c);
        ui_node_free(c);
    }

    ui_node* locals_header = ui_create_element(UI_TAG_ITEM);
    ui_set_label(locals_header, "-- Locals --");
    ui_append_child(g_debuginfo_listbox, locals_header);

    if (g_dbg.locals_count == 0)
    {
        ui_node* empty = ui_create_element(UI_TAG_ITEM);
        ui_set_label(empty, "  (none)");
        ui_append_child(g_debuginfo_listbox, empty);
    }
    for (int i = 0; i < g_dbg.locals_count; i++)
    {
        char label[400];
        snprintf(label, sizeof label, "  %s = %s", g_dbg.locals[i].name, g_dbg.locals[i].value);
        ui_node* item = ui_create_element(UI_TAG_ITEM);
        ui_set_label(item, label);
        ui_append_child(g_debuginfo_listbox, item);
    }

    ui_node* frames_header = ui_create_element(UI_TAG_ITEM);
    ui_set_label(frames_header, "-- Call Stack --");
    ui_append_child(g_debuginfo_listbox, frames_header);

    if (g_dbg.frames_count == 0)
    {
        ui_node* empty = ui_create_element(UI_TAG_ITEM);
        ui_set_label(empty, "  (none)");
        ui_append_child(g_debuginfo_listbox, empty);
    }
    for (int i = 0; i < g_dbg.frames_count; i++)
    {
        char label[300];
        snprintf(label, sizeof label, "  #%d %s", g_dbg.frames[i].index, g_dbg.frames[i].text);
        ui_node* item = ui_create_element(UI_TAG_ITEM);
        ui_set_label(item, label);
        ui_append_child(g_debuginfo_listbox, item);
    }
}

/* Whether g_dbg was DBG_STOPPED as of the end of the previous
 * debug_stream_poll() call - a persistent flag rather than a `prev_state`
 * local re-read from g_dbg.state at each call's entry, because a Continue/
 * Step menu command flips g_dbg.state to DBG_RUNNING synchronously from
 * on_ui_event(), a separate call entirely from debug_stream_poll() - by
 * the time the next poll runs, g_dbg.state already reads DBG_RUNNING, so
 * a same-call `prev_state` would never see the DBG_STOPPED it was
 * actually transitioning away from, and debug_clear_info() would never
 * fire (the Locals/Call Stack panel would keep showing the stop just
 * left, arbitrarily stale, until the next real stop overwrote it). */
static bool g_debug_was_stopped = false;

/* Called once per frame from app_frame(), same "cheap no-op when idle"
 * contract as compile_stream_poll() - debug_poll() itself already no-ops
 * with no live session. */
static void debug_stream_poll(void)
{
    debug_poll(&g_dbg);
    debug_sync_exec_line();
    if (debug_backend_exited(&g_dbg))
        debug_clear_exec_line_everywhere();
    debug_menu_refresh();

    /* Locals/Call Stack refresh, triggered exactly once per fresh stop
     * (not every frame the debuggee happens to still be stopped) - see
     * debug_refresh_info()'s own doc comment in ide_debug.h. Leaving
     * DBG_STOPPED for any reason (running again, exited, session torn
     * down) clears the panel instead, so it never shows a stale stop's
     * data once the debuggee has moved on. */
    bool now_stopped = g_dbg.state == DBG_STOPPED;
    if (now_stopped && !g_debug_was_stopped)
        debug_refresh_info(&g_dbg);
    else if (!now_stopped && g_debug_was_stopped)
        debug_clear_info(&g_dbg);
    g_debug_was_stopped = now_stopped;

    if (g_dbg.info_dirty)
    {
        debug_info_panel_refresh();
        g_dbg.info_dirty = false;
    }

    /* Auto-end the session the instant the debuggee exits, rather than
     * leaving DBG_EXITED sitting there until the user manually hits Stop
     * (Shift+F5) - every other IDE returns straight to "ready to run"
     * once the program under it finishes, and the Debug menu otherwise
     * keeps showing Stop as the live action with Start disabled even
     * though nothing is actually running any more. The debuggee's own
     * final output (and the "exited with status = N" line itself) has
     * already reached the Output window via on_output before this runs,
     * in the same debug_poll() call that set DBG_EXITED - nothing is lost
     * by tearing the session down immediately. */
    if (g_dbg.state == DBG_EXITED)
    {
        debug_shutdown(&g_dbg);
        debug_menu_refresh();
        g_debug_was_stopped = false;
    }
}

/* External Tool counterpart of compile_finish(): reap the child, note its
 * exit code, and re-parse the captured text for diagnostics so a tool that
 * prints compiler-style "file:line:col: error: msg" gets inline squiggles
 * exactly like the internal compile. */
static void exttool_finish(void)
{
    int code = ui_process_close(g_job.proc);
    g_job.proc = NULL;
    g_job.running = 0;

    /* Safe only now: the child has exited, so nothing is still reading it. */
    if (g_job.rsp_path[0])
    {
        remove(g_job.rsp_path);
        g_job.rsp_path[0] = 0;
    }

    char footer[128];
    snprintf(footer, sizeof footer, "\nExit code %d\n", code);
    compile_text_append(footer, strlen(footer));
    ui_set_value(g_output_editor, g_job.text ? g_job.text : "");

    if (g_job.text)
    {
        apply_diagnostics(g_job.text);
        g_job.len = 0;  /* strtok chopped it up - see compile_finish() */
    }

    refresh_open_windows();
    compile_status_set("");
}

/* Runs on the main thread once the worker has finished AND the pipe is
 * drained - everything the old do_compile() used to do after
 * capture_and_compile() returned. */
static void compile_finish(void)
{
    compile_stream_end();

    char summary[256];
    snprintf(summary, sizeof summary, "\n%d error(s), %d warning(s), %.2f sec\n",
              g_job.report.error_count, g_job.report.warnings_count,
              g_job.report.cpu_time_used_sec);
    compile_text_append(summary, strlen(summary));
    ui_set_value(g_output_editor, g_job.text ? g_job.text : "");

    /* Re-parse the captured text into each open file's diagnostic list -
     * see apply_diagnostics(). Cleared first so every compile starts
     * from a blank slate instead of accumulating stale diagnostics on top
     * of a previous run's. strtok mutates the buffer in place, which is
     * fine: ui_set_value() above already took its own copy of the text. */
    if (g_job.text)
    {
        apply_diagnostics(g_job.text);
        g_job.len = 0;  /* strtok chopped it up - don't reuse it as text */
    }

    /* Pick up any on-disk changes the build produced (leaves dirty windows
     * and the caret positions untouched - see refresh_open_windows). */
    refresh_open_windows();

    /* Back to blank - the slot only means "a compile is running". The
     * error/warning counts are already in the Output window's summary line,
     * so repeating them here would just be a second, staler copy. */
    compile_status_set("");
}

/* Splits the desktop area (see ui_screen_desktop_rect) in half and places
 * `left`/`right` - each a real <window> node, NOT its <modal> wrapper - side
 * by side, `left` on the left half and `right` on the right. Used by "Show
 * Generated Code" (EVT_EDITOR_SHOW_OUTPUT, below) whenever the source window
 * is currently maximized - which every editor window starts as, Playground
 * included, see make_editor_window() - since simply raising the artifact
 * window on top in that case would cover
 * the source completely edge-to-edge, hiding the very thing that produced
 * it. Only reached for when the source is actually maximized (see the call
 * site's own check) - a source left at some smaller, user-placed rect is
 * never touched by this.
 *
 * ui_window_set_rect() clears the maximized flag on whichever window it's
 * given (see its own doc comment) - that's deliberate here: once tiled,
 * neither window is "maximized" edge-to-edge anymore, and leaving the flag
 * set would have resync_maximized_windows() snap it straight back to full
 * width the very next frame, fighting the tile layout. */
static void tile_side_by_side(ui_node* left, ui_node* right)
{
    if (!left || !right)
        return;

    int dx, dy, dw, dh;
    ui_screen_desktop_rect(g_screen, &dx, &dy, &dw, &dh);
    int left_w = dw / 2;
    if (left_w < 1) left_w = 1;
    int right_w = dw - left_w;
    if (right_w < 1) right_w = 1;
    ui_window_set_rect(left, dx, dy, left_w, dh);
    ui_window_set_rect(right, dx + left_w, dy, right_w, dh);
}

/* --- Search > Replace (find/replace) --------------------------------------
 * Literal (non-regex) search over the captured editor's text, honoring the
 * dialog's Options/Direction/Scope/Origin. "Regular expression" is accepted
 * but treated as a literal search for now, and "Prompt on replace" isn't
 * wired yet (Change All replaces every match without asking). */

static int fr_is_word(unsigned char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') || c == '_';
}

/* Whether `needle` (nlen bytes) sits at byte `pos` of `hay` (hlen bytes),
 * under the case-insensitive (ci) and whole-word rules. */
static int fr_match_at(const char* hay, int hlen, int pos,
                       const char* needle, int nlen, int ci, int whole)
{
    if (pos < 0 || pos + nlen > hlen)
        return 0;
    for (int i = 0; i < nlen; i++)
    {
        char a = hay[pos + i], b = needle[i];
        if (ci)
        {
            a = (char)tolower((unsigned char)a);
            b = (char)tolower((unsigned char)b);
        }
        if (a != b)
            return 0;
    }
    if (whole)
    {
        if (pos > 0 && fr_is_word((unsigned char)hay[pos - 1]))
            return 0;
        if (pos + nlen < hlen && fr_is_word((unsigned char)hay[pos + nlen]))
            return 0;
    }
    return 1;
}

/* First match at/after `from` (forward) or at/before it (backward), confined
 * to the byte range [lo,hi). Returns the match's offset, or -1. */
static int fr_find(const char* hay, int hlen, const char* needle, int nlen,
                   int from, int fwd, int ci, int whole, int lo, int hi)
{
    if (nlen == 0)
        return -1;
    if (fwd)
    {
        for (int p = from < lo ? lo : from; p + nlen <= hi; p++)
            if (fr_match_at(hay, hlen, p, needle, nlen, ci, whole))
                return p;
    }
    else
    {
        for (int p = from > hi - nlen ? hi - nlen : from; p >= lo; p--)
            if (fr_match_at(hay, hlen, p, needle, nlen, ci, whole))
                return p;
    }
    return -1;
}

/* Read the dialog's options into *ci / *whole and the search range into
 * *lo / *hi (the whole document, or the current selection for "Selected text"
 * scope). Returns the editor, or NULL if there's nothing to search. */
static ui_node* fr_setup(int* ci, int* whole, int* lo, int* hi)
{
    ui_node* ed = g_replace.target;
    if (!ed)
        return NULL;
    *ci = !ui_group_get_checked(g_replace.opts, 0);  /* Case sensitive off => ignore case */
    *whole = ui_group_get_checked(g_replace.opts, 1);
    int len = (int)strlen(ui_get_value(ed));
    *lo = 0;
    *hi = len;
    if (ui_select_get_selected(g_replace.scope) == 1)
    {  /* Selected text */
        int slo, shi;
        if (ui_editor_get_selection(ed, &slo, &shi))
        {
            *lo = slo;
            *hi = shi;
        }
    }
    return ed;
}

/* Copy `ed`'s currently selected text into `input`, if any is selected -
 * used to pre-fill Find/Replace's "Text to Find" field with the active
 * editor's selection when those dialogs open, so the common case (search for
 * the word you just selected) needs no typing. Leaves `input` untouched if
 * `ed` is NULL or nothing is selected. */
static int copy_selection(ui_node* ed, char* out, size_t out_size)
{
    int lo, hi;
    if (!ed || !ui_editor_get_selection(ed, &lo, &hi))
        return 0;
    const char* text = ui_get_value(ed);
    int len = hi - lo;
    if (len >= (int)out_size)
        len = (int)out_size - 1;
    memcpy(out, text + lo, len);
    out[len] = '\0';
    return 1;
}

static void fill_from_selection(ui_node* ed, ui_node* input)
{
    char buf[256];
    if (copy_selection(ed, buf, sizeof buf))
        ui_set_value(input, buf);
}

/* Find (and select) the next match in `ed` per the given Options/Direction/
 * Scope/Origin controls; returns 1 if one was found. A currently-selected
 * match is stepped over so repeated searches advance instead of re-finding
 * it. Shared by Find (below) - Replace does its own splice inline. */
static int fr_find_in_editor(ui_node* ed, const char* needle, ui_node* opts,
                             ui_node* dir, ui_node* scope, ui_node* origin)
{
    int nlen = (int)strlen(needle);
    if (!ed || nlen == 0)
        return 0;

    int ci = !ui_group_get_checked(opts, 0);   /* Case sensitive off => ignore case */
    int whole = ui_group_get_checked(opts, 1);
    int fwd = ui_select_get_selected(dir) == 0;
    int from_cursor = ui_select_get_selected(origin) == 0;

    const char* text = ui_get_value(ed);
    int len = (int)strlen(text);

    int lo = 0, hi = len;
    if (ui_select_get_selected(scope) == 1)
    {  /* Selected text */
        int slo, shi;
        if (ui_editor_get_selection(ed, &slo, &shi)) { lo = slo; hi = shi; }
    }

    int from;
    if (from_cursor)
    {
        int slo, shi;
        if (ui_editor_get_selection(ed, &slo, &shi))
            from = fwd ? shi : slo - 1;   /* step past the current match */
        else
            from = fwd ? ui_editor_get_cursor(ed) : ui_editor_get_cursor(ed) - 1;
    }
    else
    {
        from = fwd ? lo : hi - nlen;
    }

    int pos = fr_find(text, len, needle, nlen, from, fwd, ci, whole, lo, hi);
    if (pos < 0 && from_cursor)
    {
        /* Wrap around the scope: a search starting from a cursor near the end
         * (e.g. a freshly opened file, whose caret sits at the end) should
         * still find matches before it - the classic "search wrapped". */
        int wrap = fwd ? lo : hi - nlen;
        pos = fr_find(text, len, needle, nlen, wrap, fwd, ci, whole, lo, hi);
    }
    if (pos < 0)
        return 0;
    ui_editor_set_selection(ed, pos, pos + nlen);  /* highlight the match */
    return 1;
}

/* Search > Find... OK: locate and select the next match, or report failure
 * with a MessageBox (a good demo of ui_message_box). */
static void do_find(void)
{
    int found = 0;
    if (g_find.target)
        found = fr_find_in_editor(g_find.target, ui_get_value(g_find.input),
                                  g_find.opts, g_find.dir, g_find.scope, g_find.origin);
    ui_screen_close_modal(g_screen, g_find.modal);
    if (found)
    {
        g_goto_pending_focus = g_find.target;  /* focus so the selection shows */
    }
    else
    {
        ui_msgbox_button ok = { "   OK   ", 0 };
        ui_message_box(g_screen, "Find", "Search string not found.", &ok, 1);
    }
}

/* Search > Search Next (F3): repeat the last Find on the active editor window,
 * reusing whatever was typed/checked in the Find dialog - no dialog needed.
 * With nothing searched yet, just opens the Find dialog instead. */
static void do_find_next(void)
{
    ui_node* ed = editor_in_window(ui_screen_top_window(g_screen));
    const char* needle = ui_get_value(g_find.input);
    if (!ed || !needle[0])
    {
        g_find.target = ed;
        ui_screen_show_modal(g_screen, g_find.modal);
        return;
    }
    if (fr_find_in_editor(ed, needle, g_find.opts, g_find.dir, g_find.scope, g_find.origin))
    {
        g_goto_pending_focus = ed;
    }
    else
    {
        ui_msgbox_button ok = { "   OK   ", 0 };
        ui_message_box(g_screen, "Find", "Search string not found.", &ok, 1);
    }
}

/* OK: find the next occurrence (from the cursor or the scope start, in the
 * chosen direction) and replace it with the New Text, selecting the result. */
static void do_replace_one(void)
{
    int ci, whole, lo, hi;
    ui_node* ed = fr_setup(&ci, &whole, &lo, &hi);
    const char* needle = ui_get_value(g_replace.find);
    int nlen = (int)strlen(needle);
    if (ed && nlen > 0)
    {
        const char* newtext = ui_get_value(g_replace.new_);
        int rlen = (int)strlen(newtext);
        const char* text = ui_get_value(ed);
        int len = (int)strlen(text);

        int fwd = ui_select_get_selected(g_replace.dir) == 0;
        int from_cursor = ui_select_get_selected(g_replace.origin) == 0;
        int from = from_cursor ? ui_editor_get_cursor(ed) : (fwd ? lo : hi - nlen);

        int pos = fr_find(text, len, needle, nlen, from, fwd, ci, whole, lo, hi);
        if (pos < 0 && from_cursor)
        {  /* wrap around, like Find */
            int wrap = fwd ? lo : hi - nlen;
            pos = fr_find(text, len, needle, nlen, wrap, fwd, ci, whole, lo, hi);
        }
        if (pos >= 0)
        {
            char* buf = malloc((size_t)(len - nlen + rlen) + 1);
            if (buf)
            {
                memcpy(buf, text, (size_t)pos);
                memcpy(buf + pos, newtext, (size_t)rlen);
                memcpy(buf + pos + rlen, text + pos + nlen, (size_t)(len - pos - nlen));
                buf[len - nlen + rlen] = 0;
                ui_set_value(ed, buf);
                free(buf);
                ui_editor_set_selection(ed, pos, pos + rlen);  /* highlight the replacement */
                ui_set_dirty(ed, 1);
            }
        }
    }
    ui_screen_close_modal(g_screen, g_replace.modal);
    if (ed)
        g_goto_pending_focus = ed;  /* focus so the selection/caret shows */
}

/* Change All: replace every occurrence within the scope, then report the
 * count with a MessageBox. */
static void do_replace_all(void)
{
    int ci, whole, lo, hi;
    ui_node* ed = fr_setup(&ci, &whole, &lo, &hi);
    const char* needle = ui_get_value(g_replace.find);
    int nlen = (int)strlen(needle);
    int count = 0;
    if (ed && nlen > 0)
    {
        const char* newtext = ui_get_value(g_replace.new_);
        int rlen = (int)strlen(newtext);
        const char* text = ui_get_value(ed);
        int len = (int)strlen(text);

        for (int p = lo; p + nlen <= hi; )
        {
            if (fr_match_at(text, len, p, needle, nlen, ci, whole))
            {
                count++;
                p += nlen;
            }
            else
            {
                p++;
            }
        }
        if (count > 0)
        {
            char* buf = malloc((size_t)(len + count * (rlen - nlen)) + 1);
            if (buf)
            {
                int w = 0;
                for (int p = 0; p < len; )
                {
                    if (p >= lo && p + nlen <= hi &&
                        fr_match_at(text, len, p, needle, nlen, ci, whole))
                    {
                        memcpy(buf + w, newtext, (size_t)rlen);
                        w += rlen;
                        p += nlen;
                    }
                    else
                    {
                        buf[w++] = text[p++];
                    }
                }
                buf[w] = 0;
                ui_set_value(ed, buf);
                free(buf);
                ui_set_dirty(ed, 1);
            }
        }
    }
    ui_screen_close_modal(g_screen, g_replace.modal);

    char msg[64];
    snprintf(msg, sizeof msg, "%d occurrence(s) replaced.", count);
    ui_msgbox_button ok = { "   OK   ", 0 };
    ui_message_box(g_screen, "Replace", msg, &ok, 1);
}

/* Window > Tile: lay the open document windows out in a grid filling the
 * desktop area - never the docked Folder/Output panels (see ui_set_dock),
 * which stay right where they're pinned; "the desktop area" is whatever
 * ui_screen_desktop_rect() reports once those panels are subtracted. */
static void tile_windows(void)
{
    ui_node* wins[16];
    int n = 0;
    int total = ui_screen_window_count(g_screen);
    for (int i = 0; i < total && n < 16; i++)
    {
        ui_node* win = ui_child_at(ui_screen_window_at(g_screen, i), 0);
        if (win && ui_get_dock(win) == UI_DOCK_NONE)
            wins[n++] = win;
    }
    if (n == 0)
        return;

    int x0, y0, area_w, area_h;
    ui_screen_desktop_rect(g_screen, &x0, &y0, &area_w, &area_h);
    if (area_h < 5)
        area_h = 5;

    int cols = 1;
    while (cols * cols < n)  /* ceil(sqrt(n)) columns */
        cols++;
    int rows = (n + cols - 1) / cols;
    int cell_w = area_w / cols;
    int cell_h = area_h / rows;

    for (int i = 0; i < n; i++)
    {
        int c = i % cols, r = i / cols;
        int x = x0 + c * cell_w;
        int y = y0 + r * cell_h;
        int w = (c == cols - 1) ? (x0 + area_w) - x : cell_w;   /* last col: remainder */
        int h = (r == rows - 1) ? (y0 + area_h) - y : cell_h;   /* last row: remainder */
        ui_window_set_rect(wins[i], x, y, w, h);
    }
}

/* Window > Cascade: stack the open document windows (never the docked
 * Folder/Output panels - see ui_set_dock/tile_windows above) at a common
 * size within the desktop area, each offset down-right from the last so
 * every title bar stays visible. */
static void cascade_windows(void)
{
    ui_node* wins[16];
    int n = 0;
    int total = ui_screen_window_count(g_screen);
    for (int i = 0; i < total && n < 16; i++)
    {
        ui_node* win = ui_child_at(ui_screen_window_at(g_screen, i), 0);
        if (win && ui_get_dock(win) == UI_DOCK_NONE)
            wins[n++] = win;
    }
    if (n == 0)
        return;

    int x0, y0, area_w, area_h;
    ui_screen_desktop_rect(g_screen, &x0, &y0, &area_w, &area_h);
    if (area_h < 5)
        area_h = 5;

    int w = area_w * 3 / 4, h = area_h * 3 / 4;
    if (w < 20) w = area_w < 20 ? area_w : 20;
    if (h < 8) h = area_h < 8 ? area_h : 8;
    int max_off_x = area_w - w > 0 ? area_w - w : 1;
    int max_off_y = area_h - h > 0 ? area_h - h : 1;

    for (int i = 0; i < n; i++)
    {
        int x = x0 + (i * 2) % max_off_x;
        int y = y0 + (i % max_off_y);
        ui_window_set_rect(wins[i], x, y, w, h);
    }
}

/* Reads entire file `path` into a newly allocated null-terminated string.
 * Caller must free(). Returns NULL on failure (fopen or read error). */
static char* read_file_to_string(const char* path)
{
    FILE* f = fopen(path, "rb");
    if (!f)
        return NULL;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size < 0)
    {
        fclose(f);
        return NULL;
    }

    char* content = malloc((size_t)size + 1);
    if (!content)
    {
        fclose(f);
        return NULL;
    }

    size_t got = fread(content, 1, (size_t)size, f);
    content[got] = '\0';
    fclose(f);
    normalize_newlines(content);
    return content;
}

/* --- Tools > Find and Replace ----------------------------------------------
 * A dockable panel (see g_fr.window/g_fr.panel above), distinct from the
 * Search menu's Find/Replace modals (do_find/do_replace_one above): those
 * act on the active editor's own text directly, while this one gathers a
 * scope (Look in: current file/dir/include dir) and a file-type filter meant
 * for a project-wide search - it fills a find_replace_options struct and
 * hands it to do_find_replace(), which for now just reports it to the Output
 * window rather than actually performing the search/replace. */

typedef enum {
    FR_LOOKIN_CURRENT_FILE = 0,
    FR_LOOKIN_CURRENT_DIR,
    FR_LOOKIN_INCLUDE_DIR,
    FR_LOOKIN_PROJECT,  /* every file in the open project (g_project.files) -
                         * see fr_search_project/fr_replace_project */
} fr_look_in;

typedef enum {
    FR_FILETYPE_C = 0,
    FR_FILETYPE_H,
    FR_FILETYPE_C_H,
    FR_FILETYPE_MD,
    FR_FILETYPE_ALL,
} fr_file_type;

typedef struct {
    int mode;  /* 0 = Find, 1 = Replace */
    char find_text[256];
    char replace_text[256];
    int match_case;
    int match_whole_word;
    fr_look_in look_in;
    fr_file_type file_type;
} find_replace_options;

static const char* fr_look_in_label(fr_look_in v)
{
    switch (v)
    {
        case FR_LOOKIN_CURRENT_FILE: return "Current File";
        case FR_LOOKIN_CURRENT_DIR:  return "Current Dir";
        case FR_LOOKIN_INCLUDE_DIR:  return "Include Dir";
        case FR_LOOKIN_PROJECT:      return "Project";
    }
    return "?";
}

/* The actual wildcard mask fed to mask_matches() for a directory search.
 * "*" alone (not "*.*") is this codebase's own "everything" mask, same as
 * g_open_filters' "All Files (*.*)" row actually stores mask "*" -
 * wildcard_match() takes '*' literally, so it wouldn't match an
 * extensionless filename against a literal "*.*". */
static const char* fr_file_type_mask(fr_file_type v)
{
    switch (v)
    {
        case FR_FILETYPE_C:   return "*.c";
        case FR_FILETYPE_H:   return "*.h";
        case FR_FILETYPE_C_H: return "*.c;*.h";
        case FR_FILETYPE_MD:  return "*.md";
        case FR_FILETYPE_ALL: return "*";
    }
    return "*";
}

/* SGR color wrapped around the matched word in each printed source line
 * below - bright/bold yellow (matches COLOR_YELLOW, this file's existing
 * "pay attention to this" color for field labels), reset right after so it
 * doesn't bleed into the rest of the line. The Output editor is UI_SYNTAX_
 * VT100 (see app_init), which interprets embedded SGR codes like these
 * directly (see ansi_parse_sgr in ide_ui.c) instead of coloring by syntax. */
#define FR_MATCH_COLOR_ON  "\x1b[1;33m"
#define FR_MATCH_COLOR_OFF "\x1b[0m"

/* The shared scan behind both fr_search_current_file and fr_search_dir
 * below - a literal (non-regex), non-overlapping search over one already-
 * loaded file's text (`name` is just for the report - basename only, not a
 * full path), reusing the same match rules as Search > Find/Replace's own
 * dialogs (fr_match_at, above: match_case/match_whole_word, case-insensitive
 * by default). Appends, per match, a "name:line:col: <line, match colored>"
 * entry to out[*used..out_size) - the "file:line:col" prefix is the same
 * shape do_compile()'s diagnostics use, so double-clicking a result line in
 * the Output window (see output_goto_source) jumps straight to it; the rest
 * of that same line is the match's actual source line, with the matched
 * word itself wrapped in FR_MATCH_COLOR_ON/OFF. *used is advanced by
 * whatever was actually written (same clamp-not-overflow convention as
 * do_compile's own capture buffer). Returns this file's match count; the
 * caller (which knows whether it's summarizing one file or many) is the one
 * that prints a summary line, not this function. No-op (returns 0) if
 * find_text is empty. */
static int fr_search_text(const char* name, const char* text,
                          const find_replace_options* opts,
                          char* out, size_t out_size, size_t* used)
{
    int len = (int)strlen(text);
    const char* needle = opts->find_text;
    int nlen = (int)strlen(needle);
    if (nlen == 0)
        return 0;

    int ci = !opts->match_case;
    int whole = opts->match_whole_word;

    int line = 1, col = 1;    /* the position text[p] is currently at */
    int line_start = 0;       /* offset of the current line's first byte -
                                * kept in sync as p advances, so a match can
                                * print its whole source line without
                                * re-scanning backward for it */
    int count = 0;
    for (int p = 0; p + nlen <= len; )
    {
        if (fr_match_at(text, len, p, needle, nlen, ci, whole))
        {
            count++;
            if (*used < out_size)
            {
                /* The matched line's end - forward from the match, same as
                 * line_start is the line's already-known start. */
                int line_end = p + nlen;
                while (line_end < len && text[line_end] != '\n')
                    line_end++;
                int prefix_len = p - line_start;
                int suffix_len = line_end - (p + nlen);

                int n = snprintf(out + *used, out_size - *used,
                    "%s:%d:%d: %.*s" FR_MATCH_COLOR_ON
                    "%.*s" FR_MATCH_COLOR_OFF "%.*s\n",
                    name, line, col,
                    prefix_len, text + line_start,
                    nlen, text + p,
                    suffix_len, text + p + nlen);
                if (n > 0)
                    *used += (size_t)n;
            }
            /* Skip past the whole match (not just one char) so overlapping
             * hits aren't double-counted - e.g. "aa" in "aaa" is 1 match,
             * not 2 - tracking line/col/line_start across it same as the
             * fall-through single-char advance below. */
            for (int i = 0; i < nlen; i++, p++)
            {
                if (text[p] == '\n') { line++; col = 1; line_start = p + 1; }
                else col++;
            }
        }
        else
        {
            if (text[p] == '\n') { line++; col = 1; line_start = p + 1; }
            else col++;
            p++;
        }
    }
    return count;
}

/* Look in: Current File - searches the active document editor's own text
 * (see g_active_editor_window - NOT ui_screen_top_window(), which while the
 * panel itself is focused/frontmost would report the docked panel, not the
 * document behind it). Writes a "no file open"/"nothing to find" note
 * instead if there's no active document or an empty Find field, otherwise
 * fr_search_text()'s per-match lines followed by a one-file summary. Returns
 * the match count. */
static int fr_search_current_file(const find_replace_options* opts, char* out, size_t out_size)
{
    ui_node* win = g_active_editor_window;
    ui_node* ed = editor_in_window(win);
    if (!ed)
    {
        snprintf(out, out_size, "No file is open to search.\n");
        return 0;
    }
    if (opts->find_text[0] == '\0')
    {
        snprintf(out, out_size, "Nothing to find - the Find field is empty.\n");
        return 0;
    }

    /* Just the filename in the report - basename_of(), same as every other
     * title/listing in this file (folder_window_refresh, open_dialog, ...) -
     * not the full path ui_get_path() carries. */
    const char* name = basename_of(ui_get_path(win));
    const char* text = ui_get_value(ed);

    size_t used = 0;
    int count = fr_search_text(name, text, opts, out, out_size, &used);

    if (used < out_size)
    {
        if (count == 0)
            snprintf(out + used, out_size - used,
                "\"%s\" not found in %s.\n", opts->find_text, name);
        else
            snprintf(out + used, out_size - used,
                "\n%d occurrence(s) of \"%s\" in %s.\n", count, opts->find_text, name);
    }
    return count;
}

/* Look in: Current Dir - fr_search_text() over every file in the active
 * document's own directory (dirname() of its path, from fs.h/fs.c) that
 * matches the File Types filter (fr_file_type_mask, via mask_matches - same
 * matcher the Folder panel/Open dialog use) - one level only, not recursive,
 * same shallow scope the Folder panel itself browses (see folder_window_
 * refresh/populate_listbox_from_dir). Walks the directory with fs.h's
 * opendir/readdir/closedir (the portable wrapper fs.c provides on Windows,
 * the real POSIX ones elsewhere) rather than ui_list_dir() - that one's a
 * fixed-capacity snapshot meant for a listbox, not a streaming scan, and
 * reading+searching one file at a time here needs no such cap.
 *
 * The one-directory scan itself, shared with fr_search_include_dirs:
 * appends to out[*used..], counts into *files_searched, and returns the
 * match count - or -1 if `dir` can't be opened. `full_names` reports each
 * file by its full path instead of its bare name, for directories the
 * Output window's double-click (output_goto_source) wouldn't otherwise
 * think of looking in. */
static int fr_search_dir_files(const char* dir, int full_names, const find_replace_options* opts,
                               char* out, size_t out_size, size_t* used, int* files_searched)
{
    DIR* d = opendir(dir);
    if (!d)
        return -1;

    const char* mask = fr_file_type_mask(opts->file_type);
    int total = 0;
    struct dirent* de;
    while ((de = readdir(d)) != NULL)
    {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;
        if (de->d_type & DT_DIR)
            continue;  /* one level only - see comment above */
        if (!mask_matches(mask, de->d_name))
            continue;

        char filepath[1024];
        path_join(filepath, sizeof filepath, dir, de->d_name);
        char* content = read_file_to_string(filepath);
        if (!content)
            continue;

        (*files_searched)++;
        total += fr_search_text(full_names ? filepath : de->d_name, content, opts, out, out_size, used);
        free(content);
    }
    closedir(d);
    return total;
}

/* Returns the total match count across every file searched. */
static int fr_search_dir(const find_replace_options* opts, char* out, size_t out_size)
{
    ui_node* win = g_active_editor_window;
    const char* path = win ? ui_get_path(win) : "";
    if (!win || !path[0])
    {
        snprintf(out, out_size, "No file is open, so there's no directory to search.\n");
        return 0;
    }
    if (opts->find_text[0] == '\0')
    {
        snprintf(out, out_size, "Nothing to find - the Find field is empty.\n");
        return 0;
    }

    char dir[1024];
    strncpy(dir, path, sizeof dir - 1);
    dir[sizeof dir - 1] = 0;
    dirname(dir);  /* fs.c: strips the last path component in place */
    if (!dir[0])
        strcpy(dir, ".");

    size_t used = 0;
    int files_searched = 0;
    int total = fr_search_dir_files(dir, 0, opts, out, out_size, &used, &files_searched);
    if (total < 0)
    {
        snprintf(out, out_size, "Could not open directory: %s\n", dir);
        return 0;
    }

    if (used < out_size)
    {
        if (total == 0)
            snprintf(out + used, out_size - used,
                "\"%s\" not found in %d file(s) in %s.\n", opts->find_text, files_searched, dir);
        else
            snprintf(out + used, out_size - used,
                "\n%d occurrence(s) of \"%s\" in %d file(s) in %s.\n",
                total, opts->find_text, files_searched, dir);
    }
    return total;
}

/* Look in: Include Dir - fr_search_dir_files() over the same include
 * directories the compiler searches, in the same order: cake's own
 * built-in <exe dir>/include first (see preprocessor_load_config in
 * tokenizer.c - never listed in cake.json), then g_include_dirs, the
 * "include_dirs" of cake.json (File > "System Directories..."). One level
 * each, like Current Dir. Results carry full paths: these directories are
 * nowhere the Output window's double-click would look on its own. A
 * directory that can't be opened is noted and skipped rather than ending
 * the search. */
static int fr_search_include_dirs(const find_replace_options* opts, char* out, size_t out_size)
{
    if (opts->find_text[0] == '\0')
    {
        snprintf(out, out_size, "Nothing to find - the Find field is empty.\n");
        return 0;
    }

    /* get_self_path()'s return value means different things per platform
     * (fs.c), so success is judged by the path itself, as tokenizer.c does. */
    char builtin_dir[FS_MAX_PATH] = { 0 };
    get_self_path(builtin_dir, sizeof builtin_dir - sizeof "/include");
    if (builtin_dir[0])
    {
        dirname(builtin_dir);
        strcat(builtin_dir, "/include");
    }

    size_t used = 0;
    int total = 0, files_searched = 0, dirs_searched = 0;
    for (int i = builtin_dir[0] ? -1 : 0; i < g_include_count; i++)
    {
        const char* dir = i < 0 ? builtin_dir : g_include_dirs[i];
        int n = fr_search_dir_files(dir, 1, opts, out, out_size, &used, &files_searched);
        if (n >= 0)
        {
            total += n;
            dirs_searched++;
        }
        else if (used < out_size)
        {
            int w = snprintf(out + used, out_size - used,
                "Could not open directory: %s\n", dir);
            if (w > 0)
                used += (size_t)w;
        }
    }

    if (used < out_size)
    {
        if (total == 0)
            snprintf(out + used, out_size - used,
                "\"%s\" not found in %d file(s) in %d include directories.\n",
                opts->find_text, files_searched, dirs_searched);
        else
            snprintf(out + used, out_size - used,
                "\n%d occurrence(s) of \"%s\" in %d file(s) in %d include directories.\n",
                total, opts->find_text, files_searched, dirs_searched);
    }
    return total;
}

/* Builds a new heap string with every match of opts->find_text in `text`
 * replaced by opts->replace_text - same match rules and non-overlapping walk
 * as fr_search_text/fr_match_at (match_case/match_whole_word), and the same
 * two-pass count-then-build approach Search > Replace's own Change All
 * (do_replace_all, above) already uses, generalized to work on any text
 * instead of always the active editor's. Writes the result through
 * *out_text (caller's to free()) and returns the replacement count; leaves
 * *out_text NULL and returns 0 if find_text is empty or there was nothing to
 * replace, so a caller can just skip the file/editor untouched rather than
 * writing back an identical copy. */
static int fr_replace_text(const char* text, const find_replace_options* opts, char** out_text)
{
    *out_text = NULL;
    const char* needle = opts->find_text;
    int nlen = (int)strlen(needle);
    if (nlen == 0)
        return 0;
    const char* repl = opts->replace_text;
    int rlen = (int)strlen(repl);
    int len = (int)strlen(text);
    int ci = !opts->match_case;
    int whole = opts->match_whole_word;

    int count = 0;
    for (int p = 0; p + nlen <= len; )
    {
        if (fr_match_at(text, len, p, needle, nlen, ci, whole))
        {
            count++;
            p += nlen;
        }
        else
        {
            p++;
        }
    }
    if (count == 0)
        return 0;

    char* buf = malloc((size_t)(len + count * (rlen - nlen)) + 1);
    if (!buf)
        return 0;

    int w = 0;
    for (int p = 0; p < len; )
    {
        if (p + nlen <= len && fr_match_at(text, len, p, needle, nlen, ci, whole))
        {
            memcpy(buf + w, repl, (size_t)rlen);
            w += rlen;
            p += nlen;
        }
        else
        {
            buf[w++] = text[p++];
        }
    }
    buf[w] = 0;

    *out_text = buf;
    return count;
}

/* Look in: Current File, Replace mode - replaces every match in the active
 * document editor's own text (fr_replace_text above), same never-touch-disk-
 * directly philosophy as Search > Replace's Change All: only the editor's
 * in-memory content changes (ui_set_value + ui_set_dirty) - the user still
 * saves it themselves (File > Save/Save all) same as any other edit. Returns
 * the replacement count. */
static int fr_replace_current_file(const find_replace_options* opts, char* out, size_t out_size)
{
    ui_node* win = g_active_editor_window;
    ui_node* ed = editor_in_window(win);
    if (!ed)
    {
        snprintf(out, out_size, "No file is open to replace in.\n");
        return 0;
    }
    if (opts->find_text[0] == '\0')
    {
        snprintf(out, out_size, "Nothing to find - the Find field is empty.\n");
        return 0;
    }

    const char* name = basename_of(ui_get_path(win));
    const char* text = ui_get_value(ed);

    char* new_text = NULL;
    int count = fr_replace_text(text, opts, &new_text);
    if (count == 0)
    {
        snprintf(out, out_size, "\"%s\" not found in %s - nothing replaced.\n",
            opts->find_text, name);
        return 0;
    }

    ui_set_value(ed, new_text);
    free(new_text);
    ui_set_dirty(ed, 1);

    snprintf(out, out_size,
        "%d replacement(s) of \"%s\" with \"%s\" in %s.\n"
        "(Not saved - use File > Save to write it to disk.)\n",
        count, opts->find_text, opts->replace_text, name);
    return count;
}

/* Look in: Current Dir, Replace mode - fr_replace_text() over every file in
 * the active document's own directory matching the File Types filter, same
 * shallow one-level scope as fr_search_dir above (see its own comment for
 * why opendir/readdir/closedir over ui_list_dir()). Same never-touch-disk-
 * directly rule as fr_replace_current_file: a file already open gets its
 * editor's content replaced in place (ui_set_value + ui_set_dirty); a file
 * that wasn't open gets a brand new editor window (make_editor_window) with
 * the replacement already applied and marked dirty, so it shows up ready to
 * review rather than being silently rewritten on disk mid-scan - File >
 * Save all (EVT_FILE_SAVEALL) already saves every open window, dirty or
 * not, so that's the natural next step once the results look right. Returns
 * the total replacement count across every file. */
static int fr_replace_dir(const find_replace_options* opts, char* out, size_t out_size)
{
    ui_node* win = g_active_editor_window;
    const char* path = win ? ui_get_path(win) : "";
    if (!win || !path[0])
    {
        snprintf(out, out_size, "No file is open, so there's no directory to replace in.\n");
        return 0;
    }
    if (opts->find_text[0] == '\0')
    {
        snprintf(out, out_size, "Nothing to find - the Find field is empty.\n");
        return 0;
    }

    char dir[1024];
    strncpy(dir, path, sizeof dir - 1);
    dir[sizeof dir - 1] = 0;
    dirname(dir);  /* fs.c: strips the last path component in place */
    if (!dir[0])
        strcpy(dir, ".");

    DIR* d = opendir(dir);
    if (!d)
    {
        snprintf(out, out_size, "Could not open directory: %s\n", dir);
        return 0;
    }

    const char* mask = fr_file_type_mask(opts->file_type);
    size_t used = 0;
    int total = 0, files_changed = 0, files_searched = 0;
    struct dirent* de;
    while ((de = readdir(d)) != NULL)
    {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;
        if (de->d_type & DT_DIR)
            continue;  /* one level only - see fr_search_dir */
        if (!mask_matches(mask, de->d_name))
            continue;

        char filepath[1024];
        path_join(filepath, sizeof filepath, dir, de->d_name);

        /* Prefer an already-open window's live (possibly unsaved) content
         * over what's on disk - replacing against a stale copy would lose
         * whatever the user already typed there. */
        ui_node* existing = find_open_window(filepath);
        char* loaded = NULL;
        const char* content;
        if (existing)
            content = ui_get_value(editor_in_window(existing));
        else
            content = loaded = read_file_to_string(filepath);
        if (!content)
            continue;

        files_searched++;
        char* new_text = NULL;
        int count = fr_replace_text(content, opts, &new_text);
        if (count > 0)
        {
            files_changed++;
            total += count;
            if (used < out_size)
            {
                int n = snprintf(out + used, out_size - used,
                    "%s: %d replacement(s)\n", de->d_name, count);
                if (n > 0)
                    used += (size_t)n;
            }

            if (existing)
            {
                ui_node* ed = editor_in_window(existing);
                ui_set_value(ed, new_text);
                ui_set_dirty(ed, 1);
            }
            else
            {
                char title[320];
                snprintf(title, sizeof title, " %s ", de->d_name);
                ui_node* w = make_editor_window(g_root, g_new_count++, title, new_text, filepath);
                ui_node* ed = editor_in_window(w);
                if (ed)
                    ui_set_dirty(ed, 1);
                ui_screen_show_window(g_screen, w);
            }
            free(new_text);
        }
        free(loaded);
    }
    closedir(d);

    if (used < out_size)
    {
        if (total == 0)
            snprintf(out + used, out_size - used,
                "\"%s\" not found in %d file(s) in %s - nothing replaced.\n",
                opts->find_text, files_searched, dir);
        else
            snprintf(out + used, out_size - used,
                "\n%d replacement(s) of \"%s\" with \"%s\" in %d file(s) (%d changed) in %s.\n"
                "(Not saved - use File > Save all to write them to disk.)\n",
                total, opts->find_text, opts->replace_text, files_searched, files_changed, dir);
    }
    return total;
}

/* Look in: Project - fr_search_text() over every file in the open project
 * (g_project.files, resolved through project_abs_path()) that matches the
 * File Types filter, instead of scanning a single directory - same
 * prefer-the-live-editor-over-disk rule as fr_search_dir. */
static int fr_search_project(const find_replace_options* opts, char* out, size_t out_size)
{
    if (!project_is_open())
    {
        snprintf(out, out_size, "No project is open, so there's nothing to search.\n");
        return 0;
    }
    if (opts->find_text[0] == '\0')
    {
        snprintf(out, out_size, "Nothing to find - the Find field is empty.\n");
        return 0;
    }

    const char* mask = fr_file_type_mask(opts->file_type);
    size_t used = 0;
    int total = 0, files_searched = 0;
    for (int i = 0; i < g_project.file_count; i++)
    {
        const char* entry = g_project.files[i];
        if (!mask_matches(mask, basename_of(entry)))
            continue;

        char abs_path[1024];
        project_abs_path(entry, abs_path, sizeof abs_path);

        ui_node* existing = find_open_window(abs_path);
        char* loaded = NULL;
        const char* content;
        if (existing)
            content = ui_get_value(editor_in_window(existing));
        else
            content = loaded = read_file_to_string(abs_path);
        if (!content)
            continue;

        files_searched++;
        total += fr_search_text(entry, content, opts, out, out_size, &used);
        free(loaded);
    }

    if (used < out_size)
    {
        if (total == 0)
            snprintf(out + used, out_size - used,
                "\"%s\" not found in %d file(s) in project \"%s\".\n",
                opts->find_text, files_searched, g_project.name);
        else
            snprintf(out + used, out_size - used,
                "\n%d occurrence(s) of \"%s\" in %d file(s) in project \"%s\".\n",
                total, opts->find_text, files_searched, g_project.name);
    }
    return total;
}

/* Look in: Project, Replace mode - fr_replace_text() over the same file list
 * as fr_search_project() above, same never-touch-disk-directly rule as
 * fr_replace_dir: an already-open window's editor is updated in place and
 * marked dirty; a file that wasn't open gets a brand new editor window with
 * the replacement already applied, also marked dirty rather than written
 * straight to disk - File > Save all is the natural next step. */
static int fr_replace_project(const find_replace_options* opts, char* out, size_t out_size)
{
    if (!project_is_open())
    {
        snprintf(out, out_size, "No project is open, so there's nothing to replace in.\n");
        return 0;
    }
    if (opts->find_text[0] == '\0')
    {
        snprintf(out, out_size, "Nothing to find - the Find field is empty.\n");
        return 0;
    }

    const char* mask = fr_file_type_mask(opts->file_type);
    size_t used = 0;
    int total = 0, files_changed = 0, files_searched = 0;
    for (int i = 0; i < g_project.file_count; i++)
    {
        const char* entry = g_project.files[i];
        if (!mask_matches(mask, basename_of(entry)))
            continue;

        char abs_path[1024];
        project_abs_path(entry, abs_path, sizeof abs_path);

        ui_node* existing = find_open_window(abs_path);
        char* loaded = NULL;
        const char* content;
        if (existing)
            content = ui_get_value(editor_in_window(existing));
        else
            content = loaded = read_file_to_string(abs_path);
        if (!content)
            continue;

        files_searched++;
        char* new_text = NULL;
        int count = fr_replace_text(content, opts, &new_text);
        if (count > 0)
        {
            files_changed++;
            total += count;
            if (used < out_size)
            {
                int n = snprintf(out + used, out_size - used,
                    "%s: %d replacement(s)\n", entry, count);
                if (n > 0)
                    used += (size_t)n;
            }

            if (existing)
            {
                ui_node* ed = editor_in_window(existing);
                ui_set_value(ed, new_text);
                ui_set_dirty(ed, 1);
            }
            else
            {
                char title[320];
                snprintf(title, sizeof title, " %s ", basename_of(entry));
                ui_node* w = make_editor_window(g_root, g_new_count++, title, new_text, abs_path);
                ui_node* ed = editor_in_window(w);
                if (ed)
                    ui_set_dirty(ed, 1);
                ui_screen_show_window(g_screen, w);
            }
            free(new_text);
        }
        free(loaded);
    }

    if (used < out_size)
    {
        if (total == 0)
            snprintf(out + used, out_size - used,
                "\"%s\" not found in %d file(s) in project \"%s\" - nothing replaced.\n",
                opts->find_text, files_searched, g_project.name);
        else
            snprintf(out + used, out_size - used,
                "\n%d replacement(s) of \"%s\" with \"%s\" in %d file(s) (%d changed) in project \"%s\".\n"
                "(Not saved - use File > Save all to write them to disk.)\n",
                total, opts->find_text, opts->replace_text, files_searched, files_changed, g_project.name);
    }
    return total;
}

/* Search or Replace (per opts->mode) for Look in: Current File, Current
 * Dir, and Project - see fr_search_current_file/fr_replace_current_file,
 * fr_search_dir/fr_replace_dir, and fr_search_project/fr_replace_project
 * above. Include Dir is search-only (fr_search_include_dirs): those are the
 * system headers, not files to rewrite. Reports every field of `opts` first (the
 * exact options the panel handed off), then the search/replace results, to
 * the Output window - same place do_compile() reports its own results. */
static void do_find_replace(const find_replace_options* opts)
{
    static char msg[CAPTURE_BUF_SIZE];

    if (opts->look_in == FR_LOOKIN_CURRENT_FILE)
    {
        if (opts->mode)
            fr_replace_current_file(opts, msg, sizeof msg);
        else
            fr_search_current_file(opts, msg, sizeof msg);
    }
    else if (opts->look_in == FR_LOOKIN_CURRENT_DIR)
    {
        if (opts->mode)
            fr_replace_dir(opts, msg, sizeof msg);
        else
            fr_search_dir(opts, msg, sizeof msg);
    }
    else if (opts->look_in == FR_LOOKIN_PROJECT)
    {
        if (opts->mode)
            fr_replace_project(opts, msg, sizeof msg);
        else
            fr_search_project(opts, msg, sizeof msg);
    }
    else if (opts->look_in == FR_LOOKIN_INCLUDE_DIR)
    {
        if (opts->mode)
            snprintf(msg, sizeof msg,
                "Replace isn't available for \"%s\" - those are the system headers. "
                "Use Find to search them.\n", fr_look_in_label(opts->look_in));
        else
            fr_search_include_dirs(opts, msg, sizeof msg);
    }

    ui_set_value(g_output_editor, msg);
    ui_screen_show_window(g_screen, g_output_window);
}

/* Search > "Go to Definition" (F12): not a real semantic lookup yet (see
 * ide_lsp.h's lsp_text_document_definition() - still a //TODO stub) - so in
 * the meantime this finds the word under the caret the plain-text way,
 * through the same machinery Tools > Find and Replace uses (fr_search_dir()/
 * fr_search_project() above), with Match case/Match whole word both forced
 * on (a bare substring search for a short identifier would drown in
 * unrelated hits) and the scope fixed to *.c;*.h files - across the whole
 * open project when there is one (so a definition in another project file
 * is actually found), else just the active file's own directory, same as
 * before projects existed. No panel is shown - like do_find_replace() itself,
 * this just writes straight to the Output window and raises it. */
static void do_goto_definition(void)
{
    ui_node* win = g_active_editor_window;
    ui_node* ed = win ? editor_in_window(win) : NULL;
    if (!ed)
        return;

    const char* text = ui_get_value(ed);
    int cursor = ui_editor_get_cursor(ed);

    find_replace_options opts = { 0 };
    if (!word_at_cursor(text, (int)strlen(text), cursor, opts.find_text, (int)sizeof opts.find_text))
    {
        ui_msgbox_button ok = { "   OK   ", 0 };
        ui_message_box(g_screen, "Go to Definition", "No identifier under the caret.", &ok, 1);
        return;
    }

    opts.mode = 0;  /* Find, not Replace */
    opts.match_case = 1;
    opts.match_whole_word = 1;
    opts.look_in = project_is_open() ? FR_LOOKIN_PROJECT : FR_LOOKIN_CURRENT_DIR;
    opts.file_type = FR_FILETYPE_C_H;

    do_find_replace(&opts);
}

/* Copies whatever's currently sitting in the panel's live widgets back into
 * the g_fr_* fields - called right before fr_rebuild_content() destroys and
 * recreates those widgets (mode toggle), and right before do_find_replace()
 * reads a snapshot of them (Find/Replace button), so neither loses what the
 * user typed/picked. g_fr.replace_input/g_fr.replace_btn are NULL while in
 * Find mode (never built - see fr_rebuild_content), so those are skipped
 * rather than dereferenced. */
static void fr_sync_from_widgets(void)
{
    if (g_fr.find_input)
        snprintf(g_fr.find_text, sizeof g_fr.find_text, "%s", ui_get_value(g_fr.find_input));
    if (g_fr.replace_input)
        snprintf(g_fr.replace_text, sizeof g_fr.replace_text, "%s", ui_get_value(g_fr.replace_input));
    if (g_fr.opts)
    {
        g_fr.match_case = ui_group_get_checked(g_fr.opts, 0);
        g_fr.match_word = ui_group_get_checked(g_fr.opts, 1);
    }
    if (g_fr.lookin)
        g_fr.look_in = ui_select_get_selected(g_fr.lookin);
    if (g_fr.filetypes)
        g_fr.file_type = ui_select_get_selected(g_fr.filetypes);
}

/* (Re)builds the panel's content from scratch to match g_fr.mode - clearing
 * every child first (same clear-then-repopulate pattern as
 * populate_listbox_from_dir above). This is how "the window changes itself"
 * between Find and Replace: in Find mode there's no Replace field/button at
 * all (not just disabled - never attached to the tree, so never drawn or
 * hit-tested); in Replace mode both appear and everything below shifts down
 * to make room. Values are restored from the g_fr_* fields (see
 * fr_sync_from_widgets) so toggling modes never loses what was typed/picked. */
static void fr_rebuild_content(void)
{
    if (!g_fr.panel)
        return;

    /* The controls are about to be freed - remember which one had focus (by
     * role, not pointer) and hand focus to its replacement at the end, so
     * e.g. clicking "Mode" keeps focus on the new Mode button instead of
     * leaving it on a freed node. */
    ui_node* focused = ui_screen_focused(g_screen);
    ui_node** roles[] = { &g_fr.find_input, &g_fr.replace_input, &g_fr.opts, &g_fr.lookin,
                          &g_fr.filetypes, &g_fr.find_btn, &g_fr.replace_btn, &g_fr.mode_btn };
    int focus_role = -1;
    for (int i = 0; i < (int)(sizeof roles / sizeof roles[0]); i++)
        if (focused && *roles[i] == focused)
            focus_role = i;
    if (focus_role >= 0)
        ui_screen_focus(g_screen, NULL);

    while (ui_child_count(g_fr.panel) > 0)
    {
        ui_node* c = ui_child_at(g_fr.panel, 0);
        ui_remove_child(g_fr.panel, c);
        ui_node_free(c);
    }

    const ui_theme* theme = ui_get_theme();
    int px, py, pw;
    ui_get_rect(g_fr.panel, &px, &py, &pw, NULL);
    /* +2/-4 rather than the flush +1/-2 an interior rect would normally use
     * (e.g. the Folder window's listbox) - leaves a 1-cell margin between
     * the border and the controls on all of the left/top/right (matching
     * every other dialog in this file, which starts its first control 2
     * cells in from the window's own x/y rather than 1), and symmetrically
     * on the right (-4 instead of -3: one cell for the border, one for this
     * same margin, on each side). */
    int cx = px + 2, cw = pw - 4;
    int cy = py + 2;

    add_text(g_fr.panel, cx, cy, "Find:", theme->label_fg, theme->window_bg);
    cy += 1;
    g_fr.find_input = add_input(g_fr.panel, cx, cy, cw, g_fr.find_text);
    ui_set_id(g_fr.find_input, EVT_FR_FIND_BTN);  /* Enter in the field == clicking Find */
    cy += 2;

    if (g_fr.mode)
    {
        add_text(g_fr.panel, cx, cy, "Replace:", theme->label_fg, theme->window_bg);
        cy += 1;
        g_fr.replace_input = add_input(g_fr.panel, cx, cy, cw, g_fr.replace_text);
        cy += 2;
    }
    else
    {
        g_fr.replace_input = NULL;
    }

    g_fr.opts = add_group(g_fr.panel, cx, cy, cw, 2, 1);
    add_group_item(g_fr.opts, "Match case");
    add_group_item(g_fr.opts, "Match whole word");
    ui_group_set_checked(g_fr.opts, 0, g_fr.match_case);
    ui_group_set_checked(g_fr.opts, 1, g_fr.match_word);
    cy += 3;

    add_text(g_fr.panel, cx, cy, "Look in:", theme->label_fg, theme->window_bg);
    cy += 1;
    g_fr.lookin = add_group(g_fr.panel, cx, cy, cw, 4, 0);
    ui_set_help(g_fr.lookin,
                "Where to search", "# Where to search\n"
                "\n"
                "Current Dir and Include Dir search one level only - subdirectories are not entered.");
    ui_set_help(add_group_item(g_fr.lookin, "Current File"),
                "Current File: the active document", "## Current File\n\nthe active document\n"
                "\n"
                "Searches the text in its editor, including unsaved changes.");
    ui_set_help(add_group_item(g_fr.lookin, "Current Dir"),
                "Current Dir: every file in the active document's folder", "## Current Dir\n\nevery file in the active document's folder\n"
                "\n"
                "Only files matching **File Types**. A file that is open in an editor is searched in its editor, unsaved changes included; the others are read from disk.");
    ui_set_help(add_group_item(g_fr.lookin, "Include Dir"),
                "Include Dir: the directories the compiler searches for headers", "## Include Dir\n\nthe directories the compiler searches for headers\n"
                "\n"
                "Cake's own `include` folder next to the executable first, then the `include_dirs` of `cake.json` (**File > System Directories...**), in that order.\n"
                "\n"
                "Search only - **Replace** never rewrites system headers.");
    ui_set_help(add_group_item(g_fr.lookin, "Project"),
                "Project: every file in the open project", "## Project\n\nevery file in the open project\n"
                "\n"
                "Only files matching **File Types**. Open files are searched in their editor, unsaved changes included.");
    ui_select_set_selected(g_fr.lookin, g_fr.look_in);
    cy += 5;

    add_text(g_fr.panel, cx, cy, "File Types:", theme->label_fg, theme->window_bg);
    cy += 1;
    g_fr.filetypes = add_select(g_fr.panel, cx, cy, cw);
    add_select_item(g_fr.filetypes, EVT_FR_FILETYPE_BASE + 0, "*.c");
    add_select_item(g_fr.filetypes, EVT_FR_FILETYPE_BASE + 1, "*.h");
    add_select_item(g_fr.filetypes, EVT_FR_FILETYPE_BASE + 2, "*.c;*.h");
    add_select_item(g_fr.filetypes, EVT_FR_FILETYPE_BASE + 3, "*.md");
    add_select_item(g_fr.filetypes, EVT_FR_FILETYPE_BASE + 4, "*.*");
    ui_select_set_selected(g_fr.filetypes, g_fr.file_type);
    cy += 2;

    g_fr.find_btn = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(g_fr.find_btn, EVT_FR_FIND_BTN);
    ui_set_label(g_fr.find_btn, "  Find  ");
    if (g_fr.mode)
    {
        int half = (cw - 1) / 2;
        ui_set_rect(g_fr.find_btn, cx, cy, half, 1);
        ui_append_child(g_fr.panel, g_fr.find_btn);

        g_fr.replace_btn = ui_create_element(UI_TAG_BUTTON);
        ui_set_id(g_fr.replace_btn, EVT_FR_REPLACE_BTN);
        ui_set_label(g_fr.replace_btn, " Replace ");
        ui_set_rect(g_fr.replace_btn, cx + half + 1, cy, cw - half - 1, 1);
        ui_append_child(g_fr.panel, g_fr.replace_btn);
    }
    else
    {
        ui_set_rect(g_fr.find_btn, cx, cy, cw, 1);
        ui_append_child(g_fr.panel, g_fr.find_btn);
        g_fr.replace_btn = NULL;
    }
    cy += 2;

    /* Mode toggle - last control, below Find/Replace rather than above them
     * (per request), so the buttons that act don't shift position depending
     * on how many fields are showing above the toggle. */
    g_fr.mode_btn = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(g_fr.mode_btn, EVT_FR_MODE);
    ui_set_rect(g_fr.mode_btn, cx, cy, cw, 1);
    ui_set_label(g_fr.mode_btn, g_fr.mode ? "Mode: Replace" : "Mode: Find");
    ui_append_child(g_fr.panel, g_fr.mode_btn);

    /* A role missing in the new mode (Replace field/button, back in Find
     * mode) is NULL here, which just leaves nothing focused. */
    if (focus_role >= 0)
        ui_screen_focus(g_screen, *roles[focus_role]);
}

/* Shared by Edit > Stringify/To Upper/To Lower: the active document's
 * <editor> and its current selection, or NULL (leaving *out_lo / *out_hi
 * untouched) if there's no active document or nothing is selected in it -
 * same "quietly do nothing" behavior Cut/Copy already have for an empty
 * selection (see editor_copy/editor_cut in ide_ui.c). Deliberately targets
 * g_active_editor_window rather than whatever currently has keyboard focus
 * (there's no public way to even ask a ui_node its tag - see
 * editor_in_window's own doc comment - so "the active document", the same
 * target Save/Save As use, is the one thing these three can reliably act
 * on regardless of, say, the Find and Replace panel's own input having
 * focus at the moment). */
static ui_node* edit_selection_target(int* out_lo, int* out_hi)
{
    ui_node* ed = editor_in_window(g_active_editor_window);
    if (!ed)
        return NULL;
    if (!ui_editor_get_selection(ed, out_lo, out_hi))
        return NULL;
    return ed;
}

/* Replaces `ed`'s [lo, hi) selection with `new_text` (a malloc'd, NUL-
 * terminated buffer - always freed here, regardless of outcome) and leaves
 * the replacement selected, so a transform's result is immediately visible
 * and, if it's not what was wanted, typing or Ctrl+X over it doesn't require
 * re-finding it first. Goes through ui_editor_replace_selection() (not a
 * raw ui_set_value() rebuild like do_replace_all()/fr_replace_current_file()
 * use) so the edit is a single Ctrl+Z away, matching real typing/paste. */
static void edit_replace_selection(ui_node* ed, int lo, int hi, char* new_text, int new_len)
{
    (void)new_len;
    ui_editor_replace_selection(ed, lo, hi, new_text);
    free(new_text);
}

static int is_c_string_safe_ascii(unsigned char b)
{
    return b >= 0x20 && b < 0x7F && b != '"' && b != '\\';
}

/* Edit > Stringify: the C string-literal equivalent of `sel[0, sel_len)`.
 * Splits on '\n' into one double-quoted literal per source line, with a
 * real line break between them in the OUTPUT too (valid C - adjacent string
 * literals concatenate into one), so the generated code's own shape still
 * shows the original text's line structure instead of burying every "\n"
 * escape in one giant line. The usual named escapes (\n \t \r \\ \") cover
 * the common cases; anything else unprintable, or any byte >= 0x80 (a UTF-8
 * continuation/lead byte - the editor's text is already UTF-8, so escaping
 * byte-for-byte reproduces the exact same UTF-8 sequence at runtime),
 * becomes a fixed-width 3-digit octal escape ("\ooo", always all 3 digits).
 * Deliberately not "\xHH": C's \x escape reads an UNBOUNDED run of hex
 * digits, so a following character that happens to look like one (0-9a-fA-f)
 * would silently get swallowed into it - \ooo reads at most 3 octal digits
 * by grammar, and zero-padding to exactly 3 every time means it always
 * consumes exactly that many, never bleeding into whatever comes next.
 * Returns a malloc'd, NUL-terminated buffer (or NULL if out of memory) and
 * writes its length (excluding the NUL) to *out_len. */
static char* stringify_text(const char* sel, int sel_len, int* out_len)
{
    /* Worst case is a lone '\n' expanding to `"` `\` `n` `"` NEWLINE `"` (6
     * bytes) - generous but simple, and freed right after use either way. */
    size_t cap = (size_t)sel_len * 6 + 32;
    char* out = malloc(cap);
    if (!out)
        return NULL;

    size_t o = 0;
    out[o++] = '"';
    for (int i = 0; i < sel_len; i++)
    {
        unsigned char b = (unsigned char)sel[i];
        if (b == '\n')
        {
            out[o++] = '\\'; out[o++] = 'n';
            out[o++] = '"'; out[o++] = '\n'; out[o++] = '"';
        }
        else if (b == '\t') { out[o++] = '\\'; out[o++] = 't'; }
        else if (b == '\r') { out[o++] = '\\'; out[o++] = 'r'; }
        else if (b == '"')  { out[o++] = '\\'; out[o++] = '"'; }
        else if (b == '\\') { out[o++] = '\\'; out[o++] = '\\'; }
        else if (is_c_string_safe_ascii(b))
        {
            out[o++] = (char)b;
        }
        else
        {
            out[o++] = '\\';
            out[o++] = (char)('0' + ((b >> 6) & 7));
            out[o++] = (char)('0' + ((b >> 3) & 7));
            out[o++] = (char)('0' + (b & 7));
        }
    }
    out[o++] = '"';
    out[o] = 0;

    *out_len = (int)o;
    return out;
}

static void do_edit_stringify(void)
{
    int lo, hi;
    ui_node* ed = edit_selection_target(&lo, &hi);
    if (!ed)
        return;
    const char* text = ui_get_value(ed);
    int out_len;
    char* out = stringify_text(text + lo, hi - lo, &out_len);
    if (out)
        edit_replace_selection(ed, lo, hi, out, out_len);
}

/* Edit > To Upper/To Lower: ASCII-only case folding (toupper()/tolower() on
 * each byte < 0x80) - a UTF-8 continuation/lead byte (>= 0x80) is always
 * outside 'A'-'Z'/'a'-'z' regardless of locale, so it passes through
 * unchanged rather than getting corrupted by a byte-wise case flip that
 * knows nothing about multi-byte sequences. Returns a malloc'd, NUL-
 * terminated buffer (or NULL if out of memory); *out_len is always
 * `sel_len` (case-folding never changes byte count). */
static char* case_transform_text(const char* sel, int sel_len, int* out_len, int upper)
{
    char* out = malloc((size_t)sel_len + 1);
    if (!out)
        return NULL;
    for (int i = 0; i < sel_len; i++)
    {
        unsigned char b = (unsigned char)sel[i];
        out[i] = (char)(upper ? toupper(b) : tolower(b));
    }
    out[sel_len] = 0;
    *out_len = sel_len;
    return out;
}

static void do_edit_case(int upper)
{
    int lo, hi;
    ui_node* ed = edit_selection_target(&lo, &hi);
    if (!ed)
        return;
    const char* text = ui_get_value(ed);
    int out_len;
    char* out = case_transform_text(text + lo, hi - lo, &out_len, upper);
    if (out)
        edit_replace_selection(ed, lo, hi, out, out_len);
}

/* Edit > Word Wrap...: a tiny growable byte buffer, used below to assemble
 * the reflowed text since - unlike Stringify/To Upper/To Lower - the output
 * length isn't a fixed function of the input length (wrapping can both add
 * newlines, when a long line splits, and remove them, when short lines
 * merge), so a single malloc(sel_len * k) worst-case estimate doesn't work
 * here. */
typedef struct { char* buf; size_t len; size_t cap; } wordwrap_buf;

static void wwbuf_init(wordwrap_buf* b)
{
    b->cap = 256;
    b->buf = malloc(b->cap);
    b->len = 0;
    if (b->buf)
        b->buf[0] = 0;
}

/* Grows `b` if needed, then appends `n` bytes from `text`. Silently gives up
 * (leaves `b->buf` NULL) on allocation failure - checked once at the end by
 * the caller rather than after every append. */
static void wwbuf_append(wordwrap_buf* b, const char* text, size_t n)
{
    if (!b->buf)
        return;
    if (b->len + n + 1 > b->cap)
    {
        size_t newcap = b->cap;
        while (b->len + n + 1 > newcap)
            newcap *= 2;
        char* nb = realloc(b->buf, newcap);
        if (!nb)
        {
            free(b->buf);
            b->buf = NULL;
            return;
        }
        b->buf = nb;
        b->cap = newcap;
    }
    memcpy(b->buf + b->len, text, n);
    b->len += n;
    b->buf[b->len] = 0;
}

static int wordwrap_is_space(char c)
{
    return c == ' ' || c == '\t' || c == '\r';
}

/* A line is blank if every byte in it is whitespace (an empty span, e.g. the
 * phantom trailing line a selection ending in '\n' splits into below,
 * counts as blank too). */
static int wordwrap_is_blank_line(const char* s, int len)
{
    for (int i = 0; i < len; i++)
        if (!wordwrap_is_space(s[i]))
            return 0;
    return 1;
}

/* Edit > Word Wrap...: reflows `sel[0, sel_len)` to `columns` columns, one
 * paragraph at a time. A "paragraph" is a run of consecutive non-blank
 * lines; blank lines (any line that's empty or all whitespace - including
 * the phantom line after a trailing '\n', which is what reproduces the
 * selection's trailing newline in the output) pass straight through
 * unchanged and reset the paragraph. Each paragraph's lines are re-split on
 * whitespace into words, then repacked greedily into new lines no wider
 * than `columns` (a single word longer than that still gets its own line
 * rather than being split mid-word) - all reindented to match the
 * paragraph's first line's own leading whitespace. When `justify` is set,
 * every line but a paragraph's last is padded with extra inter-word spaces
 * (distributed as evenly as possible, leftmost gaps getting the remainder)
 * so it reaches exactly `columns` wide - single-word lines are left alone
 * since there's no gap to pad. Returns a malloc'd, NUL-terminated buffer (or
 * NULL if out of memory); *out_len is the reflowed text's length (excluding
 * the NUL). */
static char* wordwrap_text(const char* sel, int sel_len, int columns, int justify, int* out_len)
{
    if (columns < 1)
        columns = 1;

    wordwrap_buf out;
    wwbuf_init(&out);
    int have_output = 0;  /* has at least one line already been emitted? -
                           * decides whether the next line needs a leading
                           * '\n' separator first. */

    int pos = 0;
    while (out.buf)
    {
        int line_start = pos;
        while (pos < sel_len && sel[pos] != '\n')
            pos++;
        int line_len = pos - line_start;
        const char* line = sel + line_start;
        int at_end = (pos >= sel_len);

        if (wordwrap_is_blank_line(line, line_len))
        {
            if (have_output)
                wwbuf_append(&out, "\n", 1);
            have_output = 1;
            /* Blank lines are emitted empty (trailing whitespace on an
             * otherwise-blank line is never meaningful) rather than copied
             * verbatim. */
        }
        else
        {
            /* Leading whitespace of the paragraph's first line becomes
             * every wrapped line's indent. */
            int indent_len = 0;
            while (indent_len < line_len && wordwrap_is_space(line[indent_len]))
                indent_len++;

            /* Collect every word (a maximal run of non-whitespace bytes)
             * across all of this paragraph's lines - joining them is what
             * lets short original lines merge back together. */
            typedef struct { int start, len; } word_span;
            int word_cap = 16, word_count = 0;
            word_span* words = malloc(sizeof(word_span) * (size_t)word_cap);

            for (;;)
            {
                int wi = 0;
                while (wi < line_len)
                {
                    while (wi < line_len && wordwrap_is_space(line[wi]))
                        wi++;
                    int wstart = wi;
                    while (wi < line_len && !wordwrap_is_space(line[wi]))
                        wi++;
                    if (wi > wstart && words)
                    {
                        if (word_count >= word_cap)
                        {
                            word_cap *= 2;
                            word_span* nw = realloc(words, sizeof(word_span) * (size_t)word_cap);
                            if (!nw) { free(words); words = NULL; }
                            else words = nw;
                        }
                        if (words)
                        {
                            words[word_count].start = (int)(line + wstart - sel);
                            words[word_count].len = wi - wstart;
                            word_count++;
                        }
                    }
                }

                if (at_end)
                    break;
                /* Peek at the next line: still part of this paragraph
                 * (non-blank)? If so, fold it in and keep collecting words;
                 * otherwise leave `pos`/`line`/`at_end` alone so the outer
                 * loop reprocesses it (as a blank line or a new paragraph). */
                int next_start = pos + 1;
                int p2 = next_start;
                while (p2 < sel_len && sel[p2] != '\n')
                    p2++;
                if (next_start <= sel_len && !wordwrap_is_blank_line(sel + next_start, p2 - next_start))
                {
                    pos = p2;
                    line = sel + next_start;
                    line_len = p2 - next_start;
                    at_end = (pos >= sel_len);
                }
                else
                {
                    break;
                }
            }

            if (have_output)
                wwbuf_append(&out, "\n", 1);
            have_output = 1;

            /* First pass: same greedy packing as before, but recorded as
             * word-index spans rather than emitted right away - justify
             * (below) needs to know a line's word count and whether it's
             * the paragraph's last line before writing any of its words. */
            typedef struct { int start, count; } line_span;
            int line_cap = 8, line_count = 0;
            line_span* lines = malloc(sizeof(line_span) * (size_t)line_cap);
            int w = 0;
            while (lines && w < word_count)
            {
                int span_start = w;
                int cur_len = indent_len + words[w].len;
                w++;
                while (w < word_count && cur_len + 1 + words[w].len <= columns)
                {
                    cur_len += 1 + words[w].len;
                    w++;
                }
                if (line_count >= line_cap)
                {
                    line_cap *= 2;
                    line_span* nl = realloc(lines, sizeof(line_span) * (size_t)line_cap);
                    if (!nl) { free(lines); lines = NULL; break; }
                    lines = nl;
                }
                if (lines)
                {
                    lines[line_count].start = span_start;
                    lines[line_count].count = w - span_start;
                    line_count++;
                }
            }

            for (int li = 0; lines && li < line_count; li++)
            {
                if (li > 0)
                    wwbuf_append(&out, "\n", 1);
                int wstart = lines[li].start;
                int wcount = lines[li].count;
                int is_last_line = (li == line_count - 1);

                for (int k = 0; k < indent_len; k++)
                    wwbuf_append(&out, " ", 1);

                int word_chars = 0;
                for (int k = 0; k < wcount; k++)
                    word_chars += words[wstart + k].len;
                int gaps = wcount - 1;
                int space_budget = columns - indent_len - word_chars;
                int use_justify = justify && !is_last_line && gaps > 0 && space_budget >= gaps;
                int base = use_justify ? space_budget / gaps : 1;
                int extra = use_justify ? space_budget % gaps : 0;

                for (int k = 0; k < wcount; k++)
                {
                    wwbuf_append(&out, sel + words[wstart + k].start, (size_t)words[wstart + k].len);
                    if (k < wcount - 1)
                    {
                        int spaces = base + (k < extra ? 1 : 0);
                        for (int s = 0; s < spaces; s++)
                            wwbuf_append(&out, " ", 1);
                    }
                }
            }

            free(lines);
            free(words);
        }

        if (at_end)
            break;
        pos++;  /* skip the '\n' just scanned past */
    }

    if (!out.buf)
        return NULL;
    *out_len = (int)out.len;
    return out.buf;
}

static void do_edit_wordwrap(int columns, int justify)
{
    int lo, hi;
    ui_node* ed = edit_selection_target(&lo, &hi);
    if (!ed)
        return;
    const char* text = ui_get_value(ed);
    int out_len;
    char* out = wordwrap_text(text + lo, hi - lo, columns, justify, &out_len);
    if (out)
        edit_replace_selection(ed, lo, hi, out, out_len);
}

/* Edit > Format C Source: reformats the *entire* active document via
 * lsp_text_document_formatting()/lsp_text_document_range_formatting()
 * (ide_lsp.c) - textDocument/formatting or textDocument/rangeFormatting,
 * depending on whether there's a selection, the same way a real LSP client
 * would pick between the two methods. Purely in memory, no temp files.
 *
 * Unlike Stringify/To Upper/Word Wrap, a selection restricts Format to just
 * those *lines* rather than reformatting only the selected text in
 * isolation (indentation depth depends on brace nesting from the start of
 * the file, so that wouldn't make sense) - so this goes through
 * editor_in_window() + ui_set_value() (the same whole-buffer-rebuild path
 * do_replace_all() uses) instead of edit_selection_target()/
 * edit_replace_selection(). */
static void do_edit_format(void)
{
    ui_node* ed = editor_in_window(g_active_editor_window);
    if (!ed)
        return;

    const char* text = ui_get_value(ed);

    /* Needed so #include "quoted.h" siblings of this file resolve. path/
     * untitled live on the WINDOW node (see ui_set_path/ui_set_untitled call
     * sites), not on the editor child `ed` returned by editor_in_window() -
     * same node do_compile() reads (g_active_editor_window), not `ed`. */
    const char* path = ui_get_untitled(g_active_editor_window) ? NULL : ui_get_path(g_active_editor_window);
    char* uri = lsp_path_to_uri(path);

    /* Same -style=<name> as Compile > Options... (g_compile.style), not a
     * hardcoded style - so Format matches whatever the user picked there.
     * cake_format() itself defaults to -style=cake when none is set. */
    const char* style = g_compile.style[0] ? g_compile.style : NULL;

    struct lsp_text_edit_list edits = { 0 };
    bool ok;

    /* A non-empty selection restricts Format to just those lines, fed from
     * the editor's selection instead of a command-line argument. No
     * selection (or just a caret) formats the whole file, same as before -
     * just via the two different LSP methods now instead of one function
     * with an optional flag. */
    int sel_lo = 0, sel_hi = 0;
    if (ui_editor_get_selection(ed, &sel_lo, &sel_hi) && sel_lo != sel_hi)
    {
        if (sel_lo > sel_hi)
        {
            int tmp = sel_lo;
            sel_lo = sel_hi;
            sel_hi = tmp;
        }

        struct lsp_document_range_formatting_params params = { 0 };
        params.text_document.uri = uri;
        params.options.style = style;

        /* 0-based, per lsp_position's own convention (see lsp_types.h). */
        int line_lo = 0;
        for (const char* p = text; p < text + sel_lo && *p; p++)
        {
            if (*p == '\n')
                line_lo++;
        }
        int line_hi = line_lo;
        for (const char* p = text + sel_lo; p < text + sel_hi && *p; p++)
        {
            if (*p == '\n')
                line_hi++;
        }
        params.range.start.line = line_lo;
        params.range.end.line = line_hi;

        ok = lsp_text_document_range_formatting(text, &params, &edits);
    }
    else
    {
        struct lsp_document_formatting_params params = { 0 };
        params.text_document.uri = uri;
        params.options.style = style;

        ok = lsp_text_document_formatting(text, &params, &edits);
    }

    free(uri);

    if (ok && edits.count > 0)
    {
        int cur = ui_editor_get_cursor(ed);      /* keep the caret put */
        int scroll = ui_editor_get_scroll(ed);   /* and the scroll position */
        ui_set_value(ed, edits.items[0].new_text);  /* (ui_set_value moves the
                                        * caret to the end and resets scroll
                                        * to the top) */
        ui_editor_set_selection(ed, cur, cur);
        ui_editor_set_scroll(ed, scroll);
        ui_set_dirty(ed, 1);
    }
    else
    {
        compile_status_set("Format failed");
    }

    lsp_text_edit_list_destroy(&edits);
}

static const char* skip_spaces(const char* p)
{
    while (*p == ' ' || *p == '\t')
        p++;
    return p;
}

/* Every reserved word worth completing this way: the standard C keywords,
 * plus cake's own ownership qualifiers (_Owner, _Opt, _Out, _View, _Clear,
 * _Assert - see is_c_keyword2 in ide_ui.c). */
static const char* const g_keyword_list[] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "inline", "int", "long", "register", "restrict", "return", "short",
    "signed", "sizeof", "static", "struct", "switch", "typedef", "union",
    "unsigned", "void", "volatile", "while", "static_assert",
    "try", "catch", "throw",
    "_Bool", "_Complex", "_Imaginary", "_Alignas", "_Alignof", "_Atomic",
    "_Generic", "_Noreturn", "_Static_assert", "_Thread_local",
    "_Owner", "_Opt", "_Out", "_View", "_Clear", "_Assert",
};

/* Plain prefix completion against g_keyword_list: `word` (wlen bytes, no
 * NUL) is a strict prefix of exactly one keyword -> *out_text gets that
 * keyword's remaining suffix (malloc'd) and we return 1. Zero matches, or
 * more than one keyword sharing the prefix (ambiguous - e.g. "s" fits
 * short/signed/sizeof/static/struct/switch), does nothing and returns 0. */
static int keyword_prefix_complete(const char* word, int wlen, char** out_text)
{
    const char* match = NULL;

    for (size_t i = 0; i < sizeof g_keyword_list / sizeof g_keyword_list[0]; i++)
    {
        const char* kw = g_keyword_list[i];
        size_t klen = strlen(kw);
        if (klen > (size_t)wlen && memcmp(kw, word, (size_t)wlen) == 0)
        {
            if (match)
                return 0;  /* ambiguous */
            match = kw;
        }
    }
    if (!match)
        return 0;

    const char* suffix = match + wlen;
    char* buf = malloc(strlen(suffix) + 1);
    if (!buf)
        return 0;
    strcpy(buf, suffix);
    *out_text = buf;
    return 1;
}

/* Keywords that are already a complete statement/label on their own -
 * word-for-word (not a prefix, see keyword_prefix_complete above) - just
 * need their closing punctuation appended. */
static int keyword_punct_complete(const char* word, int wlen, char** out_text)
{
    static const struct { const char* kw; char punct; } table[] = {
        { "break", ';' }, { "continue", ';' }, { "throw", ';' }, { "default", ':' },
    };
    for (size_t i = 0; i < sizeof table / sizeof table[0]; i++)
    {
        size_t klen = strlen(table[i].kw);
        if (klen == (size_t)wlen && memcmp(table[i].kw, word, klen) == 0)
        {
            char* buf = malloc(2);
            if (!buf)
                return 0;
            buf[0] = table[i].punct;
            buf[1] = 0;
            *out_text = buf;
            return 1;
        }
    }
    return 0;
}

/* Fired synchronously by ui_screen_update() (see ide_ui.c's Tab handling)
 * when Tab is pressed in an <editor> with the caret at the end of its line
 * and nothing selected. `line` is that line's text up to the caret; on a
 * match, *out_text gets a malloc'd string appended right after it (the
 * caller frees it), *out_cursor_offset is a byte offset into *out_text
 * saying where the caret should land afterward, and we return 1. Anything
 * unrecognized returns 0, which falls back to the normal soft-tab insert.
 *
 * Just a first pattern to prove the hook out: a bare "switch" (whatever its
 * own indentation) completes to a template block, reusing that same
 * indentation for the brace/case lines so it reads as part of the
 * surrounding code rather than a fixed-column snippet - and leaves the caret
 * right at "var" instead of past the whole block, since that placeholder is
 * what the user actually needs to edit next. */
static int on_line_complete(void* ctx, const char* line, char** out_text, int* out_cursor_offset)
{
    (void)ctx;

    const char* p = skip_spaces(line);
    int indent_len = (int)(p - line);
    int w = ui_indent_width();

    if (strncmp(p, "switch", sizeof "switch" - 1) == 0 && p[sizeof "switch" - 1] == 0)
    {
        char* buf = malloc((size_t)indent_len * 2 + w + 64);
        if (!buf)
            return 0;
        sprintf(buf, " (var)\n%.*s{\n%.*s%*scase 0: break;\n%.*s}",
                indent_len, line, indent_len, line, w, "", indent_len, line);
        *out_text = buf;
        *out_cursor_offset = 2;  /* right after " (", at the start of "var" */
        return 1;
    }

    if (strncmp(p, "struct", sizeof "struct" - 1) == 0 &&
        (p[sizeof "struct" - 1] == ' ' || p[sizeof "struct" - 1] == '\t'))
    {
        /* "struct name" (a tag, nothing else on the line) - the name is
         * whatever's left after skipping the space(s) that follow "struct",
         * and must run all the way to the end of the line to count (so
         * "struct foo bar" or a half-typed "struct foo (" don't match). */
        const char* name = skip_spaces(p + sizeof "struct" - 1);
        const char* q = name;
        while (is_word_char((unsigned char)*q))
            q++;
        if (q != name && *q == 0)
        {
            char* buf = malloc((size_t)indent_len * 4 + w + 64);
            if (!buf)
                return 0;
            int off = sprintf(buf, "\n%.*s{\n%.*s%*s",
                               indent_len, line, indent_len, line, w, "");
            int cursor_off = off;
            sprintf(buf + off, "\n%.*s};", indent_len, line);
            *out_text = buf;
            *out_cursor_offset = cursor_off;
            return 1;
        }
    }

    if (strncmp(p, "int", sizeof "int" - 1) == 0 &&
        (p[sizeof "int" - 1] == ' ' || p[sizeof "int" - 1] == '\t'))
    {
        /* "int main", "int main(", or "int main()" - however far the user
         * got typing the signature by hand, each completes to the same
         * "int main(void) { ... }" body, filling in only whatever's still
         * missing rather than duplicating what's already on the line. */
        const char* q = skip_spaces(p + sizeof "int" - 1);
        if (strncmp(q, "main", sizeof "main" - 1) == 0)
        {
            const char* r = skip_spaces(q + sizeof "main" - 1);
            int variant = -1;  /* 0: "int main"  1: "int main("  2: "int main()" */
            if (*r == 0)
                variant = 0;
            else if (*r == '(')
            {
                r = skip_spaces(r + 1);
                if (*r == 0)
                    variant = 1;
                else if (*r == ')' && *skip_spaces(r + 1) == 0)
                    variant = 2;
            }

            if (variant >= 0)
            {
                char* buf = malloc((size_t)indent_len * 4 + w + 64);
                if (!buf)
                    return 0;
                int off;
                if (variant == 0)
                    off = sprintf(buf, "(void)\n%.*s{\n%.*s%*s",
                                  indent_len, line, indent_len, line, w, "");
                else if (variant == 1)
                    off = sprintf(buf, "void)\n%.*s{\n%.*s%*s",
                                  indent_len, line, indent_len, line, w, "");
                else
                    off = sprintf(buf, "\n%.*s{\n%.*s%*s",
                                  indent_len, line, indent_len, line, w, "");
                int cursor_off = off;
                sprintf(buf + off, "\n%.*s}", indent_len, line);
                *out_text = buf;
                *out_cursor_offset = cursor_off;
                return 1;
            }
        }
    }

    if (strncmp(p, "static_assert", sizeof "static_assert" - 1) == 0 &&
        p[sizeof "static_assert" - 1] == 0)
    {
        /* Single-line, no brace block to reindent - just the call's own
         * text, caret dropped at "condition" same as switch's "var". */
        char* buf = malloc(64);
        if (!buf)
            return 0;
        sprintf(buf, "(condition, \"message\");");
        *out_text = buf;
        *out_cursor_offset = 1;  /* right after "(", at the start of "condition" */
        return 1;
    }

    if (strncmp(p, "if", sizeof "if" - 1) == 0 && p[sizeof "if" - 1] == 0)
    {
        char* buf = malloc((size_t)indent_len * 2 + w + 64);
        if (!buf)
            return 0;
        sprintf(buf, " (condition)\n%.*s{\n%.*s%*s\n%.*s}",
                indent_len, line, indent_len, line, w, "", indent_len, line);
        *out_text = buf;
        *out_cursor_offset = 2;  /* right after " (", at the start of "condition" */
        return 1;
    }

    if (strncmp(p, "for", sizeof "for" - 1) == 0 && p[sizeof "for" - 1] == 0)
    {
        /* The common counting-loop shape, pre-filled rather than left as
         * bare placeholders - only the bound is actually missing, so that's
         * where the caret goes ("i < |; i++)"). */
        char* buf = malloc((size_t)indent_len * 2 + w + 96);
        if (!buf)
            return 0;
        int off = sprintf(buf, " (int i = 0; i < ");
        int cursor_off = off;
        sprintf(buf + off, "; i++)\n%.*s{\n%.*s%*s\n%.*s}",
                indent_len, line, indent_len, line, w, "", indent_len, line);
        *out_text = buf;
        *out_cursor_offset = cursor_off;
        return 1;
    }

    if (strncmp(p, "while", sizeof "while" - 1) == 0 && p[sizeof "while" - 1] == 0)
    {
        char* buf = malloc((size_t)indent_len * 2 + w + 64);
        if (!buf)
            return 0;
        sprintf(buf, " (condition)\n%.*s{\n%.*s%*s\n%.*s}",
                indent_len, line, indent_len, line, w, "", indent_len, line);
        *out_text = buf;
        *out_cursor_offset = 2;  /* right after " (", at the start of "condition" */
        return 1;
    }

    if (strncmp(p, "do", sizeof "do" - 1) == 0 && p[sizeof "do" - 1] == 0)
    {
        char* buf = malloc((size_t)indent_len * 4 + w + 64);
        if (!buf)
            return 0;
        /* Body left blank, caret parked on its own indented line - same as
         * try's body above. */
        int off = sprintf(buf, "\n%.*s{\n%.*s%*s",
                           indent_len, line, indent_len, line, w, "");
        int cursor_off = off;
        sprintf(buf + off, "\n%.*s} while (condition);", indent_len, line);
        *out_text = buf;
        *out_cursor_offset = cursor_off;
        return 1;
    }

    if (strncmp(p, "else", sizeof "else" - 1) == 0 && p[sizeof "else" - 1] == 0)
    {
        char* buf = malloc((size_t)indent_len * 4 + w + 64);
        if (!buf)
            return 0;
        int off = sprintf(buf, "\n%.*s{\n%.*s%*s",
                           indent_len, line, indent_len, line, w, "");
        int cursor_off = off;
        sprintf(buf + off, "\n%.*s}", indent_len, line);
        *out_text = buf;
        *out_cursor_offset = cursor_off;
        return 1;
    }

    if (strncmp(p, "try", sizeof "try" - 1) == 0 && p[sizeof "try" - 1] == 0)
    {
        char* buf = malloc((size_t)indent_len * 6 + w * 2 + 64);
        if (!buf)
            return 0;
        /* Body left blank, caret parked on its own indented line - the same
         * "put the caret where the user needs to type next" idea as
         * switch's "var" above, just with nothing to overwrite here. */
        int off = sprintf(buf, " \n%.*s{\n%.*s%*s",
                           indent_len, line, indent_len, line, w, "");
        int cursor_off = off;
        sprintf(buf + off, "\n%.*s}\n%.*scatch\n%.*s{\n%.*s%*s\n%.*s}",
                indent_len, line, indent_len, line, indent_len, line,
                indent_len, line, w, "", indent_len, line);
        *out_text = buf;
        *out_cursor_offset = cursor_off;
        return 1;
    }

    /* No full-line snippet matched - fall back to plain prefix completion
     * on the word right before the caret (e.g. "bre" -> "break"), wherever
     * it sits in the line (unlike the snippets above, which only fire on a
     * bare reserved word filling the whole line). */
    {
        const char* wend = line + strlen(line);
        const char* wstart = wend;
        while (wstart > line && is_word_char((unsigned char)wstart[-1]))
            wstart--;
        int wlen = (int)(wend - wstart);
        if (wlen > 0 && (keyword_prefix_complete(wstart, wlen, out_text) ||
                          keyword_punct_complete(wstart, wlen, out_text)))
        {
            *out_cursor_offset = (int)strlen(*out_text);
            return 1;
        }
    }

    return 0;
}

/* Shared by EVT_COMPILE_OPTIONS and EVT_PROJECT_OPTIONS: populates every
 * g_copts widget from `cs` and shows the modal. `cs` is remembered in
 * g_copts.editing so EVT_COPTS_OK writes back to the very same struct this
 * open chose, regardless of what project_is_open()/the active document do
 * while the dialog is up. Retitled so it's obvious which one is being
 * edited. */
/* Checks the Options field with the compiler's own parser (fill_options), split the way
 * build_compile_argv() splits it. fill_options only says pass/fail, so it runs on longer and
 * longer prefixes: the first prefix that fails ends with the bad option (an option that takes a
 * value, like "-o file", is still whole). Returns 1 and copies that option into `bad` when one is rejected. */
static int copts_find_invalid(const char* text, char* bad, size_t cap)
{
    char buf[sizeof((compile_settings*)0)->options];
    snprintf(buf, sizeof buf, "%s", text);

    const char* argv[64];
    int argc = 0;
    argv[argc++] = "cake";
    for (char* tok = strtok(buf, " \t"); tok && argc < (int)_Countof(argv); tok = strtok(NULL, " \t"))
    {
        argv[argc++] = tok;
        struct options options = { 0 };
        if (fill_options(&options, argc, argv) != 0)
        {
            snprintf(bad, cap, "%s", tok);
            return 1;
        }
    }
    return 0;
}

static void open_compiler_options_dialog(compile_settings* cs, int is_project)
{
    g_copts.editing = cs;
    ui_set_label(g_copts.window, is_project ? " Compiler Options (Project) " : " Compiler Options ");
    ui_set_value(g_copts.input, cs->options);
    ui_set_value(g_copts.output, cs->output);
    ui_select_set_selected(g_copts.target, target_slug_to_index(cs->target));
    ui_select_set_selected(g_copts.style, style_slug_to_index(cs->style));
    ui_select_set_selected(g_copts.diagformat, diagformat_slug_to_index(cs->diagnostic_format));
    /* Re-sync the check-box group from the committed state every time the
     * dialog opens, same reasoning as the Target/Style <select>s above - so
     * a Cancel below discards whatever gets clicked this time (same pattern
     * as Find's g_find.opts). */
    ui_group_set_checked(g_copts.flags, 0, cs->no_output);
    ui_group_set_checked(g_copts.flags, 1, cs->line_directives);
    ui_group_set_checked(g_copts.flags, 2, cs->fanalyzer);
    ui_group_set_checked(g_copts.flags, 3, cs->const_literal);
    ui_group_set_checked(g_copts.flags, 4, cs->wall);
    ui_group_set_checked(g_copts.flags, 5, cs->unused_extern_report);
    ui_group_set_checked(g_copts.flags, 6, cs->use_cake_headers);
    ui_screen_show_modal(g_screen, g_copts.modal);
}

/* Fired synchronously by ui_screen_update() for whatever widget was
 * activated this frame - pushed straight to us, not pulled via polling.
 * `param` is NULL for most ids (see ui_fire_event's call sites in ui.c) but
 * carries the target window for the editor popup's own items (see
 * ui_screen_open_popup in app_frame()), so those don't need a global to
 * remember what the popup was opened for. */
static void on_ui_event(void* ctx, int id, void* param)
{
    (void)ctx;
    if (id == EVT_FILE_EXIT)
    {
        g_quit = 1;
    }
    else if (id == EVT_COMPILE)
    {
        do_build();
    }
    else if (id == EVT_COMPILE_FILE)
    {
        do_compile();
    }
    else if (id == EVT_DEBUG_START)
    {
        do_debug_start();
    }
    else if (id == EVT_DEBUG_STOP)
    {
        do_debug_stop();
    }
    else if (id == EVT_DEBUG_CONTINUE)
    {
        do_debug_continue();
    }
    else if (id == EVT_DEBUG_STEP_OVER)
    {
        do_debug_step_over();
    }
    else if (id == EVT_DEBUG_STEP_INTO)
    {
        do_debug_step_into();
    }
    else if (id == EVT_DEBUG_TOGGLE_BREAKPOINT)
    {
        do_debug_toggle_breakpoint();
    }
    else if (id == EVT_COMPILE_OPTIONS)
    {
        /* File > "Options..." always means the global settings (cake.json,
         * beside the executable) - the ones used for Playground and for any
         * file that isn't part of the open project (see
         * active_compile_settings()). A project's own settings are edited
         * from Project > "Options..." instead, so which file a given dialog
         * writes never depends on what happens to be the active document. */
        open_compiler_options_dialog(&g_compile, 0);
    }
    else if (id == EVT_PROJECT_OPTIONS)
    {
        /* Project > "Options..." always means the project's own settings,
         * unconditionally - unlike Compile > "Options..." above, it isn't
         * about whatever's active right now. Menu item is disabled without
         * an open project (see app_frame()'s menu_items_requiring_project
         * loop), so this is only ever reachable with one. */
        open_compiler_options_dialog(&g_project.compile, 1);
    }
    else if (id == EVT_COPTS_OK &&
             copts_find_invalid(ui_get_value(g_copts.input), g_copts.bad_option, sizeof g_copts.bad_option))
    {
        char msg[400];
        snprintf(msg, sizeof msg, "The compiler does not accept this option:\n\n  %s\n\nKeep the options anyway?", g_copts.bad_option);
        ui_msgbox_button buttons[] = { { "  Keep  ", EVT_COPTS_KEEP_INVALID }, { "  Fix  ", 0 } };
        ui_message_box(g_screen, "Compiler Options", msg, buttons, 2);
    }
    else if (id == EVT_COPTS_OK || id == EVT_COPTS_KEEP_INVALID)
    {
        /* Persist what was typed/picked so the next compile uses it, into
         * whichever settings this dialog was opened against (g_copts.editing,
         * set by open_compiler_options_dialog() - so this always matches even
         * if project_is_open()/the active document changed while the modal
         * was up). Target/Style indices map to the compiler's slugs; the
         * last option ("") -> "". */
        compile_settings* cs = g_copts.editing;
        snprintf(cs->options, sizeof cs->options, "%s",
                 ui_get_value(g_copts.input));
        snprintf(cs->output, sizeof cs->output, "%s",
                 ui_get_value(g_copts.output));
        int sel = ui_select_get_selected(g_copts.target);
        int slug_count = (int)(sizeof g_target_slugs / sizeof g_target_slugs[0]);
        cs->target = (sel >= 0 && sel < slug_count) ? g_target_slugs[sel] : "";
        int style_sel = ui_select_get_selected(g_copts.style);
        int style_count = (int)(sizeof g_style_slugs / sizeof g_style_slugs[0]);
        cs->style = (style_sel >= 0 && style_sel < style_count) ? g_style_slugs[style_sel] : "";
        int diag_sel = ui_select_get_selected(g_copts.diagformat);
        int diag_count = (int)(sizeof g_diagformat_slugs / sizeof g_diagformat_slugs[0]);
        cs->diagnostic_format = (diag_sel >= 0 && diag_sel < diag_count) ? g_diagformat_slugs[diag_sel] : "";
        cs->no_output = ui_group_get_checked(g_copts.flags, 0);
        cs->line_directives = ui_group_get_checked(g_copts.flags, 1);
        cs->fanalyzer = ui_group_get_checked(g_copts.flags, 2);
        cs->const_literal = ui_group_get_checked(g_copts.flags, 3);
        cs->wall = ui_group_get_checked(g_copts.flags, 4);
        cs->unused_extern_report = ui_group_get_checked(g_copts.flags, 5);
        cs->use_cake_headers = ui_group_get_checked(g_copts.flags, 6);
        /* Persist immediately, to whichever file owns these settings: a
         * project's own ".cakeproj", or - for Playground and any file that
         * isn't part of the project (see active_compile_settings()) - the
         * session's own "compile" node. Without the second case the IDE-wide
         * options only reached disk at shutdown, so a change made here was
         * invisible in session.json until the IDE was closed, and lost
         * outright if it never closed cleanly. */
        if (cs == &g_project.compile)
            project_save();
        else
            global_settings_save();
        ui_screen_close_modal(g_screen, g_copts.modal);
    }
    else if (id == EVT_DOCK_LEFT || id == EVT_DOCK_RIGHT || id == EVT_DOCK_BOTTOM)
    {
        /* The three ids are contiguous and in ui_dock_side order, so the
         * offset from EVT_DOCK_LEFT is the side (UI_DOCK_LEFT is 1). */
        dock_panel_to(g_dockmenu.target, (ui_dock_side)(UI_DOCK_LEFT + (id - EVT_DOCK_LEFT)));
        g_dockmenu.target = NULL;
    }
    else if (id == EVT_COPTS_CANCEL)
    {
        ui_screen_close_modal(g_screen, g_copts.modal);
    }
    else if (id == EVT_COPTS_HELP)
    {
        show_help_text(ui_get_help(g_copts.window));
    }
    else if (id == EVT_HELP_MANUAL)
    {
        do_help_manual();
    }
    else if (id == EVT_HELP_WEBSITE)
    {
        do_help_open_link("https://cakecc.org/");
    }
    else if (id == EVT_HELP_CONTEXTUAL)
    {
        /* F1 explains what the status bar shows (see ui_set_help) - with
         * no hint there, it does nothing. */
        show_hint_window();
    }
    else if (id == UI_HINT_DETAILS_ID)
    {
        show_hint_window();
    }
    else if (id == EVT_HINTWIN_CLOSE)
    {
        ui_screen_close_modal(g_screen, g_hintwin.modal);
    }
    else if (id == EVT_HELP_ABOUT)
    {
        ui_screen_show_modal(g_screen, g_about_modal);
    }
    else if (id == EVT_ABOUT_CLOSE)
    {
        ui_screen_close_modal(g_screen, g_about_modal);
    }
    else if (id == EVT_OPTIONS_DIRS)
    {
        ui_screen_show_modal(g_screen, g_dirs_modal);
    }
    else if (id == EVT_DIRS_OK || id == EVT_DIRS_CANCEL)
    {
        ui_screen_close_modal(g_screen, g_dirs_modal);
    }
    else if (id == EVT_SEARCH_GOTO)
    {
        ui_set_value(g_goto_input, "");
        ui_screen_show_modal(g_screen, g_goto_modal);
        ui_screen_focus(g_screen, g_goto_input);
    }
    else if (id == EVT_SEARCH_GOTO_DEFINITION)
    {
        do_goto_definition();
    }
    else if (id == EVT_GOTO_OK || id == EVT_GOTO_INPUT)
    {
        int line = atoi(ui_get_value(g_goto_input));
        ui_node* editor = line > 0 ? editor_in_window(ui_screen_top_window(g_screen)) : NULL;
        if (editor)
        {
            nav_record_jump();
            ui_editor_goto_line(editor, line);
        }
        ui_screen_close_modal(g_screen, g_goto_modal);
        g_goto_pending_focus = editor;  /* focus after this update finishes */
    }
    else if (id == EVT_GOTO_CANCEL)
    {
        ui_screen_close_modal(g_screen, g_goto_modal);
    }
    else if (id == EVT_EDIT_WORDWRAP)
    {
        char buf[16];
        snprintf(buf, sizeof buf, "%d", g_wordwrap_columns);
        ui_set_value(g_wordwrap_input, buf);
        ui_group_set_checked(g_wordwrap_justify, 0, g_wordwrap_justify_on);
        ui_screen_show_modal(g_screen, g_wordwrap_modal);
        ui_screen_focus(g_screen, g_wordwrap_input);
    }
    else if (id == EVT_WORDWRAP_OK || id == EVT_WORDWRAP_INPUT)
    {
        int columns = atoi(ui_get_value(g_wordwrap_input));
        ui_node* editor = NULL;
        if (columns > 0)
        {
            g_wordwrap_columns = columns;
            g_wordwrap_justify_on = ui_group_get_checked(g_wordwrap_justify, 0);
            editor = editor_in_window(g_active_editor_window);
            do_edit_wordwrap(columns, g_wordwrap_justify_on);
        }
        ui_screen_close_modal(g_screen, g_wordwrap_modal);
        g_goto_pending_focus = editor;  /* focus after this update finishes -
                                         * see app_frame - otherwise the
                                         * input's fire-then-blur leaves the
                                         * editor unfocused and the cursor
                                         * scrolled into view but not shown */
    }
    else if (id == EVT_WORDWRAP_CANCEL)
    {
        ui_screen_close_modal(g_screen, g_wordwrap_modal);
    }
    else if (id == EVT_OUTPUT_CMDLINE)
    {
        cmdline_execute();
    }
    else if (id == EVT_OUTPUT_DBLCLICK)
    {
        output_goto_source();
    }
    else if (id == EVT_EDITOR_CTRLCLICK)
    {
        do_editor_ctrlclick();
    }
    else if (id == EVT_NAV_BACK)
    {
        nav_back();
    }
    else if (id == EVT_NAV_FORWARD)
    {
        nav_forward();
    }
    else if (id == EVT_EDITOR_TOGGLE_READONLY)
    {
        ui_node* win = param ? (ui_node*)param : g_active_editor_window;
        ui_node* ed = editor_in_window(win);
        if (ed)
            ui_set_read_only(ed, !ui_get_read_only(ed));
    }
    else if (id == EVT_FOLDER_COPY_PATH)
    {
        /* Copies g_folder.dir itself - see this id's own doc comment for why
         * that's what "Copy Full Path" means here, unlike the editor popup's
         * copy of the same label just below. */
        if (g_folder.dir[0])
            ui_clipboard_set_text(g_folder.dir);
    }
    else if (id == EVT_PROJECT_POPUP_NEWFILE)
    {
        /* Same dialog the Folder panel's "New File..." opens, just rooted at
         * the project directory - see g_foldernew.in_project. */
        if (project_is_open())
        {
            g_foldernew.is_folder = 0;
            g_foldernew.in_project = 1;
            ui_set_label(g_foldernew.window, " New File ");
            ui_set_value(g_foldernew.input, "");
            ui_screen_show_modal(g_screen, g_foldernew.modal);
            ui_screen_focus(g_screen, g_foldernew.input);
        }
    }
    else if (id == EVT_FOLDER_NEWFILE || id == EVT_FOLDER_NEWFOLDER)
    {
        if (g_folder.dir[0])
        {
            g_foldernew.is_folder = (id == EVT_FOLDER_NEWFOLDER);
            g_foldernew.in_project = 0;
            ui_set_label(g_foldernew.window, g_foldernew.is_folder ? " New Folder " : " New File ");
            ui_set_value(g_foldernew.input, "");
            ui_screen_show_modal(g_screen, g_foldernew.modal);
            ui_screen_focus(g_screen, g_foldernew.input);
        }
    }
    else if (id == EVT_FOLDERNEW_OK)
    {
        const char* name = ui_get_value(g_foldernew.input);
        const char* caption = g_foldernew.is_folder ? "New Folder" : "New File";

        /* A bare name, not a path - same restriction Save As implicitly has
         * (its own Name field never carries a separator either, since it's
         * always typed alongside a directory picker, never a full path).
         * Rejecting one here keeps the new file/folder inside g_folder.dir
         * instead of silently escaping it. */
        if (!name[0] || strchr(name, '/') || strchr(name, '\\'))
        {
            ui_msgbox_button ok = { "   OK   ", 0 };
            ui_message_box(g_screen, caption,
                            "Enter a name (no \\ or /).", &ok, 1);
        }
        else
        {
            char path[1024];
            path_join(path, sizeof path,
                     g_foldernew.in_project ? g_project.dir : g_folder.dir, name);
            int created = 0;

            if (g_foldernew.is_folder)
            {
                /* fs.h's own mkdir(path, mode) shim (#define mkdir(a, b)
                 * _mkdir(a) on Windows) already treats "fine if it already
                 * exists" as this file's own get_config_dir() does - here,
                 * unlike that fire-and-forget case, the user is watching, so
                 * a non-zero return (already exists, or just not creatable)
                 * gets reported instead of silently ignored. */
                created = mkdir(path, 0755) == 0;
                if (!created)
                {
                    ui_msgbox_button ok = { "   OK   ", 0 };
                    char message[1200];
                    snprintf(message, sizeof message,
                             "Could not create %s.\nIt may already exist.", name);
                    ui_message_box(g_screen, caption, message, &ok, 1);
                }
            }
            else
            {
                FILE* existing = fopen(path, "rb");
                if (existing)
                {
                    fclose(existing);
                    ui_msgbox_button ok = { "   OK   ", 0 };
                    char message[1200];
                    snprintf(message, sizeof message, "%s already exists.", name);
                    ui_message_box(g_screen, caption, message, &ok, 1);
                }
                else
                {
                    FILE* f = fopen(path, "wb");
                    if (f)
                        fclose(f);
                    created = 1;
                }
            }

            if (created)
            {
                ui_screen_close_modal(g_screen, g_foldernew.modal);

                /* Show it in the listing. A folder stops there; a file also opens
                 * into an editor window, same as clicking any other row -
                 * see folder_window_activate. */
                folder_window_refresh();
                if (!g_foldernew.is_folder)
                {
                    /* Opened from the Project panel: the point is that the
                     * new file becomes part of the project, not just of
                     * the directory (project_add_file saves and refreshes
                     * the Project listing). */
                    if (g_foldernew.in_project)
                    {
                        project_add_file(path);
                    }
                    nav_record_jump();
                    open_file_path_into_editor(path, name);
                }
            }
        }
    }
    else if (id == EVT_FOLDERNEW_CANCEL)
    {
        ui_screen_close_modal(g_screen, g_foldernew.modal);
    }
    else if (id == EVT_FOLDER_ADD_TO_PROJECT)
    {
        /* Same "acts on the listbox's currently selected row" caveat as
         * EVT_FOLDER_DELETE just below - the popup isn't opened per-row.
         * Silently does nothing for no selection, "..", or a subdirectory
         * (project_add_file()/g_project.files[] are for individual files,
         * same restriction Add Existing File... itself has via its own
         * "*.c;*.h" mask). Menu item is already disabled without an open
         * project (see the popup's own open-time refresh), so this is only
         * ever reachable with one. */
        int index = ui_select_get_selected(g_folder.listbox);
        if (index >= 0 && index < ui_child_count(g_folder.listbox))
        {
            const char* entry = ui_get_path(ui_child_at(g_folder.listbox, index));
            size_t elen = strlen(entry);
            int is_dir = elen > 0 && entry[elen - 1] == '\\';
            if (!is_dir && entry[0] && strcmp(entry, "..") != 0)
            {
                char path[1024];
                path_join(path, sizeof path, g_folder.dir, entry);
                project_add_file(path);
            }
        }
    }
    else if (id == EVT_FOLDER_DELETE)
    {
        /* Acts on the listbox's currently selected row - see this id's own
         * doc comment for why that isn't necessarily the row the popup was
         * opened over. Silently does nothing for no selection or ".." - a
         * subdirectory (populate_listbox_from_dir's own trailing "\" marker,
         * same one dir_row_navigate keys off of) is otherwise fair game now,
         * same as a plain file. */
        int index = ui_select_get_selected(g_folder.listbox);
        if (index >= 0 && index < ui_child_count(g_folder.listbox))
        {
            const char* entry = ui_get_path(ui_child_at(g_folder.listbox, index));
            size_t elen = strlen(entry);
            int is_dir = elen > 0 && entry[elen - 1] == '\\';

            char name[300];
            snprintf(name, sizeof name, "%s", entry);
            if (is_dir)
                name[strlen(name) - 1] = 0;  /* drop the trailing "\" marker -
                                              * same as dir_row_navigate */

            if (name[0] && strcmp(name, "..") != 0)
            {
                g_pending_delete_is_dir = is_dir;
                path_join(g_pending_delete_path, sizeof g_pending_delete_path, g_folder.dir, name);

                char message[1200];
                snprintf(message, sizeof message,
                         "Are you sure you want to delete this %s?\n%s",
                         is_dir ? "folder" : "file", name);
                ui_msgbox_button btns[] = {
                    { "   OK   ", EVT_FOLDER_DELETE_CONFIRM },
                    { " Cancel ", 0 },
                };
                ui_message_box(g_screen, is_dir ? "Delete Folder" : "Delete File",
                                message, btns, 2);
            }
        }
    }
    else if (id == EVT_FOLDER_DELETE_CONFIRM)
    {
        /* g_pending_delete_path/g_pending_delete_is_dir were set right
         * before the confirm box above - same "stash it, consume+clear it
         * here" shape as g_pending_close_window/EVT_CLOSE_DISCARD. rmdir()
         * (fs.h's own #define rmdir _rmdir shim on Windows) only for a
         * directory - unlike EVT_FOLDER_NEWFOLDER's mkdir(), a failure here
         * (almost always a non-empty directory) is worth telling the user
         * about rather than pretending it worked. */
        if (g_pending_delete_path[0])
        {
            int ok = g_pending_delete_is_dir
                ? rmdir(g_pending_delete_path) == 0
                : remove(g_pending_delete_path) == 0;

            if (!ok && g_pending_delete_is_dir)
            {
                ui_msgbox_button msgok = { "   OK   ", 0 };
                ui_message_box(g_screen, "Delete Folder",
                                "Could not delete the folder.\nIt may not be empty.",
                                &msgok, 1);
            }

            g_pending_delete_path[0] = 0;
            folder_window_refresh();
        }
    }
    else if (id == EVT_EDITOR_COPY_PATH)
    {
        ui_node* win = (ui_node*)param;
        if (!win)
            win = ui_screen_top_window(g_screen);

        if (win)
        {
            const char* path = ui_get_path(win);
            if (path && path[0])
                ui_clipboard_set_text(path);
        }
    }
    else if (id == EVT_GITDIFF_EDIT)
    {
        git_diff_edit();
    }
    else if (id == EVT_GITDIFF_COPY_PATH)
    {
        if (g_gitdiff_path[0])
            ui_clipboard_set_text(g_gitdiff_path);
    }
    else if (id == EVT_GITDIFF_SHOW_FOLDER)
    {
        if (g_gitdiff_path[0])
        {
            char dir[1400];
            snprintf(dir, sizeof dir, "%s", g_gitdiff_path);
            dirname(dir);
            folder_reveal_directory(dir);
        }
    }
    else if (id == EVT_EDITOR_SHOW_FOLDER)
    {
        /* Reveal this document's containing folder in the Folder panel
         * (folder_reveal_directory). A no-op for a window with no path (shouldn't
         * happen - see EVT_EDITOR_COPY_PATH just above). */
        ui_node* win = (ui_node*)param;
        if (!win)
            win = ui_screen_top_window(g_screen);

        if (win)
        {
            const char* path = ui_get_path(win);
            if (path && path[0])
            {
                char dir[1024];
                strncpy(dir, path, sizeof dir - 1);
                dir[sizeof dir - 1] = 0;
                dirname(dir);
                folder_reveal_directory(dir);
            }
        }
    }
    else if (id == EVT_EDITOR_CODEBLOCK_COPY)
    {
        /* g_md_codeblock_text was captured when this popup opened (see
         * refresh_codeblock_items()) - the item is disabled whenever it's
         * NULL, so this should always have real text by the time a click
         * gets here, but the check costs nothing. */
        if (g_md_codeblock_text)
            ui_clipboard_set_text(g_md_codeblock_text);
    }
    else if (id == EVT_EDITOR_CODEBLOCK_PLAYGROUND)
    {
        /* Overwrite playground.c with this code block, then show it in the
         * Playground window. open_playground()/open_file_path_into_editor()
         * only actually reads the file from disk when no window for that
         * path exists yet (see open_file_path_into_editor) - if Playground
         * is already open, reopening it would just refocus the *stale*
         * window instead of picking up what was just written, so that case
         * is handled directly here instead: swap the existing window's text
         * and reset its cursor/selection to the start, the same "freshly
         * opened this file" state a brand new window would start in. */
        if (g_md_codeblock_text)
        {
            char path[FS_MAX_PATH];
            if (get_playground_file_path(path, sizeof path))
            {
                FILE* f = fopen(path, "wb");
                if (f)
                {
                    fwrite_text(g_md_codeblock_text, strlen(g_md_codeblock_text), f, file_uses_crlf(path));
                    fclose(f);
                }

                ui_node* existing = find_open_window(path);
                ui_node* existing_ed = existing ? editor_in_window(existing) : NULL;
                if (existing_ed)
                {
                    ui_set_label(existing_ed, g_md_codeblock_text);
                    ui_editor_set_selection(existing_ed, 0, 0);
                    ui_screen_show_window(g_screen, existing);
                }
                else
                {
                    open_playground();
                }
            }
        }
    }
    else if (id == EVT_EDITOR_TOGGLE_HDRSRC)
    {
        /* Compute the counterpart from the popup target (preferred) or the
         * active window, then open it if present on disk. */
        ui_node* win = param ? (ui_node*)param : ui_screen_top_window(g_screen);
        if (win)
        {
            const char* path = ui_get_path(win);
            char cp[1024];
            if (path && path[0] && header_source_counterpart(path, cp, sizeof cp))
            {
                nav_record_jump();
                open_file_path_into_editor(cp, basename_of(cp));
            }
        }
    }
    else if (id == EVT_EDITOR_SHOW_OUTPUT)
    {
        /* The editor popup's "Show Generated Code" passes the right-clicked
         * window as param. The Compile menu's copy of the same command fires with
         * no param (a plain top-menu item, not context-sensitive) - fall
         * back to the active editor window so it acts on the current file,
         * same as every other Compile menu command. */
        ui_node* win = (ui_node*)param;
        if (!win)
            win = g_active_editor_window;

        ui_node* ed = editor_in_window(win);
        if (win && ed)
        {
            const char* path = ui_get_path(win);
            const char* base = basename_of(path);

            // Extract directory part (strip the filename)
            char dir[1024];
            strncpy(dir, path, sizeof(dir) - 1);
            dir[sizeof(dir) - 1] = '\0';
            char* last_sep = NULL;
            for (char* p = dir; *p; p++)
                if (*p == '/' || *p == '\\')
                    last_sep = p;
            if (last_sep)
                *last_sep = '\0';          // truncate to directory
            else
                dir[0] = '\0';             // shouldn't happen for a valid path

            // Build new path: dir/target/basename (or original if target empty)
            char new_path[1024];
            snprintf(new_path, sizeof(new_path), "%s/%s/%s", dir, active_platform_name(), base);

            // Load the file, or report the error if it's not found
            char* content = read_file_to_string(new_path);
            if (!content)
            {
                ui_msgbox_button ok = { "   OK   ", 0 };
                char message[1536];
                snprintf(message, sizeof message, "File not found:\n%s", new_path);
                ui_message_box(g_screen, "Error", message, &ok, 1);
                return;
            }

            // Title with target info
            char title[300];
            snprintf(title, sizeof(title), " %s [%s] ", base, active_platform_name());

            ui_node* new_wrapper = make_editor_window(g_root, g_new_count++, title, content, new_path);
            ui_node* new_editor = editor_in_window(new_wrapper);
            if (new_editor)
                ui_set_read_only(new_editor, 1);

            /* make_editor_window() always opens the new artifact window
             * maximized (see its own doc comment), same as any new document
             * window - which, if the source window it was compiled from is
             * ALSO currently maximized (every editor window starts this way,
             * Playground included), would otherwise land right on top of it,
             * completely hiding the source. Tile the two side by side
             * instead in that case (see tile_side_by_side()); a source
             * window the user has resized/moved off maximized is left
             * exactly where it is, same as before this existed. */
            ui_node* src_real = win ? ui_child_at(win, 0) : NULL;
            ui_node* new_real = ui_child_at(new_wrapper, 0);
            if (src_real && new_real && ui_get_maximized(src_real))
                tile_side_by_side(src_real, new_real);

            ui_screen_show_window(g_screen, new_wrapper);
            free(content);
        }
    }
    else if (id == EVT_SEARCH_FIND)
    {
        g_find.target = editor_in_window(ui_screen_top_window(g_screen));
        fill_from_selection(g_find.target, g_find.input);
        ui_screen_show_modal(g_screen, g_find.modal);
    }
    else if (id == EVT_FIND_OK)
    {
        do_find();
    }
    else if (id == EVT_SEARCH_NEXT)
    {
        do_find_next();
    }
    else if (id == EVT_SAVEAS_OVERWRITE)
    {
        save_as_commit();  /* user confirmed overwrite; the prompt auto-closed */
    }
    else if (id == EVT_PROJECT_NEW_OVERWRITE)
    {
        project_new_create(g_project_new_path);  /* user confirmed overwrite */
    }
    else if (id == EVT_FILE_RELOAD)
    {
        file_watch_reload_file();
    }
    else if (id == EVT_PROJECT_RELOAD)
    {
        file_watch_reload_project();
    }
    else if (id == UI_CLOSE_REQUEST_ID)
    {
        /* The framework deferred to us instead of just closing (see
         * UI_CLOSE_REQUEST_ID's own doc comment): `param` is the document
         * window whose close icon was clicked while it still had unsaved
         * changes. Ask before throwing them away, same as a normal text
         * editor. */
        ui_node* win = (ui_node*)param;
        g_pending_close_window = win;
        char msg[400];
        snprintf(msg, sizeof msg, "%s has unsaved changes.\nDiscard them?",
                 basename_of(ui_get_path(win)));
        ui_msgbox_button btns[] = {
            { " Discard ", EVT_CLOSE_DISCARD },
            { " Cancel ", 0 },
        };
        ui_message_box(g_screen, "Close", msg, btns, 2);
    }
    else if (id == EVT_CLOSE_DISCARD)
    {
        /* User confirmed discarding - actually close the window now (the
         * prompt itself already auto-closed). Deferred/freed for real via the
         * usual transient-window path - see ui_screen_take_closed_window and
         * app_frame(). */
        if (g_pending_close_window)
        {
            ui_screen_close_modal(g_screen, g_pending_close_window);
            g_pending_close_window = NULL;
        }
    }
    else if (id == EVT_FIND_CANCEL)
    {
        ui_screen_close_modal(g_screen, g_find.modal);
    }
    else if (id == EVT_SEARCH_REPLACE)
    {
        /* Remember which editor to search - the frontmost editor window's
         * editor - before the modal covers it. */
        g_replace.target = editor_in_window(ui_screen_top_window(g_screen));
        fill_from_selection(g_replace.target, g_replace.find);
        ui_screen_show_modal(g_screen, g_replace.modal);
    }
    else if (id == EVT_REPLACE_OK)
    {
        do_replace_one();
    }
    else if (id == EVT_REPLACE_CHANGEALL)
    {
        do_replace_all();
    }
    else if (id == EVT_REPLACE_CANCEL)
    {
        ui_screen_close_modal(g_screen, g_replace.modal);
    }
    else if (id == EVT_OPTIONS_ENV)
    {
        ui_select_set_selected(g_envdlg.theme_select, g_envdlg.theme_index);
        if (g_envdlg.font_select)
            ui_select_set_selected(g_envdlg.font_select, g_envdlg.font_index);
        ui_screen_show_modal(g_screen, g_envdlg.modal);
    }
    else if (id == EVT_ENV_OK)
    {
        ui_screen_close_modal(g_screen, g_envdlg.modal);
    }
    else if (id == EVT_ENV_THEME_AMBAR)
    {
        apply_theme(&g_theme_ambar);
        g_envdlg.theme_index = 0;
    }
    else if (id == EVT_ENV_THEME_DARK)
    {
        apply_theme(&g_theme_dark);
        g_envdlg.theme_index = 1;
    }
    else if (id == EVT_ENV_THEME_WHITE)
    {
        apply_theme(&g_theme_white);
        g_envdlg.theme_index = 2;
    }
    else if (id == EVT_ENV_THEME_NEBULA)
    {
        apply_theme(&g_theme_nebula);
        g_envdlg.theme_index = 3;
    }
    else if (id == EVT_ENV_THEME_XCODE_DARK)
    {
        apply_theme(&g_theme_xcode_dark);
        g_envdlg.theme_index = 4;
    }
    else if (id >= EVT_ENV_FONT_BASE &&
             id < EVT_ENV_FONT_BASE + ui_env_font_family_count(g_env))
    {
        /* Applies immediately, same as picking a theme - the backend
         * reopens the font, re-derives the cell size and resizes the
         * window to keep the same column/row count. */
        g_envdlg.font_index = id - EVT_ENV_FONT_BASE;
        ui_env_set_font_family(g_env, g_envdlg.font_index);
    }
    else if (id == EVT_FILE_NEW)
    {
        ui_screen_show_window(g_screen, make_new_editor_window(g_root, g_new_count++));
    }
    else if (id == EVT_FILE_OPEN)
    {
        /* An absolute path, not the symbolic "." - "up" from "." has
         * nothing to strip once already back at it, a dead end one
         * directory above the start (see open_path_up). */
        g_open.dialog_mode = OPEN_DLG_FILE;
        ui_set_label(g_open.window, " Open a File ");
        ui_set_label(g_open.ok, "  Open  ");
        if (!ui_get_cwd(g_open.dir, sizeof g_open.dir))
            strcpy(g_open.dir, ".");
        strcpy(g_open.mask, g_open_filters[0].mask);
        ui_select_set_selected(g_open.filter, 0);
        open_dialog_set_filter_visible(1);
        open_dialog_refresh();
        ui_screen_show_modal(g_screen, g_open.modal);
    }
    else if (id == EVT_FILE_OPENFOLDER)
    {
        /* Same dialog, folder-picker mode (see open_dialog_refresh/activate) -
         * OK becomes "Select", confirming the current directory rather than
         * opening a file (see folder_select_confirm). There's no mask in
         * folder mode (populate_listbox_from_dir gets a NULL mask - see
         * open_dialog_refresh), so the file-type filter doesn't apply here -
         * it's hidden rather than just disabled (see
         * open_dialog_set_filter_visible). */
        g_open.dialog_mode = OPEN_DLG_FOLDER;
        ui_set_label(g_open.window, " Open Folder ");
        ui_set_label(g_open.ok, " Select ");
        if (!ui_get_cwd(g_open.dir, sizeof g_open.dir))
            strcpy(g_open.dir, ".");
        open_dialog_set_filter_visible(0);
        open_dialog_refresh();
        ui_screen_show_modal(g_screen, g_open.modal);
    }
    else if (id == EVT_FILE_SAVEAS)
    {
        /* Reuse the Open dialog in "save" mode - same DOM, different
         * behavior (see save_as_activate/open_saveas_dialog_for). Save the
         * frontmost *document* window (g_active_editor_window, not
         * top_window() - see its declaration: a docked Folder/Output panel
         * can be frontmost instead, and editor_in_window() alone can't tell
         * the two apart); does nothing if there isn't one. */
        open_saveas_dialog_for(g_active_editor_window);
    }
    else if (id == EVT_PROJECT_NEW)
    {
        /* Show New Project dialog - user selects folder and enters project name */
        char cwd[1024] = { 0 };
        if (ui_get_cwd(cwd, sizeof cwd))
            ui_set_value(g_newproject.folder_input, cwd);
        else
            ui_set_value(g_newproject.folder_input, ".");
        ui_set_value(g_newproject.name_input, "");
        ui_screen_show_modal(g_screen, g_newproject.modal);
    }
    else if (id == EVT_PROJECT_NEW_BROWSE)
    {
        /* Same folder-picker dialog as File > Open Folder, retargeted so OK
         * drops the chosen directory back into the New Project dialog's
         * Folder field instead (see OPEN_DLG_NEWPROJECT_FOLDER above). */
        ui_screen_close_modal(g_screen, g_newproject.modal);
        g_open.dialog_mode = OPEN_DLG_NEWPROJECT_FOLDER;
        ui_set_label(g_open.window, " Select Folder ");
        ui_set_label(g_open.ok, " Select ");
        const char* cur = ui_get_value(g_newproject.folder_input);
        if (cur && cur[0])
            strncpy(g_open.dir, cur, sizeof g_open.dir - 1);
        else if (!ui_get_cwd(g_open.dir, sizeof g_open.dir))
            strcpy(g_open.dir, ".");
        g_open.dir[sizeof g_open.dir - 1] = 0;
        open_dialog_set_filter_visible(0);
        open_dialog_refresh();
        ui_screen_show_modal(g_screen, g_open.modal);
    }
    else if (id == EVT_PROJECT_NEW_OK)
    {
        const char* folder = ui_get_value(g_newproject.folder_input);
        const char* name = ui_get_value(g_newproject.name_input);
        int create_folder = ui_group_get_checked(g_newproject.helloworld_check, 0);
        int create_helloworld = ui_group_get_checked(g_newproject.helloworld_check, 1);

        if (!folder || !folder[0] || !name || !name[0])
        {
            /* Show error: both fields required */
            ui_msgbox_button ok = { "   OK   ", 0 };
            ui_message_box(g_screen, "New Project", "Please fill in both folder and project name.", &ok, 1);
            return;
        }

        int folder_has_sep = folder[0] && (folder[strlen(folder) - 1] == '/' ||
                                            folder[strlen(folder) - 1] == '\\');

        /* "Create Folder" checked: the project lives in a new <name>
         * subdirectory of the selected folder - create it now (fs.h's own
         * mkdir(path, mode) shim reports "already exists" the same way it
         * reports any other failure, which is exactly what's wanted here:
         * either way the user needs to know before anything gets written
         * into it). Unchecked: the project is written straight into the
         * folder the user picked, same as before. */
        char project_dir[1024];
        if (create_folder)
        {
            snprintf(project_dir, sizeof project_dir, "%s%s%s",
                     folder, folder_has_sep ? "" : "/", name);
            if (mkdir(project_dir, 0755) != 0)
            {
                ui_msgbox_button ok = { "   OK   ", 0 };
                char message[1200];
                snprintf(message, sizeof message,
                         "Could not create folder:\n%s\nIt may already exist.", project_dir);
                ui_message_box(g_screen, "New Project", message, &ok, 1);
                return;
            }
        }
        else
        {
            snprintf(project_dir, sizeof project_dir, "%s", folder);
        }

        int dir_has_sep = project_dir[0] && (project_dir[strlen(project_dir) - 1] == '/' ||
                                              project_dir[strlen(project_dir) - 1] == '\\');
        char proj_path[1024], main_path[1024];
        snprintf(proj_path, sizeof proj_path, "%s%s%s" CAKE_PROJECT_EXT,
                 project_dir, dir_has_sep ? "" : "/", name);
        snprintf(main_path, sizeof main_path, "%s%smain.c",
                 project_dir, dir_has_sep ? "" : "/");

        /* Check if project file already exists */
        FILE* test = fopen(proj_path, "r");
        if (test)
        {
            fclose(test);
            ui_msgbox_button ok = { "   OK   ", 0 };
            ui_message_box(g_screen, "New Project", "Project already exists at that location.", &ok, 1);
            return;
        }

        ui_screen_close_modal(g_screen, g_newproject.modal);

        /* Writes the real ".cakeproj" JSON (project_save()'s own format,
         * not a hand-rolled placeholder) and loads it into g_project - same
         * helper the old Save-As-style New Project dialog used. */
        project_new_create(proj_path);

        /* Rebuilt off g_project.dir (project_new_create's own normalized
         * slashes), not the locally-built `main_path` above - project_add_
         * file()'s relative-path check (project_make_relative) is a literal
         * strncmp against g_project.dir, so a mismatched separator style
         * there is exactly what stores this as an absolute path instead of
         * plain "main.c". */
        snprintf(main_path, sizeof main_path, "%s/main.c", g_project.dir);

        /* Hello World checked: write main.c unless one is already sitting
         * there (never overwrite existing source), then register it with
         * the project either way - project_add_file() itself no-ops if
         * it's already listed. */
        if (create_helloworld)
        {
            FILE* test_main = fopen(main_path, "r");
            if (test_main)
            {
                fclose(test_main);
            }
            else
            {
                FILE* mf = fopen(main_path, "w");
                if (mf)
                {
                    fprintf(mf, "#include <stdio.h>\n\n");
                    fprintf(mf, "int main(void)\n");
                    fprintf(mf, "{\n");
                    fprintf(mf, "    printf(\"Hello, world!\\n\");\n");
                    fprintf(mf, "    return 0;\n");
                    fprintf(mf, "}\n");
                    fclose(mf);
                }
            }
            project_add_file(main_path);
        }
    }
    else if (id == EVT_PROJECT_NEW_CANCEL)
    {
        ui_screen_close_modal(g_screen, g_newproject.modal);
    }
    else if (id == EVT_PROJECT_OPEN)
    {
        /* File-picker mode, filtered to ".cakeproj" - OK loads that project
         * instead of opening it as a text document (see project_open_file()). */
        g_open.dialog_mode = OPEN_DLG_PROJECT_OPEN;
        ui_set_label(g_open.window, " Open Project ");
        ui_set_label(g_open.ok, "  Open  ");
        if (!ui_get_cwd(g_open.dir, sizeof g_open.dir))
            strcpy(g_open.dir, ".");
        strncpy(g_open.mask, "*" CAKE_PROJECT_EXT, sizeof g_open.mask - 1);
        g_open.mask[sizeof g_open.mask - 1] = 0;
        /* Same "Files of type" dropdown as File > Open..., just defaulted to
         * the Cake Project entry (see g_open_filters) instead of C Source. */
        ui_select_set_selected(g_open.filter, CAKE_PROJECT_FILTER_INDEX);
        open_dialog_set_filter_visible(1);
        open_dialog_refresh();
        ui_screen_show_modal(g_screen, g_open.modal);
    }
    else if (id == EVT_PROJECT_ADD_FILE)
    {
        /* Menu item is disabled without an open project (see app_frame()'s
         * own g_project.menu_items_requiring_project loop), so this is only
         * ever reachable with one already open. */
        g_open.dialog_mode = OPEN_DLG_PROJECT_ADDFILE;
        ui_set_label(g_open.window, " Add Existing File ");
        ui_set_label(g_open.ok, "  Add  ");
        strncpy(g_open.dir, g_project.dir, sizeof g_open.dir - 1);
        g_open.dir[sizeof g_open.dir - 1] = 0;
        strncpy(g_open.mask, "*.c;*.h", sizeof g_open.mask - 1);
        g_open.mask[sizeof g_open.mask - 1] = 0;
        open_dialog_set_filter_visible(0);
        open_dialog_refresh();
        ui_screen_show_modal(g_screen, g_open.modal);
    }
    else if (id == EVT_GLOBAL_INCLUDES)
    {
        /* File > "Directories..." - the same dialog as Project > "Include
         * Directories...", bound to the global list instead (cake.json), so
         * it is available with or without a project open. */
        includes_edit_global();
        ui_set_label(g_project.includes_window, " System Directories ");
        ui_set_help(g_project.includes_window,
                    "System Directories: where `#include <...>` finds the platform's headers", "# System Directories\n\nwhere `#include <...>` finds the platform's headers\n"
                    "\n"
                    "The global include directory list, saved in `cake.json` next to the IDE (and the `cake` compiler, which reads the same file). It is used for every file that is not part of the open project - the Playground, a file opened on its own. A project has its own list instead (**Project > Include Directories...**); the two are never merged.\n"
                    "\n"
                    "Directories are searched in list order - **Up** / **Down** change it - and stored as full paths.\n"
                    "\n"
                    "Cake's own annotated headers (the `include` folder next to the executable) are always searched first and are not listed here. They pull in the real header with `#include_next`, continuing the search in these directories.\n"
                    "\n"
                    "**Detect** replaces the list with the include directories the platform compiler itself searches. On Windows it finds MSVC's headers with `vswhere.exe` and the Windows SDK's from the registry, so it works outside a Developer Command Prompt.\n"
                    "\n"
                    "To find them by hand:\n"
                    "\n"
                    "- Windows, from a Developer Command Prompt: `echo %INCLUDE%`\n"
                    "- Linux: `echo | gcc -E -Wp,-v -`\n"
                    "- macOS: `echo | clang -v -E -`");
        includes_set_detect_visible(!g_includes_editing.is_project);
        project_includes_dialog_refresh(0);
        ui_screen_show_modal(g_screen, g_project.includes_modal);
    }
    else if (id == EVT_PROJECT_INCLUDES)
    {
        /* Same "menu item already enforces this" reasoning as
         * EVT_PROJECT_ADD_FILE just above. */
        includes_edit_project();
        ui_set_label(g_project.includes_window, " Include Directories ");
        ui_set_help(g_project.includes_window,
                    "Include Directories: the open project's own `#include` search path", "# Include Directories\n\nthe open project's own `#include` search path\n"
                    "\n"
                    "Saved in the project's `.cakeproj`, stored relative to the project folder so the project can be moved or shared. **Build** (F7) always uses this list, and so does **Compile** when the active file belongs to the project.\n"
                    "\n"
                    "Directories are searched in list order - **Up** / **Down** change it.\n"
                    "\n"
                    "Files that are not part of the project use the global list instead (**File > System Directories...**); the two are never merged.");
        includes_set_detect_visible(!g_includes_editing.is_project);
        project_includes_dialog_refresh(0);
        ui_screen_show_modal(g_screen, g_project.includes_modal);
    }
    else if (id == EVT_PROJECT_INCLUDES_ADD)
    {
        /* Same Open dialog, folder-picker mode as Project > "Add Existing
         * File..." - opens on top of the Include Directories dialog (which
         * stays open underneath); project_add_include()'s own EVT_OPEN_OK
         * handler refreshes this dialog's listbox too, see
         * project_includes_dialog_refresh()'s call site there. */
        g_open.dialog_mode = OPEN_DLG_PROJECT_ADDINCLUDE;
        ui_set_label(g_open.window, " Add Include Directory ");
        ui_set_label(g_open.ok, " Select ");
        /* Start browsing where the list itself lives: the project directory,
         * or - for the global list - wherever the IDE was launched from,
         * since that list has no directory of its own. */
        if (g_includes_editing.is_project)
        {
            strncpy(g_open.dir, g_project.dir, sizeof g_open.dir - 1);
            g_open.dir[sizeof g_open.dir - 1] = 0;
        }
        else if (!ui_get_cwd(g_open.dir, sizeof g_open.dir))
        {
            strcpy(g_open.dir, ".");
        }
        open_dialog_set_filter_visible(0);
        open_dialog_refresh();
        ui_screen_show_modal(g_screen, g_open.modal);
    }
    else if (id == EVT_PROJECT_INCLUDES_LISTBOX)
    {
        /* No per-row action - a directory listing has nothing to "activate"
         * into, unlike the Project panel's own files (see
         * project_window_activate()). Selecting a row is enough on its own
         * for "Remove" to act on it. */
    }
    else if (id == EVT_PROJECT_INCLUDES_REMOVE)
    {
        int sel = ui_select_get_selected(g_project.includes_listbox);
        if (g_includes_editing.dirs && sel >= 0 && sel < *g_includes_editing.count)
        {
            for (int i = sel; i + 1 < *g_includes_editing.count; i++)
                snprintf(g_includes_editing.dirs[i], 512, "%s", g_includes_editing.dirs[i + 1]);
            (*g_includes_editing.count)--;

            if (g_includes_editing.is_project)
                project_save();
            else
                global_settings_save();

            int selected_index = sel < *g_includes_editing.count ? sel : *g_includes_editing.count - 1;
            project_includes_dialog_refresh(selected_index);
        }
    }
    else if (id == EVT_PROJECT_INCLUDES_UP || id == EVT_PROJECT_INCLUDES_DOWN)
    {
        /* Include directories are searched in the order they are listed, so
         * moving one is a real edit - saved to the same file the list came
         * from, exactly like Add/Remove. The selection follows the row that
         * moved, so repeated clicks keep walking it along. */
        int sel = ui_select_get_selected(g_project.includes_listbox);
        int other = sel + (id == EVT_PROJECT_INCLUDES_DOWN ? 1 : -1);

        if (g_includes_editing.dirs &&
            sel >= 0 && sel < *g_includes_editing.count &&
            other >= 0 && other < *g_includes_editing.count)
        {
            char swap[512];
            snprintf(swap, sizeof swap, "%s", g_includes_editing.dirs[sel]);
            snprintf(g_includes_editing.dirs[sel], 512, "%s", g_includes_editing.dirs[other]);
            snprintf(g_includes_editing.dirs[other], 512, "%s", swap);

            if (g_includes_editing.is_project)
                project_save();
            else
                global_settings_save();

            project_includes_dialog_refresh(other);
        }
    }
    else if (id == EVT_PROJECT_INCLUDES_DETECT)
    {
        /* Replaces the whole list, like cake -autoconfig. Nothing found
         * keeps the current list rather than wiping it. The button only
         * exists for the global list (includes_set_detect_visible), but
         * check anyway so it can never overwrite a project's own. */
        if (g_includes_editing.dirs && !g_includes_editing.is_project)
        {
            char found[CAKE_PROJECT_MAX_INCLUDES][512];
            char problems[2048];
            int n = detect_system_include_dirs(found, CAKE_PROJECT_MAX_INCLUDES,
                                               problems, sizeof problems);

            /* Always reports back: what was found (and anything missing
             * alongside it), or why nothing was. */
            struct exttool_buf msg = { 0 };
            if (n > 0)
            {
                memcpy(g_includes_editing.dirs, found, sizeof found);
                *g_includes_editing.count = n;
                bool saved = global_settings_save();
                project_includes_dialog_refresh(0);

                char settings_path[FS_MAX_PATH];
                if (!get_global_settings_path(settings_path, sizeof settings_path))
                    snprintf(settings_path, sizeof settings_path, "cake.json");
                exttool_append(&msg, saved ? "System directories detected successfully.\n\nSaved to:\n"
                                           : "System directories were detected, but could not be saved to:\n");
                exttool_append(&msg, settings_path);
                exttool_append(&msg, "\n");
                if (problems[0])
                {
                    exttool_append(&msg, "\nWarning:\n");
                    exttool_append(&msg, problems);
                }
            }
            else
            {
                exttool_append(&msg, "Detection failed - the list was not changed.\n\n");
                exttool_append(&msg, problems);
            }

            ui_msgbox_button ok = { "   OK   ", 0 };
            ui_message_box(g_screen, "Detect", exttool_buf_text(&msg), &ok, 1);
            exttool_buf_free(&msg);
        }
    }
    else if (id == EVT_PROJECT_INCLUDES_CLOSE)
    {
        ui_screen_close_modal(g_screen, g_project.includes_modal);
    }
    else if (id == EVT_PROJECT_CLOSE)
    {
        project_close();
    }
    else if (id == EVT_WINDOW_PROJECT)
    {
        /* Always raises the panel, never closes it - see EVT_WINDOW_OUTPUT's
         * own doc comment for why these View items stopped toggling. */
        project_show_panel();
    }
    else if (id == EVT_PROJECT_LISTBOX)
    {
        project_window_activate(ui_select_get_selected(g_project.listbox));
    }
    else if (id == EVT_PROJECT_POPUP_OPEN)
    {
        project_open_at(ui_select_get_selected(g_project.listbox));
    }
    else if (id == EVT_PROJECT_POPUP_REMOVE)
    {
        project_remove_at(ui_select_get_selected(g_project.listbox));
    }
    else if (id == EVT_OPEN_NAME)
    {
        if (g_open.dialog_mode == OPEN_DLG_SAVE)
        {
            save_as_activate();
        }
        else if (g_open.dialog_mode == OPEN_DLG_PROJECT_NEW)
        {
            project_new_save_activate();
        }
        else if (g_open.dialog_mode == OPEN_DLG_FOLDER ||
                 g_open.dialog_mode == OPEN_DLG_PROJECT_ADDINCLUDE ||
                 g_open.dialog_mode == OPEN_DLG_NEWPROJECT_FOLDER ||
                 g_open.dialog_mode == OPEN_DLG_GITCLONE_FOLDER)
        {
            /* The whole field is the target directory in folder mode -
             * there's no mask to split off. Normalize back to '/' first -
             * the field displays backslashes cosmetically (open_dialog_
             * refresh), which aren't a valid separator on Linux/macOS, so
             * an unedited default would otherwise get baked in literally
             * (see save_as_activate for the same fix on the Save side). */
            char text[1024];
            strncpy(text, ui_get_value(g_open.name_input), sizeof text - 1);
            text[sizeof text - 1] = 0;
            for (char* p = text; *p; p++)
                if (*p == '\\')
                    *p = '/';
            /* Tolerate a trailing separator, except on a bare root - same
             * fix as the dir_only stripping just below for the non-folder
             * branch. Left in place, a later "%s/%s" join (open_dialog_
             * activate, folder_select_confirm's callers, ...) would double
             * up the separator instead of just adding one. */
            {
                size_t len = strlen(text);
                while (len > 1 && text[len - 1] == '/')
                    text[--len] = 0;
            }
            if (text[0])
            {
                strncpy(g_open.dir, text, sizeof g_open.dir - 1);
                g_open.dir[sizeof g_open.dir - 1] = 0;
            }
            open_dialog_refresh();
        }
        else
        {
            /* The user typed a new path/mask and pressed Enter - split it at
             * the last separator (either style) into directory + mask, same as
             * the real dialog's Name field. */
            const char* text = ui_get_value(g_open.name_input);
            char buf[1024];
            strncpy(buf, text, sizeof buf - 1);
            buf[sizeof buf - 1] = 0;
            for (char* p = buf; *p; p++)
                if (*p == '\\')
                    *p = '/';

            /* If the whole field is already a real, existing file (e.g. a
             * full path just pasted in), open it straight away instead of
             * treating it as a dir+mask to navigate to - matches what a
             * user pressing Enter/OK on a pasted path expects. Falls
             * through to the split/navigate behavior below for anything
             * that isn't an existing file (a directory, a bare mask like
             * *.c, a not-yet-existing name, ...). */
            if (path_is_regular_file(buf))
            {
                nav_record_jump();
                open_file_path_into_editor(buf, basename_of(buf));
                ui_screen_close_modal(g_screen, g_open.modal);
                return;
            }

            /* A plain directory path is navigation, not a new mask: keep
             * the mask the "Files of type" dropdown is showing so the
             * listing stays filtered. Without this the split below would
             * take the last component ("src" in /home/me/src) - or the empty
             * text after a trailing separator - as the mask and show
             * everything. */
            {
                char dir_only[1024];
                strncpy(dir_only, buf, sizeof dir_only - 1);
                dir_only[sizeof dir_only - 1] = 0;
                size_t len = strlen(dir_only);
                /* Tolerate a trailing separator, except on a bare root. */
                while (len > 1 && dir_only[len - 1] == '/')
                {
                    dir_only[--len] = 0;
                }
                struct stat st;
                /* No POSIX S_ISDIR under MSVC, same as path_is_regular_file. */
                if (dir_only[0] && stat(dir_only, &st) == 0 &&
                    (st.st_mode & S_IFMT) == S_IFDIR)
                {
                    strncpy(g_open.dir, dir_only, sizeof g_open.dir - 1);
                    g_open.dir[sizeof g_open.dir - 1] = 0;
                    open_dialog_refresh();
                    return;
                }
            }

            char* last_sep = NULL;
            for (char* p = buf; *p; p++)
                if (*p == '/')
                    last_sep = p;
            if (last_sep)
            {
                *last_sep = 0;
                strncpy(g_open.mask, last_sep + 1, sizeof g_open.mask - 1);
                g_open.mask[sizeof g_open.mask - 1] = 0;
                if (buf[0])
                {
                    strncpy(g_open.dir, buf, sizeof g_open.dir - 1);
                    g_open.dir[sizeof g_open.dir - 1] = 0;
                }
            }
            else if (buf[0])
            {
                strncpy(g_open.mask, buf, sizeof g_open.mask - 1);
                g_open.mask[sizeof g_open.mask - 1] = 0;
            }
            open_dialog_refresh();
        }
    }
    else if (id >= EVT_OPEN_FILTER && id < EVT_OPEN_FILTER + OPEN_FILTER_COUNT)
    {
        /* "Files of type" picked from the dropdown - same effect as typing
         * that mask into the Name field (EVT_OPEN_NAME's non-Save branch),
         * just friendlier. Works in both File-Open and Save-As mode: it
         * always re-filters g_open.listbox; in File-Open mode it also
         * replaces the mask portion of the Name field's text, since that
         * field *is* dir+mask there (in Save mode the field is the target
         * filename instead, so it's left alone - see open_dialog_refresh). */
        int idx = id - EVT_OPEN_FILTER;
        strncpy(g_open.mask, g_open_filters[idx].mask, sizeof g_open.mask - 1);
        g_open.mask[sizeof g_open.mask - 1] = 0;
        open_dialog_refresh();
    }
    else if (id == EVT_OPEN_LISTBOX)
    {
        /* A row was activated: directories navigate in either mode; a file
         * row opens it (open mode) or drops its name in the Name field (save
         * mode) - see open_dialog_activate. */
        open_dialog_activate(ui_select_get_selected(g_open.listbox));
    }
    else if (id == EVT_OPEN_OK)
    {
        if (g_open.dialog_mode == OPEN_DLG_SAVE)
            save_as_activate();
        else if (g_open.dialog_mode == OPEN_DLG_FOLDER)
            folder_select_confirm();
        else if (g_open.dialog_mode == OPEN_DLG_PROJECT_NEW)
            project_new_save_activate();
        else if (g_open.dialog_mode == OPEN_DLG_PROJECT_ADDINCLUDE)
        {
            project_add_include(g_open.dir);
            ui_screen_close_modal(g_screen, g_open.modal);
            g_open.dialog_mode = OPEN_DLG_FILE;
        }
        else if (g_open.dialog_mode == OPEN_DLG_NEWPROJECT_FOLDER)
        {
            ui_set_value(g_newproject.folder_input, g_open.dir);
            ui_screen_close_modal(g_screen, g_open.modal);
            g_open.dialog_mode = OPEN_DLG_FILE;
            ui_screen_show_modal(g_screen, g_newproject.modal);
        }
        else if (g_open.dialog_mode == OPEN_DLG_GITCLONE_FOLDER)
        {
            /* The chosen folder is the parent - the repository name is
             * appended, same as the URL-driven suggestion. */
            char name[256];
            git_url_repo_name(g_gitclone.last_url, name, sizeof name);
            git_clone_set_folder(g_open.dir, name);
            ui_screen_close_modal(g_screen, g_open.modal);
            g_open.dialog_mode = OPEN_DLG_FILE;
            ui_screen_show_modal(g_screen, g_gitclone.modal);
        }
        else if (g_open.dialog_mode == OPEN_DLG_EXTTOOL_CMD)
        {
            /* Same "typed/pasted a full path" shortcut as the other modes;
             * otherwise act on the selected row. */
            char buf[1024];
            strncpy(buf, ui_get_value(g_open.name_input), sizeof buf - 1);
            buf[sizeof buf - 1] = 0;
            for (char* p = buf; *p; p++)
                if (*p == '\\')
                    *p = '/';
            if (path_is_regular_file(buf))
            {
                exttool_browse_pick(buf);
                ui_screen_close_modal(g_screen, g_open.modal);
                g_open.dialog_mode = OPEN_DLG_FILE;
            }
            else
                open_dialog_activate(ui_select_get_selected(g_open.listbox));
        }
        else if (g_open.dialog_mode == OPEN_DLG_PROJECT_OPEN || g_open.dialog_mode == OPEN_DLG_PROJECT_ADDFILE)
        {
            /* Same "pasted a full path" shortcut as the generic branch below:
             * if the Name field itself already names an existing file, OK
             * acts on it directly rather than on whatever row happens to be
             * selected in the listbox. */
            char buf[1024];
            strncpy(buf, ui_get_value(g_open.name_input), sizeof buf - 1);
            buf[sizeof buf - 1] = 0;
            for (char* p = buf; *p; p++)
                if (*p == '\\')
                    *p = '/';
            if (path_is_regular_file(buf))
            {
                if (g_open.dialog_mode == OPEN_DLG_PROJECT_OPEN)
                    project_open_file(buf);
                else
                    project_add_file(buf);
                ui_screen_close_modal(g_screen, g_open.modal);
                g_open.dialog_mode = OPEN_DLG_FILE;
            }
            else if (g_open.allow_multi && open_dialog_add_checked_files())
            {
                /* Add Existing File... in its multi-select shape: every
                 * checked row is added in one go (see
                 * open_dialog_add_checked_files, which returns 0 when
                 * nothing is checked so a plain single pick still falls
                 * through to open_dialog_activate below). */
                ui_screen_close_modal(g_screen, g_open.modal);
                g_open.dialog_mode = OPEN_DLG_FILE;
            }
            else
                open_dialog_activate(ui_select_get_selected(g_open.listbox));
        }
        else
        {
            /* Same "pasted a full path" shortcut as EVT_OPEN_NAME: if the
             * Name field itself already names an existing file, OK opens it
             * directly rather than acting on whatever row happens to be
             * selected in the listbox (which may be stale/unrelated to what
             * was just typed/pasted). */
            char buf[1024];
            strncpy(buf, ui_get_value(g_open.name_input), sizeof buf - 1);
            buf[sizeof buf - 1] = 0;
            for (char* p = buf; *p; p++)
                if (*p == '\\')
                    *p = '/';
            if (path_is_regular_file(buf))
            {
                nav_record_jump();
                open_file_path_into_editor(buf, basename_of(buf));
                ui_screen_close_modal(g_screen, g_open.modal);
            }
            else
                open_dialog_activate(ui_select_get_selected(g_open.listbox));
        }
    }
    else if (id == EVT_OPEN_CANCEL)
    {
        int was_newproject_browse = (g_open.dialog_mode == OPEN_DLG_NEWPROJECT_FOLDER);
        int was_gitclone_browse = (g_open.dialog_mode == OPEN_DLG_GITCLONE_FOLDER);
        ui_screen_close_modal(g_screen, g_open.modal);
        g_open.dialog_mode = OPEN_DLG_FILE;

        /* Canceling out of a Save As that a Compile opened (see do_compile's
         * untitled-file check) aborts the compile too, rather than going
         * ahead and compiling a placeholder path - there's nothing sensible
         * to compile until the file actually has a real name. */
        g_pending_compile_after_saveas = 0;

        /* Canceling the New Project dialog's own folder browse returns to
         * that dialog instead of dropping the user back at the editor - the
         * browse was launched from inside it (see EVT_PROJECT_NEW_BROWSE). */
        if (was_newproject_browse)
            ui_screen_show_modal(g_screen, g_newproject.modal);

        /* Same, for the Clone dialog's own folder browse (see
         * EVT_GITCLONE_BROWSE). */
        if (was_gitclone_browse)
            ui_screen_show_modal(g_screen, g_gitclone.modal);
    }
    else if (id == EVT_FILE_SAVE)
    {
        /* Write the active editor window to its file (see save_active_file).
         * g_active_editor_window, not top_window() - a docked Folder/Output
         * panel can be frontmost instead (see g_active_editor_window). Still
         * untitled (see ui_get_untitled) - e.g. straight from File > New -
         * means there's no real file to write yet, so this behaves like
         * Save As instead, exactly like a normal text editor's Save on a
         * never-saved document. */
        if (ui_get_untitled(g_active_editor_window))
            open_saveas_dialog_for(g_active_editor_window);
        else
            save_active_file(g_active_editor_window);
    }
    else if (id == EVT_FILE_SAVEALL)
    {
        /* Save every open editor window. */
        for (int i = 0; i < ui_child_count(g_root); i++)
        {
            ui_node* w = ui_child_at(g_root, i);
            if (editor_in_window(w))
                save_active_file(w);
        }
    }
    else if (id == EVT_WINDOW_TILE)
    {
        tile_windows();
    }
    else if (id == EVT_WINDOW_CASCADE)
    {
        cascade_windows();
    }
    else if (id == EVT_WINDOW_CLOSEALL)
    {
        /* Document windows only - the docked Folder/Output panels and the
         * Find/Replace panel are in the same window stack, but they're app
         * furniture, not open files, and "Close all" closing them too meant
         * the command wiped out the whole workspace layout. is_editor_window()
         * is the same "only a document carries a path" test the rest of the
         * file uses. Iterating downward because closing compacts the stack. */
        for (int i = ui_screen_window_count(g_screen) - 1; i >= 0; i--)
        {
            ui_node* w = ui_screen_window_at(g_screen, i);
            if (is_editor_window(w))
                ui_screen_close_modal(g_screen, w);
        }
    }
    else if (id == EVT_WINDOW_FONT_INC)
    {
        ui_env_adjust_font_size(g_env, 1);
    }
    else if (id == EVT_WINDOW_FONT_DEC)
    {
        ui_env_adjust_font_size(g_env, -1);
    }
    else if (id == EVT_WINDOW_OUTPUT)
    {
        /* Always raises the panel, never closes it - these View items are
         * plain "show" commands now, not open/close toggles (no [x]/[ ] on
         * them any more either - see the View menu's own build_screen()
         * comment). */
        ui_screen_show_window(g_screen, g_output_window);
    }
    else if (id == EVT_WINDOW_DEBUGINFO)
    {
        ui_screen_show_window(g_screen, g_debuginfo_window);
    }
    else if (id == EVT_WINDOW_FOLDER)
    {
        folder_show_panel();
    }
    else if (id == EVT_WINDOW_GIT)
    {
        git_show_panel();
    }
    else if (id == EVT_WINDOW_PLAYGROUND)
    {
        /* Reuses the already-open Playground if there is one (same lookup
         * open_file_path_into_editor() itself does), else creates it - never
         * closes an existing one. */
        char playground_path[FS_MAX_PATH];
        ui_node* existing = get_playground_file_path(playground_path, sizeof playground_path)
            ? find_open_window(playground_path) : NULL;
        if (existing)
            ui_screen_show_window(g_screen, existing);
        else
            open_playground();
    }
    else if (id == EVT_VIEW_LINENUMBERS)
    {
        /* Toggle, matching the "[x]"/"[ ]" the View menu shows for this item
         * (see refresh_view_item) - takes effect immediately on every open
         * editor, since render_editor() reads ui_get_show_line_numbers()
         * fresh on each repaint rather than caching it per-window. */
        ui_set_show_line_numbers(!ui_get_show_line_numbers());
    }
    else if (id == EVT_TOOLS_FINDREPLACE)
    {
        /* ui_screen_show_window() docks it (see ui_set_dock) - that call
         * runs dock_layout() synchronously, so the panel's <window> is
         * already at its real on-screen rect by the time this line returns.
         * But its existing children (built back in app_init at the panel's
         * small pre-dock size) only get carried to that rect via
         * set_window_rect()'s dx/dy shifting, which anchors each child to
         * whichever half of the OLD rect it was closer to (see
         * set_window_rect in ide_ui.c) - fine for a plain move, but this
         * panel's content is taller than the small rect it was first built
         * against, so several controls sit in what looks like the "bottom
         * half" and pick up an extra, wrong shift once the dock stretches
         * the window to the screen's full height. Explicitly relaying out
         * now - after the dock, using ui_get_rect() to read the now-correct
         * rect - sidesteps that shift heuristic entirely instead of
         * fighting it. */
        /* Pre-fill the Find field with the active editor's selection, same as
         * Search > Find/Replace do. Goes through g_active_editor_window rather
         * than ui_screen_top_window() for the same reason
         * fr_search_current_file() does: this panel is docked, so once it's
         * open top_window() reports the panel itself, not the document behind
         * it. Written into g_fr.find_text so fr_rebuild_content() (which
         * rebuilds the input from scratch just below) picks it up. */
        copy_selection(editor_in_window(g_active_editor_window),
                       g_fr.find_text, sizeof g_fr.find_text);

        ui_screen_show_window(g_screen, g_fr.window);
        fr_rebuild_content();
        ui_get_rect(g_fr.panel, NULL, NULL, &g_fr.last_w, NULL);

        /* Land the caret in the Find field right away rather than leaving
         * the user to click it - ui_screen_show_window() itself blurs
         * whatever was focused (see its own doc comment), and
         * fr_rebuild_content() just built a brand new g_fr.find_input node,
         * so this has to happen last, after both. */
        ui_screen_focus(g_screen, g_fr.find_input);
    }
    else if (id == EVT_TOOLS_EXTERNAL)
    {
        /* Open on a fresh working copy of the real list - see g_exttool. */
        memcpy(g_exttool.edit, g_tools.items, sizeof g_exttool.edit);
        g_exttool.count = g_tools.count;
        g_exttool.sel = g_tools.count > 0 ? 0 : -1;
        exttool_refresh_list();
        exttool_load_fields();
        ui_screen_show_modal(g_screen, g_exttool.modal);
    }
    else if (id == EVT_EXTTOOL_LIST)
    {
        /* Selection moved: bank the fields into the row being left, then
         * load the newly selected one. */
        exttool_store_fields();
        g_exttool.sel = ui_select_get_selected(g_exttool.listbox);
        exttool_load_fields();
    }
    else if (id == EVT_EXTTOOL_ADD)
    {
        exttool_store_fields();
        if (g_exttool.count < EXT_TOOL_MAX)
        {
            ext_tool* t = &g_exttool.edit[g_exttool.count];
            memset(t, 0, sizeof *t);
            snprintf(t->title, sizeof t->title, "New Tool");
            g_exttool.sel = g_exttool.count++;
            exttool_refresh_list();
            exttool_load_fields();
        }
    }
    else if (id == EVT_EXTTOOL_DELETE)
    {
        if (g_exttool.sel >= 0 && g_exttool.sel < g_exttool.count)
        {
            for (int i = g_exttool.sel; i < g_exttool.count - 1; i++)
                g_exttool.edit[i] = g_exttool.edit[i + 1];
            g_exttool.count--;
            exttool_refresh_list();
            exttool_load_fields();
        }
    }
    else if (id == EVT_EXTTOOL_MOVEUP || id == EVT_EXTTOOL_MOVEDOWN)
    {
        /* The list order IS the Tools-menu order, so this is how the menu
         * gets arranged. Fields are banked first, since the row moves with
         * whatever is currently typed into it. */
        exttool_store_fields();
        int from = g_exttool.sel;
        int to = from + (id == EVT_EXTTOOL_MOVEUP ? -1 : 1);
        if (from >= 0 && from < g_exttool.count && to >= 0 && to < g_exttool.count)
        {
            ext_tool tmp = g_exttool.edit[from];
            g_exttool.edit[from] = g_exttool.edit[to];
            g_exttool.edit[to] = tmp;
            g_exttool.sel = to;      /* selection follows the row it moved */
            exttool_refresh_list();
            exttool_load_fields();
        }
    }
    else if (id == EVT_EXTTOOL_BROWSE)
    {
        /* Opens on top of the External Tools dialog, which stays up
         * underneath - same stacked-modal shape Project > "Include
         * Directories..."'s own "Add..." uses. OK drops the picked path into
         * the Command field, see exttool_browse_pick(). */
        g_open.dialog_mode = OPEN_DLG_EXTTOOL_CMD;
        ui_set_label(g_open.window, " Select Program ");
        ui_set_label(g_open.ok, "  Open  ");
        if (!ui_get_cwd(g_open.dir, sizeof g_open.dir))
            strcpy(g_open.dir, ".");
        strncpy(g_open.mask, g_open_filters[PROGRAM_FILTER_INDEX].mask,
                sizeof g_open.mask - 1);
        g_open.mask[sizeof g_open.mask - 1] = 0;
        ui_select_set_selected(g_open.filter, PROGRAM_FILTER_INDEX);
        open_dialog_set_filter_visible(1);
        open_dialog_refresh();
        ui_screen_show_modal(g_screen, g_open.modal);
    }
    else if (id >= EVT_EXTTOOL_MACRO_BTN && id < EVT_EXTTOOL_MACRO_BTN + 2)
    {
        /* Open the macro list just under the ">" that was clicked, aligned
         * with its own field so the two read as one control. */
        ui_node* field = g_exttool.macro_fields[id - EVT_EXTTOOL_MACRO_BTN];
        int fx, fy, fw, fh;
        ui_get_rect(field, &fx, &fy, &fw, &fh);
        g_exttool.macro_target = field;
        ui_screen_open_popup(g_screen, g_exttool.macro_popup, fx + fw, fy + 1, NULL);
    }
    else if (id >= EVT_EXTTOOL_MACRO_BASE && id < EVT_EXTTOOL_MACRO_BASE + EXT_MACRO_COUNT)
    {
        exttool_insert_macro(g_exttool.macro_target,
                             ext_macros[id - EVT_EXTTOOL_MACRO_BASE].name);
        /* The field is the natural place to be afterwards - the caret is
         * already sitting past what was just inserted. */
        ui_screen_focus(g_screen, g_exttool.macro_target);
        g_exttool.macro_target = NULL;
    }
    else if (id == EVT_EXTTOOL_OK)
    {
        exttool_store_fields();
        memcpy(g_tools.items, g_exttool.edit, sizeof g_tools.items);
        g_tools.count = g_exttool.count;
        rebuild_tools_menu();
        ui_screen_close_modal(g_screen, g_exttool.modal);
    }
    else if (id == EVT_EXTTOOL_CANCEL)
    {
        ui_screen_close_modal(g_screen, g_exttool.modal);  /* working copy discarded */
    }
    else if (id >= EVT_TOOL_RUN_BASE && id < EVT_TOOL_RUN_BASE + EXT_TOOL_MAX)
    {
        do_run_external_tool(id - EVT_TOOL_RUN_BASE);
    }
    else if (id == EVT_TOOLS_TERMINAL)
    {
        do_open_terminal();
    }
    else if (id == EVT_FR_MODE)
    {
        fr_sync_from_widgets();
        g_fr.mode = !g_fr.mode;
        fr_rebuild_content();
    }
    else if (id == EVT_FR_FIND_BTN || id == EVT_FR_REPLACE_BTN)
    {
        fr_sync_from_widgets();

        find_replace_options opts;
        memset(&opts, 0, sizeof opts);
        opts.mode = (id == EVT_FR_REPLACE_BTN) ? 1 : 0;
        snprintf(opts.find_text, sizeof opts.find_text, "%s", g_fr.find_text);
        snprintf(opts.replace_text, sizeof opts.replace_text, "%s", g_fr.replace_text);
        opts.match_case = g_fr.match_case;
        opts.match_whole_word = g_fr.match_word;
        opts.look_in = (fr_look_in)g_fr.look_in;
        opts.file_type = (fr_file_type)g_fr.file_type;
        do_find_replace(&opts);
    }
    else if (id == EVT_FOLDER_LISTBOX)
    {
        folder_window_activate(ui_select_get_selected(g_folder.listbox));
    }
    else if (id == EVT_GIT_LISTBOX)
    {
        git_window_activate(ui_select_get_selected(g_git.listbox));
    }
    else if (id == EVT_GIT_COMMIT_BTN)
    {
        git_commit_start();
    }
    else if (id == EVT_GITCOMMIT_OK)
    {
        git_commit_confirm();
    }
    else if (id == EVT_GITCOMMIT_CANCEL)
    {
        ui_screen_close_modal(g_screen, g_gitcommit.modal);
    }
    else if (id == EVT_GITDIFF_PREV)
    {
        git_diff_goto_change(-1);
    }
    else if (id == EVT_GITDIFF_NEXT)
    {
        git_diff_goto_change(1);
    }
    else if (id == EVT_GIT_DISCARD_BTN)
    {
        git_do_discard();
    }
    else if (id == EVT_GIT_DISCARD_CONFIRM)
    {
        /* g_pending_git_discard_path/g_pending_git_untracked were set right
         * before the confirm box above - see git_do_discard()'s own doc
         * comment. */
        if (g_pending_git_discard_path[0])
        {
            const char* root = g_git.root[0] ? g_git.root : g_folder.dir;
            char cmd[1200];
            struct exttool_buf out = { 0 };
            if (g_pending_git_untracked)
            {
                /* a staged new file ("A") is not in HEAD either - drop it from the index, then delete it */
                snprintf(cmd, sizeof cmd, "git reset -q -- \"%s\"", g_pending_git_discard_path);
                run_process_capture(cmd, root, &out);
                char full[1400];
                path_join(full, sizeof full, root, g_pending_git_discard_path);
                remove(full);
            }
            else
            {
                /* HEAD, not the index: also drops whatever was staged */
                snprintf(cmd, sizeof cmd, "git checkout HEAD -- \"%s\"", g_pending_git_discard_path);
                run_process_capture(cmd, root, &out);
            }
            exttool_buf_free(&out);
            g_pending_git_discard_path[0] = 0;
            git_panel_refresh();
        }
    }
    else if (id == EVT_GIT_PULL_BTN)
    {
        git_do_pull();
    }
    else if (id == EVT_GIT_PUSH_BTN)
    {
        git_do_push();
    }
    else if (id == EVT_GIT_COMMITFILE_BTN)
    {
        git_commitfile_start();
    }
    else if (id == EVT_GIT_COMMITSTAGED_BTN)
    {
        git_commitstaged_start();
    }
    else if (id == EVT_GIT_COMMITSTAGEDPUSH_BTN)
    {
        git_commitstagedpush_start();
    }
    else if (id == EVT_GIT_STAGE_BTN)
    {
        git_do_stage();
    }
    else if (id == EVT_GIT_UNSTAGE_BTN)
    {
        git_do_unstage();
    }
    else if (id == EVT_GIT_BRANCH_BTN)
    {
        git_branch_start();
    }
    else if (id == EVT_GITBRANCH_CHECKOUT)
    {
        git_branch_checkout();
    }
    else if (id == EVT_GITBRANCH_NEW)
    {
        git_branch_new();
    }
    else if (id == EVT_GITBRANCH_CANCEL)
    {
        ui_screen_close_modal(g_screen, g_gitbranch.modal);
    }
    else if (id == EVT_GIT_COMMITPUSH_BTN)
    {
        git_commitpush_start();
    }
    else if (id == EVT_GIT_SYNC_BTN)
    {
        git_do_sync();
    }
    else if (id == EVT_GIT_CLONE_BTN)
    {
        git_clone_start();
    }
    else if (id == EVT_GITCLONE_OK)
    {
        git_clone_confirm();
    }
    else if (id == EVT_GITCLONE_CANCEL)
    {
        ui_screen_close_modal(g_screen, g_gitclone.modal);
    }
    else if (id == EVT_GITCLONE_BROWSE)
    {
        /* Same folder-picker dialog as EVT_PROJECT_NEW_BROWSE, retargeted so
         * OK drops the chosen directory back into the Clone dialog's Folder
         * field instead (see OPEN_DLG_GITCLONE_FOLDER above). */
        ui_screen_close_modal(g_screen, g_gitclone.modal);
        g_open.dialog_mode = OPEN_DLG_GITCLONE_FOLDER;
        ui_set_label(g_open.window, " Select Folder ");
        ui_set_label(g_open.ok, " Select ");
        /* Browse from the parent: the full destination doesn't exist yet. */
        char cur[1024];
        git_clone_folder_parent(cur, sizeof cur);
        if (cur[0])
            strncpy(g_open.dir, cur, sizeof g_open.dir - 1);
        else if (!ui_get_cwd(g_open.dir, sizeof g_open.dir))
            strcpy(g_open.dir, ".");
        g_open.dir[sizeof g_open.dir - 1] = 0;
        open_dialog_set_filter_visible(0);
        open_dialog_refresh();
        ui_screen_show_modal(g_screen, g_open.modal);
    }
    else if (id == EVT_EDIT_UNDO)
    {
        ui_screen_undo(g_screen);
    }
    else if (id == EVT_EDIT_REDO)
    {
        ui_screen_redo(g_screen);
    }
    else if (id == EVT_EDIT_CUT)
    {
        ui_screen_cut(g_screen);
    }
    else if (id == EVT_EDIT_COPY)
    {
        ui_screen_copy(g_screen);
    }
    else if (id == EVT_EDIT_PASTE)
    {
        ui_screen_paste(g_screen);
    }
    else if (id == EVT_EDIT_STRINGIFY)
    {
        do_edit_stringify();
    }
    else if (id == EVT_EDIT_TOUPPER)
    {
        do_edit_case(1);
    }
    else if (id == EVT_EDIT_TOLOWER)
    {
        do_edit_case(0);
    }
    else if (id == EVT_EDIT_FORMAT)
    {
        do_edit_format();
    }
}

/* --- Session persistence: remembers "what the IDE was doing" across runs -
 * compiler options/target, the Folder panel's browsed directory, the active
 * document's path/caret/scroll, and the on-screen layout (main window +
 * Folder/Output dock sizes) - so the next launch picks up where this one
 * left off. Saved once on exit (see app_frame's g_quit branch) and loaded
 * once at startup (see app_init, right after every window it touches
 * already exists). Plain "key=value" text, one per line, in the platform's
 * per-user config directory - never binary, so it's easy to read, hand-
 * edit, or just delete to reset. --- */

/* %APPDATA%\cake_ide on Windows, ~/Library/Application Support/cake_ide on
 * macOS, $XDG_CONFIG_HOME/cake_ide (or ~/.config/cake_ide if that's unset)
 * elsewhere - the one config directory session.json (get_session_file_path)
 * and playground.c (get_playground_file_path) both live in. Creates it if it
 * doesn't exist yet. Returns 0 if the relevant environment variable isn't
 * set at all (e.g. a stripped-down container) - callers each fail the same
 * way this does (session persistence/Playground's file just silently don't
 * happen, rather than erroring). */
static int get_config_dir(char* buf, size_t cap)
{
    char base[FS_MAX_PATH];

#ifdef _WIN32
    const char* appdata = getenv("APPDATA");
    if (!appdata || !appdata[0])
        return 0;
    snprintf(base, sizeof base, "%s", appdata);
#elif defined(__APPLE__)
    const char* home = getenv("HOME");
    if (!home || !home[0])
        return 0;
    snprintf(base, sizeof base, "%s/Library/Application Support", home);
#else
    const char* xdg = getenv("XDG_CONFIG_HOME");
    if (xdg && xdg[0])
    {
        snprintf(base, sizeof base, "%s", xdg);
    }
    else
    {
        const char* home = getenv("HOME");
        if (!home || !home[0])
            return 0;
        snprintf(base, sizeof base, "%s/.config", home);
        mkdir(base, 0755);  /* ~/.config might not exist yet on a minimal system */
    }
#endif

#ifdef _WIN32
    snprintf(buf, cap, "%s\\cake_ide", base);
#else
    snprintf(buf, cap, "%s/cake_ide", base);
#endif
    mkdir(buf, 0755);  /* fine if it already exists - return value ignored */
    return 1;
}

static int get_session_file_path(char* buf, size_t cap)
{
    char dir[FS_MAX_PATH];
    if (!get_config_dir(dir, sizeof dir))
        return 0;

#ifdef _WIN32
    snprintf(buf, cap, "%s\\session.json", dir);
#else
    snprintf(buf, cap, "%s/session.json", dir);
#endif
    return 1;
}

/* Playground's own fixed file - unlike every other document window, it
 * always opens this exact path, never anything File > Open picked. Kept
 * next to session.json (same config directory, see get_config_dir) rather
 * than under the project/cwd the IDE happens to be launched from, since
 * Playground is meant as a standing scratch pad independent of whatever
 * project's open at the time. See open_playground() below. */
static int get_playground_file_path(char* buf, size_t cap)
{
    char dir[FS_MAX_PATH];
    if (!get_config_dir(dir, sizeof dir))
        return 0;

#ifdef _WIN32
    snprintf(buf, cap, "%s\\playground.c", dir);
#else
    snprintf(buf, cap, "%s/playground.c", dir);
#endif
    return 1;
}

/* View > "Show Playground" (EVT_WINDOW_PLAYGROUND): opens the scratch editor
 * pinned to playground.c. Playground is now a completely ordinary transient
 * document window (see make_editor_window/open_file_path_into_editor) that
 * just always happens to open the same fixed path - closing it destroys it
 * like any other window, and reopening it (via this handler, or session
 * restore) rereads playground.c fresh from disk rather than restoring any
 * in-memory state, so edits made outside the IDE between opens are picked
 * up. The file is created with a small Hello World the first time the IDE
 * ever runs (or if it's ever deleted); every run after that just reopens
 * whatever it was last saved as. */
static void open_playground(void)
{
    char path[FS_MAX_PATH] = "playground.c";
    get_playground_file_path(path, sizeof path);

    FILE* probe = fopen(path, "rb");
    if (probe)
    {
        fclose(probe);
    }
    else
    {
        static const char playground_default[] =
            "#include <stdio.h>\n"
            "int main(void)\n"
            "{\n"
            "  printf(\"Hello, world!\\n\");\n"
            "  return 0;\n"
            "}\n";
        FILE* pf = fopen(path, "wb");
        if (pf)
        {
            fwrite_text(playground_default, sizeof playground_default - 1, pf, file_uses_crlf(path));
            fclose(pf);
        }
    }

    open_file_path_into_editor(path, "Playground");
}

/* Every place that reopens a document by path alone (session restore, the
 * Back/Forward navigation history) would otherwise label Playground with
 * basename_of's default "playground.c", rather than "Playground" - the
 * title it always gets when opened the normal way, through this file's own
 * open_playground() above. Route the label through here instead, so an
 * incidentally-reopened Playground window still reads the same either way. */
static const char* label_for_path(const char* path)
{
    char playground_path[FS_MAX_PATH];
    if (get_playground_file_path(playground_path, sizeof playground_path) &&
        strcmp(path, playground_path) == 0)
        return "Playground";
    return basename_of(path);
}

/* Persists the session snapshot described above. Silently does nothing if
 * the config directory can't be resolved/created or opened for writing -
 * losing the session is far less disruptive than failing to exit. */
static void save_session(void)
{
    char path[FS_MAX_PATH];
    if (!get_session_file_path(path, sizeof path))
        return;
    struct json_value* _Opt _Owner root = calloc(1, sizeof *root);
    if (!root)
        return;
    root->type = JSON_OBJECT;

    json_set_string(root, "folder_dir", g_folder.dir);

    /* Project > New/Open Project's own ".cakeproj" file, so it reopens
     * automatically on the next launch (see load_session()'s matching key -
     * project_open_file() there does the actual reloading). Written even
     * when empty so closing a project actually clears it back out. */
    json_set_string(root, "project_path", g_project.file_path);

    json_set_number(root, "theme_index", g_envdlg.theme_index);
    json_set_number(root, "font_index", g_envdlg.font_index);
    {
        int fs = ui_env_get_font_size(g_env);
        if (fs > 0)
            json_set_number(root, "font_size", fs);
    }

    /* External Tools, one object per tool - see load_session's matching
     * parse. Written even when empty so removing every tool actually
     * persists. */
    {
        struct json_value* tools = json_set_array(root, "tools");
        for (int i = 0; i < g_tools.count; i++)
        {
            struct json_value* tool = json_add_object(tools);
            json_set_string(tool, "title", g_tools.items[i].title);
            json_set_string(tool, "command", g_tools.items[i].command);
            json_set_string(tool, "args", g_tools.items[i].args);
            json_set_string(tool, "dir", g_tools.items[i].dir);
        }
    }

    nav_pos cur;
    if (nav_capture(&cur))
    {
        struct json_value* current = json_set_object(root, "current");
        json_set_string(current, "file", cur.path);
        json_set_number(current, "cursor", cur.cursor);
        json_set_number(current, "scroll", cur.scroll);
    }

    if (g_active_editor_window)
    {
        ui_node* win = ui_child_at(g_active_editor_window, 0);
        if (win)
        {
            int x, y, w, h;
            ui_get_rect(win, &x, &y, &w, &h);

            struct json_value* main_window = json_set_object(root, "main_window");
            json_set_number(main_window, "x", x);
            json_set_number(main_window, "y", y);
            json_set_number(main_window, "w", w);
            json_set_number(main_window, "h", h);
            json_set_bool(main_window, "maximized", ui_get_maximized(win));
        }
    }

    /* Both panels can be moved between sides at runtime (see g_dockmenu), so
     * the side is saved alongside the size - and the size read back is the
     * extent of whichever axis that side owns, since a left/right dock
     * controls its width and a bottom dock its height. */
    if (g_folder.window)
    {
        ui_node* win = ui_child_at(g_folder.window, 0);
        if (win)
        {
            int w, h;
            ui_dock_side side = ui_get_dock(win);
            ui_get_rect(win, NULL, NULL, &w, &h);

            struct json_value* dock = json_set_object(root, "folder_dock");
            json_set_number(dock, "side", (int)side);
            json_set_number(dock, "size", side == UI_DOCK_BOTTOM ? h : w);
        }
    }
    if (g_output_window)
    {
        ui_node* win = ui_child_at(g_output_window, 0);
        if (win)
        {
            int w, h;
            ui_dock_side side = ui_get_dock(win);
            ui_get_rect(win, NULL, NULL, &w, &h);

            struct json_value* dock = json_set_object(root, "output_dock");
            json_set_number(dock, "side", (int)side);
            json_set_number(dock, "size", side == UI_DOCK_BOTTOM ? h : w);
        }
    }

    json_write_file(path, root);
    json_delete(root);
}

/* Public shutdown hook (see ide_ui.h) - just save_session() under a name a
 * platform backend can call directly, since save_session() itself is
 * static to this file. Safe any time: every global it touches is either a
 * plain buffer (always valid) or a ui_node* that defaults to NULL until
 * app_init() sets it, and save_session() already checks each one before
 * dereferencing it. */
void app_shutdown(void)
{
    save_session();
}

/* However load_session() finds there's no session to restore (no config
 * directory to even look in, or a config directory but no session.json in
 * it yet) - the one-time "you've never run this before" case, as opposed
 * to every other frame this same run where there's simply nothing new to
 * report. A plain OK box, same shape as the "File not found" one above. */
static void show_first_run_welcome(void)
{
    ui_msgbox_button ok = { "   OK   ", 0 };
    ui_message_box(g_screen, "Welcome", "Thanks for trying cake.", &ok, 1);
}

/* Restores the session snapshot saved by save_session() - called once from
 * app_init(), after every window it might touch (Folder/Output panels, the
 * Compiler Options dialog's own fields) already exists, but before
 * app_init's fallback "demo/test file" window is created (its caller skips
 * that window whenever this successfully reopens a real one instead).
 * Returns 1 if it reopened a current-file document, 0 otherwise (no session
 * file yet, or it had no current_file, or that file's since vanished) - the
 * missing/unreadable/partial cases are all fine, whatever can't be restored
 * is just left at its normal startup default. */
static int load_session(void)
{
    /* Already parsed: the early theme peek (session_peek_int) read the file
     * during app_init and this is that same tree. A corrupt session restores
     * nothing rather than half a layout - everything below then just stays
     * at its startup default. */
    struct json_value* root = session_json();
    if (!root)
    {
        /* Only a genuine first run gets the welcome - an unreadable file is
         * a failure, not an introduction. */
        if (!g_session_file_found)
            show_first_run_welcome();
        return 0;
    }

    char current_file[1024] = "";
    char project_path[1024] = "";
    int have_cursor = 0, have_scroll = 0, cursor = 0, scroll = 0;
    int have_main_rect = 0, main_x = 0, main_y = 0, main_w = 0, main_h = 0, main_maximized = 0;
    int have_folder_w = 0, folder_w = 0;
    int have_output_h = 0, output_h = 0;
    /* The side each panel was last docked to (see g_dockmenu) - defaults to
       where it is built, so a session file written before the sides could be
       moved restores exactly as it used to. */
    ui_dock_side folder_side = UI_DOCK_LEFT;
    ui_dock_side output_side = UI_DOCK_BOTTOM;

    g_envdlg.theme_index = project_json_get_int(root, "theme_index", g_envdlg.theme_index);
    g_envdlg.font_index = project_json_get_int(root, "font_index", g_envdlg.font_index);
    {
        int font_size = project_json_get_int(root, "font_size", 0);
        if (font_size > 0)
            ui_env_set_font_size(g_env, font_size);
    }

    project_json_get_string(root, "folder_dir", g_folder.dir, sizeof g_folder.dir);
    project_json_get_string(root, "project_path", project_path, sizeof project_path);

    /* External Tools - the array is capped at EXT_TOOL_MAX rather than
     * trusted, since this is a file on disk. */
    {
        const struct json_value* tools = json_find_member(root, "tools");
        if (tools && tools->type == JSON_ARRAY)
        {
            g_tools.count = 0;
            for (const struct json_value* tool = tools->first_child;
                 tool != NULL && g_tools.count < EXT_TOOL_MAX;
                 tool = tool->next)
            {
                if (tool->type != JSON_OBJECT)
                    continue;

                ext_tool* t = &g_tools.items[g_tools.count++];
                project_json_get_string(tool, "title", t->title, sizeof t->title);
                project_json_get_string(tool, "command", t->command, sizeof t->command);
                project_json_get_string(tool, "args", t->args, sizeof t->args);
                project_json_get_string(tool, "dir", t->dir, sizeof t->dir);
            }
        }
    }

    {
        const struct json_value* current = json_find_member(root, "current");
        if (current && current->type == JSON_OBJECT)
        {
            project_json_get_string(current, "file", current_file, sizeof current_file);
            if (json_find_member(current, "cursor"))
                { cursor = project_json_get_int(current, "cursor", 0); have_cursor = 1; }
            if (json_find_member(current, "scroll"))
                { scroll = project_json_get_int(current, "scroll", 0); have_scroll = 1; }
        }
    }

    {
        const struct json_value* main_window = json_find_member(root, "main_window");
        if (main_window && main_window->type == JSON_OBJECT)
        {
            main_x = project_json_get_int(main_window, "x", 0);
            main_y = project_json_get_int(main_window, "y", 0);
            main_w = project_json_get_int(main_window, "w", 0);
            main_h = project_json_get_int(main_window, "h", 0);
            main_maximized = project_json_get_bool(main_window, "maximized", 0);
            have_main_rect = 1;
        }
    }

    {
        /* "size" has to actually be there before the panel is resized - a
         * hand-edited file missing it would otherwise dock the panel at a
         * width of zero, hiding it with no way back short of deleting the
         * session file. */
        const struct json_value* dock = json_find_member(root, "folder_dock");
        if (dock && dock->type == JSON_OBJECT && json_find_member(dock, "size"))
        {
            folder_side = (ui_dock_side)project_json_get_int(dock, "side", (int)folder_side);
            folder_w = project_json_get_int(dock, "size", 0);
            have_folder_w = 1;
        }

        dock = json_find_member(root, "output_dock");
        if (dock && dock->type == JSON_OBJECT && json_find_member(dock, "size"))
        {
            output_side = (ui_dock_side)project_json_get_int(dock, "side", (int)output_side);
            output_h = project_json_get_int(dock, "size", 0);
            have_output_h = 1;
        }
    }

    /* Everything above is copied out into globals or locals, so the document
     * itself is done with - nothing reads it after startup. */
    session_json_close();

    /* Folder panel: re-point it at the saved directory - folder_window_
     * refresh()'s populate_listbox_from_dir silently no-ops on a bad path,
     * same as browsing there manually would, so a stale/deleted directory
     * just leaves the panel showing nothing rather than crashing. */
    if (g_folder.dir[0])
        folder_window_refresh();

    if (have_folder_w && g_folder.window)
    {
        ui_node* win = ui_child_at(g_folder.window, 0);
        if (win)
            ui_set_dock(win, folder_side, folder_w);
    }
    if (have_output_h && g_output_window)
    {
        ui_node* win = ui_child_at(g_output_window, 0);
        if (win)
            ui_set_dock(win, output_side, output_h);
    }

    /* Project > New/Open Project: reopen whatever was open last session -
     * project_open_file() already no-ops if the file's since been moved or
     * deleted, same "just leave it at the normal startup default" tolerance
     * as the Folder panel's own restore just above. */
    if (project_path[0])
        project_open_file(project_path);

    if (!current_file[0])
        return 0;

    open_file_path_into_editor(current_file, label_for_path(current_file));
    ui_node* win = find_open_window(current_file);
    if (!win)
        return 0;  /* reported "File not found" already, nothing more to do */

    ui_node* ed = editor_in_window(win);
    if (ed && have_cursor)
    {
        ui_editor_set_selection(ed, cursor, cursor);
        if (have_scroll)
            ui_editor_set_scroll(ed, scroll);
    }

    if (have_main_rect)
    {
        ui_node* real_win = ui_child_at(win, 0);
        if (real_win)
        {
            ui_window_set_rect(real_win, main_x, main_y, main_w, main_h);
            if (main_maximized)
                ui_window_maximize(g_screen, real_win);
        }
    }

    return 1;
}

void app_init(ui_env* env)
{
    g_env = env;
    const ui_theme* theme = ui_get_theme();
    g_screen = ui_screen_create();
    ui_node* root = ui_screen_root(g_screen);
    g_root = root;
    build_screen(root);


    ui_screen_set_desktop(g_screen, theme->desktop_bg);

    /* Adopt the saved theme here, before any window is built - see
     * session_peek_int()'s own comment for why this cannot wait for
     * load_session() at the end of app_init. Defaults to Dark (row 1) on a
     * first run or an unreadable session. */
    g_envdlg.theme_index = session_peek_int("theme_index", 1);
    theme = theme_by_index(g_envdlg.theme_index);
    apply_theme(theme);  /* every ui_set_color(..., theme->...) below must use
                          * the theme actually applied here - windows built
                          * during init (About, Find/Replace, Output, Folder)
                          * bake their colors in permanently */

    /* --- About modal --- */
    ui_node* modal = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, modal);
    ui_node* about_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(about_window, 15, 5, 50, 14);
    ui_set_label(about_window, " About ");
    ui_set_color(about_window, theme->modal_fg, theme->modal_bg);
    ui_append_child(modal, about_window);
    add_text(about_window, 36, 8, "Cake IDE", theme->modal_fg, theme->modal_bg);
    add_text(about_window, 32, 9, "Version " CAKE_VERSION, theme->modal_fg, theme->modal_bg);
    add_text(about_window, 31, 11, "https://cakecc.org", theme->modal_fg, theme->modal_bg);
    ui_node* close_button = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(close_button, EVT_ABOUT_CLOSE);
    ui_set_rect(close_button, 34, 15, 12, 1);
    ui_set_label(close_button, "  OK  ");
    ui_append_child(about_window, close_button);
    g_about_modal = modal;

    /* --- Directories modal --- */
    ui_node* dirs_modal = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, dirs_modal);
    ui_node* dirs_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(dirs_window, 10, 2, 60, 18);
    ui_set_label(dirs_window, " Directories ");
    ui_set_color(dirs_window, theme->modal_fg, theme->modal_bg);
    ui_append_child(dirs_modal, dirs_window);
    add_text(dirs_window, 13, 4, "Include Directories", theme->label_fg, theme->modal_bg);
    add_input(dirs_window, 13, 5, 44, "C:\\CAKEIDE\\INCLUDE");
    add_text(dirs_window, 13, 7, "Library Directories", theme->label_fg, theme->modal_bg);
    add_input(dirs_window, 13, 8, 44, "C:\\CAKEIDE\\LIB");
    add_text(dirs_window, 13, 10, "Output Directory", theme->label_fg, theme->modal_bg);
    add_input(dirs_window, 13, 11, 44, "C:\\CAKEIDE\\SOURCE");
    add_text(dirs_window, 13, 13, "Source Directories", theme->label_fg, theme->modal_bg);
    add_input(dirs_window, 13, 14, 44, "C:\\CAKEIDE\\SOURCE");
    ui_node* dirs_ok = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(dirs_ok, EVT_DIRS_OK);
    ui_set_rect(dirs_ok, 20, 17, 10, 1);
    ui_set_label(dirs_ok, "  OK  ");
    ui_append_child(dirs_window, dirs_ok);
    ui_node* dirs_cancel = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(dirs_cancel, EVT_DIRS_CANCEL);
    ui_set_rect(dirs_cancel, 34, 17, 10, 1);
    ui_set_label(dirs_cancel, "Cancel");
    ui_append_child(dirs_window, dirs_cancel);
    ui_node* dirs_help = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(dirs_help, EVT_DIRS_HELP);
    ui_set_rect(dirs_help, 48, 17, 10, 1);
    ui_set_label(dirs_help, " Help ");
    ui_append_child(dirs_window, dirs_help);
    g_dirs_modal = dirs_modal;

    /* --- Go to Line modal --- */
    ui_node* goto_modal = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, goto_modal);
    ui_node* goto_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(goto_window, 20, 7, 40, 8);
    ui_set_label(goto_window, " Go to Line Number ");
    ui_set_color(goto_window, theme->modal_fg, theme->modal_bg);
    ui_append_child(goto_modal, goto_window);
    add_text(goto_window, 23, 9, "Enter New Line Number", theme->label_fg, theme->modal_bg);
    g_goto_input = add_input(goto_window, 46, 9, 11, "");
    ui_set_id(g_goto_input, EVT_GOTO_INPUT);
    ui_set_numeric(g_goto_input, 1);
    ui_node* goto_ok = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(goto_ok, EVT_GOTO_OK);
    ui_set_rect(goto_ok, 29, 12, 10, 1);
    ui_set_label(goto_ok, "  OK  ");
    ui_append_child(goto_window, goto_ok);
    ui_node* goto_cancel = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(goto_cancel, EVT_GOTO_CANCEL);
    ui_set_rect(goto_cancel, 41, 12, 10, 1);
    ui_set_label(goto_cancel, "Cancel");
    ui_append_child(goto_window, goto_cancel);
    g_goto_modal = goto_modal;

    /* --- Word Wrap modal --- */
    ui_node* wordwrap_modal = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, wordwrap_modal);
    ui_node* wordwrap_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(wordwrap_window, 20, 7, 40, 9);
    ui_set_label(wordwrap_window, " Word Wrap ");
    ui_set_color(wordwrap_window, theme->modal_fg, theme->modal_bg);
    ui_append_child(wordwrap_modal, wordwrap_window);
    add_text(wordwrap_window, 23, 9, "Columns", theme->label_fg, theme->modal_bg);
    g_wordwrap_input = add_input(wordwrap_window, 31, 9, 11, "80");
    ui_set_id(g_wordwrap_input, EVT_WORDWRAP_INPUT);
    ui_set_numeric(g_wordwrap_input, 1);
    g_wordwrap_justify = add_group(wordwrap_window, 23, 11, 20, 1, 1);
    add_group_item(g_wordwrap_justify, "Justify");
    ui_node* wordwrap_ok = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(wordwrap_ok, EVT_WORDWRAP_OK);
    ui_set_rect(wordwrap_ok, 29, 13, 10, 1);
    ui_set_label(wordwrap_ok, "  OK  ");
    ui_append_child(wordwrap_window, wordwrap_ok);
    ui_node* wordwrap_cancel = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(wordwrap_cancel, EVT_WORDWRAP_CANCEL);
    ui_set_rect(wordwrap_cancel, 41, 13, 10, 1);
    ui_set_label(wordwrap_cancel, "Cancel");
    ui_append_child(wordwrap_window, wordwrap_cancel);
    g_wordwrap_modal = wordwrap_modal;

    /* --- Replace modal --- */
    ui_node* rep_modal = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, rep_modal);
    int rx = 9, ry = 3, rw = 60, rh = 19;
    ui_node* rep_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(rep_window, rx, ry, rw, rh);
    ui_set_label(rep_window, " Replace Text ");
    ui_set_color(rep_window, theme->modal_fg, theme->modal_bg);
    ui_append_child(rep_modal, rep_window);
    add_text(rep_window, rx + 2, ry + 2, "Text to Find", theme->label_fg, theme->modal_bg);
    g_replace.find = add_input(rep_window, rx + 16, ry + 2, 40, "");
    ui_set_id(g_replace.find, EVT_REPLACE_OK);
    add_text(rep_window, rx + 4, ry + 4, "New Text", theme->label_fg, theme->modal_bg);
    g_replace.new_ = add_input(rep_window, rx + 16, ry + 4, 40, "");
    ui_set_id(g_replace.new_, EVT_REPLACE_OK);
    add_text(rep_window, rx + 2, ry + 6, "Options", theme->label_fg, theme->modal_bg);
    ui_node* opts = add_group(rep_window, rx + 2, ry + 7, 26, 3, 1);
    g_replace.opts = opts;
    add_group_item(opts, "Case sensitive");
    add_group_item(opts, "Whole words only");
    add_group_item(opts, "Prompt on replace");
    ui_group_set_checked(opts, 0, 1);
    ui_group_set_checked(opts, 2, 1);
    add_text(rep_window, rx + 32, ry + 6, "Direction", theme->label_fg, theme->modal_bg);
    ui_node* dir = add_group(rep_window, rx + 32, ry + 7, 22, 2, 0);
    g_replace.dir = dir;
    add_group_item(dir, "Forward");
    add_group_item(dir, "Backward");
    ui_select_set_selected(dir, 0);
    add_text(rep_window, rx + 2, ry + 12, "Scope", theme->label_fg, theme->modal_bg);
    ui_node* scope = add_group(rep_window, rx + 2, ry + 13, 26, 2, 0);
    g_replace.scope = scope;
    add_group_item(scope, "Global");
    add_group_item(scope, "Selected text");
    ui_select_set_selected(scope, 0);
    add_text(rep_window, rx + 32, ry + 12, "Origin", theme->label_fg, theme->modal_bg);
    ui_node* origin = add_group(rep_window, rx + 32, ry + 13, 22, 2, 0);
    g_replace.origin = origin;
    add_group_item(origin, "From cursor");
    add_group_item(origin, "Entire scope");
    ui_select_set_selected(origin, 0);
    struct { int id; int x; int w; const char* label; } rep_btns[] = {
        { EVT_REPLACE_OK,       rx + 11, 10, "   OK   " },
        { EVT_REPLACE_CHANGEALL, rx + 23, 14, "Change All" },
        { EVT_REPLACE_CANCEL,   rx + 39, 10, " Cancel " },
    };
    for (int i = 0; i < 3; i++)
    {
        ui_node* b = ui_create_element(UI_TAG_BUTTON);
        ui_set_id(b, rep_btns[i].id);
        ui_set_rect(b, rep_btns[i].x, ry + 16, rep_btns[i].w, 1);
        ui_set_label(b, rep_btns[i].label);
        ui_append_child(rep_window, b);
    }
    g_replace.modal = rep_modal;

    /* --- Find modal --- */
    ui_node* fnd_modal = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, fnd_modal);
    int fx = 12, fy = 4, fw = 56, fh = 16;
    ui_node* fnd_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(fnd_window, fx, fy, fw, fh);
    ui_set_label(fnd_window, " Find Text ");
    ui_set_color(fnd_window, theme->modal_fg, theme->modal_bg);
    ui_append_child(fnd_modal, fnd_window);
    add_text(fnd_window, fx + 2, fy + 2, "Text to Find", theme->label_fg, theme->modal_bg);
    g_find.input = add_input(fnd_window, fx + 16, fy + 2, 36, "");
    ui_set_id(g_find.input, EVT_FIND_OK);
    add_text(fnd_window, fx + 2, fy + 4, "Options", theme->label_fg, theme->modal_bg);
    g_find.opts = add_group(fnd_window, fx + 2, fy + 5, 26, 2, 1);
    add_group_item(g_find.opts, "Case sensitive");
    add_group_item(g_find.opts, "Whole words only");
    ui_group_set_checked(g_find.opts, 0, 1);
    add_text(fnd_window, fx + 30, fy + 4, "Direction", theme->label_fg, theme->modal_bg);
    g_find.dir = add_group(fnd_window, fx + 30, fy + 5, 22, 2, 0);
    add_group_item(g_find.dir, "Forward");
    add_group_item(g_find.dir, "Backward");
    ui_select_set_selected(g_find.dir, 0);
    add_text(fnd_window, fx + 2, fy + 9, "Scope", theme->label_fg, theme->modal_bg);
    g_find.scope = add_group(fnd_window, fx + 2, fy + 10, 26, 2, 0);
    add_group_item(g_find.scope, "Global");
    add_group_item(g_find.scope, "Selected text");
    ui_select_set_selected(g_find.scope, 0);
    add_text(fnd_window, fx + 30, fy + 9, "Origin", theme->label_fg, theme->modal_bg);
    g_find.origin = add_group(fnd_window, fx + 30, fy + 10, 22, 2, 0);
    add_group_item(g_find.origin, "From cursor");
    add_group_item(g_find.origin, "Entire scope");
    ui_select_set_selected(g_find.origin, 0);
    struct { int id; int x; int w; const char* label; } fnd_btns[] = {
        { EVT_FIND_OK,     fx + 16, 10, "   OK   " },
        { EVT_FIND_CANCEL, fx + 30, 10, " Cancel " },
    };
    for (int i = 0; i < 2; i++)
    {
        ui_node* b = ui_create_element(UI_TAG_BUTTON);
        ui_set_id(b, fnd_btns[i].id);
        ui_set_rect(b, fnd_btns[i].x, fy + 13, fnd_btns[i].w, 1);
        ui_set_label(b, fnd_btns[i].label);
        ui_append_child(fnd_window, b);
    }
    g_find.modal = fnd_modal;

    /* --- Editor context menu popup --- */
    ui_node* popup = ui_create_element(UI_TAG_MENU);
    ui_append_child(root, popup);

    /* First item in the popup, ahead of everything else below. This is
     * "Compile" in the narrow sense - always just the file under the cursor
     * (EVT_COMPILE_FILE -> do_compile()), never the project. Build > "Build"
     * / F7 is the other one (EVT_COMPILE -> do_build()), which builds the
     * whole project when the active file is a member of it. Enabled only for
     * .c files, refreshed each frame next to the menu's own copy (see
     * g_editor_popup_compile). */
    ui_node* popup_compile = ui_create_element(UI_TAG_ITEM);
    ui_set_id(popup_compile, EVT_COMPILE_FILE);
    ui_set_label(popup_compile, "Compile");
    ui_append_child(popup, popup_compile);
    g_editor_popup_compile = popup_compile;
    ui_node* popup_sep0 = ui_create_element(UI_TAG_ITEM);
    ui_set_separator(popup_sep0, 1);
    ui_append_child(popup, popup_sep0);

    struct { int id; const char* label; const char* shortcut; } popup_items[] = {
        { EVT_SEARCH_FIND,       "Find...",          NULL },
        { EVT_SEARCH_REPLACE,    "Replace...",       "Ctrl+R" },
        { EVT_SEARCH_NEXT,       "Search Next",      "F3" },
        { EVT_SEARCH_GOTO,       "Go to line...",    "Ctrl+G" },
        { EVT_SEARCH_GOTO_DEFINITION, "Go to Definition", "F12" },
        { EVT_TOOLS_FINDREPLACE, "Find in Files...", "Ctrl+F" },
    };
    for (int i = 0; i < 6; i++)
    {
        ui_node* it = ui_create_element(UI_TAG_ITEM);
        ui_set_id(it, popup_items[i].id);
        ui_set_label(it, popup_items[i].label);
        ui_set_shortcut(it, popup_items[i].shortcut);
        ui_append_child(popup, it);
    }
    ui_node* popup_sep = ui_create_element(UI_TAG_ITEM);
    ui_set_separator(popup_sep, 1);
    ui_append_child(popup, popup_sep);
    ui_node* popup_readonly = ui_create_element(UI_TAG_ITEM);
    ui_set_id(popup_readonly, EVT_EDITOR_TOGGLE_READONLY);
    ui_append_child(popup, popup_readonly);
    g_editor_popup_readonly = popup_readonly;
    ui_node* popup_linenumbers = ui_create_element(UI_TAG_ITEM);
    ui_set_id(popup_linenumbers, EVT_VIEW_LINENUMBERS);
    ui_append_child(popup, popup_linenumbers);
    g_view_linenumbers_item = popup_linenumbers;

    ui_node* popup_sep2 = ui_create_element(UI_TAG_ITEM);
    ui_set_separator(popup_sep2, 1);
    ui_append_child(popup, popup_sep2);
    ui_node* popup_hdrsrc = ui_create_element(UI_TAG_ITEM);
    ui_set_id(popup_hdrsrc, EVT_EDITOR_TOGGLE_HDRSRC);
    ui_set_label(popup_hdrsrc, "Toggle Header/Source");
    ui_append_child(popup, popup_hdrsrc);
    g_editor_popup_hdrsrc = popup_hdrsrc;
    ui_node* popup_show_output = ui_create_element(UI_TAG_ITEM);
    ui_set_id(popup_show_output, EVT_EDITOR_SHOW_OUTPUT);
    ui_set_label(popup_show_output, "Show Generated Code");
    ui_append_child(popup, popup_show_output);
    g_editor_popup_show_output = popup_show_output;
    ui_node* popup_copy_path = ui_create_element(UI_TAG_ITEM);
    ui_set_id(popup_copy_path, EVT_EDITOR_COPY_PATH);
    ui_set_label(popup_copy_path, "Copy Full Path");
    ui_append_child(popup, popup_copy_path);
    ui_node* popup_show_folder = ui_create_element(UI_TAG_ITEM);
    ui_set_id(popup_show_folder, EVT_EDITOR_SHOW_FOLDER);
    ui_set_label(popup_show_folder, "Show My Folder");
    ui_append_child(popup, popup_show_folder);
    ui_node* popup_sep3 = ui_create_element(UI_TAG_ITEM);
    ui_set_separator(popup_sep3, 1);
    ui_append_child(popup, popup_sep3);
    ui_node* popup_codeblock_copy = ui_create_element(UI_TAG_ITEM);
    ui_set_id(popup_codeblock_copy, EVT_EDITOR_CODEBLOCK_COPY);
    ui_set_label(popup_codeblock_copy, "Copy Code Block");
    ui_append_child(popup, popup_codeblock_copy);
    g_editor_popup_codeblock_copy = popup_codeblock_copy;
    ui_node* popup_codeblock_playground = ui_create_element(UI_TAG_ITEM);
    ui_set_id(popup_codeblock_playground, EVT_EDITOR_CODEBLOCK_PLAYGROUND);
    ui_set_label(popup_codeblock_playground, "Copy to Playground");
    ui_append_child(popup, popup_codeblock_playground);
    ui_node* popup_format = ui_create_element(UI_TAG_ITEM);
    ui_set_id(popup_format, EVT_EDIT_FORMAT);
    ui_set_label(popup_format, "Format");
    ui_set_shortcut(popup_format, "Ctrl+Shift+F");
    ui_append_child(popup, popup_format);
    g_editor_popup_format = popup_format;

    g_editor_popup_codeblock_playground = popup_codeblock_playground;
    g_editor_popup = popup;

    /* --- Folder panel context menu popup --- */
    ui_node* folder_popup = ui_create_element(UI_TAG_MENU);
    ui_append_child(root, folder_popup);
    ui_node* folder_popup_copy_path = ui_create_element(UI_TAG_ITEM);
    ui_set_id(folder_popup_copy_path, EVT_FOLDER_COPY_PATH);
    ui_set_label(folder_popup_copy_path, "Copy Full Path");
    ui_append_child(folder_popup, folder_popup_copy_path);
    ui_node* folder_popup_sep3 = ui_create_element(UI_TAG_ITEM);
    ui_set_separator(folder_popup_sep3, 1);
    ui_append_child(folder_popup, folder_popup_sep3);
    ui_node* folder_popup_newfile = ui_create_element(UI_TAG_ITEM);
    ui_set_id(folder_popup_newfile, EVT_FOLDER_NEWFILE);
    ui_set_label(folder_popup_newfile, "New File...");
    ui_append_child(folder_popup, folder_popup_newfile);
    ui_node* folder_popup_newfolder = ui_create_element(UI_TAG_ITEM);
    ui_set_id(folder_popup_newfolder, EVT_FOLDER_NEWFOLDER);
    ui_set_label(folder_popup_newfolder, "New Folder...");
    ui_append_child(folder_popup, folder_popup_newfolder);
    ui_node* folder_popup_sep5 = ui_create_element(UI_TAG_ITEM);
    ui_set_separator(folder_popup_sep5, 1);
    ui_append_child(folder_popup, folder_popup_sep5);
    ui_node* folder_popup_add_to_project = ui_create_element(UI_TAG_ITEM);
    ui_set_id(folder_popup_add_to_project, EVT_FOLDER_ADD_TO_PROJECT);
    ui_set_label(folder_popup_add_to_project, "Add to Project");
    ui_append_child(folder_popup, folder_popup_add_to_project);
    g_folder.popup_add_to_project = folder_popup_add_to_project;
    ui_node* folder_popup_sep4 = ui_create_element(UI_TAG_ITEM);
    ui_set_separator(folder_popup_sep4, 1);
    ui_append_child(folder_popup, folder_popup_sep4);
    ui_node* folder_popup_delete = ui_create_element(UI_TAG_ITEM);
    ui_set_id(folder_popup_delete, EVT_FOLDER_DELETE);
    ui_set_label(folder_popup_delete, "Delete");
    ui_append_child(folder_popup, folder_popup_delete);
    g_folder.popup = folder_popup;

    /* --- Dock popup --- one menu for every dockable panel, see g_dockmenu. */
    ui_node* dock_popup = ui_create_element(UI_TAG_MENU);
    ui_append_child(root, dock_popup);
    struct { int id; const char* label; } dock_items[] = {
        { EVT_DOCK_LEFT,   "Dock Left" },
        { EVT_DOCK_RIGHT,  "Dock Right" },
        { EVT_DOCK_BOTTOM, "Dock Bottom" },
    };
    for (int i = 0; i < 3; i++)
    {
        ui_node* it = ui_create_element(UI_TAG_ITEM);
        ui_set_id(it, dock_items[i].id);
        ui_set_label(it, dock_items[i].label);
        ui_append_child(dock_popup, it);
        g_dockmenu.items[i] = it;
    }
    g_dockmenu.popup = dock_popup;

    /* --- New File/Folder modal --- shared by EVT_FOLDER_NEWFILE and
     * EVT_FOLDER_NEWFOLDER, which retitle it (g_foldernew.window) and set
     * g_foldernew.is_folder before showing it - see EVT_FOLDERNEW_OK. */
    ui_node* foldernew_modal = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, foldernew_modal);
    ui_node* foldernew_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(foldernew_window, 20, 7, 44, 8);
    ui_set_label(foldernew_window, " New File ");
    ui_set_color(foldernew_window, theme->modal_fg, theme->modal_bg);
    ui_append_child(foldernew_modal, foldernew_window);
    add_text(foldernew_window, 23, 9, "Name", theme->label_fg, theme->modal_bg);
    g_foldernew.input = add_input(foldernew_window, 34, 9, 26, "");
    ui_set_id(g_foldernew.input, EVT_FOLDERNEW_OK);
    ui_node* foldernew_ok = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(foldernew_ok, EVT_FOLDERNEW_OK);
    ui_set_rect(foldernew_ok, 30, 12, 10, 1);
    ui_set_label(foldernew_ok, "  OK  ");
    ui_append_child(foldernew_window, foldernew_ok);
    ui_node* foldernew_cancel = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(foldernew_cancel, EVT_FOLDERNEW_CANCEL);
    ui_set_rect(foldernew_cancel, 44, 12, 10, 1);
    ui_set_label(foldernew_cancel, "Cancel");
    ui_append_child(foldernew_window, foldernew_cancel);
    g_foldernew.window = foldernew_window;
    g_foldernew.modal = foldernew_modal;

    /* --- Git commit message modal --- opened by the Git Changes popup's
     * "Commit" item (EVT_GIT_COMMIT_BTN) - see git_commit_start()/
     * EVT_GITCOMMIT_OK. Same shape as the New File/Folder modal just above. */
    ui_node* gitcommit_modal = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, gitcommit_modal);
    ui_node* gitcommit_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(gitcommit_window, 18, 7, 48, 8);
    ui_set_label(gitcommit_window, " Commit All ");
    ui_set_color(gitcommit_window, theme->modal_fg, theme->modal_bg);
    ui_append_child(gitcommit_modal, gitcommit_window);
    add_text(gitcommit_window, 21, 9, "Message", theme->label_fg, theme->modal_bg);
    g_gitcommit.input = add_input(gitcommit_window, 21, 10, 42, "");
    ui_set_id(g_gitcommit.input, EVT_GITCOMMIT_OK);
    ui_node* gitcommit_ok = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(gitcommit_ok, EVT_GITCOMMIT_OK);
    ui_set_rect(gitcommit_ok, 34, 12, 10, 1);
    ui_set_label(gitcommit_ok, "  OK  ");
    ui_append_child(gitcommit_window, gitcommit_ok);
    ui_node* gitcommit_cancel = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(gitcommit_cancel, EVT_GITCOMMIT_CANCEL);
    ui_set_rect(gitcommit_cancel, 48, 12, 10, 1);
    ui_set_label(gitcommit_cancel, "Cancel");
    ui_append_child(gitcommit_window, gitcommit_cancel);
    g_gitcommit.window = gitcommit_window;
    g_gitcommit.modal = gitcommit_modal;

    /* --- Git branch modal --- opened by the Git Changes popup's "Branch..." item, see git_branch_start(). */
    ui_node* gitbranch_modal = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, gitbranch_modal);
    ui_node* gitbranch_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(gitbranch_window, 18, 4, 48, 17);
    ui_set_label(gitbranch_window, " Branch ");
    ui_set_color(gitbranch_window, theme->modal_fg, theme->modal_bg);
    ui_append_child(gitbranch_modal, gitbranch_window);
    add_text(gitbranch_window, 21, 6, "Local branches", theme->label_fg, theme->modal_bg);
    g_gitbranch.listbox = ui_create_element(UI_TAG_LISTBOX);
    ui_set_rect(g_gitbranch.listbox, 21, 7, 42, 6);
    ui_set_id(g_gitbranch.listbox, EVT_GITBRANCH_CHECKOUT);
    ui_append_child(gitbranch_window, g_gitbranch.listbox);
    add_text(gitbranch_window, 21, 14, "New branch", theme->label_fg, theme->modal_bg);
    g_gitbranch.input = add_input(gitbranch_window, 21, 15, 42, "");
    ui_set_id(g_gitbranch.input, EVT_GITBRANCH_NEW);
    ui_node* gitbranch_checkout = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(gitbranch_checkout, EVT_GITBRANCH_CHECKOUT);
    ui_set_rect(gitbranch_checkout, 21, 18, 12, 1);
    ui_set_label(gitbranch_checkout, "Checkout");
    ui_append_child(gitbranch_window, gitbranch_checkout);
    ui_node* gitbranch_new = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(gitbranch_new, EVT_GITBRANCH_NEW);
    ui_set_rect(gitbranch_new, 36, 18, 12, 1);
    ui_set_label(gitbranch_new, "  New   ");
    ui_append_child(gitbranch_window, gitbranch_new);
    ui_node* gitbranch_cancel = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(gitbranch_cancel, EVT_GITBRANCH_CANCEL);
    ui_set_rect(gitbranch_cancel, 51, 18, 10, 1);
    ui_set_label(gitbranch_cancel, "Cancel");
    ui_append_child(gitbranch_window, gitbranch_cancel);
    g_gitbranch.window = gitbranch_window;
    g_gitbranch.modal = gitbranch_modal;

    /* --- Git clone URL+Folder modal --- opened by the Git Changes popup's
     * "Clone..." item (EVT_GIT_CLONE_BTN) - see git_clone_start()/
     * EVT_GITCLONE_OK. Same "Folder field + '...' Browse button" shape as
     * the New Project modal below (g_newproject.folder_input/
     * EVT_PROJECT_NEW_BROWSE), retargeted via OPEN_DLG_GITCLONE_FOLDER. */
    ui_node* gitclone_modal = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, gitclone_modal);
    ui_node* gitclone_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(gitclone_window, 15, 6, 58, 13);
    ui_set_label(gitclone_window, " Clone Repository ");
    ui_set_help(gitclone_window,
                "Copy a remote Git repository to a local folder", "# Copy a remote Git repository to a local folder\n"
                "\n"
                "Runs `git clone <Repository location> <Path>` - Git must be installed "
                "and on the PATH.");
    ui_set_color(gitclone_window, theme->modal_fg, theme->modal_bg);
    ui_append_child(gitclone_modal, gitclone_window);

    /* Label-above-input, both fields flush left at x=18, right edge held 1
     * unit clear of the window's own border (15 + 56 - 1 == 70) - the Path
     * field leaves room for the "..." Browse button at its own right edge,
     * itself flush against that same 1-unit margin. */
    add_text(gitclone_window, 18, 8, "Repository location", theme->label_fg, theme->modal_bg);
    g_gitclone.input = add_input(gitclone_window, 18, 9, 52, "");
    ui_set_id(g_gitclone.input, EVT_GITCLONE_OK);
    ui_set_help(g_gitclone.input,
                "The repository's URL, e.g. `https://github.com/user/repo.git`", "# The repository's URL, e.g. `https://github.com/user/repo.git`\n"
                "\n"
                "Anything `git clone` accepts: an HTTPS or SSH URL "
                "(`git@github.com:user/repo.git`) or a local path. **Path** follows it "
                "as you type, ending in the repository's name.");

    add_text(gitclone_window, 18, 11, "Path", theme->label_fg, theme->modal_bg);
    g_gitclone.folder_input = add_input(gitclone_window, 18, 12, 46, "");
    ui_set_id(g_gitclone.folder_input, EVT_GITCLONE_OK);
    ui_set_help(g_gitclone.folder_input,
                "The new folder the repository is cloned into", "# The new folder the repository is cloned into\n"
                "\n"
                "Filled in as parent folder + the repository's name - the same name "
                "plain `git clone` would pick. Change the parent with **...** or by hand; "
                "the name keeps following the URL. Git creates the folder and any "
                "missing parents; a folder that already exists is refused.");
    ui_node* gitclone_browse = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(gitclone_browse, EVT_GITCLONE_BROWSE);
    ui_set_rect(gitclone_browse, 65, 12, 5, 1);
    ui_set_label(gitclone_browse, "...");
    ui_append_child(gitclone_window, gitclone_browse);

    /* Checked by default - unchecking it skips the folder_reveal_directory()
     * call in git_clone_confirm(), leaving whatever folder is already open
     * (if any) alone. */
    g_gitclone.open_folder_check = add_group(gitclone_window, 18, 14, 20, 1, 1);
    ui_set_help(add_group_item(g_gitclone.open_folder_check, "Open Folder"),
                "Open Folder: show the cloned folder in the Folder panel when done", "## Open Folder\n\nshow the cloned folder in the Folder panel when done\n"
                "\n"
                "Unchecked, the Folder panel keeps showing whatever it shows now.");
    ui_group_set_checked(g_gitclone.open_folder_check, 0, 1);

    /* 10-wide OK/Cancel pair, centered same as the New Project modal's own
     * (15 + (56 - (10+2+10)) / 2 == 32). */
    ui_node* gitclone_ok = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(gitclone_ok, EVT_GITCLONE_OK);
    ui_set_rect(gitclone_ok, 32, 16, 10, 1);
    ui_set_label(gitclone_ok, "  OK  ");
    ui_append_child(gitclone_window, gitclone_ok);
    ui_node* gitclone_cancel = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(gitclone_cancel, EVT_GITCLONE_CANCEL);
    ui_set_rect(gitclone_cancel, 44, 16, 10, 1);
    ui_set_label(gitclone_cancel, "Cancel");
    ui_append_child(gitclone_window, gitclone_cancel);
    g_gitclone.window = gitclone_window;
    g_gitclone.modal = gitclone_modal;

    /* --- Environment modal --- */
    ui_node* env_modal = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, env_modal);
    ui_node* env_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(env_window, 20, 6, 40, 10);
    ui_set_label(env_window, " Environment ");
    ui_set_color(env_window, theme->modal_fg, theme->modal_bg);
    ui_append_child(env_modal, env_window);
    add_text(env_window, 23, 8, "Theme:", theme->label_fg, theme->modal_bg);
    ui_node* theme_select = add_select(env_window, 30, 8, 20);
    add_select_item(theme_select, EVT_ENV_THEME_AMBAR, "Ambar");
    add_select_item(theme_select, EVT_ENV_THEME_DARK, "Dark");
    add_select_item(theme_select, EVT_ENV_THEME_WHITE, "White");
    add_select_item(theme_select, EVT_ENV_THEME_NEBULA, "Nebula");
    add_select_item(theme_select, EVT_ENV_THEME_XCODE_DARK, "Xcode Dark");
    ui_select_set_selected(theme_select, g_envdlg.theme_index);
    g_envdlg.theme_select = theme_select;

    /* Font family, from whatever monospaced shortlist the backend offers
     * for this OS (see ui_env_set_font_family_fns). A backend that offers
     * none reports 0 and the row is simply left out - no empty control. */
    int font_count = ui_env_font_family_count(env);
    if (font_count > 0)
    {
        add_text(env_window, 23, 10, "Font:", theme->label_fg, theme->modal_bg);
        ui_node* font_select = add_select(env_window, 30, 10, 20);
        for (int i = 0; i < font_count; i++)
            add_select_item(font_select, EVT_ENV_FONT_BASE + i,
                            ui_env_font_family_name(env, i));
        if (g_envdlg.font_index >= font_count)
            g_envdlg.font_index = 0;
        ui_select_set_selected(font_select, g_envdlg.font_index);
        g_envdlg.font_select = font_select;
    }

    ui_node* env_ok = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(env_ok, EVT_ENV_OK);
    ui_set_rect(env_ok, 35, 13, 10, 1);
    ui_set_label(env_ok, "  OK  ");
    ui_append_child(env_window, env_ok);
    g_envdlg.modal = env_modal;

    /* --- New Project modal (Project > New Project...) --- */
    ui_node* newproj_modal = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, newproj_modal);
    ui_node* newproj_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(newproj_window, 12, 6, 54, 12);
    ui_set_label(newproj_window, " New Project ");
    ui_set_help(newproj_window,
                "Create a new Cake project (`.cakeproj`)", "# Create a new Cake project (`.cakeproj`)\n"
                "\n"
                "A project is a `.cakeproj` file: a list of source files, plus the include directories and compiler options used to build them. File paths inside the project folder are stored relative to it, so the project can be moved or shared.\n"
                "\n"
                "## Build and Compile\n"
                "\n"
                "- **Build** (F7) compiles every `.c` file of the project in one Cake invocation - linking them is the output compiler's job. When the active file is not part of the open project (or no project is open), Build compiles just that file.\n"
                "- **Compile** (Ctrl+F7) always compiles only the active file.\n"
                "\n"
                "## Project settings vs. global settings\n"
                "\n"
                "- **Project > Include Directories...** and **Project > Options...** edit the project's own settings, saved in its `.cakeproj`. Include directories are stored relative to the project folder.\n"
                "- **File > Directories...** and **File > Options...** edit the global settings in `cake.json`, next to the IDE executable. They are used for every file that is not part of the open project - the Playground, a file opened on its own.\n"
                "\n"
                "The two are never merged: a file gets either the project's settings or the global ones.\n"
                "\n"
                "With the `default` target, the same `.cakeproj` works unchanged on Windows, Linux and macOS.");
    ui_set_color(newproj_window, theme->modal_fg, theme->modal_bg);
    ui_append_child(newproj_modal, newproj_window);
    g_newproject.modal = newproj_modal;

    /* Both inputs (and the checkbox below) share the same left column at
     * x=29 - two units clear of "Project Name" (the longer of the two
     * labels, ending at x=27) so every row gets a visible gap, not just
     * "Folder"'s own short label. */
    add_text(newproj_window, 15, 8, "Folder", theme->label_fg, theme->modal_bg);
    g_newproject.folder_input = add_input(newproj_window, 29, 8, 27, "");
    ui_set_id(g_newproject.folder_input, EVT_PROJECT_NEW_FOLDER);
    ui_node* newproj_browse = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(newproj_browse, EVT_PROJECT_NEW_BROWSE);
    ui_set_rect(newproj_browse, 57, 8, 5, 1);
    ui_set_label(newproj_browse, "...");
    ui_append_child(newproj_window, newproj_browse);

    add_text(newproj_window, 15, 10, "Project Name", theme->label_fg, theme->modal_bg);
    /* Right edge lines up with the Browse ("...") button's own right edge
     * (57 + 5 == 62). */
    g_newproject.name_input = add_input(newproj_window, 29, 10, 33, "");
    ui_set_id(g_newproject.name_input, EVT_PROJECT_NEW_NAME);
    /* Help only where it isn't obvious (see ui_set_help) - Folder and the
     * buttons speak for themselves. */
    ui_set_help(g_newproject.name_input,
                "Name of the project file: `<name>.cakeproj`", "# Name of the project file\n\n`<name>.cakeproj`\n"
                "\n"
                "It is created in **Folder** - or in a new `<name>` subfolder of "
                "it when **Create Folder** is checked. Creation stops if a project "
                "with that name already exists there.");

    /* Two independent checkboxes (not mutually exclusive, hence multi=1) -
     * index 0 is "Create Folder", index 1 is "Hello World" (see
     * EVT_PROJECT_NEW_OK's own use of ui_group_get_checked). */
    g_newproject.helloworld_check = add_group(newproj_window, 29, 12, 20, 2, 1);
    ui_set_help(add_group_item(g_newproject.helloworld_check, "Create Folder"),
                "Create Folder: put the project in a new `<Project Name>` subfolder of Folder", "## Create Folder\n\nput the project in a new `<Project Name>` subfolder of Folder\n"
                "\n"
                "Unchecked, the project file goes straight into **Folder**. "
                "If the subfolder already exists, nothing is created.");
    ui_set_help(add_group_item(g_newproject.helloworld_check, "Hello World"),
                "Hello World: start the project with a `main.c` that prints \"Hello, world!\"", "## Hello World\n\nstart the project with a `main.c` that prints \"Hello, world!\"\n"
                "\n"
                "`main.c` is added to the project. An existing `main.c` in the "
                "project folder is never overwritten - it is added as it is.");

    /* 10-wide, same as every other dialog's OK/Cancel pair (Compiler
     * Options, Environment, Word Wrap, ...) - centered in the 54-wide
     * window: 12 + (54 - (10+2+10)) / 2 == 28. */
    ui_node* newproj_ok = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(newproj_ok, EVT_PROJECT_NEW_OK);
    ui_set_rect(newproj_ok, 28, 15, 10, 1);
    ui_set_label(newproj_ok, "  OK  ");
    ui_append_child(newproj_window, newproj_ok);

    ui_node* newproj_cancel = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(newproj_cancel, EVT_PROJECT_NEW_CANCEL);
    ui_set_rect(newproj_cancel, 40, 15, 10, 1);
    ui_set_label(newproj_cancel, "Cancel");
    ui_append_child(newproj_window, newproj_cancel);

    /* --- External Tools modal (Tools > External Tools...) --- */
    ui_node* ext_modal = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, ext_modal);
    /* Laid out against ex/ey so the whole dialog moves as a unit. Widths
     * are chosen so the list sits left with the Add/Delete column parked to
     * its right, and the field rows below run out to the window's own inner
     * right edge - input, a blank column, then that row's small button. */
    int ex = 10, ey = 2, ew = 66, eh = 22;
    const int list_w = 47, list_h = 7;       /* rows visible; the list scrolls
                                              * past that up to EXT_TOOL_MAX */
    const int btn_x = ex + 2 + list_w + 2;   /* right of the list */
    const int btn_w = 12;
    ui_node* ext_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(ext_window, ex, ey, ew, eh);
    ui_set_label(ext_window, " External Tools ");
    ui_set_help(ext_window,
                "Run a compiler or any other program from the Tools menu", "# Run a compiler or any other program from the Tools menu\n"
                "\n"
                "Cake only translates C to C89-compatible C - it does not link. Linking is left to a real compiler, run from here. Each tool added here appears in the **Tools** menu.\n"
                "\n"
                "A tool can also be run from the command line at the bottom of the **Output** window: type its **Title** and press Enter. Case, spaces and punctuation are ignored, so a tool titled `Run Tests` runs with `run tests` or `runtests`. Type `help` there for the other commands.\n"
                "\n"
                "A typical setup is one tool per compiler:\n"
                "\n"
                "**GCC / Clang** (Linux, macOS)\n"
                "\n"
                "| Field | Value |\n"
                "|---|---|\n"
                "| Title | `GCC` |\n"
                "| Command | `gcc` |\n"
                "| Arguments | `-g -Wno-incompatible-library-redeclaration -Wno-builtin-requires-header $(CakeOutput) -o \"$(TargetPath)\"` |\n"
                "| Directory | `$(ProjectDir)` |\n"
                "\n"
                "**MSVC** (Windows, from a Developer Command Prompt)\n"
                "\n"
                "| Field | Value |\n"
                "|---|---|\n"
                "| Title | `MSVC` |\n"
                "| Command | `cl` |\n"
                "| Arguments | `/Zi /nologo $(CakeOutput) /Fe\"$(TargetPath)\"` |\n"
                "| Directory | `$(ProjectDir)` |\n"
                "\n"
                "Running the tool after **Build** (F7) links Cake's output into `$(TargetPath)`, which is exactly the file **Debug** (F5) launches - so build, external compile and debug all agree on one binary.\n"
                "\n"
                "Cake's output declares the library functions it uses instead of keeping the original `#include`s. Clang flags those declarations with `-Wbuiltin-requires-header` and `-Wincompatible-library-redeclaration`; both are expected for Cake output, which is why the GCC/Clang example silences them.");
    ui_set_color(ext_window, theme->modal_fg, theme->modal_bg);
    ui_append_child(ext_modal, ext_window);

    /* Layout mirrors Visual Studio's own External Tools dialog: the list
     * on the left with its action buttons in a column to the right, then
     * the field rows underneath - labels left-aligned in their own column,
     * every input starting at the same x so they form a clean edge. */
    add_text(ext_window, ex + 2, ey + 2, "Menu contents:", theme->label_fg, theme->modal_bg);
    g_exttool.listbox = ui_create_element(UI_TAG_LISTBOX);
    ui_set_rect(g_exttool.listbox, ex + 2, ey + 3, list_w, list_h);
    ui_set_id(g_exttool.listbox, EVT_EXTTOOL_LIST);
    ui_append_child(ext_window, g_exttool.listbox);

    /* One button per row, evenly spaced, sharing the column at btn_x. */
    static const struct { int id; const char* label; } ext_buttons[] = {
        { EVT_EXTTOOL_ADD,      "  Add  "  },
        { EVT_EXTTOOL_DELETE,   " Delete " },
        { EVT_EXTTOOL_MOVEUP,   " Move Up " },
        { EVT_EXTTOOL_MOVEDOWN, "Move Down" },
    };
    for (int i = 0; i < (int)(sizeof ext_buttons / sizeof ext_buttons[0]); i++)
    {
        ui_node* b = ui_create_element(UI_TAG_BUTTON);
        ui_set_id(b, ext_buttons[i].id);
        ui_set_rect(b, btn_x, ey + 3 + i * 2, btn_w, 1);
        ui_set_label(b, ext_buttons[i].label);
        ui_append_child(ext_window, b);
    }

    int ext_fy = ey + 3 + list_h + 1;        /* first field row, below the list */
    /* Widest label ("Arguments:") sets where the inputs begin, so all four
     * share one left edge. They run all the way to the window's inner right
     * edge - past the list and its button column above - because these hold
     * full paths and command lines, which are the longest things in the
     * dialog and the most painful to edit through a short window. */
    const int field_x = ex + 2 + 12;
    /* Short of the inner right edge by a button plus one blank column, so
     * every row can end in its own small button - "..." on Command, ">" on
     * Arguments/Directory - without it touching the input. Title gets the
     * same width even though it has no button, so the four inputs still
     * share one right edge. */
    const int row_btn_w = 5;
    const int row_btn_gap = 1;
    const int field_w = (ex + ew - 2) - field_x - 1 - row_btn_gap - row_btn_w;
    const int row_btn_x = field_x + field_w + row_btn_gap;

    static const struct { int id; const char* label; } ext_fields[] = {
        { EVT_EXTTOOL_TITLE, "Title:"     },
        { EVT_EXTTOOL_CMD,   "Command:"   },
        { EVT_EXTTOOL_ARGS,  "Arguments:" },
        { EVT_EXTTOOL_DIR,   "Directory:" },
    };
    /* One blank row between fields - four inputs stacked without a gap read
     * as a single block; spaced out, each label sits with its own input.
     * Title has no trailing button, so it takes the blank column the other
     * rows keep between their input and it, running right up to where that
     * button column starts. */
    const int field_row_step = 2;
    /* Each row is built whole - label, input, then that row's own button -
     * because Tab walks the window's children in creation order (see
     * focus_next), so building all four inputs first and the buttons after
     * would tab through the fields and only then back up to the buttons.
     * Command's button browses for a program (the Open dialog defaulted to
     * the "Programs" filter); Arguments' and Directory's open the macro
     * popup - the whole vocabulary, picked from a list and inserted at the
     * caret, instead of the two lines of crib-sheet text that used to sit
     * under the fields and only ever fit half of it. Title takes no button,
     * so nothing follows its input - it takes that blank column plus five
     * more, running out past where the other rows stop. */
    ui_node* ext_inputs[4];
    for (int i = 0; i < 4; i++)
    {
        int y = ext_fy + i * field_row_step;
        add_text(ext_window, ex + 2, y, ext_fields[i].label,
                 theme->label_fg, theme->modal_bg);
        ext_inputs[i] = add_input(ext_window, field_x, y,
                                  i == 0 ? field_w + row_btn_gap + 5 : field_w, "");
        ui_set_id(ext_inputs[i], ext_fields[i].id);

        if (i == 0)
            continue;
        ui_node* b = ui_create_element(UI_TAG_BUTTON);
        ui_set_id(b, i == 1 ? EVT_EXTTOOL_BROWSE : EVT_EXTTOOL_MACRO_BTN + (i - 2));
        ui_set_rect(b, row_btn_x, y, row_btn_w, 1);
        ui_set_label(b, i == 1 ? " ... " : "  >  ");
        ui_append_child(ext_window, b);
    }
    g_exttool.title_input = ext_inputs[0];
    ui_set_help(g_exttool.title_input,
                "Name shown in the Tools menu", "# Name shown in the Tools menu\n"
                "\n"
                "It is also the tool's command: type it in the Output window's command line to run the tool. Case, spaces and punctuation are ignored there.");
    g_exttool.cmd_input   = ext_inputs[1];
    g_exttool.args_input  = ext_inputs[2];
    g_exttool.dir_input   = ext_inputs[3];
    ui_set_help(g_exttool.cmd_input,
                "Program to run, e.g. `gcc` or `cl`", "# Program to run, e.g. `gcc` or `cl`\n"
                "\n"
                "The **...** button browses for a program. Macros (see Arguments) work here too.");
    ui_set_help(g_exttool.args_input,
                "Command-line arguments - `$(...)` macros expand when the tool runs", "# Command-line arguments - `$(...)` macros expand when the tool runs\n"
                "\n"
                "The **>** button inserts a macro at the caret; hover a macro there to see what it means. `$$` is a literal `$`; an unknown macro expands to nothing.\n"
                "\n"
                "Example: active document `C:/work/hello/src/main.c`, project `hello` in `C:/work/hello`, target `x64_msvc`.\n"
                "\n"
                "| Macro | Example |\n"
                "|---|---|\n"
                "| `$(FilePath)` | `C:/work/hello/src/main.c` |\n"
                "| `$(FileDir)` | `C:/work/hello/src` |\n"
                "| `$(FileName)` | `main` |\n"
                "| `$(FileExt)` | `.c` |\n"
                "| `$(CakeOutput)` | one output path per `.c` of the project |\n"
                "| `$(TargetPath)` | `C:/work/hello/x64_msvc/hello.exe` |\n"
                "| `$(TargetDir)` | `C:/work/hello/x64_msvc` |\n"
                "| `$(TargetFileName)` | `hello.exe` |\n"
                "| `$(TargetName)` | `hello` |\n"
                "| `$(TargetExt)` | `.exe` |\n"
                "| `$(ProjectDir)` | `C:/work/hello` |\n"
                "| `$(ProjectName)` | `hello` |\n"
                "| `$(Platform)` | `x64_msvc` |");
    ui_set_help(g_exttool.dir_input,
                "Directory the tool runs in - usually `$(ProjectDir)`", "# Directory the tool runs in - usually `$(ProjectDir)`\n"
                "\n"
                "The **>** button inserts a macro at the caret.");
    g_exttool.macro_fields[0] = ext_inputs[2];   /* EVT_EXTTOOL_MACRO_BTN + 0 */
    g_exttool.macro_fields[1] = ext_inputs[3];   /* ...+ 1 */

    /* The popup itself - one <menu> shared by both ">" buttons, since only
     * one can be open at a time and the field it belongs to is remembered in
     * g_exttool.macro_target. Lives on the root, like every other popup. */
    g_exttool.macro_popup = ui_create_element(UI_TAG_MENU);
    ui_append_child(root, g_exttool.macro_popup);
    for (int i = 0; i < EXT_MACRO_COUNT; i++)
    {
        ui_node* it = ui_create_element(UI_TAG_ITEM);
        ui_set_id(it, EVT_EXTTOOL_MACRO_BASE + i);
        ui_set_label(it, ext_macros[i].name);
        ui_set_help(it, ext_macros[i].short_help, ext_macros[i].help);
        ui_append_child(g_exttool.macro_popup, it);
    }

    /* OK/Cancel as a pair centered across the dialog's whole width - they
     * close the dialog itself, so they belong to it rather than to the list
     * or the field column either edge would tie them to. */
    const int ok_gap = 2;
    const int ok_row_w = btn_w * 2 + ok_gap;
    const int ok_x = ex + (ew - ok_row_w) / 2;
    ui_node* ext_ok = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(ext_ok, EVT_EXTTOOL_OK);
    ui_set_rect(ext_ok, ok_x, ext_fy + 8, btn_w, 1);
    ui_set_label(ext_ok, "  OK  ");
    ui_append_child(ext_window, ext_ok);
    ui_node* ext_cancel = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(ext_cancel, EVT_EXTTOOL_CANCEL);
    ui_set_rect(ext_cancel, ok_x + btn_w + ok_gap, ext_fy + 8, btn_w, 1);
    ui_set_label(ext_cancel, "Cancel");
    ui_append_child(ext_window, ext_cancel);
    g_exttool.modal = ext_modal;

    /* --- Compiler Options modal --- */
    ui_node* copts_modal = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, copts_modal);
    ui_node* copts_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(copts_window, 15, 5, 62, 21);
    ui_set_label(copts_window, " Compiler Options ");
    ui_set_help(copts_window,
                "Compiler Options: how Cake compiles your files", "# Compiler Options\n\nhow Cake compiles your files\n"
                "\n"
                "There are two sets of these options, and the title says which one is being edited:\n"
                "\n"
                "- **Compiler Options** - the global options in `cake.json`, next to the IDE. Used for every file that is not part of the open project: the Playground, a file opened on its own.\n"
                "- **Compiler Options (Project)** - the open project's own options, in its `.cakeproj`. Used by **Build** (F7) and by **Compile** for the project's files.\n"
                "\n"
                "The two are never merged.\n"
                "\n"
                "## Fields\n"
                "\n"
                "- **Target** - the platform the generated C89 code is for: type sizes, alignment, output style. `default` keeps a project portable across Windows, Linux and macOS.\n"
                "- **Style** - the coding style diagnostic 11 checks, or none.\n"
                "- **Diagnostic** - how diagnostic positions are printed.\n"
                "- **Flags** - on/off switches: analysis, output, warnings, headers.\n"
                "- **Output** - the built binary's name.\n"
                "- **Options** - any other command-line option, typed as is.\n"
                "\n"
                "Focus a field and press F1 (or click the status bar) for its details - the **Options** field lists every other command-line option.");
    ui_set_color(copts_window, theme->modal_fg, theme->modal_bg);
    ui_append_child(copts_modal, copts_window);
    g_copts.window = copts_window;
    add_text(copts_window, 18, 7, "Target", theme->label_fg, theme->modal_bg);
    g_copts.target = add_select(copts_window, 29, 7, 25);
    char default_label[64];
    snprintf(default_label, sizeof default_label, "Default (%s)", get_platform(TARGET_DEFAULT)->name);
    /* Help texts follow manual.md's "Command-Line Options" section. First
     * line: the short version the status bar shows; the rest: the long one
     * F1/a click on the bar adds (see ui_set_help). */
    ui_set_help(g_copts.target,
                "Compilation target platform (`-target=<name>`)", "# Compilation target platform (`-target=<name>`)\n"
                "\n"
                "Controls integer sizes, alignment, and the style of generated C89 output. Pick the platform whose compiler will build the generated code - it does not have to be the one Cake is running on.");
    ui_set_help(add_select_item(g_copts.target, EVT_COPTS_TARGET + 0, default_label),
                "Default target: the platform Cake itself was built for", "## Default target\n\nthe platform Cake itself was built for\n"
                "\n"
                "Same as omitting `-target`. This build of Cake uses the target named in the list entry (e.g. `x64_msvc` for Cake built as a Windows x64 program).");
    ui_set_help(add_select_item(g_copts.target, EVT_COPTS_TARGET + 1, "X86 MSVC"),
                "`-target=x86_msvc`: Windows x86 (32-bit)", "## `-target=x86_msvc`\n\nWindows x86 (32-bit)\n"
                "\n"
                "Data model **ILP32**. Output compiler: MSVC.\n"
                "\n"
                "| Type | Size (bytes) |\n"
                "|---|---|\n"
                "| `char` (signed) | 1 |\n"
                "| `short` | 2 |\n"
                "| `int` | 4 |\n"
                "| `long` | 4 |\n"
                "| `long long` | 8 |\n"
                "| pointer | 4 |\n"
                "| `long double` | 8 |\n"
                "| `wchar_t` | 2 (`unsigned short`) |\n"
                "| `size_t` | 4 (`unsigned int`) |\n"
                "\n"
                "Thread-local storage is emitted as `__declspec(thread)`.\n"
                "\n"
                "The generated C89 goes to a `x86_msvc` folder next to the sources; compile it with the target compiler:\n"
                "\n"
                "```\n"
                "cl x86_msvc\\file1.c\n"
                "```");
    ui_set_help(add_select_item(g_copts.target, EVT_COPTS_TARGET + 2, "X64 MSVC"),
                "`-target=x64_msvc`: Windows x64", "## `-target=x64_msvc`\n\nWindows x64\n"
                "\n"
                "Data model **LLP64**. Output compiler: MSVC.\n"
                "\n"
                "| Type | Size (bytes) |\n"
                "|---|---|\n"
                "| `char` (signed) | 1 |\n"
                "| `short` | 2 |\n"
                "| `int` | 4 |\n"
                "| `long` | 4 |\n"
                "| `long long` | 8 |\n"
                "| pointer | 8 |\n"
                "| `long double` | 8 |\n"
                "| `wchar_t` | 2 (`unsigned short`) |\n"
                "| `size_t` | 8 (`unsigned long long`) |\n"
                "\n"
                "Thread-local storage is emitted as `__declspec(thread)`.\n"
                "\n"
                "The generated C89 goes to a `x64_msvc` folder next to the sources; compile it with the target compiler:\n"
                "\n"
                "```\n"
                "cl x64_msvc\\file1.c\n"
                "```");
    ui_set_help(add_select_item(g_copts.target, EVT_COPTS_TARGET + 3, "X64 GCC"),
                "`-target=x86_x64_gcc`: Linux x86-64", "## `-target=x86_x64_gcc`\n\nLinux x86-64\n"
                "\n"
                "Data model **LP64**. Output compiler: GCC.\n"
                "\n"
                "| Type | Size (bytes) |\n"
                "|---|---|\n"
                "| `char` (signed) | 1 |\n"
                "| `short` | 2 |\n"
                "| `int` | 4 |\n"
                "| `long` | 8 |\n"
                "| `long long` | 8 |\n"
                "| pointer | 8 |\n"
                "| `long double` | 16 |\n"
                "| `wchar_t` | 4 (`int`) |\n"
                "| `size_t` | 8 (`unsigned long`) |\n"
                "\n"
                "Thread-local storage is emitted as `__thread`.\n"
                "\n"
                "The generated C89 goes to a `x86_x64_gcc` folder next to the sources; compile it with the target compiler:\n"
                "\n"
                "```\n"
                "gcc -w x86_x64_gcc/file1.c -o file1\n"
                "```");
    ui_set_help(add_select_item(g_copts.target, EVT_COPTS_TARGET + 4, "macOS ARM64"),
                "`-target=macos_arm64`: macOS arm64 (Apple Silicon)", "## `-target=macos_arm64`\n\nmacOS arm64 (Apple Silicon)\n"
                "\n"
                "Data model **LP64**. Output compiler: Clang.\n"
                "\n"
                "| Type | Size (bytes) |\n"
                "|---|---|\n"
                "| `char` (signed) | 1 |\n"
                "| `short` | 2 |\n"
                "| `int` | 4 |\n"
                "| `long` | 8 |\n"
                "| `long long` | 8 |\n"
                "| pointer | 8 |\n"
                "| `long double` | 8 |\n"
                "| `wchar_t` | 4 (`int`) |\n"
                "| `size_t` | 8 (`unsigned long`) |\n"
                "\n"
                "Thread-local storage is emitted as `__thread`.\n"
                "\n"
                "The generated C89 goes to a `macos_arm64` folder next to the sources; compile it with the target compiler:\n"
                "\n"
                "```\n"
                "clang -w macos_arm64/file1.c -o file1\n"
                "```");
    ui_select_set_selected(g_copts.target, target_slug_to_index(g_compile.target));

    /* Style (-style=<name>) - see g_style_slugs' own comment for why only
     * these four are offered. */
    add_text(copts_window, 18, 9, "Style", theme->label_fg, theme->modal_bg);
    g_copts.style = add_select(copts_window, 29, 9, 25);
    ui_set_help(g_copts.style, "Coding style checked by diagnostic 11 (`-style=<name>`)", "# Coding style checked by diagnostic 11 (`-style=<name>`)\n\n"
                "Passing `-style` turns diagnostic 11 (style) on as a note.");
    ui_set_help(add_select_item(g_copts.style, EVT_COPTS_STYLE + 0, "disabled"),
                "No style check", "## No style check\n\nNo `-style` is passed, so diagnostic 11 (style) stays off.");
    ui_set_help(add_select_item(g_copts.style, EVT_COPTS_STYLE + 1, "cake"),
                "`-style=cake`: checks the code against Cake's own style", "## `-style=cake`\n\nchecks the code against Cake's own style");
    ui_set_help(add_select_item(g_copts.style, EVT_COPTS_STYLE + 2, "gnu"),
                "`-style=gnu`: checks the code against the GNU style", "## `-style=gnu`\n\nchecks the code against the GNU style");
    ui_set_help(add_select_item(g_copts.style, EVT_COPTS_STYLE + 3, "microsoft"),
                "`-style=microsoft`: checks the code against the Microsoft style", "## `-style=microsoft`\n\nchecks the code against the Microsoft style");
    ui_select_set_selected(g_copts.style, style_slug_to_index(g_compile.style));

    /* Output Format (-fdiagnostics-format=<name>) - see g_diagformat_slugs. */
    add_text(copts_window, 18, 11, "Diagnostic", theme->label_fg, theme->modal_bg);
    g_copts.diagformat = add_select(copts_window, 29, 11, 25);
    ui_set_help(g_copts.diagformat, "How diagnostic positions are printed (`-fdiagnostics-format=<format>`)", "# How diagnostic positions are printed (`-fdiagnostics-format=<format>`)\n\n"
                "Both shapes are understood by Visual Studio and by Visual Studio Code.");
    ui_set_help(add_select_item(g_copts.diagformat, EVT_COPTS_DIAGFORMAT + 0, "cake ide"),
                "`-fdiagnostics-format=ide`: file.c:1:2: warning 10: message", "## `-fdiagnostics-format=ide`\n\nfile.c:1:2: warning 10: message");
    ui_set_help(add_select_item(g_copts.diagformat, EVT_COPTS_DIAGFORMAT + 1, "gcc"),
                "`-fdiagnostics-format=gcc`: file.c:1:2: warning 10: message", "## `-fdiagnostics-format=gcc`\n\nfile.c:1:2: warning 10: message");
    ui_set_help(add_select_item(g_copts.diagformat, EVT_COPTS_DIAGFORMAT + 2, "msvc"),
                "`-fdiagnostics-format=msvc`: file.c(1,2): warning 10: message", "## `-fdiagnostics-format=msvc`\n\nfile.c(1,2): warning 10: message");
    ui_select_set_selected(g_copts.diagformat, diagformat_slug_to_index(g_compile.diagnostic_format));

    /* Flags - a check-box GROUP, same control as Find's "Options"
     * (g_find.opts) above (add_group/add_group_item). */
    add_text(copts_window, 18, 13, "Flags", theme->label_fg, theme->modal_bg);
    g_copts.flags = add_group(copts_window, 29, 13, 45, 5, 1);
    ui_set_help(add_group_item(g_copts.flags, "-no-output"),
                "`-no-output`: run all analysis passes but write no output file", "## `-no-output`\n\nrun all analysis passes but write no output file");
    ui_set_help(add_group_item(g_copts.flags, "-line-directives"),
                "`-line-directives`: emit `#line` directives in the generated C89 output", "## `-line-directives`\n\nemit `#line` directives in the generated C89 output\n\n"
                "Preserves source location information.");
    ui_set_help(add_group_item(g_copts.flags, "-fanalyzer"),
                "`-fanalyzer`: run Cake's built-in flow analysis", "## `-fanalyzer`\n\nrun Cake's built-in flow analysis\n\n"
                "Includes ownership, nullability, and lifetime checks.");
    ui_set_help(add_group_item(g_copts.flags, "-const-literal"),
                "`-const-literal`: treat string literals as `const char[]` rather than `char[]`", "## `-const-literal`\n\ntreat string literals as `const char[]` rather than `char[]`");
    ui_set_help(add_group_item(g_copts.flags, "-Wall"),
                "`-Wall`: enable all warnings", "## `-Wall`\n\nenable all warnings");
    ui_set_help(add_group_item(g_copts.flags, "-unused-extern-report"),
                "`-unused-extern-report`: report external functions never called", "## `-unused-extern-report`\n\nreport external functions never called\n\n"
                "Tracks every non-static (external linkage) "
                "function across all the files given in this invocation, and after "
                "the last one is compiled, report the ones that were never called "
                "in any of them.");
    ui_set_help(add_group_item(g_copts.flags, "-cake-headers"),
                "`-cake-headers`: use only Cake's own headers, never the system ones", "## `-cake-headers`\n\nuse only Cake's own headers, never the system ones\n\n"
                "Cake's headers declare everything themselves instead "
                "of deferring to `#include_next`, so the real system headers are never "
                "consulted. Used to compile Cake itself and run its tests portably; "
                "not meant for ordinary programs.");
    ui_group_set_checked(g_copts.flags, 0, g_compile.no_output);
    ui_group_set_checked(g_copts.flags, 1, g_compile.line_directives);
    ui_group_set_checked(g_copts.flags, 2, g_compile.fanalyzer);
    ui_group_set_checked(g_copts.flags, 3, g_compile.const_literal);
    ui_group_set_checked(g_copts.flags, 4, g_compile.wall);
    ui_group_set_checked(g_copts.flags, 5, g_compile.unused_extern_report);
    ui_group_set_checked(g_copts.flags, 6, g_compile.use_cake_headers);

    /* The built executable's name - what $(TargetFileName) expands to and
     * what Debug launches; empty means "derive it" (see target_file_name). */
    add_text(copts_window, 18, 19, "Output", theme->label_fg, theme->modal_bg);
    g_copts.output = add_input(copts_window, 29, 19, 45, "");
    ui_set_id(g_copts.output, EVT_COPTS_OK);
    ui_set_help(g_copts.output, "Name of the built executable (empty: derived from the source/project)", "# Name of the built executable\n\nEmpty: derived from the source/project.\n\n"
                "What `$(TargetFileName)` expands to and what Debug launches.");

    /* Free-text options last - anything the rows above don't cover. */
    add_text(copts_window, 18, 21, "Options", theme->label_fg, theme->modal_bg);
    g_copts.input = add_input(copts_window, 29, 21, 45, "");
    ui_set_id(g_copts.input, EVT_COPTS_OK);
    ui_set_help(g_copts.input,
                "Other command-line options, passed to cake as typed", "# Other command-line options, passed to cake as typed\n"
                "\n"
                "Everything the fields above don't cover.\n"
                "\n"
                "## Diagnostics\n"
                "\n"
                "| Option | Effect |\n"
                "|---|---|\n"
                "| `-w<number>` | enable warning number `<number>`, e.g. `-w2` |\n"
                "| `-wd<number>` | disable warning number `<number>`, e.g. `-wd2` |\n"
                "| `-Werror` | report every enabled warning as an error |\n"
                "\n"
                "Most warnings are on unless `-wd<number>` turns them off, but a few are off until asked for:\n"
                "\n"
                "| Number | Warning |\n"
                "|---|---|\n"
                "| `2` | unused variable |\n"
                "| `6` | unused function parameter |\n"
                "| `11` | style |\n"
                "| `33` | nullable pointer flow check |\n"
                "| `35` | nullable pointer flow check |\n"
                "| `83` | parameter set but not used |\n"
                "| `84` | variable set but not used |\n"
                "\n"
                "With `-Werror`, notes are not affected and disabled warnings stay disabled. Because they become errors, warnings coming from included headers are no longer suppressed, and any occurrence makes the compilation fail.\n"
                "\n"
                "Suppress a diagnostic on one line with a trailing `lint` comment listing its number(s): `//lint 35`, `// lint 35`, or `/* lint 81 */`. An unnecessary suppression is flagged with warning 59.\n"
                "\n"
                "## Analysis\n"
                "\n"
                "| Option | Effect |\n"
                "|---|---|\n"
                "| `-ownership=enable` / `-ownership=disable` | turn the ownership checks on or off |\n"
                "| `-nullable=enabled` / `-nullable=disable` | turn the nullable pointer checks on or off (`-nullchecks` = `enabled`) |\n"
                "| `-no-discard` | make `[[nodiscard]]` the default for every function |\n"
                "\n"
                "## Preprocessor\n"
                "\n"
                "| Option | Effect |\n"
                "|---|---|\n"
                "| `-I <dir>` | add `<dir>` to the include search path |\n"
                "| `-D <macro>` | define a preprocessing symbol |\n"
                "| `-E` | print the preprocessor output instead of compiling |\n"
                "| `-H` | list every include file used |\n"
                "| `-dump-tokens` | print the tokens before preprocessing |\n"
                "| `-dump-pp-tokens` | print the tokens after preprocessing |\n"
                "| `-preprocess-def-macro` | preprocess `#define` macros after expansion |\n"
                "| `-keep-inactive-tokens` | keep the tokens of inactive blocks (`#if 0`) instead of discarding them |\n"
                "\n"
                "## Output\n"
                "\n"
                "| Option | Effect |\n"
                "|---|---|\n"
                "| `-o <name.c>` | output file name, when compiling a single file |\n"
                "| `-dont-generate-time-stamp` | leave the timestamp comment out of the generated file |\n"
                "| `-msvc-output` | diagnostics for the Visual Studio error parser (`-fdiagnostics-format=msvc` plus no colors) |\n"
                "| `-fdiagnostics-color=never` | no ANSI colors in diagnostics |\n"
                "| `-sarif` | also write SARIF diagnostic files |\n"
                "| `-sarif-path <dir>` | directory for the SARIF files |\n"
                "\n"
                "## Formatting\n"
                "\n"
                "| Option | Effect |\n"
                "|---|---|\n"
                "| `-format` | reformat the file to match the Style and print it instead of compiling |\n"
                "| `-format-lines=<first>:<last>` | restrict `-format` to a line range |\n"
                "\n"
                "## Language\n"
                "\n"
                "| Option | Effect |\n"
                "|---|---|\n"
                "| `-std=c23` | input is C23 (also `-std=c2x`) |\n"
                "| `-std=cxx` | input is C with Cake's extensions |\n"
                "\n"
                "## Setup\n"
                "\n"
                "| Option | Effect |\n"
                "|---|---|\n"
                "| `-auto-config` | generate `cake.json` with the include directories of the current system |");

    ui_node* copts_ok = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(copts_ok, EVT_COPTS_OK);
    ui_set_rect(copts_ok, 27, 23, 10, 1);
    ui_set_label(copts_ok, "  OK  ");
    ui_append_child(copts_window, copts_ok);
    ui_node* copts_cancel = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(copts_cancel, EVT_COPTS_CANCEL);
    ui_set_rect(copts_cancel, 41, 23, 10, 1);
    ui_set_label(copts_cancel, "Cancel");
    ui_append_child(copts_window, copts_cancel);
    ui_node* copts_help = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(copts_help, EVT_COPTS_HELP);
    ui_set_rect(copts_help, 55, 23, 10, 1);
    ui_set_label(copts_help, " Help ");
    ui_set_no_focus(copts_help, 1);
    ui_append_child(copts_window, copts_help);
    g_copts.modal = copts_modal;

    /* --- Full-help window (F1 on a status bar hint - see show_hint_window) --- */
    ui_node* hint_modal = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, hint_modal);
    int hx = 20, hy = 4, hw = 80, hh = 20;
    ui_node* hint_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(hint_window, hx, hy, hw, hh);
    ui_set_label(hint_window, " Help ");
    ui_set_color(hint_window, theme->modal_fg, theme->modal_bg);
    ui_append_child(hint_modal, hint_window);
    ui_node* hint_editor = ui_create_element(UI_TAG_EDITOR);
    ui_set_rect(hint_editor, hx + 2, hy + 1, hw - 4, hh - 5);
    ui_set_syntax(hint_editor, UI_SYNTAX_MARKDOWN);
    ui_set_read_only(hint_editor, 1);
    ui_append_child(hint_window, hint_editor);
    ui_node* hint_close = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(hint_close, EVT_HINTWIN_CLOSE);
    ui_set_rect(hint_close, hx + (hw - 10) / 2, hy + hh - 3, 10, 1);
    ui_set_label(hint_close, " Close ");
    ui_append_child(hint_window, hint_close);
    g_hintwin.modal = hint_modal;
    g_hintwin.editor = hint_editor;

    /* --- Open File modal --- */
    ui_node* open_modal = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, open_modal);
    int ox = 10, oy = 2, ow2 = 61, oh2 = 21;  /* the dialog's own convention is
                                                * a blank row before each new
                                                * labeled section (see the gap
                                                * before "Files" below) - the
                                                * Type row needs the same, plus
                                                * a little breathing room below
                                                * it before the window's edge */
    ui_node* open_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(open_window, ox, oy, ow2, oh2);
    ui_set_label(open_window, " Open a File ");
    ui_set_color(open_window, theme->modal_fg, theme->modal_bg);
    ui_append_child(open_modal, open_window);
    g_open.window = open_window;
    add_text(open_window, ox + 3, oy + 2, "Name", theme->label_fg, theme->modal_bg);
    g_open.name_input = add_input(open_window, ox + 3, oy + 3, 41, "");
    ui_set_id(g_open.name_input, EVT_OPEN_NAME);
    g_open.list_label = add_text(open_window, ox + 3, oy + 5, "Files", theme->label_fg, theme->modal_bg);
    g_open.listbox = ui_create_element(UI_TAG_LISTBOX);
    ui_set_rect(g_open.listbox, ox + 3, oy + 6, 41, 10);
    ui_set_id(g_open.listbox, EVT_OPEN_LISTBOX);
    ui_append_child(open_window, g_open.listbox);
    g_open.filter_label = add_text(open_window, ox + 3, oy + 17, "Type", theme->label_fg, theme->modal_bg);
    g_open.filter = add_select(open_window, ox + 3, oy + 18, 41);
    for (int i = 0; i < OPEN_FILTER_COUNT; i++)
        add_select_item(g_open.filter, EVT_OPEN_FILTER + i, g_open_filters[i].label);
    ui_select_set_selected(g_open.filter, 0);
    int obx = ox + 46;
    ui_node* open_ok = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(open_ok, EVT_OPEN_OK);
    ui_set_rect(open_ok, obx, oy + 3, 12, 1);
    ui_set_label(open_ok, "  Open  ");
    ui_append_child(open_window, open_ok);
    g_open.ok = open_ok;
    ui_node* open_cancel = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(open_cancel, EVT_OPEN_CANCEL);
    ui_set_rect(open_cancel, obx, oy + 5, 12, 1);
    ui_set_label(open_cancel, " Cancel ");
    ui_append_child(open_window, open_cancel);
    g_open.modal = open_modal;

    /* --- Output window --- */
    ui_node* output_wrapper = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, output_wrapper);
    int ow_x = 8, ow_y = 4, ow_w = 60, ow_h = 12;
    ui_node* output_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(output_window, ow_x, ow_y, ow_w, ow_h);
    ui_set_label(output_window, " Output ");
    ui_set_color(output_window, theme->window_fg, theme->window_bg);
    ui_set_resizable(output_window, 1);
    ui_set_shadow(output_window, 0);
    ui_set_dock(output_window, UI_DOCK_BOTTOM, ow_h);
    ui_append_child(output_wrapper, output_window);

    ui_node* output = ui_create_element(UI_TAG_EDITOR);
    ui_set_id(output, EVT_OUTPUT_DBLCLICK);
    ui_set_rect(output, ow_x + 1, ow_y + 1, ow_w - 2, ow_h - 3);
    ui_set_syntax(output, UI_SYNTAX_VT100);
    ui_set_small_font(output, 1);
    ui_set_value(output, "");
    ui_append_child(output_window, output);
    g_output_window = output_wrapper;
    g_output_editor = output;

    /* Position and colors are set per frame by cmdline_layout(). */
    g_cmdline.prompt = add_text(output_window, ow_x + 1, ow_y + ow_h - 2, ">", theme->editor_output_fg, theme->editor_output_bg);
    g_cmdline.input = add_input(output_window, ow_x + 3, ow_y + ow_h - 2, ow_w - 4, "");
    ui_set_id(g_cmdline.input, EVT_OUTPUT_CMDLINE);

    /* --- Folder window --- */
    ui_node* folder_wrapper = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, folder_wrapper);
    int fw_x = 10, fw_y = 3, fw_w = 20, fw_h = 16;
    ui_node* folder_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(folder_window, fw_x, fw_y, fw_w, fw_h);
    ui_set_color(folder_window, theme->window_fg, theme->window_bg);
    ui_set_resizable(folder_window, 1);
    ui_set_shadow(folder_window, 0);
    ui_set_dock(folder_window, UI_DOCK_LEFT, fw_w);
    ui_append_child(folder_wrapper, folder_window);

    g_folder.listbox = ui_create_element(UI_TAG_LISTBOX);
    ui_set_id(g_folder.listbox, EVT_FOLDER_LISTBOX);
    ui_set_rect(g_folder.listbox, fw_x + 1, fw_y + 1, fw_w - 2, fw_h - 2);
    ui_set_small_font(g_folder.listbox, 1);
    ui_append_child(folder_window, g_folder.listbox);
    g_folder.window = folder_wrapper;
    if (!ui_get_cwd(g_folder.dir, sizeof g_folder.dir))
        strcpy(g_folder.dir, ".");
    folder_window_refresh();

    /* --- Git Changes window (docked panel, `git status --porcelain`
     * listing - see g_git's own doc comment) --- */
    ui_node* git_wrapper = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, git_wrapper);
    int gw_x = 10, gw_y = 3, gw_w = 24, gw_h = 18;
    ui_node* git_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(git_window, gw_x, gw_y, gw_w, gw_h);
    ui_set_label(git_window, " Git Changes ");
    ui_set_color(git_window, theme->window_fg, theme->window_bg);
    ui_set_resizable(git_window, 1);
    ui_set_shadow(git_window, 0);
    ui_set_dock(git_window, UI_DOCK_LEFT, gw_w);
    ui_append_child(git_wrapper, git_window);

    /* Fills the whole panel, same as Folder/Project's own listbox - Commit/
     * Discard/Pull/Push are the right-click popup below (g_git.popup), not
     * buttons or an always-visible input row, so there's no empty control
     * sitting above the file list - see git_commit_start()/git_do_discard()/
     * git_do_pull()/git_do_push(). */
    g_git.listbox = ui_create_element(UI_TAG_LISTBOX);
    ui_set_id(g_git.listbox, EVT_GIT_LISTBOX);
    ui_set_rect(g_git.listbox, gw_x + 1, gw_y + 1, gw_w - 2, gw_h - 2);
    ui_set_small_font(g_git.listbox, 1);
    ui_append_child(git_window, g_git.listbox);
    g_git.window = git_wrapper;

    /* --- Git Changes panel context menu popup - Commit/Discard/Pull/Push,
     * same shape as g_folder.popup just above. Reuses the exact same event
     * ids the buttons used to carry (EVT_GIT_COMMIT_BTN etc.) - the
     * dispatcher below doesn't care whether the id came from a <button> or a
     * popup <item>. */
    ui_node* git_popup = ui_create_element(UI_TAG_MENU);
    ui_append_child(root, git_popup);
    ui_node* git_popup_commit = ui_create_element(UI_TAG_ITEM);
    ui_set_id(git_popup_commit, EVT_GIT_COMMIT_BTN);
    ui_set_label(git_popup_commit, "Commit All");
    ui_append_child(git_popup, git_popup_commit);
    ui_node* git_popup_commitfile = ui_create_element(UI_TAG_ITEM);
    ui_set_id(git_popup_commitfile, EVT_GIT_COMMITFILE_BTN);
    ui_set_label(git_popup_commitfile, "Commit File");
    ui_append_child(git_popup, git_popup_commitfile);
    ui_node* git_popup_commitstaged = ui_create_element(UI_TAG_ITEM);
    ui_set_id(git_popup_commitstaged, EVT_GIT_COMMITSTAGED_BTN);
    ui_set_label(git_popup_commitstaged, "Commit Staged");
    ui_node* git_popup_commitstagedpush = ui_create_element(UI_TAG_ITEM);
    ui_set_id(git_popup_commitstagedpush, EVT_GIT_COMMITSTAGEDPUSH_BTN);
    ui_set_label(git_popup_commitstagedpush, "Commit Staged && Push");
    ui_node* git_popup_stage = ui_create_element(UI_TAG_ITEM);
    ui_set_id(git_popup_stage, EVT_GIT_STAGE_BTN);
    ui_set_label(git_popup_stage, "Stage");
    ui_append_child(git_popup, git_popup_stage);
    ui_node* git_popup_unstage = ui_create_element(UI_TAG_ITEM);
    ui_set_id(git_popup_unstage, EVT_GIT_UNSTAGE_BTN);
    ui_set_label(git_popup_unstage, "Unstage");
    /* Not appended here - git_popup_refresh() inserts them while something is staged. */
    g_git.staged_items[0] = git_popup_commitstaged;
    g_git.staged_anchors[0] = git_popup_commitfile;
    g_git.staged_items[1] = git_popup_commitstagedpush;
    g_git.staged_anchors[1] = git_popup_commitstaged;
    g_git.staged_items[2] = git_popup_unstage;
    g_git.staged_anchors[2] = git_popup_stage;
    ui_node* git_popup_commitpush = ui_create_element(UI_TAG_ITEM);
    ui_set_id(git_popup_commitpush, EVT_GIT_COMMITPUSH_BTN);
    ui_set_label(git_popup_commitpush, "Commit All && Push");
    ui_append_child(git_popup, git_popup_commitpush);
    ui_node* git_popup_discard = ui_create_element(UI_TAG_ITEM);
    ui_set_id(git_popup_discard, EVT_GIT_DISCARD_BTN);
    ui_set_label(git_popup_discard, "Discard");
    ui_append_child(git_popup, git_popup_discard);
    ui_node* git_popup_sep = ui_create_element(UI_TAG_ITEM);
    ui_set_separator(git_popup_sep, 1);
    ui_append_child(git_popup, git_popup_sep);
    ui_node* git_popup_pull = ui_create_element(UI_TAG_ITEM);
    ui_set_id(git_popup_pull, EVT_GIT_PULL_BTN);
    ui_set_label(git_popup_pull, "Pull");
    ui_append_child(git_popup, git_popup_pull);
    ui_node* git_popup_push = ui_create_element(UI_TAG_ITEM);
    ui_set_id(git_popup_push, EVT_GIT_PUSH_BTN);
    ui_set_label(git_popup_push, "Push");
    ui_append_child(git_popup, git_popup_push);
    ui_node* git_popup_sync = ui_create_element(UI_TAG_ITEM);
    ui_set_id(git_popup_sync, EVT_GIT_SYNC_BTN);
    ui_set_label(git_popup_sync, "Sync");
    ui_append_child(git_popup, git_popup_sync);
    ui_node* git_popup_branch = ui_create_element(UI_TAG_ITEM);
    ui_set_id(git_popup_branch, EVT_GIT_BRANCH_BTN);
    ui_set_label(git_popup_branch, "Branch...");
    ui_append_child(git_popup, git_popup_branch);
    ui_node* git_popup_sep2 = ui_create_element(UI_TAG_ITEM);
    ui_set_separator(git_popup_sep2, 1);
    ui_append_child(git_popup, git_popup_sep2);
    ui_node* git_popup_clone = ui_create_element(UI_TAG_ITEM);
    ui_set_id(git_popup_clone, EVT_GIT_CLONE_BTN);
    ui_set_label(git_popup_clone, "Clone...");
    ui_append_child(git_popup, git_popup_clone);
    g_git.popup = git_popup;

    /* --- Git Diff window (floating, singleton - reused for every diff the
     * Git Changes panel opens, see g_gitdiff_window's own doc comment) --- */
    ui_node* gitdiff_wrapper = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, gitdiff_wrapper);
    int gd_x = 8, gd_y = 4, gd_w = 76, gd_h = 22;
    ui_node* gitdiff_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(gitdiff_window, gd_x, gd_y, gd_w, gd_h);
    ui_set_label(gitdiff_window, " Diff ");
    ui_set_color(gitdiff_window, theme->window_fg, theme->window_bg);
    ui_set_resizable(gitdiff_window, 1);
    ui_set_shadow(gitdiff_window, 0);
    ui_append_child(gitdiff_wrapper, gitdiff_window);

    /* "< Prev Change"/"Next Change >" - the diff always shows the whole file
     * now (see git_window_activate's -U100000), so jumping straight between
     * the actual +/- runs is the only practical way to find them in a large
     * file - see git_diff_goto_change(). */
    ui_node* gitdiff_prev = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(gitdiff_prev, EVT_GITDIFF_PREV);
    ui_set_rect(gitdiff_prev, gd_x + 1, gd_y + 1, 14, 1);
    ui_set_label(gitdiff_prev, " Previous \xE2\x86\x91 ");  /* U+2191 up arrow */
    ui_append_child(gitdiff_window, gitdiff_prev);
    g_gitdiff_counter = add_text(gitdiff_window, gd_x + 28, gd_y + 1, "", theme->window_fg, theme->window_bg);
    ui_node* gitdiff_next = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(gitdiff_next, EVT_GITDIFF_NEXT);
    ui_set_rect(gitdiff_next, gd_x + 16, gd_y + 1, 10, 1);
    ui_set_label(gitdiff_next, " \xE2\x86\x93 Next ");  /* U+2193 down arrow */
    ui_append_child(gitdiff_window, gitdiff_next);

    /* Row gd_y + 2 is left blank - a gap between the Prev/Next buttons and
     * the diff text below, so the buttons read as their own toolbar strip
     * rather than sitting flush against the first line of code. */
    ui_node* gitdiff_editor = ui_create_element(UI_TAG_EDITOR);
    ui_set_rect(gitdiff_editor, gd_x + 1, gd_y + 3, gd_w - 2, gd_h - 4);
    ui_set_syntax(gitdiff_editor, UI_SYNTAX_DIFF);
    ui_set_value(gitdiff_editor, "");
    ui_append_child(gitdiff_window, gitdiff_editor);
    g_gitdiff_window = gitdiff_wrapper;
    g_gitdiff_editor = gitdiff_editor;

    /* --- Git Diff viewer context menu popup - same items as the editor popup's --- */
    ui_node* gitdiff_popup = ui_create_element(UI_TAG_MENU);
    ui_append_child(root, gitdiff_popup);
    ui_node* gitdiff_popup_edit = ui_create_element(UI_TAG_ITEM);
    ui_set_id(gitdiff_popup_edit, EVT_GITDIFF_EDIT);
    ui_set_label(gitdiff_popup_edit, "Edit");
    ui_append_child(gitdiff_popup, gitdiff_popup_edit);
    ui_node* gitdiff_popup_copy_path = ui_create_element(UI_TAG_ITEM);
    ui_set_id(gitdiff_popup_copy_path, EVT_GITDIFF_COPY_PATH);
    ui_set_label(gitdiff_popup_copy_path, "Copy Full Path");
    ui_append_child(gitdiff_popup, gitdiff_popup_copy_path);
    ui_node* gitdiff_popup_show_folder = ui_create_element(UI_TAG_ITEM);
    ui_set_id(gitdiff_popup_show_folder, EVT_GITDIFF_SHOW_FOLDER);
    ui_set_label(gitdiff_popup_show_folder, "Show My Folder");
    ui_append_child(gitdiff_popup, gitdiff_popup_show_folder);
    g_gitdiff_popup = gitdiff_popup;

    /* --- Debug Info window (Locals + Call Stack, see debug_info_panel_
     * refresh()) --- Docked RIGHT, the one dock side Output (BOTTOM) and
     * Folder/Project (LEFT) leave free - dock_layout() (ide_ui.c) only
     * ever lays out one window per side, so this is a single combined
     * panel rather than two separate ones (see Stage 4's own plan notes). */
    ui_node* debuginfo_wrapper = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, debuginfo_wrapper);
    int dw_x = 10, dw_y = 3, dw_w = 30, dw_h = 16;
    ui_node* debuginfo_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(debuginfo_window, dw_x, dw_y, dw_w, dw_h);
    ui_set_label(debuginfo_window, " Debug Info ");
    ui_set_color(debuginfo_window, theme->window_fg, theme->window_bg);
    ui_set_resizable(debuginfo_window, 1);
    ui_set_shadow(debuginfo_window, 0);
    ui_set_dock(debuginfo_window, UI_DOCK_RIGHT, dw_w);
    ui_append_child(debuginfo_wrapper, debuginfo_window);

    g_debuginfo_listbox = ui_create_element(UI_TAG_LISTBOX);
    ui_set_rect(g_debuginfo_listbox, dw_x + 1, dw_y + 1, dw_w - 2, dw_h - 2);
    ui_append_child(debuginfo_window, g_debuginfo_listbox);
    g_debuginfo_window = debuginfo_wrapper;

    /* --- Project panel (Project > New/Open Project, View > "Show Project") ---
     * Docked LEFT by default, same as the Folder panel just above (same
     * ui_set_dock() mechanism Output/Folder/Find and Replace already share -
     * see g_dockmenu, whose right-click "Dock Left/Right/Bottom" popup works
     * on any docked panel generically via docked_panel_frame_at(), this one
     * included, no extra wiring needed). dock_layout() in ide_ui.c only lays
     * out one window per side, so showing this at the same time as another
     * LEFT-docked panel is the same known limitation as redocking, say, Find
     * and Replace onto LEFT while Folder is there too - move one of them to
     * Right or Bottom via that popup instead. */
    ui_node* project_wrapper = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, project_wrapper);
    int pw_x = 10, pw_y = 3, pw_w = 20, pw_h = 16;
    ui_node* project_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(project_window, pw_x, pw_y, pw_w, pw_h);
    ui_set_color(project_window, theme->window_fg, theme->window_bg);
    ui_set_resizable(project_window, 1);
    ui_set_shadow(project_window, 0);
    ui_set_dock(project_window, UI_DOCK_LEFT, pw_w);
    ui_append_child(project_wrapper, project_window);

    g_project.listbox = ui_create_element(UI_TAG_LISTBOX);
    ui_set_id(g_project.listbox, EVT_PROJECT_LISTBOX);
    ui_set_rect(g_project.listbox, pw_x + 1, pw_y + 1, pw_w - 2, pw_h - 2);
    ui_set_small_font(g_project.listbox, 1);
    ui_append_child(project_window, g_project.listbox);
    g_project.window = project_wrapper;
    project_window_refresh(0);

    /* --- Project panel context menu popup --- */
    ui_node* project_popup = ui_create_element(UI_TAG_MENU);
    ui_append_child(root, project_popup);
    ui_node* project_popup_open = ui_create_element(UI_TAG_ITEM);
    ui_set_id(project_popup_open, EVT_PROJECT_POPUP_OPEN);
    ui_set_label(project_popup_open, "Open");
    ui_append_child(project_popup, project_popup_open);
    ui_node* project_popup_newfile = ui_create_element(UI_TAG_ITEM);
    ui_set_id(project_popup_newfile, EVT_PROJECT_POPUP_NEWFILE);
    ui_set_label(project_popup_newfile, "New File...");
    ui_append_child(project_popup, project_popup_newfile);
    ui_node* project_popup_sep = ui_create_element(UI_TAG_ITEM);
    ui_set_separator(project_popup_sep, 1);
    ui_append_child(project_popup, project_popup_sep);
    ui_node* project_popup_remove = ui_create_element(UI_TAG_ITEM);
    ui_set_id(project_popup_remove, EVT_PROJECT_POPUP_REMOVE);
    ui_set_label(project_popup_remove, "Remove from Project");
    ui_append_child(project_popup, project_popup_remove);
    g_project.popup = project_popup;

    /* --- Project > "Include Directories..." dialog --- a small list editor
     * over g_project.include_dirs[], same "listbox + Add/Remove/Close"
     * shape the rest of this app uses for a plain list of strings (see
     * EVT_PROJECT_INCLUDES/project_includes_dialog_refresh()). "Add..."
     * reopens the Open dialog (g_open) in folder-picker mode on top of this
     * one instead of duplicating that picker here. */
    ui_node* includes_modal = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, includes_modal);
    int inc_x = 12, inc_y = 5, inc_w = 66, inc_h = 17;
    ui_node* includes_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(includes_window, inc_x, inc_y, inc_w, inc_h);
    ui_set_label(includes_window, " Include Directories ");
    ui_set_color(includes_window, theme->modal_fg, theme->modal_bg);
    ui_append_child(includes_modal, includes_window);

    /* A blank row below the title bar before "Directories", same convention
     * as the Open dialog's own "Name"/"Files" labels (see ox/oy above). */
    add_text(includes_window, inc_x + 2, inc_y + 2, "Directories", theme->label_fg, theme->modal_bg);

    g_project.includes_listbox = ui_create_element(UI_TAG_LISTBOX);
    ui_set_id(g_project.includes_listbox, EVT_PROJECT_INCLUDES_LISTBOX);
    ui_set_rect(g_project.includes_listbox, inc_x + 2, inc_y + 3, inc_w - 20, inc_h - 5);
    ui_append_child(includes_window, g_project.includes_listbox);

    const int inc_bw = 13;            /* fits the widest label, " Move Down " */
    int inc_bx = inc_x + inc_w - inc_bw - 3;  /* leaves a margin after the
                                               * buttons, before the window's
                                               * own right border */
    ui_node* includes_add = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(includes_add, EVT_PROJECT_INCLUDES_ADD);
    ui_set_rect(includes_add, inc_bx, inc_y + 3, inc_bw, 1);  /* aligned with the
                                                           * listbox's own top
                                                           * edge, below the
                                                           * "Directories" label */
    ui_set_label(includes_add, " Add... ");
    ui_append_child(includes_window, includes_add);
    ui_node* includes_remove = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(includes_remove, EVT_PROJECT_INCLUDES_REMOVE);
    ui_set_rect(includes_remove, inc_bx, inc_y + 5, inc_bw, 1);  /* 2-row gaps
                                                              * between buttons -
                                                              * keeps them close
                                                              * to each other */
    ui_set_label(includes_remove, " Remove ");
    ui_append_child(includes_window, includes_remove);
    /* Include directories are searched in order, so the list is ordered
     * rather than a set - these two reorder the selected row. */
    ui_node* includes_up = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(includes_up, EVT_PROJECT_INCLUDES_UP);
    ui_set_rect(includes_up, inc_bx, inc_y + 7, inc_bw, 1);
    ui_set_label(includes_up, " Move Up ");
    ui_append_child(includes_window, includes_up);
    ui_node* includes_down = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(includes_down, EVT_PROJECT_INCLUDES_DOWN);
    ui_set_rect(includes_down, inc_bx, inc_y + 9, inc_bw, 1);
    ui_set_label(includes_down, " Move Down ");
    ui_append_child(includes_window, includes_down);
    /* Not appended here - includes_set_detect_visible() attaches it only
     * while the dialog is editing the global (system) list. */
    ui_node* includes_detect = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(includes_detect, EVT_PROJECT_INCLUDES_DETECT);
    ui_set_rect(includes_detect, inc_bx, inc_y + 11, inc_bw, 1);
    ui_set_label(includes_detect, " Detect ");
    g_project.includes_detect = includes_detect;
    ui_set_help(g_project.includes_detect,
                "Replace the list with the include directories the platform compiler searches", "# Replace the list with the include directories the platform compiler searches\n"
                "\n"
                "On Windows, MSVC's headers are found with `vswhere.exe` and the Windows SDK's from the registry. Anything that can't be found is reported; what was found is still used.");

    ui_node* includes_close = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(includes_close, EVT_PROJECT_INCLUDES_CLOSE);
    ui_set_rect(includes_close, inc_bx, inc_y + 13, inc_bw, 1);
    ui_set_label(includes_close, " Close ");
    ui_append_child(includes_window, includes_close);
    g_project.includes_modal = includes_modal;
    g_project.includes_window = includes_window;

    /* --- Find and Replace panel (Tools > "Find and Replace...") --- */
    ui_node* fr_wrapper = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, fr_wrapper);
    int fr_x = 10, fr_y = 3, fr_w = 32, fr_h = 16;
    ui_node* fr_panel = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(fr_panel, fr_x, fr_y, fr_w, fr_h);
    ui_set_label(fr_panel, " Find and Replace ");
    ui_set_color(fr_panel, theme->window_fg, theme->window_bg);
    ui_set_resizable(fr_panel, 1);  /* dock border can be dragged - its
                                     * controls' width tracks the panel's
                                     * current width (see g_fr.last_w/
                                     * FR_PANEL_MIN_W and app_frame() below),
                                     * clamped to a sane minimum rather than
                                     * left to just misalign or get crushed */
    ui_set_shadow(fr_panel, 0);
    ui_set_dock(fr_panel, UI_DOCK_RIGHT, fr_w);
    ui_append_child(fr_wrapper, fr_panel);
    g_fr.window = fr_wrapper;
    g_fr.panel = fr_panel;
    fr_rebuild_content();
    g_fr.last_w = fr_w;  /* matches what fr_rebuild_content() just laid out
                          * against - see the width-change check in app_frame() */
    /* Not shown at startup, unlike Folder/Output - it opens on demand from
     * Tools > "Find and Replace..." (EVT_TOOLS_FINDREPLACE) and its own
     * close icon just hides it again (see ui_set_dock's docs), same as any
     * other docked window. */

    /* ===== FIX: Ensure screen dimensions are initialized before showing docked windows ===== */
    ui_screen_update(g_screen, env);   /* reads env->width/height into g_screen->screen_w/h */

    /* Now show the docked windows – they will get correct sizes */
    ui_screen_show_window(g_screen, g_folder.window);
    ui_screen_show_window(g_screen, g_output_window);

    /* Global compiler settings (cake.json, beside the executable) - loaded
     * before the session so that a project reopened by load_session() below
     * starts from these, exactly as project_reset_data() intends. */
    global_settings_load();

    /* Compiler Options dialog: g_compile.options/g_compile.target loaded
     * just above already drive do_compile() directly; only the "Options"
     * text field needs an explicit push, since (unlike the Target <select>)
     * it isn't re-synced every time the dialog opens (see
     * EVT_COMPILE_OPTIONS) - it just keeps whatever was last typed into it
     * for the rest of the run, so it has to be seeded once, at startup.
     * Seeded here rather than in load_session(), which returns early when
     * there is no session file - cake.json can carry options with no session
     * alongside it. */
    if (g_copts.input)
        ui_set_value(g_copts.input, g_compile.options);

    /* Restore last session's Folder panel
     * directory, dock sizes, and active document (path/caret/scroll/
     * layout) - see load_session(). When it successfully reopens a real
     * file, the fallback demo window below is skipped entirely; a first
     * run (or a since-vanished file) falls through to the usual startup
     * demo exactly as before. */
    if (!load_session())
    {
        /* --- Demo/test file window (maximized) --- */
        ui_node* demo_wrapper = make_new_editor_window(root, g_new_count++);
        ui_screen_show_window(g_screen, demo_wrapper);
    }

    /* Apply the restored font choice now: load_session() has just supplied
     * g_envdlg.font_index, and the backend registered its shortlist before
     * app_init ran, so this is the first point where both are known. Also
     * re-syncs the dialog's <select>, which was populated (and defaulted to
     * row 0) before the session was read. */
    /* Tools restored by load_session() above only reach the menu here. */
    rebuild_tools_menu();

    if (g_envdlg.font_index > 0 && g_envdlg.font_index < ui_env_font_family_count(env))
    {
        ui_env_set_font_family(env, g_envdlg.font_index);
        if (g_envdlg.font_select)
            ui_select_set_selected(g_envdlg.font_select, g_envdlg.font_index);
    }

    ui_screen_set_on_event(g_screen, on_ui_event, NULL);
    ui_screen_set_completion_callback(g_screen, on_line_complete, NULL);
}

/* Command-line entry point - see ide_ui.h's own doc comment for the call
 * contract (once, right after app_init(), before the first app_frame()).
 * Only argv[1] (if present) is looked at today: a bare file path given on
 * the command line ("ide file.c") opens the same way File > Open would -
 * through open_file_path_into_editor(), so a bad path gets the same "File
 * not found" message box rather than silently doing nothing. It becomes the
 * frontmost window, shown over the startup demo file app_init() already
 * opened. Anything beyond a single path (flags, multiple files, ...) is left
 * for later - not asked for yet. */
void app_main(int argc, char** argv)
{
    if (argc < 2 || !argv || !argv[1] || !argv[1][0])
        return;

    open_file_path_into_editor(argv[1], basename_of(argv[1]));
}

/* Outside-change detection: every FILE_WATCH_INTERVAL_MS, and only while the
 * IDE has focus, stat() the open project's .cakeproj and the active editor
 * window's file - at most two stats per check. Other open windows are
 * checked once they become the active one. A changed file gets a Yes/No
 * "reload?" prompt (EVT_FILE_RELOAD/EVT_PROJECT_RELOAD). The stored time is
 * updated before asking, so "No" isn't asked again until the next change. */
#define FILE_WATCH_INTERVAL_MS 2000

static struct
{
    unsigned last_check_ms;
    char reload_path[1024];  /* the file the pending EVT_FILE_RELOAD is for */
} g_filewatch;

static void file_watch_reload_file(void)
{
    ui_node* w = find_open_window(g_filewatch.reload_path);
    ui_node* editor = w ? editor_in_window(w) : NULL;
    if (!editor)
        return;
    char* content = read_file_to_string(g_filewatch.reload_path);
    if (!content)
        return;
    normalize_newlines(content);

    int cursor = ui_editor_get_cursor(editor);
    int scroll = ui_editor_get_scroll(editor);
    ui_set_value(editor, content);
    free(content);

    int len = (int)strlen(ui_get_value(editor));
    if (cursor > len)
        cursor = len;
    ui_editor_set_selection(editor, cursor, cursor);
    ui_editor_set_scroll(editor, scroll);
    ui_set_dirty(editor, 0);
    ui_set_file_time(w, file_mtime(g_filewatch.reload_path));
}

static void file_watch_reload_project(void)
{
    char path[sizeof g_project.file_path];
    snprintf(path, sizeof path, "%s", g_project.file_path);
    if (project_load_from_file(path))
        project_window_refresh(0);
}

static void file_watch_check(ui_env* env)
{
    if (!ui_env_focused(env) || ui_screen_active_modal(g_screen))
        return;
    unsigned now = ui_env_time_ms(env);
    if (now - g_filewatch.last_check_ms < FILE_WATCH_INTERVAL_MS)
        return;
    g_filewatch.last_check_ms = now;

    if (project_is_open() && g_project.file_time != 0)
    {
        long long t = file_mtime(g_project.file_path);
        if (t != 0 && t != g_project.file_time)
        {
            g_project.file_time = t;
            char msg[1400];
            snprintf(msg, sizeof msg,
                     "The project file was modified outside the IDE:\n%s\n\nReload it?",
                     g_project.file_path);
            ui_msgbox_button btns[] = {
                { "  Yes  ", EVT_PROJECT_RELOAD },
                { "  No  ", 0 },
            };
            ui_message_box(g_screen, "Project Changed", msg, btns, 2);
            return;  /* one prompt at a time */
        }
    }

    ui_node* w = g_active_editor_window;
    const char* path = w ? ui_get_path(w) : "";
    if (w && path[0] && ui_get_file_time(w) != 0)
    {
        long long t = file_mtime(path);
        if (t != 0 && t != ui_get_file_time(w))
        {
            ui_set_file_time(w, t);
            snprintf(g_filewatch.reload_path, sizeof g_filewatch.reload_path, "%s", path);
            ui_node* editor = editor_in_window(w);
            char msg[1400];
            snprintf(msg, sizeof msg,
                     "The file was modified outside the IDE:\n%s\n\n%sReload it?",
                     path,
                     editor && ui_get_dirty(editor) ? "Your unsaved changes will be lost.\n" : "");
            ui_msgbox_button btns[] = {
                { "  Yes  ", EVT_FILE_RELOAD },
                { "  No  ", 0 },
            };
            ui_message_box(g_screen, "File Changed", msg, btns, 2);
        }
    }
}

int app_frame(ui_env* env)
{
    ui_screen_update(g_screen, env);  /* delivers events to on_ui_event() as they fire */
    if (g_quit)
    {
        app_shutdown();
        return 1;
    }

    /* A document/editor window's close icon (or the various File > Open/
     * Save As/Find-and-Replace-Change-All paths that reuse the same "one
     * file, one window" shape) just closed a transient window this update -
     * tear it down for real now, rather than leaking it forever like every
     * reusable singleton window (Folder/Output/Find & Replace, About,
     * Environment, ...) correctly keeps doing (see ui_set_transient's own
     * doc comment). Done first thing, before anything below reads
     * g_active_editor_window or walks g_root, so nothing this frame can ever
     * see - let alone dereference - a pointer to the now-freed node. */
    {
        ui_node* closed = ui_screen_take_closed_window(g_screen);
        if (closed)
        {
            if (closed == g_active_editor_window)
                g_active_editor_window = NULL;
            ui_remove_child(g_root, closed);
            ui_node_free(closed);
        }
    }

    /* Find and Replace panel: notice a dock-border drag (the framework only
     * shifts the existing INPUT/GROUP/SELECT/BUTTON children's position on a
     * window resize, never their width - see set_window_rect in ide_ui.c,
     * which only auto-stretches EDITOR/LISTBOX children) and re-lay-out the
     * panel's controls to the new width via fr_rebuild_content(), clamping
     * first so the border can't be dragged narrower than FR_PANEL_MIN_W. */
    if (g_fr.panel)
    {
        int pw;
        ui_get_rect(g_fr.panel, NULL, NULL, &pw, NULL);
        if (pw < FR_PANEL_MIN_W)
        {
            int px, py, ph;
            ui_get_rect(g_fr.panel, &px, &py, NULL, &ph);
            ui_set_rect(g_fr.panel, px, py, FR_PANEL_MIN_W, ph);
            pw = FR_PANEL_MIN_W;
        }
        if (pw != g_fr.last_w)
        {
            g_fr.last_w = pw;
            fr_rebuild_content();
        }
    }

    output_diagnostic_help_refresh();
    cmdline_layout();
    cmdline_take_focus();
    git_diff_counter_refresh();

    /* Track the frontmost real document window (see g_active_editor_window)
     * - only updated when an actual editor window is frontmost, so it keeps
     * pointing at the last one even while a docked Folder/Output panel (or
     * a modal) is briefly on top instead. */
    {
        ui_node* top = ui_screen_top_window(g_screen);
        if (is_editor_window(top))
            g_active_editor_window = top;
    }

    /* The menubar opens its dropdowns itself, with no app-level callback,
     * so the View menu's
     * "Show ..." items get their "[x]"/"[ ]" kept current every frame too.
     * Output/Folder are persistent singletons (see window_is_shown's own
     * doc comment); Playground has no such singleton pointer (a normal
     * transient document window - see open_playground()), so it's looked up
     * by its fixed path instead, the same way open_file_path_into_editor()
     * itself checks for an already-open Playground before creating a new
     * one. */
    ui_set_label(g_view_output_item, "Output");
    ui_set_label(g_view_folder_item, "Folder");
    ui_set_label(g_view_git_item, "Git Changes");
    ui_set_label(g_view_debuginfo_item, "Debug Info");
    ui_set_label(g_project.view_item, "Project");
    /* Same "needs an open project" rule as the Project menu's own items just
     * below - there's nothing to show/hide without one. */
    ui_set_enabled(g_project.view_item, project_is_open());
    /* Project > Add Existing File.../Include Directories.../Build/Save
     * Project/Close Project - disabled while no project is open, same as
     * every other "only meaningful in state X" menu item in this app (e.g.
     * g_compile_item). New/Open Project are left out of
     * menu_items_requiring_project entirely - those are always enabled since
     * they're how a project gets opened in the first place. */
    {
        int open = project_is_open();
        for (int i = 0; i < (int)(sizeof g_project.menu_items_requiring_project /
                                   sizeof g_project.menu_items_requiring_project[0]); i++)
            ui_set_enabled(g_project.menu_items_requiring_project[i], open);
    }
    ui_set_label(g_view_playground_item, "Playground");

    /* Same reasoning again - "Compile" (both the menu entry and its F7
     * shortcut, since they're the same node - see g_compile_item's own doc
     * comment) is only reachable while a real .c file is frontmost (see
     * path_is_c_source's own doc comment for why .h doesn't count either);
     * with no document open at all (g_active_editor_window NULL, e.g. only
     * Folder/Output panels visible) there's nothing to compile, so it's
     * disabled the same as for a .md. */
    int compile_targets_c = g_active_editor_window != NULL &&
        path_is_c_source(ui_get_path(g_active_editor_window));
    /* "Build" is the project build whenever a project is open (see
     * do_build()), so it needs no frontmost .c then - only without a
     * project does it fall back to "Compile" and share its condition. */
    ui_set_enabled(g_compile_item, project_is_open() || compile_targets_c);
    ui_set_enabled(g_compile_file_item, compile_targets_c);
    /* Same condition - the Compile menu's own "Show Generated Code" (not
     * the popup's copy, which refreshes itself separately - see
     * g_compile_show_output_item's own doc comment). */
    ui_set_enabled(g_compile_show_output_item, compile_targets_c);

    /* "Format" (Edit menu and the editor popup's copy) only makes sense for
     * a real .c file - same path_is_c_source() condition as Compile just
     * above, since cake_format() assumes C syntax. */
    ui_set_enabled(g_edit_format_item, compile_targets_c);
    ui_set_enabled(g_editor_popup_format, compile_targets_c);

    /* The popup's own "Compile" - same .c-only rule as the Compile menu's
     * copy above, kept here so both refresh from the one condition. */
    ui_set_enabled(g_editor_popup_compile, compile_targets_c);

    git_clone_update_suggestion();

    /* Apply a Go-to-line focus request now that update() (and its
     * fire-then-blur of the input) is done, so the editor caret stays put. */
    if (g_goto_pending_focus)
    {
        ui_screen_focus(g_screen, g_goto_pending_focus);
        g_goto_pending_focus = NULL;
    }

    /* Right-click over the active editor opens its context menu at the
     * cursor - no modal must be blocking. */
    if (ui_screen_mouse_right_pressed(g_screen) && !ui_screen_active_modal(g_screen))
    {
        int mx = ui_screen_mouse_x(g_screen), my = ui_screen_mouse_y(g_screen);
        ui_node* win = ui_screen_top_window(g_screen);
        ui_node* ed = is_editor_window(win) ? editor_in_window(win) : NULL;
        if (ui_node_contains(ed, mx, my))
        {
            /* Refresh "Read-only" for this editor - checked when it already
             * is one, and disabled for VT100 (e.g. the Output window), which
             * is always read-only and never meant to be toggled back. */
            refresh_readonly_item(g_editor_popup_readonly, win);

            /* Refresh "[x] Line Numbers" - enabled only for a C source
             * editor, the only kind that draws a line-number gutter. */
            refresh_view_item(g_view_linenumbers_item, "Line Numbers", ui_get_show_line_numbers());
            ui_set_enabled(g_view_linenumbers_item, ui_get_syntax(ed) == UI_SYNTAX_C);

            /* Refresh "Toggle Header/Source" - enabled only for a .c/.h file. */
            char cp[1024];
            int has_counterpart = header_source_counterpart(ui_get_path(win), cp, sizeof cp);
            ui_set_enabled(g_editor_popup_hdrsrc, has_counterpart);

            /* Refresh "Show Generated Code" - disabled for a .md file, same
             * idea as Toggle Header/Source above: Markdown is never
             * compiled, so there's no generated code to show. */
            ui_set_enabled(g_editor_popup_show_output,
                           syntax_for_path(ui_get_path(win)) != UI_SYNTAX_MARKDOWN);

            /* Refresh "Copy Code Block"/"Copy to Playground" - enabled only
             * when this click landed on/in one of this Markdown document's
             * fenced code blocks. */
            refresh_codeblock_items(g_editor_popup_codeblock_copy,
                                     g_editor_popup_codeblock_playground, win, mx, my);

            ui_screen_open_popup(g_screen, g_editor_popup, mx, my, win);
        }
    }

    /* Right-click over the Folder panel's listbox opens its own popup at
     * the cursor - no
     * modal must be blocking. Independent of the editor popup check above: when the
     * Folder panel is frontmost, ui_screen_top_window() returns it (not an
     * editor window), so `ed` there is NULL and that block's
     * ui_node_contains(NULL, ...) already no-ops - see its own doc comment. */
    if (ui_screen_mouse_right_pressed(g_screen) && !ui_screen_active_modal(g_screen))
    {
        int mx = ui_screen_mouse_x(g_screen), my = ui_screen_mouse_y(g_screen);
        /* window_is_shown() guards against g_folder.listbox's stale rect
         * still overlapping the Project panel's current spot when Folder
         * itself isn't actually the one docked/visible there right now -
         * without it, a right-click meant for the Project panel could hit
         * this block too (both only ever occupy the same LEFT dock slot one
         * at a time - see g_project.window's own doc comment) and its popup
         * would open only to be immediately clobbered by the Project block
         * below opening its own right after. */
        if (window_is_shown(g_folder.window) && ui_node_contains(g_folder.listbox, mx, my))
        {
            ui_set_enabled(g_folder.popup_add_to_project, project_is_open());
            ui_screen_open_popup(g_screen, g_folder.popup, mx, my, NULL);
        }
    }

    /* Right-click over the Project panel's listbox opens its own popup
     * ("Open" / "New File..." / "Remove from Project") - same shape as the Folder panel's
     * block just above, including the same window_is_shown() guard. */
    if (ui_screen_mouse_right_pressed(g_screen) && !ui_screen_active_modal(g_screen))
    {
        int mx = ui_screen_mouse_x(g_screen), my = ui_screen_mouse_y(g_screen);
        if (window_is_shown(g_project.window) && ui_node_contains(g_project.listbox, mx, my))
            ui_screen_open_popup(g_screen, g_project.popup, mx, my, NULL);
    }

    /* Right-click over the Git Changes panel's listbox opens its own popup
     * (Commit/Discard/Pull/Push) - same shape as the Folder/Project blocks
     * just above, including the same window_is_shown() guard. */
    if (ui_screen_mouse_right_pressed(g_screen) && !ui_screen_active_modal(g_screen))
    {
        int mx = ui_screen_mouse_x(g_screen), my = ui_screen_mouse_y(g_screen);
        if (window_is_shown(g_git.window) && ui_node_contains(g_git.listbox, mx, my))
        {
            git_popup_refresh();
            ui_screen_open_popup(g_screen, g_git.popup, mx, my, NULL);
        }
    }

    /* Right-click over the Git Diff viewer's text opens its own popup (Copy Full Path/Show My Folder). */
    if (ui_screen_mouse_right_pressed(g_screen) && !ui_screen_active_modal(g_screen))
    {
        int mx = ui_screen_mouse_x(g_screen), my = ui_screen_mouse_y(g_screen);
        if (ui_screen_top_window(g_screen) == g_gitdiff_window && ui_node_contains(g_gitdiff_editor, mx, my))
            ui_screen_open_popup(g_screen, g_gitdiff_popup, mx, my, NULL);
    }

    /* Right-click on a docked panel's frame opens the "Dock Left/Right/
     * Bottom" popup for that panel (see g_dockmenu/docked_panel_frame_at).
     * The side it is already on is marked, same "[x] Label" convention the
     * View menu uses. */
    if (ui_screen_mouse_right_pressed(g_screen) && !ui_screen_active_modal(g_screen))
    {
        int mx = ui_screen_mouse_x(g_screen), my = ui_screen_mouse_y(g_screen);
        ui_node* panel = docked_panel_frame_at(mx, my);
        if (panel)
        {
            static const char* dock_labels[3] = { "Dock Left", "Dock Right", "Dock Bottom" };
            ui_dock_side current = ui_get_dock(panel);
            for (int i = 0; i < 3; i++)
                refresh_view_item(g_dockmenu.items[i], dock_labels[i], current == (ui_dock_side)(i + 1));

            g_dockmenu.target = panel;
            ui_screen_open_popup(g_screen, g_dockmenu.popup, mx, my, NULL);
        }
    }

    /* External Tools dialog: follow the listbox selection as it moves.
     * A single click only sets <listbox>'s selection - the id event fires
     * on double-click/Enter (see process_window's UI_TAG_LISTBOX branch),
     * which is what the Open dialog wants but not this one: here picking a
     * row should immediately show that tool's fields. Polling the selection
     * while the modal is up gets that without changing the shared widget's
     * behavior for every other dialog. */
    if (g_exttool.modal && ui_screen_active_modal(g_screen) == g_exttool.modal)
    {
        int sel = ui_select_get_selected(g_exttool.listbox);
        if (sel != g_exttool.sel)
        {
            /* Commit whatever is in the fields to the row being LEFT before
             * switching, so edits are never silently lost by clicking away.
             * Order matters: g_exttool.sel must advance before the refresh,
             * because exttool_refresh_list() re-asserts the listbox
             * selection from it and would otherwise snap back to the old
             * row. The refresh is what makes a retitled tool show its new
             * name in the list straight away. */
            exttool_store_fields();
            g_exttool.sel = sel;
            exttool_refresh_list();
            exttool_load_fields();
        }
    }

    /* Stream whatever the running compile has printed since the last frame
     * (no-op when none is running) - see compile_stream_poll(). Kept here,
     * at the end of the frame, so the Output window it writes into is
     * updated before the next render rather than a frame later. */
    compile_stream_poll();

    /* Same, for a running git command - see git_job_poll(). */
    git_job_poll();

    /* Same reasoning, for a live debug session (see ide_debug.h) - drains
     * lldb's output, updates the exec-line highlight, and keeps the Debug
     * menu's enabled state current. */
    debug_stream_poll();

    file_watch_check(env);

    return 0;
}

int app_render(int* x, int* y, int* w, int* h)
{
    return ui_screen_render(g_screen, x, y, w, h);
}

void app_invalidate(void)
{
    /* g_screen may not exist yet when a backend (re)creates its bitmap during
     * window setup, before app_init - the first render repaints fully anyway. */
    if (g_screen)
        ui_screen_invalidate(g_screen);
}
