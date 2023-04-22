#pragma once
#include "parser.h"
#include "osstream.h"

struct wasm_visit_ctx
{
    struct osstream ss;
    struct ast ast;
};

void wasm_visit(struct wasm_visit_ctx* ctx);

