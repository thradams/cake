#pragma safety enable

/*
   Forward goto joining with a fall-through path around an ownership
   move: the goto path skips free(p) entirely, the fall-through path
   calls free(p). Both paths converge at "end:", so the merged state
   there must show p as "not moved" (since the goto path never moved
   it) and the leak must be reported. This exercises the same
   goto/label state-accumulation path as goto-forward-loses-state.c.
*/

void free(void* _Owner _Opt ptr);
void* _Owner _Opt _Uninitialized malloc(int size);

void f(int condition)
{
    int* _Owner _Opt p = malloc(sizeof(int)); // owner object (p) not moved (call-opt-nonnull see line 17)

    if (condition)
    {
        goto end;
    }

    free(p);
end:
} //lint 29 owner object (p) not moved (call-opt-nonnull see line 17)
