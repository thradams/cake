#pragma safety enable

/* FIXED: `cond ? nullable : ""` resolves each arm's REF inside that arm, so the guard's narrowing survives the merge (tokenizer.c .path init) */

struct S
{
    const char* path;  /* non-_Opt: must never be null */
    int col;
};

void g(const char* s);

/* Designated initializer -- the tokenizer.c shape. */
void in_initializer(const char* _Opt filename_opt)
{
    struct S s = { .col = 1, .path = filename_opt ? filename_opt : "" };
    g(s.path);
}

/* Directly as an argument. */
void in_argument(const char* _Opt filename_opt)
{
    g(filename_opt ? filename_opt : "");
}

/* Through a local. */
void in_assignment(const char* _Opt filename_opt)
{
    const char* p = filename_opt ? filename_opt : "";
    g(p);
}

/* control 1: the arm does not re-read the condition variable, never held a REF */
void arm_does_not_reread_ok(const char* _Opt filename_opt)
{
    const char* p = filename_opt ? "x" : "";
    g(p);
}

/* control 2: explicit if/else, the use never outlives the narrowing */
void spelled_out_ok(const char* _Opt filename_opt)
{
    const char* p;
    if (filename_opt)
        p = filename_opt;
    else
        p = "";

    g(p);
}
