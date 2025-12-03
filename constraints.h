#ifndef CONSTRAINTS_H_
#define CONSTRAINTS_H_

#include "types.h"

void insert_into_constraints_list(Constraints_t *constraints, Constraint_t *element);
void transform_constraint(Constraint_t *constraint, int *additional_variable_index);
void print_constraint(Constraint_t *constraint);
void print_transformed_constraint(Constraint_t *constraint);

#endif