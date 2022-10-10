#pragma once
#include "hashmap.h"
#include "token.h"
#include "error.h"
#include "options.h"


struct include_dir
{
    const char* path;
    struct include_dir* next;
};

struct include_dir_list
{
    struct include_dir* head;
    struct include_dir* tail;
};


enum preprocessor_ctx_flags
{
    PREPROCESSOR_CTX_FLAGS_NONE = 0,
    PREPROCESSOR_CTX_FLAGS_ONLY_FINAL = 1 << 0
};

struct preprocessor_ctx
{
    struct options options;
    enum preprocessor_ctx_flags flags;
    struct hash_map macros;
    struct include_dir_list include_dir;

    /*lista dos headers marcados com pragma once*/
    struct hash_map pragma_once_map;
    
    struct token* current;
    struct token_list input_list;

    bool conditional_inclusion;
    int n_warnings;
    int n_errors;
    int (*printf)(const char* fmt, ...);
};
void preprocessor_ctx_destroy(struct preprocessor_ctx* p);


struct token_list tokenizer(const char* text, const char* filename_opt, int level, enum token_flags addflags, struct error* error);
void add_standard_macros(struct preprocessor_ctx* ctx, struct error* error);
struct include_dir* include_dir_add(struct include_dir_list* list, const char* path);

struct token_list preprocessor(struct preprocessor_ctx* ctx, struct token_list* input_list, int level, struct error* error);
struct token_list  copy_replacement_list(struct token_list* list);

void token_list_append_list(struct token_list* dest, struct token_list* source);
void print_list(struct token_list* list);
void token_list_destroy(struct token_list* list);
bool token_is_blank(struct token* p);
struct token* token_list_pop_back(struct token_list* list);
struct token* token_list_pop_front(struct token_list* list);
void remove_line_continuation(char* s);
struct token* token_list_clone_and_add(struct token_list* list, struct token* pnew);
struct token_list token_list_remove(struct token_list* list, struct token* first, struct token* last);
void token_list_insert_after(struct token_list* list, struct token* pAfter, struct token_list* append);
struct token_list tokenizer(const char* text, const char* filename_opt, int level, enum token_flags addflags, struct error* error);

const char* get_code_as_we_see(struct token_list* list, bool removeComments);
const char* get_code_as_compiler_see(struct token_list* list);

const char* get_code_as_we_see_plusmacros(struct token_list* list);
const char* get_code_as_we_see(struct token_list* list, bool removeComments);

void print_tokens(struct token* p_token);
void print_preprocessed(struct token* p_token);
const char* print_preprocessed_to_string(struct token* p_token);
const char* print_preprocessed_to_string2(struct token* p_token);
void check_unused_macros(struct hash_map* pMap);

char* readfile(const char* path);
const char* get_token_name(enum token_type tk);
