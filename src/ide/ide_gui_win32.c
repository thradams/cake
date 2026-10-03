/* ide_gui_win32.c - Win32/GDI backend of the ide GUI framework.
 *
 * Only translates OS events into calls to the core and implements the
 * drawing primitives of ide_gui_backend.h. Everything about a window
 * lives in one struct win32_window, reached from the HWND through
 * GWLP_USERDATA - no globals. The main window owns the detached ones
 * (gui_window_detach): one more OS window each, sharing its fonts.
 */
#include "ide_gui_backend.h"

#include <windows.h>
#include <shellapi.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "msimg32.lib")   /* AlphaBlend - shadows */
#endif

/* Same defaults as the old IDE: 11pt in the first installed of these. */
#define DEFAULT_FONT_PT 11
#define DEFAULT_COLS 120
#define DEFAULT_ROWS 30

/* Not in older SDKs; harmless where unsupported (before Windows 11 22H2). */
#ifndef DWMWA_BORDER_COLOR
#define DWMWA_BORDER_COLOR 34
#endif
#ifndef DWMWA_CAPTION_COLOR
#define DWMWA_CAPTION_COLOR 35
#endif

static const wchar_t* const font_candidates[] = {
    L"Cascadia Mono", L"Cascadia Code", L"Consolas",
    L"DejaVu Sans Mono", L"Lucida Console", L"Courier New",
};

static const wchar_t* const ui_font_candidates[] = {
    L"Segoe UI", L"Tahoma", L"Verdana", L"Arial",
};

struct gui_canvas
{
    HDC mem;                     /* the back buffer */
    HBITMAP bmp;
    int w, h;
    HFONT main;                  /* GUI_FONT_MAIN */
    HFONT small_font;            /* GUI_FONT_SMALL - not `small`, a macro of <rpcndr.h> */
    HFONT ui_font;               /* GUI_FONT_UI */
    int pt;                      /* the main font's size in points */
    const wchar_t* family;       /* NULL: the first installed candidate */
    wchar_t ui_family[64];       /* "": GUI_FONT_UI is the editor family */
    int editor_size;            /* the editor font: -1 smaller, 0 the base size, 1 larger */
    struct gui_metrics metrics;  /* measured from `main` */
    struct gui_metrics small_metrics;
    struct gui_metrics ui_metrics;
    int ui_ascii_w[128];         /* GUI_FONT_UI's advances of ASCII, px */
    int dpi;                     /* of the window's monitor */
    HDC shade_src;               /* 1x1 black, stretched by AlphaBlend */
    HBITMAP shade_bmp;
};

/* DwmSetWindowAttribute, looked up at run time so the program still runs
 * where dwmapi is missing. */
struct titlebar
{
    HRESULT(WINAPI* set_attribute)(HWND, DWORD, LPCVOID, DWORD);
    uint32_t color;              /* the color last applied */
    int applied;
};

/* The installed font candidates offered to the app, by index into
 * font_candidates. */
struct offered_fonts
{
    int candidate[16];
    int count;
};

/* WM_APP + 1 to a detached window whose surface is gone: destroy it, now
 * that no handler of its own is running. */
#define WM_DETACHED_GONE (WM_APP + 1)

struct win32_window;

/* The main window's detached windows. */
struct detached_list
{
    struct win32_window** items;
    int count, cap;
};

struct win32_window
{
    HWND hwnd;
    struct gui_app* app;
    struct gui_canvas canvas;
    struct titlebar titlebar;
    int argc;
    char** argv;
    int started;                 /* gui_app_start has run */
    int tracking_leave;          /* TrackMouseEvent is armed */
    struct offered_fonts fonts;
    struct win32_window* owner;  /* the main window; itself for the main window */
    struct gui_surface* surface; /* a detached window's; NULL for the main window */
    int gone;                    /* a detached window whose surface is gone */
    struct detached_list detached;  /* the main window's */
};

static COLORREF to_colorref(uint32_t rgb)
{
    return RGB((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);
}

/* --- Fonts --- */

static int CALLBACK font_exists_proc(const LOGFONTW* lf, const TEXTMETRICW* tm,
                                     DWORD type, LPARAM lparam)
{
    (void)lf; (void)tm; (void)type;
    *(int*)lparam = 1;
    return 0;   /* one hit is enough */
}

static int font_exists(HDC dc, const wchar_t* name)
{
    LOGFONTW lf = { 0 };
    lf.lfCharSet = DEFAULT_CHARSET;
    lstrcpynW(lf.lfFaceName, name, LF_FACESIZE);
    int found = 0;
    EnumFontFamiliesExW(dc, &lf, font_exists_proc, (LPARAM)&found, 0);
    return found;
}

static const wchar_t* pick_font(HDC dc)
{
    for (int i = 0; i < (int)(sizeof font_candidates / sizeof font_candidates[0]); i++)
    {
        if (font_exists(dc, font_candidates[i]))
            return font_candidates[i];
    }
    return L"Courier New";
}

/* The small font, as a percentage of the main one - the old IDE's
 * FONT_SMALL_PERCENT. */
#define SMALL_FONT_PERCENT 85
/* The editor font's "Smaller" and "Larger": this much of the dialogs' size. */
#define SMALLER_FONT_PERCENT 90
#define LARGER_FONT_PERCENT 115

/* A font of the chosen family at `pt` for the monitor's DPI, measured. The
 * advance is measured on "M" rather than taken from tmAveCharWidth, which
 * rounds. */
static HFONT make_font(HDC dc, int pt, const wchar_t* family, struct gui_metrics* m)
{
    int dpi = GetDeviceCaps(dc, LOGPIXELSY);
    HFONT font = CreateFontW(-MulDiv(pt, dpi, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, family ? family : pick_font(dc));
    HFONT old = SelectObject(dc, font);
    TEXTMETRICW tm;
    GetTextMetricsW(dc, &tm);
    SIZE size;
    GetTextExtentPoint32W(dc, L"M", 1, &size);
    SelectObject(dc, old);
    m->cell_w = size.cx > 0 ? size.cx : 1;
    m->cell_h = tm.tmHeight > 0 ? tm.tmHeight : 1;
    m->ascent = tm.tmAscent;
    return font;
}

/* GUI_FONT_UI: c->ui_family at `pt`, proportional, else the main family;
 * cell_w is half its row height, and ASCII advances go to ui_ascii_w. */
static HFONT make_ui_font(HDC dc, int pt, struct gui_canvas* c, struct gui_metrics* m)
{
    HFONT font = NULL;
    if (!c->ui_family[0])
    {
        font = make_font(dc, pt, c->family, m);
    }
    else
    {
        int dpi = GetDeviceCaps(dc, LOGPIXELSY);
        const wchar_t* face = c->ui_family;
        NONCLIENTMETRICSW ncm = { sizeof ncm };
        if (wcscmp(c->ui_family, L"System") == 0 &&
            SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof ncm, &ncm, 0))
        {
            face = ncm.lfMessageFont.lfFaceName;   /* the OS's own (Segoe UI), not the old bitmap "System" */
        }
        font = CreateFontW(-MulDiv(pt, dpi, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                           DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                           CLEARTYPE_QUALITY, VARIABLE_PITCH | FF_SWISS, face);
    }
    HFONT old = SelectObject(dc, font);
    TEXTMETRICW tm;
    GetTextMetricsW(dc, &tm);
    SIZE size;
    GetTextExtentPoint32W(dc, L"M", 1, &size);
    for (int i = 0; i < 128; i++)
    {
        WCHAR ch = (WCHAR)i;
        SIZE ch_size = { 0, 0 };
        GetTextExtentPoint32W(dc, &ch, 1, &ch_size);
        c->ui_ascii_w[i] = ch_size.cx;
    }
    /* the height of "H" above the baseline: otmsCapEmHeight is 0 in many fonts */
    int cap = 0;
    GLYPHMETRICS gm;
    MAT2 identity = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 1 } };
    if (GetGlyphOutlineW(dc, L'H', GGO_METRICS, &gm, 0, NULL, &identity) != GDI_ERROR)
    {
        cap = gm.gmptGlyphOrigin.y;
    }
    SelectObject(dc, old);
    if (c->ui_family[0])
    {
        m->cell_w = size.cx > 0 ? size.cx : 1;
        m->cell_h = tm.tmHeight > 0 ? tm.tmHeight : 1;
        m->ascent = tm.tmAscent;
        /* rows at least 1.3 em, as tall as the monospaced fonts', the capitals centered in them */
        int min_h = MulDiv(pt * 13, GetDeviceCaps(dc, LOGPIXELSY), 720);
        if (m->cell_h < min_h)
        {
            m->cell_h = min_h;
        }
        if (cap > 0)
        {
            m->ascent = (m->cell_h + cap + 1) / 2;
        }
        m->cell_w = (m->cell_h + 1) / 2;   /* the monospaced fonts' proportion: half as wide as tall */
    }
    return font;
}

/* The installed font candidates, offered to the app (gui_font_count); the
 * first one is in use. */
static void offer_fonts(struct win32_window* win)
{
    HDC dc = GetDC(win->hwnd);
    const char* names[sizeof font_candidates / sizeof font_candidates[0]];
    char utf8[sizeof font_candidates / sizeof font_candidates[0]][64];
    win->fonts.count = 0;
    for (int i = 0; i < (int)(sizeof font_candidates / sizeof font_candidates[0]); i++)
    {
        if (!font_exists(dc, font_candidates[i]))
            continue;
        WideCharToMultiByte(CP_UTF8, 0, font_candidates[i], -1, utf8[win->fonts.count], 64, NULL, NULL);
        names[win->fonts.count] = utf8[win->fonts.count];
        win->fonts.candidate[win->fonts.count++] = i;
    }
    const char* ui_names[1 + sizeof ui_font_candidates / sizeof ui_font_candidates[0] + sizeof font_candidates / sizeof font_candidates[0]];
    char ui_utf8[sizeof ui_font_candidates / sizeof ui_font_candidates[0]][64];
    int ui_count = 0;
    ui_names[ui_count++] = "System";   /* the OS's own interface font, always first */
    for (int i = 0; i < (int)(sizeof ui_font_candidates / sizeof ui_font_candidates[0]); i++)
    {
        if (!font_exists(dc, ui_font_candidates[i]))
        {
            continue;
        }
        WideCharToMultiByte(CP_UTF8, 0, ui_font_candidates[i], -1, ui_utf8[i], 64, NULL, NULL);
        ui_names[ui_count++] = ui_utf8[i];
    }
    ReleaseDC(win->hwnd, dc);
    gui_app_set_fonts(win->app, names, win->fonts.count, 0);
    for (int i = 0; i < win->fonts.count; i++)
    {
        ui_names[ui_count++] = names[i];   /* the monospaced ones too */
    }
    gui_app_set_ui_fonts(win->app, ui_names, ui_count, 0);
}

/* (Re)creates both fonts from canvas->pt: they always exist together, so
 * drawing never has to create one. */
static void apply_font(struct win32_window* win)
{
    struct gui_canvas* c = &win->canvas;
    HDC dc = GetDC(win->hwnd);
    c->dpi = GetDeviceCaps(dc, LOGPIXELSY);
    /* the dialogs' font is the base size; the editor's a little smaller, the same or a little larger */
    int percent = c->editor_size < 0 ? SMALLER_FONT_PERCENT : c->editor_size > 0 ? LARGER_FONT_PERCENT : 100;
    int main_pt = (c->pt * percent + 50) / 100;
    int small_pt = (main_pt * SMALL_FONT_PERCENT + 50) / 100;
    HFONT main = make_font(dc, main_pt, c->family, &c->metrics);
    HFONT small_font = make_font(dc, small_pt > 0 ? small_pt : 1, c->family, &c->small_metrics);
    HFONT ui_font = make_ui_font(dc, c->pt, c, &c->ui_metrics);
    ReleaseDC(win->hwnd, dc);

    if (c->mem)
        SelectObject(c->mem, main);
    if (c->main)
        DeleteObject(c->main);
    if (c->small_font)
        DeleteObject(c->small_font);
    if (c->ui_font)
        DeleteObject(c->ui_font);
    c->main = main;
    c->small_font = small_font;
    c->ui_font = ui_font;
}

/* --- Drawing primitives (ide_gui_backend.h) --- */

struct gui_metrics gui_font_metrics(struct gui_canvas* c, enum gui_font font)
{
    return font == GUI_FONT_UI ? c->ui_metrics : font == GUI_FONT_SMALL ? c->small_metrics : c->metrics;
}

/* The advance of `cp`, px: a cell, or in GUI_FONT_UI the glyph's own. */
static int advance_of(struct gui_canvas* c, uint32_t cp, enum gui_font font)
{
    int advance = gui_font_metrics(c, font).cell_w;
    if (font == GUI_FONT_UI && cp < 128)
    {
        advance = c->ui_ascii_w[cp];
    }
    else if (font == GUI_FONT_UI)
    {
        WCHAR units[2];
        int len = 1;
        if (cp > 0xFFFF)
        {
            units[0] = (WCHAR)(0xD800 + ((cp - 0x10000) >> 10));
            units[1] = (WCHAR)(0xDC00 + ((cp - 0x10000) & 0x3FF));
            len = 2;
        }
        else
        {
            units[0] = (WCHAR)cp;
        }
        HGDIOBJ old = SelectObject(c->mem, c->ui_font);
        SIZE size = { 0, 0 };
        GetTextExtentPoint32W(c->mem, units, len, &size);
        SelectObject(c->mem, old);
        advance = size.cx;
    }
    return advance;
}

int gui_text_width(struct gui_canvas* c, const uint32_t* cps, int count, enum gui_font font)
{
    int width = 0;
    for (int i = 0; i < count; i++)
    {
        width += advance_of(c, cps[i], font);
    }
    return width;
}

/* 10 px at 96 DPI and the default font size: its own size, but it grows
 * and shrinks with the DPI and the zoom like everything else. */
int gui_scrollbar_size(struct gui_canvas* c)
{
    int size = MulDiv(10 * c->pt, c->dpi > 0 ? c->dpi : 96, 96 * DEFAULT_FONT_PT);
    return size > 2 ? size : 2;
}

void gui_set_clip(struct gui_canvas* c, int x, int y, int w, int h)
{
    SelectClipRgn(c->mem, NULL);
    if (w > 0 && h > 0)
        IntersectClipRect(c->mem, x, y, x + w, y + h);
}

void gui_fill_rect(struct gui_canvas* c, int x, int y, int w, int h, uint32_t rgb)
{
    if (w <= 0 || h <= 0)
        return;
    RECT r = { x, y, x + w, y + h };
    SetBkColor(c->mem, to_colorref(rgb));
    ExtTextOutW(c->mem, 0, 0, ETO_OPAQUE, &r, NULL, 0, NULL);  /* fills with the bk color */
}

void gui_shade_rect(struct gui_canvas* c, int x, int y, int w, int h, int alpha)
{
    if (w <= 0 || h <= 0)
        return;
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, (BYTE)alpha, 0 };
    AlphaBlend(c->mem, x, y, w, h, c->shade_src, 0, 0, 1, 1, blend);
}

void gui_draw_text(struct gui_canvas* c, int x, int y, const uint32_t* cps, int count,
                   uint32_t fg, uint32_t bg, enum gui_font font)
{
    /* UTF-16 with one lpDx entry per unit: each code point advances one
     * cell (its own width in GUI_FONT_UI), the second unit of a surrogate
     * pair advances 0. */
    WCHAR text[512];
    INT dx[512];
    struct gui_metrics m = gui_font_metrics(c, font);
    SelectObject(c->mem, font == GUI_FONT_UI ? c->ui_font : font == GUI_FONT_SMALL ? c->small_font : c->main);
    while (count > 0)
    {
        int n = 0, used = 0, run_w = 0;
        while (used < count && n + 2 <= (int)(sizeof text / sizeof text[0]))
        {
            uint32_t cp = cps[used++];
            int advance = advance_of(c, cp, font);
            run_w += advance;
            if (cp > 0xFFFF)
            {
                cp -= 0x10000;
                text[n] = (WCHAR)(0xD800 + (cp >> 10));
                dx[n++] = advance;
                text[n] = (WCHAR)(0xDC00 + (cp & 0x3FF));
                dx[n++] = 0;
            }
            else
            {
                text[n] = (WCHAR)cp;
                dx[n++] = advance;
            }
        }
        RECT r = { x, y, x + run_w, y + m.cell_h };
        SetBkColor(c->mem, to_colorref(bg));
        SetTextColor(c->mem, to_colorref(fg));
        /* at the baseline the metrics give: GUI_FONT_UI's centers its capitals in the row */
        SetTextAlign(c->mem, TA_BASELINE | TA_LEFT);
        ExtTextOutW(c->mem, x, y + m.ascent, ETO_OPAQUE | ETO_CLIPPED, &r, text, (UINT)n, dx);
        x += run_w;
        cps += used;
        count -= used;
    }
}

/* --- Back buffer --- */

static void ensure_back_buffer(struct win32_window* win)
{
    struct gui_canvas* c = &win->canvas;
    RECT rc;
    GetClientRect(win->hwnd, &rc);
    int w = rc.right > 0 ? rc.right : 1;
    int h = rc.bottom > 0 ? rc.bottom : 1;
    if (c->mem && w == c->w && h == c->h)
        return;
    HDC dc = GetDC(win->hwnd);
    if (c->mem)
    {
        DeleteObject(c->bmp);
        DeleteDC(c->mem);
    }
    c->mem = CreateCompatibleDC(dc);
    c->bmp = CreateCompatibleBitmap(dc, w, h);
    if (!c->shade_src)
    {
        c->shade_src = CreateCompatibleDC(dc);
        c->shade_bmp = CreateCompatibleBitmap(dc, 1, 1);
        SelectObject(c->shade_src, c->shade_bmp);
        SetPixel(c->shade_src, 0, 0, RGB(0, 0, 0));
    }
    ReleaseDC(win->hwnd, dc);
    SelectObject(c->mem, c->bmp);
    SelectObject(c->mem, c->main);
    c->w = w;
    c->h = h;
    if (win->surface)
        gui_surface_invalidate(win->app, win->surface);
    else if (win->app)
        gui_app_invalidate(win->app);   /* a new buffer: everything drawn again */
}

/* --- Detached windows --- */

int gui_backend_can_detach(void)
{
    return 1;
}

/* A detached window draws with the main window's fonts. */
static void share_fonts(struct win32_window* d, const struct win32_window* main)
{
    struct gui_canvas* c = &d->canvas;
    c->main = main->canvas.main;
    c->small_font = main->canvas.small_font;
    c->pt = main->canvas.pt;
    c->family = main->canvas.family;
    c->metrics = main->canvas.metrics;
    c->small_metrics = main->canvas.small_metrics;
    c->ui_font = main->canvas.ui_font;
    memcpy(c->ui_family, main->canvas.ui_family, sizeof c->ui_family);
    c->editor_size = main->canvas.editor_size;
    c->ui_metrics = main->canvas.ui_metrics;
    memcpy(c->ui_ascii_w, main->canvas.ui_ascii_w, sizeof c->ui_ascii_w);
    c->dpi = main->canvas.dpi;
    if (c->mem)
        SelectObject(c->mem, c->main);
}

static void plain_caption(HWND hwnd);

/* Like the main window: no caption text, no icon. */
static void detached_open(struct win32_window* main, struct gui_surface* s, int w, int h)
{
    struct win32_window* d = calloc(1, sizeof *d);
    if (!d)
        abort();
    d->app = main->app;
    d->owner = main;
    d->surface = s;
    d->titlebar.set_attribute = main->titlebar.set_attribute;
    share_fonts(d, main);

    RECT r = { 0, 0, w > 0 ? w : 640, h > 0 ? h : 480 };
    AdjustWindowRectEx(&r, WS_OVERLAPPEDWINDOW, FALSE, 0);
    HWND hwnd = CreateWindowW(L"ide_gui_window", L"", WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top,
                              NULL, NULL, GetModuleHandleW(NULL), d);
    if (!hwnd)
    {
        free(d);
        struct gui_event ev = { 0 };
        ev.type = GUI_EVENT_CLOSE;   /* no OS window: the window goes back */
        gui_surface_event(main->app, s, &ev);
        return;
    }
    struct detached_list* list = &main->detached;
    if (list->count == list->cap)
    {
        int cap = list->cap ? list->cap * 2 : 4;
        struct win32_window** items = realloc(list->items, (size_t)cap * sizeof *items);
        if (!items)
            abort();
        list->items = items;
        list->cap = cap;
    }
    list->items[list->count++] = d;
    plain_caption(hwnd);
    ensure_back_buffer(d);
    d->started = 1;
    gui_surface_start(d->app, s, d, d->canvas.w, d->canvas.h);
    ShowWindow(hwnd, SW_SHOW);
}

/* Its surface is gone: hidden now, destroyed from its own queue
 * (WM_DETACHED_GONE), since a handler of it may be running. */
static void detached_close(struct win32_window* main, struct win32_window* d)
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
    d->gone = 1;
    d->surface = NULL;
    ShowWindow(d->hwnd, SW_HIDE);
    PostMessageW(d->hwnd, WM_DETACHED_GONE, 0, 0);
}

/* The back buffer and the window go; the fonts are the main window's. */
static void detached_destroy(struct win32_window* d)
{
    SetWindowLongPtrW(d->hwnd, GWLP_USERDATA, 0);
    DestroyWindow(d->hwnd);
    if (d->canvas.mem)
    {
        DeleteDC(d->canvas.mem);
        DeleteObject(d->canvas.bmp);
    }
    if (d->canvas.shade_src)
    {
        DeleteDC(d->canvas.shade_src);
        DeleteObject(d->canvas.shade_bmp);
    }
    free(d);
}

/* The OS windows the core asked for, made, raised and closed. */
static void sync_detached(struct win32_window* main)
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
        struct win32_window* d = native;
        if (IsIconic(d->hwnd))
            ShowWindow(d->hwnd, SW_RESTORE);
        SetForegroundWindow(d->hwnd);
    }
}

/* Tints the OS title bar and border with the app's chrome color, and only
 * when it changed. */
static void sync_titlebar(struct win32_window* win)
{
    struct titlebar* tb = &win->titlebar;
    uint32_t color = gui_app_titlebar_color(win->app);
    if (!tb->set_attribute || (tb->applied && tb->color == color))
        return;
    tb->color = color;
    tb->applied = 1;
    COLORREF cr = to_colorref(color);
    tb->set_attribute(win->hwnd, DWMWA_CAPTION_COLOR, &cr, sizeof cr);
    tb->set_attribute(win->hwnd, DWMWA_BORDER_COLOR, &cr, sizeof cr);
}

/* Lets the core paint whatever is dirty and invalidates just that rect, so
 * WM_PAINT blits it - in the main window and in each detached one. Nothing
 * dirty -> nothing happens. */
static void refresh(struct win32_window* win)
{
    win = win->owner;
    if (!win->started)
        return;
    if (gui_app_should_quit(win->app))
    {
        DestroyWindow(win->hwnd);
        return;
    }
    int ms;
    if (gui_app_take_timer(win->app, &ms))
    {
        if (ms > 0)
            SetTimer(win->hwnd, 1, (UINT)ms, NULL);
        else
            KillTimer(win->hwnd, 1);
    }
    int family = gui_app_take_font(win->app);
    if (family >= 0 && family < win->fonts.count)
    {
        win->canvas.family = font_candidates[win->fonts.candidate[family]];
        apply_font(win);
        gui_app_font_changed(win->app, &win->canvas);
    }
    int ui_index, editor_size;
    if (gui_app_take_ui_font(win->app, &ui_index, &editor_size))
    {
        win->canvas.ui_family[0] = 0;
        win->canvas.editor_size = editor_size;
        if (ui_index >= 0)
            MultiByteToWideChar(CP_UTF8, 0, gui_ui_font_name(win->app, ui_index), -1, win->canvas.ui_family, 64);
        apply_font(win);
        gui_app_font_changed(win->app, &win->canvas);
    }
    int zoom = gui_app_take_zoom(win->app);
    if (zoom)
    {
        int pt = win->canvas.pt + zoom;
        win->canvas.pt = pt < 6 ? 6 : pt > 40 ? 40 : pt;
        apply_font(win);
        gui_app_font_changed(win->app, &win->canvas);
    }
    sync_detached(win);
    struct gui_rect r;
    if (gui_app_paint(win->app, &win->canvas, &r))
    {
        RECT rc = { r.x, r.y, r.x + r.w, r.y + r.h };
        InvalidateRect(win->hwnd, &rc, FALSE);
    }
    sync_titlebar(win);
    for (int i = 0; i < win->detached.count; i++)
    {
        struct win32_window* d = win->detached.items[i];
        share_fonts(d, win);
        if (gui_surface_paint(d->app, d->surface, &d->canvas, &r))
        {
            RECT rc = { r.x, r.y, r.x + r.w, r.y + r.h };
            InvalidateRect(d->hwnd, &rc, FALSE);
        }
        sync_titlebar(d);
    }
}

/* --- Clipboard (ide_gui_backend.h) --- */

void gui_clipboard_set(struct gui_canvas* c, const char* utf8)
{
    (void)c;
    /* '\n' becomes "\r\n", what other Windows programs expect. */
    int lines = 0;
    for (const char* p = utf8; *p; p++)
        lines += *p == '\n';
    size_t len = strlen(utf8) + (size_t)lines;
    char* crlf = malloc(len + 1);
    if (!crlf)
        return;
    char* o = crlf;
    for (const char* p = utf8; *p; p++)
    {
        if (*p == '\n')
            *o++ = '\r';
        *o++ = *p;
    }
    *o = '\0';
    int wlen = MultiByteToWideChar(CP_UTF8, 0, crlf, -1, NULL, 0);
    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, (size_t)wlen * sizeof(WCHAR));
    if (mem)
    {
        MultiByteToWideChar(CP_UTF8, 0, crlf, -1, (WCHAR*)GlobalLock(mem), wlen);
        GlobalUnlock(mem);
        if (OpenClipboard(NULL))
        {
            EmptyClipboard();
            if (!SetClipboardData(CF_UNICODETEXT, mem))
                GlobalFree(mem);
            CloseClipboard();
        }
        else
        {
            GlobalFree(mem);
        }
    }
    free(crlf);
}

char* gui_clipboard_get(struct gui_canvas* c)
{
    (void)c;
    if (!OpenClipboard(NULL))
        return NULL;
    char* result = NULL;
    HANDLE h = GetClipboardData(CF_UNICODETEXT);
    const WCHAR* w = h ? GlobalLock(h) : NULL;
    if (w)
    {
        int len = WideCharToMultiByte(CP_UTF8, 0, w, -1, NULL, 0, NULL, NULL);
        result = len > 0 ? malloc((size_t)len) : NULL;
        if (result)
        {
            WideCharToMultiByte(CP_UTF8, 0, w, -1, result, len, NULL, NULL);
            /* "\r\n" back to '\n' */
            char* o = result;
            for (const char* p = result; *p; p++)
            {
                if (*p != '\r')
                    *o++ = *p;
            }
            *o = '\0';
        }
        GlobalUnlock(h);
    }
    CloseClipboard();
    return result;
}

/* --- Input --- */

static int current_mods(void)
{
    int mods = 0;
    if (GetKeyState(VK_SHIFT) < 0) mods |= GUI_MOD_SHIFT;
    if (GetKeyState(VK_CONTROL) < 0) mods |= GUI_MOD_CTRL;
    if (GetKeyState(VK_MENU) < 0) mods |= GUI_MOD_ALT;
    return mods;
}

/* A virtual key as a gui key code, or 0 for keys the framework ignores. */
static int map_vk(WPARAM vk)
{
    if (vk >= 'A' && vk <= 'Z') return (int)vk;
    if (vk >= '0' && vk <= '9') return (int)vk;
    if (vk >= VK_F1 && vk <= VK_F12) return GUI_KEY_F1 + (int)(vk - VK_F1);
    switch (vk)
    {
    case VK_ESCAPE: return GUI_KEY_ESCAPE;
    case VK_RETURN: return GUI_KEY_ENTER;
    case VK_TAB: return GUI_KEY_TAB;
    case VK_BACK: return GUI_KEY_BACKSPACE;
    case VK_DELETE: return GUI_KEY_DELETE;
    case VK_INSERT: return GUI_KEY_INSERT;
    case VK_HOME: return GUI_KEY_HOME;
    case VK_END: return GUI_KEY_END;
    case VK_PRIOR: return GUI_KEY_PAGEUP;
    case VK_NEXT: return GUI_KEY_PAGEDOWN;
    case VK_LEFT: return GUI_KEY_LEFT;
    case VK_RIGHT: return GUI_KEY_RIGHT;
    case VK_UP: return GUI_KEY_UP;
    case VK_DOWN: return GUI_KEY_DOWN;
    case VK_SPACE: return ' ';
    case VK_OEM_PLUS: case VK_ADD: return '+';
    case VK_OEM_MINUS: case VK_SUBTRACT: return '-';
    }
    return 0;
}

static void post(struct win32_window* win, const struct gui_event* ev)
{
    struct win32_window* owner = win->owner;
    if (win->surface)
        gui_surface_event(win->app, win->surface, ev);
    else
        gui_app_event(win->app, ev);
    refresh(owner);   /* `win` may be gone now */
}

static void post_mouse(struct win32_window* win, enum gui_event_type type, LPARAM lp, int button,
                       int double_click)
{
    struct gui_event ev = { 0 };
    ev.type = type;
    ev.double_click = double_click;
    ev.x = (int)(short)LOWORD(lp);
    ev.y = (int)(short)HIWORD(lp);
    ev.button = button;
    ev.mods = current_mods();
    post(win, &ev);
}

/* --- Window --- */

static LRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    struct win32_window* win = (struct win32_window*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    if (msg == WM_NCCREATE)
    {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lp;
        win = cs->lpCreateParams;
        win->hwnd = hwnd;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)win);
    }
    if (win && win->gone)
    {
        if (msg == WM_DETACHED_GONE)
        {
            detached_destroy(win);
            return 0;
        }
        return DefWindowProcW(hwnd, msg, wp, lp);
    }
    if (!win || !win->started)
        return DefWindowProcW(hwnd, msg, wp, lp);

    switch (msg)
    {
    case WM_SIZE:
        ensure_back_buffer(win);
        if (win->surface)
            gui_surface_resize(win->app, win->surface, win->canvas.w, win->canvas.h);
        else
            gui_app_resize(win->app, win->canvas.w, win->canvas.h);
        refresh(win);
        return 0;
    case WM_ERASEBKGND:
        return 1;   /* the back buffer covers everything */
    case WM_SETCURSOR:
        if (LOWORD(lp) == HTCLIENT)
        {
            static const LPCWSTR shapes[] = {
                IDC_ARROW, IDC_SIZEWE, IDC_SIZENS, IDC_SIZENWSE, IDC_SIZENESW,
            };
            enum gui_cursor cursor = win->surface ? gui_surface_cursor(win->app, win->surface)
                                                  : gui_app_cursor(win->app);
            SetCursor(LoadCursorW(NULL, shapes[cursor]));
            return TRUE;
        }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hwnd, &ps);
        RECT r = ps.rcPaint;
        BitBlt(dc, r.left, r.top, r.right - r.left, r.bottom - r.top,
               win->canvas.mem, r.left, r.top, SRCCOPY);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_MOUSEMOVE:
        if (!win->tracking_leave)
        {
            TRACKMOUSEEVENT tme = { sizeof tme, TME_LEAVE, hwnd, 0 };
            win->tracking_leave = TrackMouseEvent(&tme);
        }
        post_mouse(win, GUI_EVENT_MOUSE_MOVE, lp, 0, 0);
        SendMessageW(hwnd, WM_SETCURSOR, (WPARAM)hwnd, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
        return 0;
    case WM_MOUSELEAVE:
    {
        win->tracking_leave = 0;
        struct gui_event ev = { 0 };
        ev.type = GUI_EVENT_MOUSE_LEAVE;
        post(win, &ev);
        return 0;
    }
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
        SetCapture(hwnd);
        post_mouse(win, GUI_EVENT_MOUSE_DOWN, lp, 0, msg == WM_LBUTTONDBLCLK);
        return 0;
    case WM_LBUTTONUP:
        ReleaseCapture();
        post_mouse(win, GUI_EVENT_MOUSE_UP, lp, 0, 0);
        return 0;
    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL:
    {
        /* Screen coordinates here, unlike the other mouse messages. */
        POINT pt = { (short)LOWORD(lp), (short)HIWORD(lp) };
        ScreenToClient(hwnd, &pt);
        struct gui_event ev = { 0 };
        ev.type = GUI_EVENT_WHEEL;
        ev.x = pt.x;
        ev.y = pt.y;
        if (msg == WM_MOUSEHWHEEL)
            ev.hwheel = GET_WHEEL_DELTA_WPARAM(wp);
        else
            ev.wheel = GET_WHEEL_DELTA_WPARAM(wp);
        ev.mods = current_mods();
        post(win, &ev);
        return 0;
    }
    case WM_RBUTTONDOWN:
        post_mouse(win, GUI_EVENT_MOUSE_DOWN, lp, 1, 0);
        return 0;
    case WM_RBUTTONUP:
        post_mouse(win, GUI_EVENT_MOUSE_UP, lp, 1, 0);
        return 0;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    {
        if (msg == WM_SYSKEYDOWN && wp == VK_F4)
            break;   /* Alt+F4 still closes the window */
        int key = map_vk(wp);
        if (key)
        {
            struct gui_event ev = { 0 };
            ev.type = GUI_EVENT_KEY;
            ev.key = key;
            ev.mods = current_mods();
            post(win, &ev);
        }
        return 0;   /* Alt combinations never open the OS window menu */
    }
    case WM_CLOSE:
    {
        struct gui_event ev = { 0 };
        ev.type = GUI_EVENT_CLOSE;
        post(win, &ev);   /* refresh() destroys the window if the app quits */
        return 0;
    }
    case WM_TIMER:
    {
        struct gui_event ev = { 0 };
        ev.type = GUI_EVENT_TIMER;
        post(win, &ev);
        return 0;
    }
    case WM_SYSCHAR:
        return 0;   /* no beep for Alt+letter */
    case WM_CHAR:
        if (wp == ' ' && (GetKeyState(VK_CONTROL) & 0x8000) && !(GetKeyState(VK_MENU) & 0x8000))
            return 0;   /* Ctrl+Space is a shortcut, not a space */
        if (wp >= 32 && wp != 127)
        {
            struct gui_event ev = { 0 };
            ev.type = GUI_EVENT_CHAR;
            ev.ch = (uint32_t)wp;
            post(win, &ev);
        }
        return 0;
    case WM_DPICHANGED:
    {
        /* Moving to a monitor with another DPI: rebuild the font for it and
         * take the window rect Windows suggests. */
        RECT* suggested = (RECT*)lp;
        if (!win->surface)
        {
            apply_font(win);
            gui_app_font_changed(win->app, &win->canvas);
        }
        SetWindowPos(hwnd, NULL, suggested->left, suggested->top,
                     suggested->right - suggested->left, suggested->bottom - suggested->top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        refresh(win);
        return 0;
    }
    case WM_DESTROY:
        if (win->surface)
            return 0;
        while (win->detached.count > 0)
            detached_destroy(win->detached.items[--win->detached.count]);
        free(win->detached.items);
        win->detached.items = NULL;
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static void enable_dpi_awareness(void)
{
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    BOOL(WINAPI* set_context)(HANDLE) =
        (BOOL(WINAPI*)(HANDLE))(void*)GetProcAddress(user32, "SetProcessDpiAwarenessContext");
    if (set_context && set_context((HANDLE)-4))  /* PER_MONITOR_AWARE_V2 */
        return;
    SetProcessDPIAware();
}

/* argv as UTF-8, from the real command line. */
static void utf8_args(struct win32_window* win)
{
    int wargc = 0;
    LPWSTR* wargv = CommandLineToArgvW(GetCommandLineW(), &wargc);
    if (!wargv)
        return;
    win->argv = calloc((size_t)wargc + 1, sizeof(char*));
    if (win->argv)
    {
        for (int i = 0; i < wargc; i++)
        {
            int len = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, NULL, 0, NULL, NULL);
            win->argv[i] = len > 0 ? malloc((size_t)len) : NULL;
            if (win->argv[i])
                WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, win->argv[i], len, NULL, NULL);
        }
        win->argc = wargc;
    }
    LocalFree(wargv);
}

/* The app draws its own chrome (menubar, statusbar), so the OS caption has
 * no text and no icon - as in the old IDE. WS_EX_DLGMODALFRAME is what
 * stops Windows from keeping an empty icon slot. */
static void plain_caption(HWND hwnd)
{
    SetWindowTextW(hwnd, L"");
    LONG_PTR ex_style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex_style | WS_EX_DLGMODALFRAME);
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

int WINAPI wWinMain(HINSTANCE inst, HINSTANCE prev, PWSTR cmd, int show)
{
    (void)prev; (void)cmd;
    enable_dpi_awareness();

    struct win32_window win = { 0 };
    win.owner = &win;
    win.canvas.pt = DEFAULT_FONT_PT;
    win.canvas.editor_size = 0;
    win.app = gui_app_create();
    utf8_args(&win);

    HMODULE dwmapi = LoadLibraryW(L"dwmapi.dll");
    if (dwmapi)
        win.titlebar.set_attribute = (HRESULT(WINAPI*)(HWND, DWORD, LPCVOID, DWORD))(void*)
            GetProcAddress(dwmapi, "DwmSetWindowAttribute");

    WNDCLASSW wc = { 0 };
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = wndproc;
    wc.hInstance = inst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"ide_gui_window";
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowW(wc.lpszClassName, L"", WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
                              NULL, NULL, inst, &win);
    if (!hwnd)
        return 1;
    plain_caption(hwnd);
    apply_font(&win);

    /* Client area of DEFAULT_COLS x DEFAULT_ROWS cells, like the old IDE. */
    RECT wr, cr;
    GetWindowRect(hwnd, &wr);
    GetClientRect(hwnd, &cr);
    int border_w = (wr.right - wr.left) - cr.right;
    int border_h = (wr.bottom - wr.top) - cr.bottom;
    SetWindowPos(hwnd, NULL, 0, 0,
                 DEFAULT_COLS * win.canvas.metrics.cell_w + border_w,
                 DEFAULT_ROWS * win.canvas.metrics.cell_h + border_h,
                 SWP_NOMOVE | SWP_NOZORDER);

    ensure_back_buffer(&win);
    offer_fonts(&win);
    gui_app_start(win.app, &win.canvas, win.argc, win.argv);
    win.started = 1;
    gui_app_resize(win.app, win.canvas.w, win.canvas.h);
    refresh(&win);
    ShowWindow(hwnd, show);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    gui_app_free(win.app);
    return 0;
}
