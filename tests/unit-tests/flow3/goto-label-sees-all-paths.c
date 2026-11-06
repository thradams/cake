/* At a label every arriving path counts: a value from the path that did not take the goto is not on another side of it (A14.3, A14.4) */
#pragma safety enable

void u(int i);
int* find(int);
char* _Owner make(void);
void use(char* p);
void free(void* _Owner _Nullable p);
int X;

void cleanup_ok(void)
{
    char* _Owner p = make();
    if (!p) goto end;
    use(p);
end:
    if (p) free(p);
}

void cleanup_leak(void)
{
    char* _Owner p = make();
    if (!p) goto end;
    use(p);
end:
    ;
} //lint 29

void value_not_only_from_goto(void)
{
    char* _Owner p = make();
    if (!p) goto end;
end:
    compile_assert(p == 0); //lint 67
    free(p);
}

void null_from_else(int c)
{
    int* p = &X;
    if (c) goto L;
    else p = 0;
L:
    *p = 1; //lint 33
}

void null_from_narrowed_else(void)
{
    int* p = find(1);
    if (p) goto L;
    else u(1);
L:
    *p = 1; //lint 33
}

void double_free(int c)
{
    char* _Owner p = make();
    if (c) goto L;
    else { free(p); }
L:
    free(p); //lint 32
}
