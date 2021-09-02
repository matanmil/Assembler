all:assembler

assembler: assembler.c Input.c Input.h system.c system.h compile.c compile.h OutputBuild.c OutputBuild.h
	gcc  assembler.c Input.c Input.h system.c system.h compile.c compile.h OutputBuild.c OutputBuild.h -g -Wall -ansi -pedantic -o assembler

clean:
	rm*~
