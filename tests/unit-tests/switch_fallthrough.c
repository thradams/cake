/*
  [[fallthrough]]; suppresses the implicit fall-through warning (88) when it
  is the last statement before a case/default label; grouped empty labels
  (case 1: case 2:) never warn; a fallthrough attribute not immediately
  followed by a label is itself flagged.

  Warning 88 is phase 1 (checked only after the whole switch is parsed), so
  like the enum-not-handled check it can only be suppressed by a //lint
  comment placed right after the switch body's own closing '}' -- one "88"
  per expected occurrence.
*/

void g(void);
void h(void);

void no_warning(int n)
{
    switch (n)
    {
    case 1:
    case 2:
        g();
        [[fallthrough]];
    case 3:
        h();
        break;
    default:
        break;
    }
}

void warns(int n)
{
    switch (n)
    {
    case 1:
        g();
    case 2:
        h();
        break;
    } //lint 88
}

void misplaced_attribute(int n)
{
    switch (n)
    {
    case 1:
        g();
        [[fallthrough]];
        h();
    case 2:
        break;
    } //lint 88 88
}
