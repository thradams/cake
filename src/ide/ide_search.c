/* ide_search.c - plain-text search over files, the old IDE's fr_search_*:
 * each match is reported as "name:line:col: <its line>" with the match
 * highlighted (VT100 bold yellow), then a summary line. Find Definition's
 * fallback uses it; Find in Files will too.
 */
#include "ide_shell.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MATCH_ON "\x1b[1;33m"
#define MATCH_OFF "\x1b[0m"

void ide_text_append(struct ide_text* t, const char* data, size_t n)
{
    if (t->len + n + 1 > t->cap)
    {
        size_t cap = t->cap ? t->cap * 2 : 4096;
        while (cap < t->len + n + 1)
            cap *= 2;
        char* p = realloc(t->data, cap);
        if (!p)
            return;
        t->data = p;
        t->cap = cap;
    }
    memcpy(t->data + t->len, data, n);
    t->len += n;
    t->data[t->len] = '\0';
}

void ide_text_printf(struct ide_text* t, const char* fmt, ...)
{
    char buf[4096] = { 0 };
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(buf, sizeof buf, fmt, args);
    va_end(args);
    if (n <= 0)
        return;
    if ((size_t)n < sizeof buf)
    {
        ide_text_append(t, buf, (size_t)n);
        return;
    }
    char* big = malloc((size_t)n + 1);
    if (!big)
        return;
    va_start(args, fmt);
    vsnprintf(big, (size_t)n + 1, fmt, args);
    va_end(args);
    ide_text_append(t, big, (size_t)n);
    free(big);
}

static int is_word(unsigned char c)
{
    return isalnum(c) || c == '_';
}

static int match_at(const char* hay, int hlen, int pos, const char* needle, int nlen, int ci, int whole)
{
    if (pos < 0 || pos + nlen > hlen)
        return 0;
    for (int i = 0; i < nlen; i++)
    {
        char a = hay[pos + i], b = needle[i];
        if (ci)
        {
            a = (char)tolower((unsigned char)a);
            b = (char)tolower((unsigned char)b);
        }
        if (a != b)
            return 0;
    }
    if (whole)
    {
        if (pos > 0 && is_word((unsigned char)hay[pos - 1]))
            return 0;
        if (pos + nlen < hlen && is_word((unsigned char)hay[pos + nlen]))
            return 0;
    }
    return 1;
}

char* ide_replace_text(const char* text, const struct ide_search* s, const char* with, int* count)
{
    int len = (int)strlen(text);
    int nlen = (int)strlen(s->pattern);
    int rlen = (int)strlen(with);
    *count = 0;
    if (nlen == 0)
        return NULL;
    for (int p = 0; p + nlen <= len;)
    {
        if (match_at(text, len, p, s->pattern, nlen, !s->match_case, s->whole_word))
        {
            (*count)++;
            p += nlen;
        }
        else
        {
            p++;
        }
    }
    if (*count == 0)
        return NULL;
    char* out = malloc((size_t)(len + *count * (rlen - nlen)) + 1);
    if (!out)
    {
        *count = 0;
        return NULL;
    }
    int w = 0;
    for (int p = 0; p < len;)
    {
        if (p + nlen <= len && match_at(text, len, p, s->pattern, nlen, !s->match_case, s->whole_word))
        {
            memcpy(out + w, with, (size_t)rlen);
            w += rlen;
            p += nlen;
        }
        else
        {
            out[w++] = text[p++];
        }
    }
    out[w] = '\0';
    return out;
}

int ide_search_text(const char* name, const char* text, const struct ide_search* s, struct ide_text* out)
{
    int len = (int)strlen(text);
    int nlen = (int)strlen(s->pattern);
    if (nlen == 0)
        return 0;
    int line = 1, col = 1, line_start = 0, count = 0;
    for (int p = 0; p + nlen <= len;)
    {
        if (match_at(text, len, p, s->pattern, nlen, !s->match_case, s->whole_word))
        {
            count++;
            int line_end = p + nlen;
            while (line_end < len && text[line_end] != '\n' && text[line_end] != '\r')
                line_end++;
            ide_text_printf(out, "%s:%d:%d: %.*s" MATCH_ON "%.*s" MATCH_OFF "%.*s\n",
                             name, line, col, p - line_start, text + line_start, nlen, text + p,
                             line_end - (p + nlen), text + p + nlen);
            for (int i = 0; i < nlen; i++, p++)
            {
                if (text[p] == '\n')
                {
                    line++;
                    col = 1;
                    line_start = p + 1;
                }
                else
                {
                    col++;
                }
            }
        }
        else
        {
            if (text[p] == '\n')
            {
                line++;
                col = 1;
                line_start = p + 1;
            }
            else
            {
                col++;
            }
            p++;
        }
    }
    return count;
}
