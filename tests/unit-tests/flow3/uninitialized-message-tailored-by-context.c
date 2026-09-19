#pragma safety enable

/* warning 30 says assigning/passing/returning by context and renders the source expression (`*p`) when there is no member name */

void* _Owner _Opt _Uninitialized malloc(unsigned long size);
void free(void* _Owner _Opt ptr);
void take(int i);

/* Plain assignment: must say "assigning" and show '*p'. */
int assign_from_uninitialized_pointee(void)
{
    int * _Owner p = malloc(sizeof(int)); //lint 35
    int j;
    j = *p; //lint 33 30
    free(p);
    return j; //lint 30
}

/* Function argument: must say "passing" and show '*p'. */
void pass_uninitialized_pointee(void)
{
    int * _Owner p = malloc(sizeof(int)); //lint 35
    take(*p); //lint 33 30
    free(p);
}

/* return statement: must say "returning" and show '*p'; p is deliberately leaked here to isolate the return-site diagnostic */
int return_uninitialized_pointee(void)
{
    int * _Owner p = malloc(sizeof(int)); //lint 35
    return *p; //lint 33 30 29
}
