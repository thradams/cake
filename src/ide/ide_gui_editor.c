/* ide_gui_editor.c - GUI_EDITOR, the framework's multi-line text editor.
 *
 * The text is one UTF-8 buffer with '\n' line ends plus an index of where
 * each line starts, rebuilt after every edit. The caret and the selection
 * are byte offsets. Scrolling is by whole lines and columns - nothing is
 * ever shown in part, and nothing animates. Colors come from a pluggable
 * highlighter (gui_editor_set_highlighter); the editor keeps the state each
 * line starts in, and only recomputes from the first edited line on.
 *
 * Behavior follows the old IDE's <editor> (ide_ui.c): caret movement,
 * selection, indentation with spaces, undo/redo with typing grouped, the
 * line-number gutter, the hover-only scrollbars.
 */
#include "ide_gui_internal.h"
#include <stdlib.h>
#include <string.h>

#define TAB_SIZE 4   /* a '\t' advances to the next multiple; indentation is spaces */

/* One change: at `pos`, `removed` was replaced by `inserted`. Steps with the
 * same `group` undo and redo together (an indent of many lines is one). */
struct undo_step
{
    int pos;
    char* removed;
    int removed_len;
    char* inserted;
    int inserted_len;
    int cursor_before, anchor_before;
    int group;
};

struct undo_list
{
    struct undo_step* items;
    int count, cap;
};

struct mark
{
    enum gui_mark type;
    int line;      /* 0-based */
    char* text;
    int cols;
};

/* Sorted by line; a line's marks in the order added. */
struct mark_list
{
    struct mark* items;
    int count, cap;
};

/* Sorted 0-based line numbers. */
struct line_list
{
    int* items;
    int count, cap;
};

struct editor_data
{
    char* text;            /* NUL-terminated */
    int len, cap;
    int* line_starts;      /* byte offset of each line */
    int line_count;
    int line_cap;
    int max_cols;          /* the widest line, in columns - for the horizontal bar */

    int* states;           /* states[i]: the highlighter state line i starts in */
    int states_valid;      /* states[0 .. states_valid-1] are known */
    int states_cap;
    struct gui_highlighter highlighter;
    int has_highlighter;

    int cursor, anchor;    /* byte offsets; equal = no selection */
    int want_col;          /* the column Up/Down keep, -1 for the caret's own */
    int scroll;            /* first visible line */
    int hscroll;           /* first visible column */

    int line_numbers;
    int read_only;
    int vt100;             /* see gui_editor_set_vt100 */
    int diff;              /* see gui_editor_set_diff */
    int click_id;          /* see gui_editor_set_click_id */
    int dirty;
    int reveal;            /* bring the caret into view on the next paint */
    int center;            /* with reveal: the caret's line in the middle */
    struct mark_list marks;
    struct line_list breakpoints;
    int exec_line;         /* 0-based; -1 none */

    struct undo_list undo, redo;
    int group;             /* the next undo group */
    int typing;            /* the last step was typing that the next char may join */

    /* Scratch for painting one line: a color per byte. */
    uint32_t* line_fg;
    /* read-only: which bytes of `hidden_line` the highlighter hides
     * (GUI_SPAN_HIDDEN) - they take no column, like a VT100 escape */
    unsigned char* hidden;
    int hidden_cap;
    int hidden_line;       /* -1: not computed */
    int line_fg_cap;
};

/* --- Text and lines --- */

/* The length of the "\x1b[...<final>" sequence at text[k] in a VT100
 * editor (it takes no column), else 0. */
static int vt100_escape_len(const struct editor_data* e, int k, int end)
{
    if (!e->vt100 || e->text[k] != '\x1b' || k + 1 >= end || e->text[k + 1] != '[')
        return 0;
    int i = k + 2;
    while (i < end && (e->text[i] < 0x40 || e->text[i] > 0x7E))
        i++;
    return i < end ? i + 1 - k : 0;
}

static int line_of(const struct editor_data* e, int pos);
static int line_end(const struct editor_data* e, int line);
static void ensure_states(struct editor_data* e, int line);

/* The run of hidden bytes at text[k] (a read-only Markdown document's
 * delimiters, say), else 0. */
static int hidden_len(const struct editor_data* ce, int k, int end)
{
    struct editor_data* e = (struct editor_data*)ce;   /* the mask is a cache */
    if (!e->read_only || !e->has_highlighter || e->line_count == 0)
        return 0;
    int line = line_of(e, k);
    int start = e->line_starts[line];
    if (e->hidden_line != line)
    {
        int len = line_end(e, line) - start;
        if (len + 1 > e->hidden_cap)
        {
            e->hidden_cap = (len + 1) * 2;
            unsigned char* h = realloc(e->hidden, (size_t)e->hidden_cap);
            if (!h)
                abort();
            e->hidden = h;
        }
        memset(e->hidden, 0, (size_t)len + 1);
        ensure_states(e, line);
        int state = e->states[line];
        struct gui_span spans[256];
        int count = e->highlighter.highlight(e->highlighter.ctx, e->text + start, len, &state, spans, 256);
        for (int s = 0; s < count; s++)
        {
            if (!(spans[s].fg & GUI_SPAN_HIDDEN))
                continue;
            for (int i = spans[s].start; i < spans[s].start + spans[s].len && i < len; i++)
                if (i >= 0)
                    e->hidden[i] = 1;
        }
        e->hidden_line = line;
    }
    int n = 0;
    while (k + n < end && e->hidden[k + n - start])
        n++;
    return n;
}

/* Bytes that take no column: a VT100 escape, or hidden ones. */
static int escape_len(const struct editor_data* e, int k, int end)
{
    int n = vt100_escape_len(e, k, end);
    return n ? n : hidden_len(e, k, end);
}

static void rebuild_lines(struct editor_data* e)
{
    e->hidden_line = -1;
    e->line_count = 0;
    e->max_cols = 0;
    int start = 0;
    int mark = 0;   /* walks the marks along with the lines */
    for (int i = 0; ; i++)
    {
        if (e->text[i] == '\n' || e->text[i] == '\0')
        {
            if (e->line_count == e->line_cap)
            {
                e->line_cap = e->line_cap ? e->line_cap * 2 : 256;
                int* starts = realloc(e->line_starts, (size_t)e->line_cap * sizeof *starts);
                if (!starts)
                    abort();
                e->line_starts = starts;
            }
            e->line_starts[e->line_count++] = start;
            /* Columns of this line, counting tab stops. */
            int cols = 0;
            for (int k = start; k < i; k++)
            {
                int esc = vt100_escape_len(e, k, i);
                if (esc)
                    k += esc - 1;
                else if (e->text[k] == '\t')
                    cols = (cols / TAB_SIZE + 1) * TAB_SIZE;
                else if (((unsigned char)e->text[k] & 0xC0) != 0x80)
                    cols++;
            }
            /* A line's marks are part of its width, so the view can scroll to their end. */
            while (mark < e->marks.count && e->marks.items[mark].line < e->line_count - 1)
                mark++;
            while (mark < e->marks.count && e->marks.items[mark].line == e->line_count - 1)
                cols += e->marks.items[mark++].cols;
            if (cols > e->max_cols)
                e->max_cols = cols;
            if (e->text[i] == '\0')
                break;
            start = i + 1;
        }
    }
}

/* The columns of text[start, end), counting tab stops - a line's width. */
static int span_cols(const struct editor_data* e, int start, int end)
{
    int cols = 0;
    for (int k = start; k < end; k++)
    {
        int esc = vt100_escape_len(e, k, end);
        if (esc)
            k += esc - 1;
        else if (e->text[k] == '\t')
            cols = (cols / TAB_SIZE + 1) * TAB_SIZE;
        else if (((unsigned char)e->text[k] & 0xC0) != 0x80)
            cols++;
    }
    return cols;
}

/* After an edit that replaced old lines first..last: those lines read again
 * from the new text, the ones after shifted by `delta` bytes - an edit costs
 * the lines it touches, not the whole file. (No marks: an edit drops them.) */
static void update_lines(struct editor_data* e, int first, int last, int end_pos, int delta)
{
    int start = e->line_starts[first];
    /* the new lines: from the first one's start to the end of the line holding end_pos */
    int stop = end_pos;
    while (e->text[stop] != '\n' && e->text[stop] != '\0')
        stop++;
    int added = 1;
    for (int i = start; i < stop; i++)
        added += e->text[i] == '\n';
    int removed = last - first + 1;
    int count = e->line_count - removed + added;
    if (count > e->line_cap)
    {
        e->line_cap = count * 2;
        int* starts = realloc(e->line_starts, (size_t)e->line_cap * sizeof *starts);
        if (!starts)
            abort();
        e->line_starts = starts;
    }
    memmove(&e->line_starts[first + added], &e->line_starts[last + 1],
            (size_t)(e->line_count - last - 1) * sizeof *e->line_starts);
    for (int i = first + added; i < count; i++)
        e->line_starts[i] += delta;
    int line = first;
    e->line_starts[line++] = start;
    for (int i = start; i < stop; i++)
    {
        if (e->text[i] == '\n')
        {
            int cols = span_cols(e, e->line_starts[line - 1], i);
            if (cols > e->max_cols)
                e->max_cols = cols;
            e->line_starts[line++] = i + 1;
        }
    }
    int cols = span_cols(e, e->line_starts[line - 1], stop);
    if (cols > e->max_cols)
        e->max_cols = cols;
    e->line_count = count;
    e->hidden_line = -1;
}

static int line_end(const struct editor_data* e, int line)
{
    return line + 1 < e->line_count ? e->line_starts[line + 1] - 1 : e->len;
}

static int line_of(const struct editor_data* e, int pos)
{
    int lo = 0, hi = e->line_count - 1;
    while (lo < hi)
    {
        int mid = (lo + hi + 1) / 2;
        if (e->line_starts[mid] <= pos)
            lo = mid;
        else
            hi = mid - 1;
    }
    return lo;
}

/* The column of byte `pos` in `line`, counting tab stops. */
static int col_of(const struct editor_data* e, int line, int pos)
{
    int cols = 0;
    int end = line_end(e, line);
    for (int k = e->line_starts[line]; k < pos; k++)
    {
        int esc = escape_len(e, k, end);
        if (esc)
            k += esc - 1;
        else if (e->text[k] == '\t')
            cols = (cols / TAB_SIZE + 1) * TAB_SIZE;
        else if (((unsigned char)e->text[k] & 0xC0) != 0x80)
            cols++;
    }
    return cols;
}

/* The byte in `line` at column `col` (or the line's end). */
static int pos_of_col(const struct editor_data* e, int line, int col)
{
    int end = line_end(e, line);
    int cols = 0;
    int k = e->line_starts[line];
    while (k < end)
    {
        int esc = escape_len(e, k, end);
        if (esc)
        {
            k += esc;
            continue;
        }
        int next = e->text[k] == '\t' ? (cols / TAB_SIZE + 1) * TAB_SIZE : cols + 1;
        if (next > col)
            break;
        cols = next;
        k++;
        while (k < end && ((unsigned char)e->text[k] & 0xC0) == 0x80)
            k++;
    }
    return k;
}

static int prev_char(const struct editor_data* e, int pos)
{
    if (pos <= 0)
        return 0;
    pos--;
    while (pos > 0 && ((unsigned char)e->text[pos] & 0xC0) == 0x80)
        pos--;
    return pos;
}

static int next_char(const struct editor_data* e, int pos)
{
    if (pos >= e->len)
        return e->len;
    pos++;
    while (pos < e->len && ((unsigned char)e->text[pos] & 0xC0) == 0x80)
        pos++;
    return pos;
}

static int is_word_byte(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
           c == '_' || (unsigned char)c >= 0x80;
}

/* Ctrl+Left/Right: over spaces, then over one run of word or other bytes. */
static int word_left(const struct editor_data* e, int pos)
{
    while (pos > 0 && (e->text[pos - 1] == ' ' || e->text[pos - 1] == '\t'))
        pos--;
    if (pos > 0 && e->text[pos - 1] == '\n')
        return pos - 1;
    int word = pos > 0 && is_word_byte(e->text[pos - 1]);
    while (pos > 0 && e->text[pos - 1] != '\n' && e->text[pos - 1] != ' ' &&
           e->text[pos - 1] != '\t' && is_word_byte(e->text[pos - 1]) == word)
        pos--;
    return pos;
}

static int word_right(const struct editor_data* e, int pos)
{
    if (pos < e->len && e->text[pos] == '\n')
        return pos + 1;
    int word = pos < e->len && is_word_byte(e->text[pos]);
    while (pos < e->len && e->text[pos] != '\n' && e->text[pos] != ' ' &&
           e->text[pos] != '\t' && is_word_byte(e->text[pos]) == word)
        pos++;
    while (pos < e->len && (e->text[pos] == ' ' || e->text[pos] == '\t'))
        pos++;
    return pos;
}

static void selection(const struct editor_data* e, int* lo, int* hi)
{
    *lo = e->cursor < e->anchor ? e->cursor : e->anchor;
    *hi = e->cursor < e->anchor ? e->anchor : e->cursor;
}

/* --- Editing and undo --- */

static char* copy_bytes(const char* s, int len)
{
    char* copy = malloc((size_t)len + 1);
    if (!copy)
        abort();
    memcpy(copy, s, (size_t)len);
    copy[len] = '\0';
    return copy;
}

static void free_steps(struct undo_list* list)
{
    for (int i = 0; i < list->count; i++)
    {
        free(list->items[i].removed);
        free(list->items[i].inserted);
    }
    list->count = 0;
}

static void push_step(struct undo_list* list, const struct undo_step* step)
{
    if (list->count == list->cap)
    {
        list->cap = list->cap ? list->cap * 2 : 64;
        struct undo_step* items = realloc(list->items, (size_t)list->cap * sizeof *items);
        if (!items)
            abort();
        list->items = items;
    }
    list->items[list->count++] = *step;
}

static void free_marks(struct mark_list* l)
{
    for (int i = 0; i < l->count; i++)
        free(l->items[i].text);
    l->count = 0;
}

/* Replaces bytes [pos, pos + remove_len) with `insert` - the one place the
 * text changes. The caret is not touched here. The marks go: they were
 * about the text before the edit. */
static void splice(struct editor_data* e, int pos, int remove_len, const char* insert, int insert_len)
{
    free_marks(&e->marks);
    int first = line_of(e, pos), last = line_of(e, pos + remove_len);   /* the lines it touches */
    int new_len = e->len - remove_len + insert_len;
    if (new_len + 1 > e->cap)
    {
        e->cap = (new_len + 1) * 2;
        char* text = realloc(e->text, (size_t)e->cap);
        if (!text)
            abort();
        e->text = text;
    }
    memmove(e->text + pos + insert_len, e->text + pos + remove_len,
            (size_t)(e->len - pos - remove_len) + 1);
    memcpy(e->text + pos, insert, (size_t)insert_len);
    e->len = new_len;
    if (e->states_valid > first + 1)
        e->states_valid = first + 1;   /* lines after the edit may start differently */
    update_lines(e, first, last, pos + insert_len, insert_len - remove_len);
    e->dirty = 1;
}

/* An edit made by the user: recorded for undo (in the current group),
 * clears redo. */
static void edit(struct editor_data* e, int pos, int remove_len, const char* insert, int insert_len)
{
    struct undo_step step = { pos, copy_bytes(e->text + pos, remove_len), remove_len,
                              copy_bytes(insert, insert_len), insert_len,
                              e->cursor, e->anchor, e->group };
    push_step(&e->undo, &step);
    free_steps(&e->redo);
    splice(e, pos, remove_len, insert, insert_len);
}

/* Replaces the selection (or inserts at the caret) as one undo group. */
static void replace_selection(struct editor_data* e, const char* insert, int insert_len)
{
    int lo, hi;
    selection(e, &lo, &hi);
    e->group++;
    edit(e, lo, hi - lo, insert, insert_len);
    e->cursor = e->anchor = lo + insert_len;
    e->typing = 0;
}

/* A typed character joins the previous one in the same undo step while
 * typing continues on one line. */
static void type_text(struct editor_data* e, const char* s, int len)
{
    int lo, hi;
    selection(e, &lo, &hi);
    struct undo_step* last = e->undo.count ? &e->undo.items[e->undo.count - 1] : NULL;
    if (e->typing && lo == hi && last && last->removed_len == 0 &&
        last->pos + last->inserted_len == lo && s[0] != '\n')
    {
        char* inserted = realloc(last->inserted, (size_t)(last->inserted_len + len) + 1);
        if (!inserted)
            abort();
        memcpy(inserted + last->inserted_len, s, (size_t)len);
        last->inserted_len += len;
        inserted[last->inserted_len] = '\0';
        last->inserted = inserted;
        free_steps(&e->redo);
        splice(e, lo, 0, s, len);
        e->cursor = e->anchor = lo + len;
        return;
    }
    replace_selection(e, s, len);
    e->typing = s[0] != '\n';
}

static void undo_or_redo(struct editor_data* e, int redo)
{
    struct undo_list* from = redo ? &e->redo : &e->undo;
    struct undo_list* to = redo ? &e->undo : &e->redo;
    if (from->count == 0)
        return;
    int group = from->items[from->count - 1].group;
    while (from->count > 0 && from->items[from->count - 1].group == group)
    {
        struct undo_step step = from->items[--from->count];
        if (redo)
        {
            splice(e, step.pos, step.removed_len, step.inserted, step.inserted_len);
            e->cursor = e->anchor = step.pos + step.inserted_len;
        }
        else
        {
            splice(e, step.pos, step.inserted_len, step.removed, step.removed_len);
            e->cursor = step.cursor_before;
            e->anchor = step.anchor_before;
        }
        push_step(to, &step);
    }
    e->typing = 0;
}

/* --- Lifetime and values --- */

void editor_create(struct gui_node* n)
{
    struct editor_data* e = core_calloc(1, sizeof *e);
    e->text = core_strdup("");
    e->cap = 1;
    e->want_col = -1;
    e->states_valid = 1;
    e->exec_line = -1;
    rebuild_lines(e);
    n->editor = e;
}

void editor_free(struct gui_node* n)
{
    struct editor_data* e = n->editor;
    if (!e)
        return;
    free_steps(&e->undo);
    free_steps(&e->redo);
    free(e->undo.items);
    free(e->redo.items);
    free(e->text);
    free(e->line_starts);
    free(e->states);
    free(e->line_fg);
    free(e->hidden);
    free_marks(&e->marks);
    free(e->marks.items);
    free(e->breakpoints.items);
    free(e);
    n->editor = NULL;
}

void editor_set_text(struct gui_node* n, const char* utf8)
{
    struct editor_data* e = n->editor;
    free(e->text);
    e->len = (int)strlen(utf8);
    e->cap = e->len + 1;
    e->text = copy_bytes(utf8, e->len);
    free_steps(&e->undo);
    free_steps(&e->redo);
    e->cursor = e->anchor = 0;
    e->scroll = e->hscroll = 0;
    e->want_col = -1;
    e->states_valid = 1;
    e->dirty = 0;
    e->typing = 0;
    rebuild_lines(e);
}

const char* editor_get_text(const struct gui_node* n)
{
    return n->editor->text;
}

void gui_editor_set_highlighter(struct gui_node* ed, const struct gui_highlighter* h)
{
    struct editor_data* e = ed->editor;
    e->has_highlighter = h != NULL;
    if (h)
        e->highlighter = *h;
    e->states_valid = 1;
    e->hidden_line = -1;
}

void gui_editor_set_line_numbers(struct gui_node* ed, int on)
{
    ed->editor->line_numbers = on != 0;
}

void gui_editor_set_read_only(struct gui_node* ed, int read_only)
{
    ed->editor->read_only = read_only != 0;
    ed->editor->hidden_line = -1;
}

void gui_editor_set_click_id(struct gui_node* ed, int id)
{
    ed->editor->click_id = id;
}

int gui_editor_get_read_only(const struct gui_node* ed)
{
    return ed->editor->read_only;
}

void gui_editor_set_diff(struct gui_node* ed, int on)
{
    ed->editor->diff = on != 0;
}

/* A diff's "+" / "-" row, not its "+++" / "---" file header: '+', '-' or 0. */
static char diff_row(const struct editor_data* e, int line)
{
    int start = e->line_starts[line];
    int end = line + 1 < e->line_count ? e->line_starts[line + 1] - 1 : e->len;
    char c = start < end ? e->text[start] : 0;
    if (c != '+' && c != '-')
        return 0;
    if (end - start >= 3 && e->text[start + 1] == c && e->text[start + 2] == c)
        return 0;
    return c;
}

void gui_editor_set_vt100(struct gui_node* ed, int on)
{
    ed->editor->vt100 = on != 0;
    rebuild_lines(ed->editor);
}

int gui_editor_get_dirty(const struct gui_node* ed)
{
    return ed->editor->dirty;
}

void gui_editor_set_dirty(struct gui_node* ed, int dirty)
{
    ed->editor->dirty = dirty != 0;
}

void gui_editor_get_caret(const struct gui_node* ed, int* line, int* col)
{
    const struct editor_data* e = ed->editor;
    int l = line_of(e, e->cursor);
    if (line) *line = l + 1;
    if (col) *col = col_of(e, l, e->cursor) + 1;
}

/* --- Geometry --- */

/* Line numbers take max(3, digits) columns plus a gap column; without them
 * a one-column margin keeps text off the left edge - as in the old IDE. */
static int gutter_cols(const struct editor_data* e)
{
    if (!e->line_numbers)
        return 1;
    int digits = 1;
    for (int n = e->line_count; n >= 10; n /= 10)
        digits++;
    return (digits < 3 ? 3 : digits) + 1;
}

int gui_editor_word_at_mouse(const struct gui_app* app, const struct gui_node* ed,
                             char* buf, int cap, int* x, int* y)
{
    const struct editor_data* e = ed->editor;
    if (!e || cap <= 0 || !core_rect_contains(&ed->rect, app->mouse_x, app->mouse_y))
        return 0;
    const struct gui_metrics* m = core_node_metrics(app, ed);
    int col = (app->mouse_x - ed->rect.x) / m->cell_w - gutter_cols(e) + e->hscroll;
    int line = e->scroll + (app->mouse_y - ed->rect.y) / m->cell_h;
    if (col < 0 || line >= e->line_count)
        return 0;
    int k = pos_of_col(e, line, col);
    if (k >= e->len || !is_word_byte(e->text[k]) || col_of(e, line, k) != col)
        return 0;   /* past the line's end, or not on a word */
    int lo = k, hi = k;
    while (lo > 0 && is_word_byte(e->text[lo - 1]))
        lo--;
    while (hi < e->len && is_word_byte(e->text[hi]))
        hi++;
    if (e->text[lo] >= '0' && e->text[lo] <= '9')
        return 0;   /* a number */
    /* the fields it is in: "p->a.b" on b */
    for (;;)
    {
        int op = lo >= 2 && e->text[lo - 1] == '>' && e->text[lo - 2] == '-' ? 2 :
                 lo >= 1 && e->text[lo - 1] == '.' ? 1 : 0;
        if (!op || lo - op <= 0 || !is_word_byte(e->text[lo - op - 1]))
            break;
        lo -= op;
        while (lo > 0 && is_word_byte(e->text[lo - 1]))
            lo--;
    }
    int n = hi - lo < cap - 1 ? hi - lo : cap - 1;
    memcpy(buf, e->text + lo, (size_t)n);
    buf[n] = 0;
    *x = ed->rect.x + (gutter_cols(e) + col_of(e, line, lo) - e->hscroll) * m->cell_w;
    *y = ed->rect.y + (line - e->scroll + 1) * m->cell_h;
    return 1;
}

void gui_editor_caret_point(const struct gui_app* app, const struct gui_node* ed, int* x, int* y)
{
    const struct editor_data* e = ed->editor;
    const struct gui_metrics* m = core_node_metrics(app, ed);
    int line = line_of(e, e->cursor);
    *x = ed->rect.x + (gutter_cols(e) + col_of(e, line, e->cursor) - e->hscroll) * m->cell_w;
    *y = ed->rect.y + (line - e->scroll + 1) * m->cell_h;
}

static int view_rows(const struct gui_app* app, const struct gui_node* n)
{
    int rows = n->rect.h / core_node_metrics(app, n)->cell_h;
    return rows > 0 ? rows : 1;
}

static int view_cols(const struct gui_app* app, const struct gui_node* n)
{
    int cols = n->rect.w / core_node_metrics(app, n)->cell_w - gutter_cols(n->editor);
    return cols > 0 ? cols : 1;
}

static void clamp_scroll(const struct gui_app* app, struct gui_node* n)
{
    struct editor_data* e = n->editor;
    int max_scroll = e->line_count - view_rows(app, n);
    if (e->scroll > max_scroll) e->scroll = max_scroll;
    if (e->scroll < 0) e->scroll = 0;
    int max_h = e->max_cols - view_cols(app, n) + 1;
    if (e->hscroll > max_h) e->hscroll = max_h;
    if (e->hscroll < 0) e->hscroll = 0;
}

static void ensure_caret_visible(const struct gui_app* app, struct gui_node* n)
{
    struct editor_data* e = n->editor;
    int line = line_of(e, e->cursor);
    int rows = view_rows(app, n);
    if (line < e->scroll)
        e->scroll = line;
    else if (line >= e->scroll + rows)
        e->scroll = line - rows + 1;
    int col = col_of(e, line, e->cursor);
    int cols = view_cols(app, n);
    if (col < e->hscroll)
        e->hscroll = col;
    else if (col >= e->hscroll + cols)
        e->hscroll = col - cols + 1;
}

static int has_vbar(const struct gui_app* app, const struct gui_node* n)
{
    return n->editor->line_count > view_rows(app, n);
}

static int has_hbar(const struct gui_app* app, const struct gui_node* n)
{
    return n->editor->max_cols >= view_cols(app, n);
}

/* The vertical scrollbar: along the right edge, above the horizontal one
 * when both exist, lines as items. */
static struct scrollbar vbar(const struct gui_app* app, const struct gui_node* n)
{
    const struct editor_data* e = n->editor;
    int track = n->rect.h - (has_hbar(app, n) ? app->scrollbar_px : 0);
    struct scrollbar sb = { track, e->line_count, view_rows(app, n), e->scroll };
    return sb;
}

/* The horizontal one: along the bottom edge, past the gutter, columns as
 * items. */
static int hbar_x(const struct gui_app* app, const struct gui_node* n)
{
    return n->rect.x + gutter_cols(n->editor) * core_node_metrics(app, n)->cell_w;
}

static struct scrollbar hbar(const struct gui_app* app, const struct gui_node* n)
{
    const struct editor_data* e = n->editor;
    int track = n->rect.x + n->rect.w - hbar_x(app, n) - (has_vbar(app, n) ? app->scrollbar_px : 0);
    struct scrollbar sb = { track, e->max_cols + 1, view_cols(app, n), e->hscroll };
    return sb;
}

/* --- Highlighting --- */

/* Makes states[0 .. line] known, running the highlighter over the lines
 * before `line` that are not cached yet. */
static void ensure_states(struct editor_data* e, int line)
{
    if (line + 1 > e->states_cap)
    {
        e->states_cap = (line + 1) * 2;
        int* states = realloc(e->states, (size_t)e->states_cap * sizeof *states);
        if (!states)
            abort();
        e->states = states;
    }
    if (e->states_valid < 1)
        e->states_valid = 1;
    e->states[0] = 0;
    struct gui_span spans[256];
    while (e->states_valid <= line)
    {
        int prev = e->states_valid - 1;
        int state = e->states[prev];
        int start = e->line_starts[prev];
        e->highlighter.highlight(e->highlighter.ctx, e->text + start, line_end(e, prev) - start,
                                 &state, spans, 256);
        e->states[e->states_valid++] = state;
    }
}

/* The color of each byte of `line` into e->line_fg. */
/* The 16 VT100 colors, as the old IDE's Output: tuned for a dark panel. */
static const uint32_t ansi_palette[16] = {
    GUI_RGB(0x00, 0x00, 0x00), GUI_RGB(0xAA, 0x00, 0x00),
    GUI_RGB(0x00, 0xAA, 0x00), GUI_RGB(0xAA, 0xAA, 0x00),
    GUI_RGB(0xFF, 0xFF, 0x55), GUI_RGB(0xAA, 0x00, 0xAA),
    GUI_RGB(0x00, 0xAA, 0xAA), GUI_RGB(0xAA, 0xAA, 0xAA),
    GUI_RGB(0x55, 0x55, 0x55), GUI_RGB(0xFF, 0x55, 0x55),
    GUI_RGB(0x55, 0xFF, 0x55), GUI_RGB(0xFF, 0xFF, 0x55),
    GUI_RGB(0x55, 0x55, 0xFF), GUI_RGB(0xFF, 0x55, 0xFF),
    GUI_RGB(0x55, 0xFF, 0xFF), GUI_RGB(0xFF, 0xFF, 0xFF),
};

static int color_luma(uint32_t c)
{
    int r = (c >> 16) & 0xFF, g = (c >> 8) & 0xFF, b = c & 0xFF;
    return (r * 299 + g * 587 + b * 114) / 1000;
}

/* On a light panel, the palette's bright colors are deepened, and any
 * color too close to the panel becomes editor_output_fg; a dark panel
 * keeps the palette. */
static uint32_t ansi_readable_fg(const struct gui_theme* t, uint32_t fg)
{
    uint32_t bg = t->editor_output_bg;
    if (color_luma(bg) < 128)
        return fg;
    if (fg == GUI_RGB(0xFF, 0x55, 0x55))
        return GUI_RGB(0xCD, 0x31, 0x31);
    if (fg == GUI_RGB(0x55, 0xFF, 0x55))
        return GUI_RGB(0x2A, 0xA1, 0x98);
    int diff = color_luma(fg) - color_luma(bg);
    if (diff < 0)
        diff = -diff;
    return diff < 45 ? t->editor_output_fg : fg;
}

/* The colors of one VT100 line: each "\x1b[...m" changes the color of what
 * follows (0 resets, 1/22 bold on/off, 30-37 and 90-97 colors, 39 the
 * default); other attributes and other sequences are ignored. */
static void line_colors_vt100(struct editor_data* e, int start, int len, const struct gui_theme* t)
{
    uint32_t fg = t->editor_output_fg;
    int bold = 0;
    for (int i = 0; i < len;)
    {
        int esc = escape_len(e, start + i, start + len);
        if (!esc)
        {
            e->line_fg[i++] = ansi_readable_fg(t, fg);
            continue;
        }
        const char* s = e->text + start + i;
        int end = esc - 1;
        if (s[end] == 'm')
        {
            for (int p = 2; p <= end;)
            {
                int val = 0;
                while (p < end && s[p] >= '0' && s[p] <= '9')
                    val = val * 10 + (s[p++] - '0');
                if (val == 0)
                {
                    fg = t->editor_output_fg;
                    bold = 0;
                }
                else if (val == 1)
                    bold = 1;
                else if (val == 22)
                    bold = 0;
                else if (val >= 30 && val <= 37)
                    fg = ansi_palette[(val - 30) + (bold ? 8 : 0)];
                else if (val == 39)
                    fg = t->editor_output_fg;
                else if (val >= 90 && val <= 97)
                    fg = ansi_palette[(val - 90) + 8];
                if (p < end && s[p] == ';')
                    p++;
                else
                    break;
            }
        }
        i += esc;
    }
}

static void line_colors(struct editor_data* e, int line, uint32_t default_fg, const struct gui_theme* t)
{
    int start = e->line_starts[line], len = line_end(e, line) - start;
    if (len + 1 > e->line_fg_cap)
    {
        e->line_fg_cap = (len + 1) * 2;
        uint32_t* fg = realloc(e->line_fg, (size_t)e->line_fg_cap * sizeof *fg);
        if (!fg)
            abort();
        e->line_fg = fg;
    }
    for (int i = 0; i < len; i++)
        e->line_fg[i] = default_fg;
    if (e->vt100)
    {
        line_colors_vt100(e, start, len, t);
        return;
    }
    if (!e->has_highlighter)
        return;
    ensure_states(e, line);
    int state = e->states[line];
    struct gui_span spans[256];
    int count = e->highlighter.highlight(e->highlighter.ctx, e->text + start, len, &state, spans, 256);
    for (int s = 0; s < count; s++)
    {
        for (int i = spans[s].start; i < spans[s].start + spans[s].len && i < len; i++)
        {
            if (i >= 0)
                e->line_fg[i] = spans[s].fg & ~GUI_SPAN_HIDDEN;
        }
    }
}

/* --- Paint --- */

/* One line's text from column hscroll on, in runs of equal colors: a run
 * breaks where the color or the selected state changes. Tabs show as
 * spaces up to the next stop. */
/* The word the selection covers - ASCII word bytes only, not part of a
 * longer word - whose other occurrences are tinted: the old IDE's
 * editor_selected_word. 0 when there is none; else its length, *start its
 * offset. Not in a VT100 editor. */
static int selected_word(const struct editor_data* e, int* start)
{
    int lo, hi;
    selection(e, &lo, &hi);
    if (e->vt100 || hi <= lo)
        return 0;
    for (int i = lo; i < hi; i++)
    {
        unsigned char c = (unsigned char)e->text[i];
        if (c >= 0x80 || !is_word_byte((char)c))
            return 0;
    }
    if (lo > 0 && is_word_byte(e->text[lo - 1]))
        return 0;
    if (hi < e->len && is_word_byte(e->text[hi]))
        return 0;
    *start = lo;
    return hi - lo;
}

/* Whether the whole word `word` (len bytes) is at text[k] of a line ending at end. */
static int word_at(const struct editor_data* e, int k, int end, const char* word, int len)
{
    if (k + len > end || memcmp(e->text + k, word, (size_t)len) != 0)
        return 0;
    if (k > 0 && is_word_byte(e->text[k - 1]))
        return 0;
    return k + len >= end || !is_word_byte(e->text[k + len]);
}

static void paint_line(const struct paint* p, const struct gui_node* n, int line, int y,
                       uint32_t row_bg)
{
    const struct gui_theme* t = &p->app->theme;
    struct editor_data* e = n->editor;
    int cw = core_node_metrics(p->app, n)->cell_w;
    int x0 = n->rect.x + gutter_cols(e) * cw;
    int cols = view_cols(p->app, n);
    int lo, hi;
    selection(e, &lo, &hi);
    line_colors(e, line, e->vt100 ? t->editor_output_fg : t->editor_fg, t);

    int word_start = 0;
    int word_len = selected_word(e, &word_start);
    int match_end = 0;   /* the occurrence being drawn ends here */

    uint32_t run[256];
    int run_count = 0, run_col = 0;
    uint32_t run_fg = 0, run_bg = 0;
    int start = e->line_starts[line], end = line_end(e, line);
    int col = 0;
    for (int k = start; k < end && col < e->hscroll + cols;)
    {
        int esc = escape_len(e, k, end);
        if (esc)
        {
            k += esc;
            continue;
        }
        uint32_t cp;
        int len = core_utf8_decode(e->text + k, &cp);
        int sel = k >= lo && k < hi;
        if (word_len && k >= match_end && k != word_start && word_at(e, k, end, e->text + word_start, word_len))
            match_end = k + word_len;
        uint32_t fg = e->line_fg[k - start];   /* the selection keeps the syntax colors: only its background changes */
        uint32_t bg = sel ? t->editor_sel_bg : k < match_end ? t->editor_word_match_bg : row_bg;
        int width = 1;
        if (cp == '\t')
        {
            width = (col / TAB_SIZE + 1) * TAB_SIZE - col;
            cp = ' ';
        }
        for (int w = 0; w < width; w++, col++)
        {
            if (col < e->hscroll || col >= e->hscroll + cols)
                continue;
            if (run_count > 0 && (fg != run_fg || bg != run_bg || run_count == 256))
            {
                gui_draw_text(p->canvas, x0 + (run_col - e->hscroll) * cw, y, run, run_count,
                              run_fg, run_bg, p->font);
                run_count = 0;
            }
            if (run_count == 0)
            {
                run_col = col;
                run_fg = fg;
                run_bg = bg;
            }
            run[run_count++] = cp;
        }
        k += len;
    }
    if (run_count > 0)
        gui_draw_text(p->canvas, x0 + (run_col - e->hscroll) * cw, y, run, run_count,
                      run_fg, run_bg, p->font);

    /* A selected line end shows as one selected cell past the text. */
    if (end < e->len && end >= lo && end < hi && col >= e->hscroll && col < e->hscroll + cols)
        gui_fill_rect(p->canvas, x0 + (col - e->hscroll) * cw, y, cw, core_node_metrics(p->app, n)->cell_h,
                      t->editor_sel_bg);
}

/* The marks of one line, from `first` (all the marks on its line), drawn
 * from column `col` on in the colors of the worst one; "..." where they are
 * cut by the border. */
static void paint_marks(const struct paint* p, const struct gui_node* n, int first, int y, int col)
{
    const struct gui_theme* t = &p->app->theme;
    struct editor_data* e = n->editor;
    int cw = core_node_metrics(p->app, n)->cell_w;
    int x0 = n->rect.x + gutter_cols(e) * cw;
    int cols = view_cols(p->app, n);
    int end = e->hscroll + cols;
    int line = e->marks.items[first].line;
    int worst = first;
    int last = first;
    while (last < e->marks.count && e->marks.items[last].line == line)
    {
        if (e->marks.items[last].type > e->marks.items[worst].type)
            worst = last;
        last++;
    }
    enum gui_mark type = e->marks.items[worst].type;
    uint32_t fg = type == GUI_MARK_ERROR ? t->diag_error_fg : type == GUI_MARK_WARNING ? t->diag_warning_fg : t->diag_info_fg;
    uint32_t bg = type == GUI_MARK_ERROR ? t->diag_error_bg : type == GUI_MARK_WARNING ? t->diag_warning_bg : t->diag_info_bg;

    /* The worst first, then the others in order. */
    int start_col = col, cut = 0;
    for (int k = -1; k < last - first && !cut; k++)
    {
        int i = k < 0 ? worst : first + k;
        if (k >= 0 && i == worst)
            continue;
        for (const char* s = e->marks.items[i].text; *s;)
        {
            uint32_t cp;
            s += core_utf8_decode(s, &cp);
            if (col >= end)
            {
                cut = 1;
                break;
            }
            if (col >= e->hscroll &&
                !core_draw_symbol(p, x0 + (col - e->hscroll) * cw, y, cw, core_node_metrics(p->app, n)->cell_h, cp, fg, bg))
                gui_draw_text(p->canvas, x0 + (col - e->hscroll) * cw, y, &cp, 1, fg, bg, p->font);
            col++;
        }
    }
    if (cut)
    {
        uint32_t dot = '.';
        for (int c = end - 3 > start_col ? end - 3 : start_col; c < end; c++)
        {
            if (c >= e->hscroll)
                gui_draw_text(p->canvas, x0 + (c - e->hscroll) * cw, y, &dot, 1, fg, bg, p->font);
        }
    }
}

static int breakpoint_index(const struct editor_data* e, int line, int* found)
{
    int lo = 0, hi = e->breakpoints.count;
    while (lo < hi)
    {
        int mid = (lo + hi) / 2;
        if (e->breakpoints.items[mid] < line)
            lo = mid + 1;
        else
            hi = mid;
    }
    *found = lo < e->breakpoints.count && e->breakpoints.items[lo] == line;
    return lo;
}

static int has_breakpoint(const struct editor_data* e, int line)
{
    int found;
    breakpoint_index(e, line, &found);
    return found;
}

/* 0-based line; returns 1 when it is now set. */
static int toggle_breakpoint(struct editor_data* e, int line)
{
    struct line_list* l = &e->breakpoints;
    int found;
    int at = breakpoint_index(e, line, &found);
    if (found)
    {
        memmove(&l->items[at], &l->items[at + 1], sizeof l->items[0] * (size_t)(l->count - at - 1));
        l->count--;
        return 0;
    }
    if (l->count == l->cap)
    {
        int cap = l->cap ? l->cap * 2 : 16;
        int* items = realloc(l->items, (size_t)cap * sizeof *items);
        if (!items)
            return 0;
        l->items = items;
        l->cap = cap;
    }
    memmove(&l->items[at + 1], &l->items[at], sizeof l->items[0] * (size_t)(l->count - at));
    l->items[at] = line;
    l->count++;
    return 1;
}

void editor_paint(const struct paint* p, const struct gui_node* n)
{
    const struct gui_app* app = p->app;
    const struct gui_theme* t = &app->theme;
    struct editor_data* e = n->editor;
    int cw = core_node_metrics(app, n)->cell_w, ch = core_node_metrics(app, n)->cell_h;
    int rows = view_rows(app, n), cols = view_cols(app, n);
    int gutter = gutter_cols(e);
    int focused = app->ui.focused == n;
    if (e->reveal)
    {
        /* Asked from outside (a find, a selection set by the app), where
         * the view size is not known. */
        e->reveal = 0;
        if (e->center)
        {
            e->center = 0;
            e->scroll = line_of(e, e->cursor) - rows / 2;
        }
        ensure_caret_visible(app, (struct gui_node*)n);
    }
    /* The view may have shrunk (resize, zoom) or a goto may have put the
     * scroll past the end since the last change. */
    if (e->scroll > e->line_count - rows) e->scroll = e->line_count - rows;
    if (e->scroll < 0) e->scroll = 0;
    if (e->hscroll > e->max_cols - cols + 1) e->hscroll = e->max_cols - cols + 1;
    if (e->hscroll < 0) e->hscroll = 0;
    int lo, hi;
    selection(e, &lo, &hi);
    int caret_line = line_of(e, e->cursor);

    uint32_t bg = e->vt100 ? t->editor_output_bg : t->editor_bg;
    gui_fill_rect(p->canvas, n->rect.x, n->rect.y, n->rect.w, n->rect.h, bg);
    int mark = 0;   /* walks the marks along with the rows */
    /* diff: the gutter shows the new file's line - the row's minus the
     * removed rows before it - and nothing on a removed row */
    int removed = 0;
    for (int line = 0; e->diff && line < e->scroll && line < e->line_count; line++)
        removed += diff_row(e, line) == '-';
    for (int row = 0; row < rows; row++)
    {
        int line = e->scroll + row;
        if (line >= e->line_count)
            break;
        int y = n->rect.y + row * ch;
        char dr = e->diff ? diff_row(e, line) : 0;
        uint32_t hl_bg = 0;
        if (e->has_highlighter && e->highlighter.row_bg && lo == hi)
        {
            ensure_states(e, line);
            int start = e->line_starts[line];
            hl_bg = e->highlighter.row_bg(e->highlighter.ctx, e->text + start, line_end(e, line) - start,
                                          e->states[line]);
        }
        uint32_t row_bg = line == e->exec_line ? t->editor_exec_line_bg
                        : hl_bg ? hl_bg
                        : (dr == '+' && lo == hi) ? t->editor_diff_add_bg
                        : (dr == '-' && lo == hi) ? t->editor_diff_remove_bg
                        : (line == caret_line && lo == hi && !e->vt100) ? t->editor_current_line_bg : bg;
        gui_fill_rect(p->canvas, n->rect.x, y, n->rect.w, ch, row_bg);
        if (dr == '-')
            removed++;
        if (e->line_numbers && dr != '-')
        {
            char num[16];
            int len = 0;
            for (int v = line + 1 - removed; v > 0; v /= 10)
                num[len++] = (char)('0' + v % 10);
            for (int i = 0; i < len / 2; i++)
            {
                char c = num[i];
                num[i] = num[len - 1 - i];
                num[len - 1 - i] = c;
            }
            num[len] = '\0';
            core_draw_utf8(p, n->rect.x + (gutter - 1 - len) * cw, y, num, -1,
                           has_breakpoint(e, line) ? t->editor_breakpoint_fg : t->editor_linenum_fg, row_bg);
        }
        paint_line(p, n, line, y, row_bg);
        while (mark < e->marks.count && e->marks.items[mark].line < line)
            mark++;
        if (mark < e->marks.count && e->marks.items[mark].line == line)
            paint_marks(p, n, mark, y, col_of(e, line, line_end(e, line)));
    }

    /* The caret: a thin bar over the left fifth of its cell, hidden while
     * there is a selection - as in the old IDE. */
    if (focused && lo == hi && !e->vt100)
    {
        int col = col_of(e, caret_line, e->cursor) - e->hscroll;
        int row = caret_line - e->scroll;
        if (row >= 0 && row < rows && col >= 0 && col < cols)
        {
            int bar = cw / 5 > 0 ? cw / 5 : 1;
            gui_fill_rect(p->canvas, n->rect.x + (gutter + col) * cw, n->rect.y + row * ch, bar, ch,
                          t->editor_fg);
        }
    }

    /* The scrollbars: only while the mouse is over the editor or a thumb is
     * dragged - they never reserve space. Their size is their own, in px. */
    int show = app->ui.hot == n || app->ui.scrolling == n || app->ui.hscrolling == n;
    int w = app->scrollbar_px;
    if (show && has_vbar(app, n))
    {
        struct scrollbar sb = vbar(app, n);
        int pos, len;
        core_scrollbar_thumb(app, &sb, &pos, &len);
        int x = n->rect.x + n->rect.w - w;
        int hot = app->ui.scrolling == n || (app->ui.thumb_hot == n && app->ui.thumb_hot_bar == 1);
        gui_fill_rect(p->canvas, x, n->rect.y, w, sb.track, t->scrollbar_bg);
        gui_fill_rect(p->canvas, x, n->rect.y + pos, w, len, hot ? t->scrollbar_thumb_hot_bg : t->scrollbar_thumb_bg);
    }
    if (show && has_hbar(app, n))
    {
        struct scrollbar sb = hbar(app, n);
        int pos, len;
        core_scrollbar_thumb(app, &sb, &pos, &len);
        int x = hbar_x(app, n), y = n->rect.y + n->rect.h - w;
        int hot = app->ui.hscrolling == n || (app->ui.thumb_hot == n && app->ui.thumb_hot_bar == 2);
        gui_fill_rect(p->canvas, x, y, sb.track, w, t->scrollbar_bg);
        gui_fill_rect(p->canvas, x + pos, y, len, w, hot ? t->scrollbar_thumb_hot_bg : t->scrollbar_thumb_bg);
    }
}

int editor_thumb_at(const struct gui_app* app, const struct gui_node* n, int x, int y)
{
    int w = app->scrollbar_px;
    int pos, len;
    if (has_vbar(app, n) && x >= n->rect.x + n->rect.w - w)
    {
        struct scrollbar sb = vbar(app, n);
        core_scrollbar_thumb(app, &sb, &pos, &len);
        if (y >= n->rect.y + pos && y < n->rect.y + pos + len)
            return 1;
    }
    if (has_hbar(app, n) && y >= n->rect.y + n->rect.h - w)
    {
        struct scrollbar sb = hbar(app, n);
        core_scrollbar_thumb(app, &sb, &pos, &len);
        int x0 = hbar_x(app, n);
        if (x >= x0 + pos && x < x0 + pos + len)
            return 2;
    }
    return 0;
}

/* --- Mouse --- */

/* The byte under (x, y), clamped to the visible text. */
static int pos_at(const struct gui_app* app, const struct gui_node* n, int x, int y)
{
    const struct editor_data* e = n->editor;
    int cw = core_node_metrics(app, n)->cell_w, ch = core_node_metrics(app, n)->cell_h;
    int row = (y - n->rect.y) / ch;
    if (y < n->rect.y) row = -1;
    int line = e->scroll + row;
    if (line < 0) line = 0;
    if (line >= e->line_count) line = e->line_count - 1;
    int col = (x - n->rect.x) / cw - gutter_cols(e) + e->hscroll;
    if (col < 0) col = 0;
    /* A click past the middle of a cell goes after it. */
    return pos_of_col(e, line, col);
}

static void select_word(struct editor_data* e)
{
    int lo = e->cursor, hi = e->cursor;
    while (lo > 0 && is_word_byte(e->text[lo - 1]))
        lo--;
    while (hi < e->len && is_word_byte(e->text[hi]))
        hi++;
    e->anchor = lo;
    e->cursor = hi;
}

void editor_mouse_down(struct gui_app* app, struct gui_node* n, int double_click, int mods)
{
    struct editor_data* e = n->editor;
    int x = app->mouse_x, y = app->mouse_y;
    int w = app->scrollbar_px;
    if (has_vbar(app, n) && x >= n->rect.x + n->rect.w - w)
    {
        struct scrollbar sb = vbar(app, n);
        if (y < n->rect.y + sb.track)
        {
            app->ui.scrolling = n;
            e->scroll = core_scrollbar_press(app, &sb, y - n->rect.y);
            clamp_scroll(app, n);
            return;
        }
    }
    if (has_hbar(app, n) && y >= n->rect.y + n->rect.h - w)
    {
        struct scrollbar sb = hbar(app, n);
        int at = x - hbar_x(app, n);
        if (at >= 0 && at < sb.track)
        {
            app->ui.hscrolling = n;
            e->hscroll = core_scrollbar_press(app, &sb, at);
            clamp_scroll(app, n);
            return;
        }
    }
    /* A click on a line number toggles that line's breakpoint. */
    int cw = core_node_metrics(app, n)->cell_w, ch = core_node_metrics(app, n)->cell_h;
    if (e->line_numbers && x < n->rect.x + gutter_cols(e) * cw && y >= n->rect.y)
    {
        int line = e->scroll + (y - n->rect.y) / ch;
        if (line < e->line_count)
            toggle_breakpoint(e, line);
        return;
    }
    e->cursor = pos_at(app, n, x, y);
    if (!(mods & GUI_MOD_SHIFT))
        e->anchor = e->cursor;
    e->want_col = -1;
    e->typing = 0;
    if (double_click)
    {
        select_word(e);
        if (n->id)
            core_fire(app, n->id);
    }
    else if (e->click_id && ((mods & GUI_MOD_PRIMARY) || (e->read_only && !e->vt100)))
    {
        core_fire(app, e->click_id);
    }
    else
    {
        app->ui.selecting = n;
    }
}

void editor_mouse_drag(struct gui_app* app, struct gui_node* n)
{
    struct editor_data* e = n->editor;
    if (app->ui.scrolling == n)
    {
        struct scrollbar sb = vbar(app, n);
        e->scroll = core_scrollbar_drag(app, &sb, app->mouse_y - n->rect.y);
        clamp_scroll(app, n);
        return;
    }
    if (app->ui.hscrolling == n)
    {
        struct scrollbar sb = hbar(app, n);
        e->hscroll = core_scrollbar_drag(app, &sb, app->mouse_x - hbar_x(app, n));
        clamp_scroll(app, n);
        return;
    }
    /* Dragging past the top or bottom scrolls one line per move. */
    if (app->mouse_y < n->rect.y)
        e->scroll--;
    else if (app->mouse_y >= n->rect.y + view_rows(app, n) * core_node_metrics(app, n)->cell_h)
        e->scroll++;
    clamp_scroll(app, n);
    e->cursor = pos_at(app, n, app->mouse_x, app->mouse_y);
    ensure_caret_visible(app, n);
}

void editor_wheel(struct gui_app* app, struct gui_node* n, int rows)
{
    n->editor->scroll -= rows;
    clamp_scroll(app, n);
}

void editor_hwheel(struct gui_app* app, struct gui_node* n, int cols)
{
    n->editor->hscroll += cols;
    clamp_scroll(app, n);
}

/* --- Keyboard --- */

static void copy_selection(struct gui_app* app, const struct editor_data* e)
{
    int lo, hi;
    selection(e, &lo, &hi);
    if (lo == hi)
        return;
    char* s = copy_bytes(e->text + lo, hi - lo);
    gui_clipboard_set(app->canvas, s);
    free(s);
}

/* The leading spaces of the caret's line, up to the caret. */
static int indent_of(const struct editor_data* e, int line, int limit)
{
    int k = e->line_starts[line];
    while (k < limit && e->text[k] == ' ')
        k++;
    return k - e->line_starts[line];
}

/* Tab / Shift+Tab over every line the selection touches (or the caret's
 * line): TAB_SIZE spaces added, or up to TAB_SIZE leading spaces removed,
 * as one undo step. */
static void shift_lines(struct editor_data* e, int outdent)
{
    int lo, hi;
    selection(e, &lo, &hi);
    int first = line_of(e, lo);
    int last = line_of(e, hi);
    if (hi > lo && hi == e->line_starts[last] && last > first)
        last--;   /* a selection ending at a line start does not touch that line */
    e->group++;
    int cursor_line = line_of(e, e->cursor), anchor_line = line_of(e, e->anchor);
    int cursor_col = col_of(e, cursor_line, e->cursor), anchor_col = col_of(e, anchor_line, e->anchor);
    for (int line = first; line <= last; line++)
    {
        int start = e->line_starts[line];
        if (outdent)
        {
            int n = 0;
            while (n < TAB_SIZE && e->text[start + n] == ' ')
                n++;
            if (n > 0)
                edit(e, start, n, "", 0);
        }
        else if (line_end(e, line) > start)
        {
            edit(e, start, 0, "    ", TAB_SIZE);
        }
    }
    int shift = outdent ? -TAB_SIZE : TAB_SIZE;
    cursor_col += shift;
    anchor_col += shift;
    e->cursor = pos_of_col(e, cursor_line, cursor_col < 0 ? 0 : cursor_col);
    e->anchor = pos_of_col(e, anchor_line, anchor_col < 0 ? 0 : anchor_col);
    e->typing = 0;
}

static int move_key(const struct gui_app* app, struct gui_node* n, int key, int mods)
{
    struct editor_data* e = n->editor;
    int ctrl = (mods & GUI_MOD_PRIMARY) != 0;   /* Home/End: the document's */
    int word = (mods & GUI_MOD_WORD) != 0;      /* Left/Right: a word */
    int line = line_of(e, e->cursor);
#ifdef __APPLE__
    /* Command+arrows: the line's ends, the document's ends - the Mac's */
    if (mods & GUI_MOD_CMD)
    {
        switch (key)
        {
        case GUI_KEY_LEFT: key = GUI_KEY_HOME; ctrl = 0; break;
        case GUI_KEY_RIGHT: key = GUI_KEY_END; ctrl = 0; break;
        case GUI_KEY_UP: key = GUI_KEY_HOME; ctrl = 1; break;
        case GUI_KEY_DOWN: key = GUI_KEY_END; ctrl = 1; break;
        }
    }
#endif
    int lo, hi;
    selection(e, &lo, &hi);
    int shift = (mods & GUI_MOD_SHIFT) != 0;
    int keep_col = 0;
    switch (key)
    {
    case GUI_KEY_LEFT:
        e->cursor = (!shift && lo != hi) ? lo : word ? word_left(e, e->cursor) : prev_char(e, e->cursor);
        break;
    case GUI_KEY_RIGHT:
        e->cursor = (!shift && lo != hi) ? hi : word ? word_right(e, e->cursor) : next_char(e, e->cursor);
        break;
    case GUI_KEY_UP:
    case GUI_KEY_DOWN:
    case GUI_KEY_PAGEUP:
    case GUI_KEY_PAGEDOWN:
    {
        int page = view_rows(app, n) - 1 > 0 ? view_rows(app, n) - 1 : 1;
        int delta = key == GUI_KEY_UP ? -1 : key == GUI_KEY_DOWN ? 1 : key == GUI_KEY_PAGEUP ? -page : page;
        if (e->want_col < 0)
            e->want_col = col_of(e, line, e->cursor);
        int target = line + delta;
        if (target < 0) target = 0;
        if (target >= e->line_count) target = e->line_count - 1;
        if (key == GUI_KEY_PAGEUP || key == GUI_KEY_PAGEDOWN)
            e->scroll += target - line;
        e->cursor = pos_of_col(e, target, e->want_col);
        keep_col = 1;
        break;
    }
    case GUI_KEY_HOME:
        if (ctrl)
        {
            e->cursor = 0;
        }
        else
        {
            /* First to the indentation, then to the line start. */
            int indent = e->line_starts[line] + indent_of(e, line, line_end(e, line));
            e->cursor = e->cursor == indent ? e->line_starts[line] : indent;
        }
        break;
    case GUI_KEY_END:
        e->cursor = ctrl ? e->len : line_end(e, line);
        break;
    default:
        return 0;
    }
    if (!keep_col)
        e->want_col = -1;
    if (!shift)
        e->anchor = e->cursor;
    e->typing = 0;
    return 1;
}

int editor_key(struct gui_app* app, struct gui_node* n, int key, int mods)
{
    struct editor_data* e = n->editor;
    int ctrl = (mods & GUI_MOD_PRIMARY) != 0;   /* Command on macOS */
    int shift = (mods & GUI_MOD_SHIFT) != 0;
    int lo, hi;
    selection(e, &lo, &hi);
    int handled = 1;

    if (move_key(app, n, key, mods))
    {
        clamp_scroll(app, n);
        ensure_caret_visible(app, n);
        app->needs_paint = 1;
        return 1;
    }
    if (ctrl && key == 'A')
    {
        e->anchor = 0;
        e->cursor = e->len;
    }
    else if (ctrl && key == 'C')
    {
        copy_selection(app, e);
    }
    else if (e->read_only)
    {
        handled = 0;   /* nothing below may change the text */
    }
    else if (ctrl && key == 'X')
    {
        copy_selection(app, e);
        if (lo != hi)
            replace_selection(e, "", 0);
    }
    else if (ctrl && key == 'V')
    {
        char* s = gui_clipboard_get(app->canvas);
        if (s)
        {
            replace_selection(e, s, (int)strlen(s));
            free(s);
        }
    }
    else if (ctrl && (key == 'Z' || key == 'Y'))
    {
        undo_or_redo(e, key == 'Y' || shift);
    }
    else if (key == GUI_KEY_ENTER && !ctrl)
    {
        /* A new line keeps the current line's indentation. */
        int line = line_of(e, lo);
        int indent = indent_of(e, line, lo);
        char buf[256];
        if (indent > (int)sizeof buf - 2) indent = (int)sizeof buf - 2;
        buf[0] = '\n';
        memset(buf + 1, ' ', (size_t)indent);
        type_text(e, buf, 1 + indent);
    }
    else if (key == GUI_KEY_TAB && !ctrl)
    {
        int multi = lo != hi && line_of(e, lo) != line_of(e, hi);
        if (shift || multi)
        {
            shift_lines(e, shift);
        }
        else
        {
            int col = col_of(e, line_of(e, lo), lo);
            int spaces = TAB_SIZE - col % TAB_SIZE;
            type_text(e, "    ", spaces);
        }
    }
    else if (key == GUI_KEY_BACKSPACE)
    {
        if (lo == hi)
        {
            e->anchor = ctrl ? word_left(e, lo) : prev_char(e, lo);
            e->cursor = lo;
        }
        replace_selection(e, "", 0);
    }
    else if (key == GUI_KEY_DELETE)
    {
        if (lo == hi)
        {
            e->anchor = ctrl ? word_right(e, lo) : next_char(e, lo);
            e->cursor = lo;
        }
        replace_selection(e, "", 0);
    }
    else
    {
        handled = 0;
    }

    if (handled)
    {
        e->want_col = -1;
        clamp_scroll(app, n);
        ensure_caret_visible(app, n);
        app->needs_paint = 1;
    }
    return handled;
}

void editor_char(struct gui_app* app, struct gui_node* n, uint32_t ch)
{
    struct editor_data* e = n->editor;
    if (e->read_only)
        return;
    char buf[4];
    int len;
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
    type_text(e, buf, len);
    e->want_col = -1;
    clamp_scroll(app, n);
    ensure_caret_visible(app, n);
    app->needs_paint = 1;
}

void gui_editor_undo(struct gui_node* ed)
{
    undo_or_redo(ed->editor, 0);
}

void gui_editor_redo(struct gui_node* ed)
{
    undo_or_redo(ed->editor, 1);
}

void gui_editor_copy(struct gui_app* app, struct gui_node* ed)
{
    copy_selection(app, ed->editor);
}

void gui_editor_cut(struct gui_app* app, struct gui_node* ed)
{
    struct editor_data* e = ed->editor;
    int lo, hi;
    selection(e, &lo, &hi);
    copy_selection(app, e);
    if (lo != hi && !e->read_only)
        replace_selection(e, "", 0);
}

void gui_editor_paste(struct gui_app* app, struct gui_node* ed)
{
    struct editor_data* e = ed->editor;
    char* s = e->read_only ? NULL : gui_clipboard_get(app->canvas);
    if (s)
    {
        replace_selection(e, s, (int)strlen(s));
        free(s);
    }
}

void gui_editor_get_selection(const struct gui_node* ed, int* lo, int* hi)
{
    selection(ed->editor, lo, hi);
}

void gui_editor_set_selection(struct gui_node* ed, int lo, int hi)
{
    struct editor_data* e = ed->editor;
    if (lo < 0) lo = 0;
    if (hi > e->len) hi = e->len;
    e->anchor = lo;
    e->cursor = hi;
    e->want_col = -1;
    e->typing = 0;
    e->reveal = 1;
}

void gui_editor_replace(struct gui_node* ed, int lo, int hi, const char* text)
{
    struct editor_data* e = ed->editor;
    if (e->read_only)
        return;
    int len = (int)strlen(text);
    e->group++;
    edit(e, lo, hi - lo, text, len);
    e->anchor = lo;
    e->cursor = lo + len;
    e->typing = 0;
    e->reveal = 1;
}

void gui_editor_goto_line_center(struct gui_node* ed, int line)
{
    gui_editor_goto_line(ed, line);
    ed->editor->reveal = 1;
    ed->editor->center = 1;
}

void gui_editor_add_mark(struct gui_node* ed, enum gui_mark type, int line, const char* utf8)
{
    struct mark_list* l = &ed->editor->marks;
    if (l->count == l->cap)
    {
        int cap = l->cap ? l->cap * 2 : 16;
        struct mark* items = realloc(l->items, (size_t)cap * sizeof *items);
        if (!items)
            return;
        l->items = items;
        l->cap = cap;
    }
    int at = l->count;
    while (at > 0 && l->items[at - 1].line > line - 1)
        at--;
    memmove(&l->items[at + 1], &l->items[at], (size_t)(l->count - at) * sizeof *l->items);
    struct mark* m = &l->items[at];
    m->type = type;
    m->line = line - 1;
    m->text = core_strdup(utf8);
    m->cols = core_utf8_cells(m->text, -1);
    l->count++;
    rebuild_lines(ed->editor);
}

int gui_editor_toggle_breakpoint(struct gui_node* ed, int line)
{
    return line >= 1 && toggle_breakpoint(ed->editor, line - 1);
}

int gui_editor_get_breakpoints(const struct gui_node* ed, int* out, int max)
{
    const struct line_list* l = &ed->editor->breakpoints;
    int n = l->count < max ? l->count : max;
    for (int i = 0; i < n; i++)
        out[i] = l->items[i] + 1;
    return n;
}

void gui_editor_clear_breakpoints(struct gui_node* ed)
{
    ed->editor->breakpoints.count = 0;
}

void gui_editor_set_exec_line(struct gui_node* ed, int line)
{
    ed->editor->exec_line = line - 1;
    if (line > 0)
        gui_editor_goto_line_center(ed, line);
}

void gui_editor_clear_marks(struct gui_node* ed)
{
    free_marks(&ed->editor->marks);
    rebuild_lines(ed->editor);
}

void gui_editor_goto_line(struct gui_node* ed, int line)
{
    struct editor_data* e = ed->editor;
    line--;
    if (line < 0) line = 0;
    if (line >= e->line_count) line = e->line_count - 1;
    e->cursor = e->anchor = e->line_starts[line];
    e->scroll = line;   /* clamped on the next paint */
    e->want_col = -1;
}
