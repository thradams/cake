/* ui.c - environment + DOM tree implementation: the ui_env event queue, the
 * DOM tree, and update/render/event dispatch. See ui.h for the schema and
 * public API. Rendering walks the tree and emits draw calls directly - there
 * is no cell buffer here at all. */
#include "ide_ui.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

 /* --- Environment: event queue + screen dimensions --- */

#define UI_ENV_EVENT_QUEUE_SIZE 256

/* Text-caret blink half-period, ms (~classic Windows caret rate) - the caret
 * is solid for this long, then hidden for this long, repeating. */
#define UI_CARET_BLINK_MS 530

 /* Whether macOS-style shortcut conventions are active - see
  * ui_set_mac_shortcuts()/ui_mac_shortcuts() in ui.h. Defaults to the
  * platform's native convention (this used to be a compile-time
  * #ifdef __APPLE__ picking UI_MOD_WORD), but is now a runtime setting so an
  * app can flip it (a Preferences toggle, or testing one convention while
  * running on the other platform). */
static int g_mac_shortcuts =
#ifdef __APPLE__
1;
#else
0;
#endif

void ui_set_mac_shortcuts(int enable) { g_mac_shortcuts = enable != 0; }
int ui_mac_shortcuts(void) { return g_mac_shortcuts; }

/* The modifier that carries "primary" shortcuts - Copy/Cut/Paste/Undo/
 * Select All, and every menu <item> shortcut declared in "Ctrl+..." form
 * (see ui_set_shortcut) - Cmd on macOS, Ctrl elsewhere. */
static int primary_mod(void) { return g_mac_shortcuts ? UI_MOD_CMD : UI_MOD_CTRL; }

/* Modifier that triggers word-at-a-time cursor movement (Ctrl+Left/Right).
 * macOS reserves Ctrl+arrow for the OS (Mission Control / Spaces) and uses
 * Option+arrow for word jumps like every native Mac editor, so this is Alt
 * when macOS conventions are active; Windows and X11 use Ctrl. */
static int word_mod(void) { return g_mac_shortcuts ? UI_MOD_ALT : UI_MOD_CTRL; }

/* Cmd+Left/Right/Up/Down (line/document start-end - see the editor key
 * switch below) are a macOS-only convention layered on top of UI_MOD_CMD,
 * which no other backend ever sets - but still gated on g_mac_shortcuts so
 * turning macOS conventions off (even while actually running on macOS)
 * turns this off too, rather than only affecting the letter shortcuts. */
static int mac_nav_mod_active(int mods) { return g_mac_shortcuts && (mods & UI_MOD_CMD); }

struct ui_env {
    int width, height;
    ui_event queue[UI_ENV_EVENT_QUEUE_SIZE];
    int head;  /* next slot to write */
    int tail;  /* next slot to read */
    unsigned time_ms;  /* wall-clock ms fed by the backend (ui_env_set_time_ms),
                        * used only to drive the caret blink phase - 0 if the
                        * backend never sets it, which just leaves the caret
                        * solid (no blink) rather than breaking anything. */

                        /* The backend's font-zoom implementation (Ctrl+/Ctrl-, and now also the
                         * Window > Font Size +/- menu items) - see ui_env_set_font_zoom_fn/
                         * ui_env_adjust_font_size. NULL on a backend that doesn't support it. */
    ui_font_zoom_fn font_zoom_fn;
    void* font_zoom_ctx;
};

int ui_default_cols = 120;
int ui_default_rows = 30;

ui_env* ui_env_create(int w, int h)
{
    ui_env* e = malloc(sizeof(*e));
    if (!e)
        return NULL;

    e->width = w;
    e->height = h;
    e->head = 0;
    e->tail = 0;
    e->time_ms = 0;
    e->font_zoom_fn = NULL;
    e->font_zoom_ctx = NULL;
    return e;
}

void ui_env_set_font_zoom_fn(ui_env* e, ui_font_zoom_fn fn, void* ctx)
{
    e->font_zoom_fn = fn;
    e->font_zoom_ctx = ctx;
}

void ui_env_adjust_font_size(ui_env* e, int delta)
{
    if (e && e->font_zoom_fn)
        e->font_zoom_fn(e->font_zoom_ctx, delta);
}

void ui_env_set_time_ms(ui_env* e, unsigned ms)
{
    if (e)
        e->time_ms = ms;
}

void ui_env_free(ui_env* e)
{
    free(e);
}

int ui_env_width(ui_env* e)
{
    return e->width;
}

int ui_env_height(ui_env* e)
{
    return e->height;
}

void ui_env_resize(ui_env* e, int w, int h)
{
    e->width = w;
    e->height = h;
}

void ui_env_post_event(ui_env* e, const ui_event* ev)
{
    int next = (e->head + 1) % UI_ENV_EVENT_QUEUE_SIZE;
    if (next == e->tail)
    {
        /* Queue is full; drop the oldest event (overwrite tail). */
        e->tail = (e->tail + 1) % UI_ENV_EVENT_QUEUE_SIZE;
    }
    e->queue[e->head] = *ev;
    e->head = next;
}

int ui_env_poll_event(ui_env* e, ui_event* out)
{
    if (e->head == e->tail)
        return 0;  /* queue empty */

    *out = e->queue[e->tail];
    e->tail = (e->tail + 1) % UI_ENV_EVENT_QUEUE_SIZE;
    return 1;
}

/* --- Visual palette: the classic Turbo Vision colors, as the default
 * ui_theme (see ui.h) - every render_*() function below reads g_theme
 * instead of a hardcoded constant, so ui_set_theme() can swap the whole
 * look at runtime. --- */

static ui_theme g_theme = {
    /* The classic Turbo Vision desktop backdrop - muted blue. */
    .desktop_bg = TB_RGB(0x10, 0x20, 0x45),

    .btn_bg = TB_RGB(0x00, 0xAA, 0x00),
    .btn_bg_hot = TB_RGB(0x00, 0xFF, 0x00),
    .btn_bg_active = TB_RGB(0x00, 0x55, 0x00),
    .btn_fg = TB_RGB(0x00, 0x00, 0x00),

    .hotkey_fg = TB_RGB(0x00, 0x00, 0x00),
    .hotkey_key_fg = TB_RGB(0xAA, 0x00, 0x00),
    .hotkey_bg = TB_RGB(0xC0, 0xC0, 0xC0),
    .hotkey_fg_hot = TB_RGB(0x00, 0x00, 0x00),
    .hotkey_bg_hot = TB_RGB(0x00, 0xFF, 0x00),

    .menu_fg = TB_RGB(0x00, 0x00, 0x00),
    .menu_bg = TB_RGB(0xC0, 0xC0, 0xC0),
    .menu_fg_sel = TB_RGB(0x00, 0x00, 0x00),
    .menu_bg_sel = TB_RGB(0x00, 0xB0, 0x00),
    .menu_item_fg = TB_RGB(0x00, 0x00, 0x00),
    .menu_item_bg = TB_RGB(0xC0, 0xC0, 0xC0),
    .menu_item_fg_hot = TB_RGB(0x00, 0x00, 0x00),
    .menu_item_bg_hot = TB_RGB(0x00, 0xB0, 0x00),
    .menu_item_shortcut_fg = TB_RGB(0xAA, 0x00, 0x00),  /* same red as hotkey keys */
    .menu_item_fg_disabled = TB_RGB(0x80, 0x80, 0x80),
    /* The open dropdown/combo popup's own box outline - same black-on-gray
     * as its items by default, but its own variable since a theme may want
     * the popup's frame to read differently than the rows inside it. */
    .menu_border_fg = TB_RGB(0x00, 0x00, 0x00),
    .menu_border_bg = TB_RGB(0xC0, 0xC0, 0xC0),
    .menu_border_style = UI_BORDER_DOUBLE,

    .box_fg = TB_RGB(0x00, 0x00, 0x00),
    .box_bg = TB_RGB(0xC0, 0xC0, 0xC0),
    .box_border_style = UI_BORDER_DOUBLE,

    /* <window>'s border is white-on-gray, not black-on-gray like <box> -
     * matches the classic Borland modal dialog frame. */
    .window_border_fg = TB_RGB(0xFF, 0xFF, 0xFF),
    .window_border_bg = TB_RGB(0xC0, 0xC0, 0xC0),
    /* Border turns this color while the window is being dragged by its
     * title row - same green as the menu's selection highlight, for one
     * consistent "active/selected" accent color across the whole UI. */
    .window_border_fg_dragging = TB_RGB(0x00, 0xB0, 0x00),
    /* An unfocused window's single-line border - same muted gray already
     * used for disabled menu items, so "not the active thing" reads
     * consistently across the whole UI. */
    .window_border_fg_unfocused = TB_RGB(0x80, 0x80, 0x80),
    .window_border_style = UI_BORDER_DOUBLE,
    .window_border_style_unfocused = UI_BORDER_SINGLE,
    .window_border_style_docked = UI_BORDER_DOUBLE,
    .window_border_style_docked_unfocused = UI_BORDER_SINGLE,
    /* Close icon's filled square - same green as the dragging accent. */
    .window_close_bg = TB_RGB(0x00, 0xB0, 0x00),
    /* A non-modal <window>'s own body/interior fill - same gray panel with
     * black text as a modal dialog's, by default. */
    .window_fg = TB_RGB(0x00, 0x00, 0x00),
    .window_bg = TB_RGB(0xC0, 0xC0, 0xC0),
    /* A modal dialog's own frame - same white-on-gray double line as a
     * floating <window>'s, by default (a modal is always drawn in its one
     * focused/active style, so there's no dragging/unfocused variant). */
    .modal_border_fg = TB_RGB(0xFF, 0xFF, 0xFF),
    .modal_border_bg = TB_RGB(0xC0, 0xC0, 0xC0),
    .modal_border_style = UI_BORDER_DOUBLE,
    /* A modal dialog's own body/interior fill, inside its border - the
     * classic Borland gray panel with black text. */
    .modal_fg = TB_RGB(0x00, 0x00, 0x00),
    .modal_bg = TB_RGB(0xC0, 0xC0, 0xC0),
    /* Trough matches the window border/panel gray, with a darker gray thumb
     * on top so the slider stands out against it. */
    .scrollbar_bg = TB_RGB(0xC0, 0xC0, 0xC0),
    .scrollbar_thumb_bg = TB_RGB(0x80, 0x80, 0x80),

    /* Field background stays the same whether focused or not - only the
     * text color and the caret indicate focus, same as Turbo Vision's edit
     * lines. */
    .input_bg = TB_RGB(0x00, 0x00, 0xB0),
    .input_fg = TB_RGB(0x54, 0xFF, 0xFF),
    .input_fg_focus = TB_RGB(0xFF, 0xFF, 0xFF),
    /* Caret cell - same green/black combo as the menu's selection highlight. */
    .input_sel_bg = TB_RGB(0x00, 0xB0, 0x00),
    .input_sel_fg = TB_RGB(0x00, 0x00, 0x00),

    /* <editor> - classic Turbo Pascal/C IDE blue edit window, with its own
     * selection/caret colors (editor_sel_*), independent of <input>'s. */
    .editor_bg = TB_RGB(0x00, 0x00, 0xB0),
    .editor_fg = TB_RGB(0xC0, 0xC0, 0xC0),
    .editor_keyword_fg = TB_RGB(0xFF, 0xFF, 0x55),   /* types/storage: bright yellow */
    .editor_keyword2_fg = TB_RGB(0xFF, 0xFF, 0xFF),  /* control flow: bright white */
    .editor_string_fg = TB_RGB(0x55, 0xFF, 0x55),
    .editor_comment_fg = TB_RGB(0x55, 0xFF, 0xFF),
    .editor_linenum_fg = TB_RGB(0x80, 0x80, 0xC0),  /* muted blue-gray -
                                                     * readable against this
                                                     * theme's dark blue
                                                     * editor_bg without
                                                     * competing with any
                                                     * syntax color */
    .editor_preproc_fg = TB_RGB(0xFF, 0x55, 0xFF),
    .editor_sel_bg = TB_RGB(0x80, 0x80, 0x80),
    .editor_sel_fg = TB_RGB(0x00, 0x00, 0x00),
    .editor_current_line_bg = TB_RGB(0x18, 0x18, 0xCE),  /* one step lighter
                                                          * than editor_bg -
                                                          * visible without
                                                          * looking like a
                                                          * selection */
    .editor_bracket_fg = {
        TB_RGB(0xFF, 0x55, 0x55),  /* light red */
        TB_RGB(0xAA, 0x55, 0x00),  /* brown */
        TB_RGB(0xAA, 0xAA, 0xAA),  /* light gray */
        TB_RGB(0x55, 0x55, 0xFF),  /* light blue */
    },
    .editor_tag_fg = TB_RGB(0xFF, 0xAA, 0x00),  /* orange - struct/union/enum
                                                 * tag names */
    .editor_number_fg = TB_RGB(0x55, 0xAA, 0xFF),  /* sky blue - numeric
                                                     * literals */
    .editor_char_fg = TB_RGB(0xFF, 0xAA, 0xAA),  /* light pink - char
                                                   * literals, distinct from
                                                   * string_fg's green */

    /* UI_SYNTAX_MARKDOWN - see render_editor_line_markdown(). Defaults just
     * match the colors Markdown used to borrow from the C editor palette
     * (keyword/comment/string_fg), so switching to these dedicated fields
     * doesn't change how anything already looks; md_link_fg is new (link
     * text previously had no color of its own). */
    .md_heading_fg = TB_RGB(0xFF, 0xFF, 0x55),     /* bright yellow, same as
                                                     * editor_keyword_fg */
    .md_blockquote_fg = TB_RGB(0x55, 0xFF, 0xFF),  /* cyan, same as
                                                     * editor_comment_fg */
    .md_code_fg = TB_RGB(0x55, 0xFF, 0x55),        /* green, same as
                                                     * editor_string_fg */
    .md_bold_fg = TB_RGB(0xC0, 0xC0, 0xC0),        /* same as editor_fg -
                                                     * bold text with no real
                                                     * bold attribute to fall
                                                     * back on, just plain */
    .md_link_fg = TB_RGB(0xAA, 0xAA, 0xFF),        /* light blue/lavender -
                                                     * distinct from every
                                                     * other Markdown color */
    .md_code_bg = TB_RGB(0x00, 0x00, 0x80),        /* navy - one step darker
                                                     * than editor_bg (the
                                                     * opposite direction from
                                                     * editor_current_line_bg,
                                                     * which is lighter), so a
                                                     * code block reads as
                                                     * "recessed" */

    /* <listbox> - the classic Turbo Vision file-list cyan, dark-blue
     * selection bar (same blue as <input>'s body). */
    .listbox_fg = TB_RGB(0x00, 0x00, 0x00),
    .listbox_bg = TB_RGB(0x00, 0xAA, 0xAA),
    .listbox_sel_fg = TB_RGB(0xFF, 0xFF, 0xFF),
    .listbox_sel_bg = TB_RGB(0x00, 0x00, 0xAA),

    /* <editor> inline diagnostics (see ui_editor_add_diagnostic) - bright
     * CGA red/yellow/blue, same "bright" family as editor_keyword_fg/
     * editor_string_fg above, so they read as part of the same palette. */
        .diag_error_fg = TB_RGB(0xFF, 0xFF, 0xFF),       // white
    .diag_error_bg = TB_RGB(0xFF, 0x55, 0x55),       // bright red
    .diag_warning_fg = TB_RGB(0x00, 0x00, 0x00),     // black
    .diag_warning_bg = TB_RGB(0xFF, 0xFF, 0x55),     // bright yellow
    .diag_info_fg = TB_RGB(0x00, 0x00, 0x00),        // black
    .diag_info_bg = TB_RGB(0xFF, 0xFF, 0xFF),        // white
};

const ui_theme* ui_get_theme(void)
{
    return &g_theme;
}

void ui_set_theme(const ui_theme* theme)
{
    g_theme = *theme;
}

/* Single-line box-drawing (dropdown popup frame). */
#define UI_BOX_TL "\xE2\x94\x8C"  /* U+250C ┌ */
#define UI_BOX_TR "\xE2\x94\x90"  /* U+2510 ┐ */
#define UI_BOX_BL "\xE2\x94\x94"  /* U+2514 └ */
#define UI_BOX_BR "\xE2\x94\x98"  /* U+2518 ┘ */
#define UI_BOX_H  "\xE2\x94\x80"  /* U+2500 ─ */
#define UI_BOX_V  "\xE2\x94\x82"  /* U+2502 │ */

/* Double-line box-drawing (<box> - a focused-window-style panel). */
#define UI_DBOX_TL "\xE2\x95\x94"  /* U+2554 ╔ */
#define UI_DBOX_TR "\xE2\x95\x97"  /* U+2557 ╗ */
#define UI_DBOX_BL "\xE2\x95\x9A"  /* U+255A ╚ */
#define UI_DBOX_BR "\xE2\x95\x9D"  /* U+255D ╝ */
#define UI_DBOX_H  "\xE2\x95\x90"  /* U+2550 ═ */
#define UI_DBOX_V  "\xE2\x95\x91"  /* U+2551 ║ */

/* --- UTF-8 decoding (labels are plain UTF-8 C strings; something has to
 * turn them into code points for draw_char). --- */

#define UI_REPLACEMENT 0xFFFDu  /* U+FFFD REPLACEMENT CHARACTER */

static int utf8_decode(const char* s, uint32_t* out_cp)
{
    const unsigned char* p = (const unsigned char*)s;
    unsigned char c = p[0];

    if (c < 0x80)
    {
        *out_cp = c;
        return 1;
    }

    int len;
    uint32_t cp;
    uint32_t min;

    if ((c & 0xE0) == 0xC0) { len = 2; cp = c & 0x1F; min = 0x80; }
    else if ((c & 0xF0) == 0xE0) { len = 3; cp = c & 0x0F; min = 0x800; }
    else if ((c & 0xF8) == 0xF0) { len = 4; cp = c & 0x07; min = 0x10000; }
    else { *out_cp = UI_REPLACEMENT; return 1; }

    for (int i = 1; i < len; i++)
    {
        if ((p[i] & 0xC0) != 0x80)
        {
            *out_cp = UI_REPLACEMENT;
            return 1;
        }
        cp = (cp << 6) | (p[i] & 0x3F);
    }

    if (cp < min || cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF))
    {
        *out_cp = UI_REPLACEMENT;
        return 1;
    }

    *out_cp = cp;
    return len;
}


/* Return the number of display columns (Unicode codepoints) in a UTF‑8 string.
 * For our use case, every codepoint occupies exactly 1 column. */
static int utf8_vis_len(const char* s)
{
    int len = 0;
    while (*s)
    {
        uint32_t cp;
        int clen = utf8_decode(s, &cp);
        if (clen == 0) break;
        len++;
        s += clen;
    }
    return len;
}

/* --- Retained tree --- */

struct ui_diagnostic;  /* full definition below, right after struct ui_node */

/* One snapshot in an EDITOR's undo/redo history - see ui_editor_undo/redo
 * and the undo_stack/redo_stack fields on ui_node below. */
typedef struct ui_undo_entry {
    char* text;       /* malloc'd copy of the node's label at this point */
    int cursor;
    int sel_anchor;
} ui_undo_entry;

/* Kinds of edit an EDITOR's undo grouping distinguishes - see
 * editor_undo_mark(). Edits of the same kind, at adjacent cursor positions
 * and close together in time, coalesce into one undo step; anything else
 * starts a new one. */
enum {
    UI_UNDO_NONE = 0,
    UI_UNDO_TYPE,      /* a plain inserted character */
    UI_UNDO_NEWLINE,   /* the Enter key - always its own step (never
                         * coalesces, even with a preceding/following Enter),
                         * matching editors like Visual Studio where each
                         * line break is independently undoable */
    UI_UNDO_BACKSPACE,
    UI_UNDO_DELFWD,    /* Delete key with no selection */
    UI_UNDO_PASTE,     /* one editor_paste()'s worth of inserted text */
    UI_UNDO_DELSEL,    /* erasing an active selection - Delete/Backspace/
                         * typing over it, or Cut - always its own step */
};

#define UI_UNDO_COALESCE_MS 700u  /* a pause longer than this starts a new
                                    * undo group even mid-burst */
#define UI_UNDO_MAX 500           /* cap per editor, oldest entries evicted
                                    * first - keeps a long editing session's
                                    * memory use bounded */

struct ui_node {
    int type;            /* a ui_tag value */
    int id;               /* numeric event code; 0 = fires nothing */
    char* label;
    char* shortcut;       /* ITEM only: hint text shown right-aligned, e.g. "Ctrl+F9" */
    char* path;           /* opaque string an app can attach to any node - not
                            * read or rendered by the framework itself (e.g.
                            * turboc_demo.c uses it on a document window's
                            * wrapper to remember which file backs it) */
    int enabled;          /* hit_test_widget ignores a disabled node entirely */
    int read_only;        /* like the DOM's readonly attribute: still focusable/
                            * selectable/copyable, just can't be modified - see
                            * ui_set_read_only. INPUT/EDITOR only actually
                            * enforce it (in input_insert/backspace/
                            * input_delete_forward/editor_delete_selection);
                            * available on every tag for a caller to set/query. */
    int separator;        /* ITEM only: a plain unselectable dividing line
                            * instead of a normal label - see ui_set_separator */
    int x, y, w, h;        /* explicit (button/box/text/input) or computed by layout_*() */
    uint32_t fg, bg;      /* used by TEXT/BOX; other tags use hardcoded palettes */
    int cursor;           /* INPUT/EDITOR only: caret position, a byte offset into label */
    int sel_anchor;       /* INPUT/EDITOR only: the selection's other end (a
                            * byte offset), or -1 for no selection - the
                            * selection spans [min, max) of this and cursor */
    int scroll;           /* EDITOR only: index of the first visible line */
    int hscroll;          /* EDITOR only: first visible column (horizontal
                            * scroll offset) - follows the caret, or panned by
                            * Shift+mouse-wheel; see editor_clamp_hscroll */
    int dirty;            /* EDITOR only: 1 if edited since the last
                            * ui_set_dirty(n, 0) (e.g. a "Save"), else 0 */
    int selected;         /* SELECT/LISTBOX/GROUP: index of the chosen <item>
                            * child (GROUP single-select mode); GROUP items
                            * also reuse this as their own 0/1 checked flag in
                            * multi-select mode - see render_group */
    int numeric;          /* INPUT only: if set, only digits can be typed -
                            * see ui_set_numeric / input_insert */
    int multi;            /* GROUP only: 0 = single-select (radio), 1 =
                            * multi-select (check boxes) - see ui_set_multi */
    int cursor_row;       /* GROUP only: keyboard-focused row (distinct from
                            * the radio selection, which is `selected`) */
    int msgbox;           /* MODAL only: 1 if built by ui_message_box - the
                            * framework auto-closes and frees it when any of
                            * its buttons is pressed (or on Escape) */
    int transient;        /* MODAL (window wrapper) only: 0 (default) for a
                            * reusable singleton the app re-shows later as-is
                            * (a docked panel, an About/Options dialog, ...) -
                            * closing it just hides it, same as always. 1 for
                            * a one-off window meant to be torn down for good
                            * when closed (a document/editor window - see
                            * ui_set_transient/ui_screen_close_modal/
                            * ui_screen_take_closed_window). */
    int untitled;         /* MODAL (window wrapper) only: 1 while it's still
                            * at its original never-saved placeholder path -
                            * see ui_set_untitled. App-only flag; the
                            * framework never reads it itself. */

                            /* EDITOR only: render_editor's scan cache - the byte offset and block-
                             * comment state at the start of line `scan_line`, so a repaint at an
                             * unchanged scroll position resumes there instead of re-walking the
                             * buffer from byte 0. Invalidated (scan_valid=0) by ui_set_label, which
                             * every text edit funnels through. */
    int scan_valid;
    int scan_line;   /* the line index the cached offset/state belong to */
    int scan_off;    /* byte offset of that line's first character */
    int scan_block;  /* whether that line starts inside a block comment */
    int scan_bracket_depth;  /* C syntax only: ( [ { nesting depth at the
                              * start of that line - see render_editor_line's
                              * rainbow bracket coloring */
    int scan_c_block;    /* Markdown only: whether line `scan_line` starts
                          * inside a ```c fenced block - see render_editor()'s
                          * embedded-C dispatch, md_fence_lang_is_c() */
    int scan_c_comment;  /* that embedded block's own C block-comment state
                          * at `scan_line` - independent of scan_block above,
                          * which tracks Markdown's "inside *some* fence" */
    int scan_c_bracket;  /* that embedded block's own bracket-nesting depth -
                          * independent of scan_bracket_depth above, which is
                          * only ever used when n->syntax itself is
                          * UI_SYNTAX_C (a whole file of C, not a block
                          * embedded in Markdown) */

    /* WINDOW only: geometry saved across a maximize, so a later restore can
     * put it back exactly where (and what size) it was. */
    int maximized;
    int saved_x, saved_y, saved_w, saved_h;
    int resizable;  /* off by default - most dialogs have a fixed layout that
                      * was never designed to reflow; opt in per window (see
                      * ui_set_resizable) for ones that were, like an editor. */
    int shadow;     /* on by default, like every other shadow-casting widget
                      * (button/box/menu/dropdown) - opt individual windows
                      * out (see ui_set_shadow) for ones that shouldn't look
                      * "raised", like a floating document/editor window. */
    int dock;       /* WINDOW only: a ui_dock_side value - UI_DOCK_NONE (0,
                      * the calloc default) for an ordinary floating window,
                      * else which desktop edge it's pinned to - see
                      * ui_set_dock. Doubles as its dock thickness's storage:
                      * w for LEFT/RIGHT, h for BOTTOM (see dock_layout). */
    int syntax;     /* EDITOR only: a ui_syntax value - see ui_set_syntax.
                      * UI_SYNTAX_NONE (0, the calloc default) by default. */
    struct ui_diagnostic* diagnostics;  /* EDITOR only: singly linked, kept
                      * sorted ascending by line - see
                      * ui_editor_add_diagnostic and render_editor(). */

                      /* EDITOR only: undo/redo history - see ui_editor_undo/redo below. Each
                       * entry is a full-buffer snapshot; an <editor> holds one file's worth of
                       * text at a time, never large enough that a per-edit diff would be worth
                       * the extra complexity. undo_group/undo_group_pos/undo_group_time let a
                       * run of the same kind of edit (a burst of typing, a held-down
                       * Backspace, one paste) collapse into a single undo step instead of one
                       * per keystroke - see editor_undo_mark(). */
    struct ui_undo_entry* undo_stack; int undo_count, undo_cap;
    struct ui_undo_entry* redo_stack; int redo_count, redo_cap;
    int undo_group;            /* a UI_UNDO_* kind, or UI_UNDO_NONE between edits */
    int undo_group_pos;        /* cursor offset a continuing edit of the same
                                 * kind must start at to extend the group */
    unsigned undo_group_time;  /* g_ui_undo_now_ms as of the group's last
                                 * edit - a gap past UI_UNDO_COALESCE_MS
                                 * starts a new group even if kind/position
                                 * still match */

    struct ui_node** children;
    int child_count;
};

/* One entry in an <editor>'s diagnostic list (see ui_editor_add_diagnostic
 * in ui.h) - a singly linked list kept sorted ascending by `line`, so
 * render_editor() can walk it in lockstep with its (also top-to-bottom)
 * visible-row loop: a single merge pass, never a per-row search. Entirely
 * internal - callers only ever touch it through ui_editor_add_diagnostic/
 * ui_editor_clear_diagnostics, never the struct itself. */
struct ui_diagnostic {
    ui_diag_type type;
    int line;              /* 1-based source line, matching how compilers
                             * report diagnostics */
    char* message;
    struct ui_diagnostic* next;
};
typedef struct ui_diagnostic ui_diagnostic;

#define UI_MAX_KEY_EVENTS 8
#define UI_MODAL_STACK_MAX 8
#define UI_WINDOW_STACK_MAX 16

/* Max destructive overlay rects (shadows + cursor) tracked per frame for the
 * next-frame erase pass - see the overlay handling in emit_box/ui_screen_render.
 * Each shadow caster contributes ~4 tiny rects and the cursor 1, so this is
 * generous; overflow safely falls back to a full-screen invalidate. */
#define UI_MAX_OVERLAYS 64

 /* One cell of the render shadow (see struct ui_screen's `cache`): the exact
  * (glyph, colors) ui_screen_render last emitted for this cell. A cell whose
  * new values match is skipped - no backend draw call. For a blank cell the
  * foreground is meaningless (nothing is drawn in it), so it's normalized to 0
  * before storing/comparing so two blanks that differ only in fg still match. */
typedef struct {
    uint32_t ch, fg, bg;
    /* Z-order stamp: a monotonic counter (s->emit_seq) incremented on each
     * content write, so a later (higher-z) write over the same cell has a
     * greater seq. Only used to occlude overlays (see the replay loop in
     * ui_screen_render) - the diff compares ch/fg/bg only, never seq. */
    unsigned seq;
} ui_cell;

typedef struct {
    int x, y, w, h;
} ui_rect;

/* A destructive overlay (shadow/cursor) recorded during the tree walk and
 * replayed onto the backend after the content diff - see ui_screen_render.
 * `seq` is s->emit_seq at record time: content written after it (higher seq)
 * is above it in z-order and must occlude it on replay. */
typedef struct {
    int x, y, w, h, flags;
    unsigned seq;
} ui_overlay;

struct ui_screen {
    ui_node* root;

    int mouse_x, mouse_y;
    int mouse_down, mouse_pressed, mouse_released, mouse_moved, mouse_dblclick;
    int mouse_mods;     /* UI_MOD_* flags from the last mouse event - e.g. lets
                         * the EDITOR click handler tell a plain click from a
                         * Ctrl+click (see the Markdown "open link" check) */
    int mouse_right_pressed;  /* right button pressed this frame (for context
                               * menus - see ui_screen_mouse_right_pressed) */

    ui_node* hot;       /* node under the mouse this frame, or NULL */
    ui_node* active;    /* node currently pressed, or NULL */
    ui_node* open_menu; /* currently open top-level <menu>, or NULL */
    ui_node* open_submenu; /* an open submenu: the <item> (a child of open_menu)
                            * whose own <item> children are shown as a second
                            * dropdown to the right, or NULL */
    ui_node* open_select; /* the <select> whose popup is open, or NULL */
    void* open_popup_param; /* the param ui_screen_open_popup() was called
                             * with for the current s->open_menu, delivered
                             * to ui_fire_event() when one of its items fires
                             * (see hit_test_widget) - NULL when s->open_menu
                             * is an ordinary menubar dropdown instead. */
    ui_node* focused;   /* the <input>/<editor> with keyboard focus, or NULL */
    ui_node* selecting; /* the <input>/<editor> a mouse-drag selection is
                          * extending, or NULL - see ui_screen_update(); kept
                          * separate from `focused` since a title-bar drag
                          * mustn't be mistaken for continuing a selection
                          * drag just because the last-focused field happens
                          * to still be that same node. */

                          /* Blocking dialogs are a stack, like a real window manager's modal
                           * chain: showing one pushes it on top (the previous one, if any, just
                           * stops being interactive/visible until this one closes and it's
                           * revealed again - it was never closed). Only the top is ever active.
                           * Floating (non-blocking) windows are a completely separate stack -
                           * opening or closing a modal must never touch it, and vice versa. */
    ui_node* modal_stack[UI_MODAL_STACK_MAX];
    int modal_stack_count;

    /* Floating windows, back-to-front z-order (windows[window_count-1] is
     * the frontmost/active one). Unlike the modal stack, more than one can
     * be interactive/visible at once - e.g. several editor windows open
     * side by side - see ui_screen_show_window(). */
    ui_node* windows[UI_WINDOW_STACK_MAX];
    int window_count;

    /* Dragging a <window> by its title/border row - see the modal branch of
     * ui_screen_update(). drag_offset is the click's position relative to
     * the window's top-left, kept constant for the whole drag so the window
     * doesn't jump to re-center under the cursor. */
    ui_node* dragging_window;
    int drag_offset_x, drag_offset_y;

    /* Resizing a <window> by dragging its bottom-right corner cell. Unlike
     * dragging, no offset is needed - the new size is just the mouse
     * position relative to the window's (fixed) top-left each frame. */
    ui_node* resizing_window;

    /* Dragging an <editor>'s own scrollbar thumb (see editor_has_vscrollbar/
     * editor_has_hscrollbar) - stateless: the scroll position is recomputed
     * straight from the mouse's row/column every frame, no offset needed.
     * Holds the <editor> itself, not its window - like <listbox> below, an
     * editor's scrollbar is self-contained rather than window-driven (see
     * process_window's UI_TAG_EDITOR branch). */
    ui_node* dragging_editor_vscrollbar;
    ui_node* dragging_editor_hscrollbar;  /* horizontal counterpart (bottom row) */

    /* Dragging a <listbox>'s own scrollbar thumb (see listbox_has_scrollbar)
     * - stateless like the two above, holding the <listbox> itself for the
     * same reason (see process_window's UI_TAG_LISTBOX branch). */
    ui_node* dragging_listbox_scrollbar;

    /* A ui_message_box modal to close+free once the current ui_screen_update()
     * finishes (set when one of its buttons fires, or on Escape) - deferred so
     * the tree isn't mutated mid-walk. */
    ui_node* msgbox_to_free;

    /* A transient window (see ui_set_transient) that just closed - set by
     * ui_screen_close_modal(), consumed once by the app via
     * ui_screen_take_closed_window() polling after ui_screen_update()
     * returns. Deferred for the same reason msgbox_to_free is: the app may
     * still hold its own pointer to this window (e.g. "the active editor
     * window") that needs clearing before the node is actually freed, which
     * only the app can do - freeing it here, mid-update, would leave that
     * pointer dangling for the rest of the frame. */
    ui_node* window_closed;

    ui_event key_events[UI_MAX_KEY_EVENTS];
    int key_event_count;

    ui_event_fn on_event;
    void* on_event_ctx;

    /* Screen-wide cursor overlay - not a DOM node, like a browser caret.
     * Drawn by inverting whatever's underneath (see UI_BOX_INVERT), so
     * no color field is needed here. */
    int cursor_x, cursor_y, cursor_on;

    /* Text-caret blink (the <input>/<editor> block cursor, distinct from the
     * screen-wide overlay above). caret_visible is recomputed each update()
     * from the env's wall-clock ms; caret_blink_base is reset to "now" on any
     * keyboard/mouse activity so the caret shows solid the instant it moves
     * (like a real editor) and only starts blinking once idle. */
    int caret_visible;
    unsigned caret_blink_base;

    /* The desktop backdrop - classic Turbo Vision's dithered checkerboard
     * fill behind everything else, drawn first (bottom of the whole stack).
     * screen_w/h are cached here each update() (from the ui_env) since
     * render() only gets draw callbacks, not the env itself. */
    int desktop_on;
    uint32_t desktop_bg;
    int screen_w, screen_h;
    /* Previous frame's screen size, to detect an OS window resize and reflow
     * edge-anchored windows to the new bounds (see reflow_on_screen_resize).
     * 0 until the first update, so the first frame never counts as a resize. */
    int prev_screen_w, prev_screen_h;

    /* --- Incremental render (see ui_screen_render). --- */
    /* `cache` is what the backend currently shows (per cell); `next` is the
     * content the in-progress frame's tree walk intends. The walk fills
     * `next` with no backend calls; a single diff pass then emits a backend
     * draw only for cells where next != cache. Both are cache_w*cache_h cells
     * row-major, NULL until the first render, reallocated when the grid
     * resizes. Kept separate (not diffed inline) so a widget's background
     * fill followed by its text over the same cells doesn't ping-pong the
     * shadow and force a redraw every frame. */
    ui_cell* cache;
    ui_cell* next;
    int cache_w, cache_h;
    /* Monotonic z-order counter for the frame in progress: bumped on each
     * content write and stamped into that cell's `seq`, so overlay replay can
     * tell which cells a higher-z node later covered (see ui_cell.seq). */
    unsigned emit_seq;
    /* Set by ui_screen_invalidate: next render repaints every cell and blits
     * the whole screen (the backing store it draws into is known to no longer
     * match the shadow - new bitmap, new font size). */
    int force_full;
    /* Accumulated invalid rect for the frame in progress, in cell coords -
     * the union of every cell the diff pass actually redrew (plus overlays).
     * Empty when dirty_w == 0. */
    int dirty_x, dirty_y, dirty_w, dirty_h;
    /* Destructive overlays (shadows, XOR cursor) recorded during the walk and
     * replayed after the content diff, since they blend/invert whatever is
     * beneath rather than replacing it with a cacheable value. prev_overlays
     * is last frame's set: those cells are force-repainted this frame so a
     * moved shadow / blinked-off cursor leaves no ghost. */
    ui_overlay overlays[UI_MAX_OVERLAYS];
    int overlay_count;
    ui_rect prev_overlays[UI_MAX_OVERLAYS];
    int prev_overlay_count;
    int overlay_overflow;  /* this frame exceeded UI_MAX_OVERLAYS -> full
                            * invalidate next frame */
};

static char* xstrdup(const char* s)
{
    size_t n = strlen(s) + 1;
    char* p = malloc(n);
    if (p)
        memcpy(p, s, n);
    return p;
}

/* Shared allocation path for ui_create_element(). */
static ui_node* alloc_node(int type)
{
    ui_node* n = calloc(1, sizeof(*n));
    n->type = type;
    n->label = xstrdup("");
    n->shortcut = xstrdup("");
    n->path = xstrdup("");
    n->enabled = 1;
    n->sel_anchor = -1;
    n->shadow = 1;
    return n;
}

static void node_add_child(ui_node* parent, ui_node* child)
{
    parent->children = realloc(parent->children,
                                sizeof(ui_node*) * (parent->child_count + 1));
    parent->children[parent->child_count++] = child;
}

static void free_diagnostics(ui_diagnostic* d)
{
    while (d)
    {
        ui_diagnostic* next = d->next;
        free(d->message);
        free(d);
        d = next;
    }
}

static void node_free(ui_node* n)
{
    if (!n)
        return;
    for (int i = 0; i < n->child_count; i++)
        node_free(n->children[i]);
    free(n->children);
    free(n->label);
    free(n->shortcut);
    free(n->path);
    free_diagnostics(n->diagnostics);
    for (int i = 0; i < n->undo_count; i++)
        free(n->undo_stack[i].text);
    free(n->undo_stack);
    for (int i = 0; i < n->redo_count; i++)
        free(n->redo_stack[i].text);
    free(n->redo_stack);
    free(n);
}

/* --- Layout: shared by update() (hit-testing) and render() (drawing), so
 * the two can never disagree about where a widget is. --- */

static ui_node* find_child_by_type(ui_node* parent, int type)
{
    for (int i = 0; i < parent->child_count; i++)
        if (parent->children[i]->type == type)
            return parent->children[i];
    return NULL;
}

/* menubar/statusbar's own x/y/w/h double as the full-width bar rect (neither
 * is ever given one explicitly) so render can fill the whole row's
 * background before drawing individual items over it. */
static void layout_menubar(ui_node* menubar, int screen_width)
{
    menubar->x = 0;
    menubar->y = 0;
    menubar->w = screen_width;
    menubar->h = 1;

    int x = 1;
    for (int i = 0; i < menubar->child_count; i++)
    {
        ui_node* m = menubar->children[i];
        m->w = (int)strlen(m->label) + 2;
        m->x = x;
        m->y = 0;
        x += m->w;
    }
}

static void layout_statusbar(ui_node* statusbar, int screen_width, int screen_height)
{
    int y = screen_height - 1;

    statusbar->x = 0;
    statusbar->y = y;
    statusbar->w = screen_width;
    statusbar->h = 1;

    int x = 1;
    for (int i = 0; i < statusbar->child_count; i++)
    {
        ui_node* h = statusbar->children[i];
        h->w = (int)strlen(h->label);
        h->x = x;
        h->y = y;
        x += h->w + 2;
    }
}

/* Shortcut hints are always stored as "Ctrl+..." (see ui_set_shortcut) -
 * shortcut_matches() already treats that "Ctrl+" token as whichever modifier
 * is currently primary, so this renders the same substitution for display:
 * "Cmd+" in place of "Ctrl+" when macOS conventions are active, so the menu
 * shows what the user actually presses. `buf` must be at least
 * strlen(shortcut)+1 bytes ("Cmd+" is never longer than "Ctrl+"); returns
 * `shortcut` itself unchanged when no substitution is needed. */
static const char* display_shortcut(const char* shortcut, char* buf, size_t bufsz)
{
    if (g_mac_shortcuts && strncmp(shortcut, "Ctrl+", 5) == 0)
    {
        snprintf(buf, bufsz, "\xe2\x8c\x98 %s", shortcut + 5);
        return buf;
    }
    return shortcut;
}

/* Lays out the dropdown box for an open menu, and each item's own x/y/w/h
 * (inset by the border, ready for both hit-testing and drawing). */
static void layout_dropdown(ui_node* menu, int* out_dx, int* out_dy,
                             int* out_box_w, int* out_box_h)
{
    int maxw = 0;
    for (int i = 0; i < menu->child_count; i++)
    {
        ui_node* it = menu->children[i];
        int l = (int)strlen(it->label);
        char scbuf[32];
        int sc_len = (int)utf8_vis_len(display_shortcut(it->shortcut, scbuf, sizeof scbuf));
        if (sc_len > 0)
            l += 1 + sc_len;  /* +1 gap column before the right-aligned shortcut */
        if (l > maxw)
            maxw = l;
    }
    int inner_w = maxw + 2;
    int box_w = inner_w + 2;
    int box_h = menu->child_count + 2;
    int dx = menu->x;
    int dy = menu->y + 1;

    for (int i = 0; i < menu->child_count; i++)
    {
        ui_node* it = menu->children[i];
        it->x = dx + 1;
        it->y = dy + 1 + i;
        it->w = inner_w;
        it->h = 1;
    }

    *out_dx = dx;
    *out_dy = dy;
    *out_box_w = box_w;
    *out_box_h = box_h;
}

/* An <item> that itself has <item> children is a submenu parent - hovering it
 * opens its children as a second dropdown to the right (see the update/render
 * submenu handling). */
static int item_is_submenu(const ui_node* it)
{
    return it->type == UI_TAG_ITEM && !it->separator && it->child_count > 0;
}

/* Whether `node` is a direct child of `parent` - used to validate a possibly
 * stale s->open_submenu against the current s->open_menu. */
static int is_direct_child(const ui_node* parent, const ui_node* node)
{
    if (!parent || !node)
        return 0;
    for (int i = 0; i < parent->child_count; i++)
        if (parent->children[i] == node)
            return 1;
    return 0;
}

/* Lays out a submenu's box (the children of submenu-parent `item`) to the
 * right of the parent dropdown, its first child aligned with `item`'s row,
 * and each child's x/y/w/h. `box_left` is the parent dropdown box's right
 * edge. */
static void layout_submenu(ui_node* item, int box_left, int* out_dx, int* out_dy,
                           int* out_box_w, int* out_box_h)
{
    int maxw = 0;
    for (int i = 0; i < item->child_count; i++)
    {
        ui_node* it = item->children[i];
        int l = (int)strlen(it->label);
        char scbuf[32];
        int sc_len = (int)utf8_vis_len(display_shortcut(it->shortcut, scbuf, sizeof scbuf));
        if (sc_len > 0)
            l += 1 + sc_len;
        if (l > maxw)
            maxw = l;
    }
    int inner_w = maxw + 2;
    int box_w = inner_w + 2;
    int box_h = item->child_count + 2;
    int dx = box_left;
    int dy = item->y - 1;  /* box top one above the item, so child 0 aligns with it */

    for (int i = 0; i < item->child_count; i++)
    {
        ui_node* it = item->children[i];
        it->x = dx + 1;
        it->y = dy + 1 + i;
        it->w = inner_w;
        it->h = 1;
    }

    *out_dx = dx;
    *out_dy = dy;
    *out_box_w = box_w;
    *out_box_h = box_h;
}

/* Lays out a <select>'s open popup box, and each <item> child's own x/y/w/h
 * - directly below the control, same shape as layout_dropdown() above (a
 * combo box's popup and a menu's dropdown are the same kind of overlay).
 * At least as wide as the control itself, wider if an option's label needs
 * more room. */
static void layout_select_popup(ui_node* select, int* out_dx, int* out_dy,
                                 int* out_box_w, int* out_box_h)
{
    int maxw = select->w - 2;
    for (int i = 0; i < select->child_count; i++)
    {
        int l = (int)strlen(select->children[i]->label);
        if (l > maxw)
            maxw = l;
    }
    int inner_w = maxw + 2;
    int box_w = inner_w + 2;
    int box_h = select->child_count + 2;
    int dx = select->x;
    int dy = select->y + 1;

    for (int i = 0; i < select->child_count; i++)
    {
        ui_node* it = select->children[i];
        it->x = dx + 1;
        it->y = dy + 1 + i;
        it->w = inner_w;
        it->h = 1;
    }

    *out_dx = dx;
    *out_dy = dy;
    *out_box_w = box_w;
    *out_box_h = box_h;
}

/* --- Public API: screen/tree lifecycle --- */

ui_screen* ui_screen_create(void)
{
    ui_screen* s = calloc(1, sizeof(*s));
    s->root = alloc_node(UI_TAG_SCREEN);
    return s;
}

void ui_screen_free(ui_screen* s)
{
    if (!s)
        return;
    node_free(s->root);
    free(s->cache);
    free(s->next);
    free(s);
}

ui_node* ui_screen_root(ui_screen* s)
{
    return s->root;
}

/* --- DOM construction/editing --- */

ui_node* ui_create_element(ui_tag tag)
{
    return alloc_node(tag);
}

void ui_set_id(ui_node* n, int id)
{
    n->id = id;
}

void ui_set_label(ui_node* n, const char* label)
{
    free(n->label);
    n->label = xstrdup(label ? label : "");
    n->scan_valid = 0;  /* content changed - render_editor's scan cache is stale */
}

void ui_set_rect(ui_node* n, int x, int y, int w, int h)
{
    n->x = x;
    n->y = y;
    n->w = w;
    n->h = h;
}

void ui_get_rect(const ui_node* n, int* x, int* y, int* w, int* h)
{
    if (!n)
    {
        if (x) *x = 0;
        if (y) *y = 0;
        if (w) *w = 0;
        if (h) *h = 0;
        return;
    }
    if (x) *x = n->x;
    if (y) *y = n->y;
    if (w) *w = n->w;
    if (h) *h = n->h;
}

int ui_node_contains(const ui_node* n, int x, int y)
{
    return n && x >= n->x && x < n->x + n->w && y >= n->y && y < n->y + n->h;
}

void ui_set_color(ui_node* n, uint32_t fg, uint32_t bg)
{
    n->fg = fg;
    n->bg = bg;
}

void ui_set_shortcut(ui_node* n, const char* shortcut)
{
    free(n->shortcut);
    n->shortcut = xstrdup(shortcut ? shortcut : "");
}

void ui_set_path(ui_node* n, const char* path)
{
    free(n->path);
    n->path = xstrdup(path ? path : "");
}

const char* ui_get_path(const ui_node* n)
{
    return n->path;
}

void ui_set_enabled(ui_node* n, int enabled)
{
    n->enabled = enabled;
}

void ui_set_separator(ui_node* n, int separator)
{
    n->separator = separator;
    if (separator)
        n->enabled = 0;
}

int ui_get_separator(const ui_node* n)
{
    return n->separator;
}

void ui_set_resizable(ui_node* n, int resizable)
{
    n->resizable = resizable;
}

int ui_get_resizable(const ui_node* n)
{
    return n->resizable;
}

void ui_set_transient(ui_node* n, int transient)
{
    if (n)
        n->transient = transient;
}

int ui_get_transient(const ui_node* n)
{
    return n && n->transient;
}

void ui_set_untitled(ui_node* n, int untitled)
{
    if (n)
        n->untitled = untitled;
}

int ui_get_untitled(const ui_node* n)
{
    return n && n->untitled;
}

void ui_set_dock(ui_node* n, ui_dock_side side, int size)
{
    if (!n)
        return;
    n->dock = side;
    /* Stash the thickness directly in w/h - dock_layout() (run every
     * ui_screen_update()) fixes up x/y and the other dimension from this,
     * but callers querying/maximizing against this window before the next
     * update (e.g. right after creating it) still see the right thickness
     * immediately. size == 0 means "leave it as-is" (e.g. just moving the
     * dock from one side to the other, keeping the same width).
     *
     * Also resize any EDITOR/LISTBOX child by the same delta, right here
     * (mirroring set_window_rect's own handling of those tags) - once this
     * function overwrites w/h below, the "old" thickness is gone, so
     * dock_layout()'s left-dock branch (whose x/y/h don't move when only
     * the thickness changes) has nothing left to diff against and never
     * calls set_window_rect() to catch it up. That left the panel's own
     * frame resized but its inner listbox/editor stuck at its previous
     * width/height - a blank strip - whenever a size was pushed in after
     * the window was already on screen (e.g. restoring a saved session's
     * dock sizes in load_session()). Doing it here keeps every caller's
     * child in sync immediately, independent of dock_layout's guard. */
    if (size > 0)
    {
        if (side == UI_DOCK_LEFT || side == UI_DOCK_RIGHT)
        {
            int dw = size - n->w;
            n->w = size;
            if (dw != 0)
            {
                for (int i = 0; i < n->child_count; i++)
                {
                    ui_node* c = n->children[i];
                    if (c->type == UI_TAG_EDITOR || c->type == UI_TAG_LISTBOX)
                        c->w += dw;
                }
            }
        }
        else if (side == UI_DOCK_BOTTOM)
        {
            int dh = size - n->h;
            n->h = size;
            if (dh != 0)
            {
                for (int i = 0; i < n->child_count; i++)
                {
                    ui_node* c = n->children[i];
                    if (c->type == UI_TAG_EDITOR || c->type == UI_TAG_LISTBOX)
                        c->h += dh;
                }
            }
        }
    }
}

ui_dock_side ui_get_dock(const ui_node* n)
{
    return n ? (ui_dock_side)n->dock : UI_DOCK_NONE;
}

void ui_set_shadow(ui_node* n, int shadow)
{
    n->shadow = shadow;
}

int ui_get_shadow(const ui_node* n)
{
    return n->shadow;
}

/* WINDOW only: whether it's currently maximized (see ui_window_maximize) -
 * a plain read-only reflection of the same n->maximized flag the zoom icon
 * and toggle_window_zoom() already drive, for a caller that needs to know
 * without also changing it (e.g. persisting window layout to disk). */
int ui_get_maximized(const ui_node* n)
{
    return n->maximized;
}

void ui_set_syntax(ui_node* n, ui_syntax syntax)
{
    n->syntax = syntax;
    if (syntax == UI_SYNTAX_VT100)
        n->read_only = 1;  /* captured terminal/compiler output - never hand-edited */
}

ui_syntax ui_get_syntax(const ui_node* n)
{
    return (ui_syntax)n->syntax;
}

/* Adds one diagnostic to an <editor>'s list, kept sorted ascending by line
 * (same-line diagnostics stay in the order they were added) - see
 * render_editor()'s single merge pass over this list. */
void ui_editor_add_diagnostic(ui_node* n, ui_diag_type type, int line, const char* message)
{
    ui_diagnostic* d = malloc(sizeof(*d));
    d->type = type;
    d->line = line;
    d->message = xstrdup(message ? message : "");
    d->next = NULL;

    ui_diagnostic** link = &n->diagnostics;
    while (*link && (*link)->line <= line)
        link = &(*link)->next;
    d->next = *link;
    *link = d;
}

void ui_editor_clear_diagnostics(ui_node* n)
{
    free_diagnostics(n->diagnostics);
    n->diagnostics = NULL;
}

void ui_set_dirty(ui_node* n, int dirty)
{
    n->dirty = dirty;
}

int ui_get_dirty(const ui_node* n)
{
    return n->dirty;
}

void ui_set_numeric(ui_node* n, int numeric)
{
    n->numeric = numeric;
}

int ui_get_numeric(const ui_node* n)
{
    return n->numeric;
}

void ui_set_multi(ui_node* group, int multi)
{
    group->multi = multi;
}

int ui_get_multi(const ui_node* group)
{
    return group->multi;
}

void ui_group_set_checked(ui_node* group, int index, int checked)
{
    if (index >= 0 && index < group->child_count)
        group->children[index]->selected = checked ? 1 : 0;
}

int ui_group_get_checked(const ui_node* group, int index)
{
    if (index >= 0 && index < group->child_count)
        return group->children[index]->selected;
    return 0;
}

/* Defined later (with the rest of <listbox>'s scroll handling) - forward
 * declared so ui_select_set_selected() below can keep a newly-set <listbox>
 * row scrolled into view (a <select> ignores this - it has no scroll of its
 * own, only its popup does, which is laid out fresh every time it opens). */
static void listbox_ensure_visible(ui_node* n);

/* Defined later, next to render_diagnostic (they need the same annotation
 * width) - forward declared for the mouse-wheel handler that pans the view
 * and for the horizontal scrollbar's thumb/geometry math. */
static void editor_clamp_hscroll(ui_node* n);
static int editor_content_cols(ui_node* n);

/* <editor>'s own horizontal scrollbar (see the vertical one just below and
 * listbox_has_scrollbar for the overall design) - defined later next to
 * editor_content_cols, which its "is there anything to scroll" check and
 * thumb math both need. Forward declared so process_window's UI_TAG_EDITOR
 * click handling (far above that point in the file) can reach it. */
static int editor_has_hscrollbar(ui_node* n);
static void editor_hscrollbar_layout(ui_node* n, int* out_track_x0, int* out_track_w);
static void editor_hscrollbar_thumb(ui_node* n, int* out_start, int* out_len);
static void editor_hscrollbar_set_from_mouse(ui_node* n, int mouse_x);

void ui_select_set_selected(ui_node* select, int index)
{
    select->selected = index;
    listbox_ensure_visible(select);
}

int ui_select_get_selected(const ui_node* select)
{
    return select->selected;
}

int ui_get_id(const ui_node* n)
{
    return n->id;
}

const char* ui_get_label(const ui_node* n)
{
    return n->label;
}

const char* ui_get_shortcut(const ui_node* n)
{
    return n->shortcut;
}

int ui_get_enabled(const ui_node* n)
{
    return n->enabled;
}

void ui_set_read_only(ui_node* n, int read_only)
{
    n->read_only = read_only;
}

int ui_get_read_only(const ui_node* n)
{
    return n->read_only;
}

void ui_set_value(ui_node* n, const char* value)
{
    ui_set_label(n, value);
    n->cursor = (int)strlen(n->label);
    n->sel_anchor = -1;
    n->scroll = 0;
    n->hscroll = 0;

    /* Fresh content (e.g. a different file opened into this editor) - the
     * undo history belongs to the document that was open, not the widget. */
    for (int i = 0; i < n->undo_count; i++)
        free(n->undo_stack[i].text);
    n->undo_count = 0;
    for (int i = 0; i < n->redo_count; i++)
        free(n->redo_stack[i].text);
    n->redo_count = 0;
    n->undo_group = UI_UNDO_NONE;
}

const char* ui_get_value(const ui_node* n)
{
    return n->label;
}

void ui_append_child(ui_node* parent, ui_node* child)
{
    node_add_child(parent, child);
}

void ui_remove_child(ui_node* parent, ui_node* child)
{
    for (int i = 0; i < parent->child_count; i++)
    {
        if (parent->children[i] == child)
        {
            for (int j = i; j < parent->child_count - 1; j++)
                parent->children[j] = parent->children[j + 1];
            parent->child_count--;
            return;
        }
    }
}

int ui_child_count(const ui_node* n)
{
    return n->child_count;
}

ui_node* ui_child_at(const ui_node* n, int i)
{
    if (i < 0 || i >= n->child_count)
        return NULL;
    return n->children[i];
}

void ui_node_free(ui_node* n)
{
    node_free(n);
}

ui_node* ui_find_by_id(ui_node* root, int id)
{
    if (!root)
        return NULL;
    if (root->id == id)
        return root;
    for (int i = 0; i < root->child_count; i++)
    {
        ui_node* found = ui_find_by_id(root->children[i], id);
        if (found)
            return found;
    }
    return NULL;
}

void ui_screen_set_cursor(ui_screen* s, int x, int y, int on)
{
    s->cursor_x = x;
    s->cursor_y = y;
    s->cursor_on = on;
}

void ui_screen_set_desktop(ui_screen* s, uint32_t bg)
{
    s->desktop_on = 1;
    s->desktop_bg = bg;
}

/* --- Events --- */

static void ui_fire_event(ui_screen* s, int id, void* param)
{
    if (id == 0)
        return;  /* widgets with no "id" attribute fire nothing */
    if (s->on_event)
        s->on_event(s->on_event_ctx, id, param);
}

void ui_screen_set_on_event(ui_screen* s, ui_event_fn fn, void* ctx)
{
    s->on_event = fn;
    s->on_event_ctx = ctx;
}

void ui_screen_focus(ui_screen* s, ui_node* n)
{
    s->focused = n;
}

ui_node* ui_screen_focused(ui_screen* s)
{
    return s->focused;
}

/* Defined later (with the rest of <window> geometry handling) - forward
 * declared so ui_screen_show_modal() below can center a dialog on open. */
static void set_window_rect(ui_node* window, int new_x, int new_y, int new_w, int new_h);

/* Defined later (with dock_layout, right after set_window_rect) - forward
 * declared so ui_screen_show_window()/ui_screen_close_modal() below can
 * reflow the Folder/Output docks and every maximized window the instant a
 * dock is shown or hidden, rather than leaving it stale until whatever
 * next calls ui_screen_update() (which may not be until the next repaint
 * or the next OS event, i.e. visibly late - showing/hiding a dock is
 * itself the event that should drive the reflow). */
static void dock_layout(ui_screen* s);
static void resync_maximized_windows(ui_screen* s);

/* Center `modal`'s <window> on the current screen - every blocking dialog
 * opens centered, like a native modal dialog, regardless of where it was
 * left the last time it was shown (or wherever it sits in the document). */
static void center_modal_window(ui_screen* s, ui_node* modal)
{
    ui_node* window = find_child_by_type(modal, UI_TAG_WINDOW);
    if (!window)
        return;
    int cx = (s->screen_w - window->w) / 2;
    int cy = (s->screen_h - window->h) / 2;
    if (cx < 0) cx = 0;
    if (cy < 0) cy = 0;
    set_window_rect(window, cx, cy, window->w, window->h);
}

/* First focusable node (input/editor/listbox/group) in `n`'s subtree, in
 * document order, or NULL - so a freshly opened dialog can put the caret in
 * its first field without the user having to click it. */
static int is_focusable(int type);

static ui_node* first_focusable(ui_node* n)
{
    if (!n)
        return NULL;
    for (int i = 0; i < n->child_count; i++)
    {
        ui_node* c = n->children[i];
        if (c->enabled && is_focusable(c->type))
            return c;
        ui_node* deep = first_focusable(c);
        if (deep)
            return deep;
    }
    return NULL;
}

void ui_screen_show_modal(ui_screen* s, ui_node* modal)
{
    if (s->modal_stack_count < UI_MODAL_STACK_MAX)
        s->modal_stack[s->modal_stack_count++] = modal;
    center_modal_window(s, modal);
    s->open_menu = NULL;
    s->open_select = NULL;
    /* Focus the dialog's first field (its "Text to Find" input, etc.) so
     * typing goes there immediately - no click needed. NULL if it has none
     * (e.g. an About box with only a button). */
    s->focused = first_focusable(find_child_by_type(modal, UI_TAG_WINDOW));
    s->dragging_window = NULL;
    s->resizing_window = NULL;
    s->dragging_editor_vscrollbar = NULL;
    s->dragging_editor_hscrollbar = NULL;
    s->dragging_listbox_scrollbar = NULL;
}

void ui_message_box(ui_screen* s, const char* caption, const char* text,
                    const ui_msgbox_button* buttons, int count)
{
    if (count < 1)
        return;

    /* Measure the body: line count and the widest line. */
    int lines = 1, maxw = 0, cur = 0;
    for (const char* p = text; ; p++)
    {
        if (*p == '\n' || *p == '\0')
        {
            if (cur > maxw) maxw = cur;
            cur = 0;
            if (*p == '\0') break;
            lines++;
        }
        else
        {
            cur++;
        }
    }

    /* Measure the button row: each button is its label plus padding, min 8
     * wide, separated by a 2-cell gap. */
    int btnw[16];
    if (count > 16) count = 16;
    int roww = 0;
    for (int i = 0; i < count; i++)
    {
        int lw = (int)strlen(buttons[i].label) + 4;
        btnw[i] = lw < 8 ? 8 : lw;
        roww += btnw[i] + (i ? 2 : 0);
    }

    int capw = (int)strlen(caption) + 2;
    int inner = maxw;
    if (roww > inner) inner = roww;
    if (capw > inner) inner = capw;
    int w = inner + 4;             /* 2-cell padding each side */
    int h = lines + 6;             /* border + text + gap + button row + border */

    ui_node* modal = ui_create_element(UI_TAG_MODAL);
    modal->msgbox = 1;
    ui_append_child(s->root, modal);

    ui_node* win = ui_create_element(UI_TAG_WINDOW);
    ui_set_rect(win, 0, 0, w, h);  /* show_modal below re-centers it */
    ui_set_label(win, caption);
    ui_set_color(win, g_theme.modal_fg, g_theme.modal_bg);
    ui_append_child(modal, win);

    /* Body text, one <text> node per line. */
    int ln = 0;
    const char* start = text;
    for (const char* p = text; ; p++)
    {
        if (*p == '\n' || *p == '\0')
        {
            int len = (int)(p - start);
            char buf[512];
            if (len > (int)sizeof buf - 1) len = (int)sizeof buf - 1;
            memcpy(buf, start, (size_t)len);
            buf[len] = '\0';
            ui_node* t = ui_create_element(UI_TAG_TEXT);
            ui_set_rect(t, 2, 2 + ln, len, 1);
            ui_set_label(t, buf);
            ui_set_color(t, g_theme.modal_fg, g_theme.modal_bg);
            ui_append_child(win, t);
            ln++;
            if (*p == '\0') break;
            start = p + 1;
        }
    }

    /* Buttons, centered on the row above the bottom border. */
    int bx = (w - roww) / 2;
    if (bx < 1) bx = 1;
    int by = 2 + lines + 1;
    for (int i = 0; i < count; i++)
    {
        ui_node* b = ui_create_element(UI_TAG_BUTTON);
        ui_set_id(b, buttons[i].id);
        ui_set_rect(b, bx, by, btnw[i], 1);
        ui_set_label(b, buttons[i].label);
        ui_append_child(win, b);
        bx += btnw[i] + 2;
    }

    ui_screen_show_modal(s, modal);
}

void ui_screen_show_window(ui_screen* s, ui_node* modal)
{
    /* Already open (a singleton dialog like About/Output being re-triggered
     * from its menu item) - just bring it to the front instead of adding a
     * second copy. A brand new node (e.g. File > New handing over a freshly
     * created editor wrapper each time) always ends up appended, so the same
     * call site naturally supports both "one instance" and "many instances"
     * widgets depending on whether the caller reuses a node or creates one. */
    int idx = -1;
    for (int i = 0; i < s->window_count; i++)
    {
        if (s->windows[i] == modal)
        {
            idx = i;
            break;
        }
    }
    if (idx >= 0)
    {
        for (int j = idx; j < s->window_count - 1; j++)
            s->windows[j] = s->windows[j + 1];
        s->windows[s->window_count - 1] = modal;
    }
    else if (s->window_count < UI_WINDOW_STACK_MAX)
    {
        s->windows[s->window_count++] = modal;
    }
    else
    {
        /* Stack already full (UI_WINDOW_STACK_MAX) - evict the oldest
         * floating window, windows[0] (the bottom of the z-order - see the
         * back-to-front comment on the array's own declaration), to make
         * room instead of overflowing the array. window_count stays at
         * UI_WINDOW_STACK_MAX: one dropped from the front, one added at the
         * back. The evicted window is just no longer tracked/rendered as a
         * floating window (same visible effect as closing it) - nothing is
         * freed here, so a still-open document window pushed out this way
         * can still be found again by path and reopened normally later. */
        for (int j = 0; j < s->window_count - 1; j++)
            s->windows[j] = s->windows[j + 1];
        s->windows[s->window_count - 1] = modal;
    }
    s->open_menu = NULL;
    s->open_select = NULL;
    s->focused = NULL;
    s->dragging_window = NULL;
    s->resizing_window = NULL;
    s->dragging_editor_vscrollbar = NULL;
    s->dragging_editor_hscrollbar = NULL;
    s->dragging_listbox_scrollbar = NULL;

    /* If `modal` wraps a docked Folder/Output-style window, showing it
     * changes how much desktop space every other dock and every maximized
     * window gets - reflow right now, not next frame (see the forward
     * declaration above for why waiting for ui_screen_update() is wrong). */
    dock_layout(s);
    resync_maximized_windows(s);
}

ui_node* ui_screen_top_window(ui_screen* s)
{
    if (s->window_count == 0)
        return NULL;
    return s->windows[s->window_count - 1];  /* frontmost - see ui_screen_show_window */
}

void ui_screen_close_modal(ui_screen* s, ui_node* modal)
{
    for (int i = 0; i < s->window_count; i++)
    {
        if (s->windows[i] == modal)
        {
            for (int j = i; j < s->window_count - 1; j++)
                s->windows[j] = s->windows[j + 1];
            s->window_count--;
            break;
        }
    }
    for (int i = 0; i < s->modal_stack_count; i++)
    {
        if (s->modal_stack[i] == modal)
        {
            for (int j = i; j < s->modal_stack_count - 1; j++)
                s->modal_stack[j] = s->modal_stack[j + 1];
            s->modal_stack_count--;
            break;
        }
    }
    s->dragging_window = NULL;
    s->resizing_window = NULL;
    s->dragging_editor_vscrollbar = NULL;
    s->dragging_editor_hscrollbar = NULL;
    s->dragging_listbox_scrollbar = NULL;
    s->selecting = NULL;
    s->open_select = NULL;
    s->focused = NULL;  /* an input inside the closing window can't stay focused */

    /* Symmetric with ui_screen_show_window(): if `modal` wrapped a docked
     * Folder/Output-style window, hiding it just gave that space back to
     * every other dock and every maximized window - reflow right now. */
    dock_layout(s);
    resync_maximized_windows(s);

    /* A transient window (see ui_set_transient) is a one-off - record it for
     * the app to actually tear down via ui_screen_take_closed_window(), once
     * it's had a chance to clear any of its own pointers to it. Everything
     * above this point still runs exactly as it always has (this window is
     * every bit as "closed" - out of windows[]/modal_stack, focus cleared,
     * docks reflowed) whether or not it's transient; this is purely about
     * what happens to the node itself afterward. */
    if (modal->transient)
        s->window_closed = modal;
}

ui_node* ui_screen_take_closed_window(ui_screen* s)
{
    ui_node* w = s->window_closed;
    s->window_closed = NULL;
    return w;
}

ui_node* ui_screen_active_modal(ui_screen* s)
{
    return s->modal_stack_count > 0 ? s->modal_stack[s->modal_stack_count - 1] : NULL;
}

void ui_screen_open_popup(ui_screen* s, ui_node* menu, int x, int y, void* param)
{
    if (!menu)
        return;
    /* Reuse the whole dropdown machinery: a popup is just a <menu> shown at an
     * arbitrary spot instead of under a menubar header. layout_dropdown() draws
     * the box at menu->x, menu->y + 1, so bias y up by one to put the box's
     * top-left at (x, y). */
    menu->x = x;
    menu->y = y - 1;
    s->open_menu = menu;
    s->open_submenu = NULL;
    s->open_select = NULL;
    s->focused = NULL;
    s->open_popup_param = param;
}

/* --- Update: hit-testing + event firing --- */

/* Shared press/release/hover bookkeeping for one interactive node. Sets
 * s->hot if the mouse is inside `inside`, tracks s->active across the
 * press-then-release span, and queues the node's id on a completed click.
 * `consumed` (if non-NULL) is set to 1 if this frame's click landed on the
 * node - used by the menu to know whether a click should close it. Returns 1
 * on the frame a click completes on this node (release while still hot). */
static int hit_test_widget(ui_screen* s, ui_node* n, int inside, int* consumed, void* param)
{
    if (!n->enabled)
        return 0;  /* never hot/active/clicked - like a disabled DOM element */

    int clicked = 0;
    if (inside)
    {
        s->hot = n;
        if (s->mouse_pressed && consumed)
            *consumed = 1;
    }
    /* Handle press before release: if both a press and release drained into
     * the same update() call (a click faster than one frame), this lets the
     * release check see the active state the press *just* set, so the click
     * still completes in this call instead of leaving the node stuck active
     * forever (no future release event would arrive to clear it). */
    if (s->hot == n && s->mouse_pressed && s->active != n)
        s->active = n;
    if (s->active == n && s->mouse_released)
    {
        if (s->hot == n)
        {
            ui_fire_event(s, n->id, param);
            clicked = 1;
        }
        s->active = NULL;
    }
    return clicked;
}

/* --- <input>/<editor> text editing: caret movement and byte-level value
 * edits. n->cursor is a byte offset into n->label; all edits keep it aligned
 * to a UTF-8 codepoint boundary. No length cap - each edit allocates a
 * buffer sized exactly to the new content (old length plus/minus the edit),
 * so a <editor> document grows as large as there's memory for, the same way
 * n->label itself already does. --- */

 /* Encode one code point as UTF-8 into out (up to 4 bytes). Returns the byte
  * count written - the inverse of utf8_decode() above. */
static int utf8_encode(uint32_t cp, char out[4])
{
    if (cp < 0x80)
    {
        out[0] = (char)cp;
        return 1;
    }
    else if (cp < 0x800)
    {
        out[0] = (char)(0xC0 | (cp >> 6));
        out[1] = (char)(0x80 | (cp & 0x3F));
        return 2;
    }
    else if (cp < 0x10000)
    {
        out[0] = (char)(0xE0 | (cp >> 12));
        out[1] = (char)(0x80 | ((cp >> 6) & 0x3F));
        out[2] = (char)(0x80 | (cp & 0x3F));
        return 3;
    }
    out[0] = (char)(0xF0 | (cp >> 18));
    out[1] = (char)(0x80 | ((cp >> 12) & 0x3F));
    out[2] = (char)(0x80 | ((cp >> 6) & 0x3F));
    out[3] = (char)(0x80 | (cp & 0x3F));
    return 4;
}

/* Byte length of the codepoint ending right before byte offset `pos` in `s` -
 * for moving/deleting backward one codepoint at a time. */
static int utf8_prev_len(const char* s, int pos)
{
    int len = 1;
    while (pos - len > 0 && ((unsigned char)s[pos - len] & 0xC0) == 0x80)
        len++;
    return len;
}

/* Codepoint count of s[0..byte_pos) - the caret's column position. */
static int utf8_col_of(const char* s, int byte_pos)
{
    int col = 0;
    const char* p = s;
    while (*p && (p - s) < byte_pos)
    {
        uint32_t cp;
        p += utf8_decode(p, &cp);
        col++;
    }
    return col;
}

/* --- Read-only Markdown "true removal" column mapping ---------------------
 * render_editor_line_markdown() further down hides the same handful of
 * Markdown delimiters copy/paste already strips for a read-only document
 * (see strip_markdown_readonly() and editor_selected_text()) - fenced code
 * block "```" markers, inline `code span` backticks, **bold** markers,
 * <!-- html comments --> and [link text](dest) brackets/parens+url - by
 * actually removing them from the rendered line instead of leaving a blank
 * gap the width they used to occupy, so a read-only Markdown document reads
 * like a real preview rather than source-with-holes-punched-in-it.
 *
 * That means a screen column no longer lines up 1:1 with a raw source
 * codepoint the way it does for every other syntax mode (and the way
 * utf8_col_of() above assumes) - cursor placement, click hit-testing,
 * selection-highlight bounds and the horizontal scrollbar's width all need
 * to agree with the *collapsed* column a byte offset ends up drawn at, or
 * they'd silently drift out of sync with what render_editor_line_markdown()
 * actually puts on screen: a caret drawn over the wrong glyph, a selection
 * that doesn't match what was dragged over, or a scrollbar that under/
 * overshoots the visible text. md_scan_line() below is the one place that
 * answers "what column does this byte end up at" and "what byte ends up at
 * this column", by replaying the exact same hide rules
 * render_editor_line_markdown() draws with (the two are kept in sync
 * deliberately, same convention as strip_markdown_readonly() uses with the
 * renderer - see the comment there). Every read/click/scroll site that
 * needs this translation for a read-only Markdown document calls through
 * here instead of re-deriving its own copy of the hide rules. */

static int scan_markdown_fence_state(const char* line, int len, int in_block);

/* Answers exactly one of three questions about one line, given `in_block`
 * (the fenced-code-block state entering the line - threaded line-to-line by
 * the caller, same in/out convention as scan_markdown_fence_state(), which
 * this updates identically for a "```" line):
 *
 *  - byte_target >= 0, col_target < 0: returns the visual column the raw
 *    byte offset `byte_target` is drawn at (a byte inside a hidden run
 *    collapses to the column right before that run, since nothing inside
 *    the run advances a column).
 *  - byte_target < 0, col_target >= 0: returns the raw byte offset of
 *    whichever visible codepoint is drawn at visual column `col_target`
 *    (or line_len, the line's own end, if that column is at or past the
 *    end of the visible line - clicking/arrowing past the visible text
 *    lands at the line's end, same as every other syntax mode).
 *  - both < 0: returns the line's total visible width in columns (for
 *    editor_content_cols()'s scrollbar-width purposes). */
static int md_scan_line(const char* line, int line_len, int* in_block,
                         int byte_target, int col_target)
{
    int j = 0;
    while (j < line_len && (line[j] == ' ' || line[j] == '\t'))
        j++;
    int is_fence = (line_len - j >= 3 && line[j] == '`' && line[j + 1] == '`' && line[j + 2] == '`');
    if (is_fence)
        *in_block = !*in_block;
    int in_code_block = *in_block || is_fence;

    int col = 0, i = 0, in_span = 0, in_link_text = 0, in_link_dest = 0, in_html_comment = 0;
    while (i < line_len)
    {
        if (byte_target >= 0 && i >= byte_target)
            return col;

        uint32_t cp;
        int clen = utf8_decode(line + i, &cp);
        int hide_char = 0, skip_extra = 0;

        if (in_html_comment)
        {
            hide_char = 1;
            if (i + 2 < line_len && line[i] == '-' && line[i + 1] == '-' && line[i + 2] == '>')
            {
                skip_extra = 2;
                in_html_comment = 0;
            }
        }
        else if (is_fence)
        {
            hide_char = 1;
        }
        else if (!in_code_block)
        {
            if (cp == '`')
            {
                hide_char = 1;
                in_span = !in_span;
            }
            else if (cp == '*' && i + 1 < line_len && line[i + 1] == '*')
            {
                hide_char = 1;
                skip_extra = 1;
            }
            else if (cp == '<' && i + 3 < line_len && line[i + 1] == '!' &&
                     line[i + 2] == '-' && line[i + 3] == '-')
            {
                in_html_comment = 1;
                hide_char = 1;
                skip_extra = 3;
            }
            else if (cp == '[')
            {
                const char* p = line + i + 1;
                while (p < line + line_len && *p != ']' && *p != '\n')
                    p++;
                if (p < line + line_len && *p == ']' && p + 1 < line + line_len && p[1] == '(')
                {
                    in_link_text = 1;
                    hide_char = 1;
                }
            }
            else if (cp == ']' && in_link_text)
            {
                in_link_text = 0;
                hide_char = 1;
                if (i + 1 < line_len && line[i + 1] == '(')
                    in_link_dest = 1;
            }
            else if (in_link_dest)
            {
                hide_char = 1;
                if (cp == ')')
                    in_link_dest = 0;
            }
        }

        if (!hide_char)
        {
            if (col_target >= 0 && col == col_target)
                return i;
            col++;
        }
        i += clen + skip_extra;
    }

    if (byte_target >= 0)
        return col;
    if (col_target >= 0)
        return line_len;
    return col;
}

/* Fenced-code-block state at the start of the line beginning at byte offset
 * `line_start` in `text` - replays scan_markdown_fence_state() over every
 * earlier line, the same priming editor_selected_text() does before calling
 * strip_markdown_readonly() on a selection that starts mid-document. Used
 * wherever md_scan_line() is needed outside render_editor()'s own top-to-
 * bottom repaint loop (which already threads this state line-to-line as it
 * goes) - a click, or a caret Up/Down move - so those agree with what's
 * drawn even for a line the last repaint didn't necessarily just scan up
 * to. */
static int md_in_block_at(const char* text, int line_start)
{
    int in_block = 0, off = 0;
    while (off < line_start)
    {
        int ls = off;
        while (off < line_start && text[off] != '\n')
            off++;
        in_block = scan_markdown_fence_state(text + ls, off - ls, in_block);
        if (off < line_start)
            off++;
    }
    return in_block;
}

/* --- Read-only Markdown vertical line-folding --------------------------
 * A fenced code block's own delimiter line ("```" or "```lang") and a
 * comment-only metadata line (e.g. "<!-- runnable -->" on its own, marking
 * the block right after it) are, in read-only/preview mode, removed
 * entirely rather than just having their text hidden the way
 * render_editor_line_markdown() collapses inline delimiters (see the
 * md_scan_line() block comment above) - they don't just draw empty, they
 * take up no row at all, so the surrounding content closes the gap instead
 * of leaving a blank line behind. That means, for a read-only Markdown
 * document, a screen ROW no longer lines up 1:1 with a document LINE the
 * way it does for every other syntax mode - render_editor()'s per-row
 * loop, click-to-line mapping (editor_row_to_line), Up/Down (editor_move_
 * lines), and the vertical scrollbar (editor_clamp_scroll/editor_has_
 * vscrollbar/editor_vscrollbar_thumb/editor_visible_line_count) all need
 * to skip a foldable line rather than assign it a row/count it, or they'd
 * silently disagree with each other - a caret landing a row off from a
 * click, a scrollbar sized for rows that are never actually shown, or
 * Up/Down leaving the caret stranded on a line that never gets painted.
 * md_is_foldable_line()/md_line_at_index_is_foldable() below are the one
 * place that answers "is this line folded away" - kept in sync with
 * render_editor_line_markdown()'s own hide rules deliberately, same
 * convention as md_scan_line() uses with the renderer. */

/* Whether `line`[0, line_len) is a fenced code block delimiter, given its
 * byte range directly - the form every caller that's already walking the
 * document line-by-line (and so already has [ls, le) in hand) can use
 * without re-locating the line first. */
static int md_is_fence_line(const char* line, int line_len)
{
    int j = 0;
    while (j < line_len && (line[j] == ' ' || line[j] == '\t'))
        j++;
    return line_len - j >= 3 && line[j] == '`' && line[j + 1] == '`' && line[j + 2] == '`';
}

/* Whether an OPENING fence line (already confirmed via md_is_fence_line())
 * tags its block as C - "```c" or "```C", nothing else after the language
 * tag but trailing whitespace/'\r'. Used by render_editor() to decide
 * whether that block's content lines get highlighted with the real C
 * highlighter (render_editor_line(), same as a UI_SYNTAX_C file) instead of
 * Markdown's own flat md_code_fg - see the block comment above render_
 * editor()'s row loop where in_c_block is threaded. Meaningless on a
 * closing "```" (no tag to read); callers only ask this when a block is
 * opening in the first place. */
static int md_fence_lang_is_c(const char* line, int line_len)
{
    int j = 0;
    while (j < line_len && (line[j] == ' ' || line[j] == '\t'))
        j++;
    j += 3;  /* past the "```" itself - already confirmed present by the caller */
    int k = line_len;
    while (k > j && (line[k - 1] == ' ' || line[k - 1] == '\t' || line[k - 1] == '\r'))
        k--;
    return k - j == 1 && (line[j] == 'c' || line[j] == 'C');
}

/* Whether `line`[0, line_len) is ENTIRELY a single-line HTML comment
 * ("<!-- ... -->", nothing else outside it, e.g. a "<!-- runnable -->"
 * metadata annotation marking the code block right after it) - foldable
 * the same way a fence delimiter line is (see md_is_foldable_line() just
 * below), not just the inline "hide the delimiters, keep the row"
 * treatment render_editor_line_markdown() gives an <!-- --> that shares a
 * line with real content. Trims trailing '\r' too, so this still matches
 * on a CRLF-saved document (lines are only ever split on '\n' - see every
 * line-walking loop in this file). */
static int md_is_comment_only_line(const char* line, int line_len)
{
    int j = 0, k = line_len;
    while (j < k && (line[j] == ' ' || line[j] == '\t'))
        j++;
    while (k > j && (line[k - 1] == ' ' || line[k - 1] == '\t' || line[k - 1] == '\r'))
        k--;
    if (k - j < 7)  /* "<!--" (4) + "-->" (3) is the shortest possible match */
        return 0;
    return line[j] == '<' && line[j + 1] == '!' && line[j + 2] == '-' && line[j + 3] == '-' &&
           line[k - 1] == '>' && line[k - 2] == '-' && line[k - 3] == '-';
}

/* Whether `line`[0, line_len) should be removed entirely (no row at all)
 * from a read-only Markdown preview - a fence delimiter or a comment-only
 * line, the two cases md_is_fence_line()/md_is_comment_only_line() cover
 * respectively. Every vertical-folding call site (render_editor()'s row
 * loop, editor_row_to_line(), editor_move_lines(), editor_visible_line_
 * count()) goes through this one function rather than checking each case
 * separately, so a third foldable-line kind only ever needs adding here. */
static int md_is_foldable_line(const char* line, int line_len)
{
    return md_is_fence_line(line, line_len) || md_is_comment_only_line(line, line_len);
}

/* Whether 0-based document line `line_idx` in `text` is foldable (see
 * md_is_foldable_line()) - the by-index form md_line_at_index_is_fence()
 * above provides for a fence line alone; used by callers (a click's target
 * row, Up/Down's target line) that only have a line index in hand, not
 * its byte range. */
static int md_line_at_index_is_foldable(const char* text, int line_idx)
{
    int idx = 0, off = 0;
    int len = (int)strlen(text);
    while (off <= len)
    {
        int ls = off;
        while (off < len && text[off] != '\n')
            off++;
        if (idx == line_idx)
            return md_is_foldable_line(text + ls, off - ls);
        if (off >= len)
            break;
        off++;
        idx++;
    }
    return 0;
}

/* Wall-clock ms for the edit currently being processed - set once per
 * ui_screen_update() call from the env's time (see ui_env_set_time_ms) and
 * read by editor_undo_mark() below to decide whether an edit is close
 * enough in time to a prior one of the same kind to coalesce with it. Safe
 * as a file-static: ui.c has no threading, and every edit happens inside
 * ui_screen_update()'s single-frame key-handling loop where this is valid. */
static unsigned g_ui_undo_now_ms = 0;

/* Append a snapshot to a growable undo/redo array (shared by both -
 * `arr`/`count`/`cap` are whichever of the pair is being pushed to). Once
 * full, grows up to UI_UNDO_MAX entries, then evicts the oldest instead of
 * growing further. Takes ownership of `text`. */
static void undo_array_push(ui_undo_entry** arr, int* count, int* cap,
                             char* text, int cursor, int sel_anchor)
{
    if (*count == *cap)
    {
        if (*cap >= UI_UNDO_MAX)
        {
            free((*arr)[0].text);
            memmove(&(*arr)[0], &(*arr)[1],
                    (size_t)(*count - 1) * sizeof * *arr);
            (*count)--;
        }
        else
        {
            int newcap = *cap ? *cap * 2 : 16;
            if (newcap > UI_UNDO_MAX)
                newcap = UI_UNDO_MAX;
            *arr = realloc(*arr, (size_t)newcap * sizeof * *arr);
            *cap = newcap;
        }
    }
    ui_undo_entry* e = &(*arr)[(*count)++];
    e->text = text;
    e->cursor = cursor;
    e->sel_anchor = sel_anchor;
}

/* Free every entry in an undo/redo array and reset its count (capacity/
 * allocation are left alone - the array itself is reused). */
static void undo_array_clear(ui_undo_entry** arr, int* count)
{
    for (int i = 0; i < *count; i++)
        free((*arr)[i].text);
    *count = 0;
}

/* Push `n`'s current content onto its undo stack - the "before" snapshot for
 * whatever edit is about to happen - and clear its redo stack, the ordinary
 * "a new edit invalidates whatever was undone" rule. */
static void editor_undo_push(ui_node* n)
{
    undo_array_push(&n->undo_stack, &n->undo_count, &n->undo_cap,
                     xstrdup(n->label), n->cursor, n->sel_anchor);
    undo_array_clear(&n->redo_stack, &n->redo_count);
}

/* Call right before every content-changing operation on an EDITOR (never on
 * INPUT - single-line fields don't get undo history). `kind` identifies the
 * operation; if it matches the kind, cursor position, and recency of the
 * edit currently being grouped, this extends that group instead of starting
 * a new one - see the UI_UNDO_* enum and UI_UNDO_COALESCE_MS. UI_UNDO_NEWLINE
 * and UI_UNDO_DELSEL never coalesce, not even with a run of their own kind -
 * each Enter, and each selection erase, is always its own undo step. Always
 * call editor_undo_settle() after the mutation completes. */
static void editor_undo_mark(ui_node* n, int kind)
{
    if (n->type != UI_TAG_EDITOR)
        return;

    int coalesce = kind == n->undo_group &&
        kind != UI_UNDO_NEWLINE && kind != UI_UNDO_DELSEL &&
        n->cursor == n->undo_group_pos &&
        (g_ui_undo_now_ms - n->undo_group_time) <= UI_UNDO_COALESCE_MS;
    if (!coalesce)
        editor_undo_push(n);

    n->undo_group = kind;
    n->undo_group_time = g_ui_undo_now_ms;
}

/* Call right after a marked mutation completes, once the cursor has settled
 * at its final position - records where a *continuing* edit of the same
 * kind must start from for editor_undo_mark()'s next coalescing check. */
static void editor_undo_settle(ui_node* n)
{
    if (n->type == UI_TAG_EDITOR)
        n->undo_group_pos = n->cursor;
}

static void input_insert(ui_node* n, uint32_t cp, int undo_kind)
{
    if (n->read_only)
        return;
    if (n->numeric && (cp < '0' || cp > '9'))
        return;

    editor_undo_mark(n, undo_kind);

    char enc[4];
    int enclen = utf8_encode(cp, enc);
    size_t oldlen = strlen(n->label);

    char* buf = malloc(oldlen + (size_t)enclen + 1);
    memcpy(buf, n->label, n->cursor);
    memcpy(buf + n->cursor, enc, enclen);
    strcpy(buf + n->cursor + enclen, n->label + n->cursor);
    ui_set_label(n, buf);
    free(buf);
    n->cursor += enclen;
    editor_undo_settle(n);

    if (n->type == UI_TAG_EDITOR)
    {
        n->dirty = 1;
        /* Stale as of this keystroke - line numbers below the edit may have
         * already shifted, and the diagnostic's own line may no longer even
         * say what it used to. Cleared outright rather than re-mapped: the
         * app re-populates them on the next compile (see do_compile()'s own
         * ui_editor_clear_diagnostics() call, now redundant but harmless). */
        ui_editor_clear_diagnostics(n);
    }
}

static void input_backspace(ui_node* n)
{
    if (n->read_only || n->cursor == 0)
        return;

    editor_undo_mark(n, UI_UNDO_BACKSPACE);

    int len = utf8_prev_len(n->label, n->cursor);

    char* buf = malloc(strlen(n->label) + 1);
    memcpy(buf, n->label, n->cursor - len);
    strcpy(buf + n->cursor - len, n->label + n->cursor);
    ui_set_label(n, buf);
    free(buf);
    n->cursor -= len;
    editor_undo_settle(n);

    if (n->type == UI_TAG_EDITOR)
    {
        n->dirty = 1;
        ui_editor_clear_diagnostics(n);  /* see input_insert()'s own comment */
    }
}

static void input_delete_forward(ui_node* n)
{
    if (n->read_only)
        return;
    int len = (int)strlen(n->label);
    if (n->cursor >= len)
        return;

    editor_undo_mark(n, UI_UNDO_DELFWD);

    uint32_t cp;
    int clen = utf8_decode(n->label + n->cursor, &cp);

    char* buf = malloc((size_t)len + 1);
    memcpy(buf, n->label, n->cursor);
    strcpy(buf + n->cursor, n->label + n->cursor + clen);
    ui_set_label(n, buf);
    free(buf);
    editor_undo_settle(n);

    if (n->type == UI_TAG_EDITOR)
    {
        n->dirty = 1;
        ui_editor_clear_diagnostics(n);  /* see input_insert()'s own comment */
    }
}

/* Place an <input>'s cursor at click_col, a column relative to the widget's
 * left edge - accounting for the same horizontal scroll render_input()
 * itself derives from the (pre-click) cursor position. */
static void input_click_set_cursor(ui_node* n, int click_col)
{
    int cursor_col = utf8_col_of(n->label, n->cursor);
    int offset = cursor_col >= n->w ? cursor_col - n->w + 1 : 0;
    int target_col = offset + click_col;

    int off = 0, col = 0;
    while (col < target_col && n->label[off])
    {
        uint32_t cp;
        off += utf8_decode(n->label + off, &cp);
        col++;
    }
    n->cursor = off;
}

/* --- <editor> line navigation: its value is just <input>'s same flat UTF-8
 * string with '\n' bytes embedded - insert/backspace/delete-forward above
 * already handle that correctly (a '\n' is just another codepoint), so only
 * line-aware movement needs its own logic. --- */

 /* The [start, end) byte range of line `line_idx` in `text` (0 = first line).
  * Returns 0 if that line doesn't exist (line_idx past the last line). */
static int editor_line_range(const char* text, int line_idx, int* out_start, int* out_end)
{
    int i = 0, line = 0;
    int len = (int)strlen(text);
    while (line < line_idx)
    {
        while (i < len && text[i] != '\n')
            i++;
        if (i >= len)
            return 0;
        i++;  /* skip the '\n' */
        line++;
    }
    int start = i;
    while (i < len && text[i] != '\n')
        i++;
    *out_start = start;
    *out_end = i;
    return 1;
}

/* Which line n->cursor is on, and that line's starting byte offset. */
static void editor_cursor_line(ui_node* n, int* out_line, int* out_line_start)
{
    int line = 0, line_start = 0;
    for (int i = 0; i < n->cursor; i++)
    {
        if (n->label[i] == '\n')
        {
            line++;
            line_start = i + 1;
        }
    }
    *out_line = line;
    *out_line_start = line_start;
}

/* Count of '\n'-separated lines in `text` (a text with no '\n' is 1 line). */
static int editor_line_count(const char* text)
{
    int n = 1;
    for (const char* p = text; *p; p++)
        if (*p == '\n')
            n++;
    return n;
}

/* Global ON/OFF switch for the line-number gutter (see ui_set_show_line_
 * numbers/ui_get_show_line_numbers in ide_ui.h) - defaults ON. Not per-
 * editor and not persisted, same as the app's theme choice. */
static int g_show_line_numbers = 1;

void ui_set_show_line_numbers(int on)
{
    g_show_line_numbers = on ? 1 : 0;
}

int ui_get_show_line_numbers(void)
{
    return g_show_line_numbers;
}

/* Width in columns of `n`'s line-number gutter, 0 when the feature is off
 * (ui_set_show_line_numbers) or `n` isn't a plain C source editor -
 * UI_SYNTAX_VT100 (compiler/terminal output) and UI_SYNTAX_MARKDOWN (README/
 * help viewer - prose, not source lines worth numbering) both stay gutter-
 * less, same as View > "Line Numbers" itself only being enabled while the
 * frontmost document is code (see g_view_linenumbers_item's own doc comment
 * in ide.c). Wide enough for the document's last line number plus a one-
 * column gap, with a 3-digit (+ gap) floor so a short document doesn't get
 * a cramped 2-wide gutter - every render_editor() row, plus every hscroll/
 * click/scrollbar computation below that has to agree on where the actual
 * text starts, calls this so they can never drift apart. */
static int editor_gutter_width(const ui_node* n)
{
    if (!g_show_line_numbers || n->syntax != UI_SYNTAX_C)
        return 0;
    int total = editor_line_count(n->label);
    int digits = 1;
    for (int t = total; t >= 10; t /= 10)
        digits++;
    if (digits < 3)
        digits = 3;
    return digits + 1;  /* + 1 column gap before the text */
}

/* `n`'s text viewport width - n->w minus whatever the gutter (just above)
 * currently claims - clamped at 0 so a pathologically narrow editor can't
 * go negative. Every place that used to treat n->w as "how many text
 * columns are visible" (hscroll clamping, the horizontal scrollbar, caret-
 * follow scrolling) uses this instead once a gutter can be in the way. */
static int editor_text_w(const ui_node* n)
{
    int w = n->w - editor_gutter_width(n);
    return w > 0 ? w : 0;
}

/* Move the cursor by `delta` lines (negative = up, positive = down),
 * keeping its column as close as possible (clamped to the target line's
 * length) - the standard editor convention. Clamped to the document's first/
 * last line rather than doing nothing when the delta overshoots, so Page
 * Up/Down near either end still lands somewhere instead of being a no-op. */
static void editor_move_lines(ui_node* n, int delta)
{
    int line, line_start;
    editor_cursor_line(n, &line, &line_start);

    int target = line + delta;
    if (target < 0)
        target = 0;
    int last = editor_line_count(n->label) - 1;
    if (target > last)
        target = last;

    /* Never land the caret on a fence/comment-only line for a read-only
     * Markdown document - render_editor() never draws a row for one (see
     * the md_is_foldable_line() block comment above), so the caret would
     * just vanish there instead of visibly moving. Keep stepping the same
     * direction Up/Down was already moving until landing on a real line,
     * or hitting whichever end of the document is closer. */
    if (n->syntax == UI_SYNTAX_MARKDOWN && n->read_only)
    {
        int step = delta >= 0 ? 1 : -1;
        while (target > 0 && target < last && md_line_at_index_is_foldable(n->label, target))
            target += step;
    }

    int ls, le;
    if (!editor_line_range(n->label, target, &ls, &le))
        return;

    /* For a read-only Markdown document, keep the caret's *visual* column
     * (post-collapse - see the md_scan_line() block comment above) rather
     * than its raw source column, so Up/Down actually tracks the column the
     * caret appears under on screen instead of drifting sideways whenever
     * the two lines hide a different number of delimiters. */
    if (n->syntax == UI_SYNTAX_MARKDOWN && n->read_only)
    {
        int src_end = line_start;
        while (n->label[src_end] && n->label[src_end] != '\n')
            src_end++;
        int src_block = md_in_block_at(n->label, line_start);
        int col = md_scan_line(n->label + line_start, src_end - line_start, &src_block,
                                n->cursor - line_start, -1);
        int dst_block = md_in_block_at(n->label, ls);
        n->cursor = ls + md_scan_line(n->label + ls, le - ls, &dst_block, -1, col);
        return;
    }

    int col = utf8_col_of(n->label + line_start, n->cursor - line_start);
    int off = ls, c = 0;
    while (c < col && off < le)
    {
        uint32_t cp;
        off += utf8_decode(n->label + off, &cp);
        c++;
    }
    n->cursor = off;
}

static void editor_move_vertical(ui_node* n, int dir)
{
    editor_move_lines(n, dir);
}

/* The document line that ends up drawn at screen row `click_row` (0-based,
 * relative to the editor's own scrolled view) - identity (n->scroll +
 * click_row) for every syntax except a read-only Markdown document, where
 * a fence/comment-only line takes no row at all (see the md_is_foldable_
 * line() block comment above) - this mirrors render_editor()'s own row
 * loop skip so a click lands on the same line that's actually drawn there,
 * not on whatever line a naive n->scroll + click_row would name once
 * something above it has been skipped. */
static int editor_row_to_line(const ui_node* n, int click_row)
{
    if (n->syntax != UI_SYNTAX_MARKDOWN || !n->read_only)
        return n->scroll + click_row;

    int total = editor_line_count(n->label);
    int line = n->scroll;
    int shown = 0;
    while (line < total)
    {
        if (md_line_at_index_is_foldable(n->label, line))
        {
            line++;
            continue;
        }
        if (shown == click_row)
            return line;
        shown++;
        line++;
    }
    /* Ran off the end still short of click_row - past the document, same
     * as a plain click below a short document today (editor_line_range()
     * below rejects it and the caller falls back to "clicked past the last
     * line"). */
    return line;
}

/* Place the cursor at (click_row, click_col) within the editor's currently
 * scrolled view - click_row/click_col are relative to the widget's top-left,
 * already adjusted for its own x/y by the caller (still including the
 * gutter, if any - every call site just passes mouse_x - n->x, same as
 * before the gutter existed, so the adjustment lives here in one place
 * instead of at each of them). A click that lands on the gutter itself
 * clamps to column 0 of the text, same as clicking the first visible
 * character would. */
static void editor_click_set_cursor(ui_node* n, int click_row, int click_col)
{
    click_col -= editor_gutter_width(n);
    if (click_col < 0)
        click_col = 0;

    int target = editor_row_to_line(n, click_row);

    int ls, le;
    if (!editor_line_range(n->label, target, &ls, &le))
    {
        n->cursor = (int)strlen(n->label);  /* clicked past the last line */
        return;
    }

    /* click_col is a viewport column; the logical column is offset by the
     * horizontal scroll so clicks land on the character actually shown. */
    int target_col = n->hscroll + click_col;

    /* A read-only Markdown document draws with delimiters actually removed
     * (see the md_scan_line() block comment above render_editor()), so a
     * screen column has to be mapped back to a byte offset the same
     * collapsed way, or a click would land on the wrong character whenever
     * the line hides anything before the click point. */
    if (n->syntax == UI_SYNTAX_MARKDOWN && n->read_only)
    {
        int in_block = md_in_block_at(n->label, ls);
        n->cursor = ls + md_scan_line(n->label + ls, le - ls, &in_block, -1, target_col);
        return;
    }

    int off = ls, col = 0;
    while (col < target_col && off < le)
    {
        uint32_t cp;
        off += utf8_decode(n->label + off, &cp);
        col++;
    }
    n->cursor = off;
}

/* Whether byte `b` is part of a word (identifier) - used for double-click
 * word selection. Byte-level (ASCII) is enough here; UTF-8 continuation
 * bytes (>= 0x80) are treated as word bytes so a multi-byte glyph isn't
 * split mid-word. */
static int is_word_byte(unsigned char b)
{
    return (b >= 'a' && b <= 'z') || (b >= 'A' && b <= 'Z') ||
        (b >= '0' && b <= '9') || b == '_' || b >= 0x80;
}

/* Whitespace for word navigation. Spaces, tabs and newlines are all skipped
 * so a Ctrl+arrow lands on the first real character of the neighbouring
 * token. */
static int is_ws_byte(unsigned char b)
{
    return b == ' ' || b == '\t' || b == '\r' || b == '\n';
}

/* Visual-Studio-style Ctrl+Right: skip the rest of the token under the caret
 * (a run of word bytes, or a run of punctuation), then skip whitespace, so the
 * caret lands on the first non-space character of the next token. A run of
 * punctuation counts as its own token, so the caret stops on it rather than
 * skipping through to the next word. */
static int editor_word_right(const char* s, int len, int pos)
{
    if (pos < len)
    {
        if (is_word_byte((unsigned char)s[pos]))
        {
            while (pos < len && is_word_byte((unsigned char)s[pos]))
                pos++;
        }
        else if (!is_ws_byte((unsigned char)s[pos]))
        {
            while (pos < len && !is_word_byte((unsigned char)s[pos]) &&
                   !is_ws_byte((unsigned char)s[pos]))
                pos++;
        }
    }
    while (pos < len && is_ws_byte((unsigned char)s[pos]))
        pos++;
    return pos;
}

/* Visual-Studio-style Ctrl+Left: skip whitespace to the left, then skip the
 * token before the caret (a run of word bytes, or a run of punctuation),
 * landing on that token's first byte. */
static int editor_word_left(const char* s, int pos)
{
    while (pos > 0 && is_ws_byte((unsigned char)s[pos - 1]))
        pos--;
    if (pos > 0 && is_word_byte((unsigned char)s[pos - 1]))
    {
        while (pos > 0 && is_word_byte((unsigned char)s[pos - 1]))
            pos--;
    }
    else
    {
        while (pos > 0 && !is_word_byte((unsigned char)s[pos - 1]) &&
               !is_ws_byte((unsigned char)s[pos - 1]))
            pos--;
    }
    return pos;
}

/* First byte of the line the caret sits on (Home / macOS Cmd+Left). */
static int editor_line_start(ui_node* n)
{
    int line, line_start;
    editor_cursor_line(n, &line, &line_start);
    return line_start;
}

/* Byte just before the next newline, i.e. the end of the caret's line
 * (End / macOS Cmd+Right). */
static int editor_line_end(ui_node* n)
{
    int i = n->cursor;
    int len = (int)strlen(n->label);
    while (i < len && n->label[i] != '\n')
        i++;
    return i;
}

/* Select the word surrounding the caret (a double-click gesture). Expands
 * left/right over word bytes; a no-op selection if the caret isn't on one. */
static void editor_select_word(ui_node* n)
{
    const char* s = n->label;
    int len = (int)strlen(s);
    int start = n->cursor, end = n->cursor;
    if (start > len) start = end = len;
    while (start > 0 && is_word_byte((unsigned char)s[start - 1]))
        start--;
    while (end < len && is_word_byte((unsigned char)s[end]))
        end++;
    if (start == end)
        return;  /* not on a word - leave the caret as-is */
    n->sel_anchor = start;
    n->cursor = end;
}

/* editor_line_count(), but for a read-only Markdown document a fence/
 * comment-only line doesn't count - it never gets a row of its own (see
 * the md_is_foldable_line() block comment above), so counting it here
 * would size the vertical scrollbar/max-scroll for more rows than the
 * document actually ever shows, letting the view scroll down into blank
 * space past the real end of the content, or undersize the thumb. Every
 * other syntax mode (and an *editable* Markdown document, which draws
 * every line as its own row same as any other syntax) just falls back to
 * the plain per-line count. */
static int editor_visible_line_count(ui_node* n)
{
    if (n->syntax != UI_SYNTAX_MARKDOWN || !n->read_only)
        return editor_line_count(n->label);

    const char* text = n->label;
    int text_len = (int)strlen(text);
    int off = 0, count = 0;
    while (off <= text_len)
    {
        int ls = off;
        while (off < text_len && text[off] != '\n')
            off++;
        if (!md_is_foldable_line(text + ls, off - ls))
            count++;
        if (off >= text_len)
            break;
        off++;
    }
    return count > 0 ? count : 1;  /* editor_line_count() never returns less than 1 either */
}

/* Keep n->scroll (the index of the topmost visible line) in range - called
 * both after edits that can shrink the document and directly by mouse-wheel
 * scrolling, which moves it without touching the cursor at all. */
static void editor_clamp_scroll(ui_node* n)
{
    int max_scroll = editor_visible_line_count(n) - n->h;
    if (max_scroll < 0)
        max_scroll = 0;
    if (n->scroll > max_scroll)
        n->scroll = max_scroll;
    if (n->scroll < 0)
        n->scroll = 0;
}

/* --- <editor> scrollbar: self-contained like <listbox>'s own (see
 * listbox_has_scrollbar) - no window-level opt-in, no dependency on
 * whatever window/dialog happens to contain it; the editor owns its
 * vertical AND horizontal scrollbars directly, drawn as overlays in its
 * own last column/row (see render_editor)
 * only while the mouse is over it or a thumb is mid-drag. The horizontal
 * half (editor_has_hscrollbar and friends) lives further down, next to
 * editor_content_cols, which it needs - see the forward declarations above
 * near editor_clamp_hscroll. --- */

static int editor_has_vscrollbar(ui_node* n)
{
    return n->w > 0 && n->h > 0 && editor_visible_line_count(n) > n->h;
}

/* Same shape as listbox_scrollbar_thumb, just over line count instead of
 * child count - the track is the editor's own full height, no corners to
 * carve out of it (unlike the old window-border version). */
static void editor_vscrollbar_thumb(ui_node* n, int* out_start, int* out_len)
{
    int total = editor_visible_line_count(n);
    int max_scroll = total - n->h;
    if (max_scroll <= 0 || n->h <= 0)
    {
        *out_start = 0;
        *out_len = n->h;
        return;
    }
    int len = n->h * n->h / total;
    if (len < 1) len = 1;
    if (len > n->h) len = n->h;
    int start = (n->scroll * (n->h - len)) / max_scroll;
    if (start < 0) start = 0;
    if (start > n->h - len) start = n->h - len;
    *out_start = start;
    *out_len = len;
}

/* Sets n->scroll from a click/drag at screen row `mouse_y` within the
 * editor's own column - the inverse of editor_vscrollbar_thumb(). */
static void editor_vscrollbar_set_from_mouse(ui_node* n, int mouse_y)
{
    if (n->h <= 1)
    {
        n->scroll = 0;
    }
    else
    {
        int max_scroll = editor_visible_line_count(n) - n->h;
        if (max_scroll < 0)
            max_scroll = 0;
        int row = mouse_y - n->y;
        if (row < 0) row = 0;
        if (row > n->h - 1) row = n->h - 1;
        n->scroll = (row * max_scroll + (n->h - 1) / 2) / (n->h - 1);
    }
    editor_clamp_scroll(n);
}

/* Scroll just enough to bring the caret's line back into view - the same
 * "keep the caret visible" behavior as any real editor, called after every
 * keyboard-driven cursor move/edit. Mouse-wheel scrolling deliberately
 * bypasses this (see ui_screen_update) so it can scroll the view away from
 * the caret, same as any other editor. */
static void editor_ensure_cursor_visible(ui_node* n)
{
    int line, line_start;
    editor_cursor_line(n, &line, &line_start);
    if (line < n->scroll)
        n->scroll = line;
    else if (line >= n->scroll + n->h)
        n->scroll = line - n->h + 1;
    editor_clamp_scroll(n);

    /* Horizontal counterpart: keep the caret's column inside [hscroll,
     * hscroll + w). The caret only ever sits within real text, so following
     * it can't push hscroll past the content - no upper clamp needed here
     * (the mouse-wheel pan handles that, via editor_clamp_hscroll). */
    int col = utf8_col_of(n->label + line_start, n->cursor - line_start);
    int text_w = editor_text_w(n);
    if (col < n->hscroll)
        n->hscroll = col;
    else if (col >= n->hscroll + text_w)
        n->hscroll = col - text_w + 1;
    if (n->hscroll < 0)
        n->hscroll = 0;
}

void ui_editor_goto_line(ui_node* n, int line)
{
    if (!n || n->type != UI_TAG_EDITOR)
        return;
    if (line < 1)
        line = 1;

    /* Byte offset of the target line's start - stop at the last line if the
     * requested one is past the end of the document. */
    int cur = 1, off = 0;
    for (int i = 0; cur < line && n->label[i]; i++)
    {
        if (n->label[i] == '\n')
        {
            cur++;
            off = i + 1;
        }
    }

    n->cursor = off;
    n->sel_anchor = -1;
    editor_ensure_cursor_visible(n);
}

int ui_editor_caret_line(const ui_node* n)
{
    if (!n || n->type != UI_TAG_EDITOR)
        return 0;
    int line, line_start;
    editor_cursor_line((ui_node*)n, &line, &line_start);
    return line + 1;  /* 1-based */
}

int ui_editor_line_at_point(const ui_node* n, int x, int y)
{
    if (!n || n->type != UI_TAG_EDITOR)
        return -1;
    if (x < n->x || x >= n->x + n->w || y < n->y || y >= n->y + n->h)
        return -1;
    return n->scroll + (y - n->y);
}

static int has_selection(const ui_node* n);
static void selection_range(const ui_node* n, int* lo, int* hi);

int ui_editor_get_cursor(const ui_node* n)
{
    if (!n || n->type != UI_TAG_EDITOR)
        return 0;
    return n->cursor;
}

int ui_editor_get_selection(const ui_node* n, int* lo, int* hi)
{
    if (!n || n->type != UI_TAG_EDITOR || !has_selection(n))
        return 0;
    selection_range(n, lo, hi);
    return 1;
}

void ui_editor_set_selection(ui_node* n, int start, int end)
{
    if (!n || (n->type != UI_TAG_EDITOR && n->type != UI_TAG_INPUT))
        return;
    int len = (int)strlen(n->label);
    if (start < 0) start = 0;
    if (start > len) start = len;
    if (end < 0) end = 0;
    if (end > len) end = len;
    n->sel_anchor = start == end ? -1 : start;  /* -1 = no selection */
    n->cursor = end;
    editor_ensure_cursor_visible(n);
}

int ui_editor_get_scroll(const ui_node* n)
{
    if (!n || n->type != UI_TAG_EDITOR)
        return 0;
    return n->scroll;
}

void ui_editor_set_scroll(ui_node* n, int scroll)
{
    if (!n || n->type != UI_TAG_EDITOR)
        return;
    n->scroll = scroll;
    editor_clamp_scroll(n);
}

/* --- <listbox>: n->selected is the current row (a child index, shared with
 * <select> - see ui_select_get/set_selected), n->scroll the topmost visible
 * row. Same "keep it in range"/"scroll to keep the current row visible"
 * shape as <editor>'s scroll above, just over child count instead of line
 * count. --- */

static void listbox_clamp_scroll(ui_node* n)
{
    int max_scroll = n->child_count - n->h;
    if (max_scroll < 0)
        max_scroll = 0;
    if (n->scroll > max_scroll)
        n->scroll = max_scroll;
    if (n->scroll < 0)
        n->scroll = 0;
}

static void listbox_ensure_visible(ui_node* n)
{
    if (n->selected < n->scroll)
        n->scroll = n->selected;
    else if (n->selected >= n->scroll + n->h)
        n->scroll = n->selected - n->h + 1;
    listbox_clamp_scroll(n);
}

/* --- <listbox> scrollbar: self-contained like <editor>'s own (see
 * editor_has_vscrollbar above) - a <listbox> owns its own vertical
 * scrollbar outright, no window-level opt-in, no dependency on whatever
 * window/dialog happens to contain it. That's what lets the same
 * Folder-panel listbox and
 * the Open/Save dialogs' listbox both scroll for free. It's drawn as a thin
 * overlay in the box's own rightmost column rather than a permanently
 * reserved one, and (see render_listbox/process_window) only painted while
 * the mouse is actually over the listbox or the thumb is being dragged - a
 * plain listbox with nothing to scroll never spends a column on it, and one
 * that does only spends it when it's actually useful to look at. --- */

/* Whether `n` has anything to scroll at all - the gate for both drawing the
 * overlay and reacting to clicks/drags on its column. */
static int listbox_has_scrollbar(ui_node* n)
{
    return n->w > 0 && n->h > 0 && n->child_count > n->h;
}

/* The thumb's extent within the box's full height (n->h is the track here -
 * there are no corners to carve out of it, unlike a <window>'s border-strip
 * scrollbar) - same shape as scrollbar_thumb, just over child count. */
static void listbox_scrollbar_thumb(ui_node* n, int* out_start, int* out_len)
{
    int max_scroll = n->child_count - n->h;
    if (max_scroll <= 0 || n->h <= 0)
    {
        *out_start = 0;
        *out_len = n->h;
        return;
    }
    int len = n->h * n->h / n->child_count;
    if (len < 1) len = 1;
    if (len > n->h) len = n->h;
    int start = (n->scroll * (n->h - len)) / max_scroll;
    if (start < 0) start = 0;
    if (start > n->h - len) start = n->h - len;
    *out_start = start;
    *out_len = len;
}

/* Sets n->scroll from a click/drag at screen row `mouse_y` within the box's
 * own column - the inverse of listbox_scrollbar_thumb(), same shape as
 * scrollbar_set_from_mouse. */
static void listbox_scrollbar_set_from_mouse(ui_node* n, int mouse_y)
{
    if (n->h <= 1)
    {
        n->scroll = 0;
    }
    else
    {
        int max_scroll = n->child_count - n->h;
        if (max_scroll < 0)
            max_scroll = 0;
        int row = mouse_y - n->y;
        if (row < 0) row = 0;
        if (row > n->h - 1) row = n->h - 1;
        n->scroll = (row * max_scroll + (n->h - 1) / 2) / (n->h - 1);
    }
    listbox_clamp_scroll(n);
}

/* --- Selection: shared by <input> and <editor> - a byte range [lo, hi) into
 * n->label, anchored at n->sel_anchor with the live end at n->cursor. A
 * anchor equal to the cursor (or -1) means no selection, same convention a
 * browser's empty Range represents "just a caret". --- */

static int has_selection(const ui_node* n)
{
    return n->sel_anchor >= 0 && n->sel_anchor != n->cursor;
}

static void selection_range(const ui_node* n, int* lo, int* hi)
{
    if (!has_selection(n))
    {
        *lo = *hi = n->cursor;
        return;
    }
    *lo = n->sel_anchor < n->cursor ? n->sel_anchor : n->cursor;
    *hi = n->sel_anchor < n->cursor ? n->cursor : n->sel_anchor;
}

/* Erases the selected range (if any - a no-op otherwise), leaving the cursor
 * at its start and clearing the selection, ready for whatever replaces it
 * (typing, Enter, paste, or nothing at all for a plain Cut/Delete). */
static void editor_delete_selection(ui_node* n)
{
    if (n->read_only || !has_selection(n))
        return;

    editor_undo_mark(n, UI_UNDO_DELSEL);

    int lo, hi;
    selection_range(n, &lo, &hi);
    char* buf = malloc(strlen(n->label) + 1);
    memcpy(buf, n->label, lo);
    strcpy(buf + lo, n->label + hi);
    ui_set_label(n, buf);
    free(buf);
    n->cursor = lo;
    n->sel_anchor = -1;
    editor_undo_settle(n);

    if (n->type == UI_TAG_EDITOR)
    {
        n->dirty = 1;
        ui_editor_clear_diagnostics(n);  /* see input_insert()'s own comment */
    }
}

/* Defined later, next to the VT100/Markdown line renderers that also need
 * them (ansi_parse_sgr for color, scan_markdown_fence_state for fenced-code-
 * block state) - forward declared so editor_selected_text() below can strip
 * VT100 escapes / markdown delimiters before they reach the clipboard. */
static int ansi_parse_sgr(const char* s, int len, uint32_t* fg, int* bold);
static int scan_markdown_fence_state(const char* line, int len, int in_block);

/* Strips embedded "\x1b[...m" SGR escape sequences from a VT100/Output-window
 * selection so Copy/Cut put only the plain visible text on the clipboard,
 * not the color codes used to render it. Non-SGR/malformed escapes (rare -
 * ansi_parse_sgr returns 0 for those) are left as literal bytes, same as the
 * renderer does when it can't make sense of them. */
static char* strip_vt100_sgr(const char* s, int len)
{
    char* out = malloc(len + 1);
    int o = 0, i = 0;
    while (i < len)
    {
        if (s[i] == '\x1b')
        {
            uint32_t fg;
            int bold;
            int used = ansi_parse_sgr(s + i, len - i, &fg, &bold);
            if (used > 0)
            {
                i += used;
                continue;
            }
        }
        out[o++] = s[i++];
    }
    out[o] = 0;
    return out;
}

/* Strips the same read-only "presentation" markdown delimiters that
 * render_editor_line_markdown() hides when drawing (fenced code block
 * fences, inline `code span` backticks, **bold** markers, <!-- html
 * comments --> and [link text](dest) brackets/parens+url) - kept in sync
 * with that function's hide_char rules so copied text matches what's shown
 * on screen. Headings/blockquotes only recolor their line there and aren't
 * stripped here either, for the same reason. Operates line-by-line over
 * s[0,len); `in_block` (in/out) is the fenced-code-block state carried in
 * from before s, same as render_editor_line_markdown's caller threads it
 * across the whole document. */
static char* strip_markdown_readonly(const char* s, int len, int in_block)
{
    char* out = malloc(len + 1);
    int o = 0;
    int off = 0;
    while (off <= len)
    {
        int ls = off;
        while (off < len && s[off] != '\n')
            off++;
        int line_len = off - ls;
        const char* line = s + ls;

        int j = 0;
        while (j < line_len && (line[j] == ' ' || line[j] == '\t'))
            j++;
        int is_fence = (line_len - j >= 3 && line[j] == '`' && line[j + 1] == '`' && line[j + 2] == '`');
        if (is_fence)
            in_block = !in_block;
        int in_code_block = in_block || is_fence;

        int in_span = 0, in_link_text = 0, in_link_dest = 0, in_html_comment = 0;
        int i = 0;
        while (i < line_len)
        {
            int hide_char = 0;

            if (in_html_comment)
            {
                hide_char = 1;
                if (i + 2 < line_len && line[i] == '-' && line[i + 1] == '-' && line[i + 2] == '>')
                {
                    i += 3;
                    in_html_comment = 0;
                    continue;
                }
            }
            else if (is_fence)
            {
                hide_char = 1;
            }
            else if (!in_code_block)
            {
                if (line[i] == '`')
                {
                    hide_char = 1;
                    in_span = !in_span;
                }
                else if (line[i] == '*' && i + 1 < line_len && line[i + 1] == '*')
                {
                    i += 2;
                    continue;
                }
                else if (line[i] == '<' && i + 3 < line_len && line[i + 1] == '!' &&
                        line[i + 2] == '-' && line[i + 3] == '-')
                {
                    in_html_comment = 1;
                    i += 4;
                    continue;
                }
                else if (line[i] == '[')
                {
                    const char* p = line + i + 1;
                    while (p < line + line_len && *p != ']')
                        p++;
                    if (p < line + line_len && *p == ']' && p + 1 < line + line_len && p[1] == '(')
                    {
                        in_link_text = 1;
                        hide_char = 1;
                    }
                }
                else if (line[i] == ']' && in_link_text)
                {
                    in_link_text = 0;
                    hide_char = 1;
                    if (i + 1 < line_len && line[i + 1] == '(')
                        in_link_dest = 1;
                }
                else if (in_link_dest)
                {
                    hide_char = 1;
                    if (line[i] == ')')
                        in_link_dest = 0;
                }
            }

            if (!hide_char)
                out[o++] = line[i];
            i++;
        }

        if (off < len)
        {
            /* A fence delimiter line ("```" or "```c") already contributed
             * zero bytes of content above (is_fence hides every character on
             * it) - also skip its own newline, so the line vanishes from the
             * copied text entirely instead of leaving a blank line behind
             * where it used to be. Without this, pasting a selection that
             * spans a fence would still show the gap it used to fill, even
             * though none of its actual text (the backticks/language tag)
             * survived the copy. */
            if (!is_fence)
                out[o++] = '\n';
            off++;
        }
        else
        {
            break;
        }
    }
    out[o] = 0;
    return out;
}

/* A malloc'd copy of the selected text, or NULL if there's no selection -
 * the caller frees it. Copies the raw substring unchanged for most nodes;
 * for a VT100 output window (always read-only) or a Markdown document that's
 * currently read-only, strips the escape codes / syntax delimiters that
 * wouldn't make sense pasted elsewhere - see strip_vt100_sgr() and
 * strip_markdown_readonly(). An editable Markdown document still copies its
 * raw source, unchanged, exactly as before. */
static char* editor_selected_text(const ui_node* n)
{
    int lo, hi;
    selection_range(n, &lo, &hi);
    if (lo == hi)
        return NULL;
    int len = hi - lo;

    if (n->syntax == UI_SYNTAX_VT100)
        return strip_vt100_sgr(n->label + lo, len);

    if (n->syntax == UI_SYNTAX_MARKDOWN && n->read_only)
    {
        /* Prime the fenced-code-block state from the start of the document
         * up to the selection, the same way render_editor's scan cache does
         * for the viewport - otherwise a selection starting inside a fenced
         * block would wrongly treat its first line as ordinary text. */
        int in_block = 0, off = 0;
        while (off < lo)
        {
            int ls = off;
            while (off < lo && n->label[off] != '\n')
                off++;
            in_block = scan_markdown_fence_state(n->label + ls, off - ls, in_block);
            if (off < lo)
                off++;
        }
        return strip_markdown_readonly(n->label + lo, len, in_block);
    }

    char* buf = malloc(len + 1);
    memcpy(buf, n->label + lo, len);
    buf[len] = 0;
    return buf;
}

static void editor_copy(ui_node* n)
{
    char* sel = editor_selected_text(n);
    if (!sel)
        return;
    ui_clipboard_set_text(sel);
    free(sel);
}

static void editor_cut(ui_node* n)
{
    char* sel = editor_selected_text(n);
    if (!sel)
        return;
    ui_clipboard_set_text(sel);
    free(sel);
    editor_delete_selection(n);
    if (n->type == UI_TAG_EDITOR)
        editor_ensure_cursor_visible(n);
}

static void editor_paste(ui_node* n)
{
    char* text = ui_clipboard_get_text();
    if (!text)
        return;
    editor_delete_selection(n);
    /* Insert byte-by-byte-decoded codepoints through the same path typing
     * uses, normalizing CRLF (the clipboard's usual line ending) to the
     * single '\n' this document already stores. A single-line <input>
     * pasting a multi-line clipboard just ends up with embedded '\n's, same
     * as a real text field would truncate/garble it - not worth guarding. */
    const char* p = text;
    while (*p)
    {
        uint32_t cp;
        int len = utf8_decode(p, &cp);
        if (cp != '\r')
            input_insert(n, cp, UI_UNDO_PASTE);
        p += len;
    }
    free(text);
    if (n->type == UI_TAG_EDITOR)
        editor_ensure_cursor_visible(n);
}

/* Undo the most recent edit group: push the current state onto the redo
 * stack (so ui_editor_redo can restore it), then pop and restore the most
 * recent undo snapshot. A no-op if there's nothing to undo, or on any tag
 * other than EDITOR. */
void ui_editor_undo(ui_node* n)
{
    if (!n || n->type != UI_TAG_EDITOR || n->undo_count == 0)
        return;

    undo_array_push(&n->redo_stack, &n->redo_count, &n->redo_cap,
                     xstrdup(n->label), n->cursor, n->sel_anchor);

    ui_undo_entry* e = &n->undo_stack[--n->undo_count];
    ui_set_label(n, e->text);
    n->cursor = e->cursor;
    n->sel_anchor = e->sel_anchor;
    free(e->text);

    n->undo_group = UI_UNDO_NONE;  /* the next edit starts its own group
                                     * rather than coalescing with whatever
                                     * came before the undo */
    editor_clamp_scroll(n);
    editor_ensure_cursor_visible(n);
    n->dirty = 1;
    ui_editor_clear_diagnostics(n);  /* see input_insert()'s own comment - the
                                      * restored text may not be what the
                                      * diagnostics were reported against
                                      * either */
}

/* Redo the most recently undone edit group - the mirror image of
 * ui_editor_undo above. A no-op if there's nothing to redo, or on any tag
 * other than EDITOR. */
void ui_editor_redo(ui_node* n)
{
    if (!n || n->type != UI_TAG_EDITOR || n->redo_count == 0)
        return;

    undo_array_push(&n->undo_stack, &n->undo_count, &n->undo_cap,
                     xstrdup(n->label), n->cursor, n->sel_anchor);

    ui_undo_entry* e = &n->redo_stack[--n->redo_count];
    ui_set_label(n, e->text);
    n->cursor = e->cursor;
    n->sel_anchor = e->sel_anchor;
    free(e->text);

    n->undo_group = UI_UNDO_NONE;
    editor_clamp_scroll(n);
    editor_ensure_cursor_visible(n);
    n->dirty = 1;
    ui_editor_clear_diagnostics(n);  /* see ui_editor_undo()'s own comment */
}

int ui_editor_can_undo(const ui_node* n)
{
    return n && n->type == UI_TAG_EDITOR && n->undo_count > 0;
}

int ui_editor_can_redo(const ui_node* n)
{
    return n && n->type == UI_TAG_EDITOR && n->redo_count > 0;
}

/* Public copy/cut/paste - for an Edit menu's Copy/Cut/Paste items to call
 * directly, same effect as the focused node's own Ctrl+C/X/V (see
 * ui_screen_update). A no-op with nothing focused. */
void ui_screen_copy(ui_screen* s)
{
    if (s->focused)
        editor_copy(s->focused);
}

void ui_screen_cut(ui_screen* s)
{
    if (s->focused)
        editor_cut(s->focused);
}

void ui_screen_paste(ui_screen* s)
{
    if (s->focused)
        editor_paste(s->focused);
}

/* Public "replace this byte range with this text, as one undo step" - for
 * Edit menu commands (Stringify, To Upper, To Lower, ...) that transform the
 * current selection and want the result to be a single Ctrl+Z away, instead
 * of the whole-buffer ui_set_value() swap those commands used to do (which
 * bypasses undo entirely - fine for Find/Replace's "Replace All", not fine
 * for a selection transform the user expects to be able to undo). Reuses
 * exactly the same building blocks editor_paste() does: select the range,
 * delete it (one UI_UNDO_DELSEL step), then insert new_text codepoint by
 * codepoint under UI_UNDO_PASTE (one coalesced step) - so the net result is
 * the same two-step undo group a real paste-over-a-selection would leave. */
void ui_editor_replace_selection(ui_node* n, int lo, int hi, const char* new_text)
{
    if (!n || n->type != UI_TAG_EDITOR || n->read_only || !new_text)
        return;

    ui_editor_set_selection(n, lo, hi);
    editor_delete_selection(n);

    const char* p = new_text;
    while (*p)
    {
        uint32_t cp;
        int len = utf8_decode(p, &cp);
        if (cp != '\r')
            input_insert(n, cp, UI_UNDO_PASTE);
        p += len;
    }

    ui_editor_set_selection(n, lo, n->cursor);
    editor_ensure_cursor_visible(n);
}

/* Tab/Shift+Tab block indent - see the UI_KEY_TAB handling in
 * ui_screen_update() below, which only calls this once a selection is
 * confirmed present. How many columns one indent level is worth. */
#define EDITOR_INDENT_WIDTH 4

/* A tiny growable byte buffer, same shape as the one this reflow-style
 * transform needs elsewhere (see wordwrap_text() in ide.c) - the output
 * length isn't a fixed function of the input length (removing a leading
 * tab shrinks a line by 1, adding four spaces grows it by 4), so a single
 * malloc(block_len + slack) guess doesn't work cleanly here either. */
typedef struct { char* buf; size_t len; size_t cap; } indent_buf;

static void ibuf_init(indent_buf* b)
{
    b->cap = 256;
    b->buf = malloc(b->cap);
    b->len = 0;
    if (b->buf)
        b->buf[0] = 0;
}

static void ibuf_append(indent_buf* b, const char* text, size_t n)
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

/* Tab (outdent=0) or Shift+Tab (outdent=1) with an active selection: shifts
 * every selected line right by EDITOR_INDENT_WIDTH spaces, or left by
 * removing up to that many - a single leading '\t' counts as one full
 * level and is removed whole rather than counted as one column, so mixed
 * tab/space-indented code still de-indents sensibly one level at a time.
 * A line the selection merely reaches by ending exactly at its own start
 * (e.g. a Shift+Down that stopped at column 0) is left untouched, same as
 * VS Code/Sublime's block-indent - nothing on that line was actually
 * highlighted. Routed through ui_editor_replace_selection() so the whole
 * shift is one Ctrl+Z away and the affected lines stay selected afterward
 * (letting repeated Tab presses keep indenting the same block). A no-op on
 * a read-only editor or with no selection. */
static void editor_indent_selection(ui_node* n, int outdent)
{
    if (!n || n->type != UI_TAG_EDITOR || n->read_only || !has_selection(n))
        return;

    int lo, hi;
    selection_range(n, &lo, &hi);
    const char* label = n->label;

    int block_start = lo;
    while (block_start > 0 && label[block_start - 1] != '\n')
        block_start--;

    int hi_line_start = hi;
    while (hi_line_start > 0 && label[hi_line_start - 1] != '\n')
        hi_line_start--;
    int skip_last_line = (hi > lo) && (hi == hi_line_start) && (hi_line_start > block_start);

    int block_end = hi;
    while (label[block_end] && label[block_end] != '\n')
        block_end++;

    indent_buf out;
    ibuf_init(&out);

    int pos = block_start;
    for (;;)
    {
        int line_start = pos;
        while (pos < block_end && label[pos] != '\n')
            pos++;
        int line_len = pos - line_start;
        const char* line = label + line_start;
        int is_last = (pos >= block_end);

        if (is_last && skip_last_line)
        {
            ibuf_append(&out, line, (size_t)line_len);
        }
        else if (!outdent)
        {
            ibuf_append(&out, "    ", EDITOR_INDENT_WIDTH);
            ibuf_append(&out, line, (size_t)line_len);
        }
        else
        {
            int cut;
            if (line_len > 0 && line[0] == '\t')
                cut = 1;
            else
            {
                cut = 0;
                while (cut < line_len && cut < EDITOR_INDENT_WIDTH && line[cut] == ' ')
                    cut++;
            }
            ibuf_append(&out, line + cut, (size_t)(line_len - cut));
        }

        if (is_last)
            break;
        ibuf_append(&out, "\n", 1);
        pos++;  /* skip the '\n' just scanned past */
    }

    if (out.buf)
        ui_editor_replace_selection(n, block_start, block_end, out.buf);
    free(out.buf);
}

/* Tab with no selection: ordinary typing, not a block operation - inserts
 * enough spaces to reach the next EDITOR_INDENT_WIDTH-column stop (a "soft
 * tab"), same as pressing space that many times, confined to wherever the
 * caret already is rather than touching any other line.
 *
 * A plain mouse click leaves sel_anchor == cursor (not -1) as its own way of
 * saying "no selection" (see the UI_TAG_EDITOR mouse handling above) -
 * input_insert() never touches sel_anchor, so once the caret moves away from
 * that anchor by typing, has_selection() flips true and whatever was just
 * typed reads back as selected. Explicitly collapsing the selection onto the
 * new caret position afterward (rather than only calling
 * editor_ensure_cursor_visible, which leaves sel_anchor untouched) keeps
 * this a plain "moved the caret while typing", the same as it would be after
 * any other keystroke that starts from a real caret (sel_anchor == -1). */
static void editor_insert_soft_tab(ui_node* n)
{
    if (!n || n->type != UI_TAG_EDITOR || n->read_only)
        return;

    int line, line_start;
    editor_cursor_line(n, &line, &line_start);
    int col = utf8_col_of(n->label + line_start, n->cursor - line_start);
    int spaces = EDITOR_INDENT_WIDTH - (col % EDITOR_INDENT_WIDTH);
    for (int k = 0; k < spaces; k++)
        input_insert(n, ' ', UI_UNDO_TYPE);
    ui_editor_set_selection(n, n->cursor, n->cursor);
    editor_ensure_cursor_visible(n);
}

/* Shift+Tab with no selection: outdents just the caret's own line (removing
 * one leading tab, or up to EDITOR_INDENT_WIDTH leading spaces - same rule
 * as editor_indent_selection's per-line outdent) and keeps the caret at the
 * same piece of text it was next to, shifted left by however much leading
 * whitespace actually got removed (clamped to the new line start if the
 * caret itself sat inside the removed whitespace). */
static void editor_outdent_current_line(ui_node* n)
{
    if (!n || n->type != UI_TAG_EDITOR || n->read_only)
        return;

    int line, line_start;
    editor_cursor_line(n, &line, &line_start);
    const char* label = n->label;
    int line_end = line_start;
    while (label[line_end] && label[line_end] != '\n')
        line_end++;
    int line_len = line_end - line_start;

    int cut;
    if (line_len > 0 && label[line_start] == '\t')
        cut = 1;
    else
    {
        cut = 0;
        while (cut < line_len && cut < EDITOR_INDENT_WIDTH && label[line_start + cut] == ' ')
            cut++;
    }
    if (cut == 0)
        return;

    int cursor_col = n->cursor - line_start;
    ui_editor_replace_selection(n, line_start, line_start + cut, "");
    int new_cursor = line_start + (cursor_col > cut ? cursor_col - cut : 0);
    ui_editor_set_selection(n, new_cursor, new_cursor);
    editor_ensure_cursor_visible(n);
}

/* Public undo/redo - for an Edit menu's Undo/Redo items, same effect as the
 * focused node's own Ctrl+Z/Ctrl+Y (see ui_screen_update). A no-op with
 * nothing focused or nothing to undo/redo. */
void ui_screen_undo(ui_screen* s)
{
    if (s->focused)
        ui_editor_undo(s->focused);
}

void ui_screen_redo(ui_screen* s)
{
    if (s->focused)
        ui_editor_redo(s->focused);
}

/* --- Tab order: cycle keyboard focus across the top-level focusable nodes,
 * in document order - like Tab/Shift+Tab moving through a form. --- */

static int is_focusable(int type)
{
    return type == UI_TAG_INPUT || type == UI_TAG_EDITOR ||
        type == UI_TAG_LISTBOX || type == UI_TAG_GROUP;
}

/* Whichever container (a blocking modal's window, or the frontmost floating
 * window if no modal is active) currently owns input, or NULL if it's the
 * plain document. */
static ui_node* active_container(ui_screen* s)
{
    if (s->modal_stack_count > 0)
        return s->modal_stack[s->modal_stack_count - 1];
    return s->window_count > 0 ? s->windows[s->window_count - 1] : NULL;
}

/* The <editor> at (x, y) among `window`'s children, or NULL - shared by
 * mouse-wheel scrolling, which must find whichever editor is under the
 * cursor regardless of focus (see ui_screen_update()). */
 /* The <editor> or <listbox> at (x, y) among `window`'s children (the two
  * scrollable widget types), or NULL - shared by mouse-wheel scrolling,
  * which must find whichever one is under the cursor regardless of focus
  * (see ui_screen_update()). */
static ui_node* find_editor_at(ui_node* window, int x, int y)
{
    for (int i = 0; i < window->child_count; i++)
    {
        ui_node* c = window->children[i];
        if ((c->type == UI_TAG_EDITOR || c->type == UI_TAG_LISTBOX) &&
            x >= c->x && x < c->x + c->w && y >= c->y && y < c->y + c->h)
            return c;
    }
    return NULL;
}

static void focus_next(ui_screen* s, int forward)
{
    /* While a modal/window is active, Tab only cycles within its window -
     * the rest of the document is blocked, same as its click-through gating
     * below. */
    ui_node* container = s->root;
    ui_node* active = active_container(s);
    if (active)
    {
        ui_node* window = find_child_by_type(active, UI_TAG_WINDOW);
        if (!window)
            return;
        container = window;
    }

    int n = container->child_count;
    if (n == 0)
        return;

    int start = -1;
    for (int i = 0; i < n; i++)
    {
        if (container->children[i] == s->focused)
        {
            start = i;
            break;
        }
    }
    if (start == -1)
        start = forward ? -1 : n;  /* one step lands on index 0 / n-1 */

    int step = forward ? 1 : -1;
    int i = start;
    for (int tries = 0; tries < n; tries++)
    {
        i = ((i + step) % n + n) % n;
        if (is_focusable(container->children[i]->type))
        {
            s->focused = container->children[i];
            return;
        }
    }
    /* No focusable node anywhere in the container - nothing to do. */
}

/* Move a node and its whole subtree by (dx, dy) - every node stores absolute
 * screen coordinates (there's no parent-relative offset in this model), so
 * dragging a <window> has to shift each descendant individually to keep its
 * contents attached rather than left behind. */
static void shift_subtree(ui_node* n, int dx, int dy)
{
    n->x += dx;
    n->y += dy;
    for (int i = 0; i < n->child_count; i++)
        shift_subtree(n->children[i], dx, dy);
}

/* A <window>'s close icon - "[ ]" with a filled square in the middle, at the
 * top-left of its border row, the classic Turbo Vision window-close chrome.
 * Framework-level, not app-level: any window inside a modal gets one for
 * free, closing the modal directly, the same way Escape already does.
 * Skipped on windows too narrow to fit it without colliding with the
 * top-right corner. Shared by hit-testing and rendering so they can't
 * disagree about where it is. */
#define UI_WINDOW_CLOSE_W 3
static int window_has_close_icon(ui_node* win) { return win->w >= 5; }

/* The maximize/restore icon - up arrow "[^]" normally (click to maximize),
 * down arrow "[v]" once maximized (click to restore) - mirrors the close
 * icon but sits just left of the top-right corner instead. Needs room for
 * both icons without overlap. */
#define UI_WINDOW_ZOOM_W 3
 /* A docked window (see ui_set_dock) never maximizes/restores - it already
  * has a fixed shape along its dock's axis - so it gets no zoom icon. */
static int window_has_zoom_icon(ui_node* win) { return win->resizable && win->w >= 10 && !win->dock; }
static int window_zoom_x(ui_node* win) { return win->x + win->w - 1 - UI_WINDOW_ZOOM_W; }

/* The caret "line:col" indicator shown at the bottom-left border of a source
 * <editor> window (skipped for a VT100/output one, or a window too narrow).
 * Fills `buf` and returns its column width, or 0 when nothing is shown -
 * used by render_window, which draws it. */
static int window_caret_indicator(ui_node* win, char* buf, size_t cap)
{
    ui_node* sed = find_child_by_type(win, UI_TAG_EDITOR);
    if (!(sed && sed->syntax != UI_SYNTAX_VT100 && win->w > 14))
    {
        if (cap) buf[0] = '\0';
        return 0;
    }
    int cl, cls;
    editor_cursor_line(sed, &cl, &cls);
    int cc = utf8_col_of(sed->label + cls, sed->cursor - cls);
    snprintf(buf, cap, " %d:%d ", cl + 1, cc + 1);
    return (int)strlen(buf);
}

/* Move and/or resize a <window>, keeping its contents sensibly attached:
 * every child follows the window's move in full (there's no parent-relative
 * coordinate system - see shift_subtree), and on top of that, an <editor> or
 * <listbox> child (the one widget meant to fill its window, like a real code
 * editor or a file browser's listing) stretches by the same amount the
 * window did, while other children whose original position was past the
 * window's midpoint on an axis (buttons/text anchored toward an edge) shift
 * by the extra room on that axis so they track their corner instead of the
 * window growing away from them. */
static void set_window_rect(ui_node* window, int new_x, int new_y, int new_w, int new_h)
{
    int old_x = window->x, old_y = window->y, old_w = window->w, old_h = window->h;
    int dx = new_x - old_x, dy = new_y - old_y;
    int dw = new_w - old_w, dh = new_h - old_h;

    window->x = new_x;
    window->y = new_y;
    window->w = new_w;
    window->h = new_h;

    for (int i = 0; i < window->child_count; i++)
    {
        ui_node* c = window->children[i];
        if (c->type == UI_TAG_EDITOR || c->type == UI_TAG_LISTBOX)
        {
            shift_subtree(c, dx, dy);
            c->w += dw;
            c->h += dh;
        }
        else
        {
            int anchored_right = (c->x - old_x) > old_w / 2;
            int anchored_bottom = (c->y - old_y) > old_h / 2;
            shift_subtree(c, dx + (anchored_right ? dw : 0), dy + (anchored_bottom ? dh : 0));
        }
    }
}

/* Public move/resize of a <window> node (and its whole subtree), for window-
 * manager actions like Tile/Cascade - the app has the wrapper's <window>
 * child but can't reach the internal set_window_rect. Clears any maximized
 * state, since an explicit placement replaces it. */
void ui_window_set_rect(ui_node* window, int x, int y, int w, int h)
{
    if (!window || window->type != UI_TAG_WINDOW)
        return;
    window->maximized = 0;
    set_window_rect(window, x, y, w, h);
}

/* Toggle between the window's normal geometry and filling the desktop (minus
 * a 1-cell margin), saving/restoring whichever one isn't currently active. */
 /* The maximized rect: full width edge-to-edge (no side margins), but never
  * over the menu row at the top or the status row at the bottom - a maximized
  * window draws on top of everything, so it would otherwise cover them rather
  * than just sit below/above them. */
  /* Subtracts every visible docked window (see ui_set_dock) from the screen's
   * usable area (menu row and status row already excluded) - Output
   * (UI_DOCK_BOTTOM) takes its height off the bottom; Folder (UI_DOCK_LEFT/
   * RIGHT) takes its width off the matching side. This is the single place
   * that answers "how much desktop is left" - used both for maximizing
   * ordinary windows and for laying the docked windows out against each
   * other (see dock_layout). Only windows actually in s->windows[] (i.e.
   * currently shown - see ui_screen_show_window/close_modal) count, so
   * hiding a dock immediately gives its space back. */
static void maximized_rect(ui_screen* s, int* out_x, int* out_y, int* out_w, int* out_h)
{
    int x = 0, y = 1, w = s->screen_w, h = s->screen_h - 2;

    for (int i = 0; i < s->window_count; i++)
    {
        ui_node* win = find_child_by_type(s->windows[i], UI_TAG_WINDOW);
        if (!win || win->dock == UI_DOCK_NONE)
            continue;
        if (win->dock == UI_DOCK_LEFT) { x += win->w; w -= win->w; }
        else if (win->dock == UI_DOCK_RIGHT) { w -= win->w; }
        else if (win->dock == UI_DOCK_BOTTOM) { h -= win->h; }
    }
    if (w < 1) w = 1;
    if (h < 1) h = 1;

    *out_x = x;
    *out_y = y;
    *out_w = w;
    *out_h = h;
}

void ui_screen_desktop_rect(ui_screen* s, int* out_x, int* out_y, int* out_w, int* out_h)
{
    int x, y, w, h;
    maximized_rect(s, &x, &y, &w, &h);
    if (out_x) *out_x = x;
    if (out_y) *out_y = y;
    if (out_w) *out_w = w;
    if (out_h) *out_h = h;
}

/* Lays out every currently-shown docked window (see ui_set_dock) against
 * the current screen size and each other: Folder (UI_DOCK_LEFT/RIGHT)
 * always spans the full top-to-bottom edge (down to the status row) on its
 * own chosen side, its own width whatever it was last set to (ui_set_dock's
 * initial size, or a later border drag - see process_window); Output
 * (UI_DOCK_BOTTOM) sits below everything else but only fills the width
 * that's left over once Folder has taken its slice off the left and/or
 * right, its own height likewise whatever it was last set to. This keeps
 * Folder running the whole left (or right) edge rather than stopping above
 * Output, with Output filling in to Folder's right (or left). Idempotent
 * and cheap - called once every ui_screen_update() (so screen resizes and
 * docks being shown/hidden reflow everything) and again immediately after
 * a dock border drag changes a thickness (so its neighbor and every
 * maximized window pick that up the same frame - see process_window). */
static void dock_layout(ui_screen* s)
{
    ui_node* left = NULL, * right = NULL, * bottom = NULL;

    for (int i = 0; i < s->window_count; i++)
    {
        ui_node* win = find_child_by_type(s->windows[i], UI_TAG_WINDOW);
        if (!win)
            continue;
        if (win->dock == UI_DOCK_LEFT) left = win;
        else if (win->dock == UI_DOCK_RIGHT) right = win;
        else if (win->dock == UI_DOCK_BOTTOM) bottom = win;
    }

    int top = 1, screen_bottom = s->screen_h - 1, full_w = s->screen_w;
    int left_w = 0, right_w = 0;

    if (left)
    {
        int w = left->w;
        if (w > full_w) w = full_w;
        if (w < 1) w = 1;
        int h = screen_bottom - top;
        if (h < 1) h = 1;
        if (left->x != 0 || left->y != top || left->w != w || left->h != h)
            set_window_rect(left, 0, top, w, h);
        left_w = w;
    }

    if (right)
    {
        int w = right->w;
        if (w > full_w) w = full_w;
        if (w < 1) w = 1;
        int h = screen_bottom - top;
        if (h < 1) h = 1;
        int x = full_w - w;
        if (right->x != x || right->y != top || right->w != w || right->h != h)
            set_window_rect(right, x, top, w, h);
        right_w = w;
    }

    if (bottom)
    {
        int bw = full_w - left_w - right_w;
        if (bw < 1) bw = 1;
        int h = bottom->h;
        if (h > screen_bottom - top) h = screen_bottom - top;
        if (h < 1) h = 1;
        int y = screen_bottom - h;
        if (bottom->x != left_w || bottom->y != y || bottom->w != bw || bottom->h != h)
            set_window_rect(bottom, left_w, y, bw, h);
    }
}

/* Resyncs every currently-maximized window against maximized_rect() right
 * now, in one pass over s->windows[] - not just whichever single window
 * happens to be this frame's mouse-active one. process_window() only ever
 * processes one window per frame (see its caller), so the maximized-rect
 * check that used to live solely inside it left every OTHER maximized
 * window visibly lagging a frame (or more, until the mouse happened to
 * land on it) behind a dock border drag or an OS window resize. Called
 * once up front in ui_screen_update() (covers screen resizes and dock
 * shows/hides) and again immediately after any dock_layout() triggered
 * mid-frame by a border drag (see process_window's docked-window resize
 * branch), so a maximized document window shrinks/grows in the same frame
 * as the Folder/Output panel being dragged, instead of catching up late. */
static void resync_maximized_windows(ui_screen* s)
{
    for (int i = 0; i < s->window_count; i++)
    {
        ui_node* window = find_child_by_type(s->windows[i], UI_TAG_WINDOW);
        if (!window || !window->maximized)
            continue;
        int mx, my, mw, mh;
        maximized_rect(s, &mx, &my, &mw, &mh);
        if (window->x != mx || window->y != my || window->w != mw || window->h != mh)
            set_window_rect(window, mx, my, mw, mh);
    }
}

static void toggle_window_zoom(ui_screen* s, ui_node* window)
{
    if (window->maximized)
    {
        set_window_rect(window, window->saved_x, window->saved_y,
                         window->saved_w, window->saved_h);
        window->maximized = 0;
    }
    else
    {
        window->saved_x = window->x;
        window->saved_y = window->y;
        window->saved_w = window->w;
        window->saved_h = window->h;
        int mx, my, mw, mh;
        maximized_rect(s, &mx, &my, &mw, &mh);
        set_window_rect(window, mx, my, mw, mh);
        window->maximized = 1;
    }
}

void ui_window_maximize(ui_screen* s, ui_node* window)
{
    if (!window || window->type != UI_TAG_WINDOW || window->dock)
        return;
    if (!window->maximized)
    {
        window->saved_x = window->x;
        window->saved_y = window->y;
        window->saved_w = window->w;
        window->saved_h = window->h;
    }
    int mx, my, mw, mh;
    maximized_rect(s, &mx, &my, &mw, &mh);
    set_window_rect(window, mx, my, mw, mh);
    window->maximized = 1;
}

/* Reflow top-level windows after an OS window (screen) resize from old_w x
 * old_h to the current s->screen_w x s->screen_h. A window edge that sat
 * exactly at the old right limit (screen_w) or bottom limit (screen_h - 1, the
 * row just above the status bar) is kept pinned to the matching new limit by
 * stretching that side; the top and left are never moved. Windows not touching
 * a limit (centered dialogs, etc.) are left as-is. A maximized window sits at
 * all limits, so this re-fills it to the new screen for free. */
static void reflow_on_screen_resize(ui_screen* s, int old_w, int old_h)
{
    int old_bottom = old_h - 1;          /* usable bottom, status bar discounted */
    int new_bottom = s->screen_h - 1;
    for (int i = 0; i < s->window_count; i++)
    {
        ui_node* win = find_child_by_type(s->windows[i], UI_TAG_WINDOW);
        if (!win)
            continue;
        int nw = win->w, nh = win->h;
        if (win->x + win->w == old_w)        /* right edge was at the limit */
            nw = s->screen_w - win->x;
        if (win->y + win->h == old_bottom)   /* bottom edge was at the limit */
            nh = new_bottom - win->y;
        if (nw < 1) nw = 1;
        if (nh < 1) nh = 1;
        if (nw != win->w || nh != win->h)
            set_window_rect(win, win->x, win->y, nw, nh);
    }
}

/* Grow `window` outward on all four sides until each edge reaches the screen
 * boundary (minus the menu row at top and status row at bottom, same usable
 * area as maximized_rect) or the nearest opposing edge of another floating
 * window that overlaps it on the perpendicular axis - i.e. the largest rect
 * that still clears every other window. Each border is moved independently
 * against the window's original extent, exactly as the feature is described:
 * left goes left to 0 or another window's right edge, right to the screen
 * width or another window's left edge, and likewise top/bottom. */
static void fit_window(ui_screen* s, ui_node* window)
{
    int x0 = window->x, y0 = window->y;
    int x1 = x0 + window->w, y1 = y0 + window->h;

    int left = 0, right = s->screen_w;
    int top = 1, bottom = s->screen_h - 1;

    for (int i = 0; i < s->window_count; i++)
    {
        ui_node* o = find_child_by_type(s->windows[i], UI_TAG_WINDOW);
        if (!o || o == window)
            continue;
        int ox0 = o->x, oy0 = o->y;
        int ox1 = ox0 + o->w, oy1 = oy0 + o->h;

        /* Only windows sharing rows with the target can block its left/right
         * growth; only those sharing columns can block its top/bottom. */
        if (oy0 < y1 && oy1 > y0)
        {
            if (ox1 <= x0 && ox1 > left)   left = ox1;    /* sits to the left */
            if (ox0 >= x1 && ox0 < right)  right = ox0;   /* sits to the right */
        }
        if (ox0 < x1 && ox1 > x0)
        {
            if (oy1 <= y0 && oy1 > top)    top = oy1;      /* sits above */
            if (oy0 >= y1 && oy0 < bottom) bottom = oy0;   /* sits below */
        }
    }

    set_window_rect(window, left, top, right - left, bottom - top);
}

/* Runs one <window>'s full per-frame interaction: maximized-size tracking,
 * close/zoom icon clicks, corner-drag resize, title-row drag, and children
 * hit-testing. Shared by the blocking-modal path (always the one active
 * modal) and the floating-window path (only the frontmost window under the
 * mouse, or whichever one is mid-drag/resize - see ui_screen_update()).
 * *click_consumed carries the caller's local of the same name through.
 * Returns 1 if this window traps the rest of the frame's mouse handling (a
 * blocking modal always does; a floating one only while the mouse is
 * actually over it) - the caller should stop immediately, same as the old
 * inline `return;` this was extracted from. Returns 0 if the click should
 * fall through to whatever's behind/underneath it (another window, the
 * menubar/statusbar, or the plain document). */
static int process_window(ui_screen* s, ui_node* container, ui_node* window,
                           int over_menu, int container_blocking, int* click_consumed)
{
    if (!window)
    {
        if (container_blocking)
        {
            if (s->mouse_pressed && !over_menu)
                s->open_menu = NULL;
            return 1;
        }
        return 0;
    }

    /* A maximized window tracks the screen size every frame, not just the
     * size at the moment it was maximized - otherwise resizing the OS
     * window (or changing font size, which changes the grid's column/row
     * count) leaves it stuck at its old, now-wrong dimensions instead of
     * following along like every other maximized window does. Handled for
     * every maximized window up front now (see resync_maximized_windows,
     * called from ui_screen_update and from the docked-resize branch
     * below), not just this one - no per-window check needed here. */

     /* Every check below is gated on !over_menu: if the mouse is currently
      * over the menu bar/dropdown (checked before this is ever called, since
      * they render on top of everything including this window), the window
      * must not react to it at all - not just clicks, but hover too (s->hot),
      * since a window occupying the same screen position would otherwise
      * steal hover highlighting on every mouse-move frame regardless of
      * whether anything was pressed. */

      /* The close/zoom icons take priority over dragging/resizing: clicking
       * either acts immediately, without ever starting a drag even though
       * they sit on the same border row a drag would otherwise start on. */
    if (window_has_close_icon(window) && s->mouse_pressed && !over_menu &&
        s->mouse_y == window->y &&
        s->mouse_x >= window->x + 1 && s->mouse_x < window->x + 1 + UI_WINDOW_CLOSE_W)
    {
        s->open_menu = NULL;  /* this click is "elsewhere" for the menu */

        /* A transient window (a document/editor window) with unsaved edits
         * doesn't just close - see UI_CLOSE_REQUEST_ID's own doc comment.
         * Every other window (non-transient, or no dirty EDITOR inside)
         * closes immediately, exactly as before. */
        ui_node* ed = container->transient ? find_child_by_type(window, UI_TAG_EDITOR) : NULL;
        if (ed && ed->dirty)
            ui_fire_event(s, UI_CLOSE_REQUEST_ID, container);
        else
            ui_screen_close_modal(s, container);
        return 1;
    }
    if (window_has_zoom_icon(window) && s->mouse_pressed && !over_menu &&
        s->mouse_y == window->y &&
        s->mouse_x >= window_zoom_x(window) &&
        s->mouse_x < window_zoom_x(window) + UI_WINDOW_ZOOM_W)
    {
        s->open_menu = NULL;
        toggle_window_zoom(s, window);
        return 1;
    }

    if (window->dock)
    {
        /* A docked window (see ui_set_dock) isn't dragged or corner-resized
         * like an ordinary floating one - it's pinned in place by
         * dock_layout(), and the only gesture it responds to is dragging
         * its one free border to change its thickness: the right edge for
         * UI_DOCK_LEFT, the left edge for UI_DOCK_RIGHT, or the top edge
         * (its title row) for UI_DOCK_BOTTOM. */
        if (s->resizing_window == window && !s->mouse_down)
            s->resizing_window = NULL;

        int on_handle = 0, new_size = 0;
        if (window->dock == UI_DOCK_LEFT)
        {
            on_handle = s->mouse_x == window->x + window->w - 1 &&
                s->mouse_y > window->y && s->mouse_y < window->y + window->h - 1;
            new_size = s->mouse_x - window->x + 1;
        }
        else if (window->dock == UI_DOCK_RIGHT)
        {
            on_handle = s->mouse_x == window->x &&
                s->mouse_y > window->y && s->mouse_y < window->y + window->h - 1;
            new_size = window->x + window->w - s->mouse_x;
        }
        else if (window->dock == UI_DOCK_BOTTOM)
        {
            on_handle = s->mouse_y == window->y &&
                s->mouse_x >= window->x && s->mouse_x < window->x + window->w;
            new_size = window->y + window->h - s->mouse_y;
        }

        if (on_handle && s->mouse_pressed && !over_menu && window->resizable)
            s->resizing_window = window;

        if (s->resizing_window == window && s->mouse_down)
        {
            int cap = (window->dock == UI_DOCK_BOTTOM) ? s->screen_h - 6 : s->screen_w - 8;
            if (new_size < 8) new_size = 8;
            if (new_size > cap && cap >= 8) new_size = cap;
            /* Go through set_window_rect (not a direct window->w/h write) so
             * the docked window's editor/listbox child stretches along with
             * it - a direct write here would also make dock_layout() below
             * see w/h already matching its target and skip its own
             * set_window_rect call, leaving the content stuck at its old
             * size even though the frame resized. */
            if (window->dock == UI_DOCK_BOTTOM)
            {
                if (new_size != window->h)
                    set_window_rect(window, window->x, window->y, window->w, new_size);
            }
            else
            {
                if (new_size != window->w)
                    set_window_rect(window, window->x, window->y, new_size, window->h);
            }
            /* Reflow immediately (not just next frame) so its neighbor and
             * every maximized window rearrange in step with the drag. */
            dock_layout(s);
            resync_maximized_windows(s);
        }
    }
    else
    {
        /* Resize by dragging the bottom-right corner cell - only if the window
         * opted in (see ui_set_resizable), and not while maximized (restore
         * first, like every other window manager). */
        if (s->resizing_window == window && !s->mouse_down)
            s->resizing_window = NULL;

        int on_resize_corner = s->mouse_x == window->x + window->w - 1 &&
            s->mouse_y == window->y + window->h - 1;
        if (on_resize_corner && s->mouse_pressed && !over_menu &&
            window->resizable && !window->maximized)
            s->resizing_window = window;

        if (s->resizing_window == window && s->mouse_down)
        {
            int new_w = s->mouse_x - window->x + 1;
            int new_h = s->mouse_y - window->y + 1;
            if (new_w < 12) new_w = 12;
            if (new_h < 5) new_h = 5;
            if (new_w != window->w || new_h != window->h)
                set_window_rect(window, window->x, window->y, new_w, new_h);
        }

        /* Drag by the border/title row: press there starts it, holding moves the
         * window (and its whole subtree) with the mouse, releasing (or the
         * window closing from under it) stops it. Not while maximized, nor
         * while a resize just grabbed this frame's press (the corner cell sits
         * on this same row for wide windows - actually it's on the LAST row,
         * but stay defensive either way). */
        if (s->dragging_window == window && !s->mouse_down)
            s->dragging_window = NULL;

        int on_border = s->mouse_y == window->y &&
            s->mouse_x >= window->x && s->mouse_x < window->x + window->w;
        if (on_border && s->mouse_dblclick && !over_menu && window->resizable)
        {
            /* Double-clicking the title/border row: when maximized, restore (like
             * any window manager); otherwise run the Fit algorithm, growing the
             * window to the largest rect that clears the screen edges and every
             * other window. Takes priority over starting a drag on that press. */
            s->open_menu = NULL;
            if (window->maximized)
                toggle_window_zoom(s, window);
            else
                fit_window(s, window);
            return 1;
        }
        if (on_border && s->mouse_pressed && !over_menu &&
            !window->maximized && !s->resizing_window)
        {
            s->dragging_window = window;
            s->drag_offset_x = s->mouse_x - window->x;
            s->drag_offset_y = s->mouse_y - window->y;
        }

        if (s->dragging_window == window && s->mouse_down)
        {
            int new_x = s->mouse_x - s->drag_offset_x;
            int new_y = s->mouse_y - s->drag_offset_y;
            if (new_x != window->x || new_y != window->y)
                set_window_rect(window, new_x, new_y, window->w, window->h);
        }
    }

    for (int i = 0; i < window->child_count; i++)
    {
        ui_node* c = window->children[i];
        if (c->type == UI_TAG_BUTTON)
        {
            int inside = !over_menu &&
                s->mouse_x >= c->x && s->mouse_x < c->x + c->w &&
                s->mouse_y >= c->y && s->mouse_y < c->y + c->h;
            /* A ui_message_box button fires its id (already delivered inside
             * hit_test_widget) and then the whole box closes+frees itself. */
            if (hit_test_widget(s, c, inside, NULL, NULL) && container->msgbox)
                s->msgbox_to_free = container;
        }
        else if (c->type == UI_TAG_INPUT)
        {
            int inside = !over_menu &&
                s->mouse_x >= c->x && s->mouse_x < c->x + c->w &&
                s->mouse_y == c->y;
            if (inside)
                s->hot = c;
            if (inside && s->mouse_pressed && !*click_consumed)
            {
                /* Same press-starts-a-fresh-selection, drag-extends-it
                 * convention as <editor> below. s->selecting (not
                 * s->focused) is what a drag continuation checks - a
                 * still-focused-from-earlier field must not be mistaken for
                 * one whose drag is still in progress, e.g. while dragging
                 * this same window by its title bar. */
                s->focused = c;
                input_click_set_cursor(c, s->mouse_x - c->x);
                c->sel_anchor = c->cursor;
                s->selecting = c;
            }
            else if (s->selecting == c && s->mouse_down && s->mouse_moved && !over_menu)
            {
                input_click_set_cursor(c, s->mouse_x - c->x);
            }
            /* Double-click selects the word under the cursor, same gesture
             * (and same editor_select_word() - it only touches label/cursor/
             * sel_anchor, fields INPUT and EDITOR both have) as <editor>
             * below. A separate if, not else-if, since the mouse_pressed
             * branch above already ran for this same press (a dblclick event
             * sets mouse_pressed too - see ui_screen_update) and positioned
             * the caret/started a fresh single-point selection there; this
             * then widens it to the whole word and cancels the drag-select
             * state so a lingering mouse move doesn't collapse it back down.
             * Unlike <editor>, this deliberately does NOT fire the input's
             * id - several inputs in this app (Replace's "Text to Find",
             * Go to Line, ...) fire their OK action on their id, which a
             * plain double-click-to-select gesture must not trigger. */
            if (inside && s->mouse_dblclick)
            {
                input_click_set_cursor(c, s->mouse_x - c->x);
                editor_select_word(c);
                s->selecting = NULL;
            }
        }
        else if (c->type == UI_TAG_EDITOR)
        {
            int inside = !over_menu &&
                s->mouse_x >= c->x && s->mouse_x < c->x + c->w &&
                s->mouse_y >= c->y && s->mouse_y < c->y + c->h;
            if (inside)
                s->hot = c;

            /* The scrollbar overlays (see editor_has_vscrollbar/editor_has_
             * hscrollbar/render_editor) live in the editor's own rightmost
             * column and bottom row - same click-empty-track-jumps/click-
             * thumb-just-grabs-it/drag-to-move shape as <listbox>'s own
             * scrollbar (see the UI_TAG_LISTBOX branch below). Checked
             * before the plain click-to-position-caret handling so a click
             * there drives the scrollbar instead of moving the caret. */
            int has_hbar = editor_has_hscrollbar(c);
            int on_bottom_row = s->mouse_y == c->y + c->h - 1;
            /* The vertical bar backs off the bottom row whenever the
             * horizontal one exists - that corner cell is the horizontal
             * bar's to claim, matching render_editor's own carve-out so a
             * click there always drives whichever bar is actually drawn on
             * top of it. */
            int on_vbar = inside && editor_has_vscrollbar(c) &&
                s->mouse_x == c->x + c->w - 1 &&
                !(has_hbar && on_bottom_row);
            int on_hbar = 0;
            if (inside && has_hbar && on_bottom_row)
            {
                int track_x0, track_w;
                editor_hscrollbar_layout(c, &track_x0, &track_w);
                on_hbar = s->mouse_x >= track_x0 && s->mouse_x < track_x0 + track_w;
            }

            if (s->dragging_editor_vscrollbar == c && !s->mouse_down)
                s->dragging_editor_vscrollbar = NULL;
            if (s->dragging_editor_hscrollbar == c && !s->mouse_down)
                s->dragging_editor_hscrollbar = NULL;

            if (on_vbar && s->mouse_pressed && !*click_consumed)
            {
                s->dragging_editor_vscrollbar = c;
                int thumb_start, thumb_len;
                editor_vscrollbar_thumb(c, &thumb_start, &thumb_len);
                int row = s->mouse_y - c->y;
                int on_thumb = row >= thumb_start && row < thumb_start + thumb_len;
                if (!on_thumb)
                    editor_vscrollbar_set_from_mouse(c, s->mouse_y);
                *click_consumed = 1;
            }
            else if (s->dragging_editor_vscrollbar == c && s->mouse_down && s->mouse_moved)
            {
                editor_vscrollbar_set_from_mouse(c, s->mouse_y);
            }
            else if (on_hbar && s->mouse_pressed && !*click_consumed)
            {
                s->dragging_editor_hscrollbar = c;
                int thumb_start, thumb_len;
                editor_hscrollbar_thumb(c, &thumb_start, &thumb_len);
                int col = s->mouse_x - c->x;
                int on_thumb = col >= thumb_start && col < thumb_start + thumb_len;
                if (!on_thumb)
                    editor_hscrollbar_set_from_mouse(c, s->mouse_x);
                *click_consumed = 1;
            }
            else if (s->dragging_editor_hscrollbar == c && s->mouse_down && s->mouse_moved)
            {
                editor_hscrollbar_set_from_mouse(c, s->mouse_x);
            }
            else if (inside && s->mouse_pressed && !*click_consumed)
            {
                /* Press starts a fresh selection anchored right here - a
                 * plain click (no drag before release) leaves sel_anchor ==
                 * cursor, which has_selection() already treats as "no
                 * selection", so no separate click-vs-drag bookkeeping is
                 * needed. */
                s->focused = c;
                editor_click_set_cursor(c, s->mouse_y - c->y, s->mouse_x - c->x);
                c->sel_anchor = c->cursor;
                s->selecting = c;
            }
            else if (s->selecting == c && s->mouse_down && s->mouse_moved && !over_menu)
            {
                /* Dragging with the button still held extends the selection
                 * from wherever the press started. */
                editor_click_set_cursor(c, s->mouse_y - c->y, s->mouse_x - c->x);
            }
            /* Double-click always selects the word under the cursor. For a
             * VT100/Output editor specifically, it also fires the editor's
             * id - so a handler can read the clicked line via
             * ui_editor_caret_line (e.g. jump to a compiler-output
             * "file:line:col" reference, see turboc_demo.c's
             * output_goto_source). Every other editor just selects the
             * word: firing the id there too used to also trigger whatever
             * Ctrl+click does on that same double-clicked word (e.g. a
             * Markdown link, or a ".c"/".h"/".md" reference - see
             * do_editor_ctrlclick) - a plain double-click-to-select gesture
             * must not trigger that, same reasoning as <input>'s own
             * double-click guard above. Excludes the scrollbar columns/rows
             * (on_vbar/on_hbar) so a double-click there doesn't also select
             * whatever word happens to sit underneath the overlay. */
            if (inside && s->mouse_dblclick && !on_vbar && !on_hbar)
            {
                editor_click_set_cursor(c, s->mouse_y - c->y, s->mouse_x - c->x);
                editor_select_word(c);
                s->selecting = NULL;  /* a double-click isn't a drag */
                if (c->id && c->syntax == UI_SYNTAX_VT100)
                    ui_fire_event(s, c->id, NULL);
            }
            /* Ctrl+click also fires the editor's id (distinct from a plain
             * or double click) - the caret's already been positioned by the
             * inside-and-pressed branch above, so a handler can act on
             * wherever was clicked (e.g. a Markdown "open link" action - see
             * turboc_demo.c's do_editor_ctrlclick). The framework itself has
             * no notion of links; this just gives the app a hook. On a
             * read-only editor, Ctrl isn't required - there's no text
             * editing for a plain click to disambiguate from, so a plain
             * click fires it too. Guarded against also being a double-click
             * so the two don't both fire for the same press, and against the
             * scrollbar columns/rows for the same reason as the double-click
             * guard just above. */
            if (inside && s->mouse_pressed && !s->mouse_dblclick && !on_vbar && !on_hbar &&
                ((s->mouse_mods & UI_MOD_CTRL) || c->read_only))
            {
                if (c->id)
                    ui_fire_event(s, c->id, NULL);
            }
        }
        else if (c->type == UI_TAG_SELECT)
        {
            int inside = c->enabled && !over_menu &&
                s->mouse_x >= c->x && s->mouse_x < c->x + c->w &&
                s->mouse_y == c->y;
            if (inside)
                s->hot = c;
            if (inside && s->mouse_pressed && !*click_consumed)
            {
                s->open_select = (s->open_select == c) ? NULL : c;
                *click_consumed = 1;
            }
        }
        else if (c->type == UI_TAG_LISTBOX)
        {
            int inside = !over_menu &&
                s->mouse_x >= c->x && s->mouse_x < c->x + c->w &&
                s->mouse_y >= c->y && s->mouse_y < c->y + c->h;
            if (inside)
                s->hot = c;

            /* The scrollbar overlay (see listbox_has_scrollbar/render_
             * listbox) lives in the box's own rightmost column - same
             * click-empty-track-jumps/click-thumb-just-grabs-it/drag-to-move
             * shape as <editor>'s own scrollbar (see the UI_TAG_EDITOR
             * branch above), just vertical-only and without the horizontal
             * counterpart an editor's lines can need. */
            int has_bar = listbox_has_scrollbar(c);
            int on_bar = has_bar && s->mouse_x == c->x + c->w - 1;

            if (s->dragging_listbox_scrollbar == c && !s->mouse_down)
                s->dragging_listbox_scrollbar = NULL;

            if (inside && on_bar && s->mouse_pressed && !*click_consumed)
            {
                s->dragging_listbox_scrollbar = c;
                int thumb_start, thumb_len;
                listbox_scrollbar_thumb(c, &thumb_start, &thumb_len);
                int row = s->mouse_y - c->y;
                int on_thumb = row >= thumb_start && row < thumb_start + thumb_len;
                if (!on_thumb)
                    listbox_scrollbar_set_from_mouse(c, s->mouse_y);
                *click_consumed = 1;
            }
            else if (s->dragging_listbox_scrollbar == c && s->mouse_down && s->mouse_moved)
            {
                listbox_scrollbar_set_from_mouse(c, s->mouse_y);
            }
            else if (inside && s->mouse_pressed && !*click_consumed)
            {
                int index = c->scroll + (s->mouse_y - c->y);
                if (index >= 0 && index < c->child_count)
                {
                    s->focused = c;
                    c->selected = index;
                    *click_consumed = 1;
                    if (s->mouse_dblclick)
                        ui_fire_event(s, c->id, NULL);
                }
            }
        }
        else if (c->type == UI_TAG_GROUP)
        {
            int inside = !over_menu &&
                s->mouse_x >= c->x && s->mouse_x < c->x + c->w &&
                s->mouse_y >= c->y && s->mouse_y < c->y + c->h;
            if (inside)
                s->hot = c;
            if (inside && s->mouse_pressed && !*click_consumed)
            {
                int row = s->mouse_y - c->y;  /* one item per row, no scroll */
                if (row >= 0 && row < c->child_count)
                {
                    s->focused = c;
                    c->cursor_row = row;
                    if (c->multi)
                        c->children[row]->selected = !c->children[row]->selected;
                    else
                        c->selected = row;
                    *click_consumed = 1;
                    ui_fire_event(s, c->id, NULL);
                }
            }
        }
    }

    /* A click that reaches this point wasn't on the open <select>'s own box
     * (that's *click_consumed above) or its popup (over_menu, set by the
     * top-level check in ui_screen_update) - so it must be some other click
     * inside this same window (a different widget, empty space, ...) and the
     * popup should close, the same "click elsewhere closes it" convention
     * every combo box uses. */
    if (s->open_select && s->mouse_pressed && !*click_consumed && !over_menu)
        s->open_select = NULL;

    /* A blocking modal (dialog) always traps the rest of the frame. A
     * non-blocking one (floating window) only does while the mouse is
     * actually over it this frame - otherwise fall through so whatever's
     * behind it (another window, the menubar/statusbar/other top-level
     * nodes) still gets its own hit-testing, like clicking past a floating
     * window to the document underneath. */
    int over_window = s->mouse_x >= window->x && s->mouse_x < window->x + window->w &&
        s->mouse_y >= window->y && s->mouse_y < window->y + window->h;
    if (container_blocking || over_window)
    {
        /* This frame's click (if any) is being swallowed here rather than
         * ever reaching the open-dropdown-closes-on-outside-click check
         * further down - so it must close one itself, the same as any other
         * click that isn't on the menu. Unless it WAS on the menu
         * (over_menu) - the top block already handled that correctly, this
         * must not second-guess it. */
        if (s->mouse_pressed && !over_menu)
            s->open_menu = NULL;
        return 1;
    }
    return 0;
}

/* True if this key event is the primary-modifier shortcut for base letter
 * `ch` (e.g. 'C' for Copy), under whichever convention is active (see
 * primary_mod()):
 *  - Ctrl+<letter> arrives as a raw control character (codepoint 1-26) with
 *    code==0 - Windows' own TranslateMessage() does this translation in
 *    WM_CHAR, and a literal Ctrl+<letter> does the same in Cocoa's
 *    characters string. Nothing pre-translates Cmd+<letter> this way, so:
 *  - Cmd+<letter> (macOS) arrives as the plain letter codepoint with
 *    UI_MOD_CMD set in mods.
 * Either way mods still carries the Shift bit correctly (even alongside a
 * control-char codepoint - e.g. Ctrl+S and Ctrl+Shift+S both carry codepoint
 * 19, only Shift tells them apart), so `want_shift` disambiguates callers
 * that bind different actions to the shifted and unshifted form. */
static int primary_letter_matches(char ch, int mods, uint32_t codepoint, int want_shift)
{
    int has_shift = (mods & UI_MOD_SHIFT) != 0;
    if (has_shift != want_shift)
        return 0;
    if (codepoint == (uint32_t)(ch - 'A' + 1))
        return 1;
    return (mods & primary_mod()) != 0 &&
        (codepoint == (uint32_t)ch || codepoint == (uint32_t)(ch + 32));
}

/* Menu items already carry a decorative shortcut string (e.g. "F7",
 * "Ctrl+F9") for display in the dropdown - this makes it functional too, so
 * a global function-key press fires whatever item declares that shortcut,
 * regardless of focus. Only Fn/Ctrl+Fn are recognized (all this app's menus
 * use); anything else in the string just fails to match and is ignored.
 *
 * Shortcuts are always declared in "Ctrl+..." form, but a "Ctrl+" token here
 * means the platform's *primary* modifier (see primary_mod()) - Cmd instead
 * of Ctrl when macOS conventions are active - so every declared shortcut
 * (Ctrl+O, Ctrl+F9, Ctrl+Shift+S, ...) becomes its Cmd+ equivalent on macOS
 * without the app needing separate strings per platform. display_shortcut()
 * below renders the same substitution for the on-screen hint text. */
static int shortcut_matches(const char* shortcut, int code, int mods, uint32_t codepoint)
{
    int want_primary = 0, want_shift = 0;
    const char* p = shortcut;
    /* Consume any leading "Ctrl+"/"Shift+" modifier tokens (in either order,
     * e.g. "Ctrl+Shift+S"). */
    for (;;)
    {
        if (strncmp(p, "Ctrl+", 5) == 0) { want_primary = 1; p += 5; }
        else if (strncmp(p, "Shift+", 6) == 0) { want_shift = 1; p += 6; }
        else break;
    }
    int has_primary = (mods & primary_mod()) != 0;
    int has_shift = (mods & UI_MOD_SHIFT) != 0;

    if (p[0] == 'F' && p[1] >= '0' && p[1] <= '9')
    {
        int n = atoi(p + 1);
        if (n < 1 || n > 12)
            return 0;
        return code == UI_KEY_F1 + (n - 1) &&
            has_primary == want_primary && has_shift == want_shift;
    }
    /* Ctrl+<letter> (optionally with Shift) / Cmd+<letter> on macOS - see
     * primary_letter_matches() for how each convention's raw event shape is
     * recognized. The modifiers MUST be checked exactly: several control
     * chars double as ordinary keys (Ctrl+H==Backspace, Ctrl+I==Tab,
     * Ctrl+M==Enter), and Ctrl+S must not fire a Ctrl+Shift+S binding (both
     * carry codepoint 19 - only the Shift state tells them apart). */
    if (want_primary && p[0] >= 'A' && p[0] <= 'Z' && p[1] == '\0')
        return primary_letter_matches(p[0], mods, codepoint, want_shift);
    return 0;
}

static int activate_menu_shortcut(ui_screen* s, int code, int mods, uint32_t codepoint)
{
    ui_node* menubar = find_child_by_type(s->root, UI_TAG_MENUBAR);
    if (menubar)
    {
        for (int i = 0; i < menubar->child_count; i++)
        {
            ui_node* menu = menubar->children[i];
            for (int j = 0; j < menu->child_count; j++)
            {
                ui_node* item = menu->children[j];
                if (item->type == UI_TAG_ITEM && item->enabled && item->shortcut[0] &&
                    shortcut_matches(item->shortcut, code, mods, codepoint))
                {
                    ui_fire_event(s, item->id, NULL);
                    return 1;
                }
            }
        }
    }

    /* A <hotkey> in the status bar can carry a shortcut too (see
     * ui_set_shortcut) - e.g. Navigate Back/Forward, which the app puts
     * there instead of a menu. Same matching, just a different, flatter
     * parent (no submenu nesting to walk). */
    ui_node* statusbar = find_child_by_type(s->root, UI_TAG_STATUSBAR);
    if (statusbar)
    {
        for (int i = 0; i < statusbar->child_count; i++)
        {
            ui_node* hk = statusbar->children[i];
            if (hk->type == UI_TAG_HOTKEY && hk->enabled && hk->shortcut[0] &&
                shortcut_matches(hk->shortcut, code, mods, codepoint))
            {
                ui_fire_event(s, hk->id, NULL);
                return 1;
            }
        }
    }
    return 0;
}

/* Close and free a ui_message_box marked for teardown this frame (its button
 * fired, or Escape closed it) - detach from the stack and the document, then
 * free its subtree. Deferred to here so the tree isn't mutated mid-walk. */
static void free_pending_msgbox(ui_screen* s)
{
    ui_node* m = s->msgbox_to_free;
    if (!m)
        return;
    s->msgbox_to_free = NULL;
    ui_screen_close_modal(s, m);   /* pops the stack, clears focus/drag state */
    ui_remove_child(s->root, m);
    ui_node_free(m);
}

void ui_screen_update(ui_screen* s, ui_env* env)
{
    g_ui_undo_now_ms = env->time_ms;  /* drives editor_undo_mark's coalescing
                                        * window for any edits this frame */
    s->mouse_pressed = 0;
    s->mouse_released = 0;
    s->mouse_moved = 0;
    s->mouse_dblclick = 0;
    s->mouse_right_pressed = 0;
    s->key_event_count = 0;
    s->hot = NULL;

    /* Minimized OS windows report a degenerate content size (0x0, or a
     * sliver) on at least Windows/macOS - not a real resize. Feeding that
     * through as a genuine screen_w/h would make reflow_on_screen_resize
     * (and dock_layout right after it) squash every window, docked panels
     * included, down toward their 1-cell floor - and because those
     * functions write the squashed numbers straight into window->w/h
     * (a dock's own w/h *is* its remembered thickness - see dock_layout),
     * that becomes the new "last known good" size. Restoring the OS window
     * afterward stretches back out from the wrecked numbers instead of the
     * original layout, since nothing preserved it. So: treat anything
     * smaller than the app's own minimum window size (see the resizable-
     * window drag clamp below, 12x5) as "no usable frame yet" and simply
     * keep rendering/laying out against the last known-good screen size
     * until a sane one comes back, rather than ever touching window rects
     * against the bogus one. */
    int new_screen_w = ui_env_width(env);
    int new_screen_h = ui_env_height(env);
    if (new_screen_w >= 12 && new_screen_h >= 5)
    {
        s->screen_w = new_screen_w;
        s->screen_h = new_screen_h;
    }
    else if (!s->screen_w || !s->screen_h)
    {
        /* First frame ever, and even it is degenerate - fall back to the
         * bogus size rather than leaving 0x0, so nothing downstream divides
         * by zero; it'll self-correct the moment a real size arrives. */
        s->screen_w = new_screen_w;
        s->screen_h = new_screen_h;
    }

    /* OS window resize: keep edge-anchored windows pinned to the new bounds.
     * Guarded on a non-zero previous size so the very first frame (prev == 0)
     * is never mistaken for a resize. */
    if (s->prev_screen_w && s->prev_screen_h &&
        (s->prev_screen_w != s->screen_w || s->prev_screen_h != s->screen_h))
        reflow_on_screen_resize(s, s->prev_screen_w, s->prev_screen_h);
    s->prev_screen_w = s->screen_w;
    s->prev_screen_h = s->screen_h;

    /* Lay out any docked Folder/Output-style windows (see ui_set_dock)
     * against the (possibly just-changed) screen size before anything else
     * touches window rects this frame - a resize-drag on one later this
     * same frame calls it (and resync_maximized_windows) again itself so
     * its neighbor/maximized windows pick that up immediately too (see
     * process_window). */
    dock_layout(s);
    resync_maximized_windows(s);

    ui_event ev;
    while (ui_env_poll_event(env, &ev))
    {
        if (ev.type == UI_EVENT_MOUSE)
        {
            if (ev.data.mouse.x != s->mouse_x || ev.data.mouse.y != s->mouse_y)
                s->mouse_moved = 1;
            s->mouse_x = ev.data.mouse.x;
            s->mouse_y = ev.data.mouse.y;
            s->mouse_mods = ev.data.mouse.mods;
            if (ev.data.mouse.action == UI_MOUSE_PRESSED &&
                ev.data.mouse.button == UI_MOUSE_BUTTON_LEFT)
            {
                s->mouse_down = 1;
                s->mouse_pressed = 1;
            }
            else if (ev.data.mouse.action == UI_MOUSE_RELEASED &&
                    ev.data.mouse.button == UI_MOUSE_BUTTON_LEFT)
            {
                s->mouse_down = 0;
                s->mouse_released = 1;
            }
            else if (ev.data.mouse.action == UI_MOUSE_DBLCLICK &&
                    ev.data.mouse.button == UI_MOUSE_BUTTON_LEFT)
            {
                s->mouse_down = 1;
                s->mouse_pressed = 1;
                s->mouse_dblclick = 1;
            }
            else if (ev.data.mouse.action == UI_MOUSE_PRESSED &&
                    ev.data.mouse.button == UI_MOUSE_BUTTON_RIGHT)
            {
                s->mouse_right_pressed = 1;  /* for a context-menu popup */
            }
            else if (ev.data.mouse.action == UI_MOUSE_WHEEL)
            {
                /* Scrolls whatever <editor> is under the cursor, regardless
                 * of focus - like every other app's mouse wheel, which
                 * scrolls the pane you're pointing at rather than the one
                 * with keyboard focus. Deliberately bypasses
                 * editor_ensure_cursor_visible(): the view is meant to move
                 * away from the caret here, not snap back to it - this is
                 * the only thing that ever touches n->scroll/hscroll for a
                 * wheel event, so nothing else (caret-follow, edits, resize)
                 * can eat or resize a step out from under it. wheel_delta is
                 * already exactly +-1 per notch (see ui.h) and every notch
                 * moves the view exactly one line/row/column - no per-event
                 * multiplier - so scrolling tracks the wheel continuously
                 * instead of jumping in multi-line chunks. Checked
                 * front-to-back across every floating window (only the
                 * topmost one at that point can be under the cursor), or
                 * just the active modal's window while one is blocking. */
                ui_node* hit = NULL;
                if (s->modal_stack_count > 0)
                {
                    ui_node* window = find_child_by_type(
                        s->modal_stack[s->modal_stack_count - 1], UI_TAG_WINDOW);
                    if (window)
                        hit = find_editor_at(window, ev.data.mouse.x, ev.data.mouse.y);
                }
                else
                {
                    for (int i = s->window_count - 1; i >= 0 && !hit; i--)
                    {
                        ui_node* window = find_child_by_type(s->windows[i], UI_TAG_WINDOW);
                        if (window)
                            hit = find_editor_at(window, ev.data.mouse.x, ev.data.mouse.y);
                    }
                }
                if (hit && hit->type == UI_TAG_LISTBOX)
                {
                    hit->scroll -= ev.data.mouse.wheel_delta;  /* 1 row/notch */
                    listbox_clamp_scroll(hit);
                }
                else if (hit && (ev.data.mouse.mods & UI_MOD_SHIFT))
                {
                    /* Shift+wheel pans horizontally, so long lines and the
                     * inline diagnostics past their end can be read. */
                    hit->hscroll -= ev.data.mouse.wheel_delta;  /* 1 col/notch */
                    editor_clamp_hscroll(hit);
                }
                else if (hit)
                {
                    hit->scroll -= ev.data.mouse.wheel_delta;  /* 1 line/notch */
                    editor_clamp_scroll(hit);
                }
            }
        }
        else if (ev.type == UI_EVENT_KEY)
        {
            if (s->key_event_count < UI_MAX_KEY_EVENTS)
                s->key_events[s->key_event_count++] = ev;
        }
    }

    /* A selection drag only continues while the button that started it is
     * still held - once released, the very next click anywhere (even back
     * inside the same field) must start a fresh selection, not resume this
     * one. */
    if (!s->mouse_down)
        s->selecting = NULL;

    for (int i = 0; i < s->key_event_count; i++)
    {
        ui_event* ev2 = &s->key_events[i];
        if (ev2->data.key.code == UI_KEY_ESCAPE)
        {
            s->open_menu = NULL;
            s->open_select = NULL;
            s->focused = NULL;
            if (s->modal_stack_count > 0)
            {
                /* Escape dismisses the active blocking modal only - a message
                 * box is closed AND freed at end of update; other modals just
                 * pop off the stack (the app owns their nodes). A non-modal
                 * floating window (e.g. a document/editor window) is NOT closed
                 * by Escape - only its own close icon / a menu action does. */
                ui_node* top = s->modal_stack[s->modal_stack_count - 1];
                if (top->msgbox)
                    s->msgbox_to_free = top;
                else
                    s->modal_stack_count--;
            }
            s->dragging_window = NULL;
            s->resizing_window = NULL;
            s->dragging_editor_vscrollbar = NULL;
            s->dragging_editor_hscrollbar = NULL;
            s->dragging_listbox_scrollbar = NULL;
            s->selecting = NULL;
            continue;
        }
        if (ev2->data.key.code == UI_KEY_TAB)
        {
            /* An editor never uses Tab to move keyboard focus - it's always
             * a text-editing key there, same as any real code editor:
             *   - a selection: Tab/Shift+Tab shifts every selected line
             *     right/left (block indent/outdent) - see
             *     editor_indent_selection() above.
             *   - no selection: Tab is ordinary typing at the caret (a
             *     soft tab, affecting only the current line -
             *     editor_insert_soft_tab()); Shift+Tab outdents just that
             *     one line (editor_outdent_current_line()).
             * Anywhere else (dialog fields, buttons, ...), Tab keeps its
             * usual job of moving focus to the next/previous widget. */
            if (s->focused && s->focused->type == UI_TAG_EDITOR)
            {
                ui_node* ed = s->focused;
                int shift = (ev2->data.key.mods & UI_MOD_SHIFT) != 0;
                if (has_selection(ed))
                    editor_indent_selection(ed, shift);
                else if (shift)
                    editor_outdent_current_line(ed);
                else
                    editor_insert_soft_tab(ed);
            }
            else
            {
                focus_next(s, !(ev2->data.key.mods & UI_MOD_SHIFT));
            }
            continue;
        }
        if (activate_menu_shortcut(s, ev2->data.key.code, ev2->data.key.mods,
            ev2->data.key.codepoint))
            continue;
        if (!s->focused)
            continue;

        ui_node* in = s->focused;

        /* GROUP: Up/Down move the keyboard-focused row, Space activates it
         * (toggles a check box, or picks a radio); a radio also follows the
         * arrows directly, like a native radio cluster. */
        if (in->type == UI_TAG_GROUP)
        {
            int cnt = in->child_count;
            int focus = in->multi ? in->cursor_row : in->selected;
            if (ev2->data.key.code == UI_KEY_UP)
            {
                if (focus > 0)
                    focus--;
            }
            else if (ev2->data.key.code == UI_KEY_DOWN)
            {
                if (focus < cnt - 1)
                    focus++;
            }
            else if (ev2->data.key.codepoint == ' ' && focus >= 0 && focus < cnt)
            {
                if (in->multi)
                {
                    in->children[focus]->selected = !in->children[focus]->selected;
                    ui_fire_event(s, in->id, NULL);
                }
            }
            if (in->multi)
            {
                in->cursor_row = focus;
            }
            else if (focus != in->selected)
            {
                in->selected = focus;  /* arrows re-pick the radio immediately */
                ui_fire_event(s, in->id, NULL);
            }
            continue;
        }

        /* <listbox> has no cursor/label/selection-range concept at all - a
         * completely separate, much smaller key vocabulary (move the
         * current row, activate it) than the text-editing logic the rest of
         * this loop body is about. */
        if (in->type == UI_TAG_LISTBOX)
        {
            if (ev2->data.key.code == UI_KEY_UP)
            {
                if (in->selected > 0)
                    in->selected--;
                listbox_ensure_visible(in);
            }
            else if (ev2->data.key.code == UI_KEY_DOWN)
            {
                if (in->selected < in->child_count - 1)
                    in->selected++;
                listbox_ensure_visible(in);
            }
            else if (ev2->data.key.codepoint == '\r' || ev2->data.key.codepoint == '\n')
            {
                ui_fire_event(s, in->id, NULL);
            }
            else if ((ev2->data.key.codepoint >= 'A' && ev2->data.key.codepoint <= 'Z') ||
                    (ev2->data.key.codepoint >= 'a' && ev2->data.key.codepoint <= 'z'))
            {
                /* Letter key pressed: jump to first item starting with that letter */
                char target_letter = ev2->data.key.codepoint;
                /* Normalize to uppercase for case-insensitive search */
                if (target_letter >= 'a' && target_letter <= 'z')
                    target_letter = target_letter - 'a' + 'A';

                int start_search = (in->selected + 1) % in->child_count;
                int found = -1;

                /* Search forward from next item, wrapping around if needed */
                for (int attempt = 0; attempt < in->child_count; attempt++)
                {
                    int idx = (start_search + attempt) % in->child_count;
                    if (in->children[idx]->label && in->children[idx]->label[0])
                    {
                        char first_char = in->children[idx]->label[0];
                        /* Case-insensitive comparison */
                        if (first_char >= 'a' && first_char <= 'z')
                            first_char = first_char - 'a' + 'A';
                        if (first_char == target_letter)
                        {
                            found = idx;
                            break;
                        }
                    }
                }

                if (found >= 0)
                {
                    in->selected = found;
                    listbox_ensure_visible(in);
                }
            }
            continue;
        }

        int is_editor = (in->type == UI_TAG_EDITOR);
        int code = ev2->data.key.code;

        /* Shift+movement extends the selection (starting it fresh from the
         * cursor's current position if none is active yet); movement without
         * Shift always collapses whatever selection there was, same as every
         * other text editor. */
        int is_nav = code == UI_KEY_LEFT || code == UI_KEY_RIGHT ||
            code == UI_KEY_UP || code == UI_KEY_DOWN ||
            code == UI_KEY_HOME || code == UI_KEY_END ||
            code == UI_KEY_PAGEUP || code == UI_KEY_PAGEDOWN;
        if (is_nav)
        {
            if (ev2->data.key.mods & UI_MOD_SHIFT)
            {
                if (in->sel_anchor < 0)
                    in->sel_anchor = in->cursor;
            }
            else
            {
                in->sel_anchor = -1;
            }
        }

        switch (code)
        {
        case UI_KEY_LEFT:
            if (mac_nav_mod_active(ev2->data.key.mods))
                in->cursor = is_editor ? editor_line_start(in) : 0;  /* macOS Cmd+Left: line start */
            else if (ev2->data.key.mods & word_mod())
                in->cursor = editor_word_left(in->label, in->cursor);
            else if (in->cursor > 0)
                in->cursor -= utf8_prev_len(in->label, in->cursor);
            break;
        case UI_KEY_RIGHT:
            if (mac_nav_mod_active(ev2->data.key.mods))
            {
                in->cursor = is_editor ? editor_line_end(in)
                    : (int)strlen(in->label);  /* macOS Cmd+Right: line end */
            }
            else if (ev2->data.key.mods & word_mod())
            {
                in->cursor = editor_word_right(in->label, (int)strlen(in->label),
                                               in->cursor);
            }
            else if (in->cursor < (int)strlen(in->label))
            {
                uint32_t cp;
                in->cursor += utf8_decode(in->label + in->cursor, &cp);
            }
            break;
        case UI_KEY_UP:
            if (is_editor && mac_nav_mod_active(ev2->data.key.mods))
                in->cursor = 0;  /* macOS Cmd+Up: top of the document */
            else if (is_editor)
                editor_move_vertical(in, -1);
            break;
        case UI_KEY_DOWN:
            if (is_editor && mac_nav_mod_active(ev2->data.key.mods))
                in->cursor = (int)strlen(in->label);  /* macOS Cmd+Down: end of the document */
            else if (is_editor)
                editor_move_vertical(in, 1);
            break;
        case UI_KEY_PAGEUP:
            if (is_editor)
                editor_move_lines(in, -in->h);
            break;
        case UI_KEY_PAGEDOWN:
            if (is_editor)
                editor_move_lines(in, in->h);
            break;
        case UI_KEY_HOME:
            if (is_editor && (ev2->data.key.mods & UI_MOD_CTRL))
                in->cursor = 0;  /* Ctrl+Home: the document's first line */
            else if (is_editor)
                in->cursor = editor_line_start(in);
            else
                in->cursor = 0;
            break;
        case UI_KEY_END:
            if (is_editor && (ev2->data.key.mods & UI_MOD_CTRL))
                in->cursor = (int)strlen(in->label);  /* Ctrl+End: the last line */
            else if (is_editor)
                in->cursor = editor_line_end(in);
            else
                in->cursor = (int)strlen(in->label);
            break;
        case UI_KEY_DELETE:
            if (has_selection(in))
                editor_delete_selection(in);
            else
                input_delete_forward(in);
            break;
        default:
        {
            /* Printable chars, Backspace, Enter, and Ctrl+C/X/V all arrive
             * with code==0 and only codepoint set (see win32.c's WM_CHAR
             * handling - Windows' own TranslateMessage() turns Ctrl+letter
             * into the matching control character; primary_letter_matches()
             * above covers Cmd+letter on macOS the same way). */
            int mods = ev2->data.key.mods;
            uint32_t cp = ev2->data.key.codepoint;
            if (primary_letter_matches('A', mods, cp, 0))
            {       /* select all */
                in->sel_anchor = 0;
                in->cursor = (int)strlen(in->label);
            }
            else if (primary_letter_matches('C', mods, cp, 0))
            { /* copy */
                editor_copy(in);
            }
            else if (primary_letter_matches('X', mods, cp, 0))
            { /* cut */
                editor_cut(in);
            }
            else if (primary_letter_matches('V', mods, cp, 0))
            { /* paste */
                editor_paste(in);
            }
            else if (primary_letter_matches('Z', mods, cp, 0))
            { /* Ctrl+Z / Cmd+Z: undo */
                ui_editor_undo(in);
            }
            else if (primary_letter_matches('Z', mods, cp, 1) || /* Cmd+Shift+Z (macOS) */
                    cp == 25)
            {                                 /* Ctrl+Y (Windows/X11): redo */
                ui_editor_redo(in);
            }
            else if (ev2->data.key.codepoint == 8)
            {
                if (has_selection(in))
                    editor_delete_selection(in);
                else
                {
                    input_backspace(in);
                    /* A plain click leaves sel_anchor == cursor, not -1 (see
                     * editor_insert_soft_tab's own comment on this) -
                     * input_backspace() never touches sel_anchor, so left
                     * alone it now sits one character ahead of the moved
                     * caret and has_selection() flips true next frame,
                     * reading back as a real (if tiny) selection out of
                     * nowhere. Collapsing it onto the caret keeps this a
                     * plain "moved while editing", same as any other
                     * keystroke starting from a real caret. */
                    in->sel_anchor = -1;
                }
            }
            else if (ev2->data.key.codepoint == '\r' || ev2->data.key.codepoint == '\n')
            {
                if (is_editor)
                {
                    if (has_selection(in))
                        editor_delete_selection(in);
                    else
                        in->sel_anchor = -1;  /* see the Backspace branch's own comment above */
                    input_insert(in, '\n', UI_UNDO_NEWLINE);  /* a real newline, not a submit */
                }
                else
                {
                    ui_fire_event(s, in->id, NULL);  /* Enter: like a form submit */
                    s->focused = NULL;
                }
            }
            else if (ev2->data.key.codepoint >= 0x20 && ev2->data.key.codepoint != 0x7F)
            {
                if (has_selection(in))
                    editor_delete_selection(in);
                else
                    in->sel_anchor = -1;  /* see the Backspace branch's own comment above */
                input_insert(in, ev2->data.key.codepoint, UI_UNDO_TYPE);
            }
            break;
        }
        }

        if (is_editor)
            editor_ensure_cursor_visible(in);
    }

    /* Text-caret blink phase: any keyboard/mouse activity this frame resets it
     * to solid (so the caret is always visible the instant it moves), and once
     * idle it toggles every UI_CARET_BLINK_MS of the backend's wall clock. If
     * the backend never feeds time (time_ms stays 0), elapsed is always 0 and
     * the caret just stays solid. */
    {
        unsigned now = env->time_ms;
        if (s->key_event_count > 0 || s->mouse_pressed || s->mouse_moved)
            s->caret_blink_base = now;
        s->caret_visible = (((now - s->caret_blink_base) / UI_CARET_BLINK_MS) % 2) == 0;
    }

    /* Menubar/statusbar layout only depends on screen size and their own
     * content, never on modal state - so it must run every frame regardless
     * of what's below, not just in the non-modal branch further down. Doing
     * it only there left their cached x/y/w stale (wrong width after a
     * screen resize) for as long as a modal stayed active or the mouse
     * hovered a floating window, since both cases can return before ever
     * reaching that branch. */
    ui_node* menubar_always = find_child_by_type(s->root, UI_TAG_MENUBAR);
    ui_node* statusbar_always = find_child_by_type(s->root, UI_TAG_STATUSBAR);
    if (menubar_always)
        layout_menubar(menubar_always, ui_env_width(env));
    if (statusbar_always)
        layout_statusbar(statusbar_always, ui_env_width(env), ui_env_height(env));

    /* The menu bar and its open dropdown always render as the topmost layer
     * (see ui_screen_render) - so they must also get first pick of every
     * click AND of hover, checked here before any modal/window, even if a
     * window happens to occupy the same screen position. `over_menu` is the
     * general "the mouse is somewhere over the bar or the dropdown box right
     * now" flag (true even without a click, e.g. just moving the mouse over
     * it) - `click_consumed` is specifically "a press this frame landed on
     * it". Both are needed: without over_menu, a window underneath could
     * still steal hover highlighting (s->hot) on every mouse-move frame,
     * since that isn't gated by whether anything was pressed. */
    int click_consumed = 0;
    int over_menu = 0;
    if (menubar_always)
    {
        ui_node* menubar = menubar_always;
        if (s->mouse_y == menubar->y &&
            s->mouse_x >= menubar->x && s->mouse_x < menubar->x + menubar->w)
            over_menu = 1;  /* the whole bar row is a solid strip, not just headers */

        for (int i = 0; i < menubar->child_count; i++)
        {
            ui_node* m = menubar->children[i];
            int inside = s->mouse_x >= m->x && s->mouse_x < m->x + m->w &&
                s->mouse_y == m->y;
            if (inside)
            {
                s->hot = m;
                if (s->open_menu)
                {
                    s->open_menu = m;  /* hover-switch while a menu is open */
                    s->open_popup_param = NULL;
                }
            }
            if (inside && s->mouse_pressed)
            {
                click_consumed = 1;
                s->open_menu = (s->open_menu == m) ? NULL : m;
                s->open_popup_param = NULL;  /* an ordinary dropdown, not a popup */
            }
        }

        if (s->open_menu)
        {
            /* Cache the menu: hit_test_widget below can set s->open_menu to
             * NULL (item picked) mid-loop, and the loop must not re-read
             * s->open_menu->child_count after that (NULL dereference). */
            ui_node* menu = s->open_menu;
            int dx, dy, box_w, box_h;
            layout_dropdown(menu, &dx, &dy, &box_w, &box_h);
            if (s->mouse_x >= dx && s->mouse_x < dx + box_w &&
                s->mouse_y >= dy && s->mouse_y < dy + box_h)
                over_menu = 1;  /* the dropdown's whole box, not just its items */

            /* Drop a submenu selection left over from a now-closed menu. */
            if (s->open_submenu && !is_direct_child(menu, s->open_submenu))
                s->open_submenu = NULL;

            int closed = 0;
            for (int i = 0; i < menu->child_count && !closed; i++)
            {
                ui_node* it = menu->children[i];
                int inside = s->mouse_x >= it->x && s->mouse_x < it->x + it->w &&
                    s->mouse_y == it->y;
                if (item_is_submenu(it))
                {
                    /* Hovering a submenu parent opens its child dropdown;
                     * clicking it just keeps the menu open (it has no action
                     * of its own). */
                    if (inside)
                    {
                        s->hot = it;
                        s->open_submenu = it;
                        if (s->mouse_pressed)
                            click_consumed = 1;
                    }
                }
                else
                {
                    int item_consumed = 0;
                    if (hit_test_widget(s, it, inside, &item_consumed, s->open_popup_param))
                    {
                        s->open_menu = NULL;  /* picked this item - close all */
                        s->open_submenu = NULL;
                        click_consumed = 1;
                        closed = 1;
                    }
                    else
                    {
                        if (item_consumed)
                            click_consumed = 1;
                        if (inside)
                            s->open_submenu = NULL;  /* a normal item closes any submenu */
                    }
                }
            }

            /* The open submenu's own items (a second dropdown to the right). */
            if (!closed && s->open_submenu)
            {
                ui_node* sm = s->open_submenu;
                int sdx, sdy, sbw, sbh;
                layout_submenu(sm, dx + box_w, &sdx, &sdy, &sbw, &sbh);
                if (s->mouse_x >= sdx && s->mouse_x < sdx + sbw &&
                    s->mouse_y >= sdy && s->mouse_y < sdy + sbh)
                    over_menu = 1;

                for (int i = 0; i < sm->child_count; i++)
                {
                    ui_node* it = sm->children[i];
                    int inside = s->mouse_x >= it->x && s->mouse_x < it->x + it->w &&
                        s->mouse_y == it->y;
                    int item_consumed = 0;
                    if (hit_test_widget(s, it, inside, &item_consumed, s->open_popup_param))
                    {
                        s->open_menu = NULL;
                        s->open_submenu = NULL;
                        click_consumed = 1;
                        break;
                    }
                    if (item_consumed)
                        click_consumed = 1;
                }
            }
        }
    }

    /* A <select>'s open popup is the same kind of overlay as an open menu's
     * dropdown - always the topmost layer (see ui_screen_render), so it
     * needs the same first-pick-of-every-click-and-hover treatment, folded
     * into the very same over_menu/click_consumed flags rather than a
     * parallel pair of "over_select"/"select_consumed" ones, since both
     * mean exactly the same thing to everything checking them: "an overlay
     * above the document already claimed this." */
    if (s->open_select)
    {
        ui_node* select = s->open_select;
        int dx, dy, box_w, box_h;
        layout_select_popup(select, &dx, &dy, &box_w, &box_h);
        if (s->mouse_x >= dx && s->mouse_x < dx + box_w &&
            s->mouse_y >= dy && s->mouse_y < dy + box_h)
            over_menu = 1;

        for (int i = 0; i < select->child_count; i++)
        {
            ui_node* it = select->children[i];
            int inside = s->mouse_x >= it->x && s->mouse_x < it->x + it->w &&
                s->mouse_y == it->y;
            if (inside)
                s->hot = it;
            if (inside && s->mouse_pressed)
            {
                select->selected = i;
                s->open_select = NULL;
                click_consumed = 1;
                ui_fire_event(s, it->id, NULL);
                break;
            }
        }
    }

    if (s->modal_stack_count > 0)
    {
        /* Exactly one active container - the blocking modal on top of the
         * stack - same as before multiple floating windows existed. */
        ui_node* container = s->modal_stack[s->modal_stack_count - 1];
        ui_node* window = find_child_by_type(container, UI_TAG_WINDOW);
        int handled = process_window(s, container, window, over_menu, 1, &click_consumed);
        free_pending_msgbox(s);  /* a message-box button (or Escape) tore it down */
        if (handled)
            return;
    }
    else if (s->window_count > 0)
    {
        /* Floating windows stack in z-order (windows[window_count-1] is
         * frontmost) and, unlike a modal, more than one can be open at once
         * - so only ONE gets this frame's interaction: whichever is already
         * mid-drag/resize/scrollbar-drag takes absolute priority (an in-
         * progress gesture must keep going even if the window being
         * dragged/resized grows or moves underneath another one this very
         * frame, or the mouse strays outside it entirely - checking hit-test
         * first here was the bug: resizing a window until its corner
         * crossed under a frontmost neighbor would silently hand the rest
         * of the drag to that neighbor instead, dropping the resize; a
         * scrollbar drag has the same problem the instant the mouse leaves
         * the window's bounds, e.g. dragging fast). With no drag/resize in
         * progress, it's whichever window the mouse is over right now,
         * front-to-back (an overlapping window fully occludes whatever's
         * behind it at that point). A press on any window that isn't
         * already frontmost brings it to the front first, like every other
         * window manager - but only when it's also the one about to receive
         * the frame's interaction, never when a drag/resize elsewhere is
         * what claimed it instead. */
        int active_idx = -1;
        if (s->dragging_window || s->resizing_window || s->dragging_editor_vscrollbar ||
            s->dragging_editor_hscrollbar || s->dragging_listbox_scrollbar)
        {
            for (int i = 0; i < s->window_count; i++)
            {
                ui_node* w = find_child_by_type(s->windows[i], UI_TAG_WINDOW);
                if (w == s->dragging_window || w == s->resizing_window ||
                    is_direct_child(w, s->dragging_editor_vscrollbar) ||
                    is_direct_child(w, s->dragging_editor_hscrollbar) ||
                    is_direct_child(w, s->dragging_listbox_scrollbar))
                {
                    active_idx = i;
                    break;
                }
            }
        }

        int hit_idx = -1;
        for (int i = s->window_count - 1; i >= 0; i--)
        {
            ui_node* w = find_child_by_type(s->windows[i], UI_TAG_WINDOW);
            if (w && s->mouse_x >= w->x && s->mouse_x < w->x + w->w &&
                s->mouse_y >= w->y && s->mouse_y < w->y + w->h)
            {
                hit_idx = i;
                break;
            }
        }

        if (active_idx < 0)
            active_idx = hit_idx;

        if (active_idx >= 0)
        {
            if ((s->mouse_pressed || s->mouse_right_pressed) && hit_idx == active_idx && hit_idx != s->window_count - 1)
            {
                ui_node* promoted = s->windows[hit_idx];
                for (int j = hit_idx; j < s->window_count - 1; j++)
                    s->windows[j] = s->windows[j + 1];
                s->windows[s->window_count - 1] = promoted;
                active_idx = s->window_count - 1;
            }
            ui_node* container = s->windows[active_idx];
            ui_node* window = find_child_by_type(container, UI_TAG_WINDOW);
            if (process_window(s, container, window, over_menu, 0, &click_consumed))
                return;
        }
    }

    ui_node* statusbar = statusbar_always;

    if (statusbar)
    {
        for (int i = 0; i < statusbar->child_count; i++)
        {
            ui_node* h = statusbar->children[i];
            int inside = s->mouse_x >= h->x && s->mouse_x < h->x + h->w &&
                s->mouse_y == h->y;
            hit_test_widget(s, h, inside, NULL, NULL);
        }
    }

    for (int i = 0; i < s->root->child_count; i++)
    {
        ui_node* b = s->root->children[i];
        if (b->type != UI_TAG_BUTTON)
            continue;
        int inside = s->mouse_x >= b->x && s->mouse_x < b->x + b->w &&
            s->mouse_y >= b->y && s->mouse_y < b->y + b->h;
        hit_test_widget(s, b, inside, &click_consumed, NULL);
    }

    for (int i = 0; i < s->root->child_count; i++)
    {
        ui_node* in = s->root->children[i];
        if (in->type != UI_TAG_INPUT)
            continue;
        int inside = s->mouse_x >= in->x && s->mouse_x < in->x + in->w &&
            s->mouse_y == in->y;
        if (inside)
            s->hot = in;
        if (inside && s->mouse_pressed)
        {
            s->focused = in;
            click_consumed = 1;
        }
    }

    /* Clicking anywhere a widget didn't claim closes the open dropdown/combo
     * popup and blurs the focused input - like clicking outside a
     * <select>/<input>. */
    if (s->mouse_pressed && !click_consumed)
    {
        s->open_menu = NULL;
        s->open_select = NULL;
        s->focused = NULL;
    }
}

/* --- Render: walk the tree, emit draw calls --- */

/* The screen being rendered, for the duration of one ui_screen_render call.
 * The leaf render helpers (render_box, render_editor_line*, draw_text, ...)
 * deliberately don't take a ui_screen* - emit_char/emit_box reach the cache
 * through this instead. Rendering is never reentrant, so a single module
 * global is safe and keeps those signatures minimal. Set at the top of
 * ui_screen_render, cleared at the bottom. */
static ui_screen* g_render_screen;

/* Grow the frame's accumulated invalid rect to include cell rect (x,y,w,h),
 * clipped to the screen. */
static void union_dirty(ui_screen* s, int x, int y, int w, int h)
{
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > s->screen_w) w = s->screen_w - x;
    if (y + h > s->screen_h) h = s->screen_h - y;
    if (w <= 0 || h <= 0)
        return;
    if (s->dirty_w == 0)
    {
        s->dirty_x = x; s->dirty_y = y; s->dirty_w = w; s->dirty_h = h;
        return;
    }
    int x0 = s->dirty_x < x ? s->dirty_x : x;
    int y0 = s->dirty_y < y ? s->dirty_y : y;
    int x1 = s->dirty_x + s->dirty_w > x + w ? s->dirty_x + s->dirty_w : x + w;
    int y1 = s->dirty_y + s->dirty_h > y + h ? s->dirty_y + s->dirty_h : y + h;
    s->dirty_x = x0; s->dirty_y = y0; s->dirty_w = x1 - x0; s->dirty_h = y1 - y0;
}

/* Record one glyph cell into the frame's `next` buffer - no backend call
 * here; the diff pass in ui_screen_render decides what actually gets drawn. A
 * blank glyph has no visible foreground, so fg is normalized to 0 so two
 * blanks that differ only in fg compare equal in the diff. */
static void emit_char(int x, int y, uint32_t ch, uint32_t fg, uint32_t bg)
{
    ui_screen* s = g_render_screen;
    if (ch == 0)
        ch = ' ';
    if (!s || !s->next || x < 0 || y < 0 || x >= s->cache_w || y >= s->cache_h)
    {
        if (!s || !s->next)
            ui_draw_char(x, y, ch, fg, bg);  /* no buffer to record into */
        return;
    }
    if (ch == ' ')
        fg = 0;
    ui_cell* c = &s->next[y * s->cache_w + x];
    c->ch = ch; c->fg = fg; c->bg = bg;
    c->seq = ++s->emit_seq;  /* z-order stamp for overlay occlusion */
}

/* Record a destructive overlay (shadow/cursor) for replay after the content
 * diff - see ui_screen_render. */
static void record_overlay(ui_screen* s, int x, int y, int w, int h, int flags)
{
    if (s->overlay_count >= UI_MAX_OVERLAYS)
    {
        s->overlay_overflow = 1;  /* fall back to a full invalidate next frame */
        return;
    }
    ui_overlay* o = &s->overlays[s->overlay_count++];
    o->x = x; o->y = y; o->w = w; o->h = h; o->flags = flags;
    o->seq = s->emit_seq;  /* content written after this is above it in z-order */
}

/* Record a solid rect (flags==0) into `next`, or queue a destructive overlay
 * (UI_BOX_SHADOW / UI_BOX_INVERT) for post-diff replay. A plain fill just
 * writes blank cells with this bg into `next` - no backend call, no diffing
 * here. */
static void emit_box(int x, int y, int w, int h, uint32_t fg, uint32_t bg, int flags)
{
    ui_screen* s = g_render_screen;
    (void)fg;

    if (flags & (UI_BOX_SHADOW | UI_BOX_INVERT))
    {
        if (s && s->next)
            record_overlay(s, x, y, w, h, flags);
        else
            ui_draw_box(x, y, w, h, fg, bg, flags);
        return;
    }

    if (!s || !s->next)
    {
        ui_draw_box(x, y, w, h, fg, bg, 0);
        return;
    }

    /* Clip to the grid, then write blanks with this bg into `next`. */
    int cx = x, cy = y, cw = w, ch = h;
    if (cx < 0) { cw += cx; cx = 0; }
    if (cy < 0) { ch += cy; cy = 0; }
    if (cx + cw > s->cache_w) cw = s->cache_w - cx;
    if (cy + ch > s->cache_h) ch = s->cache_h - cy;
    unsigned z = ++s->emit_seq;  /* one z-order stamp for the whole fill */
    for (int j = 0; j < ch; j++)
    {
        ui_cell* row = &s->next[(cy + j) * s->cache_w + cx];
        for (int i = 0; i < cw; i++)
        {
            row[i].ch = ' '; row[i].fg = 0; row[i].bg = bg; row[i].seq = z;
        }
    }
}

static void draw_text(int x, int y, const char* utf8, uint32_t fg, uint32_t bg)
{
    const char* p = utf8;
    while (*p)
    {
        uint32_t cp;
        p += utf8_decode(p, &cp);
        emit_char(x, y, cp, fg, bg);
        x++;
    }
}

static void draw_fill(int x, int y, int w, int h, uint32_t fg, uint32_t bg)
{
    emit_box(x, y, w, h, fg, bg, 0);
}

/* ui_draw_box with UI_BOX_SHADOW set - a thin wrapper so every shadow-
 * casting call site below just passes coordinates + shrink flags. */
static void draw_shadow(int x, int y, int w, int h, int shrink)
{
    emit_box(x, y, w, h, 0, 0, UI_BOX_SHADOW | shrink);
}

/* The six box-drawing glyphs (corners + edges) for a given border style.
 * UI_BORDER_NONE maps every glyph to a space - emit_char() already turns
 * ' ' into a normalized blank cell, so "no border" is just "draw blanks in
 * the border color's background", not a separate skip-drawing path. */
struct ui_border_glyphs { uint32_t tl, tr, bl, br, edge_h, edge_v; };

static struct ui_border_glyphs border_glyphs(enum ui_border_style style)
{
    switch (style)
    {
    case UI_BORDER_SINGLE:
        return (struct ui_border_glyphs)
        {
            0x250C, 0x2510, 0x2514, 0x2518, 0x2500, 0x2502
        };
    case UI_BORDER_DOUBLE:
        return (struct ui_border_glyphs)
        {
            0x2554, 0x2557, 0x255A, 0x255D, 0x2550, 0x2551
        };
    case UI_BORDER_NONE:
    default:
        return (struct ui_border_glyphs) { ' ', ' ', ' ', ' ', ' ', ' ' };
    }
}

/* Draw a full rectangular frame (all four sides) in the given border style
 * and colors - the shared shape used by <box> and the dropdown/<select>
 * popup frames. A <window>'s frame is drawn by hand instead (see
 * render_window) since it needs per-side glyph overrides (unfocused style,
 * scrollbar edge skip, close/zoom icons cut into the top edge). */
static void draw_border(int x, int y, int w, int h,
                         enum ui_border_style style, uint32_t fg, uint32_t bg)
{
    struct ui_border_glyphs g = border_glyphs(style);

    emit_char(x, y, g.tl, fg, bg);
    for (int i = 1; i < w - 1; i++)
        emit_char(x + i, y, g.edge_h, fg, bg);
    emit_char(x + w - 1, y, g.tr, fg, bg);

    for (int i = 1; i < h - 1; i++)
    {
        emit_char(x, y + i, g.edge_v, fg, bg);
        emit_char(x + w - 1, y + i, g.edge_v, fg, bg);
    }

    emit_char(x, y + h - 1, g.bl, fg, bg);
    for (int i = 1; i < w - 1; i++)
        emit_char(x + i, y + h - 1, g.edge_h, fg, bg);
    emit_char(x + w - 1, y + h - 1, g.br, fg, bg);
}

static void render_button(ui_screen* s, ui_node* b)
{
    int is_active = (s->active == b && s->hot == b);
    uint32_t bg = is_active ? g_theme.btn_bg_active :
        (s->hot == b ? g_theme.btn_bg_hot : g_theme.btn_bg);

    /* Pressed: the face shifts right by one cell, into the space the shadow
     * otherwise occupies - like the button being pushed in flush with the
     * surface, so the shadow itself doesn't draw while active. */
    int ox = is_active ? 1 : 0;

    /* Four pieces, top to bottom - same shape/logic as every other shadow-
     * caster (render_open_dropdown, render_open_select, render_window):
     * the side strip's own top row is half height/bottom-anchored (tapering
     * in rather than poking out level with the top of whatever casts it,
     * mirroring the half-height/top-anchored corner at the bottom), any
     * remaining rows below it are full height, then the corner cell (half
     * height/top-anchored, full width), then the strip below the button
     * (half height/top-anchored, full width). A height-1 button ends up
     * with just that first half-height piece for its whole side strip. */
    if (!is_active)
    {
        draw_shadow(b->x + b->w, b->y, 1, 1, UI_BOX_SHADOW_SHRINK_H_BOTTOM);
        if (b->h > 1)
            draw_shadow(b->x + b->w, b->y + 1, 1, b->h - 1, 0);
        draw_shadow(b->x + b->w, b->y + b->h, 1, 1, UI_BOX_SHADOW_SHRINK_H);
        draw_shadow(b->x + 1, b->y + b->h, b->w - 1, 1, UI_BOX_SHADOW_SHRINK_H);
    }

    draw_fill(b->x + ox, b->y, b->w, b->h, g_theme.btn_fg, bg);

    int label_len = (int)strlen(b->label);
    int lx = b->x + ox + (b->w - label_len) / 2;
    int ly = b->y + b->h / 2;
    if (lx < b->x + ox)
        lx = b->x + ox;
    draw_text(lx, ly, b->label, g_theme.btn_fg, bg);
}

static void render_hotkey(ui_screen* s, ui_node* h)
{
    int is_hot = (s->hot == h);
    uint32_t bg = is_hot ? g_theme.hotkey_bg_hot : g_theme.hotkey_bg;
    const char* colon = strchr(h->label, ':');
    int key_len = colon ? (int)(colon - h->label) : h->w;

    char key_part[16];
    int n = key_len < (int)sizeof(key_part) - 1 ? key_len : (int)sizeof(key_part) - 1;
    memcpy(key_part, h->label, n);
    key_part[n] = 0;

    uint32_t key_fg = is_hot ? g_theme.hotkey_fg_hot : g_theme.hotkey_key_fg;
    draw_text(h->x, h->y, key_part, key_fg, bg);
    if (colon)
    {
        uint32_t fg = is_hot ? g_theme.hotkey_fg_hot : g_theme.hotkey_fg;
        draw_text(h->x + key_len, h->y, colon, fg, bg);
    }
}

/* Full-width background, then each hotkey item (see render_hotkey), then -
 * space permitting - the bar's own `label` (see ui_set_label/ui_get_label),
 * right-aligned past the last hotkey. Nothing in the framework requires a
 * statusbar to have a label; the app uses it purely decoratively (e.g.
 * "Cake 0.14.08" - see build_screen()). Silently omitted (not truncated)
 * whenever the bar is too narrow to fit it without overlapping the hotkeys -
 * a partial version string would be more confusing than none, and this is
 * cosmetic enough that it isn't worth a scroll/ellipsis treatment. Shared by
 * both places the statusbar is drawn each frame (its normal spot in the
 * base-layer document walk, and again as part of the app's always-on-top
 * chrome - see render_node/render() below) so the two can't drift apart. */
static void render_statusbar(ui_screen* s, ui_node* n)
{
    draw_fill(n->x, n->y, n->w, n->h, g_theme.hotkey_fg, g_theme.hotkey_bg);
    for (int j = 0; j < n->child_count; j++)
        render_hotkey(s, n->children[j]);

    if (n->label && n->label[0])
    {
        int used_x = 1;
        if (n->child_count > 0)
        {
            ui_node* last = n->children[n->child_count - 1];
            used_x = last->x + last->w;
        }
        int len = (int)strlen(n->label);
        int lx = n->x + n->w - 1 - len;  /* right-aligned, 1 col from the edge */
        if (lx > used_x + 1)
            draw_text(lx, n->y, n->label, g_theme.hotkey_fg, g_theme.hotkey_bg);
    }
}

/* Note: menubar/statusbar child x/y/w were already computed by the most
 * recent ui_screen_update() call (layout_menubar/layout_statusbar) - render
 * just reads them, it doesn't redo layout. Only the bar itself - the open
 * dropdown (if any) is a separate top-layer pass, see render_open_dropdown,
 * since it must paint over every other node regardless of document order. */
static void render_menubar(ui_screen* s, ui_node* menubar)
{
    /* Full-width background first, so the bar's color fills the row even
     * past the last menu (and behind any gaps), then items paint over it. */
    draw_fill(menubar->x, menubar->y, menubar->w, menubar->h,
              g_theme.menu_fg, g_theme.menu_bg);

    for (int i = 0; i < menubar->child_count; i++)
    {
        ui_node* m = menubar->children[i];
        int is_sel = (s->open_menu == m) || (s->hot == m);
        uint32_t fg = is_sel ? g_theme.menu_fg_sel : g_theme.menu_fg;
        uint32_t bg = is_sel ? g_theme.menu_bg_sel : g_theme.menu_bg;
        draw_fill(m->x, m->y, m->w, 1, fg, bg);
        draw_text(m->x + 1, m->y, m->label, fg, bg);
    }
}

/* Top-layer pass: the open dropdown (if any) paints last, on top of every
 * other node in the document - like a browser rendering an open <select> or
 * a modal in its own top layer rather than at its position in z-order. */
 /* Draw one dropdown panel: the shadow, the box frame, and `menu`'s <item>
  * children as rows (already laid out into their x/y/w/h). Shared by the
  * top-level menu dropdown and an open submenu. */
static void render_menu_panel(ui_screen* s, ui_node* menu, int dx, int dy, int box_w, int box_h)
{
    /* See render_button() for why this is four pieces. */
    draw_shadow(dx + box_w, dy, 1, 1, UI_BOX_SHADOW_SHRINK_H_BOTTOM);
    if (box_h > 1)
        draw_shadow(dx + box_w, dy + 1, 1, box_h - 1, 0);
    draw_shadow(dx + box_w, dy + box_h, 1, 1, UI_BOX_SHADOW_SHRINK_H);
    draw_shadow(dx + 1, dy + box_h, box_w - 1, 1, UI_BOX_SHADOW_SHRINK_H);

    draw_border(dx, dy, box_w, box_h, g_theme.menu_border_style,
                g_theme.menu_border_fg, g_theme.menu_border_bg);

    for (int i = 0; i < menu->child_count; i++)
    {
        ui_node* it = menu->children[i];

        if (it->separator)
        {
            for (int col = 0; col < it->w; col++)
                emit_char(it->x + col, it->y, 0x2500,
                          g_theme.menu_border_fg, g_theme.menu_border_bg);
            continue;
        }

        /* A submenu parent stays highlighted while its child dropdown is open,
         * as well as on plain hover. */
        int is_hot = (s->hot == it) || (it == s->open_submenu);
        uint32_t ifg = !it->enabled ? g_theme.menu_item_fg_disabled :
            is_hot ? g_theme.menu_item_fg_hot : g_theme.menu_item_fg;
        uint32_t ibg = is_hot ? g_theme.menu_item_bg_hot : g_theme.menu_item_bg;
        draw_fill(it->x, it->y, it->w, 1, ifg, ibg);
        draw_text(it->x + 1, it->y, it->label, ifg, ibg);

        if (item_is_submenu(it))
        {
            /* A right-pointing marker instead of a shortcut. */
            emit_char(it->x + it->w - 2, it->y, 0x25BA /* ► */, ifg, ibg);
        }
        else if (it->shortcut[0])
        {
            uint32_t scfg = it->enabled ? g_theme.menu_item_shortcut_fg : g_theme.menu_item_fg_disabled;
            char scbuf[32];
            const char* sc = display_shortcut(it->shortcut, scbuf, sizeof scbuf);
            int sc_len = (int)utf8_vis_len(sc);
            int sx = it->x + it->w - 1 - sc_len;
            draw_text(sx, it->y, sc, scfg, ibg);
        }
    }
}

static void render_open_dropdown(ui_screen* s)
{
    if (!s->open_menu)
        return;

    ui_node* menu = s->open_menu;
    int dx, dy, box_w, box_h;
    layout_dropdown(menu, &dx, &dy, &box_w, &box_h);
    render_menu_panel(s, menu, dx, dy, box_w, box_h);

    /* The open submenu, if any, as a second panel to the right (validated
     * against the current menu, mirroring the update-side guard). */
    if (s->open_submenu && is_direct_child(menu, s->open_submenu))
    {
        ui_node* sm = s->open_submenu;
        int sdx, sdy, sbw, sbh;
        layout_submenu(sm, dx + box_w, &sdx, &sdy, &sbw, &sbh);
        render_menu_panel(s, sm, sdx, sdy, sbw, sbh);
    }
}

/* Top-layer pass: a <select>'s open popup, the same overlay treatment as
 * render_open_dropdown() above (and, like it, styled with the menu item
 * colors - a combo box's list looks like a dropdown's, just positioned
 * under the control instead of under a menu header). */
static void render_open_select(ui_screen* s)
{
    if (!s->open_select)
        return;

    ui_node* select = s->open_select;
    int dx, dy, box_w, box_h;
    layout_select_popup(select, &dx, &dy, &box_w, &box_h);

    /* See render_button() for why this is four pieces. */
    draw_shadow(dx + box_w, dy, 1, 1, UI_BOX_SHADOW_SHRINK_H_BOTTOM);
    if (box_h > 1)
        draw_shadow(dx + box_w, dy + 1, 1, box_h - 1, 0);
    draw_shadow(dx + box_w, dy + box_h, 1, 1, UI_BOX_SHADOW_SHRINK_H);
    draw_shadow(dx + 1, dy + box_h, box_w - 1, 1, UI_BOX_SHADOW_SHRINK_H);

    draw_border(dx, dy, box_w, box_h, g_theme.menu_border_style,
                g_theme.menu_border_fg, g_theme.menu_border_bg);

    for (int i = 0; i < select->child_count; i++)
    {
        ui_node* it = select->children[i];
        int is_hot = (s->hot == it);
        uint32_t ifg = is_hot ? g_theme.menu_item_fg_hot : g_theme.menu_item_fg;
        uint32_t ibg = is_hot ? g_theme.menu_item_bg_hot : g_theme.menu_item_bg;
        draw_fill(it->x, it->y, it->w, 1, ifg, ibg);
        draw_text(it->x + 1, it->y, it->label, ifg, ibg);
    }
}

static void render_box(ui_node* n)
{
    int x = n->x, y = n->y, w = n->w, h = n->h;

    /* Interior uses the box's own color (set via ui_set_color) - the frame
     * itself always keeps the fixed Turbo Vision gray/black look. */
    if (w > 2 && h > 2)
        draw_fill(x + 1, y + 1, w - 2, h - 2, n->fg, n->bg);

    draw_border(x, y, w, h, g_theme.box_border_style,
                g_theme.box_fg, g_theme.box_bg);

    if (n->label[0])
    {
        int tx = x + (w - (int)strlen(n->label)) / 2;
        draw_text(tx, y, n->label, g_theme.box_fg, g_theme.box_bg);
    }
}

/* Forward declaration: render_window's children can themselves be any node
 * type (button, input, text, ...), so it dispatches through the same
 * function ui_screen_render's top-level loop uses. */
static void render_node(ui_screen* s, ui_node* n);

/* A modal dialog's content panel - same double-line bordered look as <box>,
 * plus a drop shadow (it floats above the whole document, unlike a box),
 * and it recursively renders its own children (button, text, input, ...). */
 /* Whether `win` is the one window that currently owns keyboard input - the
  * top of the modal stack while one's active (there's only ever one visible
  * modal, so it's always "focused"), otherwise the frontmost floating window.
  * Every other floating window behind it is unfocused. */
static int window_is_focused(ui_screen* s, ui_node* win)
{
    if (s->modal_stack_count > 0)
        return find_child_by_type(s->modal_stack[s->modal_stack_count - 1], UI_TAG_WINDOW) == win;
    if (s->window_count > 0)
        return find_child_by_type(s->windows[s->window_count - 1], UI_TAG_WINDOW) == win;
    return 0;
}

/* Whether `win` belongs to a true modal dialog (About, Find, Options, a
 * message box, ...) - i.e. its wrapper sits in s->modal_stack[] - as opposed
 * to a floating document window or docked panel (Output/Folder), which sits
 * in s->windows[] instead even though both use the same <window>/wrapper
 * tags. Modal dialogs draw with their own border colors/style (modal_border_*)
 * and are always in their single focused/active look, never dragged or
 * shown unfocused. */
static int window_is_true_modal(ui_screen* s, ui_node* win)
{
    for (int i = 0; i < s->modal_stack_count; i++)
        if (find_child_by_type(s->modal_stack[i], UI_TAG_WINDOW) == win)
            return 1;
    return 0;
}

static void render_window(ui_screen* s, ui_node* win)
{
    int x = win->x, y = win->y, w = win->w, h = win->h;
    int dragging = (s->dragging_window == win || s->resizing_window == win);
    /* Unfocused - not the window keyboard input currently goes to - draws
     * with a single-line border in a muted gray instead of the normal
     * double-line white one, like an inactive window in any other window
     * manager. Actively dragging/resizing a window always counts as
     * focused-style, even in the one-frame gap right as a click both picks
     * it and starts the drag. */
    int active_style = dragging || window_is_focused(s, win);

    /* A true modal dialog (as opposed to a floating document window or
     * docked panel that merely shares the same tags - see
     * window_is_true_modal) always draws in its own single style: it's
     * never dragged and there's only ever one on top, so it has no
     * dragging/unfocused/docked variants to pick between. */
    int is_modal = window_is_true_modal(s, win);
    uint32_t border_fg = is_modal ? g_theme.modal_border_fg :
        dragging ? g_theme.window_border_fg_dragging :
        active_style ? g_theme.window_border_fg :
        g_theme.window_border_fg_unfocused;
    uint32_t border_bg = is_modal ? g_theme.modal_border_bg :
        g_theme.window_border_bg;

    /* A docked panel (Output/Folder, via ui_set_dock) may want its own
     * border style, distinct from a floating document/dialog window's -
     * see window_border_style_docked*. */
    int is_docked = win->dock != UI_DOCK_NONE;
    enum ui_border_style border_style = is_modal ? g_theme.modal_border_style :
        is_docked ?
        (active_style ? g_theme.window_border_style_docked :
                         g_theme.window_border_style_docked_unfocused) :
        (active_style ? g_theme.window_border_style :
                         g_theme.window_border_style_unfocused);
    struct ui_border_glyphs glyphs = border_glyphs(border_style);
    uint32_t corner_tl = glyphs.tl, corner_tr = glyphs.tr;
    uint32_t corner_bl = glyphs.bl, corner_br = glyphs.br;
    uint32_t edge_h = glyphs.edge_h, edge_v = glyphs.edge_v;

    /* win->shadow (see ui_set_shadow) opts a window out of casting one at
     * all - off by default for a floating document/editor window, which
     * shouldn't look "raised" over the document the way a popped-up dialog
     * does. Also none while maximized regardless - it's edge-to-edge, there's
     * no floating look to cast one, and it'd fall partly off-screen anyway.
     * See render_button() for why this is otherwise four pieces. */
    if (win->shadow && !win->maximized)
    {
        draw_shadow(x + w, y, 1, 1, UI_BOX_SHADOW_SHRINK_H_BOTTOM);
        if (h > 1)
            draw_shadow(x + w, y + 1, 1, h - 1, 0);
        draw_shadow(x + w, y + h, 1, 1, UI_BOX_SHADOW_SHRINK_H);
        draw_shadow(x + 1, y + h, w - 1, 1, UI_BOX_SHADOW_SHRINK_H);
    }

    if (w > 2 && h > 2)
        draw_fill(x + 1, y + 1, w - 2, h - 2, win->fg, win->bg);

    emit_char(x, y, corner_tl, border_fg, border_bg);
    for (int i = 1; i < w - 1; i++)
        emit_char(x + i, y, edge_h, border_fg, border_bg);
    emit_char(x + w - 1, y, corner_tr, border_fg, border_bg);

    for (int i = 1; i < h - 1; i++)
    {
        emit_char(x, y + i, edge_v, border_fg, border_bg);
        emit_char(x + w - 1, y + i, edge_v, border_fg, border_bg);
    }

    emit_char(x, y + h - 1, corner_bl, border_fg, border_bg);
    for (int i = 1; i < w - 1; i++)
        emit_char(x + i, y + h - 1, edge_h, border_fg, border_bg);
    emit_char(x + w - 1, y + h - 1, corner_br, border_fg, border_bg);

    /* Caret line:col in the bottom-left border, for a source <editor> (skip
     * a VT100 one, e.g. the Output window's - it shows terminal/compiler
     * text, not a caret the user positions). */
    {
        char pos[32];
        if (window_caret_indicator(win, pos, sizeof pos))
            draw_text(x + 2, y + h - 1, pos, border_fg, border_bg);
    }

    if (win->label[0])
    {
        /* An unsaved <editor> child marks the title with a trailing "*",
         * the near-universal "this document has unsaved changes" cue. */
        ui_node* ed = find_child_by_type(win, UI_TAG_EDITOR);
        int dirty = ed && ed->dirty;

        /* The span the title is allowed to use: between the close icon (if
         * any) and the zoom icon (if any), each with a one-column gap so
         * the title never runs up against them - computed fresh here since
         * a title can be drawn even on a window too narrow for either icon
         * (window_has_close_icon/window_has_zoom_icon gate independently). */
        int lo = x + 1;
        if (window_has_close_icon(win))
            lo = x + 1 + UI_WINDOW_CLOSE_W + 1;
        int hi = x + w - 1;
        if (window_has_zoom_icon(win))
            hi = window_zoom_x(win) - 1;
        int max_cols = hi - lo;
        if (max_cols < 0)
            max_cols = 0;

        int label_cols = utf8_col_of(win->label, (int)strlen(win->label));
        int total_cols = label_cols + (dirty ? 1 : 0);

        if (max_cols > 0 && total_cols > max_cols)
        {
            /* Doesn't fit between the icons - truncate with "...", keeping
             * the dirty marker alongside it too if there's still room. */
            int dots = max_cols < 3 ? max_cols : 3;
            int keep_cols = max_cols - dots;
            int show_dirty = dirty && keep_cols > 0;
            if (show_dirty)
                keep_cols--;

            int tx = x + (w - (keep_cols + dots + (show_dirty ? 1 : 0))) / 2;
            const char* p = win->label;
            int col = 0, cx = tx;
            while (col < keep_cols && *p)
            {
                uint32_t cp;
                p += utf8_decode(p, &cp);
                emit_char(cx, y, cp, border_fg, border_bg);
                cx++;
                col++;
            }
            for (int i = 0; i < dots; i++, cx++)
                emit_char(cx, y, '.', border_fg, border_bg);
            if (show_dirty)
                emit_char(cx, y, '*', border_fg, border_bg);
        }
        else
        {
            int tx = x + (w - total_cols) / 2;
            draw_text(tx, y, win->label, border_fg, border_bg);
            if (dirty)
                emit_char(tx + label_cols, y, '*', border_fg, border_bg);
        }
    }

    if (window_has_close_icon(win))
    {
        emit_char(x + 1, y, '[', border_fg, border_bg);
        emit_char(x + 2, y, 0x25A0 /* black square - close */, g_theme.window_close_bg, border_bg);
        emit_char(x + 3, y, ']', border_fg, border_bg);
    }

    if (window_has_zoom_icon(win))
    {
        int zx = window_zoom_x(win);
        emit_char(zx, y, '[', border_fg, border_bg);
        emit_char(zx + 1, y, win->maximized ? 0x2193 /* down arrow - restore */ : 0x2191 /* up arrow - maximize */,
                  border_fg, border_bg);
        emit_char(zx + 2, y, ']', border_fg, border_bg);
    }

    for (int i = 0; i < win->child_count; i++)
        render_node(s, win->children[i]);
}

/* One row, scrolled so the caret is always visible - the offset is derived
 * fresh each frame from cursor/width rather than persisted, since it's a
 * pure function of the two. When focused, the cell under the caret paints
 * with fg/bg swapped as a block cursor - or, with an active selection, every
 * selected cell does instead and the caret itself is hidden, same convention
 * as <editor>. */
static void render_input(ui_screen* s, ui_node* n)
{
    int focused = (s->focused == n);
    int caret = focused && s->caret_visible;  /* blink phase - see ui_screen_update */
    uint32_t fg = focused ? g_theme.input_fg_focus : g_theme.input_fg;
    uint32_t bg = g_theme.input_bg;

    int cursor_col = utf8_col_of(n->label, n->cursor);
    int offset = cursor_col >= n->w ? cursor_col - n->w + 1 : 0;

    int has_sel = has_selection(n);
    int sel_lo_col = 0, sel_hi_col = 0;
    if (has_sel)
    {
        int lo, hi;
        selection_range(n, &lo, &hi);
        sel_lo_col = utf8_col_of(n->label, lo);
        sel_hi_col = utf8_col_of(n->label, hi);
    }

    draw_fill(n->x, n->y, n->w, 1, fg, bg);

    const char* p = n->label;
    int idx = 0, col = 0;
    while (*p && col < n->w)
    {
        uint32_t cp;
        int clen = utf8_decode(p, &cp);
        if (idx >= offset)
        {
            int selected = has_sel && idx >= sel_lo_col && idx < sel_hi_col;
            int is_caret = caret && !has_sel && idx == cursor_col;
            int inv = selected || is_caret;
            emit_char(n->x + col, n->y, cp,
                      inv ? g_theme.input_sel_fg : fg,
                      inv ? g_theme.input_sel_bg : bg);
            col++;
        }
        p += clen;
        idx++;
    }
    /* Caret sits past the last character (cursor at end of the value): the
     * loop above only paints existing chars, so draw the caret block blank. */
    if (caret && !has_sel && cursor_col >= idx && col < n->w)
        emit_char(n->x + col, n->y, ' ', g_theme.input_sel_fg, g_theme.input_sel_bg);
}

/* A <select>'s closed box: the chosen option's label (styled like an
 * <input>, since it's the same "one line of text" shape), plus a small
 * arrow button in the rightmost cell that opens the popup (see
 * render_open_select) - highlighted like a button while hot or open. */
static void render_select(ui_screen* s, ui_node* n)
{
    uint32_t fg = g_theme.input_fg, bg = g_theme.input_bg;
    int text_w = n->w > 1 ? n->w - 1 : n->w;

    draw_fill(n->x, n->y, text_w, 1, fg, bg);
    if (n->selected >= 0 && n->selected < n->child_count)
        draw_text(n->x, n->y, n->children[n->selected]->label, fg, bg);

    if (text_w < n->w)
    {
        int open = (s->open_select == n);
        uint32_t abg = (open || s->hot == n) ? g_theme.btn_bg_hot : g_theme.btn_bg;
        emit_char(n->x + text_w, n->y, 0x2193 /* down arrow */, g_theme.btn_fg, abg);
    }
}

/* An always-visible, scrollable list of <item> children (see ui.h) - each
 * one row, clipped/padded to the box's width. The row matching n->selected
 * is highlighted; there's no separate "focused vs not" look on top of that,
 * since (unlike <input>/<editor>) there's no caret/typing concept here. */
static void render_listbox(ui_screen* s, ui_node* n)
{
    for (int row = 0; row < n->h; row++)
    {
        int index = n->scroll + row;
        int is_sel = (index == n->selected);
        uint32_t fg = is_sel ? g_theme.listbox_sel_fg : g_theme.listbox_fg;
        uint32_t bg = is_sel ? g_theme.listbox_sel_bg : g_theme.listbox_bg;

        if (index < 0 || index >= n->child_count)
        {
            draw_fill(n->x, n->y + row, n->w, 1, g_theme.listbox_fg, g_theme.listbox_bg);
            continue;
        }

        const char* label = n->children[index]->label;
        const char* p = label;
        int col = 0;
        while (*p && col < n->w)
        {
            uint32_t cp;
            p += utf8_decode(p, &cp);
            emit_char(n->x + col, n->y + row, cp, fg, bg);
            col++;
        }
        for (; col < n->w; col++)
            emit_char(n->x + col, n->y + row, ' ', fg, bg);
    }

    /* The scrollbar overlay (see listbox_has_scrollbar) - painted over the
     * rightmost column just drawn above, only while it's actually relevant
     * to look at: the mouse is over this listbox, or its thumb is mid-drag
     * (which can carry the mouse off the box entirely - see process_window).
     * Anywhere else it stays fully out of the way, spending no width at
     * all - unlike a <window>'s scrollbar, this one never reserves a
     * permanent column. */
    if (listbox_has_scrollbar(n) && (s->hot == n || s->dragging_listbox_scrollbar == n))
    {
        int thumb_start, thumb_len;
        listbox_scrollbar_thumb(n, &thumb_start, &thumb_len);
        int sx = n->x + n->w - 1;
        for (int row = 0; row < n->h; row++)
        {
            int is_thumb = row >= thumb_start && row < thumb_start + thumb_len;
            emit_char(sx, n->y + row, ' ', g_theme.scrollbar_bg,
                      is_thumb ? g_theme.scrollbar_thumb_bg : g_theme.scrollbar_bg);
        }
    }
}

/* A GROUP: one <item> per row, each with a left marker for its state -
 * "( )"/"(*)" (a bullet) for a single-select radio cluster, or "[ ]"/"[X]"
 * for a multi-select check-box cluster (see ui_set_multi) - then the label
 * with its first letter accented as a hotkey cue. The keyboard-focused row is
 * drawn with the selection colors while the group holds focus. */
static void render_group(ui_screen* s, ui_node* n)
{
    int focused = (s->focused == n);
    int focus_row = n->multi ? n->cursor_row : n->selected;

    draw_fill(n->x, n->y, n->w, n->h, g_theme.listbox_fg, g_theme.listbox_bg);

    for (int row = 0; row < n->h && row < n->child_count; row++)
    {
        ui_node* item = n->children[row];
        int on = n->multi ? item->selected : (row == n->selected);
        int is_focus = focused && row == focus_row;
        uint32_t fg = is_focus ? g_theme.listbox_sel_fg : g_theme.listbox_fg;
        uint32_t bg = is_focus ? g_theme.listbox_sel_bg : g_theme.listbox_bg;
        int y = n->y + row;

        /* Marker cells: "(", state glyph, ")", gap. */
        uint32_t open = n->multi ? '[' : '(';
        uint32_t close = n->multi ? ']' : ')';
        uint32_t glyph = on ? (n->multi ? 'X' : 0x2022 /* bullet */) : ' ';
        uint32_t marker[4] = { open, glyph, close, ' ' };

        int col = 0;
        for (int i = 0; i < 4 && col < n->w; i++, col++)
            emit_char(n->x + col, y, marker[i], fg, bg);

        const char* p = item->label;
        int first = 1;
        while (*p && col < n->w)
        {
            uint32_t cp;
            p += utf8_decode(p, &cp);
            uint32_t cfg = (first && !is_focus) ? g_theme.hotkey_key_fg : fg;
            emit_char(n->x + col, y, cp, cfg, bg);
            first = 0;
            col++;
        }
    }
}

static int is_ident_char(uint32_t cp)
{
    return (cp >= 'a' && cp <= 'z') || (cp >= 'A' && cp <= 'Z') ||
        (cp >= '0' && cp <= '9') || cp == '_';
}
static int is_c_keyword1(const char* word, int len)
{
    char c = word[0];

    if (c == 'a')
    {
        if (len == 4 && memcmp(word, "auto", 4) == 0) return 1;
    }
    else if (c == 'b')
    {
        if (len == 5 && memcmp(word, "break", 5) == 0) return 1;
        if (len == 4 && memcmp(word, "bool", 4) == 0) return 1;   // C23
    }
    else if (c == 'c')
    {
        if (len == 5 && memcmp(word, "const", 5) == 0) return 1;
        if (len == 4 && memcmp(word, "char", 4) == 0) return 1;
        if (len == 4 && memcmp(word, "case", 4) == 0) return 1;
        if (len == 5 && memcmp(word, "catch", 5) == 0) return 1;  // C++
        if (len == 8 && memcmp(word, "continue", 8) == 0) return 1;
        if (len == 14 && memcmp(word, "compile_assert", 14) == 0) return 1;
        if (len == 9 && memcmp(word, "constexpr", 9) == 0) return 1;
    }
    else if (c == 'd')
    {
        if (len == 6 && memcmp(word, "double", 6) == 0) return 1;
        if (len == 2 && memcmp(word, "do", 2) == 0) return 1;
        if (len == 7 && memcmp(word, "default", 7) == 0) return 1;
    }
    else if (c == 'e')
    {
        if (len == 4 && memcmp(word, "enum", 4) == 0) return 1;
        if (len == 6 && memcmp(word, "extern", 6) == 0) return 1;
        if (len == 4 && memcmp(word, "else", 4) == 0) return 1;
    }
    else if (c == 'f')
    {
        if (len == 5 && memcmp(word, "float", 5) == 0) return 1;
        if (len == 3 && memcmp(word, "for", 3) == 0) return 1;
        if (len == 4 && memcmp(word, "false", 4) == 0) return 1;  // C23
    }
    else if (c == 'g')
    {
        if (len == 4 && memcmp(word, "goto", 4) == 0) return 1;
    }
    else if (c == 'i')
    {
        if (len == 3 && memcmp(word, "int", 3) == 0) return 1;
        if (len == 2 && memcmp(word, "if", 2) == 0) return 1;
        if (len == 6 && memcmp(word, "inline", 6) == 0) return 1;   // C99
    }
    else if (c == 'l')
    {
        if (len == 4 && memcmp(word, "long", 4) == 0) return 1;
    }
    else if (c == 'n')
    {
        if (len == 7 && memcmp(word, "nullptr", 7) == 0) return 1;
    }
    else if (c == 'r')
    {
        if (len == 8 && memcmp(word, "register", 8) == 0) return 1;
        if (len == 6 && memcmp(word, "return", 6) == 0) return 1;
        if (len == 8 && memcmp(word, "restrict", 8) == 0) return 1; // C99
    }
    else if (c == 's')
    {
        if (len == 6 && memcmp(word, "signed", 6) == 0) return 1;
        if (len == 6 && memcmp(word, "sizeof", 6) == 0) return 1;
        if (len == 6 && memcmp(word, "struct", 6) == 0) return 1;
        if (len == 5 && memcmp(word, "short", 5) == 0) return 1;
        if (len == 6 && memcmp(word, "static", 6) == 0) return 1;
        if (len == 13 && memcmp(word, "static_assert", 13) == 0) return 1; // C++ / C23
        if (len == 6 && memcmp(word, "switch", 6) == 0) return 1;
        if (len == 12 && memcmp(word, "static_debug", 12) == 0) return 1; // C++ / C23

    }
    else if (c == 't')
    {
        if (len == 7 && memcmp(word, "typedef", 7) == 0) return 1;
        if (len == 6 && memcmp(word, "typeof", 6) == 0) return 1;      // GNU
        if (len == 3 && memcmp(word, "try", 3) == 0) return 1;         // C++
        if (len == 4 && memcmp(word, "true", 4) == 0) return 1;        // C23
        if (len == 5 && memcmp(word, "throw", 5) == 0) return 1;       // C++
        if (len == 13 && memcmp(word, "typeof_unqual", 13) == 0) return 1; // GNU
    }
    else if (c == 'u')
    {
        if (len == 5 && memcmp(word, "union", 5) == 0) return 1;
        if (len == 8 && memcmp(word, "unsigned", 8) == 0) return 1;
    }
    else if (c == 'v')
    {
        if (len == 4 && memcmp(word, "void", 4) == 0) return 1;
        if (len == 8 && memcmp(word, "volatile", 8) == 0) return 1;
    }
    else if (c == 'w')
    {
        if (len == 5 && memcmp(word, "while", 5) == 0) return 1;
    }
    else if (c == '_')
    {
        // Standard C underscored keywords (C99, C11, C23)
        if (len == 5 && word[1] == 'B' && memcmp(word, "_Bool", 5) == 0) return 1;
        if (len == 7 && word[1] == 'B' && memcmp(word, "_BitInt", 7) == 0) return 1;  // C23
        if (len == 7 && word[1] == 'A' && memcmp(word, "_Atomic", 7) == 0) return 1;
        if (len == 8 && word[1] == 'A')
        {
            if (memcmp(word, "_Alignas", 8) == 0) return 1;
            if (memcmp(word, "_Alignof", 8) == 0) return 1;
        }
        if (len == 8 && word[1] == 'C')
        {
            if (memcmp(word, "_Complex", 8) == 0) return 1;
            if (memcmp(word, "_Countof", 8) == 0) return 1;   // <-- added (non‑standard, but requested)
        }
        if (len == 8 && word[1] == 'G' && memcmp(word, "_Generic", 8) == 0) return 1; // C11
        if (len == 6 && word[1] == 'D' && memcmp(word, "_Defer", 6) == 0) return 1;   // <-- added (proposed/extension)
        if (len == 9 && word[1] == 'N' && memcmp(word, "_Noreturn", 9) == 0) return 1;
        if (len == 10 && word[1] == 'I' && memcmp(word, "_Imaginary", 10) == 0) return 1;
        if (len == 13 && word[1] == 'T' && memcmp(word, "_Thread_local", 13) == 0) return 1;
        if (len == 14 && word[1] == 'S' && memcmp(word, "_Static_assert", 14) == 0) return 1;
    }
    return 0;
}

/* Group 2: control-flow keywords (editor_keyword2_fg). */
static int is_c_keyword2(const char* word, int len)
{
    if (len == 6 && memcmp(word, "_Owner", 6) == 0) return 1;
    if (len == 4 && memcmp(word, "_Opt", 4) == 0) return 1;
    if (len == 5 && memcmp(word, "_Dtor", 5) == 0) return 1;
    if (len == 5 && memcmp(word, "_Ctor", 5) == 0) return 1;
    if (len == 6 && memcmp(word, "assert", 6) == 0) return 1;
    if (len == 4 && memcmp(word, "NULL", 4) == 0) return 1;
    if (len == 6 && memcmp(word, "_Clear", 6) == 0) return 1;

    return 0;
}

/* True for exactly "struct", "union", or "enum" - the three keywords that
 * introduce a tag name (e.g. the X in "struct X"). Used by
 * render_editor_line to color that following identifier via
 * g_theme.editor_tag_fg instead of the ordinary identifier color. */
static int is_c_tag_keyword(const char* word, int len)
{
    return (len == 6 && memcmp(word, "struct", 6) == 0) ||
        (len == 5 && memcmp(word, "union", 5) == 0) ||
        (len == 4 && memcmp(word, "enum", 4) == 0);
}

/* The classic 16-color EGA/CGA palette VT100 SGR codes 30-37/90-97 index
 * into - built from the same 0x00/0x55/0xAA/0xFF channel steps already used
 * throughout g_theme, so colored compiler output reads as part of the same
 * retro palette rather than a jarring true-color mismatch. */
static const uint32_t ansi_palette[16] = {
    TB_RGB(0x00, 0x00, 0x00), TB_RGB(0xAA, 0x00, 0x00),
    TB_RGB(0x00, 0xAA, 0x00), TB_RGB(0xAA, 0xAA, 0x00),
    TB_RGB(0x00, 0x00, 0xAA), TB_RGB(0xAA, 0x00, 0xAA),
    TB_RGB(0x00, 0xAA, 0xAA), TB_RGB(0xAA, 0xAA, 0xAA),
    TB_RGB(0x55, 0x55, 0x55), TB_RGB(0xFF, 0x55, 0x55),
    TB_RGB(0x55, 0xFF, 0x55), TB_RGB(0xFF, 0xFF, 0x55),
    TB_RGB(0x55, 0x55, 0xFF), TB_RGB(0xFF, 0x55, 0xFF),
    TB_RGB(0x55, 0xFF, 0xFF), TB_RGB(0xFF, 0xFF, 0xFF),
};

/* Parses one CSI escape at s[0]=='\x1b', s[1]=='[' and, if it's an SGR
 * ("...m") sequence, updates *fg/*bold from its ';'-separated parameter
 * list. Background codes (40-47/100-107) and other SGR attributes
 * (underline, italic, etc.) are intentionally not tracked - editor_bg stays
 * the widget's fixed panel color, same as every other line. Returns the
 * sequence's total byte length so the caller can skip it as zero columns,
 * or 0 if s doesn't start a CSI sequence at all (caller then treats '\x1b'
 * as an ordinary, if unprintable, byte). A non-SGR CSI (rare in compiler
 * output) is still consumed and skipped, just without updating color. */
static int ansi_parse_sgr(const char* s, int len, uint32_t* fg, int* bold)
{
    if (len < 2 || s[0] != '\x1b' || s[1] != '[')
        return 0;

    int i = 2;
    while (i < len && s[i] != 'm' && (s[i] < 0x40 || s[i] > 0x7E))
        i++;
    if (i >= len)
        return 0;            /* truncated sequence - leave it for now */
    if (s[i] != 'm')
        return i + 1;         /* non-SGR CSI - skip without recoloring */

    int end = i, p = 2;
    while (p <= end)
    {
        int val = 0, had_digit = 0;
        while (p < end && s[p] >= '0' && s[p] <= '9')
        {
            val = val * 10 + (s[p] - '0');
            had_digit = 1;
            p++;
        }
        if (!had_digit)
            val = 0;          /* "\x1b[m" / a bare ';' == reset */

        if (val == 0)
        {
            *fg = g_theme.editor_fg;
            *bold = 0;
        }
        else if (val == 1)
        {
            *bold = 1;
        }
        else if (val == 22)
        {
            *bold = 0;
        }
        else if (val >= 30 && val <= 37)
        {
            *fg = ansi_palette[(val - 30) + (*bold ? 8 : 0)];
        }
        else if (val == 39)
        {
            *fg = g_theme.editor_fg;
        }
        else if (val >= 90 && val <= 97)
        {
            *fg = ansi_palette[(val - 90) + 8];
        }

        if (p < end && s[p] == ';')
            p++;
        else
            break;
    }
    return i + 1;              /* consume the whole "\x1b[...m" */
}

/* UI_SYNTAX_VT100 rendering path (see ui_set_syntax): same column/selection
 * mechanics as render_editor_line below, but interprets embedded VT100 SGR
 * color codes - the raw bytes Cake's diagnostics print - instead of
 * guessing at C syntax, which compiler output isn't. Escape sequences are
 * zero-width: consumed for their color, never drawn, never counted as a
 * column. */
 /* Draw one logical column `col` of a horizontally-scrolled line whose left
  * edge is at screen `x`: shift it left by `scroll_x` and draw only when it
  * falls inside the viewport [0, w). Columns scrolled off the left, or past
  * the right edge, are simply skipped. Selection/color decisions stay in
  * absolute (unscrolled) column space, so callers pass `col` unchanged. */
static void emit_hscroll(int x, int y, int col, int scroll_x, int w,
                         uint32_t cp, uint32_t fg, uint32_t bg)
{
    int vc = col - scroll_x;
    if (vc >= 0 && vc < w)
        emit_char(x + vc, y, cp, fg, bg);
}

static void render_editor_line_ansi(int x, int y, int w, int scroll_x,
                                     const char* line, int line_len, int sel_col_lo, int sel_col_hi,
                                     uint32_t bg)
{
    int col = 0, i = 0;
    uint32_t fg = g_theme.editor_fg;
    int bold = 0;

    while (i < line_len && col < scroll_x + w)
    {
        if (line[i] == '\x1b')
        {
            int consumed = ansi_parse_sgr(line + i, line_len - i, &fg, &bold);
            if (consumed > 0)
            {
                i += consumed;
                continue;
            }
        }

        uint32_t cp;
        int clen = utf8_decode(line + i, &cp);
        int selected = col >= sel_col_lo && col < sel_col_hi;
        emit_hscroll(x, y, col, scroll_x, w, cp, fg, selected ? g_theme.editor_sel_bg : bg);
        col++;
        i += clen;
    }
    for (; col < scroll_x + w; col++)
    {
        int selected = col >= sel_col_lo && col < sel_col_hi;
        emit_hscroll(x, y, col, scroll_x, w, ' ', g_theme.editor_fg, selected ? g_theme.editor_sel_bg : bg);
    }
}

/* One line of an <editor>'s value, with rough C syntax highlighting: a
 * leading '#' (preprocessor) colors the whole line; "strings" and //
 * comments run to the next quote/end-of-line; identifiers get looked up
 * against a keyword list. Deliberately simple - no block comments, no
 * escape sequences in strings - a best-effort visual aid, not a real
 * lexer. */
 /* Advance the /* ... *\/ block-comment state AND ( [ { nesting depth across
  * one line without drawing, respecting strings and // line comments (so a
  * "/*" - or a bracket - inside a string or after "//" isn't mistaken for
  * the real thing). `in_block` is whether the line begins inside a block
  * comment; the return value is whether it ends inside one. `depth` (in/out,
  * NULL if the caller doesn't need it) is the ( [ { nesting depth, carried
  * the same way across lines - used to prime both render_editor_line()'s
  * comment coloring and its rainbow bracket coloring at the first visible
  * row after a scroll. */
static int scan_line_block_state(const char* line, int len, int in_block, int* depth)
{
    int in_string = 0;
    for (int i = 0; i < len; )
    {
        if (in_block)
        {
            if (line[i] == '*' && i + 1 < len && line[i + 1] == '/') { in_block = 0; i += 2; }
            else i++;
        }
        else if (in_string)
        {
            if (line[i] == '\\' && i + 1 < len) i += 2;   /* escape */
            else { if (line[i] == '"') in_string = 0; i++; }
        }
        else if (line[i] == '"')
        {
            in_string = 1; i++;
        }
        else if (line[i] == '/' && i + 1 < len && line[i + 1] == '/')
        {
            break;  /* line comment - nothing after it can start a block */
        }
        else if (line[i] == '/' && i + 1 < len && line[i + 1] == '*')
        {
            in_block = 1; i += 2;
        }
        else if (depth && (line[i] == '(' || line[i] == '[' || line[i] == '{'))
        {
            (*depth)++;
            i++;
        }
        else if (depth && (line[i] == ')' || line[i] == ']' || line[i] == '}'))
        {
            if (*depth > 0)
                (*depth)--;
            i++;
        }
        else
        {
            i++;
        }
    }
    return in_block;  /* strings don't carry across lines; block comments (and depth) do */
}

/* Markdown counterpart of scan_line_block_state() above: advances the fenced
 * code block ("```") state across one line without drawing. `in_block` is
 * whether the line begins inside a fenced block; the return value is
 * whether it ends inside one (a "```" line itself toggles it). */
static int scan_markdown_fence_state(const char* line, int len, int in_block)
{
    int j = 0;
    while (j < len && (line[j] == ' ' || line[j] == '\t'))
        j++;
    if (len - j >= 3 && line[j] == '`' && line[j + 1] == '`' && line[j + 2] == '`')
        in_block = !in_block;
    return in_block;
}

/* Dispatches to whichever of the two scanners above matches `syntax` - a
 * no-op (state passes through unchanged) for UI_SYNTAX_NONE/VT100, which
 * have no multi-line state to track. Used to prime render_editor()'s
 * per-visible-row highlighter at the first row after a scroll. `depth` is
 * only meaningful (and only touched) for UI_SYNTAX_C - pass NULL for any
 * other syntax. */
static int scan_multiline_state(int syntax, const char* line, int len, int in_block, int* depth)
{
    if (syntax == UI_SYNTAX_C)
        return scan_line_block_state(line, len, in_block, depth);
    if (syntax == UI_SYNTAX_MARKDOWN)
        return scan_markdown_fence_state(line, len, in_block);
    return in_block;
}

/* The fence-line half of Markdown's embedded-```c-block tracking: given a
 * line already confirmed to be a "```"/"```lang" delimiter, updates in_c_
 * block (whether the block it opens or closes is tagged "c") and, on open,
 * resets that block's own C block-comment continuation and bracket-nesting
 * depth (c_comment_block/c_bracket_depth - completely separate from the
 * `in_block`/`bracket_depth` a caller may also be tracking, which are about
 * "inside *some* fence" and UI_SYNTAX_C's own top-level state respectively,
 * not this). `in_block_before` is `in_block`'s value *before* this same
 * fence line's own scan_markdown_fence_state() toggle - a fence's language
 * tag only matters at the instant its block opens, i.e. whether the fence
 * was seen while NOT already inside one. See render_editor()'s row loop for
 * where in_c_block decides whether a content line gets Markdown's flat
 * md_code_fg or the real C highlighter (render_editor_line()). */
static void md_toggle_c_block_on_fence(const char* line, int line_len, int in_block_before,
                                        int* in_c_block, int* c_comment_block, int* c_bracket_depth)
{
    if (!in_block_before)
    {
        *in_c_block = md_fence_lang_is_c(line, line_len);
        *c_comment_block = 0;
        *c_bracket_depth = 0;
    }
    else
    {
        *in_c_block = 0;
    }
}

/* Full per-line advance built on md_toggle_c_block_on_fence() above: also
 * handles a non-fence line, running it through scan_line_block_state() to
 * carry the embedded block's own C state forward when it's content inside
 * one (in_c_block already true). For a line that's never actually drawn
 * (the pre-scroll priming walk, the fence/comment-only skip loop) this is
 * the whole story; render_editor()'s row loop instead lets render_editor_
 * line() do this same advancing itself as a side effect of drawing a
 * VISIBLE ```c content line - see the dispatch switch - and only calls
 * md_toggle_c_block_on_fence() directly (not this) for a fence line, or
 * this line's md_is_fence_line() check plus a direct scan_line_block_state()
 * call for a content line scrolled off the top of the screen (sy < 0,
 * never drawn either), to avoid scanning the same line's C state twice. */
static void md_advance_c_block_state(const char* line, int line_len, int in_block_before,
                                      int* in_c_block, int* c_comment_block, int* c_bracket_depth)
{
    if (md_is_fence_line(line, line_len))
        md_toggle_c_block_on_fence(line, line_len, in_block_before,
                                    in_c_block, c_comment_block, c_bracket_depth);
    else if (*in_c_block)
        *c_comment_block = scan_line_block_state(line, line_len, *c_comment_block, c_bracket_depth);
}

/* Draw one source line. `in_block` (in/out) carries /* ... *\/ block-comment
 * state across lines: on entry, whether this line starts inside a block
 * comment; on return, whether it ends inside one. `depth` (in/out) is the
 * ( [ { nesting depth, carried the same way - each pair is colored from
 * g_theme.editor_bracket_fg[depth % UI_EDITOR_BRACKET_COLORS], cycling with
 * nesting the way "rainbow bracket" highlighting does in other editors, so
 * a closing bracket always matches its opener's color regardless of how
 * many other pairs sit between them. Brackets inside a string/comment don't
 * count or get colored - same string/comment tracking used for everything
 * else in this function. */
static void render_editor_line(int x, int y, int w, int scroll_x,
                                const char* line, int line_len, int sel_col_lo, int sel_col_hi,
                                int* in_block, uint32_t bg, int* depth)
{
    int col = 0, i = 0;
    int in_string = 0, in_comment = 0;

    /* Set right after a bare "struct"/"union"/"enum" keyword, cleared by
     * anything other than the whitespace between it and its tag name (see
     * the check just inside the loop below) - so the very next identifier
     * is coloured via editor_tag_fg instead of the ordinary identifier
     * color (see the is_ident_char branch below), and nothing further down
     * the line accidentally picks it up (e.g. "struct{ int a; Foo b; }" -
     * hitting '{' clears it before "Foo" is ever reached). Deliberately not
     * carried across lines (in/out via a pointer, the way in_block/depth
     * are) - "struct\nFoo" with the tag on its own line is valid C but
     * exotic enough that this simpler, line-local version isn't worth the
     * extra state for. */
    int pending_tag = 0;

    int j = 0;
    while (j < line_len && (line[j] == ' ' || line[j] == '\t'))
        j++;
    int is_preproc = !*in_block && (j < line_len && line[j] == '#');

    /* Scan the WHOLE line, not just the visible [scroll_x, scroll_x + w)
     * slice - col starts at 0 regardless of scroll_x, so bounding this loop
     * by the viewport width would stop scanning the instant a line is wider
     * than the editor, even with no horizontal scroll at all. That used to
     * cut the scan off before reaching a comment's closing "*\/" (or a
     * bracket) that happens to sit past that column, leaving *in_block/
     * *depth wrong for every line below - visibly, a long enough /* comment
     * would "leak" comment coloring onto the rest of the file. Safe to
     * always scan the full line: emit_hscroll() below already no-ops any
     * draw whose column falls outside the viewport, so there's no visible
     * cost, only the CPU to classify the off-screen characters - same
     * tradeoff already made for rows scrolled above the viewport (see
     * scan_multiline_state() in render_editor()). */
    while (i < line_len)
    {
        uint32_t cp;
        int clen = utf8_decode(line + i, &cp);
        uint32_t fg = g_theme.editor_fg;

        if (!*in_block && !is_preproc && !in_comment && !in_string &&
            cp != ' ' && cp != '\t' &&
            !(is_ident_char(cp) && !(cp >= '0' && cp <= '9')))
        {
            pending_tag = 0;  /* anything but whitespace/an identifier start
                               * between "struct" and here means there's no
                               * tag name coming (e.g. "struct{", "struct*") */
        }

        if (*in_block)
        {
            /* Inside a block comment: color as comment, watch for the "*\/"
             * that ends it (drawing both of its cells before clearing). */
            uint32_t cfg = g_theme.editor_comment_fg;
            if (cp == '*' && i + 1 < line_len && line[i + 1] == '/')
            {
                for (int k = 0; k < 2 && col < scroll_x + w; k++)
                {
                    int selected = col >= sel_col_lo && col < sel_col_hi;
                    emit_hscroll(x, y, col, scroll_x, w, k == 0 ? '*' : '/', cfg,
                              selected ? g_theme.editor_sel_bg : bg);
                    col++;
                }
                i += 2;
                *in_block = 0;
                continue;
            }
            fg = cfg;  /* fall through to draw this single comment char */
        }
        else if (is_preproc)
        {
            fg = g_theme.editor_preproc_fg;
        }
        else if (in_comment)
        {
            fg = g_theme.editor_comment_fg;
        }
        else if (in_string)
        {
            fg = g_theme.editor_string_fg;
            if (cp == '\\' && i + 1 < line_len)
            {
                /* An escape - draw both this backslash and the char it
                 * escapes right now, in string color, without checking the
                 * escaped char for a closing quote (so '"a\"b"' doesn't
                 * treat the escaped quote as the string's end - matches
                 * scan_line_block_state's own escape handling, which primes
                 * this same in_string tracking for the next line). */
                uint32_t ecp;
                int elen = utf8_decode(line + i + clen, &ecp);
                int selected = col >= sel_col_lo && col < sel_col_hi;
                emit_hscroll(x, y, col, scroll_x, w, cp, fg,
                          selected ? g_theme.editor_sel_bg : bg);
                col++;
                selected = col >= sel_col_lo && col < sel_col_hi;
                emit_hscroll(x, y, col, scroll_x, w, ecp, fg,
                          selected ? g_theme.editor_sel_bg : bg);
                col++;
                i += clen + elen;
                continue;
            }
            if (cp == '"')
                in_string = 0;
        }
        else if (cp == '"')
        {
            in_string = 1;
            fg = g_theme.editor_string_fg;
        }
        else if (cp == '\'')
        {
            /* A char literal ('x', '\n', '\'', '\0', even a raw multi-byte
             * UTF-8 char like 'e'-with-accent) is short, so rather than
             * carry an in_char state that could run off and swallow the
             * rest of the line on a stray apostrophe, look ahead a short,
             * bounded distance for the matching close quote (honoring a
             * backslash escape so "'\''" doesn't close early on its middle
             * quote). Only color it as a unit if a close is actually found
             * nearby; otherwise fall through and draw this "'" as a single
             * plain-colored character, same as before. */
            int k = i + 1;
            int limit = i + 12 < line_len ? i + 12 : line_len;
            int closed = 0;
            while (k < limit)
            {
                if (line[k] == '\\' && k + 1 < line_len)
                {
                    k += 2;
                    continue;
                }
                if (line[k] == '\'')
                {
                    closed = 1;
                    k++;
                    break;
                }
                k++;
            }
            if (closed)
            {
                /* Draw only the visible slice, but always advance `i` all
                 * the way to `k` regardless - same reasoning as the
                 * identifier/number branches below: if the draw loop below
                 * stops early because col has scrolled past the viewport,
                 * `i` must still land exactly where the literal actually
                 * ends, not wherever drawing happened to give up, or the
                 * outer loop would resume mid-literal and could misread the
                 * remaining "'" as a new one. */
                int p = i;
                while (p < k && col < scroll_x + w)
                {
                    uint32_t wc;
                    int wl = utf8_decode(line + p, &wc);
                    int selected = col >= sel_col_lo && col < sel_col_hi;
                    emit_hscroll(x, y, col, scroll_x, w, wc, g_theme.editor_char_fg,
                              selected ? g_theme.editor_sel_bg : bg);
                    col++;
                    p += wl;
                }
                i = k;
                continue;
            }
        }
        else if (cp == '/' && i + 1 < line_len && line[i + 1] == '/')
        {
            in_comment = 1;
            fg = g_theme.editor_comment_fg;
        }
        else if (cp == '/' && i + 1 < line_len && line[i + 1] == '*')
        {
            /* Start of a block comment - draw "/*" and enter block mode. */
            uint32_t cfg = g_theme.editor_comment_fg;
            for (int k = 0; k < 2 && col < scroll_x + w; k++)
            {
                int selected = col >= sel_col_lo && col < sel_col_hi;
                emit_hscroll(x, y, col, scroll_x, w, k == 0 ? '/' : '*', cfg,
                          selected ? g_theme.editor_sel_bg : bg);
                col++;
            }
            i += 2;
            *in_block = 1;
            continue;
        }
        else if (is_ident_char(cp) && !(cp >= '0' && cp <= '9'))
        {
            int wstart = i;
            while (i < line_len)
            {
                uint32_t wc;
                int wl = utf8_decode(line + i, &wc);
                if (!is_ident_char(wc))
                    break;
                i += wl;
            }
            uint32_t wfg = g_theme.editor_fg;
            /* is_c_keyword1 already matches struct/union/enum themselves
             * (they're type/storage keywords) - is_c_tag_keyword below is
             * the narrower "was it exactly one of those three" check that
             * decides whether the NEXT word is a tag name. */
            if (pending_tag)
                wfg = g_theme.editor_tag_fg;  /* the name right after struct/union/enum */
            else if (is_c_keyword1(line + wstart, i - wstart))
                wfg = g_theme.editor_keyword_fg;
            else if (is_c_keyword2(line + wstart, i - wstart))
                wfg = g_theme.editor_keyword2_fg;
            pending_tag = is_c_tag_keyword(line + wstart, i - wstart);
            int k = wstart;
            while (k < i && col < scroll_x + w)
            {
                uint32_t wc;
                int wl = utf8_decode(line + k, &wc);
                int selected = col >= sel_col_lo && col < sel_col_hi;
                /* Selection only swaps the background - the syntax color
                 * (wfg here) stays exactly as it'd render unselected. */
                emit_hscroll(x, y, col, scroll_x, w, wc, wfg,
                          selected ? g_theme.editor_sel_bg : bg);
                col++;
                k += wl;
            }
            continue;  /* i/col already advanced past the whole word */
        }
        else if (cp >= '0' && cp <= '9')
        {
            /* A numeric literal - decimal, hex (0x...), float, with any
             * u/l/f suffix or e/p exponent. Not a real lexer (no validation
             * that e.g. the hex letters only appear after "0x"): just greedy
             * consumption of digits/letters/'.' that plausibly belong to a
             * number, plus a '+'/'-' immediately after an exponent marker
             * (e/E/p/P) so "1e-10" colors as one token instead of splitting
             * on the minus. A leading digit can't be reached from inside an
             * identifier (the identifier branch above already consumes
             * "var1"-style trailing digits itself), so this only ever fires
             * at the true start of a number. */
            int wstart = i;
            i += clen;
            while (i < line_len)
            {
                uint32_t wc;
                int wl = utf8_decode(line + i, &wc);
                int is_num_char = (wc >= '0' && wc <= '9') || wc == '.' ||
                    (wc >= 'a' && wc <= 'f') || (wc >= 'A' && wc <= 'F') ||
                    wc == 'x' || wc == 'X' ||
                    wc == 'u' || wc == 'U' || wc == 'l' || wc == 'L';
                if (!is_num_char && (wc == '+' || wc == '-'))
                {
                    char prevc = line[i - 1];
                    is_num_char = prevc == 'e' || prevc == 'E' ||
                                  prevc == 'p' || prevc == 'P';
                }
                if (!is_num_char)
                    break;
                i += wl;
            }
            while (wstart < i && col < scroll_x + w)
            {
                uint32_t wc;
                int wl = utf8_decode(line + wstart, &wc);
                int selected = col >= sel_col_lo && col < sel_col_hi;
                emit_hscroll(x, y, col, scroll_x, w, wc, g_theme.editor_number_fg,
                          selected ? g_theme.editor_sel_bg : bg);
                col++;
                wstart += wl;
            }
            continue;  /* i/col already advanced past the whole number */
        }
        else if (cp == '(' || cp == '[' || cp == '{')
        {
            /* Opener takes the color for the level it's opening, then
             * descends into it. */
            fg = g_theme.editor_bracket_fg[*depth % UI_EDITOR_BRACKET_COLORS];
            (*depth)++;
        }
        else if (cp == ')' || cp == ']' || cp == '}')
        {
            /* Closer ascends back out FIRST, then takes the color of the
             * level it lands on - the same level its matching opener used,
             * so a pair always shares one color regardless of what's
             * nested between them. Clamped at 0 so unbalanced/partial code
             * (mid-edit, or just wrong) can't wrap depth negative. */
            if (*depth > 0)
                (*depth)--;
            fg = g_theme.editor_bracket_fg[*depth % UI_EDITOR_BRACKET_COLORS];
        }

        int selected = col >= sel_col_lo && col < sel_col_hi;
        emit_hscroll(x, y, col, scroll_x, w, cp, fg,
                  selected ? g_theme.editor_sel_bg : bg);
        col++;
        i += clen;
    }
    for (; col < scroll_x + w; col++)
    {
        int selected = col >= sel_col_lo && col < sel_col_hi;
        emit_hscroll(x, y, col, scroll_x, w, ' ', g_theme.editor_fg, selected ? g_theme.editor_sel_bg : bg);
    }
}

/* UI_SYNTAX_NONE: the fallback - draws the line in the plain editor_fg,
 * no coloring at all (e.g. a ".txt" file). Same column/selection mechanics
 * as render_editor_line above, just without any of the syntax analysis. */
static void render_editor_line_plain(int x, int y, int w, int scroll_x,
                                      const char* line, int line_len, int sel_col_lo, int sel_col_hi,
                                      uint32_t bg)
{
    int col = 0, i = 0;
    while (i < line_len && col < scroll_x + w)
    {
        uint32_t cp;
        int clen = utf8_decode(line + i, &cp);
        int selected = col >= sel_col_lo && col < sel_col_hi;
        emit_hscroll(x, y, col, scroll_x, w, cp, g_theme.editor_fg,
                  selected ? g_theme.editor_sel_bg : bg);
        col++;
        i += clen;
    }
    for (; col < scroll_x + w; col++)
    {
        int selected = col >= sel_col_lo && col < sel_col_hi;
        emit_hscroll(x, y, col, scroll_x, w, ' ', g_theme.editor_fg, selected ? g_theme.editor_sel_bg : bg);
    }
}

/* UI_SYNTAX_MARKDOWN: a lightweight (not-a-real-parser) highlighter - ATX
 * headings ("# ..." through "###### ...") and blockquotes ("> ...") color
 * their whole line, fenced code blocks ("```" ... "```", tracked line-to-line
 * via `in_block` the same way render_editor_line's block comments are) color
 * every line between the fences (including the fences themselves), and
 * inline `code span`s, **bold** spans, and [link text](dest) color just
 * their own bit - using the theme's dedicated md_heading_fg/md_blockquote_fg/md_code_fg/
 * md_bold_fg/md_link_fg, not the UI_SYNTAX_C colors, so a theme can restyle
 * Markdown independently of C source.
 *
 * That coloring is the same whether editing or previewing - only whether the
 * delimiter punctuation itself (the "```"/"`"/"**"/"<!-- -->"/"[]()" bytes)
 * is actually drawn differs: editing shows the real source, delimiters
 * included (so what you type is what you see); read-only mode removes them
 * in a presentation-style pass (not just blanked out - see the
 * md_scan_line() block comment above) so the line reads like a rendered
 * preview instead of source-with-holes-in-it. The caller keeps cursor/
 * selection/scrollbar math agreeing with that read-only collapse via
 * md_scan_line() rather than a raw column count. */
static void render_editor_line_markdown(int x, int y, int w, int scroll_x,
                                         const char* line, int line_len, int sel_col_lo, int sel_col_hi,
                                         int* in_block, int read_only, uint32_t bg)
{
    int j = 0;
    while (j < line_len && (line[j] == ' ' || line[j] == '\t'))
        j++;
    int is_fence = (line_len - j >= 3 && line[j] == '`' && line[j + 1] == '`' && line[j + 2] == '`');
    if (is_fence)
        *in_block = !*in_block;
    int in_code_block = *in_block || is_fence;

    int heading = 0;
    if (!in_code_block)
    {
        while (heading < line_len && heading < 6 && line[heading] == '#')
            heading++;
        if (heading == 0 || heading >= line_len || line[heading] != ' ')
            heading = 0;
    }
    int blockquote = !in_code_block && !heading && j < line_len && line[j] == '>';

    int col = 0, i = 0, in_span = 0, in_link_text = 0, in_link_dest = 0, in_html_comment = 0;
    int fence_prefix_len = j + 3;
    int in_bold = 0;
    while (i < line_len && col < scroll_x + w)
    {
        uint32_t cp;
        int clen = utf8_decode(line + i, &cp);
        uint32_t fg;
        /* is_delim: this codepoint (plus any skip_extra bytes right after
         * it) is Markdown punctuation, not document text - collapsed away
         * entirely in read-only mode, still drawn (just colored) while
         * editing. The is_*_marker flags identify which span a delimiter
         * itself belongs to, so (unlike in_span/in_bold/in_link_text, which
         * only become true *after* an opening delimiter toggles them) both
         * the opening and closing delimiter of a span get that span's color
         * too, not just the plain text color for whichever one happens to
         * flip the flag off. */
        int is_delim = 0, skip_extra = 0;
        int is_code_marker = 0, is_bold_marker = 0, is_link_marker = 0;

        if (in_html_comment)
        {
            is_delim = 1;
            if (i + 2 < line_len && line[i] == '-' && line[i + 1] == '-' && line[i + 2] == '>')
            {
                /* The closing "-->" is 2 more delimiter bytes beyond the one
                 * already accounted for by is_delim below. */
                skip_extra = 2;
                in_html_comment = 0;
            }
        }
        else if (is_fence)
        {
            is_delim = 1;
        }
        else if (!in_code_block)
        {
            if (cp == '`')
            {
                is_delim = 1;
                is_code_marker = 1;
                in_span = !in_span;
            }
            else if (cp == '*' && i + 1 < line_len && line[i + 1] == '*')
            {
                is_delim = 1;
                is_bold_marker = 1;
                skip_extra = 1;
                in_bold = !in_bold;
            }
            else if (cp == '<' && i + 3 < line_len && line[i + 1] == '!' &&
                    line[i + 2] == '-' && line[i + 3] == '-')
            {
                is_delim = 1;
                skip_extra = 3;
                in_html_comment = 1;
            }
            else if (cp == '[')
            {
                const char* p = line + i + 1;
                while (p < line + line_len && *p != ']' && *p != '\n')
                    p++;
                if (p < line + line_len && *p == ']' && p + 1 < line + line_len && p[1] == '(')
                {
                    in_link_text = 1;
                    is_delim = 1;
                    is_link_marker = 1;
                }
            }
            else if (cp == ']' && in_link_text)
            {
                in_link_text = 0;
                is_delim = 1;
                is_link_marker = 1;
                if (i + 1 < line_len && line[i + 1] == '(')
                    in_link_dest = 1;
            }
            else if (in_link_dest)
            {
                is_delim = 1;
                if (cp == ')')
                {
                    in_link_dest = 0;
                }
            }
        }

        if (read_only && is_delim)
        {
            /* True removal, not a blanked-out placeholder: no glyph drawn
             * and no column consumed, so the line visually closes the gap.
             * The caller (render_editor()) maps cursor/selection/scrollbar
             * columns through md_scan_line() instead of a raw column count,
             * so this doesn't desync the caret or a drag-selection from what
             * ends up on screen - see the block comment above it. */
            i += clen + skip_extra;
            continue;
        }

        if (in_code_block)
            fg = g_theme.md_code_fg;
        else if (heading)
            fg = g_theme.md_heading_fg;
        else if (blockquote)
            fg = g_theme.md_blockquote_fg;
        else if (in_span || is_code_marker)
            fg = g_theme.md_code_fg;
        else if (in_link_text || is_link_marker)
            fg = g_theme.md_link_fg;
        else
            fg = g_theme.editor_fg;
        if (in_bold || is_bold_marker)
            fg = g_theme.md_bold_fg;

        int selected = col >= sel_col_lo && col < sel_col_hi;
        emit_hscroll(x, y, col, scroll_x, w, cp, fg,
                     selected ? g_theme.editor_sel_bg : bg);
        col++;
        i += clen;

        /* Editing mode only reaches here with skip_extra > 0 (read-only
         * already collapsed the whole run above) - draw the marker's
         * remaining bytes (the 2nd '*' of "**", "!--" of "<!--", "--" of
         * "-->") one codepoint at a time, same color as the byte just
         * drawn. */
        for (; skip_extra > 0; skip_extra--)
        {
            uint32_t ecp;
            int eclen = utf8_decode(line + i, &ecp);
            int esel = col >= sel_col_lo && col < sel_col_hi;
            emit_hscroll(x, y, col, scroll_x, w, ecp, fg,
                         esel ? g_theme.editor_sel_bg : bg);
            col++;
            i += eclen;
        }
    }
    for (; col < scroll_x + w; col++)
    {
        int selected = col >= sel_col_lo && col < sel_col_hi;
        emit_hscroll(x, y, col, scroll_x, w, ' ', g_theme.editor_fg, selected ? g_theme.editor_sel_bg : bg);
    }
}


static const char* diag_tag(ui_diag_type type)
{
    switch (type)
    {
    case UI_DIAG_ERROR:   return "error";
    case UI_DIAG_WARNING: return "warning";
    case UI_DIAG_INFO:
    default:              return "info";
    }
}

/* Builds the inline annotation string render_diagnostic draws: a leading
 * "\xE2\x86\x90" (U+2190 left arrow) pointing back at the offending code, the
 * severity tag and message, and a "(+N more)" suffix when other diagnostics
 * share the line. Factored out so editor_content_cols() can measure it for
 * horizontal-scroll clamping without duplicating the format. */
static void format_diagnostic(char* buf, size_t cap, const ui_diagnostic* d, int extra_count)
{
    if (extra_count > 0)
        snprintf(buf, cap, "\xE2\x86\x90 %s: %s (+%d more)", diag_tag(d->type), d->message, extra_count);
    else
        snprintf(buf, cap, "\xE2\x86\x90 %s: %s", diag_tag(d->type), d->message);
}

/* Renders one line's diagnostic inline, "Error Lens"-style, right after its
 * source text (see render_editor() - only the single worst one of a group is
 * ever passed in here). Starts at column `text_cols` (the source line's own
 * width) and honors the same `scroll_x` horizontal offset as the text, so the
 * annotation pans in lockstep and can be read by scrolling right. */
static void render_diagnostic(int x, int y, int w, int scroll_x,
                               int text_cols, const ui_diagnostic* d, int extra_count)
{
    char buf[256];
    format_diagnostic(buf, sizeof buf, d, extra_count);

    // Get foreground and background from theme
    uint32_t fg, bg;
    switch (d->type)
    {
    case UI_DIAG_ERROR:
        fg = g_theme.diag_error_fg;
        bg = g_theme.diag_error_bg;
        break;
    case UI_DIAG_WARNING:
        fg = g_theme.diag_warning_fg;
        bg = g_theme.diag_warning_bg;
        break;
    default:
        fg = g_theme.diag_info_fg;
        bg = g_theme.diag_info_bg;
        break;
    }

    int col = text_cols;
    for (const char* p = buf; *p && col < scroll_x + w; )
    {
        uint32_t cp;
        int clen = utf8_decode(p, &cp);
        emit_hscroll(x, y, col, scroll_x, w, cp, fg, bg);
        col++;
        p += clen;
    }
}

/* Widest content row in columns. A diagnostic line's width is its source
 * columns PLUS the inline annotation render_diagnostic appends, so horizontal
 * scrolling can reach the end of that text. One lockstep walk over the buffer
 * and the (line-sorted) diagnostic list - O(document + diagnostics), matching
 * render_editor()'s own merge. */
static int editor_content_cols(ui_node* n)
{
    int text_len = (int)strlen(n->label);
    ui_diagnostic* diag = n->syntax == UI_SYNTAX_VT100 ? NULL : n->diagnostics;
    int md_ro = n->syntax == UI_SYNTAX_MARKDOWN && n->read_only;
    int in_block = 0;
    int line_idx = 0, off = 0, max_cols = 0;
    for (;;)
    {
        int ls = off;
        while (off < text_len && n->label[off] != '\n')
            off++;
        /* A read-only Markdown line's *visible* width, once hidden
         * delimiters are actually collapsed (see the md_scan_line() block
         * comment above render_editor()) - using the raw utf8_col_of() here
         * would size the scrollbar/max-hscroll for text that's wider than
         * what's actually drawn, letting the view scroll right past the end
         * of the visible content into blank space. */
        int cols = md_ro ? md_scan_line(n->label + ls, off - ls, &in_block, -1, -1)
                          : utf8_col_of(n->label + ls, off - ls);

        while (diag && diag->line - 1 < line_idx)
            diag = diag->next;
        if (diag && diag->line - 1 == line_idx)
        {
            ui_diagnostic* best = diag, * p = diag;
            int count = 0;
            while (p && p->line - 1 == line_idx)
            {
                count++;
                if (p->type > best->type)
                    best = p;
                p = p->next;
            }
            char buf[256];
            format_diagnostic(buf, sizeof buf, best, count - 1);
            cols += utf8_col_of(buf, (int)strlen(buf));
            diag = p;
        }

        if (cols > max_cols)
            max_cols = cols;

        if (off < text_len)
        {
            off++;          /* past the '\n' onto the next line */
            line_idx++;
        }
        else
        {
            break;          /* consumed the last line */
        }
    }
    return max_cols;
}

/* Keep n->hscroll in [0, content_width - w] - the horizontal analogue of
 * editor_clamp_scroll. Called after a Shift+wheel pan (caret-following moves
 * stay within real text and clamp themselves). */
static void editor_clamp_hscroll(ui_node* n)
{
    int max_hscroll = editor_content_cols(n) - editor_text_w(n);
    if (max_hscroll < 0)
        max_hscroll = 0;
    if (n->hscroll > max_hscroll)
        n->hscroll = max_hscroll;
    if (n->hscroll < 0)
        n->hscroll = 0;
}

/* The horizontal half of <editor>'s self-contained scrollbar pair (see
 * editor_has_vscrollbar and friends, above) - same shape, just over content
 * columns instead of line count, and an overlay in the editor's own bottom
 * row instead of its rightmost column. Spans the box's full width, same as
 * the vertical one's full height - see render_editor for how it still reads
 * as a slim bar rather than a solid row, using a half-height block glyph
 * instead of a narrower track. */
static int editor_has_hscrollbar(ui_node* n)
{
    if (!(n->w > 0 && n->h > 0))
        return 0;
    return editor_content_cols(n) > editor_text_w(n);
}

/* Maps onto the editor's own row, but starting past the line-number gutter
 * (if any) and narrowed to match - kept as its own function (rather than
 * inlining n->x/n->w everywhere) so process_window's hit-test, the thumb
 * math below, and render_editor's drawing can't drift apart. */
static void editor_hscrollbar_layout(ui_node* n, int* out_track_x0, int* out_track_w)
{
    int gutter_w = editor_gutter_width(n);
    *out_track_x0 = n->x + gutter_w;
    *out_track_w = n->w - gutter_w;
}

static void editor_hscrollbar_thumb(ui_node* n, int* out_start, int* out_len)
{
    int track_x0, track_w;
    editor_hscrollbar_layout(n, &track_x0, &track_w);
    (void)track_x0;

    int text_w = editor_text_w(n);
    int total = editor_content_cols(n);
    int max_hscroll = total - text_w;
    if (max_hscroll <= 0 || track_w <= 0)
    {
        *out_start = 0;
        *out_len = track_w;
        return;
    }
    int len = track_w * text_w / total;
    if (len < 1) len = 1;
    if (len > track_w) len = track_w;
    int start = (n->hscroll * (track_w - len)) / max_hscroll;
    if (start < 0) start = 0;
    if (start > track_w - len) start = track_w - len;
    *out_start = start;
    *out_len = len;
}

/* Sets n->hscroll from a click/drag at screen column `mouse_x` within the
 * editor's own track - the inverse of editor_hscrollbar_thumb(). */
static void editor_hscrollbar_set_from_mouse(ui_node* n, int mouse_x)
{
    int track_x0, track_w;
    editor_hscrollbar_layout(n, &track_x0, &track_w);

    if (track_w <= 1)
    {
        n->hscroll = 0;
    }
    else
    {
        int max_hscroll = editor_content_cols(n) - editor_text_w(n);
        if (max_hscroll < 0)
            max_hscroll = 0;
        int col = mouse_x - track_x0;
        if (col < 0) col = 0;
        if (col > track_w - 1) col = track_w - 1;
        n->hscroll = (col * max_hscroll + (track_w - 1) / 2) / (track_w - 1);
    }
    editor_clamp_hscroll(n);
}

/* A multi-line syntax-highlighted text area, scrolled to n->scroll vertically
 * and n->hscroll horizontally (both kept in view of the caret by
 * editor_ensure_cursor_visible(), or moved independently by the mouse wheel -
 * see ui_screen_update()). Columns left of n->hscroll or past n->hscroll+
 * text_w are clipped by emit_hscroll inside each line renderer - text_w
 * being n->w minus whatever the line-number gutter (editor_gutter_width())
 * currently claims on the left, 0 when it's off. */
static void render_editor(ui_screen* s, ui_node* n)
{
    int focused = (s->focused == n);
    int caret = focused && s->caret_visible;  /* blink phase - see ui_screen_update */

    /* Line-number gutter (see editor_gutter_width/ui_set_show_line_numbers) -
     * text_x/text_w stand in for n->x/n->w everywhere below that actually
     * draws or measures the document's text, so the gutter (when present)
     * simply isn't overwritten by it. The gutter's own numbers are drawn
     * per-row further down, alongside each row's line_bg, so a gutter cell
     * picks up the same current-line/code-block tint as the text next to
     * it. */
    int gutter_w = editor_gutter_width(n);
    int text_x = n->x + gutter_w;
    int text_w = n->w - gutter_w;

    int cursor_line, cursor_line_start;
    editor_cursor_line(n, &cursor_line, &cursor_line_start);
    int cursor_col;
    if (n->syntax == UI_SYNTAX_MARKDOWN && n->read_only)
    {
        /* The caret's *visual* column once hidden delimiters are actually
         * collapsed away (see the md_scan_line() block comment above) -
         * a raw utf8_col_of() here would draw the caret block over the
         * wrong glyph on any line that hides something before it. */
        int cursor_line_end = cursor_line_start;
        while (n->label[cursor_line_end] && n->label[cursor_line_end] != '\n')
            cursor_line_end++;
        int block_at_cursor_line = md_in_block_at(n->label, cursor_line_start);
        cursor_col = md_scan_line(n->label + cursor_line_start, cursor_line_end - cursor_line_start,
                                   &block_at_cursor_line, n->cursor - cursor_line_start, -1);
    }
    else
    {
        cursor_col = utf8_col_of(n->label + cursor_line_start, n->cursor - cursor_line_start);
    }

    int has_sel = has_selection(n);
    int sel_lo = 0, sel_hi = 0;
    if (has_sel)
        selection_range(n, &sel_lo, &sel_hi);
    int text_len = (int)strlen(n->label);

    /* Diagnostics (see ui_editor_add_diagnostic) are kept sorted ascending
     * by line, and `line_idx` below only ever increases as `row` does - so
     * this single pointer, advanced at most once per diagnostic across the
     * whole loop, is a merge of the two sorted sequences: O(rows +
     * diagnostics in view), never a per-row search. Not shown at all in
     * VT100 mode (compiler output, not source). */
    ui_diagnostic* diag = n->syntax == UI_SYNTAX_VT100 ? NULL : n->diagnostics;

    /* ONE forward walk over the buffer for everything below: skip the lines
     * above the viewport (priming whatever multi-line highlighter state the
     * syntax mode tracks on the way - see scan_multiline_state()), then peel
     * one line per visible row by advancing `off` to each next '\n'. The old
     * editor_line_range-per-row rescanned from the top of the buffer (plus a
     * strlen) for EVERY row - O(rows x document) per repaint; this is
     * O(offset-to-viewport + visible bytes) total, and the VT100 Output
     * window benefits equally. */
    int in_block = 0;
    int bracket_depth = 0;  /* C syntax only - see render_editor_line's
                             * rainbow bracket coloring; harmless dead weight
                             * for every other syntax, which never reads it */
    /* Markdown only - see md_advance_c_block_state()'s block comment above:
     * whether the line currently being walked lies inside a ```c fenced
     * block, and (only meaningful when it does) that block's own C block-
     * comment continuation / bracket depth, entirely separate from
     * in_block/bracket_depth just above. Harmless dead weight for every
     * other syntax mode, same as bracket_depth is. */
    int in_c_block = 0;
    int c_comment_block = 0;
    int c_bracket_depth = 0;
    int off = 0, have_line = 1;  /* start of the current line; whether it exists */
    int start_line = 0;

    /* Resume from the scan cache when it's still valid (no edits since - see
     * ui_set_label) and not past the target: an unchanged scroll position
     * resumes exactly at the viewport, skipping the walk entirely - the
     * common case, since most repaints (typing, cursor blink, hover) don't
     * scroll. Scrolling down resumes from the old position; scrolling up
     * falls back to a full walk once, then re-caches. */
    if (n->scan_valid && n->scan_line <= n->scroll)
    {
        start_line = n->scan_line;
        off = n->scan_off;
        in_block = n->scan_block;
        bracket_depth = n->scan_bracket_depth;
        in_c_block = n->scan_c_block;
        c_comment_block = n->scan_c_comment;
        c_bracket_depth = n->scan_c_bracket;
    }
    for (int line = start_line; line < n->scroll && have_line; line++)
    {
        int ls = off;
        while (off < text_len && n->label[off] != '\n')
            off++;
        if (n->syntax == UI_SYNTAX_MARKDOWN)
            md_advance_c_block_state(n->label + ls, off - ls, in_block,
                                      &in_c_block, &c_comment_block, &c_bracket_depth);
        in_block = scan_multiline_state(n->syntax, n->label + ls, off - ls, in_block, &bracket_depth);
        if (off < text_len)
            off++;           /* skip the '\n' onto the next line */
        else
            have_line = 0;   /* ran out - no line at n->scroll or beyond */
    }
    if (have_line)
    {
        /* Cache "line n->scroll starts at `off` with this block state" for
         * the next repaint. Not stored when the walk ran out - there's no
         * real line for the offset to belong to. */
        n->scan_valid = 1;
        n->scan_line = n->scroll;
        n->scan_off = off;
        n->scan_block = in_block;
        n->scan_bracket_depth = bracket_depth;
        n->scan_c_block = in_c_block;
        n->scan_c_comment = c_comment_block;
        n->scan_c_bracket = c_bracket_depth;
    }

    /* Persistent (not row-derived) from here down: for every other syntax
     * mode line_idx == n->scroll + row always, but a read-only Markdown
     * document's fence/comment-only lines take no row at all (see the
     * md_is_foldable_line() block comment above), so a row and the document
     * line it shows can drift apart as soon as one of those is skipped -
     * line_idx has to be threaded through the loop instead of recomputed
     * each time, incremented once for every document line consumed
     * (skipped or drawn), same as `off`/`in_block`/`bracket_depth` already
     * are. */
    int line_idx = n->scroll;

    for (int row = 0; row < n->h; row++)
    {
        int sy = n->y + row;
        if (sy >= s->screen_h)
            break;  /* this row and every one below it are off the bottom */

        if (n->syntax == UI_SYNTAX_MARKDOWN && n->read_only)
        {
            /* Skip forward past a run of fence/comment-only lines - none of
             * them get a row, so this row's content is whatever real line
             * comes after them (or none, if the document ends first). */
            while (have_line)
            {
                int fls = off, fle = off;
                while (fle < text_len && n->label[fle] != '\n')
                    fle++;
                if (!md_is_foldable_line(n->label + fls, fle - fls))
                    break;
                md_advance_c_block_state(n->label + fls, fle - fls, in_block,
                                          &in_c_block, &c_comment_block, &c_bracket_depth);
                in_block = scan_multiline_state(n->syntax, n->label + fls, fle - fls, in_block, &bracket_depth);
                if (fle < text_len)
                    off = fle + 1;
                else
                    have_line = 0;
                line_idx++;
            }
        }

        if (!have_line)
        {
            if (sy >= 0)  /* a blank row past the document's end - only if visible */
                draw_fill(n->x, n->y + row, n->w, 1, g_theme.editor_fg, g_theme.editor_bg);
            continue;
        }

        /* This row's line is [ls, le), le at its '\n' (or the buffer's end);
         * step the walk past it for the next row before drawing anything. */
        int ls = off, le = off;
        while (le < text_len && n->label[le] != '\n')
            le++;
        if (le < text_len)
            off = le + 1;
        else
            have_line = 0;

        /* Whether *this* line is itself a fence delimiter, and (using
         * in_block's value from *before* the scan_multiline_state()/render_
         * editor_line_markdown() call below toggles it) whether it opens or
         * closes a ```c block - see md_toggle_c_block_on_fence()'s block
         * comment. Computed unconditionally (cheap - a handful of byte
         * compares) rather than only under UI_SYNTAX_MARKDOWN, since a
         * non-Markdown document's lines never start with "```" as their
         * first non-blank content in any way that matters here anyway. */
        int line_is_fence = n->syntax == UI_SYNTAX_MARKDOWN && md_is_fence_line(n->label + ls, le - ls);
        if (line_is_fence)
            md_toggle_c_block_on_fence(n->label + ls, le - ls, in_block,
                                        &in_c_block, &c_comment_block, &c_bracket_depth);

        if (sy < 0)
        {
            /* Row is above the top of the screen: don't draw it, but keep the
             * block-comment state (and bracket depth) advancing so visible
             * rows below color right - including, for Markdown, the embedded
             * ```c block's own state (already toggled above if this line was
             * the fence itself; scanned here directly if it's content inside
             * one, since render_editor_line() - which would otherwise do
             * this as a side effect of drawing - never runs for a row that's
             * never drawn). */
            if (!line_is_fence && in_c_block)
                c_comment_block = scan_line_block_state(n->label + ls, le - ls, c_comment_block, &c_bracket_depth);
            in_block = scan_multiline_state(n->syntax, n->label + ls, le - ls, in_block, &bracket_depth);
            line_idx++;
            continue;
        }

        int sel_col_lo = -1, sel_col_hi = -1;
        if (has_sel)
        {
            /* Intersect the selection with [ls, line_end_incl), where
             * line_end_incl includes the line's trailing '\n' byte (if any) -
             * that lets a selection which swallows the newline (spanning
             * into the next line) show as extending to the full width of
             * this one, the usual multi-line-selection visual cue. */
            int line_end_incl = le < text_len ? le + 1 : le;
            int lo = sel_lo > ls ? sel_lo : ls;
            int hi = sel_hi < line_end_incl ? sel_hi : line_end_incl;
            if (lo < hi)
            {
                if (n->syntax == UI_SYNTAX_MARKDOWN && n->read_only)
                {
                    /* Selection bounds in the same collapsed column space
                     * render_editor_line_markdown() draws in (see the
                     * md_scan_line() block comment above) - a scratch copy
                     * of `in_block` here, since md_scan_line() toggles it
                     * for a fence line the same way the real renderer call
                     * just below does, and that one is the one whose result
                     * should actually carry over to the next row. */
                    int tmp_block = in_block;
                    sel_col_lo = md_scan_line(n->label + ls, le - ls, &tmp_block, lo - ls, -1);
                    tmp_block = in_block;
                    sel_col_hi = hi > le ? n->hscroll + n->w
                                         : md_scan_line(n->label + ls, le - ls, &tmp_block, hi - ls, -1);
                }
                else
                {
                    sel_col_lo = utf8_col_of(n->label + ls, lo - ls);
                    /* A selection past this line's end fills to the visible
                     * right edge (hscroll + w), not just column w, so the
                     * cue still spans the viewport when scrolled
                     * horizontally. */
                    sel_col_hi = hi > le ? n->hscroll + n->w : utf8_col_of(n->label + ls, hi - ls);
                }
            }
        }
        /* The whole row the caret is on gets a subtle background tint (see
         * editor_current_line_bg) instead of the plain editor_bg, purely as
         * a "which line am I on" cue - same idea VT100/Output used to fake
         * by reusing the selection color for its own line, now generalized
         * to every syntax mode with a color of its own so it doesn't read
         * as an actual selection. Only when there's no real selection - a
         * real one (has_sel above) always wins.
         *
         * For Markdown, a row that's part of a fenced code block - the
         * fence delimiter lines themselves and every content line between
         * them, any language, not just ```c - gets g_theme.md_code_bg
         * instead, so a code block reads as a visually distinct panel (in
         * both edit and read-only mode, same as every other Markdown color -
         * see the md_heading_fg et al. block comment in ide_ui.h). That
         * takes priority over the current-line tint (but still loses to an
         * actual selection) - `in_block` here is this line's PRE-toggle
         * state (see line_is_fence's own comment above), so it already
         * covers a plain content line (in_block was 1 entering it); ORing in
         * line_is_fence additionally covers the fence line itself, which
         * enters the block with in_block still 0 (opening) or leaves it with
         * in_block still 1 (closing) - either way it's part of the block.
         *
         * A read-only Markdown preview never gets the current-line tint at
         * all (nor, further down, a blinking caret - see the caret block's
         * own comment) - there's nothing being "edited" for a highlighted
         * line/caret to track, and both would just be a stray visual left
         * over from wherever the cursor last happened to land (a click, a
         * search result, ...) rather than an actual editing cue. */
        uint32_t line_bg;
        int md_readonly = n->syntax == UI_SYNTAX_MARKDOWN && n->read_only;
        if (!has_sel && n->syntax == UI_SYNTAX_MARKDOWN && (in_block || line_is_fence))
            line_bg = g_theme.md_code_bg;
        else if (!has_sel && !md_readonly && line_idx == cursor_line)
            line_bg = g_theme.editor_current_line_bg;
        else
            line_bg = g_theme.editor_bg;

        /* Captured before render_editor_line_markdown() below mutates
         * `in_block` for this line, so the caret "under" char lookup further
         * down (which runs after the switch) can still map cursor_col back
         * to a byte offset using the state this line actually started
         * from. */
        int line_entry_block = in_block;

        /* The gutter cell for this row - drawn before the text so a wide
         * line's leftmost (hscrolled-off) glyphs never bleed into it. Right-
         * aligned within [n->x, text_x), same convention as line_bg above:
         * it picks up whatever tint this row got (current line/code block),
         * so it reads as part of the same row rather than a separate strip.
         * Skipped entirely when gutter_w is 0 (feature off, or VT100). */
        if (gutter_w > 0)
        {
            char num[16];
            int len = snprintf(num, sizeof num, "%d", line_idx + 1);
            draw_fill(n->x, n->y + row, gutter_w, 1, g_theme.editor_linenum_fg, line_bg);
            if (len > 0 && len < gutter_w)
                draw_text(n->x + (gutter_w - 1 - len), n->y + row, num,
                          g_theme.editor_linenum_fg, line_bg);
        }

        switch (n->syntax)
        {
        case UI_SYNTAX_VT100:
            render_editor_line_ansi(text_x, n->y + row, text_w, n->hscroll, n->label + ls, le - ls,
                                     sel_col_lo, sel_col_hi, line_bg);
            break;
        case UI_SYNTAX_MARKDOWN:
            /* A content line (not the fence itself) inside a block tagged
             * "```c" reuses the real C highlighter - same coloring a plain
             * .c file gets - instead of Markdown's own flat md_code_fg; see
             * md_toggle_c_block_on_fence()'s block comment for how in_c_
             * block/c_comment_block/c_bracket_depth get here. Column math
             * (sel_col_lo/sel_col_hi, cursor_col above) needs no special
             * case either way - md_scan_line() already treats every byte in
             * a code block (of any language) as unhidden, i.e. plain
             * per-codepoint columns, the same thing utf8_col_of() would've
             * produced - see the `!in_code_block` gate in its block comment. */
            if (!line_is_fence && in_c_block)
                render_editor_line(text_x, n->y + row, text_w, n->hscroll, n->label + ls, le - ls,
                                    sel_col_lo, sel_col_hi, &c_comment_block, line_bg, &c_bracket_depth);
            else
                render_editor_line_markdown(text_x, n->y + row, text_w, n->hscroll, n->label + ls, le - ls,
                                             sel_col_lo, sel_col_hi, &in_block, n->read_only, line_bg);
            break;
        case UI_SYNTAX_C:
            render_editor_line(text_x, n->y + row, text_w, n->hscroll, n->label + ls, le - ls,
                                sel_col_lo, sel_col_hi, &in_block, line_bg, &bracket_depth);
            break;
        default:
            render_editor_line_plain(text_x, n->y + row, text_w, n->hscroll, n->label + ls, le - ls,
                                      sel_col_lo, sel_col_hi, line_bg);
            break;
        }

        while (diag && diag->line - 1 < line_idx)
            diag = diag->next;
        if (diag && diag->line - 1 == line_idx)
        {
            ui_diagnostic* best = diag, * p = diag;
            int count = 0;
            while (p && p->line - 1 == line_idx)
            {
                count++;
                if (p->type > best->type)
                    best = p;
                p = p->next;
            }
            int diag_text_cols = utf8_col_of(n->label + ls, le - ls);
            render_diagnostic(text_x, n->y + row, text_w, n->hscroll, diag_text_cols, best, count - 1);
            diag = p;  /* past this whole line's group - never revisited */
        }

        /* No blinking caret block in the VT100/Output window - it's read-only
         * compiler output, not something the user is typing into, so a caret
         * there would just be a distracting artifact of wherever the last
         * click/goto-source landed. Same reasoning for a read-only Markdown
         * preview - there's nothing to type into there either, and (see
         * line_bg above) it no longer gets the current-line tint either, so
         * a caret alone would be an orphaned block with no line-level cue to
         * go with it. */
        if (caret && !has_sel && n->syntax != UI_SYNTAX_VT100 && !md_readonly && line_idx == cursor_line &&
            cursor_col >= n->hscroll && cursor_col < n->hscroll + text_w)
        {
            uint32_t under = ' ';
            int off;
            if (n->syntax == UI_SYNTAX_MARKDOWN && n->read_only)
            {
                /* Reverse the same collapsed column mapping used for
                 * cursor_col/sel_col_* above, from the state this line
                 * entered with (line_entry_block), not the post-toggle
                 * `in_block` left behind by render_editor_line_markdown()
                 * a few lines up. */
                int tmp_block = line_entry_block;
                off = ls + md_scan_line(n->label + ls, le - ls, &tmp_block, -1, cursor_col);
            }
            else
            {
                int col = 0;
                off = ls;
                while (col < cursor_col && off < le)
                {
                    uint32_t cp;
                    off += utf8_decode(n->label + off, &cp);
                    col++;
                }
            }
            if (off < le)
                utf8_decode(n->label + off, &under);
            emit_char(text_x + cursor_col - n->hscroll, n->y + row, under,
                      g_theme.editor_sel_fg, g_theme.editor_sel_bg);
        }

        line_idx++;  /* this row's document line is now fully processed */
    }

    /* The scrollbar overlays (see editor_has_vscrollbar/editor_has_
     * hscrollbar) - painted over the rows just drawn above, only while
     * they're actually relevant to look at: the mouse is over this editor,
     * or a thumb is mid-drag (which can carry the mouse off the box
     * entirely - see process_window). Same self-contained, no-permanent-
     * space-spent design as <listbox>'s own scrollbar (see render_listbox);
     * this one just has a horizontal counterpart too, since editor lines
     * (unlike listbox rows) can run wider than the box. */
    int show_v = editor_has_vscrollbar(n) &&
        (s->hot == n || s->dragging_editor_vscrollbar == n);
    int show_h = editor_has_hscrollbar(n) &&
        (s->hot == n || s->dragging_editor_hscrollbar == n);

    /* When both bars are visible at once, they'd otherwise both claim the
     * single corner cell where the vertical bar's column meets the
     * horizontal bar's row - the vertical one backs off that last row (it's
     * the horizontal bar's to draw) so neither overwrites the other. */
    if (show_v)
    {
        int thumb_start, thumb_len;
        editor_vscrollbar_thumb(n, &thumb_start, &thumb_len);
        int sx = n->x + n->w - 1;
        int rows = show_h ? n->h - 1 : n->h;
        for (int row = 0; row < rows; row++)
        {
            int sy = n->y + row;
            if (sy < 0 || sy >= s->screen_h)
                continue;
            int is_thumb = row >= thumb_start && row < thumb_start + thumb_len;
            emit_char(sx, sy, ' ', g_theme.scrollbar_bg,
                      is_thumb ? g_theme.scrollbar_thumb_bg : g_theme.scrollbar_bg);
        }
    }
    if (show_h)
    {
        int track_x0, track_w;
        editor_hscrollbar_layout(n, &track_x0, &track_w);
        int thumb_start, thumb_len;
        editor_hscrollbar_thumb(n, &thumb_start, &thumb_len);
        int sy = n->y + n->h - 1;
        if (sy >= 0 && sy < s->screen_h)
        {
            /* Full width, like the vertical bar's full height - but drawn
             * with U+2584 (lower half block) instead of a plain space, so
             * only the bottom half of the row's cells get the scrollbar
             * color and the top half still shows the editor's own
             * background. Reads as a slim bar hugging the bottom edge
             * rather than a solid row, without giving up any width. */
            for (int col = 0; col < track_w; col++)
            {
                int is_thumb = col >= thumb_start && col < thumb_start + thumb_len;
                emit_char(track_x0 + col, sy, 0x2584 /* lower half block */,
                          is_thumb ? g_theme.scrollbar_thumb_bg : g_theme.scrollbar_bg,
                          g_theme.editor_bg);
            }
        }
    }
}

/* Dispatches one node to its type's render function - shared by the
 * top-level document walk and render_window() (a window's children are
 * ordinary nodes too, just nested one level deeper). */
static void render_node(ui_screen* s, ui_node* n)
{
    switch (n->type)
    {
    case UI_TAG_MENUBAR:
        render_menubar(s, n);
        break;
    case UI_TAG_STATUSBAR:
        /* Positions already set by the last ui_screen_update() call. */
        render_statusbar(s, n);
        break;
    case UI_TAG_BUTTON:
        render_button(s, n);
        break;
    case UI_TAG_BOX:
        render_box(n);
        break;
    case UI_TAG_TEXT:
        draw_text(n->x, n->y, n->label, n->fg, n->bg);
        break;
    case UI_TAG_INPUT:
        render_input(s, n);
        break;
    case UI_TAG_EDITOR:
        render_editor(s, n);
        break;
    case UI_TAG_SELECT:
        render_select(s, n);
        break;
    case UI_TAG_LISTBOX:
        render_listbox(s, n);
        break;
    case UI_TAG_GROUP:
        render_group(s, n);
        break;
    case UI_TAG_WINDOW:
        render_window(s, n);
        break;
    case UI_TAG_MODAL:
        /* Invisible by design - it only exists to gate input (see
         * ui_screen_update); its <window> child is what actually paints,
         * as a top-layer pass below, not here in document order. */
        break;
    default:
        break;
    }
}

void ui_screen_invalidate(ui_screen* s)
{
    if (s)
        s->force_full = 1;  /* consumed at the top of the next ui_screen_render */
}

int ui_screen_render(ui_screen* s, int* out_x, int* out_y, int* out_w, int* out_h)
{
    /* (Re)allocate the shadow (`cache`, what the backend shows) and the
     * frame's intent buffer (`next`) to the current grid. A size change (or
     * the first render) makes the old shadow meaningless, so force a full
     * repaint. */
    if (!s->cache || s->cache_w != s->screen_w || s->cache_h != s->screen_h)
    {
        free(s->cache);
        free(s->next);
        int n = s->screen_w * s->screen_h;
        s->cache = n > 0 ? calloc((size_t)n, sizeof(ui_cell)) : NULL;
        s->next = n > 0 ? calloc((size_t)n, sizeof(ui_cell)) : NULL;
        s->cache_w = s->screen_w;
        s->cache_h = s->screen_h;
        s->force_full = 1;
        s->prev_overlay_count = 0;
    }

    s->dirty_x = s->dirty_y = s->dirty_w = s->dirty_h = 0;
    s->overlay_count = 0;
    s->overlay_overflow = 0;
    s->emit_seq = 0;
    g_render_screen = s;

    int was_full = s->force_full;
    if (s->next)
    {
        /* Clear the intent buffer: ch==0 marks a cell no node wrote, treated
         * as blank-on-black by the diff below (in practice the desktop fill
         * covers every cell, so this only matters for a desktop-less app). */
        memset(s->next, 0, (size_t)(s->cache_w * s->cache_h) * sizeof(ui_cell));
    }
    if (s->force_full && s->cache)
    {
        /* Sentinel every shadow cell (a ch that can't equal a real glyph) so
         * the diff treats every cell as changed and repaints it. */
        memset(s->cache, 0xFF, (size_t)(s->cache_w * s->cache_h) * sizeof(ui_cell));
    }
    else if (s->cache)
    {
        /* Erase last frame's destructive overlays: sentinel the shadow cells
         * they touched so the diff repaints their real content this frame, so
         * a moved shadow or a blinked-off cursor leaves no ghost. */
        for (int i = 0; i < s->prev_overlay_count; i++)
        {
            ui_rect r = s->prev_overlays[i];
            for (int yy = r.y; yy < r.y + r.h; yy++)
            {
                if (yy < 0 || yy >= s->cache_h) continue;
                for (int xx = r.x; xx < r.x + r.w; xx++)
                {
                    if (xx < 0 || xx >= s->cache_w) continue;
                    s->cache[yy * s->cache_w + xx].ch = 0xFFFFFFFFu;
                }
            }
        }
    }

    /* Desktop backdrop: the very bottom of the stack, painted before
     * anything else so every node still paints over it normally - one
     * draw_box call instead of a screen_w * screen_h draw_char loop. */
    if (s->desktop_on)
        emit_box(0, 0, s->screen_w, s->screen_h, s->desktop_bg, s->desktop_bg, 0);

    /* Base layer: every node paints in document order (later siblings over
     * earlier ones), same as normal HTML flow content. */
    for (int i = 0; i < s->root->child_count; i++)
        render_node(s, s->root->children[i]);

    /* Top layer, back to front: the active modal/floating window, then the
     * open dropdown (a menu must still be reachable/visible over a floating
     * window - it can't get stuck underneath one), then the app's chrome
     * (menubar/statusbar) on top of everything, then the cursor last. Same
     * guarantee a browser gives an open <select>: always on top, regardless
     * of where it sits in the document. */
     /* Floating windows render first, back-to-front (windows[0] furthest
      * back, windows[window_count-1] frontmost/topmost), so an active
      * blocking modal - shown right after - always layers on top of every
      * one of them, matching the input-gating order above. */
    for (int i = 0; i < s->window_count; i++)
    {
        ui_node* window = s->windows[i];
        for (int j = 0; j < window->child_count; j++)
            render_node(s, window->children[j]);
    }
    if (s->modal_stack_count > 0)
    {
        ui_node* modal = s->modal_stack[s->modal_stack_count - 1];
        for (int i = 0; i < modal->child_count; i++)
            render_node(s, modal->children[i]);
    }

    render_open_select(s);
    render_open_dropdown(s);

    /* The app's chrome (menubar/statusbar) always stays on top, even over a
     * modal/floating window (or its dropdown) that happens to reach that far
     * - repainting them here, after both, guarantees it regardless of where
     * in the document they sit. Same nodes, same cached layout from
     * update(); this just re-draws them one more time as the true topmost
     * layer. */
    ui_node* menubar = find_child_by_type(s->root, UI_TAG_MENUBAR);
    ui_node* statusbar = find_child_by_type(s->root, UI_TAG_STATUSBAR);
    if (menubar)
        render_menubar(s, menubar);
    if (statusbar)
        render_statusbar(s, statusbar);

    if (s->cursor_on)
        emit_box(s->cursor_x, s->cursor_y, 1, 1, 0, 0, UI_BOX_INVERT);

    /* --- Diff `next` (this frame's intent) against `cache` (what the backend
     * shows) and emit a backend draw only for cells that actually differ.
     * This is where the CPU is saved: an unchanged screen emits nothing. --- */
    if (s->cache && s->next)
    {
        for (int y = 0; y < s->cache_h; y++)
        {
            for (int x = 0; x < s->cache_w; x++)
            {
                int idx = y * s->cache_w + x;
                ui_cell nc = s->next[idx];
                if (nc.ch == 0)
                {         /* untouched cell -> blank on black */
                    nc.ch = ' '; nc.fg = 0; nc.bg = 0;
                }
                ui_cell cc = s->cache[idx];
                if (nc.ch == cc.ch && nc.fg == cc.fg && nc.bg == cc.bg)
                    continue;
                ui_draw_char(x, y, nc.ch, nc.fg, nc.bg);
                s->cache[idx] = nc;
                union_dirty(s, x, y, 1, 1);
            }
        }
    }

    /* Replay destructive overlays on top of the freshly-diffed content, in the
     * document order they were recorded. Their cells are erased next frame via
     * prev_overlays (see the top of this function).
     *
     * Z-order: an overlay must NOT paint over a cell that a higher-z node
     * covered after the overlay was recorded (e.g. a modal button's shadow
     * under an open <select> popup, or one window's shadow under a window in
     * front of it). So each overlay cell is drawn only where the content
     * there is no newer than the overlay (next[cell].seq <= o.seq); a
     * newer cell means later, higher content occludes it. Drawn per cell
     * since occlusion is per cell - the shrink flags produce the same
     * half-cell result whether the strip is drawn whole or one cell at a
     * time. */
    for (int i = 0; i < s->overlay_count; i++)
    {
        ui_overlay o = s->overlays[i];
        for (int yy = o.y; yy < o.y + o.h; yy++)
        {
            if (yy < 0 || yy >= s->cache_h) continue;
            for (int xx = o.x; xx < o.x + o.w; xx++)
            {
                if (xx < 0 || xx >= s->cache_w) continue;
                if (s->next[yy * s->cache_w + xx].seq > o.seq)
                    continue;  /* occluded by later, higher-z content */
                ui_draw_box(xx, yy, 1, 1, 0, 0, o.flags);
            }
        }
        union_dirty(s, o.x, o.y, o.w, o.h);
    }

    /* Consume this frame's full-repaint flag, then carry the overlay set
     * forward so next frame can erase it. Too many overlays to track degrades
     * safely to a full repaint next frame. */
    s->force_full = 0;
    if (s->overlay_overflow)
    {
        s->prev_overlay_count = 0;
        s->force_full = 1;
    }
    else
    {
        for (int i = 0; i < s->overlay_count; i++)
        {
            s->prev_overlays[i].x = s->overlays[i].x;
            s->prev_overlays[i].y = s->overlays[i].y;
            s->prev_overlays[i].w = s->overlays[i].w;
            s->prev_overlays[i].h = s->overlays[i].h;
        }
        s->prev_overlay_count = s->overlay_count;
    }
    g_render_screen = NULL;

    /* A full repaint invalidates the whole screen regardless of what the
     * per-cell diff happened to accumulate (e.g. a desktop-less empty area
     * still needs its freshly-cleared backing store blitted). */
    if (was_full)
    {
        s->dirty_x = 0; s->dirty_y = 0;
        s->dirty_w = s->screen_w; s->dirty_h = s->screen_h;
    }

    if (out_x) *out_x = s->dirty_x;
    if (out_y) *out_y = s->dirty_y;
    if (out_w) *out_w = s->dirty_w;
    if (out_h) *out_h = s->dirty_h;
    return s->dirty_w > 0 && s->dirty_h > 0;
}

int ui_screen_mouse_x(ui_screen* s) { return s->mouse_x; }
int ui_screen_mouse_y(ui_screen* s) { return s->mouse_y; }
int ui_screen_mouse_moved(ui_screen* s) { return s->mouse_moved; }
int ui_screen_mouse_right_pressed(ui_screen* s) { return s->mouse_right_pressed; }

int ui_screen_width(ui_screen* s) { return s->screen_w; }
int ui_screen_height(ui_screen* s) { return s->screen_h; }

int ui_screen_window_count(ui_screen* s) { return s->window_count; }
ui_node* ui_screen_window_at(ui_screen* s, int i)
{
    return (i >= 0 && i < s->window_count) ? s->windows[i] : NULL;
}
int ui_screen_key_count(ui_screen* s) { return s->key_event_count; }
ui_event ui_screen_key_event(ui_screen* s, int i) { return s->key_events[i]; }