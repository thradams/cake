/* x11.c - X11/Xlib + Xft native graphical backend for Linux.
 *
 * Manages the window, translates X11 events into ui_env events, renders by
 * calling app_render(), which in turn calls ui_screen_render() -> the
 * ui_draw_char()/ui_draw_box() functions implemented below with
 * Xlib/Xft/XRender. Completely generic - calls into the app via an abstract
 * interface (app_init, app_frame, app_render). Does NOT know what the app
 * does, or anything about its DOM/ui_screen. Mirrors win32.c's structure
 * closely; see that file's comments for the reasoning behind most design
 * choices here (the shadow's half-cell pieces, the box-drawing codepoint
 * range, etc.) - only X11-specific differences are called out below.
 *
 * Build: gcc -std=c11 ui.c turboc_demo.c x11.c -o demo_tc \
 *            -lX11 -lXft -lXrender `pkg-config --cflags freetype2`
 */

/* strdup/usleep are POSIX, not standard C - glibc's headers hide them under
 * plain -std=c11 without this. Must come before any system header include. */
#define _DEFAULT_SOURCE

#include "ide_ui.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/Xft/Xft.h>
#include <X11/extensions/Xrender.h>
#include <X11/cursorfont.h>

#include <locale.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>


static Display *g_dpy;
static int g_screen;
static Window g_win;
static Visual *g_visual;
static Colormap g_cmap;
static GC g_gc;

/* Single persistent off-screen pixmap: cleared and fully redrawn (by walking
 * the app's DOM) every frame, then blitted once per iteration - same plain
 * double-buffering scheme as win32.c's g_mem bitmap. g_pixmap_picture is the
 * XRender handle onto the same pixmap, needed only for the shadow's alpha
 * blending (see ui_draw_box's UI_BOX_SHADOW branch) - everything else draws
 * with the plain GC or Xft, both of which work directly against a Drawable. */
static Pixmap g_pixmap;
static Picture g_pixmap_picture;
static XRenderPictFormat *g_pict_format;
static int g_pixmap_w, g_pixmap_h;

static XftDraw *g_xft_draw;
static XftFont *g_font;      /* antialiased - used for everything except box-drawing */
static XftFont *g_font_box;  /* non-antialiased - box-drawing glyphs only, see ui_draw_char */
static int g_font_pt = 11;  /* adjustable via Ctrl+/Ctrl- */
static int g_cell_w = 8;
static int g_cell_h = 16;

static ui_env *g_env;
static int g_app_initialized = 0;
static int g_running = 1;

/* Render throttling (see the main loop): app_frame() runs every iteration
 * (cheap), but the full-DOM redraw + blit (render_frame()) only runs when an
 * X event changed something, or on a slow baseline that still drives the caret
 * blink - so an idle window doesn't repaint ~1000x/sec and pin a CPU core. */
static int g_dirty = 1;                 /* need the very first frame */
static unsigned g_last_render_ms = 0;
#define RENDER_BASELINE_MS 250          /* < the 530ms caret-blink half-period */

static Atom g_wm_delete;
static Atom g_clipboard_atom, g_utf8_string_atom, g_targets_atom;

static XIM g_xim;
static XIC g_xic;

/* The text we currently own on the CLIPBOARD selection (see
 * ui_clipboard_set_text) - answered directly out of memory whenever another
 * client asks for it (SelectionRequest), the ICCCM way; there's no "the OS
 * clipboard" service to hand data to like Win32's, ownership + on-demand
 * answering IS the protocol. */
static char *g_clip_text;

/* Double-click detection: X11 has no native equivalent of Win32's
 * WM_LBUTTONDBLCLK, so it's synthesized here the same way any X toolkit
 * does - a second left press within the platform's double-click interval,
 * on the same cell, counts as one. Uses the X server's own event timestamps
 * (monotonic per session) rather than a local clock. */
#define DOUBLE_CLICK_MS 400
static Time g_last_click_time;
static int g_last_click_x = -1, g_last_click_y = -1;

/* --- Small local helpers (ui.c's UTF-8 codec is internal to it) --- */

static int utf8_encode_cp(uint32_t cp, unsigned char out[4])
{
    if (cp < 0x80) {
        out[0] = (unsigned char)cp;
        return 1;
    } else if (cp < 0x800) {
        out[0] = (unsigned char)(0xC0 | (cp >> 6));
        out[1] = (unsigned char)(0x80 | (cp & 0x3F));
        return 2;
    } else if (cp < 0x10000) {
        out[0] = (unsigned char)(0xE0 | (cp >> 12));
        out[1] = (unsigned char)(0x80 | ((cp >> 6) & 0x3F));
        out[2] = (unsigned char)(0x80 | (cp & 0x3F));
        return 3;
    }
    out[0] = (unsigned char)(0xF0 | (cp >> 18));
    out[1] = (unsigned char)(0x80 | ((cp >> 12) & 0x3F));
    out[2] = (unsigned char)(0x80 | ((cp >> 6) & 0x3F));
    out[3] = (unsigned char)(0x80 | (cp & 0x3F));
    return 4;
}

/* Decodes just the first code point of a short buffer - a single X11
 * KeyPress's XLookupString/Xutf8LookupString result is always exactly one
 * character's worth of bytes, never more. */
static uint32_t utf8_decode_first(const unsigned char *s, int len)
{
    if (len <= 0)
        return 0;
    unsigned char c = s[0];
    if (c < 0x80)
        return c;
    if ((c & 0xE0) == 0xC0 && len >= 2)
        return (uint32_t)((c & 0x1F) << 6) | (s[1] & 0x3F);
    if ((c & 0xF0) == 0xE0 && len >= 3)
        return (uint32_t)((c & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
    if ((c & 0xF8) == 0xF0 && len >= 4)
        return (uint32_t)((c & 0x07) << 18) | ((s[1] & 0x3F) << 12) |
               ((s[2] & 0x3F) << 6) | (s[3] & 0x3F);
    return c;
}

/* --- Color: our colors are 0x00RRGGBB. --- */

/* Fast path for the overwhelmingly common 24/32-bit TrueColor visual
 * (masks 0xFF0000/0x00FF00/0x0000FF) - falls back to XAllocColor, which
 * works (if slower) on any other visual class/depth too. */
static unsigned long rgb_to_pixel(uint32_t rgb)
{
    if (g_visual->red_mask == 0xFF0000 && g_visual->green_mask == 0x00FF00 &&
        g_visual->blue_mask == 0x0000FF)
        return rgb & 0xFFFFFF;

    XColor c = {0};
    c.red = (unsigned short)(((rgb >> 16) & 0xFF) * 257);
    c.green = (unsigned short)(((rgb >> 8) & 0xFF) * 257);
    c.blue = (unsigned short)((rgb & 0xFF) * 257);
    c.flags = DoRed | DoGreen | DoBlue;
    XAllocColor(g_dpy, g_cmap, &c);
    return c.pixel;
}

static void xft_color_from_rgb(uint32_t rgb, XftColor *out)
{
    XRenderColor rc;
    rc.red = (unsigned short)(((rgb >> 16) & 0xFF) * 257);
    rc.green = (unsigned short)(((rgb >> 8) & 0xFF) * 257);
    rc.blue = (unsigned short)((rgb & 0xFF) * 257);
    rc.alpha = 0xFFFF;
    XftColorAllocValue(g_dpy, g_visual, g_cmap, &rc, out);
}

/* --- ui_draw_char/ui_draw_box (see ui.h) --- */

/* Glyphs rendered with the non-antialiased font (see apply_font), same fix
 * as win32.c's ClearType/NONANTIALIASED_QUALITY split and for the same root
 * cause: antialiased subpixel filtering spills a fringe past the glyph's cell
 * that doesn't land consistently between independently-drawn cells - seams in
 * border lines, and stale fringe pixels left when a neighbor cell repaints
 * (e.g. beside the moving scrollbar arrows). All UI chrome symbols render
 * crisp and self-contained:
 *   - U+2500-259F  Box Drawing + Block Elements (borders, dithered desktop)
 *   - U+2190-21FF  Arrows (scrollbar up/down, zoom icon, <select> down-arrow)
 *   - U+25A0-25FF  Geometric Shapes (close square, submenu triangle)
 * Actual text keeps antialiasing, which looks better for everything else. */
static int is_box_drawing(uint32_t ch)
{
    return (ch >= 0x2500 && ch <= 0x259F) ||
           (ch >= 0x2190 && ch <= 0x21FF) ||
           (ch >= 0x25A0 && ch <= 0x25FF);
}

void ui_draw_char(int x, int y, uint32_t ch, uint32_t fg, uint32_t bg)
{
    int px = x * g_cell_w;
    int py = y * g_cell_h;

    XSetForeground(g_dpy, g_gc, rgb_to_pixel(bg));
    XFillRectangle(g_dpy, g_pixmap, g_gc, px, py, (unsigned)g_cell_w, (unsigned)g_cell_h);

    uint32_t cp = ch ? ch : ' ';
    unsigned char utf8[4];
    int len = utf8_encode_cp(cp, utf8);

    XftFont *font = (is_box_drawing(cp) && g_font_box) ? g_font_box : g_font;
    XftColor color;
    xft_color_from_rgb(fg, &color);
    /* Clip the glyph to its own cell: an overhang (antialiased fringe, or a
     * glyph wider than the monospace cell) would otherwise spill into the
     * neighbour, and since cells are drawn independently and only changed ones
     * repaint, that spill is never cleaned when the neighbour doesn't itself
     * repaint - e.g. editor text bleeding into the static blank scrollbar
     * column, leaving stale fringe along the track as the view scrolls. */
    XRectangle clip = { (short)px, (short)py, (unsigned short)g_cell_w, (unsigned short)g_cell_h };
    XftDrawSetClipRectangles(g_xft_draw, 0, 0, &clip, 1);
    XftDrawStringUtf8(g_xft_draw, &color, font, px, py + g_font->ascent, utf8, len);
    XftDrawSetClip(g_xft_draw, None);
    XftColorFree(g_dpy, g_visual, g_cmap, &color);
}

/* ui_draw_box (see ui.h): a solid w x h rect - ui.c's replacement for
 * building the same fill out of a per-cell ui_draw_char loop. Every current
 * caller (see draw_fill in ui.c) fills with a blank cell, so there's no
 * glyph to render at all - a normal fill (flags == 0) is just one
 * XFillRectangle over the whole rect, no font/text machinery involved;
 * `fg` is unused.
 *
 * UI_BOX_INVERT: true XOR cursor. X11's GXinvert GC function flips every
 * destination bit regardless of the source pixel, exactly like Win32's
 * PatBlt(DSTINVERT) - automatically legible over anything already painted.
 *
 * UI_BOX_SHADOW: darken the rect via true alpha blending, like win32.c's
 * AlphaBlend - here via XRender's PictOpOver onto the pixmap's Picture. A
 * solid black source needs no premultiplication (black times any alpha is
 * still black), so a plain {0,0,0,alpha} XRenderColor is exactly right
 * without extra math. */
void ui_draw_box(int x, int y, int w, int h, uint32_t fg, uint32_t bg, int flags)
{
    (void)fg;

    if (flags & UI_BOX_INVERT) {
        XSetFunction(g_dpy, g_gc, GXinvert);
        XFillRectangle(g_dpy, g_pixmap, g_gc, x * g_cell_w, y * g_cell_h,
                        (unsigned)(w * g_cell_w), (unsigned)(h * g_cell_h));
        XSetFunction(g_dpy, g_gc, GXcopy);
        return;
    }
    if (flags & UI_BOX_SHADOW) {
        if (!g_pixmap_picture)
            return;
        int px = x * g_cell_w, py = y * g_cell_h;
        int pw = w * g_cell_w, ph = h * g_cell_h;
        if (flags & UI_BOX_SHADOW_SHRINK_W)
            pw = g_cell_w / 2;
        if (flags & UI_BOX_SHADOW_SHRINK_H)
            ph = g_cell_h / 2;
        if (flags & UI_BOX_SHADOW_SHRINK_H_BOTTOM) {
            ph = g_cell_h / 2;
            py += g_cell_h - ph;
        }
        XRenderColor black = { 0, 0, 0, (unsigned short)(110 * 0xFFFF / 255) };  /* ~43% opacity */
        XRenderFillRectangle(g_dpy, PictOpOver, g_pixmap_picture, &black, px, py,
                              (unsigned)pw, (unsigned)ph);
        return;
    }

    XSetForeground(g_dpy, g_gc, rgb_to_pixel(bg));
    XFillRectangle(g_dpy, g_pixmap, g_gc, x * g_cell_w, y * g_cell_h,
                    (unsigned)(w * g_cell_w), (unsigned)(h * g_cell_h));
}

/* --- Font --- */

/* `extra` is appended verbatim to the fontconfig pattern - used to request
 * the non-antialiased box-drawing variant (see apply_font) without a whole
 * second candidate-list/name-building path. */
static XftFont *try_open_font(const char *family, int pt, const char *extra)
{
    char name[160];
    snprintf(name, sizeof name, "%s-%d%s", family, pt, extra);
    return XftFontOpenName(g_dpy, g_screen, name);
}

/* Preference order: DejaVu Sans Mono (near-universal on Linux, excellent
 * Unicode/box-drawing coverage) > Liberation Mono > Noto Sans Mono >
 * "Monospace" (fontconfig's generic alias - always resolves to *something*,
 * unlike GDI, fontconfig does substitution on its own so this practically
 * never returns NULL). */
static XftFont *pick_and_open_font(int pt, const char *extra)
{
    static const char *candidates[] = {
        "DejaVu Sans Mono", "Liberation Mono", "Noto Sans Mono", "Monospace",
    };
    for (size_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]); i++) {
        XftFont *f = try_open_font(candidates[i], pt, extra);
        if (f)
            return f;
    }
    return NULL;
}

/* (Re)load g_font/g_font_box at g_font_pt and derive g_cell_w/h from the
 * main font's metrics. Called at startup and again from Ctrl+/Ctrl- to
 * change size. Two variants at the same size/family: antialiased for
 * everything, and a non-antialiased twin used only for box-drawing glyphs
 * (ui_draw_char picks between them) - see is_box_drawing() for why. */
static void apply_font(void)
{
    XftFont *new_font = pick_and_open_font(g_font_pt, "");
    if (!new_font)
        return;  /* keep whatever font (if any) we already had */
    XftFont *new_font_box = pick_and_open_font(g_font_pt, ":antialias=false:hinting=false");

    XGlyphInfo extents;
    XftTextExtentsUtf8(g_dpy, new_font, (const FcChar8 *)"M", 1, &extents);
    g_cell_w = extents.xOff > 0 ? extents.xOff : (int)new_font->max_advance_width;
    g_cell_h = new_font->ascent + new_font->descent;

    if (g_font)
        XftFontClose(g_dpy, g_font);
    if (g_font_box)
        XftFontClose(g_dpy, g_font_box);
    g_font = new_font;
    g_font_box = new_font_box;

    /* Every cell's pixel size just changed, so the render shadow no longer
     * matches the pixmap - repaint fully next frame. (Usually followed by a
     * resize -> ensure_pixmap, but this covers the case the pixel size is
     * unchanged.) menu_bg (rather than a hardcoded color) so any sliver the
     * cell grid doesn't exactly cover blends with the menu bar instead of
     * standing out - see ensure_pixmap's own fill below for why that sliver
     * exists at all. */
    if (g_pixmap) {
        XSetForeground(g_dpy, g_gc, rgb_to_pixel(ui_get_theme()->menu_bg));
        XFillRectangle(g_dpy, g_pixmap, g_gc, 0, 0, (unsigned)g_pixmap_w, (unsigned)g_pixmap_h);
    }
    app_invalidate();
}

/* Ask the window manager to snap interactive/live resizes to whole cells -
 * the X11-native equivalent of ide_win32.c's WM_SIZING live snap and
 * ide_cocoa.c's windowWillResize:toSize: (see those files' own comments for
 * why this matters: without it, a manual resize almost never lands on an
 * exact multiple of the cell size, and the leftover sliver - painted
 * menu_bg, see ensure_pixmap - shows for the whole drag). Unlike Win32/
 * Cocoa, X11 has no frame-vs-content geometry gap to account for -
 * XSizeHints describes the client window's own size, and any decoration is
 * drawn by the window manager entirely outside that geometry - so this is
 * just the cell size as the resize increment, with a one-cell minimum.
 * Must be re-applied whenever g_cell_w/h change (see adjust_font_size), or
 * a stale increment from the old font size keeps the WM snapping to the
 * wrong grid. Not every window manager honors PResizeInc (a few ignore it
 * outright), so the menu_bg fill in ensure_pixmap remains the fallback for
 * those. */
static void set_wm_resize_hints(void)
{
    XSizeHints hints = {0};
    hints.flags = PResizeInc | PBaseSize | PMinSize;
    hints.width_inc = g_cell_w;
    hints.height_inc = g_cell_h;
    hints.base_width = 0;
    hints.base_height = 0;
    hints.min_width = g_cell_w;
    hints.min_height = g_cell_h;
    XSetWMNormalHints(g_dpy, g_win, &hints);
}

/* Change the font size by `delta` points, clamped to a sane range, and
 * resize the window to keep the same column/row count at the new cell
 * size. Registered with the ui_env as its font-zoom callback (see
 * ui_env_set_font_zoom_fn in ui.h) - invoked uniformly via
 * ui_env_adjust_font_size(), whether the trigger is Ctrl+/Ctrl- or the app's
 * Window > Font Size +/- menu items. `ctx` is unused - this backend keeps
 * everything it needs in globals (g_dpy/g_win/g_env). */
static void adjust_font_size(void *ctx, int delta)
{
    (void)ctx;
    int new_pt = g_font_pt + delta;
    if (new_pt < 6) new_pt = 6;
    if (new_pt > 36) new_pt = 36;
    if (new_pt == g_font_pt)
        return;

    g_font_pt = new_pt;
    int cols = ui_env_width(g_env);
    int rows = ui_env_height(g_env);
    apply_font();
    set_wm_resize_hints();  /* new g_cell_w/h - keep the WM's snap grid current */
    XResizeWindow(g_dpy, g_win, (unsigned)(cols * g_cell_w), (unsigned)(rows * g_cell_h));
    /* The resulting ConfigureNotify drives ui_env_resize/ensure_pixmap with
     * whatever size the window manager actually granted. */
}

/* --- Pixmap (back buffer) --- */

static void ensure_pixmap(int w, int h)
{
    if (w < 1) w = 1;
    if (h < 1) h = 1;
    if (g_pixmap && w == g_pixmap_w && h == g_pixmap_h)
        return;

    if (g_pixmap_picture) { XRenderFreePicture(g_dpy, g_pixmap_picture); g_pixmap_picture = 0; }
    if (g_xft_draw) { XftDrawDestroy(g_xft_draw); g_xft_draw = NULL; }
    if (g_pixmap) { XFreePixmap(g_dpy, g_pixmap); g_pixmap = 0; }

    unsigned depth = (unsigned)DefaultDepth(g_dpy, g_screen);
    g_pixmap = XCreatePixmap(g_dpy, g_win, (unsigned)w, (unsigned)h, depth);
    g_xft_draw = XftDrawCreate(g_dpy, g_pixmap, g_visual, g_cmap);
    if (g_pict_format)
        g_pixmap_picture = XRenderCreatePicture(g_dpy, g_pixmap, g_pict_format, 0, NULL);

    g_pixmap_w = w;
    g_pixmap_h = h;

    /* Fresh pixmap: give it a known background once (never per frame, so cells
     * the incremental renderer skips keep their pixels), and discard the
     * render shadow so the next frame repaints everything into it. menu_bg
     * (rather than a hardcoded color) so a maximize/resize that doesn't land
     * on an exact multiple of the cell size leaves its leftover sliver
     * blending with the menu bar instead of a mismatched gap. */
    XSetForeground(g_dpy, g_gc, rgb_to_pixel(ui_get_theme()->menu_bg));
    XFillRectangle(g_dpy, g_pixmap, g_gc, 0, 0, (unsigned)w, (unsigned)h);
    app_invalidate();
}

static void handle_resize(int width, int height)
{
    int cols = width / g_cell_w;
    int rows = height / g_cell_h;
    if (cols < 1) cols = 1;
    if (rows < 1) rows = 1;
    ui_env_resize(g_env, cols, rows);
    ensure_pixmap(width, height);
}

/* Have the app redraw whatever changed into the pixmap, then blit only that
 * region (the pixmap is a complete image, so a partial copy is always valid).
 * Nothing changed -> no blit. The pixmap is never cleared here - only on
 * (re)creation (see ensure_pixmap) - so skipped cells keep last frame. The
 * region is cell-tight - glyphs are clipped to their cells (see
 * ui_draw_char's XftDrawSetClipRectangles), so nothing spills past a cell
 * that would need an extra margin here. */
static void render_frame(void)
{
    int dx, dy, dw, dh;
    if (!app_render(&dx, &dy, &dw, &dh))
        return;

    int px = dx * g_cell_w, py = dy * g_cell_h;
    int pw = dw * g_cell_w, ph = dh * g_cell_h;
    XCopyArea(g_dpy, g_pixmap, g_win, g_gc, px, py, (unsigned)pw, (unsigned)ph, px, py);
    XFlush(g_dpy);
}

/* Explicitly give the window a normal arrow pointer - matches win32.c's
 * wc.hCursor = LoadCursor(NULL, IDC_ARROW): the app draws its own block
 * cursor for the text caret (see ui_draw_box's UI_BOX_INVERT branch), but
 * the OS pointer tracking the mouse is still the standard arrow, same as
 * every other native app. Set explicitly (rather than relying on whatever
 * default the window manager/X server hands out) so it's consistent across
 * WMs. */
static void set_normal_cursor(void)
{
    Cursor cursor = XCreateFontCursor(g_dpy, XC_left_ptr);
    XDefineCursor(g_dpy, g_win, cursor);
    XFreeCursor(g_dpy, cursor);
    /* Push it to the server right away rather than leaving it sitting in
     * Xlib's output buffer until whatever the next unrelated flush happens
     * to be - cheap insurance against this being applied later than the
     * window actually appears. */
    XFlush(g_dpy);
}

/* --- Input Method setup (proper UTF-8/dead-key/IME text input) --- */

static void setup_xim(void)
{
    XSetLocaleModifiers("");
    g_xim = XOpenIM(g_dpy, NULL, NULL, NULL);
    if (!g_xim)
        return;  /* Xutf8LookupString falls back to plain XLookupString below */
    g_xic = XCreateIC(g_xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
                       XNClientWindow, g_win, XNFocusWindow, g_win, (void *)NULL);
}

/* --- Key mapping --- */

static int keysym_to_uikey(KeySym sym)
{
    switch (sym) {
    case XK_Up:     return UI_KEY_UP;
    case XK_Down:   return UI_KEY_DOWN;
    case XK_Left:   return UI_KEY_LEFT;
    case XK_Right:  return UI_KEY_RIGHT;
    case XK_Home:   return UI_KEY_HOME;
    case XK_End:    return UI_KEY_END;
    case XK_Prior:  return UI_KEY_PAGEUP;
    case XK_Next:   return UI_KEY_PAGEDOWN;
    case XK_Delete: return UI_KEY_DELETE;
    case XK_Insert: return UI_KEY_INSERT;
    case XK_Escape: return UI_KEY_ESCAPE;
    case XK_Tab:    return UI_KEY_TAB;
    case XK_F1: case XK_F2: case XK_F3: case XK_F4:
    case XK_F5: case XK_F6: case XK_F7: case XK_F8:
    case XK_F9: case XK_F10: case XK_F11: case XK_F12:
        return UI_KEY_F1 + (int)(sym - XK_F1);
    default:
        return 0;
    }
}

static int mods_from_state(unsigned int state)
{
    return (state & ShiftMask ? UI_MOD_SHIFT : 0) |
           (state & ControlMask ? UI_MOD_CTRL : 0) |
           (state & Mod1Mask ? UI_MOD_ALT : 0);
}

/* --- Clipboard: the two hooks ui.c calls for <input>/<editor> copy/cut/
 * paste (see ui_screen_copy/cut/paste and Ctrl+C/X/V in ui_screen_update).
 * Unlike Win32's system clipboard service, X11's is ICCCM selection
 * ownership: "setting" the clipboard just means owning the CLIPBOARD
 * selection and answering SelectionRequest events (handled in the main
 * loop) with whatever we're holding; "getting" it means asking the current
 * owner and waiting for its reply. --- */

void ui_clipboard_set_text(const char *utf8)
{
    free(g_clip_text);
    g_clip_text = utf8 ? strdup(utf8) : NULL;
    XSetSelectionOwner(g_dpy, g_clipboard_atom, g_win, CurrentTime);
}

char *ui_clipboard_get_text(void)
{
    if (!g_dpy)
        return NULL;

    Atom prop = XInternAtom(g_dpy, "TUI_CLIP_PASTE", False);
    XConvertSelection(g_dpy, g_clipboard_atom, g_utf8_string_atom, prop, g_win, CurrentTime);
    XFlush(g_dpy);

    /* Every other X11 interaction in this backend is async (posted into
     * ui_env's queue, drained next frame) - but ui_clipboard_get_text() has
     * to return the actual text right here, so this one spot blocks
     * (briefly - a same-machine selection round-trip is normally
     * sub-millisecond) waiting for the SelectionNotify reply. Any unrelated
     * event that arrives during the short wait is simply dropped rather
     * than queued, a rare and harmless cost given how short this window
     * usually is. */
    for (int tries = 0; tries < 200; tries++) {
        while (XPending(g_dpy)) {
            XEvent ev;
            XNextEvent(g_dpy, &ev);
            if (ev.type == SelectionNotify && ev.xselection.requestor == g_win) {
                if (ev.xselection.property == None)
                    return NULL;  /* owner has no text, or declined */

                Atom actual_type;
                int actual_format;
                unsigned long nitems, bytes_after;
                unsigned char *data = NULL;
                XGetWindowProperty(g_dpy, g_win, prop, 0, 1L << 20, False,
                                    AnyPropertyType, &actual_type, &actual_format,
                                    &nitems, &bytes_after, &data);
                char *out = NULL;
                if (data) {
                    out = malloc(nitems + 1);
                    if (out) {
                        memcpy(out, data, nitems);
                        out[nitems] = 0;
                    }
                    XFree(data);
                }
                XDeleteProperty(g_dpy, g_win, prop);
                return out;
            }
        }
        usleep(1000);
    }
    return NULL;  /* no owner, or it never responded */
}

/* --- Filesystem: the directory-listing hook ui.c calls for a file-open
 * dialog (see ui_list_dir in ui.h). Plain POSIX opendir/readdir+stat - no
 * UTF-8 boundary to cross here, unlike Win32's wide-char FindFirstFileW,
 * since Linux filenames are already just bytes (typically UTF-8 already). --- */

int ui_list_dir(const char *path, ui_dir_entry *out, int max_entries)
{
    DIR *dir = opendir(path);
    if (!dir)
        return -1;

    char full[4096];
    int count = 0;
    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;
        if (count >= max_entries)
            break;

        snprintf(full, sizeof full, "%s/%s", path, ent->d_name);
        struct stat st;
        if (stat(full, &st) != 0)
            continue;  /* broken symlink, permission denied, etc. - skip it */

        ui_dir_entry *e = &out[count];
        strncpy(e->name, ent->d_name, sizeof(e->name) - 1);
        e->name[sizeof(e->name) - 1] = 0;
        e->is_dir = S_ISDIR(st.st_mode);
        e->size = e->is_dir ? 0 : (long long)st.st_size;
        e->mtime = st.st_mtime;
        count++;
    }

    closedir(dir);
    return count;
}

int ui_get_cwd(char *buf, int buf_size)
{
    return getcwd(buf, (size_t)buf_size) != NULL;
}

/* Tools > Terminal (see ide.c's do_open_terminal): launches a terminal
 * emulator with its working directory set to `dir`, then leaves it running
 * on its own - not a child this app manages afterward. There's no single
 * "the" terminal on Linux the way there's one %COMSPEC% on Windows or
 * Terminal.app on macOS, so this tries a handful of common ones in turn;
 * the first whose exec() succeeds wins.
 *
 * Double-forked (grandchild does the exec, middle child exits immediately)
 * so the terminal ends up reparented to init instead of staying this
 * process's child - the one waitpid() below reaps only that short-lived
 * middle process, and the terminal itself never becomes a zombie for this
 * app to worry about. chdir() in the grandchild (rather than a shell "cd
 * dir &&" wrapper) sidesteps any shell-quoting concern over what `dir`
 * contains. */
void ui_open_terminal(const char *dir)
{
    pid_t pid = fork();
    if (pid < 0)
        return;
    if (pid == 0)
    {
        pid_t pid2 = fork();
        if (pid2 == 0)
        {
            if (dir && dir[0])
                (void)chdir(dir);
            execlp("x-terminal-emulator", "x-terminal-emulator", (char *)NULL);
            execlp("gnome-terminal", "gnome-terminal", (char *)NULL);
            execlp("konsole", "konsole", (char *)NULL);
            execlp("xfce4-terminal", "xfce4-terminal", (char *)NULL);
            execlp("xterm", "xterm", (char *)NULL);
            _exit(127);  /* nothing above was found on PATH */
        }
        _exit(0);
    }
    waitpid(pid, NULL, 0);
}

/* Answers another client's request for our clipboard content - the other
 * half of ui_clipboard_set_text() above. */
static void handle_selection_request(const XSelectionRequestEvent *req)
{
    /* A full XEvent union, not a bare XSelectionEvent - XSendEvent expects
     * (and its wire-protocol marshaling reads) a whole XEvent's worth of
     * memory regardless of which member is populated, so a smaller,
     * same-address-cast struct would be an out-of-bounds read. */
    XEvent resp_ev = {0};
    XSelectionEvent *resp = &resp_ev.xselection;
    resp->type = SelectionNotify;
    resp->requestor = req->requestor;
    resp->selection = req->selection;
    resp->target = req->target;
    resp->time = req->time;
    resp->property = None;

    if (req->target == g_utf8_string_atom || req->target == XA_STRING) {
        if (g_clip_text) {
            XChangeProperty(g_dpy, req->requestor, req->property, req->target, 8,
                             PropModeReplace, (const unsigned char *)g_clip_text,
                             (int)strlen(g_clip_text));
            resp->property = req->property;
        }
    } else if (req->target == g_targets_atom) {
        Atom targets[2] = { g_utf8_string_atom, XA_STRING };
        XChangeProperty(g_dpy, req->requestor, req->property, XA_ATOM, 32,
                         PropModeReplace, (const unsigned char *)targets, 2);
        resp->property = req->property;
    }
    XSendEvent(g_dpy, req->requestor, False, 0, &resp_ev);
}

/* --- Event handling --- */

static void handle_key_press(XKeyEvent *xkey)
{
    int mods = mods_from_state(xkey->state);
    KeySym sym = XLookupKeysym(xkey, 0);

    /* Ctrl+/Ctrl- resize the font - a window-management concern the backend
     * handles itself, never surfaced to the app as a key event. */
    if ((mods & UI_MOD_CTRL) && (sym == XK_plus || sym == XK_equal || sym == XK_KP_Add)) {
        ui_env_adjust_font_size(g_env, 1);
        return;
    }
    if ((mods & UI_MOD_CTRL) && (sym == XK_minus || sym == XK_KP_Subtract)) {
        ui_env_adjust_font_size(g_env, -1);
        return;
    }

    int code = keysym_to_uikey(sym);
    if (code) {
        ui_event ev = {0};
        ev.type = UI_EVENT_KEY;
        ev.data.key.code = code;
        ev.data.key.mods = mods;
        ui_env_post_event(g_env, &ev);
        return;
    }

    /* Printable text, via the input method for proper UTF-8/dead-key/IME
     * composition if one was available (see setup_xim), else plain
     * XLookupString (Latin-1 only, but keyboard layouts/locales without an
     * IM configured are rare on modern Linux). */
    unsigned char buf[32];
    int n;
    KeySym ignored;
    if (g_xic) {
        Status status = 0;
        n = Xutf8LookupString(g_xic, xkey, (char *)buf, (int)sizeof(buf) - 1, &ignored, &status);
        if (status == XBufferOverflow)
            n = 0;
    } else {
        n = XLookupString(xkey, (char *)buf, (int)sizeof(buf) - 1, &ignored, NULL);
    }
    if (n <= 0)
        return;
    buf[n] = 0;

    uint32_t cp = utf8_decode_first(buf, n);
    if (cp == 0)
        return;
    if (cp == 127)
        cp = 8;  /* some keyboard mappings send DEL for Backspace - normalize */

    ui_event ev = {0};
    ev.type = UI_EVENT_KEY;
    ev.data.key.codepoint = cp;
    ev.data.key.mods = mods;
    ui_env_post_event(g_env, &ev);
}

static void handle_button_press(XButtonEvent *xb)
{
    int x = xb->x / g_cell_w;
    int y = xb->y / g_cell_h;
    int mods = mods_from_state(xb->state);

    /* X11 reports the wheel as button 4 (up)/5 (down) presses (each
     * immediately followed by a synthetic release), not a distinct event
     * type - translate here rather than exposing that quirk to the app. */
    if (xb->button == 4 || xb->button == 5) {
        ui_event ev = {0};
        ev.type = UI_EVENT_MOUSE;
        ev.data.mouse.x = x;
        ev.data.mouse.y = y;
        ev.data.mouse.action = UI_MOUSE_WHEEL;
        ev.data.mouse.wheel_delta = (xb->button == 4) ? 1 : -1;
        ev.data.mouse.mods = mods;
        ui_env_post_event(g_env, &ev);
        return;
    }

    int button = (xb->button == 1 ? UI_MOUSE_BUTTON_LEFT :
                  xb->button == 3 ? UI_MOUSE_BUTTON_RIGHT :
                  UI_MOUSE_BUTTON_MIDDLE);

    ui_event ev = {0};
    ev.type = UI_EVENT_MOUSE;
    ev.data.mouse.x = x;
    ev.data.mouse.y = y;
    ev.data.mouse.mods = mods;
    ev.data.mouse.button = button;

    if (button == UI_MOUSE_BUTTON_LEFT && x == g_last_click_x && y == g_last_click_y &&
        xb->time - g_last_click_time < DOUBLE_CLICK_MS) {
        ev.data.mouse.action = UI_MOUSE_DBLCLICK;
        g_last_click_time = 0;  /* a third click starts fresh, not a triple */
    } else {
        ev.data.mouse.action = UI_MOUSE_PRESSED;
        g_last_click_time = xb->time;
        g_last_click_x = x;
        g_last_click_y = y;
    }
    ui_env_post_event(g_env, &ev);
}

static void handle_button_release(XButtonEvent *xb)
{
    if (xb->button == 4 || xb->button == 5)
        return;  /* the wheel's paired "release" is synthetic - nothing to do */

    ui_event ev = {0};
    ev.type = UI_EVENT_MOUSE;
    ev.data.mouse.x = xb->x / g_cell_w;
    ev.data.mouse.y = xb->y / g_cell_h;
    ev.data.mouse.action = UI_MOUSE_RELEASED;
    ev.data.mouse.button = (xb->button == 1 ? UI_MOUSE_BUTTON_LEFT :
                             xb->button == 3 ? UI_MOUSE_BUTTON_RIGHT :
                             UI_MOUSE_BUTTON_MIDDLE);
    ev.data.mouse.mods = mods_from_state(xb->state);
    ui_env_post_event(g_env, &ev);
}

static void handle_event(XEvent *event)
{
    switch (event->type) {
    case KeyPress:
        handle_key_press(&event->xkey);
        break;
    case ButtonPress:
        handle_button_press(&event->xbutton);
        break;
    case ButtonRelease:
        handle_button_release(&event->xbutton);
        break;
    case MotionNotify: {
        ui_event ev = {0};
        ev.type = UI_EVENT_MOUSE;
        ev.data.mouse.x = event->xmotion.x / g_cell_w;
        ev.data.mouse.y = event->xmotion.y / g_cell_h;
        ev.data.mouse.action = UI_MOUSE_MOVED;
        ev.data.mouse.mods = mods_from_state(event->xmotion.state);
        ui_env_post_event(g_env, &ev);
        break;
    }
    case Expose:
        /* Window uncovered: restore just the exposed region straight from the
         * pixmap (a complete image of the last frame). Independent of the
         * incremental render - the content didn't change, only its visibility,
         * so app_render would report nothing to redraw. */
        if (g_pixmap)
            XCopyArea(g_dpy, g_pixmap, g_win, g_gc,
                      event->xexpose.x, event->xexpose.y,
                      (unsigned)event->xexpose.width, (unsigned)event->xexpose.height,
                      event->xexpose.x, event->xexpose.y);
        break;
    case ConfigureNotify:
        if (event->xconfigure.width != g_pixmap_w || event->xconfigure.height != g_pixmap_h)
            handle_resize(event->xconfigure.width, event->xconfigure.height);
        break;
    case SelectionRequest:
        handle_selection_request(&event->xselectionrequest);
        break;
    case SelectionClear:
        free(g_clip_text);
        g_clip_text = NULL;
        break;
    case ClientMessage:
        if ((Atom)event->xclient.data.l[0] == g_wm_delete)
            g_running = 0;
        break;
    default:
        break;
    }
}

int main(int argc, char** argv)
{
    setlocale(LC_ALL, "");

    g_dpy = XOpenDisplay(NULL);
    if (!g_dpy) {
        fprintf(stderr, "x11: cannot open display\n");
        return 1;
    }
    g_screen = DefaultScreen(g_dpy);
    g_visual = DefaultVisual(g_dpy, g_screen);
    g_cmap = DefaultColormap(g_dpy, g_screen);

    apply_font();
    if (!g_font) {
        fprintf(stderr, "x11: no usable font found (is fontconfig installed?)\n");
        return 1;
    }

    int win_w = ui_default_cols * g_cell_w;
    int win_h = ui_default_rows * g_cell_h;

    XSetWindowAttributes attrs = {0};
    /* menu_bg, matching the pixmap fills below - so the window's own
     * background (visible briefly before the first pixmap paint, or through
     * any sliver the cell grid doesn't exactly cover) doesn't stand out. */
    attrs.background_pixel = rgb_to_pixel(ui_get_theme()->menu_bg);
    attrs.bit_gravity = NorthWestGravity;
    g_win = XCreateWindow(g_dpy, RootWindow(g_dpy, g_screen), 0, 0,
                           (unsigned)win_w, (unsigned)win_h, 0,
                           DefaultDepth(g_dpy, g_screen), InputOutput, g_visual,
                           CWBackPixel | CWBitGravity, &attrs);

    XStoreName(g_dpy, g_win, "");
    set_wm_resize_hints();  /* see its own comment - live-resize cell snapping */

    g_wm_delete = XInternAtom(g_dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(g_dpy, g_win, &g_wm_delete, 1);

    g_clipboard_atom = XInternAtom(g_dpy, "CLIPBOARD", False);
    g_utf8_string_atom = XInternAtom(g_dpy, "UTF8_STRING", False);
    g_targets_atom = XInternAtom(g_dpy, "TARGETS", False);

    setup_xim();

    XSelectInput(g_dpy, g_win, ExposureMask | KeyPressMask | ButtonPressMask |
                 ButtonReleaseMask | PointerMotionMask | StructureNotifyMask);

    g_gc = XCreateGC(g_dpy, g_win, 0, NULL);
    g_pict_format = XRenderFindVisualFormat(g_dpy, g_visual);
    ensure_pixmap(win_w, win_h);
    set_normal_cursor();

    g_env = ui_env_create(ui_default_cols, ui_default_rows);
    ui_env_set_font_zoom_fn(g_env, adjust_font_size, NULL);

    XMapWindow(g_dpy, g_win);

    int xfd = ConnectionNumber(g_dpy);

    while (g_running) {
        while (XPending(g_dpy)) {
            XEvent event;
            XNextEvent(g_dpy, &event);
            if (g_xic && XFilterEvent(&event, g_win))
                continue;  /* consumed by input-method composition */
            handle_event(&event);
            g_dirty = 1;  /* an X event (input, expose, resize) - repaint */
        }

        if (!g_app_initialized) {
            app_init(g_env);
            app_main(argc, argv);
            g_app_initialized = 1;
        }
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        unsigned now = (unsigned)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
        ui_env_set_time_ms(g_env, now);  /* drives the caret blink */

        if (app_frame(g_env))
            g_running = 0;

        /* Only repaint when an event changed something or on the slow baseline
         * (which drives the caret blink and shows delayed state). Idle = almost
         * no work, instead of a full redraw every loop. */
        if (g_dirty || now - g_last_render_ms >= RENDER_BASELINE_MS) {
            g_dirty = 0;
            g_last_render_ms = now;
            render_frame();
        }

        /* Block until the next X event arrives (waking instantly on input) or
         * the baseline timeout elapses - so an idle loop sleeps at ~60Hz
         * rather than spinning. */
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(xfd, &fds);
        struct timeval tv = { 0, 16000 };  /* 16ms */
        select(xfd + 1, &fds, NULL, NULL, &tv);
    }

    /* Reached whether the loop ended via File > Exit's g_quit path (which
     * already saved the session itself - see app_frame) or the window
     * manager's own close button (WM_DELETE_WINDOW just sets g_running = 0
     * directly above, bypassing app_frame entirely) - one save either way,
     * same "harmless double-save" reasoning as ide_win32.c's WM_DESTROY. */
    if (g_app_initialized)
        app_shutdown();

    if (g_xic) XDestroyIC(g_xic);
    if (g_xim) XCloseIM(g_xim);
    ui_env_free(g_env);
    if (g_font) XftFontClose(g_dpy, g_font);
    if (g_font_box) XftFontClose(g_dpy, g_font_box);
    if (g_xft_draw) XftDrawDestroy(g_xft_draw);
    if (g_pixmap_picture) XRenderFreePicture(g_dpy, g_pixmap_picture);
    if (g_pixmap) XFreePixmap(g_dpy, g_pixmap);
    XFreeGC(g_dpy, g_gc);
    XDestroyWindow(g_dpy, g_win);
    XCloseDisplay(g_dpy);
    free(g_clip_text);
    return 0;
}
