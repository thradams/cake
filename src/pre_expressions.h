#pragma once

struct preprocessor_ctx;

int pre_constant_expression(struct preprocessor_ctx* ctx, struct error* error, long long* pvalue);

