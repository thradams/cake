#pragma safety enable

/* a write through a pointer with two targets writes the one of the path */

void assign(int c)
{
    int a = 1;
    int b = 2;
    int* p = &a;
    if (c) { p = &b; }
    *p = 10;
    if (c)
    {
        compile_assert(b == 10);
        compile_assert(a == 1);
    }
    else
    {
        compile_assert(a == 10);
        compile_assert(b == 2);
    }
}

void compound_assign(int c)
{
    int a = 1;
    int b = 2;
    int* p = &a;
    if (c) { p = &b; }
    *p += 10;
    if (c)
    {
        compile_assert(b == 12);
        compile_assert(a == 1);
    }
    else
    {
        compile_assert(a == 11);
        compile_assert(b == 2);
    }
}

void conditional_operator(int c)
{
    int a = 1;
    int b = 2;
    int* p = c ? &a : &b;
    *p = 10;
    if (c)
    {
        compile_assert(a == 10);
        compile_assert(b == 2);
    }
}

void member_targets(int c)
{
    struct { int x; int y; } s1 = { 1, 1 }, s2 = { 2, 2 };
    int* p = &s1.x;
    if (c) { p = &s2.x; }
    *p = 7;
    if (c)
    {
        compile_assert(s1.x == 1);
        compile_assert(s2.x == 7);
    }
    else
    {
        compile_assert(s1.x == 7);
        compile_assert(s2.x == 2);
    }
}

void unknown_index(int c, int i)
{
    int a[2] = { 1, 1 };
    int b[2] = { 2, 2 };
    int* p = a;
    if (c) { p = b; }
    p[i] = 5;
    if (c)
    {
        compile_assert(a[1] == 1);
    }
    else
    {
        compile_assert(b[1] == 2);
    }
}

void set(int* p);

void call(int c)
{
    int a = 1;
    int b = 2;
    int* p = &a;
    if (c) { p = &b; }
    set(p);
    if (c)
    {
        compile_assert(a == 1);
    }
    else
    {
        compile_assert(b == 2);
    }
}

/* the targets of a pointer carried around a loop cannot be told apart: each
   is written as before, and p->next reads the value just written */
struct node { struct node* _Owner _Opt next; };
void node_delete(struct node* _Owner _Opt p);

void list_clear(struct node* _Owner _Opt head)
{
    struct node* _Owner _Opt p = head;
    while (p)
    {
        struct node* _Owner _Opt next = p->next;
        p->next = 0;
        node_delete(p);
        p = next;
    }
}
