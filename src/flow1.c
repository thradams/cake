/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma safety enable

#include "ownership.h"
#include <assert.h>
#include <string.h>
#include "flow1.h"
#include "expressions.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include "console.h"

static void flow1_check_dianostic_suppression(struct flow1_visit_ctx* ctx, struct token* p_token);
static void flow1_visit_unlabeled_statement(struct flow1_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement);
static void flow1_visit_static_assertion(struct flow1_visit_ctx* ctx, struct static_assertion* p_static_assertion);

struct flow1_object_visitor
{
    int member_index;
    struct type* p_type;
    struct flow1_object* p_object;
};

unsigned int s_visit_number = 1; //creates a unique number
_Opt struct flow1_object* _Opt arena_new_object(struct flow1_visit_ctx* ctx);

bool flow1_object_is_not_null(const struct flow1_object* p)
{
    enum flow1_state e = p->current.state;
    return (
             !(e & FLOW1_OBJECT_STATE_NULL) &&
             ((e & FLOW1_OBJECT_STATE_NOT_NULL) || (e & FLOW1_OBJECT_STATE_MOVED))
           );

}

bool flow1_object_can_be_not_null_or_moved(const struct flow1_object* p)
{
    enum flow1_state e = p->current.state;
    return (e & FLOW1_OBJECT_STATE_NOT_NULL) ||
        (e & FLOW1_OBJECT_STATE_MOVED);
}

bool flow1_object_is_null(const struct flow1_object* p)
{
    enum flow1_state e = p->current.state;
    return ((e & FLOW1_OBJECT_STATE_NULL) &&
           !(e & FLOW1_OBJECT_STATE_NOT_NULL) &&
           !(e & FLOW1_OBJECT_STATE_MOVED));
}

bool flow1_object_is_zero(const struct flow1_object* p)
{
    enum flow1_state e = p->current.state;
    return ((e & FLOW1_OBJECT_STATE_ZERO) &&
            !(e & FLOW1_OBJECT_STATE_NOT_ZERO));
}

bool flow1_object_is_not_zero(const struct flow1_object* p)
{
    enum flow1_state e = p->current.state;
    return (!(e & FLOW1_OBJECT_STATE_ZERO) &&
           (e & FLOW1_OBJECT_STATE_NOT_ZERO));
}


bool flow1_object_can_be_zero(const struct flow1_object* p)
{
    enum flow1_state e = p->current.state;

    return (e & FLOW1_OBJECT_STATE_ZERO);
}

bool flow1_object_can_be_moved(const struct flow1_object* p)
{
    enum flow1_state e = p->current.state;

    return (e & FLOW1_OBJECT_STATE_MOVED);
}

bool flow1_object_can_be_null(const struct flow1_object* p)
{
    enum flow1_state e = p->current.state;

    return (e & FLOW1_OBJECT_STATE_NULL);
}

bool flow1_object_is_uninitialized(const struct flow1_object* p)
{
    enum flow1_state e = p->current.state;
    return e == FLOW1_OBJECT_STATE_UNINITIALIZED;
}

bool flow1_object_can_be_uninitialized(const struct flow1_object* p)
{
    enum flow1_state e = p->current.state;
    return (e & FLOW1_OBJECT_STATE_UNINITIALIZED);
}

bool flow1_object_can_have_its_lifetime_ended(const struct flow1_object* p)
{
    enum flow1_state e = p->current.state;
    return (e & FLOW1_OBJECT_STATE_LIFE_TIME_ENDED);
}


static void flow1_object_state_to_string_core(enum flow1_state e, struct osstream* ss0)
{
    bool first = true;
    const char* separator = " ";// " | ";

    ss_fprintf(ss0, "\"");
    if (e & FLOW1_OBJECT_STATE_UNINITIALIZED)
    {
        if (first)
            first = false;
        else
            ss_fprintf(ss0, separator);
        ss_fprintf(ss0, "uninitialized");
    }

    if (e & FLOW1_OBJECT_STATE_NOT_NULL)
    {
        if (first)
            first = false;
        else
            ss_fprintf(ss0, separator);
        ss_fprintf(ss0, "not-null");
    }

    if (e & FLOW1_OBJECT_STATE_NULL)
    {
        if (first)
            first = false;
        else
            ss_fprintf(ss0, separator);
        ss_fprintf(ss0, "null");
    }

    if (e & FLOW1_OBJECT_STATE_ZERO)
    {
        if (first)
            first = false;
        else
            ss_fprintf(ss0, separator);
        ss_fprintf(ss0, "zero");
    }

    if (e & FLOW1_OBJECT_STATE_NOT_ZERO)
    {
        if (first)
            first = false;
        else
            ss_fprintf(ss0, separator);
        ss_fprintf(ss0, "not-zero");
    }


    if (e & FLOW1_OBJECT_STATE_LIFE_TIME_ENDED)
    {
        if (first)
            first = false;
        else
            ss_fprintf(ss0, separator);
        ss_fprintf(ss0, "lifetime-ended");
    }

    if (e & FLOW1_OBJECT_STATE_MOVED)
    {
        if (first)
            first = false;
        else
            ss_fprintf(ss0, separator);
        ss_fprintf(ss0, "moved");
    }

    ss_fprintf(ss0, "\"");

}
/* Forward declarations for functions defined later in this file */
void flow1_objects_view_destroy(_Dtor struct flow1_objects_view* p);
int  flow1_objects_view_reserve(struct flow1_objects_view* p, int n);
void flow1_objects_view_copy(struct flow1_objects_view* dest, const struct flow1_objects_view* source);
bool flow1_objects_view_find(const struct flow1_objects_view* p, const struct flow1_object* p_object);
void flow1_objects_destroy(_Dtor struct flow1_objects* p);
void flow1_objects_clear(struct flow1_objects* p);
int  flow1_objects_push_back(struct flow1_objects* p, struct flow1_object* _Owner p_object);
void flow1_object_destroy(_Dtor struct flow1_object* p);
void flow1_object_set_pointer(struct flow1_object* p_object, struct flow1_object* p_object2);
void flow1_object_push_states_from(const struct flow1_object* p_object_from, struct flow1_object* p_object_to);
void flow1_object_set_unknown(struct type* p_type, bool t_is_nullable, struct flow1_object* p_object, bool nullable_enabled);
struct flow1_object* _Opt flow1_make_flow_object(struct flow1_visit_ctx* ctx,
    struct type* p_type,
                                          const struct declarator* _Opt p_declarator_opt,
                                          const struct expression* _Opt p_expression_origin);

void flow1_object_state_delete(struct flow1_object_state* _Owner _Opt p)
{
    if (p)
    {
        flow1_objects_view_destroy(&p->alternatives);
        assert(p->next == NULL);
        free(p);
    }
}

void flow1_object_state_copy(struct flow1_object_state* to, const struct flow1_object_state* from)
{
    to->state = from->state;
    to->pointed = from->pointed;
    flow1_objects_view_copy(&to->alternatives, &from->alternatives);
}

void flow1_object_print_state(struct flow1_object* p, struct osstream* ss0)
{
    flow1_object_state_to_string_core(p->current.state, ss0);
}

void flow1_object_state_set_item_print(struct flow1_object_state* item, struct osstream* ss0)
{
    flow1_object_state_to_string_core(item->state, ss0);
}

bool flow1_object_is_expansible(const struct flow1_object* _Opt p_object)
{
    return (p_object &&
            p_object->current.state != FLOW1_OBJECT_STATE_NULL &&
                p_object->current.state != FLOW1_OBJECT_STATE_UNINITIALIZED &&
                p_object->current.pointed == 0);
}

void flow1_object_expand_pointer(struct flow1_visit_ctx* ctx, struct type* p_type, struct flow1_object* p_object)
{
    assert(type_is_pointer_or_array(p_type));

    if (flow1_object_is_expansible(p_object))
    {
        const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

        struct type t2 = type_remove_pointer(p_type);
        if (!type_is_void(&t2))
        {
            struct flow1_object* _Opt p_object2 = flow1_make_flow_object(ctx, &t2, p_object->p_declarator_origin, p_object->p_expression_origin);
            if (p_object2)
            {
                const bool is_nullable = type_is_opt(&t2, nullable_enabled);
                flow1_object_set_unknown(&t2, is_nullable, p_object2, nullable_enabled);
                flow1_object_set_pointer(p_object, p_object2);////obj.pointed2 = p_object;
                flow1_object_push_states_from(p_object, p_object2);
            }
            else
            {
                //ops
            }
        }
        type_destroy(&t2);
    }
}

void flow1_object_swap(struct flow1_object* a, struct flow1_object* b)
{
    struct flow1_object temp = *a;
    *a = *b;
    *b = temp;
}

void flow1_object_delete(struct flow1_object* _Owner _Opt p)
{
    if (p)
    {
        flow1_object_destroy(p);
        free(p);
    }
}

void flow1_object_set_pointer(struct flow1_object* p_object, struct flow1_object* p_object2)
{
    p_object->current.pointed = p_object2;
}

void flow1_object_destroy(_Dtor struct flow1_object* p)
{
    flow1_objects_view_destroy(&p->members);

    struct flow1_object_state* _Owner _Opt p_flow_object_state = p->current.next;
    while (p_flow_object_state)
    {
        struct flow1_object_state* _Owner _Opt temp = p_flow_object_state->next;
        p_flow_object_state->next = NULL;
        flow1_object_state_delete(p_flow_object_state);
        p_flow_object_state = temp;
    }
    flow1_objects_view_destroy(&p->current.alternatives);
}

void flow1_object_update_current(struct flow1_object* p)
{
    if (p->current.alternatives.size > 0)
    {
        p->current.state = 0;
        for (int i = 0; i < p->current.alternatives.size; i++)
        {
            //infinite recursion?
            flow1_object_update_current(p->current.alternatives.data[i]);
            p->current.state |= p->current.alternatives.data[i]->current.state;
        }
    }
}

void flow1_object_set_current_state_to_can_be_null(struct flow1_object* p)
{
    p->current.state |= FLOW1_OBJECT_STATE_NULL;
}

void flow1_object_set_current_state_to_is_null(struct flow1_object* p)
{
    p->current.state = FLOW1_OBJECT_STATE_NULL;
    if (p->current.alternatives.size > 0)
    {
        for (int i = 0; i < p->current.alternatives.size; i++)
        {
            flow1_object_set_current_state_to_can_be_null(p->current.alternatives.data[i]);
        }
    }
}

int flow1_object_add_state(struct flow1_object* p, struct flow1_object_state* _Owner pnew)
{
    assert(pnew->next == NULL);
    pnew->next = p->current.next;
    p->current.next = pnew;
    return 0;
}

void flow1_objects_view_destroy(_Dtor struct flow1_objects_view* p)
{
    free(p->data);
}

int flow1_objects_view_reserve(struct flow1_objects_view* p, int n)
{
    if (n > p->capacity)
    {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0]))))
        {
            return EOVERFLOW;
        }

        void* _Owner _Opt pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL) return ENOMEM;

        override_state(p->data, "moved"); //p->data was moved to pnew

        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}


int flow1_objects_view_push_back(struct flow1_objects_view* p, struct flow1_object* p_object)
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

        int error = flow1_objects_view_reserve(p, new_capacity);
        if (error != 0)
        {
            return error;
        }
    }

    p->data[p->size] = p_object;
    p->size++;

    return 0;
}

void flow1_objects_view_clear(struct flow1_objects_view* p)
{
    p->size = 0;
}

void flow1_objects_view_merge(struct flow1_objects_view* dest, const struct flow1_objects_view* source)
{
    for (int i = 0; i < source->size; i++)
    {
        if (!flow1_objects_view_find(dest, source->data[i]))
        {
            flow1_objects_view_push_back(dest, source->data[i]);
        }
    }
}

void flow1_objects_view_copy(struct flow1_objects_view* dest, const struct flow1_objects_view* source)
{
    flow1_objects_view_reserve(dest, source->size);
    for (int i = 0; i < source->size; i++)
    {
        dest->data[i] = source->data[i];
    }
    dest->size = source->size;
}

bool flow1_objects_view_find(const struct flow1_objects_view* p, const struct flow1_object* p_object)
{
    for (int i = 0; i < p->size; i++)
    {
        if (p->data[i] == p_object)
            return true;
    }
    return false;
}
void flow1_objects_destroy(_Dtor struct flow1_objects* p)
{
    for (int i = 0; i < p->size; i++)
    {
        flow1_object_delete(p->data[i]); //lint 30 (bug #436)

    }
    free(p->data);
}

void flow1_objects_clear(struct flow1_objects* p)
{
    for (int i = 0; i < p->size; i++)
    {
        flow1_object_delete(p->data[i]); //lint 30 (bug #436)
    }
    p->size = 0;
}

int flow1_objects_reserve(struct flow1_objects* p, int n)
{
    if (n > p->capacity)
    {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0]))))
        {
            return EOVERFLOW;
        }

        void* _Owner _Opt pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL) return ENOMEM;

        override_state(p->data, "moved"); //p->data was moved to pnew

        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

const struct flow1_object* _Opt flow1_objects_find(const struct flow1_objects* p_objects, const struct flow1_object* object)
{
    for (int i = 0; i < p_objects->size; i++)
    {
        if (p_objects->data[i] == object)
            return object;
    }
    return NULL;
}

int flow1_objects_push_back(struct flow1_objects* p, struct flow1_object* _Owner p_object)
{
    if (p->size == INT_MAX)
    {
        flow1_object_delete(p_object);
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

        int error = flow1_objects_reserve(p, new_capacity);
        if (error != 0)
        {
            flow1_object_delete(p_object);
            return error;
        }
        assert(p->data != NULL);
    }

    p->data[p->size] = p_object; /*moved*/


    p->size++;

    return 0;
}
struct flow1_object_name_list
{
    const char* name;
    struct flow1_object_name_list* _Opt previous;
};

bool flow1_has_name(const char* name, struct flow1_object_name_list* list)
{
    struct flow1_object_name_list* _Opt p = list;

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

struct flow1_object* _Opt flow1_make_object_core(struct flow1_visit_ctx* ctx,
    struct type* p_type,
    struct flow1_object_name_list* list,
    const struct declarator* _Opt p_declarator_opt,
    const struct expression* _Opt p_expression_origin)
{
    //assert((p_declarator_opt == NULL) != (p_expression_origin == NULL));
    if (p_declarator_opt == NULL)
    {
        //assert(p_expression_origin != NULL);
    }
    if (p_expression_origin == NULL)
    {
        // assert(p_declarator_opt != NULL);
    }


    struct flow1_object* _Opt p_object = arena_new_object(ctx);

    try
    {
        if (p_object == NULL) throw;

        p_object->p_expression_origin = p_expression_origin;
        p_object->p_declarator_origin = p_declarator_opt;

        if (p_type->struct_or_union_specifier)
        {
            struct struct_or_union_specifier* _Opt p_struct_or_union_specifier =
                get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

            if (p_struct_or_union_specifier)
            {
                p_object->current.state = FLOW1_OBJECT_STATE_NOT_APPLICABLE;

                struct member_declaration* _Opt p_member_declaration =
                    p_struct_or_union_specifier->member_declaration_list.head;

                struct flow1_object_name_list l = {
                  .name = p_struct_or_union_specifier->tag_name,
                  .previous = list
                };


                //int member_index = 0;
                while (p_member_declaration)
                {
                    if (p_member_declaration->member_declarator_list_opt)
                    {
                        struct member_declarator* _Opt p_member_declarator =
                            p_member_declaration->member_declarator_list_opt->head;

                        while (p_member_declarator)
                        {
                            if (p_member_declarator->declarator)
                            {
                                char* _Opt tag = NULL;
                                if (p_member_declarator->declarator->type.struct_or_union_specifier)
                                {
                                    tag = p_member_declarator->declarator->type.struct_or_union_specifier->tag_name;
                                }
                                else if (p_member_declarator->declarator->type.next &&
                                    p_member_declarator->declarator->type.next->struct_or_union_specifier)
                                {
                                    tag = p_member_declarator->declarator->type.next->struct_or_union_specifier->tag_name;

                                }

                                if (tag && flow1_has_name(tag, &l))
                                {
                                    struct flow1_object* _Opt member_obj = arena_new_object(ctx);
                                    if (member_obj == NULL)
                                    {
                                        throw;
                                    }

                                    member_obj->parent = p_object;

                                    member_obj->p_expression_origin = p_expression_origin;
                                    member_obj->p_declarator_origin = p_declarator_opt;
                                    member_obj->current.state = FLOW1_OBJECT_STATE_NOT_APPLICABLE;
                                    flow1_objects_view_push_back(&p_object->members, member_obj);
                                }
                                else
                                {
                                    struct flow1_object* _Opt p_member_obj =
                                        flow1_make_object_core(ctx,
                                            &p_member_declarator->declarator->type,
                                            &l,
                                            p_declarator_opt,
                                            p_expression_origin);

                                    if (p_member_obj == NULL)
                                    {
                                        throw;
                                    }
                                    p_member_obj->parent = p_object;
                                    flow1_objects_view_push_back(&p_object->members, p_member_obj);
                                }
                            }
                            p_member_declarator = p_member_declarator->next;
                        }
                    }
                    else if (p_member_declaration->specifier_qualifier_list != NULL)
                    {
                        if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                        {
                            struct type t = { 0 };
                            t.category = TYPE_CATEGORY_ITSELF;
                            t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                            t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;

                            struct flow1_object* _Opt member_obj = flow1_make_object_core(ctx, &t, &l, p_declarator_opt, p_expression_origin);
                            if (member_obj == NULL)
                            {
                                type_destroy(&t);
                                throw;
                            }

                            for (int k = 0; k < member_obj->members.size; k++)
                            {
                                flow1_objects_view_push_back(&p_object->members, member_obj->members.data[k]);
                                member_obj->members.data[k] = NULL;
                            }

                            type_destroy(&t);
                        }
                    }
                    p_member_declaration = p_member_declaration->next;
                }
                return p_object;
            }
            return p_object;
        }


        else if (type_is_array(p_type))
        {
            //flow: static arrays #435

            //p_object->state = flags;
            //if (p_object->members_size > 0)
            //{
            //    //not sure if we instanticate all items of array
            //    p_object->members[0].state = flags;
            //}
            //TODO create object for fixed len arrays!
            return p_object;
        }


        p_object->current.state = FLOW1_OBJECT_STATE_UNINITIALIZED;
    }
    catch
    {
        p_object = NULL;
    }
    return p_object;
}

struct flow1_object* _Opt flow1_make_flow_object(struct flow1_visit_ctx* ctx,
    struct type* p_type,
                           const struct declarator* _Opt p_declarator_opt,
                           const struct expression* _Opt p_expression_origin)
{

    struct flow1_object_name_list list = { .name = "" };
    struct flow1_object* _Opt p_object = flow1_make_object_core(ctx, p_type, &list, p_declarator_opt, p_expression_origin);

    return p_object;
}

struct token* _Opt flow1_object_get_token(const struct flow1_object* object)
{
    if (object->p_declarator_origin)
    {
        return object->p_declarator_origin->name_opt;
    }
    if (object->p_expression_origin)
    {
        return object->p_expression_origin->first_token;
    }
    return NULL;
}

void flow1_object_add_new_state_as_a_copy_of_current_state(struct flow1_object* object, const char* name, int state_number)
{
    _Opt struct flow1_object_state* _Owner _Opt pnew = calloc(1, sizeof * pnew);
    if (pnew == NULL) return;//ENOMEM;

    pnew->dbg_name = name;
    pnew->state_number = state_number;


    pnew->next = object->current.next;
    object->current.next = pnew;

    flow1_object_state_copy(pnew, &object->current);
    //TODO fix pnew is changed..
}

void flow1_object_remove_state(struct flow1_object* object, int state_number)
{
    struct flow1_object_state* previous = &object->current;
    struct flow1_object_state* _Opt it = object->current.next;
    while (it)
    {
        if (it->state_number == state_number)
        {
            struct flow1_object_state* _Owner _Opt p_it_next = it->next;
            it->next = NULL;
            flow1_object_state_delete(previous->next);
            previous->next = p_it_next;
            break;
        }
        previous = it;
        it = it->next;
    }

}

void flow1_print_object_core(bool color_enabled,
    int ident,
    struct flow1_object_visitor* p_visitor,
    const char* previous_names,
    bool is_pointer,
    bool short_version,
    unsigned int visit_number, struct osstream* ss0)
{
    if (p_visitor->p_object->visit_number == visit_number) return;
    p_visitor->p_object->visit_number = visit_number;

    if (p_visitor->p_type->struct_or_union_specifier)
    {
        assert(p_visitor->p_object->current.state == FLOW1_OBJECT_STATE_NOT_APPLICABLE);
        struct struct_or_union_specifier* _Opt p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier)
        {
            ss_fprintf(ss0, "%*c", ident + 1, ' ');
            ss_fprintf(ss0, "#%02d {", p_visitor->p_object->id);

            struct member_declaration* _Opt p_member_declaration =
                p_struct_or_union_specifier->member_declaration_list.head;


            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator* _Opt p_member_declarator =
                        p_member_declaration->member_declarator_list_opt->head;
                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            const char* name = p_member_declarator->declarator->name_opt ? p_member_declarator->declarator->name_opt->lexeme : "";

                            char buffer[200] = { 0 };
                            if (is_pointer)
                                snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                            else
                                snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);

                            _Opt struct flow1_object_visitor visitor = { 0 };
                            visitor.p_type = &p_member_declarator->declarator->type;
                            visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];

                            flow1_print_object_core(color_enabled, ident + 2, &visitor, buffer,
                                type_is_pointer(&p_member_declarator->declarator->type), short_version,
                                visit_number, ss0);

                            p_visitor->member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else if (p_member_declaration->specifier_qualifier_list != NULL)
                {
                    struct type t = { 0 };
                    t.category = TYPE_CATEGORY_ITSELF;
                    t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                    t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;


                    struct type* temp = p_visitor->p_type;
                    p_visitor->p_type = &t;

                    int visit_number0 = p_visitor->p_object->visit_number;
                    p_visitor->p_object->visit_number = 0;
                    flow1_print_object_core(color_enabled,
                        ident + 1,
                        p_visitor,
                        previous_names,
                        false,
                        short_version,
                        visit_number, ss0);

                    p_visitor->p_type = temp; //restore
                    p_visitor->p_object->visit_number = visit_number0;
                    type_destroy(&t);
                }
                p_member_declaration = p_member_declaration->next;
            }

            ss_fprintf(ss0, "%*c", ident + 1, ' ');
            ss_fprintf(ss0, "}");
        }
    }
    else if (type_is_pointer(p_visitor->p_type))
    {
        struct type t2 = type_remove_pointer(p_visitor->p_type);
        ss_fprintf(ss0, "%*c", ident, ' ');

        if (short_version)
        {
            ss_fprintf(ss0, "#%02d %s : ", p_visitor->p_object->id, previous_names);
            flow1_object_print_state(p_visitor->p_object, ss0);
            if (flow1_object_is_null(p_visitor->p_object))
            {
            }
            else if (flow1_object_can_be_uninitialized(p_visitor->p_object))
            {
            }
            else
            {
                ss_fprintf(ss0, " -> ");

                if (p_visitor->p_object->current.pointed != NULL)
                {
                    ss_fprintf(ss0, " #%02d", p_visitor->p_object->current.pointed->id);
                }
                else
                {
                    ss_fprintf(ss0, "{...}");
                }
            }
        }
        else
        {
            ss_fprintf(ss0, "%p:%s : ", p_visitor->p_object, previous_names);
            ss_fprintf(ss0, "{");

            struct flow1_object_state* _Opt it = p_visitor->p_object->current.next;
            while (it)
            {
                if (color_enabled)
                    ss_fprintf(ss0, LIGHTCYAN);

                ss_fprintf(ss0, "(#%02d %s)", it->state_number, it->dbg_name);
                flow1_object_state_set_item_print(it, ss0);
                if (color_enabled)
                    ss_fprintf(ss0, COLOR_RESET);
                ss_fprintf(ss0, ",");
                it = it->next;
            }
            //ss_fprintf(ss0, "*");
            if (color_enabled)
                ss_fprintf(ss0, LIGHTMAGENTA);
            ss_fprintf(ss0, "(current)");
            flow1_object_print_state(p_visitor->p_object, ss0);
            if (color_enabled)
                ss_fprintf(ss0, COLOR_RESET);
            ss_fprintf(ss0, "}");
        }


#if 0
        if (p_visitor->p_object->current.ref.size > 0)
        {
            char buffer[200] = { 0 };
            if (type_is_struct_or_union(&t2))
            {
                snprintf(buffer, sizeof buffer, "%s", previous_names);
            }
            else
            {
                snprintf(buffer, sizeof buffer, "*%s", previous_names);
            }

            for (int i = 0; i < p_visitor->p_object->current.ref.size; i++)
            {
                struct object_visitor visitor = { 0 };
                visitor.p_type = &t2;
                visitor.p_object = p_visitor->p_object->current.ref.data[i];
                print_object_core(ident + 1, &visitor, buffer, is_pointer, short_version, visit_number);
            }
        }
#endif
        type_destroy(&t2);
    }
    else
    {
        ss_fprintf(ss0, "%*c", ident, ' ');

        if (short_version)
        {
            ss_fprintf(ss0, "#%02d %s : ", p_visitor->p_object->id, previous_names);
            flow1_object_print_state(p_visitor->p_object, ss0);
        }
        else
        {
            ss_fprintf(ss0, "%p:%s : ", p_visitor->p_object, previous_names);
            ss_fprintf(ss0, "{");

            struct flow1_object_state* _Opt it = p_visitor->p_object->current.next;
            while (it)
            {
                ss_fprintf(ss0, "(#%02d %s)", it->state_number, it->dbg_name);
                flow1_object_state_set_item_print(it, ss0);
                ss_fprintf(ss0, ",");
                it = it->next;
            }


            flow1_object_print_state(p_visitor->p_object, ss0);
            ss_fprintf(ss0, "}");
        }
    }


}


void flow1_object_merge_state(struct flow1_object* pdest, struct flow1_object* object1, struct flow1_object* object2)
{
    pdest->current.state = object1->current.state | object2->current.state;

    if (pdest->members.size == object1->members.size &&
        object1->members.size == object2->members.size)
    {
        for (int i = 0; i < object1->members.size; i++)
        {
            struct flow1_object* m1 = object1->members.data[i];
            struct flow1_object* m2 = object2->members.data[i];
            flow1_object_merge_state(pdest->members.data[i], m1, m2);
        }
    }

#if 0
    for (int i = 0; i < object1->current.ref.size; i++)
    {
        struct flow1_object* pointed = object1->current.ref.data[i];
        if (pointed)
        {
            objects_view_push_back(&pdest->current.ref, pointed);
        }
    }

    for (int i = 0; i < object2->current.ref.size; i++)
    {
        struct flow1_object* pointed = object2->current.ref.data[i];
        if (pointed)
        {
            objects_view_push_back(&pdest->current.ref, pointed);
        }
    }

#endif


}


int flow1_object_restore_current_state_from_core(struct flow1_object* object, int state_number, unsigned int visit_number)
{
    if (object->visit_number == visit_number) return 1;//already visited    
    object->visit_number = visit_number;


    struct flow1_object_state* _Opt it = object->current.next;
    while (it)
    {
        if (it->state_number == state_number)
        {
            object->current.state = it->state;
#if 0
            objects_view_copy(&object->current.ref, &it->ref);
#endif
            break;
        }
        it = it->next;
    }

    for (int i = 0; i < object->members.size; i++)
    {
        flow1_object_restore_current_state_from_core(object->members.data[i], state_number, visit_number);
    }
#if 0
    for (int i = 0; i < object->current.ref.size; i++)
    {
        struct flow1_object* pointed = object->current.ref.data[i];

        if (pointed)
        {
            object_restore_current_state_from_core(pointed, state_number, visit_number);
        }
    }
#endif
    return 1;
}



int flow1_object_merge_current_state_with_state_number_core(struct flow1_object* object, int state_number, unsigned int visit_number, struct osstream* ss0)
{
    if (object->visit_number == visit_number)
    {
        return 1;//already visited
    }
    object->visit_number = visit_number;

    struct flow1_object_state* _Opt it = object->current.next;
    while (it)
    {
        if (it->state_number == state_number)
        {
            it->state |= object->current.state;
#if 0
            objects_view_merge(&it->ref, &object->current.ref);
#endif
            break;
        }
        it = it->next;
    }

    for (int i = 0; i < object->members.size; i++)
    {
        flow1_object_merge_current_state_with_state_number_core(object->members.data[i], state_number, visit_number, ss0);
    }
    if (object->current.state == FLOW1_OBJECT_STATE_NULL ||
        object->current.state == FLOW1_OBJECT_STATE_UNINITIALIZED) //moved
    {
        /*
        struct Y { int i; };
        struct X { struct Y y;  };

        struct X* _Opt _Owner create(struct Y* pY)
        {
            struct X* _Opt _Owner p  = malloc(sizeof * p);
            if (p)  {
                p->y = *pY;
            }
            else
            {
               //p is null, so the pointed object does not need to merge
            }
            return p;
        */
    }
    else
    {
#if 0
        for (int i = 0; i < object->current.ref.size; i++)
        {

            struct flow1_object* pointed = object->current.ref.data[i];
            if (pointed)
            {
                object_merge_current_state_with_state_number_core(pointed, state_number, visit_number);
            }

        }
#endif
    }
    return 1;
}


void flow1_object_get_name(const struct type* p_type,
    const struct flow1_object* p_object,
    char* outname,
    int out_size);


void flow1_print_flow_object(bool color_enabled, struct type* p_type, struct flow1_object* p_object, bool short_version, struct osstream* ss0)
{
    char name[100] = { 0 };
    flow1_object_get_name(p_type, p_object, name, sizeof name);

    _Opt struct flow1_object_visitor visitor = { 0 };
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    flow1_print_object_core(color_enabled, 0, &visitor, name, type_is_pointer(p_type), short_version, s_visit_number++, ss0);
}


void flow1_object_set_uninitialized_core(struct flow1_object_visitor* p_visitor)
{
    if (p_visitor->p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier* _Opt p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier)
        {
            struct member_declaration* _Opt p_member_declaration =
                p_struct_or_union_specifier->member_declaration_list.head;


            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator* _Opt p_member_declarator =
                        p_member_declaration->member_declarator_list_opt->head;

                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            if (p_visitor->member_index < p_visitor->p_object->members.size)
                            {
                                _Opt struct flow1_object_visitor visitor = { 0 };
                                visitor.p_type = &p_member_declarator->declarator->type;
                                visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];
                                flow1_object_set_uninitialized_core(&visitor);
                            }
                            p_visitor->member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else if (p_member_declaration->specifier_qualifier_list != NULL)
                {
                    if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                    {
                        struct type t = { 0 };
                        t.category = TYPE_CATEGORY_ITSELF;
                        t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                        t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;

                        struct type* type_temp = p_visitor->p_type;
                        p_visitor->p_type = &t; //new type, same object

                        flow1_object_set_uninitialized_core(p_visitor);

                        p_visitor->p_type = type_temp; //restore                         

                        type_destroy(&t);
                    }

                }
                p_member_declaration = p_member_declaration->next;
            }
            return;
        }
    }

    if (type_is_pointer(p_visitor->p_type))
    {
        p_visitor->p_object->current.state = FLOW1_OBJECT_STATE_UNINITIALIZED;

#if 0
        if (p_visitor->p_object->current.ref.size)
        {
            struct type t2 = type_remove_pointer(p_visitor->p_type);
            for (int i = 0; i < p_visitor->p_object->current.ref.size; i++)
            {
                struct flow1_object* pointed = p_visitor->p_object->current.ref.data[i];
                object_set_nothing(&t2, pointed);
            }
            type_destroy(&t2);
        }
#endif
    }
    else
    {
        p_visitor->p_object->current.state = FLOW1_OBJECT_STATE_UNINITIALIZED;
    }
}

void flow1_object_set_uninitialized(struct type* p_type, struct flow1_object* p_object)
{
    _Opt struct flow1_object_visitor visitor = { 0 };
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    flow1_object_set_uninitialized_core(&visitor);
}

static void flow1_checked_empty_core(struct flow1_visit_ctx* ctx,
    struct type* p_type,
    struct flow1_object* p_object,
    const char* previous_names,
    const struct marker* p_marker)
{
    if (p_type->struct_or_union_specifier && p_object->members.size > 0)
    {
        struct struct_or_union_specifier* _Opt p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        struct member_declaration* _Opt p_member_declaration =
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
                struct member_declarator* _Opt p_member_declarator =
                    p_member_declaration->member_declarator_list_opt->head;
                while (p_member_declarator)
                {

                    if (p_member_declarator->declarator)
                    {
                        const char* name = p_member_declarator->declarator->name_opt ? p_member_declarator->declarator->name_opt->lexeme : "";
                        char buffer[200] = { 0 };
                        if (type_is_pointer(p_type))
                            snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                        else
                            snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);


                        flow1_checked_empty_core(ctx, &p_member_declarator->declarator->type,
                            p_object->members.data[member_index],
                            name,
                            p_marker);

                        member_index++;
                    }
                    p_member_declarator = p_member_declarator->next;
                }
            }
            else if (p_member_declaration->specifier_qualifier_list != NULL)
            {
                //TODO!!!
            }
            p_member_declaration = p_member_declaration->next;
        }
        return;
    }

    if (type_is_owner_or_pointer_to_dtor(p_type))
    {
        if (p_object->current.state == (FLOW1_OBJECT_STATE_UNINITIALIZED | FLOW1_OBJECT_STATE_NULL | FLOW1_OBJECT_STATE_MOVED) ||
            p_object->current.state == (FLOW1_OBJECT_STATE_NULL | FLOW1_OBJECT_STATE_MOVED) ||
            p_object->current.state == FLOW1_OBJECT_STATE_NULL ||
            p_object->current.state == FLOW1_OBJECT_STATE_MOVED ||
            p_object->current.state == FLOW1_OBJECT_STATE_UNINITIALIZED ||
            p_object->current.state == (FLOW1_OBJECT_STATE_UNINITIALIZED | FLOW1_OBJECT_STATE_NULL) ||
            p_object->current.state == (FLOW1_OBJECT_STATE_UNINITIALIZED | FLOW1_OBJECT_STATE_MOVED)
            )
        {
        }
        else
        {

            diagnostic(W_FLOW_MISSING_DTOR,
                ctx->ctx,
                NULL,
                p_marker,
                "object '%s' may not be empty",
                previous_names);
        }
    }
}

void flow1_checked_empty(struct flow1_visit_ctx* ctx,
    struct type* p_type,
    struct flow1_object* p_object,
    const struct marker* p_marker)
{
    char name[100] = { 0 };
    flow1_object_get_name(p_type, p_object, name, sizeof name);

    flow1_checked_empty_core(ctx,
    p_type,
    p_object,
    name,
    p_marker);
}

static void flow1_object_set_moved_core(struct flow1_object_visitor* p_visitor)
{

    if (p_visitor->p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier* _Opt p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier)
        {
            struct member_declaration* _Opt p_member_declaration =
                p_struct_or_union_specifier->member_declaration_list.head;


            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator* _Opt p_member_declarator =
                        p_member_declaration->member_declarator_list_opt->head;

                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            if (p_visitor->member_index < p_visitor->p_object->members.size)
                            {
                                _Opt struct flow1_object_visitor visitor = { 0 };
                                visitor.p_type = &p_member_declarator->declarator->type;
                                visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];
                                flow1_object_set_moved_core(&visitor);
                            }
                            p_visitor->member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else if (p_member_declaration->specifier_qualifier_list != NULL)
                {
                    if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                    {
                        struct type t = { 0 };
                        t.category = TYPE_CATEGORY_ITSELF;
                        t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                        t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;

                        struct type* type_temp = p_visitor->p_type;
                        p_visitor->p_type = &t; //new type, same object

                        flow1_object_set_moved_core(p_visitor);

                        p_visitor->p_type = type_temp; //restore                         

                        type_destroy(&t);
                    }

                }
                p_member_declaration = p_member_declaration->next;
            }
            return;
        }
    }

    if (type_is_pointer(p_visitor->p_type))
    {
        p_visitor->p_object->current.state = FLOW1_OBJECT_STATE_MOVED;
    }
    else
    {
        p_visitor->p_object->current.state = FLOW1_OBJECT_STATE_MOVED;
    }
}

void flow1_object_set_moved(struct type* p_type, struct flow1_object* p_object)
{
    _Opt struct flow1_object_visitor visitor = { 0 };
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    flow1_object_set_moved_core(&visitor);
}

static void flow1_object_set_unknown_core(struct flow1_object_visitor* p_visitor, bool t_is_nullable, unsigned int visit_number, bool nullable_enabled)
{
    if (p_visitor->p_object->visit_number == visit_number) return; //already visited
    p_visitor->p_object->visit_number = visit_number;


    if (p_visitor->p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier* _Opt p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier)
        {
            struct member_declaration* _Opt p_member_declaration =
                p_struct_or_union_specifier->member_declaration_list.head;


            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator* _Opt p_member_declarator =
                        p_member_declaration->member_declarator_list_opt->head;

                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            if (p_visitor->member_index < p_visitor->p_object->members.size)
                            {
                                _Opt struct flow1_object_visitor visitor = { 0 };
                                visitor.p_type = &p_member_declarator->declarator->type;
                                visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];

                                flow1_object_set_unknown_core(&visitor,
                                    t_is_nullable,
                                    visit_number,
                                    nullable_enabled);
                            }
                            else
                            {
                                //TODO BUG union?                                
                            }
                            p_visitor->member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else if (p_member_declaration->specifier_qualifier_list != NULL)
                {
                    if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                    {
                        struct type t = { 0 };
                        t.category = TYPE_CATEGORY_ITSELF;
                        t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                        t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;

                        struct type* temp = p_visitor->p_type;
                        p_visitor->p_type = &t;

                        int vn = p_visitor->p_object->visit_number;
                        p_visitor->p_object->visit_number = 0;
                        flow1_object_set_unknown_core(p_visitor, t_is_nullable, visit_number, nullable_enabled);



                        //restore
                        p_visitor->p_object->visit_number = vn;
                        p_visitor->p_type = temp;

                        type_destroy(&t);
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
            return;
        }
        return;
    }

    if (type_is_pointer(p_visitor->p_type))
    {
        if (t_is_nullable || type_is_opt(p_visitor->p_type, nullable_enabled))
            p_visitor->p_object->current.state = FLOW1_OBJECT_STATE_NULL | FLOW1_OBJECT_STATE_NOT_NULL;
        else
            p_visitor->p_object->current.state = FLOW1_OBJECT_STATE_NOT_NULL;


        struct flow1_object* _Opt pointed = p_visitor->p_object->current.pointed;
        if (pointed)
        {
            struct type t2 = type_remove_pointer(p_visitor->p_type);
            bool t2_is_nullable = type_is_opt(&t2, nullable_enabled);
            _Opt struct flow1_object_visitor visitor = { 0 };
            visitor.p_type = &t2;
            visitor.p_object = pointed;

            flow1_object_set_unknown_core(&visitor, t2_is_nullable, visit_number, nullable_enabled);
            type_destroy(&t2);
        }

    }
    else
    {
        if (!type_is_struct_or_union(p_visitor->p_type))
            p_visitor->p_object->current.state = FLOW1_OBJECT_STATE_ZERO | FLOW1_OBJECT_STATE_NOT_ZERO;
    }
}

void flow1_object_set_unknown(struct type* p_type, bool t_is_nullable, struct flow1_object* p_object, bool nullable_enabled)
{
    _Opt struct flow1_object_visitor visitor = { 0 };
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    flow1_object_set_unknown_core(&visitor, t_is_nullable, s_visit_number++, nullable_enabled);
}


static void flow1_object_set_deleted_core(struct type* p_type, struct flow1_object* p_object, unsigned int visit_number)
{
    if (p_object->visit_number == visit_number) return; //already visited
    p_object->visit_number = visit_number;


    if (p_type->struct_or_union_specifier && p_object->members.size > 0)
    {
        struct struct_or_union_specifier* _Opt p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier)
        {
            struct member_declaration* _Opt p_member_declaration =
                p_struct_or_union_specifier->member_declaration_list.head;

            int member_index = 0;
            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator* _Opt p_member_declarator =
                        p_member_declaration->member_declarator_list_opt->head;

                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            if (member_index < p_object->members.size)
                            {
                                flow1_object_set_deleted_core(&p_member_declarator->declarator->type, p_object->members.data[member_index], visit_number);
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
                else if (p_member_declaration->specifier_qualifier_list != NULL)
                {
                    //TODO!!!
                }
                p_member_declaration = p_member_declaration->next;
            }
            return;
        }
        return;
    }

    if (type_is_pointer(p_type))
    {
        p_object->current.state = FLOW1_OBJECT_STATE_LIFE_TIME_ENDED;
#if 0
        for (int i = 0; i < p_object->current.ref.size; i++)
        {
            struct flow1_object* pointed = p_object->current.ref.data[i];
            if (pointed)
            {
                struct type t2 = type_remove_pointer(p_type);
                object_set_deleted_core(&t2, pointed, visit_number);
                type_destroy(&t2);
            }
        }
#endif
    }
    else
    {
        if (!type_is_struct_or_union(p_type))
            p_object->current.state = FLOW1_OBJECT_STATE_LIFE_TIME_ENDED;
    }
}

void flow1_object_set_can_be_uninitialized(struct flow1_object* p_object)
{
    p_object->current.state |= FLOW1_OBJECT_STATE_UNINITIALIZED;
}
void flow1_object_set_is_unitialized(struct flow1_object* p_object)
{
    p_object->current.state = FLOW1_OBJECT_STATE_UNINITIALIZED;
    if (p_object->current.alternatives.size > 0)
    {
        for (int i = 0; i < p_object->current.alternatives.size; i++)
        {
            flow1_object_set_can_be_uninitialized(p_object->current.alternatives.data[i]);
        }
    }
}

void flow1_object_set_can_be_moved(struct flow1_object* p_object)
{
    if (p_object->current.state & FLOW1_OBJECT_STATE_NOT_NULL)
    {
        p_object->current.state &= ~FLOW1_OBJECT_STATE_NOT_NULL;
        p_object->current.state |= FLOW1_OBJECT_STATE_MOVED;
        p_object->current.state |= FLOW1_OBJECT_STATE_MOVED;
    }
}
void flow1_object_set_is_moved(struct flow1_object* p_object)
{
    p_object->current.state = FLOW1_OBJECT_STATE_MOVED;

    if (p_object->current.alternatives.size > 0)
    {
        for (int i = 0; i < p_object->current.alternatives.size; i++)
        {
            flow1_object_set_can_be_moved(p_object->current.alternatives.data[i]);
        }
    }
}


void flow1_object_set_deleted(struct type* p_type, struct flow1_object* p_object)
{
    flow1_object_set_deleted_core(p_type, p_object, s_visit_number++);
}

void flow1_object_set_zero_core(struct flow1_object_visitor* p_visitor)
{
    if (p_visitor->p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier* _Opt p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier)
        {
            struct member_declaration* _Opt p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;

            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator* _Opt p_member_declarator =
                        p_member_declaration->member_declarator_list_opt->head;

                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            if (p_visitor->member_index < p_visitor->p_object->members.size)
                            {
                                _Opt struct flow1_object_visitor visitor = { 0 };
                                visitor.p_type = &p_member_declarator->declarator->type;
                                visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];
                                flow1_object_set_zero_core(&visitor);
                            }
                            p_visitor->member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else if (p_member_declaration->specifier_qualifier_list != NULL)
                {
                    if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                    {
                        struct type t = { 0 };
                        t.category = TYPE_CATEGORY_ITSELF;
                        t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                        t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;

                        struct type* temp = p_visitor->p_type;
                        p_visitor->p_type = &t;
                        flow1_object_set_zero_core(p_visitor);
                        p_visitor->p_type = temp; //restore
                        type_destroy(&t);
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
            return;
        }
        return;
    }

    if (type_is_pointer(p_visitor->p_type))
    {
        p_visitor->p_object->current.state = FLOW1_OBJECT_STATE_NULL;
    }
    else
    {
        p_visitor->p_object->current.state = FLOW1_OBJECT_STATE_ZERO;
    }
}

void flow1_object_set_zero(struct type* p_type, struct flow1_object* p_object)
{
    _Opt struct flow1_object_visitor visitor = { 0 };
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    flow1_object_set_zero_core(&visitor);
}

void flow1_object_set_end_of_lifetime_core(struct flow1_object_visitor* p_visitor)
{
    if (p_visitor->p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier* _Opt p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier)
        {
            struct member_declaration* _Opt p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;

            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator* _Opt p_member_declarator =
                        p_member_declaration->member_declarator_list_opt->head;

                    while (p_member_declarator)
                    {
                        if (p_member_declarator->declarator)
                        {
                            if (p_visitor->member_index < p_visitor->p_object->members.size)
                            {
                                _Opt struct flow1_object_visitor visitor = { 0 };
                                visitor.p_type = &p_member_declarator->declarator->type;
                                visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];
                                flow1_object_set_end_of_lifetime_core(&visitor);
                            }
                            p_visitor->member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else if (p_member_declaration->specifier_qualifier_list != NULL)
                {
                    if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                    {
                        struct type t = { 0 };
                        t.category = TYPE_CATEGORY_ITSELF;
                        t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                        t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;

                        struct type* temp = p_visitor->p_type;
                        p_visitor->p_type = &t;
                        flow1_object_set_end_of_lifetime_core(p_visitor);
                        p_visitor->p_type = temp; //restore
                        type_destroy(&t);
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
            return;
        }
        return;
    }

    p_visitor->p_object->current.state = FLOW1_OBJECT_STATE_LIFE_TIME_ENDED;
}

void flow1_object_set_end_of_lifetime(struct type* p_type, struct flow1_object* p_object)
{
    _Opt struct flow1_object_visitor visitor = { 0 };
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    flow1_object_set_end_of_lifetime_core(&visitor);
}

//returns true if all parts that need to be moved weren't moved.
bool flow1_object_check(struct type* p_type, struct flow1_object* p_object)
{
    try
    {
        if (p_type->type_qualifier_flags & TYPE_QUALIFIER_CAKE_VIEW)
        {
            return false;
        }

        if (!type_is_owner_or_pointer_to_dtor(p_type))
        {
            return false;
        }

        if (p_type->struct_or_union_specifier && p_object->members.size > 0)
        {
            struct struct_or_union_specifier* _Opt p_struct_or_union_specifier =
                get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

            if (p_struct_or_union_specifier == NULL)
            {
                throw;
            }

            struct member_declaration* _Opt p_member_declaration =
                p_struct_or_union_specifier->member_declaration_list.head;
            int possible_need_destroy_count = 0;
            int need_destroy_count = 0;
            int member_index = 0;
            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator* _Opt p_member_declarator =
                        p_member_declaration->member_declarator_list_opt->head;
                    while (p_member_declarator)
                    {

                        if (p_member_declarator->declarator)
                        {
                            if (type_is_owner(&p_member_declarator->declarator->type))
                            {
                                possible_need_destroy_count++;
                            }

                            if (flow1_object_check(&p_member_declarator->declarator->type,
                                p_object->members.data[member_index]))
                            {
                                need_destroy_count++;
                            }
                            member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else if (p_member_declaration->specifier_qualifier_list != NULL)
                {
                    //TODO struct object_visitor* p_visitor                
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
                should_had_been_moved = (p_object->current.state & FLOW1_OBJECT_STATE_NOT_NULL);
            }
            else
            {
                if (p_object->current.state == FLOW1_OBJECT_STATE_UNINITIALIZED ||
                    (p_object->current.state & FLOW1_OBJECT_STATE_MOVED) ||
                    p_object->current.state == FLOW1_OBJECT_STATE_NOT_NULL ||
                    p_object->current.state == (FLOW1_OBJECT_STATE_UNINITIALIZED))
                {
                }
                else
                {
                    should_had_been_moved = true;
                }
            }

            return should_had_been_moved;
        }
    }
    catch
    {
    }
    return false;
}

void flow1_object_get_name_core(const struct type* p_type,
    const struct flow1_object* p_object,
    const struct flow1_object* p_object_target,
    const char* previous_names,
    char* outname,
    int out_size,
    unsigned int visit_number)
{
    try
    {
        if (p_object->visit_number == visit_number) return;
        ((struct flow1_object*)p_object)->visit_number = visit_number;

        if (p_object == p_object_target)
        {
            snprintf(outname, out_size, "%s", previous_names);
            return;
        }

        if (p_type->struct_or_union_specifier && p_object->members.size > 0)
        {
            struct struct_or_union_specifier* _Opt p_struct_or_union_specifier =
                get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

            if (p_struct_or_union_specifier == NULL)
                throw;

            struct member_declaration* _Opt p_member_declaration =
                p_struct_or_union_specifier->member_declaration_list.head;

            int member_index = 0;
            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator* _Opt p_member_declarator =
                        p_member_declaration->member_declarator_list_opt->head;
                    while (p_member_declarator)
                    {

                        if (p_member_declarator->declarator)
                        {
                            const char* name = p_member_declarator->declarator->name_opt ? p_member_declarator->declarator->name_opt->lexeme : "";
                            char buffer[200] = { 0 };
                            if (type_is_pointer(p_type))
                                snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                            else
                                snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);

                            flow1_object_get_name_core(
                                &p_member_declarator->declarator->type,
                                p_object->members.data[member_index],
                                p_object_target,
                                buffer,
                                outname,
                                out_size, visit_number);

                            member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else if (p_member_declaration->specifier_qualifier_list != NULL)
                {
                    // assert(false); //TODO
                }
                p_member_declaration = p_member_declaration->next;
            }

        }
        else
        {
            snprintf(outname, out_size, "%s", previous_names);
        }
    }
    catch
    {
    }
}


void flow1_object_get_name(const struct type* p_type,
    const struct flow1_object* p_object,
    char* outname,
    int out_size)
{
    outname[0] = '\0';

    if (p_object->p_declarator_origin)
    {
        const char* root_name = p_object->p_declarator_origin->name_opt ? p_object->p_declarator_origin->name_opt->lexeme : "?";
        const struct flow1_object* _Opt root = p_object->p_declarator_origin->p_flow_object;
        if (root)
        {
            flow1_object_get_name_core(&p_object->p_declarator_origin->type, root, p_object, root_name, outname, out_size, s_visit_number++);
        }
        else
        {
            assert(false);
        }
    }
    else if (p_object->p_expression_origin)
    {
        int bytes_written = 0;
        struct token* _Opt p = p_object->p_expression_origin->first_token;
        for (int i = 0; i < 10; i++)
        {
            const char* ps = p->lexeme;
            while (*ps)
            {
                if (bytes_written < (out_size - 1))
                {
                    outname[bytes_written] = *ps;
                }
                bytes_written++;
                ps++;
            }

            if (p == p_object->p_expression_origin->last_token)
                break;

            p = p->next;
            if (p == NULL)break;
        }

        if (bytes_written < (out_size - 1))
            outname[bytes_written] = '\0';
        else
            outname[out_size - 1] = '\0';
    }
    else
    {
        outname[0] = '?';
        outname[1] = '\0';
    }
}

static void flow1_checked_read_object_core(struct flow1_visit_ctx* ctx,
    struct flow1_object_visitor* p_visitor,
    bool is_nullable,
    const struct token* _Opt position_token_opt,
    const struct marker* _Opt p_marker_opt,
    bool check_pointed_object,
    const char* previous_names,
    unsigned int visit_number)
{
    assert(previous_names != NULL);

    if (p_visitor->p_object->visit_number == visit_number) return;
    p_visitor->p_object->visit_number = visit_number;


    if (p_visitor->p_type->struct_or_union_specifier && p_visitor->p_object->members.size > 0)
    {
        struct struct_or_union_specifier* _Opt p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);

        struct member_declaration* _Opt p_member_declaration =
            p_struct_or_union_specifier ?
            p_struct_or_union_specifier->member_declaration_list.head :
            NULL;

        /*
        *  Some parts of the object needs to be moved..
        *  we need to print error one by one
        */
        while (p_member_declaration)
        {
            if (p_member_declaration->member_declarator_list_opt)
            {
                struct member_declarator* _Opt p_member_declarator =
                    p_member_declaration->member_declarator_list_opt->head;
                while (p_member_declarator)
                {

                    if (p_member_declarator->declarator)
                    {
                        const char* name =
                            p_member_declarator->declarator->name_opt ? p_member_declarator->declarator->name_opt->lexeme : "?";

                        char buffer[200] = { 0 };
                        if (type_is_pointer(p_visitor->p_type))
                            snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                        else
                            snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);

                        _Opt struct flow1_object_visitor  visitor = { 0 };
                        visitor.p_type = &p_member_declarator->declarator->type;
                        visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];


                        flow1_checked_read_object_core(ctx,
                            &visitor,
                            is_nullable,
                            position_token_opt,
                            p_marker_opt,
                            check_pointed_object,
                            buffer,
                            visit_number);

                        p_visitor->member_index++;
                    }
                    p_member_declarator = p_member_declarator->next;
                }
            }
            else if (p_member_declaration->specifier_qualifier_list)
            {
                struct type t = { 0 };
                t.category = TYPE_CATEGORY_ITSELF;
                t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;

                char buffer[200] = { 0 };
                if (type_is_pointer(p_visitor->p_type))
                    snprintf(buffer, sizeof buffer, "%s->", previous_names);
                else
                    snprintf(buffer, sizeof buffer, "%s.", previous_names);

                struct type* temp = p_visitor->p_type;
                p_visitor->p_type = &t;

                int visit_number0 = p_visitor->p_object->visit_number;
                p_visitor->p_object->visit_number = 0;

                flow1_checked_read_object_core(ctx,
                        p_visitor,
                        is_nullable,
                        position_token_opt,
                        p_marker_opt,
                        check_pointed_object,
                        buffer,
                        visit_number);

                p_visitor->p_type = temp; //restore
                p_visitor->p_object->visit_number = visit_number0;
                type_destroy(&t);

            }
            p_member_declaration = p_member_declaration->next;
        }
        return;
    }
    else
    {

        if (type_is_pointer(p_visitor->p_type) &&
            !is_nullable &&
            !type_is_opt(p_visitor->p_type, ctx->ctx->options.null_checks_enabled) &&
            flow1_object_can_be_null(p_visitor->p_object))
        {
            diagnostic(W_FLOW_NULL_DEREFERENCE,
                ctx->ctx,
                NULL,
                p_marker_opt,
                "non-nullable pointer '%s' may be null",
                previous_names);
        }

        if (type_is_pointer(p_visitor->p_type) &&
            check_pointed_object &&
            flow1_object_can_be_not_null_or_moved(p_visitor->p_object))
        {
            struct type t2 = type_remove_pointer(p_visitor->p_type);

            if (p_visitor->p_object->current.pointed)
            {
                _Opt struct flow1_object_visitor  visitor = { 0 };
                visitor.p_type = &t2;
                visitor.p_object = p_visitor->p_object->current.pointed;

                flow1_checked_read_object_core(ctx,
                    &visitor,
                    is_nullable,
                    position_token_opt,
                    p_marker_opt,
                    true,
                    previous_names,
                    visit_number);
            }

            type_destroy(&t2);
        }


        if (p_visitor->p_object->current.state & FLOW1_OBJECT_STATE_UNINITIALIZED)
        {
            if (type_is_array(p_visitor->p_type))
            {
                //unitialized arrays are used as initialized pointers to uninitialized flow1_objects.
            }
            else
            {
                diagnostic(W_FLOW_UNINITIALIZED,
                    ctx->ctx,
                    position_token_opt,
                    p_marker_opt,
                    "uninitialized object '%s'",
                    previous_names);
            }
        }


        //TODO there is some problem with  FLOW_OBJECT_STATE_LIFE_TIME_ENDED
        //state somewhere!
        if (p_visitor->p_object->current.state & FLOW1_OBJECT_STATE_LIFE_TIME_ENDED)
        {
            /*
               current implementation does not handle
               "dependent states". Many times the pointer is null
               and the pointed object does not exist anymore.
               But these two states are not related and it is causing
               too many false positives
            */

            //diagnostic(W_FLOW_LIFETIME_ENDED,
            //    ctx->ctx,
            //     position_token_opt,
            //    p_marker_opt,
            //    "lifetime ended '%s'",
            //    previous_names);
        }


    }
}

/*
  Check if all flow1_objects connect to this one
   - are initialized
*/
void flow1_checked_read_object(struct flow1_visit_ctx* ctx,
    struct type* p_type,
    bool is_nullable,
    struct flow1_object* p_object,
    const struct token* _Opt position_token,
    const struct marker* _Opt p_marker_opt,
    bool check_pointed_object)
{
    const char* _Owner _Opt s = NULL;
    char name[200] = { 0 };

    flow1_object_get_name(p_type, p_object, name, sizeof name);

    _Opt struct flow1_object_visitor visitor = { 0 };
    visitor.p_object = p_object;
    visitor.p_type = p_type;

    flow1_checked_read_object_core(ctx,
    &visitor,
    is_nullable,
    position_token,
    p_marker_opt,
    check_pointed_object,
    name,
    s_visit_number++);
    free((void* _Owner)s);
}

static void flow1_end_of_block_visit_core(struct flow1_visit_ctx* ctx,
    struct flow1_object_visitor* p_visitor,
    bool b_type_is_view,
    const struct token* position_token,
    const char* previous_names,
    unsigned int visit_number)
{

    if (p_visitor->p_object->visit_number == visit_number) return;
    p_visitor->p_object->visit_number = visit_number;


    if (p_visitor->p_type->struct_or_union_specifier && p_visitor->p_object->members.size > 0)
    {
        struct struct_or_union_specifier* _Opt p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier == NULL)
            return;

        struct member_declaration* _Opt p_member_declaration =
            p_struct_or_union_specifier->member_declaration_list.head;

        if (flow1_object_check(p_visitor->p_type, p_visitor->p_object))
        {
            /*
            *  All parts of the object needs to be moved, so instead of
            *  describing each part we will just say that the object should
            *  have been moved.
            */
            const struct token* _Opt name = flow1_object_get_token(p_visitor->p_object);
            if (diagnostic(W_FLOW_MISSING_DTOR,
                ctx->ctx,
                name, NULL,
                "members of '%s' were not released.",
                previous_names))
            {

                if (p_visitor->p_object->p_declarator_origin)
                    diagnostic(W_LOCATION, ctx->ctx, position_token, NULL, "end of '%s' scope", previous_names);
            }
        }
        else
        {
            /*
            *  Some parts of the object needs to be moved..
            *  we need to print error one by one
            */


            while (p_member_declaration)
            {
                if (p_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator* _Opt p_member_declarator =
                        p_member_declaration->member_declarator_list_opt->head;
                    while (p_member_declarator)
                    {

                        if (p_member_declarator->declarator)
                        {
                            const char* name = p_member_declarator->declarator->name_opt ? p_member_declarator->declarator->name_opt->lexeme : "?";

                            char buffer[200] = { 0 };
                            if (type_is_pointer(p_visitor->p_type))
                                snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                            else
                                snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);

                            const bool member_is_view = type_is_view(&p_member_declarator->declarator->type);

                            _Opt struct flow1_object_visitor visitor = { 0 };
                            visitor.p_type = &p_member_declarator->declarator->type;
                            visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];

                            flow1_end_of_block_visit_core(ctx,
                                &visitor,
                                b_type_is_view || member_is_view,
                                position_token,
                                buffer,
                                visit_number);
                            p_visitor->member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else if (p_member_declaration->specifier_qualifier_list != NULL)
                {
                    struct type t = { 0 };
                    t.category = TYPE_CATEGORY_ITSELF;
                    t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                    t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;

                    struct type* temp = p_visitor->p_type;
                    p_visitor->p_type = &t;

                    const bool member_is_view = type_is_view(&t);

                    int visit_number0 = p_visitor->p_object->visit_number;
                    p_visitor->p_object->visit_number = 0;
                    flow1_end_of_block_visit_core(ctx,
                        p_visitor,
                        member_is_view,
                        position_token,
                        previous_names,
                        visit_number);

                    p_visitor->p_type = temp; //restore
                    p_visitor->p_object->visit_number = visit_number0;
                    type_destroy(&t);

                }
                p_member_declaration = p_member_declaration->next;
            }
        }


    }
    else
    {
        const char* name = previous_names;
        const struct token* _Opt position = NULL;
        if (p_visitor->p_object->p_declarator_origin)
            position = p_visitor->p_object->p_declarator_origin->name_opt ? p_visitor->p_object->p_declarator_origin->name_opt : p_visitor->p_object->p_declarator_origin->first_token_opt;
        else if (p_visitor->p_object->p_expression_origin)
            position = p_visitor->p_object->p_expression_origin->first_token;
        else
        {
            assert(false);
        }

        if (name[0] == '\0')
        {
            /*function arguments without name*/
            name = "?";
        }
        //bool should_had_been_moved = false;


        /*
           Despite the name FLOW_OBJECT_STATE_NOT_NULL does not means null, it means
           the reference is not referring an object, the value could be -1 for instance.
        */
        if (type_is_pointer(p_visitor->p_type) &&
            !b_type_is_view &&
            type_is_owner(p_visitor->p_type) &&
            p_visitor->p_object->current.state & FLOW1_OBJECT_STATE_NOT_NULL)
        {

            bool show_warning = true;

            if (p_visitor->p_object->p_declarator_origin &&
                p_visitor->p_object->p_declarator_origin->direct_declarator &&
                p_visitor->p_object->p_declarator_origin->direct_declarator->p_attribute_specifier_sequence)
            {

                enum attribute_flags  attributes_flags =
                    p_visitor->p_object->p_declarator_origin->direct_declarator->p_attribute_specifier_sequence->attributes_flags;

                if (attributes_flags & CAKE_ATTRIBUTE_LEAK)
                    show_warning = false;
            }

            if (show_warning &&
                diagnostic(W_FLOW_MISSING_DTOR,
                    ctx->ctx,
                    position, NULL,
                    "object referenced by owner '%s' was not released.", previous_names))
            {
                diagnostic(W_LOCATION,
                ctx->ctx,
                position_token, NULL,
                "end of '%s' lifetime", previous_names);
            }
        }
        else if (!b_type_is_view && type_is_pointed_dtor(p_visitor->p_type) && type_is_pointer(p_visitor->p_type))
        {
            char buffer[100] = { 0 };
            snprintf(buffer, sizeof buffer, "%s", previous_names);
            struct type t2 = type_remove_pointer(p_visitor->p_type);

            if (p_visitor->p_object->current.pointed)
            {
                _Opt struct flow1_object_visitor visitor = { 0 };
                visitor.p_type = &t2;
                visitor.p_object = p_visitor->p_object->current.pointed;
                if (position)
                {
                    flow1_end_of_block_visit_core(ctx, &visitor, b_type_is_view, position, buffer, visit_number);
                }
                else
                {
                    assert(false);
                }
            }


            type_destroy(&t2);
        }
        else if (type_is_owner(p_visitor->p_type) && !type_is_pointer(p_visitor->p_type))
        {
            //non-pointer _Owner
            if (p_visitor->p_object->current.state == FLOW1_OBJECT_STATE_UNINITIALIZED ||
                p_visitor->p_object->current.state == FLOW1_OBJECT_STATE_NULL ||
                p_visitor->p_object->current.state == FLOW1_OBJECT_STATE_MOVED)
            {
            }
            else
            {
                bool show_warning = true;

                if (p_visitor->p_object->p_declarator_origin &&
                    p_visitor->p_object->p_declarator_origin->direct_declarator &&
                    p_visitor->p_object->p_declarator_origin->direct_declarator->p_attribute_specifier_sequence)
                {

                    enum attribute_flags  attributes_flags =
                        p_visitor->p_object->p_declarator_origin->direct_declarator->p_attribute_specifier_sequence->attributes_flags;

                    if (attributes_flags & CAKE_ATTRIBUTE_LEAK)
                        show_warning = false;
                }


                if (show_warning &&
                    diagnostic(W_FLOW_MISSING_DTOR,
                        ctx->ctx,
                        position, NULL,
                        "object referenced by owner '%s' was not released.", previous_names))
                {
                    diagnostic(W_LOCATION,
                    ctx->ctx,
                    position_token, NULL,
                    "end of '%s' lifetime", previous_names);
                }
            }
        }
        else if (type_is_pointer(p_visitor->p_type))
        {
            if (p_visitor->p_type->storage_class_specifier_flags & STORAGE_SPECIFIER_PARAMETER)
            {
                if (type_is_owner_or_pointer_to_dtor(p_visitor->p_type))
                {
                    //owner pointer parameters can point to deleted objects, so 
                    //we cannot check this state inside checked_read_object
                }
                else
                {
                    //Visiting a pointer parameter. We check if we didn't mess a external object
                //TODO static flow1_objects
                    struct type t2 = type_remove_pointer(p_visitor->p_type);

                    if (p_visitor->p_object->current.pointed &&
                        p_visitor->p_object->p_declarator_origin)
                    {
                        struct token* _Opt name_token = p_visitor->p_object->p_declarator_origin->name_opt ?
                            p_visitor->p_object->p_declarator_origin->name_opt :
                            p_visitor->p_object->p_declarator_origin->first_token_opt;

                        flow1_checked_read_object(ctx,
                         &t2,
                         false,
                         p_visitor->p_object->current.pointed,
                         name_token,
                         NULL,
                         true);
                    }
                    type_destroy(&t2);
                }
            }
        }
        else
        {
        }

    }
}

void flow1_end_of_block_visit(struct flow1_visit_ctx* ctx,
    struct type* p_type,
    bool type_is_view,
    struct flow1_object* p_object,
    const struct token* position_token,
    const char* previous_names)
{
    _Opt struct flow1_object_visitor visitor = { 0 };
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    flow1_end_of_block_visit_core(ctx,
    &visitor,
    type_is_view,
    position_token,
    previous_names,
    s_visit_number++);
}

bool flow1_object_is_zero_or_null(const struct flow1_object* p_object)
{
    return (p_object->current.state == FLOW1_OBJECT_STATE_NULL) ||
        (p_object->current.state == FLOW1_OBJECT_STATE_ZERO);
}

/*
   This function must check and do the flow assignment of
   a = b
*/
static void flow1_assignment_core(struct flow1_visit_ctx* ctx,
    const struct token* error_position,
    const struct marker* p_a_marker,
    const struct marker* p_b_marker,
    enum assigment_type assigment_type,
    bool check_uninitialized_b,
    bool a_type_is_view,
    bool a_type_is_nullable,
    struct flow1_object_visitor* p_visitor_a,
    struct flow1_object_visitor* p_visitor_b,
    bool* _Opt  set_argument_to_unkown)
{
    //const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

#ifdef _DEBUG
    while (error_position->line == 25)
    {
        break;
    }
#endif
    // ss_fprintf(ss0, "line  %d\n", error_position->line);
     //type_print(p_a_type);
     //ss_fprintf(ss0, " = ");
     //type_print(p_b_type);
     //ss_fprintf(ss0, "\n");

     /*general check for copying uninitialized object*/


    if (check_uninitialized_b &&
        flow1_object_can_be_uninitialized(p_visitor_b->p_object))
    {
        //a = b where b can be uninitialized

        if (type_is_array(p_visitor_b->p_type))
        {
            if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
            {
                struct type item_type = { 0 };

                if (type_is_array(p_visitor_a->p_type))
                    item_type = get_array_item_type(p_visitor_a->p_type);
                else
                    item_type = type_remove_pointer(p_visitor_a->p_type);

                const bool cannot_be_uninitialized =
                    (ctx->ctx->options.ownership_enabled && !type_is_ctor(&item_type)) ||
                    type_is_const(&item_type);

                if (cannot_be_uninitialized)
                {
                    char b_object_name[100] = { 0 };
                    flow1_object_get_name(p_visitor_b->p_type, p_visitor_b->p_object, b_object_name, sizeof b_object_name);
                    diagnostic(W_FLOW_UNINITIALIZED,
                                ctx->ctx,
                                NULL,
                                p_b_marker,
                        "uninitialized object '%s' passed to non-optional parameter", b_object_name);
                }

                type_destroy(&item_type);
            }
        }
        else
        {
            char b_object_name[100] = { 0 };
            flow1_object_get_name(p_visitor_b->p_type, p_visitor_b->p_object, b_object_name, sizeof b_object_name);

            if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
            {
                diagnostic(W_FLOW_UNINITIALIZED,
                            ctx->ctx,
                            NULL,
                            p_b_marker,
                    "passing an uninitialized argument '%s' object", b_object_name);
            }
            else if (assigment_type == ASSIGMENT_TYPE_RETURN)
            {
                diagnostic(W_FLOW_UNINITIALIZED,
                            ctx->ctx,
                            NULL,
                            p_b_marker,
                            "returning an uninitialized '%s' object", b_object_name);
            }
            else
            {
                diagnostic(W_FLOW_UNINITIALIZED,
                            ctx->ctx,
                            NULL,
                            p_b_marker,
                            "reading an uninitialized '%s' object", b_object_name);
            }
        }

        return;
    }

    if (check_uninitialized_b && flow1_object_can_have_its_lifetime_ended(p_visitor_a->p_object))
    {
        //a = b where a was deleted
        char buffer[100] = { 0 };
        flow1_object_get_name(p_visitor_a->p_type, p_visitor_a->p_object, buffer, sizeof buffer);

        diagnostic(W_FLOW_LIFETIME_ENDED,
                    ctx->ctx,
                    NULL,
                    p_a_marker,
                    "The object '%s' may have been deleted or its lifetime have ended.", buffer);


        return;
    }

    /*general check passing possible null to non _Opt*/
    if (type_is_pointer(p_visitor_a->p_type) &&
        (!type_is_opt(p_visitor_a->p_type, ctx->ctx->options.null_checks_enabled)) &&
        flow1_object_can_be_null(p_visitor_b->p_object))
    {
        if (!a_type_is_nullable)
        {
            char buffer[100] = { 0 };
            flow1_object_get_name(p_visitor_b->p_type, p_visitor_b->p_object, buffer, sizeof buffer);

            if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
            {
                diagnostic(W_FLOW_NULLABLE_TO_NON_NULLABLE,
                       ctx->ctx,
                       NULL,
                       p_b_marker,
                       "passing a possible null pointer '%s' to non-nullable pointer parameter", buffer);
            }
            else if (assigment_type == ASSIGMENT_TYPE_RETURN)
            {
                diagnostic(W_FLOW_NULLABLE_TO_NON_NULLABLE,
                       ctx->ctx,
                       NULL,
                       p_b_marker,
                       "returning a possible null pointer '%s' to non-nullable pointer", buffer);
            }
            else
            {
                diagnostic(W_FLOW_NULLABLE_TO_NON_NULLABLE,
                       ctx->ctx,
                       NULL,
                       p_b_marker,
                       "assignment of possible null pointer '%s' to non-nullable pointer", buffer);
            }
        }
    }

    if (type_is_pointer(p_visitor_a->p_type))
    {
        if (!a_type_is_view && type_is_owner(p_visitor_a->p_type))
        {
            flow1_checked_empty(ctx, p_visitor_a->p_type, p_visitor_a->p_object, p_a_marker);
        }

        if (flow1_object_is_zero_or_null(p_visitor_b->p_object))
        {
            if (type_is_array(p_visitor_b->p_type))
            {
                p_visitor_a->p_object->current.state = FLOW1_OBJECT_STATE_NOT_NULL;
                return;
            }
            else if (type_is_nullptr_t(p_visitor_b->p_type) || type_is_integer(p_visitor_b->p_type))
            {
                flow1_object_set_current_state_to_is_null(p_visitor_a->p_object);

                return;
            }
        }
    }

    if (!a_type_is_view && type_is_pointed_dtor(p_visitor_a->p_type) && type_is_pointer(p_visitor_a->p_type))
    {
        flow1_checked_empty(ctx, p_visitor_a->p_type, p_visitor_a->p_object, p_a_marker);

        if (flow1_object_is_zero_or_null(p_visitor_b->p_object))
        {
            //0 to objec_owner??
            //a = nullpr
            //flow_object_set_zero(p_a_type, p_a_object);
            return;
        }
    }

    /*copying to void * _Owner*/
    if (type_is_void_ptr(p_visitor_a->p_type) && type_is_pointer(p_visitor_b->p_type))
    {
        p_visitor_a->p_object->current.state = p_visitor_b->p_object->current.state;

        if (!a_type_is_view && type_is_owner(p_visitor_a->p_type))
        {
            /*
            *  Moving an already moved object
            */
            if (flow1_object_can_be_moved(p_visitor_b->p_object))
            {
                diagnostic(W_FLOW_MOVED,
                                            ctx->ctx,
                                            NULL,
                                            p_b_marker,
                                            "object may be already moved");
            }

            //*b must be empty before copying to void* _Owner
            struct type t = type_remove_pointer(p_visitor_b->p_type);


            if (p_visitor_b->p_object->current.pointed == NULL)
            {
                // The question is..if we had this object expanded
                // could it possible have resources?
                //-> {...}
                if (flow1_object_is_expansible(p_visitor_b->p_object) &&
                    type_is_owner(&t))
                {
                    //if the anwser is yes then we need a warning
                    diagnostic(W_FLOW_MISSING_DTOR,
                                                ctx->ctx,
                                                NULL,
                                                p_a_marker,
                                                "pointed object may be not empty");
                }
            }
            else
            {
                flow1_checked_empty(ctx, &t, p_visitor_b->p_object->current.pointed, p_b_marker);
                flow1_object_set_deleted(&t, p_visitor_b->p_object->current.pointed);
            }

            type_destroy(&t);

            if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
            {
                flow1_object_set_is_unitialized(p_visitor_b->p_object);
                p_visitor_b->p_object->current.state = FLOW1_OBJECT_STATE_UNINITIALIZED;
            }
            else
            {
                flow1_object_set_is_moved(p_visitor_b->p_object);
            }
        }
        return;
    }


    if (type_is_pointer(p_visitor_a->p_type) && type_is_pointer(p_visitor_b->p_type))
    {
        p_visitor_a->p_object->current.state = p_visitor_b->p_object->current.state;
        p_visitor_a->p_object->current.pointed = p_visitor_b->p_object->current.pointed;

        struct type t = type_remove_pointer(p_visitor_a->p_type);

        /*if the parameter points to _Ctor object, then we don´t need to check
          argument pointed object.
        */
        const bool checked_pointed_object_read =
            ctx->ctx->options.ownership_enabled && !type_is_ctor(&t);

        bool is_nullable = a_type_is_nullable || type_is_opt(&t, ctx->ctx->options.null_checks_enabled);

        flow1_checked_read_object(ctx,
            p_visitor_b->p_type,
            is_nullable,
            p_visitor_b->p_object,
            error_position,
            p_b_marker,
            checked_pointed_object_read);


        //object_copy_state(p_a_type, p_a_object, p_b_type, p_b_object);
        type_destroy(&t);

        if (!a_type_is_view && type_is_owner(p_visitor_a->p_type))
        {
            /*
               T * _Owner pA = pB;
            */

            if (flow1_object_can_be_moved(p_visitor_b->p_object))
            {
                //TODO we need 2 positions, source, dest
                diagnostic(W_FLOW_MOVED,
                   ctx->ctx,
                   error_position, NULL,
                   "source object has already been moved");
            }


            if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
            {
                p_visitor_b->p_object->current.state = FLOW1_OBJECT_STATE_UNINITIALIZED;
                if (p_visitor_b->p_object->current.pointed)
                {
                    struct flow1_object* pointed = p_visitor_b->p_object->current.pointed;

                    struct type t2 = type_remove_pointer(p_visitor_b->p_type);
                    flow1_object_set_deleted(&t2, pointed);
                    type_destroy(&t2);
                }
            }
            else
            {

                //TODO
                p_visitor_a->p_object->current.state = p_visitor_a->p_object->current.state & ~FLOW1_OBJECT_STATE_MOVED;
                if (p_visitor_b->p_object->current.state & FLOW1_OBJECT_STATE_NOT_NULL)
                {
                    //null not-null -> null moved
                    p_visitor_b->p_object->current.state &= ~FLOW1_OBJECT_STATE_NOT_NULL;
                    p_visitor_b->p_object->current.state |= FLOW1_OBJECT_STATE_MOVED;
                }
            }
        }
        else if (!a_type_is_view && type_is_pointed_dtor(p_visitor_a->p_type))
        {
            if (type_is_owner_or_pointer_to_dtor(p_visitor_b->p_type))
            {
                if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
                {
                    if (p_visitor_b->p_object->current.pointed)
                    {
                        struct flow1_object* pointed = p_visitor_b->p_object->current.pointed;

                        struct type t2 = type_remove_pointer(p_visitor_b->p_type);
                        flow1_object_set_uninitialized(&t2, pointed);
                        type_destroy(&t2);
                    }

                }
                else
                    flow1_object_set_moved(p_visitor_b->p_type, p_visitor_b->p_object);
            }
            else
            {
                if (p_visitor_b->p_type->address_of)
                {
                    //must be address of.
                    if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
                    {
                        p_visitor_b->p_object->current.state = FLOW1_OBJECT_STATE_UNINITIALIZED;

                        if (p_visitor_b->p_object->current.pointed)
                        {
                            struct flow1_object* pointed = p_visitor_b->p_object->current.pointed;

                            struct type t2 = type_remove_pointer(p_visitor_b->p_type);
                            flow1_object_set_uninitialized(&t2, pointed);
                            type_destroy(&t2);
                        }
                    }
                    else
                        flow1_object_set_moved(p_visitor_b->p_type, p_visitor_b->p_object);
                }
                else
                {
                    //avoid error on top of error
                    //address error already emitted
                    //at this point
                }
            }

        }
        else
        {
            if (a_type_is_view || !type_is_owner(p_visitor_a->p_type))
            {
                p_visitor_a->p_object->current.state = p_visitor_b->p_object->current.state;
            }


            if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
            {
                struct type t3 = type_remove_pointer(p_visitor_a->p_type);
                if (!type_is_const(&t3))
                {
                    if (p_visitor_b->p_object->current.pointed)
                    {
                        if (set_argument_to_unkown)
                        {
                            //Tells the caller it must make argument unknown
                            *set_argument_to_unkown = true;
                        }
                        //   flow1_object_set_unknown(&t3, t3_is_nullable, pointed, nullable_enabled);
                    }
                }
                type_destroy(&t3);
            }
        }

        return;
    }

    if (p_visitor_a->p_type->struct_or_union_specifier && p_visitor_a->p_object->members.size > 0)
    {
        struct struct_or_union_specifier* _Opt p_a_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_visitor_a->p_type->struct_or_union_specifier);

        if (p_visitor_b->p_type->struct_or_union_specifier == NULL)
            return;

        struct struct_or_union_specifier* _Opt p_b_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_visitor_b->p_type->struct_or_union_specifier);

        if (p_a_struct_or_union_specifier && p_b_struct_or_union_specifier)
        {
            struct member_declaration* _Opt p_a_member_declaration =
                p_a_struct_or_union_specifier->member_declaration_list.head;

            struct member_declaration* _Opt p_b_member_declaration =
                p_b_struct_or_union_specifier->member_declaration_list.head;


            while (p_a_member_declaration && p_b_member_declaration)
            {
                if (p_a_member_declaration->member_declarator_list_opt &&
                    p_b_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator* _Opt p_a_member_declarator =
                        p_a_member_declaration->member_declarator_list_opt->head;

                    struct member_declarator* _Opt p_b_member_declarator =
                        p_b_member_declaration->member_declarator_list_opt->head;

                    while (p_a_member_declarator && p_b_member_declarator)
                    {
                        if (p_a_member_declarator->declarator &&
                            p_b_member_declarator->declarator)
                        {
                            if (p_visitor_a->member_index < p_visitor_a->p_object->members.size &&
                                p_visitor_b->member_index < p_visitor_b->p_object->members.size)
                            {
                                _Opt struct flow1_object_visitor visitor_a = { 0 };
                                visitor_a.p_type = &p_a_member_declarator->declarator->type;
                                visitor_a.p_object = p_visitor_a->p_object->members.data[p_visitor_a->member_index];


                                _Opt struct flow1_object_visitor visitor_b = { 0 };
                                visitor_b.p_type = &p_b_member_declarator->declarator->type;
                                visitor_b.p_object = p_visitor_b->p_object->members.data[p_visitor_b->member_index];


                                flow1_assignment_core(ctx,
                                    error_position,
                                    p_a_marker,
                                    p_b_marker,
                                    assigment_type,
                                    check_uninitialized_b,
                                    a_type_is_view,
                                    a_type_is_nullable,
                                    &visitor_a,
                                    &visitor_b,
                                    set_argument_to_unkown);
                            }
                            else
                            {
                                //TODO BUG union?                                
                            }
                            p_visitor_a->member_index++;
                            p_visitor_b->member_index++;

                        }
                        p_a_member_declarator = p_a_member_declarator->next;
                        p_b_member_declarator = p_b_member_declarator->next;
                    }
                }
                else
                {
                    if (p_a_member_declaration->specifier_qualifier_list &&
                       p_a_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                    {
                        struct type t = { 0 };
                        t.category = TYPE_CATEGORY_ITSELF;
                        t.struct_or_union_specifier = p_a_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                        t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;

                        struct type* temp1 = p_visitor_a->p_type;
                        struct type* temp2 = p_visitor_b->p_type;
                        p_visitor_a->p_type = &t;
                        p_visitor_b->p_type = &t;

                        flow1_assignment_core(
                                            ctx,
                                            error_position,
                                            p_a_marker,
                                            p_b_marker,
                                            assigment_type,
                                            check_uninitialized_b,
                                            a_type_is_view,
                                            a_type_is_nullable,
                                            p_visitor_a,
                                            p_visitor_b,
                                            set_argument_to_unkown);

                        //restore
                        p_visitor_a->p_type = temp1;
                        p_visitor_b->p_type = temp2;

                        type_destroy(&t);
                    }
                }
                p_a_member_declaration = p_a_member_declaration->next;
                p_b_member_declaration = p_b_member_declaration->next;
            }
            return;
        }
    }

    p_visitor_a->p_object->current.state = p_visitor_b->p_object->current.state;
    if (!a_type_is_view && type_is_owner(p_visitor_a->p_type))
    {
        if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
            flow1_object_set_uninitialized(p_visitor_b->p_type, p_visitor_b->p_object);
        else
            flow1_object_set_moved(p_visitor_b->p_type, p_visitor_b->p_object);
    }
}


struct flow1_object* _Opt  flow1_expression_get_flow_object(struct flow1_visit_ctx* ctx, struct expression* p_expression, bool nullable_enabled)
{
    try
    {
        if (p_expression->expression_type == EXPR_PRIMARY_DECLARATOR)
        {
            assert(p_expression->declarator);
            if (p_expression->declarator->p_alias_of_expression)
            {
                /*We need to original object*/
                return flow1_expression_get_flow_object(ctx, p_expression->declarator->p_alias_of_expression, nullable_enabled);
            }
            else
            {
                assert(p_expression->declarator != NULL);

                if (p_expression->declarator->declaration_specifiers &&
                    !is_automatic_variable(p_expression->declarator->declaration_specifiers->storage_class_specifier_flags))
                {
                    assert(p_expression->declarator->p_flow_object != NULL);

                    //External flow1_objects are added to the arena on-demand
                    if (flow1_objects_find(&ctx->arena, p_expression->declarator->p_flow_object) == NULL)
                    {
                        p_expression->declarator->p_flow_object = flow1_make_flow_object(ctx, &p_expression->declarator->type, p_expression->declarator, NULL);
                        if (p_expression->declarator->p_flow_object == NULL)
                            throw;

                        flow1_object_set_unknown(&p_expression->declarator->type,
                                                type_is_opt(&p_expression->declarator->type, ctx->ctx->options.null_checks_enabled),
                                                p_expression->declarator->p_flow_object,
                                                ctx->ctx->options.null_checks_enabled);
                    }
                }
                return p_expression->declarator->p_flow_object;
            }
        }

        else if (p_expression->expression_type == EXPR_UNARY_ADDRESSOF)
        {
            assert(p_expression->right != NULL);

            struct flow1_object* _Opt p_object = flow1_make_flow_object(ctx, &p_expression->type, NULL, p_expression);
            if (p_object == NULL) throw;

            struct flow1_object* _Opt p_object_pointed =
                flow1_expression_get_flow_object(ctx, p_expression->right, nullable_enabled);

            if (p_object_pointed)
                flow1_object_set_pointer(p_object, p_object_pointed);

            p_object->current.state = FLOW1_OBJECT_STATE_NOT_NULL;
            p_object->is_temporary = true;
            return p_object;
        }
        else if (p_expression->expression_type == EXPR_PRIMARY_PARENTHESIS)
        {
            assert(p_expression->right != NULL);
            return flow1_expression_get_flow_object(ctx, p_expression->right, nullable_enabled);
        }
        else if (p_expression->expression_type == EXPR_PRIMARY_STATEMENT_EXPRESSION)
        {
            assert(p_expression->compound_statement);

            struct expression* _Opt p_last_expression = NULL;
            struct block_item* _Opt  p = p_expression->compound_statement->block_item_list.head;
            while (p)
            {
                if (p->next == NULL &&
                    p->unlabeled_statement &&
                    p->unlabeled_statement->expression_statement &&
                    p->unlabeled_statement->expression_statement->expression_opt)
                {
                    p_last_expression = p->unlabeled_statement->expression_statement->expression_opt;
                }
                p = p->next;
            }

            if (p_last_expression)
                return flow1_expression_get_flow_object(ctx, p_last_expression, nullable_enabled);
        }
        else if (p_expression->expression_type == EXPR_CAST)
        {
            assert(p_expression->left != NULL);
            struct flow1_object* _Opt p = flow1_expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
            if (p)
            {
                if (type_is_pointer(&p_expression->type_name->type))
                {
                    //casting from 0 to pointer we need to change the zero to null
                    //#define NULL ((void*)0)
                    if (p->current.state & FLOW1_OBJECT_STATE_ZERO)
                    {
                        p->current.state &= ~FLOW1_OBJECT_STATE_ZERO;
                        p->current.state |= FLOW1_OBJECT_STATE_NULL;
                    }
                    if (p->current.state & FLOW1_OBJECT_STATE_NOT_ZERO)
                    {
                        p->current.state &= ~FLOW1_OBJECT_STATE_NOT_ZERO;
                        p->current.state |= FLOW1_OBJECT_STATE_NOT_NULL;
                    }
                }
            }
            return p;
        }
        else if (p_expression->expression_type == EXPR_POSTFIX_DOT)
        {
            assert(p_expression->left != NULL);

            struct flow1_object* _Opt p_obj = flow1_expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
            if (p_obj)
            {
                if (p_expression->member_index < p_obj->members.size)
                    return p_obj->members.data[p_expression->member_index];
                else
                {
                    // assert(false);
                    return NULL;
                }
            }
        }
        else if (p_expression->expression_type == EXPR_POSTFIX_ARRAY)
        {
            assert(p_expression->left != NULL);

            //All arrays items point to the same object.
            struct flow1_object* _Opt p_obj = flow1_expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
            if (p_obj)
            {

                if (p_obj->current.pointed == NULL)
                {
                    flow1_object_expand_pointer(ctx, &p_expression->left->type, p_obj);
                }

                if (p_obj->current.pointed)
                {
                    struct flow1_object* pointed = p_obj->current.pointed;
                    return pointed;
                }
#if 0
                if (p_obj->current.ref.size == 1)
                {
                    struct flow1_object* pointed = p_obj->current.ref.data[0];
                    return pointed;
                }
                else
                {
                    struct flow1_object* p_object = make_flow_object(ctx, &p_expression->type, NULL, p_expression);
                    object_set_nothing(&p_expression->type, p_object);
                    for (int i = 0; i < p_obj->current.ref.size; i++)
                    {
                        struct flow1_object* pointed = p_obj->current.ref.data[i];
                        if (pointed != NULL)
                        {
                            if (p_expression->member_index < pointed->members.size)
                            {
                                p_object->current.state |=
                                    pointed->members.data[p_expression->member_index]->current.state;
                                objects_view_merge(&p_object->current.ref, &pointed->members.data[p_expression->member_index]->current.ref);
                                //return pointed->members.data[p_expression->member_index];
                            }
                            else
                            {
                                //return NULL;
                            }
                        }
                    }
                    return p_object;
                }
#endif
            }
            return NULL;
        }
        else if (p_expression->expression_type == EXPR_POSTFIX_ARROW)
        {
            assert(p_expression->left != NULL);

            struct flow1_object* _Opt p_obj = flow1_expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
            if (p_obj)
            {
                if (p_obj->current.pointed == 0)
                {
                    flow1_object_expand_pointer(ctx, &p_expression->left->type, p_obj);
                }

                struct flow1_object* _Opt pointed = p_obj->current.pointed;

                if (pointed == NULL ||
                    p_expression->member_index >= pointed->members.size)
                {
                    //ops!
                    return NULL;
                }

                struct flow1_object* _Opt p_obj2 = pointed->members.data[p_expression->member_index];

                p_obj2->p_declarator_origin = NULL;
                p_obj2->p_expression_origin = p_expression;
                return p_obj2;
            }
            return NULL;
        }
        else if (p_expression->expression_type == EXPR_UNARY_CONTENT)
        {
            assert(p_expression->right != NULL);

            struct flow1_object* _Opt p_obj = flow1_expression_get_flow_object(ctx, p_expression->right, nullable_enabled);
            if (p_obj)
            {
                if (p_obj->current.pointed == NULL)
                {
                    flow1_object_expand_pointer(ctx, &p_expression->right->type, p_obj);
                }

                if (p_obj->current.pointed != NULL)
                {
                    return p_obj->current.pointed;
                }
            }
            return p_obj;
        }
        else if (p_expression->expression_type == EXPR_POSTFIX_FUNCTION_CALL)
        {
            struct flow1_object* _Opt p_object = flow1_make_flow_object(ctx, &p_expression->type, NULL, p_expression);
            if (p_object == NULL) throw;

            const bool is_nullable = type_is_opt(&p_expression->type, nullable_enabled);
            flow1_object_set_unknown(&p_expression->type, is_nullable, p_object, nullable_enabled);
            p_object->is_temporary = true;

            if (type_is_pointer(&p_expression->type) && flow1_object_is_expansible(p_object))
            {
                flow1_object_expand_pointer(ctx, &p_expression->type, p_object);
            }


            return p_object;
        }
        else if (p_expression->expression_type == EXPR_POSTFIX_COMPOUND_LITERAL)
        {
            assert(p_expression->type_name != NULL);
            return p_expression->type_name->abstract_declarator->p_flow_object;
        }
        else if (p_expression->expression_type == EXPR_PRIMARY_STRING_LITERAL)
        {
            struct flow1_object* _Opt p_object = flow1_make_flow_object(ctx, &p_expression->type, NULL, p_expression);
            if (p_object == NULL) throw;

            p_object->current.state = FLOW1_OBJECT_STATE_NOT_NULL;
            return p_object;
        }
        else if (p_expression->expression_type == EXPR_PRIMARY_PREDEFINED_CONSTANT)
        {
            struct flow1_object* _Opt p_object = flow1_make_flow_object(ctx, &p_expression->type, NULL, p_expression);
            if (p_object == NULL) throw;

            if (p_expression->type.type_specifier_flags == TYPE_SPECIFIER_NULLPTR_T)
            {
                p_object->current.state = FLOW1_OBJECT_STATE_NULL;
            }
            else
            {
                if (object_has_constant_value(&p_expression->object))
                {
                    bool not_zero = object_is_true(&p_expression->object);
                    p_object->current.state = not_zero ? FLOW1_OBJECT_STATE_NOT_ZERO : FLOW1_OBJECT_STATE_ZERO;
                }
            }

            return p_object;
        }
        else if (p_expression->expression_type == EXPR_ASSIGNMENT_ASSIGN)
        {
            assert(p_expression->left != NULL);

            struct flow1_object* _Opt p_obj = flow1_expression_get_flow_object(ctx, p_expression->left, nullable_enabled);

            //
            //
            return p_obj;
        }
        else if (p_expression->expression_type == EXPR_CONDITIONAL)
        {
            assert(p_expression->right != NULL);

            struct flow1_object* _Opt p_object = flow1_make_flow_object(ctx, &p_expression->type, NULL, p_expression);
            if (p_object == NULL) throw;

            /* Elvis (left == NULL): reuse condition's flow object as the true branch */
            struct flow1_object* _Opt p_obj1 =
                flow1_expression_get_flow_object(ctx,
                    p_expression->left ? p_expression->left
                                       : p_expression->condition_expr,
                    nullable_enabled); //lint 35 (bug #440)

            struct flow1_object* _Opt p_obj2 = flow1_expression_get_flow_object(ctx, p_expression->right, nullable_enabled);

            if (p_obj1 && p_obj2)
            {
                flow1_object_merge_state(p_object, p_obj1, p_obj2);
            }

            return p_object;
        }
        else if (p_expression->expression_type == EXPR_CHECKED)
        {
            struct flow1_object* _Opt p_object = flow1_make_flow_object(ctx, &p_expression->type, NULL, p_expression);
            if (p_object == NULL) throw;
            p_object->current.state = FLOW1_OBJECT_STATE_NOT_ZERO;
            return p_object;
        }
        else if (p_expression->expression_type == EXPR_EQUALITY_EQUAL ||
                 p_expression->expression_type == EXPR_EQUALITY_NOT_EQUAL)
        {

            struct flow1_object* _Opt p_object = flow1_make_flow_object(ctx, &p_expression->type, NULL, p_expression);
            if (p_object == NULL) throw;

            if (object_has_constant_value(&p_expression->object))
            {
                bool not_zero = object_is_true(&p_expression->object);
                p_object->current.state = not_zero ? FLOW1_OBJECT_STATE_NOT_ZERO : FLOW1_OBJECT_STATE_ZERO;
            }
            else
            {
                p_object->current.state = FLOW1_OBJECT_STATE_NOT_ZERO | FLOW1_OBJECT_STATE_ZERO;
            }
            return p_object;
        }
        else if (p_expression->expression_type == EXPR_ADDITIVE_PLUS)
        {
            struct flow1_object* _Opt p_object = flow1_make_flow_object(ctx, &p_expression->type, NULL, p_expression);
            if (p_object == NULL) throw;

            if (type_is_pointer(&p_expression->type))
            {
                //p + 1
                //never null
                p_object->current.state = FLOW1_OBJECT_STATE_NOT_NULL;
            }
            else
            {
                if (object_has_constant_value(&p_expression->object))
                {
                    bool not_zero = object_is_true(&p_expression->object);
                    p_object->current.state = not_zero ? FLOW1_OBJECT_STATE_NOT_NULL : FLOW1_OBJECT_STATE_NULL;
                }
                else
                {
                    p_object->current.state = FLOW1_OBJECT_STATE_NOT_NULL | FLOW1_OBJECT_STATE_NULL;
                }
            }
            return p_object;
        }
        else if (p_expression->expression_type == EXPR_UNARY_NEG ||
                 p_expression->expression_type == EXPR_UNARY_PLUS)
        {
            assert(p_expression->right != NULL);

            struct flow1_object* _Opt p_obj_right = flow1_expression_get_flow_object(ctx, p_expression->right, nullable_enabled);
            struct flow1_object* _Opt p_object = flow1_make_flow_object(ctx, &p_expression->type, NULL, p_expression);
            if (p_object == NULL) throw;

            if (p_obj_right)
            {
                p_object->current.state = p_obj_right->current.state;
            }

            return p_object;
        }
        //
        else
        {
            struct flow1_object* _Opt p_object = flow1_make_flow_object(ctx, &p_expression->type, NULL, p_expression);
            if (p_object == NULL) throw;

            if (type_is_pointer(&p_expression->type))
            {
                if (object_has_constant_value(&p_expression->object))
                {
                    bool not_zero = object_is_true(&p_expression->object);
                    p_object->current.state = not_zero ? FLOW1_OBJECT_STATE_NOT_NULL : FLOW1_OBJECT_STATE_NULL;
                }
                else
                {
                    p_object->current.state = FLOW1_OBJECT_STATE_NOT_NULL | FLOW1_OBJECT_STATE_NULL;
                }
            }
            else
            {
                if (object_has_constant_value(&p_expression->object))
                {
                    bool not_zero = object_is_true(&p_expression->object);
                    p_object->current.state = not_zero ? FLOW1_OBJECT_STATE_NOT_ZERO : FLOW1_OBJECT_STATE_ZERO;
                }
                else
                {
                    p_object->current.state = FLOW1_OBJECT_STATE_NOT_ZERO | FLOW1_OBJECT_STATE_ZERO;
                }
            }


            return p_object;
        }

    }
    catch
    {

    }
    //ss_fprintf(ss0, "null object");
    //assert(false);
    return NULL;
}

void flow1_check_assignment(
    struct flow1_visit_ctx* ctx,
    const struct token* error_position,
    const struct marker* p_a_marker,
    const struct marker* p_b_marker,
    enum assigment_type assigment_type,
    bool check_uninitialized_b,
    bool a_type_is_view,
    bool a_type_is_nullable,
    struct type* p_a_type, struct flow1_object* p_a_object,
    struct type* p_b_type, struct flow1_object* p_b_object,
    bool* _Opt set_argument_to_unkown)
{
    if (type_is_pointer(p_b_type) && flow1_object_is_expansible(p_b_object))
    {
        //number of warning goes up!
        //flow_object_expand_pointer(ctx, p_b_type, p_b_object);
    }

    struct flow1_object_visitor visitor_a = {
    .p_object = p_a_object,
    .p_type = p_a_type
    };

    struct flow1_object_visitor visitor_b = {
     .p_object = p_b_object,
     .p_type = p_b_type
    };


    flow1_assignment_core(
     ctx,
    error_position,
    p_a_marker,
    p_b_marker,
    assigment_type,
    check_uninitialized_b,
    a_type_is_view,
    a_type_is_nullable,
    &visitor_a,
    &visitor_b,
    set_argument_to_unkown);
}


void flow1_print_object_state_to_str(enum flow1_state e, char str[], int sz, struct osstream* ss0)
{
    bool first = true;
    struct osstream ss = { 0 };

    if (e == FLOW1_OBJECT_STATE_NOT_APPLICABLE)
        ss_fprintf(&ss, "--");

    if (e & FLOW1_OBJECT_STATE_UNINITIALIZED)
    {
        if (first)
            first = false;
        else
            ss_fprintf(&ss, ",");
        ss_fprintf(&ss, "unset");
    }

    if (e & FLOW1_OBJECT_STATE_NOT_NULL)
    {
        if (first)
            first = false;
        else
            ss_fprintf(&ss, ",");
        ss_fprintf(&ss, "not-null");
    }

    if (e & FLOW1_OBJECT_STATE_NULL)
    {
        if (first)
            first = false;
        else
            ss_fprintf(&ss, ",");
        ss_fprintf(&ss, "null");
    }

    if (e & FLOW1_OBJECT_STATE_ZERO)
    {
        if (first)
            first = false;
        else
            ss_fprintf(&ss, ",");
        ss_fprintf(&ss, "zero");
    }

    if (e & FLOW1_OBJECT_STATE_NOT_ZERO)
    {
        if (first)
            first = false;
        else
            ss_fprintf(&ss, ",");
        ss_fprintf(&ss, "not-zero");
    }


    if (e & FLOW1_OBJECT_STATE_LIFE_TIME_ENDED)
    {
        if (first)
            first = false;
        else
            ss_fprintf(&ss, ",");
        ss_fprintf(&ss, "lifetime-ended");
    }

    if (e & FLOW1_OBJECT_STATE_MOVED)
    {
        if (first)
            first = false;
        else
            ss_fprintf(&ss, ",");
        ss_fprintf(&ss, "moved");
    }
    snprintf(str, sz, "%s", ss.c_str);
    ss_close(&ss);
}

void flow1_object_state_print(struct flow1_object_state* p_state, struct osstream* ss0)
{
    struct osstream ss = { 0 };

    char temp[200] = { 0 };
    flow1_print_object_state_to_str(p_state->state, temp, sizeof temp, ss0);
    ss_fprintf(&ss, "%d %s", p_state->state_number, temp);


    if (p_state->pointed)
    {
        ss_fprintf(&ss, " ->%d", p_state->pointed->id);
    }

    if (p_state->alternatives.size > 0)
        ss_fprintf(&ss, " ");
    for (int i = 0; i < p_state->alternatives.size; i++)
    {
        if (i != 0)
            ss_fprintf(&ss, ",");

        ss_fprintf(&ss, "%d", p_state->alternatives.data[i]->id);
    }
    ss_fprintf(ss0, "%-25s│", ss.c_str);
    ss_close(&ss);
}

void flow1_print_object_line(struct flow1_object* p_object, int extra_cols, struct osstream* ss0)
{
    struct osstream ss = { 0 };

    if (p_object->parent)
    {
        ss_fprintf(&ss, "↑%d", p_object->parent->id);

        //if (p_object->current.alternatives.size > 0)
         //ss_fprintf( &ss, " &");

        ss_fprintf(ss0, "│%-2d│", p_object->id);
        ss_fprintf(ss0, "%-20s│", ss.c_str); //here we need compesate the unicode byte len of ↑
    }
    else
    {
        int line = 0, col = 0;
        if (p_object->p_declarator_origin)
        {
            if (p_object->p_declarator_origin->name_opt)
            {
                line = p_object->p_declarator_origin->name_opt->line;
                col = p_object->p_declarator_origin->name_opt->col;
                ss_fprintf(&ss, "%2d:%2d ", line, col);
                ss_fprintf(&ss, "%s", p_object->p_declarator_origin->name_opt->lexeme);
            }
            else
            {
                ss_fprintf(&ss, "%2d:%2d ", line, col);
                ss_fprintf(&ss, "%s", "?");
            }
        }
        else if (p_object->p_expression_origin)
        {
            line = p_object->p_expression_origin->first_token->line;
            col = p_object->p_expression_origin->first_token->col;
            ss_fprintf(&ss, "%2d:%2d ", line, col);

            ss_fprintf(&ss, "%s", p_object->p_expression_origin->first_token->lexeme);
        }
        else
        {
            ss_fprintf(&ss, "&");
        }
        ss_fprintf(ss0, "│%-2d│", p_object->id);
        ss_fprintf(ss0, "%-18s│", ss.c_str);
    }

    ss_close(&ss);


    int cols = 0;
    struct flow1_object_state* _Opt p_state = &p_object->current;
    while (p_state)
    {
        cols++;
        flow1_object_state_print(p_state, ss0);
        p_state = p_state->next;
    }

    for (int i = 0; i <= extra_cols - cols; i++)
    {
        ss_fprintf(ss0, "%-25s│", " ");
    }
    ss_fprintf(ss0, "\n");

}

void flow1_visit_declaration(struct flow1_visit_ctx* ctx, struct declaration* p_declaration);


static void flow1_visit_secondary_block(struct flow1_visit_ctx* ctx, struct secondary_block* p_secondary_block);
static void flow1_visit_struct_or_union_specifier(struct flow1_visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier);
struct flow1_true_false_set;
static void flow1_visit_full_expression(struct flow1_visit_ctx* ctx, struct expression* p_expression, struct flow1_true_false_set* a);
static void flow1_visit_statement(struct flow1_visit_ctx* ctx, struct statement* p_statement);
static void flow1_visit_enum_specifier(struct flow1_visit_ctx* ctx, struct enum_specifier* p_enum_specifier);
static void flow1_visit_type_specifier(struct flow1_visit_ctx* ctx, struct type_specifier* p_type_specifier);
static void flow1_visit_bracket_initializer_list(struct flow1_visit_ctx* ctx, struct braced_initializer* p_bracket_initializer_list);
static void flow1_visit_expression_statement(struct flow1_visit_ctx* ctx, struct expression_statement* p_expression_statement);

enum flow1_boolean_flag
{
    FLOW1_BOOLEAN_FLAG_NONE = 0,
    FLOW1_BOOLEAN_FLAG_TRUE = 1 << 0,
    FLOW1_BOOLEAN_FLAG_FALSE = 1 << 1,
};

struct flow1_true_false_set_item
{
    struct expression* p_expression;
    enum flow1_boolean_flag true_branch_state;
    enum flow1_boolean_flag false_branch_state;
};

struct flow1_true_false_set
{
    struct flow1_true_false_set_item* _Owner _Opt data;
    int size;
    int capacity;
};

static void flow1_true_false_set_clear(struct flow1_true_false_set* p)
{
    free(p->data);
    p->data = NULL;
    p->size = 0;
    p->capacity = 0;
}

enum flow1_merge_options
{
    FLOW1_MERGE_OPTIONS_A_TRUE = 1 << 0,
    FLOW1_MERGE_OPTIONS_A_FALSE = 1 << 1,
    FLOW1_MERGE_OPTIONS_B_TRUE = 1 << 2,
    FLOW1_MERGE_OPTIONS_B_FALSE = 1 << 3
};

void flow1_true_false_set_destroy(_Dtor struct flow1_true_false_set* p)
{
    free(p->data);
}

static int flow1_true_false_set_reserve(struct flow1_true_false_set* p, int n)
{
    if (n > p->capacity)
    {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0]))))
        {
            return EOVERFLOW;
        }

        void* _Owner _Opt pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL)
            return ENOMEM;
        override_state(p->data, "moved");
        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

static int flow1_true_false_set_push_back(struct flow1_true_false_set* p, const struct flow1_true_false_set_item* book)
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

        int error = flow1_true_false_set_reserve(p, new_capacity);
        if (error != 0)
        {
            return error;
        }
    }

    p->data[p->size] = *book; /*COPIED*/


    p->size++;

    return 0;
}

static void flow1_true_false_set_invert(struct flow1_true_false_set* true_false_set)
{
    for (int i = 0; i < true_false_set->size; i++)
    {
        enum flow1_boolean_flag temp = true_false_set->data[i].true_branch_state;
        true_false_set->data[i].true_branch_state = true_false_set->data[i].false_branch_state;
        true_false_set->data[i].false_branch_state = temp;
    }
}

static void flow1_true_false_set_swap(struct flow1_true_false_set* a, struct flow1_true_false_set* b)
{
    struct flow1_true_false_set temp = *a;
    *a = *b;
    *b = temp;
}

static int flow1_find_item_index_by_expression(const struct flow1_true_false_set* a, const struct expression* p_expression)
{
    for (int i = 0; i < a->size; i++)
    {
        if (a->data[i].p_expression == p_expression)
            return i;
    }
    return -1;
}

static void flow1_true_false_set_merge(struct flow1_true_false_set* result,
    struct flow1_true_false_set* a,
    struct flow1_true_false_set* b,
    enum flow1_merge_options options_true,
    enum flow1_merge_options options_false)
{

    for (int i = 0; i < a->size; i++)
    {
        const struct flow1_true_false_set_item* p_item_a = &a->data[i];

        _Opt struct flow1_true_false_set_item new_item = { 0 };
        new_item.p_expression = p_item_a->p_expression;

        if (options_true & FLOW1_MERGE_OPTIONS_A_TRUE)
            new_item.true_branch_state |= p_item_a->true_branch_state;

        if (options_true & FLOW1_MERGE_OPTIONS_A_FALSE)
            new_item.true_branch_state |= p_item_a->false_branch_state;

        if (options_false & FLOW1_MERGE_OPTIONS_A_TRUE)
            new_item.false_branch_state |= p_item_a->true_branch_state;

        if (options_false & FLOW1_MERGE_OPTIONS_A_FALSE)
            new_item.true_branch_state |= p_item_a->false_branch_state;


        flow1_true_false_set_push_back(result, &new_item);
    }

    for (int k = 0; k < b->size; k++)
    {
        const struct flow1_true_false_set_item* p_item_b = &b->data[k];

        int index = flow1_find_item_index_by_expression(result, p_item_b->p_expression);
        if (index == -1)
        {
            index = result->size;
            _Opt struct flow1_true_false_set_item item2 = { 0 };
            item2.p_expression = p_item_b->p_expression;
            flow1_true_false_set_push_back(result, &item2);
        }

        struct flow1_true_false_set_item* p_item_result = &result->data[index];

        if (options_true & FLOW1_MERGE_OPTIONS_B_TRUE)
            p_item_result->true_branch_state |= p_item_b->true_branch_state;

        if (options_true & FLOW1_MERGE_OPTIONS_B_FALSE)
            p_item_result->true_branch_state |= p_item_b->false_branch_state;

        if (options_false & FLOW1_MERGE_OPTIONS_B_TRUE)
            p_item_result->false_branch_state |= p_item_b->true_branch_state;

        if (options_false & FLOW1_MERGE_OPTIONS_B_FALSE)
            p_item_result->true_branch_state |= p_item_b->false_branch_state;
    }
}

static void flow1_true_false_set_set_objects_to_core_branch(struct flow1_visit_ctx* ctx,
    struct flow1_true_false_set* a,
    bool nullable_enabled,
    bool true_branch)
{
    for (int i = 0; i < a->size; i++)
    {
        assert(a->data[i].p_expression != NULL);

        struct flow1_object* _Opt p_object =
            flow1_expression_get_flow_object(ctx, a->data[i].p_expression, nullable_enabled);

        if (p_object)
        {
            if (p_object->current.state == FLOW1_OBJECT_STATE_NOT_NULL ||
                p_object->current.state == FLOW1_OBJECT_STATE_NULL ||
                p_object->current.state == FLOW1_OBJECT_STATE_MOVED ||
                p_object->current.state == FLOW1_OBJECT_STATE_ZERO ||
                p_object->current.state == FLOW1_OBJECT_STATE_NOT_ZERO ||
                p_object->current.state == FLOW1_OBJECT_STATE_LIFE_TIME_ENDED)
            {
                continue;
            }

            const enum flow1_boolean_flag flag =
                true_branch ?
                a->data[i].true_branch_state :
                a->data[i].false_branch_state;

            if ((flag & FLOW1_BOOLEAN_FLAG_TRUE) && (flag & FLOW1_BOOLEAN_FLAG_FALSE))
            {
            }
            else if (flag & FLOW1_BOOLEAN_FLAG_FALSE)
            {
                p_object->current.state &= ~FLOW1_OBJECT_STATE_NOT_NULL;
                p_object->current.state &= ~FLOW1_OBJECT_STATE_MOVED;
            }
            else if (flag & FLOW1_BOOLEAN_FLAG_TRUE)
            {
                p_object->current.state &= ~FLOW1_OBJECT_STATE_NULL;
                p_object->current.state &= ~FLOW1_OBJECT_STATE_ZERO;

            }
        }
    }
}

static void flow1_true_false_set_set_objects_to_true_branch(struct flow1_visit_ctx* ctx, struct flow1_true_false_set* a, bool nullable_enabled)
{
    flow1_true_false_set_set_objects_to_core_branch(ctx, a, nullable_enabled, true);
}

static void flow1_true_false_set_set_objects_to_false_branch(struct flow1_visit_ctx* ctx, struct flow1_true_false_set* a, bool nullable_enabled)
{
    flow1_true_false_set_set_objects_to_core_branch(ctx, a, nullable_enabled, false);
}

static int flow1_arena_add_copy_of_current_state(struct flow1_visit_ctx* ctx, const char* name);


static void flow1_arena_remove_state(struct flow1_visit_ctx* ctx, int state_number);

struct flow1_visit_objects {
    struct flow1_defer_scope* current_block;
    struct flow1_defer_scope* next_child;
};


static void flow1_exit_block_visit_defer_item(struct flow1_visit_ctx* ctx,
    struct defer_list_item* p_item,
    struct token* position_token)
{
    if (p_item->defer_statement)
    {
        const int error_count = ctx->ctx->p_report->error_count;
        const int warnings_count = ctx->ctx->p_report->warnings_count;
        const int info_count = ctx->ctx->p_report->info_count;

        flow1_visit_unlabeled_statement(ctx, p_item->defer_statement->unlabeled_statement);

        if (error_count != ctx->ctx->p_report->error_count ||
            warnings_count != ctx->ctx->p_report->warnings_count ||
            info_count != ctx->ctx->p_report->info_count)
        {
            diagnostic(W_LOCATION, ctx->ctx, position_token, NULL, "defer end of scope");
        }
    }
    else if (p_item->declarator)
    {
        struct declarator* p_declarator = p_item->declarator;
        const char* name = p_declarator->name_opt ? p_declarator->name_opt->lexeme : "?";

        if (p_declarator->p_flow_object)
        {
            flow1_end_of_block_visit(ctx,
                &p_declarator->type,
                type_is_view(&p_declarator->type),
                p_declarator->p_flow_object,
                position_token,
                name);
        }
        else
        {
            assert(0);
        }
    }
}

static void flow1_exit_block_visit_defer_list(struct flow1_visit_ctx* ctx,
    struct defer_list* p_defer_list,
    struct token* position_token)
{
    struct defer_list_item* _Opt p_item = p_defer_list->head;
    while (p_item)
    {
        flow1_exit_block_visit_defer_item(ctx, p_item, position_token);
        p_item = p_item->next;
    }
}

static void flow1_defer_item_set_end_of_lifetime(struct flow1_visit_ctx* ctx,
    struct defer_list_item* p_item,
    struct token* position_token)
{
    if (p_item->defer_statement)
    {
        //?
    }
    else if (p_item->declarator)
    {
        struct declarator* p_declarator = p_item->declarator;
        //const char* name = p_declarator->name_opt ? p_declarator->name_opt->lexeme : "?";

        if (p_declarator->p_flow_object)
        {
            flow1_object_set_end_of_lifetime(&p_declarator->type, p_declarator->p_flow_object);
        }
        else
        {
            assert(0);
        }
    }
}

static void flow1_defer_list_set_end_of_lifetime(struct flow1_visit_ctx* ctx,
    struct defer_list* p_defer_list,
    struct token* position_token)
{
    struct defer_list_item* _Opt p_item = p_defer_list->head;
    while (p_item)
    {
        flow1_defer_item_set_end_of_lifetime(ctx, p_item, position_token);
        p_item = p_item->next;
    }
}

static void flow1_visit_secondary_block(struct flow1_visit_ctx* ctx, struct secondary_block* p_secondary_block)
{
    flow1_visit_statement(ctx, p_secondary_block->statement);
}

static void flow1_visit_defer_statement(struct flow1_visit_ctx* ctx, struct defer_statement* p_defer_statement)
{
    /*
      We are not going to visit the secondary block here because
      this is not the place were defer is executed.
    */
}

_Attr(nodiscard)
static int flow1_arena_add_copy_of_current_state(struct flow1_visit_ctx* ctx, const char* name)
{
    int state_number = ctx->state_number_generator;
    ctx->state_number_generator++;
    for (int i = 0; i < ctx->arena.size; i++)
    {
        flow1_object_add_new_state_as_a_copy_of_current_state(ctx->arena.data[i], name, state_number);
    }
    return state_number;
}

static int flow1_arena_add_empty_state(struct flow1_visit_ctx* ctx, const char* name)
{
    int state_number = ctx->state_number_generator;
    ctx->state_number_generator++;

    for (int i = 0; i < ctx->arena.size; i++)
    {
        struct flow1_object* p_object = ctx->arena.data[i];
        _Opt struct flow1_object_state* _Owner _Opt p_flow_object_state = calloc(1, sizeof * p_flow_object_state);
        if (p_flow_object_state)
        {
            p_flow_object_state->dbg_name = name;
            p_flow_object_state->state_number = state_number;
            flow1_object_add_state(p_object, p_flow_object_state);
        }

    }
    return state_number;
}

static void flow1_object_set_state_from_current(struct flow1_object* object, int state_number)
{
    struct flow1_object_state* _Opt p_flow_object_state = object->current.next;
    while (p_flow_object_state)
    {
        if (p_flow_object_state->state_number == state_number)
        {
            flow1_object_state_copy(p_flow_object_state, &object->current);
            break;
        }
        p_flow_object_state = p_flow_object_state->next;
    }
}

static void flow1_arena_set_state_from_current(struct flow1_visit_ctx* ctx, int number_state)
{
    for (int i = 0; i < ctx->arena.size; i++)
    {
        struct flow1_object* p_object = ctx->arena.data[i];
        flow1_object_set_state_from_current(p_object, number_state);
    }
}

static int flow1_object_merge_current_with_state(struct flow1_visit_ctx* ctx, struct flow1_object* object, int state_number)
{
    try
    {
        struct flow1_object_state* _Opt it = object->current.next;
        while (it)
        {
            if (it->state_number == state_number)
            {
                it->state |= object->current.state;
                if (object->current.pointed == NULL && it->pointed == NULL)
                {
                    //nothing
                }
                else if (object->current.pointed == NULL && it->pointed != NULL)
                {
                    //mesmo
                }
                else if (object->current.pointed != NULL && it->pointed == NULL)
                {
                    it->pointed = object->current.pointed;
                }
                else if (object->current.pointed != it->pointed)
                {
                    assert(object->current.pointed != NULL);
                    assert(it->pointed != NULL);

                    struct flow1_object* _Opt p_new_object = arena_new_object(ctx);
                    if (p_new_object == NULL) throw;

                    _Opt struct flow1_object_state* _Owner _Opt p_new_state = calloc(1, sizeof * p_new_state);
                    if (p_new_state == NULL) throw;

                    p_new_state->dbg_name = "merged";
                    p_new_state->state_number = state_number;
                    flow1_objects_view_push_back(&p_new_state->alternatives, object->current.pointed);
                    flow1_objects_view_push_back(&p_new_state->alternatives, it->pointed);
                    flow1_object_add_state(p_new_object, p_new_state);

                    int n_childs_1 = object->current.pointed->members.size;
                    int n_childs_2 = it->pointed->members.size;
                    if (n_childs_1 == n_childs_2)
                    {

                        for (int j = 0; j < n_childs_1; j++)
                        {
                            struct flow1_object* _Opt p_new_child = arena_new_object(ctx);
                            if (p_new_child == NULL) throw;

                            p_new_child->parent = p_new_object;

                            struct flow1_object* child1 = object->current.pointed->members.data[j];
                            struct flow1_object* child2 = it->pointed->members.data[j];

                            _Opt struct flow1_object_state* _Owner _Opt p_child_new_state = calloc(1, sizeof * p_child_new_state);
                            if (p_child_new_state == NULL) throw;

                            p_child_new_state->dbg_name = "merged";
                            p_child_new_state->state_number = state_number;
                            p_child_new_state->state = child1->current.state | it->state;
                            flow1_objects_view_push_back(&p_child_new_state->alternatives, child1);
                            flow1_objects_view_push_back(&p_child_new_state->alternatives, child2);
                            flow1_object_add_state(p_new_child, p_child_new_state);
                            flow1_objects_view_push_back(&p_new_object->members, p_new_child);
                        }
                    }
                    else
                    {
                        //ops
                        //dbg_break();
                    }

                    it->pointed = p_new_object;
                }
                break;
            }
            it = it->next;
        }
    }
    catch
    {
    }
    return 0;
}

static void flow1_arena_merge_current_state_with_state_number(struct flow1_visit_ctx* ctx, int number_state)
{
    for (int i = 0; i < ctx->arena.size; i++)
    {
        flow1_object_merge_current_with_state(ctx, ctx->arena.data[i], number_state);
    }
}

static void flow1_object_restore_current_state_from(struct flow1_visit_ctx* ctx, struct flow1_object* object, int state_number)
{
    struct flow1_object_state* _Opt it = object->current.next;
    while (it)
    {
        if (it->state_number == state_number)
        {
            flow1_object_state_copy(&object->current, it);
            break;
        }
        it = it->next;
    }
}

static void flow1_arena_restore_current_state_from(struct flow1_visit_ctx* ctx, int number_state)
{
    for (int i = 0; i < ctx->arena.size; i++)
    {
        flow1_object_restore_current_state_from(ctx, ctx->arena.data[i], number_state);
    }
}

static void flow1_arena_remove_state(struct flow1_visit_ctx* ctx, int state_number)
{
    for (int i = 0; i < ctx->arena.size; i++)
    {
        flow1_object_remove_state(ctx->arena.data[i], state_number);
    }
}

static void flow1_visit_initializer(struct flow1_visit_ctx* ctx, struct initializer* p_initializer);
static void flow1_visit_declarator(struct flow1_visit_ctx* ctx, struct declarator* p_declarator);


static void flow1_braced_initializer_flow_core(struct flow1_visit_ctx* ctx, struct object* obj, struct flow1_object* flow1_obj)
{
    /*
       We use the obj to get the expression that initializes it.
    */

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    if (obj->p_init_expression)
    {
        struct flow1_object* _Opt p_right_object =
            flow1_expression_get_flow_object(ctx, obj->p_init_expression, nullable_enabled);

        if (p_right_object)
        {
            struct marker a_marker = {
                .p_token_begin = NULL,
                .p_token_end = NULL,
            };

            struct marker b_marker = {
                .p_token_begin = obj->p_init_expression->first_token,
                .p_token_end = obj->p_init_expression->last_token,
            };

            flow1_check_assignment(ctx,
                                obj->p_init_expression->first_token,
                                &a_marker,
                                &b_marker,
                                ASSIGMENT_TYPE_OBJECTS,
                                false,
                                type_is_view(&obj->type),
                                type_is_opt(&obj->type, ctx->ctx->options.null_checks_enabled),
                                &obj->type,
                                flow1_obj,
                                &obj->p_init_expression->type,
                                p_right_object,
                                NULL);
        }
    }
    else
    {
        /*default initialization*/
        flow1_object_set_zero(&obj->type, flow1_obj);
    }

    if (flow1_obj->members.size > 0)
    {
        /*flow_object and object have the same number of members*/
        int i = 0;

        struct object* _Opt m = obj->members.head;
        while (m)
        {
            flow1_braced_initializer_flow_core(ctx, m, flow1_obj->members.data[i]);
            m = m->next;
            i++;
        }
    }
}

static void flow1_braced_initializer_flow(struct flow1_visit_ctx* ctx, struct object* obj, struct flow1_object* flow1_obj)
{
    try
    {
        flow1_braced_initializer_flow_core(ctx, obj, flow1_obj);

        if (flow1_obj->p_declarator_origin == NULL)
        {
            throw;
        }
        //We are allowing invalid (not final) state
#if 0
        /*
           Let´s check if the object has been initialized correctly
        */

        bool is_nullable = type_is_opt(&obj->type, ctx->ctx->options.null_checks_enabled);

        struct marker a_marker = {
                          .p_token_begin = flow1_obj->p_declarator_origin->first_token_opt,
                          .p_token_end = flow1_obj->p_declarator_origin->last_token_opt,
        };
        checked_read_object(ctx,
        &obj->type,
        is_nullable,
        flow1_obj,
        flow1_obj->p_declarator_origin->first_token_opt,
        &a_marker,
        false);
#endif

    }
    catch
    {
    }
}


static void flow1_visit_init_declarator(struct flow1_visit_ctx* ctx, struct init_declarator* p_init_declarator)
{
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    try
    {
        flow1_visit_declarator(ctx, p_init_declarator->p_declarator);

        if (p_init_declarator->initializer)
        {
            if (p_init_declarator->initializer->assignment_expression)
            {
                struct flow1_true_false_set a = { 0 };
                flow1_visit_full_expression(ctx, p_init_declarator->initializer->assignment_expression, &a);
                flow1_true_false_set_destroy(&a);
            }
            else
            {
                if (p_init_declarator->initializer->braced_initializer)
                {
                    flow1_visit_bracket_initializer_list(ctx,
                        p_init_declarator->initializer->braced_initializer);
                }
                else
                {
                    assert(false);
                }
            }
        }

        if (p_init_declarator->p_declarator->type.category != TYPE_CATEGORY_FUNCTION)
        {
            if (p_init_declarator->initializer &&
                p_init_declarator->initializer->assignment_expression)
            {
                struct flow1_object* _Opt p_right_object =
                    flow1_expression_get_flow_object(ctx, p_init_declarator->initializer->assignment_expression, nullable_enabled);

                if (p_right_object)
                {
                    struct marker a_marker = {
                        .p_token_begin = p_init_declarator->p_declarator->first_token_opt,
                        .p_token_end = p_init_declarator->p_declarator->last_token_opt,
                    };

                    struct marker b_marker = {
                        .p_token_begin = p_init_declarator->initializer->assignment_expression->first_token,
                        .p_token_end = p_init_declarator->initializer->assignment_expression->last_token,
                    };

                    assert(p_init_declarator->p_declarator->p_flow_object != NULL);

                    flow1_check_assignment(ctx,
                                        p_init_declarator->initializer->assignment_expression->first_token,
                                        &a_marker,
                                        &b_marker,
                                        ASSIGMENT_TYPE_OBJECTS,
                                        false,
                                        type_is_view(&p_init_declarator->p_declarator->type),
                                        type_is_opt(&p_init_declarator->p_declarator->type, ctx->ctx->options.null_checks_enabled),
                                        &p_init_declarator->p_declarator->type,
                                        p_init_declarator->p_declarator->p_flow_object,
                                        &p_init_declarator->initializer->assignment_expression->type,
                                        p_right_object,
                                        NULL);
                }
                //cast?
                if (expression_is_malloc(p_init_declarator->initializer->assignment_expression))
                {
                    assert(p_init_declarator->p_declarator->p_flow_object != NULL);

                    struct type t = type_remove_pointer(&p_init_declarator->p_declarator->type);
                    struct flow1_object* _Opt po = flow1_make_flow_object(ctx, &t, p_init_declarator->p_declarator, NULL);
                    if (po == NULL)
                    {
                        type_destroy(&t);
                        throw;
                    }

                    flow1_object_set_pointer(p_init_declarator->p_declarator->p_flow_object, po);
                    type_destroy(&t);
                    p_init_declarator->p_declarator->p_flow_object->current.state = FLOW1_OBJECT_STATE_NOT_NULL | FLOW1_OBJECT_STATE_NULL;
                }
                else if (expression_is_calloc(p_init_declarator->initializer->assignment_expression))
                {
                    assert(p_init_declarator->p_declarator->p_flow_object != NULL);

                    struct type t = type_remove_pointer(&p_init_declarator->p_declarator->type);
                    struct flow1_object* _Opt pointed_calloc_object = flow1_make_flow_object(ctx, &t, p_init_declarator->p_declarator, NULL);
                    if (pointed_calloc_object == NULL)
                    {
                        type_destroy(&t);
                        throw;
                    }

                    flow1_object_set_zero(&t, pointed_calloc_object);
                    flow1_object_set_pointer(p_init_declarator->p_declarator->p_flow_object, pointed_calloc_object);

                    //We are allowing invalid (not final) state
#if 0
                    struct marker a_marker = {
                      .p_token_begin = p_init_declarator->p_declarator->first_token_opt,
                      .p_token_end = p_init_declarator->p_declarator->last_token_opt,
                    };

                    checked_read_object(ctx,
                        &t,
                        type_is_opt(&t, ctx->ctx->options.null_checks_enabled),
                        pointed_calloc_object,
                        p_init_declarator->p_declarator->first_token_opt,
                        &a_marker,
                        false);
#endif
                    type_destroy(&t);
                    p_init_declarator->p_declarator->p_flow_object->current.state = FLOW1_OBJECT_STATE_NOT_NULL | FLOW1_OBJECT_STATE_NULL;
                }
            }
            else if (p_init_declarator->initializer && p_init_declarator->initializer->braced_initializer)
            {
                assert(p_init_declarator->p_declarator->p_flow_object != NULL);
                flow1_braced_initializer_flow(ctx,
                    &p_init_declarator->p_declarator->object,
                    p_init_declarator->p_declarator->p_flow_object);
            }
            else
            {
                if (p_init_declarator->p_declarator->declaration_specifiers &&
                    (
                        (!(p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_BLOCK_SCOPE)) ||
                        (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_STATIC)
                        )
                    )
                {
                    assert(p_init_declarator->p_declarator->p_flow_object != NULL);
                    flow1_object_set_zero(&p_init_declarator->p_declarator->type, p_init_declarator->p_declarator->p_flow_object);
                }
                else
                {
                    assert(p_init_declarator->p_declarator->p_flow_object != NULL);
                    flow1_object_set_uninitialized(&p_init_declarator->p_declarator->type, p_init_declarator->p_declarator->p_flow_object);
                }
            }
        }
    }
    catch
    {
        //some unexpected error
    }
}


static void flow1_visit_init_declarator_list(struct flow1_visit_ctx* ctx, struct init_declarator_list* p_init_declarator_list);

static void flow1_visit_declaration_specifiers(struct flow1_visit_ctx* ctx, struct declaration_specifiers* p_declaration_specifiers, struct type* _Opt p_type);


static void flow1_visit_simple_declaration(struct flow1_visit_ctx* ctx, struct simple_declaration* p_simple_declaration)
{
    flow1_visit_declaration_specifiers(ctx, p_simple_declaration->p_declaration_specifiers, NULL);
    flow1_visit_init_declarator_list(ctx, &p_simple_declaration->init_declarator_list);
}

static void flow1_check_pointer_used_as_bool(struct flow1_visit_ctx* ctx, struct expression* p_expression);

void flow1_print_arena(struct flow1_visit_ctx* ctx, struct osstream* ss0)
{
    int extra_cols = 0;
    for (int i = 0; i < ctx->arena.size; i++)
    {
        struct flow1_object* p_object = ctx->arena.data[i];
        struct flow1_object_state* _Opt p_state = p_object->current.next;
        int count = 0;
        while (p_state)
        {
            count++;
            p_state = p_state->next;
        }
        if (count > extra_cols)
            extra_cols = count;
    }


    //┐
    ss_fprintf(ss0, "\n");
    ss_fprintf(ss0, "┌──┬──────────────────┬─────────────────────────");
    if (extra_cols > 0)
    {
        for (int i = 0; i < extra_cols; i++)
        {
            if (i < extra_cols - 1)
                ss_fprintf(ss0, "┬─────────────────────────");
            else
                ss_fprintf(ss0, "┬─────────────────────────");
        }
    }

    ss_fprintf(ss0, "┐");

    ss_fprintf(ss0, "\n");

    for (int i = 0; i < ctx->arena.size; i++)
    {
        struct flow1_object* p = ctx->arena.data[i];
        flow1_print_object_line(p, extra_cols, ss0);
    }
    ss_fprintf(ss0, "└──┴──────────────────┴─────────────────────────");
    if (extra_cols > 0)
    {
        for (int i = 0; i < extra_cols; i++)
        {
            if (i < extra_cols - 1)
                ss_fprintf(ss0, "┴─────────────────────────");
            else
                ss_fprintf(ss0, "┴─────────────────────────");
        }
    }

    ss_fprintf(ss0, "┘");

    ss_fprintf(ss0, "\n");
    ss_fprintf(ss0, "\n");
}

static void flow1_visit_if_statement(struct flow1_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    if (p_selection_statement->p_init_statement &&
        p_selection_statement->p_init_statement->p_expression_statement)
        flow1_visit_expression_statement(ctx, p_selection_statement->p_init_statement->p_expression_statement);

    if (p_selection_statement->p_init_statement &&
        p_selection_statement->p_init_statement->p_simple_declaration)
        flow1_visit_simple_declaration(ctx, p_selection_statement->p_init_statement->p_simple_declaration);


    /*
      if(int *p = f());
      The hidden expression is p
    */
    _Opt _View struct expression hidden_expression = { 0 };

    struct flow1_true_false_set true_false_set = { 0 };

    if (p_selection_statement->condition &&
        p_selection_statement->condition->expression)
    {
        flow1_check_pointer_used_as_bool(ctx, p_selection_statement->condition->expression);
        flow1_visit_full_expression(ctx, p_selection_statement->condition->expression, &true_false_set);
    }

    if (p_selection_statement->condition &&
        p_selection_statement->condition->p_init_declarator)
    {
        flow1_visit_init_declarator(ctx, p_selection_statement->condition->p_init_declarator);
    }

    if (p_selection_statement->condition &&
        p_selection_statement->condition->expression == NULL &&
        p_selection_statement->condition->p_init_declarator != NULL)
    {
        hidden_expression.expression_type = EXPR_PRIMARY_DECLARATOR;
        hidden_expression.declarator = p_selection_statement->condition->p_init_declarator->p_declarator;
        assert(p_selection_statement->condition->p_init_declarator->p_declarator->first_token_opt != NULL);
        hidden_expression.first_token = p_selection_statement->condition->p_init_declarator->p_declarator->first_token_opt;
        hidden_expression.last_token = hidden_expression.first_token;
        flow1_visit_full_expression(ctx, &hidden_expression, &true_false_set);
    }

    assert(p_selection_statement->first_token->type == TK_KEYWORD_IF);

    /*
       This index is from the end of top of stack going to base of statck
    */
    const int before_if_state_number = flow1_arena_add_copy_of_current_state(ctx, "before-if");

    flow1_true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);
    flow1_visit_secondary_block(ctx, p_selection_statement->secondary_block);

    const bool true_branch_ends_with_jump =
        secondary_block_ends_with_jump(p_selection_statement->secondary_block);


    /*let's make a copy of the state we left true branch*/
    //const int true_branch = 1;
    const int left_true_branch_state_number = flow1_arena_add_copy_of_current_state(ctx, "left-true-branch");

    flow1_arena_restore_current_state_from(ctx, before_if_state_number);


    if (p_selection_statement->else_secondary_block_opt)
    {
        flow1_true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
        flow1_visit_secondary_block(ctx, p_selection_statement->else_secondary_block_opt);
    }
    else
    {
        //we emulate a empty else {}
        flow1_true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
    }

    bool else_ends_with_jump =
        p_selection_statement->else_secondary_block_opt ?
        secondary_block_ends_with_jump(p_selection_statement->else_secondary_block_opt) :
        false;


    if (true_branch_ends_with_jump)
    {
        if (else_ends_with_jump)
        {
            /*
               if (){
                 return
               }
               else
               {
                 return;
               }
            */
            /*no merge*/
        }
        else
        {
            /*
               if (){
                 return
               }
               else
               {

               }
            */

        }
    }
    else
    {
        if (else_ends_with_jump)
        {
            /*
               if (){

               }
               else
               {
                 return;
               }
            */

            flow1_arena_restore_current_state_from(ctx, left_true_branch_state_number);
        }
        else
        {
            /*
               if (){
               }
               else {

               }
            */

            flow1_arena_merge_current_state_with_state_number(ctx, left_true_branch_state_number);

            flow1_arena_restore_current_state_from(ctx, left_true_branch_state_number);
        }
    }

    flow1_exit_block_visit_defer_list(ctx, &p_selection_statement->defer_list, p_selection_statement->last_token);
    flow1_defer_list_set_end_of_lifetime(ctx, &p_selection_statement->defer_list, p_selection_statement->last_token);

    flow1_arena_remove_state(ctx, before_if_state_number);
    flow1_arena_remove_state(ctx, left_true_branch_state_number);
    flow1_true_false_set_destroy(&true_false_set);
}

static void flow1_visit_block_item(struct flow1_visit_ctx* ctx, struct block_item* p_block_item);


static void flow1_visit_try_statement(struct flow1_visit_ctx* ctx, struct try_statement* p_try_statement)
{

    const int throw_join_state_old = ctx->throw_join_state;
    struct secondary_block* _Opt catch_secondary_block_old = ctx->catch_secondary_block_opt;

    ctx->catch_secondary_block_opt = p_try_statement->catch_secondary_block_opt;

    ctx->throw_join_state = flow1_arena_add_empty_state(ctx, "try");

    const int original_state_number = flow1_arena_add_copy_of_current_state(ctx, "original");


    flow1_visit_secondary_block(ctx, p_try_statement->secondary_block);
    flow1_arena_set_state_from_current(ctx, original_state_number); //state of end of secondary block

    if (p_try_statement->catch_secondary_block_opt)
    {
        //current all possible states of throw
        flow1_arena_restore_current_state_from(ctx, ctx->throw_join_state);
        flow1_visit_secondary_block(ctx, p_try_statement->catch_secondary_block_opt);
        //current has the state at the end of catch block
    }

    bool try_reached_the_end = !secondary_block_ends_with_jump(p_try_statement->secondary_block);
    bool catch_reached_the_end = !secondary_block_ends_with_jump(p_try_statement->catch_secondary_block_opt);

    if (try_reached_the_end && catch_reached_the_end)
    {
        //we could merge directly at current
        flow1_arena_merge_current_state_with_state_number(ctx, original_state_number);
        flow1_arena_restore_current_state_from(ctx, original_state_number);
    }
    else if (try_reached_the_end)
    {
        flow1_arena_restore_current_state_from(ctx, original_state_number);
    }
    else if (catch_reached_the_end)
    {
        //ctx_object_restore_current_state_from(ctx, orignial);       
    }


    flow1_arena_remove_state(ctx, original_state_number);
    flow1_arena_remove_state(ctx, ctx->throw_join_state);

    ctx->throw_join_state = throw_join_state_old; //restore
    ctx->catch_secondary_block_opt = catch_secondary_block_old; //restore

}

static void flow1_visit_switch_statement(struct flow1_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    const int old_initial_state = ctx->initial_state;
    const int old_break_join_state = ctx->break_join_state;

    ctx->initial_state = flow1_arena_add_copy_of_current_state(ctx, "original");
    ctx->break_join_state = flow1_arena_add_empty_state(ctx, "break join");

    flow1_visit_secondary_block(ctx, p_selection_statement->secondary_block);

    bool reached_the_end = !secondary_block_ends_with_jump(p_selection_statement->secondary_block);

    if (!reached_the_end)
    {
        flow1_arena_restore_current_state_from(ctx, ctx->break_join_state);
    }

    flow1_exit_block_visit_defer_list(ctx, &p_selection_statement->defer_list, p_selection_statement->secondary_block->last_token);
    flow1_defer_list_set_end_of_lifetime(ctx, &p_selection_statement->defer_list, p_selection_statement->secondary_block->last_token);

    flow1_arena_remove_state(ctx, ctx->initial_state);
    flow1_arena_remove_state(ctx, ctx->break_join_state);

    //restore
    ctx->initial_state = old_initial_state;
    ctx->break_join_state = old_break_join_state;
}

static void flow1_visit_selection_statement(struct flow1_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    if (p_selection_statement->first_token->type == TK_KEYWORD_IF)
    {
        flow1_visit_if_statement(ctx, p_selection_statement);
    }
    else if (p_selection_statement->first_token->type == TK_KEYWORD_SWITCH)
    {
        flow1_visit_switch_statement(ctx, p_selection_statement);
    }
    else
        assert(false);

    if (p_selection_statement->lint_token)
    {
        flow1_check_dianostic_suppression(ctx, p_selection_statement->lint_token);
    }

}

static void flow1_visit_compound_statement(struct flow1_visit_ctx* ctx, struct compound_statement* p_compound_statement);

static void flow1_visit_initializer_list(struct flow1_visit_ctx* ctx, struct initializer_list* p_initializer_list);

static void flow1_visit_bracket_initializer_list(struct flow1_visit_ctx* ctx, struct braced_initializer* p_bracket_initializer_list)
{
    if (p_bracket_initializer_list->initializer_list == NULL)
    {

    }
    else
    {
        flow1_visit_initializer_list(ctx, p_bracket_initializer_list->initializer_list);
    }
}

static void flow1_visit_bracket_initializer_list(struct flow1_visit_ctx* ctx, struct braced_initializer* p_bracket_initializer_list);

static void flow1_visit_initializer_list_item(struct flow1_visit_ctx* ctx, struct initializer_list_item* p_initializer)
{
    assert(p_initializer->initializer != NULL);
    flow1_visit_initializer(ctx, p_initializer->initializer);
}

static void flow1_visit_initializer(struct flow1_visit_ctx* ctx, struct initializer* p_initializer)
{
    if (p_initializer->assignment_expression)
    {
        struct flow1_true_false_set a = { 0 };
        flow1_visit_full_expression(ctx, p_initializer->assignment_expression, &a);
        flow1_true_false_set_destroy(&a);
    }
    else if (p_initializer->braced_initializer)
    {
        flow1_visit_bracket_initializer_list(ctx, p_initializer->braced_initializer);
    }
}

static void flow1_visit_initializer_list(struct flow1_visit_ctx* ctx, struct initializer_list* p_initializer_list)
{
    struct initializer_list_item* _Opt p_initializer = p_initializer_list->head;
    while (p_initializer)
    {
        flow1_visit_initializer_list_item(ctx, p_initializer);
        p_initializer = p_initializer->next;
    }
}

static void flow1_visit_generic_selection(struct flow1_visit_ctx* ctx, struct generic_selection* p_generic_selection)
{
    if (p_generic_selection->expression)
    {
        struct flow1_true_false_set a = { 0 };
        flow1_visit_full_expression(ctx, p_generic_selection->expression, &a);
        flow1_true_false_set_destroy(&a);
    }
}


static void flow1_compare_function_arguments(struct flow1_visit_ctx* ctx,
    struct type* p_type,
    struct argument_expression_list* p_argument_expression_list)
{
    try
    {
        const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

        const struct param_list* _Opt p_param_list = type_get_func_or_func_ptr_params(p_type);
        if (p_param_list == NULL) throw;

        struct param* _Opt p_current_parameter_type = p_param_list->head;
        struct argument_expression* _Opt p_current_argument = p_argument_expression_list->head;

        while (p_current_argument && p_current_parameter_type)
        {

            struct diagnostic temp =
                ctx->ctx->options.diagnostic_stack.stack[ctx->ctx->options.diagnostic_stack.top_index];

            //we don´t report W_FLOW_UNINITIALIZED here because it is checked next.. (TODO parts of expression)
            diagnostic_remove(&ctx->ctx->options.diagnostic_stack.stack[ctx->ctx->options.diagnostic_stack.top_index], W_FLOW_UNINITIALIZED);

            {
                struct flow1_true_false_set a2 = { 0 };
                flow1_visit_full_expression(ctx, p_current_argument->expression, &a2);
                flow1_true_false_set_destroy(&a2);
            }

            ctx->ctx->options.diagnostic_stack.stack[ctx->ctx->options.diagnostic_stack.top_index] = temp;


            struct flow1_object* _Opt p_argument_object =
                flow1_expression_get_flow_object(ctx, p_current_argument->expression, nullable_enabled);

            if (p_argument_object)
            {
                struct flow1_object* _Opt parameter_object = flow1_make_flow_object(ctx, &p_current_parameter_type->type, NULL, p_current_argument->expression);
                if (parameter_object == NULL) throw;

                flow1_object_set_uninitialized(&p_current_parameter_type->type, parameter_object);

                struct marker a_marker = {
                    .p_token_begin = p_current_argument->expression->first_token,
                    .p_token_end = p_current_argument->expression->last_token
                };

                //TODO pass real function to get location
                struct marker b_marker = {
                    .p_token_begin = p_current_argument->expression->first_token,
                    .p_token_end = p_current_argument->expression->last_token
                };

                flow1_check_assignment(ctx,
                  p_current_argument->expression->first_token,
                  &a_marker,
                  &b_marker,
                  ASSIGMENT_TYPE_PARAMETER,
                  true,
                  type_is_view(&p_current_parameter_type->type),
                  type_is_opt(&p_current_parameter_type->type, ctx->ctx->options.null_checks_enabled),
                  &p_current_parameter_type->type,
                  parameter_object, /*dest object*/

                  &p_current_argument->expression->type,
                  p_argument_object,
                  &p_current_argument->set_unkown
                );
            }
            p_current_argument = p_current_argument->next;
            p_current_parameter_type = p_current_parameter_type->next;
        }

        while (p_current_argument)
        {
            /*
               We have more argument than parameters, this happens with variadic functions
            */

            struct flow1_object* _Opt p_argument_object =
                flow1_expression_get_flow_object(ctx, p_current_argument->expression, nullable_enabled);

            if (p_argument_object)
            {
                struct marker marker = {
                    .p_token_begin = p_current_argument->expression->first_token,
                    .p_token_end = p_current_argument->expression->last_token
                };
                flow1_checked_read_object(ctx,
                    &p_current_argument->expression->type,
                    type_is_opt(&p_current_argument->expression->type, ctx->ctx->options.null_checks_enabled),
                    p_argument_object,
                    p_current_argument->expression->first_token,
                    &marker,
                    false);
            }
            else
            {
                //
            }
            p_current_argument = p_current_argument->next;
        }

        //////////////////////////// SECOND PASS ////////////////////////////
        /*
            //consider this sample...
            void f(struct X *p,  int * p);

            int main()
            {
                struct X *  pX = make();
                if (pX->p)
                {
                   //cake is making pX->p  unknown before function call..it must be after
                   f(pX, pX->p);
                }
            }
        */

        /*struct param* */ p_current_parameter_type = p_param_list->head;
        /*struct argument_expression* */ p_current_argument = p_argument_expression_list->head;


        while (p_current_argument && p_current_parameter_type)
        {
            if (p_current_argument->set_unkown &&
                type_is_pointer(&p_current_argument->expression->type))
            {
                struct type pointed_type = type_remove_pointer(&p_current_argument->expression->type);

                struct flow1_object* _Opt p_argument_object =
                    flow1_expression_get_flow_object(ctx, p_current_argument->expression, nullable_enabled);


                if (p_argument_object)
                {
                    const bool argument_type_is_nullable =
                        type_is_opt(&pointed_type, ctx->ctx->options.null_checks_enabled);

                    if (p_argument_object->current.pointed)
                    {
                        flow1_object_set_unknown(&pointed_type,
                                           argument_type_is_nullable,
                                           p_argument_object->current.pointed,
                                           ctx->ctx->options.null_checks_enabled);
                    }
                    else
                    {
                        //assert(false);
                    }
                }

                type_destroy(&pointed_type);
            }
            p_current_argument = p_current_argument->next;
            p_current_parameter_type = p_current_parameter_type->next;
        }

        while (p_current_argument)
        {
            /*
               We have more argument than parameters, this happens with variadic functions
            */

            struct flow1_object* _Opt p_argument_object =
                flow1_expression_get_flow_object(ctx, p_current_argument->expression, nullable_enabled);

            if (p_argument_object)
            {
                //??
            }
            else
            {
                //??
            }
            p_current_argument = p_current_argument->next;
        }
    }
    catch
    {
    }
}

static void flow1_check_uninitialized(struct flow1_visit_ctx* ctx, struct expression* p_expression)
{
    if (p_expression->is_assignment_expression)
        return;

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    if (type_is_function(&p_expression->type) ||
        type_is_void(&p_expression->type) ||
        type_is_nullptr_t(&p_expression->type))
    {
        return;
    }

    struct flow1_object* _Opt p_object = flow1_expression_get_flow_object(ctx, p_expression, nullable_enabled);

    if (!ctx->expression_is_not_evaluated)
    {
        if (p_object && p_object->current.state == FLOW1_OBJECT_STATE_UNINITIALIZED)
        {
            if (p_expression->expression_type == EXPR_PRIMARY_DECLARATOR &&
                p_expression->declarator &&
                p_expression->declarator->name_opt)
            {
                diagnostic(W_FLOW_UNINITIALIZED,
                    ctx->ctx,
                    p_expression->first_token, NULL, "using a uninitialized object '%s'", p_expression->declarator->name_opt->lexeme);
            }
            else
            {
                diagnostic(W_FLOW_UNINITIALIZED,
                    ctx->ctx,
                    p_expression->first_token, NULL, "using a uninitialized object");
            }
        }
        else if (p_object && p_object->current.state & FLOW1_OBJECT_STATE_UNINITIALIZED)
        {
            if (ctx->ctx->options.ownership_enabled)
            {
                if (p_expression->declarator && p_expression->declarator->name_opt)
                {
                    diagnostic(W_FLOW_UNINITIALIZED,
                        ctx->ctx,
                        p_expression->first_token,
                        NULL,
                        "object '%s' can be uninitialized ",
                        p_expression->declarator->name_opt->lexeme);
                }
                else
                {
                    diagnostic(W_FLOW_UNINITIALIZED,
                        ctx->ctx,
                        p_expression->first_token, NULL, "maybe using a uninitialized object");
                }
            }
        }
    }
}

void flow1_object_push_states_from(const struct flow1_object* p_object_from, struct flow1_object* p_object_to)
{
    struct flow1_object_state* _Opt it_from = p_object_from->current.next;
    while (it_from)
    {
#if 0
        flow1_object_add_state(
        p_object_to,
        p_object_to->current.state,
        &p_object_to->current.ref,
        it_from->dbg_name,
        it_from->state_number);
#endif

        it_from = it_from->next;
    }

    for (int i = 0; i < p_object_to->members.size; i++)
    {
        flow1_object_push_states_from(p_object_from, p_object_to->members.data[i]);
    }
}


static void flow1_check_pointer_used_as_bool(struct flow1_visit_ctx* ctx, struct expression* p_expression)
{
    //when pointer as used as bool, like if (p) or p && p->i we check
    //if the pointer has a flow condition always true or false.

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    if (type_is_pointer(&p_expression->type))
    {

        struct flow1_object* _Opt p_object = flow1_expression_get_flow_object(ctx, p_expression, nullable_enabled);
        if (p_object)
        {
            struct marker marker = {
                 .p_token_begin = p_expression->first_token,
                 .p_token_end = p_expression->last_token
            };

            if (!ctx->inside_loop && flow1_object_is_null(p_object))
            {
                diagnostic(W_FLOW_NON_NULL,
                        ctx->ctx,
                        NULL,
                        &marker,
                        "pointer is always null");

            }
            else if (!ctx->inside_loop && flow1_object_is_not_null(p_object))
            {
                diagnostic(W_FLOW_NON_NULL,
                        ctx->ctx,
                        NULL,
                        &marker,
                        "pointer is always not-null");
            }
        }
    }
}

static void flow1_arena_broadcast_change(struct flow1_visit_ctx* ctx, struct flow1_object* p)
{
    for (int i = 0; i < ctx->arena.size; i++)
    {
        struct flow1_object* p_obj = ctx->arena.data[i];

        for (int j = 0; j < p_obj->current.alternatives.size; j++)
        {
            if (p_obj->current.alternatives.data[j] == p)
            {
                flow1_object_update_current(p_obj);
                break;
            }
        }
    }
}

static struct argument_expression* _Opt flow1_param_list_find_argument_by_name(struct param_list* p_param_list,
    struct argument_expression_list* list,
    const char* name)
{
    struct param* _Opt p_param = p_param_list->head;
    struct argument_expression* _Opt p_argument_expression = list->head;
    while (p_param && p_argument_expression)
    {
        if (p_param->type.name_opt &&
            strcmp(p_param->type.name_opt, name) == 0)
        {
            return p_argument_expression;
        }
        p_argument_expression = p_argument_expression->next;
        p_param = p_param->next;
    }
    return NULL;
}

/*
   Makes all alias to null
*/
static void flow1_clear_alias(struct expression* p_expression)
{
    if (p_expression->declarator)
        p_expression->declarator->p_alias_of_expression = NULL;

    struct argument_expression* _Opt p = p_expression->argument_expression_list.head;
    while (p)
    {
        flow1_clear_alias(p->expression);
        p = p->next;
    }


    if (p_expression->left)
        flow1_clear_alias(p_expression->left);
    if (p_expression->right)
        flow1_clear_alias(p_expression->right);
}

static void flow1_expression_bind(struct flow1_visit_ctx* ctx,
                                 struct expression* p_expression,
                                 struct param_list* p_param_list,
                                 struct argument_expression_list* p_argument_expression_list)
{

    if (p_expression->expression_type == EXPR_PRIMARY_DECLARATOR)
    {
        assert(p_expression->declarator != NULL);
        assert(p_expression->declarator->name_opt != NULL);

        struct argument_expression* _Opt p_argument_expression =
            flow1_param_list_find_argument_by_name(p_param_list,
                                             p_argument_expression_list,
                                             p_expression->declarator->name_opt->lexeme);
        if (p_argument_expression)
        {
            if (p_argument_expression->expression->declarator &&
                p_argument_expression->expression->declarator->p_alias_of_expression)
            {
                p_expression->declarator->p_alias_of_expression = p_argument_expression->expression->declarator->p_alias_of_expression;
            }
            else
            {
                p_expression->declarator->p_alias_of_expression = p_argument_expression->expression;
            }
            return;
        }
    }

    struct argument_expression* _Opt p = p_expression->argument_expression_list.head;
    while (p)
    {
        flow1_expression_bind(ctx, p->expression, p_param_list, p_argument_expression_list);
        p = p->next;
    }


    if (p_expression->left)
        flow1_expression_bind(ctx, p_expression->left, p_param_list, p_argument_expression_list);
    if (p_expression->right)
        flow1_expression_bind(ctx, p_expression->right, p_param_list, p_argument_expression_list);
}



static void flow1_check_dianostic_suppression(struct flow1_visit_ctx* ctx, struct token* p_token)
{
    check_dianostic_suppression_phase(ctx->ctx, p_token, 2);
}
static void flow1_visit_expression(struct flow1_visit_ctx* ctx, struct expression* p_expression, struct flow1_true_false_set* expr_true_false_set);

static void flow1_visit_full_expression(struct flow1_visit_ctx* ctx, struct expression* p_expression, struct flow1_true_false_set* expr_true_false_set)
{
    flow1_visit_expression(ctx, p_expression, expr_true_false_set);
}

static void flow1_visit_expression(struct flow1_visit_ctx* ctx, struct expression* p_expression, struct flow1_true_false_set* expr_true_false_set)
{

    flow1_true_false_set_clear(expr_true_false_set); //out

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    switch (p_expression->expression_type)
    {
    case EXPR_INVALID:
        assert(false);
        break;

    case EXPR_PRIMARY__FUNC__:
        break;

    case EXPR_PRIMARY_ENUMERATOR:

        break;
    case EXPR_PRIMARY_DECLARATOR:
    {
        assert(p_expression->declarator != NULL);

        if (p_expression->declarator->p_alias_of_expression)
        {
            /*
               Contracts:
               in this case we visit the expression that this declaration
               is representing
            */
            flow1_visit_expression(ctx, p_expression->declarator->p_alias_of_expression, expr_true_false_set);
        }
        else
        {
            _Opt struct flow1_true_false_set_item item = { 0 };
            item.p_expression = p_expression;
            item.true_branch_state = FLOW1_BOOLEAN_FLAG_TRUE;
            item.false_branch_state = FLOW1_BOOLEAN_FLAG_FALSE;
            flow1_true_false_set_push_back(expr_true_false_set, &item);
            flow1_check_uninitialized(ctx, p_expression);
        }
    }
    break;

    case EXPR_PRIMARY_PARENTHESIS:
        assert(p_expression->right != NULL);
        flow1_visit_expression(ctx, p_expression->right, expr_true_false_set);
        break;

    case EXPR_PRIMARY_STATEMENT_EXPRESSION:
        assert(p_expression->compound_statement != NULL);
        flow1_visit_compound_statement(ctx, p_expression->compound_statement);
        break;

    case EXPR_PRIMARY_STRING_LITERAL:
    case EXPR_PRIMARY_CHAR_LITERAL:
    case EXPR_PRIMARY_NUMBER:
    case EXPR_PRIMARY_PREDEFINED_CONSTANT:
        break;

    case EXPR_PRIMARY_GENERIC:
        assert(p_expression->generic_selection != NULL);
        flow1_visit_generic_selection(ctx, p_expression->generic_selection);
        break;

    case EXPR_POSTFIX_DOT:
    {
        assert(p_expression->left != NULL);

        {
            struct flow1_true_false_set left_set = { 0 };
            flow1_visit_expression(ctx, p_expression->left, &left_set);
            flow1_true_false_set_destroy(&left_set);
        }

        struct flow1_true_false_set_item item;
        item.p_expression = p_expression;
        item.true_branch_state = FLOW1_BOOLEAN_FLAG_TRUE;
        item.false_branch_state = FLOW1_BOOLEAN_FLAG_FALSE;
        flow1_true_false_set_push_back(expr_true_false_set, &item);
    }
    break;

    case EXPR_POSTFIX_ARROW:
    {
        assert(p_expression->left != NULL);

        {
            struct flow1_true_false_set left_set = { 0 };
            flow1_visit_expression(ctx, p_expression->left, &left_set);
            flow1_true_false_set_destroy(&left_set);
        }

        struct flow1_object* _Opt p_object = flow1_expression_get_flow_object(ctx, p_expression->left, nullable_enabled);

        if (p_object != NULL)
        {
            if (flow1_object_can_be_null(p_object))
            {
                if (ctx->expression_is_not_evaluated)
                {
                }
                else
                {
                    char object_name[100] = { 0 };
                    flow1_object_get_name(&p_expression->left->type, p_object, object_name, sizeof object_name);

                    struct marker marker = { 0 };
                    marker.p_token_begin = p_expression->left->first_token;
                    marker.p_token_end = p_expression->left->last_token;
                    diagnostic(W_FLOW_NULL_DEREFERENCE,
                            ctx->ctx,
                            NULL,
                            &marker,
                           "pointer '%s' may be null", object_name);
                }
            }
            else if (flow1_object_can_be_uninitialized(p_object))
            {
                if (ctx->expression_is_not_evaluated)
                {
                }
                else
                {
                    diagnostic(W_FLOW_NULL_DEREFERENCE,
                            ctx->ctx,
                            p_expression->left->first_token, NULL, "object is possibly uninitialized");
                }
            }
            else if (flow1_object_can_have_its_lifetime_ended(p_object))
            {
                if (ctx->expression_is_not_evaluated)
                {
                }
                else
                {
                    diagnostic(W_FLOW_LIFETIME_ENDED,
                            ctx->ctx,
                            p_expression->left->first_token, NULL, "object lifetime ended");
                }
            }
        }

        if (!ctx->expression_is_not_evaluated)
        {
            struct flow1_object* _Opt p_object2 = flow1_expression_get_flow_object(ctx, p_expression, nullable_enabled);
            if (p_object2 && flow1_object_can_have_its_lifetime_ended(p_object2))
            {
                struct marker marker = {
                    .p_token_begin = p_expression->first_token,
                    .p_token_end = p_expression->last_token
                };
                diagnostic(W_FLOW_LIFETIME_ENDED,
                        ctx->ctx,
                        NULL,
                        &marker,
                        "object lifetime ended");
            }
        }

        struct flow1_true_false_set_item item;
        item.p_expression = p_expression;
        item.true_branch_state = FLOW1_BOOLEAN_FLAG_TRUE;
        item.false_branch_state = FLOW1_BOOLEAN_FLAG_FALSE;
        flow1_true_false_set_push_back(expr_true_false_set, &item);
    }
    break;

    case EXPR_POSTFIX_INCREMENT:
    case EXPR_POSTFIX_DECREMENT:
    {
        assert(p_expression->left != NULL);

        struct flow1_object* const _Opt p_object = flow1_expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
        if (p_object)
        {
            if (flow1_object_is_null(p_object))
            {
                //p_object->current.state &= ~FLOW_OBJECT_STATE_NULL;
                p_object->current.state = FLOW1_OBJECT_STATE_NOT_NULL;
            }
            else if (flow1_object_is_zero(p_object))
            {
                //p_object->current.state &= ~FLOW_OBJECT_STATE_ZERO;
                p_object->current.state = FLOW1_OBJECT_STATE_NOT_ZERO;
            }
        }
        flow1_visit_expression(ctx, p_expression->left, expr_true_false_set);
    }
    break;

    case EXPR_POSTFIX_ARRAY:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        flow1_visit_expression(ctx, p_expression->left, expr_true_false_set);
        flow1_visit_expression(ctx, p_expression->right, expr_true_false_set);
        flow1_true_false_set_clear(expr_true_false_set);

        struct flow1_true_false_set_item item;
        item.p_expression = p_expression;
        item.true_branch_state = FLOW1_BOOLEAN_FLAG_TRUE;
        item.false_branch_state = FLOW1_BOOLEAN_FLAG_FALSE;
        flow1_true_false_set_push_back(expr_true_false_set, &item);

    }
    break;

    case EXPR_POSTFIX_FUNCTION_CALL:
    {
        if (!ctx->inside_contract)
        {
            assert(p_expression->left != NULL);

            struct flow1_true_false_set left_local = { 0 };
            flow1_visit_expression(ctx, p_expression->left, &left_local);

            flow1_compare_function_arguments(ctx, &p_expression->left->type, &p_expression->argument_expression_list);
            flow1_true_false_set_destroy(&left_local);
        }

#if EXPERIMENTAL_CONTRACTS_II
        if (p_expression->left->declarator &&
            type_is_function(&p_expression->left->declarator->type) &&
            p_expression->argument_expression_list.head)
        {

            struct argument_expression* _Opt p_argument_expression = p_expression->argument_expression_list.head;

            if (p_expression->left->declarator->direct_declarator &&
                p_expression->left->declarator->direct_declarator->function_declarator &&
                p_expression->left->declarator->direct_declarator->function_declarator->parameter_type_list_opt)
            {
                struct parameter_declaration* _Opt p_parameter =
                    p_expression->left->declarator->direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head;

                while (p_parameter && p_argument_expression)
                {
                    if (p_parameter->declarator)
                    {
                        p_parameter->declarator->p_alias_of_expression = p_argument_expression->expression;
                    }

                    p_parameter = p_parameter->next;
                    p_argument_expression = p_argument_expression->next;
                }
            }

            if (p_expression->left->declarator->direct_declarator &&
                p_expression->left->declarator->direct_declarator->function_declarator &&
                p_expression->left->declarator->direct_declarator->function_declarator->p_in_block)
            {
                flow1_visit_secondary_block(ctx, p_expression->left->declarator->direct_declarator->function_declarator->p_in_block);
            }

            if (p_expression->left->declarator->direct_declarator &&
                  p_expression->left->declarator->direct_declarator->function_declarator &&
                  p_expression->left->declarator->direct_declarator->function_declarator->p_out_block)
            {
                flow1_visit_secondary_block(ctx, p_expression->left->declarator->direct_declarator->function_declarator->p_out_block);
            }

            if (p_expression->left->declarator->direct_declarator &&
                p_expression->left->declarator->direct_declarator->function_declarator)
            {
                struct parameter_declaration* _Opt p_parameter = p_expression->left->declarator->direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head;
                while (p_parameter)
                {
                    if (p_parameter->declarator)
                        p_parameter->declarator->p_alias_of_expression = NULL;
                    p_parameter = p_parameter->next;
                }
            }
        }
#endif
    }
    break;

    case EXPR_POSTFIX_FUNCTION_LITERAL:
        assert(p_expression->compound_statement != NULL);
        flow1_visit_compound_statement(ctx, p_expression->compound_statement);

        break;

    case EXPR_POSTFIX_COMPOUND_LITERAL:

        assert(p_expression->left == NULL);
        assert(p_expression->right == NULL);
        assert(p_expression->type_name != NULL);
        assert(p_expression->braced_initializer != NULL);


        flow1_visit_bracket_initializer_list(ctx, p_expression->braced_initializer);

        struct flow1_object* _Opt temp2 = flow1_make_flow_object(ctx, &p_expression->type, NULL, p_expression);
        if (temp2 == NULL)
        {
            return;
        }

        if (p_expression->type_name->abstract_declarator->p_flow_object)
        {
            flow1_object_swap(temp2, p_expression->type_name->abstract_declarator->p_flow_object);

            //TODO the state of object depends of the initializer        
            flow1_object_set_zero(&p_expression->type, p_expression->type_name->abstract_declarator->p_flow_object);
        }

        break;

    case EXPR_UNARY_STATIC_ASSERTION:

        if (p_expression->static_assertion)
        {
            flow1_visit_static_assertion(ctx, p_expression->static_assertion);
        }
        break;

    case EXPR_UNARY_ALIGNOF_EXPRESSION:

        if (p_expression->right)
        {
            flow1_visit_expression(ctx, p_expression->right, expr_true_false_set);
        }

        break;

    case EXPR_UNARY_ALIGNOF_TYPE:
        break;

    case EXPR_UNARY_ASSERT:

        if (p_expression->right)
        {
            struct flow1_true_false_set true_false_set4 = { 0 };
            bool old = ctx->inside_assert;
            ctx->inside_assert = true;
            flow1_visit_expression(ctx, p_expression->right, &true_false_set4); //assert(p == 0);            
            ctx->inside_assert = old;
            flow1_true_false_set_set_objects_to_true_branch(ctx, &true_false_set4, nullable_enabled);
            flow1_true_false_set_destroy(&true_false_set4);
        }

        break;

    case EXPR_UNARY_SIZEOF_EXPRESSION:

        if (p_expression->right)
        {
            const bool t2 = ctx->expression_is_not_evaluated;
            ctx->expression_is_not_evaluated = true;
            flow1_visit_expression(ctx, p_expression->right, expr_true_false_set);
            ctx->expression_is_not_evaluated = t2;
        }


        break;

    case EXPR_UNARY_NEG:
    case EXPR_UNARY_PLUS:
        assert(p_expression->right != NULL);
        flow1_visit_expression(ctx, p_expression->right, expr_true_false_set);
        break;

    case EXPR_UNARY_NOT:
        assert(p_expression->right != NULL);
        flow1_check_pointer_used_as_bool(ctx, p_expression->right);
        flow1_visit_expression(ctx, p_expression->right, expr_true_false_set);
        flow1_true_false_set_invert(expr_true_false_set);
        break;

    case EXPR_CHECKED:
        //state before throw
        flow1_arena_merge_current_state_with_state_number(ctx, ctx->throw_join_state);

        assert(p_expression->left != NULL);
        flow1_visit_expression(ctx, p_expression->left, expr_true_false_set);

        flow1_exit_block_visit_defer_list(ctx, &p_expression->defer_list, p_expression->first_token);

        break;

    case EXPR_UNARY_SIZEOF_TYPE:
    case EXPR_UNARY_COUNTOF:
    case EXPR_UNARY_INCREMENT:
    case EXPR_UNARY_DECREMENT:
    case EXPR_UNARY_BITNOT:
    case EXPR_UNARY_ADDRESSOF:
        break;

    case EXPR_UNARY_CONTENT:
    {
        assert(p_expression->right != NULL);

        struct flow1_object* _Opt p_object0 = flow1_expression_get_flow_object(ctx, p_expression->right, nullable_enabled);

        if (p_object0 && p_object0->current.state == FLOW1_OBJECT_STATE_UNINITIALIZED)
        {
            if (!ctx->expression_is_not_evaluated)
            {
                struct marker marker = { 0 };
                marker.p_token_begin = p_expression->right->first_token;
                marker.p_token_end = p_expression->right->last_token;
                diagnostic(W_FLOW_UNINITIALIZED,
                    ctx->ctx,
                    NULL, &marker, "using a uninitialized object");
            }
        }
        else if (p_object0 && flow1_object_can_be_null(p_object0))
        {
            /*
              *p = 1*
            */
            if (!ctx->expression_is_not_evaluated)
            {
                diagnostic(W_FLOW_NULL_DEREFERENCE,
                    ctx->ctx,
                    p_expression->right->first_token, NULL, "dereference a NULL object");
            }
        }

        struct flow1_true_false_set local_true_false = { 0 };
        flow1_visit_expression(ctx, p_expression->right, &local_true_false);
        /*empty set*/
        flow1_true_false_set_destroy(&local_true_false);
    }
    break;

    case EXPR_ASSIGNMENT_ASSIGN:
    case EXPR_ASSIGNMENT_PLUS_ASSIGN:
    case EXPR_ASSIGNMENT_MINUS_ASSIGN:
    case EXPR_ASSIGNMENT_MULTI_ASSIGN:
    case EXPR_ASSIGNMENT_DIV_ASSIGN:
    case EXPR_ASSIGNMENT_MOD_ASSIGN:
    case EXPR_ASSIGNMENT_SHIFT_LEFT_ASSIGN:
    case EXPR_ASSIGNMENT_SHIFT_RIGHT_ASSIGN:
    case EXPR_ASSIGNMENT_AND_ASSIGN:
    case EXPR_ASSIGNMENT_OR_ASSIGN:
    case EXPR_ASSIGNMENT_NOT_ASSIGN:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        {
            struct flow1_true_false_set left_set = { 0 };
            flow1_visit_expression(ctx, p_expression->left, &left_set);
            flow1_true_false_set_swap(expr_true_false_set, &left_set);
            flow1_true_false_set_destroy(&left_set);
        }

        {
            struct flow1_true_false_set right_set = { 0 };
            flow1_visit_expression(ctx, p_expression->right, &right_set);
            flow1_true_false_set_destroy(&right_set);
        }

        struct flow1_object* const _Opt p_right_object = flow1_expression_get_flow_object(ctx, p_expression->right, nullable_enabled);


        struct flow1_object* const _Opt p_dest_object = flow1_expression_get_flow_object(ctx, p_expression->left, nullable_enabled);

        if (p_dest_object == NULL || p_right_object == NULL)
        {
            return;
        }
        //TODO


        if (p_expression->expression_type != EXPR_ASSIGNMENT_ASSIGN)
        {
            if (flow1_object_can_be_uninitialized(p_dest_object))
            {
                if (flow1_object_is_uninitialized(p_dest_object))
                    diagnostic(W_UNINITIALZED, ctx->ctx, p_expression->left->first_token, NULL, "left object is uninitialized");
                else
                    diagnostic(W_UNINITIALZED, ctx->ctx, p_expression->left->first_token, NULL, "left object can be uninitialized");
            }
        }


        struct marker a_marker = {
          .p_token_begin = p_expression->left->first_token,
          .p_token_end = p_expression->left->last_token
        };

        struct marker b_marker = {
          .p_token_begin = p_expression->right->first_token,
          .p_token_end = p_expression->right->last_token
        };

        flow1_check_assignment(ctx,
            p_expression->left->first_token,
            &a_marker,
            &b_marker,
            ASSIGMENT_TYPE_OBJECTS,
            true,
            type_is_view(&p_expression->left->type), /*dest type*/
            type_is_opt(&p_expression->left->type, ctx->ctx->options.null_checks_enabled), /*dest type*/
            &p_expression->left->type, /*dest type*/
            p_dest_object, /*dest object*/
            &p_expression->right->type, /*source type*/
            p_right_object /*source*/,
            NULL);


        /*
          built-in malloc, calloc assignment
        */
        flow1_arena_broadcast_change(ctx, p_dest_object);

        if (expression_is_malloc(p_expression->right))
        {
            struct type t = type_remove_pointer(&p_expression->left->type);
            struct flow1_object* _Opt po = flow1_make_flow_object(ctx, &t, NULL, p_expression->left);
            if (po == NULL)
            {
                type_destroy(&t);
                return;
            }
            flow1_object_set_pointer(p_dest_object, po);
            type_destroy(&t);
            p_dest_object->current.state = FLOW1_OBJECT_STATE_NOT_NULL | FLOW1_OBJECT_STATE_NULL;
        }
        else if (expression_is_calloc(p_expression->right))
        {
            struct type t = type_remove_pointer(&p_expression->left->type);
            struct flow1_object* _Opt po = flow1_make_flow_object(ctx, &t, NULL, p_expression->left);
            if (po == NULL)
            {
                type_destroy(&t);
                return;
            }
            flow1_object_set_zero(&t, po);
            flow1_object_set_pointer(p_dest_object, po);
            type_destroy(&t);
            p_dest_object->current.state = FLOW1_OBJECT_STATE_NOT_NULL | FLOW1_OBJECT_STATE_NULL;
        }
    }
    break;
    case EXPR_MULTIPLICATIVE_DIV:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        struct flow1_true_false_set left_set = { 0 };
        struct flow1_true_false_set right_set = { 0 };

        flow1_visit_expression(ctx, p_expression->left, &left_set);

        struct flow1_object* _Opt p_object = flow1_expression_get_flow_object(ctx, p_expression->right, ctx->ctx->options.null_checks_enabled);
        if (p_object)
        {
            if (flow1_object_can_be_zero(p_object))
            {
                diagnostic(W_FLOW_DIVISION_BY_ZERO, ctx->ctx, p_expression->right->first_token, NULL, "possible division by zero");
            }
        }

        /*
                               true_set               false_set
             b / a             b_true_set a_true_set  a_true_set
             0 / a             -                      a_true_set a_true_set
        */

        flow1_visit_expression(ctx, p_expression->right, &right_set);
        flow1_true_false_set_merge(expr_true_false_set, &left_set, &right_set,
            FLOW1_MERGE_OPTIONS_A_TRUE | FLOW1_MERGE_OPTIONS_B_TRUE,
            FLOW1_MERGE_OPTIONS_A_TRUE | FLOW1_MERGE_OPTIONS_B_TRUE);

        flow1_true_false_set_destroy(&left_set);
        flow1_true_false_set_destroy(&right_set);
    }
    break;
    case EXPR_CAST:
    case EXPR_MULTIPLICATIVE_MULT:
    case EXPR_MULTIPLICATIVE_MOD:
    case EXPR_ADDITIVE_PLUS:
    case EXPR_ADDITIVE_MINUS:
    case EXPR_SHIFT_RIGHT:
    case EXPR_SHIFT_LEFT:
    {
        if (p_expression->left)
        {
            struct flow1_true_false_set left_set = { 0 };
            flow1_visit_expression(ctx, p_expression->left, &left_set);
            flow1_true_false_set_destroy(&left_set);
        }

        if (p_expression->right)
        {
            struct flow1_true_false_set right_set = { 0 };
            flow1_visit_expression(ctx, p_expression->right, &right_set);
            flow1_true_false_set_destroy(&right_set);
        }
    }
    break;

    case EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN:
    case EXPR_RELATIONAL_LESS_OR_EQUAL_THAN:
    case EXPR_RELATIONAL_BIGGER_THAN:
    case EXPR_RELATIONAL_LESS_THAN:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        const bool left_is_constant = object_has_constant_value(&p_expression->left->object);
        const bool right_is_constant = object_has_constant_value(&p_expression->right->object);

        if (left_is_constant)
        {
            const long long left_value = object_to_signed_long_long(&p_expression->left->object);

            struct flow1_true_false_set true_false_set_right = { 0 };
            flow1_visit_expression(ctx, p_expression->right, &true_false_set_right);
            if (left_value == 0)
            {
                flow1_true_false_set_swap(expr_true_false_set, &true_false_set_right);
                for (int i = 0; i < expr_true_false_set->size; i++)
                {
                    struct flow1_true_false_set_item* item = &expr_true_false_set->data[i];
                    item->false_branch_state |= item->true_branch_state;
                }

                if (p_expression->expression_type == EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN ||
                    p_expression->expression_type == EXPR_RELATIONAL_LESS_OR_EQUAL_THAN)
                {
                    flow1_true_false_set_invert(expr_true_false_set);
                }
            }
            flow1_true_false_set_destroy(&true_false_set_right);
        }

        else if (right_is_constant)
        {
            const long long right_value = object_to_signed_long_long(&p_expression->right->object);
            struct flow1_true_false_set true_false_set_left3 = { 0 };
            flow1_visit_expression(ctx, p_expression->left, &true_false_set_left3);
            if (right_value == 0)
            {
                flow1_true_false_set_swap(expr_true_false_set, &true_false_set_left3);
                for (int i = 0; i < expr_true_false_set->size; i++)
                {
                    struct flow1_true_false_set_item* item = &expr_true_false_set->data[i];
                    item->false_branch_state |= item->true_branch_state;
                }
                if (p_expression->expression_type == EXPR_RELATIONAL_BIGGER_OR_EQUAL_THAN ||
                    p_expression->expression_type == EXPR_RELATIONAL_LESS_OR_EQUAL_THAN)
                {
                    flow1_true_false_set_invert(expr_true_false_set);
                }
            }
            flow1_true_false_set_destroy(&true_false_set_left3);
        }
        else
        {
            struct flow1_true_false_set true_false_set = { 0 };
            flow1_visit_expression(ctx, p_expression->left, &true_false_set);
            flow1_visit_expression(ctx, p_expression->right, &true_false_set);
            flow1_true_false_set_destroy(&true_false_set);
        }
    }
    break;

    case EXPR_EQUALITY_NOT_EQUAL:
    case EXPR_EQUALITY_EQUAL:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        long long value = 0;
        struct expression* _Opt p_comp_expression = NULL;
        if (object_has_constant_value(&p_expression->left->object) &&
            !object_has_constant_value(&p_expression->right->object))
        {
            value = object_to_signed_long_long(&p_expression->left->object);
            p_comp_expression = p_expression->right;
        }
        else if (object_has_constant_value(&p_expression->right->object) &&
                !object_has_constant_value(&p_expression->left->object))
        {
            value = object_to_signed_long_long(&p_expression->right->object);
            p_comp_expression = p_expression->left;
        }


        if (p_comp_expression)
        {
            struct flow1_true_false_set true_false_set = { 0 };
            flow1_visit_expression(ctx, p_comp_expression, &true_false_set);

            //constant == p_comp_expression  |  p_comp_expression == constant
            //constant != p_comp_expression  |  p_comp_expression != constant

            struct flow1_object* _Opt p_object = flow1_expression_get_flow_object(ctx, p_comp_expression, nullable_enabled);
            if (p_object)
            {
                struct marker marker = {
                     .p_token_begin = p_comp_expression->first_token,
                     .p_token_end = p_comp_expression->last_token
                };


                if ((flow1_object_is_null(p_object) || flow1_object_is_zero(p_object)) && value == 0)
                {
                    if (p_expression->expression_type == EXPR_EQUALITY_EQUAL)
                    {
                        if (ctx->inside_assert)
                        {
                            /*
                             assert checks in runtime the same state we have at compile time
                             assert(p == NULL);
                            */
                        }
                        else
                        {
                            //if (p == NULL) { } //warning  p is always null
                            if (type_is_pointer(&p_comp_expression->type))
                            {
                                if (!ctx->inside_loop)
                                    diagnostic(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "pointer is always null");
                            }
                            else
                            {
                                if (!ctx->inside_loop)
                                    diagnostic(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "value is always zero");
                            }
                        }
                    }
                    else if (p_expression->expression_type == EXPR_EQUALITY_NOT_EQUAL)
                    {
                        /*
                           runtime check is diferent from static state
                           assert(p != NULL);
                        */
                        if (type_is_pointer(&p_comp_expression->type))
                            diagnostic(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "pointer is always null");
                        else
                            diagnostic(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "value is always zero");
                    }
                }
                else if ((flow1_object_is_not_null(p_object) || flow1_object_is_not_zero(p_object)) && value == 0)
                {
                    if (p_expression->expression_type == EXPR_EQUALITY_EQUAL)
                    {
                        /*
                           runtime check is diferent from static state
                           assert(p == NULL);
                        */
                        if (type_is_pointer(&p_comp_expression->type))
                        {
                            if (!ctx->inside_loop)
                                diagnostic(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "pointer is always non-null");
                        }
                        else
                        {
                            if (!ctx->inside_loop)
                                diagnostic(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "value is always non-zero");
                        }
                    }
                    else if (p_expression->expression_type == EXPR_EQUALITY_NOT_EQUAL)
                    {
                        /*
                           assert checks in runtime the same state we have at compile time
                           assert(p != NULL);
                        */
                        if (ctx->inside_assert)
                        {
                            /*
                             assert checks in runtime the same state we have at compile time
                             assert(p == NULL);
                            */
                        }
                        else
                        {
                            if (type_is_pointer(&p_comp_expression->type))
                            {
                                if (!ctx->inside_loop)
                                    diagnostic(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "pointer is always non-null");
                            }
                            else
                            {
                                if (!ctx->inside_loop)
                                    diagnostic(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "value is always non-zero");
                            }
                        }
                    }
                }
            }

            flow1_true_false_set_swap(expr_true_false_set, &true_false_set);
            if (p_expression->expression_type == EXPR_EQUALITY_EQUAL && value == 0)
            {
                flow1_true_false_set_invert(expr_true_false_set);
            }
            else if (p_expression->expression_type == EXPR_EQUALITY_NOT_EQUAL && value != 0)
            {
                flow1_true_false_set_invert(expr_true_false_set);
            }
            flow1_true_false_set_destroy(&true_false_set);
        }
        else
        {
            struct flow1_true_false_set true_false_set = { 0 };
            flow1_visit_expression(ctx, p_expression->left, &true_false_set);
            flow1_visit_expression(ctx, p_expression->right, &true_false_set);
            flow1_true_false_set_destroy(&true_false_set);
        }
    }
    break;

    case EXPR_LOGICAL_OR:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        flow1_check_pointer_used_as_bool(ctx, p_expression->left);
        flow1_check_pointer_used_as_bool(ctx, p_expression->right);


        struct flow1_true_false_set left_set = { 0 };
        flow1_visit_expression(ctx, p_expression->left, &left_set);

        if (object_has_constant_value(&p_expression->left->object) &&
            object_is_true(&p_expression->left->object) == true)
        {
            // left || right
            //left is true, so the right side will not run
        }
        else
        {
            const int original_state_number = flow1_arena_add_copy_of_current_state(ctx, "original");

            //Set all variables to false state, because otherwise, the right branch
            // would not be evaluated
            flow1_true_false_set_set_objects_to_false_branch(ctx, &left_set, nullable_enabled);

            struct flow1_true_false_set right_set = { 0 };
            flow1_visit_expression(ctx, p_expression->right, &right_set);

            //Tudo que faz left ser true ou right ser true

            for (int i = 0; i < left_set.size; i++)
            {
                struct flow1_true_false_set_item item5 = { 0 };
                item5.p_expression = left_set.data[i].p_expression;
                item5.true_branch_state |= (left_set.data[i].true_branch_state | left_set.data[i].false_branch_state);
                item5.false_branch_state |= left_set.data[i].false_branch_state;
                flow1_true_false_set_push_back(expr_true_false_set, &item5);
            }

            for (int k = 0; k < right_set.size; k++)
            {
                int index =
                    flow1_find_item_index_by_expression(expr_true_false_set, right_set.data[k].p_expression);
                if (index == -1)
                {
                    index = expr_true_false_set->size;

                    struct flow1_true_false_set_item item4 = {
                      .p_expression = right_set.data[k].p_expression
                    };

                    flow1_true_false_set_push_back(expr_true_false_set, &item4);
                }

                //Tudo que faz left true e right true faz expressao se true
                expr_true_false_set->data[index].p_expression = right_set.data[k].p_expression;
                //d->data[index].true_branch_state |= right_set.data[k].true_branch_state;
                //Tudo que faz left true ou left false, e right false faz ser false
                expr_true_false_set->data[index].false_branch_state |= right_set.data[k].false_branch_state;

                //No path true seria possivel nao ser feito o right
                expr_true_false_set->data[index].true_branch_state |= (FLOW1_BOOLEAN_FLAG_TRUE | FLOW1_BOOLEAN_FLAG_FALSE);

            }

            //TODO we whould not restore the previous states for states that 
            //are not true/false
            // if (false || init(a)) ... we cannot undo the out a
            flow1_arena_restore_current_state_from(ctx, original_state_number);
            flow1_arena_remove_state(ctx, original_state_number);
            flow1_true_false_set_destroy(&right_set);
        }
        flow1_true_false_set_destroy(&left_set);
    }
    break;

    case EXPR_LOGICAL_AND:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        flow1_check_pointer_used_as_bool(ctx, p_expression->left);
        flow1_check_pointer_used_as_bool(ctx, p_expression->right);


        struct flow1_true_false_set left_set = { 0 };
        flow1_visit_expression(ctx, p_expression->left, &left_set);

        const int original_state_number = flow1_arena_add_copy_of_current_state(ctx, "original");

        //Set all variables to true state, because otherwise, the right branch
        // would not be evaluated
        flow1_true_false_set_set_objects_to_true_branch(ctx, &left_set, nullable_enabled);

        struct flow1_true_false_set right_set = { 0 };
        flow1_visit_expression(ctx, p_expression->right, &right_set);
        //arena_restore_current_state_from(ctx, original_state_number);

       //Anything that makes left and right true also makes left && right true. 
       //Anything that makes left false or right false also makes left && right false.

        for (int i = 0; i < left_set.size; i++)
        {
            const struct flow1_true_false_set_item* p_item_left = &left_set.data[i];

            _Opt struct flow1_true_false_set_item left_and_right = { 0 };
            left_and_right.p_expression = p_item_left->p_expression;

            left_and_right.true_branch_state |= p_item_left->true_branch_state;
            left_and_right.false_branch_state |= p_item_left->true_branch_state | p_item_left->false_branch_state;

            flow1_true_false_set_push_back(expr_true_false_set, &left_and_right);
        }

        for (int k = 0; k < right_set.size; k++)
        {
            const struct flow1_true_false_set_item* p_item_right = &right_set.data[k];

            int index = flow1_find_item_index_by_expression(expr_true_false_set, p_item_right->p_expression);
            if (index == -1)
            {
                index = expr_true_false_set->size;
                struct flow1_true_false_set_item item2 = {
                  .p_expression = p_item_right->p_expression
                };
                flow1_true_false_set_push_back(expr_true_false_set, &item2);
            }

            //Tudo que faz left true e right true faz expressao se true
            expr_true_false_set->data[index].p_expression = p_item_right->p_expression;
            expr_true_false_set->data[index].true_branch_state |= p_item_right->true_branch_state;
            //Tudo que faz left true ou left false, e right false faz ser false
            expr_true_false_set->data[index].false_branch_state |= p_item_right->false_branch_state;

            //right expression may not be evaluated, in this case all previous states are also valid
            //so if the variable could be true and false then we need to add            
            expr_true_false_set->data[index].false_branch_state |= (FLOW1_BOOLEAN_FLAG_TRUE | FLOW1_BOOLEAN_FLAG_FALSE);

        }

        flow1_arena_restore_current_state_from(ctx, original_state_number);
        flow1_arena_remove_state(ctx, original_state_number);
        flow1_true_false_set_destroy(&left_set);
        flow1_true_false_set_destroy(&right_set);
    }
    break;

    case EXPR_INCLUSIVE_OR:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        // A | B
        struct flow1_true_false_set true_false_set = { 0 };

        flow1_visit_expression(ctx, p_expression->left, &true_false_set);
        flow1_visit_expression(ctx, p_expression->right, &true_false_set);

        flow1_true_false_set_destroy(&true_false_set);
    }
    break;

    case EXPR_AND:
    case EXPR_EXCLUSIVE_OR:

        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        flow1_visit_expression(ctx, p_expression->left, expr_true_false_set);
        flow1_visit_expression(ctx, p_expression->right, expr_true_false_set);


        break;

    case EXPR_UNARY_TRAITS:
        break;

    case EXPR_UNARY_IS_SAME:
        break;

    case EXPR_UNARY_DECLARATOR_ATTRIBUTE:
        break;

    case EXPR_EXPRESSION:
        assert(p_expression->left != NULL);
        assert(p_expression->right != NULL);


        flow1_visit_expression(ctx, p_expression->left, expr_true_false_set);
        flow1_visit_expression(ctx, p_expression->right, expr_true_false_set);

        break;

    case EXPR_CONDITIONAL:
    {
        assert(p_expression->condition_expr != NULL);
        assert(p_expression->right != NULL);

        struct flow1_true_false_set true_false_set = { 0 };

        flow1_check_pointer_used_as_bool(ctx, p_expression->condition_expr);
        flow1_visit_expression(ctx, p_expression->condition_expr, &true_false_set);

        const int before_if_state_number = flow1_arena_add_copy_of_current_state(ctx, "before-if");

        flow1_true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);

        {
            struct flow1_true_false_set set = { 0 };
            /* Elvis (left == NULL): condition result is the true branch — visit
               condition_expr again so flow sees its state on the true path */
            flow1_visit_expression(ctx,
                p_expression->left ? p_expression->left
                                   : p_expression->condition_expr,
                &set);
            flow1_true_false_set_destroy(&set);
        }


        const int left_true_branch_state_number = flow1_arena_add_copy_of_current_state(ctx, "left-true-branch");

        flow1_arena_restore_current_state_from(ctx, before_if_state_number);

        flow1_true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);

        {
            struct flow1_true_false_set set2 = { 0 };
            flow1_visit_expression(ctx, p_expression->right, &set2);
            flow1_true_false_set_destroy(&set2);
        }
        flow1_arena_merge_current_state_with_state_number(ctx, left_true_branch_state_number);

        flow1_arena_restore_current_state_from(ctx, left_true_branch_state_number);
        flow1_arena_remove_state(ctx, before_if_state_number);
        flow1_arena_remove_state(ctx, left_true_branch_state_number);
        flow1_true_false_set_destroy(&true_false_set);
    }
    break;

    case EXPR_UNARY_GCC__BUILTIN_VA_START:
    case EXPR_UNARY_GCC__BUILTIN_VA_END:
    case EXPR_UNARY_GCC__BUILTIN_VA_COPY:
    case EXPR_UNARY_GCC__BUILTIN_VA_ARG:
        break;
    case EXPR_UNARY_GCC__BUILTIN_OFFSETOF:
    case EXPR_UNARY_CONSTEVAL:
        break;

    }
}

static void flow1_visit_expression_statement(struct flow1_visit_ctx* ctx, struct expression_statement* p_expression_statement)
{
    struct flow1_true_false_set d = { 0 };
    if (p_expression_statement->expression_opt)
        flow1_visit_full_expression(ctx, p_expression_statement->expression_opt, &d);


    if (p_expression_statement->p_lint_token)
    {
        flow1_check_dianostic_suppression(ctx, p_expression_statement->p_lint_token);
    }

    flow1_true_false_set_destroy(&d);
}

static void flow1_visit_block_item_list(struct flow1_visit_ctx* ctx, struct block_item_list* p_block_item_list);

static void flow1_visit_compound_statement(struct flow1_visit_ctx* ctx, struct compound_statement* p_compound_statement)
{
    flow1_visit_block_item_list(ctx, &p_compound_statement->block_item_list);
    flow1_exit_block_visit_defer_list(ctx, &p_compound_statement->defer_list, p_compound_statement->last_token);
    flow1_defer_list_set_end_of_lifetime(ctx, &p_compound_statement->defer_list, p_compound_statement->last_token);

    if (p_compound_statement->lint_token)
    {
        flow1_check_dianostic_suppression(ctx, p_compound_statement->lint_token);
    }
}

static void flow1_visit_do_while_statement(struct flow1_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    assert(p_iteration_statement->first_token->type == TK_KEYWORD_DO);
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    struct flow1_true_false_set true_false_set = { 0 };


    if (p_iteration_statement->expression1)
    {
        flow1_visit_full_expression(ctx, p_iteration_statement->expression1, &true_false_set);
    }

    flow1_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

    flow1_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);
    flow1_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);

    bool was_last_statement_inside_true_branch_return =
        secondary_block_ends_with_jump(p_iteration_statement->secondary_block);


    if (was_last_statement_inside_true_branch_return)
    {
        //restore_state(ctx, 0);
        //if (p_object_compared_with_not_null)
        //{
            //do {}  while (p);
          //  p_object_compared_with_not_null->state = FLOW_OBJECT_STATE_NULL;
        //}
    }
    else
    {
        //do { } while (p);
        flow1_true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
    }


    flow1_true_false_set_destroy(&true_false_set);
}

static void flow1_visit_while_statement(struct flow1_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    assert(p_iteration_statement->first_token->type == TK_KEYWORD_WHILE);

    if (p_iteration_statement->expression1 == NULL)
        return;

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    const int old_initial_state = ctx->initial_state;
    const int old_break_join_state = ctx->break_join_state;

    ctx->initial_state = flow1_arena_add_copy_of_current_state(ctx, "original");
    ctx->break_join_state = flow1_arena_add_empty_state(ctx, "break join");

    struct flow1_true_false_set true_false_set = { 0 };

    /*
        we do like this to acumulate states.

        if (expression)
        {
           statements...
           if (expression)
           {
             statements...
           }
        }
        break_exit:
    */

    //We do a visit but this is not conclusive..so we ignore warnings
    diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);
    flow1_visit_full_expression(ctx, p_iteration_statement->expression1, &true_false_set);


    flow1_true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);

    flow1_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

    //Second pass warning is ON
    diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);

    {
        struct flow1_true_false_set true_false_set2 = { 0 };
        flow1_visit_full_expression(ctx, p_iteration_statement->expression1, &true_false_set2);
        flow1_true_false_set_destroy(&true_false_set2);
    }

    //visit secondary_block again
    flow1_true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);
    flow1_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

    flow1_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);

    const bool was_last_statement_inside_true_branch_return =
        secondary_block_ends_with_jump(p_iteration_statement->secondary_block);

    if (was_last_statement_inside_true_branch_return)
    {
        /*
           while (p) { return; }
        */
        flow1_arena_restore_current_state_from(ctx, ctx->initial_state);
        flow1_true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
    }
    else
    {
        flow1_true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
        flow1_arena_merge_current_state_with_state_number(ctx, ctx->break_join_state);
        flow1_arena_restore_current_state_from(ctx, ctx->break_join_state);
    }

    flow1_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);

    flow1_arena_remove_state(ctx, ctx->initial_state);
    flow1_arena_remove_state(ctx, ctx->break_join_state);


    //restore
    ctx->initial_state = old_initial_state;
    ctx->break_join_state = old_break_join_state;
    flow1_true_false_set_destroy(&true_false_set);
}
static void flow1_visit_for_statement(struct flow1_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{

    assert(p_iteration_statement->first_token->type == TK_KEYWORD_FOR);
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    struct expression* _Opt p_next = p_iteration_statement->expression2;
    struct expression* _Opt p_condition = p_iteration_statement->expression1;

    if (p_iteration_statement->declaration &&
        p_iteration_statement->declaration->init_declarator_list.head)
    {
        /*
            for (int i = 0;        i < 10;      i++)
                 ^^^^^^
                 init_declarator_list
        */
        flow1_visit_init_declarator_list(ctx, &p_iteration_statement->declaration->init_declarator_list);
    }
    else  if (p_iteration_statement->expression0)
    {
        /*
            int i;
            for (i = 0;        i < 10;      i++)
                 ^^^^^^
                 expression0
        */
        struct flow1_true_false_set true_false_set = { 0 };
        flow1_visit_full_expression(ctx, p_iteration_statement->expression0, &true_false_set);
        flow1_true_false_set_destroy(&true_false_set);
    }


    const int old_initial_state = ctx->initial_state;
    const int old_break_join_state = ctx->break_join_state;

    ctx->initial_state = flow1_arena_add_copy_of_current_state(ctx, "original");
    ctx->break_join_state = flow1_arena_add_empty_state(ctx, "break join");



    /*
        we do like this to acumulate states.

        if (expression)
        {
           statements...
           if (expression)
           {
             statements...
           }
        }
        break_exit:
    */

    if (p_condition)
    {
        /*
            for (int * _Opt p = 0; p; )
            {
                f(p); //complains
            }
        */
        bool inside_loop = ctx->inside_loop;
        ctx->inside_loop = false;
        flow1_check_pointer_used_as_bool(ctx, p_condition);
        ctx->inside_loop = inside_loop;
    }

    //We do a visit but this is not conclusive..so we ignore warnings
    diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);

    struct flow1_true_false_set true_false_set_false_branch = { 0 };

    if (p_condition)
    {
        flow1_visit_full_expression(ctx, p_condition, &true_false_set_false_branch);
        flow1_true_false_set_set_objects_to_true_branch(ctx, &true_false_set_false_branch, nullable_enabled);
    }


    flow1_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
    if (p_next)
    {
        struct flow1_true_false_set true_false_set = { 0 };
        flow1_visit_full_expression(ctx, p_next, &true_false_set);
        //true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);
        flow1_true_false_set_destroy(&true_false_set);
    }

    //Second pass warning is ON
    diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);



    if (p_condition)
    {
        struct flow1_true_false_set true_false_set = { 0 };
        flow1_visit_full_expression(ctx, p_condition, &true_false_set);
        flow1_true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);
        flow1_true_false_set_destroy(&true_false_set);
    }

    flow1_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
    if (p_next)
    {
        struct flow1_true_false_set true_false_set = { 0 };
        flow1_visit_full_expression(ctx, p_next, &true_false_set);
        //true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);
        flow1_true_false_set_destroy(&true_false_set);
    }

    if (p_condition)
    {
        struct flow1_true_false_set true_false_set = { 0 };
        flow1_visit_full_expression(ctx, p_condition, &true_false_set);
        flow1_true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
        flow1_true_false_set_destroy(&true_false_set);
    }
    flow1_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);



    const bool was_last_statement_inside_true_branch_return =
        secondary_block_ends_with_jump(p_iteration_statement->secondary_block);

    if (was_last_statement_inside_true_branch_return)
    {
        /*
           while (p) { return; }
        */
        flow1_arena_restore_current_state_from(ctx, ctx->initial_state);
        flow1_true_false_set_set_objects_to_false_branch(ctx, &true_false_set_false_branch, nullable_enabled);
    }
    else
    {
        flow1_true_false_set_set_objects_to_false_branch(ctx, &true_false_set_false_branch, nullable_enabled);
        flow1_arena_merge_current_state_with_state_number(ctx, ctx->break_join_state);
        flow1_arena_restore_current_state_from(ctx, ctx->break_join_state);
    }

    flow1_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);

    flow1_arena_remove_state(ctx, ctx->initial_state);
    flow1_arena_remove_state(ctx, ctx->break_join_state);


    //restore
    ctx->initial_state = old_initial_state;
    ctx->break_join_state = old_break_join_state;
    flow1_true_false_set_destroy(&true_false_set_false_branch);

}

static void flow1_visit_iteration_statement(struct flow1_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    bool inside_loop = ctx->inside_loop;
    ctx->inside_loop = true;

    switch (p_iteration_statement->first_token->type)
    {
    case  TK_KEYWORD_WHILE:
        flow1_visit_while_statement(ctx, p_iteration_statement);
        break;
    case TK_KEYWORD_DO:
        flow1_visit_do_while_statement(ctx, p_iteration_statement);
        break;
    case TK_KEYWORD_FOR:
        flow1_visit_for_statement(ctx, p_iteration_statement);
        break;
    default:
        assert(false);
        break;
    }
    ctx->inside_loop = inside_loop; //restore

    if (p_iteration_statement->p_lint_token)
    {
        flow1_check_dianostic_suppression(ctx, p_iteration_statement->p_lint_token);
    }
}

static void flow1_visit_jump_statement(struct flow1_visit_ctx* ctx, struct jump_statement* p_jump_statement)
{
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;
    try
    {
        if (p_jump_statement->first_token->type == TK_KEYWORD_CAKE_THROW)
        {
            flow1_arena_merge_current_state_with_state_number(ctx, ctx->throw_join_state);

            //check_all_defer_until_try(ctx, ctx->tail_block, p_jump_statement->first_token);
            flow1_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);

        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_RETURN)
        {
            if (ctx->p_return_type == NULL)
            {
                //we must be inside a function and we need this return set.
                throw;
            }

            if (p_jump_statement->expression_opt)
            {
                struct flow1_true_false_set d = { 0 };
                flow1_visit_full_expression(ctx, p_jump_statement->expression_opt, &d);
                flow1_true_false_set_destroy(&d);
            }

            /*
              returning a declarator will move the ownership
            */
            if (p_jump_statement->expression_opt)
            {
                struct flow1_object* _Opt p_object =
                    flow1_expression_get_flow_object(ctx, p_jump_statement->expression_opt, nullable_enabled);

                if (p_object)
                {
                    assert(ctx->p_return_type != NULL);
                    struct flow1_object* _Opt p_dest_object =
                        flow1_make_flow_object(ctx, ctx->p_return_type, NULL, p_jump_statement->expression_opt);

                    if (p_dest_object == NULL)
                    {
                        throw;
                    }

                    assert(ctx->p_return_type != NULL);
                    flow1_object_set_zero(ctx->p_return_type, p_dest_object);

                    struct marker a_marker = {
                       .p_token_begin = p_jump_statement->expression_opt->first_token,
                       .p_token_end = p_jump_statement->expression_opt->last_token,
                    };
                    struct marker b_marker = {
                       .p_token_begin = p_jump_statement->expression_opt->first_token,
                       .p_token_end = p_jump_statement->expression_opt->last_token,
                    };

                    assert(ctx->p_return_type != NULL);

                    flow1_check_assignment(ctx,
                     p_jump_statement->expression_opt->first_token,
                     &a_marker,
                     &b_marker,
                     ASSIGMENT_TYPE_RETURN,
                     true,
                        type_is_view(ctx->p_return_type), /*dest type*/
                        type_is_opt(ctx->p_return_type, ctx->ctx->options.null_checks_enabled), /*dest type*/
                        ctx->p_return_type, /*dest type*/
                        p_dest_object, /*dest object*/
                        &p_jump_statement->expression_opt->type, /*source type*/
                        p_object, /*source*/
                        NULL
                    );

                    const int state_before_return = flow1_arena_add_copy_of_current_state(ctx, "before-return");

                    // Simulate the function's state as if it had already returned. 
                    // This allows us to verify whether the returned object points to any 
                    // local variables.

                    flow1_defer_list_set_end_of_lifetime(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);

                    if (ctx->p_return_type == NULL)
                        throw;

                    flow1_checked_read_object(ctx,
                                        ctx->p_return_type,
                                        type_is_opt(ctx->p_return_type, ctx->ctx->options.null_checks_enabled),
                                        p_dest_object,
                                        NULL,
                                        &a_marker,
                                        true);

                    //then we restore the state
                    flow1_arena_restore_current_state_from(ctx, state_before_return);
                }

                if (p_object && p_object->is_temporary)
                {
                    //a + b
                    p_object->current.state = FLOW1_OBJECT_STATE_LIFE_TIME_ENDED;
                }

            }

            flow1_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);

        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_CONTINUE)
        {
            flow1_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_BREAK)
        {
            flow1_arena_merge_current_state_with_state_number(ctx, ctx->break_join_state);
            flow1_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_GOTO)
        {
            //goto needs a label
            assert(p_jump_statement->label != NULL);

            int label_state_number = -1;
            for (int i = 0; i < ctx->labels_size; i++)
            {
                if (ctx->labels[i].state_number == label_state_number)
                {
                    break; //already exist
                }
            }
            if (label_state_number == -1)
            {
                label_state_number = flow1_arena_add_empty_state(ctx, p_jump_statement->label->lexeme);
                ctx->labels[ctx->labels_size].state_number = label_state_number;
                ctx->labels[ctx->labels_size].label_name = p_jump_statement->label->lexeme;
                ctx->labels_size++;
            }

            flow1_arena_merge_current_state_with_state_number(ctx, label_state_number);
            flow1_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
        }
        else
        {
            assert(false);
        }
    }
    catch
    {
    }
    if (p_jump_statement->p_lint_token)
    {
        flow1_check_dianostic_suppression(ctx, p_jump_statement->p_lint_token);
    }
}

static void flow1_visit_label(struct flow1_visit_ctx* ctx, struct label* p_label);

static void flow1_visit_labeled_statement(struct flow1_visit_ctx* ctx, struct labeled_statement* p_labeled_statement)
{


    flow1_visit_label(ctx, p_labeled_statement->label);
    flow1_visit_statement(ctx, p_labeled_statement->statement);
}

static void flow1_visit_primary_block(struct flow1_visit_ctx* ctx, struct primary_block* p_primary_block)
{

    if (p_primary_block->compound_statement)
    {
        flow1_visit_compound_statement(ctx, p_primary_block->compound_statement);
    }
    else if (p_primary_block->iteration_statement)
    {
        flow1_visit_iteration_statement(ctx, p_primary_block->iteration_statement);
    }
    else if (p_primary_block->selection_statement)
    {
        flow1_visit_selection_statement(ctx, p_primary_block->selection_statement);
    }
    else if (p_primary_block->try_statement)
    {
        flow1_visit_try_statement(ctx, p_primary_block->try_statement);
    }
}

static void flow1_visit_unlabeled_statement(struct flow1_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement)
{

    if (p_unlabeled_statement->primary_block)
    {
        flow1_visit_primary_block(ctx, p_unlabeled_statement->primary_block);
    }
    else if (p_unlabeled_statement->expression_statement)
    {
        flow1_visit_expression_statement(ctx, p_unlabeled_statement->expression_statement);
    }
    else if (p_unlabeled_statement->defer_statement)
    {
        flow1_visit_defer_statement(ctx, p_unlabeled_statement->defer_statement);
    }
    else if (p_unlabeled_statement->jump_statement)
    {
        flow1_visit_jump_statement(ctx, p_unlabeled_statement->jump_statement);
    }
    else
    {
        assert(false);
    }

}

static void flow1_visit_statement(struct flow1_visit_ctx* ctx, struct statement* p_statement)
{
    if (p_statement->labeled_statement)
    {
        flow1_visit_labeled_statement(ctx, p_statement->labeled_statement);
    }
    else if (p_statement->unlabeled_statement)
    {
        flow1_visit_unlabeled_statement(ctx, p_statement->unlabeled_statement);
    }
}

static void flow1_visit_label(struct flow1_visit_ctx* ctx, struct label* p_label)
{
    if (p_label->p_identifier_opt)
    {
        for (int i = 0; i < ctx->labels_size; i++)
        {
            if (strcmp(ctx->labels[i].label_name, p_label->p_identifier_opt->lexeme) == 0)
            {

                flow1_arena_restore_current_state_from(ctx, ctx->labels[i].state_number);

                break; //already exist
            }
        }
    }
    else
    {
        //case, default
        flow1_arena_restore_current_state_from(ctx, ctx->initial_state);
    }

}

static void flow1_visit_block_item(struct flow1_visit_ctx* ctx, struct block_item* p_block_item)
{
    if (p_block_item->declaration)
    {
        flow1_visit_declaration(ctx, p_block_item->declaration);
    }
    else if (p_block_item->unlabeled_statement)
    {
        flow1_visit_unlabeled_statement(ctx, p_block_item->unlabeled_statement);
    }
    else if (p_block_item->label)
    {
        flow1_visit_label(ctx, p_block_item->label);
    }
}

static void flow1_visit_block_item_list(struct flow1_visit_ctx* ctx, struct block_item_list* p_block_item_list)
{
    struct block_item* _Opt p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        flow1_visit_block_item(ctx, p_block_item);
        p_block_item = p_block_item->next;
    }
}

static enum flow1_state flow1_parse_string_state(const char* s, bool* invalid)
{
    //TODO faling with _
    *invalid = false;

    enum flow1_state e = 0;
    const char* p = s;

    while (*p)
    {
        if (isalpha(*p))
        {
            int sz = 0;
            const char* start = p;
            while (isalpha(*p) || *p == '-')
            {
                sz++;
                p++;
            }

            if (strncmp(start, "moved", sz) == 0)
                e |= FLOW1_OBJECT_STATE_MOVED;

            else if (strncmp(start, "null", sz) == 0)
                e |= FLOW1_OBJECT_STATE_NULL;
            else if (strncmp(start, "not-null", sz) == 0)
                e |= FLOW1_OBJECT_STATE_NOT_NULL;
            else if (strncmp(start, "maybe-null", sz) == 0)
                e |= (FLOW1_OBJECT_STATE_NOT_NULL | FLOW1_OBJECT_STATE_NULL);
            else if (strncmp(start, "uninitialized", sz) == 0)
                e |= FLOW1_OBJECT_STATE_UNINITIALIZED;
            else if (strncmp(start, "zero", sz) == 0)
                e |= FLOW1_OBJECT_STATE_ZERO;
            else if (strncmp(start, "not-zero", sz) == 0)
                e |= FLOW1_OBJECT_STATE_NOT_ZERO;
            else if (strncmp(start, "any", sz) == 0)
                e |= (FLOW1_OBJECT_STATE_NOT_ZERO | FLOW1_OBJECT_STATE_ZERO);
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

static void flow1_visit_pragma_declaration(struct flow1_visit_ctx* ctx, struct pragma_declaration* p_pragma_declaration)
{
    execute_pragma_declaration(ctx->ctx, p_pragma_declaration, true);
}



static void flow1_visit_static_assertion(struct flow1_visit_ctx* ctx, struct static_assertion* p_static_assertion)
{
    const bool t2 = ctx->expression_is_not_evaluated;
    ctx->expression_is_not_evaluated = true;
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    struct flow1_true_false_set a = { 0 };
    flow1_visit_full_expression(ctx, p_static_assertion->constant_expression, &a);

    ctx->expression_is_not_evaluated = t2; //restore


    if (p_static_assertion->first_token->type == TK_KEYWORD_CAKE_STATIC_DEBUG ||
        p_static_assertion->first_token->type == TK_KEYWORD_CAKE_STATIC_DEBUG_EX)
    {
        bool ex = p_static_assertion->first_token->type == TK_KEYWORD_CAKE_STATIC_DEBUG_EX;


        struct flow1_object* _Opt p_obj =
            flow1_expression_get_flow_object(ctx, p_static_assertion->constant_expression, nullable_enabled);


        if (ex)
        {
            diagnostic(W_INFO,
                    ctx->ctx,
                    p_static_assertion->first_token,
                    NULL,
                    "debug");

            struct osstream ss = { 0 };
            flow1_print_arena(ctx, &ss);
            printf("%s", ss.c_str);

            ss_close(&ss);
        }
        else
        {
            if (p_obj)
            {
                struct osstream ss = { 0 };
                const bool color_enabled = !ctx->ctx->options.color_disabled;
                flow1_print_flow_object(color_enabled, &p_static_assertion->constant_expression->type, p_obj, !ex, &ss);
                if (p_obj->is_temporary)
                {
                    p_obj->current.state = FLOW1_OBJECT_STATE_LIFE_TIME_ENDED;
                }

                const struct marker m =
                {
                    .p_token_begin = p_static_assertion->constant_expression->first_token,
                    .p_token_end = p_static_assertion->constant_expression->last_token
                };

                diagnostic(W_INFO,
                    ctx->ctx,
                    NULL,
                    &m,
                    "%s", ss.c_str);
                ss_close(&ss);
            }
        }
    }
    else if (p_static_assertion->first_token->type == TK_KEYWORD_STATIC_STATE)
    {
        /*TODO
           check state

        */

        bool is_invalid = false;
        enum flow1_state e = 0;
        if (p_static_assertion->string_literal_opt)
            e = flow1_parse_string_state(p_static_assertion->string_literal_opt->lexeme, &is_invalid);
        if (is_invalid)
        {
            diagnostic(C_FLOW_ANALIZER_ERROR_STATIC_STATE_FAILED, ctx->ctx, p_static_assertion->first_token, NULL, "invalid parameter %s", p_static_assertion->string_literal_opt->lexeme);
        }
        else
        {

            struct flow1_object* _Opt p_obj =
                flow1_expression_get_flow_object(ctx, p_static_assertion->constant_expression, nullable_enabled);
            if (p_obj)
            {


                if (e != p_obj->current.state)
                {
                    struct osstream oss = { 0 };
                    diagnostic(C_FLOW_ANALIZER_ERROR_STATIC_STATE_FAILED, ctx->ctx, p_static_assertion->first_token, NULL, "assert_state failed");
                    if (p_static_assertion->string_literal_opt)
                        ss_fprintf(&oss, "expected :%s\n", p_static_assertion->string_literal_opt->lexeme);
                    ss_fprintf(&oss, "current  :");
                    flow1_object_print_state(p_obj, &oss);
                    ss_fprintf(&oss, "\n");
                    ss_close(&oss);
                }
            }
            else
            {
                if (e != FLOW1_OBJECT_STATE_NOT_APPLICABLE)
                {
                    diagnostic(C_FLOW_ANALIZER_ERROR_STATIC_STATE_FAILED, ctx->ctx, p_static_assertion->first_token, NULL, "assert_state failed");
                }
            }

            if (p_obj && p_obj->is_temporary)
            {
                p_obj->current.state = FLOW1_OBJECT_STATE_LIFE_TIME_ENDED;
            }

        }
    }
    else if (p_static_assertion->first_token->type == TK_KEYWORD_STATIC_SET)
    {
        struct flow1_object* _Opt p_obj =
            flow1_expression_get_flow_object(ctx, p_static_assertion->constant_expression, nullable_enabled);

        if (p_obj)
        {
            if (p_static_assertion->string_literal_opt)
            {
                const char* lexeme =
                    p_static_assertion->string_literal_opt->lexeme;

                if (strcmp(lexeme, "\"zero\"") == 0)
                {
                    //gives the semantics of {0} or calloc
                    flow1_object_set_zero(&p_static_assertion->constant_expression->type, p_obj);
                }
                else
                {
                    bool is_invalid = false;
                    enum flow1_state e =
                        flow1_parse_string_state(p_static_assertion->string_literal_opt->lexeme, &is_invalid);

                    if (!is_invalid)
                    {
                        if (p_obj->members.size > 0)
                        {
                            diagnostic(C_ERROR_STATIC_SET, ctx->ctx, p_static_assertion->first_token, NULL, "use only for non agregates");
                        }
                        p_obj->current.state = e;
                    }
                    else
                    {
                        diagnostic(C_ERROR_STATIC_SET, ctx->ctx, p_static_assertion->first_token, NULL, "invalid parameter %s", p_static_assertion->string_literal_opt->lexeme);
                    }
                }
            }

        }
        if (p_obj && p_obj->is_temporary)
        {
            p_obj->current.state = FLOW1_OBJECT_STATE_LIFE_TIME_ENDED;
        }

    }

    flow1_true_false_set_destroy(&a);
}

static void flow1_visit_direct_declarator(struct flow1_visit_ctx* ctx, struct direct_declarator* p_direct_declarator)
{
    if (p_direct_declarator->function_declarator)
    {
        struct parameter_declaration* _Opt parameter = NULL;

        if (p_direct_declarator->function_declarator->parameter_type_list_opt &&
            p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list)
        {
            parameter = p_direct_declarator->function_declarator->parameter_type_list_opt->parameter_list->head;
        }

        ctx->parameter_list++;

        while (parameter)
        {
            flow1_visit_declaration_specifiers(ctx, parameter->declaration_specifiers, &parameter->declarator->type);

            if (parameter->declarator)
            {
                flow1_visit_declarator(ctx, parameter->declarator);
            }

            parameter = parameter->next;
        }

        ctx->parameter_list--;

    }
    else if (p_direct_declarator->array_declarator)
    {
        if (p_direct_declarator->array_declarator->assignment_expression)
        {
            struct flow1_true_false_set a = { 0 };
            flow1_visit_full_expression(ctx, p_direct_declarator->array_declarator->assignment_expression, &a);
            flow1_true_false_set_destroy(&a);
        }

    }
}

static void flow1_visit_declarator(struct flow1_visit_ctx* ctx, struct declarator* p_declarator)
{
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;
    try
    {
        if (
            p_declarator->type.category != TYPE_CATEGORY_FUNCTION)
        {

            if (ctx->parameter_list > 1)
            {
                /*
                 The objective here is to avoid including the arguments
                 of function pointers inside the scope.
                 Sample
                 void x_destroy(void (*f)(void * _Owner p))
                 We add f but not p.
                */
                return;
            }


            p_declarator->p_flow_object = flow1_make_flow_object(ctx, &p_declarator->type, p_declarator, NULL);
            if (p_declarator->p_flow_object == NULL)
            {
                throw;
            }

            flow1_object_set_uninitialized(&p_declarator->type, p_declarator->p_flow_object);


            if (p_declarator->declaration_specifiers &&
                p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_PARAMETER)
            {
                if (type_is_pointer(&p_declarator->type))
                {
                    if (type_is_opt(&p_declarator->type, ctx->ctx->options.null_checks_enabled))
                    {
                        p_declarator->p_flow_object->current.state = FLOW1_OBJECT_STATE_NOT_NULL | FLOW1_OBJECT_STATE_NULL;
                    }
                    else
                    {
                        p_declarator->p_flow_object->current.state = FLOW1_OBJECT_STATE_NOT_NULL;
                    }

                    if (type_is_pointer_to_out(&p_declarator->type))
                    {
                        struct type t = type_remove_pointer(&p_declarator->type);
                        struct flow1_object* _Opt po = flow1_make_flow_object(ctx, &t, p_declarator, NULL);
                        if (po == NULL)
                        {
                            type_destroy(&t);
                            throw;
                        }
                        flow1_object_set_uninitialized(&t, po);
                        flow1_object_set_pointer(p_declarator->p_flow_object, po); //MOVED                    
                        type_destroy(&t);
                    }
                    else if (type_is_owner_or_pointer_to_dtor(&p_declarator->type))
                    {
                        struct type t = type_remove_pointer(&p_declarator->type);
                        struct flow1_object* _Opt po = flow1_make_flow_object(ctx, &t, p_declarator, NULL);
                        if (po == NULL)
                        {
                            type_destroy(&t);
                            throw;
                        }
                        const bool t_is_nullable = type_is_opt(&t, ctx->ctx->options.null_checks_enabled);
                        flow1_object_set_unknown(&t, t_is_nullable, po, nullable_enabled);
                        flow1_object_set_pointer(p_declarator->p_flow_object, po); //MOVED                    
                        type_destroy(&t);
                    }
                }
                else if (type_is_struct_or_union(&p_declarator->type))
                {
                    const bool is_nullable = type_is_opt(&p_declarator->type, nullable_enabled);
                    flow1_object_set_unknown(&p_declarator->type, is_nullable, p_declarator->p_flow_object, nullable_enabled);
                }
                else if (type_is_array(&p_declarator->type))
                {
                    // assert(false);//TODO
                     //flow_object_set_unknown(&p_declarator->type, &p_declarator->object);
                    p_declarator->p_flow_object->current.state = FLOW1_OBJECT_STATE_NOT_ZERO;
                }
                else
                {
                    p_declarator->p_flow_object->current.state = FLOW1_OBJECT_STATE_ZERO | FLOW1_OBJECT_STATE_NOT_ZERO;
                }


#if 0
                if (type_is_pointer(&p_declarator->type))
                {
                    //TODO necessary?
                    struct type t2 = type_remove_pointer(&p_declarator->type);
                    if (p_declarator->p_object->pointed)
                    {
                        set_object(&t2, p_declarator->p_object->pointed, (FLOW_OBJECT_STATE_NOT_NULL | FLOW_OBJECT_STATE_NULL));
                    }
                    type_destroy(&t2);
                }
#endif
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
            flow1_visit_direct_declarator(ctx, p_declarator->direct_declarator);
        }
    }
    catch
    {
    }
}

static void flow1_visit_init_declarator_list(struct flow1_visit_ctx* ctx, struct init_declarator_list* p_init_declarator_list)
{
    struct init_declarator* _Opt p_init_declarator = p_init_declarator_list->head;
    while (p_init_declarator)
    {
        flow1_visit_init_declarator(ctx, p_init_declarator);
        p_init_declarator = p_init_declarator->next;
    }
}

static void flow1_visit_member_declarator(struct flow1_visit_ctx* ctx, struct member_declarator* p_member_declarator)
{
    if (p_member_declarator->declarator)
    {
        flow1_visit_declarator(ctx, p_member_declarator->declarator);
    }
}

static void flow1_visit_member_declarator_list(struct flow1_visit_ctx* ctx, struct member_declarator_list* p_member_declarator_list)
{
    struct member_declarator* _Opt p_member_declarator = p_member_declarator_list->head;
    while (p_member_declarator)
    {
        flow1_visit_member_declarator(ctx, p_member_declarator);
        p_member_declarator = p_member_declarator->next;
    }
}

static void flow1_visit_member_declaration(struct flow1_visit_ctx* ctx, struct member_declaration* p_member_declaration)
{
    if (p_member_declaration->member_declarator_list_opt)
    {
        flow1_visit_member_declarator_list(ctx, p_member_declaration->member_declarator_list_opt);
    }
}

static void flow1_visit_member_declaration_list(struct flow1_visit_ctx* ctx, struct member_declaration_list* p_member_declaration_list)
{
    struct member_declaration* _Opt p_member_declaration = p_member_declaration_list->head;
    while (p_member_declaration)
    {
        flow1_visit_member_declaration(ctx, p_member_declaration);
        p_member_declaration = p_member_declaration->next;
    }
}

static void flow1_visit_struct_or_union_specifier(struct flow1_visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier)
{
    flow1_visit_member_declaration_list(ctx, &p_struct_or_union_specifier->member_declaration_list);
}

static void flow1_visit_enumerator(struct flow1_visit_ctx* ctx, struct enumerator* p_enumerator)
{
    struct flow1_true_false_set a = { 0 };
    if (p_enumerator->constant_expression_opt)
        flow1_visit_full_expression(ctx, p_enumerator->constant_expression_opt, &a);
    flow1_true_false_set_destroy(&a);
}

static void flow1_visit_enumerator_list(struct flow1_visit_ctx* ctx, struct enumerator_list* p_enumerator_list)
{
    struct enumerator* _Opt current = p_enumerator_list->head;
    while (current)
    {
        flow1_visit_enumerator(ctx, current);
        current = current->next;
    }
}

static void flow1_visit_enum_specifier(struct flow1_visit_ctx* ctx, struct enum_specifier* p_enum_specifier)
{
    flow1_visit_enumerator_list(ctx, &p_enum_specifier->enumerator_list);
}

static void flow1_visit_type_specifier(struct flow1_visit_ctx* ctx, struct type_specifier* p_type_specifier)
{
    if (p_type_specifier->struct_or_union_specifier)
    {
        flow1_visit_struct_or_union_specifier(ctx, p_type_specifier->struct_or_union_specifier);
    }

    if (p_type_specifier->enum_specifier)
    {
        flow1_visit_enum_specifier(ctx, p_type_specifier->enum_specifier);
    }
}

static void flow1_visit_type_specifier_qualifier(struct flow1_visit_ctx* ctx, struct type_specifier_qualifier* p_type_specifier_qualifier)
{
    if (p_type_specifier_qualifier->type_specifier)
    {
        flow1_visit_type_specifier(ctx, p_type_specifier_qualifier->type_specifier);
    }
}

static void flow1_visit_declaration_specifier(struct flow1_visit_ctx* ctx, struct declaration_specifier* p_declaration_specifier)
{
    if (p_declaration_specifier->type_specifier_qualifier)
    {
        flow1_visit_type_specifier_qualifier(ctx, p_declaration_specifier->type_specifier_qualifier);
    }
}

static void flow1_visit_declaration_specifiers(struct flow1_visit_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers,
    struct type* _Opt p_type_opt)
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

    struct declaration_specifier* _Opt p_declaration_specifier = p_declaration_specifiers->head;


    while (p_declaration_specifier)
    {
        flow1_visit_declaration_specifier(ctx, p_declaration_specifier);
        p_declaration_specifier = p_declaration_specifier->next;
    }
}

/*
* retorna true se o ultimo item for um return
*/
static bool flow1_is_last_item_return(struct compound_statement* p_compound_statement)
{
    if (p_compound_statement &&
        p_compound_statement->block_item_list.tail &&
        p_compound_statement->block_item_list.tail->unlabeled_statement &&
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement &&
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement->first_token &&
        p_compound_statement->block_item_list.tail->unlabeled_statement->jump_statement->first_token->type == TK_KEYWORD_RETURN) //lint 28 28
    {
        return true;
    }
    return false;
}

void flow1_visit_declaration(struct flow1_visit_ctx* ctx, struct declaration* p_declaration)
{
    if (p_declaration->static_assertion)
    {
        flow1_visit_static_assertion(ctx, p_declaration->static_assertion);
    }

    if (p_declaration->pragma_declaration)
    {
        flow1_visit_pragma_declaration(ctx, p_declaration->pragma_declaration);
    }


    if (p_declaration->declaration_specifiers)
    {
        if (p_declaration->init_declarator_list.head)
        {
            flow1_visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers,
                &p_declaration->init_declarator_list.head->p_declarator->type);
        }
        else
        {
            flow1_visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers, NULL);

        }
    }

    if (p_declaration->init_declarator_list.head)
    {
        flow1_visit_init_declarator_list(ctx, &p_declaration->init_declarator_list);
    }

    if (p_declaration->function_body)
    {
        assert(ctx->p_return_type == NULL);

        struct type type = get_function_return_type(&p_declaration->init_declarator_list.head->p_declarator->type);
        ctx->p_return_type = &type;

        flow1_visit_compound_statement(ctx, p_declaration->function_body);
        type_destroy(&type);
        ctx->p_return_type = NULL;
    }


    if (p_declaration->function_body)
    {
        if (!flow1_is_last_item_return(p_declaration->function_body))
            flow1_exit_block_visit_defer_list(ctx, &p_declaration->defer_list, p_declaration->function_body->last_token);
    }


    if (p_declaration->lint_token)
    {
        flow1_check_dianostic_suppression(ctx, p_declaration->lint_token);
    }
}

void flow1_start_visit_declaration(struct flow1_visit_ctx* ctx, struct declaration* p_declaration)
{


    //p_declaration->p_attribute_specifier_sequence

    ctx->labels_size = 0;
    flow1_objects_clear(&ctx->arena);

    ctx->state_number_generator = 1; //reserva 0 p current

    if (p_declaration->function_body)
    {
        flow1_visit_declaration(ctx, p_declaration);

    }
    else
    {
        flow1_visit_declaration(ctx, p_declaration);
    }

    flow1_objects_clear(&ctx->arena);
}


struct flow1_object* _Opt arena_new_object(struct flow1_visit_ctx* ctx)
{
    _Opt struct flow1_object* _Owner _Opt p = calloc(1, sizeof * p);
    if (p != NULL)
    {
        p->current.dbg_name = "current";
        p->id = ctx->arena.size + 1;
        if (flow1_objects_push_back(&ctx->arena, p) != 0)
        {
            p = NULL;
        }
    }
    return (struct flow1_object* _Opt) p; //lint 30
}


void flow1_visit_ctx_destroy(_Dtor struct flow1_visit_ctx* p)
{
    flow1_objects_destroy(&p->arena);
}