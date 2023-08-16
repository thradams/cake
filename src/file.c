
#pragma nullchecks

void * owner malloc(int sz);
void free(void * owner opt p);

void f1(int *p){}
void f2(int * opt p){}

int main(int i) {   
    int * owner p = malloc(1);
    if (p)
    {
      f1(p);
    }
    f1(p);
    f2(p);
    free(p);
}



