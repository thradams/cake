#pragma safety enable

/*
   A _Ctor parameter is a constructor OUT-parameter: the callee initializes it,
   so passing an uninitialized object to it is correct and must NOT warn.

   flow3 checks this via the parameter's _Ctor qualifier. For a pointer-to-_Ctor
   the qualifier is visible on the pointer, but for an ARRAY out-parameter
   (`_Ctor char buf[100]`) the _Ctor lives on the array, not on each element --
   so the per-element uninitialized check used to miss it and reported one
   "possible uninitialized" per element (e.g. 100 warnings for buf[0..99]).
   Now a directly-_Ctor destination is recognized and the whole subtree is
   skipped.
*/

/* _Ctor array out-parameter: the callee fills it. */
void fill_array(_Ctor char buf[8]);

/* _Ctor pointer out-parameter (already worked; kept for contrast). */
void fill_ptr(_Ctor char* p);

/* Non-_Ctor array parameter: the callee reads it, so passing uninitialized
   content is a real hazard and still warns. */
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

/*
   Still warns (real uninitialized read) -- shown in a comment so this file
   stays clean:

       void reads_uninit(void) {
           char b[8];
           reads_array(b);   // warns: passing possible uninitialized b[0..7]
       }
*/
