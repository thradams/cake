#pragma nullable enable

struct X { char* name; };

void receives_non_null(struct X* p) {};

int main()
{
    struct X x = {};
    receives_non_null(&x); //lint 35 passing a possible null pointer '(*&x).name' to non-nullable pointer parameter (see line
}
