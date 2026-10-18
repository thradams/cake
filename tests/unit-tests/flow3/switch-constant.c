/* switch (5): only the matching case (or default) reaches the end */

void f(const char* s);

void no_matching_case_goes_to_default(void)
{
    char* p;
    switch (5) //lint 44
    {
    case 6: break;
    default: p = ""; break;
    }
    f(p);
}

void matching_case(void)
{
    char* p;
    switch (6) //lint 44
    {
    case 6: p = ""; break;
    default: break;
    }
    f(p);
}

void fall_through_from_live_case(void)
{
    char* p;
    switch (1) //lint 44
    {
    case 1:
    case 2: p = ""; break;
    }
    f(p);
}

void no_case_matches_skips_body(void)
{
    char* p;
    switch (5) //lint 44
    {
    case 6: p = ""; break;
    }
    f(p); //lint 30
}
