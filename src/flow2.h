
#pragma once

#pragma region

#include <stdbool.h>
#include "object.h"

struct flow_path_value
{
    const struct object* key;
    struct object value;
    int ref_count;
};

struct flow_path_value;

struct flow_path {
    struct flow_path_value** data;
    int size;
    int capacity;
};


[[nodiscard]]
struct object* flow_path_find(const struct flow_path* arr, const struct object* key);

[[nodiscard]]
int flow_path_set(struct flow_path* arr, const struct object* key, const struct object* value);

struct flow_path flow_path_dup(const struct flow_path* source);

struct flow_set
{
    struct flow_path** data;
    int size;
    int capacity;
};

void flow_set_clear(struct flow_set* flow);
void flow_set_destroy(struct flow_set* flow);
struct flow_set flow_set_dup(const struct flow_set* flow);
int flow_set_set(struct flow_set* flow, const struct object* key, const struct object* value);
void flow_set_swap(struct flow_set* a, struct flow_set* b);



void flow_set_remove_states_where_object_is_true(const struct flow_set * src, const struct object * key);
void flow_set_remove_states_where_object_is_false(const struct flow_set * src, const struct object * key);
void flow_set_print(const struct flow_set * src, int line, int col);
struct flow_set flow_set_merge(const struct flow_set * a, const struct flow_set * b);

struct object * flow_set_has_known_value(const struct flow_set * src, const struct object * key);

bool flow_set_eval(const struct flow_set * src, struct object * a, int op, struct object * b, bool * result);
bool flow_set_i_know_this_is(struct flow_set* src, const struct object* key, bool istrue);

    

struct var_iterator
{
    int next_index;
    const struct object* key;
    struct object* p_value;
    const struct flow_set* p_flow_set;
};

bool var_iterator_next(struct var_iterator* p_it);
struct var_iterator flow_set_var_iterator(const struct flow_set* src, struct object* key);


[[nodiscard]]
int flow_set_push_back(struct flow_set* p, struct flow_path* book);

[[nodiscard]]
int flow_path_set(struct flow_path* arr, const struct object* key, const struct object* value);

#pragma endregion

#include "parser.h"


struct label_set_list
{
    struct label_set* head;
    struct label_set* tail;
};


struct flow_visit_ctx
{
    struct object* p_assigment_object;

    bool debug_static_started;
    struct declaration* p_declaration;
    struct parser_ctx* ctx;
    _View struct ast ast;

    struct selection_statement* _Opt p_selection_statement;

    struct flow_set switch_set;
    
    struct flow_set catch_set;
    struct flow_set break_set;
    struct flow_set continue_set;
    struct objects heap_objects;
    struct label_set_list labels;
};

void flow_start_visit_declaration(struct flow_visit_ctx* ctx, struct declaration* p_declaration);
void flow_visit_ctx_destroy(_Dtor struct flow_visit_ctx* p);
