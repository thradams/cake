#pragma safety enable


_Uninitialized void* _Owner malloc(int i);
void free(void* _Owner p);

struct X {
    char* _Nonnull _Owner name;
};

int main()
{
    struct X* _Owner p = malloc(sizeof * p);

    //object 'p' may not be empty

    p = 0; //lint 26 assignment discards _Owner 'p' without releasing it first (see line 13)
}
