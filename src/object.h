/*
   Object represents "memory" and state. Used by flow analysis
*/

#pragma once
#include "ownership.h"
#include "type.h"

enum object_state
{
    /*
       Not applicable. The state cannot be used.
    */
    OBJECT_STATE_NOT_APPLICABLE = 0,

    OBJECT_STATE_UNINITIALIZED = 1 << 0,
    /*
      non-pointer can be NULL and not ZERO.
      For pointer ZERO is set as NULL
    */
    OBJECT_STATE_NULL = 1 << 1,

    /*
       We have a reference
    */
    OBJECT_STATE_NOT_NULL = 1 << 2,

    /*
       object was moved
    */
    OBJECT_STATE_MOVED = 1 << 3,

    /*
       non-pointer with 0
    */
    OBJECT_STATE_ZERO = 1 << 4,

    /*
       non-pointer with != 0
    */
    OBJECT_STATE_NOT_ZERO = 1 << 5
};

void object_state_to_string(enum object_state e);

struct object_state_stack_item {
    const char* name;
    int state_number;
    enum object_state state;
};

struct object_state_stack
{
    struct object_state_stack_item* owner data;
    int size;
    int capacity;
};
void object_state_stack_destroy(struct object_state_stack* obj_owner p);

struct objects {
    struct object* owner data;
    int size;
    int capacity;
};

void objects_destroy(struct objects* obj_owner p);
/*
  Used in flow analysis to represent the object instance
*/
struct object
{
    /*
       state should not be used for struct, unless
       members_size is zero.
    */
    enum object_state state;
    struct object* owner pointed2;
    struct object* pointed_ref;

    /*declarator is used only to print the error message*/
    const struct declarator* declarator;

    const struct expression* p_expression_origin;
    struct objects members;
    struct object_state_stack object_state_stack;
};
void object_destroy(struct object* obj_owner p);
void object_delete(struct object* owner opt p);
void object_swap(struct object* a, struct object* b);
struct object* object_get_pointed_object(const struct object* p);
struct declarator;
struct object make_object(struct type* p_type,
    const struct declarator* p_declarator_opt,
    const struct expression* p_expression_origin);

void object_push_copy_current_state(struct object* object, const char* name, int state_number);
void object_push_empty(struct object* object, const char* name, int state_number);

void object_pop_states(struct object* object, int n);
int object_merge_current_state_with_state_number(struct object* object, int state_number);
int object_merge_current_state_with_state_number_or(struct object* object, int state_number);
int object_restore_current_state_from(struct object* object, int state_number);
int object_set_state_from_current(struct object* object, int state_number);

struct parser_ctx;
struct token;

void visit_object(struct parser_ctx* ctx,
    struct type* p_type,
    struct object* p_object,
    const struct token* position_token,
    const char* previous_names,
    bool is_assigment);

void object_restore_state(struct object* object, int state_to_restore);

void print_object_core(int ident, struct type* p_type, struct object* p_object, const char* previous_names, bool is_pointer, bool short_version);

void print_object(struct type* p_type, struct object* p_object, bool short_version);
void set_direct_state(
    struct type* p_type,
    struct object* p_object,
    enum object_state flags);
void set_object(
    struct type* p_type,
    struct object* p_object,
    enum object_state flags);

enum assigment_type
{
    ASSIGMENT_TYPE_RETURN,
    ASSIGMENT_TYPE_PARAMETER,
    ASSIGMENT_TYPE_OBJECTS,
};

void object_assignment(struct parser_ctx* ctx,
    struct object* p_source_obj_opt,
    struct type* p_source_obj_type,

    struct object* p_dest_obj_opt,
    struct type* p_dest_obj_type,

    const struct token* error_position,
    bool bool_source_zero_value,
    enum object_state source_state_after,
    enum assigment_type assigment_type);



void object_assignment3(struct parser_ctx* ctx,
    const struct token* error_position,
    enum  assigment_type assigment_type,
    bool check_uninitialized_b,
    struct type* p_a_type, struct object* p_a_object,
    struct type* p_b_type, struct object* p_b_object);

void object_set_unknown(struct type* p_type, struct object* p_object);
void object_set_zero(struct type* p_type, struct object* p_object);
void object_set_uninitialized(struct type* p_type, struct object* p_object);
void object_set_nothing(struct type* p_type, struct object* p_object);
void object_set_moved(struct type* p_type, struct object* p_object);

void checked_read_object(struct parser_ctx* ctx,
    struct type* p_type,
    struct object* p_object,
    const struct token* position_token,
    bool check_pointed_object);

bool object_is_zero_or_null(const struct object* p_object);
