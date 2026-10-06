/* ide_gui_widgets.c - the ide framework's widgets: text, box, button,
 * input, select, listbox and group. Each one looks and behaves like its
 * counterpart in the old IDE (render_button, render_input, render_select,
 * render_listbox, render_group in ide_ui.c). See GUI_IDE_SPEC.md.
 */
#include "ide_gui_internal.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

static int is_focusable_kind(enum gui_kind kind)
{
    return kind == GUI_BUTTON || kind == GUI_INPUT || kind == GUI_SELECT ||
           kind == GUI_LISTBOX || kind == GUI_GROUP || kind == GUI_EDITOR;
}

static int is_focusable(const struct gui_node* n)
{
    return n->enabled && is_focusable_kind(n->kind);
}

int widget_is_widget(const struct gui_node* n)
{
    return n->kind == GUI_TEXT || n->kind == GUI_BOX || is_focusable_kind(n->kind);
}

void widget_init(struct gui_node* n)
{
    n->value = core_strdup("");
    if (n->kind == GUI_EDITOR)
        editor_create(n);
    n->selected = (n->kind == GUI_SELECT || n->kind == GUI_LISTBOX || n->kind == GUI_GROUP) ? 0 : -1;
}

/* --- Values --- */

void gui_set_colors(struct gui_node* n, uint32_t fg, uint32_t bg)
{
    n->fg = fg;
    n->bg = bg;
    n->has_colors = 1;
}

void gui_set_value(struct gui_node* n, const char* utf8)
{
    if (n->kind == GUI_EDITOR)
    {
        editor_set_text(n, utf8);
        return;
    }
    char* value = core_strdup(utf8);
    free(n->value);
    n->value = value;
    n->cursor = (int)strlen(value);
    n->anchor = n->cursor;
    n->hscroll = 0;   /* GUI_INPUT: from the start; focused, the paint scrolls to the caret */
}

const char* gui_get_value(const struct gui_node* n)
{
    if (n->kind == GUI_EDITOR)
        return editor_get_text(n);
    return n->value;
}

void gui_set_selected(struct gui_node* n, int index)
{
    n->selected = index;
    n->cursor = index;   /* a group's keyboard row follows */
    n->reveal = 1;
}

int gui_get_selected(const struct gui_node* n)
{
    return n->selected;
}

void gui_set_multi(struct gui_node* group, int multi)
{
    group->multi = multi != 0;
}

void gui_set_checked(struct gui_node* group, int index, int checked)
{
    if (index >= 0 && index < group->child_count)
        group->children[index]->checked = checked != 0;
}

int gui_get_checked(const struct gui_node* group, int index)
{
    return index >= 0 && index < group->child_count && group->children[index]->checked;
}

void gui_button_set_tab(struct gui_node* button, int selected)
{
    button->tab = selected ? 2 : 1;
}

int gui_child_count(const struct gui_node* n)
{
    return n->child_count;
}

struct gui_node* gui_child_at(const struct gui_node* n, int i)
{
    return i >= 0 && i < n->child_count ? n->children[i] : NULL;
}

/* --- Focus --- */

static int is_descendant(const struct gui_node* n, const struct gui_node* ancestor)
{
    for (; n; n = n->parent)
    {
        if (n == ancestor)
            return 1;
    }
    return 0;
}

void gui_focus(struct gui_app* app, struct gui_node* n)
{
    struct gui_surface* owner = core_surface_of_node(app, n);
    if (owner && owner != app->active)
    {
        owner->ui.focused = n;   /* that OS window's focus, and it to the front */
        owner->want_raise = 1;
    }
    else
    {
        app->ui.focused = n;
    }
    app->needs_paint = 1;
}

struct gui_node* gui_focused(const struct gui_app* app)
{
    return app->ui.focused;
}

struct gui_node* gui_focused_item(const struct gui_app* app)
{
    struct gui_node* n = app->ui.focused;
    if (!n)
        return NULL;
    int index = n->kind == GUI_GROUP ? n->cursor
              : (n->kind == GUI_SELECT || n->kind == GUI_LISTBOX) ? n->selected : -1;
    return index >= 0 && index < n->child_count ? n->children[index] : n;
}

void widget_forget(struct gui_app* app, const struct gui_node* n)
{
    struct widget_state* ui = &app->ui;
    struct gui_node** refs[] = { &ui->focused, &ui->hot, &ui->active, &ui->open_select,
                                 &ui->selecting, &ui->scrolling, &ui->hscrolling, &ui->thumb_hot };
    for (int i = 0; i < (int)(sizeof refs / sizeof refs[0]); i++)
    {
        if (*refs[i] && is_descendant(*refs[i], n))
            *refs[i] = NULL;
    }
}

/* The focusable widgets of `scope` in document order, into out[]. */
static int collect_focusable(struct gui_node* scope, struct gui_node** out, int count, int max)
{
    for (int i = 0; i < scope->child_count && count < max; i++)
    {
        struct gui_node* c = scope->children[i];
        if (is_focusable(c))
            out[count++] = c;
        else if (c->kind == GUI_BOX || c->kind == GUI_SCREEN)
            count = collect_focusable(c, out, count, max);
    }
    return count;
}

/* Where Tab moves focus: the top window, or the root without windows. */
static struct gui_node* focus_scope(struct gui_app* app)
{
    if (app->windows.count > 0)
        return app->windows.items[app->windows.count - 1];
    return app->root;
}

static void focus_step(struct gui_app* app, int forward)
{
    struct gui_node* list[256] = { 0 };
    int n = collect_focusable(focus_scope(app), list, 0, 256);
    if (n == 0)
        return;
    int at = -1;
    for (int i = 0; i < n; i++)
    {
        if (list[i] == app->ui.focused)
            at = i;
    }
    if (at < 0)
        at = forward ? n - 1 : 0;
    gui_focus(app, list[(at + (forward ? 1 : n - 1)) % n]);
}

/* --- UTF-8 editing helpers (GUI_INPUT) --- */

/* The byte offset one code point before/after `at`. */
static int prev_char(const char* s, int at)
{
    if (at <= 0)
        return 0;
    at--;
    while (at > 0 && ((unsigned char)s[at] & 0xC0) == 0x80)
        at--;
    return at;
}

static int next_char(const char* s, int at)
{
    if (!s[at])
        return at;
    uint32_t cp = 0;
    return at + core_utf8_decode(s + at, &cp);
}

static void input_selection(const struct gui_node* n, int* lo, int* hi)
{
    *lo = n->cursor < n->anchor ? n->cursor : n->anchor;
    *hi = n->cursor < n->anchor ? n->anchor : n->cursor;
}

/* Replaces bytes [lo, hi) of the value with `text`; the caret ends after it. */
static void input_replace(struct gui_node* n, int lo, int hi, const char* text, int text_len)
{
    size_t old_len = strlen(n->value);
    char* value = malloc(old_len - (size_t)(hi - lo) + (size_t)text_len + 1);
    if (!value)
        abort();
    memcpy(value, n->value, (size_t)lo);
    memcpy(value + lo, text, (size_t)text_len);
    memcpy(value + lo + text_len, n->value + hi, old_len - (size_t)hi + 1);
    free(n->value);
    n->value = value;
    n->cursor = lo + text_len;
    n->anchor = n->cursor;
}

void gui_input_insert(struct gui_node* n, const char* utf8)
{
    int lo = 0, hi = 0;
    input_selection(n, &lo, &hi);
    input_replace(n, lo, hi, utf8, (int)strlen(utf8));
}

/* How far the text is scrolled left, px: the caret is always in view
 * (nothing stored, derived each time). */
/* The text's scroll, in px: it moves only when the focused caret leaves the
 * box, and just enough to bring it back - the code editor's rule
 * (ensure_caret_visible). The text does not chase the caret. */
static void input_follow_caret(const struct gui_app* app, struct gui_node* n)
{
    int cw = core_node_metrics(app, n)->cell_w;
    int room = n->rect.w - (cw / 5 > 0 ? cw / 5 : 1);
    if (room <= 0)
        return;
    int caret_x = core_utf8_width(app, core_node_font(n), n->value, n->cursor);
    int text_w = core_utf8_width(app, core_node_font(n), n->value, (int)strlen(n->value));
    if (caret_x > n->hscroll + room)
        n->hscroll = caret_x - room;
    else if (caret_x < n->hscroll)
        n->hscroll = caret_x;
    if (n->hscroll > text_w - room)   /* no hole after the text: its end stays at the right edge */
        n->hscroll = text_w - room;
    if (n->hscroll < 0 || text_w <= room)
        n->hscroll = 0;
}

static int input_offset(const struct gui_app* app, const struct gui_node* n)
{
    (void)app;
    return n->hscroll;
}

static int is_word_byte(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
           c == '_' || (unsigned char)c >= 0x80;
}

/* The start of the word before `at`, skipping blanks first. */
static int input_word_left(const char* s, int at)
{
    while (at > 0 && !is_word_byte(s[at - 1]))
        at--;
    while (at > 0 && is_word_byte(s[at - 1]))
        at--;
    return at;
}

/* The end of the word after `at`, skipping blanks first. */
static int input_word_right(const char* s, int at)
{
    while (s[at] && !is_word_byte(s[at]))
        at++;
    while (s[at] && is_word_byte(s[at]))
        at++;
    return at;
}

/* --- Paint --- */

static void paint_text(const struct paint* p, const struct gui_node* n)
{
    const struct gui_theme* t = &p->app->theme;
    uint32_t fg = n->has_colors ? n->fg : t->window_fg;
    uint32_t bg = n->has_colors ? n->bg : t->window_bg;
    /* centered on its first row of the layout */
    int y = n->rect.y + (core_layout_metrics(p->app, n)->cell_h - core_node_metrics(p->app, n)->cell_h) / 2;
    int x = n->rect.x;
    if (n->centered)
        x += (n->rect.w - core_utf8_width(p->app, p->font, n->label, -1)) / 2;
    core_draw_utf8(p, x, y, n->label, -1, fg, bg);
}

static void paint_box(const struct paint* p, const struct gui_node* n)
{
    const struct gui_theme* t = &p->app->theme;
    int cw = core_layout_metrics(p->app, n)->cell_w, ch = core_layout_metrics(p->app, n)->cell_h;
    const struct gui_rect* r = &n->rect;
    if (r->w > 2 * cw && r->h > 2 * ch)
        gui_fill_rect(p->frame, r->x + cw, r->y + ch, r->w - 2 * cw, r->h - 2 * ch,
                      n->has_colors ? n->bg : t->box_bg);
    core_draw_frame(p, r, t->box_border_style, t->box_fg, t->box_bg);
    if (n->label[0])
    {
        int tx = r->x + (r->w - core_utf8_width(p->app, p->font, n->label, -1)) / 2;
        int ty = r->y + (ch - core_font_metrics(p->app, p->font)->cell_h) / 2;
        core_draw_utf8(p, tx, ty, n->label, -1, t->box_fg, t->box_bg);
    }
}

/* Label centered in the rect and clipped to it (leaves the clip there). */
static void paint_centered_label(const struct paint* p, const struct gui_rect* r,
                                 const char* label, uint32_t fg, uint32_t bg)
{
    int ch = core_font_metrics(p->app, p->font)->cell_h;
    int lx = r->x + (r->w - core_utf8_width(p->app, p->font, label, -1)) / 2;
    if (lx < r->x)
        lx = r->x;
    /* the font's cell can be taller than the face; its bg must not spill out */
    gui_set_clip(p->frame, r->x, r->y, r->w, r->h);
    core_draw_utf8(p, lx, r->y + (r->h - ch) / 2, label, -1, fg, bg);
}

static void paint_button(const struct paint* p, const struct gui_node* n)
{
    const struct gui_app* app = p->app;
    const struct gui_theme* t = &app->theme;
    int hot = app->ui.hot == n, focused = app->ui.focused == n;
    if (n->tab)
    {
        /* A tab: flat, no shadow, no pressed shift. */
        uint32_t bg = n->tab == 2 ? t->btn_bg_active : (hot || focused) ? t->btn_bg_hot : t->btn_bg;
        gui_fill_rect(p->frame, n->rect.x, n->rect.y, n->rect.w, n->rect.h, bg);
        paint_centered_label(p, &n->rect, n->label, t->btn_fg, bg);
        return;
    }
    /* Pressed, the face shifts one cell right into the shadow's place. */
    int pressed = app->ui.active == n && hot;
    uint32_t bg = pressed ? t->btn_bg_active : (hot || focused) ? t->btn_bg_hot : t->btn_bg;
    if (!pressed)
        core_draw_shadow(p, &n->rect);
    struct gui_rect face = n->rect;
    if (pressed)
        face.x += core_layout_metrics(app, n)->cell_w;
    gui_fill_rect(p->frame, face.x, face.y, face.w, face.h, bg);
    paint_centered_label(p, &face, n->label, t->btn_fg, bg);
}

static void paint_input(const struct paint* p, const struct gui_node* n)
{
    const struct gui_app* app = p->app;
    const struct gui_theme* t = &app->theme;
    int cw = core_node_metrics(app, n)->cell_w, ch = core_node_metrics(app, n)->cell_h;
    int row_h = core_layout_metrics(app, n)->cell_h;
    int focused = app->ui.focused == n;
    uint32_t fg = focused ? t->input_fg_focus : t->input_fg;
    uint32_t bg = focused ? t->input_bg_focus : t->input_bg;
    int lo = 0, hi = 0;
    input_selection(n, &lo, &hi);
    int y = n->rect.y + (row_h - ch) / 2;
    if (focused)
        input_follow_caret(app, (struct gui_node*)n);   /* the scroll is the paint's: the box's size is known here */

    /* the text in one run, then again in the selection colors clipped to
     * the selection: the glyphs never move; the widget's clip cuts the text */
    gui_fill_rect(p->frame, n->rect.x, n->rect.y, n->rect.w, row_h, bg);
    int x = n->rect.x - input_offset(app, n);
    core_draw_utf8(p, x, y, n->value, -1, fg, bg);
    if (hi > lo)
    {
        int sel_x0 = x + core_utf8_width(app, p->font, n->value, lo);
        int sel_x1 = x + core_utf8_width(app, p->font, n->value, hi);
        if (sel_x0 < n->rect.x)
            sel_x0 = n->rect.x;
        if (sel_x1 > n->rect.x + n->rect.w)
            sel_x1 = n->rect.x + n->rect.w;
        if (sel_x1 > sel_x0)
        {
            gui_set_clip(p->frame, sel_x0, n->rect.y, sel_x1 - sel_x0, row_h);
            gui_fill_rect(p->frame, sel_x0, n->rect.y, sel_x1 - sel_x0, row_h, t->input_sel_bg);
            core_draw_utf8(p, x, y, n->value, -1, t->input_sel_fg, t->input_sel_bg);
            gui_set_clip(p->frame, n->rect.x, n->rect.y, n->rect.w, n->rect.h);
        }
    }
    /* The caret: a thin bar, a fifth of a cell, as in the old IDE, hidden
     * while there is a selection or in the blink's off half. */
    if (focused && lo == hi && !app->caret.off)
    {
        int caret_x = n->rect.x - input_offset(app, n) + core_utf8_width(app, p->font, n->value, n->cursor);
        int bar = cw / 5 > 0 ? cw / 5 : 1;
        gui_fill_rect(p->frame, caret_x, y, bar, ch, fg);
    }
}

static void paint_select(const struct paint* p, const struct gui_node* n)
{
    const struct gui_app* app = p->app;
    const struct gui_theme* t = &app->theme;
    int cw = core_layout_metrics(app, n)->cell_w, ch = core_layout_metrics(app, n)->cell_h;
    int focused = app->ui.focused == n;
    uint32_t fg = focused ? t->input_fg_focus : t->input_fg;
    uint32_t bg = focused ? t->input_bg_focus : t->input_bg;
    int text_w = n->rect.w > cw ? n->rect.w - cw : n->rect.w;
    gui_fill_rect(p->frame, n->rect.x, n->rect.y, text_w, ch, bg);
    if (n->selected >= 0 && n->selected < n->child_count)
    {
        /* the label, cut where the arrow's cell starts */
        const char* label = n->children[n->selected]->label;
        int y = n->rect.y + (ch - core_font_metrics(app, p->font)->cell_h) / 2;
        gui_set_clip(p->frame, n->rect.x, n->rect.y, text_w, ch);
        core_draw_utf8(p, n->rect.x, y, label, -1, fg, bg);
        gui_set_clip(p->frame, n->rect.x, n->rect.y, n->rect.w, n->rect.h);
    }
    if (text_w < n->rect.w)
    {
        int open = app->ui.open_select == n;
        uint32_t abg = (open || app->ui.hot == n || focused) ? t->btn_bg_hot : t->btn_bg;
        core_draw_utf8(p, n->rect.x + text_w, n->rect.y, "\xE2\x86\x93", -1, t->btn_fg, abg);  /* U+2193, drawn in the grid's cell */
    }
}

/* Rows a listbox/group shows. */
static int row_height(const struct gui_app* app, const struct gui_node* n)
{
    /* a group's rows are the layout's, to line up with the labels beside it */
    return n->kind == GUI_GROUP ? core_layout_metrics(app, n)->cell_h : core_node_metrics(app, n)->cell_h;
}

static int visible_rows(const struct gui_app* app, const struct gui_node* n)
{
    int rows = n->rect.h / row_height(app, n);
    return rows > 0 ? rows : 0;
}

static void clamp_scroll(const struct gui_app* app, struct gui_node* n)
{
    int max_scroll = n->child_count - visible_rows(app, n);
    if (n->scroll > max_scroll)
        n->scroll = max_scroll;
    if (n->scroll < 0)
        n->scroll = 0;
}

/* The box's vertical scrollbar: the whole height, the rows as items. */
static struct scrollbar list_scrollbar(const struct gui_app* app, const struct gui_node* n)
{
    struct scrollbar sb = { n->rect.h, n->child_count, visible_rows(app, n), n->scroll };
    return sb;
}

static int has_scrollbar(const struct gui_app* app, const struct gui_node* n)
{
    return n->child_count > visible_rows(app, n) && visible_rows(app, n) > 0;
}

static int on_scrollbar(const struct gui_app* app, const struct gui_node* n, int x)
{
    return has_scrollbar(app, n) && x >= n->rect.x + n->rect.w - app->scrollbar_px;
}

/* The scrollbar overlay along the right edge, only while the mouse is over
 * the box or its thumb is dragged - it never reserves space. */
static void paint_scrollbar(const struct paint* p, const struct gui_node* n)
{
    const struct gui_app* app = p->app;
    if (!has_scrollbar(app, n) || (app->ui.hot != n && app->ui.scrolling != n))
        return;
    struct scrollbar sb = list_scrollbar(app, n);
    int pos = 0, len = 0;
    core_scrollbar_thumb(app, &sb, &pos, &len);
    int w = app->scrollbar_px;
    int x = n->rect.x + n->rect.w - w;
    gui_fill_rect(p->frame, x, n->rect.y, w, n->rect.h, app->theme.scrollbar_bg);
    int hot = app->ui.scrolling == n || (app->ui.thumb_hot == n && app->ui.thumb_hot_bar == 1);
    gui_fill_rect(p->frame, x, n->rect.y + pos, w, len,
                  hot ? app->theme.scrollbar_thumb_hot_bg : app->theme.scrollbar_thumb_bg);
}

/* A listbox's widest label, in columns; in GUI_FONT_UI its measured width
 * in columns of cell_w, rounded up. */
static int list_max_cols(const struct gui_app* app, const struct gui_node* n)
{
    int max = 0;
    int proportional = core_node_font(n) == GUI_FONT_UI && app->canvas;
    int cw = core_node_metrics(app, n)->cell_w;
    for (int i = 0; i < n->child_count; i++)
    {
        int c = 0;
        if (proportional)
        {
            uint32_t run[256] ={0};
            int count = 0, width = 0;
            const char* s = n->children[i]->label;
            while (*s)
            {
                s += core_utf8_decode(s, &run[count++]);
                if (count == (int)(sizeof run / sizeof run[0]))
                {
                    width += gui_text_width(app->canvas, run, count, GUI_FONT_UI);
                    count = 0;
                }
            }
            width += gui_text_width(app->canvas, run, count, GUI_FONT_UI);
            c = (width + cw - 1) / cw;
        }
        else
        {
            c = core_utf8_cells(n->children[i]->label, -1);
        }
        if (c > max)
            max = c;
    }
    return max;
}

static int list_view_cols(const struct gui_app* app, const struct gui_node* n)
{
    return n->rect.w / core_node_metrics(app, n)->cell_w;
}

static int has_hbar(const struct gui_app* app, const struct gui_node* n)
{
    int cols = list_view_cols(app, n);
    return n->kind == GUI_LISTBOX && cols > 0 && list_max_cols(app, n) > cols;
}

static void clamp_hscroll(const struct gui_app* app, struct gui_node* n)
{
    int max_h = list_max_cols(app, n) - list_view_cols(app, n);
    if (n->hscroll > max_h)
        n->hscroll = max_h;
    if (n->hscroll < 0)
        n->hscroll = 0;
}

/* The box's horizontal scrollbar: along the bottom, short of the vertical one. */
static struct scrollbar list_hbar(const struct gui_app* app, const struct gui_node* n)
{
    int track = n->rect.w - (has_scrollbar(app, n) ? app->scrollbar_px : 0);
    struct scrollbar sb = { track, list_max_cols(app, n), list_view_cols(app, n), n->hscroll };
    return sb;
}

static int on_hbar(const struct gui_app* app, const struct gui_node* n, int x, int y)
{
    return has_hbar(app, n) && y >= n->rect.y + n->rect.h - app->scrollbar_px &&
           x < n->rect.x + list_hbar(app, n).track;
}

static void paint_hbar(const struct paint* p, const struct gui_node* n)
{
    const struct gui_app* app = p->app;
    if (!has_hbar(app, n) || (app->ui.hot != n && app->ui.hscrolling != n))
        return;
    struct scrollbar sb = list_hbar(app, n);
    int pos = 0, len = 0;
    core_scrollbar_thumb(app, &sb, &pos, &len);
    int w = app->scrollbar_px;
    int y = n->rect.y + n->rect.h - w;
    gui_fill_rect(p->frame, n->rect.x, y, sb.track, w, app->theme.scrollbar_bg);
    int hot = app->ui.hscrolling == n || (app->ui.thumb_hot == n && app->ui.thumb_hot_bar == 2);
    gui_fill_rect(p->frame, n->rect.x + pos, y, len, w,
                  hot ? app->theme.scrollbar_thumb_hot_bg : app->theme.scrollbar_thumb_bg);
}

/* The listbox scrollbar thumb at (x, y): 1 vertical, 2 horizontal, 0 none. */
static int list_thumb_at(const struct gui_app* app, const struct gui_node* n, int x, int y)
{
    int pos = 0, len = 0;
    if (on_scrollbar(app, n, x))
    {
        struct scrollbar sb = list_scrollbar(app, n);
        core_scrollbar_thumb(app, &sb, &pos, &len);
        return y >= n->rect.y + pos && y < n->rect.y + pos + len;
    }
    if (on_hbar(app, n, x, y))
    {
        struct scrollbar sb = list_hbar(app, n);
        core_scrollbar_thumb(app, &sb, &pos, &len);
        return x >= n->rect.x + pos && x < n->rect.x + pos + len ? 2 : 0;
    }
    return 0;
}

static void ensure_visible(const struct gui_app* app, struct gui_node* n, int index);

static void paint_listbox(const struct paint* p, const struct gui_node* n)
{
    const struct gui_app* app = p->app;
    if (n->reveal && p->app->canvas)
    {
        struct gui_node* m = (struct gui_node*)n;   /* the scroll follows a selection set from code */
        m->reveal = 0;
        if (m->selected >= 0)
            ensure_visible(app, m, m->selected);
    }
    clamp_hscroll(app, (struct gui_node*)n);   /* the box or its items may have changed */
    const struct gui_theme* t = &app->theme;
    int cw = core_node_metrics(app, n)->cell_w, ch = core_node_metrics(app, n)->cell_h;
    int focused = app->ui.focused == n;
    int cols = n->rect.w / cw;
    /* Rows are whole; the part of the box below the last one is just
     * background. */
    uint32_t body_fg = n->has_colors ? n->fg : t->listbox_fg;   /* a panel's list sets its own */
    uint32_t body_bg = n->has_colors ? n->bg : t->listbox_bg;
    gui_fill_rect(p->frame, n->rect.x, n->rect.y, n->rect.w, n->rect.h, body_bg);
    for (int row = 0; row < visible_rows(app, n); row++)
    {
        int index = n->scroll + row;
        int y = n->rect.y + row * ch;
        int sel = index < n->child_count && (n->multi ? n->children[index]->checked : index == n->selected);
        uint32_t fg = sel ? (focused ? t->listbox_sel_fg : t->listbox_sel_inactive_fg) : body_fg;
        uint32_t bg = sel ? (focused ? t->listbox_sel_bg : t->listbox_sel_inactive_bg) : body_bg;
        gui_fill_rect(p->frame, n->rect.x, y, n->rect.w, ch, bg);
        if (index < n->child_count)
        {
            const struct gui_node* item = n->children[index];
            const char* label = item->label;
            int x = n->rect.x;
            int room = cols;
            int skip = n->hscroll;
            if (core_node_font(n) == GUI_FONT_UI)
            {
                /* proportional: the whole label, shifted by hscroll columns; the widget's clip cuts it */
                room = INT_MAX;
                x -= skip * cw;
                skip = 0;
            }
            if (item->has_colors && label[0] && room > 0)
            {
                /* A file-type marker: its first glyph in its own color, selected or not. */
                uint32_t cp = 0;
                int len = core_utf8_decode(label, &cp);
                if (skip == 0)
                {
                    x = core_draw_utf8(p, x, y, label, len, item->fg, bg);
                    room--;
                }
                else
                {
                    skip--;
                }
                label += len;
            }
            label += core_utf8_prefix_bytes(label, skip);
            core_draw_utf8(p, x, y, label, core_utf8_prefix_bytes(label, room), fg, bg);
        }
    }
    paint_scrollbar(p, n);
    paint_hbar(p, n);
}

/* One row per item: "( )"/"(•)" for radios, "[ ]"/"[X]" for check boxes,
 * then the label with its first letter accented. The keyboard row shows in
 * the selection colors while the group has focus. Same as render_group. */
static void paint_group(const struct paint* p, const struct gui_node* n)
{
    const struct gui_app* app = p->app;
    const struct gui_theme* t = &app->theme;
    int ch = row_height(app, n);
    int text_dy = (ch - core_font_metrics(app, p->font)->cell_h) / 2;
    int focused = app->ui.focused == n;
    gui_fill_rect(p->frame, n->rect.x, n->rect.y, n->rect.w, n->rect.h, t->listbox_bg);
    for (int row = 0; row < visible_rows(app, n); row++)
    {
        int index = n->scroll + row;
        if (index >= n->child_count)
            break;
        const struct gui_node* item = n->children[index];
        int on = n->multi ? item->checked : index == n->selected;
        int is_focus = focused && index == n->cursor;
        uint32_t fg = is_focus ? t->listbox_sel_fg : t->listbox_fg;
        uint32_t bg = is_focus ? t->listbox_sel_bg : t->listbox_bg;
        int y = n->rect.y + row * ch;
        if (is_focus)
            gui_fill_rect(p->frame, n->rect.x, y, n->rect.w, ch, bg);
        /* the font's cell can be taller than the row; its bg must not spill into the next */
        gui_set_clip(p->frame, n->rect.x, y, n->rect.w, ch);
        const char* marker = n->multi ? (on ? "[X] " : "[ ] ")
                                      : (on ? "(\xE2\x80\xA2) " : "( ) ");   /* U+2022 bullet */
        core_draw_utf8(p, n->rect.x, y + text_dy, marker, -1, fg, bg);
        /* the labels in one column: "[ ]" and "( )" are narrower than "[X]" and "(•)" */
        int x = n->rect.x + core_utf8_width(app, p->font, n->multi ? "[X] " : "(\xE2\x80\xA2) ", -1);
        if (item->label[0])
        {
            int first = core_utf8_prefix_bytes(item->label, 1);
            x = core_draw_utf8(p, x, y + text_dy, item->label, first, is_focus ? fg : t->hotkey_key_fg, bg);
            core_draw_utf8(p, x, y + text_dy, item->label + first, -1, fg, bg);
        }
        gui_set_clip(p->frame, n->rect.x, n->rect.y, n->rect.w, n->rect.h);
    }
    paint_scrollbar(p, n);
}

void widget_paint(const struct paint* outer, const struct gui_node* n)
{
    struct paint q = *outer;   /* each widget draws in its own font */
    q.font = core_node_font(n);
    const struct paint* p = &q;
    /* Nothing a widget draws leaves its box (0 width: no box, no clip);
     * a button's box includes its shadow, one cell right and half a row
     * below. */
    if (n->rect.w > 0 && n->rect.h > 0)
    {
        int extra_w = 0, extra_h = 0;
        if (n->kind == GUI_BUTTON)
        {
            extra_w = p->app->ui_metrics.cell_w;
            extra_h = p->app->ui_metrics.cell_h / 2;
        }
        gui_set_clip(p->frame, n->rect.x, n->rect.y, n->rect.w + extra_w, n->rect.h + extra_h);
    }
    switch (n->kind)
    {
    case GUI_TEXT: paint_text(p, n); break;
    case GUI_BOX: paint_box(p, n); break;
    case GUI_BUTTON: paint_button(p, n); break;
    case GUI_INPUT: paint_input(p, n); break;
    case GUI_SELECT: paint_select(p, n); break;
    case GUI_LISTBOX: paint_listbox(p, n); break;
    case GUI_GROUP: paint_group(p, n); break;
    case GUI_EDITOR: editor_paint(p, n); break;
    default: break;
    }
    gui_set_clip(p->frame, 0, 0, 0, 0);
}

/* --- The open select list --- */

/* Directly under the control, at least as wide, one row per choice inside
 * a frame - the old IDE's layout_select_popup. Sets each item's rect. */
static struct gui_rect select_popup_layout(const struct gui_app* app, struct gui_node* select)
{
    /* the frame in the layout's cells, the rows in the select's font */
    int cw = core_layout_metrics(app, select)->cell_w, ch = core_layout_metrics(app, select)->cell_h;
    int item_h = core_node_metrics(app, select)->cell_h;
    int max_w = select->rect.w - 2 * cw;
    for (int i = 0; i < select->child_count; i++)
    {
        int w = core_utf8_width(app, core_node_font(select), select->children[i]->label, -1);
        if (w > max_w)
            max_w = w;
    }
    int inner_w = max_w + 2 * cw;
    struct gui_rect box = { select->rect.x, select->rect.y + ch, inner_w + 2 * cw,
                            select->child_count * item_h + 2 * ch };
    int max_h = app->h - (core_find_kind(app->root, GUI_STATUSBAR) ? ch : 0);
    if (box.x + box.w > app->w) box.x = app->w - box.w;
    if (box.x < 0) box.x = 0;
    if (box.y + box.h > max_h) box.y = max_h - box.h;
    if (box.y < 0) box.y = 0;
    for (int i = 0; i < select->child_count; i++)
    {
        struct gui_rect r = { box.x + cw, box.y + ch + i * item_h, inner_w, item_h };
        select->children[i]->rect = r;
    }
    return box;
}

void widget_paint_popups(const struct paint* p)
{
    struct gui_app* app = (struct gui_app*)p->app;
    struct gui_node* select = app->ui.open_select;
    if (!select)
        return;
    const struct gui_theme* t = &app->theme;
    int cw = core_layout_metrics(app, select)->cell_w;
    struct gui_rect box = select_popup_layout(app, select);
    struct paint q = *p;
    q.font = core_node_font(select);
    p = &q;
    core_draw_shadow(p, &box);
    core_draw_frame(p, &box, t->menu_border_style, t->menu_border_fg, t->menu_border_bg);
    for (int i = 0; i < select->child_count; i++)
    {
        const struct gui_node* it = select->children[i];
        int hot = app->ui.hot == it;
        uint32_t fg = hot ? t->menu_item_fg_hot : t->menu_item_fg;
        uint32_t bg = hot ? t->menu_item_bg_hot : t->menu_item_bg;
        gui_fill_rect(p->frame, it->rect.x, it->rect.y, it->rect.w, it->rect.h, bg);
        gui_set_clip(p->frame, it->rect.x, it->rect.y, it->rect.w, it->rect.h);
        core_draw_utf8(p, it->rect.x + cw, it->rect.y, it->label, -1, fg, bg);
        gui_set_clip(p->frame, 0, 0, 0, 0);
    }
}

int widget_popup_mouse_down(struct gui_app* app)
{
    struct gui_node* select = app->ui.open_select;
    if (!select)
        return 0;
    struct gui_rect box = select_popup_layout(app, select);
    app->needs_paint = 1;
    for (int i = 0; i < select->child_count; i++)
    {
        if (core_rect_contains(&select->children[i]->rect, app->mouse_x, app->mouse_y))
        {
            select->selected = i;
            app->ui.open_select = NULL;
            core_fire(app, select->children[i]->id);
            return 1;
        }
    }
    if (core_rect_contains(&box, app->mouse_x, app->mouse_y))
        return 1;   /* the frame: keep it open */
    app->ui.open_select = NULL;   /* a click anywhere else closes it */
    return core_rect_contains(&select->rect, app->mouse_x, app->mouse_y);
}

/* --- Mouse --- */

struct gui_node* widget_at(const struct gui_app* app, const struct gui_node* within, int x, int y)
{
    for (int i = within->child_count - 1; i >= 0; i--)
    {
        struct gui_node* c = within->children[i];
        if (!core_rect_contains(&c->rect, x, y))
            continue;
        if (c->kind == GUI_BOX)
        {
            struct gui_node* inner = widget_at(app, c, x, y);
            if (inner)
                return inner;
        }
        if (widget_is_widget(c))
            return c;
    }
    return NULL;
}

/* The caret position for a click at x in an input. */
static int input_byte_at(const struct gui_app* app, const struct gui_node* n, int x)
{
    /* the boundary nearest to x */
    int target = x - n->rect.x + input_offset(app, n);
    enum gui_font font = core_node_font(n);
    int at = 0, left = 0;
    while (n->value[at])
    {
        uint32_t cp = 0;
        int len = core_utf8_decode(n->value + at, &cp);
        int w = core_utf8_width(app, font, n->value + at, len);
        if (target < left + w / 2)
            break;
        left += w;
        at += len;
    }
    return at;
}

static void select_word(struct gui_node* n)
{
    const char* s = n->value;
    int lo = n->cursor, hi = n->cursor;
    while (lo > 0 && is_word_byte(s[lo - 1]))
        lo--;
    while (s[hi] && is_word_byte(s[hi]))
        hi++;
    n->anchor = lo;
    n->cursor = hi;
}

/* The listbox row under y, or -1. */
static int row_at(const struct gui_app* app, const struct gui_node* n, int y)
{
    int row = (y - n->rect.y) / row_height(app, n);
    int index = n->scroll + row;
    return row >= 0 && index < n->child_count ? index : -1;
}

/* A drag of the thumb: the point where it was grabbed stays under the mouse. */
static void scroll_to_mouse(struct gui_app* app, struct gui_node* n)
{
    struct scrollbar sb = list_scrollbar(app, n);
    n->scroll = core_scrollbar_drag(app, &sb, app->mouse_y - n->rect.y);
    clamp_scroll(app, n);
}

static void ensure_visible(const struct gui_app* app, struct gui_node* n, int index)
{
    int rows = visible_rows(app, n);
    if (index < n->scroll)
        n->scroll = index;
    else if (index >= n->scroll + rows)
        n->scroll = index - rows + 1;
    clamp_scroll(app, n);
}

/* A multi-select listbox (gui_set_multi): its picked rows are the checked
 * items. A click picks one row; Ctrl (Cmd) adds or removes it; Shift picks
 * the rows from the last one picked to it - the old IDE's. */
static void listbox_pick(struct gui_node* n, int index, int mods)
{
    if (mods & GUI_MOD_PRIMARY)
    {
        n->children[index]->checked = !n->children[index]->checked;
        n->cursor = index;
        return;
    }
    int from = (mods & GUI_MOD_SHIFT) && n->cursor >= 0 && n->cursor < n->child_count ? n->cursor : index;
    int lo = from < index ? from : index, hi = from < index ? index : from;
    for (int i = 0; i < n->child_count; i++)
        n->children[i]->checked = i >= lo && i <= hi;
    if (!(mods & GUI_MOD_SHIFT))
        n->cursor = index;
}

void widget_mouse_down(struct gui_app* app, struct gui_node* n, int double_click)
{
    if (!n->enabled)
        return;
    if (is_focusable(n))
        gui_focus(app, n);
    app->needs_paint = 1;
    int x = app->mouse_x, y = app->mouse_y;
    switch (n->kind)
    {
    case GUI_BUTTON:
        app->ui.active = n;
        break;
    case GUI_INPUT:
        n->cursor = input_byte_at(app, n, x);
        n->anchor = n->cursor;
        if (double_click)
            select_word(n);
        else
            app->ui.selecting = n;
        break;
    case GUI_SELECT:
        app->ui.open_select = app->ui.open_select == n ? NULL : n;
        break;
    case GUI_EDITOR:
        editor_mouse_down(app, n, double_click, app->mouse_mods);
        break;
    case GUI_LISTBOX:
    {
        if (on_scrollbar(app, n, x))
        {
            struct scrollbar sb = list_scrollbar(app, n);
            app->ui.scrolling = n;
            n->scroll = core_scrollbar_press(app, &sb, y - n->rect.y);
            clamp_scroll(app, n);
            break;
        }
        if (on_hbar(app, n, x, y))
        {
            struct scrollbar sb = list_hbar(app, n);
            app->ui.hscrolling = n;
            n->hscroll = core_scrollbar_press(app, &sb, x - n->rect.x);
            clamp_hscroll(app, n);
            break;
        }
        int index = row_at(app, n, y);
        if (index >= 0)
        {
            if (n->multi)
                listbox_pick(n, index, app->mouse_mods);
            n->selected = index;
            core_fire(app, n->children[index]->id);   /* the row's own id: picked */
            if (double_click)
                core_fire(app, n->id);
        }
        break;
    }
    case GUI_GROUP:
    {
        if (on_scrollbar(app, n, x))
        {
            struct scrollbar sb = list_scrollbar(app, n);
            app->ui.scrolling = n;
            n->scroll = core_scrollbar_press(app, &sb, y - n->rect.y);
            clamp_scroll(app, n);
            break;
        }
        int index = row_at(app, n, y);
        if (index < 0)
            break;
        n->cursor = index;
        if (n->multi)
            n->children[index]->checked = !n->children[index]->checked;
        else
            n->selected = index;
        break;
    }
    default:
        break;
    }
}

void widget_mouse_up(struct gui_app* app)
{
    struct gui_node* active = app->ui.active;
    app->ui.active = NULL;
    app->ui.selecting = NULL;
    if (app->ui.scrolling || app->ui.hscrolling)
    {
        app->ui.scrolling = NULL;
        app->ui.hscrolling = NULL;
        app->needs_paint = 1;
    }
    if (active)
    {
        app->needs_paint = 1;
        if (app->ui.hot == active)
            core_fire(app, active->id);
    }
}

void widget_mouse_move(struct gui_app* app)
{
    struct widget_state* ui = &app->ui;
    struct gui_node* old_hot = ui->hot;
    int x = app->mouse_x, y = app->mouse_y;

    struct gui_node* dragging = ui->selecting ? ui->selecting
                              : ui->scrolling ? ui->scrolling : ui->hscrolling;
    if (dragging && dragging->kind == GUI_EDITOR)
    {
        editor_mouse_drag(app, dragging);
        app->needs_paint = 1;
    }
    else if (ui->selecting)
    {
        ui->selecting->cursor = input_byte_at(app, ui->selecting, x);
        app->needs_paint = 1;
    }
    else if (ui->scrolling)
    {
        scroll_to_mouse(app, ui->scrolling);
        app->needs_paint = 1;
    }
    else if (ui->hscrolling)
    {
        struct scrollbar sb = list_hbar(app, ui->hscrolling);
        ui->hscrolling->hscroll = core_scrollbar_drag(app, &sb, x - ui->hscrolling->rect.x);
        clamp_hscroll(app, ui->hscrolling);
        app->needs_paint = 1;
    }

    ui->hot = NULL;
    if (ui->open_select)
    {
        select_popup_layout(app, ui->open_select);
        for (int i = 0; i < ui->open_select->child_count; i++)
        {
            if (core_rect_contains(&ui->open_select->children[i]->rect, x, y))
                ui->hot = ui->open_select->children[i];
        }
    }
    if (!ui->hot)
    {
        const struct gui_node* modal = core_top_modal(app);
        struct gui_node* win = core_window_at_point(app, x, y);
        if (!modal || win == modal)
            ui->hot = widget_at(app, win ? win : app->root, x, y);
    }
    if (ui->hot != old_hot)
        app->needs_paint = 1;

    /* The thumb under the mouse is drawn in its own color: repainted when that changes. */
    struct gui_node* thumb = NULL;
    int bar = 0;
    struct gui_node* h = ui->hot;
    if (h && h->kind == GUI_EDITOR)
        bar = editor_thumb_at(app, h, x, y);
    else if (h && (h->kind == GUI_LISTBOX || h->kind == GUI_GROUP))
        bar = list_thumb_at(app, h, x, y);
    if (bar)
        thumb = h;
    if (thumb != ui->thumb_hot || bar != ui->thumb_hot_bar)
    {
        ui->thumb_hot = thumb;
        ui->thumb_hot_bar = bar;
        app->needs_paint = 1;
    }
}

/* A notch (120 units) scrolls 3 rows, as in the old IDE. A touchpad sends
 * smaller steps: they add up, and every 40 units scroll one whole row - so
 * scrolling stays fluid without ever showing a row in part, and without any
 * animation. */
void widget_wheel(struct gui_app* app, int wheel)
{
    struct gui_node* n = app->ui.hot;
    if (!n || (n->kind != GUI_LISTBOX && n->kind != GUI_GROUP && n->kind != GUI_EDITOR))
    {
        app->ui.wheel_rest = 0;
        return;
    }
    app->ui.wheel_rest += wheel;
    int rows = app->ui.wheel_rest / 40;
    if (rows == 0)
        return;
    app->ui.wheel_rest -= rows * 40;
    if (n->kind == GUI_EDITOR)
    {
        editor_wheel(app, n, rows);
        app->needs_paint = 1;
        return;
    }
    int old = n->scroll;
    n->scroll -= rows;
    clamp_scroll(app, n);
    if (n->scroll != old)
        app->needs_paint = 1;
}

/* Sideways (a touchpad swipe, a tilt wheel, Shift+wheel): editors and
 * listboxes scroll by columns, 40 units each. */
void widget_hwheel(struct gui_app* app, int wheel)
{
    struct gui_node* n = app->ui.hot;
    if (!n || (n->kind != GUI_EDITOR && n->kind != GUI_LISTBOX))
    {
        app->ui.hwheel_rest = 0;
        return;
    }
    app->ui.hwheel_rest += wheel;
    int cols = app->ui.hwheel_rest / 40;
    if (cols == 0)
        return;
    app->ui.hwheel_rest -= cols * 40;
    if (n->kind == GUI_LISTBOX)
    {
        n->hscroll += cols;
        clamp_hscroll(app, n);
    }
    else
    {
        editor_hwheel(app, n, cols);
    }
    app->needs_paint = 1;
}

/* --- Keyboard --- */

static int input_key(struct gui_app* app, struct gui_node* n, int key, int mods)
{
    int shift = (mods & GUI_MOD_SHIFT) != 0;
    int ctrl = (mods & GUI_MOD_PRIMARY) != 0;   /* Command on macOS */
    int word = (mods & GUI_MOD_WORD) != 0;
    int lo = 0, hi = 0;
    input_selection(n, &lo, &hi);
#ifdef __APPLE__
    /* Command+arrows: the line's ends; Command+Backspace: to the start - the Mac's */
    if (mods & GUI_MOD_CMD)
    {
        if (key == GUI_KEY_LEFT || key == GUI_KEY_UP) key = GUI_KEY_HOME;
        else if (key == GUI_KEY_RIGHT || key == GUI_KEY_DOWN) key = GUI_KEY_END;
        else if (key == GUI_KEY_BACKSPACE && lo == hi)
        {
            input_replace(n, 0, hi, "", 0);
            return 1;
        }
    }
#endif
    switch (key)
    {
    case GUI_KEY_LEFT:
        n->cursor = (!shift && lo != hi) ? lo : word ? input_word_left(n->value, n->cursor) : prev_char(n->value, n->cursor);
        break;
    case GUI_KEY_RIGHT:
        n->cursor = (!shift && lo != hi) ? hi : word ? input_word_right(n->value, n->cursor) : next_char(n->value, n->cursor);
        break;
    case GUI_KEY_HOME:
        n->cursor = 0;
        break;
    case GUI_KEY_END:
        n->cursor = (int)strlen(n->value);
        break;
    case GUI_KEY_BACKSPACE:
        if (lo == hi)
            lo = word ? input_word_left(n->value, lo) : prev_char(n->value, lo);
        input_replace(n, lo, hi, "", 0);
        return 1;
    case GUI_KEY_DELETE:
        if (lo == hi)
            hi = word ? input_word_right(n->value, hi) : next_char(n->value, hi);
        input_replace(n, lo, hi, "", 0);
        return 1;
    case GUI_KEY_ENTER:
        core_fire(app, n->id);
        return 1;
    case 'A':
        if (!ctrl)
            return 0;
        n->anchor = 0;
        n->cursor = (int)strlen(n->value);
        return 1;
    case 'C':
    case 'X':
        if (!ctrl)
            return 0;
        if (lo != hi)
        {
            char* s = malloc((size_t)(hi - lo) + 1);
            if (!s)
                abort();
            memcpy(s, n->value + lo, (size_t)(hi - lo));
            s[hi - lo] = '\0';
            gui_clipboard_set(app->canvas, s);
            free(s);
            if (key == 'X')
                input_replace(n, lo, hi, "", 0);
        }
        return 1;
    case 'V':
    {
        if (!ctrl)
            return 0;
        char* s = gui_clipboard_get(app->canvas);
        if (s)
        {
            /* single line: only up to the first line break */
            int len = (int)strcspn(s, "\r\n");
            input_replace(n, lo, hi, s, len);
            free(s);
        }
        return 1;
    }
    default:
        return 0;
    }
    if (!shift)
        n->anchor = n->cursor;
    return 1;
}

static int list_key(struct gui_app* app, struct gui_node* n, int key, int mods)
{
    int rows = visible_rows(app, n);
    int index = n->kind == GUI_GROUP ? n->cursor : n->selected;
    switch (key)
    {
    case GUI_KEY_UP: index--; break;
    case GUI_KEY_DOWN: index++; break;
    case GUI_KEY_PAGEUP: index -= rows > 1 ? rows - 1 : 1; break;
    case GUI_KEY_PAGEDOWN: index += rows > 1 ? rows - 1 : 1; break;
    case GUI_KEY_HOME: index = 0; break;
    case GUI_KEY_END: index = n->child_count - 1; break;
    case GUI_KEY_ENTER:
        if (n->kind == GUI_LISTBOX)
        {
            core_fire(app, n->id);
            return 1;
        }
        return 0;
    case ' ':
        if (n->kind != GUI_GROUP || n->cursor < 0 || n->cursor >= n->child_count)
            return 0;
        if (n->multi)
            n->children[n->cursor]->checked = !n->children[n->cursor]->checked;
        else
            n->selected = n->cursor;
        return 1;
    default:
        return 0;
    }
    if (index > n->child_count - 1) index = n->child_count - 1;
    if (index < 0) index = 0;
    if (n->kind == GUI_GROUP)
        n->cursor = index;
    else
    {
        if (n->multi)
            listbox_pick(n, index, mods & GUI_MOD_SHIFT);
        n->selected = index;
        if (index >= 0 && index < n->child_count)
            core_fire(app, n->children[index]->id);   /* as a click on the row */
    }
    ensure_visible(app, n, index);
    return 1;
}

int widget_key(struct gui_app* app, int key, int mods)
{
    struct widget_state* ui = &app->ui;
    if (ui->open_select && key == GUI_KEY_ESCAPE)
    {
        ui->open_select = NULL;
        app->needs_paint = 1;
        return 1;
    }
    if (key == GUI_KEY_TAB && !(mods & (GUI_MOD_CTRL | GUI_MOD_ALT | GUI_MOD_CMD)) &&
        !(ui->focused && ui->focused->kind == GUI_EDITOR))   /* Tab indents there */
    {
        focus_step(app, !(mods & GUI_MOD_SHIFT));
        return 1;
    }
    struct gui_node* n = ui->focused;
    if (!n || !n->enabled)
        return 0;
    int handled = 0;
    switch (n->kind)
    {
    case GUI_BUTTON:
        if (key == GUI_KEY_ENTER || key == ' ')
        {
            core_fire(app, n->id);
            handled = 1;
        }
        break;
    case GUI_INPUT:
        handled = input_key(app, n, key, mods);
        break;
    case GUI_SELECT:
        if (key == GUI_KEY_ENTER || key == ' ')
        {
            ui->open_select = ui->open_select == n ? NULL : n;
            handled = 1;
        }
        else if (key == GUI_KEY_UP || key == GUI_KEY_DOWN)
        {
            int index = n->selected + (key == GUI_KEY_DOWN ? 1 : -1);
            if (index >= 0 && index < n->child_count)
            {
                n->selected = index;
                core_fire(app, n->children[index]->id);   /* as a pick with the mouse */
            }
            handled = 1;
        }
        break;
    case GUI_LISTBOX:
    case GUI_GROUP:
        handled = list_key(app, n, key, mods);
        break;
    case GUI_EDITOR:
        handled = editor_key(app, n, key, mods);
        break;
    default:
        break;
    }
    if (handled)
        app->needs_paint = 1;
    return handled;
}

/* A letter typed in a listbox: the next row (after the selected one,
 * wrapping) whose text starts with it, case-insensitive; a file-type marker
 * (an item with colors) and leading blanks are skipped - the old IDE's. */
static void listbox_type_ahead(struct gui_app* app, struct gui_node* n, uint32_t ch)
{
    if (n->child_count == 0)
        return;
    if (ch >= 'a' && ch <= 'z')
        ch -= 'a' - 'A';
    for (int attempt = 0; attempt < n->child_count; attempt++)
    {
        int index = (n->selected + 1 + attempt) % n->child_count;
        const struct gui_node* item = n->children[index];
        const char* text = item->label;
        if (item->has_colors && *text)
        {
            uint32_t cp = 0;
            text += core_utf8_decode(text, &cp);
        }
        while (*text == ' ' || *text == '\t')
            text++;
        char first = *text;
        if (first >= 'a' && first <= 'z')
            first -= 'a' - 'A';
        if ((uint32_t)(unsigned char)first == ch)
        {
            n->selected = index;
            ensure_visible(app, n, index);
            app->needs_paint = 1;
            return;
        }
    }
}

void widget_char(struct gui_app* app, uint32_t ch)
{
    struct gui_node* n = app->ui.focused;
    if (n && n->kind == GUI_EDITOR && n->enabled)
    {
        editor_char(app, n, ch);
        return;
    }
    if (n && n->kind == GUI_LISTBOX && n->enabled && ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')))
    {
        listbox_type_ahead(app, n, ch);
        return;
    }
    if (!n || n->kind != GUI_INPUT || !n->enabled)
        return;
    char buf[4] = { 0 };
    int len = 0;
    if (ch < 0x80) { buf[0] = (char)ch; len = 1; }
    else if (ch < 0x800) { buf[0] = (char)(0xC0 | (ch >> 6)); buf[1] = (char)(0x80 | (ch & 0x3F)); len = 2; }
    else if (ch < 0x10000)
    {
        buf[0] = (char)(0xE0 | (ch >> 12));
        buf[1] = (char)(0x80 | ((ch >> 6) & 0x3F));
        buf[2] = (char)(0x80 | (ch & 0x3F));
        len = 3;
    }
    else
    {
        buf[0] = (char)(0xF0 | (ch >> 18));
        buf[1] = (char)(0x80 | ((ch >> 12) & 0x3F));
        buf[2] = (char)(0x80 | ((ch >> 6) & 0x3F));
        buf[3] = (char)(0x80 | (ch & 0x3F));
        len = 4;
    }
    int lo = 0, hi = 0;
    input_selection(n, &lo, &hi);
    input_replace(n, lo, hi, buf, len);
    app->needs_paint = 1;
}
