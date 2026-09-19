#pragma safety enable
#define NULL ((void*)0)

/* array parameters decay to non-_Opt pointers: non-null on entry, literal null argument warns */

void f(int a[])
{
    a[0] = 1; /* ok: array parameter assumed non-null, like a plain pointer */
}

void g(int a[10])
{
    a[0] = 1; /* ok: same for a sized array parameter */
}

void h(int a[], int n)
{
    for (int i = 0; i < n; i++)
        a[i] = 0; /* ok: non-null assumed throughout */
}

void use(void)
{
    f(NULL); //lint 39 passing null as array
    g(NULL); //lint 39 passing null as array
}

/* passing an array parameter on to a non-optional pointer parameter must not warn (codegen.c generate_file_scope_new_name) */
struct hash_map { int x; };
int find_entry(struct hash_map* m, const char* key);

int pass_array_to_another_function(struct hash_map* m, char new_name[])
{
    return find_entry(m, new_name); /* ok: new_name is still just as non-null as any other array parameter */
}
