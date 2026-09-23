/*
  Diagnostic levels can be configured per code region using:
    #pragma CAKE diagnostic push   (save current settings)
    #pragma CAKE diagnostic error   <id> [id2 ...]
    #pragma CAKE diagnostic warning <id> [id2 ...]
    #pragma CAKE diagnostic note    <id> [id2 ...]
    #pragma CAKE diagnostic ignored <id> [id2 ...]
    #pragma CAKE diagnostic pop    (restore previous settings)

  Warning 4 = mixing enumerators from different enum types.
*/

enum Color { RED, GREEN, BLUE };
enum Direction { NORTH, SOUTH, EAST, WEST };

int main()
{
    enum Color c = RED;
    enum Direction d = NORTH;

    /* Promote warning 4 to a hard error — mixing enums breaks the build */
#pragma CAKE diagnostic push
#pragma CAKE diagnostic error 4
    if (c == SOUTH) {}   /* error: enumerators from different enums */
#pragma CAKE diagnostic pop

    /* Default level: warning */
#pragma CAKE diagnostic push
#pragma CAKE diagnostic warning 4
    if (c == SOUTH) {}   /* warning: enumerators from different enums */
#pragma CAKE diagnostic pop

    /* Demote to a note — informational only */
#pragma CAKE diagnostic push
#pragma CAKE diagnostic note 4
    if (c == SOUTH) {}   /* note: enumerators from different enums */
#pragma CAKE diagnostic pop

    /* Silence completely — useful for third-party or generated code */
#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignored 4
    if (c == SOUTH) {}   /* no diagnostic */
#pragma CAKE diagnostic pop

    /* Multiple warnings can be configured in one pragma */
#pragma CAKE diagnostic push
#pragma CAKE diagnostic ignored 4 10
    if (c == SOUTH) {}   /* 4: enum mix — suppressed  */
    c;                   /* 10: result not used — suppressed */
#pragma CAKE diagnostic pop
}
