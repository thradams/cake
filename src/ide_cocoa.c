/* cocoa.c - macOS Cocoa native graphical backend.
 *
 * Manages the window, translates Cocoa events into ui_env events, renders by
 * calling app_render(), which in turn calls ui_screen_render() -> the
 * ui_draw_char()/ui_draw_box() functions implemented below with
 * CoreGraphics/CoreText. Completely generic - calls into the app via an
 * abstract interface (app_init, app_frame, app_render). Does NOT know what
 * the app does, or anything about its DOM/ui_screen. Mirrors win32.c/x11.c's
 * structure closely; see those files' comments for the reasoning behind
 * most design choices here (the shadow's half-cell pieces, the box-drawing
 * codepoint range, etc.) - only Cocoa-specific differences are called out
 * below.
 *
 * Pure C11 - no Objective-C source, no ARC. AppKit/Foundation only expose
 * Objective-C classes, so those are driven through the objc runtime
 * (objc_msgSend) by hand; CoreGraphics/CoreText/CoreFoundation are plain C
 * APIs and used directly. Since nothing here is ARC-managed, every Cocoa
 * object this file itself creates (CF "Create" results, one-time class/
 * window/view/delegate setup) is intentionally never released - it all
 * lives exactly as long as the process does, the same one-time-allocation
 * approach win32.c/x11.c take for their own long-lived resources.
 *
 * Dead-key/diacritic composition (e.g. Portuguese ^+e -> ê, "+u -> ü) goes
 * through the real NSTextInputClient path: keyDown: calls -interpretKeyEvents:
 * for anything that isn't one of our own special keys, and the small
 * NSTextInputClient shim below (view_insertText/view_setMarkedText/etc.,
 * registered on TuiView in register_classes) receives the composed result
 * from AppKit's input context. There's no real backing text store here
 * (ui.c's <input>/<editor> widgets own the actual buffer), so the marked-
 * text/selection/candidate-window queries are approximate stand-ins - real
 * commits via -insertText:replacementRange: are what matters and are
 * handled precisely; full CJK-style multi-candidate IME composition is not
 * the target of this shim, just correct simple composition (Latin dead
 * keys, accents, etc.).
 *
 * Build: gcc -std=c11 -Wall -Wextra ui.c turboc_demo.c cocoa.c -o demo_tc \
 *            -framework Cocoa -framework CoreText -framework CoreGraphics -lobjc
 */
#include "ide_ui.h"

#include <objc/runtime.h>
#include <objc/message.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <CoreText/CoreText.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <unistd.h>


typedef struct objc_object *id;
typedef struct objc_selector *SEL;
/* NSObject's BOOL is a 1-byte signed char on 64-bit Apple platforms (NOT a
 * 4-byte int like Win32's BOOL) - matching this exactly matters for
 * objc_msgSend's implicit calling-convention cast to line up correctly. */
//typedef signed char BOOL;
//#define YES ((BOOL)1)
//#define NO ((BOOL)0)

/* NSRect/NSPoint/NSSize are literally typedef'd to CGRect/CGPoint/CGSize on
 * 64-bit Apple platforms - using the CoreGraphics types directly for both
 * CoreGraphics calls AND Cocoa method arguments/returns is correct, not an
 * approximation. */

static id cls(const char *name) { return (id)objc_getClass(name); }
static SEL sel(const char *name) { return sel_registerName(name); }

/* Note for future additions: any method called here that RETURNS a struct
 * bigger than 16 bytes (e.g. -frame/-bounds, which return NSRect - 4
 * doubles) needs the separate objc_msgSend_stret entry point on Intel Macs
 * instead of plain objc_msgSend; Apple's arm64 ABI unified this so plain
 * objc_msgSend handles every return type there. NSPoint/NSSize/CGSize's 2
 * doubles fit the ordinary register-return convention everywhere, so those
 * never need this - but -frame (NSRect) does; see msgSend_getFrame below,
 * the one place this file actually calls it. */
static CGRect msgSend_getFrame(id self)
{
#if defined(__x86_64__)
    CGRect r;
    ((void (*)(CGRect *, id, SEL))objc_msgSend_stret)(&r, self, sel("frame"));
    return r;
#else
    return ((CGRect (*)(id, SEL))objc_msgSend)(self, sel("frame"));
#endif
}

/* The window frame's fixed pixel overhead beyond its content view's own
 * size - for this window's style mask (Titled|Closable|Miniaturizable|
 * Resizable, no toolbar) that's just the title bar height, i.e.
 * (extra_w=0, extra_h=titlebar height). Measured once at startup (see
 * main()), not per resize - it doesn't change while the style mask/toolbar
 * don't - and used by delegate_windowWillResize to convert between the
 * frame size AppKit proposes and the content size that must land on a
 * whole number of cells. */
static double g_frame_extra_w, g_frame_extra_h;

/* Private CoreGraphics SPI: exported by the framework but not declared in
 * any public header (CGContext.h has no prototype for it) - same situation
 * as NSPasteboardTypeString further below, needing our own extern. Used by
 * AppKit/WebKit internally for exactly this purpose (tuning font-smoothing
 * strength against a known background), stable across macOS releases.
 * Takes a CGColorRef, not raw component floats - getting this wrong (as an
 * earlier pass here did) mismatches the real call ABI and crashes. */
extern void CGContextSetFontSmoothingBackgroundColor(CGContextRef ctx, CGColorRef color);

static ui_env *g_env;
static int g_app_initialized = 0;

/* argc/argv for app_main() - stashed here in main() (a plain C entry point,
 * so it gets a real argc/argv same as any other Unix program) since app_init
 * itself only actually runs later, from delegate_timerFired's first tick. */
static int g_argc = 0;
static char** g_argv = NULL;

/* Render throttling: render_frame() walks the whole DOM every time it's
 * called (to build ui.c's next-frame cell buffer; ui_screen_render then diffs
 * it and only redraws/blits changed cells), so it should only run when
 * something visible could have changed. Real input (mouse/key/resize) sets
 * g_force_render immediately for a same-tick-latency redraw; otherwise a
 * render still happens periodically at RENDER_BASELINE_INTERVAL just to keep
 * the blinking cursor (the one thing that changes with no input at all)
 * animating - see delegate_timerFired. */
static int g_force_render = 1;  /* start true: need the very first frame */
static double g_last_render_time = 0;
#define RENDER_BASELINE_INTERVAL 0.15  /* seconds - plenty fine-grained for a
                                        * ~500ms cursor blink period, far
                                        * cheaper than redrawing every tick */

/* CFAbsoluteTimeGetCurrent() returns seconds since 2001 - multiplied by
 * 1000 that's ~8e11 today, far past UINT_MAX (~4.3e9, ~49.7 days of ms).
 * Casting an out-of-range double to unsigned is undefined behavior (unlike
 * int-to-int truncation, it does NOT reliably wrap), so time_ms would never
 * actually advance in practice - the exact "backend never sets time_ms"
 * case ui.c's time_ms comment warns leaves the caret permanently solid.
 * Fix: measure elapsed time from a process-start epoch instead, same idea
 * as win32.c's GetTickCount() (ms since boot, naturally small) - this
 * keeps the value well within unsigned range for any realistic session
 * length. Set once in main() before the timer starts. */
static double g_time_epoch;

static id g_app, g_window, g_view;
static Class g_view_class, g_delegate_class;

static CGColorSpaceRef g_colorspace;
static CTFontRef g_font;      /* smoothed - used for everything except box-drawing */
static CGFloat g_font_pt = 14;
static int g_cell_w = 8;
static int g_cell_h = 16;

/* Running total of raw NSEvent deltaY not yet resolved into a full line -
 * see view_scrollWheel below. deltaY is in "lines" for a traditional wheel
 * (usually 1.0/detent, more if scroll speed/acceleration is turned up or
 * the wheel is flicked fast) but arrives as a stream of many small
 * fractional deltas per swipe for trackpads/continuous-scroll devices;
 * this carries whichever fraction of a line hasn't been consumed yet
 * forward across events instead of discarding it. */
static double g_wheel_accum_y = 0.0;


/* Single persistent off-screen bitmap: cleared and fully redrawn (by
 * walking the app's DOM) every frame, then blitted once per timer tick -
 * same plain double-buffering scheme as win32.c/x11.c. Flipped once at
 * creation (see ensure_bitmap) so every draw callback can use ordinary
 * top-left-origin, Y-down cell coordinates. */
static void *g_bitmap_data;
static CGContextRef g_bitmap_ctx;
static int g_bitmap_w, g_bitmap_h;              /* pixel (backing-store) size */
static int g_bitmap_point_w, g_bitmap_point_h;   /* point (view) size */

/* Retina/HiDPI: NSWindow reports how many backing pixels map to one point
 * on its current screen (1.0 on non-Retina, 2.0 on most Retina displays).
 * Rendering the bitmap at point-resolution and letting AppKit upscale the
 * final blit (the naive approach) looks visibly soft next to every other
 * Mac app - rendering at this scale and letting CoreGraphics downscale-fit
 * back into point-sized geometry keeps text crisp. Queried fresh on each
 * resize rather than cached once, since dragging the window to a display
 * with a different scale changes it (there is no live re-render if the
 * window moves screens without also resizing - a known gap, not handled
 * here via NSWindowDidChangeBackingPropertiesNotification for simplicity). */
static double window_backing_scale(void)
{
    if (!g_window)
        return 1.0;
    return (double)((CGFloat (*)(id, SEL))objc_msgSend)(g_window, sel("backingScaleFactor"));
}

/* --- Small local helpers --- */

static int utf8_decode_first(const unsigned char *s, int len)
{
    if (len <= 0)
        return 0;
    unsigned char c = s[0];
    if (c < 0x80)
        return c;
    if ((c & 0xE0) == 0xC0 && len >= 2)
        return ((c & 0x1F) << 6) | (s[1] & 0x3F);
    if ((c & 0xF0) == 0xE0 && len >= 3)
        return ((c & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
    if ((c & 0xF8) == 0xF0 && len >= 4)
        return ((c & 0x07) << 18) | ((s[1] & 0x3F) << 12) | ((s[2] & 0x3F) << 6) | (s[3] & 0x3F);
    return c;
}

/* Encode a code point as UTF-16 into out (1 or 2 units, for CoreText's
 * UniChar-based glyph lookup). Returns unit count. */
static int cp_to_utf16(uint32_t cp, UniChar out[2])
{
    if (cp > 0xFFFF) {
        cp -= 0x10000;
        out[0] = (UniChar)(0xD800 + (cp >> 10));
        out[1] = (UniChar)(0xDC00 + (cp & 0x3FF));
        return 2;
    }
    out[0] = (UniChar)cp;
    return 1;
}

static CFStringRef cfstr(const char *utf8)
{
    return CFStringCreateWithCString(kCFAllocatorDefault, utf8, kCFStringEncodingUTF8);
}

/* Tint the OS title bar to match the app's own menu-bar color (the live
 * ui_theme's menu_bg, not a fixed one - so switching themes re-tints the OS
 * chrome too, not just what's drawn inside the client area) - the Cocoa
 * equivalent of ide_win32.c's DWM-based sync_titlebar_color.
 *
 * -setTitlebarAppearsTransparent: makes the title bar strip render using the
 * window's own -backgroundColor instead of AppKit's default light/dark
 * gradient; the content view's frame/geometry is untouched by this (unlike
 * NSFullSizeContentView, which this file deliberately never adds to
 * style_mask - see main()) - only the title bar sliver above the content
 * view changes. Also swaps the window's -appearance between Aqua/Dark Aqua
 * by the new color's luminance, so the traffic-light buttons and any
 * system-drawn hairline stay legible against it either way.
 *
 * Called every frame from render_frame() but a no-op past the first
 * whenever the color hasn't actually changed, same caching as Win32's
 * version - a theme switch is the only time it does real work. */
static uint32_t g_titlebar_rgb = 0xFFFFFFFFu;  /* sentinel - forces the first apply */

/* NSAppearanceNameAqua/NSAppearanceNameDarkAqua - real exported NSString*
 * constant symbols from AppKit (present since 10.9/10.14); declared
 * ourselves rather than including AppKit's (Objective-C, non-C-compatible)
 * header - same reasoning as NSPasteboardTypeString further below. Their
 * actual string content isn't something to guess at (it doesn't necessarily
 * match the symbol name), so this is the only safe way to pass one to
 * -appearanceNamed:. */
extern id NSAppearanceNameAqua;
extern id NSAppearanceNameDarkAqua;

static void sync_titlebar_color(id window, uint32_t rgb)
{
    if (rgb == g_titlebar_rgb)
        return;
    g_titlebar_rgb = rgb;

    ((void (*)(id, SEL, BOOL))objc_msgSend)(window, sel("setTitlebarAppearsTransparent:"), YES);

    double r = ((rgb >> 16) & 0xFF) / 255.0;
    double g = ((rgb >> 8) & 0xFF) / 255.0;
    double b = (rgb & 0xFF) / 255.0;
    id color = ((id (*)(id, SEL, double, double, double, double))objc_msgSend)(
                   (id)cls("NSColor"), sel("colorWithSRGBRed:green:blue:alpha:"), r, g, b, 1.0);
    ((void (*)(id, SEL, id))objc_msgSend)(window, sel("setBackgroundColor:"), color);

    double luma = 0.2126 * r + 0.7152 * g + 0.0722 * b;
    id appearance_name = luma < 0.5 ? NSAppearanceNameDarkAqua : NSAppearanceNameAqua;
    id appearance = ((id (*)(id, SEL, id))objc_msgSend)((id)cls("NSAppearance"),
                         sel("appearanceNamed:"), appearance_name);
    ((void (*)(id, SEL, id))objc_msgSend)(window, sel("setAppearance:"), appearance);
}

/* --- Font --- */

/* Unicode Box Drawing (U+2500-257F) and Block Elements (U+2580-259F) -
 * rendered with smoothing/antialiasing turned off (see ui_draw_char), same
 * fix as win32.c's ClearType/ NONANTIALIASED_QUALITY split and x11.c's
 * antialiased/non-antialiased Xft font split, and for the same root cause:
 * subpixel-smoothed glyph positioning doesn't land consistently between
 * independently-drawn cells, leaving visible seams/gaps in border lines
 * that a single continuous smoothed text run doesn't show. */
static int is_box_drawing(uint32_t ch)
{
    return ch >= 0x2500 && ch <= 0x259F;
}

/* Preference order: Menlo (Apple's own metrics-compatible successor to
 * Monaco, ships on every Mac since 10.7, excellent Unicode/box-drawing
 * coverage) > Monaco (older Macs) > Courier New (always present, final
 * fallback). */
static CTFontRef pick_and_open_font(CGFloat pt)
{
    static const char *candidates[] = { "Menlo", "Monaco", "Courier New" };
    for (size_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]); i++) {
        CFStringRef name = cfstr(candidates[i]);
        CTFontRef font = CTFontCreateWithName(name, pt, NULL);
        CFRelease(name);
        if (font)
            return font;
    }
    return NULL;
}

/* (Re)load g_font at g_font_pt and derive g_cell_w/h from its metrics.
 * Called at startup and again from Ctrl+/Ctrl- to change size. */
static void apply_font(void)
{
    CTFontRef new_font = pick_and_open_font(g_font_pt);
    if (!new_font)
        return;  /* keep whatever font (if any) we already had */

    if (g_font)
        CFRelease(g_font);
    g_font = new_font;

    /* Monospace cell size: 'M's advance for width, the font's own vertical
     * metrics for height (ascent+descent+leading, like a terminal line). */
    UniChar ch = 'M';
    CGGlyph glyph = 0;
    CTFontGetGlyphsForCharacters(g_font, &ch, &glyph, 1);
    CGSize advance = { 0, 0 };
    if (glyph)
        CTFontGetAdvancesForGlyphs(g_font, kCTFontOrientationDefault, &glyph, &advance, 1);
    g_cell_w = advance.width > 0 ? (int)(advance.width + 0.5) : (int)(g_font_pt * 0.6 + 0.5);
    g_cell_h = (int)(CTFontGetAscent(g_font) + CTFontGetDescent(g_font) +
                      CTFontGetLeading(g_font) + 0.5);

    /* Every cell's pixel size just changed, so the render shadow no longer
     * matches the bitmap - clear it and repaint fully next frame. (Usually
     * followed by a resize -> ensure_bitmap, but this covers the case the
     * pixel size is unchanged.) menu_bg (rather than a hardcoded color) so
     * any sliver the cell grid doesn't exactly cover blends with the menu
     * bar instead of standing out - see ensure_bitmap's own fill below for
     * why that sliver exists at all. */
    if (g_bitmap_ctx) {
        uint32_t bg = ui_get_theme()->menu_bg;
        CGContextSetRGBFillColor(g_bitmap_ctx, ((bg >> 16) & 0xFF) / 255.0,
                                  ((bg >> 8) & 0xFF) / 255.0, (bg & 0xFF) / 255.0, 1.0);
        CGContextFillRect(g_bitmap_ctx, CGRectMake(0, 0, g_bitmap_point_w, g_bitmap_point_h));
    }
    app_invalidate();
}

/* --- Bitmap back buffer --- */

static void ensure_bitmap(int w, int h)
{
    if (w < 1) w = 1;
    if (h < 1) h = 1;
    if (g_bitmap_ctx && w == g_bitmap_point_w && h == g_bitmap_point_h)
        return;

    double scale = window_backing_scale();
    int pw = (int)(w * scale + 0.5);
    int ph = (int)(h * scale + 0.5);
    if (pw < 1) pw = 1;
    if (ph < 1) ph = 1;

    if (g_bitmap_ctx) { CGContextRelease(g_bitmap_ctx); g_bitmap_ctx = NULL; }
    free(g_bitmap_data);

    g_bitmap_data = malloc((size_t)pw * (size_t)ph * 4);
    g_bitmap_ctx = CGBitmapContextCreate(g_bitmap_data, (size_t)pw, (size_t)ph, 8, (size_t)pw * 4,
                                          g_colorspace,
                                          kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Host);
    /* Flip once, right here - every draw callback below then uses ordinary
     * top-left-origin, Y-down cell coordinates for the rest of this
     * context's lifetime (the CTM persists across draw calls; re-flipping
     * per frame would stack incorrectly). Then scale by the backing factor
     * so ui_draw_char/etc keep using plain point-based cell coordinates
     * (px = x * g_cell_w) while the context itself has pw x ph real pixels
     * to work with - this is what actually produces the sharper output. */
    if (g_bitmap_ctx) {
        CGContextTranslateCTM(g_bitmap_ctx, 0, ph);
        CGContextScaleCTM(g_bitmap_ctx, 1, -1);
        CGContextScaleCTM(g_bitmap_ctx, scale, scale);
    }

    g_bitmap_w = pw;
    g_bitmap_h = ph;
    g_bitmap_point_w = w;
    g_bitmap_point_h = h;

    /* Fresh (malloc'd, uninitialized) bitmap: give it a known background once,
     * never per frame, so cells the incremental renderer skips keep their
     * pixels; then discard the render shadow so the next frame repaints fully.
     * menu_bg (rather than a hardcoded color) so a maximize/resize that
     * doesn't land on an exact multiple of the cell size leaves its leftover
     * sliver blending with the menu bar instead of a mismatched gap. */
    if (g_bitmap_ctx) {
        uint32_t bg = ui_get_theme()->menu_bg;
        CGContextSetRGBFillColor(g_bitmap_ctx, ((bg >> 16) & 0xFF) / 255.0,
                                  ((bg >> 8) & 0xFF) / 255.0, (bg & 0xFF) / 255.0, 1.0);
        CGContextFillRect(g_bitmap_ctx, CGRectMake(0, 0, g_bitmap_point_w, g_bitmap_point_h));
    }
    app_invalidate();
}

/* --- ui_draw_char/ui_draw_box (see ui.h) --- */

void ui_draw_char(int x, int y, uint32_t ch, uint32_t fg, uint32_t bg)
{
    CGContextRef c = g_bitmap_ctx;
    int px = x * g_cell_w;
    int py = y * g_cell_h;

    CGContextSetRGBFillColor(c, ((bg >> 16) & 0xFF) / 255.0, ((bg >> 8) & 0xFF) / 255.0,
                              (bg & 0xFF) / 255.0, 1.0);
    CGContextFillRect(c, CGRectMake(px, py, g_cell_w, g_cell_h));

    uint32_t cp = ch ? ch : ' ';
    UniChar units[2];
    int n = cp_to_utf16(cp, units);
    CGGlyph glyphs[2] = {0, 0};
    if (!CTFontGetGlyphsForCharacters(g_font, units, glyphs, n) || glyphs[0] == 0)
        return;  /* no glyph for this char in the font - skip rather than draw a tofu box */

    CGFloat ascent = CTFontGetAscent(g_font);
    int box = is_box_drawing(cp);

    CGContextSaveGState(c);
    /* Clip the glyph to its own cell: an overhang (smoothing fringe, or a
     * glyph wider than the monospace cell) would otherwise spill into the
     * neighbour, and since cells are drawn independently and only changed ones
     * repaint, that spill is never cleaned when the neighbour doesn't itself
     * repaint - e.g. editor text bleeding into the static blank scrollbar
     * column, leaving stale fringe along the track as the view scrolls. */
    CGContextClipToRect(c, CGRectMake(px, py, g_cell_w, g_cell_h));
    CGContextSetShouldAntialias(c, !box);
    CGContextSetShouldSmoothFonts(c, !box);
    /* Font smoothing blends glyph edges against an assumed background;
     * telling it the real per-cell bg (rather than leaving it at whatever
     * default) avoids fringing/muddiness on this app's dark cell colors.
     * (Since macOS 10.14, CoreGraphics dropped true subpixel-color AA in
     * favor of grayscale AA + stem darkening, so this now mostly tunes how
     * aggressive that darkening is - still worth setting, just less
     * dramatic an effect than on older macOS.) Cached by bg value - this
     * runs per glyph at a high frame rate, and consecutive cells usually
     * share a background, so skip the alloc/release when unchanged. */
    static CGColorRef s_smoothing_bg_color;
    static uint32_t s_smoothing_bg_value = 0xFFFFFFFFu; /* sentinel: never a valid 24-bit bg */
    if (!s_smoothing_bg_color || bg != s_smoothing_bg_value) {
        if (s_smoothing_bg_color)
            CGColorRelease(s_smoothing_bg_color);
        s_smoothing_bg_color = CGColorCreateGenericRGB(((bg >> 16) & 0xFF) / 255.0,
                                                        ((bg >> 8) & 0xFF) / 255.0,
                                                        (bg & 0xFF) / 255.0, 1.0);
        s_smoothing_bg_value = bg;
    }
    CGContextSetFontSmoothingBackgroundColor(c, s_smoothing_bg_color);
    /* Snap glyph origins to whole pixels rather than subpixel-positioning
     * them - for a monospace grid this keeps every cell's glyph aligned
     * the same way instead of letting hinting drift cell-to-cell, the same
     * spirit as the box-drawing antialiasing split above. */
    CGContextSetShouldSubpixelPositionFonts(c, false);
    CGContextSetShouldSubpixelQuantizeFonts(c, false);
    CGContextSetRGBFillColor(c, ((fg >> 16) & 0xFF) / 255.0, ((fg >> 8) & 0xFF) / 255.0,
                              (fg & 0xFF) / 255.0, 1.0);

    /* CTFontDrawGlyphs expects bottom-up (Y-increases-upward) text space,
     * but this whole context is permanently flipped top-down (see
     * ensure_bitmap) - counter-flip locally just for this glyph, around
     * the point where its baseline belongs in the outer (flipped) space,
     * the standard fix for "text draws upside down in a flipped CGContext". */
    CGContextTranslateCTM(c, (CGFloat)px, (CGFloat)py + ascent);
    CGContextScaleCTM(c, 1, -1);
    CGPoint pos = { 0, 0 };
    CTFontDrawGlyphs(g_font, glyphs, &pos, 1, c);

    CGContextRestoreGState(c);
}

/* ui_draw_box (see ui.h): a solid w x h rect - ui.c's replacement for
 * building the same fill out of a per-cell ui_draw_char loop. Every current
 * caller (see draw_fill in ui.c) fills with a blank cell, so there's no
 * glyph to render at all - a normal fill (flags == 0) is just one
 * CGContextFillRect over the whole rect, no font/text machinery involved;
 * `fg` is unused.
 *
 * UI_BOX_INVERT: true XOR-style invert. Filling with white under the
 * "difference" blend mode computes dst' = |1 - dst| per channel, i.e. an
 * exact per-channel invert - CoreGraphics' equivalent of Win32's
 * PatBlt(DSTINVERT) or X11's GXinvert, needing no color of its own to be
 * legible over anything.
 *
 * UI_BOX_SHADOW: darken the rect via true alpha blending - unlike
 * win32.c/x11.c, no special API needed for this: CGContextFillRect with an
 * alpha<1 color already blends against whatever's underneath natively. */
void ui_draw_box(int x, int y, int w, int h, uint32_t fg, uint32_t bg, int flags)
{
    CGContextRef c = g_bitmap_ctx;
    (void)fg;

    if (flags & UI_BOX_INVERT) {
        CGContextSaveGState(c);
        CGContextSetBlendMode(c, kCGBlendModeDifference);
        CGContextSetRGBFillColor(c, 1, 1, 1, 1);
        CGContextFillRect(c, CGRectMake(x * g_cell_w, y * g_cell_h, w * g_cell_w, h * g_cell_h));
        CGContextRestoreGState(c);
        return;
    }
    if (flags & UI_BOX_SHADOW) {
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
        CGContextSaveGState(c);
        CGContextSetRGBFillColor(c, 0, 0, 0, 110.0 / 255.0);  /* ~43% opacity black */
        CGContextFillRect(c, CGRectMake(px, py, pw, ph));
        CGContextRestoreGState(c);
        return;
    }

    CGContextSetRGBFillColor(c, ((bg >> 16) & 0xFF) / 255.0, ((bg >> 8) & 0xFF) / 255.0,
                              (bg & 0xFF) / 255.0, 1.0);
    CGContextFillRect(c, CGRectMake(x * g_cell_w, y * g_cell_h, w * g_cell_w, h * g_cell_h));
}

/* Have the app redraw whatever changed into the persistent bitmap (never
 * cleared here - only on (re)creation, see ensure_bitmap - so skipped cells
 * keep last frame), then invalidate ONLY the changed region so view_drawRect
 * recomposites just that. Nothing changed -> no redraw, no blit.
 *
 * The rect is in the flipped view's point coords, into which cell coords map
 * straight through (see view_isFlipped). It's cell-tight - glyphs are clipped
 * to their cells (see ui_draw_char's CGContextClipToRect), so nothing spills
 * past a cell that would need an extra margin here. */
static void render_frame(void)
{
    if (!g_bitmap_ctx)
        return;

    sync_titlebar_color(g_window, ui_get_theme()->menu_bg);

    int dx, dy, dw, dh;
    if (!app_render(&dx, &dy, &dw, &dh))
        return;

    CGRect r = CGRectMake(dx * g_cell_w, dy * g_cell_h, dw * g_cell_w, dh * g_cell_h);
    ((void (*)(id, SEL, CGRect))objc_msgSend)(g_view, sel("setNeedsDisplayInRect:"), r);
}

/* --- Key mapping (standard Mac ANSI USB keyboard virtual keycodes) --- */

#define KC_RETURN 36
#define KC_TAB 48
#define KC_ESCAPE 53
#define KC_LEFT 123
#define KC_RIGHT 124
#define KC_DOWN 125
#define KC_UP 126
#define KC_HOME 115
#define KC_END 119
#define KC_PAGEUP 116
#define KC_PAGEDOWN 121
#define KC_FORWARDDELETE 117
#define KC_HELP_INSERT 114
#define KC_F1 122
#define KC_F2 120
#define KC_F3 99
#define KC_F4 118
#define KC_F5 96
#define KC_F6 97
#define KC_F7 98
#define KC_F8 100
#define KC_F9 101
#define KC_F10 109
#define KC_F11 103
#define KC_F12 111
#define KC_EQUAL 24    /* also "+" un-shifted-adjacent key - Ctrl+= grows */
#define KC_MINUS 27
#define KC_KP_PLUS 69
#define KC_BACKSPACE 51        /* the Backspace/Delete key (not Fn+Delete) */
#define KC_KEYPAD_ENTER 76

static int keycode_to_uikey(unsigned short kc)
{
    switch (kc) {
    case KC_UP:            return UI_KEY_UP;
    case KC_DOWN:          return UI_KEY_DOWN;
    case KC_LEFT:          return UI_KEY_LEFT;
    case KC_RIGHT:         return UI_KEY_RIGHT;
    case KC_HOME:          return UI_KEY_HOME;
    case KC_END:           return UI_KEY_END;
    case KC_PAGEUP:        return UI_KEY_PAGEUP;
    case KC_PAGEDOWN:      return UI_KEY_PAGEDOWN;
    case KC_FORWARDDELETE: return UI_KEY_DELETE;
    case KC_HELP_INSERT:   return UI_KEY_INSERT;
    case KC_ESCAPE:        return UI_KEY_ESCAPE;
    case KC_TAB:           return UI_KEY_TAB;
    case KC_F1:  return UI_KEY_F1;  case KC_F2:  return UI_KEY_F2;
    case KC_F3:  return UI_KEY_F3;  case KC_F4:  return UI_KEY_F4;
    case KC_F5:  return UI_KEY_F5;  case KC_F6:  return UI_KEY_F6;
    case KC_F7:  return UI_KEY_F7;  case KC_F8:  return UI_KEY_F8;
    case KC_F9:  return UI_KEY_F9;  case KC_F10: return UI_KEY_F10;
    case KC_F11: return UI_KEY_F11; case KC_F12: return UI_KEY_F12;
    default: return 0;
    }
}

/* NSEventModifierFlags bit positions (stable, documented AppKit constants). */
#define NSEVENT_MOD_SHIFT   (1UL << 17)
#define NSEVENT_MOD_CONTROL (1UL << 18)
#define NSEVENT_MOD_OPTION  (1UL << 19)
#define NSEVENT_MOD_COMMAND (1UL << 20)

static int mac_mods(id event)
{
    unsigned long flags = ((unsigned long (*)(id, SEL))objc_msgSend)(event, sel("modifierFlags"));
    return (flags & NSEVENT_MOD_SHIFT ? UI_MOD_SHIFT : 0) |
           (flags & NSEVENT_MOD_CONTROL ? UI_MOD_CTRL : 0) |
           (flags & NSEVENT_MOD_OPTION ? UI_MOD_ALT : 0) |
           (flags & NSEVENT_MOD_COMMAND ? UI_MOD_CMD : 0);
}

/* Change the font size by `delta` points, clamped to a sane range, and
 * resize the window to keep the same column/row count at the new cell
 * size. Registered with the ui_env as its font-zoom callback (see
 * ui_env_set_font_zoom_fn in ui.h) - invoked uniformly via
 * ui_env_adjust_font_size(), whether the trigger is Ctrl+/Ctrl- or the app's
 * Window > Font Size +/- menu items. `ctx` is unused - this backend keeps
 * everything it needs in globals (g_window/g_env). */
static void adjust_font_size(void *ctx, int delta)
{
    (void)ctx;
    int new_pt = (int)g_font_pt + delta;
    if (new_pt < 6) new_pt = 6;
    if (new_pt > 36) new_pt = 36;
    if (new_pt == (int)g_font_pt)
        return;

    int cols = ui_env_width(g_env);
    int rows = ui_env_height(g_env);
    g_font_pt = new_pt;
    apply_font();

    /* -setContentSize: resizes the window so its CONTENT area (excluding
     * the title bar) becomes this size - exactly what we want, without
     * having to account for the title bar's height ourselves. */
    CGSize size = { (CGFloat)(cols * g_cell_w), (CGFloat)(rows * g_cell_h) };
    ((void (*)(id, SEL, CGSize))objc_msgSend)(g_window, sel("setContentSize:"), size);
}

/* --- Custom NSView subclass: drawing + input --- */

static CGPoint view_local_point(id view, id event)
{
    CGPoint win_pt = ((CGPoint (*)(id, SEL))objc_msgSend)(event, sel("locationInWindow"));
    return ((CGPoint (*)(id, SEL, CGPoint, id))objc_msgSend)(view, sel("convertPoint:fromView:"),
                                                              win_pt, (id)0);
}

static void view_drawRect(id self, SEL _cmd, CGRect dirty)
{
    (void)self; (void)_cmd;
    if (!g_bitmap_ctx)
        return;

    id ns_ctx = ((id (*)(id, SEL))objc_msgSend)(cls("NSGraphicsContext"), sel("currentContext"));
    CGContextRef ctx = (CGContextRef)((id (*)(id, SEL))objc_msgSend)(ns_ctx, sel("CGContext"));
    if (!ctx)
        return;

    CGImageRef img = CGBitmapContextCreateImage(g_bitmap_ctx);
    if (!img)
        return;

    /* Composite only the invalidated region (see render_frame's
     * setNeedsDisplayInRect). `dirty` is in the view's flipped point space -
     * clip in that space BEFORE the counter-flip below. AppKit may pass a
     * larger dirty rect (e.g. the whole bounds on first draw or after an
     * occlusion change); clipping to it is always safe. */
    CGContextSaveGState(ctx);
    CGContextClipToRect(ctx, dirty);

    /* CGContextDrawImage always needs the destination counter-flipped when
     * (as here) the current CTM is Y-flipped, regardless of the view's own
     * isFlipped setting - the same "upside down image" gotcha as the one
     * ui_draw_char works around for text, just for the final blit instead. */
    CGContextTranslateCTM(ctx, 0, g_bitmap_point_h);
    CGContextScaleCTM(ctx, 1, -1);
    /* Destination rect is in POINTS (g_bitmap_point_w/h), not the bitmap's
     * own pixel size (g_bitmap_w/h) - the view's context is itself already
     * scaled for the screen's backing factor, so drawing a pw x ph-pixel
     * image into point-sized geometry here is what maps it 1:1 with real
     * screen pixels on Retina instead of drawing at 1x and letting the
     * window upscale it. */
    CGContextDrawImage(ctx, CGRectMake(0, 0, g_bitmap_point_w, g_bitmap_point_h), img);
    CGContextRestoreGState(ctx);
    CGImageRelease(img);
}

static BOOL view_isFlipped(id self, SEL _cmd) { (void)self; (void)_cmd; return YES; }
static BOOL view_yes(id self, SEL _cmd) { (void)self; (void)_cmd; return YES; }

static void post_mouse(int x, int y, int button, int action, int mods, int wheel_delta)
{
    ui_event ev = {0};
    ev.type = UI_EVENT_MOUSE;
    ev.data.mouse.x = x;
    ev.data.mouse.y = y;
    ev.data.mouse.button = button;
    ev.data.mouse.action = action;
    ev.data.mouse.mods = mods;
    ev.data.mouse.wheel_delta = wheel_delta;
    ui_env_post_event(g_env, &ev);
    g_force_render = 1;  /* every mouse event type routes through here */
}

static void view_mouse_button(id self, id event, int button, int is_down)
{
    CGPoint pt = view_local_point(self, event);
    int x = (int)(pt.x / g_cell_w);
    int y = (int)(pt.y / g_cell_h);
    int mods = mac_mods(event);

    int action;
    if (!is_down) {
        action = UI_MOUSE_RELEASED;
    } else {
        long click_count = ((long (*)(id, SEL))objc_msgSend)(event, sel("clickCount"));
        action = (button == UI_MOUSE_BUTTON_LEFT && click_count >= 2) ? UI_MOUSE_DBLCLICK
                                                                       : UI_MOUSE_PRESSED;
    }
    post_mouse(x, y, button, action, mods, 0);
}

static void view_mouseDown(id self, SEL _cmd, id event)
{ (void)_cmd; view_mouse_button(self, event, UI_MOUSE_BUTTON_LEFT, 1); }
static void view_mouseUp(id self, SEL _cmd, id event)
{ (void)_cmd; view_mouse_button(self, event, UI_MOUSE_BUTTON_LEFT, 0); }
static void view_rightMouseDown(id self, SEL _cmd, id event)
{ (void)_cmd; view_mouse_button(self, event, UI_MOUSE_BUTTON_RIGHT, 1); }
static void view_rightMouseUp(id self, SEL _cmd, id event)
{ (void)_cmd; view_mouse_button(self, event, UI_MOUSE_BUTTON_RIGHT, 0); }
static void view_otherMouseDown(id self, SEL _cmd, id event)
{ (void)_cmd; view_mouse_button(self, event, UI_MOUSE_BUTTON_MIDDLE, 1); }
static void view_otherMouseUp(id self, SEL _cmd, id event)
{ (void)_cmd; view_mouse_button(self, event, UI_MOUSE_BUTTON_MIDDLE, 0); }

static void view_mouse_move(id self, id event)
{
    CGPoint pt = view_local_point(self, event);
    post_mouse((int)(pt.x / g_cell_w), (int)(pt.y / g_cell_h), 0, UI_MOUSE_MOVED, mac_mods(event), 0);
}

static void view_mouseMoved(id self, SEL _cmd, id event) { (void)_cmd; view_mouse_move(self, event); }
static void view_mouseDragged(id self, SEL _cmd, id event) { (void)_cmd; view_mouse_move(self, event); }
static void view_rightMouseDragged(id self, SEL _cmd, id event) { (void)_cmd; view_mouse_move(self, event); }

static void view_scrollWheel(id self, SEL _cmd, id event)
{
    (void)_cmd;
    double dy = ((double (*)(id, SEL))objc_msgSend)(event, sel("deltaY"));
    if (dy == 0)
        return;

    /* Snapping every nonzero delta straight to +-1 (the old behavior) loses
     * information in both directions: a fast wheel flick's larger deltaY
     * would scroll only a single line, same as the gentlest nudge, and a
     * trackpad swipe's many small fractional deltas would each jump a full
     * line instead of smoothly adding up to one - jerkier and faster than
     * the actual finger movement. Accumulate the real delta instead, and
     * only emit however many whole lines it's now worth, carrying any
     * fractional remainder forward to the next event - same as the
     * WM_MOUSEWHEEL accumulator on Windows, just with a fractional (rather
     * than integer-notch) input. */
    g_wheel_accum_y += dy;
    int lines = (int)g_wheel_accum_y;  /* truncates toward zero */
    if (lines == 0)
        return;  /* not a full line yet - saved in g_wheel_accum_y for next time */
    g_wheel_accum_y -= lines;

    CGPoint pt = view_local_point(self, event);
    post_mouse((int)(pt.x / g_cell_w), (int)(pt.y / g_cell_h), 0, UI_MOUSE_WHEEL,
               mac_mods(event), lines);
}

/* --- NSTextInputClient shim (dead-key/diacritic composition) --- */

typedef struct { unsigned long location, length; } NSRange;
#define NS_NOT_FOUND ((unsigned long)-1L)  /* matches NSNotFound on 64-bit */

/* g_marked_len>0 while a composition (e.g. after typing a dead key, before
 * its base letter arrives) is in progress. g_keydown_mods is stashed by
 * keyDown: right before calling -interpretKeyEvents: so the later
 * view_insertText callback - which AppKit calls with no modifier info of
 * its own - can still tag the resulting key event with the mods that were
 * held down. */
static char g_marked_text[64];
static int  g_marked_len = 0;
static int  g_keydown_mods = 0;

static id ns_marked_string(id string)
{
    /* Plain dead-key composition hands us an NSString; some IMEs hand us
     * an NSAttributedString instead - normalize to NSString either way. */
    if (((BOOL (*)(id, SEL, id))objc_msgSend)(string, sel("isKindOfClass:"), (id)cls("NSAttributedString")))
        return ((id (*)(id, SEL))objc_msgSend)(string, sel("string"));
    return string;
}

static void post_text_utf8(const char *utf8, int mods)
{
    int len = (int)strlen(utf8);
    int i = 0;
    while (i < len) {
        uint32_t cp = (uint32_t)utf8_decode_first((const unsigned char *)utf8 + i, len - i);
        if (cp == 0)
            break;
        if (cp == 0x7F)
            cp = 8;  /* Mac Delete/Backspace sends DEL (0x7F), not BS - normalize */
        ui_event ev = {0};
        ev.type = UI_EVENT_KEY;
        ev.data.key.codepoint = cp;
        ev.data.key.mods = mods;
        ui_env_post_event(g_env, &ev);
        unsigned char c0 = (unsigned char)utf8[i];
        int adv = (c0 < 0x80) ? 1 : ((c0 & 0xE0) == 0xC0) ? 2 : ((c0 & 0xF0) == 0xE0) ? 3 : ((c0 & 0xF8) == 0xF0) ? 4 : 1;
        i += adv;
    }
    g_force_render = 1;
}

static BOOL view_hasMarkedText(id self, SEL _cmd)
{ (void)self; (void)_cmd; return g_marked_len > 0 ? YES : NO; }

static NSRange view_markedRange(id self, SEL _cmd)
{
    (void)self; (void)_cmd;
    NSRange r;
    if (g_marked_len > 0) { r.location = 0; r.length = 1; }
    else { r.location = NS_NOT_FOUND; r.length = 0; }
    return r;
}

static NSRange view_selectedRange(id self, SEL _cmd)
{
    /* No real backing text store to report a selection from - see the
     * file header note above register_classes' TuiView setup. */
    (void)self; (void)_cmd;
    NSRange r; r.location = NS_NOT_FOUND; r.length = 0;
    return r;
}

static void view_setMarkedText(id self, SEL _cmd, id string, NSRange selRange, NSRange replRange)
{
    (void)self; (void)_cmd; (void)selRange; (void)replRange;
    id s = ns_marked_string(string);
    const char *utf8 = s ? ((const char *(*)(id, SEL))objc_msgSend)(s, sel("UTF8String")) : NULL;
    if (utf8 && utf8[0]) {
        size_t n = strlen(utf8);
        if (n >= sizeof(g_marked_text)) n = sizeof(g_marked_text) - 1;
        memcpy(g_marked_text, utf8, n);
        g_marked_text[n] = 0;
        g_marked_len = (int)n;
    } else {
        g_marked_len = 0;
    }
}

static void view_unmarkText(id self, SEL _cmd)
{ (void)self; (void)_cmd; g_marked_len = 0; }

static id view_validAttributesForMarkedText(id self, SEL _cmd)
{ (void)self; (void)_cmd; return ((id (*)(id, SEL))objc_msgSend)((id)cls("NSArray"), sel("array")); }

static id view_attributedSubstringForProposedRange(id self, SEL _cmd, NSRange range, NSRange *actual)
{
    (void)self; (void)_cmd; (void)range;
    if (actual) { actual->location = NS_NOT_FOUND; actual->length = 0; }
    return (id)0;
}

static unsigned long view_characterIndexForPoint(id self, SEL _cmd, CGPoint point)
{ (void)self; (void)_cmd; (void)point; return NS_NOT_FOUND; }

static CGRect view_firstRectForCharacterRange(id self, SEL _cmd, NSRange range, NSRange *actual)
{
    (void)self; (void)_cmd; (void)range;
    if (actual) { actual->location = NS_NOT_FOUND; actual->length = 0; }
    /* Precise caret-relative placement would mean calling -frame/-window
     * (both return a >16-byte NSRect - see the objc_msgSend_stret note
     * near the top of this file) just to position a CJK-style candidate
     * popup, which the simple Latin dead keys this shim targets don't even
     * show. A zero rect is a harmless stand-in; AppKit just anchors any
     * popup at a default screen position instead. */
    CGRect r = {{0, 0}, {0, 0}};
    return r;
}

static void view_insertText(id self, SEL _cmd, id string, NSRange replRange)
{
    (void)self; (void)_cmd; (void)replRange;
    g_marked_len = 0;  /* a commit ends any in-progress composition */
    id s = ns_marked_string(string);
    const char *utf8 = s ? ((const char *(*)(id, SEL))objc_msgSend)(s, sel("UTF8String")) : NULL;
    if (utf8 && utf8[0])
        post_text_utf8(utf8, g_keydown_mods);
}

static void view_doCommandBySelector(id self, SEL _cmd, SEL command)
{
    /* Arrows, enter, delete, tab, etc. are all handled directly in
     * keyDown: via keycode_to_uikey before -interpretKeyEvents: is ever
     * called, so nothing reaches here in normal operation - this is just
     * AppKit's required NSTextInputClient fallback hook. */
    (void)self; (void)_cmd; (void)command;
}

static void view_keyDown(id self, SEL _cmd, id event)
{
    (void)_cmd;
    unsigned short keycode = ((unsigned short (*)(id, SEL))objc_msgSend)(event, sel("keyCode"));
    int mods = mac_mods(event);

    /* Ctrl+/Ctrl- resize */    
    if ((mods & UI_MOD_CTRL) && (keycode == KC_EQUAL || keycode == KC_KP_PLUS)) {
        ui_env_adjust_font_size(g_env, 1);
        return;
    }
    if ((mods & UI_MOD_CTRL) && keycode == KC_MINUS) {
        ui_env_adjust_font_size(g_env, -1);
        return;
    }

    int code = keycode_to_uikey(keycode);
    if (code) {
        ui_event ev = {0};
        ev.type = UI_EVENT_KEY;
        ev.data.key.code = code;
        ev.data.key.mods = mods;
        ui_env_post_event(g_env, &ev);
        g_force_render = 1;
        return;
    }

    /* Return and Backspace handled directly */
    if (keycode == KC_RETURN || keycode == KC_KEYPAD_ENTER) {
        ui_event ev = {0};
        ev.type = UI_EVENT_KEY;
        ev.data.key.codepoint = '\r';
        ev.data.key.mods = mods;
        ui_env_post_event(g_env, &ev);
        g_force_render = 1;
        return;
    }
    if (keycode == KC_BACKSPACE) {
        ui_event ev = {0};
        ev.type = UI_EVENT_KEY;
        ev.data.key.codepoint = 8;
        ev.data.key.mods = mods;
        ui_env_post_event(g_env, &ev);
        g_force_render = 1;
        return;
    }

    

    /* Cmd+letter shortcuts (macOS): intercept and post as codepoint events */
    if (mods & UI_MOD_CMD) {
        id chars = ((id (*)(id, SEL))objc_msgSend)(event, sel("charactersIgnoringModifiers"));
        const char *utf8 = chars ? ((const char *(*)(id, SEL))objc_msgSend)(chars, sel("UTF8String")) : NULL;
        if (utf8 && utf8[0]) {
            uint32_t cp = utf8_decode_first((const unsigned char*)utf8, (int)strlen(utf8));
            if (cp >= 'a' && cp <= 'z')
                cp = cp - 'a' + 'A';      // convert to uppercase
            if (cp >= 'A' && cp <= 'Z') {
                ui_event ev = {0};
                ev.type = UI_EVENT_KEY;
                ev.data.key.codepoint = cp;
                ev.data.key.mods = mods;   // includes UI_MOD_CMD and possibly UI_MOD_SHIFT
                ui_env_post_event(g_env, &ev);
                g_force_render = 1;
                return;                    // do not pass to interpretKeyEvents:
            }
        }
    }

    /* Everything else (dead keys, IME, ordinary characters) goes through the input system */
    g_keydown_mods = mods;
    id events = ((id (*)(id, SEL, id))objc_msgSend)((id)cls("NSArray"), sel("arrayWithObject:"), event);
    ((void (*)(id, SEL, id))objc_msgSend)(self, sel("interpretKeyEvents:"), events);
}

static void view_setFrameSize(id self, SEL _cmd, CGSize size)
{
    /* Chain to NSView's own implementation first - it still needs to
     * actually resize the view/its layer; only the ui_env/bitmap sync
     * below is our own addition. No struct RETURN here (setFrameSize:
     * returns void), so plain objc_msgSendSuper is correct on every
     * architecture - the stret concern only applies to return values. */
    struct objc_super sup = { self, (Class)cls("NSView") };
    ((void (*)(struct objc_super *, SEL, CGSize))objc_msgSendSuper)(&sup, _cmd, size);

    int w = (int)size.width, h = (int)size.height;
    if (w < 1) w = 1;
    if (h < 1) h = 1;
    int cols = w / g_cell_w, rows = h / g_cell_h;
    if (cols < 1) cols = 1;
    if (rows < 1) rows = 1;
    if (g_env)
        ui_env_resize(g_env, cols, rows);
    ensure_bitmap(w, h);
    g_force_render = 1;
}

/* --- App delegate: the frame timer + app lifecycle --- */

static void delegate_timerFired(id self, SEL _cmd, id timer)
{
    (void)self; (void)_cmd; (void)timer;
    if (!g_app_initialized) {
        app_init(g_env);
        app_main(g_argc, g_argv);
        g_app_initialized = 1;
    }
    ui_env_set_time_ms(g_env, (unsigned)((CFAbsoluteTimeGetCurrent() - g_time_epoch) * 1000.0));  /* drives the caret blink */
    if (app_frame(g_env))
        ((void (*)(id, SEL, id))objc_msgSend)(g_app, sel("terminate:"), (id)0);

    /* app_frame() above is cheap (event drain + hit-testing) and runs every
     * tick regardless; render_frame() is the expensive full-DOM-walk part
     * and only actually needs to run when something visible changed - see
     * g_force_render's declaration for why. */
    double now = CFAbsoluteTimeGetCurrent();
    if (g_force_render || (now - g_last_render_time) >= RENDER_BASELINE_INTERVAL) {
        render_frame();
        g_force_render = 0;
        g_last_render_time = now;
    }
}

static BOOL delegate_shouldTerminateAfterLastWindowClosed(id self, SEL _cmd, id app)
{ (void)self; (void)_cmd; (void)app; return YES; }

/* Reached for every quit path Cocoa knows about (Cmd+Q, the Dock/menu bar
 * Quit item, closing the last window since
 * applicationShouldTerminateAfterLastWindowClosed above says YES) - not just
 * File > Exit's own [NSApp terminate:] call (delegate_timerFired), which
 * ends up right back here anyway. One save either way, same "harmless
 * double-save" reasoning as ide_win32.c's WM_DESTROY. */
static void delegate_applicationWillTerminate(id self, SEL _cmd, id notification)
{
    (void)self; (void)_cmd; (void)notification;
    if (g_app_initialized)
        app_shutdown();
}

/* Round the frame size AppKit is about to apply to the nearest whole number
 * of cells, the same way ide_win32.c's WM_SIZING handler snaps the drag
 * rectangle - without this, macOS (unlike Windows) lets a window settle on
 * any pixel size, so a manual resize almost never lands on an exact
 * multiple of the cell size and the leftover sliver (painted menu_bg - see
 * ensure_bitmap) shows for the whole drag instead of just briefly at the
 * extremes. AppKit calls this for every frame-size change it's proposing,
 * including Zoom/the green button, so that gets snapped too; the menu_bg
 * fill in ensure_bitmap stays as a fallback for any resize path that
 * doesn't go through here (e.g. programmatic ones that skip the delegate). */
static CGSize delegate_windowWillResize(id self, SEL _cmd, id sender, CGSize proposed)
{
    (void)self; (void)_cmd; (void)sender;
    double content_w = proposed.width - g_frame_extra_w;
    double content_h = proposed.height - g_frame_extra_h;
    int cols = (int)((content_w + g_cell_w / 2.0) / g_cell_w);
    int rows = (int)((content_h + g_cell_h / 2.0) / g_cell_h);
    if (cols < 1) cols = 1;
    if (rows < 1) rows = 1;
    CGSize snapped;
    snapped.width = cols * g_cell_w + g_frame_extra_w;
    snapped.height = rows * g_cell_h + g_frame_extra_h;
    return snapped;
}

/* --- Runtime class setup --- */

static void register_classes(void)
{
    g_view_class = objc_allocateClassPair((Class)cls("NSView"), "TuiView", 0);
    class_addMethod(g_view_class, sel("drawRect:"), (IMP)view_drawRect,
                     "v@:{CGRect={CGPoint=dd}{CGSize=dd}}");
    class_addMethod(g_view_class, sel("isFlipped"), (IMP)view_isFlipped, "c@:");
    class_addMethod(g_view_class, sel("acceptsFirstResponder"), (IMP)view_yes, "c@:");
    class_addMethod(g_view_class, sel("mouseDown:"), (IMP)view_mouseDown, "v@:@");
    class_addMethod(g_view_class, sel("mouseUp:"), (IMP)view_mouseUp, "v@:@");
    class_addMethod(g_view_class, sel("mouseDragged:"), (IMP)view_mouseDragged, "v@:@");
    class_addMethod(g_view_class, sel("mouseMoved:"), (IMP)view_mouseMoved, "v@:@");
    class_addMethod(g_view_class, sel("rightMouseDown:"), (IMP)view_rightMouseDown, "v@:@");
    class_addMethod(g_view_class, sel("rightMouseUp:"), (IMP)view_rightMouseUp, "v@:@");
    class_addMethod(g_view_class, sel("rightMouseDragged:"), (IMP)view_rightMouseDragged, "v@:@");
    class_addMethod(g_view_class, sel("otherMouseDown:"), (IMP)view_otherMouseDown, "v@:@");
    class_addMethod(g_view_class, sel("otherMouseUp:"), (IMP)view_otherMouseUp, "v@:@");
    class_addMethod(g_view_class, sel("scrollWheel:"), (IMP)view_scrollWheel, "v@:@");
    class_addMethod(g_view_class, sel("keyDown:"), (IMP)view_keyDown, "v@:@");
    class_addMethod(g_view_class, sel("setFrameSize:"), (IMP)view_setFrameSize, "v@:{CGSize=dd}");

    /* NSTextInputClient - see the shim above view_keyDown for why this
     * exists (real dead-key/diacritic composition). */
    class_addMethod(g_view_class, sel("hasMarkedText"), (IMP)view_hasMarkedText, "c@:");
    class_addMethod(g_view_class, sel("markedRange"), (IMP)view_markedRange, "{?=LL}@:");
    class_addMethod(g_view_class, sel("selectedRange"), (IMP)view_selectedRange, "{?=LL}@:");
    class_addMethod(g_view_class, sel("setMarkedText:selectedRange:replacementRange:"),
                     (IMP)view_setMarkedText, "v@:@{?=LL}{?=LL}");
    class_addMethod(g_view_class, sel("unmarkText"), (IMP)view_unmarkText, "v@:");
    class_addMethod(g_view_class, sel("validAttributesForMarkedText"),
                     (IMP)view_validAttributesForMarkedText, "@@:");
    class_addMethod(g_view_class, sel("attributedSubstringForProposedRange:actualRange:"),
                     (IMP)view_attributedSubstringForProposedRange, "@@:{?=LL}^{?=LL}");
    class_addMethod(g_view_class, sel("characterIndexForPoint:"),
                     (IMP)view_characterIndexForPoint, "L@:{CGPoint=dd}");
    class_addMethod(g_view_class, sel("firstRectForCharacterRange:actualRange:"),
                     (IMP)view_firstRectForCharacterRange,
                     "{CGRect={CGPoint=dd}{CGSize=dd}}@:{?=LL}^{?=LL}");
    class_addMethod(g_view_class, sel("insertText:replacementRange:"),
                     (IMP)view_insertText, "v@:@{?=LL}");
    class_addMethod(g_view_class, sel("doCommandBySelector:"),
                     (IMP)view_doCommandBySelector, "v@::");
    {
        Protocol *text_input_proto = objc_getProtocol("NSTextInputClient");
        if (text_input_proto)
            class_addProtocol(g_view_class, text_input_proto);
    }

    objc_registerClassPair(g_view_class);

    g_delegate_class = objc_allocateClassPair((Class)cls("NSObject"), "TuiDelegate", 0);
    class_addMethod(g_delegate_class, sel("timerFired:"), (IMP)delegate_timerFired, "v@:@");
    class_addMethod(g_delegate_class, sel("applicationShouldTerminateAfterLastWindowClosed:"),
                     (IMP)delegate_shouldTerminateAfterLastWindowClosed, "c@:@");
    class_addMethod(g_delegate_class, sel("applicationWillTerminate:"),
                     (IMP)delegate_applicationWillTerminate, "v@:@");
    class_addMethod(g_delegate_class, sel("windowWillResize:toSize:"),
                     (IMP)delegate_windowWillResize, "{CGSize=dd}@:@{CGSize=dd}");
    objc_registerClassPair(g_delegate_class);
}

/* --- Clipboard: the two hooks ui.c calls for <input>/<editor> copy/cut/
 * paste (see ui_screen_copy/cut/paste and Ctrl+C/X/V in ui_screen_update). --- */

/* NSPasteboardTypeString - a real exported NSString* constant symbol from
 * AppKit (present since 10.6); declaring it ourselves rather than including
 * AppKit's (Objective-C, non-C-compatible) header. */
extern id NSPasteboardTypeString;

void ui_clipboard_set_text(const char *utf8)
{
    if (!utf8)
        return;
    id pasteboard = ((id (*)(id, SEL))objc_msgSend)(cls("NSPasteboard"), sel("generalPasteboard"));
    ((void (*)(id, SEL))objc_msgSend)(pasteboard, sel("clearContents"));
    CFStringRef str = cfstr(utf8);
    ((BOOL (*)(id, SEL, id, id))objc_msgSend)(pasteboard, sel("setString:forType:"),
                                               (id)str, NSPasteboardTypeString);
    CFRelease(str);
}

char *ui_clipboard_get_text(void)
{
    id pasteboard = ((id (*)(id, SEL))objc_msgSend)(cls("NSPasteboard"), sel("generalPasteboard"));
    id str = ((id (*)(id, SEL, id))objc_msgSend)(pasteboard, sel("stringForType:"),
                                                  NSPasteboardTypeString);
    if (!str)
        return NULL;
    const char *utf8 = ((const char *(*)(id, SEL))objc_msgSend)(str, sel("UTF8String"));
    if (!utf8)
        return NULL;
    size_t len = strlen(utf8);
    char *out = malloc(len + 1);
    if (out)
        memcpy(out, utf8, len + 1);
    return out;
}

/* --- Filesystem: the directory-listing hook ui.c calls for a file-open
 * dialog (see ui_list_dir in ui.h). Plain POSIX opendir/readdir+stat, same
 * as x11.c - no Cocoa/Objective-C needed for this at all. --- */

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

/* Tools > Terminal (see ide.c's do_open_terminal): opens Terminal.app at
 * `dir`, the same effect as `open -a Terminal <dir>` from a shell - `open`
 * hands the request off to launchd and returns almost immediately, so this
 * app never manages Terminal.app itself as a child.
 *
 * Double-forked (grandchild execs `open`, middle child exits immediately) so
 * that even the short-lived `open` launcher is reparented to init rather
 * than staying this process's child - the one waitpid() below reaps only
 * that middle process. Passing `dir` as `open`'s own argv (rather than
 * building a shell command string) sidesteps any quoting concern over what
 * it contains. */
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
                execlp("open", "open", "-a", "Terminal", dir, (char *)NULL);
            else
                execlp("open", "open", "-a", "Terminal", (char *)NULL);
            _exit(127);
        }
        _exit(0);
    }
    waitpid(pid, NULL, 0);
}

int main(int argc, char** argv)
{
    g_argc = argc;
    g_argv = argv;

    /* sRGB rather than raw DeviceRGB - DeviceRGB skips color management, so
     * on wide-gamut (P3) displays the same 0xAA byte can render as a
     * different color than intended; sRGB makes it explicit and consistent
     * across displays. */
    g_time_epoch = CFAbsoluteTimeGetCurrent();  /* see g_time_epoch's declaration */

    g_colorspace = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);

    apply_font();
    if (!g_font) {
        fprintf(stderr, "cocoa: no usable font found\n");
        return 1;
    }

    register_classes();

    g_app = ((id (*)(id, SEL))objc_msgSend)(cls("NSApplication"), sel("sharedApplication"));
    ((void (*)(id, SEL, long))objc_msgSend)(g_app, sel("setActivationPolicy:"), 0L /* Regular */);

    id delegate = ((id (*)(id, SEL))objc_msgSend)((id)g_delegate_class, sel("new"));
    ((void (*)(id, SEL, id))objc_msgSend)(g_app, sel("setDelegate:"), delegate);

    int win_w = ui_default_cols * g_cell_w;
    int win_h = ui_default_rows * g_cell_h;
    CGRect frame = CGRectMake(0, 0, win_w, win_h);
    unsigned long style_mask = 1 /* Titled */ | 2 /* Closable */ |
                                4 /* Miniaturizable */ | 8 /* Resizable */;

    id window = ((id (*)(id, SEL))objc_msgSend)(cls("NSWindow"), sel("alloc"));
    window = ((id (*)(id, SEL, CGRect, unsigned long, unsigned long, BOOL))objc_msgSend)(
                 window, sel("initWithContentRect:styleMask:backing:defer:"),
                 frame, style_mask, 2UL /* NSBackingStoreBuffered */, NO);
    g_window = window;

    CFStringRef title = cfstr(""); //caption
    ((void (*)(id, SEL, id))objc_msgSend)(window, sel("setTitle:"), (id)title);
    CFRelease(title);

    id view = ((id (*)(id, SEL))objc_msgSend)((id)g_view_class, sel("alloc"));
    view = ((id (*)(id, SEL, CGRect))objc_msgSend)(view, sel("initWithFrame:"), frame);
    g_view = view;
    /* NSViewWidthSizable | NSViewHeightSizable - track the window's content
     * size so setFrameSize: (and our ui_env_resize/ensure_pixmap hook in
     * it) fires as the user resizes the window. */
    ((void (*)(id, SEL, unsigned long))objc_msgSend)(view, sel("setAutoresizingMask:"), 2UL | 16UL);

    ((void (*)(id, SEL, id))objc_msgSend)(window, sel("setContentView:"), view);
    ((BOOL (*)(id, SEL, id))objc_msgSend)(window, sel("makeFirstResponder:"), view);

    /* windowWillResize:toSize: (live-resize cell snapping - see
     * delegate_windowWillResize) is an NSWindowDelegate method, a separate
     * role from the NSApplicationDelegate `delegate` is already set as
     * above; TuiDelegate implements both, so the same object just needs to
     * be registered for this role too. */
    ((void (*)(id, SEL, id))objc_msgSend)(window, sel("setDelegate:"), delegate);

    /* g_frame_extra_w/h: measure the window chrome's fixed pixel overhead
     * (title bar height; no left/right border) once, right here - both the
     * window and content view already have their real frame sizes at this
     * point, and neither changes as long as the style mask/toolbar don't. */
    {
        CGRect win_frame = msgSend_getFrame(window);
        CGRect view_frame = msgSend_getFrame(view);
        g_frame_extra_w = win_frame.size.width - view_frame.size.width;
        g_frame_extra_h = win_frame.size.height - view_frame.size.height;
    }

    /* NSWindow drops plain mouseMoved: events (no button held) unless told
     * otherwise - the Cocoa equivalent of x11.c's PointerMotionMask in its
     * XSelectInput call. Without this, view_mouseMoved never fires. */
    ((void (*)(id, SEL, BOOL))objc_msgSend)(window, sel("setAcceptsMouseMovedEvents:"), YES);
    ((void (*)(id, SEL))objc_msgSend)(window, sel("center"));
    ((void (*)(id, SEL, id))objc_msgSend)(window, sel("makeKeyAndOrderFront:"), (id)0);

    ensure_bitmap(win_w, win_h);
    g_env = ui_env_create(ui_default_cols, ui_default_rows);
    ui_env_set_font_zoom_fn(g_env, adjust_font_size, NULL);

    /* ~60Hz, not 1000Hz: mouse/keyboard input already arrives independently
     * via mouseDown:/keyDown:/etc (posted straight into g_env, not gated by
     * this timer at all) - this timer's only job is periodic app_frame()
     * stepping and redraw. render_frame() does a full DOM walk + full
     * bitmap clear + full glyph pass every single call (ui.c has no
     * dirty/changed tracking - see ui_screen_render), and the only thing
     * actually needing periodic animation is the blinking cursor. Running
     * that whole pipeline 1000x/second instead of ~60x/second was pure
     * wasted CPU: no display can show more than ~60-120 of those anyway,
     * and a blink cursor's own on/off period is on the order of 500ms. */
    ((id (*)(id, SEL, double, id, SEL, id, BOOL))objc_msgSend)(
        cls("NSTimer"), sel("scheduledTimerWithTimeInterval:target:selector:userInfo:repeats:"),
        1.0 / 60.0, delegate, sel("timerFired:"), (id)0, YES);

    ((void (*)(id, SEL, BOOL))objc_msgSend)(g_app, sel("activateIgnoringOtherApps:"), YES);
    ((void (*)(id, SEL))objc_msgSend)(g_app, sel("run"));

    return 0;
}