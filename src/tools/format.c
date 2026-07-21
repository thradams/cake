/*
 * format.c - a heuristic reformatter for C source files.
 *
 * Strategy
 * --------
 * This is NOT a full C parser. It tokenizes the input (strings, chars,
 * comments and preprocessor directives are recognized and copied through
 * mostly verbatim) and then re-emits the token stream with:
 *
 *   - normalized indentation, driven by brace depth ('{' / '}'), with a
 *     one-level bump while inside unmatched parens/brackets on a
 *     continuation line, and a one-level dedent for `case`/`default`/
 *     goto-style labels;
 *   - normalized horizontal spacing between tokens on the same line,
 *     driven by a small table of "does this pair need a space" rules;
 *   - original line breaks are preserved (we don't reflow/word-wrap),
 *     but runs of more than one blank line are collapsed to one.
 *
 * Usage:
 *   format <input.c> [output.c]
 *   format <input.c> -i          (format in place)
 *   cat input.c | format -       (stdin -> stdout)
 *
 * Options:
 *   -w N   indent width in spaces (default 4)
 *   -t     use tabs for indentation instead of spaces
 *
 * Limitations (documented, not silently papered over):
 *   - Continuation lines inside a single pair of unmatched parens all get
 *     the same +1 indent bump; deep expression trees are not aligned to
 *     the opening column the way clang-format would.
 *   - Label detection ("case", "default", "ident:") is heuristic and can
 *     misfire on unusual code (e.g. bitfields, ternaries split oddly).
 *   - No line wrapping / reflowing of long lines.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/* ------------------------------------------------------------------ */
/* Source buffer                                                       */
/* ------------------------------------------------------------------ */

static char *read_whole_file(FILE *f, size_t *out_len)
{
    size_t cap = 1 << 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) { fprintf(stderr, "format: out of memory\n"); exit(1); }

    for (;;) {
        if (len + 4096 > cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { fprintf(stderr, "format: out of memory\n"); exit(1); }
            buf = nb;
        }
        size_t n = fread(buf + len, 1, 4096, f);
        len += n;
        if (n < 4096) {
            if (feof(f)) break;
            if (ferror(f)) { fprintf(stderr, "format: read error\n"); exit(1); }
        }
    }
    buf[len] = '\0';
    *out_len = len;
    return buf;
}

/* ------------------------------------------------------------------ */
/* Tokenizer                                                           */
/* ------------------------------------------------------------------ */

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
    const char *text;
    int len;
    int nl_before;    /* number of newlines seen before this token (0 = same line as previous) */
    int space_before; /* was there whitespace (excluding newline) before this token, same line */
    int glue;         /* set during formatting: 1 = nothing should follow this token with a
                          leading space (e.g. '(' , '.' , '->' , a resolved prefix '-'/'+'/'*'/'&').
                          Left at 0 (its zero-initialized default) until the formatting pass
                          resolves it for punctuators where it matters. */
} Token;

static const char *keywords[] = {
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

static int is_keyword(const char *s, int len)
{
    for (int i = 0; keywords[i]; i++) {
        if ((int)strlen(keywords[i]) == len && memcmp(keywords[i], s, len) == 0)
            return 1;
    }
    return 0;
}

static int is_ident_start(int c) { return isalpha((unsigned char)c) || c == '_' || c == '$'; }
static int is_ident_char(int c)  { return isalnum((unsigned char)c) || c == '_' || c == '$'; }

typedef struct {
    Token *items;
    size_t count;
    size_t cap;
} TokVec;

static void tv_push(TokVec *v, Token t)
{
    if (v->count == v->cap) {
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
static int punct_len(const char *p, const char *end)
{
    struct { const char *s; int n; } three[] = {
        {"...", 3}, {"<<=", 3}, {">>=", 3}, {NULL, 0}
    };
    struct { const char *s; int n; } two[] = {
        {"->",2},{"++",2},{"--",2},{"<<",2},{">>",2},{"<=",2},{">=",2},
        {"==",2},{"!=",2},{"&&",2},{"||",2},{"+=",2},{"-=",2},{"*=",2},
        {"/=",2},{"%=",2},{"&=",2},{"|=",2},{"^=",2},{"::",2},{NULL,0}
    };
    for (int i = 0; three[i].s; i++) {
        int n = three[i].n;
        if (end - p >= n && memcmp(p, three[i].s, n) == 0) return n;
    }
    for (int i = 0; two[i].s; i++) {
        int n = two[i].n;
        if (end - p >= n && memcmp(p, two[i].s, n) == 0) return n;
    }
    return 1;
}

static TokVec tokenize(const char *src, size_t len)
{
    TokVec v = {0};
    const char *p = src;
    const char *end = src + len;
    int nl_pending = 0;
    int space_pending = 0;
    int at_line_start = 1; /* true if only whitespace seen so far on this line */

    while (p < end) {
        char c = *p;

        if (c == '\n') {
            nl_pending++;
            space_pending = 0;
            at_line_start = 1;
            p++;
            continue;
        }
        if (c == ' ' || c == '\t' || c == '\r' || c == '\v' || c == '\f') {
            space_pending = 1;
            p++;
            continue;
        }

        /* Preprocessor directive: from '#' (first non-blank on the line)
         * to the end of the (possibly backslash-continued) logical line. */
        if (c == '#' && at_line_start) {
            const char *start = p;
            while (p < end) {
                if (*p == '\\' && p + 1 < end && p[1] == '\n') {
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
        if (c == '/' && p + 1 < end && p[1] == '/') {
            const char *start = p;
            p += 2;
            while (p < end && *p != '\n') p++;
            Token t = { TK_LINE_COMMENT, start, (int)(p - start), nl_pending, space_pending, 0 };
            tv_push(&v, t);
            nl_pending = 0; space_pending = 0;
            continue;
        }

        /* Block comment */
        if (c == '/' && p + 1 < end && p[1] == '*') {
            const char *start = p;
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
        if (c == '"' || (c == '\'')) {
            const char *start = p;
            char quote = c;
            p++;
            while (p < end && *p != quote) {
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
        if (is_ident_start((unsigned char)c)) {
            const char *start = p;
            while (p < end && is_ident_char((unsigned char)*p)) p++;
            /* Fused literal prefix: u8"...", u"...", U"...", L"...", u8'.', etc. */
            if (p < end && (*p == '"' || *p == '\'')) {
                char quote = *p;
                const char *lit_start = start;
                p++;
                while (p < end && *p != quote) {
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
            (c == '.' && p + 1 < end && isdigit((unsigned char)p[1]))) {
            const char *start = p;
            p++;
            while (p < end) {
                char d = *p;
                if (isalnum((unsigned char)d) || d == '.' ) { p++; continue; }
                if ((d == '+' || d == '-') && p > start &&
                    (p[-1] == 'e' || p[-1] == 'E' || p[-1] == 'p' || p[-1] == 'P')) {
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

/* ------------------------------------------------------------------ */
/* Helpers for token text comparison                                   */
/* ------------------------------------------------------------------ */

static int tok_is(const Token *t, const char *s)
{
    int n = (int)strlen(s);
    return t->len == n && memcmp(t->text, s, n) == 0;
}

static int tok_is_ctrl_keyword(const Token *t)
{
    return t->type == TK_KEYWORD &&
        (tok_is(t, "if") || tok_is(t, "while") || tok_is(t, "for") ||
         tok_is(t, "switch"));
}

/* ------------------------------------------------------------------ */
/* Output buffer                                                        */
/* ------------------------------------------------------------------ */

typedef struct {
    char *buf;
    size_t len;
    size_t cap;
} Out;

static void out_reserve(Out *o, size_t extra)
{
    if (o->len + extra + 1 > o->cap) {
        size_t nc = o->cap ? o->cap * 2 : 4096;
        while (nc < o->len + extra + 1) nc *= 2;
        o->buf = realloc(o->buf, nc);
        if (!o->buf) { fprintf(stderr, "format: out of memory\n"); exit(1); }
        o->cap = nc;
    }
}

static void out_char(Out *o, char c)
{
    out_reserve(o, 1);
    o->buf[o->len++] = c;
}

static void out_str(Out *o, const char *s, int n)
{
    out_reserve(o, (size_t)n);
    memcpy(o->buf + o->len, s, (size_t)n);
    o->len += (size_t)n;
}

static void out_repeat(Out *o, char c, int n)
{
    out_reserve(o, (size_t)(n > 0 ? n : 0));
    for (int i = 0; i < n; i++) o->buf[o->len++] = c;
}

/* ------------------------------------------------------------------ */
/* Spacing rules                                                       */
/* ------------------------------------------------------------------ */

/* Tokens that, when they are the *previous* token, generally suppress a
 * following space for punctuation like '(' / '[' (function-call style):
 * identifiers, keywords that aren't control keywords, ')' and ']'. */
static int is_value_like_end(const Token *t)
{
    if (t->type == TK_IDENT || t->type == TK_NUMBER ||
        t->type == TK_STRING || t->type == TK_CHAR)
        return 1;
    if (t->type == TK_KEYWORD && !tok_is_ctrl_keyword(t) &&
        !tok_is(t, "return") && !tok_is(t, "sizeof") && !tok_is(t, "case"))
        return 1;
    if (t->type == TK_PUNCT && (tok_is(t, ")") || tok_is(t, "]")))
        return 1;
    return 0;
}

/* Decide whether a unary +, -, *, & should be treated as unary (prefix)
 * based on what came immediately before it in the real token stream. */
static int prev_makes_unary_context(const Token *prev)
{
    if (!prev) return 1;
    if (prev->type == TK_PUNCT) {
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
static int need_space(const Token *prev, const Token *cur)
{
    if (!prev) return 0;

    /* Comments always get a separating space from whatever precedes them. */
    if (cur->type == TK_LINE_COMMENT || cur->type == TK_BLOCK_COMMENT)
        return 1;
    if (prev->type == TK_LINE_COMMENT || prev->type == TK_BLOCK_COMMENT)
        return 1;

    if (cur->type == TK_PUNCT) {
        /* No space ever before these. */
        if (tok_is(cur, ";") || tok_is(cur, ",") ||
            tok_is(cur, ")") || tok_is(cur, "]"))
            return 0;
        if (tok_is(cur, "("))
            return tok_is_ctrl_keyword(prev) || tok_is(prev, "return") ||
                   !is_value_like_end(prev);
        if (tok_is(cur, "["))
            return !is_value_like_end(prev);
        if (tok_is(cur, ".") || tok_is(cur, "->"))
            return 0;
        if (tok_is(cur, "++") || tok_is(cur, "--")) {
            if (is_value_like_end(prev)) return 0; /* postfix: glued, e.g. "i++" */
            if (prev->glue) return 0;
            return 1; /* prefix after an operator/keyword/comma/semicolon */
        }
        if (tok_is(cur, ":"))
            return 0; /* case/default/goto labels; ternary colon spacing is a
                          known imperfection of this heuristic (see file header) */
        if (tok_is(cur, "{"))
            return 1; /* "){" -> ") {" */
        if (tok_is(cur, "!") || tok_is(cur, "~")) {
            if (prev->glue) return 0;
            return 1;
        }
        /* '*', '&', '+', '-' deliberately fall through: whether a space
         * precedes them is decided below by prev's own glue/category,
         * exactly like any other token. */
    }

    if (prev->glue) return 0;

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

    /* Default for remaining punctuation (binary operators like =, ==, +=,
     * <<, &&, ||, ?, etc.) and anything else adjoining a value: one space. */
    return 1;
}

/* Resolve `t`'s glue flag now that we know the real previous token. Only
 * meaningful for punctuators; everything else keeps glue == 0. */
static void resolve_glue(Token *t, const Token *prev)
{
    if (t->type != TK_PUNCT) return;

    if (tok_is(t, "(") || tok_is(t, "[") || tok_is(t, ".") || tok_is(t, "->") ||
        tok_is(t, "!") || tok_is(t, "~") || tok_is(t, "++") || tok_is(t, "--")) {
        t->glue = 1;
        return;
    }
    if (tok_is(t, "*") || tok_is(t, "&")) {
        /* Declarator-star vs multiplication is genuinely ambiguous without a
         * real parser (both have a "value-like" token before them). We bias
         * toward the declarator/deref reading: glue to whatever follows
         * ("int *ptr", "&x"). This means hand-written multiplication like
         * "a*b" comes out as "a *b" rather than "a * b" - a known,
         * documented trade-off (see file header). */
        t->glue = 1;
        return;
    }
    if (tok_is(t, "+") || tok_is(t, "-")) {
        t->glue = prev_makes_unary_context(prev);
        return;
    }
    /* everything else: glue stays 0, ordinary spacing rules apply */
}

/* ------------------------------------------------------------------ */
/* Label detection (case / default / "ident:" goto-labels)             */
/* ------------------------------------------------------------------ */

static int starts_case_or_default(const Token *toks, size_t i, size_t n)
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
static int starts_goto_label(const Token *toks, size_t i, size_t n)
{
    if (toks[i].type != TK_IDENT) return 0;
    if (i + 1 >= n) return 0;
    const Token *nx = &toks[i + 1];
    if (nx->nl_before != 0) return 0; /* colon must be same line */
    if (nx->type != TK_PUNCT || !tok_is(nx, ":")) return 0;
    return 1;
}

/* ------------------------------------------------------------------ */
/* Main formatting pass                                                 */
/* ------------------------------------------------------------------ */

typedef struct {
    int indent_width;
    int use_tabs;
} Options;

static void emit_indent(Out *o, const Options *opt, int level)
{
    if (level < 0) level = 0;
    if (opt->use_tabs) {
        out_repeat(o, '\t', level);
    } else {
        out_repeat(o, ' ', level * opt->indent_width);
    }
}

static Out format_tokens(TokVec *v, const Options *opt)
{
    Out o = {0};
    int depth = 0;        /* brace nesting depth */
    int paren_depth = 0;  /* unmatched ( / [ depth, for continuation-line bump */
    const Token *prev = NULL;
    int first = 1;

    for (size_t i = 0; i < v->count; i++) {
        Token *t = &v->items[i];
        if (t->type == TK_EOF) break;

        int this_depth = depth;

        /* Closing brace dedents its own line. */
        if (t->type == TK_PUNCT && tok_is(t, "}"))
            this_depth = depth - 1;

        /* case/default/goto-labels sit one level back from the code they head. */
        int is_label_start = 0;
        if (t->nl_before > 0 || first) {
            if (starts_case_or_default(v->items, i, v->count) ||
                starts_goto_label(v->items, i, v->count)) {
                is_label_start = 1;
            }
        }
        if (is_label_start) this_depth -= 1;

        int extra = (paren_depth > 0) ? 1 : 0;
        this_depth += extra;
        if (this_depth < 0) this_depth = 0;

        if (t->nl_before > 0 && !first) {
            int blanks = t->nl_before - 1;
            if (blanks > 1) blanks = 1;
            int total_nl = 1 + blanks;
            out_repeat(&o, '\n', total_nl);
            emit_indent(&o, opt, this_depth);
        } else if (!first) {
            if (need_space(prev, t)) out_char(&o, ' ');
        } else {
            emit_indent(&o, opt, this_depth);
        }

        out_str(&o, t->text, t->len);

        resolve_glue(t, prev);

        /* Update nesting state for subsequent tokens. */
        if (t->type == TK_PUNCT) {
            if (tok_is(t, "{")) depth++;
            else if (tok_is(t, "}")) { if (depth > 0) depth--; }
            else if (tok_is(t, "(") || tok_is(t, "[")) paren_depth++;
            else if (tok_is(t, ")") || tok_is(t, "]")) { if (paren_depth > 0) paren_depth--; }
        }

        prev = t;
        first = 0;
    }

    out_char(&o, '\n');
    return o;
}

/* ------------------------------------------------------------------ */
/* CLI                                                                  */
/* ------------------------------------------------------------------ */

static void usage(const char *prog)
{
    fprintf(stderr,
        "usage: %s <input.c> [output.c]\n"
        "       %s <input.c> -i        (format in place)\n"
        "       %s -                   (read stdin, write stdout)\n"
        "options:\n"
        "  -w N   indent width in spaces (default 4)\n"
        "  -t     indent with tabs instead of spaces\n",
        prog, prog, prog);
}

int main(int argc, char **argv)
{
    Options opt = { 4, 0 };
    const char *in_path = NULL;
    const char *out_path = NULL;
    int in_place = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-w") == 0 && i + 1 < argc) {
            opt.indent_width = atoi(argv[++i]);
            if (opt.indent_width <= 0) opt.indent_width = 4;
        } else if (strcmp(argv[i], "-t") == 0) {
            opt.use_tabs = 1;
        } else if (strcmp(argv[i], "-i") == 0) {
            in_place = 1;
        } else if (!in_path) {
            in_path = argv[i];
        } else if (!out_path) {
            out_path = argv[i];
        } else {
            usage(argv[0]);
            return 1;
        }
    }

    if (!in_path) {
        usage(argv[0]);
        return 1;
    }

    FILE *fin;
    if (strcmp(in_path, "-") == 0) {
        fin = stdin;
    } else {
        fin = fopen(in_path, "rb");
        if (!fin) {
            fprintf(stderr, "format: cannot open '%s': %s\n", in_path, strerror(errno));
            return 1;
        }
    }

    size_t len;
    char *src = read_whole_file(fin, &len);
    if (fin != stdin) fclose(fin);

    TokVec toks = tokenize(src, len);
    Out result = format_tokens(&toks, &opt);

    FILE *fout;
    if (in_place && strcmp(in_path, "-") != 0) {
        fout = fopen(in_path, "wb");
        if (!fout) {
            fprintf(stderr, "format: cannot write '%s': %s\n", in_path, strerror(errno));
            return 1;
        }
    } else if (out_path) {
        fout = fopen(out_path, "wb");
        if (!fout) {
            fprintf(stderr, "format: cannot write '%s': %s\n", out_path, strerror(errno));
            return 1;
        }
    } else {
        fout = stdout;
    }

    fwrite(result.buf, 1, result.len, fout);
    if (fout != stdout) fclose(fout);

    free(src);
    free(toks.items);
    free(result.buf);
    return 0;
}