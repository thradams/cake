/* Cake x86_x64_gcc */
struct data {
    int nr;
    char * value;
};


struct data dat[4] = {1,"Foo",2,"Bar",3,"Hello",4,"World"};
int data_cmp(void * lhs, void * rhs)
{
    struct data * l;
    struct data * r;

    l = lhs;
    r = rhs;
    if (l->nr < r->nr)
    {
        return -1;
    }
    else
    {
        if (l->nr > r->nr)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}


extern void *bsearch(void * __key, void * __base, unsigned int long __nmemb, unsigned int long __size, int (* __compar)(void *, void *));
extern int printf(char * __format, ...);

int main(void)
{
    struct data  key;
    struct data * res;

    key.nr = 3;
    key.value = 0;
    res = bsearch(&key, dat, 4L, 16L, data_cmp);
    if (res)
    {
        printf("No %d: %s\n", res->nr, res->value);
    }
    else
    {
        printf("No %d not found\n", key.nr);
    }
}


