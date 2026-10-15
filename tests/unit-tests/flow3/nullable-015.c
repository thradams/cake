
#pragma safety enable
#pragma flow enable

struct X { int i; int j; };

void clear(_Clear struct X* p) {} //lint 69 69

int main()
{
    struct X x = {1, 2};
    clear(&x);
}

