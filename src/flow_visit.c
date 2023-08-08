#include "ownership.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include "flow_visit.h"
#include "expressions.h"
#include "ownership.h"

struct flow_defer_scope
{
    struct declarator* declarator; // declarator 
    struct defer_statement* defer_statement; // defer 
    struct try_statement* p_try_statement; //try
    struct selection_statement* p_selection_statement2; //if swith
    struct iteration_statement* p_iteration_statement; //for do while
    struct statement* p_statement; //
    struct compound_statement* p_function_body;

    struct flow_defer_scope* owner last_child;
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


int objects_reserve(struct objects* p, int n)
{
    if (n > p->capacity)
    {
        if ((size_t) n > (SIZE_MAX / (sizeof(p->data[0]))))
        {
            return EOVERFLOW;
        }

        void* pnew = realloc(p->data, n * sizeof(p->data[0]));
        if (pnew == NULL) return ENOMEM;

        p->data = pnew;
        p->capacity = n;
    }
    return 0;
}

int objects_push_back(struct objects* p, const struct object* book)
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

        int error = objects_reserve(p, new_capacity);
        if (error != 0)
        {
            return error;
        }
    }

    p->data[p->size] = *book; /*COPIED*/


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


static struct object make_object_core(struct type* p_type, struct object_name_list* list, int* p_deep, struct declarator* declarator)
{
    struct object obj = {0};
    obj.declarator = declarator;

    if (p_type->struct_or_union_specifier)
    {
        struct struct_or_union_specifier* p_struct_or_union_specifier =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        if (p_struct_or_union_specifier)
        {
            obj.state = OBJECT_STATE_STRUCT;

            struct member_declaration* p_member_declaration =
                p_struct_or_union_specifier->member_declaration_list.head;

            struct object_name_list l = {0};
            l.name = p_struct_or_union_specifier->tag_name;
            l.previous = list;
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
                                member_obj.state = OBJECT_STATE_UNINITIALIZED;
                                objects_push_back(&obj.members, &member_obj);
                            }
                            else
                            {
                                struct object member_obj = make_object_core(&p_member_declarator->declarator->type, &l, p_deep, declarator);
                                objects_push_back(&obj.members, &member_obj);
                            }

                            member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
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
        obj.state = OBJECT_STATE_UNINITIALIZED;

        if (*p_deep < 2)
        {
            struct type t2 = type_remove_pointer(p_type);
            struct object* owner p_object = calloc(1, sizeof * p_object);

            *p_object = move make_object_core(&t2, list, p_deep, declarator);
            obj.pointed = move p_object;

            type_destroy(&t2);
            (*p_deep)++;
        }
    }
    else
    {
        //assert(p_object->members_size == 0);
        //p_object->state = flags;
        obj.state = OBJECT_STATE_UNINITIALIZED;
    }
    obj.declarator = declarator;
    return obj;
}

static struct object make_object(struct type* p_type, struct declarator* declarator)
{
    assert(declarator);
    int deep = 0;
    struct object_name_list list = {.name = ""};
    return make_object_core(p_type, &list, &deep, declarator);
}

static void print_object(int ident, struct type* p_type, struct object* p_object, const char* previous_names)
{
    if (p_object == NULL)
    {
        return;
    }

    if (p_type->struct_or_union_specifier)
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
                            char* name = p_member_declarator->declarator->name ? p_member_declarator->declarator->name->lexeme : "";
                            char buffer[200] = {0};
                            if (type_is_pointer(p_type))
                                snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                            else
                                snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);


                            print_object(ident + 1, &p_member_declarator->declarator->type,
                                &p_object->members.data[member_index], buffer);

                            member_index++;
                        }
                        p_member_declarator = p_member_declarator->next;
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
        struct type t2 = type_remove_pointer(p_type);
        printf("%*c", ident, ' ');
        if (p_object)
        {
            printf("%s %s\n", previous_names, object_state_to_string(p_object->state));
            if (p_object->pointed)
            {
                char buffer[200] = {0};
                snprintf(buffer, sizeof buffer, "*(%s)", previous_names);


                print_object(ident + 1, &t2, p_object->pointed, buffer);
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
            printf("%s %s\n", previous_names, object_state_to_string(p_object->state));
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
        assert(p_object->members.size == 0);
        p_object->state = flags;
    }
}

void visit_object(struct parser_ctx* ctx,
    struct type* p_type,
    struct object* p_object,
    struct token* position_token,
    const char* previous_names)
{
    if (p_object == NULL)
    {
        return;
    }
    if (p_type->type_qualifier_flags & TYPE_QUALIFIER_VIEW)
    {
        return;
    }

    if (!(p_type->type_qualifier_flags & (TYPE_QUALIFIER_OWNER | TYPE_QUALIFIER_OBJ_OWNER)))
    {
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
                        char* name = p_member_declarator->declarator->name ? p_member_declarator->declarator->name->lexeme : "";
                        char buffer[200] = {0};
                        if (type_is_pointer(p_type))
                            snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
                        else
                            snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);

                        visit_object(ctx, &p_member_declarator->declarator->type,
                            &p_object->members.data[member_index],
                            position_token,
                            buffer);

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
        const char* name = previous_names;



        if (type_is_pointer(p_type))
        {
            if (p_object->state == OBJECT_STATE_UNKNOWN ||
                p_object->state == OBJECT_STATE_NOT_ZERO)
            {
                char buffer[100];
                snprintf(buffer, sizeof buffer, "%s", previous_names);

                struct type t2 = type_remove_pointer(p_type);
                if (t2.type_qualifier_flags & TYPE_QUALIFIER_OWNER)
                {
                    visit_object(ctx,
                        &t2,
                        p_object->pointed,
                        position_token,
                        buffer);
                }
                type_destroy(&t2);
            }

        }
        const bool move_not_necessary =
            (p_object->state == OBJECT_STATE_ZERO && type_is_pointer(p_type)) ||
            (p_object->state == OBJECT_STATE_UNINITIALIZED) ||
            (p_object->state == OBJECT_STATE_MOVED) ||
            (p_object->state == OBJECT_STATE_NULL_OR_UNINITIALIZED && type_is_pointer(p_type));

        const bool move_necessary = !move_not_necessary;


        if (!(p_type->type_qualifier_flags & TYPE_QUALIFIER_OBJ_OWNER) &&
            move_necessary)
        {
            compiler_set_error_with_token(C_DESTRUCTOR_MUST_BE_CALLED_BEFORE_END_OF_SCOPE,
                ctx,
                p_object->declarator->name,
                "object '%s' was not moved/destroyed",
                name);

            if (p_object->declarator)
                compiler_set_info_with_token(W_NONE, ctx, position_token, "end of '%s' scope", name);

        }
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
            const char* name = p_declarator->name ? p_declarator->name->lexeme : "";

            visit_object(ctx->ctx, &p_declarator->type, &p_declarator->object, position_token, name);

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
    else if (deferblock->p_selection_statement2)
    {
        if (deferblock->p_selection_statement2->secondary_block)
        {
            if (flow_find_label_statement(ctx, deferblock->p_selection_statement2->secondary_block->statement, label))
                return true;
        }

        if (deferblock->p_selection_statement2->else_secondary_block_opt)
        {
            if (flow_find_label_statement(ctx, deferblock->p_selection_statement2->else_secondary_block_opt->statement, label))
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


static bool set_all_variables(struct flow_visit_ctx* ctx,
    struct flow_defer_scope* deferblock,
    enum object_state state)
{
    bool found_error = false;

    struct flow_defer_scope* deferchild = deferblock->last_child;
    while (deferchild != NULL)
    {
        if (deferchild->declarator)
        {
            struct declarator* p_declarator = deferchild->declarator;
            set_object(&p_declarator->type, &p_declarator->object, state);
        }
        deferchild = deferchild->previous;
    }
    return found_error;
}

static void set_all_until_end(struct flow_visit_ctx* ctx, struct flow_defer_scope* deferblock, enum object_state state)
{

    struct flow_defer_scope* p_defer = deferblock;
    while (p_defer != NULL)
    {
        set_all_variables(ctx, p_defer, state);
        p_defer = p_defer->previous;
    }

}

static void flow_visit_secondary_block(struct flow_visit_ctx* ctx, struct secondary_block* p_secondary_block)
{
    flow_visit_statement(ctx, p_secondary_block->statement);
}

static void flow_visit_defer_statement(struct flow_visit_ctx* ctx, struct defer_statement* p_defer_statement)
{

    struct flow_defer_scope* owner p_defer = calloc(1, sizeof * p_defer);
    p_defer->defer_statement = p_defer_statement;
    p_defer->previous = ctx->tail_block->last_child;
    ctx->tail_block->last_child = move p_defer;

    /*
      We are not going to visit the secondary block here because
      this is not the place were defer is executed.
    */
}

static void flow_visit_try_statement(struct flow_visit_ctx* ctx, struct try_statement* p_try_statement)
{
    struct flow_defer_scope* owner p_defer = calloc(1, sizeof * p_defer);
    p_defer->previous = ctx->tail_block;
    ctx->tail_block = move p_defer;
    p_defer->p_try_statement = p_try_statement;

    if (p_try_statement->secondary_block)
        flow_visit_secondary_block(ctx, p_try_statement->secondary_block);

    check_defer_and_variables(ctx, p_defer, p_try_statement->secondary_block->last_token);

    if (ctx->tail_block)
    {
        //POP        
        ctx->tail_block = move ctx->tail_block->previous;
    }
}

static struct object* expression_is_comparing_owner_with_null(struct expression* p_expression)
{
    struct object* p_object = NULL;
    if (p_expression->expression_type == PRIMARY_EXPRESSION_DECLARATOR)
    {
        if (type_is_pointer(&p_expression->type))
        {
            if (p_expression->type.type_qualifier_flags & TYPE_QUALIFIER_OWNER)
            {
                struct type type = {0};
                p_object = expression_get_object(p_expression, &type);
                type_destroy(&type);
            }
        }
    }
    else if (p_expression->expression_type == EQUALITY_EXPRESSION_EQUAL)
    {
        if (p_expression->left->expression_type == PRIMARY_EXPRESSION_DECLARATOR)
        {
            if (type_is_pointer(&p_expression->left->type) &&
                p_expression->left->type.type_qualifier_flags & TYPE_QUALIFIER_OWNER)
            {
                if (is_null_pointer_constant(p_expression->right))
                {
                    struct type type = {0};
                    p_object = expression_get_object(p_expression->left, &type);
                    type_destroy(&type);
                }
            }
        }
    }
    return p_object;
}

static void flow_visit_selection_statement(struct flow_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{
    /*
       COMMON PATTERN

       //owner is not moved here
       if (owner)
       {
          //goto
          //label
          //break;
          //no address of &owner
          // no p->owner
          // &p
       } //owner is moved here


    */

    //PUSH
    struct flow_defer_scope* owner p_defer = calloc(1, sizeof * p_defer);
    p_defer->previous = ctx->tail_block;
    ctx->tail_block = move p_defer;
    p_defer->p_selection_statement2 = p_selection_statement;



    struct object* p_object = NULL;


    if (p_selection_statement->expression)
    {
        p_object = expression_is_comparing_owner_with_null(p_selection_statement->expression);
        if (p_object)
        {
            struct type type = {0};
            type_destroy(&type);
        }
    }



    bool copy = ctx->has_jumps;
    ctx->has_jumps = false;
    if (p_selection_statement->secondary_block)
        flow_visit_secondary_block(ctx, p_selection_statement->secondary_block);


    check_defer_and_variables(ctx, p_defer, p_selection_statement->secondary_block->last_token);

    if (p_selection_statement->else_secondary_block_opt)
        flow_visit_secondary_block(ctx, p_selection_statement->else_secondary_block_opt);


    if (ctx->has_jumps)
    {
        p_object = NULL;
    }


    ctx->has_jumps = copy || ctx->has_jumps; //restore

    //POP
    ctx->tail_block = ctx->tail_block->previous;



    enum object_state state2 = 0;
    if (p_object)
    {
        state2 = p_object->state;
    }

    set_all_until_end(ctx, ctx->tail_block, OBJECT_STATE_UNKNOWN);

    if (p_object)
    {
        if (state2 == OBJECT_STATE_UNINITIALIZED ||
            state2 == OBJECT_STATE_MOVED)
        {
            p_object->state = OBJECT_STATE_NULL_OR_UNINITIALIZED;
        }
    }


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
        if (p_current_parameter_type->type.type_qualifier_flags & TYPE_QUALIFIER_OWNER ||
            p_current_parameter_type->type.type_qualifier_flags & TYPE_QUALIFIER_OBJ_OWNER)
        {
            struct type argument_object_type = {0};
            struct object* p_argument_object =
                expression_get_object(p_current_argument->expression, &argument_object_type);

            if (p_argument_object)
            {
                set_object(&argument_object_type, p_argument_object, OBJECT_STATE_UNINITIALIZED);
                if (p_argument_object->pointed)
                {
                    if (type_is_void_ptr(&p_current_parameter_type->type))
                    {
                        /* moving a owner pointer to a void* will not move the object */
                    }
                    else
                    {
                        /* moving a owner pointer to non void* moves the pointed object too*/
                        if (p_argument_object->pointed)
                        {
                            p_argument_object->pointed->state = OBJECT_STATE_MOVED;
                        }
                    }
                }
            }
        }

        p_current_argument = p_current_argument->next;
        p_current_parameter_type = p_current_parameter_type->next;
        param_num++;
    }

    return 0;
}

static void flow_visit_expression(struct flow_visit_ctx* ctx, struct expression* p_expression)
{
    switch (p_expression->expression_type)
    {
        case PRIMARY_EXPRESSION__FUNC__:
            break;
        case PRIMARY_IDENTIFIER:
            break;
        case PRIMARY_EXPRESSION_ENUMERATOR:

            break;
        case PRIMARY_EXPRESSION_DECLARATOR:

            if (p_expression->declarator->object.state == OBJECT_STATE_UNINITIALIZED)
            {
                //TODO inside sizeof(v)  is not an error. :D
                //TODO function type...
#if 0
                compiler_set_warning_with_token(W_UNINITIALZED,
                    ctx->ctx,
                    p_expression->first_token,
                    "'%s' is uninitialized ",
                    p_expression->declarator->name->lexeme);
#endif
            }

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
            //visit_expression(ctx, p_expression->left);
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

        case UNARY_EXPRESSION_STATIC_DEBUG:
        {
            compiler_set_info_with_token(W_NONE, ctx->ctx, p_expression->first_token, "");
            print_object(1, &p_expression->right->declarator->type,
                &p_expression->right->declarator->object,
                p_expression->right->declarator->name->lexeme);
        }
        break;


        case ASSIGNMENT_EXPRESSION:

            flow_visit_expression(ctx, p_expression->left);
            flow_visit_expression(ctx, p_expression->right);
            if (type_is_owner(&p_expression->left->type))
            {
                struct type right_object_type = {0};
                struct object* const p_right_object = expression_get_object(p_expression->right, &right_object_type);
                if (p_right_object)
                {
                    set_object(&right_object_type, p_right_object, OBJECT_STATE_MOVED);

                    if (p_right_object->pointed)
                    {
                        if (type_is_void_ptr(&p_expression->left->type))
                        {
                            /*
                              void * _Owner p;
                              p = _Move p_struct_x;
                            */
                        }
                        else
                        {
                            /* moving a owner pointer to non void* moves the pointed object too*/
                            p_right_object->pointed->state = OBJECT_STATE_MOVED;

                        }
                    }
                }

                struct type left_object_type = {0};
                struct object* const p_left_object = expression_get_object(p_expression->left, &left_object_type);

                if (p_left_object)
                {
                    if (constant_value_is_valid(&p_expression->right->constant_value))
                    {
                        if (constant_value_to_ull(&p_expression->right->constant_value) == 0)
                        {
                            set_object(&left_object_type, p_left_object, OBJECT_STATE_ZERO);
                        }
                        else
                        {
                            set_object(&left_object_type, p_left_object, OBJECT_STATE_UNKNOWN);
                        }
                    }
                    else
                    {
                        set_object(&left_object_type, p_left_object, OBJECT_STATE_UNKNOWN);
                    }
                }
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
        case EQUALITY_EXPRESSION_NOT_EQUAL:
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
    flow_visit_block_item_list(ctx, &p_compound_statement->block_item_list);


}

//
static void flow_visit_iteration_statement(struct flow_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{


    if (p_iteration_statement->declaration)
    {
        flow_visit_declaration(ctx, p_iteration_statement->declaration);
    }

    if (p_iteration_statement->expression1)
    {
        flow_visit_expression(ctx, p_iteration_statement->expression1);
    }

    if (p_iteration_statement->expression2)
    {
        flow_visit_expression(ctx, p_iteration_statement->expression2);
    }


    if (p_iteration_statement->secondary_block)
    {
        struct flow_defer_scope* owner p_defer = calloc(1, sizeof * p_defer);
        p_defer->previous = ctx->tail_block;
        ctx->tail_block = move p_defer;
        p_defer->p_iteration_statement = p_iteration_statement;

        flow_visit_secondary_block(ctx, p_iteration_statement->secondary_block);

        check_defer_and_variables(ctx, p_defer, p_iteration_statement->secondary_block->last_token);


        if (ctx->tail_block)
        {
            //POP
            ctx->tail_block = ctx->tail_block->previous;
        }


    }
}



static void flow_visit_jump_statement(struct flow_visit_ctx* ctx, struct jump_statement* p_jump_statement)
{

    ctx->has_jumps = true;

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
            if (p_object)
            {
                set_object(&p_jump_statement->expression_opt->type, p_object, OBJECT_STATE_MOVED);
                if (p_object->pointed)
                {
                    if (type_is_void_ptr(ctx->p_return_type))
                    {
                        /* moving a owner pointer to a void* will not move the object */
                    }
                    else
                    {
                        /* moving a owner pointer to non void* moves the pointed object too*/
                        p_object->pointed->state = OBJECT_STATE_MOVED;
                    }
                }
            }
        }
        check_all_defer_until_end(ctx, ctx->tail_block, p_jump_statement->first_token);
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_BREAK ||
        p_jump_statement->first_token->type == TK_KEYWORD_CONTINUE)
    {
        check_all_defer_until_iter(ctx, ctx->tail_block, p_jump_statement->first_token);
    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_GOTO)
    {
        check_all_defer_until_label(ctx, ctx->tail_block, p_jump_statement->label->lexeme, p_jump_statement->first_token);

        set_all_until_end(ctx, ctx->tail_block, OBJECT_STATE_UNKNOWN);
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
    //p_label->name
    set_all_until_end(ctx, ctx->tail_block, OBJECT_STATE_UNKNOWN);
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
    struct block_item* p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        flow_visit_block_item(ctx, p_block_item);
        p_block_item = p_block_item->next;
    }
}

static void flow_visit_static_assert_declaration(struct flow_visit_ctx* ctx, struct static_assert_declaration* p_static_assert_declaration)
{
    flow_visit_expression(ctx, p_static_assert_declaration->constant_expression);
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
        p_declarator->type.category != TYPE_CATEGORY_FUNCTION &&
        p_declarator->name != NULL)
    {

        struct flow_defer_scope* owner p_defer = calloc(1, sizeof * p_defer);
        p_defer->declarator = p_declarator;
        p_defer->previous = ctx->tail_block->last_child;
        ctx->tail_block->last_child = move p_defer;

        p_declarator->object = move make_object(&p_declarator->type, p_declarator);
        //print_object(0, &p_declarator->type, &p_declarator->object, p_declarator->name->lexeme);

        if (p_declarator->declaration_specifiers->storage_class_specifier_flags & STORAGE_SPECIFIER_PARAMETER)
        {
            set_object(&p_declarator->type, &p_declarator->object, OBJECT_STATE_UNKNOWN);

            if (type_is_pointer(&p_declarator->type))
            {
                struct type t2 = type_remove_pointer(&p_declarator->type);

                struct object* owner p0 = move calloc(1, sizeof * p0);
                *p0 = move make_object(&t2, p_declarator);
                p_declarator->object.pointed = move p0;
                set_object(&t2, p0, OBJECT_STATE_UNKNOWN);

                type_destroy(&t2);
            }
        }
    }

    if (p_declarator->pointer)
    {
        struct pointer* p = p_declarator->pointer;
        while (p)
        {
            p = p->pointer;
        }
    }


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
                }
            }
        }

        if (p_init_declarator->p_declarator->type.category != TYPE_CATEGORY_FUNCTION)
        {
            //int members = type_get_num_members(&p_init_declarator->p_declarator->type);

            //assert(p_init_declarator->p_declarator->object.state == NULL);
            //p_init_declarator->p_declarator->object.state = move calloc(members, sizeof(enum object_state));
            //p_init_declarator->p_declarator->object.size = members;

            if (p_init_declarator->initializer &&
                p_init_declarator->initializer->assignment_expression)
            {
                set_object(&p_init_declarator->p_declarator->type,
                    &p_init_declarator->p_declarator->object,
                    OBJECT_STATE_UNKNOWN);

                if (p_init_declarator->initializer->assignment_expression->type.type_qualifier_flags & TYPE_QUALIFIER_OWNER)
                {
                    /*
                       If the right side is owner that means it is moved
                    */
                    struct type right_object_type = {0};
                    struct object* p_right_object =
                        expression_get_object(p_init_declarator->initializer->assignment_expression, &right_object_type);

                    if (p_right_object)
                    {
                        set_object(&right_object_type, p_right_object, OBJECT_STATE_MOVED);
                    }
                }
            }
            else  if (p_init_declarator->initializer &&
                p_init_declarator->initializer->braced_initializer)
            {
                set_object(&p_init_declarator->p_declarator->type,
                    &p_init_declarator->p_declarator->object,
                    OBJECT_STATE_ZERO);
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

//struct enumerator_list* enumerator_list;
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
        assert(ctx->has_jumps == false);
        struct type type = get_function_return_type(&p_declaration->init_declarator_list.head->p_declarator->type);
        ctx->p_return_type = &type;
        flow_visit_compound_statement(ctx, p_declaration->function_body);
        type_destroy(&type);
        ctx->p_return_type = NULL;
        ctx->has_jumps = false;
    }

}

void flow_visit_function(struct flow_visit_ctx* ctx, struct declaration* p_declaration)
{
    assert(p_declaration->function_body);

    assert(ctx->tail_block == NULL);
    struct flow_defer_scope* owner p_defer = calloc(1, sizeof * p_defer);
    ctx->tail_block = move p_defer;
    p_defer->p_function_body = p_declaration->function_body;



    flow_visit_declaration(ctx, p_declaration);


    if (!flow_is_last_item_return(p_declaration->function_body))
    {
        check_defer_and_variables(ctx, p_defer, p_declaration->function_body->last_token);
    }


    /*delete all defer memory*/
    struct flow_defer_scope* owner p_block = move ctx->tail_block;
    while (p_block != NULL)
    {
        struct flow_defer_scope* owner deferchild = move p_block->last_child;
        while (deferchild != NULL)
        {
            struct flow_defer_scope* owner prev = move deferchild->previous;
            free(deferchild);
            deferchild = move prev;
        }


        struct flow_defer_scope* owner prev_block = move p_block->previous;
        free(p_block);
        p_block = move prev_block;
    }


    ctx->tail_block = NULL;
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

