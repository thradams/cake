
#pragma safety enable

void* _Owner _Opt calloc(int n, unsigned long size);
void free(void* _Owner _Opt ptr);

struct Y {
    int i;
};
struct X {
    struct Y* _Owner _Opt pY;
};

void f(struct Y* p);
int main()
{

    struct X* _Owner _Opt p = calloc(1, sizeof * p);
    if (p)
    {
        p->pY = calloc(1, sizeof(struct Y));
        if (p->pY)
        {
            f(p->pY);
            p->pY->i = 1;
//          ^^^^^ still not null
        }
        free(p->pY);
        free(p);
    }
}

