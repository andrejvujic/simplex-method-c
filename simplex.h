#ifndef _SIMPLEX_H
#define _SIMPLEX_H

void print_table(
    float **table, int table_rows, int table_cols,
    int *non_basic_variables, int non_basic_variables_num,
    int *basic_variables, int basic_variables_num);

void print_result(
    float **table, int table_rows, int table_cols,
    int *non_basic_variables, int non_basic_variables_num,
    int *basic_variables, int basic_variables_num);

#endif