struct X {
    char * p;
};


struct X f();
void free(void * p);

int main()
{
    struct X  x;

    x = 1 ? f() : f();
    free(x.p);
}

