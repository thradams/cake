#pragma nullable enable

struct X { char* name; };

void receives_non_null(struct X* p) {};

int main()
{
    struct X x;
    receives_non_null(&x); //lint 30 passing a possible uninitialized object '(*&x).name' (see line 9)
}
