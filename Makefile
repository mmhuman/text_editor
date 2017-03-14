all: a
	
a: declaration.h commands.c A.c
	gcc declaration.h commands.c A.c -Wall -ansi -pedantic -o a
