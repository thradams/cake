#include "ownership.h"
#include "flow_object.h"
#include "parser.h"
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include "console.h"
#include "flow_visit.h"


struct object_visitor
{
    int member_index;
    struct type* p_type;
    struct flow_object* p_object;
};

unsigned int s_visit_number = 1; //creates a unique number

bool flow_object_is_not_null(struct flow_object* p)
{
    enum object_state e = p->current.state;
    return (
             !(e & OBJECT_STATE_NULL) &&
             ((e & OBJECT_STATE_NOT_NULL) || (e & OBJECT_STATE_MOVED))
           );

}

bool flow_object_is_null(struct flow_object* p)
{
    enum object_state e = p->current.state;
    return ((e & OBJECT_STATE_NULL) &&
           !(e & OBJECT_STATE_NOT_NULL) &&
           !(e & OBJECT_STATE_MOVED));
}

bool flow_object_is_zero(struct flow_object* p)
{
    enum object_state e = p->current.state;
    return ((e & OBJECT_STATE_ZERO) &&
            !(e & OBJECT_STATE_NOT_ZERO));
}

bool flow_object_is_not_zero(struct flow_object* p)
{
    enum object_state e = p->current.state;
    return (!(e & OBJECT_STATE_ZERO) &&
           (e & OBJECT_STATE_NOT_ZERO));
}

bool flow_object_maybe_is_null(struct flow_object* p)
{
    enum object_state e = p->current.state;

    return (e & OBJECT_STATE_NULL);
}

bool flow_object_is_uninitialized(struct flow_object* p)
{
    enum object_state e = p->current.state;
    return (e & OBJECT_STATE_UNINITIALIZED);
}


static void object_state_to_string_core(enum object_state e)
{
    bool first = true;

    printf("\"");
    if (e & OBJECT_STATE_UNINITIALIZED)
    {
        if (first)
            first = false;
        else
            printf(" | ");
        printf("uninitialized");
    }

    if (e & OBJECT_STATE_NOT_NULL)
    {
        if (first)
            first = false;
        else
            printf(" | ");
        printf("not-null");
    }

    if (e & OBJECT_STATE_NULL)
    {
        if (first)
            first = false;
        else
            printf(" | ");
        printf("null");
    }

    if (e & OBJECT_STATE_ZERO)
    {
        if (first)
            first = false;
        else
            printf(" | ");
        printf("zero");
    }

    if (e & OBJECT_STATE_NOT_ZERO)
    {
        if (first)
            first = false;
        else
            printf(" | ");
        printf("not-zero");
    }


    if (e & OBJECT_STATE_LIFE_TIME_ENDED)
    {
        if (first)
            first = false;
        else
            printf(" | ");
        printf("lifetime-ended");
    }

    if (e & OBJECT_STATE_MOVED)
    {
        if (first)
            first = false;
        else
            printf(" | ");
        printf("moved");
    }

    printf("\"");

}

void flow_object_state_delete(struct flow_object_state* owner opt p)
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

bool object_is_expansible(const struct flow_object* p_object)
{
    return (p_object &&
            p_object->current.state != OBJECT_STATE_NULL &&
                p_object->current.state != OBJECT_STATE_UNINITIALIZED &&
                p_object->current.pointed == 0);
}

void expand_pointer_object(struct flow_visit_ctx* ctx, struct type* p_type, struct flow_object* p_object)
{
    assert(type_is_pointer_or_array(p_type));

    if (object_is_expansible(p_object))
    {
        const bool nullable_enabled = ctx->ctx->options.null_checks_enabled;

        struct type t2 = type_remove_pointer(p_type);
        if (!type_is_void(&t2))
        {
            struct flow_object* p_object2 = make_object(ctx, &t2, p_object->p_declarator_origin, p_object->p_expression_origin);
            const bool is_nullable = type_is_nullable(&t2, nullable_enabled);

            object_set_unknown(&t2, is_nullable, p_object2, nullable_enabled);
            object_set_pointer(p_object, p_object2);////obj.pointed2 = p_object;
            object_push_states_from(p_object, p_object2);
        }
        type_destroy(&t2);
    }
}

void object_swap(struct flow_object* a, struct flow_object* b)
{
    struct flow_object temp = *a;
    *a = *b;
    *b = temp;
}

void object_delete(struct flow_object* owner opt p)
{
    if (p)
    {
        object_destroy(p);
        free(p);
    }
}
int objects_view_reserve(struct objects_view* p, int n);
void object_set_pointer(struct flow_object* p_object, struct flow_object* p_object2)
{
    p_object->current.pointed = p_object2;
}

void object_destroy(struct flow_object* obj_owner p)
{
    objects_view_destroy(&p->members);

    struct flow_object_state* owner p_flow_object_state = p->current.next;
    while (p_flow_object_state)
    {
        struct flow_object_state* owner temp = p_flow_object_state->next;
        p_flow_object_state->next = NULL;
        flow_object_state_delete(p_flow_object_state);
        p_flow_object_state = temp;
    }
    objects_view_destroy(&p->current.alternatives);
}

void flow_object_destroy(struct flow_object_state* obj_owner opt p)
{
    assert(p->next == NULL);
    objects_view_destroy(&p->alternatives);
}

void flow_object_update_current(struct flow_object* p)
{
    if (p->current.alternatives.size > 0)
    {
        p->current.state = 0;
        for (int i = 0; i < p->current.alternatives.size; i++)
        {
            //infinit recursin?
            flow_object_update_current(p->current.alternatives.data[i]);
            p->current.state |= p->current.alternatives.data[i]->current.state;
        }
    }
}

void flow_object_set_current_state_to_can_be_null(struct flow_object* p)
{
    p->current.state |= OBJECT_STATE_NULL;
}

void flow_object_set_current_state_to_is_null(struct flow_object* p)
{
    p->current.state = OBJECT_STATE_NULL;
    if (p->current.alternatives.size > 0)
    {
        for (int i = 0; i < p->current.alternatives.size; i++)
        {
            flow_object_set_current_state_to_can_be_null(p->current.alternatives.data[i]);
        }
    }
}

int flow_object_add_state(struct flow_object* p, struct flow_object_state* owner pnew)
{
    assert(pnew->next == NULL);
    pnew->next = p->current.next;
    p->current.next = pnew;
    return 0;
}





void objects_view_destroy(struct objects_view* obj_owner p)
{
    free(p->data);
}


int objects_view_reserve(struct objects_view* p, int n)
{
    if (n > p->capacity)
    {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0]))))
        {
            return EOVERFLOW;
        }

        void* owner pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL) return ENOMEM;

        static_set(p->data, "moved"); //p->data was moved to pnew

        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}


int objects_view_push_back(struct objects_view* p, struct flow_object* p_object)
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

void objects_view_clear(struct objects_view* p)
{
    p->size = 0;
}

void objects_view_merge(struct objects_view* dest, const struct objects_view* source)
{
    for (int i = 0; i < source->size; i++)
    {
        if (!objects_view_find(dest, source->data[i]))
        {
            objects_view_push_back(dest, source->data[i]);
        }
    }
}

void objects_view_copy(struct objects_view* dest, const struct objects_view* source)
{
    if (source == NULL)
    {
        objects_view_clear(dest);
        return;
    }
    objects_view_reserve(dest, source->size);
    for (int i = 0; i < source->size; i++)
    {
        dest->data[i] = source->data[i];
    }
    dest->size = source->size;
}

bool objects_view_find(const struct objects_view* p, const struct flow_object* p_object)
{
    for (int i = 0; i < p->size; i++)
    {
        if (p->data[i] == p_object)
            return true;
    }
    return false;
}
void objects_destroy(struct objects* obj_owner p) /*unchecked*/
{
    for (int i = 0; i < p->size; i++)
    {
        object_delete(p->data[i]);
    }
    free(p->data);
}

void objects_clear(struct objects* p)
{
    for (int i = 0; i < p->size; i++)
    {
        object_delete(p->data[i]);
    }
    p->size = 0;
}

int objects_reserve(struct objects* p, int n)
{
    if (n > p->capacity)
    {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0]))))
        {
            return EOVERFLOW;
        }

        void* owner pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL) return ENOMEM;

        static_set(p->data, "moved"); //p->data was moved to pnew

        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

const struct flow_object* objects_find(const struct objects* p_objects, const struct flow_object* object)
{    
    for (int i = 0; i < p_objects->size; i++)
    {
        if (p_objects->data[i] == object)
            return object;
    }
    return NULL;
}

int objects_push_back(struct objects* p, struct flow_object* owner p_object)
{
    if (p->size == INT_MAX)
    {
        object_delete(p_object);
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
            object_delete(p_object);
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
    struct object_name_list* previous;
};

bool has_name(const char* name, struct object_name_list* list)
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

struct flow_object* make_object_core(struct flow_visit_ctx* ctx,
    struct type* p_type,
    struct object_name_list* list,
    const struct declarator* p_declarator_opt,
    const struct expression* p_expression_origin)
{
    //assert((p_declarator_opt == NULL) != (p_expression_origin == NULL));
    if (p_declarator_opt == NULL)
    {
        assert(p_expression_origin != NULL);
    }
    if (p_expression_origin == NULL)
    {
        assert(p_declarator_opt != NULL);
    }


    struct flow_object* p_object = arena_new_object(ctx);



    p_object->p_expression_origin = p_expression_origin;
    p_object->p_declarator_origin = p_declarator_opt;

    if (p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier* p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier)
        {
            p_object->current.state = OBJECT_STATE_NOT_APPLICABLE;

            struct member_declaration* p_member_declaration =
                p_struct_or_union_specifier->member_declaration_list.head;

            struct object_name_list l = { 0 };
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
                                struct flow_object* member_obj = arena_new_object(ctx);
                                member_obj->parent = p_object;

                                member_obj->p_expression_origin = p_expression_origin;
                                member_obj->p_declarator_origin = p_declarator_opt;
                                member_obj->current.state = OBJECT_STATE_NOT_APPLICABLE;
                                objects_view_push_back(&p_object->members, member_obj);
                            }
                            else
                            {


                                struct flow_object* p_member_obj =
                                    make_object_core(ctx, &p_member_declarator->declarator->type,
                                        &l,
                                        p_declarator_opt,
                                        p_expression_origin);
                                p_member_obj->parent = p_object;
                                objects_view_push_back(&p_object->members, p_member_obj);
                            }

                            //member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else
                {
                    if (p_member_declaration->specifier_qualifier_list &&
                        p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
                    {
                        struct type t = { 0 };
                        t.category = TYPE_CATEGORY_ITSELF;
                        t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                        t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;
                        struct flow_object* member_obj = make_object_core(ctx, &t, &l, p_declarator_opt, p_expression_origin);
                        for (int k = 0; k < member_obj->members.size; k++)
                        {
                            objects_view_push_back(&p_object->members, member_obj->members.data[k]);
                            member_obj->members.data[k] = 0;
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


    p_object->current.state = OBJECT_STATE_UNINITIALIZED;

    return p_object;
}

struct flow_object* make_object(struct flow_visit_ctx* ctx,
    struct type* p_type,
                           const struct declarator* p_declarator_opt,
                           const struct expression* p_expression_origin)
{

    struct object_name_list list = { .name = "" };
    struct flow_object* p_object = make_object_core(ctx, p_type, &list, p_declarator_opt, p_expression_origin);

    return p_object;
}

struct token* object_get_token(const struct flow_object* object)
{
    if (object->p_declarator_origin)
    {
        return object->p_declarator_origin->name;
    }
    if (object->p_expression_origin)
    {
        if (object->p_expression_origin->first_token)
            return object->p_expression_origin->first_token;

        if (object->p_expression_origin->last_token)
            return object->p_expression_origin->last_token;

    }
    return NULL;
}

void flow_object_add_new_state_as_a_copy_of_current_state(struct flow_object* object, const char* name, int state_number)
{
    struct flow_object_state* owner pnew = calloc(1, sizeof * pnew);
    if (pnew == NULL) return;//ENOMEM;

    pnew->dbg_name = name;
    pnew->state_number = state_number;


    pnew->next = object->current.next;
    object->current.next = pnew;

    flow_object_state_copy(pnew, &object->current);

}

void object_remove_state(struct flow_object* object, int state_number)
{
    struct flow_object_state* previous = &object->current;
    struct flow_object_state* it = object->current.next;
    while (it)
    {
        if (it->state_number == state_number)
        {
            struct flow_object_state* owner p_it_next = it->next;
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
    if (p_visitor->p_object == NULL)
    {
        return;
    }
    if (p_visitor->p_object->visit_number == visit_number) return;
    p_visitor->p_object->visit_number = visit_number;

    if (p_visitor->p_type->struct_or_union_specifier)
    {
        assert(p_visitor->p_object->current.state == OBJECT_STATE_NOT_APPLICABLE);
        struct struct_or_union_specifier* p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier)
        {
            if (p_visitor->p_object == NULL)
            {
                printf("%*c", ident, ' ');
                printf("%s %s\n", previous_names, "-");
                return;
            }

            printf("%*c", ident + 1, ' ');
            printf("#%02d {\n", p_visitor->p_object->id);

            struct member_declaration* p_member_declaration =
                p_struct_or_union_specifier->member_declaration_list.head;


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

                            char buffer[200] = { 0 };
                            if (is_pointer)
                                snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                            else
                                snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);

                            struct object_visitor  visitor = { 0 };
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
                else
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
            else if (flow_object_is_uninitialized(p_visitor->p_object))
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

            struct flow_object_state* it = p_visitor->p_object->current.next;
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

            struct flow_object_state* it = p_visitor->p_object->current.next;
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


void object_merge_state(struct flow_object* pdest, struct flow_object* object1, struct flow_object* object2)
{
    pdest->current.state = object1->current.state | object2->current.state;

    if (pdest->members.size == object1->members.size &&
        object1->members.size == object2->members.size)
    {
        for (int i = 0; i < object1->members.size; i++)
        {
            struct flow_object* m1 = object1->members.data[i];
            struct flow_object* m2 = object2->members.data[i];
            object_merge_state(pdest->members.data[i], m1, m2);
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


    struct flow_object_state* it = object->current.next;
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

int object_restore_current_state_from(struct flow_object* object, int state_number)
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

    struct flow_object_state* it = object->current.next;
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
    if (object->current.state == OBJECT_STATE_NULL ||
        object->current.state == OBJECT_STATE_UNINITIALIZED) //moved
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



static void object_merge_current_state_with_state_number_or_core(struct flow_object* object, int state_number, unsigned int visit_number)
{
    if (object->visit_number == visit_number) return;
    object->visit_number = visit_number;

    struct flow_object_state* it = object->current.next;
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
        object_merge_current_state_with_state_number_or_core(object->members.data[i], state_number, visit_number);
    }
#if 0
    for (int i = 0; i < object->current.ref.size; i++)
    {
        struct flow_object* pointed = object->current.ref.data[i];

        if (pointed)
        {
            object_merge_current_state_with_state_number_or_core(pointed, state_number, visit_number);
        }
    }
#endif
}


void object_get_name(const struct type* p_type,
    const struct flow_object* p_object,
    char* outname,
    int out_size);


void print_object(struct type* p_type, struct flow_object* p_object, bool short_version)
{
    if (p_object == NULL)
    {
        printf("null object");
        return;
    }
    char name[100] = { 0 };
    object_get_name(p_type, p_object, name, sizeof name);



    struct object_visitor visitor = { 0 };
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    print_object_core(0, &visitor, name, type_is_pointer(p_type), short_version, s_visit_number++);
}


void object_set_uninitialized_core(struct object_visitor* p_visitor)
{
    if (p_visitor->p_object == NULL || p_visitor->p_type == NULL)
    {
        return;
    }

    if (p_visitor->p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier* p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier)
        {
            struct member_declaration* p_member_declaration =
                p_struct_or_union_specifier->member_declaration_list.head;


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
                            if (p_visitor->member_index < p_visitor->p_object->members.size)
                            {
                                struct object_visitor visitor = { 0 };
                                visitor.p_type = &p_member_declarator->declarator->type;
                                visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];
                                object_set_uninitialized_core(&visitor);
                            }
                            p_visitor->member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else
                {
                    if (p_member_declaration->specifier_qualifier_list &&
                       p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
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
        p_visitor->p_object->current.state = OBJECT_STATE_UNINITIALIZED;

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
        p_visitor->p_object->current.state = OBJECT_STATE_UNINITIALIZED;
    }
}

void object_set_uninitialized(struct type* p_type, struct flow_object* p_object)
{
    struct object_visitor visitor = { 0 };
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    object_set_uninitialized_core(&visitor);
}

static void checked_empty_core(struct flow_visit_ctx* ctx,
    struct type* p_type,
    struct flow_object* p_object,
    const char* previous_names,
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
                        const char* name = p_member_declarator->declarator->name ? p_member_declarator->declarator->name->lexeme : "";
                        char buffer[200] = { 0 };
                        if (type_is_pointer(p_type))
                            snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                        else
                            snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);


                        checked_empty_core(ctx, &p_member_declarator->declarator->type,
                            p_object->members.data[member_index],
                            name,
                            position_token);

                        member_index++;
                    }
                    p_member_declarator = p_member_declarator->next;
                }
            }
            p_member_declaration = p_member_declaration->next;
        }
        return;
    }

    if (type_is_any_owner(p_type))
    {
        if (p_object->current.state == (OBJECT_STATE_UNINITIALIZED | OBJECT_STATE_NULL | OBJECT_STATE_MOVED) ||
            p_object->current.state == (OBJECT_STATE_NULL | OBJECT_STATE_MOVED) ||
            p_object->current.state == OBJECT_STATE_NULL ||
            p_object->current.state == OBJECT_STATE_MOVED ||
            p_object->current.state == OBJECT_STATE_UNINITIALIZED ||
            p_object->current.state == (OBJECT_STATE_UNINITIALIZED | OBJECT_STATE_NULL) ||
            p_object->current.state == (OBJECT_STATE_UNINITIALIZED | OBJECT_STATE_MOVED)
            )
        {
        }
        else
        {

            compiler_diagnostic_message(W_FLOW_MISSING_DTOR,
                ctx->ctx,
                position_token,
                "object '%s' may be not empty",
                previous_names);
        }
    }
}

void checked_empty(struct flow_visit_ctx* ctx,
    struct type* p_type,
    struct flow_object* p_object,
    const struct token* position_token)
{
    char name[100] = { 0 };
    object_get_name(p_type, p_object, name, sizeof name);
    checked_empty_core(ctx,
    p_type,
    p_object,
        name,
    position_token);

}

static void object_set_moved_core(struct object_visitor* p_visitor)
{
    if (p_visitor->p_object == NULL || p_visitor->p_type == NULL)
    {
        return;
    }

    if (p_visitor->p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier* p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier)
        {
            struct member_declaration* p_member_declaration =
                p_struct_or_union_specifier->member_declaration_list.head;


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
                            if (p_visitor->member_index < p_visitor->p_object->members.size)
                            {
                                struct object_visitor visitor = { 0 };
                                visitor.p_type = &p_member_declarator->declarator->type;
                                visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];
                                object_set_moved_core(&visitor);
                            }
                            p_visitor->member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else
                {
                    if (p_member_declaration->specifier_qualifier_list &&
                       p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
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
        p_visitor->p_object->current.state = OBJECT_STATE_MOVED;
    }
    else
    {
        p_visitor->p_object->current.state = OBJECT_STATE_MOVED;
    }
}

void object_set_moved(struct type* p_type, struct flow_object* p_object)
{
    struct object_visitor visitor = { 0 };
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    object_set_moved_core(&visitor);
}

static void object_set_unknown_core(struct object_visitor* p_visitor, bool t_is_nullable, unsigned int visit_number, bool nullable_enabled)
{
    if (p_visitor->p_object == NULL || p_visitor->p_type == NULL)
    {
        return;
    }

    if (p_visitor->p_object->visit_number == visit_number) return; //already visited
    p_visitor->p_object->visit_number = visit_number;


    if (p_visitor->p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier* p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier)
        {
            struct member_declaration* p_member_declaration =
                p_struct_or_union_specifier->member_declaration_list.head;


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
                            if (p_visitor->member_index < p_visitor->p_object->members.size)
                            {
                                struct object_visitor visitor = { 0 };
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
                else
                {
                    if (p_member_declaration->specifier_qualifier_list &&
                        p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
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
        if (t_is_nullable || type_is_nullable(p_visitor->p_type, nullable_enabled))
            p_visitor->p_object->current.state = OBJECT_STATE_NULL | OBJECT_STATE_NOT_NULL;
        else
            p_visitor->p_object->current.state = OBJECT_STATE_NOT_NULL;


        struct flow_object* pointed = p_visitor->p_object->current.pointed;
        if (pointed)
        {
            struct type t2 = type_remove_pointer(p_visitor->p_type);
            bool t2_is_nullable = type_is_nullable(&t2, nullable_enabled);
            struct object_visitor visitor = { 0 };
            visitor.p_type = &t2;
            visitor.p_object = pointed;

            object_set_unknown_core(&visitor, t2_is_nullable, visit_number, nullable_enabled);
            type_destroy(&t2);
        }

    }
    else
    {
        if (!type_is_struct_or_union(p_visitor->p_type))
            p_visitor->p_object->current.state = OBJECT_STATE_ZERO | OBJECT_STATE_NOT_ZERO;
    }
}

void object_set_unknown(struct type* p_type, bool t_is_nullable, struct flow_object* p_object, bool nullable_enabled)
{
    struct object_visitor visitor = { 0 };
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    object_set_unknown_core(&visitor, t_is_nullable, s_visit_number++, nullable_enabled);
}


static void object_set_deleted_core(struct type* p_type, struct flow_object* p_object, unsigned int visit_number)
{
    if (p_object == NULL || p_type == NULL)
    {
        return;
    }

    if (p_object->visit_number == visit_number) return; //already visited
    p_object->visit_number = visit_number;


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
                p_member_declaration = p_member_declaration->next;
            }
            return;
        }
        return;
    }

    if (type_is_pointer(p_type))
    {
        p_object->current.state = OBJECT_STATE_LIFE_TIME_ENDED;
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
            p_object->current.state = OBJECT_STATE_LIFE_TIME_ENDED;
    }
}

void flow_object_set_can_be_uninitialized(struct flow_object* p_object)
{
    p_object->current.state |= OBJECT_STATE_UNINITIALIZED;
}
void flow_object_set_is_unitialized(struct flow_object* p_object)
{
    p_object->current.state = OBJECT_STATE_UNINITIALIZED;
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
    if (p_object->current.state & OBJECT_STATE_NOT_NULL)
    {
        p_object->current.state &= ~OBJECT_STATE_NOT_NULL;
        p_object->current.state |= OBJECT_STATE_MOVED;
        p_object->current.state |= OBJECT_STATE_MOVED;
    }
}
void flow_object_set_is_moved(struct flow_object* p_object)
{
    p_object->current.state = OBJECT_STATE_MOVED;

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
    if (p_visitor->p_object == NULL || p_visitor->p_type == NULL)
    {
        return;
    }

    if (p_visitor->p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier* p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier)
        {
            struct member_declaration* p_member_declaration = p_struct_or_union_specifier->member_declaration_list.head;

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
                            if (p_visitor->member_index < p_visitor->p_object->members.size)
                            {
                                struct object_visitor visitor = { 0 };
                                visitor.p_type = &p_member_declarator->declarator->type;
                                visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];
                                object_set_zero_core(&visitor);
                            }
                            p_visitor->member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
                    }
                }
                else
                {
                    if (p_member_declaration->specifier_qualifier_list &&
                        p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
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
        p_visitor->p_object->current.state = OBJECT_STATE_NULL;
    }
    else
    {
        p_visitor->p_object->current.state = OBJECT_STATE_ZERO;
    }
}

void object_set_zero(struct type* p_type, struct flow_object* p_object)
{
    struct object_visitor visitor = { 0 };
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    object_set_zero_core(&visitor);
}

//returns true if all parts that need to be moved weren't moved.
bool object_check(struct type* p_type, struct flow_object* p_object)
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
                            p_object->members.data[member_index]))
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
            should_had_been_moved = (p_object->current.state & OBJECT_STATE_NOT_NULL);
        }
        else
        {
            if (p_object->current.state == OBJECT_STATE_UNINITIALIZED ||
                (p_object->current.state & OBJECT_STATE_MOVED) ||
                p_object->current.state == OBJECT_STATE_NOT_NULL ||
                p_object->current.state == (OBJECT_STATE_UNINITIALIZED))
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
    const struct flow_object* p_object,
    const struct flow_object* p_object_target,
    const char* previous_names,
    char* outname,
    int out_size,
    unsigned int visit_number)
{
    if (p_object == NULL)
    {
        return;
    }
    if (p_object->visit_number == visit_number) return;
    ((struct flow_object*)p_object)->visit_number = visit_number;

    if (p_object == p_object_target)
    {
        snprintf(outname, out_size, "%s", previous_names);
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
            p_member_declaration = p_member_declaration->next;
        }

    }
    else
    {
        if (type_is_pointer(p_type))
        {
            char buffer[100] = { 0 };
            snprintf(buffer, sizeof buffer, "%s", previous_names);

            struct type t2 = type_remove_pointer(p_type);
            if (type_is_owner(&t2))
            {
#if 0
                if (p_object->current.ref.size > 0)
                {
                    object_get_name_core(
                        &t2,
                        p_object->current.ref.data[0],
                        p_object_target,
                        buffer,
                        outname,
                        out_size,
                        visit_number);
                }
#endif
            }
            type_destroy(&t2);
        }
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

        const char* root_name = p_object->p_declarator_origin->name ? p_object->p_declarator_origin->name->lexeme : "?";
        //snprintf(outname, out_size, "%s",root_name);

        const struct flow_object* root = p_object->p_declarator_origin->p_object;

        object_get_name_core(&p_object->p_declarator_origin->type, root, p_object, root_name, outname, out_size, s_visit_number++);
    }
    else if (p_object->p_expression_origin)
    {
        int bytes_written = 0;
        struct token* p = p_object->p_expression_origin->first_token;
        for (int i = 0; i < 10; i++)
        {
            const char* ps = p->lexeme;
            while (ps && *ps)
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
        }
        outname[bytes_written] = '\0';
    }
    else
    {
        outname[0] = '?';
        outname[1] = '\0';
    }
}

void checked_moved_core(struct flow_visit_ctx* ctx,
    struct type* p_type,
    struct flow_object* p_object,
    const struct token* position_token,
    unsigned int visit_number)
{
    if (p_object == NULL)
    {
        return;
    }

    if (p_object->visit_number == visit_number) return;//already visited    
    p_object->visit_number = visit_number;

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
                        checked_moved_core(ctx, &p_member_declarator->declarator->type, p_object->members.data[member_index], position_token, visit_number);
                        member_index++;
                    }

                    p_member_declarator = p_member_declarator->next;
                }
            }
            p_member_declaration = p_member_declaration->next;
        }
        return;
    }
    else
    {
        if (type_is_pointer(p_type) && !type_is_any_owner(p_type))
        {
            if (p_object->current.state != OBJECT_STATE_UNINITIALIZED &&
                p_object->current.state != OBJECT_STATE_NULL)
            {
                struct type t2 = type_remove_pointer(p_type);
#if 0
                for (int i = 0; i < p_object->current.ref.size; i++)
                {
                    checked_moved_core(ctx,
                        &t2,
                        p_object->current.ref.data[i],
                        position_token,
                        visit_number);
                }
#endif
                type_destroy(&t2);
            }
        }

        if (p_object->current.state & OBJECT_STATE_MOVED)
        {
            struct token* name_pos = object_get_token(p_object);
            const char* parameter_name = name_pos ? name_pos->lexeme : "?";


            char name[200] = { 0 };
            object_get_name(p_type, p_object, name, sizeof name);
            if (compiler_diagnostic_message(W_FLOW_MISSING_DTOR,
                ctx->ctx,
                position_token,
                "parameter '%s' is leaving scoped with a moved object '%s'",
                parameter_name,
                name))
            {
                compiler_diagnostic_message(W_LOCATION, ctx->ctx, name_pos, "parameter", name);
            }
        }

        if (p_object->current.state & OBJECT_STATE_UNINITIALIZED)
        {
            struct token* name_pos = object_get_token(p_object);
            const char* parameter_name = name_pos ? name_pos->lexeme : "?";

            char name[200] = { 0 };
            object_get_name(p_type, p_object, name, sizeof name);
            if (compiler_diagnostic_message(W_FLOW_MISSING_DTOR,
                ctx->ctx,
                position_token,
                "parameter '%s' is leaving scoped with a uninitialized object '%s'",
                parameter_name,
                name))
            {
                compiler_diagnostic_message(W_LOCATION, ctx->ctx, name_pos, "parameter", name);
            }
        }
    }
}

void checked_moved(struct flow_visit_ctx* ctx,
    struct type* p_type,
    struct flow_object* p_object,
    const struct token* position_token)
{
    checked_moved_core(ctx,
    p_type,
    p_object,
    position_token,
    s_visit_number++);
}

void checked_read_object_core(struct flow_visit_ctx* ctx,
    struct object_visitor* p_visitor,
    bool is_nullable,
    const struct token* position_token,
    bool check_pointed_object,
    const char* previous_names,
    unsigned int visit_number)
{
    if (p_visitor->p_object == NULL)
    {
        return;
    }

    if (p_visitor->p_object->visit_number == visit_number) return;
    p_visitor->p_object->visit_number = visit_number;


    if (p_visitor->p_type->struct_or_union_specifier && p_visitor->p_object->members.size > 0)
    {
        struct struct_or_union_specifier* p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);

        struct member_declaration* p_member_declaration =
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
                struct member_declarator* p_member_declarator =
                    p_member_declaration->member_declarator_list_opt->head;
                while (p_member_declarator)
                {

                    if (p_member_declarator->declarator)
                    {
                        const char* name =
                            p_member_declarator->declarator->name ? p_member_declarator->declarator->name->lexeme : "?";

                        char buffer[200] = { 0 };
                        if (type_is_pointer(p_visitor->p_type))
                            snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                        else
                            snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);

                        struct object_visitor  visitor = { 0 };
                        visitor.p_type = &p_member_declarator->declarator->type;
                        visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];


                        checked_read_object_core(ctx,
                            &visitor,
                            is_nullable,
                            position_token,
                            check_pointed_object,
                            buffer,
                            visit_number);

                        p_visitor->member_index++;
                    }
                    p_member_declarator = p_member_declarator->next;
                }
            }
            else
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
                        position_token,
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
            !type_is_nullable(p_visitor->p_type, ctx->ctx->options.null_checks_enabled) &&
            flow_object_maybe_is_null(p_visitor->p_object))
        {
            compiler_diagnostic_message(W_FLOW_NULL_DEREFERENCE,
                ctx->ctx,
                position_token,
                "non-nullable pointer '%s' may be null",
                previous_names);
        }

        if (type_is_pointer(p_visitor->p_type) &&
            check_pointed_object &&
            p_visitor->p_object->current.state & OBJECT_STATE_NOT_NULL /*we don't need to check pointed object*/
            )
        {
            struct type t2 = type_remove_pointer(p_visitor->p_type);

            if (p_visitor->p_object->current.pointed)
            {
                struct object_visitor  visitor = { 0 };
                visitor.p_type = &t2;
                visitor.p_object = p_visitor->p_object->current.pointed;

                checked_read_object_core(ctx,
                    &visitor,
                    is_nullable,
                    position_token,
                    true,
                    previous_names,
                    visit_number);
            }

            type_destroy(&t2);
        }


        if (p_visitor->p_object->current.state & OBJECT_STATE_UNINITIALIZED)
        {
            compiler_diagnostic_message(W_FLOW_UNINITIALIZED,
                ctx->ctx,
                position_token,
                "uninitialized object '%s'",
                previous_names);
        }

#if 0
        //TODO there is some problem with  OBJECT_STATE_LIFE_TIME_ENDED
        //state somewhere!
        if (p_object->current.state & OBJECT_STATE_LIFE_TIME_ENDED)
        {
            compiler_diagnostic_message(W_FLOW_UNINITIALIZED,
                ctx->ctx,
                position_token,
                "lifetime ended '%s'",
                previous_names);
        }
#endif

    }
}

/*
  Check if all objects connect to this one
   - are initialized
*/
void checked_read_object(struct flow_visit_ctx* ctx,
    struct type* p_type,
    bool is_nullable,
    struct flow_object* p_object,
    const struct token* position_token,
    bool check_pointed_object)
{
    const char* owner s = NULL;
    const char* name = "";
    if (p_object->p_declarator_origin)
        name = p_object->p_declarator_origin->name ? p_object->p_declarator_origin->name->lexeme : "?";
    else if (p_object->p_expression_origin)
    {
        if (p_object->p_expression_origin->first_token &&
            p_object->p_expression_origin->last_token)
        {
            view struct token_list list = { .head = p_object->p_expression_origin->first_token,
                                            .tail = p_object->p_expression_origin->last_token
            };
            s = get_code_as_we_see(&list, true);
            name = s;
        }
        else
        {
            //assert(false);
        }
    }

    struct object_visitor visitor = { 0 };
    visitor.p_object = p_object;
    visitor.p_type = p_type;

    checked_read_object_core(ctx,
    &visitor,
    is_nullable,
    position_token,
    check_pointed_object,
    name,
    s_visit_number++);
    free((void* owner)s);
}


static void end_of_storage_visit_core(struct flow_visit_ctx* ctx,
    struct object_visitor* p_visitor,
    bool b_type_is_view,
    const struct token* position_token,
    const char* previous_names,
    unsigned int visit_number)
{
    if (p_visitor->p_object == NULL)
    {
        return;
    }

    if (p_visitor->p_object->visit_number == visit_number) return;
    p_visitor->p_object->visit_number = visit_number;


    if (p_visitor->p_type->struct_or_union_specifier && p_visitor->p_object->members.size > 0)
    {
        struct struct_or_union_specifier* p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_visitor->p_type->struct_or_union_specifier);

        struct member_declaration* p_member_declaration =
            p_struct_or_union_specifier->member_declaration_list.head;

        if (object_check(p_visitor->p_type, p_visitor->p_object))
        {
            /*
            *  All parts of the object needs to be moved, so instead of
            *  describing each part we will just say that the object should
            *  have been moved.
            */
            const struct token* name = object_get_token(p_visitor->p_object);
            if (compiler_diagnostic_message(W_FLOW_MISSING_DTOR,
                ctx->ctx,
                name,
                "object '%s' was not moved/destroyed",
                previous_names))
            {

                if (p_visitor->p_object->p_declarator_origin)
                    compiler_diagnostic_message(W_LOCATION, ctx->ctx, position_token, "end of '%s' scope", previous_names);
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
                    struct member_declarator* p_member_declarator =
                        p_member_declaration->member_declarator_list_opt->head;
                    while (p_member_declarator)
                    {

                        if (p_member_declarator->declarator)
                        {
                            const char* name = p_member_declarator->declarator->name ? p_member_declarator->declarator->name->lexeme : "?";

                            char buffer[200] = { 0 };
                            if (type_is_pointer(p_visitor->p_type))
                                snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                            else
                                snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);

                            const bool member_is_view = type_is_view(&p_member_declarator->declarator->type);

                            struct object_visitor  visitor = { 0 };
                            visitor.p_type = &p_member_declarator->declarator->type;
                            visitor.p_object = p_visitor->p_object->members.data[p_visitor->member_index];

                            end_of_storage_visit_core(ctx,
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
                else
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
                    end_of_storage_visit_core(ctx,
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
        const struct token* position = NULL;
        if (p_visitor->p_object->p_declarator_origin)
            position = p_visitor->p_object->p_declarator_origin->name ? p_visitor->p_object->p_declarator_origin->name : p_visitor->p_object->p_declarator_origin->first_token;
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
           Despite the name OBJECT_STATE_NOT_NULL does not means null, it means
           the reference is not referring an object, the value could be -1 for instance.
        */
        if (type_is_pointer(p_visitor->p_type) &&
            !b_type_is_view &&
            type_is_owner(p_visitor->p_type) &&
            p_visitor->p_object->current.state & OBJECT_STATE_NOT_NULL)
        {
            if (compiler_diagnostic_message(W_FLOW_MISSING_DTOR,
                ctx->ctx,
                position,
                "ownership of '%s' not moved before the end of lifetime", previous_names))
            {
                compiler_diagnostic_message(W_LOCATION,
                ctx->ctx,
                position_token,
                "end of lifetime");
            }
        }
        else if (!b_type_is_view && type_is_obj_owner(p_visitor->p_type) && type_is_pointer(p_visitor->p_type))
        {
            char buffer[100] = { 0 };
            snprintf(buffer, sizeof buffer, "%s", previous_names);
            struct type t2 = type_remove_pointer(p_visitor->p_type);

            if (p_visitor->p_object->current.pointed)
            {
                struct object_visitor visitor = { 0 };
                visitor.p_type = &t2;
                visitor.p_object = p_visitor->p_object->current.pointed;
                end_of_storage_visit_core(ctx, &visitor, b_type_is_view, position, buffer, visit_number);
            }


            type_destroy(&t2);
        }
        else if (type_is_owner(p_visitor->p_type) && !type_is_pointer(p_visitor->p_type))
        {
            //non-pointer owner
            if (p_visitor->p_object->current.state == OBJECT_STATE_UNINITIALIZED ||
                p_visitor->p_object->current.state == OBJECT_STATE_NULL ||
                p_visitor->p_object->current.state == OBJECT_STATE_MOVED)
            {
            }
            else
            {
                if (compiler_diagnostic_message(W_FLOW_MISSING_DTOR,
                    ctx->ctx,
                    position,
                    "ownership of '%s' not moved before the end of lifetime", previous_names))
                {
                    compiler_diagnostic_message(W_LOCATION,
                    ctx->ctx,
                    position_token,
                    "end of lifetime");
                }
            }
        }
        else
        {
        }

        p_visitor->p_object->current.state = OBJECT_STATE_LIFE_TIME_ENDED;
    }
}

void end_of_storage_visit(struct flow_visit_ctx* ctx,
    struct type* p_type,
    bool type_is_view,
    struct flow_object* p_object,
    const struct token* position_token,
    const char* previous_names)
{
    struct object_visitor visitor = { 0 };
    visitor.p_type = p_type;
    visitor.p_object = p_object;
    end_of_storage_visit_core(ctx,
    &visitor,
    type_is_view,
    position_token,
    previous_names,
    s_visit_number++);
}


bool object_is_zero_or_null(const struct flow_object* p_object)
{
    return (p_object->current.state == OBJECT_STATE_NULL) ||
        (p_object->current.state == OBJECT_STATE_ZERO);
}


/*
   This function must check and do the flow assignment of
   a = b
*/
static void flow_assignment_core(
    struct flow_visit_ctx* ctx,
    const struct token* error_position,
    enum assigment_type assigment_type,
    bool check_uninitialized_b,
    bool a_type_is_view,
    bool a_type_is_nullable,
    struct object_visitor* p_visitor_a,
    struct object_visitor* p_visitor_b)
{
    if (p_visitor_a->p_object == NULL || p_visitor_b->p_object == NULL)
    {
        return;
    }
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
    if (check_uninitialized_b && p_visitor_b->p_object->current.state & OBJECT_STATE_UNINITIALIZED)
    {
        //a = b where b is uninitialized
        char buffer[100] = { 0 };
        object_get_name(p_visitor_b->p_type, p_visitor_b->p_object, buffer, sizeof buffer);
        if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
        {
            if (!type_is_out(p_visitor_a->p_type))
            {
                compiler_diagnostic_message(W_FLOW_UNINITIALIZED,
                            ctx->ctx,
                            error_position,
                            "passing an uninitialized argument '%s' object", buffer);
            }
        }
        else if (assigment_type == ASSIGMENT_TYPE_RETURN)
        {
            compiler_diagnostic_message(W_FLOW_UNINITIALIZED,
                        ctx->ctx,
                        error_position,
                        "returning an uninitialized '%s' object", buffer);
        }
        else
        {
            compiler_diagnostic_message(W_FLOW_UNINITIALIZED,
                        ctx->ctx,
                        error_position,
                        "reading an uninitialized '%s' object", buffer);
        }

        return;
    }

    if (check_uninitialized_b && p_visitor_a->p_object->current.state & OBJECT_STATE_LIFE_TIME_ENDED)
    {
        //a = b where a was deleted
        char buffer[100] = { 0 };
        object_get_name(p_visitor_a->p_type, p_visitor_a->p_object, buffer, sizeof buffer);

        compiler_diagnostic_message(W_FLOW_LIFETIME_ENDED,
                    ctx->ctx,
                    error_position,
                    "The object '%s' may have been deleted or its lifetime have ended.", buffer);


        return;
    }

    /*general check passing possible null to non opt*/
    if (type_is_pointer(p_visitor_a->p_type) &&
        (!type_is_nullable(p_visitor_a->p_type, ctx->ctx->options.null_checks_enabled)) &&
        p_visitor_b->p_object->current.state & OBJECT_STATE_NULL)
    {
        if (!a_type_is_nullable)
        {
            char buffer[100] = { 0 };
            object_get_name(p_visitor_b->p_type, p_visitor_b->p_object, buffer, sizeof buffer);

            compiler_diagnostic_message(W_FLOW_NULLABLE_TO_NON_NULLABLE,
                       ctx->ctx,
                       error_position,
                       "assignment of possible null object '%s' to non-nullable pointer", buffer);
        }

    }

    if (type_is_pointer(p_visitor_a->p_type))
    {
        if (!a_type_is_view && type_is_owner(p_visitor_a->p_type))
        {
            checked_empty(ctx, p_visitor_a->p_type, p_visitor_a->p_object, error_position);
        }

        if (object_is_zero_or_null(p_visitor_b->p_object))
        {
            if (type_is_array(p_visitor_b->p_type))
            {
                //int b[2] = {0};
                //int * a = b;
                //object_set_zero(p_a_type, p_a_object);
#if 0
                objects_view_clear(&p_visitor_a->p_object->current.ref);
#endif
                p_visitor_a->p_object->current.state = OBJECT_STATE_NOT_NULL;
                return;
            }
            else if (type_is_nullptr_t(p_visitor_b->p_type) || type_is_integer(p_visitor_b->p_type))
            {
                //a = nullpr
                //object_set_zero(p_a_type, p_a_object);
                //p_a_object->pointed_ref = NULL;
#if 0
                objects_view_clear(&p_visitor_a->p_object->current.ref);
#endif
                flow_object_set_current_state_to_is_null(p_visitor_a->p_object);

                return;
            }
        }
    }

    if (!a_type_is_view && type_is_obj_owner(p_visitor_a->p_type) && type_is_pointer(p_visitor_a->p_type))
    {
        checked_empty(ctx, p_visitor_a->p_type, p_visitor_a->p_object, error_position);

        if (object_is_zero_or_null(p_visitor_b->p_object))
        {
            //0 to objec_owner??
            //a = nullpr
            //object_set_zero(p_a_type, p_a_object);
            return;
        }
    }

    /*copying to void * owner*/
    if (type_is_void_ptr(p_visitor_a->p_type) && type_is_pointer(p_visitor_b->p_type))
    {
        p_visitor_a->p_object->current.state = p_visitor_b->p_object->current.state;
#if 0
        objects_view_copy(&p_visitor_a->p_object->current.ref, &p_visitor_b->p_object->current.ref);
#endif

        if (!a_type_is_view && type_is_owner(p_visitor_a->p_type))
        {
            //*b must be empty before copying to void* owner
            struct type t = type_remove_pointer(p_visitor_b->p_type);


            if (p_visitor_b->p_object->current.pointed == NULL)
            {
                // The question is..if we had this object expanded
                // could it possible have resources?
                //-> {...}
                if (object_is_expansible(p_visitor_b->p_object) &&
                    type_is_owner(&t))
                {
                    //if the anwser is yes then we need a warning
                    compiler_diagnostic_message(W_FLOW_MISSING_DTOR,
                                                ctx->ctx,
                                                error_position,
                                                "pointed object may be not empty");
                }
            }
            else
            {
                checked_empty(ctx, &t, p_visitor_b->p_object->current.pointed, error_position);
                object_set_deleted(&t, p_visitor_b->p_object->current.pointed);
            }

            type_destroy(&t);

            if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
            {
                flow_object_set_is_unitialized(p_visitor_b->p_object);
                p_visitor_b->p_object->current.state = OBJECT_STATE_UNINITIALIZED;
            }
            else
            {
                flow_object_set_is_moved(p_visitor_b->p_object);
                //p_visitor_b->p_object->current.state &= ~OBJECT_STATE_NOT_NULL;
                //p_visitor_b->p_object->current.state |= OBJECT_STATE_MOVED;
            }

        }


        return;
    }



    if (type_is_pointer(p_visitor_a->p_type) && type_is_pointer(p_visitor_b->p_type))
    {


        p_visitor_a->p_object->current.state = p_visitor_b->p_object->current.state;
        p_visitor_a->p_object->current.pointed = p_visitor_b->p_object->current.pointed;

#if 0
        objects_view_copy(&p_visitor_a->p_object->current.ref, &p_visitor_b->p_object->current.ref);
#endif
        struct type t = type_remove_pointer(p_visitor_a->p_type);

        /*if the parameter points to out object, then we don´t need to check
          argument pointed object.
        */
        const bool checked_pointed_object_read = !type_is_out(&t);
        bool is_nullable = a_type_is_nullable || type_is_nullable(&t, ctx->ctx->options.null_checks_enabled);
        checked_read_object(ctx, p_visitor_b->p_type, is_nullable, p_visitor_b->p_object, error_position, checked_pointed_object_read);


        //object_copy_state(p_a_type, p_a_object, p_b_type, p_b_object);
        type_destroy(&t);

        if (!a_type_is_view && type_is_owner(p_visitor_a->p_type))
        {
            /*
               T * _Owner pA = pB;
            */

            if (p_visitor_b->p_object->current.state & OBJECT_STATE_MOVED)
            {
                //TODO we need 2 positions, source, dest
                compiler_diagnostic_message(W_FLOW_MOVED,
                   ctx->ctx,
                   error_position,
                   "source object has already been moved");
            }


            if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
            {
                p_visitor_b->p_object->current.state = OBJECT_STATE_UNINITIALIZED;


                if (p_visitor_b->p_object->current.pointed)
                {
                    struct flow_object* pointed = p_visitor_b->p_object->current.pointed;
                    if (pointed)
                    {

                        struct type t2 = type_remove_pointer(p_visitor_b->p_type);
                        object_set_deleted(&t2, pointed);
                        type_destroy(&t2);
                    }
                }


                //p_b_object->pointed_ref = NULL;

                //object_set_uninitialized()
            }
            else
            {

                //TODO
                p_visitor_a->p_object->current.state = p_visitor_a->p_object->current.state & ~OBJECT_STATE_MOVED;
                if (p_visitor_b->p_object->current.state & OBJECT_STATE_NOT_NULL)
                {
                    //null not-null -> null moved
                    p_visitor_b->p_object->current.state &= ~OBJECT_STATE_NOT_NULL;
                    p_visitor_b->p_object->current.state |= OBJECT_STATE_MOVED;
                }
            }
        }
        else if (!a_type_is_view && type_is_obj_owner(p_visitor_a->p_type))
        {
            if (type_is_any_owner(p_visitor_b->p_type))
            {
                if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
                {

                    //p_b_object->state = OBJECT_STATE_UNINITIALIZED;
                    if (p_visitor_b->p_object->current.pointed)
                    {
                        struct flow_object* pointed = p_visitor_b->p_object->current.pointed;
                        if (pointed)
                        {
                            struct type t2 = type_remove_pointer(p_visitor_b->p_type);
                            object_set_uninitialized(&t2, pointed);
                            type_destroy(&t2);
                        }
                    }

                }
                else
                    object_set_moved(p_visitor_b->p_type, p_visitor_b->p_object);
            }
            else
            {
                if (p_visitor_b->p_type->address_of)
                {
                    //must be address of.
                    if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
                    {
                        p_visitor_b->p_object->current.state = OBJECT_STATE_UNINITIALIZED;

                        if (p_visitor_b->p_object->current.pointed)
                        {
                            struct flow_object* pointed = p_visitor_b->p_object->current.pointed;
                            if (pointed)
                            {
                                struct type t2 = type_remove_pointer(p_visitor_b->p_type);
                                object_set_uninitialized(&t2, pointed);
                                type_destroy(&t2);
                            }
                        }


                    }
                    else
                        object_set_moved(p_visitor_b->p_type, p_visitor_b->p_object);
                }
                else
                {
                    //avoid error on top of error
                    //address error already emmited
                    //at this point
                }
            }

        }
        else
        {
            if (a_type_is_view || !type_is_owner(p_visitor_a->p_type))
            {

                p_visitor_a->p_object->current.state = p_visitor_b->p_object->current.state;

                p_visitor_a->p_object->current.state &= ~OBJECT_STATE_MOVED;
            }

            if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
            {
                struct type t3 = type_remove_pointer(p_visitor_a->p_type);
                if (!type_is_const(&t3))
                {

                    if (p_visitor_b->p_object->current.pointed)
                    {
                        struct flow_object* pointed = p_visitor_b->p_object->current.pointed;
                        if (pointed)
                        {

                            bool nullable_enabled = ctx->ctx->options.null_checks_enabled;
                            const bool t3_is_nullable = type_is_nullable(&t3, nullable_enabled);
                            object_set_unknown(&t3, t3_is_nullable, pointed, nullable_enabled);
                        }
                    }


                }
                type_destroy(&t3);
            }
        }

        return;
    }

    if (p_visitor_a->p_type->struct_or_union_specifier && p_visitor_a->p_object->members.size > 0)
    {
        struct struct_or_union_specifier* p_a_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_visitor_a->p_type->struct_or_union_specifier);

        struct struct_or_union_specifier* p_b_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_visitor_b->p_type->struct_or_union_specifier);

        if (p_a_struct_or_union_specifier && p_b_struct_or_union_specifier)
        {
            struct member_declaration* p_a_member_declaration =
                p_a_struct_or_union_specifier->member_declaration_list.head;

            struct member_declaration* p_b_member_declaration =
                p_b_struct_or_union_specifier->member_declaration_list.head;


            while (p_a_member_declaration && p_b_member_declaration)
            {
                if (p_a_member_declaration->member_declarator_list_opt)
                {
                    struct member_declarator* p_a_member_declarator =
                        p_a_member_declaration->member_declarator_list_opt->head;

                    struct member_declarator* p_b_member_declarator =
                        p_b_member_declaration->member_declarator_list_opt->head;

                    while (p_a_member_declarator && p_b_member_declarator)
                    {
                        if (p_a_member_declarator->declarator &&
                            p_b_member_declarator->declarator)
                        {
                            if (p_visitor_a->member_index < p_visitor_a->p_object->members.size &&
                                p_visitor_b->member_index < p_visitor_b->p_object->members.size)
                            {
                                struct object_visitor visitor_a = { 0 };
                                visitor_a.p_type = &p_a_member_declarator->declarator->type;
                                visitor_a.p_object = p_visitor_a->p_object->members.data[p_visitor_a->member_index];


                                struct object_visitor visitor_b = { 0 };
                                visitor_b.p_type = &p_b_member_declarator->declarator->type;
                                visitor_b.p_object = p_visitor_b->p_object->members.data[p_visitor_b->member_index];


                                flow_assignment_core(ctx,
                                    error_position,
                                    assigment_type,
                                    check_uninitialized_b,
                                    a_type_is_view,
                                    a_type_is_nullable,
                                    &visitor_a,
                                    &visitor_b);
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
                                            assigment_type,
                                            check_uninitialized_b,
                                            a_type_is_view,
                                            a_type_is_nullable,
                                            p_visitor_a,
                                            p_visitor_b);

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
            object_set_uninitialized(p_visitor_b->p_type, p_visitor_b->p_object);
        else
            object_set_moved(p_visitor_b->p_type, p_visitor_b->p_object);
    }
}


struct flow_object* expression_get_object(struct flow_visit_ctx* ctx, struct expression* p_expression, bool nullable_enabled)
{
    if (p_expression == NULL)
        return NULL;
    if (p_expression->expression_type == PRIMARY_EXPRESSION_DECLARATOR)
    {
        if (p_expression->declarator->declaration_specifiers &&
            p_expression->declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_EXTERN)
        {
            //External objects are added to the arena on-demand
            if (objects_find(&ctx->arena, p_expression->declarator->p_object) == NULL)
            {
                p_expression->declarator->p_object = make_object(ctx, &p_expression->declarator->type, p_expression->declarator, NULL);

                object_set_unknown(&p_expression->declarator->type,
                                   type_is_nullable(&p_expression->declarator->type, ctx->ctx->options.null_checks_enabled),
                                   p_expression->declarator->p_object,
                                   ctx->ctx->options.null_checks_enabled);
            }
        }
        return p_expression->declarator->p_object;
    }

    else if (p_expression->expression_type == UNARY_EXPRESSION_ADDRESSOF)
    {
        struct flow_object* p_object = make_object(ctx, &p_expression->type, NULL, p_expression);
        object_set_pointer(p_object, expression_get_object(ctx, p_expression->right, nullable_enabled));
        p_object->current.state = OBJECT_STATE_NOT_NULL;
        p_object->is_temporary = true;
        return p_object;
    }
    else if (p_expression->expression_type == PRIMARY_EXPRESSION_PARENTESIS)
    {
        return expression_get_object(ctx, p_expression->right, nullable_enabled);
    }
    else if (p_expression->expression_type == CAST_EXPRESSION)
    {

        struct flow_object* p = expression_get_object(ctx, p_expression->left, nullable_enabled);
        if (p)
        {
            if (type_is_pointer(&p_expression->type_name->type))
            {
                //casting from 0 to pointer we need to change the zero to null
                //#define NULL ((void*)0)
                if (p->current.state & OBJECT_STATE_ZERO)
                {
                    p->current.state &= ~OBJECT_STATE_ZERO;
                    p->current.state |= OBJECT_STATE_NULL;
                }
                if (p->current.state & OBJECT_STATE_NOT_ZERO)
                {
                    p->current.state &= ~OBJECT_STATE_NOT_ZERO;
                    p->current.state |= OBJECT_STATE_NOT_NULL;
                }
            }
        }
        return p;
    }
    else if (p_expression->expression_type == POSTFIX_DOT)
    {
        struct flow_object* p_obj = expression_get_object(ctx, p_expression->left, nullable_enabled);
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
        //All arrays items point to the same object.
        struct flow_object* p_obj = expression_get_object(ctx, p_expression->left, nullable_enabled);
        if (p_obj)
        {

            if (p_obj->current.pointed == NULL)
            {
                expand_pointer_object(ctx, &p_expression->left->type, p_obj);
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
                struct flow_object* p_object = make_object(ctx, &p_expression->type, NULL, p_expression);
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
        struct flow_object* p_obj = expression_get_object(ctx, p_expression->left, nullable_enabled);
        if (p_obj)
        {
            if (p_obj->current.pointed == 0)
            {
                expand_pointer_object(ctx, &p_expression->left->type, p_obj);
            }

            struct flow_object* pointed = p_obj->current.pointed;
            if (pointed != NULL)
            {
                if (p_expression->member_index < pointed->members.size)
                    return pointed->members.data[p_expression->member_index];
                else
                {
                    return NULL;
                }
            }


#if 0
            if (p_obj->current.ref.size == 1)
            {
                struct flow_object* pointed = p_obj->current.ref.data[0];
                if (pointed != NULL)
                {
                    if (p_expression->member_index < pointed->members.size)
                        return pointed->members.data[p_expression->member_index];
                    else
                    {
                        return NULL;
                    }
                }
            }
            else
            {
                struct flow_object* p_object = make_object(ctx, &p_expression->type, NULL, p_expression);
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
    else if (p_expression->expression_type == UNARY_EXPRESSION_CONTENT)
    {
        struct flow_object* p_obj = expression_get_object(ctx, p_expression->right, nullable_enabled);
        if (p_obj)
        {
            if (p_obj->current.pointed == NULL)
            {
                expand_pointer_object(ctx, &p_expression->right->type, p_obj);
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
        struct flow_object* p_object = make_object(ctx, &p_expression->type, NULL, p_expression);

        const bool is_nullable = type_is_nullable(&p_expression->type, nullable_enabled);
        object_set_unknown(&p_expression->type, is_nullable, p_object, nullable_enabled);
        p_object->is_temporary = true;

        if (type_is_pointer(&p_expression->type) && object_is_expansible(p_object))
        {
            expand_pointer_object(ctx, &p_expression->type, p_object);
        }


        return p_object;
    }
    else if (p_expression->expression_type == POSTFIX_EXPRESSION_COMPOUND_LITERAL)
    {
        return p_expression->type_name->declarator->p_object;
    }
    else if (p_expression->expression_type == PRIMARY_EXPRESSION_STRING_LITERAL)
    {
        struct flow_object* p_object = make_object(ctx, &p_expression->type, NULL, p_expression);
        p_object->current.state = OBJECT_STATE_NOT_NULL;
        return p_object;
    }
    else if (p_expression->expression_type == PRIMARY_EXPRESSION_PREDEFINED_CONSTANT)
    {
        struct flow_object* p_object = make_object(ctx, &p_expression->type, NULL, p_expression);
        if (p_expression->type.type_specifier_flags == TYPE_SPECIFIER_NULLPTR_T)
        {
            p_object->current.state = OBJECT_STATE_NULL;
        }
        else
        {
            if (constant_value_is_valid(&p_expression->constant_value))
            {
                bool not_zero = constant_value_to_bool(&p_expression->constant_value);
                p_object->current.state = not_zero ? OBJECT_STATE_NOT_ZERO : OBJECT_STATE_ZERO;
            }
        }

        return p_object;
    }
    else if (p_expression->expression_type == ASSIGNMENT_EXPRESSION)
    {
        struct flow_object* p_obj = expression_get_object(ctx, p_expression->left, nullable_enabled);

        //
        //
        return p_obj;
    }
    else if (p_expression->expression_type == CONDITIONAL_EXPRESSION)
    {
        struct flow_object* p_object = make_object(ctx, &p_expression->type, NULL, p_expression);


        struct flow_object* p_obj1 = expression_get_object(ctx, p_expression->left, nullable_enabled);


        struct flow_object* p_obj2 = expression_get_object(ctx, p_expression->right, nullable_enabled);


        object_merge_state(p_object, p_obj1, p_obj2);



        //object_destroy(&obj1);
        //object_destroy(&obj2);

        return p_object;
    }
    else if (p_expression->expression_type == EQUALITY_EXPRESSION_EQUAL ||
             p_expression->expression_type == EQUALITY_EXPRESSION_NOT_EQUAL)
    {

        struct flow_object* p_object = make_object(ctx, &p_expression->type, NULL, p_expression);
        if (constant_value_is_valid(&p_expression->constant_value))
        {
            bool not_zero = constant_value_to_bool(&p_expression->constant_value);
            p_object->current.state = not_zero ? OBJECT_STATE_NOT_ZERO : OBJECT_STATE_ZERO;
        }
        else
        {
            p_object->current.state = OBJECT_STATE_NOT_ZERO | OBJECT_STATE_ZERO;
        }
        return p_object;
    }
    //
    else
    {

        struct flow_object* p_object = make_object(ctx, &p_expression->type, NULL, p_expression);

        if (type_is_pointer(&p_expression->type))
        {
            if (constant_value_is_valid(&p_expression->constant_value))
            {
                bool not_zero = constant_value_to_bool(&p_expression->constant_value);
                p_object->current.state = not_zero ? OBJECT_STATE_NOT_NULL : OBJECT_STATE_NULL;
            }
            else
            {
                p_object->current.state = OBJECT_STATE_NOT_NULL | OBJECT_STATE_NULL;
            }
        }
        else
        {
            if (constant_value_is_valid(&p_expression->constant_value))
            {
                bool not_zero = constant_value_to_bool(&p_expression->constant_value);
                p_object->current.state = not_zero ? OBJECT_STATE_NOT_ZERO : OBJECT_STATE_ZERO;
            }
            else
            {
                p_object->current.state = OBJECT_STATE_NOT_ZERO | OBJECT_STATE_ZERO;
            }
        }


        return p_object;
    }

    printf("null object");
    //assert(false);
    return NULL;
}

void flow_assignment(
    struct flow_visit_ctx* ctx,
    const struct token* error_position,
    enum assigment_type assigment_type,
    bool check_uninitialized_b,
    bool a_type_is_view,
    bool a_type_is_nullable,
    struct type* p_a_type, struct flow_object* p_a_object,
    struct type* p_b_type, struct flow_object* p_b_object)
{
    if (type_is_pointer(p_b_type) && object_is_expansible(p_b_object))
    {
        //number of warning goes up!
        //expand_pointer_object(ctx, p_b_type, p_b_object);
    }

    struct object_visitor visitor_a = { 0 };
    visitor_a.p_object = p_a_object;
    visitor_a.p_type = p_a_type;
    struct object_visitor visitor_b = { 0 };
    visitor_b.p_object = p_b_object;
    visitor_b.p_type = p_b_type;

    flow_assignment_core(
     ctx,
    error_position,
    assigment_type,
    check_uninitialized_b,
    a_type_is_view,
    a_type_is_nullable,
    &visitor_a,
    &visitor_b);
}


void print_object_state_to_str(enum object_state e, char str[], int sz)
{
    bool first = true;
    struct osstream ss = { 0 };

    if (e == OBJECT_STATE_NOT_APPLICABLE)
        ss_fprintf(&ss, "--");

    if (e & OBJECT_STATE_UNINITIALIZED)
    {
        if (first)
            first = false;
        else
            ss_fprintf(&ss, ",");
        ss_fprintf(&ss, "unset");
    }

    if (e & OBJECT_STATE_NOT_NULL)
    {
        if (first)
            first = false;
        else
            ss_fprintf(&ss, ",");
        ss_fprintf(&ss, "not-null");
    }

    if (e & OBJECT_STATE_NULL)
    {
        if (first)
            first = false;
        else
            ss_fprintf(&ss, ",");
        ss_fprintf(&ss, "null");
    }

    if (e & OBJECT_STATE_ZERO)
    {
        if (first)
            first = false;
        else
            ss_fprintf(&ss, ",");
        ss_fprintf(&ss, "zero");
    }

    if (e & OBJECT_STATE_NOT_ZERO)
    {
        if (first)
            first = false;
        else
            ss_fprintf(&ss, ",");
        ss_fprintf(&ss, "not-zero");
    }


    if (e & OBJECT_STATE_LIFE_TIME_ENDED)
    {
        if (first)
            first = false;
        else
            ss_fprintf(&ss, ",");
        ss_fprintf(&ss, "lifetime-ended");
    }

    if (e & OBJECT_STATE_MOVED)
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
            if (p_object->p_declarator_origin->name)
            {
                line = p_object->p_declarator_origin->name->line;
                col = p_object->p_declarator_origin->name->col;
                ss_fprintf(&ss, "%2d:%2d ", line, col);
                ss_fprintf(&ss, "%s", p_object->p_declarator_origin->name->lexeme);
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
    struct flow_object_state* p_state = &p_object->current;
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
