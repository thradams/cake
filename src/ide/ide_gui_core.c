/* ide_gui_core.c - the portable part of the ide GUI framework: the node
 * tree, layout (anchors), input and painting. See GUI_IDE_SPEC.md.
 *
 * Painting is a full repaint of the window whenever anything is dirty; the
 * backend presents the painted rect. Nothing is painted while nothing
 * changes.
 */
#include "ide_gui_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

void* core_calloc(size_t n, size_t size)
{
    void* p = calloc(n, size);
    if (!p)
        abort();
    return p;
}

char* core_strdup(const char* s)
{
    size_t len = strlen(s);
    char* copy = malloc(len + 1);
    if (!copy)
        abort();
    memcpy(copy, s, len + 1);
    return copy;
}

int core_rect_contains(const struct gui_rect* r, int x, int y)
{
    return x >= r->x && x < r->x + r->w && y >= r->y && y < r->y + r->h;
}

/* --- UTF-8 --- */

/* Decodes one code point at s (which must not be at its terminator) into
 * *cp; returns the number of bytes used. Malformed bytes decode as
 * themselves, one byte each, so text never stalls on bad input. */
int core_utf8_decode(const char* s, uint32_t* cp)
{
    const unsigned char* u = (const unsigned char*)s;
    if (u[0] < 0x80)
    {
        *cp = u[0];
        return 1;
    }
    if ((u[0] & 0xE0) == 0xC0 && (u[1] & 0xC0) == 0x80)
    {
        *cp = ((uint32_t)(u[0] & 0x1F) << 6) | (u[1] & 0x3F);
        return 2;
    }
    if ((u[0] & 0xF0) == 0xE0 && (u[1] & 0xC0) == 0x80 && (u[2] & 0xC0) == 0x80)
    {
        *cp = ((uint32_t)(u[0] & 0x0F) << 12) | ((uint32_t)(u[1] & 0x3F) << 6) | (u[2] & 0x3F);
        return 3;
    }
    if ((u[0] & 0xF8) == 0xF0 && (u[1] & 0xC0) == 0x80 && (u[2] & 0xC0) == 0x80 &&
        (u[3] & 0xC0) == 0x80)
    {
        *cp = ((uint32_t)(u[0] & 0x07) << 18) | ((uint32_t)(u[1] & 0x3F) << 12) |
              ((uint32_t)(u[2] & 0x3F) << 6) | (u[3] & 0x3F);
        return 4;
    }
    *cp = u[0];
    return 1;
}

/* Number of code points in the first `bytes` bytes of s (all of it if
 * bytes < 0) - its width in cells, for a monospaced font. */
int core_utf8_cells(const char* s, int bytes)
{
    int n = 0;
    const char* end = bytes < 0 ? s + strlen(s) : s + bytes;
    while (s < end && *s)
    {
        uint32_t cp = 0;
        s += core_utf8_decode(s, &cp);
        n++;
    }
    return n;
}

/* Byte length of the first `cells` code points of s. */
int core_utf8_prefix_bytes(const char* s, int cells)
{
    const char* p = s;
    while (cells > 0 && *p)
    {
        uint32_t cp = 0;
        p += core_utf8_decode(p, &cp);
        cells--;
    }
    return (int)(p - s);
}

/* The symbols the interface uses that some fonts lack - arrows, triangles,
 * the bullet, the square, blocks - drawn as shapes in the cell at (x, y)
 * instead. 0 when `cp` is not one of them. */
int core_draw_symbol(const struct paint* p, int x, int y, int cw, int ch, uint32_t cp, uint32_t fg, uint32_t bg)
{
    void* c = p->frame;
    switch (cp)
    {
    case 0x2588: case 0x2580: case 0x2584:   /* full, upper half, lower half block */
        gui_fill_rect(c, x, y, cw, ch, bg);
        gui_fill_rect(c, x, cp == 0x2584 ? y + ch / 2 : y, cw, cp == 0x2588 ? ch : (cp == 0x2580 ? ch / 2 : ch - ch / 2), fg);
        return 1;
    case 0x25A0:   /* square */
    case 0x2022:   /* bullet */
    {
        int side = (cw < ch ? cw : ch) * (cp == 0x25A0 ? 3 : 2) / (cp == 0x25A0 ? 4 : 5);
        if (side < 2)
            side = 2;
        gui_fill_rect(c, x, y, cw, ch, bg);
        gui_fill_rect(c, x + (cw - side) / 2, y + (ch - side) / 2, side, side, fg);
        return 1;
    }
    case 0x2318:   /* the Command sign: four loops around a square */
    {
        int side = (cw < ch ? cw : ch) * 9 / 16;   /* about a capital letter's height */
        int t = core_line_weight(p->app);
        int loop = side * 3 / 8 > 2 * t + 1 ? side * 3 / 8 : 2 * t + 1;
        int x0 = x + (cw - side) / 2, y0 = y + (ch - side) / 2;
        gui_fill_rect(c, x, y, cw, ch, bg);
        /* each corner's loop, then the square between them */
        int at[4][2] = { { x0, y0 }, { x0 + side - loop, y0 }, { x0, y0 + side - loop }, { x0 + side - loop, y0 + side - loop } };
        for (int i = 0; i < 4; i++)
        {
            int lx = at[i][0], ly = at[i][1];
            gui_fill_rect(c, lx, ly, loop, t, fg);
            gui_fill_rect(c, lx, ly + loop - t, loop, t, fg);
            gui_fill_rect(c, lx, ly, t, loop, fg);
            gui_fill_rect(c, lx + loop - t, ly, t, loop, fg);
        }
        int ix = x0 + loop - t, iy = y0 + loop - t, iw = side - 2 * (loop - t);
        gui_fill_rect(c, ix, iy, iw, t, fg);
        gui_fill_rect(c, ix, iy + iw - t, iw, t, fg);
        gui_fill_rect(c, ix, iy, t, iw, fg);
        gui_fill_rect(c, ix + iw - t, iy, t, iw, fg);
        return 1;
    }
    case 0x25BA: case 0x25C4:   /* right / left pointing triangle */
    {
        int h = ch / 2 | 1;          /* odd: the point is one pixel */
        int w = (h + 1) / 2;
        int x0 = x + (cw - w) / 2, cy = y + ch / 2;
        gui_fill_rect(c, x, y, cw, ch, bg);
        for (int k = 0; k < w; k++)
        {
            int len = h - 2 * k;     /* the column k from the base */
            int col = cp == 0x25BA ? x0 + k : x0 + w - 1 - k;
            gui_fill_rect(c, col, cy - len / 2, 1, len, fg);
        }
        return 1;
    }
    case 0x2191: case 0x2193: case 0x2190: case 0x2192:   /* arrows: up, down, left, right */
    {
        int vertical = cp == 0x2191 || cp == 0x2193;
        int len = (vertical ? ch : cw) * 2 / 3;    /* the arrow's length, along it */
        int head = (vertical ? cw : ch / 2) * 3 / 4 | 1;   /* the head's width, across it */
        int head_l = (head + 1) / 2;
        int stem = head / 5 > 1 ? head / 5 : 1;
        int stem_l = len - head_l > 1 ? len - head_l : 1;
        gui_fill_rect(c, x, y, cw, ch, bg);
        if (vertical)
        {
            int cx = x + cw / 2, top = y + (ch - head_l - stem_l) / 2, up = cp == 0x2191;
            for (int k = 0; k < head_l; k++)
                gui_fill_rect(c, cx - k, up ? top + k : top + stem_l + head_l - 1 - k, 1 + 2 * k, 1, fg);
            gui_fill_rect(c, cx - stem / 2, up ? top + head_l : top, stem, stem_l, fg);
        }
        else
        {
            int cy = y + ch / 2, left = x + (cw - head_l - stem_l) / 2, to_left = cp == 0x2190;
            for (int k = 0; k < head_l; k++)
                gui_fill_rect(c, to_left ? left + k : left + stem_l + head_l - 1 - k, cy - k, 1, 1 + 2 * k, fg);
            gui_fill_rect(c, to_left ? left + head_l : left, cy - stem / 2, stem_l, stem, fg);
        }
        return 1;
    }
    default:
        return 0;
    }
}

/* Draws the first `bytes` bytes of s (all of it if bytes < 0) at (x, y), in
 * runs of up to 256 code points - one backend call per run, the symbols of
 * core_draw_symbol drawn as shapes. Returns the x after the last cell. */
int core_utf8_width(const struct gui_app* app, enum gui_font font, const char* s, int bytes)
{
    int width = 0;
    if (font != GUI_FONT_UI || !app->canvas)
    {
        width = core_utf8_cells(s, bytes) * core_font_metrics(app, font)->cell_w;
    }
    else
    {
        uint32_t run[256] = { 0 };
        int count = 0;
        int cell_w = core_font_metrics(app, font)->cell_w;
        const char* end = bytes < 0 ? s + strlen(s) : s + bytes;
        while (s < end && *s)
        {
            uint32_t cp = 0;
            s += core_utf8_decode(s, &cp);
            if (cp >= 0x2000)
            {
                /* a symbol takes one cell, as core_draw_utf8 draws it */
                width += cell_w;
                continue;
            }
            run[count++] = cp;
            if (count == (int)(sizeof run / sizeof run[0]))
            {
                width += gui_text_width(app->canvas, run, count, font);
                count = 0;
            }
        }
        width += count > 0 ? gui_text_width(app->canvas, run, count, font) : 0;
    }
    return width;
}

int core_draw_utf8(const struct paint* p, int x, int y, const char* s, int bytes,
                     uint32_t fg, uint32_t bg)
{
    uint32_t run[256] = { 0 };
    int count = 0;
    const struct gui_metrics* m = core_font_metrics(p->app, p->font);
    int cell_w = m->cell_w;
    const char* end = bytes < 0 ? s + strlen(s) : s + bytes;
    while (s < end && *s)
    {
        uint32_t cp = 0;
        s += core_utf8_decode(s, &cp);
        if (cp >= 0x2000)
        {
            /* the run so far, then the symbol in its own cell */
            if (count > 0)
            {
                gui_draw_text(p->frame, x, y, run, count, fg, bg, p->font);
                x += frame_text_width(p->frame, run, count, p->font);
                count = 0;
            }
            /* the Command sign takes the space after it too: one cell is
             * half as wide as tall, too small for its loops */
            if (cp == 0x2318 && s < end && *s == ' ' &&
                core_draw_symbol(p, x, y, 2 * cell_w, m->cell_h, cp, fg, bg))
            {
                s++;
                x += 2 * cell_w;
                continue;
            }
            if (core_draw_symbol(p, x, y, cell_w, m->cell_h, cp, fg, bg))
            {
                x += cell_w;
                continue;
            }
        }
        run[count++] = cp;
        if (count == (int)(sizeof run / sizeof run[0]))
        {
            gui_draw_text(p->frame, x, y, run, count, fg, bg, p->font);
            x += frame_text_width(p->frame, run, count, p->font);
            count = 0;
        }
    }
    if (count > 0)
    {
        gui_draw_text(p->frame, x, y, run, count, fg, bg, p->font);
        x += frame_text_width(p->frame, run, count, p->font);
    }
    return x;
}

/* --- Tree --- */

static struct gui_len cells(int n)
{
    struct gui_len len = { n, 0 };
    return len;
}

struct gui_app* gui_app_create(void)
{
    struct gui_app* app = core_calloc(1, sizeof *app);
    app->root = gui_create(app, GUI_SCREEN);
    app->frame = frame_create();
    app->mouse_x = -1;
    app->mouse_y = -1;
    app->needs_layout = 1;
    app->needs_paint = 1;
    app->main_surface.root = app->root;
    app->active = &app->main_surface;
    return app;
}

static void node_free(struct gui_node* n)
{
    for (int i = 0; i < n->child_count; i++)
        node_free(n->children[i]);
    free(n->children);
    free(n->label);
    free(n->shortcut);
    free(n->hint);
    free(n->window);
    free(n->value);
    editor_free(n);
    free(n);
}

static void surface_free(struct gui_surface* s);
static struct gui_surface* surface_enter(struct gui_app* app, struct gui_surface* s);

void gui_app_free(struct gui_app* app)
{
    if (!app)
        return;
    surface_enter(app, &app->main_surface);
    for (int i = 0; i < app->surfaces.count; i++)
        surface_free(app->surfaces.items[i]);
    free(app->surfaces.items);
    node_free(app->root);
    free(app->windows.items);
    free(app->tooltip);
    frame_free(app->frame);
    free(app);
}

struct gui_node* gui_root(struct gui_app* app)
{
    return app->root;
}

struct gui_node* gui_create(struct gui_app* app, enum gui_kind kind)
{
    (void)app;
    struct gui_node* n = core_calloc(1, sizeof *n);
    n->kind = kind;
    n->label = core_strdup("");
    n->shortcut = core_strdup("");
    n->hint = core_strdup("");
    n->enabled = 1;
    n->layout.anchors = GUI_ANCHOR_LEFT | GUI_ANCHOR_TOP;
    if (kind == GUI_MENUBAR)
    {
        n->layout.anchors = GUI_ANCHOR_LEFT | GUI_ANCHOR_TOP | GUI_ANCHOR_RIGHT;
        n->layout.height = cells(1);
    }
    else if (kind == GUI_STATUSBAR)
    {
        n->layout.anchors = GUI_ANCHOR_LEFT | GUI_ANCHOR_BOTTOM | GUI_ANCHOR_RIGHT;
        n->layout.height = cells(1);
    }
    else if (kind == GUI_WINDOW)
    {
        n->window = core_calloc(1, sizeof *n->window);
        n->window->resizable = 1;
    }
    widget_init(n);
    return n;
}

void gui_clear_children(struct gui_node* n)
{
    for (int i = 0; i < n->child_count; i++)
        node_free(n->children[i]);
    n->child_count = 0;
}

void gui_append(struct gui_node* parent, struct gui_node* child)
{
    if (parent->child_count == parent->child_cap)
    {
        int cap = parent->child_cap ? parent->child_cap * 2 : 4;
        struct gui_node** children = realloc(parent->children, (size_t)cap * sizeof *children);
        if (!children)
            abort();
        parent->children = children;
        parent->child_cap = cap;
    }
    parent->children[parent->child_count++] = child;
    child->parent = parent;
}

void gui_remove(struct gui_node* parent, struct gui_node* child)
{
    for (int i = 0; i < parent->child_count; i++)
    {
        if (parent->children[i] == child)
        {
            memmove(&parent->children[i], &parent->children[i + 1],
                    (size_t)(parent->child_count - i - 1) * sizeof parent->children[0]);
            parent->child_count--;
            child->parent = NULL;
            return;
        }
    }
}

void gui_set_label(struct gui_node* n, const char* utf8)
{
    char* label = core_strdup(utf8);
    free(n->label);
    n->label = label;
}

const char* gui_get_label(const struct gui_node* n)
{
    return n->label;
}

enum gui_kind gui_get_kind(const struct gui_node* n)
{
    return n->kind;
}

struct gui_node* gui_get_parent(const struct gui_node* n)
{
    return n->parent;
}

void gui_set_layout(struct gui_node* n, const struct gui_layout* layout)
{
    n->layout = *layout;
}

void gui_set_after_label(struct gui_node* n, struct gui_node* label)
{
    n->after_label = label;
}

void gui_set_centered(struct gui_node* n, int centered)
{
    n->centered = centered;
}

void gui_set_font_size(struct gui_node* n, enum gui_font_size size)
{
    n->font_size = size;
}

enum gui_font core_node_font(const struct gui_node* n)
{
    /* nothing set: an editor in the editor font, everything else in the "other fonts" one */
    enum gui_font font = n && n->kind == GUI_EDITOR ? GUI_FONT_MAIN : GUI_FONT_UI;
    for (; n; n = n->parent)
    {
        if (n->font_size == GUI_FONT_SIZE_SMALL)
        {
            font = GUI_FONT_SMALL;
            break;
        }
        if (n->font_size == GUI_FONT_SIZE_NORMAL)
        {
            font = GUI_FONT_MAIN;
            break;
        }
        if (n->font_size == GUI_FONT_SIZE_UI)
        {
            font = GUI_FONT_UI;
            break;
        }
    }
    return font;
}

/* --- Scrollbars --- */

void core_scrollbar_thumb(const struct gui_app* app, const struct scrollbar* sb, int* pos, int* len)
{
    int max_scroll = sb->total - sb->visible;
    if (max_scroll <= 0 || sb->total <= 0)
    {
        *pos = 0;
        *len = sb->track;
        return;
    }
    int min_len = app->scrollbar_px;   /* never shorter than it is thick */
    *len = (int)((long long)sb->track * sb->visible / sb->total);
    if (*len < min_len) *len = min_len;
    if (*len > sb->track) *len = sb->track;
    *pos = (int)((long long)(sb->track - *len) * sb->scroll / max_scroll);
}

/* The scroll whose thumb starts at `thumb_pos` px, to the nearest item. */
static int scroll_for_thumb(const struct gui_app* app, const struct scrollbar* sb, int thumb_pos)
{
    int pos = 0, len = 0;
    core_scrollbar_thumb(app, sb, &pos, &len);
    int room = sb->track - len;
    int max_scroll = sb->total - sb->visible;
    if (room <= 0 || max_scroll <= 0)
        return 0;
    if (thumb_pos < 0) thumb_pos = 0;
    if (thumb_pos > room) thumb_pos = room;
    return (int)(((long long)thumb_pos * max_scroll + room / 2) / room);
}

int core_scrollbar_press(struct gui_app* app, const struct scrollbar* sb, int at)
{
    int pos = 0, len = 0;
    core_scrollbar_thumb(app, sb, &pos, &len);
    if (at >= pos && at < pos + len)
    {
        app->ui.scroll_grab = at - pos;
        return sb->scroll;
    }
    app->ui.scroll_grab = len / 2;
    return scroll_for_thumb(app, sb, at - app->ui.scroll_grab);
}

int core_scrollbar_drag(const struct gui_app* app, const struct scrollbar* sb, int at)
{
    return scroll_for_thumb(app, sb, at - app->ui.scroll_grab);
}

const struct gui_metrics* core_font_metrics(const struct gui_app* app, enum gui_font font)
{
    return font == GUI_FONT_UI ? &app->ui_metrics : font == GUI_FONT_SMALL ? &app->small_metrics : &app->metrics;
}

const struct gui_metrics* core_node_metrics(const struct gui_app* app, const struct gui_node* n)
{
    return core_font_metrics(app, core_node_font(n));
}

const struct gui_metrics* core_layout_metrics(const struct gui_app* app, const struct gui_node* n)
{
    /* an editor's place and frame are in the grid of "Font"; its text is in its own */
    enum gui_font font = core_node_font(n);
    return core_font_metrics(app, font == GUI_FONT_MAIN ? GUI_FONT_UI : font);
}

void gui_set_id(struct gui_node* n, int id)
{
    n->id = id;
}

void gui_set_change_id(struct gui_node* n, int id)
{
    n->change_id = id;
}

void gui_set_shortcut(struct gui_node* n, const char* shortcut)
{
    char* copy = core_strdup(shortcut);
    free(n->shortcut);
    n->shortcut = copy;
}

void gui_set_enabled(struct gui_node* n, int enabled)
{
    n->enabled = enabled != 0;
}

void gui_set_separator(struct gui_node* n, int separator)
{
    n->separator = separator != 0;
}

void gui_set_hint(struct gui_node* n, const char* utf8)
{
    char* copy = core_strdup(utf8);
    free(n->hint);
    n->hint = copy;
}

void gui_set_hint_highlighter(struct gui_app* app, const struct gui_highlighter* h)
{
    app->hint_highlighter = h;
}

void gui_set_theme(struct gui_app* app, const struct gui_theme* theme)
{
    app->theme = *theme;
    app->needs_paint = 1;
}

void gui_set_on_event(struct gui_app* app, void (*fn)(void* ctx, int id), void* ctx)
{
    app->on_event.fn = fn;
    app->on_event.ctx = ctx;
}

struct gui_node* core_find_kind(const struct gui_node* n, enum gui_kind kind)
{
    for (int i = 0; i < n->child_count; i++)
    {
        if (n->children[i]->kind == kind)
            return n->children[i];
    }
    return NULL;
}

/* --- Layout --- */

static int len_px(struct gui_len len, int cell, int parent_size)
{
    return len.cells * cell + len.px + len.percent * parent_size / 100;
}

/* One axis of the anchor rule (see ide_gui.h). */
struct axis
{
    int anchored_lo, anchored_hi;    /* left/top, right/bottom */
    int parent_pos, parent_size;
    int lo, hi, size;                /* the node's distances and size, px */
    int cell;                        /* cell size on this axis, px */
};

static void layout_axis(const struct axis* a, int* out_pos, int* out_size)
{
    if (a->anchored_lo && a->anchored_hi)
    {
        *out_pos = a->parent_pos + a->lo;
        *out_size = a->parent_size - a->lo - a->hi;
    }
    else if (a->anchored_lo)
    {
        *out_pos = a->parent_pos + a->lo;
        *out_size = a->size;
    }
    else if (a->anchored_hi)
    {
        *out_pos = a->parent_pos + a->parent_size - a->hi - a->size;
        *out_size = a->size;
    }
    else
    {
        /* Snapped to a whole cell, like every centered dialog of the old IDE. */
        *out_pos = a->parent_pos + (a->parent_size - a->size) / (2 * a->cell) * a->cell;
        *out_size = a->size;
    }
    if (*out_size < 0)
        *out_size = 0;
}

static void layout_anchored(const struct gui_app* app, struct gui_node* n,
                            const struct gui_node* parent)
{
    const struct gui_layout* l = &n->layout;
    const struct gui_metrics* m = core_layout_metrics(app, parent);
    int cw = m->cell_w, ch = m->cell_h;

    struct axis horizontal = {
        (l->anchors & GUI_ANCHOR_LEFT) != 0, (l->anchors & GUI_ANCHOR_RIGHT) != 0,
        parent->rect.x, parent->rect.w,
        len_px(l->left, cw, parent->rect.w), len_px(l->right, cw, parent->rect.w), len_px(l->width, cw, parent->rect.w), cw,
    };
    struct axis vertical = {
        (l->anchors & GUI_ANCHOR_TOP) != 0, (l->anchors & GUI_ANCHOR_BOTTOM) != 0,
        parent->rect.y, parent->rect.h,
        len_px(l->top, ch, parent->rect.h), len_px(l->bottom, ch, parent->rect.h), len_px(l->height, ch, parent->rect.h), ch,
    };
    layout_axis(&horizontal, &n->rect.x, &n->rect.w);
    layout_axis(&vertical, &n->rect.y, &n->rect.h);
}

static void layout_children(const struct gui_app* app, struct gui_node* n);

/* The menubar's titles flow left to right from column 1, each " label "
 * wide; the statusbar's hotkeys likewise, 2 columns apart. Same as the old
 * IDE's layout_menubar / layout_statusbar. */
static void layout_bar(const struct gui_app* app, struct gui_node* bar)
{
    int cw = app->ui_metrics.cell_w;
    int x = bar->rect.x + cw;
    for (int i = 0; i < bar->child_count; i++)
    {
        struct gui_node* c = bar->children[i];
        c->rect.x = x;
        c->rect.y = bar->rect.y;
        c->rect.h = bar->rect.h;
        if (bar->kind == GUI_MENUBAR)
        {
            /* the title in the "other fonts" font, a cell of padding each side */
            c->rect.w = core_utf8_width(app, GUI_FONT_UI, c->label, -1) + 2 * cw;
            x += c->rect.w;
        }
        else
        {
            c->rect.w = core_utf8_width(app, GUI_FONT_UI, c->label, -1);
            x += c->rect.w + 2 * cw;
        }
    }
}

static void layout_children(const struct gui_app* app, struct gui_node* n)
{
    if (n->kind == GUI_MENUBAR || n->kind == GUI_STATUSBAR)
    {
        layout_bar(app, n);
        return;
    }
    if (n->kind != GUI_SCREEN && n->kind != GUI_WINDOW && n->kind != GUI_BOX)
        return;   /* a select's, listbox's or group's children are its rows */
    for (int i = 0; i < n->child_count; i++)
    {
        layout_anchored(app, n->children[i], n);
    }
    /* every label is placed: the controls that follow one go after its column */
    for (int i = 0; i < n->child_count; i++)
    {
        struct gui_node* c = n->children[i];
        if (c->after_label)
        {
            /* the widest label of the column, measured, then one cell */
            int column_w = 0;
            for (int k = 0; k < n->child_count; k++)
            {
                const struct gui_node* label = n->children[k]->after_label;
                if (label && label->rect.x == c->after_label->rect.x)
                {
                    int w = core_utf8_width(app, core_node_font(label), label->label, -1);
                    if (w > column_w)
                        column_w = w;
                }
            }
            int right = c->rect.x + c->rect.w;
            c->rect.x = c->after_label->rect.x + column_w + core_layout_metrics(app, n)->cell_w;
            c->rect.w = right > c->rect.x ? right - c->rect.x : 0;
        }
        layout_children(app, c);
    }
}

static struct gui_rect desktop_rect(const struct gui_app* app);
static void dock_layout(struct gui_app* app);

static void layout(struct gui_app* app)
{
    struct gui_rect whole = { 0, 0, app->w, app->h };
    app->root->rect = whole;
    layout_children(app, app->root);
    dock_layout(app);
    for (int i = 0; i < app->windows.count; i++)
    {
        struct gui_node* win = app->windows.items[i];
        if (win->window->maximized)
            win->rect = desktop_rect(app);   /* follows the desktop as it changes */
        layout_children(app, win);
    }
}

/* --- Windows --- */


/* Between the menubar and the statusbar - where docked windows go. */
static struct gui_rect area_rect(const struct gui_app* app)
{
    int ch = app->ui_metrics.cell_h;
    struct gui_rect r = { 0, 0, app->w, app->h };
    if (core_find_kind(app->root, GUI_MENUBAR))
    {
        r.y += ch;
        r.h -= ch;
    }
    if (core_find_kind(app->root, GUI_STATUSBAR))
        r.h -= ch;
    if (r.h < ch)
        r.h = ch;
    return r;
}

static struct gui_node* docked_on(const struct gui_app* app, enum gui_dock side)
{
    for (int i = 0; i < app->windows.count; i++)
    {
        if (app->windows.items[i]->window->dock == side)
            return app->windows.items[i];
    }
    return NULL;
}

/* The area minus the open docked windows - where maximized windows go.
 * Same as the old IDE's maximized_rect. */
static struct gui_rect desktop_rect(const struct gui_app* app)
{
    int cw = app->ui_metrics.cell_w, ch = app->ui_metrics.cell_h;
    struct gui_rect r = area_rect(app);
    const struct gui_node* left = docked_on(app, GUI_DOCK_LEFT);
    const struct gui_node* right = docked_on(app, GUI_DOCK_RIGHT);
    const struct gui_node* bottom = docked_on(app, GUI_DOCK_BOTTOM);
    if (left) { r.x += left->rect.w; r.w -= left->rect.w; }
    if (right) r.w -= right->rect.w;
    if (bottom) r.h -= bottom->rect.h;
    if (r.w < cw) r.w = cw;
    if (r.h < ch) r.h = ch;
    return r;
}

/* The largest thickness a dock may have: a side one leaves 8 columns, the
 * bottom one 4 rows, so it never hides the rest (the old IDE's caps). */
static int dock_cap(const struct gui_app* app, enum gui_dock side)
{
    struct gui_rect area = area_rect(app);
    if (side == GUI_DOCK_BOTTOM)
    {
        int cap = area.h - 4 * app->ui_metrics.cell_h;
        return cap >= app->ui_metrics.cell_h ? cap : area.h;
    }
    int cap = area.w - 8 * app->ui_metrics.cell_w;
    return cap >= app->ui_metrics.cell_w ? cap : area.w;
}

/* Pins every open docked window to its side - the old IDE's dock_layout:
 * left/right span the area top to bottom, bottom fills the width between
 * them. Run on every layout, so resizes and docks shown/hidden reflow. */
static void dock_layout(struct gui_app* app)
{
    int cw = app->ui_metrics.cell_w, ch = app->ui_metrics.cell_h;
    struct gui_rect area = area_rect(app);
    struct gui_node* left = docked_on(app, GUI_DOCK_LEFT);
    struct gui_node* right = docked_on(app, GUI_DOCK_RIGHT);
    struct gui_node* bottom = docked_on(app, GUI_DOCK_BOTTOM);
    int left_w = 0, right_w = 0;
    if (left)
    {
        int w = left->rect.w;
        if (w > dock_cap(app, GUI_DOCK_LEFT)) w = dock_cap(app, GUI_DOCK_LEFT);
        if (w < cw) w = cw;
        struct gui_rect r = { area.x, area.y, w, area.h };
        left->rect = r;
        left_w = w;
    }
    if (right)
    {
        int w = right->rect.w;
        if (w > dock_cap(app, GUI_DOCK_RIGHT)) w = dock_cap(app, GUI_DOCK_RIGHT);
        if (w < cw) w = cw;
        struct gui_rect r = { area.x + area.w - w, area.y, w, area.h };
        right->rect = r;
        right_w = w;
    }
    if (bottom)
    {
        int h = bottom->rect.h;
        if (h > dock_cap(app, GUI_DOCK_BOTTOM)) h = dock_cap(app, GUI_DOCK_BOTTOM);
        if (h < ch) h = ch;
        int w = area.w - left_w - right_w;
        if (w < cw) w = cw;
        struct gui_rect r = { area.x + left_w, area.y + area.h - h, w, h };
        bottom->rect = r;
    }
}

void gui_window_set_dock(struct gui_node* win, enum gui_dock side, int size)
{
    win->window->dock = side;
    win->window->maximized = 0;
    if (size > 0)
    {
        if (side == GUI_DOCK_BOTTOM)
            win->rect.h = size;
        else if (side != GUI_DOCK_NONE)
            win->rect.w = size;
    }
}

enum gui_dock gui_window_get_dock(const struct gui_node* win)
{
    return win->window->dock;
}

void gui_set_context_menu(struct gui_node* n, struct gui_node* menu)
{
    n->context_menu = menu;
}

struct gui_node* gui_context_target(const struct gui_app* app)
{
    return app->menu.target;
}

void gui_popup_menu(struct gui_app* app, struct gui_node* menu, struct gui_node* below)
{
    app->menu.open = menu;
    app->menu.open_sub = NULL;
    app->menu.key = NULL;
    app->menu.pressed = NULL;
    app->menu.target = below;
    app->menu.popup_x = below->rect.x;
    app->menu.popup_y = below->rect.y + below->rect.h;
    app->needs_paint = 1;
}

void gui_popup_menu_at(struct gui_app* app, struct gui_node* menu, struct gui_node* target, int x, int y)
{
    app->menu.open = menu;
    app->menu.open_sub = NULL;
    app->menu.key = NULL;
    app->menu.pressed = NULL;
    app->menu.target = target;
    app->menu.popup_x = x;
    app->menu.popup_y = y;
    app->menu.key = NULL;
    for (int i = 0; i < menu->child_count && !app->menu.key; i++)
        if (menu->children[i]->kind == GUI_ITEM && menu->children[i]->enabled && !menu->children[i]->separator)
            app->menu.key = menu->children[i];   /* the first, for Enter */
    app->needs_paint = 1;
}

void gui_set_clipboard(struct gui_app* app, const char* utf8)
{
    gui_clipboard_set(app->canvas, utf8);
}

struct gui_rect gui_desktop_rect(const struct gui_app* app)
{
    return desktop_rect(app);
}

void gui_cell_size(const struct gui_app* app, int* w, int* h)
{
    if (w) *w = app->ui_metrics.cell_w;
    if (h) *h = app->ui_metrics.cell_h;
}

void gui_window_set_rect(struct gui_node* win, const struct gui_rect* r)
{
    win->rect = *r;
    win->window->maximized = 0;
}

struct gui_rect gui_window_get_rect(const struct gui_node* win)
{
    return win->rect;
}

void gui_window_set_resizable(struct gui_node* win, int resizable)
{
    win->window->resizable = resizable != 0;
}

void gui_window_set_shadow(struct gui_node* win, int shadow)
{
    win->window->shadow = shadow != 0;
}

void gui_window_set_min_size(struct gui_node* win, int cols, int rows)
{
    win->window->min_cols = cols;
    win->window->min_rows = rows;
}

static int window_index(const struct gui_app* app, const struct gui_node* win)
{
    for (int i = 0; i < app->windows.count; i++)
    {
        if (app->windows.items[i] == win)
            return i;
    }
    return -1;
}

static void window_remove(struct gui_app* app, int index)
{
    struct window_list* list = &app->windows;
    for (int i = index; i < list->count - 1; i++)
        list->items[i] = list->items[i + 1];
    list->count--;
}

static struct gui_surface* surface_of_window(const struct gui_app* app, const struct gui_node* win);
static struct gui_surface* surface_enter(struct gui_app* app, struct gui_surface* s);

void gui_window_open(struct gui_app* app, struct gui_node* win)
{
    struct gui_surface* owner = surface_of_window(app, win);
    if (owner && owner != app->active)
    {
        /* open in another OS window: to the top there, and that OS window to the front */
        struct gui_surface* prev = surface_enter(app, owner);
        gui_window_open(app, win);
        surface_enter(app, prev);
        owner->want_raise = 1;
        return;
    }
    struct window_list* list = &app->windows;
    int index = window_index(app, win);
    if (index >= 0)
        window_remove(app, index);   /* already open: just move it to the top */
    if (list->count == list->cap)
    {
        int cap = list->cap ? list->cap * 2 : 8;
        struct gui_node** items = realloc(list->items, (size_t)cap * sizeof *items);
        if (!items)
            abort();
        list->items = items;
        list->cap = cap;
    }
    list->items[list->count++] = win;
    app->needs_layout = 1;
    app->needs_paint = 1;
}

struct gui_node* core_top_modal(const struct gui_app* app)
{
    if (app->windows.count == 0)
        return NULL;
    struct gui_node* top = app->windows.items[app->windows.count - 1];
    return top->window->modal ? top : NULL;
}

void gui_window_set_modal(struct gui_node* win, int modal)
{
    win->window->modal = modal != 0;
}

/* Centered on the screen, on a whole cell - the old IDE's
 * center_modal_window. */
void gui_window_center(struct gui_app* app, struct gui_node* win)
{
    int cw = app->ui_metrics.cell_w, ch = app->ui_metrics.cell_h;
    int x = (app->w - win->rect.w) / (2 * cw) * cw;
    int y = (app->h - win->rect.h) / (2 * ch) * ch;
    win->rect.x = x > 0 ? x : 0;
    win->rect.y = y > 0 ? y : 0;
    win->window->maximized = 0;
}

void gui_window_close(struct gui_app* app, struct gui_node* win)
{
    struct gui_surface* owner = surface_of_window(app, win);
    if (owner && owner != app->active)
    {
        struct gui_surface* prev = surface_enter(app, owner);
        gui_window_close(app, win);
        surface_enter(app, prev);
        return;
    }
    int index = window_index(app, win);
    if (index < 0)
        return;
    window_remove(app, index);
    if (app->active->detached == win)
    {
        app->active->detached = NULL;   /* its OS window goes */
        app->active->closing = 1;
    }
    if (app->drag.win == win)
        app->drag.mode = DRAG_NONE;
    widget_forget(app, win);
    if (app->menu.target == win)
        app->menu.target = NULL;
    app->needs_paint = 1;
    if (win->window->free_on_close)
        node_free(win);
}

void gui_window_free(struct gui_app* app, struct gui_node* win)
{
    gui_window_close(app, win);
    if (!win->window->free_on_close)
        node_free(win);
}

int gui_window_get_maximized(const struct gui_node* win)
{
    return win->window->maximized;
}

void gui_window_set_ask_close(struct gui_node* win, int ask)
{
    win->window->ask_close = ask != 0;
}

/* --- Message box --- */

/* Sized like the old IDE's ui_message_box: text lines with 2 cells of
 * padding, a gap, then the buttons (each its label plus 4, at least 8 wide,
 * 2 apart) centered on their own row. */
void gui_message_box(struct gui_app* app, const char* caption, const char* text,
                     const char* const labels[], const int ids[], int count)
{
    int cw = app->ui_metrics.cell_w, ch = app->ui_metrics.cell_h;
    int lines = 1, max_cols = 0, cur = 0;
    for (const char* p = text; ; p++)
    {
        if (*p == '\n' || *p == '\0')
        {
            if (cur > max_cols) max_cols = cur;
            cur = 0;
            if (*p == '\0') break;
            lines++;
        }
        else if (((unsigned char)*p & 0xC0) != 0x80)
        {
            cur++;
        }
    }
    int row_cols = 0;
    for (int i = 0; i < count; i++)
    {
        int bw = core_utf8_cells(labels[i], -1) + 6;   /* the label and a margin of 3 cells each side */
        row_cols += (bw < 10 ? 10 : bw) + (i ? 2 : 0);
    }
    int inner = max_cols;
    if (row_cols > inner) inner = row_cols;
    if (core_utf8_cells(caption, -1) + 2 > inner) inner = core_utf8_cells(caption, -1) + 2;

    struct gui_node* win = gui_create(app, GUI_WINDOW);
    gui_set_label(win, caption);
    win->window->modal = 1;
    win->window->resizable = 0;
    win->window->shadow = 1;
    win->window->free_on_close = 1;
    struct gui_rect r = { 0, 0, (inner + 4) * cw, (lines + 6) * ch };
    win->rect = r;

    const char* start = text;
    for (int ln = 0; ln < lines; ln++)
    {
        const char* end = strchr(start, '\n');
        size_t len = end ? (size_t)(end - start) : strlen(start);
        char* line = core_calloc(len + 1, 1);
        memcpy(line, start, len);
        struct gui_node* t = gui_create(app, GUI_TEXT);
        gui_set_label(t, line);
        free(line);
        gui_set_colors(t, app->theme.modal_fg, app->theme.modal_bg);
        t->layout.left.cells = 2;
        t->layout.top.cells = 2 + ln;
        t->layout.width.cells = (int)len;
        t->layout.height.cells = 1;
        gui_append(win, t);
        start = end ? end + 1 : start + len;
    }

    int bx = (inner + 4 - row_cols) / 2;
    if (bx < 1) bx = 1;
    for (int i = 0; i < count; i++)
    {
        int bw = core_utf8_cells(labels[i], -1) + 6;
        if (bw < 10) bw = 10;
        struct gui_node* b = gui_create(app, GUI_BUTTON);
        gui_set_label(b, labels[i]);
        b->id = ids[i];
        b->layout.left.cells = bx;
        b->layout.top.cells = 2 + lines + 1;
        b->layout.width.cells = bw;
        b->layout.height.cells = 1;
        gui_append(win, b);
        bx += bw + 2;
    }
    gui_window_center(app, win);
    gui_window_open(app, win);
}

/* The window that holds `n`, or NULL. */
static struct gui_node* window_of(struct gui_node* n)
{
    for (; n; n = n->parent)
    {
        if (n->kind == GUI_WINDOW)
            return n;
    }
    return NULL;
}

/* --- Surfaces: the main OS window and the detached ones --- */

static void surface_store(const struct gui_app* app, struct gui_surface* s)
{
    s->root = app->root;
    s->w = app->w;
    s->h = app->h;
    s->mouse_x = app->mouse_x;
    s->mouse_y = app->mouse_y;
    s->mouse_mods = app->mouse_mods;
    s->menu = app->menu;
    s->windows = app->windows;
    s->drag = app->drag;
    s->ui = app->ui;
    s->tooltip = app->tooltip;
    s->tooltip_x = app->tooltip_x;
    s->tooltip_y = app->tooltip_y;
    s->frame = app->frame;
}

static void surface_load(struct gui_app* app, const struct gui_surface* s)
{
    app->root = s->root;
    app->w = s->w;
    app->h = s->h;
    app->mouse_x = s->mouse_x;
    app->mouse_y = s->mouse_y;
    app->mouse_mods = s->mouse_mods;
    app->menu = s->menu;
    app->windows = s->windows;
    app->drag = s->drag;
    app->ui = s->ui;
    app->tooltip = s->tooltip;
    app->tooltip_x = s->tooltip_x;
    app->tooltip_y = s->tooltip_y;
    app->frame = s->frame;
}

/* Makes `s` the one the core works on; returns the one that was, to go
 * back to. */
static struct gui_surface* surface_enter(struct gui_app* app, struct gui_surface* s)
{
    struct gui_surface* prev = app->active;
    if (s != prev)
    {
        surface_store(app, prev);
        surface_load(app, s);
        app->active = s;
    }
    return prev;
}

static void surface_free(struct gui_surface* s)
{
    node_free(s->root);
    free(s->windows.items);
    free(s->tooltip);
    frame_free(s->frame);
    free(s);
}

/* needs_layout / needs_paint concern every surface: each gets its own
 * flag, cleared when it is laid out / painted. */
static void surface_spread_dirty(struct gui_app* app)
{
    if (!app->needs_layout && !app->needs_paint)
        return;
    struct gui_surface* m = &app->main_surface;
    m->dirty_layout |= app->needs_layout;
    m->dirty_paint |= app->needs_paint;
    for (int i = 0; i < app->surfaces.count; i++)
    {
        app->surfaces.items[i]->dirty_layout |= app->needs_layout;
        app->surfaces.items[i]->dirty_paint |= app->needs_paint;
    }
    app->needs_layout = 0;
    app->needs_paint = 0;
}

static int list_has(const struct window_list* list, const struct gui_node* win)
{
    for (int i = 0; i < list->count; i++)
    {
        if (list->items[i] == win)
            return 1;
    }
    return 0;
}

/* The surface `win` is open in, or NULL. */
static struct gui_surface* surface_of_window(const struct gui_app* app, const struct gui_node* win)
{
    if (list_has(&app->windows, win))
        return app->active;
    if (app->active != &app->main_surface && list_has(&app->main_surface.windows, win))
        return (struct gui_surface*)&app->main_surface;
    for (int i = 0; i < app->surfaces.count; i++)
    {
        struct gui_surface* s = app->surfaces.items[i];
        if (s != app->active && list_has(&s->windows, win))
            return s;
    }
    return NULL;
}

/* The menus and the statusbar live in the main surface; a detached one
 * runs their shortcuts too. */
static struct gui_node* main_root(const struct gui_app* app)
{
    return app->main_surface.root;
}

struct gui_surface* core_surface_of_node(const struct gui_app* app, const struct gui_node* n)
{
    const struct gui_node* win = n;
    while (win && win->kind != GUI_WINDOW)
        win = win->parent;
    return win ? surface_of_window(app, win) : NULL;
}

int gui_can_detach(const struct gui_app* app)
{
    (void)app;
    return gui_backend_can_detach();
}

int gui_window_get_detached(const struct gui_app* app, const struct gui_node* win)
{
    for (int i = 0; i < app->surfaces.count; i++)
    {
        if (app->surfaces.items[i]->detached == win)
            return 1;
    }
    return 0;
}

void gui_window_detach(struct gui_app* app, struct gui_node* win)
{
    if (!gui_can_detach(app) || surface_of_window(app, win) != &app->main_surface)
        return;
    struct gui_surface* prev = surface_enter(app, &app->main_surface);
    struct gui_surface* s = core_calloc(1, sizeof *s);
    s->restore = win->window->maximized ? win->window->restore : win->rect;
    s->restore_maximized = win->window->maximized;
    int w = win->rect.w, h = win->rect.h;
    int focused = app->ui.focused && window_of(app->ui.focused) == win;
    struct gui_node* focus = focused ? app->ui.focused : NULL;
    gui_window_close(app, win);

    s->root = gui_create(app, GUI_SCREEN);
    s->w = w;
    s->h = h;
    s->mouse_x = -1;
    s->mouse_y = -1;
    s->frame = frame_create();
    s->detached = win;
    s->want_open = 1;
    struct surface_list* list = &app->surfaces;
    if (list->count == list->cap)
    {
        int cap = list->cap ? list->cap * 2 : 4;
        struct gui_surface** items = realloc(list->items, (size_t)cap * sizeof *items);
        if (!items)
            abort();
        list->items = items;
        list->cap = cap;
    }
    list->items[list->count++] = s;

    surface_enter(app, s);
    gui_window_open(app, win);
    gui_window_maximize(app, win);
    app->ui.focused = focus;
    surface_enter(app, prev == s ? &app->main_surface : prev);
    app->needs_layout = 1;
    app->needs_paint = 1;
}

void gui_window_attach(struct gui_app* app, struct gui_node* win)
{
    struct gui_surface* s = surface_of_window(app, win);
    if (!s || s->detached != win)
        return;
    struct gui_surface* prev = surface_enter(app, s);
    int focused = app->ui.focused && window_of(app->ui.focused) == win;
    struct gui_node* focus = focused ? app->ui.focused : NULL;
    gui_window_close(app, win);   /* marks s closing */
    surface_enter(app, &app->main_surface);
    win->window->maximized = 0;
    win->rect = s->restore;
    gui_window_open(app, win);
    if (s->restore_maximized)
        gui_window_maximize(app, win);
    if (focus)
        app->ui.focused = focus;
    surface_enter(app, prev == s ? &app->main_surface : prev);
    app->needs_layout = 1;
    app->needs_paint = 1;
}

struct gui_surface* gui_app_take_surface_open(struct gui_app* app, int* w, int* h)
{
    for (int i = 0; i < app->surfaces.count; i++)
    {
        struct gui_surface* s = app->surfaces.items[i];
        if (s->want_open && !s->closing)
        {
            s->want_open = 0;
            *w = s->w;
            *h = s->h;
            return s;
        }
    }
    return NULL;
}

void gui_surface_start(struct gui_app* app, struct gui_surface* s, void* native, int w, int h)
{
    s->native = native;
    gui_surface_resize(app, s, w, h);
}

void* gui_app_take_surface_raise(struct gui_app* app)
{
    for (int i = 0; i < app->surfaces.count; i++)
    {
        struct gui_surface* s = app->surfaces.items[i];
        if (s->want_raise && s->native && !s->closing)
        {
            s->want_raise = 0;
            return s->native;
        }
    }
    return NULL;
}

void* gui_app_take_surface_close(struct gui_app* app)
{
    for (int i = 0; i < app->surfaces.count; i++)
    {
        struct gui_surface* s = app->surfaces.items[i];
        if (!s->closing || s == app->active)
            continue;
        void* native = s->native;
        struct surface_list* list = &app->surfaces;
        for (int k = i; k < list->count - 1; k++)
            list->items[k] = list->items[k + 1];
        list->count--;
        surface_free(s);
        return native;
    }
    return NULL;
}

void gui_surface_resize(struct gui_app* app, struct gui_surface* s, int w, int h)
{
    struct gui_surface* prev = surface_enter(app, s);
    frame_invalidate(app->frame);   /* a new size: a new back buffer */
    app->w = w;
    app->h = h;
    app->needs_layout = 1;
    app->needs_paint = 1;
    surface_enter(app, prev);
}

void gui_surface_invalidate(struct gui_app* app, struct gui_surface* s)
{
    struct gui_surface* prev = surface_enter(app, s);
    frame_invalidate(app->frame);
    app->needs_paint = 1;
    surface_enter(app, prev);
}

void gui_surface_event(struct gui_app* app, struct gui_surface* s, const struct gui_event* ev)
{
    if (ev->type == GUI_EVENT_CLOSE)
    {
        if (s->detached)
            gui_window_attach(app, s->detached);
        return;
    }
    struct gui_surface* prev = surface_enter(app, s);
    gui_app_event(app, ev);
    surface_enter(app, prev);
}

enum gui_cursor gui_surface_cursor(struct gui_app* app, struct gui_surface* s)
{
    struct gui_surface* prev = surface_enter(app, s);
    enum gui_cursor cursor = gui_app_cursor(app);
    surface_enter(app, prev);
    return cursor;
}

int gui_window_count(const struct gui_app* app)
{
    return app->windows.count;
}

struct gui_node* gui_window_at(const struct gui_app* app, int i)
{
    return i >= 0 && i < app->windows.count ? app->windows.items[i] : NULL;
}

void gui_window_maximize(struct gui_app* app, struct gui_node* win)
{
    struct gui_surface* owner = surface_of_window(app, win);
    if (owner && owner != app->active)
    {
        struct gui_surface* prev = surface_enter(app, owner);
        gui_window_maximize(app, win);
        surface_enter(app, prev);
        return;
    }
    if (!win->window->maximized)
        win->window->restore = win->rect;
    win->window->maximized = 1;
    win->rect = desktop_rect(app);
    app->needs_layout = 1;
    app->needs_paint = 1;
}

static void toggle_maximize(struct gui_app* app, struct gui_node* win)
{
    if (win->window->maximized)
    {
        win->window->maximized = 0;
        win->rect = win->window->restore;
        app->needs_layout = 1;
        app->needs_paint = 1;
    }
    else
    {
        gui_window_maximize(app, win);
    }
}

/* Grows `win` on each side until it meets the desktop edge or the nearest
 * other window overlapping it on the other axis - the old IDE's fit_window,
 * run by a double click on the title row. */
static void fit_window(struct gui_app* app, struct gui_node* win)
{
    struct gui_rect d = desktop_rect(app);
    int x0 = win->rect.x, y0 = win->rect.y;
    int x1 = x0 + win->rect.w, y1 = y0 + win->rect.h;
    int left = d.x, right = d.x + d.w, top = d.y, bottom = d.y + d.h;
    for (int i = 0; i < app->windows.count; i++)
    {
        const struct gui_node* o = app->windows.items[i];
        if (o == win)
            continue;
        int ox0 = o->rect.x, oy0 = o->rect.y;
        int ox1 = ox0 + o->rect.w, oy1 = oy0 + o->rect.h;
        if (oy0 < y1 && oy1 > y0)
        {
            if (ox1 <= x0 && ox1 > left) left = ox1;
            if (ox0 >= x1 && ox0 < right) right = ox0;
        }
        if (ox0 < x1 && ox1 > x0)
        {
            if (oy1 <= y0 && oy1 > top) top = oy1;
            if (oy0 >= y1 && oy0 < bottom) bottom = oy0;
        }
    }
    struct gui_rect r = { left, top, right - left, bottom - top };
    win->rect = r;
    app->needs_layout = 1;
    app->needs_paint = 1;
}

/* The OS window was resized: a window whose right or bottom edge touched
 * the old desktop's edge keeps touching it (the old IDE's
 * reflow_on_screen_resize). */
static void reflow_windows(struct gui_app* app, const struct gui_rect* old_desktop)
{
    struct gui_rect d = desktop_rect(app);
    int cw = app->ui_metrics.cell_w, ch = app->ui_metrics.cell_h;
    for (int i = 0; i < app->windows.count; i++)
    {
        struct gui_node* win = app->windows.items[i];
        if (win->window->maximized)
            continue;
        struct gui_rect* r = &win->rect;
        if (r->x + r->w == old_desktop->x + old_desktop->w)
            r->w = d.x + d.w - r->x;
        if (r->y + r->h == old_desktop->y + old_desktop->h)
            r->h = d.y + d.h - r->y;
        if (r->w < cw) r->w = cw;
        if (r->h < ch) r->h = ch;
    }
}

/* The close icon "[■]" one cell in from the top-left corner, if the window
 * is wide enough; the maximize icon "[↑]" one cell in from the top-right
 * corner, for resizable windows. Shared by hit testing and painting. */
static int has_close_icon(const struct gui_app* app, const struct gui_node* win)
{
    return win->rect.w >= 5 * app->ui_metrics.cell_w;
}

static int has_zoom_icon(const struct gui_app* app, const struct gui_node* win)
{
    /* a modal dialog resizes but does not maximize */
    return win->window->resizable && !win->window->modal && win->window->dock == GUI_DOCK_NONE &&
           win->rect.w >= 10 * app->ui_metrics.cell_w;
}

static int zoom_icon_x(const struct gui_app* app, const struct gui_node* win)
{
    return win->rect.x + win->rect.w - 4 * app->ui_metrics.cell_w;
}

struct gui_node* core_window_at_point(const struct gui_app* app, int x, int y)
{
    for (int i = app->windows.count - 1; i >= 0; i--)
    {
        if (core_rect_contains(&app->windows.items[i]->rect, x, y))
            return app->windows.items[i];
    }
    return NULL;
}

/* Which frame edges (enum resize_edge) the point is on: the outer cell of
 * the left, right and bottom borders, below the title row. 0 when the
 * window cannot be resized now. */
static int resize_edges_at(const struct gui_app* app, const struct gui_node* win, int x, int y)
{
    int cw = app->ui_metrics.cell_w, ch = app->ui_metrics.cell_h;
    const struct gui_rect* r = &win->rect;
    if (!win->window->resizable || win->window->maximized || win->window->dock != GUI_DOCK_NONE ||
        !core_rect_contains(r, x, y) || y < r->y + ch)
        return 0;
    int edges = 0;
    if (x < r->x + cw) edges |= EDGE_LEFT;
    if (x >= r->x + r->w - cw) edges |= EDGE_RIGHT;
    if (y >= r->y + r->h - ch) edges |= EDGE_BOTTOM;
    return edges;
}

/* Whether the point is on a docked window's free border - the one that
 * changes its thickness: the right column of a left dock, the left column
 * of a right dock, the title row of a bottom dock. */
static int on_dock_handle(const struct gui_app* app, const struct gui_node* win, int x, int y)
{
    int cw = app->ui_metrics.cell_w, ch = app->ui_metrics.cell_h;
    const struct gui_rect* r = &win->rect;
    if (!win->window->resizable || !core_rect_contains(r, x, y))
        return 0;
    switch (win->window->dock)
    {
    case GUI_DOCK_LEFT:
        return x >= r->x + r->w - cw && y >= r->y + ch && y < r->y + r->h - ch;
    case GUI_DOCK_RIGHT:
        return x < r->x + cw && y >= r->y + ch && y < r->y + r->h - ch;
    case GUI_DOCK_BOTTOM:
        return y < r->y + ch;
    default:
        return 0;
    }
}

/* A left button press that no menu took: on a window it raises it and may
 * hit an icon, start a move (title row) or a resize (a border). */
static void window_mouse_down(struct gui_app* app, int double_click)
{
    int x = app->mouse_x, y = app->mouse_y;
    int cw = app->ui_metrics.cell_w, ch = app->ui_metrics.cell_h;
    struct gui_node* win = core_window_at_point(app, x, y);
    const struct gui_node* modal = core_top_modal(app);
    if (modal && win != modal)
        return;   /* nothing under a modal reacts */
    if (!win)
    {
        struct gui_node* w = widget_at(app, app->root, x, y);
        if (w)
            widget_mouse_down(app, w, double_click);
        return;
    }
    gui_window_open(app, win);   /* to the top */

    struct gui_node* w = widget_at(app, win, x, y);
    if (w)
    {
        widget_mouse_down(app, w, double_click);
        return;
    }

    const struct gui_rect* r = &win->rect;
    int on_title = y >= r->y && y < r->y + ch;
    if (on_title && has_close_icon(app, win) && x >= r->x + cw && x < r->x + 4 * cw)
    {
        if (win->window->ask_close)
        {
            core_fire(app, win->id);   /* the handler may free win */
            return;
        }
        gui_window_close(app, win);
        core_fire(app, win->id);
        return;
    }
    if (on_title && has_zoom_icon(app, win) && x >= zoom_icon_x(app, win) &&
        x < zoom_icon_x(app, win) + 3 * cw)
    {
        toggle_maximize(app, win);
        return;
    }
    if (win->window->dock != GUI_DOCK_NONE)
    {
        /* Pinned: no move, no maximize - only its free border drags. */
        if (on_dock_handle(app, win, x, y))
        {
            app->drag.mode = DRAG_DOCK;
            app->drag.win = win;
            app->drag.start = *r;
            app->drag.start_x = x;
            app->drag.start_y = y;
            app->needs_paint = 1;
        }
        return;
    }
    if (on_title && double_click && win->window->resizable)
    {
        if (win->window->maximized)
            toggle_maximize(app, win);
        else
            fit_window(app, win);
        return;
    }
    int edges = resize_edges_at(app, win, x, y);
    if (edges)
    {
        app->drag.mode = DRAG_RESIZE;
        app->drag.win = win;
        app->drag.edges = edges;
        app->drag.start = *r;
        app->drag.start_x = x;
        app->drag.start_y = y;
    }
    else if (on_title && !win->window->maximized)
    {
        app->drag.mode = DRAG_MOVE;
        app->drag.win = win;
        app->drag.offset_x = x - r->x;
        app->drag.offset_y = y - r->y;
    }
    app->needs_paint = 1;   /* the frame shows the dragging color */
}

/* Moves or resizes the dragged window to follow the mouse, pixel by pixel. */
static void window_drag_to(struct gui_app* app)
{
    struct window_drag* d = &app->drag;
    if (d->mode == DRAG_NONE)
        return;
    struct gui_rect* r = &d->win->rect;
    int cw = app->ui_metrics.cell_w, ch = app->ui_metrics.cell_h;
    if (d->mode == DRAG_MOVE)
    {
        r->x = app->mouse_x - d->offset_x;
        r->y = app->mouse_y - d->offset_y;
    }
    else if (d->mode == DRAG_DOCK)
    {
        /* At least 8 cells thick, at most dock_cap; dock_layout puts it
         * back against its side. */
        enum gui_dock side = d->win->window->dock;
        int dx = app->mouse_x - d->start_x, dy = app->mouse_y - d->start_y;
        int size = side == GUI_DOCK_LEFT ? d->start.w + dx
                 : side == GUI_DOCK_RIGHT ? d->start.w - dx
                 : d->start.h - dy;
        int min_cells = side == GUI_DOCK_BOTTOM ? d->win->window->min_rows : d->win->window->min_cols;
        int min_size = (min_cells > 8 ? min_cells : 8) * (side == GUI_DOCK_BOTTOM ? ch : cw);
        int cap = dock_cap(app, side);
        if (size > cap) size = cap;
        if (size < min_size) size = min_size;
        if (side == GUI_DOCK_BOTTOM)
            r->h = size;
        else
            r->w = size;
    }
    else
    {
        /* Each moved edge follows the mouse by how far it moved since the
         * press; the left edge keeps the right edge fixed. Never smaller
         * than 12 x 5 cells or the window's own minimum. */
        int min_w = (d->win->window->min_cols > 12 ? d->win->window->min_cols : 12) * cw;
        int min_h = (d->win->window->min_rows > 5 ? d->win->window->min_rows : 5) * ch;
        int dx = app->mouse_x - d->start_x, dy = app->mouse_y - d->start_y;
        *r = d->start;
        if (d->edges & EDGE_RIGHT)
            r->w = d->start.w + dx;
        if (d->edges & EDGE_LEFT)
            r->w = d->start.w - dx;
        if (d->edges & EDGE_BOTTOM)
            r->h = d->start.h + dy;
        if (r->w < min_w) r->w = min_w;
        if (r->h < min_h) r->h = min_h;
        if (d->edges & EDGE_LEFT)
            r->x = d->start.x + d->start.w - r->w;
    }
    app->needs_layout = 1;
    app->needs_paint = 1;
}

/* --- Menus: shortcut text --- */

/* The shortcut as shown in a dropdown: every '+' between keys becomes a
 * space ("Ctrl+Shift+S" -> "Ctrl Shift S", "Ctrl++" -> "Ctrl +"), and on
 * macOS "Ctrl" is the Command sign, "⌘ S" - what is pressed there (Ctrl+Space
 * stays Ctrl). As the old IDE. */
static void shortcut_display(const char* shortcut, char* buf, size_t cap)
{
    size_t n = 0;
    const char* p = shortcut;
#ifdef __APPLE__
    if (strncmp(p, "Ctrl+", 5) == 0 && strcmp(p, "Ctrl+Space") != 0 && cap > 5)
    {
        memcpy(buf, "\xE2\x8C\x98 ", 4);   /* U+2318, drawn by core_draw_symbol */
        n = 4;
        p += 5;
    }
#endif
    for (; *p && n + 1 < cap; p++)
        buf[n++] = (*p == '+' && p[1] != '\0') ? ' ' : *p;
    buf[n] = '\0';
}

/* Whether key + mods is `shortcut` ("Ctrl+Shift+S", "F7", "Ctrl++",
 * "Alt+Left", "Ctrl+Space"). The modifiers must match exactly, so Ctrl+S
 * never fires a Ctrl+Shift+S binding. */
static int shortcut_matches(const char* shortcut, int key, int mods)
{
    int want = 0;
    const char* p = shortcut;
    for (;;)
    {
        if (strncmp(p, "Ctrl+", 5) == 0 && p[5])
        {
            /* the primary modifier - Command on macOS - but Ctrl+Space is Ctrl everywhere */
            want |= strcmp(p, "Ctrl+Space") == 0 ? GUI_MOD_CTRL : GUI_MOD_PRIMARY;
            p += 5;
        }
        else if (strncmp(p, "Shift+", 6) == 0 && p[6]) { want |= GUI_MOD_SHIFT; p += 6; }
        else if (strncmp(p, "Alt+", 4) == 0 && p[4]) { want |= GUI_MOD_ALT; p += 4; }
        else break;
    }
    if (*p == '\0' || mods != want)
        return 0;

    if (p[0] == 'F' && p[1] >= '1' && p[1] <= '9')
    {
        int n = atoi(p + 1);
        return n >= 1 && n <= 12 && key == GUI_KEY_F1 + (n - 1);
    }
    if (strcmp(p, "Left") == 0) return key == GUI_KEY_LEFT;
    if (strcmp(p, "Right") == 0) return key == GUI_KEY_RIGHT;
    if (strcmp(p, "Space") == 0) return key == ' ';
    if (strcmp(p, "Del") == 0) return key == GUI_KEY_DELETE;
    if (p[1] == '\0')
    {
        int c = (unsigned char)p[0];
        if (c >= 'a' && c <= 'z')
            c -= 32;
        return key == c;
    }
    return 0;
}

/* --- Menus: layout of an open dropdown --- */

static int item_is_submenu(const struct gui_node* it)
{
    return it->kind == GUI_ITEM && !it->separator && it->child_count > 0;
}

/* Lays out the dropdown of `menu` (a GUI_MENU, or a submenu parent item)
 * with its top-left at (x, y): one row per item inside a one-cell frame,
 * wide enough for the longest "label  shortcut". Kept on screen and above
 * the statusbar. Sets every item's rect; returns the whole box. */
static struct gui_rect layout_dropdown(const struct gui_app* app, struct gui_node* menu,
                                       int x, int y)
{
    /* the frame is in the main font's cells, the items in the "other fonts" font */
    int cw = app->ui_metrics.cell_w, ch = app->ui_metrics.cell_h;
    int item_h = app->ui_metrics.cell_h;
    int max_w = 0;
    for (int i = 0; i < menu->child_count; i++)
    {
        const struct gui_node* it = menu->children[i];
        int w = core_utf8_width(app, GUI_FONT_UI, it->label, -1);
        if (item_is_submenu(it))
            w += 2 * cw;   /* " ►" - the old IDE drew the marker over the last letter */
        else if (it->shortcut[0])
        {
            char buf[64] = { 0 };
            shortcut_display(it->shortcut, buf, sizeof buf);
            w += 2 * cw + core_utf8_width(app, GUI_FONT_UI, buf, -1);
        }
        if (w > max_w)
            max_w = w;
    }
    int inner_w = max_w + 2 * cw;
    struct gui_rect box = { x, y, inner_w + 2 * cw, menu->child_count * item_h + 2 * ch };

    int max_h = app->h;
    if (core_find_kind(app->root, GUI_STATUSBAR))
        max_h -= ch;
    if (box.x + box.w > app->w) box.x = app->w - box.w;
    if (box.x < 0) box.x = 0;
    if (box.y + box.h > max_h) box.y = max_h - box.h;
    if (box.y < 0) box.y = 0;

    for (int i = 0; i < menu->child_count; i++)
    {
        struct gui_rect r = { box.x + cw, box.y + ch + i * item_h, inner_w, item_h };
        menu->children[i]->rect = r;
    }
    return box;
}

/* The open menu's dropdown box: under its title, or for a context menu at
 * the point it was opened. */
static struct gui_rect layout_open_menu(const struct gui_app* app)
{
    const struct gui_node* title = app->menu.open;
    if (title->parent && title->parent->kind == GUI_MENUBAR)
        return layout_dropdown(app, app->menu.open, title->rect.x, title->rect.y + title->rect.h);
    return layout_dropdown(app, app->menu.open, app->menu.popup_x, app->menu.popup_y);
}

/* The open submenu's box, to the right of the parent dropdown `parent_box`,
 * its first item level with the parent item. */
static struct gui_rect layout_open_sub(const struct gui_app* app, const struct gui_rect* parent_box)
{
    const struct gui_node* item = app->menu.open_sub;
    return layout_dropdown(app, app->menu.open_sub, parent_box->x + parent_box->w,
                           item->rect.y - app->ui_metrics.cell_h);
}

/* --- Input --- */

static void close_menu(struct gui_app* app)
{
    if (app->menu.open)
        app->needs_paint = 1;
    app->menu.open = NULL;
    app->menu.open_sub = NULL;
    app->menu.key = NULL;
    app->menu.pressed = NULL;
}

void core_fire(struct gui_app* app, int id)
{
    if (id != 0 && app->on_event.fn)
        app->on_event.fn(app->on_event.ctx, id);
    app->needs_layout = 1;   /* the handler may have changed labels */
    app->needs_paint = 1;
}

/* Recomputes what is under the mouse: a menubar title (which also switches
 * the open menu, if one is open) or an item of the open dropdowns (a
 * submenu parent opens its submenu). Returns whether the mouse is over the
 * menubar or an open dropdown at all. */
static int update_menu_hover(struct gui_app* app)
{
    struct menu_state* m = &app->menu;
    struct gui_node* old_hot = m->hot;
    struct gui_node* old_open = m->open;
    struct gui_node* old_sub = m->open_sub;
    int over = 0;
    m->hot = NULL;

    struct gui_node* menubar = core_find_kind(app->root, GUI_MENUBAR);
    if (menubar && !core_top_modal(app) &&
        core_rect_contains(&menubar->rect, app->mouse_x, app->mouse_y))
    {
        over = 1;
        for (int i = 0; i < menubar->child_count; i++)
        {
            struct gui_node* title = menubar->children[i];
            if (core_rect_contains(&title->rect, app->mouse_x, app->mouse_y))
            {
                m->hot = title;
                if (m->open && m->open != title && m->open->parent == menubar)
                {
                    m->open = title;    /* hover switches while a menu is open */
                    m->open_sub = NULL;
                    m->key = NULL;
                }
            }
        }
    }

    if (m->open)
    {
        struct gui_rect box = layout_open_menu(app);
        if (core_rect_contains(&box, app->mouse_x, app->mouse_y))
            over = 1;
        for (int i = 0; i < m->open->child_count; i++)
        {
            struct gui_node* it = m->open->children[i];
            if (!core_rect_contains(&it->rect, app->mouse_x, app->mouse_y))
                continue;
            m->hot = it;
            if (item_is_submenu(it))
                m->open_sub = it;
            else
                m->open_sub = NULL;   /* a plain item closes the submenu */
        }
        if (m->open_sub)
        {
            struct gui_rect sub_box = layout_open_sub(app, &box);
            if (core_rect_contains(&sub_box, app->mouse_x, app->mouse_y))
                over = 1;
            for (int i = 0; i < m->open_sub->child_count; i++)
            {
                struct gui_node* it = m->open_sub->children[i];
                if (core_rect_contains(&it->rect, app->mouse_x, app->mouse_y))
                    m->hot = it;
            }
        }
    }

    /* A statusbar hotkey that does something is clickable, like a button. */
    struct gui_node* statusbar = core_find_kind(app->root, GUI_STATUSBAR);
    if (!m->hot && !m->open && statusbar && !core_top_modal(app) &&
        core_rect_contains(&statusbar->rect, app->mouse_x, app->mouse_y))
    {
        for (int i = 0; i < statusbar->child_count; i++)
        {
            struct gui_node* h = statusbar->children[i];
            if (h->id && h->enabled && core_rect_contains(&h->rect, app->mouse_x, app->mouse_y))
            {
                m->hot = h;
                over = 1;
            }
        }
    }

    if (m->hot && m->hot != old_hot && m->hot->kind == GUI_ITEM && m->open &&
        (m->hot->parent == m->open || m->hot->parent == m->open_sub))
        m->key = m->hot;   /* the mouse on an item: it is the choice now */
    if (m->hot != old_hot || m->open != old_open || m->open_sub != old_sub)
        app->needs_paint = 1;
    return over;
}

/* An item that does something when picked. */
static int item_can_fire(const struct gui_node* it)
{
    return it && it->kind == GUI_ITEM && it->enabled && !it->separator && !item_is_submenu(it);
}

static void window_mouse_down(struct gui_app* app, int double_click);

/* A right click: the widget under the mouse, or the nearest of its
 * parents up to its window, that has a context menu opens it. The window's
 * own menu opens only where no child covers it. */
static void context_menu_at(struct gui_app* app)
{
    struct gui_node* win = core_window_at_point(app, app->mouse_x, app->mouse_y);
    const struct gui_node* modal = core_top_modal(app);
    if (!win || (modal && win != modal))
        return;
    gui_window_open(app, win);   /* to the top, as a left click */
    struct gui_node* target = widget_at(app, win, app->mouse_x, app->mouse_y);
    int popup_x = app->mouse_x;
    if (target && target->kind == GUI_EDITOR)
    {
        gui_focus(app, target);   /* the menu acts on the editor clicked, not the one focused before */
        editor_context_click(app, target);   /* the menu acts where the click was */
        /* the caret may land right of the click: the menu opens past it, not over it */
        int caret_x = 0, caret_y = 0;
        gui_editor_caret_point(app, target, &caret_x, &caret_y);
        int cw = core_node_metrics(app, target)->cell_w;
        int bar = cw / 5 > 0 ? cw / 5 : 1;
        if (caret_x + bar >= popup_x && caret_x <= popup_x + cw)   /* only the caret just placed, not one left elsewhere */
        {
            popup_x = caret_x + bar + 1;
        }
    }
    while (target && target != win && !target->context_menu)
        target = target->parent;
    if (!target || target == win)
    {
        for (int i = 0; i < win->child_count; i++)
        {
            if (core_rect_contains(&win->children[i]->rect, app->mouse_x, app->mouse_y))
                return;
        }
        target = win;
    }
    if (!target->context_menu)
        return;
    if (target->kind == GUI_LISTBOX)
    {
        widget_mouse_down(app, target, 0);   /* the row clicked is the one acted on */
        widget_mouse_up(app);
    }
    app->menu.open = target->context_menu;
    app->menu.open_sub = NULL;
    app->menu.key = NULL;
    app->menu.target = target;
    app->menu.popup_x = popup_x;
    app->menu.popup_y = app->mouse_y;
    app->needs_paint = 1;
    if (target->context_menu->id)
        core_fire(app, target->context_menu->id);
}

static void mouse_down(struct gui_app* app, int button, int double_click)
{
    if (button == 0 && widget_popup_mouse_down(app))
        return;
    int over = update_menu_hover(app);
    struct menu_state* m = &app->menu;
    if (button == 1 && !over)
    {
        close_menu(app);
        context_menu_at(app);
        return;
    }
    if (button != 0)
        return;
    if (m->hot && m->hot->kind == GUI_MENU)
    {
        m->open = (m->open == m->hot) ? NULL : m->hot;
        m->open_sub = NULL;
        m->key = NULL;
        app->needs_paint = 1;
    }
    else if (m->hot && (m->hot->kind == GUI_ITEM || m->hot->kind == GUI_HOTKEY))
    {
        m->pressed = m->hot;
    }
    else if (!over)
    {
        close_menu(app);   /* a click anywhere else closes the menu */
        window_mouse_down(app, double_click);
    }
}

static void mouse_up(struct gui_app* app, int button)
{
    update_menu_hover(app);
    struct menu_state* m = &app->menu;
    if (button != 0)
        return;
    if (app->drag.mode != DRAG_NONE)
    {
        app->drag.mode = DRAG_NONE;
        app->needs_paint = 1;   /* back to the normal frame color */
    }
    struct gui_node* pressed = m->pressed;
    m->pressed = NULL;
    if (pressed && pressed == m->hot && pressed->kind == GUI_HOTKEY)
    {
        core_fire(app, pressed->id);
        return;
    }
    if (pressed && pressed == m->hot && item_can_fire(pressed))
    {
        close_menu(app);
        core_fire(app, pressed->id);
    }

    widget_mouse_move(app);   /* what is under the release itself */
    struct gui_node* clicked_widget = app->ui.active;
    struct gui_node* box = clicked_widget ? window_of(clicked_widget) : NULL;
    int clicked = clicked_widget && app->ui.hot == clicked_widget;
    widget_mouse_up(app);
    if (clicked && box && box->window->free_on_close)
        gui_window_close(app, box);
}

/* A shortcut of any menu item (menus open or not). */
/* The statusbar's hotkeys: "Alt+Left" for Back, say. With a modal open only
 * the plain keys - F1 for Help works over any dialog. */
static int run_statusbar_hotkey(struct gui_app* app, int key, int mods, int modal)
{
    struct gui_node* statusbar = core_find_kind(main_root(app), GUI_STATUSBAR);
    for (int i = 0; statusbar && i < statusbar->child_count; i++)
    {
        struct gui_node* h = statusbar->children[i];
        if (h->shortcut[0] && h->id && h->enabled && (!modal || !strchr(h->shortcut, '+')) &&
            shortcut_matches(h->shortcut, key, mods))
        {
            core_fire(app, h->id);
            return 1;
        }
    }
    return 0;
}

static int run_shortcut(struct gui_app* app, int key, int mods)
{
    if (run_statusbar_hotkey(app, key, mods, 0))
        return 1;
    /* the focused widget's context menu: its items' shortcuts act on it */
    struct gui_node* focused = app->ui.focused;
    struct gui_node* context = focused ? focused->context_menu : NULL;
    for (int j = 0; context && j < context->child_count; j++)
    {
        struct gui_node* it = context->children[j];
        if (it->shortcut[0] && item_can_fire(it) && shortcut_matches(it->shortcut, key, mods))
        {
            close_menu(app);
            core_fire(app, it->id);
            return 1;
        }
    }
    struct gui_node* menubar = core_find_kind(main_root(app), GUI_MENUBAR);
    if (!menubar)
        return 0;
    for (int i = 0; i < menubar->child_count; i++)
    {
        struct gui_node* menu = menubar->children[i];
        for (int j = 0; j < menu->child_count; j++)
        {
            struct gui_node* it = menu->children[j];
            if (it->shortcut[0] && item_can_fire(it) && shortcut_matches(it->shortcut, key, mods))
            {
                close_menu(app);
                core_fire(app, it->id);
                return 1;
            }
        }
    }
    return 0;
}

/* The keyboard on an open menu (a popup list, say): Up / Down move the
 * highlight over the items that can fire, Enter picks, Escape closes. 0:
 * another key - the menu closes and the key goes on (typing continues). */
static int menu_key(struct gui_app* app, int key)
{
    struct menu_state* m = &app->menu;
    struct gui_node* menu = m->open_sub ? m->open_sub : m->open;
    if (key == GUI_KEY_ESCAPE)
    {
        close_menu(app);
        return 1;
    }
    if (key == GUI_KEY_ENTER)
    {
        struct gui_node* it = m->key;
        if (!item_can_fire(it))
            return 1;
        close_menu(app);
        core_fire(app, it->id);
        return 1;
    }
    if (key == GUI_KEY_UP || key == GUI_KEY_DOWN)
    {
        int n = menu->child_count, at = -1;
        for (int i = 0; i < n; i++)
            if (menu->children[i] == m->key)
                at = i;
        int step = key == GUI_KEY_DOWN ? 1 : -1;
        for (int k = 1; k <= n; k++)
        {
            int i = at < 0 ? (step > 0 ? k - 1 : n - k) : ((at + step * k) % n + n) % n;
            if (item_can_fire(menu->children[i]))
            {
                m->key = menu->children[i];
                app->needs_paint = 1;
                break;
            }
        }
        return 1;
    }
    if (key == GUI_KEY_LEFT || key == GUI_KEY_RIGHT || key == GUI_KEY_PAGEUP || key == GUI_KEY_PAGEDOWN)
        return 1;
    close_menu(app);
    return 0;
}

static int core_timer_interval(const struct gui_app* app);
static void caret_restart(struct gui_app* app);
static void caret_tick(struct gui_app* app, int interval);

void gui_app_event(struct gui_app* app, const struct gui_event* ev)
{
    switch (ev->type)
    {
    case GUI_EVENT_CLOSE:
        if (app->quit_id)
            core_fire(app, app->quit_id);
        else
            app->quit = 1;
        break;
    case GUI_EVENT_TIMER:
    {
        int interval = core_timer_interval(app);
        caret_tick(app, interval);
        app->timer.elapsed += interval;
        if (app->timer.ms > 0 && app->timer.elapsed >= app->timer.ms)
        {
            app->timer.elapsed = 0;
            if (app->timer.id != 0 && app->on_event.fn)
                app->on_event.fn(app->on_event.ctx, app->timer.id);   /* no repaint unless asked */
        }
        break;
    }
    case GUI_EVENT_MOUSE_MOVE:
        app->mouse_x = ev->x;
        app->mouse_y = ev->y;
        if (app->drag.mode != DRAG_NONE)
        {
            window_drag_to(app);
        }
        else
        {
            update_menu_hover(app);
            widget_mouse_move(app);
        }
        break;
    case GUI_EVENT_MOUSE_LEAVE:
        app->mouse_x = -1;
        app->mouse_y = -1;
        update_menu_hover(app);
        widget_mouse_move(app);
        break;
    case GUI_EVENT_WHEEL:
        app->mouse_x = ev->x;
        app->mouse_y = ev->y;
        widget_mouse_move(app);
        if (ev->wheel && (ev->mods & GUI_MOD_SHIFT) && !ev->hwheel)
            widget_hwheel(app, -ev->wheel);
        else
        {
            if (ev->wheel)
                widget_wheel(app, ev->wheel);
            if (ev->hwheel)
                widget_hwheel(app, ev->hwheel);
        }
        break;
    case GUI_EVENT_MOUSE_DOWN:
        app->mouse_x = ev->x;
        app->mouse_y = ev->y;
        app->mouse_mods = ev->mods;
        caret_restart(app);
        mouse_down(app, ev->button, ev->double_click);
        break;
    case GUI_EVENT_MOUSE_UP:
        app->mouse_x = ev->x;
        app->mouse_y = ev->y;
        mouse_up(app, ev->button);
        break;
    case GUI_EVENT_KEY:
    {
        /* Escape closes the innermost thing open: a menu, a select list
         * (widget_key), then the modal on top. */
        caret_restart(app);
        struct gui_node* modal = core_top_modal(app);
        struct gui_node* focused = app->ui.focused;
        if (app->menu.open && menu_key(app, ev->key))
        {
            /* an open menu took it: Up / Down / Enter / Escape */
        }
        else if (widget_key(app, ev->key, ev->mods))
        {
            /* Enter/Space on a message box button also closes the box. */
            struct gui_node* box = focused ? window_of(focused) : NULL;
            if (focused && focused->kind == GUI_BUTTON && box && box->window->free_on_close &&
                (ev->key == GUI_KEY_ENTER || ev->key == ' '))
                gui_window_close(app, box);
        }
        else if (ev->key == GUI_KEY_ESCAPE && modal)
            gui_window_close(app, modal);
        else if (!modal)
            run_shortcut(app, ev->key, ev->mods);
        else
            run_statusbar_hotkey(app, ev->key, ev->mods, 1);
        break;
    }
    case GUI_EVENT_CHAR:
        caret_restart(app);
        widget_char(app, ev->ch);
        break;
    }
}

/* --- Paint --- */

int core_line_weight(const struct gui_app* app)
{
    int t = app->ui_metrics.cell_w / 7;   /* same rule as the old IDE's box glyphs */
    return t < 1 ? 1 : t;
}

/* The outline of a rect whose corners are (x0, y0) .. (x1, y1), each line
 * `t` thick and growing right/down from its coordinate. */
static void draw_outline(const struct paint* p, int x0, int y0, int x1, int y1, int t,
                         uint32_t color)
{
    gui_fill_rect(p->frame, x0, y0, x1 - x0 + t, t, color);   /* top */
    gui_fill_rect(p->frame, x0, y1, x1 - x0 + t, t, color);   /* bottom */
    gui_fill_rect(p->frame, x0, y0, t, y1 - y0 + t, color);   /* left */
    gui_fill_rect(p->frame, x1, y0, t, y1 - y0 + t, color);   /* right */
}

/* A one-cell frame around `r`: the border cells filled with `bg`, then the
 * line through the middle of them - where the old IDE's box-drawing glyphs
 * put it (single, or two lines for double). */
void core_draw_frame(const struct paint* p, const struct gui_rect* r,
                       enum gui_border_style style, uint32_t fg, uint32_t bg)
{
    int cw = p->app->ui_metrics.cell_w, ch = p->app->ui_metrics.cell_h;
    int t = core_line_weight(p->app);
    gui_fill_rect(p->frame, r->x, r->y, r->w, ch, bg);
    gui_fill_rect(p->frame, r->x, r->y + r->h - ch, r->w, ch, bg);
    gui_fill_rect(p->frame, r->x, r->y, cw, r->h, bg);
    gui_fill_rect(p->frame, r->x + r->w - cw, r->y, cw, r->h, bg);

    int x0 = r->x + cw / 2, y0 = r->y + ch / 2;
    int x1 = r->x + r->w - cw + cw / 2, y1 = r->y + r->h - ch + ch / 2;
    if (style == GUI_BORDER_SINGLE)
    {
        draw_outline(p, x0, y0, x1, y1, t, fg);
    }
    else if (style == GUI_BORDER_DOUBLE)
    {
        int d = t + 1;   /* half the gap between the two lines */
        draw_outline(p, x0 - d, y0 - d, x1 + d, y1 + d, t, fg);
        draw_outline(p, x0 + d, y0 + d, x1 - d, y1 - d, t, fg);
    }
}

/* The drop shadow of a raised rect: one cell to the right and half a cell
 * below, tapering at the top-right and bottom-left - the four pieces of
 * the old IDE's render_button/render_window shadow. */
void core_draw_shadow(const struct paint* p, const struct gui_rect* r)
{
    int cw = p->app->ui_metrics.cell_w, ch = p->app->ui_metrics.cell_h;
    int half = ch / 2;
    int alpha = 110;   /* ~43% black, as in the old IDE */
    gui_shade_rect(p->frame, r->x + r->w, r->y + ch - half, cw, half, alpha);
    if (r->h > ch)
        gui_shade_rect(p->frame, r->x + r->w, r->y + ch, cw, r->h - ch, alpha);
    gui_shade_rect(p->frame, r->x + r->w, r->y + r->h, cw, half, alpha);
    gui_shade_rect(p->frame, r->x + cw, r->y + r->h, r->w - cw, half, alpha);
}

static void paint_dropdown(const struct paint* p, const struct gui_node* menu,
                           const struct gui_rect* box)
{
    const struct gui_theme* t = &p->app->theme;
    const struct menu_state* m = &p->app->menu;
    int cw = p->app->ui_metrics.cell_w;
    struct paint q = *p;   /* the items: the "other fonts" font */
    q.font = GUI_FONT_UI;

    core_draw_shadow(p, box);
    core_draw_frame(p, box, t->menu_border_style, t->menu_border_fg, t->menu_border_bg);

    for (int i = 0; i < menu->child_count; i++)
    {
        const struct gui_node* it = menu->children[i];
        const struct gui_rect* r = &it->rect;
        if (it->separator)
        {
            gui_fill_rect(p->frame, r->x, r->y, r->w, r->h, t->menu_border_bg);
            gui_fill_rect(p->frame, r->x, r->y + r->h / 2, r->w, core_line_weight(p->app),
                          t->menu_border_fg);
            continue;
        }
        int hot = it == m->open_sub || (m->key ? it == m->key : it == m->hot);
        uint32_t fg = !it->enabled ? t->menu_item_fg_disabled
                    : hot ? t->menu_item_fg_hot : t->menu_item_fg;
        uint32_t bg = hot ? t->menu_item_bg_hot : t->menu_item_bg;
        gui_fill_rect(p->frame, r->x, r->y, r->w, r->h, bg);
        /* the font's cell can be taller than the item; its bg must not spill out */
        gui_set_clip(p->frame, r->x, r->y, r->w, r->h);
        core_draw_utf8(&q, r->x + cw, r->y, it->label, -1, fg, bg);

        if (item_is_submenu(it))
        {
            core_draw_utf8(&q, r->x + r->w - 2 * cw, r->y, "\xE2\x96\xBA", -1, fg, bg);   /* U+25BA */
        }
        else if (it->shortcut[0])
        {
            char buf[64] = { 0 };
            shortcut_display(it->shortcut, buf, sizeof buf);
            int sx = r->x + r->w - cw - core_utf8_width(p->app, GUI_FONT_UI, buf, -1);
            core_draw_utf8(&q, sx, r->y, buf, -1,
                      it->enabled ? t->menu_item_shortcut_fg : t->menu_item_fg_disabled, bg);
        }
        gui_set_clip(p->frame, 0, 0, 0, 0);
    }
}

static void paint_menubar(const struct paint* p, const struct gui_node* bar)
{
    const struct gui_theme* t = &p->app->theme;
    const struct menu_state* m = &p->app->menu;
    gui_fill_rect(p->frame, bar->rect.x, bar->rect.y, bar->rect.w, bar->rect.h, t->menu_bg);
    for (int i = 0; i < bar->child_count; i++)
    {
        const struct gui_node* title = bar->children[i];
        int sel = title == m->open || title == m->hot;
        uint32_t fg = sel ? t->menu_fg_sel : t->menu_fg;
        uint32_t bg = sel ? t->menu_bg_sel : t->menu_bg;
        gui_fill_rect(p->frame, title->rect.x, title->rect.y, title->rect.w, title->rect.h, bg);
        struct paint q = *p;
        q.font = GUI_FONT_UI;
        int ty = title->rect.y + (title->rect.h - p->app->ui_metrics.cell_h) / 2;
        gui_set_clip(p->frame, title->rect.x, title->rect.y, title->rect.w, title->rect.h);
        core_draw_utf8(&q, title->rect.x + p->app->ui_metrics.cell_w, ty, title->label, -1, fg, bg);
        gui_set_clip(p->frame, 0, 0, 0, 0);
    }
}

/* "key:label" - the key part in hotkey_key_fg, the rest in hotkey_fg; a
 * label with no ':' is plain text. Same as the old IDE's render_hotkey. */
static void paint_hotkey(const struct paint* p, const struct gui_node* h)
{
    const struct gui_theme* t = &p->app->theme;
    /* One with no id is plain text - a message: no hover, no "key:" part. */
    int hot = p->app->menu.hot == h && h->id != 0;   /* under the mouse: the hover colors */
    uint32_t fg = hot ? t->hotkey_fg_hot : t->hotkey_fg;
    uint32_t key_fg = hot ? t->hotkey_fg_hot : t->hotkey_key_fg;
    uint32_t bg = hot ? t->hotkey_bg_hot : t->hotkey_bg;
    const char* colon = h->id != 0 ? strchr(h->label, ':') : NULL;
    int y = h->rect.y + (h->rect.h - core_font_metrics(p->app, p->font)->cell_h) / 2;
    if (hot)
        gui_fill_rect(p->frame, h->rect.x, h->rect.y, h->rect.w, h->rect.h, bg);
    /* the font's cell can be taller than the bar; its bg must not spill out */
    gui_set_clip(p->frame, h->rect.x, h->rect.y, h->rect.w, h->rect.h);
    if (!colon)
    {
        core_draw_utf8(p, h->rect.x, y, h->label, -1, fg, bg);
    }
    else
    {
        int x = core_draw_utf8(p, h->rect.x, y, h->label, (int)(colon - h->label), key_fg, bg);
        core_draw_utf8(p, x, y, colon, -1, fg, bg);
    }
    gui_set_clip(p->frame, 0, 0, 0, 0);
}

/* Walks a hint as the app's hint highlighter colors it: hidden spans
 * skipped, other colored text in the hotkey key color. Draws it, or with
 * `draw` 0 only measures it. Returns its width, px, and in *x the x after
 * the text. */
static int paint_hint_text(const struct paint* p, int* x, int y, const char* hint, int draw)
{
    const struct gui_theme* t = &p->app->theme;
    const struct gui_highlighter* h = p->app->hint_highlighter;
    struct gui_span spans[64] = { 0 };
    int len = (int)strlen(hint), state = 0, count = 0;
    if (h && h->highlight)
        count = h->highlight(h->ctx, hint, len, &state, spans, 64);
    int width = 0, k = 0;
    for (int i = 0; i < len;)
    {
        while (k < count && spans[k].start + spans[k].len <= i)
            k++;
        int in_span = k < count && spans[k].start <= i;
        int end = in_span ? spans[k].start + spans[k].len : k < count ? spans[k].start : len;
        uint32_t fg = in_span ? spans[k].fg : t->editor_fg;
        if (in_span && (fg & GUI_SPAN_HIDDEN))
        {
            i = end;
            continue;
        }
        int bytes = end - i;
        int run_w = core_utf8_width(p->app, p->font, hint + i, bytes);
        if (draw)
            *x = core_draw_utf8(p, *x, y, hint + i, bytes, fg != t->editor_fg ? t->hotkey_key_fg : t->hotkey_fg,
                                t->hotkey_bg);
        width += run_w;
        i += bytes > 0 ? bytes : 1;
    }
    return width;
}

/* While the node under the mouse has a hint, the bar reads
 * "F1:Help | hint": the first hotkey stays, the rest give way to the hint,
 * cut with "..." if it does not fit. Same as the old IDE. */
static void paint_statusbar_hint(const struct paint* p, const struct gui_node* bar,
                                 const char* hint)
{
    const struct gui_theme* t = &p->app->theme;
    int cw = p->app->ui_metrics.cell_w, ch = p->app->ui_metrics.cell_h;
    int x = bar->rect.x + cw;
    if (bar->child_count > 0)
    {
        const struct gui_node* first = bar->children[0];
        paint_hotkey(p, first);
        x = first->rect.x + first->rect.w + 3 * cw;
        int sep_x = x - 2 * cw + cw / 2;
        gui_fill_rect(p->frame, sep_x, bar->rect.y, core_line_weight(p->app), ch, t->hotkey_fg);
    }
    int right = bar->rect.x + bar->rect.w - cw;
    int y = bar->rect.y + (ch - core_font_metrics(p->app, p->font)->cell_h) / 2;
    int hint_x = x;
    int width = paint_hint_text(p, &hint_x, y, hint, 0);
    if (x + width <= right)
    {
        gui_set_clip(p->frame, x, bar->rect.y, right - x, ch);
        paint_hint_text(p, &x, y, hint, 1);
        gui_set_clip(p->frame, 0, 0, 0, 0);
    }
    else
    {
        /* cut where "..." still fits */
        int dots_x = right - core_utf8_width(p->app, p->font, "...", 3);
        if (dots_x > x)
        {
            gui_set_clip(p->frame, x, bar->rect.y, dots_x - x, ch);
            paint_hint_text(p, &x, y, hint, 1);
            gui_set_clip(p->frame, dots_x, bar->rect.y, right - dots_x, ch);
            core_draw_utf8(p, dots_x, y, "...", -1, t->hotkey_fg, t->hotkey_bg);
            gui_set_clip(p->frame, 0, 0, 0, 0);
        }
    }
}

static void paint_statusbar(const struct paint* p, const struct gui_node* bar)
{
    const struct gui_theme* t = &p->app->theme;
    int cw = p->app->ui_metrics.cell_w;
    gui_fill_rect(p->frame, bar->rect.x, bar->rect.y, bar->rect.w, bar->rect.h, t->hotkey_bg);

    const struct gui_node* hot = p->app->menu.hot;
    if (hot && !hot->hint[0])
        return;   /* a menu item without help: the bar stays blank */
    if (!hot)
        hot = core_top_modal(p->app) ? gui_focused_item(p->app) : NULL;
    if (hot && hot->hint[0])
    {
        paint_statusbar_hint(p, bar, hot->hint);
        return;
    }

    for (int i = 0; i < bar->child_count; i++)
        paint_hotkey(p, bar->children[i]);

    /* The bar's own label, right-aligned one column from the edge, and only
     * if it clears the last hotkey by more than a column - left out rather
     * than cut, like the old IDE. */
    if (bar->label[0])
    {
        int used_x = bar->rect.x + cw;
        if (bar->child_count > 0)
        {
            const struct gui_node* last = bar->children[bar->child_count - 1];
            used_x = last->rect.x + last->rect.w;
        }
        int lx = bar->rect.x + bar->rect.w - cw - core_utf8_width(p->app, p->font, bar->label, -1);
        int y = bar->rect.y + (bar->rect.h - core_font_metrics(p->app, p->font)->cell_h) / 2;
        if (lx > used_x + cw)
        {
            gui_set_clip(p->frame, bar->rect.x, bar->rect.y, bar->rect.w, bar->rect.h);
            core_draw_utf8(p, lx, y, bar->label, -1, t->hotkey_fg, t->hotkey_bg);
            gui_set_clip(p->frame, 0, 0, 0, 0);
        }
    }
}

static void paint_node(const struct paint* p, const struct gui_node* n)
{
    switch (n->kind)
    {
    case GUI_MENUBAR:
    case GUI_STATUSBAR:
    case GUI_MENU:
    case GUI_ITEM:
        return;   /* the bars and dropdowns are painted on top of everything */
    default:
        break;
    }
    if (widget_is_widget(n))
    {
        widget_paint(p, n);
        if (n->kind != GUI_BOX)
            return;   /* its children are its rows, painted by it */
    }
    for (int i = 0; i < n->child_count; i++)
        paint_node(p, n->children[i]);
}

/* The title centered in the top border, one space each side, between the
 * icons; cut with "..." when it does not fit. Same as the old IDE's
 * render_window. */
static void paint_window_title(const struct paint* p, const struct gui_node* win,
                               uint32_t fg, uint32_t bg)
{
    const struct gui_app* app = p->app;
    int cw = app->ui_metrics.cell_w;
    const struct gui_rect* r = &win->rect;
    if (!win->label[0])
        return;
    /* A window whose editor has unsaved changes shows " *" after its title. */
    char marked[512] = { 0 };
    const char* label = win->label;
    const struct gui_node* ed = core_find_kind(win, GUI_EDITOR);
    if (ed && !gui_editor_get_read_only(ed) && gui_editor_get_dirty(ed))
    {
        snprintf(marked, sizeof marked, "%s *", win->label);
        label = marked;
    }
    int lo = r->x + cw;
    if (has_close_icon(app, win))
        lo = r->x + 5 * cw;
    int hi = r->x + r->w - cw;
    if (has_zoom_icon(app, win))
        hi = zoom_icon_x(app, win) - cw;
    /* in the "other fonts" font, centered in the border row */
    struct paint q = *p;
    q.font = GUI_FONT_UI;
    int y = r->y + (app->ui_metrics.cell_h - app->ui_metrics.cell_h) / 2;
    int room = hi - lo;
    int space_w = core_utf8_width(app, GUI_FONT_UI, " ", 1);
    int pad = room >= 5 * cw ? space_w : 0;
    room -= 2 * pad;
    int label_w = core_utf8_width(app, GUI_FONT_UI, label, -1);
    int bytes = (int)strlen(label);
    int dots = 0;
    if (label_w > room)
    {
        /* the longest prefix that leaves room for "..." */
        int dots_w = core_utf8_width(app, GUI_FONT_UI, "...", 3);
        int at = 0, w = 0;
        while (label[at])
        {
            uint32_t cp = 0;
            int len = core_utf8_decode(label + at, &cp);
            int cw_char = core_utf8_width(app, GUI_FONT_UI, label + at, len);
            if (w + cw_char + dots_w > room)
                break;
            w += cw_char;
            at += len;
        }
        bytes = at;
        dots = 1;
        label_w = w + dots_w;
    }
    if (label_w > 0 && label_w <= room)
    {
        int tx = r->x + (r->w - label_w) / 2;
        if (tx < lo + pad)
            tx = lo + pad;
        /* the font's cell can be taller than the border row; its bg must not spill into the window */
        gui_set_clip(p->frame, r->x, r->y, r->w, app->ui_metrics.cell_h);
        if (pad)
            core_draw_utf8(&q, tx - pad, y, " ", -1, fg, bg);
        int end = core_draw_utf8(&q, tx, y, label, bytes, fg, bg);
        if (dots)
            end = core_draw_utf8(&q, end, y, "...", -1, fg, bg);
        if (pad)
            core_draw_utf8(&q, end, y, " ", -1, fg, bg);
        gui_set_clip(p->frame, 0, 0, 0, 0);
    }
}

static void paint_window(const struct paint* p, const struct gui_node* win, int focused)
{
    const struct gui_app* app = p->app;
    const struct gui_theme* t = &app->theme;
    int cw = app->ui_metrics.cell_w, ch = app->ui_metrics.cell_h;
    const struct gui_rect* r = &win->rect;
    int dragging = app->drag.mode != DRAG_NONE && app->drag.win == win;
    int active = dragging || focused;
    uint32_t border_fg = dragging ? t->window_border_fg_dragging
                       : active ? t->window_border_fg : t->window_border_fg_unfocused;
    int docked = win->window->dock != GUI_DOCK_NONE;
    enum gui_border_style style =
        docked ? (active ? t->window_border_style_docked : t->window_border_style_docked_unfocused)
               : (active ? t->window_border_style : t->window_border_style_unfocused);
    uint32_t border_bg = t->window_border_bg;
    uint32_t body_bg = t->window_bg;
    if (win->window->modal)
    {
        /* A modal dialog: always its single active look, in modal colors. */
        border_fg = t->modal_border_fg;
        border_bg = t->modal_border_bg;
        body_bg = t->modal_bg;
        style = t->modal_border_style;
    }

    if (win->window->shadow && !win->window->maximized && !docked)
        core_draw_shadow(p, r);
    if (r->w > 2 * cw && r->h > 2 * ch)
        gui_fill_rect(p->frame, r->x + cw, r->y + ch, r->w - 2 * cw, r->h - 2 * ch, body_bg);
    core_draw_frame(p, r, style, border_fg, border_bg);
    paint_window_title(p, win, border_fg, border_bg);

    /* The icons: the symbol in the middle cell of three, between drawn
     * brackets - not a font's "[" "]" - centered on it both ways, whatever the font. */
    for (int icon = 0; icon < 2; icon++)
    {
        if (icon == 0 ? !has_close_icon(app, win) : !has_zoom_icon(app, win))
        {
            continue;
        }
        int x = icon == 0 ? r->x + cw : zoom_icon_x(app, win);
        int sx = x + cw;   /* the symbol's cell */
        gui_fill_rect(p->frame, x, r->y, 3 * cw, ch, border_bg);
        if (icon == 0)
        {
            core_draw_symbol(p, sx, r->y, cw, ch, 0x25A0, t->window_close_bg, border_bg);
        }
        else
        {
            core_draw_symbol(p, sx, r->y, cw, ch, win->window->maximized ? 0x2193 : 0x2191, border_fg, border_bg);   /* up: maximize, down: restore */
        }
        int lw = core_line_weight(app);
        int side = (cw < ch ? cw : ch) * 3 / 4;   /* the square's side, see core_draw_symbol */
        int gap = side / 3 > 1 ? side / 3 : 1;
        int bh = side + 2 * gap;                  /* the brackets: a little taller than the square */
        int tick = gap + lw;
        int by = r->y + (ch - bh) / 2;
        int lx = sx + (cw - side) / 2 - gap - lw;
        int rx = sx + (cw - side) / 2 + side + gap;
        gui_fill_rect(p->frame, lx, by, lw, bh, border_fg);
        gui_fill_rect(p->frame, lx, by, tick, lw, border_fg);
        gui_fill_rect(p->frame, lx, by + bh - lw, tick, lw, border_fg);
        gui_fill_rect(p->frame, rx, by, lw, bh, border_fg);
        gui_fill_rect(p->frame, rx + lw - tick, by, tick, lw, border_fg);
        gui_fill_rect(p->frame, rx + lw - tick, by + bh - lw, tick, lw, border_fg);
    }

    for (int i = 0; i < win->child_count; i++)
        paint_node(p, win->children[i]);
}

/* The open menu and submenu, the top layer. */
static void paint_open_menus(const struct paint* p, struct gui_app* app)
{
    if (!app->menu.open)
        return;
    struct gui_rect box = layout_open_menu(app);
    paint_dropdown(p, app->menu.open, &box);
    if (app->menu.open_sub)
    {
        struct gui_rect sub_box = layout_open_sub(app, &box);
        paint_dropdown(p, app->menu.open_sub, &sub_box);
    }
}

/* --- Called by the backend --- */

void gui_app_start(struct gui_app* app, struct gui_canvas* c, int argc, char** argv)
{
    app->canvas = c;
    app->metrics = gui_font_metrics(c, GUI_FONT_MAIN);
    app->small_metrics = gui_font_metrics(c, GUI_FONT_SMALL);
    app->ui_metrics = gui_font_metrics(c, GUI_FONT_UI);
    app->scrollbar_px = gui_scrollbar_size(c);
    frame_invalidate(app->frame);
    gui_main(app, argc, argv);
    app->needs_layout = 1;
    app->needs_paint = 1;
}

void gui_app_resize(struct gui_app* app, int w, int h)
{
    frame_invalidate(app->frame);   /* a new size: a new back buffer */
    if (w == app->w && h == app->h)
        return;
    struct gui_rect old_desktop = desktop_rect(app);
    app->w = w;
    app->h = h;
    if (old_desktop.w > 0 && old_desktop.h > 0)
        reflow_windows(app, &old_desktop);
    app->needs_layout = 1;
    app->needs_paint = 1;
}

void gui_zoom(struct gui_app* app, int delta)
{
    app->zoom += delta;
    app->zoom_total += delta;
}

int gui_get_zoom(const struct gui_app* app)
{
    return app->zoom_total;
}

void gui_quit(struct gui_app* app)
{
    app->quit = 1;
}

void gui_repaint(struct gui_app* app)
{
    app->needs_layout = 1;
    app->needs_paint = 1;
}

void gui_app_set_fonts(struct gui_app* app, const char* const names[], int count, int current)
{
    struct app_fonts* f = &app->fonts;
    f->count = count < GUI_MAX_FONTS ? count : GUI_MAX_FONTS;
    for (int i = 0; i < f->count; i++)
        snprintf(f->names[i], sizeof f->names[i], "%s", names[i]);
    f->current = current;
    f->requested = -1;
}

int gui_app_take_font(struct gui_app* app)
{
    int r = app->fonts.requested;
    app->fonts.requested = -1;
    if (r >= 0)
        app->fonts.current = r;
    return r;
}

void gui_app_set_ui_fonts(struct gui_app* app, const char* const names[], int count, int current)
{
    struct app_fonts* f = &app->ui_fonts;
    f->count = count < GUI_MAX_FONTS ? count : GUI_MAX_FONTS;
    for (int i = 0; i < f->count; i++)
    {
        snprintf(f->names[i], sizeof f->names[i], "%s", names[i]);
    }
    f->current = current;
    f->requested = -1;
    f->changed = f->count > 0;   /* the backend opens `current` on its first tick */
    f->editor_size = 0;
}

int gui_app_take_ui_font(struct gui_app* app, int* index, int* editor_size)
{
    int changed = app->ui_fonts.changed;
    app->ui_fonts.changed = 0;
    *index = app->ui_fonts.current;
    *editor_size = app->ui_fonts.editor_size;
    return changed;
}

int gui_get_editor_size(const struct gui_app* app)
{
    return app->ui_fonts.editor_size;
}

void gui_set_editor_size(struct gui_app* app, int size)
{
    if (size >= -1 && size <= 1 && size != app->ui_fonts.editor_size)
    {
        app->ui_fonts.editor_size = size;
        app->ui_fonts.changed = 1;   /* the grid stays: the windows keep their rects */
    }
}

int gui_ui_font_count(const struct gui_app* app)
{
    return app->ui_fonts.count;
}

const char* gui_ui_font_name(const struct gui_app* app, int index)
{
    return index >= 0 && index < app->ui_fonts.count ? app->ui_fonts.names[index] : "";
}

int gui_get_ui_font(const struct gui_app* app)
{
    return app->ui_fonts.current;
}

void gui_set_ui_font(struct gui_app* app, int index)
{
    if (index >= 0 && index < app->ui_fonts.count && index != app->ui_fonts.current)
    {
        app->ui_fonts.current = index;
        app->ui_fonts.changed = 1;
        app->ui_fonts.rescale = 1;
    }
}

int gui_font_count(const struct gui_app* app)
{
    return app->fonts.count;
}

const char* gui_font_name(const struct gui_app* app, int index)
{
    return index >= 0 && index < app->fonts.count ? app->fonts.names[index] : "";
}

int gui_get_font(const struct gui_app* app)
{
    return app->fonts.requested >= 0 ? app->fonts.requested : app->fonts.current;
}

void gui_set_font(struct gui_app* app, int index)
{
    if (index >= 0 && index < app->fonts.count && index != gui_get_font(app))
        app->fonts.requested = index;
}

int gui_modal_open(const struct gui_app* app)
{
    return core_top_modal(app) != NULL;
}

void gui_set_quit_id(struct gui_app* app, int id)
{
    app->quit_id = id;
}

int gui_app_should_quit(const struct gui_app* app)
{
    return app->quit;
}

static int gcd(int a, int b)
{
    while (b)
    {
        int r = a % b;
        a = b;
        b = r;
    }
    return a;
}

/* The backend's one timer serves both the app's timer and the caret blink:
 * it ticks at a period that divides both. */
static int core_timer_interval(const struct gui_app* app)
{
    return app->timer.ms > 0 ? gcd(app->timer.ms, GUI_CARET_BLINK_MS) : GUI_CARET_BLINK_MS;
}

void gui_set_timer(struct gui_app* app, int ms, int id)
{
    if (ms < 0)
        ms = 0;
    if (ms != app->timer.ms)
    {
        app->timer.changed = 1;
        app->timer.elapsed = 0;
    }
    app->timer.ms = ms;
    app->timer.id = id;
}

int gui_app_take_timer(struct gui_app* app, int* ms)
{
    int changed = app->timer.changed;
    app->timer.changed = 0;
    *ms = core_timer_interval(app);
    return changed;
}

/* Typing or clicking shows the caret solid, the blink restarting from there. */
static void caret_restart(struct gui_app* app)
{
    app->caret.off = 0;
    app->caret.elapsed = 0;
}

/* One tick of the backend timer: the caret toggles every
 * GUI_CARET_BLINK_MS, repainting only when an editor or input has focus. */
static void caret_tick(struct gui_app* app, int interval)
{
    app->caret.elapsed += interval;
    if (app->caret.elapsed < GUI_CARET_BLINK_MS)
        return;
    app->caret.elapsed = 0;
    app->caret.off = !app->caret.off;
    const struct gui_node* f = app->ui.focused;
    if (f && (f->kind == GUI_EDITOR || f->kind == GUI_INPUT))
        app->needs_paint = 1;
}

int gui_app_take_zoom(struct gui_app* app)
{
    int zoom = app->zoom;
    app->zoom = 0;
    return zoom;
}

void gui_app_font_changed(struct gui_app* app, struct gui_canvas* c)
{
    struct gui_metrics old = app->ui_metrics;
    app->metrics = gui_font_metrics(c, GUI_FONT_MAIN);
    app->small_metrics = gui_font_metrics(c, GUI_FONT_SMALL);
    app->ui_metrics = gui_font_metrics(c, GUI_FONT_UI);
    if (app->ui_fonts.rescale && old.cell_w > 0 && old.cell_h > 0)
    {
        /* a new "Font": every window keeps its size in cells, a floating one its center */
        int cw = app->ui_metrics.cell_w, ch = app->ui_metrics.cell_h;
        for (int i = 0; i < app->windows.count; i++)
        {
            struct gui_node* win = app->windows.items[i];
            struct gui_rect* r = &win->rect;
            int w = r->w * cw / old.cell_w, h = r->h * ch / old.cell_h;
            if (win->window->dock == GUI_DOCK_NONE)
            {
                r->x += (r->w - w) / 2;
                r->y += (r->h - h) / 2;
                if (r->x < 0) r->x = 0;
                if (r->y < 0) r->y = 0;
            }
            r->w = w;
            r->h = h;
        }
    }
    app->ui_fonts.rescale = 0;
    app->scrollbar_px = gui_scrollbar_size(c);
    app->needs_layout = 1;
    app->needs_paint = 1;
}

uint32_t gui_app_titlebar_color(const struct gui_app* app)
{
    return app->theme.menu_bg;
}

static enum gui_cursor cursor_for_edges(int edges)
{
    if ((edges & EDGE_BOTTOM) && (edges & EDGE_RIGHT)) return GUI_CURSOR_SIZE_NWSE;
    if ((edges & EDGE_BOTTOM) && (edges & EDGE_LEFT)) return GUI_CURSOR_SIZE_NESW;
    if (edges & EDGE_BOTTOM) return GUI_CURSOR_SIZE_NS;
    if (edges) return GUI_CURSOR_SIZE_WE;
    return GUI_CURSOR_ARROW;
}

static enum gui_cursor cursor_for_dock(const struct gui_node* win)
{
    return win->window->dock == GUI_DOCK_BOTTOM ? GUI_CURSOR_SIZE_NS : GUI_CURSOR_SIZE_WE;
}

enum gui_cursor gui_app_cursor(const struct gui_app* app)
{
    if (app->drag.mode == DRAG_RESIZE)
        return cursor_for_edges(app->drag.edges);
    if (app->drag.mode == DRAG_DOCK)
        return cursor_for_dock(app->drag.win);
    if (app->drag.mode != DRAG_NONE || app->menu.open)
        return GUI_CURSOR_ARROW;
    const struct gui_node* win = core_window_at_point(app, app->mouse_x, app->mouse_y);
    if (!win)
        return GUI_CURSOR_ARROW;
    const struct gui_rect* r = &win->rect;
    int cw = app->ui_metrics.cell_w, ch = app->ui_metrics.cell_h;
    int on_title = app->mouse_y >= r->y && app->mouse_y < r->y + ch;
    if (on_title && has_close_icon(app, win) && app->mouse_x >= r->x + cw && app->mouse_x < r->x + 4 * cw)
    {
        return GUI_CURSOR_ARROW;   /* the [x] close icon */
    }
    if (on_dock_handle(app, win, app->mouse_x, app->mouse_y))
        return cursor_for_dock(win);
    return cursor_for_edges(resize_edges_at(app, win, app->mouse_x, app->mouse_y));
}

void gui_set_tooltip(struct gui_app* app, int x, int y, const char* utf8)
{
    if (!utf8 || !utf8[0])
    {
        if (app->tooltip)
        {
            free(app->tooltip);
            app->tooltip = NULL;
            app->needs_paint = 1;
        }
        return;
    }
    if (app->tooltip && strcmp(app->tooltip, utf8) == 0 && app->tooltip_x == x && app->tooltip_y == y)
        return;
    free(app->tooltip);
    app->tooltip = core_strdup(utf8);
    app->tooltip_x = x;
    app->tooltip_y = y;
    app->needs_paint = 1;
}

/* The tooltip: its text a cell in from a one-pixel border, in the menu's colors. */
static void paint_tooltip(const struct paint* p)
{
    const struct gui_app* app = p->app;
    if (!app->tooltip)
        return;
    const struct gui_theme* t = &app->theme;
    /* in the "other fonts" font */
    struct paint q = *p;
    q.font = GUI_FONT_UI;
    int cw = app->ui_metrics.cell_w, ch = app->ui_metrics.cell_h;
    int text_w = core_utf8_width(app, GUI_FONT_UI, app->tooltip, -1);
    if (text_w > app->w - 2 * cw)
        text_w = app->w - 2 * cw;
    int w = text_w + 2 * cw, h = ch + 2;
    int x = app->tooltip_x, y = app->tooltip_y;
    if (x + w > app->w) x = app->w - w;
    if (y + h > app->h) y = app->tooltip_y - h - ch;   /* above the word instead */
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    gui_fill_rect(p->frame, x, y, w, h, t->menu_border_fg);
    gui_fill_rect(p->frame, x + 1, y + 1, w - 2, h - 2, t->menu_bg);
    gui_set_clip(p->frame, x + 1, y + 1, w - 2, h - 2);
    core_draw_utf8(&q, x + cw, y + 1, app->tooltip, -1, t->menu_fg, t->menu_bg);
    gui_set_clip(p->frame, 0, 0, 0, 0);
}

/* Paints the active surface, if it is dirty. */
static int paint_active(struct gui_app* app, struct gui_canvas* c, struct gui_rect* painted)
{
    struct gui_surface* s = app->active;
    surface_spread_dirty(app);
    if (s->dirty_layout)
    {
        s->dirty_layout = 0;
        layout(app);
        update_menu_hover(app);   /* rects moved under the mouse */
        surface_spread_dirty(app);
    }
    if (!s->dirty_paint)
        return 0;
    s->dirty_paint = 0;

    struct paint p = { app, app->frame, GUI_FONT_MAIN };
    frame_begin(app->frame, c);
    gui_fill_rect(p.frame, 0, 0, app->w, app->h, app->theme.desktop_bg);
    paint_node(&p, app->root);
    /* a maximized window hides every undocked window below it */
    int first = 0;
    for (int i = 0; i < app->windows.count; i++)
    {
        const struct gui_node* win = app->windows.items[i];
        if (win->window->maximized && win->window->dock == GUI_DOCK_NONE)
            first = i;
    }
    for (int i = 0; i < app->windows.count; i++)
    {
        const struct gui_node* win = app->windows.items[i];
        if (i >= first || win->window->dock != GUI_DOCK_NONE)
            paint_window(&p, win, i == app->windows.count - 1);
    }
    struct gui_node* menubar = core_find_kind(app->root, GUI_MENUBAR);
    struct gui_node* statusbar = core_find_kind(app->root, GUI_STATUSBAR);
    if (menubar)
        paint_menubar(&p, menubar);
    if (statusbar)
    {
        struct paint bar_paint = p;   /* the "other fonts" font */
        bar_paint.font = GUI_FONT_UI;
        paint_statusbar(&bar_paint, statusbar);
    }
    widget_paint_popups(&p);
    paint_open_menus(&p, app);
    paint_tooltip(&p);

    return frame_end(app->frame, app->w, app->h, painted);   /* only what changed */
}

int gui_app_paint(struct gui_app* app, struct gui_canvas* c, struct gui_rect* painted)
{
    struct gui_surface* prev = surface_enter(app, &app->main_surface);
    int r = paint_active(app, c, painted);
    surface_enter(app, prev);
    return r;
}

int gui_surface_paint(struct gui_app* app, struct gui_surface* s, struct gui_canvas* c, struct gui_rect* painted)
{
    struct gui_surface* prev = surface_enter(app, s);
    int r = paint_active(app, c, painted);
    surface_enter(app, prev);
    return r;
}

void gui_app_invalidate(struct gui_app* app)
{
    struct gui_surface* prev = surface_enter(app, &app->main_surface);
    frame_invalidate(app->frame);
    app->needs_paint = 1;
    surface_enter(app, prev);
}
