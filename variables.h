#ifndef VARIABLES_H_
#define VARIABLES_H_

#include "types.h"

void insert_into_variables_list(Variables_t *variables, Variable_t *element);
void free_variables(Variables_t *variables);
void parse_variable(Variables_t *current_variables, Variable_t **last_variable, float coefficient, char *variable_str);
int extract_index_from_variable(char *variable);
float get_variable_coefficient_by_index(Variables_t *variables, int index);

#endif