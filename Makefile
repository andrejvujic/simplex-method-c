CC     = gcc
BISON  = bison
FLEX   = flex
OUTPUT = simplex

all: parser/parser.tab.c parser/lex.yy.c $(OUTPUT)

parser/parser.tab.c: parser/parser.y
	$(BISON) -d -o parser/parser.tab.c parser/parser.y

parser/lex.yy.c: parser/parser.tab.c parser/lexer.l
	$(FLEX) -o parser/lex.yy.c parser/lexer.l

$(OUTPUT): parser/parser.tab.c parser/lex.yy.c main.c constraints.c variables.c simplex.c
	$(CC) -o $(OUTPUT) main.c parser/parser.tab.c parser/lex.yy.c constraints.c variables.c simplex.c lib/fort/fort.c

clean:
	rm -rf parser/parser.tab.c
	rm -rf parser/parser.tab.h
	rm -rf parser/lex.yy.c
	rm -rf $(OUTPUT)