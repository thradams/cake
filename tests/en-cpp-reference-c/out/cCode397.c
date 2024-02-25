//en.cppreference.com/w/c/string/wide/wcsncmp.html
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
 
void demo(const wchar_t *lhs, const wchar_t *rhs, int sz)
{
    int rc = wcsncmp(lhs, rhs, sz);
    if(rc == 0)
        printf("First %d characters of [%ls] equal [%ls]\n", sz, lhs, rhs);
    else if(rc < 0)
        printf("First %d characters of [%ls] precede [%ls]\n", sz, lhs, rhs);
    else if(rc > 0)
        printf("First %d characters of [%ls] follow [%ls]\n", sz, lhs, rhs);
}
 
int main(void)
{
    const wchar_t *str1 = L"안녕하세요";
    const wchar_t *str2 = L"안녕히 가십시오";
 
    setlocale(LC_ALL, "en_US.utf8");
    demo(str1, str2, 5);
    demo(str2, str1, 8);
    demo(str1, str2, 2);
}