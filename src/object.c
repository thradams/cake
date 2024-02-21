#include "ownership.h"
#include "object.h"
#include "parser.h"
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>


void object_swap(struct object* a, struct object* b)
{
	struct object temp = *a;
	*a = *b;
	*b = temp;
}

void object_delete(struct object* owner p)
{
	if (p)
	{
		object_destroy(p);
		free(p);
	}
}

void object_destroy(struct object* obj_owner p)
{
	object_delete(p->pointed);
	objects_destroy(&p->members);
	object_state_stack_destroy(&p->object_state_stack);
}


void object_state_stack_destroy(struct object_state_stack* obj_owner p)
{
	free(p->data);
}

int object_state_stack_reserve(struct object_state_stack* p, int n) unchecked
{
	if (n > p->capacity)
	{
		if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0]))))
		{
			return EOVERFLOW;
		}

		void* owner pnew = realloc(p->data, n * sizeof(p->data[0]));
		if (pnew == NULL) return ENOMEM;

		p->data = pnew;
		p->capacity = n;
	}
	return 0;
}

int object_state_stack_push_back(struct object_state_stack* p, enum object_state e)
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

	p->data[p->size] = e;
	p->size++;

	return 0;
}

void objects_destroy(struct objects* obj_owner p) unchecked
{
	for (int i = 0; i < p->size; i++)
	{
		object_destroy(&p->data[i]);
	}
	free(p->data);
}

int objects_reserve(struct objects* p, int n) unchecked
{
	if (n > p->capacity)
	{
		if ((size_t)n > (SIZE_MAX / (sizeof(p->data[0]))))
		{
			return EOVERFLOW;
		}

		void* owner pnew = realloc(p->data, n * sizeof(p->data[0]));
		if (pnew == NULL) return ENOMEM;

		p->data = pnew;
		p->capacity = n;
	}
	return 0;
}

int objects_push_back(struct objects* p, struct object* obj_owner p_object)
{
	if (p->size == INT_MAX)
	{
		object_destroy(p_object);
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
			object_destroy(p_object);
			return error;
		}
	}

	p->data[p->size] = *p_object; /*COPIED*/


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

struct object make_object_core(struct type* p_type, struct object_name_list* list, int deep, const struct declarator* declarator)
{
	struct object obj = { 0 };
	obj.declarator = declarator;

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
								struct object member_obj = { 0 };
								member_obj.declarator = declarator;
								member_obj.state = OBJECT_STATE_NOT_APPLICABLE;
								objects_push_back(&obj.members, &member_obj);
							}
							else
							{
								struct object member_obj = make_object_core(&p_member_declarator->declarator->type, &l, deep, declarator);
								objects_push_back(&obj.members, &member_obj);
							}

							//member_index++;
						}
						p_member_declarator = p_member_declarator->next;
					}
				}
				else
				{
					if (p_member_declaration->specifier_qualifier_list->struct_or_union_specifier)
					{
						//struct object obj = {0};
						//obj.state = OBJECT_STATE_STRUCT;
						//objects_push_back(&obj.members, &obj);


						struct type t = { 0 };
						t.category = TYPE_CATEGORY_ITSELF;
						t.struct_or_union_specifier = p_member_declaration->specifier_qualifier_list->struct_or_union_specifier;
						t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;
						struct object member_obj = make_object_core(&t, &l, deep, declarator);
						objects_push_back(&obj.members, &member_obj);
						type_destroy(&t);
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
		obj.state = OBJECT_STATE_NOT_APPLICABLE;

		if (deep < 1)
		{
			struct type t2 = type_remove_pointer(p_type);
			if (type_is_struct_or_union(&t2))
			{
				struct object* owner p_object = calloc(1, sizeof(struct object));
				*p_object = make_object_core(&t2, list, deep + 1, declarator);
				obj.pointed = p_object;
			}

			type_destroy(&t2);
			//(*p_deep)++;
		}
	}
	else
	{
		//assert(p_object->members_size == 0);
		//p_object->state = flags;
		obj.state = OBJECT_STATE_NOT_APPLICABLE;
	}

	return obj;
}

struct object make_object(struct type* p_type, const struct declarator* declarator)
{
	assert(declarator);
	struct object_name_list list = { .name = "" };
	return make_object_core(p_type, &list, 0, declarator);
}

void object_push_copy_current_state(struct object* object)
{

	object_state_stack_push_back(&object->object_state_stack, object->state);

	if (object->pointed)
	{
		object_push_copy_current_state(object->pointed);
	}

	for (int i = 0; i < object->members.size; i++)
	{
		object_push_copy_current_state(&object->members.data[i]);
	}

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

	if (object->pointed)
	{
		object_pop_states(object->pointed, n);
	}

	for (int i = 0; i < object->members.size; i++)
	{
		object_pop_states(&object->members.data[i], n);
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

	enum object_state sstate = object->object_state_stack.data[index];
	object->state = sstate;

	if (object->pointed)
	{
		object_restore_state(object->pointed, state_to_restore);
	}

	for (int i = 0; i < object->members.size; i++)
	{
		object_restore_state(&object->members.data[i], state_to_restore);
	}
}

void print_object_core(int ident, struct type* p_type, struct object* p_object, const char* previous_names, bool is_pointer, bool short_version)
{
	if (p_object == NULL)
	{
		return;
	}

	if (p_type->struct_or_union_specifier && p_object->members.size > 0)
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
							const char* name = p_member_declarator->declarator->name ? p_member_declarator->declarator->name->lexeme : "";

							char buffer[200] = { 0 };
							if (is_pointer)
								snprintf(buffer, sizeof buffer, "%s->%s", previous_names, name);
							else
								snprintf(buffer, sizeof buffer, "%s.%s", previous_names, name);


							print_object_core(ident + 1, &p_member_declarator->declarator->type,
								&p_object->members.data[member_index], buffer,
								type_is_pointer(&p_member_declarator->declarator->type), short_version);

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

					print_object_core(ident + 1, &t, &p_object->members.data[member_index], previous_names, false, short_version);

					member_index++;
					type_destroy(&t);
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
			if (short_version)
			{
				printf("%s == ", previous_names);
				object_state_to_string(p_object->state);
			}
			else
			{
				printf("%p:%s == ", p_object, previous_names);
				printf("{");
				for (int i = 0; i < p_object->object_state_stack.size; i++)
				{
					object_state_to_string(p_object->object_state_stack.data[i]);
					printf(",");
				}
				printf("*");
				object_state_to_string(p_object->state);
				printf("}");
			}
			printf("\n");


			if (p_object->pointed)
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



				print_object_core(ident + 1, &t2, p_object->pointed, buffer, is_pointer, short_version);
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
		{
			if (short_version)
			{
				printf("%s == ", previous_names);
				object_state_to_string(p_object->state);
			}
			else
			{
				printf("%p:%s == ", p_object, previous_names);
				printf("{");
				for (int i = 0; i < p_object->object_state_stack.size; i++)
				{
					object_state_to_string(p_object->object_state_stack.data[i]);
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



	print_object_core(0, p_type, p_object, name, type_is_pointer(p_type), short_version);
}

void set_object(
	struct type* p_type,
	struct object* p_object,
	enum object_state flags);

void set_object_state(
	struct parser_ctx* ctx,
	struct type* p_type,
	struct object* p_object,
	const struct type* p_source_type,
	const struct object* p_object_source,
	const struct token* error_position)
{
	if (p_object_source == NULL)
	{
		return;
	}
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
								set_object_state(ctx,
									&p_member_declarator->declarator->type,
									&p_object->members.data[member_index],
									&p_object_source->members.data[member_index].declarator->type,
									&p_object_source->members.data[member_index],
									error_position);
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
			p_object->state = p_object_source->state;
		}
	}
	else if (type_is_array(p_type))
	{
		p_object->state = p_object_source->state;
		if (p_object->members.size > 0)
		{
			//not sure if we instantiate all items of array
			p_object->members.data[0].state = p_object_source->members.data[0].state;
		}
	}
	else if (type_is_pointer(p_type))
	{
		if (p_object_source)
		{
			if (p_object_source->state == OBJECT_STATE_UNINITIALIZED)
			{
				char buffer[100] = { 0 };
				object_get_name(p_source_type, p_object_source, buffer, sizeof buffer);
				compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
					ctx,
					error_position,
					"source object '%s' is uninitialized", buffer);
			}
			else if (p_object_source->state & OBJECT_STATE_UNINITIALIZED)
			{
				char buffer[100] = { 0 };
				object_get_name(p_source_type, p_object_source, buffer, sizeof buffer);

				compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
					ctx,
					error_position,
					"source object '%s' may be uninitialized", buffer);
			}

			if (type_is_any_owner(p_type) &&
				type_is_any_owner(p_source_type))
			{
				if (p_object_source->state == OBJECT_STATE_MOVED)
				{
					char buffer[100] = { 0 };
					object_get_name(p_source_type, p_object_source, buffer, sizeof buffer);

					compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
						ctx,
						error_position,
						"source object '%s' have been moved", buffer);
				}
				else if (p_object_source->state & OBJECT_STATE_MOVED)
				{
					char buffer[100] = { 0 };
					object_get_name(p_source_type, p_object_source, buffer, sizeof buffer);

					compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
						ctx,
						error_position,
						"source object '%s' may have been moved", buffer);
				}
			}

		}


		if (type_is_any_owner(p_type))
		{
			p_object->state = p_object_source->state;
		}
		else
		{
			//MOVED state is not applicable to non owner objects
			p_object->state = p_object_source->state & ~OBJECT_STATE_MOVED;
		}


		if (p_object->pointed)
		{
			struct type t2 = type_remove_pointer(p_type);
			if (p_object_source->pointed)
			{
				set_object_state(ctx, &t2, p_object->pointed, p_source_type, p_object_source->pointed, error_position);
			}
			else
			{
				set_object(&t2, p_object->pointed, OBJECT_STATE_NULL | OBJECT_STATE_NOT_NULL);
			}
			type_destroy(&t2);
		}
	}
	else
	{


		//assert(p_object->members.size == 0); //enum?
		p_object->state = p_object_source->state;
	}
}


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
								set_direct_state(&p_member_declarator->declarator->type, &p_object->members.data[member_index], flags);
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
			if (type_is_out(&t2))
			{
				flags = OBJECT_STATE_UNINITIALIZED;
			}
			set_object(&t2, p_object->pointed, flags);
			type_destroy(&t2);
		}
	}
	else
	{
		//assert(p_object->members.size == 0); //enum?
		p_object->state = flags;
	}
}


void object_set_unknown(struct type* p_type, struct object* p_object)
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
								object_set_unknown(&p_member_declarator->declarator->type, &p_object->members.data[member_index]);
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
		p_object->state = OBJECT_STATE_NULL | OBJECT_STATE_NOT_NULL;

		if (p_object->pointed)
		{
			struct type t2 = type_remove_pointer(p_type);
			object_set_unknown(&t2, p_object->pointed);
			type_destroy(&t2);
		}
	}
	else
	{
		p_object->state = OBJECT_STATE_ZERO | OBJECT_STATE_NOT_ZERO;
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
							&p_object->members.data[member_index]))
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
				p_object->state == OBJECT_STATE_MOVED ||
				p_object->state == OBJECT_STATE_NOT_NULL ||
				p_object->state == (OBJECT_STATE_UNINITIALIZED | OBJECT_STATE_MOVED))
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
	int out_size)
{
	if (p_object == NULL)
	{
		return;
	}

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
							&p_object->members.data[member_index],
							p_object_target,
							buffer,
							outname,
							out_size);

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
				object_get_name_core(
					&t2,
					p_object->pointed,
					p_object_target,
					buffer,
					outname,
					out_size);
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
	if (p_object->declarator == NULL)
	{
		outname[0] = '?';
		outname[1] = '\0';
		return;
	}


	const char* root_name = p_object->declarator->name ? p_object->declarator->name->lexeme : "?";
	const struct object* root = &p_object->declarator->object;

	object_get_name_core(&p_object->declarator->type, root, p_object, root_name, outname, out_size);
}

void checked_moved(struct parser_ctx* ctx,
	struct type* p_type,
	struct object* p_object,
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
						checked_moved(ctx, &p_member_declarator->declarator->type,
							&p_object->members.data[member_index],
							position_token);

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
		if (type_is_pointer(p_type) && !type_is_any_owner(p_type))
		{
			struct type t2 = type_remove_pointer(p_type);
			checked_moved(ctx,
				&t2,
				p_object->pointed,
				position_token);
			type_destroy(&t2);
		}

		if (p_object->state & OBJECT_STATE_MOVED)
		{
			struct token* name_pos = p_object->declarator->name ? p_object->declarator->name : p_object->declarator->first_token;
			const char* parameter_name = p_object->declarator->name ? p_object->declarator->name->lexeme : "?";

			char name[200] = { 0 };
			object_get_name(p_type, p_object, name, sizeof name);
			compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
				ctx,
				position_token,
				"parameter '%s' is leaving scoped with a moved object '%s'",
				parameter_name,
				name);

			compiler_set_info_with_token(W_NONE, ctx, name_pos, "parameter", name);
		}

		if (p_object->state & OBJECT_STATE_UNINITIALIZED)
		{
			struct token* name_pos = p_object->declarator->name ? p_object->declarator->name : p_object->declarator->first_token;
			const char* parameter_name = p_object->declarator->name ? p_object->declarator->name->lexeme : "?";

			char name[200] = { 0 };
			object_get_name(p_type, p_object, name, sizeof name);
			compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
				ctx,
				position_token,
				"parameter '%s' is leaving scoped with a uninitialized object '%s'",
				parameter_name,
				name);

			compiler_set_info_with_token(W_NONE, ctx, name_pos, "parameter", name);
		}
	}
}


void checked_read_object(struct parser_ctx* ctx,
	struct type* p_type,
	struct object* p_object,
	const struct token* position_token,
	bool check_pointed_object)
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
						checked_read_object(ctx, &p_member_declarator->declarator->type,
							&p_object->members.data[member_index],
							position_token,
							check_pointed_object);

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
		if (type_is_pointer(p_type) &&
			check_pointed_object &&
			p_object->state & OBJECT_STATE_NOT_NULL /*we don't need to check pointed object*/
			)
		{
			struct type t2 = type_remove_pointer(p_type);
			checked_read_object(ctx,
				&t2,
				p_object->pointed,
				position_token,
				true);
			type_destroy(&t2);
		}

		if (p_object->state & OBJECT_STATE_MOVED)
		{
			//struct token* name_pos = p_object->declarator->name ? p_object->declarator->name : p_object->declarator->first_token;
			//const char* parameter_name = p_object->declarator->name ? p_object->declarator->name->lexeme : "?";

			char name[200] = { 0 };
			object_get_name(p_type, p_object, name, sizeof name);
			compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
				ctx,
				position_token,
				"object '%s' was moved",
				name);
		}

		if (p_object->state & OBJECT_STATE_UNINITIALIZED)
		{
			char name[200] = { 0 };
			object_get_name(p_type, p_object, name, sizeof name);
			compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
				ctx,
				position_token,
				"uninitialized object '%s'",
				name);
		}
	}
}


void visit_object(struct parser_ctx* ctx,
	struct type* p_type,
	struct object* p_object,
	const struct token* position_token,
	const char* previous_names,
	bool is_assigment)
{
	if (p_object == NULL)
	{
		return;
	}
	if (p_type->type_qualifier_flags & TYPE_QUALIFIER_VIEW)
	{
		return;
	}

	if (!type_is_any_owner(p_type))
	{
		if (p_type->storage_class_specifier_flags & STORAGE_SPECIFIER_PARAMETER)
		{
			//for view parameters we need to check if they left something moved..
			checked_moved(ctx,
				p_type,
				p_object,
				position_token);

		}
		return;
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
			const struct token* const name = p_object->declarator->name ? p_object->declarator->name : p_object->declarator->first_token;
			compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
				ctx,
				name,
				"object '%s' was not moved/destroyed",
				previous_names);

			if (p_object->declarator)
				compiler_set_info_with_token(W_NONE, ctx, position_token, "end of '%s' scope", previous_names);
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

							visit_object(ctx, &p_member_declarator->declarator->type,
								&p_object->members.data[member_index],
								position_token,
								buffer,
								is_assigment);

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
		const struct token* const position =
			p_object->declarator->name ? p_object->declarator->name : p_object->declarator->first_token;

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
			should_had_been_moved = (p_object->state & OBJECT_STATE_NOT_NULL);
		}
		else
		{
			if (p_object->state == OBJECT_STATE_UNINITIALIZED ||
				p_object->state == OBJECT_STATE_MOVED ||
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
					visit_object(ctx,
						&t2,
						p_object->pointed,
						position_token,
						buffer,
						is_assigment);
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

					if (!pointed_is_out)
					{
						if (is_assigment)
						{
							compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
								ctx,
								position_token,
								"memory pointed by '%s' was not released before assignment.",
								name);
						}
						else
						{
							compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
								ctx,
								position,
								"memory pointed by '%s' was not released.",
								name);
							if (p_object->declarator)
							{
								compiler_set_info_with_token(W_NONE, ctx, position_token, "end of '%s' scope", name);
							}
						}
					}
				}
				else
				{
					if (is_assigment)
					{
						compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
							ctx,
							position_token,
							"previous members of '%s' were not moved before this assignment.",
							name);
					}
					else
					{
						compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
							ctx,
							position,
							"object '%s' was not moved.",
							name);
						if (p_object->declarator)
						{
							compiler_set_info_with_token(W_NONE, ctx, position_token, "end of '%s' scope", name);
						}
					}
				}
			}
		}
	}

}


void object_assignment(struct parser_ctx* ctx,
	struct object* p_source_obj_opt,
	struct type* p_source_obj_type,

	struct object* p_dest_obj_opt,
	struct type* p_dest_obj_type,

	const struct token* error_position,
	bool bool_source_zero_value,
	enum object_state source_state_after)
{
	if (p_dest_obj_opt)
	{
		if (type_is_owner(p_dest_obj_type) && !type_is_out(p_dest_obj_type))
		{
			char buffer[100] = { 0 };
			object_get_name(p_dest_obj_type, p_dest_obj_opt, buffer, sizeof buffer);
			visit_object(ctx,
				p_dest_obj_type,
				p_dest_obj_opt,
				error_position,
				buffer,
				true);
		}
	}


	if (p_dest_obj_opt)
	{
		if (bool_source_zero_value)
		{
			set_object(p_dest_obj_type, p_dest_obj_opt, OBJECT_STATE_NULL);
		}
		else
		{
			if (p_source_obj_opt)
			{
				set_object_state(ctx, p_dest_obj_type, p_dest_obj_opt, p_source_obj_type, p_source_obj_opt, error_position);
			}
			else
			{
				object_set_unknown(p_dest_obj_type, p_dest_obj_opt);
			}
		}

	}


	if (type_is_any_owner(p_source_obj_type) &&
		!type_is_owner(p_dest_obj_type) &&
		p_source_obj_type->storage_class_specifier_flags & STORAGE_SPECIFIER_FUNCTION_RETURN)
	{
		/*
		int main()
		{
		   struct X * p = (struct X * owner) malloc(1);
		}
		*/

		compiler_set_error_with_token(C_OWNERSHIP_FLOW_MISSING_DTOR,
			ctx,
			error_position,
			"Object must be owner qualified.");
	}

	if (type_is_any_owner(p_dest_obj_type) && type_is_any_owner(p_source_obj_type) && type_is_pointer(p_source_obj_type))
	{
		if (type_is_void_ptr(p_dest_obj_type))
		{
			if (p_source_obj_opt)
			{
				struct type t2 = type_remove_pointer(p_source_obj_type);
				const char* name = p_source_obj_opt->declarator->name ?
					p_source_obj_opt->declarator->name->lexeme :
					"?";

				visit_object(ctx,
					&t2,
					p_source_obj_opt->pointed,
					error_position,
					name,
					true);
				p_source_obj_opt->state = source_state_after;
				type_destroy(&t2);
			}
		}
		else if (type_is_obj_owner(p_dest_obj_type))
		{
			if (type_is_owner(p_source_obj_type))
			{
				if (p_source_obj_opt->pointed)
				{
					struct type t = type_remove_pointer(p_source_obj_type);
					set_object(&t, p_source_obj_opt->pointed, source_state_after);
					type_destroy(&t);
				}
			}
			else if (type_is_obj_owner(p_source_obj_type))
			{
				if (p_source_obj_opt->pointed)
				{
					struct type t = type_remove_pointer(p_source_obj_type);
					set_object(&t, p_source_obj_opt->pointed, source_state_after);
					type_destroy(&t);
				}
			}
		}
		else
		{

			if (p_source_obj_opt)
			{
				set_object(p_source_obj_type, p_source_obj_opt, source_state_after);
			}
		}
	}
	else if (type_is_any_owner(p_dest_obj_type) && type_is_any_owner(p_source_obj_type))
	{
		/*everthing is moved*/
		if (p_source_obj_opt)
			set_object(p_source_obj_type, p_source_obj_opt, source_state_after);
	}
	else
	{
		/*nothing changes*/
	}

}

