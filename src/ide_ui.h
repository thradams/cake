/* ui.h - the whole framework interface: an environment (ui_env) that hosts a
 * document (ui_screen) made of DOM nodes (ui_node), and the app interface a
 * backend drives to run one. Same relationship as the web: ui_env is the
 * `window` (renders, delivers input events), ui_screen/ui_node are the
 * document/DOM - build it programmatically (createElement/appendChild/
 * setAttribute-style functions), then a renderer walks the tree to paint it.
 * There is no intermediate cell buffer the tree gets flattened into -
 * rendering emits draw calls straight to the backend. Pure C,
 * platform-agnostic - no platform-specific types or headers here.
 *
 * Each frame the backend:
 *   1. posts input events into the ui_env (ui_env_post_event).
 *   2. calls app_frame(), which calls ui_screen_update() - this drains the
 *      env's event queue, hit-tests the tree against input, updates each
 *      node's retained hot/active state, and invokes the event handler
 *      (registered via ui_screen_set_on_event) for anything activated this
 *      frame (button clicked, menu item picked, ...), like Win32's WM_COMMAND
 *      being dispatched straight to a handler rather than pulled from a queue.
 *   3. calls app_render(), which calls ui_screen_render() with draw
 *      callbacks to paint the tree.
 */
#ifndef UI_H
#define UI_H

#include <stdint.h>
#include <time.h>

 /* --- Environment: a host that renders a document and delivers input events
  * into it, the same role the browser's `window` plays for a web page. It
  * owns an input event queue plus the screen's dimensions in character cells
  * - no drawing surface: the backend (Win32, termios, etc) posts events, and
  * the app renders by walking its own DOM tree. --- */

typedef struct ui_env ui_env;

/* Event types. */
enum {
    UI_EVENT_NONE = 0,
    UI_EVENT_KEY,
    UI_EVENT_MOUSE,
};

/* Special key codes (for `key.code`). ASCII printable chars use their code point;
 * these are for non-printable keys. */
enum {
    UI_KEY_ESCAPE = 256,
    UI_KEY_ENTER,
    UI_KEY_TAB,
    UI_KEY_BACKSPACE,
    UI_KEY_DELETE,
    UI_KEY_HOME,
    UI_KEY_END,
    UI_KEY_PAGEUP,
    UI_KEY_PAGEDOWN,
    UI_KEY_UP,
    UI_KEY_DOWN,
    UI_KEY_LEFT,
    UI_KEY_RIGHT,
    UI_KEY_INSERT,
    UI_KEY_F1,
    UI_KEY_F2,
    UI_KEY_F3,
    UI_KEY_F4,
    UI_KEY_F5,
    UI_KEY_F6,
    UI_KEY_F7,
    UI_KEY_F8,
    UI_KEY_F9,
    UI_KEY_F10,
    UI_KEY_F11,
    UI_KEY_F12,
};

/* Modifier flags (for `key.mods` and `mouse.mods`). */
#define UI_MOD_SHIFT  0x01
#define UI_MOD_CTRL   0x02
#define UI_MOD_ALT    0x04
#define UI_MOD_CMD    0x08  /* macOS Command (Win/X11 never set this) */

/* Whether macOS-style shortcut conventions are active: Cmd instead of Ctrl
 * for Copy/Cut/Paste/Undo/Select All, Cmd+Shift+Z instead of Ctrl+Y for
 * Redo, Option instead of Ctrl for word-at-a-time cursor movement
 * (Option/Ctrl+Left/Right), and Cmd instead of Ctrl in every menu <item>
 * shortcut declared via ui_set_shortcut() (e.g. "Ctrl+O" becomes Cmd+O).
 * Cmd+Left/Right/Up/Down for line/document start-end are macOS-only either
 * way, gated on this same setting.
 *
 * This defaults to on when built for macOS (__APPLE__) and off otherwise,
 * but it's a runtime setting, not a compile-time one - an app can call
 * ui_set_mac_shortcuts() to override it (e.g. from a Preferences toggle, or
 * to test one convention while running on the other platform). */
void ui_set_mac_shortcuts(int enable);
int ui_mac_shortcuts(void);

/* Mouse button and action. */
enum {
    UI_MOUSE_BUTTON_LEFT = 0,
    UI_MOUSE_BUTTON_RIGHT = 1,
    UI_MOUSE_BUTTON_MIDDLE = 2,
};

enum {
    UI_MOUSE_PRESSED = 0,
    UI_MOUSE_RELEASED = 1,
    UI_MOUSE_MOVED = 2,
    UI_MOUSE_DBLCLICK = 3,  /* the backend's own double-click detection, e.g.
                             * Win32's WM_LBUTTONDBLCLK - not synthesized here */
    UI_MOUSE_WHEEL = 4,     /* mouse.wheel_delta holds the scroll amount */
};

typedef struct {
    int type;  /* UI_EVENT_KEY, UI_EVENT_MOUSE, or UI_EVENT_NONE */
    union {
        struct {
            uint32_t codepoint;  /* Unicode code point (0 for special keys) */
            int code;            /* UI_KEY_*, or 0 for printable chars */
            int mods;            /* UI_MOD_* flags */
        } key;
        struct {
            int x, y;            /* screen grid coordinates */
            int button;          /* UI_MOUSE_BUTTON_* */
            int action;          /* UI_MOUSE_PRESSED, etc. */
            int mods;            /* UI_MOD_* flags */
            int wheel_delta;     /* UI_MOUSE_WHEEL only: +1 per notch up/away
                                   * from the user, -1 per notch down/toward */
        } mouse;
    } data;
} ui_event;

/* The starting terminal grid size (character cells) each backend (Win32/X11/
 * Cocoa) creates its ui_env and initial window at - one shared pair instead
 * of every backend duplicating its own DEFAULT_COLS/DEFAULT_ROWS #define, so
 * changing the app's starting size means editing this one place rather than
 * three. Plain variables (not a #define) so a backend could in principle
 * override them (e.g. from a saved setting) before calling ui_env_create(),
 * though none does today - each just passes these straight through. */
extern int ui_default_cols;
extern int ui_default_rows;

/* Create an environment with a w x h screen (character cells). Returns NULL
 * on failure. */
ui_env* ui_env_create(int w, int h);

/* Free an environment and its resources. Safe to call with NULL. */
void ui_env_free(ui_env* e);

/* The screen's dimensions in character cells - needed for hit-testing and
 * auto-layout (e.g. the status bar always sits on the last row) even though
 * there's no buffer backing them. */
int ui_env_width(ui_env* e);
int ui_env_height(ui_env* e);

/* Update the stored dimensions (e.g. on a window resize). */
void ui_env_resize(ui_env* e, int w, int h);

/* Feed the backend's wall-clock time (any monotonic ms count, e.g. Win32's
 * GetTickCount()) once per frame before ui_screen_update(). Used only to
 * drive the text-caret blink; if never called, the caret just stays solid. */
void ui_env_set_time_ms(ui_env* e, unsigned ms);

/* Post an event from the platform (keyboard/mouse). The environment queues
 * it for the app to poll. Safe to call from event handlers. */
void ui_env_post_event(ui_env* e, const ui_event* ev);

/* Font zoom: a backend-implemented action (resize the actual font, re-derive
 * cell size, resize the window) that the app can trigger uniformly - used by
 * both the backend's own Ctrl+/Ctrl- handling and the Window > Font Size +/-
 * menu items. `delta` is in the backend's own font-size units (e.g. points).
 * A backend registers its implementation once via ui_env_set_font_zoom_fn
 * (typically right after ui_env_create); ui_env_adjust_font_size() is what
 * callers (including the backend itself, for its own shortcut) use to invoke
 * it. A backend with no such concept (e.g. a plain terminal) just never
 * registers one - ui_env_adjust_font_size() is then a no-op. */
typedef void (*ui_font_zoom_fn)(void* ctx, int delta);
void ui_env_set_font_zoom_fn(ui_env* e, ui_font_zoom_fn fn, void* ctx);
void ui_env_adjust_font_size(ui_env* e, int delta);

/* Poll for an event. Returns 1 if an event was available and written to `out`,
 * 0 if the queue is empty. Non-blocking. */
int ui_env_poll_event(ui_env* e, ui_event* out);

/* --- Document: a DOM tree of ui_nodes, same model as HTML. --- */

typedef struct ui_screen ui_screen;   /* opaque */
typedef struct ui_node ui_node;       /* opaque - a DOM node */

/* Pack 8-bit R,G,B into a 0x00RRGGBB color. */
#define TB_RGB(r, g, b) \
    (((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b))

/* A frame style for anything box-drawing-bordered: <box>, a <window>, or a
 * dropdown/<select> popup. UI_BORDER_NONE draws no frame glyphs at all
 * (the bordered rect's edge cells are simply left blank, still painted in
 * the border color's background) - useful for a flat, borderless panel
 * look. */
enum ui_border_style {
    UI_BORDER_NONE,
    UI_BORDER_SINGLE,
    UI_BORDER_DOUBLE,
};

/* Number of colors in a theme's editor_bracket_fg[] cycle - see its own
 * comment below. */
#define UI_EDITOR_BRACKET_COLORS 4

/* The whole visual palette, one field per color role - every render_*()
 * function in ui.c reads its colors from here instead of hardcoded
 * constants, so a whole new look (a "theme") is just a different filled-in
 * ui_theme passed to ui_set_theme(), swappable at runtime (e.g. from a
 * menu item) without rebuilding anything. ui_get_theme() returns the
 * current one (starts out as the classic Turbo Vision palette) - to tweak
 * just a few colors, copy it, change those fields, and set it back. */
typedef struct {
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
    enum ui_border_style menu_border_style;  /* dropdown/<select> popup frame */

    uint32_t box_fg, box_bg;
    enum ui_border_style box_border_style;  /* <box>'s own frame */

    uint32_t window_border_fg, window_border_bg, window_border_fg_dragging,
        window_border_fg_unfocused, window_close_bg;
    enum ui_border_style window_border_style;  /* a focused/dragging <window>'s
                                                 * frame */
    enum ui_border_style window_border_style_unfocused;  /* an unfocused
                                                            * <window>'s frame */
    enum ui_border_style window_border_style_docked;  /* a focused/dragging
                            * docked window's frame (ui_set_dock != NONE) -
                            * e.g. the Output/Folder panels, which a theme
                            * may want styled differently from a floating
                            * document window */
    enum ui_border_style window_border_style_docked_unfocused;  /* an
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
    enum ui_border_style modal_border_style;
    uint32_t modal_fg, modal_bg;  /* a modal dialog's own body/interior fill -
                                    * distinct from its border (modal_border_*
                                    * above) */
    uint32_t scrollbar_bg, scrollbar_thumb_bg;  /* an <editor>'s or <listbox>'s
                                                  * own scrollbar overlay (see
                                                  * editor_has_vscrollbar/
                                                  * listbox_has_scrollbar in
                                                  * ide_ui.c) - the track's
                                                  * solid fill, and the thumb's
                                                  * solid fill on top of it */

    uint32_t input_bg, input_fg, input_fg_focus, input_sel_bg, input_sel_fg;

    uint32_t editor_bg, editor_fg, editor_keyword_fg, editor_keyword2_fg,
        editor_string_fg,
        editor_comment_fg, editor_preproc_fg, editor_sel_bg, editor_sel_fg;
    /* editor_keyword_fg colors type/storage keywords (is_c_keyword1 - int,
     * struct, const, ...); editor_keyword2_fg colors control-flow keywords
     * (is_c_keyword2 - if, for, return, ...). */

    uint32_t editor_current_line_bg;  /* the whole row the caret is on, in
                                       * every <editor> (source or VT100/
                                       * Output alike) - a subtle fill,
                                       * distinct from editor_sel_bg, so it
                                       * doesn't read as a real selection.
                                       * Only shown when there's no real
                                       * selection (see render_editor). */

    uint32_t editor_bracket_fg[UI_EDITOR_BRACKET_COLORS];  /* UI_SYNTAX_C
                                       * only: ( [ { ) ] } "rainbow bracket"
                                       * coloring - cycles by nesting depth
                                       * (depth % UI_EDITOR_BRACKET_COLORS),
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

    uint32_t listbox_fg, listbox_bg, listbox_sel_fg, listbox_sel_bg;
    uint32_t diag_error_fg;
    uint32_t diag_error_bg;
    uint32_t diag_warning_fg;
    uint32_t diag_warning_bg;
    uint32_t diag_info_fg;
    uint32_t diag_info_bg;

} ui_theme;

const ui_theme* ui_get_theme(void);
void ui_set_theme(const ui_theme* theme);

/* Element tags, needed by ui_create_element() below. TEXT/BOX are generic
 * content, not tied to any particular screen layout - everything else is a
 * widget. */
typedef enum {
    UI_TAG_SCREEN, UI_TAG_MENUBAR, UI_TAG_MENU, UI_TAG_ITEM,
    UI_TAG_STATUSBAR, UI_TAG_HOTKEY, UI_TAG_BUTTON,
    UI_TAG_TEXT, UI_TAG_BOX, UI_TAG_INPUT,
    UI_TAG_MODAL, UI_TAG_WINDOW, UI_TAG_EDITOR,
    UI_TAG_SELECT, UI_TAG_LISTBOX, UI_TAG_GROUP,
} ui_tag;

/* Create an empty screen (just a bare <screen> root, no children) to
 * populate programmatically via ui_screen_root() + ui_append_child(). */
ui_screen* ui_screen_create(void);

/* Free a screen and everything it owns. Safe to call with NULL. */
void ui_screen_free(ui_screen* s);

/* The screen's root <screen> node. Append top-level children to it
 * (<menubar>, <statusbar>, <button>, <box>, <text>) via ui_append_child(). */
ui_node* ui_screen_root(ui_screen* s);

/* --- DOM construction/editing. --- */

 /* Create a detached node of the given tag - like document.createElement().
  * Caller must attach it with ui_append_child() or free it with
  * ui_node_free(); a node that's never attached is just leaked otherwise. */
ui_node* ui_create_element(ui_tag tag);

/* Attribute setters - like element.setAttribute(...). id/label/color apply
 * to any node; the rect applies to <button>/<box>/<text> (menu/hotkey layout
 * is always auto-computed and ignores it). label is copied; the caller's
 * string need not outlive the call. */
void ui_set_id(ui_node* n, int id);
void ui_set_label(ui_node* n, const char* label);
void ui_set_rect(ui_node* n, int x, int y, int w, int h);

/* Reads back whatever ui_set_rect() last stored (or whatever the framework's
 * own layout - a window move/resize, a dock border drag, ui_window_maximize,
 * ...  - has since updated it to). Needed by app code that repositions a
 * <window>'s children on the fly (e.g. rebuilding a panel's content to a new
 * layout) and must place them relative to the window's CURRENT rect, not
 * whatever rect it was first created with - a docked window (ui_set_dock)
 * moves once ui_screen_update()'s first dock_layout pass runs, and its
 * existing children shift along with it (see set_window_rect in ide_ui.c),
 * but any node built fresh afterward has no such history to inherit and must
 * be told the real coordinates explicitly. Any out-param may be NULL. Safe to
 * call with n == NULL (writes 0/0/0/0). */
void ui_get_rect(const ui_node* n, int* x, int* y, int* w, int* h);

/* Whether screen cell (x, y) falls inside node `n`'s rect - a hit-test helper
 * for app code, since ui_node is opaque (its x/y/w/h aren't directly
 * reachable). Safe with n == NULL (returns 0). */
int ui_node_contains(const ui_node* n, int x, int y);
void ui_set_color(ui_node* n, uint32_t fg, uint32_t bg);

/* ITEM-only: a hint like "Ctrl+F9" or "F7" drawn right-aligned in the
 * dropdown, in its own accent color, distinct from the item's label. Also
 * functional: ui_screen_update() recognizes "F1".."F12" and "Ctrl+F1".."F12"
 * and fires this item's id whenever that key is pressed, regardless of
 * focus - any other text just never matches, so it stays purely decorative.
 * Always declare shortcuts in "Ctrl+..." form - when ui_mac_shortcuts() is
 * on, "Ctrl+" is both matched and displayed as "Cmd+" instead, so the same
 * string works as the native shortcut on either convention. */
void ui_set_shortcut(ui_node* n, const char* shortcut);
const char* ui_get_shortcut(const ui_node* n);

/* An opaque string an app can attach to any node - never read or rendered by
 * the framework itself. Handy for tagging a node with app-specific identity
 * (e.g. which file on disk backs a document window) without the framework
 * needing to know what that means. Defaults to "". */
void ui_set_path(ui_node* n, const char* path);
const char* ui_get_path(const ui_node* n);

/* ITEM-only: makes it a plain dividing line instead of a normal option -
 * label/shortcut/id are ignored, it draws as a single horizontal rule across
 * the dropdown's width, and (like a disabled item) it's never hot/active and
 * never fires - setting this also forces `enabled` off. Used to group
 * related items in a long menu, e.g. right before a destructive one like
 * "Exit". */
void ui_set_separator(ui_node* n, int separator);
int ui_get_separator(const ui_node* n);

/* Enable/disable any node - like the DOM's disabled attribute. A disabled
 * node is inert: hit_test_widget() (used by menu items, buttons, hotkeys)
 * skips it entirely, so it never becomes hot/active and never fires its id.
 * Rendering dims a disabled menu item's text; other tags keep their normal
 * look but simply stop responding to input. Nodes are enabled by default. */
void ui_set_enabled(ui_node* n, int enabled);
int ui_get_enabled(const ui_node* n);

/* Mark any node read-only - like the DOM's readonly attribute (distinct from
 * disabled: a read-only node stays focusable, selectable, and copyable, it
 * just can't be modified). INPUT/EDITOR are the tags that actually enforce
 * it - typing, Backspace/Delete, and paste are all no-ops on a read-only
 * one, though the caret still moves and Ctrl+C still copies. Available on
 * every tag (e.g. an app could use it as a plain flag on its own widgets),
 * but only INPUT/EDITOR give it behavior. Off by default. */
void ui_set_read_only(ui_node* n, int read_only);
int ui_get_read_only(const ui_node* n);

/* WINDOW-only: whether it can be resized (bottom-right corner drag) and
 * maximized/restored (title-bar icon, double-click). Off by default - most
 * dialogs have a fixed layout that was never designed to reflow; opt in
 * per window for ones that were, like a document/editor window. */
void ui_set_resizable(ui_node* n, int resizable);
int ui_get_resizable(const ui_node* n);

/* MODAL (a top-level window wrapper) only: whether closing it should tear
 * the whole subtree down for good, instead of just hiding it for a later
 * ui_screen_show_window() to bring back as-is. Off by default, matching
 * every existing window/dialog/docked panel's "reusable singleton"
 * behavior - opt in per wrapper for one-off windows meant to be freed when
 * closed, like a document/editor window (see make_editor_window). A closed
 * transient window is detached from the tree lazily, not inside
 * ui_screen_close_modal() itself (freeing it synchronously mid-frame, while
 * the app may still hold its own pointer to it - e.g. "the active editor
 * window" - would leave that pointer dangling for the rest of the frame) -
 * see ui_screen_take_closed_window(). */
void ui_set_transient(ui_node* n, int transient);
int ui_get_transient(const ui_node* n);

/* MODAL (a document window wrapper) only: whether it's still at its original
 * never-saved placeholder path (e.g. "NONAME00.C" - see make_new_editor_
 * window) rather than a real file the user picked/opened. Off by default,
 * and off again as soon as the app commits a real Save As target (see
 * save_as_commit) - a File > Open'd window never has this set in the first
 * place. Meant purely as a flag for the app's own File > Save / Compile
 * logic to check ("does this window need a Save As target before it can be
 * written to disk for real?"); the framework itself never reads it. */
void ui_set_untitled(ui_node* n, int untitled);
int ui_get_untitled(const ui_node* n);

/* Returns the most recently closed transient window (see ui_set_transient)
 * and clears the pending slot, or NULL if none is pending - meant to be
 * polled once per frame, right after ui_screen_update(), before anything
 * else runs that might read a stale reference to it. The caller (the app)
 * is responsible for the actual teardown: clear any of its own globals that
 * might still point at the returned node, then ui_remove_child() it from
 * wherever it was attached (always ui_screen_root() for a document window)
 * and ui_node_free() it. Returns the same window only once - a second call
 * this frame (or any later one, until another transient window closes)
 * returns NULL. */
ui_node* ui_screen_take_closed_window(ui_screen* s);

/* WINDOW-only: whether it casts a drop shadow - on by default, like every
 * other shadow-casting widget (button/box/menu dropdown/modal dialog). Turn
 * it off for windows that shouldn't look "raised" above the document, like a
 * floating document/editor window sitting alongside ordinary content rather
 * than popped up over it. */
void ui_set_shadow(ui_node* n, int shadow);
int ui_get_shadow(const ui_node* n);

/* WINDOW only: whether it's currently maximized - see ui_window_maximize. */
int ui_get_maximized(const ui_node* n);

/* WINDOW-only: docking a window to an edge of the desktop instead of it
 * floating freely - the shape a persistent Folder/Output panel needs (see
 * turboc_demo.c's g_folder_window/g_output_window). A docked window:
 *   - is never dragged, corner-resized, or maximized/restored - it has no
 *     zoom icon, and double-clicking its border row does nothing;
 *   - always spans the full extent of its one axis: UI_DOCK_BOTTOM spans the
 *     full screen width along the bottom edge; UI_DOCK_LEFT/RIGHT span from
 *     just under the menu row down to just above Output (if it's docked and
 *     visible) or the status row otherwise;
 *   - is resizable (if ui_set_resizable is also on) only along its one free
 *     border - the right edge for UI_DOCK_LEFT, the left edge for
 *     UI_DOCK_RIGHT, the top edge for UI_DOCK_BOTTOM - dragging it changes
 *     that dock's thickness (`size`, in cells: width for a side dock, height
 *     for the bottom one);
 *   - still has a working close icon, which just hides it (see
 *     ui_screen_close_modal) - re-showing it later (ui_screen_show_window)
 *     restores it to its dock, not wherever it last floated.
 * Every OTHER window that's maximized (see ui_window_maximize, or the zoom
 * icon/double-click) automatically fills only the desktop area left over
 * once docked windows are subtracted - see ui_screen_desktop_rect - and
 * re-fills it immediately whenever a dock is shown, hidden, or resized.
 *
 * `size` is the initial/starting thickness in cells; pass 0 to leave the
 * window's current width/height as its thickness (e.g. just changing which
 * side it's docked to). Pass UI_DOCK_NONE to undock back to an ordinary
 * floating window (its rect is left as-is - follow with ui_window_set_rect
 * if you want it somewhere specific). */
typedef enum {
    UI_DOCK_NONE = 0,
    UI_DOCK_LEFT,
    UI_DOCK_RIGHT,
    UI_DOCK_BOTTOM,
} ui_dock_side;

void ui_set_dock(ui_node *n, ui_dock_side side, int size);
ui_dock_side ui_get_dock(const ui_node *n);

/* EDITOR-only: how its content is colored while drawing - see ui_set_syntax.
 * NONE is the fallback (plain text, no coloring at all - e.g. a ".txt" file);
 * C highlights keywords/strings/comments/preprocessor directives; MARKDOWN
 * highlights headings/blockquotes/code spans/fenced code blocks; VT100
 * interprets embedded VT100 SGR color codes instead of coloring by syntax at
 * all (e.g. captured compiler/terminal output, not source text). */
typedef enum {
    UI_SYNTAX_NONE,
    UI_SYNTAX_C,
    UI_SYNTAX_MARKDOWN,
    UI_SYNTAX_VT100,
} ui_syntax;

/* Sets an EDITOR's syntax color mode (see ui_syntax above). Defaults to
 * UI_SYNTAX_NONE. Setting UI_SYNTAX_VT100 also forces the node read-only
 * (see ui_set_read_only) - captured terminal/compiler output is never
 * hand-edited, so there's no path back to plain-text editing once set. */
void ui_set_syntax(ui_node* n, ui_syntax syntax);
ui_syntax ui_get_syntax(const ui_node* n);

/* EDITOR-only: whether it has unsaved changes - like a document's "modified"
 * flag. The framework sets this to 1 on its own (any edit - typing, paste,
 * cut, Delete/Backspace...); it never clears it on its own, since it has no
 * notion of "saved" - the app calls ui_set_dirty(n, 0) once it's actually
 * persisted the content (e.g. from a File > Save handler). Shown as a "*"
 * after the title in the window's border while set. */
void ui_set_dirty(ui_node* n, int dirty);
int ui_get_dirty(const ui_node* n);

/* EDITOR-only: undo/redo the last content-changing edit (typing, Enter,
 * paste, cut, Delete/Backspace). A contiguous run of the same kind of edit -
 * a burst of typing, a held-down Backspace, one paste's worth of inserted
 * text - collapses into a single undo step; anything else (a different kind
 * of edit, a cursor jump in between, or pausing longer than the framework's
 * short coalescing window) starts a new one, same grouping a normal text
 * editor does. Pressing Enter is always its own step and never merges with
 * surrounding typing, matching editors like Visual Studio. ui_editor_undo()
 * moves the current state onto the redo stack before restoring the previous
 * one, so ui_editor_redo() can bring it back; any new edit after an undo
 * discards whatever was sitting in redo, same as everywhere else. Both are a
 * no-op if there's nothing to undo/redo, or on any tag other than EDITOR.
 * Loading new content via ui_set_value() (e.g. opening a different file into
 * the same editor) clears both stacks - the history belongs to the document
 * that was open, not the widget. */
void ui_editor_undo(ui_node* n);
void ui_editor_redo(ui_node* n);
int ui_editor_can_undo(const ui_node* n);
int ui_editor_can_redo(const ui_node* n);

/* EDITOR-only: move the caret to the start of the given 1-based line
 * (clamped to [1, line count]), clear any selection, and scroll to bring it
 * into view - what a "Go to line" dialog needs. No-op on other tags. */
void ui_editor_goto_line(ui_node* n, int line);

/* EDITOR-only: the 1-based line the caret is currently on (0 on other tags).
 * A mouse click positions the caret, so a double-click handler (an <editor>
 * fires its id on double-click) can read back which line was clicked. */
int ui_editor_caret_line(const ui_node* n);

/* EDITOR-only cursor/selection access by byte offset into the value - the
 * hooks a find/replace feature needs to locate matches and highlight them.
 * ui_editor_get_cursor() returns the caret's byte offset; ui_editor_get_
 * selection() writes the selected [lo,hi) byte range and returns 1, or
 * returns 0 if nothing is selected; ui_editor_set_selection() selects
 * [start,end) (start==end just moves the caret, clearing any selection) and
 * scrolls to bring the caret into view. */
int ui_editor_get_cursor(const ui_node* n);
int ui_editor_get_selection(const ui_node* n, int* lo, int* hi);
void ui_editor_set_selection(ui_node* n, int start, int end);

/* EDITOR-only: replace the byte range [lo,hi) with new_text as a single
 * undoable edit - the programmatic equivalent of selecting [lo,hi) and
 * pasting new_text over it (same delete-selection + insert grouping
 * ui_screen_paste() uses), so a single ui_editor_undo() call reverts the
 * whole replacement. For an Edit menu command that transforms the current
 * selection (Stringify, To Upper/Lower, and similar): read the selection
 * with ui_editor_get_selection(), build new_text, then call this instead
 * of ui_set_value() so the edit stays undoable. Leaves the replaced range
 * selected, same as the transform commands did before. A no-op if n isn't
 * an EDITOR or is read-only. */
void ui_editor_replace_selection(ui_node* n, int lo, int hi, const char* new_text);

/* EDITOR-only: the index of the topmost visible line - lets a caller save
 * and restore the scroll position (e.g. across a ui_set_value() that reloads
 * content, which would otherwise reset it to the top) exactly, without the
 * recentering ui_editor_set_selection()'s "scroll to bring the caret into
 * view" would do. ui_editor_set_scroll() clamps to the document's actual
 * line count, same as scrolling normally would. */
int ui_editor_get_scroll(const ui_node* n);
void ui_editor_set_scroll(ui_node* n, int scroll);

/* INPUT-only: restrict typing to digits 0-9 - any other key press is
 * ignored, so the field can only ever hold a number (e.g. a line-number
 * prompt). Off by default. */
void ui_set_numeric(ui_node* n, int numeric);
int ui_get_numeric(const ui_node* n);

/* Attribute getters. */
int ui_get_id(const ui_node* n);
const char* ui_get_label(const ui_node* n);

/* value is <input>'s current text - like element.value. It's an alias for
 * label/ui_set_label under the hood (both are just "the node's string"), but
 * named to match the web analogy at the call site. Setting it also moves the
 * caret to the end, like assigning .value in JS.
 *
 * Also used for <editor> - a multi-line text area with basic C syntax
 * highlighting (keywords, "strings", // comments, #preprocessor lines).
 * Lines are just '\n' bytes embedded in the same value string; typing Enter
 * inserts one instead of firing an event/blurring like it does for <input>.
 * Up/Down move by line, Home/End by the current line (not the whole value).
 * Needs explicit x/y/w/h like <box> - it's the visible viewport, scrolling
 * vertically to keep the caret's line in view (there's no horizontal scroll
 * yet - a line longer than w is simply clipped). */
void ui_set_value(ui_node* n, const char* value);
const char* ui_get_value(const ui_node* n);

/* <select> - a combo box: a single-line box showing the currently selected
 * option, with a "v" button on its right edge that opens a popup list of its
 * <item> children (same tag/shape a <menu>'s options use) below it. Clicking
 * an option there picks it (closing the popup, updating the visible text,
 * and firing that specific <item>'s own id - like a menu item) and moving
 * on. Build it the same way as a <menu>'s items - ui_create_element(),
 * ui_set_label()/ui_set_id() on each <item>, ui_append_child() onto the
 * <select>. Needs explicit x/y/w/h, height always 1 (it never grows to fit
 * its options - only the popup does).
 *
 * Also the getter/setter for <listbox>'s current row below - both tags just
 * keep their selection as a plain child index, so one pair of functions
 * serves either. */
void ui_select_set_selected(ui_node* select, int index);
int ui_select_get_selected(const ui_node* select);

/* <listbox> - an always-visible, scrollable list of single-line <item>
 * children (rebuild the list with ui_remove_child()+ui_node_free() on each
 * old child, then ui_create_element(UI_TAG_ITEM)+ui_set_label()+
 * ui_append_child() per new row - the same shape a <select>'s popup uses,
 * just always on-screen instead of behind a button). Clicking a row selects
 * it (see ui_select_get/set_selected above); double-clicking one, pressing
 * Enter while focused, or the mouse wheel over it, respectively activate it
 * (fires the <listbox>'s own id - the app reads back which row via
 * ui_select_get_selected), move the selection, or scroll the view - like a
 * native single-select list box. Needs explicit x/y/w/h; h is how many rows
 * show at once, independent of how many <item> children there are. */

/* GROUP - a vertical cluster of <item> choices, drawn one per row with a
 * marker before each label. One element serves two modes, chosen with
 * ui_set_multi():
 *   - single-select (multi=0, the default): a radio-button group - exactly
 *     one item chosen at a time, drawn "(*) Label" / "( ) Label". The choice
 *     is the group's selected index (ui_select_get/set_selected, same as
 *     <select>/<listbox>).
 *   - multi-select (multi=1): a check-box group - each item independently
 *     on/off, drawn "[X] Label" / "[ ] Label". Per-item state is read/written
 *     with ui_group_get/set_checked().
 * Either way clicking a row (or Space on the keyboard-focused row; arrows move
 * within the group) activates it and fires the group's own id. Built like a
 * <select>/<listbox>: ui_create_element(UI_TAG_GROUP), one
 * ui_create_element(UI_TAG_ITEM)+ui_set_label() per choice, ui_append_child()
 * each onto the group. Needs explicit x/y/w/h; one row per item, so h is the
 * item count. */
void ui_set_multi(ui_node* group, int multi);
int ui_get_multi(const ui_node* group);
void ui_group_set_checked(ui_node* group, int index, int checked);
int ui_group_get_checked(const ui_node* group, int index);

 /* Tree editing - like parent.appendChild(child) / child.remove(). Appending
  * a node already attached elsewhere is undefined (detach it first). */
void ui_append_child(ui_node* parent, ui_node* child);
void ui_remove_child(ui_node* parent, ui_node* child);  /* detaches; doesn't free */
int ui_child_count(const ui_node* n);
ui_node* ui_child_at(const ui_node* n, int i);

/* Recursively free a detached node and its subtree. Do not call this on a
 * node that's still attached to a screen's live tree. */
void ui_node_free(ui_node* n);

/* Depth-first search for the first node with the given id anywhere under
 * `root` (inclusive) - like getElementById(). Returns NULL if not found. */
ui_node* ui_find_by_id(ui_node* root, int id);

/* The screen's cursor: a visible marker (not a DOM node, since it's a single
 * screen-wide concept, like the browser's caret rather than an element).
 * Drawn as the final overlay step of a render pass by inverting whatever's
 * underneath (see UI_BOX_INVERT) - a true XOR cursor needs no color of
 * its own, since inverting is automatically legible over anything: green
 * inverts to pink, gray inverts to near-black, white to black, etc. */
void ui_screen_set_cursor(ui_screen* s, int x, int y, int on);

/* The desktop backdrop - a solid fill covering the whole screen, behind
 * everything else. Off by default (the screen just shows whatever the
 * backend clears to); once set, it stays on and repaints every frame as the
 * bottom of the whole stack, so every node still paints over it normally. */
void ui_screen_set_desktop(ui_screen* s, uint32_t bg);

/* Event handler - called synchronously, straight from ui_screen_update(),
 * for every widget activated during that update. `ctx` is passed through
 * unchanged from ui_screen_set_on_event(). A widget with no "id" attribute
 * never fires (id 0). `param` is an extra caller-supplied payload alongside
 * the id - NULL for every event the framework fires today (see
 * ui_fire_event); it exists so a future firing site (or an app-level
 * ui_fire_event-alike) can pass something id alone can't carry, e.g. which
 * window a menu command should act on, without the handler having to
 * re-derive "the active one" itself. */
typedef void (*ui_event_fn)(void* ctx, int id, void* param);

/* Register the handler ui_screen_update() delivers fired widget ids to.
 * Replaces any previously registered handler. Pass fn=NULL to stop receiving
 * events (they're then silently dropped, same as an unset handler). */
void ui_screen_set_on_event(ui_screen* s, ui_event_fn fn, void* ctx);

/* Reserved id fired through the same on_event handler (see ui_screen_set_
 * on_event) instead of an ordinary close, when a transient window's (see
 * ui_set_transient) close icon is clicked while it has a dirty EDITOR
 * descendant (see ui_set_dirty) - closing right away would silently discard
 * unsaved changes, so the framework defers to the app rather than calling
 * ui_screen_close_modal() itself. `param` is the window (the MODAL wrapper -
 * exactly what ui_screen_close_modal() expects); the app's handler decides
 * what to do (e.g. show a Discard/Cancel prompt) and calls
 * ui_screen_close_modal() itself once/if the user confirms discarding.
 * Negative so it can never collide with an app's own (always >= 0 by
 * convention) widget ids. Never fired for a non-transient window (a docked
 * panel/dialog) or one with no unsaved changes - those still just close
 * immediately, exactly as before. */
#define UI_CLOSE_REQUEST_ID (-1)

/* Keyboard focus - like element.focus() / document.activeElement. Only
 * <input> is focusable today. While a node is focused, ui_screen_update()
 * routes printable/editing key events into it (typing, arrow keys, Home/
 * End, Backspace/Delete) instead of leaving them for the app to read via the
 * passthrough accessors below; Enter fires the input's id then blurs it,
 * Escape blurs without firing. Clicking anywhere else blurs automatically. */
void ui_screen_focus(ui_screen* s, ui_node* n);
ui_node* ui_screen_focused(ui_screen* s);

/* Text selection and clipboard - <input> and <editor> both support a mouse-
 * drag or Shift+arrow/Home/End selection (see ui_screen_update()); Ctrl+C/X/V
 * on the focused node do the same thing these three call directly, so an
 * Edit menu's Copy/Cut/Paste items can be wired straight to them:
 *   ui_screen_set_on_event(...) { if (id == EVT_EDIT_COPY) ui_screen_copy(s); }
 * Each is a no-op if nothing is focused (copy/cut) or there's no selection
 * (cut) or the clipboard has no text (paste).
 *
 * ui_clipboard_set_text()/ui_clipboard_get_text() are the two hooks the
 * backend must provide (Win32: OpenClipboard/SetClipboardData with
 * CF_UNICODETEXT, converting to/from UTF-8 at the boundary) - ui.c calls them
 * but never talks to the OS clipboard itself, same separation as the
 * ui_draw_*_fn callbacks. ui_clipboard_get_text() returns a malloc'd string
 * the caller must free(), or NULL if the clipboard has no text. */
void ui_screen_copy(ui_screen* s);
void ui_screen_cut(ui_screen* s);
void ui_screen_paste(ui_screen* s);

/* Same pattern, for an Edit menu's Undo/Redo items - wraps ui_editor_undo/
 * redo around the focused node:
 *   ui_screen_set_on_event(...) { if (id == EVT_EDIT_UNDO) ui_screen_undo(s); }
 * A no-op with nothing focused or nothing to undo/redo. */
void ui_screen_undo(ui_screen* s);
void ui_screen_redo(ui_screen* s);

void ui_clipboard_set_text(const char* utf8);
char* ui_clipboard_get_text(void);

/* --- Filesystem: directory listing for a file-open/save dialog, the other
 * OS-dependent thing (besides the clipboard) an app might need that C has
 * no portable API for. Same separation as ui_clipboard_*: the backend
 * implements ui_list_dir() (Win32: FindFirstFileW/FindNextFileW; POSIX:
 * opendir/readdir+stat), ui.c/the app never touch the OS directly. --- */

typedef struct {
    char name[256];   /* filename only, no path */
    int is_dir;
    long long size;   /* bytes; meaningless (0) for directories */
    time_t mtime;      /* last-modified, local time - format with strftime */
} ui_dir_entry;

/* Lists every entry in `path` (except "." and ".." - the app decides how,
 * or whether, to offer "go up a directory" itself) into `out`, up to
 * `max_entries`. Returns the number of entries written (0 for an empty
 * directory), or -1 if `path` couldn't be opened/read. No filtering or
 * sorting - wildcard masks, dirs-first-then-alphabetical, etc. are a
 * display policy, not a filesystem one, and belong in the app. */
int ui_list_dir(const char* path, ui_dir_entry* out, int max_entries);

/* Fills `buf` (of size buf_size, bytes) with the process's current working
 * directory as a UTF-8 path, NUL-terminated. Returns 1 on success, 0 if it
 * didn't fit or the OS call failed. An app navigating "up" a directory by
 * stripping path components (see e.g. an Open-File dialog) needs to start
 * from a real absolute path - starting from the symbolic "." instead has no
 * parent to strip once back at it, a dead end one level up. */
int ui_get_cwd(char* buf, int buf_size);

/* Opens a new native terminal window with its working directory set to
 * `dir` (the app's own cwd if `dir` is NULL/empty) - Tools > Terminal (see
 * ide.c's do_open_terminal). A fire-and-forget launch: the terminal runs
 * completely independently afterward, this app never manages it as a child
 * process. Each backend picks whatever "the" terminal means on its platform
 * (%COMSPEC%'s cmd.exe on Windows, Terminal.app on macOS, the first of a
 * handful of common emulators found on Linux/X11). */
void ui_open_terminal(const char* dir);

/* Modal dialogs and floating windows share the same <modal>/<window> DOM
 * shape and the same drag/resize/maximize/close chrome - they only differ in
 * whether they block the rest of the document. A <modal> node is always
 * invisible - it exists purely to gate input (or not); its <window> child
 * (bordered panel, same look as <box>, plus a drop shadow, draggable by its
 * title row, resizable from its bottom-right corner, closable/maximizable
 * via its title-bar icons) is what actually renders, always as the topmost
 * layer regardless of where the modal sits in the document.
 *
 * ui_screen_show_modal(): a blocking dialog. Only the window's own children
 * (button, input, ...) are clickable/focusable/tabbable while it's active -
 * everything else (menubar, statusbar, other top-level nodes, and any
 * floating window shown via ui_screen_show_window()) is blocked, like a
 * native modal dialog trapping input. Blocking dialogs are kept on a stack:
 * showing one on top of another (modal calling modal) pushes it, and closing
 * it reveals the previous one again, still active where it left off. The
 * floating window (if any) is a completely separate slot - opening or
 * closing a modal never closes or otherwise touches it.
 *
 * ui_screen_show_window(): a non-blocking floating window (e.g. a document/
 * editor window). Its own content and chrome work exactly the same way, but
 * clicks that land outside it still reach the menubar/statusbar/other
 * top-level nodes underneath, like a normal (non-modal) window. There is
 * only ever one floating window at a time; it stays open across modal
 * dialogs being shown/closed on top of it.
 *
 * Either way, showing one closes any open dropdown and blurs any focused
 * input, since those are covered/blocked anyway. Typically triggered from a
 * menu item's fired id, e.g. in the app's event handler:
 *   ui_screen_show_modal(screen, ui_find_by_id(root, MODAL_ID));
 * and a "Close" button inside the window (or its own close icon) calling
 * ui_screen_close_modal(screen, that_same_modal) the same way. Escape also
 * closes whichever container (modal stack top, or the floating window if no
 * modal is active) is currently active. */
void ui_screen_show_modal(ui_screen* s, ui_node* modal);
void ui_screen_show_window(ui_screen* s, ui_node* modal);
void ui_screen_close_modal(ui_screen* s, ui_node* modal);
ui_node* ui_screen_active_modal(ui_screen* s);

/* A Windows-MessageBox-style dialog: a caption (title bar), a body message,
 * and a row of buttons - built, centered, and shown in one call. `text` may
 * contain '\n' for multiple lines; the box auto-sizes to fit the widest line,
 * the button row, and the caption. Each button carries its own event id;
 * clicking one delivers that id to the app's event handler (like any widget)
 * and then the framework closes AND frees the whole box (so the caller need
 * not retain or free it). Escape also dismisses it (firing no id). Because it
 * frees itself, do not keep the node around or pass it to ui_screen_*. */
typedef struct {
    const char* label;
    int id;         /* fired when this button is pressed */
} ui_msgbox_button;

void ui_message_box(ui_screen* s, const char* caption, const char* text,
                    const ui_msgbox_button* buttons, int count);

/* The frontmost floating window shown via ui_screen_show_window() - the one
 * a click promotes to the front, i.e. the "active" one - or NULL if none is
 * open. Returns the same wrapper node passed to ui_screen_show_window(). */
ui_node* ui_screen_top_window(ui_screen* s);

/* Open floating windows (those shown via ui_screen_show_window), in back-to-
 * front z-order - for window-manager actions like Tile/Cascade/Close All.
 * ui_screen_window_at() returns the wrapper node passed to show_window (its
 * <window> child is at child index 0). */
int ui_screen_window_count(ui_screen* s);
ui_node* ui_screen_window_at(ui_screen* s, int i);

/* The screen's current size in character cells (cached each ui_screen_update
 * from the env), so app-side layout (tiling windows, etc.) needn't thread the
 * env through. */
int ui_screen_width(ui_screen* s);
int ui_screen_height(ui_screen* s);

/* Move/resize a <window> node and its whole subtree (children shift/stretch
 * with it) - the public entry for app window management. Pass the wrapper's
 * <window> child (ui_child_at(wrapper, 0)). Clears any maximized state. */
void ui_window_set_rect(ui_node* window, int x, int y, int w, int h);

/* The current desktop rect: the screen minus the menu row (top) and status
 * row (bottom), and minus any visible docked window (see ui_set_dock) - the
 * area a maximized floating window fills (and what ui_window_maximize()
 * below uses). Re-derive after showing/hiding/resizing a dock to lay out
 * app-side content that isn't itself a <window> node. */
void ui_screen_desktop_rect(ui_screen* s, int* out_x, int* out_y, int* out_w, int* out_h);

/* Maximize a <window> node immediately, filling the current desktop rect
 * (see ui_screen_desktop_rect) - the same result toggling its zoom icon
 * would produce, but usable right when a window is created so it opens
 * already filling that space (per-frame tracking then keeps it filling that
 * space exactly like any other maximized window - see ui_set_dock) instead
 * of appearing at whatever rect it was created with and waiting for a
 * manual maximize. Saves the window's current geometry as the "restore"
 * rect for a later un-maximize, same as a manual maximize would. No-op on a
 * docked window (see ui_set_dock) - those don't maximize. */
void ui_window_maximize(ui_screen* s, ui_node* window);

/* Open a <menu> as a popup (context) menu with its box's top-left at (x, y),
 * instead of under a menubar header - the same dropdown otherwise, driven by
 * the identical hit-testing/rendering (submenus, shortcuts, hover, and
 * click-elsewhere/Escape to close all work). The menu node is built like a
 * menubar's <menu> (ui_create_element(UI_TAG_MENU) + <item> children) but
 * lives outside the menubar; its items fire their ids the same way. Typically
 * called on a right-click - see ui_screen_mouse_right_pressed(). `param` is
 * delivered as-is (see ui_event_fn) when one of the popup's own items fires -
 * e.g. the window it was opened for - so a handler doesn't need to stash that
 * in a global just to read it back if the item is picked. Pass NULL if the
 * popup's handlers don't need one. */
void ui_screen_open_popup(ui_screen* s, ui_node* menu, int x, int y, void* param);

/* Call once per frame, before rendering. Drains env's event queue:
 * hit-tests the tree against the mouse, updates each node's hot/active state
 * and the open dropdown (if any), and calls the registered event handler for
 * every widget activated this frame. Also records raw mouse/key state for
 * the passthrough accessors below. */
void ui_screen_update(ui_screen* s, ui_env* env);

/* Draw primitives a backend implements as plain global functions (not
 * callbacks - see win32.c/x11.c/cocoa.c) - ui.c calls these directly while
 * walking the tree. Exactly one backend's implementation is ever linked
 * into a given build, so there's no need to pass them in as parameters:
 * ui_draw_char paints one glyph cell; ui_draw_box (below) fills a rect. */
void ui_draw_char(int x, int y, uint32_t ch, uint32_t fg, uint32_t bg);

/* A solid w x h rect, drawn in a single call instead of a per-cell
 * ui_draw_char loop - one plain rect fill (Win32's FillRect, X11's
 * XFillRectangle, Cocoa's CGContextFillRect), never a glyph/text draw. Used
 * for every solid fill ui.c used to build out of repeated ui_draw_char
 * calls: backgrounds, blank menu/listbox/statusbar rows, a <box>'s
 * interior, etc. - every such fill is a blank cell, so there's no glyph to
 * draw one-by-one in the first place. `fg` is carried only for symmetry
 * with ui_draw_char's signature and is unused by every backend today.
 *
 * `flags` selects the fill's flavor - normal (0, fills with `bg`),
 * UI_BOX_INVERT (bg ignored - invert the rect in place, dest = NOT dest,
 * e.g. Win32's PatBlt DSTINVERT - the XOR-style screen cursor's overlay), or
 * UI_BOX_SHADOW (bg ignored - darken the rect via alpha blending, a shadow
 * hint; backends without true alpha blending may just no-op it). A node
 * needing a shadow (button, open dropdown) calls ui_draw_box with
 * UI_BOX_SHADOW right after its own face, so the shadow composites over
 * whatever's already painted beneath/around it.
 *
 * The UI_BOX_SHADOW_SHRINK_* bits (meaningful only with UI_BOX_SHADOW) say
 * which pixel dimension(s) of the rect are the shadow's half-cell thickness
 * (the classic Turbo Vision shadow is only half a cell thick, hugging the
 * widget rather than filling a full extra row/column): SHRINK_W for a
 * vertical strip beside a widget, SHRINK_H for a horizontal strip below it
 * (keeping the TOP half of the cell, since it hugs the widget just above
 * it), SHRINK_H_BOTTOM the same but keeping the BOTTOM half instead (for the
 * row where a side strip tapers into the corner), and SHRINK_W together with
 * either H flag for the single corner cell where two strips meet (it needs
 * shrinking on both axes at once - it's not just "part of" either strip). */
#define UI_BOX_INVERT 0x1
#define UI_BOX_SHADOW 0x2
#define UI_BOX_SHADOW_SHRINK_W 0x4
#define UI_BOX_SHADOW_SHRINK_H 0x8
#define UI_BOX_SHADOW_SHRINK_H_BOTTOM 0x10
void ui_draw_box(int x, int y, int w, int h, uint32_t fg, uint32_t bg, int flags);

/* Walk the tree in document order (later siblings paint over earlier ones -
 * the same top-to-bottom painting order as HTML) and emit ui_draw_char calls
 * for every visible node (ui_draw_box for solid fills, shadows, and the
 * cursor overlay - see above), then the open dropdown (if any) as a top
 * layer, then the cursor last.
 *
 * Incremental: ui_screen keeps a per-cell shadow of what it last emitted, so
 * a cell whose (char, colors) are unchanged since the previous render is
 * skipped entirely - no ui_draw_char/ui_draw_box call reaches the backend
 * for it. The union of every cell that DID change this frame is the
 * "invalidate rect" (Windows-style): written to *x,*y,*w,*h in CHARACTER
 * CELL coordinates, it's the only region the backend needs to blit to the
 * screen. Returns 1 if anything changed (rect valid, non-empty), 0 if the
 * frame was visually identical to the last (rect untouched - the backend
 * should skip its blit entirely, the common case for an idle window). The
 * out-params may be NULL if the caller doesn't need the rect. */
int ui_screen_render(ui_screen* s, int* x, int* y, int* w, int* h);

/* Force the whole screen to repaint on the next ui_screen_render, discarding
 * the per-cell shadow - the backend calls this (via app_invalidate) whenever
 * its own backing store no longer matches what the shadow believes is on
 * screen: a bitmap/pixmap it just (re)created, or a font reload that changed
 * every cell's pixel size/position. */
void ui_screen_invalidate(ui_screen* s);

/* Raw input passthrough, for app logic that lives outside the widget tree
 * (e.g. a free-roaming demo cursor, a global "press Q to quit"). Mirrors what
 * the old immediate-mode ui_context exposed. Valid until the next
 * ui_screen_update() call. */
int ui_screen_mouse_x(ui_screen* s);
/* Whether the right mouse button was pressed this frame (position via
 * ui_screen_mouse_x/y) - the trigger for a context-menu popup. */
int ui_screen_mouse_right_pressed(ui_screen* s);
int ui_screen_mouse_y(ui_screen* s);
int ui_screen_mouse_moved(ui_screen* s);
int ui_screen_key_count(ui_screen* s);
ui_event ui_screen_key_event(ui_screen* s, int i);

 /* --- App interface: what a backend drives to run one app on one ui_env. --- */

 /* Initialize the app on the given environment. Call once after the window is
  * created. */
void app_init(ui_env* env);

/* Handle the process's command-line arguments - call once, right after
 * app_init(), before the first app_frame(). argv[0] is the program name,
 * same convention as a normal C main(); a backend without a real argc/argv
 * (or nothing to pass) can call app_main(0, NULL) or app_main(1, argv) with
 * just the program name - both are a no-op beyond that. Today this just
 * looks for a single file path argument ("ide file.c") and opens it, the
 * same as picking it from File > Open. */
void app_main(int argc, char** argv);

/* Process one frame: handle events and update state. Call from a message loop
 * or timer repeatedly. Returns 0 to continue, non-zero if the user quit. */
int app_frame(ui_env* env);

/* Render the app's current state. Keeps the backend from needing to know
 * about the app's ui_screen (or any DOM internals) - the app calls
 * ui_screen_render() on its own tree internally, which in turn calls
 * ui_draw_char()/ui_draw_box() (implemented by whichever backend is linked
 * into this build). Forwards ui_screen_render's incremental-repaint contract:
 * writes the changed region (character cells) to *x,*y,*w,*h and returns 1 if
 * anything changed, 0 if the frame was identical and the backend can skip its
 * blit. Out-params may be NULL. */
int app_render(int* x, int* y, int* w, int* h);

/* Discard the render shadow so the next app_render repaints everything - a
 * backend calls this after (re)creating its backing bitmap/pixmap or
 * reloading its font (see ui_screen_invalidate). */
void app_invalidate(void);

/* Saves the session snapshot (see ide.c's save_session) so the next launch
 * can restore it - call once, right before the process actually exits,
 * however it's exiting. app_frame() already calls this itself once app_init
 * has run and the user quits through the app's own File > Exit (its return
 * value going non-zero); a backend whose OS can also tear the window down
 * WITHOUT app_frame ever returning non-zero (e.g. the native title bar's
 * close button, if it doesn't route through the same quit flag) needs to
 * call this itself from that path too - safe to call more than once, and a
 * no-op before app_init has run. */
void app_shutdown(void);

#endif /* UI_H */

typedef enum {
    UI_DIAG_INFO,
    UI_DIAG_WARNING,
    UI_DIAG_ERROR,
} ui_diag_type;

void ui_editor_add_diagnostic(ui_node* n, ui_diag_type type, int line, const char* message);
void ui_editor_clear_diagnostics(ui_node* n);