#pragma safety enable

/* a _Dtor call on one member ends only that member, later access to sibling members must not warn (codegen_visit_ctx_destroy) */

struct hash_map { int* _Owner _Opt data; };
void hashmap_destroy(_Dtor struct hash_map* map);

struct ctx {
    struct hash_map tag_names;
    struct hash_map structs_map;
    struct hash_map file_scope_declarator_map;
};

/* must NOT warn: each hashmap_destroy ends only its own member */
void destroy_members_one_by_one(_Dtor struct ctx* ctx)
{
    hashmap_destroy(&ctx->tag_names);
    hashmap_destroy(&ctx->structs_map);
    hashmap_destroy(&ctx->file_scope_declarator_map);
}

/* genuine reuse of the destroyed member is still caught: its contents are uninitialized (30), the storage itself still exists */
struct node { int x; };
void destroy_node(_Dtor struct node* n);
struct holder { struct node* _Owner a; };

int use_after_destroy_same_member_warns(_Dtor struct holder* h)
{
    destroy_node(h->a);
    int v = h->a->x; /* warns: h->a's contents are uninitialized after destroy_node */ //lint 30
    return v; //lint 29 30
}
