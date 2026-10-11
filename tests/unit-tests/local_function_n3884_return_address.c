/* N3884: the address of a local function can be returned */

typedef void (*callback_t)(void);

callback_t register_handler()
{
    static void handler() { /* ... */ }
    return &handler;  /* valid */
}
