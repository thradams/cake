//en.cppreference.com/w/c/string/wide/wmemcmp.html
#include <locale.h>
#include <stdio.h>
#include <wchar.h>
 
void demo(const wchar_t* lhs, const wchar_t* rhs, size_t sz)
{
    for (size_t n = 0; n < sz; ++n)
        putwchar(lhs[n]);
 
    int rc = wmemcmp(lhs, rhs, sz);
    if (rc == 0)
        wprintf(L" compares equal to ");
    else if(rc < 0)
        wprintf(L" precedes ");
    else if(rc > 0)
        wprintf(L" follows ");
 
    for (size_t n = 0; n < sz; ++n)
        putwchar(rhs[n]);
    wprintf(L" in lexicographical order\n");
}
 
int main(void)
{
    setlocale(LC_ALL, "en_US.utf8");
 
    wchar_t a1[] = {L'α',L'β',L'γ'};
    wchar_t a2[] = {L'α',L'β',L'δ'};
 
    size_t sz = sizeof a1 / sizeof *a1;
    demo(a1, a2, sz);
    demo(a2, a1, sz);
    demo(a1, a1, sz);
}