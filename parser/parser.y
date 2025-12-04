%{
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include "../types.h"
    #include "../constraints.h"
    #include "../variables.h"
    
    int yyparse(void);
    int yylex(void);
    int yyerror(char *);
    extern int yylineno;

    OptimizationProblem_t problem = {
        .function = NULL,
        .constraints = NULL
    };

    Function_t *function;
    Constraints_t *constraints;

    int current_variables_num = 0;
    Variables_t *current_variables = NULL;
    Variable_t *last_variable = NULL;
%}

%union {
    int i;
    char *s;
}

%token _FUNCTION_DEFINITION
%token <i> _OPTIMUM_TYPE
%token <s> _VAR
%token <i> _NUM
%token _EQ
%token _PLUS
%token _MINUS
%token _LT
%token _GT
%token _SEMICOLON

%type <i> arithmetic_operator 
%type <i> relational_operator

%%

simplex_problem
    : function constraints {
        problem.function = function;
        problem.constraints = constraints;
    }
    ;

function
    : _OPTIMUM_TYPE _FUNCTION_DEFINITION _EQ variables _SEMICOLON
    {
        function = (Function_t*)malloc(sizeof(Function_t));
        function->variables = current_variables;
        function->variables_num = current_variables_num;
        function->optimum_type = $1;
        current_variables = NULL;
        current_variables_num = 0;
    }
    ;

constraints
    : constraint
    | constraints constraint
    ;

constraint
    : variables relational_operator _NUM _SEMICOLON
    {
        Constraint_t* constraint = (Constraint_t*)malloc(sizeof(Constraint_t));
        constraint->variables = current_variables;
        constraint->type = $2;
        constraint->value = $3;
        
        if (constraints == NULL)
            constraints = (Constraints_t*)malloc(sizeof(Constraints_t));

        insert_into_constraints_list(constraints, constraint);

        current_variables = NULL;
        current_variables_num = 0;
    }
    ;

variables
    : variable 
    | variables arithmetic_operator variable {
        if ($2 == SUBTRACT && last_variable != NULL) {
            last_variable->coefficient *= -1;
        }
    }
    ;

relational_operator
    : _LT { $$ = LT; }
    | _GT { $$ = GT; }
    ;

arithmetic_operator
    : _PLUS { $$ = ADD; }
    | _MINUS { $$ = SUBTRACT; }
    ;

variable
    : _NUM _VAR
    { 
        if (current_variables == NULL)
            current_variables = (Variables_t*)malloc(sizeof(Variable_t));
        parse_variable(current_variables, &last_variable, $1, $2);
        ++current_variables_num;
    }
    | _VAR { 
        if (current_variables == NULL)
            current_variables = (Variables_t*)malloc(sizeof(Variable_t));
        parse_variable(current_variables, &last_variable, 1, $1);
        ++current_variables_num;
    }
    ;

%%

int yyerror(char *error) {
    fprintf(stderr, "Error on line %d: %s\n", yylineno, error);
    return 0;
}
