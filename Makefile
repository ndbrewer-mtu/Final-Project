prog: code.c
	gcc -I . code.c -l:libnetpbm.so.11 -o prog
