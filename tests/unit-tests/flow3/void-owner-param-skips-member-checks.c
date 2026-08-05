#pragma safety enable

/*
   Regression test for a false positive reported directly against real
   code shaped like this:

     struct X { int i; };
     void free(void* _Owner _Opt p);
     void x_destroy(_Dtor struct X* p);
     void f(struct X* _Owner p) {
         x_destroy(p);
         free(p); // "object '(*p).i' lifetime has ended" -- false positive
     }

   x_destroy (a _Dtor call) correctly ends the lifetime of *p's members.
   The following free(p) should just consume p itself -- free's own
   parameter type is void*, a type-erased pointer through which the
   callee cannot read or access any member of whatever *p used to be.

   Root cause: flow3_check_object_init_assigment always recursed into
   the pointee's own tracked state (flow3_check_object_access, which
   reports uninitialized/null/moved/lifetime-ended members) whenever the
   source alternative was a concrete pointer-to-object, regardless of
   what the DESTINATION parameter's type actually was. So passing p to
   a void* parameter still walked and reported on *p's already-ended
   members, even though the call itself could never have looked at
   them. Fixed by skipping that member-state check entirely when the
   destination parameter's type is void* (type_is_void_ptr) -- moving
   the pointer itself (via flow3_map_set_object_moved, for _Owner
   destinations) is unaffected and still happens correctly.
*/

struct X { int i; };
void free(void* _Owner _Opt p);
void x_destroy(_Dtor struct X* p);

void f(struct X* _Owner p)
{
    x_destroy(p);
    free(p); /* ok: void* can't access *p's members, so nothing to flag */
}

/* Contrast: passing the same ended-lifetime object to a parameter that
   COULD actually access its members (not void*) is still correctly
   flagged. */
void read_x(struct X* p);

void g(struct X* _Owner p)
{
    x_destroy(p);
    read_x(p); //lint 31 object '(*p).i' lifetime has ended (see line 48)
} //lint 29 owner object (p) not moved (root see line 48)
