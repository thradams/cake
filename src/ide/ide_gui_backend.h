/* ide_gui_backend.h - what a platform backend (ide_gui_win32.c,
 * ide_gui_x11.c, ide_gui_cocoa.c) provides to the framework core, and what
 * the core provides to the backend. Internal: apps include only ide_gui.h.
 *
 * A backend only translates OS events and implements the drawing
 * primitives. Layout, painting order, hit testing and behavior live in the
 * core (ide_gui_core.c).
 */
#ifndef IDE_GUI_BACKEND_H
#define IDE_GUI_BACKEND_H

#include "ide_gui.h"

/* --- Implemented by the backend --- */

/* The backend's drawing target (its back buffer, fonts, ...). Opaque to the
 * core, which only passes it back into the primitives below. */
struct gui_canvas;

enum gui_font
{
    GUI_FONT_MAIN = 0,  /* the main monospaced font */
    GUI_FONT_SMALL,     /* the same family, smaller - both always exist */
    GUI_FONT_UI,        /* the "other fonts" family, may be proportional, small or normal size */
};

struct gui_metrics
{
    int cell_w;  /* advance of one character, px */
    int cell_h;  /* line height, px */
    int ascent;  /* baseline offset from the top of a line, px */
};

struct gui_metrics gui_font_metrics(struct gui_canvas* c, enum gui_font font);

/* The thickness of a scrollbar, px - its own size, scaled for the DPI,
 * not tied to any font. */
int gui_scrollbar_size(struct gui_canvas* c);

/* Drawing goes into the canvas's back buffer; the backend presents the
 * region the core reports as painted. */
/* Drawing stays inside this rect until the next call; w or h <= 0
 * clears it. */
void gui_set_clip(struct gui_canvas* c, int x, int y, int w, int h);
void gui_fill_rect(struct gui_canvas* c, int x, int y, int w, int h, uint32_t rgb);

/* Darkens the rect with black at `alpha` (0..255) - drop shadows. */
void gui_shade_rect(struct gui_canvas* c, int x, int y, int w, int h, int alpha);

/* One run of text at (x, y) = top-left of its first cell: every code point
 * advances exactly cell_w, `bg` fills the run's rect, and the glyphs are
 * clipped to it. */
void gui_draw_text(struct gui_canvas* c, int x, int y, const uint32_t* cps, int count,
                   uint32_t fg, uint32_t bg, enum gui_font font);

/* The width of a run, px: count * cell_w, except in GUI_FONT_UI, where
 * every glyph advances its own width (and gui_draw_text draws them so). */
int gui_text_width(struct gui_canvas* c, const uint32_t* cps, int count, enum gui_font font);

/* Whether this backend can show a detached window in an OS window of its
 * own (gui_window_detach). 0: the surface calls below are never needed. */
int gui_backend_can_detach(void);

/* --- Clipboard --- */

/* The canvas reaches the backend's window, which X11 needs to own the
 * selection. */
void gui_clipboard_set(struct gui_canvas* c, const char* utf8);  /* '
' line ends */
char* gui_clipboard_get(struct gui_canvas* c);           /* malloc'ed, '
' line ends; NULL if empty */

/* --- Input, posted by the backend --- */

/* Keys other than characters. Letters and digits use their uppercase ASCII
 * code ('A', '7'); '+', '-' and ' ' likewise. */
enum gui_key
{
    GUI_KEY_ESCAPE = 256,
    GUI_KEY_ENTER,
    GUI_KEY_TAB,
    GUI_KEY_BACKSPACE,
    GUI_KEY_DELETE,
    GUI_KEY_INSERT,
    GUI_KEY_HOME,
    GUI_KEY_END,
    GUI_KEY_PAGEUP,
    GUI_KEY_PAGEDOWN,
    GUI_KEY_LEFT,
    GUI_KEY_RIGHT,
    GUI_KEY_UP,
    GUI_KEY_DOWN,
    GUI_KEY_F1,   /* F1..F12 are consecutive */
};

enum gui_mod
{
    GUI_MOD_SHIFT = 1,
    GUI_MOD_CTRL = 2,
    GUI_MOD_ALT = 4,
    GUI_MOD_CMD = 8,    /* macOS Command: the primary modifier there */
};

enum gui_event_type
{
    GUI_EVENT_KEY,         /* key, mods - a key went down */
    GUI_EVENT_CHAR,        /* ch - a character was typed */
    GUI_EVENT_MOUSE_MOVE,  /* x, y */
    GUI_EVENT_MOUSE_DOWN,  /* x, y, button */
    GUI_EVENT_MOUSE_UP,    /* x, y, button */
    GUI_EVENT_MOUSE_LEAVE, /* the pointer left the window */
    GUI_EVENT_WHEEL,       /* x, y, wheel */
    GUI_EVENT_TIMER,       /* the timer gui_app_take_timer asked for went off */
    GUI_EVENT_CLOSE,       /* the user asked to close the OS window */
};

struct gui_event
{
    enum gui_event_type type;
    int key;       /* enum gui_key or an ASCII code */
    int mods;      /* enum gui_mod flags */
    uint32_t ch;   /* GUI_EVENT_CHAR */
    int x, y;      /* px, client coordinates */
    int button;    /* 0 left, 1 right, 2 middle */
    int double_click;  /* GUI_EVENT_MOUSE_DOWN: the second click of a double click */
    int wheel;     /* GUI_EVENT_WHEEL: 120 per notch away from the user, less from
                    * a touchpad - the OS units, so small steps stay smooth */
    int hwheel;    /* GUI_EVENT_WHEEL: the same units sideways, positive scrolls
                    * the view right */
};

/* --- Implemented by the core, called by the backend --- */

struct gui_app* gui_app_create(void);
void gui_app_free(struct gui_app* app);

/* Once, after the window exists and its font is set: calls gui_main. */
void gui_app_start(struct gui_app* app, struct gui_canvas* c, int argc, char** argv);

/* The client area changed size (px). */
void gui_app_resize(struct gui_app* app, int w, int h);
/* The back buffer was made again (its contents lost): the next paint draws
 * everything. */
void gui_app_invalidate(struct gui_app* app);

/* The zoom the app asked for (gui_zoom) since the last call, in points;
 * the backend applies it to the font, then calls gui_app_font_changed. */
int gui_app_take_zoom(struct gui_app* app);

/* Whether the app changed its timer (gui_set_timer) since the last call;
 * if so *ms is its new period, 0 to stop it. */
int gui_app_take_timer(struct gui_app* app, int* ms);

/* Before gui_app_start: the font families the backend can use (UTF-8,
 * copied) and the one it uses. */
void gui_app_set_fonts(struct gui_app* app, const char* const names[], int count, int current);

/* The font family the app asked for (gui_set_font) since the last call, or
 * -1; the backend applies it, then calls gui_app_font_changed. */
int gui_app_take_font(struct gui_app* app);

/* Before gui_app_start: the proportional font families (UTF-8); current
 * -1: GUI_FONT_UI is the small font. */
void gui_app_set_ui_fonts(struct gui_app* app, const char* const names[], int count, int current);

/* 1 when the app picked another GUI_FONT_UI family or size since the last
 * call: the family's index in *index (-1: the editor's), *small 1 for the
 * small size; the backend applies it, then calls gui_app_font_changed. */
int gui_app_take_ui_font(struct gui_app* app, int* index, int* small);

/* Whether the app asked to end (gui_quit). */
int gui_app_should_quit(const struct gui_app* app);

/* The font changed: metrics are re-read and everything is laid out again. */
void gui_app_font_changed(struct gui_app* app, struct gui_canvas* c);

void gui_app_event(struct gui_app* app, const struct gui_event* ev);

/* The color the backend paints the OS title bar with, if it can. */
uint32_t gui_app_titlebar_color(const struct gui_app* app);

/* The mouse cursor to show where the mouse is now. */
enum gui_cursor
{
    GUI_CURSOR_ARROW,
    GUI_CURSOR_SIZE_WE,    /* left/right border */
    GUI_CURSOR_SIZE_NS,    /* bottom border */
    GUI_CURSOR_SIZE_NWSE,  /* bottom-right corner */
    GUI_CURSOR_SIZE_NESW,  /* bottom-left corner */
};

enum gui_cursor gui_app_cursor(const struct gui_app* app);

/* Paints whatever is dirty into the canvas. Returns 1 and sets *painted if
 * anything was painted, 0 if nothing was dirty. */
int gui_app_paint(struct gui_app* app, struct gui_canvas* c, struct gui_rect* painted);

/* --- Detached windows: one more OS window each ---
 *
 * After each event the backend asks for the OS windows to make, raise and
 * destroy. Each has its own canvas (the same fonts as the main one) and
 * gets its own events, size and paints through the calls below. */
struct gui_surface;

/* A surface that needs its OS window, or NULL: its client size in *w, *h
 * (px). The backend makes it, then calls gui_surface_start. */
struct gui_surface* gui_app_take_surface_open(struct gui_app* app, int* w, int* h);
void gui_surface_start(struct gui_app* app, struct gui_surface* s, void* native, int w, int h);
/* A surface whose OS window should come to the front, or NULL; its native. */
void* gui_app_take_surface_raise(struct gui_app* app);
/* A surface that is gone: the native of the OS window to destroy, or NULL. */
void* gui_app_take_surface_close(struct gui_app* app);

void gui_surface_resize(struct gui_app* app, struct gui_surface* s, int w, int h);
void gui_surface_invalidate(struct gui_app* app, struct gui_surface* s);
void gui_surface_event(struct gui_app* app, struct gui_surface* s, const struct gui_event* ev);
enum gui_cursor gui_surface_cursor(struct gui_app* app, struct gui_surface* s);
int gui_surface_paint(struct gui_app* app, struct gui_surface* s, struct gui_canvas* c, struct gui_rect* painted);

#endif
