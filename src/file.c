
struct parser_ctx { int x; };
enum token_type { A };

struct token* parser_match(struct parser_ctx* ctx);
struct token* parser_match(struct parser_ctx* ctx) {}

[[nodiscard]]
int parser_match_tk(struct parser_ctx* ctx, enum token_type type);

[[nodiscard]]
int parser_match_tk(struct parser_ctx* ctx, enum token_type type)
{
}

void call(void)
{
    struct parser_ctx ctx;
    parser_match_tk(&ctx, A);
}
