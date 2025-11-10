// Crear un vector de enteros de 10 posiciones, recorrerlo e imprimir por pantalla cada uno de sus
// valores. ¿Qué tamaño ocupa en memoria?

#include <stdio.h>

int main() {
    int vector[10] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    int i;

    printf("Recorriendo el vector:\n");
    for (i = 0; i < 10; i++) {
        printf("Posición %d: Valor = %d\n", i, vector[i]);
    }

    size_t tamano_int = sizeof(int);
    size_t tamano_total = sizeof(vector);
    
    printf("\nTamaño en memoria\n");
    printf("Tamaño de 1 int: %lu bytes\n", tamano_int);
    printf("Tamaño total del vector (10 * %lu): %lu bytes\n", tamano_int, tamano_total);
    
    return 0;
}
 