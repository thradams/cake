#ifndef IDE_FORMAT_H
#define IDE_FORMAT_H

#include <stddef.h>

typedef struct {
    int indent_width;
    int use_tabs;
    int max_blank_lines; /* cap on consecutive blank lines to preserve (0 = default of 1) */
} Options;

char *format_c_source(const char *src, size_t len, const Options *opt, size_t *out_len);

#endif /* IDE_FORMAT_H */
