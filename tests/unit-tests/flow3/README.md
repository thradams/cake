# flow3 tests

Regression samples for the flow3 analyser (`src/flow3.c`). 316 files here,
plus `failing/` for bugs that are characterised but not yet fixed.

Most are small reductions of real false positives or missed diagnostics, many
found by running cake on its own source. The header comment of each file is the
point of the test: what it reports, why that is right or wrong, and where it was
reduced from. Keep that comment truthful — several files in here carried a
diagnosis that later turned out to be wrong, and the stale explanation cost more
time than the bug.

## Running

```bash
cake -fanalyzer -nullable=enabled -no-output -fdiagnostics-color=never file.c
```

A passing test prints nothing but the banner and filename. Any `warning`/`error`
line is a failure.

To sweep the folder:

```bash
for f in *.c; do cake -fanalyzer -nullable=enabled -no-output -fdiagnostics-color=never "$f"; done
```

## Conventions

- `#pragma safety enable` at the top (287 of 316 files).
- `//lint N` on a line suppresses warning N there. 124 files use this.

`//lint` is not just a mute button — it is an **assertion that the warning
fires**. If the warning stops firing, cake reports `warning 59: diagnostic 'N'
not recognized` for the unused suppression. So a `//lint` marks expected
behaviour and fails loudly when that behaviour changes, in either direction.

Two ways it gets used, and they mean opposite things:

```c
free_item(l->head);
} //lint 72 correct: l->head really is left consumed   <- expected TRUE positive

p = &cond; //lint 68  TODO FALSE WARNING              <- known FALSE positive
```

When a fix removes a false positive, delete the `//lint` along with the `TODO`,
or the unused suppression becomes a `warning 59`.

## Checking a flow3 change

The unit tests alone are **not** a sufficient bar. They are small and do not
exercise the owner-releasing loop idioms that live in real code. A change that
passes every test here can still regress the codebase badly — one did, taking
the source tree from 73 to 128 warnings while this folder stayed green.

Always also run the analyser over cake's own 19 source files and compare totals
against the commit you started from:

```bash
cd src
for f in main.c codegen.c compile.c console.c defer.c error.c expressions.c \
         flow3.c fs.c hashmap.c object.c options.c osstream.c parser.c \
         pre_expressions.c target.c tokenizer.c token.c type.c; do
  cake -fanalyzer -nullable=enabled -no-output -fdiagnostics-color=never "$f"
done
```

Compare per-file counts, not just the total — a fix and a regression can cancel
out. Build a binary from the base commit and diff the two runs.

## failing/

Correct C that flow3 still gets wrong. See `failing/README.md`. Move a file up
here once fixed, and drop its `TODO` suppressions so the fix stays locked in.
