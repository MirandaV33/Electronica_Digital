// Crear una función que realice la suma de dos enteros. Utilizarla para imprimir por pantalla la
// suma de dos enteros predefinidos.
 
#include <stdio.h>

// Definición de la función
int sumar(int a, int b) {
    return a + b;
}

// Cuerpo principal (provisto en la guía)
int main(){
    int suma; 
    int x = 1, y = 2;
    suma= sumar(x, y);
    printf("Cuenta: %d + %d = %d\n\n", x, y, suma);
    return 0;
}