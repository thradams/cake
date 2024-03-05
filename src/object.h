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

struct object_state_stack
{
    enum object_state* owner data;
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
    struct object* owner pointed;

    /*declarator is used only to print the error message*/
    const struct declarator* declarator;

    struct objects members;
    struct object_state_stack object_state_stack;
};
void object_destroy(struct object* obj_owner p);
void object_delete(struct object* owner opt p);
void object_swap(struct object* a, struct object* b);

struct declarator;
struct object make_object(struct type* p_type, const struct declarator* declarator);

void object_push_copy_current_state(struct object* object);

void object_pop_states(struct object* object, int n);

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

void object_assignment(struct parser_ctx* ctx,
    struct object* p_source_obj_opt,
    struct type* p_source_obj_type,

    struct object* p_dest_obj_opt,
    struct type* p_dest_obj_type,

    const struct token* error_position,
    bool bool_source_zero_value,
    enum object_state source_state_after);

void object_set_unknown(struct type* p_type, struct object* p_object);
void object_set_zero(struct type* p_type, struct object* p_object);

void checked_read_object(struct parser_ctx* ctx,
    struct type* p_type,
    struct object* p_object,
    const struct token* position_token,
    bool check_pointed_object);
