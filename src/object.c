#include "ownership.h"
#include "object.h"
#include "parser.h"
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include "console.h"

unsigned int s_visit_number = 1; //creates a unique number
unsigned int s_object_id_generator = 0;

bool is_moved(enum object_state e)
{
    return e == OBJECT_STATE_MOVED;
}

bool is_not_null(enum object_state e)
{
    return (
             !(e & OBJECT_STATE_NULL) &&
             ((e & OBJECT_STATE_NOT_NULL) || (e & OBJECT_STATE_MOVED))
           );
}


bool is_null(enum object_state e)
{
    return ((e & OBJECT_STATE_NULL) &&
           !(e & OBJECT_STATE_NOT_NULL) &&
           !(e & OBJECT_STATE_MOVED));
}

bool is_zero(enum object_state e)
{
    return ((e & OBJECT_STATE_ZERO) &&
           !(e & OBJECT_STATE_NOT_ZERO));
}

bool is_not_zero(enum object_state e)
{
    return (!(e & OBJECT_STATE_ZERO) &&
           (e & OBJECT_STATE_NOT_ZERO));
}

bool maybe_is_null(enum object_state e)
{
    return (e & OBJECT_STATE_NULL);
}

bool is_uninitialized(enum object_state e)
{
    return (e & OBJECT_STATE_UNINITIALIZED);
}

/*
  returns true if the pointer object can/should be expanded
*/
bool object_is_expansible(const struct object* p_object)
{
    return (p_object->state != OBJECT_STATE_NULL &&
                p_object->state != OBJECT_STATE_UNINITIALIZED &&
                p_object->ref.size == 0);
}

void object_state_to_string(enum object_state e)
{
    bool first = true;

    printf("\"");
    if (e & OBJECT_STATE_UNINITIALIZED)
    {
        if (first)
            first = false;
        else
            printf(" ");
        printf("uninitialized");
    }

    if (e & OBJECT_STATE_NOT_NULL &&
        e & OBJECT_STATE_NULL)
    {
        if (first)
            first = false;
        else
            printf(" ");
        printf("maybe-null");
    }
    else if (e & OBJECT_STATE_NOT_NULL)
    {
        if (first)
            first = false;
        else
            printf(" ");
        printf("not-null");
    }
    else if (e & OBJECT_STATE_NULL)
    {
        if (first)
            first = false;
        else
            printf(" ");
        printf("null");
    }

    if (e & OBJECT_STATE_NOT_ZERO &&
        e & OBJECT_STATE_ZERO)
    {
        if (first)
            first = false;
        else
            printf(" ");
        printf("any");
    }
    else if (e & OBJECT_STATE_ZERO)
    {
        if (first)
            first = false;
        else
            printf(" ");
        printf("zero");
    }
    else if (e & OBJECT_STATE_NOT_ZERO)
    {
        if (first)
            first = false;
        else
            printf(" ");
        printf("not-zero");
    }


    if (e & OBJECT_STATE_LIFE_TIME_ENDED)
    {
        if (first)
            first = false;
        else
            printf(" ");
        printf("lifetime-ended");
    }


    if (e & OBJECT_STATE_MOVED)
    {
        if (first)
            first = false;
        else
            printf(" ");
        printf("moved");
    }



    printf("\"");

}



void object_swap(struct object* a, struct object* b)
{
    struct object temp = *a;
    *a = *b;
    *b = temp;
}

void object_delete(struct object* owner opt p)
{
    if (p)
    {
        object_destroy(p);
        free(p);
    }
}
int objects_view_reserve(struct objects_view* p, int n);
void object_set_pointer(struct object* p_object, struct object* p_object2)
{
    if (p_object)
    {
        objects_view_reserve(&p_object->ref, 1);
        p_object->ref.size = 1;
        p_object->ref.data[0] = p_object2;
    }
    else
    {
        p_object->ref.size = 0;
    }

}

void object_destroy(struct object* obj_owner p)
{
    objects_destroy(&p->members);
    object_state_stack_destroy(&p->object_state_stack);
}


void object_state_stack_destroy(struct object_state_stack* obj_owner p)
{
    free(p->data);
}

int object_state_stack_reserve(struct object_state_stack* p, int n)
{
    if (n > p->capacity)
    {
        if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0]))))
        {
            return EOVERFLOW;
        }

        void* owner pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL) return ENOMEM;
        static_set(p->data, "moved");
        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

int object_state_stack_push_back(struct object_state_stack* p, enum object_state e, struct objects_view* pointed_ref, const char* name, int state_number)
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

    p->data[p->size].state = e;
    p->data[p->size].name = name;
    p->data[p->size].state_number = state_number;
    p->data[p->size].ref.data = NULL;
    p->data[p->size].ref.size = 0;
    p->data[p->size].ref.capacity = 0;

    objects_view_copy(&p->data[p->size].ref, pointed_ref);

    p->size++;

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


int objects_view_push_back(struct objects_view* p, struct object* p_object)
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

bool objects_view_find(const struct objects_view* p, const struct object* p_object)
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
    free(p->data);
    p->data = NULL;
    p->capacity = 0;
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

bool objects_find(const struct objects* p_objects, const struct object* object)
{
    for (int i = 0; i < p_objects->size; i++)
    {
        if (p_objects->data[i] == object)
            return true;
    }
    return false;
}

int objects_push_back(struct objects* p, struct object* owner p_object)
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

struct object make_object_core(struct type* p_type,
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



    struct object obj = { 0 };
    obj.id = s_object_id_generator++;


    obj.p_expression_origin = p_expression_origin;
    obj.declarator = p_declarator_opt;

    if (p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier* p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier)
        {
            obj.state = OBJECT_STATE_NOT_APPLICABLE;

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
                                struct object* member_obj = calloc(1, sizeof * member_obj);
                                member_obj->p_expression_origin = p_expression_origin;
                                member_obj->declarator = p_declarator_opt;
                                member_obj->state = OBJECT_STATE_NOT_APPLICABLE;
                                objects_push_back(&obj.members, member_obj);
                            }
                            else
                            {
                                struct object* owner p_member_obj = calloc(1, sizeof * p_member_obj);
                                struct object member_obj =
                                    make_object_core(&p_member_declarator->declarator->type,
                                        &l,
                                        p_declarator_opt,
                                        p_expression_origin);
                                *p_member_obj = member_obj;
                                objects_push_back(&obj.members, p_member_obj);
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
                        struct object member_obj = make_object_core(&t, &l, p_declarator_opt, p_expression_origin);
                        struct object* p_member_obj = calloc(1, sizeof * p_member_obj);
                        *p_member_obj = member_obj;
                        objects_push_back(&obj.members, p_member_obj);
                        type_destroy(&t);
                    }
                }
                p_member_declaration = p_member_declaration->next;
            }
            return obj;
        }
        return obj;
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
        return obj;
    }


    obj.state = OBJECT_STATE_UNINITIALIZED;

    return obj;
}

struct object make_object(struct type* p_type,
    const struct declarator* p_declarator_opt,
    const struct expression* p_expression_origin)
{
    //assert(p_token_position);
    struct object_name_list list = { .name = "" };
    return make_object_core(p_type, &list, p_declarator_opt, p_expression_origin);
}

struct token* object_get_token(const struct object* object)
{
    if (object->declarator)
    {
        return object->declarator->name;
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

void object_push_empty_core(struct object* object, const char* name, int state_number, unsigned int visit_number)
{
    if (object->visit_number == visit_number) return;
    object->visit_number = visit_number;

    object_state_stack_push_back(&object->object_state_stack, 0, NULL, name, state_number);

    for (int i = 0; i < object->ref.size; i++)// object_get_pointed_object(object))
    {
        object_push_empty_core(object->ref.data[i], name, state_number, visit_number);
    }

    for (int i = 0; i < object->members.size; i++)
    {
        object_push_empty_core(object->members.data[i], name, state_number, visit_number);
    }
}

void object_push_empty(struct object* object, const char* name, int state_number)
{
    object_push_empty_core(object, name, state_number, s_visit_number++);
}
static void object_push_copy_current_state_core(struct object* object, const char* name, int state_number, unsigned int visit_number)
{
    if (object == NULL) return;
    if (object->visit_number == visit_number) return;
    object->visit_number = visit_number;

    object_state_stack_push_back(&object->object_state_stack, object->state, &object->ref, name, state_number);

    for (int i = 0; i < object->ref.size; i++)
    {
        object_push_copy_current_state_core(object->ref.data[i], name, state_number, visit_number);
    }

    for (int i = 0; i < object->members.size; i++)
    {
        object_push_copy_current_state_core(object->members.data[i], name, state_number, visit_number);
    }

}
void object_push_copy_current_state(struct object* object, const char* name, int state_number)
{
    object_push_copy_current_state_core(object, name, state_number, s_visit_number++);
}

void object_pop_states(struct object* object, int n)
{

    if (object->object_state_stack.size < n)
    {
        //assert(false);
        return;
    }

    object->object_state_stack.size =
        object->object_state_stack.size - n;

    for (int i = 0; i < object->ref.size; i++)
    {
        object_pop_states(object->ref.data[i], n);
    }

    for (int i = 0; i < object->members.size; i++)
    {
        object_pop_states(object->members.data[i], n);
    }

}

void object_restore_state(struct object* object, int state_to_restore)
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

    enum object_state sstate = object->object_state_stack.data[index].state;
    object->state = sstate;

    for (int i = 0; i < object->ref.size; i++)
    {
        object_restore_state(object->ref.data[i], state_to_restore);
    }

    for (int i = 0; i < object->members.size; i++)
    {
        object_restore_state(object->members.data[i], state_to_restore);
    }
}

void print_object_core(int ident, struct type* p_type, struct object* p_object, const char* previous_names, bool is_pointer, bool short_version,
    unsigned int visit_number)
{
    if (p_object == NULL)
    {
        return;
    }
    if (p_object->visit_number == visit_number) return;
    p_object->visit_number = visit_number;

    if (p_type->struct_or_union_specifier && p_object->members.size > 0)
    {
        assert(p_object->state == OBJECT_STATE_NOT_APPLICABLE);
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
            printf("%*c", ident + 1, ' ');
            printf("#%02d {\n", p_object->id);

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
                            if (is_pointer)
                                snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                            else
                                snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);


                            print_object_core(ident + 2, &p_member_declarator->declarator->type,
                                p_object->members.data[member_index], buffer,
                                type_is_pointer(&p_member_declarator->declarator->type), short_version,
                                visit_number);

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

                    struct type t = { 0 };
                    t.category = TYPE_CATEGORY_ITSELF;
                    t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
                    t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;

                    print_object_core(ident + 1, &t, p_object->members.data[member_index], previous_names, false, short_version, visit_number);

                    member_index++;
                    type_destroy(&t);
                }
                p_member_declaration = p_member_declaration->next;
            }

            printf("%*c", ident + 1, ' ');
            printf("}\n");
        }
    }
    //else if (type_is_array(p_type))
    //{
        //p_object->state = flags;
        //if (p_object->members_size > 0)
        //{
        //    //not sure if we instanticate all items of array
        //    p_object->members[0].state = flags;
        //}
    //}
    else if (type_is_pointer(p_type))
    {
        struct type t2 = type_remove_pointer(p_type);
        printf("%*c", ident, ' ');
        if (p_object)
        {
            if (short_version)
            {
                printf("#%02d %s == ", p_object->id, previous_names);
                object_state_to_string(p_object->state);
                if (is_null(p_object->state))
                {
                }
                else if (is_uninitialized(p_object->state))
                {
                }
                else
                {
                    printf(" -> ");
                    if (p_object->ref.size > 0)
                    {
                        for (int i = 0; i < p_object->ref.size; i++)
                        {
                            printf(" #%02d", p_object->ref.data[i]->id);
                        }
                    }
                    else
                    {
                        printf("{...}");
                    }
                }
            }
            else
            {
                printf("%p:%s == ", p_object, previous_names);
                printf("{");
                for (int i = 0; i < p_object->object_state_stack.size; i++)
                {
                    printf(LIGHTCYAN);
                    printf("(#%02d %s)", p_object->object_state_stack.data[i].state_number, p_object->object_state_stack.data[i].name);
                    object_state_to_string(p_object->object_state_stack.data[i].state);
                    printf(RESET);
                    printf(",");
                }
                //printf("*");
                printf(LIGHTMAGENTA);
                printf("(current)");
                object_state_to_string(p_object->state);
                printf(RESET);
                printf("}");
            }
            printf("\n");

            if (p_object->ref.size > 0)
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

                for (int i = 0; i < p_object->ref.size; i++)
                {
                    print_object_core(ident + 1, &t2, p_object->ref.data[i], buffer, is_pointer, short_version, visit_number);
                }
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
                printf("#%02d %s == ", p_object->id, previous_names);
                object_state_to_string(p_object->state);
            }
            else
            {
                printf("%p:%s == ", p_object, previous_names);
                printf("{");
                for (int i = 0; i < p_object->object_state_stack.size; i++)
                {
                    printf("(#%02d %s)", p_object->object_state_stack.data[i].state_number, p_object->object_state_stack.data[i].name);
                    object_state_to_string(p_object->object_state_stack.data[i].state);
                    printf(",");
                }
                object_state_to_string(p_object->state);
                printf("}");
            }


            printf("\n");
        }
    }


}

enum object_state state_merge(enum object_state before, enum object_state after)
{
    enum object_state e = before | after;


    return e;
}

static void object_set_state_from_current_core(struct object* object, int state_number, unsigned int visit_number)
{
    if (object->visit_number == visit_number) return;
    object->visit_number = visit_number;

    for (int i = object->object_state_stack.size - 1; i >= 0; i--)
    {
        if (object->object_state_stack.data[i].state_number == state_number)
        {
            object->object_state_stack.data[i].state = object->state;
            break;
        }
    }

    for (int i = 0; i < object->members.size; i++)
    {
        object_set_state_from_current_core(object->members.data[i], state_number, visit_number);
    }

    for (int i = 0; i < object->ref.size; i++)
    {
        struct object* pointed = object->ref.data[i];
        if (pointed)
        {
            object_set_state_from_current_core(pointed, state_number, visit_number);
        }
    }

}

void object_merge_state(struct object* pdest, struct object* object1, struct object* object2)
{
    pdest->state = object1->state | object2->state;

    if (pdest->members.size == object1->members.size &&
        object1->members.size == object2->members.size)
    {
        for (int i = 0; i < object1->members.size; i++)
        {
            struct object* m1 = object1->members.data[i];
            struct object* m2 = object2->members.data[i];
            object_merge_state(pdest->members.data[i], m1, m2);
        }
    }


    for (int i = 0; i < object1->ref.size; i++)
    {
        struct object* pointed = object1->ref.data[i];
        if (pointed)
        {
            objects_view_push_back(&pdest->ref, pointed);
        }
    }

    for (int i = 0; i < object2->ref.size; i++)
    {
        struct object* pointed = object2->ref.data[i];
        if (pointed)
        {
            objects_view_push_back(&pdest->ref, pointed);
        }
    }



}

void object_set_state_from_current(struct object* object, int state_number)
{
    object_set_state_from_current_core(object, state_number, s_visit_number++);
}
int object_restore_current_state_from_core(struct object* object, int state_number, unsigned int visit_number)
{
    if (object->visit_number == visit_number) return 1;//already visited    
    object->visit_number = visit_number;


    for (int i = object->object_state_stack.size - 1; i >= 0; i--)
    {
        if (object->object_state_stack.data[i].state_number == state_number)
        {
            object->state = object->object_state_stack.data[i].state;
            objects_view_copy(&object->ref, &object->object_state_stack.data[i].ref);
            break;
        }
    }

    for (int i = 0; i < object->members.size; i++)
    {
        object_restore_current_state_from_core(object->members.data[i], state_number, visit_number);
    }

    for (int i = 0; i < object->ref.size; i++)
    {
        struct object* pointed = object->ref.data[i];

        if (pointed)
        {
            object_restore_current_state_from_core(pointed, state_number, visit_number);
        }
    }
    return 1;
}

int object_restore_current_state_from(struct object* object, int state_number)
{
    return object_restore_current_state_from_core(object, state_number, s_visit_number++);
}

int object_merge_current_state_with_state_number_core(struct object* object, int state_number, unsigned int visit_number)
{
    if (object->visit_number == visit_number)
    {
        return 1;//already visited
    }
    object->visit_number = visit_number;

    for (int i = object->object_state_stack.size - 1; i >= 0; i--)
    {
        if (object->object_state_stack.data[i].state_number == state_number)
        {
            object->object_state_stack.data[i].state |= object->state;
            objects_view_merge(&object->object_state_stack.data[i].ref, &object->ref);
            break;
        }
    }

    for (int i = 0; i < object->members.size; i++)
    {
        object_merge_current_state_with_state_number_core(object->members.data[i], state_number, visit_number);
    }

    for (int i = 0; i < object->ref.size; i++)
    {
        struct object* pointed = object->ref.data[i];
        if (pointed)
        {
            object_merge_current_state_with_state_number_core(pointed, state_number, visit_number);
        }
    }
    return 1;
}

int object_merge_current_state_with_state_number(struct object* object, int state_number)
{
    return object_merge_current_state_with_state_number_core(object, state_number, s_visit_number++);
}

static void object_merge_current_state_with_state_number_or_core(struct object* object, int state_number, unsigned int visit_number)
{
    if (object->visit_number == visit_number) return;
    object->visit_number = visit_number;

    for (int i = object->object_state_stack.size - 1; i >= 0; i--)
    {
        if (object->object_state_stack.data[i].state_number == state_number)
        {
            object->object_state_stack.data[i].state |= object->state;
            objects_view_merge(&object->object_state_stack.data[i].ref, &object->ref);
            break;
        }
    }

    for (int i = 0; i < object->members.size; i++)
    {
        object_merge_current_state_with_state_number_or_core(object->members.data[i], state_number, visit_number);
    }

    for (int i = 0; i < object->ref.size; i++)
    {
        struct object* pointed = object->ref.data[i];

        if (pointed)
        {
            object_merge_current_state_with_state_number_or_core(pointed, state_number, visit_number);
        }
    }
}

void object_merge_current_state_with_state_number_or(struct object* object, int state_number)
{
    object_merge_current_state_with_state_number_or_core(object, state_number, s_visit_number++);
}
void object_get_name(const struct type* p_type,
    const struct object* p_object,
    char* outname,
    int out_size);


void print_object(struct type* p_type, struct object* p_object, bool short_version)
{
    if (p_object == NULL)
    {
        printf("null object");
        return;
    }
    char name[100] = { 0 };
    object_get_name(p_type, p_object, name, sizeof name);



    print_object_core(0, p_type, p_object, name, type_is_pointer(p_type), short_version, s_visit_number++);
}

void set_object(
    struct type* p_type,
    struct object* p_object,
    enum object_state flags);



void set_direct_state(
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
                                set_direct_state(&p_member_declarator->declarator->type, p_object->members.data[member_index], flags);
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

void set_object(
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
                                set_object(&p_member_declarator->declarator->type, p_object->members.data[member_index], flags);
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
            p_object->members.data[0]->state = flags;
        }
    }
    else if (type_is_pointer(p_type))
    {
        p_object->state = flags;

        if (p_object->ref.size > 0)
        {
            struct type t2 = type_remove_pointer(p_type);
            if (type_is_out(&t2))
            {
                flags = OBJECT_STATE_UNINITIALIZED;
            }
            for (int i = 0; i < p_object->ref.size; i++)
            {
                struct object* pointed = p_object->ref.data[i];
                set_object(&t2, pointed, flags);
            }

            type_destroy(&t2);
        }
    }
    else
    {
        //assert(p_object->members.size == 0); //enum?
        p_object->state = flags;
    }
}

void object_set_nothing(struct type* p_type, struct object* p_object)
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
                                object_set_nothing(&p_member_declarator->declarator->type, p_object->members.data[member_index]);
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
        p_object->state = 0;

        if (p_object->ref.size > 0)
        {
            struct type t2 = type_remove_pointer(p_type);
            for (int i = 0; i < p_object->ref.size; i++)
            {
                struct object* pointed = p_object->ref.data[i];
                object_set_uninitialized(&t2, pointed);
            }
            type_destroy(&t2);
        }
    }
    else
    {
        p_object->state = 0;
    }
}
void object_set_uninitialized(struct type* p_type, struct object* p_object)
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
                                object_set_uninitialized(&p_member_declarator->declarator->type, p_object->members.data[member_index]);
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
        p_object->state = OBJECT_STATE_UNINITIALIZED;


        if (p_object->ref.size)
        {
            struct type t2 = type_remove_pointer(p_type);
            for (int i = 0; i < p_object->ref.size; i++)
            {
                struct object* pointed = p_object->ref.data[i];
                object_set_nothing(&t2, pointed);
            }
            type_destroy(&t2);
        }
    }
    else
    {
        p_object->state = OBJECT_STATE_UNINITIALIZED;
    }
}


static void checked_empty_core(struct parser_ctx* ctx,
    struct type* p_type,
    struct object* p_object,
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
        if (is_moved(p_object->state) ||
            is_uninitialized(p_object->state) ||
            is_null(p_object->state))
        {
        }
        else
        {

            compiler_diagnostic_message(W_FLOW_MISSING_DTOR,
                ctx,
                position_token,
                "object '%s' may be not empty",
                previous_names);
        }
    }
}

void checked_empty(struct parser_ctx* ctx,
    struct type* p_type,
    struct object* p_object,
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

void object_set_moved(struct type* p_type, struct object* p_object)
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
                                object_set_moved(&p_member_declarator->declarator->type, p_object->members.data[member_index]);
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
        p_object->state |= OBJECT_STATE_MOVED;

        if (p_object->ref.size > 0)
        {
            struct type t2 = type_remove_pointer(p_type);
            for (int i = 0; i < p_object->ref.size; i++)
            {
                struct object* pointed = p_object->ref.data[i];
                object_set_nothing(&t2, pointed);
            }
            type_destroy(&t2);
        }
    }
    else
    {
        p_object->state |= OBJECT_STATE_MOVED;
    }
}

static void object_set_unknown_core(struct type* p_type, bool t_is_nullable, struct object* p_object, unsigned int visit_number, bool nullable_enabled)
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
                                object_set_unknown_core(&p_member_declarator->declarator->type,
                                    t_is_nullable,
                                    p_object->members.data[member_index], visit_number, nullable_enabled);
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
        if (t_is_nullable || type_is_nullable(p_type, nullable_enabled))
            p_object->state = OBJECT_STATE_NULL | OBJECT_STATE_NOT_NULL;
        else
            p_object->state = OBJECT_STATE_NOT_NULL;


        for (int i = 0; i < p_object->ref.size; i++)
        {
            struct object* pointed = p_object->ref.data[i];
            if (pointed)
            {
                struct type t2 = type_remove_pointer(p_type);
                bool t2_is_nullable = type_is_nullable(&t2, nullable_enabled);
                object_set_unknown_core(&t2, t2_is_nullable, pointed, visit_number, nullable_enabled);
                type_destroy(&t2);
            }
        }
    }
    else
    {
        if (!type_is_struct_or_union(p_type))
            p_object->state = OBJECT_STATE_ZERO | OBJECT_STATE_NOT_ZERO;
    }
}

void object_set_unknown(struct type* p_type, bool t_is_nullable, struct object* p_object, bool nullable_enabled)
{
    object_set_unknown_core(p_type, t_is_nullable, p_object, s_visit_number++, nullable_enabled);
}


static void object_set_deleted_core(struct type* p_type, struct object* p_object, unsigned int visit_number)
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
        p_object->state = OBJECT_STATE_LIFE_TIME_ENDED;
        for (int i = 0; i < p_object->ref.size; i++)
        {
            struct object* pointed = p_object->ref.data[i];
            if (pointed)
            {
                struct type t2 = type_remove_pointer(p_type);
                object_set_deleted_core(&t2, pointed, visit_number);
                type_destroy(&t2);
            }
        }
    }
    else
    {
        if (!type_is_struct_or_union(p_type))
            p_object->state = OBJECT_STATE_LIFE_TIME_ENDED;
    }
}

void object_set_deleted(struct type* p_type, struct object* p_object)
{
    object_set_deleted_core(p_type, p_object, s_visit_number++);
}

void object_set_zero(struct type* p_type, struct object* p_object)
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
                                object_set_zero(&p_member_declarator->declarator->type, p_object->members.data[member_index]);
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
        p_object->state = OBJECT_STATE_NULL;


        if (p_object->ref.size)
        {
            /*
              if the pointer is null, there is no pointed object
            */
            struct type t2 = type_remove_pointer(p_type);
            for (int i = 0; i < p_object->ref.size; i++)
            {
                object_set_nothing(&t2, p_object->ref.data[i]);
            }
            type_destroy(&t2);
        }
    }
    else
    {
        p_object->state = OBJECT_STATE_ZERO;
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
            should_had_been_moved = (p_object->state & OBJECT_STATE_NOT_NULL);
        }
        else
        {
            if (p_object->state == OBJECT_STATE_UNINITIALIZED ||
                (p_object->state & OBJECT_STATE_MOVED) ||
                p_object->state == OBJECT_STATE_NOT_NULL ||
                p_object->state == (OBJECT_STATE_UNINITIALIZED))
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
    char* outname,
    int out_size,
    unsigned int visit_number)
{
    if (p_object == NULL)
    {
        return;
    }
    if (p_object->visit_number == visit_number) return;
    ((struct object*)p_object)->visit_number = visit_number;

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
                if (p_object->ref.size > 0)
                {
                    object_get_name_core(
                        &t2,
                        p_object->ref.data[0],
                        p_object_target,
                        buffer,
                        outname,
                        out_size,
                        visit_number);
                }
            }
            type_destroy(&t2);
        }
    }
}


void object_get_name(const struct type* p_type,
    const struct object* p_object,
    char* outname,
    int out_size)
{
    outname[0] = '\0';

    if (p_object->declarator)
    {

        const char* root_name = p_object->declarator->name ? p_object->declarator->name->lexeme : "?";
        //snprintf(outname, out_size, "%s",root_name);

        const struct object* root = &p_object->declarator->object;

        object_get_name_core(&p_object->declarator->type, root, p_object, root_name, outname, out_size, s_visit_number++);
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

void checked_moved_core(struct parser_ctx* ctx,
    struct type* p_type,
    struct object* p_object,
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
            if (p_object->state != OBJECT_STATE_UNINITIALIZED &&
                p_object->state != OBJECT_STATE_NULL)
            {
                struct type t2 = type_remove_pointer(p_type);
                for (int i = 0; i < p_object->ref.size; i++)
                {
                    checked_moved_core(ctx,
                        &t2,
                        p_object->ref.data[i],
                        position_token,
                        visit_number);
                }
                type_destroy(&t2);
            }
        }

        if (p_object->state & OBJECT_STATE_MOVED)
        {
            struct token* name_pos = object_get_token(p_object);
            const char* parameter_name = name_pos ? name_pos->lexeme : "?";


            char name[200] = { 0 };
            object_get_name(p_type, p_object, name, sizeof name);
            if (compiler_diagnostic_message(W_FLOW_MISSING_DTOR,
                ctx,
                position_token,
                "parameter '%s' is leaving scoped with a moved object '%s'",
                parameter_name,
                name))
            {
                compiler_diagnostic_message(W_LOCATION, ctx, name_pos, "parameter", name);
            }
        }

        if (p_object->state & OBJECT_STATE_UNINITIALIZED)
        {
            struct token* name_pos = object_get_token(p_object);
            const char* parameter_name = name_pos ? name_pos->lexeme : "?";

            char name[200] = { 0 };
            object_get_name(p_type, p_object, name, sizeof name);
            if (compiler_diagnostic_message(W_FLOW_MISSING_DTOR,
                ctx,
                position_token,
                "parameter '%s' is leaving scoped with a uninitialized object '%s'",
                parameter_name,
                name))
            {
                compiler_diagnostic_message(W_LOCATION, ctx, name_pos, "parameter", name);
            }
        }
    }
}

void checked_moved(struct parser_ctx* ctx,
    struct type* p_type,
    struct object* p_object,
    const struct token* position_token)
{
    checked_moved_core(ctx,
    p_type,
    p_object,
    position_token,
    s_visit_number++);
}

void checked_read_object_core(struct parser_ctx* ctx,
    struct type* p_type,
    bool is_nullable,
    struct object* p_object,
    const struct token* position_token,
    bool check_pointed_object,
    const char* previous_names,
    unsigned int visit_number)
{
    if (p_object == NULL)
    {
        return;
    }

    if (p_object->visit_number == visit_number) return;
    p_object->visit_number = visit_number;


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
                        const char* name =
                            p_member_declarator->declarator->name ? p_member_declarator->declarator->name->lexeme : "?";

                        char buffer[200] = { 0 };
                        if (type_is_pointer(p_type))
                            snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                        else
                            snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);

                        checked_read_object_core(ctx,
                            &p_member_declarator->declarator->type,
                            is_nullable,
                            p_object->members.data[member_index],
                            position_token,
                            check_pointed_object,
                            buffer,
                            visit_number);

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

        if (type_is_pointer(p_type) &&
            !is_nullable &&
            !type_is_nullable(p_type, ctx->options.null_checks_enabled) &&
            maybe_is_null(p_object->state))
        {
            compiler_diagnostic_message(W_FLOW_NULL_DEREFERENCE,
                ctx,
                position_token,
                "object '%s', (non-optional) may be null",
                previous_names);
        }

        if (type_is_pointer(p_type) &&
            check_pointed_object &&
            p_object->state & OBJECT_STATE_NOT_NULL /*we don't need to check pointed object*/
            )
        {
            struct type t2 = type_remove_pointer(p_type);
            for (int i = 0; i < p_object->ref.size; i++)
            {
                checked_read_object_core(ctx,
                    &t2,
                    is_nullable,
                    p_object->ref.data[i],
                    position_token,
                    true,
                    previous_names,
                    visit_number);
            }
            type_destroy(&t2);
        }


        if (p_object->state & OBJECT_STATE_UNINITIALIZED)
        {
            compiler_diagnostic_message(W_FLOW_UNINITIALIZED,
                ctx,
                position_token,
                "uninitialized object '%s'",
                previous_names);
        }
    }
}


void checked_read_object(struct parser_ctx* ctx,
    struct type* p_type,
    bool is_nullable,
    struct object* p_object,
    const struct token* position_token,
    bool check_pointed_object)
{
    const char* owner s = NULL;
    const char* name = "";
    if (p_object->declarator)
        name = p_object->declarator->name ? p_object->declarator->name->lexeme : "?";
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

    checked_read_object_core(ctx,
    p_type,
    is_nullable,
    p_object,
    position_token,
    check_pointed_object,
    name,
    s_visit_number++);
    free((void* owner)s);
}


static void visit_object_core(struct parser_ctx* ctx,
    struct type* p_type,
    struct object* p_object,
    const struct token* position_token,
    const char* previous_names,
    bool is_assigment,
    unsigned int visit_number)
{
    if (p_object == NULL)
    {
        return;
    }

    if (p_object->visit_number == visit_number) return;
    p_object->visit_number = visit_number;

    //if (p_type->type_qualifier_flags & TYPE_QUALIFIER_VIEW)
    //{
      //  if (!type_is_struct_or_union(p_type)
        //  p_object->state = OBJECT_STATE_LIFE_TIME_ENDED;
        //return;
    //}

    if (!type_is_any_owner(p_type))
    {
        //if (p_type->storage_class_specifier_flags & STORAGE_SPECIFIER_PARAMETER)
        //{
            //for view parameters we need to check if they left something moved..
          //  checked_moved(ctx,
            //    p_type,
              //  p_object,
                //position_token);

        //}
        //p_object->state = OBJECT_STATE_LIFE_TIME_ENDED;
        //return;
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
            const struct token* name = object_get_token(p_object);
            if (compiler_diagnostic_message(W_FLOW_MISSING_DTOR,
                ctx,
                name,
                "object '%s' was not moved/destroyed",
                previous_names))
            {

                if (p_object->declarator)
                    compiler_diagnostic_message(W_LOCATION, ctx, position_token, "end of '%s' scope", previous_names);
            }
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

                            char buffer[200] = { 0 };
                            if (type_is_pointer(p_type))
                                snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                            else
                                snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);

                            visit_object_core(ctx, &p_member_declarator->declarator->type,
                                p_object->members.data[member_index],
                                position_token,
                                buffer,
                                is_assigment,
                                visit_number);

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
        const struct token* position = NULL;
        if (p_object->declarator)
            position = p_object->declarator->name ? p_object->declarator->name : p_object->declarator->first_token;
        else if (p_object->p_expression_origin)
            position = p_object->p_expression_origin->first_token;
        else
        {
            assert(false);
        }

        if (name[0] == '\0')
        {
            /*function arguments without name*/
            name = "?";
        }
        bool should_had_been_moved = false;


        /*
           Despite the name OBJECT_STATE_NOT_NULL does not means null, it means
           the reference is not referring an object, the value could be -1 for instance.
        */
        if (type_is_pointer(p_type))
        {
            should_had_been_moved = (p_object->state & OBJECT_STATE_NOT_NULL) &&
                !(p_object->state & OBJECT_STATE_MOVED);
        }
        else
        {
            if (p_object->state == OBJECT_STATE_UNINITIALIZED ||
                (p_object->state & OBJECT_STATE_MOVED) ||
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
                char buffer[100] = { 0 };
                snprintf(buffer, sizeof buffer, "%s", previous_names);

                struct type t2 = type_remove_pointer(p_type);
                if (type_is_owner(&t2))
                {
                    for (int i = 0; i < p_object->ref.size; i++)
                    {
                        visit_object_core(ctx,
                            &t2,
                            p_object->ref.data[i],
                            position_token,
                            buffer,
                            is_assigment,
                            visit_number);
                    }
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
                    struct type t2 = type_remove_pointer(p_type);
                    bool pointed_is_out = type_is_out(&t2);
                    type_destroy(&t2);

                    if (!pointed_is_out && !(p_object->state & OBJECT_STATE_MOVED))
                    {
                        if (is_assigment)
                        {
                            compiler_diagnostic_message(W_FLOW_MISSING_DTOR,
                                ctx,
                                position_token,
                                "memory pointed by '%s' was not released before assignment.",
                                name);
                        }
                        else
                        {
                            compiler_diagnostic_message(W_FLOW_MISSING_DTOR,
                                ctx,
                                position,
                                "memory pointed by '%s' was not released.",
                                name);
                            if (p_object->declarator)
                            {
                                compiler_diagnostic_message(W_LOCATION, ctx, position_token, "end of '%s' scope", name);
                            }
                        }
                    }
                }
                else
                {
                    if (is_assigment)
                    {
                        compiler_diagnostic_message(W_FLOW_MISSING_DTOR,
                            ctx,
                            position_token,
                            "previous members of '%s' were not moved before this assignment.",
                            name);
                    }
                    else
                    {
                        compiler_diagnostic_message(W_FLOW_MISSING_DTOR,
                            ctx,
                            position,
                            "object '%s' was not moved.",
                            name);
                        if (p_object->declarator)
                        {
                            compiler_diagnostic_message(W_LOCATION, ctx, position_token, "end of '%s' scope", name);
                        }
                    }
                }
            }
        }

        p_object->state = OBJECT_STATE_LIFE_TIME_ENDED;
    }

}

void visit_object(struct parser_ctx* ctx,
    struct type* p_type,
    struct object* p_object,
    const struct token* position_token,
    const char* previous_names,
    bool is_assigment)
{
    visit_object_core(ctx,
    p_type,
    p_object,
    position_token,
    previous_names,
    is_assigment, s_visit_number++);
}


static void end_of_storage_visit_core(struct parser_ctx* ctx,
    struct type* p_type,
    bool b_type_is_view,
    struct object* p_object,
    const struct token* position_token,
    const char* previous_names,
    unsigned int visit_number)
{
    if (p_object == NULL)
    {
        return;
    }

    if (p_object->visit_number == visit_number) return;
    p_object->visit_number = visit_number;


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
            const struct token* name = object_get_token(p_object);
            if (compiler_diagnostic_message(W_FLOW_MISSING_DTOR,
                ctx,
                name,
                "object '%s' was not moved/destroyed",
                previous_names))
            {

                if (p_object->declarator)
                    compiler_diagnostic_message(W_LOCATION, ctx, position_token, "end of '%s' scope", previous_names);
            }
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

                            char buffer[200] = { 0 };
                            if (type_is_pointer(p_type))
                                snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                            else
                                snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);

                            end_of_storage_visit_core(ctx,
                                &p_member_declarator->declarator->type,
                                b_type_is_view,
                                p_object->members.data[member_index],
                                position_token,
                                buffer,
                                visit_number);

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
        const struct token* position = NULL;
        if (p_object->declarator)
            position = p_object->declarator->name ? p_object->declarator->name : p_object->declarator->first_token;
        else if (p_object->p_expression_origin)
            position = p_object->p_expression_origin->first_token;
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
        if (type_is_pointer(p_type) &&
            !b_type_is_view &&
            type_is_owner(p_type) &&
            p_object->state & OBJECT_STATE_NOT_NULL)
        {
            if (compiler_diagnostic_message(W_FLOW_MISSING_DTOR,
                ctx,
                position,
                "ownership of '%s' not moved before the end of lifetime", previous_names))
            {
                compiler_diagnostic_message(W_LOCATION,
                ctx,
                position_token,
                "end of lifetime");
            }
        }
        else if (!b_type_is_view && type_is_obj_owner(p_type) && type_is_pointer(p_type))
        {
            char buffer[100] = { 0 };
            snprintf(buffer, sizeof buffer, "%s", previous_names);
            struct type t2 = type_remove_pointer(p_type);

            for (int i = 0; i < p_object->ref.size; i++)
            {
                end_of_storage_visit_core(ctx, &t2, b_type_is_view, p_object->ref.data[i], position, buffer, visit_number);
            }
            type_destroy(&t2);
        }
        else if (type_is_owner(p_type) && !type_is_pointer(p_type))
        {
            //non-pointer owner
            if (p_object->state == OBJECT_STATE_UNINITIALIZED ||
                p_object->state == OBJECT_STATE_NULL ||
                p_object->state == OBJECT_STATE_MOVED)
            {
            }
            else
            {
                if (compiler_diagnostic_message(W_FLOW_MISSING_DTOR,
                    ctx,
                    position,
                    "ownership of '%s' not moved before the end of lifetime", previous_names))
                {
                    compiler_diagnostic_message(W_LOCATION,
                    ctx,
                    position_token,
                    "end of lifetime");
                }
            }
        }
        else
        {
        }

        p_object->state = OBJECT_STATE_LIFE_TIME_ENDED;
    }
}

void end_of_storage_visit(struct parser_ctx* ctx,
    struct type* p_type,
    bool type_is_view,
    struct object* p_object,
    const struct token* position_token,
    const char* previous_names)
{
    end_of_storage_visit_core(ctx,
    p_type,
    type_is_view,
    p_object,
    position_token,
    previous_names,
    s_visit_number++);
}



bool object_is_zero_or_null(const struct object* p_object)
{
    return (p_object->state == OBJECT_STATE_NULL) ||
        (p_object->state == OBJECT_STATE_ZERO);
}




/*
   This function must check and do the flow assignment of
   a = b
*/
void object_assignment3(
    struct parser_ctx* ctx,
    const struct token* error_position,
    enum assigment_type assigment_type,
    bool check_uninitialized_b,
    bool a_type_is_view,
    bool a_type_is_nullable,
    struct type* p_a_type, struct object* p_a_object,
    struct type* p_b_type, struct object* p_b_object)
{
    if (p_a_object == NULL || p_b_object == NULL)
    {
        return;
    }
    const bool nullable_enabled = ctx->options.null_checks_enabled;

    // printf("line  %d\n", error_position->line);
     //type_print(p_a_type);
     //printf(" = ");
     //type_print(p_b_type);
     //printf("\n");

     /*general check for copying uninitialized object*/
    if (check_uninitialized_b && p_b_object->state & OBJECT_STATE_UNINITIALIZED)
    {
        //a = b where b is uninitialized
        char buffer[100] = { 0 };
        object_get_name(p_b_type, p_b_object, buffer, sizeof buffer);
        if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
        {
            if (!type_is_out(p_a_type))
            {
                compiler_diagnostic_message(W_FLOW_UNINITIALIZED,
                            ctx,
                            error_position,
                            "passing an uninitialized argument '%s' object", buffer);
            }
        }
        else if (assigment_type == ASSIGMENT_TYPE_RETURN)
        {
            compiler_diagnostic_message(W_FLOW_UNINITIALIZED,
                        ctx,
                        error_position,
                        "returning an uninitialized '%s' object", buffer);
        }
        else
        {
            compiler_diagnostic_message(W_FLOW_UNINITIALIZED,
                        ctx,
                        error_position,
                        "reading an uninitialized '%s' object", buffer);
        }

        return;
    }

    if (check_uninitialized_b && p_a_object->state & OBJECT_STATE_LIFE_TIME_ENDED)
    {
        //a = b where a was deleted
        char buffer[100] = { 0 };
        object_get_name(p_a_type, p_a_object, buffer, sizeof buffer);

        compiler_diagnostic_message(W_FLOW_LIFETIME_ENDED,
                    ctx,
                    error_position,
                    "The object '%s' may have been deleted or its lifetime have ended.", buffer);


        return;
    }

    /*general check passing possible null to non opt*/
    if (type_is_pointer(p_a_type) &&
        (!type_is_nullable(p_a_type, ctx->options.null_checks_enabled)) &&
        p_b_object->state & OBJECT_STATE_NULL)
    {
        if (!a_type_is_nullable)
        {
            char buffer[100] = { 0 };
            object_get_name(p_b_type, p_b_object, buffer, sizeof buffer);

            compiler_diagnostic_message(W_FLOW_NULLABLE_TO_NON_NULLABLE,
                       ctx,
                       error_position,
                       "assignment of possible null object '%s' to non-nullable pointer", buffer);
        }

    }

    if (type_is_pointer(p_a_type))
    {
        if (!a_type_is_view && type_is_owner(p_a_type))
        {
            checked_empty(ctx, p_a_type, p_a_object, error_position);
        }

        if (object_is_zero_or_null(p_b_object))
        {
            if (type_is_array(p_b_type))
            {
                //int b[2] = {0};
                //int * a = b;
                //object_set_zero(p_a_type, p_a_object);
                objects_view_clear(&p_a_object->ref);
                p_a_object->state = OBJECT_STATE_NOT_NULL;
                return;
            }
            else if (type_is_nullptr_t(p_b_type) || type_is_integer(p_b_type))
            {
                //a = nullpr
                //object_set_zero(p_a_type, p_a_object);
                //p_a_object->pointed_ref = NULL;
                objects_view_clear(&p_a_object->ref);
                p_a_object->state = OBJECT_STATE_NULL;
                return;
            }
        }
    }

    if (!a_type_is_view && type_is_obj_owner(p_a_type) && type_is_pointer(p_a_type))
    {
        checked_empty(ctx, p_a_type, p_a_object, error_position);

        if (object_is_zero_or_null(p_b_object))
        {
            //0 to objec_owner??
            //a = nullpr
            //object_set_zero(p_a_type, p_a_object);
            return;
        }
    }

    /*copying to void * owner*/
    if (type_is_void_ptr(p_a_type) && type_is_pointer(p_b_type))
    {
        p_a_object->state = p_b_object->state;
        objects_view_copy(&p_a_object->ref, &p_b_object->ref);
        //p_a_object->pointed_ref = p_b_object->pointed_ref;


        if (!a_type_is_view && type_is_owner(p_a_type))
        {
            //*b must be empty before copying to void* owner
            struct type t = type_remove_pointer(p_b_type);

            if (p_b_object->ref.size == 0)
            {
                // The question is..if we had this object expanded
                // could it possible have resources?
                //-> {...}
                if (object_is_expansible(p_b_object) &&
                    type_is_owner(&t))
                {
                    //if the anwser is yes then we need a warning
                    compiler_diagnostic_message(W_FLOW_MISSING_DTOR,
                                                ctx,
                                                error_position,
                                                "pointed object may be not empty");
                }
            }
            else
            {
                for (int i = 0; i < p_b_object->ref.size; i++)
                {
                    checked_empty(ctx, &t, p_b_object->ref.data[i], error_position);
                    object_set_deleted(&t, p_b_object->ref.data[i]);
                }
            }
            type_destroy(&t);

            if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
            {
                p_b_object->state = OBJECT_STATE_UNINITIALIZED;
                //objects_view_clear(&p_b_object->ref);
                //p_b_object->pointed_ref = NULL;
                //object_set_deleted(p_b_type, p_b_object);
            }
            else
            {
                p_b_object->state &= ~OBJECT_STATE_NOT_NULL;
                p_b_object->state |= OBJECT_STATE_MOVED;
            }

        }
        return;
    }

    if (!type_is_any_owner(p_a_type) && type_is_any_owner(p_b_type))
    {
        if (p_b_type->storage_class_specifier_flags & STORAGE_SPECIFIER_FUNCTION_RETURN)
        {
            compiler_diagnostic_message(W_OWNERSHIP_USING_TEMPORARY_OWNER,
            ctx,
            error_position,
            "passing a temporary owner to a view");
        }
    }
    if (type_is_pointer(p_a_type) && type_is_pointer(p_b_type))
    {
        p_a_object->state = p_b_object->state;
        objects_view_copy(&p_a_object->ref, &p_b_object->ref);
        //p_a_object->pointed_ref = p_b_object->pointed_ref;

        struct type t = type_remove_pointer(p_a_type);

        /*if the parameter points to out object, then we don´t need to check
          argument pointed object.
        */
        const bool checked_pointed_object_read = !type_is_out(&t);
        bool is_nullable = a_type_is_nullable || type_is_nullable(&t, ctx->options.null_checks_enabled);
        checked_read_object(ctx, p_b_type, is_nullable, p_b_object, error_position, checked_pointed_object_read);


        //object_copy_state(p_a_type, p_a_object, p_b_type, p_b_object);
        type_destroy(&t);

        if (!a_type_is_view && type_is_owner(p_a_type))
        {
            /*
               T * _Owner pA = pB;
            */

            if (p_b_object->state & OBJECT_STATE_MOVED)
            {
                //TODO we need 2 positions, source, dest
                compiler_diagnostic_message(W_FLOW_MOVED,
                   ctx,
                   error_position,
                   "source object has already been moved");
            }


            if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
            {
                p_b_object->state = OBJECT_STATE_UNINITIALIZED;

                for (int i = 0; i < p_b_object->ref.size; i++)
                {
                    struct object* pointed = p_b_object->ref.data[i];
                    if (pointed)
                    {

                        struct type t2 = type_remove_pointer(p_b_type);
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
                p_a_object->state = p_a_object->state & ~OBJECT_STATE_MOVED;
                if (p_b_object->state & OBJECT_STATE_NOT_NULL)
                {
                    //null not-null -> null moved
                    p_b_object->state &= ~OBJECT_STATE_NOT_NULL;
                    p_b_object->state |= OBJECT_STATE_MOVED;
                }
            }
        }
        else if (!a_type_is_view && type_is_obj_owner(p_a_type))
        {
            if (type_is_any_owner(p_b_type))
            {
                if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
                {
                    //p_b_object->state = OBJECT_STATE_UNINITIALIZED;
                    for (int i = 0; i < p_b_object->ref.size; i++)
                    {
                        struct object* pointed = p_b_object->ref.data[i];
                        if (pointed)
                        {
                            struct type t2 = type_remove_pointer(p_b_type);
                            object_set_uninitialized(&t2, pointed);
                            type_destroy(&t2);
                        }
                    }

                }
                else
                    object_set_moved(p_b_type, p_b_object);
            }
            else
            {
                if (p_b_type->address_of)
                {
                    //must be address of.
                    if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
                    {
                        p_b_object->state = OBJECT_STATE_UNINITIALIZED;
                        for (int i = 0; i < p_b_object->ref.size; i++)
                        {
                            struct object* pointed = p_b_object->ref.data[i];
                            if (pointed)
                            {
                                struct type t2 = type_remove_pointer(p_b_type);
                                object_set_uninitialized(&t2, pointed);
                                type_destroy(&t2);
                            }
                        }

                    }
                    else
                        object_set_moved(p_b_type, p_b_object);
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
            if (a_type_is_view || !type_is_owner(p_a_type))
            {
                if (type_is_owner(p_b_type))
                {
                    if (p_b_type->storage_class_specifier_flags & STORAGE_SPECIFIER_FUNCTION_RETURN)
                    {
                        /*
                      This situation
                      struct X* p = (struct X* _Owner) malloc(1);
                    */
                        compiler_diagnostic_message(W_OWNERSHIP_MISSING_OWNER_QUALIFIER,
                       ctx,
                       error_position,
                       "owner object has short lifetime");
                    }
                }

                p_a_object->state = p_b_object->state;

                p_a_object->state = p_a_object->state & ~OBJECT_STATE_MOVED;
            }

            if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
            {
                struct type t3 = type_remove_pointer(p_a_type);
                if (!type_is_const(&t3))
                {
                    for (int i = 0; i < p_b_object->ref.size; i++)
                    {
                        struct object* pointed = p_b_object->ref.data[i];
                        if (pointed)
                        {
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

    if (p_a_type->struct_or_union_specifier && p_a_object->members.size > 0)
    {
        struct struct_or_union_specifier* p_a_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_a_type->struct_or_union_specifier);

        struct struct_or_union_specifier* p_b_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_b_type->struct_or_union_specifier);

        if (p_a_struct_or_union_specifier && p_b_struct_or_union_specifier)
        {
            struct member_declaration* p_a_member_declaration =
                p_a_struct_or_union_specifier->member_declaration_list.head;

            struct member_declaration* p_b_member_declaration =
                p_b_struct_or_union_specifier->member_declaration_list.head;

            int member_index = 0;
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
                            if (member_index < p_a_object->members.size &&
                                member_index < p_b_object->members.size)
                            {

                                struct type* p_a_member_type = &p_a_member_declarator->declarator->type;
                                struct object* p_a_member_object = p_a_object->members.data[member_index];

                                struct type* p_b_member_type = &p_b_member_declarator->declarator->type;
                                struct object* p_b_member_object = p_b_object->members.data[member_index];

                                object_assignment3(ctx,
                                    error_position,
                                    assigment_type,
                                    check_uninitialized_b,
                                    a_type_is_view,
                                    a_type_is_nullable,
                                    p_a_member_type, p_a_member_object,
                                    p_b_member_type, p_b_member_object);
                            }
                            else
                            {
                                //TODO BUG union?                                
                            }
                            member_index++;
                        }
                        p_a_member_declarator = p_a_member_declarator->next;
                        p_b_member_declarator = p_b_member_declarator->next;
                    }
                }
                p_a_member_declaration = p_a_member_declaration->next;
                p_b_member_declaration = p_b_member_declaration->next;
            }
            return;
        }
    }

    p_a_object->state = p_b_object->state;
    if (!a_type_is_view && type_is_owner(p_a_type))
    {
        if (assigment_type == ASSIGMENT_TYPE_PARAMETER)
            object_set_uninitialized(p_b_type, p_b_object);
        else
            object_set_moved(p_b_type, p_b_object);
    }
}
