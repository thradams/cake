#pragma safety enable

/* flow analysis stops with warning 97 when a function tracks too many objects,
   reports it once, and reports nothing after it */

struct row
{
    char text[128];
};

struct big
{
    struct row rows[128]; /* 128 * 128 = 16384 objects, over the limit of 10000 */
};

static struct big g_big;

void fill(struct big* p);

void stops_once(int n)
{
    if (n > 0)
    {
        {
            fill(&g_big); //lint 97
        }
        g_big.rows[0].text[0] = 'a';
    }

    int x;
    int y = x; /* not reported: analysis stopped above */
    (void)y;
}

/* the next function is analysed normally */
void analysed_again(void)
{
    int x;
    int y = x; //lint 30
    (void)y;
}
