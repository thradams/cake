#pragma safety enable

/* issue 491: an _Owner result of ?: moves the owner of the arm that chose it */

void free(void* _Owner _Opt p);
char* _Owner _Opt strdup(const char* s);

void double_free(void)
{
    char* _Owner _Opt p = strdup("a");
    char* _Owner _Opt q = p ? p : 0;
    free(q);
    free(p); //lint 32
}

void overwrite_owner(int c)
{
    char* _Owner _Opt p = strdup("a");
    char* _Owner _Opt p1 = strdup("b");
    char* _Owner _Opt p2 = strdup("c");
    p = c ? p2 : p1; //lint 26
    free(p);
} //lint 29 29

void argument(void)
{
    char* _Owner _Opt p = strdup("a");
    free(p ? p : 0);
    free(p); //lint 32
}

void elvis(void)
{
    char* _Owner _Opt p = strdup("a");
    char* _Owner _Opt q = p ?: 0;
    free(q);
    free(p); //lint 32
}
