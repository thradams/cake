#pragma safety enable

/* a fact supported by many branches is reported once, with the count ("(+4 branches)"), and explained from a few of them only (FLOW_FINDING_MAX_SAMPLES) */

void use(int* p);
int g;

void null_on_five_branches(int a, int b, int c, int d, int e)
{
    int* _Opt p = &g;
    if (a) p = 0;
    else if (b) p = 0;
    else if (c) p = 0;
    else if (d) p = 0;
    else if (e) p = 0;
    use(p); //lint 35
}

void null_on_one_branch(int a)
{
    int* _Opt p = &g;
    if (a) p = 0;
    use(p); //lint 35
}
