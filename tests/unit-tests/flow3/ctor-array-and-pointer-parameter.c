#pragma safety enable

/*
   Regression test confirming _Out works identically whether the
   parameter is declared as an array (which decays to a pointer per C's
   parameter-adjustment rule) or as a plain pointer -- no warning should
   be emitted for either call, and the array is genuinely treated as
   initialized afterward.
*/

void parse1(_Out char a[]);
void parse2(_Out char* a);

char file_scope_buf[200];

void use_file_scope(void)
{
    parse1(file_scope_buf); /* ok */
    
    parse2(file_scope_buf); /* ok */
}

void use_local(void)
{
    char buf[200];
    parse1(buf); /* ok */
    parse2(buf); /* ok */
    buf[0] = 1;  /* ok: _Out initialized it */
}
