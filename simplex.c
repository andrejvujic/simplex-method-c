#include "simplex.h"
#include "string.h"
#include <stdio.h>
#include "lib/fort/fort.h"

void print_table(
    float **table, int table_rows, int table_cols,
    int *non_basic_variables, int non_basic_variables_num,
    int *basic_variables, int basic_variables_num)
{
    ft_table_t *ft_table = ft_create_table();
    ft_set_border_style(ft_table, FT_DOUBLE2_STYLE);

    ft_printf(ft_table, " | ");
    for (int index = 0; index < non_basic_variables_num; ++index)
        ft_printf(ft_table, "x%d", non_basic_variables[index]);
    ft_ln(ft_table);

    for (int row_index = 0; row_index < table_rows; ++row_index)
    {
        if (row_index < table_rows - 1)
            ft_printf(ft_table, "x%d", basic_variables[row_index]);
        else
            ft_printf(ft_table, "f");

        for (int col_index = 0; col_index < table_cols; ++col_index)
            ft_printf(ft_table, "%.3f", table[row_index][col_index]);
        ft_ln(ft_table);
    }

    printf("%s", ft_to_string(ft_table));
    ft_destroy_table(ft_table);
}

void print_result(
    float **table, int table_rows, int table_cols,
    int *basic_variables, int basic_variables_num,
    int *non_basic_variables, int non_basic_variables_num)
{
    printf("\n--------------------------\n");
    printf("Result of Simplex method:\n");

    ft_table_t *ft_table = ft_create_table();
    ft_set_border_style(ft_table, FT_DOUBLE2_STYLE);

    ft_printf(ft_table, "f");

    for (int index = 0; index < basic_variables_num; ++index)
        ft_printf(ft_table, "x%d", basic_variables[index]);

    for (int index = 0; index < non_basic_variables_num; ++index)
        ft_printf(ft_table, "x%d", non_basic_variables[index]);

    ft_ln(ft_table);

    ft_printf(ft_table, "%.3f", table[table_rows - 1][0]);

    for (int index = 0; index < table_rows - 1; ++index)
        ft_printf(ft_table, "%.3f", table[index][0]);

    for (int index = 0; index < non_basic_variables_num; ++index)
        ft_printf(ft_table, "0");

    printf("%s", ft_to_string(ft_table));
    ft_destroy_table(ft_table);
}