#pragma nullable enable

int * _Owner _Opt f();
void close(int * _Owner p);
int main()
{
    int * _Owner _Opt  p = f();
    close(p);
}