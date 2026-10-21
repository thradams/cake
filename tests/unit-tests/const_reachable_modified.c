/* warning 120: the analysis assumes a call through const changes nothing reachable (flow.md 6.3); implementations that do not agree */

struct Y { int value; };
struct X { int i; struct Y * p; };

void f0(const struct Y * y);
void f1(struct Y * y);

/* writing below a const pointee */
void writes(const struct X * x)
{
    x->p->value = 1; //lint 120
    x->p->value++; //lint 120
    x->p->value += 2; //lint 120
    (*x).p->value = 3; //lint 120
}

/* forwarding to a const parameter: that function's own check */
void forwards_const(const struct X * x)
{
    f0(x->p);
}

/* forwarding to a non-const parameter: the callee may change it */
void forwards_mutable(const struct X * x)
{
    f1(x->p); //lint 120
}

/* a void* parameter is assumed not to change it */
void * memset(void * s, int c, unsigned long n);
void forwards_void(const struct X * x)
{
    memset(x->p, 0, sizeof * x->p);
}

/* a const member: what it reaches is not changed either */
struct L { struct Y * data; };
struct R { const struct L * listener; };
void member_const(struct R * r)
{
    f1(r->listener->data); //lint 120
    r->listener->data->value = 1; //lint 120
}

/* a const local: it may not be the intention either */
void local_const(struct X * x)
{
    const struct X * c = x;
    c->p->value = 1; //lint 120
} //lint 82

/* an explicit cast is the programmer dropping const */
void casts(const struct X * x)
{
    ((struct Y *) x->p)->value = 1;
}

/* not const: nothing */
void mutable_x(struct X * x)
{
    x->p->value = 1;
    f1(x->p);
}

/* reading is fine */
int reads(const struct X * x)
{
    return x->p->value;
}
