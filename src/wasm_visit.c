#include "wasm_visit.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "expressions.h"
#include "ownership.h"

static void wasm_visit_attribute_specifier_sequence(struct wasm_visit_ctx* ctx, struct attribute_specifier_sequence* p_visit_attribute_specifier_sequence);


static void wasm_visit_struct_or_union_specifier(struct wasm_visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier);
static void wasm_visit_expression(struct wasm_visit_ctx* ctx, struct expression* p_expression);
static void wasm_visit_statement(struct wasm_visit_ctx* ctx, struct statement* p_statement);
static void wasm_visit_enum_specifier(struct wasm_visit_ctx* ctx, struct enum_specifier* p_enum_specifier);
static void wasm_visit_type_specifier(struct wasm_visit_ctx* ctx, struct type_specifier* p_type_specifier);


static void wasm_visit_secondary_block(struct wasm_visit_ctx* ctx, struct secondary_block* p_secondary_block)
{
    wasm_visit_statement(ctx, p_secondary_block->statement);
}

static void wasm_visit_defer_statement(struct wasm_visit_ctx* ctx, struct defer_statement* p_defer_statement)
{






    if (p_defer_statement->secondary_block)
    {
        wasm_visit_secondary_block(ctx, p_defer_statement->secondary_block);
    }



}

static void wasm_visit_try_statement(struct wasm_visit_ctx* ctx, struct try_statement* p_try_statement)
{



    if (p_try_statement->secondary_block)
        wasm_visit_secondary_block(ctx, p_try_statement->secondary_block);




}

static void wasm_visit_selection_statement(struct wasm_visit_ctx* ctx, struct selection_statement* p_selection_statement)
{

    if (p_selection_statement->else_secondary_block_opt)
        wasm_visit_secondary_block(ctx, p_selection_statement->else_secondary_block_opt);


}

static void wasm_visit_compound_statement(struct wasm_visit_ctx* ctx, struct compound_statement* p_compound_statement);



static void wasm_visit_initializer_list(struct wasm_visit_ctx* ctx, struct initializer_list* p_initializer_list);

static void wasm_visit_bracket_initializer_list(struct wasm_visit_ctx* ctx, struct braced_initializer* p_bracket_initializer_list)
{
    if (p_bracket_initializer_list->initializer_list == NULL)
    {

    }
    else
    {
        wasm_visit_initializer_list(ctx, p_bracket_initializer_list->initializer_list);
    }
}

static void wasm_visit_designation(struct wasm_visit_ctx* ctx, struct designation* p_designation)
{
}

static void wasm_visit_initializer(struct wasm_visit_ctx* ctx, struct initializer* p_initializer)
{
    if (p_initializer->designation)
    {
        wasm_visit_designation(ctx, p_initializer->designation);
    }

    if (p_initializer->assignment_expression)
    {
        wasm_visit_expression(ctx, p_initializer->assignment_expression);
    }
    else if (p_initializer->braced_initializer)
    {
        wasm_visit_bracket_initializer_list(ctx, p_initializer->braced_initializer);
    }
}

static void wasm_visit_initializer_list(struct wasm_visit_ctx* ctx, struct initializer_list* p_initializer_list)
{
    struct initializer* p_initializer = p_initializer_list->head;
    while (p_initializer)
    {
        wasm_visit_initializer(ctx, p_initializer);
        p_initializer = p_initializer->next;
    }
}

static void wasm_visit_type_qualifier(struct wasm_visit_ctx* ctx, struct type_qualifier* p_type_qualifier)
{
    if (p_type_qualifier->token &&
        p_type_qualifier->token->type == TK_KEYWORD_RESTRICT)
    {

    }
}

static void wasm_visit_specifier_qualifier(struct wasm_visit_ctx* ctx, struct type_specifier_qualifier* p_specifier_qualifier)
{
    if (p_specifier_qualifier->type_specifier)
        wasm_visit_type_specifier(ctx, p_specifier_qualifier->type_specifier);

    if (p_specifier_qualifier->type_qualifier)
        wasm_visit_type_qualifier(ctx, p_specifier_qualifier->type_qualifier);
}

static void wasm_visit_specifier_qualifier_list(struct wasm_visit_ctx* ctx, struct specifier_qualifier_list* p_specifier_qualifier_list_opt)
{
    if (p_specifier_qualifier_list_opt == NULL)
        return;

    if (p_specifier_qualifier_list_opt->struct_or_union_specifier)
    {
        wasm_visit_struct_or_union_specifier(ctx, p_specifier_qualifier_list_opt->struct_or_union_specifier);
    }
    else if (p_specifier_qualifier_list_opt->enum_specifier)
    {
        wasm_visit_enum_specifier(ctx, p_specifier_qualifier_list_opt->enum_specifier);
    }
    else if (p_specifier_qualifier_list_opt->typedef_declarator)
    {
        //typedef name
    }
    //else if (p_specifier_qualifier_list->p_typeof_expression_opt)
    //{
      //  wasm_visit_expression(ctx, p_specifier_qualifier_list->p_typeof_expression_opt);
    //}
    else
    {
        struct type_specifier_qualifier* p_specifier_qualifier = p_specifier_qualifier_list_opt->head;
        while (p_specifier_qualifier)
        {
            wasm_visit_specifier_qualifier(ctx, p_specifier_qualifier);
            p_specifier_qualifier = p_specifier_qualifier->next;
        }
    }
}
static void wasm_visit_declarator(struct wasm_visit_ctx* ctx, struct declarator* p_declarator);
static void wasm_visit_type_name(struct wasm_visit_ctx* ctx, struct type_name* p_type_name)
{
    wasm_visit_specifier_qualifier_list(ctx, p_type_name->specifier_qualifier_list);
    wasm_visit_declarator(ctx, p_type_name->declarator);
}


static void wasm_visit_argument_expression_list(struct wasm_visit_ctx* ctx, struct argument_expression_list* p_argument_expression_list)
{
    struct argument_expression* p_argument_expression =
        p_argument_expression_list->head;
    while (p_argument_expression)
    {
        wasm_visit_expression(ctx, p_argument_expression->expression);
        p_argument_expression = p_argument_expression->next;
    }
}

static void wasm_visit_generic_selection(struct wasm_visit_ctx* ctx, struct generic_selection* p_generic_selection)
{
    if (p_generic_selection->expression)
    {
        wasm_visit_expression(ctx, p_generic_selection->expression);
    }
    else if (p_generic_selection->type_name)
    {
        wasm_visit_type_name(ctx, p_generic_selection->type_name);
    }



}


static void wasm_visit_expression(struct wasm_visit_ctx* ctx, struct expression* p_expression)
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
        //p_expression->declarator->name    
        break;
    case PRIMARY_EXPRESSION_STRING_LITERAL:
        break;
    case PRIMARY_EXPRESSION_CHAR_LITERAL:
        break;
    case PRIMARY_EXPRESSION_NUMBER:
        break;

    case PRIMARY_EXPRESSION_PREDEFINED_CONSTANT:
        if (p_expression->first_token &&
            p_expression->first_token->type == TK_KEYWORD_NULLPTR)
        {

        }
        else if (p_expression->first_token &&
            p_expression->first_token->type == TK_KEYWORD_TRUE)
        {

        }
        else if (p_expression->first_token &&
            p_expression->first_token->type == TK_KEYWORD_FALSE)
        {

        }
        break;

    case PRIMARY_EXPRESSION_GENERIC:
        wasm_visit_generic_selection(ctx, p_expression->generic_selection);
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
        wasm_visit_expression(ctx, p_expression->left);
        wasm_visit_argument_expression_list(ctx, &p_expression->argument_expression_list);
        break;
    case POSTFIX_EXPRESSION_FUNCTION_LITERAL:
    {

        wasm_visit_type_name(ctx, p_expression->type_name);
        wasm_visit_compound_statement(ctx, p_expression->compound_statement);


    }
    break;

    case POSTFIX_EXPRESSION_COMPOUND_LITERAL:

        if (p_expression->type_name)
        {
            wasm_visit_type_name(ctx, p_expression->type_name);
        }

        //struct token_list list0 = tokenizer("int teste = 0;", NULL, 0, TK_FLAG_NONE);
        //token_list_append_list(&ctx->insert_before_block_item, &list0);

        wasm_visit_bracket_initializer_list(ctx, p_expression->braced_initializer);

        assert(p_expression->left == NULL);
        assert(p_expression->right == NULL);

        break;

    case UNARY_EXPRESSION_ALIGNOF:


        if (p_expression->right)
        {
            wasm_visit_expression(ctx, p_expression->right);
        }

        if (p_expression->type_name)
        {
            /*sizeof*/
            wasm_visit_type_name(ctx, p_expression->type_name);
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
            wasm_visit_expression(ctx, p_expression->right);
        }

        if (p_expression->type_name)
        {
            /*sizeof*/
            wasm_visit_type_name(ctx, p_expression->type_name);
        }

        break;
    
    case UNARY_DECLARATOR_ATTRIBUTE_EXPR:
        break;

    
    case CAST_EXPRESSION:
    case ASSIGNMENT_EXPRESSION:
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
            wasm_visit_expression(ctx, p_expression->left);
        }
        if (p_expression->right)
        {
            wasm_visit_expression(ctx, p_expression->right);
        }
        if (p_expression->type_name)
        {
            wasm_visit_type_name(ctx, p_expression->type_name);
        }
        break;

    case CONDITIONAL_EXPRESSION:
        if (p_expression->left)
        {
            wasm_visit_expression(ctx, p_expression->left);
        }
        if (p_expression->right)
        {
            wasm_visit_expression(ctx, p_expression->right);
        }
        if (p_expression->condition_expr)
        {
            wasm_visit_expression(ctx, p_expression->condition_expr);
        }
        break;
    case UNARY_EXPRESSION_TRAITS:
    {

    }
    break;

    case UNARY_EXPRESSION_IS_SAME:
        break;

    default:
        break;
    }
}

static void wasm_visit_expression_statement(struct wasm_visit_ctx* ctx, struct expression_statement* p_expression_statement)
{
    if (p_expression_statement->expression_opt)
        wasm_visit_expression(ctx, p_expression_statement->expression_opt);
}

static void wasm_visit_block_item_list(struct wasm_visit_ctx* ctx, struct block_item_list* p_block_item_list);

static void wasm_visit_compound_statement(struct wasm_visit_ctx* ctx, struct compound_statement* p_compound_statement)
{
    wasm_visit_block_item_list(ctx, &p_compound_statement->block_item_list);
}

//
static void wasm_visit_iteration_statement(struct wasm_visit_ctx* ctx, struct iteration_statement* p_iteration_statement)
{

    if (p_iteration_statement->expression1)
    {
        wasm_visit_expression(ctx, p_iteration_statement->expression1);
    }

    if (p_iteration_statement->expression2)
    {
        wasm_visit_expression(ctx, p_iteration_statement->expression2);
    }

    if (p_iteration_statement->secondary_block)
    {

        wasm_visit_secondary_block(ctx, p_iteration_statement->secondary_block);



    }
}



static void wasm_visit_jump_statement(struct wasm_visit_ctx* ctx, struct jump_statement* p_jump_statement)
{

    if (p_jump_statement->first_token->type == TK_KEYWORD_THROW)
    {

    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_RETURN)
    {

    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_BREAK ||
        p_jump_statement->first_token->type == TK_KEYWORD_CONTINUE)
    {

    }
    else if (p_jump_statement->first_token->type == TK_KEYWORD_GOTO)
    {

    }
    else
    {
        assert(false);
    }
}


static void wasm_visit_labeled_statement(struct wasm_visit_ctx* ctx, struct labeled_statement* p_labeled_statement)
{

    if (p_labeled_statement->statement)
    {
        wasm_visit_statement(ctx, p_labeled_statement->statement);
    }
}

static void wasm_visit_primary_block(struct wasm_visit_ctx* ctx, struct primary_block* p_primary_block)
{



    if (p_primary_block->defer_statement)
    {
        wasm_visit_defer_statement(ctx, p_primary_block->defer_statement);
    }
    else
    {
        if (p_primary_block->compound_statement)
        {
            wasm_visit_compound_statement(ctx, p_primary_block->compound_statement);
        }
        else if (p_primary_block->iteration_statement)
        {
            wasm_visit_iteration_statement(ctx, p_primary_block->iteration_statement);
        }
        else if (p_primary_block->selection_statement)
        {
            wasm_visit_selection_statement(ctx, p_primary_block->selection_statement);
        }
        else if (p_primary_block->try_statement)
        {
            wasm_visit_try_statement(ctx, p_primary_block->try_statement);
        }
    }

}

static void wasm_visit_unlabeled_statement(struct wasm_visit_ctx* ctx, struct unlabeled_statement* p_unlabeled_statement)
{
    if (p_unlabeled_statement->primary_block)
    {
        wasm_visit_primary_block(ctx, p_unlabeled_statement->primary_block);
    }
    else if (p_unlabeled_statement->expression_statement)
    {
        wasm_visit_expression_statement(ctx, p_unlabeled_statement->expression_statement);
    }
    else if (p_unlabeled_statement->jump_statement)
    {
        wasm_visit_jump_statement(ctx, p_unlabeled_statement->jump_statement);
    }
    else
    {
        assert(false);
    }
}

static void wasm_visit_declaration(struct wasm_visit_ctx* ctx, struct declaration* p_declaration);

static void wasm_visit_statement(struct wasm_visit_ctx* ctx, struct statement* p_statement)
{
    if (p_statement->labeled_statement)
    {
        wasm_visit_labeled_statement(ctx, p_statement->labeled_statement);
    }
    else if (p_statement->unlabeled_statement)
    {
        wasm_visit_unlabeled_statement(ctx, p_statement->unlabeled_statement);
    }
}

static void wasm_visit_label(struct wasm_visit_ctx* ctx, struct label* p_label)
{
    //p_label->name
}
static void wasm_visit_block_item(struct wasm_visit_ctx* ctx, struct block_item* p_block_item)
{
    if (p_block_item->declaration)
    {
        wasm_visit_declaration(ctx, p_block_item->declaration);
    }
    else if (p_block_item->unlabeled_statement)
    {
        wasm_visit_unlabeled_statement(ctx, p_block_item->unlabeled_statement);
    }
    else if (p_block_item->label)
    {
        wasm_visit_label(ctx, p_block_item->label);
    }

}

static void wasm_visit_block_item_list(struct wasm_visit_ctx* ctx, struct block_item_list* p_block_item_list)
{
    struct block_item* p_block_item = p_block_item_list->head;
    while (p_block_item)
    {
        wasm_visit_block_item(ctx, p_block_item);
        p_block_item = p_block_item->next;
    }
}

static void wasm_visit_static_assert_declaration(struct wasm_visit_ctx* ctx, struct static_assert_declaration* p_static_assert_declaration)
{
    wasm_visit_expression(ctx, p_static_assert_declaration->constant_expression);


}
static void wasm_visit_declaration_specifiers(struct wasm_visit_ctx* ctx, struct declaration_specifiers* p_declaration_specifiers);


static void wasm_visit_direct_declarator(struct wasm_visit_ctx* ctx, struct direct_declarator* p_direct_declarator)
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
            ss_fprintf(&ctx->ss, "(param ");

            ss_fprintf(&ctx->ss, " i32"); //todo type


            if (parameter->declarator->name)
                ss_fprintf(&ctx->ss, " %%%s", parameter->declarator->name->lexeme);


            if (parameter->attribute_specifier_sequence_opt)
            {
                wasm_visit_attribute_specifier_sequence(ctx, parameter->attribute_specifier_sequence_opt);
            }

            wasm_visit_declaration_specifiers(ctx, parameter->declaration_specifiers);
            wasm_visit_declarator(ctx, parameter->declarator);
            parameter = parameter->next;
            ss_fprintf(&ctx->ss, ")");
        }

    }
    else if (p_direct_declarator->array_declarator)
    {

    }
}

static void wasm_visit_declarator(struct wasm_visit_ctx* ctx, struct declarator* p_declarator)
{
    if (p_declarator->direct_declarator)
    {
        wasm_visit_direct_declarator(ctx, p_declarator->direct_declarator);
    }
}

static void wasm_visit_init_declarator_list(struct wasm_visit_ctx* ctx, struct init_declarator_list* p_init_declarator_list)
{
    struct init_declarator* p_init_declarator = p_init_declarator_list->head;



    while (p_init_declarator)
    {
        if (p_init_declarator->p_declarator)
        {
            wasm_visit_declarator(ctx, p_init_declarator->p_declarator);
        }

        if (p_init_declarator->initializer)
        {
            if (p_init_declarator->initializer->assignment_expression)
            {
                wasm_visit_expression(ctx, p_init_declarator->initializer->assignment_expression);
            }
            else
            {
                assert(p_init_declarator->initializer->braced_initializer != NULL);
                if (p_init_declarator->initializer->braced_initializer)
                {
                    wasm_visit_bracket_initializer_list(ctx,
                        p_init_declarator->initializer->braced_initializer);
                }

            }
        }

        p_init_declarator = p_init_declarator->next;
    }
}



static void wasm_visit_member_declarator(struct wasm_visit_ctx* ctx, struct member_declarator* p_member_declarator)
{
    if (p_member_declarator->declarator)
    {

    }
}

static void wasm_visit_member_declarator_list(struct wasm_visit_ctx* ctx, struct member_declarator_list* p_member_declarator_list)
{
    struct member_declarator* p_member_declarator = p_member_declarator_list->head;
    while (p_member_declarator)
    {
        wasm_visit_member_declarator(ctx, p_member_declarator);
        p_member_declarator = p_member_declarator->next;
    }
}
static void wasm_visit_member_declaration(struct wasm_visit_ctx* ctx, struct member_declaration* p_member_declaration)
{
    wasm_visit_specifier_qualifier_list(ctx, p_member_declaration->specifier_qualifier_list);

    if (p_member_declaration->member_declarator_list_opt)
    {
        wasm_visit_member_declarator_list(ctx, p_member_declaration->member_declarator_list_opt);
    }
}

static void wasm_visit_member_declaration_list(struct wasm_visit_ctx* ctx, struct member_declaration_list* p_member_declaration_list)
{
    struct member_declaration* p_member_declaration =
        p_member_declaration_list->head;
    while (p_member_declaration)
    {
        wasm_visit_member_declaration(ctx, p_member_declaration);
        p_member_declaration = p_member_declaration->next;
    }
}

static void wasm_visit_attribute_specifier(struct wasm_visit_ctx* ctx, struct attribute_specifier* p_attribute_specifier)
{

}

static void wasm_visit_attribute_specifier_sequence(struct wasm_visit_ctx* ctx, struct attribute_specifier_sequence* p_visit_attribute_specifier_sequence)
{
    struct attribute_specifier* current = p_visit_attribute_specifier_sequence->head;
    while (current)
    {
        wasm_visit_attribute_specifier(ctx, current);
        current = current->next;
    }
}

static void wasm_visit_struct_or_union_specifier(struct wasm_visit_ctx* ctx, struct struct_or_union_specifier* p_struct_or_union_specifier)
{

    if (p_struct_or_union_specifier->attribute_specifier_sequence_opt)
        wasm_visit_attribute_specifier_sequence(ctx, p_struct_or_union_specifier->attribute_specifier_sequence_opt);


    wasm_visit_member_declaration_list(ctx, &p_struct_or_union_specifier->member_declaration_list);

}

static void wasm_visit_enumerator(struct wasm_visit_ctx* ctx, struct enumerator* p_enumerator)
{
    if (p_enumerator->constant_expression_opt)
        wasm_visit_expression(ctx, p_enumerator->constant_expression_opt);

}

//struct enumerator_list* enumerator_list;
static void wasm_visit_enumerator_list(struct wasm_visit_ctx* ctx, struct enumerator_list* p_enumerator_list)
{
    struct enumerator* current = p_enumerator_list->head;
    while (current)
    {
        wasm_visit_enumerator(ctx, current);
        current = current->next;
    }
}

static void wasm_visit_enum_specifier(struct wasm_visit_ctx* ctx, struct enum_specifier* p_enum_specifier)
{
    

    if (p_enum_specifier->attribute_specifier_sequence_opt)
    {
        wasm_visit_attribute_specifier_sequence(ctx, p_enum_specifier->attribute_specifier_sequence_opt);
    }
    wasm_visit_enumerator_list(ctx, &p_enum_specifier->enumerator_list);

}

static void wasm_visit_typeof_specifier(struct wasm_visit_ctx* ctx, struct typeof_specifier* p_typeof_specifier)
{




}

static void wasm_visit_type_specifier(struct wasm_visit_ctx* ctx, struct type_specifier* p_type_specifier)
{
    if (p_type_specifier->typeof_specifier)
    {
        wasm_visit_typeof_specifier(ctx, p_type_specifier->typeof_specifier);
    }

    if (p_type_specifier->struct_or_union_specifier)
    {
        wasm_visit_struct_or_union_specifier(ctx, p_type_specifier->struct_or_union_specifier);
    }

    if (p_type_specifier->enum_specifier)
    {
        wasm_visit_enum_specifier(ctx, p_type_specifier->enum_specifier);
    }


    if (p_type_specifier->atomic_type_specifier)
    {
        //visit_deped(ctx, p_type_specifier->enum_specifier);
    }


}

static void wasm_visit_type_specifier_qualifier(struct wasm_visit_ctx* ctx, struct type_specifier_qualifier* p_type_specifier_qualifier)
{
    if (p_type_specifier_qualifier->type_qualifier)
    {
    }
    else if (p_type_specifier_qualifier->type_specifier)
    {
        wasm_visit_type_specifier(ctx, p_type_specifier_qualifier->type_specifier);
    }
    else if (p_type_specifier_qualifier->alignment_specifier)
    {
    }
}

static void wasm_visit_storage_class_specifier(struct wasm_visit_ctx* ctx, struct storage_class_specifier* p_storage_class_specifier)
{
    if (p_storage_class_specifier->flags & STORAGE_SPECIFIER_CONSTEXPR)
    {


    }
}

static void wasm_visit_declaration_specifier(struct wasm_visit_ctx* ctx, struct declaration_specifier* p_declaration_specifier)
{

    if (p_declaration_specifier->function_specifier)
    {
        if (p_declaration_specifier->function_specifier->token->type == TK_KEYWORD__NORETURN)
        {


        }
    }


    if (p_declaration_specifier->storage_class_specifier)
    {
        wasm_visit_storage_class_specifier(ctx, p_declaration_specifier->storage_class_specifier);

    }

    if (p_declaration_specifier->type_specifier_qualifier)
    {
        wasm_visit_type_specifier_qualifier(ctx, p_declaration_specifier->type_specifier_qualifier);

    }

}

static void wasm_visit_declaration_specifiers(struct wasm_visit_ctx* ctx, struct declaration_specifiers* p_declaration_specifiers)
{
    struct declaration_specifier* p_declaration_specifier = p_declaration_specifiers->head;
    while (p_declaration_specifier)
    {
        wasm_visit_declaration_specifier(ctx, p_declaration_specifier);
        p_declaration_specifier = p_declaration_specifier->next;
    }
}


static void wasm_visit_declaration(struct wasm_visit_ctx* ctx, struct declaration* p_declaration)
{
    if (p_declaration->static_assert_declaration)
    {
        wasm_visit_static_assert_declaration(ctx, p_declaration->static_assert_declaration);
    }

    if (p_declaration->p_attribute_specifier_sequence_opt)
    {
        wasm_visit_attribute_specifier_sequence(ctx, p_declaration->p_attribute_specifier_sequence_opt);
    }

    if (p_declaration &&
        p_declaration->init_declarator_list.head &&
        p_declaration->init_declarator_list.head->p_declarator)
    {
        if (type_is_function(&p_declaration->init_declarator_list.head->p_declarator->type))
        {
            ss_fprintf(&ctx->ss, "(func\n");
        }
    }

    if (p_declaration->declaration_specifiers)
    {
        wasm_visit_declaration_specifiers(ctx, p_declaration->declaration_specifiers);
    }

    if (p_declaration->p_attribute_specifier_sequence_opt)
    {
    }


    if (p_declaration->init_declarator_list.head)
    {
        wasm_visit_init_declarator_list(ctx, &p_declaration->init_declarator_list);
    }

    //if (type_is_function(&p_declaration->init_declarator_list.head->declarator->type))
    //{
        //TODO result
      //  ss_fprintf(&ctx->ss, " (result i23)\n");
    //}

    if (p_declaration->function_body)
    {
        wasm_visit_compound_statement(ctx, p_declaration->function_body);
    }
    ss_fprintf(&ctx->ss, ")\n");
}

int wasm_visit_literal_string(struct wasm_visit_ctx* ctx, struct token* current)
{


    return 0;
}

void wasm_visit_ctx_destroy(struct wasm_visit_ctx * obj_owner p)
{
    ss_close(&p->ss);
}

void wasm_visit(struct wasm_visit_ctx* ctx)
{
    //https://developer.mozilla.org/en-US/docs/WebAssembly
    //https://wasdk.github.io/WasmFiddle/    
    //https://webassembly.github.io/wabt/demo/wat2wasm/
    //https://webassembly.github.io/wabt/demo/wasm2wat/index.html

    ss_fprintf(&ctx->ss, "(module\n");
    struct declaration* p_declaration = ctx->ast.declaration_list.head;
    while (p_declaration)
    {
        wasm_visit_declaration(ctx, p_declaration);


        p_declaration = p_declaration->next;
    }
    ss_fprintf(&ctx->ss, "\n)");

}