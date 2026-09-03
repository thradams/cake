/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
 *
 *  cake's own (in-process, for now) implementations of LSP methods - see
 *  lsp_types.h's doc comment for why these are shaped like the real
 *  protocol's methods instead of one-off function signatures.
 */

#pragma once
#include "lsp_types.h"
#include <stdbool.h>

/*
   textDocument/definition.

   `src` is the full text of the document named by params->text_document.uri.
   A real out-of-process LSP server would look the document up by uri in its
   own store (kept in sync via textDocument/didOpen and didChange); in-process
   there is no such store yet, so the caller (which already has the buffer
   open) just hands it over directly.

   Returns false when nothing resolvable sits at params->position - which,
   right now, is every call: see the .c file. On true, *out is filled in and
   its uri is an owned string the caller must free().

   TODO: not implemented yet - always returns false. This is the one place
   cake's real tokenizer/parser needs to be wired in (an earlier prototype -
   compile_find()/find_visit.c - did exactly this; see git history); nothing
   above this layer (do_goto_definition() in ide.c) should need to change
   when that lands.
*/
bool lsp_text_document_definition(const char* src, const struct lsp_text_document_position_params* params,
    struct lsp_location* out);

/*
   textDocument/formatting - reformats the whole document. Unlike
   lsp_text_document_definition() above, this one is a real, working
   implementation: it forwards to cake_format() (compile.c) - see this
   file's own lsp_run_cake_format() - same as Edit > "Format C Source"
   already did via ide_format.c's format_c_source() before this file
   replaced it.

   Returns false (matching cake_format()'s own NULL-on-error contract) on
   failure; `*out` is untouched in that case. On true, `*out` holds
   exactly one TextEdit spanning the entire document, whose new_text is the
   complete reformatted buffer - the caller must lsp_text_edit_list_destroy()
   it. `params->options.style` is cake's own -style=<name> extension (see
   lsp_types.h); NULL uses cake_format()'s own default.
*/
bool lsp_text_document_formatting(const char* src, const struct lsp_document_formatting_params* params,
    struct lsp_text_edit_list* out);

/*
   textDocument/rangeFormatting - same as lsp_text_document_formatting()
   above, restricted to params->range's lines (cake's -format-lines=first:last,
   see options.c) - but still returns one edit spanning the *whole* document,
   not just that range: cake_format() always hands back the complete buffer,
   even when -format-lines restricted which lines it actually touched, so
   there is no minimal fragment to slice out here. A real minimal-diff
   implementation would need to diff the old and new text and emit only the
   lines that changed instead.
*/
bool lsp_text_document_range_formatting(const char* src, const struct lsp_document_range_formatting_params* params,
    struct lsp_text_edit_list* out);
