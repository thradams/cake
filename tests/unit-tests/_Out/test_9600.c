#pragma safety enable

    
void * /*_Owner*/ /*_Opt*/ calloc(int i, int n);
void free(void * /*_Owner*/ /*_Opt*/ p);

int main() {
    int * /*_Owner*/ /*_Opt*/ p1 = 0;
    int * /*_Owner*/ /*_Opt*/ p2 = calloc(1, sizeof(int));

    if (p2 == 0) {
        return 1;
    }
    else
    {
      p1 = p2;
    }
    
    free(p1);
    return 0;
}