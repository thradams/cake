#include "ownership.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include "flow_visit.h"
#include "expressions.h"
#include "ownership.h"
#include <ctype.h>

/*
              NULL
                |
            [function] (NULL <- child0 <- last_child)
                |
              [try]  (NULL <- child0 <- last_child)
                |
tail_block-> [while] (NULL <- child0 <- last_child)

*/

struct flow_defer_scope
{

    //things must called at end of scope
    struct declarator* declarator; // declarator 
    struct defer_statement* defer_statement; // defer 

    //statements for controling where jump like break, throw stop.

    struct try_statement* p_try_statement; //try
    struct selection_statement* p_selection_statement; //if switch
    struct iteration_statement* p_iteration_statement; //for do while
    struct statement* p_statement;//
    struct compound_statement* p_compound_statement;
    struct compound_statement* p_function_body;
    struct secondary_block* p_secondary_block;
    struct primary_block* p_primary_block;

    struct flow_defer_scope* owner last_child; /**/
    struct flow_defer_scope* owner previous;
};

void flow_visit_declaration(struct flow_visit_ctx* ctx, struct declaration* p_declaration);
static void flow_visit_attribute_specifier_sequence(struct flow_visit_ctx* ctx, struct attribute_specifier_sequence* p_visit_attribute_specifier_sequence);

static void flow_visit_secondary_block(struct flow_visit_ctx* ctx, struct secondary_block* p_secondary_block);
static void flow_visit_struct_or_union_specifier(struct flow_visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier);
static void flow_visit_expression(struct flow_visit_ctx* ctx, struct expression* p_expression);
static void flow_visit_statement(struct flow_visit_ctx* ctx, struct statement* p_statement);
static void flow_visit_enum_specifier(struct flow_visit_ctx* ctx, struct enum_specifier* p_enum_specifier);
static void flow_visit_type_specifier(struct flow_visit_ctx* ctx, struct type_specifier* p_type_specifier);

void object_swap(struct object* a, struct object* b)
{
    struct object temp = *a;
    *a = *b;
    *b = temp;
}

void object_delete(struct object* owner p)
{
    if (p)
    {
        object_destroy(p);
        free(p);
    }
}

void object_destroy(struct object* obj_owner p)
{
    object_delete(p->pointed);
    objects_destroy(&p->members);
    object_state_stack_destroy(&p->object_state_stack);
}


void object_state_stack_destroy(struct object_state_stack* obj_owner p)
{
    free(p->data);
}

int object_state_stack_reserve(struct object_state_stack* p, int n) unchecked
{
    if (n > p->capacity)
    {
        if ((size_t) n > (SIZE_MAX / (sizeof(p->data[0]))))
        {
            return EOVERFLOW;
        }

        void* owner pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL) return ENOMEM;

        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

int object_state_stack_push_back(struct object_state_stack* p, enum object_state e)
{
    if (p->size == INT_MAX)
    {
        return EOVERFLOW;
    }

    if (p->size + 1 > p->capacity)
    {
        int new_capacity = 0;
        if (p->capacity > (INT_MAX - p->capacity / 2))
        {
            /*overflow*/
            new_capacity = INT_MAX;
        }
        else
        {
            new_capacity = p->capacity + p->capacity / 2;
            if (new_capacity < p->size + 1)
            {
                new_capacity = p->size + 1;
            }
        }

        int error = object_state_stack_reserve(p, new_capacity);
        if (error != 0)
        {
            return error;
        }
    }

    p->data[p->size] = e;
    p->size++;

    return 0;
}

void objects_destroy(struct objects* obj_owner p) unchecked
{
    for (int i = 0; i < p->size; i++)
    {
        object_destroy(&p->data[i]);
    }
    free(p->data);
}

int objects_reserve(struct objects* p, int n) unchecked
{
    if (n > p->capacity)
    {
        if ((size_t) n > (SIZE_MAX / (sizeof(p->data[0]))))
        {
            return EOVERFLOW;
        }

        void* owner pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL) return ENOMEM;

        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

int objects_push_back(struct objects* p, struct object* obj_owner p_object)
{
    if (p->size == INT_MAX)
    {
        object_destroy(p_object);
        return EOVERFLOW;
    }

    if (p->size + 1 > p->capacity)
    {
        int new_capacity = 0;
        if (p->capacity > (INT_MAX - p->capacity / 2))
        {
            /*overflow*/
            new_capacity = INT_MAX;
        }
        else
        {
            new_capacity = p->capacity + p->capacity / 2;
            if (new_capacity < p->size + 1)
            {
                new_capacity = p->size + 1;
            }
        }

        int error = objects_reserve(p, new_capacity);
        if (error != 0)
        {
            object_destroy(p_object);
            return error;
        }
    }

    p->data[p->size] = *p_object; /*COPIED*/


    p->size++;

    return 0;
}
struct object_name_list
{
    const char* name;
    struct object_name_list* previous;
};

static bool has_name(const char* name, struct object_name_list* list)
{
    struct object_name_list* p = list;

    while (p)
    {
        if (strcmp(p->name, name) == 0)
        {
            return true;
        }
        p = p->previous;
    }
    return false;
}

static struct object make_object_core(struct type* p_type, struct object_name_list* list, int deep, const struct declarator* declarator)
{
    struct object obj = {0};
    obj.declarator = declarator;

    if (p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier* p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier)
        {
            obj.state = OBJECT_STATE_EMPTY;

            struct member_declaration* p_member_declaration =
                p_struct_or_union_specifier->member_declaration_list.head;

            struct object_name_list l = {0};
            l.name = p_struct_or_union_specifier->tag_name;
            l.previous = list;
            //int member_index = 0;
            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator* p_member_declarator =
                        p_member_declaration->member_declarator_list_opt->head;

                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            char* tag = NULL;
                            if (p_member_declarator->declarator->type.struct_or_union_specifier)
                            {
                                tag = p_member_declarator->declarator->type.struct_or_union_specifier->tag_name;
                            }
                            else if (p_member_declarator->declarator->type.next &&
                                p_member_declarator->declarator->type.next->struct_or_union_specifier)
                            {
                                tag = p_member_declarator->declarator->type.next->struct_or_union_specifier->tag_name;

                            }

                            if (tag && has_name(tag, &l))
                            {
                                struct object member_obj = {0};
                                member_obj.declarator = declarator;
                                member_obj.state = OBJECT_STATE_EMPTY;
                                objects_push_back(&obj.members, &member_obj);
                            }
                            else
                            {
                                struct object member_obj = make_object_core(&p_member_declarator->declarator->type, &l, deep, declarator);
                                objects_push_back(&obj.members, &member_obj);
                            }

                            //member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else
                {
                    if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                    {
                        //struct object obj = {0};
                        //obj.state = OBJECT_STATE_STRUCT;
                        //objects_push_back(&obj.members, &obj);


                        struct type t = {0};
                        t.category = TYPE_CATEGORY_ITSELF;
                        t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                        t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;
                        struct object member_obj = make_object_core(&t, &l, deep, declarator);
                        objects_push_back(&obj.members, &member_obj);
                        type_destroy(&t);
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
        }
    }


    else if (type_is_array(p_type))
    {
        //p_object->state = flags;
        //if (p_object->members_size > 0)
        //{
        //    //not sure if we instanticate all items of array
        //    p_object->members[0].state = flags;
        //}
    }
    else if (type_is_pointer(p_type))
    {
        obj.state = OBJECT_STATE_EMPTY;

        if (deep < 1)
        {
            struct type t2 = type_remove_pointer(p_type);
            if (type_is_struct_or_union(&t2))
            {
                struct object* owner p_object = calloc(1, sizeof(struct object));
                *p_object = make_object_core(&t2, list, deep + 1, declarator);
                obj.pointed = p_object;
            }

            type_destroy(&t2);
            //(*p_deep)++;
        }
    }
    else
    {
        //assert(p_object->members_size == 0);
        //p_object->state = flags;
        obj.state = OBJECT_STATE_EMPTY;
    }

    return obj;
}

static struct object make_object(struct type* p_type, const struct declarator* declarator)
{
    assert(declarator);
    struct object_name_list list = {.name = ""};
    return make_object_core(p_type, &list, 0, declarator);
}

static void object_push_copy_current_state(struct object* object)
{

    object_state_stack_push_back(&object->object_state_stack, object->state);

    if (object->pointed)
    {
        object_push_copy_current_state(object->pointed);
    }

    for (int i = 0; i < object->members.size; i++)
    {
        object_push_copy_current_state(&object->members.data[i]);
    }

}



static void object_pop_states(struct object* object, int n)
{

    if (object->object_state_stack.size < n)
    {
        //assert(false);
        return;
    }

    object->object_state_stack.size =
        object->object_state_stack.size - n;

    if (object->pointed)
    {
        object_pop_states(object->pointed, n);
    }

    for (int i = 0; i < object->members.size; i++)
    {
        object_pop_states(&object->members.data[i], n);
    }

}


static void object_restore_state(struct object* object, int state_to_restore)
{
    assert(state_to_restore > 0);

    //0 zero is top of stack
    //1 is the before top
    int index = object->object_state_stack.size - state_to_restore;
    if (index >= 0 && index < object->object_state_stack.size)
    {
    }
    else
    {
        //assert(false);
        return;
    }

    enum object_state sstate = object->object_state_stack.data[index];
    object->state = sstate;

    if (object->pointed)
    {
        object_restore_state(object->pointed, state_to_restore);
    }

    for (int i = 0; i < object->members.size; i++)
    {
        object_restore_state(&object->members.data[i], state_to_restore);
    }
}

enum object_state state_merge(enum object_state before, enum object_state after)
{
    enum object_state e = before | after;


    return e;
}



static void print_object_core(int ident, struct type* p_type, struct object* p_object, const char* previous_names, bool is_pointer, bool short_version)
{
    if (p_object == NULL)
    {
        return;
    }

    if (p_type->struct_or_union_specifier && p_object->members.size > 0)
    {
        struct struct_or_union_specifier* p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier)
        {
            if (p_object == NULL)
            {
                printf("%*c", ident, ' ');
                printf("%s %s\n", previous_names, "-");
                return;
            }
            //obj.state = OBJECT_STATE_STRUCT;

            struct member_declaration* p_member_declaration =
                p_struct_or_union_specifier->member_declaration_list.head;

            int member_index = 0;
            while (p_member_declaration)
            {


                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator* p_member_declarator =
                        p_member_declaration->member_declarator_list_opt->head;
                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            const char* name = p_member_declarator->declarator->name ? p_member_declarator->declarator->name->lexeme : "";

                            char buffer[200] = {0};
                            if (is_pointer)
                                snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                            else
                                snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);


                            print_object_core(ident + 1, &p_member_declarator->declarator->type,
                                &p_object->members.data[member_index], buffer,
                                type_is_pointer(&p_member_declarator->declarator->type), short_version);

                            member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else
                {
                    //char buffer[200] = {0};
                    //if (is_pointer)
                      //  snprintf(buffer, sizeof buffer, "%s", previous_names, "");
                    //else
                      //  snprintf(buffer, sizeof buffer, "%s", previous_names, "");

                    struct type t = {0};
                    t.category = TYPE_CATEGORY_ITSELF;
                    t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                    t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;

                    print_object_core(ident + 1, &t, &p_object->members.data[member_index], previous_names, false, short_version);

                    member_index++;
                    type_destroy(&t);
                }
                p_member_declaration = p_member_declaration->next;
            }

        }
    }
    else if (type_is_array(p_type))
    {
        //p_object->state = flags;
        //if (p_object->members_size > 0)
        //{
        //    //not sure if we instanticate all items of array
        //    p_object->members[0].state = flags;
        //}
    }
    else if (type_is_pointer(p_type))
    {
        struct type t2 = type_remove_pointer(p_type);
        printf("%*c", ident, ' ');
        if (p_object)
        {
            if (short_version)
            {
                printf("%s == ", previous_names);
                object_state_to_string(p_object->state);
            }
            else
            {
                printf("%p:%s == ", p_object, previous_names);
                printf("{");
                for (int i = 0; i < p_object->object_state_stack.size; i++)
                {
                    object_state_to_string(p_object->object_state_stack.data[i]);
                    printf(",");
                }
                printf("*");
                object_state_to_string(p_object->state);
                printf("}");
            }
            printf("\n");


            if (p_object->pointed)
            {
                char buffer[200] = {0};
                if (type_is_struct_or_union(&t2))
                {
                    snprintf(buffer, sizeof buffer, "%s", previous_names);
                }
                else
                {
                    snprintf(buffer, sizeof buffer, "*%s", previous_names);
                }



                print_object_core(ident + 1, &t2, p_object->pointed, buffer, is_pointer, short_version);
            }
            else
            {
                //printf("%s %s\n");
            }
        }
        type_destroy(&t2);
    }
    else
    {
        printf("%*c", ident, ' ');
        if (p_object)
        {
            if (short_version)
            {
                printf("%s == ", previous_names);
                object_state_to_string(p_object->state);
            }
            else
            {
                printf("%p:%s == ", p_object, previous_names);
                printf("{");
                for (int i = 0; i < p_object->object_state_stack.size; i++)
                {
                    object_state_to_string(p_object->object_state_stack.data[i]);
                    printf(",");
                }
                object_state_to_string(p_object->state);
                printf("}");
            }


            printf("\n");
        }
    }


}
void object_get_name(const struct type* p_type,
    const struct object* p_object,
    char* out,
    int out_size);


static void print_object(struct type* p_type, struct object* p_object, bool short_version)
{
    if (p_object == NULL)
    {
        printf("null object");
        return;
    }
    char name[100] = {0};
    object_get_name(p_type, p_object, name, sizeof name);



    print_object_core(0, p_type, p_object, name, type_is_pointer(p_type), short_version);
}

static void set_object(
    struct type* p_type,
    struct object* p_object,
    enum object_state flags);

static void set_object_state(
    struct parser_ctx* ctx,
    struct type* p_type,
    struct object* p_object,
    const struct type* p_source_type,
    const struct object* p_object_source,
    const struct token* error_position)
{
    if (p_object_source == NULL)
    {
        return;
    }
    if (p_object == NULL || p_type == NULL)
    {
        return;
    }


    if (p_type->struct_or_union_specifier && p_object->members.size > 0)
    {
        struct struct_or_union_specifier* p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier)
        {
            struct member_declaration* p_member_declaration =
                p_struct_or_union_specifier->member_declaration_list.head;

            int member_index = 0;
            while (p_member_declaration)
            {

                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator* p_member_declarator =
                        p_member_declaration->member_declarator_list_opt->head;

                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            if (member_index < p_object->members.size)
                            {
                                set_object_state(ctx,
                                    &p_member_declarator->declarator->type,
                                    &p_object->members.data[member_index],
                                    &p_object_source->members.data[member_index].declarator->type,
                                    &p_object_source->members.data[member_index],
                                    error_position);
                            }
                            else
                            {
                                //TODO BUG union?                                
                            }
                            member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
        }
        else
        {
            assert(p_object->members.size == 0);
            p_object->state = p_object_source->state;
        }
    }
    else if (type_is_array(p_type))
    {
        p_object->state = p_object_source->state;
        if (p_object->members.size > 0)
        {
            //not sure if we instantiate all items of array
            p_object->members.data[0].state = p_object_source->members.data[0].state;
        }
    }
    else if (type_is_pointer(p_type))
    {
        if (p_object_source)
        {
            if (p_object_source->state == OBJECT_STATE_UNINITIALIZED)
            {
                char buffer[100] = {0};
                object_get_name(p_source_type, p_object_source, buffer, sizeof buffer);
                compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
                    ctx,
                    error_position,
                    "source object '%s' is uninitialized", buffer);
            }
            else if (p_object_source->state & OBJECT_STATE_UNINITIALIZED)
            {
                char buffer[100] = {0};
                object_get_name(p_source_type, p_object_source, buffer, sizeof buffer);

                compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
                    ctx,
                    error_position,
                    "source object '%s' may be uninitialized", buffer);
            }

            if (type_is_any_owner(p_type) &&
                type_is_any_owner(p_source_type))
            {
                if (p_object_source->state == OBJECT_STATE_MOVED)
                {
                    char buffer[100] = {0};
                    object_get_name(p_source_type, p_object_source, buffer, sizeof buffer);

                    compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
                        ctx,
                        error_position,
                        "source object '%s' have been moved", buffer);
                }
                else if (p_object_source->state & OBJECT_STATE_MOVED)
                {
                    char buffer[100] = {0};
                    object_get_name(p_source_type, p_object_source, buffer, sizeof buffer);

                    compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
                        ctx,
                        error_position,
                        "source object '%s' may have been moved", buffer);
                }
            }

        }


        if (type_is_any_owner(p_type))
        {
            p_object->state = p_object_source->state;
        }
        else
        {
            //MOVED state is not applicable to non owner objects
            p_object->state = p_object_source->state & ~OBJECT_STATE_MOVED;
        }


        if (p_object->pointed)
        {
            struct type t2 = type_remove_pointer(p_type);
            if (p_object_source->pointed)
            {
                set_object_state(ctx, &t2, p_object->pointed, p_source_type, p_object_source->pointed, error_position);
            }
            else
            {
                set_object(&t2, p_object->pointed, OBJECT_STATE_NULL | OBJECT_STATE_NOT_NULL);
            }
            type_destroy(&t2);
        }
    }
    else
    {


        //assert(p_object->members.size == 0); //enum?
        p_object->state = p_object_source->state;
    }
}


static void set_direct_state(
    struct type* p_type,
    struct object* p_object,
    enum object_state flags)
{
    if (p_object == NULL || p_type == NULL)
    {
        return;
    }

    if (p_type->struct_or_union_specifier && p_object->members.size > 0)
    {
        struct struct_or_union_specifier* p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier)
        {
            struct member_declaration* p_member_declaration =
                p_struct_or_union_specifier->member_declaration_list.head;

            int member_index = 0;
            while (p_member_declaration)
            {

                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator* p_member_declarator =
                        p_member_declaration->member_declarator_list_opt->head;

                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            if (member_index < p_object->members.size)
                            {
                                set_direct_state(&p_member_declarator->declarator->type, &p_object->members.data[member_index], flags);
                            }
                            else
                            {
                                //TODO BUG union?                                
                            }
                            member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
        }
        else
        {
            assert(p_object->members.size == 0);
            p_object->state = flags;
        }
    }

    if (type_is_pointer(p_type))
    {
        if (flags == OBJECT_STATE_ZERO)
        {
            /*zero for pointers is null*/
            p_object->state = OBJECT_STATE_NULL;
        }
        else
        {
            p_object->state = flags;
        }
    }
    else
    {
        p_object->state = flags;
    }
}

static void set_object(
    struct type* p_type,
    struct object* p_object,
    enum object_state flags)
{
    if (p_object == NULL || p_type == NULL)
    {
        return;
    }


    if (p_type->struct_or_union_specifier && p_object->members.size > 0)
    {
        struct struct_or_union_specifier* p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier)
        {
            struct member_declaration* p_member_declaration =
                p_struct_or_union_specifier->member_declaration_list.head;

            int member_index = 0;
            while (p_member_declaration)
            {

                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator* p_member_declarator =
                        p_member_declaration->member_declarator_list_opt->head;

                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            if (member_index < p_object->members.size)
                            {
                                set_object(&p_member_declarator->declarator->type, &p_object->members.data[member_index], flags);
                            }
                            else
                            {
                                //TODO BUG union?                                
                            }
                            member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
        }
        else
        {
            assert(p_object->members.size == 0);
            p_object->state = flags;
        }
    }
    else if (type_is_array(p_type))
    {
        p_object->state = flags;
        if (p_object->members.size > 0)
        {
            //not sure if we instantiate all items of array
            p_object->members.data[0].state = flags;
        }
    }
    else if (type_is_pointer(p_type))
    {
        p_object->state = flags;

        if (p_object->pointed)
        {
            struct type t2 = type_remove_pointer(p_type);
            set_object(&t2, p_object->pointed, flags);
            type_destroy(&t2);
        }
    }
    else
    {
        //assert(p_object->members.size == 0); //enum?
        p_object->state = flags;
    }
}


static void object_set_unknown(struct type* p_type, struct object* p_object)
{
    if (p_object == NULL || p_type == NULL)
    {
        return;
    }

    if (p_type->struct_or_union_specifier && p_object->members.size > 0)
    {
        struct struct_or_union_specifier* p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier)
        {
            struct member_declaration* p_member_declaration =
                p_struct_or_union_specifier->member_declaration_list.head;

            int member_index = 0;
            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator* p_member_declarator =
                        p_member_declaration->member_declarator_list_opt->head;

                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            if (member_index < p_object->members.size)
                            {
                                object_set_unknown(&p_member_declarator->declarator->type, &p_object->members.data[member_index]);
                            }
                            else
                            {
                                //TODO BUG union?                                
                            }
                            member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
            return;
        }
    }

    if (type_is_pointer(p_type))
    {
        p_object->state = OBJECT_STATE_NULL | OBJECT_STATE_NOT_NULL;

        if (p_object->pointed)
        {
            struct type t2 = type_remove_pointer(p_type);
            object_set_unknown(&t2, p_object->pointed);
            type_destroy(&t2);
        }
    }
    else
    {
        p_object->state = OBJECT_STATE_ZERO | OBJECT_STATE_NOT_ZERO;
    }
}

//returns true if all parts that need to be moved weren't moved.
bool object_check(struct type* p_type, struct object* p_object)
{
    if (p_object == NULL)
    {
        return false;
    }
    if (p_type->type_qualifier_flags & TYPE_QUALIFIER_VIEW)
    {
        return false;
    }

    if (!type_is_any_owner(p_type))
    {
        return false;
    }

    if (p_type->struct_or_union_specifier && p_object->members.size > 0)
    {
        struct struct_or_union_specifier* p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        struct member_declaration* p_member_declaration =
            p_struct_or_union_specifier->member_declaration_list.head;
        int possible_need_destroy_count = 0;
        int need_destroy_count = 0;
        int member_index = 0;
        while (p_member_declaration)
        {
            if (p_member_declaration->member_declarator_list_opt)
            {
                struct member_declarator* p_member_declarator =
                    p_member_declaration->member_declarator_list_opt->head;
                while (p_member_declarator)
                {

                    if (p_member_declarator->declarator)
                    {
                        if (type_is_owner(&p_member_declarator->declarator->type))
                        {
                            possible_need_destroy_count++;
                        }

                        if (object_check(&p_member_declarator->declarator->type,
                            &p_object->members.data[member_index]))
                        {
                            need_destroy_count++;
                        }
                        member_index++;
                    }
                    p_member_declarator = p_member_declarator->next;
                }
            }
            p_member_declaration = p_member_declaration->next;
        }

        return need_destroy_count > 1 && (need_destroy_count == possible_need_destroy_count);
    }
    else
    {
        bool should_had_been_moved = false;
        if (type_is_pointer(p_type))
        {
            should_had_been_moved = (p_object->state & OBJECT_STATE_NOT_NULL);
        }
        else
        {
            if (p_object->state == OBJECT_STATE_UNINITIALIZED ||
                p_object->state == OBJECT_STATE_MOVED ||
                p_object->state == OBJECT_STATE_NOT_NULL ||
                p_object->state == (OBJECT_STATE_UNINITIALIZED | OBJECT_STATE_MOVED))
            {
            }
            else
            {
                should_had_been_moved = true;
            }
        }

        return should_had_been_moved;
    }

    return false;
}

void object_get_name_core(
    const struct type* p_type,
    const struct object* p_object,
    const struct object* p_object_target,
    const char* previous_names,
    char* out,
    int out_size)
{
    if (p_object == NULL)
    {
        return;
    }

    if (p_object == p_object_target)
    {
        snprintf(out, out_size, "%s", previous_names);
        return;
    }

    if (p_type->struct_or_union_specifier && p_object->members.size > 0)
    {
        struct struct_or_union_specifier* p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        struct member_declaration* p_member_declaration =
            p_struct_or_union_specifier->member_declaration_list.head;

        int member_index = 0;
        while (p_member_declaration)
        {
            if (p_member_declaration->member_declarator_list_opt)
            {
                struct member_declarator* p_member_declarator =
                    p_member_declaration->member_declarator_list_opt->head;
                while (p_member_declarator)
                {

                    if (p_member_declarator->declarator)
                    {
                        const char* name = p_member_declarator->declarator->name ? p_member_declarator->declarator->name->lexeme : "";
                        char buffer[200] = {0};
                        if (type_is_pointer(p_type))
                            snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                        else
                            snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);

                        object_get_name_core(
                            &p_member_declarator->declarator->type,
                            &p_object->members.data[member_index],
                            p_object_target,
                            buffer,
                            out,
                            out_size);

                        member_index++;
                    }
                    p_member_declarator = p_member_declarator->next;
                }
            }
            p_member_declaration = p_member_declaration->next;
        }

    }
    else
    {
        if (type_is_pointer(p_type))
        {
            char buffer[100] = {0};
            snprintf(buffer, sizeof buffer, "%s", previous_names);

            struct type t2 = type_remove_pointer(p_type);
            if (type_is_owner(&t2))
            {
                object_get_name_core(
                    &t2,
                    p_object->pointed,
                    p_object_target,
                    buffer,
                    out,
                    out_size);
            }
            type_destroy(&t2);
        }
    }
}


void object_get_name(const struct type* p_type,
    const struct object* p_object,
    char* out,
    int out_size)
{
    if (p_object->declarator == NULL)
    {
        out[0] = '?';
        out[1] = '\0';
        return;
    }


    const char* root_name = p_object->declarator->name ? p_object->declarator->name->lexeme : "?";
    const struct object* root = &p_object->declarator->object;

    object_get_name_core(&p_object->declarator->type, root, p_object, root_name, out, out_size);
}

void checked_moved(struct parser_ctx* ctx,
    struct type* p_type,
    struct object* p_object,
    const struct token* position_token)
{
    if (p_object == NULL)
    {
        return;
    }
    if (p_type->struct_or_union_specifier && p_object->members.size > 0)
    {
        struct struct_or_union_specifier* p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        struct member_declaration* p_member_declaration =
            p_struct_or_union_specifier->member_declaration_list.head;

        /*
        *  Some parts of the object needs to be moved..
        *  we need to print error one by one
        */
        int member_index = 0;
        while (p_member_declaration)
        {
            if (p_member_declaration->member_declarator_list_opt)
            {
                struct member_declarator* p_member_declarator =
                    p_member_declaration->member_declarator_list_opt->head;
                while (p_member_declarator)
                {

                    if (p_member_declarator->declarator)
                    {
                        checked_moved(ctx, &p_member_declarator->declarator->type,
                            &p_object->members.data[member_index],
                            position_token);

                        member_index++;
                    }
                    p_member_declarator = p_member_declarator->next;
                }
            }
            p_member_declaration = p_member_declaration->next;
        }
    }
    else
    {
        if (type_is_pointer(p_type) && !type_is_any_owner(p_type))
        {
            struct type t2 = type_remove_pointer(p_type);
            checked_moved(ctx,
                &t2,
                p_object->pointed,
                position_token);
            type_destroy(&t2);
        }

        if (p_object->state & OBJECT_STATE_MOVED)
        {
            struct token* name_pos = p_object->declarator->name ? p_object->declarator->name : p_object->declarator->first_token;
            const char* parameter_name = p_object->declarator->name ? p_object->declarator->name->lexeme : "?";

            char name[200] = {0};
            object_get_name(p_type, p_object, name, sizeof name);
            compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
                ctx,
                position_token,
                "parameter '%s' is leaving scoped with a moved object '%s'",
                parameter_name,
                name);

            compiler_set_info_with_token(W_NONE, ctx, name_pos, "parameter", name);
        }

        if (p_object->state & OBJECT_STATE_UNINITIALIZED)
        {
            struct token* name_pos = p_object->declarator->name ? p_object->declarator->name : p_object->declarator->first_token;
            const char* parameter_name = p_object->declarator->name ? p_object->declarator->name->lexeme : "?";

            char name[200] = {0};
            object_get_name(p_type, p_object, name, sizeof name);
            compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
                ctx,
                position_token,
                "parameter '%s' is leaving scoped with a uninitialized object '%s'",
                parameter_name,
                name);

            compiler_set_info_with_token(W_NONE, ctx, name_pos, "parameter", name);
        }
    }
}


void checked_read_object(struct parser_ctx* ctx,
    struct type* p_type,
    struct object* p_object,
    const struct token* position_token,
    bool check_pointed_object)
{
    if (p_object == NULL)
    {
        return;
    }
    if (p_type->struct_or_union_specifier && p_object->members.size > 0)
    {
        struct struct_or_union_specifier* p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        struct member_declaration* p_member_declaration =
            p_struct_or_union_specifier ?
            p_struct_or_union_specifier->member_declaration_list.head :
            NULL;

        /*
        *  Some parts of the object needs to be moved..
        *  we need to print error one by one
        */
        int member_index = 0;
        while (p_member_declaration)
        {
            if (p_member_declaration->member_declarator_list_opt)
            {
                struct member_declarator* p_member_declarator =
                    p_member_declaration->member_declarator_list_opt->head;
                while (p_member_declarator)
                {

                    if (p_member_declarator->declarator)
                    {
                        checked_read_object(ctx, &p_member_declarator->declarator->type,
                            &p_object->members.data[member_index],
                            position_token,
                            check_pointed_object);

                        member_index++;
                    }
                    p_member_declarator = p_member_declarator->next;
                }
            }
            p_member_declaration = p_member_declaration->next;
        }
    }
    else
    {
        if (type_is_pointer(p_type) &&
            check_pointed_object &&
            p_object->state & OBJECT_STATE_NOT_NULL /*we don't need to check pointed object*/
            )
        {
            struct type t2 = type_remove_pointer(p_type);
            checked_read_object(ctx,
                &t2,
                p_object->pointed,
                position_token,
                true);
            type_destroy(&t2);
        }

        if (p_object->state & OBJECT_STATE_MOVED)
        {
            //struct token* name_pos = p_object->declarator->name ? p_object->declarator->name : p_object->declarator->first_token;
            //const char* parameter_name = p_object->declarator->name ? p_object->declarator->name->lexeme : "?";

            char name[200] = {0};
            object_get_name(p_type, p_object, name, sizeof name);
            compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
                ctx,
                position_token,
                "object '%s' was moved",
                name);
        }

        if (p_object->state & OBJECT_STATE_UNINITIALIZED)
        {
            //struct token* name_pos = p_object->declarator->name ? p_object->declarator->name : p_object->declarator->first_token;

            char name[200] = {0};
            object_get_name(p_type, p_object, name, sizeof name);
            compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
                ctx,
                position_token,
                "uninitialized object '%s'",
                name);
        }
    }
}


void visit_object(struct parser_ctx* ctx,
    struct type* p_type,
    struct object* p_object,
    const struct token* position_token,
    const char* previous_names,
    bool is_assigment)
{
    if (p_object == NULL)
    {
        return;
    }
    if (p_type->type_qualifier_flags & TYPE_QUALIFIER_VIEW)
    {
        return;
    }

    if (!type_is_any_owner(p_type))
    {
        if (p_type->storage_class_specifier_flags & STORAGE_SPECIFIER_PARAMETER)
        {
            //for view parameters we need to check if they left something moved..
            checked_moved(ctx,
                p_type,
                p_object,
                position_token);

        }
        return;
    }


    if (p_type->struct_or_union_specifier && p_object->members.size > 0)
    {
        struct struct_or_union_specifier* p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        struct member_declaration* p_member_declaration =
            p_struct_or_union_specifier->member_declaration_list.head;

        if (object_check(p_type, p_object))
        {
            /*
            *  All parts of the object needs to be moved, so instead of
            *  describing each part we will just say that the object should
            *  have been moved.
            */
            const struct token* const name = p_object->declarator->name ? p_object->declarator->name : p_object->declarator->first_token;
            compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
                ctx,
                name,
                "object '%s' was not moved/destroyed",
                previous_names);

            if (p_object->declarator)
                compiler_set_info_with_token(W_NONE, ctx, position_token, "end of '%s' scope", previous_names);
        }
        else
        {
            /*
            *  Some parts of the object needs to be moved..
            *  we need to print error one by one
            */

            int member_index = 0;
            while (p_member_declaration)
            {

                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator* p_member_declarator =
                        p_member_declaration->member_declarator_list_opt->head;
                    while (p_member_declarator)
                    {

                        if (p_member_declarator->declarator)
                        {
                            const char* name = p_member_declarator->declarator->name ? p_member_declarator->declarator->name->lexeme : "?";

                            char buffer[200] = {0};
                            if (type_is_pointer(p_type))
                                snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                            else
                                snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);

                            visit_object(ctx, &p_member_declarator->declarator->type,
                                &p_object->members.data[member_index],
                                position_token,
                                buffer,
                                is_assigment);

                            member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
        }


    }
    else
    {
        const char* name = previous_names;
        const struct token* const position =
            p_object->declarator->name ? p_object->declarator->name : p_object->declarator->first_token;

        if (name[0] == '\0')
        {
            /*function arguments without name*/
            name = "?";
        }
        bool should_had_been_moved = false;


        /*
           Despide the name OBJECT_STATE_NOT_NULL does not means null, it means
           the reference is not refering an object, the value could be -1 for istnace.
        */
        if (type_is_pointer(p_type))
        {
            should_had_been_moved = (p_object->state & OBJECT_STATE_NOT_NULL);
        }
        else
        {
            if (p_object->state == OBJECT_STATE_UNINITIALIZED ||
                p_object->state == OBJECT_STATE_MOVED ||
                p_object->state == OBJECT_STATE_NULL)
            {
            }
            else
            {
                should_had_been_moved = true;
            }
        }


        if (type_is_pointer(p_type))
        {
            if (should_had_been_moved)
            {
                char buffer[100] = {0};
                snprintf(buffer, sizeof buffer, "%s", previous_names);

                struct type t2 = type_remove_pointer(p_type);
                if (type_is_owner(&t2))
                {
                    visit_object(ctx,
                        &t2,
                        p_object->pointed,
                        position_token,
                        buffer,
                        is_assigment);
                }
                type_destroy(&t2);
            }

        }


        if (should_had_been_moved)
        {
            if (type_is_obj_owner(p_type))
            {

            }
            else
            {
                if (type_is_pointer(p_type))
                {
                    if (is_assigment)
                    {
                        compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
                            ctx,
                            position_token,
                            "memory pointed by '%s' was not released before assignment.",
                            name);
                    }
                    else
                    {
                        compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
                            ctx,
                            position,
                            "memory pointed by '%s' was not released.",
                            name);
                        if (p_object->declarator)
                        {
                            compiler_set_info_with_token(W_NONE, ctx, position_token, "end of '%s' scope", name);
                        }
                    }

                }
                else
                {
                    if (is_assigment)
                    {
                        compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
                            ctx,
                            position_token,
                            "previous members of '%s' were not moved before this assignment.",
                            name);
                    }
                    else
                    {
                        compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
                            ctx,
                            position,
                            "object '%s' was not moved.",
                            name);
                        if (p_object->declarator)
                        {
                            compiler_set_info_with_token(W_NONE, ctx, position_token, "end of '%s' scope", name);
                        }
                    }
                }
            }
        }
    }

}


void object_assigment(struct parser_ctx* ctx,
    struct object* p_source_obj_opt,
    struct type* p_source_obj_type,

    struct object* p_dest_obj_opt,
    struct type* p_dest_obj_type,

    const struct token* error_position,
    bool bool_source_zero_value,
    enum object_state source_state_after)
{
    if (p_dest_obj_opt)
    {
        if (type_is_owner(p_dest_obj_type))
        {
            char buffer[100] = {0};
            object_get_name(p_dest_obj_type, p_dest_obj_opt, buffer, sizeof buffer);
            visit_object(ctx,
                p_dest_obj_type,
                p_dest_obj_opt,
                error_position,
                buffer,
                true);
        }
    }


    if (p_dest_obj_opt)
    {
        if (bool_source_zero_value)
        {
            set_object(p_dest_obj_type, p_dest_obj_opt, OBJECT_STATE_NULL);
        }
        else
        {
            if (p_source_obj_opt)
            {
                set_object_state(ctx, p_dest_obj_type, p_dest_obj_opt, p_source_obj_type, p_source_obj_opt, error_position);
            }
            else
            {
                object_set_unknown(p_dest_obj_type, p_dest_obj_opt);
            }
        }

    }


    if (type_is_any_owner(p_source_obj_type) &&
        !type_is_owner(p_dest_obj_type) &&
        p_source_obj_type->storage_class_specifier_flags & STORAGE_SPECIFIER_FUNCTION_RETURN)
    {
        /*
        int main()
        {
           struct X * p = (struct X * owner) malloc(1);
        }
        */

        compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
            ctx,
            error_position,
            "Object must be owner qualified.");
    }

    if (type_is_any_owner(p_dest_obj_type) && type_is_any_owner(p_source_obj_type) && type_is_pointer(p_source_obj_type))
    {
        if (type_is_void_ptr(p_dest_obj_type))
        {
            if (p_source_obj_opt)
            {
                struct type t2 = type_remove_pointer(p_source_obj_type);
                const char* name = p_source_obj_opt->declarator->name ?
                    p_source_obj_opt->declarator->name->lexeme :
                    "?";

                visit_object(ctx,
                    &t2,
                    p_source_obj_opt->pointed,
                    error_position,
                    name,
                    true);
                p_source_obj_opt->state = source_state_after;
                type_destroy(&t2);
            }
        }
        else if (type_is_obj_owner(p_dest_obj_type))
        {
            if (type_is_owner(p_source_obj_type))
            {
                if (p_source_obj_opt->pointed)
                {
                    struct type t = type_remove_pointer(p_source_obj_type);
                    set_object(&t, p_source_obj_opt->pointed, source_state_after);
                    type_destroy(&t);
                }
            }
            else if (type_is_obj_owner(p_source_obj_type))
            {
                if (p_source_obj_opt->pointed)
                {
                    struct type t = type_remove_pointer(p_source_obj_type);
                    set_object(&t, p_source_obj_opt->pointed, source_state_after);
                    type_destroy(&t);
                }
            }
        }
        else
        {

            if (p_source_obj_opt)
            {
                set_object(p_source_obj_type, p_source_obj_opt, source_state_after);
            }
        }
    }
    else if (type_is_any_owner(p_dest_obj_type) && type_is_any_owner(p_source_obj_type))
    {
        /*everthing is moved*/
        if (p_source_obj_opt)
            set_object(p_source_obj_type, p_source_obj_opt, source_state_after);
    }
    else
    {
        /*nothing changes*/
    }

}


struct visit_objects {
    struct flow_defer_scope* current_block;
    struct flow_defer_scope* next_child;
};

struct object* visit_objects_next(struct visit_objects* visit_objects)
{
    while (visit_objects->current_block)
    {
        while (visit_objects->next_child)
        {
            if (visit_objects->next_child->declarator)
            {
                struct object* p = &visit_objects->next_child->declarator->object;
                visit_objects->next_child = visit_objects->next_child->previous;
                return p;
            }
            else
            {
                visit_objects->next_child = visit_objects->next_child->previous;
            }
        }

        visit_objects->current_block = visit_objects->current_block->previous;
        if (visit_objects->current_block)
            visit_objects->next_child = visit_objects->current_block->last_child;



    }

    return NULL;
}

struct flow_defer_scope* flow_visit_ctx_push_tail_block(struct flow_visit_ctx* ctx)
{
    struct flow_defer_scope* owner p_block = calloc(1, sizeof * p_block);
    p_block->previous = ctx->tail_block;
    ctx->tail_block = p_block;
    return ctx->tail_block;
}

struct flow_defer_scope* flow_visit_ctx_push_child(struct flow_visit_ctx* ctx)
{
    struct flow_defer_scope* owner child = calloc(1, sizeof * child);
    child->previous = ctx->tail_block->last_child;
    ctx->tail_block->last_child = child;
    return ctx->tail_block->last_child;
}

void flow_defer_scope_delete_one(struct flow_defer_scope* owner p)
{
    struct flow_defer_scope* owner p_block = p;
    if (p_block != NULL)
    {
        struct flow_defer_scope* owner child = p_block->last_child;
        while (child != NULL)
        {
            struct flow_defer_scope* owner previous = child->previous;
            child->previous = NULL;
            flow_defer_scope_delete_one(child);
            child = previous;
        }
        assert(p_block->previous == NULL);
        free(p_block);
    }
}


void flow_visit_ctx_pop_tail_block(struct flow_visit_ctx* ctx)
{
    if (ctx->tail_block)
    {
        struct flow_defer_scope* owner previous = ctx->tail_block->previous;
        ctx->tail_block->previous = NULL;
        flow_defer_scope_delete_one(ctx->tail_block);
        ctx->tail_block = previous;
    }
}

static bool check_defer_and_variables(struct flow_visit_ctx* ctx,
    struct flow_defer_scope* deferblock,
    struct token* position_token)
{
    bool found_error = false;

    struct flow_defer_scope* deferchild = deferblock->last_child;
    while (deferchild != NULL)
    {
        if (deferchild->defer_statement)
        {
            flow_visit_secondary_block(ctx, deferchild->defer_statement->secondary_block);
        }
        else if (deferchild->declarator)
        {
            struct declarator* p_declarator = deferchild->declarator;
            const char* name = p_declarator->name ? p_declarator->name->lexeme : "?";

            visit_object(ctx->ctx, &p_declarator->type, &p_declarator->object, position_token, name, false);

        }
        deferchild = deferchild->previous;
    }
    return found_error;
}

static bool flow_find_label_unlabeled_statement(struct flow_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement, const char* label);

static bool check_all_defer_until_try(struct flow_visit_ctx* ctx, struct flow_defer_scope* deferblock,
    struct token* position_token)
{
    bool found_error = false;
    struct flow_defer_scope* p_defer = deferblock;
    while (p_defer != NULL)
    {
        if (check_defer_and_variables(ctx, p_defer, position_token))
            found_error = true;

        if (p_defer->p_try_statement)
        {
            break;
        }

        p_defer = p_defer->previous;
    }
    return found_error;
}

static bool flow_find_label_block_item_list(struct flow_visit_ctx* ctx, struct block_item_list* p_block_item, const char* label)
{
    struct block_item* block_item = p_block_item->head;
    while (block_item)
    {
        if (block_item &&
            block_item->label &&
            block_item->label->name &&
            strcmp(block_item->label->name->lexeme, label) == 0)
        {
            /*achou*/
            return true;
        }
        else if (block_item->unlabeled_statement)
        {
            if (flow_find_label_unlabeled_statement(ctx, block_item->unlabeled_statement, label))
            {
                return true;
            }
        }

        block_item = block_item->next;
    }

    return false;
}

static bool flow_find_label_statement(struct flow_visit_ctx* ctx, struct statement* statement, const char* label);
static bool flow_find_label_unlabeled_statement(struct flow_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement, const char* label)
{
    if (p_unlabeled_statement->primary_block)
    {
        if (p_unlabeled_statement->primary_block->compound_statement)
        {
            if (flow_find_label_block_item_list(ctx,
                &p_unlabeled_statement->primary_block->compound_statement->block_item_list,
                label))
            {
                return true;
            }
        }
        else if (p_unlabeled_statement->primary_block->selection_statement)
        {
            if (flow_find_label_statement(ctx, p_unlabeled_statement->primary_block->selection_statement->secondary_block->statement, label))
            {
                return true;
            }
            if (p_unlabeled_statement->primary_block->selection_statement->else_secondary_block_opt)
            {
                if (flow_find_label_statement(ctx, p_unlabeled_statement->primary_block->selection_statement->else_secondary_block_opt->statement, label))
                {
                    return true;
                }
            }
        }
        else if (p_unlabeled_statement->primary_block->try_statement)
        {
            if (flow_find_label_statement(ctx, p_unlabeled_statement->primary_block->try_statement->secondary_block->statement, label))
            {
                return true;
            }
            if (p_unlabeled_statement->primary_block->try_statement->catch_secondary_block_opt)
            {
                if (flow_find_label_statement(ctx, p_unlabeled_statement->primary_block->try_statement->catch_secondary_block_opt->statement, label))
                {
                    return true;
                }
            }
        }
        else if (p_unlabeled_statement->primary_block->iteration_statement)
        {
            if (flow_find_label_statement(ctx, p_unlabeled_statement->primary_block->iteration_statement->secondary_block->statement, label))
            {
                return true;
            }
        }
    }
    return false;
}

static bool flow_find_label_statement(struct flow_visit_ctx* ctx, struct statement* statement, const char* label)
{
    if (statement->labeled_statement)
    {
        if (statement->labeled_statement &&
            statement->labeled_statement->label &&
            strcmp(statement->labeled_statement->label->name->lexeme, label) == 0)
        {
            /*achou*/
            return true;
        }
    }
    else if (statement->unlabeled_statement)
    {
        if (flow_find_label_unlabeled_statement(ctx, statement->unlabeled_statement, label))
            return true;
    }
    return false;
}

static bool flow_find_label_scope(struct flow_visit_ctx* ctx, struct flow_defer_scope* deferblock, const char* label)
{
    if (deferblock->p_iteration_statement)
    {
        if (deferblock->p_iteration_statement->secondary_block)
        {
            if (flow_find_label_statement(ctx, deferblock->p_iteration_statement->secondary_block->statement, label))
                return true;
        }
    }
    else if (deferblock->p_selection_statement)
    {
        if (deferblock->p_selection_statement->secondary_block)
        {
            if (flow_find_label_statement(ctx, deferblock->p_selection_statement->secondary_block->statement, label))
                return true;
        }

        if (deferblock->p_selection_statement->else_secondary_block_opt)
        {
            if (flow_find_label_statement(ctx, deferblock->p_selection_statement->else_secondary_block_opt->statement, label))
                return true;
        }
    }
    else if (deferblock->p_try_statement)
    {
        if (deferblock->p_try_statement->secondary_block)
        {
            if (flow_find_label_statement(ctx, deferblock->p_try_statement->secondary_block->statement, label))
                return true;
        }

        if (deferblock->p_try_statement->catch_secondary_block_opt)
        {
            if (flow_find_label_statement(ctx, deferblock->p_try_statement->catch_secondary_block_opt->statement, label))
                return true;
        }
    }
    else if (deferblock->p_statement)
    {
        if (flow_find_label_statement(ctx, deferblock->p_statement, label))
            return true;
    }
    else if (deferblock->p_function_body)
    {
        if (flow_find_label_block_item_list(ctx,
            &deferblock->p_function_body->block_item_list,
            label))
        {
            return true;
        }
    }
    else if (deferblock->p_compound_statement)
    {
        if (flow_find_label_block_item_list(ctx,
            &deferblock->p_compound_statement->block_item_list,
            label))
        {
            return true;
        }
    }
    return false;
}

static bool check_all_defer_until_label(struct flow_visit_ctx* ctx, struct flow_defer_scope* deferblock, const char* label,
    struct token* position_token)
{
    bool error_found = false;
    /*
    * Precisamos saber quantos escopos nós saimos até achar o label.
    * Para isso procuramos no escopo atual aonde aparede o goto.
    * Se o label não esta diretamente neste escopo ou dentro de algum escopo interno
    * Não nós imprimos os defers pois estamos saindo do escopo e vamos para o escopo
    * de cima. Assim vamos repetindo em cada saida de escopo imprimos o defer.
    */
    struct flow_defer_scope* p_defer = deferblock;

    while (p_defer != NULL)
    {
        if (!flow_find_label_scope(ctx, p_defer, label))
        {
            error_found = error_found || check_defer_and_variables(ctx, p_defer, position_token);
        }
        else
        {
            break;
        }
        p_defer = p_defer->previous;
    }
    return error_found;
}

static bool check_all_defer_until_iter(struct flow_visit_ctx* ctx,
    struct flow_defer_scope* deferblock,
    struct token* position_token)
{
    bool error_found = false;
    struct flow_defer_scope* p_defer = deferblock;
    while (p_defer != NULL)
    {
        if (check_defer_and_variables(ctx, p_defer, position_token))
            error_found = true;
        if (p_defer->p_iteration_statement)
        {
            break;
        }
        p_defer = p_defer->previous;
    }
    return error_found;
}

static bool check_all_defer_until_iteration_or_selection_statement(struct flow_visit_ctx* ctx,
    struct flow_defer_scope* deferblock,
    struct token* position_token)
{
    bool error_found = false;
    struct flow_defer_scope* p_defer = deferblock;
    while (p_defer != NULL)
    {
        if (check_defer_and_variables(ctx, p_defer, position_token))
            error_found = true;
        if (p_defer->p_iteration_statement || p_defer->p_selection_statement)
        {
            //break pode ser usado em loops or switch
            break;
        }
        p_defer = p_defer->previous;
    }
    return error_found;
}

static bool check_all_defer_until_end(struct flow_visit_ctx* ctx, struct flow_defer_scope* deferblock, struct token* position_token)
{
    bool found_found = false;
    struct flow_defer_scope* p_defer = deferblock;
    while (p_defer != NULL)
    {
        found_found = found_found || check_defer_and_variables(ctx, p_defer, position_token);
        p_defer = p_defer->previous;
    }
    return found_found;
}



static void flow_visit_secondary_block(struct flow_visit_ctx* ctx, struct secondary_block* p_secondary_block)
{

    flow_visit_statement(ctx, p_secondary_block->statement);

}

static void flow_visit_defer_statement(struct flow_visit_ctx* ctx, struct defer_statement* p_defer_statement)
{

    //adiciona como filho do ultimo bloco
//    struct defer_scope* p_defer = visit_ctx_push_tail_child(ctx);
  //  p_defer->defer_statement = p_defer_statement;




    struct flow_defer_scope* p_defer = flow_visit_ctx_push_child(ctx);
    p_defer->defer_statement = p_defer_statement;


    /*
      We are not going to visit the secondary block here because
      this is not the place were defer is executed.
    */
}

static void flow_visit_try_statement(struct flow_visit_ctx* ctx, struct try_statement* p_try_statement)
{
    struct flow_defer_scope* p_defer = flow_visit_ctx_push_tail_block(ctx);
    p_defer->p_try_statement = p_try_statement;

    if (p_try_statement->secondary_block)
        flow_visit_secondary_block(ctx, p_try_statement->secondary_block);

    check_defer_and_variables(ctx, p_defer, p_try_statement->secondary_block->last_token);

    flow_visit_ctx_pop_tail_block(ctx);
}

static struct object* expression_is_comparing_owner_with_null(struct expression* p_expression)
{
    if (p_expression->expression_type == EQUALITY_EXPRESSION_EQUAL &&
        type_is_pointer(&p_expression->left->type) &&
        is_null_pointer_constant(p_expression->right))
    {
        // if ( pointer == NULL )
        struct type type = {0};
        struct object* p_object = expression_get_object(p_expression->left, &type);
        type_destroy(&type);
        return p_object;
    }


    if (p_expression->expression_type == EQUALITY_EXPRESSION_EQUAL &&
        is_null_pointer_constant(p_expression->left) &&
        type_is_pointer(&p_expression->right->type))
    {
        // if (NULL == pointer)
        struct type type = {0};
        struct object* p_object = expression_get_object(p_expression->right, &type);
        type_destroy(&type);
        return p_object;
    }


    if (p_expression->expression_type == UNARY_EXPRESSION_NOT &&
        type_is_pointer(&p_expression->right->type))
    {
        //if (!p) same as p == NULL
        struct type type = {0};
        struct object* p_object = expression_get_object(p_expression->right, &type);
        type_destroy(&type);
        return p_object;
    }

    return NULL;
}

static struct object* expression_is_comparing_owner_with_not_null(struct expression* p_expression)
{


    if (p_expression->expression_type == EQUALITY_EXPRESSION_NOT_EQUAL &&
        type_is_pointer(&p_expression->left->type) &&
        is_null_pointer_constant(p_expression->right))
    {
        //if (p != NULL)
        struct type type = {0};
        struct object* p_object = expression_get_object(p_expression->left, &type);
        type_destroy(&type);
        return p_object;
    }

    if (p_expression->expression_type == EQUALITY_EXPRESSION_NOT_EQUAL &&
        is_null_pointer_constant(p_expression->right) &&
        type_is_pointer(&p_expression->left->type))
    {
        //NULL != p 
        struct type type = {0};
        struct object* p_object = expression_get_object(p_expression->right, &type);
        type_destroy(&type);
        return p_object;
    }


    if (type_is_pointer(&p_expression->type))
    {
        // if (p) same as p != NULL
        struct type type = {0};
        struct object* p_object = expression_get_object(p_expression, &type);
        type_destroy(&type);
        return p_object;
    }

    return NULL;
}

void push_copy_of_current_state(struct flow_visit_ctx* ctx)
{
    /*
      top of stack constains the copy
    */

    struct visit_objects v1 = {.current_block = ctx->tail_block,
                                  .next_child = ctx->tail_block->last_child};

    struct object* p_object = visit_objects_next(&v1);
    while (p_object)
    {
        object_push_copy_current_state(p_object);
        p_object = visit_objects_next(&v1);
    }

}

void restore_state(struct flow_visit_ctx* ctx, int state_index_to_restore)
{
    struct visit_objects v1 = {.current_block = ctx->tail_block,
                               .next_child = ctx->tail_block->last_child};

    struct object* p_object = visit_objects_next(&v1);
    while (p_object)
    {
        object_restore_state(p_object, state_index_to_restore);
        p_object = visit_objects_next(&v1);
    };
}


static void object_merge_states_with_current(struct object* object,
    int dest_index,
    int before_index,
    int after_index)
{
    if (dest_index == 0 || (object->object_state_stack.size - dest_index >= 0 &&
        object->object_state_stack.size - dest_index < object->object_state_stack.size))
    {
    }
    else
    {
        return;
    }

    enum object_state* dest = dest_index == 0 ? &object->state :
        &object->object_state_stack.data[object->object_state_stack.size - dest_index];


    if (before_index == 0 || (object->object_state_stack.size - before_index >= 0 &&
        object->object_state_stack.size - before_index < object->object_state_stack.size))
    {
    }
    else
    {
        return;
    }
    enum object_state state_before = before_index == 0 ? object->state :
        object->object_state_stack.data[object->object_state_stack.size - before_index];




    if (after_index == 0 ||
        (object->object_state_stack.size - after_index >= 0 &&
            object->object_state_stack.size - after_index < object->object_state_stack.size))
    {
    }
    else
    {

        return;
    }
    enum object_state state_after = after_index == 0 ? object->state :
        object->object_state_stack.data[object->object_state_stack.size - after_index];

    *dest |= (state_before | state_after);


    if (object->pointed)
    {
        object_merge_states_with_current(object->pointed, dest_index, before_index, after_index);
    }

    for (int i = 0; i < object->members.size; i++)
    {
        object_merge_states_with_current(&object->members.data[i], dest_index, before_index, after_index);
    }

}

void merge_states(struct flow_visit_ctx* ctx,
    int dest_index,
    int before_index, //before while
    int after_index)
{
    struct visit_objects v1 = {.current_block = ctx->tail_block,
                               .next_child = ctx->tail_block->last_child};

    struct object* p_object = visit_objects_next(&v1);
    while (p_object)
    {
        object_merge_states_with_current(p_object, dest_index,
            before_index,
            after_index);
        p_object = visit_objects_next(&v1);
    };
}


static void object_merge_if_else_states(struct object* object,
    int dest_index,
    int original_state,
    int true_branch_state,
    int false_branch_state)
{
    if (dest_index == 0 || (object->object_state_stack.size - dest_index >= 0 &&
        object->object_state_stack.size - dest_index < object->object_state_stack.size))
    {
    }
    else
    {
        return;
    }
    if (original_state == 0 || (object->object_state_stack.size - original_state >= 0 &&
        object->object_state_stack.size - original_state < object->object_state_stack.size))
    {
    }
    else
    {

        return;
    }    if (true_branch_state == 0 ||
        (object->object_state_stack.size - true_branch_state >= 0 &&
            object->object_state_stack.size - true_branch_state < object->object_state_stack.size))
    {
    }
    else
    {
        return;
    }
    if (false_branch_state == 0 || (object->object_state_stack.size - false_branch_state >= 0 &&
        object->object_state_stack.size - false_branch_state < object->object_state_stack.size))
    {
    }
    else
    {

        return;
    }


    enum object_state* dest = dest_index == 0 ? &object->state :
        &object->object_state_stack.data[object->object_state_stack.size - dest_index];


    enum object_state s_original = original_state == 0 ? object->state :
        object->object_state_stack.data[object->object_state_stack.size - original_state];


    enum object_state s_true_branch = true_branch_state == 0 ? object->state :
        object->object_state_stack.data[object->object_state_stack.size - true_branch_state];


    enum object_state s_false_branch = false_branch_state == 0 ? object->state :
        object->object_state_stack.data[object->object_state_stack.size - false_branch_state];


    if (s_true_branch != s_original &&
        s_false_branch != s_original)
    {
        *dest |= (s_true_branch | s_false_branch);
    }
    else if (s_true_branch != s_original)
    {
        *dest |= (s_true_branch | s_original);
    }
    else if (s_false_branch != s_original)
    {
        *dest |= (s_false_branch | s_original);
    }
    else
    {
        *dest |= s_original;
    }

    if (object->pointed)
    {
        object_merge_if_else_states(object->pointed, dest_index, original_state, true_branch_state, false_branch_state);
    }

    for (int i = 0; i < object->members.size; i++)
    {
        object_merge_if_else_states(&object->members.data[i], dest_index, original_state, true_branch_state, false_branch_state);
    }

}

void merge_if_else_states(struct flow_visit_ctx* ctx,
    int dest_index,
    int original_state, //original
    int true_branch_state, //true branch
    int false_branch_state) //false branch
{
    struct visit_objects v1 = {.current_block = ctx->tail_block,
                               .next_child = ctx->tail_block->last_child};

    struct object* p_object = visit_objects_next(&v1);
    while (p_object)
    {
        object_merge_if_else_states(p_object, dest_index,
            original_state,
            true_branch_state,
            false_branch_state);
        p_object = visit_objects_next(&v1);
    };
}

void pop_states(struct flow_visit_ctx* ctx, int n)
{
    struct visit_objects v1 = {.current_block = ctx->tail_block,
                               .next_child = ctx->tail_block->last_child};

    struct object* p_object = visit_objects_next(&v1);
    while (p_object)
    {
        object_pop_states(p_object, n);
        p_object = visit_objects_next(&v1);
    };
}


static void flow_visit_if_statement(struct flow_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    assert(p_selection_statement->first_token->type == TK_KEYWORD_IF);
    struct object* p_object_compared_with_null = NULL;

    if (p_selection_statement->expression)
    {
        p_object_compared_with_null = expression_is_comparing_owner_with_null(p_selection_statement->expression);
    }

    struct object* p_object_compared_with_not_null = NULL;
    if (p_selection_statement->expression)
    {
        p_object_compared_with_not_null = expression_is_comparing_owner_with_not_null(p_selection_statement->expression);
    }


    /*
       This index is from the end of top of stack going to base of statck
    */
    const int original = 2;
    push_copy_of_current_state(ctx);

    if (p_object_compared_with_null)
    {
        //if (p == 0) {  p is null }
        p_object_compared_with_null->state = OBJECT_STATE_NULL;
    }
    if (p_object_compared_with_not_null)
    {
        //if (p != 0) {  p is not null }
        p_object_compared_with_not_null->state = OBJECT_STATE_NOT_NULL;
    }

    if (p_selection_statement->secondary_block)
    {

        flow_visit_secondary_block(ctx, p_selection_statement->secondary_block);
        // check_defer_and_variables(ctx, p_defer, p_selection_statement->secondary_block->last_token);

    }

    bool was_last_statement_inside_true_branch_return = false;
    if (ctx->p_last_jump_statement)
    {
        //TODO gotos etc...

        was_last_statement_inside_true_branch_return =
            ctx->p_last_jump_statement->first_token->type == TK_KEYWORD_RETURN ||
            ctx->p_last_jump_statement->first_token->type == TK_KEYWORD_BREAK ||
            ctx->p_last_jump_statement->first_token->type == TK_KEYWORD_THROW ||
            ctx->p_last_jump_statement->first_token->type == TK_KEYWORD_CONTINUE;
    }

    //enum object_state state_left_in_true_branch = 0;
    //if (p_object_compared_with_null)
      //  state_left_in_true_branch = p_object_compared_with_null->state;
    //else if (p_object_compared_with_not_null)
      //  state_left_in_true_branch = p_object_compared_with_not_null->state;

    /*let's make a copy of the state we left true branch*/
    const int true_branch = 1;
    push_copy_of_current_state(ctx);

    restore_state(ctx, original);

    const int current = 0;

    /*when we dont have else block we have a empty imaginary one */
    /*we invert the object state*/
    if (p_object_compared_with_null)
    {
        // if (p == NULL) { } else { p is not null }
        p_object_compared_with_null->state = OBJECT_STATE_NOT_NULL;
    }
    if (p_object_compared_with_not_null)
    {
        // if (p != NULL) { } else { p is null }
        p_object_compared_with_not_null->state = OBJECT_STATE_NULL;
    }

    ctx->p_last_jump_statement = NULL;
    if (p_selection_statement->else_secondary_block_opt)
    {
        //struct flow_defer_scope* owner p_defer = calloc(1, sizeof * p_defer);
        //p_defer->previous = ctx->tail_block;
        //ctx->tail_block = p_defer;
        //p_defer->p_selection_statement = p_selection_statement;
        flow_visit_secondary_block(ctx, p_selection_statement->else_secondary_block_opt);
        //ctx->tail_block = ctx->tail_block->previous; //POP

    }

    bool was_last_statement_inside_else_branch_return = false;
    if (ctx->p_last_jump_statement)
    {
        //TODO gotos etc...
        was_last_statement_inside_else_branch_return =
            ctx->p_last_jump_statement->first_token->type == TK_KEYWORD_RETURN ||
            ctx->p_last_jump_statement->first_token->type == TK_KEYWORD_BREAK ||
            ctx->p_last_jump_statement->first_token->type == TK_KEYWORD_THROW ||
            ctx->p_last_jump_statement->first_token->type == TK_KEYWORD_CONTINUE;
    }


    if (was_last_statement_inside_true_branch_return)
    {
        if (was_last_statement_inside_else_branch_return)
        {
            restore_state(ctx, original);
        }
        else
        {
            //the else state is the current state
        }
    }
    else
    {
        if (was_last_statement_inside_else_branch_return)
        {
            restore_state(ctx, true_branch);
        }
        else
        {
            merge_if_else_states(ctx, current, original, true_branch, current);
        }
    }


    pop_states(ctx, 2);



}
static void flow_visit_block_item(struct flow_visit_ctx* ctx, struct block_item* p_block_item);

static void flow_visit_switch_statement(struct flow_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    assert(p_selection_statement->first_token->type == TK_KEYWORD_SWITCH);

    int inverse_stack = 1; //we have 1 item
    push_copy_of_current_state(ctx); //2 (permanent copy)


    //const int current = 0;

    //const int nothing = -1;

    //bool has_default_case = false;
    int default_index = -1;

    //for each case
    // visit
    // merge not so simple if set in all branches the initial cannot be merged
    // restore
    struct statement* p_statement = p_selection_statement->secondary_block->statement;
    if (p_statement &&
        p_statement->unlabeled_statement &&
        p_statement->unlabeled_statement->primary_block &&
        p_statement->unlabeled_statement->primary_block->compound_statement)
    {
        struct compound_statement* p_compound_statement =
            p_statement->unlabeled_statement->primary_block->compound_statement;

        struct block_item* item = p_compound_statement->block_item_list.head;
        while (item)
        {
            if (item->label && item->first_token->type == TK_KEYWORD_CASE)
            {
                /*
                  Each time we find a case we restore the state to the state we
                  have before entering switch
                */
                restore_state(ctx, inverse_stack);
            }
            if (item->label && item->first_token->type == TK_KEYWORD_DEFAULT)
            {
                default_index = inverse_stack;
                /*
                  Each time we find a case we restore the state to the state we
                  have before entering switch
                */
                restore_state(ctx, inverse_stack);
            }
            else if (item->unlabeled_statement &&
                item->unlabeled_statement->jump_statement &&
                item->first_token->type == TK_KEYWORD_BREAK)
            {
                /*
                  Each time we find a break we safe the state
                  pushing it
                */
                push_copy_of_current_state(ctx);
                inverse_stack++;
            }
            flow_visit_block_item(ctx, item);

            item = item->next;
        }
    }

    if (default_index == -1)
    {
        inverse_stack++;
        default_index = inverse_stack;
        push_copy_of_current_state(ctx);

    }

    const int original = inverse_stack;

    merge_if_else_states(ctx, 0, original, 2, default_index);

    for (int i = 3; i < inverse_stack; i++)
    {
        merge_if_else_states(ctx, 0, original, i, 0);
    }


    /*
      we have a stack of states for each case label
      now we need to merge them
    */
    //The number of case we have is  inverse_stack - 1

    pop_states(ctx, inverse_stack);
}

static void flow_visit_selection_statement(struct flow_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    struct flow_defer_scope* p_defer = flow_visit_ctx_push_tail_block(ctx);
    p_defer->p_selection_statement = p_selection_statement;


    if (p_selection_statement->first_token->type == TK_KEYWORD_IF)
    {
        flow_visit_if_statement(ctx, p_selection_statement);
    }
    else if (p_selection_statement->first_token->type == TK_KEYWORD_SWITCH)
    {
        flow_visit_switch_statement(ctx, p_selection_statement);
    }
    else
        assert(false);

    flow_visit_ctx_pop_tail_block(ctx);
}

static void flow_visit_compound_statement(struct flow_visit_ctx* ctx, struct compound_statement* p_compound_statement);

static void flow_visit_initializer_list(struct flow_visit_ctx* ctx, struct initializer_list* p_initializer_list);

static void flow_visit_bracket_initializer_list(struct flow_visit_ctx* ctx, struct braced_initializer* p_bracket_initializer_list)
{
    if (p_bracket_initializer_list->initializer_list == NULL)
    {

    }
    else
    {
        flow_visit_initializer_list(ctx, p_bracket_initializer_list->initializer_list);
    }
}

static void flow_visit_designation(struct flow_visit_ctx* ctx, struct designation* p_designation)
{
}

static void flow_visit_initializer(struct flow_visit_ctx* ctx, struct initializer* p_initializer)
{
    if (p_initializer->p_attribute_specifier_sequence_opt)
    {
        flow_visit_attribute_specifier_sequence(ctx, p_initializer->p_attribute_specifier_sequence_opt);
    }

    if (p_initializer->designation)
    {
        flow_visit_designation(ctx, p_initializer->designation);
    }

    if (p_initializer->assignment_expression)
    {
        flow_visit_expression(ctx, p_initializer->assignment_expression);
    }
    else if (p_initializer->braced_initializer)
    {
        flow_visit_bracket_initializer_list(ctx, p_initializer->braced_initializer);
    }
}

static void flow_visit_initializer_list(struct flow_visit_ctx* ctx, struct initializer_list* p_initializer_list)
{
    struct initializer* p_initializer = p_initializer_list->head;
    while (p_initializer)
    {
        flow_visit_initializer(ctx, p_initializer);
        p_initializer = p_initializer->next;
    }
}

static void flow_visit_type_qualifier(struct flow_visit_ctx* ctx, struct type_qualifier* p_type_qualifier)
{

}

static void flow_visit_specifier_qualifier(struct flow_visit_ctx* ctx, struct type_specifier_qualifier* p_specifier_qualifier)
{
    if (p_specifier_qualifier->type_specifier)
        flow_visit_type_specifier(ctx, p_specifier_qualifier->type_specifier);

    if (p_specifier_qualifier->type_qualifier)
        flow_visit_type_qualifier(ctx, p_specifier_qualifier->type_qualifier);
}

static void flow_visit_specifier_qualifier_list(struct flow_visit_ctx* ctx, struct specifier_qualifier_list* p_specifier_qualifier_list_opt,
    struct type* p_type)
{
    if (p_specifier_qualifier_list_opt == NULL)
        return;

    if (p_specifier_qualifier_list_opt->struct_or_union_specifier)
    {
        flow_visit_struct_or_union_specifier(ctx, p_specifier_qualifier_list_opt->struct_or_union_specifier);
    }
    else if (p_specifier_qualifier_list_opt->enum_specifier)
    {
        flow_visit_enum_specifier(ctx, p_specifier_qualifier_list_opt->enum_specifier);
    }
    else if (p_specifier_qualifier_list_opt->typedef_declarator)
    {
        //typedef name
    }
    //else if (p_specifier_qualifier_list->p_typeof_expression_opt)
    //{
      //  flow_visit_expression(ctx, p_specifier_qualifier_list->p_typeof_expression_opt);
    //}
    else
    {
        struct type_specifier_qualifier* p_specifier_qualifier = p_specifier_qualifier_list_opt->head;
        while (p_specifier_qualifier)
        {
            flow_visit_specifier_qualifier(ctx, p_specifier_qualifier);
            p_specifier_qualifier = p_specifier_qualifier->next;
        }
    }
}
static void flow_visit_declarator(struct flow_visit_ctx* ctx, struct declarator* p_declarator);
static void flow_visit_type_name(struct flow_visit_ctx* ctx, struct type_name* p_type_name)
{

    flow_visit_specifier_qualifier_list(ctx, p_type_name->specifier_qualifier_list, &p_type_name->type);
    flow_visit_declarator(ctx, p_type_name->declarator);


    /*
    * Vamos esconder tudo e gerar um novo
    *  Exemplo
    *  (const typeof(int (*)())) -> *  ( int (*const )() )
    */
}

static void flow_visit_argument_expression_list(struct flow_visit_ctx* ctx, struct argument_expression_list* p_argument_expression_list)
{
    struct argument_expression* p_argument_expression =
        p_argument_expression_list->head;
    while (p_argument_expression)
    {
        flow_visit_expression(ctx, p_argument_expression->expression);
        p_argument_expression = p_argument_expression->next;
    }
}

static void flow_visit_generic_selection(struct flow_visit_ctx* ctx, struct generic_selection* p_generic_selection)
{
    if (p_generic_selection->expression)
    {
        flow_visit_expression(ctx, p_generic_selection->expression);
    }
    else if (p_generic_selection->type_name)
    {
        flow_visit_type_name(ctx, p_generic_selection->type_name);
    }
}

static int compare_function_arguments2(struct parser_ctx* ctx,
    struct type* p_type,
    struct argument_expression_list* p_argument_expression_list)
{

    struct param* p_current_parameter_type = NULL;

    const struct param_list* p_param_list = type_get_func_or_func_ptr_params(p_type);

    if (p_param_list)
    {
        p_current_parameter_type = p_param_list->head;
    }

    int param_num = 1;
    struct argument_expression* p_current_argument = p_argument_expression_list->head;

    while (p_current_argument && p_current_parameter_type)
    {
        struct type argument_object_type = {0};
        struct object* p_argument_object =
            expression_get_object(p_current_argument->expression, &argument_object_type);

        bool bool_source_zero_value = constant_value_is_valid(&p_current_argument->expression->constant_value) &&
            constant_value_to_ull(&p_current_argument->expression->constant_value) == 0;


        if (ctx->options.null_checks)
        {
            if (type_is_pointer(&p_current_parameter_type->type) &&
                !(p_current_parameter_type->type.type_qualifier_flags & TYPE_QUALIFIER_OPT))
            {
                struct type argument_object_type2 = {0};
                struct object* p_argument_object2 =
                    expression_get_object(p_current_argument->expression, &argument_object_type2);

                if (p_argument_object2 &&
                    p_argument_object2->state & OBJECT_STATE_NULL)
                {
                    compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
                        ctx,
                        p_current_argument->expression->first_token,
                        "pointer can be null, but the parameter is not optional");
                }

                type_destroy(&argument_object_type2);
            }
        }


        /*
           checking is some uninitialized or moved object is being used as parameter
        */
        if (p_argument_object)
        {
            //TODO check if pointed object is const
            bool check_pointed_object = !type_is_void_ptr(&p_current_parameter_type->type);

            checked_read_object(ctx,
                &argument_object_type,
                p_argument_object,
                p_current_argument->expression->first_token,
                check_pointed_object);
        }

        if (type_is_any_owner(&p_current_parameter_type->type))
        {
#if 1

#endif

            object_assigment(ctx,
                p_argument_object,
                &argument_object_type,
                NULL, /*dest object*/
                &p_current_parameter_type->type,
                p_current_argument->expression->first_token,
                bool_source_zero_value,
                OBJECT_STATE_UNINITIALIZED);
        }
        else
        {
            if (p_argument_object &&
                type_is_pointer(&p_current_parameter_type->type))
            {
                struct type pointed_type =
                    type_remove_pointer(&p_current_parameter_type->type);

                if (!type_is_const(&pointed_type))
                {
                    if (type_is_void(&pointed_type))
                    {
                        //
                    }
                    else
                    {
                        object_set_unknown(&argument_object_type, p_argument_object);
                    }
                }

                type_destroy(&pointed_type);
            }
        }
        p_current_argument = p_current_argument->next;
        p_current_parameter_type = p_current_parameter_type->next;
        param_num++;

        type_destroy(&argument_object_type);
    }

    return 0;
}

static void flow_visit_expression(struct flow_visit_ctx* ctx, struct expression* p_expression)
{
    if (p_expression == NULL)
        return;

    switch (p_expression->expression_type)
    {
        case PRIMARY_EXPRESSION__FUNC__:
            break;
        case PRIMARY_IDENTIFIER:
            break;
        case PRIMARY_EXPRESSION_ENUMERATOR:

            break;
        case PRIMARY_EXPRESSION_DECLARATOR:

            if (p_expression->declarator->object.state & OBJECT_STATE_UNINITIALIZED)
            {
                //TODO inside sizeof(v)  is not an error. :D
                //TODO function type...
#if 0
                compiler_set_warning_with_token(W_UNINITIALZED,
                    ctx->ctx,
                    p_expression->first_token,
                    "'%s' is uninitialized ",
                    p_expression->declarator->object_name->lexeme);
#endif
            }

            break;

        case PRIMARY_EXPRESSION_PARENTESIS:
            flow_visit_expression(ctx, p_expression->right);
            break;

        case PRIMARY_EXPRESSION_STRING_LITERAL:
            break;
        case PRIMARY_EXPRESSION_CHAR_LITERAL:
            break;
        case PRIMARY_EXPRESSION_NUMBER:
            break;

        case PRIMARY_EXPRESSION_PREDEFINED_CONSTANT:

            break;

        case PRIMARY_EXPRESSION_GENERIC:
            flow_visit_generic_selection(ctx, p_expression->generic_selection);
            break;

        case POSTFIX_DOT:
            break;
        case POSTFIX_ARROW:
            break;
        case POSTFIX_INCREMENT:
            break;
        case POSTFIX_DECREMENT:
            break;
        case POSTFIX_ARRAY:

            flow_visit_expression(ctx, p_expression->left);
            flow_visit_expression(ctx, p_expression->right);

            struct type t = {0};
            struct object* p_object = expression_get_object(p_expression->left, &t);

            if (p_object && p_object->state == OBJECT_STATE_UNINITIALIZED)
            {
                compiler_set_error_with_token(C_STATIC_ASSERT_FAILED,
                    ctx->ctx,
                    p_expression->left->first_token, "using a uninitialized object");
            }
            else if (p_object && p_object->state & OBJECT_STATE_UNINITIALIZED)
            {
                compiler_set_error_with_token(C_STATIC_ASSERT_FAILED,
                    ctx->ctx,
                    p_expression->left->first_token, "maybe using a uninitialized object");
            }
            type_destroy(&t);

            break;

        case POSTFIX_FUNCTION_CALL:

            flow_visit_expression(ctx, p_expression->left);

            flow_visit_argument_expression_list(ctx, &p_expression->argument_expression_list);
            compare_function_arguments2(ctx->ctx, &p_expression->left->type, &p_expression->argument_expression_list);

            break;
        case POSTFIX_EXPRESSION_FUNCTION_LITERAL:


            flow_visit_compound_statement(ctx, p_expression->compound_statement);


            break;

        case POSTFIX_EXPRESSION_COMPOUND_LITERAL:

            if (p_expression->type_name)
            {
                flow_visit_type_name(ctx, p_expression->type_name);
            }

            flow_visit_bracket_initializer_list(ctx, p_expression->braced_initializer);

            assert(p_expression->left == NULL);
            assert(p_expression->right == NULL);

            break;

        case UNARY_EXPRESSION_ALIGNOF:

            if (p_expression->right)
            {
                flow_visit_expression(ctx, p_expression->right);
            }

            if (p_expression->type_name)
            {
                /*sizeof*/
                flow_visit_type_name(ctx, p_expression->type_name);
            }
            break;

        case UNARY_EXPRESSION_ASSERT:

            if (p_expression->right)
            {
                flow_visit_expression(ctx, p_expression->right);

                struct object* p_object_compared_with_null = NULL;

                if (p_expression->right)
                {
                    p_object_compared_with_null = expression_is_comparing_owner_with_null(p_expression->right);
                }

                struct object* p_object_compared_with_not_null = NULL;
                if (p_expression->right)
                {
                    p_object_compared_with_not_null = expression_is_comparing_owner_with_not_null(p_expression->right);
                }

                if (p_object_compared_with_null)
                {
                    //if (p == 0) {  p is null }
                    p_object_compared_with_null->state = OBJECT_STATE_NULL;
                }

                if (p_object_compared_with_not_null)
                {
                    //if (p != 0) {  p is not null }
                    p_object_compared_with_not_null->state = OBJECT_STATE_NOT_NULL;
                }
            }

            break;

        case UNARY_EXPRESSION_SIZEOF_EXPRESSION:
        case UNARY_EXPRESSION_SIZEOF_TYPE:
        case UNARY_EXPRESSION_INCREMENT:
        case UNARY_EXPRESSION_DECREMENT:

        case UNARY_EXPRESSION_NOT:
        case UNARY_EXPRESSION_BITNOT:
        case UNARY_EXPRESSION_NEG:
        case UNARY_EXPRESSION_PLUS:
        case UNARY_EXPRESSION_CONTENT:
        case UNARY_EXPRESSION_ADDRESSOF:
            if (p_expression->right)
            {
                flow_visit_expression(ctx, p_expression->right);
            }

            if (p_expression->type_name)
            {
                /*sizeof*/
                flow_visit_type_name(ctx, p_expression->type_name);
            }

            break;







        case ASSIGNMENT_EXPRESSION:
        {

            struct type right_object_type = {0};
            struct object* const p_right_object = expression_get_object(p_expression->right, &right_object_type);

            struct type dest_object_type = {0};
            struct object* const p_dest_object = expression_get_object(p_expression->left, &dest_object_type);
            //print_object(&dest_object_type, p_dest_object);
            flow_visit_expression(ctx, p_expression->left);
            //print_object(&dest_object_type, p_dest_object);
            flow_visit_expression(ctx, p_expression->right);



            bool bool_source_zero_value = constant_value_is_valid(&p_expression->right->constant_value) &&
                constant_value_to_ull(&p_expression->right->constant_value) == 0;

            if (p_expression->right &&
                p_expression->right->expression_type == POSTFIX_FUNCTION_CALL)
            {
                if (p_expression->right->left &&
                    p_expression->right->left->declarator &&
                    p_expression->right->left->declarator->name &&
                    strcmp(p_expression->right->left->declarator->name->lexeme, "calloc") == 0)
                {
                    bool_source_zero_value = true;
                }
            }

            object_assigment(ctx->ctx,
                p_right_object, /*source*/
                &right_object_type, /*source type*/
                p_dest_object, /*dest object*/
                &dest_object_type, /*dest type*/
                p_expression->left->first_token,
                bool_source_zero_value,
                OBJECT_STATE_MOVED);

            type_destroy(&right_object_type);
            type_destroy(&dest_object_type);

        }
        break;

        case CAST_EXPRESSION:
        case MULTIPLICATIVE_EXPRESSION_MULT:
        case MULTIPLICATIVE_EXPRESSION_DIV:
        case MULTIPLICATIVE_EXPRESSION_MOD:
        case ADDITIVE_EXPRESSION_PLUS:
        case ADDITIVE_EXPRESSION_MINUS:
        case SHIFT_EXPRESSION_RIGHT:
        case SHIFT_EXPRESSION_LEFT:
        case RELATIONAL_EXPRESSION_BIGGER_THAN:
        case RELATIONAL_EXPRESSION_LESS_THAN:


        case EQUALITY_EXPRESSION_EQUAL:
            flow_visit_expression(ctx, p_expression->left);
            flow_visit_expression(ctx, p_expression->right);

            break;

        case EQUALITY_EXPRESSION_NOT_EQUAL:
            flow_visit_expression(ctx, p_expression->left);
            flow_visit_expression(ctx, p_expression->right);
            break;

        case AND_EXPRESSION:
        case EXCLUSIVE_OR_EXPRESSION:
        case INCLUSIVE_OR_EXPRESSION:
        case INCLUSIVE_AND_EXPRESSION:
        case LOGICAL_OR_EXPRESSION:
        case RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN:
        case RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN:

            if (p_expression->left)
            {
                flow_visit_expression(ctx, p_expression->left);
            }
            if (p_expression->right)
            {
                flow_visit_expression(ctx, p_expression->right);
            }
            if (p_expression->type_name)
            {
                flow_visit_type_name(ctx, p_expression->type_name);
            }
            break;

        case UNARY_EXPRESSION_TRAITS:
        {

        }
        break;

        case UNARY_EXPRESSION_IS_SAME:
            break;

        case UNARY_DECLARATOR_ATTRIBUTE_EXPR:
            break;

        case CONDITIONAL_EXPRESSION:
            if (p_expression->condition_expr)
            {
                flow_visit_expression(ctx, p_expression->condition_expr);
            }

            if (p_expression->left)
            {
                flow_visit_expression(ctx, p_expression->left);
            }
            if (p_expression->right)
            {
                flow_visit_expression(ctx, p_expression->right);
            }

            break;

        default:
            break;
            }
    }

static void flow_visit_expression_statement(struct flow_visit_ctx* ctx, struct expression_statement* p_expression_statement)
{
    if (p_expression_statement->expression_opt)
        flow_visit_expression(ctx, p_expression_statement->expression_opt);
}

static void flow_visit_block_item_list(struct flow_visit_ctx* ctx, struct block_item_list* p_block_item_list);

static void flow_visit_compound_statement(struct flow_visit_ctx* ctx, struct compound_statement* p_compound_statement)
{
    struct flow_defer_scope* p_defer = flow_visit_ctx_push_tail_block(ctx);
    p_defer->p_compound_statement = p_compound_statement;

    flow_visit_block_item_list(ctx, &p_compound_statement->block_item_list);
    check_defer_and_variables(ctx, p_defer, p_compound_statement->last_token);

    flow_visit_ctx_pop_tail_block(ctx);
}

static void flow_visit_do_while_statement(struct flow_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    assert(p_iteration_statement->first_token->type == TK_KEYWORD_DO);

    struct object* p_object_compared_with_not_null = NULL;

    if (p_iteration_statement->expression1)
    {
        p_object_compared_with_not_null = expression_is_comparing_owner_with_not_null(p_iteration_statement->expression1);
        flow_visit_expression(ctx, p_iteration_statement->expression1);
    }

    if (p_iteration_statement->secondary_block)
    {
        struct flow_defer_scope* p_defer = flow_visit_ctx_push_tail_block(ctx);
        p_defer->p_iteration_statement = p_iteration_statement;

        flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

        check_defer_and_variables(ctx, p_defer, p_iteration_statement->secondary_block->last_token);

        flow_visit_ctx_pop_tail_block(ctx);

        bool was_last_statement_inside_true_branch_return = false;
        if (ctx->p_last_jump_statement)
        {

            was_last_statement_inside_true_branch_return =
                ctx->p_last_jump_statement->first_token->type == TK_KEYWORD_RETURN ||
                ctx->p_last_jump_statement->first_token->type == TK_KEYWORD_BREAK ||
                ctx->p_last_jump_statement->first_token->type == TK_KEYWORD_THROW ||
                ctx->p_last_jump_statement->first_token->type == TK_KEYWORD_CONTINUE;
        }

        if (was_last_statement_inside_true_branch_return)
        {
            //restore_state(ctx, 0);
            //if (p_object_compared_with_not_null)
            //{
                //do {}  while (p);
              //  p_object_compared_with_not_null->state = OBJECT_STATE_NULL;
            //}
        }
        else
        {
            //do { } while (p); 

            if (p_object_compared_with_not_null)
            {
                p_object_compared_with_not_null->state = OBJECT_STATE_NULL;
            }
        }
    }
}
static void flow_visit_while_statement(struct flow_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    assert(p_iteration_statement->first_token->type == TK_KEYWORD_WHILE);

    struct object* p_object_compared_with_not_null = NULL;

    if (p_iteration_statement->expression1)
    {
        p_object_compared_with_not_null = expression_is_comparing_owner_with_not_null(p_iteration_statement->expression1);
        flow_visit_expression(ctx, p_iteration_statement->expression1);
    }

    if (p_iteration_statement->secondary_block)
    {
        const int original = 1;
        push_copy_of_current_state(ctx);

        const int current = 0;

        struct flow_defer_scope* p_defer = flow_visit_ctx_push_tail_block(ctx);
        p_defer->p_iteration_statement = p_iteration_statement;

        if (p_object_compared_with_not_null)
        {
            //while (p != 0) {  p is not null }
            p_object_compared_with_not_null->state = OBJECT_STATE_NOT_NULL;
        }

        flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

        check_defer_and_variables(ctx, p_defer, p_iteration_statement->secondary_block->last_token);


        bool was_last_statement_inside_true_branch_return = false;
        if (ctx->p_last_jump_statement)
        {
            //TODO gotos etc...
            was_last_statement_inside_true_branch_return =
                ctx->p_last_jump_statement->first_token->type == TK_KEYWORD_RETURN;
        }

        if (was_last_statement_inside_true_branch_return)
        {
            restore_state(ctx, original);
            //if (p_object_compared_with_not_null)
            //{
                /*
                 while (p)
                 {
                       return;
                 }
                */
                //  p_object_compared_with_not_null->state = OBJECT_STATE_NULL;
              //}
        }
        else
        {
            /*
               while (p)
               {
               }
            */
            //merge_if_else_states(ctx, current, current, original, -1 /*nothing*/);
            merge_states(ctx, current, original, current);
            if (p_object_compared_with_not_null)
            {
                p_object_compared_with_not_null->state = OBJECT_STATE_NULL;
            }
        }
        pop_states(ctx, 1);
        flow_visit_ctx_pop_tail_block(ctx);
    }
}

static void flow_visit_iteration_statement(struct flow_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    if (p_iteration_statement->first_token->type == TK_KEYWORD_WHILE)
    {
        flow_visit_while_statement(ctx, p_iteration_statement);
    }
    else if (p_iteration_statement->first_token->type == TK_KEYWORD_DO)
    {
        flow_visit_do_while_statement(ctx, p_iteration_statement);
    }
    else if (p_iteration_statement->first_token->type == TK_KEYWORD_FOR)
    {
        //flow_visit_do_while_statement(ctx, p_iteration_statement);
    }
}

static void flow_visit_jump_statement(struct flow_visit_ctx* ctx, struct jump_statement* p_jump_statement)
{
    ctx->p_last_jump_statement = p_jump_statement;


    if (p_jump_statement->first_token->type == TK_KEYWORD_THROW)
    {
        check_all_defer_until_try(ctx, ctx->tail_block, p_jump_statement->first_token);
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_RETURN)
    {
        if (p_jump_statement->expression_opt)
        {
            flow_visit_expression(ctx, p_jump_statement->expression_opt);
        }

        /*
          returning a declarator will move the onwership
        */
        if (p_jump_statement->expression_opt)
        {
            struct type type = {0};
            struct object* p_object = expression_get_object(p_jump_statement->expression_opt, &type);
            bool bool_source_zero_value = constant_value_is_valid(&p_jump_statement->expression_opt->constant_value) &&
                constant_value_to_ull(&p_jump_statement->expression_opt->constant_value) == 0;


            checked_read_object(ctx->ctx,
                &type,
                p_object,
                p_jump_statement->expression_opt->first_token,
                true);

            object_assigment(ctx->ctx,
                p_object, /*source*/
                &type, /*source type*/
                NULL, /*dest object*/
                ctx->p_return_type, /*dest type*/
                p_jump_statement->expression_opt->first_token,
                bool_source_zero_value,
                OBJECT_STATE_UNINITIALIZED);

            type_destroy(&type);

        }
        check_all_defer_until_end(ctx, ctx->tail_block, p_jump_statement->first_token);
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_CONTINUE)
    {
        check_all_defer_until_iter(ctx, ctx->tail_block, p_jump_statement->first_token);
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_BREAK)
    {
        check_all_defer_until_iteration_or_selection_statement(ctx, ctx->tail_block, p_jump_statement->first_token);
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_GOTO)
    {
        check_all_defer_until_label(ctx, ctx->tail_block, p_jump_statement->label->lexeme, p_jump_statement->first_token);

        //set_all_until_end(ctx, ctx->tail_block, (OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL));
    }
    else
    {
        assert(false);
    }
}

static void flow_visit_label(struct flow_visit_ctx* ctx, struct label* p_label);

static void flow_visit_labeled_statement(struct flow_visit_ctx* ctx, struct labeled_statement* p_labeled_statement)
{
    if (p_labeled_statement->label)
    {
        flow_visit_label(ctx, p_labeled_statement->label);
    }

    if (p_labeled_statement->statement)
    {
        flow_visit_statement(ctx, p_labeled_statement->statement);
    }
}

static void flow_visit_primary_block(struct flow_visit_ctx* ctx, struct primary_block* p_primary_block)
{

    if (p_primary_block->defer_statement)
    {
        flow_visit_defer_statement(ctx, p_primary_block->defer_statement);
    }
    else
    {
        if (p_primary_block->compound_statement)
        {
            flow_visit_compound_statement(ctx, p_primary_block->compound_statement);
        }
        else if (p_primary_block->iteration_statement)
        {
            flow_visit_iteration_statement(ctx, p_primary_block->iteration_statement);
        }
        else if (p_primary_block->selection_statement)
        {
            flow_visit_selection_statement(ctx, p_primary_block->selection_statement);
        }
        else if (p_primary_block->try_statement)
        {
            flow_visit_try_statement(ctx, p_primary_block->try_statement);
        }
    }
}

static void flow_visit_unlabeled_statement(struct flow_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement)
{
    ctx->p_last_jump_statement = NULL;
    if (p_unlabeled_statement->primary_block)
    {
        flow_visit_primary_block(ctx, p_unlabeled_statement->primary_block);
    }
    else if (p_unlabeled_statement->expression_statement)
    {
        flow_visit_expression_statement(ctx, p_unlabeled_statement->expression_statement);
    }
    else if (p_unlabeled_statement->jump_statement)
    {
        flow_visit_jump_statement(ctx, p_unlabeled_statement->jump_statement);
    }
    else
    {
        assert(false);
    }
}

static void flow_visit_statement(struct flow_visit_ctx* ctx, struct statement* p_statement)
{
    ctx->p_last_jump_statement = NULL;

    if (p_statement->labeled_statement)
    {
        flow_visit_labeled_statement(ctx, p_statement->labeled_statement);
    }
    else if (p_statement->unlabeled_statement)
    {
        flow_visit_unlabeled_statement(ctx, p_statement->unlabeled_statement);
    }
}

static void flow_visit_label(struct flow_visit_ctx* ctx, struct label* p_label)
{

}

static void flow_visit_block_item(struct flow_visit_ctx* ctx, struct block_item* p_block_item)
{
    ctx->p_last_jump_statement = NULL;
    if (p_block_item->declaration)
    {
        flow_visit_declaration(ctx, p_block_item->declaration);
    }
    else if (p_block_item->unlabeled_statement)
    {
        flow_visit_unlabeled_statement(ctx, p_block_item->unlabeled_statement);
    }
    else if (p_block_item->label)
    {
        flow_visit_label(ctx, p_block_item->label);
    }

}

static void flow_visit_block_item_list(struct flow_visit_ctx* ctx, struct block_item_list* p_block_item_list)
{
    struct block_item* p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        flow_visit_block_item(ctx, p_block_item);
        p_block_item = p_block_item->next;
    }
}

enum object_state parse_string_state(const char* s, bool* invalid)
{
    *invalid = false;

    enum object_state e = 0;
    const char* p = s;

    while (*p)
    {
        if (isalpha(*p))
        {
            const char* start = p;
            while (isalpha(*p) || *p == '-')
            {
                p++;
            }

            if (strncmp(start, "moved", p - start) == 0)
                e |= OBJECT_STATE_MOVED;
            else if (strncmp(start, "null", p - start) == 0)
                e |= OBJECT_STATE_NULL;
            else if (strncmp(start, "not-null", p - start) == 0)
                e |= OBJECT_STATE_NOT_NULL;
            else if (strncmp(start, "maybe-null", p - start) == 0)
                e |= (OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL);
            else if (strncmp(start, "uninitialized", p - start) == 0)
                e |= OBJECT_STATE_UNINITIALIZED;
            else if (strncmp(start, "zero", p - start) == 0)
                e |= OBJECT_STATE_ZERO;
            else if (strncmp(start, "not-zero", p - start) == 0)
                e |= OBJECT_STATE_NOT_ZERO;
            else if (strncmp(start, "any", p - start) == 0)
                e |= (OBJECT_STATE_NOT_ZERO | OBJECT_STATE_ZERO);
            else if (strncmp(start, "moved", p - start) == 0)
                e |= OBJECT_STATE_MOVED;
            else if (strncmp(start, "or", p - start) == 0)
            {
                //skiped
            }
            else
            {
                *invalid = true;
                return 0; //error invalid name
            }
        }
        else
        {
            p++;
        }
    }

    return e;

}


static void flow_visit_static_assert_declaration(struct flow_visit_ctx* ctx, struct static_assert_declaration* p_static_assert_declaration)
{
    flow_visit_expression(ctx, p_static_assert_declaration->constant_expression);

    if (p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_DEBUG)
    {
        compiler_set_info_with_token(W_NONE, ctx->ctx, p_static_assert_declaration->first_token, "static_debug");

        struct type t = {0};
        struct object* p_obj = expression_get_object(p_static_assert_declaration->constant_expression, &t);



        if (p_obj)
        {
            print_object(&t, p_obj, true);
        }

        type_destroy(&t);
    }
    else if (p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_STATE)
    {
        struct type t = {0};
        struct object* p_obj = expression_get_object(p_static_assert_declaration->constant_expression, &t);
        if (p_obj)
        {
            if (p_static_assert_declaration->string_literal_opt)
            {
                bool is_invalid = false;
                enum object_state e =
                    parse_string_state(p_static_assert_declaration->string_literal_opt->lexeme, &is_invalid);
                if (!is_invalid)
                {
                    if (e != p_obj->state)
                    {
                        compiler_set_error_with_token(C_STATIC_ASSERT_FAILED, ctx->ctx, p_static_assert_declaration->first_token, "static_state failed");
                    }
                }
                else
                {
                    compiler_set_error_with_token(C_STATIC_ASSERT_FAILED, ctx->ctx, p_static_assert_declaration->first_token, "invalid parameter %s", p_static_assert_declaration->string_literal_opt->lexeme);
                }
            }

        }
        type_destroy(&t);
    }
    else if (p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_SET)
    {
        struct type t = {0};
        struct object* p_obj = expression_get_object(p_static_assert_declaration->constant_expression, &t);
        if (p_obj)
        {
            if (p_static_assert_declaration->string_literal_opt)
            {

                const char* lexeme =
                    p_static_assert_declaration->string_literal_opt->lexeme;

                if (strcmp(lexeme, "\"zero\"") == 0)
                {
                    //gives the semantics of {0} or calloc                    
                    set_direct_state(&t, p_obj, OBJECT_STATE_ZERO);
                }
                else
                {
                    bool is_invalid = false;
                    enum object_state e =
                        parse_string_state(p_static_assert_declaration->string_literal_opt->lexeme, &is_invalid);

                    if (!is_invalid)
                    {
                        set_object(&t, p_obj, e);
                    }
                    else
                    {
                        compiler_set_error_with_token(C_STATIC_ASSERT_FAILED, ctx->ctx, p_static_assert_declaration->first_token, "invalid parameter %s", p_static_assert_declaration->string_literal_opt->lexeme);
                    }
                }
            }

        }
        type_destroy(&t);
    }
}

static void flow_visit_declaration_specifiers(struct flow_visit_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers,
    struct type* p_type);


static void flow_visit_direct_declarator(struct flow_visit_ctx* ctx, struct direct_declarator* p_direct_declarator)
{
    if (p_direct_declarator->function_declarator)
    {
        struct parameter_declaration* parameter = NULL;

        if (p_direct_declarator->function_declarator->parameter_type_list_opt)
        {
            parameter = p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head;
        }


        ctx->parameter_list++;

        while (parameter)
        {
            if (parameter->attribute_specifier_sequence_opt)
            {
                flow_visit_attribute_specifier_sequence(ctx, parameter->attribute_specifier_sequence_opt);
            }

            flow_visit_declaration_specifiers(ctx, parameter->declaration_specifiers, &parameter->declarator->type);
            flow_visit_declarator(ctx, parameter->declarator);
            parameter = parameter->next;
        }

        ctx->parameter_list--;

    }
    else if (p_direct_declarator->array_declarator)
    {
        if (p_direct_declarator->array_declarator->assignment_expression)
        {
            flow_visit_expression(ctx, p_direct_declarator->array_declarator->assignment_expression);
        }

    }
}

static void flow_visit_declarator(struct flow_visit_ctx* ctx, struct declarator* p_declarator)
{
    if (ctx->tail_block &&
        p_declarator->type.category != TYPE_CATEGORY_FUNCTION)
    {

        if (ctx->parameter_list > 1)
        {
            /*
             The objective here is to avoid including the arguments
             of function pointers inside the scope.
             Sample
             void x_destroy(void (*f)(void * owner p))
             We add f but not p.
            */
            return;
        }

        struct flow_defer_scope* p_defer = flow_visit_ctx_push_child(ctx);
        p_defer->declarator = p_declarator;


        struct object temp = make_object(&p_declarator->type, p_declarator);
        object_swap(&temp, &p_declarator->object);
        object_destroy(&temp);

        if (p_declarator->declaration_specifiers &&
            p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_PARAMETER)
        {
            if (type_is_pointer(&p_declarator->type))
            {
                if (!ctx->ctx->options.null_checks ||
                    (p_declarator->type.type_qualifier_flags & TYPE_QUALIFIER_OPT))
                {
                    set_object(&p_declarator->type, &p_declarator->object, (OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL));
                }
                else
                {
                    set_object(&p_declarator->type, &p_declarator->object, (OBJECT_STATE_NOT_NULL));
                }
            }
            else
            {
                set_object(&p_declarator->type, &p_declarator->object, (OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL));
            }



            if (type_is_pointer(&p_declarator->type))
            {
                //TODO necessary?
                struct type t2 = type_remove_pointer(&p_declarator->type);
                if (p_declarator->object.pointed)
                {
                    set_object(&t2, p_declarator->object.pointed, (OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL));
                }
                type_destroy(&t2);
            }
        }
    }

    /*if (p_declarator->pointer)
    {
        struct pointer* p = p_declarator->pointer;
        while (p)
        {
            p = p->pointer;
        }
    }*/


    if (p_declarator->direct_declarator)
    {
        flow_visit_direct_declarator(ctx, p_declarator->direct_declarator);
    }
}

static void flow_visit_init_declarator_list(struct flow_visit_ctx* ctx, struct init_declarator_list* p_init_declarator_list)
{
    struct init_declarator* p_init_declarator = p_init_declarator_list->head;

    while (p_init_declarator)
    {
        if (p_init_declarator->p_declarator)
        {
            flow_visit_declarator(ctx, p_init_declarator->p_declarator);
        }

        if (p_init_declarator->initializer)
        {
            if (p_init_declarator->initializer->assignment_expression)
            {
                flow_visit_expression(ctx, p_init_declarator->initializer->assignment_expression);
            }
            else
            {
                assert(p_init_declarator->initializer->braced_initializer != NULL);
                if (p_init_declarator->initializer->braced_initializer)
                {
                    flow_visit_bracket_initializer_list(ctx,
                        p_init_declarator->initializer->braced_initializer);

                    //set zero
                }
            }
        }

        if (p_init_declarator->p_declarator->type.category != TYPE_CATEGORY_FUNCTION)
        {
            if (p_init_declarator->initializer &&
                p_init_declarator->initializer->assignment_expression)
            {
                struct type right_object_type = {0};
                struct object* p_right_object =
                    expression_get_object(p_init_declarator->initializer->assignment_expression, &right_object_type);

                bool bool_source_zero_value = constant_value_is_valid(&p_init_declarator->initializer->assignment_expression->constant_value) &&
                    constant_value_to_ull(&p_init_declarator->initializer->assignment_expression->constant_value) == 0;


                //cast?
                if (p_init_declarator->initializer->assignment_expression->expression_type == POSTFIX_FUNCTION_CALL &&
                    p_init_declarator->initializer->assignment_expression->left &&
                    p_init_declarator->initializer->assignment_expression->left->declarator &&
                    p_init_declarator->initializer->assignment_expression->left->declarator->name)
                {
                    if (strcmp(p_init_declarator->initializer->assignment_expression->left->declarator->name->lexeme, "calloc") == 0)
                    {
                        p_init_declarator->p_declarator->object.state = OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL;

                        if (p_init_declarator->p_declarator->object.pointed)
                        {
                            struct type t = type_remove_pointer(&p_init_declarator->p_declarator->type);
                            set_direct_state(&t, p_init_declarator->p_declarator->object.pointed, OBJECT_STATE_ZERO);
                            type_destroy(&t);
                        }
                    }
                    else if (strcmp(p_init_declarator->initializer->assignment_expression->left->declarator->name->lexeme, "malloc") == 0)
                    {
                        p_init_declarator->p_declarator->object.state = OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL;

                        struct type t = type_remove_pointer(&p_init_declarator->p_declarator->type);
                        set_direct_state(&t, p_init_declarator->p_declarator->object.pointed, OBJECT_STATE_UNINITIALIZED);
                        type_destroy(&t);
                    }
                    else
                    {
                        const struct token* const token_position =
                            p_init_declarator->p_declarator->name ?
                            p_init_declarator->p_declarator->name :
                            p_init_declarator->p_declarator->first_token
                            ;

                        object_assigment(ctx->ctx, p_right_object, &right_object_type,
                            &p_init_declarator->p_declarator->object,
                            &p_init_declarator->p_declarator->type,
                            token_position,
                            bool_source_zero_value,
                            OBJECT_STATE_MOVED);
                    }
                }
                else
                {
                    const struct token* const token_position =
                        p_init_declarator->p_declarator->name ?
                        p_init_declarator->p_declarator->name :
                        p_init_declarator->p_declarator->first_token
                        ;

                    object_assigment(ctx->ctx, p_right_object, &right_object_type,
                        &p_init_declarator->p_declarator->object,
                        &p_init_declarator->p_declarator->type,
                        token_position,
                        bool_source_zero_value,
                        OBJECT_STATE_MOVED);
                }

                type_destroy(&right_object_type);
            }
            else  if (p_init_declarator->initializer &&
                p_init_declarator->initializer->braced_initializer)
            {
                bool is_zero_initialized = false;
                if (p_init_declarator->initializer->braced_initializer->initializer_list == NULL)
                {
                    is_zero_initialized = true;
                }
                else
                {
                    if (p_init_declarator->initializer->braced_initializer->initializer_list->size == 1 &&
                        p_init_declarator->initializer->braced_initializer->initializer_list->head->assignment_expression)
                    {
                        struct constant_value* p_constant_value =
                            &p_init_declarator->initializer->braced_initializer->initializer_list->head->assignment_expression->constant_value;

                        if (constant_value_is_valid(p_constant_value) &&
                            constant_value_to_ull(p_constant_value) == 0)
                        {
                            is_zero_initialized = true;
                        }

                    }
                }

                if (is_zero_initialized)
                {
                    set_direct_state(&p_init_declarator->p_declarator->type, &p_init_declarator->p_declarator->object, OBJECT_STATE_ZERO);
                }
                else
                {
                    set_direct_state(&p_init_declarator->p_declarator->type, &p_init_declarator->p_declarator->object, OBJECT_STATE_ZERO);
                }
            }
            else
            {
                set_object(&p_init_declarator->p_declarator->type,
                    &p_init_declarator->p_declarator->object,
                    OBJECT_STATE_UNINITIALIZED);
            }
        }

        p_init_declarator = p_init_declarator->next;
    }



}

static void flow_visit_member_declarator(struct flow_visit_ctx* ctx, struct member_declarator* p_member_declarator)
{
    if (p_member_declarator->declarator)
    {
        flow_visit_declarator(ctx, p_member_declarator->declarator);
    }
}

static void flow_visit_member_declarator_list(struct flow_visit_ctx* ctx, struct member_declarator_list* p_member_declarator_list)
{
    struct member_declarator* p_member_declarator = p_member_declarator_list->head;
    while (p_member_declarator)
    {
        flow_visit_member_declarator(ctx, p_member_declarator);
        p_member_declarator = p_member_declarator->next;
    }
}

static void flow_visit_member_declaration(struct flow_visit_ctx* ctx, struct member_declaration* p_member_declaration)
{
    if (p_member_declaration->member_declarator_list_opt)
    {
        flow_visit_specifier_qualifier_list(ctx,
            p_member_declaration->specifier_qualifier_list,
            &p_member_declaration->member_declarator_list_opt->head->declarator->type); /*se nao tem?*/
    }

    if (p_member_declaration->member_declarator_list_opt)
    {
        flow_visit_member_declarator_list(ctx, p_member_declaration->member_declarator_list_opt);
    }
}

static void flow_visit_member_declaration_list(struct flow_visit_ctx* ctx, struct member_declaration_list* p_member_declaration_list)
{
    struct member_declaration* p_member_declaration =
        p_member_declaration_list->head;
    while (p_member_declaration)
    {
        flow_visit_member_declaration(ctx, p_member_declaration);
        p_member_declaration = p_member_declaration->next;
    }
}

static void flow_visit_attribute_specifier(struct flow_visit_ctx* ctx, struct attribute_specifier* p_attribute_specifier)
{

}

static void flow_visit_attribute_specifier_sequence(struct flow_visit_ctx* ctx, struct attribute_specifier_sequence* p_visit_attribute_specifier_sequence)
{
    struct attribute_specifier* current = p_visit_attribute_specifier_sequence->head;
    while (current)
    {
        flow_visit_attribute_specifier(ctx, current);
        current = current->next;
    }
}

static void flow_visit_struct_or_union_specifier(struct flow_visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier)
{

    if (p_struct_or_union_specifier->attribute_specifier_sequence_opt)
        flow_visit_attribute_specifier_sequence(ctx, p_struct_or_union_specifier->attribute_specifier_sequence_opt);

    //struct struct_or_union_specifier* p_complete = get_complete_struct_or_union_specifier(p_struct_or_union_specifier);


    flow_visit_member_declaration_list(ctx, &p_struct_or_union_specifier->member_declaration_list);

}

static void flow_visit_enumerator(struct flow_visit_ctx* ctx, struct enumerator* p_enumerator)
{
    if (p_enumerator->constant_expression_opt)
        flow_visit_expression(ctx, p_enumerator->constant_expression_opt);

}

static void flow_visit_enumerator_list(struct flow_visit_ctx* ctx, struct enumerator_list* p_enumerator_list)
{
    struct enumerator* current = p_enumerator_list->head;
    while (current)
    {
        flow_visit_enumerator(ctx, current);
        current = current->next;
    }
}

static void flow_visit_enum_specifier(struct flow_visit_ctx* ctx, struct enum_specifier* p_enum_specifier)
{
    if (p_enum_specifier->attribute_specifier_sequence_opt)
    {
        flow_visit_attribute_specifier_sequence(ctx, p_enum_specifier->attribute_specifier_sequence_opt);
    }

    flow_visit_enumerator_list(ctx, &p_enum_specifier->enumerator_list);
}

static void flow_visit_typeof_specifier(struct flow_visit_ctx* ctx, struct typeof_specifier* p_typeof_specifier)
{
}

static void flow_visit_type_specifier(struct flow_visit_ctx* ctx, struct type_specifier* p_type_specifier)
{
    if (p_type_specifier->typeof_specifier)
    {
        flow_visit_typeof_specifier(ctx, p_type_specifier->typeof_specifier);
    }

    if (p_type_specifier->struct_or_union_specifier)
    {
        flow_visit_struct_or_union_specifier(ctx, p_type_specifier->struct_or_union_specifier);
    }

    if (p_type_specifier->enum_specifier)
    {
        flow_visit_enum_specifier(ctx, p_type_specifier->enum_specifier);
    }


    if (p_type_specifier->atomic_type_specifier)
    {
        //visit_deped(ctx, p_type_specifier->enum_specifier);
    }

}

static void flow_visit_type_specifier_qualifier(struct flow_visit_ctx* ctx, struct type_specifier_qualifier* p_type_specifier_qualifier)
{
    if (p_type_specifier_qualifier->type_qualifier)
    {
    }
    else if (p_type_specifier_qualifier->type_specifier)
    {
        flow_visit_type_specifier(ctx, p_type_specifier_qualifier->type_specifier);
    }
    else if (p_type_specifier_qualifier->alignment_specifier)
    {
    }
}

static void flow_visit_storage_class_specifier(struct flow_visit_ctx* ctx, struct storage_class_specifier* p_storage_class_specifier)
{
    if (p_storage_class_specifier->flags & STORAGE_SPECIFIER_AUTO)
    {

    }
}

static void flow_visit_declaration_specifier(struct flow_visit_ctx* ctx, struct declaration_specifier* p_declaration_specifier)
{

    if (p_declaration_specifier->function_specifier)
    {

    }


    if (p_declaration_specifier->storage_class_specifier)
    {
        flow_visit_storage_class_specifier(ctx, p_declaration_specifier->storage_class_specifier);

    }

    if (p_declaration_specifier->type_specifier_qualifier)
    {
        flow_visit_type_specifier_qualifier(ctx, p_declaration_specifier->type_specifier_qualifier);

    }

}

static void flow_visit_declaration_specifiers(struct flow_visit_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers,
    struct type* p_type_opt)
{
    /*
        * Se tiver typeof ou auto vamos apagar todos type specifiers.
        * e trocar por um novo
        * const typeof(int (*)()) a;
           //a = 1;
          auto p = (const typeof(int (*)())) 0;

          TODO esconder os type spefiver e qualifider , esconder auto.
          o resto tipo static deixar.

        */
        //

    struct declaration_specifier* p_declaration_specifier = p_declaration_specifiers->head;


    while (p_declaration_specifier)
    {
        flow_visit_declaration_specifier(ctx, p_declaration_specifier);
        p_declaration_specifier = p_declaration_specifier->next;
    }
}

/*
* retorna true se o ultimo item for um return
*/
static bool flow_is_last_item_return(struct compound_statement* p_compound_statement)
{
    if (p_compound_statement &&
        p_compound_statement->block_item_list.tail &&
        p_compound_statement->block_item_list.tail->unlabeled_statement &&
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement &&
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement->first_token &&
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement->first_token->type == TK_KEYWORD_RETURN)
    {
        return true;
    }
    return false;
}

void flow_visit_declaration(struct flow_visit_ctx* ctx, struct declaration* p_declaration)
{
    if (p_declaration->static_assert_declaration)
    {
        flow_visit_static_assert_declaration(ctx, p_declaration->static_assert_declaration);
    }


    if (p_declaration->p_attribute_specifier_sequence_opt)
    {
        flow_visit_attribute_specifier_sequence(ctx, p_declaration->p_attribute_specifier_sequence_opt);
    }


    if (p_declaration->declaration_specifiers)
    {
        if (p_declaration->init_declarator_list.head)
        {
            flow_visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers,
                &p_declaration->init_declarator_list.head->p_declarator->type);
        }
        else
        {
            flow_visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers, NULL);

        }

    }

    if (p_declaration->p_attribute_specifier_sequence_opt)
    {
    }

    if (p_declaration->init_declarator_list.head)
    {
        flow_visit_init_declarator_list(ctx, &p_declaration->init_declarator_list);
    }

    if (p_declaration->function_body)
    {

        assert(ctx->p_return_type == NULL);

        struct type type = get_function_return_type(&p_declaration->init_declarator_list.head->p_declarator->type);
        ctx->p_return_type = &type;

        //struct flow_defer_scope* p_defer = flow_visit_ctx_push_tail_block(ctx);
        //p_defer->p_statement = p_declaration->function_body;

        flow_visit_compound_statement(ctx, p_declaration->function_body);
        type_destroy(&type);
        ctx->p_return_type = NULL;

        //flow_visit_ctx_pop_tail_block(ctx);
    }

}


void flow_visit_function(struct flow_visit_ctx* ctx, struct declaration* p_declaration)
{
    assert(p_declaration->function_body);

    assert(ctx->tail_block == NULL);
    struct flow_defer_scope* p_defer = flow_visit_ctx_push_tail_block(ctx);
    p_defer->p_function_body = p_declaration->function_body;

    flow_visit_declaration(ctx, p_declaration);

    if (!flow_is_last_item_return(p_declaration->function_body))
    {
        check_defer_and_variables(ctx, p_defer, p_declaration->function_body->last_token);
    }

    flow_visit_ctx_pop_tail_block(ctx);
}

void flow_visit_ctx_destroy(struct flow_visit_ctx* obj_owner p)
{
    assert(p->tail_block == NULL);
}

void flow_analysis_visit(struct flow_visit_ctx* ctx)
{
    struct declaration* p_declaration = ctx->ast.declaration_list.head;
    while (p_declaration)
    {
        flow_visit_declaration(ctx, p_declaration);
        p_declaration = p_declaration->next;
    }
}

