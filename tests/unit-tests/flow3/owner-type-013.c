#pragma safety  enable

void consume(int* _Owner p);

void f(int* _Owner p2)
{
    int* _Owner p1 = p2;
    consume(p1);
}
