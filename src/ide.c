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

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    EVT_WINDOW_REFRESH = 84,
    EVT_WINDOW_FOLDER = 87,  /* the View > "Show Folder" menu item's id -
                              * re-raises the persistent folder browser
                              * window (moved here from Window, its ids kept
                              * as-is - see build_screen()) */
    EVT_WINDOW_PLAYGROUND = 88,  /* View > "Show Playground" - opens (or
                                  * re-raises, if already open) the scratch
                                  * editor window pinned to playground.c;
                                  * see open_playground() */
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
    EVT_COMPILE = 40,
    EVT_COMPILE_OPTIONS = 45,  /* Compile > Options... - opens the dialog below */
    EVT_TOGGLE_AUTOCOMPILE = 44,
    EVT_WINDOW_OUTPUT = 83,  /* the View > "Show Output" menu item's id -
                              * re-raises the diagnostics Output window
                              * (moved here from Window - see build_screen()) */
    /* Window > Font Size +/- drive the same backend action as Ctrl+/Ctrl-,
     * via ui_env_adjust_font_size() (see ui.h). */
    EVT_WINDOW_FONT_INC = 85,
    EVT_WINDOW_FONT_DEC = 86,
    EVT_HELP_INDEX = 90,  /* the Help > "Index" menu item's id */
    EVT_HELP_CONTEXTUAL = 200,  /* the status bar's F1 hotkey - see
                                 * do_help_contextual() */
    EVT_HELP_ABOUT = 92,
    EVT_HELP_CHECK = 93,  /* the Help > "Check" menu item's id - see
                           * do_help_check() */
    EVT_OPTIONS_ENV = 72,
    EVT_OPTIONS_DIRS = 73,
    EVT_ABOUT_MODAL = 400,
    EVT_ABOUT_CLOSE = 401,
    EVT_DIRS_OK = 501,
    EVT_DIRS_CANCEL = 502,
    EVT_DIRS_HELP = 503,
    EVT_ENV_OK = 601,
    EVT_ENV_THEME_CLASSIC = 610,
    EVT_ENV_THEME_DARK = 611,
    EVT_ENV_THEME_WHITE = 612,
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
    EVT_SEARCH_FIND = 20,      /* the Search > "Find..." menu item's id */
    EVT_SEARCH_REPLACE = 21,   /* the Search > "Replace..." menu item's id */
    EVT_SEARCH_NEXT = 23,      /* Search > "Search Next" (F3) */
    EVT_REPLACE_OK = 820,
    EVT_REPLACE_CHANGEALL = 821,
    EVT_REPLACE_CANCEL = 822,
    EVT_FIND_OK = 830,
    EVT_FIND_CANCEL = 831,
    EVT_SAVEAS_OVERWRITE = 840,  /* "Yes" in the overwrite-confirm message box */
    EVT_CLOSE_DISCARD = 841,  /* "Discard" in the unsaved-changes-on-close
                               * confirm message box - see UI_CLOSE_REQUEST_ID
                               * and g_pending_close_window */
    EVT_WORDWRAP_INPUT = 850,   /* Edit > Word Wrap...'s "Columns" <input> */
    EVT_WORDWRAP_OK = 851,
    EVT_WORDWRAP_CANCEL = 852,
    EVT_COPTS_OK = 900,
    EVT_COPTS_CANCEL = 901,
    EVT_COPTS_TARGET = 910,  /* base id for the Target <select>'s options */
    EVT_TOOLS_FINDREPLACE = 63,  /* Tools > "Find and Replace..." - opens/
                                  * raises the docked panel (see g_fr_window) */
    EVT_TOOLS_TERMINAL = 64,    /* Tools > "Terminal" - see do_open_terminal() */
    EVT_FR_MODE = 950,        /* the panel's own Find/Replace mode-toggle button */
    EVT_FR_FIND_BTN = 951,
    EVT_FR_REPLACE_BTN = 952,
    EVT_FR_FILETYPE_BASE = 960,  /* base id for the File Types <select>'s
                                  * options - EVT_FR_FILETYPE_BASE + index;
                                  * reserves 960..96x */
    EVT_NAV_BACK = 970,     /* status bar's "Back" hotkey - see nav_back() */
    EVT_NAV_FORWARD = 971,  /* status bar's "Forward" hotkey - see nav_forward() */
    EVT_FOLDER_TOGGLE_FILTER = 972,  /* the Folder panel's own right-click
                                      * popup's "Filter" item - see
                                      * g_folder_popup/g_folder_filter_enabled */
    EVT_FOLDER_SHOW_FILTER = 973,  /* same popup's "Show Filter"/"Create
                                    * Filter" item - opens g_folder_dir's
                                    * CAKE_FOLDER_FILTER_NAME into an editor
                                    * window, creating it first (pre-filled
                                    * with the current listing) if it
                                    * doesn't exist yet - see
                                    * refresh_folder_show_filter_item/
                                    * create_default_filter_file */
};

/* One row of a <menu>'s dropdown: an id/label/shortcut triple, "---" for a
 * separator (id/shortcut ignored), and an enabled flag - see add_menu()
 * below, which turns an array of these into the real ui_node tree. */
typedef struct {
    int id;
    const char* label;
    const char* shortcut;  /* NULL if none */
    int enabled;
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
        }
        ui_append_child(menu, item);
    }
    return menu;
}

/* Build the menu bar/dropdowns/status bar, appended once to `root` at
 * startup. The "Welcome" panel is appended to the same tree programmatically
 * in app_init() below, via the same ui_create_element/ui_append_child/
 * ui_set_* calls - both routes build the identical ui_node tree. */
 /* The Edit menu's "Read-only" item (see build_screen() below) - forward
  * declared here since build_screen() is defined ahead of the rest of the
  * app's globals. Kept in sync with the active editor once per frame (see
  * app_frame()), the same way the popup's own copy is refreshed on open. */
static ui_node* g_edit_readonly_item;

/* Compile > Auto-compile's status bar mirror (see EVT_TOGGLE_AUTOCOMPILE in
 * on_ui_event) - a second clickable node with the same id as the menu item,
 * so the mode is visible/toggleable without opening the Compile menu. Kept
 * as its own pointer rather than relying on ui_find_by_id(EVT_TOGGLE_AUTOCOMPILE)
 * because that only ever returns the first match in the tree (the menu item),
 * so the status bar copy needs its label updated separately. */
static ui_node* g_statusbar_autocompile_item;

static void build_screen(ui_node* root)
{
    ui_node* menubar = ui_create_element(UI_TAG_MENUBAR);
    ui_append_child(root, menubar);

    static const menu_item_spec file_items[] = {
        { 1, "New", NULL, 1 },
        { 2, "Open...", "Ctrl+O", 1 },
        { EVT_FILE_OPENFOLDER, "Open Folder...", NULL, 1 },
        { 3, "Save", "Ctrl+S", 1 },
        { 4, "Save As...", NULL, 1 },
        { 6, "Save all", "Ctrl+Shift+S", 1 },
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
        { EVT_EDIT_TOUPPER, "To Upper", NULL, 1 },
        { EVT_EDIT_TOLOWER, "To Lower", NULL, 1 },
        SEP,
        { EVT_EDIT_WORDWRAP, "Word Wrap...", NULL, 1 },
    };
    ui_node* edit_menu = add_menu(menubar, "Edit", edit_items, sizeof edit_items / sizeof edit_items[0]);
    ui_node* edit_sep = ui_create_element(UI_TAG_ITEM);
    ui_set_separator(edit_sep, 1);
    ui_append_child(edit_menu, edit_sep);
    ui_node* edit_readonly = ui_create_element(UI_TAG_ITEM);
    ui_set_id(edit_readonly, EVT_EDITOR_TOGGLE_READONLY);
    ui_append_child(edit_menu, edit_readonly);
    g_edit_readonly_item = edit_readonly;

    /* View > "Show Output"/"Show Folder"/"Show Playground" - previously on
     * the Window menu (see EVT_WINDOW_OUTPUT/EVT_WINDOW_FOLDER/
     * EVT_WINDOW_PLAYGROUND's own doc comments) alongside window-management
     * commands (Tile/Cascade/Close all/Refresh/Font size) that don't really
     * belong with "raise this panel" commands - moved next to Edit into
     * their own menu instead. ids are unchanged, only where they're shown. */
    static const menu_item_spec view_items[] = {
        { EVT_WINDOW_OUTPUT, "Show Output", NULL, 1 },
        { EVT_WINDOW_FOLDER, "Show Folder", NULL, 1 },
        { EVT_WINDOW_PLAYGROUND, "Show Playground", NULL, 1 },
    };
    add_menu(menubar, "View", view_items, sizeof view_items / sizeof view_items[0]);

    static const menu_item_spec search_items[] = {
        { 20, "Find...", "Ctrl+F", 1 },
        { 21, "Replace...", "Ctrl+R", 1 },
        { 23, "Search Next", "F3", 1 },
        { 22, "Go to line...", "Ctrl+G", 1 },
        SEP,
        { EVT_TOOLS_FINDREPLACE, "Find in Files...", NULL, 1 }
       
      
    };
    add_menu(menubar, "Search", search_items, sizeof search_items / sizeof search_items[0]);

    /*
    static const menu_item_spec run_items[] = {
        { 30, "Run", "Ctrl+F9", 1 },
        { 31, "Program reset", "Ctrl+F2", 0 },
        { 33, "Go to cursor", "F4", 1 },
        { 34, "Trace into", NULL, 1 },
        { 35, "Step over", "F8", 1 },
        { 32, "Parameters...", NULL, 1 },
    };
    add_menu(menubar, "Run", run_items, sizeof run_items / sizeof run_items[0]);
    */

    static const menu_item_spec compile_items[] = {
        { 40, "Compile", "F7", 1 },
        //{ 41, "Make", NULL, 1 },
       // { 42, "Link", NULL, 1 },
       // { 43, "Build all", NULL, 1 },
        SEP,
        { EVT_EDITOR_SHOW_OUTPUT, "Show Generated Code", NULL, 1 },
        { 44, "Auto-compile: Off", NULL, 1 },
        { 45, "Options...", NULL, 1 },
    };
    add_menu(menubar, "Compile", compile_items, sizeof compile_items / sizeof compile_items[0]);

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
    add_menu(menubar, "Tools", tools_items, sizeof tools_items / sizeof tools_items[0]);

    static const menu_item_spec options_items[] = {
        //   { 70, "Compiler...", NULL, 1 },
       //    { 71, "Linker...", NULL, 1 },
           { 72, "Environment...", NULL, 1 },
           //  { 73, "Directories...", NULL, 1 },
    };
    add_menu(menubar, "Options", options_items, sizeof options_items / sizeof options_items[0]);

    static const menu_item_spec window_items[] = {
        { 80, "Tile", NULL, 1 },
        { 81, "Cascade", NULL, 1 },
        { 82, "Close all", NULL, 1 },
        SEP,
        { 84, "Refresh", "F5", 1 },
        SEP,
        { 85, "Font", "Ctrl +", 1 },
        { 86, "Font", "Ctrl -", 1 },
    };
    add_menu(menubar, "Window", window_items, sizeof window_items / sizeof window_items[0]);

    static const menu_item_spec help_items[] = {
        { 90, "Index", NULL, 1 },
        //{ 91, "Topic search", NULL, 1 },
        { EVT_HELP_CHECK, "Check", NULL, 1 },
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
     * real global shortcut too (see do_help_contextual()), unlike the
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

    /* Auto-compile on/off, mirrored from the Compile menu (id 44 /
     * EVT_TOGGLE_AUTOCOMPILE - both nodes share the id, so clicking either
     * one fires the same handler; see on_ui_event). Starts "Off" to match
     * g_auto_compile_enabled's default and the menu item's own initial
     * label - both are kept in sync from then on by the toggle handler. */
    ui_node* auto_item = ui_create_element(UI_TAG_HOTKEY);
    ui_set_id(auto_item, EVT_TOGGLE_AUTOCOMPILE);
    ui_set_label(auto_item, "Auto:Off");
    ui_append_child(statusbar, auto_item);
    g_statusbar_autocompile_item = auto_item;

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
static ui_node* g_env_modal;
static ui_node* g_env_theme_select;  /* the "Theme" <select> - refreshed to
                                      * match g_theme_index each time the
                                      * modal opens (see EVT_OPTIONS_ENV) */

                                      /* Compile > Options... dialog and the settings it edits. g_compile_options is
                                       * the raw text from the "Options" field (split on whitespace into argv tokens
                                       * at compile time); g_compile_target is the slug for the chosen Target, or ""
                                       * for none - both fed into do_compile()'s argv. */
static ui_node* g_copts_modal;
static ui_node* g_copts_input;
static ui_node* g_copts_target;
static char g_compile_options[512] = "";
static const char* g_target_slugs[] = {
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

#if defined(__APPLE__)
static const char* g_compile_target = "macos_arm64";
#elif defined(_WIN32)
static const char* g_compile_target = "x86_msvc";
#else
static const char* g_compile_target = "x86_x64_gcc";
#endif
static ui_node* g_output_window;
static ui_node* g_output_editor;

/* The persistent folder browser window - File > Open Folder... (via the
 * picker dialog, folder_select_confirm) or Window > Folder both just
 * re-raise this one singleton, same pattern as g_output_window above.
 * g_folder_dir is the directory it's currently showing (independent of the
 * Open dialog's own g_open_dir, which is only live while that dialog is
 * open). */
static ui_node* g_folder_window;
static ui_node* g_folder_listbox;
static char g_folder_dir[1024];

/* The Folder panel's own right-click popup - a "Filter" toggle (see
 * EVT_FOLDER_TOGGLE_FILTER) that flips g_folder_filter_enabled, plus a
 * second item that opens g_folder_dir's own CAKE_FOLDER_FILTER_NAME (see
 * EVT_FOLDER_SHOW_FILTER); opened over g_folder_listbox the same way
 * g_editor_popup opens over an editor (see app_frame()). g_folder_popup_filter
 * and g_folder_popup_show are those two items, whose labels are refreshed
 * each time the popup opens (refresh_folder_filter_item and
 * refresh_folder_show_filter_item), same pattern as the editor popup's
 * "Read-only" (refresh_readonly_item). */
static ui_node* g_folder_popup;
static ui_node* g_folder_popup_filter;
static ui_node* g_folder_popup_show;

/* Whether folder_window_refresh applies the browsed directory's own
 * index.txt (filtering the listing down to just what it lists, in that
 * order - see apply_index_order) - on by default. Unchecking "[x] Filter"
 * in the Folder panel's right-click popup turns it off, showing every entry
 * unfiltered instead, same as a folder with no index.txt always has. */
static int g_folder_filter_enabled = 1;

/* Tools > Find and Replace - a persistent docked panel (UI_DOCK_RIGHT), same
 * singleton pattern as g_folder_window/g_output_window above: raised again
 * instead of rebuilt every time the Tools menu item fires. Its content is
 * rebuilt from scratch (fr_rebuild_content, defined near do_find_replace
 * below) each time the mode toggles between Find and Replace, since the two
 * modes show a different set of fields - the g_fr_find_text..g_fr_file_type
 * fields below are what survives that rebuild (the widgets themselves don't).
 * fr_rebuild_content() reads g_fr_panel's CURRENT rect (ui_get_rect), not
 * whatever it was created with - it's a docked window, so ui_set_dock's first
 * layout pass (dock_layout, in ide_ui.c) relocates it once shown, and a node
 * built fresh after that has no shift history of its own to inherit; only
 * nodes that already existed at the time of that move get carried along with
 * it (see set_window_rect in ide_ui.c). */
static ui_node* g_fr_window;         /* wrapper <modal> passed to ui_screen_show_window */
static ui_node* g_fr_panel;          /* the <window> node itself */
#define FR_PANEL_MIN_W 26  /* dragging the dock border narrower than this
                            * would crush the Find/Replace buttons and
                            * checkboxes past usability - enforced each frame
                            * in app_frame() rather than left to the generic
                            * dock-resize clamp (which only floors at 1) */
static int g_fr_last_w;              /* the panel's width as of the last
                                      * fr_rebuild_content() call - app_frame()
                                      * compares against this each frame to
                                      * notice a border drag and re-lay-out the
                                      * controls to the new width */
static int g_fr_mode = 0;            /* 0 = Find, 1 = Replace */
static ui_node* g_fr_mode_btn;
static ui_node* g_fr_find_input;
static ui_node* g_fr_replace_input;  /* NULL while in Find mode (not built) */
static ui_node* g_fr_opts;           /* "Match case" / "Match whole word" - GROUP multi=1 */
static ui_node* g_fr_lookin;         /* "Current File"/"Current Dir"/"Include Dir" - GROUP multi=0 */
static ui_node* g_fr_filetypes;      /* "*.c" / "*.c;*.h" / "*.md" / "*.*" - SELECT */
static ui_node* g_fr_find_btn;
static ui_node* g_fr_replace_btn;    /* NULL while in Find mode (not built) */
static char g_fr_find_text[256] = "";
static char g_fr_replace_text[256] = "";
static int g_fr_match_case = 1;
static int g_fr_match_word = 0;
static int g_fr_look_in = 0;         /* index into "Look in" - see fr_look_in */
static int g_fr_file_type = 0;       /* index into "File Types" - see fr_file_type */

static ui_node* g_root;
static int g_new_count = 0;

/* Options > File > Open...'s dialog state - the directory currently being
 * browsed and the wildcard mask filtering its file rows (directories always
 * show regardless of the mask, same as a classic DOS-era file dialog). */
static ui_node* g_open_modal;
static ui_node* g_open_window;      /* the <window> node - title changes per mode */
static ui_node* g_open_name_input;
static ui_node* g_open_listbox;
static ui_node* g_open_ok;          /* the OK button - label changes per mode */
static ui_node* g_open_filter;      /* the "Files of type" <select> - see g_open_filters */
static ui_node* g_open_filter_label; /* its "Type" <text> label - shown/hidden together, see open_dialog_set_filter_visible */

/* The Open/Save dialog's "Files of type" options, Windows-Explorer style -
 * label shown in the dropdown paired with the mask it applies to
 * g_open_mask (see mask_matches). Picking one re-filters g_open_listbox and,
 * in File-Open mode, replaces the mask portion of the Name field too (see
 * open_dialog_refresh and the EVT_OPEN_FILTER handler). Index 0 is the
 * default selected when the dialog opens (see EVT_FILE_OPEN/EVT_FILE_SAVEAS). */
typedef struct { const char* label; const char* mask; } open_filter_entry;
static const open_filter_entry g_open_filters[] = {
    { "C Source Files (*.c)",    "*.c" },
    { "Header Files (*.h)",      "*.h" },
    { "C/C++ Sources (*.c;*.h)", "*.c;*.h" },
    { "Markdown Files (*.md)",   "*.md" },
    { "All Files (*.*)",         "*" },
};
#define OPEN_FILTER_COUNT ((int)(sizeof g_open_filters / sizeof g_open_filters[0]))

/* File > Save As... and File > Open Folder... both reuse the Open dialog
 * (see save_as_activate/folder_select_confirm) - g_open_dialog_mode picks
 * which behavior its Name field/listbox/OK button follow. In save mode the
 * OK button writes the active editor to the chosen path instead of opening a
 * file; g_save_window is the editor window being saved, g_save_name the bare
 * filename shown/edited in the Name field. In folder mode the listbox shows
 * only directories and OK confirms the current one (see open_dialog_refresh/
 * activate). */
typedef enum { OPEN_DLG_FILE, OPEN_DLG_SAVE, OPEN_DLG_FOLDER } open_dialog_mode;
static open_dialog_mode g_open_dialog_mode;
static ui_node* g_save_window;
static char g_save_name[300];
static char g_saveas_path[1024];  /* target path, pending the overwrite prompt */

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

/* Edit > Word Wrap...'s dialog: a single "Columns" <input>, prefilled with
 * the last value used (COLUMNS_DEFAULT the first time). OK reflows the
 * active editor's selection to that width - see do_edit_wordwrap(). */
#define WORDWRAP_COLUMNS_DEFAULT 80
static ui_node* g_wordwrap_modal;
static ui_node* g_wordwrap_input;
static int g_wordwrap_columns = WORDWRAP_COLUMNS_DEFAULT;

/* Search > Replace...'s dialog. The widget handles are kept so the OK/Change
 * All handlers can read what the user typed/checked; g_replace_target is the
 * editor window's editor captured when the dialog opened. */
static ui_node* g_replace_modal;
static ui_node* g_replace_find;    /* "Text to Find" <input> */
static ui_node* g_replace_new;     /* "New Text" <input> */
static ui_node* g_replace_opts;    /* Options check-box group */
static ui_node* g_replace_dir;     /* Direction radio group */
static ui_node* g_replace_scope;   /* Scope radio group */
static ui_node* g_replace_origin;  /* Origin radio group */
static ui_node* g_replace_target;  /* the <editor> being searched */

/* Search > Find...'s dialog - same shape as Replace minus the New Text field
 * (OK just locates and selects the next match, no replacement). */
static ui_node* g_find_modal;
static ui_node* g_find_input;
static ui_node* g_find_opts;
static ui_node* g_find_dir;
static ui_node* g_find_scope;
static ui_node* g_find_origin;
static ui_node* g_find_target;

/* Right-click context menu for the editor (see ui_screen_open_popup).
 * g_editor_popup_readonly is its "Read-only" item - its label/enabled state
 * is refreshed each time the popup opens (see app_frame()), since it reflects
 * whichever editor is under the popup, not a single fixed state. */
static ui_node* g_editor_popup;
static ui_node* g_editor_popup_readonly;
static ui_node* g_editor_popup_hdrsrc;  /* "Toggle Header/Source" item */
static ui_node* g_editor_popup_show_output;  /* "Show Generated Code" item -
                                              * disabled for a .md file, see
                                              * app_frame() */

/* Forward declaration: the <editor> inside a document window's wrapper. */
static ui_node* editor_in_window(ui_node* wrapper);

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

/* Options > Compile > Auto-compile toggle. Declared up here (not with the
 * rest of the auto-compile state further down) so open_dialog_activate can
 * see it - opening a file while it's on compiles that file immediately. */
static int g_auto_compile_enabled = 0;
static void do_compile(void);  /* defined below; called on auto-compile-open */
static char* dupstr(const char* s);  /* defined below; used by auto_compile_tick() */
static void open_playground(void);  /* defined below; called on View > "Show Playground" */
static const char* label_for_path(const char* path);  /* defined below; used wherever a
                                                        * path is reopened without going
                                                        * through open_playground() itself */
static char g_open_dir[1024] = ".";
static char g_open_mask[64] = "*.c";

#define OPEN_MAX_ENTRIES 512

/* The Folder panel's own per-folder filter file (see apply_index_order and
 * EVT_FOLDER_SHOW_FILTER) - deliberately not "index.txt": that's a plain,
 * common filename a real project folder might already use for its own
 * content, and this app would otherwise silently swallow it into the
 * filtering feature (and hide it from the listing). A dotfile-style name
 * only this feature would ever create keeps the two from colliding. */
#define CAKE_FOLDER_FILTER_NAME ".cakefilter"

/* Options > Environment...'s Theme select: "Classic" is just whatever
 * ui_get_theme() returns at startup, snapshotted once before anything could
 * change it, rather than a duplicated literal that could drift out of sync
 * with the framework's actual default. "Dark" and "White" mirror Visual
 * Studio's own Dark and Light (Blue-accented) themes' actual palette, and
 * prove ui_set_theme() really does re-theme the whole running app, not just
 * newly-created widgets. */
static ui_theme g_theme_classic;

/* Visual Studio Dark theme palette (editor #1E1E1E/#D4D4D4, chrome #2D2D30,
 * accent #007ACC). */
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
    .window_bg = TB_RGB(0x2D, 0x2D, 0x30),
    .modal_border_fg = TB_RGB(0xF1, 0xF1, 0xF1),
    .modal_border_bg = TB_RGB(0x2D, 0x2D, 0x30),
    .modal_border_style = UI_BORDER_DOUBLE,
    .modal_fg = TB_RGB(0xF1, 0xF1, 0xF1),
    .modal_bg = TB_RGB(0x2D, 0x2D, 0x30),
    .scrollbar_bg = TB_RGB(0x2D, 0x2D, 0x30),  /* matches the window border */
    .scrollbar_thumb_bg = TB_RGB(0x42, 0x42, 0x42),

    .input_bg = TB_RGB(0x1E, 0x1E, 0x1E),
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
    .editor_preproc_fg = TB_RGB(0xC5, 0x86, 0xC0),
    .editor_sel_bg = TB_RGB(0x26, 0x4F, 0x78),  /* #264F78 - VS Dark's actual selection color */
    .editor_sel_fg = TB_RGB(0xFF, 0xFF, 0xFF),
    .editor_current_line_bg = TB_RGB(0x2A, 0x2A, 0x2A),  /* subtle - close to
                                                          * VS Code Dark's own
                                                          * current-line tint */
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

    /* <listbox> - same body colors as <editor>/<input> rather than the
     * classic theme's cyan, selection reuses the same #007ACC accent as
     * every other "selected" state in this theme. */
    .listbox_fg = TB_RGB(0xD4, 0xD4, 0xD4),
    .listbox_bg = TB_RGB(0x1E, 0x1E, 0x1E),
    .listbox_sel_fg = TB_RGB(0xFF, 0xFF, 0xFF),
    .listbox_sel_bg = TB_RGB(0x00, 0x7A, 0xCC),

    /* <editor> inline diagnostics - VS Code Dark's actual error/warning/info
     * squiggle colors, so they read as authentically part of this theme. */
    .diag_error_fg = TB_RGB(0xF4, 0x47, 0x47),
    .diag_warning_fg = TB_RGB(0xCC, 0xA7, 0x00),
    .diag_info_fg = TB_RGB(0x37, 0x94, 0xFF),
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
    .menu_fg_sel = TB_RGB(0x00, 0x00, 0x00),
    .menu_bg_sel = TB_RGB(0xCC, 0xE8, 0xFF),
    .menu_item_fg = TB_RGB(0x1E, 0x1E, 0x1E),
    .menu_item_bg = TB_RGB(0xFF, 0xFF, 0xFF),
    .menu_item_fg_hot = TB_RGB(0x00, 0x00, 0x00),
    .menu_item_bg_hot = TB_RGB(0xCC, 0xE8, 0xFF),
    .menu_item_shortcut_fg = TB_RGB(0x00, 0x66, 0x99),
    .menu_item_fg_disabled = TB_RGB(0xA0, 0xA0, 0xA0),
    .menu_border_fg = TB_RGB(0xCC, 0xCC, 0xCC),  /* light gray, not near-black */
    .menu_border_bg = TB_RGB(0xFF, 0xFF, 0xFF),
    .menu_border_style = UI_BORDER_DOUBLE,

    .box_fg = TB_RGB(0x1E, 0x1E, 0x1E),
    .box_bg = TB_RGB(0xFF, 0xFF, 0xFF),
    .box_border_style = UI_BORDER_DOUBLE,

    .window_border_fg = TB_RGB(0x1E, 0x1E, 0x1E),
    .window_border_bg = TB_RGB(0xF3, 0xF3, 0xF3),
    .window_border_fg_dragging = TB_RGB(0x00, 0x7A, 0xCC),
    .window_border_fg_unfocused = TB_RGB(0xA0, 0xA0, 0xA0),
    .window_border_style = UI_BORDER_DOUBLE,
    .window_border_style_unfocused = UI_BORDER_SINGLE,
    .window_border_style_docked = UI_BORDER_DOUBLE,
    .window_border_style_docked_unfocused = UI_BORDER_SINGLE,
    .window_close_bg = TB_RGB(0xE8, 0x11, 0x23),
    .window_fg = TB_RGB(0x1E, 0x1E, 0x1E),
    .window_bg = TB_RGB(0xF3, 0xF3, 0xF3),
    .modal_border_fg = TB_RGB(0x1E, 0x1E, 0x1E),
    .modal_border_bg = TB_RGB(0xF3, 0xF3, 0xF3),
    .modal_border_style = UI_BORDER_DOUBLE,
    .modal_fg = TB_RGB(0x1E, 0x1E, 0x1E),
    .modal_bg = TB_RGB(0xF3, 0xF3, 0xF3),
    .scrollbar_bg = TB_RGB(0xF3, 0xF3, 0xF3),  /* matches the window border */
    .scrollbar_thumb_bg = TB_RGB(0xC2, 0xC2, 0xC2),

    .input_bg = TB_RGB(0xFF, 0xFF, 0xFF),
    .input_fg = TB_RGB(0x1E, 0x1E, 0x1E),
    .input_fg_focus = TB_RGB(0x00, 0x00, 0x00),
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
    .editor_preproc_fg = TB_RGB(0x80, 0x00, 0x80),
    .editor_sel_bg = TB_RGB(0xAD, 0xD6, 0xFF),  /* #ADD6FF - VS Light's actual selection color */
    .editor_sel_fg = TB_RGB(0x00, 0x00, 0x00),
    .editor_current_line_bg = TB_RGB(0xF0, 0xF0, 0xF0),  /* subtle - close to
                                                          * VS Code Light's own
                                                          * current-line tint */
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

    /* <listbox> - same body colors as <editor>/<input> rather than the
     * classic theme's cyan, selection reuses the same #CCE8FF accent as
     * every other "selected" state in this theme. */
    .listbox_fg = TB_RGB(0x1E, 0x1E, 0x1E),
    .listbox_bg = TB_RGB(0xFF, 0xFF, 0xFF),
    .listbox_sel_fg = TB_RGB(0x00, 0x00, 0x00),
    .listbox_sel_bg = TB_RGB(0xCC, 0xE8, 0xFF),

    /* <editor> inline diagnostics - VS Code Light's actual error/warning/
     * info squiggle colors, so they read as authentically part of this
     * theme. */
    .diag_error_fg = TB_RGB(0xE5, 0x14, 0x00),
    .diag_warning_fg = TB_RGB(0xBF, 0x88, 0x03),
    .diag_info_fg = TB_RGB(0x1A, 0x85, 0xFF),
};

/* Index into the Theme <select> (Classic=0/Dark=1/White=2 - matches the
 * add_select_item() order in app_init()) of whichever theme is currently
 * applied. Kept in sync by apply_theme()'s three callers below and read
 * back by EVT_OPTIONS_ENV to select the right row each time the dialog
 * opens, instead of always defaulting to "Classic". */
static int g_theme_index = 0;

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
static ui_node* make_editor_window(ui_node* root, int seq, const char* title,
                                    const char* content, const char* path)
{
    ui_node* wrapper = ui_create_element(UI_TAG_MODAL);
    ui_set_path(wrapper, path);
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
    ui_set_syntax(editor, syntax_for_path(path));
    ui_set_value(editor, content);
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
static void apply_theme(const ui_theme* theme)
{
    ui_set_theme(theme);
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
 * is always set programmatically, never opened from a file. */
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

/* Update the Folder panel popup's "Filter" item's label to reflect
 * g_folder_filter_enabled - same [x]/[ ] convention as refresh_readonly_item,
 * just with nothing to disable (unlike Read-only, this toggle is always
 * available). */
static void refresh_folder_filter_item(ui_node* item)
{
    if (!item)
        return;
    ui_set_label(item, g_folder_filter_enabled ? "[x] Filter" : "[ ] Filter");
}

/* Update the Folder panel popup's "Show Filter"/"Create Filter" item's
 * label to reflect whether g_folder_dir already has a
 * CAKE_FOLDER_FILTER_NAME on disk - "Show Filter" opens the existing one
 * (EVT_FOLDER_SHOW_FILTER's original behavior); "Create Filter" means that
 * same click will first generate one, pre-filled with the folder's current
 * contents (see create_default_filter_file), before opening it. Refreshed
 * each time the popup opens, same pattern as refresh_folder_filter_item. */
static void refresh_folder_show_filter_item(ui_node* item)
{
    if (!item)
        return;
    char path[1024];
    snprintf(path, sizeof path, "%s/%s", g_folder_dir, CAKE_FOLDER_FILTER_NAME);
    FILE* f = fopen(path, "rb");
    if (f)
    {
        fclose(f);
        ui_set_label(item, "Show Filter");
    }
    else
    {
        ui_set_label(item, "Create Filter");
    }
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

/* Strips the last path component from `dir` in place - "go up a directory".
 * Accepts either separator style, so a path the user typed with forward
 * slashes still navigates correctly. Does nothing if there's nowhere left
 * to go (already at a bare drive letter or root). */
static void open_path_up(char* dir)
{
    size_t len = strlen(dir);
    while (len > 0 && (dir[len - 1] == '/' || dir[len - 1] == '\\'))
        dir[--len] = 0;
    while (len > 0 && dir[len - 1] != '/' && dir[len - 1] != '\\')
        len--;
    if (len > 0)
        dir[len - 1] = 0;
}

/* Forward-declared here (defined further below, alongside the other small
 * file-reading helpers) so apply_index_order can use it without moving it
 * up the file. */
static char* read_file_to_string(const char* path);

/* ---- CAKE_FOLDER_FILTER_NAME (".cakefilter"): optional per-folder
 * filter+order for the persistent Folder browser window only
 * (folder_window_refresh, gated further by g_folder_filter_enabled - see
 * EVT_FOLDER_TOGGLE_FILTER) - never applied to the Open/Save-As/
 * folder-picker dialog (open_dialog_refresh), which is about opening/
 * saving one file, not browsing. Each line names one entry already on
 * disk - a plain file/subfolder name - and that's the only listing shown,
 * in that order (right after ".."): anything on disk this file doesn't
 * list is left out entirely, not merely sorted after. A line may also use
 * a "Title / name" form, similar in spirit to (but with a plain "/" instead
 * of " \ ", to keep the example any auto-created file leads with legible)
 * src/tools/help2md's own index.txt "Title \ name" syntax - a different,
 * unrelated file, see its own comment for why this one isn't also named
 * index.txt. When a title is present that's what the listbox displays for
 * that row (e.g. "Introduction" for "Introduction / introduction.md"), same
 * idea as help2md's own sidebar. Either way the row's
 * real disk name is preserved as its ui_set_path (see
 * populate_listbox_from_dir) so navigating into it or opening it is
 * unaffected by the display text. No filter file, or an unreadable one,
 * just means no filtering at all - every entry shows, same as before this
 * feature existed. Nothing here writes the file as a side effect of
 * filtering/browsing - unlike help2md's generator, newly-discovered entries
 * are never silently persisted back to disk. The one exception is
 * EVT_FOLDER_SHOW_FILTER itself: if the folder doesn't have one yet, the
 * popup item reads "Create Filter" instead of "Show Filter" (see
 * refresh_folder_show_filter_item), and clicking it generates one up front
 * - pre-filled with the folder's current contents via
 * create_default_filter_file - before opening it, so the user edits/prunes
 * a real starting point by hand instead of an empty file. */

/* Parses one filter-file line into an optional title and a required name -
 * same "Title / diskname" idea as help2md's own parse_idx_line, just with a
 * plain "/" delimiter as the current, preferred form (see
 * CAKE_FOLDER_FILTER_NAME's own doc comment for why the two files don't
 * have to match). The older " \ " delimiter (this format's own original
 * separator, before it switched to "/") is still recognized too, purely for
 * backward compatibility with filter files already on disk (e.g.
 * help/Manual's) written before the switch - whichever of the two appears
 * first in the line wins, so a line can't accidentally match both. `title`
 * is left as an empty string when the line has neither (bare name only).
 * Returns 0 for a blank line (nothing to place). */
static int idx_parse_line(const char* line, char* title, size_t title_size,
                           char* name, size_t name_size)
{
    while (*line == ' ' || *line == '\t')
        line++;
    if (*line == 0 || *line == '\r' || *line == '\n')
        return 0;

    title[0] = 0;
    const char* sep_slash = strstr(line, " / ");
    const char* sep_bslash = strstr(line, " \\ ");
    const char* sep;
    if (sep_slash && sep_bslash)
        sep = sep_slash < sep_bslash ? sep_slash : sep_bslash;
    else
        sep = sep_slash ? sep_slash : sep_bslash;
    const char* nm;
    if (sep)
    {
        size_t tlen = (size_t)(sep - line);
        while (tlen > 0 && (line[tlen - 1] == ' ' || line[tlen - 1] == '\t'))
            tlen--;
        if (tlen >= title_size)
            tlen = title_size - 1;
        memcpy(title, line, tlen);
        title[tlen] = 0;
        nm = sep + 3;
    }
    else
    {
        nm = line;
    }
    while (*nm == ' ' || *nm == '\t')
        nm++;

    size_t len = strlen(nm);
    while (len > 0 && (nm[len - 1] == '\r' || nm[len - 1] == '\n' ||
                        nm[len - 1] == ' ' || nm[len - 1] == '\t'))
        len--;
    if (len == 0)
        return 0;
    if (len >= name_size)
        len = name_size - 1;
    memcpy(name, nm, len);
    name[len] = 0;
    return 1;
}

/* Filters+reorders `names[0..count)` (each still carrying
 * populate_listbox_from_dir's trailing "\" marker for directories - the
 * real disk identity, unaffected by any of this) down to just what
 * `dir`/CAKE_FOLDER_FILTER_NAME lists, in that file's order (matched
 * against `names` with or without the "\", so the filter file itself never
 * needs to spell that out) - entries on disk but not listed are dropped
 * from the listing entirely, not merely reordered after. Fills
 * `titles[0..newcount)` with what should be displayed for each: the line's
 * title if it gave one (plus the same trailing "\" its disk name carries,
 * so a retitled folder still reads as one), otherwise its disk name as-is.
 * Returns the new, filtered count. If `dir` has no readable filter file,
 * `names`/`titles` are left untouched and `count` is returned as-is - no
 * filter file means no filtering, every entry still shows (see
 * populate_listbox_from_dir). Caller must pre-size both arrays the same
 * (OPEN_MAX_ENTRIES x 300, as populate_listbox_from_dir does). */
static int apply_index_order(const char* dir, char names[][300], char titles[][300], int count)
{
    char idx_path[1024];
    snprintf(idx_path, sizeof idx_path, "%s/%s", dir, CAKE_FOLDER_FILTER_NAME);
    char* text = read_file_to_string(idx_path);
    if (!text)
        return count;

    static char oname[OPEN_MAX_ENTRIES][300];
    static char otitle[OPEN_MAX_ENTRIES][300];
    static int used[OPEN_MAX_ENTRIES];
    memset(used, 0, (size_t)count * sizeof used[0]);
    int n_ordered = 0;

    char* line = text;
    while (line && *line)
    {
        char* nl = strchr(line, '\n');
        if (nl)
            *nl = 0;

        char title[300], want[300];
        if (idx_parse_line(line, title, sizeof title, want, sizeof want))
        {
            for (int i = 0; i < count; i++)
            {
                if (used[i])
                    continue;
                char bare[300];
                strncpy(bare, names[i], sizeof bare - 1);
                bare[sizeof bare - 1] = 0;
                size_t bl = strlen(bare);
                int is_dir = bl > 0 && bare[bl - 1] == '\\';
                if (is_dir)
                    bare[bl - 1] = 0;
                if (strcmp(bare, want) == 0)
                {
                    used[i] = 1;
                    strncpy(oname[n_ordered], names[i], sizeof oname[n_ordered] - 1);
                    oname[n_ordered][sizeof oname[n_ordered] - 1] = 0;
                    if (title[0])
                        snprintf(otitle[n_ordered], sizeof otitle[n_ordered], "%s%s",
                                 title, is_dir ? "\\" : "");
                    else
                    {
                        strncpy(otitle[n_ordered], names[i], sizeof otitle[n_ordered] - 1);
                        otitle[n_ordered][sizeof otitle[n_ordered] - 1] = 0;
                    }
                    n_ordered++;
                    break;
                }
            }
        }
        line = nl ? nl + 1 : NULL;
    }
    free(text);

    /* Unlike an earlier version of this function, entries index.txt doesn't
     * list are simply dropped here - no append-the-rest step - so the
     * Folder panel shows exactly index.txt's list and nothing else. */
    for (int i = 0; i < n_ordered; i++)
    {
        strncpy(names[i], oname[i], sizeof names[i] - 1);
        names[i][sizeof names[i] - 1] = 0;
        strncpy(titles[i], otitle[i], sizeof titles[i] - 1);
        titles[i][sizeof titles[i] - 1] = 0;
    }
    return n_ordered;
}

/* Rebuilds `listbox`'s rows from `dir`'s contents: ".." to go up, then every
 * subdirectory (marked with a trailing "\" - see dir_row_navigate, which
 * keys off that marker) and, unless `dirs_only`, every file whose name
 * matches `mask` (NULL/empty = no filtering, show them all) - dirs-first,
 * alphabetical (case-insensitive) within each group, unless `use_index_order`
 * asks to filter that down to just the folder's own CAKE_FOLDER_FILTER_NAME
 * (and, per-entry, retitle) on top of that (see apply_index_order above) -
 * true only for the persistent Folder browser window (folder_window_refresh,
 * itself gated by g_folder_filter_enabled); the Open/Save-As/folder-picker
 * dialog (open_dialog_refresh, below) always passes 0, so opening a file is
 * never affected. When a filter file exists, anything on disk it doesn't
 * list is left out of the listing entirely - it's the whole list, not a
 * partial order merged with the rest. No filter file (or an unreadable one)
 * just means no filtering - every entry still shows, same as before this
 * parameter existed. Either way, the filter file itself is also hidden from
 * the listing, same as help2md treats its own index.txt.
 *
 * Every row's real disk name (with the trailing "\" marker) is always kept
 * in ui_set_path, regardless of `use_index_order` - that's what
 * dir_row_navigate/folder_window_activate/open_dialog_activate key off of.
 * ui_set_label carries what's actually displayed, which is the same disk
 * name unless the filter file gave that entry a title (see
 * apply_index_order) - so a listed "Introduction \ introduction.md" shows
 * as "Introduction" in the Folder panel while still opening introduction.md
 * underneath. Shared by both callers. */
static void populate_listbox_from_dir(ui_node* listbox, const char* dir,
                                       const char* mask, int dirs_only,
                                       int use_index_order)
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
    static char titles[OPEN_MAX_ENTRIES][300];
    int count = 0;
    for (int i = 0; i < n; i++)
    {
        if (!raw[i].is_dir)
        {
            if (use_index_order && strcmp(raw[i].name, CAKE_FOLDER_FILTER_NAME) == 0)
                continue;  /* the filter file itself, never a listing entry */
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

    /* Default display = the disk name, until/unless index.txt overrides it
     * below - so a folder with no index.txt (or an entry it doesn't list)
     * looks exactly as it always has. */
    for (int i = 0; i < count; i++)
    {
        strncpy(titles[i], names[i], sizeof titles[i] - 1);
        titles[i][sizeof titles[i] - 1] = 0;
    }
    if (use_index_order && count > 0)
        count = apply_index_order(dir, names, titles, count);

    ui_node* up_item = ui_create_element(UI_TAG_ITEM);
    ui_set_label(up_item, "..\\");
    ui_set_path(up_item, "..\\");
    ui_append_child(listbox, up_item);

    for (int i = 0; i < count; i++)
    {
        ui_node* item = ui_create_element(UI_TAG_ITEM);
        ui_set_label(item, titles[i]);
        ui_set_path(item, names[i]);
        ui_append_child(listbox, item);
    }
    ui_select_set_selected(listbox, 0);
}

/* Writes `name` with its extension removed (the part from the last '.'
 * onward - a leading dot, as in a dotfile with no other '.', doesn't count,
 * so ".gitignore" is left whole rather than reduced to an empty title) into
 * `out`. Used only to build the "Title" half of create_default_filter_file's
 * generated "Title / name" lines - a folder or already-extensionless file
 * just gets its own name back unchanged. */
static void name_without_extension(const char* name, char* out, size_t out_size)
{
    const char* dot = strrchr(name, '.');
    size_t len = (dot && dot != name) ? (size_t)(dot - name) : strlen(name);
    if (len >= out_size)
        len = out_size - 1;
    memcpy(out, name, len);
    out[len] = 0;
}

/* Creates `dir`'s own CAKE_FOLDER_FILTER_NAME from scratch, pre-filled with
 * every entry currently on disk there, dirs-first-then-alphabetical - the
 * same order the Folder panel itself would show before any filtering (see
 * open_entry_cmp/populate_listbox_from_dir) - so "Create Filter"
 * (EVT_FOLDER_SHOW_FILTER when the file doesn't exist yet, see
 * refresh_folder_show_filter_item) hands the user a ready-made starting
 * point to prune/reorder/retitle by hand rather than an empty file. Every
 * line is written in the full "Title / name" form (see idx_parse_line/
 * CAKE_FOLDER_FILTER_NAME's own doc comment) - name_without_extension() as
 * the title, the real disk name as-is after it - rather than a bare name,
 * so the generated file itself demonstrates that syntax on real, live
 * entries instead of needing a separate made-up example line. Silently does
 * nothing if `dir` can't be listed or the file can't be written -
 * EVT_FOLDER_SHOW_FILTER's own open_file_path_into_editor still reports a
 * missing file exactly as it always has in that case. */
static void create_default_filter_file(const char* dir)
{
    static ui_dir_entry raw[OPEN_MAX_ENTRIES];
    int n = ui_list_dir(dir, raw, OPEN_MAX_ENTRIES);
    if (n <= 0)
        return;

    static char names[OPEN_MAX_ENTRIES][300];
    int count = 0;
    for (int i = 0; i < n; i++)
    {
        if (!raw[i].is_dir && strcmp(raw[i].name, CAKE_FOLDER_FILTER_NAME) == 0)
            continue;  /* never list the filter file inside itself */
        snprintf(names[count], sizeof names[count], "%s%s", raw[i].name,
                 raw[i].is_dir ? "\\" : "");
        count++;
    }
    if (count > 1)
        qsort(names, (size_t)count, sizeof names[0], open_entry_cmp);

    char path[1024];
    snprintf(path, sizeof path, "%s/%s", dir, CAKE_FOLDER_FILTER_NAME);
    FILE* f = fopen(path, "wb");
    if (!f)
        return;

    for (int i = 0; i < count; i++)
    {
        /* The trailing "\" is populate_listbox_from_dir's own disk-identity
         * marker (used above just to sort dirs first) - the filter file's
         * own format is a plain name either way (see CAKE_FOLDER_FILTER_NAME's
         * doc comment), so strip it back off before writing. */
        size_t len = strlen(names[i]);
        if (len > 0 && names[i][len - 1] == '\\')
            names[i][len - 1] = 0;

        char title[300];
        name_without_extension(names[i], title, sizeof title);
        fprintf(f, "%s / %s\n", title, names[i]);
    }
    fclose(f);
}

/* Rebuilds g_open_listbox's rows from g_open_dir (see
 * populate_listbox_from_dir), and refreshes the Name field to match -
 * called on first opening the dialog and after every navigation (up, into a
 * subdirectory, or a typed Name field). */
static void open_dialog_refresh(void)
{
    int folder_mode = g_open_dialog_mode == OPEN_DLG_FOLDER;
    /* use_index_order is always 0 here - the Open/Save-As/folder-picker
     * dialog is about opening/saving one file, never reordered by
     * index.txt even in folder-picker mode. See populate_listbox_from_dir. */
    populate_listbox_from_dir(g_open_listbox, g_open_dir,
                               folder_mode ? NULL : g_open_mask, folder_mode, 0);

    /* Cosmetic display only (backslashes regardless of platform, matching
     * this whole app's DOS/Windows IDE look) - never parsed back except
     * through EVT_OPEN_NAME, which accepts either separator style. Folder
     * mode shows just the directory - there's no mask/filename to append. */
    char display[1024];
    if (folder_mode)
        snprintf(display, sizeof display, "%s", g_open_dir);
    else
        snprintf(display, sizeof display, "%s/%s", g_open_dir,
                  g_open_dialog_mode == OPEN_DLG_SAVE ? g_save_name : g_open_mask);
    for (char* p = display; *p; p++)
        if (*p == '/')
            *p = '\\';
    ui_set_value(g_open_name_input, display);

    /* Select just the mask/filename portion (after the last backslash), not
     * the directory - so the user can start typing a new mask like *.md
     * straight away and have it replace rather than get appended to
     * whatever mask/filename was showing before (see ui_set_value, which
     * always leaves the cursor at the end with nothing selected). */
    const char* last_bslash = strrchr(display, '\\');
    int sel_start = last_bslash ? (int)(last_bslash - display + 1) : 0;
    ui_editor_set_selection(g_open_name_input, sel_start, (int)strlen(display));
}

/* Shows or hides the "Type" label + files-of-type <select> as a pair.
 * Folder mode has no mask concept at all (populate_listbox_from_dir gets a
 * NULL mask there - see open_dialog_refresh), so rather than just disabling
 * the control - which would still sit there looking live - it's detached
 * from the dialog entirely. Always detaches first, which is a no-op if it
 * isn't currently attached (see ui_remove_child), so this is safe to call
 * regardless of the dialog's previous mode. */
static void open_dialog_set_filter_visible(int visible)
{
    ui_remove_child(g_open_window, g_open_filter_label);
    ui_remove_child(g_open_window, g_open_filter);
    if (visible)
    {
        ui_append_child(g_open_window, g_open_filter_label);
        ui_append_child(g_open_window, g_open_filter);
        ui_set_enabled(g_open_filter, 1);
    }
}

/* An already-open editor window backed by `path`, or NULL - see
 * find_open_window() (defined after editor_in_window, which it needs). */
static ui_node* find_open_window(const char* path);

/* The <editor> inside a document window's <modal> wrapper - see
 * editor_in_window() (defined further below; forward-declared so
 * open_link_in_window()/do_editor_ctrlclick() above it can call it too). */
static ui_node* editor_in_window(ui_node* wrapper);

/* File > Open Folder...'s OK/"Select" confirmation - defined with the rest
 * of the persistent folder browser window, forward-declared so the main
 * event dispatcher (which comes first) can call it from EVT_OPEN_OK. */
static void folder_select_confirm(void);

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
  * (g_open_dir vs g_folder_dir). */
static int dir_row_navigate(char* dir_buf, size_t dir_buf_size, const char* label)
{
    size_t len = strlen(label);
    if (len == 0 || label[len - 1] != '\\')
        return 0;

    char name[300];
    strncpy(name, label, sizeof name - 1);
    name[sizeof name - 1] = 0;
    name[strlen(name) - 1] = 0;  /* drop the trailing "\" marker */

    if (strcmp(name, "..") == 0)
    {
        open_path_up(dir_buf);
    }
    else
    {
        char new_dir[1024];
        snprintf(new_dir, sizeof new_dir, "%s/%s", dir_buf, name);
        strncpy(dir_buf, new_dir, dir_buf_size - 1);
        dir_buf[dir_buf_size - 1] = 0;
    }
    return 1;
}

/* Opens `path` (labeled `label` for the new window's title) into a document
 * editor window - already open? just re-raises it instead of duplicating.
 * Shared by open_dialog_activate (which also closes the Open dialog
 * afterward - not this function's concern) and folder_window_activate
 * (which has no modal to close, the folder window isn't one). Also fires an
 * auto-compile if that's enabled, same as opening a file always has. */
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
    }
    fclose(f);

    char title[300];
    snprintf(title, sizeof title, " %s ", label);
    ui_screen_show_window(g_screen,
        make_editor_window(g_root, g_new_count++, title, content ? content : "", path));
    free(content);

    /* Compile the freshly opened file right away (it's now the active/
     * frontmost window) - opening it counts as wanting to see its
     * diagnostics, even though nothing was typed. */
    if (g_auto_compile_enabled)
        do_compile();
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
static nav_pos g_nav_back[NAV_STACK_MAX];
static int g_nav_back_count = 0;
static nav_pos g_nav_fwd[NAV_STACK_MAX];
static int g_nav_fwd_count = 0;

/* Set while nav_back()/nav_forward() are themselves the ones moving the
 * caret, so the jump they perform isn't mistaken for a brand new explicit
 * jump and re-recorded - that would bulldoze the very stack being walked. */
static int g_nav_restoring = 0;

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
 * g_nav_restoring (see above) or if there's nothing to capture. */
static void nav_record_jump(void)
{
    if (g_nav_restoring)
        return;
    nav_pos here;
    if (!nav_capture(&here))
        return;
    nav_push(g_nav_back, &g_nav_back_count, &here);
    g_nav_fwd_count = 0;
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
    if (g_nav_back_count == 0)
        return;
    nav_pos here;
    int have_here = nav_capture(&here);

    nav_pos target = g_nav_back[--g_nav_back_count];
    if (have_here)
        nav_push(g_nav_fwd, &g_nav_fwd_count, &here);

    g_nav_restoring = 1;
    nav_restore(&target);
    g_nav_restoring = 0;
}

static void nav_forward(void)
{
    if (g_nav_fwd_count == 0)
        return;
    nav_pos here;
    int have_here = nav_capture(&here);

    nav_pos target = g_nav_fwd[--g_nav_fwd_count];
    if (have_here)
        nav_push(g_nav_back, &g_nav_back_count, &here);

    g_nav_restoring = 1;
    nav_restore(&target);
    g_nav_restoring = 0;
}

static void open_dialog_activate(int index)
{
    if (index < 0 || index >= ui_child_count(g_open_listbox))
        return;
    const char* label = ui_get_label(ui_child_at(g_open_listbox, index));
    if (!label[0])
        return;

    if (dir_row_navigate(g_open_dir, sizeof g_open_dir, label))
    {
        open_dialog_refresh();
        return;
    }

    /* Save mode: picking a file row doesn't open it - it drops that name into
     * the Name field to overwrite (the user still confirms via Save). */
    if (g_open_dialog_mode == OPEN_DLG_SAVE)
    {
        strncpy(g_save_name, label, sizeof g_save_name - 1);
        g_save_name[sizeof g_save_name - 1] = 0;
        open_dialog_refresh();
        return;
    }

    char path[1024];
    snprintf(path, sizeof path, "%s/%s", g_open_dir, label);
    nav_record_jump();
    open_file_path_into_editor(path, label);
    ui_screen_close_modal(g_screen, g_open_modal);
}

/* Rebuilds the persistent folder browser window's listbox from g_folder_dir
 * (only source files - see the mask - plus all subdirectories for
 * navigation, see populate_listbox_from_dir), and retitles the window to
 * show the directory it's now showing. use_index_order tracks
 * g_folder_filter_enabled - the Folder panel's own right-click "Filter"
 * toggle (see EVT_FOLDER_TOGGLE_FILTER) - rather than always being on, so
 * the user can turn g_folder_dir's own CAKE_FOLDER_FILTER_NAME filtering
 * off and see everything unfiltered instead. */
static void folder_window_refresh(void)
{
    populate_listbox_from_dir(g_folder_listbox, g_folder_dir, "*.h;*.c;*.md", 0,
                               g_folder_filter_enabled);

    /* Just the folder's own name, not the full path - there's no room for
     * that in the title bar. */
    ui_node* window = ui_child_at(g_folder_window, 0);
    if (window)
    {
        char title[320];
        snprintf(title, sizeof title, " %s ", basename_of(g_folder_dir));
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
    if (index < 0 || index >= ui_child_count(g_folder_listbox))
        return;
    const char* entry = ui_get_path(ui_child_at(g_folder_listbox, index));
    if (!entry[0])
        return;

    if (dir_row_navigate(g_folder_dir, sizeof g_folder_dir, entry))
    {
        folder_window_refresh();
        return;
    }

    char path[1024];
    snprintf(path, sizeof path, "%s/%s", g_folder_dir, entry);
    nav_record_jump();
    open_file_path_into_editor(path, entry);
}

/* File > Open Folder...'s OK/"Select" confirmation (EVT_OPEN_OK while
 * g_open_dialog_mode == OPEN_DLG_FOLDER): point the singleton folder browser
 * window at whichever directory the picker had navigated to, close the
 * picker, and raise it. */
static void folder_select_confirm(void)
{
    strncpy(g_folder_dir, g_open_dir, sizeof g_folder_dir - 1);
    g_folder_dir[sizeof g_folder_dir - 1] = 0;
    folder_window_refresh();

    ui_screen_close_modal(g_screen, g_open_modal);
    g_open_dialog_mode = OPEN_DLG_FILE;
    ui_screen_show_window(g_screen, g_folder_window);
}

/* Points the persistent Folder panel at `dir` and raises it - shared by the
 * editor popup's "Show My Folder" (EVT_EDITOR_SHOW_FOLDER) and F1's contextual
 * help (do_help_contextual), so both land the user on the relevant folder
 * without a separate File > Open Folder... trip. Same "write g_folder_dir
 * directly, then refresh+show" idiom folder_select_confirm uses above, just
 * without an Open dialog to close first - neither caller has one open. A
 * no-op for a NULL/empty dir (e.g. a document with no path yet). */
static void folder_reveal_directory(const char* dir)
{
    if (!dir || !dir[0])
        return;
    strncpy(g_folder_dir, dir, sizeof g_folder_dir - 1);
    g_folder_dir[sizeof g_folder_dir - 1] = 0;
    folder_window_refresh();
    ui_screen_show_window(g_screen, g_folder_window);
}

/* Help > Index (EVT_HELP_INDEX): opens the help index from the executable's
 * help directory, so the help content travels with the app. Already open?
 * Just re-raise it, same as File > Open would. Missing? A message box
 * instead of silently doing nothing. */
static void do_help_index(void)
{
    nav_record_jump();  /* opening help counts as an explicit jump too - see
                         * nav_record_jump()'s own doc comment */

    char dir[1024] = { 0 };
    char exe_path[1024] = { 0 };

    if (!get_self_path(exe_path, sizeof exe_path))
    {
        strncpy(dir, exe_path, sizeof dir - 1);
        dir[sizeof dir - 1] = 0;
        dirname(dir);
    }

    if (!dir[0] && !ui_get_cwd(dir, sizeof dir))
        dir[0] = 0;

    char path[1024];
    snprintf(path, sizeof path, "%s/help/index.md", dir);

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
        snprintf(message, sizeof message, "help/index.md not found at:\n%s", path);
        ui_message_box(g_screen, "Help", message, &ok, 1);
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

    ui_node* win = make_editor_window(g_root, g_new_count++, " help/index.md ", content ? content : "", path);
    free(content);

    ui_node* editor = editor_in_window(win);
    if (editor)
        ui_set_read_only(editor, 1);  /* help/index.md is reference text, never hand-edited */

    ui_screen_show_window(g_screen, win);
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

/* F1 - contextual help. Looks up the identifier under the caret (see
 * word_at_cursor) as its own topic file, help/<word>.md, resolved the same
 * way do_help_index() finds help/index.md; if the help set has no such
 * topic, falls back to the general index, still trying to land on a
 * matching "# <word>" heading in it rather than just dumping the reader at
 * the top. No document focused, or nothing under the caret? Same as
 * pressing Help > Index. */

/* Defined later, next to the rest of the session/config-path plumbing it
 * reports on (see get_session_file_path) - forward declared so on_ui_event
 * (far above that point in the file) can reach it. */
static void do_help_check(void);

static void do_help_contextual(void)
{
    char dir[1024] = { 0 };
    char exe_path[1024] = { 0 };
    if (!get_self_path(exe_path, sizeof exe_path))
    {
        strncpy(dir, exe_path, sizeof dir - 1);
        dir[sizeof dir - 1] = 0;
        dirname(dir);
    }
    if (!dir[0] && !ui_get_cwd(dir, sizeof dir))
        dir[0] = 0;

    /* Whichever help file F1 lands on below - a dedicated topic, a matched
     * heading in the general index, or the index itself - always lives in
     * this same help/ subfolder, so reveal it in the Folder panel right
     * away rather than duplicating this call in all three outcomes.
     * Shared with the editor popup's "Show My Folder" - see
     * folder_reveal_directory(). */
    char help_dir[1024];
    snprintf(help_dir, sizeof help_dir, "%s/help", dir);
    folder_reveal_directory(help_dir);

    ui_node* win = g_active_editor_window;
    ui_node* ed = win ? editor_in_window(win) : NULL;

    char word[128];
    int have_word = 0;
    if (ed)
    {
        const char* text = ui_get_value(ed);
        int cursor = ui_editor_get_cursor(ed);
        have_word = word_at_cursor(text, (int)strlen(text), cursor, word, (int)sizeof word);
    }

    if (!have_word)
    {
        do_help_index();
        return;
    }

    /* A dedicated help/<word>.md topic, if the help set has one. */
    char topic_path[1024];
    snprintf(topic_path, sizeof topic_path, "%s/help/%s.md", dir, word);
    FILE* tf = fopen(topic_path, "rb");
    if (tf)
    {
        fclose(tf);
        nav_record_jump();
        open_file_path_into_editor(topic_path, word);
        /* Reference text, like help/index.md - never hand-edited. */
        ui_node* topic_win = find_open_window(topic_path);
        ui_node* topic_ed = topic_win ? editor_in_window(topic_win) : NULL;
        if (topic_ed)
            ui_set_read_only(topic_ed, 1);
        return;
    }

    /* No dedicated topic - read the general index looking for a matching
     * "# word" heading (case-insensitive, exact text after the "# ") to
     * jump straight to, same ATX-heading shape render_editor_line_markdown
     * recognizes for coloring. */
    char index_path[1024];
    snprintf(index_path, sizeof index_path, "%s/help/index.md", dir);
    FILE* f = fopen(index_path, "rb");
    if (!f)
    {
        do_help_index();  /* lets it report "not found", same as Help > Index */
        return;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size < 0)
        size = 0;
    char* content = malloc((size_t)size + 1);
    int match_line = 0;
    if (content)
    {
        size_t got = fread(content, 1, (size_t)size, f);
        content[got] = 0;

        int line = 1;
        for (const char* p = content; *p; )
        {
            const char* eol = strchr(p, '\n');
            int llen = eol ? (int)(eol - p) : (int)strlen(p);
            int j = 0;
            while (j < llen && p[j] == '#')
                j++;
            if (j > 0 && j < llen && p[j] == ' ')
            {
                char heading[128];
                int hlen = llen - j - 1;
                if (hlen >= (int)sizeof heading)
                    hlen = sizeof heading - 1;
                memcpy(heading, p + j + 1, hlen);
                heading[hlen] = 0;
                if (ci_strcmp(heading, word) == 0)
                {
                    match_line = line;
                    break;
                }
            }
            p = eol ? eol + 1 : p + llen;
            line++;
        }
        free(content);
    }
    fclose(f);

    do_help_index();  /* opens/re-raises help/index.md; records the jump */
    if (match_line > 0)
    {
        ui_node* idxwin = find_open_window(index_path);
        ui_node* idxed = idxwin ? editor_in_window(idxwin) : NULL;
        if (idxed)
            ui_editor_goto_line(idxed, match_line);
    }
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

    if (!dir[0] && g_folder_dir[0])
    {
        strncpy(dir, g_folder_dir, sizeof dir - 1);
        dir[sizeof dir - 1] = 0;
    }

    if (!dir[0])
        ui_get_cwd(dir, sizeof dir);

    ui_open_terminal(dir);
}

/* Reads `link` (resolved relative to `win`'s own directory, or the app's
 * working directory if `win` has none) and replaces `win`'s <editor> content
 * with it in place - unlike File > Open/Help > Index, this never opens a new
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
    snprintf(path, sizeof path, "%s/%s", dir, link);

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

/* Runs compile(), capturing whatever it printf's to stdout along the way -
 * it has no other channel back to us for that text. A GUI-subsystem build
 * (win32.c links -mwindows) has no console, so stdout's underlying file
 * descriptor is invalid from the very start of the process - redirecting
 * fd 1 via dup2 doesn't help, since the CRT's stdout stream object already
 * recorded that invalid descriptor and every printf() using it short-
 * circuits before ever reaching a real fd. freopen() sidesteps that by
 * fully reassociating the stream with a real file, discarding whatever fd
 * (valid or not) it had before - the portable way to do this regardless of
 * whether a console is attached. Returns a calloc'd buffer - caller's to
 * free(). */
static char* capture_and_compile(int argc, const char** argv, struct report* report)
{
    char* buf = calloc(CAPTURE_BUF_SIZE, 1);

    fflush(stdout);
    freopen(CAPTURE_TMP_FILE, "w+", stdout);

    compile(argc, argv, report);

    fflush(stdout);
    long len = ftell(stdout);
    if (len < 0)
        len = 0;
    if (len > CAPTURE_BUF_SIZE - 1)
        len = CAPTURE_BUF_SIZE - 1;
    rewind(stdout);
    size_t n = fread(buf, 1, (size_t)len, stdout);
    buf[n] = 0;

#ifdef _WIN32
    freopen("NUL", "w", stdout);
#else
    freopen("/dev/null", "w", stdout);
#endif
    remove(CAPTURE_TMP_FILE);

    return buf;
}

/* An editor window's only child is its <window>, whose only child is its
 * <editor> - see make_editor_window(). Direct indexing rather than a
 * type-searching walk since we control that exact shape. */
static ui_node* editor_in_window(ui_node* wrapper)
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
static int is_editor_window(ui_node* wrapper)
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
static void save_active_file(ui_node* active)
{
    ui_node* editor = editor_in_window(active);
    const char* path = active ? ui_get_path(active) : "";
    if (!editor || !path[0])
        return;

    FILE* f = fopen(path, "wb");
    if (!f)
        return;
    const char* content = ui_get_value(editor);
    fwrite(content, 1, strlen(content), f);
    fclose(f);
    ui_set_dirty(editor, 0);
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

    ui_screen_close_modal(g_screen, g_open_modal);
    g_open_dialog_mode = OPEN_DLG_FILE;

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
    int idx = ui_select_get_selected(g_open_filter);
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
        ui_screen_close_modal(g_screen, g_open_modal);
        g_open_dialog_mode = OPEN_DLG_FILE;
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
    strncpy(buf, ui_get_value(g_open_name_input), sizeof buf - 1);
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
            strncpy(g_open_dir, buf, sizeof g_open_dir - 1);
            g_open_dir[sizeof g_open_dir - 1] = 0;
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

    snprintf(g_saveas_path, sizeof g_saveas_path, "%s/%s", g_open_dir, g_save_name);

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
    g_open_dialog_mode = OPEN_DLG_SAVE;

    /* Default the Name to the window's current file, and start browsing in
     * that file's own directory when it has one. */
    const char* cur = ui_get_path(win);
    const char* base = basename_of(cur);
    strncpy(g_save_name, base, sizeof g_save_name - 1);
    g_save_name[sizeof g_save_name - 1] = 0;

    if (base > cur)
    {
        size_t dlen = (size_t)(base - cur - 1);
        if (dlen > 0 && dlen < sizeof g_open_dir)
        {
            memcpy(g_open_dir, cur, dlen);
            g_open_dir[dlen] = 0;
        }
        else if (!ui_get_cwd(g_open_dir, sizeof g_open_dir))
            strcpy(g_open_dir, ".");
    }
    else if (!ui_get_cwd(g_open_dir, sizeof g_open_dir))
    {
        strcpy(g_open_dir, ".");
    }
    strcpy(g_open_mask, g_open_filters[0].mask);
    ui_select_set_selected(g_open_filter, 0);
    open_dialog_set_filter_visible(1);

    ui_set_label(g_open_window, " Save File As ");
    ui_set_label(g_open_ok, "  Save  ");
    open_dialog_refresh();
    ui_screen_show_modal(g_screen, g_open_modal);
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
 * (do_editor_ctrlclick), or a Tools > Find and Replace "Current Dir" result,
 * which only ever prints bare names (see fr_search_text) - against the
 * directories it most plausibly came from: the active document's own
 * folder, then the persistent Folder panel's browsed directory, before
 * falling back to `filename` as-is (already a real openable path for e.g. a
 * compiler diagnostic, which carries a real path). Writes the resolved path
 * into `out` (up to out_cap bytes, always 0-terminated). */
static void resolve_referenced_path(const char* filename, char* out, size_t out_cap)
{
    char candidate[1024];
    const char* open_path = NULL;

    if (g_active_editor_window)
    {
        char dir[1024];
        strncpy(dir, ui_get_path(g_active_editor_window), sizeof dir - 1);
        dir[sizeof dir - 1] = 0;
        dirname(dir);
        snprintf(candidate, sizeof candidate, "%s/%s", dir, filename);
        if (file_readable(candidate))
            open_path = candidate;
    }
    if (!open_path && g_folder_dir[0])
    {
        snprintf(candidate, sizeof candidate, "%s/%s", g_folder_dir, filename);
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

    /* The filename ends at the first ':' that's followed by a digit - so a
     * Windows drive letter ("C:") isn't mistaken for the separator - and the
     * line number is the digits right after it. */
    char* colon = NULL;
    for (char* p = line; *p; p++)
        if (*p == ':' && isdigit((unsigned char)p[1]))
        {
            colon = p;
            break;
        }
    if (!colon)
        return;
    *colon = '\0';
    int src_line = atoi(colon + 1);
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

static int parse_diagnostic_line(char* line, ui_diag_type* type, int* out_line, char** message)
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

    // 2. Extract source line number from the "file:line:col" prefix
    // Find the first colon that is followed by a digit (skips drive letters)
    const char* p = line;
    const char* line_start = NULL;
    while (*p)
    {
        if (*p == ':' && isdigit((unsigned char)*(p + 1)))
        {
            line_start = p + 1; // point to the first digit of the line number
            break;
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
    // Skip keyword itself, then any whitespace, then optional digits (error code),
    // then whitespace and a colon, then the actual message.
    const char* msg_start = kw_pos + strlen(keywords[found_type]);
    while (*msg_start == ' ' || *msg_start == '\t') msg_start++;
    while (isdigit((unsigned char)*msg_start)) msg_start++;  // skip error code (nonexistent for "note")
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
    return 1;
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
static void do_compile(void)
{
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

    /* Build argv from the Compile > Options... settings: "tcc", the target
     * flag (when one is picked), the option tokens (split on whitespace), then
     * the file. argv[] and optbuf are sized well past any realistic option
     * count/length; excess tokens are simply dropped. */
    const char* argv[64];
    int argc = 0;
    argv[argc++] = "tcc";
    char target[20] = { 0 };
    if (g_compile_target[0])
    {
        snprintf(target, sizeof target, "-target=%s", g_compile_target);
        argv[argc++] = target;
    }
    char optbuf[sizeof g_compile_options];
    snprintf(optbuf, sizeof optbuf, "%s", g_compile_options);

    for (char* tok = strtok(optbuf, " \t"); tok && argc < 63;
         tok = strtok(NULL, " \t"))
    {
        argv[argc++] = tok;
    }
    argv[argc++] = file;

    char* captured = capture_and_compile(argc, argv, &report);

    char summary[256];
    snprintf(summary, sizeof summary, "\n%d error(s), %d warning(s), %.2f sec\n",
              report.error_count, report.warnings_count, report.cpu_time_used_sec);

    static char full[CAPTURE_BUF_SIZE + sizeof summary];
    snprintf(full, sizeof full, "%s%s", captured, summary);

    /* Re-parse the same captured text (before it's freed) into the active
     * editor's diagnostic list - see parse_diagnostic_line(). Cleared first
     * so every compile starts from a blank slate instead of accumulating
     * stale diagnostics on top of a previous run's. strtok mutates
     * `captured` in place, which is fine - `full` above already has its
     * own independent copy of the text. */
    ui_node* src_editor = editor_in_window(active);
    if (src_editor)
    {
        ui_editor_clear_diagnostics(src_editor);
        for (char* line = strtok(captured, "\n"); line; line = strtok(NULL, "\n"))
        {
            ui_diag_type type;
            int diag_line;
            char* message;
            if (parse_diagnostic_line(line, &type, &diag_line, &message))
                ui_editor_add_diagnostic(src_editor, type, diag_line, message);
        }
    }
    free(captured);

    ui_set_value(g_output_editor, full);
    ui_screen_show_window(g_screen, g_output_window);

    /* ui_screen_show_window() promotes whatever it's given to the front and
     * clears focus - Compile must never steal the editor's keyboard focus
     * or hand its "active window" status (which F7/auto-compile target -
     * see ui_screen_top_window) over to Output, so immediately hand both
     * back. Output stays open, just no longer frontmost/focused. */
    if (active)
    {
        ui_screen_show_window(g_screen, active);
        ui_node* editor = editor_in_window(active);
        if (editor)
            ui_screen_focus(g_screen, editor);
    }

    /* Pick up any on-disk changes the build produced (leaves dirty windows
     * and the caret positions untouched - see refresh_open_windows). */
    refresh_open_windows();
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

static char* dupstr(const char* s)
{
    size_t len = strlen(s) + 1;
    char* out = malloc(len);
    if (out)
        memcpy(out, s, len);
    return out;
}

/* Options > Compile > "Auto-compile" toggle. A plain time(NULL) (1-second
 * resolution, standard C, no per-backend clock needed) is precise enough
 * for "wait a bit after the last keystroke, then compile" - no multi-
 * platform timer required. */
#define AUTO_COMPILE_DELAY_SEC 1

static ui_node* g_auto_compile_window;
static char* g_auto_compile_snapshot;
static time_t g_auto_compile_last_change;

/* Called every frame from app_frame(). Debounces on content actually
 * changing (dirty alone can't tell us *when* the last edit happened - it
 * stays set from the first keystroke until the next save) and only fires
 * once per burst of edits, since do_compile() -> save_active_file() clears
 * dirty and the content stops changing, so the guard condition goes false
 * right after and stays false until the next edit sets dirty again. */
static void auto_compile_tick(void)
{
    if (!g_auto_compile_enabled)
        return;

    /* g_active_editor_window, not ui_screen_top_window() - same reasoning as
     * do_compile() above: a docked Folder/Output panel can be frontmost
     * instead, and neither has a real path or a document <editor> to check
     * dirty on. */
    ui_node* active = g_active_editor_window;
    ui_node* editor = editor_in_window(active);

    /* Still untitled (see ui_get_untitled) - the idle auto-compile timer
     * must never pop the Save As dialog on its own just because the user
     * paused typing in a brand new, never-saved file; that only happens for
     * an explicit Run > Compile (see do_compile). Treated the same as "no
     * editor" below - stop tracking it until it either becomes a real file
     * (Save/Save As) or the user switches away. */
    if (!editor || ui_get_untitled(active))
    {
        free(g_auto_compile_snapshot);
        g_auto_compile_snapshot = NULL;
        g_auto_compile_window = NULL;
        return;
    }

    const char* content = ui_get_value(editor);

    /* Switched to a different window - start tracking it fresh rather than
     * comparing its content against whatever the previously active window
     * last held, which would misfire as "changed". */
    if (active != g_auto_compile_window)
    {
        free(g_auto_compile_snapshot);
        g_auto_compile_snapshot = dupstr(content);
        g_auto_compile_window = active;
        g_auto_compile_last_change = time(NULL);
        return;
    }

    if (!g_auto_compile_snapshot || strcmp(g_auto_compile_snapshot, content) != 0)
    {
        free(g_auto_compile_snapshot);
        g_auto_compile_snapshot = dupstr(content);
        g_auto_compile_last_change = time(NULL);
        return;
    }

    if (ui_get_dirty(editor) && time(NULL) - g_auto_compile_last_change >= AUTO_COMPILE_DELAY_SEC)
        do_compile();
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

/* Read the dialog's options into *ci/*whole and the search range into
 * *lo/*hi (the whole document, or the current selection for "Selected text"
 * scope). Returns the editor, or NULL if there's nothing to search. */
static ui_node* fr_setup(int* ci, int* whole, int* lo, int* hi)
{
    ui_node* ed = g_replace_target;
    if (!ed)
        return NULL;
    *ci = !ui_group_get_checked(g_replace_opts, 0);  /* Case sensitive off => ignore case */
    *whole = ui_group_get_checked(g_replace_opts, 1);
    int len = (int)strlen(ui_get_value(ed));
    *lo = 0;
    *hi = len;
    if (ui_select_get_selected(g_replace_scope) == 1)
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
static void fill_from_selection(ui_node* ed, ui_node* input)
{
    int lo, hi;
    if (!ed || !ui_editor_get_selection(ed, &lo, &hi))
        return;
    const char* text = ui_get_value(ed);
    char buf[256];
    int len = hi - lo;
    if (len >= (int)sizeof buf)
        len = sizeof buf - 1;
    memcpy(buf, text + lo, len);
    buf[len] = '\0';
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
    if (g_find_target)
        found = fr_find_in_editor(g_find_target, ui_get_value(g_find_input),
                                  g_find_opts, g_find_dir, g_find_scope, g_find_origin);
    ui_screen_close_modal(g_screen, g_find_modal);
    if (found)
    {
        g_goto_pending_focus = g_find_target;  /* focus so the selection shows */
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
    const char* needle = ui_get_value(g_find_input);
    if (!ed || !needle[0])
    {
        g_find_target = ed;
        ui_screen_show_modal(g_screen, g_find_modal);
        return;
    }
    if (fr_find_in_editor(ed, needle, g_find_opts, g_find_dir, g_find_scope, g_find_origin))
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
    const char* needle = ui_get_value(g_replace_find);
    int nlen = (int)strlen(needle);
    if (ed && nlen > 0)
    {
        const char* newtext = ui_get_value(g_replace_new);
        int rlen = (int)strlen(newtext);
        const char* text = ui_get_value(ed);
        int len = (int)strlen(text);

        int fwd = ui_select_get_selected(g_replace_dir) == 0;
        int from_cursor = ui_select_get_selected(g_replace_origin) == 0;
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
    ui_screen_close_modal(g_screen, g_replace_modal);
    if (ed)
        g_goto_pending_focus = ed;  /* focus so the selection/caret shows */
}

/* Change All: replace every occurrence within the scope, then report the
 * count with a MessageBox. */
static void do_replace_all(void)
{
    int ci, whole, lo, hi;
    ui_node* ed = fr_setup(&ci, &whole, &lo, &hi);
    const char* needle = ui_get_value(g_replace_find);
    int nlen = (int)strlen(needle);
    int count = 0;
    if (ed && nlen > 0)
    {
        const char* newtext = ui_get_value(g_replace_new);
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
    ui_screen_close_modal(g_screen, g_replace_modal);

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
    return content;
}

/* --- Tools > Find and Replace ----------------------------------------------
 * A dockable panel (see g_fr_window/g_fr_panel above), distinct from the
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
} fr_look_in;

typedef enum {
    FR_FILETYPE_C = 0,
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
 * reading+searching one file at a time here needs no such cap. Returns the
 * total match count across every file searched. */
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

    DIR* d = opendir(dir);
    if (!d)
    {
        snprintf(out, out_size, "Could not open directory: %s\n", dir);
        return 0;
    }

    const char* mask = fr_file_type_mask(opts->file_type);
    size_t used = 0;
    int total = 0, files_searched = 0;
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
        snprintf(filepath, sizeof filepath, "%s/%s", dir, de->d_name);
        char* content = read_file_to_string(filepath);
        if (!content)
            continue;

        files_searched++;
        total += fr_search_text(de->d_name, content, opts, out, out_size, &used);
        free(content);
    }
    closedir(d);

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
        snprintf(filepath, sizeof filepath, "%s/%s", dir, de->d_name);

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

/* Search or Replace (per opts->mode) is implemented for Look in: Current
 * File and Current Dir - see fr_search_current_file/fr_replace_current_file
 * and fr_search_dir/fr_replace_dir above; Include Dir still just says so
 * rather than doing anything (there's no configured include-directories
 * list to search/replace in yet). Reports every field of `opts` first (the
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
    else
    {
        snprintf(msg, sizeof msg,
            "\"%s\" scope isn't implemented yet - only Current File/Current "
            "Dir actually search or replace.\n", fr_look_in_label(opts->look_in));
    }

    ui_set_value(g_output_editor, msg);
    ui_screen_show_window(g_screen, g_output_window);
}

/* Copies whatever's currently sitting in the panel's live widgets back into
 * the g_fr_* fields - called right before fr_rebuild_content() destroys and
 * recreates those widgets (mode toggle), and right before do_find_replace()
 * reads a snapshot of them (Find/Replace button), so neither loses what the
 * user typed/picked. g_fr_replace_input/g_fr_replace_btn are NULL while in
 * Find mode (never built - see fr_rebuild_content), so those are skipped
 * rather than dereferenced. */
static void fr_sync_from_widgets(void)
{
    if (g_fr_find_input)
        snprintf(g_fr_find_text, sizeof g_fr_find_text, "%s", ui_get_value(g_fr_find_input));
    if (g_fr_replace_input)
        snprintf(g_fr_replace_text, sizeof g_fr_replace_text, "%s", ui_get_value(g_fr_replace_input));
    if (g_fr_opts)
    {
        g_fr_match_case = ui_group_get_checked(g_fr_opts, 0);
        g_fr_match_word = ui_group_get_checked(g_fr_opts, 1);
    }
    if (g_fr_lookin)
        g_fr_look_in = ui_select_get_selected(g_fr_lookin);
    if (g_fr_filetypes)
        g_fr_file_type = ui_select_get_selected(g_fr_filetypes);
}

/* (Re)builds the panel's content from scratch to match g_fr_mode - clearing
 * every child first (same clear-then-repopulate pattern as
 * populate_listbox_from_dir above). This is how "the window changes itself"
 * between Find and Replace: in Find mode there's no Replace field/button at
 * all (not just disabled - never attached to the tree, so never drawn or
 * hit-tested); in Replace mode both appear and everything below shifts down
 * to make room. Values are restored from the g_fr_* fields (see
 * fr_sync_from_widgets) so toggling modes never loses what was typed/picked. */
static void fr_rebuild_content(void)
{
    if (!g_fr_panel)
        return;

    while (ui_child_count(g_fr_panel) > 0)
    {
        ui_node* c = ui_child_at(g_fr_panel, 0);
        ui_remove_child(g_fr_panel, c);
        ui_node_free(c);
    }

    const ui_theme* theme = ui_get_theme();
    int px, py, pw;
    ui_get_rect(g_fr_panel, &px, &py, &pw, NULL);
    /* +2/-4 rather than the flush +1/-2 an interior rect would normally use
     * (e.g. the Folder window's listbox) - leaves a 1-cell margin between
     * the border and the controls on all of the left/top/right (matching
     * every other dialog in this file, which starts its first control 2
     * cells in from the window's own x/y rather than 1), and symmetrically
     * on the right (-4 instead of -3: one cell for the border, one for this
     * same margin, on each side). */
    int cx = px + 2, cw = pw - 4;
    int cy = py + 2;

    add_text(g_fr_panel, cx, cy, "Find:", COLOR_YELLOW, theme->window_bg);
    cy += 1;
    g_fr_find_input = add_input(g_fr_panel, cx, cy, cw, g_fr_find_text);
    cy += 2;

    if (g_fr_mode)
    {
        add_text(g_fr_panel, cx, cy, "Replace:", COLOR_YELLOW, theme->window_bg);
        cy += 1;
        g_fr_replace_input = add_input(g_fr_panel, cx, cy, cw, g_fr_replace_text);
        cy += 2;
    }
    else
    {
        g_fr_replace_input = NULL;
    }

    g_fr_opts = add_group(g_fr_panel, cx, cy, cw, 2, 1);
    add_group_item(g_fr_opts, "Match case");
    add_group_item(g_fr_opts, "Match whole word");
    ui_group_set_checked(g_fr_opts, 0, g_fr_match_case);
    ui_group_set_checked(g_fr_opts, 1, g_fr_match_word);
    cy += 3;

    add_text(g_fr_panel, cx, cy, "Look in:", COLOR_YELLOW, theme->window_bg);
    cy += 1;
    g_fr_lookin = add_group(g_fr_panel, cx, cy, cw, 3, 0);
    add_group_item(g_fr_lookin, "Current File");
    add_group_item(g_fr_lookin, "Current Dir");
    add_group_item(g_fr_lookin, "Include Dir");
    ui_select_set_selected(g_fr_lookin, g_fr_look_in);
    cy += 4;

    add_text(g_fr_panel, cx, cy, "File Types:", COLOR_YELLOW, theme->window_bg);
    cy += 1;
    g_fr_filetypes = add_select(g_fr_panel, cx, cy, cw);
    add_select_item(g_fr_filetypes, EVT_FR_FILETYPE_BASE + 0, "*.c");
    add_select_item(g_fr_filetypes, EVT_FR_FILETYPE_BASE + 1, "*.c;*.h");
    add_select_item(g_fr_filetypes, EVT_FR_FILETYPE_BASE + 2, "*.md");
    add_select_item(g_fr_filetypes, EVT_FR_FILETYPE_BASE + 3, "*.*");
    ui_select_set_selected(g_fr_filetypes, g_fr_file_type);
    cy += 2;

    g_fr_find_btn = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(g_fr_find_btn, EVT_FR_FIND_BTN);
    ui_set_label(g_fr_find_btn, "  Find  ");
    if (g_fr_mode)
    {
        int half = (cw - 1) / 2;
        ui_set_rect(g_fr_find_btn, cx, cy, half, 1);
        ui_append_child(g_fr_panel, g_fr_find_btn);

        g_fr_replace_btn = ui_create_element(UI_TAG_BUTTON);
        ui_set_id(g_fr_replace_btn, EVT_FR_REPLACE_BTN);
        ui_set_label(g_fr_replace_btn, " Replace ");
        ui_set_rect(g_fr_replace_btn, cx + half + 1, cy, cw - half - 1, 1);
        ui_append_child(g_fr_panel, g_fr_replace_btn);
    }
    else
    {
        ui_set_rect(g_fr_find_btn, cx, cy, cw, 1);
        ui_append_child(g_fr_panel, g_fr_find_btn);
        g_fr_replace_btn = NULL;
    }
    cy += 2;

    /* Mode toggle - last control, below Find/Replace rather than above them
     * (per request), so the buttons that act don't shift position depending
     * on how many fields are showing above the toggle. */
    g_fr_mode_btn = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(g_fr_mode_btn, EVT_FR_MODE);
    ui_set_rect(g_fr_mode_btn, cx, cy, cw, 1);
    ui_set_label(g_fr_mode_btn, g_fr_mode ? "Mode: Replace" : "Mode: Find");
    ui_append_child(g_fr_panel, g_fr_mode_btn);
}

/* Shared by Edit > Stringify/To Upper/To Lower: the active document's
 * <editor> and its current selection, or NULL (leaving *out_lo/*out_hi
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
 * paragraph's first line's own leading whitespace. Returns a malloc'd,
 * NUL-terminated buffer (or NULL if out of memory); *out_len is the
 * reflowed text's length (excluding the NUL). */
static char* wordwrap_text(const char* sel, int sel_len, int columns, int* out_len)
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

            int cur_len = 0;  /* bytes on the output line being built, incl. indent */
            for (int w = 0; w < word_count; w++)
            {
                int wlen = words[w].len;
                const char* wtext = sel + words[w].start;
                int is_first_on_line = (cur_len == 0);
                int needed = is_first_on_line ? indent_len + wlen : cur_len + 1 + wlen;

                if (!is_first_on_line && needed > columns)
                {
                    wwbuf_append(&out, "\n", 1);
                    cur_len = 0;
                    is_first_on_line = 1;
                }
                if (is_first_on_line)
                {
                    for (int k = 0; k < indent_len; k++)
                        wwbuf_append(&out, " ", 1);
                    cur_len = indent_len;
                }
                else
                {
                    wwbuf_append(&out, " ", 1);
                    cur_len++;
                }
                wwbuf_append(&out, wtext, (size_t)wlen);
                cur_len += wlen;
            }

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

static void do_edit_wordwrap(int columns)
{
    int lo, hi;
    ui_node* ed = edit_selection_target(&lo, &hi);
    if (!ed)
        return;
    const char* text = ui_get_value(ed);
    int out_len;
    char* out = wordwrap_text(text + lo, hi - lo, columns, &out_len);
    if (out)
        edit_replace_selection(ed, lo, hi, out, out_len);
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
        do_compile();
    }
    else if (id == EVT_COMPILE_OPTIONS)
    {
        ui_select_set_selected(g_copts_target, target_slug_to_index(g_compile_target));
        ui_screen_show_modal(g_screen, g_copts_modal);
    }
    else if (id == EVT_COPTS_OK)
    {
        /* Persist what was typed/picked so the next compile uses it. Target
         * index maps to the compiler's slug; the last option ("") -> "". */
        snprintf(g_compile_options, sizeof g_compile_options, "%s",
                 ui_get_value(g_copts_input));
        int sel = ui_select_get_selected(g_copts_target);
        int slug_count = (int)(sizeof g_target_slugs / sizeof g_target_slugs[0]);
        g_compile_target = (sel >= 0 && sel < slug_count) ? g_target_slugs[sel] : "";
        ui_screen_close_modal(g_screen, g_copts_modal);
    }
    else if (id == EVT_COPTS_CANCEL)
    {
        ui_screen_close_modal(g_screen, g_copts_modal);
    }
    else if (id == EVT_TOGGLE_AUTOCOMPILE)
    {
        g_auto_compile_enabled = !g_auto_compile_enabled;
        ui_node* item = ui_find_by_id(g_root, EVT_TOGGLE_AUTOCOMPILE);
        if (item)
            ui_set_label(item, g_auto_compile_enabled ? "Auto-compile: On" : "Auto-compile: Off");
        if (g_statusbar_autocompile_item)
            ui_set_label(g_statusbar_autocompile_item, g_auto_compile_enabled ? "Auto:On" : "Auto:Off");
    }
    else if (id == EVT_HELP_INDEX)
    {
        do_help_index();
    }
    else if (id == EVT_HELP_CONTEXTUAL)
    {
        do_help_contextual();
    }
    else if (id == EVT_HELP_CHECK)
    {
        do_help_check();
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
        ui_screen_show_modal(g_screen, g_wordwrap_modal);
        ui_screen_focus(g_screen, g_wordwrap_input);
    }
    else if (id == EVT_WORDWRAP_OK || id == EVT_WORDWRAP_INPUT)
    {
        int columns = atoi(ui_get_value(g_wordwrap_input));
        if (columns > 0)
        {
            g_wordwrap_columns = columns;
            do_edit_wordwrap(columns);
        }
        ui_screen_close_modal(g_screen, g_wordwrap_modal);
    }
    else if (id == EVT_WORDWRAP_CANCEL)
    {
        ui_screen_close_modal(g_screen, g_wordwrap_modal);
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
    else if (id == EVT_FOLDER_TOGGLE_FILTER)
    {
        /* Flips the Folder panel's own filtering (see
         * g_folder_filter_enabled/apply_index_order) and re-lists
         * g_folder_dir immediately, so the effect is visible without
         * having to navigate away and back. */
        g_folder_filter_enabled = !g_folder_filter_enabled;
        folder_window_refresh();
    }
    else if (id == EVT_FOLDER_SHOW_FILTER)
    {
        /* Opens g_folder_dir's own CAKE_FOLDER_FILTER_NAME - creating it
         * first, pre-filled with the folder's current contents, if it
         * doesn't exist yet (the popup item itself already read "Create
         * Filter" in that case - see refresh_folder_show_filter_item).
         * Either way this then opens it via the same open_file_path_
         * into_editor as any other file. */
        char path[1024];
        snprintf(path, sizeof path, "%s/%s", g_folder_dir, CAKE_FOLDER_FILTER_NAME);

        FILE* existing = fopen(path, "rb");
        if (existing)
            fclose(existing);
        else
            create_default_filter_file(g_folder_dir);

        nav_record_jump();
        open_file_path_into_editor(path, CAKE_FOLDER_FILTER_NAME);
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
    else if (id == EVT_EDITOR_SHOW_FOLDER)
    {
        /* Reveal this document's containing folder in the Folder panel -
         * same folder_reveal_directory() F1's contextual help uses (see
         * do_help_contextual). A no-op for a window with no path (shouldn't
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
            if (g_compile_target[0] != '\0')
                snprintf(new_path, sizeof(new_path), "%s/%s/%s", dir, g_compile_target, base);
            else
                snprintf(new_path, sizeof(new_path), "%s", path);   // fallback

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
            snprintf(title, sizeof(title), " %s [%s] ", base,
                     g_compile_target[0] ? g_compile_target : "default");

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
        g_find_target = editor_in_window(ui_screen_top_window(g_screen));
        fill_from_selection(g_find_target, g_find_input);
        ui_screen_show_modal(g_screen, g_find_modal);
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
        ui_screen_close_modal(g_screen, g_find_modal);
    }
    else if (id == EVT_SEARCH_REPLACE)
    {
        /* Remember which editor to search - the frontmost editor window's
         * editor - before the modal covers it. */
        g_replace_target = editor_in_window(ui_screen_top_window(g_screen));
        fill_from_selection(g_replace_target, g_replace_find);
        ui_screen_show_modal(g_screen, g_replace_modal);
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
        ui_screen_close_modal(g_screen, g_replace_modal);
    }
    else if (id == EVT_OPTIONS_ENV)
    {
        ui_select_set_selected(g_env_theme_select, g_theme_index);
        ui_screen_show_modal(g_screen, g_env_modal);
    }
    else if (id == EVT_ENV_OK)
    {
        ui_screen_close_modal(g_screen, g_env_modal);
    }
    else if (id == EVT_ENV_THEME_CLASSIC)
    {
        apply_theme(&g_theme_classic);
        g_theme_index = 0;
    }
    else if (id == EVT_ENV_THEME_DARK)
    {
        apply_theme(&g_theme_dark);
        g_theme_index = 1;
    }
    else if (id == EVT_ENV_THEME_WHITE)
    {
        apply_theme(&g_theme_white);
        g_theme_index = 2;
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
        g_open_dialog_mode = OPEN_DLG_FILE;
        ui_set_label(g_open_window, " Open a File ");
        ui_set_label(g_open_ok, "  Open  ");
        if (!ui_get_cwd(g_open_dir, sizeof g_open_dir))
            strcpy(g_open_dir, ".");
        strcpy(g_open_mask, g_open_filters[0].mask);
        ui_select_set_selected(g_open_filter, 0);
        open_dialog_set_filter_visible(1);
        open_dialog_refresh();
        ui_screen_show_modal(g_screen, g_open_modal);
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
        g_open_dialog_mode = OPEN_DLG_FOLDER;
        ui_set_label(g_open_window, " Open Folder ");
        ui_set_label(g_open_ok, " Select ");
        if (!ui_get_cwd(g_open_dir, sizeof g_open_dir))
            strcpy(g_open_dir, ".");
        open_dialog_set_filter_visible(0);
        open_dialog_refresh();
        ui_screen_show_modal(g_screen, g_open_modal);
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
    else if (id == EVT_OPEN_NAME)
    {
        if (g_open_dialog_mode == OPEN_DLG_SAVE)
        {
            save_as_activate();
        }
        else if (g_open_dialog_mode == OPEN_DLG_FOLDER)
        {
            /* The whole field is the target directory in folder mode -
             * there's no mask to split off. Normalize back to '/' first -
             * the field displays backslashes cosmetically (open_dialog_
             * refresh), which aren't a valid separator on Linux/macOS, so
             * an unedited default would otherwise get baked in literally
             * (see save_as_activate for the same fix on the Save side). */
            char text[1024];
            strncpy(text, ui_get_value(g_open_name_input), sizeof text - 1);
            text[sizeof text - 1] = 0;
            for (char* p = text; *p; p++)
                if (*p == '\\')
                    *p = '/';
            if (text[0])
            {
                strncpy(g_open_dir, text, sizeof g_open_dir - 1);
                g_open_dir[sizeof g_open_dir - 1] = 0;
            }
            open_dialog_refresh();
        }
        else
        {
            /* The user typed a new path/mask and pressed Enter - split it at
             * the last separator (either style) into directory + mask, same as
             * the real dialog's Name field. */
            const char* text = ui_get_value(g_open_name_input);
            char buf[1024];
            strncpy(buf, text, sizeof buf - 1);
            buf[sizeof buf - 1] = 0;
            for (char* p = buf; *p; p++)
                if (*p == '\\')
                    *p = '/';
            char* last_sep = NULL;
            for (char* p = buf; *p; p++)
                if (*p == '/')
                    last_sep = p;
            if (last_sep)
            {
                *last_sep = 0;
                strncpy(g_open_mask, last_sep + 1, sizeof g_open_mask - 1);
                g_open_mask[sizeof g_open_mask - 1] = 0;
                if (buf[0])
                {
                    strncpy(g_open_dir, buf, sizeof g_open_dir - 1);
                    g_open_dir[sizeof g_open_dir - 1] = 0;
                }
            }
            else if (buf[0])
            {
                strncpy(g_open_mask, buf, sizeof g_open_mask - 1);
                g_open_mask[sizeof g_open_mask - 1] = 0;
            }
            open_dialog_refresh();
        }
    }
    else if (id >= EVT_OPEN_FILTER && id < EVT_OPEN_FILTER + OPEN_FILTER_COUNT)
    {
        /* "Files of type" picked from the dropdown - same effect as typing
         * that mask into the Name field (EVT_OPEN_NAME's non-Save branch),
         * just friendlier. Works in both File-Open and Save-As mode: it
         * always re-filters g_open_listbox; in File-Open mode it also
         * replaces the mask portion of the Name field's text, since that
         * field *is* dir+mask there (in Save mode the field is the target
         * filename instead, so it's left alone - see open_dialog_refresh). */
        int idx = id - EVT_OPEN_FILTER;
        strncpy(g_open_mask, g_open_filters[idx].mask, sizeof g_open_mask - 1);
        g_open_mask[sizeof g_open_mask - 1] = 0;
        open_dialog_refresh();
    }
    else if (id == EVT_OPEN_LISTBOX)
    {
        /* A row was activated: directories navigate in either mode; a file
         * row opens it (open mode) or drops its name in the Name field (save
         * mode) - see open_dialog_activate. */
        open_dialog_activate(ui_select_get_selected(g_open_listbox));
    }
    else if (id == EVT_OPEN_OK)
    {
        if (g_open_dialog_mode == OPEN_DLG_SAVE)
            save_as_activate();
        else if (g_open_dialog_mode == OPEN_DLG_FOLDER)
            folder_select_confirm();
        else
            open_dialog_activate(ui_select_get_selected(g_open_listbox));
    }
    else if (id == EVT_OPEN_CANCEL)
    {
        ui_screen_close_modal(g_screen, g_open_modal);
        g_open_dialog_mode = OPEN_DLG_FILE;

        /* Canceling out of a Save As that a Compile opened (see do_compile's
         * untitled-file check) aborts the compile too, rather than going
         * ahead and compiling a placeholder path - there's nothing sensible
         * to compile until the file actually has a real name. */
        g_pending_compile_after_saveas = 0;
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
        while (ui_screen_window_count(g_screen) > 0)
            ui_screen_close_modal(g_screen, ui_screen_window_at(g_screen, 0));
    }
    else if (id == EVT_WINDOW_REFRESH)
    {
        refresh_open_windows();
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
        ui_screen_show_window(g_screen, g_output_window);
    }
    else if (id == EVT_WINDOW_FOLDER)
    {
        ui_screen_show_window(g_screen, g_folder_window);
    }
    else if (id == EVT_WINDOW_PLAYGROUND)
    {
        open_playground();
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
        ui_screen_show_window(g_screen, g_fr_window);
        fr_rebuild_content();
        ui_get_rect(g_fr_panel, NULL, NULL, &g_fr_last_w, NULL);

        /* Land the caret in the Find field right away rather than leaving
         * the user to click it - ui_screen_show_window() itself blurs
         * whatever was focused (see its own doc comment), and
         * fr_rebuild_content() just built a brand new g_fr_find_input node,
         * so this has to happen last, after both. */
        ui_screen_focus(g_screen, g_fr_find_input);
    }
    else if (id == EVT_TOOLS_TERMINAL)
    {
        do_open_terminal();
    }
    else if (id == EVT_FR_MODE)
    {
        fr_sync_from_widgets();
        g_fr_mode = !g_fr_mode;
        fr_rebuild_content();
    }
    else if (id == EVT_FR_FIND_BTN || id == EVT_FR_REPLACE_BTN)
    {
        fr_sync_from_widgets();

        find_replace_options opts;
        memset(&opts, 0, sizeof opts);
        opts.mode = (id == EVT_FR_REPLACE_BTN) ? 1 : 0;
        snprintf(opts.find_text, sizeof opts.find_text, "%s", g_fr_find_text);
        snprintf(opts.replace_text, sizeof opts.replace_text, "%s", g_fr_replace_text);
        opts.match_case = g_fr_match_case;
        opts.match_whole_word = g_fr_match_word;
        opts.look_in = (fr_look_in)g_fr_look_in;
        opts.file_type = (fr_file_type)g_fr_file_type;
        do_find_replace(&opts);
    }
    else if (id == EVT_FOLDER_LISTBOX)
    {
        folder_window_activate(ui_select_get_selected(g_folder_listbox));
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
 * elsewhere - the one config directory session.txt (get_session_file_path)
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
    snprintf(buf, cap, "%s\\session.txt", dir);
#else
    snprintf(buf, cap, "%s/session.txt", dir);
#endif
    return 1;
}

/* Playground's own fixed file - unlike every other document window, it
 * always opens this exact path, never anything File > Open picked. Kept
 * next to session.txt (same config directory, see get_config_dir) rather
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
            fwrite(playground_default, 1, sizeof playground_default - 1, pf);
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

/* Help > Check (EVT_HELP_CHECK): prints a couple of paths worth knowing when
 * troubleshooting - where the session config file lives (see
 * get_session_file_path) and where the running executable itself lives (see
 * fs.h's get_self_path) - to the Output window, same place do_compile()/
 * do_find_replace() report their own results. dirname() (fs.c) strips the
 * last path component in place, same pattern used throughout this file
 * (see e.g. do_help_index just above) to turn a file path into its
 * containing directory. */
/* Builds Help > Check's report: not a developer path dump but a short,
 * user-facing install check - app dir first (everything else is explained
 * relative to it), then each file it found/didn't find, each with a
 * one-line "what it's for" and, when something's missing, a one-line fix. */
static void do_help_check(void)
{
    char exe_dir[FS_MAX_PATH] = "(unavailable)";
    char exe_path[FS_MAX_PATH] = { 0 };
    get_self_path(exe_path, sizeof exe_path);
    if (exe_path[0])
    {
        snprintf(exe_dir, sizeof exe_dir, "%s", exe_path);
        dirname(exe_dir);
    }

    char session_dir[FS_MAX_PATH] = "(unavailable)";
    char session_path[FS_MAX_PATH];
    if (get_session_file_path(session_path, sizeof session_path))
    {
        snprintf(session_dir, sizeof session_dir, "%s", session_path);
        dirname(session_dir);  /* strips "session.txt", leaving just the dir */
    }

    /* cakeconf.h: same file/location the compiler itself looks for (see
     * CAKE_CONFIG_FILE_NAME, include_config_header() in tokenizer.c/lib.c,
     * and generate_config_file() in compile.c/lib.c) - it lives next to the
     * executable and, when present, supplies the default #include search
     * directories used when none are passed explicitly on the command line
     * (a fresh copy can be generated with the compiler's -autoconfig flag). */
    char cakeconfig_path[FS_MAX_PATH] = "(unavailable)";
    char* cakeconfig_content = NULL;
    int cakeconfig_found = 0;
    if (exe_path[0])
    {
        snprintf(cakeconfig_path, sizeof cakeconfig_path, "%s/cakeconf.h", exe_dir);
        FILE* cf = fopen(cakeconfig_path, "rb");
        if (cf)
        {
            fseek(cf, 0, SEEK_END);
            long size = ftell(cf);
            fseek(cf, 0, SEEK_SET);
            if (size < 0)
                size = 0;
            cakeconfig_content = malloc((size_t)size + 1);
            if (cakeconfig_content)
            {
                size_t got = fread(cakeconfig_content, 1, (size_t)size, cf);
                cakeconfig_content[got] = 0;
                cakeconfig_found = 1;
            }
            fclose(cf);
        }
    }

    char cakeconfig_section[FS_MAX_PATH + 512];
    if (cakeconfig_found)
    {
        snprintf(cakeconfig_section, sizeof cakeconfig_section,
                 "cakeconf.h: found\n"
                 "  %s\n"
                 "  Tells the compiler where to find system headers (like stdio.h).\n",
                 cakeconfig_path);
    }
    else
    {
        snprintf(cakeconfig_section, sizeof cakeconfig_section,
                 "cakeconf.h: NOT FOUND\n"
                 "  looked in: %s\n"
                 "  Tells the compiler where to find system headers (like stdio.h).\n"
                 "  Tip: run the compiler with -autoconfig to generate one.\n",
                 cakeconfig_path);
    }

    /* playground.c: the fixed file the Playground window always edits (see
     * get_playground_file_path/open_playground's own doc comment) - created
     * automatically the first time Playground is opened, so "not found"
     * here just means Playground hasn't been opened yet this install (or
     * something deleted the file out from under a running IDE). */
    char playground_path[FS_MAX_PATH] = "(unavailable)";
    int playground_found = 0;
    if (get_playground_file_path(playground_path, sizeof playground_path))
    {
        FILE* pf = fopen(playground_path, "rb");
        if (pf)
        {
            playground_found = 1;
            fclose(pf);
        }
    }

    char playground_section[FS_MAX_PATH + 256];
    if (playground_found)
    {
        snprintf(playground_section, sizeof playground_section,
                 "playground.c: found\n"
                 "  %s\n"
                 "  The fixed scratch file the Playground window always edits.\n",
                 playground_path);
    }
    else
    {
        snprintf(playground_section, sizeof playground_section,
                 "playground.c: NOT FOUND\n"
                 "  looked in: %s\n"
                 "  The fixed scratch file the Playground window always edits -\n"
                 "  recreated automatically next time the IDE starts.\n",
                 playground_path);
    }

    char* msg = NULL;
    size_t total = sizeof cakeconfig_section + sizeof playground_section +
                    strlen(exe_dir) + strlen(session_dir) +
                    (cakeconfig_content ? strlen(cakeconfig_content) : 0) + 768;
    msg = malloc(total);
    if (msg)
    {
        int n = snprintf(msg, total,
                 "=== Installation Check ===\n\n"
#if defined(__APPLE__)
                 "Note: Cake currently compiles and runs on macOS. However,\n"
                 "Clang's system headers are not yet parsed correctly on\n"
                 "macOS, so you may need to provide a few declarations\n"
                 "manually while testing. This should be fixed soon, and\n"
                 "hopefully it won't require Cake to implement Objective-C.\n\n"
#endif
                 "App directory\n"
                 "  %s\n"
                 "  Folder containing this program - the other files below are\n"
                 "  found (or looked for) relative to it.\n\n"
                 "%s\n"
                 "Config directory (session.txt)\n"
                 "  %s\n"
                 "  Remembers your last session: open file, window size/position,\n"
                 "  and panel layout - kept separately from the app itself.\n"
                 "  Tip: if the IDE opens looking wrong, delete session.txt here\n"
                 "  to reset it.\n\n"
                 "%s",
                 exe_dir, cakeconfig_section, session_dir, playground_section);

        if (cakeconfig_found && cakeconfig_content && n > 0 && (size_t)n < total)
        {
            snprintf(msg + n, total - (size_t)n,
                     "\ncakeconf.h contents\n----------------------------------------\n%s\n",
                     cakeconfig_content);
        }
    }

    ui_set_value(g_output_editor, msg ? msg : "Check failed: out of memory.\n");
    free(msg);
    free(cakeconfig_content);
    ui_screen_show_window(g_screen, g_output_window);
}

/* Persists the session snapshot described above. Silently does nothing if
 * the config directory can't be resolved/created or opened for writing -
 * losing the session is far less disruptive than failing to exit. */
static void save_session(void)
{
    char path[FS_MAX_PATH];
    if (!get_session_file_path(path, sizeof path))
        return;
    FILE* f = fopen(path, "wb");
    if (!f)
        return;

    fprintf(f, "compile_options=%s\n", g_compile_options);
    fprintf(f, "compile_target=%s\n", g_compile_target);
    fprintf(f, "folder_dir=%s\n", g_folder_dir);

    nav_pos cur;
    if (nav_capture(&cur))
    {
        fprintf(f, "current_file=%s\n", cur.path);
        fprintf(f, "current_cursor=%d\n", cur.cursor);
        fprintf(f, "current_scroll=%d\n", cur.scroll);
    }

    if (g_active_editor_window)
    {
        ui_node* win = ui_child_at(g_active_editor_window, 0);
        if (win)
        {
            int x, y, w, h;
            ui_get_rect(win, &x, &y, &w, &h);
            fprintf(f, "main_x=%d\n", x);
            fprintf(f, "main_y=%d\n", y);
            fprintf(f, "main_w=%d\n", w);
            fprintf(f, "main_h=%d\n", h);
            fprintf(f, "main_maximized=%d\n", ui_get_maximized(win));
        }
    }

    if (g_folder_window)
    {
        ui_node* win = ui_child_at(g_folder_window, 0);
        if (win)
        {
            int w;
            ui_get_rect(win, NULL, NULL, &w, NULL);
            fprintf(f, "folder_dock_w=%d\n", w);
        }
    }
    if (g_output_window)
    {
        ui_node* win = ui_child_at(g_output_window, 0);
        if (win)
        {
            int h;
            ui_get_rect(win, NULL, NULL, NULL, &h);
            fprintf(f, "output_dock_h=%d\n", h);
        }
    }

    fclose(f);
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

/* One "key=value" line from `f` into `key`/`val` (each NUL-terminated,
 * trailing \r\n stripped) - 0 at EOF, 1 otherwise. A line with no '=' (or a
 * blank one) yields an empty key/value rather than failing - tolerant of a
 * hand-edited or partially-written file. */
static int session_read_line(FILE* f, char* key, size_t key_cap, char* val, size_t val_cap)
{
    char line[2048];
    if (!fgets(line, sizeof line, f))
        return 0;

    size_t len = strlen(line);
    while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
        line[--len] = 0;

    char* eq = strchr(line, '=');
    if (!eq)
    {
        if (key_cap) key[0] = 0;
        if (val_cap) val[0] = 0;
        return 1;
    }
    *eq = 0;
    snprintf(key, key_cap, "%s", line);
    snprintf(val, val_cap, "%s", eq + 1);
    return 1;
}

/* However load_session() finds there's no session to restore (no config
 * directory to even look in, or a config directory but no session.txt in
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
    char path[FS_MAX_PATH];
    if (!get_session_file_path(path, sizeof path))
    {
        show_first_run_welcome();
        return 0;
    }
    FILE* f = fopen(path, "rb");
    if (!f)
    {
        show_first_run_welcome();
        return 0;
    }

    char current_file[1024] = "";
    int have_cursor = 0, have_scroll = 0, cursor = 0, scroll = 0;
    int have_main_rect = 0, main_x = 0, main_y = 0, main_w = 0, main_h = 0, main_maximized = 0;
    int have_folder_w = 0, folder_w = 0;
    int have_output_h = 0, output_h = 0;

    char key[64], val[1024];
    while (session_read_line(f, key, sizeof key, val, sizeof val))
    {
        if (strcmp(key, "compile_options") == 0)
            snprintf(g_compile_options, sizeof g_compile_options, "%s", val);
        else if (strcmp(key, "compile_target") == 0)
            g_compile_target = g_target_slugs[target_slug_to_index(val)];
        else if (strcmp(key, "folder_dir") == 0)
        {
            strncpy(g_folder_dir, val, sizeof g_folder_dir - 1);
            g_folder_dir[sizeof g_folder_dir - 1] = 0;
        }
        else if (strcmp(key, "current_file") == 0)
            snprintf(current_file, sizeof current_file, "%s", val);
        else if (strcmp(key, "current_cursor") == 0)
            { cursor = atoi(val); have_cursor = 1; }
        else if (strcmp(key, "current_scroll") == 0)
            { scroll = atoi(val); have_scroll = 1; }
        else if (strcmp(key, "main_x") == 0)
            { main_x = atoi(val); have_main_rect = 1; }
        else if (strcmp(key, "main_y") == 0)
            main_y = atoi(val);
        else if (strcmp(key, "main_w") == 0)
            main_w = atoi(val);
        else if (strcmp(key, "main_h") == 0)
            main_h = atoi(val);
        else if (strcmp(key, "main_maximized") == 0)
            main_maximized = atoi(val);
        else if (strcmp(key, "folder_dock_w") == 0)
            { folder_w = atoi(val); have_folder_w = 1; }
        else if (strcmp(key, "output_dock_h") == 0)
            { output_h = atoi(val); have_output_h = 1; }
    }
    fclose(f);

    /* Folder panel: re-point it at the saved directory - folder_window_
     * refresh()'s populate_listbox_from_dir silently no-ops on a bad path,
     * same as browsing there manually would, so a stale/deleted directory
     * just leaves the panel showing nothing rather than crashing. */
    if (g_folder_dir[0])
        folder_window_refresh();

    if (have_folder_w && g_folder_window)
    {
        ui_node* win = ui_child_at(g_folder_window, 0);
        if (win)
            ui_set_dock(win, UI_DOCK_LEFT, folder_w);
    }
    if (have_output_h && g_output_window)
    {
        ui_node* win = ui_child_at(g_output_window, 0);
        if (win)
            ui_set_dock(win, UI_DOCK_BOTTOM, output_h);
    }

    /* Compiler Options dialog: g_compile_options/g_compile_target above
     * already drive do_compile() directly; only the "Options" text field
     * needs an explicit push here, since (unlike the Target <select>) it
     * isn't re-synced every time the dialog opens (see EVT_COMPILE_OPTIONS)
     * - it just keeps whatever was last typed into it for the rest of the
     * run, so it has to be seeded once, here, at startup. */
    if (g_copts_input)
        ui_set_value(g_copts_input, g_compile_options);

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
    g_theme_classic = *ui_get_theme();  /* snapshot before anything can change it */
    const ui_theme* theme = &g_theme_classic;
    g_screen = ui_screen_create();
    ui_node* root = ui_screen_root(g_screen);
    g_root = root;
    build_screen(root);


    /* Classic Turbo Vision desktop backdrop. */
    ui_screen_set_desktop(g_screen, theme->desktop_bg);

    apply_theme(&g_theme_dark);
    g_theme_index = 1;  /* matches the "Dark" row set as startup default here */

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
    add_text(dirs_window, 13, 4, "Include Directories", COLOR_YELLOW, theme->modal_bg);
    add_input(dirs_window, 13, 5, 44, "C:\\CAKEIDE\\INCLUDE");
    add_text(dirs_window, 13, 7, "Library Directories", COLOR_YELLOW, theme->modal_bg);
    add_input(dirs_window, 13, 8, 44, "C:\\CAKEIDE\\LIB");
    add_text(dirs_window, 13, 10, "Output Directory", COLOR_YELLOW, theme->modal_bg);
    add_input(dirs_window, 13, 11, 44, "C:\\CAKEIDE\\SOURCE");
    add_text(dirs_window, 13, 13, "Source Directories", COLOR_YELLOW, theme->modal_bg);
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
    add_text(goto_window, 23, 9, "Enter New Line Number", COLOR_YELLOW, theme->modal_bg);
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
    ui_set_rect(wordwrap_window, 20, 7, 40, 8);
    ui_set_label(wordwrap_window, " Word Wrap ");
    ui_set_color(wordwrap_window, theme->modal_fg, theme->modal_bg);
    ui_append_child(wordwrap_modal, wordwrap_window);
    add_text(wordwrap_window, 23, 9, "Columns", COLOR_YELLOW, theme->modal_bg);
    g_wordwrap_input = add_input(wordwrap_window, 46, 9, 11, "80");
    ui_set_id(g_wordwrap_input, EVT_WORDWRAP_INPUT);
    ui_set_numeric(g_wordwrap_input, 1);
    ui_node* wordwrap_ok = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(wordwrap_ok, EVT_WORDWRAP_OK);
    ui_set_rect(wordwrap_ok, 29, 12, 10, 1);
    ui_set_label(wordwrap_ok, "  OK  ");
    ui_append_child(wordwrap_window, wordwrap_ok);
    ui_node* wordwrap_cancel = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(wordwrap_cancel, EVT_WORDWRAP_CANCEL);
    ui_set_rect(wordwrap_cancel, 41, 12, 10, 1);
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
    add_text(rep_window, rx + 2, ry + 2, "Text to Find", COLOR_YELLOW, theme->modal_bg);
    g_replace_find = add_input(rep_window, rx + 16, ry + 2, 40, "");
    ui_set_id(g_replace_find, EVT_REPLACE_OK);
    add_text(rep_window, rx + 4, ry + 4, "New Text", COLOR_YELLOW, theme->modal_bg);
    g_replace_new = add_input(rep_window, rx + 16, ry + 4, 40, "");
    ui_set_id(g_replace_new, EVT_REPLACE_OK);
    add_text(rep_window, rx + 2, ry + 6, "Options", COLOR_YELLOW, theme->modal_bg);
    ui_node* opts = add_group(rep_window, rx + 2, ry + 7, 26, 3, 1);
    g_replace_opts = opts;
    add_group_item(opts, "Case sensitive");
    add_group_item(opts, "Whole words only");
    add_group_item(opts, "Prompt on replace");
    ui_group_set_checked(opts, 0, 1);
    ui_group_set_checked(opts, 2, 1);
    add_text(rep_window, rx + 32, ry + 6, "Direction", COLOR_YELLOW, theme->modal_bg);
    ui_node* dir = add_group(rep_window, rx + 32, ry + 7, 22, 2, 0);
    g_replace_dir = dir;
    add_group_item(dir, "Forward");
    add_group_item(dir, "Backward");
    ui_select_set_selected(dir, 0);
    add_text(rep_window, rx + 2, ry + 12, "Scope", COLOR_YELLOW, theme->modal_bg);
    ui_node* scope = add_group(rep_window, rx + 2, ry + 13, 26, 2, 0);
    g_replace_scope = scope;
    add_group_item(scope, "Global");
    add_group_item(scope, "Selected text");
    ui_select_set_selected(scope, 0);
    add_text(rep_window, rx + 32, ry + 12, "Origin", COLOR_YELLOW, theme->modal_bg);
    ui_node* origin = add_group(rep_window, rx + 32, ry + 13, 22, 2, 0);
    g_replace_origin = origin;
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
    g_replace_modal = rep_modal;

    /* --- Find modal --- */
    ui_node* fnd_modal = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, fnd_modal);
    int fx = 12, fy = 4, fw = 56, fh = 16;
    ui_node* fnd_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(fnd_window, fx, fy, fw, fh);
    ui_set_label(fnd_window, " Find Text ");
    ui_set_color(fnd_window, theme->modal_fg, theme->modal_bg);
    ui_append_child(fnd_modal, fnd_window);
    add_text(fnd_window, fx + 2, fy + 2, "Text to Find", COLOR_YELLOW, theme->modal_bg);
    g_find_input = add_input(fnd_window, fx + 16, fy + 2, 36, "");
    ui_set_id(g_find_input, EVT_FIND_OK);
    add_text(fnd_window, fx + 2, fy + 4, "Options", COLOR_YELLOW, theme->modal_bg);
    g_find_opts = add_group(fnd_window, fx + 2, fy + 5, 26, 2, 1);
    add_group_item(g_find_opts, "Case sensitive");
    add_group_item(g_find_opts, "Whole words only");
    ui_group_set_checked(g_find_opts, 0, 1);
    add_text(fnd_window, fx + 30, fy + 4, "Direction", COLOR_YELLOW, theme->modal_bg);
    g_find_dir = add_group(fnd_window, fx + 30, fy + 5, 22, 2, 0);
    add_group_item(g_find_dir, "Forward");
    add_group_item(g_find_dir, "Backward");
    ui_select_set_selected(g_find_dir, 0);
    add_text(fnd_window, fx + 2, fy + 9, "Scope", COLOR_YELLOW, theme->modal_bg);
    g_find_scope = add_group(fnd_window, fx + 2, fy + 10, 26, 2, 0);
    add_group_item(g_find_scope, "Global");
    add_group_item(g_find_scope, "Selected text");
    ui_select_set_selected(g_find_scope, 0);
    add_text(fnd_window, fx + 30, fy + 9, "Origin", COLOR_YELLOW, theme->modal_bg);
    g_find_origin = add_group(fnd_window, fx + 30, fy + 10, 22, 2, 0);
    add_group_item(g_find_origin, "From cursor");
    add_group_item(g_find_origin, "Entire scope");
    ui_select_set_selected(g_find_origin, 0);
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
    g_find_modal = fnd_modal;

    /* --- Editor context menu popup --- */
    ui_node* popup = ui_create_element(UI_TAG_MENU);
    ui_append_child(root, popup);
    struct { int id; const char* label; const char* shortcut; } popup_items[] = {
        { EVT_SEARCH_FIND,    "Find...",      "Ctrl+F" },
        { EVT_SEARCH_REPLACE, "Replace...",   "Ctrl+R" },
        { EVT_SEARCH_NEXT,    "Search Next",  "F3" },
        { EVT_SEARCH_GOTO,    "Go to line...", "Ctrl+G" },
    };
    for (int i = 0; i < 4; i++)
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
    g_editor_popup = popup;

    /* --- Folder panel context menu popup --- */
    ui_node* folder_popup = ui_create_element(UI_TAG_MENU);
    ui_append_child(root, folder_popup);
    ui_node* folder_popup_filter = ui_create_element(UI_TAG_ITEM);
    ui_set_id(folder_popup_filter, EVT_FOLDER_TOGGLE_FILTER);
    ui_append_child(folder_popup, folder_popup_filter);
    g_folder_popup_filter = folder_popup_filter;
    ui_node* folder_popup_sep = ui_create_element(UI_TAG_ITEM);
    ui_set_separator(folder_popup_sep, 1);
    ui_append_child(folder_popup, folder_popup_sep);
    ui_node* folder_popup_show = ui_create_element(UI_TAG_ITEM);
    ui_set_id(folder_popup_show, EVT_FOLDER_SHOW_FILTER);
    ui_set_label(folder_popup_show, "Show Filter");
    ui_append_child(folder_popup, folder_popup_show);
    g_folder_popup_show = folder_popup_show;
    g_folder_popup = folder_popup;

    /* --- Environment modal --- */
    ui_node* env_modal = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, env_modal);
    ui_node* env_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(env_window, 20, 6, 40, 8);
    ui_set_label(env_window, " Environment ");
    ui_set_color(env_window, theme->modal_fg, theme->modal_bg);
    ui_append_child(env_modal, env_window);
    add_text(env_window, 23, 8, "Theme:", COLOR_YELLOW, theme->modal_bg);
    ui_node* theme_select = add_select(env_window, 30, 8, 20);
    add_select_item(theme_select, EVT_ENV_THEME_CLASSIC, "Classic");
    add_select_item(theme_select, EVT_ENV_THEME_DARK, "Dark");
    add_select_item(theme_select, EVT_ENV_THEME_WHITE, "White");
    ui_select_set_selected(theme_select, g_theme_index);
    g_env_theme_select = theme_select;
    ui_node* env_ok = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(env_ok, EVT_ENV_OK);
    ui_set_rect(env_ok, 35, 11, 10, 1);
    ui_set_label(env_ok, "  OK  ");
    ui_append_child(env_window, env_ok);
    g_env_modal = env_modal;

    /* --- Compiler Options modal --- */
    ui_node* copts_modal = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, copts_modal);
    ui_node* copts_window = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(copts_window, 15, 5, 50, 10);
    ui_set_label(copts_window, " Compiler Options ");
    ui_set_color(copts_window, theme->modal_fg, theme->modal_bg);
    ui_append_child(copts_modal, copts_window);
    add_text(copts_window, 18, 7, "Options", COLOR_YELLOW, theme->modal_bg);
    g_copts_input = add_input(copts_window, 27, 7, 34, "");
    ui_set_id(g_copts_input, EVT_COPTS_OK);
    add_text(copts_window, 18, 9, "Target", COLOR_YELLOW, theme->modal_bg);
    g_copts_target = add_select(copts_window, 27, 9, 20);
    add_select_item(g_copts_target, EVT_COPTS_TARGET + 0, "X86 MSVC");
    add_select_item(g_copts_target, EVT_COPTS_TARGET + 1, "X64 MSVC");
    add_select_item(g_copts_target, EVT_COPTS_TARGET + 2, "X64 GCC");
    add_select_item(g_copts_target, EVT_COPTS_TARGET + 3, "macOS ARM64");
    ui_select_set_selected(g_copts_target, target_slug_to_index(g_compile_target));
    ui_node* copts_ok = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(copts_ok, EVT_COPTS_OK);
    ui_set_rect(copts_ok, 27, 12, 10, 1);
    ui_set_label(copts_ok, "  OK  ");
    ui_append_child(copts_window, copts_ok);
    ui_node* copts_cancel = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(copts_cancel, EVT_COPTS_CANCEL);
    ui_set_rect(copts_cancel, 41, 12, 10, 1);
    ui_set_label(copts_cancel, "Cancel");
    ui_append_child(copts_window, copts_cancel);
    g_copts_modal = copts_modal;

    /* --- Open File modal --- */
    ui_node* open_modal = ui_create_element(UI_TAG_MODAL);
    ui_append_child(root, open_modal);
    int ox = 10, oy = 2, ow2 = 60, oh2 = 21;  /* the dialog's own convention is
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
    g_open_window = open_window;
    add_text(open_window, ox + 2, oy + 2, "Name", COLOR_YELLOW, theme->modal_bg);
    g_open_name_input = add_input(open_window, ox + 2, oy + 3, 42, "");
    ui_set_id(g_open_name_input, EVT_OPEN_NAME);
    add_text(open_window, ox + 2, oy + 5, "Files", COLOR_YELLOW, theme->modal_bg);
    g_open_listbox = ui_create_element(UI_TAG_LISTBOX);
    ui_set_rect(g_open_listbox, ox + 2, oy + 6, 42, 10);
    ui_set_id(g_open_listbox, EVT_OPEN_LISTBOX);
    ui_append_child(open_window, g_open_listbox);
    g_open_filter_label = add_text(open_window, ox + 2, oy + 17, "Type", COLOR_YELLOW, theme->modal_bg);
    g_open_filter = add_select(open_window, ox + 2, oy + 18, 42);
    for (int i = 0; i < OPEN_FILTER_COUNT; i++)
        add_select_item(g_open_filter, EVT_OPEN_FILTER + i, g_open_filters[i].label);
    ui_select_set_selected(g_open_filter, 0);
    int obx = ox + 46;
    ui_node* open_ok = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(open_ok, EVT_OPEN_OK);
    ui_set_rect(open_ok, obx, oy + 3, 12, 1);
    ui_set_label(open_ok, "  Open  ");
    ui_append_child(open_window, open_ok);
    g_open_ok = open_ok;
    ui_node* open_cancel = ui_create_element(UI_TAG_BUTTON);
    ui_set_id(open_cancel, EVT_OPEN_CANCEL);
    ui_set_rect(open_cancel, obx, oy + 5, 12, 1);
    ui_set_label(open_cancel, " Cancel ");
    ui_append_child(open_window, open_cancel);
    g_open_modal = open_modal;

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
    ui_set_rect(output, ow_x + 1, ow_y + 1, ow_w - 2, ow_h - 2);
    ui_set_syntax(output, UI_SYNTAX_VT100);
    ui_set_value(output, "");
    ui_append_child(output_window, output);
    g_output_window = output_wrapper;
    g_output_editor = output;

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

    g_folder_listbox = ui_create_element(UI_TAG_LISTBOX);
    ui_set_id(g_folder_listbox, EVT_FOLDER_LISTBOX);
    ui_set_rect(g_folder_listbox, fw_x + 1, fw_y + 1, fw_w - 2, fw_h - 2);
    ui_append_child(folder_window, g_folder_listbox);
    g_folder_window = folder_wrapper;
    if (!ui_get_cwd(g_folder_dir, sizeof g_folder_dir))
        strcpy(g_folder_dir, ".");
    folder_window_refresh();

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
                                     * current width (see g_fr_last_w/
                                     * FR_PANEL_MIN_W and app_frame() below),
                                     * clamped to a sane minimum rather than
                                     * left to just misalign or get crushed */
    ui_set_shadow(fr_panel, 0);
    ui_set_dock(fr_panel, UI_DOCK_RIGHT, fr_w);
    ui_append_child(fr_wrapper, fr_panel);
    g_fr_window = fr_wrapper;
    g_fr_panel = fr_panel;
    fr_rebuild_content();
    g_fr_last_w = fr_w;  /* matches what fr_rebuild_content() just laid out
                          * against - see the width-change check in app_frame() */
    /* Not shown at startup, unlike Folder/Output - it opens on demand from
     * Tools > "Find and Replace..." (EVT_TOOLS_FINDREPLACE) and its own
     * close icon just hides it again (see ui_set_dock's docs), same as any
     * other docked window. */

    /* ===== FIX: Ensure screen dimensions are initialized before showing docked windows ===== */
    ui_screen_update(g_screen, env);   /* reads env->width/height into g_screen->screen_w/h */

    /* Now show the docked windows – they will get correct sizes */
    ui_screen_show_window(g_screen, g_folder_window);
    ui_screen_show_window(g_screen, g_output_window);

    /* Restore last session's compiler options/target, Folder panel
     * directory, dock sizes, and active document (path/caret/scroll/
     * layout) - see load_session(). When it successfully reopens a real
     * file, the fallback demo window below is skipped entirely; a first
     * run (or a since-vanished file) falls through to the usual startup
     * demo exactly as before. */
    if (!load_session())
    {
        /* --- Demo/test file window (maximized) --- */
        ui_node* demo_wrapper = make_new_editor_window(root, g_new_count++);
        ui_node* demo_editor = editor_in_window(demo_wrapper);
        ui_screen_show_window(g_screen, demo_wrapper);
    }

    ui_screen_set_on_event(g_screen, on_ui_event, NULL);
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
    if (g_fr_panel)
    {
        int pw;
        ui_get_rect(g_fr_panel, NULL, NULL, &pw, NULL);
        if (pw < FR_PANEL_MIN_W)
        {
            int px, py, ph;
            ui_get_rect(g_fr_panel, &px, &py, NULL, &ph);
            ui_set_rect(g_fr_panel, px, py, FR_PANEL_MIN_W, ph);
            pw = FR_PANEL_MIN_W;
        }
        if (pw != g_fr_last_w)
        {
            g_fr_last_w = pw;
            fr_rebuild_content();
        }
    }

    /* Track the frontmost real document window (see g_active_editor_window)
     * - only updated when an actual editor window is frontmost, so it keeps
     * pointing at the last one even while a docked Folder/Output panel (or
     * a modal) is briefly on top instead. */
    {
        ui_node* top = ui_screen_top_window(g_screen);
        if (is_editor_window(top))
            g_active_editor_window = top;
    }

    /* The Edit menu's own "Read-only" item has no open-time hook the way the
     * popup does (see below) - the menubar opens its dropdowns itself, with
     * no app-level callback - so it's just kept current every frame instead,
     * off of whichever document window is frontmost. */
    refresh_readonly_item(g_edit_readonly_item, g_active_editor_window);

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

            /* Refresh "Toggle Header/Source" - enabled only for a .c/.h file. */
            char cp[1024];
            int has_counterpart = header_source_counterpart(ui_get_path(win), cp, sizeof cp);
            ui_set_enabled(g_editor_popup_hdrsrc, has_counterpart);

            /* Refresh "Show Generated Code" - disabled for a .md file, same
             * idea as Toggle Header/Source above: Markdown is never
             * compiled, so there's no generated code to show. */
            ui_set_enabled(g_editor_popup_show_output,
                           syntax_for_path(ui_get_path(win)) != UI_SYNTAX_MARKDOWN);

            ui_screen_open_popup(g_screen, g_editor_popup, mx, my, win);
        }
    }

    /* Right-click over the Folder panel's listbox opens its own popup (the
     * "Filter" toggle + "Show Filter"/"Create Filter") at the cursor - no
     * modal must be blocking. Independent of the editor popup check above: when the
     * Folder panel is frontmost, ui_screen_top_window() returns it (not an
     * editor window), so `ed` there is NULL and that block's
     * ui_node_contains(NULL, ...) already no-ops - see its own doc comment. */
    if (ui_screen_mouse_right_pressed(g_screen) && !ui_screen_active_modal(g_screen))
    {
        int mx = ui_screen_mouse_x(g_screen), my = ui_screen_mouse_y(g_screen);
        if (ui_node_contains(g_folder_listbox, mx, my))
        {
            refresh_folder_filter_item(g_folder_popup_filter);
            refresh_folder_show_filter_item(g_folder_popup_show);
            ui_screen_open_popup(g_screen, g_folder_popup, mx, my, NULL);
        }
    }

    auto_compile_tick();

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
