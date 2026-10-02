/* ide_gui_internal.h - the framework's internal data structures and the
 * helpers ide_gui_core.c shares with ide_gui_widgets.c. Not for apps
 * (they include ide_gui.h) nor backends (ide_gui_backend.h).
 */
#ifndef IDE_GUI_INTERNAL_H
#define IDE_GUI_INTERNAL_H

#include "ide_gui_backend.h"
#include <stddef.h>

/* The core's drawing goes through the frame recorder, which hands the
 * backend only what changed since the last paint (ide_gui_frame.c). */
#include "ide_gui_frame.h"
#define gui_fill_rect frame_fill_rect
#define gui_shade_rect frame_shade_rect
#define gui_set_clip frame_set_clip
#define gui_draw_text frame_draw_text

/* The modifier of the primary shortcuts - copy, paste, every "Ctrl+..."
 * menu shortcut: Command on macOS, Ctrl elsewhere (the old IDE's). Words
 * move with Option there (Ctrl+arrows are the system's), Ctrl elsewhere. */
#ifdef __APPLE__
#define GUI_MOD_PRIMARY GUI_MOD_CMD
#define GUI_MOD_WORD GUI_MOD_ALT
#else
#define GUI_MOD_PRIMARY GUI_MOD_CTRL
#define GUI_MOD_WORD GUI_MOD_CTRL
#endif

struct gui_node
{
    enum gui_kind kind;
    char* label;               /* UTF-8, never NULL */
    char* shortcut;            /* never NULL; "" for none */
    char* hint;                /* never NULL; "" for none */
    int id;
    int enabled;
    int separator;
    struct gui_layout layout;  /* where it sits in its parent (see ide_gui.h) */
    struct gui_rect rect;      /* computed by layout, px, window coordinates;
                                * for a GUI_WINDOW, its stored rect */
    struct window_props* window;  /* GUI_WINDOW only, else NULL */
    struct gui_node* context_menu;  /* opened by a right click - see gui_set_context_menu */

    /* Widgets (see ide_gui_widgets.c) */
    char* value;               /* GUI_INPUT: the text, never NULL */
    int cursor, anchor;        /* GUI_INPUT: caret and selection end, byte offsets */
    int hscroll;               /* GUI_INPUT: first visible column */
    int selected;              /* GUI_SELECT/LISTBOX/radio GROUP: index, -1 none */
    int reveal;                /* LISTBOX: scroll the selected row into view on the next paint */
    int scroll;                /* GUI_LISTBOX/GROUP: first visible row - rows are
                                * always whole, never shown in part */
    int multi;                 /* GUI_GROUP: check boxes */
    int checked;               /* GUI_ITEM of a check-box group */
    int tab;                   /* GUI_BUTTON: drawn as a tab; 2 = the selected tab */
    int has_colors;            /* GUI_TEXT/BOX: fg/bg below were set */
    uint32_t fg, bg;
    struct editor_data* editor;  /* GUI_EDITOR only, else NULL - see ide_gui_editor.c */
    enum gui_font_size font_size;
    struct gui_node* parent;
    struct gui_node** children;
    int child_count, child_cap;
};

/* What only a GUI_WINDOW has. */
struct window_props
{
    int resizable;
    int shadow;
    int min_cols, min_rows;
    int maximized;
    struct gui_rect restore;   /* the rect to go back to from maximized */
    enum gui_dock dock;
    int modal;                 /* blocks input to everything under it */
    int free_on_close;         /* a message box: freed when it closes */
    int ask_close;             /* the close icon fires the id and does not close */
};

/* The open windows, bottom to top. */
struct window_list
{
    struct gui_node** items;
    int count, cap;
};

/* A window being moved or resized with the mouse. */
enum drag_mode
{
    DRAG_NONE,
    DRAG_MOVE,
    DRAG_RESIZE,
    DRAG_DOCK,   /* the free border of a docked window: its thickness */
};

/* The frame edges a resize moves - the left, right and bottom borders and
 * the corners between them. The top border is the title row, which moves
 * the window instead. */
enum resize_edge
{
    EDGE_LEFT = 1,
    EDGE_RIGHT = 2,
    EDGE_BOTTOM = 4,
};

struct window_drag
{
    enum drag_mode mode;
    struct gui_node* win;
    int offset_x, offset_y;    /* DRAG_MOVE: the mouse inside the window */
    int edges;                 /* DRAG_RESIZE: enum resize_edge flags */
    struct gui_rect start;     /* DRAG_RESIZE/DOCK: the rect when the press happened */
    int start_x, start_y;      /* DRAG_RESIZE/DOCK: the mouse when the press happened */
};

/* The menu the user is in, if any. */
struct menu_state
{
    struct gui_node* open;     /* the open GUI_MENU, or NULL */
    struct gui_node* open_sub; /* its submenu parent item that is open, or NULL */
    struct gui_node* hot;      /* the title or item under the mouse, or NULL */
    struct gui_node* key;      /* the item the keyboard chose in the open menu (Up/Down, Enter) */
    struct gui_node* pressed;  /* the item the left button went down on */
    int popup_x, popup_y;      /* where `open` shows when it is a context menu */
    struct gui_node* target;   /* the node the last context menu opened over */
};

/* Widget interaction state. */
struct widget_state
{
    struct gui_node* focused;      /* keyboard focus */
    struct gui_node* hot;          /* the widget under the mouse */
    struct gui_node* active;       /* the button the left button went down on */
    struct gui_node* open_select;  /* the GUI_SELECT whose list is open */
    struct gui_node* selecting;    /* the GUI_INPUT a drag is selecting in */
    struct gui_node* scrolling;    /* the GUI_LISTBOX/EDITOR whose vertical scrollbar is dragged */
    struct gui_node* hscrolling;   /* the GUI_EDITOR whose horizontal scrollbar is dragged */
    struct gui_node* thumb_hot;    /* the widget whose scrollbar thumb is under the mouse */
    int thumb_hot_bar;             /* which: 1 vertical, 2 horizontal */
    int wheel_rest;                /* wheel units not yet turned into a row */
    int hwheel_rest;               /* the same, sideways, into a column */
    int scroll_grab;               /* px from the thumb's start to where it was grabbed */
};

struct event_handler
{
    void (*fn)(void* ctx, int id);
    void* ctx;
};

struct app_timer
{
    int ms;        /* 0: stopped */
    int id;        /* fired on every tick */
    int changed;   /* the backend has not applied ms yet */
};

#define GUI_MAX_FONTS 16

/* The font families the backend offers - see gui_font_count. */
struct app_fonts
{
    char names[GUI_MAX_FONTS][64];
    int count;
    int current;
    int requested;   /* -1: none */
};

struct gui_app
{
    struct gui_canvas* canvas;   /* the backend's, from gui_app_start - for the clipboard */
    struct gui_node* root;
    struct gui_theme theme;
    struct gui_metrics metrics;  /* of GUI_FONT_MAIN, re-read on font change */
    struct gui_metrics small_metrics;  /* of GUI_FONT_SMALL */
    int scrollbar_px;            /* scrollbar thickness, from the backend */
    int w, h;                    /* client area, px */
    int mouse_x, mouse_y;        /* px; -1 when outside the window */
    int mouse_mods;              /* the modifiers of the last button press */
    struct menu_state menu;
    struct window_list windows;
    struct window_drag drag;
    struct widget_state ui;
    struct event_handler on_event;
    struct app_timer timer;
    int quit_id;                 /* see gui_set_quit_id */
    char* tooltip;               /* see gui_set_tooltip; NULL: none */
    int tooltip_x, tooltip_y;
    struct app_fonts fonts;
    int zoom;                    /* points asked for by gui_zoom, not applied yet */
    int zoom_total;              /* every gui_zoom, summed - see gui_get_zoom */
    int quit;                    /* gui_quit was called */
    int needs_layout;
    int needs_paint;
};

/* What every paint function needs: the app (theme, metrics) and where to
 * draw. */
struct paint
{
    const struct gui_app* app;
    struct gui_canvas* canvas;
    enum gui_font font;   /* the font core_draw_utf8 draws with */
};

/* --- Shared helpers (ide_gui_core.c) --- */

void* core_calloc(size_t n, size_t size);
char* core_strdup(const char* s);
int core_rect_contains(const struct gui_rect* r, int x, int y);
int core_utf8_decode(const char* s, uint32_t* cp);
int core_utf8_cells(const char* s, int bytes);
int core_utf8_prefix_bytes(const char* s, int cells);
/* A symbol some fonts lack (arrows, triangles, bullet, square, blocks) drawn
 * as shapes in the cell; 0 when `cp` is not one of them. */
int core_draw_symbol(const struct paint* p, int x, int y, int cw, int ch, uint32_t cp, uint32_t fg, uint32_t bg);
int core_draw_utf8(const struct paint* p, int x, int y, const char* s, int bytes,
                   uint32_t fg, uint32_t bg);
int core_line_weight(const struct gui_app* app);
enum gui_font core_node_font(const struct gui_node* n);   /* its own, or inherited */
const struct gui_metrics* core_font_metrics(const struct gui_app* app, enum gui_font font);
const struct gui_metrics* core_node_metrics(const struct gui_app* app, const struct gui_node* n);
void core_draw_frame(const struct paint* p, const struct gui_rect* r,
                     enum gui_border_style style, uint32_t fg, uint32_t bg);
void core_draw_shadow(const struct paint* p, const struct gui_rect* r);
void core_fire(struct gui_app* app, int id);
struct gui_node* core_find_kind(const struct gui_node* n, enum gui_kind kind);
struct gui_node* core_window_at_point(const struct gui_app* app, int x, int y);
struct gui_node* core_top_modal(const struct gui_app* app);   /* the top window if modal */

/* A scrollbar: a `track` px long bar for `total` items of which `visible`
 * show, scrolled by `scroll` items. The thumb is sized and placed in px -
 * its length is the track times visible / total - while the content moves
 * by whole items. Shared by every scrolling widget. */
struct scrollbar
{
    int track, total, visible, scroll;
};

void core_scrollbar_thumb(const struct gui_app* app, const struct scrollbar* sb, int* pos, int* len);
/* A press at `at` px along the track: on the thumb it grabs it there, off
 * it the thumb jumps to center on the press. Returns the new scroll. */
int core_scrollbar_press(struct gui_app* app, const struct scrollbar* sb, int at);
/* A drag to `at`: the grabbed point stays under the mouse. Returns the new scroll. */
int core_scrollbar_drag(const struct gui_app* app, const struct scrollbar* sb, int at);

/* --- Widgets (ide_gui_widgets.c) --- */

void widget_init(struct gui_node* n);                 /* defaults for a new node */
int widget_is_widget(const struct gui_node* n);
void widget_paint(const struct paint* p, const struct gui_node* n);
void widget_paint_popups(const struct paint* p);       /* an open select list, on top */
struct gui_node* widget_at(const struct gui_app* app, const struct gui_node* within, int x, int y);
int widget_popup_mouse_down(struct gui_app* app);      /* 1 if an open select list took it */
void widget_mouse_down(struct gui_app* app, struct gui_node* n, int double_click);
void widget_mouse_up(struct gui_app* app);
void widget_mouse_move(struct gui_app* app);
void widget_wheel(struct gui_app* app, int wheel);
void widget_hwheel(struct gui_app* app, int wheel);
int widget_key(struct gui_app* app, int key, int mods);   /* 1 if handled */
void widget_char(struct gui_app* app, uint32_t ch);
void widget_forget(struct gui_app* app, const struct gui_node* n);  /* n is going away */

/* The scrollbar thumb of an editor at (x, y): 1 vertical, 2 horizontal, 0 none. */
int editor_thumb_at(const struct gui_app* app, const struct gui_node* n, int x, int y);

/* --- Editor (ide_gui_editor.c) --- */

void editor_create(struct gui_node* n);
void editor_free(struct gui_node* n);
void editor_set_text(struct gui_node* n, const char* utf8);
const char* editor_get_text(const struct gui_node* n);
void editor_paint(const struct paint* p, const struct gui_node* n);
void editor_mouse_down(struct gui_app* app, struct gui_node* n, int double_click, int mods);
void editor_mouse_drag(struct gui_app* app, struct gui_node* n);   /* selecting / scrollbars */
void editor_wheel(struct gui_app* app, struct gui_node* n, int rows);
void editor_hwheel(struct gui_app* app, struct gui_node* n, int cols);
int editor_key(struct gui_app* app, struct gui_node* n, int key, int mods);
void editor_char(struct gui_app* app, struct gui_node* n, uint32_t ch);

#endif
