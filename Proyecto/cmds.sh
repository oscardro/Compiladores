bison simple.y
flex simple.l
gcc -o simple.exe simple.tab.c lex.yy.c -lfl -ly
