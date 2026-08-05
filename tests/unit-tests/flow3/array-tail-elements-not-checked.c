#pragma safety enable

/*
   TEMPORARY dismissal, recorded as a test so its scope is pinned.

   A fixed array used only up to a companion count is the standard C idiom:

       struct label_state labels[100];   // flow3.h, real code
       int labels_size;                  // only [0, labels_size) is live

   Zero-initializing the containing struct makes every element's non-_Opt
   pointer members null, so passing the container to a parameter whose members
   are non-nullable reported one warning per element per member -- 200 of them
   for a single argument at parser.c:12365, every one technically true and none
   actionable, because the tail elements are never read.

   Deciding this properly means relating an array to the count that bounds its
   live prefix, which flow3 does not model. Until it does,
   flow3_check_object_access carries in_array_element and does not report
   "possible null pointer ... to non-nullable pointer parameter" for anything
   reached by walking into an array's elements.

   What this deliberately does NOT disable is below: a zeroed non-_Opt member
   of a plain struct still warns (zero-is-not-ok.c covers that ground in more
   detail), and so does a directly subscripted element, which reaches the check
   through the subscript path rather than through an aggregate walk.
*/

struct label_state
{
    const char* label_name;      /* non-_Opt: null is a contract violation */
    struct label_state* next;    /* non-_Opt */
};

struct ctx
{
    struct label_state labels[100];
    int labels_size;
};

struct plain
{
    const char* name;            /* non-_Opt */
};

void take_ctx(struct ctx* p);
void take_plain(struct plain p);
void take_name(const char* s);

/* DISMISSED: 100 elements x 2 non-_Opt members would be 200 warnings, all
   about tail entries that labels_size keeps out of reach. */
void passes_whole_array(void)
{
    struct ctx c = { 0 };
    take_ctx(&c);
}

/* NOT dismissed: a zeroed non-_Opt member of a plain struct. */
void passes_plain_struct(void)
{
    struct plain p = { 0 };
    take_plain(p); //lint 35 passing a possible null pointer '.name' to non-nullable pointer parameter
}

/* NOT dismissed: a directly subscripted element is checked through the
   subscript path, not the aggregate walk. */
void passes_one_element(void)
{
    struct ctx c = { 0 };
    take_name(c.labels[0].label_name); //lint 35 passing a possible null pointer to non-nullable pointer parameter
}
