#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "types.h"
#include "variables.h"

void insert_into_variables_list(Variables_t *variables, Variable_t *element)
{
    if (variables->head == NULL)
    {
        variables->head = element;
        return;
    }

    Variable_t *head = variables->head;
    while (head->next != NULL)
        head = head->next;
    head->next = element;
}

void parse_variable(Variables_t *current_variables, Variable_t **last_variable, int coefficient, char *variable_str)
{
    int index = extract_index_from_variable(variable_str);
    Variable_t *variable = (Variable_t *)malloc(sizeof(Variable_t));
    variable->coefficient = coefficient;
    variable->index = index;
    variable->next = NULL;

    insert_into_variables_list(current_variables, variable);

    if (last_variable != NULL)
        *last_variable = variable;
}

int extract_index_from_variable(char *variable)
{
    int value = 0;
    int length = strlen(variable);
    int index = length - 1;
    while (index > 0)
    {
        int digit = variable[index] - '0';
        value += digit * pow(10, (length - 1) - index);
        --index;
    }
    return value;
}

int get_variable_coefficient_by_index(Variables_t *variables, int index)
{
    Variable_t *variable = variables->head;
    while (variable != NULL)
    {
        if (variable->index == index)
            return variable->coefficient;
        variable = variable->next;
    }
    return 0;
}