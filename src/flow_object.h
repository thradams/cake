
/*
   Object represents "memory" and state. Used by flow analysis
*/

#pragma once
#include "ownership.h"
#include "type.h"


struct flow_visit_ctx;

extern unsigned int s_visit_number; //creates a unique number


enum object_state
{
    /*
       Not applicable. The state cannot be used.
       struct...
       TODO we need empty state when object does not exist
       and the merge it is not a problem
       merge with nothing then
    */
    OBJECT_STATE_NOT_APPLICABLE = 0,

    OBJECT_STATE_UNINITIALIZED = 1 << 0,

    OBJECT_STATE_NULL = 1 << 1,
    OBJECT_STATE_NOT_NULL = 1 << 2,

    //means not-null moved at same time
    OBJECT_STATE_MOVED = 1 << 3,

    OBJECT_STATE_ZERO = 1 << 5,
    OBJECT_STATE_NOT_ZERO = 1 << 6,

    OBJECT_STATE_LIFE_TIME_ENDED = 1 << 7,
};


struct objects {
    struct flow_object* _Owner* _Owner data;
    int size;
    int capacity;
};

void objects_clear(struct objects* p);
void objects_destroy(struct objects* _Obj_owner p);
int objects_push_back(struct objects* p, struct flow_object* _Owner p_object);
const struct flow_object* objects_find(const struct objects* p, const struct flow_object* p_object);


struct objects_view {
    struct flow_object** _Owner _Opt data;
    int size;
    int capacity;
};

void objects_view_destroy(struct objects_view* _Obj_owner p);
int objects_view_push_back(struct objects_view* p, struct flow_object* p_object);
bool objects_view_find(const struct objects_view* p, const struct flow_object* p_object);
void objects_view_copy(struct objects_view* dest, const struct objects_view* source);
void objects_view_merge(struct objects_view* dest, const struct objects_view* source);
void objects_view_clear(struct objects_view* p);


struct flow_object_state {
    const char* dbg_name;
    int state_number;

    struct flow_object* pointed;
    enum object_state state;
    struct objects_view alternatives;
    struct flow_object_state* _Owner _Opt next;
};

void flow_object_state_copy(struct flow_object_state* to, const struct flow_object_state* from);
void flow_object_state_delete(struct flow_object_state* _Owner _Opt p);


/*
  Used in flow analysis to represent the object instance
*/
struct flow_object
{
    //used to avoid infinite recursion
    unsigned int visit_number;

    struct flow_object* _Opt parent;

    /*object are the result of expressions or they are declarators*/
    const struct declarator* _Opt p_declarator_origin;
    const struct expression* _Opt p_expression_origin;

    struct objects_view members;

    struct flow_object_state current;

    int id; //helps debugging
    bool is_temporary;
};

void flow_object_set_is_moved(struct flow_object* p_object);
void flow_object_set_can_be_uninitialized(struct flow_object* p_object);
void flow_object_set_is_unitialized(struct flow_object* p_object);
void flow_object_update_current(struct flow_object* p);
void flow_object_set_current_state_to_can_be_null(struct flow_object* p);
void flow_object_set_current_state_to_is_null(struct flow_object* p);

int flow_object_add_state(struct flow_object* p, struct flow_object_state* _Owner pnew);

bool flow_object_is_zero_or_null(const struct flow_object* p_object);

bool flow_object_is_not_null(struct flow_object* p);
bool flow_object_can_be_not_null_or_moved(struct flow_object* p);

bool flow_object_is_null(struct flow_object* p);
bool flow_object_can_be_null(struct flow_object* p);
bool flow_object_can_be_zero(const struct flow_object* p);



bool flow_object_is_not_zero(struct flow_object* p);
bool flow_object_is_zero(struct flow_object* p);

bool flow_object_is_uninitialized(struct flow_object* p);
bool flow_object_can_be_uninitialized(struct flow_object* p);

bool flow_object_can_have_its_lifetime_ended(struct flow_object* p);

void flow_object_print_state(struct flow_object* p);

void object_set_pointer(struct flow_object* p_object, struct flow_object* p_object2);

void object_destroy(struct flow_object* _Obj_owner p);
void object_delete(struct flow_object* _Owner _Opt p);
void object_swap(struct flow_object* a, struct flow_object* b);
void print_object_line(struct flow_object* p_object, int cols);
void print_object_state_to_str(enum object_state e, char str[], int sz);

struct declarator;
struct flow_object* _Opt make_object(struct flow_visit_ctx* ctx,
                                     struct type* p_type,
                                     const struct declarator* _Opt p_declarator_opt,
                                     const struct expression* _Opt p_expression_origin);

void flow_object_add_new_state_as_a_copy_of_current_state(struct flow_object* object, const char* name, int state_number);
struct token* object_get_token(const struct flow_object* object);
void object_remove_state(struct flow_object* object, int state_number);


int object_restore_current_state_from(struct flow_object* object, int state_number);

void object_merge_state(struct flow_object* pdest, struct flow_object* object1, struct flow_object* object2);


struct flow_visit_ctx;
struct token;


void print_object(struct type* p_type, struct flow_object* p_object, bool short_version);

void flow_assignment(struct flow_visit_ctx* ctx,
    const struct token* error_position,
    enum  assigment_type assigment_type,
    bool check_uninitialized_b,
    bool a_type_is_view,
    bool a_type_is_nullable,
    struct type* p_a_type, struct flow_object* p_a_object,
    struct type* p_b_type, struct flow_object* p_b_object);


void object_set_zero(struct type* p_type, struct flow_object* p_object);
void object_set_uninitialized(struct type* p_type, struct flow_object* p_object);
void object_set_moved(struct type* p_type, struct flow_object* p_object);

void object_set_unknown(struct type* p_type, bool t_is_nullable, struct flow_object* p_object, bool nullable_enabled);


void checked_read_object(struct flow_visit_ctx* ctx,
    struct type* p_type,
    bool is_nullable,
    struct flow_object* p_object,
    const struct token* position_token,
    bool check_pointed_object);


void end_of_storage_visit(struct flow_visit_ctx* ctx,
    struct type* p_type,
    bool type_is_view,
    struct flow_object* p_object,
    const struct token* position_token,
    const char* previous_names);


bool object_is_expansible(const struct flow_object* p_object);
void expand_pointer_object(struct flow_visit_ctx* ctx, struct type* p_type, struct flow_object* p_object);
void object_push_states_from(const struct flow_object* p_object_from, struct flow_object* p_object_to);

struct flow_object* expression_get_object(struct flow_visit_ctx* ctx, struct expression* p_expression, bool nullable_enabled);
