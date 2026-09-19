#pragma safety enable

/* _Out works the same for an array parameter and a pointer parameter */

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
