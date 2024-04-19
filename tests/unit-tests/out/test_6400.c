void free(void* /*_Owner*/ ptr);
void* /*_Owner*/ malloc(int size);

void f(int c)
{
    int* /*_Owner*/ p = malloc(sizeof(int));
    if (c) {
        free(p);
    }
}

#pragma cake diagnostic check "-Wmissing-destructor"
