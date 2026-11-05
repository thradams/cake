#pragma safety enable

/*
   any operation on a non-null pointer (+ - ++ -- += -=) gives a non-null pointer.
   Array elements are not covered: arrays are not ready.
*/

struct x { const char* _Nonnull s; };
const char* _Nonnull get(void);

void plus(const char* p)
{
    const char* q = p + 1;
    compile_assert(q != 0);
    (void)*q;
}

void minus(const char* p)
{
    const char* q = p - 1;
    compile_assert(q != 0);
    (void)*q;
}

void int_plus_pointer(const char* p)
{
    const char* q = 1 + p;
    compile_assert(q != 0);
    (void)*q;
}

void unknown_offset(const char* p, int n)
{
    const char* q = p + n;
    compile_assert(q != 0);
    (void)*q;
}

void chained(const char* p)
{
    const char* q = p + 2 - 1;
    compile_assert(q != 0);
    (void)*q;
}

void postfix_increment(const char* p)
{
    p++;
    compile_assert(p != 0);
    (void)*p;
}

void postfix_decrement(const char* p)
{
    p--;
    compile_assert(p != 0);
    (void)*p;
}

void prefix_increment(const char* p)
{
    ++p;
    compile_assert(p != 0);
    (void)*p;
}

void prefix_decrement(const char* p)
{
    --p;
    compile_assert(p != 0);
    (void)*p;
}

void plus_assign(const char* p)
{
    p += 2;
    compile_assert(p != 0);
    (void)*p;
}

void minus_assign(const char* p)
{
    p -= 2;
    compile_assert(p != 0);
    (void)*p;
}

void increment_result(const char* p)
{
    const char* a = p++;
    const char* b = ++p;
    compile_assert(a != 0);
    compile_assert(b != 0);
}

void local_copy(const char* p)
{
    const char* l = p;
    const char* q = l + 1;
    l++;
    compile_assert(q != 0);
    compile_assert(l != 0);
}

void narrowed_opt(const char* _Opt p)
{
    if (p != 0)
    {
        const char* q = p + 1;
        compile_assert(q != 0);
        p++;
        compile_assert(p != 0);
        p += 1;
        compile_assert(p != 0);
    }
}

void cast_operand(void* p)
{
    const char* q = (const char*)p + 1;
    compile_assert(q != 0);
    (void)*q;
}

/* the value of a compound assignment */
void compound_assignment_result(const char* p)
{
    const char* a = (p += 1);
    const char* b = (p -= 1);
    compile_assert(a != 0);
    compile_assert(b != 0);
}

/* the value of a compound assignment with two possible destinations */
void compound_assignment_two_targets(const char* a, const char* b, int c)
{
    const char** p = c ? &a : &b;
    const char* q = (*p += 1);
    compile_assert(q != 0);
}

/* _Nonnull pointer read through a pointer */
void through_pointer(const char* _Nonnull const* pp)
{
    const char* q = *pp + 1;
    compile_assert(q != 0);
    (void)*q;
}

/* _Nonnull struct member */
void member(const struct x* p)
{
    const char* q = p->s + 1;
    (void)*q;
}

void member_write(struct x* p)
{
    p->s++;
    (void)*p->s;
    p->s += 1;
    (void)*p->s;
}

void member_by_value(struct x v)
{
    const char* q = v.s + 1;
    (void)*q;
}

/* _Nonnull function result */
void call_result(void)
{
    const char* q = get() + 1;
    compile_assert(q != 0);
    (void)*q;
}

void call_result_increment(void)
{
    const char* q = get();
    q++;
    compile_assert(q != 0);
}
