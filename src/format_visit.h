#pragma once
#include "parser.h"
#include  "error.h"
#include"ownership.h"

struct format_visit_ctx
{
    view struct ast ast;
    int identation;
};

void format_visit(struct format_visit_ctx* ctx);
