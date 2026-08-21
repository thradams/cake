#pragma safety enable

/*
   Overwriting a struct that holds several _Owner members must report ONCE.

   flow3_check_discarding_owner_before_overwrite walked the destination's leaves
   and reported from inside the walk, so one assignment said the same thing once
   per owner leaf. cake's own parser.c:3230,

       p_init_declarator->p_declarator->type = make_type_using_declarator(...);

   reported .type.name_opt, .type.next and .type.params.head separately -- three
   diagnostics for one write that discards one `struct type`.

   It also reported once per ALTERNATIVE, with no report-once guard, so an
   object that had accumulated several alternatives repeated each message again.
   Alternatives differ only in provenance; the extra copies carry nothing the
   first one lacks. This is the same rule flow3_check_object_access and
   flow3_check_object_init_assigment already applied.

   Both are fixed by collecting first and reporting after: the walk now fills a
   flow3_discarded_owner_scan (count, first name, line) and the entry point
   emits a single diagnostic.

   The COUNT decides the name, which is the part worth pinning:

     - exactly one leaf  -> name the leaf. `x->data = p;` must still say
       `.data`; blaming the enclosing struct would be less precise, not more.
     - several leaves    -> name the object the assignment targets, since one
       write discards them all together.

   A top-level destination (a whole object rather than a member) carries an
   EMPTY designator, not a null one -- `*obj = *p;` at object.c:1632 is that
   shape -- so the fallback tests for content and uses the first leaf's name
   instead of printing ''.
*/

#define NULL ((void*)0)

struct blob
{
    char* _Owner _Opt data;
    int len;
};

/* Several owner members, so an overwrite discards more than one at a time. */
struct holder
{
    char* _Owner _Opt first;
    char* _Owner _Opt second;
    char* _Owner _Opt third;
    int tag;
};

struct outer
{
    struct holder inner;
    int tag;
};

struct holder make_holder(void);
char* _Owner _Opt dup(const char* s);
void del_str(char* _Owner _Opt s);

/*
   Several owner leaves discarded by one write, and the destination IS a member,
   so it has a designator to report: one diagnostic naming `.inner` rather than
   three naming .inner.first, .inner.second and .inner.third. This is
   parser.c:3230's shape, which collapsed from three reports to one `.type`.
*/
void whole_member_reports_once(struct outer* o)
{
    o->inner = make_holder(); //lint 26 one report naming .inner, not one per owner member
}

/*
   The same overwrite where the destination is a whole object rather than a
   member: its designator is EMPTY, so the report falls back to the first leaf's
   name instead of printing ''. Still one diagnostic, not three. This is
   object.c:1632's shape (`*obj = *p;`).
*/
void whole_object_falls_back_to_leaf_name(struct holder* h)
{
    *h = make_holder(); //lint 26 empty designator: names the first leaf, still reported once
}

/*
   Control: a single owner leaf still names that leaf. This is the distinction
   the count is there to preserve -- collapsing this to the struct would lose
   precision rather than gain brevity.
*/
void single_member_names_the_member(struct blob* b)
{
    b->data = dup("x"); //lint 26 names .data, not the enclosing struct
}

/*
   Control: nothing to discard. The members are provably null, so the overwrite
   releases nothing and must stay silent -- the collapse must not turn "no
   findings" into a report.
*/
void nothing_live_is_silent(void)
{
    struct holder h = { 0 };

    h.first = dup("a");
    h.second = dup("b");
    h.third = dup("c");

    del_str(h.first);
    del_str(h.second);
    del_str(h.third);
}
