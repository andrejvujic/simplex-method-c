%option yylineno

%{
    #include <string.h>
    #include "../types.h"
    #include "parser.tab.h"
%}

%%

[ \n\t]+        { }
x[0-9]+         { yylval.s = strdup(yytext); return _VAR; }
[-]?[1-9][0-9]* { yylval.i = atoi(yytext); return _NUM; }
"f(x)"          { return _FUNCTION_DEFINITION; }
"max"           { yylval.i = MAX; return _OPTIMUM_TYPE; }
"min"           { yylval.i = MIN; return _OPTIMUM_TYPE; }
"="             { return _EQ; }
"<="            { return _LT; }
">="            { return _GT; }
"+"             { return _PLUS; }
"-"             { return _MINUS; } 
";"             { return _SEMICOLON; }
.               { printf("Unexpected character: %c on line %d\n", yylineno, *yytext); exit(1); }
%%

int yywrap(void) {
    return 1;
}