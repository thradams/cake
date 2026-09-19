#pragma safety enable


void free(void* _Owner _Opt p);
char* _Owner _Opt strdup(const char* s);

struct X
{
    const char* _Owner _Opt text;
    int i;
};

int hard_clearing(_Out _Clear struct X* p)
{
    p->i = 0;
    p->text = 0;
}

void reported_case(void)
{
    struct X x = { 0 };
    x.text = strdup("a");
    hard_clearing(&x); //lint 26 assignment discards _Owner '&x.text' without releasing it first (see line 27)
}

void clear_only_qualifier(_Clear struct X* p);

void plain_clear_is_fine(void)
{
    /* plain _Clear may free owned members itself before zeroing, so the call site is not flagged; only _Out _Clear is */
    struct X x = { 0 };
    x.text = strdup("a");
    clear_only_qualifier(&x);
}

void freed_first_is_fine(void)
{
    struct X x = { 0 };
    x.text = strdup("a");
    free((void* _Owner _Opt)x.text);
    x.text = 0;
    hard_clearing(&x);
}

void never_assigned_is_fine(void)
{
    struct X x = { 0 };
    hard_clearing(&x);
}
