#pragma safety enable

void accepts_null(int** p);
void rejects_null(int* _Nonnull* p);

struct X { int* _Nonnull m; };
void use_void(void* p);
void use_x(struct X* p);

void parameter_to_void(int* p)
{
    p = 0;
    use_void(&p);
}

/* one level only: the member of a parameter keeps its own _Nonnull */
void member_of_parameter(struct X x)
{
    x.m = 0; //lint 35
    use_x(&x); //lint 35
}

void parameter_to_accepts_null(int* p)
{
    p = 0;
    accepts_null(&p);
}

void parameter_to_rejects_null(int* p)
{
    p = 0;
    rejects_null(&p); //lint 35
}

void local_to_accepts_null(void)
{
    int* q = 0;
    accepts_null(&q);
}

void local_to_rejects_null(void)
{
    int* q = 0;
    rejects_null(&q); //lint 35
}

void two_alternatives(int* p, int c)
{
    int* q = p;
    if (c)
        q = 0;
    accepts_null(&q);
    rejects_null(&q); //lint 35
}
