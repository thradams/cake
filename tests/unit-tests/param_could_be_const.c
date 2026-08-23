/*
   W_PARAM_COULD_BE_CONST (82): a pointer parameter whose pointed object is
   never written through and never escapes somewhere that could write it.

   Enabled explicitly so this file keeps testing the warning regardless of
   whether it is on by default.
*/
#pragma CAKE diagnostic warning 82

struct X { int i; int arr[4]; char* buf; };
struct X* g_ptr;

void g(struct X* q);
void gc(const struct X* q);
void gi(int* q);
void gic(const int* q);
int printf(const char* fmt, ...);

/* ---------- reported ---------- */

int read_member(struct X* p)
{
    return p->i;
} //lint 82

int read_deref(int* p)
{
    return *p;
} //lint 82

int read_index(struct X* p)
{
    return p->arr[2];
} //lint 82

int read_paren_deref(struct X* p)
{
    return (*p).i;
} //lint 82

void pass_to_const_callee(struct X* p)
{
    gc(p);
} //lint 82

void addr_of_member_to_const(struct X* p)
{
    gic(&p->i);
} //lint 82

/* Writing through a POINTER member does not modify *p, and stays legal
   under `const struct X*`. */
void write_through_pointer_member(struct X* p)
{
    p->buf[0] = 'x';
} //lint 82

/* An array parameter and a pointer to array are reported too. */
int read_array_param(int a[], int n)
{
    return n > 0 ? a[0] : 0;
} //lint 82

int read_pointer_to_array(int (*a)[10])
{
    return (*a)[0];
} //lint 82

/* ---------- not reported ---------- */

void write_member(struct X* p) { p->i = 1; }
void write_deref(int* p) { *p = 1; }
void write_compound(struct X* p) { p->i += 1; }
void write_postinc(struct X* p) { p->i++; }
void write_predec(struct X* p) { --p->i; }

/* An ARRAY member does live inside *p. */
void write_array_member(struct X* p) { p->arr[1] = 5; }

/* Escapes: the callee, an alias, a global, the return value, a cast,
   a variadic argument -- each could write later. */
void pass_to_non_const_callee(struct X* p) { g(p); }
void addr_of_member_to_non_const(struct X* p) { gi(&p->i); }
void alias(struct X* p) { struct X* q = p; q->i = 1; }
void store_in_global(struct X* p) { g_ptr = p; }
struct X* returned(struct X* p) { return p; }
void cast_away(struct X* p) { g((struct X*)p); (void)p->i; }
void variadic(struct X* p) { printf("%p", (void*)p); (void)p->i; }
void conditional(struct X* p, int c) { struct X* q = c ? p : g_ptr; g(q); (void)p->i; }

/* Already const. */
void already_const(const struct X* p) { (void)p->i; }
void already_const_array(const int a[]) { (void)a[0]; }
void already_const_ptr_to_array(const int (*a)[10]) { (void)(*a)[0]; }

/* Unevaluated operands never touch the object, so they are not uses. */
unsigned long only_sizeof(int* p) { return sizeof(*p); }

/* Never dereferenced at all -- warning 6's subject, not this one. */
void never_dereferenced(struct X* p) { p = 0; (void)p; }

int main(void)
{
    return 0;
}
