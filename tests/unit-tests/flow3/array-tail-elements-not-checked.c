#pragma safety enable

/* TEMPORARY: null non-_Opt members reached by walking an array's elements are not reported (labels[100] + labels_size idiom) */

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

/* dismissed: would be 200 warnings about tail entries labels_size keeps out of reach */
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

/* not dismissed: a directly subscripted element goes through the subscript path */
void passes_one_element(void)
{
    struct ctx c = { 0 };
    take_name(c.labels[0].label_name); //lint 35 passing a possible null pointer to non-nullable pointer parameter
}
