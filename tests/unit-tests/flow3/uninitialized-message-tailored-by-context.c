#pragma safety enable

/*
   W_FLOW_UNINITIALIZED (30) used to always say "passing a possible
   uninitialized object '%s'", regardless of whether the uninitialized value
   was actually being passed to a function, returned, or just assigned to a
   plain local -- and when the source had no member_designator of its own
   (e.g. a dereferenced pointer's pointee, `*p`), the name printed as an
   empty ''. User-reported repro:

       int * _Owner _Opt p = malloc(sizeof (int));
       int j;
       j = *p; // warned: passing a possible uninitialized object  '' (see line ...)

   Fixed by tailoring the verb to enum init_type (INIT_OBJ -> "assigning",
   INIT_PARAMETER -> "passing", INIT_RETURN -> "returning") and falling back
   to rendering the source expression itself (e.g. "*p") when there is no
   member name to show.
*/

void* _Owner _Opt _Uninitialized malloc(unsigned long size);
void free(void* _Owner _Opt ptr);
void take(int i);

/* Plain assignment: must say "assigning" and show '*p'. */
int assign_from_uninitialized_pointee(void)
{
    int * _Owner p = malloc(sizeof(int)); //lint 35
    int j;
    j = *p; //lint 33 30
    free(p);
    return j; //lint 30
}

/* Function argument: must say "passing" and show '*p'. */
void pass_uninitialized_pointee(void)
{
    int * _Owner p = malloc(sizeof(int)); //lint 35
    take(*p); //lint 33 30
    free(p);
}

/* Return statement: must say "returning" and show '*p'. Leaks p (never
   freed) on this path -- accepted here purely to exercise the return-site
   diagnostic in isolation; not a pattern to copy. */
int return_uninitialized_pointee(void)
{
    int * _Owner p = malloc(sizeof(int)); //lint 35
    return *p; //lint 33 30 29
}
