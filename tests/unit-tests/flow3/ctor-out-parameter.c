#pragma safety enable

/* passing an uninitialized object to a _Out parameter must not warn, including an _Out array parameter (no per-element warning) */

/* _Out array out-parameter: the callee fills it. */
void fill_array(_Out char buf[8]);

/* _Out pointer out-parameter (already worked; kept for contrast). */
void fill_ptr(_Out char* p);

/* non-_Out array parameter: the callee reads it, uninitialized content still warns */
void reads_array(char buf[8]);

void ctor_array_ok(void)
{
    char b[8];
    fill_array(b);           /* clean: b is constructed by fill_array */
}

void ctor_ptr_ok(void)
{
    char b[8];
    fill_ptr(b);             /* clean */
}

/* documented: passing an uninitialized array to reads_array warns per element */
