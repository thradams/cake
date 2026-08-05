#pragma safety enable

/*
   A _Dtor call on ONE member of a struct must end the lifetime of only that
   member (and its own sub-members, if any) -- not the entire containing
   object.

   flow3_map_set_object_lifetime_ended correctly recurses into obj->members
   and marks only the LEAVES of the object it is given as ended. But the ->
   operator's "pointed object lifetime has ended" check used to walk
   "any leaf ended" over the WHOLE pointed-to object instead of the specific
   member being accessed. So after hashmap_destroy(&ctx->tag_names), every
   later ctx-> access -- even to a completely unrelated sibling member --
   was falsely flagged as using an object whose lifetime had ended.

   This is the exact shape of codegen_visit_ctx_destroy in codegen.c, which
   surfaced the bug: several hashmap_destroy(&ctx->X) calls in a row, each
   on a different member.
*/

struct hash_map { int* _Owner _Opt data; };
void hashmap_destroy(_Dtor struct hash_map* map);

struct ctx {
    struct hash_map tag_names;
    struct hash_map structs_map;
    struct hash_map file_scope_declarator_map;
};

/* Must NOT warn: each hashmap_destroy ends only its own member's lifetime,
   never a sibling's. Before the fix, every access after the first
   hashmap_destroy falsely reported "pointed object lifetime has ended". */
void destroy_members_one_by_one(_Dtor struct ctx* ctx)
{
    hashmap_destroy(&ctx->tag_names);
    hashmap_destroy(&ctx->structs_map);
    hashmap_destroy(&ctx->file_scope_declarator_map);
}

/* The check must still catch genuine reuse of the SAME object whose
   lifetime ended -- the fix narrows the scope of the check, it must not
   disable it. */
struct node { int x; };
void destroy_node(_Dtor struct node* n);
struct holder { struct node* _Owner a; };

int use_after_destroy_same_member_warns(_Dtor struct holder* h)
{
    destroy_node(h->a);
    int v = h->a->x; /* warns: h->a's lifetime already ended */ //lint 31 31
    return v; //lint 29
}
