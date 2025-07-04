/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#pragma safety enable

#include "ownership.h"
#include <assert.h>
#include <string.h>
#include <assert.h>
#include "flow.h"
#include "expressions.h"
#include "ownership.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include "console.h"

struct object_visitor
{
    int member_index;
    struct type* p_type;
    struct flow_object* p_object;
};

unsigned int s_visit_number = 1; //creates a unique number
_Opt struct flow_object* _Opt arena_new_object(struct flow_visit_ctx* ctx);

bool flow_object_is_not_null(const struct flow_object* p)
{
    enum flow_state e = p->current.state;
    return (
             !(e & FLOW_OBJECT_STATE_NULL) &&
             ((e & FLOW_OBJECT_STATE_NOT_NULL) || (e & FLOW_OBJECT_STATE_MOVED))
           );

}

bool flow_object_can_be_not_null_or_moved(const struct flow_object* p)
{
    enum flow_state e = p->current.state;
    return (e & FLOW_OBJECT_STATE_NOT_NULL) ||
        (e & FLOW_OBJECT_STATE_MOVED);
}

bool flow_object_is_null(const struct flow_object* p)
{
    enum flow_state e = p->current.state;
    return ((e & FLOW_OBJECT_STATE_NULL) &&
           !(e & FLOW_OBJECT_STATE_NOT_NULL) &&
           !(e & FLOW_OBJECT_STATE_MOVED));
}

bool flow_object_is_zero(const struct flow_object* p)
{
    enum flow_state e = p->current.state;
    return ((e & FLOW_OBJECT_STATE_ZERO) &&
            !(e & FLOW_OBJECT_STATE_NOT_ZERO));
}

bool flow_object_is_not_zero(const struct flow_object* p)
{
    enum flow_state e = p->current.state;
    return (!(e & FLOW_OBJECT_STATE_ZERO) &&
           (e & FLOW_OBJECT_STATE_NOT_ZERO));
}


bool flow_object_can_be_zero(const struct flow_object* p)
{
    enum flow_state e = p->current.state;

    return (e & FLOW_OBJECT_STATE_ZERO);
}

bool flow_object_can_be_moved(const struct flow_object* p)
{
    enum flow_state e = p->current.state;

    return (e & FLOW_OBJECT_STATE_MOVED);
}

bool flow_object_can_be_null(const struct flow_object* p)
{
    enum flow_state e = p->current.state;

    return (e & FLOW_OBJECT_STATE_NULL);
}

bool flow_object_is_uninitialized(const struct flow_object* p)
{
    enum flow_state e = p->current.state;
    return e == FLOW_OBJECT_STATE_UNINITIALIZED;
}

bool flow_object_can_be_uninitialized(const struct flow_object* p)
{
    enum flow_state e = p->current.state;
    return (e & FLOW_OBJECT_STATE_UNINITIALIZED);
}

bool flow_object_can_have_its_lifetime_ended(const struct flow_object* p)
{
    enum flow_state e = p->current.state;
    return (e & FLOW_OBJECT_STATE_LIFE_TIME_ENDED);
}


static void object_state_to_string_core(enum flow_state e)
{
    bool first = true;

    printf("\"");
    if (e & FLOW_OBJECT_STATE_UNINITIALIZED)
    {
        if (first)
            first = false;
        else
            printf(" | ");
        printf("uninitialized");
    }

    if (e & FLOW_OBJECT_STATE_NOT_NULL)
    {
        if (first)
            first = false;
        else
            printf(" | ");
        printf("not-null");
    }

    if (e & FLOW_OBJECT_STATE_NULL)
    {
        if (first)
            first = false;
        else
            printf(" | ");
        printf("null");
    }

    if (e & FLOW_OBJECT_STATE_ZERO)
    {
        if (first)
            first = false;
        else
            printf(" | ");
        printf("zero");
    }

    if (e & FLOW_OBJECT_STATE_NOT_ZERO)
    {
        if (first)
            first = false;
        else
            printf(" | ");
        printf("not-zero");
    }


    if (e & FLOW_OBJECT_STATE_LIFE_TIME_ENDED)
    {
        if (first)
            first = false;
        else
            printf(" | ");
        printf("lifetime-ended");
    }

    if (e & FLOW_OBJECT_STATE_MOVED)
    {
        if (first)
            first = false;
        else
            printf(" | ");
        printf("moved");
    }

    printf("\"");

}

void flow_object_state_delete(struct flow_object_state* _Owner _Opt p)
{
    if (p)
    {
        objects_view_destroy(&p->alternatives);
        assert(p->next == NULL);
        free(p);
    }
}
void flow_object_state_copy(struct flow_object_state* to, const struct flow_object_state* from)
{
    to->state = from->state;
    to->pointed = from->pointed;
    objects_view_copy(&to->alternatives, &from->alternatives);
}

void flow_object_print_state(struct flow_object* p)
{
    object_state_to_string_core(p->current.state);
}

void object_state_set_item_print(struct flow_object_state* item)
{
    object_state_to_string_core(item->state);
}

bool flow_object_is_expansible(const struct flow_object* _Opt p_object)
{
    return (p_object &&
            p_object->current.state != FLOW_OBJECT_STATE_NULL &&
                p_object->current.state != FLOW_OBJECT_STATE_UNINITIALIZED &&
                p_object->current.pointed == 0);
}

void flow_object_expand_pointer(struct flow_visit_ctx* ctx, struct type* p_type, struct flow_object* p_object)
{
    assert(type_is_pointer_or_array(p_type));

    if (flow_object_is_expansible(p_object))
    {
        const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

        struct type t2 = type_remove_pointer(p_type);
        if (!type_is_void(&t2))
        {
            struct flow_object* _Opt p_object2 = make_flow_object(ctx, &t2, p_object->p_declarator_origin, p_object->p_expression_origin);
            if (p_object2)
            {
                const bool is_nullable = type_is_opt(&t2, nullable_enabled);
                flow_object_set_unknown(&t2, is_nullable, p_object2, nullable_enabled);
                object_set_pointer(p_object, p_object2);////obj.pointed2 = p_object;
                flow_object_push_states_from(p_object, p_object2);
            }
            else
            {
                //ops
            }
        }
        type_destroy(&t2);
    }
}

void flow_object_swap(struct flow_object* a, struct flow_object* b)
{
    struct flow_object temp = *a;
    *a = *b;
    *b = temp;
}

void flow_object_delete(struct flow_object* _Owner _Opt p)
{
    if (p)
    {
        flow_object_destroy(p);
        free(p);
    }
}
int objects_view_reserve(struct flow_objects_view* p, int n);
void object_set_pointer(struct flow_object* p_object, struct flow_object* p_object2)
{
    p_object->current.pointed = p_object2;
}

void flow_object_destroy(_Dtor struct flow_object* p)
{
    objects_view_destroy(&p->members);

    struct flow_object_state* _Owner _Opt p_flow_object_state = p->current.next;
    while (p_flow_object_state)
    {
        struct flow_object_state* _Owner _Opt temp = p_flow_object_state->next;
        p_flow_object_state->next = NULL;
        flow_object_state_delete(p_flow_object_state);
        p_flow_object_state = temp;
    }
    objects_view_destroy(&p->current.alternatives);
}

void flow_object_update_current(struct flow_object* p)
{
    if (p->current.alternatives.size > 0)
    {
        p->current.state = 0;
        for (int i = 0; i < p->current.alternatives.size; i++)
        {
            //infinite recursion?
            flow_object_update_current(p->current.alternatives.data[i]);
            p->current.state |= p->current.alternatives.data[i]->current.state;
        }
    }
}

void flow_object_set_current_state_to_can_be_null(struct flow_object* p)
{
    p->current.state |= FLOW_OBJECT_STATE_NULL;
}

void flow_object_set_current_state_to_is_null(struct flow_object* p)
{
    p->current.state = FLOW_OBJECT_STATE_NULL;
    if (p->current.alternatives.size > 0)
    {
        for (int i = 0; i < p->current.alternatives.size; i++)
        {
            flow_object_set_current_state_to_can_be_null(p->current.alternatives.data[i]);
        }
    }
}

int flow_object_add_state(struct flow_object* p, struct flow_object_state* _Owner pnew)
{
    assert(pnew->next == NULL);
    pnew->next = p->current.next;
    p->current.next = pnew;
    return 0;
}





void objects_view_destroy(_Dtor struct flow_objects_view* p)
{
    free(p->data);
}


int objects_view_reserve(struct flow_objects_view* p, int n)
{
    if (n > p->capacity)
    {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0]))))
        {
            return EOVERFLOW;
        }

        void* _Owner _Opt pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL) return ENOMEM;

        static_set(p->data, "moved"); //p->data was moved to pnew

        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}


int objects_view_push_back(struct flow_objects_view* p, struct flow_object* p_object)
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

        int error = objects_view_reserve(p, new_capacity);
        if (error != 0)
        {
            return error;
        }
    }

    p->data[p->size] = p_object;


    p->size++;

    return 0;
}

void objects_view_clear(struct flow_objects_view* p)
{
    p->size = 0;
}

void objects_view_merge(struct flow_objects_view* dest, const struct flow_objects_view* source)
{
    for (int i = 0; i < source->size; i++)
    {
        if (!objects_view_find(dest, source->data[i]))
        {
            objects_view_push_back(dest, source->data[i]);
        }
    }
}

void objects_view_copy(struct flow_objects_view* dest, const struct flow_objects_view* source)
{
    objects_view_reserve(dest, source->size);
    for (int i = 0; i < source->size; i++)
    {
        dest->data[i] = source->data[i];
    }
    dest->size = source->size;
}

bool objects_view_find(const struct flow_objects_view* p, const struct flow_object* p_object)
{
    for (int i = 0; i < p->size; i++)
    {
        if (p->data[i] == p_object)
            return true;
    }
    return false;
}
void flow_objects_destroy(_Dtor struct flow_objects* p)
{
    for (int i = 0; i < p->size; i++)
    {
        flow_object_delete(p->data[i]);
    }
    free(p->data);
}

void flow_objects_clear(struct flow_objects* p)
{
    for (int i = 0; i < p->size; i++)
    {
        flow_object_delete(p->data[i]);
    }
    p->size = 0;
}

int objects_reserve(struct flow_objects* p, int n)
{
    if (n > p->capacity)
    {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0]))))
        {
            return EOVERFLOW;
        }

        void* _Owner _Opt pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL) return ENOMEM;

        static_set(p->data, "moved"); //p->data was moved to pnew

        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

const struct flow_object* _Opt flow_objects_find(const struct flow_objects* p_objects, const struct flow_object* object)
{
    for (int i = 0; i < p_objects->size; i++)
    {
        if (p_objects->data[i] == object)
            return object;
    }
    return NULL;
}

int flow_objects_push_back(struct flow_objects* p, struct flow_object* _Owner p_object)
{
    if (p->size == INT_MAX)
    {
        flow_object_delete(p_object);
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
            flow_object_delete(p_object);
            return error;
        }
        assert(p->data != NULL);
    }

    p->data[p->size] = p_object; /*moved*/


    p->size++;

    return 0;
}
struct object_name_list
{
    const char* name;
    struct object_name_list* _Opt previous;
};

bool has_name(const char* name, struct object_name_list* list)
{
    struct object_name_list* _Opt p = list;

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

struct flow_object* _Opt make_object_core(struct flow_visit_ctx* ctx,
    struct type* p_type,
    struct object_name_list* list,
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


    struct flow_object* _Opt p_object = arena_new_object(ctx);

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
                p_object->current.state = FLOW_OBJECT_STATE_NOT_APPLICABLE;

                struct member_declaration* _Opt p_member_declaration =
                    p_struct_or_union_specifier->member_declaration_list.head;

                struct object_name_list l = {
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

                                if (tag && has_name(tag, &l))
                                {
                                    struct flow_object* _Opt member_obj = arena_new_object(ctx);
                                    if (member_obj == NULL)
                                    {
                                        throw;
                                    }

                                    member_obj->parent = p_object;

                                    member_obj->p_expression_origin = p_expression_origin;
                                    member_obj->p_declarator_origin = p_declarator_opt;
                                    member_obj->current.state = FLOW_OBJECT_STATE_NOT_APPLICABLE;
                                    objects_view_push_back(&p_object->members, member_obj);
                                }
                                else
                                {
                                    struct flow_object* _Opt p_member_obj =
                                        make_object_core(ctx,
                                            &p_member_declarator->declarator->type,
                                            &l,
                                            p_declarator_opt,
                                            p_expression_origin);

                                    if (p_member_obj == NULL)
                                    {
                                        throw;
                                    }
                                    p_member_obj->parent = p_object;
                                    objects_view_push_back(&p_object->members, p_member_obj);
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

                            struct flow_object* _Opt member_obj = make_object_core(ctx, &t, &l, p_declarator_opt, p_expression_origin);
                            if (member_obj == NULL)
                            {
                                type_destroy(&t);
                                throw;
                            }

                            for (int k = 0; k < member_obj->members.size; k++)
                            {
                                objects_view_push_back(&p_object->members, member_obj->members.data[k]);
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
            //p_object->state = flags;
            //if (p_object->members_size > 0)
            //{
            //    //not sure if we instanticate all items of array
            //    p_object->members[0].state = flags;
            //}
            //TODO create object for fixed len arrays!
            return p_object;
        }


        p_object->current.state = FLOW_OBJECT_STATE_UNINITIALIZED;
    }
    catch
    {
        p_object = NULL;
    }
    return p_object;
}

struct flow_object* _Opt make_flow_object(struct flow_visit_ctx* ctx,
    struct type* p_type,
                           const struct declarator* _Opt p_declarator_opt,
                           const struct expression* _Opt p_expression_origin)
{

    struct object_name_list list = { .name = "" };
    struct flow_object* _Opt p_object = make_object_core(ctx, p_type, &list, p_declarator_opt, p_expression_origin);

    return p_object;
}

struct token* _Opt flow_object_get_token(const struct flow_object* object)
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

void flow_object_add_new_state_as_a_copy_of_current_state(struct flow_object* object, const char* name, int state_number)
{
    _Opt struct flow_object_state* _Owner _Opt pnew = calloc(1, sizeof * pnew);
    if (pnew == NULL) return;//ENOMEM;

    pnew->dbg_name = name;
    pnew->state_number = state_number;


    pnew->next = object->current.next;
    object->current.next = pnew;

    flow_object_state_copy(pnew, &object->current);
    //TODO fix pnew is changed..
}

void flow_object_remove_state(struct flow_object* object, int state_number)
{
    struct flow_object_state* previous = &object->current;
    struct flow_object_state* _Opt it = object->current.next;
    while (it)
    {
        if (it->state_number == state_number)
        {
            struct flow_object_state* _Owner _Opt p_it_next = it->next;
            it->next = NULL;
            flow_object_state_delete(previous->next);
            previous->next = p_it_next;
            break;
        }
        previous = it;
        it = it->next;
    }

}

void print_object_core(int ident,
    struct object_visitor* p_visitor,
    const char* previous_names,
    bool is_pointer,
    bool short_version,
    unsigned int visit_number)
{
    if (p_visitor->p_object->visit_number == visit_number) return;
    p_visitor->p_object->visit_number = visit_number;

    if (p_visitor->p_type->struct_or_union_specifier)
    {
        assert(p_visitor->p_object->current.state == FLOW_OBJECT_STATE_NOT_APPLICABLE);
        struct struct_or_union_specifier* _Opt p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier)
        {
            printf("%*c", ident + 1, ' ');
            printf("#%02d {\n", p_visitor->p_object->id);

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

                            _Opt struct object_visitor visitor = { 0 };
                            visitor.p_type = &p_member_declarator->declarator->type;
                            visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];

                            print_object_core(ident + 2, &visitor, buffer,
                                type_is_pointer(&p_member_declarator->declarator->type), short_version,
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

                    int visit_number0 = p_visitor->p_object->visit_number;
                    p_visitor->p_object->visit_number = 0;
                    print_object_core(ident + 1,
                        p_visitor,
                        previous_names,
                        false,
                        short_version,
                        visit_number);

                    p_visitor->p_type = temp; //restore
                    p_visitor->p_object->visit_number = visit_number0;
                    type_destroy(&t);
                }
                p_member_declaration = p_member_declaration->next;
            }

            printf("%*c", ident + 1, ' ');
            printf("}\n");
        }
    }
    else if (type_is_pointer(p_visitor->p_type))
    {
        struct type t2 = type_remove_pointer(p_visitor->p_type);
        printf("%*c", ident, ' ');

        if (short_version)
        {
            printf("#%02d %s == ", p_visitor->p_object->id, previous_names);
            flow_object_print_state(p_visitor->p_object);
            if (flow_object_is_null(p_visitor->p_object))
            {
            }
            else if (flow_object_can_be_uninitialized(p_visitor->p_object))
            {
            }
            else
            {
                printf(" -> ");

                if (p_visitor->p_object->current.pointed != NULL)
                {
                    printf(" #%02d", p_visitor->p_object->current.pointed->id);
                }
                else
                {
                    printf("{...}");
                }
            }
        }
        else
        {
            printf("%p:%s == ", p_visitor->p_object, previous_names);
            printf("{");

            struct flow_object_state* _Opt it = p_visitor->p_object->current.next;
            while (it)
            {
                printf(LIGHTCYAN);
                printf("(#%02d %s)", it->state_number, it->dbg_name);
                object_state_set_item_print(it);
                printf(RESET);
                printf(",");
                it = it->next;
            }
            //printf("*");
            printf(LIGHTMAGENTA);
            printf("(current)");
            flow_object_print_state(p_visitor->p_object);
            printf(RESET);
            printf("}");
        }
        printf("\n");

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
        printf("%*c", ident, ' ');

        if (short_version)
        {
            printf("#%02d %s == ", p_visitor->p_object->id, previous_names);
            flow_object_print_state(p_visitor->p_object);
        }
        else
        {
            printf("%p:%s == ", p_visitor->p_object, previous_names);
            printf("{");

            struct flow_object_state* _Opt it = p_visitor->p_object->current.next;
            while (it)
            {
                printf("(#%02d %s)", it->state_number, it->dbg_name);
                object_state_set_item_print(it);
                printf(",");
                it = it->next;
            }


            flow_object_print_state(p_visitor->p_object);
            printf("}");
        }

        printf("\n");
    }


}


void flow_object_merge_state(struct flow_object* pdest, struct flow_object* object1, struct flow_object* object2)
{
    pdest->current.state = object1->current.state | object2->current.state;

    if (pdest->members.size == object1->members.size &&
        object1->members.size == object2->members.size)
    {
        for (int i = 0; i < object1->members.size; i++)
        {
            struct flow_object* m1 = object1->members.data[i];
            struct flow_object* m2 = object2->members.data[i];
            flow_object_merge_state(pdest->members.data[i], m1, m2);
        }
    }

#if 0
    for (int i = 0; i < object1->current.ref.size; i++)
    {
        struct flow_object* pointed = object1->current.ref.data[i];
        if (pointed)
        {
            objects_view_push_back(&pdest->current.ref, pointed);
        }
    }

    for (int i = 0; i < object2->current.ref.size; i++)
    {
        struct flow_object* pointed = object2->current.ref.data[i];
        if (pointed)
        {
            objects_view_push_back(&pdest->current.ref, pointed);
        }
    }

#endif


}


int object_restore_current_state_from_core(struct flow_object* object, int state_number, unsigned int visit_number)
{
    if (object->visit_number == visit_number) return 1;//already visited    
    object->visit_number = visit_number;


    struct flow_object_state* _Opt it = object->current.next;
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
        object_restore_current_state_from_core(object->members.data[i], state_number, visit_number);
    }
#if 0
    for (int i = 0; i < object->current.ref.size; i++)
    {
        struct flow_object* pointed = object->current.ref.data[i];

        if (pointed)
        {
            object_restore_current_state_from_core(pointed, state_number, visit_number);
        }
    }
#endif
    return 1;
}

int flow_object_restore_current_state_from(struct flow_object* object, int state_number)
{
    return object_restore_current_state_from_core(object, state_number, s_visit_number++);
}

int object_merge_current_state_with_state_number_core(struct flow_object* object, int state_number, unsigned int visit_number)
{
    if (object->visit_number == visit_number)
    {
        return 1;//already visited
    }
    object->visit_number = visit_number;

    struct flow_object_state* _Opt it = object->current.next;
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
        object_merge_current_state_with_state_number_core(object->members.data[i], state_number, visit_number);
    }
    if (object->current.state == FLOW_OBJECT_STATE_NULL ||
        object->current.state == FLOW_OBJECT_STATE_UNINITIALIZED) //moved
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

            struct flow_object* pointed = object->current.ref.data[i];
            if (pointed)
            {
                object_merge_current_state_with_state_number_core(pointed, state_number, visit_number);
            }

        }
#endif
    }
    return 1;
}


void object_get_name(const struct type* p_type,
    const struct flow_object* p_object,
    char* outname,
    int out_size);


void print_flow_object(struct type* p_type, struct flow_object* p_object, bool short_version)
{
    char name[100] = { 0 };
    object_get_name(p_type, p_object, name, sizeof name);

    _Opt struct object_visitor visitor = { 0 };
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    print_object_core(0, &visitor, name, type_is_pointer(p_type), short_version, s_visit_number++);
}


void object_set_uninitialized_core(struct object_visitor* p_visitor)
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
                                _Opt struct object_visitor visitor = { 0 };
                                visitor.p_type = &p_member_declarator->declarator->type;
                                visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];
                                object_set_uninitialized_core(&visitor);
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

                        object_set_uninitialized_core(p_visitor);

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
        p_visitor->p_object->current.state = FLOW_OBJECT_STATE_UNINITIALIZED;

#if 0
        if (p_visitor->p_object->current.ref.size)
        {
            struct type t2 = type_remove_pointer(p_visitor->p_type);
            for (int i = 0; i < p_visitor->p_object->current.ref.size; i++)
            {
                struct flow_object* pointed = p_visitor->p_object->current.ref.data[i];
                object_set_nothing(&t2, pointed);
            }
            type_destroy(&t2);
        }
#endif
    }
    else
    {
        p_visitor->p_object->current.state = FLOW_OBJECT_STATE_UNINITIALIZED;
    }
}

void flow_object_set_uninitialized(struct type* p_type, struct flow_object* p_object)
{
    _Opt struct object_visitor visitor = { 0 };
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    object_set_uninitialized_core(&visitor);
}

static void checked_empty_core(struct flow_visit_ctx* ctx,
    struct type* p_type,
    struct flow_object* p_object,
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


                        checked_empty_core(ctx, &p_member_declarator->declarator->type,
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
        if (p_object->current.state == (FLOW_OBJECT_STATE_UNINITIALIZED | FLOW_OBJECT_STATE_NULL | FLOW_OBJECT_STATE_MOVED) ||
            p_object->current.state == (FLOW_OBJECT_STATE_NULL | FLOW_OBJECT_STATE_MOVED) ||
            p_object->current.state == FLOW_OBJECT_STATE_NULL ||
            p_object->current.state == FLOW_OBJECT_STATE_MOVED ||
            p_object->current.state == FLOW_OBJECT_STATE_UNINITIALIZED ||
            p_object->current.state == (FLOW_OBJECT_STATE_UNINITIALIZED | FLOW_OBJECT_STATE_NULL) ||
            p_object->current.state == (FLOW_OBJECT_STATE_UNINITIALIZED | FLOW_OBJECT_STATE_MOVED)
            )
        {
        }
        else
        {

            compiler_diagnostic(W_FLOW_MISSING_DTOR,
                ctx->ctx,
                NULL,
                p_marker,
                "object '%s' may not be empty",
                previous_names);
        }
    }
}

void checked_empty(struct flow_visit_ctx* ctx,
    struct type* p_type,
    struct flow_object* p_object,
    const struct marker* p_marker)
{
    char name[100] = { 0 };
    object_get_name(p_type, p_object, name, sizeof name);

    checked_empty_core(ctx,
    p_type,
    p_object,
    name,
    p_marker);
}

static void object_set_moved_core(struct object_visitor* p_visitor)
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
                                _Opt struct object_visitor visitor = { 0 };
                                visitor.p_type = &p_member_declarator->declarator->type;
                                visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];
                                object_set_moved_core(&visitor);
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

                        object_set_moved_core(p_visitor);

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
        p_visitor->p_object->current.state = FLOW_OBJECT_STATE_MOVED;
    }
    else
    {
        p_visitor->p_object->current.state = FLOW_OBJECT_STATE_MOVED;
    }
}

void flow_object_set_moved(struct type* p_type, struct flow_object* p_object)
{
    _Opt struct object_visitor visitor = { 0 };
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    object_set_moved_core(&visitor);
}

static void object_set_unknown_core(struct object_visitor* p_visitor, bool t_is_nullable, unsigned int visit_number, bool nullable_enabled)
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
                                _Opt struct object_visitor visitor = { 0 };
                                visitor.p_type = &p_member_declarator->declarator->type;
                                visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];

                                object_set_unknown_core(&visitor,
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
                        object_set_unknown_core(p_visitor, t_is_nullable, visit_number, nullable_enabled);



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
            p_visitor->p_object->current.state = FLOW_OBJECT_STATE_NULL | FLOW_OBJECT_STATE_NOT_NULL;
        else
            p_visitor->p_object->current.state = FLOW_OBJECT_STATE_NOT_NULL;


        struct flow_object* _Opt pointed = p_visitor->p_object->current.pointed;
        if (pointed)
        {
            struct type t2 = type_remove_pointer(p_visitor->p_type);
            bool t2_is_nullable = type_is_opt(&t2, nullable_enabled);
            _Opt struct object_visitor visitor = { 0 };
            visitor.p_type = &t2;
            visitor.p_object = pointed;

            object_set_unknown_core(&visitor, t2_is_nullable, visit_number, nullable_enabled);
            type_destroy(&t2);
        }

    }
    else
    {
        if (!type_is_struct_or_union(p_visitor->p_type))
            p_visitor->p_object->current.state = FLOW_OBJECT_STATE_ZERO | FLOW_OBJECT_STATE_NOT_ZERO;
    }
}

void flow_object_set_unknown(struct type* p_type, bool t_is_nullable, struct flow_object* p_object, bool nullable_enabled)
{
    _Opt struct object_visitor visitor = { 0 };
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    object_set_unknown_core(&visitor, t_is_nullable, s_visit_number++, nullable_enabled);
}


static void object_set_deleted_core(struct type* p_type, struct flow_object* p_object, unsigned int visit_number)
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
                                object_set_deleted_core(&p_member_declarator->declarator->type, p_object->members.data[member_index], visit_number);
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
        p_object->current.state = FLOW_OBJECT_STATE_LIFE_TIME_ENDED;
#if 0
        for (int i = 0; i < p_object->current.ref.size; i++)
        {
            struct flow_object* pointed = p_object->current.ref.data[i];
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
            p_object->current.state = FLOW_OBJECT_STATE_LIFE_TIME_ENDED;
    }
}

void flow_object_set_can_be_uninitialized(struct flow_object* p_object)
{
    p_object->current.state |= FLOW_OBJECT_STATE_UNINITIALIZED;
}
void flow_object_set_is_unitialized(struct flow_object* p_object)
{
    p_object->current.state = FLOW_OBJECT_STATE_UNINITIALIZED;
    if (p_object->current.alternatives.size > 0)
    {
        for (int i = 0; i < p_object->current.alternatives.size; i++)
        {
            flow_object_set_can_be_uninitialized(p_object->current.alternatives.data[i]);
        }
    }
}

void flow_object_set_can_be_moved(struct flow_object* p_object)
{
    if (p_object->current.state & FLOW_OBJECT_STATE_NOT_NULL)
    {
        p_object->current.state &= ~FLOW_OBJECT_STATE_NOT_NULL;
        p_object->current.state |= FLOW_OBJECT_STATE_MOVED;
        p_object->current.state |= FLOW_OBJECT_STATE_MOVED;
    }
}
void flow_object_set_is_moved(struct flow_object* p_object)
{
    p_object->current.state = FLOW_OBJECT_STATE_MOVED;

    if (p_object->current.alternatives.size > 0)
    {
        for (int i = 0; i < p_object->current.alternatives.size; i++)
        {
            flow_object_set_can_be_moved(p_object->current.alternatives.data[i]);
        }
    }
}


void object_set_deleted(struct type* p_type, struct flow_object* p_object)
{
    object_set_deleted_core(p_type, p_object, s_visit_number++);
}

void object_set_zero_core(struct object_visitor* p_visitor)
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
                                _Opt struct object_visitor visitor = { 0 };
                                visitor.p_type = &p_member_declarator->declarator->type;
                                visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];
                                object_set_zero_core(&visitor);
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
                        object_set_zero_core(p_visitor);
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
        p_visitor->p_object->current.state = FLOW_OBJECT_STATE_NULL;
    }
    else
    {
        p_visitor->p_object->current.state = FLOW_OBJECT_STATE_ZERO;
    }
}

void flow_object_set_zero(struct type* p_type, struct flow_object* p_object)
{
    _Opt struct object_visitor visitor = { 0 };
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    object_set_zero_core(&visitor);
}

void object_set_end_of_lifetime_core(struct object_visitor* p_visitor)
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
                                _Opt struct object_visitor visitor = { 0 };
                                visitor.p_type = &p_member_declarator->declarator->type;
                                visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];
                                object_set_end_of_lifetime_core(&visitor);
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
                        object_set_end_of_lifetime_core(p_visitor);
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

    p_visitor->p_object->current.state = FLOW_OBJECT_STATE_LIFE_TIME_ENDED;
}

void flow_object_set_end_of_lifetime(struct type* p_type, struct flow_object* p_object)
{
    _Opt struct object_visitor visitor = { 0 };
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    object_set_end_of_lifetime_core(&visitor);
}

//returns true if all parts that need to be moved weren't moved.
bool object_check(struct type* p_type, struct flow_object* p_object)
{
    try
    {
        if (p_type->type_qualifier_flags & TYPE_QUALIFIER_VIEW)
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

                            if (object_check(&p_member_declarator->declarator->type,
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
                should_had_been_moved = (p_object->current.state & FLOW_OBJECT_STATE_NOT_NULL);
            }
            else
            {
                if (p_object->current.state == FLOW_OBJECT_STATE_UNINITIALIZED ||
                    (p_object->current.state & FLOW_OBJECT_STATE_MOVED) ||
                    p_object->current.state == FLOW_OBJECT_STATE_NOT_NULL ||
                    p_object->current.state == (FLOW_OBJECT_STATE_UNINITIALIZED))
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

void object_get_name_core(
    const struct type* p_type,
    const struct flow_object* p_object,
    const struct flow_object* p_object_target,
    const char* previous_names,
    char* outname,
    int out_size,
    unsigned int visit_number)
{
    try
    {
        if (p_object->visit_number == visit_number) return;
        ((struct flow_object*)p_object)->visit_number = visit_number;

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

                            object_get_name_core(
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


void object_get_name(const struct type* p_type,
    const struct flow_object* p_object,
    char* outname,
    int out_size)
{
    outname[0] = '\0';

    if (p_object->p_declarator_origin)
    {
        const char* root_name = p_object->p_declarator_origin->name_opt ? p_object->p_declarator_origin->name_opt->lexeme : "?";
        const struct flow_object* _Opt root = p_object->p_declarator_origin->p_flow_object;
        object_get_name_core(&p_object->p_declarator_origin->type, root, p_object, root_name, outname, out_size, s_visit_number++);
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
            assert(p != NULL);                            
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

static void checked_read_object_core(struct flow_visit_ctx* ctx,
    struct object_visitor* p_visitor,
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

                        _Opt struct object_visitor  visitor = { 0 };
                        visitor.p_type = &p_member_declarator->declarator->type;
                        visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];


                        checked_read_object_core(ctx,
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

                checked_read_object_core(ctx,
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
            flow_object_can_be_null(p_visitor->p_object))
        {
            compiler_diagnostic(W_FLOW_NULL_DEREFERENCE,
                ctx->ctx,
                NULL,
                p_marker_opt,
                "non-nullable pointer '%s' may be null",
                previous_names);
        }

        if (type_is_pointer(p_visitor->p_type) &&
            check_pointed_object &&
            flow_object_can_be_not_null_or_moved(p_visitor->p_object))
        {
            struct type t2 = type_remove_pointer(p_visitor->p_type);

            if (p_visitor->p_object->current.pointed)
            {
                _Opt struct object_visitor  visitor = { 0 };
                visitor.p_type = &t2;
                visitor.p_object = p_visitor->p_object->current.pointed;

                checked_read_object_core(ctx,
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


        if (p_visitor->p_object->current.state & FLOW_OBJECT_STATE_UNINITIALIZED)
        {
            if (type_is_array(p_visitor->p_type))
            {
                //unitialized arrays are used as initialized pointers to uninitialized flow_objects.
            }
            else
            {
                compiler_diagnostic(W_FLOW_UNINITIALIZED,
                    ctx->ctx,
                    position_token_opt,
                    p_marker_opt,
                    "uninitialized object '%s'",
                    previous_names);
            }
        }


        //TODO there is some problem with  FLOW_OBJECT_STATE_LIFE_TIME_ENDED
        //state somewhere!
        if (p_visitor->p_object->current.state & FLOW_OBJECT_STATE_LIFE_TIME_ENDED)
        {
            /*
               current implementation does not handle
               "dependent states". Many times the pointer is null
               and the pointed object does not exist anymore.
               But these two states are not related and it is causing
               too many false positives
            */

            //compiler_diagnostic(W_FLOW_LIFETIME_ENDED,
            //    ctx->ctx,
            //     position_token_opt,
            //    p_marker_opt,
            //    "lifetime ended '%s'",
            //    previous_names);
        }


    }
}

/*
  Check if all flow_objects connect to this one
   - are initialized
*/
void checked_read_object(struct flow_visit_ctx* ctx,
    struct type* p_type,
    bool is_nullable,
    struct flow_object* p_object,
    const struct token* _Opt position_token,
    const struct marker* _Opt p_marker_opt,
    bool check_pointed_object)
{
    const char* _Owner _Opt s = NULL;
    char name[200] = { 0 };

    object_get_name(p_type, p_object, name, sizeof name);

    _Opt struct object_visitor visitor = { 0 };
    visitor.p_object = p_object;
    visitor.p_type = p_type;

    checked_read_object_core(ctx,
    &visitor,
    is_nullable,
    position_token,
    p_marker_opt,
    check_pointed_object,
    name,
    s_visit_number++);
    free((void* _Owner)s);
}

static void flow_end_of_block_visit_core(struct flow_visit_ctx* ctx,
    struct object_visitor* p_visitor,
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

        if (object_check(p_visitor->p_type, p_visitor->p_object))
        {
            /*
            *  All parts of the object needs to be moved, so instead of
            *  describing each part we will just say that the object should
            *  have been moved.
            */
            const struct token* _Opt name = flow_object_get_token(p_visitor->p_object);
            if (compiler_diagnostic(W_FLOW_MISSING_DTOR,
                ctx->ctx,
                name, NULL,
                "members of '%s' were not released.",
                previous_names))
            {

                if (p_visitor->p_object->p_declarator_origin)
                    compiler_diagnostic(W_LOCATION, ctx->ctx, position_token, NULL, "end of '%s' scope", previous_names);
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

                            _Opt struct object_visitor visitor = { 0 };
                            visitor.p_type = &p_member_declarator->declarator->type;
                            visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];

                            flow_end_of_block_visit_core(ctx,
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
                    flow_end_of_block_visit_core(ctx,
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
            p_visitor->p_object->current.state & FLOW_OBJECT_STATE_NOT_NULL)
        {
            if (compiler_diagnostic(W_FLOW_MISSING_DTOR,
                ctx->ctx,
                position, NULL,
                "object pointed by '%s' was not released.", previous_names))
            {
                compiler_diagnostic(W_LOCATION,
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
                _Opt struct object_visitor visitor = { 0 };
                visitor.p_type = &t2;
                visitor.p_object = p_visitor->p_object->current.pointed;
                flow_end_of_block_visit_core(ctx, &visitor, b_type_is_view, position, buffer, visit_number);
            }


            type_destroy(&t2);
        }
        else if (type_is_owner(p_visitor->p_type) && !type_is_pointer(p_visitor->p_type))
        {
            //non-pointer _Owner
            if (p_visitor->p_object->current.state == FLOW_OBJECT_STATE_UNINITIALIZED ||
                p_visitor->p_object->current.state == FLOW_OBJECT_STATE_NULL ||
                p_visitor->p_object->current.state == FLOW_OBJECT_STATE_MOVED)
            {
            }
            else
            {
                if (compiler_diagnostic(W_FLOW_MISSING_DTOR,
                    ctx->ctx,
                    position, NULL,
                    "object pointed by '%s' was not released.", previous_names))
                {
                    compiler_diagnostic(W_LOCATION,
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
                //TODO static flow_objects
                    struct type t2 = type_remove_pointer(p_visitor->p_type);

                    if (p_visitor->p_object->current.pointed &&
                        p_visitor->p_object->p_declarator_origin)
                    {
                        struct token* _Opt name_token = p_visitor->p_object->p_declarator_origin->name_opt ?
                            p_visitor->p_object->p_declarator_origin->name_opt :
                            p_visitor->p_object->p_declarator_origin->first_token_opt;

                        checked_read_object(ctx,
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

void flow_end_of_block_visit(struct flow_visit_ctx* ctx,
    struct type* p_type,
    bool type_is_view,
    struct flow_object* p_object,
    const struct token* position_token,
    const char* previous_names)
{
    _Opt struct object_visitor visitor = { 0 };
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    flow_end_of_block_visit_core(ctx,
    &visitor,
    type_is_view,
    position_token,
    previous_names,
    s_visit_number++);
}

bool flow_object_is_zero_or_null(const struct flow_object* p_object)
{
    return (p_object->current.state == FLOW_OBJECT_STATE_NULL) ||
        (p_object->current.state == FLOW_OBJECT_STATE_ZERO);
}

/*
   This function must check and do the flow assignment of
   a = b
*/
static void flow_assignment_core(
    struct flow_visit_ctx* ctx,
    const struct token* error_position,
    const struct marker* p_a_marker,
    const struct marker* p_b_marker,
    enum assigment_type assigment_type,
    bool check_uninitialized_b,
    bool a_type_is_view,
    bool a_type_is_nullable,
    struct object_visitor* p_visitor_a,
    struct object_visitor* p_visitor_b,
    bool* _Opt  set_argument_to_unkown)
{
    //const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

#ifdef _DEBUG
    while (error_position->line == 25)
    {
        break;
    }
#endif
    // printf("line  %d\n", error_position->line);
     //type_print(p_a_type);
     //printf(" = ");
     //type_print(p_b_type);
     //printf("\n");

     /*general check for copying uninitialized object*/


    if (check_uninitialized_b &&
        flow_object_can_be_uninitialized(p_visitor_b->p_object))
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
                    object_get_name(p_visitor_b->p_type, p_visitor_b->p_object, b_object_name, sizeof b_object_name);
                    compiler_diagnostic(W_FLOW_UNINITIALIZED,
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
            object_get_name(p_visitor_b->p_type, p_visitor_b->p_object, b_object_name, sizeof b_object_name);

            if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
            {
                compiler_diagnostic(W_FLOW_UNINITIALIZED,
                            ctx->ctx,
                            NULL,
                            p_b_marker,
                    "passing an uninitialized argument '%s' object", b_object_name);
            }
            else if (assigment_type == ASSIGMENT_TYPE_RETURN)
            {
                compiler_diagnostic(W_FLOW_UNINITIALIZED,
                            ctx->ctx,
                            NULL,
                            p_b_marker,
                            "returning an uninitialized '%s' object", b_object_name);
            }
            else
            {
                compiler_diagnostic(W_FLOW_UNINITIALIZED,
                            ctx->ctx,
                            NULL,
                            p_b_marker,
                            "reading an uninitialized '%s' object", b_object_name);
            }
        }

        return;
    }

    if (check_uninitialized_b && flow_object_can_have_its_lifetime_ended(p_visitor_a->p_object))
    {
        //a = b where a was deleted
        char buffer[100] = { 0 };
        object_get_name(p_visitor_a->p_type, p_visitor_a->p_object, buffer, sizeof buffer);

        compiler_diagnostic(W_FLOW_LIFETIME_ENDED,
                    ctx->ctx,
                    NULL,
                    p_a_marker,
                    "The object '%s' may have been deleted or its lifetime have ended.", buffer);


        return;
    }

    /*general check passing possible null to non _Opt*/
    if (type_is_pointer(p_visitor_a->p_type) &&
        (!type_is_opt(p_visitor_a->p_type, ctx->ctx->options.null_checks_enabled)) &&
        flow_object_can_be_null(p_visitor_b->p_object))
    {
        if (!a_type_is_nullable)
        {
            char buffer[100] = { 0 };
            object_get_name(p_visitor_b->p_type, p_visitor_b->p_object, buffer, sizeof buffer);

            if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
            {
                compiler_diagnostic(W_FLOW_NULLABLE_TO_NON_NULLABLE,
                       ctx->ctx,
                       NULL,
                       p_b_marker,
                       "passing a possible null pointer '%s' to non-nullable pointer parameter", buffer);
            }
            else if (assigment_type == ASSIGMENT_TYPE_RETURN)
            {
                compiler_diagnostic(W_FLOW_NULLABLE_TO_NON_NULLABLE,
                       ctx->ctx,
                       NULL,
                       p_b_marker,
                       "returning a possible null pointer '%s' to non-nullable pointer", buffer);
            }
            else
            {
                compiler_diagnostic(W_FLOW_NULLABLE_TO_NON_NULLABLE,
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
            checked_empty(ctx, p_visitor_a->p_type, p_visitor_a->p_object, p_a_marker);
        }

        if (flow_object_is_zero_or_null(p_visitor_b->p_object))
        {
            if (type_is_array(p_visitor_b->p_type))
            {
                p_visitor_a->p_object->current.state = FLOW_OBJECT_STATE_NOT_NULL;
                return;
            }
            else if (type_is_nullptr_t(p_visitor_b->p_type) || type_is_integer(p_visitor_b->p_type))
            {
                flow_object_set_current_state_to_is_null(p_visitor_a->p_object);

                return;
            }
        }
    }

    if (!a_type_is_view && type_is_pointed_dtor(p_visitor_a->p_type) && type_is_pointer(p_visitor_a->p_type))
    {
        checked_empty(ctx, p_visitor_a->p_type, p_visitor_a->p_object, p_a_marker);

        if (flow_object_is_zero_or_null(p_visitor_b->p_object))
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
            if (flow_object_can_be_moved(p_visitor_b->p_object))
            {
                compiler_diagnostic(W_FLOW_MOVED,
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
                if (flow_object_is_expansible(p_visitor_b->p_object) &&
                    type_is_owner(&t))
                {
                    //if the anwser is yes then we need a warning
                    compiler_diagnostic(W_FLOW_MISSING_DTOR,
                                                ctx->ctx,
                                                NULL,
                                                p_a_marker,
                                                "pointed object may be not empty");
                }
            }
            else
            {
                checked_empty(ctx, &t, p_visitor_b->p_object->current.pointed, p_b_marker);
                object_set_deleted(&t, p_visitor_b->p_object->current.pointed);
            }

            type_destroy(&t);

            if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
            {
                flow_object_set_is_unitialized(p_visitor_b->p_object);
                p_visitor_b->p_object->current.state = FLOW_OBJECT_STATE_UNINITIALIZED;
            }
            else
            {
                flow_object_set_is_moved(p_visitor_b->p_object);
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

        checked_read_object(ctx,
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

            if (flow_object_can_be_moved(p_visitor_b->p_object))
            {
                //TODO we need 2 positions, source, dest
                compiler_diagnostic(W_FLOW_MOVED,
                   ctx->ctx,
                   error_position, NULL,
                   "source object has already been moved");
            }


            if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
            {
                p_visitor_b->p_object->current.state = FLOW_OBJECT_STATE_UNINITIALIZED;
                if (p_visitor_b->p_object->current.pointed)
                {
                    struct flow_object* pointed = p_visitor_b->p_object->current.pointed;

                    struct type t2 = type_remove_pointer(p_visitor_b->p_type);
                    object_set_deleted(&t2, pointed);
                    type_destroy(&t2);
                }
            }
            else
            {

                //TODO
                p_visitor_a->p_object->current.state = p_visitor_a->p_object->current.state & ~FLOW_OBJECT_STATE_MOVED;
                if (p_visitor_b->p_object->current.state & FLOW_OBJECT_STATE_NOT_NULL)
                {
                    //null not-null -> null moved
                    p_visitor_b->p_object->current.state &= ~FLOW_OBJECT_STATE_NOT_NULL;
                    p_visitor_b->p_object->current.state |= FLOW_OBJECT_STATE_MOVED;
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
                        struct flow_object* pointed = p_visitor_b->p_object->current.pointed;

                        struct type t2 = type_remove_pointer(p_visitor_b->p_type);
                        flow_object_set_uninitialized(&t2, pointed);
                        type_destroy(&t2);
                    }

                }
                else
                    flow_object_set_moved(p_visitor_b->p_type, p_visitor_b->p_object);
            }
            else
            {
                if (p_visitor_b->p_type->address_of)
                {
                    //must be address of.
                    if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
                    {
                        p_visitor_b->p_object->current.state = FLOW_OBJECT_STATE_UNINITIALIZED;

                        if (p_visitor_b->p_object->current.pointed)
                        {
                            struct flow_object* pointed = p_visitor_b->p_object->current.pointed;

                            struct type t2 = type_remove_pointer(p_visitor_b->p_type);
                            flow_object_set_uninitialized(&t2, pointed);
                            type_destroy(&t2);
                        }
                    }
                    else
                        flow_object_set_moved(p_visitor_b->p_type, p_visitor_b->p_object);
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
                        //   flow_object_set_unknown(&t3, t3_is_nullable, pointed, nullable_enabled);
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
                                _Opt struct object_visitor visitor_a = { 0 };
                                visitor_a.p_type = &p_a_member_declarator->declarator->type;
                                visitor_a.p_object = p_visitor_a->p_object->members.data[p_visitor_a->member_index];


                                _Opt struct object_visitor visitor_b = { 0 };
                                visitor_b.p_type = &p_b_member_declarator->declarator->type;
                                visitor_b.p_object = p_visitor_b->p_object->members.data[p_visitor_b->member_index];


                                flow_assignment_core(ctx,
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

                        flow_assignment_core(
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
            flow_object_set_uninitialized(p_visitor_b->p_type, p_visitor_b->p_object);
        else
            flow_object_set_moved(p_visitor_b->p_type, p_visitor_b->p_object);
    }
}


struct flow_object* _Opt  expression_get_flow_object(struct flow_visit_ctx* ctx, struct expression* p_expression, bool nullable_enabled)
{
    try
    {
        if (p_expression->expression_type == PRIMARY_EXPRESSION_DECLARATOR)
        {
            assert(p_expression->declarator);
            if (p_expression->declarator->p_alias_of_expression)
            {
                /*We need to original object*/
                return expression_get_flow_object(ctx, p_expression->declarator->p_alias_of_expression, nullable_enabled);
            }
            else
            {
                assert(p_expression->declarator != NULL);

                if (p_expression->declarator->declaration_specifiers &&
                    !(p_expression->declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_AUTOMATIC_STORAGE))
                {
                    assert(p_expression->declarator->p_flow_object != NULL);

                    //External flow_objects are added to the arena on-demand
                    if (flow_objects_find(&ctx->arena, p_expression->declarator->p_flow_object) == NULL)
                    {
                        p_expression->declarator->p_flow_object = make_flow_object(ctx, &p_expression->declarator->type, p_expression->declarator, NULL);
                        if (p_expression->declarator->p_flow_object == NULL)
                            throw;

                        flow_object_set_unknown(&p_expression->declarator->type,
                                                type_is_opt(&p_expression->declarator->type, ctx->ctx->options.null_checks_enabled),
                                                p_expression->declarator->p_flow_object,
                                                ctx->ctx->options.null_checks_enabled);
                    }
                }
                return p_expression->declarator->p_flow_object;
            }
        }

        else if (p_expression->expression_type == UNARY_EXPRESSION_ADDRESSOF)
        {
            assert(p_expression->right != NULL);

            struct flow_object* _Opt p_object = make_flow_object(ctx, &p_expression->type, NULL, p_expression);
            if (p_object == NULL) throw;

            struct flow_object* _Opt p_object_pointed =
                expression_get_flow_object(ctx, p_expression->right, nullable_enabled);

            if (p_object_pointed)
                object_set_pointer(p_object, p_object_pointed);

            p_object->current.state = FLOW_OBJECT_STATE_NOT_NULL;
            p_object->is_temporary = true;
            return p_object;
        }
        else if (p_expression->expression_type == PRIMARY_EXPRESSION_PARENTESIS)
        {
            assert(p_expression->right != NULL);
            return expression_get_flow_object(ctx, p_expression->right, nullable_enabled);
        }
        else if (p_expression->expression_type == CAST_EXPRESSION)
        {
            assert(p_expression->left != NULL);
            struct flow_object* _Opt p = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
            if (p)
            {
                if (type_is_pointer(&p_expression->type_name->type))
                {
                    //casting from 0 to pointer we need to change the zero to null
                    //#define NULL ((void*)0)
                    if (p->current.state & FLOW_OBJECT_STATE_ZERO)
                    {
                        p->current.state &= ~FLOW_OBJECT_STATE_ZERO;
                        p->current.state |= FLOW_OBJECT_STATE_NULL;
                    }
                    if (p->current.state & FLOW_OBJECT_STATE_NOT_ZERO)
                    {
                        p->current.state &= ~FLOW_OBJECT_STATE_NOT_ZERO;
                        p->current.state |= FLOW_OBJECT_STATE_NOT_NULL;
                    }
                }
            }
            return p;
        }
        else if (p_expression->expression_type == POSTFIX_DOT)
        {
            assert(p_expression->left != NULL);

            struct flow_object* _Opt p_obj = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
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
        else if (p_expression->expression_type == POSTFIX_ARRAY)
        {
            assert(p_expression->left != NULL);

            //All arrays items point to the same object.
            struct flow_object* _Opt p_obj = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
            if (p_obj)
            {

                if (p_obj->current.pointed == NULL)
                {
                    flow_object_expand_pointer(ctx, &p_expression->left->type, p_obj);
                }

                if (p_obj->current.pointed)
                {
                    struct flow_object* pointed = p_obj->current.pointed;
                    return pointed;
                }
#if 0
                if (p_obj->current.ref.size == 1)
                {
                    struct flow_object* pointed = p_obj->current.ref.data[0];
                    return pointed;
                }
                else
                {
                    struct flow_object* p_object = make_flow_object(ctx, &p_expression->type, NULL, p_expression);
                    object_set_nothing(&p_expression->type, p_object);
                    for (int i = 0; i < p_obj->current.ref.size; i++)
                    {
                        struct flow_object* pointed = p_obj->current.ref.data[i];
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
        else if (p_expression->expression_type == POSTFIX_ARROW)
        {
            assert(p_expression->left != NULL);

            struct flow_object* _Opt p_obj = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
            if (p_obj)
            {
                if (p_obj->current.pointed == 0)
                {
                    flow_object_expand_pointer(ctx, &p_expression->left->type, p_obj);
                }

                struct flow_object* _Opt pointed = p_obj->current.pointed;

                if (pointed == NULL ||
                    p_expression->member_index >= pointed->members.size)
                {
                    //ops!
                    return NULL;
                }

                struct flow_object* _Opt p_obj2 = pointed->members.data[p_expression->member_index];

                p_obj2->p_declarator_origin = NULL;
                p_obj2->p_expression_origin = p_expression;
                return p_obj2;
            }
            return NULL;
        }
        else if (p_expression->expression_type == UNARY_EXPRESSION_CONTENT)
        {
            assert(p_expression->right != NULL);

            struct flow_object* _Opt p_obj = expression_get_flow_object(ctx, p_expression->right, nullable_enabled);
            if (p_obj)
            {
                if (p_obj->current.pointed == NULL)
                {
                    flow_object_expand_pointer(ctx, &p_expression->right->type, p_obj);
                }

                if (p_obj->current.pointed != NULL)
                {
                    return p_obj->current.pointed;
                }
            }
            return p_obj;
        }
        else if (p_expression->expression_type == POSTFIX_FUNCTION_CALL)
        {
            struct flow_object* _Opt p_object = make_flow_object(ctx, &p_expression->type, NULL, p_expression);
            if (p_object == NULL) throw;

            const bool is_nullable = type_is_opt(&p_expression->type, nullable_enabled);
            flow_object_set_unknown(&p_expression->type, is_nullable, p_object, nullable_enabled);
            p_object->is_temporary = true;

            if (type_is_pointer(&p_expression->type) && flow_object_is_expansible(p_object))
            {
                flow_object_expand_pointer(ctx, &p_expression->type, p_object);
            }


            return p_object;
        }
        else if (p_expression->expression_type == POSTFIX_EXPRESSION_COMPOUND_LITERAL)
        {
            assert(p_expression->type_name != NULL);
            return p_expression->type_name->abstract_declarator->p_flow_object;
        }
        else if (p_expression->expression_type == PRIMARY_EXPRESSION_STRING_LITERAL)
        {
            struct flow_object* _Opt p_object = make_flow_object(ctx, &p_expression->type, NULL, p_expression);
            if (p_object == NULL) throw;

            p_object->current.state = FLOW_OBJECT_STATE_NOT_NULL;
            return p_object;
        }
        else if (p_expression->expression_type == PRIMARY_EXPRESSION_PREDEFINED_CONSTANT)
        {
            struct flow_object* _Opt p_object = make_flow_object(ctx, &p_expression->type, NULL, p_expression);
            if (p_object == NULL) throw;

            if (p_expression->type.type_specifier_flags == TYPE_SPECIFIER_NULLPTR_T)
            {
                p_object->current.state = FLOW_OBJECT_STATE_NULL;
            }
            else
            {
                if (object_has_constant_value(&p_expression->object))
                {
                    bool not_zero = object_to_bool(&p_expression->object);
                    p_object->current.state = not_zero ? FLOW_OBJECT_STATE_NOT_ZERO : FLOW_OBJECT_STATE_ZERO;
                }
            }

            return p_object;
        }
        else if (p_expression->expression_type == ASSIGNMENT_EXPRESSION_ASSIGN)
        {
            assert(p_expression->left != NULL);

            struct flow_object* _Opt p_obj = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);

            //
            //
            return p_obj;
        }
        else if (p_expression->expression_type == CONDITIONAL_EXPRESSION)
        {
            assert(p_expression->left != NULL);
            assert(p_expression->right != NULL);

            struct flow_object* _Opt p_object = make_flow_object(ctx, &p_expression->type, NULL, p_expression);
            if (p_object == NULL) throw;

            struct flow_object* _Opt p_obj1 = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);

            struct flow_object* _Opt p_obj2 = expression_get_flow_object(ctx, p_expression->right, nullable_enabled);

            if (p_obj1 && p_obj2)
            {
                flow_object_merge_state(p_object, p_obj1, p_obj2);
            }

            return p_object;
        }
        else if (p_expression->expression_type == EQUALITY_EXPRESSION_EQUAL ||
                 p_expression->expression_type == EQUALITY_EXPRESSION_NOT_EQUAL)
        {

            struct flow_object* _Opt p_object = make_flow_object(ctx, &p_expression->type, NULL, p_expression);
            if (p_object == NULL) throw;

            if (object_has_constant_value(&p_expression->object))
            {
                bool not_zero = object_to_bool(&p_expression->object);
                p_object->current.state = not_zero ? FLOW_OBJECT_STATE_NOT_ZERO : FLOW_OBJECT_STATE_ZERO;
            }
            else
            {
                p_object->current.state = FLOW_OBJECT_STATE_NOT_ZERO | FLOW_OBJECT_STATE_ZERO;
            }
            return p_object;
        }
        else if (p_expression->expression_type == ADDITIVE_EXPRESSION_PLUS)
        {
            struct flow_object* _Opt p_object = make_flow_object(ctx, &p_expression->type, NULL, p_expression);
            if (p_object == NULL) throw;

            if (type_is_pointer(&p_expression->type))
            {
                //p + 1
                //never null
                p_object->current.state = FLOW_OBJECT_STATE_NOT_NULL;
            }
            else
            {
                if (object_has_constant_value(&p_expression->object))
                {
                    bool not_zero = object_to_bool(&p_expression->object);
                    p_object->current.state = not_zero ? FLOW_OBJECT_STATE_NOT_NULL : FLOW_OBJECT_STATE_NULL;
                }
                else
                {
                    p_object->current.state = FLOW_OBJECT_STATE_NOT_NULL | FLOW_OBJECT_STATE_NULL;
                }
            }
            return p_object;
        }
        else if (p_expression->expression_type == UNARY_EXPRESSION_NEG ||
                 p_expression->expression_type == UNARY_EXPRESSION_PLUS)
        {
            assert(p_expression->right != NULL);

            struct flow_object* _Opt p_obj_right = expression_get_flow_object(ctx, p_expression->right, nullable_enabled);
            struct flow_object* _Opt p_object = make_flow_object(ctx, &p_expression->type, NULL, p_expression);
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
            struct flow_object* _Opt p_object = make_flow_object(ctx, &p_expression->type, NULL, p_expression);
            if (p_object == NULL) throw;

            if (type_is_pointer(&p_expression->type))
            {
                if (object_has_constant_value(&p_expression->object))
                {
                    bool not_zero = object_to_bool(&p_expression->object);
                    p_object->current.state = not_zero ? FLOW_OBJECT_STATE_NOT_NULL : FLOW_OBJECT_STATE_NULL;
                }
                else
                {
                    p_object->current.state = FLOW_OBJECT_STATE_NOT_NULL | FLOW_OBJECT_STATE_NULL;
                }
            }
            else
            {
                if (object_has_constant_value(&p_expression->object))
                {
                    bool not_zero = object_to_bool(&p_expression->object);
                    p_object->current.state = not_zero ? FLOW_OBJECT_STATE_NOT_ZERO : FLOW_OBJECT_STATE_ZERO;
                }
                else
                {
                    p_object->current.state = FLOW_OBJECT_STATE_NOT_ZERO | FLOW_OBJECT_STATE_ZERO;
                }
            }


            return p_object;
        }

    }
    catch
    {

    }
    //printf("null object");
    //assert(false);
    return NULL;
}

void flow_check_assignment(
    struct flow_visit_ctx* ctx,
    const struct token* error_position,
    const struct marker* p_a_marker,
    const struct marker* p_b_marker,
    enum assigment_type assigment_type,
    bool check_uninitialized_b,
    bool a_type_is_view,
    bool a_type_is_nullable,
    struct type* p_a_type, struct flow_object* p_a_object,
    struct type* p_b_type, struct flow_object* p_b_object,
    bool* _Opt set_argument_to_unkown)
{
    if (type_is_pointer(p_b_type) && flow_object_is_expansible(p_b_object))
    {
        //number of warning goes up!
        //flow_object_expand_pointer(ctx, p_b_type, p_b_object);
    }

    struct object_visitor visitor_a = {
    .p_object = p_a_object,
    .p_type = p_a_type
    };

    struct object_visitor visitor_b = {
     .p_object = p_b_object,
     .p_type = p_b_type
    };


    flow_assignment_core(
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


void print_object_state_to_str(enum flow_state e, char str[], int sz)
{
    bool first = true;
    struct osstream ss = { 0 };

    if (e == FLOW_OBJECT_STATE_NOT_APPLICABLE)
        ss_fprintf(&ss, "--");

    if (e & FLOW_OBJECT_STATE_UNINITIALIZED)
    {
        if (first)
            first = false;
        else
            ss_fprintf(&ss, ",");
        ss_fprintf(&ss, "unset");
    }

    if (e & FLOW_OBJECT_STATE_NOT_NULL)
    {
        if (first)
            first = false;
        else
            ss_fprintf(&ss, ",");
        ss_fprintf(&ss, "not-null");
    }

    if (e & FLOW_OBJECT_STATE_NULL)
    {
        if (first)
            first = false;
        else
            ss_fprintf(&ss, ",");
        ss_fprintf(&ss, "null");
    }

    if (e & FLOW_OBJECT_STATE_ZERO)
    {
        if (first)
            first = false;
        else
            ss_fprintf(&ss, ",");
        ss_fprintf(&ss, "zero");
    }

    if (e & FLOW_OBJECT_STATE_NOT_ZERO)
    {
        if (first)
            first = false;
        else
            ss_fprintf(&ss, ",");
        ss_fprintf(&ss, "not-zero");
    }


    if (e & FLOW_OBJECT_STATE_LIFE_TIME_ENDED)
    {
        if (first)
            first = false;
        else
            ss_fprintf(&ss, ",");
        ss_fprintf(&ss, "lifetime-ended");
    }

    if (e & FLOW_OBJECT_STATE_MOVED)
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

void flow_object_state_print(struct flow_object_state* p_state)
{
    struct osstream ss = { 0 };

    char temp[200] = { 0 };
    print_object_state_to_str(p_state->state, temp, sizeof temp);
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
    printf("%-25s│", ss.c_str);
    ss_close(&ss);
}

void print_object_line(struct flow_object* p_object, int extra_cols)
{
    struct osstream ss = { 0 };

    if (p_object->parent)
    {
        ss_fprintf(&ss, "↑%d", p_object->parent->id);

        //if (p_object->current.alternatives.size > 0)
         //ss_fprintf(&ss, " &");

        printf("│%-2d│", p_object->id);
        printf("%-20s│", ss.c_str); //here we need compesate the unicode byte len of ↑
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
        printf("│%-2d│", p_object->id);
        printf("%-18s│", ss.c_str);
    }

    ss_close(&ss);


    int cols = 0;
    struct flow_object_state* _Opt p_state = &p_object->current;
    while (p_state)
    {
        cols++;
        flow_object_state_print(p_state);
        p_state = p_state->next;
    }

    for (int i = 0; i <= extra_cols - cols; i++)
    {
        printf("%-25s│", " ");
    }
    printf("\n");

}

void flow_visit_declaration(struct flow_visit_ctx* ctx, struct declaration* p_declaration);


static void flow_visit_secondary_block(struct flow_visit_ctx* ctx, struct secondary_block* p_secondary_block);
static void flow_visit_struct_or_union_specifier(struct flow_visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier);
struct true_false_set;
static void flow_visit_expression(struct flow_visit_ctx* ctx, struct expression* p_expression, struct true_false_set* a);
static void flow_visit_statement(struct flow_visit_ctx* ctx, struct statement* p_statement);
static void flow_visit_enum_specifier(struct flow_visit_ctx* ctx, struct enum_specifier* p_enum_specifier);
static void flow_visit_type_specifier(struct flow_visit_ctx* ctx, struct type_specifier* p_type_specifier);
static void flow_visit_bracket_initializer_list(struct flow_visit_ctx* ctx, struct braced_initializer* p_bracket_initializer_list);
static void flow_visit_expression_statement(struct flow_visit_ctx* ctx, struct expression_statement* p_expression_statement);

enum boolean_flag
{
    BOOLEAN_FLAG_NONE = 0,
    BOOLEAN_FLAG_TRUE = 1 << 0,
    BOOLEAN_FLAG_FALSE = 1 << 1,
};

struct true_false_set_item
{
    struct expression* p_expression;
    enum boolean_flag true_branch_state;
    enum boolean_flag false_branch_state;
};

struct true_false_set
{
    struct true_false_set_item* _Owner _Opt data;
    int size;
    int capacity;
};

static void true_false_set_clear(struct true_false_set* p)
{
    free(p->data);
    p->data = NULL;
    p->size = 0;
    p->capacity = 0;
}

enum merge_options
{
    MERGE_OPTIONS_A_TRUE = 1 << 0,
    MERGE_OPTIONS_A_FALSE = 1 << 1,
    MERGE_OPTIONS_B_TRUE = 1 << 2,
    MERGE_OPTIONS_B_FALSE = 1 << 3
};

void true_false_set_destroy(_Dtor struct true_false_set* p)
{
    free(p->data);
}

static int true_false_set_reserve(struct true_false_set* p, int n)
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
        static_set(p->data, "moved");
        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

static int true_false_set_push_back(struct true_false_set* p, const struct true_false_set_item* book)
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

        int error = true_false_set_reserve(p, new_capacity);
        if (error != 0)
        {
            return error;
        }
    }

    p->data[p->size] = *book; /*COPIED*/


    p->size++;

    return 0;
}

static void true_false_set_invert(struct true_false_set* true_false_set)
{
    for (int i = 0; i < true_false_set->size; i++)
    {
        enum boolean_flag temp = true_false_set->data[i].true_branch_state;
        true_false_set->data[i].true_branch_state = true_false_set->data[i].false_branch_state;
        true_false_set->data[i].false_branch_state = temp;
    }
}

static void true_false_set_swap(struct true_false_set* a, struct true_false_set* b)
{
    struct true_false_set temp = *a;
    *a = *b;
    *b = temp;
}

static int find_item_index_by_expression(const struct true_false_set* a, const struct expression* p_expression)
{
    for (int i = 0; i < a->size; i++)
    {
        if (a->data[i].p_expression == p_expression)
            return i;
    }
    return -1;
}

static void true_false_set_merge(struct true_false_set* result,
    struct true_false_set* a,
    struct true_false_set* b,
    enum merge_options options_true,
    enum merge_options options_false)
{

    for (int i = 0; i < a->size; i++)
    {
        const struct true_false_set_item* p_item_a = &a->data[i];

        _Opt struct true_false_set_item new_item = { 0 };
        new_item.p_expression = p_item_a->p_expression;

        if (options_true & MERGE_OPTIONS_A_TRUE)
            new_item.true_branch_state |= p_item_a->true_branch_state;

        if (options_true & MERGE_OPTIONS_A_FALSE)
            new_item.true_branch_state |= p_item_a->false_branch_state;

        if (options_false & MERGE_OPTIONS_A_TRUE)
            new_item.false_branch_state |= p_item_a->true_branch_state;

        if (options_false & MERGE_OPTIONS_A_FALSE)
            new_item.true_branch_state |= p_item_a->false_branch_state;


        true_false_set_push_back(result, &new_item);
    }

    for (int k = 0; k < b->size; k++)
    {
        const struct true_false_set_item* p_item_b = &b->data[k];

        int index = find_item_index_by_expression(result, p_item_b->p_expression);
        if (index == -1)
        {
            index = result->size;
            _Opt struct true_false_set_item item2 = { 0 };
            item2.p_expression = p_item_b->p_expression;
            true_false_set_push_back(result, &item2);
        }

        struct true_false_set_item* p_item_result = &result->data[index];

        if (options_true & MERGE_OPTIONS_B_TRUE)
            p_item_result->true_branch_state |= p_item_b->true_branch_state;

        if (options_true & MERGE_OPTIONS_B_FALSE)
            p_item_result->true_branch_state |= p_item_b->false_branch_state;

        if (options_false & MERGE_OPTIONS_B_TRUE)
            p_item_result->false_branch_state |= p_item_b->true_branch_state;

        if (options_false & MERGE_OPTIONS_B_FALSE)
            p_item_result->true_branch_state |= p_item_b->false_branch_state;
    }
}

static void true_false_set_set_objects_to_core_branch(struct flow_visit_ctx* ctx,
    struct true_false_set* a,
    bool nullable_enabled,
    bool true_branch)
{
    for (int i = 0; i < a->size; i++)
    {
        assert(a->data[i].p_expression != NULL);

        struct flow_object* _Opt p_object =
            expression_get_flow_object(ctx, a->data[i].p_expression, nullable_enabled);

        if (p_object)
        {
            if (p_object->current.state == FLOW_OBJECT_STATE_NOT_NULL ||
                p_object->current.state == FLOW_OBJECT_STATE_NULL ||
                p_object->current.state == FLOW_OBJECT_STATE_MOVED ||
                p_object->current.state == FLOW_OBJECT_STATE_ZERO ||
                p_object->current.state == FLOW_OBJECT_STATE_NOT_ZERO ||
                p_object->current.state == FLOW_OBJECT_STATE_LIFE_TIME_ENDED)
            {
                continue;
            }

            const enum boolean_flag flag =
                true_branch ?
                a->data[i].true_branch_state :
                a->data[i].false_branch_state;

            if ((flag & BOOLEAN_FLAG_TRUE) && (flag & BOOLEAN_FLAG_FALSE))
            {
            }
            else if (flag & BOOLEAN_FLAG_FALSE)
            {
                p_object->current.state &= ~FLOW_OBJECT_STATE_NOT_NULL;
                p_object->current.state &= ~FLOW_OBJECT_STATE_MOVED;
            }
            else if (flag & BOOLEAN_FLAG_TRUE)
            {
                p_object->current.state &= ~FLOW_OBJECT_STATE_NULL;
                p_object->current.state &= ~FLOW_OBJECT_STATE_ZERO;

            }
        }
    }
}

static void true_false_set_set_objects_to_true_branch(struct flow_visit_ctx* ctx, struct true_false_set* a, bool nullable_enabled)
{
    true_false_set_set_objects_to_core_branch(ctx, a, nullable_enabled, true);
}

static void true_false_set_set_objects_to_false_branch(struct flow_visit_ctx* ctx, struct true_false_set* a, bool nullable_enabled)
{
    true_false_set_set_objects_to_core_branch(ctx, a, nullable_enabled, false);
}

static int arena_add_copy_of_current_state(struct flow_visit_ctx* ctx, const char* name);


static void arena_remove_state(struct flow_visit_ctx* ctx, int state_number);

struct visit_objects {
    struct flow_defer_scope* current_block;
    struct flow_defer_scope* next_child;
};


static void flow_exit_block_visit_defer_item(struct flow_visit_ctx* ctx,
    struct defer_list_item* p_item,
    struct token* position_token)
{
    if (p_item->defer_statement)
    {
        const int error_count = ctx->ctx->p_report->error_count;
        const int warnings_count = ctx->ctx->p_report->warnings_count;
        const int info_count = ctx->ctx->p_report->info_count;

        flow_visit_secondary_block(ctx, p_item->defer_statement->secondary_block);

        if (error_count != ctx->ctx->p_report->error_count ||
            warnings_count != ctx->ctx->p_report->warnings_count ||
            info_count != ctx->ctx->p_report->info_count)
        {
            compiler_diagnostic(W_LOCATION, ctx->ctx, position_token, NULL, "defer end of scope");
        }
    }
    else if (p_item->declarator)
    {
        struct declarator* p_declarator = p_item->declarator;
        const char* name = p_declarator->name_opt ? p_declarator->name_opt->lexeme : "?";

        if (p_declarator->p_flow_object)
        {
            flow_end_of_block_visit(ctx,
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

static void flow_exit_block_visit_defer_list(struct flow_visit_ctx* ctx,
    struct defer_list* p_defer_list,
    struct token* position_token)
{
    struct defer_list_item* _Opt p_item = p_defer_list->head;
    while (p_item)
    {
        flow_exit_block_visit_defer_item(ctx, p_item, position_token);
        p_item = p_item->next;
    }
}

static void flow_defer_item_set_end_of_lifetime(struct flow_visit_ctx* ctx,
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
            flow_object_set_end_of_lifetime(&p_declarator->type, p_declarator->p_flow_object);
        }
        else
        {
            assert(0);
        }
    }
}

static void flow_defer_list_set_end_of_lifetime(struct flow_visit_ctx* ctx,
    struct defer_list* p_defer_list,
    struct token* position_token)
{
    struct defer_list_item* _Opt p_item = p_defer_list->head;
    while (p_item)
    {
        flow_defer_item_set_end_of_lifetime(ctx, p_item, position_token);
        p_item = p_item->next;
    }
}

static void flow_visit_secondary_block(struct flow_visit_ctx* ctx, struct secondary_block* p_secondary_block)
{
    flow_visit_statement(ctx, p_secondary_block->statement);
}

static void flow_visit_defer_statement(struct flow_visit_ctx* ctx, struct defer_statement* p_defer_statement)
{
    /*
      We are not going to visit the secondary block here because
      this is not the place were defer is executed.
    */
}

NODISCARD
static int arena_add_copy_of_current_state(struct flow_visit_ctx* ctx, const char* name)
{
    int state_number = ctx->state_number_generator;
    ctx->state_number_generator++;
    for (int i = 0; i < ctx->arena.size; i++)
    {
        flow_object_add_new_state_as_a_copy_of_current_state(ctx->arena.data[i], name, state_number);
    }
    return state_number;
}

static int arena_add_empty_state(struct flow_visit_ctx* ctx, const char* name)
{
    int state_number = ctx->state_number_generator;
    ctx->state_number_generator++;

    for (int i = 0; i < ctx->arena.size; i++)
    {
        struct flow_object* p_object = ctx->arena.data[i];
        _Opt struct flow_object_state* _Owner _Opt p_flow_object_state = calloc(1, sizeof * p_flow_object_state);
        if (p_flow_object_state)
        {
            p_flow_object_state->dbg_name = name;
            p_flow_object_state->state_number = state_number;
            flow_object_add_state(p_object, p_flow_object_state);
        }

    }
    return state_number;
}

static void flow_object_set_state_from_current(struct flow_object* object, int state_number)
{
    struct flow_object_state* _Opt p_flow_object_state = object->current.next;
    while (p_flow_object_state)
    {
        if (p_flow_object_state->state_number == state_number)
        {
            flow_object_state_copy(p_flow_object_state, &object->current);
            break;
        }
        p_flow_object_state = p_flow_object_state->next;
    }
}

static void arena_set_state_from_current(struct flow_visit_ctx* ctx, int number_state)
{
    for (int i = 0; i < ctx->arena.size; i++)
    {
        struct flow_object* p_object = ctx->arena.data[i];
        flow_object_set_state_from_current(p_object, number_state);
    }
}

static int flow_object_merge_current_with_state(struct flow_visit_ctx* ctx, struct flow_object* object, int state_number)
{
    try
    {
        struct flow_object_state* _Opt it = object->current.next;
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

                    struct flow_object* _Opt p_new_object = arena_new_object(ctx);
                    if (p_new_object == NULL) throw;

                    _Opt struct flow_object_state* _Owner _Opt p_new_state = calloc(1, sizeof * p_new_state);
                    if (p_new_state == NULL) throw;

                    p_new_state->dbg_name = "merged";
                    p_new_state->state_number = state_number;
                    objects_view_push_back(&p_new_state->alternatives, object->current.pointed);
                    objects_view_push_back(&p_new_state->alternatives, it->pointed);
                    flow_object_add_state(p_new_object, p_new_state);

                    int n_childs_1 = object->current.pointed->members.size;
                    int n_childs_2 = it->pointed->members.size;
                    if (n_childs_1 == n_childs_2)
                    {

                        for (int j = 0; j < n_childs_1; j++)
                        {
                            struct flow_object* _Opt p_new_child = arena_new_object(ctx);
                            if (p_new_child == NULL) throw;

                            p_new_child->parent = p_new_object;

                            struct flow_object* child1 = object->current.pointed->members.data[j];
                            struct flow_object* child2 = it->pointed->members.data[j];

                            _Opt struct flow_object_state* _Owner _Opt p_child_new_state = calloc(1, sizeof * p_child_new_state);
                            if (p_child_new_state == NULL) throw;

                            p_child_new_state->dbg_name = "merged";
                            p_child_new_state->state_number = state_number;
                            p_child_new_state->state = child1->current.state | it->state;
                            objects_view_push_back(&p_child_new_state->alternatives, child1);
                            objects_view_push_back(&p_child_new_state->alternatives, child2);
                            flow_object_add_state(p_new_child, p_child_new_state);
                            objects_view_push_back(&p_new_object->members, p_new_child);
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

static void arena_merge_current_state_with_state_number(struct flow_visit_ctx* ctx, int number_state)
{
    for (int i = 0; i < ctx->arena.size; i++)
    {
        flow_object_merge_current_with_state(ctx, ctx->arena.data[i], number_state);
    }
}

static void object_restore_current_state_from(struct flow_visit_ctx* ctx, struct flow_object* object, int state_number)
{
    struct flow_object_state* _Opt it = object->current.next;
    while (it)
    {
        if (it->state_number == state_number)
        {
            flow_object_state_copy(&object->current, it);
            break;
        }
        it = it->next;
    }
}

static void arena_restore_current_state_from(struct flow_visit_ctx* ctx, int number_state)
{
    for (int i = 0; i < ctx->arena.size; i++)
    {
        object_restore_current_state_from(ctx, ctx->arena.data[i], number_state);
    }
}

static void arena_remove_state(struct flow_visit_ctx* ctx, int state_number)
{
    for (int i = 0; i < ctx->arena.size; i++)
    {
        flow_object_remove_state(ctx->arena.data[i], state_number);
    }
}

static void flow_visit_initializer(struct flow_visit_ctx* ctx, struct initializer* p_initializer);
static void flow_visit_declarator(struct flow_visit_ctx* ctx, struct declarator* p_declarator);


static void braced_initializer_flow_core(struct flow_visit_ctx* ctx, struct object* obj, struct flow_object* flow_obj)
{
    /*
       We use the obj to get the expression that initializes it.
    */

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    if (obj->p_init_expression)
    {
        struct flow_object* _Opt p_right_object =
            expression_get_flow_object(ctx, obj->p_init_expression, nullable_enabled);

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

            flow_check_assignment(ctx,
                                obj->p_init_expression->first_token,
                                &a_marker,
                                &b_marker,
                                ASSIGMENT_TYPE_OBJECTS,
                                false,
                                type_is_view(&obj->type),
                                type_is_opt(&obj->type, ctx->ctx->options.null_checks_enabled),
                                &obj->type,
                                flow_obj,
                                &obj->p_init_expression->type,
                                p_right_object,
                                NULL);
        }
    }
    else
    {
        /*default initialization*/
        flow_object_set_zero(&obj->type, flow_obj);
    }

    if (flow_obj->members.size > 0)
    {
        /*flow_object and object have the same number of members*/
        int i = 0;

        struct object* _Opt m = obj->members;
        while (m)
        {
            braced_initializer_flow_core(ctx, m, flow_obj->members.data[i]);
            m = m->next;
            i++;
        }
    }
}

static void braced_initializer_flow(struct flow_visit_ctx* ctx, struct object* obj, struct flow_object* flow_obj)
{
    try
    {
        braced_initializer_flow_core(ctx, obj, flow_obj);

        if (flow_obj->p_declarator_origin == NULL)
        {
            throw;
        }

        /*
           Let´s check if the object has been initialized correctly
        */

        bool is_nullable = type_is_opt(&obj->type, ctx->ctx->options.null_checks_enabled);

        struct marker a_marker = {
                          .p_token_begin = flow_obj->p_declarator_origin->first_token_opt,
                          .p_token_end = flow_obj->p_declarator_origin->last_token_opt,
        };
        checked_read_object(ctx,
        &obj->type,
        is_nullable,
        flow_obj,
        flow_obj->p_declarator_origin->first_token_opt,
        &a_marker,
        false);
    }
    catch
    {
    }
}


static void flow_visit_init_declarator(struct flow_visit_ctx* ctx, struct init_declarator* p_init_declarator)
{
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    try
    {
        flow_visit_declarator(ctx, p_init_declarator->p_declarator);

        if (p_init_declarator->initializer)
        {
            if (p_init_declarator->initializer->assignment_expression)
            {
                struct true_false_set a = { 0 };
                flow_visit_expression(ctx, p_init_declarator->initializer->assignment_expression, &a);
                true_false_set_destroy(&a);
            }
            else
            {
                if (p_init_declarator->initializer->braced_initializer)
                {
                    flow_visit_bracket_initializer_list(ctx,
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
                struct flow_object* _Opt p_right_object =
                    expression_get_flow_object(ctx, p_init_declarator->initializer->assignment_expression, nullable_enabled);

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

                    flow_check_assignment(ctx,
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
                    struct flow_object* _Opt po = make_flow_object(ctx, &t, p_init_declarator->p_declarator, NULL);
                    if (po == NULL)
                    {
                        type_destroy(&t);
                        throw;
                    }

                    object_set_pointer(p_init_declarator->p_declarator->p_flow_object, po);
                    type_destroy(&t);
                    p_init_declarator->p_declarator->p_flow_object->current.state = FLOW_OBJECT_STATE_NOT_NULL | FLOW_OBJECT_STATE_NULL;
                }
                else if (expression_is_calloc(p_init_declarator->initializer->assignment_expression))
                {
                    assert(p_init_declarator->p_declarator->p_flow_object != NULL);

                    struct type t = type_remove_pointer(&p_init_declarator->p_declarator->type);
                    struct flow_object* _Opt pointed_calloc_object = make_flow_object(ctx, &t, p_init_declarator->p_declarator, NULL);
                    if (pointed_calloc_object == NULL)
                    {
                        type_destroy(&t);
                        throw;
                    }

                    flow_object_set_zero(&t, pointed_calloc_object);
                    object_set_pointer(p_init_declarator->p_declarator->p_flow_object, pointed_calloc_object);

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

                    type_destroy(&t);
                    p_init_declarator->p_declarator->p_flow_object->current.state = FLOW_OBJECT_STATE_NOT_NULL | FLOW_OBJECT_STATE_NULL;
                }
            }
            else  if (p_init_declarator->initializer &&
                p_init_declarator->initializer->braced_initializer)
            {
                assert(p_init_declarator->p_declarator->p_flow_object != NULL);
                braced_initializer_flow(ctx,
                    &p_init_declarator->p_declarator->object,
                    p_init_declarator->p_declarator->p_flow_object);
            }
            else
            {
                if (p_init_declarator->p_declarator->declaration_specifiers &&
                    (
                        (!(p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_AUTOMATIC_STORAGE)) ||
                        (p_init_declarator->p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_STATIC)
                        )
                    )
                {
                    assert(p_init_declarator->p_declarator->p_flow_object != NULL);
                    flow_object_set_zero(&p_init_declarator->p_declarator->type, p_init_declarator->p_declarator->p_flow_object);
                }
                else
                {
                    assert(p_init_declarator->p_declarator->p_flow_object != NULL);
                    flow_object_set_uninitialized(&p_init_declarator->p_declarator->type, p_init_declarator->p_declarator->p_flow_object);
                }
            }
        }
    }
    catch
    {
        //some unexpected error
    }
}


static void flow_visit_init_declarator_list(struct flow_visit_ctx* ctx, struct init_declarator_list* p_init_declarator_list);

static void flow_visit_declaration_specifiers(struct flow_visit_ctx* ctx,
    struct declaration_specifiers* p_declaration_specifiers,
    struct type* _Opt p_type);


static void flow_visit_simple_declaration(struct flow_visit_ctx* ctx, struct simple_declaration* p_simple_declaration)
{
    flow_visit_declaration_specifiers(ctx, p_simple_declaration->p_declaration_specifiers, NULL);
    flow_visit_init_declarator_list(ctx, &p_simple_declaration->init_declarator_list);
}

static void flow_check_pointer_used_as_bool(struct flow_visit_ctx* ctx, struct expression* p_expression);

void print_arena(struct flow_visit_ctx* ctx)
{
    int extra_cols = 0;
    for (int i = 0; i < ctx->arena.size; i++)
    {
        struct flow_object* p_object = ctx->arena.data[i];
        struct flow_object_state* _Opt p_state = p_object->current.next;
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
    printf("\n");
    printf("┌──┬──────────────────┬─────────────────────────");
    if (extra_cols > 0)
    {
        for (int i = 0; i < extra_cols; i++)
        {
            if (i < extra_cols - 1)
                printf("┬─────────────────────────");
            else
                printf("┬─────────────────────────");
        }
    }

    printf("┐");

    printf("\n");

    for (int i = 0; i < ctx->arena.size; i++)
    {
        struct flow_object* p = ctx->arena.data[i];
        print_object_line(p, extra_cols);
    }
    printf("└──┴──────────────────┴─────────────────────────");
    if (extra_cols > 0)
    {
        for (int i = 0; i < extra_cols; i++)
        {
            if (i < extra_cols - 1)
                printf("┴─────────────────────────");
            else
                printf("┴─────────────────────────");
        }
    }

    printf("┘");

    printf("\n");
    printf("\n");
}

static void flow_visit_if_statement(struct flow_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    if (p_selection_statement->p_init_statement &&
        p_selection_statement->p_init_statement->p_expression_statement)
        flow_visit_expression_statement(ctx, p_selection_statement->p_init_statement->p_expression_statement);

    if (p_selection_statement->p_init_statement &&
        p_selection_statement->p_init_statement->p_simple_declaration)
        flow_visit_simple_declaration(ctx, p_selection_statement->p_init_statement->p_simple_declaration);


    /*
      if(int *p = f());
      The hidden expression is p
    */
    _Opt _View struct expression hidden_expression = { 0 };

    struct true_false_set true_false_set = { 0 };

    if (p_selection_statement->condition &&
        p_selection_statement->condition->expression)
    {
        flow_check_pointer_used_as_bool(ctx, p_selection_statement->condition->expression);
        flow_visit_expression(ctx, p_selection_statement->condition->expression, &true_false_set);
    }

    if (p_selection_statement->condition &&
        p_selection_statement->condition->p_init_declarator)
    {
        flow_visit_init_declarator(ctx, p_selection_statement->condition->p_init_declarator);
    }

    if (p_selection_statement->condition &&
        p_selection_statement->condition->expression == NULL &&
        p_selection_statement->condition->p_init_declarator != NULL)
    {
        hidden_expression.expression_type = PRIMARY_EXPRESSION_DECLARATOR;
        hidden_expression.declarator = p_selection_statement->condition->p_init_declarator->p_declarator;
        assert(p_selection_statement->condition->p_init_declarator->p_declarator->first_token_opt != NULL);
        hidden_expression.first_token = p_selection_statement->condition->p_init_declarator->p_declarator->first_token_opt;
        hidden_expression.last_token = hidden_expression.first_token;
        flow_visit_expression(ctx, &hidden_expression, &true_false_set);
    }

    assert(p_selection_statement->first_token->type == TK_KEYWORD_IF);

    /*
       This index is from the end of top of stack going to base of statck
    */
    const int before_if_state_number = arena_add_copy_of_current_state(ctx, "before-if");

    true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);
    flow_visit_secondary_block(ctx, p_selection_statement->secondary_block);

    const bool true_branch_ends_with_jump =
        secondary_block_ends_with_jump(p_selection_statement->secondary_block);


    /*let's make a copy of the state we left true branch*/
    //const int true_branch = 1;
    const int left_true_branch_state_number = arena_add_copy_of_current_state(ctx, "left-true-branch");

    arena_restore_current_state_from(ctx, before_if_state_number);


    if (p_selection_statement->else_secondary_block_opt)
    {
        true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
        flow_visit_secondary_block(ctx, p_selection_statement->else_secondary_block_opt);
    }
    else
    {
        //we emulate a empty else {}
        true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
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

            arena_restore_current_state_from(ctx, left_true_branch_state_number);
        }
        else
        {
            /*
               if (){
               }
               else {

               }
            */

            arena_merge_current_state_with_state_number(ctx, left_true_branch_state_number);

            arena_restore_current_state_from(ctx, left_true_branch_state_number);
        }
    }

    flow_exit_block_visit_defer_list(ctx, &p_selection_statement->defer_list, p_selection_statement->last_token);
    flow_defer_list_set_end_of_lifetime(ctx, &p_selection_statement->defer_list, p_selection_statement->last_token);

    arena_remove_state(ctx, before_if_state_number);
    arena_remove_state(ctx, left_true_branch_state_number);
    true_false_set_destroy(&true_false_set);

}

static void flow_visit_block_item(struct flow_visit_ctx* ctx, struct block_item* p_block_item);


static void flow_visit_try_statement(struct flow_visit_ctx* ctx, struct try_statement* p_try_statement)
{
    try
    {
        const int throw_join_state_old = ctx->throw_join_state;
        struct secondary_block* _Opt catch_secondary_block_old = ctx->catch_secondary_block_opt;

        ctx->catch_secondary_block_opt = p_try_statement->catch_secondary_block_opt;

        ctx->throw_join_state = arena_add_empty_state(ctx, "try");

        const int original_state_number = arena_add_copy_of_current_state(ctx, "original");


        flow_visit_secondary_block(ctx, p_try_statement->secondary_block);
        arena_set_state_from_current(ctx, original_state_number); //state of end of secondary block

        if (p_try_statement->catch_secondary_block_opt)
        {
            //current all possible states of throw
            arena_restore_current_state_from(ctx, ctx->throw_join_state);
            flow_visit_secondary_block(ctx, p_try_statement->catch_secondary_block_opt);
            //current has the state at the end of catch block
        }

        bool try_reached_the_end = !secondary_block_ends_with_jump(p_try_statement->secondary_block);
        bool catch_reached_the_end = !secondary_block_ends_with_jump(p_try_statement->catch_secondary_block_opt);

        if (try_reached_the_end && catch_reached_the_end)
        {
            //we could merge directly at current
            arena_merge_current_state_with_state_number(ctx, original_state_number);
            arena_restore_current_state_from(ctx, original_state_number);
        }
        else if (try_reached_the_end)
        {
            arena_restore_current_state_from(ctx, original_state_number);
        }
        else if (catch_reached_the_end)
        {
            //ctx_object_restore_current_state_from(ctx, orignial);       
        }


        arena_remove_state(ctx, original_state_number);
        arena_remove_state(ctx, ctx->throw_join_state);

        ctx->throw_join_state = throw_join_state_old; //restore
        ctx->catch_secondary_block_opt = catch_secondary_block_old; //restore
    }
    catch
    {
    }
}

static void flow_visit_switch_statement(struct flow_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    const int old_initial_state = ctx->initial_state;
    const int old_break_join_state = ctx->break_join_state;

    ctx->initial_state = arena_add_copy_of_current_state(ctx, "original");
    ctx->break_join_state = arena_add_empty_state(ctx, "break join");

    flow_visit_secondary_block(ctx, p_selection_statement->secondary_block);

    bool reached_the_end = !secondary_block_ends_with_jump(p_selection_statement->secondary_block);

    if (!reached_the_end)
    {
        arena_restore_current_state_from(ctx, ctx->break_join_state);
    }

    flow_exit_block_visit_defer_list(ctx, &p_selection_statement->defer_list, p_selection_statement->secondary_block->last_token);
    flow_defer_list_set_end_of_lifetime(ctx, &p_selection_statement->defer_list, p_selection_statement->secondary_block->last_token);

    arena_remove_state(ctx, ctx->initial_state);
    arena_remove_state(ctx, ctx->break_join_state);

    //restore
    ctx->initial_state = old_initial_state;
    ctx->break_join_state = old_break_join_state;
}

static void flow_visit_selection_statement(struct flow_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
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

static void flow_visit_bracket_initializer_list(struct flow_visit_ctx* ctx, struct braced_initializer* p_bracket_initializer_list);

static void flow_visit_initializer_list_item(struct flow_visit_ctx* ctx, struct initializer_list_item* p_initializer)
{
    assert(p_initializer->initializer != NULL);
    flow_visit_initializer(ctx, p_initializer->initializer);
}

static void flow_visit_initializer(struct flow_visit_ctx* ctx, struct initializer* p_initializer)
{
    if (p_initializer->assignment_expression)
    {
        struct true_false_set a = { 0 };
        flow_visit_expression(ctx, p_initializer->assignment_expression, &a);
        true_false_set_destroy(&a);
    }
    else if (p_initializer->braced_initializer)
    {
        flow_visit_bracket_initializer_list(ctx, p_initializer->braced_initializer);
    }
}

static void flow_visit_initializer_list(struct flow_visit_ctx* ctx, struct initializer_list* p_initializer_list)
{
    struct initializer_list_item* _Opt p_initializer = p_initializer_list->head;
    while (p_initializer)
    {
        flow_visit_initializer_list_item(ctx, p_initializer);
        p_initializer = p_initializer->next;
    }
}

static void flow_visit_generic_selection(struct flow_visit_ctx* ctx, struct generic_selection* p_generic_selection)
{
    if (p_generic_selection->expression)
    {
        struct true_false_set a = { 0 };
        flow_visit_expression(ctx, p_generic_selection->expression, &a);
        true_false_set_destroy(&a);
    }
}


static void flow_compare_function_arguments(struct flow_visit_ctx* ctx,
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

            struct true_false_set a = { 0 };

            struct diagnostic temp =
                ctx->ctx->options.diagnostic_stack.stack[ctx->ctx->options.diagnostic_stack.top_index];

            //we don´t report W_FLOW_UNINITIALIZED here because it is checked next.. (TODO parts of expression)
            diagnostic_remove(&ctx->ctx->options.diagnostic_stack.stack[ctx->ctx->options.diagnostic_stack.top_index], W_FLOW_UNINITIALIZED);

            flow_visit_expression(ctx, p_current_argument->expression, &a);

            ctx->ctx->options.diagnostic_stack.stack[ctx->ctx->options.diagnostic_stack.top_index] = temp;

            true_false_set_destroy(&a);

            struct flow_object* _Opt p_argument_object =
                expression_get_flow_object(ctx, p_current_argument->expression, nullable_enabled);

            if (p_argument_object)
            {
                struct flow_object* _Opt parameter_object = make_flow_object(ctx, &p_current_parameter_type->type, NULL, p_current_argument->expression);
                if (parameter_object == NULL) throw;

                flow_object_set_uninitialized(&p_current_parameter_type->type, parameter_object);

                struct marker a_marker = {
                    .p_token_begin = p_current_argument->expression->first_token,
                    .p_token_end = p_current_argument->expression->last_token
                };

                //TODO pass real function to get location
                struct marker b_marker = {
                    .p_token_begin = p_current_argument->expression->first_token,
                    .p_token_end = p_current_argument->expression->last_token
                };

                flow_check_assignment(ctx,
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

            struct flow_object* _Opt p_argument_object =
                expression_get_flow_object(ctx, p_current_argument->expression, nullable_enabled);

            if (p_argument_object)
            {
                struct marker marker = {
                    .p_token_begin = p_current_argument->expression->first_token,
                    .p_token_end = p_current_argument->expression->last_token
                };
                checked_read_object(ctx,
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

                struct flow_object* _Opt p_argument_object =
                    expression_get_flow_object(ctx, p_current_argument->expression, nullable_enabled);


                if (p_argument_object)
                {
                    const bool argument_type_is_nullable =
                        type_is_opt(&pointed_type, ctx->ctx->options.null_checks_enabled);

                    if (p_argument_object->current.pointed)
                    {
                        flow_object_set_unknown(&pointed_type,
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

            struct flow_object* _Opt p_argument_object =
                expression_get_flow_object(ctx, p_current_argument->expression, nullable_enabled);

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

static void check_uninitialized(struct flow_visit_ctx* ctx, struct expression* p_expression)
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

    struct flow_object* _Opt p_object = expression_get_flow_object(ctx, p_expression, nullable_enabled);

    if (!ctx->expression_is_not_evaluated)
    {
        if (p_object && p_object->current.state == FLOW_OBJECT_STATE_UNINITIALIZED)
        {
            if (p_expression->expression_type == PRIMARY_EXPRESSION_DECLARATOR &&
                p_expression->declarator &&
                p_expression->declarator->name_opt)
            {
                compiler_diagnostic(W_FLOW_UNINITIALIZED,
                    ctx->ctx,
                    p_expression->first_token, NULL, "using a uninitialized object '%s'", p_expression->declarator->name_opt->lexeme);
            }
            else
            {
                compiler_diagnostic(W_FLOW_UNINITIALIZED,
                    ctx->ctx,
                    p_expression->first_token, NULL, "using a uninitialized object");
            }
        }
        else if (p_object && p_object->current.state & FLOW_OBJECT_STATE_UNINITIALIZED)
        {
            if (ctx->ctx->options.ownership_enabled)
            {
                if (p_expression->declarator && p_expression->declarator->name_opt)
                {
                    compiler_diagnostic(W_FLOW_UNINITIALIZED,
                        ctx->ctx,
                        p_expression->first_token,
                        NULL,
                        "object '%s' can be uninitialized ",
                        p_expression->declarator->name_opt->lexeme);
                }
                else
                {
                    compiler_diagnostic(W_FLOW_UNINITIALIZED,
                        ctx->ctx,
                        p_expression->first_token, NULL, "maybe using a uninitialized object");
                }
            }
        }
    }
}

void flow_object_push_states_from(const struct flow_object* p_object_from, struct flow_object* p_object_to)
{
    struct flow_object_state* _Opt it_from = p_object_from->current.next;
    while (it_from)
    {
#if 0
        flow_object_add_state(
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
        flow_object_push_states_from(p_object_from, p_object_to->members.data[i]);
    }
}


static void flow_check_pointer_used_as_bool(struct flow_visit_ctx* ctx, struct expression* p_expression)
{
    //when pointer as used as bool, like if (p) or p && p->i we check
    //if the pointer has a flow condition always true or false.

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    if (type_is_pointer(&p_expression->type))
    {

        struct flow_object* _Opt p_object = expression_get_flow_object(ctx, p_expression, nullable_enabled);
        if (p_object)
        {
            struct marker marker = {
                 .p_token_begin = p_expression->first_token,
                 .p_token_end = p_expression->last_token
            };

            if (!ctx->inside_loop && flow_object_is_null(p_object))
            {
                compiler_diagnostic(W_FLOW_NON_NULL,
                        ctx->ctx,
                        NULL,
                        &marker,
                        "pointer is always null");

            }
            else if (!ctx->inside_loop && flow_object_is_not_null(p_object))
            {
                compiler_diagnostic(W_FLOW_NON_NULL,
                        ctx->ctx,
                        NULL,
                        &marker,
                        "pointer is always not-null");
            }
        }
    }
}

static void arena_broadcast_change(struct flow_visit_ctx* ctx, struct flow_object* p)
{
    for (int i = 0; i < ctx->arena.size; i++)
    {
        struct flow_object* p_obj = ctx->arena.data[i];

        for (int j = 0; j < p_obj->current.alternatives.size; j++)
        {
            if (p_obj->current.alternatives.data[j] == p)
            {
                flow_object_update_current(p_obj);
                break;
            }
        }
    }
}

static struct argument_expression* _Opt param_list_find_argument_by_name(struct param_list* p_param_list,
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
static void flow_clear_alias(struct expression* p_expression)
{
    if (p_expression->declarator)
        p_expression->declarator->p_alias_of_expression = NULL;

    struct argument_expression* _Opt p = p_expression->argument_expression_list.head;
    while (p)
    {
        flow_clear_alias(p->expression);
        p = p->next;
    }


    if (p_expression->left)
        flow_clear_alias(p_expression->left);
    if (p_expression->right)
        flow_clear_alias(p_expression->right);
}

static void flow_expression_bind(struct flow_visit_ctx* ctx,
                                 struct expression* p_expression,
                                 struct param_list* p_param_list,
                                 struct argument_expression_list* p_argument_expression_list)
{

    if (p_expression->expression_type == PRIMARY_EXPRESSION_DECLARATOR)
    {
        assert(p_expression->declarator != NULL);
        assert(p_expression->declarator->name_opt != NULL);

        struct argument_expression* _Opt p_argument_expression =
            param_list_find_argument_by_name(p_param_list,
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
        flow_expression_bind(ctx, p->expression, p_param_list, p_argument_expression_list);
        p = p->next;
    }


    if (p_expression->left)
        flow_expression_bind(ctx, p_expression->left, p_param_list, p_argument_expression_list);
    if (p_expression->right)
        flow_expression_bind(ctx, p_expression->right, p_param_list, p_argument_expression_list);
}


static void flow_visit_expression(struct flow_visit_ctx* ctx, struct expression* p_expression, struct true_false_set* expr_true_false_set)
{
    true_false_set_clear(expr_true_false_set); //out

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    switch (p_expression->expression_type)
    {
    case EXPRESSION_TYPE_INVALID:
        assert(false);
        break;

    case PRIMARY_EXPRESSION__FUNC__:
        break;

    case PRIMARY_EXPRESSION_ENUMERATOR:

        break;
    case PRIMARY_EXPRESSION_DECLARATOR:
    {
        assert(p_expression->declarator != NULL);

        if (p_expression->declarator->p_alias_of_expression)
        {
            /*
               Contracts:
               in this case we visit the expression that this declaration
               is representing
            */
            flow_visit_expression(ctx, p_expression->declarator->p_alias_of_expression, expr_true_false_set);
        }
        else
        {
            _Opt struct true_false_set_item item = { 0 };
            item.p_expression = p_expression;
            item.true_branch_state = BOOLEAN_FLAG_TRUE;
            item.false_branch_state = BOOLEAN_FLAG_FALSE;
            true_false_set_push_back(expr_true_false_set, &item);
            check_uninitialized(ctx, p_expression);
        }
    }
    break;

    case PRIMARY_EXPRESSION_PARENTESIS:
        assert(p_expression->right != NULL);
        flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
        break;

    case PRIMARY_EXPRESSION_STRING_LITERAL:
    case PRIMARY_EXPRESSION_CHAR_LITERAL:
    case PRIMARY_EXPRESSION_NUMBER:
    case PRIMARY_EXPRESSION_PREDEFINED_CONSTANT:
        break;

    case PRIMARY_EXPRESSION_GENERIC:
        assert(p_expression->generic_selection != NULL);
        flow_visit_generic_selection(ctx, p_expression->generic_selection);
        break;

    case POSTFIX_DOT:
    {
        assert(p_expression->left != NULL);

        struct true_false_set left_set = { 0 };
        flow_visit_expression(ctx, p_expression->left, &left_set);

        true_false_set_destroy(&left_set);

        struct true_false_set_item item;
        item.p_expression = p_expression;
        item.true_branch_state = BOOLEAN_FLAG_TRUE;
        item.false_branch_state = BOOLEAN_FLAG_FALSE;
        true_false_set_push_back(expr_true_false_set, &item);
    }
    break;

    case POSTFIX_ARROW:
    {
        assert(p_expression->left != NULL);

        struct true_false_set left_set = { 0 };
        flow_visit_expression(ctx, p_expression->left, &left_set);
        true_false_set_destroy(&left_set);

        struct flow_object* _Opt p_object = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);

        if (p_object != NULL)
        {
            if (flow_object_can_be_null(p_object))
            {
                if (ctx->expression_is_not_evaluated)
                {
                }
                else
                {
                    struct marker marker = { 0 };
                    marker.p_token_begin = p_expression->left->first_token;
                    marker.p_token_end = p_expression->left->last_token;
                    compiler_diagnostic(W_FLOW_NULL_DEREFERENCE,
                            ctx->ctx,
                            NULL,
                            &marker,
                           "pointer may be null");
                }
            }
            else if (flow_object_can_be_uninitialized(p_object))
            {
                if (ctx->expression_is_not_evaluated)
                {
                }
                else
                {
                    compiler_diagnostic(W_FLOW_NULL_DEREFERENCE,
                            ctx->ctx,
                            p_expression->left->first_token, NULL, "object is possibly uninitialized");
                }
            }
            else if (flow_object_can_have_its_lifetime_ended(p_object))
            {
                if (ctx->expression_is_not_evaluated)
                {
                }
                else
                {
                    compiler_diagnostic(W_FLOW_LIFETIME_ENDED,
                            ctx->ctx,
                            p_expression->left->first_token, NULL, "object lifetime ended");
                }
            }
        }

        if (!ctx->expression_is_not_evaluated)
        {
            struct flow_object* _Opt p_object2 = expression_get_flow_object(ctx, p_expression, nullable_enabled);
            if (p_object2 && flow_object_can_have_its_lifetime_ended(p_object2))
            {
                struct marker marker = {
                    .p_token_begin = p_expression->first_token,
                    .p_token_end = p_expression->last_token
                };
                compiler_diagnostic(W_FLOW_LIFETIME_ENDED,
                        ctx->ctx,
                        NULL,
                        &marker,
                        "object lifetime ended");
            }
        }

        struct true_false_set_item item;
        item.p_expression = p_expression;
        item.true_branch_state = BOOLEAN_FLAG_TRUE;
        item.false_branch_state = BOOLEAN_FLAG_FALSE;
        true_false_set_push_back(expr_true_false_set, &item);
    }
    break;

    case POSTFIX_INCREMENT:
    case POSTFIX_DECREMENT:
    {
        assert(p_expression->left != NULL);

        struct flow_object* const _Opt p_object = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);
        if (p_object)
        {
            if (flow_object_is_null(p_object))
            {
                //p_object->current.state &= ~FLOW_OBJECT_STATE_NULL;
                p_object->current.state = FLOW_OBJECT_STATE_NOT_NULL;
            }
            else if (flow_object_is_zero(p_object))
            {
                //p_object->current.state &= ~FLOW_OBJECT_STATE_ZERO;
                p_object->current.state = FLOW_OBJECT_STATE_NOT_ZERO;
            }
        }
        flow_visit_expression(ctx, p_expression->left, expr_true_false_set);
    }
    break;

    case POSTFIX_ARRAY:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        flow_visit_expression(ctx, p_expression->left, expr_true_false_set);
        flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
        true_false_set_clear(expr_true_false_set);

        struct true_false_set_item item;
        item.p_expression = p_expression;
        item.true_branch_state = BOOLEAN_FLAG_TRUE;
        item.false_branch_state = BOOLEAN_FLAG_FALSE;
        true_false_set_push_back(expr_true_false_set, &item);

    }
    break;

    case POSTFIX_FUNCTION_CALL:
    {
        if (!ctx->inside_contract)
        {
            assert(p_expression->left != NULL);

            struct true_false_set left_local = { 0 };
            flow_visit_expression(ctx, p_expression->left, &left_local);

            flow_compare_function_arguments(ctx, &p_expression->left->type, &p_expression->argument_expression_list);
            true_false_set_destroy(&left_local);
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////
#if EXPERIMENTAL_CONTRACTS
        if (p_expression->left->declarator &&
            type_is_function(&p_expression->left->declarator->type))
        {
            struct type return_type = get_function_return_type(&p_expression->left->declarator->type);
            if (p_expression->left->declarator->p_expression_true)
            {
                struct expression* p_expression_true = p_expression->left->declarator->p_expression_true;

                /*given you expression we clear all previous alias*/
                flow_clear_alias(p_expression_true);

                /*then we bind new alias*/
                flow_expression_bind(ctx,
                                     p_expression_true,
                                     &p_expression->left->declarator->type.params,
                                     &p_expression->argument_expression_list);


                if (type_is_scalar(&return_type))
                {
                    struct true_false_set local = { 0 };

                    bool inside_contract = ctx->inside_contract;

                    ctx->inside_contract = true;
                    flow_visit_expression(ctx, p_expression_true, &local);
                    ctx->inside_contract = inside_contract; //restore

                    for (int i = 0; i < local.size; i++)
                    {
                        struct true_false_set_item item5 = {
                          .p_expression = local.data[i].p_expression,
                          .true_branch_state = local.data[i].true_branch_state
                        };

                        true_false_set_push_back(expr_true_false_set, &item5);
                    }
                    true_false_set_destroy(&local);
                }
                else
                {
                    struct true_false_set true_false_set4 = { 0 };
                    bool old = ctx->inside_assert;
                    ctx->inside_assert = true;

                    bool inside_contract = ctx->inside_contract;
                    ctx->inside_contract = true;
                    flow_visit_expression(ctx, p_expression->left->declarator->p_expression_true, &true_false_set4); //assert(p == 0);            
                    ctx->inside_contract = inside_contract; //restore

                    ctx->inside_assert = old;
                    true_false_set_set_objects_to_true_branch(ctx, &true_false_set4, nullable_enabled);
                    true_false_set_destroy(&true_false_set4);
                }
            }

            if (p_expression->left->declarator->p_expression_false)
            {
                struct expression* p_expression_false = p_expression->left->declarator->p_expression_false;

                /*given you expression we clear all previous alias*/
                flow_clear_alias(p_expression_false);

                /*then we bind new alias*/
                flow_expression_bind(ctx,
                                     p_expression_false,
                                     &p_expression->left->declarator->type.params,
                                     &p_expression->argument_expression_list);


                struct true_false_set local = { 0 };



                bool inside_contract = ctx->inside_contract;

                ctx->inside_contract = true;
                flow_visit_expression(ctx, p_expression_false, &local);
                ctx->inside_contract = inside_contract; //restore

                for (int i = 0; i < local.size; i++)
                {
                    int index =
                        find_item_index_by_expression(expr_true_false_set, local.data[i].p_expression);
                    if (index == -1)
                    {
                        struct true_false_set_item item5 = {
                          .p_expression = local.data[i].p_expression,
                          .false_branch_state = local.data[i].true_branch_state
                        };

                        true_false_set_push_back(expr_true_false_set, &item5);
                    }
                    else
                    {
                        expr_true_false_set->data[index].false_branch_state |= local.data[i].false_branch_state;
                    }

                }
                true_false_set_destroy(&local);

            }
            type_destroy(&return_type);

        }
#endif
    }
    break;

    case POSTFIX_EXPRESSION_FUNCTION_LITERAL:
        assert(p_expression->compound_statement != NULL);
        flow_visit_compound_statement(ctx, p_expression->compound_statement);

        break;

    case POSTFIX_EXPRESSION_COMPOUND_LITERAL:

        assert(p_expression->left == NULL);
        assert(p_expression->right == NULL);
        assert(p_expression->type_name != NULL);
        assert(p_expression->braced_initializer != NULL);


        flow_visit_bracket_initializer_list(ctx, p_expression->braced_initializer);

        struct flow_object* _Opt temp2 = make_flow_object(ctx, &p_expression->type, NULL, p_expression);
        if (temp2 == NULL)
        {
            return;
        }

        if (p_expression->type_name->abstract_declarator->p_flow_object)
        {
            flow_object_swap(temp2, p_expression->type_name->abstract_declarator->p_flow_object);

            //TODO the state of object depends of the initializer        
            flow_object_set_zero(&p_expression->type, p_expression->type_name->abstract_declarator->p_flow_object);
        }

        break;

    case UNARY_EXPRESSION_ALIGNOF:

        if (p_expression->right)
        {
            flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
        }


        break;

    case UNARY_EXPRESSION_ASSERT:

        if (p_expression->right)
        {
            struct true_false_set true_false_set4 = { 0 };
            bool old = ctx->inside_assert;
            ctx->inside_assert = true;
            flow_visit_expression(ctx, p_expression->right, &true_false_set4); //assert(p == 0);            
            ctx->inside_assert = old;
            true_false_set_set_objects_to_true_branch(ctx, &true_false_set4, nullable_enabled);
            true_false_set_destroy(&true_false_set4);
        }

        break;

    case UNARY_EXPRESSION_SIZEOF_EXPRESSION:

        if (p_expression->right)
        {
            const bool t2 = ctx->expression_is_not_evaluated;
            ctx->expression_is_not_evaluated = true;
            flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
            ctx->expression_is_not_evaluated = t2;
        }


        break;

    case UNARY_EXPRESSION_NEG:
    case UNARY_EXPRESSION_PLUS:
        assert(p_expression->right != NULL);
        flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
        break;

    case UNARY_EXPRESSION_NOT:
        assert(p_expression->right != NULL);
        flow_check_pointer_used_as_bool(ctx, p_expression->right);
        flow_visit_expression(ctx, p_expression->right, expr_true_false_set);
        true_false_set_invert(expr_true_false_set);
        break;

    case UNARY_EXPRESSION_SIZEOF_TYPE:
    case UNARY_EXPRESSION_NELEMENTSOF_TYPE:
    case UNARY_EXPRESSION_INCREMENT:
    case UNARY_EXPRESSION_DECREMENT:
    case UNARY_EXPRESSION_BITNOT:
    case UNARY_EXPRESSION_ADDRESSOF:
        break;

    case UNARY_EXPRESSION_CONTENT:
    {
        assert(p_expression->right != NULL);

        struct flow_object* _Opt p_object0 = expression_get_flow_object(ctx, p_expression->right, nullable_enabled);

        if (p_object0 && p_object0->current.state == FLOW_OBJECT_STATE_UNINITIALIZED)
        {
            if (!ctx->expression_is_not_evaluated)
            {
                struct marker marker = { 0 };
                marker.p_token_begin = p_expression->right->first_token;
                marker.p_token_end = p_expression->right->last_token;
                compiler_diagnostic(W_FLOW_UNINITIALIZED,
                    ctx->ctx,
                    NULL, &marker, "using a uninitialized object");
            }
        }
        else if (p_object0 && flow_object_can_be_null(p_object0))
        {
            /*
              *p = 1*
            */
            if (!ctx->expression_is_not_evaluated)
            {
                compiler_diagnostic(W_FLOW_NULL_DEREFERENCE,
                    ctx->ctx,
                    p_expression->right->first_token, NULL, "dereference a NULL object");
            }
        }

        struct true_false_set local_true_false = { 0 };
        flow_visit_expression(ctx, p_expression->right, &local_true_false);
        /*empty set*/
        true_false_set_destroy(&local_true_false);
    }
    break;

    case ASSIGNMENT_EXPRESSION_ASSIGN:
    case ASSIGNMENT_EXPRESSION_PLUS_ASSIGN:
    case ASSIGNMENT_EXPRESSION_MINUS_ASSIGN:
    case ASSIGNMENT_EXPRESSION_MULTI_ASSIGN:
    case ASSIGNMENT_EXPRESSION_DIV_ASSIGN:
    case ASSIGNMENT_EXPRESSION_MOD_ASSIGN:
    case ASSIGNMENT_EXPRESSION_SHIFT_LEFT_ASSIGN:
    case ASSIGNMENT_EXPRESSION_SHIFT_RIGHT_ASSIGN:
    case ASSIGNMENT_EXPRESSION_AND_ASSIGN:
    case ASSIGNMENT_EXPRESSION_OR_ASSIGN:
    case ASSIGNMENT_EXPRESSION_NOT_ASSIGN:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        struct true_false_set left_set = { 0 };
        flow_visit_expression(ctx, p_expression->left, &left_set);
        true_false_set_swap(expr_true_false_set, &left_set);
        true_false_set_destroy(&left_set);

        struct true_false_set right_set = { 0 };
        flow_visit_expression(ctx, p_expression->right, &right_set);
        true_false_set_destroy(&right_set);

        //struct object temp_obj1 = { 0 };
        struct flow_object* const _Opt p_right_object = expression_get_flow_object(ctx, p_expression->right, nullable_enabled);

        //struct object temp_obj2 = { 0 };
        struct flow_object* const _Opt p_dest_object = expression_get_flow_object(ctx, p_expression->left, nullable_enabled);

        if (p_dest_object == NULL || p_right_object == NULL)
        {
            return;
        }
        //TODO

        struct marker a_marker = {
          .p_token_begin = p_expression->left->first_token,
          .p_token_end = p_expression->left->last_token
        };

        struct marker b_marker = {
          .p_token_begin = p_expression->right->first_token,
          .p_token_end = p_expression->right->last_token
        };

        flow_check_assignment(ctx,
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
        arena_broadcast_change(ctx, p_dest_object);

        if (expression_is_malloc(p_expression->right))
        {
            struct type t = type_remove_pointer(&p_expression->left->type);
            struct flow_object* _Opt po = make_flow_object(ctx, &t, NULL, p_expression->left);
            if (po == NULL)
            {
                type_destroy(&t);
                return;
            }
            object_set_pointer(p_dest_object, po);
            type_destroy(&t);
            p_dest_object->current.state = FLOW_OBJECT_STATE_NOT_NULL | FLOW_OBJECT_STATE_NULL;
        }
        else if (expression_is_calloc(p_expression->right))
        {
            struct type t = type_remove_pointer(&p_expression->left->type);
            struct flow_object* _Opt po = make_flow_object(ctx, &t, NULL, p_expression->left);
            if (po == NULL)
            {
                type_destroy(&t);
                return;
            }
            flow_object_set_zero(&t, po);
            object_set_pointer(p_dest_object, po);
            type_destroy(&t);
            p_dest_object->current.state = FLOW_OBJECT_STATE_NOT_NULL | FLOW_OBJECT_STATE_NULL;
        }
    }
    break;
    case MULTIPLICATIVE_EXPRESSION_DIV:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        struct true_false_set left_set = { 0 };
        struct true_false_set right_set = { 0 };

        flow_visit_expression(ctx, p_expression->left, &left_set);

        struct flow_object* _Opt p_object = expression_get_flow_object(ctx, p_expression->right, ctx->ctx->options.null_checks_enabled);
        if (p_object)
        {
            if (flow_object_can_be_zero(p_object))
            {
                compiler_diagnostic(W_FLOW_DIVIZION_BY_ZERO, ctx->ctx, p_expression->right->first_token, NULL, "possible division by zero");
            }
        }

        /*
                               true_set               false_set
             b / a             b_true_set a_true_set  a_true_set
             0 / a             -                      a_true_set a_true_set
        */

        flow_visit_expression(ctx, p_expression->right, &right_set);
        true_false_set_merge(expr_true_false_set, &left_set, &right_set,
            MERGE_OPTIONS_A_TRUE | MERGE_OPTIONS_B_TRUE,
            MERGE_OPTIONS_A_TRUE | MERGE_OPTIONS_B_TRUE);

        true_false_set_destroy(&left_set);
        true_false_set_destroy(&right_set);
    }
    break;
    case CAST_EXPRESSION:
    case MULTIPLICATIVE_EXPRESSION_MULT:
    case MULTIPLICATIVE_EXPRESSION_MOD:
    case ADDITIVE_EXPRESSION_PLUS:
    case ADDITIVE_EXPRESSION_MINUS:
    case SHIFT_EXPRESSION_RIGHT:
    case SHIFT_EXPRESSION_LEFT:
    {
        if (p_expression->left)
        {
            struct true_false_set left_set = { 0 };
            flow_visit_expression(ctx, p_expression->left, &left_set);
            true_false_set_destroy(&left_set);
        }

        if (p_expression->right)
        {
            struct true_false_set right_set = { 0 };
            flow_visit_expression(ctx, p_expression->right, &right_set);
            true_false_set_destroy(&right_set);
        }
    }
    break;

    case RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN:
    case RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN:
    case RELATIONAL_EXPRESSION_BIGGER_THAN:
    case RELATIONAL_EXPRESSION_LESS_THAN:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        const bool left_is_constant = object_has_constant_value(&p_expression->left->object);
        const bool right_is_constant = object_has_constant_value(&p_expression->right->object);

        if (left_is_constant)
        {
            const long long left_value = object_to_signed_long_long(&p_expression->left->object);

            struct true_false_set true_false_set_right = { 0 };
            flow_visit_expression(ctx, p_expression->right, &true_false_set_right);
            if (left_value == 0)
            {
                true_false_set_swap(expr_true_false_set, &true_false_set_right);
                for (int i = 0; i < expr_true_false_set->size; i++)
                {
                    struct true_false_set_item* item = &expr_true_false_set->data[i];
                    item->false_branch_state |= item->true_branch_state;
                }

                if (p_expression->expression_type == RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN ||
                    p_expression->expression_type == RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN)
                {
                    true_false_set_invert(expr_true_false_set);
                }
            }
            true_false_set_destroy(&true_false_set_right);
        }

        else if (right_is_constant)
        {
            const long long right_value = object_to_signed_long_long(&p_expression->right->object);
            struct true_false_set true_false_set_left3 = { 0 };
            flow_visit_expression(ctx, p_expression->left, &true_false_set_left3);
            if (right_value == 0)
            {
                true_false_set_swap(expr_true_false_set, &true_false_set_left3);
                for (int i = 0; i < expr_true_false_set->size; i++)
                {
                    struct true_false_set_item* item = &expr_true_false_set->data[i];
                    item->false_branch_state |= item->true_branch_state;
                }
                if (p_expression->expression_type == RELATIONAL_EXPRESSION_BIGGER_OR_EQUAL_THAN ||
                    p_expression->expression_type == RELATIONAL_EXPRESSION_LESS_OR_EQUAL_THAN)
                {
                    true_false_set_invert(expr_true_false_set);
                }
            }
            true_false_set_destroy(&true_false_set_left3);
        }
        else
        {
            struct true_false_set true_false_set = { 0 };
            flow_visit_expression(ctx, p_expression->left, &true_false_set);
            flow_visit_expression(ctx, p_expression->right, &true_false_set);
            true_false_set_destroy(&true_false_set);
        }
    }
    break;

    case EQUALITY_EXPRESSION_NOT_EQUAL:
    case EQUALITY_EXPRESSION_EQUAL:
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
            struct true_false_set true_false_set = { 0 };
            flow_visit_expression(ctx, p_comp_expression, &true_false_set);

            //constant == p_comp_expression  |  p_comp_expression == constant
            //constant != p_comp_expression  |  p_comp_expression != constant

            struct flow_object* _Opt p_object = expression_get_flow_object(ctx, p_comp_expression, nullable_enabled);
            if (p_object)
            {
                struct marker marker = {
                     .p_token_begin = p_comp_expression->first_token,
                     .p_token_end = p_comp_expression->last_token
                };


                if ((flow_object_is_null(p_object) || flow_object_is_zero(p_object)) && value == 0)
                {
                    if (p_expression->expression_type == EQUALITY_EXPRESSION_EQUAL)
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
                                    compiler_diagnostic(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "pointer is always null");
                            }
                            else
                            {
                                if (!ctx->inside_loop)
                                    compiler_diagnostic(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "value is always zero");
                            }
                        }
                    }
                    else if (p_expression->expression_type == EQUALITY_EXPRESSION_NOT_EQUAL)
                    {
                        /*
                           runtime check is diferent from static state
                           assert(p != NULL);
                        */
                        if (type_is_pointer(&p_comp_expression->type))
                            compiler_diagnostic(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "pointer is always null");
                        else
                            compiler_diagnostic(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "value is always zero");
                    }
                }
                else if ((flow_object_is_not_null(p_object) || flow_object_is_not_zero(p_object)) && value == 0)
                {
                    if (p_expression->expression_type == EQUALITY_EXPRESSION_EQUAL)
                    {
                        /*
                           runtime check is diferent from static state
                           assert(p == NULL);
                        */
                        if (type_is_pointer(&p_comp_expression->type))
                        {
                            if (!ctx->inside_loop)
                                compiler_diagnostic(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "pointer is always non-null");
                        }
                        else
                        {
                            if (!ctx->inside_loop)
                                compiler_diagnostic(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "value is always non-zero");
                        }
                    }
                    else if (p_expression->expression_type == EQUALITY_EXPRESSION_NOT_EQUAL)
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
                                    compiler_diagnostic(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "pointer is always non-null");
                            }
                            else
                            {
                                if (!ctx->inside_loop)
                                    compiler_diagnostic(W_FLOW_NON_NULL, ctx->ctx, NULL, &marker, "value is always non-zero");
                            }
                        }
                    }
                }
            }

            true_false_set_swap(expr_true_false_set, &true_false_set);
            if (p_expression->expression_type == EQUALITY_EXPRESSION_EQUAL && value == 0)
            {
                true_false_set_invert(expr_true_false_set);
            }
            else if (p_expression->expression_type == EQUALITY_EXPRESSION_NOT_EQUAL && value != 0)
            {
                true_false_set_invert(expr_true_false_set);
            }
            true_false_set_destroy(&true_false_set);
        }
        else
        {
            struct true_false_set true_false_set = { 0 };
            flow_visit_expression(ctx, p_expression->left, &true_false_set);
            flow_visit_expression(ctx, p_expression->right, &true_false_set);
            true_false_set_destroy(&true_false_set);
        }
    }
    break;

    case LOGICAL_OR_EXPRESSION:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        flow_check_pointer_used_as_bool(ctx, p_expression->left);
        flow_check_pointer_used_as_bool(ctx, p_expression->right);


        struct true_false_set left_set = { 0 };
        flow_visit_expression(ctx, p_expression->left, &left_set);

        if (object_has_constant_value(&p_expression->left->object) &&
            object_to_bool(&p_expression->left->object) == true)
        {
            // left || right
            //left is true, so the right side will not run
        }
        else
        {
            const int original_state_number = arena_add_copy_of_current_state(ctx, "original");

            //Set all variables to false state, because otherwise, the right branch
            // would not be evaluated
            true_false_set_set_objects_to_false_branch(ctx, &left_set, nullable_enabled);

            struct true_false_set right_set = { 0 };
            flow_visit_expression(ctx, p_expression->right, &right_set);

            //Tudo que faz left ser true ou right ser true

            for (int i = 0; i < left_set.size; i++)
            {
                struct true_false_set_item item5;

                item5.p_expression = left_set.data[i].p_expression;
                item5.true_branch_state |= (left_set.data[i].true_branch_state | left_set.data[i].false_branch_state);
                item5.false_branch_state |= left_set.data[i].false_branch_state;
                true_false_set_push_back(expr_true_false_set, &item5);
            }

            for (int k = 0; k < right_set.size; k++)
            {
                int index =
                    find_item_index_by_expression(expr_true_false_set, right_set.data[k].p_expression);
                if (index == -1)
                {
                    index = expr_true_false_set->size;

                    struct true_false_set_item item4 = {
                      .p_expression = right_set.data[k].p_expression
                    };

                    true_false_set_push_back(expr_true_false_set, &item4);
                }

                //Tudo que faz left true e right true faz expressao se true
                expr_true_false_set->data[index].p_expression = right_set.data[k].p_expression;
                //d->data[index].true_branch_state |= right_set.data[k].true_branch_state;
                //Tudo que faz left true ou left false, e right false faz ser false
                expr_true_false_set->data[index].false_branch_state |= right_set.data[k].false_branch_state;

                //No path true seria possivel nao ser feito o right
                expr_true_false_set->data[index].true_branch_state |= (BOOLEAN_FLAG_TRUE | BOOLEAN_FLAG_FALSE);

            }

            //TODO we whould not restore the previous states for states that 
            //are not true/false
            // if (false || init(a)) ... we cannot undo the out a
            arena_restore_current_state_from(ctx, original_state_number);
            arena_remove_state(ctx, original_state_number);
            true_false_set_destroy(&right_set);
        }
        true_false_set_destroy(&left_set);
    }
    break;

    case LOGICAL_AND_EXPRESSION:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        flow_check_pointer_used_as_bool(ctx, p_expression->left);
        flow_check_pointer_used_as_bool(ctx, p_expression->right);


        struct true_false_set left_set = { 0 };
        flow_visit_expression(ctx, p_expression->left, &left_set);

        const int original_state_number = arena_add_copy_of_current_state(ctx, "original");

        //Set all variables to true state, because otherwise, the right branch
        // would not be evaluated
        true_false_set_set_objects_to_true_branch(ctx, &left_set, nullable_enabled);

        struct true_false_set right_set = { 0 };
        flow_visit_expression(ctx, p_expression->right, &right_set);
        //arena_restore_current_state_from(ctx, original_state_number);

       //Anything that makes left and right true also makes left && right true. 
       //Anything that makes left false or right false also makes left && right false.

        for (int i = 0; i < left_set.size; i++)
        {
            const struct true_false_set_item* p_item_left = &left_set.data[i];

            _Opt struct true_false_set_item left_and_right = { 0 };
            left_and_right.p_expression = p_item_left->p_expression;

            left_and_right.true_branch_state |= p_item_left->true_branch_state;
            left_and_right.false_branch_state |= p_item_left->true_branch_state | p_item_left->false_branch_state;

            true_false_set_push_back(expr_true_false_set, &left_and_right);
        }

        for (int k = 0; k < right_set.size; k++)
        {
            const struct true_false_set_item* p_item_right = &right_set.data[k];

            int index = find_item_index_by_expression(expr_true_false_set, p_item_right->p_expression);
            if (index == -1)
            {
                index = expr_true_false_set->size;
                struct true_false_set_item item2 = {
                  .p_expression = p_item_right->p_expression
                };
                true_false_set_push_back(expr_true_false_set, &item2);
            }

            //Tudo que faz left true e right true faz expressao se true
            expr_true_false_set->data[index].p_expression = p_item_right->p_expression;
            expr_true_false_set->data[index].true_branch_state |= p_item_right->true_branch_state;
            //Tudo que faz left true ou left false, e right false faz ser false
            expr_true_false_set->data[index].false_branch_state |= p_item_right->false_branch_state;

            //right expression may not be evaluated, in this case all previous states are also valid
            //so if the variable could be true and false then we need to add            
            expr_true_false_set->data[index].false_branch_state |= (BOOLEAN_FLAG_TRUE | BOOLEAN_FLAG_FALSE);

        }

        arena_restore_current_state_from(ctx, original_state_number);
        arena_remove_state(ctx, original_state_number);
        true_false_set_destroy(&left_set);
        true_false_set_destroy(&right_set);
    }
    break;

    case INCLUSIVE_OR_EXPRESSION:
    {
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        // A | B
        struct true_false_set true_false_set = { 0 };

        flow_visit_expression(ctx, p_expression->left, &true_false_set);
        flow_visit_expression(ctx, p_expression->right, &true_false_set);

        true_false_set_destroy(&true_false_set);
    }
    break;

    case AND_EXPRESSION:
    case EXCLUSIVE_OR_EXPRESSION:

        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);

        flow_visit_expression(ctx, p_expression->left, expr_true_false_set);
        flow_visit_expression(ctx, p_expression->right, expr_true_false_set);


        break;

    case UNARY_EXPRESSION_TRAITS:
        break;

    case UNARY_EXPRESSION_IS_SAME:
        break;

    case UNARY_DECLARATOR_ATTRIBUTE_EXPR:
        break;

    case EXPRESSION_EXPRESSION:
        break;

    case CONDITIONAL_EXPRESSION:
    {
        assert(p_expression->condition_expr != NULL);
        assert(p_expression->right != NULL);
        assert(p_expression->left != NULL);


        struct true_false_set true_false_set = { 0 };

        flow_check_pointer_used_as_bool(ctx, p_expression->condition_expr);
        flow_visit_expression(ctx, p_expression->condition_expr, &true_false_set);

        const int before_if_state_number = arena_add_copy_of_current_state(ctx, "before-if");

        true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);

        struct true_false_set set = { 0 };
        flow_visit_expression(ctx, p_expression->left, &set);
        true_false_set_destroy(&set);


        const int left_true_branch_state_number = arena_add_copy_of_current_state(ctx, "left-true-branch");

        arena_restore_current_state_from(ctx, before_if_state_number);

        true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);

        struct true_false_set set2 = { 0 };
        flow_visit_expression(ctx, p_expression->right, &set2);
        true_false_set_destroy(&set2);

        arena_merge_current_state_with_state_number(ctx, left_true_branch_state_number);

        arena_restore_current_state_from(ctx, left_true_branch_state_number);
        arena_remove_state(ctx, before_if_state_number);
        arena_remove_state(ctx, left_true_branch_state_number);
        true_false_set_destroy(&true_false_set);
    }
    break;

    }
}

static void flow_visit_expression_statement(struct flow_visit_ctx* ctx, struct expression_statement* p_expression_statement)
{
    struct true_false_set d = { 0 };
    if (p_expression_statement->expression_opt)
        flow_visit_expression(ctx, p_expression_statement->expression_opt, &d);
    true_false_set_destroy(&d);
}

static void flow_visit_block_item_list(struct flow_visit_ctx* ctx, struct block_item_list* p_block_item_list);

static void flow_visit_compound_statement(struct flow_visit_ctx* ctx, struct compound_statement* p_compound_statement)
{
    flow_visit_block_item_list(ctx, &p_compound_statement->block_item_list);
    flow_exit_block_visit_defer_list(ctx, &p_compound_statement->defer_list, p_compound_statement->last_token);
    flow_defer_list_set_end_of_lifetime(ctx, &p_compound_statement->defer_list, p_compound_statement->last_token);
}

static void flow_visit_do_while_statement(struct flow_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    assert(p_iteration_statement->first_token->type == TK_KEYWORD_DO);
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    struct true_false_set true_false_set = { 0 };

    try
    {
        if (p_iteration_statement->expression1)
        {
            flow_visit_expression(ctx, p_iteration_statement->expression1, &true_false_set);
        }

        flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

        flow_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);
        flow_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);

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
            true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
        }
    }
    catch
    {
    }

    true_false_set_destroy(&true_false_set);
}

static void flow_visit_while_statement(struct flow_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    assert(p_iteration_statement->first_token->type == TK_KEYWORD_WHILE);

    if (p_iteration_statement->expression1 == NULL)
        return;

    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    const int old_initial_state = ctx->initial_state;
    const int old_break_join_state = ctx->break_join_state;

    ctx->initial_state = arena_add_copy_of_current_state(ctx, "original");
    ctx->break_join_state = arena_add_empty_state(ctx, "break join");

    struct true_false_set true_false_set = { 0 };

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
    flow_visit_expression(ctx, p_iteration_statement->expression1, &true_false_set);


    true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);

    flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

    //Second pass warning is ON
    diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);

    struct true_false_set true_false_set2 = { 0 };
    flow_visit_expression(ctx, p_iteration_statement->expression1, &true_false_set2);
    true_false_set_destroy(&true_false_set2);

    //visit secondary_block again
    true_false_set_set_objects_to_true_branch(ctx, &true_false_set, nullable_enabled);
    flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

    flow_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);

    const bool was_last_statement_inside_true_branch_return =
        secondary_block_ends_with_jump(p_iteration_statement->secondary_block);

    if (was_last_statement_inside_true_branch_return)
    {
        /*
           while (p) { return; }
        */
        arena_restore_current_state_from(ctx, ctx->initial_state);
        true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
    }
    else
    {
        true_false_set_set_objects_to_false_branch(ctx, &true_false_set, nullable_enabled);
        arena_merge_current_state_with_state_number(ctx, ctx->break_join_state);
        arena_restore_current_state_from(ctx, ctx->break_join_state);
    }

    flow_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);

    arena_remove_state(ctx, ctx->initial_state);
    arena_remove_state(ctx, ctx->break_join_state);


    //restore
    ctx->initial_state = old_initial_state;
    ctx->break_join_state = old_break_join_state;
    true_false_set_destroy(&true_false_set);
}

static void flow_visit_for_statement(struct flow_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    /*
      TODO
       {
        int i = 0;
        if (condition)
        {
            //second-block
            i++;
            if (condition)
            {
                //second-block
                i++;
                //recursive
            }
        }
    }
    */

    assert(p_iteration_statement->first_token->type == TK_KEYWORD_FOR);

    struct true_false_set d = { 0 };

    try
    {
        if (p_iteration_statement->declaration &&
            p_iteration_statement->declaration->init_declarator_list.head)
        {
            flow_visit_init_declarator_list(ctx, &p_iteration_statement->declaration->init_declarator_list);
        }

        if (p_iteration_statement->expression0)
        {
            flow_visit_expression(ctx, p_iteration_statement->expression0, &d);
        }

        if (p_iteration_statement->expression1)
        {
            flow_check_pointer_used_as_bool(ctx, p_iteration_statement->expression1);
            flow_visit_expression(ctx, p_iteration_statement->expression1, &d);
        }

        //TODO we need to merge states inside loops

        //Disable warning because the state is temporary..missing a visit
        diagnostic_stack_push_empty(&ctx->ctx->options.diagnostic_stack);

        flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

        diagnostic_stack_pop(&ctx->ctx->options.diagnostic_stack);


        if (p_iteration_statement->expression2)
        {
            flow_visit_expression(ctx, p_iteration_statement->expression2, &d);
        }
        const bool b_secondary_block_ends_with_jump =
            secondary_block_ends_with_jump(p_iteration_statement->secondary_block);

        /*we visit again*/
        if (!b_secondary_block_ends_with_jump)
        {
            flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);
            flow_exit_block_visit_defer_list(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);
            flow_defer_list_set_end_of_lifetime(ctx, &p_iteration_statement->defer_list, p_iteration_statement->secondary_block->last_token);
        }

    }
    catch
    {
    }
    true_false_set_destroy(&d);
}


static void flow_visit_iteration_statement(struct flow_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{
    bool inside_loop = ctx->inside_loop;
    ctx->inside_loop = true;

    switch (p_iteration_statement->first_token->type)
    {
    case  TK_KEYWORD_WHILE:
        flow_visit_while_statement(ctx, p_iteration_statement);
        break;
    case TK_KEYWORD_DO:
        flow_visit_do_while_statement(ctx, p_iteration_statement);
        break;
    case TK_KEYWORD_FOR:
        flow_visit_for_statement(ctx, p_iteration_statement);
        break;
    default:
        assert(false);
        break;
    }
    ctx->inside_loop = inside_loop; //restore
}

static void flow_visit_jump_statement(struct flow_visit_ctx* ctx, struct jump_statement* p_jump_statement)
{
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;
    try
    {
        if (p_jump_statement->first_token->type == TK_KEYWORD_THROW)
        {
            arena_merge_current_state_with_state_number(ctx, ctx->throw_join_state);

            //check_all_defer_until_try(ctx, ctx->tail_block, p_jump_statement->first_token);
            flow_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
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
                struct true_false_set d = { 0 };
                flow_visit_expression(ctx, p_jump_statement->expression_opt, &d);
                true_false_set_destroy(&d);
            }

            /*
              returning a declarator will move the ownership
            */
            if (p_jump_statement->expression_opt)
            {
                struct flow_object* _Opt p_object =
                    expression_get_flow_object(ctx, p_jump_statement->expression_opt, nullable_enabled);

                if (p_object)
                {
                    assert(ctx->p_return_type != NULL);
                    struct flow_object* _Opt p_dest_object =
                        make_flow_object(ctx, ctx->p_return_type, NULL, p_jump_statement->expression_opt);

                    if (p_dest_object == NULL)
                    {
                        throw;
                    }

                    assert(ctx->p_return_type != NULL);
                    flow_object_set_zero(ctx->p_return_type, p_dest_object);

                    struct marker a_marker = {
                       .p_token_begin = p_jump_statement->expression_opt->first_token,
                       .p_token_end = p_jump_statement->expression_opt->last_token,
                    };
                    struct marker b_marker = {
                       .p_token_begin = p_jump_statement->expression_opt->first_token,
                       .p_token_end = p_jump_statement->expression_opt->last_token,
                    };

                    assert(ctx->p_return_type != NULL);

                    flow_check_assignment(ctx,
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

                    const int state_before_return = arena_add_copy_of_current_state(ctx, "before-return");

                    // Simulate the function's state as if it had already returned. 
                    // This allows us to verify whether the returned object points to any 
                    // local variables.

                    flow_defer_list_set_end_of_lifetime(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);

                    if (ctx->p_return_type == NULL)
                        throw;

                    checked_read_object(ctx,
                                        ctx->p_return_type,
                                        type_is_opt(ctx->p_return_type, ctx->ctx->options.null_checks_enabled),
                                        p_dest_object,
                                        NULL,
                                        &a_marker,
                                        true);

                    //then we restore the state
                    arena_restore_current_state_from(ctx, state_before_return);
                }

                if (p_object && p_object->is_temporary)
                {
                    //a + b
                    p_object->current.state = FLOW_OBJECT_STATE_LIFE_TIME_ENDED;
                }

            }

            flow_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_CONTINUE)
        {
            flow_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
        }
        else if (p_jump_statement->first_token->type == TK_KEYWORD_BREAK)
        {
            arena_merge_current_state_with_state_number(ctx, ctx->break_join_state);
            flow_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
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
                label_state_number = arena_add_empty_state(ctx, p_jump_statement->label->lexeme);
                ctx->labels[ctx->labels_size].state_number = label_state_number;
                ctx->labels[ctx->labels_size].label_name = p_jump_statement->label->lexeme;
                ctx->labels_size++;
            }

            arena_merge_current_state_with_state_number(ctx, label_state_number);
            flow_exit_block_visit_defer_list(ctx, &p_jump_statement->defer_list, p_jump_statement->first_token);
        }
        else
        {
            assert(false);
        }
    }
    catch
    {
    }
}

static void flow_visit_label(struct flow_visit_ctx* ctx, struct label* p_label);

static void flow_visit_labeled_statement(struct flow_visit_ctx* ctx, struct labeled_statement* p_labeled_statement)
{
    flow_visit_label(ctx, p_labeled_statement->label);
    flow_visit_statement(ctx, p_labeled_statement->statement);
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
    if (p_label->p_identifier_opt)
    {
        for (int i = 0; i < ctx->labels_size; i++)
        {
            if (strcmp(ctx->labels[i].label_name, p_label->p_identifier_opt->lexeme) == 0)
            {

                arena_restore_current_state_from(ctx, ctx->labels[i].state_number);

                break; //already exist
            }
        }
    }
    else
    {
        //case, default
        arena_restore_current_state_from(ctx, ctx->initial_state);
    }
}

static void flow_visit_block_item(struct flow_visit_ctx* ctx, struct block_item* p_block_item)
{
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
    struct block_item* _Opt p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        flow_visit_block_item(ctx, p_block_item);
        p_block_item = p_block_item->next;
    }
}

static enum flow_state parse_string_state(const char* s, bool* invalid)
{
    //TODO faling with _
    *invalid = false;

    enum flow_state e = 0;
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
                e |= FLOW_OBJECT_STATE_MOVED;

            else if (strncmp(start, "null", sz) == 0)
                e |= FLOW_OBJECT_STATE_NULL;
            else if (strncmp(start, "not-null", sz) == 0)
                e |= FLOW_OBJECT_STATE_NOT_NULL;
            else if (strncmp(start, "maybe-null", sz) == 0)
                e |= (FLOW_OBJECT_STATE_NOT_NULL | FLOW_OBJECT_STATE_NULL);
            else if (strncmp(start, "uninitialized", sz) == 0)
                e |= FLOW_OBJECT_STATE_UNINITIALIZED;
            else if (strncmp(start, "zero", sz) == 0)
                e |= FLOW_OBJECT_STATE_ZERO;
            else if (strncmp(start, "not-zero", sz) == 0)
                e |= FLOW_OBJECT_STATE_NOT_ZERO;
            else if (strncmp(start, "any", sz) == 0)
                e |= (FLOW_OBJECT_STATE_NOT_ZERO | FLOW_OBJECT_STATE_ZERO);
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

static void flow_visit_pragma_declaration(struct flow_visit_ctx* ctx, struct pragma_declaration* p_pragma_declaration)
{
    execute_pragma(ctx->ctx, p_pragma_declaration, true);
}

static void flow_visit_static_assert_declaration(struct flow_visit_ctx* ctx, struct static_assert_declaration* p_static_assert_declaration)
{
    const bool t2 = ctx->expression_is_not_evaluated;
    ctx->expression_is_not_evaluated = true;
    const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

    struct true_false_set a = { 0 };
    flow_visit_expression(ctx, p_static_assert_declaration->constant_expression, &a);

    ctx->expression_is_not_evaluated = t2; //restore


    if (p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_DEBUG ||
        p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_DEBUG_EX)
    {
        bool ex = p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_DEBUG_EX;

        compiler_diagnostic(W_LOCATION, ctx->ctx, p_static_assert_declaration->first_token, NULL, "static_debug");

        struct flow_object* _Opt p_obj =
            expression_get_flow_object(ctx, p_static_assert_declaration->constant_expression, nullable_enabled);

        if (p_obj)
        {
            print_flow_object(&p_static_assert_declaration->constant_expression->type, p_obj, !ex);
            if (p_obj->is_temporary)
            {
                p_obj->current.state = FLOW_OBJECT_STATE_LIFE_TIME_ENDED;
            }
        }

        if (ex)
        {
            print_arena(ctx);
        }
    }
    else if (p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_STATE)
    {
        /*TODO
           check state

        */

        bool is_invalid = false;
        enum flow_state e = 0;
        if (p_static_assert_declaration->string_literal_opt)
            e = parse_string_state(p_static_assert_declaration->string_literal_opt->lexeme, &is_invalid);
        if (is_invalid)
        {
            compiler_diagnostic(C_ANALIZER_ERROR_STATIC_STATE_FAILED, ctx->ctx, p_static_assert_declaration->first_token, NULL, "invalid parameter %s", p_static_assert_declaration->string_literal_opt->lexeme);
        }
        else
        {

            struct flow_object* _Opt p_obj =
                expression_get_flow_object(ctx, p_static_assert_declaration->constant_expression, nullable_enabled);
            if (p_obj)
            {


                if (e != p_obj->current.state)
                {
                    compiler_diagnostic(C_ANALIZER_ERROR_STATIC_STATE_FAILED, ctx->ctx, p_static_assert_declaration->first_token, NULL, "static_state failed");
                    if (p_static_assert_declaration->string_literal_opt)
                        printf("expected :%s\n", p_static_assert_declaration->string_literal_opt->lexeme);
                    printf("current  :");
                    flow_object_print_state(p_obj);
                    printf("\n");
                }
            }
            else
            {
                if (e != FLOW_OBJECT_STATE_NOT_APPLICABLE)
                {
                    compiler_diagnostic(C_ANALIZER_ERROR_STATIC_STATE_FAILED, ctx->ctx, p_static_assert_declaration->first_token, NULL, "static_state failed");
                }
            }

            if (p_obj && p_obj->is_temporary)
            {
                p_obj->current.state = FLOW_OBJECT_STATE_LIFE_TIME_ENDED;
            }

        }
    }
    else if (p_static_assert_declaration->first_token->type == TK_KEYWORD_STATIC_SET)
    {
        struct flow_object* _Opt p_obj =
            expression_get_flow_object(ctx, p_static_assert_declaration->constant_expression, nullable_enabled);

        if (p_obj)
        {
            if (p_static_assert_declaration->string_literal_opt)
            {
                const char* lexeme =
                    p_static_assert_declaration->string_literal_opt->lexeme;

                if (strcmp(lexeme, "\"zero\"") == 0)
                {
                    //gives the semantics of {0} or calloc
                    flow_object_set_zero(&p_static_assert_declaration->constant_expression->type, p_obj);
                }
                else
                {
                    bool is_invalid = false;
                    enum flow_state e =
                        parse_string_state(p_static_assert_declaration->string_literal_opt->lexeme, &is_invalid);

                    if (!is_invalid)
                    {
                        if (p_obj->members.size > 0)
                        {
                            compiler_diagnostic(C_ERROR_STATIC_SET, ctx->ctx, p_static_assert_declaration->first_token, NULL, "use only for non agregates");
                        }
                        p_obj->current.state = e;
                    }
                    else
                    {
                        compiler_diagnostic(C_ERROR_STATIC_SET, ctx->ctx, p_static_assert_declaration->first_token, NULL, "invalid parameter %s", p_static_assert_declaration->string_literal_opt->lexeme);
                    }
                }
            }

        }
        if (p_obj && p_obj->is_temporary)
        {
            p_obj->current.state = FLOW_OBJECT_STATE_LIFE_TIME_ENDED;
        }

    }

    true_false_set_destroy(&a);
}

static void flow_visit_direct_declarator(struct flow_visit_ctx* ctx, struct direct_declarator* p_direct_declarator)
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
            flow_visit_declaration_specifiers(ctx, parameter->declaration_specifiers, &parameter->declarator->type);

            if (parameter->declarator)
            {
                flow_visit_declarator(ctx, parameter->declarator);
            }

            parameter = parameter->next;
        }

        ctx->parameter_list--;

    }
    else if (p_direct_declarator->array_declarator)
    {
        if (p_direct_declarator->array_declarator->assignment_expression)
        {
            struct true_false_set a = { 0 };
            flow_visit_expression(ctx, p_direct_declarator->array_declarator->assignment_expression, &a);
            true_false_set_destroy(&a);
        }

    }
}

static void flow_visit_declarator(struct flow_visit_ctx* ctx, struct declarator* p_declarator)
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


            p_declarator->p_flow_object = make_flow_object(ctx, &p_declarator->type, p_declarator, NULL);
            if (p_declarator->p_flow_object == NULL)
            {
                throw;
            }

            flow_object_set_uninitialized(&p_declarator->type, p_declarator->p_flow_object);


            if (p_declarator->declaration_specifiers &&
                p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_PARAMETER)
            {
                if (type_is_pointer(&p_declarator->type))
                {
                    if (type_is_opt(&p_declarator->type, ctx->ctx->options.null_checks_enabled))
                    {
                        p_declarator->p_flow_object->current.state = FLOW_OBJECT_STATE_NOT_NULL | FLOW_OBJECT_STATE_NULL;
                    }
                    else
                    {
                        p_declarator->p_flow_object->current.state = FLOW_OBJECT_STATE_NOT_NULL;
                    }

                    if (type_is_pointer_to_out(&p_declarator->type))
                    {
                        struct type t = type_remove_pointer(&p_declarator->type);
                        struct flow_object* _Opt po = make_flow_object(ctx, &t, p_declarator, NULL);
                        if (po == NULL)
                        {
                            type_destroy(&t);
                            throw;
                        }
                        flow_object_set_uninitialized(&t, po);
                        object_set_pointer(p_declarator->p_flow_object, po); //MOVED                    
                        type_destroy(&t);
                    }
                    else if (type_is_owner_or_pointer_to_dtor(&p_declarator->type))
                    {
                        struct type t = type_remove_pointer(&p_declarator->type);
                        struct flow_object* _Opt po = make_flow_object(ctx, &t, p_declarator, NULL);
                        if (po == NULL)
                        {
                            type_destroy(&t);
                            throw;
                        }
                        const bool t_is_nullable = type_is_opt(&t, ctx->ctx->options.null_checks_enabled);
                        flow_object_set_unknown(&t, t_is_nullable, po, nullable_enabled);
                        object_set_pointer(p_declarator->p_flow_object, po); //MOVED                    
                        type_destroy(&t);
                    }
                }
                else if (type_is_struct_or_union(&p_declarator->type))
                {
                    const bool is_nullable = type_is_opt(&p_declarator->type, nullable_enabled);
                    flow_object_set_unknown(&p_declarator->type, is_nullable, p_declarator->p_flow_object, nullable_enabled);
                }
                else if (type_is_array(&p_declarator->type))
                {
                    // assert(false);//TODO
                     //flow_object_set_unknown(&p_declarator->type, &p_declarator->object);
                    p_declarator->p_flow_object->current.state = FLOW_OBJECT_STATE_NOT_ZERO;
                }
                else
                {
                    p_declarator->p_flow_object->current.state = FLOW_OBJECT_STATE_ZERO | FLOW_OBJECT_STATE_NOT_ZERO;
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
            flow_visit_direct_declarator(ctx, p_declarator->direct_declarator);
        }
    }
    catch
    {
    }
}

static void flow_visit_init_declarator_list(struct flow_visit_ctx* ctx, struct init_declarator_list* p_init_declarator_list)
{
    struct init_declarator* _Opt p_init_declarator = p_init_declarator_list->head;
    while (p_init_declarator)
    {
        flow_visit_init_declarator(ctx, p_init_declarator);
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
    struct member_declarator* _Opt p_member_declarator = p_member_declarator_list->head;
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
        flow_visit_member_declarator_list(ctx, p_member_declaration->member_declarator_list_opt);
    }
}

static void flow_visit_member_declaration_list(struct flow_visit_ctx* ctx, struct member_declaration_list* p_member_declaration_list)
{
    struct member_declaration* _Opt p_member_declaration = p_member_declaration_list->head;
    while (p_member_declaration)
    {
        flow_visit_member_declaration(ctx, p_member_declaration);
        p_member_declaration = p_member_declaration->next;
    }
}

static void flow_visit_struct_or_union_specifier(struct flow_visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier)
{
    flow_visit_member_declaration_list(ctx, &p_struct_or_union_specifier->member_declaration_list);
}

static void flow_visit_enumerator(struct flow_visit_ctx* ctx, struct enumerator* p_enumerator)
{
    struct true_false_set a = { 0 };
    if (p_enumerator->constant_expression_opt)
        flow_visit_expression(ctx, p_enumerator->constant_expression_opt, &a);
    true_false_set_destroy(&a);
}

static void flow_visit_enumerator_list(struct flow_visit_ctx* ctx, struct enumerator_list* p_enumerator_list)
{
    struct enumerator* _Opt current = p_enumerator_list->head;
    while (current)
    {
        flow_visit_enumerator(ctx, current);
        current = current->next;
    }
}

static void flow_visit_enum_specifier(struct flow_visit_ctx* ctx, struct enum_specifier* p_enum_specifier)
{
    flow_visit_enumerator_list(ctx, &p_enum_specifier->enumerator_list);
}

static void flow_visit_type_specifier(struct flow_visit_ctx* ctx, struct type_specifier* p_type_specifier)
{
    if (p_type_specifier->struct_or_union_specifier)
    {
        flow_visit_struct_or_union_specifier(ctx, p_type_specifier->struct_or_union_specifier);
    }

    if (p_type_specifier->enum_specifier)
    {
        flow_visit_enum_specifier(ctx, p_type_specifier->enum_specifier);
    }
}

static void flow_visit_type_specifier_qualifier(struct flow_visit_ctx* ctx, struct type_specifier_qualifier* p_type_specifier_qualifier)
{
    if (p_type_specifier_qualifier->type_specifier)
    {
        flow_visit_type_specifier(ctx, p_type_specifier_qualifier->type_specifier);
    }
}

static void flow_visit_declaration_specifier(struct flow_visit_ctx* ctx, struct declaration_specifier* p_declaration_specifier)
{
    if (p_declaration_specifier->type_specifier_qualifier)
    {
        flow_visit_type_specifier_qualifier(ctx, p_declaration_specifier->type_specifier_qualifier);
    }
}

static void flow_visit_declaration_specifiers(struct flow_visit_ctx* ctx,
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
        flow_visit_declaration_specifier(ctx, p_declaration_specifier);
        p_declaration_specifier = p_declaration_specifier->next;
    }
}

/*
* retorna true se o ultimo item for um return
*/
static bool flow_is_last_item_return(struct compound_statement* p_compound_statement)
{
#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignored "-Wflow-not-null"

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

#pragma CAKE diagnostic pop
}

void flow_visit_declaration(struct flow_visit_ctx* ctx, struct declaration* p_declaration)
{
    if (p_declaration->static_assert_declaration)
    {
        flow_visit_static_assert_declaration(ctx, p_declaration->static_assert_declaration);
    }

    if (p_declaration->pragma_declaration)
    {
        flow_visit_pragma_declaration(ctx, p_declaration->pragma_declaration);
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

    if (p_declaration->init_declarator_list.head)
    {
        flow_visit_init_declarator_list(ctx, &p_declaration->init_declarator_list);
    }

    if (p_declaration->function_body)
    {
        assert(ctx->p_return_type == NULL);

        struct type type = get_function_return_type(&p_declaration->init_declarator_list.head->p_declarator->type);
        ctx->p_return_type = &type;

        flow_visit_compound_statement(ctx, p_declaration->function_body);
        type_destroy(&type);
        ctx->p_return_type = NULL;
    }
}

void flow_start_visit_declaration(struct flow_visit_ctx* ctx, struct declaration* p_declaration)
{
    ctx->labels_size = 0;
    flow_objects_clear(&ctx->arena);

    ctx->state_number_generator = 1; //reserva 0 p current

    if (p_declaration->function_body)
    {
        flow_visit_declaration(ctx, p_declaration);
        assert(p_declaration->function_body != NULL); //flow_visit_declaration does not change this

        if (!flow_is_last_item_return(p_declaration->function_body))
        {
            flow_exit_block_visit_defer_list(ctx, &p_declaration->defer_list, p_declaration->function_body->last_token);
        }
    }
    else
    {
        flow_visit_declaration(ctx, p_declaration);
    }

    flow_objects_clear(&ctx->arena);
}

#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignored "-Wanalyzer-maybe-uninitialized" 

_Opt struct flow_object* _Opt arena_new_object(struct flow_visit_ctx* ctx)
{
    _Opt struct flow_object* _Owner _Opt p = calloc(1, sizeof * p);
    if (p != NULL)
    {
        p->current.dbg_name = "current";
        p->id = ctx->arena.size + 1;
        if (flow_objects_push_back(&ctx->arena, p) != 0)
        {
            p = NULL;
        }
    }
    return (struct flow_object* _Opt)p; //warning removed
}

#pragma CAKE diagnostic pop


void flow_visit_ctx_destroy(_Dtor struct flow_visit_ctx* p)
{
    flow_objects_destroy(&p->arena);
}


