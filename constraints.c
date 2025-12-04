#include <stdlib.h>
#include <stdio.h>

#include "constraints.h"
#include "variables.h"

void insert_into_constraints_list(Constraints_t *constraints, Constraint_t *element)
{
    if (constraints->head == NULL)
    {
        constraints->head = element;
        return;
    }

    Constraint_t *head = constraints->head;
    while (head->next != NULL)
        head = head->next;
    head->next = element;
}

void transform_constraint(Constraint_t *constraint, int *last_variable_index)
{

    int new_variable_index = ++(*last_variable_index);
    Variables_t *variables = constraint->variables;

    Variable_t *new_variable = (Variable_t *)malloc(sizeof(Variable_t));
    new_variable->coefficient = 1;
    new_variable->index = new_variable_index;

    if (constraint->type == GT)
    {
        constraint->value *= -1;
        Variable_t *variable = constraint->variables->head;

        while (variable != NULL)
        {
            variable->coefficient *= -1;
            variable = variable->next;
        }
    }

    insert_into_variables_list(variables, new_variable);
}

void print_constraint(Constraint_t *constraint)
{
    int variable_index = 0;
    Variable_t *variable = constraint->variables->head;

    while (variable != NULL)
    {
        float coefficient = variable->coefficient;
        if (coefficient > 0 && variable_index > 0)
            printf("+");

        printf("%fx%d", variable->coefficient, variable->index);

        variable = variable->next;
        ++variable_index;
    }

    printf("%s%f", (constraint->type == LT ? "<=" : ">="), constraint->value);
}

void print_transformed_constraint(Constraint_t *constraint)
{
    int variable_index = 0;
    Variable_t *variable = constraint->variables->head;

    while (variable != NULL)
    {
        float coefficient = variable->coefficient;
        if (coefficient > 0 && variable_index > 0)
            printf("+");

        printf("%fx%d", variable->coefficient, variable->index);

        variable = variable->next;
        ++variable_index;
    }

    printf("=%f", constraint->value);

    printf("\n");
}
