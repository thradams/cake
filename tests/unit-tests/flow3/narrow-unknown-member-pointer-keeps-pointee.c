/* After a call l->head is unknown; `l->head == 0` leaves a pointer "!= null", so `->` has a pointee and the test of l->head->o holds in the body */
#pragma safety enable

struct O { const char* _Nonnull lexeme; };
struct T { struct O* o; };
struct L { struct T* head; };
void g(const char* s);
void pop(struct L* l);

void f1(struct L* l)
{
    pop(l);
    if (l->head == 0) return;
    if (l->head->o) g(l->head->o->lexeme);
}

void f2(struct L* l)
{
    pop(l);
    if (l->head == 0) return;
    g(l->head->o ? l->head->o->lexeme : "");
}
