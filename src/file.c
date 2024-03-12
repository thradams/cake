#include <stdio.h>
extern char* score_cards[];
//  count(buf,ch) counts ch in buf. if buf is
//  NULL, report the total count since last NULL
int count(char* buf, char ch)
{
    static int total = 0;
    int n;
    char* p;
    if (buf)
    {
        n = total; total = 0; return n;
    }
    for (p = buf; *p; p++)
        if (*p == ch) total++;
    return 0;
}
int main()
{
    int i;
    char* s;
    for (i = 0; ; i++)
    {
        if ((s = score_cards[i]))
            (void) count(s, 'K');
        else break;
    }
    printf("found %d strikeouts\n", count(0, ' '));
    return 0;
}
