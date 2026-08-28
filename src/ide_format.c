#pragma safety enable

/* format_c_source() forwards to cake_format() (compile.c), which runs
 * cake's real tokenizer/parser and lets the -style spacing/brace checks
 * (parser.c) fix the token stream instead of only diagnosing it. See that
 * file for what actually moves.
 */

#include "ownership.h"
#include <stdlib.h>
#include <string.h>

#include "ide_format.h"
#include "compile.h"
#include "parser.h" /*struct report*/

char* format_c_source(const char* options, const char* path, const char* src, size_t len, size_t* out_len)
{
    char* _Owner _Opt content = malloc(len + 1);
    if (content == NULL)
    {
        return NULL;
    }
    memcpy(content, src, len);
    content[len] = '\0';

    struct report report = { 0 };
    char* _Owner _Opt out = (char* _Owner _Opt) cake_format(options, path, content, &report);
    free(content);

    if (out != NULL && out_len != NULL)
    {
        *out_len = strlen(out);
    }
    return out;
}
