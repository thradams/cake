/* Cake 0.12.26 x86_msvc */
struct X {
    char * name;
};


void *malloc(int i);
void free(void * p);

int main()
{
    struct X * p;

    p = malloc(4);
    free(p);
}


