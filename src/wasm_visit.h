#pragma once
#include "parser.h"
#include "osstream.h"
#include "ownership.h"
struct wasm_visit_ctx
{
    struct osstream ss;
    view struct ast ast;
};

void wasm_visit_ctx_destroy(struct wasm_visit_ctx * obj_owner p);
void wasm_visit(struct wasm_visit_ctx* ctx);

