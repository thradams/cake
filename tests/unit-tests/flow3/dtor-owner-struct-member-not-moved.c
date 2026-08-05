#pragma safety enable

/*
   Regression test for a false positive found in cake's own source
   (hashmap.c), reported as:

     struct hash_item_set item = { 0 };
     hashmap_set(&ctx->p_ast->file_scope.variables, new_name, &item);
     hash_item_set_destroy(&item);

   flow3 warned "owner object (.xxx) not moved" for every _Owner member
   of `item`, even though hash_item_set_destroy is declared
   `_Dtor struct hash_item_set*` and is exactly the function whose job is
   to release those members.

   Root cause: passing `&item` to a _Dtor parameter correctly marks each
   leaf member's alternative as FLOW3_IMAGINARY_ENDED
   (flow3_map_set_object_lifetime_ended), but the scope-exit "_Owner not
   moved" check (flow3_check_object_at_exit) only ever treated
   FLOW3_IMAGINARY_MOVED as a resolved owner -- ENDED fell through to the
   same branch as "still live", so it warned regardless. Fixed by
   accepting ENDED alongside MOVED.
*/

struct payload { int x; };
void payload_free(struct payload* _Owner _Opt p);
void free(void* _Owner _Opt p);

struct item_set
{
    unsigned long number;
    struct payload* _Owner _Opt p1;
    struct payload* _Owner _Opt p2;
    char* _Owner _Opt text;
};

void item_set_destroy(_Dtor struct item_set* p)
{
    payload_free(p->p1);
    payload_free(p->p2);
    free(p->text); /* the _Dtor contract requires the callee to release
                      every owner member: flow3_check_dtor_params_at_exit
                      flags this function if it is missing */
}

struct map { int x; };
int map_set(struct map* map, const char* key, struct item_set* item);

void use(struct map* map, const char* new_name)
{
    struct item_set item = { 0 };
    map_set(map, new_name, &item); /* plain pointer: item's fields become ANY, not owner-tracked-live */
    item_set_destroy(&item); /* ends p1/p2/text's lifetimes -- nothing left to flag */
}

/* Contrast: forgetting the _Dtor call is still correctly flagged. */
void use_forgets_destroy(struct map* map, const char* new_name)
{
    struct item_set item = { 0 }; // owner object (.p1) not moved (root see line 60); owner object (.p2) not moved (root see line 60); owner object (.text) not moved (root see line 60)
    map_set(map, new_name, &item);
} //lint 29 29 29 owner object (.p1) not moved (root see line 60)
