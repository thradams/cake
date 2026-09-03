/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
 *
 *  A minimal subset of the Language Server Protocol's own data shapes (see
 *  https://microsoft.github.io/language-server-protocol/specification),
 *  modeled as plain C structs - no JSON, no wire format.
 *
 *  cakeide talks to cake in-process today (see ide_lsp.h), but framing that
 *  boundary in real LSP vocabulary buys two things for later, for free:
 *    - the IDE side could point at a different, out-of-process LSP server
 *      instead, without changing the shape of what it sends/receives;
 *    - cake's own handler (lsp_text_document_definition() in ide_lsp.h) could be
 *      wrapped in a real JSON-RPC transport to become an actual LSP server,
 *      without changing its signature either.
 *
 *  Covers textDocument/definition, textDocument/formatting, and
 *  textDocument/rangeFormatting so far - extend the same way as more LSP
 *  methods get wired up.
 */

#pragma once
#include "ownership.h"
#include <stdbool.h>

/* Position: 0-based, per the spec (`character` is a UTF-16 code unit offset;
   cakeide currently treats it as a byte offset instead - fine for ASCII,
   wrong for multi-byte UTF-8, same simplification the rest of the editor
   already makes). cake's own struct token::line/col (tokenizer.h) are
   1-based - convert at the boundary, not by carrying two different
   conventions through the same struct. */
struct lsp_position
{
    int line;
    int character;
};

struct lsp_range
{
    struct lsp_position start;
    struct lsp_position end;
};

/* DocumentUri: a real "file:///..." URI string - see lsp_path_to_uri()/
   lsp_uri_to_path() below for converting to/from a plain filesystem path at
   the boundary (an in-process caller has paths; the wire format has URIs). */
struct lsp_text_document_identifier
{
    char* _Owner _Opt uri;
};

struct lsp_location
{
    char* _Owner _Opt uri;
    struct lsp_range range;
};

struct lsp_text_document_position_params
{
    struct lsp_text_document_identifier text_document;
    struct lsp_position position;
};

/* FormattingOptions per the spec (tabSize/insertSpaces) - unused by cake's
   own formatter today, which picks spacing/brace style by name (-style=,
   see options.c) rather than by tab width, so these two are accepted but
   ignored for now.

   `style` is not part of the real spec - FormattingOptions is explicitly
   documented as extensible with implementation-specific properties beyond
   tabSize/insertSpaces, and this is cake's one: the -style=<name> to format
   with (NULL for cake_format()'s own default). */
struct lsp_formatting_options
{
    int tab_size;
    bool insert_spaces;

    const char* _Opt style;  /* cake-specific extension - see above */
};

struct lsp_document_formatting_params
{
    struct lsp_text_document_identifier text_document;
    struct lsp_formatting_options options;
};

struct lsp_document_range_formatting_params
{
    struct lsp_text_document_identifier text_document;
    struct lsp_range range;
    struct lsp_formatting_options options;
};

struct lsp_text_edit
{
    struct lsp_range range;
    char* _Owner _Opt new_text;
};

/* A TextEdit[], per the spec - cake's formatter always returns one edit
   spanning the whole document (see ide_lsp.c's own doc comments on the two
   formatting handlers for why), so in practice count is always 0 or 1; the
   list shape is kept anyway so a future minimal-diff implementation can
   emit more without changing the return type. */
struct lsp_text_edit_list
{
    struct lsp_text_edit* _Owner _Opt items;
    int count;
};

void lsp_text_edit_list_destroy(struct lsp_text_edit_list* list);

/* Converts a plain filesystem path to a "file://" URI. Percent-encoding is
   not implemented - fine for the paths cakeide deals with, not a general
   URI encoder. Returns an owned string the caller must free(), or NULL for
   a NULL/"" path (e.g. an untitled buffer) or on allocation failure. */
char* _Owner _Opt lsp_path_to_uri(const char* _Opt path);

/* The inverse of lsp_path_to_uri(): strips the "file://" prefix, returning
   an owned plain path the caller must free(). Returns NULL for a NULL/""
   uri, or one that isn't the "file://" scheme. */
char* _Owner _Opt lsp_uri_to_path(const char* _Opt uri);
