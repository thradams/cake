#pragma once
#include <stddef.h>
#include "ownership.h"

enum json_type
{
  JSON_OBJECT,
  JSON_ARRAY,
  JSON_STRING,
  JSON_NUMBER,
  JSON_TRUE,
  JSON_FALSE,
  JSON_NULL
};

struct json_value
{
  enum json_type type;

  char * _Owner _Opt key;
  char * _Owner _Opt string;

  /* Valid only for JSON_NUMBER. */
  double number;

  /* Children of an object or array, in document order. */
  struct json_value *_Opt _Owner first_child;
  struct json_value *_Opt last_child;
  struct json_value * _Opt _Owner next;
};

struct json_error
{
  char message[128];
  size_t line;   /* 1-based */
  size_t column; /* 1-based, counted in bytes */
};

struct json_value *_Owner _Opt json_parse(const char * _Opt text, struct json_error * _Opt error);
void json_delete(struct json_value * _Owner _Opt value);

struct json_value * _Opt json_find_member(const struct json_value *object, const char *key);

struct json_value *_Opt json_item(const struct json_value *value, size_t index);

size_t json_count(const struct json_value *value);


/* Append a member to an object. */
struct json_value *_Opt json_set_string(struct json_value *object, const char *key, const char *text);
struct json_value *_Opt json_set_number(struct json_value *object, const char *key, double number);
struct json_value *_Opt json_set_bool(struct json_value *object, const char *key, int boolean_value);
struct json_value *_Opt json_set_null(struct json_value *object, const char *key);
struct json_value *_Opt json_set_object(struct json_value *object, const char *key);
struct json_value *_Opt json_set_array(struct json_value *object, const char *key);

/* Append an element to an array. */
struct json_value *_Opt json_add_string(struct json_value * _Opt array, const char *text);
struct json_value *_Opt json_add_number(struct json_value * _Opt array, double number);
struct json_value *_Opt json_add_bool(struct json_value * _Opt array, int boolean_value);
struct json_value *_Opt json_add_null(struct json_value * _Opt array);
struct json_value *_Opt json_add_object(struct json_value * _Opt array);
struct json_value * _Opt json_add_array(struct json_value * _Opt array);

int json_write_file(const char *path, const struct json_value *root);



