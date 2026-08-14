#pragma safety enable

/* Indent-only C formatter.
 *
 * The one rule this file obeys: it never changes code, it only changes the
 * leading whitespace of a line. Everything from the first non-blank
 * character of a line to its end is copied through byte for byte - spacing
 * around operators, alignment of trailing comments, blank lines, line
 * breaks and where the braces sit are all left exactly as the author wrote
 * them.
 *
 * Indentation depth comes from '{' / '}' nesting and nothing else: a line
 * inside n unclosed braces gets n indent levels, and a line that starts
 * with '}' gets n-1 so it lines up with the '{' that opened it. Wrap a
 * region in a new pair of braces and run this, and the whole region slides
 * in by one level with its internal shape intact.
 *
 * Only a line that opens a statement is placed at its brace level. A line
 * that continues the statement above it moves by the same amount that
 * statement moved, so hand-made layout survives untouched:
 *
 *     a =                     {
 *       1;         becomes        a =
 *                                   1;
 *                             }
 *
 * That is also how a switch body is handled, but for the whole block: the
 * braces of a switch make everything between them rigid, moving as one
 * piece. Nothing inside a switch is re-anchored, so "case" keeps whatever
 * column it was written in - this code base uses two different styles for
 * it and neither is going to be rewritten into the other.
 *
 * Lines that are left completely untouched (not even re-indented):
 *   - blank lines,
 *   - preprocessor directives (their column is often meaningful to the
 *     reader, and a '{' inside a macro body must not move real code),
 *   - continuation lines of a multi-line string literal, and any line that
 *     a token started on an earlier line runs into. Re-indenting those
 *     would edit the literal's own content. The interior lines of a block
 *     comment are the one exception: they travel with the comment so its
 *     internal alignment is kept.
 *
 * The tokenizer exists purely so that braces inside comments, string
 * literals, character constants and preprocessor directives do not move
 * the indent level; see compute_levels() for how the braces of an
 * #if/#else arm are kept from leaking a phantom level into the rest of
 * the file.
 */

#include "ownership.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ide_format.h"

typedef enum {
    TK_EOF,
    TK_IDENT,
    TK_NUMBER,
    TK_STRING,
    TK_CHAR,
    TK_PUNCT,
    TK_LINE_COMMENT,
    TK_BLOCK_COMMENT,
    TK_PREPROC
} TokType;

typedef struct {
    TokType type;
    const char* text;
    int len;
} Token;

static int is_ident_start(int c) { return isalpha((unsigned char)c) || c == '_' || c == '$'; }
static int is_ident_char(int c) { return isalnum((unsigned char)c) || c == '_' || c == '$'; }

typedef struct {
    Token* _Owner _Opt items;
    size_t count;
    size_t cap;
} TokVec;

static void tv_push(TokVec* v, Token t)
{
    if (v->count == v->cap)
    {
        v->cap = v->cap ? v->cap * 2 : 1024;
        v->items = realloc(v->items, v->cap * sizeof(Token));
        if (!v->items)
        {
            fprintf(stderr, "format: out of memory\n");
            exit(1);
        }
    }
    v->items[v->count++] = t;
}

/* Tokens are only ever inspected for '{' and '}', so everything else is
 * lumped together as coarsely as possible - the scan just has to know how
 * far each construct reaches so braces hiding inside one are skipped. */
static TokVec tokenize(const char* src, size_t len)
{
    TokVec v = { 0 };
    const char* p = src;
    const char* end = src + len;
    int at_line_start = 1; /* only whitespace seen so far on this line */

    while (p < end)
    {
        char c = *p;

        if (c == '\n')
        {
            at_line_start = 1;
            p++;
            continue;
        }
        if (c == ' ' || c == '\t' || c == '\r' || c == '\v' || c == '\f')
        {
            p++;
            continue;
        }

        /* Preprocessor directive: '#' as the first non-blank on the line
         * through the end of the backslash-continued logical line. */
        if (c == '#' && at_line_start)
        {
            const char* start = p;
            while (p < end)
            {
                if (*p == '\\' && p + 1 < end && p[1] == '\n')
                {
                    p += 2; /* line continuation: still the same directive */
                    continue;
                }
                /* A block comment opened here runs past the end of the
                 * directive line, so swallow it whole - otherwise its tail
                 * would come back as stray '*' and '/' code tokens. */
                if (*p == '/' && p + 1 < end && p[1] == '*')
                {
                    p += 2;
                    while (p < end && !(*p == '*' && p + 1 < end && p[1] == '/'))
                    {
                        p++;
                    }
                    if (p < end)
                    {
                        p += 2;
                    }
                    continue;
                }
                if (*p == '\n')
                {
                    break;
                }
                p++;
            }
            Token t = { TK_PREPROC, start, (int)(p - start) };
            tv_push(&v, t);
            at_line_start = 0;
            continue;
        }

        at_line_start = 0;

        /* Line comment */
        if (c == '/' && p + 1 < end && p[1] == '/')
        {
            const char* start = p;
            p += 2;
            while (p < end && *p != '\n')
            {
                p++;
            }
            Token t = { TK_LINE_COMMENT, start, (int)(p - start) };
            tv_push(&v, t);
            continue;
        }

        /* Block comment (may span lines) */
        if (c == '/' && p + 1 < end && p[1] == '*')
        {
            const char* start = p;
            p += 2;
            while (p < end && !(*p == '*' && p + 1 < end && p[1] == '/'))
            {
                p++;
            }
            if (p < end)
            {
                p += 2; /* consume the closing */
            }
            Token t = { TK_BLOCK_COMMENT, start, (int)(p - start) };
            tv_push(&v, t);
            continue;
        }

        /* String or character literal */
        if (c == '"' || c == '\'')
        {
            const char* start = p;
            char quote = c;
            p++;
            while (p < end && *p != quote)
            {
                if (*p == '\\' && p + 1 < end)
                {
                    p += 2; /* also covers a backslash-newline continuation */
                }
                else if (*p == '\n')
                {
                    break; /* unterminated: bail out rather than run away */
                }
                else
                {
                    p++;
                }
            }
            if (p < end && *p == quote)
            {
                p++;
            }
            Token t = { quote == '"' ? TK_STRING : TK_CHAR, start, (int)(p - start) };
            tv_push(&v, t);
            continue;
        }

        /* Identifier / keyword / number, plus prefixed literals (u8"..", L'x') */
        if (is_ident_start((unsigned char)c) || isdigit((unsigned char)c))
        {
            const char* start = p;
            while (p < end && (is_ident_char((unsigned char)*p) || *p == '.'))
            {
                p++;
            }
            if (p < end && (*p == '"' || *p == '\''))
            {
                char quote = *p;
                p++;
                while (p < end && *p != quote)
                {
                    if (*p == '\\' && p + 1 < end)
                    {
                        p += 2;
                    }
                    else if (*p == '\n')
                    {
                        break;
                    }
                    else
                    {
                        p++;
                    }
                }
                if (p < end && *p == quote)
                {
                    p++;
                }
                Token t = { quote == '"' ? TK_STRING : TK_CHAR, start, (int)(p - start) };
                tv_push(&v, t);
                continue;
            }
            Token t = { isdigit((unsigned char)c) ? TK_NUMBER : TK_IDENT, start, (int)(p - start) };
            tv_push(&v, t);
            continue;
        }

        /* Anything else is punctuation, one character at a time. Multi-char
         * operators need no special handling here: none of them contain a
         * brace, so splitting them changes nothing. */
        {
            Token t = { TK_PUNCT, p, 1 };
            tv_push(&v, t);
            p++;
        }
    }

    Token eof = { TK_EOF, end, 0 };
    tv_push(&v, eof);
    return v;
}

typedef struct {
    char* _Opt _Owner buf;
    size_t len;
    size_t cap;
} Out;

static void out_reserve(Out* o, size_t extra)
{
    if (o->len + extra + 1 > o->cap)
    {
        size_t nc = o->cap ? o->cap * 2 : 4096;
        while (nc < o->len + extra + 1)
        {
            nc *= 2;
        }
        o->buf = realloc(o->buf, nc);
        if (!o->buf)
        {
            fprintf(stderr, "format: out of memory\n");
            exit(1);
        }
        o->cap = nc;
    }
}

static void out_str(Out* o, const char* s, size_t n)
{
    out_reserve(o, n);
    memcpy(o->buf + o->len, s, n);
    o->len += n;
}

static void out_char(Out* o, char c)
{
    out_reserve(o, 1);
    o->buf[o->len++] = c;
}

static void emit_columns(Out* o, const Options* opt, int columns, int width)
{
    if (columns < 0)
    {
        columns = 0;
    }
    if (opt->use_tabs)
    {
        int tabs = columns / width;
        int spaces = columns % width; /* whatever doesn't fill a whole tab */
        out_reserve(o, (size_t)(tabs + spaces));
        for (int i = 0; i < tabs; i++)
        {
            o->buf[o->len++] = '\t';
        }
        for (int i = 0; i < spaces; i++)
        {
            o->buf[o->len++] = ' ';
        }
    }
    else
    {
        out_reserve(o, (size_t)columns);
        for (int i = 0; i < columns; i++)
        {
            o->buf[o->len++] = ' ';
        }
    }
}

/* Width in columns of the blank run in front of `first`, with tabs
 * advancing to the next multiple of the indent width. */
static int leading_columns(const char* line_start, const char* first, int width)
{
    int col = 0;
    for (const char* p = line_start; p < first; p++)
    {
        if (*p == '\t')
        {
            col += width - (col % width);
        }
        else
        {
            col++;
        }
    }
    return col;
}

/* Does the line whose first token is toks[i] begin a new statement, or is
 * it the continuation of one that started further up?
 *
 * The test is what the previous token was, ignoring comments: after ';',
 * '{', '}', ':' or a directive a new statement begins, and anything else
 * ('=', ',', '(', an operator, ')' closing an if header, ...) means the
 * line still belongs to the construct above it. A '}' always starts its
 * own line so it can line up with its '{'. */
static int starts_statement(const TokVec* v, size_t i)
{
    if (v->items[i].type == TK_PUNCT && v->items[i].len == 1 && v->items[i].text[0] == '}')
    {
        return 1;
    }

    size_t j = i;
    while (j > 0)
    {
        j--;
        const Token* prev = &v->items[j];
        if (prev->type == TK_LINE_COMMENT || prev->type == TK_BLOCK_COMMENT)
        {
            continue;
        }
        if (prev->type == TK_PREPROC)
        {
            return 1;
        }
        if (prev->type == TK_PUNCT && prev->len == 1)
        {
            char c = prev->text[0];
            return c == ';' || c == '{' || c == '}' || c == ':';
        }
        return 0;
    }
    return 1; /* nothing but comments above: first statement of the file */
}

static int tok_is(const Token* t, const char* s)
{
    size_t n = strlen(s);
    return (size_t)t->len == n && memcmp(t->text, s, n) == 0;
}

static int is_punct(const Token* t, char c)
{
    return t->type == TK_PUNCT && t->len == 1 && t->text[0] == c;
}

/* Which conditional-compilation directive this is, if any: 'i' for
 * #if/#ifdef/#ifndef, 'e' for #else/#elif, 'd' for #endif, 0 otherwise. */
static char directive_kind(const Token* t)
{
    const char* p = t->text + 1; /* past the '#' */
    const char* end = t->text + t->len;
    while (p < end && (*p == ' ' || *p == '\t'))
    {
        p++;
    }
    const char* start = p;
    while (p < end && is_ident_char((unsigned char)*p))
    {
        p++;
    }
    size_t n = (size_t)(p - start);

    if ((n == 2 && memcmp(start, "if", 2) == 0) ||
        (n == 5 && memcmp(start, "ifdef", 5) == 0) ||
        (n == 6 && memcmp(start, "ifndef", 6) == 0))
    {
        return 'i';
    }
    if ((n == 4 && memcmp(start, "else", 4) == 0) ||
        (n == 4 && memcmp(start, "elif", 4) == 0))
    {
        return 'e';
    }
    if (n == 5 && memcmp(start, "endif", 5) == 0)
    {
        return 'd';
    }
    return 0;
}

/* The indent level for the line each token opens, plus a per-token flag
 * marking the tokens inside a switch body.
 *
 * Braces drive the levels: a '{' sits at the level of the statement that
 * introduced it, and its contents sit one level further in.
 *
 * A switch body is the one place that gets no opinion imposed on it. This
 * code base writes "case" both flush with the braces and one level inside
 * them, and there is no way to pick one without rewriting the other, so
 * everything between a switch's braces is marked rigid: it travels with
 * the switch as a block, keeping the exact shape it was written in. */
static void compute_levels(const TokVec* v, int* _Owner* out_levels, int* _Owner* out_rigid)
{
    int* _Owner levels = malloc(sizeof(int) * (v->count + 1));
    int* _Owner rigid = malloc(sizeof(int) * (v->count + 1));
    /* base[d] = level of a plain statement at brace depth d; in_switch[d]
     * says whether depth d lies inside a switch body. */
    int* _Owner base = malloc(sizeof(int) * (v->count + 2));
    int* _Owner in_switch = malloc(sizeof(int) * (v->count + 2));
    if (!levels || !rigid || !base || !in_switch)
    {
        fprintf(stderr, "format: out of memory\n");
        exit(1);
    }

    size_t depth = 0;
    base[0] = 0;
    in_switch[0] = 0;

    /* The token that opened the statement we are inside, so that when a
     * '{' turns up we know whether it belongs to a "switch". */
    const Token* _Opt head = NULL;

    /* Brace depth at each open #if, so the arms of a conditional can be
     * rewound to a common starting point.
     *
     * Two patterns need this. The arms of an #if/#else often open the same
     * block twice, and counting both would bury everything below in a
     * phantom extra level:
     *
     *     #ifndef MOCKFILES
     *         while (fread(&ch, 1, 1, file)) {
     *     #else
     *         while (*pch) {
     *     #endif
     *
     * And a lone arm may open a brace that its own #endif never closes -
     * the "extern C" guard, whose '}' lives under a second #ifdef at the
     * bottom of the file:
     *
     *     #ifdef __cplusplus
     *     extern "C" {
     *     #endif
     *
     * So: an #else rewinds to where the #if began, and an #endif rewinds
     * too when there was no #else - a single arm that fails to balance is
     * a conditional brace, not a real level for the rest of the file. When
     * there was an #else, whatever the last arm left standing is kept,
     * since both arms agreeing on an open brace is the first pattern. */
    size_t* _Owner cond_depth = malloc(sizeof(size_t) * (v->count + 2));
    int* _Owner cond_saw_else = malloc(sizeof(int) * (v->count + 2));
    size_t cond_top = 0;
    if (!cond_depth || !cond_saw_else)
    {
        fprintf(stderr, "format: out of memory\n");
        exit(1);
    }

    for (size_t i = 0; i < v->count; i++)
    {
        const Token* t = &v->items[i];

        if (t->type == TK_PREPROC)
        {
            char kind = directive_kind(t);
            if (kind == 'i')
            {
                cond_saw_else[cond_top] = 0;
                cond_depth[cond_top] = depth;
                cond_top++;
            }
            else if (kind == 'e' && cond_top > 0)
            {
                cond_saw_else[cond_top - 1] = 1;
                depth = cond_depth[cond_top - 1]; /* rewind to the #if */
            }
            else if (kind == 'd' && cond_top > 0)
            {
                cond_top--;
                if (!cond_saw_else[cond_top])
                {
                    depth = cond_depth[cond_top];
                }
            }
            levels[i] = base[depth];
            rigid[i] = in_switch[depth];
            continue;
        }

        if (starts_statement(v, i))
        {
            head = t;
        }

        if (is_punct(t, '{'))
        {
            levels[i] = base[depth];
            rigid[i] = in_switch[depth];
            int opens_switch = head != NULL && head->type == TK_IDENT && tok_is(head, "switch");
            depth++;
            base[depth] = levels[i] + 1;
            /* Once inside a switch, stay rigid all the way down - an inner
             * block re-anchoring itself would undo the preservation. */
            in_switch[depth] = in_switch[depth - 1] || opens_switch;
        }
        else if (is_punct(t, '}'))
        {
            if (depth > 0)
            {
                depth--;
            }
            levels[i] = base[depth];
            rigid[i] = in_switch[depth];
        }
        else
        {
            levels[i] = base[depth];
            rigid[i] = in_switch[depth];
        }
    }

    free(base);
    free(in_switch);
    free(cond_depth);
    free(cond_saw_else);
    *out_levels = levels;
    *out_rigid = rigid;
}

/* One physical line, resolved against the token stream. Both the formatter
 * and the indent detector walk lines through this, so the detector measures
 * exactly the lines the formatter would move. */
typedef struct {
    const char* line_start;
    const char* content;   /* first non-blank char (== line_end if blank) */
    const char* line_end;  /* end of the text, before any CR */
    int has_cr;
    int has_newline;

    /* The token that opens this line, and its index; NULL when the line is
     * blank or is covered by a token that began on an earlier line. */
    const Token* _Opt first;
    size_t index;

    /* A token from an earlier line that reaches into this one: the inside
     * of a block comment or of a continued literal. */
    const Token* _Opt spanning;

    const char* _cursor;
    const char* _end;
    size_t _tok;
} LineIter;

static void line_iter_init(LineIter* it, const char* src, size_t len)
{
    memset(it, 0, sizeof *it);
    it->_cursor = src;
    it->_end = src + len;
}

static int line_iter_next(LineIter* it, const TokVec* v)
{
    if (it->_cursor > it->_end)
    {
        return 0;
    }

    const char* end = it->_end;
    it->line_start = it->_cursor;

    const char* newline = it->line_start;
    while (newline < end && *newline != '\n')
    {
        newline++;
    }
    it->has_newline = newline < end;

    /* A CRLF file keeps its CRLF: the '\r' belongs to the line ending, not
     * to the text, so it is held back here and put out again with the '\n'
     * rather than being trimmed away as trailing blanks. */
    it->line_end = newline;
    it->has_cr = 0;
    if (it->line_end > it->line_start && it->line_end[-1] == '\r')
    {
        it->line_end--;
        it->has_cr = 1;
    }

    /* Walk forward to this line, then look back one token to see whether
     * the one we stepped over reaches into it. */
    while (it->_tok < v->count && v->items[it->_tok].text < it->line_start)
    {
        it->_tok++;
    }
    it->spanning = NULL;
    if (it->_tok > 0)
    {
        const Token* prev = &v->items[it->_tok - 1];
        if (prev->text + prev->len > it->line_start)
        {
            it->spanning = prev;
        }
    }

    it->first = NULL;
    it->index = it->_tok;
    if (it->_tok < v->count &&
        v->items[it->_tok].type != TK_EOF &&
        v->items[it->_tok].text < it->line_end)
    {
        it->first = &v->items[it->_tok];
    }

    it->content = it->line_start;
    while (it->content < it->line_end &&
           (*it->content == ' ' || *it->content == '\t'))
    {
        it->content++;
    }

    it->_cursor = newline + 1;
    return 1;
}

/* Number of leading spaces on a line, or -1 if its indentation is not
 * made purely of spaces (a tab, or no indentation at all to measure). */
static int leading_space_run(const LineIter* it)
{
    if (it->content == it->line_start)
    {
        return 0;
    }
    for (const char* p = it->line_start; p < it->content; p++)
    {
        if (*p != ' ')
        {
            return -1;
        }
    }
    return (int)(it->content - it->line_start);
}

/* Guesses the indentation the source already uses.
 *
 * The measurement only looks at lines the formatter would anchor - a line
 * that opens a statement, outside a switch, not a directive and not inside
 * a comment - and pairs each with the previous such line. When two of them
 * sit one brace level apart, the gap between their columns is one indent
 * step, and the most frequently occurring gap wins. Continuation lines and
 * comment interiors are never counted: those carry hand-made alignment
 * that has nothing to do with the indent size.
 *
 * Returns 1 when there was enough evidence to call it, 0 when the source
 * has too little indentation to tell (*out is then left alone). */
static int detect_indent(const char* src, size_t len, const TokVec* toks,
                         const int* levels, const int* rigid, Options* out)
{
    enum { MAX_STEP = 16 };
    int votes[MAX_STEP + 1] = { 0 };
    int tab_lines = 0;
    int space_lines = 0;

    int have_prev = 0;
    int prev_level = 0;
    int prev_columns = 0;
    int samples = 0;

    LineIter it;
    line_iter_init(&it, src, len);
    while (line_iter_next(&it, toks))
    {
        if (it.content >= it.line_end)
        {
            continue; /* blank */
        }

        /* Which character the indentation is made of is worth counting on
         * every indented line, whether or not it can be measured. */
        if (it.content > it.line_start)
        {
            if (*it.line_start == '\t')
            {
                tab_lines++;
            }
            else
            {
                space_lines++;
            }
        }

        if (it.spanning != NULL || it.first == NULL ||
            it.first->type == TK_PREPROC)
        {
            continue;
        }
        if (rigid[it.index] || !starts_statement(toks, it.index))
        {
            continue;
        }

        int columns = leading_space_run(&it);
        if (columns < 0)
        {
            have_prev = 0; /* tab-indented: no column to compare */
            continue;
        }

        int level = levels[it.index];
        if (have_prev && level - prev_level == 1 && columns > prev_columns)
        {
            int step = columns - prev_columns;
            if (step <= MAX_STEP)
            {
                votes[step]++;
                samples++;
            }
        }
        else if (have_prev && prev_level - level == 1 && prev_columns > columns)
        {
            int step = prev_columns - columns;
            if (step <= MAX_STEP)
            {
                votes[step]++;
                samples++;
            }
        }

        have_prev = 1;
        prev_level = level;
        prev_columns = columns;
    }

    if (tab_lines > space_lines)
    {
        /* Tabs carry one level each, so there is no width to measure - the
         * width only survives as the tab stop used to line comments up. */
        out->use_tabs = 1;
        if (out->indent_width <= 0)
        {
            out->indent_width = 4;
        }
        return 1;
    }

    if (samples == 0)
    {
        return 0;
    }

    int best = 0;
    for (int step = 1; step <= MAX_STEP; step++)
    {
        if (votes[step] > votes[best])
        {
            best = step;
        }
    }

    /* One clear winner or nothing. A file whose steps are all over the
     * place has no indent size to report, and guessing at one would only
     * push the caller into reformatting it to a width it never used. */
    if (best == 0 || votes[best] * 2 <= samples)
    {
        return 0;
    }

    out->use_tabs = 0;
    out->indent_width = best;
    return 1;
}

int format_detect_indent(const char* src, size_t len, Options* out)
{
    TokVec toks = tokenize(src, len);
    int* _Owner levels;
    int* _Owner rigid;
    compute_levels(&toks, &levels, &rigid);

    int found = detect_indent(src, len, &toks, levels, rigid, out);

    free(levels);
    free(rigid);
    free(toks.items);
    return found;
}

char* format_c_source(const char* src, size_t len, const Options* opt, size_t* out_len)
{
    TokVec toks = tokenize(src, len);
    int* _Owner levels;
    int* _Owner rigid;
    compute_levels(&toks, &levels, &rigid);

    /* indent_width 0 means "keep doing whatever this file already does". */
    Options effective = *opt;
    if (effective.indent_width <= 0)
    {
        effective.indent_width = 4;
        effective.use_tabs = opt->use_tabs;
        if (!detect_indent(src, len, &toks, levels, rigid, &effective))
        {
            effective.indent_width = 4;
        }
    }
    opt = &effective;

    Out o = { 0 };
    const int width = effective.indent_width;

    /* How far the line that opened the current statement moved. Its
     * continuation lines move by the same amount, so the shape the author
     * gave the statement survives intact - only the whole thing slides. */
    int shift = 0;

    LineIter it;
    line_iter_init(&it, src, len);
    while (line_iter_next(&it, &toks))
    {
        const char* line_start = it.line_start;
        const char* line_end = it.line_end;
        const char* content = it.content;
        const Token* _Opt first = it.first;
        const Token* _Opt spanning = it.spanning;
        size_t tok_index = it.index;

        if (spanning != NULL)
        {
            /* Inside a construct that opened above. Only a block comment
             * may be re-indented, and then only by the same shift as the
             * code it is attached to, so its interior alignment holds. A
             * string literal is content: not one byte of it moves. */
            if (spanning->type == TK_BLOCK_COMMENT && content < line_end)
            {
                int columns = leading_columns(line_start, content, width) + shift;
                emit_columns(&o, opt, columns, width);
                out_str(&o, content, (size_t)(line_end - content));
            }
            else
            {
                out_str(&o, line_start, (size_t)(line_end - line_start));
            }
        }
        else if (first == NULL || first->type == TK_PREPROC)
        {
            /* Blank line, or a directive - both go out untouched. */
            out_str(&o, line_start, (size_t)(line_end - line_start));
            if (first != NULL && !rigid[tok_index])
            {
                shift = 0; /* a directive anchors itself; nothing continues it */
            }
        }
        else
        {
            /* Re-indent: drop the existing leading whitespace, keep every
             * byte from the first token onwards. */
            const char* content_end = line_end;

            /* Trailing whitespace is only ours to trim when the line really
             * ends here - if the last token runs past it, those blanks are
             * inside a comment or a continued literal. */
            size_t last = tok_index;
            while (last + 1 < toks.count && toks.items[last + 1].text < line_end)
            {
                last++;
            }
            const Token* tail = &toks.items[last];
            const char* tail_end = tail->text + tail->len;
            if (tail_end <= line_end)
            {
                while (content_end > first->text &&
                       (content_end[-1] == ' ' || content_end[-1] == '\t'))
                {
                    content_end--;
                }
            }

            int old_columns = leading_columns(line_start, first->text, width);
            int new_columns;
            if (!rigid[tok_index] && starts_statement(&toks, tok_index))
            {
                new_columns = levels[tok_index] * width;
                shift = new_columns - old_columns;
            }
            else
            {
                new_columns = old_columns + shift;
                if (new_columns < 0)
                {
                    new_columns = 0;
                }
            }

            emit_columns(&o, opt, new_columns, width);
            out_str(&o, first->text, (size_t)(content_end - first->text));
        }

        if (it.has_cr)
        {
            out_char(&o, '\r');
        }
        if (it.has_newline)
        {
            out_char(&o, '\n');
        }
    }

    free(levels);
    free(rigid);
    free(toks.items);

    out_char(&o, '\0');
    o.len--; /* the NUL is a terminator, not part of the text */

    if (out_len)
    {
        *out_len = o.len;
    }
    return o.buf;
}
