#pragma safety enable

_Clear void* _Owner _Opt calloc(unsigned long n, unsigned long size);
void free(void* _Owner _Opt ptr);


int main()
{
    int* _Owner _Opt p = calloc(1, sizeof(int));
    int* _Owner _Opt p2 = p;    //MOVED
    free(p2);

    //object may be already moved
    /* FIXED: no extra "(*p) lifetime has ended", free takes void*; only "p is moved" is real */
    free(p); //lint 32 object 'p' is moved (see line 9)

}
