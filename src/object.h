/*
   Object represents "memory" and state. Used by flow analysis
*/

#pragma once
#include "ownership.h"
#include "type.h"

enum object_state
{
    /*
       Not used
    */
    OBJECT_STATE_EMPTY = 0,

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
void object_delete(struct object* owner p);
void object_swap(struct object* a, struct object* b);

struct declarator;
struct object make_object(struct type* p_type, const struct declarator* declarator);
void visit_object(struct parser_ctx* ctx,
    struct type* p_type,
    struct object* p_object,
    const struct token* position_token,
    const char* previous_names,
    bool is_assigment);
