#ifndef IDE_FORMAT_H
#define IDE_FORMAT_H

#include <stddef.h>

typedef struct {
    /* Columns per indent level, or 0 for "auto": measure what the source
     * already does and keep doing that (see format_detect_indent). */
    int indent_width;
    int use_tabs; /* indent with tabs instead of spaces */
} Options;

/* Re-indents C source and changes nothing else: every line keeps its
 * content byte for byte from its first non-blank character onward, and only
 * the leading whitespace is recomputed from '{' / '}' nesting. Line breaks,
 * blank lines, brace placement and spacing around operators are all left as
 * the author wrote them.
 *
 * Returns a NUL-terminated buffer the caller must free(); *out_len (when
 * not NULL) receives its length, not counting the NUL. */
char* format_c_source(const char* src, size_t len, const Options* opt, size_t* out_len);

/* Measures the indentation the source already uses and writes it to *out.
 *
 * Returns 1 when the source gave enough evidence to call it, and 0 when it
 * did not - too few indented lines, or no consistent step between them. On
 * 0, *out is left untouched, so seed it with the fallback you want before
 * calling. format_c_source() runs this itself when opt->indent_width is 0.
 *
 * Both fields are set: use_tabs when the file indents with tabs, and
 * indent_width to the step it indents by. */
int format_detect_indent(const char* src, size_t len, Options* out);

#endif /* IDE_FORMAT_H */
