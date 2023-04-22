#pragma once
#include "parser.h"
#include  "error.h"

struct format_visit_ctx
{
    struct ast ast;
    int identation;
};

void format_visit(struct format_visit_ctx* ctx);
