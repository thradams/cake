#pragma safety enable

/* issue 489: a block-scope compound literal's lifetime ends at the end of the enclosing block */

char* _Owner strdup(const char* s);
void free(void* _Owner _Nullable ptr);

struct X { char* _Owner name; };

void take(struct X x)
{
    free(x.name);
}

/* must warn: the literal still owns name at the end of the block */
void leaks(void)
{
    struct X* p = &(struct X){ strdup("a") };
    (void)p;
} //lint 29

/* must NOT warn: name was moved out of the literal through p */
void moved_member(void)
{
    struct X* p = &(struct X){ strdup("a") };
    char* _Owner s = p->name;
    free(s);
}

/* must NOT warn: the whole literal was moved into x */
void moved_to_variable(void)
{
    struct X x = (struct X){ strdup("a") };
    free(x.name);
}

/* must NOT warn: the literal is moved into the argument */
void moved_to_argument(void)
{
    take((struct X){ strdup("a") });
}

/* must NOT warn: a static compound literal is not ended at the end of the block */
void static_literal(void)
{
    struct X* p = &(static struct X){ 0 };
    (void)p;
}

/* must NOT warn: static variables are not ended at the end of the block */
void static_variable(void)
{
    static char* _Owner _Nullable s = 0;
    free(s);
    s = strdup("a");
}
