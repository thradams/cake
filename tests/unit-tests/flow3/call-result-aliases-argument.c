#pragma safety enable

void* _Owner malloc(unsigned long n);
void free(void* _Owner _Nullable p);

struct item { int value; };
struct list { struct item* _Owner head; };

int* _Nonnull first(int* p);
void* _Nonnull data(void* p);
int* _Nonnull copy_of(int* restrict p);
int* _Nonnull find(struct item* it);
struct item* _Nonnull list_add(struct list* l, struct item* _Owner it);

/* the result may be the argument */
int result_is_argument(void)
{
    int* _Owner p = malloc(sizeof(int));
    if (!p) return 0;
    *p = 1;
    int* q = first(p);
    free(p);
    return *q; //lint 31
}

/* a void* result may be any argument */
int void_result(void)
{
    int* _Owner p = malloc(sizeof(int));
    if (!p) return 0;
    *p = 1;
    int* q = data(p);
    free(p);
    return q[0]; //lint 31
}

/* no use after free: nothing to report */
int used_before_free(void)
{
    int* _Owner p = malloc(sizeof(int));
    if (!p) return 0;
    *p = 1;
    int* q = first(p);
    int v = *q;
    free(p);
    return v;
}

/* a restrict parameter is not returned */
int restrict_parameter(void)
{
    int* _Owner p = malloc(sizeof(int));
    if (!p) return 0;
    *p = 1;
    int* q = copy_of(p);
    free(p);
    return *q;
}

/* int* cannot point to the struct item argument */
int other_pointed_type(void)
{
    struct item* _Owner it = malloc(sizeof(struct item));
    if (!it) return 0;
    it->value = 1;
    int* v = find(it);
    free(it);
    return *v;
}

/* an owner moved into the callee is kept alive by it */
int moved_into_callee(struct list* l)
{
    struct item* _Owner it = malloc(sizeof(struct item));
    if (!it) return 0;
    it->value = 1;
    struct item* view = list_add(l, it);
    return view->value;
}
