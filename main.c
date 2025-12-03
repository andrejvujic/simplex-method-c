#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "constraints.h"
#include "variables.h"

extern int yyparse(void);
extern FILE *yyin;
extern OptimizationProblem_t problem;

int get_max_variable_index(Function_t *function, Constraints_t *constraints)
{
    int max_index = INT32_MIN;
    Variables_t *function_variables = function->variables;
    Variable_t *variable = function_variables->head;

    while (variable != NULL)
    {
        if (variable->index > max_index)
            max_index = variable->index;
        variable = variable->next;
    }

    /*
    All of the variables should already be used in the function?

    Constraint_t *constraint = constraints->head;
    while (constraint != NULL)
    {
        variable = constraint->variables->head;
        while (variable != NULL)
        {
            if (variable->index > max_index)
                max_index = variable->index;
            variable = variable->next;
        }
        constraint = constraint->next;
    }
    */
    return max_index;
}

void populate_non_basic_variables(Variables_t *function_variables, int *basic_variables)
{
    Variable_t *variable = function_variables->head;
    int index = 0;
    while (variable != NULL)
    {
        basic_variables[index++] = variable->index;
        variable = variable->next;
    }
}

void populate_basic_variables(int last_variable_index, int *non_basic_variables, int non_basic_variables_num)
{
    int index = 0;
    while (index < non_basic_variables_num)
        non_basic_variables[index++] = ++last_variable_index;
}

void populate_table(
    int **table, int table_rows, int table_cols,
    int *basic_variables, int *non_basic_variables,
    int basic_variables_num, int non_basic_variables_num,
    Function_t *function, Constraints_t *constraints)
{
    int current_row = 0;

    Constraint_t *constraint = constraints->head;
    while (constraint != NULL)
    {
        int current_col = 0;
        table[current_row][current_col++] = constraint->value;

        for (int index = 0; index < non_basic_variables_num; ++index)
            table[current_row][current_col++] = get_variable_coefficient_by_index(constraint->variables, non_basic_variables[index]);

        constraint = constraint->next;
        ++current_row;
    }

    int current_col = 0;
    table[current_row][current_col++] = 0; // Sets the function value to zero...
    Variable_t *function_variable = function->variables->head;
    while (function_variable != NULL)
    {
        for (int index = 0; index < non_basic_variables_num; ++index)
            table[current_row][current_col++] = get_variable_coefficient_by_index(function->variables, non_basic_variables[index]);

        function_variable = function_variable->next;
    }
}

void print_table(int **table, int table_rows, int table_cols)
{
    for (int i = 0; i < table_rows; ++i)
    {
        for (int j = 0; j < table_cols; ++j)
            printf("%d ", table[i][j]);
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "You need to provide a file which specifies the optimization problem.\n");
        fprintf(stderr, "Please check the examples folder to see how it's supposed to be formatted.\n");
        fprintf(stderr, "Example usage: ./simplex path/to/optimization_problem\n");
        exit(EXIT_FAILURE);
    }

    printf("Reading and parsing optimization problem! from: %s\n", argv[1]);

    yyin = fopen(argv[1], "r");
    if (!yyin)
    {
        fprintf(stderr, "Couldn't parse optimization problem!\n");
        exit(EXIT_FAILURE);
    }

    yyparse();

    Function_t *function = problem.function;
    Constraints_t *constraints = problem.constraints;

    int max_variable_index = get_max_variable_index(function, constraints);
    int last_variable_index = max_variable_index;

    printf("\nYour optimization problem:\n");
    printf("--------------------------\n");
    printf("Function to optimize: f(x)");

    Variables_t *function_variables = function->variables;
    Variable_t *function_variable = function_variables->head;
    int variable_index = 0;
    while (function_variable != NULL)
    {
        function_variable->coefficient = -1 * function_variable->coefficient;
        int coefficient = function_variable->coefficient;

        if (coefficient > 0 && variable_index > 0)
            printf("+");

        printf("%dx%d", coefficient, function_variable->index);
        function_variable = function_variable->next;
        ++variable_index;
    }
    printf("=0\n");

    printf("\nConstraints:\n");

    int constraint_index = 0;
    Constraint_t *constraint = constraints->head;
    while (constraint != NULL)
    {
        print_constraint(constraint);
        printf(" -> ");
        transform_constraint(constraint, &last_variable_index);
        print_transformed_constraint(constraint);
        constraint = constraint->next;
    }
    printf("--------------------------\n");
    fclose(yyin);

    int non_basic_variables_num = function->variables_num;
    int basic_variables_num = last_variable_index - function->variables_num;
    int *non_basic_variables = (int *)(malloc(sizeof(int) * basic_variables_num));
    int *basic_variables = (int *)(malloc(sizeof(int) * non_basic_variables_num));

    // At the start of the algorithm the additional variables are the
    // basic variables.
    populate_basic_variables(last_variable_index, basic_variables, basic_variables_num);

    // Extract function variables into the non basic variables.
    // This means that the function variables will be zero at the start
    // of the simplex algorithm.
    populate_non_basic_variables(function_variables, non_basic_variables);

    // The number of rows is calculated based on the number of
    // non basic variables - an additional row and an additional column
    // are also required (that's why there's a + 1).
    int table_rows = basic_variables_num + 1;
    int table_cols = non_basic_variables_num + 1;

    int **table = (int **)malloc(sizeof(int *) * table_rows);
    for (int index = 0; index < table_rows; ++index)
        table[index] = (int *)malloc(sizeof(int) * table_cols);
    populate_table(
        table,
        table_rows, table_cols,
        basic_variables, non_basic_variables,
        basic_variables_num, non_basic_variables_num,
        function, constraints);
    print_table(table, table_rows, table_cols);

    // Freeing of allocated memory:
    for (int index = 0; index < table_rows; ++index)
        free(table[index]);
    free(table);

    free(basic_variables);
    free(non_basic_variables);
}