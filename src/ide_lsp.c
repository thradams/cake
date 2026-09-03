/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
 */

#include "ownership.h"
#include "ide_lsp.h"
#include "compile.h" /* cake_format() */
#include "parser.h"  /* struct report */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define FILE_URI_SCHEME "file://"

char* _Owner _Opt lsp_path_to_uri(const char* _Opt path)
{
    if (path == NULL || path[0] == '\0')
        return NULL;

    size_t n = strlen(FILE_URI_SCHEME) + strlen(path) + 1;
    char* _Owner _Opt uri = malloc(n);
    if (uri == NULL)
        return NULL;

    snprintf(uri, n, "%s%s", FILE_URI_SCHEME, path);
    return uri;
}

char* _Owner _Opt lsp_uri_to_path(const char* _Opt uri)
{
    if (uri == NULL || uri[0] == '\0')
        return NULL;

    size_t scheme_len = strlen(FILE_URI_SCHEME);
    if (strncmp(uri, FILE_URI_SCHEME, scheme_len) != 0)
        return NULL;

    return strdup(uri + scheme_len);
}

bool lsp_text_document_definition(const char* src, const struct lsp_text_document_position_params* params,
    struct lsp_location* out)
{
    (void)src;
    (void)params;
    (void)out;

    /* TODO: not implemented yet - see this function's doc comment in ide_lsp.h. */
    return false;
}

void lsp_text_edit_list_destroy(struct lsp_text_edit_list* list)
{
    if (list->items)
    {
        for (int i = 0; i < list->count; i++)
            free(list->items[i].new_text);
        free(list->items);
    }
    list->items = NULL;
    list->count = 0;
}

/* 0-based end position of `text` - the last line's index and its length,
 * i.e. where the caret would sit right after the very last character. Used
 * to build a TextEdit range spanning an entire document (see the two
 * formatting handlers below): (0,0) to this is "the whole thing". */
static struct lsp_position lsp_end_of_text(const char* text)
{
    struct lsp_position pos = { 0, 0 };
    for (const char* p = text; *p; p++)
    {
        if (*p == '\n')
        {
            pos.line++;
            pos.character = 0;
        }
        else
        {
            pos.character++;
        }
    }
    return pos;
}

/* Wraps `new_text` (already-owned, e.g. straight from format_c_source()) as
 * a one-item TextEdit list spanning the whole of `original` - the shape both
 * formatting handlers below return. Takes ownership of `new_text` either
 * way: frees it on the allocation-failure path, hands it to *out on the
 * success path. */
static bool lsp_text_edit_list_set_whole_document(struct lsp_text_edit_list* out,
    const char* original, char* _Owner new_text)
{
    struct lsp_text_edit* _Owner _Opt items = malloc(sizeof *items);
    if (items == NULL)
    {
        free(new_text);
        return false;
    }

    items[0].range.start.line = 0;
    items[0].range.start.character = 0;
    items[0].range.end = lsp_end_of_text(original);
    items[0].new_text = new_text;  /* MOVED */

    out->items = items;
    out->count = 1;
    return true;
}

/* Shared by lsp_text_document_formatting()/lsp_text_document_range_formatting()
 * below - builds the same options string do_edit_format() (ide.c) used to
 * before this file existed, then runs cake_format() (compile.c) over the
 * whole buffer. `range_lines_arg` is "" for whole-document formatting, or a
 * pre-built " -format-lines=first:last" for the range variant. */
static char* _Owner _Opt lsp_run_cake_format(const char* _Opt style, const char* range_lines_arg,
    const char* _Opt uri, const char* src, size_t len)
{
    char options[96] = "-format";
    if (style && style[0])
    {
        char stylearg[32];
        snprintf(stylearg, sizeof stylearg, " -style=%s", style);
        strncat(options, stylearg, sizeof(options) - strlen(options) - 1);
    }
    strncat(options, range_lines_arg, sizeof(options) - strlen(options) - 1);

    char* _Owner _Opt path = lsp_uri_to_path(uri);

    char* _Owner _Opt content = malloc(len + 1);
    if (content == NULL)
    {
        free(path);
        return NULL;
    }
    memcpy(content, src, len);
    content[len] = '\0';

    struct report report = { 0 };
    char* _Owner _Opt out = (char* _Owner _Opt) cake_format(options, path, content, &report);

    free(path);
    free(content);
    return out;
}

bool lsp_text_document_formatting(const char* src, const struct lsp_document_formatting_params* params,
    struct lsp_text_edit_list* out)
{
    char* _Owner _Opt formatted = lsp_run_cake_format(params->options.style, "",
        params->text_document.uri, src, strlen(src));
    if (formatted == NULL)
        return false;

    return lsp_text_edit_list_set_whole_document(out, src, formatted);
}

bool lsp_text_document_range_formatting(const char* src, const struct lsp_document_range_formatting_params* params,
    struct lsp_text_edit_list* out)
{
    char range_lines_arg[32];
    /* -format-lines wants 1-based lines (see options.c); params->range is
       0-based, per LSP's own Position. */
    snprintf(range_lines_arg, sizeof range_lines_arg, " -format-lines=%d:%d",
        params->range.start.line + 1, params->range.end.line + 1);

    char* _Owner _Opt formatted = lsp_run_cake_format(params->options.style, range_lines_arg,
        params->text_document.uri, src, strlen(src));
    if (formatted == NULL)
        return false;

    return lsp_text_edit_list_set_whole_document(out, src, formatted);
}
