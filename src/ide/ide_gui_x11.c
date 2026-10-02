/* ide_gui_x11.c - X11/Xft backend of the ide GUI framework.
 *
 * Only translates X events into calls to the core and implements the
 * drawing primitives of ide_gui_backend.h. Everything about a window
 * lives in one struct x11_window; the canvas is its first member, so the
 * clipboard (which only gets the canvas) reaches the window - no globals.
 * The main window owns the detached ones (gui_window_detach): one more X
 * window each, on the same display, with its fonts and cursors.
 *
 * Build: gcc ... ide_gui_x11.c -lX11 -lXft -lXrender -lfontconfig
 *            $(pkg-config --cflags freetype2)
 */
#define _DEFAULT_SOURCE

#include "ide_gui_backend.h"
#include "../version.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <X11/Xft/Xft.h>
#include <X11/extensions/Xrender.h>

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include <sys/select.h>

#define DEFAULT_FONT_PT 11
#define DEFAULT_COLS 120
#define DEFAULT_ROWS 30
#define SMALL_FONT_PERCENT 85
#define DOUBLE_CLICK_MS 400

static const char* const font_candidates[] = {
    "DejaVu Sans Mono", "Liberation Mono", "Noto Sans Mono", "Ubuntu Mono", "Monospace",
};

#define CANDIDATE_COUNT ((int)(sizeof font_candidates / sizeof font_candidates[0]))

struct clip_rect
{
    int x, y, w, h;
    int on;
};

struct gui_canvas
{
    Display* dpy;
    int screen;
    Window win;
    Visual* visual;
    Colormap cmap;
    GC gc;
    Pixmap pixmap;               /* the back buffer */
    Picture picture;             /* XRender view of the pixmap - shadows */
    XftDraw* draw;
    int w, h;
    XftFont* main;               /* GUI_FONT_MAIN */
    XftFont* small_font;         /* GUI_FONT_SMALL */
    int pt;                      /* the main font's size in points */
    const char* family;          /* NULL: the first installed candidate */
    struct gui_metrics metrics;
    struct gui_metrics small_metrics;
    double dpi;
    struct clip_rect clip;
    char* clip_text;             /* what we own on CLIPBOARD, answered on request */
    Atom clipboard, utf8_string, targets, paste_prop;
};

struct offered_fonts
{
    int candidate[16];
    int count;
};

struct click
{
    Time time;
    int x, y;
};

struct x11_window;

/* The main window's detached windows. */
struct detached_list
{
    struct x11_window** items;
    int count, cap;
};

struct x11_window
{
    struct gui_canvas canvas;    /* first: gui_clipboard_* get only this */
    struct gui_app* app;
    XIM xim;
    XIC xic;
    Atom wm_delete;
    Cursor cursors[5];           /* by enum gui_cursor */
    enum gui_cursor cursor;
    struct offered_fonts fonts;
    struct click last_click;
    int timer_ms;                /* 0: stopped */
    long long timer_due;         /* ms, monotonic */
    uint32_t titlebar_color;
    int titlebar_applied;
    int running;
    int pending;                 /* events taken since the last refresh */
    struct x11_window* owner;    /* the main window; itself for the main window */
    struct gui_surface* surface; /* a detached window's; NULL for the main window */
    struct detached_list detached;  /* the main window's */
};

static long long now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

static unsigned long to_pixel(struct gui_canvas* c, uint32_t rgb)
{
    if (c->visual->red_mask == 0xFF0000 && c->visual->green_mask == 0xFF00 &&
        c->visual->blue_mask == 0xFF)
        return rgb & 0xFFFFFF;
    XColor xc = { 0 };
    xc.red = (unsigned short)(((rgb >> 16) & 0xFF) * 257);
    xc.green = (unsigned short)(((rgb >> 8) & 0xFF) * 257);
    xc.blue = (unsigned short)((rgb & 0xFF) * 257);
    xc.flags = DoRed | DoGreen | DoBlue;
    XAllocColor(c->dpy, c->cmap, &xc);
    return xc.pixel;
}

/* --- Fonts --- */

/* fontconfig substitutes instead of failing, so a family is installed only
 * if the match has that very name. */
static int font_exists(struct gui_canvas* c, const char* name)
{
    FcPattern* pat = FcNameParse((const FcChar8*)name);
    if (!pat)
        return 0;
    FcResult result;
    FcPattern* match = XftFontMatch(c->dpy, c->screen, pat, &result);
    FcPatternDestroy(pat);
    if (!match)
        return 0;
    FcChar8* family = NULL;
    int same = FcPatternGetString(match, FC_FAMILY, 0, &family) == FcResultMatch &&
               strcasecmp((const char*)family, name) == 0;
    FcPatternDestroy(match);
    return same;
}

static const char* pick_font(struct gui_canvas* c)
{
    for (int i = 0; i < CANDIDATE_COUNT; i++)
    {
        if (font_exists(c, font_candidates[i]))
            return font_candidates[i];
    }
    return "Monospace";
}

/* The DPI Xft draws with: the Xft.dpi resource, else the screen's. */
static double screen_dpi(struct gui_canvas* c)
{
    const char* s = XGetDefault(c->dpy, "Xft", "dpi");
    double dpi = s ? atof(s) : 0;
    if (dpi <= 0)
    {
        int mm = DisplayHeightMM(c->dpy, c->screen);
        dpi = mm > 0 ? DisplayHeight(c->dpy, c->screen) * 25.4 / mm : 96;
    }
    return dpi > 0 ? dpi : 96;
}

/* A font of the chosen family at `pt`, measured: the advance of "M" and
 * ascent + descent. */
static XftFont* make_font(struct gui_canvas* c, int pt, struct gui_metrics* m)
{
    const char* family = c->family ? c->family : pick_font(c);
    XftFont* font = XftFontOpen(c->dpy, c->screen,
                                XFT_FAMILY, XftTypeString, family,
                                XFT_SIZE, XftTypeDouble, (double)pt,
                                XFT_DPI, XftTypeDouble, c->dpi,
                                XFT_SPACING, XftTypeInteger, XFT_MONO,
                                NULL);
    if (!font)
        return NULL;
    XGlyphInfo ext;
    XftTextExtentsUtf8(c->dpy, font, (const FcChar8*)"M", 1, &ext);
    m->cell_w = ext.xOff > 0 ? ext.xOff : 1;
    m->cell_h = font->ascent + font->descent > 0 ? font->ascent + font->descent : 1;
    m->ascent = font->ascent;
    return font;
}

static void offer_fonts(struct x11_window* win)
{
    const char* names[CANDIDATE_COUNT];
    win->fonts.count = 0;
    for (int i = 0; i < CANDIDATE_COUNT; i++)
    {
        if (!font_exists(&win->canvas, font_candidates[i]))
            continue;
        names[win->fonts.count] = font_candidates[i];
        win->fonts.candidate[win->fonts.count++] = i;
    }
    gui_app_set_fonts(win->app, names, win->fonts.count, 0);
}

/* (Re)creates both fonts from canvas->pt: they always exist together. */
static int apply_font(struct gui_canvas* c)
{
    c->dpi = screen_dpi(c);
    int small_pt = (c->pt * SMALL_FONT_PERCENT + 50) / 100;
    struct gui_metrics m, sm;
    XftFont* main = make_font(c, c->pt, &m);
    XftFont* small_font = make_font(c, small_pt > 0 ? small_pt : 1, &sm);
    if (!main || !small_font)
    {
        if (main) XftFontClose(c->dpy, main);
        if (small_font) XftFontClose(c->dpy, small_font);
        return 0;
    }
    if (c->main) XftFontClose(c->dpy, c->main);
    if (c->small_font) XftFontClose(c->dpy, c->small_font);
    c->main = main;
    c->small_font = small_font;
    c->metrics = m;
    c->small_metrics = sm;
    return 1;
}

/* --- Drawing primitives (ide_gui_backend.h) --- */

struct gui_metrics gui_font_metrics(struct gui_canvas* c, enum gui_font font)
{
    return font == GUI_FONT_SMALL ? c->small_metrics : c->metrics;
}

int gui_scrollbar_size(struct gui_canvas* c)
{
    int size = (int)(10.0 * c->pt * c->dpi / (96.0 * DEFAULT_FONT_PT) + 0.5);
    return size > 2 ? size : 2;
}

/* The rect intersected with the clip; 0 if nothing is left. */
static int clip_to(const struct gui_canvas* c, XRectangle* r, int x, int y, int w, int h)
{
    if (c->clip.on)
    {
        int x1 = x + w, y1 = y + h;
        int cx1 = c->clip.x + c->clip.w, cy1 = c->clip.y + c->clip.h;
        if (x < c->clip.x) x = c->clip.x;
        if (y < c->clip.y) y = c->clip.y;
        if (x1 > cx1) x1 = cx1;
        if (y1 > cy1) y1 = cy1;
        w = x1 - x;
        h = y1 - y;
    }
    if (w <= 0 || h <= 0)
        return 0;
    r->x = (short)x;
    r->y = (short)y;
    r->width = (unsigned short)w;
    r->height = (unsigned short)h;
    return 1;
}

void gui_set_clip(struct gui_canvas* c, int x, int y, int w, int h)
{
    c->clip.on = w > 0 && h > 0;
    c->clip.x = x;
    c->clip.y = y;
    c->clip.w = w;
    c->clip.h = h;
    if (c->clip.on)
    {
        XRectangle r = { (short)x, (short)y, (unsigned short)w, (unsigned short)h };
        XSetClipRectangles(c->dpy, c->gc, 0, 0, &r, 1, Unsorted);
        XRenderSetPictureClipRectangles(c->dpy, c->picture, 0, 0, &r, 1);
    }
    else
    {
        XSetClipMask(c->dpy, c->gc, None);
        XRenderPictureAttributes pa = { 0 };
        pa.clip_mask = None;
        XRenderChangePicture(c->dpy, c->picture, CPClipMask, &pa);
    }
}

void gui_fill_rect(struct gui_canvas* c, int x, int y, int w, int h, uint32_t rgb)
{
    if (w <= 0 || h <= 0)
        return;
    XSetForeground(c->dpy, c->gc, to_pixel(c, rgb));
    XFillRectangle(c->dpy, c->pixmap, c->gc, x, y, (unsigned)w, (unsigned)h);
}

void gui_shade_rect(struct gui_canvas* c, int x, int y, int w, int h, int alpha)
{
    if (w <= 0 || h <= 0)
        return;
    XRenderColor black = { 0, 0, 0, (unsigned short)(alpha * 257) };
    XRenderFillRectangle(c->dpy, PictOpOver, c->picture, &black, x, y, (unsigned)w, (unsigned)h);
}

void gui_draw_text(struct gui_canvas* c, int x, int y, const uint32_t* cps, int count,
                   uint32_t fg, uint32_t bg, enum gui_font font)
{
    const struct gui_metrics* m = font == GUI_FONT_SMALL ? &c->small_metrics : &c->metrics;
    XftFont* xf = font == GUI_FONT_SMALL ? c->small_font : c->main;
    gui_fill_rect(c, x, y, count * m->cell_w, m->cell_h, bg);

    XRectangle r;
    if (!clip_to(c, &r, x, y, count * m->cell_w, m->cell_h))
        return;
    XRenderColor rc = {
        (unsigned short)(((fg >> 16) & 0xFF) * 257),
        (unsigned short)(((fg >> 8) & 0xFF) * 257),
        (unsigned short)((fg & 0xFF) * 257),
        0xFFFF,
    };
    XftColor color;
    XftColorAllocValue(c->dpy, c->visual, c->cmap, &rc, &color);
    XftDrawSetClipRectangles(c->draw, 0, 0, &r, 1);

    XftCharSpec specs[256];
    while (count > 0)
    {
        int n = count < 256 ? count : 256;
        int k = 0;
        for (int i = 0; i < n; i++)
        {
            if (cps[i] == ' ' || cps[i] == 0)
                continue;
            specs[k].ucs4 = cps[i];
            specs[k].x = (short)(x + i * m->cell_w);
            specs[k].y = (short)(y + m->ascent);
            k++;
        }
        if (k > 0)
            XftDrawCharSpec(c->draw, &color, xf, specs, k);
        x += n * m->cell_w;
        cps += n;
        count -= n;
    }
    XftDrawSetClip(c->draw, None);
    XftColorFree(c->dpy, c->visual, c->cmap, &color);
}

/* --- Back buffer --- */

static void ensure_back_buffer(struct gui_canvas* c, int w, int h)
{
    if (w < 1) w = 1;
    if (h < 1) h = 1;
    if (c->pixmap && w == c->w && h == c->h)
        return;
    if (c->draw) XftDrawDestroy(c->draw);
    if (c->picture) XRenderFreePicture(c->dpy, c->picture);
    if (c->pixmap) XFreePixmap(c->dpy, c->pixmap);
    c->pixmap = XCreatePixmap(c->dpy, c->win, (unsigned)w, (unsigned)h,
                              (unsigned)DefaultDepth(c->dpy, c->screen));
    c->picture = XRenderCreatePicture(c->dpy, c->pixmap,
                                      XRenderFindVisualFormat(c->dpy, c->visual), 0, NULL);
    c->draw = XftDrawCreate(c->dpy, c->pixmap, c->visual, c->cmap);
    c->w = w;
    c->h = h;
    gui_set_clip(c, 0, 0, 0, 0);
}

/* The closest X11 gets to tinting the title bar: GTK-themed window
 * managers honor a dark or light variant. */
static void sync_titlebar(struct x11_window* win)
{
    uint32_t color = gui_app_titlebar_color(win->app);
    if (win->titlebar_applied && win->titlebar_color == color)
        return;
    win->titlebar_color = color;
    win->titlebar_applied = 1;
    int luma = (int)(((color >> 16) & 0xFF) * 2126 + ((color >> 8) & 0xFF) * 7152 + (color & 0xFF) * 722) / 10000;
    const char* variant = luma < 128 ? "dark" : "light";
    struct gui_canvas* c = &win->canvas;
    XChangeProperty(c->dpy, c->win, XInternAtom(c->dpy, "_GTK_THEME_VARIANT", False),
                    c->utf8_string, 8, PropModeReplace,
                    (const unsigned char*)variant, (int)strlen(variant));
}

static void present(struct gui_canvas* c, int x, int y, int w, int h)
{
    XSetClipMask(c->dpy, c->gc, None);
    XCopyArea(c->dpy, c->pixmap, c->win, c->gc, x, y, (unsigned)w, (unsigned)h, x, y);
    if (c->clip.on)
        gui_set_clip(c, c->clip.x, c->clip.y, c->clip.w, c->clip.h);
}

/* --- Detached windows --- */

int gui_backend_can_detach(void)
{
    return 1;
}

/* A detached window draws with the main window's fonts. */
static void share_fonts(struct x11_window* d, const struct x11_window* main)
{
    struct gui_canvas* c = &d->canvas;
    c->main = main->canvas.main;
    c->small_font = main->canvas.small_font;
    c->pt = main->canvas.pt;
    c->family = main->canvas.family;
    c->metrics = main->canvas.metrics;
    c->small_metrics = main->canvas.small_metrics;
    c->dpi = main->canvas.dpi;
}

static void detached_open(struct x11_window* main, struct gui_surface* s, int w, int h)
{
    const struct gui_canvas* mc = &main->canvas;
    struct x11_window* d = calloc(1, sizeof *d);
    if (!d)
        abort();
    struct gui_canvas* c = &d->canvas;
    c->dpy = mc->dpy;
    c->screen = mc->screen;
    c->visual = mc->visual;
    c->cmap = mc->cmap;
    c->clipboard = mc->clipboard;
    c->utf8_string = mc->utf8_string;
    c->targets = mc->targets;
    c->paste_prop = mc->paste_prop;
    share_fonts(d, main);
    d->app = main->app;
    d->owner = main;
    d->surface = s;
    d->wm_delete = main->wm_delete;
    memcpy(d->cursors, main->cursors, sizeof d->cursors);

    if (w < 1) w = 640;
    if (h < 1) h = 480;
    XSetWindowAttributes attrs = { 0 };
    attrs.background_pixmap = None;
    attrs.bit_gravity = NorthWestGravity;
    c->win = XCreateWindow(c->dpy, RootWindow(c->dpy, c->screen), 0, 0, (unsigned)w, (unsigned)h, 0,
                           DefaultDepth(c->dpy, c->screen), InputOutput, c->visual,
                           CWBackPixmap | CWBitGravity, &attrs);
    XSetWMProtocols(c->dpy, c->win, &d->wm_delete, 1);
    static const char title[] = "Cake " CAKE_VERSION;
    XStoreName(c->dpy, c->win, title);
    XChangeProperty(c->dpy, c->win, XInternAtom(c->dpy, "_NET_WM_NAME", False), c->utf8_string, 8,
                    PropModeReplace, (const unsigned char*)title, (int)strlen(title));
    XDefineCursor(c->dpy, c->win, d->cursors[GUI_CURSOR_ARROW]);
    if (main->xim)
        d->xic = XCreateIC(main->xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
                           XNClientWindow, c->win, XNFocusWindow, c->win, NULL);
    XSelectInput(c->dpy, c->win, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask |
                 PointerMotionMask | LeaveWindowMask | StructureNotifyMask);
    c->gc = XCreateGC(c->dpy, c->win, 0, NULL);
    ensure_back_buffer(c, w, h);

    struct detached_list* list = &main->detached;
    if (list->count == list->cap)
    {
        int cap = list->cap ? list->cap * 2 : 4;
        struct x11_window** items = realloc(list->items, (size_t)cap * sizeof *items);
        if (!items)
            abort();
        list->items = items;
        list->cap = cap;
    }
    list->items[list->count++] = d;
    gui_surface_start(d->app, s, d, c->w, c->h);
    XMapWindow(c->dpy, c->win);
}

/* The X window and its back buffer go; the fonts and cursors are the main
 * window's. */
static void detached_destroy(struct x11_window* d)
{
    struct gui_canvas* c = &d->canvas;
    if (d->xic) XDestroyIC(d->xic);
    if (c->draw) XftDrawDestroy(c->draw);
    if (c->picture) XRenderFreePicture(c->dpy, c->picture);
    if (c->pixmap) XFreePixmap(c->dpy, c->pixmap);
    XFreeGC(c->dpy, c->gc);
    XDestroyWindow(c->dpy, c->win);
    free(d);
}

static void detached_close(struct x11_window* main, struct x11_window* d)
{
    struct detached_list* list = &main->detached;
    for (int i = 0; i < list->count; i++)
    {
        if (list->items[i] == d)
        {
            memmove(&list->items[i], &list->items[i + 1], sizeof list->items[0] * (size_t)(list->count - i - 1));
            list->count--;
            break;
        }
    }
    detached_destroy(d);
}

/* The window an X event is for: the main one or a detached one; NULL for
 * one already gone. */
static struct x11_window* window_for(struct x11_window* main, Window w)
{
    if (w == main->canvas.win)
        return main;
    for (int i = 0; i < main->detached.count; i++)
    {
        if (main->detached.items[i]->canvas.win == w)
            return main->detached.items[i];
    }
    return NULL;
}

/* Raised the way window managers honor: _NET_ACTIVE_WINDOW to the root. */
static void detached_raise(struct x11_window* d)
{
    struct gui_canvas* c = &d->canvas;
    XEvent ev = { 0 };
    ev.xclient.type = ClientMessage;
    ev.xclient.window = c->win;
    ev.xclient.message_type = XInternAtom(c->dpy, "_NET_ACTIVE_WINDOW", False);
    ev.xclient.format = 32;
    ev.xclient.data.l[0] = 1;   /* from a normal application */
    ev.xclient.data.l[1] = CurrentTime;
    XMapRaised(c->dpy, c->win);
    XSendEvent(c->dpy, RootWindow(c->dpy, c->screen), False,
               SubstructureRedirectMask | SubstructureNotifyMask, &ev);
}

/* The X windows the core asked for, made, raised and closed. */
static void sync_detached(struct x11_window* main)
{
    int w, h;
    struct gui_surface* s;
    while ((s = gui_app_take_surface_open(main->app, &w, &h)) != NULL)
        detached_open(main, s, w, h);
    void* native;
    while ((native = gui_app_take_surface_close(main->app)) != NULL)
        detached_close(main, native);
    while ((native = gui_app_take_surface_raise(main->app)) != NULL)
        detached_raise(native);
}

/* Applies what the app asked for (quit, timer, font, zoom), lets the core
 * paint whatever is dirty and copies just that rect to the window - the
 * main one and each detached one. */
static void refresh(struct x11_window* win)
{
    struct gui_canvas* c = &win->canvas;
    if (gui_app_should_quit(win->app))
    {
        win->running = 0;
        return;
    }
    int ms;
    if (gui_app_take_timer(win->app, &ms))
    {
        win->timer_ms = ms > 0 ? ms : 0;
        win->timer_due = now_ms() + win->timer_ms;
    }
    int family = gui_app_take_font(win->app);
    if (family >= 0 && family < win->fonts.count)
    {
        c->family = font_candidates[win->fonts.candidate[family]];
        if (apply_font(c))
            gui_app_font_changed(win->app, c);
    }
    int zoom = gui_app_take_zoom(win->app);
    if (zoom)
    {
        int pt = c->pt + zoom;
        c->pt = pt < 6 ? 6 : pt > 40 ? 40 : pt;
        if (apply_font(c))
            gui_app_font_changed(win->app, c);
    }
    sync_detached(win);
    struct gui_rect r;
    if (gui_app_paint(win->app, c, &r))
        present(c, r.x, r.y, r.w, r.h);
    sync_titlebar(win);
    for (int i = 0; i < win->detached.count; i++)
    {
        struct x11_window* d = win->detached.items[i];
        share_fonts(d, win);
        if (gui_surface_paint(d->app, d->surface, &d->canvas, &r))
            present(&d->canvas, r.x, r.y, r.w, r.h);
        sync_titlebar(d);
    }
    XFlush(c->dpy);
}

/* --- Clipboard (ide_gui_backend.h) ---
 * X11 has no clipboard service: setting it means owning the CLIPBOARD
 * selection and answering SelectionRequest; getting it means asking the
 * owner and waiting for the reply. */

static void answer_selection_request(struct gui_canvas* c, const XSelectionRequestEvent* req)
{
    XEvent reply = { 0 };   /* a whole XEvent: XSendEvent reads that much */
    XSelectionEvent* sel = &reply.xselection;
    sel->type = SelectionNotify;
    sel->requestor = req->requestor;
    sel->selection = req->selection;
    sel->target = req->target;
    sel->time = req->time;
    sel->property = None;
    if ((req->target == c->utf8_string || req->target == XA_STRING) && c->clip_text)
    {
        XChangeProperty(c->dpy, req->requestor, req->property, req->target, 8, PropModeReplace,
                        (const unsigned char*)c->clip_text, (int)strlen(c->clip_text));
        sel->property = req->property;
    }
    else if (req->target == c->targets)
    {
        Atom targets[3] = { c->targets, c->utf8_string, XA_STRING };
        XChangeProperty(c->dpy, req->requestor, req->property, XA_ATOM, 32, PropModeReplace,
                        (const unsigned char*)targets, 3);
        sel->property = req->property;
    }
    XSendEvent(c->dpy, req->requestor, False, 0, &reply);
}

void gui_clipboard_set(struct gui_canvas* c, const char* utf8)
{
    free(c->clip_text);
    size_t len = strlen(utf8);
    c->clip_text = malloc(len + 1);
    if (c->clip_text)
        memcpy(c->clip_text, utf8, len + 1);
    XSetSelectionOwner(c->dpy, c->clipboard, c->win, CurrentTime);
}

static char* read_paste_property(struct gui_canvas* c)
{
    Atom type;
    int format;
    unsigned long count, after;
    unsigned char* data = NULL;
    XGetWindowProperty(c->dpy, c->win, c->paste_prop, 0, 1L << 22, True, AnyPropertyType,
                       &type, &format, &count, &after, &data);
    char* result = NULL;
    if (data && format == 8)
    {
        result = malloc(count + 1);
        if (result)
        {
            /* "\r\n" from other programs back to '\n' */
            size_t o = 0;
            for (unsigned long i = 0; i < count; i++)
            {
                if (data[i] != '\r')
                    result[o++] = (char)data[i];
            }
            result[o] = '\0';
        }
    }
    if (data)
        XFree(data);
    return result;
}

char* gui_clipboard_get(struct gui_canvas* c)
{
    Window owner = XGetSelectionOwner(c->dpy, c->clipboard);
    if (owner == None)
        return NULL;
    if (owner == c->win)
    {
        if (!c->clip_text)
            return NULL;
        size_t len = strlen(c->clip_text);
        char* copy = malloc(len + 1);
        if (copy)
            memcpy(copy, c->clip_text, len + 1);
        return copy;
    }
    XConvertSelection(c->dpy, c->clipboard, c->utf8_string, c->paste_prop, c->win, CurrentTime);
    XFlush(c->dpy);
    /* Waits for the reply, up to 1 s. Other events stay queued for the main
     * loop; only requests for our own selection are answered meanwhile. */
    for (int tries = 0; tries < 1000; tries++)
    {
        XEvent ev;
        if (XCheckTypedWindowEvent(c->dpy, c->win, SelectionNotify, &ev))
            return ev.xselection.property == None ? NULL : read_paste_property(c);
        if (XCheckTypedWindowEvent(c->dpy, c->win, SelectionRequest, &ev))
        {
            answer_selection_request(c, &ev.xselectionrequest);
            continue;
        }
        usleep(1000);
    }
    return NULL;
}

/* --- Input --- */

static int mods_of(unsigned int state)
{
    return (state & ShiftMask ? GUI_MOD_SHIFT : 0) |
           (state & ControlMask ? GUI_MOD_CTRL : 0) |
           (state & Mod1Mask ? GUI_MOD_ALT : 0);
}

/* A keysym as a gui key code, or 0 for keys the framework ignores. */
static int map_keysym(KeySym sym)
{
    if (sym >= XK_a && sym <= XK_z) return (int)(sym - XK_a + 'A');
    if (sym >= XK_A && sym <= XK_Z) return (int)sym;
    if (sym >= XK_0 && sym <= XK_9) return (int)sym;
    if (sym >= XK_F1 && sym <= XK_F12) return GUI_KEY_F1 + (int)(sym - XK_F1);
    switch (sym)
    {
    case XK_Escape: return GUI_KEY_ESCAPE;
    case XK_Return: case XK_KP_Enter: return GUI_KEY_ENTER;
    case XK_Tab: case XK_ISO_Left_Tab: return GUI_KEY_TAB;
    case XK_BackSpace: return GUI_KEY_BACKSPACE;
    case XK_Delete: case XK_KP_Delete: return GUI_KEY_DELETE;
    case XK_Insert: case XK_KP_Insert: return GUI_KEY_INSERT;
    case XK_Home: case XK_KP_Home: return GUI_KEY_HOME;
    case XK_End: case XK_KP_End: return GUI_KEY_END;
    case XK_Page_Up: case XK_KP_Page_Up: return GUI_KEY_PAGEUP;
    case XK_Page_Down: case XK_KP_Page_Down: return GUI_KEY_PAGEDOWN;
    case XK_Left: case XK_KP_Left: return GUI_KEY_LEFT;
    case XK_Right: case XK_KP_Right: return GUI_KEY_RIGHT;
    case XK_Up: case XK_KP_Up: return GUI_KEY_UP;
    case XK_Down: case XK_KP_Down: return GUI_KEY_DOWN;
    case XK_space: return ' ';
    case XK_plus: case XK_equal: case XK_KP_Add: return '+';
    case XK_minus: case XK_KP_Subtract: return '-';
    }
    return 0;
}

static uint32_t utf8_first(const unsigned char* s, int len)
{
    if (len <= 0) return 0;
    if (s[0] < 0x80) return s[0];
    if ((s[0] & 0xE0) == 0xC0 && len >= 2) return (uint32_t)(s[0] & 0x1F) << 6 | (s[1] & 0x3F);
    if ((s[0] & 0xF0) == 0xE0 && len >= 3)
        return (uint32_t)(s[0] & 0x0F) << 12 | (uint32_t)(s[1] & 0x3F) << 6 | (s[2] & 0x3F);
    if ((s[0] & 0xF8) == 0xF0 && len >= 4)
        return (uint32_t)(s[0] & 0x07) << 18 | (uint32_t)(s[1] & 0x3F) << 12 |
               (uint32_t)(s[2] & 0x3F) << 6 | (s[3] & 0x3F);
    return 0;
}

static int utf8_len(unsigned char c)
{
    return c < 0x80 ? 1 : (c & 0xE0) == 0xC0 ? 2 : (c & 0xF0) == 0xE0 ? 3 : (c & 0xF8) == 0xF0 ? 4 : 1;
}

/* The event to the app; the paint waits until every queued event is taken
 * (the main loop), so a burst of motion or key repeats costs one frame, not
 * one each - on XWayland every frame goes through the compositor. */
static void post(struct x11_window* win, const struct gui_event* ev)
{
    if (win->surface)
        gui_surface_event(win->app, win->surface, ev);
    else
        gui_app_event(win->app, ev);
    win->owner->pending = 1;
}

static void update_cursor(struct x11_window* win)
{
    enum gui_cursor cursor = win->surface ? gui_surface_cursor(win->app, win->surface)
                                          : gui_app_cursor(win->app);
    if (cursor == win->cursor)
        return;
    win->cursor = cursor;
    XDefineCursor(win->canvas.dpy, win->canvas.win, win->cursors[cursor]);
}

/* Like Win32: a key event for keys and shortcuts, then a char event for
 * what the key types - none with Ctrl or Alt held. */
static void key_press(struct x11_window* win, XKeyEvent* xkey)
{
    int mods = mods_of(xkey->state);
    int key = map_keysym(XLookupKeysym(xkey, 0));
    if (key)
    {
        struct gui_event ev = { 0 };
        ev.type = GUI_EVENT_KEY;
        ev.key = key;
        ev.mods = mods;
        post(win, &ev);
    }
    if (mods & (GUI_MOD_CTRL | GUI_MOD_ALT))
        return;

    char buf[64];
    KeySym sym;
    int n;
    if (win->xic)
    {
        Status status = 0;
        n = Xutf8LookupString(win->xic, xkey, buf, (int)sizeof buf - 1, &sym, &status);
        if (status != XLookupChars && status != XLookupBoth)
            n = 0;
    }
    else
    {
        n = XLookupString(xkey, buf, (int)sizeof buf - 1, &sym, NULL);
    }
    for (int i = 0; i < n; )
    {
        int len = utf8_len((unsigned char)buf[i]);
        uint32_t ch = utf8_first((const unsigned char*)buf + i, n - i);
        i += len;
        if (ch < 32 || ch == 127)
            continue;
        struct gui_event ev = { 0 };
        ev.type = GUI_EVENT_CHAR;
        ev.ch = ch;
        post(win, &ev);
    }
}

static void button_press(struct x11_window* win, XButtonEvent* xb)
{
    struct gui_event ev = { 0 };
    ev.x = xb->x;
    ev.y = xb->y;
    ev.mods = mods_of(xb->state);
    if (xb->button == 4 || xb->button == 5)
    {
        ev.type = GUI_EVENT_WHEEL;
        ev.wheel = xb->button == 4 ? 120 : -120;
        post(win, &ev);
        return;
    }
    if (xb->button == 6 || xb->button == 7)
    {
        ev.type = GUI_EVENT_WHEEL;
        ev.hwheel = xb->button == 7 ? 120 : -120;
        post(win, &ev);
        return;
    }
    if (xb->button != 1 && xb->button != 3)
        return;
    ev.type = GUI_EVENT_MOUSE_DOWN;
    ev.button = xb->button == 1 ? 0 : 1;
    if (ev.button == 0)
    {
        /* X11 has no double click: a second press soon, a few px away. */
        struct click* last = &win->last_click;
        int near = abs(xb->x - last->x) <= 4 && abs(xb->y - last->y) <= 4;
        ev.double_click = near && last->time && xb->time - last->time < DOUBLE_CLICK_MS;
        last->time = ev.double_click ? 0 : xb->time;
        last->x = xb->x;
        last->y = xb->y;
    }
    post(win, &ev);
}

static void button_release(struct x11_window* win, XButtonEvent* xb)
{
    if (xb->button != 1 && xb->button != 3)
        return;
    struct gui_event ev = { 0 };
    ev.type = GUI_EVENT_MOUSE_UP;
    ev.x = xb->x;
    ev.y = xb->y;
    ev.button = xb->button == 1 ? 0 : 1;
    ev.mods = mods_of(xb->state);
    post(win, &ev);
}

static void handle_event(struct x11_window* win, XEvent* e)
{
    struct gui_canvas* c = &win->canvas;
    switch (e->type)
    {
    case KeyPress:
        key_press(win, &e->xkey);
        break;
    case ButtonPress:
        button_press(win, &e->xbutton);
        break;
    case ButtonRelease:
        button_release(win, &e->xbutton);
        break;
    case MotionNotify:
    {
        struct gui_event ev = { 0 };
        ev.type = GUI_EVENT_MOUSE_MOVE;
        ev.x = e->xmotion.x;
        ev.y = e->xmotion.y;
        ev.mods = mods_of(e->xmotion.state);
        post(win, &ev);
        update_cursor(win);
        break;
    }
    case LeaveNotify:
        if (e->xcrossing.mode == NotifyNormal)
        {
            struct gui_event ev = { 0 };
            ev.type = GUI_EVENT_MOUSE_LEAVE;
            post(win, &ev);
        }
        break;
    case Expose:
        present(c, e->xexpose.x, e->xexpose.y, e->xexpose.width, e->xexpose.height);
        break;
    case ConfigureNotify:
        if (e->xconfigure.width != c->w || e->xconfigure.height != c->h)
        {
            ensure_back_buffer(c, e->xconfigure.width, e->xconfigure.height);
            if (win->surface)
                gui_surface_resize(win->app, win->surface, c->w, c->h);
            else
                gui_app_resize(win->app, c->w, c->h);
            win->owner->pending = 1;   /* a drag sends many: one paint for them */
        }
        break;
    case SelectionRequest:
        answer_selection_request(c, &e->xselectionrequest);
        break;
    case SelectionClear:
        free(c->clip_text);
        c->clip_text = NULL;
        break;
    case ClientMessage:
        if ((Atom)e->xclient.data.l[0] == win->wm_delete)
        {
            struct gui_event ev = { 0 };
            ev.type = GUI_EVENT_CLOSE;
            post(win, &ev);   /* refresh() ends the loop if the app quits */
        }
        break;
    }
}

static void setup_input_method(struct x11_window* win)
{
    struct gui_canvas* c = &win->canvas;
    XSetLocaleModifiers("");
    win->xim = XOpenIM(c->dpy, NULL, NULL, NULL);
    if (!win->xim)
    {
        XSetLocaleModifiers("@im=none");
        win->xim = XOpenIM(c->dpy, NULL, NULL, NULL);
    }
    if (win->xim)
        win->xic = XCreateIC(win->xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
                             XNClientWindow, c->win, XNFocusWindow, c->win, NULL);
}

/* Blocks until an X event arrives or the app's timer is due. */
static void wait_event(struct x11_window* win)
{
    struct gui_canvas* c = &win->canvas;
    if (XPending(c->dpy))
        return;
    int fd = ConnectionNumber(c->dpy);
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    struct timeval tv, *timeout = NULL;
    if (win->timer_ms > 0)
    {
        long long left = win->timer_due - now_ms();
        if (left < 0)
            left = 0;
        tv.tv_sec = (time_t)(left / 1000);
        tv.tv_usec = (suseconds_t)(left % 1000 * 1000);
        timeout = &tv;
    }
    select(fd + 1, &fds, NULL, NULL, timeout);
}

int main(int argc, char** argv)
{
    setlocale(LC_ALL, "");

    struct x11_window win = { 0 };
    win.owner = &win;
    struct gui_canvas* c = &win.canvas;
    c->dpy = XOpenDisplay(NULL);
    if (!c->dpy)
    {
        fprintf(stderr, "cannot open the X display\n");
        return 1;
    }
    c->screen = DefaultScreen(c->dpy);
    c->visual = DefaultVisual(c->dpy, c->screen);
    c->cmap = DefaultColormap(c->dpy, c->screen);
    c->pt = DEFAULT_FONT_PT;
    if (!apply_font(c))
    {
        fprintf(stderr, "no usable font (is fontconfig installed?)\n");
        return 1;
    }

    int w = DEFAULT_COLS * c->metrics.cell_w;
    int h = DEFAULT_ROWS * c->metrics.cell_h;
    XSetWindowAttributes attrs = { 0 };
    attrs.background_pixmap = None;   /* the back buffer covers everything */
    attrs.bit_gravity = NorthWestGravity;
    c->win = XCreateWindow(c->dpy, RootWindow(c->dpy, c->screen), 0, 0, (unsigned)w, (unsigned)h, 0,
                           DefaultDepth(c->dpy, c->screen), InputOutput, c->visual,
                           CWBackPixmap | CWBitGravity, &attrs);
    win.wm_delete = XInternAtom(c->dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(c->dpy, c->win, &win.wm_delete, 1);
    c->clipboard = XInternAtom(c->dpy, "CLIPBOARD", False);
    c->utf8_string = XInternAtom(c->dpy, "UTF8_STRING", False);

    /* "Cake <version>" in the title bar and the taskbar - left empty, the
     * window manager writes its own "Untitled" */
    static const char title[] = "Cake " CAKE_VERSION;
    XStoreName(c->dpy, c->win, title);
    XChangeProperty(c->dpy, c->win, XInternAtom(c->dpy, "_NET_WM_NAME", False), c->utf8_string, 8,
                    PropModeReplace, (const unsigned char*)title, (int)strlen(title));
    XClassHint* class_hint = XAllocClassHint();
    if (class_hint)
    {
        class_hint->res_name = "cakeide";
        class_hint->res_class = "Cake";
        XSetClassHint(c->dpy, c->win, class_hint);
        XFree(class_hint);
    }
    c->targets = XInternAtom(c->dpy, "TARGETS", False);
    c->paste_prop = XInternAtom(c->dpy, "IDE_PASTE", False);

    static const unsigned int shapes[] = {
        XC_left_ptr, XC_sb_h_double_arrow, XC_sb_v_double_arrow,
        XC_bottom_right_corner, XC_bottom_left_corner,
    };
    for (int i = 0; i < 5; i++)
        win.cursors[i] = XCreateFontCursor(c->dpy, shapes[i]);
    XDefineCursor(c->dpy, c->win, win.cursors[GUI_CURSOR_ARROW]);

    setup_input_method(&win);
    XSelectInput(c->dpy, c->win, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask |
                 PointerMotionMask | LeaveWindowMask | StructureNotifyMask);
    c->gc = XCreateGC(c->dpy, c->win, 0, NULL);
    ensure_back_buffer(c, w, h);

    win.app = gui_app_create();
    offer_fonts(&win);
    gui_app_start(win.app, c, argc, argv);
    gui_app_resize(win.app, c->w, c->h);
    win.running = 1;
    refresh(&win);
    XMapWindow(c->dpy, c->win);

    while (win.running)
    {
        wait_event(&win);
        while (win.running && XPending(c->dpy))
        {
            XEvent e;
            XNextEvent(c->dpy, &e);
            if (XFilterEvent(&e, None))
                continue;   /* taken by the input method */
            struct x11_window* target = window_for(&win, e.xany.window);
            if (target)
                handle_event(target, &e);
        }
        if (win.running && win.timer_ms > 0 && now_ms() >= win.timer_due)
        {
            win.timer_due = now_ms() + win.timer_ms;
            struct gui_event ev = { 0 };
            ev.type = GUI_EVENT_TIMER;
            post(&win, &ev);
        }
        if (win.pending)
        {
            win.pending = 0;
            refresh(&win);   /* once for everything taken above */
        }
    }

    while (win.detached.count > 0)
        detached_destroy(win.detached.items[--win.detached.count]);
    free(win.detached.items);
    gui_app_free(win.app);
    if (win.xic) XDestroyIC(win.xic);
    if (win.xim) XCloseIM(win.xim);
    XftDrawDestroy(c->draw);
    XRenderFreePicture(c->dpy, c->picture);
    XFreePixmap(c->dpy, c->pixmap);
    XftFontClose(c->dpy, c->main);
    XftFontClose(c->dpy, c->small_font);
    for (int i = 0; i < 5; i++)
        XFreeCursor(c->dpy, win.cursors[i]);
    XFreeGC(c->dpy, c->gc);
    XDestroyWindow(c->dpy, c->win);
    XCloseDisplay(c->dpy);
    free(c->clip_text);
    return 0;
}
