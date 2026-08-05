#pragma safety enable

void* _Owner _Opt _Clear calloc(unsigned long n, unsigned long size);
void free(void* _Owner _Opt ptr);


int main()
{
    int* _Owner _Opt p = calloc(1, sizeof(int));
    int* _Owner _Opt p2 = p;    //MOVED
    free(p2);

    //object may be already moved
    /* FIXED: used to ALSO warn "object '(*p)' lifetime has ended" here
       -- free's parameter is void*, so it never accesses *p's
       structure; only the "p is moved" warning below is real. */
    free(p); //lint 32 object 'p' is moved (see line 9)

}
