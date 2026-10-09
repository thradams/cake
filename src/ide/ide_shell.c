/* ide_shell.c - the Cake IDE 2, an app of the ide GUI framework.
 *
 * Builds the old IDE's screen - same menus, panels and dialogs - and runs
 * the commands that do not need the compiler yet: opening and saving
 * files, the Folder panel, the Edit commands, windows, themes, zoom, Go to
 * Line. Everything else says so in the statusbar until Cake is wired in
 * (see GUI_IDE_SPEC.md, section 11).
 */
#include "ide_shell.h"
#include "../version.h"
#include "../json.h"
#include "../target.h"
#include "../compile.h"
#include "../parser.h"
#include "ide_debugger.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <strings.h>
#define _stricmp strcasecmp
#endif

/* Every event the IDE handles. Menu items get theirs from the menu table. */
enum ide_event
{
    EV_NONE,
    /* File */
    EV_NEW_FILE, EV_OPEN, EV_OPEN_FOLDER, EV_GIT_CLONE, EV_SAVE, EV_SAVE_AS, EV_SAVE_ALL,
    EV_EXIT,
    /* Edit */
    EV_UNDO, EV_REDO, EV_CUT, EV_COPY, EV_PASTE, EV_STRINGIFY, EV_TO_UPPER, EV_TO_LOWER,
    EV_WORD_WRAP, EV_FORMAT, EV_COMPLETE,
    /* View */
    EV_VIEW_OUTPUT, EV_VIEW_FIND_RESULTS, EV_VIEW_FOLDER, EV_VIEW_PROJECT, EV_VIEW_GIT,
    EV_VIEW_PLAYGROUND,
    /* Search */
    EV_FIND, EV_REPLACE, EV_SEARCH_NEXT, EV_GOTO_LINE, EV_FIND_DECLARATION,
    EV_FIND_DEFINITION, EV_FIND_USAGES, EV_RENAME, EV_FIND_IN_FILES,
    /* Project */
    EV_PROJECT_NEW, EV_PROJECT_OPEN, EV_PROJECT_ADD_FILE,
    EV_MENU_RECENT,   /* submenu parent: fires nothing */
    EV_PROJECT_OPTIONS, EV_PROJECT_REPORT_UNUSED, EV_PROJECT_CLOSE, EV_PROJECT_RENAME,
    /* Build */
    EV_BUILD, EV_REBUILD, EV_COMPILE, EV_SHOW_GENERATED,
    /* Debug */
    EV_DEBUG_START, EV_DEBUG_STOP, EV_DEBUG_CONTINUE, EV_DEBUG_STEP_OVER, EV_DEBUG_STEP_INTO, EV_DEBUG_STEP_OUT,
    EV_DEBUG_BREAKPOINT, EV_DEBUG_INFO,
    /* Tools */
    EV_TERMINAL, EV_EXTERNAL_TOOLS,
    /* Window */
    EV_TILE, EV_CASCADE, EV_CLOSE_ALL, EV_ENVIRONMENT, EV_FONT_BIGGER, EV_FONT_SMALLER,
    /* Help */
    EV_MANUAL, EV_WEBSITE, EV_ABOUT,
    /* Statusbar */
    EV_HELP, EV_BACK, EV_FORWARD,
    /* Panels and dialogs */
    EV_FOLDER_OPEN, EV_GOTO_OK, EV_GOTO_CANCEL, EV_ENV_OK, EV_ABOUT_OK,
    EV_OPEN_LIST, EV_OPEN_OK, EV_OPEN_CANCEL,
    EV_OPEN_FILTER,                       /* + the filter's index */
    EV_NEWFILE_OK = EV_OPEN_FILTER + 16, EV_NEWFILE_CANCEL, EV_NEWFILE_BROWSE,
    EV_WRAP_OK, EV_WRAP_CANCEL,
    EV_FIND_OK, EV_FIND_CANCEL,
    EV_REPLACE_OK, EV_REPLACE_ALL, EV_REPLACE_CANCEL,
    /* Context menus */
    EV_EDIT_STRING, EV_TOGGLE_HDRSRC, EV_COPY_PATH, EV_SHOW_FOLDER,
    EV_FOLDER_COPY_PATH, EV_FOLDER_NEW_FOLDER, EV_FOLDER_DELETE,
    /* More dialogs */
    EV_RENAME_OK, EV_RENAME_CANCEL, EV_ESTR_OK, EV_ESTR_CANCEL,
    EV_NEWFOLDER_OK, EV_NEWFOLDER_CANCEL,
    EV_CLONE_OK, EV_CLONE_CANCEL, EV_CLONE_BROWSE, EV_CLONE_URL_CHANGED, EV_CLONE_PATH_CHANGED,
    EV_NEWPROJ_OK, EV_NEWPROJ_CANCEL, EV_NEWPROJ_BROWSE,
    EV_COPTS_OK, EV_COPTS_CANCEL, EV_COPTS_HELP, EV_COPTS_CONFIG,
    EV_COPTS_AUTO_CONFIG, EV_COPTS_INC_ADD, EV_COPTS_INC_REMOVE, EV_COPTS_INC_UP, EV_COPTS_INC_DOWN,
    EV_DBG_BROWSE, EV_POST_BUILD_BROWSE, EV_COPTS_PAGE,
    EV_COPTS_CONFIG_NEW, EV_COPTS_CONFIG_RENAME, EV_COPTS_CONFIG_DELETE, EV_COPTS_CONFIG_DELETE_YES, EV_CONFIG_NAME_OK, EV_CONFIG_NAME_CANCEL,
    EV_EXT_LIST, EV_EXT_ADD, EV_EXT_DELETE, EV_EXT_UP, EV_EXT_DOWN, EV_EXT_OK, EV_EXT_CANCEL,
    EV_EXT_BROWSE,
    EV_HELP_CLOSE, EV_HELP_BACK,
    EV_FOLDER_DELETE_OK,
    EV_TICK,                              /* the timer: 50 ms while compiling, else 2 s */
    EV_FILE_RELOAD, EV_PROJECT_RELOAD,    /* "Yes" in the changed-outside prompts */
    EV_OUTPUT_DBLCLICK,
    EV_DOC_CLOSE,                         /* a document's close icon */
    EV_CLOSE_DISCARD,
    EV_EXIT_SAVE, EV_EXIT_DISCARD,
    EV_PROJECT_LIST, EV_PROJ_COPY_PATH, EV_PROJ_NEW_FILE, EV_PROJ_REMOVE, EV_PROJ_DELETE,
    EV_PROJ_DELETE_OK, EV_FOLDER_ADD_TO_PROJECT,
    EV_PROJ_RENAME_OK, EV_PROJ_RENAME_CANCEL,
    EV_FR_DBLCLICK, EV_FR_CLEAR,
    EV_NEWFILE_OVERWRITE, EV_SAVEAS_OVERWRITE, EV_OPEN_LINK,
    EV_EDITOR_MENU, EV_TOGGLE_READONLY, EV_TOGGLE_DETACH,

    EV_FR_TAB_FIND, EV_FR_TAB_REPLACE, EV_FR_FIND, EV_FR_REPLACE,
    EV_EDITOR_CTRLCLICK, EV_HELP_CTRLCLICK,
    EV_CMDLINE, EV_COPTS_KEEP_INVALID,
    EV_GIT_LIST, EV_GIT_REFRESH, EV_GIT_COMMIT, EV_GIT_COMMIT_PUSH, EV_GIT_COMMIT_FILE,
    EV_GIT_COMMIT_STAGED, EV_GIT_COMMIT_STAGED_PUSH, EV_GIT_STAGE, EV_GIT_UNSTAGE, EV_GIT_DISCARD, EV_GIT_DISCARD_OK,
    EV_GIT_DISCARD_ALL, EV_GIT_DISCARD_ALL_OK, EV_GIT_IGNORE_FILE, EV_GIT_IGNORE_EXT, EV_GIT_IGNORE_FOLDER, EV_GIT_PULL, EV_GIT_PUSH, EV_GIT_SYNC,
    EV_GIT_BRANCH, EV_GITCOMMIT_OK, EV_GITCOMMIT_CANCEL,
    EV_GITBRANCH_CHECKOUT, EV_GITBRANCH_NEW, EV_GITBRANCH_CANCEL,
    EV_GITDIFF_PREV, EV_GITDIFF_NEXT, EV_GITDIFF_EDIT, EV_GITDIFF_COPY_PATH, EV_GITDIFF_SHOW_FOLDER,
    EV_COMPLETE_ITEM,                     /* + the candidate's index, up to 30 */
    EV_COMPLETE_ITEM_LAST = EV_COMPLETE_ITEM + 29,
    EV_ENV_THEME,                         /* + the theme's index, up to 8 */
    EV_ENV_THEME_LAST = EV_ENV_THEME + 7,
    EV_ENV_FONT,                          /* + the font's index, up to 16 */
    EV_ENV_FONT_LAST = EV_ENV_FONT + 15,
    EV_ENV_UI_FONT,                       /* + the font's index, up to 16 */
    EV_ENV_UI_FONT_LAST = EV_ENV_UI_FONT + 15,
    EV_ENV_EDITOR_SIZE,                   /* + 0 smaller, 1 normal, 2 larger */
    EV_ENV_EDITOR_SIZE_LAST = EV_ENV_EDITOR_SIZE + 2,
    EV_OUTPUT_COPY_ALL, EV_OUTPUT_SELECT_ALL, EV_OUTPUT_CLEAR,
    EV_MACRO,                             /* + the button's index in macro_buttons */
    EV_MACRO_ITEM = EV_MACRO + 16,        /* + the macro's index in macros[] */
    EV_TOOL_RUN = EV_MACRO_ITEM + 32,     /* + the tool's index (MAX_EXT_TOOLS) */
    EV_CONFIG_ITEM = EV_TOOL_RUN + 32,    /* + the configuration's row */
    EV_RECENT_ITEM = EV_CONFIG_ITEM + MAX_CONFIGURATIONS,   /* + the row in the recent projects */
    EV_COUNT = EV_RECENT_ITEM + 10
};

/* --- The menus: the old IDE's build_screen(), same items and shortcuts --- */

struct menu_item
{
    int id;             /* EV_NONE: a separator */
    const char* label;
    const char* shortcut;
    int enabled;
    const char* hint;
};

struct menu
{
    const char* title;
    const struct menu_item* items;
    int count;
};

#define COUNT(a) ((int)(sizeof(a) / sizeof((a)[0])))
#if defined(__CAKE__) || __STDC_VERSION__ >= 202311L
#define FALLTHROUGH [[fallthrough]]
#define NODISCARD [[nodiscard]]
#else
#define FALLTHROUGH ((void)0)
#define NODISCARD
#endif
#define SEPARATOR { EV_NONE, NULL, NULL, 1, NULL }

static const struct menu_item file_items[] = {
    { EV_NEW_FILE, "New File...", NULL, 1, "Create a new file" },
    { EV_OPEN, "Open File...", "Ctrl+O", 1, "Open a file" },
    { EV_OPEN_FOLDER, "Open Folder...", NULL, 1, "Show a folder in the Folder panel" },
    { EV_GIT_CLONE, "Clone Repository...", NULL, 1, "Copy a remote Git repository to a local folder" },
    SEPARATOR,
    { EV_SAVE, "Save", "Ctrl+S", 1, "Save the active file" },
    { EV_SAVE_AS, "Save As...", NULL, 1, "Save the active file under a new name" },
    { EV_SAVE_ALL, "Save all", "Ctrl+Shift+S", 1, "Save every modified file" },
    SEPARATOR,
    { EV_MENU_RECENT, "Recent Projects", NULL, 1, "The projects opened last; click one to open it" },
    SEPARATOR,
    { EV_EXIT, "Exit", NULL, 1, "Close the IDE" },
};
static const struct menu_item edit_items[] = {
    { EV_UNDO, "Undo", "Ctrl+Z", 1, "Undo the last edit" },
    { EV_REDO, "Redo", "Ctrl+Y", 1, "Redo the last undone edit" },
    SEPARATOR,
    { EV_CUT, "Cut", "Ctrl+X", 1, "Move the selection to the clipboard" },
    { EV_COPY, "Copy", "Ctrl+C", 1, "Copy the selection to the clipboard" },
    { EV_PASTE, "Paste", "Ctrl+V", 1, "Insert the clipboard at the caret" },
    SEPARATOR,
    { EV_STRINGIFY, "Stringify", NULL, 1, "Turn the selection into a C string literal" },
    { EV_TO_UPPER, "To Upper", "Ctrl+U", 1, "Convert the selection to upper case" },
    { EV_TO_LOWER, "To Lower", "Ctrl+L", 1, "Convert the selection to lower case" },
    SEPARATOR,
    { EV_WORD_WRAP, "Word Wrap...", "Ctrl+W", 1, "Wrap the selected text at a column" },
    SEPARATOR,
    { EV_FORMAT, "Format", "Ctrl+Shift+F", 1, "Format the active file's code" },
    { EV_COMPLETE, "Auto-complete", "Ctrl+Space", 1, "Complete the word at the caret" },
    { EV_RENAME, "Rename...", "F2", 1, "Rename the identifier at the caret everywhere it is used" },
};
static const struct menu_item view_items[] = {
    { EV_VIEW_PROJECT, "Project", NULL, 1, "Show the Project panel" },
    { EV_VIEW_FOLDER, "Folder", NULL, 1, "Show the Folder panel" },
    { EV_VIEW_GIT, "Git Changes", NULL, 1, "Show the Git Changes panel" },
    SEPARATOR,
    { EV_VIEW_PLAYGROUND, "Playground", NULL, 1, "Show the Playground" },
    SEPARATOR,
    { EV_VIEW_OUTPUT, "Output", NULL, 1, "Show the Output panel" },
    { EV_VIEW_FIND_RESULTS, "Find Results", NULL, 1, "Show the Find Results panel" },
};
static const struct menu_item search_items[] = {
    { EV_FIND, "Find...", NULL, 1, "Find text in the active file" },
    { EV_REPLACE, "Replace...", "Ctrl+R", 1, "Find and replace text in the active file" },
    { EV_SEARCH_NEXT, "Search Next", "F3", 1, "Repeat the last search" },
    SEPARATOR,
    { EV_GOTO_LINE, "Go to line...", "Ctrl+G", 1, "Move the caret to a line number" },
    { EV_FIND_DECLARATION, "Find Declaration", NULL, 1, "Go to the declaration of the identifier at the caret" },
    { EV_FIND_DEFINITION, "Find Definition", "F12", 1, "Go to the definition of the identifier at the caret" },
    { EV_FIND_USAGES, "Find Usages", NULL, 1, "List the uses of the identifier at the caret in Find Results" },
    SEPARATOR,
    { EV_FIND_IN_FILES, "Find in Files...", "Ctrl+F", 1, "Search text in many files; the result in Find Results" },
};
static const struct menu_item project_items[] = {
    { EV_PROJECT_NEW, "New Project...", NULL, 1, "Create a Cake project (.cakeproj), optionally with a main.c" },
    { EV_PROJECT_OPEN, "Open Project...", NULL, 1, "Open a .cakeproj project" },
    SEPARATOR,
    { EV_PROJECT_ADD_FILE, "Add Existing File...", NULL, 1, "Add files to the open project" },
    SEPARATOR,
    { EV_PROJECT_RENAME, "Rename Project...", NULL, 1, "Rename the open project's .cakeproj file" },
    { EV_PROJECT_CLOSE, "Close Project", NULL, 1, "Close the project and its documents without unsaved changes" },
    SEPARATOR,
    { EV_PROJECT_OPTIONS, "Properties...", NULL, 1, "The open project's options - with no project, the playground's" },
};
static const struct menu_item build_items[] = {
    { EV_BUILD, "Build", "F7", 1, "Compile the project's .c files modified since the last Build (or the current file)" },
    { EV_REBUILD, "Rebuild", NULL, 1, "Compile every .c file of the project, modified or not" },
    SEPARATOR,
    { EV_COMPILE, "Compile", "Ctrl+F7", 1, "Compile only the current file" },
    { EV_SHOW_GENERATED, "Show Generated Code", NULL, 1, "Open the C89 code Cake generated for the current file" },
    SEPARATOR,
    { EV_PROJECT_REPORT_UNUSED, "Report Unused", NULL, 1, "List the project's non-static functions never called in any of its files, in Find Results (-unused-extern-report)" },
};
static const struct menu_item debug_items[] = {
    { EV_DEBUG_START, "Start Debugging", "F5", 1, "Build (Pre-Build Event, Cake, Post-Build Event) and, without errors, debug the program" },
    { EV_DEBUG_STOP, "Stop Debugging", "Shift+F5", 0, "End the debugging session and the program" },
    SEPARATOR,
    { EV_DEBUG_CONTINUE, "Continue", "F5", 0, "Run until the next breakpoint or the end of the program" },
    { EV_DEBUG_STEP_OVER, "Step Over", "F10", 0, "Run the current line, calls included, and stop at the next one" },
    { EV_DEBUG_STEP_INTO, "Step Into", "F11", 0, "Run the current line, stopping inside the function it calls" },
    { EV_DEBUG_STEP_OUT, "Step Out", "Shift+F11", 0, "Run until the current function returns" },
    SEPARATOR,
    { EV_DEBUG_BREAKPOINT, "Toggle Breakpoint", "F9", 1, "Set or clear a breakpoint on the caret's line (or click its line number)" },
    SEPARATOR,
    { EV_DEBUG_INFO, "Debug Info", NULL, 1, "Show the Locals and Call Stack of the stopped program" },
};
static const struct menu_item tools_items[] = {
    { EV_TERMINAL, "Terminal", NULL, 1, "Open a terminal in the active file's folder (else the Folder panel's)" },
    { EV_EXTERNAL_TOOLS, "External Tools...", NULL, 1, "Add, edit and order the programs this menu runs - compilers, scripts" },
};
static const struct menu_item window_items[] = {
    { EV_TILE, "Tile", NULL, 1, "Arrange the open windows side by side" },
    { EV_CASCADE, "Cascade", NULL, 1, "Arrange the open windows overlapping" },
    { EV_CLOSE_ALL, "Close all", NULL, 1, "Close all open documents" },
    SEPARATOR,
    { EV_FONT_BIGGER, "Font", "Ctrl++", 1, "Make the font bigger" },
    { EV_FONT_SMALLER, "Font", "Ctrl+-", 1, "Make the font smaller" },
    SEPARATOR,
    { EV_ENVIRONMENT, "Change Theme...", NULL, 1, "Choose the colors of the IDE" },
};
static const struct menu_item help_items[] = {
    { EV_MANUAL, "Quick Reference", "F1", 1, "Open the Cake quick reference" },
    SEPARATOR,
    { EV_WEBSITE, "Cake Website", NULL, 1, "Open the Cake website in the browser" },
    SEPARATOR,
    { EV_ABOUT, "About...", NULL, 1, "Show the Cake version and credits" },
};

static const struct menu menus[] = {
    { "File", file_items, COUNT(file_items) },
    { "Edit", edit_items, COUNT(edit_items) },
    { "View", view_items, COUNT(view_items) },
    { "Search", search_items, COUNT(search_items) },
    { "Project", project_items, COUNT(project_items) },
    { "Build", build_items, COUNT(build_items) },
    { "Debug", debug_items, COUNT(debug_items) },
    { "Tools", tools_items, COUNT(tools_items) },
    { "Window", window_items, COUNT(window_items) },
    { "Help", help_items, COUNT(help_items) },
};

/* --- State --- */

#define MAX_DOCS 64
#define MAX_FOLDER_ENTRIES 2048

/* An open file: its window and editor. */
struct sent_breakpoint
{
    char file[260];
    int line;
    int id;
};

struct doc
{
    struct gui_node* window;
    struct gui_node* editor;
    char path[1024];
    int crlf;          /* the file had "\r\n" line ends; saved back the same way */
    long long file_time;   /* the file's time when we last read or wrote it */
};

struct folder_panel
{
    struct gui_node* window;
    struct gui_node* list;
    char dir[1024];
    struct ide_dir_entry entries[MAX_FOLDER_ENTRIES];
    int count;         /* entries shown after the ".." row */
    char pending_delete[1400];   /* what the Delete confirmation is about */
    int pending_is_dir;
};

struct output_panel
{
    struct gui_node* window;
    struct gui_node* editor;
    struct gui_node* prompt;   /* the command line under it: ">" or "stdin>" */
    struct gui_node* input;
    int to_stdin;              /* the prompt shown is "stdin>" */
};

/* Find Results: shares the bottom dock with Output; each search goes above
 * the previous ones. */
/* Back/Forward (Alt+Left/Right): where a jump left from - the old IDE's g_nav. */
#define NAV_STACK_MAX 64

struct nav_pos
{
    char path[1024];
    int caret;
};

struct nav_stack
{
    struct nav_pos items[NAV_STACK_MAX];
    int count;
};

struct navigation
{
    struct nav_stack back, forward;
    int restoring;   /* a Back/Forward jump records nothing */
};

struct find_results
{
    struct gui_node* window;
    struct gui_node* editor;
    struct gui_node* menu;
    char* previous;   /* the panel's text when this search began */
};

/* Search > Find in Files: the Find and Replace panel, docked right - the
 * old IDE's. Its content is rebuilt when the mode (tab) changes; the
 * settings live here between rebuilds. */
enum look_in
{
    LOOK_IN_FILE,
    LOOK_IN_DIR,
    LOOK_IN_INCLUDE_DIRS,
    LOOK_IN_PROJECT,
};

/* The Find and Replace buttons, 2 + 11 + 1 + 11 + 2: the panel's minimum
 * width, and the width it first opens at. */
#define FIF_MIN_COLS 27

struct find_in_files
{
    struct gui_node* window;
    struct gui_node* find;
    struct gui_node* replace;   /* NULL in Find mode */
    struct gui_node* options;
    struct gui_node* look_in;
    struct gui_node* file_types;
    int mode;                   /* 0 Find, 1 Replace */
    char find_text[256];
    char replace_text[256];
    int match_case, whole_word;
    int look, file_type;
};

enum find_kind
{
    FIND_NONE = -1,
    FIND_DEFINITION,
    FIND_DECLARATION,
    FIND_USAGES,
    FIND_UNUSED,     /* Build > Report Unused */
};

struct goto_dialog
{
    struct gui_node* window;
    struct gui_node* input;
};

struct env_dialog
{
    struct gui_node* window;
    struct gui_node* theme;
    struct gui_node* font;   /* NULL when the backend offers no fonts */
    struct gui_node* ui_font;   /* proportional; NULL when none offered */
    struct gui_node* ui_size;   /* Small / Normal / Larger, of the editor font */
};

struct open_dialog
{
    struct gui_node* window;
    struct gui_node* name;
    struct gui_node* list;
    struct gui_node* filter;
    struct gui_node* filter_label;
    int filter_hidden;     /* picking a folder: no Type */
    char save_name[260];   /* Save As: the name after the folder in Name */
    struct gui_node* list_label;
    struct gui_node* ok;
    char dir[1024];
    struct ide_dir_entry entries[MAX_FOLDER_ENTRIES];   /* the rows after ".." */
    int count;
    int save_as;           /* Save As rather than Open */
    int pick_folder;       /* Open Folder: OK picks the folder shown */
    struct gui_node* pick_input;   /* a picker: the path goes into this input */
    int pick_include;      /* a picker: the folder goes into the Properties' include list */
    int open_project;      /* Open Project: OK opens the .cakeproj as the project */
    int add_to_project;    /* Add Existing File: OK adds the file to the project */
};

struct newfile_dialog
{
    struct gui_node* window;
    struct gui_node* folder;
    struct gui_node* name;
};

struct wrap_dialog
{
    struct gui_node* window;
    struct gui_node* target;   /* the editor it was opened for */
    struct gui_node* columns;
    struct gui_node* justify;
};

struct find_dialog
{
    struct gui_node* window;
    struct gui_node* input;
    struct gui_node* options;
    struct gui_node* direction;
    struct gui_node* scope;
    struct gui_node* origin;
};

struct replace_dialog
{
    struct gui_node* window;
    struct gui_node* find;
    struct gui_node* new_text;
    struct gui_node* options;
    struct gui_node* direction;
    struct gui_node* scope;
    struct gui_node* origin;
};

/* The last search's settings - Search Next (F3) repeats it. */
struct search_state
{
    char pattern[256];
    int match_case, whole, forward;
    int selected_only;     /* scope: the text selected when the dialog was used */
    int from_start;        /* origin: the whole scope rather than from the caret */
    int scope_lo, scope_hi;
};

struct rename_dialog
{
    struct gui_node* window;
    struct gui_node* input;
    char file[1024];   /* the identifier's file, line and column (1-based, bytes) */
    int line, col;
    int running;       /* the running compile is this rename */
};

struct project_rename_dialog
{
    struct gui_node* window;
    struct gui_node* input;
};

struct edit_string_dialog
{
    struct gui_node* window;
    struct gui_node* editor;
    char path[1024];           /* the document whose string is edited */
    int lo, hi;                /* the literals, quotes included: [lo, hi) of the document */
};

struct new_folder_dialog
{
    struct gui_node* window;
    struct gui_node* input;
};

struct git_clone_dialog
{
    struct gui_node* window;
    struct gui_node* url;
    struct gui_node* path;
    struct gui_node* open_folder;
    char parent[1024];   /* where Path puts the URL's folder */
    int path_edited;     /* Path typed by the user: the URL no longer fills it */
};

/* Git Changes: `git status --porcelain` of the Folder panel's repository. */
struct git_panel
{
    struct gui_node* window;
    struct gui_node* list;
    struct gui_node* menu;
    struct gui_node* staged_items[2];   /* Commit Staged (&& Push): only with something staged */
    char root[1024];              /* git rev-parse --show-toplevel */
    struct ide_strings paths;    /* each row's path, from the root */
    struct ide_strings xy;       /* each row's two status letters */
    /* the commit dialog */
    struct gui_node* commit_window;
    struct gui_node* message;
    enum { GIT_COMMIT_ALL, GIT_COMMIT_STAGED, GIT_COMMIT_FILE } commit_mode;
    int commit_push;
    char commit_file[1024];
    /* the branch dialog */
    struct gui_node* branch_window;
    struct gui_node* branches;
    struct gui_node* new_branch;
    /* the diff window: one, reused for every file - the old IDE's */
    struct gui_node* diff_window;
    struct gui_node* diff_editor;
    struct gui_node* diff_counter;
    struct gui_highlighter diff_highlighter;
    char diff_path[2100];         /* the file, absolute */
    int diff_prefixed;            /* a real diff (each row "+", "-" or " "), not a whole new file */
    int* runs;                    /* first line (1-based) of each run of changed rows */
    int run_count;
};

struct new_project_dialog
{
    struct gui_node* window;
    struct gui_node* folder;
    struct gui_node* name;
    struct gui_node* checks;   /* Create Folder, Hello World */
};

#define MAX_RECENT_PROJECTS 10


#define COPTS_PAGES 4   /* Compiler, Includes, Build, Debugger */

struct copts_dialog
{
    struct gui_node* window;
    struct gui_node* configs;   /* the configuration being edited, not the one in use */
    struct gui_node* config_buttons[3];   /* New, Rename, Delete */
    struct gui_node* pages;
    /* each page's nodes: only the selected page's are in the window */
    struct gui_node* page_nodes[COPTS_PAGES][24];
    int page_node_count[COPTS_PAGES];
    struct gui_node* buttons[4];   /* OK, Cancel, Help, Auto Config: kept after the page, for the Tab order */
    int page;
    struct gui_node* cake_target;
    struct gui_node* headers;
    struct gui_node* style;
    struct gui_node* diag;
    struct gui_node* flags;
    struct gui_node* output;
    struct gui_node* options;
    struct gui_node* post_build[3];   /* Command, Arguments, Directory */
    struct gui_node* debugger;
    struct gui_node* debug[3];   /* Command, Arguments, Directory */
    struct gui_node* includes;
    struct include_dirs include_dirs;   /* the Includes page's list, being edited */
    struct compiler_settings* settings;   /* the global ones or the project's */
    struct compiler_settings edit;        /* what OK saves to settings */
    int shown;                            /* the configuration whose options are shown, -1 none */
    struct gui_node* name_window;         /* New and Rename: the configuration's name */
    struct gui_node* name_input;
    int name_new;                         /* New, else Rename */
};

/* A Build (F7) or F5 in steps: the Pre-Build Event, the Build, the
 * Post-Build Event, then, for F5, the debugger. Each step starts only when
 * the one before ends without errors (a tool: exit code 0). */
enum build_stage
{
    STAGE_NONE,
    STAGE_PRE_BUILD,
    STAGE_BUILD,
    STAGE_POST_BUILD,
};

struct build_chain
{
    enum build_stage stage;   /* the step running */
    int rebuild;
    int compile;              /* Compile (Ctrl+F7): Cake on the active file only */
    int debug;                /* F5: the debugger last */
};

#define MAX_EXT_TOOLS 32
#define EDIT_STRING_COLUMNS 80   /* Edit String's literals stay within it, as the help texts */
#define EXT_DIALOG_COLS 66
#define EXT_DIALOG_ROWS 22

/* Each field is owned; see ext_tool_init / ext_tool_destroy. */
struct ext_tool
{
    char* title;
    char* command;
    char* arguments;
    char* directory;
};

struct ext_tools
{
    struct ext_tool tools[MAX_EXT_TOOLS];
    int count;
};

/* *field replaced by a copy of `s`; kept as it was when out of memory. */
static void ext_set(char** field, const char* s)
{
    size_t n = strlen(s) + 1;
    char* copy = malloc(n);
    if (!copy)
        return;
    memcpy(copy, s, n);
    free(*field);
    *field = copy;
}

static void ext_tool_init(struct ext_tool* t, const char* title, const char* command,
                          const char* arguments, const char* directory)
{
    memset(t, 0, sizeof *t);
    ext_set(&t->title, title);
    ext_set(&t->command, command);
    ext_set(&t->arguments, arguments);
    ext_set(&t->directory, directory);
}

static void ext_tool_destroy(struct ext_tool* t)
{
    free(t->title);
    free(t->command);
    free(t->arguments);
    free(t->directory);
    memset(t, 0, sizeof *t);
}

static void ext_tools_clear(struct ext_tools* l)
{
    for (int i = 0; i < l->count; i++)
        ext_tool_destroy(&l->tools[i]);
    l->count = 0;
}

static void ext_tools_copy(struct ext_tools* dst, const struct ext_tools* src)
{
    ext_tools_clear(dst);
    for (int i = 0; i < src->count; i++)
    {
        const struct ext_tool* t = &src->tools[i];
        ext_tool_init(&dst->tools[i], t->title, t->command, t->arguments, t->directory);
    }
    dst->count = src->count;
}

struct ext_dialog
{
    struct gui_node* window;
    struct gui_node* list;
    struct gui_node* fields[4];   /* Title, Command, Arguments, Directory */
    struct ext_tools edit;        /* the copy being edited; OK keeps it */
    int current;                  /* the tool the fields show */
};


/* The External Tools macros, as the old IDE offers them; picking one
 * appends it to the field its "  >  " button sits by. */
static const struct macro
{
    const char* name;
    const char* hint;
} macros[] = {
    { "$(FilePath)", "The active document's full path (quoted in Arguments)" },
    { "$(FileDir)", "The active document's folder, without a trailing slash" },
    { "$(FileName)", "The active document's file name, without its extension" },
    { "$(FileExt)", "The active document's extension, including the dot" },
    { "$(CakeOutput)", "Cake's output file(s) - the C89 code Build generates" },
    { "$(CakeOutputChanged)", "Cake's output file(s) the last Build compiled" },
    { "$(CakeInputFiles)", "The project's .c source files" },
    { "$(CakeInputChanged)", "The project's .c source files the last Build compiled" },
    { "$(TargetPath)", "The full path of the binary - exactly what Debug (F5) launches" },
    { "$(TargetDir)", "The folder the binary goes to: <project dir>/<platform>, without a trailing slash" },
    { "$(TargetFileName)", "The binary's file name, with its extension" },
    { "$(TargetName)", "The binary's file name without its extension" },
    { "$(TargetExt)", "The binary's extension, including the dot" },
    { "$(ProjectDir)", "The open project's folder, without a trailing slash" },
    { "$(ProjectName)", "The open project's name" },
    { "$(InstallDir)", "The folder of cakeide, where cake is installed, without a trailing slash" },
    { "$(Platform)", "The compilation target's name, e.g. x86_64-pc-windows-msvc" },
    { "$(IncludeDirs)", "The open project's include directories, as -I options" },
};

#define MAX_MACRO_BUTTONS 16

/* The "  >  " buttons and the input each one fills. */
struct macro_buttons
{
    struct gui_node* menu;
    struct gui_node* buttons[MAX_MACRO_BUTTONS];
    struct gui_node* inputs[MAX_MACRO_BUTTONS];
    int count;
    struct gui_node* target;   /* the input the open popup fills */
};

/* The F1 texts, help_topics[]: a field or dialog names its own with set_help. */
enum help_id
{
    HELP_NONE = -1,
    HELP_OVERVIEW,
    HELP_EXT_TOOLS,
    HELP_EXT_LIST,
    HELP_EXT_TITLE,
    HELP_EXT_COMMAND,
    HELP_EXT_ARGUMENTS,
    HELP_EXT_DIRECTORY,
    HELP_CLONE,
    HELP_CLONE_URL,
    HELP_CLONE_PATH,
    HELP_CLONE_OPEN_FOLDER,
    HELP_NEW_PROJECT,
    HELP_NEW_PROJECT_NAME,
    HELP_NEW_PROJECT_CREATE_FOLDER,
    HELP_NEW_PROJECT_HELLO_WORLD,
    HELP_COPTS,
    HELP_COPTS_TARGET,
    HELP_AUTO_CONFIG,
    HELP_TARGET_CLANG_MACOS_ARM64,
    HELP_TARGET_GCC_LINUX_ARM64,
    HELP_TARGET_GCC_LINUX_X64,
    HELP_TARGET_MSVC_WIN_X64,
    HELP_TARGET_MSVC_WIN_X86,
    HELP_TARGET_TCC_LINUX_X64,
    HELP_TARGET_TCC_MACOS_ARM64,
    HELP_TARGET_TCC_WIN_X64,
    HELP_TARGET_GCC_LINUX_ARM32,
    HELP_COPTS_HEADERS,
    HELP_HEADERS_SYSTEM,
    HELP_HEADERS_CAKE,
    HELP_COPTS_STYLE,
    HELP_STYLE_NONE,
    HELP_STYLE_CAKE,
    HELP_STYLE_GNU,
    HELP_STYLE_MICROSOFT,
    HELP_COPTS_DIAG,
    HELP_DIAG_IDE,
    HELP_DIAG_GCC,
    HELP_DIAG_MSVC,
    HELP_FLAG_LINE_DIRECTIVES,
    HELP_FLAG_FLOW,
    HELP_FLAG_CONST_LITERAL,
    HELP_FLAG_WALL,
    HELP_FLAG_DEFAULT_NONNULL,
    HELP_FLAG_ANNOTATIONS,
    HELP_COPTS_OUTPUT,
    HELP_COPTS_OPTIONS,
    HELP_COPTS_CONFIG,
    HELP_DEBUGGER_CDB,
    HELP_DEBUGGER_LLDB,
    HELP_DEBUG_COMMAND,
    HELP_DEBUG_ARGUMENTS,
    HELP_DEBUG_DIRECTORY,
    HELP_DEBUG_OPTIONS,
    HELP_DEBUG_INFO,
    HELP_BUILD_OPTIONS,
    HELP_PRE_BUILD,
    HELP_POST_BUILD,
    HELP_FIND_LOOK_IN,
    HELP_LOOK_IN_FILE,
    HELP_LOOK_IN_DIR,
    HELP_LOOK_IN_INCLUDE_DIRS,
    HELP_LOOK_IN_PROJECT,
    HELP_INCLUDE_DIRS,
    HELP_PLAYGROUND,
    HELP_COUNT
};

struct help_topic
{
    const char* slug;   /* the link name: [Arguments](help:ext-arguments) */
    const char* text;
};

/* The topics shown before the current one, for Back. */
struct help_history
{
    enum help_id* items;
    int count, cap;
};

struct help_window
{
    struct gui_node* window;
    struct gui_node* editor;
    struct gui_node* back;
    /* F1's topic for a dialog or one of its fields - the old IDE's ui_set_help */
    struct { const struct gui_node* node; enum help_id topic; } docs[128];
    int doc_count;
    enum help_id current;          /* HELP_NONE: a text built when shown */
    struct help_history history;
};

enum themed_role { THEMED_LABEL, THEMED_MODAL, THEMED_MODAL_BG, THEMED_PANEL };

/* Texts colored from the theme, recolored when the theme changes. */
struct themed_texts
{
    struct { struct gui_node* node; enum themed_role role; uint32_t fg; } items[512];
    int count;
};

struct ide
{
    struct gui_app* app;
    const struct gui_theme* theme;
    struct themed_texts themed;
    struct gui_highlighter c_highlighter;   /* ctx: the current theme */
    struct gui_highlighter md_highlighter;  /* .md source; ctx: the theme */
    struct gui_highlighter string_highlighter;  /* Edit String; ctx: the theme */
    struct gui_node* statusbar;
    struct gui_node* status_message;   /* its last item: the IDE's messages */
    struct gui_node* dock_menu;
    struct folder_panel folder;
    struct output_panel output;
    struct find_results fr;
    struct navigation nav;
    enum find_kind find_kind;     /* the running compile is a Find Definition/... */
    char find_word[128];          /* its word, for the text search fallback */
    struct find_in_files fif;
    struct doc docs[MAX_DOCS];
    int doc_count;
    int docs_made;     /* for placing the next window */
    struct goto_dialog go;
    struct env_dialog env;
    struct open_dialog open;
    struct newfile_dialog newfile;
    struct wrap_dialog wrap;
    struct find_dialog find;
    struct replace_dialog replace;
    struct search_state search;
    struct gui_node* editor_menu;
    struct gui_node* folder_menu;
    struct rename_dialog rename;
    struct project_rename_dialog project_rename;
    struct edit_string_dialog estr;
    struct new_folder_dialog new_folder;
    struct git_clone_dialog clone;
    struct git_panel git;
    /* A program running in the background - an External Tool, a git command -
     * its output streamed into the Output panel; its steps run one after
     * the other, a failed one stops them. The old IDE's g_job / g_gitjob. */
    struct
    {
        struct ide_process* proc;
        struct ide_strings steps;
        int step;
        int failed;
        char dir[1024];
        char title[64];
        enum { RUN_TOOL, RUN_GIT, RUN_GIT_QUIET, RUN_CLONE } kind;
        char clone_dest[1024];
        int clone_open;
        struct ide_text text;     /* what it printed, for the git message box */
    } run;
    /* Complete Word: a -complete compile, then the candidates - the old IDE's */
    struct
    {
        int running;
        struct doc* doc;
        int cursor;           /* the caret when asked; moved since: the answer is dropped */
        int prefix_start;
        char prefix[128];
        char op_fix[3];       /* "->" or ".": the operator before the prefix was the wrong one */
        char names[30][128];
        int count;
        struct gui_node* popup;
    } complete;
    /* Folder, Project and Git are one place, one of them shown: where it is */
    struct
    {
        enum gui_dock side;    /* GUI_DOCK_NONE: floating at rect */
        int size;              /* docked: width (height at the bottom) */
        struct gui_rect rect;
        int active;            /* 0 folder, 1 project, 2 git */
    } side;
    struct new_project_dialog new_project;
    struct ide_strings recent_projects;   /* .cakeproj paths, newest first, at most MAX_RECENT_PROJECTS */
    struct copts_dialog copts;
    struct compiler_settings global_options;

    struct debug_session session;   /* Start Debugging's lldb (cdb on Windows) */
    struct gui_node* debug_items[6];   /* Start, Stop, Continue, Step Over, Step Into, Step Out */
    struct sent_breakpoint* sent_bps;  /* the session's breakpoints, as the debugger has them */
    int sent_bp_count, sent_bp_cap;
    int next_bp_id;
    struct gui_node* debug_info_window;   /* Locals and Call Stack, docked right */
    struct gui_node* debug_info_list;
    int session_was_stopped;
    char session_file[1024];        /* where the exec line was last shown */
    int session_line;
    struct ext_dialog ext;
    struct ext_tools ext_tools;
    struct ide_project project;
    struct gui_node* project_window;
    struct gui_node* project_list;
    struct gui_node* project_menu;
    struct gui_node* project_items[6];   /* menu items that need an open project */
    struct ide_build_state pending;     /* what the running Build reads (worker thread) */
    int project_build;                   /* the running compile is a project Build */
    struct build_chain chain;
    char pending_delete[1400];           /* the Project panel's Delete confirmation */
    int newfile_in_project;              /* New File from the Project panel: added to it */
    char reload_path[1024];              /* the file the Reload? prompt is about */
    char pending_path[1400];             /* the file an Overwrite? prompt is about */
    char* pending_url;                   /* the web page an Open? prompt is about */
    struct help_window help;
    struct gui_node* about;
    struct gui_node* about_ok;
    struct macro_buttons macro;
    struct gui_node* tools_menu;
    struct gui_node* config_menu;   /* Build > Configuration: the configuration the Build uses */
    struct gui_node* recent_menu;   /* File > Recent Projects */
    struct ide_compile_job* job;
    struct gui_node* pending_close;   /* the window the Close prompt is about */
    struct gui_node* exit_window;     /* the window the Exit prompt is about */
    const char* labels[EV_COUNT];   /* each menu event's label, for messages */
};

static const struct gui_theme* const themes[] = {
    &ide_theme_ambar, &ide_theme_dark, &ide_theme_white, &ide_theme_nebula,
    &ide_theme_xcode_dark, &ide_theme_raspberry_pi, &ide_theme_nebula2,
};
static const char* const theme_names[] = { "Ambar", "Dark", "White", "Nebula", "Xcode Dark", "Raspberry Pi", "Nebula II" };

/* --- Small helpers --- */

/* A message in the statusbar, right after its hotkeys - the version keeps
 * the right corner, as in the old IDE. */
static void status(struct ide* ide, const char* text)
{
    gui_set_label(ide->status_message, text);
}

/* One line at the end of the Output panel. */
static void output(struct ide* ide, const char* line)
{
    const char* old = gui_get_value(ide->output.editor);
    size_t old_len = strlen(old), len = strlen(line);
    int lines = 1, caret_line = 0, caret_col = 0, lo = 0, hi = 0;
    for (const char* c = old; *c; c++)
        lines += *c == '\n';
    gui_editor_get_caret(ide->output.editor, &caret_line, &caret_col);
    gui_editor_get_selection(ide->output.editor, &lo, &hi);
    char* text = malloc(old_len + len + 2);
    if (!text)
        return;
    memcpy(text, old, old_len);
    memcpy(text + old_len, line, len);
    text[old_len + len] = '\n';
    text[old_len + len + 1] = '\0';
    gui_set_value(ide->output.editor, text);
    free(text);
    if (caret_line >= lines)
    {
        gui_editor_goto_line(ide->output.editor, 1 << 30);   /* following the end */
    }
    else
    {
        /* the caret moved up (a click, a selection): the view stays there */
        gui_editor_goto_line(ide->output.editor, caret_line);
        gui_editor_set_selection(ide->output.editor, lo, hi);
    }
}

static struct gui_node* create(struct ide* ide, enum gui_kind kind, const char* label)
{
    struct gui_node* n = gui_create(ide->app, kind);
    if (label)
        gui_set_label(n, label);
    return n;
}

/* A node `cols` x `rows` cells at (col, row) cells of its parent - how the
 * old IDE's dialogs are laid out. */
static struct gui_node* add_at(struct ide* ide, struct gui_node* parent, enum gui_kind kind,
                               int col, int row, int cols, int rows, const char* label)
{
    struct gui_node* n = create(ide, kind, label);
    struct gui_layout l = { GUI_ANCHOR_LEFT | GUI_ANCHOR_TOP };
    l.left.cells = col;
    l.top.cells = row;
    l.width.cells = cols;
    l.height.cells = rows;
    gui_set_layout(n, &l);
    gui_append(parent, n);
    return n;
}

/* A child filling `parent` but for the given margins, in cells. */
static void fill_margins(struct gui_node* n, int left, int top, int right, int bottom)
{
    struct gui_layout l = { GUI_ANCHOR_LEFT | GUI_ANCHOR_TOP | GUI_ANCHOR_RIGHT | GUI_ANCHOR_BOTTOM };
    l.left.cells = left;
    l.top.cells = top;
    l.right.cells = right;
    l.bottom.cells = bottom;
    gui_set_layout(n, &l);
}

/* A child filling `parent` inside its one-cell frame. */
static void fill_frame(struct gui_node* n)
{
    struct gui_layout l = { GUI_ANCHOR_LEFT | GUI_ANCHOR_TOP | GUI_ANCHOR_RIGHT | GUI_ANCHOR_BOTTOM };
    l.left.cells = l.top.cells = l.right.cells = l.bottom.cells = 1;
    gui_set_layout(n, &l);
}

static void color_themed(const struct ide* ide, struct gui_node* n, enum themed_role role, uint32_t fg)
{
    const struct gui_theme* t = ide->theme;
    switch (role)
    {
    case THEMED_LABEL: gui_set_colors(n, t->label_fg, t->modal_bg); break;
    case THEMED_MODAL: gui_set_colors(n, t->modal_fg, t->modal_bg); break;
    case THEMED_MODAL_BG: gui_set_colors(n, fg, t->modal_bg); break;
    case THEMED_PANEL: gui_set_colors(n, t->panel_fg, t->panel_bg); break;
    }
}

/* THEMED_MODAL_BG keeps `fg`; the others take theirs from the theme. */
static void set_themed(struct ide* ide, struct gui_node* n, enum themed_role role, uint32_t fg)
{
    color_themed(ide, n, role, fg);
    if (ide->themed.count < COUNT(ide->themed.items))
    {
        ide->themed.items[ide->themed.count].node = n;
        ide->themed.items[ide->themed.count].role = role;
        ide->themed.items[ide->themed.count].fg = fg;
        ide->themed.count++;
    }
}

static struct gui_node* add_label(struct ide* ide, struct gui_node* parent, int col, int row, const char* text)
{
    struct gui_node* n = add_at(ide, parent, GUI_TEXT, col, row, 0, 1, text);
    set_themed(ide, n, THEMED_LABEL, 0);
    return n;
}

/* `n` placed `cols` x `rows` cells at (col, row) of a `w` x `h` dialog,
 * kept at its distance from the edges in `anchors` when the dialog resizes;
 * left and right both: it stretches. */
static void anchor_in(struct gui_node* n, int anchors, int col, int row, int cols, int rows, int w, int h)
{
    struct gui_layout l = { anchors };
    l.left.cells = col;
    l.top.cells = row;
    l.right.cells = w - col - cols;
    l.bottom.cells = h - row - rows;
    l.width.cells = cols;
    l.height.cells = rows;
    gui_set_layout(n, &l);
}

/* A modal dialog; its size in cells is applied when it opens. */
static struct gui_node* new_dialog(struct ide* ide, const char* title)
{
    struct gui_node* win = create(ide, GUI_WINDOW, title);
    gui_window_set_modal(win, 1);
    gui_window_set_resizable(win, 0);
    gui_window_set_shadow(win, 1);
    return win;
}

/* Sized in cells now, with the current font, then centered. */
static void show_dialog(struct ide* ide, struct gui_node* win, int cols, int rows,
                        struct gui_node* first)
{
    int cw = 0, ch = 0;
    gui_cell_size(ide->app, &cw, &ch);
    struct gui_rect r = { 0, 0, cols * cw, rows * ch };
    gui_window_set_rect(win, &r);
    gui_window_center(ide->app, win);
    gui_window_open(ide->app, win);
    if (first)
        gui_focus(ide->app, first);
}

/* A resizable dialog: sized `cols` x `rows` the first time, then it keeps
 * the size the user gave it. */
static void show_resizable_dialog(struct ide* ide, struct gui_node* win, int cols, int rows,
                                  struct gui_node* first)
{
    if (gui_window_get_rect(win).w == 0)
    {
        show_dialog(ide, win, cols, rows, first);
        return;
    }
    gui_window_center(ide->app, win);
    gui_window_open(ide->app, win);
    if (first)
        gui_focus(ide->app, first);
}

static int is_open(struct ide* ide, struct gui_node* win)
{
    for (int i = 0; i < gui_window_count(ide->app); i++)
    {
        if (gui_window_at(ide->app, i) == win)
            return 1;
    }
    return 0;
}

/* --- Documents --- */

static int ends_with(const char* s, const char* suffix)
{
    size_t a = strlen(s), b = strlen(suffix);
    return a >= b && _stricmp(s + a - b, suffix) == 0;
}

static const char* file_name(const char* path)
{
    const char* name = path;
    for (const char* p = path; *p; p++)
    {
        if (*p == '/' || *p == '\\')
            name = p + 1;
    }
    return name;
}

/* The document whose editor has the focus, or else the top document window. */
static struct doc* active_doc(struct ide* ide)
{
    struct gui_node* focused = gui_focused(ide->app);
    for (int i = 0; i < ide->doc_count; i++)
    {
        if (ide->docs[i].editor == focused)
            return &ide->docs[i];
    }
    for (int w = gui_window_count(ide->app) - 1; w >= 0; w--)
    {
        struct gui_node* win = gui_window_at(ide->app, w);
        for (int i = 0; i < ide->doc_count; i++)
        {
            if (ide->docs[i].window == win)
                return &ide->docs[i];
        }
    }
    return NULL;
}

/* The editor under keyboard focus (a document's or a panel's), or NULL. */
static struct gui_node* focused_editor(struct ide* ide)
{
    struct gui_node* f = gui_focused(ide->app);
    if (f && gui_get_kind(f) == GUI_EDITOR)
        return f;
    struct doc* d = active_doc(ide);
    return d ? d->editor : NULL;
}

/* Opens `path` in a new document window, or raises the one it is in -
 * placed like the old IDE's make_editor_window: 70 x 20 cells, each new
 * one shifted down-right, maximized into the desktop. */
static void project_open(struct ide* ide, const char* path);

static void open_file(struct ide* ide, const char* name)
{
    char path[1024] = { 0 };
    ide_full_path(name, path, sizeof path);
    for (int i = 0; i < ide->doc_count; i++)
    {
        if (_stricmp(ide->docs[i].path, path) == 0)
        {
            gui_window_open(ide->app, ide->docs[i].window);
            gui_focus(ide->app, ide->docs[i].editor);
            return;
        }
    }
    if (ide->doc_count == MAX_DOCS)
    {
        status(ide, "Too many open files");
        return;
    }
    int crlf = 0;
    char* text = ide_read_file(path, &crlf);
    if (!text)
    {
        char msg[1100] = { 0 };
        snprintf(msg, sizeof msg, "Cannot read %s", path);
        status(ide, msg);
        return;
    }

    int cw = 0, ch = 0;
    gui_cell_size(ide->app, &cw, &ch);
    int seq = ide->docs_made++;
    struct doc* d = &ide->docs[ide->doc_count++];
    snprintf(d->path, sizeof d->path, "%s", path);
    d->crlf = crlf;
    d->file_time = ide_file_time(path);
    d->window = create(ide, GUI_WINDOW, file_name(path));
    gui_set_id(d->window, EV_DOC_CLOSE);
    gui_window_set_ask_close(d->window, 1);
    struct gui_rect r = { (5 + (seq % 6) * 2) * cw, (2 + seq % 6) * ch, 70 * cw, 20 * ch };
    gui_window_set_rect(d->window, &r);
    d->editor = create(ide, GUI_EDITOR, NULL);
    fill_frame(d->editor);
    if (ends_with(path, ".c") || ends_with(path, ".h"))
    {
        gui_editor_set_line_numbers(d->editor, 1);
        gui_editor_set_highlighter(d->editor, &ide->c_highlighter);
    }
    else if (ends_with(path, ".md"))
    {
        gui_editor_set_highlighter(d->editor, &ide->md_highlighter);
    }
    gui_set_value(d->editor, text);
    free(text);
    gui_set_context_menu(d->editor, ide->editor_menu);
    gui_editor_set_click_id(d->editor, EV_EDITOR_CTRLCLICK);
    gui_append(d->window, d->editor);
    gui_window_open(ide->app, d->window);
    gui_window_maximize(ide->app, d->window);
    gui_focus(ide->app, d->editor);

    char msg[1100] = { 0 };
    snprintf(msg, sizeof msg, "Opened %s", path);
    output(ide, msg);
}

static void save_doc(struct ide* ide, struct doc* d)
{
    char msg[1100] = { 0 };
    if (ide_write_file(d->path, gui_get_value(d->editor), d->crlf) == 0)
    {
        gui_editor_set_dirty(d->editor, 0);
        d->file_time = ide_file_time(d->path);   /* our own write is no outside change */
        snprintf(msg, sizeof msg, "Saved %s", d->path);
    }
    else
    {
        snprintf(msg, sizeof msg, "Cannot write %s", d->path);
    }
    status(ide, msg);
    output(ide, msg);
}

/* --- The Folder panel --- */

static void folder_refresh(struct ide* ide)
{
    struct folder_panel* f = &ide->folder;
    gui_clear_children(f->list);
    int n = ide_list_dir(f->dir, f->entries, MAX_FOLDER_ENTRIES);
    f->count = n > 0 ? n : 0;

    /* As the old IDE: "..\", then folders with a trailing '\', then files. */
    gui_append(f->list, create(ide, GUI_ITEM, "..\\"));
    for (int i = 0; i < f->count; i++)
    {
        char label[300] = { 0 };
        snprintf(label, sizeof label, "%s%s", f->entries[i].name, f->entries[i].is_dir ? "\\" : "");
        gui_append(f->list, create(ide, GUI_ITEM, label));
    }
    gui_set_selected(f->list, 0);
    gui_set_label(f->window, file_name(f->dir));
}

/* A double click or Enter on a row: ".." goes up, a folder goes in, a file
 * opens. */
static void folder_open_selected(struct ide* ide)
{
    struct folder_panel* f = &ide->folder;
    int row = gui_get_selected(f->list);
    if (row == 0)
    {
        char* slash = strrchr(f->dir, '\\');
        if (!slash)
            slash = strrchr(f->dir, '/');
        if (slash && slash != f->dir && slash[-1] != ':')
            *slash = '\0';
        else if (slash)
            slash[1] = '\0';
        folder_refresh(ide);
        return;
    }
    if (row < 1 || row > f->count)
        return;
    const struct ide_dir_entry* e = &f->entries[row - 1];
    char path[1400] = { 0 };
    size_t len = strlen(f->dir);
    snprintf(path, sizeof path, "%s%s%s", f->dir,
             (len > 0 && (f->dir[len - 1] == '\\' || f->dir[len - 1] == '/')) ? "" : IDE_PATH_SEP, e->name);
    if (e->is_dir && strlen(path) >= sizeof f->dir)
    {
        status(ide, "The path is too long");
    }
    else if (e->is_dir)
    {
        memcpy(f->dir, path, strlen(path) + 1);
        folder_refresh(ide);
    }
    else if (ends_with(path, ".cakeproj"))
    {
        project_open(ide, path);
    }
    else
    {
        open_file(ide, path);
    }
}

/* --- Building the screen --- */

static struct gui_node* menu_item_create(struct ide* ide, const struct menu_item* spec)
{
    struct gui_node* it = create(ide, GUI_ITEM, spec->label);
    if (spec->id == EV_NONE)
    {
        gui_set_separator(it, 1);
        return it;
    }
    if (spec->id != EV_MENU_RECENT)
        gui_set_id(it, spec->id);
    ide->labels[spec->id] = spec->label;
    if (spec->shortcut)
        gui_set_shortcut(it, spec->shortcut);
    if (spec->hint)
        gui_set_hint(it, spec->hint);
    gui_set_enabled(it, spec->enabled);
    return it;
}

static void build_menus(struct ide* ide)
{
    struct gui_node* menubar = create(ide, GUI_MENUBAR, NULL);
    for (int m = 0; m < COUNT(menus); m++)
    {
        struct gui_node* menu = create(ide, GUI_MENU, menus[m].title);
        for (int i = 0; i < menus[m].count; i++)
        {
            const struct menu_item* spec = &menus[m].items[i];
            struct gui_node* it = menu_item_create(ide, spec);
            if (spec->id == EV_MENU_RECENT)
                ide->recent_menu = it;   /* filled by recent_menu_refresh */
            gui_append(menu, it);
            static const int debug_ids[] = { EV_DEBUG_START, EV_DEBUG_STOP, EV_DEBUG_CONTINUE,
                                             EV_DEBUG_STEP_OVER, EV_DEBUG_STEP_INTO, EV_DEBUG_STEP_OUT };
            for (int k = 0; k < COUNT(debug_ids); k++)
            {
                if (spec->id == debug_ids[k])
                    ide->debug_items[k] = it;
            }
            static const int needs_project[] = {
                EV_PROJECT_ADD_FILE,
                EV_PROJECT_REPORT_UNUSED, EV_PROJECT_CLOSE, EV_PROJECT_RENAME,
            };
            for (int k = 0; k < COUNT(needs_project); k++)
            {
                if (spec->id == needs_project[k])
                {
                    ide->project_items[k] = it;
                    gui_set_enabled(it, 0);
                }
            }
        }
        if (menus[m].items == tools_items)
            ide->tools_menu = menu;
        if (menus[m].items == build_items)
        {
            struct gui_node* sep = create(ide, GUI_ITEM, NULL);
            gui_set_separator(sep, 1);
            gui_append(menu, sep);
            ide->config_menu = create(ide, GUI_ITEM, "Configuration");
            gui_set_hint(ide->config_menu, "The configuration Build, Compile and Start Debugging use");
            gui_append(menu, ide->config_menu);   /* filled by config_menu_refresh */
        }
        gui_append(menubar, menu);
    }
    gui_append(gui_root(ide->app), menubar);
}

static void build_statusbar(struct ide* ide)
{
    ide->statusbar = create(ide, GUI_STATUSBAR, "Cake " CAKE_VERSION);
    static const struct { int id; const char* label; const char* shortcut; } hotkeys[] = {
        { EV_HELP, "F1:Help", "F1" },
        { EV_BACK, "Alt \xE2\x97\x84:Back", "Alt+Left" },          /* U+25C4 */
        { EV_FORWARD, "Alt \xE2\x96\xBA:Forward", "Alt+Right" },   /* U+25BA */
    };
    for (int i = 0; i < COUNT(hotkeys); i++)
    {
        struct gui_node* h = create(ide, GUI_HOTKEY, hotkeys[i].label);
        gui_set_id(h, hotkeys[i].id);
        if (hotkeys[i].shortcut)
            gui_set_shortcut(h, hotkeys[i].shortcut);
        gui_append(ide->statusbar, h);
    }
    ide->status_message = create(ide, GUI_HOTKEY, "");
    gui_append(ide->statusbar, ide->status_message);
    gui_append(gui_root(ide->app), ide->statusbar);
}

/* A docked panel `size` cells thick, with the dock popup. */
/* The panels' first sizes, in cells: panels_size turns them into pixels
 * once settings_load has set the UI font - before it, the cells are the
 * default font's. */
static struct
{
    struct gui_node* win;
    enum gui_dock side;
    int cells;
} panel_sizes[16];
static int panel_size_count;

static void panel_set_size(struct ide* ide, struct gui_node* win, enum gui_dock side, int cells)
{
    int cw = 0, ch = 0;
    gui_cell_size(ide->app, &cw, &ch);
    gui_window_set_dock(win, side, cells * (side == GUI_DOCK_BOTTOM ? ch : cw));
}

static struct gui_node* new_panel(struct ide* ide, const char* title, enum gui_dock side, int size)
{
    struct gui_node* win = create(ide, GUI_WINDOW, title);
    panel_set_size(ide, win, side, size);
    gui_set_context_menu(win, ide->dock_menu);
    if (panel_size_count < (int)(sizeof panel_sizes / sizeof panel_sizes[0]))
    {
        panel_sizes[panel_size_count].win = win;
        panel_sizes[panel_size_count].side = side;
        panel_sizes[panel_size_count].cells = size;
        panel_size_count++;
    }
    return win;
}

/* Every panel at its first size in the UI font now set. */
static void panels_size(struct ide* ide)
{
    for (int i = 0; i < panel_size_count; i++)
        panel_set_size(ide, panel_sizes[i].win, panel_sizes[i].side, panel_sizes[i].cells);
}

static void build_popups(struct ide* ide);
static void add_popup_item(struct ide* ide, struct gui_node* menu, int id, const char* label,
                           const char* shortcut);
static void close_doc(struct ide* ide, struct doc* d);
static void show_side_panel(struct ide* ide, struct gui_node* win);
static void cmdline_layout(struct ide* ide);
static void debug_menu_refresh(struct ide* ide);
static void debug_info_refresh(struct ide* ide);
static void git_refresh(struct ide* ide);

static void build_panels(struct ide* ide)
{
    build_popups(ide);
    ide->dock_menu = create(ide, GUI_MENU, NULL);
    static const char* const dock_labels[] = { "Dock Left", "Dock Right", "Dock Bottom" };
    for (int i = 0; i < 3; i++)
    {
        struct gui_node* it = create(ide, GUI_ITEM, dock_labels[i]);
        gui_set_id(it, EV_COUNT + 1 + i);   /* past every other event - see on_event */
        gui_append(ide->dock_menu, it);
    }

    /* Folder: the current directory, in the "other fonts" font. */
    struct folder_panel* f = &ide->folder;
    f->window = new_panel(ide, "Folder", GUI_DOCK_LEFT, 20);
    f->list = create(ide, GUI_LISTBOX, NULL);
    fill_frame(f->list);
    set_themed(ide, f->list, THEMED_PANEL, 0);
    gui_set_font_size(f->list, GUI_FONT_SIZE_UI);
    gui_set_id(f->list, EV_FOLDER_OPEN);
    gui_set_context_menu(f->list, ide->folder_menu);
    gui_append(f->window, f->list);
    ide_current_dir(f->dir, sizeof f->dir);
    folder_refresh(ide);
    gui_window_open(ide->app, f->window);

    /* Output: read-only, small font. */
    ide->output.window = new_panel(ide, "Output", GUI_DOCK_BOTTOM, 12);
    ide->output.editor = create(ide, GUI_EDITOR, NULL);
    {
        /* the editor, then the command line on the last row */
        struct gui_layout l = { GUI_ANCHOR_LEFT | GUI_ANCHOR_TOP | GUI_ANCHOR_RIGHT | GUI_ANCHOR_BOTTOM };
        l.left.cells = l.top.cells = l.right.cells = 1;
        l.bottom.cells = 2;
        gui_set_layout(ide->output.editor, &l);
        ide->output.prompt = create(ide, GUI_TEXT, ">");
        struct gui_layout pl = { GUI_ANCHOR_LEFT | GUI_ANCHOR_BOTTOM };
        pl.left.cells = pl.bottom.cells = 1;
        pl.width.cells = 7;
        pl.height.cells = 1;
        gui_set_layout(ide->output.prompt, &pl);
        gui_set_font_size(ide->output.prompt, GUI_FONT_SIZE_SMALL);
        ide->output.input = create(ide, GUI_INPUT, NULL);
        gui_set_id(ide->output.input, EV_CMDLINE);
        gui_set_font_size(ide->output.input, GUI_FONT_SIZE_SMALL);
        gui_set_hint(ide->output.input, "An IDE command (help), a tool's title, a menu item, or a shell command");
        gui_append(ide->output.window, ide->output.prompt);
        gui_append(ide->output.window, ide->output.input);
        cmdline_layout(ide);
    }
    gui_set_font_size(ide->output.editor, GUI_FONT_SIZE_SMALL);
    gui_editor_set_read_only(ide->output.editor, 1);
    gui_set_id(ide->output.editor, EV_OUTPUT_DBLCLICK);
    gui_editor_set_vt100(ide->output.editor, 1);
    struct gui_node* output_menu = create(ide, GUI_MENU, NULL);
    add_popup_item(ide, output_menu, EV_OUTPUT_COPY_ALL, "Copy All", NULL);
    add_popup_item(ide, output_menu, EV_OUTPUT_SELECT_ALL, "Select All", NULL);
    add_popup_item(ide, output_menu, EV_OUTPUT_CLEAR, "Clear", NULL);
    gui_set_context_menu(ide->output.editor, output_menu);
    gui_append(ide->output.window, ide->output.editor);
    gui_window_open(ide->app, ide->output.window);

    /* Project: docked left in the Folder panel's place, shown when a project
     * opens - the old IDE's. */
    ide->project_window = new_panel(ide, "Project", GUI_DOCK_LEFT, 20);
    ide->project_list = create(ide, GUI_LISTBOX, NULL);
    fill_frame(ide->project_list);
    set_themed(ide, ide->project_list, THEMED_PANEL, 0);
    gui_set_font_size(ide->project_list, GUI_FONT_SIZE_UI);
    gui_set_id(ide->project_list, EV_PROJECT_LIST);
    ide->project_menu = create(ide, GUI_MENU, NULL);
    add_popup_item(ide, ide->project_menu, EV_PROJ_NEW_FILE, "New File...", NULL);
    add_popup_item(ide, ide->project_menu, EV_PROJ_COPY_PATH, "Copy Full Path", NULL);
    add_popup_item(ide, ide->project_menu, EV_PROJ_REMOVE, "Remove from Project", NULL);
    add_popup_item(ide, ide->project_menu, EV_PROJ_DELETE, "Delete", "Del");
    gui_set_context_menu(ide->project_list, ide->project_menu);
    gui_append(ide->project_window, ide->project_list);

    /* Git Changes: the third panel of the left side */
    struct git_panel* g = &ide->git;
    g->window = new_panel(ide, "Git Changes", GUI_DOCK_LEFT, 20);
    g->list = create(ide, GUI_LISTBOX, NULL);
    fill_frame(g->list);
    set_themed(ide, g->list, THEMED_PANEL, 0);
    gui_set_font_size(g->list, GUI_FONT_SIZE_UI);
    gui_set_id(g->list, EV_GIT_LIST);
    g->menu = create(ide, GUI_MENU, NULL);
    add_popup_item(ide, g->menu, EV_GIT_REFRESH, "Refresh", NULL);
    add_popup_item(ide, g->menu, EV_NONE, "", NULL);
    add_popup_item(ide, g->menu, EV_GIT_STAGE, "Stage", NULL);
    add_popup_item(ide, g->menu, EV_GIT_UNSTAGE, "Unstage", NULL);
    add_popup_item(ide, g->menu, EV_GIT_DISCARD, "Discard", NULL);
    add_popup_item(ide, g->menu, EV_GIT_DISCARD_ALL, "Discard All", NULL);
    add_popup_item(ide, g->menu, EV_NONE, "", NULL);
    add_popup_item(ide, g->menu, EV_GIT_IGNORE_FILE, "Ignore This Item", NULL);
    add_popup_item(ide, g->menu, EV_GIT_IGNORE_EXT, "Ignore This Extension", NULL);
    add_popup_item(ide, g->menu, EV_GIT_IGNORE_FOLDER, "Ignore This Folder", NULL);
    add_popup_item(ide, g->menu, EV_NONE, "", NULL);
    add_popup_item(ide, g->menu, EV_GIT_COMMIT, "Commit All...", NULL);
    add_popup_item(ide, g->menu, EV_GIT_COMMIT_PUSH, "Commit All && Push...", NULL);
    add_popup_item(ide, g->menu, EV_GIT_COMMIT_STAGED, "Commit Staged...", NULL);
    g->staged_items[0] = gui_child_at(g->menu, gui_child_count(g->menu) - 1);
    add_popup_item(ide, g->menu, EV_GIT_COMMIT_STAGED_PUSH, "Commit Staged && Push...", NULL);
    g->staged_items[1] = gui_child_at(g->menu, gui_child_count(g->menu) - 1);
    add_popup_item(ide, g->menu, EV_GIT_COMMIT_FILE, "Commit File...", NULL);
    add_popup_item(ide, g->menu, EV_NONE, "", NULL);
    add_popup_item(ide, g->menu, EV_GIT_PULL, "Pull", NULL);
    add_popup_item(ide, g->menu, EV_GIT_PUSH, "Push", NULL);
    add_popup_item(ide, g->menu, EV_GIT_SYNC, "Sync", NULL);
    add_popup_item(ide, g->menu, EV_GIT_BRANCH, "Branch...", NULL);
    add_popup_item(ide, g->menu, EV_GIT_CLONE, "Clone...", NULL);
    gui_set_context_menu(g->list, g->menu);
    gui_append(g->window, g->list);

    /* Debug Info: Locals and Call Stack while stopped - docked right */
    ide->debug_info_window = new_panel(ide, "Debug Info", GUI_DOCK_RIGHT, 30);
    ide->debug_info_list = create(ide, GUI_LISTBOX, NULL);
    fill_frame(ide->debug_info_list);
    set_themed(ide, ide->debug_info_list, THEMED_PANEL, 0);
    gui_set_font_size(ide->debug_info_list, GUI_FONT_SIZE_SMALL);
    gui_append(ide->debug_info_window, ide->debug_info_list);

    /* Find Results: the bottom dock, in Output's place when shown. */
    ide->fr.window = new_panel(ide, "Find Results", GUI_DOCK_BOTTOM, 12);
    ide->fr.editor = create(ide, GUI_EDITOR, NULL);
    fill_frame(ide->fr.editor);
    gui_set_font_size(ide->fr.editor, GUI_FONT_SIZE_SMALL);
    gui_editor_set_read_only(ide->fr.editor, 1);
    gui_editor_set_vt100(ide->fr.editor, 1);
    gui_set_id(ide->fr.editor, EV_FR_DBLCLICK);
    ide->fr.menu = create(ide, GUI_MENU, NULL);
    add_popup_item(ide, ide->fr.menu, EV_FR_CLEAR, "Clear", NULL);
    gui_set_context_menu(ide->fr.editor, ide->fr.menu);
    gui_append(ide->fr.window, ide->fr.editor);
    ide->find_kind = FIND_NONE;

    /* Find and Replace: docked right, shown from Search > Find in Files. */
    ide->fif.window = new_panel(ide, "Find and Replace", GUI_DOCK_RIGHT, FIF_MIN_COLS);
    gui_window_set_min_size(ide->fif.window, FIF_MIN_COLS, 0);
    ide->fif.match_case = 1;
    ide->fif.file_type = 2;   /* *.c;*.h */
}

static void build_open(struct ide* ide);
static void build_rename(struct ide* ide);
static void build_project_rename(struct ide* ide);
static void build_edit_string(struct ide* ide);
static void build_new_folder(struct ide* ide);
static void build_git_clone(struct ide* ide);
static void build_git(struct ide* ide);
static void build_new_project(struct ide* ide);
static void build_compiler_options(struct ide* ide);
static int ext_tool_find(struct ide* ide, const char* title);
static struct gui_node* add_macro_button(struct ide* ide, struct gui_node* parent, int col, int row,
                                         struct gui_node* input);
static void build_external_tools(struct ide* ide);
static void set_help(struct ide* ide, struct gui_node* node, const char* hint, enum help_id topic);
static enum help_id help_find(const char* slug, int len);
static void build_help(struct ide* ide);
static void build_help_texts(struct ide* ide);
static void build_newfile(struct ide* ide);
static void build_wrap(struct ide* ide);
static void build_find(struct ide* ide);

/* About: the name, version and site. 30 x 10. */
static void build_about(struct ide* ide)
{
    ide->about = new_dialog(ide, "");
    static const struct { int row; const char* text; } lines[] = {
        { 2, "Cake IDE" }, { 3, "Version " CAKE_VERSION }, { 5, "https://cakecc.org" },
    };
    for (int i = 0; i < COUNT(lines); i++)
    {
        /* the dialog's inside, the text centered in it */
        struct gui_node* n = add_at(ide, ide->about, GUI_TEXT, 1, lines[i].row, 28, 1, lines[i].text);
        set_themed(ide, n, THEMED_MODAL, 0);
        gui_set_centered(n, 1);
    }
    ide->about_ok = add_at(ide, ide->about, GUI_BUTTON, 9, 7, 12, 1, "OK");
    gui_set_id(ide->about_ok, EV_ABOUT_OK);
}

static void build_dialogs(struct ide* ide)
{
    build_about(ide);
    /* Go to Line Number - the old IDE's goto dialog, 40 x 8. */
    ide->go.window = new_dialog(ide, "Go to Line Number");
    add_label(ide, ide->go.window, 3, 2, "Enter New Line Number");
    ide->go.input = add_at(ide, ide->go.window, GUI_INPUT, 26, 2, 11, 1, NULL);
    gui_set_id(ide->go.input, EV_GOTO_OK);
    gui_set_id(add_at(ide, ide->go.window, GUI_BUTTON, 9, 5, 10, 1, "OK"), EV_GOTO_OK);
    gui_set_id(add_at(ide, ide->go.window, GUI_BUTTON, 21, 5, 10, 1, "Cancel"), EV_GOTO_CANCEL);

    /* Environment - 50 x 11: the theme and fonts. */
    ide->env.window = new_dialog(ide, "Environment");
    struct gui_node* theme_label = add_label(ide, ide->env.window, 3, 2, "Theme:");
    ide->env.theme = add_at(ide, ide->env.window, GUI_SELECT, 16, 2, 30, 1, NULL);
    gui_set_after_label(ide->env.theme, theme_label);
    /* Picking a theme applies it at once, as the old IDE; OK keeps it. */
    for (int i = 0; i < COUNT(theme_names); i++)
    {
        struct gui_node* it = create(ide, GUI_ITEM, theme_names[i]);
        gui_set_id(it, EV_ENV_THEME + i);
        gui_append(ide->env.theme, it);
    }
    /* Font: every control but the editors */
    if (gui_ui_font_count(ide->app) > 0)
    {
        struct gui_node* font_label = add_label(ide, ide->env.window, 3, 4, "Font:");
        ide->env.ui_font = add_at(ide, ide->env.window, GUI_SELECT, 16, 4, 30, 1, NULL);
        gui_set_after_label(ide->env.ui_font, font_label);
        for (int i = 0; i < gui_ui_font_count(ide->app) && i < 16; i++)
        {
            struct gui_node* it = create(ide, GUI_ITEM, gui_ui_font_name(ide->app, i));
            gui_set_id(it, EV_ENV_UI_FONT + i);
            gui_append(ide->env.ui_font, it);
        }
    }
    /* Editor Font: the backend's monospaced shortlist, and its size beside it; picking one applies it at once, as the old IDE. */
    if (gui_font_count(ide->app) > 0)
    {
        struct gui_node* editor_font_label = add_label(ide, ide->env.window, 3, 6, "Editor Font:");
        ide->env.font = add_at(ide, ide->env.window, GUI_SELECT, 16, 6, 20, 1, NULL);
        gui_set_after_label(ide->env.font, editor_font_label);
        for (int i = 0; i < gui_font_count(ide->app) && i < 16; i++)
        {
            struct gui_node* it = create(ide, GUI_ITEM, gui_font_name(ide->app, i));
            gui_set_id(it, EV_ENV_FONT + i);
            gui_append(ide->env.font, it);
        }
    }
    ide->env.ui_size = add_at(ide, ide->env.window, GUI_SELECT, 37, 6, 9, 1, NULL);
    static const char* const sizes[] = { "Small", "Normal", "Larger" };
    for (int i = 0; i < COUNT(sizes); i++)
    {
        struct gui_node* it = create(ide, GUI_ITEM, sizes[i]);
        gui_set_id(it, EV_ENV_EDITOR_SIZE + i);
        gui_append(ide->env.ui_size, it);
    }
    gui_set_id(add_at(ide, ide->env.window, GUI_BUTTON, 20, 8, 10, 1, "OK"), EV_ENV_OK);

    build_open(ide);
    build_newfile(ide);
    build_wrap(ide);
    build_find(ide);
    build_rename(ide);
    build_project_rename(ide);
    build_edit_string(ide);
    build_new_folder(ide);
    build_git_clone(ide);
    build_git(ide);
    ide->complete.popup = create(ide, GUI_MENU, NULL);   /* Complete Word's list */
    build_new_project(ide);
    build_compiler_options(ide);
    build_external_tools(ide);
    build_help(ide);
    build_help_texts(ide);
}

static void project_refresh(struct ide* ide, int selected);

static void apply_theme(struct ide* ide, int index)
{
    ide->theme = themes[index];
    ide->c_highlighter.ctx = (void*)ide->theme;
    ide->md_highlighter.ctx = (void*)ide->theme;
    ide->string_highlighter.ctx = (void*)ide->theme;
    gui_set_theme(ide->app, ide->theme);
    for (int i = 0; i < ide->themed.count; i++)
        color_themed(ide, ide->themed.items[i].node, ide->themed.items[i].role, ide->themed.items[i].fg);
    if (ide->project_list)
        project_refresh(ide, gui_get_selected(ide->project_list));   /* the markers' colors */
    for (int i = 0; i < ide->doc_count; i++)
    {
        if (ends_with(ide->docs[i].path, ".c") || ends_with(ide->docs[i].path, ".h"))
            gui_editor_set_highlighter(ide->docs[i].editor, &ide->c_highlighter);
        else if (ends_with(ide->docs[i].path, ".md"))
            gui_editor_set_highlighter(ide->docs[i].editor, &ide->md_highlighter);
    }
    if (ide->estr.editor)   /* the editor keeps a copy of the highlighter, with the old theme */
        gui_editor_set_highlighter(ide->estr.editor, &ide->string_highlighter);
}

/* --- Open a File / Save As: the old IDE's open dialog, 61 x 21 --- */

static const struct open_filter
{
    const char* label;
    const char* patterns;   /* ';'-separated "*.ext", or "*" */
} open_filters[] = {
    { "C Source Files (*.c)", "*.c" },
    { "Header Files (*.h)", "*.h" },
    { "C/C++ Sources (*.c;*.h)", "*.c;*.h" },
    { "Markdown Files (*.md)", "*.md" },
    { "Cake Project Files (*.cakeproj)", "*.cakeproj" },
    { "Programs (*.exe;*.bat;*.cmd)", "*.exe;*.bat;*.cmd" },
    { "All Files (*.*)", "*" },
};

#define C_SOURCES_FILTER 2   /* *.c;*.h */
#define PROJECT_FILTER 4     /* *.cakeproj */
#ifdef _WIN32
#define PROGRAMS_FILTER 5    /* *.exe;*.bat;*.cmd */
#else
#define PROGRAMS_FILTER 6    /* All Files: programs have no extension */
#endif

static int matches_filter(const char* name, const char* patterns)
{
    const char* p = patterns;
    while (*p)
    {
        const char* end = strchr(p, ';');
        size_t len = end ? (size_t)(end - p) : strlen(p);
        if (len == 1 && p[0] == '*')
            return 1;
        if (len > 1 && p[0] == '*')
        {
            char ext[32] = { 0 };
            snprintf(ext, sizeof ext, "%.*s", (int)(len - 1), p + 1);
            if (ends_with(name, ext))
                return 1;
        }
        p += len;
        if (*p == ';')
            p++;
    }
    return 0;
}

/* `dir` joined with `name`; "" when it does not fit. */
static void join_path(char* out, size_t cap, const char* dir, const char* name)
{
    size_t len = strlen(dir);
    int slash = len > 0 && (dir[len - 1] == '\\' || dir[len - 1] == '/');
    if (snprintf(out, cap, "%s%s%s", dir, slash ? "" : IDE_PATH_SEP, name) >= (int)cap)
    {
        out[0] = '\0';
    }
}

/* `dir` without its last component (kept as "C:\" at the root). */
static void parent_dir(char* dir)
{
    char* slash = strrchr(dir, '\\');
    if (!slash)
        slash = strrchr(dir, '/');
    if (slash && slash != dir && slash[-1] != ':')
        *slash = '\0';
    else if (slash)
        slash[1] = '\0';
}

static void open_refresh(struct ide* ide)
{
    struct open_dialog* o = &ide->open;
    struct ide_dir_entry* all = malloc(sizeof(struct ide_dir_entry) * MAX_FOLDER_ENTRIES);
    if (!all)
        return;
    int n = ide_list_dir(o->dir, all, MAX_FOLDER_ENTRIES);
    const char* patterns = open_filters[gui_get_selected(o->filter)].patterns;
    o->count = 0;
    for (int i = 0; i < n; i++)
    {
        if (all[i].is_dir || (!o->pick_folder && matches_filter(all[i].name, patterns)))
            o->entries[o->count++] = all[i];
    }
    free(all);

    gui_clear_children(o->list);
    gui_append(o->list, create(ide, GUI_ITEM, "..\\"));
    for (int i = 0; i < o->count; i++)
    {
        char label[300] = { 0 };
        snprintf(label, sizeof label, o->entries[i].is_dir ? "%s\\" : "%s", o->entries[i].name);
        gui_append(o->list, create(ide, GUI_ITEM, label));
    }
    gui_set_selected(o->list, 0);
    /* Name shows the folder - and Save As' name after it - as the old IDE */
    char shown[1400] = { 0 };
    if (o->pick_folder)
        snprintf(shown, sizeof shown, "%s", o->dir);
    else
        join_path(shown, sizeof shown, o->dir, o->save_name);
    gui_set_value(o->name, shown);
}

static void build_open(struct ide* ide)
{
    struct open_dialog* o = &ide->open;
    o->window = new_dialog(ide, "Open a File");
    add_label(ide, o->window, 3, 2, "Name");
    o->name = add_at(ide, o->window, GUI_INPUT, 3, 3, 41, 1, NULL);
    gui_set_id(o->name, EV_OPEN_OK);
    add_label(ide, o->window, 3, 5, "Files");
    o->list_label = gui_child_at(o->window, gui_child_count(o->window) - 1);
    o->list = add_at(ide, o->window, GUI_LISTBOX, 3, 6, 41, 10, NULL);
    gui_set_id(o->list, EV_OPEN_LIST);
    add_label(ide, o->window, 3, 17, "Type");
    o->filter_label = gui_child_at(o->window, gui_child_count(o->window) - 1);
    o->filter = add_at(ide, o->window, GUI_SELECT, 3, 18, 41, 1, NULL);
    for (int i = 0; i < COUNT(open_filters); i++)
    {
        struct gui_node* it = create(ide, GUI_ITEM, open_filters[i].label);
        gui_set_id(it, EV_OPEN_FILTER + i);
        gui_append(o->filter, it);
    }
    o->ok = add_at(ide, o->window, GUI_BUTTON, 46, 3, 12, 1, "Open");
    gui_set_id(o->ok, EV_OPEN_OK);
    gui_set_id(add_at(ide, o->window, GUI_BUTTON, 46, 5, 12, 1, "Cancel"), EV_OPEN_CANCEL);
}

/* Picking a folder: only folders listed and no Type - the old IDE's. */
static void open_folder_mode(struct ide* ide, int on)
{
    struct open_dialog* o = &ide->open;
    o->pick_folder = on;
    if (on && !o->filter_hidden)
    {
        gui_remove(o->window, o->filter_label);
        gui_remove(o->window, o->filter);
    }
    else if (!on && o->filter_hidden)
    {
        gui_append(o->window, o->filter_label);
        gui_append(o->window, o->filter);
    }
    o->filter_hidden = on;
    gui_set_label(o->list_label, on ? "Folders" : "Files");
    open_refresh(ide);
}

/* Open... (save_as 0) or Save As... (1), starting in `dir`. */
static void show_open(struct ide* ide, int save_as, const char* dir, const char* name)
{
    struct open_dialog* o = &ide->open;
    o->save_as = save_as;
    open_folder_mode(ide, 0);
    o->pick_input = NULL;
    o->pick_include = 0;
    o->open_project = 0;
    o->add_to_project = 0;
    gui_set_multi(o->list, 0);
    gui_set_label(o->window, save_as ? "Save As" : "Open a File");
    gui_set_label(o->ok, save_as ? "Save" : "Open");
    snprintf(o->dir, sizeof o->dir, "%s", dir);
    snprintf(o->save_name, sizeof o->save_name, "%s", save_as ? name : "");
    open_refresh(ide);
    show_dialog(ide, o->window, 61, 21, o->name);
}

static void open_file(struct ide* ide, const char* path);
static void folder_refresh(struct ide* ide);
static void includes_add_detected(void* ctx, const char* dir);
static void dirs_add(struct ide* ide, struct gui_node* list, struct include_dirs* l, int project, const char* dir);
static void dirs_refresh(struct ide* ide, struct gui_node* list, const struct include_dirs* l, int selected);
static void dirs_edit(struct ide* ide, struct gui_node* list, struct include_dirs* l, int move);
static void project_open(struct ide* ide, const char* path);
static void project_add_file(struct ide* ide, const char* path);
static void save_doc(struct ide* ide, struct doc* d);
static struct doc* active_doc(struct ide* ide);

static void save_as_commit(struct ide* ide);

/* The name typed (or picked): a folder is entered, a file opened or saved. */
static void open_accept(struct ide* ide, const char* name)
{
    struct open_dialog* o = &ide->open;
    if (!name[0])
        return;
    char path[1400] = { 0 };
    if (strcmp(name, "..") == 0)
    {
        parent_dir(o->dir);
        gui_set_value(o->name, "");
        open_refresh(ide);
        return;
    }
    if (name[0] == '\\' || name[0] == '/' || (name[0] && name[1] == ':'))
        snprintf(path, sizeof path, "%s", name);
    else
        join_path(path, sizeof path, o->dir, name);
    if (ide_is_dir(path) && strlen(path) >= sizeof o->dir)
    {
        status(ide, "The path is too long");
        return;
    }
    if (ide_is_dir(path))
    {
        memcpy(o->dir, path, strlen(path) + 1);
        gui_set_value(o->name, "");
        open_refresh(ide);
        return;
    }
    if (o->pick_folder)
        return;   /* only folders here */
    gui_window_close(ide->app, o->window);
    if (o->pick_input)
    {
        gui_set_value(o->pick_input, path);
        return;
    }
    if (o->open_project)
    {
        project_open(ide, path);
        return;
    }
    if (o->add_to_project)
    {
        project_add_file(ide, path);
        return;
    }
    if (!o->save_as)
    {
        open_file(ide, path);
        return;
    }
    snprintf(ide->pending_path, sizeof ide->pending_path, "%s", path);
    if (ide_file_exists(path))
    {
        char msg[400] = { 0 };
        snprintf(msg, sizeof msg, "%.300s already exists.\nOverwrite?", file_name(path));
        static const char* const labels[] = { "Yes", "No" };
        static const int ids[] = { EV_SAVEAS_OVERWRITE, 0 };
        gui_message_box(ide->app, "Save As", msg, labels, ids, 2);
        return;
    }
    save_as_commit(ide);
}

/* The active document saved under pending_path. */
static void save_as_commit(struct ide* ide)
{
    struct doc* d = active_doc(ide);
    if (d && strlen(ide->pending_path) >= sizeof d->path)
    {
        static const char* const ok[] = { "OK" };
        static const int ok_id[] = { 0 };
        gui_message_box(ide->app, "Save As", "The path is too long.", ok, ok_id, 1);
    }
    else if (d)
    {
        memcpy(d->path, ide->pending_path, strlen(ide->pending_path) + 1);
        gui_set_label(d->window, file_name(d->path));
        save_doc(ide, d);
    }
}

/* The OK button (or Enter in Name). Open Folder: the folder shown becomes
 * the Folder panel's. Otherwise the name typed, or the selected row when
 * nothing is typed. */
/* `p` without trailing separators (a root keeps its own). */
static void strip_separators(char* p)
{
    size_t n = strlen(p);
    while (n > 1 && (p[n - 1] == '/' || p[n - 1] == '\\') && p[n - 2] != ':')
        p[--n] = '\0';
}

/* Add Existing File with several files picked: all of them added; 0 when
 * fewer than two are picked (one goes the usual way, Name and all). */
static int open_add_picked(struct ide* ide)
{
    struct open_dialog* o = &ide->open;
    int picked = 0;
    for (int i = 0; i < o->count; i++)
        picked += !o->entries[i].is_dir && gui_get_checked(o->list, i + 1);
    if (picked < 2)
        return 0;
    gui_window_close(ide->app, o->window);
    for (int i = 0; i < o->count; i++)
    {
        if (o->entries[i].is_dir || !gui_get_checked(o->list, i + 1))
            continue;
        char path[1400] = { 0 };
        join_path(path, sizeof path, o->dir, o->entries[i].name);
        project_add_file(ide, path);
    }
    return 1;
}

static void open_ok(struct ide* ide)
{
    struct open_dialog* o = &ide->open;
    if (o->add_to_project && open_add_picked(ide))
        return;
    const char* name = gui_get_value(o->name);
    /* Name, resolved: absolute, or in the folder shown */
    char picked[1400] = { 0 }, here[1400] = { 0 };
    if (name[0] == '\\' || name[0] == '/' || (name[0] && name[1] == ':'))
        snprintf(picked, sizeof picked, "%s", name);
    else
        join_path(picked, sizeof picked, o->dir, name);
    strip_separators(picked);
    snprintf(here, sizeof here, "%s", o->dir);
    strip_separators(here);
    int same = !name[0] || ide_path_equal(picked, here);
    if (!same && ide_is_dir(picked))
    {
        /* another folder typed: it is listed - the old IDE's */
        snprintf(o->dir, sizeof o->dir, "%s", picked);
        open_refresh(ide);
        return;
    }
    if (o->pick_folder && !same)
    {
        status(ide, "Not a folder");
        return;
    }
    if (!o->pick_folder)
    {
        if (same)
        {
            /* the folder itself in Name: the row picked in the list */
            int row = gui_get_selected(o->list);
            if (row == 0)
                open_accept(ide, "..");
            else if (row >= 1 && row <= o->count)
                open_accept(ide, o->entries[row - 1].name);
            return;
        }
        open_accept(ide, picked);
        return;
    }
    snprintf(picked, sizeof picked, "%s", o->dir);
    if (o->pick_folder)
    {
        gui_window_close(ide->app, o->window);
        if (o->pick_input)
        {
            gui_set_value(o->pick_input, picked);
            return;
        }
        if (o->pick_include)
        {
            struct copts_dialog* c = &ide->copts;
            dirs_add(ide, c->includes, &c->include_dirs, c->settings == &ide->project.compile, picked);
            return;
        }
        snprintf(ide->folder.dir, sizeof ide->folder.dir, "%s", picked);
        folder_refresh(ide);
        show_side_panel(ide, ide->folder.window);
        return;
    }
}

/* A double click or Enter on a row. */
static void open_list_pick(struct ide* ide)
{
    struct open_dialog* o = &ide->open;
    int row = gui_get_selected(o->list);
    if (row == 0)
    {
        open_accept(ide, "..");
        return;
    }
    if (row < 1 || row > o->count)
        return;
    open_accept(ide, o->entries[row - 1].name);
}

/* The folder every path dialog starts in, the same order as
 * resolve_referenced_path:
 *   1. the active document's folder
 *   2. the open project's folder
 *   3. the Folder panel's folder
 *   4. the current directory */
static void start_dir(struct ide* ide, char* out, size_t cap)
{
    struct doc* d = active_doc(ide);
    if (d)
    {
        snprintf(out, cap, "%s", d->path);
        parent_dir(out);
        return;
    }
    if (ide_project_is_open(&ide->project))
    {
        snprintf(out, cap, "%s", ide->project.dir);
        return;
    }
    if (ide->folder.dir[0])
    {
        snprintf(out, cap, "%s", ide->folder.dir);
        return;
    }
    ide_current_dir(out, (int)cap);
}

/* --- New File: the old IDE's dialog, 54 x 9 --- */

static void build_newfile(struct ide* ide)
{
    struct newfile_dialog* nf = &ide->newfile;
    nf->window = new_dialog(ide, "New File");
    add_label(ide, nf->window, 3, 2, "Folder");
    nf->folder = add_at(ide, nf->window, GUI_INPUT, 17, 2, 27, 1, NULL);
    gui_set_id(add_at(ide, nf->window, GUI_BUTTON, 45, 2, 5, 1, "..."), EV_NEWFILE_BROWSE);
    add_label(ide, nf->window, 3, 4, "File Name");
    nf->name = add_at(ide, nf->window, GUI_INPUT, 17, 4, 33, 1, NULL);
    gui_set_hint(nf->name, "Name of the new file - .c when no extension is given");
    gui_set_id(nf->name, EV_NEWFILE_OK);
    gui_set_id(add_at(ide, nf->window, GUI_BUTTON, 16, 6, 10, 1, "OK"), EV_NEWFILE_OK);
    gui_set_id(add_at(ide, nf->window, GUI_BUTTON, 28, 6, 10, 1, "Cancel"), EV_NEWFILE_CANCEL);
}

/* A .c file starts with a Hello World, a .h with #pragma once, a .md with a
 * title, anything else empty - as in the old IDE. */
static void newfile_create(struct ide* ide);
static struct doc* find_doc(struct ide* ide, const char* name);

static void newfile_accept(struct ide* ide)
{
    struct newfile_dialog* nf = &ide->newfile;
    char name[300] = { 0 };
    snprintf(name, sizeof name, "%s", gui_get_value(nf->name));
    if (!name[0])
        return;
    if (!strchr(name, '.'))
        snprintf(name + strlen(name), sizeof name - strlen(name), ".c");
    join_path(ide->pending_path, sizeof ide->pending_path, gui_get_value(nf->folder), name);
    if (ide_file_exists(ide->pending_path))
    {
        char msg[400] = { 0 };
        snprintf(msg, sizeof msg, "%s already exists.\nOverwrite?", name);
        static const char* const labels[] = { "Yes", "No" };
        static const int ids[] = { EV_NEWFILE_OVERWRITE, 0 };
        gui_message_box(ide->app, "New File", msg, labels, ids, 2);
        return;
    }
    newfile_create(ide);
}

/* Writes pending_path with a starting text for its extension and opens it;
 * an overwritten file that is open gets the new text too. */
static void newfile_create(struct ide* ide)
{
    const char* path = ide->pending_path;
    const char* name = file_name(path);
    const char* text = "";
    if (_stricmp(name, "main.c") == 0)
        text = "#include <stdio.h>\n\nint main()\n{\n    printf(\"Hello, World!\\n\");\n}\n";
    else if (ends_with(name, ".h"))
        text = "#pragma once\n";
    else if (ends_with(name, ".md"))
        text = "# Title\n";
    if (ide_write_file(path, text, 1) != 0)
    {
        char msg[1500] = { 0 };
        snprintf(msg, sizeof msg, "Cannot create %s", path);
        status(ide, msg);
        return;
    }
    gui_window_close(ide->app, ide->newfile.window);
    folder_refresh(ide);
    if (ide->newfile_in_project)
        project_add_file(ide, path);
    struct doc* open = find_doc(ide, path);
    if (open)
    {
        gui_set_value(open->editor, text);
        gui_editor_set_dirty(open->editor, 0);
    }
    open_file(ide, path);
}

/* --- Edit: To Upper / To Lower / Stringify on the selection --- */

static void transform_selection(struct ide* ide, struct gui_node* ed, int upper)
{
    int lo = 0, hi = 0;
    gui_editor_get_selection(ed, &lo, &hi);
    if (lo == hi)
    {
        status(ide, "Select some text first");
        return;
    }
    const char* text = gui_get_value(ed);
    char* copy = malloc((size_t)(hi - lo) + 1);
    if (!copy)
        return;
    for (int i = lo; i < hi; i++)
    {
        char c = text[i];   /* ASCII only - multi-byte letters stay as they are */
        if (upper && c >= 'a' && c <= 'z') c = (char)(c - 32);
        if (!upper && c >= 'A' && c <= 'Z') c = (char)(c + 32);
        copy[i - lo] = c;
    }
    copy[hi - lo] = '\0';
    gui_editor_replace(ed, lo, hi, copy);
    free(copy);
}

/* Each selected line becomes a C string literal ending in "\n". */
static void stringify_selection(struct ide* ide, struct gui_node* ed)
{
    int lo = 0, hi = 0;
    gui_editor_get_selection(ed, &lo, &hi);
    if (lo == hi)
    {
        status(ide, "Select some text first");
        return;
    }
    const char* text = gui_get_value(ed);
    char* out = malloc((size_t)(hi - lo) * 2 + 64);
    if (!out)
        return;
    size_t n = 0;
    out[n++] = '"';
    for (int i = lo; i < hi; i++)
    {
        char c = text[i];
        if (c == '\n')
        {
            memcpy(out + n, "\\n\"\n\"", 5);
            n += 5;
            continue;
        }
        if (c == '"' || c == '\\')
            out[n++] = '\\';
        out[n++] = c;
    }
    out[n++] = '"';
    out[n] = '\0';
    gui_editor_replace(ed, lo, hi, out);
    free(out);
}

/* --- Word Wrap: the old IDE's dialog, 40 x 9 --- */

/* Closed: the editor it was opened for has the focus back, its window on top. */
static void wrap_close(struct ide* ide)
{
    gui_window_close(ide->app, ide->wrap.window);
    struct gui_node* target = ide->wrap.target;
    if (!target)
        return;
    struct gui_node* win = target;
    while (win && gui_get_kind(win) != GUI_WINDOW)
        win = gui_get_parent(win);
    if (win)
        gui_window_open(ide->app, win);
    gui_focus(ide->app, target);
}

static void build_wrap(struct ide* ide)
{
    struct wrap_dialog* w = &ide->wrap;
    w->window = new_dialog(ide, "Word Wrap");
    add_label(ide, w->window, 3, 2, "Columns");
    w->columns = add_at(ide, w->window, GUI_INPUT, 11, 2, 11, 1, NULL);
    gui_set_id(w->columns, EV_WRAP_OK);
    w->justify = add_at(ide, w->window, GUI_GROUP, 3, 4, 20, 1, NULL);
    gui_set_multi(w->justify, 1);
    gui_append(w->justify, create(ide, GUI_ITEM, "Justify"));
    gui_set_id(add_at(ide, w->window, GUI_BUTTON, 9, 6, 10, 1, "OK"), EV_WRAP_OK);
    gui_set_id(add_at(ide, w->window, GUI_BUTTON, 21, 6, 10, 1, "Cancel"), EV_WRAP_CANCEL);
}

/* Rewraps the selected lines (or the paragraph around the caret) to
 * `columns`, keeping the first line's indentation; with `justify`, every
 * line but the last is padded with spaces to the full width. */
static void wrap_text(struct gui_node* ed, int columns, int justify)
{
    const char* text = gui_get_value(ed);
    int len = (int)strlen(text);
    int lo = 0, hi = 0;
    gui_editor_get_selection(ed, &lo, &hi);
    if (lo == hi)
    {
        /* The paragraph: the lines around the caret up to blank lines. */
        while (lo > 0 && !(text[lo - 1] == '\n' && (lo < 2 || text[lo - 2] == '\n')))
            lo--;
        while (hi < len && !(text[hi] == '\n' && (hi + 1 >= len || text[hi + 1] == '\n')))
            hi++;
    }
    while (lo > 0 && text[lo - 1] != '\n')
        lo--;
    int indent = 0;
    while (lo + indent < hi && text[lo + indent] == ' ')
        indent++;
    if (columns <= indent + 1)
        columns = indent + 2;

    char* out = malloc((size_t)(hi - lo) * 2 + (size_t)columns * 2 + 16);
    char* line = malloc((size_t)(hi - lo) + (size_t)columns + 16);
    if (!out || !line)
    {
        free(out);
        free(line);
        return;
    }
    size_t n = 0;
    int line_len = 0, words_in_line = 0;
    int i = lo;
    for (;;)
    {
        while (i < hi && (text[i] == ' ' || text[i] == '\n' || text[i] == '\t'))
            i++;
        int w0 = i;
        while (i < hi && text[i] != ' ' && text[i] != '\n' && text[i] != '\t')
            i++;
        int wlen = i - w0;
        int last = wlen == 0;
        if (!last && words_in_line > 0 && indent + line_len + 1 + wlen > columns)
            last = -1;   /* the line is full: flush it, then this word starts the next */
        if (last && words_in_line > 0)
        {
            int pad = justify && last == -1 ? columns - indent - line_len : 0;
            memset(out + n, ' ', (size_t)indent);
            n += (size_t)indent;
            int gaps = words_in_line - 1;
            for (int k = 0; k < line_len; k++)
            {
                out[n++] = line[k];
                if (line[k] == ' ' && gaps > 0 && pad > 0)
                {
                    int extra = (pad + gaps - 1) / gaps;
                    for (int e = 0; e < extra; e++)
                        out[n++] = ' ';
                    pad -= extra;
                    gaps--;
                }
            }
            out[n++] = '\n';
            line_len = 0;
            words_in_line = 0;
        }
        if (wlen == 0)
            break;
        if (words_in_line > 0)
            line[line_len++] = ' ';
        memcpy(line + line_len, text + w0, (size_t)wlen);
        line_len += wlen;
        words_in_line++;
    }
    if (n > 0 && (hi >= len || text[hi] == '\n'))
        n--;   /* the block's own last line end stays where it was */
    out[n] = '\0';
    gui_editor_replace(ed, lo, hi, out);
    free(out);
    free(line);
}

/* --- Find Text (56 x 16) and Replace Text (60 x 19): the old IDE's --- */

static struct gui_node* add_group_of(struct ide* ide, struct gui_node* parent, int col, int row,
                                     int cols, const char* const* items, int count, int multi)
{
    struct gui_node* g = add_at(ide, parent, GUI_GROUP, col, row, cols, count, NULL);
    gui_set_multi(g, multi);
    for (int i = 0; i < count; i++)
        gui_append(g, create(ide, GUI_ITEM, items[i]));
    return g;
}

static void build_find(struct ide* ide)
{
    static const char* const find_options[] = { "Case sensitive", "Whole words only" };
    static const char* const replace_options[] = { "Case sensitive", "Whole words only", "Prompt on replace" };
    static const char* const direction[] = { "Forward", "Backward" };
    static const char* const scope[] = { "Global", "Selected text" };
    static const char* const origin[] = { "From cursor", "Entire scope" };

    struct find_dialog* f = &ide->find;
    f->window = new_dialog(ide, "Find Text");
    add_label(ide, f->window, 2, 2, "Text to Find");
    f->input = add_at(ide, f->window, GUI_INPUT, 16, 2, 36, 1, NULL);
    gui_set_id(f->input, EV_FIND_OK);
    add_label(ide, f->window, 2, 4, "Options");
    f->options = add_group_of(ide, f->window, 2, 5, 26, find_options, 2, 1);
    gui_set_checked(f->options, 0, 1);
    add_label(ide, f->window, 30, 4, "Direction");
    f->direction = add_group_of(ide, f->window, 30, 5, 22, direction, 2, 0);
    add_label(ide, f->window, 2, 9, "Scope");
    f->scope = add_group_of(ide, f->window, 2, 10, 26, scope, 2, 0);
    add_label(ide, f->window, 30, 9, "Origin");
    f->origin = add_group_of(ide, f->window, 30, 10, 22, origin, 2, 0);
    gui_set_id(add_at(ide, f->window, GUI_BUTTON, 17, 13, 10, 1, "OK"), EV_FIND_OK);
    gui_set_id(add_at(ide, f->window, GUI_BUTTON, 29, 13, 10, 1, "Cancel"), EV_FIND_CANCEL);

    struct replace_dialog* r = &ide->replace;
    r->window = new_dialog(ide, "Replace Text");
    add_label(ide, r->window, 2, 2, "Text to Find");
    r->find = add_at(ide, r->window, GUI_INPUT, 16, 2, 40, 1, NULL);
    gui_set_id(r->find, EV_REPLACE_OK);
    add_label(ide, r->window, 4, 4, "New Text");
    r->new_text = add_at(ide, r->window, GUI_INPUT, 16, 4, 40, 1, NULL);
    gui_set_id(r->new_text, EV_REPLACE_OK);
    add_label(ide, r->window, 2, 6, "Options");
    r->options = add_group_of(ide, r->window, 2, 7, 26, replace_options, 3, 1);
    gui_set_checked(r->options, 0, 1);
    gui_set_checked(r->options, 2, 1);
    add_label(ide, r->window, 32, 6, "Direction");
    r->direction = add_group_of(ide, r->window, 32, 7, 22, direction, 2, 0);
    add_label(ide, r->window, 2, 12, "Scope");
    r->scope = add_group_of(ide, r->window, 2, 13, 26, scope, 2, 0);
    add_label(ide, r->window, 32, 12, "Origin");
    r->origin = add_group_of(ide, r->window, 32, 13, 22, origin, 2, 0);
    gui_set_id(add_at(ide, r->window, GUI_BUTTON, 11, 16, 10, 1, "OK"), EV_REPLACE_OK);
    gui_set_id(add_at(ide, r->window, GUI_BUTTON, 23, 16, 14, 1, "Change All"), EV_REPLACE_ALL);
    gui_set_id(add_at(ide, r->window, GUI_BUTTON, 39, 16, 10, 1, "Cancel"), EV_REPLACE_CANCEL);
}

static int is_word_char(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
}

static int same_char(char a, char b, int match_case)
{
    if (!match_case)
    {
        if (a >= 'A' && a <= 'Z') a = (char)(a + 32);
        if (b >= 'A' && b <= 'Z') b = (char)(b + 32);
    }
    return a == b;
}

/* The first match of `pattern` in text[from, to) going forward (or the
 * last one going backward), honoring case and whole words; -1 if none. */
static int find_in(const char* text, int from, int to, const char* pattern, int forward,
                   int match_case, int whole)
{
    int plen = (int)strlen(pattern);
    if (plen == 0 || to - from < plen)
        return -1;
    int step = forward ? 1 : -1;
    for (int at = forward ? from : to - plen; forward ? at <= to - plen : at >= from; at += step)
    {
        int k = 0;
        while (k < plen && same_char(text[at + k], pattern[k], match_case))
            k++;
        if (k < plen)
            continue;
        if (whole && ((at > 0 && is_word_char(text[at - 1])) ||
                      is_word_char(text[at + plen])))
            continue;
        return at;
    }
    return -1;
}

/* One search with the dialog's settings in the active document; selects
 * the match. `restart` searches the whole scope rather than from the caret. */
static int search(struct ide* ide, const struct search_state* st, int restart)
{
    struct doc* d = active_doc(ide);
    if (!d || !st->pattern[0])
        return 0;
    const char* text = gui_get_value(d->editor);
    int len = (int)strlen(text);
    int lo = 0, hi = 0;
    gui_editor_get_selection(d->editor, &lo, &hi);
    int from = st->selected_only ? st->scope_lo : 0;
    int to = st->selected_only ? st->scope_hi : len;
    if (!restart)
    {
        if (st->forward && hi > from) from = hi;
        if (!st->forward && lo < to) to = lo;
    }
    int at = find_in(text, from, to, st->pattern, st->forward, st->match_case, st->whole);
    if (at < 0)
    {
        status(ide, "Search string not found");
        return 0;
    }
    int line = 1;
    for (int i = 0; i < at; i++)
        if (text[i] == '\n') line++;
    gui_editor_goto_line_center(d->editor, line);   /* the match's line in the middle */
    gui_editor_set_selection(d->editor, at, at + (int)strlen(st->pattern));
    gui_focus(ide->app, d->editor);
    return 1;
}

/* The settings of the Find or Replace dialog, remembered for Search Next. */
static void read_search(struct ide* ide, struct gui_node* input, struct gui_node* options,
                        struct gui_node* direction, struct gui_node* scope, struct gui_node* origin)
{
    struct search_state* st = &ide->search;
    snprintf(st->pattern, sizeof st->pattern, "%s", gui_get_value(input));
    st->match_case = gui_get_checked(options, 0);
    st->whole = gui_get_checked(options, 1);
    st->forward = gui_get_selected(direction) == 0;
    st->selected_only = gui_get_selected(scope) == 1;
    st->from_start = gui_get_selected(origin) == 1;
    struct doc* d = active_doc(ide);
    if (d)
        gui_editor_get_selection(d->editor, &st->scope_lo, &st->scope_hi);
}

/* Replace: the next match is replaced; Change All replaces every match in
 * the scope. Returns how many were replaced. */
static int replace(struct ide* ide, int all)
{
    struct doc* d = active_doc(ide);
    struct search_state* st = &ide->search;
    if (!d || !st->pattern[0])
        return 0;
    const char* new_text = gui_get_value(ide->replace.new_text);
    int count = 0;
    int restart = st->from_start;
    while (search(ide, st, restart))
    {
        int lo = 0, hi = 0;
        gui_editor_get_selection(d->editor, &lo, &hi);
        gui_editor_replace(d->editor, lo, hi, new_text);
        int delta = (int)strlen(new_text) - (hi - lo);
        if (st->selected_only)
            st->scope_hi += delta;
        count++;
        restart = 0;
        if (!all)
            break;
    }
    char msg[100] = { 0 };
    snprintf(msg, sizeof msg, "%d occurrence(s) replaced", count);
    status(ide, msg);
    return count;
}


/* --- Context menus: the old IDE's editor and Folder panel popups --- */

static void add_popup_item(struct ide* ide, struct gui_node* menu, int id, const char* label,
                           const char* shortcut)
{
    struct gui_node* it = create(ide, GUI_ITEM, label);
    if (id == EV_NONE)
        gui_set_separator(it, 1);
    gui_set_id(it, id);
    if (shortcut)
        gui_set_shortcut(it, shortcut);
    if (id > 0 && id < EV_COUNT)
        ide->labels[id] = label;
    gui_append(menu, it);
}

static void build_popups(struct ide* ide)
{
    ide->editor_menu = create(ide, GUI_MENU, NULL);
    add_popup_item(ide, ide->editor_menu, EV_COMPILE, "Compile", NULL);
    add_popup_item(ide, ide->editor_menu, EV_SHOW_GENERATED, "Show Generated Code", NULL);
    add_popup_item(ide, ide->editor_menu, EV_TOGGLE_HDRSRC, "Switch Header/Source", NULL);
    add_popup_item(ide, ide->editor_menu, EV_NONE, NULL, NULL);
    add_popup_item(ide, ide->editor_menu, EV_FIND_DECLARATION, "Find Declaration", NULL);
    add_popup_item(ide, ide->editor_menu, EV_FIND_DEFINITION, "Find Definition", "F12");
    add_popup_item(ide, ide->editor_menu, EV_FIND_USAGES, "Find Usages", NULL);
    add_popup_item(ide, ide->editor_menu, EV_RENAME, "Rename...", "F2");
    add_popup_item(ide, ide->editor_menu, EV_EDIT_STRING, "Edit String...", NULL);
    add_popup_item(ide, ide->editor_menu, EV_FIND_IN_FILES, "Find in Files...", "Ctrl+F");
    add_popup_item(ide, ide->editor_menu, EV_NONE, NULL, NULL);
    add_popup_item(ide, ide->editor_menu, EV_TOGGLE_READONLY, "[ ] Read-only", NULL);
    add_popup_item(ide, ide->editor_menu, EV_NONE, NULL, NULL);
    add_popup_item(ide, ide->editor_menu, EV_COPY_PATH, "Copy Full Path", NULL);
    add_popup_item(ide, ide->editor_menu, EV_SHOW_FOLDER, "Show My Folder", NULL);
    add_popup_item(ide, ide->editor_menu, EV_FORMAT, "Format", NULL);
    if (gui_can_detach(ide->app))
    {
        add_popup_item(ide, ide->editor_menu, EV_NONE, NULL, NULL);
        add_popup_item(ide, ide->editor_menu, EV_TOGGLE_DETACH, "Detach Window", NULL);
    }

    gui_set_id(ide->editor_menu, EV_EDITOR_MENU);

    ide->folder_menu = create(ide, GUI_MENU, NULL);
    add_popup_item(ide, ide->folder_menu, EV_FOLDER_COPY_PATH, "Copy Full Path", NULL);
    add_popup_item(ide, ide->folder_menu, EV_NEW_FILE, "New File...", NULL);
    add_popup_item(ide, ide->folder_menu, EV_FOLDER_NEW_FOLDER, "New Folder...", NULL);
    add_popup_item(ide, ide->folder_menu, EV_FOLDER_ADD_TO_PROJECT, "Add to Project", NULL);
    add_popup_item(ide, ide->folder_menu, EV_FOLDER_DELETE, "Delete", NULL);
}

static struct doc* doc_of_editor(struct ide* ide, const struct gui_node* ed)
{
    for (int i = 0; i < ide->doc_count; i++)
    {
        if (ide->docs[i].editor == ed)
            return &ide->docs[i];
    }
    return NULL;
}

/* The editor popup opens over a document: Read-only shows its state, and
 * the C-only items are enabled only for a .c or .h file. */
static void editor_menu_refresh(struct ide* ide)
{
    struct doc* d = doc_of_editor(ide, gui_context_target(ide->app));
    int source = d && (ends_with(d->path, ".c") || ends_with(d->path, ".h"));
    static const int source_ids[] = {
        EV_COMPILE, EV_SHOW_GENERATED, EV_FIND_DECLARATION, EV_FIND_DEFINITION, EV_FIND_USAGES,
        EV_RENAME, EV_EDIT_STRING, EV_TOGGLE_HDRSRC, EV_FORMAT,
    };
    for (int i = 0; i < gui_child_count(ide->editor_menu); i++)
    {
        struct gui_node* it = gui_child_at(ide->editor_menu, i);
        for (int k = 0; k < COUNT(source_ids); k++)
        {
            if (ide->labels[source_ids[k]] && gui_get_label(it) &&
                strcmp(gui_get_label(it), ide->labels[source_ids[k]]) == 0)
                gui_set_enabled(it, source);
        }
        if (gui_get_label(it) && strstr(gui_get_label(it), "Read-only"))
        {
            gui_set_label(it, d && gui_editor_get_read_only(d->editor) ? "[x] Read-only" : "[ ] Read-only");
            gui_set_enabled(it, d != NULL);
        }
        if (gui_get_label(it) && strstr(gui_get_label(it), " Window") &&
            (strncmp(gui_get_label(it), "Detach", 6) == 0 || strncmp(gui_get_label(it), "Attach", 6) == 0))
        {
            gui_set_label(it, d && gui_window_get_detached(ide->app, d->window) ? "Attach Window" : "Detach Window");
            gui_set_enabled(it, d != NULL);
        }
    }
}

/* Switch Header/Source: x.c <-> x.h, opened when it exists. */
static void toggle_header_source(struct ide* ide, struct doc* d)
{
    char other[1024] = { 0 };
    snprintf(other, sizeof other, "%s", d->path);
    size_t n = strlen(other);
    if (n > 2 && other[n - 2] == '.' && (other[n - 1] == 'c' || other[n - 1] == 'h'))
    {
        other[n - 1] = other[n - 1] == 'c' ? 'h' : 'c';
        if (ide_file_exists(other))
        {
            open_file(ide, other);
            return;
        }
    }
    status(ide, "No matching header/source file");
}

/* Show My Folder: the Folder panel goes to the document's folder. */
static void show_folder_of(struct ide* ide, const char* path)
{
    snprintf(ide->folder.dir, sizeof ide->folder.dir, "%s", path);
    parent_dir(ide->folder.dir);
    folder_refresh(ide);
    /* the file itself selected - its row past ".." */
    const char* name = file_name(path);
    for (int i = 0; i < ide->folder.count; i++)
    {
        if (!ide->folder.entries[i].is_dir && ide_path_equal(ide->folder.entries[i].name, name))
            gui_set_selected(ide->folder.list, i + 1);
    }
    show_side_panel(ide, ide->folder.window);
}

/* --- Rename: 50 x 8 --- */

static void build_rename(struct ide* ide)
{
    struct rename_dialog* r = &ide->rename;
    r->window = new_dialog(ide, "Rename");
    add_label(ide, r->window, 2, 2, "New name");
    r->input = add_at(ide, r->window, GUI_INPUT, 12, 2, 36, 1, NULL);
    gui_set_id(r->input, EV_RENAME_OK);
    gui_set_id(add_at(ide, r->window, GUI_BUTTON, 14, 5, 10, 1, "OK"), EV_RENAME_OK);
    gui_set_id(add_at(ide, r->window, GUI_BUTTON, 26, 5, 10, 1, "Cancel"), EV_RENAME_CANCEL);
}

/* --- Rename Project: 50 x 8 --- */

static void build_project_rename(struct ide* ide)
{
    struct project_rename_dialog* r = &ide->project_rename;
    r->window = new_dialog(ide, "Rename Project");
    add_label(ide, r->window, 2, 2, "New name");
    r->input = add_at(ide, r->window, GUI_INPUT, 12, 2, 36, 1, NULL);
    gui_set_hint(r->input, "Name of the project file: <name>.cakeproj");
    gui_set_id(r->input, EV_PROJ_RENAME_OK);
    gui_set_id(add_at(ide, r->window, GUI_BUTTON, 14, 5, 10, 1, "OK"), EV_PROJ_RENAME_OK);
    gui_set_id(add_at(ide, r->window, GUI_BUTTON, 26, 5, 10, 1, "Cancel"), EV_PROJ_RENAME_CANCEL);
}

/* The identifier under the caret, into `out` (empty when there is none). */
static void word_at_caret(struct gui_node* ed, char* out, size_t cap)
{
    const char* text = gui_get_value(ed);
    int lo = 0, hi = 0;
    gui_editor_get_selection(ed, &lo, &hi);
    while (lo > 0 && (isalnum((unsigned char)text[lo - 1]) || text[lo - 1] == '_'))
        lo--;
    while (text[hi] && (isalnum((unsigned char)text[hi]) || text[hi] == '_'))
        hi++;
    size_t n = (size_t)(hi - lo);
    if (n >= cap)
        n = cap - 1;
    memcpy(out, text + lo, n);
    out[n] = '\0';
}

/* --- Edit String: 70 x 18, the string literal under the caret --- */

static void build_edit_string(struct ide* ide)
{
    struct edit_string_dialog* e = &ide->estr;
    e->window = new_dialog(ide, "Edit String");
    gui_window_set_modal(e->window, 0);   /* the other tools - Word Wrap - work on its text */
    gui_window_set_resizable(e->window, 1);
    gui_window_set_min_size(e->window, 40, 8);
    e->editor = create(ide, GUI_EDITOR, NULL);
    fill_margins(e->editor, 2, 1, 2, 4);
    gui_append(e->window, e->editor);
    gui_editor_set_highlighter(e->editor, &ide->string_highlighter);
    /* OK and Cancel centered as a pair */
    static const struct { int id; const char* label; int offset; } bottom[] = {
        { EV_ESTR_OK, "OK", -11 }, { EV_ESTR_CANCEL, "Cancel", 1 },
    };
    for (int i = 0; i < COUNT(bottom); i++)
    {
        struct gui_node* n = create(ide, GUI_BUTTON, bottom[i].label);
        struct gui_layout l = { GUI_ANCHOR_LEFT | GUI_ANCHOR_BOTTOM };
        l.left.percent = 50;
        l.left.cells = bottom[i].offset;
        l.bottom.cells = 2;
        l.width.cells = 10;
        l.height.cells = 1;
        gui_set_layout(n, &l);
        gui_set_id(n, bottom[i].id);
        gui_append(e->window, n);
    }
}

/* The closing quote of the "..." whose opening quote is at `open`; -1 when
 * the line ends first. */
static int literal_close(const char* text, int open)
{
    int i = open + 1;
    while (text[i] && text[i] != '\n' && text[i] != '"')
        i += (text[i] == '\\' && text[i + 1] && text[i + 1] != '\n') ? 2 : 1;
    return text[i] == '"' ? i : -1;
}

/* The opening quote of the "..." on `pos`'s line that holds `pos` - its
 * quotes included - or -1. Skips '...' and stops at a // comment. */
static int literal_around(const char* text, int pos)
{
    int i = pos;
    while (i > 0 && text[i - 1] != '\n')
        i--;
    while (text[i] && text[i] != '\n')
    {
        if (text[i] == '\'' || text[i] == '"')
        {
            int close = -1;
            if (text[i] == '"')
                close = literal_close(text, i);
            else
            {
                int k = i + 1;
                while (text[k] && text[k] != '\n' && text[k] != '\'')
                    k += (text[k] == '\\' && text[k + 1] && text[k + 1] != '\n') ? 2 : 1;
                if (text[k] == '\'')
                    close = k;
            }
            if (close < 0)
                return -1;
            if (text[i] == '"' && pos >= i && pos <= close)
                return i;
            i = close + 1;
            continue;
        }
        if (text[i] == '/' && text[i + 1] == '/')
            break;
        i++;
    }
    return -1;
}

/* The run of adjacent "..." "..." around the caret - only blanks and line
 * ends between them, as C concatenates them: [*lo, *hi) from the first
 * opening quote to past the last closing one. 0 when the caret is in none. */
static int string_at_caret(struct gui_node* ed, int* lo, int* hi)
{
    const char* text = gui_get_value(ed);
    int caret = 0, unused = 0;
    gui_editor_get_selection(ed, &caret, &unused);
    int open = literal_around(text, caret);
    if (open < 0 && caret > 0)
        open = literal_around(text, caret - 1);   /* just past the closing quote */
    if (open < 0)
        return 0;
    int first = open, last = literal_close(text, open);
    for (;;)
    {
        int k = first;
        while (k > 0 && (text[k - 1] == ' ' || text[k - 1] == '\t' || text[k - 1] == '\r' || text[k - 1] == '\n'))
            k--;
        if (k == 0 || text[k - 1] != '"')
            break;
        int prev = literal_around(text, k - 1);
        if (prev < 0 || literal_close(text, prev) != k - 1)
            break;
        first = prev;
    }
    for (;;)
    {
        int k = last + 1;
        while (text[k] == ' ' || text[k] == '\t' || text[k] == '\r' || text[k] == '\n')
            k++;
        if (text[k] != '"')
            break;
        int next = literal_close(text, k);
        if (next < 0)
            break;
        last = next;
    }
    *lo = first;
    *hi = last + 1;
    return 1;
}

static void edit_string_open(struct ide* ide, struct gui_node* ed)
{
    struct edit_string_dialog* e = &ide->estr;
    struct doc* d = NULL;
    for (int i = 0; i < ide->doc_count && !d; i++)
    {
        if (ide->docs[i].editor == ed)
            d = &ide->docs[i];
    }
    if (!d)
    {
        status(ide, "Edit String works on a document");
        return;
    }
    int lo = 0, hi = 0;
    if (!string_at_caret(ed, &lo, &hi))
    {
        status(ide, "No string literal under the caret");
        return;
    }
    const char* text = gui_get_value(ed);
    char* plain = malloc((size_t)(hi - lo) + 1);
    if (!plain)
        return;
    size_t n = 0;
    /* each literal's contents, decoded, one after the other */
    for (int i = lo; i < hi;)
    {
        if (text[i] != '"')
        {
            i++;
            continue;
        }
        int close = literal_close(text, i);
        for (int k = i + 1; k < close; k++)
        {
            if (text[k] != '\\' || k + 1 >= close)
            {
                plain[n++] = text[k];
                continue;
            }
            char c = text[++k];
            plain[n++] = c == 'n' ? '\n' : c == 't' ? '\t' : c;
        }
        i = close + 1;
    }
    plain[n] = '\0';
    gui_set_value(e->editor, plain);
    free(plain);
    snprintf(e->path, sizeof e->path, "%s", d->path);
    e->lo = lo;
    e->hi = hi;
    struct ide_text title = { 0 };
    ide_text_printf(&title, "Edit String - %s", file_name(d->path));
    gui_set_label(e->window, title.data ? title.data : "Edit String");
    free(title.data);
    show_resizable_dialog(ide, e->window, 70, 18, e->editor);
}

/* The document may have changed since Edit String opened: [lo, hi) must
 * still begin with a '"' and be exactly one run of adjacent literals -
 * none just before it, none just after. */
static int edit_string_still_there(const char* text, int lo, int hi)
{
    int len = (int)strlen(text);
    if (hi > len || text[lo] != '"' || text[hi - 1] != '"')
        return 0;
    int k = lo;
    while (k > 0 && (text[k - 1] == ' ' || text[k - 1] == '\t' || text[k - 1] == '\r' || text[k - 1] == '\n'))
        k--;
    if (k > 0 && text[k - 1] == '"')
        return 0;
    int close = literal_close(text, lo);
    for (;;)
    {
        if (close < 0)
            return 0;
        int next = close + 1;
        while (text[next] == ' ' || text[next] == '\t' || text[next] == '\r' || text[next] == '\n')
            next++;
        if (text[next] != '"')
            break;
        close = literal_close(text, next);
    }
    return close == hi - 1;
}

/* `enc` (already escaped, `len` bytes) as "..." literals of at most `room`
 * bytes each, cut after a space - never inside an escape; each one after
 * the very first starts on a new line, `prefix` (a line end and indent). */
static void append_split_literal(struct ide_text* out, const char* enc, int len, int room,
                                 const char* prefix, int* first)
{
    int i = 0;
    do
    {
        int cut = len - i;
        if (cut > room)
        {
            int space = -1;
            for (int k = i; k < len && k - i < room; k++)
            {
                if (enc[k] == '\\')
                    k++;
                else if (enc[k] == ' ')
                    space = k + 1;
            }
            if (space > i)
                cut = space - i;
        }
        if (!*first)
            ide_text_append(out, prefix, strlen(prefix));
        *first = 0;
        ide_text_append(out, "\"", 1);
        ide_text_append(out, enc + i, (size_t)cut);
        ide_text_append(out, "\"", 1);
        i += cut;
    } while (i < len);
}

static void edit_string_accept(struct ide* ide)
{
    struct edit_string_dialog* e = &ide->estr;
    static const char* const ok[] = { "OK" };
    static const int ok_id[] = { 0 };
    struct doc* d = NULL;
    for (int i = 0; i < ide->doc_count && !d; i++)
    {
        if (ide_path_equal(ide->docs[i].path, e->path))
            d = &ide->docs[i];
    }
    if (!d || !edit_string_still_there(gui_get_value(d->editor), e->lo, e->hi))
    {
        gui_message_box(ide->app, "Edit String",
                        d ? "The string moved in the document since Edit String opened - it can no longer be applied."
                          : "The document was closed.",
                        ok, ok_id, 1);
        return;
    }
    gui_window_close(ide->app, e->window);
    struct gui_node* target = d->editor;
    const char* plain = gui_get_value(e->editor);
    /* one literal per line of the text - split at spaces to stay within
     * EDIT_STRING_COLUMNS - each on its own source line under the first
     * one's opening quote */
    const char* src = gui_get_value(target);
    int line_start = e->lo;
    while (line_start > 0 && src[line_start - 1] != '\n')
        line_start--;
    struct ide_text prefix = { 0 }, out = { 0 }, enc = { 0 };
    ide_text_append(&prefix, "\n", 1);
    for (int k = line_start; k < e->lo; k++)
        ide_text_append(&prefix, src[k] == '\t' ? "\t" : " ", 1);
    int room = EDIT_STRING_COLUMNS - (e->lo - line_start) - 2;
    if (room < 20)
        room = 20;
    int first = 1;
    for (const char* p = plain;; p++)
    {
        if (*p == '\0' || *p == '\n')
        {
            if (*p == '\n')
                ide_text_append(&enc, "\\n", 2);
            if (enc.len > 0 || first)
                append_split_literal(&out, enc.data ? enc.data : "", (int)enc.len, room,
                                     prefix.data ? prefix.data : "\n", &first);
            enc.len = 0;
            if (*p == '\0' || p[1] == '\0')
                break;
            continue;
        }
        switch (*p)
        {
        case '\t': ide_text_append(&enc, "\\t", 2); break;
        case '\\': ide_text_append(&enc, "\\\\", 2); break;
        case '"': ide_text_append(&enc, "\\\"", 2); break;
        default: ide_text_append(&enc, p, 1); break;
        }
    }
    if (out.data)
        gui_editor_replace(target, e->lo, e->hi, out.data);
    free(out.data);
    free(enc.data);
    free(prefix.data);
}

/* --- New Folder: 44 x 8, in the Folder panel's folder --- */

static void build_new_folder(struct ide* ide)
{
    struct new_folder_dialog* f = &ide->new_folder;
    f->window = new_dialog(ide, "New Folder");
    add_label(ide, f->window, 3, 2, "Name");
    f->input = add_at(ide, f->window, GUI_INPUT, 14, 2, 26, 1, NULL);
    gui_set_id(f->input, EV_NEWFOLDER_OK);
    gui_set_id(add_at(ide, f->window, GUI_BUTTON, 11, 5, 10, 1, "OK"), EV_NEWFOLDER_OK);
    gui_set_id(add_at(ide, f->window, GUI_BUTTON, 23, 5, 10, 1, "Cancel"), EV_NEWFOLDER_CANCEL);
}

static void new_folder_accept(struct ide* ide)
{
    const char* name = gui_get_value(ide->new_folder.input);
    if (!name[0])
        return;
    char path[1400] = { 0 };
    join_path(path, sizeof path, ide->folder.dir, name);
    gui_window_close(ide->app, ide->new_folder.window);
    char msg[1500] = { 0 };
    if (ide_make_dir(path) == 0)
    {
        folder_refresh(ide);
        snprintf(msg, sizeof msg, "Created %s", path);
    }
    else
    {
        snprintf(msg, sizeof msg, "Cannot create %s", path);
    }
    status(ide, msg);
    output(ide, msg);
}

/* Delete on the Folder panel's selected row: a file, or an empty folder,
 * after a confirmation that names it. */
static void folder_delete_ask(struct ide* ide)
{
    struct folder_panel* f = &ide->folder;
    int row = gui_get_selected(f->list);
    if (row < 1 || row > f->count)
        return;
    const struct ide_dir_entry* e = &f->entries[row - 1];
    join_path(f->pending_delete, sizeof f->pending_delete, f->dir, e->name);
    f->pending_is_dir = e->is_dir;
    char msg[600] = { 0 };
    snprintf(msg, sizeof msg, "Are you sure you want to delete this %s?\n%s",
             e->is_dir ? "folder" : "file", e->name);
    static const char* const labels[] = { "OK", "Cancel" };
    static const int ids[] = { EV_FOLDER_DELETE_OK, 0 };
    gui_message_box(ide->app, e->is_dir ? "Delete Folder" : "Delete File", msg, labels, ids, 2);
}

static void folder_delete_confirmed(struct ide* ide)
{
    struct folder_panel* f = &ide->folder;
    char msg[1500] = { 0 };
    if (ide_delete_path(f->pending_delete, f->pending_is_dir) == 0)
    {
        folder_refresh(ide);
        snprintf(msg, sizeof msg, "Deleted %s", f->pending_delete);
    }
    else
    {
        snprintf(msg, sizeof msg, f->pending_is_dir ? "Cannot delete %s (is it empty?)" : "Cannot delete %s",
                 f->pending_delete);
    }
    status(ide, msg);
    output(ide, msg);
}

/* --- Git Clone: 58 x 13 --- */

static void build_git_clone(struct ide* ide)
{
    struct git_clone_dialog* g = &ide->clone;
    g->window = new_dialog(ide, "Clone Repository");
    add_label(ide, g->window, 3, 2, "Repository location");
    g->url = add_at(ide, g->window, GUI_INPUT, 3, 3, 52, 1, NULL);
    gui_set_id(g->url, EV_CLONE_OK);
    gui_set_change_id(g->url, EV_CLONE_URL_CHANGED);
    add_label(ide, g->window, 3, 5, "Path");
    g->path = add_at(ide, g->window, GUI_INPUT, 3, 6, 46, 1, NULL);
    gui_set_id(g->path, EV_CLONE_OK);
    gui_set_change_id(g->path, EV_CLONE_PATH_CHANGED);
    gui_set_id(add_at(ide, g->window, GUI_BUTTON, 50, 6, 5, 1, "..."), EV_CLONE_BROWSE);
    g->open_folder = add_at(ide, g->window, GUI_GROUP, 3, 8, 20, 1, NULL);
    gui_set_multi(g->open_folder, 1);
    gui_append(g->open_folder, create(ide, GUI_ITEM, "Open Folder"));
    gui_set_checked(g->open_folder, 0, 1);
    gui_set_id(add_at(ide, g->window, GUI_BUTTON, 17, 10, 10, 1, "OK"), EV_CLONE_OK);
    gui_set_id(add_at(ide, g->window, GUI_BUTTON, 29, 10, 10, 1, "Cancel"), EV_CLONE_CANCEL);
}

/* --- New Project: 54 x 12 --- */

static void build_new_project(struct ide* ide)
{
    struct new_project_dialog* p = &ide->new_project;
    p->window = new_dialog(ide, "New Project");
    add_label(ide, p->window, 3, 2, "Folder");
    p->folder = add_at(ide, p->window, GUI_INPUT, 17, 2, 27, 1, NULL);
    gui_set_id(add_at(ide, p->window, GUI_BUTTON, 45, 2, 5, 1, "..."), EV_NEWPROJ_BROWSE);
    add_label(ide, p->window, 3, 4, "Project Name");
    p->name = add_at(ide, p->window, GUI_INPUT, 17, 4, 33, 1, NULL);
    gui_set_id(p->name, EV_NEWPROJ_OK);
    p->checks = add_at(ide, p->window, GUI_GROUP, 17, 6, 20, 2, NULL);
    gui_set_multi(p->checks, 1);
    gui_append(p->checks, create(ide, GUI_ITEM, "Create Folder"));
    gui_append(p->checks, create(ide, GUI_ITEM, "Hello World"));
    gui_set_id(add_at(ide, p->window, GUI_BUTTON, 16, 9, 10, 1, "OK"), EV_NEWPROJ_OK);
    gui_set_id(add_at(ide, p->window, GUI_BUTTON, 28, 9, 10, 1, "Cancel"), EV_NEWPROJ_CANCEL);
}

/* --- Properties: the project's options, or the Playground project's --- */

static const char* const copts_headers[] = { "System Headers", "Cake Headers" };
static const char* const copts_styles[] = { "disabled", "cake", "gnu", "microsoft" };
static const char* const copts_diags[] = { "cake ide", "gcc", "msvc" };
static const char* const copts_flags[] = {
    "-line-directives", "-flow", "-const-literal", "-Wall",
    "-default-nonnull", "-check-annotations",
};

static struct gui_node* add_select_of(struct ide* ide, struct gui_node* parent, int col, int row,
                                      int cols, const char* const* items, int count)
{
    struct gui_node* s = add_at(ide, parent, GUI_SELECT, col, row, cols, 1, NULL);
    for (int i = 0; i < count; i++)
        gui_append(s, create(ide, GUI_ITEM, items[i]));
    gui_set_selected(s, 0);
    return s;
}

/* Project Properties: the target being edited on top, the pages on the
 * left, the selected page on the right - Visual Studio's Property Pages. */
static const char* const copts_pages[COPTS_PAGES] = { "Compiler", "Includes", "Build", "Debugger" };

/* The dialog is resizable: fields grow to the right, `right` cells from the edge. */
#define COPTS_COLS 75
#define COPTS_ROWS 23

static void stretch_right(struct gui_node* n, int col, int row, int right, int rows)
{
    struct gui_layout l = { GUI_ANCHOR_LEFT | GUI_ANCHOR_TOP | GUI_ANCHOR_RIGHT };
    l.left.cells = col;
    l.top.cells = row;
    l.right.cells = right;
    l.height.cells = rows;
    gui_set_layout(n, &l);
}

static void pin_right(struct gui_node* n, int row, int right, int cols)
{
    struct gui_layout l = { GUI_ANCHOR_RIGHT | GUI_ANCHOR_TOP };
    l.right.cells = right;
    l.top.cells = row;
    l.width.cells = cols;
    l.height.cells = 1;
    gui_set_layout(n, &l);
}

/* The window's nodes from `first` on are page `page`'s. */
static void copts_page_take(struct copts_dialog* c, int page, int first)
{
    int n = gui_child_count(c->window);
    for (int i = first; i < n && c->page_node_count[page] < COUNT(c->page_nodes[page]); i++)
        c->page_nodes[page][c->page_node_count[page]++] = gui_child_at(c->window, i);
}

static void build_compiler_options(struct ide* ide)
{
    struct copts_dialog* c = &ide->copts;
    c->window = new_dialog(ide, "Properties");
    gui_window_set_resizable(c->window, 1);
    gui_window_set_min_size(c->window, COPTS_COLS, COPTS_ROWS);
    struct gui_node* configs_label = add_label(ide, c->window, 2, 2, "Configuration");
    c->configs = add_at(ide, c->window, GUI_SELECT, 16, 2, 25, 1, NULL);   /* filled by copts_fill_configs */
    stretch_right(c->configs, 16, 2, 34, 1);
    gui_set_after_label(c->configs, configs_label);
    /* on the right, Delete ending where the fields below end */
    static const char* const config_labels[] = { "New...", "Rename...", "Delete" };
    static const int config_ids[] = { EV_COPTS_CONFIG_NEW, EV_COPTS_CONFIG_RENAME, EV_COPTS_CONFIG_DELETE };
    static const int config_right[][2] = { { 24, 8 }, { 12, 11 }, { 3, 8 } };   /* cells from the edge, width */
    for (int i = 0; i < 3; i++)
    {
        c->config_buttons[i] = add_at(ide, c->window, GUI_BUTTON, 0, 2, config_right[i][1], 1, config_labels[i]);
        pin_right(c->config_buttons[i], 2, config_right[i][0], config_right[i][1]);
        gui_set_id(c->config_buttons[i], config_ids[i]);
    }
    c->pages = add_at(ide, c->window, GUI_LISTBOX, 2, 4, 14, 15, NULL);
    {
        struct gui_layout l = { GUI_ANCHOR_LEFT | GUI_ANCHOR_TOP | GUI_ANCHOR_BOTTOM };
        l.left.cells = 2;
        l.top.cells = 4;
        l.bottom.cells = 4;
        l.width.cells = 14;
        gui_set_layout(c->pages, &l);
    }
    for (int i = 0; i < COUNT(copts_pages); i++)
    {
        struct gui_node* it = create(ide, GUI_ITEM, copts_pages[i]);
        gui_set_id(it, EV_COPTS_PAGE);
        gui_append(c->pages, it);
    }
    gui_set_selected(c->pages, 0);
    int first = gui_child_count(c->window);
    /* the Compiler page */
    struct gui_node* cake_target_label = add_label(ide, c->window, 18, 4, "Target");
    c->cake_target = add_at(ide, c->window, GUI_INPUT, 30, 4, 28, 1, NULL);
    stretch_right(c->cake_target, 30, 4, 3, 1);
    gui_set_after_label(c->cake_target, cake_target_label);
    struct gui_node* style_label = add_label(ide, c->window, 18, 6, "Style");
    c->style = add_select_of(ide, c->window, 30, 6, 28, copts_styles, COUNT(copts_styles));
    stretch_right(c->style, 30, 6, 3, 1);
    gui_set_after_label(c->style, style_label);
    struct gui_node* diagnostic_label = add_label(ide, c->window, 18, 8, "Diagnostic");
    c->diag = add_select_of(ide, c->window, 30, 8, 28, copts_diags, COUNT(copts_diags));
    stretch_right(c->diag, 30, 8, 3, 1);
    gui_set_after_label(c->diag, diagnostic_label);
    struct gui_node* flags_label = add_label(ide, c->window, 18, 10, "Flags");
    c->flags = add_group_of(ide, c->window, 30, 10, 28, copts_flags, COUNT(copts_flags), 1);
    gui_set_after_label(c->flags, flags_label);
    struct gui_node* options_label = add_label(ide, c->window, 18, 17, "Options");
    c->options = add_at(ide, c->window, GUI_INPUT, 30, 17, 28, 1, NULL);
    stretch_right(c->options, 30, 17, 3, 1);
    gui_set_after_label(c->options, options_label);
    copts_page_take(c, 0, first);
    /* the Includes page: the target's #include search path */
    first = gui_child_count(c->window);
    struct gui_node* headers_label = add_label(ide, c->window, 18, 4, "Headers");
    c->headers = add_select_of(ide, c->window, 30, 4, 28, copts_headers, COUNT(copts_headers));
    stretch_right(c->headers, 30, 4, 3, 1);
    gui_set_after_label(c->headers, headers_label);
    add_label(ide, c->window, 18, 6, "Include Directories");
    c->includes = add_at(ide, c->window, GUI_LISTBOX, 18, 7, 25, 12, NULL);
    fill_margins(c->includes, 18, 7, 17, 4);
    static const char* const inc_labels[] = { "Add...", "Remove", "Move Up", "Move Down" };
    static const int inc_ids[] = { EV_COPTS_INC_ADD, EV_COPTS_INC_REMOVE, EV_COPTS_INC_UP, EV_COPTS_INC_DOWN };
    for (int i = 0; i < 4; i++)
    {
        struct gui_node* b = add_at(ide, c->window, GUI_BUTTON, 0, 7 + 2 * i, 13, 1, inc_labels[i]);
        pin_right(b, 7 + 2 * i, 3, 13);
        gui_set_id(b, inc_ids[i]);
    }
    copts_page_take(c, 1, first);
    /* the Build page: the Output, and the Pre-Build and Post-Build Events -
     * commands run before a Build (F7, or F5's) and after it ends without errors */
    first = gui_child_count(c->window);
    struct gui_node* output_label = add_label(ide, c->window, 18, 4, "Output");
    c->output = add_at(ide, c->window, GUI_INPUT, 30, 4, 28, 1, NULL);
    stretch_right(c->output, 30, 4, 3, 1);
    gui_set_after_label(c->output, output_label);
    /* the Pre-Build Event is kept and run, but not shown for now */
    static const char* const event_labels[] = { "Command", "Arguments", "Directory" };
    for (int i = 0; i < 3; i++)
    {
        int row = 6 + 2 * i;
        struct gui_node* label = add_label(ide, c->window, 18, row, event_labels[i]);
        c->post_build[i] = add_at(ide, c->window, GUI_INPUT, 30, row, 22, 1, NULL);
        stretch_right(c->post_build[i], 30, row, 9, 1);
        gui_set_after_label(c->post_build[i], label);
        struct gui_node* b = i == 0 ? add_at(ide, c->window, GUI_BUTTON, 53, row, 5, 1, "...")
                                    : add_macro_button(ide, c->window, 53, row, c->post_build[i]);
        if (i == 0)
            gui_set_id(b, EV_POST_BUILD_BROWSE);
        if (b)
            pin_right(b, row, 3, 5);
    }
    copts_page_take(c, 2, first);
    /* the Debugger page */
    first = gui_child_count(c->window);
#ifdef _WIN32
    static const char* const debuggers[] = { "cdb" };
#else
    static const char* const debuggers[] = { "lldb" };
#endif
    static const char* const debug_labels[] = { "Command", "Arguments", "Directory" };
    struct gui_node* debugger_label = add_label(ide, c->window, 18, 4, "Debugger");
    c->debugger = add_select_of(ide, c->window, 30, 4, 22, debuggers, COUNT(debuggers));
    gui_set_after_label(c->debugger, debugger_label);
    for (int i = 0; i < 3; i++)
    {
        int row = 6 + i * 2;
        struct gui_node* label = add_label(ide, c->window, 18, row, debug_labels[i]);
        c->debug[i] = add_at(ide, c->window, GUI_INPUT, 30, row, 22, 1, NULL);
        stretch_right(c->debug[i], 30, row, 9, 1);
        gui_set_after_label(c->debug[i], label);
        struct gui_node* b = i == 0 ? add_at(ide, c->window, GUI_BUTTON, 53, row, 5, 1, "...")
                                    : add_macro_button(ide, c->window, 53, row, c->debug[i]);
        if (i == 0)
            gui_set_id(b, EV_DBG_BROWSE);
        if (b)
            pin_right(b, row, 3, 5);
    }
    copts_page_take(c, 3, first);
    first = gui_child_count(c->window);
    gui_set_id(add_at(ide, c->window, GUI_BUTTON, 24, 20, 10, 1, "OK"), EV_COPTS_OK);
    gui_set_id(add_at(ide, c->window, GUI_BUTTON, 36, 20, 10, 1, "Cancel"), EV_COPTS_CANCEL);
    gui_set_id(add_at(ide, c->window, GUI_BUTTON, 48, 20, 10, 1, "Help"), EV_COPTS_HELP);
    /* Auto Config, bottom left: kept with the buttons, on every page */
    c->name_window = new_dialog(ide, "Configuration");
    add_label(ide, c->name_window, 2, 2, "Name");
    c->name_input = add_at(ide, c->name_window, GUI_INPUT, 12, 2, 36, 1, NULL);
    gui_set_id(c->name_input, EV_CONFIG_NAME_OK);
    gui_set_id(add_at(ide, c->name_window, GUI_BUTTON, 14, 5, 10, 1, "OK"), EV_CONFIG_NAME_OK);
    gui_set_id(add_at(ide, c->name_window, GUI_BUTTON, 26, 5, 10, 1, "Cancel"), EV_CONFIG_NAME_CANCEL);
    struct gui_node* auto_config = add_at(ide, c->window, GUI_BUTTON, 2, 20, 14, 1, "Auto Config");
    gui_set_id(auto_config, EV_COPTS_AUTO_CONFIG);
    {
        struct gui_layout l = { GUI_ANCHOR_LEFT | GUI_ANCHOR_BOTTOM };
        l.left.cells = 2;
        l.bottom.cells = 2;
        l.width.cells = 14;
        l.height.cells = 1;
        gui_set_layout(auto_config, &l);
    }
    c->buttons[3] = auto_config;
    for (int i = 0; i < 3; i++)
    {
        c->buttons[i] = gui_child_at(c->window, first + i);
        struct gui_layout l = { GUI_ANCHOR_RIGHT | GUI_ANCHOR_BOTTOM };
        l.right.cells = 27 - 12 * i;
        l.bottom.cells = 2;
        l.width.cells = 10;
        l.height.cells = 1;
        gui_set_layout(c->buttons[i], &l);
    }
    c->page = 0;
    for (int page = 1; page < COPTS_PAGES; page++)
    {
        for (int i = 0; i < c->page_node_count[page]; i++)
            gui_remove(c->window, c->page_nodes[page][i]);
    }
}

/* Shows page `page`: its nodes put back before the buttons, the other's taken out. */
static void copts_select_page(struct copts_dialog* c, int page)
{
    gui_set_selected(c->pages, page);
    if (page == c->page)
        return;
    for (int i = 0; i < c->page_node_count[c->page]; i++)
        gui_remove(c->window, c->page_nodes[c->page][i]);
    for (int i = 0; i < COUNT(c->buttons); i++)
        gui_remove(c->window, c->buttons[i]);
    for (int i = 0; i < c->page_node_count[page]; i++)
        gui_append(c->window, c->page_nodes[page][i]);
    for (int i = 0; i < COUNT(c->buttons); i++)
        gui_append(c->window, c->buttons[i]);
    c->page = page;
}

/* The dialog shows the options of the selected configuration; each keeps its own. */
static void copts_show(struct ide* ide)
{
    struct copts_dialog* c = &ide->copts;
    static const struct target_settings none = { 0 };
    int i = gui_get_selected(c->configs);
    c->shown = c->edit.count > 0 && i >= 0 && i < c->edit.count ? i : -1;
    const struct target_settings* s = c->shown >= 0 ? &c->edit.configurations[c->shown] : &none;
    gui_set_value(c->cake_target, s->cake_target);
    gui_set_selected(c->headers, s->headers);
    gui_set_selected(c->style, s->style);
    gui_set_selected(c->diag, s->diag);
    for (int k = 0; k < COUNT(copts_flags); k++)
        gui_set_checked(c->flags, k, s->flags[k]);
    gui_set_value(c->output, s->output);
    gui_set_value(c->options, s->options);
    c->include_dirs = s->include_dirs;
    dirs_refresh(ide, c->includes, &c->include_dirs, 0);
    for (int k = 0; k < 3; k++)
    {
        gui_set_value(c->post_build[k], s->post_build[k]);
        gui_set_value(c->debug[k], s->debug[k]);
    }
}

static void copts_store(struct ide* ide)
{
    struct copts_dialog* c = &ide->copts;
    if (c->shown < 0 || c->shown >= c->edit.count)
        return;
    struct target_settings* s = &c->edit.configurations[c->shown];
    snprintf(s->cake_target, sizeof s->cake_target, "%s", gui_get_value(c->cake_target));
    s->headers = gui_get_selected(c->headers);
    s->style = gui_get_selected(c->style);
    s->diag = gui_get_selected(c->diag);
    for (int i = 0; i < COUNT(copts_flags); i++)
        s->flags[i] = gui_get_checked(c->flags, i);
    snprintf(s->output, sizeof s->output, "%s", gui_get_value(c->output));
    snprintf(s->options, sizeof s->options, "%s", gui_get_value(c->options));
    s->include_dirs = c->include_dirs;
    for (int i = 0; i < 3; i++)   /* pre_build is not shown: it stays */
    {
        snprintf(s->post_build[i], sizeof s->post_build[i], "%s", gui_get_value(c->post_build[i]));
        snprintf(s->debug[i], sizeof s->debug[i], "%s", gui_get_value(c->debug[i]));
    }
}

/* A new configuration: every field empty but -line-directives. */
static void config_default(struct target_settings* s)
{
    memset(s, 0, sizeof *s);
    s->flags[0] = 1;   /* -line-directives on by default: the debugger needs it */
}

/* The row of the configuration named `name`, -1 none. */
static int config_find(const struct compiler_settings* cs, const char* name)
{
    for (int i = 0; i < cs->count; i++)
    {
        if (strcmp(cs->configurations[i].name, name) == 0)
            return i;
    }
    return -1;
}

/* The combo's rows: the configurations being edited, `selected` shown. */
static void copts_fill_configs(struct ide* ide, int selected)
{
    struct copts_dialog* c = &ide->copts;
    gui_clear_children(c->configs);
    for (int i = 0; i < c->edit.count; i++)
    {
        struct gui_node* it = create(ide, GUI_ITEM, c->edit.configurations[i].name);
        gui_set_id(it, EV_COPTS_CONFIG);
        gui_append(c->configs, it);
    }
    if (c->edit.count > 0)
        gui_set_selected(c->configs, selected >= 0 && selected < c->edit.count ? selected : 0);
}

/* New... and Rename...: the name dialog. New copies the shown configuration. */
static void config_name_open(struct ide* ide, int is_new)
{
    struct copts_dialog* c = &ide->copts;
    if ((!is_new && c->shown < 0) || (is_new && c->edit.count == MAX_CONFIGURATIONS))
        return;
    c->name_new = is_new;
    gui_set_label(c->name_window, is_new ? "New Configuration" : "Rename Configuration");
    gui_set_value(c->name_input, c->shown >= 0 ? c->edit.configurations[c->shown].name : "");
    show_dialog(ide, c->name_window, 50, 8, c->name_input);
}

static void config_name_accept(struct ide* ide)
{
    struct copts_dialog* c = &ide->copts;
    char name[64] = { 0 };
    snprintf(name, sizeof name, "%s", gui_get_value(c->name_input));
    gui_window_close(ide->app, c->name_window);
    int same = config_find(&c->edit, name);
    if (!name[0] || (same >= 0 && (c->name_new || same != c->shown)))
    {
        static const char* const ok[] = { "OK" };
        static const int ok_id[] = { 0 };
        gui_message_box(ide->app, "Configuration", name[0] ? "There is a configuration with this name." : "The name is empty.", ok, ok_id, 1);
        return;
    }
    copts_store(ide);
    int i = c->shown;
    if (c->name_new)
    {
        i = c->edit.count++;
        if (c->shown >= 0)
            c->edit.configurations[i] = c->edit.configurations[c->shown];
        else
            config_default(&c->edit.configurations[i]);
        if (c->edit.current < 0)
            c->edit.current = i;
    }
    snprintf(c->edit.configurations[i].name, sizeof c->edit.configurations[i].name, "%s", name);
    copts_fill_configs(ide, i);
    copts_show(ide);
}

/* Delete: asks first. */
static void config_delete_ask(struct ide* ide)
{
    struct copts_dialog* c = &ide->copts;
    if (c->shown < 0)
        return;
    char msg[200] = { 0 };
    snprintf(msg, sizeof msg, "Delete the configuration \"%s\"?", c->edit.configurations[c->shown].name);
    static const char* const labels[] = { "Delete", "Cancel" };
    static const int ids[] = { EV_COPTS_CONFIG_DELETE_YES, 0 };
    gui_message_box(ide->app, "Configuration", msg, labels, ids, 2);
}

/* Its Delete: the shown configuration; the Properties' Cancel brings it back. */
static void config_delete(struct ide* ide)
{
    struct copts_dialog* c = &ide->copts;
    int i = c->shown;
    if (i < 0)
        return;
    memmove(&c->edit.configurations[i], &c->edit.configurations[i + 1],
            (size_t)(c->edit.count - i - 1) * sizeof c->edit.configurations[0]);
    c->edit.count--;
    if (c->edit.current == i)
        c->edit.current = c->edit.count > 0 ? 0 : -1;
    else if (c->edit.current > i)
        c->edit.current--;
    copts_fill_configs(ide, i < c->edit.count ? i : c->edit.count - 1);
    copts_show(ide);
}

/* What Auto Config found on this machine, read once per click. */
struct auto_config_found
{
    struct ide_msvc_toolchain msvc;
    struct include_dirs msvc_dirs;   /* MSVC and the Windows SDK's headers */
    char tcc[1024];                  /* "" when there is no tcc */
    struct include_dirs tcc_dirs;
    int gcc, clang;
    struct include_dirs cc_dirs;     /* gcc's or clang's headers */
};

static void set3(char (*f)[512], const char* a, const char* b, const char* c)
{
    snprintf(f[0], sizeof f[0], "%s", a);
    snprintf(f[1], sizeof f[1], "%s", b);
    snprintf(f[2], sizeof f[2], "%s", c);
}

/* The compilers Auto Config knows: the name of their configurations and
 * the target Cake is given. */
struct auto_config_target
{
    const char* name;
    const char* cake_target;
};

static const struct auto_config_target auto_config_targets[] = {
    { "MSVC x64", "x86_64-pc-windows-msvc" },
    { "MSVC x86", "i686-pc-windows-msvc" },
    { "TCC Windows x64", "x86_64-w64-mingw32-tcc" },
    { "TCC Linux x64", "x86_64-linux-gnu-tcc" },
    { "TCC macOS ARM64", "aarch64-apple-darwin-tcc" },
    { "GCC Linux x64", "x86_64-linux-gnu-gcc" },
    { "GCC Linux ARM64", "aarch64-linux-gnu-gcc" },
    { "GCC Linux ARM32", "arm-linux-gnueabihf-gcc" },
    { "Clang macOS ARM64", "aarch64-apple-darwin-clang" },
};

/* The compiler of Cake target `slug` as the Post-Build Event, its headers
 * as the include directories, the built program as the Debugger's command -
 * Debug or Release. 0 when the compiler is not on this machine. */
static int auto_config_one(const struct auto_config_found* f, const char* slug, int debug, struct target_settings* s)
{
    char args[2048] = { 0 };
    if (ends_with(slug, "-msvc"))
    {
#ifdef _WIN32
        const struct ide_msvc_toolchain* tc = &f->msvc;
        if (!tc->vs_dir[0] || !tc->version[0] || !tc->sdk_root[0] || !tc->sdk_version[0])
            return 0;
        const char* arch = strncmp(slug, "x86_64-", 7) == 0 ? "x64" : "x86";   /* the x64-hosted cl for it, with its libraries */
        char command[700] = { 0 };
        snprintf(command, sizeof command, "%s\\VC\\Tools\\MSVC\\%s\\bin\\Hostx64\\%s\\cl.exe", tc->vs_dir, tc->version, arch);
        snprintf(args, sizeof args,
                 "/nologo %s $(CakeOutput) /Fe$(TargetPath) /link"
                 " /LIBPATH:\"%s\\VC\\Tools\\MSVC\\%s\\lib\\%s\""
                 " /LIBPATH:\"%sLib\\%s\\ucrt\\%s\""
                 " /LIBPATH:\"%sLib\\%s\\um\\%s\""
                 " user32.lib gdi32.lib shell32.lib advapi32.lib msimg32.lib",
                 debug ? "/Zi /Od" : "/O2 /DNDEBUG",
                 tc->vs_dir, tc->version, arch, tc->sdk_root, tc->sdk_version, arch, tc->sdk_root, tc->sdk_version, arch);
        set3(s->post_build, command, args, "$(TargetDir)");
        s->include_dirs = f->msvc_dirs;
#else
        return 0;
#endif
    }
    else if (ends_with(slug, "-tcc"))
    {
#if defined(_WIN32)
        const char* host = "x86_64-w64-mingw32-tcc";
#elif defined(__APPLE__)
        const char* host = "aarch64-apple-darwin-tcc";
#else
        const char* host = "x86_64-linux-gnu-tcc";
#endif
        if (!f->tcc[0] || strcmp(slug, host) != 0)
            return 0;
#ifdef _WIN32
        /* one way only: its debug info is not the PDB cdb reads */
        set3(s->post_build, f->tcc, debug ? "$(CakeOutput) -o $(TargetPath)"
                                          : "-DNDEBUG $(CakeOutput) -o $(TargetPath)", "$(TargetDir)");
#else
        if (debug)
        {
            /* -gdwarf: plain -g is stabs, which lldb does not read; one file per
             * tcc run: tcc gives every unit of a multi-file run the same low_pc */
            char command[1100] = { 0 };
            snprintf(command, sizeof command, "rm -f *.o && %s", f->tcc);
            snprintf(args, sizeof args, "-gdwarf -c $(CakeOutput) && %s -gdwarf *.o -o $(TargetFileName)", f->tcc);
            set3(s->post_build, command, args, "$(TargetDir)");
        }
        else
        {
            /* bare name, run in $(TargetDir): tcc's own codesign on macOS does not quote a path with spaces */
            set3(s->post_build, f->tcc, "-DNDEBUG $(CakeOutput) -o $(TargetFileName)", "$(TargetDir)");
        }
#endif
        s->include_dirs = f->tcc_dirs;
    }
    else if (ends_with(slug, "-gcc"))
    {
#if defined(__linux__)
        if (!f->gcc)
            return 0;
        snprintf(args, sizeof args, "%s -Wno-builtin-declaration-mismatch $(CakeOutput) -o $(TargetPath)",
                 debug ? "-g -O0" : "-O2 -DNDEBUG");
        set3(s->post_build, "gcc", args, "$(TargetDir)");
        s->include_dirs = f->cc_dirs;
#else
        return 0;
#endif
    }
    else if (ends_with(slug, "-clang"))
    {
#if defined(__APPLE__)
        if (!f->clang)
            return 0;
        snprintf(args, sizeof args, "%s -Wno-builtin-requires-header -Wno-incompatible-library-redeclaration"
                 " $(CakeOutput) -o $(TargetPath)", debug ? "-g -O0" : "-O2 -DNDEBUG");
        set3(s->post_build, "clang", args, "$(TargetDir)");
        s->include_dirs = f->cc_dirs;
#else
        return 0;
#endif
    }
    else
        return 0;
    set3(s->debug, "$(TargetPath)", "", "$(TargetDir)");
    return 1;
}

/* Auto Config: a Debug and a Release configuration for every compiler on
 * this machine - created, or updated when one has the same name; the other
 * configurations are left as they are. A report says which. */
static void copts_auto_config(struct ide* ide)
{
    struct copts_dialog* c = &ide->copts;
    copts_store(ide);
    static struct auto_config_found f;   /* too big for the stack */
    memset(&f, 0, sizeof f);
    char problems[2048] = { 0 };
#ifdef _WIN32
    ide_detect_include_dirs(includes_add_detected, &f.msvc_dirs, problems, sizeof problems, &f.msvc);
#endif
    if (ide_find_tcc(f.tcc, sizeof f.tcc))
        ide_detect_tcc_include_dirs(includes_add_detected, &f.tcc_dirs, problems, sizeof problems);
#if defined(__linux__)
    f.gcc = ide_output_has("gcc --version", "Free Software Foundation");
    if (f.gcc)
        ide_detect_cc_include_dirs("gcc", includes_add_detected, &f.cc_dirs, problems, sizeof problems);
#elif defined(__APPLE__)
    f.clang = ide_output_has("clang --version", "clang version");
    if (f.clang)
        ide_detect_cc_include_dirs("clang", includes_add_detected, &f.cc_dirs, problems, sizeof problems);
#endif
    struct ide_text configured = { 0 };
    for (int t = 0; t < COUNT(auto_config_targets); t++)
    {
        for (int k = 0; k < 2; k++)
        {
            static struct target_settings found;   /* too big for the stack */
            config_default(&found);
            snprintf(found.name, sizeof found.name, "%s %s", auto_config_targets[t].name, k == 0 ? "Debug" : "Release");
            snprintf(found.cake_target, sizeof found.cake_target, "%s", auto_config_targets[t].cake_target);
            if (!auto_config_one(&f, found.cake_target, k == 0, &found))
                continue;
            int i = config_find(&c->edit, found.name);
            if (i >= 0)
            {
                /* what Auto Config sets; the other fields stay */
                struct target_settings* s = &c->edit.configurations[i];
                snprintf(s->cake_target, sizeof s->cake_target, "%s", found.cake_target);
                s->include_dirs = found.include_dirs;
                memcpy(s->post_build, found.post_build, sizeof s->post_build);
                memcpy(s->debug, found.debug, sizeof s->debug);
            }
            else if (c->edit.count < MAX_CONFIGURATIONS)
            {
                c->edit.configurations[c->edit.count++] = found;
            }
            else
            {
                continue;
            }
            ide_text_printf(&configured, "  %s\n", found.name);
        }
    }
    if (c->edit.current < 0 && c->edit.count > 0)
        c->edit.current = 0;
    struct ide_text report = { 0 };
    ide_text_printf(&report, "Configurations created or updated:\n%s",
                    configured.data ? configured.data : "  none - no compiler found on this machine\n");
    free(configured.data);
    copts_fill_configs(ide, c->shown);
    copts_show(ide);
    if (problems[0])
        ide_text_printf(&report, "\n%s", problems);
    static const char* const labels[] = { "OK" };
    static const int ids[] = { 0 };
    gui_message_box(ide->app, "Auto Config", report.data ? report.data : "", labels, ids, 1);
    free(report.data);
}

static void copts_open(struct ide* ide, struct compiler_settings* s, const char* title, int page)
{
    struct copts_dialog* c = &ide->copts;
    copts_select_page(c, page);
    c->settings = s;
    c->edit = *s;
    gui_set_label(c->window, title);
    copts_fill_configs(ide, s->current);
    copts_show(ide);
    show_resizable_dialog(ide, c->window, COPTS_COLS, COPTS_ROWS, c->pages);
}

static void copts_config_changed(struct ide* ide)
{
    copts_store(ide);
    copts_show(ide);
}

static void copts_accept(struct ide* ide)
{
    struct copts_dialog* c = &ide->copts;
    copts_store(ide);
    *c->settings = c->edit;   /* the configuration in use stays: the combo only picks what to edit */
    gui_window_close(ide->app, c->window);
}

/* A "  >  " button that pops up the macros for `input`. */
static struct gui_node* add_macro_button(struct ide* ide, struct gui_node* parent, int col, int row,
                                         struct gui_node* input)
{
    struct macro_buttons* m = &ide->macro;
    if (!m->menu)
    {
        m->menu = create(ide, GUI_MENU, NULL);
        for (int i = 0; i < COUNT(macros); i++)
        {
            struct gui_node* it = create(ide, GUI_ITEM, macros[i].name);
            gui_set_id(it, EV_MACRO_ITEM + i);
            gui_set_hint(it, macros[i].hint);
            gui_append(m->menu, it);
        }
    }
    if (m->count == MAX_MACRO_BUTTONS)
        return NULL;
    struct gui_node* b = add_at(ide, parent, GUI_BUTTON, col, row, 5, 1, ">");
    gui_set_id(b, EV_MACRO + m->count);
    m->buttons[m->count] = b;
    m->inputs[m->count] = input;
    m->count++;
    return b;
}

static void macro_event(struct ide* ide, int id)
{
    struct macro_buttons* m = &ide->macro;
    if (id < EV_MACRO_ITEM)
    {
        int k = id - EV_MACRO;
        m->target = m->inputs[k];
        gui_popup_menu(ide->app, m->menu, m->buttons[k]);
        return;
    }
    if (!m->target)
        return;
    gui_input_insert(m->target, macros[id - EV_MACRO_ITEM].name);
    gui_focus(ide->app, m->target);
}

/* The External Tool titled `title`, or -1. */
static int ext_tool_find(struct ide* ide, const char* title)
{
    for (int i = 0; i < ide->ext_tools.count; i++)
    {
        if (ide->ext_tools.tools[i].title[0] && strcmp(ide->ext_tools.tools[i].title, title) == 0)
            return i;
    }
    return -1;
}

/* --- External Tools: 66 x 22, a list of tools and the selected one's
 * fields. The dialog edits a copy; OK keeps it. --- */

static void build_external_tools(struct ide* ide)
{
    struct ext_dialog* x = &ide->ext;
    static const struct { int id; const char* label; } buttons[] = {
        { EV_EXT_ADD, "Add" }, { EV_EXT_DELETE, "Delete" },
        { EV_EXT_UP, "Move Up" }, { EV_EXT_DOWN, "Move Down" },
    };
    static const char* const labels[] = { "Title:", "Command:", "Arguments:", "Directory:" };
    /* the size show_dialog opens it at; the list takes what a resize adds */
    enum { W = EXT_DIALOG_COLS, H = EXT_DIALOG_ROWS };
    const int top_left = GUI_ANCHOR_LEFT | GUI_ANCHOR_TOP;
    const int top_right = GUI_ANCHOR_RIGHT | GUI_ANCHOR_TOP;
    const int bottom_left = GUI_ANCHOR_LEFT | GUI_ANCHOR_BOTTOM;
    const int bottom_right = GUI_ANCHOR_RIGHT | GUI_ANCHOR_BOTTOM;
    const int bottom_wide = GUI_ANCHOR_LEFT | GUI_ANCHOR_RIGHT | GUI_ANCHOR_BOTTOM;
    x->window = new_dialog(ide, "External Tools");
    gui_window_set_resizable(x->window, 1);
    gui_window_set_min_size(x->window, W, H);
    add_label(ide, x->window, 2, 2, "Menu contents:");
    x->list = add_at(ide, x->window, GUI_LISTBOX, 2, 3, 47, 7, NULL);
    anchor_in(x->list, top_left | GUI_ANCHOR_RIGHT | GUI_ANCHOR_BOTTOM, 2, 3, 47, 7, W, H);
    gui_set_id(x->list, EV_EXT_LIST);
    for (int i = 0; i < COUNT(buttons); i++)
    {
        struct gui_node* b = add_at(ide, x->window, GUI_BUTTON, 51, 3 + i * 2, 12, 1, buttons[i].label);
        anchor_in(b, top_right, 51, 3 + i * 2, 12, 1, W, H);
        gui_set_id(b, buttons[i].id);
    }
    for (int i = 0; i < 4; i++)
    {
        int row = 11 + i * 2;
        anchor_in(add_label(ide, x->window, 2, row, labels[i]), bottom_left, 2, row, 0, 1, W, H);
        int cols = i == 0 ? 49 : 43;
        x->fields[i] = add_at(ide, x->window, GUI_INPUT, 14, row, cols, 1, NULL);
        anchor_in(x->fields[i], bottom_wide, 14, row, cols, 1, W, H);
        struct gui_node* b = NULL;
        if (i == 1)
        {
            b = add_at(ide, x->window, GUI_BUTTON, 58, row, 5, 1, "...");
            gui_set_id(b, EV_EXT_BROWSE);
        }
        else if (i > 1)
        {
            b = add_macro_button(ide, x->window, 58, row, x->fields[i]);
        }
        if (b)
            anchor_in(b, bottom_right, 58, row, 5, 1, W, H);
    }
    set_help(ide, x->window, "Run a compiler or any other program from the Tools menu",
             HELP_EXT_TOOLS);
    set_help(ide, x->list, "The tools, in the order the Tools menu shows them",
             HELP_EXT_LIST);
    set_help(ide, x->fields[0], "Name shown in the Tools menu",
             HELP_EXT_TITLE);
    set_help(ide, x->fields[1], "Program to run, e.g. `gcc` or `cl`",
             HELP_EXT_COMMAND);
    set_help(ide, x->fields[2], "Command-line arguments - `$(...)` macros expand when the tool runs",
             HELP_EXT_ARGUMENTS);
    set_help(ide, x->fields[3], "Directory the tool runs in - usually `$(ProjectDir)`",
             HELP_EXT_DIRECTORY);
    /* OK and Cancel centered as a pair: from the middle, 13 cells left and 1 right */
    static const struct { int id; const char* label; int offset; } bottom[] = {
        { EV_EXT_OK, "OK", -13 }, { EV_EXT_CANCEL, "Cancel", 1 },
    };
    for (int i = 0; i < COUNT(bottom); i++)
    {
        struct gui_node* n = add_at(ide, x->window, GUI_BUTTON, 0, 0, 12, 1, bottom[i].label);
        struct gui_layout l = { GUI_ANCHOR_LEFT | GUI_ANCHOR_BOTTOM };
        l.left.percent = 50;
        l.left.cells = bottom[i].offset;
        l.bottom.cells = H - 19 - 1;
        l.width.cells = 12;
        l.height.cells = 1;
        gui_set_layout(n, &l);
        gui_set_id(n, bottom[i].id);
    }
}

static char** ext_field(struct ext_tool* t, int i)
{
    return i == 0 ? &t->title : i == 1 ? &t->command : i == 2 ? &t->arguments : &t->directory;
}

/* The fields back into the tool they show. */
static void ext_store(struct ide* ide)
{
    struct ext_dialog* x = &ide->ext;
    if (x->current < 0 || x->current >= x->edit.count)
        return;
    struct ext_tool* t = &x->edit.tools[x->current];
    for (int i = 0; i < 4; i++)
        ext_set(ext_field(t, i), gui_get_value(x->fields[i]));
}

/* The list rebuilt, `current` selected and its fields shown. */
static void ext_show(struct ide* ide, int current)
{
    struct ext_dialog* x = &ide->ext;
    gui_clear_children(x->list);
    for (int i = 0; i < x->edit.count; i++)
        gui_append(x->list, create(ide, GUI_ITEM, x->edit.tools[i].title));
    x->current = current < x->edit.count ? current : x->edit.count - 1;
    for (int i = 0; i < 4; i++)
        gui_set_value(x->fields[i], x->current >= 0 ? *ext_field(&x->edit.tools[x->current], i) : "");
    if (x->current >= 0)
        gui_set_selected(x->list, x->current);
}

static void tools_menu_refresh(struct ide* ide);

static void ext_event(struct ide* ide, int id)
{
    struct ext_dialog* x = &ide->ext;
    struct ext_tools* e = &x->edit;
    ext_store(ide);
    int sel = gui_get_selected(x->list);
    switch (id)
    {
    case EV_EXT_LIST:
        ext_show(ide, sel);
        break;
    case EV_EXT_ADD:
        if (e->count < MAX_EXT_TOOLS)
        {
            ext_tool_init(&e->tools[e->count], "New Tool", "", "", "");
            e->count++;
            ext_show(ide, e->count - 1);
            gui_focus(ide->app, x->fields[0]);
        }
        break;
    case EV_EXT_DELETE:
        if (sel >= 0 && sel < e->count)
        {
            ext_tool_destroy(&e->tools[sel]);
            memmove(&e->tools[sel], &e->tools[sel + 1], sizeof e->tools[0] * (size_t)(e->count - sel - 1));
            e->count--;
            ext_show(ide, sel);
        }
        break;
    case EV_EXT_UP:
    case EV_EXT_DOWN:
    {
        int other = id == EV_EXT_UP ? sel - 1 : sel + 1;
        if (sel >= 0 && sel < e->count && other >= 0 && other < e->count)
        {
            struct ext_tool t = e->tools[sel];
            e->tools[sel] = e->tools[other];
            e->tools[other] = t;
            ext_show(ide, other);
        }
        break;
    }
    case EV_EXT_OK:
        ext_tools_copy(&ide->ext_tools, e);
        gui_window_close(ide->app, x->window);
        tools_menu_refresh(ide);
        break;
    default:
        break;
    }
}

/* The Tools menu: one item per external tool, then - when there are any -
 * a line, then its own items. */
/* The settings whose configuration Build > Configuration picks: the open project's, else the global ones. */
static struct compiler_settings* config_settings_in_use(struct ide* ide)
{
    return ide_project_is_open(&ide->project) ? &ide->project.compile : &ide->global_options;
}

/* Build > Configuration's rows, the one in use marked. */
static void config_menu_refresh(struct ide* ide)
{
    const struct compiler_settings* cs = config_settings_in_use(ide);
    gui_clear_children(ide->config_menu);
    for (int i = 0; i < cs->count; i++)
    {
        struct gui_node* it = create(ide, GUI_ITEM, cs->configurations[i].name);
        gui_set_mark(it, i == cs->current);
        gui_set_id(it, EV_CONFIG_ITEM + i);
        gui_append(ide->config_menu, it);
    }
    char right[120] = { 0 };   /* the statusbar's right: the configuration in use */
    snprintf(right, sizeof right, "%s   Cake " CAKE_VERSION,
             cs->current >= 0 && cs->current < cs->count ? cs->configurations[cs->current].name : "no configuration");
    gui_set_label(ide->statusbar, right);
}

static void project_save(struct ide* ide);
static void settings_save(struct ide* ide);

static void config_pick(struct ide* ide, int i)
{
    struct compiler_settings* cs = config_settings_in_use(ide);
    if (i < 0 || i >= cs->count)
        return;
    cs->current = i;
    if (cs == &ide->project.compile)
        project_save(ide);
    else
        settings_save(ide);
    config_menu_refresh(ide);
}

static void tools_menu_refresh(struct ide* ide)
{
    gui_clear_children(ide->tools_menu);
    for (int i = 0; i < ide->ext_tools.count; i++)
    {
        struct gui_node* it = create(ide, GUI_ITEM, ide->ext_tools.tools[i].title);
        gui_set_id(it, EV_TOOL_RUN + i);
        gui_set_hint(it, ide->ext_tools.tools[i].command);
        gui_append(ide->tools_menu, it);
    }
    if (ide->ext_tools.count > 0)
    {
        struct gui_node* sep = create(ide, GUI_ITEM, NULL);
        gui_set_separator(sep, 1);
        gui_append(ide->tools_menu, sep);
    }
    for (int i = 0; i < COUNT(tools_items); i++)
    {
        struct gui_node* it = create(ide, GUI_ITEM, tools_items[i].label);
        gui_set_id(it, tools_items[i].id);
        gui_set_hint(it, tools_items[i].hint);
        gui_append(ide->tools_menu, it);
    }
}

static const char* platform_for(struct ide* ide, const char* path);
static int file_uses_project(struct ide* ide, const char* path);
static const struct target_settings* settings_in_use(const struct compiler_settings* cs);

/* $(TargetFileName): the Build page's Output when set, else the project's
 * name or the document's, with .exe for an MSVC target - the old IDE's. */
static void target_file_name(struct ide* ide, const char* path, const char* doc_base, char* out, size_t cap)
{
    int project = file_uses_project(ide, path);
    const struct compiler_settings* cs = project ? &ide->project.compile : &ide->global_options;
    const struct target_settings* s = settings_in_use(cs);
    if (s->output[0])
    {
        snprintf(out, cap, "%s", s->output);
        return;
    }
    snprintf(out, cap, "%s%s", project ? ide->project.name : doc_base,
             strstr(platform_for(ide, path), "msvc") ? ".exe" : "");
}

/* $(TargetDir): <project or document folder>/<platform>. */
static void target_dir(struct ide* ide, const char* path, const char* doc_dir, char* out, size_t cap)
{
    /* join_path: a folder at the drive root ("C:\") already ends with a slash */
    join_path(out, cap, file_uses_project(ide, path) ? ide->project.dir : doc_dir, platform_for(ide, path));
}

/* A project .c file the last Build compiled (all of them before any Build). */
static int project_entry_changed(struct ide* ide, const char* entry)
{
    struct ide_project* p = &ide->project;
    if (!p->built.settings)
        return 1;
    char abs[1024] = { 0 };
    ide_project_absolute(p, entry, abs, sizeof abs);
    for (int i = 0; i < p->compiled.count; i++)
    {
        if (ide_path_equal(p->compiled.items[i], abs))
            return 1;
    }
    return 0;
}

/* A Compile (Ctrl+F7) pipeline covers only the active file. */
static int project_entry_skipped(struct ide* ide, const char* entry, const char* path)
{
    if (!ide->chain.compile || ide->chain.stage == STAGE_NONE)
        return 0;
    char abs[1024] = { 0 };
    ide_project_absolute(&ide->project, entry, abs, sizeof abs);
    return !ide_path_equal(abs, path);
}

/* $(CakeInputFiles) / $(CakeInputChanged): the project's .c files, quoted. */
static void append_cake_input(struct ide* ide, struct ide_text* out, const char* path, int only_changed)
{
    struct ide_project* p = &ide->project;
    if (!file_uses_project(ide, path))
    {
        if (path[0] && ends_with(path, ".c"))
            ide_text_printf(out, "\"%s\"", path);
        return;
    }
    int first = 1;
    for (int i = 0; i < p->files.count; i++)
    {
        const char* entry = p->files.items[i];
        if (!ends_with(entry, ".c") || (only_changed && !project_entry_changed(ide, entry)) || project_entry_skipped(ide, entry, path))
            continue;
        char abs[1024] = { 0 };
        ide_project_absolute(p, entry, abs, sizeof abs);
        ide_text_printf(out, "%s\"%s\"", first ? "" : " ", abs);
        first = 0;
    }
}

/* $(CakeOutput) / $(CakeOutputChanged): the C89 files Cake writes - under
 * <project dir>/<platform>/ for a project, else next to the document. */
static void append_cake_output(struct ide* ide, struct ide_text* out, const char* path, const char* dir,
                               const char* name, const char* ext, int only_changed)
{
    struct ide_project* p = &ide->project;
    const char* platform = platform_for(ide, path);
    if (file_uses_project(ide, path))
    {
        int first = 1;
        for (int i = 0; i < p->files.count; i++)
        {
            const char* entry = p->files.items[i];
            if (!ends_with(entry, ".c") || (only_changed && !project_entry_changed(ide, entry)) || project_entry_skipped(ide, entry, path))
                continue;
            int absolute = entry[0] == '/' || entry[0] == '\\' || (entry[0] && entry[1] == ':');
            if (absolute)
            {
                char edir[1024] = { 0 };
                snprintf(edir, sizeof edir, "%s", entry);
                parent_dir(edir);
                ide_text_printf(out, "%s\"%s" IDE_PATH_SEP "%s" IDE_PATH_SEP "%s\"", first ? "" : " ", edir, platform, file_name(entry));
            }
            else
            {
                ide_text_printf(out, "%s\"%s" IDE_PATH_SEP "%s" IDE_PATH_SEP "%s\"", first ? "" : " ", p->dir, platform, entry);
            }
            first = 0;
        }
        return;
    }
    if (!path[0] || strcmp(ext, ".c") != 0)
        return;
    ide_text_printf(out, "\"%s%s%s" IDE_PATH_SEP "%s%s\"", dir, dir[0] ? IDE_PATH_SEP : "", platform, name, ext);
}

static void append_include_arg(struct ide_text* out, const char* dir)
{
    size_t len = strlen(dir);
    while (len > 1 && (dir[len - 1] == '/' || dir[len - 1] == '\\'))
        len--;
    ide_text_printf(out, "%s-I\"%.*s\"", out->len > 0 ? " " : "", (int)len, dir);
}

/* `in` with the External Tools macros replaced - the old IDE's
 * exttool_expand: $(FilePath) $(FileDir) $(FileName) $(FileExt) (and the
 * $(Item...) spellings), $(CakeOutput[Changed]), $(CakeInput{Files,Changed}),
 * $(Target{Dir,FileName,Name,Ext,Path}), $(Platform), $(Target),
 * $(ProjectName), $(ProjectDir), $(InstallDir), $(IncludeDirs); "$$"
 * is a '$'. An unknown macro expands to nothing. */
static void expand_macros(struct ide* ide, const char* in, struct ide_text* out, int quote)
{
    struct doc* d = active_doc(ide);
    const char* path = d ? d->path : "";
    char dir[1024] = "", name[1024] = "", ext[64] = "";
    char source[1024] = "";
    if (path[0])
    {
        /* a generated file <folder>/<platform>/<name>: its source <folder>/<name> */
        snprintf(dir, sizeof dir, "%s", path);
        parent_dir(dir);
        char source_dir[1024] = { 0 };
        snprintf(source_dir, sizeof source_dir, "%s", dir);
        parent_dir(source_dir);
        join_path(source, sizeof source, source_dir, file_name(path));
        if (ide_file_exists(source) && strcmp(file_name(dir), platform_for(ide, source)) == 0)
        {
            path = source;
        }
    }
    if (path[0])
    {
        snprintf(dir, sizeof dir, "%s", path);
        parent_dir(dir);
        snprintf(name, sizeof name, "%s", file_name(path));
        char* dot = strrchr(name, '.');
        if (dot)
        {
            snprintf(ext, sizeof ext, "%s", dot);
            *dot = '\0';
        }
    }
    struct ide_project* p = &ide->project;
    ide_text_append(out, "", 0);
    for (const char* c = in; *c;)
    {
        if (c[0] == '$' && c[1] == '$')
        {
            ide_text_append(out, "$", 1);
            c += 2;
            continue;
        }
        const char* close = c[0] == '$' && c[1] == '(' ? strchr(c + 2, ')') : NULL;
        if (!close)
        {
            const char* start = c++;
            while (*c && *c != '$')
                c++;
            ide_text_append(out, start, (size_t)(c - start));
            continue;
        }
        char macro[32] = { 0 };
        snprintf(macro, sizeof macro, "%.*s", (int)(close - (c + 2)), c + 2);
        c = close + 1;
        char buf[1600] = { 0 }, tdir[1024] = { 0 }, tname[512] = { 0 };
        int is_path = 0;
        if (strcmp(macro, "FilePath") == 0 || strcmp(macro, "ItemPath") == 0)
        {
            snprintf(buf, sizeof buf, "%s", path);
            is_path = 1;
        }
        else if (strcmp(macro, "FileDir") == 0 || strcmp(macro, "ItemDir") == 0)
        {
            snprintf(buf, sizeof buf, "%s", dir);
            ide_text_printf(out, "%s", buf);   /* a folder: never quoted, the text goes on after it */
        }
        else if (strcmp(macro, "FileName") == 0 || strcmp(macro, "ItemFilename") == 0)
            ide_text_printf(out, "%s", name);
        else if (strcmp(macro, "FileExt") == 0 || strcmp(macro, "ItemExt") == 0)
            ide_text_printf(out, "%s", ext);
        else if (strcmp(macro, "CakeOutput") == 0 || strcmp(macro, "CakeOutputChanged") == 0)
            append_cake_output(ide, out, path, dir, name, ext, macro[10] == 'C');
        else if (strcmp(macro, "CakeInputFiles") == 0 || strcmp(macro, "CakeInputChanged") == 0)
            append_cake_input(ide, out, path, macro[9] == 'C');
        else if (strcmp(macro, "TargetDir") == 0)
        {
            target_dir(ide, path, dir, buf, sizeof buf);
            ide_text_printf(out, "%s", buf);
        }
        else if (strcmp(macro, "TargetFileName") == 0)
        {
            /* a file name, not a path: not quoted, like $(FileName) */
            target_file_name(ide, path, name, buf, sizeof buf);
            ide_text_printf(out, "%s", buf);
        }
        else if (strcmp(macro, "TargetName") == 0 || strcmp(macro, "TargetExt") == 0)
        {
            target_file_name(ide, path, name, buf, sizeof buf);
            char* dot = strrchr(buf, '.');
            if (macro[6] == 'N')
            {
                if (dot)
                    *dot = '\0';
                ide_text_printf(out, "%s", buf);
            }
            else
            {
                ide_text_printf(out, "%s", dot ? dot : "");
            }
        }
        else if (strcmp(macro, "TargetPath") == 0)
        {
            target_dir(ide, path, dir, tdir, sizeof tdir);
            target_file_name(ide, path, name, tname, sizeof tname);
            join_path(buf, sizeof buf, tdir, tname);
            is_path = 1;
        }
        else if (strcmp(macro, "Platform") == 0 || strcmp(macro, "Target") == 0)
            ide_text_printf(out, "%s", platform_for(ide, path));
        else if (strcmp(macro, "ProjectName") == 0)
            ide_text_printf(out, "%s", file_uses_project(ide, path) ? p->name : name);
        else if (strcmp(macro, "ProjectDir") == 0)
        {
            snprintf(buf, sizeof buf, "%s", file_uses_project(ide, path) ? p->dir : dir);
            ide_text_printf(out, "%s", buf);
        }
        else if (strcmp(macro, "InstallDir") == 0)
        {
            ide_exe_dir(buf, (int)sizeof buf);
            ide_text_printf(out, "%s", buf);
        }
        else if (strcmp(macro, "IncludeDirs") == 0)
        {
            /* the project's for its files, else the playground project's (absolute) */
            int project = file_uses_project(ide, path);
            const struct include_dirs* dirs = &settings_in_use(project ? &p->compile : &ide->global_options)->include_dirs;
            for (int i = 0; i < dirs->count; i++)
            {
                if (project)
                    ide_project_absolute(p, dirs->dirs[i], buf, sizeof buf);
                else
                    snprintf(buf, sizeof buf, "%s", dirs->dirs[i]);
                append_include_arg(out, buf);
            }
        }
        if (is_path)
        {
            /* quoted for the command line, unless the text already quotes it */
            int already_quoted = out->len > 0 && out->data[out->len - 1] == '"';
            ide_text_printf(out, quote && !already_quoted ? "\"%s\"" : "%s", buf);
        }
    }
}

/* --- Running in the background (see ide->run) --- */

static void bottom_panel_show(struct ide* ide, struct gui_node* show, struct gui_node* hide);

/* `text` appended to the Output panel as it came - no line end added. */
static void output_raw(struct ide* ide, const char* text, int len)
{
    const char* old = gui_get_value(ide->output.editor);
    size_t old_len = strlen(old);
    char* all = malloc(old_len + (size_t)len + 1);
    if (!all)
        return;
    memcpy(all, old, old_len);
    size_t n = old_len;
    for (int i = 0; i < len; i++)
    {
        if (text[i] != '\r')
            all[n++] = text[i];
    }
    all[n] = '\0';
    gui_set_value(ide->output.editor, all);
    free(all);
    gui_editor_goto_line(ide->output.editor, 1 << 30);
}

static int run_busy(struct ide* ide)
{
    if (!ide->run.proc)
        return 0;
    char msg[120] = { 0 };
    snprintf(msg, sizeof msg, "Busy: %s is running", ide->run.title);
    status(ide, msg);
    return 1;
}

static void run_finish(struct ide* ide);
static void apply_diagnostics(struct ide* ide, char* text, int clear);
static void debug_launch(struct ide* ide);
static void chain_build(struct ide* ide);

static void run_step(struct ide* ide)
{
    const char* cmd = ide->run.steps.items[ide->run.step];
    struct ide_text line = { 0 };
    ide_text_printf(&line, "> %s", cmd);
    output(ide, line.data);
    free(line.data);
    ide->run.proc = ide_process_start(cmd, ide->run.dir[0] ? ide->run.dir : NULL);
    cmdline_layout(ide);   /* "stdin>" while it runs */
    if (!ide->run.proc)
    {
        struct ide_text msg = { 0 };
        ide_text_printf(&msg, "Cannot run %s", cmd);
        output(ide, msg.data);
        free(msg.data);
        ide->run.failed = 1;
        run_finish(ide);
        return;
    }
    gui_set_timer(ide->app, 50, EV_TICK);
}

/* Starts `nsteps` command lines in `dir`, one after the other. */
static void run_start(struct ide* ide, int kind, const char* title, const char* dir, const char* const steps[], int nsteps)
{
    ide->run.kind = kind;
    ide_strings_clear(&ide->run.steps);
    for (int i = 0; i < nsteps; i++)
        ide_strings_add(&ide->run.steps, steps[i]);
    ide->run.step = 0;
    ide->run.failed = 0;
    snprintf(ide->run.dir, sizeof ide->run.dir, "%s", dir ? dir : "");
    snprintf(ide->run.title, sizeof ide->run.title, "%s", title);
    ide->run.text.len = 0;
    if (ide->run.text.data)
        ide->run.text.data[0] = '\0';
    bottom_panel_show(ide, ide->output.window, ide->fr.window);
    char msg[100] = { 0 };
    snprintf(msg, sizeof msg, "%s...", title);
    status(ide, msg);
    run_step(ide);
}

static void run_poll(struct ide* ide)
{
    char buf[4096] = { 0 };
    for (int rounds = 0; rounds < 16 && ide->run.proc; rounds++)
    {
        int n = ide_process_read(ide->run.proc, buf, sizeof buf);
        if (n == 0)
            return;
        if (n > 0)
        {
            output_raw(ide, buf, n);
            ide_text_append(&ide->run.text, buf, (size_t)n);
            continue;
        }
        int code = ide_process_close(ide->run.proc);
        ide->run.proc = NULL;
        const char* out = gui_get_value(ide->output.editor);
        size_t len = strlen(out);
        if (len > 0 && out[len - 1] != '\n')
            output_raw(ide, "\n", 1);
        if (code != 0)
        {
            char msg[64] = { 0 };
            snprintf(msg, sizeof msg, "(exit code %d)", code);
            output(ide, msg);
            ide->run.failed = 1;
        }
        if (!ide->run.failed && ++ide->run.step < ide->run.steps.count)
            run_step(ide);
        else
            run_finish(ide);
    }
}

static void git_refresh(struct ide* ide);

static void run_finish(struct ide* ide)
{
    cmdline_layout(ide);
    char msg[120] = { 0 };
    snprintf(msg, sizeof msg, "%s %s", ide->run.title, ide->run.failed ? "failed" : "finished");
    status(ide, msg);
    if (ide->run.kind == RUN_TOOL)
    {
        if (ide->run.text.data)
            apply_diagnostics(ide, ide->run.text.data, 0);
        enum build_stage stage = ide->chain.stage;
        ide->chain.stage = STAGE_NONE;
        if (ide->run.failed)
            return;
        if (stage == STAGE_PRE_BUILD)
            chain_build(ide);
        else if (stage == STAGE_POST_BUILD && ide->chain.debug)
            debug_launch(ide);
        return;
    }
    if (ide->run.kind == RUN_CLONE && !ide->run.failed && ide->run.clone_open)
    {
        snprintf(ide->folder.dir, sizeof ide->folder.dir, "%s", ide->run.clone_dest);
        folder_refresh(ide);
        show_side_panel(ide, ide->folder.window);
    }
    git_refresh(ide);
    /* git's own words in a box, as the old IDE - Stage/Unstage only on a failure */
    if (ide->run.kind != RUN_GIT_QUIET || ide->run.failed)
    {
        static const char* const ok[] = { "OK" };
        static const int ok_id[] = { 0 };
        const char* text = ide->run.text.data && ide->run.text.data[0] ? ide->run.text.data
                         : ide->run.failed ? "Failed." : "Done.";
        gui_message_box(ide->app, ide->run.title, text, ok, ok_id, 1);
    }
}

/* Runs the tool in the background; what it prints streams to the Output panel. */
static void bottom_panel_show(struct ide* ide, struct gui_node* show, struct gui_node* hide);

static void run_command(struct ide* ide, const char* title, const char* cmd, const char* args, const char* directory)
{
    if (run_busy(ide))
        return;
    struct ide_text command = { 0 }, arguments = { 0 }, dir = { 0 }, line = { 0 };
    expand_macros(ide, cmd, &command, 0);
    expand_macros(ide, args, &arguments, 1);
    expand_macros(ide, directory, &dir, 0);
#ifdef _WIN32
    ide_text_printf(&line, "\"%s\" ", command.data);
#else
    ide_text_printf(&line, "%s ", command.data);   /* through sh, as the old IDE: "rm -f *.o && tcc" */
#endif
    ide_text_append(&line, arguments.data, arguments.len);
    const char* steps[] = { line.data };
    run_start(ide, RUN_TOOL, title, dir.data, steps, 1);
    free(command.data);
    free(arguments.data);
    free(dir.data);
    free(line.data);
}

static void run_tool(struct ide* ide, int index)
{
    const struct ext_tool* t = &ide->ext_tools.tools[index];
    run_command(ide, t->title[0] ? t->title : "Tool", t->command, t->arguments, t->directory);
}

static void external_tools_open(struct ide* ide)
{
    ext_tools_copy(&ide->ext.edit, &ide->ext_tools);
    ext_show(ide, 0);
    show_dialog(ide, ide->ext.window, EXT_DIALOG_COLS, EXT_DIALOG_ROWS, ide->ext.list);
}

/* `l` shown in `list`, row `selected` selected. */
static void dirs_refresh(struct ide* ide, struct gui_node* list, const struct include_dirs* l, int selected)
{
    gui_clear_children(list);
    for (int i = 0; i < l->count; i++)
        gui_append(list, create(ide, GUI_ITEM, l->dirs[i]));
    if (selected >= l->count)
        selected = l->count - 1;
    if (selected >= 0)
        gui_set_selected(list, selected);
}

static void project_save(struct ide* ide);

/* A project's entries are relative to its folder; the global lists keep
 * the path as picked. */
static void dirs_add(struct ide* ide, struct gui_node* list, struct include_dirs* l, int project, const char* dir)
{
    char entry[1024] = { 0 };
    if (project)
        ide_project_relative(&ide->project, dir, entry, sizeof entry);
    else
        snprintf(entry, sizeof entry, "%s", dir);
    for (int i = 0; i < l->count; i++)
    {
        if (strcmp(l->dirs[i], entry) == 0)
            return;
    }
    if (l->count == MAX_INCLUDE_DIRS)
        return;
    if (strlen(entry) >= sizeof l->dirs[0])
    {
        status(ide, "The path is too long");
        return;
    }
    memcpy(l->dirs[l->count++], entry, strlen(entry) + 1);
    dirs_refresh(ide, list, l, l->count - 1);
}

static void includes_add_detected(void* ctx, const char* dir)
{
    struct include_dirs* l = ctx;
    if (l->count < MAX_INCLUDE_DIRS)
        snprintf(l->dirs[l->count++], sizeof l->dirs[0], "%s", dir);
}

static void settings_save(struct ide* ide);
static void settings_path(char* out, size_t cap);
static void playground_project_save(struct ide* ide);
static void compile_to_json(struct json_value* c, const struct compiler_settings* s);
static void compile_from_json(const struct json_value* c, struct compiler_settings* s);

/* The selected row removed (move 0), or moved up (-1) or down (+1). */
static void dirs_edit(struct ide* ide, struct gui_node* list, struct include_dirs* l, int move)
{
    int sel = gui_get_selected(list);
    if (sel < 0 || sel >= l->count)
        return;
    if (move == 0)
    {
        memmove(l->dirs[sel], l->dirs[sel + 1], sizeof l->dirs[0] * (size_t)(l->count - sel - 1));
        l->count--;
        dirs_refresh(ide, list, l, sel);
        return;
    }
    int other = sel + move;
    if (other >= 0 && other < l->count)
    {
        char t[sizeof l->dirs[0]] = { 0 };
        memcpy(t, l->dirs[sel], sizeof t);
        memcpy(l->dirs[sel], l->dirs[other], sizeof t);
        memcpy(l->dirs[other], t, sizeof t);
        dirs_refresh(ide, list, l, other);
    }
}

/* --- Help: 80 x 20, read-only text --- */

/* Every F1 text, by id; "help:<slug>" links one to another. */
static const struct help_topic help_topics[HELP_COUNT] = {
    [HELP_OVERVIEW] = { "overview",
        "# Cake IDE\n"
        "\n"
        "An IDE for Cake, the C front end that checks C (ownership, flow\n"
        "analysis) and translates it to C89-compatible C.\n"
        "\n"
        "## Links\n"
        "\n"
        "Ctrl+click a link to open it in the browser.\n"
        "\n"
        "- Site: https://cakecc.org\n"
        "- Playground: https://cakecc.org/playground.html\n"
        "- GitHub: https://github.com/thradams/cake\n"
        "- Releases: https://github.com/thradams/cake/releases\n"
        "- Discord server: https://discord.gg/YRekr2N65S\n"
        "\n"
        "## File menu\n"
        "\n"
        "- **New** > **File...** a new file; **Project...** a new "
        "[project](help:new-project).\n"
        "- **Open** > **File...** (Ctrl+O) a file; **Project...** a `.cakeproj`; "
        "**Folder...** a folder, in the Folder panel.\n"
        "- **Clone Repository...** - a [remote Git repository](help:clone), "
        "copied to a local folder.\n"
        "- **Save**, **Save As...**, **Save all**.\n"
        "- **Recent Projects** > the projects opened last, as `name.cakeproj "
        "(full path)` - click one to open it. A project that no longer opens "
        "leaves the list.\n"
        "- **Exit**.\n"
        "\n"
        "**Project > Properties...** edits how the open project - or the "
        "[Playground](help:playground) - is built and debugged.\n"
        "\n" },
    [HELP_EXT_TOOLS] = { "ext-tools",
        "# Run a compiler or any other program from the Tools menu\n"
        "\n"
        "Cake only translates C to C89-compatible C - it does not link. \n"
        "Linking is left to a real compiler, run from here. \n"
        "Each tool added here appears in the **Tools** menu.\n"
        "\n"
        "A tool can also be run from the command line at the bottom of the "
        "**Output**\n"
        "window: type its **Title** and press Enter. Case, spaces and "
        "punctuation are\n"
        "ignored, so a tool titled `Run Tests` runs with `run tests` or "
        "`runtests`. Type\n"
        "`help` there for the other commands.\n"
        "\n"
        "A typical setup is one tool per compiler:\n"
        "\n"
        "**GCC / Clang** (Linux, macOS)\n"
        "\n"
        "| Field | Value |\n"
        "|---|---|\n"
        "| Title | `GCC` |\n"
        "| Command | `gcc` |\n"
        "| Arguments | `-g -Wno-incompatible-library-redeclaration "
        "-Wno-builtin-requires-header $(CakeOutput) -o $(TargetPath)` |\n"
        "| Directory | `$(ProjectDir)` |\n"
        "\n"
        "**MSVC** (Windows, from a Developer Command Prompt)\n"
        "\n"
        "| Field | Value |\n"
        "|---|---|\n"
        "| Title | `MSVC` |\n"
        "| Command | `cl` |\n"
        "| Arguments | `/Zi /nologo $(CakeOutput) /Fe$(TargetPath)` |\n"
        "| Directory | `$(ProjectDir)` |\n"
        "\n"
        "Running the tool after **Build** (F7) links Cake's output into "
        "`$(TargetPath)`, which is exactly the file **Debug** (F5) launches - "
        "so build, external compile and debug all agree on one binary.\n"
        "\n"
        "Cake's output declares the library functions it uses instead of "
        "keeping the original `#include`s. Clang flags those declarations with "
        "`-Wbuiltin-requires-header` and "
        "`-Wincompatible-library-redeclaration`; both are expected for Cake "
        "output, which is why the GCC/Clang example silences them.\n"
        "\n"
        "## See also\n"
        "\n"
        "- [Arguments and macros](help:ext-arguments)\n"
        "- [Build](help:build-options)\n"
        "- [Overview](help:overview)\n" },
    [HELP_EXT_LIST] = { "ext-list",
        "# Menu contents\n"
        "\n"
        "The tools, in the order the **Tools** menu shows them. Pick one to "
        "edit its fields below; "
        "**Add**, **Delete**, **Move Up** and **Move Down** change the list. "
        "OK keeps the changes, Cancel drops them.\n"
        "\n"
        "A tool can also be the **Pre-Build** or **Post-Build Event** (**Build "
        "> Options...**), "
        "chosen by its **Title** - renaming the tool means choosing it "
        "there again." },
    [HELP_EXT_TITLE] = { "ext-title",
        "# Name shown in the Tools menu\n"
        "\n"
        "It is also the tool's command: type it in the Output window's "
        "command line to run the tool. Case, spaces and punctuation are "
        "ignored there." },
    [HELP_EXT_COMMAND] = { "ext-command",
        "# Program to run, e.g. `gcc` or `cl`\n"
        "\n"
        "The **...** button browses for a program. Macros (see Arguments) "
        "work here too." },
    [HELP_EXT_ARGUMENTS] = { "ext-arguments",
        "# Command-line arguments - `$(...)` macros expand when the tool runs\n"
        "\n"
        "The **>** button inserts a macro at the caret; hover a macro there to "
        "see what\n"
        "it means. `$$` is a literal `$`; an unknown macro expands to "
        "nothing.\n"
        "\n"
        "Example: active document `C:/work/hello/src/main.c`, project `hello` "
        "in\n"
        "`C:/work/hello`, target `x86_64-pc-windows-msvc`.\n"
        "\n"
        "| Macro | Example |\n"
        "|---|---|\n"
        "| `$(FilePath)` | `C:/work/hello/src/main.c` |\n"
        "| `$(FileDir)` | `C:/work/hello/src` |\n"
        "| `$(FileName)` | `main` |\n"
        "| `$(FileExt)` | `.c` |\n"
        "| `$(CakeOutput)` | one output path per `.c` of the project |\n"
        "| `$(CakeOutputChanged)` | one output path per `.c` the last Build "
        "compiled |\n"
        "| `$(CakeInputFiles)` | `\"C:/work/hello/src/main.c\"` - every `.c` "
        "of the project |\n"
        "| `$(CakeInputChanged)` | the `.c` files the last Build compiled |\n"
        "| `$(TargetPath)` | `C:/work/hello/x86_64-pc-windows-msvc/hello.exe` |\n"
        "| `$(TargetDir)` | `C:/work/hello/x86_64-pc-windows-msvc` |\n"
        "| `$(TargetFileName)` | `hello.exe` |\n"
        "| `$(TargetName)` | `hello` |\n"
        "| `$(TargetExt)` | `.exe` |\n"
        "| `$(ProjectDir)` | `C:/work/hello` |\n"
        "| `$(ProjectName)` | `hello` |\n"
        "| `$(InstallDir)` | `C:/Program Files/cake/0.15.15` - the folder of cakeide |\n"
        "| `$(Platform)` | `x86_64-pc-windows-msvc` |\n"
        "\n"
        "Every `...Dir` macro ends without a slash: write "
        "`$(ProjectDir)/name`.\n"
        "\n"
        "In **Arguments**, a macro that is a path (`$(FilePath)`, "
        "`$(TargetPath)`, and each file of\n"
        "`$(CakeOutput)`, `$(CakeInputFiles)`, `$(IncludeDirs)`) is quoted "
        "for you; names and extensions\n"
        "are not. Command and Directory are never quoted.\n"
        "\n"
        "## See also\n"
        "\n"
        "- [External Tools](help:ext-tools)\n"
        "- [Directory](help:ext-directory)\n" },
    [HELP_EXT_DIRECTORY] = { "ext-directory",
        "# Directory the tool runs in - usually `$(ProjectDir)`\n"
        "\n"
        "The **>** button inserts a macro at the caret."
        "\n\n## See also\n\n"
        "- [Arguments and macros](help:ext-arguments)\n" },
    [HELP_CLONE] = { "clone",
        "# Copy a remote Git repository to a local folder\n"
        "\n"
        "**File > Clone Repository...** runs `git clone <Repository location> "
        "<Path>` - Git must be installed and on the PATH." },
    [HELP_CLONE_URL] = { "clone-url",
        "# The repository's URL, e.g. `https://github.com/user/repo.git`\n"
        "\n"
        "Anything `git clone` accepts: an HTTPS or SSH URL "
        "(`git@github.com:user/repo.git`) or a local path. **Path** follows it "
        "as you type, ending in the repository's name." },
    [HELP_CLONE_PATH] = { "clone-path",
        "# The new folder the repository is cloned into\n"
        "\n"
        "Filled in as parent folder + the repository's name - the same name "
        "plain `git clone` would pick. Once you edit it or pick it with "
        "**...**, it stops following the URL. Its parent folder must exist; "
        "a folder that already exists is refused." },
    [HELP_CLONE_OPEN_FOLDER] = { "clone-open-folder",
        "## Open Folder\n\nshow the cloned folder in the Folder panel when "
        "done\n"
        "\n"
        "Unchecked, the Folder panel keeps showing whatever it shows now." },
    [HELP_NEW_PROJECT] = { "new-project",
        "# Create a new Cake project (`.cakeproj`)\n"
        "\n"
        "A project is a `.cakeproj` file: a list of source files, plus the "
        "options used to build them - for every target and configuration. File "
        "paths inside the project folder are stored relative to it, so the "
        "project can be moved or shared.\n"
        "\n"
        "**File > New > Project...** creates one; **File > Open > Project...** "
        "opens one, and **File > Recent Projects** lists the ones opened last.\n"
        "\n"
        "A new project starts with the [Playground](help:playground)'s options; "
        "press **Auto Config** in **Project > Properties...** to set its compilers.\n"
        "\n"
        "## Build and Compile\n"
        "\n"
        "- **Build** (F7) compiles every `.c` file of the project in one Cake "
        "invocation - linking them is the output compiler's job. With the "
        "Playground active (or no project open), Build compiles just the active file.\n"
        "- **Compile** (Ctrl+F7) always compiles only the active file.\n"
        "\n"
        "## Project settings vs. the Playground's\n"
        "\n"
        "- A project's files use the project's options, saved in its `.cakeproj`.\n"
        "- Every other file - the Playground, a file opened on its own - uses the "
        "[Playground](help:playground) project's options.\n"
        "\n"
        "The two are never merged. **Project > Properties...** edits the one the "
        "active file uses." },
    [HELP_NEW_PROJECT_NAME] = { "new-project-name",
        "# Name of the project file\n\n`<name>.cakeproj`\n"
        "\n"
        "It is created in **Folder** - or in a new `<name>` subfolder of "
        "it when **Create Folder** is checked. Creation stops if a project "
        "with that name already exists there." },
    [HELP_NEW_PROJECT_CREATE_FOLDER] = { "new-project-create-folder",
        "## Create Folder\n\nput the project in a new `<Project Name>` "
        "subfolder of Folder\n"
        "\n"
        "Unchecked, the project file goes straight into **Folder**. "
        "If the subfolder already exists, nothing is created." },
    [HELP_NEW_PROJECT_HELLO_WORLD] = { "new-project-hello-world",
        "## Hello World\n\nstart the project with a `main.c` that prints "
        "\"Hello, world!\"\n"
        "\n"
        "`main.c` is added to the project. An existing `main.c` in the "
        "project folder is never overwritten - it is added as it is." },
    [HELP_COPTS] = { "copts",
        "# Properties\n\nhow a project - or the Playground - is built and debugged\n"
        "\n"
        "**Project > Properties...** opens it. The title says whose options these are:\n"
        "\n"
        "- **Properties (Project)** - the open project's, in its `.cakeproj`.\n"
        "- **Properties (Playground)** - the [Playground](help:playground) project's, "
        "used by every file outside the open project. Opened when no project is open, "
        "or when the active file is not part of it.\n"
        "\n"
        "## Target and Configuration\n"
        "\n"
        "Every target and configuration (Debug, Release) has its own options. The "
        "two combos at the top pick the ones being edited - not the ones in use, which "
        "**Build > Target** and **Build > Configuration** pick, and the status bar "
        "shows.\n"
        "\n"
        "**[All]**: a field changed here goes to every target (or configuration); "
        "the fields not changed keep each one's own value.\n"
        "\n"
        "## Pages\n"
        "\n"
        "- **Compiler** - how Cake compiles: headers, style, diagnostics, flags, options.\n"
        "- **Includes** - the `#include` search path, as `-I`.\n"
        "- **Build** - the output name and the command run after Cake.\n"
        "- **Debugger** - the program F5 runs.\n"
        "\n"
        "**Auto Config** sets the compilers found on this machine. **OK** saves; "
        "**Cancel** forgets every change, Auto Config's too.\n"
        "\n"
        "Focus a field and press F1 (or click the status bar) for its details."
        "\n\n## See also\n\n"
        "- [Target](help:copts-target)\n"
        "- [Configuration](help:copts-configuration)\n"
        "- [Auto Config](help:auto-config)\n"
        "- [Include Directories](help:include-dirs)\n"
        "- [Build](help:build-options)\n"
        "- [Debugger](help:debug-options)\n" },
    [HELP_COPTS_TARGET] = { "copts-target",
        "# Target (`-target=<name>`)\n"
        "\n"
        "The platform the generated C89 code is for: integer sizes, alignment, "
        "and the style of the output. Pick the platform whose compiler will build "
        "the generated code - it does not have to be the one Cake is running on.\n"
        "\n"
        "Each configuration has its own, passed to cake as `-target=` followed by "
        "this text. Empty: the target Cake was built for. `cake -target=x` lists them."
        "\n\n## See also\n\n"
        "- [x86_64-pc-windows-msvc](help:target-x86_64-pc-windows-msvc)\n"
        "- [x86_64-linux-gnu-gcc](help:target-x86_64-linux-gnu-gcc)\n"
        "- [Properties](help:copts)\n" },
    [HELP_AUTO_CONFIG] = { "auto-config",
        "# Auto Config\n\nsets the compilers found on this machine\n"
        "\n"
        "For every compiler here, a Debug and a Release configuration - created, "
        "or updated when one has the same name (e.g. \"MSVC x64 Debug\"):\n"
        "\n"
        "- **Build** - the compiler as the command run after Cake: `cl.exe` "
        "(Visual Studio) for msvc, `tcc`, `gcc` on Linux, `clang` on macOS - with "
        "`-g`/`/Zi` for Debug and `-O2`/`/O2 -DNDEBUG` for Release.\n"
        "- **Includes** - that compiler's headers (Visual Studio and the Windows "
        "SDK, or tcc's).\n"
        "- **Debugger** - `$(TargetPath)`, run in `$(TargetDir)`, no arguments.\n"
        "\n"
        "The other configurations are left as they are - so Auto Config on "
        "Windows and then on a Mac sets both. A report lists which.\n"
        "\n"
        "Nothing is saved until **OK**."
        "\n\n## See also\n\n"
        "- [Properties](help:copts)\n" },
    [HELP_TARGET_CLANG_MACOS_ARM64] = { "target-aarch64-apple-darwin-clang",
        "## `-target=aarch64-apple-darwin-clang`\n\nmacOS arm64 (Apple Silicon)\n"
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
        "The generated C89 goes to an `aarch64-apple-darwin-clang` folder next to the "
        "sources; compile it with the target compiler:\n"
        "\n"
        "```\n"
        "clang -w aarch64-apple-darwin-clang/file1.c -o file1\n"
        "```" },
    [HELP_TARGET_GCC_LINUX_ARM64] = { "target-aarch64-linux-gnu-gcc",
        "## `-target=aarch64-linux-gnu-gcc`\n\nLinux aarch64 (e.g. Raspberry Pi)\n"
        "\n"
        "Data model **LP64**. Output compiler: GCC. Plain `char` is unsigned.\n"
        "\n"
        "| Type | Size (bytes) |\n"
        "|---|---|\n"
        "| `char` (unsigned) | 1 |\n"
        "| `short` | 2 |\n"
        "| `int` | 4 |\n"
        "| `long` | 8 |\n"
        "| `long long` | 8 |\n"
        "| pointer | 8 |\n"
        "| `long double` | 16 |\n"
        "| `wchar_t` | 4 (`unsigned int`) |\n"
        "| `size_t` | 8 (`unsigned long`) |\n"
        "\n"
        "The generated C89 goes to an `aarch64-linux-gnu-gcc` folder next to the "
        "sources; compile it with the target compiler:\n"
        "\n"
        "```\n"
        "gcc -w aarch64-linux-gnu-gcc/file1.c -o file1\n"
        "```" },
    [HELP_TARGET_GCC_LINUX_X64] = { "target-x86_64-linux-gnu-gcc",
        "## `-target=x86_64-linux-gnu-gcc`\n\nLinux x86-64\n"
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
        "The generated C89 goes to an `x86_64-linux-gnu-gcc` folder next to the "
        "sources; compile it with the target compiler:\n"
        "\n"
        "```\n"
        "gcc -w x86_64-linux-gnu-gcc/file1.c -o file1\n"
        "```" },
    [HELP_TARGET_MSVC_WIN_X64] = { "target-x86_64-pc-windows-msvc",
        "## `-target=x86_64-pc-windows-msvc`\n\nWindows x64\n"
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
        "The generated C89 goes to an `x86_64-pc-windows-msvc` folder next to the "
        "sources; compile it with the target compiler:\n"
        "\n"
        "```\n"
        "cl x86_64-pc-windows-msvc\\file1.c\n"
        "```" },
    [HELP_TARGET_MSVC_WIN_X86] = { "target-i686-pc-windows-msvc",
        "## `-target=i686-pc-windows-msvc`\n\nWindows x86 (32-bit)\n"
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
        "The generated C89 goes to an `i686-pc-windows-msvc` folder next to the "
        "sources; compile it with the target compiler:\n"
        "\n"
        "```\n"
        "cl i686-pc-windows-msvc\\file1.c\n"
        "```" },
    [HELP_TARGET_TCC_LINUX_X64] = { "target-x86_64-linux-gnu-tcc",
        "## `-target=x86_64-linux-gnu-tcc`\n\nLinux x86-64 with the Tiny C Compiler\n"
        "\n"
        "Data model **LP64** (the sizes of `x86_64-linux-gnu-gcc`), TCC's predefined "
        "macros.\n"
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
        "The generated C89 goes to an `x86_64-linux-gnu-tcc` folder next to the "
        "sources; compile it with TCC:\n"
        "\n"
        "```\n"
        "tcc x86_64-linux-gnu-tcc/file1.c -o file1\n"
        "```" },
    [HELP_TARGET_TCC_MACOS_ARM64] = { "target-aarch64-apple-darwin-tcc",
        "## `-target=aarch64-apple-darwin-tcc`\n\nmacOS arm64 with the Tiny C Compiler\n"
        "\n"
        "Data model **LP64** (the sizes of `aarch64-apple-darwin-clang`), TCC's "
        "predefined macros. `__builtin_inf` and `__builtin_fabs` are written "
        "as plain C.\n"
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
        "The generated C89 goes to an `aarch64-apple-darwin-tcc` folder next to the "
        "sources; compile it with TCC:\n"
        "\n"
        "```\n"
        "tcc aarch64-apple-darwin-tcc/file1.c -o file1\n"
        "```" },
    [HELP_TARGET_TCC_WIN_X64] = { "target-x86_64-w64-mingw32-tcc",
        "## `-target=x86_64-w64-mingw32-tcc`\n\nWindows x64 with the Tiny C Compiler\n"
        "\n"
        "Data model **LLP64** (the sizes of `x86_64-pc-windows-msvc`), GCC syntax, TCC's "
        "predefined macros (`__TINYC__`, `__WINT_TYPE__`, ...) and no "
        "`_MSC_VER`. Use it with TCC's own headers - Detect in the System "
        "Directories dialog offers them.\n"
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
        "The generated C89 goes to an `x86_64-w64-mingw32-tcc` folder next to the sources; "
        "compile it with TCC:\n"
        "\n"
        "```\n"
        "tcc x86_64-w64-mingw32-tcc\\file1.c -o file1.exe\n"
        "```" },
    [HELP_TARGET_GCC_LINUX_ARM32] = { "target-arm-linux-gnueabihf-gcc",
        "## `-target=arm-linux-gnueabihf-gcc`\n\nLinux 32-bit ARM, EABI hard-float "
        "(e.g. Raspberry Pi 1/2 with a 32-bit OS)\n"
        "\n"
        "Data model **ILP32**. Output compiler: GCC. Plain `char` is "
        "unsigned.\n"
        "\n"
        "| Type | Size (bytes) |\n"
        "|---|---|\n"
        "| `char` (unsigned) | 1 |\n"
        "| `short` | 2 |\n"
        "| `int` | 4 |\n"
        "| `long` | 4 |\n"
        "| `long long` | 8 |\n"
        "| pointer | 4 |\n"
        "| `long double` | 8 |\n"
        "| `wchar_t` | 4 (`unsigned int`) |\n"
        "| `size_t` | 4 (`unsigned int`) |\n"
        "\n"
        "The generated C89 goes to an `arm-linux-gnueabihf-gcc` folder next to the "
        "sources; compile it with the target compiler:\n"
        "\n"
        "```\n"
        "gcc -w arm-linux-gnueabihf-gcc/file1.c -o file1\n"
        "```" },
    [HELP_COPTS_HEADERS] = { "copts-headers",
        "# Which headers `#include <...>` finds (`-cake-headers`)\n\n"
        "Cake has its own copy of some standard headers (`stdio.h`, `string.h`, "
        "...). In both modes `#include <...>` takes Cake's header when it has one; "
        "the modes differ in what that header does next.\n\n"
        "- **System Headers** - Cake's header adds its annotations, then includes "
        "the system header with `#include_next`.\n"
        "- **Cake Headers** - Cake's header declares everything itself; the system "
        "header is not included (`-cake-headers`).\n\n"
        "A header Cake has no copy of is found in the include directories, in both "
        "modes." },
    [HELP_HEADERS_SYSTEM] = { "headers-system",
        "## System Headers\n\nCake's header, if it has one, then the system one\n\n"
        "`#include <...>` takes Cake's own header when it exists; it adds Cake's "
        "annotations and includes the system header with `#include_next`. Headers "
        "Cake has no copy of come straight from the include directories. "
        "No `-cake-headers` is passed." },
    [HELP_HEADERS_CAKE] = { "headers-cake",
        "## Cake Headers (`-cake-headers`)\n\nCake's header, if it has one, alone\n\n"
        "`#include <...>` takes Cake's own header when it exists, and that header "
        "declares everything itself: the system header is not included. Headers "
        "Cake has no copy of still come from the include directories.\n\n"
        "Gives the same declarations on every platform - used to compile Cake "
        "itself and run its tests portably." },
    [HELP_COPTS_STYLE] = { "copts-style",
        "# Coding style checked by diagnostic 11 (`-style=<name>`)\n\n"
        "Passing `-style` turns diagnostic 11 (style) on as a note." },
    [HELP_STYLE_NONE] = { "style-none",
        "## No style check\n\nNo `-style` is passed, so diagnostic 11 "
        "(style) stays off." },
    [HELP_STYLE_CAKE] = { "style-cake",
        "## `-style=cake`\n\nchecks the code against Cake's own style" },
    [HELP_STYLE_GNU] = { "style-gnu",
        "## `-style=gnu`\n\nchecks the code against the GNU style" },
    [HELP_STYLE_MICROSOFT] = { "style-microsoft",
        "## `-style=microsoft`\n\nchecks the code against the Microsoft "
        "style" },
    [HELP_COPTS_DIAG] = { "copts-diag",
        "# How diagnostic positions are printed "
        "(`-fdiagnostics-format=<format>`)\n\n"
        "Both shapes are understood by Visual Studio and by Visual Studio "
        "Code." },
    [HELP_DIAG_IDE] = { "diag-ide",
        "## `-fdiagnostics-format=ide`\n\nfile.c:1:2: warning 10: message" },
    [HELP_DIAG_GCC] = { "diag-gcc",
        "## `-fdiagnostics-format=gcc`\n\nfile.c:1:2: warning 10: message" },
    [HELP_DIAG_MSVC] = { "diag-msvc",
        "## `-fdiagnostics-format=msvc`\n\nfile.c(1,2): warning 10: message" },
    [HELP_FLAG_LINE_DIRECTIVES] = { "flag-line-directives",
        "## `-line-directives`\n\nemit `#line` directives in the generated C89 "
        "output\n\n"
        "Preserves source location information." },
    [HELP_FLAG_FLOW] = { "flag-flow",
        "## `-flow`\n\nrun Cake's built-in flow analysis\n\n"
        "Checks uninitialized values, null dereference, unreachable code, "
        "and the annotations when `-check-annotations` is on. "
        "Same as `#pragma flow enable`." },
    [HELP_FLAG_CONST_LITERAL] = { "flag-const-literal",
        "## `-const-literal`\n\ntreat string literals as `const char[]` "
        "rather than `char[]`" },
    [HELP_FLAG_WALL] = { "flag-wall",
        "## `-Wall`\n\nenable all warnings" },
    [HELP_FLAG_DEFAULT_NONNULL] = { "flag-default-nonnull",
        "## `-default-nonnull`\n\npointers without `_Opt` are non-null\n\n"
        "Requires `-check-annotations`. Same as `#pragma default_nonnull`." },
    [HELP_FLAG_ANNOTATIONS] = { "flag-annotations",
        "## `-check-annotations`\n\n`_Owner`, `_View`, `_Dtor`, `_Out`, `_Clear`, `_Opt`... "
        "are checked\n\n"
        "Without it they are ignored, like empty macros. "
        "Same as `#pragma check_annotations enable`." },
    [HELP_COPTS_OUTPUT] = { "copts-output",
        "# Output\n\nName of the built executable. Empty: derived from the "
        "source/project.\n\n"
        "What `$(TargetFileName)` expands to and what Debug launches." },
    [HELP_COPTS_OPTIONS] = { "copts-options",
        "# Other command-line options, passed to cake as typed\n"
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
        "Most warnings are on unless `-wd<number>` turns them off, but a few "
        "are off until asked for:\n"
        "\n"
        "| Number | Warning |\n"
        "|---|---|\n"
        "| `2` | unused variable |\n"
        "| `6` | unused function parameter |\n"
        "| `11` | style |\n"
        "| `83` | parameter set but not used |\n"
        "| `84` | variable set but not used |\n"
        "\n"
        "With `-Werror`, notes are not affected and disabled warnings stay "
        "disabled. Because they become errors, warnings coming from included "
        "headers are no longer suppressed, and any occurrence makes the "
        "compilation fail.\n"
        "\n"
        "Suppress a diagnostic on one line with a trailing `lint` comment "
        "listing its number(s): `//lint 35`, `// lint 35`, or `/* lint 81 */`. "
        "An unnecessary suppression is flagged with warning 59.\n"
        "\n"
        "## Analysis\n"
        "\n"
        "| Option | Effect |\n"
        "|---|---|\n"
        "| `-flow` | run flow analysis (`#pragma flow enable`) |\n"
        "| `-check-annotations` | `_Owner`, `_Opt`, `_Out`... are checked; without it "
        "they are ignored (`#pragma check_annotations enable`) |\n"
        "| `-default-nonnull` | pointers without `_Opt` are non-null "
        "(`#pragma default_nonnull`) |\n"
        "| `-no-discard` | make `[[nodiscard]]` the default for every function "
        "|\n"
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
        "| `-preprocess-def-macro` | preprocess `#define` macros after "
        "expansion |\n"
        "| `-keep-inactive-tokens` | keep the tokens of inactive blocks (`#if "
        "0`) instead of discarding them |\n"
        "\n"
        "## Output\n"
        "\n"
        "| Option | Effect |\n"
        "|---|---|\n"
        "| `-o <name.c>` | output file name, when compiling a single file |\n"
        "| `-dont-generate-time-stamp` | leave the timestamp comment out of "
        "the generated file |\n"
        "| `-msvc-output` | diagnostics for the Visual Studio error parser "
        "(`-fdiagnostics-format=msvc` plus no colors) |\n"
        "| `-fdiagnostics-color=never` | no ANSI colors in diagnostics |\n"
        "| `-sarif` | also write SARIF diagnostic files |\n"
        "| `-sarif-path <dir>` | directory for the SARIF files |\n"
        "\n"
        "## Formatting\n"
        "\n"
        "| Option | Effect |\n"
        "|---|---|\n"
        "| `-format` | reformat the file to match the Style and print it "
        "instead of compiling |\n"
        "| `-format-lines=<first>:<last>` | restrict `-format` to a line range "
        "|\n"
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
        "| `-auto-config` | generate `cake.json` with the include "
        "directories of the current system |" },
    [HELP_COPTS_CONFIG] = { "copts-configuration",
        "# Configuration\n\nA named set of options\n"
        "\n"
        "A project has a list of configurations, each with its own options: "
        "target, headers, include directories, the build command, the debugger. "
        "The combo picks the one being edited; **Build > Configuration** picks "
        "the one Build uses.\n"
        "\n"
        "**New...** copies the configuration shown under a new name, **Rename...** "
        "renames it, **Delete** removes it. **Auto Config** creates a Debug and a "
        "Release configuration for every compiler on this machine."
        "\n\n## See also\n\n"
        "- [Target](help:copts-target)\n"
        "- [Properties](help:copts)\n" },
    [HELP_DEBUGGER_CDB] = { "debugger-cdb",
        "# cdb\n\n"
        "Microsoft's console debugger - the same engine as WinDbg, without the "
        "window. "
        "Start Debugging (F5) runs it as `cdb -lines <program> <arguments>`.\n"
        "\n"
        "It reads the PDB debug info the MSVC compiler writes (`/Zi`), so "
        "build with a `*_msvc` target.\n"
        "\n"
        "## Download\n"
        "\n"
        "- WinDbg, which includes cdb: `winget install Microsoft.WinDbg`\n"
        "- or the Windows SDK installer, feature **Debugging Tools for "
        "Windows**: "
        "https://developer.microsoft.com/windows/downloads/windows-sdk/\n"
        "\n"
        "`cdb.exe` must be on the PATH." },
    [HELP_DEBUGGER_LLDB] = { "debugger-lldb",
        "# lldb\n\n"
        "The LLVM project's debugger. Start Debugging (F5) runs it as "
        "`lldb --no-use-colors -x -- <program> <arguments>`.\n"
        "\n"
        "## Download\n"
        "\n"
        "- macOS: `xcode-select --install`\n"
        "- Linux: the `lldb` package, e.g. `sudo apt install lldb`\n"
        "- or https://releases.llvm.org\n"
        "\n"
        "`lldb` must be on the PATH." },
    [HELP_DEBUG_COMMAND] = { "debug-command",
        "# Command\n\nProgram to debug - usually `$(TargetPath)`, the built "
        "target.\n\nThe **...** button browses for a program."
        "\n\n## See also\n\n"
        "- [Debugger](help:debug-options)\n"
        "- [macros](help:ext-arguments)\n" },
    [HELP_DEBUG_ARGUMENTS] = { "debug-arguments",
        "# Arguments\n\nCommand-line arguments passed to the program. Use "
        "\"quotes\" for an argument with spaces.\n\nThe **>** button "
        "inserts a macro at the caret." },
    [HELP_DEBUG_DIRECTORY] = { "debug-directory",
        "# Directory\n\nDirectory the program runs in. Empty: the target's "
        "directory, `$(TargetDir)`.\n\nThe **>** button inserts a macro at "
        "the caret, e.g. `$(ProjectDir)`." },
    [HELP_DEBUG_OPTIONS] = { "debug-options",
        "# Debugger\n\n"
        "What Start Debugging (F5) runs after the Build ends without errors - "
        "a page of [Properties](help:copts), for each target and configuration.\n"
        "\n"
        "- **Debugger** - `cdb` on Windows, `lldb` elsewhere\n"
        "- **Command** - the program to debug, usually `$(TargetPath)`\n"
        "- **Arguments** - its command line\n"
        "- **Directory** - where it runs; empty: `$(TargetDir)`\n"
        "\n"
        "Breakpoints map back to the C source through `#line` directives, so "
        "`-line-directives` must be on (the **Compiler** page)."
        "\n\n## See also\n\n"
        "- [Command](help:debug-command)\n"
        "- [Arguments](help:debug-arguments)\n"
        "- [Directory](help:debug-directory)\n"
        "- [Debug Info](help:debug-info)\n" },
    [HELP_DEBUG_INFO] = { "debug-info",
        "# Debug Info\n\n"
        "While the program is stopped - at a breakpoint or after a step - "
        "shows its local variables "
        "and the call stack, asked of the debugger at every stop. Cleared when "
        "the program runs again.\n"
        "\n"
        "**Debug > Debug Info** shows or hides it." },
    [HELP_BUILD_OPTIONS] = { "build-options",
        "# Build\n\n"
        "Build (F7), Rebuild, Compile (Ctrl+F7) and Start Debugging (F5) run a "
        "pipeline:\n"
        "\n"
        "1. **Cake** - the project's changed `.c` files (Compile: only the "
        "active file; the active file too without a project, or in the Playground)\n"
        "2. the **command** of this page - e.g. the C compiler that builds the "
        "program from Cake's output\n"
        "3. the debugger - F5 only\n"
        "\n"
        "Any error stops the pipeline: Cake reporting errors, or the command not "
        "starting or ending with an exit code other than 0. When every file is up "
        "to date Cake compiles nothing and the pipeline goes on.\n"
        "\n"
        "A page of [Properties](help:copts), for each target and configuration: "
        "**Output** (the built program's name), and the command - **Command**, "
        "**Arguments**, **Directory**, as an External Tool's. Empty Command: "
        "nothing runs. [Auto Config](help:auto-config) fills it."
        "\n\n## See also\n\n"
        "- [Post-Build Event](help:post-build)\n"
        "- [Arguments and macros](help:ext-arguments)\n" },
    [HELP_PRE_BUILD] = { "pre-build",
        "# Pre-Build Event\n\n"
        "A command run before Cake compiles - to generate sources, say. If it "
        "cannot start or ends with an exit code other than 0, the Build stops "
        "there.\n"
        "\n"
        "Not shown in [Properties](help:copts) for now; a `.cakeproj` that has "
        "one still runs it."
        "\n\n## See also\n\n"
        "- [Build](help:build-options)\n" },
    [HELP_POST_BUILD] = { "post-build",
        "# Command run after Cake\n\n"
        "Run after Cake compiles without errors - usually the C compiler that "
        "builds the program from Cake's output, `$(CakeOutput)`, into "
        "`$(TargetPath)`.\n"
        "\n"
        "It also runs when every file is up to date. If it cannot start or "
        "ends with an exit code other than 0, the pipeline stops there and F5 "
        "does not start the debugger.\n"
        "\n"
        "Empty: nothing runs. [Auto Config](help:auto-config) sets it for the "
        "compilers found on this machine."
        "\n\n## See also\n\n"
        "- [Build](help:build-options)\n"
        "- [Arguments and macros](help:ext-arguments)\n"
        "- [Debugger](help:debug-options)\n" },
    [HELP_FIND_LOOK_IN] = { "find-look-in",
        "# Where to search\n"
        "\n"
        "Current Dir and Include Dir search one level only - subdirectories "
        "are not entered." },
    [HELP_LOOK_IN_FILE] = { "look-in-file",
        "## Current File\n\nthe active document\n"
        "\n"
        "Searches the text in its editor, including unsaved changes." },
    [HELP_LOOK_IN_DIR] = { "look-in-dir",
        "## Current Dir\n\nevery file in the active document's folder\n"
        "\n"
        "Only files matching **File Types**. A file that is open in an "
        "editor is searched in its editor, unsaved changes included; the "
        "others are read from disk." },
    [HELP_LOOK_IN_INCLUDE_DIRS] = { "look-in-include-dirs",
        "## Include Dir\n\nthe directories the compiler searches for headers\n"
        "\n"
        "Cake's own `include` folder next to the executable first, then the "
        "[include directories](help:include-dirs) of the target in use of the "
        "active file - the project's or the Playground's - in that order.\n"
        "\n"
        "Search only - **Replace** never rewrites system headers." },
    [HELP_LOOK_IN_PROJECT] = { "look-in-project",
        "## Project\n\nevery file in the open project\n"
        "\n"
        "Only files matching **File Types**. Open files are searched in "
        "their editor, unsaved changes included." },
    [HELP_INCLUDE_DIRS] = { "include-dirs",
        "# Include Directories\n"
        "\n"
        "The `#include` search path\n"
        "\n"
        "A page of [Properties](help:copts), for each configuration. \n"
        "\n"
        "Every directory goes to Cake as `-I`, with `-no-includes`: "
        "`cake.json` is not\n"
        "read, so this list must have the system headers too - "
        "[AutoConfig](help:auto-config) \n"
        "puts the compiler's there. The C compiler after Cake does not need "
        "them: \n"
        "Cake's output has no `#include`.\n"
        "\n"
        "Directories are searched in list order - **Move Up** / **Move Down** "
        "change it.\n"
        "A project's are stored relative to the project folder; the "
        "Playground's as full\n"
        "paths.\n"
        "\n"
        "Cake's own annotated headers (the `include` folder next to the "
        "executable) are\n"
        "always searched first and are not listed here." },
    [HELP_PLAYGROUND] = { "playground",
        "# Playground\n\na scratch file, and the project of every loose file\n"
        "\n"
        "**View > Playground** opens `playground.c`. Its options are a project's - "
        "`playground.cakeproj`, next to it - used by every file that is not part of "
        "the open project: the Playground, a file opened on its own. Each compiles "
        "itself, with the Playground project's target, configuration, includes, "
        "build command and debugger.\n"
        "\n"
        "**Project > Properties...** edits them when no project is open, or when the "
        "active file is not part of it - the title says **Properties (Playground)**. "
        "A new project starts as a copy of them."
        "\n\n## See also\n\n"
        "- [Properties](help:copts)\n"
        "- [New project](help:new-project)\n" },
};

static void build_help(struct ide* ide)
{
    struct help_window* h = &ide->help;
    h->window = new_dialog(ide, "Help");
    gui_window_set_resizable(h->window, 1);
    gui_window_set_min_size(h->window, 30, 8);
    h->editor = create(ide, GUI_EDITOR, NULL);
    fill_margins(h->editor, 2, 1, 2, 4);
    gui_append(h->window, h->editor);
    gui_editor_set_read_only(h->editor, 1);
    gui_editor_set_click_id(h->editor, EV_HELP_CTRLCLICK);
    /* Back and Close centered as a pair, as External Tools' OK and Cancel */
    static const struct { int id; const char* label; int offset; } bottom[] = {
        { EV_HELP_BACK, "Back", -11 }, { EV_HELP_CLOSE, "Close", 1 },
    };
    for (int i = 0; i < COUNT(bottom); i++)
    {
        struct gui_node* n = create(ide, GUI_BUTTON, bottom[i].label);
        struct gui_layout l = { GUI_ANCHOR_LEFT | GUI_ANCHOR_BOTTOM };
        l.left.percent = 50;
        l.left.cells = bottom[i].offset;
        l.bottom.cells = 2;
        l.width.cells = 10;
        l.height.cells = 1;
        gui_set_layout(n, &l);
        gui_set_id(n, bottom[i].id);
        gui_append(h->window, n);
        if (bottom[i].id == EV_HELP_BACK)
            h->back = n;
    }
    h->current = HELP_NONE;
#ifndef NDEBUG
    /* every help:<slug> link names a topic */
    for (int t = 0; t < HELP_COUNT; t++)
    {
        for (const char* l = strstr(help_topics[t].text, "(help:"); l; l = strstr(l + 1, "(help:"))
        {
            const char* slug = l + 6;
            const char* end = strchr(slug, ')');
            assert(end && help_find(slug, (int)(end - slug)) != HELP_NONE);
        }
    }
#endif
}

/* `node`'s help: `hint` in the statusbar under the mouse, `topic` on F1 -
 * as the old IDE's ui_set_help(node, hint, text). */
static void set_help(struct ide* ide, struct gui_node* node, const char* hint, enum help_id topic)
{
    struct help_window* h = &ide->help;
    if (!node)
        return;
    if (hint)
        gui_set_hint(node, hint);
    if (topic == HELP_NONE)
        return;
    for (int i = 0; i < h->doc_count; i++)
    {
        if (h->docs[i].node == node)
        {
            h->docs[i].topic = topic;   /* a dialog whose text follows what it edits */
            return;
        }
    }
    if (h->doc_count < COUNT(h->docs))
    {
        h->docs[h->doc_count].node = node;
        h->docs[h->doc_count].topic = topic;
        h->doc_count++;
    }
}

static enum help_id help_of(struct ide* ide, const struct gui_node* node)
{
    for (int i = 0; node && i < ide->help.doc_count; i++)
    {
        if (ide->help.docs[i].node == node)
            return ide->help.docs[i].topic;
    }
    return HELP_NONE;
}

/* The topic whose slug is the `len` bytes at `slug`, or HELP_NONE. */
static enum help_id help_find(const char* slug, int len)
{
    for (int t = 0; t < HELP_COUNT; t++)
    {
        if ((int)strlen(help_topics[t].slug) == len && memcmp(help_topics[t].slug, slug, (size_t)len) == 0)
            return (enum help_id)t;
    }
    return HELP_NONE;
}

static void help_history_push(struct help_history* h, enum help_id topic)
{
    if (h->count == h->cap)
    {
        int cap = h->cap ? h->cap * 2 : 16;
        void* items = realloc(h->items, (size_t)cap * sizeof *h->items);
        if (!items)
            return;
        h->items = items;
        h->cap = cap;
    }
    h->items[h->count++] = topic;
}

static enum help_id help_history_pop(struct help_history* h)
{
    return h->count > 0 ? h->items[--h->count] : HELP_NONE;
}

/* Display width of a Markdown table cell as the read-only Markdown editor
 * draws it: inline `code` backticks and **bold** markers are hidden there
 * (the Markdown highlighter's GUI_SPAN_HIDDEN), and a UTF-8 sequence is
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
    const char* cell[MD_TABLE_MAX_ROWS][MD_TABLE_MAX_COLS] = { 0 };
    size_t cell_len[MD_TABLE_MAX_ROWS][MD_TABLE_MAX_COLS] = { 0 };
    int ncell[MD_TABLE_MAX_ROWS] = { 0 };
    int is_rule[MD_TABLE_MAX_ROWS] = { 0 };
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
            const char* rows[MD_TABLE_MAX_ROWS] = { 0 };
            size_t row_len[MD_TABLE_MAX_ROWS] = { 0 };
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

/* `md` in the help window: its paragraphs wrapped to the window and its
 * tables lined up (md_wrap_paragraphs), read-only so the delimiters are
 * hidden - the old IDE's. */
static void help_set_markdown(struct ide* ide, const char* md)
{
    size_t cap = strlen(md) * 2 + 4096;   /* wrapping and padding only add spaces and line ends */
    char* wrapped = malloc(cap);
    if (wrapped)
        md_wrap_paragraphs(md, 76 - 3, wrapped, cap);   /* the editor's 76 columns, as the old IDE's ew - 3 */
    gui_set_value(ide->help.editor, wrapped ? wrapped : md);
    gui_editor_set_highlighter(ide->help.editor, &ide->md_highlighter);
    free(wrapped);
}

/* The Open dialog's F1, by what it was opened for - the old IDE's
 * open_dialog_overview. NULL where it explains itself. */
static const char* open_overview(struct ide* ide)
{
    const struct open_dialog* o = &ide->open;
    if (o->open_project)
        return "# Open a Cake project (`.cakeproj`)\n"
            "\n"
            "Pick the project's `.cakeproj` file.\n"
            "\n"
            "A project is a `.cakeproj` file: a list of source files, plus the include directories and compiler options used to build them. File paths inside the project folder are stored relative to it, so the project can be moved or shared.\n"
            "\n"
            "## Build and Compile\n"
            "\n"
            "- **Build** (F7) compiles every `.c` file of the project in one Cake invocation - linking them is the output compiler's job. With the Playground active (or no project open), Build compiles just the active file.\n"
            "- **Compile** (Ctrl+F7) always compiles only the active file.\n"
            "\n"
            "## Project settings vs. the Playground's\n"
            "\n"
            "A project's files use the project's options, saved in its `.cakeproj`; every other file uses the Playground project's. **Project > Properties...** edits the one the active file uses. A new project starts with the Playground's options - press **Auto Config** there to set its compilers.";
    if (o->add_to_project)
        return "# Add existing source files to the open project\n"
            "\n"
            "Check several files to add them all at once. Files inside the project folder are stored relative to it; files elsewhere keep their full path. A file already in the project is not added twice.";
    if (o->pick_include)
        return "# Pick a directory to add to the include directory list\n"
            "\n"
            "The directory is searched for `#include` files, in list order - **Move Up** / **Move Down** on the Includes page change that order. For a project the path is stored relative to the project folder; for the Playground it is stored as a full path.";
    return NULL;
}

/* F1: the focused field's help, else its dialog's, else the IDE's. */
/* `topic` in the Help window; Back leads to what it showed before. */
static void help_show(struct ide* ide, enum help_id topic, const char* text)
{
    struct help_window* h = &ide->help;
    h->current = topic;
    help_set_markdown(ide, topic != HELP_NONE ? help_topics[topic].text : text);
    gui_set_enabled(h->back, h->history.count > 0);
}

/* A help:<slug> link: its topic, the one shown now kept for Back. */
static void help_follow(struct ide* ide, enum help_id topic)
{
    struct help_window* h = &ide->help;
    if (h->current != HELP_NONE)
        help_history_push(&h->history, h->current);
    help_show(ide, topic, NULL);
}

static void help_back(struct ide* ide)
{
    enum help_id topic = help_history_pop(&ide->help.history);
    if (topic != HELP_NONE)
        help_show(ide, topic, NULL);
}

static void help_open(struct ide* ide)
{
    struct gui_node* focused = gui_focused(ide->app);
    enum help_id topic = help_of(ide, gui_focused_item(ide->app));
    if (topic == HELP_NONE)
        topic = help_of(ide, focused);
    const char* text = NULL;
    for (int i = gui_window_count(ide->app) - 1; topic == HELP_NONE && !text && i >= 0; i--)
    {
        struct gui_node* win = gui_window_at(ide->app, i);
        if (win != ide->help.window)
        {
            if (win == ide->open.window)
                text = open_overview(ide);
            else
                topic = help_of(ide, win);
            if (gui_window_get_dock(win) == GUI_DOCK_NONE)
                break;   /* only the window on top */
        }
    }
    if (topic == HELP_NONE && !text)
        topic = HELP_OVERVIEW;
    ide->help.history.count = 0;
    help_show(ide, topic, text);
    show_resizable_dialog(ide, ide->help.window, 80, 20, ide->help.editor);
}

/* The Open dialog as a picker: a folder (or, with `file`, a file) goes
 * into `input`, or with `input` NULL into the include list being edited. */
static void pick_path(struct ide* ide, struct gui_node* input, int file, const char* dir)
{
    struct open_dialog* o = &ide->open;
    char start[1024] = { 0 };
    if (dir[0])
        snprintf(start, sizeof start, "%s", dir);
    else
        start_dir(ide, start, sizeof start);
    show_open(ide, 0, start, "");
    open_folder_mode(ide, !file);
    o->pick_input = input;
    o->pick_include = input == NULL;
    gui_set_label(o->window, file ? "Choose a File" : "Choose a Folder");
    gui_set_label(o->ok, file ? "Open" : "Select");
}

/* --- Help texts: the old IDE's ui_set_help, word for word --- */

static void build_help_texts(struct ide* ide)
{
    struct copts_dialog* c = &ide->copts;
    set_help(ide, ide->clone.window,
             "Copy a remote Git repository to a local folder",
             HELP_CLONE);
    set_help(ide, ide->clone.url,
             "The repository's URL, e.g. `https://github.com/user/repo.git`",
             HELP_CLONE_URL);
    set_help(ide, ide->clone.path,
             "The new folder the repository is cloned into",
             HELP_CLONE_PATH);
    set_help(ide, gui_child_at(ide->clone.open_folder, 0),
             "Open Folder: show the cloned folder in the Folder panel when done",
             HELP_CLONE_OPEN_FOLDER);
    set_help(ide, ide->new_project.window,
             "Create a new Cake project (`.cakeproj`)",
             HELP_NEW_PROJECT);
    set_help(ide, ide->new_project.name,
             "Name of the project file: `<name>.cakeproj`",
             HELP_NEW_PROJECT_NAME);
    set_help(ide, gui_child_at(ide->new_project.checks, 0),
             "Create Folder: put the project in a new `<Project Name>` subfolder of Folder",
             HELP_NEW_PROJECT_CREATE_FOLDER);
    set_help(ide, gui_child_at(ide->new_project.checks, 1),
             "Hello World: start the project with a `main.c` that prints \"Hello, world!\"",
             HELP_NEW_PROJECT_HELLO_WORLD);
    set_help(ide, c->window,
             "Properties: how the project - or the Playground - is built and debugged",
             HELP_COPTS);
    set_help(ide, c->configs,
             "The configuration being edited",
             HELP_COPTS_CONFIG);
    set_help(ide, c->config_buttons[0],
             "New: a copy of the configuration shown, with a new name",
             HELP_COPTS_CONFIG);
    set_help(ide, c->config_buttons[1],
             "Rename the configuration shown",
             HELP_COPTS_CONFIG);
    set_help(ide, c->config_buttons[2],
             "Delete the configuration shown",
             HELP_COPTS_CONFIG);
    set_help(ide, c->buttons[3],
             "Auto Config: a Debug and a Release configuration for every compiler found on this machine",
             HELP_AUTO_CONFIG);
    set_help(ide, c->includes,
             "The #include search path, passed as -I; Auto Config adds the compiler's headers",
             HELP_INCLUDE_DIRS);
    static const char* const page_hints[COPTS_PAGES] = {
        "Compiler: how Cake compiles", "Includes: the #include search path",
        "Build: the output name and the command run after Cake", "Debugger: the program F5 runs",
    };
    static const enum help_id page_topics[COPTS_PAGES] = { HELP_COPTS, HELP_INCLUDE_DIRS, HELP_BUILD_OPTIONS, HELP_DEBUG_OPTIONS };
    for (int i = 0; i < COPTS_PAGES; i++)
        set_help(ide, gui_child_at(c->pages, i), page_hints[i], page_topics[i]);
    set_help(ide, c->headers,
             "Which headers `#include <...>` finds (`-cake-headers`)",
             HELP_COPTS_HEADERS);
    set_help(ide, gui_child_at(c->headers, 0),
             "System Headers: Cake's header if it has one, then the system header",
             HELP_HEADERS_SYSTEM);
    set_help(ide, gui_child_at(c->headers, 1),
             "`-cake-headers`: Cake's header if it has one, without the system header",
             HELP_HEADERS_CAKE);
    set_help(ide, c->style,
             "Coding style checked by diagnostic 11 (`-style=<name>`)",
             HELP_COPTS_STYLE);
    set_help(ide, gui_child_at(c->style, 0),
             "No style check",
             HELP_STYLE_NONE);
    set_help(ide, gui_child_at(c->style, 1),
             "`-style=cake`: checks the code against Cake's own style",
             HELP_STYLE_CAKE);
    set_help(ide, gui_child_at(c->style, 2),
             "`-style=gnu`: checks the code against the GNU style",
             HELP_STYLE_GNU);
    set_help(ide, gui_child_at(c->style, 3),
             "`-style=microsoft`: checks the code against the Microsoft style",
             HELP_STYLE_MICROSOFT);
    set_help(ide, c->diag,
             "How diagnostic positions are printed (`-fdiagnostics-format=<format>`)",
             HELP_COPTS_DIAG);
    set_help(ide, gui_child_at(c->diag, 0),
             "`-fdiagnostics-format=ide`: file.c:1:2: warning 10: message",
             HELP_DIAG_IDE);
    set_help(ide, gui_child_at(c->diag, 1),
             "`-fdiagnostics-format=gcc`: file.c:1:2: warning 10: message",
             HELP_DIAG_GCC);
    set_help(ide, gui_child_at(c->diag, 2),
             "`-fdiagnostics-format=msvc`: file.c(1,2): warning 10: message",
             HELP_DIAG_MSVC);
    set_help(ide, gui_child_at(c->flags, 0),
             "`-line-directives`: emit `#line` directives in the generated C89 output",
             HELP_FLAG_LINE_DIRECTIVES);
    set_help(ide, gui_child_at(c->flags, 1),
             "`-flow`: run Cake's built-in flow analysis",
             HELP_FLAG_FLOW);
    set_help(ide, gui_child_at(c->flags, 2),
             "`-const-literal`: treat string literals as `const char[]` rather than `char[]`",
             HELP_FLAG_CONST_LITERAL);
    set_help(ide, gui_child_at(c->flags, 3),
             "`-Wall`: enable all warnings",
             HELP_FLAG_WALL);
    set_help(ide, gui_child_at(c->flags, 4),
             "`-default-nonnull`: pointers without `_Opt` are non-null",
             HELP_FLAG_DEFAULT_NONNULL);
    set_help(ide, gui_child_at(c->flags, 5),
             "`-check-annotations`: `_Owner`, `_Opt`, `_Out`... are checked",
             HELP_FLAG_ANNOTATIONS);
    set_help(ide, c->output,
             "Name of the built executable (empty: derived from the source/project)",
             HELP_COPTS_OUTPUT);
    set_help(ide, c->cake_target,
             "Passed to cake as `-target=`",
             HELP_COPTS_TARGET);
    set_help(ide, c->options,
             "Other command-line options, passed to cake as typed",
             HELP_COPTS_OPTIONS);
#if defined(_WIN32)
    set_help(ide, c->debugger,
             "`cdb`: Microsoft's console debugger (Debugging Tools for Windows)",
             HELP_DEBUGGER_CDB);
#else
    set_help(ide, c->debugger,
             "`lldb`: the LLVM debugger",
             HELP_DEBUGGER_LLDB);
#endif
    set_help(ide, c->debug[0],
             "Program to debug - usually `$(TargetPath)`",
             HELP_DEBUG_COMMAND);
    set_help(ide, c->debug[1],
             "Command-line arguments passed to the program",
             HELP_DEBUG_ARGUMENTS);
    set_help(ide, c->debug[2],
             "Directory the program runs in (empty: the target's directory)",
             HELP_DEBUG_DIRECTORY);
    set_help(ide, ide->debug_info_window,
             "Debug Info: the Locals and Call Stack of the stopped program",
             HELP_DEBUG_INFO);
    set_help(ide, c->post_build[0],
             "Command run after Cake compiles without errors - e.g. the C compiler",
             HELP_POST_BUILD);
}

/* --- Window > Tile / Cascade: the old IDE's algorithms, in whole cells,
 * on the windows that are not docked. --- */

static int floating_windows(struct ide* ide, struct gui_node** out, int max)
{
    int n = 0;
    for (int i = 0; i < gui_window_count(ide->app) && n < max; i++)
    {
        struct gui_node* win = gui_window_at(ide->app, i);
        if (gui_window_get_dock(win) == GUI_DOCK_NONE)
            out[n++] = win;
    }
    return n;
}

static void tile(struct ide* ide)
{
    struct gui_node* wins[MAX_DOCS] = { 0 };
    int n = floating_windows(ide, wins, MAX_DOCS);
    if (n == 0)
        return;
    int cw = 0, ch = 0;
    gui_cell_size(ide->app, &cw, &ch);
    struct gui_rect desk = gui_desktop_rect(ide->app);
    int x0 = desk.x / cw, y0 = desk.y / ch, area_w = desk.w / cw, area_h = desk.h / ch;
    int cols = 1;
    while (cols * cols < n)
        cols++;
    int rows = (n + cols - 1) / cols;
    int cell_w = area_w / cols, cell_h = area_h / rows;
    for (int i = 0; i < n; i++)
    {
        int c = i % cols, r = i / cols;
        int x = x0 + c * cell_w, y = y0 + r * cell_h;
        int w = (c == cols - 1) ? x0 + area_w - x : cell_w;
        int h = (r == rows - 1) ? y0 + area_h - y : cell_h;
        struct gui_rect rect = { x * cw, y * ch, w * cw, h * ch };
        gui_window_set_rect(wins[i], &rect);
    }
}

static void cascade(struct ide* ide)
{
    struct gui_node* wins[MAX_DOCS] = { 0 };
    int n = floating_windows(ide, wins, MAX_DOCS);
    if (n == 0)
        return;
    int cw = 0, ch = 0;
    gui_cell_size(ide->app, &cw, &ch);
    struct gui_rect desk = gui_desktop_rect(ide->app);
    int x0 = desk.x / cw, y0 = desk.y / ch, area_w = desk.w / cw, area_h = desk.h / ch;
    int w = area_w * 3 / 4, h = area_h * 3 / 4;
    if (w < 20) w = area_w < 20 ? area_w : 20;
    if (h < 8) h = area_h < 8 ? area_h : 8;
    int max_off_x = area_w - w > 0 ? area_w - w : 1;
    int max_off_y = area_h - h > 0 ? area_h - h : 1;
    for (int i = 0; i < n; i++)
    {
        struct gui_rect rect = { (x0 + (i * 2) % max_off_x) * cw, (y0 + i % max_off_y) * ch,
                                 w * cw, h * ch };
        gui_window_set_rect(wins[i], &rect);
    }
}

/* Dock Left/Right/Bottom on the panel the popup opened over: refused when
 * another panel holds that side (the old IDE's dock_panel_to). */
static void dock_panel(struct ide* ide, enum gui_dock side)
{
    struct gui_node* win = gui_context_target(ide->app);
    if (!win)
        return;
    for (int i = 0; i < gui_window_count(ide->app); i++)
    {
        struct gui_node* other = gui_window_at(ide->app, i);
        if (other != win && gui_window_get_dock(other) == side)
        {
            char msg[200] = { 0 };
            snprintf(msg, sizeof msg, "There is already a panel there (%s)", gui_get_label(other));
            status(ide, msg);
            return;
        }
    }
    int cw = 0, ch = 0;
    gui_cell_size(ide->app, &cw, &ch);
    struct gui_rect desk = gui_desktop_rect(ide->app);
    int size = side == GUI_DOCK_BOTTOM ? (desk.h / ch / 4) * ch : (desk.w / cw / 4) * cw;
    gui_window_set_dock(win, side, size);
}

/* --- Compile: the active file, with its project's Properties (or the
 * Playground project's), as the old IDE's do_compile --- */

static const char* const style_slugs[] = { "", "cake", "gnu", "microsoft" };
static const char* const diag_slugs[] = { "ide", "gcc", "msvc" };

/* The options Build, Compile and Start Debugging use: the target's and configuration's in use. */
static const struct target_settings* settings_in_use(const struct compiler_settings* cs)
{
    static const struct target_settings none = { 0 };
    return cs->current >= 0 && cs->current < cs->count ? &cs->configurations[cs->current] : &none;
}

/* New settings: one configuration, every field empty but -line-directives. */
static void compiler_settings_default(struct compiler_settings* s)
{
    memset(s, 0, sizeof *s);
    s->count = 1;
    s->current = 0;
    config_default(&s->configurations[0]);
    snprintf(s->configurations[0].name, sizeof s->configurations[0].name, "Default");
}

/* The command line the Properties of the target and configuration in use
 * say, as the old job_argv_from_settings. */
static void compile_args(struct ide* ide, const struct compiler_settings* cs)
{
    const struct target_settings* s = settings_in_use(cs);
    struct ide_compile_job* job = ide->job;
    char flag[1100] = { 0 };
    ide_compile_reset(job);
    ide_compile_arg(job, "cake");
    ide_compile_arg(job, "-no-includes");   /* not cake.json's: the target's include directories, as -I */
    if (s->diag >= 0 && s->diag < COUNT(diag_slugs))
    {
        snprintf(flag, sizeof flag, "-fdiagnostics-format=%s", diag_slugs[s->diag]);
        ide_compile_arg(job, flag);
    }
    if (s->cake_target[0])
    {
        snprintf(flag, sizeof flag, "-target=%s", s->cake_target);
        ide_compile_arg(job, flag);
    }
    if (s->style > 0 && s->style < COUNT(style_slugs))
    {
        snprintf(flag, sizeof flag, "-style=%s", style_slugs[s->style]);
        ide_compile_arg(job, flag);
    }
    for (int i = 0; i < COUNT(copts_flags); i++)
    {
        if (s->flags[i])
            ide_compile_arg(job, copts_flags[i]);
    }
    if (s->headers == 1)
        ide_compile_arg(job, "-cake-headers");
    char options[sizeof s->options] = { 0 };
    snprintf(options, sizeof options, "%s", s->options);
    for (char* tok = strtok(options, " \t"); tok; tok = strtok(NULL, " \t"))
        ide_compile_arg(job, tok);
}

/* Drops the "\x1b[...m" color codes - the editor shows plain text. */
static void strip_ansi(char* s)
{
    char* w = s;
    for (char* r = s; *r;)
    {
        if (r[0] == '\x1b' && r[1] == '[')
        {
            char* p = r + 2;
            while (*p && (*p < 0x40 || *p > 0x7E))
                p++;
            if (*p)
            {
                r = p + 1;
                continue;
            }
        }
        if (*r == '\r')
        {
            r++;
            continue;
        }
        *w++ = *r++;
    }
    *w = '\0';
}

/* Output and Find Results share the bottom dock, which holds one window:
 * showing one closes the other, which keeps its text. */
static void bottom_panel_show(struct ide* ide, struct gui_node* show, struct gui_node* hide)
{
    if (is_open(ide, hide) && gui_window_get_dock(hide) == GUI_DOCK_BOTTOM &&
        gui_window_get_dock(show) == GUI_DOCK_BOTTOM)
    {
        int h = gui_window_get_rect(hide).h;
        gui_window_close(ide->app, hide);
        gui_window_set_dock(show, GUI_DOCK_BOTTOM, h);
    }
    gui_window_open(ide->app, show);
}

/* `text` with every line starting with the project folder made relative to
 * it - a double click finds them again through resolve_referenced_path. */
static char* project_relative_paths(struct ide* ide, const char* text)
{
    size_t n = strlen(text);
    char* out = malloc(n + 1);
    if (!out)
        return NULL;
    const char* dir = ide->project.dir;
    size_t dir_len = strlen(dir);
    if (!ide_project_is_open(&ide->project) || dir_len == 0)
    {
        memcpy(out, text, n + 1);
        return out;
    }
    char* w = out;
    for (const char* line = text; *line;)
    {
        /* leading color codes stay */
        const char* p = line;
        while (p[0] == '\x1b' && p[1] == '[')
        {
            const char* q = p + 2;
            while (*q && (*q < 0x40 || *q > 0x7E))
                q++;
            if (!*q)
                break;
            p = q + 1;
        }
        memcpy(w, line, (size_t)(p - line));
        w += p - line;
        size_t i = 0;
        for (; i < dir_len && p[i]; i++)
        {
            char a = dir[i], b = p[i];
            int same = (a == '/' || a == '\\') ? (b == '/' || b == '\\')
                                                : tolower((unsigned char)a) == tolower((unsigned char)b);
            if (!same)
                break;
        }
        if (i == dir_len && (p[i] == '/' || p[i] == '\\'))
            p += dir_len + 1;
        const char* end = strchr(p, '\n');
        size_t len = end ? (size_t)(end + 1 - p) : strlen(p);
        memcpy(w, p, len);
        w += len;
        line = p + len;
    }
    *w = '\0';
    return out;
}

/* The current search's results begin: the panel's text is kept to show
 * below them. */
static void findresults_begin(struct ide* ide)
{
    free(ide->fr.previous);
    const char* now = gui_get_value(ide->fr.editor);
    size_t n = strlen(now);
    ide->fr.previous = malloc(n + 1);
    if (ide->fr.previous)
        memcpy(ide->fr.previous, now, n + 1);
}

/* `text` - this search's results - above the previous searches, one blank
 * line between them. */
static void findresults_set(struct ide* ide, const char* text_in)
{
    char* shortened = project_relative_paths(ide, text_in);
    const char* text = shortened ? shortened : text_in;
    const char* previous = ide->fr.previous ? ide->fr.previous : "";
    if (text[0] == '\0' || previous[0] == '\0')
    {
        gui_set_value(ide->fr.editor, text[0] ? text : previous);
        free(shortened);
        return;
    }
    size_t len = strlen(text);
    while (len > 0 && (text[len - 1] == '\n' || text[len - 1] == '\r'))
        len--;
    while (*previous == '\n' || *previous == '\r')
        previous++;
    char* all = malloc(len + 2 + strlen(previous) + 1);
    if (all)
    {
        memcpy(all, text, len);
        all[len] = '\n';
        all[len + 1] = '\n';
        strcpy(all + len + 2, previous);
        gui_set_value(ide->fr.editor, all);
        free(all);
    }
    else
    {
        gui_set_value(ide->fr.editor, text);
    }
    free(shortened);
}

static void compile_show_output(struct ide* ide)
{
    char* text = malloc(strlen(ide_compile_output(ide->job)) + 1);
    if (!text)
        return;
    char* o = text;
    for (const char* p = ide_compile_output(ide->job); *p; p++)
    {
        if (*p != '\r')
            *o++ = *p;
    }
    *o = '\0';
    if (ide->find_kind != FIND_NONE)
    {
        findresults_set(ide, text);   /* on top of the previous searches */
        free(text);
        return;
    }
    gui_set_value(ide->output.editor, text);
    free(text);
    gui_editor_goto_line(ide->output.editor, 1 << 30);
}

static struct doc* find_doc(struct ide* ide, const char* name);
static void find_finish(struct ide* ide);

/* --- Back / Forward --- */

static int nav_capture(struct ide* ide, struct nav_pos* out)
{
    struct doc* d = active_doc(ide);
    if (!d)
        return 0;
    snprintf(out->path, sizeof out->path, "%s", d->path);
    int unused = 0;
    gui_editor_get_selection(d->editor, &out->caret, &unused);
    return 1;
}

static void nav_push(struct nav_stack* s, const struct nav_pos* pos)
{
    if (s->count == NAV_STACK_MAX)
    {
        memmove(&s->items[0], &s->items[1], (NAV_STACK_MAX - 1) * sizeof s->items[0]);
        s->count--;
    }
    s->items[s->count++] = *pos;
}

/* Before a jump (go to an error, a definition, a file): where it leaves. */
static void nav_record_jump(struct ide* ide)
{
    struct nav_pos here = { 0 };
    if (ide->nav.restoring || !nav_capture(ide, &here))
        return;
    nav_push(&ide->nav.back, &here);
    ide->nav.forward.count = 0;
}

static void nav_go(struct ide* ide, struct nav_stack* from, struct nav_stack* to)
{
    if (from->count == 0)
        return;
    struct nav_pos here = { 0 };
    int have_here = nav_capture(ide, &here);
    struct nav_pos target = from->items[--from->count];
    if (have_here)
        nav_push(to, &here);
    ide->nav.restoring = 1;
    open_file(ide, target.path);
    struct doc* d = find_doc(ide, target.path);
    if (d)
    {
        int len = (int)strlen(gui_get_value(d->editor));
        int caret = target.caret < len ? target.caret : len;
        gui_editor_set_selection(d->editor, caret, caret);
        gui_focus(ide->app, d->editor);
    }
    ide->nav.restoring = 0;
}

/* --- Diagnostics and going to them: the old IDE's parse_diagnostic_line,
 * apply_diagnostics and output_goto_source --- */

/* Case-insensitive comparison by file name: the output may name a file
 * with no path while the document has one, or the other way round. */
static int paths_match(const char* a, const char* b)
{
    a = file_name(a);
    b = file_name(b);
    while (*a && *b && tolower((unsigned char)*a) == tolower((unsigned char)*b))
    {
        a++;
        b++;
    }
    return *a == *b;
}

static const char* ci_strstr(const char* haystack, const char* needle)
{
    size_t n = strlen(needle);
    for (; *haystack; haystack++)
    {
        size_t i = 0;
        while (i < n && tolower((unsigned char)haystack[i]) == tolower((unsigned char)needle[i]))
            i++;
        if (i == n)
            return haystack;
    }
    return NULL;
}

/* The "file:line:col" (gcc/ide) or "file(line,col)" / "file(line)" (msvc)
 * position in `line`: returns the separator before the line number, NULL
 * if there is none. The first ':' followed by a digit skips drive letters. */
static char* position_separator(char* line, int* src_line)
{
    for (char* p = line; *p; p++)
    {
        if (*p == ':' && isdigit((unsigned char)p[1]))
        {
            *src_line = atoi(p + 1);
            return p;
        }
        if (*p == '(' && isdigit((unsigned char)p[1]))
        {
            char* q = p + 1;
            while (isdigit((unsigned char)*q))
                q++;
            if (*q != ',' && *q != ')')
                continue;
            *src_line = atoi(p + 1);
            return p;
        }
    }
    return NULL;
}

/* One diagnostic line: its file (cut in place), mark type, line, code
 * ("" when none: "82" from Cake, "C4133" from msvc) and message. 0 if
 * `line` is not a diagnostic. */
static int parse_diagnostic_line(char* line, char** file, enum gui_mark* type, int* src_line,
                                 char** code, char** message)
{
    strip_ansi(line);
    static const char* const keywords[] = { "error", "warning", "info", "note" };
    static const enum gui_mark types[] = { GUI_MARK_ERROR, GUI_MARK_WARNING, GUI_MARK_INFO, GUI_MARK_INFO };
    const char* kw = NULL;
    int found = -1;
    for (int i = 0; i < 4 && found < 0; i++)
    {
        size_t len = strlen(keywords[i]);
        for (const char* p = line; (p = ci_strstr(p, keywords[i])) != NULL; p++)
        {
            int prev_ok = p == line || (!isalnum((unsigned char)p[-1]) && p[-1] != '_');
            int next_ok = !isalnum((unsigned char)p[len]) && p[len] != '_';
            if (prev_ok && next_ok)
            {
                found = i;
                kw = p;
                break;
            }
        }
    }
    if (found < 0)
        return 0;
    *type = types[found];

    char* sep = position_separator(line, src_line);
    if (!sep)
        return 0;

    char* m = (char*)kw + strlen(keywords[found]);
    while (*m == ' ' || *m == '\t') m++;
    char* code_end = m;
    while (isalnum((unsigned char)*code_end))
        code_end++;
    char* after = code_end;
    while (*after == ' ' || *after == '\t') after++;
    if (code_end > m && *after == ':')
    {
        *code = m;
        m = after + 1;
        *code_end = '\0';
    }
    else
    {
        *code = "";
        if (*m == ':') m++;
    }
    while (*m == ' ' || *m == '\t') m++;
    char* msg = (char*)m;
    char* end = msg + strlen(msg);
    while (end > msg && (end[-1] == '\n' || end[-1] == '\r' || end[-1] == ' ' || end[-1] == '\t'))
        end--;
    *end = '\0';
    *message = msg;

    *sep = '\0';
    char* f = line;
    while (*f == ' ' || *f == '\t')
        f++;
    *file = f;
    return 1;
}

/* Each diagnostic line of `text` marked on the document of the file it
 * names, after clearing every document's marks when `clear` - Cake's
 * compile clears, an External Tool's output (msvc, gcc) adds to it.
 * Changes `text`. */
static void apply_diagnostics(struct ide* ide, char* text, int clear)
{
    for (int i = 0; clear && i < ide->doc_count; i++)
        gui_editor_clear_marks(ide->docs[i].editor);
    for (char* line = strtok(text, "\n"); line; line = strtok(NULL, "\n"))
    {
        char* file = 0;
        char* message = 0;
        enum gui_mark type = 0;
        char* code = 0;
        int src_line = 0;
        if (!parse_diagnostic_line(line, &file, &type, &src_line, &code, &message))
            continue;
        static const char* const tags[] = { "info", "warning", "error" };
        char mark[1200] = { 0 };
        if (code[0])
            snprintf(mark, sizeof mark, " \xE2\x86\x90 %s %s: %s", tags[type], code, message);
        else
            snprintf(mark, sizeof mark, " \xE2\x86\x90 %s: %s", tags[type], message);
        for (int i = 0; i < ide->doc_count; i++)
        {
            if (paths_match(ide->docs[i].path, file))
            {
                gui_editor_add_mark(ide->docs[i].editor, type, src_line, mark);
                break;
            }
        }
    }
}

/* Every document without unsaved edits takes what is on disk now, if that
 * changed - a build may have rewritten files. The caret stays. */
static void refresh_open_docs(struct ide* ide)
{
    for (int i = 0; i < ide->doc_count; i++)
    {
        struct doc* d = &ide->docs[i];
        if (gui_editor_get_dirty(d->editor))
            continue;
        int crlf = 0;
        char* text = ide_read_file(d->path, &crlf);
        if (!text)
            continue;
        if (strcmp(text, gui_get_value(d->editor)) != 0)
        {
            int lo = 0, hi = 0;
            gui_editor_get_selection(d->editor, &lo, &hi);
            gui_set_value(d->editor, text);
            int len = (int)strlen(text);
            gui_editor_set_selection(d->editor, lo < len ? lo : len, lo < len ? lo : len);
            gui_editor_set_dirty(d->editor, 0);
            d->crlf = crlf;
            d->file_time = ide_file_time(d->path);
        }
        free(text);
    }
}

/* A bare file name, tried in this order:
 *   1. the active document's folder
 *   2. the open project's folder
 *   3. the Folder panel's folder
 * else as it is. */
static void resolve_referenced_path(struct ide* ide, const char* name, char* out, size_t cap)
{
    char candidate[1400] = { 0 };
    struct doc* d = active_doc(ide);
    if (d)
    {
        char dir[1024] = { 0 };
        snprintf(dir, sizeof dir, "%s", d->path);
        parent_dir(dir);
        join_path(candidate, sizeof candidate, dir, name);
        if (ide_file_exists(candidate))
        {
            snprintf(out, cap, "%s", candidate);
            return;
        }
    }
    if (ide_project_is_open(&ide->project))
    {
        ide_project_absolute(&ide->project, name, candidate, sizeof candidate);
        if (ide_file_exists(candidate))
        {
            snprintf(out, cap, "%s", candidate);
            return;
        }
    }
    join_path(candidate, sizeof candidate, ide->folder.dir, name);
    if (ide_file_exists(candidate))
    {
        snprintf(out, cap, "%s", candidate);
        return;
    }
    snprintf(out, cap, "%s", name);
}

/* A line that is not a diagnostic ("dir" or "git status" output): the name
 * at its end, which may have spaces - each suffix after a blank, longest
 * first, against the Folder panel's folder. A file opens; a folder becomes
 * the Folder panel's. */
static void output_open_listed_path(struct ide* ide, const char* line)
{
    for (const char* p = line; *p; p++)
    {
        if (p != line && !(p[-1] == ' ' || p[-1] == '\t'))
            continue;
        if (*p == ' ' || *p == '\t')
            continue;
        char name[1024] = { 0 };
        snprintf(name, sizeof name, "%s", p);
        size_t n = strlen(name);
        while (n > 0 && (name[n - 1] == ' ' || name[n - 1] == '\t'))
            name[--n] = '\0';
        if (strcmp(name, ".") == 0)
            continue;
        char path[1400] = { 0 };
        if (name[0] == '/' || name[0] == '\\' || (isalpha((unsigned char)name[0]) && name[1] == ':'))
            snprintf(path, sizeof path, "%s", name);
        else
            join_path(path, sizeof path, ide->folder.dir, name);
        if (ide_is_dir(path) && strlen(path) < sizeof ide->folder.dir)
        {
            memcpy(ide->folder.dir, path, strlen(path) + 1);
            folder_refresh(ide);
            show_side_panel(ide, ide->folder.window);
            return;
        }
        if (ide_file_exists(path))
        {
            open_file(ide, path);
            return;
        }
    }
}

/* A double click on an Output line: a "file:line" there opens the file at
 * that line. */
static void output_goto_source(struct ide* ide, struct gui_node* panel)
{
    int row = 0, col = 0;
    gui_editor_get_caret(panel, &row, &col);
    const char* ls = gui_get_value(panel);
    for (int i = 1; i < row && *ls; ls++)
    {
        if (*ls == '\n')
            i++;
    }
    const char* le = ls;
    while (*le && *le != '\n')
        le++;
    char line[1024] = { 0 };
    int len = (int)(le - ls) < (int)sizeof line - 1 ? (int)(le - ls) : (int)sizeof line - 1;
    memcpy(line, ls, (size_t)len);
    line[len] = '\0';
    strip_ansi(line);

    int src_line = 0;
    char* sep = position_separator(line, &src_line);
    if (!sep)
    {
        output_open_listed_path(ide, line);
        return;
    }
    *sep = '\0';
    if (src_line < 1)
        return;
    nav_record_jump(ide);
    const char* name = line;
    while (*name == ' ' || *name == '\t')
        name++;

    struct doc* target = NULL;
    for (int i = 0; i < ide->doc_count && !target; i++)
    {
        if (paths_match(ide->docs[i].path, name))
            target = &ide->docs[i];
    }
    if (!target)
    {
        char path[1400] = { 0 };
        resolve_referenced_path(ide, name, path, sizeof path);
        open_file(ide, path);
        target = find_doc(ide, path);
        if (!target)
            return;   /* open_file said why */
    }
    gui_window_open(ide->app, target->window);
    gui_editor_goto_line_center(target->editor, src_line);
    gui_focus(ide->app, target->editor);
}

/* A file outside the open project is compiled with the playground
 * project's settings: its include directories are passed. */
static void playground_args(struct ide* ide)
{
    const struct compiler_settings* cs = &ide->global_options;
    const struct include_dirs* dirs = &settings_in_use(cs)->include_dirs;
    char flag[1200] = { 0 };
    for (int i = 0; i < dirs->count; i++)
    {
        snprintf(flag, sizeof flag, "-I%s", dirs->dirs[i]);
        ide_compile_arg(ide->job, flag);
    }
}

/* A project's compile: its output goes under the project folder, and its
 * include directories are passed. */
static void project_args(struct ide* ide)
{
    struct ide_project* p = &ide->project;
    char flag[1200] = { 0 };
    snprintf(flag, sizeof flag, "-output-root=%s", p->dir);
    ide_compile_arg(ide->job, flag);
    const struct include_dirs* dirs = &settings_in_use(&p->compile)->include_dirs;
    for (int i = 0; i < dirs->count; i++)
    {
        char abs[1024] = { 0 };
        ide_project_absolute(p, dirs->dirs[i], abs, sizeof abs);
        snprintf(flag, sizeof flag, "-I%s", abs);
        ide_compile_arg(ide->job, flag);
    }
}

/* 0 when the compile could not start. */
static int compile_start(struct ide* ide, const char* what)
{
    if (!ide_compile_start(ide->job))
    {
        ide->project_build = 0;
        status(ide, "Could not start the compile");
        return 0;
    }
    status(ide, what);
    gui_set_timer(ide->app, 50, EV_TICK);
    return 1;
}

/* Compile: the active file, with its project's settings when it is one of
 * the project's files, else the global ones. */
static int is_path_char(int c)
{
    return isalnum((unsigned char)c) || c == '_' || c == '-' || c == '.' || c == '/' || c == '\\';
}

/* Ctrl+click: the .c/.h/.md file name (or relative path) under the caret
 * opens - looked for as Find Results' names are. The old IDE's. */
/* A "[text](dest)" link around `cursor` on its line: dest into `link`. */
static int markdown_link_at(const char* text, int len, int cursor, char* link, int cap)
{
    int ls = cursor, le = cursor;
    while (ls > 0 && text[ls - 1] != '\n')
        ls--;
    while (le < len && text[le] != '\n')
        le++;
    for (const char* p = text + ls; p < text + le;)
    {
        const char* rb = *p == '[' ? memchr(p + 1, ']', (size_t)(text + le - (p + 1))) : NULL;
        if (!rb || rb + 1 >= text + le || rb[1] != '(')
        {
            p++;
            continue;
        }
        const char* lp = rb + 2;
        const char* rp = memchr(lp, ')', (size_t)(text + le - lp));
        if (!rp)
            break;
        if (cursor >= p - text && cursor <= rp - text)
        {
            snprintf(link, (size_t)cap, "%.*s", (int)(rp - lp), lp);
            return 1;
        }
        p = rp + 1;
    }
    return 0;
}

static int is_web_link(const char* link)
{
    return strncmp(link, "http://", 7) == 0 || strncmp(link, "https://", 8) == 0;
}

/* "Open this link in the browser?" - Yes (EV_OPEN_LINK) opens `url`. */
static void ask_open_url(struct ide* ide, const char* url)
{
    size_t n = strlen(url) + 1;
    char* copy = malloc(n);
    if (!copy)
        return;
    memcpy(copy, url, n);
    free(ide->pending_url);
    ide->pending_url = copy;
    struct ide_text msg = { 0 };
    ide_text_printf(&msg, "Open this link in the browser?\n\n%s", url);
    static const char* const labels[] = { "Yes", "No" };
    static const int ids[] = { EV_OPEN_LINK, 0 };
    gui_message_box(ide->app, "Open Link", msg.data ? msg.data : url, labels, ids, 2);
    free(msg.data);
}

/* The bare http(s) URL around `cursor` - up to the blanks and brackets
 * around it - into `link`. 0 if there is none. */
static int bare_url_at(const char* text, int len, int cursor, char* link, int cap)
{
    static const char stops[] = " \t\r\n()<>[]\"'`";
    int lo = cursor, hi = cursor;
    while (lo > 0 && !strchr(stops, text[lo - 1]))
        lo--;
    while (hi < len && !strchr(stops, text[hi]))
        hi++;
    while (hi > lo && strchr(".,;:", text[hi - 1]))
        hi--;
    snprintf(link, (size_t)cap, "%.*s", hi - lo, text + lo);
    return is_web_link(link);
}

/* Ctrl+click in the Help window: a [text](help:<slug>) link shows that
 * topic; a web link, Markdown or bare, opens in the browser. */
static void help_ctrlclick(struct ide* ide)
{
    struct gui_node* ed = ide->help.editor;
    const char* text = gui_get_value(ed);
    int len = (int)strlen(text);
    int lo = 0, hi = 0;
    gui_editor_get_selection(ed, &lo, &hi);
    char link[512] = { 0 };
    if (markdown_link_at(text, len, hi, link, sizeof link) && strncmp(link, "help:", 5) == 0)
    {
        enum help_id topic = help_find(link + 5, (int)strlen(link + 5));
        if (topic != HELP_NONE)
            help_follow(ide, topic);
        return;
    }
    if ((markdown_link_at(text, len, hi, link, sizeof link) && is_web_link(link)) ||
        bare_url_at(text, len, hi, link, sizeof link))
        ask_open_url(ide, link);
}

static void editor_ctrlclick(struct ide* ide)
{
    struct doc* d = active_doc(ide);
    if (!d)
        return;
    const char* text = gui_get_value(d->editor);
    int len = (int)strlen(text);
    int lo = 0, hi = 0;
    gui_editor_get_selection(d->editor, &lo, &hi);
    char link[512] = { 0 };
    if (ends_with(d->path, ".md") && markdown_link_at(text, len, hi, link, sizeof link))
    {
        /* a Markdown link: a web page in the browser, else the file beside this one */
        if (is_web_link(link))
        {
            ask_open_url(ide, link);
            return;
        }
        char* hash = strchr(link, '#');
        if (hash)
            *hash = '\0';   /* the file, not its anchor */
        if (!link[0])
            return;
        char dir[1024] = { 0 }, path[1600] = { 0 };
        snprintf(dir, sizeof dir, "%s", d->path);
        parent_dir(dir);
        join_path(path, sizeof path, dir, link);
        if (!ide_file_exists(path))
        {
            char msg[700] = { 0 };
            snprintf(msg, sizeof msg, "%s not found.", link);
            static const char* const ok[] = { "OK" };
            static const int ok_id[] = { 0 };
            gui_message_box(ide->app, "Open Link", msg, ok, ok_id, 1);
            return;
        }
        nav_record_jump(ide);
        open_file(ide, path);
        return;
    }
    lo = hi;
    if (lo > 0 && !(lo < len && is_path_char(text[lo])) && is_path_char(text[lo - 1]))
    {
        lo--;
        hi--;
    }
    while (lo > 0 && is_path_char(text[lo - 1]))
        lo--;
    while (hi < len && is_path_char(text[hi]))
        hi++;
    char name[512] = { 0 };
    if (hi <= lo || hi - lo >= (int)sizeof name)
        return;
    snprintf(name, sizeof name, "%.*s", hi - lo, text + lo);
    const char* dot = strrchr(name, '.');
    if (!dot || dot == name || (_stricmp(dot, ".c") != 0 && _stricmp(dot, ".h") != 0 && _stricmp(dot, ".md") != 0))
        return;
    char path[1400] = { 0 };
    resolve_referenced_path(ide, name, path, sizeof path);
    nav_record_jump(ide);
    open_file(ide, path);
}

/* Edit > Format (Ctrl+Shift+F): cake_format with the Playground project's style;
 * a selection formats just its lines. The caret stays. The old IDE's. */
static void format_doc(struct ide* ide)
{
    struct doc* d = active_doc(ide);
    if (!d)
        return;
    const char* text = gui_get_value(d->editor);
    char options[96] = "-format";
    int style = settings_in_use(&ide->global_options)->style;
    if (style > 0 && style < COUNT(style_slugs))
        snprintf(options + strlen(options), sizeof options - strlen(options), " -style=%s", style_slugs[style]);
    int lo = 0, hi = 0;
    gui_editor_get_selection(d->editor, &lo, &hi);
    if (hi > lo)
    {
        int first = 1, last;
        for (int i = 0; i < lo; i++)
            first += text[i] == '\n';
        last = first;
        for (int i = lo; i < hi; i++)
            last += text[i] == '\n';
        snprintf(options + strlen(options), sizeof options - strlen(options), " -format-lines=%d:%d", first, last);
    }
    size_t len = strlen(text);
    char* content = malloc(len + 1);
    if (!content)
        return;
    memcpy(content, text, len + 1);
    struct report report = { 0 };
    char* out = (char*)cake_format(options, d->path, content, &report);
    free(content);
    if (!out)
    {
        status(ide, "Format failed");
        return;
    }
    int caret = 0, unused = 0;
    gui_editor_get_selection(d->editor, &caret, &unused);
    gui_set_value(d->editor, out);
    int n = (int)strlen(out);
    gui_editor_set_selection(d->editor, caret < n ? caret : n, caret < n ? caret : n);
    gui_editor_set_dirty(d->editor, 1);
    free(out);
}

/* 0 when there is nothing to compile or the compile could not start. */
static int compile_active(struct ide* ide)
{
    if (ide_compile_running(ide->job))
        return 0;   /* one at a time; the statusbar already says so */
    struct doc* d = active_doc(ide);
    if (!d)
    {
        status(ide, "No file to compile");
        return 0;
    }
    if (gui_editor_get_dirty(d->editor))
        save_doc(ide, d);
    int in_project = ide_project_contains(&ide->project, d->path);
    compile_args(ide, in_project ? &ide->project.compile : &ide->global_options);
    if (in_project)
        project_args(ide);
    else
        playground_args(ide);
    ide_compile_arg(ide->job, d->path);
    gui_set_value(ide->output.editor, "");
    bottom_panel_show(ide, ide->output.window, ide->fr.window);
    gui_window_open(ide->app, d->window);
    gui_focus(ide->app, d->editor);
    return compile_start(ide, "Compiling...");
}

/* The worker thread's view of a project Build: each header a source
 * includes, and each source's error count. */
static void build_on_include(void* ctx, const char* source, const char* header)
{
    struct ide_build_state* pending = ctx;
    int s = ide_build_state_find(pending, source);
    if (s < 0)
        return;
    int h = ide_build_state_add(pending, header, ide_file_time(header));
    if (h >= 0)
        ide_build_state_add_dep(pending, s, h);
}

static void build_on_done(void* ctx, const char* source, int errors)
{
    struct ide_build_state* pending = ctx;
    int s = ide_build_state_find(pending, source);
    if (s >= 0)
        pending->items[s].ok = errors == 0;
}

static int is_playground(struct ide* ide, const struct doc* d);
static void debug_launch(struct ide* ide);
static void post_build(struct ide* ide);

/* Build (F7): with a project open, the project's .c files changed since
 * the last successful Build - the file or a header it included - unless
 * the settings changed; Rebuild compiles them all. Without a project, or
 * with the Playground active, Build is Compile. 0 on an error: no .c
 * files, or the compile could not start; all files up to date is not one. */
static int build(struct ide* ide, int rebuild)
{
    struct ide_project* p = &ide->project;
    struct doc* active = active_doc(ide);
    if (!ide_project_is_open(p) || (active && is_playground(ide, active)))
        return compile_active(ide);
    if (ide_compile_running(ide->job))
        return 0;
    if (rebuild)
        ide_build_state_clear(&p->built);

    /* The compiler reads the disk: unsaved project files are saved first. */
    for (int i = 0; i < ide->doc_count; i++)
    {
        if (gui_editor_get_dirty(ide->docs[i].editor) && ide_project_contains(p, ide->docs[i].path))
            save_doc(ide, &ide->docs[i]);
    }

    compile_args(ide, &p->compile);
    project_args(ide);
    struct ide_build_state* built = &p->built;
    struct ide_build_state* pending = &ide->pending;
    ide_build_state_clear(pending);
    char settings[16384] = { 0 };
    ide_compile_command(ide->job, settings, sizeof settings);
    size_t n = strlen(settings);
    pending->settings = malloc(n + 1);
    if (pending->settings)
        memcpy(pending->settings, settings, n + 1);
    int rebuild_all = !pending->settings || !built->settings || strcmp(pending->settings, built->settings) != 0;
    for (int i = 0; i < built->count; i++)
        built->items[i].changed = ide_file_time(built->items[i].path) != built->items[i].time;

    struct ide_strings reasons = { 0 };
    int c_count = 0, file_count = 0;
    for (int i = 0; i < p->files.count; i++)
    {
        char abs[1024] = { 0 };
        ide_project_absolute(p, p->files.items[i], abs, sizeof abs);
        if (!ends_with(abs, ".c"))
            continue;
        c_count++;
        int index = ide_build_state_find(built, abs);
        int dirty = rebuild_all || index < 0 || !built->items[index].compiled || built->items[index].changed;
        const char* header = NULL;
        for (int k = 0; !dirty && k < built->items[index].deps.count; k++)
        {
            dirty = built->items[built->items[index].deps.items[k]].changed;
            if (dirty)
                header = built->items[built->items[index].deps.items[k]].path;
        }
        /* why each file is compiled or skipped - nothing on the first build */
        if (built->settings)
        {
            char line[2200] = { 0 };
            if (!dirty)
                snprintf(line, sizeof line, "%s: skipped, up to date\n", abs);
            else if (rebuild_all)
                snprintf(line, sizeof line, "%s: settings changed\n", abs);
            else if (index < 0)
                snprintf(line, sizeof line, "%s: new file\n", abs);
            else if (!built->items[index].compiled)
                snprintf(line, sizeof line, "%s: had errors\n", abs);
            else if (built->items[index].changed)
                snprintf(line, sizeof line, "%s: changed\n", abs);
            else
                snprintf(line, sizeof line, "%s: header changed (%s)\n", abs, header ? header : "");
            ide_strings_add(&reasons, line);
        }
        if (!dirty)
            continue;
        int e = ide_build_state_add(pending, abs, ide_file_time(abs));
        if (e >= 0)
            pending->items[e].compiled = 1;
        ide_compile_arg(ide->job, abs);
        file_count++;
    }

    bottom_panel_show(ide, ide->output.window, ide->fr.window);
    if (c_count == 0 || file_count == 0)
    {
        ide_strings_destroy(&reasons);
        ide_build_state_clear(pending);
        if (file_count == 0 && c_count > 0)
            ide_strings_clear(&p->compiled);
        gui_set_value(ide->output.editor, c_count == 0 ? "The open project has no .c files to build.\n"
                                                        : "Build: all files are up to date.\n");
        return c_count > 0;
    }
    for (int i = 0; i < reasons.count; i++)
        ide_compile_note(ide->job, reasons.items[i]);
    ide_strings_destroy(&reasons);
    struct ide_build_listener listener = { build_on_include, build_on_done, pending };
    ide_compile_set_listener(ide->job, &listener);
    gui_set_value(ide->output.editor, "");
    ide->project_build = 1;
    return compile_start(ide, "Building...");
}

/* The Build Events of the target in use: the project's, or the global ones
 * for a Compile of a file outside it, or with no project. */
static const struct target_settings* chain_settings(struct ide* ide)
{
    struct doc* d = active_doc(ide);
    /* as build() and compile_active() choose: F7 with the playground active is its Compile */
    int project = ide->chain.compile ? d && file_uses_project(ide, d->path)
                                     : ide_project_is_open(&ide->project) && !(d && is_playground(ide, d));
    const struct compiler_settings* cs = project ? &ide->project.compile : &ide->global_options;
    return settings_in_use(cs);
}

/* F7 (or Rebuild), Ctrl+F7 with `compile` (the active file only), and F5
 * with `debug`: the Pre-Build Event, then Cake - see struct build_chain. */
static void build_then(struct ide* ide, int rebuild, int compile, int debug)
{
    if (ide_compile_running(ide->job) || run_busy(ide))
        return;
    ide->chain.rebuild = rebuild;
    ide->chain.compile = compile;
    ide->chain.debug = debug;
    const struct target_settings* s = chain_settings(ide);
    if (!s->pre_build[0][0])
    {
        chain_build(ide);
        return;
    }
    ide->chain.stage = STAGE_PRE_BUILD;
    run_command(ide, "Pre-Build", s->pre_build[0], s->pre_build[1], s->pre_build[2]);
}

/* The Build step: Cake. Any error stops the pipeline; nothing to compile
 * (up to date) goes on to the Post-Build Event at once. */
static void chain_build(struct ide* ide)
{
    ide->chain.stage = STAGE_BUILD;
    int ok = ide->chain.compile ? compile_active(ide) : build(ide, ide->chain.rebuild);
    if (!ide_compile_running(ide->job))
    {
        ide->chain.stage = STAGE_NONE;
        if (ok)
            post_build(ide);
    }
}

/* The Post-Build Event, if any; without one, F5's debugger at once. */
static void post_build(struct ide* ide)
{
    const struct target_settings* s = chain_settings(ide);
    if (!s->post_build[0][0])
    {
        if (ide->chain.debug)
            debug_launch(ide);
        return;
    }
    if (run_busy(ide))
        return;
    ide->chain.stage = STAGE_POST_BUILD;
    run_command(ide, "Post-Build", s->post_build[0], s->post_build[1], s->post_build[2]);
}

/* --- Find Definition / Declaration / Usages: the old IDE's - a compile
 * with -find-definition (or -find-declaration, -find-usages) line col,
 * streamed into Find Results; when the compiler finds nothing, a text
 * search for the word. --- */

static const struct
{
    const char* title;
    const char* option;
    const char* status;
} find_kinds[] = {
    [FIND_DEFINITION] = { "Find Definition", "-find-definition", "Finding definition..." },
    [FIND_DECLARATION] = { "Find Declaration", "-find-declaration", "Finding declaration..." },
    [FIND_USAGES] = { "Find Usages", "-find-usages", "Finding usages..." },
    [FIND_UNUSED] = { "Report Unused", "-unused-extern-report", "Reporting unused..." },
};

/* Build > Report Unused: the project's .c files compiled with
 * -unused-extern-report - the compiler then reports only the unused
 * functions - into Find Results. As the old IDE. */
static void find_push_files(struct ide* ide, const char* file);

/* Search > Rename (F2): the identifier under the caret - its file, line
 * and column kept - and the dialog with its name. The old IDE's. */
static void rename_open(struct ide* ide)
{
    if (ide_compile_running(ide->job))
        return;
    struct doc* d = active_doc(ide);
    if (!d)
        return;
    static const char* const ok[] = { "OK" };
    static const int ok_id[] = { 0 };
    char word[200] = { 0 };
    word_at_caret(d->editor, word, sizeof word);
    if (!word[0])
    {
        gui_message_box(ide->app, "Rename", "No word under the caret.", ok, ok_id, 1);
        return;
    }
    const char* text = gui_get_value(d->editor);
    int caret = 0, unused = 0;
    gui_editor_get_selection(d->editor, &caret, &unused);
    struct rename_dialog* r = &ide->rename;
    r->line = 1;
    r->col = 1;
    for (int i = 0; i < caret && text[i]; i++)
    {
        if (text[i] == '\n')
        {
            r->line++;
            r->col = 1;
        }
        else
        {
            r->col++;
        }
    }
    snprintf(r->file, sizeof r->file, "%s", d->path);
    gui_set_value(r->input, word);
    show_dialog(ide, r->window, 50, 8, r->input);
}

static int is_identifier(const char* s)
{
    if (!(isalpha((unsigned char)s[0]) || s[0] == '_'))
        return 0;
    for (const char* p = s + 1; *p; p++)
    {
        if (!(isalnum((unsigned char)*p) || *p == '_'))
            return 0;
    }
    return 1;
}

/* Its OK: a compile with -rename line col new_name over the files Find
 * Definition looks in; the compiler rewrites them on disk. */
static void rename_run(struct ide* ide)
{
    struct rename_dialog* r = &ide->rename;
    char new_name[200] = { 0 };
    snprintf(new_name, sizeof new_name, "%s", gui_get_value(r->input));
    gui_window_close(ide->app, r->window);
    if (!is_identifier(new_name))
    {
        static const char* const ok[] = { "OK" };
        static const int ok_id[] = { 0 };
        gui_message_box(ide->app, "Rename", "The new name is not an identifier.", ok, ok_id, 1);
        return;
    }
    if (ide_compile_running(ide->job))
        return;
    struct doc* d = find_doc(ide, r->file);
    if (d && gui_editor_get_dirty(d->editor))
        save_doc(ide, d);
    int in_project = ide_project_contains(&ide->project, r->file);
    compile_args(ide, in_project ? &ide->project.compile : &ide->global_options);
    if (in_project)
        project_args(ide);
    else
        playground_args(ide);
    char number[16] = { 0 };
    ide_compile_arg(ide->job, "-rename");
    snprintf(number, sizeof number, "%d", r->line);
    ide_compile_arg(ide->job, number);
    snprintf(number, sizeof number, "%d", r->col);
    ide_compile_arg(ide->job, number);
    ide_compile_arg(ide->job, new_name);
    find_push_files(ide, r->file);
    gui_set_value(ide->output.editor, "");
    bottom_panel_show(ide, ide->output.window, ide->fr.window);
    r->running = 1;
    if (!ide_compile_start(ide->job))
    {
        r->running = 0;
        status(ide, "");
        return;
    }
    status(ide, "Renaming...");
    gui_set_timer(ide->app, 50, EV_TICK);
}

static void report_unused(struct ide* ide)
{
    struct ide_project* p = &ide->project;
    if (ide_compile_running(ide->job) || !ide_project_is_open(p))
        return;
    for (int i = 0; i < ide->doc_count; i++)
    {
        if (gui_editor_get_dirty(ide->docs[i].editor) && ide_project_contains(p, ide->docs[i].path))
            save_doc(ide, &ide->docs[i]);
    }
    compile_args(ide, &p->compile);
    project_args(ide);
    ide_compile_arg(ide->job, find_kinds[FIND_UNUSED].option);
    int files = 0;
    for (int i = 0; i < p->files.count; i++)
    {
        if (!ends_with(p->files.items[i], ".c"))
            continue;
        char abs[1024] = { 0 };
        ide_project_absolute(p, p->files.items[i], abs, sizeof abs);
        ide_compile_arg(ide->job, abs);
        files++;
    }
    findresults_begin(ide);
    bottom_panel_show(ide, ide->fr.window, ide->output.window);
    if (files == 0)
    {
        findresults_set(ide, "The open project has no .c files.\n");
        return;
    }
    findresults_set(ide, "");
    ide->find_kind = FIND_UNUSED;
    if (!ide_compile_start(ide->job))
    {
        ide->find_kind = FIND_NONE;
        findresults_set(ide, "Could not start the report (pipe/thread creation failed).\n");
        return;
    }
    status(ide, find_kinds[FIND_UNUSED].status);
    gui_set_timer(ide->app, 50, EV_TICK);
}

/* `word`, whole and with case, in the project's .c/.h files (the open
 * documents' text, else the disk's), or else in the active file's folder. */
static void find_definition_text_search(struct ide* ide, const char* word)
{
    struct ide_search s = { "", 1, 1 };
    snprintf(s.pattern, sizeof s.pattern, "%s", word);
    struct ide_text out = { 0 };
    int total = 0, files = 0;
    struct ide_project* p = &ide->project;
    findresults_begin(ide);
    if (ide_project_is_open(p))
    {
        for (int i = 0; i < p->files.count; i++)
        {
            const char* entry = p->files.items[i];
            if (!ends_with(entry, ".c") && !ends_with(entry, ".h"))
                continue;
            char abs[1024] = { 0 };
            ide_project_absolute(p, entry, abs, sizeof abs);
            struct doc* d = find_doc(ide, abs);
            int crlf = 0;
            char* loaded = d ? NULL : ide_read_file(abs, &crlf);
            const char* content = d ? gui_get_value(d->editor) : loaded;
            if (!content)
                continue;
            files++;
            total += ide_search_text(entry, content, &s, &out);
            free(loaded);
        }
        if (total == 0)
            ide_text_printf(&out, "\"%s\" not found in %d file(s) in project \"%s\".\n", word, files, p->name);
        else
            ide_text_printf(&out, "\n%d occurrence(s) of \"%s\" in %d file(s) in project \"%s\".\n",
                             total, word, files, p->name);
    }
    else
    {
        struct doc* d = active_doc(ide);
        if (!d)
        {
            ide_text_printf(&out, "No file is open, so there's no directory to search.\n");
        }
        else
        {
            char dir[1024] = { 0 };
            snprintf(dir, sizeof dir, "%s", d->path);
            parent_dir(dir);
            struct ide_dir_entry* entries = malloc(sizeof *entries * MAX_FOLDER_ENTRIES);
            int n = entries ? ide_list_dir(dir, entries, MAX_FOLDER_ENTRIES) : -1;
            if (n < 0)
            {
                ide_text_printf(&out, "Could not open directory: %s\n", dir);
            }
            else
            {
                for (int i = 0; i < n; i++)
                {
                    if (entries[i].is_dir || (!ends_with(entries[i].name, ".c") && !ends_with(entries[i].name, ".h")))
                        continue;
                    char path[1400] = { 0 };
                    join_path(path, sizeof path, dir, entries[i].name);
                    int crlf = 0;
                    char* content = ide_read_file(path, &crlf);
                    if (!content)
                        continue;
                    files++;
                    total += ide_search_text(entries[i].name, content, &s, &out);
                    free(content);
                }
                if (total == 0)
                    ide_text_printf(&out, "\"%s\" not found in %d file(s) in %s.\n", word, files, dir);
                else
                    ide_text_printf(&out, "\n%d occurrence(s) of \"%s\" in %d file(s) in %s.\n",
                                     total, word, files, dir);
            }
            free(entries);
        }
    }
    findresults_set(ide, out.data ? out.data : "");
    free(out.data);
    bottom_panel_show(ide, ide->fr.window, ide->output.window);
}

/* The files to look in, by the chance of finding the symbol: the file (the
 * caret is in it), for a header its .c, then the project's other .c files.
 * A file outside the project goes alone. Open project files are saved -
 * the compiler reads the disk. */
static void find_push_files(struct ide* ide, const char* file)
{
    struct ide_project* p = &ide->project;
    for (int i = 0; i < ide->doc_count; i++)
    {
        if (gui_editor_get_dirty(ide->docs[i].editor) && ide_project_contains(p, ide->docs[i].path))
            save_doc(ide, &ide->docs[i]);
    }
    ide_compile_arg(ide->job, file);
    if (!ide_project_contains(p, file))
        return;
    char counterpart[1024] = "";
    if (ends_with(file, ".h"))
    {
        snprintf(counterpart, sizeof counterpart, "%s", file);
        counterpart[strlen(counterpart) - 1] = 'c';
        if (ide_file_exists(counterpart))
            ide_compile_arg(ide->job, counterpart);
        else
            counterpart[0] = '\0';
    }
    for (int i = 0; i < p->files.count; i++)
    {
        if (!ends_with(p->files.items[i], ".c"))
            continue;
        char abs[1024] = { 0 };
        ide_project_absolute(p, p->files.items[i], abs, sizeof abs);
        if (ide_path_equal(abs, file) || ide_path_equal(abs, counterpart))
            continue;
        ide_compile_arg(ide->job, abs);
    }
}

static void find_definition(struct ide* ide, enum find_kind kind)
{
    if (ide_compile_running(ide->job))
        return;
    struct doc* d = active_doc(ide);
    if (!d)
        return;
    char word[sizeof ide->find_word] = { 0 };
    word_at_caret(d->editor, word, sizeof word);
    if (!word[0])
    {
        static const char* const labels[] = { "OK" };
        static const int ids[] = { 0 };
        gui_message_box(ide->app, find_kinds[kind].title, "No word under the caret.", labels, ids, 1);
        return;
    }
    /* 1-based and in bytes, the line:col the compiler gives its tokens */
    const char* text = gui_get_value(d->editor);
    int caret = 0, unused = 0;
    gui_editor_get_selection(d->editor, &caret, &unused);
    int line = 1, col = 1;
    for (int i = 0; i < caret && text[i]; i++)
    {
        if (text[i] == '\n')
        {
            line++;
            col = 1;
        }
        else
        {
            col++;
        }
    }
    if (gui_editor_get_dirty(d->editor))
        save_doc(ide, d);

    int in_project = ide_project_contains(&ide->project, d->path);
    compile_args(ide, in_project ? &ide->project.compile : &ide->global_options);
    if (in_project)
        project_args(ide);
    else
        playground_args(ide);
    char number[16] = { 0 };
    ide_compile_arg(ide->job, find_kinds[kind].option);
    snprintf(number, sizeof number, "%d", line);
    ide_compile_arg(ide->job, number);
    snprintf(number, sizeof number, "%d", col);
    ide_compile_arg(ide->job, number);
    find_push_files(ide, d->path);

    ide->find_kind = kind;
    snprintf(ide->find_word, sizeof ide->find_word, "%s", word);
    findresults_begin(ide);
    findresults_set(ide, "");
    bottom_panel_show(ide, ide->fr.window, ide->output.window);
    if (!ide_compile_start(ide->job))
    {
        ide->find_kind = FIND_NONE;
        find_definition_text_search(ide, word);
        return;
    }
    status(ide, find_kinds[kind].status);
    gui_set_timer(ide->app, 50, EV_TICK);
}

/* The lookup ended: its result, with the time at the end of its first
 * line, or the text search when the compiler found nothing. */
static void find_finish(struct ide* ide)
{
    enum find_kind kind = ide->find_kind;
    ide->find_kind = FIND_NONE;
    status(ide, "");
    const char* result = ide_compile_output(ide->job);
    if (kind == FIND_UNUSED)
    {
        findresults_set(ide, result[0] ? result : "No unused functions found.\n");
        return;
    }
    if (!result[0])
    {
        find_definition_text_search(ide, ide->find_word);
        return;
    }
    int errors = 0, warnings = 0;
    double seconds = 0;
    ide_compile_counts(ide->job, &errors, &warnings, &seconds);
    char elapsed[32] = { 0 };
    snprintf(elapsed, sizeof elapsed, "  (%.1fs)", seconds);
    size_t first = strcspn(result, "\r\n");
    size_t total = strlen(result);
    char* shown = malloc(total + strlen(elapsed) + 1);
    if (!shown)
        return;
    memcpy(shown, result, first);
    strcpy(shown + first, elapsed);
    strcat(shown, result + first);
    char* o = shown;
    for (const char* q = shown; *q; q++)
    {
        if (*q != '\r')
            *o++ = *q;
    }
    *o = '\0';
    findresults_set(ide, shown);
    free(shown);
}

/* --- Complete Word (Ctrl+Space): the compiler, with -complete line col,
 * prints what can be written at the caret, name<TAB>kind<TAB>type per line;
 * those starting with the prefix left of the caret are offered - one is
 * inserted, more open a list at the caret. The old IDE's. --- */

static int complete_cmp(const void* a, const void* b)
{
    return strcmp(a, b);
}

/* The prefix replaced with `name`, caret after it. */
static void complete_insert(struct ide* ide, const char* name)
{
    struct doc* d = ide->complete.doc;
    if (!d)
        return;
    const char* text = gui_get_value(d->editor);
    int start = ide->complete.prefix_start;
    char replacement[300] = { 0 };
    snprintf(replacement, sizeof replacement, "%s", name);
    if (ide->complete.op_fix[0])
    {
        /* the wrong operator ('.' for '->' or the reverse) is replaced too, the spaces after it kept */
        int op_end = start;
        while (op_end > 0 && (text[op_end - 1] == ' ' || text[op_end - 1] == '\t'))
            op_end--;
        int op_len = ide->complete.op_fix[0] == '.' ? 2 : 1;   /* "->" becomes ".", "." becomes "->" */
        if (op_end >= op_len && strncmp(text + op_end - op_len, op_len == 2 ? "->" : ".", (size_t)op_len) == 0)
        {
            snprintf(replacement, sizeof replacement, "%s%.*s%s", ide->complete.op_fix, start - op_end, text + op_end, name);
            start = op_end - op_len;
        }
    }
    gui_editor_replace(d->editor, start, ide->complete.cursor, replacement);
    int end = start + (int)strlen(replacement);
    gui_editor_set_selection(d->editor, end, end);
    gui_focus(ide->app, d->editor);
}

static void complete_show(struct ide* ide, const char* output)
{
    struct doc* d = ide->complete.doc;
    int caret = 0, unused = 0;
    if (!d || d != active_doc(ide))
        return;
    gui_editor_get_selection(d->editor, &caret, &unused);
    if (caret != ide->complete.cursor)
        return;   /* moved on while the compiler worked */
    size_t prefix_len = strlen(ide->complete.prefix);
    int count = 0, cap = 64;
    char (*list)[128] = malloc(sizeof *list * (size_t)cap);
    if (!list)
        return;
    ide->complete.op_fix[0] = '\0';
    for (const char* line = output; *line;)
    {
        const char* end = strchr(line, '\n');
        size_t len = end ? (size_t)(end - line) : strlen(line);
        const char* tab1 = memchr(line, '\t', len);
        const char* tab2 = tab1 ? memchr(tab1 + 1, '\t', len - (size_t)(tab1 + 1 - line)) : NULL;
        if (tab2 && tab2 - tab1 - 1 == 8 && strncmp(tab1 + 1, "operator", 8) == 0 &&
            tab1 - line < (int)sizeof ide->complete.op_fix)
        {
            snprintf(ide->complete.op_fix, sizeof ide->complete.op_fix, "%.*s", (int)(tab1 - line), line);
        }
        else if (tab2 && (size_t)(tab1 - line) < sizeof *list && (size_t)(tab1 - line) >= prefix_len &&
                 strncmp(line, ide->complete.prefix, prefix_len) == 0)
        {
            if (count == cap)
            {
                cap *= 2;
                char (*bigger)[128] = realloc(list, sizeof *list * (size_t)cap);
                if (!bigger)
                    break;
                list = bigger;
            }
            snprintf(list[count++], sizeof *list, "%.*s", (int)(tab1 - line), line);
        }
        line = end ? end + 1 : line + len;
    }
    qsort(list, (size_t)count, sizeof *list, complete_cmp);
    int unique = 0;
    for (int i = 0; i < count; i++)
    {
        if (unique == 0 || strcmp(list[unique - 1], list[i]) != 0)
            memmove(list[unique++], list[i], sizeof *list);
    }
    count = unique;
    if (count == 0)
    {
        status(ide, "No completions");
    }
    else if (count == 1 && prefix_len > 0)
    {
        complete_insert(ide, list[0]);
        status(ide, "");
    }
    else
    {
        gui_clear_children(ide->complete.popup);
        ide->complete.count = count < 30 ? count : 30;
        for (int i = 0; i < ide->complete.count; i++)
        {
            snprintf(ide->complete.names[i], sizeof ide->complete.names[i], "%s", list[i]);
            struct gui_node* it = create(ide, GUI_ITEM, list[i]);
            gui_set_id(it, EV_COMPLETE_ITEM + i);
            gui_append(ide->complete.popup, it);
        }
        int x = 0, y = 0, cw = 0;
        gui_editor_caret_point(ide->app, d->editor, &x, &y);
        gui_cell_size(ide->app, &cw, NULL);
        gui_popup_menu_at(ide->app, ide->complete.popup, d->editor, x - (int)prefix_len * cw, y);
        if (count > 30)
        {
            char msg[64] = { 0 };
            snprintf(msg, sizeof msg, "%d more - type more of the name", count - 30);
            status(ide, msg);
        }
        else
        {
            status(ide, "");
        }
    }
    free(list);
}

static void complete_word(struct ide* ide)
{
    if (ide_compile_running(ide->job))
        return;
    struct doc* d = active_doc(ide);
    if (!d || !(ends_with(d->path, ".c") || ends_with(d->path, ".h")))
        return;
    const char* text = gui_get_value(d->editor);
    int cursor = 0, unused = 0;
    gui_editor_get_selection(d->editor, &cursor, &unused);
    int start = cursor;
    while (start > 0 && (isalnum((unsigned char)text[start - 1]) || text[start - 1] == '_'))
        start--;
    if (cursor - start >= (int)sizeof ide->complete.prefix)
        return;
    ide->complete.doc = d;
    ide->complete.cursor = cursor;
    ide->complete.prefix_start = start;
    snprintf(ide->complete.prefix, sizeof ide->complete.prefix, "%.*s", cursor - start, text + start);
    int line = 1, col = 1;
    for (int i = 0; i < cursor && text[i]; i++)
    {
        if (text[i] == '\n')
            line++, col = 1;
        else
            col++;
    }
    if (gui_editor_get_dirty(d->editor))
        save_doc(ide, d);   /* the compiler reads the file */
    int in_project = ide_project_contains(&ide->project, d->path);
    compile_args(ide, in_project ? &ide->project.compile : &ide->global_options);
    if (in_project)
        project_args(ide);
    else
        playground_args(ide);
    char number[16] = { 0 };
    ide_compile_arg(ide->job, "-complete");
    snprintf(number, sizeof number, "%d", line);
    ide_compile_arg(ide->job, number);
    snprintf(number, sizeof number, "%d", col);
    ide_compile_arg(ide->job, number);
    ide_compile_arg(ide->job, d->path);
    if (!ide_compile_start(ide->job))
    {
        status(ide, "No completions");
        return;
    }
    ide->complete.running = 1;
    status(ide, "Completing...");
    gui_set_timer(ide->app, 50, EV_TICK);
}

static void compile_poll(struct ide* ide)
{
    int new_output = 0;
    int done = ide_compile_poll(ide->job, &new_output);
    if (new_output || done)
        gui_repaint(ide->app);
    if (new_output && !ide->complete.running)
        compile_show_output(ide);
    if (!done)
        return;
    gui_set_timer(ide->app, 2000, EV_TICK);   /* back to watching the files */
    if (ide->complete.running)
    {
        ide->complete.running = 0;
        complete_show(ide, ide_compile_output(ide->job));
        return;
    }
    if (ide->find_kind != FIND_NONE)
    {
        find_finish(ide);
        return;
    }
    if (ide->rename.running)
    {
        /* The compiler changed the files: the open ones are reloaded now. */
        ide->rename.running = 0;
        int errors = 0, warnings = 0;
        double seconds = 0;
        ide_compile_counts(ide->job, &errors, &warnings, &seconds);
        char elapsed[64] = { 0 };
        snprintf(elapsed, sizeof elapsed, "\nRename time: %.1f s\n", seconds);
        ide_compile_note(ide->job, elapsed);
        compile_show_output(ide);
        refresh_open_docs(ide);
        status(ide, "");
        return;
    }
    if (ide->project_build)
    {
        /* Files compiled without errors are recorded; the others are compiled again next time. */
        ide->project_build = 0;
        struct ide_project* p = &ide->project;
        ide_strings_clear(&p->compiled);
        for (int i = 0; i < ide->pending.count; i++)
        {
            if (ide->pending.items[i].compiled && ide->pending.items[i].ok)
                ide_strings_add(&p->compiled, ide->pending.items[i].path);
        }
        ide_build_state_commit(&p->built, &ide->pending);
    }
    int errors = 0, warnings = 0;
    double seconds = 0;
    ide_compile_counts(ide->job, &errors, &warnings, &seconds);
    char msg[200] = { 0 };
    snprintf(msg, sizeof msg, "%d error(s), %d warning(s) - %.2f s", errors, warnings, seconds);
    status(ide, msg);

    char* text = malloc(strlen(ide_compile_output(ide->job)) + 1);
    if (text)
    {
        strcpy(text, ide_compile_output(ide->job));
        apply_diagnostics(ide, text, 1);
        free(text);
    }
    refresh_open_docs(ide);
    if (ide->chain.stage == STAGE_BUILD)
    {
        ide->chain.stage = STAGE_NONE;
        if (errors == 0)
            post_build(ide);
    }
}

/* --- Playground: a scratch C file, always the same one, independent of any
 * project - the old IDE's. It starts as a Hello World. --- */

static int playground_path(char* out, size_t cap)
{
    char dir[1024] = { 0 };
    if (!ide_config_dir(dir, sizeof dir))
        return 0;
    join_path(out, cap, dir, "playground.c");
    return 1;
}

static struct doc* find_doc(struct ide* ide, const char* name)
{
    char path[1024] = { 0 };
    ide_full_path(name, path, sizeof path);
    for (int i = 0; i < ide->doc_count; i++)
    {
        if (_stricmp(ide->docs[i].path, path) == 0)
            return &ide->docs[i];
    }
    return NULL;
}

static int is_playground(struct ide* ide, const struct doc* d)
{
    (void)ide;
    char path[1400] = { 0 };
    return playground_path(path, sizeof path) && ide_path_equal(path, d->path);
}

/* View > Playground: the open one comes to the front, else it is opened
 * (and created when missing). */
static void open_playground(struct ide* ide)
{
    char path[1400] = { 0 };
    if (!playground_path(path, sizeof path))
    {
        status(ide, "No folder for the Playground (APPDATA is not set)");
        return;
    }
    if (!ide_file_exists(path))
    {
        static const char hello[] =
            "#include <stdio.h>\n"
            "int main(void)\n"
            "{\n"
            "  printf(\"Hello, world!\\n\");\n"
            "  return 0;\n"
            "}\n";
        ide_write_file(path, hello, 1);
    }
    open_file(ide, path);
    struct doc* d = find_doc(ide, path);
    if (d)
        gui_set_label(d->window, "Playground");
}

/* --- Show Generated Code: the C89 file Cake wrote for the document, in
 * <its folder>/<platform>/<name>, opened like any other file --- */

/* A file outside the open project is compiled on its own; no file at all
 * means the open project - the old IDE's file_uses_project. */
static int file_uses_project(struct ide* ide, const char* path)
{
    return ide_project_is_open(&ide->project) && (!path[0] || ide_project_contains(&ide->project, path));
}

/* The platform `path` is compiled for: the target of its project's configuration, or the global one. */
static const char* platform_for(struct ide* ide, const char* path)
{
    const struct target_settings* s = settings_in_use(file_uses_project(ide, path) ? &ide->project.compile : &ide->global_options);
    if (s->cake_target[0])
        return s->cake_target;
    static struct platform host;
    platform_default(&host);
    return host.name;
}

static const char* platform_name(struct ide* ide)
{
    struct doc* d = active_doc(ide);
    return platform_for(ide, d ? d->path : "");
}

static void show_generated_code(struct ide* ide)
{
    /* The editor the popup opened over, else the active document. */
    struct doc* src = NULL;
    struct gui_node* target = gui_context_target(ide->app);
    for (int i = 0; i < ide->doc_count && target; i++)
    {
        if (ide->docs[i].editor == target)
            src = &ide->docs[i];
    }
    if (!src)
        src = active_doc(ide);
    if (!src)
        return;

    /* where the compile wrote it: a project file under <project>/<platform>/<its
     * path in the project>, any other file under <its folder>/<platform>/ -
     * the platform of the target its settings use */
    const char* platform = platform_for(ide, src->path);
    char dir[1024] = { 0 }, rel[1024] = { 0 };
    snprintf(dir, sizeof dir, "%s", src->path);
    parent_dir(dir);
    snprintf(rel, sizeof rel, "%s", file_name(src->path));
    if (file_uses_project(ide, src->path))
    {
        char r[1024] = { 0 };
        ide_project_relative(&ide->project, src->path, r, sizeof r);
        if (!ide_path_equal(r, src->path))   /* outside the project folder: kept whole */
        {
            snprintf(dir, sizeof dir, "%s", ide->project.dir);
            snprintf(rel, sizeof rel, "%s", r);
        }
    }
    char sub[1100] = { 0 }, path[1400] = { 0 };
    join_path(sub, sizeof sub, dir, platform);
    join_path(path, sizeof path, sub, rel);
    if (!ide_file_exists(path))
    {
        char msg[1500] = { 0 };
        snprintf(msg, sizeof msg, "File not found:\n%s", path);
        static const char* const labels[] = { "OK" };
        static const int ids[] = { 0 };
        gui_message_box(ide->app, "Error", msg, labels, ids, 1);
        return;
    }

    open_file(ide, path);
}

/* --- Find in Files --- */

static const char* const fif_types[] = { "*.c", "*.h", "*.c;*.h", "*.md", "*.*" };

static int fif_type_matches(int type, const char* name)
{
    return type == 4 || matches_filter(name, fif_types[type]);
}

/* A child of the panel at `row`, stretched between its side borders. */
static struct gui_node* fif_add(struct ide* ide, enum gui_kind kind, int row, int rows, const char* label)
{
    struct gui_node* n = create(ide, kind, label);
    struct gui_layout l = { GUI_ANCHOR_LEFT | GUI_ANCHOR_TOP | GUI_ANCHOR_RIGHT };
    l.left.cells = l.right.cells = 2;
    l.top.cells = row;
    l.height.cells = rows;
    gui_set_layout(n, &l);
    gui_append(ide->fif.window, n);
    return n;
}

static void fif_label(struct ide* ide, int row, const char* text)
{
    gui_set_colors(fif_add(ide, GUI_TEXT, row, 1, text), ide->theme->label_fg, ide->theme->window_bg);
}

/* The widgets' values back into the settings. */
static void fif_sync(struct ide* ide)
{
    struct find_in_files* f = &ide->fif;
    if (!f->find)
        return;
    snprintf(f->find_text, sizeof f->find_text, "%s", gui_get_value(f->find));
    if (f->replace)
        snprintf(f->replace_text, sizeof f->replace_text, "%s", gui_get_value(f->replace));
    f->match_case = gui_get_checked(f->options, 0);
    f->whole_word = gui_get_checked(f->options, 1);
    f->look = gui_get_selected(f->look_in);
    f->file_type = gui_get_selected(f->file_types);
}

/* The panel's content for its mode: tabs, Find (and Replace), options,
 * Look in, File Types, the buttons - as the old IDE's fr_rebuild_content. */
static void fif_build(struct ide* ide)
{
    struct find_in_files* f = &ide->fif;
    gui_clear_children(f->window);
    int cw = 0, ch = 0;
    gui_cell_size(ide->app, &cw, &ch);
    /* two tabs, each half of the panel, following its width */
    struct gui_node* tab_find = add_at(ide, f->window, GUI_BUTTON, 2, 1, 12, 1, "Find");
    struct gui_layout half = { GUI_ANCHOR_LEFT | GUI_ANCHOR_TOP | GUI_ANCHOR_RIGHT };
    half.left.cells = 2;
    half.right.percent = 50;
    half.top.cells = 1;
    half.height.cells = 1;
    gui_set_layout(tab_find, &half);
    gui_set_id(tab_find, EV_FR_TAB_FIND);
    gui_button_set_tab(tab_find, !f->mode);
    struct gui_node* tab_replace = add_at(ide, f->window, GUI_BUTTON, 14, 1, 12, 1, "Replace");
    half.left.cells = 0;
    half.left.percent = 50;
    half.right.percent = 0;
    half.right.cells = 2;
    gui_set_layout(tab_replace, &half);
    gui_set_id(tab_replace, EV_FR_TAB_REPLACE);
    gui_button_set_tab(tab_replace, f->mode);
    int row = 3;
    fif_label(ide, row++, "Find:");
    f->find = fif_add(ide, GUI_INPUT, row, 1, NULL);
    gui_set_value(f->find, f->find_text);
    gui_set_id(f->find, EV_FR_FIND);   /* Enter == Find */
    row += 2;
    f->replace = NULL;
    if (f->mode)
    {
        fif_label(ide, row++, "Replace:");
        f->replace = fif_add(ide, GUI_INPUT, row, 1, NULL);
        gui_set_value(f->replace, f->replace_text);
        row += 2;
    }
    f->options = fif_add(ide, GUI_GROUP, row, 2, NULL);
    gui_set_multi(f->options, 1);
    gui_append(f->options, create(ide, GUI_ITEM, "Match case"));
    gui_append(f->options, create(ide, GUI_ITEM, "Match whole word"));
    gui_set_checked(f->options, 0, f->match_case);
    gui_set_checked(f->options, 1, f->whole_word);
    row += 3;
    fif_label(ide, row++, "Look in:");
    f->look_in = fif_add(ide, GUI_GROUP, row, 4, NULL);
    static const char* const looks[] = { "Current File", "Current Dir", "Include Dir", "Project" };
    for (int i = 0; i < 4; i++)
        gui_append(f->look_in, create(ide, GUI_ITEM, looks[i]));
    gui_set_selected(f->look_in, f->look);
    set_help(ide, f->look_in,
             "Where to search",
             HELP_FIND_LOOK_IN);
    set_help(ide, gui_child_at(f->look_in, 0),
             "Current File: the active document",
             HELP_LOOK_IN_FILE);
    set_help(ide, gui_child_at(f->look_in, 1),
             "Current Dir: every file in the active document's folder",
             HELP_LOOK_IN_DIR);
    set_help(ide, gui_child_at(f->look_in, 2),
             "Include Dir: the directories the compiler searches for headers",
             HELP_LOOK_IN_INCLUDE_DIRS);
    set_help(ide, gui_child_at(f->look_in, 3),
             "Project: every file in the open project",
             HELP_LOOK_IN_PROJECT);
    row += 5;
    fif_label(ide, row++, "File Types:");
    f->file_types = fif_add(ide, GUI_SELECT, row, 1, NULL);
    for (int i = 0; i < COUNT(fif_types); i++)
        gui_append(f->file_types, create(ide, GUI_ITEM, fif_types[i]));
    gui_set_selected(f->file_types, f->file_type);
    row += 2;
    if (f->mode)
    {
        gui_set_id(add_at(ide, f->window, GUI_BUTTON, 2, row, 11, 1, "Find"), EV_FR_FIND);
        gui_set_id(add_at(ide, f->window, GUI_BUTTON, 14, row, 11, 1, "Replace"), EV_FR_REPLACE);
    }
    else
    {
        gui_set_id(fif_add(ide, GUI_BUTTON, row, 1, "Find"), EV_FR_FIND);
    }
    gui_focus(ide->app, f->find);
}

/* Ctrl+F: the panel, its Find field filled with the active selection. */
static void fif_open(struct ide* ide)
{
    struct find_in_files* f = &ide->fif;
    fif_sync(ide);
    struct doc* d = active_doc(ide);
    if (d)
    {
        int lo = 0, hi = 0;
        gui_editor_get_selection(d->editor, &lo, &hi);
        if (hi > lo && hi - lo < (int)sizeof f->find_text && !memchr(gui_get_value(d->editor) + lo, '\n', (size_t)(hi - lo)))
            snprintf(f->find_text, sizeof f->find_text, "%.*s", hi - lo, gui_get_value(d->editor) + lo);
    }
    gui_window_open(ide->app, f->window);
    fif_build(ide);
}

/* One file's search or replace: the open document's text when it is open
 * (unsaved changes included), else the disk's. A replace changes the open
 * document, or opens the file with the new text - never the disk. Returns
 * the count; *searched counts the file. */
static int fif_file(struct ide* ide, const char* path, const char* name, int replace,
                    const struct ide_search* s, struct ide_text* out, int* searched)
{
    struct doc* d = find_doc(ide, path);
    int crlf = 0;
    char* loaded = d ? NULL : ide_read_file(path, &crlf);
    const char* text = d ? gui_get_value(d->editor) : loaded;
    if (!text)
        return 0;
    (*searched)++;
    int count = 0;
    if (!replace)
    {
        count = ide_search_text(name, text, s, out);
    }
    else
    {
        char* changed = ide_replace_text(text, s, ide->fif.replace_text, &count);
        if (changed)
        {
            ide_text_printf(out, "%s: %d replacement(s)\n", name, count);
            if (!d)
            {
                open_file(ide, path);
                d = find_doc(ide, path);
            }
            if (d)
            {
                gui_set_value(d->editor, changed);
                gui_editor_set_dirty(d->editor, 1);
            }
            free(changed);
        }
    }
    free(loaded);
    return count;
}

/* The .c/.h... files of one folder (not its subfolders). Returns the count,
 * -1 when the folder cannot be read. */
static int fif_dir(struct ide* ide, const char* dir, int full_names, int replace,
                   const struct ide_search* s, struct ide_text* out, int* searched, int* changed)
{
    struct ide_dir_entry* entries = malloc(sizeof *entries * MAX_FOLDER_ENTRIES);
    int n = entries ? ide_list_dir(dir, entries, MAX_FOLDER_ENTRIES) : -1;
    if (n < 0)
    {
        free(entries);
        return -1;
    }
    int total = 0;
    for (int i = 0; i < n; i++)
    {
        if (entries[i].is_dir || !fif_type_matches(ide->fif.file_type, entries[i].name))
            continue;
        char path[1400] = { 0 };
        join_path(path, sizeof path, dir, entries[i].name);
        int c = fif_file(ide, path, full_names ? path : entries[i].name, replace, s, out, searched);
        total += c;
        if (c > 0)
            (*changed)++;
    }
    free(entries);
    return total;
}

/* Find or Replace with the panel's settings; the result in Find Results,
 * worded as the old IDE's. */
static void fif_run(struct ide* ide, int replace)
{
    struct find_in_files* f = &ide->fif;
    fif_sync(ide);
    struct ide_search s = { 0 };
    snprintf(s.pattern, sizeof s.pattern, "%s", f->find_text);
    s.match_case = f->match_case;
    s.whole_word = f->whole_word;
    struct ide_text out = { 0 };
    int total = 0, searched = 0, changed = 0;
    const char* what = replace ? "replace in" : "search";
    struct doc* d = active_doc(ide);
    findresults_begin(ide);
    if (!s.pattern[0])
    {
        ide_text_printf(&out, "Nothing to find - the Find field is empty.\n");
    }
    else if (f->look == LOOK_IN_FILE)
    {
        if (!d)
        {
            ide_text_printf(&out, "No file is open to %s.\n", replace ? "replace in" : "search");
        }
        else
        {
            const char* name = file_name(d->path);
            total = fif_file(ide, d->path, name, replace, &s, &out, &searched);
            if (replace)
            {
                if (total == 0)
                    ide_text_printf(&out, "\"%s\" not found in %s - nothing replaced.\n", s.pattern, name);
                else
                    ide_text_printf(&out, "%d replacement(s) of \"%s\" with \"%s\" in %s.\n"
                                           "(Not saved - use File > Save to write it to disk.)\n",
                                     total, s.pattern, f->replace_text, name);
            }
            else if (total == 0)
                ide_text_printf(&out, "\"%s\" not found in %s.\n", s.pattern, name);
            else
                ide_text_printf(&out, "\n%d occurrence(s) of \"%s\" in %s.\n", total, s.pattern, name);
        }
    }
    else if (f->look == LOOK_IN_DIR)
    {
        if (!d)
        {
            ide_text_printf(&out, "No file is open, so there's no directory to %s.\n", what);
        }
        else
        {
            char dir[1024] = { 0 };
            snprintf(dir, sizeof dir, "%s", d->path);
            parent_dir(dir);
            total = fif_dir(ide, dir, 0, replace, &s, &out, &searched, &changed);
            if (total < 0)
                ide_text_printf(&out, "Could not open directory: %s\n", dir);
            else if (total == 0)
                ide_text_printf(&out, "\"%s\" not found in %d file(s) in %s%s.\n", s.pattern, searched, dir,
                                 replace ? " - nothing replaced" : "");
            else if (replace)
                ide_text_printf(&out, "\n%d replacement(s) of \"%s\" with \"%s\" in %d file(s) (%d changed) in %s.\n"
                                       "(Not saved - use File > Save all to write them to disk.)\n",
                                 total, s.pattern, f->replace_text, searched, changed, dir);
            else
                ide_text_printf(&out, "\n%d occurrence(s) of \"%s\" in %d file(s) in %s.\n", total, s.pattern, searched, dir);
        }
    }
    else if (f->look == LOOK_IN_INCLUDE_DIRS)
    {
        if (replace)
        {
            ide_text_printf(&out, "Replace isn't available for \"Include Dir\" - those are the system headers. "
                                   "Use Find to search them.\n");
        }
        else
        {
            /* Cake's own include folder first, then the target's include directories - the compiler's order */
            char builtin[1024] = { 0 };
            ide_exe_dir(builtin, sizeof builtin);
            snprintf(builtin + strlen(builtin), sizeof builtin - strlen(builtin), IDE_PATH_SEP "include");
            struct doc* active = active_doc(ide);
            int project = active ? file_uses_project(ide, active->path) : ide_project_is_open(&ide->project);
            const struct include_dirs* l = &settings_in_use(project ? &ide->project.compile : &ide->global_options)->include_dirs;
            int dirs = 0;
            for (int i = -1; i < l->count; i++)
            {
                char abs[1024] = { 0 };
                if (i >= 0 && project)
                    ide_project_absolute(&ide->project, l->dirs[i], abs, sizeof abs);
                else
                    snprintf(abs, sizeof abs, "%s", i < 0 ? builtin : l->dirs[i]);
                const char* dir = abs;
                int n = fif_dir(ide, dir, 1, 0, &s, &out, &searched, &changed);
                if (n >= 0)
                {
                    total += n;
                    dirs++;
                }
                else
                {
                    ide_text_printf(&out, "Could not open directory: %s\n", dir);
                }
            }
            if (total == 0)
                ide_text_printf(&out, "\"%s\" not found in %d file(s) in %d include directories.\n", s.pattern, searched, dirs);
            else
                ide_text_printf(&out, "\n%d occurrence(s) of \"%s\" in %d file(s) in %d include directories.\n",
                                 total, s.pattern, searched, dirs);
        }
    }
    else
    {
        struct ide_project* p = &ide->project;
        if (!ide_project_is_open(p))
        {
            ide_text_printf(&out, "No project is open, so there's nothing to %s.\n", replace ? "replace in" : "search");
        }
        else
        {
            for (int i = 0; i < p->files.count; i++)
            {
                const char* entry = p->files.items[i];
                if (!fif_type_matches(f->file_type, file_name(entry)))
                    continue;
                char abs[1024] = { 0 };
                ide_project_absolute(p, entry, abs, sizeof abs);
                int c = fif_file(ide, abs, entry, replace, &s, &out, &searched);
                total += c;
                if (c > 0)
                    changed++;
            }
            if (total == 0)
                ide_text_printf(&out, "\"%s\" not found in %d file(s) in project \"%s\"%s.\n", s.pattern, searched, p->name,
                                 replace ? " - nothing replaced" : "");
            else if (replace)
                ide_text_printf(&out, "\n%d replacement(s) of \"%s\" with \"%s\" in %d file(s) (%d changed) in project \"%s\".\n"
                                       "(Not saved - use File > Save all to write them to disk.)\n",
                                 total, s.pattern, f->replace_text, searched, changed, p->name);
            else
                ide_text_printf(&out, "\n%d occurrence(s) of \"%s\" in %d file(s) in project \"%s\".\n",
                                 total, s.pattern, searched, p->name);
        }
    }
    findresults_set(ide, out.data ? out.data : "");
    free(out.data);
    bottom_panel_show(ide, ide->fr.window, ide->output.window);
}

/* --- Outside changes: every 2 s, with no modal open, the .cakeproj and the
 * active document are checked against their times; a change asks to
 * reload - one prompt at a time, as the old IDE's file_watch_check. --- */

static void file_watch_check(struct ide* ide)
{
    if (gui_modal_open(ide->app))
        return;
    struct ide_project* p = &ide->project;
    if (ide_project_is_open(p) && p->file_time != 0)
    {
        long long t = ide_file_time(p->file_path);
        if (t != 0 && t != p->file_time)
        {
            p->file_time = t;
            char msg[1400] = { 0 };
            snprintf(msg, sizeof msg, "The project file was modified outside the IDE:\n%s\n\nReload it?",
                     p->file_path);
            static const char* const labels[] = { "Yes", "No" };
            static const int ids[] = { EV_PROJECT_RELOAD, 0 };
            gui_message_box(ide->app, "Project Changed", msg, labels, ids, 2);
            gui_repaint(ide->app);
            return;
        }
    }
    struct doc* d = active_doc(ide);
    if (!d || d->file_time == 0)
        return;
    long long t = ide_file_time(d->path);
    if (t == 0 || t == d->file_time)
        return;
    d->file_time = t;
    snprintf(ide->reload_path, sizeof ide->reload_path, "%s", d->path);
    char msg[1400] = { 0 };
    snprintf(msg, sizeof msg, "The file was modified outside the IDE:\n%s\n\n%sReload it?", d->path,
             gui_editor_get_dirty(d->editor) ? "Your unsaved changes will be lost.\n" : "");
    static const char* const labels[] = { "Yes", "No" };
    static const int ids[] = { EV_FILE_RELOAD, 0 };
    gui_message_box(ide->app, "File Changed", msg, labels, ids, 2);
    gui_repaint(ide->app);
}

/* Reload: the disk's text, the caret kept where it was. */
static void file_reload(struct ide* ide)
{
    struct doc* d = find_doc(ide, ide->reload_path);
    if (!d)
        return;
    int crlf = 0;
    char* text = ide_read_file(d->path, &crlf);
    if (!text)
        return;
    int lo = 0, hi = 0;
    gui_editor_get_selection(d->editor, &lo, &hi);
    gui_set_value(d->editor, text);
    int len = (int)strlen(text);
    gui_editor_set_selection(d->editor, lo < len ? lo : len, lo < len ? lo : len);
    gui_editor_set_dirty(d->editor, 0);
    d->crlf = crlf;
    d->file_time = ide_file_time(d->path);
    free(text);
}

/* --- Settings: ide.json beside the executable - the IDE's own file, not
 * the old IDE's cake.json. Saved whenever a dialog's OK changes them. --- */

/* The playground project: playground.cakeproj, next to playground.c. Its
 * settings are global_options - Project > Properties edits them, and every file
 * outside the open project (the playground among them) compiles with them. */
static int playground_project_path(char* out, size_t cap)
{
    char dir[1024] = { 0 };
    if (!ide_config_dir(dir, sizeof dir))
        return 0;
    join_path(out, cap, dir, "playground.cakeproj");
    return 1;
}

static void playground_project_save(struct ide* ide)
{
    char path[1400] = { 0 };
    if (!playground_project_path(path, sizeof path))
        return;
    struct json_value* root = calloc(1, sizeof *root);
    if (!root)
        return;
    root->type = JSON_OBJECT;
    json_set_string(root, "name", "playground");
    compile_to_json(json_set_object(root, "compile"), &ide->global_options);
    struct json_value* files = json_set_array(root, "files");
    if (files)
        json_add_string(files, "playground.c");
    if (!json_write_file(path, root))
    {
        char msg[1600] = { 0 };
        snprintf(msg, sizeof msg, "Cannot write %s", path);
        status(ide, msg);
    }
    json_delete(root);
}

/* Missing: global_options keep what ide.json had (the old place). */
static void playground_project_load(struct ide* ide)
{
    char path[1400] = { 0 };
    if (!playground_project_path(path, sizeof path))
        return;
    int crlf = 0;
    char* text = ide_read_file(path, &crlf);
    if (!text)
        return;
    struct json_error error = { 0 };
    struct json_value* root = json_parse(text, &error);
    free(text);
    if (root && root->type == JSON_OBJECT)
        compile_from_json(json_find_member(root, "compile"), &ide->global_options);
    json_delete(root);
}

static void settings_path(char* out, size_t cap)
{
    char dir[1024] = { 0 };
    ide_exe_dir(dir, sizeof dir);
    join_path(out, cap, dir, "ide.json");
}

static void get_string(const struct json_value* obj, const char* key, char* out, size_t cap)
{
    const struct json_value* v = obj ? json_find_member(obj, key) : NULL;
    if (v && v->type == JSON_STRING && v->string)
        snprintf(out, cap, "%s", v->string);
}

/* The string member `key` of `obj`, "" when missing. */
static const char* get_cstring(const struct json_value* obj, const char* key)
{
    const struct json_value* v = obj ? json_find_member(obj, key) : NULL;
    return v && v->type == JSON_STRING && v->string ? v->string : "";
}

static int get_int(const struct json_value* obj, const char* key, int fallback)
{
    const struct json_value* v = obj ? json_find_member(obj, key) : NULL;
    if (v && v->type == JSON_NUMBER)
        return (int)v->number;
    if (v && (v->type == JSON_TRUE || v->type == JSON_FALSE))
        return v->type == JSON_TRUE;
    return fallback;
}

static int clamp_index(int i, int count)
{
    return i >= 0 && i < count ? i : 0;
}

/* The "compile" object, in the old IDE's .cakeproj keys, so both IDEs read
 * each other's projects. */
static const char* const flag_keys[] = { "line_directives", "flow", "const_literal", "wall", "default_nonnull", "check_annotations" };

static int slug_index(const char* slug, const char* const* slugs, int count);

static void target_to_json(struct json_value* c, const struct target_settings* s)
{
    if (!c)
        return;
    json_set_string(c, "target", s->cake_target);
    json_set_string(c, "options", s->options);
    json_set_string(c, "output", s->output);
    json_set_string(c, "style", style_slugs[clamp_index(s->style, COUNT(style_slugs))]);
    json_set_string(c, "diagnostic_format", diag_slugs[clamp_index(s->diag, COUNT(diag_slugs))]);
    for (int i = 0; i < COUNT(flag_keys); i++)
        json_set_bool(c, flag_keys[i], s->flags[i]);
    json_set_bool(c, "use_cake_headers", s->headers == 1);
    struct json_value* bld = json_set_object(c, "build");
    if (bld)
    {
        static const char* const keys[] = { "command", "arguments", "directory" };
        struct json_value* pre = json_set_object(bld, "pre_build");
        struct json_value* post = json_set_object(bld, "post_build");
        for (int i = 0; i < 3; i++)
        {
            if (pre)
                json_set_string(pre, keys[i], s->pre_build[i]);
            if (post)
                json_set_string(post, keys[i], s->post_build[i]);
        }
    }
    struct json_value* dirs = json_set_array(c, "include_dirs");
    for (int i = 0; dirs && i < s->include_dirs.count; i++)
        json_add_string(dirs, s->include_dirs.dirs[i]);
    struct json_value* dbg = json_set_object(c, "debugger");
    if (dbg)
    {
        json_set_string(dbg, "command", s->debug[0]);
        json_set_string(dbg, "arguments", s->debug[1]);
        json_set_string(dbg, "directory", s->debug[2]);
    }
}

/* An "include_dirs" array; none keeps the list. */
static void dirs_from_json(const struct json_value* dirs, struct include_dirs* l)
{
    if (!dirs || dirs->type != JSON_ARRAY)
        return;
    l->count = 0;
    size_t n = json_count(dirs);
    for (size_t i = 0; i < n && l->count < MAX_INCLUDE_DIRS; i++)
    {
        const struct json_value* d = json_item(dirs, i);
        if (d && d->type == JSON_STRING && d->string)
            snprintf(l->dirs[l->count++], sizeof l->dirs[0], "%s", d->string);
    }
}

/* "build": "pre_build" and "post_build", each a command, arguments, directory. */
static void build_from_json(const struct json_value* bld, struct target_settings* s)
{
    static const char* const keys[] = { "command", "arguments", "directory" };
    const struct json_value* pre = json_find_member(bld, "pre_build");
    const struct json_value* post = json_find_member(bld, "post_build");
    for (int i = 0; i < 3; i++)
    {
        get_string(pre, keys[i], s->pre_build[i], sizeof s->pre_build[i]);
        get_string(post, keys[i], s->post_build[i], sizeof s->post_build[i]);
    }
}

/* "debug": command, arguments, directory - "working_dir" in the old .cakeproj. */
static void debug_from_json(const struct json_value* dbg, struct target_settings* s)
{
    get_string(dbg, "command", s->debug[0], sizeof s->debug[0]);
    get_string(dbg, "arguments", s->debug[1], sizeof s->debug[1]);
    get_string(dbg, "working_dir", s->debug[2], sizeof s->debug[2]);
    get_string(dbg, "directory", s->debug[2], sizeof s->debug[2]);
}

static void target_from_json(const struct json_value* c, struct target_settings* s)
{
    if (!c || c->type != JSON_OBJECT)
        return;
    get_string(c, "target", s->cake_target, sizeof s->cake_target);
    get_string(c, "options", s->options, sizeof s->options);
    get_string(c, "output", s->output, sizeof s->output);
    char slug[64] = "";
    get_string(c, "style", slug, sizeof slug);
    s->style = slug_index(slug, style_slugs, COUNT(style_slugs));
    snprintf(slug, sizeof slug, "ide");
    get_string(c, "diagnostic_format", slug, sizeof slug);
    s->diag = slug_index(slug, diag_slugs, COUNT(diag_slugs));
    for (int i = 0; i < COUNT(flag_keys); i++)
        s->flags[i] = get_int(c, flag_keys[i], s->flags[i]);
    s->headers = get_int(c, "use_cake_headers", s->headers == 1) ? 1 : 0;
    debug_from_json(json_find_member(c, "debug"), s);   /* the old files' name */
    debug_from_json(json_find_member(c, "debugger"), s);
    build_from_json(json_find_member(c, "build"), s);
    dirs_from_json(json_find_member(c, "include_dirs"), &s->include_dirs);
}

/* "compile": the configuration in use, by its name, and "configurations",
 * each with its name and options. */
static void compile_to_json(struct json_value* c, const struct compiler_settings* s)
{
    if (!c)
        return;
    json_set_string(c, "configuration", s->current >= 0 && s->current < s->count ? s->configurations[s->current].name : "");
    struct json_value* list = json_set_array(c, "configurations");
    for (int i = 0; list && i < s->count; i++)
    {
        struct json_value* o = json_add_object(list);
        if (!o)
            continue;
        json_set_string(o, "name", s->configurations[i].name);
        target_to_json(o, &s->configurations[i]);
    }
}

/* Without "configurations" - nothing, or the old format - the defaults stay. */
static void compile_from_json(const struct json_value* c, struct compiler_settings* s)
{
    if (!c || c->type != JSON_OBJECT)
        return;
    const struct json_value* list = json_find_member(c, "configurations");
    if (!list || list->type != JSON_ARRAY)
        return;
    s->count = 0;
    size_t n = json_count(list);
    for (size_t i = 0; i < n && s->count < MAX_CONFIGURATIONS; i++)
    {
        const struct json_value* o = json_item(list, i);
        if (!o || o->type != JSON_OBJECT)
            continue;
        struct target_settings* t = &s->configurations[s->count++];
        config_default(t);
        get_string(o, "name", t->name, sizeof t->name);
        target_from_json(o, t);
    }
    char name[64] = "";
    get_string(c, "configuration", name, sizeof name);
    s->current = config_find(s, name);
    if (s->current < 0 && s->count > 0)
        s->current = 0;
}

/* --- Projects: the old IDE's - a .cakeproj (JSON) with the name, the
 * compile settings, the include directories and the files, the last two
 * relative to the project folder --- */

static void project_refresh(struct ide* ide, int selected)
{
    struct ide_project* p = &ide->project;
    gui_clear_children(ide->project_list);
    for (int i = 0; i < p->files.count; i++)
    {
        /* The old IDE's marker: a letter per file type, in the theme's color for it. */
        const char* f = p->files.items[i];
        char label[600] = { 0 };
        snprintf(label, sizeof label, "%c %s",
                 ends_with(f, ".c") ? 'C' : ends_with(f, ".h") ? 'H' : ends_with(f, ".md") ? 'M' : ' ', f);
        struct gui_node* item = create(ide, GUI_ITEM, label);
        if (ends_with(f, ".c"))
            gui_set_colors(item, ide->theme->project_icon_c_fg, 0);
        else if (ends_with(f, ".h"))
            gui_set_colors(item, ide->theme->project_icon_h_fg, 0);
        else if (ends_with(f, ".md"))
            gui_set_colors(item, ide->theme->project_icon_md_fg, 0);
        gui_append(ide->project_list, item);
    }
    if (selected >= p->files.count)
        selected = p->files.count - 1;
    if (selected >= 0)
        gui_set_selected(ide->project_list, selected);
    gui_set_label(ide->project_window, p->name[0] ? p->name : "Project");
    config_menu_refresh(ide);
    for (int i = 0; i < COUNT(ide->project_items); i++)
    {
        if (ide->project_items[i])
            gui_set_enabled(ide->project_items[i], ide_project_is_open(p));
    }
}

static void project_save(struct ide* ide)
{
    struct ide_project* p = &ide->project;
    if (!ide_project_is_open(p))
        return;
    struct json_value* root = calloc(1, sizeof *root);
    if (!root)
        return;
    root->type = JSON_OBJECT;
    json_set_string(root, "name", p->name);
    struct json_value* c = json_set_object(root, "compile");
    compile_to_json(c, &p->compile);
    struct json_value* files = json_set_array(root, "files");
    for (int i = 0; i < p->files.count; i++)
        json_add_string(files, p->files.items[i]);
    if (!json_write_file(p->file_path, root))
    {
        char msg[1200] = { 0 };
        snprintf(msg, sizeof msg, "Cannot write %s", p->file_path);
        status(ide, msg);
    }
    json_delete(root);
    p->file_time = ide_file_time(p->file_path);
}

/* The Project panel takes the Folder panel's place when that is on the left. */
/* Folder, Project and Git are one place: the same side and size, only one
 * of them shown. The place follows the shown one when it is resized,
 * redocked or dragged. */
static struct gui_node* side_panel(struct ide* ide, int i)
{
    return i == 0 ? ide->folder.window : i == 1 ? ide->project_window : ide->git.window;
}

/* The place, from the panel shown now - or, when all are closed, from the
 * one shown last, which keeps its place while closed. */
static void side_capture(struct ide* ide)
{
    int found = ide->side.active;
    for (int i = 0; i < 3; i++)
    {
        struct gui_node* win = side_panel(ide, i);
        if (win && is_open(ide, win))
        {
            found = i;
            break;
        }
    }
    struct gui_node* win = side_panel(ide, found);
    if (!win)
        return;
    struct gui_rect r = gui_window_get_rect(win);
    ide->side.side = gui_window_get_dock(win);
    ide->side.rect = r;
    ide->side.size = ide->side.side == GUI_DOCK_BOTTOM ? r.h : r.w;
    ide->side.active = found;
}

static void side_apply(struct ide* ide, struct gui_node* win)
{
    if (ide->side.side == GUI_DOCK_NONE && ide->side.rect.w > 0)
    {
        /* inside the area - the place may be from before the window shrank */
        struct gui_rect a = gui_desktop_rect(ide->app);
        struct gui_rect r = ide->side.rect;
        if (r.w > a.w) r.w = a.w;
        if (r.h > a.h) r.h = a.h;
        if (r.x + r.w > a.x + a.w) r.x = a.x + a.w - r.w;
        if (r.y + r.h > a.y + a.h) r.y = a.y + a.h - r.h;
        if (r.x < a.x) r.x = a.x;
        if (r.y < a.y) r.y = a.y;
        gui_window_set_dock(win, GUI_DOCK_NONE, 0);
        gui_window_set_rect(win, &r);
    }
    else if (ide->side.side != GUI_DOCK_NONE)
    {
        int cw = 0, ch = 0;
        gui_cell_size(ide->app, &cw, &ch);
        int min_size = 8 * (ide->side.side == GUI_DOCK_BOTTOM ? ch : cw);   /* as dragging */
        gui_window_set_dock(win, ide->side.side, ide->side.size < min_size ? min_size : ide->side.size);
    }
}

static void show_side_panel(struct ide* ide, struct gui_node* win)
{
    side_capture(ide);
    for (int i = 0; i < 3; i++)
    {
        struct gui_node* other = side_panel(ide, i);
        if (other && other != win && is_open(ide, other))
            gui_window_close(ide->app, other);
        if (other == win)
            ide->side.active = i;
    }
    side_apply(ide, win);
    gui_window_open(ide->app, win);
}

static void project_show_panel(struct ide* ide)
{
    show_side_panel(ide, ide->project_window);
}

/* Loads `path`, replacing any open project. 0 if it cannot be read; then
 * `why` says what is wrong. */
static int project_load(struct ide* ide, const char* path, char* why, size_t why_cap)
{
    int crlf = 0;
    char* text = ide_read_file(path, &crlf);
    if (!text)
    {
        snprintf(why, why_cap, "The file cannot be read.");
        return 0;
    }
    struct json_error error = { 0 };
    struct json_value* root = json_parse(text, &error);
    free(text);
    if (!root || root->type != JSON_OBJECT)
    {
        if (!root)
            snprintf(why, why_cap, "Line %d, column %d: %s", (int)error.line, (int)error.column, error.message);
        else
            snprintf(why, why_cap, "It is not a JSON object.");
        json_delete(root);
        return 0;
    }
    struct ide_project* p = &ide->project;
    ide_project_reset(p);
    compiler_settings_default(&p->compile);   /* a field the file lacks is the default, not the Playground's */
    const struct json_value* c = json_find_member(root, "compile");
    compile_from_json(c, &p->compile);
    get_string(root, "name", p->name, sizeof p->name);
    const struct json_value* files = json_find_member(root, "files");
    size_t n = files && files->type == JSON_ARRAY ? json_count(files) : 0;
    for (size_t i = 0; i < n; i++)
    {
        const struct json_value* f = json_item(files, i);
        if (f && f->type == JSON_STRING && f->string)
            ide_strings_add(&p->files, f->string);
    }
    json_delete(root);

    ide_full_path(path, p->file_path, sizeof p->file_path);
    p->file_time = ide_file_time(p->file_path);
    snprintf(p->dir, sizeof p->dir, "%s", p->file_path);
    parent_dir(p->dir);
    if (!p->name[0])
    {
        snprintf(p->name, sizeof p->name, "%s", file_name(p->file_path));
        char* dot = strrchr(p->name, '.');
        if (dot)
            *dot = '\0';
    }
    return 1;
}

static void recent_menu_refresh(struct ide* ide);

static void recent_project_remove(struct ide* ide, const char* path)
{
    struct ide_strings* r = &ide->recent_projects;
    for (int i = r->count - 1; i >= 0; i--)
    {
        if (ide_path_equal(r->items[i], path))
            ide_strings_remove_at(r, i);
    }
    if (ide->recent_menu)
        recent_menu_refresh(ide);
}

/* File > Recent Projects: a row per project, "name.cakeproj (full path)". */
static void recent_menu_refresh(struct ide* ide)
{
    const struct ide_strings* r = &ide->recent_projects;
    gui_clear_children(ide->recent_menu);
    for (int i = 0; i < r->count && i < MAX_RECENT_PROJECTS; i++)
    {
        char label[1200] = { 0 };
        snprintf(label, sizeof label, "%s (%s)", file_name(r->items[i]), r->items[i]);
        struct gui_node* it = create(ide, GUI_ITEM, label);
        gui_set_id(it, EV_RECENT_ITEM + i);
        gui_set_hint(it, r->items[i]);
        gui_append(ide->recent_menu, it);
    }
    if (r->count == 0)
    {
        struct gui_node* none = create(ide, GUI_ITEM, "(none)");
        gui_set_enabled(none, 0);
        gui_append(ide->recent_menu, none);
    }
}

/* `path` to the top of the recent projects. */
static void recent_project_add(struct ide* ide, const char* path)
{
    struct ide_strings* r = &ide->recent_projects;
    recent_project_remove(ide, path);
    if (!ide_strings_add(r, path))
        return;
    char* added = r->items[r->count - 1];
    memmove(r->items + 1, r->items, (size_t)(r->count - 1) * sizeof *r->items);
    r->items[0] = added;
    while (r->count > MAX_RECENT_PROJECTS)
        ide_strings_remove_at(r, r->count - 1);
    recent_menu_refresh(ide);
}

/* A running compile: a Build commits its results into ide->project when it
 * ends, so the project cannot change under it. 1 (and says so) when busy. */
static int project_switch_busy(struct ide* ide)
{
    if (!ide_compile_running(ide->job))
        return 0;
    status(ide, "Wait for the compile to finish before changing the project");
    return 1;
}

/* The open project's files that have no unsaved changes, closed - before
 * another project takes its place, so its tabs do not stay behind. */
static void project_close_docs(struct ide* ide)
{
    if (!ide_project_is_open(&ide->project))
        return;
    for (int i = ide->doc_count - 1; i >= 0; i--)
    {
        if (ide_project_contains(&ide->project, ide->docs[i].path) && !gui_editor_get_dirty(ide->docs[i].editor))
            close_doc(ide, &ide->docs[i]);
    }
}

static void project_open(struct ide* ide, const char* path)
{
    char why[300] = { 0 };
    if (project_switch_busy(ide))
        return;
    project_close_docs(ide);
    if (!project_load(ide, path, why, sizeof why))
    {
        recent_project_remove(ide, path);   /* it does not open: out of the list */
        char msg[1600] = { 0 };
        snprintf(msg, sizeof msg, "Cannot open the project:\n%s\n\n%s", path, why);
        static const char* const labels[] = { "OK" };
        static const int ids[] = { 0 };
        gui_message_box(ide->app, "Open Project", msg, labels, ids, 1);
        return;
    }
    recent_project_add(ide, ide->project.file_path);
    project_refresh(ide, 0);
    project_show_panel(ide);
}

/* A new, empty project at `path` (its folder is created if needed). */
static void project_create(struct ide* ide, const char* path)
{
    struct ide_project* p = &ide->project;
    project_close_docs(ide);
    ide_project_reset(p);
    compiler_settings_default(&p->compile);   /* empty: nothing from the Playground */
    ide_full_path(path, p->file_path, sizeof p->file_path);
    snprintf(p->dir, sizeof p->dir, "%s", p->file_path);
    parent_dir(p->dir);
    ide_make_dir(p->dir);
    snprintf(p->name, sizeof p->name, "%s", file_name(p->file_path));
    char* dot = strrchr(p->name, '.');
    if (dot)
        *dot = '\0';
    project_save(ide);
    recent_project_add(ide, p->file_path);
    project_refresh(ide, 0);
    project_show_panel(ide);
}

/* Project files order: by name without extension, then by extension,
 * so a.c and a.h stay together. */
static int project_file_cmp(const void* a, const void* b)
{
    const char* x = *(const char* const*)a;
    const char* y = *(const char* const*)b;
    const char* dx = strrchr(x, '.');
    const char* dy = strrchr(y, '.');
    size_t nx = dx ? (size_t)(dx - x) : strlen(x);
    size_t ny = dy ? (size_t)(dy - y) : strlen(y);
    int r = strncmp(x, y, nx < ny ? nx : ny);
    if (r != 0)
        return r;
    if (nx != ny)
        return nx < ny ? -1 : 1;
    return strcmp(x + nx, y + ny);
}

/* Add Existing File / Add to Project / New File from the panel. */
static void project_add_file(struct ide* ide, const char* path)
{
    struct ide_project* p = &ide->project;
    if (!ide_project_is_open(p))
        return;
    char entry[1024] = { 0 };
    ide_project_relative(p, path, entry, sizeof entry);
    for (int i = 0; i < p->files.count; i++)
    {
        if (strcmp(p->files.items[i], entry) == 0)
            return;
    }
    if (!ide_strings_add(&p->files, entry))
        return;
    qsort(p->files.items, (size_t)p->files.count, sizeof p->files.items[0], project_file_cmp);
    int index = 0;
    for (int i = 0; i < p->files.count; i++)
    {
        if (strcmp(p->files.items[i], entry) == 0)
            index = i;
    }
    project_save(ide);
    project_refresh(ide, index);
}

static void project_remove_at(struct ide* ide, int index)
{
    struct ide_project* p = &ide->project;
    if (index < 0 || index >= p->files.count)
        return;
    ide_strings_remove_at(&p->files, index);
    project_save(ide);
    project_refresh(ide, index);
}

/* New Project's OK: Folder and Name; with Create Folder the project goes
 * into a new <Name> folder; with Hello World a main.c is written (never
 * over an existing one) and added. */
static void new_project_accept(struct ide* ide)
{
    struct new_project_dialog* d = &ide->new_project;
    const char* folder = gui_get_value(d->folder);
    const char* name = gui_get_value(d->name);
    static const char* const ok_label[] = { "OK" };
    static const int ok_id[] = { 0 };
    if (!folder[0] || !name[0])
    {
        gui_message_box(ide->app, "New Project", "Please fill in both folder and project name.", ok_label, ok_id, 1);
        return;
    }
    if (project_switch_busy(ide))
        return;
    char dir[1024] = { 0 };
    if (gui_get_checked(d->checks, 0))
    {
        join_path(dir, sizeof dir, folder, name);
        if (ide_make_dir(dir) != 0)
        {
            char msg[1200] = { 0 };
            snprintf(msg, sizeof msg, "Could not create folder:\n%s\nIt may already exist.", dir);
            gui_message_box(ide->app, "New Project", msg, ok_label, ok_id, 1);
            return;
        }
    }
    else
    {
        snprintf(dir, sizeof dir, "%s", folder);
    }
    char file[300] = { 0 }, path[1400] = { 0 };
    snprintf(file, sizeof file, "%s.cakeproj", name);
    join_path(path, sizeof path, dir, file);
    if (ide_file_exists(path))
    {
        gui_message_box(ide->app, "New Project", "Project already exists at that location.", ok_label, ok_id, 1);
        return;
    }
    gui_window_close(ide->app, d->window);
    project_create(ide, path);
    if (gui_get_checked(d->checks, 1))
    {
        char main_path[1400] = { 0 };
        join_path(main_path, sizeof main_path, ide->project.dir, "main.c");
        if (!ide_file_exists(main_path))
        {
            ide_write_file(main_path,
                            "#include <stdio.h>\n\nint main(void)\n{\n    printf(\"Hello, world!\\n\");\n    return 0;\n}\n",
                            1);
        }
        project_add_file(ide, main_path);
        open_file(ide, main_path);
    }
}

/* Rename Project's OK: the .cakeproj renamed on disk, in its same folder,
 * so the files and include directories relative to it stay valid. */
static void project_rename_accept(struct ide* ide)
{
    struct ide_project* p = &ide->project;
    static const char* const ok_label[] = { "OK" };
    static const int ok_id[] = { 0 };
    char name[300] = { 0 };
    snprintf(name, sizeof name, "%s", gui_get_value(ide->project_rename.input));
    if (ends_with(name, ".cakeproj"))
        name[strlen(name) - strlen(".cakeproj")] = '\0';
    if (!name[0] || strpbrk(name, "\\/:*?\"<>|"))
    {
        gui_message_box(ide->app, "Rename Project", "Please enter a valid project name.", ok_label, ok_id, 1);
        return;
    }
    char file[320] = { 0 }, path[1400] = { 0 };
    snprintf(file, sizeof file, "%s.cakeproj", name);
    join_path(path, sizeof path, p->dir, file);
    if (ide_path_equal(path, p->file_path))
    {
        gui_window_close(ide->app, ide->project_rename.window);
        return;
    }
    if (ide_file_exists(path))
    {
        gui_message_box(ide->app, "Rename Project", "A project with that name already exists.", ok_label, ok_id, 1);
        return;
    }
    if (rename(p->file_path, path) != 0)
    {
        char msg[1600] = { 0 };
        snprintf(msg, sizeof msg, "Could not rename the project to:\n%s", path);
        gui_message_box(ide->app, "Rename Project", msg, ok_label, ok_id, 1);
        return;
    }
    gui_window_close(ide->app, ide->project_rename.window);
    recent_project_remove(ide, p->file_path);
    snprintf(p->file_path, sizeof p->file_path, "%s", path);
    snprintf(p->name, sizeof p->name, "%s", name);
    project_save(ide);
    recent_project_add(ide, p->file_path);
    project_refresh(ide, gui_get_selected(ide->project_list));
}

/* Close Project: documents without unsaved changes close, the Playground
 * and the Folder panel come back. */
static void project_close(struct ide* ide)
{
    if (!ide_project_is_open(&ide->project))
        return;
    if (project_switch_busy(ide))
        return;
    ide_project_reset(&ide->project);
    project_refresh(ide, 0);
    gui_set_value(ide->output.editor, "");   /* the closed project's compile output */
    gui_window_close(ide->app, ide->project_window);
    for (int i = ide->doc_count - 1; i >= 0; i--)
    {
        if (!gui_editor_get_dirty(ide->docs[i].editor))
            close_doc(ide, &ide->docs[i]);
    }
    open_playground(ide);
    show_side_panel(ide, ide->folder.window);
}

/* The Project panel's Delete (and the Del key): asks Remove - from the
 * project only - or Delete - from the project and from disk. */
static void project_delete_ask(struct ide* ide)
{
    struct ide_project* p = &ide->project;
    int row = gui_get_selected(ide->project_list);
    if (row < 0 || row >= p->files.count)
        return;
    ide_project_absolute(p, p->files.items[row], ide->pending_delete, sizeof ide->pending_delete);
    char msg[1600] = { 0 };
    snprintf(msg, sizeof msg, "Remove this file from the project, or remove it and delete it from disk?\n%s",
             ide->pending_delete);
    static const char* const labels[] = { "Remove", "Delete", "Cancel" };
    static const int ids[] = { EV_PROJ_REMOVE, EV_PROJ_DELETE_OK, 0 };
    gui_message_box(ide->app, "Remove File", msg, labels, ids, 3);
}

static void project_delete_confirmed(struct ide* ide)
{
    struct ide_project* p = &ide->project;
    for (int i = 0; i < p->files.count; i++)
    {
        char abs[1024] = { 0 };
        ide_project_absolute(p, p->files.items[i], abs, sizeof abs);
        if (ide_path_equal(abs, ide->pending_delete))
        {
            project_remove_at(ide, i);
            break;
        }
    }
    for (int i = 0; i < ide->doc_count; i++)
    {
        if (ide_path_equal(ide->docs[i].path, ide->pending_delete))
        {
            close_doc(ide, &ide->docs[i]);
            break;
        }
    }
    ide_delete_path(ide->pending_delete, 0);
    ide->pending_delete[0] = '\0';
    folder_refresh(ide);
}

static void project_event(struct ide* ide, int id)
{
    struct ide_project* p = &ide->project;
    int row = gui_get_selected(ide->project_list);
    char abs[1024] = { 0 };
    switch (id)
    {
    case EV_PROJECT_LIST:
        if (row >= 0 && row < p->files.count)
        {
            ide_project_absolute(p, p->files.items[row], abs, sizeof abs);
            nav_record_jump(ide);
            open_file(ide, abs);
        }
        break;
    case EV_PROJ_COPY_PATH:
        if (row >= 0 && row < p->files.count)
        {
            ide_project_absolute(p, p->files.items[row], abs, sizeof abs);
            gui_set_clipboard(ide->app, abs);
        }
        break;
    case EV_PROJ_NEW_FILE:
        ide->newfile_in_project = 1;
        gui_set_value(ide->newfile.folder, p->dir);
        gui_set_value(ide->newfile.name, "");
        show_dialog(ide, ide->newfile.window, 54, 9, ide->newfile.name);
        break;
    case EV_PROJ_REMOVE: project_remove_at(ide, row); break;
    case EV_PROJ_DELETE: project_delete_ask(ide); break;
    case EV_PROJ_DELETE_OK: project_delete_confirmed(ide); break;
    case EV_FOLDER_ADD_TO_PROJECT:
    {
        struct folder_panel* f = &ide->folder;
        int frow = gui_get_selected(f->list);
        if (frow >= 1 && frow <= f->count && !f->entries[frow - 1].is_dir)
        {
            join_path(abs, sizeof abs, f->dir, f->entries[frow - 1].name);
            project_add_file(ide, abs);
        }
        break;
    }
    default:
        break;
    }
}

static void settings_save(struct ide* ide)
{
    struct json_value* root = calloc(1, sizeof *root);
    if (!root)
        return;
    root->type = JSON_OBJECT;
    for (int i = 0; i < COUNT(themes); i++)
    {
        if (themes[i] == ide->theme)
            json_set_string(root, "theme", theme_names[i]);
    }
    if (gui_font_count(ide->app) > 0)
        json_set_string(root, "editor_font", gui_font_name(ide->app, gui_get_font(ide->app)));
    if (gui_ui_font_count(ide->app) > 0)
    {
        json_set_string(root, "font", gui_ui_font_name(ide->app, gui_get_ui_font(ide->app)));
    }
    json_set_string(root, "editor_font_size", gui_get_editor_size(ide->app) < 0 ? "small" : gui_get_editor_size(ide->app) > 0 ? "larger" : "normal");

    playground_project_save(ide);   /* global_options: the playground project's */


    struct json_value* tools = json_set_array(root, "external_tools");
    for (int i = 0; i < ide->ext_tools.count; i++)
    {
        struct json_value* t = json_add_object(tools);
        if (!t)
            continue;
        json_set_string(t, "title", ide->ext_tools.tools[i].title);
        json_set_string(t, "command", ide->ext_tools.tools[i].command);
        json_set_string(t, "arguments", ide->ext_tools.tools[i].arguments);
        json_set_string(t, "directory", ide->ext_tools.tools[i].directory);
    }

    char path[1400] = { 0 };
    settings_path(path, sizeof path);
    if (!json_write_file(path, root))
    {
        char msg[1500] = { 0 };
        snprintf(msg, sizeof msg, "Cannot write %s", path);
        status(ide, msg);
    }
    json_delete(root);
}

static int slug_index(const char* slug, const char* const* slugs, int count)
{
    for (int i = 0; i < count; i++)
    {
        if (strcmp(slug, slugs[i]) == 0)
            return i;
    }
    return 0;
}

/* The tools installed with cake ($(InstallDir)/tools), used while ide.json has no "external_tools". */
static void add_default_tools(struct ide* ide)
{
#ifdef _WIN32
    const char* loc = "$(InstallDir)/tools/loc.exe";
#else
    const char* loc = "$(InstallDir)/tools/loc";
#endif
    if (ide->ext_tools.count < MAX_EXT_TOOLS)
        ext_tool_init(&ide->ext_tools.tools[ide->ext_tools.count++], "Lines of code", loc, "$(CakeInputFiles)", "$(ProjectDir)");
    tools_menu_refresh(ide);
}

/* Anything missing keeps its default. */
static void settings_load(struct ide* ide)
{
    char path[1400] = { 0 };
    settings_path(path, sizeof path);
    int crlf = 0;
    char* text = ide_read_file(path, &crlf);
    if (!text)
    {
        add_default_tools(ide);   /* no settings yet: the defaults */
        return;
    }
    struct json_error error = { 0 };
    struct json_value* root = json_parse(text, &error);
    free(text);
    if (!root || root->type != JSON_OBJECT)
    {
        char msg[1600] = { 0 };
        if (!root)
            snprintf(msg, sizeof msg, "The settings file is invalid - the defaults are used:\n%.1024s\n\nLine %d, column %d: %.400s",
                     path, (int)error.line, (int)error.column, error.message);
        else
            snprintf(msg, sizeof msg, "The settings file is invalid - the defaults are used:\n%s\n\nIt is not a JSON object.",
                     path);
        static const char* const labels[] = { "OK" };
        static const int ids[] = { 0 };
        gui_message_box(ide->app, "Settings", msg, labels, ids, 1);
        json_delete(root);
        return;
    }

    char name[64] = "";
    char font[64] = "";
    get_string(root, "editor_font", font, sizeof font);
    for (int i = 0; i < gui_font_count(ide->app); i++)
    {
        if (strcmp(font, gui_font_name(ide->app, i)) == 0)
            gui_set_font(ide->app, i);
    }
    char ui_font[64] = "";
    get_string(root, "font", ui_font, sizeof ui_font);
    for (int i = 0; i < gui_ui_font_count(ide->app); i++)
    {
        if (strcmp(ui_font, gui_ui_font_name(ide->app, i)) == 0)
        {
            gui_set_ui_font(ide->app, i);
        }
    }
    char ui_size[16] = "";
    get_string(root, "editor_font_size", ui_size, sizeof ui_size);
    gui_set_editor_size(ide->app, strcmp(ui_size, "small") == 0 ? -1 : strcmp(ui_size, "larger") == 0 ? 1 : 0);
    get_string(root, "theme", name, sizeof name);
    for (int i = 0; i < COUNT(themes); i++)
    {
        if (strcmp(name, theme_names[i]) == 0)
            apply_theme(ide, i);
    }

    const struct json_value* compile = json_find_member(root, "compile");
    compile_from_json(compile, &ide->global_options);

    const struct json_value* tools = json_find_member(root, "external_tools");
    if (!tools)
        add_default_tools(ide);
    size_t n = tools && tools->type == JSON_ARRAY ? json_count(tools) : 0;
    for (size_t i = 0; i < n && ide->ext_tools.count < MAX_EXT_TOOLS; i++)
    {
        const struct json_value* t = json_item(tools, i);
        ext_tool_init(&ide->ext_tools.tools[ide->ext_tools.count++], get_cstring(t, "title"),
                      get_cstring(t, "command"), get_cstring(t, "arguments"), get_cstring(t, "directory"));
    }
    json_delete(root);
    tools_menu_refresh(ide);
}

/* --- Session: %APPDATA%\cake_ide\session.json - the Folder panel's folder,
 * the open project, the font size, the current document (caret, window
 * rect), the Folder and Output docks. Saved on exit, restored at start;
 * as the old IDE's save_session/load_session. --- */

static void session_path(char* out, size_t cap)
{
    char dir[1024] = { 0 };
    if (!ide_config_dir(dir, sizeof dir))
    {
        out[0] = '\0';
        return;
    }
    join_path(out, cap, dir, "session.json");
}

static void save_dock(struct json_value* root, const char* key, struct gui_node* win)
{
    enum gui_dock side = gui_window_get_dock(win);
    if (side == GUI_DOCK_NONE)
        return;
    struct gui_rect r = gui_window_get_rect(win);
    struct json_value* dock = json_set_object(root, key);
    if (!dock)
        return;
    json_set_number(dock, "side", (int)side);
    json_set_number(dock, "size", side == GUI_DOCK_BOTTOM ? r.h : r.w);
}

static void session_save(struct ide* ide)
{
    char path[1400] = { 0 };
    session_path(path, sizeof path);
    if (!path[0])
        return;
    struct json_value* root = calloc(1, sizeof *root);
    if (!root)
        return;
    root->type = JSON_OBJECT;
    json_set_string(root, "folder_dir", ide->folder.dir);
    json_set_string(root, "project_path", ide->project.file_path);
    struct json_value* recent = json_set_array(root, "recent_projects");
    for (int i = 0; i < ide->recent_projects.count; i++)
        json_add_string(recent, ide->recent_projects.items[i]);
    json_set_number(root, "zoom", gui_get_zoom(ide->app));
    struct doc* d = active_doc(ide);
    if (d)
    {
        struct json_value* current = json_set_object(root, "current");
        int caret = 0, unused = 0;
        gui_editor_get_selection(d->editor, &caret, &unused);
        json_set_string(current, "file", d->path);
        json_set_number(current, "cursor", caret);
        struct gui_rect r = gui_window_get_rect(d->window);
        struct json_value* w = json_set_object(root, "main_window");
        json_set_number(w, "x", r.x);
        json_set_number(w, "y", r.y);
        json_set_number(w, "w", r.w);
        json_set_number(w, "h", r.h);
        json_set_bool(w, "maximized", gui_window_get_maximized(d->window));
    }
    side_capture(ide);
    if (ide->side.side != GUI_DOCK_NONE)
    {
        struct json_value* dock = json_set_object(root, "folder_dock");   /* the side place */
        json_set_number(dock, "side", (int)ide->side.side);
        json_set_number(dock, "size", ide->side.size);
        json_set_number(dock, "active", ide->side.active);
    }
    save_dock(root, "output_dock", ide->output.window);
    json_write_file(path, root);
    json_delete(root);
}

static void load_dock(const struct json_value* root, const char* key, struct gui_node* win)
{
    const struct json_value* dock = json_find_member(root, key);
    if (!dock)
        return;
    int side = get_int(dock, "side", 0);
    int size = get_int(dock, "size", 0);
    if (side > GUI_DOCK_NONE && side <= GUI_DOCK_BOTTOM && size > 0)
        gui_window_set_dock(win, (enum gui_dock)side, size);
}

/* 1 when it reopened a document - else the Playground opens. */
static int session_load(struct ide* ide)
{
    char path[1400] = { 0 };
    session_path(path, sizeof path);
    int crlf = 0;
    char* text = path[0] ? ide_read_file(path, &crlf) : NULL;
    if (!text)
        return 0;
    struct json_value* root = json_parse(text, NULL);
    free(text);
    if (!root || root->type != JSON_OBJECT)
    {
        json_delete(root);
        return 0;
    }
    char dir[1024] = "";
    get_string(root, "folder_dir", dir, sizeof dir);
    if (dir[0] && ide_is_dir(dir))
    {
        snprintf(ide->folder.dir, sizeof ide->folder.dir, "%s", dir);
        folder_refresh(ide);
    }
    int zoom = get_int(root, "zoom", 0);
    if (zoom)
        gui_zoom(ide->app, zoom);
    load_dock(root, "folder_dock", ide->folder.window);
    load_dock(root, "output_dock", ide->output.window);
    side_capture(ide);   /* the Folder panel, where the session put it */
    const struct json_value* side_dock = json_find_member(root, "folder_dock");
    int active = get_int(side_dock, "active", 0);
    char project[1024] = "";
    get_string(root, "project_path", project, sizeof project);
    const struct json_value* recent = json_find_member(root, "recent_projects");
    size_t recent_n = recent && recent->type == JSON_ARRAY ? json_count(recent) : 0;
    for (size_t i = 0; i < recent_n && ide->recent_projects.count < MAX_RECENT_PROJECTS; i++)
    {
        const struct json_value* r = json_item(recent, i);
        if (r && r->type == JSON_STRING && r->string)
            ide_strings_add(&ide->recent_projects, r->string);
    }
    if (project[0] && ide_file_exists(project))
        project_open(ide, project);
    if (active == 2 || (active == 0 && is_open(ide, ide->project_window)))
    {
        if (active == 2)
            git_refresh(ide);
        show_side_panel(ide, side_panel(ide, active));   /* the one shown last time */
    }

    int reopened = 0;
    const struct json_value* current = json_find_member(root, "current");
    char file[1024] = "";
    get_string(current, "file", file, sizeof file);
    if (file[0] && ide_file_exists(file))
    {
        open_file(ide, file);
        struct doc* d = find_doc(ide, file);
        if (d)
        {
            reopened = 1;
            const struct json_value* w = json_find_member(root, "main_window");
            if (w && !get_int(w, "maximized", 1))
            {
                struct gui_rect r = { get_int(w, "x", 0), get_int(w, "y", 0), get_int(w, "w", 0), get_int(w, "h", 0) };
                if (r.w > 0 && r.h > 0)
                    gui_window_set_rect(d->window, &r);
            }
            int len = (int)strlen(gui_get_value(d->editor));
            int caret = get_int(current, "cursor", 0);
            caret = caret < 0 ? 0 : caret > len ? len : caret;
            gui_editor_set_selection(d->editor, caret, caret);
        }
    }
    json_delete(root);
    return reopened;
}

/* --- Closing documents and exiting: the old IDE's unsaved-changes prompts --- */

static struct doc* doc_of_window(struct ide* ide, struct gui_node* win)
{
    for (int i = 0; i < ide->doc_count; i++)
    {
        if (ide->docs[i].window == win)
            return &ide->docs[i];
    }
    return NULL;
}

/* The document and its window are gone; opening the file again reads it. */
static void close_doc(struct ide* ide, struct doc* d)
{
    gui_window_free(ide->app, d->window);
    int i = (int)(d - ide->docs);
    memmove(&ide->docs[i], &ide->docs[i + 1], sizeof ide->docs[0] * (size_t)(ide->doc_count - i - 1));
    ide->doc_count--;
}

/* A document's close icon: with unsaved changes, ask first. */
static void doc_close_request(struct ide* ide)
{
    struct gui_node* win = gui_window_at(ide->app, gui_window_count(ide->app) - 1);
    struct doc* d = doc_of_window(ide, win);
    if (!d)
        return;
    if (!gui_editor_get_dirty(d->editor))
    {
        close_doc(ide, d);
        return;
    }
    ide->pending_close = win;
    char msg[400] = { 0 };
    snprintf(msg, sizeof msg, "%s has unsaved changes.\nDiscard them?", file_name(d->path));
    static const char* const labels[] = { "Discard", "Cancel" };
    static const int ids[] = { EV_CLOSE_DISCARD, 0 };
    gui_message_box(ide->app, "Close", msg, labels, ids, 2);
}

/* File > Exit (and the window's close button): asks about the first
 * modified document, or quits when there is none. */
static void exit_check(struct ide* ide)
{
    ide->exit_window = NULL;
    for (int i = 0; i < ide->doc_count; i++)
    {
        if (gui_editor_get_dirty(ide->docs[i].editor))
        {
            ide->exit_window = ide->docs[i].window;
            break;
        }
    }
    if (!ide->exit_window)
    {
        session_save(ide);
        gui_quit(ide->app);
        return;
    }
    struct doc* d = doc_of_window(ide, ide->exit_window);
    gui_window_open(ide->app, d->window);
    char msg[400] = { 0 };
    snprintf(msg, sizeof msg, "%s has unsaved changes.\nSave them?", file_name(d->path));
    static const char* const labels[] = { "Save", "Discard", "Cancel" };
    static const int ids[] = { EV_EXIT_SAVE, EV_EXIT_DISCARD, 0 };
    gui_message_box(ide->app, "Exit", msg, labels, ids, 3);
}

/* --- Events --- */

/* --- Git: the Git Changes panel, its diff, commit, branch and clone - the
 * old IDE's, running git synchronously in the repository's root --- */

/* `git <args>` in `dir`; echoed with its output to the Output panel unless
 * quiet. The output, malloc'ed ("" when git cannot run). */
static char* git_run(struct ide* ide, const char* dir, const char* args, int quiet)
{
    char cmd[2400] = { 0 };
    snprintf(cmd, sizeof cmd, "git %s", args);
    char* out = ide_run_capture(cmd, dir && dir[0] ? dir : NULL);
    if (!out)
    {
        out = malloc(1);
        if (out)
            out[0] = '\0';
    }
    if (!quiet)
    {
        char line[2500] = { 0 };
        snprintf(line, sizeof line, "> %s", cmd);
        output(ide, line);
        if (out && out[0])
        {
            char* o = out;
            for (const char* q = out; *q; q++)
            {
                if (*q != '\r')
                    *o++ = *q;
            }
            *o = '\0';
            size_t len = strlen(out);
            if (len > 0 && out[len - 1] == '\n')
                out[len - 1] = '\0';
            output(ide, out);
        }
    }
    return out;
}

static const char* git_dir(struct ide* ide)
{
    return ide->git.root[0] ? ide->git.root : ide->folder.dir;
}

static void git_refresh(struct ide* ide)
{
    struct git_panel* g = &ide->git;
    char* root = git_run(ide, ide->folder.dir, "rev-parse --show-toplevel", 1);
    snprintf(g->root, sizeof g->root, "%s", root && strncmp(root, "fatal", 5) != 0 ? root : "");
    g->root[strcspn(g->root, "\r\n")] = '\0';
    free(root);

    int selected = gui_get_selected(g->list);
    gui_clear_children(g->list);
    ide_strings_clear(&g->paths);
    ide_strings_clear(&g->xy);
    char* out = g->root[0] ? git_run(ide, g->root, "status --porcelain", 1) : NULL;
    for (const char* p = out ? out : ""; *p;)
    {
        const char* eol = strchr(p, '\n');
        size_t len = eol ? (size_t)(eol - p) : strlen(p);
        if (len >= 4)
        {
            char x = p[0], y = p[1];
            char name[1024] = { 0 };
            snprintf(name, sizeof name, "%.*s", (int)(len - 3), p + 3);
            name[strcspn(name, "\r")] = '\0';
            char* arrow = strstr(name, " -> ");   /* a rename: the new path is the file */
            const char* real = arrow ? arrow + 4 : name;
            char marker = ' ';
            uint32_t fg = 0;
            if (x == '?' && y == '?')
                marker = '?', fg = ide->theme->diag_info_fg;
            else if (x == 'A' || y == 'A')
                marker = 'A', fg = ide->theme->editor_number_fg;
            else if (x == 'D' || y == 'D')
                marker = 'D', fg = ide->theme->diag_error_fg;
            else if (x == 'M' || y == 'M')
                marker = 'M', fg = ide->theme->diag_warning_fg;
            const char* staged = "";
            if (x != ' ' && x != '?')
                staged = y != ' ' ? "  [partly staged]" : "  [staged]";
            char label[1200] = { 0 };
            snprintf(label, sizeof label, "%c %s%s", marker, name, staged);
            struct gui_node* item = create(ide, GUI_ITEM, label);
            if (fg)
                gui_set_colors(item, fg, 0);
            gui_append(g->list, item);
            char xy[3] = { x, y, '\0' };
            ide_strings_add(&g->paths, real);
            ide_strings_add(&g->xy, xy);
        }
        p += len + (eol ? 1 : 0);
    }
    free(out);
    int staged = 0;
    for (int i = 0; i < g->xy.count; i++)
        staged |= g->xy.items[i][0] != ' ' && g->xy.items[i][0] != '?';
    for (int i = 0; i < 2; i++)
        gui_set_enabled(g->staged_items[i], staged);
    if (g->paths.count > 0)
        gui_set_selected(g->list, selected >= 0 && selected < g->paths.count ? selected : 0);
    char title[80] = { 0 };
    if (g->root[0])
        snprintf(title, sizeof title, "Git Changes (%d)", g->paths.count);
    else
        snprintf(title, sizeof title, "Git Changes (no repository)");
    gui_set_label(g->window, title);
}

static void git_show_panel(struct ide* ide)
{
    git_refresh(ide);
    show_side_panel(ide, ide->git.window);
}

/* The selected row's path, from the root; NULL when none. */
static const char* git_selected(struct ide* ide, char* xy)
{
    struct git_panel* g = &ide->git;
    int row = gui_get_selected(g->list);
    if (row < 0 || row >= g->paths.count)
        return NULL;
    if (xy)
        memcpy(xy, g->xy.items[row], 3);
    return g->paths.items[row];
}

/* C colors after a diff row's "+", "-" or " " - the old IDE's UI_SYNTAX_DIFF. */
static int highlight_diff(void* ctx, const char* line, int len, int* state, struct gui_span* spans, int max)
{
    struct ide* ide = ctx;
    if (!ide->git.diff_prefixed)
        return ide_highlight_c((void*)ide->theme, line, len, state, spans, max);
    if (len <= 0)
        return 0;
    if ((line[0] == '+' || line[0] == '-') && len >= 3 && line[1] == line[0] && line[2] == line[0])
        return 0;   /* the "+++" / "---" header */
    int n = ide_highlight_c((void*)ide->theme, line + 1, len - 1, state, spans, max);
    for (int i = 0; i < n; i++)
        spans[i].start++;
    return n;
}

static int diff_row_changed(const char* s, int len)
{
    if (len <= 0 || (s[0] != '+' && s[0] != '-'))
        return 0;
    return !(len >= 3 && s[1] == s[0] && s[2] == s[0]);
}

/* Where each run of changed rows starts, for Previous / Next. */
static void git_diff_index(struct ide* ide)
{
    struct git_panel* g = &ide->git;
    free(g->runs);
    g->runs = NULL;
    g->run_count = 0;
    const char* text = gui_get_value(g->diff_editor);
    if (!g->diff_prefixed || !text[0])
        return;
    int lines = 1;
    for (const char* p = text; *p; p++)
        lines += *p == '\n';
    g->runs = malloc(sizeof(int) * (size_t)lines);
    if (!g->runs)
        return;
    int prev = 0, line = 1;
    for (const char* p = text;;)
    {
        const char* nl = strchr(p, '\n');
        int len = nl ? (int)(nl - p) : (int)strlen(p);
        int changed = diff_row_changed(p, len);
        if (changed && !prev)
            g->runs[g->run_count++] = line;
        prev = changed;
        if (!nl)
            break;
        p = nl + 1;
        line++;
    }
}

/* "2/23": the change the caret is on, of how many. */
static void git_diff_counter_refresh(struct ide* ide)
{
    struct git_panel* g = &ide->git;
    int cur = 0, col = 0;
    gui_editor_get_caret(g->diff_editor, &cur, &col);
    int index = 0;
    while (index < g->run_count && g->runs[index] <= cur)
        index++;
    char label[32] = { 0 };
    if (g->run_count == 0)
        snprintf(label, sizeof label, " no changes");
    else if (index == 0)
        snprintf(label, sizeof label, "  -/%d", g->run_count);
    else
        snprintf(label, sizeof label, "%3d/%d", index, g->run_count);
    gui_set_label(g->diff_counter, label);
}

static void git_diff_goto_change(struct ide* ide, int dir)
{
    struct git_panel* g = &ide->git;
    int cur = 0, col = 0;
    gui_editor_get_caret(g->diff_editor, &cur, &col);
    int target = -1;
    if (dir > 0)
    {
        for (int i = 0; i < g->run_count && target < 0; i++)
            if (g->runs[i] > cur)
                target = g->runs[i];
        if (target < 0 && g->run_count > 0)
            target = g->runs[0];   /* wraps to the first */
    }
    else
    {
        for (int i = g->run_count - 1; i >= 0 && target < 0; i--)
            if (g->runs[i] < cur)
                target = g->runs[i];
        if (target < 0 && g->run_count > 0)
            target = g->runs[g->run_count - 1];   /* wraps to the last */
    }
    if (target > 0)
        gui_editor_goto_line_center(g->diff_editor, target);
    gui_focus(ide->app, g->diff_editor);
    git_diff_counter_refresh(ide);
}

/* A double click on a row: the file's diff - working tree, else staged,
 * else (untracked) the file itself - the whole file (-U100000) in the diff
 * window, maximized, at the first change. */
static void git_show_diff(struct ide* ide)
{
    struct git_panel* g = &ide->git;
    const char* path = git_selected(ide, NULL);
    if (!path)
        return;
    char args[1200] = { 0 };
    snprintf(args, sizeof args, "diff -U100000 -- \"%s\"", path);
    char* out = git_run(ide, git_dir(ide), args, 1);
    if (!out || !out[0])
    {
        free(out);
        snprintf(args, sizeof args, "diff -U100000 --cached -- \"%s\"", path);
        out = git_run(ide, git_dir(ide), args, 1);
    }
    join_path(g->diff_path, sizeof g->diff_path, git_dir(ide), path);
    if (!out || !out[0])
    {
        free(out);
        int crlf = 0;
        out = ide_read_file(g->diff_path, &crlf);
        if (!out)
        {
            out = malloc(32);
            if (!out)
                return;
            snprintf(out, 32, "(no changes to display)\n");
        }
    }
    /* "\r" out, the trailing newlines too */
    char* o = out;
    for (const char* q = out; *q; q++)
    {
        if (*q != '\r')
            *o++ = *q;
    }
    *o = '\0';
    size_t len = strlen(out);
    while (len > 0 && out[len - 1] == '\n')
        out[--len] = '\0';
    /* a real diff: its header up to the first "@@" line goes */
    const char* text = out;
    g->diff_prefixed = strncmp(out, "diff --git ", 11) == 0 || strstr(out, "\ndiff --git ") != NULL;
    if (g->diff_prefixed)
    {
        const char* at = strstr(out, "\n@@");
        const char* eol = at ? strchr(at + 1, '\n') : NULL;
        if (eol)
            text = eol + 1;
    }
    char title[300] = { 0 };
    snprintf(title, sizeof title, "Diff: %s", file_name(path));
    gui_set_label(g->diff_window, title);
    gui_set_value(g->diff_editor, text);
    free(out);
    git_diff_index(ide);
    gui_window_open(ide->app, g->diff_window);
    gui_window_maximize(ide->app, g->diff_window);
    gui_editor_goto_line(g->diff_editor, g->run_count > 0 ? g->runs[0] : 1);
    gui_focus(ide->app, g->diff_editor);
    git_diff_counter_refresh(ide);
}

/* Edit: the file itself, at the caret's line - for a "+" or " " row the
 * new file's line, the rows "-" skipped. */
static void git_diff_edit(struct ide* ide)
{
    struct git_panel* g = &ide->git;
    if (!g->diff_path[0])
        return;
    int caret_line = 0, col = 0;
    gui_editor_get_caret(g->diff_editor, &caret_line, &col);
    int file_line = caret_line;
    if (g->diff_prefixed)
    {
        file_line = 1;
        const char* p = gui_get_value(g->diff_editor);
        for (int line = 1; line < caret_line && *p; line++)
        {
            if (*p != '-')
                file_line++;
            while (*p && *p != '\n')
                p++;
            if (*p)
                p++;
        }
    }
    nav_record_jump(ide);
    open_file(ide, g->diff_path);
    struct doc* d = find_doc(ide, g->diff_path);
    if (d)
    {
        gui_editor_goto_line_center(d->editor, file_line);
        gui_focus(ide->app, d->editor);
    }
}

static void build_git_diff(struct ide* ide)
{
    struct git_panel* g = &ide->git;
    g->diff_window = create(ide, GUI_WINDOW, "Diff");
    struct gui_rect r = { 0, 0, 76 * 8, 22 * 16 };
    gui_window_set_rect(g->diff_window, &r);
    gui_set_id(add_at(ide, g->diff_window, GUI_BUTTON, 1, 1, 14, 1, "Previous \xE2\x86\x91"), EV_GITDIFF_PREV);   /* U+2191 */
    gui_set_id(add_at(ide, g->diff_window, GUI_BUTTON, 16, 1, 10, 1, "\xE2\x86\x93 Next"), EV_GITDIFF_NEXT);      /* U+2193 */
    g->diff_counter = add_at(ide, g->diff_window, GUI_TEXT, 28, 1, 14, 1, "");
    /* a blank row, then the diff to the window's edges */
    g->diff_editor = create(ide, GUI_EDITOR, NULL);
    struct gui_layout l = { GUI_ANCHOR_LEFT | GUI_ANCHOR_TOP | GUI_ANCHOR_RIGHT | GUI_ANCHOR_BOTTOM };
    l.left.cells = l.right.cells = l.bottom.cells = 1;
    l.top.cells = 3;
    gui_set_layout(g->diff_editor, &l);
    gui_editor_set_read_only(g->diff_editor, 1);
    gui_editor_set_line_numbers(g->diff_editor, 1);
    gui_editor_set_diff(g->diff_editor, 1);
    g->diff_highlighter.highlight = highlight_diff;
    g->diff_highlighter.ctx = ide;
    gui_editor_set_highlighter(g->diff_editor, &g->diff_highlighter);
    struct gui_node* menu = create(ide, GUI_MENU, NULL);
    add_popup_item(ide, menu, EV_GITDIFF_EDIT, "Edit", NULL);
    add_popup_item(ide, menu, EV_GITDIFF_COPY_PATH, "Copy Full Path", NULL);
    add_popup_item(ide, menu, EV_GITDIFF_SHOW_FOLDER, "Show My Folder", NULL);
    gui_set_context_menu(g->diff_editor, menu);
    gui_append(g->diff_window, g->diff_editor);
}

static void git_on_selected(struct ide* ide, const char* verb, const char* title)
{
    const char* path = git_selected(ide, NULL);
    if (!path || run_busy(ide))
        return;
    char cmd[1200] = { 0 };
    snprintf(cmd, sizeof cmd, "git %s -- \"%s\"", verb, path);
    const char* steps[] = { cmd };
    run_start(ide, RUN_GIT_QUIET, title, git_dir(ide), steps, 1);
}

static void git_ask(struct ide* ide, const char* text, int ok_id)
{
    static const char* const labels[] = { "OK", "Cancel" };
    int ids[] = { ok_id, 0 };
    gui_message_box(ide->app, "Git", text, labels, ids, 2);
}

static void git_discard_ask(struct ide* ide)
{
    const char* path = git_selected(ide, NULL);
    if (!path)
        return;
    char msg[1300] = { 0 };
    snprintf(msg, sizeof msg, "Discard the changes to\n%s?\n\nThis cannot be undone.", path);
    git_ask(ide, msg, EV_GIT_DISCARD_OK);
}

static void git_discard(struct ide* ide)
{
    char xy[3] = { 0 };
    const char* path = git_selected(ide, xy);
    if (!path)
        return;
    char args[1200] = { 0 };
    if (xy[0] == '?')
        snprintf(args, sizeof args, "clean -f -- \"%s\"", path);   /* untracked: delete it */
    else
        snprintf(args, sizeof args, "checkout HEAD -- \"%s\"", path);
    free(git_run(ide, git_dir(ide), args, 0));
    git_refresh(ide);
}

static void git_discard_all(struct ide* ide)
{
    free(git_run(ide, git_dir(ide), "reset --hard HEAD", 0));
    free(git_run(ide, git_dir(ide), "clean -fd", 0));
    git_refresh(ide);
}

/* Add to Ignore List: one line appended to the repository's .gitignore -
 * the selected path, "*.ext" or its folder ("dir/"). */
static void git_ignore(struct ide* ide, int id)
{
    const char* path = git_selected(ide, NULL);
    if (!path)
        return;
    char line[1100] = { 0 };
    if (id == EV_GIT_IGNORE_FILE)
    {
        snprintf(line, sizeof line, "/%s", path);
    }
    else if (id == EV_GIT_IGNORE_EXT)
    {
        const char* name = file_name(path);
        const char* dot = strrchr(name, '.');
        if (!dot || dot == name)
        {
            status(ide, "Ignore: the file has no extension");
            return;
        }
        snprintf(line, sizeof line, "*%s", dot);
    }
    else
    {
        snprintf(line, sizeof line, "/%s", path);
        size_t n = strlen(line);
        if (n > 0 && line[n - 1] == '/')
            line[--n] = '\0';   /* an untracked folder comes as "dir/" */
        char* slash = strrchr(line, '/');
        if (slash == line)
        {
            status(ide, "Ignore: the item is at the repository root");
            return;
        }
        slash[1] = '\0';
    }
    char gitignore[1100] = { 0 };
    join_path(gitignore, sizeof gitignore, git_dir(ide), ".gitignore");
    int crlf = 0;
    char* old = ide_read_file(gitignore, &crlf);
    size_t old_len = old ? strlen(old) : 0;
    size_t size = old_len + strlen(line) + 3;
    char* text = malloc(size);
    if (!text)
    {
        free(old);
        return;
    }
    snprintf(text, size, "%s%s%s\n", old ? old : "",
             old_len > 0 && old[old_len - 1] != '\n' ? "\n" : "", line);
    char msg[1300] = { 0 };
    if (ide_write_file(gitignore, text, crlf) == 0)
        snprintf(msg, sizeof msg, "Added %s to .gitignore", line);
    else
        snprintf(msg, sizeof msg, "Cannot write %s", gitignore);
    status(ide, msg);
    free(text);
    free(old);
    git_refresh(ide);
}

static void git_commit_open(struct ide* ide, int mode, int push)
{
    struct git_panel* g = &ide->git;
    g->commit_mode = mode;
    g->commit_push = push;
    g->commit_file[0] = '\0';
    if (mode == GIT_COMMIT_FILE)
    {
        const char* path = git_selected(ide, NULL);
        if (!path)
            return;
        snprintf(g->commit_file, sizeof g->commit_file, "%s", path);
    }
    gui_set_value(g->message, "");
    show_dialog(ide, g->commit_window, 60, 8, g->message);
}

static void git_commit(struct ide* ide)
{
    struct git_panel* g = &ide->git;
    char message[1000] = { 0 };
    snprintf(message, sizeof message, "%s", gui_get_value(g->message));
    for (char* c = message; *c; c++)
    {
        if (*c == '"')
            *c = '\'';
    }
    if (!message[0])
    {
        status(ide, "Commit: the message is empty");
        return;
    }
    if (run_busy(ide))
        return;
    gui_window_close(ide->app, g->commit_window);
    gui_set_value(ide->output.editor, "");
    static char steps[4][2400];
    const char* list[4] = { 0 };
    int n = 0;
    if (g->commit_mode == GIT_COMMIT_ALL)
        snprintf(steps[n++], sizeof steps[0], "git add -A");
    if (g->commit_mode == GIT_COMMIT_FILE)
    {
        snprintf(steps[n++], sizeof steps[0], "git add -- \"%s\"", g->commit_file);
        snprintf(steps[n++], sizeof steps[0], "git commit -m \"%s\" -- \"%s\"", message, g->commit_file);
    }
    else
    {
        snprintf(steps[n++], sizeof steps[0], "git commit -m \"%s\"", message);
    }
    if (g->commit_push)
        snprintf(steps[n++], sizeof steps[0], "git push");
    for (int i = 0; i < n; i++)
        list[i] = steps[i];
    run_start(ide, RUN_GIT, g->commit_push ? "Commit && Push" : "Commit", git_dir(ide), list, n);
}

static void git_remote(struct ide* ide, int id)
{
    if (run_busy(ide))
        return;
    gui_set_value(ide->output.editor, "");
    static const char* const pull[] = { "git pull" };
    static const char* const push[] = { "git push" };
    static const char* const sync[] = { "git pull", "git push" };
    if (id == EV_GIT_PULL)
        run_start(ide, RUN_GIT, "Pull", git_dir(ide), pull, 1);
    else if (id == EV_GIT_PUSH)
        run_start(ide, RUN_GIT, "Push", git_dir(ide), push, 1);
    else
        run_start(ide, RUN_GIT, "Sync", git_dir(ide), sync, 2);
}

static void git_branch_open(struct ide* ide)
{
    struct git_panel* g = &ide->git;
    gui_clear_children(g->branches);
    char* out = git_run(ide, git_dir(ide), "branch", 1);
    int current = 0, count = 0;
    for (const char* p = out ? out : ""; *p;)
    {
        const char* eol = strchr(p, '\n');
        size_t len = eol ? (size_t)(eol - p) : strlen(p);
        if (len > 2)
        {
            char name[300] = { 0 };
            snprintf(name, sizeof name, "%.*s", (int)len, p);
            name[strcspn(name, "\r")] = '\0';
            if (name[0] == '*')
                current = count;
            gui_append(g->branches, create(ide, GUI_ITEM, name));
            count++;
        }
        p += len + (eol ? 1 : 0);
    }
    free(out);
    if (count > 0)
        gui_set_selected(g->branches, current);
    gui_set_value(g->new_branch, "");
    show_dialog(ide, g->branch_window, 50, 18, g->branches);
}

static void git_branch_run(struct ide* ide, int create_new)
{
    struct git_panel* g = &ide->git;
    char name[300] = { 0 };
    if (create_new)
    {
        snprintf(name, sizeof name, "%s", gui_get_value(g->new_branch));
    }
    else
    {
        int row = gui_get_selected(g->branches);
        struct gui_node* it = row >= 0 && row < gui_child_count(g->branches) ? gui_child_at(g->branches, row) : NULL;
        if (it)
            snprintf(name, sizeof name, "%s", gui_get_label(it) + 2);   /* past "* " / "  " */
    }
    if (!name[0] || strchr(name, '"'))
        return;
    if (run_busy(ide))
        return;
    gui_window_close(ide->app, g->branch_window);
    char cmd[400] = { 0 };
    snprintf(cmd, sizeof cmd, create_new ? "git checkout -b \"%s\"" : "git checkout \"%s\"", name);
    gui_set_value(ide->output.editor, "");
    const char* steps[] = { cmd };
    run_start(ide, RUN_GIT, "Branch", git_dir(ide), steps, 1);
}

/* Git Clone's Path, unless typed by the user: the parent folder plus the
 * URL's last part without ".git" - the folder a plain `git clone` makes. */
static void git_clone_fill_path(struct ide* ide)
{
    struct git_clone_dialog* c = &ide->clone;
    if (c->path_edited)
        return;
    const char* url = gui_get_value(c->url);
    char name[1024] = { 0 };
    const char* slash = strrchr(url, '/');
    snprintf(name, sizeof name, "%s", slash ? slash + 1 : url);
    size_t n = strlen(name);
    if (n > 4 && strcmp(name + n - 4, ".git") == 0)
        name[n - 4] = '\0';
    char dest[1024] = { 0 };
    if (name[0])
        join_path(dest, sizeof dest, c->parent, name);
    else
        snprintf(dest, sizeof dest, "%s", c->parent);
    gui_set_value(c->path, dest);
}

/* Git Clone's OK: `git clone <url> <path>` from Path's parent, then the
 * Folder panel there. */
static void git_clone(struct ide* ide)
{
    struct git_clone_dialog* c = &ide->clone;
    char url[1024] = { 0 }, dest[1024] = { 0 };
    snprintf(url, sizeof url, "%s", gui_get_value(c->url));
    snprintf(dest, sizeof dest, "%s", gui_get_value(c->path));
    if (!url[0] || !dest[0] || strchr(url, '"') || strchr(dest, '"'))
    {
        status(ide, "Clone: enter the repository URL and path");
        return;
    }
    if (run_busy(ide))
        return;
    char parent[1024] = { 0 };
    snprintf(parent, sizeof parent, "%s", dest);
    parent_dir(parent);
    if (!ide_is_dir(parent))
    {
        char text[1200] = { 0 };
        snprintf(text, sizeof text, "The location does not exist:\n%s", parent);
        static const char* const ok[] = { "OK" };
        static const int ok_id[] = { 0 };
        gui_message_box(ide->app, "Clone Repository", text, ok, ok_id, 1);
        return;
    }
    if (ide_is_dir(dest))
    {
        char text[1200] = { 0 };
        snprintf(text, sizeof text, "The folder already exists:\n%s", dest);
        static const char* const ok[] = { "OK" };
        static const int ok_id[] = { 0 };
        gui_message_box(ide->app, "Clone Repository", text, ok, ok_id, 1);
        return;
    }
    gui_window_close(ide->app, c->window);
    gui_set_value(ide->output.editor, "");
    snprintf(ide->run.clone_dest, sizeof ide->run.clone_dest, "%s", dest);
    ide->run.clone_open = gui_get_checked(c->open_folder, 0);
    char cmd[2200] = { 0 };
    snprintf(cmd, sizeof cmd, "git clone \"%s\" \"%s\"", url, dest);
    const char* steps[] = { cmd };
    run_start(ide, RUN_CLONE, "Clone", parent, steps, 1);
}

static void build_git(struct ide* ide)
{
    struct git_panel* g = &ide->git;
    build_git_diff(ide);
    g->commit_window = new_dialog(ide, "Commit");
    add_label(ide, g->commit_window, 2, 2, "Message");
    g->message = add_at(ide, g->commit_window, GUI_INPUT, 11, 2, 45, 1, NULL);
    gui_set_id(g->message, EV_GITCOMMIT_OK);
    gui_set_id(add_at(ide, g->commit_window, GUI_BUTTON, 18, 5, 10, 1, "OK"), EV_GITCOMMIT_OK);
    gui_set_id(add_at(ide, g->commit_window, GUI_BUTTON, 30, 5, 10, 1, "Cancel"), EV_GITCOMMIT_CANCEL);

    g->branch_window = new_dialog(ide, "Branch");
    g->branches = add_at(ide, g->branch_window, GUI_LISTBOX, 2, 2, 44, 9, NULL);
    gui_set_id(g->branches, EV_GITBRANCH_CHECKOUT);
    add_label(ide, g->branch_window, 2, 12, "New");
    g->new_branch = add_at(ide, g->branch_window, GUI_INPUT, 7, 12, 39, 1, NULL);
    gui_set_id(g->new_branch, EV_GITBRANCH_NEW);
    gui_set_id(add_at(ide, g->branch_window, GUI_BUTTON, 4, 15, 12, 1, "Checkout"), EV_GITBRANCH_CHECKOUT);
    gui_set_id(add_at(ide, g->branch_window, GUI_BUTTON, 18, 15, 12, 1, "Create"), EV_GITBRANCH_NEW);
    gui_set_id(add_at(ide, g->branch_window, GUI_BUTTON, 32, 15, 12, 1, "Cancel"), EV_GITBRANCH_CANCEL);
}

static void git_event(struct ide* ide, int id)
{
    switch (id)
    {
    case EV_GIT_LIST: git_show_diff(ide); break;
    case EV_GIT_REFRESH: git_refresh(ide); break;
    case EV_GIT_STAGE: git_on_selected(ide, "add", "Stage"); break;
    case EV_GIT_UNSTAGE: git_on_selected(ide, "reset -q HEAD", "Unstage"); break;
    case EV_GIT_DISCARD: git_discard_ask(ide); break;
    case EV_GIT_DISCARD_OK: git_discard(ide); break;
    case EV_GIT_DISCARD_ALL:
        git_ask(ide, "Discard every change, untracked files included?\n\nThis cannot be undone.", EV_GIT_DISCARD_ALL_OK);
        break;
    case EV_GIT_DISCARD_ALL_OK: git_discard_all(ide); break;
    case EV_GIT_IGNORE_FILE:
    case EV_GIT_IGNORE_EXT:
    case EV_GIT_IGNORE_FOLDER: git_ignore(ide, id); break;
    case EV_GIT_COMMIT: git_commit_open(ide, GIT_COMMIT_ALL, 0); break;
    case EV_GIT_COMMIT_PUSH: git_commit_open(ide, GIT_COMMIT_ALL, 1); break;
    case EV_GIT_COMMIT_STAGED: git_commit_open(ide, GIT_COMMIT_STAGED, 0); break;
    case EV_GIT_COMMIT_STAGED_PUSH: git_commit_open(ide, GIT_COMMIT_STAGED, 1); break;
    case EV_GIT_COMMIT_FILE: git_commit_open(ide, GIT_COMMIT_FILE, 0); break;
    case EV_GITCOMMIT_OK: git_commit(ide); break;
    case EV_GITCOMMIT_CANCEL: gui_window_close(ide->app, ide->git.commit_window); break;
    case EV_GIT_PULL:
    case EV_GIT_PUSH:
    case EV_GIT_SYNC: git_remote(ide, id); break;
    case EV_GIT_BRANCH: git_branch_open(ide); break;
    case EV_GITBRANCH_CHECKOUT: git_branch_run(ide, 0); break;
    case EV_GITBRANCH_NEW: git_branch_run(ide, 1); break;
    case EV_GITBRANCH_CANCEL: gui_window_close(ide->app, ide->git.branch_window); break;
    case EV_GITDIFF_PREV: git_diff_goto_change(ide, -1); break;
    case EV_GITDIFF_NEXT: git_diff_goto_change(ide, 1); break;
    case EV_GITDIFF_EDIT: git_diff_edit(ide); break;
    case EV_GITDIFF_COPY_PATH: gui_set_clipboard(ide->app, ide->git.diff_path); break;
    case EV_GITDIFF_SHOW_FOLDER:
        if (ide->git.diff_path[0])
            show_folder_of(ide, ide->git.diff_path);
        break;
    default: break;
    }
}

/* --- Debugging: ide_debugger.c's lldb (cdb on Windows) session, polled by the
 * 50 ms tick while it lasts --- */

static void debug_output(void* ctx, const char* line, size_t len)
{
    struct ide* ide = ctx;
    char buf[2048] = { 0 };
    while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
        len--;
    snprintf(buf, sizeof buf, "%.*s", (int)len, line);
    output(ide, buf);
}

static struct doc* doc_for_debug_file(struct ide* ide, const char* file)
{
    const char* base = file_name(file);
    for (int i = 0; i < ide->doc_count; i++)
    {
        if (ide_path_equal(ide->docs[i].path, file))
            return &ide->docs[i];
    }
    for (int i = 0; i < ide->doc_count; i++)
    {
        if (ide_path_equal(file_name(ide->docs[i].path), base))
            return &ide->docs[i];
    }
    return NULL;
}

static void debug_clear_exec_lines(struct ide* ide)
{
    for (int i = 0; i < ide->doc_count; i++)
        gui_editor_set_exec_line(ide->docs[i].editor, 0);
    ide->session_file[0] = '\0';
    ide->session_line = 0;
}

static void debug_sync_exec_line(struct ide* ide)
{
    struct debug_session* s = &ide->session;
    if (s->state != DBG_STOPPED || s->cur_line <= 0)
    {
        if (ide->session_line)
            debug_clear_exec_lines(ide);
        return;
    }
    if (ide->session_line == s->cur_line && strcmp(ide->session_file, s->cur_file) == 0)
        return;
    debug_clear_exec_lines(ide);
    struct doc* d = doc_for_debug_file(ide, s->cur_file);
    if (!d && ide_file_time(s->cur_file) != 0)
    {
        open_file(ide, s->cur_file);
        d = doc_for_debug_file(ide, s->cur_file);
    }
    if (d)
    {
        gui_window_open(ide->app, d->window);
        gui_editor_set_exec_line(d->editor, s->cur_line);
        gui_editor_goto_line_center(d->editor, s->cur_line);
    }
    snprintf(ide->session_file, sizeof ide->session_file, "%s", s->cur_file);
    ide->session_line = s->cur_line;
}

/* `text` split at blanks, "quoted" words kept whole, into argv (NULL-ended). */
static int split_args(char* text, const char* argv[], int max)
{
    int n = 0;
    char* p = text;
    while (*p && n < max - 1)
    {
        while (*p == ' ' || *p == '\t')
            p++;
        if (!*p)
            break;
        char* out = p;
        argv[n++] = p;
        int quoted = 0;
        while (*p && (quoted || (*p != ' ' && *p != '\t')))
        {
            if (*p == '"')
                quoted = !quoted;
            else
                *out++ = *p;
            p++;
        }
        if (*p)
            p++;
        *out = '\0';
    }
    argv[n] = NULL;
    return n;
}

static void debug_sync_breakpoints(struct ide* ide);

static void debug_start_session(struct ide* ide)
{
    struct debug_session* s = &ide->session;
    if (s->state == DBG_STOPPED)
    {
        debug_continue(s);
        return;
    }
    if (s->state != DBG_IDLE)
        return;
    struct doc* doc = active_doc(ide);
    if (!doc || !doc->path[0])
    {
        status(ide, "Start Debugging: open a source file first");
        return;
    }
    struct ide_project* p = &ide->project;
    int use_project = file_uses_project(ide, doc->path);
    const struct compiler_settings* cs = use_project ? &p->compile : &ide->global_options;
    if (!settings_in_use(cs)->flags[0])   /* -line-directives */
    {
        static const char* const ok[] = { "OK" };
        static const int ok_id[] = { 0 };
        gui_message_box(ide->app, "Start Debugging",
                        use_project ?
                        "Line directives are off, so breakpoints in this source cannot be found.\n\n"
                        "Check \"-line-directives\" in Project > Properties..., build again and start debugging.\n" :
                        "Line directives are off, so breakpoints in this source cannot be found.\n\n"
                        "Check \"-line-directives\" in Project > Properties... (Playground), build again and start debugging.\n",
                        ok, ok_id, 1);
        return;
    }
    /* Built first, as Visual Studio; debugging starts when the build ends without errors. */
    build_then(ide, 0, 0, 1);
}

/* The debugger started on the built executable. */
static void debug_launch(struct ide* ide)
{
    struct debug_session* s = &ide->session;
    struct doc* doc = active_doc(ide);
    if (s->state != DBG_IDLE || !doc)
        return;
    struct ide_project* p = &ide->project;
    int use_project = file_uses_project(ide, doc->path);
    if (gui_editor_get_dirty(doc->editor))
        save_doc(ide, doc);

    /* Command, Arguments, Directory: the target in use's */
    const struct compiler_settings* cs = use_project ? &p->compile : &ide->global_options;
    const char* fields[3] = { 0 };
    for (int i = 0; i < 3; i++)
        fields[i] = settings_in_use(cs)->debug[i];
    struct ide_text exe = { 0 }, args = { 0 }, dir = { 0 };
    expand_macros(ide, fields[0][0] ? fields[0] : "$(TargetPath)", &exe, 0);
    expand_macros(ide, fields[1], &args, 1);
    expand_macros(ide, fields[2], &dir, 0);
    const char* exe_args[64] = { 0 };
    split_args(args.data ? args.data : (char*)"", exe_args, 64);

    bottom_panel_show(ide, ide->output.window, ide->fr.window);   /* the build's output kept above */
    char header[1400] = { 0 };
#ifdef _WIN32
    snprintf(header, sizeof header, "> cdb -lines \"%s\"", exe.data ? exe.data : "");
#else
    snprintf(header, sizeof header, "> lldb -- \"%s\"", exe.data ? exe.data : "");
#endif
    output(ide, header);

    debug_init(s);
    s->on_output = debug_output;
    s->on_output_ctx = ide;
    char err[256] = { 0 };
    const char* exe_path = exe.data ? exe.data : "";
    if (ide_file_time(exe_path) == 0)
    {
        char msg[1400] = { 0 };
        snprintf(msg, sizeof msg, "No built executable at '%s' - build first.", exe_path);
        output(ide, msg);
        status(ide, "Nothing to debug");
    }
    else if (!debug_start(s, exe_path, exe_args, dir.data && dir.data[0] ? dir.data : NULL, err, sizeof err))
    {
        char msg[600] = { 0 };
#ifdef _WIN32
        snprintf(msg, sizeof msg, "Could not start cdb: %s\nInstall it with: winget install Microsoft.WinDbg", err);
#else
        snprintf(msg, sizeof msg, "Could not start lldb: %s\nIs lldb installed and on PATH?", err);
#endif
        output(ide, msg);
        debug_init(s);
    }
    else
    {
        ide->sent_bp_count = 0;
        debug_sync_breakpoints(ide);
        debug_run(s);
        ide->session_was_stopped = 0;
        gui_set_timer(ide->app, 50, EV_TICK);
        debug_menu_refresh(ide);
        status(ide, "Debugging");
    }
    free(exe.data);
    free(args.data);
    free(dir.data);
}

/* The debugger's breakpoints made the open C files' - at the start and on
 * every tick, so a breakpoint toggled during the session (F9, a click on
 * the line number) takes effect at once. By file name: the line directives
 * map them through Cake's output. */
static void debug_sync_breakpoints(struct ide* ide)
{
    struct debug_session* s = &ide->session;
    if (s->query_state != DQS_IDLE)
        return;   /* next tick: its echoes would mix with the query's */
    int changed = 0;
    /* gone: cleared */
    for (int i = 0; i < ide->sent_bp_count;)
    {
        struct sent_breakpoint* b = &ide->sent_bps[i];
        int keep = 0;
        for (int d = 0; d < ide->doc_count && !keep; d++)
        {
            int lines[256] = { 0 };
            if (strcmp(file_name(ide->docs[d].path), b->file) != 0)
                continue;
            int n = gui_editor_get_breakpoints(ide->docs[d].editor, lines, 256);
            for (int k = 0; k < n && !keep; k++)
                keep = lines[k] == b->line;
        }
        if (keep)
        {
            i++;
            continue;
        }
        debug_break_delete(s, b->file, b->line, b->id);
        ide->sent_bps[i] = ide->sent_bps[--ide->sent_bp_count];
        changed = 1;
    }
    /* new: set */
    for (int d = 0; d < ide->doc_count; d++)
    {
        int lines[256] = { 0 };
        const char* name = file_name(ide->docs[d].path);
        int n = gui_editor_get_breakpoints(ide->docs[d].editor, lines, 256);
        for (int k = 0; k < n; k++)
        {
            int known = 0;
            for (int i = 0; i < ide->sent_bp_count && !known; i++)
                known = ide->sent_bps[i].line == lines[k] && strcmp(ide->sent_bps[i].file, name) == 0;
            if (known)
                continue;
            if (ide->sent_bp_count == ide->sent_bp_cap)
            {
                int cap = ide->sent_bp_cap ? ide->sent_bp_cap * 2 : 32;
                void* p = realloc(ide->sent_bps, (size_t)cap * sizeof *ide->sent_bps);
                if (!p)
                    return;
                ide->sent_bps = p;
                ide->sent_bp_cap = cap;
            }
            struct sent_breakpoint* b = &ide->sent_bps[ide->sent_bp_count++];
            snprintf(b->file, sizeof b->file, "%s", name);
            b->line = lines[k];
            b->id = ++ide->next_bp_id;
            debug_break_insert(s, b->file, b->line, b->id);
            changed = 1;
        }
    }
    if (changed)
        debug_break_end(s);
}

/* The IDE's side of a session's end; the debugger may have already quit by itself. */
static void debug_end_session(struct ide* ide)
{
    gui_set_tooltip(ide->app, 0, 0, NULL);
    debug_shutdown(&ide->session);
    debug_init(&ide->session);
    debug_clear_exec_lines(ide);
    debug_info_refresh(ide);
    debug_menu_refresh(ide);
    status(ide, "Debugging stopped");
}

static void debug_stop_session(struct ide* ide)
{
    if (ide->session.state == DBG_IDLE)
        return;
    debug_end_session(ide);
}

/* The menu as the session allows: Start when idle, Stop while it runs,
 * Continue and the steps when stopped - F5 is Start or Continue. */
static void debug_menu_refresh(struct ide* ide)
{
    enum debug_state st = ide->session.state;
    int enabled[6] = { st == DBG_IDLE, st != DBG_IDLE, st == DBG_STOPPED, st == DBG_STOPPED, st == DBG_STOPPED,
                       st == DBG_STOPPED };
    for (int i = 0; i < 6; i++)
    {
        if (ide->debug_items[i])
            gui_set_enabled(ide->debug_items[i], enabled[i]);
    }
}

static void debug_info_refresh(struct ide* ide)
{
    struct debug_session* s = &ide->session;
    struct gui_node* list = ide->debug_info_list;
    gui_clear_children(list);
    gui_append(list, create(ide, GUI_ITEM, "-- Locals --"));
    if (s->locals.count == 0)
        gui_append(list, create(ide, GUI_ITEM, "  (none)"));
    for (int i = 0; i < s->locals.count; i++)
    {
        char label[400] = { 0 };
        snprintf(label, sizeof label, "  %s = %s", s->locals.items[i].name, s->locals.items[i].value);
        gui_append(list, create(ide, GUI_ITEM, label));
    }
    gui_append(list, create(ide, GUI_ITEM, "-- Call Stack --"));
    if (s->frames.count == 0)
        gui_append(list, create(ide, GUI_ITEM, "  (none)"));
    for (int i = 0; i < s->frames.count; i++)
    {
        char label[300] = { 0 };
        snprintf(label, sizeof label, "  #%d %s", s->frames.items[i].index, s->frames.items[i].text);
        gui_append(list, create(ide, GUI_ITEM, label));
    }
}

/* Stopped, the mouse over a variable or field in a C file: its value in a
 * tooltip. A local comes from the Locals the stop already asked for; the
 * rest (globals, fields) is asked once per name and stop. */
static void debug_hover(struct ide* ide)
{
    struct debug_session* s = &ide->session;
    char word[128] = { 0 }, tip[400] = { 0 };
    int x = 0, y = 0;
    tip[0] = 0;
    /* only in the stopped frame's file, and not before this stop's Locals
     * are asked: a name elsewhere may be another function's */
    struct doc* d = s->state == DBG_STOPPED && !s->refresh_pending ? doc_for_debug_file(ide, s->cur_file) : NULL;
    if (d && !gui_modal_open(ide->app) &&
        gui_editor_word_at_mouse(ide->app, d->editor, word, sizeof word, &x, &y))
    {
        for (int k = 0; k < s->locals.count && !tip[0]; k++)
        {
            if (strcmp(s->locals.items[k].name, word) == 0)
                snprintf(tip, sizeof tip, "%s = %s", word, s->locals.items[k].value);
        }
        if (tip[0])
            ;
        else if (strcmp(s->eval_expr, word) != 0)
            debug_evaluate(s, word);   /* asked now (once the Locals are in); shown on a later tick */
        else if (s->eval_done && s->eval_value[0])
            snprintf(tip, sizeof tip, "%s = %s", word, s->eval_value);
    }
    gui_set_tooltip(ide->app, x, y, tip);
}

/* The tick while a session is up. */
static void debug_tick(struct ide* ide)
{
    struct debug_session* s = &ide->session;
    debug_poll(s);
    debug_sync_breakpoints(ide);
    debug_sync_exec_line(ide);
    int stopped = s->state == DBG_STOPPED;
    if (stopped && s->refresh_pending && s->query_state == DQS_IDLE)
    {
        /* every stop, a quick step's too, and once a hover's question is answered */
        status(ide, "Stopped");
        debug_refresh_info(s);   /* Locals and Call Stack, asked of the debugger */
        s->refresh_pending = false;
    }
    else if (!stopped && ide->session_was_stopped)
    {
        debug_clear_info(s);
    }
    ide->session_was_stopped = stopped;
    if (s->info_dirty)
    {
        debug_info_refresh(ide);
        s->info_dirty = false;
    }
    debug_hover(ide);
    debug_menu_refresh(ide);
    if (s->state == DBG_EXITED || debug_backend_exited(s))
    {
        char msg[100] = { 0 };
        snprintf(msg, sizeof msg, "The program exited with code %d", s->last_exit_code);
        debug_end_session(ide);
        output(ide, msg);
        status(ide, msg);
    }
}

static void on_event(void* ctx, int id);

/* --- The Output's command line: an IDE command ("help"), an External
 * Tool's title or a menu item's name runs it; anything else - or a line
 * starting with "!" - runs in the shell, in the Folder's directory. While
 * a program runs, a line goes to its input. The old IDE's. --- */

static void cmdline_layout(struct ide* ide)
{
    int to_stdin = ide->run.proc != NULL;
    struct gui_layout l = { GUI_ANCHOR_LEFT | GUI_ANCHOR_RIGHT | GUI_ANCHOR_BOTTOM };
    l.left.cells = to_stdin ? 8 : 3;
    l.right.cells = l.bottom.cells = 1;
    l.height.cells = 1;
    gui_set_layout(ide->output.input, &l);
    gui_set_label(ide->output.prompt, to_stdin ? "stdin>" : ">");
    ide->output.to_stdin = to_stdin;
}

/* A label as the command line matches it: letters and digits, lowercase. */
static void cmdline_key(const char* label, char* out, size_t cap)
{
    size_t n = 0;
    for (const char* p = label; *p && n + 1 < cap; p++)
    {
        if (isalnum((unsigned char)*p))
            out[n++] = (char)tolower((unsigned char)*p);
    }
    out[n] = '\0';
}

static int cmdline_menu(struct ide* ide, const char* line)
{
    char key[192] = { 0 }, item[128] = { 0 }, full[256] = { 0 };
    cmdline_key(line, key, sizeof key);
    if (!key[0])
        return 0;
    int found = 0, ambiguous = 0;
    for (int m = 0; m < COUNT(menus); m++)
    {
        char menu_key[64] = { 0 };
        cmdline_key(menus[m].title, menu_key, sizeof menu_key);
        for (int i = 0; i < menus[m].count; i++)
        {
            const struct menu_item* it = &menus[m].items[i];
            if (it->id == EV_NONE)
                continue;
            cmdline_key(it->label, item, sizeof item);
            snprintf(full, sizeof full, "%s%s", menu_key, item);
            if (strcmp(full, key) == 0)
            {
                on_event(ide, it->id);
                return 1;
            }
            if (strcmp(item, key) == 0)
            {
                ambiguous = found != 0;
                found = it->id;
            }
        }
    }
    if (ambiguous)
    {
        output(ide, "Several menus have that item - prefix the menu name, e.g. \"file close\".");
        return 1;
    }
    if (found)
        on_event(ide, found);
    return found != 0;
}

static void cmdline_help(struct ide* ide)
{
    output(ide, "  help       list the IDE commands\n"
                "  clone [url] open the Git Clone dialog\n"
                "  build      build the project or the active file\n"
                "  clear      clear the Output window\n"
                "  tools      open the External Tools dialog\n"
                "  line <n>   go to line n of the active file\n"
                "  menu       list the menu items - type one (\"format\", \"file close\") to run it\n"
                "  cd [dir]   show or change the Folder directory, where shell commands run\n"
                "An External Tool's title or a menu item's name runs it (see \"menu\"). Anything else, or a line starting with !, runs in the shell.\n"
                "While a program runs, a line is sent to its input.");
}

static void cmdline_execute(struct ide* ide)
{
    char line[1024] = { 0 };
    snprintf(line, sizeof line, "%s", gui_get_value(ide->output.input));
    gui_set_value(ide->output.input, "");
    gui_focus(ide->app, ide->output.input);
    if (ide->run.proc)
    {
        char typed[1100] = { 0 };
        snprintf(typed, sizeof typed, "%s\n", line);
        if (ide_process_write(ide->run.proc, typed, (int)strlen(typed)) < 0)
            output(ide, "(the running program does not accept input)");
        else
            output_raw(ide, typed, (int)strlen(typed));
        return;
    }
    const char* start = line;
    while (*start == ' ' || *start == '\t')
        start++;
    if (!*start)
        return;
    char word[64] = "", arg[1024] = "";
    sscanf(start, "%63s %1023[^\n]", word, arg);
    if (*start == '!')
    {
        if (start[1])
        {
            const char* steps[] = { start + 1 };
            run_start(ide, RUN_TOOL, start + 1, ide->folder.dir, steps, 1);
        }
    }
    else if (strcmp(word, "help") == 0)
        cmdline_help(ide);
    else if (strcmp(word, "clone") == 0)
    {
        on_event(ide, EV_GIT_CLONE);
        if (arg[0])
            gui_set_value(ide->clone.url, arg);
    }
    else if (strcmp(word, "build") == 0)
        on_event(ide, EV_BUILD);
    else if (strcmp(word, "clear") == 0)
        gui_set_value(ide->output.editor, "");
    else if (strcmp(word, "tools") == 0)
        on_event(ide, EV_EXTERNAL_TOOLS);
    else if (strcmp(word, "line") == 0)
    {
        struct doc* d = active_doc(ide);
        int n = atoi(arg);
        if (n <= 0 || !d)
            output(ide, n <= 0 ? "Not a line number." : "No file is open.");
        else
        {
            nav_record_jump(ide);
            gui_editor_goto_line_center(d->editor, n);
            gui_focus(ide->app, d->editor);
        }
    }
    else if (strcmp(word, "menu") == 0)
    {
        for (int m = 0; m < COUNT(menus); m++)
        {
            struct ide_text t = { 0 };
            char key[128] = { 0 };
            cmdline_key(menus[m].title, key, sizeof key);
            ide_text_printf(&t, "%s:", key);
            for (int i = 0; i < menus[m].count; i++)
            {
                if (menus[m].items[i].id == EV_NONE)
                    continue;
                cmdline_key(menus[m].items[i].label, key, sizeof key);
                ide_text_printf(&t, " %s", key);
            }
            output(ide, t.data);
            free(t.data);
        }
    }
    else if (strcmp(word, "cd") == 0)
    {
        if (arg[0])
        {
            char dir[1024] = { 0 };
            int absolute = arg[0] == '/' || arg[0] == '\\' || (isalpha((unsigned char)arg[0]) && arg[1] == ':');
            if (absolute)
                snprintf(dir, sizeof dir, "%s", arg);
            else
                join_path(dir, sizeof dir, ide->folder.dir, arg);
            char full[1024] = { 0 };
            ide_full_path(dir, full, sizeof full);
            if (!ide_is_dir(full))
            {
                char msg[1100] = { 0 };
                snprintf(msg, sizeof msg, "No such directory: %s", full);
                output(ide, msg);
                return;
            }
            snprintf(ide->folder.dir, sizeof ide->folder.dir, "%s", full);
            folder_refresh(ide);
        }
        output(ide, ide->folder.dir);
    }
    else
    {
        /* a tool's title, a menu item, else the shell */
        char key[192] = { 0 }, title[192] = { 0 };
        cmdline_key(start, key, sizeof key);
        for (int i = 0; strncmp(start, "./", 2) != 0 && i < ide->ext_tools.count; i++)
        {
            cmdline_key(ide->ext_tools.tools[i].title, title, sizeof title);
            if (key[0] && strcmp(title, key) == 0)
            {
                run_tool(ide, i);
                return;
            }
        }
        if (strncmp(start, "./", 2) != 0 && cmdline_menu(ide, start))
            return;
#ifdef _WIN32
        char aliased[1100] = { 0 };
        if (strncmp(start, "ls", 2) == 0 && (start[2] == 0 || start[2] == ' '))
        {
            snprintf(aliased, sizeof aliased, "dir%s", start + 2);   /* the old IDE's one alias */
            start = aliased;
        }
        char cmd[1200] = { 0 };
        snprintf(cmd, sizeof cmd, "cmd /c %s", start);
        const char* steps[] = { cmd };
#else
        const char* steps[] = { start };
#endif
        run_start(ide, RUN_TOOL, start, ide->folder.dir, steps, 1);
    }
}

static void on_event(void* ctx, int id)
{
    struct ide* ide = ctx;
    struct doc* doc = active_doc(ide);
    struct gui_node* ed = focused_editor(ide);

    if (id > EV_COUNT && id <= EV_COUNT + 3)
    {
        static const enum gui_dock sides[] = { GUI_DOCK_LEFT, GUI_DOCK_RIGHT, GUI_DOCK_BOTTOM };
        dock_panel(ide, sides[id - EV_COUNT - 1]);
        return;
    }

    switch (id)
    {
    case EV_NEW_FILE:
    {
        ide->newfile_in_project = 0;
        char dir[1024] = { 0 };
        start_dir(ide, dir, sizeof dir);
        gui_set_value(ide->newfile.folder, dir);
        gui_set_value(ide->newfile.name, "");
        show_dialog(ide, ide->newfile.window, 54, 9, ide->newfile.name);
        break;
    }
    case EV_NEWFILE_OK: newfile_accept(ide); break;
    case EV_NEWFILE_OVERWRITE: newfile_create(ide); break;
    case EV_SAVEAS_OVERWRITE: save_as_commit(ide); break;
    case EV_OPEN_LINK:
        if (ide->pending_url)
            ide_open_url(ide->pending_url);
        break;
    case EV_NEWFILE_CANCEL: gui_window_close(ide->app, ide->newfile.window); break;
    case EV_NEWFILE_BROWSE: pick_path(ide, ide->newfile.folder, 0, gui_get_value(ide->newfile.folder)); break;
    case EV_OPEN:
    {
        char dir[1024] = { 0 };
        start_dir(ide, dir, sizeof dir);
        show_open(ide, 0, dir, "");
        break;
    }
    case EV_SAVE_AS:
        if (doc)
        {
            char dir[1024] = { 0 };
            start_dir(ide, dir, sizeof dir);
            show_open(ide, 1, dir, file_name(doc->path));
        }
        break;
    case EV_OPEN_OK: open_ok(ide); break;
    case EV_RENAME:
    {
        int lo = 0, hi = 0;
        if (ed && string_at_caret(ed, &lo, &hi))
            edit_string_open(ide, ed);   /* F2 on a string literal edits it */
        else
            rename_open(ide);
        break;
    }
    case EV_RENAME_OK: rename_run(ide); break;
    case EV_RENAME_CANCEL: gui_window_close(ide->app, ide->rename.window); break;
    case EV_EDIT_STRING: if (ed) edit_string_open(ide, ed); break;
    case EV_ESTR_OK: edit_string_accept(ide); break;
    case EV_ESTR_CANCEL: gui_window_close(ide->app, ide->estr.window); break;
    case EV_TOGGLE_HDRSRC: if (doc) toggle_header_source(ide, doc); break;
    case EV_EDITOR_MENU: editor_menu_refresh(ide); break;
    case EV_EDITOR_CTRLCLICK: editor_ctrlclick(ide); break;
    case EV_HELP_CTRLCLICK: help_ctrlclick(ide); break;
    case EV_BACK: nav_go(ide, &ide->nav.back, &ide->nav.forward); break;
    case EV_FORWARD: nav_go(ide, &ide->nav.forward, &ide->nav.back); break;
    case EV_FORMAT: format_doc(ide); break;
    case EV_DEBUG_START: debug_start_session(ide); break;
    case EV_DEBUG_INFO:
        if (is_open(ide, ide->debug_info_window))
            gui_window_close(ide->app, ide->debug_info_window);
        else
        {
            debug_info_refresh(ide);
            gui_window_open(ide->app, ide->debug_info_window);
        }
        break;
    case EV_COMPLETE: complete_word(ide); break;
    case EV_CMDLINE: cmdline_execute(ide); break;
    case EV_TERMINAL:
    {
        /* the active document's folder, else the Folder panel's - the old IDE's */
        struct doc* d = active_doc(ide);
        char dir[1024] = { 0 };
        snprintf(dir, sizeof dir, "%s", d && d->path[0] && !is_playground(ide, d) ? d->path : "");
        if (dir[0])
            parent_dir(dir);
        else
            snprintf(dir, sizeof dir, "%s", ide->folder.dir);
        ide_open_terminal(dir);
        break;
    }
    case EV_DEBUG_STOP: debug_stop_session(ide); break;
    case EV_DEBUG_CONTINUE: if (ide->session.state == DBG_STOPPED) debug_continue(&ide->session); break;
    case EV_DEBUG_STEP_OVER: if (ide->session.state == DBG_STOPPED) debug_step_over(&ide->session); break;
    case EV_DEBUG_STEP_INTO: if (ide->session.state == DBG_STOPPED) debug_step_into(&ide->session); break;
    case EV_DEBUG_STEP_OUT: if (ide->session.state == DBG_STOPPED) debug_step_out(&ide->session); break;
    case EV_DEBUG_BREAKPOINT:
        /* F9: the caret's line, in a C file - as a click on its number */
        if (doc && (ends_with(doc->path, ".c") || ends_with(doc->path, ".h")))
        {
            int line = 0, col = 0;
            gui_editor_get_caret(doc->editor, &line, &col);
            gui_editor_toggle_breakpoint(doc->editor, line);
        }
        break;
    case EV_OUTPUT_COPY_ALL:
    case EV_OUTPUT_SELECT_ALL:
        gui_editor_set_selection(ide->output.editor, 0, (int)strlen(gui_get_value(ide->output.editor)));
        gui_focus(ide->app, ide->output.editor);
        if (id == EV_OUTPUT_COPY_ALL)
            gui_editor_copy(ide->app, ide->output.editor);
        break;
    case EV_OUTPUT_CLEAR: gui_set_value(ide->output.editor, ""); break;
    case EV_TOGGLE_READONLY:
    {
        struct doc* d = doc_of_editor(ide, gui_context_target(ide->app));
        if (d)
            gui_editor_set_read_only(d->editor, !gui_editor_get_read_only(d->editor));
        break;
    }
    case EV_TOGGLE_DETACH:
    {
        struct doc* d = doc_of_editor(ide, gui_context_target(ide->app));
        if (!d)
            break;
        if (gui_window_get_detached(ide->app, d->window))
            gui_window_attach(ide->app, d->window);
        else
            gui_window_detach(ide->app, d->window);
        gui_focus(ide->app, d->editor);
        break;
    }
    case EV_SHOW_FOLDER: if (doc) show_folder_of(ide, doc->path); break;
    case EV_COPY_PATH:
        if (doc)
        {
            gui_set_clipboard(ide->app, doc->path);
            status(ide, "Path copied");
        }
        break;
    case EV_FOLDER_COPY_PATH:
        /* the panel's folder, as the old IDE */
        gui_set_clipboard(ide->app, ide->folder.dir);
        status(ide, "Path copied");
        break;
    case EV_FOLDER_NEW_FOLDER:
        gui_set_value(ide->new_folder.input, "");
        show_dialog(ide, ide->new_folder.window, 44, 8, ide->new_folder.input);
        break;
    case EV_NEWFOLDER_OK: new_folder_accept(ide); break;
    case EV_COMPILE: build_then(ide, 0, 1, 0); break;
    case EV_BUILD: build_then(ide, 0, 0, 0); break;
    case EV_REBUILD: build_then(ide, 1, 0, 0); break;
    case EV_TICK:
        if (is_open(ide, ide->git.diff_window))
            git_diff_counter_refresh(ide);
        if (ide->session.state != DBG_IDLE)
            debug_tick(ide);
        if (ide->run.proc)
            run_poll(ide);
        if (ide_compile_running(ide->job))
            compile_poll(ide);
        else
            file_watch_check(ide);
        /* fast while something runs, else back to watching the files */
        gui_set_timer(ide->app, ide->session.state != DBG_IDLE || ide->run.proc || ide_compile_running(ide->job) ? 50 : 2000,
                      EV_TICK);
        break;
    case EV_FILE_RELOAD: file_reload(ide); break;
    case EV_PROJECT_RELOAD:
    {
        char path[1024] = { 0 };
        snprintf(path, sizeof path, "%s", ide->project.file_path);
        project_open(ide, path);
        break;
    }
    case EV_SHOW_GENERATED: show_generated_code(ide); break;
    case EV_OUTPUT_DBLCLICK: output_goto_source(ide, ide->output.editor); break;
    case EV_FR_DBLCLICK: output_goto_source(ide, ide->fr.editor); break;
    case EV_FR_CLEAR:
        free(ide->fr.previous);
        ide->fr.previous = NULL;
        gui_set_value(ide->fr.editor, "");
        break;
    case EV_FIND_DEFINITION: find_definition(ide, FIND_DEFINITION); break;
    case EV_FIND_DECLARATION: find_definition(ide, FIND_DECLARATION); break;
    case EV_FIND_USAGES: find_definition(ide, FIND_USAGES); break;
    case EV_PROJECT_REPORT_UNUSED: report_unused(ide); break;
    case EV_VIEW_FIND_RESULTS: bottom_panel_show(ide, ide->fr.window, ide->output.window); break;
    case EV_FOLDER_DELETE: folder_delete_ask(ide); break;
    case EV_FOLDER_DELETE_OK: folder_delete_confirmed(ide); break;
    case EV_NEWFOLDER_CANCEL: gui_window_close(ide->app, ide->new_folder.window); break;
    case EV_GIT_CLONE:
        snprintf(ide->clone.parent, sizeof ide->clone.parent, "%s", ide->folder.dir);
        ide->clone.path_edited = 0;
        git_clone_fill_path(ide);
        show_dialog(ide, ide->clone.window, 58, 13, ide->clone.url);
        break;
    case EV_CLONE_OK: git_clone(ide); break;
    case EV_VIEW_GIT: git_show_panel(ide); break;
    case EV_GIT_LIST: case EV_GIT_REFRESH: case EV_GIT_COMMIT: case EV_GIT_COMMIT_PUSH:
    case EV_GIT_COMMIT_FILE: case EV_GIT_COMMIT_STAGED: case EV_GIT_COMMIT_STAGED_PUSH: case EV_GIT_STAGE: case EV_GIT_UNSTAGE:
    case EV_GIT_DISCARD: case EV_GIT_DISCARD_OK: case EV_GIT_DISCARD_ALL: case EV_GIT_DISCARD_ALL_OK:
    case EV_GIT_PULL: case EV_GIT_PUSH: case EV_GIT_SYNC: case EV_GIT_BRANCH:
    case EV_GITCOMMIT_OK: case EV_GITCOMMIT_CANCEL:
    case EV_GITBRANCH_CHECKOUT: case EV_GITBRANCH_NEW: case EV_GITBRANCH_CANCEL:
    case EV_GITDIFF_PREV: case EV_GITDIFF_NEXT: case EV_GITDIFF_EDIT: case EV_GITDIFF_COPY_PATH:
    case EV_GITDIFF_SHOW_FOLDER:
        git_event(ide, id);
        break;
    case EV_CLONE_CANCEL: gui_window_close(ide->app, ide->clone.window); break;
    case EV_CLONE_BROWSE:
        ide->clone.path_edited = 1;
        pick_path(ide, ide->clone.path, 0, gui_get_value(ide->clone.path));
        break;
    case EV_CLONE_URL_CHANGED: git_clone_fill_path(ide); break;
    case EV_CLONE_PATH_CHANGED: ide->clone.path_edited = 1; break;
    case EV_PROJECT_NEW:
    {
        char dir[1024] = { 0 };
        start_dir(ide, dir, sizeof dir);
        gui_set_value(ide->new_project.folder, dir);
        gui_set_value(ide->new_project.name, "");
        show_dialog(ide, ide->new_project.window, 54, 12, ide->new_project.name);
        break;
    }
    case EV_NEWPROJ_OK: new_project_accept(ide); break;
    case EV_PROJECT_OPEN:
    {
        /* File > Open > Project: the file picker at once - the recent ones are File > Recent Projects */
        char dir[1024] = { 0 };
        start_dir(ide, dir, sizeof dir);
        gui_set_selected(ide->open.filter, PROJECT_FILTER);
        show_open(ide, 0, dir, "");
        ide->open.open_project = 1;
        gui_set_label(ide->open.window, "Open Project");
        break;
    }
    case EV_PROJECT_ADD_FILE:
        if (ide_project_is_open(&ide->project))
        {
            gui_set_selected(ide->open.filter, C_SOURCES_FILTER);
            char dir[1024] = { 0 };
            start_dir(ide, dir, sizeof dir);
            show_open(ide, 0, dir, "");
            ide->open.add_to_project = 1;
            gui_set_multi(ide->open.list, 1);   /* Ctrl / Shift pick several files */
            gui_set_label(ide->open.window, "Add Existing File");
            gui_set_label(ide->open.ok, "Add");
        }
        break;
    case EV_PROJECT_CLOSE: project_close(ide); break;
    case EV_PROJECT_RENAME:
        if (ide_project_is_open(&ide->project))
        {
            gui_set_value(ide->project_rename.input, ide->project.name);
            show_dialog(ide, ide->project_rename.window, 50, 8, ide->project_rename.input);
        }
        break;
    case EV_PROJ_RENAME_OK: project_rename_accept(ide); break;
    case EV_PROJ_RENAME_CANCEL: gui_window_close(ide->app, ide->project_rename.window); break;
    case EV_VIEW_PROJECT: project_show_panel(ide); break;
    case EV_PROJECT_LIST:
    case EV_PROJ_COPY_PATH:
    case EV_PROJ_NEW_FILE:
    case EV_PROJ_REMOVE:
    case EV_PROJ_DELETE:
    case EV_PROJ_DELETE_OK:
    case EV_FOLDER_ADD_TO_PROJECT:
        project_event(ide, id);
        break;
    case EV_NEWPROJ_CANCEL: gui_window_close(ide->app, ide->new_project.window); break;
    case EV_NEWPROJ_BROWSE:
        pick_path(ide, ide->new_project.folder, 0, gui_get_value(ide->new_project.folder));
        break;
    case EV_PROJECT_OPTIONS:
    {
        /* the active file outside the project (the playground, a loose file), or no
         * project: the playground project's, used by every file outside a project */
        struct doc* d = active_doc(ide);
        if (ide_project_is_open(&ide->project) && (!d || file_uses_project(ide, d->path)))
            copts_open(ide, &ide->project.compile, "Properties (Project)", 0);
        else
            copts_open(ide, &ide->global_options, "Properties (Playground)", 0);
        break;
    }
    case EV_COPTS_OK:
    {
        /* an option the compiler refuses: Keep it anyway, or Fix it - the old IDE's */
        char buf[1024] = { 0 }, bad[200] = "";
        snprintf(buf, sizeof buf, "%s", gui_get_value(ide->copts.options));
        const char* argv[64] = { 0 };
        int argc = 0;
        argv[argc++] = "cake";
        for (char* tok = strtok(buf, " \t"); tok && argc < COUNT(argv) && !bad[0]; tok = strtok(NULL, " \t"))
        {
            argv[argc++] = tok;
            struct options options = { 0 };
            if (fill_options(&options, argc, argv) != 0)
                snprintf(bad, sizeof bad, "%s", tok);
        }
        if (bad[0])
        {
            char msg[400] = { 0 };
            snprintf(msg, sizeof msg, "The compiler does not accept this option:\n\n  %s\n\nKeep the options anyway?", bad);
            static const char* const labels[] = { "Keep", "Fix" };
            static const int ids[] = { EV_COPTS_KEEP_INVALID, 0 };
            gui_message_box(ide->app, "Properties", msg, labels, ids, 2);
            break;
        }
    }
    FALLTHROUGH;
    case EV_COPTS_KEEP_INVALID:
        copts_accept(ide);
        if (ide->copts.settings == &ide->project.compile)
            project_save(ide);
        else
            settings_save(ide);
        config_menu_refresh(ide);
        break;
    case EV_COPTS_CONFIG: copts_config_changed(ide); break;
    case EV_COPTS_CONFIG_NEW: config_name_open(ide, 1); break;
    case EV_COPTS_CONFIG_RENAME: config_name_open(ide, 0); break;
    case EV_COPTS_CONFIG_DELETE: config_delete_ask(ide); break;
    case EV_COPTS_CONFIG_DELETE_YES: config_delete(ide); break;
    case EV_CONFIG_NAME_OK: config_name_accept(ide); break;
    case EV_CONFIG_NAME_CANCEL: gui_window_close(ide->app, ide->copts.name_window); break;
    case EV_COPTS_CANCEL: gui_window_close(ide->app, ide->copts.window); break;
    case EV_COPTS_HELP: help_open(ide); break;
    case EV_COPTS_AUTO_CONFIG: copts_auto_config(ide); break;
    case EV_COPTS_INC_ADD:
        pick_path(ide, NULL, 0, "");
        ide->open.pick_include = 1;
        break;
    case EV_COPTS_INC_REMOVE: dirs_edit(ide, ide->copts.includes, &ide->copts.include_dirs, 0); break;
    case EV_COPTS_INC_UP: dirs_edit(ide, ide->copts.includes, &ide->copts.include_dirs, -1); break;
    case EV_COPTS_INC_DOWN: dirs_edit(ide, ide->copts.includes, &ide->copts.include_dirs, 1); break;
    case EV_COPTS_PAGE: copts_select_page(&ide->copts, gui_get_selected(ide->copts.pages)); break;
    case EV_DBG_BROWSE:
        gui_set_selected(ide->open.filter, PROGRAMS_FILTER);
        pick_path(ide, ide->copts.debug[0], 1, "");
        break;
    case EV_POST_BUILD_BROWSE:
        gui_set_selected(ide->open.filter, PROGRAMS_FILTER);
        pick_path(ide, ide->copts.post_build[0], 1, "");
        break;
    case EV_EXTERNAL_TOOLS: external_tools_open(ide); break;
    case EV_EXT_LIST:
    case EV_EXT_ADD:
    case EV_EXT_DELETE:
    case EV_EXT_UP:
    case EV_EXT_DOWN:
        ext_event(ide, id);
        break;
    case EV_EXT_OK:
        ext_event(ide, id);
        settings_save(ide);
        break;
    case EV_EXT_CANCEL: gui_window_close(ide->app, ide->ext.window); break;
    case EV_EXT_BROWSE: pick_path(ide, ide->ext.fields[1], 1, ""); break;
    case EV_HELP:
    case EV_MANUAL:
        help_open(ide);
        break;
    case EV_HELP_CLOSE: gui_window_close(ide->app, ide->help.window); break;
    case EV_HELP_BACK: help_back(ide); break;

    case EV_OPEN_FOLDER:
    {
        char dir[1024] = { 0 };
        start_dir(ide, dir, sizeof dir);
        show_open(ide, 0, dir, "");
        open_folder_mode(ide, 1);
        gui_set_label(ide->open.window, "Open Folder");
        gui_set_label(ide->open.ok, "Select");
        break;
    }
    case EV_OPEN_LIST: open_list_pick(ide); break;
    case EV_OPEN_CANCEL: gui_window_close(ide->app, ide->open.window); break;
    case EV_TO_UPPER: if (ed) transform_selection(ide, ed, 1); break;
    case EV_TO_LOWER: if (ed) transform_selection(ide, ed, 0); break;
    case EV_STRINGIFY: if (ed) stringify_selection(ide, ed); break;
    case EV_WORD_WRAP:
        if (ed)
        {
            ide->wrap.target = ed;
            if (!gui_get_value(ide->wrap.columns)[0])
                gui_set_value(ide->wrap.columns, "80");
            show_dialog(ide, ide->wrap.window, 40, 9, ide->wrap.columns);
        }
        break;
    case EV_WRAP_OK:
        if (ide->wrap.target)
            wrap_text(ide->wrap.target, atoi(gui_get_value(ide->wrap.columns)), gui_get_checked(ide->wrap.justify, 0));
        wrap_close(ide);
        break;
    case EV_WRAP_CANCEL: wrap_close(ide); break;
    case EV_FIND:
        if (doc)
            show_dialog(ide, ide->find.window, 56, 16, ide->find.input);
        break;
    case EV_FIND_OK:
    {
        struct find_dialog* f = &ide->find;
        read_search(ide, f->input, f->options, f->direction, f->scope, f->origin);
        gui_window_close(ide->app, f->window);
        search(ide, &ide->search, ide->search.from_start);
        break;
    }
    case EV_FIND_CANCEL: gui_window_close(ide->app, ide->find.window); break;
    case EV_SEARCH_NEXT: search(ide, &ide->search, 0); break;
    case EV_REPLACE:
        if (doc)
            show_dialog(ide, ide->replace.window, 60, 19, ide->replace.find);
        break;
    case EV_REPLACE_OK:
    case EV_REPLACE_ALL:
    {
        struct replace_dialog* r = &ide->replace;
        read_search(ide, r->find, r->options, r->direction, r->scope, r->origin);
        gui_window_close(ide->app, r->window);
        replace(ide, id == EV_REPLACE_ALL);
        break;
    }
    case EV_REPLACE_CANCEL: gui_window_close(ide->app, ide->replace.window); break;
    case EV_SAVE:
        if (doc)
            save_doc(ide, doc);
        break;
    case EV_SAVE_ALL:
        for (int i = 0; i < ide->doc_count; i++)
        {
            if (gui_editor_get_dirty(ide->docs[i].editor))
                save_doc(ide, &ide->docs[i]);
        }
        break;
    case EV_EXIT: exit_check(ide); break;
    case EV_EXIT_SAVE:
    {
        struct doc* d = doc_of_window(ide, ide->exit_window);
        if (d)
            save_doc(ide, d);
        if (d && gui_editor_get_dirty(d->editor))
            break;   /* it could not be saved: the statusbar says why */
        exit_check(ide);
        break;
    }
    case EV_EXIT_DISCARD:
    {
        struct doc* d = doc_of_window(ide, ide->exit_window);
        if (d)
            gui_editor_set_dirty(d->editor, 0);
        exit_check(ide);
        break;
    }
    case EV_DOC_CLOSE: doc_close_request(ide); break;
    case EV_CLOSE_DISCARD:
    {
        struct doc* d = doc_of_window(ide, ide->pending_close);
        ide->pending_close = NULL;
        if (d)
            close_doc(ide, d);
        break;
    }
    case EV_UNDO: if (ed) gui_editor_undo(ed); break;
    case EV_REDO: if (ed) gui_editor_redo(ed); break;
    case EV_CUT: if (ed) gui_editor_cut(ide->app, ed); break;
    case EV_COPY: if (ed) gui_editor_copy(ide->app, ed); break;
    case EV_PASTE: if (ed) gui_editor_paste(ide->app, ed); break;
    case EV_VIEW_OUTPUT: bottom_panel_show(ide, ide->output.window, ide->fr.window); break;
    case EV_VIEW_FOLDER: show_side_panel(ide, ide->folder.window); break;
    case EV_VIEW_PLAYGROUND: open_playground(ide); break;
    case EV_FIND_IN_FILES: fif_open(ide); break;
    case EV_FR_TAB_FIND:
    case EV_FR_TAB_REPLACE:
        if ((id == EV_FR_TAB_REPLACE) != ide->fif.mode)
        {
            fif_sync(ide);
            ide->fif.mode = id == EV_FR_TAB_REPLACE;
            fif_build(ide);
        }
        break;
    case EV_FR_FIND: fif_run(ide, 0); break;
    case EV_FR_REPLACE: fif_run(ide, 1); break;
    case EV_GOTO_LINE:
        if (doc)
        {
            gui_set_value(ide->go.input, "");
            show_dialog(ide, ide->go.window, 40, 8, ide->go.input);
        }
        break;
    case EV_GOTO_OK:
    {
        int line = atoi(gui_get_value(ide->go.input));
        gui_window_close(ide->app, ide->go.window);
        if (doc && line > 0)
        {
            nav_record_jump(ide);
            gui_editor_goto_line_center(doc->editor, line);
            gui_focus(ide->app, doc->editor);
        }
        break;
    }
    case EV_GOTO_CANCEL:
        gui_window_close(ide->app, ide->go.window);
        break;
    case EV_TILE: tile(ide); break;
    case EV_CASCADE: cascade(ide); break;
    case EV_CLOSE_ALL:
        /* Documents only; one with unsaved changes stays open. */
        for (int i = ide->doc_count - 1; i >= 0; i--)
        {
            if (!gui_editor_get_dirty(ide->docs[i].editor))
                close_doc(ide, &ide->docs[i]);
        }
        break;
    case EV_ENVIRONMENT:
        for (int i = 0; i < COUNT(themes); i++)
        {
            if (themes[i] == ide->theme)
                gui_set_selected(ide->env.theme, i);
        }
        if (ide->env.font)
            gui_set_selected(ide->env.font, gui_get_font(ide->app));
        if (ide->env.ui_font)
        {
            gui_set_selected(ide->env.ui_font, gui_get_ui_font(ide->app));
        }
        gui_set_selected(ide->env.ui_size, gui_get_editor_size(ide->app) + 1);
        show_dialog(ide, ide->env.window, 50, 11, ide->env.theme);
        break;
    case EV_ENV_OK:
        apply_theme(ide, gui_get_selected(ide->env.theme));
        gui_window_close(ide->app, ide->env.window);
        settings_save(ide);
        break;
    case EV_FONT_BIGGER: gui_zoom(ide->app, 1); break;
    case EV_FONT_SMALLER: gui_zoom(ide->app, -1); break;
    case EV_ABOUT: show_dialog(ide, ide->about, 30, 10, ide->about_ok); break;
    case EV_WEBSITE: ide_open_url("https://cakecc.org/"); break;
    case EV_ABOUT_OK: gui_window_close(ide->app, ide->about); break;
    case EV_FOLDER_OPEN:
        folder_open_selected(ide);
        break;
    default:
        if (id >= EV_ENV_THEME && id <= EV_ENV_THEME_LAST && id - EV_ENV_THEME < COUNT(themes))
        {
            apply_theme(ide, id - EV_ENV_THEME);
            settings_save(ide);
            break;
        }
        if (id >= EV_ENV_FONT && id <= EV_ENV_FONT_LAST)
        {
            gui_set_font(ide->app, id - EV_ENV_FONT);
            settings_save(ide);
            break;
        }
        if (id >= EV_ENV_EDITOR_SIZE && id <= EV_ENV_EDITOR_SIZE_LAST)
        {
            gui_set_editor_size(ide->app, id - EV_ENV_EDITOR_SIZE - 1);
            settings_save(ide);
            gui_repaint(ide->app);
            break;
        }
        if (id >= EV_ENV_UI_FONT && id <= EV_ENV_UI_FONT_LAST)
        {
            gui_set_ui_font(ide->app, id - EV_ENV_UI_FONT);
            settings_save(ide);
            gui_repaint(ide->app);
            break;
        }
        if (id >= EV_MACRO && id < EV_MACRO + ide->macro.count)
        {
            macro_event(ide, id);
            break;
        }
        if (id >= EV_MACRO_ITEM && id < EV_MACRO_ITEM + COUNT(macros))
        {
            macro_event(ide, id);
            break;
        }
        if (id >= EV_COMPLETE_ITEM && id < EV_COMPLETE_ITEM + ide->complete.count && id - EV_COMPLETE_ITEM < COUNT(ide->complete.names))
        {
            complete_insert(ide, ide->complete.names[id - EV_COMPLETE_ITEM]);
            break;
        }
        if (id >= EV_CONFIG_ITEM && id < EV_CONFIG_ITEM + MAX_CONFIGURATIONS)
        {
            config_pick(ide, id - EV_CONFIG_ITEM);
            break;
        }
        if (id >= EV_RECENT_ITEM && id < EV_RECENT_ITEM + ide->recent_projects.count)
        {
            char path[1024] = { 0 };
            snprintf(path, sizeof path, "%s", ide->recent_projects.items[id - EV_RECENT_ITEM]);   /* project_open may drop it */
            project_open(ide, path);
            break;
        }
        if (id >= EV_TOOL_RUN && id < EV_TOOL_RUN + ide->ext_tools.count)
        {
            run_tool(ide, id - EV_TOOL_RUN);
            break;
        }
        if (id >= EV_OPEN_FILTER && id < EV_OPEN_FILTER + COUNT(open_filters))
        {
            open_refresh(ide);   /* another Type: the list follows */
            break;
        }
        if (id > 0 && id < EV_COUNT)
        {
            char msg[200] = { 0 };
            snprintf(msg, sizeof msg, "Not implemented yet: %s",
                     ide->labels[id] ? ide->labels[id] : "this command");
            status(ide, msg);
        }
        break;
    }
}

void gui_main(struct gui_app* app, int argc, char** argv)
{
    struct ide* ide = calloc(1, sizeof *ide);   /* lives as long as the app */
    if (!ide)
        abort();
    ide->app = app;
    ide->job = ide_compile_create();
    if (!ide->job)
        abort();
    ide->theme = &ide_theme_dark;
    ide->c_highlighter.highlight = ide_highlight_c;
    ide->c_highlighter.ctx = (void*)ide->theme;
    ide->md_highlighter.highlight = ide_highlight_md;
    ide->md_highlighter.row_bg = ide_md_row_bg;
    ide->md_highlighter.ctx = (void*)ide->theme;
    ide->string_highlighter.highlight = ide_highlight_string;
    ide->string_highlighter.ctx = (void*)ide->theme;
    gui_set_hint_highlighter(app, &ide->md_highlighter);
    gui_set_theme(app, ide->theme);
    gui_set_on_event(app, on_event, ide);
    gui_set_quit_id(app, EV_EXIT);

    build_menus(ide);
    build_statusbar(ide);
    build_panels(ide);
    build_dialogs(ide);
    compiler_settings_default(&ide->global_options);
    settings_load(ide);
    panels_size(ide);   /* before session_load, which puts back the saved sizes */
    playground_project_load(ide);
    config_menu_refresh(ide);
    recent_menu_refresh(ide);
    gui_set_timer(app, 2000, EV_TICK);   /* the outside-change check */

    /* Files named on the command line open at start; without any, the
     * Playground - the old IDE's start when there is no session. */
    for (int i = 1; i < argc; i++)
        open_file(ide, argv[i]);
    if (argc < 2 && !session_load(ide))
        open_playground(ide);
}
