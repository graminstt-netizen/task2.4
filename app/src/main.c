/*
lib_main.c - реализация функций библиотеки (только для Windows).

Бабурин Дмитрий Сергеевич
МК-101
*/


#include <lib_main.h>
 
#include <stdio.h>


int main (int argc, char *argv[], char *envp[]) {

int a = 2;
int b = 3;

	printf ("%d + %d = %d\n", a, b, LibAddFunction(a, b));
	return 0;
}
