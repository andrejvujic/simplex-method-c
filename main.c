#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "types.h"
#include "constraints.h"
#include "variables.h"
#include "simplex.h"

#define EPS 1e-6
#define MAX_STEPS 20

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

void populate_basic_variables(int max_variable_index, int *non_basic_variables, int non_basic_variables_num)
{
    int index = 0;
    while (index < non_basic_variables_num)
        non_basic_variables[index++] = ++max_variable_index;
}

void populate_table(
    float **table, int table_rows, int table_cols,
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

int get_pivot_row(float **table, float *division_result, int pivot_col, int table_rows)
{
    // TODO: Implement checking if any positive division results even exist.

    float smallest = FLT_MAX;
    int pivot_row = -1;
    // We need to find the smallest positive value... That
    // element's row becomes the pivot row.
    for (int row_index = 0; row_index < table_rows - 1; ++row_index)
    {
        float value = division_result[row_index];
        int infinity = value == FLT_MAX || value == FLT_MIN;
        if (infinity)
            printf("%.3f -> %s\n", table[row_index][pivot_col], "infinity");
        else
            printf("%.3f -> %.3f\n", table[row_index][pivot_col], value);

        // We need the smallest positive value...
        // Negative values can be skipped.
        if (division_result[row_index] < 0)
            continue;

        if (division_result[row_index] < smallest)
        {
            smallest = division_result[row_index];
            pivot_row = row_index;
        }
    }
    return pivot_row;
}

int get_pivot_column(float **table, int table_rows, int table_cols, int optimum_type)
{
    // Calculates the pivot column based on
    // which optimum type we're looking for - either the minimum
    // or the maxmimum of the function.
    int pivot_col = -1;
    switch (optimum_type)
    {
    case MAX:
    {
        float smallest = FLT_MAX;

        for (int col_index = 1; col_index < table_cols; ++col_index)
            if (table[table_rows - 1][col_index] < smallest)
            {
                smallest = table[table_rows - 1][col_index];
                pivot_col = col_index;
            }

        if (pivot_col >= 0)
            printf("\nSmallest value: %.3f, column index: %d\n", smallest, pivot_col);
        break;
    }
    case MIN:
    {
        float biggest = FLT_MIN;

        for (int col_index = 1; col_index < table_cols; ++col_index)
            if (table[table_rows - 1][col_index] > biggest)
            {
                biggest = table[table_rows - 1][col_index];
                pivot_col = col_index;
            }

        if (pivot_col >= 0)
            printf("\nBiggest value: %.3f, column index: %d\n", biggest, pivot_col);
        break;
    }
    }

    return pivot_col;
}

void repopulate_table(float **table, int table_rows, int table_cols, int pivot_row, int pivot_col)
{
    float pivot = table[pivot_row][pivot_col];

    float **table_copy = (float **)malloc(sizeof(float *) * table_rows);
    for (int index = 0; index < table_rows; ++index)
        table_copy[index] = (float *)malloc(sizeof(float) * table_cols);

    for (int row_index = 0; row_index < table_rows; ++row_index)
        for (int col_index = 0; col_index < table_cols; ++col_index)
            table_copy[row_index][col_index] = table[row_index][col_index];

    for (int row_index = 0; row_index < table_rows; ++row_index)
        for (int col_index = 0; col_index < table_cols; ++col_index)
        {
            if (row_index == pivot_row && col_index == pivot_col)
            {
                table[row_index][col_index] = 1.0 / pivot;
                continue;
            }

            if (row_index == pivot_row)
            {
                // The element is located in the pivot row.
                table[row_index][col_index] = table_copy[row_index][col_index] / pivot;
                continue;
            }

            if (col_index == pivot_col)
            {
                // The element is located in the pivot column.
                table[row_index][col_index] = -table_copy[row_index][col_index] / pivot;
                continue;
            }

            float pivot_col_element = table_copy[row_index][pivot_col];
            float pivot_row_element = table_copy[pivot_row][col_index];
            table[row_index][col_index] = table_copy[row_index][col_index] - pivot_row_element * pivot_col_element / pivot;
        }

    // Free the copy of the table:
    for (int index = 0; index < table_rows; ++index)
        free(table_copy[index]);
    free(table_copy);
}

void swap_variables(int *basic_variables, int *non_basic_variables, int pivot_row, int pivot_col)
{
    int adjusted_pivot_col = pivot_col - 1;
    int tmp = non_basic_variables[adjusted_pivot_col];
    non_basic_variables[adjusted_pivot_col] = basic_variables[pivot_row];
    basic_variables[pivot_row] = tmp;
}

int is_another_step_required(float **table, int table_rows, int table_cols, int optimum_type)
{
    // Checks if there's more room for optimization... If we have to
    // do another step of the Simplex method.
    for (int col_index = 1; col_index < table_cols; ++col_index)
    {
        float value = table[table_rows - 1][col_index];
        switch (optimum_type)
        {
        case MAX:
            if (value < 0)
                return 1;
            break;

        case MIN:
            if (value > 0)
                return 1;
            break;
        }
    }

    return 0;
}

void simplex_step(float **table, int table_rows, int table_cols, int *basic_variables, int *non_basic_variables, int basic_variables_num, int non_basic_variables_num, int optimum_type, int step_index)
{
    if (step_index + 1 > MAX_STEPS)
    {
        printf("\nAlgorithm didn't converege %d step(s)...\nAre you sure that the optimum if your function isn't infinity?\nIf you think that more steps are required you can edit the macro which defines the max step count.\n", MAX);
        exit(1);
    }

    int pivot_col = get_pivot_column(table, table_rows, table_cols, optimum_type);
    if (pivot_col < 0)
        return;

    printf("\n--------------------------\n");
    printf("Performing Simplex step number %d:\n", step_index + 1);

    printf("Dividing first column elements by corresponding pivot column elements...\n");

    float *division_result = (float *)malloc(sizeof(float) * (table_rows - 1));
    for (int row_index = 0; row_index < table_rows - 1; ++row_index)
    {
        float value1 = table[row_index][0];
        float value2 = table[row_index][pivot_col];
        if (value2 > -EPS && value2 < EPS)
        {
            division_result[row_index] = FLT_MAX;
            continue;
        }
        division_result[row_index] = value1 / value2;
    }

    int pivot_row = get_pivot_row(table, division_result, pivot_col, table_rows);
    free(division_result);
    division_result = NULL;
    if (pivot_row == -1)
        return;

    printf("\nThe pivot element is (%d, %d) = %.3f\n", pivot_row, pivot_col, table[pivot_row][pivot_col]);

    printf("\nSwapping variables and recalculating table elements...\n");
    swap_variables(basic_variables, non_basic_variables, pivot_row, pivot_col);
    repopulate_table(table, table_rows, table_cols, pivot_row, pivot_col);

    print_table(
        table, table_rows, table_cols,
        non_basic_variables, non_basic_variables_num,
        basic_variables, basic_variables_num);

    // We need to check if another Simplex method step is required.
    if (is_another_step_required(table, table_rows, table_cols, optimum_type))
        simplex_step(
            table,
            table_rows, table_cols,
            basic_variables, non_basic_variables,
            basic_variables_num, non_basic_variables_num,
            optimum_type,
            step_index + 1);
}

void free_problem(Function_t **function, Constraints_t **constraints)
{
    if (*function != NULL)
    {
        Variables_t *variables = (*function)->variables;
        free_variables(variables);
        free(*function);
        *function = NULL;
    }

    if (*constraints != NULL)
    {
        while ((*constraints)->head != NULL)
        {
            Constraint_t *constraint = (*constraints)->head;
            (*constraints)->head = constraint->next;
            free_variables(constraint->variables);
            free(constraint);
        }
        free(*constraints);
        *constraints = NULL;
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

    printf("Reading and parsing optimization problem from: %s\n", argv[1]);

    yyin = fopen(argv[1], "r");
    if (!yyin)
    {
        fprintf(stderr, "Couldn't parse optimization problem!\n");
        exit(EXIT_FAILURE);
    }

    if (yyparse())
    {
        printf("Please check if your problem specification follows the rules.\n");
        free_problem(&problem.function, &problem.constraints);
        fclose(yyin);
        exit(1);
    }
    fclose(yyin);

    Function_t *function = problem.function;
    Constraints_t *constraints = problem.constraints;

    int max_variable_index = get_max_variable_index(function, constraints);
    int last_variable_index = max_variable_index;

    printf("\nYour optimization problem:\n");
    printf("--------------------------\n");
    printf("Function to optimize:\n");
    printf("We are looking for the function %s\n", function->optimum_type == MAX ? "maximum" : "minimum.");

    Variables_t *function_variables = function->variables;
    Variable_t *function_variable = function_variables->head;
    int variable_index = 0;
    printf("f(x)");
    while (function_variable != NULL)
    {
        function_variable->coefficient = -1.0 * function_variable->coefficient;
        float coefficient = function_variable->coefficient;

        if (coefficient > 0 && variable_index > 0)
            printf("+");

        printf("%.3fx%d", coefficient, function_variable->index);
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
    printf("\n--------------------------\n");

    int non_basic_variables_num = function->variables_num;
    int basic_variables_num = last_variable_index - function->variables_num;

    int *non_basic_variables = (int *)(malloc(sizeof(int) * non_basic_variables_num));
    int *basic_variables = (int *)(malloc(sizeof(int) * basic_variables_num));

    // At the start of the algorithm the additional variables are the
    // basic variables.
    populate_basic_variables(max_variable_index, basic_variables, basic_variables_num);

    // Extract function variables into the non basic variables.
    // This means that the function variables will be zero at the start
    // of the simplex algorithm.
    populate_non_basic_variables(function_variables, non_basic_variables);

    // The number of rows is calculated based on the number of
    // non basic variables - an additional row and an additional column
    // are also required (that's why there's a + 1).
    int table_rows = basic_variables_num + 1;
    int table_cols = non_basic_variables_num + 1;

    float **table = (float **)malloc(sizeof(float *) * table_rows);
    for (int index = 0; index < table_rows; ++index)
        table[index] = (float *)malloc(sizeof(float) * table_cols);
    populate_table(
        table,
        table_rows, table_cols,
        basic_variables, non_basic_variables,
        basic_variables_num, non_basic_variables_num,
        function, constraints);

    printf("Initial Simplex table:\n");
    print_table(
        table, table_rows, table_cols,
        non_basic_variables, non_basic_variables_num,
        basic_variables, basic_variables_num);

    simplex_step(
        table,
        table_rows, table_cols,
        basic_variables, non_basic_variables,
        basic_variables_num, non_basic_variables_num,
        function->optimum_type,
        0);

    print_result(table, table_rows, table_cols, basic_variables, basic_variables_num, non_basic_variables, non_basic_variables_num);

    // Freeing the table:
    for (int index = 0; index < table_rows; ++index)
        free(table[index]);

    free(table);

    // Freeing the optimization problem
    // including the function, constraints and
    // their variables...
    free_problem(&function, &constraints);

    free(basic_variables);

    free(non_basic_variables);
}