/*
6. Se tiene la siguiente declaración:
int x[5];
int *ptr;
a) ¿Cómo haría para que el puntero ptr apunte a la primera posición del vector x?
b) ¿Recorrer el vector completo utilizando incrementos en el puntero ptr?
c) Realizar un printf del puntero ptr para cada incremento del punto b. Indicar por qué entre
valor y valor existe un salto en la secuencia.
d) Si en lugar de tener un vector de enteros (int) en el ejemplo utilizáramos un vector de chars, el
salto entre valores consecutivos del punto c cambiaría? Corroborarlo modificando lo que
corresponda del código.
*/

#include <stdio.h>

int main() {
    int x[5] = {11, 22, 33, 44, 55};
    int *ptr;
    int i;
    
    ptr = x; // ptr apunta a x[0]

    printf("Recorriendo con punteros (int):\n");
    for (i = 0; i < 5; i++) {
        // %p imprime direcciones de memoria
        printf("Dirección: %p \t Valor: %d\n", ptr, *ptr);
        ptr++;
    }
    
    printf("\n El puntero salta de a %lu bytes (sizeof(int)).\n", sizeof(int));
    return 0;
}