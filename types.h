#ifndef _TYPES_H
#define _TYPES_H
enum ArithmeticOperations
{
    ADD,
    SUBTRACT
};

enum FunctionOptimumType
{
    MIN,
    MAX
};

typedef struct Variable
{
    struct Variable *next;
    int coefficient, index;
} Variable_t;

typedef struct Variables
{
    Variable_t *head;
} Variables_t;

typedef struct Constraint
{
    struct Constraint *next;
    Variables_t *variables;
    int value;
} Constraint_t;

typedef struct Constraints
{
    Constraint_t *head;
} Constraints_t;

typedef struct Function
{
    int optimum_type;
    Variables_t *variables;
} Function_t;

typedef struct OptimizationProblem
{
    Function_t *function;
    Constraints_t *constraints;
} OptimizationProblem_t;

#endif