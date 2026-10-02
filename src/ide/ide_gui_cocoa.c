/* ide_gui_cocoa.c - macOS Cocoa/CoreText backend of the ide GUI framework.
 *
 * Only translates Cocoa events into calls to the core and implements the
 * drawing primitives of ide_gui_backend.h. Everything about a window
 * lives in one struct cocoa_window, reached from the view and the delegate
 * through an instance variable - no globals. The main window owns the
 * detached ones (gui_window_detach): one more NSWindow each, sharing its
 * fonts.
 *
 * Plain C: AppKit is driven through the objc runtime (objc_msgSend);
 * CoreGraphics and CoreText are C APIs. Cocoa objects created here live as
 * long as the process and are never released.
 *
 * Build: clang ... ide_gui_cocoa.c -framework Cocoa -framework CoreText
 *            -framework CoreGraphics -lobjc
 */
#include "ide_gui_backend.h"
#include "ide_icon.h"

#include <objc/runtime.h>
#include <objc/message.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <CoreText/CoreText.h>

#include <stdlib.h>
#include <string.h>

#define DEFAULT_FONT_PT 13
#define DEFAULT_COLS 120
#define DEFAULT_ROWS 30
#define SMALL_FONT_PERCENT 85

static const char* const font_candidates[] = { "Menlo", "SF Mono", "Monaco", "Courier New" };

#define CANDIDATE_COUNT ((int)(sizeof font_candidates / sizeof font_candidates[0]))

/* AppKit constants exported as symbols; their headers are Objective-C. */
extern id NSPasteboardTypeString;
extern id NSAppearanceNameAqua;
extern id NSAppearanceNameDarkAqua;

#define MSG(ret, ...) ((ret (*)(__VA_ARGS__))objc_msgSend)

static id cls(const char* name) { return (id)objc_getClass(name); }
static SEL sel(const char* name) { return sel_registerName(name); }

struct gui_canvas
{
    CGContextRef ctx;            /* the back buffer, flipped: y grows down */
    void* data;
    int w, h;                    /* points */
    double scale;                /* backing pixels per point */
    CGColorSpaceRef colorspace;
    int clipped;                 /* a gui_set_clip state is saved on ctx */
    CTFontRef main;              /* GUI_FONT_MAIN */
    CTFontRef small_font;        /* GUI_FONT_SMALL */
    int pt;
    const char* family;          /* NULL: the first installed candidate */
    struct gui_metrics metrics;
    struct gui_metrics small_metrics;
};

struct offered_fonts
{
    int candidate[16];
    int count;
};

struct cocoa_window;

/* The main window's detached windows - open, or closed and waiting to be
 * freed (a handler of theirs may still be running). */
struct detached_list
{
    struct cocoa_window** items;
    int count, cap;
};

struct cocoa_window
{
    struct gui_canvas canvas;
    struct gui_app* app;
    id window, view, timer, delegate;
    struct offered_fonts fonts;
    int started;                 /* gui_app_start has run */
    double wheel_rest;           /* precise scrolling not yet sent */
    double hwheel_rest;
    uint32_t titlebar_color;
    int titlebar_applied;
    struct cocoa_window* owner;  /* the main window; itself for the main window */
    struct gui_surface* surface; /* a detached window's; NULL for the main window */
    struct detached_list detached;  /* the main window's */
    struct detached_list gone;      /* the main window's: closed, to free */
};

/* The struct cocoa_window behind a view or the delegate. */
static struct cocoa_window* backend_of(id obj)
{
    Ivar iv = class_getInstanceVariable(object_getClass(obj), "backend");
    return iv ? (struct cocoa_window*)(void*)object_getIvar(obj, iv) : NULL;
}

static void set_backend(id obj, struct cocoa_window* win)
{
    Ivar iv = class_getInstanceVariable(object_getClass(obj), "backend");
    if (iv)
        object_setIvar(obj, iv, (id)(void*)win);
}

static CFStringRef cfstr(const char* utf8)
{
    return CFStringCreateWithCString(kCFAllocatorDefault, utf8, kCFStringEncodingUTF8);
}

static void set_fill(CGContextRef ctx, uint32_t rgb)
{
    CGContextSetRGBFillColor(ctx, ((rgb >> 16) & 0xFF) / 255.0, ((rgb >> 8) & 0xFF) / 255.0,
                             (rgb & 0xFF) / 255.0, 1.0);
}

/* --- Fonts --- */

/* CoreText substitutes instead of failing, so a family is installed only
 * if the font it gives has that very name. */
static CTFontRef open_font(const char* family, CGFloat pt)
{
    CFStringRef name = cfstr(family);
    CTFontRef font = CTFontCreateWithName(name, pt, NULL);
    CFStringRef got = font ? CTFontCopyFamilyName(font) : NULL;
    int same = got && CFStringCompare(got, name, kCFCompareCaseInsensitive) == kCFCompareEqualTo;
    if (got)
        CFRelease(got);
    CFRelease(name);
    if (!same && font)
    {
        CFRelease(font);
        font = NULL;
    }
    return font;
}

static CTFontRef make_font(struct gui_canvas* c, int pt, struct gui_metrics* m)
{
    CTFontRef font = c->family ? open_font(c->family, pt) : NULL;
    for (int i = 0; !font && i < CANDIDATE_COUNT; i++)
        font = open_font(font_candidates[i], pt);
    if (!font)
        font = CTFontCreateUIFontForLanguage(kCTFontUIFontUserFixedPitch, pt, NULL);
    if (!font)
        return NULL;
    UniChar ch = 'M';
    CGGlyph glyph = 0;
    CGSize advance = { 0, 0 };
    if (CTFontGetGlyphsForCharacters(font, &ch, &glyph, 1))
        CTFontGetAdvancesForGlyphs(font, kCTFontOrientationDefault, &glyph, &advance, 1);
    double ascent = CTFontGetAscent(font);
    m->cell_w = advance.width > 0 ? (int)(advance.width + 0.5) : (int)(pt * 0.6 + 0.5);
    m->cell_h = (int)(ascent + CTFontGetDescent(font) + CTFontGetLeading(font) + 0.5);
    m->ascent = (int)(ascent + 0.5);
    if (m->cell_w < 1) m->cell_w = 1;
    if (m->cell_h < 1) m->cell_h = 1;
    return font;
}

static void offer_fonts(struct cocoa_window* win)
{
    const char* names[CANDIDATE_COUNT];
    win->fonts.count = 0;
    for (int i = 0; i < CANDIDATE_COUNT; i++)
    {
        CTFontRef f = open_font(font_candidates[i], DEFAULT_FONT_PT);
        if (!f)
            continue;
        CFRelease(f);
        names[win->fonts.count] = font_candidates[i];
        win->fonts.candidate[win->fonts.count++] = i;
    }
    gui_app_set_fonts(win->app, names, win->fonts.count, 0);
}

/* (Re)creates both fonts from canvas->pt: they always exist together. */
static int apply_font(struct gui_canvas* c)
{
    int small_pt = (c->pt * SMALL_FONT_PERCENT + 50) / 100;
    struct gui_metrics m, sm;
    CTFontRef main = make_font(c, c->pt, &m);
    CTFontRef small_font = make_font(c, small_pt > 0 ? small_pt : 1, &sm);
    if (!main || !small_font)
    {
        if (main) CFRelease(main);
        if (small_font) CFRelease(small_font);
        return 0;
    }
    if (c->main) CFRelease(c->main);
    if (c->small_font) CFRelease(c->small_font);
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
    int size = (10 * c->pt + DEFAULT_FONT_PT / 2) / DEFAULT_FONT_PT;
    return size > 2 ? size : 2;
}

void gui_set_clip(struct gui_canvas* c, int x, int y, int w, int h)
{
    if (c->clipped)
        CGContextRestoreGState(c->ctx);
    c->clipped = w > 0 && h > 0;
    if (c->clipped)
    {
        CGContextSaveGState(c->ctx);
        CGContextClipToRect(c->ctx, CGRectMake(x, y, w, h));
    }
}

void gui_fill_rect(struct gui_canvas* c, int x, int y, int w, int h, uint32_t rgb)
{
    if (w <= 0 || h <= 0)
        return;
    set_fill(c->ctx, rgb);
    CGContextFillRect(c->ctx, CGRectMake(x, y, w, h));
}

void gui_shade_rect(struct gui_canvas* c, int x, int y, int w, int h, int alpha)
{
    if (w <= 0 || h <= 0)
        return;
    CGContextSetRGBFillColor(c->ctx, 0, 0, 0, alpha / 255.0);
    CGContextFillRect(c->ctx, CGRectMake(x, y, w, h));
}

void gui_draw_text(struct gui_canvas* c, int x, int y, const uint32_t* cps, int count,
                   uint32_t fg, uint32_t bg, enum gui_font font)
{
    const struct gui_metrics* m = font == GUI_FONT_SMALL ? &c->small_metrics : &c->metrics;
    CTFontRef ct = font == GUI_FONT_SMALL ? c->small_font : c->main;
    gui_fill_rect(c, x, y, count * m->cell_w, m->cell_h, bg);

    CGContextRef ctx = c->ctx;
    CGContextSaveGState(ctx);
    CGContextClipToRect(ctx, CGRectMake(x, y, count * m->cell_w, m->cell_h));
    CGContextSetShouldSubpixelPositionFonts(ctx, false);
    CGContextSetShouldSubpixelQuantizeFonts(ctx, false);
    set_fill(ctx, fg);
    /* CoreText draws y-up: flip back around the baseline. */
    CGContextTranslateCTM(ctx, x, y + m->ascent);
    CGContextScaleCTM(ctx, 1, -1);

    CGGlyph glyphs[256];
    CGPoint pos[256];
    for (int start = 0; start < count; start += 256)
    {
        int n = count - start < 256 ? count - start : 256;
        int k = 0;
        for (int i = 0; i < n; i++)
        {
            uint32_t cp = cps[start + i];
            if (cp == ' ' || cp == 0)
                continue;
            UniChar units[2];
            int len = 1;
            if (cp > 0xFFFF)
            {
                units[0] = (UniChar)(0xD800 + ((cp - 0x10000) >> 10));
                units[1] = (UniChar)(0xDC00 + ((cp - 0x10000) & 0x3FF));
                len = 2;
            }
            else
            {
                units[0] = (UniChar)cp;
            }
            CGGlyph g[2] = { 0, 0 };
            if (!CTFontGetGlyphsForCharacters(ct, units, g, len) || !g[0])
                continue;   /* not in the font */
            glyphs[k] = g[0];
            pos[k].x = (start + i) * m->cell_w;
            pos[k].y = 0;
            k++;
        }
        if (k > 0)
            CTFontDrawGlyphs(ct, glyphs, pos, (size_t)k, ctx);
    }
    CGContextRestoreGState(ctx);
}

/* --- Back buffer --- */

static void ensure_back_buffer(struct cocoa_window* win, int w, int h)
{
    struct gui_canvas* c = &win->canvas;
    if (w < 1) w = 1;
    if (h < 1) h = 1;
    double scale = MSG(CGFloat, id, SEL)(win->window, sel("backingScaleFactor"));
    if (scale <= 0)
        scale = 1;
    if (c->ctx && w == c->w && h == c->h && scale == c->scale)
        return;
    int pw = (int)(w * scale + 0.5);
    int ph = (int)(h * scale + 0.5);
    if (c->ctx)
        CGContextRelease(c->ctx);
    free(c->data);
    c->data = calloc((size_t)pw * (size_t)ph, 4);
    c->ctx = CGBitmapContextCreate(c->data, (size_t)pw, (size_t)ph, 8, (size_t)pw * 4, c->colorspace,
                                   kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Host);
    /* Flipped once for the context's life, then scaled to points. */
    CGContextTranslateCTM(c->ctx, 0, ph);
    CGContextScaleCTM(c->ctx, 1, -1);
    CGContextScaleCTM(c->ctx, scale, scale);
    c->w = w;
    c->h = h;
    c->scale = scale;
    c->clipped = 0;
    if (win->surface)
        gui_surface_invalidate(win->app, win->surface);
    else if (win->app)
        gui_app_invalidate(win->app);   /* a new buffer: everything drawn again */
}

/* Tints the title bar with the app's chrome color, and only when it
 * changed. */
static void sync_titlebar(struct cocoa_window* win)
{
    uint32_t color = gui_app_titlebar_color(win->app);
    if (win->titlebar_applied && win->titlebar_color == color)
        return;
    win->titlebar_color = color;
    win->titlebar_applied = 1;
    double r = ((color >> 16) & 0xFF) / 255.0;
    double g = ((color >> 8) & 0xFF) / 255.0;
    double b = (color & 0xFF) / 255.0;
    MSG(void, id, SEL, BOOL)(win->window, sel("setTitlebarAppearsTransparent:"), YES);
    id ns_color = MSG(id, id, SEL, CGFloat, CGFloat, CGFloat, CGFloat)(
        cls("NSColor"), sel("colorWithSRGBRed:green:blue:alpha:"), r, g, b, 1.0);
    MSG(void, id, SEL, id)(win->window, sel("setBackgroundColor:"), ns_color);
    double luma = 0.2126 * r + 0.7152 * g + 0.0722 * b;
    id appearance = MSG(id, id, SEL, id)(cls("NSAppearance"), sel("appearanceNamed:"),
                                         luma < 0.5 ? NSAppearanceNameDarkAqua : NSAppearanceNameAqua);
    MSG(void, id, SEL, id)(win->window, sel("setAppearance:"), appearance);
}

static void set_timer(struct cocoa_window* win, int ms)
{
    if (win->timer)
    {
        MSG(void, id, SEL)(win->timer, sel("invalidate"));
        win->timer = NULL;
    }
    if (ms > 0)
    {
        id delegate = MSG(id, id, SEL)(cls("NSApplication"), sel("sharedApplication"));
        delegate = MSG(id, id, SEL)(delegate, sel("delegate"));
        win->timer = MSG(id, id, SEL, double, id, SEL, id, BOOL)(
            cls("NSTimer"), sel("scheduledTimerWithTimeInterval:target:selector:userInfo:repeats:"),
            ms / 1000.0, delegate, sel("timerFired:"), NULL, YES);
    }
}

static void update_cursor(struct cocoa_window* win)
{
    static const char* const names[] = {
        "arrowCursor", "resizeLeftRightCursor", "resizeUpDownCursor",
        "_windowResizeNorthWestSouthEastCursor", "_windowResizeNorthEastSouthWestCursor",
    };
    id cursor_class = cls("NSCursor");
    enum gui_cursor cursor = win->surface ? gui_surface_cursor(win->app, win->surface) : gui_app_cursor(win->app);
    SEL s = sel(names[cursor]);
    if (!MSG(BOOL, id, SEL, SEL)(cursor_class, sel("respondsToSelector:"), s))
        s = sel("arrowCursor");
    MSG(void, id, SEL)(MSG(id, id, SEL)(cursor_class, s), sel("set"));
}

/* --- Detached windows --- */

int gui_backend_can_detach(void)
{
    return 1;
}

static void list_add(struct detached_list* list, struct cocoa_window* w)
{
    if (list->count == list->cap)
    {
        int cap = list->cap ? list->cap * 2 : 4;
        struct cocoa_window** items = realloc(list->items, (size_t)cap * sizeof *items);
        if (!items)
            abort();
        list->items = items;
        list->cap = cap;
    }
    list->items[list->count++] = w;
}

static void list_remove(struct detached_list* list, struct cocoa_window* w)
{
    for (int i = 0; i < list->count; i++)
    {
        if (list->items[i] == w)
        {
            memmove(&list->items[i], &list->items[i + 1], sizeof list->items[0] * (size_t)(list->count - i - 1));
            list->count--;
            return;
        }
    }
}

/* A detached window draws with the main window's fonts. */
static void share_fonts(struct cocoa_window* d, const struct cocoa_window* main)
{
    struct gui_canvas* c = &d->canvas;
    c->main = main->canvas.main;
    c->small_font = main->canvas.small_font;
    c->pt = main->canvas.pt;
    c->family = main->canvas.family;
    c->metrics = main->canvas.metrics;
    c->small_metrics = main->canvas.small_metrics;
}

static id make_view(struct cocoa_window* win, CGRect frame);

static void detached_open(struct cocoa_window* main, struct gui_surface* s, int w, int h)
{
    struct cocoa_window* d = calloc(1, sizeof *d);
    if (!d)
        abort();
    d->app = main->app;
    d->owner = main;
    d->surface = s;
    d->canvas.colorspace = main->canvas.colorspace;
    share_fonts(d, main);

    CGRect frame = CGRectMake(0, 0, w > 0 ? w : 640, h > 0 ? h : 480);
    unsigned long style = 1 | 2 | 4 | 8;   /* titled, closable, miniaturizable, resizable */
    d->window = MSG(id, id, SEL)(cls("NSWindow"), sel("alloc"));
    d->window = MSG(id, id, SEL, CGRect, unsigned long, unsigned long, BOOL)(
        d->window, sel("initWithContentRect:styleMask:backing:defer:"), frame, style, 2UL, NO);
    MSG(void, id, SEL, BOOL)(d->window, sel("setReleasedWhenClosed:"), NO);
    CFStringRef title = cfstr("");
    MSG(void, id, SEL, id)(d->window, sel("setTitle:"), (id)title);
    CFRelease(title);
    d->delegate = MSG(id, id, SEL)((id)objc_getClass("IdeGuiDelegate"), sel("new"));
    set_backend(d->delegate, d);
    MSG(void, id, SEL, id)(d->window, sel("setDelegate:"), d->delegate);
    MSG(void, id, SEL, BOOL)(d->window, sel("setAcceptsMouseMovedEvents:"), YES);
    d->view = make_view(d, frame);

    list_add(&main->detached, d);
    ensure_back_buffer(d, (int)frame.size.width, (int)frame.size.height);
    d->started = 1;
    gui_surface_start(d->app, s, d, d->canvas.w, d->canvas.h);
    MSG(void, id, SEL)(d->window, sel("center"));
    MSG(void, id, SEL, id)(d->window, sel("makeKeyAndOrderFront:"), NULL);
}

/* Its surface is gone: the window goes off screen and stops reaching us;
 * the struct is freed on a later refresh, since a handler of it may be
 * running now. */
static void detached_close(struct cocoa_window* main, struct cocoa_window* d)
{
    list_remove(&main->detached, d);
    d->started = 0;
    d->surface = NULL;
    set_backend(d->view, NULL);
    set_backend(d->delegate, NULL);
    MSG(void, id, SEL, id)(d->window, sel("setDelegate:"), NULL);
    MSG(void, id, SEL, id)(d->window, sel("orderOut:"), NULL);
    list_add(&main->gone, d);
}

static void free_gone(struct cocoa_window* main)
{
    while (main->gone.count > 0)
    {
        struct cocoa_window* d = main->gone.items[--main->gone.count];
        if (d->canvas.ctx)
            CGContextRelease(d->canvas.ctx);
        free(d->canvas.data);
        free(d);
    }
}

/* The windows the core asked for, made, raised and closed. */
static void sync_detached(struct cocoa_window* main)
{
    int w, h;
    struct gui_surface* s;
    while ((s = gui_app_take_surface_open(main->app, &w, &h)) != NULL)
        detached_open(main, s, w, h);
    void* native;
    while ((native = gui_app_take_surface_close(main->app)) != NULL)
        detached_close(main, native);
    while ((native = gui_app_take_surface_raise(main->app)) != NULL)
    {
        struct cocoa_window* d = native;
        MSG(void, id, SEL, id)(d->window, sel("makeKeyAndOrderFront:"), NULL);
    }
}

/* Applies what the app asked for (quit, timer, font, zoom), lets the core
 * paint whatever is dirty and invalidates just that rect - in the main
 * window and in each detached one. */
static void refresh(struct cocoa_window* win)
{
    win = win->owner;
    if (!win || !win->started)
        return;
    free_gone(win);
    struct gui_canvas* c = &win->canvas;
    if (gui_app_should_quit(win->app))
    {
        id app = MSG(id, id, SEL)(cls("NSApplication"), sel("sharedApplication"));
        MSG(void, id, SEL, id)(app, sel("terminate:"), NULL);
        return;
    }
    int ms;
    if (gui_app_take_timer(win->app, &ms))
        set_timer(win, ms);
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
        c->pt = pt < 8 ? 8 : pt > 48 ? 48 : pt;
        if (apply_font(c))
            gui_app_font_changed(win->app, c);
    }
    sync_detached(win);
    struct gui_rect r;
    if (gui_app_paint(win->app, c, &r))
        MSG(void, id, SEL, CGRect)(win->view, sel("setNeedsDisplayInRect:"),
                                    CGRectMake(r.x, r.y, r.w, r.h));
    sync_titlebar(win);
    for (int i = 0; i < win->detached.count; i++)
    {
        struct cocoa_window* d = win->detached.items[i];
        share_fonts(d, win);
        if (gui_surface_paint(d->app, d->surface, &d->canvas, &r))
            MSG(void, id, SEL, CGRect)(d->view, sel("setNeedsDisplayInRect:"),
                                        CGRectMake(r.x, r.y, r.w, r.h));
        sync_titlebar(d);
    }
}

/* --- Clipboard (ide_gui_backend.h) --- */

void gui_clipboard_set(struct gui_canvas* c, const char* utf8)
{
    (void)c;
    id pasteboard = MSG(id, id, SEL)(cls("NSPasteboard"), sel("generalPasteboard"));
    MSG(long, id, SEL)(pasteboard, sel("clearContents"));
    CFStringRef s = cfstr(utf8);
    if (!s)
        return;
    MSG(BOOL, id, SEL, id, id)(pasteboard, sel("setString:forType:"), (id)s, NSPasteboardTypeString);
    CFRelease(s);
}

char* gui_clipboard_get(struct gui_canvas* c)
{
    (void)c;
    id pasteboard = MSG(id, id, SEL)(cls("NSPasteboard"), sel("generalPasteboard"));
    id s = MSG(id, id, SEL, id)(pasteboard, sel("stringForType:"), NSPasteboardTypeString);
    const char* utf8 = s ? MSG(const char*, id, SEL)(s, sel("UTF8String")) : NULL;
    if (!utf8)
        return NULL;
    char* result = malloc(strlen(utf8) + 1);
    if (result)
    {
        /* "\r\n" and lone '\r' from other programs to '\n' */
        char* o = result;
        for (const char* p = utf8; *p; p++)
        {
            if (*p == '\r')
            {
                if (p[1] != '\n')
                    *o++ = '\n';
            }
            else
            {
                *o++ = *p;
            }
        }
        *o = '\0';
    }
    return result;
}

/* --- Input --- */

#define MOD_SHIFT   (1UL << 17)
#define MOD_CONTROL (1UL << 18)
#define MOD_OPTION  (1UL << 19)
#define MOD_COMMAND (1UL << 20)

/* Command is the primary modifier (GUI_MOD_PRIMARY): Cmd+C copies, as on any Mac. */
static int mods_of(id event)
{
    unsigned long flags = MSG(unsigned long, id, SEL)(event, sel("modifierFlags"));
    return (flags & MOD_SHIFT ? GUI_MOD_SHIFT : 0) |
           (flags & MOD_CONTROL ? GUI_MOD_CTRL : 0) |
           (flags & MOD_COMMAND ? GUI_MOD_CMD : 0) |   /* the primary shortcuts' (GUI_MOD_PRIMARY) */
           (flags & MOD_OPTION ? GUI_MOD_ALT : 0);
}

/* Mac virtual key codes (ANSI keyboard) of the keys that type nothing. */
static int map_keycode(unsigned short kc)
{
    switch (kc)
    {
    case 53: return GUI_KEY_ESCAPE;
    case 36: case 76: return GUI_KEY_ENTER;
    case 48: return GUI_KEY_TAB;
    case 51: return GUI_KEY_BACKSPACE;
    case 117: return GUI_KEY_DELETE;
    case 114: return GUI_KEY_INSERT;   /* Help, where Insert is on PC keyboards */
    case 115: return GUI_KEY_HOME;
    case 119: return GUI_KEY_END;
    case 116: return GUI_KEY_PAGEUP;
    case 121: return GUI_KEY_PAGEDOWN;
    case 123: return GUI_KEY_LEFT;
    case 124: return GUI_KEY_RIGHT;
    case 126: return GUI_KEY_UP;
    case 125: return GUI_KEY_DOWN;
    case 122: return GUI_KEY_F1;
    case 120: return GUI_KEY_F1 + 1;
    case 99: return GUI_KEY_F1 + 2;
    case 118: return GUI_KEY_F1 + 3;
    case 96: return GUI_KEY_F1 + 4;
    case 97: return GUI_KEY_F1 + 5;
    case 98: return GUI_KEY_F1 + 6;
    case 100: return GUI_KEY_F1 + 7;
    case 101: return GUI_KEY_F1 + 8;
    case 109: return GUI_KEY_F1 + 9;
    case 103: return GUI_KEY_F1 + 10;
    case 111: return GUI_KEY_F1 + 11;
    }
    return 0;
}

/* Letters, digits, '+', '-' and ' ' from the key's unmodified character. */
static int map_character(id event)
{
    id chars = MSG(id, id, SEL)(event, sel("charactersIgnoringModifiers"));
    const char* s = chars ? MSG(const char*, id, SEL)(chars, sel("UTF8String")) : NULL;
    if (!s || !s[0] || s[1])
        return 0;
    char ch = s[0];
    if (ch >= 'a' && ch <= 'z') return ch - 'a' + 'A';
    if ((ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == ' ' || ch == '-') return ch;
    if (ch == '+' || ch == '=') return '+';
    return 0;
}

static void post(struct cocoa_window* win, const struct gui_event* ev)
{
    if (!win || !win->started)
        return;
    if (win->surface)
        gui_surface_event(win->app, win->surface, ev);
    else
        gui_app_event(win->app, ev);
    refresh(win);
}

static CGPoint local_point(id view, id event)
{
    CGPoint p = MSG(CGPoint, id, SEL)(event, sel("locationInWindow"));
    return MSG(CGPoint, id, SEL, CGPoint, id)(view, sel("convertPoint:fromView:"), p, NULL);
}

static void post_mouse(id view, id event, enum gui_event_type type, int button)
{
    struct cocoa_window* win = backend_of(view);
    CGPoint p = local_point(view, event);
    struct gui_event ev = { 0 };
    ev.type = type;
    ev.x = (int)p.x;
    ev.y = (int)p.y;
    ev.button = button;
    ev.mods = mods_of(event);
    if (type == GUI_EVENT_MOUSE_DOWN && button == 0)
    {
        long clicks = MSG(long, id, SEL)(event, sel("clickCount"));
        ev.double_click = clicks >= 2 && clicks % 2 == 0;
    }
    post(win, &ev);
    if (win && win->started)
        update_cursor(win);
}

static void view_mouse_down(id self, SEL cmd, id event) { (void)cmd; post_mouse(self, event, GUI_EVENT_MOUSE_DOWN, 0); }
static void view_mouse_up(id self, SEL cmd, id event) { (void)cmd; post_mouse(self, event, GUI_EVENT_MOUSE_UP, 0); }
static void view_right_down(id self, SEL cmd, id event) { (void)cmd; post_mouse(self, event, GUI_EVENT_MOUSE_DOWN, 1); }
static void view_right_up(id self, SEL cmd, id event) { (void)cmd; post_mouse(self, event, GUI_EVENT_MOUSE_UP, 1); }
static void view_mouse_moved(id self, SEL cmd, id event) { (void)cmd; post_mouse(self, event, GUI_EVENT_MOUSE_MOVE, 0); }

static void view_mouse_exited(id self, SEL cmd, id event)
{
    (void)cmd; (void)event;
    struct gui_event ev = { 0 };
    ev.type = GUI_EVENT_MOUSE_LEAVE;
    post(backend_of(self), &ev);
}

/* A notch is 120 units, as on Windows; a touchpad's precise deltas are
 * points, 40 units per row of the main font. */
static void view_scroll_wheel(id self, SEL cmd, id event)
{
    (void)cmd;
    struct cocoa_window* win = backend_of(self);
    if (!win || !win->started)
        return;
    double units, hunits;
    if (MSG(BOOL, id, SEL)(event, sel("hasPreciseScrollingDeltas")))
    {
        units = MSG(CGFloat, id, SEL)(event, sel("scrollingDeltaY")) * 40.0 / win->canvas.metrics.cell_h;
        hunits = -MSG(CGFloat, id, SEL)(event, sel("scrollingDeltaX")) * 40.0 / win->canvas.metrics.cell_w;
    }
    else
    {
        units = MSG(CGFloat, id, SEL)(event, sel("deltaY")) * 40.0;   /* a notch: one line, as the old IDE on macOS */
        hunits = -MSG(CGFloat, id, SEL)(event, sel("deltaX")) * 40.0;
    }
    win->wheel_rest += units;
    win->hwheel_rest += hunits;
    int wheel = (int)win->wheel_rest;
    win->wheel_rest -= wheel;
    int hwheel = (int)win->hwheel_rest;
    win->hwheel_rest -= hwheel;
    if (wheel == 0 && hwheel == 0)
        return;
    CGPoint p = local_point(self, event);
    struct gui_event ev = { 0 };
    ev.type = GUI_EVENT_WHEEL;
    ev.x = (int)p.x;
    ev.y = (int)p.y;
    ev.wheel = wheel;
    ev.hwheel = hwheel;
    ev.mods = mods_of(event);
    post(win, &ev);
}

/* Like Win32: a key event for keys and shortcuts, then the text the key
 * types - through interpretKeyEvents:, so dead keys compose - unless Ctrl
 * or Cmd is held, or Option made it a shortcut. */
static void view_key_down(id self, SEL cmd, id event)
{
    (void)cmd;
    struct cocoa_window* win = backend_of(self);
    if (!win || !win->started)
        return;
    unsigned short kc = MSG(unsigned short, id, SEL)(event, sel("keyCode"));
    int mods = mods_of(event);
    int key = map_keycode(kc);
    int special = key != 0;
    if (!key)
        key = map_character(event);
    if (key)
    {
        struct gui_event ev = { 0 };
        ev.type = GUI_EVENT_KEY;
        ev.key = key;
        ev.mods = mods;
        post(win, &ev);
    }
    if (special || (mods & (GUI_MOD_CTRL | GUI_MOD_CMD)) || ((mods & GUI_MOD_ALT) && key))
        return;
    id events = MSG(id, id, SEL, id)(cls("NSArray"), sel("arrayWithObject:"), event);
    MSG(void, id, SEL, id)(self, sel("interpretKeyEvents:"), events);
}

/* NSTextInputClient, reduced to what composition needs: the committed
 * text. Marked text (an unfinished dead key) is not shown. */
struct ns_range
{
    unsigned long location, length;
};

#define NOT_FOUND ((unsigned long)-1L)

static void view_insert_text(id self, SEL cmd, id string, struct ns_range replace)
{
    (void)cmd; (void)replace;
    if (MSG(BOOL, id, SEL, id)(string, sel("isKindOfClass:"), cls("NSAttributedString")))
        string = MSG(id, id, SEL)(string, sel("string"));
    const char* s = MSG(const char*, id, SEL)(string, sel("UTF8String"));
    struct cocoa_window* win = backend_of(self);
    while (s && *s)
    {
        const unsigned char* u = (const unsigned char*)s;
        uint32_t ch;
        int len;
        if (u[0] < 0x80) { ch = u[0]; len = 1; }
        else if ((u[0] & 0xE0) == 0xC0 && u[1]) { ch = (uint32_t)(u[0] & 0x1F) << 6 | (u[1] & 0x3F); len = 2; }
        else if ((u[0] & 0xF0) == 0xE0 && u[1] && u[2])
        { ch = (uint32_t)(u[0] & 0x0F) << 12 | (uint32_t)(u[1] & 0x3F) << 6 | (u[2] & 0x3F); len = 3; }
        else if ((u[0] & 0xF8) == 0xF0 && u[1] && u[2] && u[3])
        {
            ch = (uint32_t)(u[0] & 0x07) << 18 | (uint32_t)(u[1] & 0x3F) << 12 |
                 (uint32_t)(u[2] & 0x3F) << 6 | (u[3] & 0x3F);
            len = 4;
        }
        else break;
        s += len;
        if (ch < 32 || ch == 127)
            continue;
        struct gui_event ev = { 0 };
        ev.type = GUI_EVENT_CHAR;
        ev.ch = ch;
        post(win, &ev);
    }
}

static void view_set_marked_text(id self, SEL cmd, id string, struct ns_range sel_range, struct ns_range replace)
{
    (void)self; (void)cmd; (void)string; (void)sel_range; (void)replace;
}

static void view_unmark_text(id self, SEL cmd) { (void)self; (void)cmd; }
static BOOL view_has_marked_text(id self, SEL cmd) { (void)self; (void)cmd; return NO; }

static struct ns_range view_no_range(id self, SEL cmd)
{
    (void)self; (void)cmd;
    struct ns_range r = { NOT_FOUND, 0 };
    return r;
}

static id view_valid_attributes(id self, SEL cmd)
{
    (void)self; (void)cmd;
    return MSG(id, id, SEL)(cls("NSArray"), sel("array"));
}

static id view_attributed_substring(id self, SEL cmd, struct ns_range range, struct ns_range* actual)
{
    (void)self; (void)cmd; (void)range;
    if (actual) { actual->location = NOT_FOUND; actual->length = 0; }
    return NULL;
}

static unsigned long view_character_index(id self, SEL cmd, CGPoint p)
{
    (void)self; (void)cmd; (void)p;
    return NOT_FOUND;
}

static CGRect view_first_rect(id self, SEL cmd, struct ns_range range, struct ns_range* actual)
{
    (void)self; (void)cmd; (void)range;
    if (actual) { actual->location = NOT_FOUND; actual->length = 0; }
    return CGRectMake(0, 0, 0, 0);
}

static void view_do_command(id self, SEL cmd, SEL command) { (void)self; (void)cmd; (void)command; }

/* --- The view --- */

static void view_draw_rect(id self, SEL cmd, CGRect dirty)
{
    (void)cmd;
    struct cocoa_window* win = backend_of(self);
    if (!win || !win->canvas.ctx)
        return;
    struct gui_canvas* c = &win->canvas;
    id ns_ctx = MSG(id, id, SEL)(cls("NSGraphicsContext"), sel("currentContext"));
    CGContextRef ctx = (CGContextRef)MSG(void*, id, SEL)(ns_ctx, sel("CGContext"));
    CGImageRef img = ctx ? CGBitmapContextCreateImage(c->ctx) : NULL;
    if (!img)
        return;
    CGContextSaveGState(ctx);
    CGContextClipToRect(ctx, dirty);
    /* The view is flipped; images draw y-up, so flip back. */
    CGContextTranslateCTM(ctx, 0, c->h);
    CGContextScaleCTM(ctx, 1, -1);
    CGContextDrawImage(ctx, CGRectMake(0, 0, c->w, c->h), img);
    CGContextRestoreGState(ctx);
    CGImageRelease(img);
}

static BOOL view_yes(id self, SEL cmd) { (void)self; (void)cmd; return YES; }

static void view_set_frame_size(id self, SEL cmd, CGSize size)
{
    struct objc_super super = { self, (Class)cls("NSView") };
    ((void (*)(struct objc_super*, SEL, CGSize))objc_msgSendSuper)(&super, cmd, size);
    struct cocoa_window* win = backend_of(self);
    if (!win || !win->started)
        return;
    ensure_back_buffer(win, (int)size.width, (int)size.height);
    if (win->surface)
        gui_surface_resize(win->app, win->surface, win->canvas.w, win->canvas.h);
    else
        gui_app_resize(win->app, win->canvas.w, win->canvas.h);
    refresh(win);
}

/* Moving to a screen with another backing scale: a new buffer for it. */
static void view_backing_changed(id self, SEL cmd)
{
    (void)cmd;
    struct cocoa_window* win = backend_of(self);
    if (!win || !win->started)
        return;
    ensure_back_buffer(win, win->canvas.w, win->canvas.h);
    if (!win->surface)
        gui_app_font_changed(win->app, &win->canvas);
    refresh(win);
}

/* --- The delegate: app, window and timer --- */

static void delegate_timer_fired(id self, SEL cmd, id timer)
{
    (void)cmd; (void)timer;
    struct gui_event ev = { 0 };
    ev.type = GUI_EVENT_TIMER;
    post(backend_of(self), &ev);
}

/* The close button asks the app, which may quit (see refresh). */
static BOOL delegate_window_should_close(id self, SEL cmd, id sender)
{
    (void)cmd; (void)sender;
    struct gui_event ev = { 0 };
    ev.type = GUI_EVENT_CLOSE;
    post(backend_of(self), &ev);
    return NO;
}

static BOOL delegate_terminate_after_last_window(id self, SEL cmd, id app)
{
    (void)self; (void)cmd; (void)app;
    return YES;
}

static void delegate_will_terminate(id self, SEL cmd, id note)
{
    (void)cmd; (void)note;
    struct cocoa_window* win = backend_of(self);
    if (win && win->app)
    {
        gui_app_free(win->app);
        win->app = NULL;
        win->started = 0;
    }
}

static Class register_view_class(void)
{
    Class c = objc_allocateClassPair((Class)cls("NSView"), "IdeGuiView", 0);
    class_addIvar(c, "backend", sizeof(void*), sizeof(void*) == 8 ? 3 : 2, "^v");
    class_addMethod(c, sel("drawRect:"), (IMP)view_draw_rect, "v@:{CGRect={CGPoint=dd}{CGSize=dd}}");
    class_addMethod(c, sel("isFlipped"), (IMP)view_yes, "c@:");
    class_addMethod(c, sel("isOpaque"), (IMP)view_yes, "c@:");
    class_addMethod(c, sel("acceptsFirstResponder"), (IMP)view_yes, "c@:");
    class_addMethod(c, sel("acceptsFirstMouse:"), (IMP)view_yes, "c@:@");
    class_addMethod(c, sel("mouseDown:"), (IMP)view_mouse_down, "v@:@");
    class_addMethod(c, sel("mouseUp:"), (IMP)view_mouse_up, "v@:@");
    class_addMethod(c, sel("rightMouseDown:"), (IMP)view_right_down, "v@:@");
    class_addMethod(c, sel("rightMouseUp:"), (IMP)view_right_up, "v@:@");
    class_addMethod(c, sel("mouseMoved:"), (IMP)view_mouse_moved, "v@:@");
    class_addMethod(c, sel("mouseDragged:"), (IMP)view_mouse_moved, "v@:@");
    class_addMethod(c, sel("rightMouseDragged:"), (IMP)view_mouse_moved, "v@:@");
    class_addMethod(c, sel("mouseExited:"), (IMP)view_mouse_exited, "v@:@");
    class_addMethod(c, sel("scrollWheel:"), (IMP)view_scroll_wheel, "v@:@");
    class_addMethod(c, sel("keyDown:"), (IMP)view_key_down, "v@:@");
    class_addMethod(c, sel("setFrameSize:"), (IMP)view_set_frame_size, "v@:{CGSize=dd}");
    class_addMethod(c, sel("viewDidChangeBackingProperties"), (IMP)view_backing_changed, "v@:");

    class_addMethod(c, sel("insertText:replacementRange:"), (IMP)view_insert_text, "v@:@{_NSRange=QQ}");
    class_addMethod(c, sel("setMarkedText:selectedRange:replacementRange:"), (IMP)view_set_marked_text,
                    "v@:@{_NSRange=QQ}{_NSRange=QQ}");
    class_addMethod(c, sel("unmarkText"), (IMP)view_unmark_text, "v@:");
    class_addMethod(c, sel("hasMarkedText"), (IMP)view_has_marked_text, "c@:");
    class_addMethod(c, sel("markedRange"), (IMP)view_no_range, "{_NSRange=QQ}@:");
    class_addMethod(c, sel("selectedRange"), (IMP)view_no_range, "{_NSRange=QQ}@:");
    class_addMethod(c, sel("validAttributesForMarkedText"), (IMP)view_valid_attributes, "@@:");
    class_addMethod(c, sel("attributedSubstringForProposedRange:actualRange:"),
                    (IMP)view_attributed_substring, "@@:{_NSRange=QQ}^{_NSRange=QQ}");
    class_addMethod(c, sel("characterIndexForPoint:"), (IMP)view_character_index, "Q@:{CGPoint=dd}");
    class_addMethod(c, sel("firstRectForCharacterRange:actualRange:"), (IMP)view_first_rect,
                    "{CGRect={CGPoint=dd}{CGSize=dd}}@:{_NSRange=QQ}^{_NSRange=QQ}");
    class_addMethod(c, sel("doCommandBySelector:"), (IMP)view_do_command, "v@::");
    Protocol* text_input = objc_getProtocol("NSTextInputClient");
    if (text_input)
        class_addProtocol(c, text_input);
    objc_registerClassPair(c);
    return c;
}

static Class register_delegate_class(void)
{
    Class c = objc_allocateClassPair((Class)cls("NSObject"), "IdeGuiDelegate", 0);
    class_addIvar(c, "backend", sizeof(void*), sizeof(void*) == 8 ? 3 : 2, "^v");
    class_addMethod(c, sel("timerFired:"), (IMP)delegate_timer_fired, "v@:@");
    class_addMethod(c, sel("windowShouldClose:"), (IMP)delegate_window_should_close, "c@:@");
    class_addMethod(c, sel("applicationShouldTerminateAfterLastWindowClosed:"),
                    (IMP)delegate_terminate_after_last_window, "c@:@");
    class_addMethod(c, sel("applicationWillTerminate:"), (IMP)delegate_will_terminate, "v@:@");
    objc_registerClassPair(c);
    return c;
}

/* The window's content view, flipped, tracking the mouse - the first
 * responder. */
static id make_view(struct cocoa_window* win, CGRect frame)
{
    id view = MSG(id, id, SEL)((id)objc_getClass("IdeGuiView"), sel("alloc"));
    view = MSG(id, id, SEL, CGRect)(view, sel("initWithFrame:"), frame);
    set_backend(view, win);
    MSG(void, id, SEL, unsigned long)(view, sel("setAutoresizingMask:"), 2UL | 16UL);
    MSG(void, id, SEL, id)(win->window, sel("setContentView:"), view);
    MSG(BOOL, id, SEL, id)(win->window, sel("makeFirstResponder:"), view);

    /* Entered/exited (for MOUSE_LEAVE), always, following the view's size. */
    id area = MSG(id, id, SEL)(cls("NSTrackingArea"), sel("alloc"));
    area = MSG(id, id, SEL, CGRect, unsigned long, id, id)(
        area, sel("initWithRect:options:owner:userInfo:"), frame, 0x01UL | 0x80UL | 0x200UL, view, NULL);
    MSG(void, id, SEL, id)(view, sel("addTrackingArea:"), area);
    return view;
}

int main(int argc, char** argv)
{
    struct cocoa_window win = { 0 };   /* lives while -run does: it never returns */
    win.owner = &win;
    struct gui_canvas* c = &win.canvas;
    c->colorspace = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
    c->pt = DEFAULT_FONT_PT;
    if (!apply_font(c))
        return 1;

    id app = MSG(id, id, SEL)(cls("NSApplication"), sel("sharedApplication"));
    MSG(void, id, SEL, long)(app, sel("setActivationPolicy:"), 0L);   /* Regular */
    {
        /* the Dock's icon: Cake's, from the bytes built in - a plain
         * executable has no bundle to take it from */
        id data = MSG(id, id, SEL, const void*, unsigned long)(cls("NSData"), sel("dataWithBytes:length:"),
                                                               cake_icon_png, (unsigned long)sizeof cake_icon_png);
        id image = MSG(id, id, SEL)(cls("NSImage"), sel("alloc"));
        image = data ? MSG(id, id, SEL, id)(image, sel("initWithData:"), data) : NULL;
        if (image)
        {
            /* Apple's icon grid: the artwork is 824 of 1024 - a margin
             * around it, or it looks bigger than every other icon */
            CGSize size = { 256, 256 };
            double margin = 256.0 * 100 / 1024;
            id icon = MSG(id, id, SEL)(cls("NSImage"), sel("alloc"));
            icon = MSG(id, id, SEL, CGSize)(icon, sel("initWithSize:"), size);
            MSG(void, id, SEL)(icon, sel("lockFocus"));
            CGRect to = { { margin, margin }, { 256 - 2 * margin, 256 - 2 * margin } };
            CGRect from = { { 0, 0 }, { 0, 0 } };   /* the whole image */
            MSG(void, id, SEL, CGRect, CGRect, unsigned long, CGFloat)(image, sel("drawInRect:fromRect:operation:fraction:"),
                                                                      to, from, 2UL /* SourceOver */, 1.0);
            MSG(void, id, SEL)(icon, sel("unlockFocus"));
            MSG(void, id, SEL, id)(app, sel("setApplicationIconImage:"), icon);
        }
    }

    register_view_class();
    Class delegate_class = register_delegate_class();
    id delegate = MSG(id, id, SEL)((id)delegate_class, sel("new"));
    set_backend(delegate, &win);
    MSG(void, id, SEL, id)(app, sel("setDelegate:"), delegate);

    CGRect frame = CGRectMake(0, 0, DEFAULT_COLS * c->metrics.cell_w, DEFAULT_ROWS * c->metrics.cell_h);
    unsigned long style = 1 | 2 | 4 | 8;   /* titled, closable, miniaturizable, resizable */
    win.window = MSG(id, id, SEL)(cls("NSWindow"), sel("alloc"));
    win.window = MSG(id, id, SEL, CGRect, unsigned long, unsigned long, BOOL)(
        win.window, sel("initWithContentRect:styleMask:backing:defer:"), frame, style, 2UL, NO);
    CFStringRef title = cfstr("");
    MSG(void, id, SEL, id)(win.window, sel("setTitle:"), (id)title);
    CFRelease(title);
    MSG(void, id, SEL, id)(win.window, sel("setDelegate:"), delegate);
    MSG(void, id, SEL, BOOL)(win.window, sel("setAcceptsMouseMovedEvents:"), YES);

    win.delegate = delegate;
    win.view = make_view(&win, frame);

    ensure_back_buffer(&win, (int)frame.size.width, (int)frame.size.height);
    win.app = gui_app_create();
    offer_fonts(&win);
    gui_app_start(win.app, c, argc, argv);
    win.started = 1;
    gui_app_resize(win.app, c->w, c->h);
    refresh(&win);

    MSG(void, id, SEL)(win.window, sel("center"));
    MSG(void, id, SEL, id)(win.window, sel("makeKeyAndOrderFront:"), NULL);
    MSG(void, id, SEL, BOOL)(app, sel("activateIgnoringOtherApps:"), YES);
    MSG(void, id, SEL)(app, sel("run"));
    return 0;
}
