/*
 * The frame recorder: the core paints the whole screen every time (simple,
 * and cheap on its side), but the backend only gets what changed. The
 * drawing calls of a paint are recorded, compared with the previous
 * paint's, and only the rect where they differ is drawn - clipped to it -
 * and copied to the screen. On X11 every call is drawn by the server, and
 * on XWayland every copy goes through the compositor: a menu item lighting
 * up should cost its own cells, not the window.
 */
#include "ide_gui_frame.h"
#include <stdlib.h>
#include <string.h>

enum op_kind { OP_FILL, OP_SHADE, OP_TEXT, OP_CLIP };

struct op
{
    enum op_kind kind;
    int x, y, w, h;          /* the rect it touches (a clip's rect; 0 w: none) */
    uint32_t a, b;           /* fill: rgb; shade: alpha; text: fg, bg */
    int font;
    int text, count;         /* text: its code points in the frame's pool */
};

struct frame
{
    struct op* ops;
    int count, cap;
    uint32_t* cps;
    int cps_count, cps_cap;
};

static struct frame frames[2];
static int current;          /* the frame being recorded */
static int full = 1;         /* the next paint draws everything */

static void grow(void** items, int* cap, int need, size_t size)
{
    if (need <= *cap)
        return;
    int c = *cap ? *cap * 2 : 1024;
    while (c < need)
        c *= 2;
    void* p = realloc(*items, (size_t)c * size);
    if (!p)
        abort();
    *items = p;
    *cap = c;
}

static struct op* add(enum op_kind kind, int x, int y, int w, int h)
{
    struct frame* f = &frames[current];
    grow((void**)&f->ops, &f->cap, f->count + 1, sizeof *f->ops);
    struct op* o = &f->ops[f->count++];
    memset(o, 0, sizeof *o);
    o->kind = kind;
    o->x = x;
    o->y = y;
    o->w = w;
    o->h = h;
    return o;
}

void frame_invalidate(void)
{
    full = 1;
}

void frame_begin(void)
{
    frames[current].count = 0;
    frames[current].cps_count = 0;
}

void frame_fill_rect(struct gui_canvas* c, int x, int y, int w, int h, uint32_t rgb)
{
    (void)c;
    if (w > 0 && h > 0)
        add(OP_FILL, x, y, w, h)->a = rgb;
}

void frame_shade_rect(struct gui_canvas* c, int x, int y, int w, int h, int alpha)
{
    (void)c;
    if (w > 0 && h > 0)
        add(OP_SHADE, x, y, w, h)->a = (uint32_t)alpha;
}

void frame_set_clip(struct gui_canvas* c, int x, int y, int w, int h)
{
    (void)c;
    add(OP_CLIP, x, y, w > 0 && h > 0 ? w : 0, w > 0 && h > 0 ? h : 0);
}

void frame_draw_text(struct gui_canvas* c, int x, int y, const uint32_t* cps, int count,
                     uint32_t fg, uint32_t bg, enum gui_font font)
{
    if (count <= 0)
        return;
    struct gui_metrics m = gui_font_metrics(c, font);
    struct frame* f = &frames[current];
    grow((void**)&f->cps, &f->cps_cap, f->cps_count + count, sizeof *f->cps);
    struct op* o = add(OP_TEXT, x, y, count * m.cell_w, m.cell_h);
    o->a = fg;
    o->b = bg;
    o->font = (int)font;
    o->text = f->cps_count;
    o->count = count;
    memcpy(f->cps + f->cps_count, cps, (size_t)count * sizeof *cps);
    f->cps_count += count;
}

static int same(const struct frame* fa, const struct op* a, const struct frame* fb, const struct op* b)
{
    if (a->kind != b->kind || a->x != b->x || a->y != b->y || a->w != b->w || a->h != b->h ||
        a->a != b->a || a->b != b->b || a->font != b->font || a->count != b->count)
        return 0;
    return a->kind != OP_TEXT ||
           memcmp(fa->cps + a->text, fb->cps + b->text, (size_t)a->count * sizeof *fa->cps) == 0;
}

static void unite(struct gui_rect* r, int x, int y, int w, int h)
{
    if (w <= 0 || h <= 0)
        return;
    if (r->w <= 0)
    {
        *r = (struct gui_rect){ x, y, w, h };
        return;
    }
    int x1 = r->x + r->w > x + w ? r->x + r->w : x + w;
    int y1 = r->y + r->h > y + h ? r->y + r->h : y + h;
    r->x = r->x < x ? r->x : x;
    r->y = r->y < y ? r->y : y;
    r->w = x1 - r->x;
    r->h = y1 - r->y;
}

static int intersect(const struct gui_rect* a, int x, int y, int w, int h, struct gui_rect* out)
{
    int x0 = a->x > x ? a->x : x, y0 = a->y > y ? a->y : y;
    int x1 = a->x + a->w < x + w ? a->x + a->w : x + w;
    int y1 = a->y + a->h < y + h ? a->y + a->h : y + h;
    *out = (struct gui_rect){ x0, y0, x1 - x0, y1 - y0 };
    return out->w > 0 && out->h > 0;
}

int frame_end(struct gui_canvas* c, int width, int height, struct gui_rect* painted)
{
    struct frame* now = &frames[current];
    struct frame* before = &frames[!current];
    struct gui_rect dirty = { 0, 0, 0, 0 };
    if (full)
    {
        dirty = (struct gui_rect){ 0, 0, width, height };
        full = 0;
    }
    else
    {
        /* every call that differs, at its place in the order, in either frame */
        int n = now->count > before->count ? now->count : before->count;
        for (int i = 0; i < n; i++)
        {
            const struct op* a = i < now->count ? &now->ops[i] : NULL;
            const struct op* b = i < before->count ? &before->ops[i] : NULL;
            if (a && b && same(now, a, before, b))
                continue;
            if (a)
                unite(&dirty, a->x, a->y, a->w, a->h);
            if (b)
                unite(&dirty, b->x, b->y, b->w, b->h);
        }
        /* a clip change alone draws nothing; one that changed what showed
         * also changed a call above */
        struct gui_rect screen = { 0, 0, width, height };
        if (!intersect(&screen, dirty.x, dirty.y, dirty.w, dirty.h, &dirty))
        {
            current = !current;
            return 0;   /* the same picture: nothing to draw or copy */
        }
    }

    /* the calls that touch the dirty rect, clipped to it */
    struct gui_rect clip = dirty, r;
    gui_set_clip(c, dirty.x, dirty.y, dirty.w, dirty.h);
    for (int i = 0; i < now->count; i++)
    {
        const struct op* o = &now->ops[i];
        if (o->kind == OP_CLIP)
        {
            if (o->w <= 0)
                clip = dirty;
            else if (!intersect(&dirty, o->x, o->y, o->w, o->h, &clip))
                clip = (struct gui_rect){ 0, 0, 0, 0 };
            if (clip.w > 0)
                gui_set_clip(c, clip.x, clip.y, clip.w, clip.h);
            continue;
        }
        if (clip.w <= 0 || !intersect(&clip, o->x, o->y, o->w, o->h, &r))
            continue;
        switch (o->kind)
        {
        case OP_FILL: gui_fill_rect(c, o->x, o->y, o->w, o->h, o->a); break;
        case OP_SHADE: gui_shade_rect(c, o->x, o->y, o->w, o->h, (int)o->a); break;
        case OP_TEXT:
            gui_draw_text(c, o->x, o->y, now->cps + o->text, o->count, o->a, o->b, (enum gui_font)o->font);
            break;
        default: break;
        }
    }
    gui_set_clip(c, 0, 0, 0, 0);
    *painted = dirty;
    current = !current;
    return 1;
}
