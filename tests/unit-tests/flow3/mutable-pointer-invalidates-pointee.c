#pragma safety enable

/*
   This is the "variant of _Clear" the user pointed out:

     struct X { int i; };
     void f(struct X* p) {}
     int main() {
       struct X x = make();
       f(&x);
       // f is not const, so we must assume x can be changed inside it,
       // even though f's own body (or f may only be declared, not
       // defined) does nothing we can see.
     }

   f has no write-qualifier at all (no _Clear/_Ctor/_Dtor) and its
   parameter is a plain non-const pointer. That means the callee is free
   to write ANYTHING through it -- flow3 has no way to know it won't. Any
   value flow3 previously knew about *p must therefore be treated as
   unknown (any) the moment the address is passed to such a parameter,
   the same way _Clear resets the pointee to a known 0 and _Ctor resets
   it to a known-initialized-but-unknown value.

   Before this fix, flow3_check_object_init_assigment only invalidated the
   pointee for the specific qualifiers it already knew about (_Clear ->
   zero, _Dtor -> lifetime ended, _Ctor -> any, _Owner pointer -> lifetime
   ended); a plain mutable pointer parameter fell through all four checks
   and the pointee's old alternatives were left completely untouched --
   confirmed with:

     struct X{ int i; };
     void f(struct X *p){}
     int main(){
       struct X x = {5};
       compile_assert(x.i == 5);
       f(&x);
       compile_assert(x.i == 5); // WRONGLY passed before the fix
     }

   Fixed by adding a final case: if none of _Clear/_Dtor/_Ctor/_Owner
   applied and the pointee isn't const, invalidate it to ANY (same
   primitive _Ctor already used: flow3_map_set_object_any).
*/

struct X
{
    int i;
};

void f(struct X* p) {}
void f_const(const struct X* p) {}

struct X make(void);

void mutable_pointer_invalidates_known_value(void)
{
    struct X x = { 5 };
    compile_assert(x.i == 5);
    f(&x);
    /* x.i is no longer trusted to be 5 -- f could have written anything */
    // static_debug(x);
    // compile_assert(x.i == 5); // fails now: value could be any value
}

void const_pointer_does_not_invalidate(void)
{
    struct X x = { 5 };
    f_const(&x);
    /* f_const's parameter points to const: it cannot write through p,
       so whatever flow3 knew about x.i survives the call unchanged */
    compile_assert(x.i == 5);
}

void repeated_calls_stay_any(void)
{
    struct X x = { 5 };
    f(&x);
    f(&x); /* still just "any": a second mutating call doesn't un-know it */
    // static_debug(x);
}

struct pair
{
    int a;
    int b;
};

void touch_pair(struct pair* p) {}

void whole_struct_invalidated_recursively(void)
{
    /* the invalidation recurses into every leaf member, same as _Clear */
    struct pair p = { 1, 2 };
    touch_pair(&p);
    // static_debug(p);
    // compile_assert(p.a == 1); // fails: any value
    // compile_assert(p.b == 2); // fails: any value
}
