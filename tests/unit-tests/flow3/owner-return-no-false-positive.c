#pragma safety enable

/*
   Regression test for the MOVED/ENDED interaction.

   flow3_defer_list_set_end_of_lifetime marks every alternative of an
   exiting object as ENDED. It used to do this unconditionally,
   including alternatives that were already MOVED (e.g. an owner
   returned to the caller). A later exit-check pass (the enclosing
   scope's own defer list, run after the return statement's checks)
   would then see ENDED instead of MOVED and report a false
   "owner object not moved".
*/

void* _Owner _Uninitialized malloc(unsigned long size);

int* _Owner f_return_owner(void)
{
    int* _Owner p = malloc(sizeof(int));
    return p; /* moves p to the caller: must NOT warn "not moved" */
}

int* _Owner _Opt f_return_owner_opt(void)
{
    int* _Owner _Opt p = malloc(sizeof(int));
    return p; /* same, with an optional owner */
}
