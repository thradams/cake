/*
   Crash regression: selection_statement's ";"-branch for the if-init-
   statement form ("fixing the last_token") had a copy-pasted else-if that
   dereferenced p_init_statement->p_simple_declaration->last_token even
   when the init-statement was a bare EXPRESSION (i++), not a declaration.

   init_statement() sets p_simple_declaration XOR p_expression_statement
   (see init_statement(), parser.c) -- so whenever p_expression_statement
   is the one that's set, p_simple_declaration is guaranteed NULL, and the
   removed branch was a straight NULL->last_token write. It also could
   never have been correct even fixed to reference p_expression_statement:
   struct expression_statement has no last_token field at all.
*/

int main()
{
    int i = 0;
    if (i++; i > 0) {}
}
