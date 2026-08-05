
#pragma nullable enable

struct X { int i; int j; };

void clear(_Clear struct X* p) {};

int main()
{
    struct X x = {1, 2};
    clear(&x);
    // static_debug(x); //should print  0
}

