/* win32.c - Windows framework/backend.
 *
 * Manages the window, translates Win32 events into ui_env events, renders
 * by calling app_render(), which in turn calls ui_screen_render() -> the
 * ui_draw_char()/ui_draw_box() functions implemented below with GDI.
 * Completely generic - calls into the app via an abstract interface
 * (app_init, app_frame, app_render). Does NOT know what the app does, or
 * anything about its DOM/ui_screen.
 */
#include "ide_ui.h"

#include <windows.h>
#include <shellapi.h>
#include <stdlib.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")  /* CommandLineToArgvW, for app_main()'s argv */

/* Extract mouse coordinates from lParam. */
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

static ui_env *g_env;
static HFONT g_font;      /* ClearType - used for everything except box-drawing */
static HFONT g_font_box;  /* non-antialiased - box-drawing glyphs only, see ui_draw_char */
static int g_cell_w = 8;
static int g_cell_h = 16;
static int g_font_pt = 11;  /* adjustable via Ctrl+/Ctrl- */

/* Single persistent off-screen bitmap: cleared and fully redrawn (by walking
 * the app's DOM) every frame, then blitted once in WM_PAINT. This is plain
 * double-buffering for a flicker-free blit - unlike the old cell-diffing
 * scheme, there's no "clean copy to recomposite shadows from" concern here,
 * since every frame is a fresh full redraw anyway. */
static HDC g_mem;
static HBITMAP g_bmp;
static int g_bmp_w, g_bmp_h;

/* 1x1 black bitmap stretched over each shadow rect as the AlphaBlend source -
 * created once (any solid color works; only its alpha matters). */
static HDC g_shadow_src;
static HBITMAP g_shadow_src_bmp;

/* AlphaBlend lives in msimg32.dll; load it dynamically rather than linking
 * against an import lib that isn't guaranteed present in every toolchain -
 * same reasoning as the dynamically-loaded DPI-awareness calls below. */
typedef BOOL (WINAPI *AlphaBlendFn)(HDC, int, int, int, int,
                                     HDC, int, int, int, int, BLENDFUNCTION);
static AlphaBlendFn g_AlphaBlend;

/* App state. */
static int g_app_initialized = 0;

/* argc/argv for app_main() - wWinMain only gets a single PWSTR of everything
 * after the program name (no argv[0], and it's not pre-split the way a
 * normal main()'s argv is), so these are built once in wWinMain via
 * CommandLineToArgvW + WideCharToMultiByte before the first pump_frame()
 * call, then just handed to app_main() as plain UTF-8 argv. */
static int g_argc = 0;
static char** g_argv = NULL;

/* Render throttling: app_frame() runs every timer tick (cheap - drains input,
 * hit-tests), but the expensive full-DOM redraw + blit (refresh()) only runs
 * when something actually changed. g_dirty is set by every input message;
 * otherwise a slow baseline still repaints periodically so time-based state
 * (the caret blink, an auto-compile finishing) shows without any input. This
 * is what keeps an idle window from pinning a CPU core - see WM_TIMER. */
static int g_dirty = 1;                 /* need the very first frame */
static unsigned g_last_render_ms = 0;
#define RENDER_BASELINE_MS 250          /* < the 530ms caret-blink half-period */

/* Running total of raw WM_MOUSEWHEEL deltas not yet resolved into a full
 * notch - see WM_MOUSEWHEEL below. Precision touchpads and many "smooth
 * scroll" mice split one notch across several messages whose individual
 * deltas don't divide evenly by WHEEL_DELTA (120); this carries the
 * remainder forward across messages instead of truncating (and losing) it. */
static int g_wheel_accum = 0;

/* GDI has no automatic font fallback (TextOutW draws a missing glyph as a
 * tofu box using whatever font is selected) - so pick one font up front that
 * actually has clean box-drawing glyphs, rather than assuming Consolas. */
static int CALLBACK font_exists_proc(const LOGFONTW *lf, const TEXTMETRICW *tm,
                                      DWORD type, LPARAM lparam)
{
    (void)lf; (void)tm; (void)type;
    *(int *)lparam = 1;
    return 0;  /* non-zero would continue enumerating; we only need one hit */
}

static int font_exists(HDC dc, const wchar_t *name)
{
    LOGFONTW lf = {0};
    lf.lfCharSet = DEFAULT_CHARSET;
    int n = 0;
    while (name[n] && n < LF_FACESIZE - 1) { lf.lfFaceName[n] = name[n]; n++; }
    lf.lfFaceName[n] = 0;

    int found = 0;
    EnumFontFamiliesExW(dc, &lf, font_exists_proc, (LPARAM)&found, 0);
    return found;
}

/* Preference order: Cascadia Mono (ships with Windows Terminal on 10/11,
 * excellent box-drawing) > Cascadia Code > Consolas > DejaVu Sans Mono (often
 * present via other dev tools) > Lucida Console > Courier New (always on
 * Windows - the final fallback). */
static const wchar_t *pick_font(HDC dc)
{
    static const wchar_t *candidates[] = {
        L"Cascadia Mono", L"Cascadia Code", L"Consolas",
        L"DejaVu Sans Mono", L"Lucida Console", L"Courier New",
    };
    for (size_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]); i++) {
        if (font_exists(dc, candidates[i]))
            return candidates[i];
    }
    return L"Courier New";
}

/* Our colors are 0x00RRGGBB; GDI COLORREF is 0x00BBGGRR. Swap R and B. */
static COLORREF to_colorref(uint32_t rgb)
{
    return RGB((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);
}

/* Encode a code point as UTF-16 into out (1 or 2 units). Returns unit count. */
static int cp_to_utf16(uint32_t cp, WCHAR out[2])
{
    if (cp > 0xFFFF) {
        cp -= 0x10000;
        out[0] = (WCHAR)(0xD800 + (cp >> 10));
        out[1] = (WCHAR)(0xDC00 + (cp & 0x3FF));
        return 2;
    }
    out[0] = (WCHAR)cp;
    return 1;
}

/* Glyphs drawn with the non-antialiased font (see apply_font) instead of
 * ClearType. ClearType's subpixel antialiasing spills a color fringe past
 * the glyph's cell that doesn't land consistently between our
 * independently-drawn cells - visible as seams/gaps in border lines and
 * dithered fills, and (with the incremental partial-redraw) as stale fringe
 * pixels left behind when a neighbor cell repaints, e.g. beside the moving
 * scrollbar arrows. So all UI chrome symbols render crisp and self-contained:
 *   - U+2500-259F  Box Drawing + Block Elements (borders, dithered desktop)
 *   - U+2190-21FF  Arrows (scrollbar up/down, zoom up-down icon)
 *   - U+25A0-25FF  Geometric Shapes (close square, submenu triangle,
 *                  <select> down-arrow lives in the Arrows block above)
 * Actual text keeps ClearType, which looks better for everything else. */
static int is_box_drawing(uint32_t ch)
{
    return (ch >= 0x2500 && ch <= 0x259F) ||
           (ch >= 0x2190 && ch <= 0x21FF) ||
           (ch >= 0x25A0 && ch <= 0x25FF);
}

/* ui_draw_char (see ui.h): paint one glyph cell directly into the
 * (already-cleared) back-bitmap g_mem.
 *
 * ETO_CLIPPED clips the glyph to its own cell. Without it, a glyph's overhang
 * - especially ClearType's subpixel fringe, but also any glyph slightly wider
 * than the monospace cell - spills into the neighbouring cell's pixels. Since
 * cells are drawn independently and the incremental renderer only repaints
 * cells that changed, that spill is never cleaned when the neighbour doesn't
 * itself repaint: e.g. editor text bleeding right into the (static, blank)
 * scrollbar column leaves stale fringe along the track as the view scrolls.
 * Clipping keeps every cell self-contained. */
void ui_draw_char(int x, int y, uint32_t ch, uint32_t fg, uint32_t bg)
{
    HDC mem = g_mem;
    int px = x * g_cell_w;
    int py = y * g_cell_h;
    COLORREF bgc = to_colorref(bg);
    COLORREF fgc = to_colorref(fg);

    RECT cellrc = { px, py, px + g_cell_w, py + g_cell_h };

    SelectObject(mem, is_box_drawing(ch) ? g_font_box : g_font);
    SetBkColor(mem, bgc);
    SetTextColor(mem, fgc);
    WCHAR w[2];
    int n = cp_to_utf16(ch ? ch : ' ', w);
    ExtTextOutW(mem, px, py, ETO_OPAQUE | ETO_CLIPPED, &cellrc, w, n, NULL);
}

/* ui_draw_box (see ui.h): a solid w x h rect drawn into the back-bitmap
 * g_mem - ui.c's replacement for building the same fill out of a per-cell
 * ui_draw_char loop. Every current caller (see draw_fill in ui.c) fills
 * with a blank cell, so there's no glyph to render at all - a normal fill
 * (flags == 0) is just one FillRect over the whole rect, no text/font
 * machinery involved; `fg` is unused.
 *
 * UI_BOX_INVERT: true XOR cursor - DSTINVERT flips every bit of whatever's
 * already there (dest = ~dest), so it needs no color of its own and is
 * automatically legible over anything already painted.
 *
 * UI_BOX_SHADOW: darken the rect via true alpha blending. ui.c passes
 * shadow pieces in whole cells, but the classic Turbo Vision shadow is only
 * half a cell thick, hugging the widget it's cast from rather than filling
 * a full extra row/column - the UI_BOX_SHADOW_SHRINK_* flags say which
 * pixel dimension(s) to halve, explicit rather than inferred from w/h,
 * since a height-1 widget's corner cell is 1x1 either way. */
void ui_draw_box(int x, int y, int w, int h, uint32_t fg, uint32_t bg, int flags)
{
    HDC mem = g_mem;
    (void)fg;

    if (flags & UI_BOX_INVERT) {
        PatBlt(mem, x * g_cell_w, y * g_cell_h, w * g_cell_w, h * g_cell_h, DSTINVERT);
        return;
    }
    if (flags & UI_BOX_SHADOW) {
        if (!g_AlphaBlend)
            return;  /* msimg32.dll unavailable: shadows just don't render */
        int px = x * g_cell_w, py = y * g_cell_h;
        int pw = w * g_cell_w, ph = h * g_cell_h;
        if (flags & UI_BOX_SHADOW_SHRINK_W)
            pw = g_cell_w / 2;
        if (flags & UI_BOX_SHADOW_SHRINK_H)
            ph = g_cell_h / 2;
        if (flags & UI_BOX_SHADOW_SHRINK_H_BOTTOM) {
            ph = g_cell_h / 2;
            py += g_cell_h - ph;  /* keep the bottom half instead of the top */
        }
        BLENDFUNCTION blend = { AC_SRC_OVER, 0, 110, 0 };  /* ~43% opacity black */
        g_AlphaBlend(mem, px, py, pw, ph, g_shadow_src, 0, 0, 1, 1, blend);
        return;
    }

    RECT rc = { x * g_cell_w, y * g_cell_h, (x + w) * g_cell_w, (y + h) * g_cell_h };
    HBRUSH br = CreateSolidBrush(to_colorref(bg));
    FillRect(mem, &rc, br);
    DeleteObject(br);
}

static void clear_backbuffer(void);  /* defined with refresh(), below */

/* (Re)create the persistent back-bitmap to match the client area. */
static void ensure_bitmap(HWND hwnd)
{
    RECT rc;
    GetClientRect(hwnd, &rc);
    if (rc.right < 1) rc.right = 1;
    if (rc.bottom < 1) rc.bottom = 1;
    if (g_mem && rc.right == g_bmp_w && rc.bottom == g_bmp_h)
        return;

    if (g_mem) { DeleteObject(g_bmp); DeleteDC(g_mem); }

    HDC dc = GetDC(hwnd);
    g_mem = CreateCompatibleDC(dc);
    g_bmp = CreateCompatibleBitmap(dc, rc.right, rc.bottom);

    if (!g_shadow_src) {
        /* One-time 1x1 source bitmap for AlphaBlend, reused/stretched over
         * every shadow rect regardless of size. */
        g_shadow_src = CreateCompatibleDC(dc);
        g_shadow_src_bmp = CreateCompatibleBitmap(dc, 1, 1);
        SelectObject(g_shadow_src, g_shadow_src_bmp);
        SetPixel(g_shadow_src, 0, 0, RGB(0, 0, 0));
    }
    ReleaseDC(hwnd, dc);

    SelectObject(g_mem, g_bmp);
    SelectObject(g_mem, g_font);
    SetBkMode(g_mem, OPAQUE);

    g_bmp_w = rc.right;
    g_bmp_h = rc.bottom;

    /* Fresh bitmap: give it a known background, and discard the render shadow
     * so the next frame repaints everything into it (its pixels are new). */
    clear_backbuffer();
    app_invalidate();
}

/* Defined later (with the rest of the DWM title bar tinting code) - forward
 * declared so refresh() below can keep the OS chrome in sync with the app's
 * theme every frame. */
static void sync_titlebar_color(HWND hwnd, uint32_t rgb);

/* Fill the whole back-bitmap with the theme's menu background. Done ONCE
 * when the bitmap is (re)created (see ensure_bitmap) - never per frame - so
 * cells the incremental renderer chooses to skip keep last frame's pixels
 * instead of being erased. Maximizing (or any resize) that doesn't land on
 * an exact multiple of the cell size leaves a sliver past the last full row/
 * column that no cell ever draws over, so it stays whatever color this
 * fills with - menu_bg (rather than a hardcoded color) so that sliver blends
 * with the menu bar instead of standing out as a mismatched gap. */
static void clear_backbuffer(void)
{
    if (!g_mem)
        return;
    RECT full = { 0, 0, g_bmp_w, g_bmp_h };
    HBRUSH bg = CreateSolidBrush(to_colorref(ui_get_theme()->menu_bg));
    FillRect(g_mem, &full, bg);
    DeleteObject(bg);
}

/* Have the app redraw whatever changed into the back-bitmap, then invalidate
 * only the changed region so WM_PAINT blits just that (the back-bitmap is a
 * complete image, so a partial blit is always valid). Nothing changed -> no
 * invalidate, no blit. The region is cell-tight - glyphs are clipped to their
 * cells (see ui_draw_char's ETO_CLIPPED), so nothing spills past a cell that
 * would need an extra margin here. */
static void refresh(HWND hwnd)
{
    if (!g_app_initialized)
        return;

    sync_titlebar_color(hwnd, ui_get_theme()->menu_bg);

    int dx, dy, dw, dh;
    if (app_render(&dx, &dy, &dw, &dh)) {
        RECT r = { dx * g_cell_w, dy * g_cell_h,
                   (dx + dw) * g_cell_w, (dy + dh) * g_cell_h };
        InvalidateRect(hwnd, &r, FALSE);
    }
}

static void paint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC dc = BeginPaint(hwnd, &ps);
    RECT r = ps.rcPaint;  /* the OS-accumulated invalid region (see refresh) */
    BitBlt(dc, r.left, r.top, r.right - r.left, r.bottom - r.top,
           g_mem, r.left, r.top, SRCCOPY);
    EndPaint(hwnd, &ps);
}

static void resize_buffer(HWND hwnd)
{
    RECT rc;
    GetClientRect(hwnd, &rc);
    int cols = rc.right / g_cell_w;
    int rows = rc.bottom / g_cell_h;
    if (cols < 1) cols = 1;
    if (rows < 1) rows = 1;
    ui_env_resize(g_env, cols, rows);
    ensure_bitmap(hwnd);
    refresh(hwnd);
}

/* Total non-client size (title bar + borders): window rect minus client rect. */
static void get_border_size(HWND hwnd, int *bw, int *bh)
{
    RECT wr, cr;
    GetWindowRect(hwnd, &wr);
    GetClientRect(hwnd, &cr);
    *bw = (wr.right - wr.left) - (cr.right - cr.left);
    *bh = (wr.bottom - wr.top) - (cr.bottom - cr.top);
}

/* Resize the window so its client area is exactly cols*cell_w x rows*cell_h -
 * used right after creation so the initial window has no partial cell. */
static void resize_window_to_grid(HWND hwnd, int cols, int rows)
{
    int bw, bh;
    get_border_size(hwnd, &bw, &bh);
    SetWindowPos(hwnd, NULL, 0, 0, cols * g_cell_w + bw, rows * g_cell_h + bh,
                 SWP_NOMOVE | SWP_NOZORDER);
}

/* (Re)create g_font at g_font_pt and derive g_cell_w/h from its metrics.
 * Called at window creation and again from Ctrl+/Ctrl- to change size. */
static void apply_font(HWND hwnd)
{
    HDC dc = GetDC(hwnd);

    /* Size the font to the real monitor DPI so Windows doesn't bitmap-
     * stretch (and blur) our output. With the process marked DPI-aware,
     * LOGPIXELSY reports the true DPI. */
    int dpi = GetDeviceCaps(dc, LOGPIXELSY);
    int height = -MulDiv(g_font_pt, dpi, 72);

    /* Monospace font; derive cell size from its metrics. Two variants at the
     * same size/family: ClearType for everything, and a non-antialiased twin
     * used only for box-drawing glyphs (ui_draw_char picks between them) -
     * ClearType's subpixel positioning doesn't line up consistently between
     * our independently-drawn cells, leaving visible gaps in border lines;
     * plain text doesn't have that problem and looks better antialiased. */
    const wchar_t *font_name = pick_font(dc);
    HFONT new_font = CreateFontW(height, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                  CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                  FIXED_PITCH | FF_MODERN, font_name);
    HFONT new_font_box = CreateFontW(height, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                      DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                      CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY,
                                      FIXED_PITCH | FF_MODERN, font_name);
    HFONT old = SelectObject(dc, new_font);
    TEXTMETRICW tm;
    GetTextMetricsW(dc, &tm);
    /* Exact glyph advance for a monospace cell (tmAveCharWidth rounds). */
    SIZE sz;
    GetTextExtentPoint32W(dc, L"M", 1, &sz);
    g_cell_w = sz.cx;
    g_cell_h = tm.tmHeight;
    SelectObject(dc, old);
    ReleaseDC(hwnd, dc);

    if (g_font)
        DeleteObject(g_font);
    if (g_font_box)
        DeleteObject(g_font_box);
    g_font = new_font;
    g_font_box = new_font_box;

    /* Every cell's pixel size/position just changed, so the render shadow no
     * longer matches the screen - repaint fully next frame. (On a font-zoom
     * this is usually followed by a window resize -> ensure_bitmap, but if the
     * pixel size happens not to change, this is the only thing that forces the
     * stale buffer to be redrawn.) */
    clear_backbuffer();
    app_invalidate();
}

/* Change the font size by `delta` points (positive to grow, negative to
 * shrink), clamped to a sane range, and resize the window to keep the same
 * column/row count at the new cell size. A no-op if already at a clamp
 * bound. Registered with the ui_env as its font-zoom callback (see
 * ui_env_set_font_zoom_fn in ui.h) - invoked uniformly via
 * ui_env_adjust_font_size(), whether the trigger is Ctrl+/Ctrl- (WM_KEYDOWN
 * below) or the app's Window > Font Size +/- menu items. */
static void adjust_font_size(void *ctx, int delta)
{
    HWND hwnd = (HWND)ctx;
    int new_pt = g_font_pt + delta;
    if (new_pt < 6) new_pt = 6;
    if (new_pt > 36) new_pt = 36;
    if (new_pt == g_font_pt)
        return;

    g_font_pt = new_pt;
    int cols = ui_env_width(g_env);
    int rows = ui_env_height(g_env);
    apply_font(hwnd);
    resize_window_to_grid(hwnd, cols, rows);  /* triggers WM_SIZE -> refresh */
}

/* One app tick: process a frame and, if warranted, repaint. `force_paint`
 * repaints immediately (and blits synchronously) regardless of the throttle -
 * used on keystrokes so the caret/typing responds without the up-to-16ms
 * timer latency; the timer itself passes 0 (throttled). */
static void pump_frame(HWND hwnd, int force_paint)
{
    if (!g_app_initialized) {
        app_init(g_env);
        app_main(g_argc, g_argv);
        g_app_initialized = 1;
    }
    unsigned now = GetTickCount();
    ui_env_set_time_ms(g_env, now);  /* drives the caret blink */
    if (app_frame(g_env)) {
        PostQuitMessage(0);
        return;
    }
    if (force_paint || g_dirty || now - g_last_render_ms >= RENDER_BASELINE_MS) {
        g_dirty = 0;
        g_last_render_ms = now;
        refresh(hwnd);
        if (force_paint)
            UpdateWindow(hwnd);  /* blit now, don't wait for the queued WM_PAINT */
    }
}

static LRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
    case WM_CREATE: {
        apply_font(hwnd);

        g_env = ui_env_create(ui_default_cols, ui_default_rows);
        ui_env_set_font_zoom_fn(g_env, adjust_font_size, hwnd);
        ensure_bitmap(hwnd);
        /* ~60Hz tick to pump the app. Rendering is throttled separately (see
         * WM_TIMER) - the tick itself is cheap; the old 1ms timer made it try
         * to fully redraw up to ~1000x/sec, pinning the CPU (badly so with two
         * instances). */
        SetTimer(hwnd, 1, 16, NULL);
        return 0;
    }
    case WM_SIZE:
        resize_buffer(hwnd);
        return 0;
    case WM_SIZING: {
        /* Snap the drag rectangle to a whole number of cells live, so the
         * window never settles on a partial row/column. */
        RECT *r = (RECT *)lp;
        int bw, bh;
        get_border_size(hwnd, &bw, &bh);

        int client_w = (r->right - r->left) - bw;
        int client_h = (r->bottom - r->top) - bh;
        int cols = (client_w + g_cell_w / 2) / g_cell_w;
        int rows = (client_h + g_cell_h / 2) / g_cell_h;
        if (cols < 1) cols = 1;
        if (rows < 1) rows = 1;
        int new_w = cols * g_cell_w + bw;
        int new_h = rows * g_cell_h + bh;

        /* Keep the edge opposite the one being dragged fixed. */
        switch (wp) {
        case WMSZ_LEFT: case WMSZ_TOPLEFT: case WMSZ_BOTTOMLEFT:
            r->left = r->right - new_w;
            break;
        default:
            r->right = r->left + new_w;
            break;
        }
        switch (wp) {
        case WMSZ_TOP: case WMSZ_TOPLEFT: case WMSZ_TOPRIGHT:
            r->top = r->bottom - new_h;
            break;
        default:
            r->bottom = r->top + new_h;
            break;
        }
        return TRUE;
    }
    case WM_TIMER:
        /* Throttled tick - repaints only on the baseline or a pending change
         * (idle windows stay cheap). Keystrokes force an immediate repaint
         * themselves (see WM_KEYDOWN/WM_CHAR), bypassing this throttle. */
        pump_frame(hwnd, 0);
        return 0;
    case WM_KEYDOWN: {
        /* Ctrl+/Ctrl- resize the font - a window-management concern the
         * backend handles itself, never surfaced to the app as a key event. */
        if (GetKeyState(VK_CONTROL) < 0 &&
            (wp == VK_OEM_PLUS || wp == VK_ADD || wp == VK_OEM_MINUS || wp == VK_SUBTRACT)) {
            int grew = (wp == VK_OEM_PLUS || wp == VK_ADD);
            ui_env_adjust_font_size(g_env, grew ? 1 : -1);
            return 0;
        }

        /* Non-printable/special keys arrive here, not via WM_CHAR (which only
         * fires for characters). Translate the ones we care about and post
         * them; let the app decide what to do. */
        int code = 0;
        switch (wp) {
        case VK_UP:     code = UI_KEY_UP;       break;
        case VK_DOWN:   code = UI_KEY_DOWN;     break;
        case VK_LEFT:   code = UI_KEY_LEFT;     break;
        case VK_RIGHT:  code = UI_KEY_RIGHT;    break;
        case VK_HOME:   code = UI_KEY_HOME;     break;
        case VK_END:    code = UI_KEY_END;      break;
        case VK_PRIOR:  code = UI_KEY_PAGEUP;   break;
        case VK_NEXT:   code = UI_KEY_PAGEDOWN; break;
        case VK_DELETE: code = UI_KEY_DELETE;   break;
        case VK_INSERT: code = UI_KEY_INSERT;   break;
        case VK_ESCAPE: code = UI_KEY_ESCAPE;   break;
        case VK_TAB:    code = UI_KEY_TAB;      break;
        case VK_F1:  case VK_F2:  case VK_F3:  case VK_F4:
        case VK_F5:  case VK_F6:  case VK_F7:  case VK_F8:
        case VK_F9:  case VK_F10: case VK_F11: case VK_F12:
            code = UI_KEY_F1 + (int)(wp - VK_F1);
            break;
        default:
            return DefWindowProcW(hwnd, msg, wp, lp);  /* let WM_CHAR handle it */
        }

        ui_event ev = {0};
        ev.type = UI_EVENT_KEY;
        ev.data.key.code = code;
        ev.data.key.mods = (GetKeyState(VK_SHIFT) < 0 ? UI_MOD_SHIFT : 0) |
                           (GetKeyState(VK_CONTROL) < 0 ? UI_MOD_CTRL : 0) |
                           (GetKeyState(VK_MENU) < 0 ? UI_MOD_ALT : 0);
        ui_env_post_event(g_env, &ev);
        pump_frame(hwnd, 1);  /* process + repaint now, don't wait for the timer */
        return 0;
    }
    case WM_CHAR: {
        /* Printable characters and a few control chars (backspace, enter).
         * Just post the event - the app owns all state. */
        WCHAR w = (WCHAR)wp;
        ui_event ev = {0};
        ev.type = UI_EVENT_KEY;
        ev.data.key.codepoint = w;
        ev.data.key.mods = (GetKeyState(VK_SHIFT) < 0 ? UI_MOD_SHIFT : 0) |
                           (GetKeyState(VK_CONTROL) < 0 ? UI_MOD_CTRL : 0) |
                           (GetKeyState(VK_MENU) < 0 ? UI_MOD_ALT : 0);
        ui_env_post_event(g_env, &ev);
        pump_frame(hwnd, 1);  /* process + repaint now, don't wait for the timer */
        return 0;
    }
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP: {
        /* Post mouse events to the environment's queue. */
        int x = GET_X_LPARAM(lp) / g_cell_w;
        int y = GET_Y_LPARAM(lp) / g_cell_h;
        ui_event ev = {0};
        ev.type = UI_EVENT_MOUSE;
        ev.data.mouse.x = x;
        ev.data.mouse.y = y;
        ev.data.mouse.mods = (GetKeyState(VK_SHIFT) < 0 ? UI_MOD_SHIFT : 0) |
                             (GetKeyState(VK_CONTROL) < 0 ? UI_MOD_CTRL : 0) |
                             (GetKeyState(VK_MENU) < 0 ? UI_MOD_ALT : 0);

        if (msg == WM_MOUSEMOVE) {
            ev.data.mouse.action = UI_MOUSE_MOVED;
        } else if (msg == WM_LBUTTONDBLCLK) {
            /* Windows' own double-click detection (CS_DBLCLKS on the window
             * class) - not something we time/synthesize ourselves. */
            ev.data.mouse.action = UI_MOUSE_DBLCLICK;
            ev.data.mouse.button = UI_MOUSE_BUTTON_LEFT;
        } else if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN) {
            ev.data.mouse.action = UI_MOUSE_PRESSED;
            ev.data.mouse.button = (msg == WM_LBUTTONDOWN ? UI_MOUSE_BUTTON_LEFT :
                                    msg == WM_RBUTTONDOWN ? UI_MOUSE_BUTTON_RIGHT :
                                    UI_MOUSE_BUTTON_MIDDLE);
        } else {
            ev.data.mouse.action = UI_MOUSE_RELEASED;
            ev.data.mouse.button = (msg == WM_LBUTTONUP ? UI_MOUSE_BUTTON_LEFT :
                                    msg == WM_RBUTTONUP ? UI_MOUSE_BUTTON_RIGHT :
                                    UI_MOUSE_BUTTON_MIDDLE);
        }
        ui_env_post_event(g_env, &ev);
        g_dirty = 1;  /* input changed something - repaint next tick */
        return 0;
    }
    case WM_MOUSEWHEEL: {
        /* Unlike the other mouse messages, this one's position is in SCREEN
         * coordinates, not client - has to be converted before dividing down
         * to the character grid. */
        POINT pt = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
        ScreenToClient(hwnd, &pt);

        /* HIWORD(wp) is this message's own raw signed delta - WHEEL_DELTA
         * (120) is one full notch, but that's not a guarantee any single
         * message carries a multiple of it: precision touchpads and many
         * "smooth scroll" mice split one notch across several messages
         * (e.g. 40, 40, 40) meant to accumulate into a full notch over
         * time. Dividing each message's delta by WHEEL_DELTA on its own
         * truncates every one of those partial messages to zero, silently
         * dropping the scroll instead of ever letting it add up - so
         * accumulate across messages here and only emit whole notches once
         * the running total reaches one, carrying any leftover forward. */
        g_wheel_accum += (int)((short)HIWORD(wp));
        int notches = g_wheel_accum / WHEEL_DELTA;
        g_wheel_accum -= notches * WHEEL_DELTA;
        if (notches == 0)
            return 0;  /* not a full notch yet - saved in g_wheel_accum for next time */

        ui_event ev = {0};
        ev.type = UI_EVENT_MOUSE;
        ev.data.mouse.x = pt.x / g_cell_w;
        ev.data.mouse.y = pt.y / g_cell_h;
        ev.data.mouse.action = UI_MOUSE_WHEEL;
        ev.data.mouse.wheel_delta = notches;
        ev.data.mouse.mods = (GetKeyState(VK_SHIFT) < 0 ? UI_MOD_SHIFT : 0) |
                             (GetKeyState(VK_CONTROL) < 0 ? UI_MOD_CTRL : 0) |
                             (GetKeyState(VK_MENU) < 0 ? UI_MOD_ALT : 0);
        ui_env_post_event(g_env, &ev);
        g_dirty = 1;  /* input changed something - repaint next tick */
        return 0;
    }

    case WM_PAINT:
        paint(hwnd);
        return 0;
    case WM_ERASEBKGND:
        return 1;  /* handled in paint; avoid flicker */
    case WM_DESTROY:
        /* Reached by the native title bar's close button (WM_CLOSE's default
         * handling calls DestroyWindow straight away) WITHOUT app_frame()
         * ever returning non-zero - File > Exit's g_quit path is the other
         * way to end up here, and it already saves the session itself
         * before PostQuitMessage ever gets posted (see app_frame), so this
         * is a no-op double-save in that case, not a missed one in this
         * one. */
        if (g_app_initialized)
            app_shutdown();
        KillTimer(hwnd, 1);
        if (g_mem) { DeleteObject(g_bmp); DeleteDC(g_mem); }
        if (g_shadow_src) { DeleteObject(g_shadow_src_bmp); DeleteDC(g_shadow_src); }
        ui_env_free(g_env);
        DeleteObject(g_font);
        DeleteObject(g_font_box);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

/* Mark the process per-monitor DPI-aware so Windows renders our window at the
 * monitor's true resolution instead of stretching a 96-DPI bitmap (the usual
 * cause of blurry text). Loaded dynamically to work on any Windows/header
 * version, with graceful fallbacks. */
static void enable_dpi_awareness(void)
{
    HMODULE u32 = GetModuleHandleW(L"user32.dll");
    typedef BOOL (WINAPI *SetCtxFn)(HANDLE);
    SetCtxFn set_ctx = (SetCtxFn)(void *)GetProcAddress(u32, "SetProcessDpiAwarenessContext");
    if (set_ctx) {
        /* DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 == (HANDLE)-4 */
        if (set_ctx((HANDLE)-4))
            return;
    }
    /* System-DPI-aware fallback (Vista+), also loaded dynamically. */
    typedef BOOL (WINAPI *SetAwareFn)(void);
    SetAwareFn set_aware = (SetAwareFn)(void *)GetProcAddress(u32, "SetProcessDPIAware");
    if (set_aware)
        set_aware();
}

/* Load AlphaBlend from msimg32.dll. Leaves g_AlphaBlend NULL if unavailable
 * (ui_draw_box's UI_BOX_SHADOW branch then just skips shadow rendering -
 * graceful, since shadows are a cosmetic enhancement, not required for the
 * app to function). */
static void load_alpha_blend(void)
{
    HMODULE msimg = LoadLibraryW(L"msimg32.dll");
    if (msimg)
        g_AlphaBlend = (AlphaBlendFn)(void *)GetProcAddress(msimg, "AlphaBlend");
}

/* DWMWA_BORDER_COLOR/DWMWA_CAPTION_COLOR are only in newer Windows SDKs and
 * only take effect on Windows 11 22H2+ - defining them ourselves (rather
 * than relying on dwmapi.h) means this builds fine with an older SDK too,
 * and DwmSetWindowAttribute simply fails harmlessly on Windows 10/older 11. */
#ifndef DWMWA_BORDER_COLOR
#define DWMWA_BORDER_COLOR 34
#endif
#ifndef DWMWA_CAPTION_COLOR
#define DWMWA_CAPTION_COLOR 35
#endif

typedef HRESULT (WINAPI *DwmSetWindowAttributeFn)(HWND, DWORD, LPCVOID, DWORD);

static DwmSetWindowAttributeFn g_dwm_set_attr;
static uint32_t g_titlebar_rgb = 0xFFFFFFFFu;  /* sentinel - forces the first apply */

/* Looked up once at startup - dwmapi.dll's function pointer never changes,
 * only the color we call it with does (see sync_titlebar_color below). */
static void load_dwm(void)
{
    HMODULE dwmapi = LoadLibraryW(L"dwmapi.dll");
    if (!dwmapi)
        return;
    g_dwm_set_attr = (DwmSetWindowAttributeFn)(void *)GetProcAddress(dwmapi, "DwmSetWindowAttribute");
}

/* Tint the OS title bar/border to match the app's own menu-bar color (the
 * live ui_theme's menu_bg, not a fixed one - so switching themes re-tints
 * the OS chrome too, not just what's drawn inside the client area). Called
 * every frame from refresh() but a no-op past the first whenever the color
 * hasn't actually changed, so a theme switch is the only time it does real
 * work. Simply skipped if dwmapi is unavailable or the attribute is
 * rejected (pre-Windows 11 22H2). */
static void sync_titlebar_color(HWND hwnd, uint32_t rgb)
{
    if (rgb == g_titlebar_rgb || !g_dwm_set_attr)
        return;
    g_titlebar_rgb = rgb;
    COLORREF color = to_colorref(rgb);
    g_dwm_set_attr(hwnd, DWMWA_CAPTION_COLOR, &color, sizeof(color));
    g_dwm_set_attr(hwnd, DWMWA_BORDER_COLOR, &color, sizeof(color));
}

/* Blank the title bar's text and remove its icon - the app draws its own
 * "chrome" (menu bar, status bar) inside the client area, so the default
 * icon/caption text just add clutter above it. The frame itself (and its
 * drag/resize/minimize/maximize/close behavior) stays untouched. */
static void remove_icon_and_caption(HWND hwnd)
{
    SetWindowTextW(hwnd, L"");
    SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)NULL);
    SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)NULL);

    /* WM_SETICON alone leaves a blank icon-sized gap in the caption on most
     * Windows versions - WS_EX_DLGMODALFRAME is what actually tells Windows
     * not to reserve/draw an icon slot at all. SWP_FRAMECHANGED forces the
     * non-client area to redo its layout with the new extended style. */
    LONG_PTR ex_style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex_style | WS_EX_DLGMODALFRAME);
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

/* --- Clipboard: the two hooks ui.c calls for <input>/<editor> copy/cut/
 * paste (see ui_screen_copy/cut/paste and Ctrl+C/X/V in ui_screen_update).
 * The OS clipboard only speaks UTF-16 (CF_UNICODETEXT) - convert at this
 * boundary so the rest of the app only ever deals in UTF-8. --- */

void ui_clipboard_set_text(const char *utf8)
{
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    if (wlen <= 0)
        return;

    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, (SIZE_T)wlen * sizeof(WCHAR));
    if (!mem)
        return;
    WCHAR *dst = (WCHAR *)GlobalLock(mem);
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, dst, wlen);
    GlobalUnlock(mem);

    if (!OpenClipboard(NULL)) {
        GlobalFree(mem);
        return;
    }
    EmptyClipboard();
    SetClipboardData(CF_UNICODETEXT, mem);  /* clipboard now owns `mem` */
    CloseClipboard();
}

char *ui_clipboard_get_text(void)
{
    if (!OpenClipboard(NULL))
        return NULL;

    HANDLE mem = GetClipboardData(CF_UNICODETEXT);
    if (!mem) {
        CloseClipboard();
        return NULL;
    }
    const WCHAR *src = (const WCHAR *)GlobalLock(mem);
    if (!src) {
        CloseClipboard();
        return NULL;
    }

    int len = WideCharToMultiByte(CP_UTF8, 0, src, -1, NULL, 0, NULL, NULL);
    char *out = len > 0 ? malloc((size_t)len) : NULL;
    if (out)
        WideCharToMultiByte(CP_UTF8, 0, src, -1, out, len, NULL, NULL);

    GlobalUnlock(mem);
    CloseClipboard();
    return out;
}

/* --- Filesystem: the directory-listing hook ui.c calls for a file-open
 * dialog (see ui_list_dir in ui.h). Same UTF-8/UTF-16 boundary as the
 * clipboard above - FindFirstFileW/FindNextFileW only speak UTF-16. --- */

int ui_list_dir(const char *path, ui_dir_entry *out, int max_entries)
{
    int wlen = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
    if (wlen <= 0)
        return -1;

    /* Classic FindFirstFileW is limited to MAX_PATH (260 chars) unless the
     * path carries the "\\?\" long-path prefix - which in turn requires an
     * absolute, backslash-only path with no "." or ".." components. That's
     * exactly the shape ui_get_cwd()/the app's own path-joining already
     * produce (an Open-File dialog navigates by stripping/appending real
     * directory names, never by leaving "." or ".." in the string), so
     * adding it unconditionally is safe - and fixes directories several
     * levels deep silently failing to list once the full path crossed 260
     * characters, with no other symptom than "some directories vanish". */
    static const WCHAR PREFIX[] = L"\\\\?\\";
    const int prefix_len = 4;

    /* +3 for a separator, '*', and the NUL the "\*" search pattern needs
     * appended after the path itself. */
    WCHAR *wpath = malloc((size_t)(prefix_len + wlen + 3) * sizeof(WCHAR));
    if (!wpath)
        return -1;
    memcpy(wpath, PREFIX, (size_t)prefix_len * sizeof(WCHAR));
    MultiByteToWideChar(CP_UTF8, 0, path, -1, wpath + prefix_len, wlen);

    /* "\\?\" doesn't understand "/" as a separator - normalize any the
     * app's own path-joining introduced (it always joins with "/"). */
    for (int i = prefix_len; wpath[i]; i++)
        if (wpath[i] == L'/')
            wpath[i] = L'\\';

    /* Strip any trailing separator first so "C:\FOO\" doesn't end up as
     * "C:\FOO\\*" (FindFirstFileW tolerates it, but this is tidier). */
    int plen = prefix_len + wlen - 1;  /* wlen counts the NUL; back off past it */
    while (plen > prefix_len && wpath[plen - 1] == L'\\')
        plen--;
    wpath[plen] = L'\\';
    wpath[plen + 1] = L'*';
    wpath[plen + 2] = 0;

    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(wpath, &fd);
    free(wpath);
    if (h == INVALID_HANDLE_VALUE)
        return -1;

    int count = 0;
    do {
        int is_dot = fd.cFileName[0] == L'.' &&
                     (fd.cFileName[1] == 0 || (fd.cFileName[1] == L'.' && fd.cFileName[2] == 0));
        if (is_dot)
            continue;
        if (count >= max_entries)
            break;

        ui_dir_entry *e = &out[count];
        WideCharToMultiByte(CP_UTF8, 0, fd.cFileName, -1, e->name, (int)sizeof(e->name), NULL, NULL);
        e->is_dir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        e->size = ((long long)fd.nFileSizeHigh << 32) | fd.nFileSizeLow;

        /* FILETIME: 100ns intervals since 1601-01-01 UTC. time_t: seconds
         * since 1970-01-01 UTC - 116444736000000000 is the gap between
         * those two epochs in 100ns units. */
        ULARGE_INTEGER uli;
        uli.LowPart = fd.ftLastWriteTime.dwLowDateTime;
        uli.HighPart = fd.ftLastWriteTime.dwHighDateTime;
        e->mtime = (time_t)((uli.QuadPart - 116444736000000000ULL) / 10000000ULL);

        count++;
    } while (FindNextFileW(h, &fd));

    FindClose(h);
    return count;
}

int ui_get_cwd(char *buf, int buf_size)
{
    WCHAR wbuf[MAX_PATH];
    DWORD n = GetCurrentDirectoryW(MAX_PATH, wbuf);
    if (n == 0 || n >= MAX_PATH)
        return 0;
    return WideCharToMultiByte(CP_UTF8, 0, wbuf, -1, buf, buf_size, NULL, NULL) > 0;
}

/* Tools > Terminal (see ide.c's do_open_terminal): spawns a new console
 * window running the user's shell (%COMSPEC%, falling back to cmd.exe if
 * that's unset) with its working directory set to `dir` - a detached
 * process, not something this app waits on or manages afterward, same as
 * double-clicking cmd.exe from Explorer. CreateProcessW rather than
 * system()/ShellExecute: this is a GUI-subsystem build with no console of
 * its own to inherit, and CREATE_NEW_CONSOLE gives the shell a fresh one
 * without going through a cmd.exe /c wrapper. */
void ui_open_terminal(const char *dir)
{
    WCHAR wdir[MAX_PATH] = { 0 };
    if (dir && dir[0])
        MultiByteToWideChar(CP_UTF8, 0, dir, -1, wdir, MAX_PATH);

    WCHAR shell[MAX_PATH];
    DWORD n = GetEnvironmentVariableW(L"COMSPEC", shell, MAX_PATH);
    if (n == 0 || n >= MAX_PATH)
        wcscpy(shell, L"cmd.exe");

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof si);
    si.cb = sizeof si;
    ZeroMemory(&pi, sizeof pi);

    if (CreateProcessW(NULL, shell, NULL, NULL, FALSE, CREATE_NEW_CONSOLE,
                        NULL, wdir[0] ? wdir : NULL, &si, &pi))
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

int WINAPI wWinMain(HINSTANCE inst, HINSTANCE prev, PWSTR cmd, int show)
{
    (void)prev; (void)cmd;

    /* Build a plain UTF-8 argc/argv for app_main() - GetCommandLineW() (not
     * the `cmd` parameter above, which omits argv[0] and isn't split) is the
     * standard way to recover the real, quote-aware argument list a wWinMain
     * app doesn't otherwise get. */
    {
        int wargc = 0;
        LPWSTR* wargv = CommandLineToArgvW(GetCommandLineW(), &wargc);
        if (wargv) {
            g_argv = malloc(sizeof(char*) * (size_t)wargc);
            if (g_argv) {
                g_argc = wargc;
                for (int i = 0; i < wargc; i++) {
                    int len = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, NULL, 0, NULL, NULL);
                    g_argv[i] = len > 0 ? malloc((size_t)len) : NULL;
                    if (g_argv[i])
                        WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, g_argv[i], len, NULL, NULL);
                }
            }
            LocalFree(wargv);
        }
    }

    enable_dpi_awareness();
    load_alpha_blend();

    WNDCLASSW wc = {0};
    wc.style = CS_DBLCLKS;  /* so WM_LBUTTONDBLCLK actually fires */
    wc.lpfnWndProc = wndproc;
    wc.hInstance = inst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);      // Normal arrow
    wc.lpszClassName = L"TuiTextBufWindow";
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowW(wc.lpszClassName, L"textbuf - graphical test",
                              WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                              720, 460, NULL, NULL, inst, NULL);

    load_dwm();
    sync_titlebar_color(hwnd, ui_get_theme()->menu_bg);
    remove_icon_and_caption(hwnd);

    /* WM_CREATE (run synchronously above) already set g_cell_w/h; snap the
     * window to an exact ui_default_cols x ui_default_rows client area
     * before the first paint, so it never starts with a partial row/column. */
    resize_window_to_grid(hwnd, ui_default_cols, ui_default_rows);
    ShowWindow(hwnd, show);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}