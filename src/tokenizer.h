#pragma once
#include "hashmap.h"
#include "token.h"
#include "error.h"
#include "options.h"
#include "ownership.h"

#define CAKE_CFG_FNAME "/cakeconfig.h"

struct include_dir
{
    const char* _Owner path;
    struct include_dir* _Owner _Opt next;
};

struct include_dir_list
{
    struct include_dir* _Owner _Opt head;
    struct include_dir* _Opt tail;
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
    struct owner_hash_map macros;
    struct include_dir_list include_dir;

    /*map of pragma once already included files*/
    struct hash_map pragma_once_map;
    
    struct token* _Opt current;
    struct token_list input_list;
    unsigned int count_macro_value;
    bool conditional_inclusion;
    int n_warnings;
    int n_errors;    
};
void preprocessor_ctx_destroy( struct preprocessor_ctx* _Obj_owner p);

bool preprocessor_diagnostic_message(enum diagnostic_id w, struct preprocessor_ctx* ctx, const struct token* p_token, const char* fmt, ...);


struct tokenizer_ctx
{
    struct options options;
    int n_warnings;
    int n_errors;    
};

struct token_list tokenizer(struct tokenizer_ctx* ctx, const char* text, const char* _Opt filename_opt, int level, enum token_flags addflags);
void add_standard_macros(struct preprocessor_ctx* ctx);
struct include_dir* include_dir_add(struct include_dir_list* list, const char* path);

struct token_list preprocessor(struct preprocessor_ctx* ctx, struct token_list* input_list, int level);
struct token_list  copy_replacement_list(struct token_list* list);

void token_list_append_list(struct token_list* dest, struct token_list* _Obj_owner source);
void print_list(struct token_list* list);
void token_list_destroy( struct token_list* _Obj_owner list);
bool token_is_blank(struct token* p);
void token_list_pop_back(struct token_list* list);
void token_list_pop_front(struct token_list* list);
struct token* _Owner token_list_pop_front_get(struct token_list* list);
void remove_line_continuation(char* s);
struct token* token_list_clone_and_add(struct token_list* list, struct token* pnew);

void token_list_insert_after(struct token_list* list, struct token* after, struct token_list* append);
void token_list_insert_before(struct token_list* token_list, struct token* after, struct token_list* append_list);
void token_list_paste_string_after(struct token_list* list,
    struct token* after,
    const char* s);
void token_list_paste_string_before(struct token_list* list,
    struct token* before,
    const char* s);
struct token_list tokenizer(struct tokenizer_ctx* p, const char* text, const char* _Opt filename_opt, int level, enum token_flags addflags);

void print_code_as_we_see(struct token_list* list, bool remove_comments);
const char* _Owner get_code_as_we_see(struct token_list* list, bool remove_comments);
const char* _Owner get_code_as_compiler_see(struct token_list* list);

const char* _Owner get_code_as_we_see_plus_macros(struct token_list* list);
const char* _Owner get_code_as_we_see(struct token_list* list, bool remove_comments);

void print_tokens(struct token* p_token);
void print_preprocessed(struct token* p_token);
const char* _Owner print_preprocessed_to_string(struct token* p_token);
const char* _Owner print_preprocessed_to_string2(struct token* _Opt p_token);
void check_unused_macros(struct owner_hash_map* map);

char* _Owner _Opt read_file(const char* path);
const char* get_token_name(enum token_type tk);
void print_all_macros(struct preprocessor_ctx* prectx);

int string_literal_byte_size(const char* s);
int get_char_type(const char* s);
void include_config_header(struct preprocessor_ctx* ctx);
int stringify(const char* input, int n, char output[]);