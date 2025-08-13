/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/
 
#pragma once
#include "parser.h"

struct flow_visit_ctx;

extern unsigned int s_visit_number; //creates a unique number


enum flow_state
{
    /*
       Not applicable. The state cannot be used.
       struct...
       TODO we need empty state when object does not exist
       and the merge it is not a problem
       merge with nothing then
    */
    FLOW_OBJECT_STATE_NOT_APPLICABLE = 0,

    FLOW_OBJECT_STATE_UNINITIALIZED = 1 << 0,


    /*
      The only reason we have null and zero is because
      of non pointer references -1 for instance can be the "null"
    */
    FLOW_OBJECT_STATE_NULL = 1 << 1,
    FLOW_OBJECT_STATE_NOT_NULL = 1 << 2,

    //means not-null moved at same time
    FLOW_OBJECT_STATE_MOVED = 1 << 3,

    FLOW_OBJECT_STATE_ZERO = 1 << 5,
    FLOW_OBJECT_STATE_NOT_ZERO = 1 << 6,

    FLOW_OBJECT_STATE_LIFE_TIME_ENDED = 1 << 7,
    
    FLOW_OBJECT_STATE_DANGLING = 1 << 8
};


struct flow_objects
{
    struct flow_object* _Owner* _Owner _Opt data;
    int size;
    int capacity;
};

void flow_objects_clear(struct flow_objects* p);
void flow_objects_destroy(_Dtor struct flow_objects* p);
int flow_objects_push_back(struct flow_objects* p, struct flow_object* _Owner p_object);
const struct flow_object* _Opt flow_objects_find(const struct flow_objects* p, const struct flow_object* p_object);


struct flow_objects_view 
{
    struct flow_object** _Owner _Opt data;
    int size;
    int capacity;
};

void objects_view_destroy(_Dtor struct flow_objects_view* p);
int objects_view_push_back(struct flow_objects_view* p, struct flow_object* p_object);
bool objects_view_find(const struct flow_objects_view* p, const struct flow_object* p_object);
void objects_view_copy(struct flow_objects_view* dest, const struct flow_objects_view* source);
void objects_view_merge(struct flow_objects_view* dest, const struct flow_objects_view* source);
void objects_view_clear(struct flow_objects_view* p);


struct flow_object_state
{
    const char* dbg_name;
    int state_number;

    struct flow_object* _Opt pointed;
    enum flow_state state;
    struct flow_objects_view alternatives;
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

    struct flow_objects_view members;

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

bool flow_object_is_not_null(const struct flow_object* p);
bool flow_object_can_be_not_null_or_moved(const struct flow_object* p);

bool flow_object_is_null(const struct flow_object* p);
bool flow_object_can_be_null(const struct flow_object* p);
bool flow_object_can_be_moved(const struct flow_object* p);
bool flow_object_can_be_zero(const struct flow_object* p);



bool flow_object_is_not_zero(const struct flow_object* p);
bool flow_object_is_zero(const struct flow_object* p);

bool flow_object_is_uninitialized(const struct flow_object* p);
bool flow_object_can_be_uninitialized(const struct flow_object* p);

bool flow_object_can_have_its_lifetime_ended(const struct flow_object* p);

void flow_object_print_state(struct flow_object* p);

void object_set_pointer(struct flow_object* p_object, struct flow_object* p_object2);

void flow_object_destroy(_Dtor struct flow_object* p);
void flow_object_delete(struct flow_object* _Owner _Opt p);
void flow_object_swap(struct flow_object* a, struct flow_object* b);
void print_object_line(struct flow_object* p_object, int cols);
void print_object_state_to_str(enum flow_state e, char str[], int sz);

struct declarator;
struct flow_object* _Opt make_flow_object(struct flow_visit_ctx* ctx,
                                     struct type* p_type,
                                     const struct declarator* _Opt p_declarator_opt,
                                     const struct expression* _Opt p_expression_origin);

void flow_object_add_new_state_as_a_copy_of_current_state(struct flow_object* object, const char* name, int state_number);
struct token* _Opt flow_object_get_token(const struct flow_object* object);
void flow_object_remove_state(struct flow_object* object, int state_number);


int flow_object_restore_current_state_from(struct flow_object* object, int state_number);

void flow_object_merge_state(struct flow_object* pdest, struct flow_object* object1, struct flow_object* object2);


struct flow_visit_ctx;
struct token;


void print_flow_object(struct type* p_type, struct flow_object* p_object, bool short_version);

struct marker;

void flow_check_assignment(struct flow_visit_ctx* ctx,
    const struct token* error_position,
    const struct marker* p_a_marker,
    const struct marker* p_b_marker,
    enum  assigment_type assigment_type,
    bool check_uninitialized_b,
    bool a_type_is_view,
    bool a_type_is_nullable,
    struct type* p_a_type, struct flow_object* p_a_object,
    struct type* p_b_type, struct flow_object* p_b_object,
    bool * _Opt set_argument_to_unkown);

void flow_object_set_end_of_lifetime(struct type* p_type, struct flow_object* p_object);
void flow_object_set_zero(struct type* p_type, struct flow_object* p_object);
void flow_object_set_uninitialized(struct type* p_type, struct flow_object* p_object);
void flow_object_set_moved(struct type* p_type, struct flow_object* p_object);

void flow_object_set_unknown(struct type* p_type, bool t_is_nullable, struct flow_object* p_object, bool nullable_enabled);


void checked_read_object(struct flow_visit_ctx* ctx,
    struct type* p_type,
    bool is_nullable,
    struct flow_object* p_object,
    const struct token* _Opt position_token,
    const struct marker* _Opt p_marker,
    bool check_pointed_object);


void flow_end_of_block_visit(struct flow_visit_ctx* ctx,
    struct type* p_type,
    bool type_is_view,
    struct flow_object* p_object,
    const struct token* position_token,
    const char* previous_names);


bool flow_object_is_expansible(const struct flow_object* _Opt p_object);
void flow_object_expand_pointer(struct flow_visit_ctx* ctx, struct type* p_type, struct flow_object* p_object);
void flow_object_push_states_from(const struct flow_object* p_object_from, struct flow_object* p_object_to);

struct flow_object* _Opt expression_get_flow_object(struct flow_visit_ctx* ctx, struct expression* p_expression, bool nullable_enabled);


struct label_state
{
    const char * label_name;
    int state_number;
};

struct flow_visit_ctx
{
    struct secondary_block* _Opt catch_secondary_block_opt;

    struct parser_ctx *ctx;
    _View struct ast ast;    
 
    struct type* _Opt p_return_type;
    int parameter_list;
    
    int state_number_generator;
    bool expression_is_not_evaluated; //true when is expression for sizeof, missing state_set, typeof
    bool inside_assert;
    bool inside_contract;

    /*avoid messages like always something, because in loop the same expression is visited in diferent states*/
    bool inside_loop;

    int throw_join_state; /*state where throws are joined*/
    int break_join_state; /*state where breaks are joined*/
    int initial_state;    /*used to keep the original state*/

    struct flow_objects arena;

    struct label_state labels[100]; //max 100 labels in a function (case not included)
    int labels_size;
};

void flow_visit_ctx_destroy(_Dtor struct flow_visit_ctx* p);
void flow_start_visit_declaration(struct flow_visit_ctx* ctx, struct declaration* p_declaration);
