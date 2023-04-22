#pragma once
#include "parser.h"

struct wasm_visit_ctx
{
    struct ast ast;
};

void wasm_visit(struct wasm_visit_ctx* ctx);

