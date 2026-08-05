#pragma safety enable

/*
   p is an alias of pX (not a separate owner). free(pY) frees the object
   pX->pY points to, but neither pX->pY nor the alias p is updated to
   reflect that -- so p->pY->i = 1 dereferences through a member that was
   just freed.

   Root cause: the `->` operator's "pointed object lifetime has ended"
   check filtered candidate ENDED facts by an origin-ancestor check against
   the POINTER alternative's own origin (p->pY's tracked value, whose
   origin dates back to `pX->pY = pY;`). That origin has no ancestor
   relationship to the origin of the ENDED fact recorded later by free(pY)
   (a different, still-open branch) -- confirmed via static_debug(0),
   which showed the two origins as unrelated siblings from each other's
   perspective, even though both are simultaneously active at the point of
   the dereference. Fixed by also accepting the fact if its origin is an
   ancestor of ctx->p_current_flow3_map (the CURRENT map), not just of the
   pointer alternative's origin.

   That fix had to be layered ON TOP of the original single-filter check,
   not replace it: dangling-pointer-to-local.c needs exactly the original
   pointer-alternative-origin filter (after a branch merge, only that
   alternative's own origin correctly correlates it with the matching
   ended fact from the SAME branch -- ctx->p_current_flow3_map post-merge
   is not that branch's descendant). Both shapes are exercised below,
   alongside the two false-positive regressions the original filter
   existed to prevent (they are their own files:
   owner-reassigned-null-in-catch-false-positive.c and
   deref-after-catch-reset-false-positive.c).
*/

void* _Owner _Opt _Clear calloc(unsigned int n, unsigned long size);
void free(void* _Owner _Opt ptr);

struct Y {
    int i;
};

struct X {
    int i;
    struct Y* _Opt pY;
};

/* Must warn: p is an alias of pX, pX->pY was freed via the pY alias, and
   the dereference through p->pY reaches the same, now-dead object. */
int aliased_free_not_seen_through_alias(void)
{
    struct X* _Owner _Opt pX = calloc(1, sizeof *pX);
    if (pX)
    {
        struct Y* _Owner _Opt pY = calloc(1, sizeof *pY);
        if (pY)
        {
            pX->pY = pY;
            struct X* _Opt p = pX;
            free(pY);
            p->pY->i = 1; /* warns: p->pY's pointee lifetime has ended */ //lint 31
        }
        free(pX);
    }
    return 0;
}

/* Must NOT warn: no free happened, p->pY is still live. */
void aliased_no_free_is_clean(void)
{
    struct X* _Owner _Opt pX = calloc(1, sizeof *pX);
    if (pX)
    {
        struct Y* _Owner _Opt pY = calloc(1, sizeof *pY);
        if (pY)
        {
            pX->pY = pY;
            struct X* _Opt p = pX;
            p->pY->i = 1;
            free(pY);
        }
        free(pX);
    }
}
