#ifndef IDE_FORMAT_H
#define IDE_FORMAT_H

#include <stddef.h>

/* Reformats C source with cake's own tokenizer/parser and -style checks
 * (see cake_format() in compile.c): spacing and brace placement are fixed
 * up, everything else is left as the author wrote it.
 *
 * `options` is a cake command-line options string, e.g. "-format -style=cake"
 * - format_c_source() does not add -format itself, so the caller decides the
 * style. To restrict the fix to a line range (e.g. an editor's current
 * selection), add -format-lines=first:last - same flag the CLI takes.
 *
 * `path` is the real path `src` was loaded from (NULL/"" if there is none,
 * e.g. an unsaved buffer) - needed to resolve #include "quoted.h" headers
 * relative to the right directory; <system.h> headers don't need it.
 *
 * Returns a NUL-terminated buffer the caller must free(), or NULL on error;
 * *out_len (when not NULL) receives its length, not counting the NUL. */
char* format_c_source(const char* options, const char* path, const char* src, size_t len, size_t* out_len);

#endif /* IDE_FORMAT_H */
