#pragma safety enable





/* overwriting a struct with several _Owner members reports ONCE: one leaf names the leaf, several leaves name the target object, an empty designator falls back to the first leaf */





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





/* several owner leaves, destination is a member: one diagnostic naming `.inner` (parser.c:3230 shape) */


void whole_member_reports_once(struct outer* o)


{


    o->inner = make_holder(); //lint 26 one report naming .inner, not one per owner member


}





/* destination is a whole object with an empty designator: one diagnostic naming the first leaf (object.c `*obj = *p;`) */


void whole_object_falls_back_to_leaf_name(struct holder* h)


{


    *h = make_holder(); //lint 26 empty designator: names the first leaf, still reported once


}





/* control: a single owner leaf still names that leaf */


void single_member_names_the_member(struct blob* b)


{


    b->data = dup("x"); //lint 26 names .data, not the enclosing struct


}





/* control: members provably null, nothing to discard, no report */


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


