#pragma safety enable

/* FIXED: `out = *list[j];` from a source with no member state sets the destination's members to ANY, so `if (out.k != 0)` is reachable (arrays excluded, string literals zero-fill) */

struct s2 { int k; int v; };

void two_int_members(const struct s2* _Opt const* _Opt list, int count)
{
    for (int j = 0; j < count; j++)
    {
        struct s2 out = {0};
        out = *list[j];
        if (out.k != 0) { break; } /* reachable: *list[j] can hold anything */
    }
}

/* Same defect with a union member, matching the original struct's shape. */
struct s_union
{
    int value_kind;
    union { long long i; unsigned long long u; void* p; } value;
    int rel;
};

void with_union_member(const struct s_union* _Opt const* _Opt list, int count)
{
    for (int j = 0; j < count; j++)
    {
        struct s_union out = {0};
        out = *list[j];
        if (out.value_kind != 0) { break; } /* reachable */
    }
}
