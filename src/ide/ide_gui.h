/* ide_gui.h - public interface of the ide GUI framework.
 *
 * A small retained-mode UI framework: an app builds a tree of nodes (menubar,
 * statusbar, windows, widgets...), the framework lays it out, paints it and
 * routes input to it. It knows nothing about C, Cake or IDEs - the IDE is just
 * one app written with it (see GUI_IDE_SPEC.md).
 *
 * Everything is in device pixels. Distances an app gives are gui_len values:
 * a number of cells of the main monospaced font plus a number of pixels, so
 * a layout keeps its proportions when the font changes (zoom) and is
 * recomputed, never rescaled.
 */
#ifndef IDE_GUI_H
#define IDE_GUI_H

#include <stdint.h>

struct gui_app;   /* the framework instance */
struct gui_node;  /* one node of the tree */

/* Colors are 0x00RRGGBB. */
#define GUI_RGB(r, g, b) (((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b))

enum gui_kind
{
    GUI_SCREEN,     /* the root - the whole window's client area */
    GUI_MENUBAR,    /* top row; its GUI_MENU children are the menu titles */
    GUI_MENU,       /* a menu title; its GUI_ITEM children are the dropdown */
    GUI_ITEM,       /* a menu item; one with GUI_ITEM children is a submenu */
    GUI_STATUSBAR,  /* bottom row; its GUI_HOTKEY children are "key:label" (one
                     * with no id is plain text, e.g. a message); its own
                     * label is right-aligned */
    GUI_HOTKEY,
    GUI_WINDOW,     /* a framed, movable window - see gui_window_open */
    GUI_TEXT,       /* a line of text (its label), in its own colors */
    GUI_BOX,        /* a framed rect, its label centered on the top border */
    GUI_BUTTON,     /* fires its id when clicked, or Enter/Space when focused */
    GUI_INPUT,      /* one line of editable text (gui_get_value); Enter fires its id */
    GUI_SELECT,     /* a closed list; its GUI_ITEM children are the choices */
    GUI_LISTBOX,    /* rows of GUI_ITEM children; Enter or a double click fires its id */
    GUI_GROUP,      /* radio buttons, or check boxes (gui_set_multi); GUI_ITEM rows */
    GUI_EDITOR,     /* a multi-line text editor (gui_set_value / gui_get_value) */
};

/* A rect in px, window (client area) coordinates. */
struct gui_rect
{
    int x, y, w, h;
};

/* --- Layout: anchors (see GUI_IDE_SPEC.md, 2.3) --- */

enum gui_anchor
{
    GUI_ANCHOR_LEFT = 1,
    GUI_ANCHOR_TOP = 2,
    GUI_ANCHOR_RIGHT = 4,
    GUI_ANCHOR_BOTTOM = 8,
};

/* cells * (cell size of the main font) + px + percent of the parent's size
 * on that axis (two tabs, each half of a panel, say) */
struct gui_len
{
    int cells;
    int px;
    int percent;
};

/* Where a node sits inside its parent's client area. Per axis: anchored on
 * both sides it stretches with the parent; on one side it keeps its
 * width/height at that distance from that side; on neither it is centered. */
struct gui_layout
{
    int anchors;                              /* enum gui_anchor flags */
    struct gui_len left, top, right, bottom;  /* distance to the parent's edges */
    struct gui_len width, height;             /* used when an axis has one anchor or none */
};

/* --- Theme --- */

enum gui_border_style
{
    GUI_BORDER_NONE,
    GUI_BORDER_SINGLE,
    GUI_BORDER_DOUBLE,
};

/* Number of colors in a theme's editor_bracket_fg[] cycle - see its own
 * comment below. */
#define GUI_EDITOR_BRACKET_COLORS 4

/* The whole visual palette, one field per color role - every render_*()
 * function in ui.c reads its colors from here instead of hardcoded
 * constants, so a whole new look (a "theme") is just a different filled-in
 * gui_theme passed to gui_set_theme(), swappable at runtime (e.g. from a
 * menu item) without rebuilding anything. Same fields, names and meanings as the old
 * IDE's ui_theme, so its themes copied over unchanged. */
struct gui_theme
{
    uint32_t desktop_bg;  /* the general backdrop behind everything else -
                            * see ui_screen_set_desktop() */

    uint32_t btn_bg, btn_bg_hot, btn_bg_active, btn_fg;

    uint32_t hotkey_fg, hotkey_key_fg, hotkey_bg, hotkey_fg_hot, hotkey_bg_hot;

    uint32_t menu_fg, menu_bg, menu_fg_sel, menu_bg_sel;
    uint32_t menu_item_fg, menu_item_bg, menu_item_fg_hot, menu_item_bg_hot,
        menu_item_shortcut_fg, menu_item_fg_disabled;
    uint32_t menu_border_fg, menu_border_bg;  /* the open dropdown/combo
                                                * popup's own box outline -
                                                * distinct from its items' */
    enum gui_border_style menu_border_style;  /* dropdown/<select> popup frame */

    uint32_t box_fg, box_bg;
    enum gui_border_style box_border_style;  /* <box>'s own frame */

    uint32_t window_border_fg, window_border_bg, window_border_fg_dragging,
        window_border_fg_unfocused, window_close_bg;
    enum gui_border_style window_border_style;  /* a focused/dragging <window>'s
                                                 * frame */
    enum gui_border_style window_border_style_unfocused;  /* an unfocused
                                                            * <window>'s frame */
    enum gui_border_style window_border_style_docked;  /* a focused/dragging
                            * docked window's frame (ui_set_dock != NONE) -
                            * e.g. the Output/Folder panels, which a theme
                            * may want styled differently from a floating
                            * document window */
    enum gui_border_style window_border_style_docked_unfocused;  /* an
                            * unfocused docked window's frame */
    uint32_t window_fg, window_bg;  /* a non-modal <window>'s own body/
                                      * interior fill - document windows and
                                      * docked panels (Output/Folder) - distinct
                                      * from its border (window_border_* above)
                                      * and from a modal dialog's own fill
                                      * (modal_fg/modal_bg below) */

    uint32_t modal_border_fg, modal_border_bg;  /* a modal dialog's own frame
                                                  * (About, Find, Options,
                                                  * message boxes, ...) -
                                                  * distinct from a floating
                                                  * <window>'s (window_border_*
                                                  * above), since a modal is
                                                  * always drawn in its single
                                                  * focused/active style */
    enum gui_border_style modal_border_style;
    uint32_t modal_fg, modal_bg;  /* a modal dialog's own body/interior fill -
                                    * distinct from its border (modal_border_*
                                    * above) */
    uint32_t label_fg;  /* an accented caption an app puts above a field or
                          * group inside a dialog/panel ("Include Directories"
                          * over its <input>) - the framework never draws this
                          * itself, it's here so apps can pick it per theme
                          * instead of hardcoding one accent that only reads
                          * on a dark background. Must stay legible against
                          * modal_bg AND window_bg. */
    uint32_t scrollbar_bg, scrollbar_thumb_bg;  /* an <editor>'s or <listbox>'s
                                                  * own scrollbar overlay (see
                                                  * editor_has_vscrollbar/
                                                  * listbox_has_scrollbar in
                                                  * ide_ui.c) - the track's
                                                  * solid fill, and the thumb's
                                                  * solid fill on top of it */
    uint32_t scrollbar_thumb_hot_bg;  /* the thumb while the mouse is over it
                                       * or drags it - stands out from
                                       * scrollbar_thumb_bg */

    uint32_t input_bg, input_fg, input_fg_focus, input_sel_bg, input_sel_fg;
    uint32_t input_bg_focus;  /* the focused <input>'s own row fill, distinct
                               * from input_bg - lets a theme visually mark
                               * which field has focus (e.g. Visual Studio's
                               * blue-tinted active text box) instead of only
                               * the caret/selection giving it away */

    uint32_t editor_bg, editor_fg, editor_keyword_fg, editor_keyword2_fg,
        editor_string_fg,
        editor_comment_fg, editor_preproc_fg, editor_sel_bg, editor_sel_fg;
    uint32_t editor_attribute_fg;  /* a C23 [[attribute]] */
    uint32_t editor_caret_fg;  /* the caret bar; a theme with no dedicated
                                 * caret color sets this equal to editor_fg */
    /* editor_keyword_fg colors type/storage keywords (is_c_keyword1 - int,
     * struct, const, ...); editor_keyword2_fg colors control-flow keywords
     * (is_c_keyword2 - if, for, return, ...). */

    uint32_t editor_lint_fg;  /* a "//lint" line comment - same idea as
                               * editor_comment_fg but its own color so a
                               * lint directive stands out from an ordinary
                               * comment (see render_editor_line in
                               * ide_ui.c) */

    uint32_t editor_linenum_fg;  /* the line-number gutter's digits (see
                                  * editor_gutter_width/render_editor in
                                  * ide_ui.c) - deliberately its own muted
                                  * color rather than reusing editor_comment_fg
                                  * (which is a syntax-highlight accent, e.g.
                                  * green, not the neutral gray real editors
                                  * use for line numbers). */

    uint32_t editor_word_match_bg;  /* every other visible occurrence of the
                                      * word the selection covers, tinted
                                      * behind the text (the syntax colors
                                      * stay) - a "where else is this
                                      * identifier" cue, NOT a selection:
                                      * nothing about it is editable or
                                      * copyable, and the real selection keeps
                                      * its own input_sel_bg. Keep it subtle
                                      * and clearly distinct from input_sel_bg
                                      * so the two don't read as the same
                                      * thing. */

    uint32_t editor_current_line_bg;  /* the whole row the caret is on, in
                                       * every <editor> (source or VT100/
                                       * Output alike) - a subtle fill,
                                       * distinct from editor_sel_bg, so it
                                       * doesn't read as a real selection.
                                       * Only shown when there's no real
                                       * selection (see render_editor). */

    uint32_t editor_breakpoint_fg;  /* the gutter marker for a line with a
                                     * breakpoint set (see ui_editor_toggle_
                                     * breakpoint) - a bullet drawn in place
                                     * of that row's line number, so it wants
                                     * to read as "stop sign", distinct from
                                     * editor_linenum_fg's neutral gray. */

    uint32_t editor_exec_line_bg;  /* the row the debugger is currently
                                    * stopped on (see ui_set_exec_line) -
                                    * takes priority over editor_current_
                                    * line_bg so a breakpoint hit is
                                    * unambiguous even when the caret
                                    * happens to sit on the same line. */

    uint32_t editor_bracket_fg[GUI_EDITOR_BRACKET_COLORS];  /* UI_SYNTAX_C
                                       * only: ( [ { ) ] } "rainbow bracket"
                                       * coloring - cycles by nesting depth
                                       * (depth % GUI_EDITOR_BRACKET_COLORS),
                                       * a matched pair always sharing one
                                       * color - see render_editor_line. */

    uint32_t editor_tag_fg;  /* UI_SYNTAX_C only: the name right after a bare
                              * "struct"/"union"/"enum" keyword (e.g. the X
                              * in "struct X") - see is_c_tag_keyword() and
                              * render_editor_line's pending_tag. */

    uint32_t editor_number_fg;  /* UI_SYNTAX_C only: a numeric literal - int,
                                  * float, hex (0x...), with any u/l/f suffix
                                  * or exponent - see render_editor_line's
                                  * number-token scan. */

    uint32_t editor_char_fg;  /* UI_SYNTAX_C only: a 'x'-style character
                                * constant, including escapes ('\n', '\'',
                                * ...) - see render_editor_line's bounded
                                * char-literal lookahead. */

    uint32_t editor_function_fg;  /* UI_SYNTAX_C only: an identifier used as a
                                    * function - a non-keyword name immediately
                                    * followed by '(' (e.g. the foo in
                                    * "foo(x)") - see render_editor_line's
                                    * call lookahead. */

    uint32_t editor_output_bg;  /* UI_SYNTAX_VT100 only (the Output/compiler
                                  * panel): its own body fill, distinct from a
                                  * source editor's editor_bg, so the Output
                                  * panel can read as a recessed chrome-colored
                                  * pane (e.g. gray against a white document).
                                  * A theme with no dedicated Output look sets
                                  * this equal to editor_bg. */

    uint32_t editor_output_fg;  /* UI_SYNTAX_VT100 only: the Output panel's
                                  * default text color (uncolored output, and
                                  * the readable fallback when a bright ANSI
                                  * color would be invisible against a light
                                  * editor_output_bg - see ansi_readable_fg).
                                  * A theme with no dedicated Output look sets
                                  * this equal to editor_fg. */

    /* UI_SYNTAX_MARKDOWN only - see render_editor_line_markdown(). Its own
     * knobs rather than reusing the UI_SYNTAX_C colors above, so a theme can
     * restyle a Markdown document (help viewer, README preview, ...)
     * independently of how it colors C source. */
    uint32_t md_heading_fg;      /* an ATX heading line ("# ..." through
                                   * "###### ...") */
    uint32_t md_blockquote_fg;   /* a "> ..." blockquote line */
    uint32_t md_code_fg;         /* fenced ```code block``` content and
                                   * inline `code span`s alike */
    uint32_t md_bold_fg;         /* **bold** span text (and its ** markers) -
                                   * colored in both edit and read-only mode;
                                   * only read-only actually collapses the
                                   * ** markers away */
    uint32_t md_link_fg;         /* the visible text of a [link](dest) (and
                                   * its [] brackets) - colored in both edit
                                   * and read-only mode; only read-only
                                   * collapses the brackets/"(dest)" away */
    uint32_t editor_diff_add_bg;     /* UI_SYNTAX_DIFF only: background tint
                                       * for a row whose line starts with '+'
                                       * (added) - see render_editor()'s
                                       * line_bg computation. Takes priority
                                       * over editor_current_line_bg, same as
                                       * md_code_bg below. */
    uint32_t editor_diff_remove_bg;  /* UI_SYNTAX_DIFF only: same, for a row
                                       * starting with '-' (removed). */
    uint32_t editor_diff_add_word_bg;    /* the part of a '+' row that differs
                                           * from its paired '-' row */
    uint32_t editor_diff_remove_word_bg; /* same, for the '-' row */

    uint32_t md_code_bg;         /* background tint for every row that's part
                                   * of a fenced ```code block``` - the fence
                                   * delimiter lines themselves and every
                                   * content line between them (whatever
                                   * language, not just ```c) - see render_
                                   * editor()'s line_bg computation. Takes
                                   * priority over editor_current_line_bg, so
                                   * the caret's row inside a code block still
                                   * reads as "code", not "current line". */

    uint32_t listbox_fg, listbox_bg, listbox_sel_fg, listbox_sel_bg;
    /* The selected row while the <listbox> does NOT have keyboard focus -
     * a muted stand-in for listbox_sel_* so the selection stays readable
     * without competing with whichever widget is actually focused (the
     * convention every desktop toolkit follows). */
    uint32_t listbox_sel_inactive_fg, listbox_sel_inactive_bg;
    uint32_t panel_fg, panel_bg;  /* the <listbox> of a docked panel (Folder,
                          * Project, Git Changes, Debug Info) - the app colors
                          * it with gui_set_colors; a theme with no dedicated
                          * panel look sets these equal to listbox_fg/_bg */

    /* Project panel only: the leading file-type marker prepended to each
     * row's label (see project_window_refresh() in ide.c) - one color per
     * extension it recognizes, so a theme can keep them legible against its
     * own listbox_bg. A file type not listed here gets no marker (just
     * reserved blank space, no color needed). */
    uint32_t project_icon_c_fg;   /* ".c" rows */
    uint32_t project_icon_h_fg;   /* ".h" rows */
    uint32_t project_icon_md_fg;  /* ".md" rows */

    uint32_t diag_error_fg;
    uint32_t diag_error_bg;
    uint32_t diag_warning_fg;
    uint32_t diag_warning_bg;
    uint32_t diag_info_fg;
    uint32_t diag_info_bg;

};

void gui_set_theme(struct gui_app* app, const struct gui_theme* theme);
struct gui_highlighter;
/* Colors the statusbar's hints (a Markdown one hides its delimiters); NULL: plain. */
void gui_set_hint_highlighter(struct gui_app* app, const struct gui_highlighter* h);

/* --- Tree --- */

struct gui_node* gui_root(struct gui_app* app);

/* A new node, not attached to anything yet. Menubars and statusbars come
 * with their usual layout (full width, top / bottom row); every other node
 * starts anchored LEFT|TOP at 0,0 with no size. */
struct gui_node* gui_create(struct gui_app* app, enum gui_kind kind);

void gui_append(struct gui_node* parent, struct gui_node* child);
/* Takes `child` out of `parent`, not freed - gui_append puts it back. */
void gui_remove(struct gui_node* parent, struct gui_node* child);
void gui_set_label(struct gui_node* n, const char* utf8);
const char* gui_get_label(const struct gui_node* n);
enum gui_kind gui_get_kind(const struct gui_node* n);
struct gui_node* gui_get_parent(const struct gui_node* n);
void gui_set_layout(struct gui_node* n, const struct gui_layout* layout);
/* `n` starts one cell after the widest label of `label`'s column (the
 * sibling labels at its x that controls follow), its right edge kept: the
 * labels' measured width, whatever the font. */
void gui_set_after_label(struct gui_node* n, struct gui_node* label);
/* A GUI_TEXT draws its label centered in its rect, by its measured width. */
void gui_set_centered(struct gui_node* n, int centered);

/* Like HTML: a node draws in the normal or the small font, or inherits its
 * parent's (the default). Rows, columns and text inside it follow its
 * font; its own place in the parent (the anchors) is measured in the
 * parent's font. */
enum gui_font_size
{
    GUI_FONT_SIZE_INHERIT,
    GUI_FONT_SIZE_NORMAL,
    GUI_FONT_SIZE_SMALL,
    GUI_FONT_SIZE_UI,   /* in the "other fonts" family and size - may be proportional */
};

void gui_set_font_size(struct gui_node* n, enum gui_font_size size);

/* The number an activated node fires (see gui_set_on_event); 0 fires
 * nothing. */
void gui_set_id(struct gui_node* n, int id);

/* A menu item's (or a statusbar hotkey's) shortcut, as shown in its dropdown and matched against
 * keys: "Ctrl+O", "Ctrl+Shift+S", "F7", "Shift+F5", "Ctrl++", "Alt+Left",
 * "Ctrl+Space". It works whether the menu is open or not. */
void gui_set_shortcut(struct gui_node* n, const char* shortcut);

void gui_set_enabled(struct gui_node* n, int enabled);  /* default 1 */
void gui_set_separator(struct gui_node* n, int separator);  /* a menu item drawn as a line */

/* One line shown in the statusbar while the node is under the mouse. */
void gui_set_hint(struct gui_node* n, const char* utf8);

/* --- Windows ---
 *
 * A GUI_WINDOW is placed by the user (moved, resized, maximized), so its rect
 * is stored in px - the one kind of node that has no anchors. Its children
 * are laid out with anchors relative to the window's whole rect (the frame
 * is the outer cell on each side). The label is the title. */

/* Makes the main font `delta` points bigger (or smaller, negative). Every
 * font-derived size follows on the next frame; windows keep their rects. */
void gui_zoom(struct gui_app* app, int delta);
int gui_get_zoom(const struct gui_app* app);   /* the sum of every gui_zoom, points */

/* Fires `id` every `ms` milliseconds, until called again; 0 ms stops it.
 * One timer per app - for polling work that runs outside the UI thread.
 * A tick repaints nothing by itself: a handler that changed what is shown
 * calls gui_repaint. */
void gui_set_timer(struct gui_app* app, int ms, int id);

/* Lays out and paints again after the current event. */
void gui_repaint(struct gui_app* app);

/* The monospaced font families the backend offers (those installed of its
 * shortlist), and the one in use: gui_set_font changes it after the
 * current event, as gui_zoom does the size. */
int gui_font_count(const struct gui_app* app);
const char* gui_font_name(const struct gui_app* app, int index);
int gui_get_font(const struct gui_app* app);
void gui_set_font(struct gui_app* app, int index);

/* The font families the backend offers for every control but the editors
 * (GUI_FONT_UI), proportional and monospaced, and the one in use. */
int gui_ui_font_count(const struct gui_app* app);
const char* gui_ui_font_name(const struct gui_app* app, int index);
int gui_get_ui_font(const struct gui_app* app);
void gui_set_ui_font(struct gui_app* app, int index);
/* The dialogs' font (GUI_FONT_UI) is the base size, the one gui_zoom
 * changes; the editor font is a little smaller (-1), the same size (0, the
 * default) or a little larger (1). */
int gui_get_editor_size(const struct gui_app* app);
void gui_set_editor_size(struct gui_app* app, int size);

/* Ends the program after the current event. */
void gui_quit(struct gui_app* app);

/* The OS window's close button (or Alt+F4) fires `id` instead of ending
 * the program, so the app can ask first and then call gui_quit. 0: it just
 * ends (the default). */
void gui_set_quit_id(struct gui_app* app, int id);

/* The main font's cell, px - to size a window in cells when opening it. */
void gui_cell_size(const struct gui_app* app, int* w, int* h);

/* The area windows live in: between the menubar and the statusbar. */
struct gui_rect gui_desktop_rect(const struct gui_app* app);

void gui_window_set_rect(struct gui_node* win, const struct gui_rect* r);
struct gui_rect gui_window_get_rect(const struct gui_node* win);

/* Shows `win` on top of every other window, or brings it to the top if it
 * is already open. */
void gui_window_open(struct gui_app* app, struct gui_node* win);
void gui_window_close(struct gui_app* app, struct gui_node* win);

/* Closes `win` (not part of any tree) and frees it with all it holds. */
void gui_window_free(struct gui_app* app, struct gui_node* win);

/* The close icon only fires the window's id, without closing it - the app
 * decides (asks about unsaved changes, say). When it fires, `win` is the
 * top window. */
void gui_window_set_ask_close(struct gui_node* win, int ask);
int gui_window_count(const struct gui_app* app);           /* open windows */
struct gui_node* gui_window_at(const struct gui_app* app, int i);  /* 0 = bottom */

void gui_window_set_resizable(struct gui_node* win, int resizable);  /* default 1 */
void gui_window_set_shadow(struct gui_node* win, int shadow);        /* default 0 */
void gui_window_set_min_size(struct gui_node* win, int cols, int rows);
void gui_window_maximize(struct gui_app* app, struct gui_node* win);
int gui_window_get_maximized(const struct gui_node* win);

/* Detaching moves a window into an OS window of its own, filling it;
 * attaching brings it back where it was. Closing that OS window attaches
 * it. Only where the backend can (gui_can_detach); elsewhere both do
 * nothing. */
int gui_can_detach(const struct gui_app* app);
void gui_window_detach(struct gui_app* app, struct gui_node* win);
void gui_window_attach(struct gui_app* app, struct gui_node* win);
int gui_window_get_detached(const struct gui_app* app, const struct gui_node* win);

/* --- Docked windows ---
 *
 * A docked window is pinned to one side of the area between the menubar and
 * the statusbar; each side holds one. Left and right span that area's
 * height; the bottom one fills the width the side ones leave. Only its
 * thickness is its own: `size` px (width for left/right, height for
 * bottom), changed by dragging its free border; 0 keeps the current one.
 * Maximized windows, gui_desktop_rect and Tile/Cascade use what is left. */
enum gui_dock
{
    GUI_DOCK_NONE,
    GUI_DOCK_LEFT,
    GUI_DOCK_RIGHT,
    GUI_DOCK_BOTTOM,
};

void gui_window_set_dock(struct gui_node* win, enum gui_dock side, int size);
enum gui_dock gui_window_get_dock(const struct gui_node* win);

/* --- Context menus ---
 *
 * A right click on `n` (for a window: on its frame or anywhere no child
 * covers) opens `menu` (a GUI_MENU not in the menubar) at the mouse. While
 * it is open, and in the handler of the item picked, gui_context_target
 * says which node it was opened over. */
void gui_set_context_menu(struct gui_node* n, struct gui_node* menu);
/* A context menu with an id fires it as it opens - gui_context_target
 * already set - so the app can update its items (labels, enabled). */
struct gui_node* gui_context_target(const struct gui_app* app);

/* Opens `menu` right below the node `below` (a button, say), as if it were
 * its context menu: gui_context_target says `below` until it closes. */
void gui_popup_menu(struct gui_app* app, struct gui_node* menu, struct gui_node* below);
/* The same at (x, y), over `target`. */
void gui_popup_menu_at(struct gui_app* app, struct gui_node* menu, struct gui_node* target, int x, int y);

/* Puts `utf8` ('\n' line ends) on the system clipboard. */
void gui_set_clipboard(struct gui_app* app, const char* utf8);
/* One line in a box over everything, its top left at (x, y) px - moved up
 * or left to stay inside the window. NULL or "" hides it. */
void gui_set_tooltip(struct gui_app* app, int x, int y, const char* utf8);

void gui_window_set_modal(struct gui_node* win, int modal);  /* blocks everything else */
int gui_modal_open(const struct gui_app* app);   /* a modal window is open */
void gui_window_center(struct gui_app* app, struct gui_node* win);

/* A modal box with `text` (lines split on '
') and a row of buttons;
 * a button closes the box and fires its id. Sized to fit, centered. */
void gui_message_box(struct gui_app* app, const char* caption, const char* text,
                     const char* const labels[], const int ids[], int count);

/* --- Widgets --- */

/* GUI_TEXT, GUI_BOX; on a listbox's GUI_ITEM, `fg` colors just the label's
 * first glyph (a file-type marker), bg is unused. */
void gui_set_colors(struct gui_node* n, uint32_t fg, uint32_t bg);
void gui_set_value(struct gui_node* n, const char* utf8);           /* GUI_INPUT */
void gui_input_insert(struct gui_node* n, const char* utf8);        /* GUI_INPUT: replaces the selection, at the caret */
const char* gui_get_value(const struct gui_node* n);
void gui_set_selected(struct gui_node* n, int index);  /* GUI_SELECT, GUI_LISTBOX, radio GUI_GROUP */
int gui_get_selected(const struct gui_node* n);
void gui_set_multi(struct gui_node* group, int multi);  /* check boxes instead of radios */
void gui_set_checked(struct gui_node* group, int index, int checked);
int gui_get_checked(const struct gui_node* group, int index);
void gui_button_set_tab(struct gui_node* button, int selected);  /* drawn flat, as a tab */

/* Frees every child of `n` - to refill a listbox, for example. */
void gui_clear_children(struct gui_node* n);
int gui_child_count(const struct gui_node* n);
struct gui_node* gui_child_at(const struct gui_node* n, int i);

/* Keyboard focus; Tab / Shift+Tab move it inside the top window. */
void gui_focus(struct gui_app* app, struct gui_node* n);
struct gui_node* gui_focused(const struct gui_app* app);
/* The item a focused select, listbox or group is on (a group's keyboard row), else the focused node. */
struct gui_node* gui_focused_item(const struct gui_app* app);

/* --- Editor ---
 *
 * GUI_EDITOR holds UTF-8 text with '
' line ends, set and read with
 * gui_set_value / gui_get_value. Scrolling is always by whole lines and
 * columns. Colors come from a highlighter: for each line it gets the line's
 * bytes and the state the previous line ended in (0 for the first line),
 * fills spans and sets *state to the state this line ends in - so a block
 * comment can carry over. The editor keeps each line's start state and only
 * recomputes from the first edited line on. */
struct gui_span
{
    int start;     /* byte offset in the line */
    int len;       /* bytes */
    uint32_t fg;
};
/* In fg: the bytes are hidden in a read-only editor - no glyph, no column
 * (a rendered Markdown's delimiters). Shown, colored fg, while editing. */
#define GUI_SPAN_HIDDEN 0x01000000u

struct gui_highlighter
{
    /* Returns how many spans it filled (at most `max`); bytes no span covers
     * use the theme's editor_fg. */
    int (*highlight)(void* ctx, const char* line, int len, int* state,
                     struct gui_span* spans, int max);
    void* ctx;
    /* Optional: the row's background (0: the editor's), given the state the
     * line starts in - a Markdown code block's tint, say. */
    uint32_t (*row_bg)(void* ctx, const char* line, int len, int state);
};

void gui_editor_set_highlighter(struct gui_node* ed, const struct gui_highlighter* h);  /* NULL: plain */
void gui_editor_set_line_numbers(struct gui_node* ed, int on);  /* else a one-column margin */
void gui_editor_set_read_only(struct gui_node* ed, int read_only);
int gui_editor_get_read_only(const struct gui_node* ed);

/* `id` fires on a Ctrl+click in the editor - and on any click when it is
 * read-only and not VT100 - after the caret moved there. 0: none. */
void gui_editor_set_click_id(struct gui_node* ed, int id);

/* VT100 text, as a compiler prints it: "\x1b[...m" color codes take no
 * column and color what follows (each line starts in editor_output_fg),
 * on the theme's editor_output_bg; no caret is drawn. For an Output panel. */
void gui_editor_set_vt100(struct gui_node* ed, int on);

/* A git diff: "+" rows on editor_diff_add_bg, "-" rows on editor_diff_remove_bg,
 * and the line numbers are the new file's (none on a "-" row). */
void gui_editor_set_diff(struct gui_node* ed, int on);
int gui_editor_get_dirty(const struct gui_node* ed);      /* edited since set_value / set_dirty(0) */
void gui_editor_set_dirty(struct gui_node* ed, int dirty);
void gui_editor_goto_line(struct gui_node* ed, int line);  /* 1-based; caret there, line in view */
void gui_editor_goto_line_center(struct gui_node* ed, int line);  /* the same, line in the middle */

/* Marks: text shown right after a line's text ("Error Lens"), in the
 * theme's diag_* colors of the worst mark on that line; a line's marks are
 * drawn side by side, cut with "..." at the editor's border. They stay on
 * their line numbers until cleared, or until the text is edited (they were
 * about the text before the edit). A double click on an editor with an id
 * fires it (after selecting the word), so an app can act on the line. */
enum gui_mark
{
    GUI_MARK_INFO,
    GUI_MARK_WARNING,
    GUI_MARK_ERROR,
};

void gui_editor_add_mark(struct gui_node* ed, enum gui_mark type, int line, const char* utf8);  /* 1-based */

/* Breakpoints (1-based lines): an editor with line numbers draws a line
 * that has one with its number in the theme's editor_breakpoint_fg; a
 * click on the number toggles it. They stay on their line numbers. */
int gui_editor_toggle_breakpoint(struct gui_node* ed, int line);   /* 1 = now set */
int gui_editor_get_breakpoints(const struct gui_node* ed, int* out, int max);   /* how many */
void gui_editor_clear_breakpoints(struct gui_node* ed);

/* The line the debugger stopped on, in editor_exec_line_bg; 0 = none. */
void gui_editor_set_exec_line(struct gui_node* ed, int line);
void gui_editor_clear_marks(struct gui_node* ed);
void gui_editor_get_caret(const struct gui_node* ed, int* line, int* col);  /* 1-based */
/* The caret's cell, bottom left, in pixels - where a popup under it goes. */
/* The identifier under the mouse - with the fields it is in, "p->a.b" on
 * b - when it is over `ed`: copied to buf, 1
 * returned, (*x, *y) the px just below its first letter. Else 0. */
int gui_editor_word_at_mouse(const struct gui_app* app, const struct gui_node* ed,
                             char* buf, int cap, int* x, int* y);
void gui_editor_caret_point(const struct gui_app* app, const struct gui_node* ed, int* x, int* y);

/* The Edit menu's commands, for when they come from a menu rather than
 * the keyboard. */
void gui_editor_undo(struct gui_node* ed);
void gui_editor_redo(struct gui_node* ed);
void gui_editor_cut(struct gui_app* app, struct gui_node* ed);
void gui_editor_copy(struct gui_app* app, struct gui_node* ed);
void gui_editor_paste(struct gui_app* app, struct gui_node* ed);

/* The selection as byte offsets, lo <= hi (equal: just the caret). */
void gui_editor_get_selection(const struct gui_node* ed, int* lo, int* hi);
/* Selects [lo, hi) - the caret at hi - and brings it into view. */
void gui_editor_set_selection(struct gui_node* ed, int lo, int hi);
/* Replaces bytes [lo, hi) with `text`, as one undo step; selects the new text. */
void gui_editor_replace(struct gui_node* ed, int lo, int hi, const char* text);

/* --- Events --- */

/* `fn(ctx, id)` runs when a node with a non-zero id is activated (a menu
 * item clicked or its shortcut pressed). */
void gui_set_on_event(struct gui_app* app, void (*fn)(void* ctx, int id), void* ctx);

/* --- The app ---
 *
 * The framework owns main()/WinMain: it creates the window and the gui_app,
 * then calls gui_main once, where the app builds its tree. */
void gui_main(struct gui_app* app, int argc, char** argv);

#endif
