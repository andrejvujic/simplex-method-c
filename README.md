# Simplex Method in C

A compact, educational implementation of the [simplex algorithm](https://en.wikipedia.org/wiki/Simplex_algorithm) for solving linear-programming problems.

The program reads a human-friendly problem description, parses it with Flex and Bison, converts the constraints to a simplex tableau, and prints every pivot step before reporting the optimum.

## Features

- Supports both `max` and `min` objective functions.
- Accepts linear constraints written with `<=` or `>=`.
- Generates slack variables automatically.
- Shows the initial tableau and every subsequent pivot operation.
- Prints the objective value and all basic and non-basic variables at the end.
- Includes eight ready-to-run examples.

## Requirements

The build uses the following tools and libraries:

| Dependency | Why it is used                                                                                                                             |
| ---------- | ------------------------------------------------------------------------------------------------------------------------------------------ |
| Bison      | Generates the syntax parser from `parser/parser.y`.                                                                                        |
| Flex       | Generates the lexical scanner from `parser/lexer.l`.                                                                                       |
| libfort    | Renders the simplex tableaus as readable Unicode tables. A copy is included in `lib/fort`, so it does not need to be installed separately. |

## Build and run

Build the executable from the project root:

```sh
make
```

Run it by passing a problem file as the only argument:

```sh
./simplex examples/example_7.txt
```

Remove the executable and generated parser files with:

```sh
make clean
```

## Input format

A problem starts with one objective function, followed by one or more constraints. Every line must end with a semicolon.

```text
max f(x) = x1 + x2;
x1 + 2x2 <= 4;
2x1 + x2 <= 6;
```

The accepted form is:

```text
max|min f(x) = <linear expression>;
<linear expression> <=|>= <number>;
```

Variables are named `x1`, `x2`, and so on. Integer and decimal coefficients are accepted, and a coefficient of `1` may be omitted. The current implementation expects variables to be numbered consecutively and all decision variables to appear in the objective function. As in the usual simplex standard form, decision variables are treated as non-negative.

## Example

For `examples/example_7.txt`, the program starts with

```text
maximize  f(x) = x1 + x2

subject to
    x1 + 2x2 <= 4
   2x1 +  x2 <= 6
```

It introduces `x3` and `x4` as slack variables, builds the initial tableau, and performs two pivots. The final table printed by the program is:

```text
╔═══════╤═══════╤═══════╤════╤════╗
║ f     │ x2    │ x1    │ x4 │ x3 ║
╟───────┼───────┼───────┼────┼────╢
║ 3.333 │ 0.667 │ 2.667 │ 0  │ 0  ║
╚═══════╧═══════╧═══════╧════╧════╝
```

Therefore, the optimum is approximately

```text
x1 = 2.667
x2 = 0.667
f  = 3.333
```

The values are displayed to three decimal places.

## How it works

1. **Tokenize the input.** Flex recognizes keywords, variable names, numbers, arithmetic operators, relation operators, and semicolons.
2. **Parse the problem.** Bison checks the grammar and constructs linked-list representations of the objective function and constraints.
3. **Create standard-form equations.** The objective coefficients are moved into tableau form. Each constraint receives a new slack variable; `>=` rows are multiplied by `-1` first.
4. **Build the initial tableau.** Decision variables begin as non-basic variables, while the generated slack variables form the initial basis.
5. **Choose a pivot.** For maximization, the solver selects the most negative objective-row coefficient; for minimization, it selects the most positive one. A ratio test chooses the leaving row.
6. **Pivot the tableau.** The entering and leaving variables are swapped and all tableau cells are recalculated.
7. **Repeat until optimal.** Pivoting stops when the objective row contains no coefficient that can improve the requested optimum. The first column then contains the objective value and values of the basic variables; non-basic variables are zero.

## Project structure

```text
.
├── main.c                 Program entry point and simplex iteration logic
├── simplex.c/.h           Tableau and final-result rendering
├── constraints.c/.h       Constraint storage, transformation, and printing
├── variables.c/.h         Variable-list utilities and coefficient lookup
├── types.h                Shared enums and data structures
├── Makefile               Parser generation, compilation, and cleanup
├── parser/
│   ├── lexer.l            Flex token definitions
│   ├── parser.y           Bison grammar and problem construction
│   ├── lex.yy.c           Generated scanner
│   ├── parser.tab.c        Generated parser
│   └── parser.tab.h        Generated parser declarations
├── lib/fort/
│   ├── fort.c
│   └── fort.h             Vendored libfort table-formatting library
└── examples/
    └── example_1.txt ... example_8.txt
                            Sample optimization problems
```

The generated files in `parser/` should not normally be edited by hand. Change `lexer.l` or `parser.y`, then run `make` to regenerate them.

## Implementation notes

This project is intended to demonstrate the mechanics of the simplex method rather than replace a full linear-programming package.

- Tableau values use single-precision `float` arithmetic.
- Output is rounded to three decimal places.
- The solver limits a run to 20 pivot steps (`MAX_STEPS` in `main.c`).

## About the Simplex method

For background, geometric intuition, variants, and the history of the method, see [Simplex algorithm on Wikipedia](https://en.wikipedia.org/wiki/Simplex_algorithm).
