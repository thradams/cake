/* warning 61: the cast does nothing because the operand already has this type */

typedef int my_int;
typedef int* int_ptr;
typedef struct X x_t;

struct X { int i; char* s; };
enum E { A };

int get_int(void);
char* get_str(void);

#define TO_INT(x) ((int)(x))

/* ---- the warning appears ---- */

void same_scalar(int i, double d, enum E e)
{
    int a = (int)i; //lint 61
    double b = (double)d; //lint 61
    enum E c = (enum E)e; //lint 61
    (void)a;
    (void)b;
    (void)c;
}

void same_pointer(char* s, const char* cs, void* v, struct X* x, char** pp)
{
    char* a = (char*)s; //lint 61
    const char* b = (const char*)cs; //lint 61
    void* c = (void*)v; //lint 61
    struct X* d = (struct X*)x; //lint 61
    char** e = (char**)pp; //lint 61
    (void)a;
    (void)b;
    (void)c;
    (void)d;
    (void)e;
}

void same_member_and_call(struct X* x)
{
    int a = (int)x->i; //lint 61
    char* b = (char*)x->s; //lint 61
    int c = (int)get_int(); //lint 61
    char* d = (char*)get_str(); //lint 61
    (void)a;
    (void)b;
    (void)c;
    (void)d;
}

/* ---- the warning does not appear ---- */

/* the type changes */
void different_type(int i, long l, char* s, struct X* x)
{
    long a = (long)i;
    int b = (int)l;
    void* c = (void*)s;
    char* d = (char*)x;
    (void)a;
    (void)b;
    (void)c;
    (void)d;
}

/* the base type of an enum is not fixed, and A | B holds more than the enumerators */
void enum_conversion(enum E e, unsigned int u, const enum E* p)
{
    unsigned int a = (unsigned int)e;
    unsigned int b = (unsigned int)*p;
    enum E c = (enum E)u;
    enum E d = (enum E)(A | e);
    enum E f = (enum E)A; /* an enumerator is an int */
    (void)f;
    (void)a;
    (void)b;
    (void)c;
    (void)d;
}

/* the type of a computed value depends on the promotions of the target */
void computed_value(enum E e, int i)
{
    unsigned int a = (unsigned int)(e - A + 1);
    int b = (int)(i + 1);
    (void)a;
    (void)b;
}

/* a qualifier changes */
void different_qualifier(const char* cs, char* s)
{
    char* a = (char*)cs;
    s[0] = 0;
    const char* b = (const char*)s;
    (void)a;
    (void)b;
}

/* reached through const: without the cast it is warning 120 */
struct Y { struct X* x; };
char* through_const(const struct Y* y)
{
    return (char*)y->x->s;
}

/* the result is discarded */
void nothing(void);
void cast_to_void(int i)
{
    (void)i;
    (void)nothing();
}

/* typedef in the cast */
void typedef_in_the_cast(int i, int* p, struct X* x)
{
    my_int a = (my_int)i;
    int_ptr b = (int_ptr)p;
    my_int* c = (my_int*)p;
    x_t* d = (x_t*)x;
    (void)a;
    (void)b;
    (void)c;
    (void)d;
}

/* typedef in the operand */
void typedef_in_the_operand(my_int i, int_ptr p, my_int* q, x_t* x)
{
    int a = (int)i;
    int* b = (int*)p;
    int* c = (int*)q;
    struct X* d = (struct X*)x;
    (void)a;
    (void)b;
    (void)c;
    (void)d;
}

/* inside a macro the types depend on the arguments */
void inside_macro(int i)
{
    int a = TO_INT(i);
    (void)a;
}
