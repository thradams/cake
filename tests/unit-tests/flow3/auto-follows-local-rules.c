#pragma safety enable

void* _Owner malloc(unsigned long size);
void free(void* _Owner _Nullable p);
int* find(void);

/* same as 'void* p = malloc(1);' */
void owner_from_call(void)
{
    auto p = malloc(1);
    free(p);
}

void owner_from_call_not_released(void)
{
    auto p = malloc(1);
} //lint 29

/* same as 'char* b = a;' */
void view_from_owner(char* _Owner a)
{
    auto b = a;
    if (b) { char c = b[0]; }
    free(a);
}

void view_cannot_release(char* _Owner a)
{
    auto b = a;
    free(b); //lint 78
    free(a);
}

/* a local is nullable: flow follows the value */
void nullable_local(void)
{
    auto q = find();
    if (q) { *q = 1; }
    q = 0;
}

void nullable_local_read(void)
{
    auto q = find();
    *q = 1; //lint 33
}

void not_a_pointer(void)
{
    auto x = 1;
    x = 2;
}

/* not a local: a pointer object is not made nullable */
int global;
static auto file_scope = &global;

void read_file_scope(void)
{
    *file_scope = 1;
}
