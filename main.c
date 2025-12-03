#include <stdio.h>
#include <stdlib.h>
#include "types.h"

extern int yyparse(void);
extern FILE *yyin;
extern OptimizationProblem_t problem;

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

    yyparse();

    int index = 0;
    Constraint_t *constraint = problem.constraints->head;
    while (constraint != NULL)
    {
        Variable_t *variable = constraint->variables->head;
        printf("Constraint %d:\n", index++);
        while (variable != NULL)
        {
            printf("%dx%d\n", variable->coefficient, variable->index);
            variable = variable->next;
        }

        printf("\n\n\n");
        constraint = constraint->next;
    }

    fclose(yyin);
}