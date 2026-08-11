#pragma safety enable

/*
   False "operator -> applied to a possible null pointer" starting from the
   SECOND (and every subsequent) `||` operand that dereferences the same
   member, after an earlier operand passed the enclosing pointer to a
   function taking a plain (non-const) pointer parameter.

   Reduced from src/expressions.c's is_first_of_unary_expression:

     return first_of_postfix_expression(ctx) ||
         ctx->current->type == '++' ||
         ctx->current->type == '--' ||   // starts warning here
         ...

   Root cause (confirmed via instrumenting flow3_seed_member_default):
   `fpost(ctx)` -- ctx is a plain mutable (non-const, non-owner) pointer
   argument -- correctly widens `*ctx` to ANY (flow3_map_set_object_any_n),
   since the callee could have modified `ctx->current` through the pointer.

   The FIRST `ctx->current->type` access afterward resolves `.current`'s
   real `struct object*` to the SAME instance that carries the ANY state
   (search_up finds it, an ANY alternative isn't flagged as "possibly
   null", so no warning -- silently accepted).

   The SECOND access resolves `.current` to a DIFFERENT `struct object*`
   instance (confirmed via pointer identity, not just designator) --
   apparently a fresh on-demand-fabricated pointee for `*ctx` rather than
   the one the ANY-widening was applied to. flow3_seed_member_default sees
   no flow state for THIS instance and seeds a fresh conservative
   "_Opt member -> maybe null" default (FLOW3_MAP_OPT_MEM_NULL /
   FLOW3_MAP_OPT_MEM_NONNULL), which the null-check then flags on the very
   same and every later occurrence.

   ACTUAL root cause (found by tracing further, confirmed via instrumenting
   flow3_map_merge_arms directly): its cleanup loop (the "drop each arm's
   own buckets" pass right after flow3_map_move_entries) assumed every arm
   it walked from arms[i] up to `parent` was a fresh scratch map created
   exclusively for this merge, and freed each map's entries along the way
   once its data had been copied into `parent`. But an arm that never
   actually branched -- e.g. a plain function call as the left operand of
   `||`, whose fallback branch pair is `{p_before, p_before}` -- IS
   p_before itself: an ancestor `parent` was just built on top of, not a
   descendant. Walking "up" from such an arm never reached `parent`
   (parent is below it), so the loop kept climbing past it all the way to
   the true root, calling flow3_map_free_entries() on every real ancestor
   map along the way -- wiping out live state (here, `.current`'s
   ANY-widening from the `fpost(ctx)` call effect) that later,
   still-pending `||` operands in the SAME statement still needed to read.

   FIXED: a plain "stop at parent's own ancestors" guard on the cleanup
   loop wasn't enough by itself -- it fixed this exact case but regressed
   `_Assert(p && p->next); p->i = 1;`-style code, because a chain
   interposed by flow3_narrow_map_branch can also be SHARED between two
   SIBLING branches across different merges (e.g. `L && R`: R is evaluated
   once starting from L's true-branch map, so right_pair.p_true and
   right_pair.p_false are siblings off that same map; freeing along
   right_pair.p_false's chain during the `&&`'s own internal false-branch
   merge wiped out L's narrowing that right_pair.p_true -- returned as the
   `&&`'s true result and used afterward -- still needed). There is no
   cheap local way to tell "exclusively mine" from "shared with a sibling
   still alive elsewhere" without reference counting the whole map arena,
   so the cleanup loop no longer frees anything at all: these maps are
   arena-allocated and reclaimed in bulk when the declaration's analysis
   finishes, so this trades a memory-only optimization for correctness. */


struct token { int type; };
struct ctx { struct token* _Opt current; };

int fpost(struct ctx* ctx);

int f(struct ctx* ctx)
{
    if (ctx->current == 0)
        return 0;

    return fpost(ctx) ||
        ctx->current->type == 1 ||
        ctx->current->type == 2 ||
        ctx->current->type == 3;
}
