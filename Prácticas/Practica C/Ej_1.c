// Armar un código en C para mostrar por pantalla los diferentes tamaños de los tipos: char, short,
// int, long, float y double (utilizar la función sizeof).
 
#include <stdio.h>

int main() {
    printf("Tamaño de char:   %lu bytes\n", sizeof(char));
    printf("Tamaño de short:  %lu bytes\n", sizeof(short));
    printf("Tamaño de int:    %lu bytes\n", sizeof(int));
    printf("Tamaño de long:   %lu bytes\n", sizeof(long));
    printf("Tamaño de float:  %lu bytes\n", sizeof(float));
    printf("Tamaño de double: %lu bytes\n", sizeof(double));
    
    return 0;
}