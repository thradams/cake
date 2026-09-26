/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake 
*/

#pragma once
#include "hashmap.h"
#include "token.h"
#include "error.h"
#include "options.h"
#include "cake_compat.h"


#define CAKE_CONFIG_FILE_NAME "cake.json"


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
    struct hash_map macros;
    struct include_dir_list include_dir;
        
    bool cake_config_found; /*whether cake.json (next to the executable) was found and used*/

    /*map of pragma once already included files*/
    struct hash_map pragma_once_map;

    /*options -copy-headers*/
    struct hash_map copy_headers;
    
    struct token* _Opt current;
    struct token_list input_list;
    unsigned int count_macro_value;
    bool conditional_inclusion;
    int n_warnings;
    int n_errors;    

    /* -find-definition: the #define of the macro name under the cursor; the parser does not run then */
    const struct token* _Opt p_find_definition;
};

void preprocessor_ctx_destroy( _Dtor struct preprocessor_ctx* p);

void pre_unexpected_end_of_file(const struct token* _Opt p_token, struct preprocessor_ctx* ctx);
bool preprocessor_diagnostic(enum diagnostic_id w, struct preprocessor_ctx* ctx, const struct token* _Opt p_token, const char* fmt, ...);


struct tokenizer_ctx
{
    struct options options;
    int n_warnings;
    int n_errors;    
};

struct token_list tokenizer(struct tokenizer_ctx* ctx, const char* text, const char* _Opt filename_opt, int level, enum token_flags addflags);
void add_standard_macros(struct preprocessor_ctx* ctx, enum target target);
struct include_dir* _Opt include_dir_add(struct include_dir_list* list, const char* path);

struct token_list preprocessor(struct preprocessor_ctx* ctx, struct token_list* input_list, int level);
struct token_list copy_replacement_list(const struct preprocessor_ctx* ctx, const struct token_list* list);

void token_list_append_list(struct token_list* dest, _Clear struct token_list* source);
void print_list(bool color_enabled, struct token_list* list);
void token_list_destroy(_Opt _Dtor struct token_list* list);
bool token_is_blank(const struct token* p);
void token_list_pop_back(struct token_list* list);
void token_list_pop_front(struct token_list* list);
struct token* _Owner _Opt token_list_pop_front_get(struct token_list* list);
void remove_line_continuation(char* s);
bool token_list_is_equal(const struct token_list* list_a, const struct token_list* list_b);
void token_list_insert_after(struct token_list* list, struct token* _Opt after, struct token_list* append);
void token_list_insert_before(struct token_list* token_list, struct token* after, struct token_list* append_list);
struct token_list tokenizer(struct tokenizer_ctx* p, const char* text, const char* _Opt filename_opt, int level, enum token_flags addflags);

void print_code_as_we_see(const struct token_list* list, bool remove_comments);
const char* _Owner _Opt get_code_as_compiler_see(const struct token_list* list);

const char* _Owner _Opt get_code_as_we_see(const struct token_list* list, bool remove_comments);

void print_tokens(bool color_enabled, const struct token* _Opt p_token);
void print_preprocessed(const struct token* p_token);
const char* _Owner _Opt print_preprocessed_to_string(const struct token* p_token);
const char* _Owner _Opt print_preprocessed_to_string2(const struct token* _Opt p_token);
void preprocessor_mark_predefined_macros(struct preprocessor_ctx* ctx);

/*
  -unused-extern-report: what the files of one invocation define, collected
  across all of them and reported at the end (global_unused_functions_report).
*/
enum global_unused_kind
{
    GLOBAL_UNUSED_FUNCTION, /* external function, identified by name */
    GLOBAL_UNUSED_MACRO,      /* identified by name, file and line */
    GLOBAL_UNUSED_ENUMERATOR, /* identified by name, file and line */
};

struct global_unused_entry
{
    enum global_unused_kind kind;
    char* _Owner name;
    char* _Owner file;
    int line;
    bool used;            /* true if used in at least one file seen so far */
    bool has_definition;  /* true if defined (in a file that is reported) in at least one file so far */
};

struct global_unused_list
{
    struct global_unused_entry* _Owner _Opt data;
    int size;
    int capacity;

    /* only macros defined under this directory (the common directory of the files) are reported */
    char* _Owner _Opt root_dir;
};

void global_unused_functions_clear(_Clear struct global_unused_list* p);
/* only what is defined under the project directory (root_dir) is reported */
bool is_file_under_project_folder(const struct global_unused_list* p, const char* file);
void global_unused_register(struct global_unused_list* p, enum global_unused_kind kind, const char* name, const char* file, int line, bool used_here, bool has_definition_here);

/* registers the macros of the file just preprocessed */
void preprocessor_register_unused_macros(struct preprocessor_ctx* ctx, struct global_unused_list* p);

const char* get_token_name(enum token_type tk);
const char* get_diagnostic_friendly_token_name(enum token_type tk);
const char* get_diagnostic_token_text(const struct token* _Opt p_token);
void print_all_macros(const struct preprocessor_ctx* prectx);


int preprocessor_load_config(struct preprocessor_ctx* ctx);
void get_cake_config_path(char* out, size_t out_size);
int stringify(const char* input, int n, char output[]);
void print_path(const char* path, bool fullpath);
int preprocessor_copy_included_headers(const struct preprocessor_ctx* ctx, const char* dest_dir);

