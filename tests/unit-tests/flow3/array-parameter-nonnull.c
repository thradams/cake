#pragma safety enable
#define NULL ((void*)0)

/*
   Array parameters decay to pointers, and flow3 treats them exactly like
   a plain (non-_Opt) pointer parameter: assumed non-null on entry (no
   warning dereferencing/indexing them), and flagged if a caller passes a
   literal null.
*/

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

/*
   Regression test for a false positive found in cake's own source
   (generate_file_scope_new_name, codegen.c):

     int f(struct hash_map* m, const char* key);
     void use(struct hash_map* m, char new_name[]) { f(m, new_name); }

   Passing an array parameter on to ANOTHER function's non-optional
   pointer parameter incorrectly warned "passing a possible null
   pointer 'new_name'". Root cause: unlike a real pointer parameter, an
   array parameter (`char new_name[]`) never took the "assume non-null"
   seeding branch in flow3_parameter_object_init at all -- that branch
   was gated on type_is_pointer(p_type), which is false for
   TYPE_CATEGORY_ARRAY. So the array's own tracked alternative fell
   through to the generic ANY seed instead of a definite NOT_EQUAL-0.
   Indexing it directly (new_name[0]) still looked safe (that check
   doesn't consult this relation), but passing the same array on to
   another function's non-optional pointer parameter read ANY as
   "could be zero" and warned. Fixed by also taking the non-null
   seeding branch for array-category parameter types.
*/
struct hash_map { int x; };
int find_entry(struct hash_map* m, const char* key);

int pass_array_to_another_function(struct hash_map* m, char new_name[])
{
    return find_entry(m, new_name); /* ok: new_name is still just as non-null as any other array parameter */
}
