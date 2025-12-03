%{
    #include <stdio.h>
    #include <math.h>
    #include <string.h>
    #include <stdlib.h>
    #include "../types.h"

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

    Variables_t *current_variables = NULL;
    Variable_t *last_variable = NULL;


    void insert_into_constraints_list(Constraints_t* constraints, Constraint_t *element) {
        if (constraints->head == NULL) {
            constraints->head = element;
            return;
        }

        Constraint_t* head = constraints->head;
        while (head->next != NULL) head = head->next;
        head->next = element;
    }

    void insert_into_variables_list(Variables_t* variables, Variable_t *element) {
        if (variables->head == NULL) {
            variables->head = element;
            return;
        }

        Variable_t* head = variables->head;
        while (head->next != NULL) head = head->next;
        head->next = element;
    }

    int extract_index_from_variable(char *variable) {
        int value = 0;
        int length = strlen(variable);
        int index = length - 1;
        while (index > 0) {
            int digit = variable[index] - '0';
            value += digit * pow(10, (length - 1) - index);
            --index;
        }
        return value;
    }

    void parse_variable(int coefficient, char *variable_str) {
        int index = extract_index_from_variable(variable_str);
        Variable_t* variable = (Variable_t*)malloc(sizeof(Variable_t));
        variable->coefficient = coefficient;
        variable->index = index;
        variable->next = NULL;
        insert_into_variables_list(current_variables, variable);

        last_variable = variable;
    }
%}

%union {
    int i;
    char *s;
}

%token _FUNDEF
%token <s> _VAR
%token <i> _NUM
%token _EQ
%token _PLUS
%token _MINUS
%token _LT
%token _GT
%token _SEMICOLON

%type <i> arithmetic_operator 

%%

simplex_problem
    : function constraints {
        problem.function = function;
        problem.constraints = constraints;
    }
    ;

function
    : _FUNDEF _EQ variables _SEMICOLON
    {
        function = (Function_t*)malloc(sizeof(Function_t));
        function->variables = current_variables;
        current_variables = NULL;
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
        constraint->value = $3;
        
        if (constraints == NULL)
            constraints = (Constraints_t*)malloc(sizeof(Constraints_t));

        insert_into_constraints_list(constraints, constraint);

        current_variables = NULL;
    }
    ;

variables
    : variable 
    | variables arithmetic_operator variable {
        if ($2 == SUBTRACT)
            last_variable->coefficient *= -1;
    }
    ;

relational_operator
    : _LT
    | _GT
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
        parse_variable($1, $2);
    }
    | _VAR { 
        if (current_variables == NULL)
            current_variables = (Variables_t*)malloc(sizeof(Variable_t));
        parse_variable(1, $1);
    }
    ;

%%

int yyerror(char *error) {
    fprintf(stderr, "Error on line %d: %s\n", yylineno, error);
    
    return 0;
}
