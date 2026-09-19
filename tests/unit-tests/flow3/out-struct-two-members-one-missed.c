#pragma safety enable

struct P { int x; int y; };

void init(_Out struct P* p)
{
    p->x = 1;
} //lint 71 'y' never set

int main()
{
    struct P p;
    init(&p);
    return p.x + p.y;
}
