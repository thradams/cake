/* ide_highlight.c - the IDE's syntax highlighters for the framework's
 * editor (struct gui_highlighter). The keyword lists are the old IDE's
 * (ide_ui.c), copied unchanged.
 */
#include "ide_shell.h"
#include <stdlib.h>
#include <string.h>

/* --- Keywords (from the old IDE) --- */

static int is_ident_char(uint32_t cp)
{
    return (cp >= 'a' && cp <= 'z') || (cp >= 'A' && cp <= 'Z') ||
        (cp >= '0' && cp <= '9') || cp == '_';
}
static int is_c_keyword1(const char* word, int len)
{
    char c = word[0];

    if (c == 'a')
    {
        if (len == 4 && memcmp(word, "auto", 4) == 0) return 1;
        if (len == 7 && memcmp(word, "alignof", 7) == 0) return 1;
    }
    else if (c == 'b')
    {
        if (len == 5 && memcmp(word, "break", 5) == 0) return 1;
        if (len == 4 && memcmp(word, "bool", 4) == 0) return 1;   // C23
    }
    else if (c == 'c')
    {
        if (len == 5 && memcmp(word, "const", 5) == 0) return 1;
        if (len == 4 && memcmp(word, "char", 4) == 0) return 1;
        if (len == 4 && memcmp(word, "case", 4) == 0) return 1;
        if (len == 5 && memcmp(word, "catch", 5) == 0) return 1;  // C++
        if (len == 8 && memcmp(word, "continue", 8) == 0) return 1;
        if (len == 14 && memcmp(word, "compile_assert", 14) == 0) return 1;
        if (len == 9 && memcmp(word, "constexpr", 9) == 0) return 1;
    }
    else if (c == 'd')
    {
        if (len == 6 && memcmp(word, "double", 6) == 0) return 1;
        if (len == 2 && memcmp(word, "do", 2) == 0) return 1;
        if (len == 7 && memcmp(word, "default", 7) == 0) return 1;
    }
    else if (c == 'e')
    {
        if (len == 4 && memcmp(word, "enum", 4) == 0) return 1;
        if (len == 6 && memcmp(word, "extern", 6) == 0) return 1;
        if (len == 4 && memcmp(word, "else", 4) == 0) return 1;
    }
    else if (c == 'f')
    {
        if (len == 5 && memcmp(word, "float", 5) == 0) return 1;
        if (len == 3 && memcmp(word, "for", 3) == 0) return 1;
        if (len == 4 && memcmp(word, "false", 4) == 0) return 1;  // C23
    }
    else if (c == 'g')
    {
        if (len == 4 && memcmp(word, "goto", 4) == 0) return 1;
    }
    else if (c == 'i')
    {
        if (len == 3 && memcmp(word, "int", 3) == 0) return 1;
        if (len == 2 && memcmp(word, "if", 2) == 0) return 1;
        if (len == 6 && memcmp(word, "inline", 6) == 0) return 1;   // C99
    }
    else if (c == 'l')
    {
        if (len == 4 && memcmp(word, "long", 4) == 0) return 1;
    }
    else if (c == 'n')
    {
        if (len == 7 && memcmp(word, "nullptr", 7) == 0) return 1;
    }
    else if (c == 'r')
    {
        if (len == 8 && memcmp(word, "register", 8) == 0) return 1;
        if (len == 6 && memcmp(word, "return", 6) == 0) return 1;
        if (len == 8 && memcmp(word, "restrict", 8) == 0) return 1; // C99        
    }
    else if (c == 's')
    {
        if (len == 6 && memcmp(word, "signed", 6) == 0) return 1;
        if (len == 6 && memcmp(word, "sizeof", 6) == 0) return 1;
        if (len == 6 && memcmp(word, "struct", 6) == 0) return 1;
        if (len == 5 && memcmp(word, "short", 5) == 0) return 1;
        if (len == 6 && memcmp(word, "static", 6) == 0) return 1;
        if (len == 13 && memcmp(word, "static_assert", 13) == 0) return 1; // C++ / C23
        if (len == 6 && memcmp(word, "switch", 6) == 0) return 1;
        if (len == 12 && memcmp(word, "static_debug", 12) == 0) return 1; // C++ / C23

    }
    else if (c == 't')
    {
        if (len == 7 && memcmp(word, "typedef", 7) == 0) return 1;
        if (len == 6 && memcmp(word, "typeof", 6) == 0) return 1;      // GNU
        if (len == 3 && memcmp(word, "try", 3) == 0) return 1;         // C++
        if (len == 4 && memcmp(word, "true", 4) == 0) return 1;        // C23
        if (len == 5 && memcmp(word, "throw", 5) == 0) return 1;       // C++
        if (len == 13 && memcmp(word, "typeof_unqual", 13) == 0) return 1; // GNU
    }
    else if (c == 'u')
    {
        if (len == 5 && memcmp(word, "union", 5) == 0) return 1;
        if (len == 8 && memcmp(word, "unsigned", 8) == 0) return 1;
    }
    else if (c == 'v')
    {
        if (len == 4 && memcmp(word, "void", 4) == 0) return 1;
        if (len == 8 && memcmp(word, "volatile", 8) == 0) return 1;
    }
    else if (c == 'w')
    {
        if (len == 5 && memcmp(word, "while", 5) == 0) return 1;
    }
    else if (c == '_')
    {
        // Standard C underscored keywords (C99, C11, C23)
        if (len == 5 && word[1] == 'B' && memcmp(word, "_Bool", 5) == 0) return 1;
        if (len == 7 && word[1] == 'B' && memcmp(word, "_BitInt", 7) == 0) return 1;  // C23
        if (len == 7 && word[1] == 'A' && memcmp(word, "_Atomic", 7) == 0) return 1;
        if (len == 8 && word[1] == 'A')
        {
            if (memcmp(word, "_Alignas", 8) == 0) return 1;
            if (memcmp(word, "_Alignof", 8) == 0) return 1;
        }
        if (len == 8 && word[1] == 'C')
        {
            if (memcmp(word, "_Complex", 8) == 0) return 1;
            if (memcmp(word, "_Countof", 8) == 0) return 1;   // <-- added (non‑standard, but requested)
        }
        if (len == 8 && word[1] == 'G' && memcmp(word, "_Generic", 8) == 0) return 1; // C11
        if (len == 6 && word[1] == 'D' && memcmp(word, "_Defer", 6) == 0) return 1;   // <-- added (proposed/extension)
        if (len == 9 && word[1] == 'N' && memcmp(word, "_Noreturn", 9) == 0) return 1;
        if (len == 10 && word[1] == 'I' && memcmp(word, "_Imaginary", 10) == 0) return 1;
        if (len == 13 && word[1] == 'T' && memcmp(word, "_Thread_local", 13) == 0) return 1;
        if (len == 14 && word[1] == 'S' && memcmp(word, "_Static_assert", 14) == 0) return 1;
        if (len == 7 && memcmp(word, "_Assert", 7) == 0) return 1;
    }
    return 0;
}

/* Group 2: control-flow keywords (editor_keyword2_fg). */
static int is_c_keyword2(const char* word, int len)
{
    if (len == 6 && memcmp(word, "_Owner", 6) == 0) return 1;
    if (len == 4 && memcmp(word, "_Opt", 4) == 0) return 1;
    if (len == 5 && memcmp(word, "_Dtor", 5) == 0) return 1;
    if (len == 4 && memcmp(word, "_Out", 4) == 0) return 1;
    if (len == 5 && memcmp(word, "_View", 5) == 0) return 1;
    if (len == 6 && memcmp(word, "assert", 6) == 0) return 1;
    if (len == 4 && memcmp(word, "NULL", 4) == 0) return 1;
    if (len == 6 && memcmp(word, "_Clear", 6) == 0) return 1;
    if (len == 14 && memcmp(word, "_Uninitialized", 14) == 0) return 1;
    if (len == sizeof("_Nullable") - 1 && memcmp(word, "_Nullable", sizeof("_Nullable") - 1) == 0) return 1;



    return 0;
}

/* True for exactly "struct", "union", or "enum" - the three keywords that
 * introduce a tag name (e.g. the X in "struct X"). Used by
 * render_editor_line to color that following identifier via
 * g_theme.editor_tag_fg instead of the ordinary identifier color. */
static int is_c_tag_keyword(const char* word, int len)
{
    return (len == 6 && memcmp(word, "struct", 6) == 0) ||
        (len == 5 && memcmp(word, "union", 5) == 0) ||
        (len == 4 && memcmp(word, "enum", 4) == 0);
}

/* --- The C highlighter ---
 *
 * The state carried from line to line: bit 0 is "inside a block comment",
 * the rest the ( [ { nesting depth, for the bracket colors. */

#define IN_COMMENT 1

static int add_span(struct gui_span* spans, int n, int max, int start, int len, uint32_t fg)
{
    if (n < max && len > 0)
    {
        spans[n].start = start;
        spans[n].len = len;
        spans[n].fg = fg;
        n++;
    }
    return n;
}

static int is_digit(char c)
{
    return c >= '0' && c <= '9';
}

static int skip_blanks(const char* line, int len, int i)
{
    while (i < len && (line[i] == ' ' || line[i] == '\t'))
        i++;
    return i;
}

int ide_highlight_c(void* ctx, const char* line, int len, int* state,
                     struct gui_span* spans, int max)
{
    const struct gui_theme* t = ctx;
    int in_comment = *state & IN_COMMENT;
    int depth = *state >> 1;
    int n = 0, i = 0;
    int pending_tag = 0;   /* the last word was struct/union/enum */
    int in_attr = 0;       /* inside a C23 [[attribute]] */
    int attr_depth = 0;    /* open [ inside the attribute */

    /* A preprocessor line: the '#' and its directive word. */
    int k = 0;
    while (k < len && (line[k] == ' ' || line[k] == '\t'))
        k++;
    if (!in_comment && k < len && line[k] == '#')
    {
        int start = k++;
        while (k < len && (line[k] == ' ' || line[k] == '\t'))
            k++;
        while (k < len && is_ident_char((unsigned char)line[k]))
            k++;
        n = add_span(spans, n, max, start, k - start, t->editor_preproc_fg);
        i = k;
    }

    while (i < len)
    {
        int start = i;
        char c = line[i];
        if (in_comment || (c == '/' && i + 1 < len && line[i + 1] == '*'))
        {
            if (!in_comment)
                i += 2;
            in_comment = 1;
            while (i < len && !(line[i] == '*' && i + 1 < len && line[i + 1] == '/'))
                i++;
            if (i < len)
            {
                i += 2;
                in_comment = 0;
            }
            n = add_span(spans, n, max, start, i - start, t->editor_comment_fg);
        }
        else if (c == '/' && i + 1 < len && line[i + 1] == '/')
        {
            int lint = len - i >= 6 && memcmp(line + i, "//lint", 6) == 0;
            n = add_span(spans, n, max, i, len - i, lint ? t->editor_lint_fg : t->editor_comment_fg);
            i = len;
        }
        else if (c == '"' || c == '\'')
        {
            i++;
            while (i < len && line[i] != c)
                i += (line[i] == '\\' && i + 1 < len) ? 2 : 1;
            if (i < len)
                i++;
            n = add_span(spans, n, max, start, i - start,
                         c == '"' ? t->editor_string_fg : t->editor_char_fg);
        }
        else if (is_digit(c) || (c == '.' && i + 1 < len && is_digit(line[i + 1])))
        {
            /* int, float, hex, with suffixes and exponents */
            while (i < len && (is_ident_char((unsigned char)line[i]) || line[i] == '.' ||
                               ((line[i] == '+' || line[i] == '-') &&
                                (line[i - 1] == 'e' || line[i - 1] == 'E' ||
                                 line[i - 1] == 'p' || line[i - 1] == 'P'))))
                i++;
            n = add_span(spans, n, max, start, i - start, t->editor_number_fg);
        }
        else if (is_ident_char((unsigned char)c))
        {
            while (i < len && is_ident_char((unsigned char)line[i]))
                i++;
            int wlen = i - start;
            int next = i;
            while (next < len && (line[next] == ' ' || line[next] == '\t'))
                next++;
            if (in_attr)
                n = add_span(spans, n, max, start, wlen, t->editor_attribute_fg);
            else if (is_c_keyword1(line + start, wlen))
                n = add_span(spans, n, max, start, wlen, t->editor_keyword_fg);
            else if (is_c_keyword2(line + start, wlen))
                n = add_span(spans, n, max, start, wlen, t->editor_keyword2_fg);
            else if (pending_tag)
                n = add_span(spans, n, max, start, wlen, t->editor_tag_fg);
            else if (next < len && line[next] == '(')
                n = add_span(spans, n, max, start, wlen, t->editor_function_fg);
            pending_tag = is_c_tag_keyword(line + start, wlen);
            continue;
        }
        else if (!in_attr && c == '[' && skip_blanks(line, len, i + 1) < len &&
                 line[skip_blanks(line, len, i + 1)] == '[')
        {
            int second = skip_blanks(line, len, i + 1);
            n = add_span(spans, n, max, i, 1, t->editor_attribute_fg);
            n = add_span(spans, n, max, second, 1, t->editor_attribute_fg);
            in_attr = 1;
            attr_depth = 0;
            i = second + 1;
        }
        else if (in_attr && c == '[')
        {
            attr_depth++;
            i++;
        }
        else if (in_attr && c == ']' && attr_depth > 0)
        {
            attr_depth--;
            i++;
        }
        else if (in_attr && c == ']' && skip_blanks(line, len, i + 1) < len &&
                 line[skip_blanks(line, len, i + 1)] == ']')
        {
            int second = skip_blanks(line, len, i + 1);
            n = add_span(spans, n, max, i, 1, t->editor_attribute_fg);
            n = add_span(spans, n, max, second, 1, t->editor_attribute_fg);
            in_attr = 0;
            i = second + 1;
        }
        else if (c == '(' || c == '[' || c == '{')
        {
            n = add_span(spans, n, max, i, 1, t->editor_bracket_fg[depth % GUI_EDITOR_BRACKET_COLORS]);
            depth++;
            i++;
        }
        else if (c == ')' || c == ']' || c == '}')
        {
            if (depth > 0)
                depth--;
            n = add_span(spans, n, max, i, 1, t->editor_bracket_fg[depth % GUI_EDITOR_BRACKET_COLORS]);
            i++;
        }
        else
        {
            i++;
        }
        if (c != ' ' && c != '\t')
            pending_tag = 0;
    }
    *state = (depth << 1) | in_comment;
    return n;
}

/* --- Markdown: the old IDE's UI_SYNTAX_MARKDOWN. Headings ("# ") and
 * blockquotes ("> ") color their whole line, ``` fences color the lines
 * between them, and `code`, **bold** and [link](dest) their own bit, in the
 * theme's md_* colors. Editing shows the delimiters, colored; a read-only
 * editor hides them (GUI_SPAN_HIDDEN). --- */

/* One line, byte by byte: `emit(i, n, fg, delim)` for bytes i .. i+n-1;
 * delim: Markdown punctuation, not text. *in_block: inside a ``` block. */
/* The length of the bare http(s) URL at `p`, up to a blank, quote or
 * bracket, without a trailing '.', ',', ';' or ':'; 0 if none starts there. */
static int md_url_len(const char* p, int len)
{
    int k = len >= 7 && memcmp(p, "http://", 7) == 0 ? 7 : len >= 8 && memcmp(p, "https://", 8) == 0 ? 8 : 0;
    if (k == 0)
        return 0;
    while (k < len && !strchr(" \t\r\n()<>[]\"'`", p[k]))
        k++;
    while (k > 0 && strchr(".,;:", p[k - 1]))
        k--;
    return k;
}

static void md_scan(const struct gui_theme* t, const char* line, int len, int* in_block,
                    void (*emit)(void* ctx, int i, int n, uint32_t fg, int delim), void* ctx)
{
    int j = 0;
    while (j < len && (line[j] == ' ' || line[j] == '\t'))
        j++;
    int fence = len - j >= 3 && line[j] == '`' && line[j + 1] == '`' && line[j + 2] == '`';
    if (fence)
        *in_block = !*in_block;
    int code_block = *in_block || fence;
    int heading = 0;
    if (!code_block)
    {
        while (heading < len && heading < 6 && line[heading] == '#')
            heading++;
        if (heading == 0 || heading >= len || line[heading] != ' ')
            heading = 0;
    }
    int quote = !code_block && !heading && j < len && line[j] == '>';
    int in_span = 0, in_bold = 0, in_link = 0, in_dest = 0, in_comment = 0;
    for (int i = 0; i < len;)
    {
        int n = 1, delim = 0, code_mark = 0, bold_mark = 0, link_mark = 0, url = 0;
        char c = line[i];
        if (in_comment)
        {
            delim = 1;
            if (i + 2 < len && c == '-' && line[i + 1] == '-' && line[i + 2] == '>')
                n = 3, in_comment = 0;
        }
        else if (fence)
            delim = 1, n = len - i;
        else if (!code_block)
        {
            if (!in_span && !in_dest && (url = md_url_len(line + i, len - i)) > 0)
                n = url;
            else if (c == '`')
                delim = code_mark = 1, in_span = !in_span;
            else if (c == '*' && i + 1 < len && line[i + 1] == '*')
                delim = bold_mark = 1, n = 2, in_bold = !in_bold;
            else if (c == '<' && i + 3 < len && line[i + 1] == '!' && line[i + 2] == '-' && line[i + 3] == '-')
                delim = 1, n = 4, in_comment = 1;
            else if (c == '[')
            {
                int k = i + 1;
                while (k < len && line[k] != ']')
                    k++;
                if (k + 1 < len && line[k + 1] == '(')
                    in_link = delim = link_mark = 1;
            }
            else if (c == ']' && in_link)
            {
                in_link = 0;
                delim = link_mark = 1;
                if (i + 1 < len && line[i + 1] == '(')
                    in_dest = 1;
            }
            else if (in_dest)
            {
                delim = 1;
                if (c == ')')
                    in_dest = 0;
            }
        }
        uint32_t fg = code_block ? t->md_code_fg
                    : heading ? t->md_heading_fg
                    : quote ? t->md_blockquote_fg
                    : (in_span || code_mark) ? t->md_code_fg
                    : (in_link || link_mark || url) ? t->md_link_fg
                    : t->editor_fg;
        if (in_bold || bold_mark)
            fg = t->md_bold_fg;
        if (i + n > len)
            n = len - i;
        emit(ctx, i, n, fg, delim);
        i += n;
    }
}

struct md_spans
{
    struct gui_span* spans;
    int count, max;
};

static void md_emit_span(void* ctx, int i, int n, uint32_t fg, int delim)
{
    struct md_spans* s = ctx;
    if (delim)
        fg |= GUI_SPAN_HIDDEN;   /* gone in a read-only editor */
    if (s->count > 0 && s->spans[s->count - 1].fg == fg && s->spans[s->count - 1].start + s->spans[s->count - 1].len == i)
        s->spans[s->count - 1].len += n;
    else if (s->count < s->max)
        s->spans[s->count++] = (struct gui_span){ i, n, fg };
}

int ide_highlight_string(void* ctx, const char* line, int len, int* state,
                         struct gui_span* spans, int max)
{
    const struct gui_theme* t = ctx;
    (void)line;
    (void)state;
    if (max < 1 || len == 0)
        return 0;
    spans[0].start = 0;
    spans[0].len = len;
    spans[0].fg = t->editor_string_fg;
    return 1;
}

int ide_highlight_md(void* ctx, const char* line, int len, int* state,
                      struct gui_span* spans, int max)
{
    struct md_spans s = { spans, 0, max };
    md_scan(ctx, line, len, state, md_emit_span, &s);
    return s.count;
}

uint32_t ide_md_row_bg(void* ctx, const char* line, int len, int state)
{
    const struct gui_theme* t = ctx;
    int j = 0;
    while (j < len && (line[j] == ' ' || line[j] == '\t'))
        j++;
    int fence = len - j >= 3 && line[j] == '`' && line[j + 1] == '`' && line[j + 2] == '`';
    return state || fence ? t->md_code_bg : 0;
}
