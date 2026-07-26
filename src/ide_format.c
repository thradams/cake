#pragma safety enable

#include "ownership.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "ide_format.h"


typedef enum {
    TK_EOF,
    TK_IDENT,
    TK_KEYWORD,
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
    int nl_before;    /* number of newlines seen before this token (0 = same line as previous) */
    int space_before; /* was there whitespace (excluding newline) before this token, same line */
    int glue;         /* set during formatting: 1 = nothing should follow this token with a
                          leading space (e.g. '(' , '.' , '->' , a resolved prefix '-'/'+'/'*'/'&').
                          Left at 0 (its zero-initialized default) until the formatting pass
                          resolves it for punctuators where it matters. */
} Token;

static const char* keywords[] = {
    /* C89/C99/C11/C17/C23 keywords, including underscore-prefixed forms */
    "auto","break","case","char","const","continue","default","do",
    "double","else","enum","extern","float","for","goto","if",
    "inline","int","long","register","restrict","return","short",
    "signed","sizeof","static","struct","switch","typedef","union",
    "unsigned","void","volatile","while",
    "_Alignas","_Alignof","_Atomic","_Bool","_Complex","_Generic",
    "_Imaginary","_Noreturn","_Static_assert","_Thread_local",
    /* C23 */
    "alignas","alignof","bool","true","false","nullptr","thread_local",
    "static_assert","typeof","typeof_unqual","constexpr",
    NULL
};

static int is_keyword(const char* s, int len)
{
    for (int i = 0; keywords[i]; i++)
    {
        if ((int)strlen(keywords[i]) == len && memcmp(keywords[i], s, len) == 0)
            return 1;
    }
    return 0;
}

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
        if (!v->items) { fprintf(stderr, "format: out of memory\n"); exit(1); }
    }
    v->items[v->count++] = t;
}

/* Recognize the longest matching multi-character punctuator starting at p.
 * Returns its length (>=1). This list intentionally excludes '#' and '##'
 * since '#' is only meaningful at the start of a preprocessor line, which
 * is tokenized separately as TK_PREPROC. */
static int punct_len(const char* p, const char* end)
{
    struct { const char* s; int n; } three[] = {
        {"...", 3}, {"<<=", 3}, {">>=", 3}, {NULL, 0}
    };
    struct { const char* s; int n; } two[] = {
        {"->",2},{"++",2},{"--",2},{"<<",2},{">>",2},{"<=",2},{">=",2},
        {"==",2},{"!=",2},{"&&",2},{"||",2},{"+=",2},{"-=",2},{"*=",2},
        {"/=",2},{"%=",2},{"&=",2},{"|=",2},{"^=",2},{"::",2},{NULL,0}
    };
    for (int i = 0; three[i].s; i++)
    {
        int n = three[i].n;
        if (end - p >= n && memcmp(p, three[i].s, n) == 0) return n;
    }
    for (int i = 0; two[i].s; i++)
    {
        int n = two[i].n;
        if (end - p >= n && memcmp(p, two[i].s, n) == 0) return n;
    }
    return 1;
}

static TokVec tokenize(const char* src, size_t len)
{
    TokVec v = { 0 };
    const char* p = src;
    const char* end = src + len;
    int nl_pending = 0;
    int space_pending = 0;
    int at_line_start = 1; /* true if only whitespace seen so far on this line */

    while (p < end)
    {
        char c = *p;

        if (c == '\n')
        {
            nl_pending++;
            space_pending = 0;
            at_line_start = 1;
            p++;
            continue;
        }
        if (c == ' ' || c == '\t' || c == '\r' || c == '\v' || c == '\f')
        {
            space_pending = 1;
            p++;
            continue;
        }

        /* Preprocessor directive: from '#' (first non-blank on the line)
         * to the end of the (possibly backslash-continued) logical line. */
        if (c == '#' && at_line_start)
        {
            const char* start = p;
            while (p < end)
            {
                if (*p == '\\' && p + 1 < end && p[1] == '\n')
                {
                    p += 2; /* line continuation: swallow, keep scanning same directive */
                    continue;
                }
                if (*p == '\n') break;
                p++;
            }
            Token t = { TK_PREPROC, start, (int)(p - start), nl_pending, space_pending, 0 };
            tv_push(&v, t);
            nl_pending = 0; space_pending = 0; at_line_start = 0;
            continue;
        }

        at_line_start = 0;

        /* Line comment */
        if (c == '/' && p + 1 < end && p[1] == '/')
        {
            const char* start = p;
            p += 2;
            while (p < end && *p != '\n') p++;
            Token t = { TK_LINE_COMMENT, start, (int)(p - start), nl_pending, space_pending, 0 };
            tv_push(&v, t);
            nl_pending = 0; space_pending = 0;
            continue;
        }

        /* Block comment */
        if (c == '/' && p + 1 < end && p[1] == '*')
        {
            const char* start = p;
            p += 2;
            while (p < end && !(*p == '*' && p + 1 < end && p[1] == '/')) p++;
            if (p < end) p += 2; /* consume closing */
            Token t = { TK_BLOCK_COMMENT, start, (int)(p - start), nl_pending, space_pending, 0 };
            tv_push(&v, t);
            nl_pending = 0; space_pending = 0;
            continue;
        }

        /* String literal (with optional u8/u/U/L prefix already consumed as ident normally;
         * handle prefixed forms by checking ahead from an identifier - simpler: just handle
         * the quote itself here, prefixes are separate ident tokens if not fused visually,
         * but conventionally u8".." etc. appear with no space, so we special-case them). */
        if (c == '"' || (c == '\''))
        {
            const char* start = p;
            char quote = c;
            p++;
            while (p < end && *p != quote)
            {
                if (*p == '\\' && p + 1 < end) p += 2;
                else if (*p == '\n') break; /* unterminated - bail to be safe */
                else p++;
            }
            if (p < end && *p == quote) p++;
            Token t = { quote == '"' ? TK_STRING : TK_CHAR, start, (int)(p - start), nl_pending, space_pending, 0 };
            tv_push(&v, t);
            nl_pending = 0; space_pending = 0;
            continue;
        }

        /* Identifier / keyword (also swallows string/char prefixes like u8"..", L'x') */
        if (is_ident_start((unsigned char)c))
        {
            const char* start = p;
            while (p < end && is_ident_char((unsigned char)*p)) p++;
            /* Fused literal prefix: u8"...", u"...", U"...", L"...", u8'.', etc. */
            if (p < end && (*p == '"' || *p == '\''))
            {
                char quote = *p;
                const char* lit_start = start;
                p++;
                while (p < end && *p != quote)
                {
                    if (*p == '\\' && p + 1 < end) p += 2;
                    else if (*p == '\n') break;
                    else p++;
                }
                if (p < end && *p == quote) p++;
                Token t = { quote == '"' ? TK_STRING : TK_CHAR, lit_start, (int)(p - lit_start), nl_pending, space_pending, 0 };
                tv_push(&v, t);
                nl_pending = 0; space_pending = 0;
                continue;
            }
            int idlen = (int)(p - start);
            Token t = { is_keyword(start, idlen) ? TK_KEYWORD : TK_IDENT, start, idlen, nl_pending, space_pending, 0 };
            tv_push(&v, t);
            nl_pending = 0; space_pending = 0;
            continue;
        }

        /* Number (int/float, hex, exponents, suffixes; permissive scan) */
        if (isdigit((unsigned char)c) ||
            (c == '.' && p + 1 < end && isdigit((unsigned char)p[1])))
        {
            const char* start = p;
            p++;
            while (p < end)
            {
                char d = *p;
                if (isalnum((unsigned char)d) || d == '.') { p++; continue; }
                if ((d == '+' || d == '-') && p > start &&
                    (p[-1] == 'e' || p[-1] == 'E' || p[-1] == 'p' || p[-1] == 'P'))
                {
                    p++; continue;
                }
                break;
            }
            Token t = { TK_NUMBER, start, (int)(p - start), nl_pending, space_pending, 0 };
            tv_push(&v, t);
            nl_pending = 0; space_pending = 0;
            continue;
        }

        /* Punctuation / operators */
        {
            int n = punct_len(p, end);
            Token t = { TK_PUNCT, p, n, nl_pending, space_pending, 0 };
            tv_push(&v, t);
            nl_pending = 0; space_pending = 0;
            p += n;
        }
    }

    Token eof = { TK_EOF, end, 0, nl_pending, space_pending, 0 };
    tv_push(&v, eof);
    return v;
}


static int tok_is(const Token* t, const char* s)
{
    int n = (int)strlen(s);
    return t->len == n && memcmp(t->text, s, n) == 0;
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
        while (nc < o->len + extra + 1) nc *= 2;
        o->buf = realloc(o->buf, nc);
        if (!o->buf) { fprintf(stderr, "format: out of memory\n"); exit(1); }
        o->cap = nc;
    }
}

static void out_char(Out* o, char c)
{
    out_reserve(o, 1);
    o->buf[o->len++] = c;
}

static void out_str(Out* o, const char* s, int n)
{
    out_reserve(o, (size_t)n);
    memcpy(o->buf + o->len, s, (size_t)n);
    o->len += (size_t)n;
}

static void out_repeat(Out* o, char c, int n)
{
    out_reserve(o, (size_t)(n > 0 ? n : 0));
    for (int i = 0; i < n; i++) o->buf[o->len++] = c;
}


/* Decide whether a unary +, -, *, & should be treated as unary (prefix)
 * based on what came immediately before it in the real token stream. */
static int prev_makes_unary_context(const Token* prev)
{
    if (!prev) return 1;
    if (prev->type == TK_PUNCT)
    {
        if (tok_is(prev, ")") || tok_is(prev, "]")) return 0;
        return 1; /* after '(' ',' '=' other operators etc: unary context */
    }
    if (prev->type == TK_KEYWORD) return 1; /* e.g. "return -1", "int *ptr" */
    if (prev->type == TK_IDENT || prev->type == TK_NUMBER ||
        prev->type == TK_STRING || prev->type == TK_CHAR)
        return 0;
    return 1;
}

/* Returns 1 if a single space should be printed between prev and cur
 * (both on the same source line).
 *
 * Design: rather than re-deriving whether `prev` was a "tight" prefix
 * punctuator (which would require knowing what came *before* prev), the
 * formatting pass resolves each punctuator's `glue` flag exactly once,
 * when it is `cur`, using the real token that preceded it. Once resolved,
 * `prev->glue` is authoritative for whether the *next* token should be
 * glued to it - no re-derivation, no stale logic. */
static int need_space(const Token* prev, const Token* cur)
{
    if (!prev) return 0;

    /* Comments always get a separating space from whatever precedes them. */
    if (cur->type == TK_LINE_COMMENT || cur->type == TK_BLOCK_COMMENT)
        return 1;
    if (prev->type == TK_LINE_COMMENT || prev->type == TK_BLOCK_COMMENT)
        return 1;

    /* ----- Preserve original spacing around ALL punctuation (operators, delimiters) ----- */
    if (cur->type == TK_PUNCT)
        return cur->space_before;
    if (prev->type == TK_PUNCT)
        return cur->space_before;   // space after prev is space before cur
    /* ---------------------------------------------------------------------------------- */

    /* From here on, only non‑punctuation tokens remain (identifiers, keywords, numbers, etc.) */

    if (prev->glue) return 0;

    /* Punctuation cases that involve non‑punctuation tokens (e.g., after a keyword) */
    if (cur->type == TK_PUNCT)  // not reached due to early return, but kept for clarity
        return cur->space_before;

    if (prev->type == TK_PUNCT &&
        (tok_is(prev, ";") || tok_is(prev, ",") || tok_is(prev, "{") || tok_is(prev, "}")))
        return 1;

    if (cur->type == TK_PUNCT && tok_is(cur, "}"))
        return 1;

    /* Two identifier/keyword-ish tokens in a row (e.g. "unsigned int",
     * "struct Foo") always need a space. */
    if ((prev->type == TK_IDENT || prev->type == TK_KEYWORD || prev->type == TK_NUMBER) &&
        (cur->type == TK_IDENT || cur->type == TK_KEYWORD || cur->type == TK_NUMBER))
        return 1;

    /* Default for anything else adjoining a value: one space. */
    return 1;
}

/* Resolve `t`'s glue flag now that we know the real previous token. Only
 * meaningful for punctuators; everything else keeps glue == 0. */
static void resolve_glue(Token* t, const Token* prev)
{
    if (t->type != TK_PUNCT) return;

    if (tok_is(t, "(") || tok_is(t, "[") || tok_is(t, ".") || tok_is(t, "->") ||
        tok_is(t, "!") || tok_is(t, "~") || tok_is(t, "++") || tok_is(t, "--"))
    {
        t->glue = 1;
        return;
    }

    if (tok_is(t, "*") || tok_is(t, "&"))
    {
        /* Declarator-star vs multiplication is genuinely ambiguous without a
         * real parser (both have a "value-like" token before them). We bias
         * toward the declarator/deref reading: glue to whatever follows
         * ("int *ptr", "&x"). This means hand-written multiplication like
         * "a*b" comes out as "a *b" rather than "a * b" - a known,
         * documented trade-off (see file header). */
        t->glue = 1;
        return;
    }


    if (tok_is(t, "+") || tok_is(t, "-"))
    {
        t->glue = prev_makes_unary_context(prev);
        return;
    }
    /* everything else: glue stays 0, ordinary spacing rules apply */
}

/* ------------------------------------------------------------------ */
/* Label detection (case / default / "ident:" goto-labels)             */
/* ------------------------------------------------------------------ */

static int starts_case_or_default(const Token* toks, size_t i, size_t n)
{
    (void)n;
    return toks[i].type == TK_KEYWORD &&
        (tok_is(&toks[i], "case") || tok_is(&toks[i], "default"));
}

/* "ident :" at the start of a line, where the ':' is not "::" and is
 * followed by something other than another ':' - heuristic goto-label.
 * We deliberately do NOT try to distinguish this from a bitfield
 * declaration's ':' (e.g. "int x : 4;") when it's the first token on a
 * line - that pattern is rare in practice, and mislabeling it only costs
 * one indent level, not correctness of the emitted code. */
static int starts_goto_label(const Token* toks, size_t i, size_t n)
{
    if (toks[i].type != TK_IDENT) return 0;
    if (i + 1 >= n) return 0;
    const Token* nx = &toks[i + 1];
    if (nx->nl_before != 0) return 0; /* colon must be same line */
    if (nx->type != TK_PUNCT || !tok_is(nx, ":")) return 0;
    return 1;
}


/* Returns the next token that isn't a comment (or NULL at EOF/end).
 * Used to peek past an if/for/while/else/do header to see whether its
 * body is a '{' block or a brace-less single statement. */
static const Token* next_real_token(const TokVec* v, size_t i)
{
    for (size_t j = i + 1; j < v->count; j++)
    {
        if (v->items[j].type == TK_EOF) return NULL;
        if (v->items[j].type == TK_LINE_COMMENT || v->items[j].type == TK_BLOCK_COMMENT)
            continue;
        return &v->items[j];
    }
    return NULL;
}

static void emit_indent(Out* o, const Options* opt, int level)
{
    if (level < 0) level = 0;
    if (opt->use_tabs)
    {
        out_repeat(o, '\t', level);
    }
    else
    {
        out_repeat(o, ' ', level * opt->indent_width);
    }
}

static Out format_tokens(TokVec* v, const Options* opt)
{
    Out o = { 0 };
    int depth = 0;        /* brace nesting depth */
    int paren_depth = 0;  /* unmatched ( / [ depth, for continuation-line bump */
    const Token* _Opt prev = NULL;
    int first = 1;

    /* --- brace-less single-statement body tracking (if/for/while/else/do) --- */
    int pend = 0;             /* extra indent levels owed to brace-less bodies */
    int header_pending = 0;   /* just saw if/for/while, waiting for its ')' to close */
    int suppress = 0;         /* >0 while inside a brace that will clear `pend` on close */
    int* owns = malloc(sizeof(int) * (v->count + 1));
    size_t owns_top = 0;
    if (!owns) { fprintf(stderr, "format: out of memory\n"); exit(1); }

    for (size_t i = 0; i < v->count; i++)
    {
        Token* t = &v->items[i];
        if (t->type == TK_EOF) break;

        int this_depth = depth;

        /* Closing brace dedents its own line. */
        if (t->type == TK_PUNCT && tok_is(t, "}"))
            this_depth = depth - 1;

        /* case/default/goto-labels sit one level back from the code they head. */
        int is_label_start = 0;
        if (t->nl_before > 0 || first)
        {
            if (starts_case_or_default(v->items, i, v->count) ||
                starts_goto_label(v->items, i, v->count))
            {
                is_label_start = 1;
            }
        }
        if (is_label_start) this_depth -= 1;

        int extra = (paren_depth > 0) ? 1 : 0;
        this_depth += extra;
        this_depth += pend;
        if (this_depth < 0) this_depth = 0;

        if (t->nl_before > 0 && !first)
        {
            int max_blanks = opt->max_blank_lines > 0 ? opt->max_blank_lines : 1;
            int blanks = t->nl_before - 1;
            if (blanks > max_blanks) blanks = max_blanks;
            int total_nl = 1 + blanks;
            out_repeat(&o, '\n', total_nl);
            emit_indent(&o, opt, this_depth);
        }
        else if (!first)
        {
            if (need_space(prev, t)) out_char(&o, ' ');
        }
        else
        {
            emit_indent(&o, opt, this_depth);
        }

        out_str(&o, t->text, t->len);

        resolve_glue(t, prev);

        /* Update nesting state for subsequent tokens. */
        if (t->type == TK_PUNCT)
        {
            if (tok_is(t, "{"))
            {
                depth++;
                /* If a brace-less header's body turns out to be this very
                 * block, this brace "owns" clearing `pend` once it closes. */
                int owns_it = (pend > 0) ? 1 : 0;
                owns[owns_top++] = owns_it;
                if (owns_it) suppress++;
            }
            else if (tok_is(t, "}"))
            {
                if (depth > 0) depth--;
                if (owns_top > 0)
                {
                    int owns_it = owns[--owns_top];
                    if (owns_it)
                    {
                        pend = 0;
                        if (suppress > 0) suppress--;
                    }
                }
            }
            else if (tok_is(t, "(") || tok_is(t, "["))
            {
                paren_depth++;
            }
            else if (tok_is(t, ")") || tok_is(t, "]"))
            {
                if (paren_depth > 0) paren_depth--;
                if (tok_is(t, ")") && header_pending && paren_depth == 0)
                {
                    header_pending = 0;
                    const Token* nxt = next_real_token(v, i);
                    if (!(nxt && nxt->type == TK_PUNCT && tok_is(nxt, "{")))
                        pend++;
                }
            }
            else if (tok_is(t, ";") && paren_depth == 0 && suppress == 0 && pend > 0)
            {
                pend = 0;
            }
        }
        else if (t->type == TK_KEYWORD)
        {
            if (tok_is(t, "if") || tok_is(t, "for") || tok_is(t, "while"))
            {
                header_pending = 1;
            }
            else if (tok_is(t, "else") || tok_is(t, "do"))
            {
                const Token* nxt = next_real_token(v, i);
                int followed_by_brace = nxt && nxt->type == TK_PUNCT && tok_is(nxt, "{");
                int followed_by_else_if = tok_is(t, "else") && nxt && nxt->type == TK_KEYWORD && tok_is(nxt, "if");
                if (!followed_by_brace && !followed_by_else_if)
                    pend++;
            }
        }

        prev = t;
        first = 0;
    }

    free(owns);
    out_char(&o, '\n');
    return o;
}

char* format_c_source(const char* src, size_t len, const Options* opt, size_t* out_len)
{
    TokVec toks = tokenize(src, len);
    Out result = format_tokens(&toks, opt);
    free(toks.items);

    out_char(&result, '\0');
    result.len--; /* don't count the NUL in the reported length */

    if (out_len) *out_len = result.len;
    return result.buf;
}
